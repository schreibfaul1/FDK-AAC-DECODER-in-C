/* -----------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2018 Fraunhofer-Gesellschaft zur Förderung der angewandten
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

/******************* MPEG transport format decoder library *********************

   Author(s):   Manuel Jander

   Description: MPEG Transport data tables

*******************************************************************************/

#ifndef TP_DATA_H
#define TP_DATA_H

#include "../libAACdec/newAACDecoder.h"
#include "../libSYS/FDK_audio.h"
#include "../libFDK/FDK_bitstream.h"

/*
 * Configuration
 */

#define TP_USAC_MAX_SPEAKERS (24)

#define TP_USAC_MAX_EXT_ELEMENTS ((24))

#define TP_USAC_MAX_ELEMENTS ((24) + TP_USAC_MAX_EXT_ELEMENTS)

#define TP_USAC_MAX_CONFIG_LEN                                         \
  512 /* next power of two of maximum of escapedValue(hBs, 4, 4, 8) in \
         AudioPreRoll() (285) */

#define TPDEC_USAC_NUM_CONFIG_CHANGE_FRAMES \
  (1) /* Number of frames for config change in USAC */

enum {
  TPDEC_FLUSH_OFF = 0,
  TPDEC_RSV60_CFG_CHANGE_ATSC_FLUSH_ON = 1,
  TPDEC_RSV60_DASH_IPF_ATSC_FLUSH_ON = 2,
  TPDEC_USAC_DASH_IPF_FLUSH_ON = 3
};

enum {
  TPDEC_BUILD_UP_OFF = 0,
  TPDEC_RSV60_BUILD_UP_ON = 1,
  TPDEC_RSV60_BUILD_UP_ON_IN_BAND = 2,
  TPDEC_USAC_BUILD_UP_ON = 3,
  TPDEC_RSV60_BUILD_UP_IDLE = 4,
  TPDEC_RSV60_BUILD_UP_IDLE_IN_BAND = 5
};

/**
 * ProgramConfig struct.
 */
/* ISO/IEC 14496-3 4.4.1.1 Table 4.2 Program config element */
#define PC_FSB_CHANNELS_MAX 16 /* Front/Side/Back channels */
#define PC_LFE_CHANNELS_MAX 4
#define PC_ASSOCDATA_MAX 8
#define PC_CCEL_MAX 16 /* CC elements */
#define PC_COMMENTLENGTH 256
#define PC_NUM_HEIGHT_LAYER 3

typedef struct {
  /* PCE bitstream elements: */
  uint8_t ElementInstanceTag;
  uint8_t Profile;
  uint8_t SamplingFrequencyIndex;
  uint8_t NumFrontChannelElements;
  uint8_t NumSideChannelElements;
  uint8_t NumBackChannelElements;
  uint8_t NumLfeChannelElements;
  uint8_t NumAssocDataElements;
  uint8_t NumValidCcElements;

  uint8_t MonoMixdownPresent;
  uint8_t MonoMixdownElementNumber;

  uint8_t StereoMixdownPresent;
  uint8_t StereoMixdownElementNumber;

  uint8_t MatrixMixdownIndexPresent;
  uint8_t MatrixMixdownIndex;
  uint8_t PseudoSurroundEnable;

  uint8_t FrontElementIsCpe[PC_FSB_CHANNELS_MAX];
  uint8_t FrontElementTagSelect[PC_FSB_CHANNELS_MAX];
  uint8_t FrontElementHeightInfo[PC_FSB_CHANNELS_MAX];

  uint8_t SideElementIsCpe[PC_FSB_CHANNELS_MAX];
  uint8_t SideElementTagSelect[PC_FSB_CHANNELS_MAX];
  uint8_t SideElementHeightInfo[PC_FSB_CHANNELS_MAX];

  uint8_t BackElementIsCpe[PC_FSB_CHANNELS_MAX];
  uint8_t BackElementTagSelect[PC_FSB_CHANNELS_MAX];
  uint8_t BackElementHeightInfo[PC_FSB_CHANNELS_MAX];

  uint8_t LfeElementTagSelect[PC_LFE_CHANNELS_MAX];

  uint8_t AssocDataElementTagSelect[PC_ASSOCDATA_MAX];

  uint8_t CcElementIsIndSw[PC_CCEL_MAX];
  uint8_t ValidCcElementTagSelect[PC_CCEL_MAX];

  uint8_t CommentFieldBytes;
  uint8_t Comment[PC_COMMENTLENGTH];

  /* Helper variables for administration: */
  uint8_t isValid; /*!< Flag showing if PCE has been read successfully. */
  uint8_t
  NumChannels; /*!< Amount of audio channels summing all channel elements
                  including LFEs */
  uint8_t NumEffectiveChannels; /*!< Amount of audio channels summing only SCEs
                                 and CPEs */
  uint8_t elCounter;

} CProgramConfig;

