

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  Sbr decoder
  This module provides the actual decoder implementation. The SBR data (side
  information) is already decoded. Only three functions are provided:

  \li 1.) createSbrDec(): One time initialization
  \li 2.) resetSbrDec(): Called by sbr_Apply() when the information contained in
  an SBR_HEADER_ELEMENT requires a reset and recalculation of important SBR
  structures. \li 3.) sbr_dec(): The actual decoder. Calls the different tools
  such as filterbanks, lppTransposer(), and calculateSbrEnvelope() [the envelope
  adjuster].

  \sa sbr_dec(), \ref documentationOverview
*/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "sbr_dec.h"

#include "sbr_ram.h"
#include "env_extr.h"
#include "env_calc.h"
#include "../libFDK/scale.h"
#include "../libFDK/FDK_matrixCalloc.h"
#include "hbe.h"


#include "sbrdec_drc.h"

static void copyHarmonicSpectrum(int32_t *xOverQmf, int32_t **qmfReal,
                                 int32_t **qmfImag, int32_t noCols, int32_t overlap,
                                 KEEP_STATES_SYNCED_MODE keepStatesSynced) {
  int32_t patchBands;
  int32_t patch, band, col, target, sourceBands, i;
  int32_t numPatches = 0;
  int32_t slotOffset = 0;

  int32_t **ppqmfReal = qmfReal + overlap;
  int32_t **ppqmfImag = qmfImag + overlap;

  if (keepStatesSynced == KEEP_STATES_SYNCED_NORMAL) {
    slotOffset = noCols - overlap - LPC_ORDER;
  }

  if (keepStatesSynced == KEEP_STATES_SYNCED_OUTDIFF) {
    ppqmfReal = qmfReal;
    ppqmfImag = qmfImag;
  }

  for (i = 1; i < MAX_NUM_PATCHES; i++) {
    if (xOverQmf[i] != 0) {
      numPatches++;
    }
  }

  for (patch = (MAX_STRETCH_HBE - 1); patch < numPatches; patch++) {
    patchBands = xOverQmf[patch + 1] - xOverQmf[patch];
    target = xOverQmf[patch];
    sourceBands = xOverQmf[MAX_STRETCH_HBE - 1] - xOverQmf[MAX_STRETCH_HBE - 2];

    while (patchBands > 0) {
      int32_t numBands = sourceBands;
      int32_t startBand = xOverQmf[MAX_STRETCH_HBE - 1] - 1;
      if (target + numBands >= xOverQmf[patch + 1]) {
        numBands = xOverQmf[patch + 1] - target;
      }
      if ((((target + numBands - 1) % 2) +
           ((xOverQmf[MAX_STRETCH_HBE - 1] - 1) % 2)) %
          2) {
        if (numBands == sourceBands) {
          numBands--;
        } else {
          startBand--;
        }
      }
      if (keepStatesSynced == KEEP_STATES_SYNCED_OUTDIFF) {
        for (col = slotOffset; col < overlap + LPC_ORDER; col++) {
          i = 0;
          for (band = numBands; band > 0; band--) {
            if ((target + band - 1 < 64) &&
                (target + band - 1 < xOverQmf[patch + 1])) {
              ppqmfReal[col][target + band - 1] = ppqmfReal[col][startBand - i];
              ppqmfImag[col][target + band - 1] = ppqmfImag[col][startBand - i];
              i++;
            }
          }
        }
      } else {
        for (col = slotOffset; col < noCols; col++) {
          i = 0;
          for (band = numBands; band > 0; band--) {
            if ((target + band - 1 < 64) &&
                (target + band - 1 < xOverQmf[patch + 1])) {
              ppqmfReal[col][target + band - 1] = ppqmfReal[col][startBand - i];
              ppqmfImag[col][target + band - 1] = ppqmfImag[col][startBand - i];
              i++;
            }
          }
        }
      }
      target += numBands;
      patchBands -= numBands;
    }
  }
}

/*!
  \brief      SBR decoder core function for one channel

  \image html  BufferMgmtDetailed-1632.png

  Besides the filter states of the QMF filter bank and the LPC-states of
  the LPP-Transposer, processing is mainly based on four buffers:
  #timeIn, #timeOut, #WorkBuffer2 and #OverlapBuffer. The #WorkBuffer2
  is reused for all channels and might be used by the core decoder, a
  static overlap buffer is required for each channel. Due to in-place
  processing, #timeIn and #timeOut point to identical locations.

  The spectral data is organized in so-called slots. Each slot
  contains 64 bands of complex data. The number of slots per frame
  depends on the frame size. For mp3PRO, there are 18 slots per frame
  and 6 slots per #OverlapBuffer. It is not necessary to have the slots
  in located consecutive address ranges.

  To optimize memory usage and to minimize the number of memory
  accesses, the memory management is organized as follows (slot numbers
  based on mp3PRO):

  1.) Input time domain signal is located in #timeIn. The last slots
  (0..5) of the spectral data of the previous frame are located in the
  #OverlapBuffer. In addition, #frameData of the current frame resides
  in the upper part of #timeIn.

  2.) During the cplxAnalysisQmfFiltering(), 32 samples from #timeIn are
  transformed into a slot of up to 32 complex spectral low band values at a
  time. The first spectral slot -- nr. 6 -- is written at slot number
  zero of #WorkBuffer2. #WorkBuffer2 will be completely filled with
  spectral data.

  3.) LPP-Transposition in lppTransposer() is processed on 24 slots. During the
  transposition, the high band part of the spectral data is replicated
  based on the low band data.

  Envelope Adjustment is processed on the high band part of the spectral
  data only by calculateSbrEnvelope().

  4.) The cplxSynthesisQmfFiltering() creates 64 time domain samples out
  of a slot of 64 complex spectral values at a time. The first 6 slots
  in #timeOut are filled from the results of spectral slots 0..5 in the
  #OverlapBuffer. The consecutive slots in timeOut are now filled with
  the results of spectral slots 6..17.

  5.) The preprocessed slots 18..23 have to be stored in the
  #OverlapBuffer.

*/

