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

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: independent channel concealment

*******************************************************************************/

/*!
  \page concealment AAC core concealment

  This AAC core implementation includes a concealment function, which can be
  enabled using the several defines during compilation.

  There are various tests inside the core, starting with simple CRC tests and
  ending in a variety of plausibility checks. If such a check indicates an
  invalid bitstream, then concealment is applied.

  Concealment is also applied when the calling main program indicates a
  distorted or missing data frame using the frameOK flag. This is used for error
  detection on the transport layer. (See below)

  There are three concealment-modes:

  1) Muting: The spectral data is simply set to zero in case of an detected
  error.

  2) Noise substitution: In case of an detected error, concealment copies the
  last frame and adds attenuates the spectral data. For this mode you have to
  set the #CONCEAL_NOISE define. Noise substitution adds no additional delay.

  3) Interpolation: The interpolation routine swaps the spectral data from the
  previous and the current frame just before the final frequency to time
  conversion. In case a single frame is corrupted, concealmant interpolates
  between the last good and the first good frame to create the spectral data for
  the missing frame. If multiple frames are corrupted, concealment implements
  first a fade out based on slightly modified spectral values from the last good
     frame. As soon as good frames are available, concealmant fades in the new
  spectral data. For this mode you have to set the #CONCEAL_INTER define. Note
  that in this case, you also need to set #SBR_BS_DELAY_ENABLE, which basically
  adds approriate delay in the SBR decoder. Note that the
  Interpolating-Concealment increases the delay of your decoder by one frame and
  that it does require additional resources such as memory and computational
  complexity.

  <h2>How concealment can be used with errors on the transport layer</h2>

  Many errors can or have to be detected on the transport layer. For example in
  IP based systems packet loss can occur. The transport protocol used should
  indicate such packet loss by inserting an empty frame with frameOK=0.
*/

#include <stdint.h>
#include "conceal.h"

#include "aac_rom.h"
#include "../libSYS/genericStds.h"

/* PNS (of block) */
#include "aacdec_pns.h"
#include "block.h"

#define CONCEAL_DFLT_COMF_NOISE_LEVEL (0x100000)

#define CONCEAL_NOT_DEFINED ((UCHAR)-1)

/* default settings */
#define CONCEAL_DFLT_FADEOUT_FRAMES (6)
#define CONCEAL_DFLT_FADEIN_FRAMES (5)
#define CONCEAL_DFLT_MUTE_RELEASE_FRAMES (0)

#define CONCEAL_DFLT_FADE_FACTOR (0.707106781186548f) /* 1/sqrt(2) */

/* some often used constants: */
#define FIXP_ZERO FL2FXCONST_DBL(0.0f)
#define FIXP_ONE FL2FXCONST_DBL(1.0f)
#define FIXP_FL_CORRECTION FL2FXCONST_DBL(0.53333333333333333f)

/* For parameter conversion */
#define CONCEAL_PARAMETER_BITS (8)
#define CONCEAL_MAX_QUANT_FACTOR ((1 << CONCEAL_PARAMETER_BITS) - 1)
/*#define CONCEAL_MIN_ATTENUATION_FACTOR_025  ( FL2FXCONST_DBL(0.971627951577106174) )*/ /* -0.25 dB */
#define CONCEAL_MIN_ATTENUATION_FACTOR_025_LD \
  FL2FXCONST_DBL(-0.041524101186092029596853445212299)
/*#define CONCEAL_MIN_ATTENUATION_FACTOR_050  ( FL2FXCONST_DBL(0.944060876285923380) )*/ /* -0.50 dB */
#define CONCEAL_MIN_ATTENUATION_FACTOR_050_LD \
  FL2FXCONST_DBL(-0.083048202372184059253597008145293)

typedef enum {
  CConcealment_NoExpand,
  CConcealment_Expand,
  CConcealment_Compress
} CConcealmentExpandType;

static const FIXP_SGL facMod4Table[4] = {
    FL2FXCONST_SGL(0.500000000f), /* FIXP_SGL(0x4000),  2^-(1-0,00) */
    FL2FXCONST_SGL(0.594603558f), /* FIXP_SGL(0x4c1b),  2^-(1-0,25) */
    FL2FXCONST_SGL(0.707106781f), /* FIXP_SGL(0x5a82),  2^-(1-0,50) */
    FL2FXCONST_SGL(0.840896415f)  /* FIXP_SGL(0x6ba2)   2^-(1-0,75) */
};

static void CConcealment_CalcBandEnergy(
    int32_t *spectrum, const SamplingRateInfo *pSamplingRateInfo,
    const int32_t blockType, CConcealmentExpandType ex, int32_t *sfbEnergy);

static void CConcealment_InterpolateBuffer(int32_t *spectrum,
                                           int16_t *pSpecScalePrev,
                                           int16_t *pSpecScaleAct,
                                           int16_t *pSpecScaleOut, int32_t *enPrv,
                                           int32_t *enAct, int32_t sfbCnt,
                                           const int16_t *pSfbOffset);

static int32_t CConcealment_ApplyInter(
    CConcealmentInfo *pConcealmentInfo,
    CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    const SamplingRateInfo *pSamplingRateInfo, const int32_t samplesPerFrame,
    const int32_t improveTonal, const int32_t frameOk, const int32_t mute_release_active);

static int32_t CConcealment_ApplyNoise(
    CConcealmentInfo *pConcealmentInfo,
    CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo,
    const SamplingRateInfo *pSamplingRateInfo, const int32_t samplesPerFrame,
    const uint32_t flags);

static void CConcealment_UpdateState(
    CConcealmentInfo *pConcealmentInfo, int32_t frameOk,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo,
    const int32_t samplesPerFrame, CAacDecoderChannelInfo *pAacDecoderChannelInfo);

static void CConcealment_ApplyRandomSign(int32_t iRandomPhase, int32_t *spec,
                                         int32_t samplesPerFrame);

/* TimeDomainFading */
static void CConcealment_TDFadePcmAtt(int32_t start, int32_t len, int32_t fadeStart,
                                      int32_t fadeStop, int32_t *pcmdata);
static void CConcealment_TDFadeFillFadingStations(int32_t *fadingStations,
                                                  int32_t *fadingSteps,
                                                  int32_t fadeStop,
                                                  int32_t fadeStart,
                                                  TDfadingType fadingType);
static void CConcealment_TDFading_doLinearFadingSteps(int32_t *fadingSteps);

/* Streamline the state machine */
static int32_t CConcealment_ApplyFadeOut(
    int32_t mode, CConcealmentInfo *pConcealmentInfo,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo,
    const int32_t samplesPerFrame, CAacDecoderChannelInfo *pAacDecoderChannelInfo);

static int32_t CConcealment_TDNoise_Random(uint32_t *seed);
static void CConcealment_TDNoise_Apply(CConcealmentInfo *const pConcealmentInfo,
                                       const int32_t len,
                                       const int32_t aacOutDataHeadroom,
                                       int32_t *const pcmdata);

static BLOCK_TYPE CConcealment_GetWinSeq(int32_t prevWinSeq) {
  BLOCK_TYPE newWinSeq = BLOCK_LONG;

  /* Try to have only int32_t blocks */
  if (prevWinSeq == BLOCK_START || prevWinSeq == BLOCK_SHORT) {
    newWinSeq = BLOCK_STOP;
  }

  return (newWinSeq);
}

/*!
  \brief Init common concealment information data

  \param pConcealCommonData Pointer to the concealment common data structure.
*/
void CConcealment_InitCommonData(CConcealParams *pConcealCommonData) {
  if (pConcealCommonData != NULL) {
    int32_t i;

    /* Set default error concealment technique */
    pConcealCommonData->method = ConcealMethodInter;

    pConcealCommonData->numFadeOutFrames = CONCEAL_DFLT_FADEOUT_FRAMES;
    pConcealCommonData->numFadeInFrames = CONCEAL_DFLT_FADEIN_FRAMES;
    pConcealCommonData->numMuteReleaseFrames = CONCEAL_DFLT_MUTE_RELEASE_FRAMES;

    pConcealCommonData->comfortNoiseLevel =
        (int32_t)CONCEAL_DFLT_COMF_NOISE_LEVEL;

    /* Init fade factors (symetric) */
    pConcealCommonData->fadeOutFactor[0] =
        FL2FXCONST_SGL(CONCEAL_DFLT_FADE_FACTOR);
    pConcealCommonData->fadeInFactor[0] = pConcealCommonData->fadeOutFactor[0];

    for (i = 1; i < CONCEAL_MAX_NUM_FADE_FACTORS; i++) {
      pConcealCommonData->fadeOutFactor[i] =
          FX_DBL2FX_SGL(fMult(pConcealCommonData->fadeOutFactor[i - 1],
                              FL2FXCONST_SGL(CONCEAL_DFLT_FADE_FACTOR)));
      pConcealCommonData->fadeInFactor[i] =
          pConcealCommonData->fadeOutFactor[i];
    }
  }
}

/*!
  \brief Get current concealment method.

  \param pConcealCommonData Pointer to common concealment data (for all
  channels)
*/
CConcealmentMethod CConcealment_GetMethod(CConcealParams *pConcealCommonData) {
  CConcealmentMethod method = ConcealMethodNone;

  if (pConcealCommonData != NULL) {
    method = pConcealCommonData->method;
  }

  return (method);
}

