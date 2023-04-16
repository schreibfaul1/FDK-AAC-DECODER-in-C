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

/************************* MPEG-D DRC decoder library **************************

   Author(s):   Andreas Hoelzer

   Description: DRC Set Selection

*******************************************************************************/

#include <memory.h>
#include "drcDec_selectionProcess.h"
#include "drcDec_tools.h"

typedef enum {
  DETR_NONE = 0,
  DETR_NIGHT = 1,
  DETR_NOISY = 2,
  DETR_LIMITED = 3,
  DETR_LOWLEVEL = 4,
  DETR_DIALOG = 5,
  DETR_GENERAL_COMPR = 6,
  DETR_EXPAND = 7,
  DETR_ARTISTIC = 8,
  DETR_COUNT
} DRC_EFFECT_TYPE_REQUEST;

typedef enum {
  DFRT_EFFECT_TYPE,
  DFRT_DYNAMIC_RANGE,
  DFRT_DRC_CHARACTERISTIC
} DRC_FEATURE_REQUEST_TYPE;

typedef enum {
  MDR_DEFAULT = 0,
  MDR_PROGRAM_LOUDNESS = 1,
  MDR_ANCHOR_LOUDNESS = 2
} METHOD_DEFINITION_REQUEST;

typedef enum {
  MSR_DEFAULT = 0,
  MSR_BS_1770_4 = 1,
  MSR_USER = 2,
  MSR_EXPERT_PANEL = 3,
  MSR_RESERVED_A = 4,
  MSR_RESERVED_B = 5,
  MSR_RESERVED_C = 6,
  MSR_RESERVED_D = 7,
  MSR_RESERVED_E = 8
} MEASUREMENT_SYSTEM_REQUEST;

typedef enum {
  LPR_DEFAULT = 0,
  LPR_OFF = 1,
  LPR_HIGHPASS = 2
} LOUDNESS_PREPROCESSING_REQUEST;

typedef enum {
  DRMRT_SHORT_TERM_LOUDNESS_TO_AVG = 0,
  DRMRT_MOMENTARY_LOUDNESS_TO_AVG = 1,
  DRMRT_TOP_OF_LOUDNESS_RANGE_TO_AVG = 2
} DYN_RANGE_MEASUREMENT_REQUEST_TYPE;

typedef enum {
  TCRT_DOWNMIX_ID = 0,
  TCRT_TARGET_LAYOUT = 1,
  TCRT_TARGET_CHANNEL_COUNT = 2
} TARGET_CONFIG_REQUEST_TYPE;

typedef shouldBeUnion {
  struct {
    uint8_t numRequests;
    uint8_t numRequestsDesired;
    DRC_EFFECT_TYPE_REQUEST request[MAX_REQUESTS_DRC_EFFECT_TYPE];
  } drcEffectType;
  struct {
    DYN_RANGE_MEASUREMENT_REQUEST_TYPE measurementRequestType;
    uint8_t requestedIsRange;
    int32_t requestValue;    /* e = 7 */
    int32_t requestValueMin; /* e = 7 */
    int32_t requestValueMax; /* e = 7 */
  } dynamicRange;
  uint8_t drcCharacteristic;
}
DRC_FEATURE_REQUEST;

typedef struct {
  /* system parameters */
  int8_t baseChannelCount;
  int8_t baseLayout; /* not supported */
  TARGET_CONFIG_REQUEST_TYPE targetConfigRequestType;
  uint8_t numDownmixIdRequests;
  uint8_t downmixIdRequested[MAX_REQUESTS_DOWNMIX_ID];
  uint8_t targetLayoutRequested;
  uint8_t targetChannelCountRequested;
  int32_t audioSampleRate; /* needed for complexity estimation, currently not
                           supported */

  /* loudness normalization parameters */
  uint8_t loudnessNormalizationOn;
  int32_t targetLoudness; /* e = 7 */
  uint8_t albumMode;
  uint8_t peakLimiterPresent;
  uint8_t loudnessDeviationMax; /* resolution: 1 dB */
  METHOD_DEFINITION_REQUEST loudnessMeasurementMethod;
  MEASUREMENT_SYSTEM_REQUEST loudnessMeasurementSystem;
  LOUDNESS_PREPROCESSING_REQUEST loudnessMeasurementPreProc; /* not supported */
  int32_t deviceCutOffFrequency;                                /* not supported */
  int32_t loudnessNormalizationGainDbMax;                   /* e = 7 */
  int32_t loudnessNormalizationGainModificationDb;          /* e = 7 */
  int32_t outputPeakLevelMax;                               /* e = 7 */

  /* dynamic range control parameters */
  uint8_t dynamicRangeControlOn;
  uint8_t numDrcFeatureRequests;
  DRC_FEATURE_REQUEST_TYPE drcFeatureRequestType[MAX_REQUESTS_DRC_FEATURE];
  DRC_FEATURE_REQUEST drcFeatureRequest[MAX_REQUESTS_DRC_FEATURE];

  /* other */
  int16_t boost;                /* e = 1 */
  int16_t compress;             /* e = 1 */
  uint8_t drcCharacteristicTarget; /* not supported */
} SEL_PROC_INPUT, *HANDLE_SEL_PROC_INPUT;

/* Table E.1 of ISO/IEC DIS 23003-4: Recommended order of fallback effect type
 * requests */
static DRC_EFFECT_TYPE_REQUEST fallbackEffectTypeRequests[6][5] = {
    /* Night */ {DETR_GENERAL_COMPR, DETR_NOISY, DETR_LIMITED, DETR_LOWLEVEL,
                 DETR_DIALOG},
    /* Noisy */
    {DETR_GENERAL_COMPR, DETR_NIGHT, DETR_LIMITED, DETR_LOWLEVEL, DETR_DIALOG},
    /* Limited */
    {DETR_GENERAL_COMPR, DETR_NIGHT, DETR_NOISY, DETR_LOWLEVEL, DETR_DIALOG},
    /* LowLevel */
    {DETR_GENERAL_COMPR, DETR_NOISY, DETR_NIGHT, DETR_LIMITED, DETR_DIALOG},
    /* Dialog */
    {DETR_GENERAL_COMPR, DETR_NIGHT, DETR_NOISY, DETR_LIMITED, DETR_LOWLEVEL},
    /* General */
    {DETR_NIGHT, DETR_NOISY, DETR_LIMITED, DETR_LOWLEVEL, DETR_DIALOG}};

/*******************************************/
typedef struct {
  uint8_t selectionFlag;
  uint8_t downmixIdRequestIndex;
  int32_t outputPeakLevel;                     /* e = 7 */
  int32_t loudnessNormalizationGainDbAdjusted; /* e = 7 */
  int32_t outputLoudness;                      /* e = 7 */
  DRC_INSTRUCTIONS_UNI_DRC* pInst;

} DRCDEC_SELECTION_DATA;

typedef struct {
  uint8_t numData;
  DRCDEC_SELECTION_DATA data[(12 + 1 + 6)];

} DRCDEC_SELECTION;

/*******************************************/
/* helper functions                        */
/*******************************************/

static int32_t _isError(int32_t x) {
  if (x < DRCDEC_SELECTION_PROCESS_WARNING) {
    return 1;
  }

  return 0;
}

/* compare and assign */
static inline int32_t _compAssign(uint8_t* dest, const uint8_t src) {
  int32_t diff = 0;
  if (*dest != src) diff = 1;
  *dest = src;
  return diff;
}

static inline int32_t _compAssign(int8_t* dest, const int8_t src) {
  int32_t diff = 0;
  if (*dest != src) diff = 1;
  *dest = src;
  return diff;
}

static inline int32_t _compAssign(int32_t* dest, const int32_t src) {
  int32_t diff = 0;
  if (*dest != src) diff = 1;
  *dest = src;
  return diff;
}

static inline int32_t _compAssign(int16_t* dest, const int16_t src) {
  int32_t diff = 0;
  if (*dest != src) diff = 1;
  *dest = src;
  return diff;
}

static inline int32_t _compAssign(TARGET_CONFIG_REQUEST_TYPE* dest, const int32_t src) {
  int32_t diff = 0;
  if (*dest != src) diff = 1;
  *dest = (TARGET_CONFIG_REQUEST_TYPE)src;
  return diff;
}

static inline int32_t _compAssign(METHOD_DEFINITION_REQUEST* dest, const int32_t src) {
  int32_t diff = 0;
  if (*dest != src) diff = 1;
  *dest = (METHOD_DEFINITION_REQUEST)src;
  return diff;
}

static inline int32_t _compAssign(DRC_FEATURE_REQUEST_TYPE* dest, const int32_t src) {
  int32_t diff = 0;
  if (*dest != src) diff = 1;
  *dest = (DRC_FEATURE_REQUEST_TYPE)src;
  return diff;
}

static inline int32_t _compAssign(DRC_EFFECT_TYPE_REQUEST* dest, const int32_t src) {
  int32_t diff = 0;
  if (*dest != src) diff = 1;
  *dest = (DRC_EFFECT_TYPE_REQUEST)src;
  return diff;
}

static DRCDEC_SELECTION_DATA* _drcdec_selection_addNew(
    DRCDEC_SELECTION* pSelection);

static DRCDEC_SELECTION_DATA* _drcdec_selection_add(
    DRCDEC_SELECTION* pSelection, DRCDEC_SELECTION_DATA* pDataIn);

static int32_t _drcdec_selection_clear(DRCDEC_SELECTION* pSelection);

static int32_t _drcdec_selection_getNumber(DRCDEC_SELECTION* pSelection);

static int32_t _drcdec_selection_setNumber(DRCDEC_SELECTION* pSelection, int32_t num);

static DRCDEC_SELECTION_DATA* _drcdec_selection_getAt(
    DRCDEC_SELECTION* pSelection, int32_t at);

static int32_t _swapSelectionAndClear(DRCDEC_SELECTION** ppCandidatesPotential,
                                  DRCDEC_SELECTION** ppCandidatesSelected);

static int32_t _swapSelection(DRCDEC_SELECTION** ppCandidatesPotential,
                          DRCDEC_SELECTION** ppCandidatesSelected);

/*******************************************/
/* declarations of static functions        */
/*******************************************/

static DRCDEC_SELECTION_PROCESS_RETURN _initDefaultParams(
    HANDLE_SEL_PROC_INPUT hSelProcInput);

static DRCDEC_SELECTION_PROCESS_RETURN _initCodecModeParams(
    HANDLE_SEL_PROC_INPUT hSelProcInput, const SEL_PROC_CODEC_MODE codecMode);

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetPreSelection(
    SEL_PROC_INPUT* hSelProcInput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
    DRCDEC_SELECTION** ppCandidatesPotential,
    DRCDEC_SELECTION** ppCandidatesSelected, SEL_PROC_CODEC_MODE codecMode);

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetFinalSelection_peakValue0(
    DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected);

static DRCDEC_SELECTION_PROCESS_RETURN _dynamicRangeMeasurement(
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet, DRC_INSTRUCTIONS_UNI_DRC* pInst,
    uint8_t downmixIdRequested,
    DYN_RANGE_MEASUREMENT_REQUEST_TYPE dynamicRangeMeasurementType,
    int32_t albumMode, int32_t* peakToAveragePresent, int32_t* peakToAverage);

static DRCDEC_SELECTION_PROCESS_RETURN _channelLayoutToDownmixIdMapping(
    HANDLE_SEL_PROC_INPUT hSelProcInput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig);

static DRCDEC_SELECTION_PROCESS_RETURN _generateVirtualDrcSets(
    HANDLE_SEL_PROC_INPUT hSelProcInput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    SEL_PROC_CODEC_MODE codecMode);

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetRequestSelection(
    SEL_PROC_INPUT* hSelProcInput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
    DRCDEC_SELECTION** ppCandidatesPotential,
    DRCDEC_SELECTION** ppCandidatesSelected);

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetFinalSelection(
    HANDLE_SEL_PROC_INPUT hSelProcInput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    DRCDEC_SELECTION** ppCandidatesPotential,
    DRCDEC_SELECTION** ppCandidatesSelected, SEL_PROC_CODEC_MODE codecMode);

static DRCDEC_SELECTION_PROCESS_RETURN _generateOutputInfo(
    HANDLE_SEL_PROC_INPUT hSelProcInput, HANDLE_SEL_PROC_OUTPUT hSelProcOutput,
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
    DRCDEC_SELECTION_DATA* pSelectionData, SEL_PROC_CODEC_MODE codecMode);

static DRCDEC_SELECTION_PROCESS_RETURN _selectDownmixMatrix(
    HANDLE_SEL_PROC_OUTPUT hSelProcOutput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig);

static DRCDEC_SELECTION_PROCESS_RETURN _getLoudness(
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet, int32_t albumMode,
    METHOD_DEFINITION_REQUEST measurementMethodRequested,
    MEASUREMENT_SYSTEM_REQUEST measurementSystemRequested,
    int32_t targetLoudness, int32_t drcSetId, int32_t downmixIdRequested,
    int32_t* pLoudnessNormalizationGain, int32_t* pLoudness);

static DRCDEC_SELECTION_PROCESS_RETURN _getMixingLevel(
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet, int32_t downmixIdRequested,
    int32_t drcSetIdRequested, int32_t albumMode, int32_t* pMixingLevel);

static DRCDEC_SELECTION_PROCESS_RETURN _getSignalPeakLevel(
    HANDLE_SEL_PROC_INPUT hSelProcInput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet, DRC_INSTRUCTIONS_UNI_DRC* pInst,
    int32_t downmixIdRequested, int32_t* explicitPeakInformationPresent,
    int32_t* signalPeakLevelOut, /* e = 7 */
    SEL_PROC_CODEC_MODE codecMode);

static DRCDEC_SELECTION_PROCESS_RETURN _extractLoudnessPeakToAverageValue(
    LOUDNESS_INFO* loudnessInfo,
    DYN_RANGE_MEASUREMENT_REQUEST_TYPE dynamicRangeMeasurementType,
    int32_t* pLoudnessPeakToAverageValuePresent,
    int32_t* pLoudnessPeakToAverageValue);

static DRCDEC_SELECTION_PROCESS_RETURN _selectAlbumLoudness(
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
    DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected);

static int32_t _findMethodDefinition(LOUDNESS_INFO* pLoudnessInfo,
                                 int32_t methodDefinition, int32_t startIndex);

/*******************************************/
/* public functions                        */
/*******************************************/

