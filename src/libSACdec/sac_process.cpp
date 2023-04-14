/* -----------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2019 Fraunhofer-Gesellschaft zur Förderung der angewandten
Forschung e.V. All rights reserved.

 1.    INTRODUCTION
The Fraunhofer FDK AAC Codec Library for Android ("FDK AAC Codec") is software
that implements the MPEG Advanced Audio Coding ("AAC") encoding and decoding
scheme for digital audio. This FDK AAC Codec software is intended to be used on
a wide variety of Android devices.

AAC's HE-AAC and HE-AAC v2 versions are regarded as today's most efficient
general perceptual audio codecs. AAC-ELD is considered the best-performing
full-bandwidth communications codec by independent studies and is widely
deployed. AAC has been standardized by ISO and IEC as part of the MPEG
specifications.

Patent licenses for necessary patent claims for the FDK AAC Codec (including
those of Fraunhofer) may be obtained through Via Licensing
(www.vialicensing.com) or through the respective patent owners individually for
the purpose of encoding or decoding bit streams in products that are compliant
with the ISO/IEC MPEG audio standards. Please note that most manufacturers of
Android devices already license these patent claims through Via Licensing or
directly from the patent owners, and therefore FDK AAC Codec software may
already be covered under those patent licenses when it is used for those
licensed purposes only.

Commercially-licensed AAC software libraries, including floating-point versions
with enhanced sound quality, are also available from Fraunhofer. Users are
encouraged to check the Fraunhofer website for additional applications
information and documentation.

2.    COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification,
are permitted without payment of copyright license fees provided that you
satisfy the following conditions:

You must retain the complete text of this software license in redistributions of
the FDK AAC Codec or your modifications thereto in source code form.

You must retain the complete text of this software license in the documentation
and/or other materials provided with redistributions of the FDK AAC Codec or
your modifications thereto in binary form. You must make available free of
charge copies of the complete source code of the FDK AAC Codec and your
modifications thereto to recipients of copies in binary form.

The name of Fraunhofer may not be used to endorse or promote products derived
from this library without prior written permission.

You may not charge copyright license fees for anyone to use, copy or distribute
the FDK AAC Codec software or your modifications thereto.

Your modified versions of the FDK AAC Codec must carry prominent notices stating
that you changed the software and the date of any change. For modified versions
of the FDK AAC Codec, the term "Fraunhofer FDK AAC Codec Library for Android"
must be replaced by the term "Third-Party Modified Version of the Fraunhofer FDK
AAC Codec Library for Android."

3.    NO PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without
limitation the patents of Fraunhofer, ARE GRANTED BY THIS SOFTWARE LICENSE.
Fraunhofer provides no warranty of patent non-infringement with respect to this
software.

You may use this FDK AAC Codec software or modifications thereto only for
purposes that are authorized by appropriate patent licenses.

4.    DISCLAIMER

This FDK AAC Codec software is provided by Fraunhofer on behalf of the copyright
holders and contributors "AS IS" and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
including but not limited to the implied warranties of merchantability and
fitness for a particular purpose. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE for any direct, indirect, incidental, special, exemplary,
or consequential damages, including but not limited to procurement of substitute
goods or services; loss of use, data, or profits, or business interruption,
however caused and on any theory of liability, whether in contract, strict
liability, or tort (including negligence), arising in any way out of the use of
this software, even if advised of the possibility of such damage.

5.    CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Audio and Multimedia Departments - FDK AAC LL
Am Wolfsmantel 33
91058 Erlangen, Germany

www.iis.fraunhofer.de/amm
amm-info@iis.fraunhofer.de
----------------------------------------------------------------------------- */

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: SAC Processing

*******************************************************************************/

/* data structures and interfaces for spatial audio reference software */
#include "sac_process.h"
#include "sac_bitdec.h"
#include "sac_calcM1andM2.h"
#include "sac_smoothing.h"
#include "sac_rom.h"
#include "sac_dec_errorcodes.h"

#include "../libFDK/FDK_trigFcts.h"
#include "../libFDK/FDK_decorrelate.h"

#define SAC_DEC_APPLY_M2_SCALE(spec, s) ((spec) >> (-(s)))

/**
 * \brief  Linear interpolation between two parameter values.
 *         a*alpha + b*(1-alpha)
 *       = a*alpha + b - b*alpha
 *
 * \param alpha               Weighting factor.
 * \param a                   Parameter a.
 * \param b                   Parameter b.
 *
 * \return Interpolated parameter value.
 */
static inline int32_t interpolateParameter(const FIXP_SGL alpha, const int32_t a,
                                         const int32_t b) {
  return (b - fMult(alpha, b) + fMult(alpha, a));
}

/**
 * \brief Map MPEG Surround channel indices to MPEG 4 PCE like channel indices.
 * \param self Spatial decoder handle.
 * \param ch MPEG Surround channel index.
 * \return MPEG 4 PCE style channel index, corresponding to the given MPEG
 * Surround channel index.
 */
static uint32_t mapChannel(spatialDec *self, uint32_t ch) {
  static const uint8_t chanelIdx[][8] = {
      {0, 1, 2, 3, 4, 5, 6, 7}, /*  binaural, TREE_212, arbitrary tree */
  };

  int32_t idx = 0;

  return (chanelIdx[idx][ch]);
}

int32_t getChGain(spatialDec *self, uint32_t ch, int32_t *scale) {
  /* init no gain modifier */
  int32_t gain = 0x80000000;
  *scale = 0;

  if ((!isTwoChMode(self->upmixType)) &&
      (self->upmixType != UPMIXTYPE_BYPASS)) {
    if ((ch == 0) || (ch == 1) || (ch == 2)) {
      /* no modifier */
    }
  }

  return gain;
}