/*!
  \brief Init concealment information for each channel

  \param pConcealChannelInfo Pointer to the channel related concealment info
  structure to be initialized. \param pConcealCommonData  Pointer to common
  concealment data (for all channels) \param initRenderMode      Initial render
  mode to be set for the current channel. \param samplesPerFrame     The number
  of samples per frame.
*/
void CConcealment_InitChannelData(CConcealmentInfo *pConcealChannelInfo,
                                  CConcealParams *pConcealCommonData,
                                  AACDEC_RENDER_MODE initRenderMode,
                                  int32_t samplesPerFrame) {
  int32_t i;
  pConcealChannelInfo->TDNoiseSeed = 0;
  FDKmemclear(pConcealChannelInfo->TDNoiseStates,
              sizeof(pConcealChannelInfo->TDNoiseStates));
  pConcealChannelInfo->TDNoiseCoef[0] = FL2FXCONST_SGL(0.05f);
  pConcealChannelInfo->TDNoiseCoef[1] = FL2FXCONST_SGL(0.5f);
  pConcealChannelInfo->TDNoiseCoef[2] = FL2FXCONST_SGL(0.45f);

  pConcealChannelInfo->pConcealParams = pConcealCommonData;

  pConcealChannelInfo->lastRenderMode = initRenderMode;

  pConcealChannelInfo->windowShape = CONCEAL_NOT_DEFINED;
  pConcealChannelInfo->windowSequence = BLOCK_LONG; /* default type */
  pConcealChannelInfo->lastWinGrpLen = 1;

  pConcealChannelInfo->concealState = ConcealState_Ok;

  FDKmemclear(pConcealChannelInfo->spectralCoefficient,
              1024 * sizeof(FIXP_CNCL));

  for (i = 0; i < 8; i++) {
    pConcealChannelInfo->specScale[i] = 0;
  }

  pConcealChannelInfo->iRandomPhase = 0;

  pConcealChannelInfo->prevFrameOk[0] = 1;
  pConcealChannelInfo->prevFrameOk[1] = 1;

  pConcealChannelInfo->cntFadeFrames = 0;
  pConcealChannelInfo->cntValidFrames = 0;
  pConcealChannelInfo->fade_old = (int32_t)MAXVAL_DBL;
  pConcealChannelInfo->winGrpOffset[0] = 0;
  pConcealChannelInfo->winGrpOffset[1] = 0;
  pConcealChannelInfo->attGrpOffset[0] = 0;
  pConcealChannelInfo->attGrpOffset[1] = 0;
}

/*!
  \brief Set error concealment parameters

  \param concealParams
  \param method
  \param fadeOutSlope
  \param fadeInSlope
  \param muteRelease
  \param comfNoiseLevel
*/
AAC_DECODER_ERROR
CConcealment_SetParams(CConcealParams *concealParams, int32_t method,
                       int32_t fadeOutSlope, int32_t fadeInSlope, int32_t muteRelease,
                       int32_t comfNoiseLevel) {
  /* set concealment technique */
  if (method != AACDEC_CONCEAL_PARAM_NOT_SPECIFIED) {
    switch ((CConcealmentMethod)method) {
      case ConcealMethodMute:
      case ConcealMethodNoise:
      case ConcealMethodInter:
        /* Be sure to enable delay adjustment of SBR decoder! */
        if (concealParams == NULL) {
          return AAC_DEC_INVALID_HANDLE;
        } else {
          /* set param */
          concealParams->method = (CConcealmentMethod)method;
        }
        break;

      default:
        return AAC_DEC_SET_PARAM_FAIL;
    }
  }

  /* set number of frames for fade-out slope */
  if (fadeOutSlope != AACDEC_CONCEAL_PARAM_NOT_SPECIFIED) {
    if ((fadeOutSlope < CONCEAL_MAX_NUM_FADE_FACTORS) && (fadeOutSlope >= 0)) {
      if (concealParams == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      } else {
        /* set param */
        concealParams->numFadeOutFrames = fadeOutSlope;
      }
    } else {
      return AAC_DEC_SET_PARAM_FAIL;
    }
  }

  /* set number of frames for fade-in slope */
  if (fadeInSlope != AACDEC_CONCEAL_PARAM_NOT_SPECIFIED) {
    if ((fadeInSlope < CONCEAL_MAX_NUM_FADE_FACTORS) && (fadeInSlope >= 0)) {
      if (concealParams == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      } else {
        /* set param */
        concealParams->numFadeInFrames = fadeInSlope;
      }
    } else {
      return AAC_DEC_SET_PARAM_FAIL;
    }
  }

  /* set number of error-free frames after which the muting will be released */
  if (muteRelease != AACDEC_CONCEAL_PARAM_NOT_SPECIFIED) {
    if ((muteRelease < (CONCEAL_MAX_NUM_FADE_FACTORS << 1)) &&
        (muteRelease >= 0)) {
      if (concealParams == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      } else {
        /* set param */
        concealParams->numMuteReleaseFrames = muteRelease;
      }
    } else {
      return AAC_DEC_SET_PARAM_FAIL;
    }
  }

  /* set confort noise level which will be inserted while in state 'muting' */
  if (comfNoiseLevel != (int32_t)AACDEC_CONCEAL_PARAM_NOT_SPECIFIED) {
    if ((comfNoiseLevel < (int32_t)0) ||
        (comfNoiseLevel > (int32_t)MAXVAL_DBL)) {
      return AAC_DEC_SET_PARAM_FAIL;
    }
    if (concealParams == NULL) {
      return AAC_DEC_INVALID_HANDLE;
    } else {
      concealParams->comfortNoiseLevel = (int32_t)comfNoiseLevel;
    }
  }

  return (AAC_DEC_OK);
}

/*!
  \brief Set fade-out/in attenuation factor vectors

  \param concealParams
  \param fadeOutAttenuationVector
  \param fadeInAttenuationVector

  \return 0 if OK all other values indicate errors
*/
AAC_DECODER_ERROR
CConcealment_SetAttenuation(CConcealParams *concealParams,
                            const int16_t *fadeOutAttenuationVector,
                            const int16_t *fadeInAttenuationVector) {
  if ((fadeOutAttenuationVector == NULL) && (fadeInAttenuationVector == NULL)) {
    return AAC_DEC_SET_PARAM_FAIL;
  }

  /* Fade-out factors */
  if (fadeOutAttenuationVector != NULL) {
    int32_t i;

    /* check quantized factors first */
    for (i = 0; i < CONCEAL_MAX_NUM_FADE_FACTORS; i++) {
      if ((fadeOutAttenuationVector[i] < 0) ||
          (fadeOutAttenuationVector[i] > CONCEAL_MAX_QUANT_FACTOR)) {
        return AAC_DEC_SET_PARAM_FAIL;
      }
    }
    if (concealParams == NULL) {
      return AAC_DEC_INVALID_HANDLE;
    }

    /* now dequantize factors */
    for (i = 0; i < CONCEAL_MAX_NUM_FADE_FACTORS; i++) {
      concealParams->fadeOutFactor[i] =
          FX_DBL2FX_SGL(fLdPow(CONCEAL_MIN_ATTENUATION_FACTOR_025_LD, 0,
                               (int32_t)((int32_t)(FL2FXCONST_DBL(1.0 / 2.0) >>
                                                (CONCEAL_PARAMETER_BITS - 1)) *
                                          (int32_t)fadeOutAttenuationVector[i]),
                               CONCEAL_PARAMETER_BITS));
    }
  }

  /* Fade-in factors */
  if (fadeInAttenuationVector != NULL) {
    int32_t i;

    /* check quantized factors first */
    for (i = 0; i < CONCEAL_MAX_NUM_FADE_FACTORS; i++) {
      if ((fadeInAttenuationVector[i] < 0) ||
          (fadeInAttenuationVector[i] > CONCEAL_MAX_QUANT_FACTOR)) {
        return AAC_DEC_SET_PARAM_FAIL;
      }
    }
    if (concealParams == NULL) {
      return AAC_DEC_INVALID_HANDLE;
    }

    /* now dequantize factors */
    for (i = 0; i < CONCEAL_MAX_NUM_FADE_FACTORS; i++) {
      concealParams->fadeInFactor[i] = FX_DBL2FX_SGL(
          fLdPow(CONCEAL_MIN_ATTENUATION_FACTOR_025_LD, 0,
                 (int32_t)((int32_t)(FIXP_ONE >> CONCEAL_PARAMETER_BITS) *
                            (int32_t)fadeInAttenuationVector[i]),
                 CONCEAL_PARAMETER_BITS));
    }
  }

  return (AAC_DEC_OK);
}

/*!
  \brief Get state of concealment module.

  \param pConcealChannelInfo

  \return Concealment state.
*/
CConcealmentState CConcealment_GetState(CConcealmentInfo *pConcealChannelInfo) {
  CConcealmentState state = ConcealState_Ok;

  if (pConcealChannelInfo != NULL) {
    state = pConcealChannelInfo->concealState;
  }

  return (state);
}

/*!
  \brief Store data for concealment techniques applied later

  Interface function to store data for different concealment strategies
 */
void CConcealment_Store(
    CConcealmentInfo *hConcealmentInfo,
    CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo) {
  UCHAR nbDiv = NB_DIV;

  if (!(pAacDecoderChannelInfo->renderMode == AACDEC_RENDER_LPD &&
        pAacDecoderChannelInfo->data.usac.mod[nbDiv - 1] == 0))

  {
    int32_t *pSpectralCoefficient =
        SPEC_LONG(pAacDecoderChannelInfo->pSpectralCoefficient);
    int16_t *pSpecScale = pAacDecoderChannelInfo->specScale;
    CIcsInfo *pIcsInfo = &pAacDecoderChannelInfo->icsInfo;

    int16_t tSpecScale[8];
    UCHAR tWindowShape;
    BLOCK_TYPE tWindowSequence;

    /* store old window infos for swapping */
    tWindowSequence = hConcealmentInfo->windowSequence;
    tWindowShape = hConcealmentInfo->windowShape;

    /* store old scale factors for swapping */
    FDKmemcpy(tSpecScale, hConcealmentInfo->specScale, 8 * sizeof(int16_t));

    /* store new window infos */
    hConcealmentInfo->windowSequence = GetWindowSequence(pIcsInfo);
    hConcealmentInfo->windowShape = GetWindowShape(pIcsInfo);
    hConcealmentInfo->lastWinGrpLen =
        *(GetWindowGroupLengthTable(pIcsInfo) + GetWindowGroups(pIcsInfo) - 1);

    /* store new scale factors */
    FDKmemcpy(hConcealmentInfo->specScale, pSpecScale, 8 * sizeof(int16_t));

    if (hConcealmentInfo->pConcealParams->method < ConcealMethodInter) {
    /* store new spectral bins */
#if (CNCL_FRACT_BITS == DFRACT_BITS)
      FDKmemcpy(hConcealmentInfo->spectralCoefficient, pSpectralCoefficient,
                1024 * sizeof(FIXP_CNCL));
#else
      FIXP_CNCL *RESTRICT pCncl =
          &hConcealmentInfo->spectralCoefficient[1024 - 1];
      int32_t *RESTRICT pSpec = &pSpectralCoefficient[1024 - 1];
      int32_t i;
      for (i = 1024; i != 0; i--) {
        *pCncl-- = FX_DBL2FX_CNCL(*pSpec--);
      }
#endif
    } else {
    /* swap spectral data */
#if (FIXP_CNCL == int32_t)
      C_ALLOC_SCRATCH_START(pSpecTmp, int32_t, 1024);
      FDKmemcpy(pSpecTmp, pSpectralCoefficient, 1024 * sizeof(int32_t));
      FDKmemcpy(pSpectralCoefficient, hConcealmentInfo->spectralCoefficient,
                1024 * sizeof(int32_t));
      FDKmemcpy(hConcealmentInfo->spectralCoefficient, pSpecTmp,
                1024 * sizeof(int32_t));
      C_ALLOC_SCRATCH_END(pSpecTmp, int32_t, 1024);
#else
      FIXP_CNCL *RESTRICT pCncl =
          &hConcealmentInfo->spectralCoefficient[1024 - 1];
      int32_t *RESTRICT pSpec = &pSpectralCoefficient[1024 - 1];
      int32_t tSpec;

      for (int32_t i = 1024; i != 0; i--) {
        tSpec = *pSpec;
        *pSpec-- = FX_CNCL2FX_DBL(*pCncl);
        *pCncl-- = FX_DBL2FX_CNCL(tSpec);
      }
#endif

      /* complete swapping of window infos */
      pIcsInfo->WindowSequence = tWindowSequence;
      pIcsInfo->WindowShape = tWindowShape;

      /* complete swapping of scale factors */
      FDKmemcpy(pSpecScale, tSpecScale, 8 * sizeof(int16_t));
    }
  }

  if (pAacDecoderChannelInfo->renderMode == AACDEC_RENDER_LPD) {
    /* Store LSF4 */
    FDKmemcpy(hConcealmentInfo->lsf4, pAacDecoderStaticChannelInfo->lpc4_lsf,
              sizeof(hConcealmentInfo->lsf4));
    /* Store TCX gain */
    hConcealmentInfo->last_tcx_gain =
        pAacDecoderStaticChannelInfo->last_tcx_gain;
    hConcealmentInfo->last_tcx_gain_e =
        pAacDecoderStaticChannelInfo->last_tcx_gain_e;
  }
}

