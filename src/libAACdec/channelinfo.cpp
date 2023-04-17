

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: individual channel stream info

*******************************************************************************/
#include "newAACDecoder.h"
#include "channelinfo.h"
#include "aac_rom.h"
#include "../libFDK/FDK_bitstream.h"

AAC_DECODER_ERROR IcsReadMaxSfb(HANDLE_FDK_BITSTREAM bs, CIcsInfo *pIcsInfo,
                                const SamplingRateInfo *pSamplingRateInfo) {
  AAC_DECODER_ERROR ErrorStatus = AAC_DEC_OK;
  int32_t nbits;

  if (IsLongBlock(pIcsInfo)) {
    nbits = 6;
    pIcsInfo->TotalSfBands = pSamplingRateInfo->NumberOfScaleFactorBands_Long;
  } else {
    nbits = 4;
    pIcsInfo->TotalSfBands = pSamplingRateInfo->NumberOfScaleFactorBands_Short;
  }
  pIcsInfo->MaxSfBands = (uint8_t)FDKreadBits(bs, nbits);

  if (pIcsInfo->MaxSfBands > pIcsInfo->TotalSfBands) {
    ErrorStatus = AAC_DEC_PARSE_ERROR;
  }

  return ErrorStatus;
}

AAC_DECODER_ERROR IcsRead(HANDLE_FDK_BITSTREAM bs, CIcsInfo *pIcsInfo,
                          const SamplingRateInfo *pSamplingRateInfo,
                          const uint32_t flags) {
  AAC_DECODER_ERROR ErrorStatus = AAC_DEC_OK;

  pIcsInfo->Valid = 0;

  if (flags & AC_ELD) {
    pIcsInfo->WindowSequence = BLOCK_LONG;
    pIcsInfo->WindowShape = 0;
  } else {
    if (!(flags & (AC_USAC | AC_RSVD50 | AC_RSV603DA))) {
      FDKreadBits(bs, 1);
    }
    pIcsInfo->WindowSequence = (BLOCK_TYPE)FDKreadBits(bs, 2);
    pIcsInfo->WindowShape = (uint8_t)FDKreadBits(bs, 1);
    if (flags & AC_LD) {
      if (pIcsInfo->WindowShape) {
        pIcsInfo->WindowShape = 2; /* select low overlap instead of KBD */
      }
    }
  }

  /* Sanity check */
  if ((flags & (AC_ELD | AC_LD)) && pIcsInfo->WindowSequence != BLOCK_LONG) {
    pIcsInfo->WindowSequence = BLOCK_LONG;
    ErrorStatus = AAC_DEC_PARSE_ERROR;
    goto bail;
  }

  ErrorStatus = IcsReadMaxSfb(bs, pIcsInfo, pSamplingRateInfo);
  if (ErrorStatus != AAC_DEC_OK) {
    goto bail;
  }

  if (IsLongBlock(pIcsInfo)) {
    if (!(flags & (AC_ELD | AC_SCALABLE | AC_BSAC | AC_USAC | AC_RSVD50 |
                   AC_RSV603DA))) /* If not ELD nor Scalable nor BSAC nor USAC
                                     syntax then ... */
    {
      if ((uint8_t)FDKreadBits(bs, 1) != 0) /* uint8_t PredictorDataPresent */
      {
        ErrorStatus = AAC_DEC_UNSUPPORTED_PREDICTION;
        goto bail;
      }
    }

    pIcsInfo->WindowGroups = 1;
    pIcsInfo->WindowGroupLength[0] = 1;
  } else {
    int32_t i;
    uint32_t mask;

    pIcsInfo->ScaleFactorGrouping = (uint8_t)FDKreadBits(bs, 7);

    pIcsInfo->WindowGroups = 0;

    for (i = 0; i < (8 - 1); i++) {
      mask = 1 << (6 - i);
      pIcsInfo->WindowGroupLength[i] = 1;

      if (pIcsInfo->ScaleFactorGrouping & mask) {
        pIcsInfo->WindowGroupLength[pIcsInfo->WindowGroups]++;
      } else {
        pIcsInfo->WindowGroups++;
      }
    }

    /* loop runs to i < 7 only */
    pIcsInfo->WindowGroupLength[8 - 1] = 1;
    pIcsInfo->WindowGroups++;
  }

bail:
  if (ErrorStatus == AAC_DEC_OK) pIcsInfo->Valid = 1;

  return ErrorStatus;
}

/*
  interleave codebooks the following way

    9 (84w) |  1 (51w)
   10 (82w) |  2 (39w)
  SCL (65w) |  4 (38w)
    3 (39w) |  5 (41w)
            |  6 (40w)
            |  7 (31w)
            |  8 (31w)
     (270w)     (271w)
*/

/*
  Table entries are sorted as following:
  | num_swb_long_window | sfbands_long | num_swb_short_window | sfbands_short |
*/
AAC_DECODER_ERROR getSamplingRateInfo(SamplingRateInfo *t, uint32_t samplesPerFrame,
                                      uint32_t samplingRateIndex,
                                      uint32_t samplingRate) {
  int32_t index = 0;

  /* Search closest samplerate according to ISO/IEC 13818-7:2005(E) 8.2.4 (Table
   * 38): */
  if ((samplingRateIndex >= 15) || (samplesPerFrame == 768)) {
    const uint32_t borders[] = {(uint32_t)-1, 92017, 75132, 55426, 46009, 37566,
                            27713,    23004, 18783, 13856, 11502, 9391};
    uint32_t i, samplingRateSearch = samplingRate;

    if (samplesPerFrame == 768) {
      samplingRateSearch = (samplingRate * 4) / 3;
    }

    for (i = 0; i < 11; i++) {
      if (borders[i] > samplingRateSearch &&
          samplingRateSearch >= borders[i + 1]) {
        break;
      }
    }
    samplingRateIndex = i;
  }

  t->samplingRateIndex = samplingRateIndex;
  t->samplingRate = samplingRate;

  switch (samplesPerFrame) {
    case 1024:
      index = 0;
      break;
    case 960:
      index = 1;
      break;
    case 768:
      index = 2;
      break;
    case 512:
      index = 3;
      break;
    case 480:
      index = 4;
      break;

    default:
      return AAC_DEC_UNSUPPORTED_FORMAT;
  }

  t->ScaleFactorBands_Long =
      sfbOffsetTables[index][samplingRateIndex].sfbOffsetLong;
  t->ScaleFactorBands_Short =
      sfbOffsetTables[index][samplingRateIndex].sfbOffsetShort;
  t->NumberOfScaleFactorBands_Long =
      sfbOffsetTables[index][samplingRateIndex].numberOfSfbLong;
  t->NumberOfScaleFactorBands_Short =
      sfbOffsetTables[index][samplingRateIndex].numberOfSfbShort;

  if (t->ScaleFactorBands_Long == NULL ||
      t->NumberOfScaleFactorBands_Long == 0) {
    t->samplingRate = 0;
    return AAC_DEC_UNSUPPORTED_FORMAT;
  }

  assert((uint32_t)t->ScaleFactorBands_Long[t->NumberOfScaleFactorBands_Long] ==
             samplesPerFrame);
  assert(
      t->ScaleFactorBands_Short == NULL ||
      (uint32_t)t->ScaleFactorBands_Short[t->NumberOfScaleFactorBands_Short] * 8 ==
          samplesPerFrame);

  return AAC_DEC_OK;
}
