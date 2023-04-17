
/**************************** AAC decoder library ******************************

   Author(s):   Christian Griebel

   Description: Dynamic range control (DRC) decoder tool for AAC

*******************************************************************************/

#include <memory.h>
#include <stdint.h>
#include "newAACDecoder.h"
#include "aacdec_drc.h"

#include "channelinfo.h"
#include "aac_rom.h"

#include "../libSBRdec/sbrdecoder.h"

/*
 * Dynamic Range Control
 */

/* For parameter conversion */
#define DRC_PARAMETER_BITS (7)
#define DRC_MAX_QUANT_STEPS (1 << DRC_PARAMETER_BITS)
#define DRC_MAX_QUANT_FACTOR (DRC_MAX_QUANT_STEPS - 1)
#define DRC_PARAM_QUANT_STEP \
  (FL2FXCONST_DBL(1.0f / (float)DRC_MAX_QUANT_FACTOR))
#define DRC_PARAM_SCALE (1)
#define DRC_SCALING_MAX \
  ((int32_t)((int32_t)(DRC_PARAM_QUANT_STEP >> DRC_PARAM_SCALE) * (int32_t)127))

#define DRC_BLOCK_LEN (1024)
#define DRC_BAND_MULT (4)
#define DRC_BLOCK_LEN_DIV_BAND_MULT (DRC_BLOCK_LEN / DRC_BAND_MULT)

#define MAX_REFERENCE_LEVEL (127)

#define DRC_HEAVY_THRESHOLD_DB (10)

#define DVB_ANC_DATA_SYNC_BYTE (0xBC) /* DVB ancillary data sync byte. */

#define OFF 0
#define ON 1

static int32_t convert_drcParam(int32_t param_dbl) {
  /* converts an internal DRC boost/cut scaling factor in int32_t
     (which is downscaled by DRC_PARAM_SCALE)
     back to an integer value between 0 and 127. */
  int32_t param_long;

  param_long = (int32_t)param_dbl >> 7;
  param_long = param_long * (int32_t)DRC_MAX_QUANT_FACTOR;
  param_long >>= 31 - 7 - DRC_PARAM_SCALE - 1;
  param_long += 1; /* for rounding */
  param_long >>= 1;

  return (int32_t)param_long;
}

/*!
\brief Reset DRC information

\self Handle of DRC info

\return none
*/
void aacDecoder_drcReset(HANDLE_AAC_DRC self) {
  self->applyExtGain = 0;
  self->additionalGainPrev = AACDEC_DRC_GAIN_INIT_VALUE;
  self->additionalGainFilterState = AACDEC_DRC_GAIN_INIT_VALUE;
  self->additionalGainFilterState1 = AACDEC_DRC_GAIN_INIT_VALUE;
}

/*!
  \brief Initialize DRC information

  \self Handle of DRC info

  \return none
*/
void aacDecoder_drcInit(HANDLE_AAC_DRC self) {
  CDrcParams_t *pParams;

  if (self == NULL) {
    return;
  }

  /* init control fields */
  self->enable = OFF;
  self->numThreads = 0;

  /* init params */
  pParams = &self->params;
  pParams->bsDelayEnable = 0;
  pParams->cut = FL2FXCONST_DBL(0.0f);
  pParams->usrCut = FL2FXCONST_DBL(0.0f);
  pParams->boost = FL2FXCONST_DBL(0.0f);
  pParams->usrBoost = FL2FXCONST_DBL(0.0f);
  pParams->targetRefLevel = 96;
  pParams->expiryFrame = AACDEC_DRC_DFLT_EXPIRY_FRAMES;
  pParams->applyHeavyCompression = OFF;
  pParams->usrApplyHeavyCompression = OFF;

  pParams->defaultPresentationMode = DISABLED_PARAMETER_HANDLING;
  pParams->encoderTargetLevel = MAX_REFERENCE_LEVEL; /* worst case assumption */

  self->update = 1;
  self->numOutChannels = 0;
  self->prevAacNumChannels = 0;

  /* initial program ref level = target ref level */
  self->progRefLevel = pParams->targetRefLevel;
  self->progRefLevelPresent = 0;
  self->presMode = -1;
  self->uniDrcPrecedence = 0;

  aacDecoder_drcReset(self);
}

/*!
  \brief Initialize DRC control data for one channel

  \self Handle of DRC info

  \return none
*/
void aacDecoder_drcInitChannelData(CDrcChannelData_t *pDrcChData) {
  if (pDrcChData != NULL) {
    pDrcChData->expiryCount = 0;
    pDrcChData->numBands = 1;
    pDrcChData->bandTop[0] = DRC_BLOCK_LEN_DIV_BAND_MULT - 1;
    pDrcChData->drcValue[0] = 0;
    pDrcChData->drcInterpolationScheme = 0;
    pDrcChData->drcDataType = UNKNOWN_PAYLOAD;
  }
}

