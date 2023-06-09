

/**************************** PCM utility library ******************************

   Author(s):   Christian Griebel

   Description: Defines functions that perform downmixing or a simple channel
                expansion in the PCM time domain.

*******************************************************************************/

#include <memory.h>
#include <assert.h>
#include "../libAACdec/newAACDecoder.h"
#include "pcmdmx_lib.h"

#include "../libFDK/fixpoint_math.h"
#include "../libFDK/FDK_core.h"

/* library version */
#include "version.h"
/* library title */
#define PCMDMX_LIB_TITLE "PCM Downmix Lib"

#define FALSE 0
#define TRUE 1
#define IN 0
#define OUT 1


/* Fixed and unique channel group indices.
 * The last group index has to be smaller than ( 4 ). */
#define CH_GROUP_FRONT (0)
#define CH_GROUP_SIDE (1)
#define CH_GROUP_REAR (2)
#define CH_GROUP_LFE (3)

/* Fixed and unique channel plain indices. */
#define CH_PLAIN_NORMAL (0)
#define CH_PLAIN_TOP (1)
#define CH_PLAIN_BOTTOM (2)

/* The ordering of the following fixed channel labels has to be in MPEG-4 style.
 * From the center to the back with left and right channel interleaved (starting
 * with left). The last channel label index has to be smaller than ( 8 ). */
#define CENTER_FRONT_CHANNEL (0) /* C  */
#define LEFT_FRONT_CHANNEL (1)   /* L  */
#define RIGHT_FRONT_CHANNEL (2)  /* R  */
#define LEFT_REAR_CHANNEL \
  (3) /* Lr (aka left back channel) or center back channel */
#define RIGHT_REAR_CHANNEL (4)      /* Rr (aka right back channel) */
#define LOW_FREQUENCY_CHANNEL (5)   /* Lf */
#define LEFT_MULTIPRPS_CHANNEL (6)  /* Left multipurpose channel */
#define RIGHT_MULTIPRPS_CHANNEL (7) /* Right multipurpose channel */

/* 22.2 channel specific fixed channel lables: */
#define LEFT_SIDE_CHANNEL (8)            /* Lss  */
#define RIGHT_SIDE_CHANNEL (9)           /* Rss  */
#define CENTER_REAR_CHANNEL (10)         /* Cs   */
#define CENTER_FRONT_CHANNEL_TOP (11)    /* Cv   */
#define LEFT_FRONT_CHANNEL_TOP (12)      /* Lv   */
#define RIGHT_FRONT_CHANNEL_TOP (13)     /* Rv   */
#define LEFT_SIDE_CHANNEL_TOP (14)       /* Lvss */
#define RIGHT_SIDE_CHANNEL_TOP (15)      /* Rvss */
#define CENTER_SIDE_CHANNEL_TOP (16)     /* Ts   */
#define LEFT_REAR_CHANNEL_TOP (17)       /* Lvr  */
#define RIGHT_REAR_CHANNEL_TOP (18)      /* Rvr  */
#define CENTER_REAR_CHANNEL_TOP (19)     /* Cvr  */
#define CENTER_FRONT_CHANNEL_BOTTOM (20) /* Cb   */
#define LEFT_FRONT_CHANNEL_BOTTOM (21)   /* Lb   */
#define RIGHT_FRONT_CHANNEL_BOTTOM (22)  /* Rb   */
#define LOW_FREQUENCY_CHANNEL_2 (23)     /* LFE2 */

/* More constants */
#define ONE_CHANNEL (1)
#define TWO_CHANNEL (2)
#define SIX_CHANNEL (6)
#define EIGHT_CHANNEL (8)
#define TWENTY_FOUR_CHANNEL (24)

#define PCMDMX_THRESHOLD_MAP_HEAT_1 (0) /* Store only exact matches */
#define PCMDMX_THRESHOLD_MAP_HEAT_2 (20)
#define PCMDMX_THRESHOLD_MAP_HEAT_3 \
  (256) /* Do not assign normal channels to LFE */

#define SP_Z_NRM (0)
#define SP_Z_TOP (2)
#define SP_Z_BOT (-2)
#define SP_Z_LFE (-18)
#define SP_Z_MUL (8) /* Should be smaller than SP_Z_LFE */

typedef struct {
  int8_t x; /* horizontal position:  center (0), left (-), right (+) */
  int8_t y; /* deepth position:      front, side, back, position */
  int8_t z; /* heigth positions:     normal, top, bottom, lfe */
} PCM_DMX_SPEAKER_POSITION;

/* CAUTION: The maximum x-value should be less or equal to
 * PCMDMX_SPKR_POS_X_MAX_WIDTH. */
static const PCM_DMX_SPEAKER_POSITION spkrSlotPos[] = {
    /*  x,  y,  z  */
    {0, 0, SP_Z_NRM},  /* 0  CENTER_FRONT_CHANNEL        */
    {-2, 0, SP_Z_NRM}, /* 1  LEFT_FRONT_CHANNEL          */
    {2, 0, SP_Z_NRM},  /* 2  RIGHT_FRONT_CHANNEL         */
    {-3, 4, SP_Z_NRM}, /* 3  LEFT_REAR_CHANNEL           */
    {3, 4, SP_Z_NRM},  /* 4  RIGHT_REAR_CHANNEL          */
    {0, 0, SP_Z_LFE},  /* 5  LOW_FREQUENCY_CHANNEL       */
    {-2, 2, SP_Z_MUL}, /* 6  LEFT_MULTIPRPS_CHANNEL      */
    {2, 2, SP_Z_MUL}   /* 7  RIGHT_MULTIPRPS_CHANNEL     */
};

/* List of packed channel modes */
typedef enum { /* CH_MODE_<numFrontCh>_<numSideCh>_<numBackCh>_<numLfCh> */
               CH_MODE_UNDEFINED = 0x0000,
               /* 1 channel */
               CH_MODE_1_0_0_0 = 0x0001, /* chCfg 1 */
               /* 2 channels */
               CH_MODE_2_0_0_0 = 0x0002 /* chCfg 2 */
                                        /* 3 channels */
               ,
               CH_MODE_3_0_0_0 = 0x0003, /* chCfg 3 */
               CH_MODE_2_0_1_0 = 0x0102,
               CH_MODE_2_0_0_1 = 0x1002,
               /* 4 channels */
               CH_MODE_3_0_1_0 = 0x0103, /* chCfg 4 */
               CH_MODE_2_0_2_0 = 0x0202,
               CH_MODE_2_0_1_1 = 0x1102,
               CH_MODE_4_0_0_0 = 0x0004,
               /* 5 channels */
               CH_MODE_3_0_2_0 = 0x0203, /* chCfg 5 */
               CH_MODE_2_0_2_1 = 0x1202,
               CH_MODE_3_0_1_1 = 0x1103,
               CH_MODE_3_2_0_0 = 0x0023,
               CH_MODE_5_0_0_0 = 0x0005,
               /* 6 channels */
               CH_MODE_3_0_2_1 = 0x1203, /* chCfg 6 */
               CH_MODE_3_2_0_1 = 0x1023,
               CH_MODE_3_2_1_0 = 0x0123,
               CH_MODE_5_0_1_0 = 0x0105,
               CH_MODE_6_0_0_0 = 0x0006,
               /* 7 channels */
               CH_MODE_2_2_2_1 = 0x1222,
               CH_MODE_3_0_3_1 = 0x1303, /* chCfg 11 */
               CH_MODE_3_2_1_1 = 0x1123,
               CH_MODE_3_2_2_0 = 0x0223,
               CH_MODE_3_0_2_2 = 0x2203,
               CH_MODE_5_0_2_0 = 0x0205,
               CH_MODE_5_0_1_1 = 0x1105,
               CH_MODE_7_0_0_0 = 0x0007,
               /* 8 channels */
               CH_MODE_3_2_2_1 = 0x1223,
               CH_MODE_3_0_4_1 = 0x1403, /* chCfg 12 */
               CH_MODE_5_0_2_1 = 0x1205, /* chCfg 7 + 14 */
               CH_MODE_5_2_1_0 = 0x0125,
               CH_MODE_3_2_1_2 = 0x2123,
               CH_MODE_2_2_2_2 = 0x2222,
               CH_MODE_3_0_3_2 = 0x2303,
               CH_MODE_8_0_0_0 = 0x0008

} PCM_DMX_CHANNEL_MODE;

/* These are the channel configurations linked to
   the number of output channels give by the user: */
static const PCM_DMX_CHANNEL_MODE outChModeTable[(8) + 1] = {
    CH_MODE_UNDEFINED,
    CH_MODE_1_0_0_0, /* 1 channel  */
    CH_MODE_2_0_0_0  /* 2 channels */
    ,
    CH_MODE_3_0_0_0, /* 3 channels */
    CH_MODE_3_0_1_0, /* 4 channels */
    CH_MODE_3_0_2_0, /* 5 channels */
    CH_MODE_3_0_2_1  /* 6 channels */
    ,
    CH_MODE_3_0_3_1, /* 7 channels */
    CH_MODE_3_0_4_1  /* 8 channels */
};

static const int16_t abMixLvlValueTab[8] = {
      16384, /* scaled by 1 */
      27558,   23167,   19530,
      16384,   13828,   11633,
      0};

static const int16_t lfeMixLvlValueTab[16] = {
    /*             value,        scale */
      25903, /*     2 */
      16384, /*     2 */
      27509, /*     1 */
      23151, /*     1 */
      19481, /*     1 */
      16384,  /*     1 */
      27558,  /*     0 */
      23167,  /*     0 */
      19530,  /*     0 */
      16384,  /*     0 */
      10355,  /*     0 */
      5833,  /*     0 */
      3277,  /*     0 */
      1049,  /*     0 */
      328,  /*     0 */
      0   /*     0 */
};
/* MPEG matrix mixdown:
    Set 1:  L' = (1 + 2^-0.5 + A )^-1 * [L + C * 2^-0.5 + A * Ls];
            R' = (1 + 2^-0.5 + A )^-1 * [R + C * 2^-0.5 + A * Rs];

    Set 2:  L' = (1 + 2^-0.5 + 2A )^-1 * [L + C * 2^-0.5 - A * (Ls + Rs)];
            R' = (1 + 2^-0.5 + 2A )^-1 * [R + C * 2^-0.5 + A * (Ls + Rs)];

    M = (3 + 2A)^-1 * [L + C + R + A*(Ls + Rs)];
*/
static const int16_t mpegMixDownIdx2Coef[4] = {
      23170,   16384,
      11585,   0};

static const int16_t mpegMixDownIdx2PreFact[3][4] = {
    {/* Set 1: */
       13573,   14847,
       15902,   19195},
    {/* Set 2: */
       10498,   12104,
       13573,   19195},
    {/* Mono DMX set: */
       7423,   8192,
       8839,   10923}};

#define TYPE_NONE (0x00)
#define TYPE_PCE_DATA (0x01)
#define TYPE_DSE_CLEV_DATA (0x02)
#define TYPE_DSE_SLEV_DATA (0x04)
#define TYPE_DSE_DMIX_AB_DATA (0x08)
#define TYPE_DSE_DMIX_LFE_DATA (0x10)
#define TYPE_DSE_DMX_GAIN_DATA (0x20)
#define TYPE_DSE_DMX_CGL_DATA (0x40)
#define TYPE_DSE_DATA (0x7E)

typedef struct {
  uint32_t typeFlags;
  /* From DSE */
  uint8_t cLevIdx;
  uint8_t sLevIdx;
  uint8_t dmixIdxA;
  uint8_t dmixIdxB;
  uint8_t dmixIdxLfe;
  uint8_t dmxGainIdx2;
  uint8_t dmxGainIdx5;
  /* From PCE */
  uint8_t matrixMixdownIdx;
  /* Attributes: */
  int8_t pseudoSurround; /*!< If set to 1 the signal is pseudo surround
                           compatible. The value 0 tells that it is not. If the
                           value is -1 the information is not available.  */
  uint32_t expiryCount; /*!< Counter to monitor the life time of a meta data set. */

} DMX_BS_META_DATA;

/* Default metadata */
static const DMX_BS_META_DATA dfltMetaData = {0, 2, 2, 2,  2, 15,
                                              0, 0, 0, -1, 0};

/* Dynamic (user) params:
     See the definition of PCMDMX_PARAM for details on the specific fields. */
typedef struct {
  DMX_PROFILE_TYPE dmxProfile; /*!< Linked to DMX_PRFL_STANDARD              */
  uint32_t expiryFrame;            /*!< Linked to DMX_BS_DATA_EXPIRY_FRAME       */
  DUAL_CHANNEL_MODE dualChannelMode; /*!< Linked to DMX_DUAL_CHANNEL_MODE */
  PSEUDO_SURROUND_MODE
  pseudoSurrMode;          /*!< Linked to DMX_PSEUDO_SURROUND_MODE       */
  int16_t numOutChannelsMin; /*!< Linked to MIN_NUMBER_OF_OUTPUT_CHANNELS  */
  int16_t numOutChannelsMax; /*!< Linked to MAX_NUMBER_OF_OUTPUT_CHANNELS  */
  uint8_t frameDelay;        /*!< Linked to DMX_BS_DATA_DELAY              */

} PCM_DMX_USER_PARAMS;

/* Modules main data structure: */
struct PCM_DMX_INSTANCE {
  /* Metadata */
  DMX_BS_META_DATA bsMetaData[(1) + 1];
  PCM_DMX_USER_PARAMS userParams;

  uint8_t applyProcessing; /*!< Flag to en-/disable modules processing.
                              The max channel limiting is done independently. */
};

/* Memory allocation macro */


