

/**************************** AAC decoder library ******************************

   Author(s):   Christian Griebel

   Description: Dynamic range control (DRC) decoder tool for AAC

*******************************************************************************/

#ifndef AACDEC_DRC_H
#define AACDEC_DRC_H
#include "newAACDecoder.h"
#include "../libMpegTPDec/tp_data.h"

#include "aacdec_drc_types.h"
#include "channel.h"
#include "../libFDK/FDK_bitstream.h"

#define AACDEC_DRC_GAIN_SCALING (11) /* Scaling of DRC gains */
#define AACDEC_DRC_GAIN_INIT_VALUE \
  (FL2FXCONST_DBL(                 \
      1.0f / (1 << AACDEC_DRC_GAIN_SCALING))) /* Init value for DRC gains */

#define AACDEC_DRC_DFLT_EXPIRY_FRAMES \
  (0) /* Default DRC data expiry time in AAC frames   */

/* #define AACDEC_DRC_IGNORE_FRAMES_WITH_MULTIPLE_CH_THREADS */ /* The name says
                                                                   it all. */
/* #define AACDEC_DRC_DEBUG */

/**
 * \brief DRC module setting parameters
 */
typedef enum {
  DRC_CUT_SCALE = 0,
  DRC_BOOST_SCALE,
  TARGET_REF_LEVEL,
  DRC_BS_DELAY,
  DRC_DATA_EXPIRY_FRAME,
  APPLY_HEAVY_COMPRESSION,
  DEFAULT_PRESENTATION_MODE,
  ENCODER_TARGET_LEVEL,
  MAX_OUTPUT_CHANNELS,
  UNIDRC_PRECEDENCE
} AACDEC_DRC_PARAM;

/**
 * \brief DRC module interface functions
 */
void aacDecoder_drcReset(HANDLE_AAC_DRC self);

void aacDecoder_drcInit(HANDLE_AAC_DRC self);

void aacDecoder_drcInitChannelData(CDrcChannelData_t *pDrcChannel);

AAC_DECODER_ERROR aacDecoder_drcSetParam(HANDLE_AAC_DRC self,
                                         AACDEC_DRC_PARAM param, int32_t value);

int32_t aacDecoder_drcMarkPayload(HANDLE_AAC_DRC self, HANDLE_FDK_BITSTREAM hBs,
                              AACDEC_DRC_PAYLOAD_TYPE type);

int32_t aacDecoder_drcProlog(
    HANDLE_AAC_DRC self, HANDLE_FDK_BITSTREAM hBs,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo[],
    uint8_t pceInstanceTag, uint8_t channelMapping[], int32_t validChannels);

/**
 * \brief Apply DRC. If SBR is present, DRC data is handed over to the SBR
 * decoder.
 * \param self AAC decoder instance
 * \param pSbrDec pointer to SBR decoder instance
 * \param pAacDecoderChannelInfo AAC decoder channel instance to be processed
 * \param pDrcDat DRC channel data
 * \param extGain Pointer to a int32_t where a externally applyable gain will
 * be stored into (independently on whether it will be apply internally or not).
 *                At function call the buffer must hold the scale (0 >= scale <
 * DFRACT_BITS) to be applied on the gain value.
 * \param ch channel index
 * \param aacFrameSize AAC frame size
 * \param bSbrPresent flag indicating that SBR is present, in which case DRC is
 * handed over to the SBR instance pSbrDec
 */
void aacDecoder_drcApply(HANDLE_AAC_DRC self, void *pSbrDec,
                         CAacDecoderChannelInfo *pAacDecoderChannelInfo,
                         CDrcChannelData_t *pDrcDat, int32_t *extGain, int32_t ch,
                         int32_t aacFrameSize, int32_t bSbrPresent);

int32_t aacDecoder_drcEpilog(
    HANDLE_AAC_DRC self, HANDLE_FDK_BITSTREAM hBs,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo[],
    uint8_t pceInstanceTag, uint8_t channelMapping[], int32_t validChannels);

/**
 * \brief Get metadata information found in bitstream.
 * \param self DRC module instance handle.
 * \param pPresMode Pointer to field where the presentation mode will be written
 * to.
 * \param pProgRefLevel Pointer to field where the program reference level will
 * be written to.
 * \return Nothing.
 */
void aacDecoder_drcGetInfo(HANDLE_AAC_DRC self, int8_t *pPresMode,
                           int8_t *pProgRefLevel);

/**
 * \brief  Apply DRC Level Normalization.
 *
 *         This function prepares/applies the gain values for the DRC Level
 * Normalization and returns the exponent of the time data. The following two
 * cases are handled:
 *
 *         - Limiter enabled:
 *           The input data must be interleaved.
 *           One gain per sample is written to the buffer pGainPerSample.
 *           If necessary the time data is rescaled.
 *
 *         - Limiter disabled:
 *           The input data can be interleaved or deinterleaved.
 *           The gain values are applied to the time data.
 *           If necessary the time data is rescaled.
 *
 * \param hDrcInfo                     [i/o] handle to drc data structure.
 * \param samplesIn                    [i/o] pointer to time data.
 * \param pGain                        [i  ] pointer to gain to be applied to
 * the time data.
 * \param pGainPerSample               [o  ] pointer to the gain per sample to
 * be applied to the time data in the limiter.
 * \param gain_scale                   [i  ] exponent to be applied to the time
 * data.
 * \param gain_delay                   [i  ] delay[samples] with which the gains
 * in pGain shall be applied (gain_delay <= nSamples).
 * \param nSamples                     [i  ] number of samples per frame.
 * \param channels                     [i  ] number of channels.
 * \param stride                       [i  ] channel stride of time data.
 * \param limiterEnabled               [i  ] 1 if limiter is enabled, otherwise
 * 0.
 *
 * \return exponent of time data
 */
int32_t applyDrcLevelNormalization(HANDLE_AAC_DRC hDrcInfo, int32_t *samplesIn,
                               int32_t *pGain, int32_t *pGainPerSample,
                               const int32_t gain_scale, const uint32_t gain_delay,
                               const uint32_t nSamples, const uint32_t channels,
                               const uint32_t stride, const uint32_t limiterEnabled);

#endif /* AACDEC_DRC_H */
