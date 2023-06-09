

/**************************** PCM utility library ******************************

   Author(s):   Christian Griebel

   Description:

*******************************************************************************/

/**
 * \file   pcmdmx_lib.h
 * \brief  FDK PCM audio mixdown library interface header file.

 \page INTRO Introduction


 \section SCOPE Scope

 This document describes the high-level application interface and usage of the
 FDK PCM audio mixdown module library developed by the Fraunhofer Institute for
 Integrated Circuits (IIS). Depending on the library configuration, the module
 can manipulate the number of audio channels of a given PCM signal. It can
 create for example a two channel stereo audio signal from a given multi-channel
 configuration (e.g. 5.1 channels).


 \page ABBREV List of abbreviations

  \li \b AAC - Advanced Audio Coding\n
    Is an audio coding standard for lossy digital audio compression standardized
 by ISO and IEC, as part of the MPEG-2 (ISO/IEC 13818-7:2006) and MPEG-4
 (ISO/IEC 14496-3:2009) specifications.

  \li \b DSE - Data Stream Element\n
    A syntactical element of the MPEG-2/4 Advanced Audio Coding bitstream
 standardized in ISO/IEC 14496-3:2009. It can convey any kind of data associated
 to one program.

  \li \b PCE - Program Config Element\n
    A syntactical element of the MPEG-2/4 Advanced Audio Coding bitstream
 standardized in ISO/IEC 14496-3:2009 that can define the stream configuration
 for a single program. In addition it can comprise simple downmix meta data.

 */

#ifndef PCMDMX_LIB_H
#define PCMDMX_LIB_H

#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/common_fix.h"
#include "../libFDK/FDK_bitstream.h"

/**
 *  \enum  PCMDMX_ERROR
 *
 *  Error codes that can be returned by module interface functions.
 */
typedef enum {
  PCMDMX_OK = 0x0, /*!< No error happened. */
  PCMDMX_UNSUPPORTED =
      0x1, /*!< The requested feature/service is unavailable. This can
                occur if the module was built for a wrong configuration.  */
  pcm_dmx_fatal_error_start,
  PCMDMX_OUT_OF_MEMORY, /*!< Not enough memory to set up an instance of the
                           module.    */
  pcm_dmx_fatal_error_end,

  PCMDMX_INVALID_HANDLE,   /*!< The given instance handle is not valid.   */
  PCMDMX_INVALID_ARGUMENT, /*!< One of the parameters handed over is invalid. */
  PCMDMX_INVALID_CH_CONFIG, /*!< The given channel configuration is not
                               supported and thus no processing was performed.
                             */
  PCMDMX_INVALID_MODE,  /*!< The set configuration/mode is not applicable.  */
  PCMDMX_UNKNOWN_PARAM, /*!< The handed parameter is not known/supported. */
  PCMDMX_UNABLE_TO_SET_PARAM, /*!< Unable to set the specific parameter. Most
                                 probably the    value ist out of range.
                               */
  PCMDMX_CORRUPT_ANC_DATA,    /*!< The read ancillary data was corrupt.       */
  PCMDMX_OUTPUT_BUFFER_TOO_SMALL /*!< The size of pcm output buffer is too
                                    small.               */

} PCMDMX_ERROR;

/** Macro to identify fatal errors. */
#define PCMDMX_IS_FATAL_ERROR(err)          \
  ((((err) >= pcm_dmx_fatal_error_start) && \
    ((err) <= pcm_dmx_fatal_error_end))     \
       ? 1                                  \
       : 0)

/**
 *  \enum  PCMDMX_PARAM
 *
 *  Modules dynamic runtime parameters that can be handed to function
 * pcmDmx_SetParam() and pcmDmx_GetParam().
 */
