

/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#ifndef DRCDEC_TYPES_H
#define DRCDEC_TYPES_H

#include <stdint.h>
#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/common_fix.h"

/* Data structures corresponding to static and dynamic DRC/Loudness payload
   as defined in section 7 of MPEG-D DRC standard, ISO/IEC 23003-4 */

/**************/
/* uniDrcGain */
/**************/

typedef struct {
  int16_t gainDb; /* e = 7 */
  int16_t time;
} GAIN_NODE;

/* uniDrcGainExtension() (Table 56) */
typedef struct {
  uint8_t uniDrcGainExtType[8];
  uint32_t extBitSize[8 - 1];
} UNI_DRC_GAIN_EXTENSION;

/* uniDrcGain() (Table 55) */
typedef struct {
  uint8_t nNodes[12]; /* unsaturated value, i.e. as provided in bitstream */
  GAIN_NODE gainNode[12][16];

  uint8_t uniDrcGainExtPresent;
  UNI_DRC_GAIN_EXTENSION uniDrcGainExtension;

  /* derived data */
  uint8_t status;
} UNI_DRC_GAIN, *HANDLE_UNI_DRC_GAIN;

/****************/
/* uniDrcConfig */
/****************/

typedef enum {
  EB_NIGHT = 0x0001,
  EB_NOISY = 0x0002,
  EB_LIMITED = 0x0004,
  EB_LOWLEVEL = 0x0008,
  EB_DIALOG = 0x0010,
  EB_GENERAL_COMPR = 0x0020,
  EB_EXPAND = 0x0040,
  EB_ARTISTIC = 0x0080,
  EB_CLIPPING = 0x0100,
  EB_FADE = 0x0200,
  EB_DUCK_OTHER = 0x0400,
  EB_DUCK_SELF = 0x0800
} EFFECT_BIT;

typedef enum {
  GCP_REGULAR = 0,
  GCP_FADING = 1,
  GCP_CLIPPING_DUCKING = 2,
  GCP_CONSTANT = 3
} GAIN_CODING_PROFILE;

typedef enum { GIT_SPLINE = 0, GIT_LINEAR = 1 } GAIN_INTERPOLATION_TYPE;

typedef enum { CS_LEFT = 0, CS_RIGHT = 1 } CHARACTERISTIC_SIDE;

typedef enum { CF_SIGMOID = 0, CF_NODES = 1 } CHARACTERISTIC_FORMAT;

typedef enum {
  GF_QMF32 = 0x1,
  GF_QMFHYBRID39 = 0x2,
  GF_QMF64 = 0x3,
  GF_QMFHYBRID71 = 0x4,
  GF_QMF128 = 0x5,
  GF_QMFHYBRID135 = 0x6,
  GF_UNIFORM = 0x7
} EQ_SUBBAND_GAIN_FORMAT;

typedef struct {
  uint8_t duckingScalingPresent;
  int16_t duckingScaling; /* e = 2 */
} DUCKING_MODIFICATION;

typedef struct {
  uint8_t targetCharacteristicLeftPresent;
  uint8_t targetCharacteristicLeftIndex;
  uint8_t targetCharacteristicRightPresent;
  uint8_t targetCharacteristicRightIndex;
  uint8_t gainScalingPresent;
  int16_t attenuationScaling;   /* e = 2 */
  int16_t amplificationScaling; /* e = 2 */
  uint8_t gainOffsetPresent;
  int16_t gainOffset; /* e = 4 */
} GAIN_MODIFICATION;

typedef union {
  uint8_t crossoverFreqIndex;
  uint16_t startSubBandIndex;
} BAND_BORDER;

typedef struct {
  uint8_t left;
  uint8_t right;
} CUSTOM_INDEX;

typedef struct {
  uint8_t present;
  uint8_t isCICP;
  union {
    uint8_t cicpIndex;
    CUSTOM_INDEX custom;
  };
} DRC_CHARACTERISTIC;

typedef struct {
  uint8_t gainCodingProfile;
  uint8_t gainInterpolationType;
  uint8_t fullFrame;
  uint8_t timeAlignment;
  uint8_t timeDeltaMinPresent;
  uint16_t timeDeltaMin;
  uint8_t bandCount;
  uint8_t drcBandType;
  uint8_t gainSequenceIndex[4];
  DRC_CHARACTERISTIC drcCharacteristic[4];
  BAND_BORDER bandBorder[4];
} GAIN_SET;

