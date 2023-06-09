

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: Space Decoder

*******************************************************************************/

#ifndef SAC_DEC_LIB_H
#define SAC_DEC_LIB_H

#include "../libFDK/common_fix.h"
#include "../libAACdec/newAACDecoder.h"
#include "sac_dec_errorcodes.h"
#include "../libFDK/FDK_bitstream.h"
#include "../libFDK/FDK_qmf_domain.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief  MPEG Surround input data interface mode.
 **/
typedef enum {
  SAC_INTERFACE_QMF =
      0, /*!< Use QMF domain interface for the input downmix audio.  */
  SAC_INTERFACE_TIME, /*!< Use time domain interface for the input downmix
                         audio. */
  SAC_INTERFACE_AUTO  /*!<          */
} SAC_INPUT_CONFIG;

/**
 * \brief  MPEG Surround output mode.
 **/
typedef enum {
  SACDEC_OUT_MODE_NORMAL =
      0, /*!< Normal multi channel processing without output restrictions. */
  SACDEC_OUT_MODE_BINAURAL, /*!< Two channel output with binaural processsing.
                             */
  SACDEC_OUT_MODE_STEREO,   /*!< Always two channel output mode.   */
  SACDEC_OUT_MODE_6CHANNEL  /*!< Always process with 5.1 channel output.  */
} SAC_DEC_OUTPUT_MODE;

/**
 * \brief  MPEG Surround binaural HRTF model.
 *         HRTF will be applied only in combination with upmixtype
 *SAC_UPMIX_TYPE_BINAURAL.
 **/
typedef enum {
  SAC_BINAURAL_HRTF_KEMAR = 0,
  SAC_BINAURAL_HRTF_VAST,
  SAC_BINAURAL_HRTF_MPSVT,
  SAC_BINAURAL_SINGLE_HRTFS
} SAC_BINAURAL_HRTF_MODEL;

/**
 * \brief  MPEG Surround decoder instance available.
 **/
typedef enum {
  SAC_INSTANCE_NOT_FULL_AVAILABLE =
      0, /*!< MPEG Surround decoder instance not full available. */
  SAC_INSTANCE_FULL_AVAILABLE /*!< MPEG Surround decoder instance full
                                 available. */
} SAC_INSTANCE_AVAIL;

/**
 * \brief  MPEG Surround decoder dynamic parameters.
 *
 * Use mpegSurroundDecoder_SetParam() function to configure internal status of
 * following parameters.
 */
