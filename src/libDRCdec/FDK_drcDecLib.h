

/************************* MPEG-D DRC decoder library **************************

   Author(s):   Bernhard Neugebauer

   Description: MPEG-D DRC Decoder

*******************************************************************************/

#ifndef FDK_DRCDECLIB_H
#define FDK_DRCDECLIB_H
#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/FDK_bitstream.h"
#include "../libFDK/common_fix.h"

/* DRC decoder according to ISO/IEC 23003-4 (MPEG-D DRC) */
/* including ISO/IEC 23003-4/AMD1 (Amendment 1) */

#ifdef __cplusplus
extern "C" {
#endif

#define DRC_DEC_LOUDNESS_NOT_PRESENT (int32_t)0x7FFFFFFE

typedef struct s_drc_decoder* HANDLE_DRC_DECODER;
typedef struct s_uni_drc_interface* HANDLE_UNI_DRC_INTERFACE;
typedef struct s_selection_process_output* HANDLE_SEL_PROC_OUTPUT;

typedef enum {
  DRC_DEC_SELECTION = 0x1, /* DRC decoder instance for DRC set selection only */
  DRC_DEC_GAIN = 0x2,      /* DRC decoder instance for applying DRC only */
  DRC_DEC_ALL = 0x3        /* DRC decoder with full functionality */
} DRC_DEC_FUNCTIONAL_RANGE;

typedef enum {
  /* get and set userparams */
  DRC_DEC_BOOST,
  DRC_DEC_COMPRESS,
  /* set only userparams */
  DRC_DEC_LOUDNESS_NORMALIZATION_ON,
  DRC_DEC_TARGET_LOUDNESS, /**< target loudness in dB, with exponent e = 7 */
  DRC_DEC_EFFECT_TYPE,
  DRC_DEC_EFFECT_TYPE_FALLBACK_CODE,
  DRC_DEC_LOUDNESS_MEASUREMENT_METHOD,
  DRC_DEC_ALBUM_MODE,
  /* set only system (not user) parameters */
  DRC_DEC_DOWNMIX_ID,
  DRC_DEC_TARGET_CHANNEL_COUNT_REQUESTED, /**< number of output channels
                                             notified to FDK_drcDecLib for
                                             choosing an appropriate
                                             downmixInstruction */
  DRC_DEC_BASE_CHANNEL_COUNT,
  DRC_DEC_FRAME_SIZE,
  DRC_DEC_SAMPLE_RATE,
  /* get only system parameters */
  DRC_DEC_IS_MULTIBAND_DRC_1,
  DRC_DEC_IS_MULTIBAND_DRC_2,
  DRC_DEC_IS_ACTIVE, /**< MPEG-D DRC payload is present and at least one of
                        Dynamic Range Control (DRC) or Loudness Normalization
                        (LN) is activated */
  DRC_DEC_TARGET_CHANNEL_COUNT_SELECTED, /**< number of output channels if
                                            appropriate downmixInstruction
                                            exists */
  DRC_DEC_OUTPUT_LOUDNESS /**< output loudness in dB, with exponent e = 7, or
                             DRC_DEC_LOUDNESS_NOT_PRESENT if no loudness is
                             contained in the bitstream */
} DRC_DEC_USERPARAM;

typedef enum {
  DRC_DEC_OK = 0,

  DRC_DEC_NOT_OK = -10000,
  DRC_DEC_OUT_OF_MEMORY,
  DRC_DEC_NOT_OPENED,
  DRC_DEC_NOT_READY,
  DRC_DEC_PARAM_OUT_OF_RANGE,
  DRC_DEC_INVALID_PARAM,
  DRC_DEC_UNSUPPORTED_FUNCTION
} DRC_DEC_ERROR;

typedef enum {
  DRC_DEC_TEST_TIME_DOMAIN = -100,
  DRC_DEC_TEST_QMF_DOMAIN,
  DRC_DEC_TEST_STFT_DOMAIN,
  DRC_DEC_CODEC_MODE_UNDEFINED = -1,
  DRC_DEC_MPEG_4_AAC,
  DRC_DEC_MPEG_D_USAC,
  DRC_DEC_MPEG_H_3DA
} DRC_DEC_CODEC_MODE;

/* Apply only DRC sets dedicated to processing location.
   DRC1: before downmix
   DRC2: before or after downmix (AMD1: only after downmix)
   DRC3: after downmix */
typedef enum {
  DRC_DEC_DRC1,
  DRC_DEC_DRC1_DRC2,
  DRC_DEC_DRC2,
  DRC_DEC_DRC3,
  DRC_DEC_DRC2_DRC3
} DRC_DEC_LOCATION;

DRC_DEC_ERROR
FDK_drcDec_Open(HANDLE_DRC_DECODER* phDrcDec,
                const DRC_DEC_FUNCTIONAL_RANGE functionalRange);

DRC_DEC_ERROR
FDK_drcDec_SetCodecMode(HANDLE_DRC_DECODER hDrcDec,
                        const DRC_DEC_CODEC_MODE codecMode);

DRC_DEC_ERROR
FDK_drcDec_Init(HANDLE_DRC_DECODER hDrcDec, const int32_t frameSize,
                const int32_t sampleRate, const int32_t baseChannelCount);

DRC_DEC_ERROR
FDK_drcDec_Close(HANDLE_DRC_DECODER* phDrcDec);

/* set single user request */
DRC_DEC_ERROR
FDK_drcDec_SetParam(HANDLE_DRC_DECODER hDrcDec,
                    const DRC_DEC_USERPARAM requestType,
                    const int32_t requestValue);

int32_t FDK_drcDec_GetParam(HANDLE_DRC_DECODER hDrcDec,
                         const DRC_DEC_USERPARAM requestType);

DRC_DEC_ERROR
FDK_drcDec_SetInterfaceParameters(HANDLE_DRC_DECODER hDrcDec,
                                  HANDLE_UNI_DRC_INTERFACE uniDrcInterface);

DRC_DEC_ERROR
FDK_drcDec_SetSelectionProcessMpeghParameters_simple(
    HANDLE_DRC_DECODER hDrcDec, const int32_t groupPresetIdRequested,
    const int32_t numGroupIdsRequested, const int32_t* groupIdsRequested);

DRC_DEC_ERROR
FDK_drcDec_SetDownmixInstructions(HANDLE_DRC_DECODER hDrcDec,
                                  const int32_t numDowmixId, const int32_t* downmixId,
                                  const int32_t* targetLayout,
                                  const int32_t* targetChannelCount);

void FDK_drcDec_SetSelectionProcessOutput(
    HANDLE_DRC_DECODER hDrcDec, HANDLE_SEL_PROC_OUTPUT hSelProcOutput);

HANDLE_SEL_PROC_OUTPUT
FDK_drcDec_GetSelectionProcessOutput(HANDLE_DRC_DECODER hDrcDec);

int32_t /* int32_t, e = 7 */
FDK_drcDec_GetGroupLoudness(HANDLE_SEL_PROC_OUTPUT hSelProcOutput,
                            const int32_t groupID, int32_t* groupLoudnessAvailable);

void FDK_drcDec_SetChannelGains(HANDLE_DRC_DECODER hDrcDec,
                                const int32_t numChannels, const int32_t frameSize,
                                int32_t* channelGainDb, int32_t* audioBuffer,
                                const int32_t audioBufferChannelOffset);

DRC_DEC_ERROR
FDK_drcDec_ReadUniDrcConfig(HANDLE_DRC_DECODER hDrcDec,
                            HANDLE_FDK_BITSTREAM hBitstream);

DRC_DEC_ERROR
FDK_drcDec_ReadLoudnessInfoSet(HANDLE_DRC_DECODER hDrcDec,
                               HANDLE_FDK_BITSTREAM hBitstream);

DRC_DEC_ERROR
FDK_drcDec_ReadLoudnessBox(HANDLE_DRC_DECODER hDrcDec,
                           HANDLE_FDK_BITSTREAM hBitstream);

DRC_DEC_ERROR
FDK_drcDec_ReadDownmixInstructions_Box(HANDLE_DRC_DECODER hDrcDec,
                                       HANDLE_FDK_BITSTREAM hBitstream);

DRC_DEC_ERROR
FDK_drcDec_ReadUniDrcInstructions_Box(HANDLE_DRC_DECODER hDrcDec,
                                      HANDLE_FDK_BITSTREAM hBitstream);

DRC_DEC_ERROR
FDK_drcDec_ReadUniDrcCoefficients_Box(HANDLE_DRC_DECODER hDrcDec,
                                      HANDLE_FDK_BITSTREAM hBitstream);

DRC_DEC_ERROR
FDK_drcDec_ReadUniDrcGain(HANDLE_DRC_DECODER hDrcDec,
                          HANDLE_FDK_BITSTREAM hBitstream);

/* either call FDK_drcDec_ReadUniDrcConfig, FDK_drcDec_ReadLoudnessInfoSet and
   FDK_drcDec_ReadUniDrcGain separately, or call FDK_drcDec_ReadUniDrc */
DRC_DEC_ERROR
FDK_drcDec_ReadUniDrc(HANDLE_DRC_DECODER hDrcDec,
                      HANDLE_FDK_BITSTREAM hBitstream);

/* calling sequence:
   FDK_drcDec_Read...()
   FDK_drcDec_SetChannelGains()
   FDK_drcDec_Preprocess()
   FDK_drcDec_Process...() */

DRC_DEC_ERROR
FDK_drcDec_Preprocess(HANDLE_DRC_DECODER hDrcDec);

DRC_DEC_ERROR
FDK_drcDec_ProcessTime(HANDLE_DRC_DECODER hDrcDec, const int32_t delaySamples,
                       const DRC_DEC_LOCATION drcLocation,
                       const int32_t channelOffset, const int32_t drcChannelOffset,
                       const int32_t numChannelsProcessed, int32_t* realBuffer,
                       const int32_t timeDataChannelOffset);

DRC_DEC_ERROR
FDK_drcDec_ProcessFreq(HANDLE_DRC_DECODER hDrcDec, const int32_t delaySamples,
                       const DRC_DEC_LOCATION drcLocation,
                       const int32_t channelOffset, const int32_t drcChannelOffset,
                       const int32_t numChannelsProcessed,
                       const int32_t processSingleTimeslot, int32_t** realBuffer,
                       int32_t** imagBuffer);

DRC_DEC_ERROR
FDK_drcDec_ApplyDownmix(HANDLE_DRC_DECODER hDrcDec, int32_t* reverseInChannelMap,
                        int32_t* reverseOutChannelMap, int32_t* realBuffer,
                        int32_t* pNChannels);

/* Get library info for this module. */
DRC_DEC_ERROR
FDK_drcDec_GetLibInfo(LIB_INFO_t* info);

#ifdef __cplusplus
}
#endif
#endif
