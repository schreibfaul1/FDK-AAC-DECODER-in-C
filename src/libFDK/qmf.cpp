

/******************* Library for basic calculation routines ********************

   Author(s):   Markus Lohwasser, Josef Hoepfl, Manuel Jander

   Description: QMF filterbank

*******************************************************************************/

/*!
  \file
  \brief  Complex qmf analysis/synthesis
  This module contains the qmf filterbank for analysis [
  cplxAnalysisQmfFiltering() ] and synthesis [ cplxSynthesisQmfFiltering() ]. It
  is a polyphase implementation of a complex exponential modulated filter bank.
  The analysis part usually runs at half the sample rate than the synthesis
  part. (So called "dual-rate" mode.)

  The coefficients of the prototype filter are specified in #qmf_pfilt640 (in
  sbr_rom.cpp). Thus only a 64 channel version (32 on the analysis side) with a
  640 tap prototype filter are used.

  \anchor PolyphaseFiltering <h2>About polyphase filtering</h2>
  The polyphase implementation of a filterbank requires filtering at the input
  and output. This is implemented as part of cplxAnalysisQmfFiltering() and
  cplxSynthesisQmfFiltering(). The implementation requires the filter
  coefficients in a specific structure as described in #sbr_qmf_64_640_qmf (in
  sbr_rom.cpp).

  This module comprises the computationally most expensive functions of the SBR
  decoder. The accuracy of computations is also important and has a direct
  impact on the overall sound quality. Therefore a special test program is
  available which can be used to only test the filterbank: main_audio.cpp

  This modules also uses scaling of data to provide better SNR on fixed-point
  processors. See #QMF_SCALE_FACTOR (in sbr_scale.h) for details. An interesting
  note: The function getScalefactor() can constitute a significant amount of
  computational complexity - very much depending on the bitrate. Since it is a
  rather small function, effective assembler optimization might be possible.

*/
#include <memory.h>

#include "../libAACdec/newAACDecoder.h"
#include "qmf.h"

#include "FDK_trigFcts.h"
#include "fixpoint_math.h"
#include "dct.h"

#define QSSCALE (0)
#define FX_DBL2FX_QSS(x) (x)
#define FX_QSS2FX_DBL(x) (x)

/* moved to qmf_pcm.h: -> qmfSynPrototypeFirSlot */
/* moved to qmf_pcm.h: -> qmfSynPrototypeFirSlot_NonSymmetric */
/* moved to qmf_pcm.h: -> qmfSynthesisFilteringSlot */

/*!
 *
 * \brief Perform real-valued forward modulation of the time domain
 *        data of timeIn and stores the real part of the subband
 *        samples in rSubband
 *
 */
static void qmfForwardModulationLP_even(
    HANDLE_QMF_FILTER_BANK anaQmf, /*!< Handle of Qmf Analysis Bank  */
    int32_t *timeIn,              /*!< Time Signal */
    int32_t *rSubband)            /*!< Real Output */
{
  int32_t i;
  int32_t L = anaQmf->no_channels;
  int32_t M = L >> 1;
  int32_t scale = 0;
  int32_t accu;

  const int32_t *timeInTmp1 = (int32_t *)&timeIn[3 * M];
  const int32_t *timeInTmp2 = timeInTmp1;
  int32_t *rSubbandTmp = rSubband;

  rSubband[0] = timeIn[3 * M] >> 1;

  for (i = M - 1; i != 0; i--) {
    accu = ((*--timeInTmp1) >> 1) + ((*++timeInTmp2) >> 1);
    *++rSubbandTmp = accu;
  }

  timeInTmp1 = &timeIn[2 * M];
  timeInTmp2 = &timeIn[0];
  rSubbandTmp = &rSubband[M];

  for (i = L - M; i != 0; i--) {
    accu = ((*timeInTmp1--) >> 1) - ((*timeInTmp2++) >> 1);
    *rSubbandTmp++ = accu;
  }

  dct_III(rSubband, timeIn, L, &scale);
}