/*!
  \brief Apply concealment

  Interface function to different concealment strategies
 */
int32_t CConcealment_Apply(
    CConcealmentInfo *hConcealmentInfo,
    CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo,
    const SamplingRateInfo *pSamplingRateInfo, const int32_t samplesPerFrame,
    const UCHAR lastLpdMode, const int32_t frameOk, const uint32_t flags) {
  int32_t appliedProcessing = 0;
  const int32_t mute_release_active =
      frameOk && (hConcealmentInfo->concealState >= ConcealState_Mute) &&
      (hConcealmentInfo->cntValidFrames + 1 <=
       hConcealmentInfo->pConcealParams->numMuteReleaseFrames);

  if (hConcealmentInfo->windowShape == CONCEAL_NOT_DEFINED) {
    /* Initialize window_shape with same value as in the current (parsed) frame.
       Because section 4.6.11.3.2 (Windowing and block switching) of ISO/IEC
       14496-3:2009 says: For the first raw_data_block() to be decoded the
       window_shape of the left and right half of the window are identical. */
    hConcealmentInfo->windowShape = pAacDecoderChannelInfo->icsInfo.WindowShape;
  }

  if (frameOk && !mute_release_active) {
    /* Update render mode if frameOk except for ongoing mute release state. */
    hConcealmentInfo->lastRenderMode =
        (SCHAR)pAacDecoderChannelInfo->renderMode;

    /* Rescue current data for concealment in future frames */
    CConcealment_Store(hConcealmentInfo, pAacDecoderChannelInfo,
                       pAacDecoderStaticChannelInfo);
    /* Reset index to random sign vector to make sign calculation frame agnostic
       (only depends on number of subsequently concealed spectral blocks) */
    hConcealmentInfo->iRandomPhase = 0;
  } else {
    if (hConcealmentInfo->lastRenderMode == AACDEC_RENDER_INVALID) {
      hConcealmentInfo->lastRenderMode = AACDEC_RENDER_IMDCT;
    }
    pAacDecoderChannelInfo->renderMode =
        (AACDEC_RENDER_MODE)hConcealmentInfo->lastRenderMode;
  }

  /* hand current frame status to the state machine */
  CConcealment_UpdateState(hConcealmentInfo, frameOk,
                           pAacDecoderStaticChannelInfo, samplesPerFrame,
                           pAacDecoderChannelInfo);

  {
    if (!frameOk && pAacDecoderChannelInfo->renderMode == AACDEC_RENDER_IMDCT) {
      /* LPC extrapolation */
      CLpc_Conceal(pAacDecoderChannelInfo->data.usac.lsp_coeff,
                   pAacDecoderStaticChannelInfo->lpc4_lsf,
                   pAacDecoderStaticChannelInfo->lsf_adaptive_mean,
                   hConcealmentInfo->lastRenderMode == AACDEC_RENDER_IMDCT);
      FDKmemcpy(hConcealmentInfo->lsf4, pAacDecoderStaticChannelInfo->lpc4_lsf,
                sizeof(pAacDecoderStaticChannelInfo->lpc4_lsf));
    }

    /* Create data for signal rendering according to the selected concealment
     * method and decoder operating mode. */

    if ((!frameOk || mute_release_active) &&
        (pAacDecoderChannelInfo->renderMode == AACDEC_RENDER_LPD)) {
      /* Restore old LSF4 */
      FDKmemcpy(pAacDecoderStaticChannelInfo->lpc4_lsf, hConcealmentInfo->lsf4,
                sizeof(pAacDecoderStaticChannelInfo->lpc4_lsf));
      /* Restore old TCX gain */
      pAacDecoderStaticChannelInfo->last_tcx_gain =
          hConcealmentInfo->last_tcx_gain;
      pAacDecoderStaticChannelInfo->last_tcx_gain_e =
          hConcealmentInfo->last_tcx_gain_e;
    }

    if (!(pAacDecoderChannelInfo->renderMode == AACDEC_RENDER_LPD &&
          pAacDecoderStaticChannelInfo->last_lpd_mode == 0)) {
      switch (hConcealmentInfo->pConcealParams->method) {
        default:
        case ConcealMethodMute:
          if (!frameOk) {
            /* Mute spectral data in case of errors */
            FDKmemclear(pAacDecoderChannelInfo->pSpectralCoefficient,
                        samplesPerFrame * sizeof(int32_t));
            /* Set last window shape */
            pAacDecoderChannelInfo->icsInfo.WindowShape =
                hConcealmentInfo->windowShape;
            appliedProcessing = 1;
          }
          break;

        case ConcealMethodNoise:
          /* Noise substitution error concealment technique */
          appliedProcessing = CConcealment_ApplyNoise(
              hConcealmentInfo, pAacDecoderChannelInfo,
              pAacDecoderStaticChannelInfo, pSamplingRateInfo, samplesPerFrame,
              flags);
          break;

        case ConcealMethodInter:
          /* Energy interpolation concealment based on 3GPP */
          appliedProcessing = CConcealment_ApplyInter(
              hConcealmentInfo, pAacDecoderChannelInfo, pSamplingRateInfo,
              samplesPerFrame, 0, /* don't use tonal improvement */
              frameOk, mute_release_active);
          break;
      }
    } else if (!frameOk || mute_release_active) {
      /* simply restore the buffer */
      int32_t *pSpectralCoefficient =
          SPEC_LONG(pAacDecoderChannelInfo->pSpectralCoefficient);
      int16_t *pSpecScale = pAacDecoderChannelInfo->specScale;
      CIcsInfo *pIcsInfo = &pAacDecoderChannelInfo->icsInfo;
#if (CNCL_FRACT_BITS != DFRACT_BITS)
      FIXP_CNCL *RESTRICT pCncl =
          &hConcealmentInfo->spectralCoefficient[1024 - 1];
      int32_t *RESTRICT pSpec = &pSpectralCoefficient[1024 - 1];
      int32_t i;
#endif

      /* restore window infos (gri) do we need that? */
      pIcsInfo->WindowSequence = hConcealmentInfo->windowSequence;
      pIcsInfo->WindowShape = hConcealmentInfo->windowShape;

      if (hConcealmentInfo->concealState != ConcealState_Mute) {
        /* restore scale factors */
        FDKmemcpy(pSpecScale, hConcealmentInfo->specScale, 8 * sizeof(int16_t));

        /* restore spectral bins */
#if (CNCL_FRACT_BITS == DFRACT_BITS)
        FDKmemcpy(pSpectralCoefficient, hConcealmentInfo->spectralCoefficient,
                  1024 * sizeof(int32_t));
#else
        for (i = 1024; i != 0; i--) {
          *pSpec-- = FX_CNCL2FX_DBL(*pCncl--);
        }
#endif
      } else {
        /* clear scale factors */
        FDKmemclear(pSpecScale, 8 * sizeof(int16_t));

        /* clear buffer */
        FDKmemclear(pSpectralCoefficient, 1024 * sizeof(FIXP_CNCL));
      }
    }
  }
  /* update history */
  hConcealmentInfo->prevFrameOk[0] = hConcealmentInfo->prevFrameOk[1];
  hConcealmentInfo->prevFrameOk[1] = frameOk;

  return mute_release_active ? -1 : appliedProcessing;
}

/*!
\brief Apply concealment noise substitution

  In case of frame lost this function produces a noisy frame with respect to the
  energies values of past frame.
 */
static int32_t CConcealment_ApplyNoise(
    CConcealmentInfo *pConcealmentInfo,
    CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo,
    const SamplingRateInfo *pSamplingRateInfo, const int32_t samplesPerFrame,
    const uint32_t flags) {
  int32_t *pSpectralCoefficient =
      SPEC_LONG(pAacDecoderChannelInfo->pSpectralCoefficient);
  CIcsInfo *pIcsInfo = &pAacDecoderChannelInfo->icsInfo;

  int32_t appliedProcessing = 0;

  FDK_ASSERT(pConcealmentInfo != NULL);
  FDK_ASSERT((samplesPerFrame >= 120) && (samplesPerFrame <= 1024));

  switch (pConcealmentInfo->concealState) {
    case ConcealState_Ok:
      /* Nothing to do here! */
      break;

    case ConcealState_Single:
    case ConcealState_FadeOut:
      appliedProcessing = CConcealment_ApplyFadeOut(
          /*mode =*/1, pConcealmentInfo, pAacDecoderStaticChannelInfo,
          samplesPerFrame, pAacDecoderChannelInfo);
      break;

    case ConcealState_Mute: {
      /* set dummy window parameters */
      pIcsInfo->Valid = 0; /* Trigger the generation of a consitent IcsInfo */
      pIcsInfo->WindowShape =
          pConcealmentInfo->windowShape; /* Prevent an invalid WindowShape
                                            (required for F/T transform) */
      pIcsInfo->WindowSequence =
          CConcealment_GetWinSeq(pConcealmentInfo->windowSequence);
      pConcealmentInfo->windowSequence =
          pIcsInfo->WindowSequence; /* Store for next frame
                                       (spectrum in concealment
                                       buffer can't be used at
                                       all) */

      /* mute spectral data */
      FDKmemclear(pSpectralCoefficient, samplesPerFrame * sizeof(int32_t));
      FDKmemclear(pConcealmentInfo->spectralCoefficient,
                  samplesPerFrame * sizeof(int32_t));

      appliedProcessing = 1;
    } break;

    case ConcealState_FadeIn: {
      /* TimeDomainFading:                                        */
      /* Attenuation of signal is done in CConcealment_TDFading() */

      appliedProcessing = 1;
    } break;

    default:
      /* we shouldn't come here anyway */
      FDK_ASSERT(0);
      break;
  }

  return appliedProcessing;
}