typedef struct {
  int16_t gain;    /* e = 6 */
  int16_t ioRatio; /* e = 2 */
  int16_t exp;     /* e = 5 */
  uint8_t flipSign;
} CUSTOM_DRC_CHAR_SIGMOID;

typedef struct {
  uint8_t characteristicNodeCount;
  int16_t nodeLevel[4 + 1]; /* e = 7 */
  int16_t nodeGain[4 + 1];  /* e = 7 */
} CUSTOM_DRC_CHAR_NODES;

typedef shouldBeUnion {
  CUSTOM_DRC_CHAR_SIGMOID sigmoid;
  CUSTOM_DRC_CHAR_NODES nodes;
}
CUSTOM_DRC_CHAR;

/* drcCoefficientsUniDrc() (Table 67) */
typedef struct {
  uint8_t drcLocation;
  uint8_t drcFrameSizePresent;
  uint16_t drcFrameSize;
  uint8_t characteristicLeftCount;
  uint8_t characteristicLeftFormat[16];
  CUSTOM_DRC_CHAR customCharacteristicLeft[16];
  uint8_t characteristicRightCount;
  uint8_t characteristicRightFormat[16];
  CUSTOM_DRC_CHAR customCharacteristicRight[16];
  uint8_t
  gainSequenceCount;  /* unsaturated value, i.e. as provided in bitstream */
  uint8_t gainSetCount; /* saturated to 12 */
  GAIN_SET gainSet[12];
  /* derived data */
  uint8_t gainSetIndexForGainSequence[12];
} DRC_COEFFICIENTS_UNI_DRC;

/* drcInstructionsUniDrc() (Table 72) */
typedef struct {
  int8_t drcSetId;
  uint8_t drcSetComplexityLevel;
  uint8_t drcLocation;
  uint8_t drcApplyToDownmix;
  uint8_t downmixIdCount;
  uint8_t downmixId[8];
  uint16_t drcSetEffect;
  uint8_t limiterPeakTargetPresent;
  int16_t limiterPeakTarget; /* e = 5 */
  uint8_t drcSetTargetLoudnessPresent;
  int8_t drcSetTargetLoudnessValueUpper;
  int8_t drcSetTargetLoudnessValueLower;
  uint8_t dependsOnDrcSetPresent;
  union {
    int8_t dependsOnDrcSet;
    uint8_t noIndependentUse;
  };
  uint8_t requiresEq;
  shouldBeUnion {
    GAIN_MODIFICATION gainModificationForChannelGroup[8][4];
    DUCKING_MODIFICATION duckingModificationForChannel[8];
  };
  int8_t gainSetIndex[8];

  /* derived data */
  uint8_t drcChannelCount;
  uint8_t nDrcChannelGroups;
  int8_t gainSetIndexForChannelGroup[8];
} DRC_INSTRUCTIONS_UNI_DRC;

/* channelLayout() (Table 62) */
typedef struct {
  uint8_t baseChannelCount;
  uint8_t layoutSignalingPresent;
  uint8_t definedLayout;
  uint8_t speakerPosition[8];
} CHANNEL_LAYOUT;

/* downmixInstructions() (Table 63) */
typedef struct {
  uint8_t downmixId;
  uint8_t targetChannelCount;
  uint8_t targetLayout;
  uint8_t downmixCoefficientsPresent;
  uint8_t bsDownmixOffset;
  int32_t downmixCoefficient[8 * 8]; /* e = 2 */
} DOWNMIX_INSTRUCTIONS;

typedef struct {
  uint8_t uniDrcConfigExtType[8];
  uint32_t extBitSize[8 - 1];
} UNI_DRC_CONFIG_EXTENSION;