#if !defined(FUNCTION_qmfForwardModulationLP_odd)
static void qmfForwardModulationLP_odd(
    HANDLE_QMF_FILTER_BANK anaQmf, /*!< Handle of Qmf Analysis Bank  */
    const int32_t *timeIn,        /*!< Time Signal */
    int32_t *rSubband)            /*!< Real Output */
{
  int32_t i;
  int32_t L = anaQmf->no_channels;
  int32_t M = L >> 1;
  int32_t shift = (anaQmf->no_channels >> 6) + 1;

  for (i = 0; i < M; i++) {
    rSubband[M + i] = (timeIn[L - 1 - i] >> 1) - (timeIn[i] >> shift);
    rSubband[M - 1 - i] =
        (timeIn[L + i] >> 1) + (timeIn[2 * L - 1 - i] >> shift);
  }

  dct_IV(rSubband, L, &shift);
}
#endif /* !defined(FUNCTION_qmfForwardModulationLP_odd) */

/*!
 *
 * \brief Perform complex-valued forward modulation of the time domain
 *        data of timeIn and stores the real part of the subband
 *        samples in rSubband, and the imaginary part in iSubband
 *
 *
 */
#if !defined(FUNCTION_qmfForwardModulationHQ)
static void qmfForwardModulationHQ(
    HANDLE_QMF_FILTER_BANK anaQmf,   /*!< Handle of Qmf Analysis Bank  */
    const int32_t * timeIn, /*!< Time Signal */
    int32_t * rSubband,     /*!< Real Output */
    int32_t * iSubband      /*!< Imaginary Output */
) {
  int32_t i;
  int32_t L = anaQmf->no_channels;
  int32_t L2 = L << 1;
  int32_t shift = 0;

  /* Time advance by one sample, which is equivalent to the complex
     rotation at the end of the analysis. Works only for STD mode. */
  if ((L == 64) && !(anaQmf->flags & (QMF_FLAG_CLDFB | QMF_FLAG_MPSLDFB))) {
    int32_t x, y;

    /*rSubband[0] = u[1] + u[0]*/
    /*iSubband[0] = u[1] - u[0]*/
    x = timeIn[1] >> 1;
    y = timeIn[0];
    rSubband[0] = x + (y >> 1);
    iSubband[0] = x - (y >> 1);

    /*rSubband[n] = u[n+1] - u[2M-n], n=1,...,M-1*/
    /*iSubband[n] = u[n+1] + u[2M-n], n=1,...,M-1*/
    for (i = 1; i < L; i++) {
      x = timeIn[i + 1] >> 1; /*u[n+1]  */
      y = timeIn[L2 - i];     /*u[2M-n] */
      rSubband[i] = x - (y >> 1);
      iSubband[i] = x + (y >> 1);
    }
  } else {
    for (i = 0; i < L; i += 2) {
      int32_t x0, x1, y0, y1;

      x0 = timeIn[i + 0] >> 1;
      x1 = timeIn[i + 1] >> 1;
      y0 = timeIn[L2 - 1 - i];
      y1 = timeIn[L2 - 2 - i];

      rSubband[i + 0] = x0 - (y0 >> 1);
      rSubband[i + 1] = x1 - (y1 >> 1);
      iSubband[i + 0] = x0 + (y0 >> 1);
      iSubband[i + 1] = x1 + (y1 >> 1);
    }
  }

  dct_IV(rSubband, L, &shift);
  dst_IV(iSubband, L, &shift);

  /* Do the complex rotation except for the case of 64 bands (in STD mode). */
  if ((L != 64) || (anaQmf->flags & (QMF_FLAG_CLDFB | QMF_FLAG_MPSLDFB))) {
    if (anaQmf->flags & QMF_FLAG_MPSLDFB_OPTIMIZE_MODULATION) {
      int32_t iBand;
      for (i = 0; i < fMin(anaQmf->lsb, L); i += 2) {
        iBand = rSubband[i];
        rSubband[i] = -iSubband[i];
        iSubband[i] = iBand;

        iBand = -rSubband[i + 1];
        rSubband[i + 1] = iSubband[i + 1];
        iSubband[i + 1] = iBand;
      }
    } else {
      const FIXP_QTW *sbr_t_cos;
      const FIXP_QTW *sbr_t_sin;
      const int32_t len = L; /* was len = fMin(anaQmf->lsb, L) but in case of USAC
                            the signal above lsb is actually needed in some
                            cases (HBE?) */
      sbr_t_cos = anaQmf->t_cos;
      sbr_t_sin = anaQmf->t_sin;

      for (i = 0; i < len; i++) {
        cplxMult(&iSubband[i], &rSubband[i], iSubband[i], rSubband[i],
                 sbr_t_cos[i], sbr_t_sin[i]);
      }
    }
  }
}
#endif /* FUNCTION_qmfForwardModulationHQ */

