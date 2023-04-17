

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: interface - spatial specific config struct

*******************************************************************************/

#ifndef SAC_DEC_SSC_STRUCT_H
#define SAC_DEC_SSC_STRUCT_H

#include "../libAACdec/newAACDecoder.h"

#define MAX_NUM_QMF_BANDS (128)
#define MAX_TIME_SLOTS 64
#define MAX_INPUT_CHANNELS 1
#define MAX_OUTPUT_CHANNELS                                             \
  2 /* CAUTION: This does NOT  the number of                    \
                            output channels exclusively! In addition it \
       affects the max number of bitstream and residual channels! */
#define MAX_NUM_OTT (5)
#define MAX_NUM_TTT (0)
#define MAX_NUM_EXT_TYPES (8)
#define MAX_PARAMETER_BANDS (28)
#define MAX_PARAMETER_BANDS_LD (23)

#define MAX_NUM_XCHANNELS (6)

#define MAX_ARBITRARY_TREE_LEVELS (0)

typedef enum {
  /* CAUTION: Do not change enum values! */
  SPATIALDEC_FREQ_RES_40 = 40,
  SPATIALDEC_FREQ_RES_28 = 28,
  SPATIALDEC_FREQ_RES_23 = 23,
  SPATIALDEC_FREQ_RES_20 = 20,
  SPATIALDEC_FREQ_RES_15 = 15,
  SPATIALDEC_FREQ_RES_14 = 14,
  SPATIALDEC_FREQ_RES_12 = 12,
  SPATIALDEC_FREQ_RES_10 = 10,
  SPATIALDEC_FREQ_RES_9 = 9,
  SPATIALDEC_FREQ_RES_7 = 7,
  SPATIALDEC_FREQ_RES_5 = 5,
  SPATIALDEC_FREQ_RES_4 = 4

} SPATIALDEC_FREQ_RES;

typedef enum {

  SPATIALDEC_QUANT_FINE_DEF = 0,
  SPATIALDEC_QUANT_EDQ1 = 1,
  SPATIALDEC_QUANT_EDQ2 = 2,
  SPATIALDEC_QUANT_RSVD3 = 3,
  SPATIALDEC_QUANT_RSVD4 = 4,
  SPATIALDEC_QUANT_RSVD5 = 5,
  SPATIALDEC_QUANT_RSVD6 = 6,
  SPATIALDEC_QUANT_RSVD7 = 7

} SPATIALDEC_QUANT_MODE;

typedef enum { SPATIALDEC_MODE_RSVD7 = 7 } SPATIALDEC_TREE_CONFIG;

typedef enum {

  SPATIALDEC_GAIN_MODE0 = 0,
  SPATIALDEC_GAIN_RSVD1 = 1,
  SPATIALDEC_GAIN_RSVD2 = 2,
  SPATIALDEC_GAIN_RSVD3 = 3,
  SPATIALDEC_GAIN_RSVD4 = 4,
  SPATIALDEC_GAIN_RSVD5 = 5,
  SPATIALDEC_GAIN_RSVD6 = 6,
  SPATIALDEC_GAIN_RSVD7 = 7,
  SPATIALDEC_GAIN_RSVD8 = 8,
  SPATIALDEC_GAIN_RSVD9 = 9,
  SPATIALDEC_GAIN_RSVD10 = 10,
  SPATIALDEC_GAIN_RSVD11 = 11,
  SPATIALDEC_GAIN_RSVD12 = 12,
  SPATIALDEC_GAIN_RSVD13 = 13,
  SPATIALDEC_GAIN_RSVD14 = 14,
  SPATIALDEC_GAIN_RSVD15 = 15

} SPATIALDEC_FIXED_GAINS;

typedef enum {

  SPATIALDEC_TS_TPNOWHITE = 0,
  SPATIALDEC_TS_TPWHITE = 1,
  SPATIALDEC_TS_TES = 2,
  SPATIALDEC_TS_NOTS = 3,
  SPATIALDEC_TS_RSVD4 = 4,
  SPATIALDEC_TS_RSVD5 = 5,
  SPATIALDEC_TS_RSVD6 = 6,
  SPATIALDEC_TS_RSVD7 = 7,
  SPATIALDEC_TS_RSVD8 = 8,
  SPATIALDEC_TS_RSVD9 = 9,
  SPATIALDEC_TS_RSVD10 = 10,
  SPATIALDEC_TS_RSVD11 = 11,
  SPATIALDEC_TS_RSVD12 = 12,
  SPATIALDEC_TS_RSVD13 = 13,
  SPATIALDEC_TS_RSVD14 = 14,
  SPATIALDEC_TS_RSVD15 = 15

} SPATIALDEC_TS_CONF;