typedef enum {
  ASCEXT_UNKOWN = -1,
  ASCEXT_SBR = 0x2b7,
  ASCEXT_PS = 0x548,
  ASCEXT_MPS = 0x76a,
  ASCEXT_SAOC = 0x7cb,
  ASCEXT_LDMPS = 0x7cc

} TP_ASC_EXTENSION_ID;

/**
 * GaSpecificConfig struct
 */
typedef struct {
  uint32_t m_frameLengthFlag;
  uint32_t m_dependsOnCoreCoder;
  uint32_t m_coreCoderDelay;

  uint32_t m_extensionFlag;
  uint32_t m_extensionFlag3;

  uint32_t m_layer;
  uint32_t m_numOfSubFrame;
  uint32_t m_layerLength;

} CSGaSpecificConfig;

typedef enum {
  ELDEXT_TERM = 0x0,         /* Termination tag */
  ELDEXT_SAOC = 0x1,         /* SAOC config */
  ELDEXT_LDSAC = 0x2,        /* LD MPEG Surround config */
  ELDEXT_DOWNSCALEINFO = 0x3 /* ELD sample rate adaptation */
  /* reserved */
} ASC_ELD_EXT_TYPE;

typedef struct {
  uint8_t m_frameLengthFlag;

  uint8_t m_sbrPresentFlag;
  uint8_t
  m_useLdQmfTimeAlign; /* Use LD-MPS QMF in SBR to achive time alignment */
  uint8_t m_sbrSamplingRate;
  uint8_t m_sbrCrcFlag;
  uint32_t m_downscaledSamplingFrequency;

} CSEldSpecificConfig;

typedef struct {
  USAC_EXT_ELEMENT_TYPE usacExtElementType;
  uint16_t usacExtElementConfigLength;
  uint16_t usacExtElementDefaultLength;
  uint8_t usacExtElementPayloadFrag;
  uint8_t usacExtElementHasAudioPreRoll;
} CSUsacExtElementConfig;

typedef struct {
  MP4_ELEMENT_ID usacElementType;
  uint8_t m_noiseFilling;
  uint8_t m_harmonicSBR;
  uint8_t m_interTes;
  uint8_t m_pvc;
  uint8_t m_stereoConfigIndex;
  CSUsacExtElementConfig extElement;
} CSUsacElementConfig;

typedef struct {
  uint8_t m_frameLengthFlag;
  uint8_t m_coreSbrFrameLengthIndex;
  uint8_t m_sbrRatioIndex;
  uint8_t m_nUsacChannels; /* number of audio channels signaled in
                            UsacDecoderConfig() / rsv603daDecoderConfig() via
                            numElements and usacElementType */
  uint8_t m_channelConfigurationIndex;
  uint32_t m_usacNumElements;
  CSUsacElementConfig element[TP_USAC_MAX_ELEMENTS];

  uint8_t numAudioChannels;
  uint8_t m_usacConfigExtensionPresent;
  uint8_t elementLengthPresent;
  uint8_t UsacConfig[TP_USAC_MAX_CONFIG_LEN];
  uint16_t UsacConfigBits;
} CSUsacConfig;