void sbr_dec(
    HANDLE_SBR_DEC hSbrDec,             /*!< handle to Decoder channel */
    int32_t *timeIn,                       /*!< pointer to input time signal */
    int32_t *timeOut,                      /*!< pointer to output time signal */
    HANDLE_SBR_DEC hSbrDecRight,        /*!< handle to Decoder channel right */
    int32_t *timeOutRight,                 /*!< pointer to output time signal */
    const int32_t strideOut,                /*!< Time data traversal strideOut */
    HANDLE_SBR_HEADER_DATA hHeaderData, /*!< Static control data */
    HANDLE_SBR_FRAME_DATA hFrameData,   /*!< Control data of current frame */
    HANDLE_SBR_PREV_FRAME_DATA
        hPrevFrameData,        /*!< Some control data of last frame */
    const int32_t applyProcessing, /*!< Flag for SBR operation */
    HANDLE_PS_DEC h_ps_d, const uint32_t flags, const int32_t codecFrameSize,
    const int32_t sbrInDataHeadroom) {
  int32_t i, slot, reserve;
  int32_t saveLbScale;
  int32_t lastSlotOffs;
  int32_t maxVal;

  /* temporary pointer / variable for QMF;
     required as we want to use temporary buffer
     creating one frame delay for HBE in LP mode */
  int32_t *pTimeInQmf = timeIn;

  /* Number of QMF timeslots in the overlap buffer: */
  int32_t ov_len = hSbrDec->LppTrans.pSettings->overlap;

  /* Number of QMF slots per frame */
  int32_t noCols = hHeaderData->numberTimeSlots * hHeaderData->timeStep;

  /* create pointer array for data to use for HBE and legacy sbr */
  int32_t *pLowBandReal[(3 * 4) + 2 * ((1024) / (32) * (4) / 2)];
  int32_t *pLowBandImag[(3 * 4) + 2 * ((1024) / (32) * (4) / 2)];

  /* set pReal to where QMF analysis writes in case of legacy SBR */
  int32_t **pReal = pLowBandReal + ov_len;
  int32_t **pImag = pLowBandImag + ov_len;

  /* map QMF buffer to pointer array (Overlap + Frame)*/
  for (i = 0; i < noCols + ov_len; i++) {
    pLowBandReal[i] = hSbrDec->qmfDomainInCh->hQmfSlotsReal[i];
    pLowBandImag[i] = hSbrDec->qmfDomainInCh->hQmfSlotsImag[i];
  }

  if ((flags & SBRDEC_USAC_HARMONICSBR)) {
    /* in case of harmonic SBR and no HBE_LP map additional buffer for
       one more frame to pointer arry */
    for (i = 0; i < noCols; i++) {
      pLowBandReal[i + noCols + ov_len] = hSbrDec->hQmfHBESlotsReal[i];
      pLowBandImag[i + noCols + ov_len] = hSbrDec->hQmfHBESlotsImag[i];
    }

    /* shift scale values according to buffer */
    hSbrDec->scale_ov = hSbrDec->scale_lb;
    hSbrDec->scale_lb = hSbrDec->scale_hbe;

    /* set pReal to where QMF analysis writes in case of HBE */
    pReal += noCols;
    pImag += noCols;
    if (flags & SBRDEC_SKIP_QMF_ANA) {
      /* stereoCfgIndex3 with HBE */
      FDK_QmfDomain_QmfData2HBE(hSbrDec->qmfDomainInCh,
                                hSbrDec->hQmfHBESlotsReal,
                                hSbrDec->hQmfHBESlotsImag);
    } else {
      /* We have to move old hbe frame data to lb area of buffer */
      for (i = 0; i < noCols; i++) {
        memcpy(pLowBandReal[ov_len + i], hSbrDec->hQmfHBESlotsReal[i],
                  hHeaderData->numberOfAnalysisBands * sizeof(int32_t));
        memcpy(pLowBandImag[ov_len + i], hSbrDec->hQmfHBESlotsImag[i],
                  hHeaderData->numberOfAnalysisBands * sizeof(int32_t));
      }
    }
  }

  /*
    low band codec signal subband filtering
   */

  if (flags & SBRDEC_SKIP_QMF_ANA) {
    if (!(flags & SBRDEC_USAC_HARMONICSBR)) /* stereoCfgIndex3 w/o HBE */
      FDK_QmfDomain_WorkBuffer2ProcChannel(hSbrDec->qmfDomainInCh);
  } else {
    int32_t _qmfTemp[(2 * 64) + (8 + sizeof(int32_t) - 1)];
    int32_t *qmfTemp = (int32_t *)(_qmfTemp + (((int32_t)8 - ((size_t)(_qmfTemp) & 7)) & 7));



    qmfAnalysisFiltering(&hSbrDec->qmfDomainInCh->fb, pReal, pImag,
                         &hSbrDec->qmfDomainInCh->scaling, pTimeInQmf,
                         0 + sbrInDataHeadroom, 1, qmfTemp);
  }

  /*
    Clear upper half of spectrum
  */
  if (!((flags & SBRDEC_USAC_HARMONICSBR) &&
        (hFrameData->sbrPatchingMode == 0))) {
    int32_t nAnalysisBands = hHeaderData->numberOfAnalysisBands;

    if (!(flags & SBRDEC_LOW_POWER)) {
      for (slot = ov_len; slot < noCols + ov_len; slot++) {
        memset(&pLowBandReal[slot][nAnalysisBands], 0, ((64) - nAnalysisBands) * sizeof(int32_t));
        memset(&pLowBandImag[slot][nAnalysisBands], 0, ((64) - nAnalysisBands) * sizeof(int32_t));
      }
    } else {
      for (slot = ov_len; slot < noCols + ov_len; slot++) {
        memset(&pLowBandReal[slot][nAnalysisBands], 0, ((64) - nAnalysisBands) * sizeof(int32_t));
      }
    }
  }

  /*
    Shift spectral data left to gain accuracy in transposer and adjustor
  */
  /* Range was increased from lsb to no_channels because in some cases (e.g.
     USAC conf eSbr_4_Pvc.mp4 and some HBE cases) it could be observed that the
     signal between lsb and no_channels is used for the patching process.
  */
  maxVal = maxSubbandSample(pReal, (flags & SBRDEC_LOW_POWER) ? NULL : pImag, 0,
                            hSbrDec->qmfDomainInCh->fb.no_channels, 0, noCols);

  reserve = fixMax(0, CntLeadingZeros(maxVal) - 1);
  reserve = fixMin(reserve,
                   DFRACT_BITS - 1 - hSbrDec->qmfDomainInCh->scaling.lb_scale);

  /* If all data is zero, lb_scale could become too large */
  rescaleSubbandSamples(pReal, (flags & SBRDEC_LOW_POWER) ? NULL : pImag, 0,
                        hSbrDec->qmfDomainInCh->fb.no_channels, 0, noCols,
                        reserve);

  hSbrDec->qmfDomainInCh->scaling.lb_scale += reserve;

  if ((flags & SBRDEC_USAC_HARMONICSBR)) {
    /* actually this is our hbe_scale */
    hSbrDec->scale_hbe = hSbrDec->qmfDomainInCh->scaling.lb_scale;
    /* the real lb_scale is stored in scale_lb from sbr */
    hSbrDec->qmfDomainInCh->scaling.lb_scale = hSbrDec->scale_lb;
  }
  /*
    save low band scale, wavecoding or parametric stereo may modify it
  */
  saveLbScale = hSbrDec->qmfDomainInCh->scaling.lb_scale;

  if (applyProcessing) {
    uint8_t *borders = hFrameData->frameInfo.borders;
    lastSlotOffs = borders[hFrameData->frameInfo.nEnvelopes] -
                   hHeaderData->numberTimeSlots;

    int32_t degreeAlias[(64)];
    PVC_DYNAMIC_DATA pvcDynamicData;
    pvcInitFrame(
        &hSbrDec->PvcStaticData, &pvcDynamicData,
        (hHeaderData->frameErrorFlag ? 0 : hHeaderData->bs_info.pvc_mode),
        hFrameData->ns, hHeaderData->timeStep,
        hHeaderData->freqBandData.lowSubband,
        hFrameData->frameInfo.pvcBorders[0], hFrameData->pvcID);

    if (!hHeaderData->frameErrorFlag && (hHeaderData->bs_info.pvc_mode > 0)) {
      pvcDecodeFrame(&hSbrDec->PvcStaticData, &pvcDynamicData, pLowBandReal,
                     pLowBandImag, ov_len,
                     SCALE2EXP(hSbrDec->qmfDomainInCh->scaling.ov_lb_scale),
                     SCALE2EXP(hSbrDec->qmfDomainInCh->scaling.lb_scale));
    }
    pvcEndFrame(&hSbrDec->PvcStaticData, &pvcDynamicData);

    /* The transposer will override most values in degreeAlias[].
       The array needs to be cleared at least from lowSubband to highSubband
       before. */
    if (flags & SBRDEC_LOW_POWER)
      memset(&degreeAlias[hHeaderData->freqBandData.lowSubband], 0,
                  (hHeaderData->freqBandData.highSubband -
                   hHeaderData->freqBandData.lowSubband) *sizeof(int32_t));

    /*
      Inverse filtering of lowband and transposition into the SBR-frequency
      range
    */

    {
      KEEP_STATES_SYNCED_MODE keepStatesSyncedMode =
          ((flags & SBRDEC_USAC_HARMONICSBR) &&
           (hFrameData->sbrPatchingMode != 0))
              ? KEEP_STATES_SYNCED_NORMAL
              : KEEP_STATES_SYNCED_OFF;

      if (flags & SBRDEC_USAC_HARMONICSBR) {
        if (flags & SBRDEC_QUAD_RATE) {
          pReal -= 32;
          pImag -= 32;
        }

        if ((hSbrDec->savedStates == 0) && (hFrameData->sbrPatchingMode == 1)) {
          /* copy saved states from previous frame to legacy SBR lpc filterstate
           * buffer   */
          for (i = 0; i < LPC_ORDER + ov_len; i++) {
            memcpy(
                hSbrDec->LppTrans.lpcFilterStatesRealLegSBR[i],
                hSbrDec->codecQMFBufferReal[noCols - LPC_ORDER - ov_len + i],
                hSbrDec->hHBE->noChannels * sizeof(int32_t));
            memcpy(
                hSbrDec->LppTrans.lpcFilterStatesImagLegSBR[i],
                hSbrDec->codecQMFBufferImag[noCols - LPC_ORDER - ov_len + i],
                hSbrDec->hHBE->noChannels * sizeof(int32_t));
          }
        }

        /* saving unmodified QMF states in case we are switching from legacy SBR
         * to HBE */
        for (i = 0; i < hSbrDec->hHBE->noCols; i++) {
          memcpy(hSbrDec->codecQMFBufferReal[i], pLowBandReal[ov_len + i],
                    hSbrDec->hHBE->noChannels * sizeof(int32_t));
          memcpy(hSbrDec->codecQMFBufferImag[i], pLowBandImag[ov_len + i],
                    hSbrDec->hHBE->noChannels * sizeof(int32_t));
        }

        QmfTransposerApply(
            hSbrDec->hHBE, pReal, pImag, noCols, pLowBandReal, pLowBandImag,
            hSbrDec->LppTrans.lpcFilterStatesRealHBE,
            hSbrDec->LppTrans.lpcFilterStatesImagHBE,
            hFrameData->sbrPitchInBins, hSbrDec->scale_lb, hSbrDec->scale_hbe,
            &hSbrDec->qmfDomainInCh->scaling.hb_scale, hHeaderData->timeStep,
            borders[0], ov_len, keepStatesSyncedMode);

        if (flags & SBRDEC_QUAD_RATE) {
          int32_t *xOverQmf = GetxOverBandQmfTransposer(hSbrDec->hHBE);

          copyHarmonicSpectrum(xOverQmf, pLowBandReal, pLowBandImag, noCols,
                               ov_len, keepStatesSyncedMode);
        }
      }
    }

    if ((flags & SBRDEC_USAC_HARMONICSBR) &&
        (hFrameData->sbrPatchingMode == 0)) {
      hSbrDec->prev_frame_lSbr = 0;
      hSbrDec->prev_frame_hbeSbr = 1;

      lppTransposerHBE(
          &hSbrDec->LppTrans, hSbrDec->hHBE, &hSbrDec->qmfDomainInCh->scaling,
          pLowBandReal, pLowBandImag, hHeaderData->timeStep, borders[0],
          lastSlotOffs, hHeaderData->freqBandData.nInvfBands,
          hFrameData->sbr_invf_mode, hPrevFrameData->sbr_invf_mode);

    } else {
      if (flags & SBRDEC_USAC_HARMONICSBR) {
        for (i = 0; i < LPC_ORDER + hSbrDec->LppTrans.pSettings->overlap; i++) {
          /*
          Store the unmodified qmf Slots values for upper part of spectrum
          (required for LPC filtering) required if next frame is a HBE frame
          */
          memcpy(hSbrDec->LppTrans.lpcFilterStatesRealHBE[i],
                    hSbrDec->qmfDomainInCh
                        ->hQmfSlotsReal[hSbrDec->hHBE->noCols - LPC_ORDER + i],
                    (64) * sizeof(int32_t));
          memcpy(hSbrDec->LppTrans.lpcFilterStatesImagHBE[i],
                    hSbrDec->qmfDomainInCh
                        ->hQmfSlotsImag[hSbrDec->hHBE->noCols - LPC_ORDER + i],
                    (64) * sizeof(int32_t));
        }
      }
      {
        hSbrDec->prev_frame_lSbr = 1;
        hSbrDec->prev_frame_hbeSbr = 0;
      }

      lppTransposer(
          &hSbrDec->LppTrans, &hSbrDec->qmfDomainInCh->scaling, pLowBandReal,
          degreeAlias,  // only used if useLP = 1
          pLowBandImag, flags & SBRDEC_LOW_POWER,
          hHeaderData->bs_info.sbr_preprocessing,
          hHeaderData->freqBandData.v_k_master[0], hHeaderData->timeStep,
          borders[0], lastSlotOffs, hHeaderData->freqBandData.nInvfBands,
          hFrameData->sbr_invf_mode, hPrevFrameData->sbr_invf_mode);
    }

    /*
      Adjust envelope of current frame.
    */

    if ((hFrameData->sbrPatchingMode !=
         hSbrDec->SbrCalculateEnvelope.sbrPatchingMode)) {
      ResetLimiterBands(hHeaderData->freqBandData.limiterBandTable,
                        &hHeaderData->freqBandData.noLimiterBands,
                        hHeaderData->freqBandData.freqBandTable[0],
                        hHeaderData->freqBandData.nSfb[0],
                        hSbrDec->LppTrans.pSettings->patchParam,
                        hSbrDec->LppTrans.pSettings->noOfPatches,
                        hHeaderData->bs_data.limiterBands,
                        hFrameData->sbrPatchingMode,
                        (flags & SBRDEC_USAC_HARMONICSBR) &&
                                (hFrameData->sbrPatchingMode == 0)
                            ? GetxOverBandQmfTransposer(hSbrDec->hHBE)
                            : NULL,
                        Get41SbrQmfTransposer(hSbrDec->hHBE));

      hSbrDec->SbrCalculateEnvelope.sbrPatchingMode =
          hFrameData->sbrPatchingMode;
    }

    calculateSbrEnvelope(
        &hSbrDec->qmfDomainInCh->scaling, &hSbrDec->SbrCalculateEnvelope,
        hHeaderData, hFrameData, &pvcDynamicData, pLowBandReal, pLowBandImag,
        flags & SBRDEC_LOW_POWER,

        degreeAlias, flags,
        (hHeaderData->frameErrorFlag || hPrevFrameData->frameErrorFlag));

#if (SBRDEC_MAX_HB_FADE_FRAMES > 0)
    /* Avoid hard onsets of high band */
    if (hHeaderData->frameErrorFlag) {
      if (hSbrDec->highBandFadeCnt < SBRDEC_MAX_HB_FADE_FRAMES) {
        hSbrDec->highBandFadeCnt += 1;
      }
    } else {
      if (hSbrDec->highBandFadeCnt >
          0) { /* Manipulate high band scale factor to get a smooth fade-in */
        hSbrDec->qmfDomainInCh->scaling.hb_scale += hSbrDec->highBandFadeCnt;
        hSbrDec->qmfDomainInCh->scaling.hb_scale =
            fMin(hSbrDec->qmfDomainInCh->scaling.hb_scale, DFRACT_BITS - 1);
        hSbrDec->highBandFadeCnt -= 1;
      }
    }

#endif
    /*
      Update hPrevFrameData (to be used in the next frame)
    */
    for (i = 0; i < hHeaderData->freqBandData.nInvfBands; i++) {
      hPrevFrameData->sbr_invf_mode[i] = hFrameData->sbr_invf_mode[i];
    }
    hPrevFrameData->coupling = hFrameData->coupling;
    hPrevFrameData->stopPos = borders[hFrameData->frameInfo.nEnvelopes];
    hPrevFrameData->ampRes = hFrameData->ampResolutionCurrentFrame;
    hPrevFrameData->prevSbrPitchInBins = hFrameData->sbrPitchInBins;
    /* could be done in extractFrameInfo_pvc() but hPrevFrameData is not
     * available there */
    memcpy(&hPrevFrameData->prevFrameInfo, &hFrameData->frameInfo,
              sizeof(FRAME_INFO));
  } else {
    /* rescale from lsb to nAnalysisBands in order to compensate scaling with
     * hb_scale in this area, done by synthesisFiltering*/
    int32_t rescale;
    int32_t lsb;
    int32_t length;

    /* Reset hb_scale if no highband is present, because hb_scale is considered
     * in the QMF-synthesis */
    hSbrDec->qmfDomainInCh->scaling.hb_scale = saveLbScale;

    rescale = hSbrDec->qmfDomainInCh->scaling.hb_scale -
              hSbrDec->qmfDomainInCh->scaling.ov_lb_scale;
    lsb = hSbrDec->qmfDomainOutCh->fb.lsb;
    length = (hSbrDec->qmfDomainInCh->fb.no_channels - lsb);

    if ((rescale < 0) && (length > 0)) {
      if (!(flags & SBRDEC_LOW_POWER)) {
        for (i = 0; i < ov_len; i++) {
          scaleValues(&pLowBandReal[i][lsb], length, rescale);
          scaleValues(&pLowBandImag[i][lsb], length, rescale);
        }
      } else {
        for (i = 0; i < ov_len; i++) {
          scaleValues(&pLowBandReal[i][lsb], length, rescale);
        }
      }
    }
  }

  if (!(flags & SBRDEC_USAC_HARMONICSBR)) {
    int32_t length = hSbrDec->qmfDomainInCh->fb.lsb;
    if (flags & SBRDEC_SYNTAX_USAC) {
      length = hSbrDec->qmfDomainInCh->fb.no_channels;
    }

    /* in case of legacy sbr saving of filter states here */
    for (i = 0; i < LPC_ORDER + ov_len; i++) {
      /*
        Store the unmodified qmf Slots values (required for LPC filtering)
      */
      if (!(flags & SBRDEC_LOW_POWER)) {
        memcpy(hSbrDec->LppTrans.lpcFilterStatesRealLegSBR[i],
                  pLowBandReal[noCols - LPC_ORDER + i],
                  length * sizeof(int32_t));
        memcpy(hSbrDec->LppTrans.lpcFilterStatesImagLegSBR[i],
                  pLowBandImag[noCols - LPC_ORDER + i],
                  length * sizeof(int32_t));
      } else
        memcpy(hSbrDec->LppTrans.lpcFilterStatesRealLegSBR[i],
                  pLowBandReal[noCols - LPC_ORDER + i],
                  length * sizeof(int32_t));
    }
  }

  /*
    Synthesis subband filtering.
  */

  if (!(flags & SBRDEC_PS_DECODED)) {
    if (!(flags & SBRDEC_SKIP_QMF_SYN)) {
      int32_t outScalefactor = -(8);

      if (h_ps_d != NULL) {
        h_ps_d->procFrameBased = 1; /* we here do frame based processing */
      }

      sbrDecoder_drcApply(&hSbrDec->sbrDrcChannel, pLowBandReal,
                          (flags & SBRDEC_LOW_POWER) ? NULL : pLowBandImag,
                          hSbrDec->qmfDomainOutCh->fb.no_col, &outScalefactor);

      qmfChangeOutScalefactor(&hSbrDec->qmfDomainOutCh->fb, outScalefactor);

      {
        HANDLE_FREQ_BAND_DATA hFreq = &hHeaderData->freqBandData;
        int32_t save_usb = hSbrDec->qmfDomainOutCh->fb.usb;

        int32_t _qmfTemp[(2 * 64) + (8 + sizeof(int32_t) - 1)];
        int32_t *qmfTemp = (int32_t *)(_qmfTemp + (((int32_t)8 - ((size_t)(_qmfTemp) & 7)) & 7));




        if (hSbrDec->qmfDomainOutCh->fb.usb < hFreq->ov_highSubband) {
          /* we need to patch usb for this frame as overlap may contain higher
             frequency range if headerchange occured; fb. usb is always limited
             to maximum fb.no_channels; In case of wrongly decoded headers it
             might be that ov_highSubband is higher than the number of synthesis
             channels (fb.no_channels), which is forbidden, therefore we need to
             limit ov_highSubband with fMin function to avoid not allowed usb in
             synthesis filterbank. */
          hSbrDec->qmfDomainOutCh->fb.usb =
              fMin((uint32_t)hFreq->ov_highSubband,
                   (uint32_t)hSbrDec->qmfDomainOutCh->fb.no_channels);
        }
        {
          qmfSynthesisFiltering(
              &hSbrDec->qmfDomainOutCh->fb, pLowBandReal,
              (flags & SBRDEC_LOW_POWER) ? NULL : pLowBandImag,
              &hSbrDec->qmfDomainInCh->scaling,
              hSbrDec->LppTrans.pSettings->overlap, timeOut, strideOut,
              qmfTemp);
        }
        /* restore saved value */
        hSbrDec->qmfDomainOutCh->fb.usb = save_usb;
        hFreq->ov_highSubband = save_usb;
      }
    }

  } else { /* (flags & SBRDEC_PS_DECODED) */
    int32_t sdiff;
    int32_t scaleFactorHighBand, scaleFactorLowBand_ov, scaleFactorLowBand_no_ov;

    HANDLE_QMF_FILTER_BANK synQmf = &hSbrDec->qmfDomainOutCh->fb;
    HANDLE_QMF_FILTER_BANK synQmfRight = &hSbrDecRight->qmfDomainOutCh->fb;

    /* adapt scaling */
    sdiff = hSbrDec->qmfDomainInCh->scaling.lb_scale -
            reserve; /* Scaling difference */
    scaleFactorHighBand = sdiff - hSbrDec->qmfDomainInCh->scaling.hb_scale;
    scaleFactorLowBand_ov = sdiff - hSbrDec->qmfDomainInCh->scaling.ov_lb_scale;
    scaleFactorLowBand_no_ov = sdiff - hSbrDec->qmfDomainInCh->scaling.lb_scale;

    /* Scale of low band overlapping QMF data */
    scaleFactorLowBand_ov =
        fMin(DFRACT_BITS - 1, fMax(-(DFRACT_BITS - 1), scaleFactorLowBand_ov));
    /* Scale of low band current QMF data     */
    scaleFactorLowBand_no_ov = fMin(
        DFRACT_BITS - 1, fMax(-(DFRACT_BITS - 1), scaleFactorLowBand_no_ov));
    /* Scale of current high band */
    scaleFactorHighBand =
        fMin(DFRACT_BITS - 1, fMax(-(DFRACT_BITS - 1), scaleFactorHighBand));

    if (h_ps_d->procFrameBased == 1) /* If we have switched from frame to slot
                                        based processing copy filter states */
    {                                /* procFrameBased will be unset later */
      /* copy filter states from left to right */
      /* was ((640)-(64))*sizeof(FIXP_QSS)
         flexible amount of synthesis bands needed for QMF based resampling
      */
      assert(hSbrDec->qmfDomainInCh->pGlobalConf->nBandsSynthesis <=
                 QMF_MAX_SYNTHESIS_BANDS);
      qmfChangeOutScalefactor(synQmfRight, -(8));
      memcpy(synQmfRight->FilterStates, synQmf->FilterStates,
                9 * hSbrDec->qmfDomainInCh->pGlobalConf->nBandsSynthesis *
                    sizeof(FIXP_QSS));
    }

    /* Feed delaylines when parametric stereo is switched on. */
    PreparePsProcessing(h_ps_d, pLowBandReal, pLowBandImag,
                        scaleFactorLowBand_ov);

    /* use the same synthese qmf values for left and right channel */
    synQmfRight->no_col = synQmf->no_col;
    synQmfRight->lsb = synQmf->lsb;
    synQmfRight->usb = synQmf->usb;

    int32_t env = 0;

    {
      int32_t _pWorkBuffer[(2 * 64) + (8 + sizeof(int32_t) - 1)];
      int32_t *pWorkBuffer = (int32_t *)(_pWorkBuffer + (((int32_t)8 - ((size_t)(_pWorkBuffer) & 7)) & 7));


      int32_t maxShift = 0;

      if (hSbrDec->sbrDrcChannel.enable != 0) {
        if (hSbrDec->sbrDrcChannel.prevFact_exp > maxShift) {
          maxShift = hSbrDec->sbrDrcChannel.prevFact_exp;
        }
        if (hSbrDec->sbrDrcChannel.currFact_exp > maxShift) {
          maxShift = hSbrDec->sbrDrcChannel.currFact_exp;
        }
        if (hSbrDec->sbrDrcChannel.nextFact_exp > maxShift) {
          maxShift = hSbrDec->sbrDrcChannel.nextFact_exp;
        }
      }

      /* copy DRC data to right channel (with PS both channels use the same DRC
       * gains) */
      memcpy(&hSbrDecRight->sbrDrcChannel, &hSbrDec->sbrDrcChannel,
                sizeof(SBRDEC_DRC_CHANNEL));

      for (i = 0; i < synQmf->no_col; i++) { /* ----- no_col loop ----- */

        int32_t outScalefactorR, outScalefactorL;

        /* qmf timeslot of right channel */
        int32_t *rQmfReal = pWorkBuffer;
        int32_t *rQmfImag = pWorkBuffer + synQmf->no_channels;

        {
          if (i ==
              h_ps_d->bsData[h_ps_d->processSlot].mpeg.aEnvStartStop[env]) {
            initSlotBasedRotation(h_ps_d, env,
                                  hHeaderData->freqBandData.highSubband);
            env++;
          }

          ApplyPsSlot(
              h_ps_d,             /* parametric stereo decoder handle  */
              (pLowBandReal + i), /* one timeslot of left/mono channel */
              (pLowBandImag + i), /* one timeslot of left/mono channel */
              rQmfReal,           /* one timeslot or right channel     */
              rQmfImag,           /* one timeslot or right channel     */
              scaleFactorLowBand_no_ov,
              (i < hSbrDec->LppTrans.pSettings->overlap)
                  ? scaleFactorLowBand_ov
                  : scaleFactorLowBand_no_ov,
              scaleFactorHighBand, synQmf->lsb, synQmf->usb);

          outScalefactorL = outScalefactorR =
              1 + sbrInDataHeadroom; /* psDiffScale! (MPEG-PS) */
        }

        sbrDecoder_drcApplySlot(/* right channel */
                                &hSbrDecRight->sbrDrcChannel, rQmfReal,
                                rQmfImag, i, synQmfRight->no_col, maxShift);

        outScalefactorR += maxShift;

        sbrDecoder_drcApplySlot(/* left channel */
                                &hSbrDec->sbrDrcChannel, *(pLowBandReal + i),
                                *(pLowBandImag + i), i, synQmf->no_col,
                                maxShift);

        outScalefactorL += maxShift;

        if (!(flags & SBRDEC_SKIP_QMF_SYN)) {
          qmfChangeOutScalefactor(synQmf, -(8));
          qmfChangeOutScalefactor(synQmfRight, -(8));

          qmfSynthesisFilteringSlot(
              synQmfRight, rQmfReal, /* QMF real buffer */
              rQmfImag,              /* QMF imag buffer */
              outScalefactorL, outScalefactorL,
              timeOutRight + (i * synQmf->no_channels * strideOut), strideOut,
              pWorkBuffer);

          qmfSynthesisFilteringSlot(
              synQmf, *(pLowBandReal + i), /* QMF real buffer */
              *(pLowBandImag + i),         /* QMF imag buffer */
              outScalefactorR, outScalefactorR,
              timeOut + (i * synQmf->no_channels * strideOut), strideOut,
              pWorkBuffer);
        }
      } /* no_col loop  i  */
    }
  }

  sbrDecoder_drcUpdateChannel(&hSbrDec->sbrDrcChannel);

  /*
    Update overlap buffer
    Even bands above usb are copied to avoid outdated spectral data in case
    the stop frequency raises.
  */

  if (!(flags & SBRDEC_SKIP_QMF_SYN)) {
    {
      FDK_QmfDomain_SaveOverlap(hSbrDec->qmfDomainInCh, 0);
      assert(hSbrDec->qmfDomainInCh->scaling.ov_lb_scale == saveLbScale);
    }
  }

  hSbrDec->savedStates = 0;

  /* Save current frame status */
  hPrevFrameData->frameErrorFlag = hHeaderData->frameErrorFlag;
  hSbrDec->applySbrProc_old = applyProcessing;

} /* sbr_dec() */