/*!
  \brief  Set one single DRC parameter

  \self   Handle of DRC info.
  \param  Parameter to be set.
  \value  Value to be set.

  \return an error code.
*/
AAC_DECODER_ERROR aacDecoder_drcSetParam(HANDLE_AAC_DRC self,
                                         AACDEC_DRC_PARAM param, int32_t value) {
  AAC_DECODER_ERROR ErrorStatus = AAC_DEC_OK;

  switch (param) {
    case DRC_CUT_SCALE:
      /* set attenuation scale factor */
      if ((value < 0) || (value > DRC_MAX_QUANT_FACTOR)) {
        return AAC_DEC_SET_PARAM_FAIL;
      }
      if (self == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      }
      self->params.usrCut = (int32_t)(
          (int32_t)(DRC_PARAM_QUANT_STEP >> DRC_PARAM_SCALE) * (int32_t)value);
      self->update = 1;
      break;
    case DRC_BOOST_SCALE:
      /* set boost factor */
      if ((value < 0) || (value > DRC_MAX_QUANT_FACTOR)) {
        return AAC_DEC_SET_PARAM_FAIL;
      }
      if (self == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      }
      self->params.usrBoost = (int32_t)(
          (int32_t)(DRC_PARAM_QUANT_STEP >> DRC_PARAM_SCALE) * (int32_t)value);
      self->update = 1;
      break;
    case TARGET_REF_LEVEL:
      if (value > MAX_REFERENCE_LEVEL || value < -MAX_REFERENCE_LEVEL) {
        return AAC_DEC_SET_PARAM_FAIL;
      }
      if (self == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      }
      if (value < 0) {
        self->params.targetRefLevel = -1;
      } else {
        if (self->params.targetRefLevel != (int8_t)value) {
          self->params.targetRefLevel = (int8_t)value;
          self->progRefLevel = (int8_t)value; /* Always set the program reference
                                                level equal to the target level
                                                according to 4.5.2.7.3 of
                                                ISO/IEC 14496-3. */
        }
        self->update = 1;
      }
      break;
    case APPLY_HEAVY_COMPRESSION:
      if ((value != OFF) && (value != ON)) {
        return AAC_DEC_SET_PARAM_FAIL;
      }
      if (self == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      }
      /* Store new parameter value */
      self->params.usrApplyHeavyCompression = (uint8_t)value;
      self->update = 1;
      break;
    case DEFAULT_PRESENTATION_MODE:
      if (value < AAC_DRC_PARAMETER_HANDLING_DISABLED ||
          value > AAC_DRC_PRESENTATION_MODE_2_DEFAULT) {
        return AAC_DEC_SET_PARAM_FAIL;
      }
      if (self == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      }
      self->params.defaultPresentationMode =
          (AACDEC_DRC_PARAMETER_HANDLING_t)value;
      self->update = 1;
      break;
    case ENCODER_TARGET_LEVEL:
      if (value > MAX_REFERENCE_LEVEL || value < 0) {
        return AAC_DEC_SET_PARAM_FAIL;
      }
      if (self == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      }
      self->params.encoderTargetLevel = (uint8_t)value;
      self->update = 1;
      break;
    case DRC_BS_DELAY:
      if (value < 0 || value > 1) {
        return AAC_DEC_SET_PARAM_FAIL;
      }
      if (self == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      }
      self->params.bsDelayEnable = value;
      break;
    case DRC_DATA_EXPIRY_FRAME:
      if (self == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      }
      self->params.expiryFrame = (value > 0) ? (uint32_t)value : 0;
      break;
    case MAX_OUTPUT_CHANNELS:
      if (self == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      }
      self->numOutChannels = (int32_t)value;
      self->update = 1;
      break;
    case UNIDRC_PRECEDENCE:
      if (self == NULL) {
        return AAC_DEC_INVALID_HANDLE;
      }
      self->uniDrcPrecedence = (uint8_t)value;
      break;
    default:
      return AAC_DEC_SET_PARAM_FAIL;
  } /* switch(param) */

  return ErrorStatus;
}

static int32_t parseExcludedChannels(uint32_t *excludedChnsMask,
                                 HANDLE_FDK_BITSTREAM bs) {
  uint32_t excludeMask = 0;
  uint32_t i, j;
  int32_t bitCnt = 9;

  for (i = 0, j = 1; i < 7; i++, j <<= 1) {
    if (FDKreadBits(bs, 1)) {
      excludeMask |= j;
    }
  }

  /* additional_excluded_chns */
  while (FDKreadBits(bs, 1)) {
    for (i = 0; i < 7; i++, j <<= 1) {
      if (FDKreadBits(bs, 1)) {
        excludeMask |= j;
      }
    }
    bitCnt += 9;
    assert(j < (uint32_t)-1);
  }

  *excludedChnsMask = excludeMask;

  return (bitCnt);
}

/*!
  \brief Save DRC payload bitstream position

  \self Handle of DRC info
  \bs Handle of FDK bitstream

  \return The number of DRC payload bits
*/
int32_t aacDecoder_drcMarkPayload(HANDLE_AAC_DRC self, HANDLE_FDK_BITSTREAM bs,
                              AACDEC_DRC_PAYLOAD_TYPE type) {
  uint32_t bsStartPos;
  int32_t i, numBands = 1, bitCnt = 0;

  if (self == NULL) {
    return 0;
  }

  bsStartPos = FDKgetValidBits(bs);

  switch (type) {
    case MPEG_DRC_EXT_DATA: {
      bitCnt = 4;

      if (FDKreadBits(bs, 1)) { /* pce_tag_present */
        FDKreadBits(bs, 8);     /* pce_instance_tag + drc_tag_reserved_bits */
        bitCnt += 8;
      }

      if (FDKreadBits(bs, 1)) { /* excluded_chns_present */
        FDKreadBits(bs, 7);     /* exclude mask [0..7] */
        bitCnt += 8;
        while (FDKreadBits(bs, 1)) { /* additional_excluded_chns */
          FDKreadBits(bs, 7);        /* exclude mask [x..y] */
          bitCnt += 8;
        }
      }

      if (FDKreadBits(bs, 1)) {         /* drc_bands_present */
        numBands += FDKreadBits(bs, 4); /* drc_band_incr */
        FDKreadBits(bs, 4);             /* reserved */
        bitCnt += 8;
        for (i = 0; i < numBands; i++) {
          FDKreadBits(bs, 8); /* drc_band_top[i] */
          bitCnt += 8;
        }
      }

      if (FDKreadBits(bs, 1)) { /* prog_ref_level_present */
        FDKreadBits(bs, 8); /* prog_ref_level + prog_ref_level_reserved_bits */
        bitCnt += 8;
      }

      for (i = 0; i < numBands; i++) {
        FDKreadBits(bs, 8); /* dyn_rng_sgn[i] + dyn_rng_ctl[i] */
        bitCnt += 8;
      }

      if ((self->numPayloads < MAX_DRC_THREADS) &&
          ((int32_t)FDKgetValidBits(bs) >= 0)) {
        self->drcPayloadPosition[self->numPayloads++] = bsStartPos;
      }
    } break;

    case DVB_DRC_ANC_DATA:
      bitCnt += 8;
      /* check sync word */
      if (FDKreadBits(bs, 8) == DVB_ANC_DATA_SYNC_BYTE) {
        int32_t dmxLevelsPresent, compressionPresent;
        int32_t coarseGrainTcPresent, fineGrainTcPresent;

        /* bs_info field */
        FDKreadBits(
            bs,
            8); /* mpeg_audio_type, dolby_surround_mode, presentation_mode */
        bitCnt += 8;

        /* Evaluate ancillary_data_status */
        FDKreadBits(bs, 3); /* reserved, set to 0 */
        dmxLevelsPresent =
            FDKreadBits(bs, 1); /* downmixing_levels_MPEG4_status */
        FDKreadBits(bs, 1);     /* reserved, set to 0 */
        compressionPresent =
            FDKreadBits(bs, 1); /* audio_coding_mode_and_compression status */
        coarseGrainTcPresent =
            FDKreadBits(bs, 1); /* coarse_grain_timecode_status */
        fineGrainTcPresent =
            FDKreadBits(bs, 1); /* fine_grain_timecode_status */
        bitCnt += 8;

        /* MPEG4 downmixing levels */
        if (dmxLevelsPresent) {
          FDKreadBits(bs, 8); /* downmixing_levels_MPEG4 */
          bitCnt += 8;
        }
        /* audio coding mode and compression status */
        if (compressionPresent) {
          FDKreadBits(bs, 16); /* audio_coding_mode, Compression_value */
          bitCnt += 16;
        }
        /* coarse grain timecode */
        if (coarseGrainTcPresent) {
          FDKreadBits(bs, 16); /* coarse_grain_timecode */
          bitCnt += 16;
        }
        /* fine grain timecode */
        if (fineGrainTcPresent) {
          FDKreadBits(bs, 16); /* fine_grain_timecode */
          bitCnt += 16;
        }
        if (!self->dvbAncDataAvailable && ((int32_t)FDKgetValidBits(bs) >= 0)) {
          self->dvbAncDataPosition = bsStartPos;
          self->dvbAncDataAvailable = 1;
        }
      }
      break;

    default:
      break;
  }

  return (bitCnt);
}

