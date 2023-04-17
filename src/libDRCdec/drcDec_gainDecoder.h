

/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#ifndef DRCDEC_GAINDECODER_H
#define DRCDEC_GAINDECODER_H

#include <stdint.h>
#include "../libAACdec/newAACDecoder.h"

#include "drcDecoder.h"

/* Definitions common to gainDecoder submodule */

#define NUM_LNB_FRAMES                                                         \
  5 /* previous frame + this frame + one frame for DM_REGULAR_DELAY + (maximum \
       delaySamples)/frameSize */

/* QMF64 */
#define SUBBAND_NUM_BANDS_QMF64 64
#define SUBBAND_DOWNSAMPLING_FACTOR_QMF64 64
#define SUBBAND_ANALYSIS_DELAY_QMF64 320

/* QMF71 (according to ISO/IEC 23003-1:2007) */
#define SUBBAND_NUM_BANDS_QMF71 71
#define SUBBAND_DOWNSAMPLING_FACTOR_QMF71 64
#define SUBBAND_ANALYSIS_DELAY_QMF71 320 + 384

/* STFT256 (according to ISO/IEC 23008-3:2015/AMD3) */
#define SUBBAND_NUM_BANDS_STFT256 256
#define SUBBAND_DOWNSAMPLING_FACTOR_STFT256 256
#define SUBBAND_ANALYSIS_DELAY_STFT256 256

typedef enum {
  GAIN_DEC_DRC1,
  GAIN_DEC_DRC1_DRC2,
  GAIN_DEC_DRC2,
  GAIN_DEC_DRC3,
  GAIN_DEC_DRC2_DRC3
} GAIN_DEC_LOCATION;

typedef enum { GAIN_DEC_FRAME_SIZE, GAIN_DEC_SAMPLE_RATE } GAIN_DEC_PARAM;

typedef struct {
  int32_t gainLin; /* e = 7 */
  int16_t time;
} NODE_LIN;

typedef struct {
  GAIN_INTERPOLATION_TYPE gainInterpolationType;
  int32_t nNodes[NUM_LNB_FRAMES]; /* number of nodes, saturated to 16 */
  NODE_LIN linearNode[NUM_LNB_FRAMES][16];
} LINEAR_NODE_BUFFER;

typedef struct {
  int32_t lnbPointer;
  LINEAR_NODE_BUFFER linearNodeBuffer[12];
  LINEAR_NODE_BUFFER dummyLnb;
  int32_t channelGain[8][NUM_LNB_FRAMES]; /* e = 8 */
} DRC_GAIN_BUFFERS;

typedef struct {
  int32_t activeDrcOffset;
  DRC_INSTRUCTIONS_UNI_DRC* pInst;
  DRC_COEFFICIENTS_UNI_DRC* pCoef;

  DUCKING_MODIFICATION duckingModificationForChannelGroup[8];
  int8_t channelGroupForChannel[8];

  uint8_t bandCountForChannelGroup[8];
  uint8_t gainElementForGroup[8];
  uint8_t channelGroupIsParametricDrc[8];
  uint8_t gainElementCount; /* number of different DRC gains inluding all DRC
                             bands */
  int32_t lnbIndexForChannel[8][NUM_LNB_FRAMES];
  int32_t subbandGainsReady;
} ACTIVE_DRC;

typedef struct {
  int32_t deltaTminDefault;
  int32_t frameSize;
  int32_t loudnessNormalisationGainDb;
  DELAY_MODE delayMode;

  int32_t nActiveDrcs;
  ACTIVE_DRC activeDrc[MAX_ACTIVE_DRCS];
  int32_t multiBandActiveDrcIndex;
  int32_t channelGainActiveDrcIndex;
  int32_t channelGain[8]; /* e = 8 */

  DRC_GAIN_BUFFERS drcGainBuffers;
  int32_t subbandGains[12][4 * 1024 / 256];
  int32_t dummySubbandGains[4 * 1024 / 256];

  int32_t status;
  int32_t timeDomainSupported;
  SUBBAND_DOMAIN_MODE subbandDomainSupported;
} DRC_GAIN_DECODER, *HANDLE_DRC_GAIN_DECODER;