static uint32_t getSpeakerDistance(PCM_DMX_SPEAKER_POSITION posA,
                               PCM_DMX_SPEAKER_POSITION posB) {
  PCM_DMX_SPEAKER_POSITION diff;

  diff.x = posA.x - posB.x;
  diff.y = posA.y - posB.y;
  diff.z = posA.z - posB.z;

  return ((diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z));
}

static PCM_DMX_SPEAKER_POSITION getSpeakerPos(AUDIO_CHANNEL_TYPE_t chType,
                                              uint8_t chIndex, uint8_t numChInGrp) {
#define PCMDMX_SPKR_POS_X_MAX_WIDTH (3)
#define PCMDMX_SPKR_POS_Y_SPREAD (2)
#define PCMDMX_SPKR_POS_Z_SPREAD (2)

  PCM_DMX_SPEAKER_POSITION spkrPos = {0, 0, 0};
  AUDIO_CHANNEL_TYPE_t chGrp = (AUDIO_CHANNEL_TYPE_t)(chType & 0x0F);
  unsigned fHasCenter = numChInGrp & 0x1;
  unsigned chGrpWidth = numChInGrp >> 1;
  unsigned fIsCenter = 0;
  unsigned fIsLfe = (chType == ACT_LFE) ? 1 : 0;
  int32_t offset = 0;

  assert(chIndex < numChInGrp);

  if ((chGrp == ACT_FRONT) && fHasCenter) {
    if (chIndex == 0) fIsCenter = 1;
    chIndex = (uint8_t)fMax(0, chIndex - 1);
  } else if (fHasCenter && (chIndex == numChInGrp - 1)) {
    fIsCenter = 1;
  }
  /* now all even indices are left (-) */
  if (!fIsCenter) {
    offset = chIndex >> 1;
    if ((chGrp > ACT_FRONT) && (chType != ACT_SIDE) && !fIsLfe) {
      /* the higher the index the lower the distance to the center position */
      offset = chGrpWidth - fHasCenter - offset;
    }
    if ((chIndex & 0x1) == 0) { /* even */
      offset = -(offset + 1);
    } else {
      offset += 1;
    }
  }
  /* apply the offset */
  if (chType == ACT_SIDE) {
    spkrPos.x = (offset < 0) ? -PCMDMX_SPKR_POS_X_MAX_WIDTH
                             : PCMDMX_SPKR_POS_X_MAX_WIDTH;
    spkrPos.y = /* 1x */ PCMDMX_SPKR_POS_Y_SPREAD + (int8_t)fAbs(offset) - 1;
    spkrPos.z = 0;
  } else {
    unsigned spread =
        ((chGrpWidth == 1) && (!fIsLfe)) ? PCMDMX_SPKR_POS_X_MAX_WIDTH - 1 : 1;
    spkrPos.x = (int8_t)offset * (int8_t)spread;
    if (fIsLfe) {
      spkrPos.y = 0;
      spkrPos.z = SP_Z_LFE;
    } else {
      spkrPos.y = (int8_t)fMax((int8_t)chGrp - 1, 0) * PCMDMX_SPKR_POS_Y_SPREAD;
      spkrPos.z = (int8_t)chType >> 4;
      if (spkrPos.z == 2) { /* ACT_BOTTOM */
        spkrPos.z = -1;
      }
      spkrPos.z *= PCMDMX_SPKR_POS_Z_SPREAD;
    }
  }
  return spkrPos;
}

/** Return the channel mode of a given horizontal channel plain (normal, top,
 *bottom) for a given channel configuration. NOTE: This function shall get
 *obsolete once the channel mode has been changed to be nonambiguous.
 * @param [in] Index of the requested channel plain.
 * @param [in] The packed channel mode for the complete channel configuration
 *(all plains).
 * @param [in] The MPEG-4 channel configuration index which is necessary in
 *cases where the (packed) channel mode is ambiguous.
 * @returns Returns the packed channel mode of the requested channel plain.
 **/
static PCM_DMX_CHANNEL_MODE getChMode4Plain(
    const int32_t plainIndex, const PCM_DMX_CHANNEL_MODE totChMode,
    const int32_t chCfg) {
  PCM_DMX_CHANNEL_MODE plainChMode = totChMode;

  switch (totChMode) {
    case CH_MODE_5_0_2_1:
      if (chCfg == 14) {
        switch (plainIndex) {
          case CH_PLAIN_BOTTOM:
            plainChMode = (PCM_DMX_CHANNEL_MODE)0x0000;
            break;
          case CH_PLAIN_TOP:
            plainChMode = CH_MODE_2_0_0_0;
            break;
          case CH_PLAIN_NORMAL:
          default:
            plainChMode = CH_MODE_3_0_2_1;
            break;
        }
      }
      break;
    default:
      break;
  }

  return plainChMode;
}

static inline uint32_t getIdxSum(uint8_t numCh) {
  uint32_t result = 0;
  int32_t i;
  for (i = 1; i < numCh; i += 1) {
    result += i;
  }
  return result;
}

/** Evaluate a given channel configuration and extract a packed channel mode. In
 *addition the function generates a channel offset table for the mapping to the
 *internal representation. This function is the inverse to the
 *getChannelDescription() routine.
 * @param [in] The total number of channels of the given configuration.
 * @param [in] Array holding the corresponding channel types for each channel.
 * @param [in] Array holding the corresponding channel type indices for each
 *channel.
 * @param [out] Array where the buffer offsets for each channel are stored into.
 * @param [out] The generated packed channel mode that represents the given
 *input configuration.
 * @returns Returns an error code.
 **/
static PCMDMX_ERROR getChannelMode(
    const uint32_t numChannels,                 /* in */
    const AUDIO_CHANNEL_TYPE_t channelType[], /* in */
    uint8_t channelIndices[],                 /* in */
    uint8_t offsetTable[(8)],                 /* out */
    PCM_DMX_CHANNEL_MODE *chMode            /* out */
) {
  uint32_t idxSum[(3)][(4)];
  uint8_t numCh[(3)][(4)];
  uint8_t mapped[(8)];
  PCM_DMX_SPEAKER_POSITION spkrPos[(8)];
  PCMDMX_ERROR err = PCMDMX_OK;
  unsigned ch, numMappedInChs = 0;
  unsigned startSlot;
  unsigned stopSlot = LOW_FREQUENCY_CHANNEL;

  assert(channelType != NULL);
  assert(channelIndices != NULL);
  assert(offsetTable != NULL);
  assert(chMode != NULL);

  /* For details see ISO/IEC 13818-7:2005(E), 8.5.3 Channel configuration */
  memset(idxSum, 0, (3) * (4) * sizeof(uint32_t));
  memset(numCh, 0, (3) * (4) * sizeof(uint8_t));
  memset(mapped, 0, (8) * sizeof(uint8_t));
  memset(spkrPos, 0, (8) * sizeof(PCM_DMX_SPEAKER_POSITION));
  /* Init output */
  memset(offsetTable, 255, (8) * sizeof(uint8_t));
  *chMode = CH_MODE_UNDEFINED;

  /* Determine how many channels are assigned to each channels each group: */
  for (ch = 0; ch < numChannels; ch += 1) {
    unsigned chGrp = fMax(
        (channelType[ch] & 0x0F) - 1,
        0); /* Assign all undefined channels (ACT_NONE) to front channels. */
    numCh[channelType[ch] >> 4][chGrp] += 1;
    idxSum[channelType[ch] >> 4][chGrp] += channelIndices[ch];
  }
  if (numChannels > TWO_CHANNEL) {
    int32_t chGrp;
    /* Sanity check on the indices */
    for (chGrp = 0; chGrp < (4); chGrp += 1) {
      int32_t plane;
      for (plane = 0; plane < (3); plane += 1) {
        if (idxSum[plane][chGrp] != getIdxSum(numCh[plane][chGrp])) {
          unsigned idxCnt = 0;
          for (ch = 0; ch < numChannels; ch += 1) {
            if (channelType[ch] ==
                (AUDIO_CHANNEL_TYPE_t)((plane << 4) | ((chGrp + 1) & 0xF))) {
              channelIndices[ch] = idxCnt++;
            }
          }
          err = PCMDMX_INVALID_CH_CONFIG;
        }
      }
    }
  }
  /* Mapping HEAT 1:
   *   Determine the speaker position of each input channel and map it to a
   * internal slot if it matches exactly (with zero distance). */
  for (ch = 0; ch < numChannels; ch += 1) {
    uint32_t mapDist = (unsigned)-1;
    unsigned mapCh, mapPos = (unsigned)-1;
    unsigned chGrp = fMax(
        (channelType[ch] & 0x0F) - 1,
        0); /* Assign all undefined channels (ACT_NONE) to front channels. */

    if (channelIndices[ch] >= numCh[channelType[ch] >> 4][chGrp])
      return PCMDMX_INVALID_CH_CONFIG;
    spkrPos[ch] = getSpeakerPos(channelType[ch], channelIndices[ch],
                                numCh[channelType[ch] >> 4][chGrp]);

    for (mapCh = 0; mapCh <= stopSlot; mapCh += 1) {
      if (offsetTable[mapCh] == 255) {
        uint32_t dist = getSpeakerDistance(spkrPos[ch], spkrSlotPos[mapCh]);
        if (dist < mapDist) {
          mapPos = mapCh;
          mapDist = dist;
        }
      }
    }
    if (mapDist <= PCMDMX_THRESHOLD_MAP_HEAT_1) {
      offsetTable[mapPos] = (uint8_t)ch;
      mapped[ch] = 1;
      numMappedInChs += 1;
    }
  }

  /* Mapping HEAT 2:
   *   Go through the unmapped input channels and assign them to the internal
   * slots that matches best (least distance). But assign center channels to
   * center slots only. */
  startSlot =
      ((numCh[CH_PLAIN_NORMAL][CH_GROUP_FRONT] & 0x1) || (numChannels >= (8)))
          ? 0
          : 1;
  for (ch = 0; ch < (unsigned)numChannels; ch += 1) {
    if (!mapped[ch]) {
      uint32_t mapDist = (unsigned)-1;
      unsigned mapCh, mapPos = (unsigned)-1;

      for (mapCh = startSlot; mapCh <= stopSlot; mapCh += 1) {
        if (offsetTable[mapCh] == 255) {
          uint32_t dist = getSpeakerDistance(spkrPos[ch], spkrSlotPos[mapCh]);
          if (dist < mapDist) {
            mapPos = mapCh;
            mapDist = dist;
          }
        }
      }
      if ((mapPos <= stopSlot) && (mapDist < PCMDMX_THRESHOLD_MAP_HEAT_2) &&
          (((spkrPos[ch].x != 0) && (spkrSlotPos[mapPos].x != 0)) /* XOR */
           || ((spkrPos[ch].x == 0) &&
               (spkrSlotPos[mapPos].x ==
                0)))) { /* Assign center channels to center slots only. */
        offsetTable[mapPos] = (uint8_t)ch;
        mapped[ch] = 1;
        numMappedInChs += 1;
      }
    }
  }

  /* Mapping HEAT 3:
   *   Assign the rest by searching for the nearest input channel for each
   * internal slot. */
  for (ch = startSlot; (ch < (8)) && (numMappedInChs < numChannels); ch += 1) {
    if (offsetTable[ch] == 255) {
      uint32_t mapDist = (unsigned)-1;
      unsigned mapCh, mapPos = (unsigned)-1;

      for (mapCh = 0; mapCh < (unsigned)numChannels; mapCh += 1) {
        if (!mapped[mapCh]) {
          uint32_t dist = getSpeakerDistance(spkrPos[mapCh], spkrSlotPos[ch]);
          if (dist < mapDist) {
            mapPos = mapCh;
            mapDist = dist;
          }
        }
      }
      if (mapDist < PCMDMX_THRESHOLD_MAP_HEAT_3) {
        offsetTable[ch] = (uint8_t)mapPos;
        mapped[mapPos] = 1;
        numMappedInChs += 1;
        if ((spkrPos[mapPos].x == 0) && (spkrSlotPos[ch].x != 0) &&
            (numChannels <
             (8))) { /* Skip the paired slot if we assigned a center channel. */
          ch += 1;
        }
      }
    }
  }

  /* Finaly compose the channel mode */
  for (ch = 0; ch < (4); ch += 1) {
    int32_t plane, numChInGrp = 0;
    for (plane = 0; plane < (3); plane += 1) {
      numChInGrp += numCh[plane][ch];
    }
    *chMode = (PCM_DMX_CHANNEL_MODE)(*chMode | (numChInGrp << (ch * 4)));
  }

  return err;
}

/** Generate a channel offset table and complete channel description for a given
 *(packed) channel mode. This function is the inverse to the getChannelMode()
 *routine but does not support weird channel configurations.
 * @param [in] The packed channel mode of the configuration to be processed.
 * @param [in] Array containing the channel mapping to be used (From MPEG PCE
 *ordering to whatever is required).
 * @param [out] Array where corresponding channel types for each channels are
 *stored into.
 * @param [out] Array where corresponding channel type indices for each output
 *channel are stored into.
 * @param [out] Array where the buffer offsets for each channel are stored into.
 * @returns None.
 **/