typedef enum {
  DMX_PROFILE_SETTING =
      0x01, /*!< Defines which equations, coefficients and default/
                 fallback values used for downmixing. See
                 ::DMX_PROFILE_TYPE type for details.             */
  DMX_BS_DATA_EXPIRY_FRAME =
      0x10, /*!< The number of frames without new metadata that
                 have to go by before the bitstream data expires.
                 The value 0 disables expiry.                     */
  DMX_BS_DATA_DELAY =
      0x11, /*!< The number of delay frames of the output samples
                 compared to the bitstream data.                  */
  MIN_NUMBER_OF_OUTPUT_CHANNELS =
      0x20, /*!< The minimum number of output channels. For all
                 input configurations that have less than the given
                 channels the module will modify the output
                 automatically to obtain the given number of output
                 channels. Mono signals will be duplicated. If more
                 than two output channels are desired the module
                 just adds empty channels. The parameter value must
                 be either -1, 0, 1, 2, 6 or 8. If the value is
                 greater than zero and exceeds the value of
                 parameter ::MAX_NUMBER_OF_OUTPUT_CHANNELS the
                 latter will be set to the same value. Both values
                 -1 and 0 disable the feature.                    */
  MAX_NUMBER_OF_OUTPUT_CHANNELS =
      0x21, /*!< The maximum number of output channels. For all
                 input configurations that have more than the given
                 channels the module will apply a mixdown
                 automatically to obtain the given number of output
                 channels. The value must be either -1, 0, 1, 2, 6
                 or 8. If it's greater than zero and lower or equal
                 than the value of ::MIN_NUMBER_OF_OUTPUT_CHANNELS
                 parameter the latter will be set to the same value.
                 The values -1 and 0 disable the feature.         */
  DMX_DUAL_CHANNEL_MODE =
      0x30, /*!< Downmix mode for two channel audio data. See type
                 ::DUAL_CHANNEL_MODE for details.                 */
  DMX_PSEUDO_SURROUND_MODE =
      0x31 /*!< Defines how module handles pseudo surround
                compatible signals. See ::PSEUDO_SURROUND_MODE
                type for details.                                */
} PCMDMX_PARAM;

/**
 *  \enum  DMX_PROFILE_TYPE
 *
 *  Valid value list for parameter ::DMX_PROFILE_SETTING.
 */
typedef enum {
  DMX_PRFL_STANDARD =
      0x0, /*!< The standard profile creates mixdown signals based on
                the advanced downmix metadata (from a DSE), equations
                and default values defined in ISO/IEC 14496:3
                Ammendment 4. Any other (legacy) downmix metadata will
                be ignored.                                            */
  DMX_PRFL_MATRIX_MIX =
      0x1, /*!< This profile behaves just as the standard profile if
                advanced downmix metadata (from a DSE) is available. If
                not, the matrix_mixdown information embedded in the
                program configuration element (PCE) will be applied. If
                neither is the case the module creates a mixdown using
                the default coefficients defined in MPEG-4 Ammendment 4.
                The profile can be used e.g. to support legacy digital
                TV (e.g. DVB) streams.                                 */
  DMX_PRFL_FORCE_MATRIX_MIX =
      0x2, /*!< Similar to the ::DMX_PRFL_MATRIX_MIX profile but if both
                the advanced (DSE) and the legacy (PCE) MPEG downmix
                metadata are available the latter will be applied.     */
  DMX_PRFL_ARIB_JAPAN =
      0x3 /*!< Downmix creation as described in ABNT NBR 15602-2. But
               if advanced downmix metadata is available it will be
               prefered.                                              */
} DMX_PROFILE_TYPE;

/**
 *  \enum PSEUDO_SURROUND_MODE
 *
 *  Valid value list for parameter ::DMX_PSEUDO_SURROUND_MODE.
 */
typedef enum {
  NEVER_DO_PS_DMX =
      -1, /*!< Ignore any metadata and do never create a pseudo surround
               compatible downmix. (Default)                             */
  AUTO_PS_DMX = 0, /*!< Create a pseudo surround compatible downmix only if
                        signalled in bitstreams meta data. */
  FORCE_PS_DMX =
      1 /*!< Always create a pseudo surround compatible downmix.
             CAUTION: This can lead to excessive signal cancellations
             and signal level differences for non-compatible signals.  */
} PSEUDO_SURROUND_MODE;

/**
 *  \enum DUAL_CHANNEL_MODE
 *
 *  Valid value list for parameter ::DMX_DUAL_CHANNEL_MODE.
 */