/*!
 *
 * \brief Perform low power inverse modulation of the subband
 *        samples stored in rSubband (real part) and iSubband (imaginary
 *        part) and stores the result in pWorkBuffer.
 *
 */
inline static void qmfInverseModulationLP_even(
    HANDLE_QMF_FILTER_BANK synQmf, /*!< Handle of Qmf Synthesis Bank  */
    const int32_t *qmfReal, /*!< Pointer to qmf real subband slot (input) */
    const int32_t scaleFactorLowBand,  /*!< Scalefactor for Low band */
    const int32_t scaleFactorHighBand, /*!< Scalefactor for High band */
    int32_t *pTimeOut             /*!< Pointer to qmf subband slot (output)*/
) {
  int32_t i;
  int32_t L = synQmf->no_channels;
  int32_t M = L >> 1;
  int32_t scale = 0;
  int32_t tmp;
  int32_t * tReal = pTimeOut;
  int32_t * tImag = pTimeOut + L;

  /* Move input to output vector with offset */
  scaleValuesSaturate(&tReal[0], &qmfReal[0], synQmf->lsb, scaleFactorLowBand);
  scaleValuesSaturate(&tReal[0 + synQmf->lsb], &qmfReal[0 + synQmf->lsb],
                      synQmf->usb - synQmf->lsb, scaleFactorHighBand);
  memset(&tReal[0 + synQmf->usb], 0, (L - synQmf->usb) * sizeof(int32_t));

  /* Dct type-2 transform */
  dct_II(tReal, tImag, L, &scale);

  /* Expand output and replace inplace the output buffers */
  tImag[0] = tReal[M];
  tImag[M] = (int32_t)0;
  tmp = tReal[0];
  tReal[0] = tReal[M];
  tReal[M] = tmp;

  for (i = 1; i < M / 2; i++) {
    /* Imag */
    tmp = tReal[L - i];
    tImag[M - i] = tmp;
    tImag[i + M] = -tmp;

    tmp = tReal[M + i];
    tImag[i] = tmp;
    tImag[L - i] = -tmp;

    /* Real */
    tReal[M + i] = tReal[i];
    tReal[L - i] = tReal[M - i];
    tmp = tReal[i];
    tReal[i] = tReal[M - i];
    tReal[M - i] = tmp;
  }
  /* Remaining odd terms */
  tmp = tReal[M + M / 2];
  tImag[M / 2] = tmp;
  tImag[M / 2 + M] = -tmp;

  tReal[M + M / 2] = tReal[M / 2];
}