static void getChannelDescription(
    const PCM_DMX_CHANNEL_MODE chMode,         /* in */
    const FDK_channelMapDescr_t *const mapDescr, /* in */
    AUDIO_CHANNEL_TYPE_t channelType[],          /* out */
    uint8_t channelIndices[],                    /* out */
    uint8_t offsetTable[(8)]                     /* out */
) {
  int32_t grpIdx, plainIdx, numPlains = 1, numTotalChannels = 0;
  int32_t chCfg, ch = 0;

  assert(channelType != NULL);
  assert(channelIndices != NULL);
  assert(mapDescr != NULL);
  assert(offsetTable != NULL);

  /* Init output arrays */
  memset(channelType, 0, (8) * sizeof(AUDIO_CHANNEL_TYPE_t));
  memset(channelIndices, 0, (8) * sizeof(uint8_t));
  memset(offsetTable, 255, (8) * sizeof(uint8_t));

  /* Summerize to get the total number of channels */
  for (grpIdx = 0; grpIdx < (4); grpIdx += 1) {
    numTotalChannels += (chMode >> (grpIdx * 4)) & 0xF;
  }

  /* Get the appropriate channel map */
  switch (chMode) {
    case CH_MODE_1_0_0_0:
    case CH_MODE_2_0_0_0:
    case CH_MODE_3_0_0_0:
    case CH_MODE_3_0_1_0:
    case CH_MODE_3_0_2_0:
    case CH_MODE_3_0_2_1:
      chCfg = numTotalChannels;
      break;
    case CH_MODE_3_0_3_1:
      chCfg = 11;
      break;
    case CH_MODE_3_0_4_1:
      chCfg = 12;
      break;
    case CH_MODE_5_0_2_1:
      chCfg = 7;
      break;
    default:
      /* fallback */
      chCfg = 0;
      break;
  }

  /* Compose channel offset table */

  for (plainIdx = 0; plainIdx < numPlains; plainIdx += 1) {
    PCM_DMX_CHANNEL_MODE plainChMode;
    uint8_t numChInGrp[(4)];

    plainChMode = getChMode4Plain(plainIdx, chMode, chCfg);

    /* Extract the number of channels per group */
    numChInGrp[CH_GROUP_FRONT] = plainChMode & 0xF;
    numChInGrp[CH_GROUP_SIDE] = (plainChMode >> 4) & 0xF;
    numChInGrp[CH_GROUP_REAR] = (plainChMode >> 8) & 0xF;
    numChInGrp[CH_GROUP_LFE] = (plainChMode >> 12) & 0xF;

    /* Non-symmetric channels */
    if ((numChInGrp[CH_GROUP_FRONT] & 0x1) && (plainIdx == CH_PLAIN_NORMAL)) {
      /* Odd number of front channels -> we have a center channel.
         In MPEG-4 the center has the index 0. */
      int32_t mappedIdx = FDK_chMapDescr_getMapValue(mapDescr, (uint8_t)ch, chCfg);
      offsetTable[CENTER_FRONT_CHANNEL] = (uint8_t)mappedIdx;
      channelType[mappedIdx] = ACT_FRONT;
      channelIndices[mappedIdx] = 0;
      ch += 1;
    }

    for (grpIdx = 0; grpIdx < (4); grpIdx += 1) {
      AUDIO_CHANNEL_TYPE_t type = ACT_NONE;
      int32_t chMapPos = 0, maxChannels = 0;
      int32_t chIdx = 0; /* Index of channel within the specific group */

      switch (grpIdx) {
        case CH_GROUP_FRONT:
          type = (AUDIO_CHANNEL_TYPE_t)((plainIdx << 4) | ACT_FRONT);
          switch (plainIdx) {
            default:
              chMapPos = LEFT_FRONT_CHANNEL;
              chIdx = numChInGrp[grpIdx] & 0x1;
              break;
          }
          maxChannels = 3;
          break;
        case CH_GROUP_SIDE:
          /* Always map side channels to the multipurpose group. */
          type = (AUDIO_CHANNEL_TYPE_t)((plainIdx << 4) | ACT_SIDE);
          if (plainIdx == CH_PLAIN_TOP) {
            chMapPos = LEFT_SIDE_CHANNEL_TOP;
            maxChannels = 3;
          } else {
            chMapPos = LEFT_MULTIPRPS_CHANNEL;
            maxChannels = 2;
          }
          break;
        case CH_GROUP_REAR:
          type = (AUDIO_CHANNEL_TYPE_t)((plainIdx << 4) | ACT_BACK);
          if (plainIdx == CH_PLAIN_TOP) {
            chMapPos = LEFT_REAR_CHANNEL_TOP;
            maxChannels = 3;
          } else {
            chMapPos = LEFT_REAR_CHANNEL;
            maxChannels = 2;
          }
          break;
        case CH_GROUP_LFE:
          if (plainIdx == CH_PLAIN_NORMAL) {
            type = ACT_LFE;
            chMapPos = LOW_FREQUENCY_CHANNEL;
            maxChannels = 1;
          }
          break;
        default:
          break;
      }

      /* Map all channels in this group */
      for (; chIdx < numChInGrp[grpIdx]; chIdx += 1) {
        int32_t mappedIdx = FDK_chMapDescr_getMapValue(mapDescr, (uint8_t)ch, chCfg);
        if ((chIdx == maxChannels) || (offsetTable[chMapPos] < 255)) {
          /* No space left in this channel group! */
          if (offsetTable[LEFT_MULTIPRPS_CHANNEL] ==
              255) { /* Use the multipurpose group: */
            chMapPos = LEFT_MULTIPRPS_CHANNEL;
          } else {
            assert(0);
          }
        }
        offsetTable[chMapPos] = (uint8_t)mappedIdx;
        channelType[mappedIdx] = type;
        channelIndices[mappedIdx] = (uint8_t)chIdx;
        chMapPos += 1;
        ch += 1;
      }
    }
  }
}

/** Private helper function for downmix matrix manipulation that initializes
 *  one row in a given downmix matrix (corresponding to one output channel).
 * @param [inout] Pointer to fixed-point parts of the downmix matrix.
 * @param [inout] Pointer to scale factor matrix associated to the downmix
 *factors.
 * @param [in]    Index of channel (row) to be initialized.
 * @returns       Nothing to return.
 **/
static void dmxInitChannel(int16_t mixFactors[(8)][(8)],
                           int32_t mixScales[(8)][(8)], const uint32_t outCh) {
  uint32_t inCh;
  for (inCh = 0; inCh < (8); inCh += 1) {
    if (inCh == outCh) {
      mixFactors[outCh][inCh] =   16384;
      mixScales[outCh][inCh] = 1;
    } else {
      mixFactors[outCh][inCh] =   0;
      mixScales[outCh][inCh] = 0;
    }
  }
}

/** Private helper function for downmix matrix manipulation that does a reset
 *  of one row in a given downmix matrix (corresponding to one output channel).
 * @param [inout] Pointer to fixed-point parts of the downmix matrix.
 * @param [inout] Pointer to scale factor matrix associated to the downmix
 *factors.
 * @param [in]    Index of channel (row) to be cleared/reset.
 * @returns       Nothing to return.
 **/
static void dmxClearChannel(int16_t mixFactors[(8)][(8)],
                            int32_t mixScales[(8)][(8)], const uint32_t outCh) {
  assert((outCh >= 0) && (outCh < (8)));
  memset(&mixFactors[outCh], 0, (8) * sizeof(int16_t));
  memset(&mixScales[outCh], 0,  (8) * sizeof(int32_t));
}

/** Private helper function for downmix matrix manipulation that applies a
 *source channel (row) scaled by a given mix factor to a destination channel
 *(row) in a given downmix matrix. Existing mix factors of the destination
 *channel (row) will get overwritten.
 * @param [inout] Pointer to fixed-point parts of the downmix matrix.
 * @param [inout] Pointer to scale factor matrix associated to the downmix
 *factors.
 * @param [in]    Index of source channel (row).
 * @param [in]    Index of destination channel (row).
 * @param [in]    Fixed-point part of mix factor to be applied.
 * @param [in]    Scale factor of mix factor to be applied.
 * @returns       Nothing to return.
 **/
static void dmxSetChannel(int16_t mixFactors[(8)][(8)],
                          int32_t mixScales[(8)][(8)], const uint32_t dstCh,
                          const uint32_t srcCh, const int16_t factor,
                          const int32_t scale) {
  int32_t ch;
  for (ch = 0; ch < (8); ch += 1) {
    if (mixFactors[srcCh][ch] != (int16_t)0) {
      mixFactors[dstCh][ch] =
          FX_DBL2FX_SGL(fMult(mixFactors[srcCh][ch], factor));
      mixScales[dstCh][ch] = mixScales[srcCh][ch] + scale;
    }
  }
}

/** Private helper function for downmix matrix manipulation that adds a source
 *channel (row) scaled by a given mix factor to a destination channel (row) in a
 *given downmix matrix.
 * @param [inout] Pointer to fixed-point parts of the downmix matrix.
 * @param [inout] Pointer to scale factor matrix associated to the downmix
 *factors.
 * @param [in]    Index of source channel (row).
 * @param [in]    Index of destination channel (row).
 * @param [in]    Fixed-point part of mix factor to be applied.
 * @param [in]    Scale factor of mix factor to be applied.
 * @returns       Nothing to return.
 **/
static void dmxAddChannel(int16_t mixFactors[(8)][(8)],
                          int32_t mixScales[(8)][(8)], const uint32_t dstCh,
                          const uint32_t srcCh, const int16_t factor,
                          const int32_t scale) {
  int32_t ch;
  for (ch = 0; ch < (8); ch += 1) {
    int32_t addFact = fMult(mixFactors[srcCh][ch], factor);
    if (addFact != (int16_t)0) {
      int32_t newScale = mixScales[srcCh][ch] + scale;
      if (mixFactors[dstCh][ch] != (int16_t)0) {
        if (newScale > mixScales[dstCh][ch]) {
          mixFactors[dstCh][ch] >>= newScale - mixScales[dstCh][ch];
        } else {
          addFact >>= mixScales[dstCh][ch] - newScale;
          newScale = mixScales[dstCh][ch];
        }
      }
      mixFactors[dstCh][ch] += FX_DBL2FX_SGL(addFact);
      mixScales[dstCh][ch] = newScale;
    }
  }
}

/** Private function that creates a downmix factor matrix depending on the input
 and output
 *  configuration, the user parameters as well as the given metadata. This
 function is the modules
 *  brain and hold all downmix algorithms.
 * @param [in]  Flag that indicates if inChMode holds a real (packed) channel
 mode or has been converted to a MPEG-4 channel configuration index.
 * @param [in]  Dependent on the inModeIsCfg flag this field hands in a (packed)
 channel mode or the corresponding MPEG-4 channel configuration index.of the
 input configuration.
 * @param [in]  The (packed) channel mode of the output configuration.
 * @param [in]  Pointer to structure holding all current user parameter.
 * @param [in]  Pointer to field holding all current meta data.
 * @param [out] Pointer to fixed-point parts of the downmix matrix. Normalized
 to one scale factor.
 * @param [out] The common scale factor of the downmix matrix.
 * @returns     An error code.
 **/