SACDEC_ERROR SpatialDecQMFAnalysis(spatialDec *self, const PCM_MPS *inData,
                                   const int32_t ts, const int32_t bypassMode,
                                   int32_t **qmfReal, int32_t **qmfImag,
                                   const int32_t numInputChannels) {
  SACDEC_ERROR err = MPS_OK;
  int32_t ch, offset;

  offset = self->pQmfDomain->globalConf.nBandsSynthesis *
           self->pQmfDomain->globalConf.nQmfTimeSlots;

  {
    for (ch = 0; ch < numInputChannels; ch++) {
      const PCM_MPS *inSamples =
          &inData[ts * self->pQmfDomain->globalConf.nBandsAnalysis];
      int32_t *pQmfRealAnalysis = qmfReal[ch]; /* no delay in blind mode */
      int32_t *pQmfImagAnalysis = qmfImag[ch];

      CalculateSpaceAnalysisQmf(&self->pQmfDomain->QmfDomainIn[ch].fb,
                                inSamples + (ch * offset), pQmfRealAnalysis,
                                pQmfImagAnalysis);

      if (!isTwoChMode(self->upmixType) && !bypassMode) {
        int32_t i;
        for (i = 0; i < self->qmfBands; i++) {
          qmfReal[ch][i] = fMult(
              scaleValueSaturate(qmfReal[ch][i], self->sacInDataHeadroom - (1)),
              self->clipProtectGain__FDK);
          qmfImag[ch][i] = fMult(
              scaleValueSaturate(qmfImag[ch][i], self->sacInDataHeadroom - (1)),
              self->clipProtectGain__FDK);
        }
      }
    }
  }

  self->qmfInputDelayBufPos =
      (self->qmfInputDelayBufPos + 1) % self->pc_filterdelay;

  return err;
}

SACDEC_ERROR SpatialDecFeedQMF(spatialDec *self, int32_t **qmfInDataReal,
                               int32_t **qmfInDataImag, const int32_t ts,
                               const int32_t bypassMode, int32_t **qmfReal__FDK,
                               int32_t **qmfImag__FDK,
                               const int32_t numInputChannels) {
  SACDEC_ERROR err = MPS_OK;
  int32_t ch;

  {
    for (ch = 0; ch < numInputChannels; ch++) {
      int32_t *pQmfRealAnalysis =
          qmfReal__FDK[ch]; /* no delay in blind mode */
      int32_t *pQmfImagAnalysis = qmfImag__FDK[ch];

      /* Write Input data to pQmfRealAnalysis. */
      if (self->bShareDelayWithSBR) {
        FDK_QmfDomain_GetSlot(&self->pQmfDomain->QmfDomainIn[ch],
                              ts + HYBRID_FILTER_DELAY, 0,
                              MAX_QMF_BANDS_TO_HYBRID, pQmfRealAnalysis,
                              pQmfImagAnalysis, 15 + (1));
        FDK_QmfDomain_GetSlot(&self->pQmfDomain->QmfDomainIn[ch], ts,
                              MAX_QMF_BANDS_TO_HYBRID, self->qmfBands,
                              pQmfRealAnalysis, pQmfImagAnalysis, 15 + (1));
      } else {
        FDK_QmfDomain_GetSlot(&self->pQmfDomain->QmfDomainIn[ch], ts, 0,
                              self->qmfBands, pQmfRealAnalysis,
                              pQmfImagAnalysis, 15 + (1));
      }
      if (ts == self->pQmfDomain->globalConf.nQmfTimeSlots - 1) {
        /* Is currently also needed in case we dont have any overlap. We need to
         * save lb_scale to ov_lb_scale */
        FDK_QmfDomain_SaveOverlap(&self->pQmfDomain->QmfDomainIn[ch], 0);
      }

      /* Apply clip protection to output. */
      if (!isTwoChMode(self->upmixType) && !bypassMode) {
        int32_t i;
        for (i = 0; i < self->qmfBands; i++) {
          qmfReal__FDK[ch][i] =
              fMult(qmfReal__FDK[ch][i], self->clipProtectGain__FDK);
          qmfImag__FDK[ch][i] =
              fMult(qmfImag__FDK[ch][i], self->clipProtectGain__FDK);
        }
      }

    } /* End of loop over numInputChannels */
  }

  self->qmfInputDelayBufPos =
      (self->qmfInputDelayBufPos + 1) % self->pc_filterdelay;

  return err;
}