typedef enum {
  SACDEC_OUTPUT_MODE = 0x0001, /*!< Set MPEG Surround decoder output mode. See
                                  SAC_DEC_OUTPUT_MODE. */
  SACDEC_BLIND_ENABLE =
      0x0002, /*!< Multi channel output without MPEG Surround side info.    */
  SACDEC_PARTIALLY_COMPLEX =
      0x0003, /*!< Set partially complex flag for MPEG Surround.
                    0: Use complex valued QMF data.
                    1: Use real valued QMF data (low power mode) */
  SACDEC_INTERFACE =
      0x0004, /*!< Select signal input interface for MPEG Surround.
                     Switch time interface off:  0
                     Switch time interface on:   1 */
  SACDEC_BS_DELAY = 0x0005, /*!< Select bit stream delay for MPEG Surround.
                                   Switch bit stream delay off:  0
                                   Switch bit stream delay on:   1 */
  SACDEC_BINAURAL_QUALITY =
      0x0102, /*!< Set binaural quality for MPEG Surround binaural mode.
                   0: Low Complexity,
                   1: High Quality */
  SACDEC_BINAURAL_DISTANCE = 0x0103, /*!< Set perceived distance for binaural
                                        playback (binaural mode only). The valid
                                        values  range from 0 to 100. Where 100
                                        corresponds to the farthest perceived
                                        distance. */
  SACDEC_BINAURAL_DIALOG_CLARITY =
      0x0104, /*!< Set dialog clarity (for binaural playback).
                   The valid values range from 0 to 100. */
  SACDEC_BINAURAL_FRONT_ANGLE = 0x0105, /*!< Set angle between the virtual front
                                           speaker pair (binaural mode only).
                                             The valid range is from 0 to 180
                                           angular degrees. */
  SACDEC_BINAURAL_BACK_ANGLE = 0x0106,  /*!< Set angle between the virtual back
                                           speaker pair (binaural mode only).  The
                                           valid range is from 0 to 180 angular
                                           degrees. */
  SACDEC_BINAURAL_PRESET = 0x0107, /*!< Set a virtual speaker setup preset for
                                      binaural playback (binaural mode only).
                                      This meta-parameter implicitly modifies
                                      the following parameters:
                                          SACDEC_BINAURAL_DISTANCE,
                                          SACDEC_BINAURAL_DIALOG_CLARITY,
                                          SACDEC_BINAURAL_FRONT_ANGLE and
                                          SACDEC_BINAURAL_BACK_ANGLE.
                                        The following presets are available:
                                          1: Dry room
                                          2: Living room (default)
                                          3: Cinema */

  SACDEC_BS_INTERRUPTION =
      0x0200, /*!< If the given value is unequal to 0 hint the MPEG Surround
                 decoder that the next input data is discontinuous, because of
                 frame loss, seeking, etc. Announce the decoder that the
                 bitstream data was interrupted (fSync = 0). This will cause the
                 surround decoder not to parse any new bitstream data until a
                 new header with a valid Spatial Specific Config and a
                 independently decodable frame is found. Specially important
                 when the MPEG Surround data is split accross several frames
                 (for example in the case of AAC-LC downmix with 1024
                 framelength and 2048 surround frame length) and a discontinuity
                 in the bitstream data occurs. If fSync is 1, assume that MPEG
                 Surround data is in sync (out of band config for example). */
  SACDEC_CLEAR_HISTORY = 0x0201, /*!< If the given value is unequal to 0 clear
                                    all internal states (delay lines, QMF
                                    states, ...) of the MPEG Surround decoder.
                                    This will cause a discontinuity in the audio
                                    output signal. */

  SACDEC_CONCEAL_NUM_KEEP_FRAMES =
      0x0301, /*!< Error concealment: The Number of frames the module keeps the
                 last spatial image before fading to the particular spatial
                 scenario starts. The default is 10 frames. */
  SACDEC_CONCEAL_FADE_OUT_SLOPE_LENGTH =
      0x0302, /*!< Error concealment: Length of the slope (in frames) the module
                 creates to fade from the last spatial scenario to the
                 particular default scenario (downmix) in case of consecutive
                 errors. Default is 5. */
  SACDEC_CONCEAL_FADE_IN_SLOPE_LENGTH =
      0x0303, /*!< Error concealment: Length of the slope (in frames) the module
                 creates to fade from the default spatial scenario (downmix) to
                 the current scenario after fade-out. Default parameter value
                 is 5. */
  SACDEC_CONCEAL_NUM_RELEASE_FRAMES =
      0x0304 /*!< Error concealment: The number of error free frames before the
                module starts fading from default to the current spatial
                scenario. Default parameter value is 3 frames. */
} SACDEC_PARAM;

#define PCM_MPS int32_t

/**
 * \brief MPEG Surround decoder handle.
 */
typedef struct MpegSurroundDecoder CMpegSurroundDecoder;

/**
 * \brief  Check if the full MPEG Surround decoder instance is allocated.
 *
 * Check if the full MPEG Surround decoder instance is allocated.
 *
 * \param pMpegSurroundDecoder   A pointer to a decoder stucture.
 *
 * \return SACDEC_ERROR error code
 */
SAC_INSTANCE_AVAIL
mpegSurroundDecoder_IsFullMpegSurroundDecoderInstanceAvailable(
    CMpegSurroundDecoder *pMpegSurroundDecoder);

/**
 * \brief  Open one instance of the MPEG Surround decoder.
 *
 * Allocate one instance of decoder and input buffers.
 * - Allocate decoder structure
 * - Allocate input buffers (QMF/time/MPS data)
 *
 * \param pMpegSurroundDecoder   A pointer to a decoder handle; filled on
 * return.
 * \param splitMemoryAllocation  Allocate only outer layer of MPS decoder. Core
 * part is reallocated later if needed.
 * \param stereoConfigIndex      USAC: Save memory by opening the MPS decoder
 * for a specific stereoConfigIndex. (Needs optimization macros enabled.)
 * \param pQmfDomain             Pointer to QMF domain data structure.
 *
 * \return SACDEC_ERROR error code
 */
SACDEC_ERROR mpegSurroundDecoder_Open(
    CMpegSurroundDecoder **pMpegSurroundDecoder, int32_t stereoConfigIndex,
    HANDLE_FDK_QMF_DOMAIN pQmfDomain);