static PCMDMX_ERROR getMixFactors(const uint8_t inModeIsCfg,
                                  PCM_DMX_CHANNEL_MODE inChMode,
                                  const PCM_DMX_CHANNEL_MODE outChMode,
                                  const PCM_DMX_USER_PARAMS *pParams,
                                  const DMX_BS_META_DATA *pMetaData,
                                  int16_t mixFactors[(8)][(8)],
                                  int32_t *pOutScale) {
  PCMDMX_ERROR err = PCMDMX_OK;
  int32_t mixScales[(8)][(8)];
  int32_t maxScale = 0;
  int32_t numInChannel;
  int32_t numOutChannel;
  int32_t dmxMethod;
  uint32_t outCh, inChCfg = 0;
  uint32_t valid[(8)] = {0};

  assert(pMetaData != NULL);
  assert(mixFactors != NULL);
  /* Check on a supported output configuration.
     Add new one only after extensive testing! */
  if (!((outChMode == CH_MODE_1_0_0_0) || (outChMode == CH_MODE_2_0_0_0) ||
        (outChMode == CH_MODE_3_0_2_1) || (outChMode == CH_MODE_3_0_4_1) ||
        (outChMode == CH_MODE_5_0_2_1))) {
    assert(0);
  }

  if (inModeIsCfg) {
    /* Convert channel config to channel mode: */
    inChCfg = (uint32_t)inChMode;
    switch (inChCfg) {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
        inChMode = outChModeTable[inChCfg];
        break;
      case 11:
        inChMode = CH_MODE_3_0_3_1;
        break;
      case 12:
        inChMode = CH_MODE_3_0_4_1;
        break;
      case 7:
      case 14:
        inChMode = CH_MODE_5_0_2_1;
        break;
      default:
        assert(0);
    }
  }

  /* Extract the total number of input channels */
  numInChannel = (inChMode & 0xF) + ((inChMode >> 4) & 0xF) +
                 ((inChMode >> 8) & 0xF) + ((inChMode >> 12) & 0xF);
  /* Extract the total number of output channels */
  numOutChannel = (outChMode & 0xF) + ((outChMode >> 4) & 0xF) +
                  ((outChMode >> 8) & 0xF) + ((outChMode >> 12) & 0xF);

  /* MPEG ammendment 4 aka ETSI metadata and fallback mode: */

  /* Create identity DMX matrix: */
  for (outCh = 0; outCh < (8); outCh += 1) {
    dmxInitChannel(mixFactors, mixScales, outCh);
  }
  if (((inChMode >> 12) & 0xF) == 0) {
    /* Clear empty or wrongly mapped input channel */
    dmxClearChannel(mixFactors, mixScales, LOW_FREQUENCY_CHANNEL);
  }

  /* FIRST STAGE: */
  if (numInChannel > SIX_CHANNEL) { /* Always use MPEG equations either with
                                       meta data or with default values. */
    int16_t dMixFactA, dMixFactB;
    int32_t dMixScaleA, dMixScaleB;
    int32_t isValidCfg = TRUE;

    /* Get factors from meta data */
    dMixFactA = abMixLvlValueTab[pMetaData->dmixIdxA];
    dMixScaleA = (pMetaData->dmixIdxA == 0) ? 1 : 0;
    dMixFactB = abMixLvlValueTab[pMetaData->dmixIdxB];
    dMixScaleB = (pMetaData->dmixIdxB == 0) ? 1 : 0;

    /* Check if input is in the list of supported configurations */
    switch (inChMode) {
      case CH_MODE_3_2_1_1: /* chCfg 11 but with side channels */
      case CH_MODE_3_2_1_0:
        isValidCfg = FALSE;
        err = PCMDMX_INVALID_MODE;
        /* fall through */;
      case CH_MODE_3_0_3_1: /* chCfg 11 */
        /* 6.1ch:  C' = C;  L' = L;  R' = R;  LFE' = LFE;
                   Ls' = Ls*dmix_a_idx + Cs*dmix_b_idx;
                   Rs' = Rs*dmix_a_idx + Cs*dmix_b_idx; */
        dmxClearChannel(
            mixFactors, mixScales,
            RIGHT_MULTIPRPS_CHANNEL); /* clear empty input channel */
        dmxSetChannel(mixFactors, mixScales, LEFT_REAR_CHANNEL,
                      LEFT_REAR_CHANNEL, dMixFactA, dMixScaleA);
        dmxSetChannel(mixFactors, mixScales, LEFT_REAR_CHANNEL,
                      LEFT_MULTIPRPS_CHANNEL, dMixFactB, dMixScaleB);
        dmxSetChannel(mixFactors, mixScales, RIGHT_REAR_CHANNEL,
                      RIGHT_REAR_CHANNEL, dMixFactA, dMixScaleA);
        dmxSetChannel(mixFactors, mixScales, RIGHT_REAR_CHANNEL,
                      LEFT_MULTIPRPS_CHANNEL, dMixFactB, dMixScaleB);
        break;
      case CH_MODE_3_0_4_1: /* chCfg 12 */
        /* 7.1ch Surround Back:  C' = C;  L' = L;  R' = R;  LFE' = LFE;
                                 Ls' = Ls*dmix_a_idx + Lsr*dmix_b_idx;
                                 Rs' = Rs*dmix_a_idx + Rsr*dmix_b_idx; */
        dmxSetChannel(mixFactors, mixScales, LEFT_REAR_CHANNEL,
                      LEFT_REAR_CHANNEL, dMixFactA, dMixScaleA);
        dmxSetChannel(mixFactors, mixScales, LEFT_REAR_CHANNEL,
                      LEFT_MULTIPRPS_CHANNEL, dMixFactB, dMixScaleB);
        dmxSetChannel(mixFactors, mixScales, RIGHT_REAR_CHANNEL,
                      RIGHT_REAR_CHANNEL, dMixFactA, dMixScaleA);
        dmxSetChannel(mixFactors, mixScales, RIGHT_REAR_CHANNEL,
                      RIGHT_MULTIPRPS_CHANNEL, dMixFactB, dMixScaleB);
        break;
      case CH_MODE_5_0_1_0:
      case CH_MODE_5_0_1_1:
        dmxClearChannel(mixFactors, mixScales,
                        RIGHT_REAR_CHANNEL); /* clear empty input channel */
        dmxSetChannel(mixFactors, mixScales, RIGHT_REAR_CHANNEL,
                      LEFT_REAR_CHANNEL,   16384, 1);
        dmxSetChannel(mixFactors, mixScales, LEFT_REAR_CHANNEL,
                      LEFT_REAR_CHANNEL,   16384, 1);
        /* fall through */;
      case CH_MODE_5_2_1_0:
        isValidCfg = FALSE;
        err = PCMDMX_INVALID_MODE;
        /* fall through */;
      case CH_MODE_5_0_2_1: /* chCfg 7 || 14 */
        if (inChCfg == 14) {
          /* 7.1ch Front Height:  C' = C;  Ls' = Ls;  Rs' = Rs;  LFE' = LFE;
                                  L' = L*dmix_a_idx + Lv*dmix_b_idx;
                                  R' = R*dmix_a_idx + Rv*dmix_b_idx; */
          dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                        LEFT_FRONT_CHANNEL, dMixFactA, dMixScaleA);
          dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                        LEFT_MULTIPRPS_CHANNEL, dMixFactB, dMixScaleB);
          dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                        RIGHT_FRONT_CHANNEL, dMixFactA, dMixScaleA);
          dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                        RIGHT_MULTIPRPS_CHANNEL, dMixFactB, dMixScaleB);
        } else {
          /* 7.1ch Front:  Ls' = Ls;  Rs' = Rs;  LFE' = LFE;
                           C' = C + (Lc+Rc)*dmix_a_idx;
                           L' = L + Lc*dmix_b_idx;
                           R' = R + Rc*dmix_b_idx; */
          dmxSetChannel(mixFactors, mixScales, CENTER_FRONT_CHANNEL,
                        LEFT_MULTIPRPS_CHANNEL, dMixFactA, dMixScaleA);
          dmxSetChannel(mixFactors, mixScales, CENTER_FRONT_CHANNEL,
                        RIGHT_MULTIPRPS_CHANNEL, dMixFactA, dMixScaleA);
          dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                        LEFT_MULTIPRPS_CHANNEL, dMixFactB, dMixScaleB);
          dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                        LEFT_FRONT_CHANNEL,   16384, 1);
          dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                        RIGHT_MULTIPRPS_CHANNEL, dMixFactB, dMixScaleB);
          dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                        RIGHT_FRONT_CHANNEL,   16384, 1);
        }
        break;
      default:
        /* Nothing to do. Just use the identity matrix. */
        isValidCfg = FALSE;
        err = PCMDMX_INVALID_MODE;
        break;
    }

    /* Add additional DMX gain */
    if ((isValidCfg == TRUE) &&
        (pMetaData->dmxGainIdx5 != 0)) { /* Apply DMX gain 5 */
      int16_t dmxGain;
      int32_t dmxScale;
      int32_t sign = (pMetaData->dmxGainIdx5 & 0x40) ? -1 : 1;
      int32_t val = pMetaData->dmxGainIdx5 & 0x3F;

      /* 10^(dmx_gain_5/80) */
      dmxGain = FX_DBL2FX_SGL(
          fLdPow(FL2FXCONST_DBL(0.830482023721841f), 2, /* log2(10) */
                 (int32_t)(sign * val * (int32_t)FL2FXCONST_DBL(0.0125f)), 0,
                 &dmxScale));
      /* Currently only positive scale factors supported! */
      if (dmxScale < 0) {
        dmxGain >>= -dmxScale;
        dmxScale = 0;
      }

      dmxSetChannel(mixFactors, mixScales, CENTER_FRONT_CHANNEL,
                    CENTER_FRONT_CHANNEL, dmxGain, dmxScale);
      dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                    LEFT_FRONT_CHANNEL, dmxGain, dmxScale);
      dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                    RIGHT_FRONT_CHANNEL, dmxGain, dmxScale);
      dmxSetChannel(mixFactors, mixScales, LEFT_REAR_CHANNEL, LEFT_REAR_CHANNEL,
                    dmxGain, dmxScale);
      dmxSetChannel(mixFactors, mixScales, RIGHT_REAR_CHANNEL,
                    RIGHT_REAR_CHANNEL, dmxGain, dmxScale);
      dmxSetChannel(mixFactors, mixScales, LOW_FREQUENCY_CHANNEL,
                    LOW_FREQUENCY_CHANNEL, dmxGain, dmxScale);
    }

    /* Mark the output channels */
    valid[CENTER_FRONT_CHANNEL] = 1;
    valid[LEFT_FRONT_CHANNEL] = 1;
    valid[RIGHT_FRONT_CHANNEL] = 1;
    valid[LEFT_REAR_CHANNEL] = 1;
    valid[RIGHT_REAR_CHANNEL] = 1;
    valid[LOW_FREQUENCY_CHANNEL] = 1;

    /* Update channel mode for the next stage */
    inChMode = CH_MODE_3_0_2_1;
  }

    /* For the X (> 6) to 6 channel downmix we had no choice.
       To mix from 6 to 2 (or 1) channel(s) we have several possibilities (MPEG
       DSE | MPEG PCE | ITU | ARIB | DLB). Use profile and the metadata
       available flags to determine which equation to use: */