inline static void qmfInverseModulationLP_odd(
    HANDLE_QMF_FILTER_BANK synQmf, /*!< Handle of Qmf Synthesis Bank  */
    const int32_t *qmfReal, /*!< Pointer to qmf real subband slot (input) */
    const int32_t scaleFactorLowBand,  /*!< Scalefactor for Low band */
    const int32_t scaleFactorHighBand, /*!< Scalefactor for High band */
    int32_t *pTimeOut             /*!< Pointer to qmf subband slot (output)*/
) {
  int32_t i;
  int32_t L = synQmf->no_channels;
  int32_t M = L >> 1;
  int32_t shift = 0;

  /* Move input to output vector with offset */
  scaleValuesSaturate(pTimeOut + M, qmfReal, synQmf->lsb, scaleFactorLowBand);
  scaleValuesSaturate(pTimeOut + M + synQmf->lsb, qmfReal + synQmf->lsb,
                      synQmf->usb - synQmf->lsb, scaleFactorHighBand);
  memset(pTimeOut + M + synQmf->usb, 0, (L - synQmf->usb) * sizeof(int32_t));

  dct_IV(pTimeOut + M, L, &shift);
  for (i = 0; i < M; i++) {
    pTimeOut[i] = pTimeOut[L - 1 - i];
    pTimeOut[2 * L - 1 - i] = -pTimeOut[L + i];
  }
}

#ifndef FUNCTION_qmfInverseModulationHQ
/*!
 *
 * \brief Perform complex-valued inverse modulation of the subband
 *        samples stored in rSubband (real part) and iSubband (imaginary
 *        part) and stores the result in pWorkBuffer.
 *
 */
inline static void qmfInverseModulationHQ(
    HANDLE_QMF_FILTER_BANK synQmf, /*!< Handle of Qmf Synthesis Bank     */
    const int32_t *qmfReal,       /*!< Pointer to qmf real subband slot */
    const int32_t *qmfImag,       /*!< Pointer to qmf imag subband slot */
    const int32_t scaleFactorLowBand,  /*!< Scalefactor for Low band         */
    const int32_t scaleFactorHighBand, /*!< Scalefactor for High band        */
    int32_t *pWorkBuffer          /*!< WorkBuffer (output)              */
) {
  int32_t i;
  int32_t L = synQmf->no_channels;
  int32_t M = L >> 1;
  int32_t shift = 0;
  int32_t * tReal = pWorkBuffer;
  int32_t * tImag = pWorkBuffer + L;

  if (synQmf->flags & QMF_FLAG_CLDFB) {
    for (i = 0; i < synQmf->usb; i++) {
      cplxMultDiv2(&tImag[i], &tReal[i], qmfImag[i], qmfReal[i],
                   synQmf->t_cos[i], synQmf->t_sin[i]);
    }
    scaleValuesSaturate(&tReal[0], synQmf->lsb, scaleFactorLowBand + 1);
    scaleValuesSaturate(&tReal[0 + synQmf->lsb], synQmf->usb - synQmf->lsb,
                        scaleFactorHighBand + 1);
    scaleValuesSaturate(&tImag[0], synQmf->lsb, scaleFactorLowBand + 1);
    scaleValuesSaturate(&tImag[0 + synQmf->lsb], synQmf->usb - synQmf->lsb,
                        scaleFactorHighBand + 1);
  }

  if ((synQmf->flags & QMF_FLAG_CLDFB) == 0) {
    scaleValuesSaturate(&tReal[0], &qmfReal[0], synQmf->lsb,
                        scaleFactorLowBand);
    scaleValuesSaturate(&tReal[0 + synQmf->lsb], &qmfReal[0 + synQmf->lsb],
                        synQmf->usb - synQmf->lsb, scaleFactorHighBand);
    scaleValuesSaturate(&tImag[0], &qmfImag[0], synQmf->lsb,
                        scaleFactorLowBand);
    scaleValuesSaturate(&tImag[0 + synQmf->lsb], &qmfImag[0 + synQmf->lsb],
                        synQmf->usb - synQmf->lsb, scaleFactorHighBand);
  }

  memset(&tReal[synQmf->usb], 0,
              (synQmf->no_channels - synQmf->usb) * sizeof(int32_t));
  memset(&tImag[synQmf->usb], 0,
              (synQmf->no_channels - synQmf->usb) * sizeof(int32_t));

  dct_IV(tReal, L, &shift);
  dst_IV(tImag, L, &shift);

  if (synQmf->flags & QMF_FLAG_CLDFB) {
    for (i = 0; i < M; i++) {
      int32_t r1, i1, r2, i2;
      r1 = tReal[i];
      i2 = tImag[L - 1 - i];
      r2 = tReal[L - i - 1];
      i1 = tImag[i];

      tReal[i] = (r1 - i1) >> 1;
      tImag[L - 1 - i] = -(r1 + i1) >> 1;
      tReal[L - i - 1] = (r2 - i2) >> 1;
      tImag[i] = -(r2 + i2) >> 1;
    }
  } else {
    /* The array accesses are negative to compensate the missing minus sign in
     * the low and hi band gain. */
    /* 26 cycles on ARM926 */
    for (i = 0; i < M; i++) {
      int32_t r1, i1, r2, i2;
      r1 = -tReal[i];
      i2 = -tImag[L - 1 - i];
      r2 = -tReal[L - i - 1];
      i1 = -tImag[i];

      tReal[i] = (r1 - i1) >> 1;
      tImag[L - 1 - i] = -(r1 + i1) >> 1;
      tReal[L - i - 1] = (r2 - i2) >> 1;
      tImag[i] = -(r2 + i2) >> 1;
    }
  }
}
#endif /* #ifndef FUNCTION_qmfInverseModulationHQ */