/*!
  \brief Parse DRC parameters from bitstream

  \bs Handle of FDK bitstream (in)
  \pDrcBs Pointer to DRC payload data container (out)
  \payloadPosition Bitstream position of MPEG DRC data chunk (in)

  \return Flag telling whether new DRC data has been found or not.
*/
static int32_t aacDecoder_drcParse(HANDLE_FDK_BITSTREAM bs, CDrcPayload_t *pDrcBs,
                               uint32_t payloadPosition) {
  int32_t i, numBands;

  /* Move to the beginning of the DRC payload field */
  FDKpushBiDirectional(bs, (int32_t)FDKgetValidBits(bs) - (int32_t)payloadPosition);

  /* pce_tag_present */
  if (FDKreadBits(bs, 1)) {
    pDrcBs->pceInstanceTag = FDKreadBits(bs, 4); /* pce_instance_tag */
    /* only one program supported */
    FDKreadBits(bs, 4); /* drc_tag_reserved_bits */
  } else {
    pDrcBs->pceInstanceTag = -1; /* not present */
  }

  if (FDKreadBits(bs, 1)) { /* excluded_chns_present */
    /* get excluded_chn_mask */
    parseExcludedChannels(&pDrcBs->excludedChnsMask, bs);
  } else {
    pDrcBs->excludedChnsMask = 0;
  }

  numBands = 1;
  if (FDKreadBits(bs, 1)) /* drc_bands_present */
  {
    /* get band_incr */
    numBands += FDKreadBits(bs, 4); /* drc_band_incr */
    pDrcBs->channelData.drcInterpolationScheme =
        FDKreadBits(bs, 4); /* drc_interpolation_scheme */
    /* band_top */
    for (i = 0; i < numBands; i++) {
      pDrcBs->channelData.bandTop[i] = FDKreadBits(bs, 8); /* drc_band_top[i] */
    }
  } else {
    pDrcBs->channelData.bandTop[0] = DRC_BLOCK_LEN_DIV_BAND_MULT -
                                     1; /* ... comprising the whole spectrum. */
    ;
  }

  pDrcBs->channelData.numBands = numBands;

  if (FDKreadBits(bs, 1)) /* prog_ref_level_present */
  {
    pDrcBs->progRefLevel = FDKreadBits(bs, 7); /* prog_ref_level */
    FDKreadBits(bs, 1); /* prog_ref_level_reserved_bits */
  } else {
    pDrcBs->progRefLevel = -1;
  }

  for (i = 0; i < numBands; i++) {
    pDrcBs->channelData.drcValue[i] = FDKreadBits(bs, 1)
                                      << 7; /* dyn_rng_sgn[i] */
    pDrcBs->channelData.drcValue[i] |=
        FDKreadBits(bs, 7) & 0x7F; /* dyn_rng_ctl[i] */
  }

  /* Set DRC payload type */
  pDrcBs->channelData.drcDataType = MPEG_DRC_EXT_DATA;

  return (1);
}

/*!
  \brief Parse heavy compression value transported in DSEs of DVB streams with
  MPEG-4 content.

  \bs Handle of FDK bitstream (in)
  \pDrcBs Pointer to DRC payload data container (out)
  \payloadPosition Bitstream position of DVB ancillary data chunk

  \return Flag telling whether new DRC data has been found or not.
*/
#define DVB_COMPRESSION_SCALE (8) /* 48,164 dB */

static int32_t aacDecoder_drcReadCompression(HANDLE_FDK_BITSTREAM bs,
                                         CDrcPayload_t *pDrcBs,
                                         uint32_t payloadPosition) {
  int32_t foundDrcData = 0;
  int32_t dmxLevelsPresent, compressionPresent;

  /* Move to the beginning of the DRC payload field */
  FDKpushBiDirectional(bs, (int32_t)FDKgetValidBits(bs) - (int32_t)payloadPosition);

  /* Sanity checks */
  if (FDKgetValidBits(bs) < 24) {
    return 0;
  }

  /* Check sync word */
  if (FDKreadBits(bs, 8) != DVB_ANC_DATA_SYNC_BYTE) {
    return 0;
  }

  /* Evaluate bs_info field */
  if (FDKreadBits(bs, 2) != 3) { /* mpeg_audio_type */
    /* No MPEG-4 audio data */
    return 0;
  }
  FDKreadBits(bs, 2);                    /* dolby_surround_mode */
  pDrcBs->presMode = FDKreadBits(bs, 2); /* presentation_mode */
  FDKreadBits(bs, 1);                    /* stereo_downmix_mode */
  if (FDKreadBits(bs, 1) != 0) {         /* reserved, set to 0 */
    return 0;
  }

  /* Evaluate ancillary_data_status */
  if (FDKreadBits(bs, 3) != 0) { /* reserved, set to 0 */
    return 0;
  }
  dmxLevelsPresent = FDKreadBits(bs, 1); /* downmixing_levels_MPEG4_status */
  /*extensionPresent =*/FDKreadBits(bs,
                                    1); /* ancillary_data_extension_status; */
  compressionPresent =
      FDKreadBits(bs, 1); /* audio_coding_mode_and_compression status */
  /*coarseGrainTcPresent =*/FDKreadBits(bs,
                                        1); /* coarse_grain_timecode_status */
  /*fineGrainTcPresent   =*/FDKreadBits(bs, 1); /* fine_grain_timecode_status */

  if (dmxLevelsPresent) {
    FDKreadBits(bs, 8); /* downmixing_levels_MPEG4 */
  }

  /* audio_coding_mode_and_compression_status */
  if (compressionPresent) {
    uint8_t compressionOn, compressionValue;

    /* audio_coding_mode */
    if (FDKreadBits(bs, 7) != 0) { /* The reserved bits shall be set to "0". */
      return 0;
    }
    compressionOn = (uint8_t)FDKreadBits(bs, 1);    /* compression_on */
    compressionValue = (uint8_t)FDKreadBits(bs, 8); /* Compression_value */

    if (compressionOn) {
      /* A compression value is available so store the data just like MPEG DRC
       * data */
      pDrcBs->channelData.numBands = 1; /* One band ... */
      pDrcBs->channelData.drcValue[0] =
          compressionValue; /* ... with one value ... */
      pDrcBs->channelData.bandTop[0] =
          DRC_BLOCK_LEN_DIV_BAND_MULT -
          1; /* ... comprising the whole spectrum. */
      ;
      pDrcBs->pceInstanceTag = -1; /* Not present */
      pDrcBs->progRefLevel = -1;   /* Not present */
      pDrcBs->channelData.drcDataType =
          DVB_DRC_ANC_DATA; /* Set DRC payload type to DVB. */
      foundDrcData = 1;
    }
  }

  return (foundDrcData);
}