/*******************************************************************************
 Functionname: SpatialDecHybridAnalysis
 *******************************************************************************

 Description:

 Arguments:

 Input:
  float** pointers[4] leftReal, leftIm, rightReal, rightIm

 Output:
  float self->qmfInputReal[MAX_INPUT_CHANNELS][MAX_TIME_SLOTS][MAX_QMF_BANDS];
  float self->qmfInputImag[MAX_INPUT_CHANNELS][MAX_TIME_SLOTS][MAX_QMF_BANDS];

  float
self->hybInputReal[MAX_INPUT_CHANNELS][MAX_TIME_SLOTS][MAX_HYBRID_BANDS]; float
self->hybInputImag[MAX_INPUT_CHANNELS][MAX_TIME_SLOTS][MAX_HYBRID_BANDS];


*******************************************************************************/
SACDEC_ERROR SpatialDecHybridAnalysis(spatialDec *self, int32_t **qmfInputReal,
                                      int32_t **qmfInputImag,
                                      int32_t **hybOutputReal,
                                      int32_t **hybOutputImag, const int32_t ts,
                                      const int32_t numInputChannels) {
  SACDEC_ERROR err = MPS_OK;
  int32_t ch;

  for (ch = 0; ch < numInputChannels;
       ch++) /* hybrid filtering for down-mix signals */
  {
    if (self->pConfigCurrent->syntaxFlags & SACDEC_SYNTAX_LD) {
      int32_t k;
      /* No hybrid filtering. Just copy the QMF data. */
      for (k = 0; k < self->hybridBands; k += 1) {
        hybOutputReal[ch][k] = qmfInputReal[ch][k];
        hybOutputImag[ch][k] = qmfInputImag[ch][k];
      }
    } else {
      self->hybridAnalysis[ch].hfMode = self->bShareDelayWithSBR;

      if (self->stereoConfigIndex == 3)
        assert(self->hybridAnalysis[ch].hfMode == 0);
      FDKhybridAnalysisApply(&self->hybridAnalysis[ch], qmfInputReal[ch],
                             qmfInputImag[ch], hybOutputReal[ch],
                             hybOutputImag[ch]);
    }
  }

  if ((self->pConfigCurrent->syntaxFlags & SACDEC_SYNTAX_USAC) &&
      self->residualCoding) {
    self->hybridAnalysis[numInputChannels].hfMode = 0;
    FDKhybridAnalysisApply(
        &self->hybridAnalysis[numInputChannels],
        self->qmfResidualReal__FDK[0][0], self->qmfResidualImag__FDK[0][0],
        self->hybResidualReal__FDK[0], self->hybResidualImag__FDK[0]);
  }

  return err;
}

SACDEC_ERROR SpatialDecCreateX(spatialDec *self, int32_t **hybInputReal,
                               int32_t **hybInputImag, int32_t **pxReal,
                               int32_t **pxImag) {
  SACDEC_ERROR err = MPS_OK;
  int32_t row;

  /* Creating wDry */
  for (row = 0; row < self->numInputChannels; row++) {
    /* pointer to direct signals */
    pxReal[row] = hybInputReal[row];
    pxImag[row] = hybInputImag[row];
  }

  return err;
}

static void M2ParamToKernelMult(FIXP_SGL * pKernel,
                                int32_t * Mparam,
                                int32_t * MparamPrev,
                                int32_t * pWidth, FIXP_SGL alpha__FDK,
                                int32_t nBands) {
  int32_t pb;

  for (pb = 0; pb < nBands; pb++) {
    FIXP_SGL tmp = FX_DBL2FX_SGL(
        interpolateParameter(alpha__FDK, Mparam[pb], MparamPrev[pb]));

    int32_t i = pWidth[pb];
    if (i & 1) *pKernel++ = tmp;
    if (i & 2) {
      *pKernel++ = tmp;
      *pKernel++ = tmp;
    }
    for (i >>= 2; i--;) {
      *pKernel++ = tmp;
      *pKernel++ = tmp;
      *pKernel++ = tmp;
      *pKernel++ = tmp;
    }
  }
}

SACDEC_ERROR SpatialDecApplyM1_CreateW_Mode212(
    spatialDec *self, const SPATIAL_BS_FRAME *frame, int32_t **xReal,
    int32_t **xImag, int32_t **vReal, int32_t **vImag) {
  SACDEC_ERROR err = MPS_OK;
  int32_t res;
  int32_t *decorrInReal = vReal[0];
  int32_t *decorrInImag = vImag[0];

  /* M1 does not do anything in 212 mode, so use simplified processing */
  assert(self->numVChannels == 2);
  assert(self->numDirektSignals == 1);
  assert(self->numDecorSignals == 1);
  FDKmemcpy(vReal[0], xReal[0], self->hybridBands * sizeof(int32_t));
  FDKmemcpy(vImag[0], xImag[0], self->hybridBands * sizeof(int32_t));

  if (isTsdActive(frame->TsdData)) {
    /* Generate v_{x,nonTr} as input for allpass based decorrelator */
    TsdGenerateNonTr(self->hybridBands, frame->TsdData, self->TsdTs, vReal[0],
                     vImag[0], vReal[1], vImag[1], &decorrInReal,
                     &decorrInImag);
  }
  /* - Decorrelate */
  res = SpatialDecGetResidualIndex(self, 1);
  if (FDKdecorrelateApply(&self->apDecor[0], decorrInReal, decorrInImag,
                          vReal[1], vImag[1],
                          self->param2hyb[self->residualBands[res]])) {
    return MPS_NOTOK;
  }
  if (isTsdActive(frame->TsdData)) {
    /* Generate v_{x,Tr}, apply transient decorrelator and add to allpass based
     * decorrelator output */
    TsdApply(self->hybridBands, frame->TsdData, &self->TsdTs,
             vReal[0], /* input: v_x */
             vImag[0],
             vReal[1], /* input: d_{x,nonTr}; output: d_{x,nonTr} + d_{x,Tr} */
             vImag[1]);
  }

  /* Write residual signal in approriate parameter bands */
  if (self->residualBands[res] > 0) {
    int32_t stopBand = self->param2hyb[self->residualBands[res]];
    FDKmemcpy(vReal[1], self->hybResidualReal__FDK[res],
              fixMin(stopBand, self->hybridBands) * sizeof(int32_t));
    FDKmemcpy(vImag[1], self->hybResidualImag__FDK[res],
              fixMin(stopBand, self->hybridBands) * sizeof(int32_t));
  } /* (self->residualBands[res]>0) */

  return err;
}