#define DMX_METHOD_MPEG_AMD4 1
#define DMX_METHOD_MPEG_LEGACY 2
#define DMX_METHOD_ARIB_JAPAN 4
#define DMX_METHOD_ITU_RECOM 8
#define DMX_METHOD_CUSTOM 16

  dmxMethod = DMX_METHOD_MPEG_AMD4; /* default */

  if ((pParams->dmxProfile == DMX_PRFL_FORCE_MATRIX_MIX) &&
      (pMetaData->typeFlags & TYPE_PCE_DATA)) {
    dmxMethod = DMX_METHOD_MPEG_LEGACY;
  } else if (!(pMetaData->typeFlags &
               (TYPE_DSE_CLEV_DATA | TYPE_DSE_SLEV_DATA))) {
    switch (pParams->dmxProfile) {
      default:
      case DMX_PRFL_STANDARD:
        /* dmxMethod = DMX_METHOD_MPEG_AMD4; */
        break;
      case DMX_PRFL_MATRIX_MIX:
      case DMX_PRFL_FORCE_MATRIX_MIX:
        if (pMetaData->typeFlags & TYPE_PCE_DATA) {
          dmxMethod = DMX_METHOD_MPEG_LEGACY;
        }
        break;
      case DMX_PRFL_ARIB_JAPAN:
        dmxMethod = DMX_METHOD_ARIB_JAPAN;
        break;
    }
  }

  /* SECOND STAGE: */
  if (numOutChannel <= TWO_CHANNEL) {
    /* Create DMX matrix according to input configuration */
    switch (inChMode) {
      case CH_MODE_2_0_0_0: /* chCfg 2 */
        /* Apply the dual channel mode. */
        switch (pParams->dualChannelMode) {
          case CH1_MODE: /* L' = 0.707 * Ch1;
                            R' = 0.707 * Ch1; */
            dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                          LEFT_FRONT_CHANNEL,   23167, 0);
            dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                          LEFT_FRONT_CHANNEL,   23167, 0);
            break;
          case CH2_MODE: /* L' = 0.707 * Ch2;
                            R' = 0.707 * Ch2; */
            dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                          RIGHT_FRONT_CHANNEL,   23167, 0);
            dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                          RIGHT_FRONT_CHANNEL,   23167, 0);
            break;
          case MIXED_MODE: /* L' = 0.5*Ch1 + 0.5*Ch2;
                              R' = 0.5*Ch1 + 0.5*Ch2; */
            dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                          LEFT_FRONT_CHANNEL,   16384, 0);
            dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                          RIGHT_FRONT_CHANNEL,   16384, 0);
            dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                          LEFT_FRONT_CHANNEL,   16384, 0);
            dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                          RIGHT_FRONT_CHANNEL,   16384, 0);
            break;
          default:
          case STEREO_MODE:
            /* Nothing to do */
            break;
        }
        break;
      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
       * - - - - - - - - - - - - - - - - - - - */
      case CH_MODE_2_0_1_0: {
        int16_t sMixLvl;
        if (dmxMethod == DMX_METHOD_ARIB_JAPAN) {
          /* L' = 0.707*L + 0.5*S;  R' = 0.707*R + 0.5*S; */
          dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                        LEFT_FRONT_CHANNEL,   23167, 0);
          dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                        RIGHT_FRONT_CHANNEL,   23167, 0);
          sMixLvl =   16384;
        } else { /* L' = L + 0.707*S;  R' = R + 0.707*S; */
          sMixLvl =   23167;
        }
        dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                      LEFT_REAR_CHANNEL, sMixLvl, 0);
        dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                      LEFT_REAR_CHANNEL, sMixLvl, 0);
      } break;
      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
       * - - - - - - - - - - - - - - - - - - - */
      case CH_MODE_3_0_0_0: /* chCfg 3 */
      {
        int16_t cMixLvl;
        if (dmxMethod == DMX_METHOD_ARIB_JAPAN) {
          /* L' = 0.707*L + 0.5*C;  R' = 0.707*R + 0.5*C; */
          dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                        LEFT_FRONT_CHANNEL,   23167, 0);
          dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                        RIGHT_FRONT_CHANNEL,   23167, 0);
          cMixLvl =   16384;
        } else { /* L' = L + 0.707*C;  R' = R + 0.707*C; */
          cMixLvl =   23167;
        }
        dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                      CENTER_FRONT_CHANNEL, cMixLvl, 0);
        dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                      CENTER_FRONT_CHANNEL, cMixLvl, 0);
      } break;
      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
       * - - - - - - - - - - - - - - - - - - - */
      case CH_MODE_3_0_1_0: /* chCfg 4 */
      {
        int16_t csMixLvl;
        if (dmxMethod == DMX_METHOD_ARIB_JAPAN) {
          /* L' = 0.707*L + 0.5*C + 0.5*S;  R' = 0.707*R + 0.5*C + 0.5*S; */
          dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                        LEFT_FRONT_CHANNEL,   23167, 0);
          dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                        RIGHT_FRONT_CHANNEL,   23167, 0);
          csMixLvl =   16384;
        } else { /* L' = L + 0.707*C + 0.707*S;
                    R' = R + 0.707*C + 0.707*S; */
          csMixLvl =   23167;
        }
        dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                      CENTER_FRONT_CHANNEL, csMixLvl, 0);
        dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                      LEFT_REAR_CHANNEL, csMixLvl, 0);
        dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                      CENTER_FRONT_CHANNEL, csMixLvl, 0);
        dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                      LEFT_REAR_CHANNEL, csMixLvl, 0);
      } break;
      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
       * - - - - - - - - - - - - - - - - - - - */
      case CH_MODE_3_0_2_0: /* chCfg 5 */
      case CH_MODE_3_0_2_1: /* chCfg 6 */
      {
        switch (dmxMethod) {
          default:
          case DMX_METHOD_MPEG_AMD4: {
            int16_t cMixLvl, sMixLvl, lMixLvl;
            int32_t cMixScale, sMixScale, lMixScale;

            /* Get factors from meta data */
            cMixLvl = abMixLvlValueTab[pMetaData->cLevIdx];
            cMixScale = (pMetaData->cLevIdx == 0) ? 1 : 0;
            sMixLvl = abMixLvlValueTab[pMetaData->sLevIdx];
            sMixScale = (pMetaData->sLevIdx == 0) ? 1 : 0;
            lMixLvl = lfeMixLvlValueTab[pMetaData->dmixIdxLfe];
            if (pMetaData->dmixIdxLfe <= 1) {
              lMixScale = 2;
            } else if (pMetaData->dmixIdxLfe <= 5) {
              lMixScale = 1;
            } else {
              lMixScale = 0;
            }
            /* Setup the DMX matrix */
            if ((pParams->pseudoSurrMode == FORCE_PS_DMX) ||
                ((pParams->pseudoSurrMode == AUTO_PS_DMX) &&
                 (pMetaData->pseudoSurround ==
                  1))) { /* L' = L + C*clev - (Ls+Rs)*slev + LFE*lflev;
                            R' = R + C*clev + (Ls+Rs)*slev + LFE*lflev; */
              dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                            CENTER_FRONT_CHANNEL, cMixLvl, cMixScale);
              dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                            LEFT_REAR_CHANNEL, -sMixLvl, sMixScale);
              dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                            RIGHT_REAR_CHANNEL, -sMixLvl, sMixScale);
              dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                            LOW_FREQUENCY_CHANNEL, lMixLvl, lMixScale);
              dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                            CENTER_FRONT_CHANNEL, cMixLvl, cMixScale);
              dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                            LEFT_REAR_CHANNEL, sMixLvl, sMixScale);
              dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                            RIGHT_REAR_CHANNEL, sMixLvl, sMixScale);
              dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                            LOW_FREQUENCY_CHANNEL, lMixLvl, lMixScale);
            } else { /* L' = L + C*clev + Ls*slev + LFE*llev;
                        R' = R + C*clev + Rs*slev + LFE*llev; */
              dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                            CENTER_FRONT_CHANNEL, cMixLvl, cMixScale);
              dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                            LEFT_REAR_CHANNEL, sMixLvl, sMixScale);
              dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                            LOW_FREQUENCY_CHANNEL, lMixLvl, lMixScale);
              dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                            CENTER_FRONT_CHANNEL, cMixLvl, cMixScale);
              dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                            RIGHT_REAR_CHANNEL, sMixLvl, sMixScale);
              dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                            LOW_FREQUENCY_CHANNEL, lMixLvl, lMixScale);
            }

            /* Add additional DMX gain */
            if (pMetaData->dmxGainIdx2 != 0) { /* Apply DMX gain 2 */
              int16_t dmxGain;
              int32_t dmxScale;
              int32_t sign = (pMetaData->dmxGainIdx2 & 0x40) ? -1 : 1;
              int32_t val = pMetaData->dmxGainIdx2 & 0x3F;

              /* 10^(dmx_gain_2/80) */
              dmxGain = FX_DBL2FX_SGL(
                  fLdPow(FL2FXCONST_DBL(0.830482023721841f), 2, /* log2(10) */
                         (int32_t)(sign * val * (int32_t)FL2FXCONST_DBL(0.0125f)),
                         0, &dmxScale));
              /* Currently only positive scale factors supported! */
              if (dmxScale < 0) {
                dmxGain >>= -dmxScale;
                dmxScale = 0;
              }

              dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                            LEFT_FRONT_CHANNEL, dmxGain, dmxScale);
              dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                            RIGHT_FRONT_CHANNEL, dmxGain, dmxScale);
            }
          } break;
          case DMX_METHOD_ARIB_JAPAN:
          case DMX_METHOD_MPEG_LEGACY: {
            int16_t flev, clev, slevLL, slevLR, slevRL, slevRR;
            int16_t mtrxMixDwnCoef =
                mpegMixDownIdx2Coef[pMetaData->matrixMixdownIdx];

            if ((pParams->pseudoSurrMode == FORCE_PS_DMX) ||
                ((pParams->pseudoSurrMode == AUTO_PS_DMX) &&
                 (pMetaData->pseudoSurround == 1))) {
              if (dmxMethod == DMX_METHOD_ARIB_JAPAN) {
                /* 3/2 input: L' = 0.707 * [L+0.707*C-k*Ls-k*Rs];
                              R' = 0.707 * [R+0.707*C+k*Ls+k*Rs]; */
                flev = mpegMixDownIdx2Coef[0]; /* a = 0.707 */
              } else { /* 3/2 input: L' = (1.707+2*A)^-1 *
                          [L+0.707*C-A*Ls-A*Rs]; R' = (1.707+2*A)^-1 *
                          [R+0.707*C+A*Ls+A*Rs]; */
                flev = mpegMixDownIdx2PreFact[1][pMetaData->matrixMixdownIdx];
              }
              slevRR = slevRL = FX_DBL2FX_SGL(fMult(flev, mtrxMixDwnCoef));
              slevLL = slevLR = -slevRL;
            } else {
              if (dmxMethod == DMX_METHOD_ARIB_JAPAN) {
                /* 3/2 input: L' = 0.707 * [L+0.707*C+k*Ls];
                              R' = 0.707 * [R+0.707*C+k*Rs]; */
                flev = mpegMixDownIdx2Coef[0]; /* a = 0.707 */
              } else { /* 3/2 input: L' = (1.707+A)^-1 * [L+0.707*C+A*Ls];
                                     R' = (1.707+A)^-1 * [R+0.707*C+A*Rs]; */
                flev = mpegMixDownIdx2PreFact[0][pMetaData->matrixMixdownIdx];
              }
              slevRR = slevLL = FX_DBL2FX_SGL(fMult(flev, mtrxMixDwnCoef));
              slevLR = slevRL = (int16_t)0;
            }
            /* common factor */
            clev =
                FX_DBL2FX_SGL(fMult(flev, mpegMixDownIdx2Coef[0] /* 0.707 */));

            dmxSetChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                          LEFT_FRONT_CHANNEL, flev, 0);
            dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                          CENTER_FRONT_CHANNEL, clev, 0);
            dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                          LEFT_REAR_CHANNEL, slevLL, 0);
            dmxAddChannel(mixFactors, mixScales, LEFT_FRONT_CHANNEL,
                          RIGHT_REAR_CHANNEL, slevLR, 0);

            dmxSetChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                          RIGHT_FRONT_CHANNEL, flev, 0);
            dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                          CENTER_FRONT_CHANNEL, clev, 0);
            dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                          LEFT_REAR_CHANNEL, slevRL, 0);
            dmxAddChannel(mixFactors, mixScales, RIGHT_FRONT_CHANNEL,
                          RIGHT_REAR_CHANNEL, slevRR, 0);
          } break;
        } /* switch (dmxMethod) */
      } break;
      default:
        /* This configuration does not fit to any known downmix equation! */
        err = PCMDMX_INVALID_MODE;
        break;
    } /* switch (inChMode) */

    /* Mark the output channels */
    memset(valid, 0, (8) * sizeof(uint32_t));
    valid[LEFT_FRONT_CHANNEL] = 1;
    valid[RIGHT_FRONT_CHANNEL] = 1;
  }

  if (numOutChannel == ONE_CHANNEL) {
    int16_t monoMixLevel;
    int32_t monoMixScale = 0;

    dmxClearChannel(mixFactors, mixScales,
                    CENTER_FRONT_CHANNEL); /* C is not in the mix */

    if (dmxMethod ==
        DMX_METHOD_MPEG_LEGACY) { /* C' = (3+2*A)^-1 * [C+L+R+A*Ls+A+Rs]; */
      monoMixLevel = mpegMixDownIdx2PreFact[2][pMetaData->matrixMixdownIdx];

      mixFactors[CENTER_FRONT_CHANNEL][CENTER_FRONT_CHANNEL] = monoMixLevel;
      mixFactors[CENTER_FRONT_CHANNEL][LEFT_FRONT_CHANNEL] = monoMixLevel;
      mixFactors[CENTER_FRONT_CHANNEL][RIGHT_FRONT_CHANNEL] = monoMixLevel;
      monoMixLevel = FX_DBL2FX_SGL(fMult(monoMixLevel, mpegMixDownIdx2Coef[pMetaData->matrixMixdownIdx]));
      mixFactors[CENTER_FRONT_CHANNEL][LEFT_REAR_CHANNEL] = monoMixLevel;
      mixFactors[CENTER_FRONT_CHANNEL][RIGHT_REAR_CHANNEL] = monoMixLevel;
    } else {
      switch (dmxMethod) {
        case DMX_METHOD_MPEG_AMD4:
          /* C' = L + R; */
          monoMixLevel =   16384;
          monoMixScale = 1;
          break;
        default:
          /* C' = 0.5*L + 0.5*R; */
          monoMixLevel =   16384;
          monoMixScale = 0;
          break;
      }
      dmxSetChannel(mixFactors, mixScales, CENTER_FRONT_CHANNEL,
                    LEFT_FRONT_CHANNEL, monoMixLevel, monoMixScale);
      dmxAddChannel(mixFactors, mixScales, CENTER_FRONT_CHANNEL,
                    RIGHT_FRONT_CHANNEL, monoMixLevel, monoMixScale);
    }

    /* Mark the output channel */
    memset(valid, 0, (8) * sizeof(uint32_t));
    valid[CENTER_FRONT_CHANNEL] = 1;
  }

#define MAX_SEARCH_START_VAL (-7)

  {
    int32_t chSum[(8)];
    int32_t chSumMax = MAX_SEARCH_START_VAL;

    /* Determine the current maximum scale factor */
    for (outCh = 0; outCh < (8); outCh += 1) {
      if (valid[outCh] != 0) {
        uint32_t inCh;
        for (inCh = 0; inCh < (8); inCh += 1) {
          if (mixScales[outCh][inCh] > maxScale) { /* Store the new maximum */
            maxScale = mixScales[outCh][inCh];
          }
        }
      }
    }

    /* Individualy analyse output chanal levels */
    for (outCh = 0; outCh < (8); outCh += 1) {
      chSum[outCh] = MAX_SEARCH_START_VAL;
      if (valid[outCh] != 0) {
        int32_t ovrflwProtScale = 0;
        uint32_t inCh;

        /* Accumulate all factors for each output channel */
        chSum[outCh] = 0;
        for (inCh = 0; inCh < (8); inCh += 1) {
          int16_t addFact = (int16_t)(mixFactors[outCh][inCh]);
          if (mixScales[outCh][inCh] <= maxScale) {
            addFact >>= maxScale - mixScales[outCh][inCh];
          } else {
            addFact <<= mixScales[outCh][inCh] - maxScale;
          }
          chSum[outCh] += addFact;
        }
        if (chSum[outCh] > (int32_t)MAXVAL_SGL) {
          while (chSum[outCh] > (int32_t)MAXVAL_SGL) {
            ovrflwProtScale += 1;
            chSum[outCh] >>= 1;
          }
        } else if (chSum[outCh] > 0) {
          while ((chSum[outCh] << 1) <= (int32_t)MAXVAL_SGL) {
            ovrflwProtScale -= 1;
            chSum[outCh] <<= 1;
          }
        }
        /* Store the differential scaling in the same array */
        chSum[outCh] = ovrflwProtScale;
      }
    }

    for (outCh = 0; outCh < (8); outCh += 1) {
      if ((valid[outCh] != 0) &&
          (chSum[outCh] > chSumMax)) { /* Store the new maximum */
        chSumMax = chSum[outCh];
      }
    }
    maxScale = fMax(maxScale + chSumMax, 0);

    /* Normalize all factors */
    for (outCh = 0; outCh < (8); outCh += 1) {
      if (valid[outCh] != 0) {
        uint32_t inCh;
        for (inCh = 0; inCh < (8); inCh += 1) {
          if (mixFactors[outCh][inCh] != (int16_t)0) {
            if (mixScales[outCh][inCh] <= maxScale) {
              mixFactors[outCh][inCh] >>= maxScale - mixScales[outCh][inCh];
            } else {
              mixFactors[outCh][inCh] <<= mixScales[outCh][inCh] - maxScale;
            }
            mixScales[outCh][inCh] = maxScale;
          }
        }
      }
    }
  }

  /* return the scale factor */
  *pOutScale = maxScale;

  return (err);
}

/** Open and initialize an instance of the PCM downmix module
 * @param [out] Pointer to a buffer receiving the handle of the new instance.
 * @returns Returns an error code.
 **/
PCMDMX_ERROR pcmDmx_Open(HANDLE_PCM_DOWNMIX *pSelf) {
  HANDLE_PCM_DOWNMIX self;

  if (pSelf == NULL) {
    return (PCMDMX_INVALID_HANDLE);
  }

  *pSelf = NULL;

  self = (HANDLE_PCM_DOWNMIX)(struct PCM_DMX_INSTANCE *)FDKcalloc(1, sizeof(struct PCM_DMX_INSTANCE));
  if (self == NULL) {
    return (PCMDMX_OUT_OF_MEMORY);
  }

  /* Reset the full instance */
  pcmDmx_Reset(self, PCMDMX_RESET_FULL);

  *pSelf = self;

  return (PCMDMX_OK);
}

/** Reset all static values like e.g. mixdown coefficients.
 * @param [in] Handle of PCM downmix module instance.
 * @param [in] Flags telling which parts of the module shall be reset.
 * @returns Returns an error code.
 **/