/*
 * Extract DRC payload from bitstream and map it to channels.
 *   Valid return values are:
 *     -1 : An unexpected error occured.
 *      0 : No error and no valid DRC data available.
 *      1 : No error and valid DRC data has been mapped.
 */
static int32_t aacDecoder_drcExtractAndMap(
    HANDLE_AAC_DRC self, HANDLE_FDK_BITSTREAM hBs,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo[],
    uint8_t pceInstanceTag,
    uint8_t channelMapping[], /* Channel mapping translating drcChannel index to
                               canonical channel index */
    int32_t validChannels) {
  CDrcPayload_t threadBs[MAX_DRC_THREADS];
  CDrcPayload_t *validThreadBs[MAX_DRC_THREADS];
  CDrcParams_t *pParams;
  uint32_t backupBsPosition;
  int32_t result = 0;
  int32_t i, thread, validThreads = 0;

  assert(self != NULL);
  assert(hBs != NULL);
  assert(pAacDecoderStaticChannelInfo != NULL);

  pParams = &self->params;

  self->numThreads = 0;
  backupBsPosition = FDKgetValidBits(hBs);

  for (i = 0; i < self->numPayloads && self->numThreads < MAX_DRC_THREADS;
       i++) {
    /* Init payload data chunk. The memclear is very important because it
       initializes the most values. Without it the module wouldn't work properly
       or crash. */
    memset(&threadBs[self->numThreads], 0, sizeof(CDrcPayload_t));
    threadBs[self->numThreads].channelData.bandTop[0] =
        DRC_BLOCK_LEN_DIV_BAND_MULT - 1;

    /* Extract payload */
    self->numThreads += aacDecoder_drcParse(hBs, &threadBs[self->numThreads],
                                            self->drcPayloadPosition[i]);
  }
  self->numPayloads = 0;

  if (self->dvbAncDataAvailable &&
      self->numThreads < MAX_DRC_THREADS) { /* Append a DVB heavy compression
                                               payload thread if available. */

    /* Init payload data chunk. The memclear is very important because it
       initializes the most values. Without it the module wouldn't work properly
       or crash. */
    memset(&threadBs[self->numThreads] , 0, sizeof(CDrcPayload_t));
    threadBs[self->numThreads].channelData.bandTop[0] =
        DRC_BLOCK_LEN_DIV_BAND_MULT - 1;

    /* Extract payload */
    self->numThreads += aacDecoder_drcReadCompression(
        hBs, &threadBs[self->numThreads], self->dvbAncDataPosition);
  }
  self->dvbAncDataAvailable = 0;

  /* Reset the bitbufffer */
  FDKpushBiDirectional(hBs, (int32_t)FDKgetValidBits(hBs) - (int32_t)backupBsPosition);

  /* calculate number of valid bits in excl_chn_mask */

  /* coupling channels not supported */

  /* check for valid threads */
  for (thread = 0; thread < self->numThreads; thread++) {
    CDrcPayload_t *pThreadBs = &threadBs[thread];
    int32_t numExclChns = 0;

    switch ((AACDEC_DRC_PAYLOAD_TYPE)pThreadBs->channelData.drcDataType) {
      default:
        continue;
      case MPEG_DRC_EXT_DATA:
      case DVB_DRC_ANC_DATA:
        break;
    }

    if (pThreadBs->pceInstanceTag >= 0) { /* if PCE tag present */
      if (pThreadBs->pceInstanceTag != pceInstanceTag) {
        continue; /* don't accept */
      }
    }

    /* calculate number of excluded channels */
    if (pThreadBs->excludedChnsMask > 0) {
      int32_t exclMask = pThreadBs->excludedChnsMask;
      int32_t ch;
      for (ch = 0; ch < validChannels; ch++) {
        numExclChns += exclMask & 0x1;
        exclMask >>= 1;
      }
    }
    if (numExclChns < validChannels) {
      validThreadBs[validThreads] = pThreadBs;
      validThreads++;
    }
  }

  /* map DRC bitstream information onto DRC channel information */
  for (thread = 0; thread < validThreads; thread++) {
    CDrcPayload_t *pThreadBs = validThreadBs[thread];
    int32_t exclMask = pThreadBs->excludedChnsMask;
    AACDEC_DRC_PAYLOAD_TYPE drcPayloadType =
        (AACDEC_DRC_PAYLOAD_TYPE)pThreadBs->channelData.drcDataType;
    int32_t ch;

    /* last progRefLevel transmitted is the one that is used
     * (but it should really only be transmitted once per block!)
     */
    if (pThreadBs->progRefLevel >= 0) {
      self->progRefLevel = pThreadBs->progRefLevel;
      self->progRefLevelPresent = 1;
      self->prlExpiryCount = 0; /* Got a new value -> Reset counter */
    }

    if (drcPayloadType == DVB_DRC_ANC_DATA) {
      /* Announce the presentation mode of this valid thread. */
      self->presMode = pThreadBs->presMode;
    }

    /* SCE, CPE and LFE */
    for (ch = 0; ch < validChannels; ch++) {
      AACDEC_DRC_PAYLOAD_TYPE prvPayloadType = UNKNOWN_PAYLOAD;
      int32_t mapedChannel = channelMapping[ch];

      if ((mapedChannel >= validChannels) ||
          ((exclMask & (1 << mapedChannel)) != 0))
        continue;

      if ((pParams->expiryFrame <= 0) ||
          (pAacDecoderStaticChannelInfo[ch]->drcData.expiryCount <
           pParams->expiryFrame)) {
        prvPayloadType =
            (AACDEC_DRC_PAYLOAD_TYPE)pAacDecoderStaticChannelInfo[ch]
                ->drcData.drcDataType;
      }
      if (((drcPayloadType == MPEG_DRC_EXT_DATA) &&
           (prvPayloadType != DVB_DRC_ANC_DATA)) ||
          ((drcPayloadType == DVB_DRC_ANC_DATA) &&
           (pParams->applyHeavyCompression ==
            ON))) { /* copy thread to channel */
        pAacDecoderStaticChannelInfo[ch]->drcData = pThreadBs->channelData;
        result = 1;
      }
    }
    /* CCEs not supported by now */
  }

  /* Increment and check expiry counter for the program reference level: */
  if ((pParams->expiryFrame > 0) &&
      (self->prlExpiryCount++ >
       pParams->expiryFrame)) { /* The program reference level is too old, so
                                   set it back to the target level. */
    self->progRefLevelPresent = 0;
    self->progRefLevel = pParams->targetRefLevel;
    self->prlExpiryCount = 0;
  }

  return result;
}