SACDEC_ERROR SpatialDecApplyM2_Mode212(spatialDec *self, int32_t ps,
                                       const FIXP_SGL alpha, int32_t **wReal,
                                       int32_t **wImag,
                                       int32_t **hybOutputRealDry,
                                       int32_t **hybOutputImagDry) {
  SACDEC_ERROR err = MPS_OK;
  int32_t row;

  int32_t *pWidth = self->kernels_width;
  /* for stereoConfigIndex == 3 case hybridBands is < 71 */
  int32_t pb_max = self->kernels[self->hybridBands - 1] + 1;
  int32_t max_row = self->numOutputChannels;

  int32_t M2_exp = 0;
  if (self->residualCoding) M2_exp = 3;

  for (row = 0; row < max_row; row++)  // 2 times
  {
    int32_t *Mparam0 = self->M2Real__FDK[row][0];
    int32_t *Mparam1 = self->M2Real__FDK[row][1];
    int32_t *MparamPrev0 = self->M2RealPrev__FDK[row][0];
    int32_t *MparamPrev1 = self->M2RealPrev__FDK[row][1];

    int32_t * pHybOutRealDry = hybOutputRealDry[row];
    int32_t * pHybOutImagDry = hybOutputImagDry[row];

    int32_t * pWReal0 = wReal[0];
    int32_t * pWReal1 = wReal[1];
    int32_t * pWImag0 = wImag[0];
    int32_t * pWImag1 = wImag[1];
    for (int32_t pb = 0; pb < pb_max; pb++) {
      int32_t tmp0, tmp1;

      tmp0 = interpolateParameter(alpha, Mparam0[pb], MparamPrev0[pb]);
      tmp1 = interpolateParameter(alpha, Mparam1[pb], MparamPrev1[pb]);

      int32_t i = pWidth[pb];

      do  // about 3-4 times
      {
        int32_t var0, var1, real, imag;

        var0 = *pWReal0++;
        var1 = *pWReal1++;
        real = fMultDiv2(var0, tmp0);
        var0 = *pWImag0++;
        real = fMultAddDiv2(real, var1, tmp1);
        var1 = *pWImag1++;
        imag = fMultDiv2(var0, tmp0);
        *pHybOutRealDry++ = real << (1 + M2_exp);
        imag = fMultAddDiv2(imag, var1, tmp1);
        *pHybOutImagDry++ = imag << (1 + M2_exp);
      } while (--i != 0);
    }
  }
  return err;
}

SACDEC_ERROR SpatialDecApplyM2_Mode212_ResidualsPlusPhaseCoding(
    spatialDec *self, int32_t ps, const FIXP_SGL alpha, int32_t **wReal,
    int32_t **wImag, int32_t **hybOutputRealDry,
    int32_t **hybOutputImagDry) {
  SACDEC_ERROR err = MPS_OK;
  int32_t row;
  int32_t scale_param_m2;
  int32_t *pWidth = self->kernels_width;
  int32_t pb_max = self->kernels[self->hybridBands - 1] + 1;

  scale_param_m2 = SCALE_PARAM_M2_212_PRED + SCALE_DATA_APPLY_M2;

  for (row = 0; row < self->numM2rows; row++) {
    int32_t qs, pb;

    int32_t * pWReal0 = wReal[0];
    int32_t * pWImag0 = wImag[0];
    int32_t * pWReal1 = wReal[1];
    int32_t * pWImag1 = wImag[1];

    int32_t *MReal0 = self->M2Real__FDK[row][0];
    int32_t *MImag0 = self->M2Imag__FDK[row][0];
    int32_t *MReal1 = self->M2Real__FDK[row][1];
    int32_t *MRealPrev0 = self->M2RealPrev__FDK[row][0];
    int32_t *MImagPrev0 = self->M2ImagPrev__FDK[row][0];
    int32_t *MRealPrev1 = self->M2RealPrev__FDK[row][1];

    int32_t * pHybOutRealDry = hybOutputRealDry[row];
    int32_t * pHybOutImagDry = hybOutputImagDry[row];

    assert(!(self->pConfigCurrent->syntaxFlags & SACDEC_SYNTAX_LD));
    assert((pWidth[0] + pWidth[1]) >= 3);

    for (pb = 0, qs = 3; pb < 2; pb++) {
      int32_t s;
      int32_t maxVal;
      int32_t mReal1;
      int32_t mReal0, mImag0;
      int32_t iReal0, iImag0, iReal1;

      iReal0 = interpolateParameter(alpha, MReal0[pb], MRealPrev0[pb]);
      iImag0 = -interpolateParameter(alpha, MImag0[pb], MImagPrev0[pb]);
      iReal1 = interpolateParameter(alpha, MReal1[pb], MRealPrev1[pb]);

      maxVal = fAbs(iReal0) | fAbs(iImag0);
      maxVal |= fAbs(iReal1);

      s = fMax(CntLeadingZeros(maxVal) - 1, 0);
      s = fMin(s, scale_param_m2);

      mReal0 = iReal0 << s;
      mImag0 = iImag0 << s;
      mReal1 = iReal1 << s;

      s = scale_param_m2 - s;

      int32_t i = pWidth[pb];

      do {
        int32_t real, imag, wReal0, wImag0, wReal1, wImag1;

        wReal0 = *pWReal0++;
        wImag0 = *pWImag0++;
        wReal1 = *pWReal1++;
        wImag1 = *pWImag1++;

        cplxMultDiv2(&real, &imag, wReal0, wImag0, mReal0, mImag0);

        *pHybOutRealDry++ = fMultAddDiv2(real, wReal1, mReal1) << s;
        *pHybOutImagDry++ = fMultAddDiv2(imag, wImag1, mReal1) << s;

        if (qs > 0) {
          mImag0 = -mImag0;
          qs--;
        }
      } while (--i != 0);
    }

    for (; pb < pb_max; pb++) {
      int32_t s;
      int32_t maxVal;
      FIXP_SGL mReal1;
      FIXP_SGL mReal0, mImag0;
      int32_t iReal0, iImag0, iReal1;

      iReal0 = interpolateParameter(alpha, MReal0[pb], MRealPrev0[pb]);
      iImag0 = interpolateParameter(alpha, MImag0[pb], MImagPrev0[pb]);
      iReal1 = interpolateParameter(alpha, MReal1[pb], MRealPrev1[pb]);

      maxVal = fAbs(iReal0) | fAbs(iImag0);
      maxVal |= fAbs(iReal1);

      s = fMax(CntLeadingZeros(maxVal) - 1, 0);
      s = fMin(s, scale_param_m2);

      mReal0 = FX_DBL2FX_SGL(iReal0 << s);
      mImag0 = FX_DBL2FX_SGL(iImag0 << s);
      mReal1 = FX_DBL2FX_SGL(iReal1 << s);

      s = scale_param_m2 - s;

      int32_t i = pWidth[pb];

      do {
        int32_t real, imag, wReal0, wImag0, wReal1, wImag1;

        wReal0 = *pWReal0++;
        wImag0 = *pWImag0++;
        wReal1 = *pWReal1++;
        wImag1 = *pWImag1++;

        cplxMultDiv2(&real, &imag, wReal0, wImag0, mReal0, mImag0);

        *pHybOutRealDry++ = fMultAddDiv2(real, wReal1, mReal1) << s;
        *pHybOutImagDry++ = fMultAddDiv2(imag, wImag1, mReal1) << s;
      } while (--i != 0);
    }
  }

  return err;
}