/*!
  \brief Apply concealment interpolation

  The function swaps the data from the current and the previous frame. If an
  error has occured, frame interpolation is performed to restore the missing
  frame. In case of multiple faulty frames, fade-in and fade-out is applied.
*/
static int32_t CConcealment_ApplyInter(
    CConcealmentInfo *pConcealmentInfo,
    CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    const SamplingRateInfo *pSamplingRateInfo, const int32_t samplesPerFrame,
    const int32_t improveTonal, const int32_t frameOk, const int32_t mute_release_active) {
#if defined(FDK_ASSERT_ENABLE)
  CConcealParams *pConcealCommonData = pConcealmentInfo->pConcealParams;
#endif

  int32_t *pSpectralCoefficient =
      SPEC_LONG(pAacDecoderChannelInfo->pSpectralCoefficient);
  CIcsInfo *pIcsInfo = &pAacDecoderChannelInfo->icsInfo;
  int16_t *pSpecScale = pAacDecoderChannelInfo->specScale;

  int32_t sfbEnergyPrev[64];
  int32_t sfbEnergyAct[64];

  int32_t i, appliedProcessing = 0;

  /* clear/init */
  FDKmemclear(sfbEnergyPrev, 64 * sizeof(int32_t));
  FDKmemclear(sfbEnergyAct, 64 * sizeof(int32_t));

  if (!frameOk || mute_release_active) {
    /* Restore last frame from concealment buffer */
    pIcsInfo->WindowShape = pConcealmentInfo->windowShape;
    pIcsInfo->WindowSequence = pConcealmentInfo->windowSequence;

    /* Restore spectral data */
    for (i = 0; i < samplesPerFrame; i++) {
      pSpectralCoefficient[i] =
          FX_CNCL2FX_DBL(pConcealmentInfo->spectralCoefficient[i]);
    }

    /* Restore scale factors */
    FDKmemcpy(pSpecScale, pConcealmentInfo->specScale, 8 * sizeof(int16_t));
  }

  /* if previous frame was not ok */
  if (!pConcealmentInfo->prevFrameOk[1] || mute_release_active) {
    /* if current frame (f_n) is ok and the last but one frame (f_(n-2))
       was ok, too, then interpolate both frames in order to generate
       the current output frame (f_(n-1)). Otherwise, use the last stored
       frame (f_(n-2) or f_(n-3) or ...). */
    if (frameOk && pConcealmentInfo->prevFrameOk[0] && !mute_release_active) {
      appliedProcessing = 1;

      /* Interpolate both frames in order to generate the current output frame
       * (f_(n-1)). */
      if (pIcsInfo->WindowSequence == BLOCK_SHORT) {
        /* f_(n-2) == BLOCK_SHORT */
        /* int16_t--??????--int16_t, int16_t--??????--int32_t interpolation */
        /* int16_t--int16_t---int16_t, int16_t---int32_t---int32_t interpolation */

        int32_t wnd;

        if (pConcealmentInfo->windowSequence ==
            BLOCK_SHORT) { /* f_n == BLOCK_SHORT */
          /* int16_t--int16_t---int16_t interpolation */

          int32_t scaleFactorBandsTotal =
              pSamplingRateInfo->NumberOfScaleFactorBands_Short;
          const int16_t *pSfbOffset = pSamplingRateInfo->ScaleFactorBands_Short;
          pIcsInfo->WindowShape = (samplesPerFrame <= 512) ? 2 : 1;
          pIcsInfo->WindowSequence = BLOCK_SHORT;

          for (wnd = 0; wnd < 8; wnd++) {
            CConcealment_CalcBandEnergy(
                &pSpectralCoefficient[wnd *
                                      (samplesPerFrame / 8)], /* spec_(n-2) */
                pSamplingRateInfo, BLOCK_SHORT, CConcealment_NoExpand,
                sfbEnergyPrev);

            CConcealment_CalcBandEnergy(
                &pConcealmentInfo->spectralCoefficient[wnd * (samplesPerFrame /
                                                              8)], /* spec_n */
                pSamplingRateInfo, BLOCK_SHORT, CConcealment_NoExpand,
                sfbEnergyAct);

            CConcealment_InterpolateBuffer(
                &pSpectralCoefficient[wnd *
                                      (samplesPerFrame / 8)], /* spec_(n-1) */
                &pSpecScale[wnd], &pConcealmentInfo->specScale[wnd],
                &pSpecScale[wnd], sfbEnergyPrev, sfbEnergyAct,
                scaleFactorBandsTotal, pSfbOffset);
          }
        } else { /* f_n != BLOCK_SHORT */
          /* int16_t---int32_t---int32_t interpolation */

          int32_t scaleFactorBandsTotal =
              pSamplingRateInfo->NumberOfScaleFactorBands_Long;
          const int16_t *pSfbOffset = pSamplingRateInfo->ScaleFactorBands_Long;
          int16_t specScaleOut;

          CConcealment_CalcBandEnergy(
              &pSpectralCoefficient[samplesPerFrame -
                                    (samplesPerFrame /
                                     8)], /* [wnd] spec_(n-2) */
              pSamplingRateInfo, BLOCK_SHORT, CConcealment_Expand,
              sfbEnergyAct);

          CConcealment_CalcBandEnergy(
              pConcealmentInfo->spectralCoefficient, /* spec_n */
              pSamplingRateInfo, BLOCK_LONG, CConcealment_NoExpand,
              sfbEnergyPrev);

          pIcsInfo->WindowShape = 0;
          pIcsInfo->WindowSequence = BLOCK_STOP;

          for (i = 0; i < samplesPerFrame; i++) {
            pSpectralCoefficient[i] =
                pConcealmentInfo->spectralCoefficient[i]; /* spec_n */
          }

          for (i = 0; i < 8; i++) { /* search for max(specScale) */
            if (pSpecScale[i] > pSpecScale[0]) {
              pSpecScale[0] = pSpecScale[i];
            }
          }

          CConcealment_InterpolateBuffer(
              pSpectralCoefficient, /* spec_(n-1) */
              &pConcealmentInfo->specScale[0], &pSpecScale[0], &specScaleOut,
              sfbEnergyPrev, sfbEnergyAct, scaleFactorBandsTotal, pSfbOffset);

          pSpecScale[0] = specScaleOut;
        }
      } else {
        /* int32_t--??????--int16_t, int32_t--??????--int32_t interpolation */
        /* int32_t---int32_t---int16_t, int32_t---int32_t---int32_t interpolation */

        int32_t scaleFactorBandsTotal =
            pSamplingRateInfo->NumberOfScaleFactorBands_Long;
        const int16_t *pSfbOffset = pSamplingRateInfo->ScaleFactorBands_Long;
        int16_t specScaleAct = pConcealmentInfo->specScale[0];

        CConcealment_CalcBandEnergy(pSpectralCoefficient, /* spec_(n-2) */
                                    pSamplingRateInfo, BLOCK_LONG,
                                    CConcealment_NoExpand, sfbEnergyPrev);

        if (pConcealmentInfo->windowSequence ==
            BLOCK_SHORT) { /* f_n == BLOCK_SHORT */
          /* int32_t---int32_t---int16_t interpolation */

          pIcsInfo->WindowShape = (samplesPerFrame <= 512) ? 2 : 1;
          pIcsInfo->WindowSequence = BLOCK_START;

          for (i = 1; i < 8; i++) { /* search for max(specScale) */
            if (pConcealmentInfo->specScale[i] > specScaleAct) {
              specScaleAct = pConcealmentInfo->specScale[i];
            }
          }

          /* Expand first int16_t spectrum */
          CConcealment_CalcBandEnergy(
              pConcealmentInfo->spectralCoefficient,               /* spec_n */
              pSamplingRateInfo, BLOCK_SHORT, CConcealment_Expand, /* !!! */
              sfbEnergyAct);
        } else {
          /* int32_t---int32_t---int32_t interpolation */

          pIcsInfo->WindowShape = 0;
          pIcsInfo->WindowSequence = BLOCK_LONG;

          CConcealment_CalcBandEnergy(
              pConcealmentInfo->spectralCoefficient, /* spec_n */
              pSamplingRateInfo, BLOCK_LONG, CConcealment_NoExpand,
              sfbEnergyAct);
        }

        CConcealment_InterpolateBuffer(
            pSpectralCoefficient, /* spec_(n-1) */
            &pSpecScale[0], &specScaleAct, &pSpecScale[0], sfbEnergyPrev,
            sfbEnergyAct, scaleFactorBandsTotal, pSfbOffset);
      }
    }

    /* Noise substitution of sign of the output spectral coefficients */
    CConcealment_ApplyRandomSign(pConcealmentInfo->iRandomPhase,
                                 pSpectralCoefficient, samplesPerFrame);
    /* Increment random phase index to avoid repetition artifacts. */
    pConcealmentInfo->iRandomPhase =
        (pConcealmentInfo->iRandomPhase + 1) & (AAC_NF_NO_RANDOM_VAL - 1);
  }

  /* scale spectrum according to concealment state */
  switch (pConcealmentInfo->concealState) {
    case ConcealState_Single:
      appliedProcessing = 1;
      break;

    case ConcealState_FadeOut: {
      FDK_ASSERT(pConcealmentInfo->cntFadeFrames >= 0);
      FDK_ASSERT(pConcealmentInfo->cntFadeFrames <
                 CONCEAL_MAX_NUM_FADE_FACTORS);
      FDK_ASSERT(pConcealmentInfo->cntFadeFrames <
                 pConcealCommonData->numFadeOutFrames);

      /* TimeDomainFading:                                        */
      /* Attenuation of signal is done in CConcealment_TDFading() */

      appliedProcessing = 1;
    } break;

    case ConcealState_FadeIn: {
      FDK_ASSERT(pConcealmentInfo->cntFadeFrames >= 0);
      FDK_ASSERT(pConcealmentInfo->cntFadeFrames <
                 CONCEAL_MAX_NUM_FADE_FACTORS);
      FDK_ASSERT(pConcealmentInfo->cntFadeFrames <
                 pConcealCommonData->numFadeInFrames);

      /* TimeDomainFading:                                        */
      /* Attenuation of signal is done in CConcealment_TDFading() */

      appliedProcessing = 1;
    } break;

    case ConcealState_Mute: {
      /* set dummy window parameters */
      pIcsInfo->Valid = 0; /* Trigger the generation of a consitent IcsInfo */
      pIcsInfo->WindowShape =
          pConcealmentInfo->windowShape; /* Prevent an invalid WindowShape
                                            (required for F/T transform) */
      pIcsInfo->WindowSequence =
          CConcealment_GetWinSeq(pConcealmentInfo->windowSequence);
      pConcealmentInfo->windowSequence =
          pIcsInfo->WindowSequence; /* Store for next frame
                                       (spectrum in concealment
                                       buffer can't be used at
                                       all) */

      /* mute spectral data */
      FDKmemclear(pSpectralCoefficient, samplesPerFrame * sizeof(int32_t));

      appliedProcessing = 1;
    } break;

    default:
      /* nothing to do here */
      break;
  }

  return appliedProcessing;
}