void aacDecoder_drcApply(HANDLE_AAC_DRC self, void *pSbrDec,
                         CAacDecoderChannelInfo *pAacDecoderChannelInfo,
                         CDrcChannelData_t *pDrcChData, int32_t *extGain,
                         int32_t ch, /* needed only for SBR */
                         int32_t aacFrameSize, int32_t bSbrPresent) {
  int32_t band, bin, numBands;
  int32_t bottom = 0;
  int32_t modifyBins = 0;

  int32_t max_mantissa;
  int32_t max_exponent;

  int32_t norm_mantissa = FL2FXCONST_DBL(0.5f);
  int32_t norm_exponent = 1;

  int32_t fact_mantissa[MAX_DRC_BANDS];
  int32_t fact_exponent[MAX_DRC_BANDS];

  CDrcParams_t *pParams = &self->params;

  int32_t *pSpectralCoefficient =
      SPEC_LONG(pAacDecoderChannelInfo->pSpectralCoefficient);
  CIcsInfo *pIcsInfo = &pAacDecoderChannelInfo->icsInfo;
  int16_t *pSpecScale = pAacDecoderChannelInfo->specScale;

  int32_t winSeq = pIcsInfo->WindowSequence;

  /* Increment and check expiry counter */
  if ((pParams->expiryFrame > 0) &&
      (++pDrcChData->expiryCount >
       pParams->expiryFrame)) { /* The DRC data is too old, so delete it. */
    aacDecoder_drcInitChannelData(pDrcChData);
  }

  if (self->enable != ON) {
    sbrDecoder_drcDisable((HANDLE_SBRDECODER)pSbrDec, ch);
    if (extGain != NULL) {
      int32_t gainScale = (int32_t)*extGain;
      /* The gain scaling must be passed to the function in the buffer pointed
       * on by extGain. */
      if (gainScale >= 0 && gainScale <= DFRACT_BITS) {
        *extGain = scaleValue(norm_mantissa, norm_exponent - gainScale);
      } else {
        assert(0);
      }
    }
    return;
  }

  numBands = pDrcChData->numBands;

  /* If program reference normalization is done in the digital domain,
  modify factor to perform normalization.  prog_ref_level can
  alternatively be passed to the system for modification of the level in
  the analog domain.  Analog level modification avoids problems with
  reduced DAC SNR (if signal is attenuated) or clipping (if signal is
  boosted) */

  if (pParams->targetRefLevel >= 0) {
    /* 0.5^((targetRefLevel - progRefLevel)/24) */
    norm_mantissa =
        fLdPow(FL2FXCONST_DBL(-1.0), /* log2(0.5) */
               0,
               (int32_t)((int32_t)(FL2FXCONST_DBL(1.0f / 24.0) >> 3) *
                          (int32_t)(pParams->targetRefLevel - self->progRefLevel)),
               3, &norm_exponent);
  }
  /* Always export the normalization gain (if possible). */
  if (extGain != NULL) {
    int32_t gainScale = (int32_t)*extGain;
    /* The gain scaling must be passed to the function in the buffer pointed on
     * by extGain. */
    if (gainScale >= 0 && gainScale <= DFRACT_BITS) {
      *extGain = scaleValue(norm_mantissa, norm_exponent - gainScale);
    } else {
      assert(0);
    }
  }
  /* Reset normalization gain since this module must not apply it */
  norm_mantissa = FL2FXCONST_DBL(0.5f);
  norm_exponent = 1;

  /* calc scale factors */
  for (band = 0; band < numBands; band++) {
    uint8_t drcVal = pDrcChData->drcValue[band];

    fact_mantissa[band] = FL2FXCONST_DBL(0.5f);
    fact_exponent[band] = 1;

    if ((pParams->applyHeavyCompression == ON) &&
        ((AACDEC_DRC_PAYLOAD_TYPE)pDrcChData->drcDataType ==
         DVB_DRC_ANC_DATA)) {
      int32_t compressionFactorVal_e;
      int32_t valX, valY;

      valX = drcVal >> 4;
      valY = drcVal & 0x0F;

      /* calculate the unscaled heavy compression factor.
         compressionFactor = 48.164 - 6.0206*valX - 0.4014*valY dB
         range: -48.166 dB to 48.164 dB */
      if (drcVal != 0x7F) {
        fact_mantissa[band] = fPowInt(
            FL2FXCONST_DBL(0.95483867181), /* -0.4014dB = 0.95483867181 */
            0, valY, &compressionFactorVal_e);

        /* -0.0008dB (48.164 - 6.0206*8 = -0.0008) */
        fact_mantissa[band] =
            fMult(FL2FXCONST_DBL(0.99990790084), fact_mantissa[band]);

        fact_exponent[band] =
            DVB_COMPRESSION_SCALE - valX + compressionFactorVal_e;
      }
    } else if ((AACDEC_DRC_PAYLOAD_TYPE)pDrcChData->drcDataType ==
               MPEG_DRC_EXT_DATA) {
      /* apply the scaled dynamic range control words to factor.
       * if scaling drc_cut (or drc_boost), or control word drc_mantissa is 0
       * then there is no dynamic range compression
       *
       * if pDrcChData->drcSgn[band] is
       *  1 then gain is < 1 :  factor = 2^(-self->cut   *
       * pDrcChData->drcMag[band] / 24) 0 then gain is > 1 :  factor = 2^(
       * self->boost * pDrcChData->drcMag[band] / 24)
       */

      if ((drcVal & 0x7F) > 0) {
        int32_t tParamVal = (drcVal & 0x80) ? -pParams->cut : pParams->boost;

        fact_mantissa[band] = f2Pow(
            (int32_t)((int32_t)fMult(FL2FXCONST_DBL(1.0f / 192.0f), tParamVal) *
                       (drcVal & 0x7F)),
            3 + DRC_PARAM_SCALE, &fact_exponent[band]);
      }
    }

    fact_mantissa[band] = fMult(fact_mantissa[band], norm_mantissa);
    fact_exponent[band] += norm_exponent;

  } /* end loop over bands */

  /* normalizations */
  {
    int32_t res;

    max_mantissa = FL2FXCONST_DBL(0.0f);
    max_exponent = 0;
    for (band = 0; band < numBands; band++) {
      max_mantissa = fixMax(max_mantissa, fact_mantissa[band]);
      max_exponent = fixMax(max_exponent, fact_exponent[band]);
    }

    /* left shift factors to gain accurancy */
    res = CntLeadingZeros(max_mantissa) - 1;

    /* above topmost DRC band gain factor is 1 */
    if (((pDrcChData->bandTop[fMax(0, numBands - 1)] + 1) << 2) < aacFrameSize)
      res = 0;

    if (res > 0) {
      res = fixMin(res, max_exponent);
      max_exponent -= res;

      for (band = 0; band < numBands; band++) {
        fact_mantissa[band] <<= res;
        fact_exponent[band] -= res;
      }
    }

    /* normalize magnitudes to one scale factor */
    for (band = 0; band < numBands; band++) {
      if (fact_exponent[band] < max_exponent) {
        fact_mantissa[band] >>= max_exponent - fact_exponent[band];
      }
      if (fact_mantissa[band] != FL2FXCONST_DBL(0.5f)) {
        modifyBins = 1;
      }
    }
    if (max_exponent != 1) {
      modifyBins = 1;
    }
  }

  /*  apply factor to spectral lines
   *  int16_t blocks must take care that bands fall on
   *  block boundaries!
   */
  if (!bSbrPresent) {
    bottom = 0;

    if (!modifyBins) {
      /* We don't have to modify the spectral bins because the fractional part
         of all factors is 0.5. In order to keep accurancy we don't apply the
         factor but decrease the exponent instead. */
      max_exponent -= 1;
    } else {
      for (band = 0; band < numBands; band++) {
        int32_t top = fixMin((int32_t)((pDrcChData->bandTop[band] + 1) << 2),
                         aacFrameSize); /* ... * DRC_BAND_MULT; */

        for (bin = bottom; bin < top; bin++) {
          pSpectralCoefficient[bin] =
              fMult(pSpectralCoefficient[bin], fact_mantissa[band]);
        }

        bottom = top;
      }
    }

    /* above topmost DRC band gain factor is 1 */
    if (max_exponent > 0) {
      for (bin = bottom; bin < aacFrameSize; bin += 1) {
        pSpectralCoefficient[bin] >>= max_exponent;
      }
    }

    /* adjust scaling */
    pSpecScale[0] += max_exponent;

    if (winSeq == BLOCK_SHORT) {
      int32_t win;
      for (win = 1; win < 8; win++) {
        pSpecScale[win] += max_exponent;
      }
    }
  } else {
    HANDLE_SBRDECODER hSbrDecoder = (HANDLE_SBRDECODER)pSbrDec;
    numBands = pDrcChData->numBands;

    /* feed factors into SBR decoder for application in QMF domain. */
    sbrDecoder_drcFeedChannel(hSbrDecoder, ch, numBands, fact_mantissa,
                              max_exponent, pDrcChData->drcInterpolationScheme,
                              winSeq, pDrcChData->bandTop);
  }

  return;
}