/* init functions */
DRC_ERROR
drcDec_GainDecoder_Open(HANDLE_DRC_GAIN_DECODER* phGainDec);

DRC_ERROR
drcDec_GainDecoder_Init(HANDLE_DRC_GAIN_DECODER hGainDec);

DRC_ERROR
drcDec_GainDecoder_SetParam(HANDLE_DRC_GAIN_DECODER hGainDec,
                            const GAIN_DEC_PARAM paramType,
                            const int32_t paramValue);

DRC_ERROR
drcDec_GainDecoder_SetCodecDependentParameters(
    HANDLE_DRC_GAIN_DECODER hGainDec, const DELAY_MODE delayMode,
    const int32_t timeDomainSupported,
    const SUBBAND_DOMAIN_MODE subbandDomainSupported);

DRC_ERROR
drcDec_GainDecoder_Config(HANDLE_DRC_GAIN_DECODER hGainDec,
                          HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
                          const uint8_t numSelectedDrcSets,
                          const int8_t* selectedDrcSetIds,
                          const uint8_t* selectedDownmixIds);

/* close functions */
DRC_ERROR
drcDec_GainDecoder_Close(HANDLE_DRC_GAIN_DECODER* phGainDec);

/* process functions */

/* call drcDec_GainDecoder_Preprocess first */
DRC_ERROR
drcDec_GainDecoder_Preprocess(HANDLE_DRC_GAIN_DECODER hGainDec,
                              HANDLE_UNI_DRC_GAIN hUniDrcGain,
                              const int32_t loudnessNormalizationGainDb,
                              const int16_t boost, const int16_t compress);

/* Then call one of drcDec_GainDecoder_ProcessTimeDomain or
 * drcDec_GainDecoder_ProcessSubbandDomain */
DRC_ERROR
drcDec_GainDecoder_ProcessTimeDomain(
    HANDLE_DRC_GAIN_DECODER hGainDec, const int32_t delaySamples,
    const GAIN_DEC_LOCATION drcLocation, const int32_t channelOffset,
    const int32_t drcChannelOffset, const int32_t numChannelsProcessed,
    const int32_t timeDataChannelOffset, int32_t* audioIOBuffer);

DRC_ERROR
drcDec_GainDecoder_ProcessSubbandDomain(
    HANDLE_DRC_GAIN_DECODER hGainDec, const int32_t delaySamples,
    GAIN_DEC_LOCATION drcLocation, const int32_t channelOffset,
    const int32_t drcChannelOffset, const int32_t numChannelsProcessed,
    const int32_t processSingleTimeslot, int32_t* audioIOBufferReal[],
    int32_t* audioIOBufferImag[]);

DRC_ERROR
drcDec_GainDecoder_Conceal(HANDLE_DRC_GAIN_DECODER hGainDec,
                           HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
                           HANDLE_UNI_DRC_GAIN hUniDrcGain);

DRC_ERROR
drcDec_GainDecoder_SetLoudnessNormalizationGainDb(
    HANDLE_DRC_GAIN_DECODER hGainDec, int32_t loudnessNormalizationGainDb);

int32_t drcDec_GainDecoder_GetFrameSize(HANDLE_DRC_GAIN_DECODER hGainDec);

int32_t drcDec_GainDecoder_GetDeltaTminDefault(HANDLE_DRC_GAIN_DECODER hGainDec);

void drcDec_GainDecoder_SetChannelGains(HANDLE_DRC_GAIN_DECODER hGainDec,
                                        const int32_t numChannels,
                                        const int32_t frameSize,
                                        const int32_t* channelGainDb,
                                        const int32_t audioBufferChannelOffset,
                                        int32_t* audioBuffer);

#endif