/*!
 *
 * \brief Create QMF filter bank instance
 *
 * \return 0 if successful
 *
 */
static int32_t qmfInitFilterBank(
    HANDLE_QMF_FILTER_BANK h_Qmf, /*!< Handle to return */
    void *pFilterStates,          /*!< Handle to filter states */
    int32_t noCols,                   /*!< Number of timeslots per frame */
    int32_t lsb,                      /*!< Lower end of QMF frequency range */
    int32_t usb,                      /*!< Upper end of QMF frequency range */
    int32_t no_channels,              /*!< Number of channels (bands) */
    uint32_t flags,                   /*!< flags */
    int32_t synflag)                  /*!< 1: synthesis; 0: analysis */
{
  memset(h_Qmf, 0, sizeof(QMF_FILTER_BANK));

  if (flags & QMF_FLAG_MPSLDFB) {
    flags |= QMF_FLAG_NONSYMMETRIC;
    flags |= QMF_FLAG_MPSLDFB_OPTIMIZE_MODULATION;

    h_Qmf->t_cos = NULL;
    h_Qmf->t_sin = NULL;
    h_Qmf->filterScale = QMF_MPSLDFB_PFT_SCALE;
    h_Qmf->p_stride = 1;

    switch (no_channels) {
      case 64:
        h_Qmf->p_filter = qmf_mpsldfb_640;
        h_Qmf->FilterSize = 640;
        break;
      case 32:
        h_Qmf->p_filter = qmf_mpsldfb_320;
        h_Qmf->FilterSize = 320;
        break;
      default:
        return -1;
    }
  }

  if (!(flags & QMF_FLAG_MPSLDFB) && (flags & QMF_FLAG_CLDFB)) {
    flags |= QMF_FLAG_NONSYMMETRIC;
    h_Qmf->filterScale = QMF_CLDFB_PFT_SCALE;

    h_Qmf->p_stride = 1;
    switch (no_channels) {
      case 64:
        h_Qmf->t_cos = qmf_phaseshift_cos64_cldfb;
        h_Qmf->t_sin = qmf_phaseshift_sin64_cldfb;
        h_Qmf->p_filter = qmf_cldfb_640;
        h_Qmf->FilterSize = 640;
        break;
      case 32:
        h_Qmf->t_cos = (synflag) ? qmf_phaseshift_cos32_cldfb_syn
                                 : qmf_phaseshift_cos32_cldfb_ana;
        h_Qmf->t_sin = qmf_phaseshift_sin32_cldfb;
        h_Qmf->p_filter = qmf_cldfb_320;
        h_Qmf->FilterSize = 320;
        break;
      case 16:
        h_Qmf->t_cos = (synflag) ? qmf_phaseshift_cos16_cldfb_syn
                                 : qmf_phaseshift_cos16_cldfb_ana;
        h_Qmf->t_sin = qmf_phaseshift_sin16_cldfb;
        h_Qmf->p_filter = qmf_cldfb_160;
        h_Qmf->FilterSize = 160;
        break;
      case 8:
        h_Qmf->t_cos = (synflag) ? qmf_phaseshift_cos8_cldfb_syn
                                 : qmf_phaseshift_cos8_cldfb_ana;
        h_Qmf->t_sin = qmf_phaseshift_sin8_cldfb;
        h_Qmf->p_filter = qmf_cldfb_80;
        h_Qmf->FilterSize = 80;
        break;
      default:
        return -1;
    }
  }

  if (!(flags & QMF_FLAG_MPSLDFB) && ((flags & QMF_FLAG_CLDFB) == 0)) {
    switch (no_channels) {
      case 64:
        h_Qmf->p_filter = qmf_pfilt640;
        h_Qmf->t_cos = qmf_phaseshift_cos64;
        h_Qmf->t_sin = qmf_phaseshift_sin64;
        h_Qmf->p_stride = 1;
        h_Qmf->FilterSize = 640;
        h_Qmf->filterScale = 0;
        break;
      case 40:
        if (synflag) {
          break;
        } else {
          h_Qmf->p_filter = qmf_pfilt400; /* Scaling factor 0.8 */
          h_Qmf->t_cos = qmf_phaseshift_cos40;
          h_Qmf->t_sin = qmf_phaseshift_sin40;
          h_Qmf->filterScale = 1;
          h_Qmf->p_stride = 1;
          h_Qmf->FilterSize = no_channels * 10;
        }
        break;
      case 32:
        h_Qmf->p_filter = qmf_pfilt640;
        if (flags & QMF_FLAG_DOWNSAMPLED) {
          h_Qmf->t_cos = qmf_phaseshift_cos_downsamp32;
          h_Qmf->t_sin = qmf_phaseshift_sin_downsamp32;
        } else {
          h_Qmf->t_cos = qmf_phaseshift_cos32;
          h_Qmf->t_sin = qmf_phaseshift_sin32;
        }
        h_Qmf->p_stride = 2;
        h_Qmf->FilterSize = 640;
        h_Qmf->filterScale = 0;
        break;
      case 20:
        h_Qmf->p_filter = qmf_pfilt200;
        h_Qmf->p_stride = 1;
        h_Qmf->FilterSize = 200;
        h_Qmf->filterScale = 0;
        break;
      case 12:
        h_Qmf->p_filter = qmf_pfilt120;
        h_Qmf->p_stride = 1;
        h_Qmf->FilterSize = 120;
        h_Qmf->filterScale = 0;
        break;
      case 8:
        h_Qmf->p_filter = qmf_pfilt640;
        h_Qmf->p_stride = 8;
        h_Qmf->FilterSize = 640;
        h_Qmf->filterScale = 0;
        break;
      case 16:
        h_Qmf->p_filter = qmf_pfilt640;
        h_Qmf->t_cos = qmf_phaseshift_cos16;
        h_Qmf->t_sin = qmf_phaseshift_sin16;
        h_Qmf->p_stride = 4;
        h_Qmf->FilterSize = 640;
        h_Qmf->filterScale = 0;
        break;
      case 24:
        h_Qmf->p_filter = qmf_pfilt240;
        h_Qmf->t_cos = qmf_phaseshift_cos24;
        h_Qmf->t_sin = qmf_phaseshift_sin24;
        h_Qmf->p_stride = 1;
        h_Qmf->FilterSize = 240;
        h_Qmf->filterScale = 1;
        break;
      default:
        return -1;
    }
  }

  h_Qmf->synScalefactor = h_Qmf->filterScale;
  // DCT|DST dependency
  switch (no_channels) {
    case 128:
      h_Qmf->synScalefactor += ALGORITHMIC_SCALING_IN_SYNTHESIS_FILTERBANK + 1;
      break;
    case 40: {
      h_Qmf->synScalefactor += ALGORITHMIC_SCALING_IN_SYNTHESIS_FILTERBANK - 1;
    } break;
    case 64:
      h_Qmf->synScalefactor += ALGORITHMIC_SCALING_IN_SYNTHESIS_FILTERBANK;
      break;
    case 8:
      h_Qmf->synScalefactor += ALGORITHMIC_SCALING_IN_SYNTHESIS_FILTERBANK - 3;
      break;
    case 12:
      h_Qmf->synScalefactor += ALGORITHMIC_SCALING_IN_SYNTHESIS_FILTERBANK;
      break;
    case 20:
      h_Qmf->synScalefactor += ALGORITHMIC_SCALING_IN_SYNTHESIS_FILTERBANK + 1;
      break;
    case 32:
      h_Qmf->synScalefactor += ALGORITHMIC_SCALING_IN_SYNTHESIS_FILTERBANK - 1;
      break;
    case 16:
      h_Qmf->synScalefactor += ALGORITHMIC_SCALING_IN_SYNTHESIS_FILTERBANK - 2;
      break;
    case 24:
      h_Qmf->synScalefactor += ALGORITHMIC_SCALING_IN_SYNTHESIS_FILTERBANK - 1;
      break;
    default:
      return -1;
  }

  h_Qmf->flags = flags;

  h_Qmf->no_channels = no_channels;
  h_Qmf->no_col = noCols;

  h_Qmf->lsb = fMin(lsb, h_Qmf->no_channels);
  h_Qmf->usb = synflag
                   ? fMin(usb, h_Qmf->no_channels)
                   : usb; /* was: h_Qmf->usb = fMin(usb, h_Qmf->no_channels); */

  h_Qmf->FilterStates = (void *)pFilterStates;

  h_Qmf->outScalefactor =
      (ALGORITHMIC_SCALING_IN_ANALYSIS_FILTERBANK + h_Qmf->filterScale) +
      h_Qmf->synScalefactor;

  h_Qmf->outGain_m =
      (int32_t)0x80000000; /* default init value will be not applied */
  h_Qmf->outGain_e = 0;

  return (0);
}