struct s_drcdec_selection_process {
  SEL_PROC_CODEC_MODE codecMode;
  SEL_PROC_INPUT selProcInput;
  DRCDEC_SELECTION
  selectionData[2]; /* 2 instances, one before and one after selection */
};

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_Create(HANDLE_DRC_SELECTION_PROCESS* phInstance) {
  HANDLE_DRC_SELECTION_PROCESS hInstance;
  hInstance = (HANDLE_DRC_SELECTION_PROCESS)FDKcalloc(
      1, sizeof(struct s_drcdec_selection_process));

  if (!hInstance) return DRCDEC_SELECTION_PROCESS_OUTOFMEMORY;

  hInstance->codecMode = SEL_PROC_CODEC_MODE_UNDEFINED;

  *phInstance = hInstance;
  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_Init(HANDLE_DRC_SELECTION_PROCESS hInstance) {
  if (!hInstance) return DRCDEC_SELECTION_PROCESS_NOT_OK;

  _initDefaultParams(&hInstance->selProcInput);
  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_SetCodecMode(HANDLE_DRC_SELECTION_PROCESS hInstance,
                                     const SEL_PROC_CODEC_MODE codecMode) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;

  if (!hInstance) return DRCDEC_SELECTION_PROCESS_NOT_OK;

  switch (codecMode) {
    case SEL_PROC_MPEG_4_AAC:
    case SEL_PROC_MPEG_D_USAC:
    case SEL_PROC_TEST_TIME_DOMAIN:
    case SEL_PROC_TEST_QMF_DOMAIN:
    case SEL_PROC_TEST_STFT_DOMAIN:
      hInstance->codecMode = codecMode;
      break;

    case SEL_PROC_CODEC_MODE_UNDEFINED:
    default:
      return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  retVal = _initCodecModeParams(&(hInstance->selProcInput),
                                hInstance->codecMode = codecMode);

  return retVal;
}

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_SetParam(HANDLE_DRC_SELECTION_PROCESS hInstance,
                                 const SEL_PROC_USER_PARAM requestType,
                                 int32_t requestValue, int32_t* pDiff) {
  int32_t requestValueInt = (int32_t)requestValue;
  int32_t i, diff = 0;
  SEL_PROC_INPUT* pSelProcInput = &(hInstance->selProcInput);

  switch (requestType) {
    case SEL_PROC_LOUDNESS_NORMALIZATION_ON:
      if ((requestValueInt != 0) && (requestValueInt != 1))
        return DRCDEC_SELECTION_PROCESS_PARAM_OUT_OF_RANGE;
      diff |=
          _compAssign(&pSelProcInput->loudnessNormalizationOn, requestValueInt);
      break;
    case SEL_PROC_TARGET_LOUDNESS:
      /* Lower boundary: drcSetTargetLoudnessValueLower default value.
         Upper boundary: drcSetTargetLoudnessValueUpper default value */
      if ((requestValue < FL2FXCONST_DBL(-63.0f / (float)(1 << 7))) ||
          (requestValue > (int32_t)0))
        return DRCDEC_SELECTION_PROCESS_PARAM_OUT_OF_RANGE;
      if (requestValue >
          FL2FXCONST_DBL(-10.0f /
                         (float)(1 << 7))) /* recommended maximum value */
        requestValue = FL2FXCONST_DBL(-10.0f / (float)(1 << 7));
      diff |= _compAssign(&pSelProcInput->targetLoudness, requestValue);
      break;
    case SEL_PROC_EFFECT_TYPE:
      if ((requestValueInt < -1) || (requestValueInt >= DETR_COUNT))
        return DRCDEC_SELECTION_PROCESS_PARAM_OUT_OF_RANGE;
      /* Caution. This overrides all drcFeatureRequests requested so far! */
      if (requestValueInt == -1) {
        diff |= _compAssign(&pSelProcInput->dynamicRangeControlOn, 0);
      } else if (requestValueInt == DETR_NONE) {
        diff |= _compAssign(&pSelProcInput->dynamicRangeControlOn, 1);
        diff |= _compAssign(&pSelProcInput->numDrcFeatureRequests, 0);
      } else {
        diff |= _compAssign(&pSelProcInput->dynamicRangeControlOn, 1);
        diff |= _compAssign(&pSelProcInput->numDrcFeatureRequests, 1);
        diff |= _compAssign(&pSelProcInput->drcFeatureRequestType[0],
                            DFRT_EFFECT_TYPE);
        diff |= _compAssign(&pSelProcInput->drcFeatureRequest[0]
                                 .drcEffectType.numRequestsDesired,
                            1);
        diff |= _compAssign(
            &pSelProcInput->drcFeatureRequest[0].drcEffectType.request[0],
            requestValueInt);
        if ((requestValueInt > DETR_NONE) &&
            (requestValueInt <= DETR_GENERAL_COMPR)) {
          /* use fallback effect type requests */
          for (i = 0; i < 5; i++) {
            diff |=
                _compAssign(&pSelProcInput->drcFeatureRequest[0]
                                 .drcEffectType.request[i + 1],
                            fallbackEffectTypeRequests[requestValueInt - 1][i]);
          }
          diff |= _compAssign(
              &pSelProcInput->drcFeatureRequest[0].drcEffectType.numRequests,
              6);
        } else {
          diff |= _compAssign(
              &pSelProcInput->drcFeatureRequest[0].drcEffectType.numRequests,
              1);
        }
      }
      break;
    case SEL_PROC_LOUDNESS_MEASUREMENT_METHOD:
      if ((requestValueInt < 0) || (requestValueInt > 2))
        return DRCDEC_SELECTION_PROCESS_PARAM_OUT_OF_RANGE;
      diff |= _compAssign(&pSelProcInput->loudnessMeasurementMethod,
                          requestValueInt);
      break;
    case SEL_PROC_ALBUM_MODE:
      if ((requestValueInt < 0) || (requestValueInt > 1))
        return DRCDEC_SELECTION_PROCESS_PARAM_OUT_OF_RANGE;
      diff |= _compAssign(&pSelProcInput->albumMode, requestValueInt);
      break;
    case SEL_PROC_DOWNMIX_ID:
      diff |=
          _compAssign(&pSelProcInput->targetConfigRequestType, TCRT_DOWNMIX_ID);
      if (requestValueInt < 0) { /* negative requests signal no downmixId */
        diff |= _compAssign(&pSelProcInput->numDownmixIdRequests, 0);
      } else {
        diff |= _compAssign(&pSelProcInput->numDownmixIdRequests, 1);
        diff |=
            _compAssign(&pSelProcInput->downmixIdRequested[0], requestValueInt);
      }
      break;
    case SEL_PROC_TARGET_LAYOUT:
      /* Request target layout according to ChannelConfiguration in ISO/IEC
       * 23001-8 (CICP) */
      if ((requestValueInt < 1) || (requestValueInt > 63))
        return DRCDEC_SELECTION_PROCESS_PARAM_OUT_OF_RANGE;
      diff |= _compAssign(&pSelProcInput->targetConfigRequestType,
                          TCRT_TARGET_LAYOUT);
      diff |=
          _compAssign(&pSelProcInput->targetLayoutRequested, requestValueInt);
      break;
    case SEL_PROC_TARGET_CHANNEL_COUNT:
      if ((requestValueInt < 1) || (requestValueInt > 8))
        return DRCDEC_SELECTION_PROCESS_PARAM_OUT_OF_RANGE;
      diff |= _compAssign(&pSelProcInput->targetConfigRequestType,
                          TCRT_TARGET_CHANNEL_COUNT);
      diff |= _compAssign(&pSelProcInput->targetChannelCountRequested,
                          requestValueInt);
      break;
    case SEL_PROC_BASE_CHANNEL_COUNT:
      if (requestValueInt < 0)
        return DRCDEC_SELECTION_PROCESS_PARAM_OUT_OF_RANGE;
      diff |= _compAssign(&pSelProcInput->baseChannelCount, requestValueInt);
      break;
    case SEL_PROC_SAMPLE_RATE:
      if (requestValueInt < 0)
        return DRCDEC_SELECTION_PROCESS_PARAM_OUT_OF_RANGE;
      diff |= _compAssign(&pSelProcInput->audioSampleRate, requestValueInt);
      break;
    case SEL_PROC_BOOST:
      if ((requestValue < (int32_t)0) ||
          (requestValue > FL2FXCONST_DBL(1.0f / (float)(1 << 1))))
        return DRCDEC_SELECTION_PROCESS_PARAM_OUT_OF_RANGE;
      diff |= _compAssign(
          &pSelProcInput->boost,
          FX_DBL2FX_SGL(
              requestValue +
              (int32_t)(1 << 15))); /* convert to int16_t with rounding */
      break;
    case SEL_PROC_COMPRESS:
      if ((requestValue < (int32_t)0) ||
          (requestValue > FL2FXCONST_DBL(1.0f / (float)(1 << 1))))
        return DRCDEC_SELECTION_PROCESS_PARAM_OUT_OF_RANGE;
      diff |= _compAssign(
          &pSelProcInput->compress,
          FX_DBL2FX_SGL(
              requestValue +
              (int32_t)(1 << 15))); /* convert to int16_t with rounding */
      break;
    default:
      return DRCDEC_SELECTION_PROCESS_INVALID_PARAM;
  }

  if (pDiff != NULL) {
    *pDiff |= diff;
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

int32_t
drcDec_SelectionProcess_GetParam(HANDLE_DRC_SELECTION_PROCESS hInstance,
                                 const SEL_PROC_USER_PARAM requestType) {
  SEL_PROC_INPUT* pSelProcInput = &(hInstance->selProcInput);

  switch (requestType) {
    case SEL_PROC_LOUDNESS_NORMALIZATION_ON:
      return (int32_t)pSelProcInput->loudnessNormalizationOn;
    case SEL_PROC_DYNAMIC_RANGE_CONTROL_ON:
      return (int32_t)pSelProcInput->dynamicRangeControlOn;
    default:
      return (int32_t)0;
  }
}

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_Delete(HANDLE_DRC_SELECTION_PROCESS* phInstance) {
  if (phInstance == NULL || *phInstance == NULL)
    return DRCDEC_SELECTION_PROCESS_INVALID_HANDLE;

  FDKfree(*phInstance);
  *phInstance = NULL;
  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_Process(HANDLE_DRC_SELECTION_PROCESS hInstance,
                                HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
                                HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
                                HANDLE_SEL_PROC_OUTPUT hSelProcOutput) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;
  DRCDEC_SELECTION* pCandidatesSelected;
  DRCDEC_SELECTION* pCandidatesPotential;

  if (hInstance == NULL) return DRCDEC_SELECTION_PROCESS_INVALID_HANDLE;

  pCandidatesSelected = &(hInstance->selectionData[0]);
  pCandidatesPotential = &(hInstance->selectionData[1]);
  _drcdec_selection_setNumber(pCandidatesSelected, 0);
  _drcdec_selection_setNumber(pCandidatesPotential, 0);

  retVal = _generateVirtualDrcSets(&(hInstance->selProcInput), hUniDrcConfig,
                                   hInstance->codecMode);
  if (retVal) return (retVal);

  if (hInstance->selProcInput.baseChannelCount !=
      hUniDrcConfig->channelLayout.baseChannelCount) {
    hInstance->selProcInput.baseChannelCount =
        hUniDrcConfig->channelLayout.baseChannelCount;
  }

  if ((hInstance->selProcInput.targetConfigRequestType != 0) ||
      (hInstance->selProcInput.targetConfigRequestType == 0 &&
       hInstance->selProcInput.numDownmixIdRequests == 0)) {
    retVal = _channelLayoutToDownmixIdMapping(&(hInstance->selProcInput),
                                              hUniDrcConfig);

    if (_isError(retVal)) return (retVal);
  }

  retVal = _drcSetPreSelection(&(hInstance->selProcInput), hUniDrcConfig,
                               hLoudnessInfoSet, &pCandidatesPotential,
                               &pCandidatesSelected, hInstance->codecMode);
  if (retVal) return (retVal);

  if (hInstance->selProcInput.albumMode) {
    _swapSelectionAndClear(&pCandidatesPotential, &pCandidatesSelected);

    retVal = _selectAlbumLoudness(hLoudnessInfoSet, pCandidatesPotential,
                                  pCandidatesSelected);
    if (retVal) return (retVal);

    if (_drcdec_selection_getNumber(pCandidatesSelected) == 0) {
      _swapSelection(&pCandidatesPotential, &pCandidatesSelected);
    }
  }

  _swapSelectionAndClear(&pCandidatesPotential, &pCandidatesSelected);

  retVal = _drcSetRequestSelection(&(hInstance->selProcInput), hUniDrcConfig,
                                   hLoudnessInfoSet, &pCandidatesPotential,
                                   &pCandidatesSelected);
  if (retVal) return (retVal);

  retVal = _drcSetFinalSelection(&(hInstance->selProcInput), hUniDrcConfig,
                                 &pCandidatesPotential, &pCandidatesSelected,
                                 hInstance->codecMode);
  if (retVal) return (retVal);

  retVal = _generateOutputInfo(
      &(hInstance->selProcInput), hSelProcOutput, hUniDrcConfig,
      hLoudnessInfoSet, &(pCandidatesSelected->data[0]), hInstance->codecMode);

  if (_isError(retVal)) return (retVal);

  retVal = _selectDownmixMatrix(hSelProcOutput, hUniDrcConfig);
  if (retVal) return (retVal);

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

/*******************************************/
/* static functions                        */
/*******************************************/

static DRCDEC_SELECTION_PROCESS_RETURN _initDefaultParams(
    HANDLE_SEL_PROC_INPUT hSelProcInput) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;

  if (hSelProcInput == NULL) return DRCDEC_SELECTION_PROCESS_INVALID_HANDLE;

  /* system parameters */
  hSelProcInput->baseChannelCount = -1;
  hSelProcInput->baseLayout = -1;
  hSelProcInput->targetConfigRequestType = TCRT_DOWNMIX_ID;
  hSelProcInput->numDownmixIdRequests = 0;

  /* loudness normalization parameters */
  hSelProcInput->albumMode = 0;
  hSelProcInput->peakLimiterPresent = 0;
  hSelProcInput->loudnessNormalizationOn = 1;
  hSelProcInput->targetLoudness = FL2FXCONST_DBL(-24.0f / (float)(1 << 7));
  hSelProcInput->loudnessDeviationMax = DEFAULT_LOUDNESS_DEVIATION_MAX;
  hSelProcInput->loudnessMeasurementMethod = MDR_ANCHOR_LOUDNESS;
  hSelProcInput->loudnessMeasurementSystem = MSR_EXPERT_PANEL;
  hSelProcInput->loudnessMeasurementPreProc = LPR_DEFAULT;
  hSelProcInput->deviceCutOffFrequency = 500;
  hSelProcInput->loudnessNormalizationGainDbMax =
      (int32_t)MAXVAL_DBL; /* infinity as default */
  hSelProcInput->loudnessNormalizationGainModificationDb = (int32_t)0;
  hSelProcInput->outputPeakLevelMax = (int32_t)0;
  if (hSelProcInput->peakLimiterPresent == 1) {
    hSelProcInput->outputPeakLevelMax = FL2FXCONST_DBL(6.0f / (float)(1 << 7));
  }

  /* dynamic range control parameters */
  hSelProcInput->dynamicRangeControlOn = 1;

  hSelProcInput->numDrcFeatureRequests = 0;

  /* other parameters */
  hSelProcInput->boost = FL2FXCONST_SGL(1.f / (float)(1 << 1));
  hSelProcInput->compress = FL2FXCONST_SGL(1.f / (float)(1 << 1));
  hSelProcInput->drcCharacteristicTarget = 0;

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _initCodecModeParams(
    HANDLE_SEL_PROC_INPUT hSelProcInput, const SEL_PROC_CODEC_MODE codecMode) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;

  if (hSelProcInput == NULL) return DRCDEC_SELECTION_PROCESS_INVALID_HANDLE;

  switch (codecMode) {
    case SEL_PROC_MPEG_H_3DA:
      hSelProcInput->loudnessDeviationMax = 0;
      hSelProcInput->peakLimiterPresent = 1; /* peak limiter is mandatory */
      /* The peak limiter also has to catch overshoots due to user
      interactivity, downmixing etc. Therefore the maximum output peak level is
      reduced to 0 dB. */
      hSelProcInput->outputPeakLevelMax = (int32_t)0;
      break;
    case SEL_PROC_MPEG_4_AAC:
    case SEL_PROC_MPEG_D_USAC:
      hSelProcInput->loudnessDeviationMax = DEFAULT_LOUDNESS_DEVIATION_MAX;
      hSelProcInput->peakLimiterPresent = 1;
      /* A peak limiter is present at the end of the decoder, therefore we can
       * allow for a maximum output peak level greater than full scale
       */
      hSelProcInput->outputPeakLevelMax =
          FL2FXCONST_DBL(6.0f / (float)(1 << 7));
      break;
    case SEL_PROC_TEST_TIME_DOMAIN:
    case SEL_PROC_TEST_QMF_DOMAIN:
    case SEL_PROC_TEST_STFT_DOMAIN:
      /* for testing, adapt to default settings in reference software */
      hSelProcInput->loudnessNormalizationOn = 0;
      hSelProcInput->dynamicRangeControlOn = 0;
      break;
    case SEL_PROC_CODEC_MODE_UNDEFINED:
    default:
      hSelProcInput->loudnessDeviationMax = DEFAULT_LOUDNESS_DEVIATION_MAX;
      hSelProcInput->peakLimiterPresent = 0;
  }

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _channelLayoutToDownmixIdMapping(
    HANDLE_SEL_PROC_INPUT hSelProcInput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;

  DOWNMIX_INSTRUCTIONS* pDown = NULL;

  int32_t i;

  hSelProcInput->numDownmixIdRequests = 0;

  switch (hSelProcInput->targetConfigRequestType) {
    case TCRT_DOWNMIX_ID:
      if (hSelProcInput->numDownmixIdRequests == 0) {
        hSelProcInput->downmixIdRequested[0] = 0;
        hSelProcInput->numDownmixIdRequests = 1;
      }

      break;

    case TCRT_TARGET_LAYOUT:
      if (hSelProcInput->targetLayoutRequested == hSelProcInput->baseLayout) {
        hSelProcInput->downmixIdRequested[0] = 0;
        hSelProcInput->numDownmixIdRequests = 1;
      }

      if (hSelProcInput->numDownmixIdRequests == 0) {
        for (i = 0; i < hUniDrcConfig->downmixInstructionsCount; i++) {
          pDown = &(hUniDrcConfig->downmixInstructions[i]);

          if (hSelProcInput->targetLayoutRequested == pDown->targetLayout) {
            hSelProcInput
                ->downmixIdRequested[hSelProcInput->numDownmixIdRequests] =
                pDown->downmixId;
            hSelProcInput->numDownmixIdRequests++;
          }
        }
      }

      if (hSelProcInput->baseLayout == -1) {
        retVal = DRCDEC_SELECTION_PROCESS_WARNING;
      }

      if (hSelProcInput->numDownmixIdRequests == 0) {
        hSelProcInput->downmixIdRequested[0] = 0;
        hSelProcInput->numDownmixIdRequests = 1;
        retVal = DRCDEC_SELECTION_PROCESS_WARNING;
      }

      break;

    case TCRT_TARGET_CHANNEL_COUNT:
      if (hSelProcInput->targetChannelCountRequested ==
          hSelProcInput->baseChannelCount) {
        hSelProcInput->downmixIdRequested[0] = 0;
        hSelProcInput->numDownmixIdRequests = 1;
      }

      if (hSelProcInput->numDownmixIdRequests == 0) {
        for (i = 0; i < hUniDrcConfig->downmixInstructionsCount; i++) {
          pDown = &(hUniDrcConfig->downmixInstructions[i]);

          if (hSelProcInput->targetChannelCountRequested ==
              pDown->targetChannelCount) {
            hSelProcInput
                ->downmixIdRequested[hSelProcInput->numDownmixIdRequests] =
                pDown->downmixId;
            hSelProcInput->numDownmixIdRequests++;
          }
        }
      }

      if (hSelProcInput->baseChannelCount == -1) {
        retVal = DRCDEC_SELECTION_PROCESS_WARNING;
      }

      if (hSelProcInput->numDownmixIdRequests == 0) {
        retVal = DRCDEC_SELECTION_PROCESS_WARNING;
        hSelProcInput->downmixIdRequested[0] = 0;
        hSelProcInput->numDownmixIdRequests = 1;
      }

      break;

    default:
      return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  return retVal;
}

/*******************************************/

/* Note: Numbering of DRC pre-selection steps according to MPEG-D Part-4 DRC
 * Amd1 */

/* #1: DownmixId of DRC set matches the requested downmixId.
   #2: Output channel layout of DRC set matches the requested layout.
   #3: Channel count of DRC set matches the requested channel count. */
static DRCDEC_SELECTION_PROCESS_RETURN _preSelectionRequirement123(
    int32_t nRequestedDownmixId, DRC_INSTRUCTIONS_UNI_DRC* pDrcInstructionUniDrc,
    int32_t* pMatchFound) {
  int32_t i;
  *pMatchFound = 0;

  for (i = 0; i < pDrcInstructionUniDrc->downmixIdCount; i++) {
    if ((pDrcInstructionUniDrc->downmixId[i] == nRequestedDownmixId) ||
        (pDrcInstructionUniDrc->downmixId[i] == DOWNMIX_ID_ANY_DOWNMIX) ||
        ((pDrcInstructionUniDrc->downmixId[i] == DOWNMIX_ID_BASE_LAYOUT) &&
         (pDrcInstructionUniDrc->drcSetId > 0))) {
      *pMatchFound = 1;
      break;
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

/* #4: The DRC set is not a "Fade-" or "Ducking-" only DRC set. */
static DRCDEC_SELECTION_PROCESS_RETURN _preSelectionRequirement4(
    DRC_INSTRUCTIONS_UNI_DRC* pDrcInstruction, int32_t nDynamicRangeControlOn,
    int32_t* pMatchFound) {
  *pMatchFound = 0;

  if (nDynamicRangeControlOn == 1) {
    if ((pDrcInstruction->drcSetEffect != EB_FADE) &&
        (pDrcInstruction->drcSetEffect != EB_DUCK_OTHER) &&
        (pDrcInstruction->drcSetEffect != EB_DUCK_SELF) &&
        (pDrcInstruction->drcSetEffect != 0 || pDrcInstruction->drcSetId < 0)) {
      *pMatchFound = 1;
    }
  } else {
    *pMatchFound = 1;
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

/* #5: The number of DRC bands is supported. Moreover, gainSetIndex and
 * gainSequenceIndex are within the allowed range. */
static DRCDEC_SELECTION_PROCESS_RETURN _preSelectionRequirement5(
    DRC_INSTRUCTIONS_UNI_DRC* pDrcInstructionUniDrc,
    DRC_COEFFICIENTS_UNI_DRC* pCoef, int32_t* pMatchFound) {
  int32_t b, i;

  *pMatchFound = 1;

  if (pDrcInstructionUniDrc->drcSetId < 0) /* virtual DRC sets are okay */
  {
    return DRCDEC_SELECTION_PROCESS_NO_ERROR;
  }

  if (pCoef == NULL) /* check for parametricDRC */
  {
    *pMatchFound = 0; /* parametricDRC not supported */
    return DRCDEC_SELECTION_PROCESS_NO_ERROR;
  }

  if (pCoef->drcLocation !=
      pDrcInstructionUniDrc
          ->drcLocation) /* drcLocation must be LOCATION_SELECTED */
  {
    *pMatchFound = 0;
    return DRCDEC_SELECTION_PROCESS_NO_ERROR;
  }

  for (i = 0; i < pDrcInstructionUniDrc->nDrcChannelGroups; i++) {
    int32_t indexDrcCoeff = pDrcInstructionUniDrc->gainSetIndexForChannelGroup[i];
    int32_t bandCount = 0;

    if (indexDrcCoeff >= 12) {
      *pMatchFound = 0;
      return DRCDEC_SELECTION_PROCESS_NO_ERROR;
    }

    if (indexDrcCoeff > pCoef->gainSetCount - 1) /* check for parametricDRC */
    {
      continue;
    }

    GAIN_SET* gainSet = &(pCoef->gainSet[indexDrcCoeff]);
    bandCount = gainSet->bandCount;

    if (bandCount > 4) {
      *pMatchFound = 0;
    }

    for (b = 0; b < bandCount; b++) {
      if ((gainSet->gainSequenceIndex[b] >= 12) ||
          (gainSet->gainSequenceIndex[b] >= pCoef->gainSequenceCount)) {
        *pMatchFound = 0;
        return DRCDEC_SELECTION_PROCESS_NO_ERROR;
      }
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

/* #6: Independent use of DRC set is permitted.*/
static DRCDEC_SELECTION_PROCESS_RETURN _preSelectionRequirement6(
    DRC_INSTRUCTIONS_UNI_DRC* pDrcInstructionUniDrc, int32_t* pMatchFound) {
  *pMatchFound = 0;

  if (((pDrcInstructionUniDrc->dependsOnDrcSetPresent == 0) &&
       (pDrcInstructionUniDrc->noIndependentUse == 0)) ||
      (pDrcInstructionUniDrc->dependsOnDrcSetPresent == 1)) {
    *pMatchFound = 1;
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

/* #7: DRC sets that require EQ are only permitted if EQ is supported. */
static DRCDEC_SELECTION_PROCESS_RETURN _preSelectionRequirement7(
    DRC_INSTRUCTIONS_UNI_DRC* pDrcInstructionUniDrc, int32_t* pMatchFound) {
  *pMatchFound = 1;

  if (pDrcInstructionUniDrc->requiresEq) {
    /* EQ is not supported */
    *pMatchFound = 0;
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static void _setSelectionDataInfo(
    DRCDEC_SELECTION_DATA* pData, int32_t loudness, /* e = 7 */
    int32_t loudnessNormalizationGainDb,            /* e = 7 */
    int32_t loudnessNormalizationGainDbMax,         /* e = 7 */
    int32_t loudnessDeviationMax,                   /* e = 7 */
    int32_t signalPeakLevel,                        /* e = 7 */
    int32_t outputPeakLevelMax,                     /* e = 7 */
    int32_t applyAdjustment) {
  int32_t adjustment = 0; /* e = 8 */

  /* use e = 8 for all function parameters to prevent overflow */
  loudness >>= 1;
  loudnessNormalizationGainDb >>= 1;
  loudnessNormalizationGainDbMax >>= 1;
  loudnessDeviationMax >>= 1;
  signalPeakLevel >>= 1;
  outputPeakLevelMax >>= 1;

  if (applyAdjustment) {
    adjustment =
        fMax((int32_t)0, signalPeakLevel + loudnessNormalizationGainDb -
                              outputPeakLevelMax);
    adjustment = fMin(adjustment, fMax((int32_t)0, loudnessDeviationMax));
  }

  pData->loudnessNormalizationGainDbAdjusted = fMin(
      loudnessNormalizationGainDb - adjustment, loudnessNormalizationGainDbMax);
  pData->outputLoudness = loudness + pData->loudnessNormalizationGainDbAdjusted;
  pData->outputPeakLevel =
      signalPeakLevel + pData->loudnessNormalizationGainDbAdjusted;

  /* shift back to e = 7 using saturation */
  pData->loudnessNormalizationGainDbAdjusted = SATURATE_LEFT_SHIFT(
      pData->loudnessNormalizationGainDbAdjusted, 1, DFRACT_BITS);
  pData->outputLoudness =
      SATURATE_LEFT_SHIFT(pData->outputLoudness, 1, DFRACT_BITS);
  pData->outputPeakLevel =
      SATURATE_LEFT_SHIFT(pData->outputPeakLevel, 1, DFRACT_BITS);
}

static int32_t _targetLoudnessInRange(
    DRC_INSTRUCTIONS_UNI_DRC* pDrcInstructionUniDrc, int32_t targetLoudness) {
  int32_t retVal = 0;

  int32_t drcSetTargetLoudnessValueUpper =
      ((int32_t)pDrcInstructionUniDrc->drcSetTargetLoudnessValueUpper)
      << (DFRACT_BITS - 1 - 7);
  int32_t drcSetTargetLoudnessValueLower =
      ((int32_t)pDrcInstructionUniDrc->drcSetTargetLoudnessValueLower)
      << (DFRACT_BITS - 1 - 7);

  if (pDrcInstructionUniDrc->drcSetTargetLoudnessPresent &&
      drcSetTargetLoudnessValueUpper >= targetLoudness &&
      drcSetTargetLoudnessValueLower < targetLoudness) {
    retVal = 1;
  }

  return retVal;
}

static int32_t _drcSetIsUsable(HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
                           DRC_INSTRUCTIONS_UNI_DRC* pInst) {
  int32_t usable = 0;
  DRC_COEFFICIENTS_UNI_DRC* pCoef =
      selectDrcCoefficients(hUniDrcConfig, LOCATION_SELECTED);

  /* check if ID is unique */
  if (selectDrcInstructions(hUniDrcConfig, pInst->drcSetId) != pInst) return 0;
  /* sanity check on drcInstructions */
  _preSelectionRequirement5(pInst, pCoef, &usable);
  return usable;
}

/* #8: The range of the target loudness specified for a DRC set has to include
 * the requested decoder target loudness. */
static DRCDEC_SELECTION_PROCESS_RETURN _preSelectionRequirement8(
    SEL_PROC_INPUT* hSelProcInput, int32_t downmixIdIndex,
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
    DRC_INSTRUCTIONS_UNI_DRC* pDrcInstructionUniDrc,
    DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected, SEL_PROC_CODEC_MODE codecMode) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;
  int32_t explicitPeakInformationPresent;
  int32_t signalPeakLevel;
  int32_t addToCandidate = 0;

  int32_t loudnessNormalizationGainDb;
  int32_t loudness;

  int32_t loudnessDeviationMax =
      ((int32_t)hSelProcInput->loudnessDeviationMax) << (DFRACT_BITS - 1 - 7);

  {
    retVal = _getLoudness(hLoudnessInfoSet, hSelProcInput->albumMode,
                          hSelProcInput->loudnessMeasurementMethod,
                          hSelProcInput->loudnessMeasurementSystem,
                          hSelProcInput->targetLoudness,
                          pDrcInstructionUniDrc->drcSetId,
                          hSelProcInput->downmixIdRequested[downmixIdIndex],
                          &loudnessNormalizationGainDb, &loudness);
    if (retVal) return (retVal);
  }

  if (!hSelProcInput->loudnessNormalizationOn) {
    loudnessNormalizationGainDb = (int32_t)0;
  }

  retVal = _getSignalPeakLevel(
      hSelProcInput, hUniDrcConfig, hLoudnessInfoSet, pDrcInstructionUniDrc,
      hSelProcInput->downmixIdRequested[downmixIdIndex],
      &explicitPeakInformationPresent, &signalPeakLevel, codecMode

  );
  if (retVal) return (retVal);

  if (hSelProcInput->dynamicRangeControlOn) {
    if (explicitPeakInformationPresent == 0) {
      if (pDrcInstructionUniDrc->drcSetTargetLoudnessPresent &&
          ((hSelProcInput->loudnessNormalizationOn &&
            _targetLoudnessInRange(pDrcInstructionUniDrc,
                                   hSelProcInput->targetLoudness)) ||
           !hSelProcInput->loudnessNormalizationOn)) {
        DRCDEC_SELECTION_DATA* pData =
            _drcdec_selection_addNew(pCandidatesSelected);
        if (pData == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

        _setSelectionDataInfo(pData, loudness, loudnessNormalizationGainDb,
                              hSelProcInput->loudnessNormalizationGainDbMax,
                              loudnessDeviationMax, signalPeakLevel,
                              hSelProcInput->outputPeakLevelMax, 0);
        pData->downmixIdRequestIndex = downmixIdIndex;
        pData->pInst = pDrcInstructionUniDrc;
        pData->selectionFlag =
            1; /* signal pre-selection step dealing with drcSetTargetLoudness */

        if (hSelProcInput->loudnessNormalizationOn) {
          pData->outputPeakLevel =
              hSelProcInput->targetLoudness -
              (((int32_t)pData->pInst->drcSetTargetLoudnessValueUpper)
               << (DFRACT_BITS - 1 - 7));
        } else {
          pData->outputPeakLevel = (int32_t)0;
        }
      } else {
        if ((!hSelProcInput->loudnessNormalizationOn) ||
            (!pDrcInstructionUniDrc->drcSetTargetLoudnessPresent) ||
            (hSelProcInput->loudnessNormalizationOn &&
             _targetLoudnessInRange(pDrcInstructionUniDrc,
                                    hSelProcInput->targetLoudness))) {
          addToCandidate = 1;
        }
      }
    } else {
      addToCandidate = 1;
    }

    if (addToCandidate) {
      DRCDEC_SELECTION_DATA* pData =
          _drcdec_selection_addNew(pCandidatesPotential);
      if (pData == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

      _setSelectionDataInfo(pData, loudness, loudnessNormalizationGainDb,
                            hSelProcInput->loudnessNormalizationGainDbMax,
                            loudnessDeviationMax, signalPeakLevel,
                            hSelProcInput->outputPeakLevelMax, 0);
      pData->downmixIdRequestIndex = downmixIdIndex;
      pData->pInst = pDrcInstructionUniDrc;
      pData->selectionFlag = 0;
    }
  } else {
    if (pDrcInstructionUniDrc->drcSetId < 0) {
      DRCDEC_SELECTION_DATA* pData =
          _drcdec_selection_addNew(pCandidatesSelected);
      if (pData == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

      _setSelectionDataInfo(pData, loudness, loudnessNormalizationGainDb,
                            hSelProcInput->loudnessNormalizationGainDbMax,
                            loudnessDeviationMax, signalPeakLevel,
                            hSelProcInput->outputPeakLevelMax, 1);

      pData->downmixIdRequestIndex = downmixIdIndex;
      pData->pInst = pDrcInstructionUniDrc;
      pData->selectionFlag = 0;
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

/* #9: Clipping is minimized. */
static DRCDEC_SELECTION_PROCESS_RETURN _preSelectionRequirement9(
    SEL_PROC_INPUT* hSelProcInput, DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected) {
  int32_t i;

  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    DRCDEC_SELECTION_DATA* pCandidate =
        _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    if (pCandidate->outputPeakLevel <= hSelProcInput->outputPeakLevelMax) {
      if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
        return DRCDEC_SELECTION_PROCESS_NOT_OK;
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetPreSelectionSingleInstruction(
    SEL_PROC_INPUT* hSelProcInput, int32_t downmixIdIndex,
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
    DRC_INSTRUCTIONS_UNI_DRC* pDrcInstructionUniDrc,
    DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected, SEL_PROC_CODEC_MODE codecMode) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;
  int32_t matchFound = 0;
  DRC_COEFFICIENTS_UNI_DRC* pCoef =
      selectDrcCoefficients(hUniDrcConfig, LOCATION_SELECTED);

  retVal = _preSelectionRequirement123(
      hSelProcInput->downmixIdRequested[downmixIdIndex], pDrcInstructionUniDrc,
      &matchFound);

  if (!retVal && matchFound)
    retVal = _preSelectionRequirement4(pDrcInstructionUniDrc,
                                       hSelProcInput->dynamicRangeControlOn,
                                       &matchFound);

  if (!retVal && matchFound)
    retVal =
        _preSelectionRequirement5(pDrcInstructionUniDrc, pCoef, &matchFound);

  if (!retVal && matchFound)
    retVal = _preSelectionRequirement6(pDrcInstructionUniDrc, &matchFound);

  if (!retVal && matchFound)
    retVal = _preSelectionRequirement7(pDrcInstructionUniDrc, &matchFound);

  if (!retVal && matchFound)
    retVal = _preSelectionRequirement8(
        hSelProcInput, downmixIdIndex, hUniDrcConfig, hLoudnessInfoSet,
        pDrcInstructionUniDrc, pCandidatesPotential, pCandidatesSelected,
        codecMode);

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetSelectionAddCandidates(
    SEL_PROC_INPUT* hSelProcInput, DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;
  int32_t nHitCount = 0;
  int32_t i;

  DRCDEC_SELECTION_DATA* pCandidate = NULL;
  DRC_INSTRUCTIONS_UNI_DRC* pDrcInstructionUniDrc = NULL;

  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    pDrcInstructionUniDrc = pCandidate->pInst;

    if (_targetLoudnessInRange(pDrcInstructionUniDrc,
                               hSelProcInput->targetLoudness)) {
      nHitCount++;
    }
  }

  if (nHitCount != 0) {
    for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
      pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
      if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

      pDrcInstructionUniDrc = pCandidate->pInst;

      if (_targetLoudnessInRange(pDrcInstructionUniDrc,
                                 hSelProcInput->targetLoudness)) {
        if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
          return DRCDEC_SELECTION_PROCESS_NOT_OK;
      }
    }
  } else {
    int32_t lowestPeakLevel = MAXVAL_DBL; /* e = 7 */
    int32_t peakLevel = 0;                /* e = 7 */

    for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
      pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
      if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

      peakLevel = pCandidate->outputPeakLevel;

      if (peakLevel < lowestPeakLevel) {
        lowestPeakLevel = peakLevel;
      }
    }

    /* add all with lowest peak level or max 1dB above */
    for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
      int32_t loudnessDeviationMax =
          ((int32_t)hSelProcInput->loudnessDeviationMax)
          << (DFRACT_BITS - 1 - 7); /* e = 7 */

      pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
      if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

      peakLevel = pCandidate->outputPeakLevel;

      if (peakLevel == lowestPeakLevel ||
          peakLevel <=
              lowestPeakLevel + FL2FXCONST_DBL(1.0f / (float)(1 << 7))) {
        int32_t adjustment =
            fMax((int32_t)0, peakLevel - hSelProcInput->outputPeakLevelMax);
        adjustment = fMin(adjustment, fMax((int32_t)0, loudnessDeviationMax));

        pCandidate->loudnessNormalizationGainDbAdjusted -= adjustment;
        pCandidate->outputPeakLevel -= adjustment;
        pCandidate->outputLoudness -= adjustment;
        if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
          return DRCDEC_SELECTION_PROCESS_NOT_OK;
      }
    }
  }

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _dependentDrcInstruction(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig, DRC_INSTRUCTIONS_UNI_DRC* pInst,
    DRC_INSTRUCTIONS_UNI_DRC** ppDrcInstructionsDependent) {
  int32_t i;
  DRC_INSTRUCTIONS_UNI_DRC* pDependentDrc = NULL;

  for (i = 0; i < hUniDrcConfig->drcInstructionsUniDrcCount; i++) {
    pDependentDrc =
        (DRC_INSTRUCTIONS_UNI_DRC*)&(hUniDrcConfig->drcInstructionsUniDrc[i]);

    if (pDependentDrc->drcSetId == pInst->dependsOnDrcSet) {
      break;
    }
  }

  if (i == hUniDrcConfig->drcInstructionsUniDrcCount) {
    return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  if (pDependentDrc->dependsOnDrcSetPresent == 1) {
    return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  *ppDrcInstructionsDependent = pDependentDrc;

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static DRCDEC_SELECTION_PROCESS_RETURN _selectDrcSetEffectNone(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig, DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected) {
  int32_t i;

  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    DRCDEC_SELECTION_DATA* pCandidate =
        _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    if ((pCandidate->pInst->drcSetEffect & 0xff) == 0) {
      if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
        return DRCDEC_SELECTION_PROCESS_NOT_OK;
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static DRCDEC_SELECTION_PROCESS_RETURN _selectSingleEffectType(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig, DRC_EFFECT_TYPE_REQUEST effectType,
    DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected) {
  int32_t i;
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;
  DRC_INSTRUCTIONS_UNI_DRC* pInst;
  DRC_INSTRUCTIONS_UNI_DRC* pDrcInstructionsDependent;

  if (effectType == DETR_NONE) {
    retVal = _selectDrcSetEffectNone(hUniDrcConfig, pCandidatesPotential,
                                     pCandidatesSelected);
    if (retVal) return (retVal);
  } else {
    int32_t effectBitPosition = 1 << (effectType - 1);

    for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
      DRCDEC_SELECTION_DATA* pCandidate =
          _drcdec_selection_getAt(pCandidatesPotential, i);
      if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

      pInst = pCandidate->pInst;

      if (!pInst->dependsOnDrcSetPresent) {
        if ((pInst->drcSetEffect & effectBitPosition)) {
          if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
            return DRCDEC_SELECTION_PROCESS_NOT_OK;
        }
      } else {
        retVal = _dependentDrcInstruction(hUniDrcConfig, pInst,
                                          &pDrcInstructionsDependent);
        if (retVal) return (retVal);

        if (((pInst->drcSetEffect & effectBitPosition)) ||
            ((pDrcInstructionsDependent->drcSetEffect & effectBitPosition))) {
          if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
            return DRCDEC_SELECTION_PROCESS_NOT_OK;
        }
      }
    }
  }

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _selectEffectTypeFeature(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig, DRC_FEATURE_REQUEST drcFeatureRequest,
    DRCDEC_SELECTION** ppCandidatesPotential,
    DRCDEC_SELECTION** ppCandidatesSelected) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;
  int32_t i;
  int32_t desiredEffectTypeFound = 0;

  for (i = 0; i < drcFeatureRequest.drcEffectType.numRequestsDesired; i++) {
    retVal = _selectSingleEffectType(
        hUniDrcConfig, drcFeatureRequest.drcEffectType.request[i],
        *ppCandidatesPotential, *ppCandidatesSelected);
    if (retVal) return (retVal);

    if (_drcdec_selection_getNumber(*ppCandidatesSelected)) {
      desiredEffectTypeFound = 1;
      _swapSelectionAndClear(ppCandidatesPotential, ppCandidatesSelected);
    }
  }

  if (!desiredEffectTypeFound) {
    for (i = drcFeatureRequest.drcEffectType.numRequestsDesired;
         i < drcFeatureRequest.drcEffectType.numRequests; i++) {
      retVal = _selectSingleEffectType(
          hUniDrcConfig, drcFeatureRequest.drcEffectType.request[i],
          *ppCandidatesPotential, *ppCandidatesSelected);
      if (retVal) return (retVal);

      if (_drcdec_selection_getNumber(*ppCandidatesSelected)) {
        _swapSelectionAndClear(ppCandidatesPotential, ppCandidatesSelected);
        break;
      }
    }
  }

  _swapSelection(ppCandidatesPotential, ppCandidatesSelected);

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _selectDynamicRange(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
    DRC_FEATURE_REQUEST drcFeatureRequest, uint8_t* pDownmixIdRequested,
    int32_t albumMode, DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* ppCandidatesSelected) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;
  int32_t i;
  int32_t peakToAveragePresent;
  int32_t peakToAverage;

  int32_t minVal = MAXVAL_DBL;
  int32_t val = 0;

  int32_t numSelectedCandidates = _drcdec_selection_getNumber(ppCandidatesSelected);

  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    DRCDEC_SELECTION_DATA* pCandidate =
        _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    retVal = _dynamicRangeMeasurement(
        hLoudnessInfoSet, pCandidate->pInst,
        pDownmixIdRequested[pCandidate->downmixIdRequestIndex],
        drcFeatureRequest.dynamicRange.measurementRequestType, albumMode,
        &peakToAveragePresent, &peakToAverage);
    if (retVal) return (retVal);

    if (peakToAveragePresent) {
      if (!drcFeatureRequest.dynamicRange.requestedIsRange) {
        val = fAbs(drcFeatureRequest.dynamicRange.requestValue - peakToAverage);

        if (minVal > val) {
          minVal = val;

          _drcdec_selection_setNumber(ppCandidatesSelected,
                                      numSelectedCandidates);
        }
        if (_drcdec_selection_add(ppCandidatesSelected, pCandidate) == NULL)
          return DRCDEC_SELECTION_PROCESS_NOT_OK;
      } else {
        if ((peakToAverage >= drcFeatureRequest.dynamicRange.requestValueMin) &&
            (peakToAverage <= drcFeatureRequest.dynamicRange.requestValueMax)) {
          if (_drcdec_selection_add(ppCandidatesSelected, pCandidate) == NULL)
            return DRCDEC_SELECTION_PROCESS_NOT_OK;
        }
      }
    }
  }

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _selectSingleDrcCharacteristic(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig, int32_t requestedDrcCharacteristic,
    DRCDEC_SELECTION** ppCandidatesPotential,
    DRCDEC_SELECTION** ppCandidatesSelected) {
  int32_t i, j, b;
  int32_t hit = 0;

  DRC_INSTRUCTIONS_UNI_DRC* pInst = NULL;
  DRC_COEFFICIENTS_UNI_DRC* pCoef = NULL;
  GAIN_SET* pGainSet = NULL;

  if (requestedDrcCharacteristic < 1) {
    return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  pCoef = selectDrcCoefficients(hUniDrcConfig, LOCATION_SELECTED);

  if (pCoef == NULL) /* check for parametricDRC */
  {
    return DRCDEC_SELECTION_PROCESS_NO_ERROR;
  }

  for (i = 0; i < _drcdec_selection_getNumber(*ppCandidatesPotential); i++) {
    DRCDEC_SELECTION_DATA* pCandidate =
        _drcdec_selection_getAt(*ppCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    pInst = pCandidate->pInst;

    hit = 0;

    for (j = 0; j < pInst->nDrcChannelGroups; j++) {
      int32_t bandCount = 0;
      int32_t indexDrcCoeff = pInst->gainSetIndexForChannelGroup[j];

      if (indexDrcCoeff > pCoef->gainSetCount - 1) /* check for parametricDRC */
      {
        return DRCDEC_SELECTION_PROCESS_NO_ERROR;
      }

      pGainSet = &(pCoef->gainSet[indexDrcCoeff]);
      bandCount = pGainSet->bandCount;

      for (b = 0; b < bandCount; b++) {
        if ((pGainSet->drcCharacteristic[b].isCICP) &&
            (pGainSet->drcCharacteristic[b].cicpIndex ==
             requestedDrcCharacteristic)) {
          hit = 1;
          break;
        }
      }

      if (hit) break;
    }

    if (hit) {
      if (_drcdec_selection_add(*ppCandidatesSelected, pCandidate) == NULL)
        return DRCDEC_SELECTION_PROCESS_NOT_OK;
    }
  }

  if (_drcdec_selection_getNumber(*ppCandidatesSelected)) {
    _swapSelectionAndClear(ppCandidatesPotential, ppCandidatesSelected);
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static DRCDEC_SELECTION_PROCESS_RETURN _selectDrcCharacteristic(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig, int32_t drcCharacteristicRequested,
    DRCDEC_SELECTION** ppCandidatesPotential,
    DRCDEC_SELECTION** ppCandidatesSelected) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;

  const int32_t secondTry[12] = {0, 2, 3, 4, 5, 6, 5, 9, 10, 7, 8, 10};

  retVal = _selectSingleDrcCharacteristic(
      hUniDrcConfig, drcCharacteristicRequested, ppCandidatesPotential,
      ppCandidatesSelected);
  if (retVal) return (retVal);

  if ((drcCharacteristicRequested <= 11) &&
      (_drcdec_selection_getNumber(*ppCandidatesSelected) == 0)) {
    retVal = _selectSingleDrcCharacteristic(
        hUniDrcConfig, secondTry[drcCharacteristicRequested],
        ppCandidatesPotential, ppCandidatesSelected);
    if (retVal) return (retVal);
  }

  if (_drcdec_selection_getNumber(*ppCandidatesSelected) == 0) {
    if ((drcCharacteristicRequested >= 2) &&
        (drcCharacteristicRequested <= 5)) {
      retVal = _selectSingleDrcCharacteristic(
          hUniDrcConfig, drcCharacteristicRequested - 1, ppCandidatesPotential,
          ppCandidatesSelected);
      if (retVal) return (retVal);
    } else if (drcCharacteristicRequested == 11) {
      retVal = _selectSingleDrcCharacteristic(
          hUniDrcConfig, 9, ppCandidatesPotential, ppCandidatesSelected);
      if (retVal) return (retVal);
    }
  }

  _swapSelection(ppCandidatesPotential, ppCandidatesSelected);

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetFinalSelection_peakValue0(
    DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected) {
  int32_t i;

  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    DRCDEC_SELECTION_DATA* pCandidate =
        _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    if (pCandidate->outputPeakLevel <= int32_t(0)) {
      if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
        return DRCDEC_SELECTION_PROCESS_NOT_OK;
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetFinalSelection_downmixId(
    HANDLE_SEL_PROC_INPUT hSelProcInput,
    DRCDEC_SELECTION** ppCandidatesPotential,
    DRCDEC_SELECTION** ppCandidatesSelected) {
  int32_t i, j;
  DRCDEC_SELECTION_DATA* pCandidate = NULL;
  DRC_INSTRUCTIONS_UNI_DRC* pInst = NULL;

  for (i = 0; i < _drcdec_selection_getNumber(*ppCandidatesPotential); i++) {
    pCandidate = _drcdec_selection_getAt(*ppCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    pInst = pCandidate->pInst;

    for (j = 0; j < pInst->downmixIdCount; j++) {
      if (DOWNMIX_ID_BASE_LAYOUT != pInst->downmixId[j] &&
          DOWNMIX_ID_ANY_DOWNMIX != pInst->downmixId[j] &&
          hSelProcInput
                  ->downmixIdRequested[pCandidate->downmixIdRequestIndex] ==
              pInst->downmixId[j]) {
        if (_drcdec_selection_add(*ppCandidatesSelected, pCandidate) == NULL)
          return DRCDEC_SELECTION_PROCESS_NOT_OK;
      }
    }
  }

  if (_drcdec_selection_getNumber(*ppCandidatesSelected) == 0) {
    _swapSelection(ppCandidatesPotential, ppCandidatesSelected);
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static int32_t _crossSum(int32_t value) {
  int32_t sum = 0;

  while (value != 0) {
    if ((value & 1) == 1) {
      sum++;
    }

    value >>= 1;
  }

  return sum;
}

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetFinalSelection_effectTypes(
    DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected) {
  int32_t i;
  int32_t minNumEffects = 1000;
  int32_t numEffects = 0;
  int32_t effects = 0;
  DRCDEC_SELECTION_DATA* pCandidate = NULL;
  DRC_INSTRUCTIONS_UNI_DRC* pInst = NULL;

  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    pInst = pCandidate->pInst;

    effects = pInst->drcSetEffect;
    effects &= 0xffff ^ (EB_GENERAL_COMPR);
    numEffects = _crossSum(effects);

    if (numEffects < minNumEffects) {
      minNumEffects = numEffects;
    }
  }

  /* add all with minimum number of effects */
  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    pInst = pCandidate->pInst;

    effects = pInst->drcSetEffect;
    effects &= 0xffff ^ (EB_GENERAL_COMPR);
    numEffects = _crossSum(effects);

    if (numEffects == minNumEffects) {
      if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
        return DRCDEC_SELECTION_PROCESS_NOT_OK;
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static DRCDEC_SELECTION_PROCESS_RETURN _selectSmallestTargetLoudnessValueUpper(
    DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected) {
  int32_t i;
  int8_t minVal = 0x7F;
  int8_t val = 0;
  DRCDEC_SELECTION_DATA* pCandidate = NULL;

  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    val = pCandidate->pInst->drcSetTargetLoudnessValueUpper;

    if (val < minVal) {
      minVal = val;
    }
  }

  /* add all with same smallest drcSetTargetLoudnessValueUpper */
  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    val = pCandidate->pInst->drcSetTargetLoudnessValueUpper;

    if (val == minVal) {
      if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
        return DRCDEC_SELECTION_PROCESS_NOT_OK;
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetFinalSelection_targetLoudness(
    int32_t targetLoudness, DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;
  int32_t i;
  DRCDEC_SELECTION_DATA* pCandidate = NULL;

  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    if (pCandidate->selectionFlag == 0) {
      if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
        return DRCDEC_SELECTION_PROCESS_NOT_OK;
    }
  }

  if (_drcdec_selection_getNumber(pCandidatesSelected) == 0) {
    retVal = _selectSmallestTargetLoudnessValueUpper(pCandidatesPotential,
                                                     pCandidatesSelected);
    if (retVal) return (retVal);
  }

  if (_drcdec_selection_getNumber(pCandidatesSelected) > 1) {
    DRC_INSTRUCTIONS_UNI_DRC* pDrcInstructionUniDrc = NULL;

    _swapSelectionAndClear(&pCandidatesPotential, &pCandidatesSelected);

    for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
      pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
      if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

      pDrcInstructionUniDrc = pCandidate->pInst;

      if (_targetLoudnessInRange(pDrcInstructionUniDrc, targetLoudness)) {
        if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
          return DRCDEC_SELECTION_PROCESS_NOT_OK;
      }
    }

    if (_drcdec_selection_getNumber(pCandidatesSelected) > 1) {
      _swapSelectionAndClear(&pCandidatesPotential, &pCandidatesSelected);

      retVal = _selectSmallestTargetLoudnessValueUpper(pCandidatesPotential,
                                                       pCandidatesSelected);
      if (retVal) return (retVal);
    }
  }

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetFinalSelection_peakValueLargest(
    DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected) {
  int32_t i;
  int32_t largestPeakLevel = MINVAL_DBL;
  int32_t peakLevel = 0;
  DRCDEC_SELECTION_DATA* pCandidate = NULL;

  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    peakLevel = pCandidate->outputPeakLevel;

    if (peakLevel > largestPeakLevel) {
      largestPeakLevel = peakLevel;
    }
  }

  /* add all with same largest peak level */
  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    peakLevel = pCandidate->outputPeakLevel;

    if (peakLevel == largestPeakLevel) {
      if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
        return DRCDEC_SELECTION_PROCESS_NOT_OK;
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetFinalSelection_drcSetId(
    DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected) {
  int32_t i;
  int32_t largestId = -1000;
  int32_t id = 0;
  DRCDEC_SELECTION_DATA* pCandidate = NULL;
  DRCDEC_SELECTION_DATA* pCandidateSelected = NULL;

  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    pCandidate = _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    id = pCandidate->pInst->drcSetId;

    if (id > largestId) {
      largestId = id;
      pCandidateSelected = pCandidate;
    }
  }

  if (pCandidateSelected != NULL) {
    if (_drcdec_selection_add(pCandidatesSelected, pCandidateSelected) == NULL)
      return DRCDEC_SELECTION_PROCESS_NOT_OK;
  } else {
    return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetFinalSelection(
    HANDLE_SEL_PROC_INPUT hSelProcInput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    DRCDEC_SELECTION** ppCandidatesPotential,
    DRCDEC_SELECTION** ppCandidatesSelected, SEL_PROC_CODEC_MODE codecMode) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;

  if (_drcdec_selection_getNumber(*ppCandidatesPotential) == 0) {
    return DRCDEC_SELECTION_PROCESS_NOT_OK;
  } else if (_drcdec_selection_getNumber(*ppCandidatesPotential) == 1) {
    _swapSelection(ppCandidatesPotential, ppCandidatesSelected);
    /* finished */
  } else /* > 1 */
  {
    retVal = _drcSetFinalSelection_peakValue0(*ppCandidatesPotential,
                                              *ppCandidatesSelected);
    if (retVal) return (retVal);

    if (_drcdec_selection_getNumber(*ppCandidatesSelected) > 1) {
      _swapSelectionAndClear(ppCandidatesPotential, ppCandidatesSelected);
      retVal = _drcSetFinalSelection_downmixId(
          hSelProcInput, ppCandidatesPotential, ppCandidatesSelected);
      if (retVal) return (retVal);
    }

    if (_drcdec_selection_getNumber(*ppCandidatesSelected) > 1) {
      _swapSelectionAndClear(ppCandidatesPotential, ppCandidatesSelected);
      retVal = _drcSetFinalSelection_effectTypes(*ppCandidatesPotential,
                                                 *ppCandidatesSelected);
      if (retVal) return (retVal);
    }

    if (_drcdec_selection_getNumber(*ppCandidatesSelected) > 1) {
      _swapSelectionAndClear(ppCandidatesPotential, ppCandidatesSelected);
      retVal = _drcSetFinalSelection_targetLoudness(
          hSelProcInput->targetLoudness, *ppCandidatesPotential,
          *ppCandidatesSelected);
      if (retVal) return (retVal);
    }

    if (_drcdec_selection_getNumber(*ppCandidatesSelected) > 1) {
      _swapSelectionAndClear(ppCandidatesPotential, ppCandidatesSelected);
      retVal = _drcSetFinalSelection_peakValueLargest(*ppCandidatesPotential,
                                                      *ppCandidatesSelected);
      if (retVal) return (retVal);
    }

    if (_drcdec_selection_getNumber(*ppCandidatesSelected) > 1) {
      _swapSelectionAndClear(ppCandidatesPotential, ppCandidatesSelected);
      retVal = _drcSetFinalSelection_drcSetId(*ppCandidatesPotential,
                                              *ppCandidatesSelected);
      if (retVal) return (retVal);
    }
  }

  if (_drcdec_selection_getNumber(*ppCandidatesSelected) == 0) {
    return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _generateVirtualDrcSets(
    HANDLE_SEL_PROC_INPUT hSelProcInput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    SEL_PROC_CODEC_MODE codecMode) {
  int32_t i;
  int32_t nMixes = hUniDrcConfig->downmixInstructionsCount + 1;
  int32_t index = hUniDrcConfig->drcInstructionsUniDrcCount;
  int32_t indexVirtual = -1;
  DRC_INSTRUCTIONS_UNI_DRC* pDrcInstruction =
      &(hUniDrcConfig->drcInstructionsUniDrc[index]);

  if (codecMode == SEL_PROC_MPEG_H_3DA) {
    nMixes = 1;
  }

  if ((index + nMixes) > (12 + 1 + 6)) {
    return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  memset(pDrcInstruction, 0, sizeof(DRC_INSTRUCTIONS_UNI_DRC));

  pDrcInstruction->drcSetId = indexVirtual;
  index++;
  indexVirtual--;
  pDrcInstruction->downmixIdCount = 1;

  if ((codecMode == SEL_PROC_MPEG_H_3DA) &&
      (hSelProcInput->numDownmixIdRequests)) {
    pDrcInstruction->downmixId[0] = hSelProcInput->downmixIdRequested[0];
  } else {
    pDrcInstruction->downmixId[0] = DOWNMIX_ID_BASE_LAYOUT;
  }

  for (i = 1; i < nMixes; i++) {
    pDrcInstruction = &(hUniDrcConfig->drcInstructionsUniDrc[index]);
    memset(pDrcInstruction, 0, sizeof(DRC_INSTRUCTIONS_UNI_DRC));
    pDrcInstruction->drcSetId = indexVirtual;
    pDrcInstruction->downmixId[0] =
        hUniDrcConfig->downmixInstructions[i - 1].downmixId;
    pDrcInstruction->downmixIdCount = 1;
    index++;
    indexVirtual--;
  }

  hUniDrcConfig->drcInstructionsCountInclVirtual =
      hUniDrcConfig->drcInstructionsUniDrcCount + nMixes;

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static DRCDEC_SELECTION_PROCESS_RETURN _generateOutputInfo(
    HANDLE_SEL_PROC_INPUT hSelProcInput, HANDLE_SEL_PROC_OUTPUT hSelProcOutput,
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
    DRCDEC_SELECTION_DATA* pSelectionData, SEL_PROC_CODEC_MODE codecMode) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;

  int32_t i, j;
  int32_t hasDependend = 0;
  int32_t hasFading = 0;
  int32_t hasDucking = 0;
  int32_t selectedDrcSetIds;
  int32_t selectedDownmixIds;
  int32_t mixingLevel = 0;
  int32_t albumMode = hSelProcInput->albumMode;
  uint8_t* pDownmixIdRequested = hSelProcInput->downmixIdRequested;
  int16_t boost = hSelProcInput->boost;
  int16_t compress = hSelProcInput->compress;

  hSelProcOutput->numSelectedDrcSets = 1;
  hSelProcOutput->selectedDrcSetIds[0] = pSelectionData->pInst->drcSetId;
  hSelProcOutput->selectedDownmixIds[0] =
      pSelectionData->pInst->drcApplyToDownmix == 1
          ? pSelectionData->pInst->downmixId[0]
          : 0;
  hSelProcOutput->loudnessNormalizationGainDb =
      pSelectionData->loudnessNormalizationGainDbAdjusted +
      hSelProcInput->loudnessNormalizationGainModificationDb;
  hSelProcOutput->outputPeakLevelDb = pSelectionData->outputPeakLevel;
  hSelProcOutput->outputLoudness = pSelectionData->outputLoudness;

  hSelProcOutput->boost = boost;
  hSelProcOutput->compress = compress;
  hSelProcOutput->baseChannelCount =
      hUniDrcConfig->channelLayout.baseChannelCount;
  hSelProcOutput->targetChannelCount =
      hUniDrcConfig->channelLayout.baseChannelCount;
  hSelProcOutput->activeDownmixId =
      pDownmixIdRequested[pSelectionData->downmixIdRequestIndex];

  _getMixingLevel(hLoudnessInfoSet, *pDownmixIdRequested,
                  hSelProcOutput->selectedDrcSetIds[0], albumMode,
                  &mixingLevel);
  hSelProcOutput->mixingLevel = mixingLevel;

  /*dependent*/
  if (pSelectionData->pInst->dependsOnDrcSetPresent) {
    int32_t dependsOnDrcSetID = pSelectionData->pInst->dependsOnDrcSet;

    for (i = 0; i < hUniDrcConfig->drcInstructionsCountInclVirtual; i++) {
      DRC_INSTRUCTIONS_UNI_DRC* pInst =
          &(hUniDrcConfig->drcInstructionsUniDrc[i]);
      if (!_drcSetIsUsable(hUniDrcConfig, pInst)) continue;

      if (pInst->drcSetId == dependsOnDrcSetID) {
        hSelProcOutput->selectedDrcSetIds[hSelProcOutput->numSelectedDrcSets] =
            hUniDrcConfig->drcInstructionsUniDrc[i].drcSetId;
        hSelProcOutput->selectedDownmixIds[hSelProcOutput->numSelectedDrcSets] =
            hUniDrcConfig->drcInstructionsUniDrc[i].drcApplyToDownmix == 1
                ? hUniDrcConfig->drcInstructionsUniDrc[i].downmixId[0]
                : 0;
        hSelProcOutput->numSelectedDrcSets++;
        hasDependend = 1;
        break;
      }
    }
  }

  /* fading */
  if (hSelProcInput->albumMode == 0) {
    for (i = 0; i < hUniDrcConfig->drcInstructionsUniDrcCount; i++) {
      DRC_INSTRUCTIONS_UNI_DRC* pInst =
          &(hUniDrcConfig->drcInstructionsUniDrc[i]);
      if (!_drcSetIsUsable(hUniDrcConfig, pInst)) continue;

      if (pInst->drcSetEffect & EB_FADE) {
        if (pInst->downmixId[0] == DOWNMIX_ID_ANY_DOWNMIX) {
          hSelProcOutput->numSelectedDrcSets = hasDependend + 1;
          hSelProcOutput
              ->selectedDrcSetIds[hSelProcOutput->numSelectedDrcSets] =
              hUniDrcConfig->drcInstructionsUniDrc[i].drcSetId;
          hSelProcOutput
              ->selectedDownmixIds[hSelProcOutput->numSelectedDrcSets] =
              hUniDrcConfig->drcInstructionsUniDrc[i].drcApplyToDownmix == 1
                  ? hUniDrcConfig->drcInstructionsUniDrc[i].downmixId[0]
                  : 0;
          hSelProcOutput->numSelectedDrcSets++;
          hasFading = 1;

        } else {
          retVal = DRCDEC_SELECTION_PROCESS_NOT_OK;
          if (retVal) return DRCDEC_SELECTION_PROCESS_NOT_OK;
        }
      }
    }
  }

  /* ducking */
  for (i = 0; i < hUniDrcConfig->drcInstructionsUniDrcCount; i++) {
    DRC_INSTRUCTIONS_UNI_DRC* pInst =
        &(hUniDrcConfig->drcInstructionsUniDrc[i]);
    if (!_drcSetIsUsable(hUniDrcConfig, pInst)) continue;

    if (pInst->drcSetEffect & (EB_DUCK_OTHER | EB_DUCK_SELF)) {
      for (j = 0; j < pInst->downmixIdCount; j++) {
        if (pInst->downmixId[j] == hSelProcOutput->activeDownmixId) {
          hSelProcOutput->numSelectedDrcSets =
              hasDependend + 1; /* ducking overrides fading */

          hSelProcOutput
              ->selectedDrcSetIds[hSelProcOutput->numSelectedDrcSets] =
              hUniDrcConfig->drcInstructionsUniDrc[i].drcSetId;
          /* force ducking DRC set to be processed on base layout */
          hSelProcOutput
              ->selectedDownmixIds[hSelProcOutput->numSelectedDrcSets] = 0;
          hSelProcOutput->numSelectedDrcSets++;
          hasDucking = 1;
        }
      }
    }
  }

  /* repeat for DOWNMIX_ID_BASE_LAYOUT if no ducking found*/

  if (!hasDucking) {
    for (i = 0; i < hUniDrcConfig->drcInstructionsUniDrcCount; i++) {
      DRC_INSTRUCTIONS_UNI_DRC* pInst =
          &(hUniDrcConfig->drcInstructionsUniDrc[i]);
      if (!_drcSetIsUsable(hUniDrcConfig, pInst)) continue;

      if (pInst->drcSetEffect & (EB_DUCK_OTHER | EB_DUCK_SELF)) {
        for (j = 0; j < pInst->downmixIdCount; j++) {
          if (pInst->downmixId[j] == DOWNMIX_ID_BASE_LAYOUT) {
            hSelProcOutput->numSelectedDrcSets = hasDependend + hasFading + 1;
            hSelProcOutput
                ->selectedDrcSetIds[hSelProcOutput->numSelectedDrcSets] =
                hUniDrcConfig->drcInstructionsUniDrc[i].drcSetId;
            /* force ducking DRC set to be processed on base layout */
            hSelProcOutput
                ->selectedDownmixIds[hSelProcOutput->numSelectedDrcSets] = 0;
            hSelProcOutput->numSelectedDrcSets++;
          }
        }
      }
    }
  }

  if (hSelProcOutput->numSelectedDrcSets > 3) {
    /* maximum permitted number of applied DRC sets is 3, see section 6.3.5 of
     * ISO/IEC 23003-4 */
    hSelProcOutput->numSelectedDrcSets = 0;
    return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  /* sorting: Ducking/Fading -> Dependent -> Selected */
  if (hSelProcOutput->numSelectedDrcSets == 3) {
    selectedDrcSetIds = hSelProcOutput->selectedDrcSetIds[0];
    selectedDownmixIds = hSelProcOutput->selectedDownmixIds[0];
    hSelProcOutput->selectedDrcSetIds[0] = hSelProcOutput->selectedDrcSetIds[2];
    hSelProcOutput->selectedDownmixIds[0] =
        hSelProcOutput->selectedDownmixIds[2];
    hSelProcOutput->selectedDrcSetIds[2] = selectedDrcSetIds;
    hSelProcOutput->selectedDownmixIds[2] = selectedDownmixIds;
  } else if (hSelProcOutput->numSelectedDrcSets == 2) {
    selectedDrcSetIds = hSelProcOutput->selectedDrcSetIds[0];
    selectedDownmixIds = hSelProcOutput->selectedDownmixIds[0];
    hSelProcOutput->selectedDrcSetIds[0] = hSelProcOutput->selectedDrcSetIds[1];
    hSelProcOutput->selectedDownmixIds[0] =
        hSelProcOutput->selectedDownmixIds[1];
    hSelProcOutput->selectedDrcSetIds[1] = selectedDrcSetIds;
    hSelProcOutput->selectedDownmixIds[1] = selectedDownmixIds;
  }

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _selectDownmixMatrix(
    HANDLE_SEL_PROC_OUTPUT hSelProcOutput,
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig) {
  int32_t i;
  hSelProcOutput->baseChannelCount =
      hUniDrcConfig->channelLayout.baseChannelCount;
  hSelProcOutput->targetChannelCount =
      hUniDrcConfig->channelLayout.baseChannelCount;
  hSelProcOutput->targetLayout = -1;
  hSelProcOutput->downmixMatrixPresent = 0;

  if (hSelProcOutput->activeDownmixId != 0) {
    for (i = 0; i < hUniDrcConfig->downmixInstructionsCount; i++) {
      DOWNMIX_INSTRUCTIONS* pDown = &(hUniDrcConfig->downmixInstructions[i]);
      if (pDown->targetChannelCount > 8) {
        continue;
      }

      if (hSelProcOutput->activeDownmixId == pDown->downmixId) {
        hSelProcOutput->targetChannelCount = pDown->targetChannelCount;
        hSelProcOutput->targetLayout = pDown->targetLayout;

        if (pDown->downmixCoefficientsPresent) {
          int32_t j, k;
          int32_t downmixOffset = getDownmixOffset(
              pDown, hSelProcOutput->baseChannelCount); /* e = 1 */

          for (j = 0; j < hSelProcOutput->baseChannelCount; j++) {
            for (k = 0; k < hSelProcOutput->targetChannelCount; k++) {
              hSelProcOutput->downmixMatrix[j][k] =
                  fMultDiv2(
                      downmixOffset,
                      pDown->downmixCoefficient[j + k * hSelProcOutput
                                                            ->baseChannelCount])
                  << 2;
            }
          }

          hSelProcOutput->downmixMatrixPresent = 1;
        }
        break;
      }
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetPreSelection(
    SEL_PROC_INPUT* hSelProcInput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
    DRCDEC_SELECTION** ppCandidatesPotential,
    DRCDEC_SELECTION** ppCandidatesSelected, SEL_PROC_CODEC_MODE codecMode) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;
  int32_t i, j;

  for (i = 0; i < hSelProcInput->numDownmixIdRequests; i++) {
    for (j = 0; j < hUniDrcConfig->drcInstructionsCountInclVirtual; j++) {
      DRC_INSTRUCTIONS_UNI_DRC* pDrcInstruction =
          &(hUniDrcConfig->drcInstructionsUniDrc[j]);
      /* check if ID is unique */
      if (selectDrcInstructions(hUniDrcConfig, pDrcInstruction->drcSetId) !=
          pDrcInstruction)
        continue;

      retVal = _drcSetPreSelectionSingleInstruction(
          hSelProcInput, i, hUniDrcConfig, hLoudnessInfoSet, pDrcInstruction,
          *ppCandidatesPotential, *ppCandidatesSelected, codecMode);
      if (retVal) return DRCDEC_SELECTION_PROCESS_NOT_OK;
    }
  }

  retVal = _preSelectionRequirement9(hSelProcInput, *ppCandidatesPotential,
                                     *ppCandidatesSelected);
  if (retVal) return DRCDEC_SELECTION_PROCESS_NOT_OK;

  if (_drcdec_selection_getNumber(*ppCandidatesSelected) == 0) {
    retVal = _drcSetSelectionAddCandidates(
        hSelProcInput, *ppCandidatesPotential, *ppCandidatesSelected);
    if (retVal) return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _drcSetRequestSelection(
    SEL_PROC_INPUT* hSelProcInput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
    DRCDEC_SELECTION** ppCandidatesPotential,
    DRCDEC_SELECTION** ppCandidatesSelected) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal;
  int32_t i;

  if (_drcdec_selection_getNumber(*ppCandidatesPotential) == 0) {
    retVal = DRCDEC_SELECTION_PROCESS_NOT_OK;
    if (retVal) return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  if (hSelProcInput->dynamicRangeControlOn) {
    if (hSelProcInput->numDrcFeatureRequests == 0) {
      retVal = _selectDrcSetEffectNone(hUniDrcConfig, *ppCandidatesPotential,
                                       *ppCandidatesSelected);
      if (retVal) return (retVal);

      if (_drcdec_selection_getNumber(*ppCandidatesSelected) == 0) {
        DRC_FEATURE_REQUEST fallbackRequest;
        fallbackRequest.drcEffectType.numRequests = 5;
        fallbackRequest.drcEffectType.numRequestsDesired = 5;
        fallbackRequest.drcEffectType.request[0] = DETR_GENERAL_COMPR;
        fallbackRequest.drcEffectType.request[1] = DETR_NIGHT;
        fallbackRequest.drcEffectType.request[2] = DETR_NOISY;
        fallbackRequest.drcEffectType.request[3] = DETR_LIMITED;
        fallbackRequest.drcEffectType.request[4] = DETR_LOWLEVEL;

        retVal = _selectEffectTypeFeature(hUniDrcConfig, fallbackRequest,
                                          ppCandidatesPotential,
                                          ppCandidatesSelected);
        if (retVal) return DRCDEC_SELECTION_PROCESS_NOT_OK;
      }

      _swapSelectionAndClear(ppCandidatesPotential, ppCandidatesSelected);
    } else {
      for (i = 0; i < hSelProcInput->numDrcFeatureRequests; i++) {
        if (hSelProcInput->drcFeatureRequestType[i] == DFRT_EFFECT_TYPE) {
          retVal = _selectEffectTypeFeature(
              hUniDrcConfig, hSelProcInput->drcFeatureRequest[i],
              ppCandidatesPotential, ppCandidatesSelected);

          _swapSelectionAndClear(ppCandidatesPotential, ppCandidatesSelected);
          if (retVal) return DRCDEC_SELECTION_PROCESS_NOT_OK;
        }

        else if (hSelProcInput->drcFeatureRequestType[i] ==
                 DFRT_DYNAMIC_RANGE) {
          retVal = _selectDynamicRange(
              hUniDrcConfig, hLoudnessInfoSet,
              hSelProcInput->drcFeatureRequest[i],
              hSelProcInput->downmixIdRequested, hSelProcInput->albumMode,
              *ppCandidatesPotential, *ppCandidatesSelected);

          if (_drcdec_selection_getNumber(*ppCandidatesSelected) > 0) {
            _swapSelectionAndClear(ppCandidatesPotential, ppCandidatesSelected);
          }
          if (retVal) return DRCDEC_SELECTION_PROCESS_NOT_OK;
        } else if (hSelProcInput->drcFeatureRequestType[i] ==
                   DFRT_DRC_CHARACTERISTIC) {
          retVal = _selectDrcCharacteristic(
              hUniDrcConfig,
              hSelProcInput->drcFeatureRequest[i].drcCharacteristic,
              ppCandidatesPotential, ppCandidatesSelected);

          if (_drcdec_selection_getNumber(*ppCandidatesSelected) > 0) {
            _swapSelectionAndClear(ppCandidatesPotential, ppCandidatesSelected);
          }
          if (retVal) return DRCDEC_SELECTION_PROCESS_NOT_OK;
        }
      }
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

/*******************************************/
static DRCDEC_SELECTION_PROCESS_RETURN _dynamicRangeMeasurement(
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet, DRC_INSTRUCTIONS_UNI_DRC* pInst,
    uint8_t downmixIdRequested,
    DYN_RANGE_MEASUREMENT_REQUEST_TYPE dynamicRangeMeasurementType,
    int32_t albumMode, int32_t* pPeakToAveragePresent, int32_t* pPeakToAverage) {
  int32_t i;
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;
  int32_t drcSetId = fMax(0, pInst->drcSetId);

  *pPeakToAveragePresent = 0;

  if (albumMode) {
    for (i = 0; i < hLoudnessInfoSet->loudnessInfoAlbumCount; i++) {
      LOUDNESS_INFO* pLoudnessInfo = &(hLoudnessInfoSet->loudnessInfoAlbum[i]);

      if (drcSetId == pLoudnessInfo->drcSetId) {
        if (downmixIdRequested == pLoudnessInfo->downmixId) {
          retVal = _extractLoudnessPeakToAverageValue(
              pLoudnessInfo, dynamicRangeMeasurementType, pPeakToAveragePresent,
              pPeakToAverage);
          if (retVal) return (retVal);
        }
      }
    }
  }

  if (*pPeakToAveragePresent == 0) {
    for (i = 0; i < hLoudnessInfoSet->loudnessInfoCount; i++) {
      LOUDNESS_INFO* pLoudnessInfo = &(hLoudnessInfoSet->loudnessInfo[i]);

      if (drcSetId == pLoudnessInfo->drcSetId) {
        if (downmixIdRequested == pLoudnessInfo->downmixId) {
          retVal = _extractLoudnessPeakToAverageValue(
              pLoudnessInfo, dynamicRangeMeasurementType, pPeakToAveragePresent,
              pPeakToAverage);
          if (retVal) return (retVal);
        }
      }
    }
  }

  return retVal;
}
/*******************************************/

static DRCDEC_SELECTION_DATA* _drcdec_selection_addNew(
    DRCDEC_SELECTION* pSelection) {
  if (pSelection->numData < (12 + 1 + 6)) {
    DRCDEC_SELECTION_DATA* pData = &(pSelection->data[pSelection->numData]);
    memset(pData, 0, sizeof(DRCDEC_SELECTION_DATA));
    pSelection->numData++;

    return pData;
  } else {
    return NULL;
  }
}

static DRCDEC_SELECTION_DATA* _drcdec_selection_add(
    DRCDEC_SELECTION* pSelection, DRCDEC_SELECTION_DATA* pDataIn) {
  if (pSelection->numData < (12 + 1 + 6)) {
    DRCDEC_SELECTION_DATA* pData = &(pSelection->data[pSelection->numData]);
    memcpy(pData, pDataIn, sizeof(DRCDEC_SELECTION_DATA));
    pSelection->numData++;
    return pData;
  } else {
    return NULL;
  }
}

static int32_t _drcdec_selection_clear(DRCDEC_SELECTION* pSelection) {
  return pSelection->numData = 0;
}

static int32_t _drcdec_selection_getNumber(DRCDEC_SELECTION* pSelection) {
  return pSelection->numData;
}

static int32_t _drcdec_selection_setNumber(DRCDEC_SELECTION* pSelection, int32_t num) {
  if (num >= 0 && num < pSelection->numData) {
    return pSelection->numData = num;
  } else {
    return pSelection->numData;
  }
}

static DRCDEC_SELECTION_DATA* _drcdec_selection_getAt(
    DRCDEC_SELECTION* pSelection, int32_t at) {
  if (at >= 0 && at < (12 + 1 + 6)) {
    return &(pSelection->data[at]);
  } else {
    return NULL;
  }
}

static int32_t _swapSelectionAndClear(DRCDEC_SELECTION** ppCandidatesPotential,
                                  DRCDEC_SELECTION** ppCandidatesSelected) {
  DRCDEC_SELECTION* pTmp = *ppCandidatesPotential;
  *ppCandidatesPotential = *ppCandidatesSelected;
  *ppCandidatesSelected = pTmp;
  _drcdec_selection_clear(*ppCandidatesSelected);
  return 0;
}

static int32_t _swapSelection(DRCDEC_SELECTION** ppCandidatesPotential,
                          DRCDEC_SELECTION** ppCandidatesSelected) {
  DRCDEC_SELECTION* pTmp = *ppCandidatesPotential;
  *ppCandidatesPotential = *ppCandidatesSelected;
  *ppCandidatesSelected = pTmp;
  return 0;
}

/*******************************************/

static LOUDNESS_INFO* _getLoudnessInfoStructure(
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet, int32_t drcSetId, int32_t downmixId,
    int32_t albumMode) {
  int32_t i, j;
  int32_t count;

  LOUDNESS_INFO* pLoudnessInfo = NULL;

  if (albumMode) {
    count = hLoudnessInfoSet->loudnessInfoAlbumCount;
    pLoudnessInfo = hLoudnessInfoSet->loudnessInfoAlbum;
  } else {
    count = hLoudnessInfoSet->loudnessInfoCount;
    pLoudnessInfo = hLoudnessInfoSet->loudnessInfo;
  }

  for (i = 0; i < count; i++) {
    if ((pLoudnessInfo[i].drcSetId == drcSetId) &&
        (pLoudnessInfo[i].downmixId == downmixId)) {
      for (j = 0; j < pLoudnessInfo[i].measurementCount; j++) {
        if ((pLoudnessInfo[i].loudnessMeasurement[j].methodDefinition == 1) ||
            (pLoudnessInfo[i].loudnessMeasurement[j].methodDefinition == 2)) {
          return &pLoudnessInfo[i];
        }
      }
    }
  }

  return NULL;
}

static LOUDNESS_INFO* _getApplicableLoudnessInfoStructure(
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet, int32_t drcSetId,
    int32_t downmixIdRequested, int32_t albumMode) {
  LOUDNESS_INFO* pLoudnessInfo = NULL;

  /* default value */
  pLoudnessInfo = _getLoudnessInfoStructure(hLoudnessInfoSet, drcSetId,
                                            downmixIdRequested, albumMode);

  /* fallback values */
  if (pLoudnessInfo == NULL) {
    pLoudnessInfo =
        _getLoudnessInfoStructure(hLoudnessInfoSet, drcSetId, 0x7F, albumMode);
  }

  if (pLoudnessInfo == NULL) {
    pLoudnessInfo = _getLoudnessInfoStructure(hLoudnessInfoSet, 0x3F,
                                              downmixIdRequested, albumMode);
  }

  if (pLoudnessInfo == NULL) {
    pLoudnessInfo = _getLoudnessInfoStructure(hLoudnessInfoSet, 0,
                                              downmixIdRequested, albumMode);
  }

  if (pLoudnessInfo == NULL) {
    pLoudnessInfo =
        _getLoudnessInfoStructure(hLoudnessInfoSet, 0x3F, 0x7F, albumMode);
  }

  if (pLoudnessInfo == NULL) {
    pLoudnessInfo =
        _getLoudnessInfoStructure(hLoudnessInfoSet, 0, 0x7F, albumMode);
  }

  if (pLoudnessInfo == NULL) {
    pLoudnessInfo =
        _getLoudnessInfoStructure(hLoudnessInfoSet, drcSetId, 0, albumMode);
  }

  if (pLoudnessInfo == NULL) {
    pLoudnessInfo =
        _getLoudnessInfoStructure(hLoudnessInfoSet, 0x3F, 0, albumMode);
  }

  if (pLoudnessInfo == NULL) {
    pLoudnessInfo =
        _getLoudnessInfoStructure(hLoudnessInfoSet, 0, 0, albumMode);
  }

  return pLoudnessInfo;
}

/*******************************************/

typedef struct {
  int32_t value;
  int32_t order;
} VALUE_ORDER;

void _initValueOrder(VALUE_ORDER* pValue) {
  pValue->value = (int32_t)0;
  pValue->order = -1;
}

enum {
  MS_BONUS0 = 0,
  MS_BONUS1770,
  MS_BONUSUSER,
  MS_BONUSEXPERT,
  MS_RESA,
  MS_RESB,
  MS_RESC,
  MS_RESD,
  MS_RESE,
  MS_PROGRAMLOUDNESS,
  MS_PEAKLOUDNESS
};

static DRCDEC_SELECTION_PROCESS_RETURN _getMethodValue(
    VALUE_ORDER* pValueOrder, int32_t value, int32_t measurementSystem,
    int32_t measurementSystemRequested) {
  const int32_t rows = 11;
  const int32_t columns = 12;
  const int32_t pOrdering[rows][columns] = {
      {0, 0, 8, 0, 1, 3, 0, 5, 6, 7, 4, 2}, /* default = bonus1770 */
      {0, 0, 8, 0, 1, 3, 0, 5, 6, 7, 4, 2}, /* bonus1770 */
      {0, 0, 1, 0, 8, 5, 0, 2, 3, 4, 6, 7}, /* bonusUser */
      {0, 0, 3, 0, 1, 8, 0, 4, 5, 6, 7, 2}, /* bonusExpert */
      {0, 0, 5, 0, 1, 3, 0, 8, 6, 7, 4, 2}, /* ResA */
      {0, 0, 5, 0, 1, 3, 0, 6, 8, 7, 4, 2}, /* ResB */
      {0, 0, 5, 0, 1, 3, 0, 6, 7, 8, 4, 2}, /* ResC */
      {0, 0, 3, 0, 1, 7, 0, 4, 5, 6, 8, 2}, /* ResD */
      {0, 0, 1, 0, 7, 5, 0, 2, 3, 4, 6, 8}, /* ResE */
      {0, 0, 1, 0, 0, 0, 0, 2, 3, 4, 0, 0}, /* ProgramLoudness */
      {0, 7, 0, 0, 0, 0, 6, 5, 4, 3, 2, 1}  /* PeakLoudness */
  };

  if (measurementSystemRequested < 0 || measurementSystemRequested >= rows ||
      measurementSystem < 0 || measurementSystem >= columns) {
    return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  if (pOrdering[measurementSystemRequested][measurementSystem] >
      pValueOrder->order) {
    pValueOrder->order =
        pOrdering[measurementSystemRequested][measurementSystem];
    pValueOrder->value = value;
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

/*******************************************/

static DRCDEC_SELECTION_PROCESS_RETURN _getLoudness(
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet, int32_t albumMode,
    METHOD_DEFINITION_REQUEST measurementMethodRequested,
    MEASUREMENT_SYSTEM_REQUEST measurementSystemRequested,
    int32_t targetLoudness, /* e = 7 */
    int32_t drcSetId, int32_t downmixIdRequested,
    int32_t* pLoudnessNormalizationGain, /* e = 7 */
    int32_t* pLoudness)                  /* e = 7 */
{
  int32_t index;

  LOUDNESS_INFO* pLoudnessInfo = NULL;
  VALUE_ORDER valueOrder;

  /* map MDR_DEFAULT to MDR_PROGRAM_LOUDNESS */
  METHOD_DEFINITION_REQUEST requestedMethodDefinition =
      measurementMethodRequested < MDR_ANCHOR_LOUDNESS ? MDR_PROGRAM_LOUDNESS
                                                       : MDR_ANCHOR_LOUDNESS;

  if (measurementMethodRequested > MDR_ANCHOR_LOUDNESS) {
    return DRCDEC_SELECTION_PROCESS_NOT_OK;
  }

  _initValueOrder(&valueOrder);

  *pLoudness = UNDEFINED_LOUDNESS_VALUE;
  *pLoudnessNormalizationGain = (int32_t)0;

  if (drcSetId < 0) {
    drcSetId = 0;
  }

  pLoudnessInfo = _getApplicableLoudnessInfoStructure(
      hLoudnessInfoSet, drcSetId, downmixIdRequested, albumMode);

  if (albumMode && (pLoudnessInfo == NULL)) {
    pLoudnessInfo = _getApplicableLoudnessInfoStructure(
        hLoudnessInfoSet, drcSetId, downmixIdRequested, 0);
  }

  if (pLoudnessInfo == NULL) {
    return DRCDEC_SELECTION_PROCESS_NO_ERROR;
  }

  index = -1;

  do {
    index = _findMethodDefinition(pLoudnessInfo, requestedMethodDefinition,
                                  index + 1);

    if (index >= 0) {
      _getMethodValue(
          &valueOrder, pLoudnessInfo->loudnessMeasurement[index].methodValue,
          pLoudnessInfo->loudnessMeasurement[index].measurementSystem,
          measurementSystemRequested);
    }
  } while (index >= 0);

  /* repeat with other method definition */
  if (valueOrder.order == -1) {
    index = -1;

    do {
      index = _findMethodDefinition(
          pLoudnessInfo,
          requestedMethodDefinition == MDR_PROGRAM_LOUDNESS
              ? MDR_ANCHOR_LOUDNESS
              : MDR_PROGRAM_LOUDNESS,
          index + 1);

      if (index >= 0) {
        _getMethodValue(
            &valueOrder, pLoudnessInfo->loudnessMeasurement[index].methodValue,
            pLoudnessInfo->loudnessMeasurement[index].measurementSystem,
            measurementSystemRequested);
      }
    } while (index >= 0);
  }

  if (valueOrder.order == -1) {
    return DRCDEC_SELECTION_PROCESS_NOT_OK;
  } else {
    *pLoudnessNormalizationGain = targetLoudness - valueOrder.value;
    *pLoudness = valueOrder.value;
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

/*******************************************/

static int32_t _truePeakLevelIsPresent(HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
                                   int32_t drcSetId, int32_t downmixId, int32_t albumMode) {
  int32_t i;
  int32_t count;
  LOUDNESS_INFO* pLoudnessInfo = NULL;

  if (albumMode) {
    count = hLoudnessInfoSet->loudnessInfoAlbumCount;
    pLoudnessInfo = hLoudnessInfoSet->loudnessInfoAlbum;
  } else {
    count = hLoudnessInfoSet->loudnessInfoCount;
    pLoudnessInfo = hLoudnessInfoSet->loudnessInfo;
  }

  for (i = 0; i < count; i++) {
    if ((pLoudnessInfo[i].drcSetId == drcSetId) &&
        (pLoudnessInfo[i].downmixId == downmixId)) {
      if (pLoudnessInfo[i].truePeakLevelPresent) return 1;
    }
  }

  return 0;
}

static DRCDEC_SELECTION_PROCESS_RETURN _getTruePeakLevel(
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet, int32_t drcSetId, int32_t downmixId,
    int32_t albumMode, int32_t* pTruePeakLevel) {
  int32_t i;
  int32_t count;
  LOUDNESS_INFO* pLoudnessInfo = NULL;

  if (albumMode) {
    count = hLoudnessInfoSet->loudnessInfoAlbumCount;
    pLoudnessInfo = hLoudnessInfoSet->loudnessInfoAlbum;
  } else {
    count = hLoudnessInfoSet->loudnessInfoCount;
    pLoudnessInfo = hLoudnessInfoSet->loudnessInfo;
  }

  for (i = 0; i < count; i++) {
    if ((pLoudnessInfo[i].drcSetId == drcSetId) &&
        (pLoudnessInfo[i].downmixId == downmixId)) {
      if (pLoudnessInfo[i].truePeakLevelPresent) {
        *pTruePeakLevel = pLoudnessInfo[i].truePeakLevel;
        return DRCDEC_SELECTION_PROCESS_NO_ERROR;
      }
    }
  }

  return DRCDEC_SELECTION_PROCESS_NOT_OK;
}

static int32_t _samplePeakLevelIsPresent(HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
                                     int32_t drcSetId, int32_t downmixId,
                                     int32_t albumMode) {
  int32_t i;
  int32_t count;
  LOUDNESS_INFO* pLoudnessInfo = NULL;

  if (albumMode) {
    count = hLoudnessInfoSet->loudnessInfoAlbumCount;
    pLoudnessInfo = hLoudnessInfoSet->loudnessInfoAlbum;
  } else {
    count = hLoudnessInfoSet->loudnessInfoCount;
    pLoudnessInfo = hLoudnessInfoSet->loudnessInfo;
  }

  for (i = 0; i < count; i++) {
    if ((pLoudnessInfo[i].drcSetId == drcSetId) &&
        (pLoudnessInfo[i].downmixId == downmixId)) {
      if (pLoudnessInfo[i].samplePeakLevelPresent) return 1;
    }
  }

  return 0;
}

static DRCDEC_SELECTION_PROCESS_RETURN _getSamplePeakLevel(
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet, int32_t drcSetId, int32_t downmixId,
    int32_t albumMode, int32_t* pSamplePeakLevel /* e = 7 */
) {
  int32_t i;
  int32_t count;
  LOUDNESS_INFO* pLoudnessInfo = NULL;

  if (albumMode) {
    count = hLoudnessInfoSet->loudnessInfoAlbumCount;
    pLoudnessInfo = hLoudnessInfoSet->loudnessInfoAlbum;
  } else {
    count = hLoudnessInfoSet->loudnessInfoCount;
    pLoudnessInfo = hLoudnessInfoSet->loudnessInfo;
  }

  for (i = 0; i < count; i++) {
    if ((pLoudnessInfo[i].drcSetId == drcSetId) &&
        (pLoudnessInfo[i].downmixId == downmixId)) {
      if (pLoudnessInfo[i].samplePeakLevelPresent) {
        *pSamplePeakLevel = pLoudnessInfo[i].samplePeakLevel;
        return DRCDEC_SELECTION_PROCESS_NO_ERROR;
      }
    }
  }

  return DRCDEC_SELECTION_PROCESS_NOT_OK;
}

static int32_t _limiterPeakTargetIsPresent(
    DRC_INSTRUCTIONS_UNI_DRC* pDrcInstruction, int32_t drcSetId, int32_t downmixId) {
  int32_t i;

  if (pDrcInstruction->limiterPeakTargetPresent) {
    if ((pDrcInstruction->downmixId[0] == downmixId) ||
        (pDrcInstruction->downmixId[0] == 0x7F)) {
      return 1;
    }

    for (i = 0; i < pDrcInstruction->downmixIdCount; i++) {
      if (pDrcInstruction->downmixId[i] == downmixId) {
        return 1;
      }
    }
  }

  return 0;
}

static DRCDEC_SELECTION_PROCESS_RETURN _getLimiterPeakTarget(
    DRC_INSTRUCTIONS_UNI_DRC* pDrcInstruction, int32_t drcSetId, int32_t downmixId,
    int32_t* pLimiterPeakTarget) {
  int32_t i;

  if (pDrcInstruction->limiterPeakTargetPresent) {
    if ((pDrcInstruction->downmixId[0] == downmixId) ||
        (pDrcInstruction->downmixId[0] == 0x7F)) {
      *pLimiterPeakTarget =
          ((FX_SGL2FX_DBL(pDrcInstruction->limiterPeakTarget) >> 2));
      return DRCDEC_SELECTION_PROCESS_NO_ERROR;
    }

    for (i = 0; i < pDrcInstruction->downmixIdCount; i++) {
      if (pDrcInstruction->downmixId[i] == downmixId) {
        *pLimiterPeakTarget =
            ((FX_SGL2FX_DBL(pDrcInstruction->limiterPeakTarget) >> 2));
        return DRCDEC_SELECTION_PROCESS_NO_ERROR;
      }
    }
  }

  return DRCDEC_SELECTION_PROCESS_NOT_OK;
}

static int32_t _downmixCoefficientsArePresent(HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
                                          int32_t downmixId, int32_t* pIndex) {
  int32_t i;
  *pIndex = -1;

  for (i = 0; i < hUniDrcConfig->downmixInstructionsCount; i++) {
    if (hUniDrcConfig->downmixInstructions[i].downmixId == downmixId) {
      if (hUniDrcConfig->downmixInstructions[i].downmixCoefficientsPresent) {
        if (hUniDrcConfig->downmixInstructions[i].targetChannelCount > 8)
          return 0;
        *pIndex = i;
        return 1;
      }
    }
  }

  return 0;
}

static DRCDEC_SELECTION_PROCESS_RETURN _getSignalPeakLevel(
    HANDLE_SEL_PROC_INPUT hSelProcInput, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet, DRC_INSTRUCTIONS_UNI_DRC* pInst,
    int32_t downmixIdRequested, int32_t* explicitPeakInformationPresent,
    int32_t* signalPeakLevelOut, /* e = 7 */
    SEL_PROC_CODEC_MODE codecMode

) {
  DRCDEC_SELECTION_PROCESS_RETURN retVal = DRCDEC_SELECTION_PROCESS_NO_ERROR;

  int32_t albumMode = hSelProcInput->albumMode;

  int32_t signalPeakLevelTmp = (int32_t)0;
  int32_t signalPeakLevel = int32_t(0);

  int32_t dmxId = downmixIdRequested;

  int32_t drcSetId = pInst->drcSetId;

  if (drcSetId < 0) {
    drcSetId = 0;
  }

  *explicitPeakInformationPresent = 1;

  if (_truePeakLevelIsPresent(hLoudnessInfoSet, drcSetId, dmxId, albumMode)) {
    retVal = _getTruePeakLevel(hLoudnessInfoSet, drcSetId, dmxId, albumMode,
                               &signalPeakLevel);
    if (retVal) return (retVal);
  } else if (_samplePeakLevelIsPresent(hLoudnessInfoSet, drcSetId, dmxId,
                                       albumMode)) {
    retVal = _getSamplePeakLevel(hLoudnessInfoSet, drcSetId, dmxId, albumMode,
                                 &signalPeakLevel);
    if (retVal) return (retVal);
  } else if (_truePeakLevelIsPresent(hLoudnessInfoSet, 0x3F, dmxId,
                                     albumMode)) {
    retVal = _getTruePeakLevel(hLoudnessInfoSet, 0x3F, dmxId, albumMode,
                               &signalPeakLevel);
    if (retVal) return (retVal);
  } else if (_samplePeakLevelIsPresent(hLoudnessInfoSet, 0x3F, dmxId,
                                       albumMode)) {
    retVal = _getSamplePeakLevel(hLoudnessInfoSet, 0x3F, dmxId, albumMode,
                                 &signalPeakLevel);
    if (retVal) return (retVal);
  } else if (_limiterPeakTargetIsPresent(pInst, drcSetId, dmxId)) {
    retVal = _getLimiterPeakTarget(pInst, drcSetId, dmxId, &signalPeakLevel);
    if (retVal) return (retVal);
  } else if (dmxId != 0) {
    int32_t downmixInstructionIndex = 0;
    int32_t downmixPeakLevelDB = 0;

    *explicitPeakInformationPresent = 0;

    signalPeakLevelTmp = int32_t(0);

    if (_downmixCoefficientsArePresent(hUniDrcConfig, dmxId,
                                       &downmixInstructionIndex)) {
      int32_t dB_m;
      int32_t dB_e;
      int32_t coeff;
      int32_t sum, maxSum; /* e = 7, so it is possible to sum up up to 32
                               downmix coefficients (with e = 2) */
      int32_t i, j;
      DOWNMIX_INSTRUCTIONS* pDown =
          &(hUniDrcConfig->downmixInstructions[downmixInstructionIndex]);
      int32_t downmixOffset = getDownmixOffset(
          pDown, hUniDrcConfig->channelLayout.baseChannelCount); /* e = 1 */
      maxSum = (int32_t)0;

      for (i = 0; i < pDown->targetChannelCount; i++) {
        sum = (int32_t)0;
        for (j = 0; j < hUniDrcConfig->channelLayout.baseChannelCount; j++) {
          coeff = pDown->downmixCoefficient[j + i * hUniDrcConfig->channelLayout
                                                        .baseChannelCount];
          sum += coeff >> 5;
        }
        if (maxSum < sum) maxSum = sum;
      }

      maxSum = fMultDiv2(maxSum, downmixOffset) << 2;

      if (maxSum == FL2FXCONST_DBL(1.0f / (float)(1 << 7))) {
        downmixPeakLevelDB = (int32_t)0;
      } else {
        dB_m = lin2dB(maxSum, 7, &dB_e); /* e_maxSum = 7 */
        downmixPeakLevelDB =
            scaleValue(dB_m, dB_e - 7); /* e_downmixPeakLevelDB = 7 */
      }
    }

    if (_truePeakLevelIsPresent(hLoudnessInfoSet, drcSetId, 0, albumMode)) {
      retVal = _getTruePeakLevel(hLoudnessInfoSet, drcSetId, 0, albumMode,
                                 &signalPeakLevelTmp);
      if (retVal) return (retVal);
    } else if (_samplePeakLevelIsPresent(hLoudnessInfoSet, drcSetId, 0,
                                         albumMode)) {
      retVal = _getSamplePeakLevel(hLoudnessInfoSet, drcSetId, 0, albumMode,
                                   &signalPeakLevelTmp);
      if (retVal) return (retVal);
    } else if (_truePeakLevelIsPresent(hLoudnessInfoSet, 0x3F, 0, albumMode)) {
      retVal = _getTruePeakLevel(hLoudnessInfoSet, 0x3F, 0, albumMode,
                                 &signalPeakLevelTmp);
      if (retVal) return (retVal);
    } else if (_samplePeakLevelIsPresent(hLoudnessInfoSet, 0x3F, 0,
                                         albumMode)) {
      retVal = _getSamplePeakLevel(hLoudnessInfoSet, 0x3F, 0, albumMode,
                                   &signalPeakLevelTmp);
      if (retVal) return (retVal);
    } else if (_limiterPeakTargetIsPresent(pInst, drcSetId, 0)) {
      retVal = _getLimiterPeakTarget(pInst, drcSetId, 0, &signalPeakLevelTmp);
      if (retVal) return (retVal);
    }

    signalPeakLevel = signalPeakLevelTmp + downmixPeakLevelDB;
  } else {
    signalPeakLevel = int32_t(0); /* worst case estimate */
    *explicitPeakInformationPresent = int32_t(0);
  }

  *signalPeakLevelOut = signalPeakLevel;

  return retVal;
}

static DRCDEC_SELECTION_PROCESS_RETURN _extractLoudnessPeakToAverageValue(
    LOUDNESS_INFO* loudnessInfo,
    DYN_RANGE_MEASUREMENT_REQUEST_TYPE dynamicRangeMeasurementType,
    int32_t* pLoudnessPeakToAverageValuePresent,
    int32_t* pLoudnessPeakToAverageValue) {
  int32_t i;

  VALUE_ORDER valueOrderLoudness;
  VALUE_ORDER valueOrderPeakLoudness;

  _initValueOrder(&valueOrderLoudness);
  _initValueOrder(&valueOrderPeakLoudness);

  LOUDNESS_MEASUREMENT* pLoudnessMeasure = NULL;

  *pLoudnessPeakToAverageValuePresent = 0;

  for (i = 0; i < loudnessInfo->measurementCount; i++) {
    pLoudnessMeasure = &(loudnessInfo->loudnessMeasurement[i]);

    if (pLoudnessMeasure->methodDefinition == MD_PROGRAM_LOUDNESS) {
      _getMethodValue(&valueOrderLoudness, pLoudnessMeasure->methodValue,
                      pLoudnessMeasure->measurementSystem, MS_PROGRAMLOUDNESS);
    }

    if ((dynamicRangeMeasurementType == DRMRT_SHORT_TERM_LOUDNESS_TO_AVG) &&
        (pLoudnessMeasure->methodDefinition == MD_SHORT_TERM_LOUDNESS_MAX)) {
      _getMethodValue(&valueOrderPeakLoudness, pLoudnessMeasure->methodValue,
                      pLoudnessMeasure->measurementSystem, MS_PEAKLOUDNESS);
    }

    if ((dynamicRangeMeasurementType == DRMRT_MOMENTARY_LOUDNESS_TO_AVG) &&
        (pLoudnessMeasure->methodDefinition == MD_MOMENTARY_LOUDNESS_MAX)) {
      _getMethodValue(&valueOrderPeakLoudness, pLoudnessMeasure->methodValue,
                      pLoudnessMeasure->measurementSystem, MS_PEAKLOUDNESS);
    }

    if ((dynamicRangeMeasurementType == DRMRT_TOP_OF_LOUDNESS_RANGE_TO_AVG) &&
        (pLoudnessMeasure->methodDefinition == MD_MAX_OF_LOUDNESS_RANGE)) {
      _getMethodValue(&valueOrderPeakLoudness, pLoudnessMeasure->methodValue,
                      pLoudnessMeasure->measurementSystem, MS_PEAKLOUDNESS);
    }
  }

  if ((valueOrderLoudness.order > -1) && (valueOrderPeakLoudness.order > -1)) {
    *pLoudnessPeakToAverageValue =
        valueOrderPeakLoudness.value - valueOrderLoudness.value;
    *pLoudnessPeakToAverageValuePresent = 1;
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

/*******************************************/

static DRCDEC_SELECTION_PROCESS_RETURN _selectAlbumLoudness(
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
    DRCDEC_SELECTION* pCandidatesPotential,
    DRCDEC_SELECTION* pCandidatesSelected) {
  int32_t i, j;

  for (i = 0; i < _drcdec_selection_getNumber(pCandidatesPotential); i++) {
    DRCDEC_SELECTION_DATA* pCandidate =
        _drcdec_selection_getAt(pCandidatesPotential, i);
    if (pCandidate == NULL) return DRCDEC_SELECTION_PROCESS_NOT_OK;

    for (j = 0; j < hLoudnessInfoSet->loudnessInfoAlbumCount; j++) {
      if (pCandidate->pInst->drcSetId ==
          hLoudnessInfoSet->loudnessInfoAlbum[j].drcSetId) {
        if (_drcdec_selection_add(pCandidatesSelected, pCandidate) == NULL)
          return DRCDEC_SELECTION_PROCESS_NOT_OK;
      }
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

/*******************************************/

static int32_t _findMethodDefinition(LOUDNESS_INFO* pLoudnessInfo,
                                 int32_t methodDefinition, int32_t startIndex) {
  int32_t i;
  int32_t index = -1;

  for (i = startIndex; i < pLoudnessInfo->measurementCount; i++) {
    if (pLoudnessInfo->loudnessMeasurement[i].methodDefinition ==
        methodDefinition) {
      index = i;
      break;
    }
  }

  return index;
}

/*******************************************/

static DRCDEC_SELECTION_PROCESS_RETURN _getMixingLevel(
    HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet, int32_t downmixIdRequested,
    int32_t drcSetIdRequested, int32_t albumMode, int32_t* pMixingLevel) {
  const int32_t mixingLevelDefault = FL2FXCONST_DBL(85.0f / (float)(1 << 7));

  int32_t i;
  int32_t count;

  LOUDNESS_INFO* pLoudnessInfo = NULL;

  *pMixingLevel = mixingLevelDefault;

  if (drcSetIdRequested < 0) {
    drcSetIdRequested = 0;
  }

  if (albumMode) {
    count = hLoudnessInfoSet->loudnessInfoAlbumCount;
    pLoudnessInfo = hLoudnessInfoSet->loudnessInfoAlbum;
  } else {
    count = hLoudnessInfoSet->loudnessInfoCount;
    pLoudnessInfo = hLoudnessInfoSet->loudnessInfo;
  }

  for (i = 0; i < count; i++) {
    if ((drcSetIdRequested == pLoudnessInfo[i].drcSetId) &&
        ((downmixIdRequested == pLoudnessInfo[i].downmixId) ||
         (DOWNMIX_ID_ANY_DOWNMIX == pLoudnessInfo[i].downmixId))) {
      int32_t index = _findMethodDefinition(&pLoudnessInfo[i], MD_MIXING_LEVEL, 0);

      if (index >= 0) {
        *pMixingLevel = pLoudnessInfo[i].loudnessMeasurement[index].methodValue;
        break;
      }
    }
  }

  return DRCDEC_SELECTION_PROCESS_NO_ERROR;
}

/*******************************************/