typedef enum {

  SPATIALDEC_DECORR_MODE0 = 0,
  SPATIALDEC_DECORR_MODE1 = 1,
  SPATIALDEC_DECORR_MODE2 = 2,
  SPATIALDEC_DECORR_RSVD3 = 3,
  SPATIALDEC_DECORR_RSVD4 = 4,
  SPATIALDEC_DECORR_RSVD5 = 5,
  SPATIALDEC_DECORR_RSVD6 = 6,
  SPATIALDEC_DECORR_RSVD7 = 7,
  SPATIALDEC_DECORR_RSVD8 = 8,
  SPATIALDEC_DECORR_RSVD9 = 9,
  SPATIALDEC_DECORR_RSVD10 = 10,
  SPATIALDEC_DECORR_RSVD11 = 11,
  SPATIALDEC_DECORR_RSVD12 = 12,
  SPATIALDEC_DECORR_RSVD13 = 13,
  SPATIALDEC_DECORR_RSVD14 = 14,
  SPATIALDEC_DECORR_RSVD15 = 15

} SPATIALDEC_DECORR_CONF;

typedef struct T_SPATIALDEC_OTT_CONF {
  int32_t nOttBands;

} SPATIALDEC_OTT_CONF;

typedef struct T_SPATIALDEC_RESIDUAL_CONF {
  int32_t bResidualPresent;
  int32_t nResidualBands;

} SPATIALDEC_RESIDUAL_CONF;

typedef struct T_SPATIAL_SPECIFIC_CONFIG {
  uint32_t syntaxFlags;
  int32_t samplingFreq;
  int32_t nTimeSlots;
  SPATIALDEC_FREQ_RES freqRes;
  SPATIALDEC_TREE_CONFIG treeConfig;
  SPATIALDEC_QUANT_MODE quantMode;
  int32_t bArbitraryDownmix;

  int32_t bResidualCoding;
  SPATIALDEC_FIXED_GAINS bsFixedGainDMX;

  SPATIALDEC_TS_CONF tempShapeConfig;
  SPATIALDEC_DECORR_CONF decorrConfig;

  int32_t nInputChannels;  /* derived from  treeConfig */
  int32_t nOutputChannels; /* derived from  treeConfig */

  /* ott config */
  int32_t nOttBoxes;                              /* derived from  treeConfig */
  SPATIALDEC_OTT_CONF OttConfig[MAX_NUM_OTT]; /* dimension nOttBoxes */

  /* ttt config */
  int32_t nTttBoxes; /* derived from  treeConfig */

  /* residual config */
  SPATIALDEC_RESIDUAL_CONF
  ResidualConfig[MAX_NUM_OTT +
                 MAX_NUM_TTT]; /* dimension (nOttBoxes + nTttBoxes) */

  int32_t sacExtCnt;
  int32_t sacExtType[MAX_NUM_EXT_TYPES];
  int32_t envQuantMode;

  AUDIO_OBJECT_TYPE_t coreCodec;

  uint8_t stereoConfigIndex;
  uint8_t coreSbrFrameLengthIndex; /* Table 70 in ISO/IEC FDIS 23003-3:2011 */
  uint8_t bsHighRateMode;
  uint8_t bsDecorrType;
  uint8_t bsPseudoLr;
  uint8_t bsPhaseCoding;
  uint8_t bsOttBandsPhasePresent;
  int32_t bsOttBandsPhase;

  int8_t ottCLDdefault[MAX_NUM_OTT];
  uint8_t numOttBandsIPD;
  uint8_t bitstreamOttBands[MAX_NUM_OTT];
  uint8_t numOttBands[MAX_NUM_OTT];

} SPATIAL_SPECIFIC_CONFIG;

#endif