SACDEC_ERROR SpatialDecApplyM2(spatialDec *self, int32_t ps, const FIXP_SGL alpha,
                               int32_t **wReal, int32_t **wImag,
                               int32_t **hybOutputRealDry,
                               int32_t **hybOutputImagDry,
                               int32_t **hybOutputRealWet,
                               int32_t **hybOutputImagWet) {
  SACDEC_ERROR err = MPS_OK;

  {
    int32_t qs, row, col;
    int32_t complexHybBands;
    int32_t complexParBands;
    int32_t scale_param_m2 = 0;
    int32_t toolsDisabled;

    uint8_t activParamBands;
    int32_t * pWReal, * pWImag, * pHybOutRealDry,
        * pHybOutImagDry, * pHybOutRealWet,
        * pHybOutImagWet;
    C_ALLOC_SCRATCH_START(pKernel, FIXP_SGL, MAX_HYBRID_BANDS);

    /* The wet signal is added to the dry signal directly in applyM2 if GES and
     * STP are disabled */
    toolsDisabled =
        ((self->tempShapeConfig == 1) || (self->tempShapeConfig == 2)) ? 0 : 1;

    {
      complexHybBands = self->hybridBands;
      complexParBands = self->numParameterBands;
    }

    FDKmemclear(hybOutputImagDry[0],
                self->createParams.maxNumOutputChannels *
                    self->createParams.maxNumCmplxHybBands * sizeof(int32_t));
    FDKmemclear(hybOutputRealDry[0], self->createParams.maxNumOutputChannels *
                                         self->createParams.maxNumHybridBands *
                                         sizeof(int32_t));

    if (!toolsDisabled) {
      FDKmemclear(hybOutputRealWet[0],
                  self->createParams.maxNumOutputChannels *
                      self->createParams.maxNumHybridBands * sizeof(int32_t));
      FDKmemclear(hybOutputImagWet[0],
                  self->createParams.maxNumOutputChannels *
                      self->createParams.maxNumCmplxHybBands *
                      sizeof(int32_t));
    }

    if (self->phaseCoding == 3) {
      scale_param_m2 = -(SCALE_DATA_APPLY_M2_PC - 1);
    }

    for (row = 0; row < self->numM2rows; row++) {
      pHybOutRealDry = hybOutputRealDry[row];
      pHybOutImagDry = hybOutputImagDry[row];

      if (toolsDisabled) {
        pHybOutRealWet = hybOutputRealDry[row];
        pHybOutImagWet = hybOutputImagDry[row];
      } else {
        pHybOutRealWet = hybOutputRealWet[row];
        pHybOutImagWet = hybOutputImagWet[row];
      }

      for (col = 0; col < self->numDirektSignals; col++) {
        if (self->pActivM2ParamBands ==
            0) { /* default setting, calculate all rows and columns */
          activParamBands = 1;
        } else {
          if (self->pActivM2ParamBands[MAX_M2_INPUT * row +
                                       col]) /* table with activ and inactiv
                                                bands exists for current
                                                configuration */
            activParamBands = 1;
          else
            activParamBands = 0;
        }
        if (activParamBands) {
          pWReal = wReal[col];
          pWImag = wImag[col];

          M2ParamToKernelMult(pKernel, self->M2Real__FDK[row][col],
                              self->M2RealPrev__FDK[row][col],
                              self->kernels_width, alpha,
                              self->numParameterBands);

          if (1 && (self->phaseCoding != 3)) {
            /* direct signals */
            {
              /* only one sample will be assigned to each row, hence
               * accumulation is not neccessary; that is valid for all
               * configurations */
              for (qs = 0; qs < complexHybBands; qs++) {
                pHybOutRealDry[qs] = fMult(pWReal[qs], pKernel[qs]);
                pHybOutImagDry[qs] = fMult(pWImag[qs], pKernel[qs]);
              }
            }
          } else { /*  isBinauralMode(self->upmixType)  */

            for (qs = 0; qs < complexHybBands; qs++) {
              pHybOutRealDry[qs] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWReal[qs], pKernel[qs]), scale_param_m2);
              pHybOutImagDry[qs] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWImag[qs], pKernel[qs]), scale_param_m2);
            }

            M2ParamToKernelMult(pKernel, self->M2Imag__FDK[row][col],
                                self->M2ImagPrev__FDK[row][col],
                                self->kernels_width, alpha, complexParBands);

            /* direct signals sign is -1 for qs = 0,2 */
            pHybOutRealDry[0] += SAC_DEC_APPLY_M2_SCALE(
                fMultDiv2(pWImag[0], pKernel[0]), scale_param_m2);
            pHybOutImagDry[0] -= SAC_DEC_APPLY_M2_SCALE(
                fMultDiv2(pWReal[0], pKernel[0]), scale_param_m2);

            pHybOutRealDry[2] += SAC_DEC_APPLY_M2_SCALE(
                fMultDiv2(pWImag[2], pKernel[2]), scale_param_m2);
            pHybOutImagDry[2] -= SAC_DEC_APPLY_M2_SCALE(
                fMultDiv2(pWReal[2], pKernel[2]), scale_param_m2);

            /* direct signals sign is +1 for qs = 1,3,4,5,...,complexHybBands */
            pHybOutRealDry[1] -= SAC_DEC_APPLY_M2_SCALE(
                fMultDiv2(pWImag[1], pKernel[1]), scale_param_m2);
            pHybOutImagDry[1] += SAC_DEC_APPLY_M2_SCALE(
                fMultDiv2(pWReal[1], pKernel[1]), scale_param_m2);

            for (qs = 3; qs < complexHybBands; qs++) {
              pHybOutRealDry[qs] -= SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWImag[qs], pKernel[qs]), scale_param_m2);
              pHybOutImagDry[qs] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWReal[qs], pKernel[qs]), scale_param_m2);
            }
          } /* self->upmixType */
        }   /* if (activParamBands) */
      }     /* self->numDirektSignals */

      for (; col < self->numVChannels; col++) {
        if (self->pActivM2ParamBands ==
            0) { /* default setting, calculate all rows and columns */
          activParamBands = 1;
        } else {
          if (self->pActivM2ParamBands[MAX_M2_INPUT * row +
                                       col]) /* table with activ and inactiv
                                                bands exists for current
                                                configuration */
            activParamBands = 1;
          else
            activParamBands = 0;
        }

        if (activParamBands) {
          int32_t resBandIndex;
          int32_t resHybIndex;

          resBandIndex =
              self->residualBands[SpatialDecGetResidualIndex(self, col)];
          resHybIndex = self->param2hyb[resBandIndex];

          pWReal = wReal[col];
          pWImag = wImag[col];

          M2ParamToKernelMult(pKernel, self->M2Real__FDK[row][col],
                              self->M2RealPrev__FDK[row][col],
                              self->kernels_width, alpha,
                              self->numParameterBands);

          if (1 && (self->phaseCoding != 3)) {
            /* residual signals */
            for (qs = 0; qs < resHybIndex; qs++) {
              pHybOutRealDry[qs] += fMult(pWReal[qs], pKernel[qs]);
              pHybOutImagDry[qs] += fMult(pWImag[qs], pKernel[qs]);
            }
            /* decor signals */
            for (; qs < complexHybBands; qs++) {
              pHybOutRealWet[qs] += fMult(pWReal[qs], pKernel[qs]);
              pHybOutImagWet[qs] += fMult(pWImag[qs], pKernel[qs]);
            }
          } else { /* self->upmixType */
            /* residual signals */
            int32_t * pHybOutReal;
            int32_t * pHybOutImag;

            for (qs = 0; qs < resHybIndex; qs++) {
              pHybOutRealDry[qs] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWReal[qs], pKernel[qs]), scale_param_m2);
              pHybOutImagDry[qs] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWImag[qs], pKernel[qs]), scale_param_m2);
            }
            /* decor signals */
            for (; qs < complexHybBands; qs++) {
              pHybOutRealWet[qs] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWReal[qs], pKernel[qs]), scale_param_m2);
              pHybOutImagWet[qs] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWImag[qs], pKernel[qs]), scale_param_m2);
            }

            M2ParamToKernelMult(pKernel, self->M2Imag__FDK[row][col],
                                self->M2ImagPrev__FDK[row][col],
                                self->kernels_width, alpha, complexParBands);

            /* direct signals sign is -1 for qs = 0,2 */
            /* direct signals sign is +1 for qs = 1,3.. */
            if (toolsDisabled) {
              pHybOutRealDry[0] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWImag[0], pKernel[0]), scale_param_m2);
              pHybOutImagDry[0] -= SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWReal[0], pKernel[0]), scale_param_m2);

              pHybOutRealDry[1] -= SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWImag[1], pKernel[1]), scale_param_m2);
              pHybOutImagDry[1] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWReal[1], pKernel[1]), scale_param_m2);

              pHybOutRealDry[2] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWImag[2], pKernel[2]), scale_param_m2);
              pHybOutImagDry[2] -= SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWReal[2], pKernel[2]), scale_param_m2);
            } else {
              pHybOutReal = &pHybOutRealDry[0];
              pHybOutImag = &pHybOutImagDry[0];
              if (0 == resHybIndex) {
                pHybOutReal = &pHybOutRealWet[0];
                pHybOutImag = &pHybOutImagWet[0];
              }
              pHybOutReal[0] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWImag[0], pKernel[0]), scale_param_m2);
              pHybOutImag[0] -= SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWReal[0], pKernel[0]), scale_param_m2);

              if (1 == resHybIndex) {
                pHybOutReal = &pHybOutRealWet[0];
                pHybOutImag = &pHybOutImagWet[0];
              }
              pHybOutReal[1] -= SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWImag[1], pKernel[1]), scale_param_m2);
              pHybOutImag[1] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWReal[1], pKernel[1]), scale_param_m2);

              if (2 == resHybIndex) {
                pHybOutReal = &pHybOutRealWet[0];
                pHybOutImag = &pHybOutImagWet[0];
              }
              pHybOutReal[2] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWImag[2], pKernel[2]), scale_param_m2);
              pHybOutImag[2] -= SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWReal[2], pKernel[2]), scale_param_m2);
            }

            for (qs = 3; qs < resHybIndex; qs++) {
              pHybOutRealDry[qs] -= SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWImag[qs], pKernel[qs]), scale_param_m2);
              pHybOutImagDry[qs] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWReal[qs], pKernel[qs]), scale_param_m2);
            }
            /* decor signals */
            for (; qs < complexHybBands; qs++) {
              pHybOutRealWet[qs] -= SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWImag[qs], pKernel[qs]), scale_param_m2);
              pHybOutImagWet[qs] += SAC_DEC_APPLY_M2_SCALE(
                  fMultDiv2(pWReal[qs], pKernel[qs]), scale_param_m2);
            }
          } /* self->upmixType */
        }   /* if (activParamBands) { */
      }     /*  self->numVChannels */

      if (self->phaseCoding == 3) {
        scaleValuesSaturate(pHybOutRealDry, complexHybBands,
                            SCALE_PARAM_M2_212_PRED + SCALE_DATA_APPLY_M2_PC);
        scaleValuesSaturate(pHybOutImagDry, complexHybBands,
                            SCALE_PARAM_M2_212_PRED + SCALE_DATA_APPLY_M2_PC);

        if (!toolsDisabled) {
          scaleValuesSaturate(pHybOutRealWet, complexHybBands,
                              SCALE_PARAM_M2_212_PRED + SCALE_DATA_APPLY_M2_PC);
          scaleValuesSaturate(pHybOutImagWet, complexHybBands,
                              SCALE_PARAM_M2_212_PRED + SCALE_DATA_APPLY_M2_PC);
        }
      }
    }

    C_ALLOC_SCRATCH_END(pKernel, FIXP_SGL, MAX_HYBRID_BANDS);
  }

  return err;
}