/*!
  \brief Calculate the spectral energy

  The function calculates band-wise the spectral energy. This is used for
  frame interpolation.
*/
static void CConcealment_CalcBandEnergy(
    int32_t *spectrum, const SamplingRateInfo *pSamplingRateInfo,
    const int32_t blockType, CConcealmentExpandType expandType, int32_t *sfbEnergy) {
  const int16_t *pSfbOffset;
  int32_t line, sfb, scaleFactorBandsTotal = 0;

  /* In the following calculations, enAccu is initialized with LSB-value in
   * order to avoid zero energy-level */

  line = 0;

  switch (blockType) {
    case BLOCK_LONG:
    case BLOCK_START:
    case BLOCK_STOP:

      if (expandType == CConcealment_NoExpand) {
        /* standard int32_t calculation */
        scaleFactorBandsTotal =
            pSamplingRateInfo->NumberOfScaleFactorBands_Long;
        pSfbOffset = pSamplingRateInfo->ScaleFactorBands_Long;

        for (sfb = 0; sfb < scaleFactorBandsTotal; sfb++) {
          int32_t enAccu = (int32_t)(int32_t)1;
          int32_t sfbScale =
              (sizeof(int32_t) << 3) -
              CntLeadingZeros(pSfbOffset[sfb + 1] - pSfbOffset[sfb]) - 1;
          /* scaling depends on sfb width. */
          for (; line < pSfbOffset[sfb + 1]; line++) {
            enAccu += fPow2Div2(*(spectrum + line)) >> sfbScale;
          }
          *(sfbEnergy + sfb) = CntLeadingZeros(enAccu) - 1;
        }
      } else {
        /* compress int32_t to int16_t */
        scaleFactorBandsTotal =
            pSamplingRateInfo->NumberOfScaleFactorBands_Short;
        pSfbOffset = pSamplingRateInfo->ScaleFactorBands_Short;

        for (sfb = 0; sfb < scaleFactorBandsTotal; sfb++) {
          int32_t enAccu = (int32_t)(int32_t)1;
          int32_t sfbScale =
              (sizeof(int32_t) << 3) -
              CntLeadingZeros(pSfbOffset[sfb + 1] - pSfbOffset[sfb]) - 1;
          /* scaling depends on sfb width. */
          for (; line < pSfbOffset[sfb + 1] << 3; line++) {
            enAccu +=
                (enAccu + (fPow2Div2(*(spectrum + line)) >> sfbScale)) >> 3;
          }
          *(sfbEnergy + sfb) = CntLeadingZeros(enAccu) - 1;
        }
      }
      break;

    case BLOCK_SHORT:

      if (expandType == CConcealment_NoExpand) {
        /*   standard int16_t calculation */
        scaleFactorBandsTotal =
            pSamplingRateInfo->NumberOfScaleFactorBands_Short;
        pSfbOffset = pSamplingRateInfo->ScaleFactorBands_Short;

        for (sfb = 0; sfb < scaleFactorBandsTotal; sfb++) {
          int32_t enAccu = (int32_t)(int32_t)1;
          int32_t sfbScale =
              (sizeof(int32_t) << 3) -
              CntLeadingZeros(pSfbOffset[sfb + 1] - pSfbOffset[sfb]) - 1;
          /* scaling depends on sfb width. */
          for (; line < pSfbOffset[sfb + 1]; line++) {
            enAccu += fPow2Div2(*(spectrum + line)) >> sfbScale;
          }
          *(sfbEnergy + sfb) = CntLeadingZeros(enAccu) - 1;
        }
      } else {
        /*  expand int16_t to int32_t spectrum */
        scaleFactorBandsTotal =
            pSamplingRateInfo->NumberOfScaleFactorBands_Long;
        pSfbOffset = pSamplingRateInfo->ScaleFactorBands_Long;

        for (sfb = 0; sfb < scaleFactorBandsTotal; sfb++) {
          int32_t enAccu = (int32_t)(int32_t)1;
          int32_t sfbScale =
              (sizeof(int32_t) << 3) -
              CntLeadingZeros(pSfbOffset[sfb + 1] - pSfbOffset[sfb]) - 1;
          /* scaling depends on sfb width. */
          for (; line < pSfbOffset[sfb + 1]; line++) {
            enAccu += fPow2Div2(*(spectrum + (line >> 3))) >> sfbScale;
          }
          *(sfbEnergy + sfb) = CntLeadingZeros(enAccu) - 1;
        }
      }
      break;
  }
}

/*!
  \brief Interpolate buffer

  The function creates the interpolated spectral data according to the
  energy of the last good frame and the current (good) frame.
*/
static void CConcealment_InterpolateBuffer(int32_t *spectrum,
                                           int16_t *pSpecScalePrv,
                                           int16_t *pSpecScaleAct,
                                           int16_t *pSpecScaleOut, int32_t *enPrv,
                                           int32_t *enAct, int32_t sfbCnt,
                                           const int16_t *pSfbOffset) {
  int32_t sfb, line = 0;
  int32_t fac_shift;
  int32_t fac_mod;

  for (sfb = 0; sfb < sfbCnt; sfb++) {
    fac_shift =
        enPrv[sfb] - enAct[sfb] + ((*pSpecScaleAct - *pSpecScalePrv) << 1);
    fac_mod = fac_shift & 3;
    fac_shift = (fac_shift >> 2) + 1;
    fac_shift += *pSpecScalePrv - fixMax(*pSpecScalePrv, *pSpecScaleAct);
    fac_shift = fMax(fMin(fac_shift, DFRACT_BITS - 1), -(DFRACT_BITS - 1));

    for (; line < pSfbOffset[sfb + 1]; line++) {
      int32_t accu = fMult(*(spectrum + line), facMod4Table[fac_mod]);
      *(spectrum + line) = scaleValue(accu, fac_shift);
    }
  }
  *pSpecScaleOut = fixMax(*pSpecScalePrv, *pSpecScaleAct);
}

/*!
  \brief Find next fading frame in case of changing fading direction

  \param pConcealCommonData Pointer to the concealment common data structure.
  \param actFadeIndex Last index used for fading
  \param direction Direction of change: 0 : change from FADE-OUT to FADE-IN,  1
  : change from FADE-IN to FADE-OUT

  This function determines the next fading index to be used for the fading
  direction to be changed to.
*/

static int32_t findEquiFadeFrame(CConcealParams *pConcealCommonData,
                             int32_t actFadeIndex, int32_t direction) {
  FIXP_SGL *pFactor;
  FIXP_SGL referenceVal;
  FIXP_SGL minDiff = (FIXP_SGL)MAXVAL_SGL;

  int32_t nextFadeIndex = 0;

  int32_t i;

  /* init depending on direction */
  if (direction == 0) { /* FADE-OUT => FADE-IN */
    if (actFadeIndex < 0) {
      referenceVal = (FIXP_SGL)MAXVAL_SGL;
    } else {
      referenceVal = pConcealCommonData->fadeOutFactor[actFadeIndex] >> 1;
    }
    pFactor = pConcealCommonData->fadeInFactor;
  } else { /* FADE-IN => FADE-OUT */
    if (actFadeIndex < 0) {
      referenceVal = (FIXP_SGL)MAXVAL_SGL;
    } else {
      referenceVal = pConcealCommonData->fadeInFactor[actFadeIndex] >> 1;
    }
    pFactor = pConcealCommonData->fadeOutFactor;
  }

  /* search for minimum difference */
  for (i = 0; i < CONCEAL_MAX_NUM_FADE_FACTORS; i++) {
    FIXP_SGL diff = fixp_abs((pFactor[i] >> 1) - referenceVal);
    if (diff < minDiff) {
      minDiff = diff;
      nextFadeIndex = i;
    }
  }

  /* check and adjust depending on direction */
  if (direction == 0) { /* FADE-OUT => FADE-IN */
    if (nextFadeIndex > pConcealCommonData->numFadeInFrames) {
      nextFadeIndex = fMax(pConcealCommonData->numFadeInFrames - 1, 0);
    }
    if (((pFactor[nextFadeIndex] >> 1) <= referenceVal) &&
        (nextFadeIndex > 0)) {
      nextFadeIndex -= 1;
    }
  } else { /* FADE-IN => FADE-OUT */
    if (((pFactor[nextFadeIndex] >> 1) >= referenceVal) &&
        (nextFadeIndex < CONCEAL_MAX_NUM_FADE_FACTORS - 1)) {
      nextFadeIndex += 1;
    }
  }

  return (nextFadeIndex);
}