/*!
 *
 * \brief Adjust synthesis qmf filter states
 *
 * \return void
 *
 */
static inline void qmfAdaptFilterStates(
    HANDLE_QMF_FILTER_BANK synQmf, /*!< Handle of Qmf Filter Bank */
    int32_t scaleFactorDiff)           /*!< Scale factor difference to be applied */
{
  if (synQmf == NULL || synQmf->FilterStates == NULL) {
    return;
  }
  if (scaleFactorDiff > 0) {
    scaleValuesSaturate((FIXP_QSS *)synQmf->FilterStates,
                        synQmf->no_channels * (QMF_NO_POLY * 2 - 1),
                        scaleFactorDiff);
  } else {
    scaleValues((FIXP_QSS *)synQmf->FilterStates,
                synQmf->no_channels * (QMF_NO_POLY * 2 - 1), scaleFactorDiff);
  }
}

/*!
 *
 * \brief Create QMF filter bank instance
 *
 *
 * \return 0 if succesful
 *
 */
int32_t qmfInitSynthesisFilterBank(
    HANDLE_QMF_FILTER_BANK h_Qmf, /*!< Returns handle */
    FIXP_QSS *pFilterStates,      /*!< Handle to filter states */
    int32_t noCols,                   /*!< Number of timeslots per frame */
    int32_t lsb,                      /*!< lower end of QMF */
    int32_t usb,                      /*!< upper end of QMF */
    int32_t no_channels,              /*!< Number of channels (bands) */
    int32_t flags)                    /*!< Low Power flag */
{
  int32_t oldOutScale = h_Qmf->outScalefactor;
  int32_t err = qmfInitFilterBank(h_Qmf, pFilterStates, noCols, lsb, usb,
                              no_channels, flags, 1);
  if (h_Qmf->FilterStates != NULL) {
    if (!(flags & QMF_FLAG_KEEP_STATES)) {
      memset(
          h_Qmf->FilterStates, 0,
          (2 * QMF_NO_POLY - 1) * h_Qmf->no_channels * sizeof(FIXP_QSS));
    } else {
      qmfAdaptFilterStates(h_Qmf, oldOutScale - h_Qmf->outScalefactor);
    }
  }

  assert(h_Qmf->no_channels >= h_Qmf->lsb);
  assert(h_Qmf->no_channels >= h_Qmf->usb);

  return err;
}

