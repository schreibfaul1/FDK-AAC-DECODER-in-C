

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "psbitdec.h"
#include "sbr_rom.h"
#include "huff_dec.h"

/* PS dec privat functions */
SBR_ERROR ResetPsDec(HANDLE_PS_DEC h_ps_d);

/***************************************************************************/
/*!
  \brief  huffman decoding by codebook table

  \return index of huffman codebook table

****************************************************************************/
static int8_t decode_huff_cw(
    Huffman h,                    /*!< pointer to huffman codebook table */
    HANDLE_FDK_BITSTREAM hBitBuf, /*!< Handle to Bitbuffer */
    int32_t *length)                  /*!< length of huffman codeword (or NULL) */
{
  uint8_t bit = 0;
  int8_t index = 0;
  uint8_t bitCount = 0;

  while (index >= 0) {
    bit = FDKreadBits(hBitBuf, 1);
    bitCount++;
    index = h[index][bit];
  }
  if (length) {
    *length = bitCount;
  }
  return (index + 64); /* Add offset */
}

/***************************************************************************/
/*!
  \brief  helper function - limiting of value to min/max values

  \return limited value

****************************************************************************/

static int8_t limitMinMax(int8_t i, int8_t min, int8_t max) {
  if (i < min)
    return min;
  else if (i > max)
    return max;
  else
    return i;
}

/***************************************************************************/
/*!
  \brief  Decodes delta values in-place and updates
          data buffers according to quantization classes.

  When delta coded in frequency the first element is deltacode from zero.
  aIndex buffer is decoded from delta values to actual values.

  \return none

****************************************************************************/
static void deltaDecodeArray(
    int8_t enable, int8_t *aIndex,  /*!< ICC/IID parameters */
    int8_t *aPrevFrameIndex,       /*!< ICC/IID parameters  of previous frame */
    int8_t DtDf, uint8_t nrElements, /*!< as conveyed in bitstream */
                                  /*!< output array size: nrElements*stride */
    uint8_t stride,                 /*!< 1=dflt, 2=half freq. resolution */
    int8_t minIdx, int8_t maxIdx) {
  int32_t i;

  /* Delta decode */
  if (enable == 1) {
    if (DtDf == 0) { /* Delta coded in freq */
      aIndex[0] = 0 + aIndex[0];
      aIndex[0] = limitMinMax(aIndex[0], minIdx, maxIdx);
      for (i = 1; i < nrElements; i++) {
        aIndex[i] = aIndex[i - 1] + aIndex[i];
        aIndex[i] = limitMinMax(aIndex[i], minIdx, maxIdx);
      }
    } else { /* Delta time */
      for (i = 0; i < nrElements; i++) {
        aIndex[i] = aPrevFrameIndex[i * stride] + aIndex[i];
        aIndex[i] = limitMinMax(aIndex[i], minIdx, maxIdx);
      }
    }
  } else { /* No data is sent, set index to zero */
    for (i = 0; i < nrElements; i++) {
      aIndex[i] = 0;
    }
  }
  if (stride == 2) {
    for (i = nrElements * stride - 1; i > 0; i--) {
      aIndex[i] = aIndex[i >> 1];
    }
  }
}

/***************************************************************************/
/*!
  \brief Mapping of ICC/IID parameters to 20 stereo bands

  \return none

****************************************************************************/
static void map34IndexTo20(int8_t *aIndex, /*!< decoded ICC/IID parameters */
                           uint8_t noBins)  /*!< number of stereo bands     */
{
  aIndex[0] = (2 * aIndex[0] + aIndex[1]) / 3;
  aIndex[1] = (aIndex[1] + 2 * aIndex[2]) / 3;
  aIndex[2] = (2 * aIndex[3] + aIndex[4]) / 3;
  aIndex[3] = (aIndex[4] + 2 * aIndex[5]) / 3;
  aIndex[4] = (aIndex[6] + aIndex[7]) / 2;
  aIndex[5] = (aIndex[8] + aIndex[9]) / 2;
  aIndex[6] = aIndex[10];
  aIndex[7] = aIndex[11];
  aIndex[8] = (aIndex[12] + aIndex[13]) / 2;
  aIndex[9] = (aIndex[14] + aIndex[15]) / 2;
  aIndex[10] = aIndex[16];
  /* For IPD/OPD it stops here */

  if (noBins == NO_HI_RES_BINS) {
    aIndex[11] = aIndex[17];
    aIndex[12] = aIndex[18];
    aIndex[13] = aIndex[19];
    aIndex[14] = (aIndex[20] + aIndex[21]) / 2;
    aIndex[15] = (aIndex[22] + aIndex[23]) / 2;
    aIndex[16] = (aIndex[24] + aIndex[25]) / 2;
    aIndex[17] = (aIndex[26] + aIndex[27]) / 2;
    aIndex[18] = (aIndex[28] + aIndex[29] + aIndex[30] + aIndex[31]) / 4;
    aIndex[19] = (aIndex[32] + aIndex[33]) / 2;
  }
}