/*!
  \brief Update the concealment state

  The function updates the state of the concealment state-machine. The
  states are: mute, fade-in, fade-out, interpolate and frame-ok.
*/
static void CConcealment_UpdateState(
    CConcealmentInfo *pConcealmentInfo, int32_t frameOk,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo,
    const int32_t samplesPerFrame, CAacDecoderChannelInfo *pAacDecoderChannelInfo) {
  CConcealParams *pConcealCommonData = pConcealmentInfo->pConcealParams;

  switch (pConcealCommonData->method) {
    case ConcealMethodNoise: {
      if (pConcealmentInfo->concealState != ConcealState_Ok) {
        /* count the valid frames during concealment process */
        if (frameOk) {
          pConcealmentInfo->cntValidFrames += 1;
        } else {
          pConcealmentInfo->cntValidFrames = 0;
        }
      }

      /* -- STATE MACHINE for Noise Substitution -- */
      switch (pConcealmentInfo->concealState) {
        case ConcealState_Ok:
          if (!frameOk) {
            pConcealmentInfo->cntFadeFrames = 0;
            pConcealmentInfo->cntValidFrames = 0;
            pConcealmentInfo->attGrpOffset[0] = 0;
            pConcealmentInfo->attGrpOffset[1] = 0;
            pConcealmentInfo->winGrpOffset[0] = 0;
            pConcealmentInfo->winGrpOffset[1] = 0;
            if (pConcealCommonData->numFadeOutFrames > 0) {
              /* change to state SINGLE-FRAME-LOSS */
              pConcealmentInfo->concealState = ConcealState_Single;
              /* mode 0 just updates the Fading counter */
              CConcealment_ApplyFadeOut(
                  /*mode =*/0, pConcealmentInfo, pAacDecoderStaticChannelInfo,
                  samplesPerFrame, pAacDecoderChannelInfo);

            } else {
              /* change to state MUTE */
              pConcealmentInfo->concealState = ConcealState_Mute;
            }
          }
          break;

        case ConcealState_Single: /* Just a pre-stage before fade-out begins.
                                     Stay here only one frame! */
          if (frameOk) {
            /* change to state OK */
            pConcealmentInfo->concealState = ConcealState_Ok;
          } else {
            if (pConcealmentInfo->cntFadeFrames >=
                pConcealCommonData->numFadeOutFrames) {
              /* change to state MUTE */
              pConcealmentInfo->concealState = ConcealState_Mute;
            } else {
              /* change to state FADE-OUT */
              pConcealmentInfo->concealState = ConcealState_FadeOut;
              /* mode 0 just updates the Fading counter */
              CConcealment_ApplyFadeOut(
                  /*mode =*/0, pConcealmentInfo, pAacDecoderStaticChannelInfo,
                  samplesPerFrame, pAacDecoderChannelInfo);
            }
          }
          break;

        case ConcealState_FadeOut:
          if (pConcealmentInfo->cntValidFrames >
              pConcealCommonData->numMuteReleaseFrames) {
            if (pConcealCommonData->numFadeInFrames > 0) {
              /* change to state FADE-IN */
              pConcealmentInfo->concealState = ConcealState_FadeIn;
              pConcealmentInfo->cntFadeFrames = findEquiFadeFrame(
                  pConcealCommonData, pConcealmentInfo->cntFadeFrames,
                  0 /* FadeOut -> FadeIn */);
            } else {
              /* change to state OK */
              pConcealmentInfo->concealState = ConcealState_Ok;
            }
          } else {
            if (frameOk) {
              /* we have good frame information but stay fully in concealment -
               * reset winGrpOffset/attGrpOffset */
              pConcealmentInfo->winGrpOffset[0] = 0;
              pConcealmentInfo->winGrpOffset[1] = 0;
              pConcealmentInfo->attGrpOffset[0] = 0;
              pConcealmentInfo->attGrpOffset[1] = 0;
            }
            if (pConcealmentInfo->cntFadeFrames >=
                pConcealCommonData->numFadeOutFrames) {
              /* change to state MUTE */
              pConcealmentInfo->concealState = ConcealState_Mute;
            } else /* Stay in FADE-OUT */
            {
              /* mode 0 just updates the Fading counter */
              CConcealment_ApplyFadeOut(
                  /*mode =*/0, pConcealmentInfo, pAacDecoderStaticChannelInfo,
                  samplesPerFrame, pAacDecoderChannelInfo);
            }
          }
          break;

        case ConcealState_Mute:
          if (pConcealmentInfo->cntValidFrames >
              pConcealCommonData->numMuteReleaseFrames) {
            if (pConcealCommonData->numFadeInFrames > 0) {
              /* change to state FADE-IN */
              pConcealmentInfo->concealState = ConcealState_FadeIn;
              pConcealmentInfo->cntFadeFrames =
                  pConcealCommonData->numFadeInFrames - 1;
            } else {
              /* change to state OK */
              pConcealmentInfo->concealState = ConcealState_Ok;
            }
          } else {
            if (frameOk) {
              /* we have good frame information but stay fully in concealment -
               * reset winGrpOffset/attGrpOffset */
              pConcealmentInfo->winGrpOffset[0] = 0;
              pConcealmentInfo->winGrpOffset[1] = 0;
              pConcealmentInfo->attGrpOffset[0] = 0;
              pConcealmentInfo->attGrpOffset[1] = 0;
            }
          }
          break;

        case ConcealState_FadeIn:
          pConcealmentInfo->cntFadeFrames -= 1;
          if (frameOk) {
            if (pConcealmentInfo->cntFadeFrames < 0) {
              /* change to state OK */
              pConcealmentInfo->concealState = ConcealState_Ok;
            }
          } else {
            if (pConcealCommonData->numFadeOutFrames > 0) {
              /* change to state FADE-OUT */
              pConcealmentInfo->concealState = ConcealState_FadeOut;
              pConcealmentInfo->cntFadeFrames = findEquiFadeFrame(
                  pConcealCommonData, pConcealmentInfo->cntFadeFrames + 1,
                  1 /* FadeIn -> FadeOut */);
              pConcealmentInfo->winGrpOffset[0] = 0;
              pConcealmentInfo->winGrpOffset[1] = 0;
              pConcealmentInfo->attGrpOffset[0] = 0;
              pConcealmentInfo->attGrpOffset[1] = 0;

              pConcealmentInfo
                  ->cntFadeFrames--; /* decrease because
                                        CConcealment_ApplyFadeOut() will
                                        increase, accordingly */
              /* mode 0 just updates the Fading counter */
              CConcealment_ApplyFadeOut(
                  /*mode =*/0, pConcealmentInfo, pAacDecoderStaticChannelInfo,
                  samplesPerFrame, pAacDecoderChannelInfo);
            } else {
              /* change to state MUTE */
              pConcealmentInfo->concealState = ConcealState_Mute;
            }
          }
          break;

        default:
          FDK_ASSERT(0);
          break;
      }
    } break;

    case ConcealMethodInter:
    case ConcealMethodTonal: {
      if (pConcealmentInfo->concealState != ConcealState_Ok) {
        /* count the valid frames during concealment process */
        if (pConcealmentInfo->prevFrameOk[1] ||
            (pConcealmentInfo->prevFrameOk[0] &&
             !pConcealmentInfo->prevFrameOk[1] && frameOk)) {
          /* The frame is OK even if it can be estimated by the energy
           * interpolation algorithm */
          pConcealmentInfo->cntValidFrames += 1;
        } else {
          pConcealmentInfo->cntValidFrames = 0;
        }
      }

      /* -- STATE MACHINE for energy interpolation -- */
      switch (pConcealmentInfo->concealState) {
        case ConcealState_Ok:
          if (!(pConcealmentInfo->prevFrameOk[1] ||
                (pConcealmentInfo->prevFrameOk[0] &&
                 !pConcealmentInfo->prevFrameOk[1] && frameOk))) {
            if (pConcealCommonData->numFadeOutFrames > 0) {
              /* Fade out only if the energy interpolation algorithm can not be
               * applied! */
              pConcealmentInfo->concealState = ConcealState_FadeOut;
            } else {
              /* change to state MUTE */
              pConcealmentInfo->concealState = ConcealState_Mute;
            }
            pConcealmentInfo->cntFadeFrames = 0;
            pConcealmentInfo->cntValidFrames = 0;
          }
          break;

        case ConcealState_Single:
          pConcealmentInfo->concealState = ConcealState_Ok;
          break;

        case ConcealState_FadeOut:
          pConcealmentInfo->cntFadeFrames += 1;

          if (pConcealmentInfo->cntValidFrames >
              pConcealCommonData->numMuteReleaseFrames) {
            if (pConcealCommonData->numFadeInFrames > 0) {
              /* change to state FADE-IN */
              pConcealmentInfo->concealState = ConcealState_FadeIn;
              pConcealmentInfo->cntFadeFrames = findEquiFadeFrame(
                  pConcealCommonData, pConcealmentInfo->cntFadeFrames - 1,
                  0 /* FadeOut -> FadeIn */);
            } else {
              /* change to state OK */
              pConcealmentInfo->concealState = ConcealState_Ok;
            }
          } else {
            if (pConcealmentInfo->cntFadeFrames >=
                pConcealCommonData->numFadeOutFrames) {
              /* change to state MUTE */
              pConcealmentInfo->concealState = ConcealState_Mute;
            }
          }
          break;

        case ConcealState_Mute:
          if (pConcealmentInfo->cntValidFrames >
              pConcealCommonData->numMuteReleaseFrames) {
            if (pConcealCommonData->numFadeInFrames > 0) {
              /* change to state FADE-IN */
              pConcealmentInfo->concealState = ConcealState_FadeIn;
              pConcealmentInfo->cntFadeFrames =
                  pConcealCommonData->numFadeInFrames - 1;
            } else {
              /* change to state OK */
              pConcealmentInfo->concealState = ConcealState_Ok;
            }
          }
          break;

        case ConcealState_FadeIn:
          pConcealmentInfo->cntFadeFrames -=
              1; /* used to address the fade-in factors */

          if (frameOk || pConcealmentInfo->prevFrameOk[1]) {
            if (pConcealmentInfo->cntFadeFrames < 0) {
              /* change to state OK */
              pConcealmentInfo->concealState = ConcealState_Ok;
            }
          } else {
            if (pConcealCommonData->numFadeOutFrames > 0) {
              /* change to state FADE-OUT */
              pConcealmentInfo->concealState = ConcealState_FadeOut;
              pConcealmentInfo->cntFadeFrames = findEquiFadeFrame(
                  pConcealCommonData, pConcealmentInfo->cntFadeFrames + 1,
                  1 /* FadeIn -> FadeOut */);
            } else {
              /* change to state MUTE */
              pConcealmentInfo->concealState = ConcealState_Mute;
            }
          }
          break;
      } /* End switch(pConcealmentInfo->concealState) */
    } break;

    default:
      /* Don't need a state machine for other concealment methods. */
      break;
  }
}

