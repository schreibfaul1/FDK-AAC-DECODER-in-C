

/************************* MPEG-D DRC decoder library **************************

   Author(s):   Andreas Hoelzer

   Description: DRC Set Selection

*******************************************************************************/

#ifndef DRCDEC_SELECTIONPROCESS_H
#define DRCDEC_SELECTIONPROCESS_H

#include <stdint.h>
#include "../libAACdec/newAACDecoder.h"
#include "drcDec_types.h"
#include "drcDecoder.h"

/* DRC set selection according to section 6.2 of ISO/IEC 23003-4 (MPEG-D DRC) */
/* including ISO/IEC 23003-4/AMD1 (Amendment 1) */

typedef struct s_drcdec_selection_process* HANDLE_DRC_SELECTION_PROCESS;

#define UNDEFINED_LOUDNESS_VALUE (int32_t)(MAXVAL_DBL - 1)

typedef enum {
  DRCDEC_SELECTION_PROCESS_NO_ERROR = 0,

  DRCDEC_SELECTION_PROCESS_WARNING = -1000,

  DRCDEC_SELECTION_PROCESS_NOT_OK = -2000,
  DRCDEC_SELECTION_PROCESS_OUTOFMEMORY,
  DRCDEC_SELECTION_PROCESS_INVALID_HANDLE,
  DRCDEC_SELECTION_PROCESS_NOT_SUPPORTED,
  DRCDEC_SELECTION_PROCESS_INVALID_PARAM,
  DRCDEC_SELECTION_PROCESS_PARAM_OUT_OF_RANGE

} DRCDEC_SELECTION_PROCESS_RETURN;

typedef enum {
  SEL_PROC_TEST_TIME_DOMAIN = -100,
  SEL_PROC_TEST_QMF_DOMAIN,
  SEL_PROC_TEST_STFT_DOMAIN,

  SEL_PROC_CODEC_MODE_UNDEFINED = -1,
  SEL_PROC_MPEG_4_AAC,
  SEL_PROC_MPEG_D_USAC,
  SEL_PROC_MPEG_H_3DA
} SEL_PROC_CODEC_MODE;

typedef enum {
  /* set and get user param */
  SEL_PROC_LOUDNESS_NORMALIZATION_ON,
  /* get only user param */
  SEL_PROC_DYNAMIC_RANGE_CONTROL_ON,
  /* set only user params */
  SEL_PROC_TARGET_LOUDNESS,
  SEL_PROC_EFFECT_TYPE,
  SEL_PROC_EFFECT_TYPE_FALLBACK_CODE,
  SEL_PROC_LOUDNESS_MEASUREMENT_METHOD,
  SEL_PROC_ALBUM_MODE,
  SEL_PROC_DOWNMIX_ID,
  SEL_PROC_TARGET_LAYOUT,
  SEL_PROC_TARGET_CHANNEL_COUNT,
  SEL_PROC_BASE_CHANNEL_COUNT,
  SEL_PROC_SAMPLE_RATE,
  SEL_PROC_BOOST,
  SEL_PROC_COMPRESS
} SEL_PROC_USER_PARAM;

typedef struct s_selection_process_output {
  int32_t outputPeakLevelDb;           /* e = 7 */
  int32_t loudnessNormalizationGainDb; /* e = 7 */
  int32_t outputLoudness;              /* e = 7 */

  uint8_t numSelectedDrcSets;
  int8_t selectedDrcSetIds[MAX_ACTIVE_DRCS];
  uint8_t selectedDownmixIds[MAX_ACTIVE_DRCS];

  uint8_t activeDownmixId;
  uint8_t baseChannelCount;
  uint8_t targetChannelCount;
  int8_t targetLayout;
  uint8_t downmixMatrixPresent;
  int32_t downmixMatrix[8][8]; /* e = 2 */

  int16_t boost;    /* e = 1 */
  int16_t compress; /* e = 1 */

  int32_t mixingLevel; /* e = 7 */

} SEL_PROC_OUTPUT, *HANDLE_SEL_PROC_OUTPUT;

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_Create(HANDLE_DRC_SELECTION_PROCESS* phInstance);

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_Delete(HANDLE_DRC_SELECTION_PROCESS* phInstance);

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_Init(HANDLE_DRC_SELECTION_PROCESS hInstance);

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_SetCodecMode(HANDLE_DRC_SELECTION_PROCESS hInstance,
                                     const SEL_PROC_CODEC_MODE codecMode);

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_SetParam(HANDLE_DRC_SELECTION_PROCESS hInstance,
                                 const SEL_PROC_USER_PARAM requestType,
                                 int32_t requestValue, int32_t* pDiff);

int32_t
drcDec_SelectionProcess_GetParam(HANDLE_DRC_SELECTION_PROCESS hInstance,
                                 const SEL_PROC_USER_PARAM requestType);

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_SetMpeghParams(
    HANDLE_DRC_SELECTION_PROCESS hInstance, const int32_t numGroupIdsRequested,
    const int32_t* groupIdRequested, const int32_t numGroupPresetIdsRequested,
    const int32_t* groupPresetIdRequested,
    const int32_t* numMembersGroupPresetIdsRequested,
    const int32_t groupPresetIdRequestedPreference, int32_t* pDiff);

DRCDEC_SELECTION_PROCESS_RETURN
drcDec_SelectionProcess_Process(HANDLE_DRC_SELECTION_PROCESS hInstance,
                                HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
                                HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
                                HANDLE_SEL_PROC_OUTPUT hSelProcOutput);

#endif