typedef enum {
  STEREO_MODE = 0x0, /*!< Leave stereo signals as they are. */
  CH1_MODE = 0x1,    /*!< Create a dual mono output signal from channel 1.    */
  CH2_MODE = 0x2,    /*!< Create a dual mono output signal from channel 2.    */
  MIXED_MODE = 0x3   /*!< Create a dual mono output signal by mixing the two
                          channels.   */
} DUAL_CHANNEL_MODE;

#define DMX_PCM int32_t
#define DMX_PCMF int32_t
#define DMX_PCM_BITS DFRACT_BITS

/* ------------------------ *
 *     MODULES INTERFACE:   *
 * ------------------------ */
typedef struct PCM_DMX_INSTANCE *HANDLE_PCM_DOWNMIX;

/*! \addtogroup pcmDmxResetFlags Modules reset flags
 *  Macros that can be used as parameter for function pcmDmx_Reset() to specify
 * which parts of the module shall be reset.
 *  @{
 *
 *  \def  PCMDMX_RESET_PARAMS
 *  Only reset the user specific parameters that have been modified with
 * pcmDmx_SetParam().
 *
 *  \def  PCMDMX_RESET_BS_DATA
 *  Delete the meta data that has been fed with the appropriate interface
 * functions.
 *
 *  \def  PCMDMX_RESET_FULL
 *  Reset the complete module instance to the state after pcmDmx_Open() had been
 * called.
 */
#define PCMDMX_RESET_PARAMS (1)
#define PCMDMX_RESET_BS_DATA (2)
#define PCMDMX_RESET_FULL (PCMDMX_RESET_PARAMS | PCMDMX_RESET_BS_DATA)
/*! @} */