/*!
 *
 * \brief Change scale factor for output data and adjust qmf filter states
 *
 * \return void
 *
 */
void qmfChangeOutScalefactor(
    HANDLE_QMF_FILTER_BANK synQmf, /*!< Handle of Qmf Synthesis Bank */
    int32_t outScalefactor             /*!< New scaling factor for output data */
) {
  if (synQmf == NULL) {
    return;
  }

  /* Add internal filterbank scale */
  outScalefactor +=
      (ALGORITHMIC_SCALING_IN_ANALYSIS_FILTERBANK + synQmf->filterScale) +
      synQmf->synScalefactor;

  /* adjust filter states when scale factor has been changed */
  if (synQmf->outScalefactor != outScalefactor) {
    int32_t diff;

    diff = synQmf->outScalefactor - outScalefactor;

    qmfAdaptFilterStates(synQmf, diff);

    /* save new scale factor */
    synQmf->outScalefactor = outScalefactor;
  }
}

/*!
 *
 * \brief Get scale factor change which was set by qmfChangeOutScalefactor()
 *
 * \return scaleFactor
 *
 */
int32_t qmfGetOutScalefactor(
    HANDLE_QMF_FILTER_BANK synQmf) /*!< Handle of Qmf Synthesis Bank */
{
  int32_t scaleFactor = synQmf->outScalefactor
                        ? (synQmf->outScalefactor -
                           (ALGORITHMIC_SCALING_IN_ANALYSIS_FILTERBANK +
                            synQmf->filterScale + synQmf->synScalefactor))
                        : 0;
  return scaleFactor;
}

/*!
 *
 * \brief Change gain for output data
 *
 * \return void
 *
 */
void qmfChangeOutGain(
    HANDLE_QMF_FILTER_BANK synQmf, /*!< Handle of Qmf Synthesis Bank */
    int32_t outputGain,           /*!< New gain for output data (mantissa) */
    int32_t outputGainScale            /*!< New gain for output data (exponent) */
) {
  synQmf->outGain_m = outputGain;
  synQmf->outGain_e = outputGainScale;
}

#define INT_PCM_QMFOUT int16_t
#define SAMPLE_BITS_QMFOUT SAMPLE_BITS
#include "qmf_pcm.h"
#if SAMPLE_BITS == 16
  /* also create a 32 bit output version */
#undef INT_PCM_QMFOUT
#undef SAMPLE_BITS_QMFOUT
#undef QMF_PCM_H
#undef FIXP_QAS
#undef QAS_BITS
#undef INT_PCM_QMFIN
#define INT_PCM_QMFOUT int32_t
#define SAMPLE_BITS_QMFOUT 32
#define FIXP_QAS int32_t
#define QAS_BITS 32
#define INT_PCM_QMFIN int32_t
#include "qmf_pcm.h"
#endif