/**
 * Audio configuration struct, suitable for encoder and decoder configuration.
 */
typedef struct {
  /* XYZ Specific Data */
  union {
    CSGaSpecificConfig
        m_gaSpecificConfig; /**< General audio specific configuration. */
    CSEldSpecificConfig m_eldSpecificConfig; /**< ELD specific configuration. */
    CSUsacConfig m_usacConfig; /**< USAC specific configuration               */
  } m_sc;

  /* Common ASC parameters */
  CProgramConfig m_progrConfigElement; /**< Program configuration. */

  AUDIO_OBJECT_TYPE m_aot;  /**< Audio Object Type.  */
  uint32_t m_samplingFrequency; /**< Samplerate. */
  uint32_t m_samplesPerFrame;   /**< Amount of samples per frame.   */
  uint32_t m_directMapping; /**< Document this please !!                         */

  AUDIO_OBJECT_TYPE m_extensionAudioObjectType; /**< Audio object type */
  uint32_t m_extensionSamplingFrequency;            /**< Samplerate            */

  int8_t m_channelConfiguration; /**< Channel configuration index */

  int8_t m_epConfig;  /**< Error protection index                           */
  int8_t m_vcb11Flag; /**< aacSectionDataResilienceFlag                     */
  int8_t m_rvlcFlag;  /**< aacScalefactorDataResilienceFlag                 */
  int8_t m_hcrFlag;   /**< aacSpectralDataResilienceFlag                    */

  int8_t m_sbrPresentFlag; /**< Flag indicating the presence of SBR data in the
                             bitstream               */
  int8_t
  m_psPresentFlag; /**< Flag indicating the presence of parametric stereo
                      data in the bitstream */
  uint8_t m_samplingFrequencyIndex;          /**< Samplerate index          */
  uint8_t m_extensionSamplingFrequencyIndex; /**< Samplerate index */
  int8_t m_extensionChannelConfiguration;   /**< Channel configuration index   */

  uint8_t
  configMode; /**< The flag indicates if the callback shall work in memory
                 allocation mode or in config change detection mode */
  uint8_t AacConfigChanged; /**< The flag will be set if at least one aac config
                             parameter has changed that requires a memory
                             reconfiguration, otherwise it will be cleared */
  uint8_t SbrConfigChanged; /**< The flag will be set if at least one sbr config
                             parameter has changed that requires a memory
                             reconfiguration, otherwise it will be cleared */
  uint8_t SacConfigChanged; /**< The flag will be set if at least one sac config
                             parameter has changed that requires a memory
                             reconfiguration, otherwise it will be cleared */

  uint8_t
  config[TP_USAC_MAX_CONFIG_LEN]; /**< Configuration stored as bitstream */
  uint32_t configBits;                /**< Configuration length in bits */

} CSAudioSpecificConfig;

typedef struct {
  int8_t flushCnt;      /**< Flush frame counter */
  uint8_t flushStatus;   /**< Flag indicates flush mode: on|off */
  int8_t buildUpCnt;    /**< Build up frame counter */
  uint8_t buildUpStatus; /**< Flag indicates build up mode: on|off */
  uint8_t cfgChanged; /**< Flag indicates that the config changed and the decoder
                       needs to be initialized again via callback. Make sure
                       that memory is freed before initialization. */
  uint8_t contentChanged; /**< Flag indicates that the content changed i.e. a
                           right truncation occured before */
  uint8_t forceCfgChange; /**< Flag indicates if config change has to be forced
                           even if new config is the same */
} CCtrlCFGChange;

typedef int32_t (*cbUpdateConfig_t)(void *, const CSAudioSpecificConfig *,
                                const uint8_t configMode, uint8_t *configChanged);