SACDEC_ERROR SpatialDecSynthesis(spatialDec *self, const int32_t ts,
                                 int32_t **hybOutputReal,
                                 int32_t **hybOutputImag, PCM_MPS *timeOut,
                                 const int32_t numInputChannels,
                                 const FDK_channelMapDescr *const mapDescr) {
  SACDEC_ERROR err = MPS_OK;

  int32_t ch;
  int32_t stride, offset;

  stride = self->numOutputChannelsAT;
  offset = 1;

  PCM_MPS *pTimeOut__FDK =
      &timeOut[stride * self->pQmfDomain->globalConf.nBandsSynthesis * ts];
  C_ALLOC_SCRATCH_START(pQmfReal, int32_t, QMF_MAX_SYNTHESIS_BANDS);
  C_ALLOC_SCRATCH_START(pQmfImag, int32_t, QMF_MAX_SYNTHESIS_BANDS);

  for (ch = 0; ch < self->numOutputChannelsAT; ch++) {
    if (self->pConfigCurrent->syntaxFlags & SACDEC_SYNTAX_LD) {
      int32_t k;
      /* No hybrid filtering. Just copy the QMF data. */
      for (k = 0; k < self->hybridBands; k += 1) {
        pQmfReal[k] = hybOutputReal[ch][k];
        pQmfImag[k] = hybOutputImag[ch][k];
      }
    } else {
      FDKhybridSynthesisApply(&self->hybridSynthesis[ch], hybOutputReal[ch],
                              hybOutputImag[ch], pQmfReal, pQmfImag);
    }

    /* Map channel indices from MPEG Surround -> PCE style -> channelMapping[]
     */
    assert(self->numOutputChannelsAT <= 6);
    int32_t outCh = FDK_chMapDescr_getMapValue(mapDescr, mapChannel(self, ch),
                                           self->numOutputChannelsAT);

    {
      if (self->stereoConfigIndex == 3) {
        /* MPS -> SBR */
        int32_t i;
        int32_t *pWorkBufReal, *pWorkBufImag;
        assert((self->pQmfDomain->QmfDomainOut[outCh].fb.outGain_m ==
                    (int32_t)0x80000000) &&
                   (self->pQmfDomain->QmfDomainOut[outCh].fb.outGain_e == 0));
        FDK_QmfDomain_GetWorkBuffer(&self->pQmfDomain->QmfDomainIn[outCh], ts,
                                    &pWorkBufReal, &pWorkBufImag);
        assert(self->qmfBands <=
                   self->pQmfDomain->QmfDomainIn[outCh].workBuf_nBands);
        for (i = 0; i < self->qmfBands; i++) {
          pWorkBufReal[i] = pQmfReal[i];
          pWorkBufImag[i] = pQmfImag[i];
        }
        self->pQmfDomain->QmfDomainIn[outCh].scaling.lb_scale =
            -7; /*-ALGORITHMIC_SCALING_IN_ANALYSIS_FILTERBANK;*/
        self->pQmfDomain->QmfDomainIn[outCh].scaling.lb_scale -=
            self->pQmfDomain->QmfDomainIn[outCh].fb.filterScale;
        self->pQmfDomain->QmfDomainIn[outCh].scaling.lb_scale -=
            self->clipProtectGainSF__FDK;

        self->pQmfDomain->QmfDomainIn[outCh].scaling.lb_scale -= (1);
      } else {
        /* Call the QMF synthesis for dry. */
        err = CalculateSpaceSynthesisQmf(&self->pQmfDomain->QmfDomainOut[outCh],
                                         pQmfReal, pQmfImag, stride,
                                         pTimeOut__FDK + (offset * outCh));
      }
      if (err != MPS_OK) goto bail;
    }
  } /* ch loop */

bail:
  C_ALLOC_SCRATCH_END(pQmfImag, int32_t, QMF_MAX_SYNTHESIS_BANDS);
  C_ALLOC_SCRATCH_END(pQmfReal, int32_t, QMF_MAX_SYNTHESIS_BANDS);

  return err;
}