/*!
  \brief     Creates sbr decoder structure
  \return    errorCode, 0 if successful
*/
SBR_ERROR
createSbrDec(SBR_CHANNEL *hSbrChannel,
             HANDLE_SBR_HEADER_DATA hHeaderData, /*!< Static control data */
             TRANSPOSER_SETTINGS *pSettings,
             const int32_t downsampleFac, /*!< Downsampling factor */
             const uint32_t qmfFlags, /*!< flags -> 1: HQ/LP selector, 2: CLDFB */
             const uint32_t flags, const int32_t overlap,
             int32_t chan, /*!< Channel for which to assign buffers etc. */
             int32_t codecFrameSize)

{
  SBR_ERROR err = SBRDEC_OK;
  int32_t timeSlots =
      hHeaderData->numberTimeSlots; /* Number of SBR slots per frame */
  int32_t noCols =
      timeSlots * hHeaderData->timeStep; /* Number of QMF slots per frame */
  HANDLE_SBR_DEC hs = &(hSbrChannel->SbrDec);

#if (SBRDEC_MAX_HB_FADE_FRAMES > 0)
  hs->highBandFadeCnt = SBRDEC_MAX_HB_FADE_FRAMES;

#endif
  hs->scale_hbe = 15;
  hs->scale_lb = 15;
  hs->scale_ov = 15;

  hs->prev_frame_lSbr = 0;
  hs->prev_frame_hbeSbr = 0;

  hs->codecFrameSize = codecFrameSize;

  /*
    create envelope calculator
  */
  err = createSbrEnvelopeCalc(&hs->SbrCalculateEnvelope, hHeaderData, chan,
                              flags);
  if (err != SBRDEC_OK) {
    return err;
  }

  initSbrPrevFrameData(&hSbrChannel->prevFrameData, timeSlots);

  /*
    create transposer
  */
  err = createLppTransposer(
      &hs->LppTrans, pSettings, hHeaderData->freqBandData.lowSubband,
      hHeaderData->freqBandData.v_k_master, hHeaderData->freqBandData.numMaster,
      hHeaderData->freqBandData.highSubband, timeSlots, noCols,
      hHeaderData->freqBandData.freqBandTableNoise,
      hHeaderData->freqBandData.nNfb, hHeaderData->sbrProcSmplRate, chan,
      overlap);
  if (err != SBRDEC_OK) {
    return err;
  }

  if (flags & SBRDEC_USAC_HARMONICSBR) {
    int32_t noChannels, bSbr41 = flags & SBRDEC_QUAD_RATE ? 1 : 0;

    noChannels =
        QMF_SYNTH_CHANNELS /
        ((bSbr41 + 1) * 2); /* 32 for (32:64 and 24:64) and 16 for 16:64 */

    /* shared memory between hbeLightTimeDelayBuffer and hQmfHBESlotsReal if
     * SBRDEC_HBE_ENABLE */
    hSbrChannel->SbrDec.tmp_memory = (int32_t **)fdkCallocMatrix2D_aligned(
        noCols, noChannels, sizeof(int32_t));
    if (hSbrChannel->SbrDec.tmp_memory == NULL) {
      return SBRDEC_MEM_ALLOC_FAILED;
    }

    hSbrChannel->SbrDec.hQmfHBESlotsReal = hSbrChannel->SbrDec.tmp_memory;
    hSbrChannel->SbrDec.hQmfHBESlotsImag =
        (int32_t **)fdkCallocMatrix2D_aligned(noCols, noChannels,
                                               sizeof(int32_t));
    if (hSbrChannel->SbrDec.hQmfHBESlotsImag == NULL) {
      return SBRDEC_MEM_ALLOC_FAILED;
    }

    /* buffers containing unmodified qmf data; required when switching from
     * legacy SBR to HBE                       */
    /* buffer can be used as LPCFilterstates buffer because legacy SBR needs
     * exactly these values for LPC filtering */
    hSbrChannel->SbrDec.codecQMFBufferReal =
        (int32_t **)fdkCallocMatrix2D_aligned(noCols, noChannels,
                                               sizeof(int32_t));
    if (hSbrChannel->SbrDec.codecQMFBufferReal == NULL) {
      return SBRDEC_MEM_ALLOC_FAILED;
    }

    hSbrChannel->SbrDec.codecQMFBufferImag =
        (int32_t **)fdkCallocMatrix2D_aligned(noCols, noChannels,
                                               sizeof(int32_t));
    if (hSbrChannel->SbrDec.codecQMFBufferImag == NULL) {
      return SBRDEC_MEM_ALLOC_FAILED;
    }

    err = QmfTransposerCreate(&hs->hHBE, codecFrameSize, 0, bSbr41);
    if (err != SBRDEC_OK) {
      return err;
    }
  }

  return err;
}