/*!
\brief Randomizes the sign of the spectral data

  The function toggles the sign of the spectral data randomly. This is
  useful to ensure the quality of the concealed frames.
 */
static void CConcealment_ApplyRandomSign(int32_t randomPhase, int32_t *spec,
                                         int32_t samplesPerFrame) {
  int32_t i;
  uint16_t packedSign = 0;

  /* random table 512x16bit has been reduced to 512 packed sign bits = 32x16 bit
   */

  /* read current packed sign word */
  packedSign = AacDec_randomSign[randomPhase >> 4];
  packedSign >>= (randomPhase & 0xf);

  for (i = 0; i < samplesPerFrame; i++) {
    if ((randomPhase & 0xf) == 0) {
      packedSign = AacDec_randomSign[randomPhase >> 4];
    }

    if (packedSign & 0x1) {
      spec[i] = -fMax(spec[i], (int32_t)(MINVAL_DBL + 1));
    }
    packedSign >>= 1;

    randomPhase = (randomPhase + 1) & (AAC_NF_NO_RANDOM_VAL - 1);
  }
}

/*!
  \brief Get fadeing factor for current concealment state.

  The function returns the state (ok or not) of the previous frame.
  If called before the function CConcealment_Apply() set the fBeforeApply
  flag to get the correct value.

  \return Frame OK flag of previous frame.
 */
int32_t CConcealment_GetLastFrameOk(CConcealmentInfo *hConcealmentInfo,
                                const int32_t fBeforeApply) {
  int32_t prevFrameOk = 1;

  if (hConcealmentInfo != NULL) {
    prevFrameOk = hConcealmentInfo->prevFrameOk[fBeforeApply & 0x1];
  }

  return prevFrameOk;
}

/*!
  \brief Get the number of delay frames introduced by concealment technique.

  \return Number of delay frames.
 */
uint32_t CConcealment_GetDelay(CConcealParams *pConcealCommonData) {
  uint32_t frameDelay = 0;

  if (pConcealCommonData != NULL) {
    switch (pConcealCommonData->method) {
      case ConcealMethodTonal:
      case ConcealMethodInter:
        frameDelay = 1;
        break;
      default:
        break;
    }
  }

  return frameDelay;
}

static int32_t CConcealment_ApplyFadeOut(
    int32_t mode, CConcealmentInfo *pConcealmentInfo,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo,
    const int32_t samplesPerFrame, CAacDecoderChannelInfo *pAacDecoderChannelInfo) {
  /* mode 1 = apply RandomSign and mute spectral coefficients if necessary,  *
   * mode 0 = Update cntFadeFrames                                            */

  /* restore frequency coefficients from buffer with a specific muting */
  int32_t srcWin, dstWin, numWindows = 1;
  int32_t windowLen = samplesPerFrame;
  int32_t srcGrpStart = 0;
  int32_t winIdxStride = 1;
  int32_t numWinGrpPerFac, attIdx, attIdxStride;
  int32_t i;
  int32_t appliedProcessing = 0;

  CIcsInfo *pIcsInfo = &pAacDecoderChannelInfo->icsInfo;
  int32_t *pSpectralCoefficient =
      SPEC_LONG(pAacDecoderChannelInfo->pSpectralCoefficient);
  int16_t *pSpecScale = pAacDecoderChannelInfo->specScale;

  /* set old window parameters */
  if (pConcealmentInfo->lastRenderMode == AACDEC_RENDER_LPD) {
    switch (pAacDecoderStaticChannelInfo->last_lpd_mode) {
      case 1:
        numWindows = 4;
        srcGrpStart = 3;
        windowLen = samplesPerFrame >> 2;
        break;
      case 2:
        numWindows = 2;
        srcGrpStart = 1;
        windowLen = samplesPerFrame >> 1;
        winIdxStride = 2;
        break;
      case 3:
        numWindows = 1;
        srcGrpStart = 0;
        windowLen = samplesPerFrame;
        winIdxStride = 4;
        break;
    }
    pConcealmentInfo->lastWinGrpLen = 1;
  } else {
    pIcsInfo->WindowShape = pConcealmentInfo->windowShape;
    pIcsInfo->WindowSequence = pConcealmentInfo->windowSequence;

    if (pConcealmentInfo->windowSequence == BLOCK_SHORT) {
      /* int16_t block handling */
      numWindows = 8;
      windowLen = samplesPerFrame >> 3;
      srcGrpStart = numWindows - pConcealmentInfo->lastWinGrpLen;
    }
  }

  attIdxStride =
      fMax(1, (int32_t)(numWindows / (pConcealmentInfo->lastWinGrpLen + 1)));

  /* load last state */
  attIdx = pConcealmentInfo->cntFadeFrames;
  numWinGrpPerFac = pConcealmentInfo->attGrpOffset[mode];
  srcWin = srcGrpStart + pConcealmentInfo->winGrpOffset[mode];

  FDK_ASSERT((srcGrpStart * windowLen + windowLen) <= samplesPerFrame);
  FDK_ASSERT((srcWin * windowLen + windowLen) <= 1024);

  for (dstWin = 0; dstWin < numWindows; dstWin += 1) {
    FIXP_CNCL *pCncl =
        pConcealmentInfo->spectralCoefficient + (srcWin * windowLen);
    int32_t *pOut = pSpectralCoefficient + (dstWin * windowLen);

    if (mode == 1) {
      /* mute if attIdx gets large enaugh */
      if (attIdx > pConcealmentInfo->pConcealParams->numFadeOutFrames) {
        FDKmemclear(pCncl, sizeof(int32_t) * windowLen);
      }

      /* restore frequency coefficients from buffer - attenuation is done later
       */
      for (i = 0; i < windowLen; i++) {
        pOut[i] = pCncl[i];
      }

      /* apply random change of sign for spectral coefficients */
      CConcealment_ApplyRandomSign(pConcealmentInfo->iRandomPhase, pOut,
                                   windowLen);

      /* Increment random phase index to avoid repetition artifacts. */
      pConcealmentInfo->iRandomPhase =
          (pConcealmentInfo->iRandomPhase + 1) & (AAC_NF_NO_RANDOM_VAL - 1);

      /* set old scale factors */
      pSpecScale[dstWin * winIdxStride] =
          pConcealmentInfo->specScale[srcWin * winIdxStride];
    }

    srcWin += 1;

    if (srcWin >= numWindows) {
      /* end of sequence -> rewind to first window of group */
      srcWin = srcGrpStart;
      numWinGrpPerFac += 1;
      if (numWinGrpPerFac >= attIdxStride) {
        numWinGrpPerFac = 0;
        attIdx += 1;
      }
    }
  }

  /* store current state */

  pConcealmentInfo->winGrpOffset[mode] = srcWin - srcGrpStart;
  FDK_ASSERT((pConcealmentInfo->winGrpOffset[mode] >= 0) &&
             (pConcealmentInfo->winGrpOffset[mode] < 8));
  pConcealmentInfo->attGrpOffset[mode] = numWinGrpPerFac;
  FDK_ASSERT((pConcealmentInfo->attGrpOffset[mode] >= 0) &&
             (pConcealmentInfo->attGrpOffset[mode] < attIdxStride));

  if (mode == 0) {
    pConcealmentInfo->cntFadeFrames = attIdx;
  }

  appliedProcessing = 1;

  return appliedProcessing;
}

/*!
  \brief Do Time domain fading (TDFading) in concealment case

  In case of concealment, this function takes care of the fading, after time
domain signal has been rendered by the respective signal rendering functions.
  The fading out in case of ACELP decoding is not done by this function but by
the ACELP decoder for the first concealed frame if CONCEAL_CORE_IGNORANT_FADE is
not set.

  TimeDomain fading never creates jumps in energy / discontinuities, it always
does a continuous fading. To achieve this, fading is always done from a starting
point to a target point, while the starting point is always determined to be the
last target point. By varying the target point of a fading, the fading slope can
be controlled.

  This principle is applied to the fading within a frame and the fading from
frame to frame.

  One frame is divided into 8 subframes to obtain 8 parts of fading slopes
within a frame, each maybe with its own gradient.

  Workflow:
  1.) Determine Fading behavior and end-of-frame target fading level, based on
concealmentState (determined by CConcealment_UpdateState()) and the core mode.
        - By _DEFAULT_,
          The target fading level is determined by fadeOutFactor[cntFadeFrames]
in case of fadeOut, or fadeInFactor[cntFadeFrames] in case of fadeIn.
          --> fading type is FADE_TIMEDOMAIN in this case. Target fading level
is determined by fading index cntFadeFrames.

        - If concealmentState is signalling a _MUTED SIGNAL_,
          TDFading decays to 0 within 1/8th of a frame if numFadeOutFrames == 0.
          --> fading type is FADE_TIMEDOMAIN_TOSPECTRALMUTE in this case.

        - If concealmentState is signalling the _END OF MUTING_,
          TDFading fades to target fading level within 1/8th of a frame if
numFadeInFrames == 0.
          --> fading type is FADE_TIMEDOMAIN_FROMSPECTRALMUTE in this case.
Target fading level is determined by fading index cntFadeFrames.

#ifndef CONCEAL_CORE_IGNORANT_FADE
        - In case of an _ACELP FADEOUT_,
          TDFading leaves fading control to ACELP decoder for 1/2 frame.
          --> fading type is FADE_ACELPDOMAIN in this case.
#endif

  2.) Render fading levels within current frame and do the final fading:
      Map Fading slopes to fading levels and apply to time domain signal.


*/