/***************************************************************************/
/*!
  \brief  Decodes delta coded IID, ICC, IPD and OPD indices

  \return PS processing flag. If set to 1

****************************************************************************/
int32_t DecodePs(struct PS_DEC *h_ps_d,  /*!< PS handle */
             const uint8_t frameError, /*!< Flag telling that frame had errors */
             PS_DEC_COEFFICIENTS *pScratch) {
  MPEG_PS_BS_DATA *pBsData;
  uint8_t gr, env;
  int32_t bPsHeaderValid, bPsDataAvail;

  /* Assign Scratch */
  h_ps_d->specificTo.mpeg.pCoef = pScratch;

  /* Shortcuts to avoid deferencing and keep the code readable */
  pBsData = &h_ps_d->bsData[h_ps_d->processSlot].mpeg;
  bPsHeaderValid = pBsData->bPsHeaderValid;
  bPsDataAvail =
      (h_ps_d->bPsDataAvail[h_ps_d->processSlot] == ppt_mpeg) ? 1 : 0;

  /***************************************************************************************
   * Decide whether to process or to conceal PS data or not. */

  if ((h_ps_d->psDecodedPrv && !frameError && !bPsDataAvail) ||
      (!h_ps_d->psDecodedPrv &&
       (frameError || !bPsDataAvail || !bPsHeaderValid))) {
    /* Don't apply PS processing.
     * Declare current PS header and bitstream data invalid. */
    pBsData->bPsHeaderValid = 0;
    h_ps_d->bPsDataAvail[h_ps_d->processSlot] = ppt_none;
    return (0);
  }

  if (frameError ||
      !bPsHeaderValid) { /* no new PS data available (e.g. frame loss) */
    /* => keep latest data constant (i.e. FIX with noEnv=0) */
    pBsData->noEnv = 0;
  }

  /***************************************************************************************
   * Decode bitstream payload or prepare parameter for concealment:
   */
  for (env = 0; env < pBsData->noEnv; env++) {
    int8_t *aPrevIidIndex;
    int8_t *aPrevIccIndex;

    uint8_t noIidSteps = pBsData->bFineIidQ ? NO_IID_STEPS_FINE : NO_IID_STEPS;

    if (env == 0) {
      aPrevIidIndex = h_ps_d->specificTo.mpeg.aIidPrevFrameIndex;
      aPrevIccIndex = h_ps_d->specificTo.mpeg.aIccPrevFrameIndex;
    } else {
      aPrevIidIndex = pBsData->aaIidIndex[env - 1];
      aPrevIccIndex = pBsData->aaIccIndex[env - 1];
    }

    deltaDecodeArray(pBsData->bEnableIid, pBsData->aaIidIndex[env],
                     aPrevIidIndex, pBsData->abIidDtFlag[env],
                     FDK_sbrDecoder_aNoIidBins[pBsData->freqResIid],
                     (pBsData->freqResIid) ? 1 : 2, -noIidSteps, noIidSteps);

    deltaDecodeArray(pBsData->bEnableIcc, pBsData->aaIccIndex[env],
                     aPrevIccIndex, pBsData->abIccDtFlag[env],
                     FDK_sbrDecoder_aNoIccBins[pBsData->freqResIcc],
                     (pBsData->freqResIcc) ? 1 : 2, 0, NO_ICC_STEPS - 1);
  } /* for (env=0; env<pBsData->noEnv; env++) */

  /* handling of FIX noEnv=0 */
  if (pBsData->noEnv == 0) {
    /* set noEnv=1, keep last parameters or force 0 if not enabled */
    pBsData->noEnv = 1;

    if (pBsData->bEnableIid) {
      pBsData->bFineIidQ = h_ps_d->specificTo.mpeg.bPrevFrameFineIidQ;
      pBsData->freqResIid = h_ps_d->specificTo.mpeg.prevFreqResIid;
      for (gr = 0; gr < NO_HI_RES_IID_BINS; gr++) {
        pBsData->aaIidIndex[pBsData->noEnv - 1][gr] =
            h_ps_d->specificTo.mpeg.aIidPrevFrameIndex[gr];
      }
    } else {
      for (gr = 0; gr < NO_HI_RES_IID_BINS; gr++) {
        pBsData->aaIidIndex[pBsData->noEnv - 1][gr] = 0;
      }
    }

    if (pBsData->bEnableIcc) {
      pBsData->freqResIcc = h_ps_d->specificTo.mpeg.prevFreqResIcc;
      for (gr = 0; gr < NO_HI_RES_ICC_BINS; gr++) {
        pBsData->aaIccIndex[pBsData->noEnv - 1][gr] =
            h_ps_d->specificTo.mpeg.aIccPrevFrameIndex[gr];
      }
    } else {
      for (gr = 0; gr < NO_HI_RES_ICC_BINS; gr++) {
        pBsData->aaIccIndex[pBsData->noEnv - 1][gr] = 0;
      }
    }
  }

  /* Update previous frame Iid quantization */
  h_ps_d->specificTo.mpeg.bPrevFrameFineIidQ = pBsData->bFineIidQ;

  /* Update previous frequency resolution for IID */
  h_ps_d->specificTo.mpeg.prevFreqResIid = pBsData->freqResIid;

  /* Update previous frequency resolution for ICC */
  h_ps_d->specificTo.mpeg.prevFreqResIcc = pBsData->freqResIcc;

  /* Update previous frame index buffers */
  for (gr = 0; gr < NO_HI_RES_IID_BINS; gr++) {
    h_ps_d->specificTo.mpeg.aIidPrevFrameIndex[gr] =
        pBsData->aaIidIndex[pBsData->noEnv - 1][gr];
  }
  for (gr = 0; gr < NO_HI_RES_ICC_BINS; gr++) {
    h_ps_d->specificTo.mpeg.aIccPrevFrameIndex[gr] =
        pBsData->aaIccIndex[pBsData->noEnv - 1][gr];
  }

  /* PS data from bitstream (if avail) was decoded now */
  h_ps_d->bPsDataAvail[h_ps_d->processSlot] = ppt_none;

  /* handling of env borders for FIX & VAR */
  if (pBsData->bFrameClass == 0) {
    /* FIX_BORDERS NoEnv=0,1,2,4 */
    pBsData->aEnvStartStop[0] = 0;
    for (env = 1; env < pBsData->noEnv; env++) {
      pBsData->aEnvStartStop[env] =
          (env * h_ps_d->noSubSamples) / pBsData->noEnv;
    }
    pBsData->aEnvStartStop[pBsData->noEnv] = h_ps_d->noSubSamples;
    /* 1024 (32 slots) env borders:  0, 8, 16, 24, 32 */
    /*  960 (30 slots) env borders:  0, 7, 15, 22, 30 */
  } else { /* if (h_ps_d->bFrameClass == 0) */
    /* VAR_BORDERS NoEnv=1,2,3,4 */
    pBsData->aEnvStartStop[0] = 0;

    /* handle case aEnvStartStop[noEnv]<noSubSample for VAR_BORDERS by
       duplicating last PS parameters and incrementing noEnv */
    if (pBsData->aEnvStartStop[pBsData->noEnv] < h_ps_d->noSubSamples) {
      for (gr = 0; gr < NO_HI_RES_IID_BINS; gr++) {
        pBsData->aaIidIndex[pBsData->noEnv][gr] =
            pBsData->aaIidIndex[pBsData->noEnv - 1][gr];
      }
      for (gr = 0; gr < NO_HI_RES_ICC_BINS; gr++) {
        pBsData->aaIccIndex[pBsData->noEnv][gr] =
            pBsData->aaIccIndex[pBsData->noEnv - 1][gr];
      }
      pBsData->noEnv++;
      pBsData->aEnvStartStop[pBsData->noEnv] = h_ps_d->noSubSamples;
    }

    /* enforce strictly monotonic increasing borders */
    for (env = 1; env < pBsData->noEnv; env++) {
      uint8_t thr;
      thr = (uint8_t)h_ps_d->noSubSamples - (pBsData->noEnv - env);
      if (pBsData->aEnvStartStop[env] > thr) {
        pBsData->aEnvStartStop[env] = thr;
      } else {
        thr = pBsData->aEnvStartStop[env - 1] + 1;
        if (pBsData->aEnvStartStop[env] < thr) {
          pBsData->aEnvStartStop[env] = thr;
        }
      }
    }
  } /* if (h_ps_d->bFrameClass == 0) ... else */

  /* copy data prior to possible 20<->34 in-place mapping */
  for (env = 0; env < pBsData->noEnv; env++) {
    uint8_t i;
    for (i = 0; i < NO_HI_RES_IID_BINS; i++) {
      h_ps_d->specificTo.mpeg.pCoef->aaIidIndexMapped[env][i] =
          pBsData->aaIidIndex[env][i];
    }
    for (i = 0; i < NO_HI_RES_ICC_BINS; i++) {
      h_ps_d->specificTo.mpeg.pCoef->aaIccIndexMapped[env][i] =
          pBsData->aaIccIndex[env][i];
    }
  }

  /* MPEG baseline PS */
  /* Baseline version of PS always uses the hybrid filter structure with 20
   * stereo bands. */
  /* If ICC/IID parameters for 34 stereo bands are decoded they have to be
   * mapped to 20   */
  /* stereo bands. */
  /* Additionaly the IPD/OPD parameters won't be used. */

  for (env = 0; env < pBsData->noEnv; env++) {
    if (pBsData->freqResIid == 2)
      map34IndexTo20(h_ps_d->specificTo.mpeg.pCoef->aaIidIndexMapped[env],
                     NO_HI_RES_IID_BINS);
    if (pBsData->freqResIcc == 2)
      map34IndexTo20(h_ps_d->specificTo.mpeg.pCoef->aaIccIndexMapped[env],
                     NO_HI_RES_ICC_BINS);

    /* IPD/OPD is disabled in baseline version and thus was removed here */
  }

  return (1);
}