PCMDMX_ERROR pcmDmx_Reset(HANDLE_PCM_DOWNMIX self, uint32_t flags) {
  if (self == NULL) {
    return (PCMDMX_INVALID_HANDLE);
  }

  if (flags & PCMDMX_RESET_PARAMS) {
    PCM_DMX_USER_PARAMS *pParams = &self->userParams;

    pParams->dualChannelMode = STEREO_MODE;
    pParams->pseudoSurrMode = NEVER_DO_PS_DMX;
    pParams->numOutChannelsMax = (6);
    pParams->numOutChannelsMin = (0);
    pParams->frameDelay = 0;
    pParams->expiryFrame = (0);

    self->applyProcessing = 0;
  }

  if (flags & PCMDMX_RESET_BS_DATA) {
    int32_t slot;
    /* Init all slots with a default set */
    for (slot = 0; slot <= (1); slot += 1) {
      memcpy(&self->bsMetaData[slot], &dfltMetaData,
                sizeof(DMX_BS_META_DATA));
    }
  }

  return (PCMDMX_OK);
}

/** Set one parameter for one instance of the PCM downmix module.
 * @param [in] Handle of PCM downmix module instance.
 * @param [in] Parameter to be set.
 * @param [in] Parameter value.
 * @returns Returns an error code.
 **/
PCMDMX_ERROR pcmDmx_SetParam(HANDLE_PCM_DOWNMIX self, const PCMDMX_PARAM param,
                             const int32_t value) {
  switch (param) {
    case DMX_PROFILE_SETTING:
      switch ((DMX_PROFILE_TYPE)value) {
        case DMX_PRFL_STANDARD:
        case DMX_PRFL_MATRIX_MIX:
        case DMX_PRFL_FORCE_MATRIX_MIX:
        case DMX_PRFL_ARIB_JAPAN:
          break;
        default:
          return (PCMDMX_UNABLE_TO_SET_PARAM);
      }
      if (self == NULL) return (PCMDMX_INVALID_HANDLE);
      self->userParams.dmxProfile = (DMX_PROFILE_TYPE)value;
      break;

    case DMX_BS_DATA_EXPIRY_FRAME:
      if (self == NULL) return (PCMDMX_INVALID_HANDLE);
      self->userParams.expiryFrame = (value > 0) ? (uint32_t)value : 0;
      break;

    case DMX_BS_DATA_DELAY:
      if ((value > (1)) || (value < 0)) {
        return (PCMDMX_UNABLE_TO_SET_PARAM);
      }
      if (self == NULL) {
        return (PCMDMX_INVALID_HANDLE);
      }
      self->userParams.frameDelay = (uint8_t)value;
      break;

    case MIN_NUMBER_OF_OUTPUT_CHANNELS:
      switch (value) { /* supported output channels */
        case -1:
        case 0:
        case ONE_CHANNEL:
        case TWO_CHANNEL:
        case SIX_CHANNEL:
        case EIGHT_CHANNEL:
          break;
        default:
          return (PCMDMX_UNABLE_TO_SET_PARAM);
      }
      if (self == NULL) return (PCMDMX_INVALID_HANDLE);
      /* Store the new value */
      self->userParams.numOutChannelsMin = (value > 0) ? (int16_t)value : -1;
      if ((value > 0) && (self->userParams.numOutChannelsMax > 0) &&
          (value > self->userParams
                       .numOutChannelsMax)) { /* MIN > MAX would be an invalid
                                                 state. Thus set MAX = MIN in
                                                 this case. */
        self->userParams.numOutChannelsMax = self->userParams.numOutChannelsMin;
      }
      break;

    case MAX_NUMBER_OF_OUTPUT_CHANNELS:
      switch (value) { /* supported output channels */
        case -1:
        case 0:
        case ONE_CHANNEL:
        case TWO_CHANNEL:
        case SIX_CHANNEL:
        case EIGHT_CHANNEL:
          break;
        default:
          return (PCMDMX_UNABLE_TO_SET_PARAM);
      }
      if (self == NULL) return (PCMDMX_INVALID_HANDLE);
      /* Store the new value */
      self->userParams.numOutChannelsMax = (value > 0) ? (int16_t)value : -1;
      if ((value > 0) &&
          (value < self->userParams
                       .numOutChannelsMin)) { /* MAX < MIN would be an invalid
                                                 state. Thus set MIN = MAX in
                                                 this case. */
        self->userParams.numOutChannelsMin = self->userParams.numOutChannelsMax;
      }
      break;

    case DMX_DUAL_CHANNEL_MODE:
      switch ((DUAL_CHANNEL_MODE)value) {
        case STEREO_MODE:
        case CH1_MODE:
        case CH2_MODE:
        case MIXED_MODE:
          break;
        default:
          return (PCMDMX_UNABLE_TO_SET_PARAM);
      }
      if (self == NULL) return (PCMDMX_INVALID_HANDLE);
      self->userParams.dualChannelMode = (DUAL_CHANNEL_MODE)value;
      self->applyProcessing = ((DUAL_CHANNEL_MODE)value != STEREO_MODE)
                                  ? 1
                                  : 0; /* Force processing if necessary. */
      break;

    case DMX_PSEUDO_SURROUND_MODE:
      switch ((PSEUDO_SURROUND_MODE)value) {
        case NEVER_DO_PS_DMX:
        case AUTO_PS_DMX:
        case FORCE_PS_DMX:
          break;
        default:
          return (PCMDMX_UNABLE_TO_SET_PARAM);
      }
      if (self == NULL) return (PCMDMX_INVALID_HANDLE);
      self->userParams.pseudoSurrMode = (PSEUDO_SURROUND_MODE)value;
      break;

    default:
      return (PCMDMX_UNKNOWN_PARAM);
  }

  return (PCMDMX_OK);
}

/** Get one parameter value of one PCM downmix module instance.
 * @param [in] Handle of PCM downmix module instance.
 * @param [in] Parameter to be set.
 * @param [out] Pointer to buffer receiving the parameter value.
 * @returns Returns an error code.
 **/
PCMDMX_ERROR pcmDmx_GetParam(HANDLE_PCM_DOWNMIX self, const PCMDMX_PARAM param,
                             int32_t *const pValue) {
  PCM_DMX_USER_PARAMS *pUsrParams;

  if ((self == NULL) || (pValue == NULL)) {
    return (PCMDMX_INVALID_HANDLE);
  }
  pUsrParams = &self->userParams;

  switch (param) {
    case DMX_PROFILE_SETTING:
      *pValue = (int32_t)pUsrParams->dmxProfile;
      break;
    case DMX_BS_DATA_EXPIRY_FRAME:
      *pValue = (int32_t)pUsrParams->expiryFrame;
      break;
    case DMX_BS_DATA_DELAY:
      *pValue = (int32_t)pUsrParams->frameDelay;
      break;
    case MIN_NUMBER_OF_OUTPUT_CHANNELS:
      *pValue = (int32_t)pUsrParams->numOutChannelsMin;
      break;
    case MAX_NUMBER_OF_OUTPUT_CHANNELS:
      *pValue = (int32_t)pUsrParams->numOutChannelsMax;
      break;
    case DMX_DUAL_CHANNEL_MODE:
      *pValue = (int32_t)pUsrParams->dualChannelMode;
      break;
    case DMX_PSEUDO_SURROUND_MODE:
      *pValue = (int32_t)pUsrParams->pseudoSurrMode;
      break;
    default:
      return (PCMDMX_UNKNOWN_PARAM);
  }

  return (PCMDMX_OK);
}

/*
 * Read DMX meta-data from a data stream element.
 */
PCMDMX_ERROR pcmDmx_Parse(HANDLE_PCM_DOWNMIX self, HANDLE_FDK_BITSTREAM hBs,
                          uint32_t ancDataBits, int32_t isMpeg2) {
  PCMDMX_ERROR errorStatus = PCMDMX_OK;

#define MAX_DSE_ANC_BYTES (16)    /* 15 bytes */
#define ANC_DATA_SYNC_BYTE (0xBC) /* ancillary data sync byte. */

  DMX_BS_META_DATA *pBsMetaData;

  int32_t skip4Dmx = 0, skip4Ext = 0;
  int32_t dmxLvlAvail = 0, extDataAvail = 0;
  uint32_t foundNewData = 0;
  uint32_t minAncBits = ((isMpeg2) ? 5 : 3) * 8;

  if ((self == NULL) || (hBs == NULL)) {
    return (PCMDMX_INVALID_HANDLE);
  }

  /* sanity checks */
  if ((ancDataBits < minAncBits) || (ancDataBits > FDKgetValidBits(hBs))) {
    return (PCMDMX_CORRUPT_ANC_DATA);
  }

  pBsMetaData = &self->bsMetaData[0];

  if (isMpeg2) {
    /* skip DVD ancillary data */
    FDKpushFor(hBs, 16);
  }

  /* check sync word */
  if (FDKreadBits(hBs, 8) != ANC_DATA_SYNC_BYTE) {
    return (PCMDMX_CORRUPT_ANC_DATA);
  }

  /* skip MPEG audio type and Dolby surround mode */
  FDKpushFor(hBs, 4);

  if (isMpeg2) {
    /* int32_t numAncBytes = */ FDKreadBits(hBs, 4);
    /* advanced dynamic range control */
    if (FDKreadBit(hBs)) skip4Dmx += 24;
    /* dialog normalization */
    if (FDKreadBit(hBs)) skip4Dmx += 8;
    /* reproduction_level */
    if (FDKreadBit(hBs)) skip4Dmx += 8;
  } else {
    FDKpushFor(hBs, 2); /* drc presentation mode */
    pBsMetaData->pseudoSurround = (int8_t)FDKreadBit(hBs);
    FDKpushFor(hBs, 4); /* reserved bits */
  }

  /* downmixing levels MPEGx status */
  dmxLvlAvail = FDKreadBit(hBs);

  if (isMpeg2) {
    /* scale factor CRC status */
    if (FDKreadBit(hBs)) skip4Ext += 16;
  } else {
    /* ancillary data extension status */
    extDataAvail = FDKreadBit(hBs);
  }

  /* audio coding and compression status */
  if (FDKreadBit(hBs)) skip4Ext += 16;
  /* coarse grain timecode status */
  if (FDKreadBit(hBs)) skip4Ext += 16;
  /* fine grain timecode status */
  if (FDKreadBit(hBs)) skip4Ext += 16;

  /* skip the useless data to get to the DMX levels */
  FDKpushFor(hBs, skip4Dmx);

  /* downmix_levels_MPEGX */
  if (dmxLvlAvail) {
    if (FDKreadBit(hBs)) { /* center_mix_level_on */
      pBsMetaData->cLevIdx = (uint8_t)FDKreadBits(hBs, 3);
      foundNewData |= TYPE_DSE_CLEV_DATA;
    } else {
      FDKreadBits(hBs, 3);
    }
    if (FDKreadBit(hBs)) { /* surround_mix_level_on */
      pBsMetaData->sLevIdx = (uint8_t)FDKreadBits(hBs, 3);
      foundNewData |= TYPE_DSE_SLEV_DATA;
    } else {
      FDKreadBits(hBs, 3);
    }
  }

  /* skip the useless data to get to the ancillary data extension */
  FDKpushFor(hBs, skip4Ext);

  /* anc data extension (MPEG-4 only) */
  if (extDataAvail) {
    int32_t extDmxLvlSt, extDmxGainSt, extDmxLfeSt;

    FDKreadBit(hBs); /* reserved bit */
    extDmxLvlSt = FDKreadBit(hBs);
    extDmxGainSt = FDKreadBit(hBs);
    extDmxLfeSt = FDKreadBit(hBs);
    FDKreadBits(hBs, 4); /* reserved bits */

    if (extDmxLvlSt) {
      pBsMetaData->dmixIdxA = (uint8_t)FDKreadBits(hBs, 3);
      pBsMetaData->dmixIdxB = (uint8_t)FDKreadBits(hBs, 3);
      FDKreadBits(hBs, 2); /* reserved bits */
      foundNewData |= TYPE_DSE_DMIX_AB_DATA;
    }
    if (extDmxGainSt) {
      pBsMetaData->dmxGainIdx5 = (uint8_t)FDKreadBits(hBs, 7);
      FDKreadBit(hBs); /* reserved bit */
      pBsMetaData->dmxGainIdx2 = (uint8_t)FDKreadBits(hBs, 7);
      FDKreadBit(hBs); /* reserved bit */
      foundNewData |= TYPE_DSE_DMX_GAIN_DATA;
    }
    if (extDmxLfeSt) {
      pBsMetaData->dmixIdxLfe = (uint8_t)FDKreadBits(hBs, 4);
      FDKreadBits(hBs, 4); /* reserved bits */
      foundNewData |= TYPE_DSE_DMIX_LFE_DATA;
    }
  }

  /* final sanity check on the amount of read data */
  if ((int32_t)FDKgetValidBits(hBs) < 0) {
    errorStatus = PCMDMX_CORRUPT_ANC_DATA;
  }

  if ((errorStatus == PCMDMX_OK) && (foundNewData != 0)) {
    /* announce new data */
    pBsMetaData->typeFlags |= foundNewData;
    /* reset expiry counter */
    pBsMetaData->expiryCount = 0;
  }

  return (errorStatus);
}

/*
 * Read DMX meta-data from a data stream element.
 */
PCMDMX_ERROR pcmDmx_ReadDvbAncData(HANDLE_PCM_DOWNMIX self, uint8_t *pAncDataBuf,
                                   uint32_t ancDataBytes, int32_t isMpeg2) {
  PCMDMX_ERROR errorStatus = PCMDMX_OK;
  FDK_BITSTREAM_t bs;
  HANDLE_FDK_BITSTREAM hBs = &bs;

  if (self == NULL) {
    return (PCMDMX_INVALID_HANDLE);
  }

  /* sanity checks */
  if ((pAncDataBuf == NULL) || (ancDataBytes == 0)) {
    return (PCMDMX_CORRUPT_ANC_DATA);
  }

  FDKinitBitStream(hBs, pAncDataBuf, MAX_DSE_ANC_BYTES, ancDataBytes * 8,
                   BS_READER);

  errorStatus = pcmDmx_Parse(self, hBs, ancDataBytes * 8, isMpeg2);

  return (errorStatus);
}

