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

   Description:

*******************************************************************************/

#ifndef AACDECODER_H
#define AACDECODER_H

#include "../libFDK/common_fix.h"

#include "../libFDK/FDK_bitstream.h"

#include "channel.h"

#include "../libMpegTPDec/tpdec_lib.h"
#include "../libSYS/FDK_audio.h"

#include "block.h"
#include "../libFDK/FDK_qmf_domain.h"

#include "../libSBRdec/sbrdecoder.h"

#include "../libAACdec/aacdec_drc.h"

#include "../libPCMutils/pcmdmx_lib.h"

#include "../libDRCdec/FDK_drcDecLib.h"

#include "../libPCMutils/limiter.h"

#include "../libAACdec/FDK_delay.h"

#define TIME_DATA_FLUSH_SIZE (128)
#define TIME_DATA_FLUSH_SIZE_SF (7)

#define AACDEC_MAX_NUM_PREROLL_AU_USAC (3)
#if (AACDEC_MAX_NUM_PREROLL_AU < 3)
#undef AACDEC_MAX_NUM_PREROLL_AU
#define AACDEC_MAX_NUM_PREROLL_AU AACDEC_MAX_NUM_PREROLL_AU_USAC
#endif

typedef struct AAC_DECODER_INSTANCE *HANDLE_AACDECODER;

enum { L = 0, R = 1 };

typedef struct {
  unsigned char *buffer;
  int32_t bufferSize;
  int32_t offset[8];
  int32_t nrElements;
} CAncData;

typedef enum { NOT_DEFINED = -1, MODE_HQ = 0, MODE_LP = 1 } QMF_MODE;

typedef struct {
  int32_t bsDelay;
} SBR_PARAMS;

enum {
  AACDEC_FLUSH_OFF = 0,
  AACDEC_RSV60_CFG_CHANGE_ATSC_FLUSH_ON = 1,
  AACDEC_RSV60_DASH_IPF_ATSC_FLUSH_ON = 2,
  AACDEC_USAC_DASH_IPF_FLUSH_ON = 3
};

enum {
  AACDEC_BUILD_UP_OFF = 0,
  AACDEC_RSV60_BUILD_UP_ON = 1,
  AACDEC_RSV60_BUILD_UP_ON_IN_BAND = 2,
  AACDEC_USAC_BUILD_UP_ON = 3,
  AACDEC_RSV60_BUILD_UP_IDLE = 4,
  AACDEC_RSV60_BUILD_UP_IDLE_IN_BAND = 5
};

typedef struct {
  /* Usac Extension Elements */
  USAC_EXT_ELEMENT_TYPE_t usacExtElementType[(3)];
  uint32_t usacExtElementDefaultLength[(3)];
  uint8_t usacExtElementPayloadFrag[(3)];
} CUsacCoreExtensions;

/* AAC decoder (opaque toward userland) struct declaration */
struct AAC_DECODER_INSTANCE {
  int32_t aacChannels; /*!< Amount of AAC decoder channels allocated.        */
  int32_t ascChannels[(1 *
                   1)]; /*!< Amount of AAC decoder channels signalled in ASC. */
  int32_t blockNumber;      /*!< frame counter                                    */

  int32_t nrOfLayers;

  int32_t outputInterleaved; /*!< PCM output format (interleaved/none interleaved).
                          */

  int32_t aacOutDataHeadroom; /*!< Headroom of the output time signal to prevent
                             clipping */

  HANDLE_TRANSPORTDEC hInput; /*!< Transport layer handle. */

  SamplingRateInfo
      samplingRateInfo[(1 * 1)]; /*!< Sampling Rate information table */

  uint8_t
  frameOK; /*!< Will be unset if a consistency check, e.g. CRC etc. fails */

  uint32_t flags[(1 * 1)]; /*!< Flags for internal decoder use. DO NOT USE
                          self::streaminfo::flags ! */
  uint32_t elFlags[(3 * ((8) * 2) + (((8) * 2)) / 2 + 4 * (1) +
                1)]; /*!< Flags for internal decoder use (element specific). DO
                        NOT USE self::streaminfo::flags ! */