/*
 * DRC parameter and presentation mode handling
 */
static void aacDecoder_drcParameterHandling(HANDLE_AAC_DRC self,
                                            int32_t aacNumChannels,
                                            int8_t prevDrcProgRefLevel,
                                            int8_t prevDrcPresMode) {
  int32_t isDownmix, isMonoDownmix, isStereoDownmix;
  int32_t dDmx, dHr;
  AACDEC_DRC_PARAMETER_HANDLING_t drcParameterHandling;
  CDrcParams_t *p;

  assert(self != NULL);

  p = &self->params;

  if (self->progRefLevel != prevDrcProgRefLevel) self->update = 1;

  if (self->presMode != prevDrcPresMode) self->update = 1;

  if (self->prevAacNumChannels != aacNumChannels) self->update = 1;

  /* return if no relevant parameter has changed */
  if (!self->update) {
    return;
  }

  /* derive downmix property. aacNumChannels: number of channels in aac stream,
   * numOutChannels: number of output channels */
  isDownmix = (aacNumChannels > self->numOutChannels);
  isDownmix = (isDownmix && (self->numOutChannels > 0));
  isMonoDownmix = (isDownmix && (self->numOutChannels == 1));
  isStereoDownmix = (isDownmix && (self->numOutChannels == 2));

  if ((self->presMode == 1) || (self->presMode == 2)) {
    drcParameterHandling = (AACDEC_DRC_PARAMETER_HANDLING_t)self->presMode;
  } else { /* no presentation mode -> use parameter handling specified by
              AAC_DRC_DEFAULT_PRESENTATION_MODE */
    drcParameterHandling = p->defaultPresentationMode;
  }

  /* by default, do as desired */
  p->cut = p->usrCut;
  p->boost = p->usrBoost;
  p->applyHeavyCompression = p->usrApplyHeavyCompression;

  switch (drcParameterHandling) {
    case DISABLED_PARAMETER_HANDLING:
    default:
      /* use drc parameters as requested */
      break;

    case ENABLED_PARAMETER_HANDLING:
      /* dDmx: estimated headroom reduction due to downmix, format: -1/4*dB
         dDmx = floor(-4*20*log10(aacNumChannels/numOutChannels)) */
      if (isDownmix) {
        int32_t dmxTmp;
        int32_t e_log, e_mult;
        dmxTmp = fDivNorm(self->numOutChannels,
                          aacNumChannels); /* inverse division ->
                                              negative sign after
                                              logarithm */
        dmxTmp = fLog2(dmxTmp, 0, &e_log);
        dmxTmp = fMultNorm(
            dmxTmp, FL2FXCONST_DBL(4.0f * 20.0f * 0.30103f / (float)(1 << 5)),
            &e_mult); /* e = e_log + e_mult + 5 */
        dDmx = (int32_t)scaleValue(dmxTmp, e_log + e_mult + 5 - (DFRACT_BITS - 1));
      } else {
        dDmx = 0;
      }

      /* dHr: Full estimated (decoder) headroom reduction due to loudness
       * normalisation (DTL - PRL) and downmix. Format: -1/4*dB */
      if (p->targetRefLevel >= 0) { /* if target level is provided */
        dHr = p->targetRefLevel + dDmx - self->progRefLevel;
      } else {
        dHr = dDmx;
      }

      if (dHr < 0) { /* if headroom is reduced */
        /* Use compression, but as little as possible. */
        /* eHr: Headroom provided by encoder, format: -1/4 dB */
        int32_t eHr = fixMin(p->encoderTargetLevel - self->progRefLevel, 0);
        if (eHr <
            dHr) { /* if encoder provides more headroom than decoder needs */
          /* derive scaling of light DRC */
          int32_t calcFactor_norm;
          int32_t calcFactor; /* fraction of DRC gains that is minimally needed for
                             clipping prevention */
          calcFactor_norm =
              fDivNorm(-dHr, -eHr); /* 0.0 < calcFactor_norm < 1.0 */
          calcFactor_norm = calcFactor_norm >> DRC_PARAM_SCALE;
          /* quantize to 128 steps */
          calcFactor = convert_drcParam(
              calcFactor_norm); /* convert to integer value between 0 and 127 */
          calcFactor_norm = (int32_t)(
              (int32_t)(DRC_PARAM_QUANT_STEP >> DRC_PARAM_SCALE) * calcFactor);
          p->cut = (calcFactor_norm > p->cut)
                       ? calcFactor_norm
                       : p->cut; /* use calcFactor_norm as lower limit */
        } else {
          /* encoder provides equal or less headroom than decoder needs */
          /* the time domain limiter must always be active in this case. It is
           * assumed that the framework activates it by default */
          p->cut = DRC_SCALING_MAX;
          if ((dHr - eHr) <=
              -4 * DRC_HEAVY_THRESHOLD_DB) { /* use heavy compression if
                                                headroom deficit is equal or
                                                higher than
                                                DRC_HEAVY_THRESHOLD_DB */
            p->applyHeavyCompression = ON;
          }
        }
      } else { /* dHr >= 0 */
        /* no restrictions required, as headroom is not reduced. */
        /* p->cut = p->usrCut; */
      }
      break;

      /* presentation mode 1 and 2 according to ETSI TS 101 154:
         Digital Video Broadcasting (DVB); Specification for the use of Video
         and Audio Coding in Broadcasting Applications based on the MPEG-2
         Transport Stream, section C.5.4., "Decoding", and Table C.33. Also
         according to amendment 4 to ISO/IEC 14496-3, section 4.5.2.14.2.4, and
         Table AMD4.11. ISO DRC            -> applyHeavyCompression = OFF (Use
         light compression, MPEG-style) Compression_value  ->
         applyHeavyCompression = ON  (Use heavy compression, DVB-style) scaling
         restricted -> p->cut = DRC_SCALING_MAX */

    case DRC_PRESENTATION_MODE_1: /* presentation mode 1, Light:-31/Heavy:-23 */
      if ((p->targetRefLevel >= 0) &&
          (p->targetRefLevel <
           124)) { /* if target level is provided and > -31 dB */
        /* playback up to -23 dB */
        p->applyHeavyCompression = ON;
      } else { /* target level <= -31 dB or not provided */
        /* playback -31 dB */
        if (isMonoDownmix || isStereoDownmix) { /* stereo or mono downmixing */
          p->cut = DRC_SCALING_MAX;
        }
      }
      break;

    case DRC_PRESENTATION_MODE_2: /* presentation mode 2, Light:-23/Heavy:-23 */
      if ((p->targetRefLevel >= 0) &&
          (p->targetRefLevel <
           124)) { /* if target level is provided and > -31 dB */
        /* playback up to -23 dB */
        if (isMonoDownmix) { /* if mono downmix */
          p->applyHeavyCompression = ON;
        } else {
          p->applyHeavyCompression = OFF;
          p->cut = DRC_SCALING_MAX;
        }
      } else { /* target level <= -31 dB or not provided */
        /* playback -31 dB */
        p->applyHeavyCompression = OFF;
        if (isMonoDownmix || isStereoDownmix) { /* stereo or mono downmixing */
          p->cut = DRC_SCALING_MAX;
        }
      }
      break;
  } /*  switch (drcParameterHandling) */

  /* With heavy compression, there is no scaling.
     Scaling factors are set for notification only. */
  if (p->applyHeavyCompression == ON) {
    p->boost = DRC_SCALING_MAX;
    p->cut = DRC_SCALING_MAX;
  }

  /* switch on/off processing */
  self->enable = ((p->boost > (int32_t)0) || (p->cut > (int32_t)0) ||
                  (p->applyHeavyCompression == ON) || (p->targetRefLevel >= 0));
  self->enable = (self->enable && !self->uniDrcPrecedence);

  self->prevAacNumChannels = aacNumChannels;
  self->update = 0;
}