/** Set the matrix mixdown information extracted from the PCE of an AAC
 *bitstream. Note: Call only if matrix_mixdown_idx_present is true.
 * @param [in] Handle of PCM downmix module instance.
 * @param [in] The 2 bit matrix mixdown index extracted from PCE.
 * @param [in] The pseudo surround enable flag extracted from PCE.
 * @returns Returns an error code.
 **/
PCMDMX_ERROR pcmDmx_SetMatrixMixdownFromPce(HANDLE_PCM_DOWNMIX self,
                                            int32_t matrixMixdownPresent,
                                            int32_t matrixMixdownIdx,
                                            int32_t pseudoSurroundEnable) {
  if (self == NULL) {
    return (PCMDMX_INVALID_HANDLE);
  }

  {
    DMX_BS_META_DATA *pBsMetaData = &self->bsMetaData[0];

    if (matrixMixdownPresent) {
      pBsMetaData->pseudoSurround = (pseudoSurroundEnable) ? 1 : 0;
      pBsMetaData->matrixMixdownIdx = matrixMixdownIdx & 0x03;
      pBsMetaData->typeFlags |= TYPE_PCE_DATA;
      /* Reset expiry counter */
      pBsMetaData->expiryCount = 0;
    }
  }

  return (PCMDMX_OK);
}

/** Apply down or up mixing.
 * @param [in]    Handle of PCM downmix module instance.
 * @param [inout] Pointer to buffer that hold the time domain signal.
 * @param [in]    Pointer where the amount of output samples is returned into.
 * @param [in]    Size of pPcmBuf.
 * @param [inout] Pointer where the amount of output channels is returned into.
 * @param [in]    Input and output samples are processed interleaved.
 * @param [inout] Array where the corresponding channel type for each output
 *audio channel is stored into.
 * @param [inout] Array where the corresponding channel type index for each
 *output audio channel is stored into.
 * @param [in]    Array containing the out channel mapping to be used (From MPEG
 *PCE ordering to whatever is required).
 * @param [out]   Pointer on a field receiving the scale factor that has to be
 *applied on all samples afterwards. If the handed pointer is NULL scaling is
 *done internally.
 * @returns Returns an error code.
 **/