typedef int32_t (*cbFreeMem_t)(void *, const CSAudioSpecificConfig *);
typedef int32_t (*cbCtrlCFGChange_t)(void *, const CCtrlCFGChange *);
typedef int32_t (*cbSsc_t)(void *, HANDLE_FDK_BITSTREAM,
                       const AUDIO_OBJECT_TYPE coreCodec,
                       const int32_t samplingRate, const int32_t frameSize,
                       const int32_t stereoConfigIndex,
                       const int32_t coreSbrFrameLengthIndex, const int32_t configBytes,
                       const uint8_t configMode, uint8_t *configChanged);

typedef int32_t (*cbSbr_t)(void *self, HANDLE_FDK_BITSTREAM hBs,
                       const int32_t sampleRateIn, const int32_t sampleRateOut,
                       const int32_t samplesPerFrame,
                       const AUDIO_OBJECT_TYPE coreCodec,
                       const MP4_ELEMENT_ID elementID, const int32_t elementIndex,
                       const uint8_t harmonicSbr, const uint8_t stereoConfigIndex,
                       const uint8_t configMode, uint8_t *configChanged,
                       const int32_t downscaleFactor);

typedef int32_t (*cbUsac_t)(void *self, HANDLE_FDK_BITSTREAM hBs);

typedef int32_t (*cbUniDrc_t)(void *self, HANDLE_FDK_BITSTREAM hBs,
                          const int32_t fullPayloadLength, const int32_t payloadType,
                          const int32_t subStreamIndex, const int32_t payloadStart,
                          const AUDIO_OBJECT_TYPE);

typedef struct {
  cbUpdateConfig_t cbUpdateConfig; /*!< Function pointer for Config change
                                      notify callback.  */
  void *cbUpdateConfigData; /*!< User data pointer for Config change notify
                               callback. */
  cbFreeMem_t cbFreeMem;    /*!< Function pointer for free memory callback.  */
  void *cbFreeMemData;      /*!< User data pointer for free memory callback. */
  cbCtrlCFGChange_t cbCtrlCFGChange; /*!< Function pointer for config change
                                        control callback. */
  void *cbCtrlCFGChangeData; /*!< User data pointer for config change control
                                callback. */
  cbSsc_t cbSsc;             /*!< Function pointer for SSC parser callback. */
  void *cbSscData;           /*!< User data pointer for SSC parser callback. */
  cbSbr_t cbSbr;   /*!< Function pointer for SBR header parser callback. */
  void *cbSbrData; /*!< User data pointer for SBR header parser callback. */
  cbUsac_t cbUsac;
  void *cbUsacData;
  cbUniDrc_t cbUniDrc; /*!< Function pointer for uniDrcConfig and
                          loudnessInfoSet parser callback. */
  void *cbUniDrcData;  /*!< User data pointer for uniDrcConfig and
                          loudnessInfoSet parser callback. */
} CSTpCallBacks;

static const uint32_t SamplingRateTable[] = {
    96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025,
    8000,  7350,  0,     0,     57600, 51200, 40000, 38400, 34150, 28800, 25600,
    20000, 19200, 17075, 14400, 12800, 9600,  0,     0,     0,     0};

static inline int32_t getSamplingRateIndex(uint32_t samplingRate, uint32_t nBits) {
  uint32_t sf_index;
  uint32_t tableSize = (1 << nBits) - 1;

  for (sf_index = 0; sf_index < tableSize; sf_index++) {
    if (SamplingRateTable[sf_index] == samplingRate) break;
  }

  if (sf_index > tableSize) {
    return tableSize - 1;
  }

  return sf_index;
}

/*
 * Get Channel count from channel configuration
 */
static inline int32_t getNumberOfTotalChannels(int32_t channelConfig) {
  switch (channelConfig) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      return channelConfig;
    case 7:
    case 12:
    case 14:
      return 8;
    case 11:
      return 7;
    case 13:
      return 24;
    default:
      return 0;
  }
}

static inline int32_t getNumberOfEffectiveChannels(
    const int32_t
        channelConfig) { /* index: 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 */
  const int32_t n[] = {0, 1, 2, 3, 4, 5, 5, 7, 0, 0, 0, 6, 7, 22, 7, 0};
  return n[channelConfig];
}

#endif /* TP_DATA_H */