/*!
  \brief     Delete sbr decoder structure
  \return    errorCode, 0 if successful
*/
int32_t deleteSbrDec(SBR_CHANNEL *hSbrChannel) {
  HANDLE_SBR_DEC hs = &hSbrChannel->SbrDec;

  deleteSbrEnvelopeCalc(&hs->SbrCalculateEnvelope);

  if (hs->tmp_memory != NULL) {
    FDK_FREE_MEMORY_2D_ALIGNED(hs->tmp_memory);
  }

  /* modify here */
  FDK_FREE_MEMORY_2D_ALIGNED(hs->hQmfHBESlotsImag);

  if (hs->hHBE != NULL) QmfTransposerClose(hs->hHBE);

  if (hs->codecQMFBufferReal != NULL) {
    FDK_FREE_MEMORY_2D_ALIGNED(hs->codecQMFBufferReal);
  }

  if (hs->codecQMFBufferImag != NULL) {
    FDK_FREE_MEMORY_2D_ALIGNED(hs->codecQMFBufferImag);
  }

  return 0;
}

/*!
  \brief     resets sbr decoder structure
  \return    errorCode, 0 if successful
*/
SBR_ERROR
resetSbrDec(HANDLE_SBR_DEC hSbrDec, HANDLE_SBR_HEADER_DATA hHeaderData,
            HANDLE_SBR_PREV_FRAME_DATA hPrevFrameData, const int32_t downsampleFac,
            const uint32_t flags, HANDLE_SBR_FRAME_DATA hFrameData) {
  SBR_ERROR sbrError = SBRDEC_OK;
  int32_t i;
  int32_t *pLowBandReal[128];
  int32_t *pLowBandImag[128];
  int32_t useLP = flags & SBRDEC_LOW_POWER;

  int32_t old_lsb = hSbrDec->qmfDomainInCh->fb.lsb;
  int32_t old_usb = hSbrDec->qmfDomainInCh->fb.usb;
  int32_t new_lsb = hHeaderData->freqBandData.lowSubband;
  /* int32_t new_usb = hHeaderData->freqBandData.highSubband; */
  int32_t l, startBand, stopBand, startSlot, size;

  int32_t **OverlapBufferReal = hSbrDec->qmfDomainInCh->hQmfSlotsReal;
  int32_t **OverlapBufferImag = hSbrDec->qmfDomainInCh->hQmfSlotsImag;

  /* in case the previous frame was not active in terms of SBR processing, the
     full band from 0 to no_channels was rescaled and not overwritten. Thats why
     the scaling factor lb_scale can be seen as assigned to all bands from 0 to
     no_channels in the previous frame. The same states for the current frame if
     the current frame is not active in terms of SBR processing
  */
  int32_t applySbrProc = (hHeaderData->syncState == SBR_ACTIVE ||
                      (hHeaderData->frameErrorFlag == 0 &&
                       hHeaderData->syncState == SBR_HEADER));
  int32_t applySbrProc_old = hSbrDec->applySbrProc_old;

  if (!applySbrProc) {
    new_lsb = (hSbrDec->qmfDomainInCh->fb).no_channels;
  }
  if (!applySbrProc_old) {
    old_lsb = (hSbrDec->qmfDomainInCh->fb).no_channels;
    old_usb = old_lsb;
  }

  resetSbrEnvelopeCalc(&hSbrDec->SbrCalculateEnvelope);

  /* Change lsb and usb */
  /* Synthesis */
  assert(hSbrDec->qmfDomainOutCh != NULL);
  hSbrDec->qmfDomainOutCh->fb.lsb =
      fixMin((int32_t)hSbrDec->qmfDomainOutCh->fb.no_channels,
             (int32_t)hHeaderData->freqBandData.lowSubband);
  hSbrDec->qmfDomainOutCh->fb.usb =
      fixMin((int32_t)hSbrDec->qmfDomainOutCh->fb.no_channels,
             (int32_t)hHeaderData->freqBandData.highSubband);
  /* Analysis */
  assert(hSbrDec->qmfDomainInCh != NULL);
  hSbrDec->qmfDomainInCh->fb.lsb = hSbrDec->qmfDomainOutCh->fb.lsb;
  hSbrDec->qmfDomainInCh->fb.usb = hSbrDec->qmfDomainOutCh->fb.usb;

  /*
    The following initialization of spectral data in the overlap buffer
    is required for dynamic x-over or a change of the start-freq for 2 reasons:

    1. If the lowband gets _wider_, unadjusted data would remain

    2. If the lowband becomes _smaller_, the highest bands of the old lowband
       must be cleared because the whitening would be affected
  */
  startBand = old_lsb;
  stopBand = new_lsb;
  startSlot = fMax(0, hHeaderData->timeStep * (hPrevFrameData->stopPos -
                                               hHeaderData->numberTimeSlots));
  size = fMax(0, stopBand - startBand);

  /* in case of USAC we don't want to zero out the memory, as this can lead to
     holes in the spectrum; fix shall only be applied for USAC not for MPEG-4
     SBR, in this case setting zero remains         */
  if (!(flags & SBRDEC_SYNTAX_USAC)) {
    /* keep already adjusted data in the x-over-area */
    if (!useLP) {
      for (l = startSlot; l < hSbrDec->LppTrans.pSettings->overlap; l++) {
        memset(&OverlapBufferReal[l][startBand], 0, size * sizeof(int32_t));
        memset(&OverlapBufferImag[l][startBand], 0, size * sizeof(int32_t));
      }
    } else {
      for (l = startSlot; l < hSbrDec->LppTrans.pSettings->overlap; l++) {
        memset(&OverlapBufferReal[l][startBand], 0, size * sizeof(int32_t));
      }
    }

    /*
    reset LPC filter states
    */
    startBand = fixMin(old_lsb, new_lsb);
    stopBand = fixMax(old_lsb, new_lsb);
    size = fixMax(0, stopBand - startBand);

    memset(&hSbrDec->LppTrans.lpcFilterStatesRealLegSBR[0][startBand], 0,
                size * sizeof(int32_t));
    memset(&hSbrDec->LppTrans.lpcFilterStatesRealLegSBR[1][startBand], 0,
                size * sizeof(int32_t));
    if (!useLP) {
      memset(&hSbrDec->LppTrans.lpcFilterStatesImagLegSBR[0][startBand], 0,
                  size * sizeof(int32_t));
      memset(&hSbrDec->LppTrans.lpcFilterStatesImagLegSBR[1][startBand], 0,
                  size * sizeof(int32_t));
    }
  }

  if (startSlot != 0) {
    int32_t source_exp, target_exp, delta_exp, target_lsb, target_usb, reserve;
    int32_t maxVal;

    /*
    Rescale already processed spectral data between old and new x-over
    frequency. This must be done because of the separate scalefactors for
    lowband and highband.
    */

    /* We have four relevant transitions to cover:
    1. old_usb is lower than new_lsb; old SBR area is completely below new SBR
    area.
       -> entire old area was highband and belongs to lowband now
          and has to be rescaled.
    2. old_lsb is higher than new_usb; new SBR area is completely below old SBR
    area.
       -> old area between new_lsb and old_lsb was lowband and belongs to
    highband now and has to be rescaled to match new highband scale.
    3. old_lsb is lower and old_usb is higher than new_lsb; old and new SBR
    areas overlap.
       -> old area between old_lsb and new_lsb was highband and belongs to
    lowband now and has to be rescaled to match new lowband scale.
    4. new_lsb is lower and new_usb_is higher than old_lsb; old and new SBR
    areas overlap.
       -> old area between new_lsb and old_usb was lowband and belongs to
    highband now and has to be rescaled to match new highband scale.
    */

    if (new_lsb > old_lsb) {
      /* case 1 and 3 */
      source_exp = SCALE2EXP(hSbrDec->qmfDomainInCh->scaling.ov_hb_scale);
      target_exp = SCALE2EXP(hSbrDec->qmfDomainInCh->scaling.ov_lb_scale);

      startBand = old_lsb;

      if (new_lsb >= old_usb) {
        /* case 1 */
        stopBand = old_usb;
      } else {
        /* case 3 */
        stopBand = new_lsb;
      }

      target_lsb = 0;
      target_usb = old_lsb;
    } else {
      /* case 2 and 4 */
      source_exp = SCALE2EXP(hSbrDec->qmfDomainInCh->scaling.ov_lb_scale);
      target_exp = SCALE2EXP(hSbrDec->qmfDomainInCh->scaling.ov_hb_scale);

      startBand = new_lsb;
      stopBand = old_lsb;

      target_lsb = old_lsb;
      target_usb = old_usb;
    }

    maxVal =
        maxSubbandSample(OverlapBufferReal, (useLP) ? NULL : OverlapBufferImag,
                         startBand, stopBand, 0, startSlot);

    reserve = ((int32_t)maxVal != 0 ? CntLeadingZeros(maxVal) - 1 : 0);
    reserve = fixMin(
        reserve,
        DFRACT_BITS - 1 -
            EXP2SCALE(
                source_exp)); /* what is this line for, why do we need it? */

    /* process only if x-over-area is not dominant after rescale;
       otherwise I'm not sure if all buffers are scaled correctly;
    */
    if (target_exp - (source_exp - reserve) >= 0) {
      rescaleSubbandSamples(OverlapBufferReal,
                            (useLP) ? NULL : OverlapBufferImag, startBand,
                            stopBand, 0, startSlot, reserve);
      source_exp -= reserve;
    }

    delta_exp = target_exp - source_exp;

    if (delta_exp < 0) { /* x-over-area is dominant */
      startBand = target_lsb;
      stopBand = target_usb;
      delta_exp = -delta_exp;

      if (new_lsb > old_lsb) {
        /* The lowband has to be rescaled */
        hSbrDec->qmfDomainInCh->scaling.ov_lb_scale = EXP2SCALE(source_exp);
      } else {
        /* The highband has to be rescaled */
        hSbrDec->qmfDomainInCh->scaling.ov_hb_scale = EXP2SCALE(source_exp);
      }
    }

    assert(startBand <= stopBand);

    if (!useLP) {
      for (l = 0; l < startSlot; l++) {
        scaleValues(OverlapBufferReal[l] + startBand, stopBand - startBand,
                    -delta_exp);
        scaleValues(OverlapBufferImag[l] + startBand, stopBand - startBand,
                    -delta_exp);
      }
    } else
      for (l = 0; l < startSlot; l++) {
        scaleValues(OverlapBufferReal[l] + startBand, stopBand - startBand,
                    -delta_exp);
      }
  } /* startSlot != 0 */

  /*
    Initialize transposer and limiter
  */
  sbrError = resetLppTransposer(
      &hSbrDec->LppTrans, hHeaderData->freqBandData.lowSubband,
      hHeaderData->freqBandData.v_k_master, hHeaderData->freqBandData.numMaster,
      hHeaderData->freqBandData.freqBandTableNoise,
      hHeaderData->freqBandData.nNfb, hHeaderData->freqBandData.highSubband,
      hHeaderData->sbrProcSmplRate);
  if (sbrError != SBRDEC_OK) return sbrError;

  hSbrDec->savedStates = 0;

  if ((flags & SBRDEC_USAC_HARMONICSBR) && applySbrProc) {
    sbrError = QmfTransposerReInit(hSbrDec->hHBE,
                                   hHeaderData->freqBandData.freqBandTable,
                                   hHeaderData->freqBandData.nSfb);
    if (sbrError != SBRDEC_OK) return sbrError;

    /* copy saved states from previous frame to legacy SBR lpc filterstate
     * buffer   */
    for (i = 0; i < LPC_ORDER + hSbrDec->LppTrans.pSettings->overlap; i++) {
      memcpy(
          hSbrDec->LppTrans.lpcFilterStatesRealLegSBR[i],
          hSbrDec->codecQMFBufferReal[hSbrDec->hHBE->noCols - LPC_ORDER -
                                      hSbrDec->LppTrans.pSettings->overlap + i],
          hSbrDec->hHBE->noChannels * sizeof(int32_t));
      memcpy(
          hSbrDec->LppTrans.lpcFilterStatesImagLegSBR[i],
          hSbrDec->codecQMFBufferImag[hSbrDec->hHBE->noCols - LPC_ORDER -
                                      hSbrDec->LppTrans.pSettings->overlap + i],
          hSbrDec->hHBE->noChannels * sizeof(int32_t));
    }
    hSbrDec->savedStates = 1;

    {
      /* map QMF buffer to pointer array (Overlap + Frame)*/
      for (i = 0; i < hSbrDec->LppTrans.pSettings->overlap + LPC_ORDER; i++) {
        pLowBandReal[i] = hSbrDec->LppTrans.lpcFilterStatesRealHBE[i];
        pLowBandImag[i] = hSbrDec->LppTrans.lpcFilterStatesImagHBE[i];
      }

      /* map QMF buffer to pointer array (Overlap + Frame)*/
      for (i = 0; i < hSbrDec->hHBE->noCols; i++) {
        pLowBandReal[i + hSbrDec->LppTrans.pSettings->overlap + LPC_ORDER] =
            hSbrDec->codecQMFBufferReal[i];
        pLowBandImag[i + hSbrDec->LppTrans.pSettings->overlap + LPC_ORDER] =
            hSbrDec->codecQMFBufferImag[i];
      }

      if (flags & SBRDEC_QUAD_RATE) {
        if (hFrameData->sbrPatchingMode == 0) {
          int32_t *xOverQmf = GetxOverBandQmfTransposer(hSbrDec->hHBE);

          /* in case of harmonic SBR and no HBE_LP map additional buffer for
          one more frame to pointer arry */
          for (i = 0; i < hSbrDec->hHBE->noCols / 2; i++) {
            pLowBandReal[i + hSbrDec->hHBE->noCols +
                         hSbrDec->LppTrans.pSettings->overlap + LPC_ORDER] =
                hSbrDec->hQmfHBESlotsReal[i];
            pLowBandImag[i + hSbrDec->hHBE->noCols +
                         hSbrDec->LppTrans.pSettings->overlap + LPC_ORDER] =
                hSbrDec->hQmfHBESlotsImag[i];
          }

          QmfTransposerApply(
              hSbrDec->hHBE,
              pLowBandReal + hSbrDec->LppTrans.pSettings->overlap +
                  hSbrDec->hHBE->noCols / 2 + LPC_ORDER,
              pLowBandImag + hSbrDec->LppTrans.pSettings->overlap +
                  hSbrDec->hHBE->noCols / 2 + LPC_ORDER,
              hSbrDec->hHBE->noCols, pLowBandReal, pLowBandImag,
              hSbrDec->LppTrans.lpcFilterStatesRealHBE,
              hSbrDec->LppTrans.lpcFilterStatesImagHBE,
              hPrevFrameData->prevSbrPitchInBins, hSbrDec->scale_lb,
              hSbrDec->scale_hbe, &hSbrDec->qmfDomainInCh->scaling.hb_scale,
              hHeaderData->timeStep, hFrameData->frameInfo.borders[0],
              hSbrDec->LppTrans.pSettings->overlap, KEEP_STATES_SYNCED_OUTDIFF);

          copyHarmonicSpectrum(
              xOverQmf, pLowBandReal, pLowBandImag, hSbrDec->hHBE->noCols,
              hSbrDec->LppTrans.pSettings->overlap, KEEP_STATES_SYNCED_OUTDIFF);
        }
      } else {
        /* in case of harmonic SBR and no HBE_LP map additional buffer for
        one more frame to pointer arry */
        for (i = 0; i < hSbrDec->hHBE->noCols; i++) {
          pLowBandReal[i + hSbrDec->hHBE->noCols +
                       hSbrDec->LppTrans.pSettings->overlap + LPC_ORDER] =
              hSbrDec->hQmfHBESlotsReal[i];
          pLowBandImag[i + hSbrDec->hHBE->noCols +
                       hSbrDec->LppTrans.pSettings->overlap + LPC_ORDER] =
              hSbrDec->hQmfHBESlotsImag[i];
        }

        if (hFrameData->sbrPatchingMode == 0) {
          QmfTransposerApply(
              hSbrDec->hHBE,
              pLowBandReal + hSbrDec->LppTrans.pSettings->overlap + LPC_ORDER,
              pLowBandImag + hSbrDec->LppTrans.pSettings->overlap + LPC_ORDER,
              hSbrDec->hHBE->noCols, pLowBandReal, pLowBandImag,
              hSbrDec->LppTrans.lpcFilterStatesRealHBE,
              hSbrDec->LppTrans.lpcFilterStatesImagHBE,
              0 /* not required for keeping states updated in this frame*/,
              hSbrDec->scale_lb, hSbrDec->scale_lb,
              &hSbrDec->qmfDomainInCh->scaling.hb_scale, hHeaderData->timeStep,
              hFrameData->frameInfo.borders[0],
              hSbrDec->LppTrans.pSettings->overlap, KEEP_STATES_SYNCED_NOOUT);
        }

        QmfTransposerApply(
            hSbrDec->hHBE,
            pLowBandReal + hSbrDec->LppTrans.pSettings->overlap +
                hSbrDec->hHBE->noCols + LPC_ORDER,
            pLowBandImag + hSbrDec->LppTrans.pSettings->overlap +
                hSbrDec->hHBE->noCols + LPC_ORDER,
            hSbrDec->hHBE->noCols, pLowBandReal, pLowBandImag,
            hSbrDec->LppTrans.lpcFilterStatesRealHBE,
            hSbrDec->LppTrans.lpcFilterStatesImagHBE,
            hPrevFrameData->prevSbrPitchInBins, hSbrDec->scale_lb,
            hSbrDec->scale_hbe, &hSbrDec->qmfDomainInCh->scaling.hb_scale,
            hHeaderData->timeStep, hFrameData->frameInfo.borders[0],
            hSbrDec->LppTrans.pSettings->overlap, KEEP_STATES_SYNCED_OUTDIFF);
      }

      if (hFrameData->sbrPatchingMode == 0) {
        for (i = startSlot; i < hSbrDec->LppTrans.pSettings->overlap; i++) {
          /*
          Store the unmodified qmf Slots values for upper part of spectrum
          (required for LPC filtering) required if next frame is a HBE frame
          */
          memcpy(hSbrDec->qmfDomainInCh->hQmfSlotsReal[i],
                    hSbrDec->LppTrans.lpcFilterStatesRealHBE[i + LPC_ORDER],
                    (64) * sizeof(int32_t));
          memcpy(hSbrDec->qmfDomainInCh->hQmfSlotsImag[i],
                    hSbrDec->LppTrans.lpcFilterStatesImagHBE[i + LPC_ORDER],
                    (64) * sizeof(int32_t));
        }

        for (i = startSlot; i < hSbrDec->LppTrans.pSettings->overlap; i++) {
          /*
          Store the unmodified qmf Slots values for upper part of spectrum
          (required for LPC filtering) required if next frame is a HBE frame
          */
          memcpy(
              hSbrDec->qmfDomainInCh->hQmfSlotsReal[i],
              hSbrDec->codecQMFBufferReal[hSbrDec->hHBE->noCols -
                                          hSbrDec->LppTrans.pSettings->overlap +
                                          i],
              new_lsb * sizeof(int32_t));
          memcpy(
              hSbrDec->qmfDomainInCh->hQmfSlotsImag[i],
              hSbrDec->codecQMFBufferImag[hSbrDec->hHBE->noCols -
                                          hSbrDec->LppTrans.pSettings->overlap +
                                          i],
              new_lsb * sizeof(int32_t));
        }
      }
    }
  }

  {
    int32_t adapt_lb = 0, diff = 0,
        new_scale = hSbrDec->qmfDomainInCh->scaling.ov_lb_scale;

    if ((hSbrDec->qmfDomainInCh->scaling.ov_lb_scale !=
         hSbrDec->qmfDomainInCh->scaling.lb_scale) &&
        startSlot != 0) {
      /* we need to adapt spectrum to have equal scale factor, always larger
       * than zero */
      diff = SCALE2EXP(hSbrDec->qmfDomainInCh->scaling.ov_lb_scale) -
             SCALE2EXP(hSbrDec->qmfDomainInCh->scaling.lb_scale);

      if (diff > 0) {
        adapt_lb = 1;
        diff = -diff;
        new_scale = hSbrDec->qmfDomainInCh->scaling.ov_lb_scale;
      }

      stopBand = new_lsb;
    }

    if (hFrameData->sbrPatchingMode == 1) {
      /* scale states from LegSBR filterstates buffer */
      for (i = 0; i < hSbrDec->LppTrans.pSettings->overlap + LPC_ORDER; i++) {
        scaleValues(hSbrDec->LppTrans.lpcFilterStatesRealLegSBR[i], new_lsb,
                    diff);
        if (!useLP) {
          scaleValues(hSbrDec->LppTrans.lpcFilterStatesImagLegSBR[i], new_lsb,
                      diff);
        }
      }

      if (flags & SBRDEC_SYNTAX_USAC) {
        /* get missing states between old and new x_over from LegSBR
         * filterstates buffer */
        /* in case of legacy SBR we leave these values zeroed out */
        for (i = startSlot; i < hSbrDec->LppTrans.pSettings->overlap; i++) {
          memcpy(&OverlapBufferReal[i][old_lsb],
                    &hSbrDec->LppTrans
                         .lpcFilterStatesRealLegSBR[LPC_ORDER + i][old_lsb],
                    fMax(new_lsb - old_lsb, 0) * sizeof(int32_t));
          if (!useLP) {
            memcpy(&OverlapBufferImag[i][old_lsb],
                      &hSbrDec->LppTrans
                           .lpcFilterStatesImagLegSBR[LPC_ORDER + i][old_lsb],
                      fMax(new_lsb - old_lsb, 0) * sizeof(int32_t));
          }
        }
      }

      if (new_lsb > old_lsb) {
        stopBand = old_lsb;
      }
    }
    if ((adapt_lb == 1) && (stopBand > startBand)) {
      for (l = startSlot; l < hSbrDec->LppTrans.pSettings->overlap; l++) {
        scaleValues(OverlapBufferReal[l] + startBand, stopBand - startBand,
                    diff);
        if (!useLP) {
          scaleValues(OverlapBufferImag[l] + startBand, stopBand - startBand,
                      diff);
        }
      }
    }
    hSbrDec->qmfDomainInCh->scaling.ov_lb_scale = new_scale;
  }

  sbrError = ResetLimiterBands(hHeaderData->freqBandData.limiterBandTable,
                               &hHeaderData->freqBandData.noLimiterBands,
                               hHeaderData->freqBandData.freqBandTable[0],
                               hHeaderData->freqBandData.nSfb[0],
                               hSbrDec->LppTrans.pSettings->patchParam,
                               hSbrDec->LppTrans.pSettings->noOfPatches,
                               hHeaderData->bs_data.limiterBands,
                               hFrameData->sbrPatchingMode,
                               GetxOverBandQmfTransposer(hSbrDec->hHBE),
                               Get41SbrQmfTransposer(hSbrDec->hHBE));

  hSbrDec->SbrCalculateEnvelope.sbrPatchingMode = hFrameData->sbrPatchingMode;

  return sbrError;
}