void SpatialDecBufferMatrices(spatialDec *self) {
  int32_t row, col;
  int32_t complexParBands;
  complexParBands = self->numParameterBands;

  /*
    buffer matrices M2
  */
  for (row = 0; row < self->numM2rows; row++) {
    for (col = 0; col < self->numVChannels; col++) {
      FDKmemcpy(self->M2RealPrev__FDK[row][col], self->M2Real__FDK[row][col],
                self->numParameterBands * sizeof(int32_t));
      if (0 || (self->phaseCoding == 3)) {
        FDKmemcpy(self->M2ImagPrev__FDK[row][col], self->M2Imag__FDK[row][col],
                  complexParBands * sizeof(int32_t));
      }
    }
  }

  /* buffer phase */
  FDKmemcpy(self->PhasePrevLeft__FDK, self->PhaseLeft__FDK,
            self->numParameterBands * sizeof(int32_t));
  FDKmemcpy(self->PhasePrevRight__FDK, self->PhaseRight__FDK,
            self->numParameterBands * sizeof(int32_t));
}

#define PHASE_SCALE 2

#ifndef P_PI
#define P_PI 3.1415926535897932
#endif

/* For better precision, PI (pi_x2) is already doubled */
static int32_t interp_angle__FDK(int32_t angle1, int32_t angle2,
                                  FIXP_SGL alpha, int32_t pi_x2) {
  if (angle2 - angle1 > (pi_x2 >> 1)) angle2 -= pi_x2;

  if (angle1 - angle2 > (pi_x2 >> 1)) angle1 -= pi_x2;

  return interpolateParameter(alpha, angle2, angle1);
}

