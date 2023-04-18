

/******************* MPEG transport format decoder library *********************

   Author(s):   Manuel Jander

   Description: MPEG Transport data tables

*******************************************************************************/

#ifndef TP_DATA_H
#define TP_DATA_H

#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/FDK_bitstream.h"

/*
 * Configuration
 */



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

typedef enum {
  ELDEXT_TERM = 0x0,         /* Termination tag */
  ELDEXT_SAOC = 0x1,         /* SAOC config */
  ELDEXT_LDSAC = 0x2,        /* LD MPEG Surround config */
  ELDEXT_DOWNSCALEINFO = 0x3 /* ELD sample rate adaptation */
  /* reserved */
} ASC_ELD_EXT_TYPE;

/**
 * Audio configuration struct, suitable for encoder and decoder configuration.
 */


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