  MP4_ELEMENT_ID_t elements[(3 * ((8) * 2) + (((8) * 2)) / 2 + 4 * (1) +
                           1)]; /*!< Table where the element Id's are listed */
  uint8_t elTags[(3 * ((8) * 2) + (((8) * 2)) / 2 + 4 * (1) +
                1)]; /*!< Table where the elements id Tags are listed      */
  uint8_t chMapping[((8) * 2)]; /*!< Table of MPEG canonical order to bitstream
                                 channel order mapping. */

  AUDIO_CHANNEL_TYPE_t channelType[(8)]; /*!< Audio channel type of each output
                                          audio channel (from 0 upto
                                          numChannels).           */
  uint8_t channelIndices[(8)]; /*!< Audio channel index for each output audio
                                channel (from 0 upto numChannels).         */
  /* See ISO/IEC 13818-7:2005(E), 8.5.3.2 Explicit channel mapping using a
   * program_config_element() */

  FDK_channelMapDescr_t mapDescr; /*!< Describes the output channel mapping. */
  uint8_t chMapIndex; /*!< Index to access one line of the channelOutputMapping
                       table. This is required because not all 8 channel
                       configurations have the same output mapping. */
  int32_t sbrDataLen;   /*!< Expected length of the SBR remaining in bitbuffer after
                         the AAC payload has been pared.   */

  CProgramConfig pce;
  CStreamInfo
      streamInfo; /*!< Pointer to StreamInfo data (read from the bitstream) */
  CAacDecoderChannelInfo
      *pAacDecoderChannelInfo[(8)]; /*!< Temporal channel memory */
  CAacDecoderStaticChannelInfo
      *pAacDecoderStaticChannelInfo[(8)]; /*!< Persistent channel memory */

  int32_t *workBufferCore1;
  int32_t *workBufferCore2;
  int32_t *pTimeData2;
  int32_t timeData2Size;

  CpePersistentData *cpeStaticData[(
      3 * ((8) * 2) + (((8) * 2)) / 2 + 4 * (1) +
      1)]; /*!< Pointer to persistent data shared by both channels of a CPE.
This structure is allocated once for each CPE. */

  CConcealParams concealCommonData;
  CConcealmentMethod concealMethodUser;

  CUsacCoreExtensions usacCoreExt; /*!< Data and handles to extend USAC FD/LPD
                                      core decoder (SBR, MPS, ...) */
  uint32_t numUsacElements[(1 * 1)];
  uint8_t usacStereoConfigIndex[(3 * ((8) * 2) + (((8) * 2)) / 2 + 4 * (1) + 1)];
  const CSUsacConfig *pUsacConfig[(1 * 1)];
  int32_t nbDiv; /*!< number of frame divisions in LPD-domain */

  uint8_t useLdQmfTimeAlign;

  int32_t aacChannelsPrev; /*!< The amount of AAC core channels of the last
                          successful decode call.         */
  AUDIO_CHANNEL_TYPE_t channelTypePrev[(8)]; /*!< Array holding the channelType
                                              values of the last successful
                                              decode call.    */
  uint8_t
  channelIndicesPrev[(8)]; /*!< Array holding the channelIndices values of
                              the last successful decode call. */

  uint8_t
  downscaleFactor; /*!< Variable to store a supported ELD downscale factor
                      of 1, 2, 3 or 4 */
  uint8_t downscaleFactorInBS; /*!< Variable to store the (not necessarily
                                supported) ELD downscale factor discovered in
                                the bitstream */

  HANDLE_SBRDECODER hSbrDecoder; /*!< SBR decoder handle. */
  uint8_t sbrEnabled;     /*!< flag to store if SBR has been detected     */
  uint8_t sbrEnabledPrev; /*!< flag to store if SBR has been detected from
                           previous frame */
  uint8_t psPossible;     /*!< flag to store if PS is possible            */
  SBR_PARAMS sbrParams; /*!< struct to store all sbr parameters         */