int32_t CConcealment_TDFading(
    int32_t len, CAacDecoderStaticChannelInfo **ppAacDecoderStaticChannelInfo,
    const int32_t aacOutDataHeadroom, int32_t *pcmdata, int32_t *pcmdata_1) {
  /*
  Do the fading in Time domain based on concealment states and core mode
  */
  int32_t fadeStop, attMute = (int32_t)0;
  int32_t idx = 0, ii;
  CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo =
      *ppAacDecoderStaticChannelInfo;
  CConcealmentInfo *pConcealmentInfo =
      &pAacDecoderStaticChannelInfo->concealmentInfo;
  CConcealParams *pConcealParams = pConcealmentInfo->pConcealParams;
  const CConcealmentState concealState = pConcealmentInfo->concealState;
  TDfadingType fadingType;
  int32_t fadingStations[9] = {0};
  int32_t fadingSteps[8] = {0};
  const int32_t fadeStart =
      pConcealmentInfo
          ->fade_old; /* start fading at last end-of-frame attenuation */
  FIXP_SGL *fadeFactor = pConcealParams->fadeOutFactor;
  const int32_t cntFadeFrames = pConcealmentInfo->cntFadeFrames;
  int32_t TDFadeOutStopBeforeMute = 1;
  int32_t TDFadeInStopBeforeFullLevel = 1;

  /*
  determine Fading behaviour (end-of-frame attenuation and fading type) (1.)
  */

  switch (concealState) {
    case ConcealState_Single:
    case ConcealState_Mute:
    case ConcealState_FadeOut:
      idx = (pConcealParams->method == ConcealMethodNoise) ? cntFadeFrames - 1
                                                           : cntFadeFrames;
      fadingType = FADE_TIMEDOMAIN;

      if (concealState == ConcealState_Mute ||
          (cntFadeFrames + TDFadeOutStopBeforeMute) >
              pConcealmentInfo->pConcealParams->numFadeOutFrames) {
        fadingType = FADE_TIMEDOMAIN_TOSPECTRALMUTE;
      }

      break;
    case ConcealState_FadeIn:
      idx = cntFadeFrames;
      idx -= TDFadeInStopBeforeFullLevel;
      FDK_FALLTHROUGH;
    case ConcealState_Ok:
      fadeFactor = pConcealParams->fadeInFactor;
      idx = (concealState == ConcealState_Ok) ? -1 : idx;
      fadingType = (pConcealmentInfo->concealState_old == ConcealState_Mute)
                       ? FADE_TIMEDOMAIN_FROMSPECTRALMUTE
                       : FADE_TIMEDOMAIN;
      break;
    default:
      FDK_ASSERT(0);
      fadingType = FADE_TIMEDOMAIN_TOSPECTRALMUTE;
      break;
  }

  /* determine Target end-of-frame fading level and fading slope */
  switch (fadingType) {
    case FADE_TIMEDOMAIN_FROMSPECTRALMUTE:
      fadeStop =
          (idx < 0) ? (int32_t)MAXVAL_DBL : FX_SGL2FX_DBL(fadeFactor[idx]);
      if (pConcealmentInfo->pConcealParams->numFadeInFrames == 0) {
        /* do step as fast as possible */
        fadingSteps[0] = 1;
        break;
      }
      CConcealment_TDFading_doLinearFadingSteps(&fadingSteps[0]);
      break;
    case FADE_TIMEDOMAIN:
      fadeStop =
          (idx < 0) ? (int32_t)MAXVAL_DBL : FX_SGL2FX_DBL(fadeFactor[idx]);
      CConcealment_TDFading_doLinearFadingSteps(&fadingSteps[0]);
      break;
    case FADE_TIMEDOMAIN_TOSPECTRALMUTE:
      fadeStop = attMute;
      if (pConcealmentInfo->pConcealParams->numFadeOutFrames == 0) {
        /* do step as fast as possible */
        fadingSteps[0] = 1;
        break;
      }
      CConcealment_TDFading_doLinearFadingSteps(&fadingSteps[0]);
      break;
  }

  /*
  Render fading levels within current frame and do the final fading (2.)
  */

  len >>= 3;
  CConcealment_TDFadeFillFadingStations(fadingStations, fadingSteps, fadeStop,
                                        fadeStart, fadingType);

  if ((fadingStations[8] != (int32_t)MAXVAL_DBL) ||
      (fadingStations[7] != (int32_t)MAXVAL_DBL) ||
      (fadingStations[6] != (int32_t)MAXVAL_DBL) ||
      (fadingStations[5] != (int32_t)MAXVAL_DBL) ||
      (fadingStations[4] != (int32_t)MAXVAL_DBL) ||
      (fadingStations[3] != (int32_t)MAXVAL_DBL) ||
      (fadingStations[2] != (int32_t)MAXVAL_DBL) ||
      (fadingStations[1] != (int32_t)MAXVAL_DBL) ||
      (fadingStations[0] !=
       (int32_t)MAXVAL_DBL)) /* if there's something to fade */
  {
    int32_t start = 0;
    for (ii = 0; ii < 8; ii++) {
      CConcealment_TDFadePcmAtt(start, len, fadingStations[ii],
                                fadingStations[ii + 1], pcmdata);
      start += len;
    }
  }
  CConcealment_TDNoise_Apply(pConcealmentInfo, len, aacOutDataHeadroom,
                             pcmdata);

  /* Save end-of-frame attenuation and fading type */
  pConcealmentInfo->lastFadingType = fadingType;
  pConcealmentInfo->fade_old = fadeStop;
  pConcealmentInfo->concealState_old = concealState;

  return 1;
}

/* attenuate pcmdata in Time Domain Fading process */
static void CConcealment_TDFadePcmAtt(int32_t start, int32_t len, int32_t fadeStart,
                                      int32_t fadeStop, int32_t *pcmdata) {
  int32_t i;
  int32_t dStep;
  int32_t dGain;
  int32_t dGain_apply;

  /* set start energy */
  dGain = fadeStart;
  /* determine energy steps from sample to sample */
  dStep = (int32_t)((int32_t)((fadeStart >> 1) - (fadeStop >> 1)) / len) << 1;

  for (i = start; i < (start + len); i++) {
    dGain -= dStep;
    /* prevent gain from getting negative due to possible fixpoint inaccuracies
     */
    dGain_apply = fMax((int32_t)0, dGain);
    /* finally, attenuate samples */
    pcmdata[i] = FIXP_DBL2PCM_DEC(fMult(pcmdata[i], dGain_apply));
  }
}

/*
\brief Fill FadingStations

The fadingstations are the attenuation factors, being applied to its dedicated
portions of pcm data. They are calculated using the fadingsteps. One fadingstep
is the weighted contribution to the fading slope within its dedicated portion of
pcm data.

*Fadingsteps  :      0  0  0  1  0  1  2  0

                  |<-  1 Frame pcm data ->|
      fadeStart-->|__________             |
                  ^  ^  ^  ^ \____        |
 Attenuation  :   |  |  |  |  ^  ^\__     |
                  |  |  |  |  |  |  ^\    |
                  |  |  |  |  |  |  | \___|<-- fadeStop
                  |  |  |  |  |  |  |  ^  ^
                  |  |  |  |  |  |  |  |  |
Fadingstations:  [0][1][2][3][4][5][6][7][8]

(Fadingstations "[0]" is "[8] from previous frame", therefore its not meaningful
to be edited)

*/
static void CConcealment_TDFadeFillFadingStations(int32_t *fadingStations,
                                                  int32_t *fadingSteps,
                                                  int32_t fadeStop,
                                                  int32_t fadeStart,
                                                  TDfadingType fadingType) {
  int32_t i;
  int32_t fadingSteps_sum = 0;
  int32_t fadeDiff;

  fadingSteps_sum = fadingSteps[0] + fadingSteps[1] + fadingSteps[2] +
                    fadingSteps[3] + fadingSteps[4] + fadingSteps[5] +
                    fadingSteps[6] + fadingSteps[7];
  fadeDiff = ((int32_t)(fadeStop - fadeStart) / fMax(fadingSteps_sum, (int32_t)1));
  fadingStations[0] = fadeStart;
  for (i = 1; i < 8; i++) {
    fadingStations[i] =
        fadingStations[i - 1] + (int32_t)(fadeDiff * fadingSteps[i - 1]);
  }
  fadingStations[8] = fadeStop;
}

static void CConcealment_TDFading_doLinearFadingSteps(int32_t *fadingSteps) {
  fadingSteps[0] = fadingSteps[1] = fadingSteps[2] = fadingSteps[3] =
      fadingSteps[4] = fadingSteps[5] = fadingSteps[6] = fadingSteps[7] = 1;
}

/* end of TimeDomainFading functions */

/* derived from int32_t UsacRandomSign() */
static int32_t CConcealment_TDNoise_Random(uint32_t *seed) {
  *seed = (uint32_t)(((uint64_t)(*seed) * 69069) + 5);
  return (int32_t)(*seed);
}

static void CConcealment_TDNoise_Apply(CConcealmentInfo *const pConcealmentInfo,
                                       const int32_t len,
                                       const int32_t aacOutDataHeadroom,
                                       int32_t *const pcmdata) {
  int32_t *states = pConcealmentInfo->TDNoiseStates;
  int32_t noiseVal;
  int32_t noiseValLong;
  FIXP_SGL *coef = pConcealmentInfo->TDNoiseCoef;
  int32_t TDNoiseAtt;
  uint32_t seed = pConcealmentInfo->TDNoiseSeed =
      (uint32_t)CConcealment_TDNoise_Random(&pConcealmentInfo->TDNoiseSeed) + 1;

  TDNoiseAtt = pConcealmentInfo->pConcealParams->comfortNoiseLevel;

  int32_t ii;

  if ((pConcealmentInfo->concealState != ConcealState_Ok ||
       pConcealmentInfo->concealState_old != ConcealState_Ok) &&
      TDNoiseAtt != (int32_t)0) {
    for (ii = 0; ii < (len << 3); ii++) {
      /* create filtered noise */
      states[2] = states[1];
      states[1] = states[0];
      states[0] =
          FIXP_DBL2PCM_DEC((int32_t)CConcealment_TDNoise_Random(&seed));
      noiseValLong = fMult(states[0], coef[0]) + fMult(states[1], coef[1]) +
                     fMult(states[2], coef[2]);
      noiseVal = FIXP_DBL2PCM_DEC(fMult(noiseValLong, TDNoiseAtt) >>
                                  aacOutDataHeadroom);

      /* add filtered noise - check for clipping, before */
      if (noiseVal > (int32_t)0 &&
          pcmdata[ii] > (int32_t)MAXVAL_DBL - noiseVal) {
        noiseVal = noiseVal * (int32_t)-1;
      } else if (noiseVal < (int32_t)0 &&
                 pcmdata[ii] < (int32_t)MINVAL_DBL - noiseVal) {
        noiseVal = noiseVal * (int32_t)-1;
      }

      pcmdata[ii] += noiseVal;
    }
  }
}