/**
 * \brief  Init one instance of the MPEG Surround decoder.
 *
 * Init one instance of the MPEG Surround decoder
 *
 * \param pMpegSurroundDecoder  A pointer to a decoder handle;
 *
 * \return SACDEC_ERROR error code
 */
SACDEC_ERROR mpegSurroundDecoder_Init(
    CMpegSurroundDecoder *pMpegSurroundDecoder);

/**
 * \brief Read and parse SpatialSpecificConfig.
 *
 * \param pMpegSurroundDecoder  A pointer to a decoder handle.
 * \param hBs bitstream handle config parsing data source.
 *
 * \return SACDEC_ERROR error code
 */
SACDEC_ERROR mpegSurroundDecoder_Config(
    CMpegSurroundDecoder *pMpegSurroundDecoder, HANDLE_FDK_BITSTREAM hBs,
    AUDIO_OBJECT_TYPE_t coreCodec, int32_t samplingRate, int32_t frameSize,
    int32_t stereoConfigIndex, int32_t coreSbrFrameLengthIndex, int32_t configBytes,
    const uint8_t configMode, uint8_t *configChanged);

SACDEC_ERROR
mpegSurroundDecoder_ConfigureQmfDomain(
    CMpegSurroundDecoder *pMpegSurroundDecoder,
    SAC_INPUT_CONFIG sac_dec_interface, uint32_t coreSamplingRate,
    AUDIO_OBJECT_TYPE_t coreCodec);

/**
 * \brief Parse MPEG Surround data without header
 *
 * \param pMpegSurroundDecoder A MPEG Surrround decoder handle.
 * \param hBs                  Bit stream handle data input source
 * \param pMpsDataBits         Pointer to number of valid bits in extension
 * payload. Function updates mpsDataBits while parsing bitstream.
 * \param fGlobalIndependencyFlag Global independency flag of current frame.
 *
 * \return  Error code.
 */
int32_t mpegSurroundDecoder_ParseNoHeader(
    CMpegSurroundDecoder *pMpegSurroundDecoder, HANDLE_FDK_BITSTREAM hBs,
    int32_t *pMpsDataBits, int32_t fGlobalIndependencyFlag);

/* #ifdef SACDEC_MPS_ENABLE */
/**
 * \brief Parse MPEG Surround data with header. Header is ancType, ancStart,
 ancStop (4 bits total). Body is ancDataSegmentByte[i].
 *
 * \param pMpegSurroundDecoder A MPEG Surrround decoder handle.
 * \param hBs                  Bit stream handle data input source
 * \param pMpsDataBits         Pointer to number of valid bits in extension
 payload. Function updates mpsDataBits while parsing bitstream. Needs to be a
 multiple of 8 + 4 (4 bits header).
 * \param coreCodec            The audio object type of the core codec handling
 the downmix input signal.
 * \param sampleRate           Samplerate of input downmix data.
 * \param nChannels            Amount of input channels.
 * \param frameSize            Amount of input samples.
 * \param fGlobalIndependencyFlag Global independency flag of current frame.
 *
 * \return  Error code.
 */
int32_t mpegSurroundDecoder_Parse(CMpegSurroundDecoder *pMpegSurroundDecoder,
                              HANDLE_FDK_BITSTREAM hBs, int32_t *pMpsDataBits,
                              AUDIO_OBJECT_TYPE_t coreCodec, int32_t sampleRate,
                              int32_t frameSize, int32_t fGlobalIndependencyFlag);
/* #endif */