  uint8_t *pDrmBsBuffer; /*!< Pointer to dynamic buffer which is used to reverse
                          the bits of the DRM SBR payload */
  uint16_t drmBsBufferSize; /*!< Size of the dynamic buffer which is used to
                             reverse the bits of the DRM SBR payload */
  FDK_QMF_DOMAIN
  qmfDomain; /*!< Instance of module for QMF domain data handling */

  QMF_MODE qmfModeCurr; /*!< The current QMF mode                       */
  QMF_MODE qmfModeUser; /*!< The QMF mode requested by the library user */

  HANDLE_AAC_DRC hDrcInfo; /*!< handle to DRC data structure               */
  int32_t metadataExpiry;      /*!< Metadata expiry time in milli-seconds.     */

  void *pMpegSurroundDecoder; /*!< pointer to mpeg surround decoder structure */
  uint8_t mpsEnableUser;        /*!< MPS enable user flag                       */
  uint8_t mpsEnableCurr;        /*!< MPS enable decoder state                   */
  uint8_t mpsApplicable;        /*!< MPS applicable                             */
  int8_t mpsOutputMode; /*!< setting: normal = 0, binaural = 1, stereo = 2, 5.1ch
                          = 3 */
  int32_t mpsOutChannelsLast; /*!< The amount of channels returned by the last
                             successful MPS decoder call. */
  int32_t mpsFrameSizeLast;   /*!< The frame length returned by the last successful
                             MPS decoder call. */

  CAncData ancData; /*!< structure to handle ancillary data         */

  HANDLE_PCM_DOWNMIX hPcmUtils; /*!< privat data for the PCM utils. */

  TDLimiterPtr hLimiter;   /*!< Handle of time domain limiter.             */
  uint8_t limiterEnableUser; /*!< The limiter configuration requested by the
                              library user */
  uint8_t limiterEnableCurr; /*!< The current limiter configuration.         */

  int32_t extGain[1]; /*!< Gain that must be applied to the output signal. */
  uint32_t extGainDelay;   /*!< Delay that must be accounted for extGain. */

  HANDLE_DRC_DECODER hUniDrcDecoder;
  uint8_t multibandDrcPresent;
  uint8_t numTimeSlots;
  uint32_t loudnessInfoSetPosition[3];
  int8_t defaultTargetLoudness;

  int16_t
  *pTimeDataFlush[((8) * 2)]; /*!< Pointer to the flushed time data which
                                 will be used for the crossfade in case of
                                 an USAC DASH IPF config change */

  uint8_t flushStatus;     /*!< Indicates flush status: on|off */
  int8_t flushCnt;        /*!< Flush frame counter */
  uint8_t buildUpStatus;   /*!< Indicates build up status: on|off */
  int8_t buildUpCnt;      /*!< Build up frame counter */
  uint8_t hasAudioPreRoll; /*!< Indicates preRoll status: on|off */
  uint32_t prerollAULength[AACDEC_MAX_NUM_PREROLL_AU + 1]; /*!< Relative offset of
                                                          the prerollAU end
                                                          position to the AU
                                                          start position in the
                                                          bitstream */
  int32_t accessUnit; /*!< Number of the actual processed preroll accessUnit */
  uint8_t applyCrossfade; /*!< if set crossfade for seamless stream switching is
                           applied */

  FDK_SignalDelay usacResidualDelay; /*!< Delay residual signal to compensate
                                        for eSBR delay of DMX signal in case of
                                        stereoConfigIndex==2. */
};

#define AAC_DEBUG_EXTHLP \
  "\
--- AAC-Core ---\n\
    0x00010000 Header data\n\
    0x00020000 CRC data\n\
    0x00040000 Channel info\n\
    0x00080000 Section data\n\
    0x00100000 Scalefactor data\n\
    0x00200000 Pulse data\n\
    0x00400000 Tns data\n\
    0x00800000 Quantized spectrum\n\
    0x01000000 Requantized spectrum\n\
    0x02000000 Time output\n\
    0x04000000 Fatal errors\n\
    0x08000000 Buffer fullness\n\
    0x10000000 Average bitrate\n\
    0x20000000 Synchronization\n\
    0x40000000 Concealment\n\
    0x7FFF0000 all AAC-Core-Info\n\
"