/*
 *
 */
void SpatialDecApplyPhase(spatialDec *self, FIXP_SGL alpha__FDK,
                          int32_t lastSlotOfParamSet) {
  int32_t pb, qs;
  int32_t ppb[MAX_PARAMETER_BANDS *
               4]; /* left real, imag - right real, imag interleaved */

  const int32_t pi_x2 = PIx2__IPD;
  for (pb = 0; pb < self->numParameterBands; pb++) {
    int32_t pl, pr;

    pl = interp_angle__FDK(self->PhasePrevLeft__FDK[pb],
                           self->PhaseLeft__FDK[pb], alpha__FDK, pi_x2);
    pr = interp_angle__FDK(self->PhasePrevRight__FDK[pb],
                           self->PhaseRight__FDK[pb], alpha__FDK, pi_x2);

    inline_fixp_cos_sin(pl, pr, IPD_SCALE, &ppb[4 * pb]);
  }

  /* sign is -1 for qs = 0,2 and +1 for qs = 1 */

  const int8_t *kernels = &self->kernels[0];

  int32_t *Dry_real0 = &self->hybOutputRealDry__FDK[0][0];
  int32_t *Dry_imag0 = &self->hybOutputImagDry__FDK[0][0];
  int32_t *Dry_real1 = &self->hybOutputRealDry__FDK[1][0];
  int32_t *Dry_imag1 = &self->hybOutputImagDry__FDK[1][0];

  for (qs = 2; qs >= 0; qs--) {
    int32_t out_re, out_im;

    pb = *kernels++;
    if (qs == 1) /* sign[qs] >= 0 */
    {
      cplxMultDiv2(&out_re, &out_im, *Dry_real0, *Dry_imag0, ppb[4 * pb + 0],
                   ppb[4 * pb + 1]);
      out_re <<= PHASE_SCALE - 1;
      out_im <<= PHASE_SCALE - 1;
      *Dry_real0++ = out_re;
      *Dry_imag0++ = out_im;

      cplxMultDiv2(&out_re, &out_im, *Dry_real1, *Dry_imag1, ppb[4 * pb + 2],
                   ppb[4 * pb + 3]);
      out_re <<= PHASE_SCALE - 1;
      out_im <<= PHASE_SCALE - 1;
      *Dry_real1++ = out_re;
      *Dry_imag1++ = out_im;
    } else {
      cplxMultDiv2(&out_re, &out_im, *Dry_real0, *Dry_imag0, ppb[4 * pb + 0],
                   -ppb[4 * pb + 1]);
      out_re <<= PHASE_SCALE - 1;
      out_im <<= PHASE_SCALE - 1;
      *Dry_real0++ = out_re;
      *Dry_imag0++ = out_im;

      cplxMultDiv2(&out_re, &out_im, *Dry_real1, *Dry_imag1, ppb[4 * pb + 2],
                   -ppb[4 * pb + 3]);
      out_re <<= PHASE_SCALE - 1;
      out_im <<= PHASE_SCALE - 1;
      *Dry_real1++ = out_re;
      *Dry_imag1++ = out_im;
    }
  }

  /* sign is +1 for qs >=3 */
  for (qs = self->hybridBands - 3; qs--;) {
    int32_t out_re, out_im;

    pb = *kernels++;
    cplxMultDiv2(&out_re, &out_im, *Dry_real0, *Dry_imag0, ppb[4 * pb + 0],
                 ppb[4 * pb + 1]);
    out_re <<= PHASE_SCALE - 1;
    out_im <<= PHASE_SCALE - 1;
    *Dry_real0++ = out_re;
    *Dry_imag0++ = out_im;

    cplxMultDiv2(&out_re, &out_im, *Dry_real1, *Dry_imag1, ppb[4 * pb + 2],
                 ppb[4 * pb + 3]);
    out_re <<= PHASE_SCALE - 1;
    out_im <<= PHASE_SCALE - 1;
    *Dry_real1++ = out_re;
    *Dry_imag1++ = out_im;
  }
}