#ifdef __cplusplus
extern "C" {
#endif

/** Open and initialize an instance of the PCM downmix module
 * @param[out] pSelf  Pointer to a buffer receiving the handle of the new
 *instance.
 * @returns           Returns an error code of type ::PCMDMX_ERROR.
 **/
PCMDMX_ERROR pcmDmx_Open(HANDLE_PCM_DOWNMIX *pSelf);

/** Set one parameter for a single instance of the PCM downmix module.
 * @param[in] self   Handle of PCM downmix instance.
 * @param[in] param  Parameter to be set. Can be one from the ::PCMDMX_PARAM
 *list.
 * @param[in] value  Parameter value.
 * @returns          Returns an error code of type ::PCMDMX_ERROR.
 **/
PCMDMX_ERROR pcmDmx_SetParam(HANDLE_PCM_DOWNMIX self, const PCMDMX_PARAM param,
                             const int32_t value);

/** Get one parameter value of a single PCM downmix module instance.
 * @param[in]  self    Handle of PCM downmix module instance.
 * @param[in]  param   Parameter to query. Can be one from the ::PCMDMX_PARAM
 *list.
 * @param[out] pValue  Pointer to buffer receiving the parameter value.
 * @returns            Returns an error code of type ::PCMDMX_ERROR.
 **/
PCMDMX_ERROR pcmDmx_GetParam(HANDLE_PCM_DOWNMIX self, const PCMDMX_PARAM param,
                             int32_t *const pValue);

/** \cond
 *  Extract relevant downmix meta-data directly from a given bitstream. The
 *function can handle both data specified in ETSI TS 101 154 or ISO/IEC
 *14496-3:2009/Amd.4:2013.
 * @param[in] self         Handle of PCM downmix instance.
 * @param[in] hBitStream   Handle of FDK bitstream buffer.
 * @param[in] ancDataBits  Length of ancillary data in bits.
 * @param[in] isMpeg2      Flag indicating wheter the ancillary data is from a
 *MPEG-1/2 or a MPEG-4 stream.
 * @returns                Returns an error code of type ::PCMDMX_ERROR.
 **/
PCMDMX_ERROR pcmDmx_Parse(HANDLE_PCM_DOWNMIX self,
                          HANDLE_FDK_BITSTREAM hBitStream, uint32_t ancDataBits,
                          int32_t isMpeg2);
/** \endcond */

/** Read from a given ancillary data buffer and extract the relevant downmix
 *meta-data. The function can handle both data specified in ETSI TS 101 154 or
 *ISO/IEC 14496-3:2009/Amd.4:2013.
 * @param[in] self          Handle of PCM downmix instance.
 * @param[in] pAncDataBuf   Pointer to ancillary buffer holding the data.
 * @param[in] ancDataBytes  Size of ancillary data in bytes.
 * @param[in] isMpeg2       Flag indicating wheter the ancillary data is from a
 *MPEG-1/2 or a MPEG-4 stream.
 * @returns                 Returns an error code of type ::PCMDMX_ERROR.
 **/
PCMDMX_ERROR pcmDmx_ReadDvbAncData(HANDLE_PCM_DOWNMIX self, uint8_t *pAncDataBuf,
                                   uint32_t ancDataBytes, int32_t isMpeg2);

/** Set the matrix mixdown information extracted from the PCE of an AAC
 *bitstream.
 * @param[in] self                  Handle of PCM downmix instance.
 * @param[in] matrixMixdownPresent  Matrix mixdown index present flag extracted
 *from PCE.
 * @param[in] matrixMixdownIdx      The 2 bit matrix mixdown index extracted
 *from PCE.
 * @param[in] pseudoSurroundEnable  The pseudo surround enable flag extracted
 *from PCE.
 * @returns                         Returns an error code of type
 *::PCMDMX_ERROR.
 **/
PCMDMX_ERROR pcmDmx_SetMatrixMixdownFromPce(HANDLE_PCM_DOWNMIX self,
                                            int32_t matrixMixdownPresent,
                                            int32_t matrixMixdownIdx,
                                            int32_t pseudoSurroundEnable);

/** Reset the module.
 * @param[in] self   Handle of PCM downmix instance.
 * @param[in] flags  Flags telling which parts of the module shall be reset.
 *                   See \ref pcmDmxResetFlags for details.
 * @returns          Returns an error code of type ::PCMDMX_ERROR.
 **/
PCMDMX_ERROR pcmDmx_Reset(HANDLE_PCM_DOWNMIX self, uint32_t flags);

/** Create a mixdown, bypass or extend the output signal depending on the
 *modules settings and the respective given input configuration.
 *
 * \param[in]     self            Handle of PCM downmix module instance.
 * \param[in,out] pPcmBuf         Pointer to time buffer with PCM samples.
 * \param[in]     pcmBufSize      Size of pPcmBuf buffer.
 * \param[in]     frameSize       The I/O block size which is the number of samples per channel.
 * \param[in,out] nChannels       Pointer to buffer that holds the number of input channels and
 *                                where the amount of output channels is written
 *to.
 * \param[in]     fInterleaved    Input and output samples are processed interleaved.
 * \param[in,out] channelType     Array were the corresponding channel type for each output audio
 *                                channel is stored into.
 * \param[in,out] channelIndices  Array were the corresponding channel type index for each output
 *                                audio channel is stored into.
 * \param[in]     mapDescr        Pointer to a FDK channel mapping descriptor that contains the
 *                                channel mapping to be used.
 * \param[out]    pDmxOutScale    Pointer on a field receiving the scale factor that has to be
 *                                applied on all samples afterwards. If the
 *handed pointer is NULL the final scaling is done internally.
 * @returns                       Returns an error code of type ::PCMDMX_ERROR.
 **/
PCMDMX_ERROR pcmDmx_ApplyFrame(HANDLE_PCM_DOWNMIX self, DMX_PCM *pPcmBuf,
                               const int32_t pcmBufSize, uint32_t frameSize,
                               int32_t *nChannels, int32_t fInterleaved,
                               AUDIO_CHANNEL_TYPE_t channelType[],
                               uint8_t channelIndices[],
                               const FDK_channelMapDescr_t *const mapDescr,
                               int32_t *pDmxOutScale);

/** Close an instance of the PCM downmix module.
 * @param[in,out] pSelf  Pointer to a buffer containing the handle of the
 *instance.
 * @returns              Returns an error code of type ::PCMDMX_ERROR.
 **/
PCMDMX_ERROR pcmDmx_Close(HANDLE_PCM_DOWNMIX *pSelf);

/** Get library info for this module.
 * @param[out] info  Pointer to an allocated LIB_INFO structure.
 * @returns          Returns an error code of type ::PCMDMX_ERROR.
 */
PCMDMX_ERROR pcmDmx_GetLibInfo(LIB_INFO_t *info);

#ifdef __cplusplus
}
#endif

#endif /* PCMDMX_LIB_H */