/***************************************************************************/
/*!

  \brief  Reads parametric stereo data from bitstream

  \return

****************************************************************************/
uint32_t ReadPsData(
    HANDLE_PS_DEC h_ps_d,         /*!< handle to struct PS_DEC */
    HANDLE_FDK_BITSTREAM hBitBuf, /*!< handle to struct BIT_BUF */
    int32_t nBitsLeft                 /*!< max number of bits available */
) {
  MPEG_PS_BS_DATA *pBsData;

  uint8_t gr, env;
  int8_t dtFlag;
  int32_t startbits;
  Huffman CurrentTable;
  int8_t bEnableHeader;

  if (!h_ps_d) return 0;

  pBsData = &h_ps_d->bsData[h_ps_d->bsReadSlot].mpeg;

  if (h_ps_d->bsReadSlot != h_ps_d->bsLastSlot) {
    /* Copy last header data */
    memcpy(pBsData, &h_ps_d->bsData[h_ps_d->bsLastSlot].mpeg,
              sizeof(MPEG_PS_BS_DATA));
  }

  startbits = (int32_t)FDKgetValidBits(hBitBuf);

  bEnableHeader = (int8_t)FDKreadBits(hBitBuf, 1);

  /* Read header */
  if (bEnableHeader) {
    pBsData->bPsHeaderValid = 1;
    pBsData->bEnableIid = (uint8_t)FDKreadBits(hBitBuf, 1);
    if (pBsData->bEnableIid) {
      pBsData->modeIid = (uint8_t)FDKreadBits(hBitBuf, 3);
    }

    pBsData->bEnableIcc = (uint8_t)FDKreadBits(hBitBuf, 1);
    if (pBsData->bEnableIcc) {
      pBsData->modeIcc = (uint8_t)FDKreadBits(hBitBuf, 3);
    }

    pBsData->bEnableExt = (uint8_t)FDKreadBits(hBitBuf, 1);
  }

  pBsData->bFrameClass = (uint8_t)FDKreadBits(hBitBuf, 1);
  if (pBsData->bFrameClass == 0) {
    /* FIX_BORDERS NoEnv=0,1,2,4 */
    pBsData->noEnv =
        FDK_sbrDecoder_aFixNoEnvDecode[(uint8_t)FDKreadBits(hBitBuf, 2)];
    /* all additional handling of env borders is now in DecodePs() */
  } else {
    /* VAR_BORDERS NoEnv=1,2,3,4 */
    pBsData->noEnv = 1 + (uint8_t)FDKreadBits(hBitBuf, 2);
    for (env = 1; env < pBsData->noEnv + 1; env++)
      pBsData->aEnvStartStop[env] = ((uint8_t)FDKreadBits(hBitBuf, 5)) + 1;
    /* all additional handling of env borders is now in DecodePs() */
  }

  /* verify that IID & ICC modes (quant grid, freq res) are supported */
  if ((pBsData->modeIid > 5) || (pBsData->modeIcc > 5)) {
    /* no useful PS data could be read from bitstream */
    h_ps_d->bPsDataAvail[h_ps_d->bsReadSlot] = ppt_none;
    /* discard all remaining bits */
    nBitsLeft -= startbits - (int32_t)FDKgetValidBits(hBitBuf);
    while (nBitsLeft > 0) {
      int32_t i = nBitsLeft;
      if (i > 8) {
        i = 8;
      }
      FDKreadBits(hBitBuf, i);
      nBitsLeft -= i;
    }
    return (uint32_t)(startbits - (int32_t)FDKgetValidBits(hBitBuf));
  }

  if (pBsData->modeIid > 2) {
    pBsData->freqResIid = pBsData->modeIid - 3;
    pBsData->bFineIidQ = 1;
  } else {
    pBsData->freqResIid = pBsData->modeIid;
    pBsData->bFineIidQ = 0;
  }

  if (pBsData->modeIcc > 2) {
    pBsData->freqResIcc = pBsData->modeIcc - 3;
  } else {
    pBsData->freqResIcc = pBsData->modeIcc;
  }

  /* Extract IID data */
  if (pBsData->bEnableIid) {
    for (env = 0; env < pBsData->noEnv; env++) {
      dtFlag = (int8_t)FDKreadBits(hBitBuf, 1);
      if (!dtFlag) {
        if (pBsData->bFineIidQ)
          CurrentTable = (Huffman)&aBookPsIidFineFreqDecode;
        else
          CurrentTable = (Huffman)&aBookPsIidFreqDecode;
      } else {
        if (pBsData->bFineIidQ)
          CurrentTable = (Huffman)&aBookPsIidFineTimeDecode;
        else
          CurrentTable = (Huffman)&aBookPsIidTimeDecode;
      }

      for (gr = 0; gr < FDK_sbrDecoder_aNoIidBins[pBsData->freqResIid]; gr++)
        pBsData->aaIidIndex[env][gr] =
            decode_huff_cw(CurrentTable, hBitBuf, NULL);
      pBsData->abIidDtFlag[env] = dtFlag;
    }
  }

  /* Extract ICC data */
  if (pBsData->bEnableIcc) {
    for (env = 0; env < pBsData->noEnv; env++) {
      dtFlag = (int8_t)FDKreadBits(hBitBuf, 1);
      if (!dtFlag)
        CurrentTable = (Huffman)&aBookPsIccFreqDecode;
      else
        CurrentTable = (Huffman)&aBookPsIccTimeDecode;

      for (gr = 0; gr < FDK_sbrDecoder_aNoIccBins[pBsData->freqResIcc]; gr++)
        pBsData->aaIccIndex[env][gr] =
            decode_huff_cw(CurrentTable, hBitBuf, NULL);
      pBsData->abIccDtFlag[env] = dtFlag;
    }
  }

  if (pBsData->bEnableExt) {
    /*!
    Decoders that support only the baseline version of the PS tool are allowed
    to ignore the IPD/OPD data, but according header data has to be parsed.
    ISO/IEC 14496-3 Subpart 8 Annex 4
    */

    int32_t cnt = FDKreadBits(hBitBuf, PS_EXTENSION_SIZE_BITS);
    if (cnt == (1 << PS_EXTENSION_SIZE_BITS) - 1) {
      cnt += FDKreadBits(hBitBuf, PS_EXTENSION_ESC_COUNT_BITS);
    }
    while (cnt--) FDKreadBits(hBitBuf, 8);
  }

  /* new PS data was read from bitstream */
  h_ps_d->bPsDataAvail[h_ps_d->bsReadSlot] = ppt_mpeg;

  return (startbits - (int32_t)FDKgetValidBits(hBitBuf));
}