/**
 * \brief Synchronise QMF mode for all modules using QMF data.
 * \param self decoder handle
 */
void CAacDecoder_SyncQmfMode(HANDLE_AACDECODER self);

/**
 * \brief Signal a bit stream interruption to the decoder
 * \param self decoder handle
 */
void CAacDecoder_SignalInterruption(HANDLE_AACDECODER self);

/*!
  \brief Initialize ancillary buffer

  \ancData Pointer to ancillary data structure
  \buffer Pointer to (external) anc data buffer
  \size Size of the buffer pointed on by buffer

  \return  Error code
*/
AAC_DECODER_ERROR CAacDecoder_AncDataInit(CAncData *ancData,
                                          unsigned char *buffer, int32_t size);

/*!
  \brief Get one ancillary data element

  \ancData Pointer to ancillary data structure
  \index Index of the anc data element to get
  \ptr Pointer to a buffer receiving a pointer to the requested anc data element
  \size Pointer to a buffer receiving the length of the requested anc data
  element

  \return  Error code
*/
AAC_DECODER_ERROR CAacDecoder_AncDataGet(CAncData *ancData, int32_t index,
                                         unsigned char **ptr, int32_t *size);

/* initialization of aac decoder */
HANDLE_AACDECODER CAacDecoder_Open(TRANSPORT_TYPE_t bsFormat);

/* Initialization of channel elements */
AAC_DECODER_ERROR CAacDecoder_Init(HANDLE_AACDECODER self,
                                              const CSAudioSpecificConfig *asc,
                                              uint8_t configMode,
                                              uint8_t *configChanged);
/*!
  \brief Decodes one aac frame

  The function decodes one aac frame. The decoding of coupling channel
  elements are not supported. The transport layer might signal, that the
  data of the current frame is invalid, e.g. as a result of a packet
  loss in streaming mode.
  The bitstream position of transportDec_GetBitstream(self->hInput) must
  be exactly the end of the access unit, including all byte alignment bits.
  For this purpose, the variable auStartAnchor is used.

  \return  error status
*/
AAC_DECODER_ERROR CAacDecoder_DecodeFrame(
    HANDLE_AACDECODER self, const uint32_t flags, int32_t *pTimeData,
    const int32_t timeDataSize, const int32_t timeDataChannelOffset);

/* Free config dependent AAC memory */
AAC_DECODER_ERROR CAacDecoder_FreeMem(HANDLE_AACDECODER self,
                                                 const int32_t subStreamIndex);

/* Prepare crossfade for USAC DASH IPF config change */
AAC_DECODER_ERROR CAacDecoder_PrepareCrossFade(
    const int16_t *pTimeData, int16_t **pTimeDataFlush, const int32_t numChannels,
    const int32_t frameSize, const int32_t interleaved);

/* Apply crossfade for USAC DASH IPF config change */
AAC_DECODER_ERROR CAacDecoder_ApplyCrossFade(
    int16_t *pTimeData, int16_t **pTimeDataFlush, const int32_t numChannels,
    const int32_t frameSize, const int32_t interleaved);

/* Set flush and build up mode */
AAC_DECODER_ERROR CAacDecoder_CtrlCFGChange(HANDLE_AACDECODER self,
                                                       uint8_t flushStatus,
                                                       int8_t flushCnt,
                                                       uint8_t buildUpStatus,
                                                       int8_t buildUpCnt);

/* Parse preRoll Extension Payload */
AAC_DECODER_ERROR CAacDecoder_PreRollExtensionPayloadParse(
    HANDLE_AACDECODER self, uint32_t *numPrerollAU, uint32_t *prerollAUOffset,
    uint32_t *prerollAULength);

/* Destroy aac decoder */
void CAacDecoder_Close(HANDLE_AACDECODER self);

/* get streaminfo handle from decoder */
CStreamInfo *CAacDecoder_GetStreamInfo(HANDLE_AACDECODER self);

#endif /* #ifndef AACDECODER_H */