PCMDMX_ERROR pcmDmx_ApplyFrame(HANDLE_PCM_DOWNMIX self, DMX_PCM *pPcmBuf,
                               const int32_t pcmBufSize, uint32_t frameSize,
                               int32_t *nChannels, int32_t fInterleaved,
                               AUDIO_CHANNEL_TYPE_t channelType[],
                               uint8_t channelIndices[],
                               const FDK_channelMapDescr_t *const mapDescr,
                               int32_t *pDmxOutScale) {
  PCM_DMX_USER_PARAMS *pParam = NULL;
  PCMDMX_ERROR errorStatus = PCMDMX_OK;
  DUAL_CHANNEL_MODE dualChannelMode;
  PCM_DMX_CHANNEL_MODE inChMode;
  PCM_DMX_CHANNEL_MODE outChMode;
  int32_t devNull; /* Just a dummy to avoid a lot of branches in the code */
  int32_t numOutChannels, numInChannels;
  int32_t inStride, outStride, offset;
  int32_t dmxMaxScale, dmxScale;
  int32_t slot;
  uint8_t inOffsetTable[(8)];

  DMX_BS_META_DATA bsMetaData;

  if ((self == NULL) || (nChannels == NULL) || (channelType == NULL) ||
      (channelIndices == NULL) || (!FDK_chMapDescr_isValid(mapDescr))) {
    return (PCMDMX_INVALID_HANDLE);
  }

  /* Init the output scaling */
  dmxScale = 0;
  if (pDmxOutScale != NULL) {
    /* Avoid final scaling internally and hand it to the outside world. */
    *pDmxOutScale = 0;
    dmxMaxScale = (3);
  } else {
    /* Apply the scaling internally. */
    pDmxOutScale = &devNull; /* redirect to temporal stack memory */
    dmxMaxScale = 0;
  }

  pParam = &self->userParams;
  numInChannels = *nChannels;

  /* Perform some input sanity checks */
  if (pPcmBuf == NULL) {
    return (PCMDMX_INVALID_ARGUMENT);
  }
  if (frameSize == 0) {
    return (PCMDMX_INVALID_ARGUMENT);
  }
  if (numInChannels == 0) {
    return (PCMDMX_INVALID_ARGUMENT);
  }
  if (numInChannels > (8)) {
    return (PCMDMX_INVALID_CH_CONFIG);
  }

  /* Check on misconfiguration */
  assert((pParam->numOutChannelsMax <= 0) ||
             (pParam->numOutChannelsMax >= pParam->numOutChannelsMin));

  /* Determine if the module has to do processing */
  if ((self->applyProcessing == 0) &&
      ((pParam->numOutChannelsMax <= 0) ||
       (pParam->numOutChannelsMax >= numInChannels)) &&
      (pParam->numOutChannelsMin <= numInChannels)) {
    /* Nothing to do */
    return (errorStatus);
  }

  /* Determine the number of output channels */
  if ((pParam->numOutChannelsMax > 0) &&
      (numInChannels > pParam->numOutChannelsMax)) {
    numOutChannels = pParam->numOutChannelsMax;
  } else if (numInChannels < pParam->numOutChannelsMin) {
    numOutChannels = pParam->numOutChannelsMin;
  } else {
    numOutChannels = numInChannels;
  }

  /* Check I/O buffer size */
  if ((uint32_t)pcmBufSize < (uint32_t)numOutChannels * frameSize) {
    return (PCMDMX_OUTPUT_BUFFER_TOO_SMALL);
  }

  dualChannelMode = pParam->dualChannelMode;

  /* Analyse input channel configuration and get channel offset
   * table that can be accessed with the fixed channel labels. */
  errorStatus = getChannelMode(numInChannels, channelType, channelIndices,
                               inOffsetTable, &inChMode);
  if (PCMDMX_IS_FATAL_ERROR(errorStatus) || (inChMode == CH_MODE_UNDEFINED)) {
    /* We don't need to restore because the channel
       configuration has not been changed. Just exit. */
    return (PCMDMX_INVALID_CH_CONFIG);
  }

  /* Set input stride and offset */
  if (fInterleaved) {
    inStride = numInChannels;
    offset = 1; /* Channel specific offset factor */
  } else {
    inStride = 1;
    offset = frameSize; /* Channel specific offset factor */
  }

  /* Reset downmix meta data if necessary */
  if ((pParam->expiryFrame > 0) &&
      (++self->bsMetaData[0].expiryCount >
       pParam
           ->expiryFrame)) { /* The metadata read from bitstream is too old. */

    PCMDMX_ERROR err = pcmDmx_Reset(self, PCMDMX_RESET_BS_DATA);
    assert(err == PCMDMX_OK);

  }
  memcpy(&bsMetaData, &self->bsMetaData[pParam->frameDelay],
            sizeof(DMX_BS_META_DATA));
  /* Maintain delay line */
  for (slot = pParam->frameDelay; slot > 0; slot -= 1) {
    memcpy(&self->bsMetaData[slot], &self->bsMetaData[slot - 1],
              sizeof(DMX_BS_META_DATA));
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   * - - - - - - - - - - - - - - - - - - */
  if (numInChannels > numOutChannels) { /* Apply downmix */
    DMX_PCM *pInPcm[(8)] = {NULL};
    DMX_PCM *pOutPcm[(8)] = {NULL};
    int16_t mixFactors[(8)][(8)];
    uint8_t outOffsetTable[(8)];
    uint32_t sample;
    int32_t chCfg = 0;
    int32_t bypScale = 0;

    if (numInChannels > SIX_CHANNEL) {
      AUDIO_CHANNEL_TYPE_t multiPurposeChType[2];

      /* Get the type of the multipurpose channels */
      multiPurposeChType[0] =
          channelType[inOffsetTable[LEFT_MULTIPRPS_CHANNEL]];
      multiPurposeChType[1] =
          channelType[inOffsetTable[RIGHT_MULTIPRPS_CHANNEL]];

      /* Check if the input configuration is one defined in the standard. */
      switch (inChMode) {
        case CH_MODE_5_0_2_1: /* chCfg 7 || 14 */
          /* Further analyse the input config to distinguish the two
           * CH_MODE_5_0_2_1 configs. */
          if ((multiPurposeChType[0] == ACT_FRONT_TOP) &&
              (multiPurposeChType[1] == ACT_FRONT_TOP)) {
            chCfg = 14;
          } else {
            chCfg = 7;
          }
          break;
        case CH_MODE_3_0_3_1: /* chCfg 11 */
          chCfg = 11;
          break;
        case CH_MODE_3_0_4_1: /* chCfg 12 */
          chCfg = 12;
          break;
        default:
          chCfg = 0; /* Not a known config */
          break;
      }
    }

    /* Set this stages output stride and channel mode: */
    outStride = (fInterleaved) ? numOutChannels : 1;
    outChMode = outChModeTable[numOutChannels];
    assert(outChMode != CH_MODE_UNDEFINED);

    /* Get channel description and channel mapping for the desired output
     * configuration. */
    getChannelDescription(outChMode, mapDescr, channelType, channelIndices,
                          outOffsetTable);
    /* Now there is no way back because we modified the channel configuration!
     */

    /* Create the DMX matrix */
    errorStatus =
        getMixFactors((chCfg > 0) ? 1 : 0,
                      (chCfg > 0) ? (PCM_DMX_CHANNEL_MODE)chCfg : inChMode,
                      outChMode, pParam, &bsMetaData, mixFactors, &dmxScale);
    /* No fatal errors can occur here. The function is designed to always return
       a valid matrix. The error code is used to signal configurations and
       matrices that are not conform to any standard. */

    /* Determine the final scaling */
    bypScale = fMin(dmxMaxScale, dmxScale);
    *pDmxOutScale += bypScale;
    dmxScale -= bypScale;

    { /* Set channel pointer for input. Remove empty cols. */
      int32_t inCh, outCh, map[(8)];
      int32_t ch = 0;
      for (inCh = 0; inCh < (8); inCh += 1) {
        if (inOffsetTable[inCh] < (uint8_t)numInChannels) {
          pInPcm[ch] = &pPcmBuf[inOffsetTable[inCh] * offset];
          map[ch++] = inCh;
        }
      }
      for (; ch < (8); ch += 1) {
        map[ch] = ch;
      }

      /* Remove unused cols from factor matrix */
      for (inCh = 0; inCh < numInChannels; inCh += 1) {
        if (inCh != map[inCh]) {
          for (outCh = 0; outCh < (8); outCh += 1) {
            mixFactors[outCh][inCh] = mixFactors[outCh][map[inCh]];
          }
        }
      }

      /* Set channel pointer for output. Remove empty cols. */
      ch = 0;
      for (outCh = 0; outCh < (8); outCh += 1) {
        if (outOffsetTable[outCh] < (uint8_t)numOutChannels) {
          pOutPcm[ch] = &pPcmBuf[outOffsetTable[outCh] * offset];
          map[ch++] = outCh;
        }
      }
      for (; ch < (8); ch += 1) {
        map[ch] = ch;
      }

      /* Remove unused rows from factor matrix */
      for (outCh = 0; outCh < numOutChannels; outCh += 1) {
        if (outCh != map[outCh]) {
          memcpy(&mixFactors[outCh], &mixFactors[map[outCh]],
                    (8) * sizeof(int16_t));
        }
      }
    }

    /* Sample processing loop */
    for (sample = 0; sample < frameSize; sample++) {
      DMX_PCM tIn[(8)] = {0};
      int32_t tOut[(8)] = {(int32_t)0};
      int32_t inCh, outCh;

      /* Preload all input samples */
      for (inCh = 0; inCh < numInChannels; inCh += 1) {
        if (pInPcm[inCh] != NULL) {
          tIn[inCh] = *pInPcm[inCh];
          pInPcm[inCh] += inStride;
        } else {
          tIn[inCh] = (DMX_PCM)0;
        }
      }
      /* Apply downmix coefficients to input samples and accumulate for output
       */
      for (outCh = 0; outCh < numOutChannels; outCh += 1) {
        for (inCh = 0; inCh < numInChannels; inCh += 1) {
          tOut[outCh] += fMult((DMX_PCMF)tIn[inCh], mixFactors[outCh][inCh]);
        }
        assert(pOutPcm[outCh] >= pPcmBuf);
        assert(pOutPcm[outCh] < &pPcmBuf[pcmBufSize]);
        /* Write sample */
        *pOutPcm[outCh] = (DMX_PCM)SATURATE_SHIFT(
            tOut[outCh], DFRACT_BITS - DMX_PCM_BITS - dmxScale, DMX_PCM_BITS);
        pOutPcm[outCh] += outStride;
      }
    }

    /* Update the number of output channels */
    *nChannels = numOutChannels;

  } /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
       - - - - - - - - - - - - - - - - - - */
  else if (numInChannels < numOutChannels) { /* Apply rudimentary upmix */
    /* Set up channel pointer */
    uint8_t outOffsetTable[(8)];

    /* FIRST STAGE
         Create a stereo/dual channel signal */
    if (numInChannels == ONE_CHANNEL) {
      DMX_PCM *pInPcm[(8)];
      DMX_PCM *pOutLF, *pOutRF;
      uint32_t sample;

      /* Set this stages output stride and channel mode: */
      outStride = (fInterleaved) ? TWO_CHANNEL : 1;
      outChMode = outChModeTable[TWO_CHANNEL];

      /* Get channel description and channel mapping for this
       * stages number of output channels (always STEREO). */
      getChannelDescription(outChMode, mapDescr, channelType, channelIndices,
                            outOffsetTable);
      /* Now there is no way back because we modified the channel configuration!
       */

      /* Set input channel pointer. The first channel is always at index 0. */
      pInPcm[CENTER_FRONT_CHANNEL] =
          &pPcmBuf[(frameSize - 1) *
                   inStride]; /* Considering input mapping could lead to a
                                 invalid pointer here if the channel is not
                                 declared to be a front channel. */

      /* Set output channel pointer (for this stage). */
      pOutLF = &pPcmBuf[outOffsetTable[LEFT_FRONT_CHANNEL] * offset +
                        (frameSize - 1) * outStride];
      pOutRF = &pPcmBuf[outOffsetTable[RIGHT_FRONT_CHANNEL] * offset +
                        (frameSize - 1) * outStride];

      /* 1/0 input: */
      for (sample = 0; sample < frameSize; sample++) {
        /* L' = C;  R' = C; */
        *pOutLF = *pOutRF = *pInPcm[CENTER_FRONT_CHANNEL];

        pInPcm[CENTER_FRONT_CHANNEL] -= inStride;
        pOutLF -= outStride;
        pOutRF -= outStride;
      }

      /* Prepare for next stage: */
      inStride = outStride;
      inChMode = outChMode;
      memcpy(inOffsetTable, outOffsetTable, (8) * sizeof(uint8_t));
    }

    /* SECOND STAGE
         Extend with zero channels to achieved the desired number of output
       channels. */
    if (numOutChannels > TWO_CHANNEL) {
      DMX_PCM *pIn[(8)] = {NULL};
      DMX_PCM *pOut[(8)] = {NULL};
      uint32_t sample;
      AUDIO_CHANNEL_TYPE_t inChTypes[(8)];
      uint8_t inChIndices[(8)];
      uint8_t numChPerGrp[2][(4)];
      int32_t nContentCh = 0; /* Number of channels with content */
      int32_t nEmptyCh = 0;   /* Number of channels with content */
      int32_t ch, chGrp, isCompatible = 1;

      /* Do not change the signalling which is the channel types and indices.
         Just reorder and add channels. So first save the input signalling. */
      memcpy(inChTypes, channelType,
                numInChannels * sizeof(AUDIO_CHANNEL_TYPE_t));
      memset(inChTypes + numInChannels, 0,
                  ((8) - numInChannels) * sizeof(AUDIO_CHANNEL_TYPE_t));
      memcpy(inChIndices, channelIndices, numInChannels * sizeof(uint8_t));
      memset(inChIndices + numInChannels, 0,
                  ((8) - numInChannels) * sizeof(uint8_t));

      /* Set this stages output stride and channel mode: */
      outStride = (fInterleaved) ? numOutChannels : 1;
      outChMode = outChModeTable[numOutChannels];
      assert(outChMode != CH_MODE_UNDEFINED);

      /* Check if input channel config can be easily mapped to the desired
       * output config. */
      for (chGrp = 0; chGrp < (4); chGrp += 1) {
        numChPerGrp[IN][chGrp] = (inChMode >> (chGrp * 4)) & 0xF;
        numChPerGrp[OUT][chGrp] = (outChMode >> (chGrp * 4)) & 0xF;

        if (numChPerGrp[IN][chGrp] > numChPerGrp[OUT][chGrp]) {
          isCompatible = 0;
          break;
        }
      }

      if (isCompatible) {
        /* Get new channel description and channel
         * mapping for the desired output channel mode. */
        getChannelDescription(outChMode, mapDescr, channelType, channelIndices,
                              outOffsetTable);
        /* If the input config has a back center channel but the output
           config has not, copy it to left and right (if available). */
        if ((numChPerGrp[IN][CH_GROUP_REAR] % 2) &&
            !(numChPerGrp[OUT][CH_GROUP_REAR] % 2)) {
          if (numChPerGrp[IN][CH_GROUP_REAR] == 1) {
            inOffsetTable[RIGHT_REAR_CHANNEL] =
                inOffsetTable[LEFT_REAR_CHANNEL];
          } else if (numChPerGrp[IN][CH_GROUP_REAR] == 3) {
            inOffsetTable[RIGHT_MULTIPRPS_CHANNEL] =
                inOffsetTable[LEFT_MULTIPRPS_CHANNEL];
          }
        }
      } else {
        /* Just copy and extend the original config */
        memcpy(outOffsetTable, inOffsetTable, (8) * sizeof(uint8_t));
      }

      /* Set I/O channel pointer.
         Note: The following assignment algorithm clears the channel offset
         tables. Thus they can not be used afterwards. */
      for (ch = 0; ch < (8); ch += 1) {
        if ((outOffsetTable[ch] < 255) &&
            (inOffsetTable[ch] < 255)) { /* Set I/O pointer: */
          pIn[nContentCh] =
              &pPcmBuf[inOffsetTable[ch] * offset + (frameSize - 1) * inStride];
          pOut[nContentCh] = &pPcmBuf[outOffsetTable[ch] * offset +
                                      (frameSize - 1) * outStride];
          /* Update signalling */
          channelType[outOffsetTable[ch]] = inChTypes[inOffsetTable[ch]];
          channelIndices[outOffsetTable[ch]] = inChIndices[inOffsetTable[ch]];
          inOffsetTable[ch] = 255;
          outOffsetTable[ch] = 255;
          nContentCh += 1;
        }
      }
      if (isCompatible) {
        /* Assign the remaining input channels.
           This is just a safety appliance. We should never need it. */
        for (ch = 0; ch < (8); ch += 1) {
          if (inOffsetTable[ch] < 255) {
            int32_t outCh;
            for (outCh = 0; outCh < (8); outCh += 1) {
              if (outOffsetTable[outCh] < 255) {
                break;
              }
            }
            if (outCh >= (8)) {
              assert(0);
              break;
            }
            /* Set I/O pointer: */
            pIn[nContentCh] = &pPcmBuf[inOffsetTable[ch] * offset +
                                       (frameSize - 1) * inStride];
            pOut[nContentCh] = &pPcmBuf[outOffsetTable[outCh] * offset +
                                        (frameSize - 1) * outStride];
            /* Update signalling */
            assert(inOffsetTable[outCh] < numInChannels);
            assert(outOffsetTable[outCh] < numOutChannels);
            channelType[outOffsetTable[outCh]] = inChTypes[inOffsetTable[ch]];
            channelIndices[outOffsetTable[outCh]] =
                inChIndices[inOffsetTable[ch]];
            inOffsetTable[ch] = 255;
            outOffsetTable[outCh] = 255;
            nContentCh += 1;
          }
        }
        /* Set the remaining output channel pointer */
        for (ch = 0; ch < (8); ch += 1) {
          if (outOffsetTable[ch] < 255) {
            pOut[nContentCh + nEmptyCh] = &pPcmBuf[outOffsetTable[ch] * offset +
                                                   (frameSize - 1) * outStride];
            /* Expand output signalling */
            channelType[outOffsetTable[ch]] = ACT_NONE;
            channelIndices[outOffsetTable[ch]] = (uint8_t)nEmptyCh;
            outOffsetTable[ch] = 255;
            nEmptyCh += 1;
          }
        }
      } else {
        /* Set the remaining output channel pointer */
        for (ch = nContentCh; ch < numOutChannels; ch += 1) {
          pOut[ch] = &pPcmBuf[ch * offset + (frameSize - 1) * outStride];
          /* Expand output signalling */
          channelType[ch] = ACT_NONE;
          channelIndices[ch] = (uint8_t)nEmptyCh;
          nEmptyCh += 1;
        }
      }

      /* First copy the channels that have signal */
      for (sample = 0; sample < frameSize; sample += 1) {
        DMX_PCM tIn[(8)];
        /* Read all channel samples */
        for (ch = 0; ch < nContentCh; ch += 1) {
          tIn[ch] = *pIn[ch];
          pIn[ch] -= inStride;
        }
        /* Write all channel samples */
        for (ch = 0; ch < nContentCh; ch += 1) {
          *pOut[ch] = tIn[ch];
          pOut[ch] -= outStride;
        }
      }

      /* Clear all the other channels */
      for (sample = 0; sample < frameSize; sample++) {
        for (ch = nContentCh; ch < numOutChannels; ch += 1) {
          *pOut[ch] = (DMX_PCM)0;
          pOut[ch] -= outStride;
        }
      }
    }

    /* update the number of output channels */
    *nChannels = numOutChannels;
  } /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
       - - - - - - - - - - - - - - - - - - */
  else if (numInChannels == numOutChannels) {
    /* Don't need to change the channel description here */

    switch (numInChannels) {
      case 2: { /* Set up channel pointer */
        DMX_PCM *pInPcm[(8)];
        DMX_PCM *pOutL, *pOutR;
        int16_t flev;

        uint32_t sample;

        if (fInterleaved) {
          inStride = numInChannels;
          outStride =
              2; /* fixed !!! (below stereo is donwmixed to mono if required */
          offset = 1; /* Channel specific offset factor */
        } else {
          inStride = 1;
          outStride = 1;
          offset = frameSize; /* Channel specific offset factor */
        }

        /* Set input channel pointer */
        pInPcm[LEFT_FRONT_CHANNEL] =
            &pPcmBuf[inOffsetTable[LEFT_FRONT_CHANNEL] * offset];
        pInPcm[RIGHT_FRONT_CHANNEL] =
            &pPcmBuf[inOffsetTable[RIGHT_FRONT_CHANNEL] * offset];

        /* Set output channel pointer (same as input) */
        pOutL = pInPcm[LEFT_FRONT_CHANNEL];
        pOutR = pInPcm[RIGHT_FRONT_CHANNEL];

        /* Set downmix levels: */
        flev =   23170;
        /* 2/0 input: */
        switch (dualChannelMode) {
          case CH1_MODE: /* L' = 0.707 * Ch1;  R' = 0.707 * Ch1 */
            for (sample = 0; sample < frameSize; sample++) {
              *pOutL = *pOutR = (DMX_PCM)SATURATE_RIGHT_SHIFT(
                  fMult((DMX_PCMF)*pInPcm[LEFT_FRONT_CHANNEL], flev),
                  DFRACT_BITS - DMX_PCM_BITS, DMX_PCM_BITS);

              pInPcm[LEFT_FRONT_CHANNEL] += inStride;
              pOutL += outStride;
              pOutR += outStride;
            }
            break;
          case CH2_MODE: /* L' = 0.707 * Ch2;  R' = 0.707 * Ch2 */
            for (sample = 0; sample < frameSize; sample++) {
              *pOutL = *pOutR = (DMX_PCM)SATURATE_RIGHT_SHIFT(
                  fMult((DMX_PCMF)*pInPcm[RIGHT_FRONT_CHANNEL], flev),
                  DFRACT_BITS - DMX_PCM_BITS, DMX_PCM_BITS);

              pInPcm[RIGHT_FRONT_CHANNEL] += inStride;
              pOutL += outStride;
              pOutR += outStride;
            }
            break;
          case MIXED_MODE: /* L' = 0.5*Ch1 + 0.5*Ch2;  R' = 0.5*Ch1 + 0.5*Ch2 */
            for (sample = 0; sample < frameSize; sample++) {
              *pOutL = *pOutR = (*pInPcm[LEFT_FRONT_CHANNEL] >> 1) +
                                (*pInPcm[RIGHT_FRONT_CHANNEL] >> 1);

              pInPcm[LEFT_FRONT_CHANNEL] += inStride;
              pInPcm[RIGHT_FRONT_CHANNEL] += inStride;
              pOutL += outStride;
              pOutR += outStride;
            }
            break;
          default:
          case STEREO_MODE:
            /* nothing to do */
            break;
        }
      } break;

      default:
        /* nothing to do */
        break;
    }
  }

  return (errorStatus);
}

/** Close an instance of the PCM downmix module.
 * @param [inout] Pointer to a buffer containing the handle of the instance.
 * @returns Returns an error code.
 **/
PCMDMX_ERROR pcmDmx_Close(HANDLE_PCM_DOWNMIX *pSelf) {
  if (pSelf == NULL) {
    return (PCMDMX_INVALID_HANDLE);
  }
  free(pSelf); pSelf = NULL;
  //*pSelf = NULL;

  return (PCMDMX_OK);
}

/** Get library info for this module.
 * @param [out] Pointer to an allocated LIB_INFO structure.
 * @returns Returns an error code.
 */
PCMDMX_ERROR pcmDmx_GetLibInfo(LIB_INFO_t *info) {
  int32_t i;

  if (info == NULL) {
    return PCMDMX_INVALID_ARGUMENT;
  }

  /* Search for next free tab */
  for (i = 0; i < FDK_MODULE_LAST; i++) {
    if (info[i].module_id == FDK_NONE) break;
  }
  if (i == FDK_MODULE_LAST) {
    return PCMDMX_INVALID_ARGUMENT;
  }


  /* Set flags */
  info[i].flags = 0 | CAPF_DMX_BLIND /* At least blind downmixing is possible */
                  | CAPF_DMX_PCE     /* Guided downmix with data from MPEG-2/4
                                        Program Config Elements (PCE). */
                  | CAPF_DMX_ARIB /* PCE guided downmix with slightly different
                                     equations and levels. */
                  | CAPF_DMX_DVB  /* Guided downmix with data from DVB ancillary
                                     data fields. */
                  | CAPF_DMX_CH_EXP /* Simple upmixing by dublicating channels
                                       or adding zero channels. */
                  | CAPF_DMX_6_CH | CAPF_DMX_8_CH;

  /* Add lib info for FDK tools (if not yet done). */
  FDK_toolsGetLibInfo(info);

  return PCMDMX_OK;
}