/*
 * Prepare DRC processing
 *   Valid return values are:
 *     -1 : An unexpected error occured.
 *      0 : No error and no valid DRC data available.
 *      1 : No error and valid DRC data has been mapped.
 */
int32_t aacDecoder_drcProlog(
    HANDLE_AAC_DRC self, HANDLE_FDK_BITSTREAM hBs,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo[],
    uint8_t pceInstanceTag,
    uint8_t channelMapping[], /* Channel mapping translating drcChannel index to
                               canonical channel index */
    int32_t validChannels) {
  int32_t result = 0;

  if (self == NULL) {
    return -1;
  }

  if (!self->params.bsDelayEnable) {
    /* keep previous progRefLevel and presMode for update flag in
     * drcParameterHandling */
    int32_t prevPRL, prevPM = 0;
    prevPRL = self->progRefLevel;
    prevPM = self->presMode;

    result = aacDecoder_drcExtractAndMap(
        self, hBs, pAacDecoderStaticChannelInfo, pceInstanceTag, channelMapping,
        validChannels);

    if (result < 0) {
      return result;
    }

    /* Drc parameter handling */
    aacDecoder_drcParameterHandling(self, validChannels, prevPRL, prevPM);
  }

  return result;
}

/*
 * Finalize DRC processing
 *   Valid return values are:
 *     -1 : An unexpected error occured.
 *      0 : No error and no valid DRC data available.
 *      1 : No error and valid DRC data has been mapped.
 */