/**
 * \brief Apply MPEG Surround upmix.
 *
 * Process one downmix audio frame and decode one surround frame if it applies.
 * Downmix framing can be different from surround framing, so depending on the
 * frame size of the downmix audio data and the framing being used by the MPEG
 * Surround decoder, it could be that only every second call, for example, of
 * this function actually surround data was decoded. The returned value of
 * frameSize will be zero, if no surround data was decoded.
 *
 * Decoding one MPEG Surround frame. Depending on interface configuration
 * mpegSurroundDecoder_SetParam(self, SACDEC_INTERFACE, value), the QMF or time
 * interface will be applied. External access to QMF buffer interface can be
 * achieved by mpegSurroundDecoder_GetQmfBuffer() call before decode frame.
 * While using time interface, pTimeData buffer will be shared as input and
 * output buffer.
 *
 * \param pMpegSurroundDecoder A MPEG Surrround decoder handle.
 * \param pTimeData            Pointer to time buffer. Depending on interface
 * configuration, the content of pTimeData is ignored, and the internal QMF
 * buffer will be used as input data source.
 * Otherwise, the MPEG Surround processing is applied to the timesignal
 * pTimeData. For both variants, the resulting MPEG
 * Surround signal is written into pTimeData.
 * \param timeDataSize         Size of pTimeData (available buffer size).
 * \param timeDataFrameSize    Frame size of input timedata
 * \param nChannels            Pointer where the amount of input channels is
 * given and amount of output channels is returned.
 * \param frameSize            Pointer where the amount of output samples is
 * returned into.
 * \param channelType          Array were the corresponding channel type for
 * each output audio channel is stored into.
 * \param channelIndices       Array were the corresponding channel type index
 * for each output audio channel is stored into.
 * \param mapDescr             Channep map descriptor for output channel mapping
 * to be used (From MPEG PCE ordering to whatever is required).
 * \param inDataHeadroom       Headroom of SAC input time signal to prevent
 * clipping.
 * \param outDataHeadroom      Pointer to headroom of SAC output time signal to
 * prevent clipping.
 *
 * \return  Error code.
 */
int32_t mpegSurroundDecoder_Apply(CMpegSurroundDecoder *pMpegSurroundDecoder,
                              PCM_MPS *input, PCM_MPS *pTimeData,
                              const int32_t timeDataSize, int32_t timeDataFrameSize,
                              int32_t *nChannels, int32_t *frameSize, int32_t sampleRate,
                              AUDIO_OBJECT_TYPE_t coreCodec,
                              AUDIO_CHANNEL_TYPE_t channelType[],
                              uint8_t channelIndices[],
                              const FDK_channelMapDescr_t *const mapDescr,
                              const int32_t inDataHeadroom, int32_t *outDataHeadroom);

/**
 * \brief                       Deallocate a MPEG Surround decoder instance.
 * \param pMpegSurroundDecoder  A decoder handle.
 * \return                      No return value.
 */
void mpegSurroundDecoder_Close(CMpegSurroundDecoder *pMpegSurroundDecoder);

/**
 * \brief                       Free config dependent MPEG Surround memory.
 * \param pMpegSurroundDecoder  A decoder handle.
 * \return                      error.
 */
SACDEC_ERROR mpegSurroundDecoder_FreeMem(
    CMpegSurroundDecoder *pMpegSurroundDecoder);

/**
 * \brief  Set one single MPEG Surround decoder parameter.
 *
 * \param pMpegSurroundDecoder  A MPEG Surrround decoder handle. Must not be
 * NULL pointer.
 * \param param                 Parameter to be set. See SACDEC_PARAM.
 * \param value                 Parameter value. See SACDEC_PARAM.
 *
 * \return  0 on sucess, and non-zero on failure.
 */
SACDEC_ERROR mpegSurroundDecoder_SetParam(
    CMpegSurroundDecoder *pMpegSurroundDecoder, const SACDEC_PARAM param,
    const int32_t value);

/**
 * \brief          Retrieve MPEG Surround decoder library info and fill info list with all depending library infos.
 * \param libInfo  Pointer to library info list to be filled.
 * \return         0 on sucess, and non-zero on failure.
 **/
int32_t mpegSurroundDecoder_GetLibInfo(LIB_INFO_t *libInfo);

/**
 * \brief  Set one single MPEG Surround decoder parameter.
 *
 * \param pMpegSurroundDecoder  A valid MPEG Surrround decoder handle.
 *
 * \return  The additional signal delay caused by the module.
 */
uint32_t mpegSurroundDecoder_GetDelay(const CMpegSurroundDecoder *self);

/**
 * \brief  Get info on whether the USAC pseudo LR feature is active.
 *
 * \param pMpegSurroundDecoder  A valid MPEG Surrround decoder handle.
 * \param bsPseudoLr            Pointer to return wether pseudo LR USAC feature
 * is used.
 *
 * \return  0 on sucess, and non-zero on failure.
 */
SACDEC_ERROR mpegSurroundDecoder_IsPseudoLR(
    CMpegSurroundDecoder *pMpegSurroundDecoder, int32_t *bsPseudoLr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef SAC_DEC_LIB_H */