/* uniDrcConfig() (Table 57) */
typedef struct {
  uint8_t sampleRatePresent;
  uint32_t sampleRate;
  uint8_t downmixInstructionsCountV0;
  uint8_t downmixInstructionsCountV1;
  uint8_t downmixInstructionsCount; /* saturated to 6 */
  uint8_t drcCoefficientsUniDrcCountV0;
  uint8_t drcCoefficientsUniDrcCountV1;
  uint8_t drcCoefficientsUniDrcCount; /* saturated to 2 */
  uint8_t drcInstructionsUniDrcCountV0;
  uint8_t drcInstructionsUniDrcCountV1;
  uint8_t drcInstructionsUniDrcCount; /* saturated to (12 + 1 + 6) */
  CHANNEL_LAYOUT channelLayout;
  DOWNMIX_INSTRUCTIONS downmixInstructions[6];
  DRC_COEFFICIENTS_UNI_DRC drcCoefficientsUniDrc[2];
  DRC_INSTRUCTIONS_UNI_DRC drcInstructionsUniDrc[(12 + 1 + 6)];
  uint8_t uniDrcConfigExtPresent;
  UNI_DRC_CONFIG_EXTENSION uniDrcConfigExt;

  /* derived data */
  uint8_t drcInstructionsCountInclVirtual;
  uint8_t diff;
} UNI_DRC_CONFIG, *HANDLE_UNI_DRC_CONFIG;

/*******************/
/* loudnessInfoSet */
/*******************/

typedef enum {
  MD_UNKNOWN_OTHER = 0,
  MD_PROGRAM_LOUDNESS = 1,
  MD_ANCHOR_LOUDNESS = 2,
  MD_MAX_OF_LOUDNESS_RANGE = 3,
  MD_MOMENTARY_LOUDNESS_MAX = 4,
  MD_SHORT_TERM_LOUDNESS_MAX = 5,
  MD_LOUDNESS_RANGE = 6,
  MD_MIXING_LEVEL = 7,
  MD_ROOM_TYPE = 8,
  MD_SHORT_TERM_LOUDNESS = 9
} METHOD_DEFINITION;

typedef enum {
  MS_UNKNOWN_OTHER = 0,
  MS_EBU_R_128 = 1,
  MS_BS_1770_4 = 2,
  MS_BS_1770_4_PRE_PROCESSING = 3,
  MS_USER = 4,
  MS_EXPERT_PANEL = 5,
  MS_BS_1771_1 = 6,
  MS_RESERVED_A = 7,
  MS_RESERVED_B = 8,
  MS_RESERVED_C = 9,
  MS_RESERVED_D = 10,
  MS_RESERVED_E = 11
} MEASUREMENT_SYSTEM;

typedef enum {
  R_UKNOWN = 0,
  R_UNVERIFIED = 1,
  R_CEILING = 2,
  R_ACCURATE = 3
} RELIABILITY;

typedef struct {
  uint8_t methodDefinition;
  int32_t methodValue; /* e = 7 for all methodDefinitions */
  uint8_t measurementSystem;
  uint8_t reliability;
} LOUDNESS_MEASUREMENT;

/* loudnessInfo() (Table 59) */
typedef struct {
  int8_t drcSetId;
  uint8_t eqSetId;
  uint8_t downmixId;
  uint8_t samplePeakLevelPresent;
  int32_t samplePeakLevel; /* e = 7 */
  uint8_t truePeakLevelPresent;
  int32_t truePeakLevel; /* e = 7 */
  uint8_t truePeakLevelMeasurementSystem;
  uint8_t truePeakLevelReliability;
  uint8_t measurementCount; /* saturated to 8 */
  LOUDNESS_MEASUREMENT loudnessMeasurement[8];
} LOUDNESS_INFO;

/* loudnessInfoSetExtension() (Table 61) */
typedef struct {
  uint8_t loudnessInfoSetExtType[8];
  uint32_t extBitSize[8 - 1];
} LOUDNESS_INFO_SET_EXTENSION;

/* loudnessInfoSet() (Table 58) */
typedef struct {
  uint8_t loudnessInfoAlbumCountV0;
  uint8_t loudnessInfoAlbumCountV1;
  uint8_t loudnessInfoAlbumCount; /* saturated to 12 */
  uint8_t loudnessInfoCountV0;
  uint8_t loudnessInfoCountV1;
  uint8_t loudnessInfoCount; /* saturated to 12 */
  LOUDNESS_INFO loudnessInfoAlbum[12];
  LOUDNESS_INFO loudnessInfo[12];
  uint8_t loudnessInfoSetExtPresent;
  LOUDNESS_INFO_SET_EXTENSION loudnessInfoSetExt;
  /* derived data */
  uint8_t diff;
} LOUDNESS_INFO_SET, *HANDLE_LOUDNESS_INFO_SET;

#endif