int32_t aacDecoder_drcEpilog(
    HANDLE_AAC_DRC self, HANDLE_FDK_BITSTREAM hBs,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo[],
    uint8_t pceInstanceTag,
    uint8_t channelMapping[], /* Channel mapping translating drcChannel index to
                               canonical channel index */
    int32_t validChannels) {
  int32_t result = 0;

  if (self == NULL) {
    return -1;
  }

  if (self->params.bsDelayEnable) {
    /* keep previous progRefLevel and presMode for update flag in
     * drcParameterHandling */
    int32_t prevPRL, prevPM = 0;
    prevPRL = self->progRefLevel;
    prevPM = self->presMode;

    result = aacDecoder_drcExtractAndMap(
        self, hBs, pAacDecoderStaticChannelInfo, pceInstanceTag, channelMapping,
        validChannels);

    if (result < 0) {
      return result;
    }

    /* Drc parameter handling */
    aacDecoder_drcParameterHandling(self, validChannels, prevPRL, prevPM);
  }

  return result;
}

/*
 * Export relevant metadata info from bitstream payload.
 */
void aacDecoder_drcGetInfo(HANDLE_AAC_DRC self, int8_t *pPresMode,
                           int8_t *pProgRefLevel) {
  if (self != NULL) {
    if (pPresMode != NULL) {
      *pPresMode = self->presMode;
    }
    if (pProgRefLevel != NULL) {
      if (self->progRefLevelPresent) {
        *pProgRefLevel = self->progRefLevel;
      } else {
        *pProgRefLevel = -1;
      }
    }
  }
}

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
                               const uint32_t stride, const uint32_t limiterEnabled) {
  uint32_t i;
  int32_t additionalGain_scaling;
  int32_t additionalGain;

  assert(gain_delay <= nSamples);

  int32_t additionalGainSmoothState = hDrcInfo->additionalGainFilterState;
  int32_t additionalGainSmoothState1 = hDrcInfo->additionalGainFilterState1;

  if (!gain_delay) {
    additionalGain = pGain[0];

    /* Apply the additional scaling gain_scale[0] that has no delay and no
     * smoothing */
    additionalGain_scaling =
        fMin(gain_scale, CntLeadingZeros(additionalGain) - 1);
    additionalGain = scaleValue(additionalGain, additionalGain_scaling);

    /* if it's not possible to fully apply gain_scale to additionalGain, apply
     * it to the input signal */
    additionalGain_scaling -= gain_scale;

    if (additionalGain_scaling) {
      scaleValuesSaturate(samplesIn, channels * nSamples,
                          -additionalGain_scaling);
    }

    if (limiterEnabled) {
      assert(pGainPerSample != NULL);

      for (i = 0; i < nSamples; i++) {
        pGainPerSample[i] = additionalGain;
      }
    } else {
      for (i = 0; i < channels * nSamples; i++) {
        samplesIn[i] = FIXP_DBL2PCM_DEC(fMult(samplesIn[i], additionalGain));
      }
    }
  } else {
    uint32_t inc;
    int32_t additionalGainUnfiltered;

    inc = (stride == 1) ? channels : 1;

    for (i = 0; i < nSamples; i++) {
      if (i < gain_delay) {
        additionalGainUnfiltered = hDrcInfo->additionalGainPrev;
      } else {
        additionalGainUnfiltered = pGain[0];
      }

      /* Smooth additionalGain */

      /* [b,a] = butter(1, 0.01) */
      static const int16_t b[] = {  507,
                                     507};
      static const int16_t a[] = {(int16_t)MAXVAL_SGL,
                                     -31754};

      additionalGain = -fMult(additionalGainSmoothState, a[1]) +
                       fMultDiv2(additionalGainUnfiltered, b[0]) +
                       fMultDiv2(additionalGainSmoothState1, b[1]);
      additionalGainSmoothState1 = additionalGainUnfiltered;
      additionalGainSmoothState = additionalGain;

      /* Apply the additional scaling gain_scale[0] that has no delay and no
       * smoothing */
      additionalGain_scaling =
          fMin(gain_scale, CntLeadingZeros(additionalGain) - 1);
      additionalGain = scaleValue(additionalGain, additionalGain_scaling);

      /* if it's not possible to fully apply gain_scale[0] to additionalGain,
       * apply it to the input signal */
      additionalGain_scaling -= gain_scale;

      if (limiterEnabled) {
        assert(stride == 1);
        assert(pGainPerSample != NULL);

        if (additionalGain_scaling) {
          scaleValuesSaturate(samplesIn, channels, -additionalGain_scaling);
        }

        pGainPerSample[i] = additionalGain;
      } else {
        if (additionalGain_scaling) {
          for (uint32_t k = 0; k < channels; k++) {
            scaleValuesSaturate(&samplesIn[k * stride], 1,
                                -additionalGain_scaling);
          }
        }

        for (uint32_t k = 0; k < channels; k++) {
          samplesIn[k * stride] =
              FIXP_DBL2PCM_DEC(fMult(samplesIn[k * stride], additionalGain));
        }
      }

      samplesIn += inc;
    }
  }

  hDrcInfo->additionalGainPrev = pGain[0];
  hDrcInfo->additionalGainFilterState = additionalGainSmoothState;
  hDrcInfo->additionalGainFilterState1 = additionalGainSmoothState1;

  return (AACDEC_DRC_GAIN_SCALING);
}
