

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: SAC Dec bitstream decoder

*******************************************************************************/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "sac_bitdec.h"

#include "sac_dec_errorcodes.h"
#include "../libFDK/nlc_dec.h"
#include "sac_rom.h"
#include "../libFDK/FDK_matrixCalloc.h"
#include "sac_tsd.h"

enum {
  ottVsTotInactiv = 0,
  ottVsTotDb1Activ = 1,
  ottVsTotDb2Activ = 2,
  ottVsTotDb1Db2Activ = 3
};

static SACDEC_ERROR SpatialDecDecodeHelperInfo(
    SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig, UPMIXTYPE upmixType) {
  int32_t i;
  uint32_t syntaxFlags;

  /* Determine bit stream syntax */
  syntaxFlags = 0;
  switch (pSpatialSpecificConfig->coreCodec) {
    case AOT_ER_AAC_ELD:
    case AOT_ER_AAC_LD:
      syntaxFlags |= SACDEC_SYNTAX_LD;
      break;
    case AOT_USAC:
      syntaxFlags |= SACDEC_SYNTAX_USAC;
      break;
    case AOT_NONE:
    default:
      return MPS_UNSUPPORTED_FORMAT;
  }

  pSpatialSpecificConfig->syntaxFlags = syntaxFlags;

  switch (pSpatialSpecificConfig->treeConfig) {
    case TREE_212: {
      pSpatialSpecificConfig->ottCLDdefault[0] = 0;
    } break;
    default:
      return MPS_INVALID_TREECONFIG;
  }

  if (syntaxFlags & SACDEC_SYNTAX_USAC) {
    if (pSpatialSpecificConfig->bsOttBandsPhasePresent) {
      pSpatialSpecificConfig->numOttBandsIPD =
          pSpatialSpecificConfig->bsOttBandsPhase;
    } else {
      int32_t numParameterBands;

      numParameterBands = pSpatialSpecificConfig->freqRes;
      switch (numParameterBands) {
        case 4:
        case 5:
          pSpatialSpecificConfig->numOttBandsIPD = 2;
          break;
        case 7:
          pSpatialSpecificConfig->numOttBandsIPD = 3;
          break;
        case 10:
          pSpatialSpecificConfig->numOttBandsIPD = 5;
          break;
        case 14:
          pSpatialSpecificConfig->numOttBandsIPD = 7;
          break;
        case 20:
        case 28:
          pSpatialSpecificConfig->numOttBandsIPD = 10;
          break;
        default:
          return MPS_INVALID_PARAMETERBANDS;
      }
    }
  } else {
    pSpatialSpecificConfig->numOttBandsIPD = 0;
  }
  for (i = 0; i < pSpatialSpecificConfig->nOttBoxes; i++) {
    {
      pSpatialSpecificConfig->bitstreamOttBands[i] =
          pSpatialSpecificConfig->freqRes;
    }
    {
      pSpatialSpecificConfig->numOttBands[i] =
          pSpatialSpecificConfig->bitstreamOttBands[i];
      if (syntaxFlags & SACDEC_SYNTAX_USAC &&
          !pSpatialSpecificConfig->bsOttBandsPhasePresent) {
        if (pSpatialSpecificConfig->bResidualCoding &&
            pSpatialSpecificConfig->ResidualConfig[i].bResidualPresent &&
            (pSpatialSpecificConfig->numOttBandsIPD <
             pSpatialSpecificConfig->ResidualConfig[i].nResidualBands)) {
          pSpatialSpecificConfig->numOttBandsIPD =
              pSpatialSpecificConfig->ResidualConfig[i].nResidualBands;
        }
      }
    }
  } /* i */

  return MPS_OK;
}

/*******************************************************************************
 Functionname: SpatialDecParseExtensionConfig
 *******************************************************************************

 Description:

 Arguments:

 Return:

*******************************************************************************/

static SACDEC_ERROR SpatialDecParseExtensionConfig(
    HANDLE_FDK_BITSTREAM bitstream, SPATIAL_SPECIFIC_CONFIG *config,
    int32_t numOttBoxes, int32_t numTttBoxes, int32_t numOutChan, int32_t bitsAvailable) {
  SACDEC_ERROR err = MPS_OK;
  int32_t ba = bitsAvailable;

  config->sacExtCnt = 0;
  config->bResidualCoding = 0;

  ba = fMin((int32_t)FDKgetValidBits(bitstream), ba);

  while ((ba >= 8) && (config->sacExtCnt < MAX_NUM_EXT_TYPES)) {
    int32_t bitsRead, nFillBits;
    int32_t tmp;
    uint32_t sacExtLen;

    config->sacExtType[config->sacExtCnt] = FDKreadBits(bitstream, 4);
    ba -= 4;

    sacExtLen = FDKreadBits(bitstream, 4);
    ba -= 4;

    if (sacExtLen == 15) {
      sacExtLen += FDKreadBits(bitstream, 8);
      ba -= 8;
      if (sacExtLen == 15 + 255) {
        sacExtLen += FDKreadBits(bitstream, 16);
        ba -= 16;
      }
    }

    tmp = (int32_t)FDKgetValidBits(
        bitstream); /* Extension config payload start anchor. */
    if ((tmp <= 0) || (tmp < (int32_t)sacExtLen * 8) || (ba < (int32_t)sacExtLen * 8)) {
      err = MPS_PARSE_ERROR;
      goto bail;
    }

    switch (config->sacExtType[config->sacExtCnt]) {
      default:; /* unknown extension data => do nothing */
    }

    /* skip remaining extension data */
    bitsRead = tmp - FDKgetValidBits(bitstream);
    nFillBits = 8 * sacExtLen - bitsRead;

    if (nFillBits < 0) {
      err = MPS_PARSE_ERROR;
      goto bail;
    } else {
      /* Skip fill bits or an unkown extension. */
      FDKpushFor(bitstream, nFillBits);
    }

    ba -= 8 * sacExtLen;
    config->sacExtCnt++;
  }

bail:
  return err;
}

SACDEC_ERROR SpatialDecParseSpecificConfigHeader(
    HANDLE_FDK_BITSTREAM bitstream,
    SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig,
    AUDIO_OBJECT_TYPE_t coreCodec, SPATIAL_DEC_UPMIX_TYPE upmixType) {
  SACDEC_ERROR err = MPS_OK;
  int32_t numFillBits;
  int32_t sacHeaderLen = 0;
  int32_t sacTimeAlignFlag = 0;

  sacTimeAlignFlag = FDKreadBits(bitstream, 1);
  sacHeaderLen = FDKreadBits(bitstream, 7);

  if (sacHeaderLen == 127) {
    sacHeaderLen += FDKreadBits(bitstream, 16);
  }
  numFillBits = (int32_t)FDKgetValidBits(bitstream);

  err = SpatialDecParseSpecificConfig(bitstream, pSpatialSpecificConfig,
                                      sacHeaderLen, coreCodec);

  numFillBits -=
      (int32_t)FDKgetValidBits(bitstream); /* the number of read bits (tmpBits) */
  numFillBits = (8 * sacHeaderLen) - numFillBits;
  if (numFillBits < 0) {
    /* Parsing went wrong */
    err = MPS_PARSE_ERROR;
  }
  /* Move to the very end of the SSC */
  FDKpushBiDirectional(bitstream, numFillBits);

  if ((err == MPS_OK) && sacTimeAlignFlag) {
    /* not supported */
    FDKreadBits(bitstream, 16);
    err = MPS_UNSUPPORTED_CONFIG;
  }

  /* Derive additional helper variables */
  SpatialDecDecodeHelperInfo(pSpatialSpecificConfig, (UPMIXTYPE)upmixType);

  return err;
}

SACDEC_ERROR SpatialDecParseMps212Config(
    HANDLE_FDK_BITSTREAM bitstream,
    SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig, int32_t samplingRate,
    AUDIO_OBJECT_TYPE_t coreCodec, int32_t stereoConfigIndex,
    int32_t coreSbrFrameLengthIndex) {
  int32_t i;

  memset(pSpatialSpecificConfig, 0, sizeof(SPATIAL_SPECIFIC_CONFIG));

  pSpatialSpecificConfig->stereoConfigIndex = stereoConfigIndex;
  pSpatialSpecificConfig->coreSbrFrameLengthIndex = coreSbrFrameLengthIndex;
  pSpatialSpecificConfig->freqRes =
      (SPATIALDEC_FREQ_RES)freqResTable[FDKreadBits(bitstream, 3)];
  if (pSpatialSpecificConfig->freqRes == 0) {
    return MPS_PARSE_ERROR; /* reserved value */
  }

  switch (coreCodec) {
    case AOT_DRM_USAC:
      pSpatialSpecificConfig->bsFixedGainDMX =
          (SPATIALDEC_FIXED_GAINS)FDKreadBits(bitstream, 3);
      /* tempShapeConfig = (bsTempShapeConfigDrm == 1) ? 3 : 0 */
      pSpatialSpecificConfig->tempShapeConfig =
          (SPATIALDEC_TS_CONF)(FDKreadBits(bitstream, 1) * 3);
      pSpatialSpecificConfig->decorrConfig = (SPATIALDEC_DECORR_CONF)0;
      pSpatialSpecificConfig->bsDecorrType = 0;
      break;
    case AOT_USAC:
      pSpatialSpecificConfig->bsFixedGainDMX =
          (SPATIALDEC_FIXED_GAINS)FDKreadBits(bitstream, 3);
      pSpatialSpecificConfig->tempShapeConfig =
          (SPATIALDEC_TS_CONF)FDKreadBits(bitstream, 2);
      pSpatialSpecificConfig->decorrConfig =
          (SPATIALDEC_DECORR_CONF)FDKreadBits(bitstream, 2);
      if (pSpatialSpecificConfig->decorrConfig > 2) {
        return MPS_PARSE_ERROR; /* reserved value */
      }
      pSpatialSpecificConfig->bsDecorrType = 0;
      break;
    default:
      return MPS_UNSUPPORTED_FORMAT;
  }
  pSpatialSpecificConfig->nTimeSlots = (coreSbrFrameLengthIndex == 4) ? 64 : 32;
  pSpatialSpecificConfig->bsHighRateMode = (uint8_t)FDKreadBits(bitstream, 1);

  {
    pSpatialSpecificConfig->bsPhaseCoding = (uint8_t)FDKreadBits(bitstream, 1);
    pSpatialSpecificConfig->bsOttBandsPhasePresent =
        (uint8_t)FDKreadBits(bitstream, 1);
    if (pSpatialSpecificConfig->bsOttBandsPhasePresent) {
      if (MAX_PARAMETER_BANDS < (pSpatialSpecificConfig->bsOttBandsPhase =
                                     FDKreadBits(bitstream, 5))) {
        return MPS_PARSE_ERROR;
      }
    } else {
      pSpatialSpecificConfig->bsOttBandsPhase = 0;
    }
  }

  if (stereoConfigIndex > 1) { /* do residual coding */
    pSpatialSpecificConfig->bResidualCoding = 1;
    pSpatialSpecificConfig->ResidualConfig->bResidualPresent = 1;
    if (pSpatialSpecificConfig->freqRes <
        (pSpatialSpecificConfig->ResidualConfig->nResidualBands =
             FDKreadBits(bitstream, 5))) {
      return MPS_PARSE_ERROR;
    }
    pSpatialSpecificConfig->bsOttBandsPhase =
        fMax(pSpatialSpecificConfig->bsOttBandsPhase,
             pSpatialSpecificConfig->ResidualConfig->nResidualBands);
    pSpatialSpecificConfig->bsPseudoLr = (uint8_t)FDKreadBits(bitstream, 1);

    if (pSpatialSpecificConfig->bsPhaseCoding) {
      pSpatialSpecificConfig->bsPhaseCoding = 3;
    }
  } else {
    pSpatialSpecificConfig->bResidualCoding = 0;
    pSpatialSpecificConfig->ResidualConfig->bResidualPresent = 0;
  }

  if (pSpatialSpecificConfig->tempShapeConfig == 2) {
    switch (coreCodec) {
      case AOT_USAC:
        pSpatialSpecificConfig->envQuantMode = FDKreadBits(bitstream, 1);
        break;
      default: /* added to avoid compiler warning */
        break; /* added to avoid compiler warning */
    }
  }

  /* Static parameters */

  pSpatialSpecificConfig->samplingFreq =
      samplingRate; /* wrong for stereoConfigIndex == 3 but value is unused */
  pSpatialSpecificConfig->treeConfig = SPATIALDEC_MODE_RSVD7;
  pSpatialSpecificConfig->nOttBoxes =
      treePropertyTable[pSpatialSpecificConfig->treeConfig].numOttBoxes;
  pSpatialSpecificConfig->nInputChannels =
      treePropertyTable[pSpatialSpecificConfig->treeConfig].numInputChannels;
  pSpatialSpecificConfig->nOutputChannels =
      treePropertyTable[pSpatialSpecificConfig->treeConfig].numOutputChannels;

  pSpatialSpecificConfig->bArbitraryDownmix = 0;

  for (i = 0; i < pSpatialSpecificConfig->nOttBoxes; i++) {
    pSpatialSpecificConfig->OttConfig[i].nOttBands = 0;
  }

  if (coreCodec == AOT_DRM_USAC) {
    /* MPS payload is MPEG conform -> no need for pseudo DRM AOT */
    coreCodec = AOT_USAC;
  }
  pSpatialSpecificConfig->coreCodec = coreCodec;

  /* Derive additional helper variables */
  SpatialDecDecodeHelperInfo(pSpatialSpecificConfig, UPMIXTYPE_NORMAL);

  return MPS_OK;
}

SACDEC_ERROR SpatialDecParseSpecificConfig(
    HANDLE_FDK_BITSTREAM bitstream,
    SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig, int32_t sacHeaderLen,
    AUDIO_OBJECT_TYPE_t coreCodec) {
  SACDEC_ERROR err = MPS_OK;
  int32_t i;
  int32_t bsSamplingFreqIndex;
  int32_t bsFreqRes, b3DaudioMode = 0;
  int32_t numHeaderBits;
  int32_t cfgStartPos, bitsAvailable;
  int32_t treeConfig;

  memset(pSpatialSpecificConfig, 0, sizeof(SPATIAL_SPECIFIC_CONFIG));

  cfgStartPos = FDKgetValidBits(bitstream);
  /* It might be that we do not know the SSC length beforehand. */
  if (sacHeaderLen == 0) {
    bitsAvailable = cfgStartPos;
  } else {
    bitsAvailable = 8 * sacHeaderLen;
    if (bitsAvailable > cfgStartPos) {
      err = MPS_PARSE_ERROR;
      goto bail;
    }
  }

  bsSamplingFreqIndex = FDKreadBits(bitstream, 4);

  if (bsSamplingFreqIndex == 15) {
    pSpatialSpecificConfig->samplingFreq = FDKreadBits(bitstream, 24);
  } else {
    pSpatialSpecificConfig->samplingFreq =
        samplingFreqTable[bsSamplingFreqIndex];
    if (pSpatialSpecificConfig->samplingFreq == 0) {
      err = MPS_PARSE_ERROR;
      goto bail;
    }
  }

  pSpatialSpecificConfig->nTimeSlots = FDKreadBits(bitstream, 5) + 1;
  if ((pSpatialSpecificConfig->nTimeSlots < 1) ||
      (pSpatialSpecificConfig->nTimeSlots > MAX_TIME_SLOTS)) {
    err = MPS_PARSE_ERROR;
    goto bail;
  }

  bsFreqRes = FDKreadBits(bitstream, 3);

  pSpatialSpecificConfig->freqRes =
      (SPATIALDEC_FREQ_RES)freqResTable_LD[bsFreqRes];

  treeConfig = FDKreadBits(bitstream, 4);

  if (treeConfig != SPATIALDEC_MODE_RSVD7) {
    err = MPS_UNSUPPORTED_CONFIG;
    goto bail;
  }
  pSpatialSpecificConfig->treeConfig = (SPATIALDEC_TREE_CONFIG) treeConfig;

  {
    pSpatialSpecificConfig->nOttBoxes =
        treePropertyTable[pSpatialSpecificConfig->treeConfig].numOttBoxes;
    pSpatialSpecificConfig->nTttBoxes =
        treePropertyTable[pSpatialSpecificConfig->treeConfig].numTttBoxes;
    pSpatialSpecificConfig->nInputChannels =
        treePropertyTable[pSpatialSpecificConfig->treeConfig].numInputChannels;
    pSpatialSpecificConfig->nOutputChannels =
        treePropertyTable[pSpatialSpecificConfig->treeConfig].numOutputChannels;
  }

  pSpatialSpecificConfig->quantMode =
      (SPATIALDEC_QUANT_MODE)FDKreadBits(bitstream, 2);

  pSpatialSpecificConfig->bArbitraryDownmix = FDKreadBits(bitstream, 1);

  pSpatialSpecificConfig->bsFixedGainDMX =
      (SPATIALDEC_FIXED_GAINS)FDKreadBits(bitstream, 3);

  pSpatialSpecificConfig->tempShapeConfig =
      (SPATIALDEC_TS_CONF)FDKreadBits(bitstream, 2);
  if (pSpatialSpecificConfig->tempShapeConfig > 2) {
    return MPS_PARSE_ERROR; /* reserved value */
  }

  pSpatialSpecificConfig->decorrConfig =
      (SPATIALDEC_DECORR_CONF)FDKreadBits(bitstream, 2);
  if (pSpatialSpecificConfig->decorrConfig > 2) {
    return MPS_PARSE_ERROR; /* reserved value */
  }

  for (i = 0; i < pSpatialSpecificConfig->nOttBoxes; i++) {
    pSpatialSpecificConfig->OttConfig[i].nOttBands = 0;
  }

  for (i = 0; i < pSpatialSpecificConfig->nTttBoxes; i++) {
    int32_t bTttDualMode = FDKreadBits(bitstream, 1);
    FDKreadBits(bitstream, 3); /* not supported */

    if (bTttDualMode) {
      FDKreadBits(bitstream, 8); /* not supported */
    }
  }

  if (pSpatialSpecificConfig->tempShapeConfig == 2) {
    pSpatialSpecificConfig->envQuantMode = FDKreadBits(bitstream, 1);
  }

  if (b3DaudioMode) {
    if (FDKreadBits(bitstream, 2) == 0) { /* b3DaudioHRTFset ? */
      int32_t hc;
      int32_t HRTFnumBand;
      int32_t HRTFfreqRes = FDKreadBits(bitstream, 3);
      int32_t HRTFnumChan = FDKreadBits(bitstream, 4);
      int32_t HRTFasymmetric = FDKreadBits(bitstream, 1);

      HRTFnumBand = freqResTable_LD[HRTFfreqRes];

      for (hc = 0; hc < HRTFnumChan; hc++) {
        FDKpushFor(bitstream, HRTFnumBand * 6); /* HRTFlevelLeft[hc][hb] */
        if (HRTFasymmetric) {
          FDKpushFor(bitstream, HRTFnumBand * 6); /* HRTFlevelRight[hc][hb] */
        }
        if (FDKreadBits(bitstream, 1)) {          /* HRTFphase[hc] ? */
          FDKpushFor(bitstream, HRTFnumBand * 6); /* HRTFphaseLR[hc][hb] */
        }
        if (FDKreadBits(bitstream, 1)) {          /* HRTFicc[hc] ? */
          FDKpushFor(bitstream, HRTFnumBand * 6); /* HRTFiccLR[hc][hb] */
        }
      }
    }
  }

  FDKbyteAlign(bitstream,
               cfgStartPos); /* ISO/IEC FDIS 23003-1: 5.2. ... byte alignment
                                with respect to the beginning of the syntactic
                                element in which ByteAlign() occurs. */

  numHeaderBits = cfgStartPos - (int32_t)FDKgetValidBits(bitstream);
  bitsAvailable -= numHeaderBits;
  if (bitsAvailable < 0) {
    err = MPS_PARSE_ERROR;
    goto bail;
  }

  pSpatialSpecificConfig->sacExtCnt = 0;
  pSpatialSpecificConfig->bResidualCoding = 0;

  err = SpatialDecParseExtensionConfig(
      bitstream, pSpatialSpecificConfig, pSpatialSpecificConfig->nOttBoxes,
      pSpatialSpecificConfig->nTttBoxes,
      pSpatialSpecificConfig->nOutputChannels, bitsAvailable);

  FDKbyteAlign(
      bitstream,
      cfgStartPos); /* Same alignment anchor as above because
                       SpatialExtensionConfig() always reads full bytes */

  pSpatialSpecificConfig->coreCodec = coreCodec;

  SpatialDecDecodeHelperInfo(pSpatialSpecificConfig, UPMIXTYPE_NORMAL);

bail:
  if (sacHeaderLen > 0) {
    /* If the config is of known length then assure that the
       bitbuffer is exactly at its end when leaving the function. */
    FDKpushBiDirectional(
        bitstream,
        (sacHeaderLen * 8) - (cfgStartPos - (int32_t)FDKgetValidBits(bitstream)));
  }

  return err;
}

int32_t SpatialDecDefaultSpecificConfig(
    SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig,
    AUDIO_OBJECT_TYPE_t coreCodec, int32_t samplingFreq, int32_t nTimeSlots,
    int32_t sacDecoderLevel, int32_t isBlind, int32_t numCoreChannels)

{
  int32_t err = MPS_OK;
  int32_t i;

  assert(coreCodec != AOT_NONE);
  assert(nTimeSlots > 0);
  assert(samplingFreq > 0);

  pSpatialSpecificConfig->coreCodec = coreCodec;
  pSpatialSpecificConfig->samplingFreq = samplingFreq;
  pSpatialSpecificConfig->nTimeSlots = nTimeSlots;
  if ((pSpatialSpecificConfig->coreCodec == AOT_ER_AAC_ELD) ||
      (pSpatialSpecificConfig->coreCodec == AOT_ER_AAC_LD))
    pSpatialSpecificConfig->freqRes = SPATIALDEC_FREQ_RES_23;
  else
    pSpatialSpecificConfig->freqRes = SPATIALDEC_FREQ_RES_28;

  {
    pSpatialSpecificConfig->treeConfig =
        SPATIALDEC_MODE_RSVD7; /* 212  configuration */
  }

  {
    pSpatialSpecificConfig->nOttBoxes =
        treePropertyTable[pSpatialSpecificConfig->treeConfig].numOttBoxes;
    pSpatialSpecificConfig->nInputChannels =
        treePropertyTable[pSpatialSpecificConfig->treeConfig].numInputChannels;
    pSpatialSpecificConfig->nOutputChannels =
        treePropertyTable[pSpatialSpecificConfig->treeConfig].numOutputChannels;
  }

  pSpatialSpecificConfig->quantMode = SPATIALDEC_QUANT_FINE_DEF;
  pSpatialSpecificConfig->bArbitraryDownmix = 0;
  pSpatialSpecificConfig->bResidualCoding = 0;
  if ((pSpatialSpecificConfig->coreCodec == AOT_ER_AAC_ELD) ||
      (pSpatialSpecificConfig->coreCodec == AOT_ER_AAC_LD))
    pSpatialSpecificConfig->bsFixedGainDMX = SPATIALDEC_GAIN_RSVD2;
  else
    pSpatialSpecificConfig->bsFixedGainDMX = SPATIALDEC_GAIN_MODE0;

  pSpatialSpecificConfig->tempShapeConfig = SPATIALDEC_TS_TPNOWHITE;
  pSpatialSpecificConfig->decorrConfig = SPATIALDEC_DECORR_MODE0;

  for (i = 0; i < pSpatialSpecificConfig->nOttBoxes; i++) {
    pSpatialSpecificConfig->OttConfig[i].nOttBands = 0;
  }

  return err;
}

/*******************************************************************************
 Functionname: coarse2fine
 *******************************************************************************

 Description:
   Parameter index mapping from coarse to fine quantization

 Arguments:

Input:

Output:

*******************************************************************************/
static void coarse2fine(int8_t *data, DATA_TYPE dataType, int32_t startBand,
                        int32_t numBands) {
  int32_t i;

  for (i = startBand; i < startBand + numBands; i++) {
    data[i] <<= 1;
  }

  if (dataType == t_CLD) {
    for (i = startBand; i < startBand + numBands; i++) {
      if (data[i] == -14)
        data[i] = -15;
      else if (data[i] == 14)
        data[i] = 15;
    }
  }
}

/*******************************************************************************
 Functionname: fine2coarse
 *******************************************************************************

 Description:
   Parameter index mapping from fine to coarse quantization

 Arguments:

Input:

Output:

*******************************************************************************/
static void fine2coarse(int8_t *data, DATA_TYPE dataType, int32_t startBand,
                        int32_t numBands) {
  int32_t i;

  for (i = startBand; i < startBand + numBands; i++) {
    /* Note: the if cases below actually make a difference (negative values) */
    if (dataType == t_CLD)
      data[i] /= 2;
    else
      data[i] >>= 1;
  }
}

/*******************************************************************************
 Functionname: getStrideMap
 *******************************************************************************

 Description:
   Index Mapping accroding to pbStrides

 Arguments:

Input:

Output:

*******************************************************************************/
static int32_t getStrideMap(int32_t freqResStride, int32_t startBand, int32_t stopBand,
                        int32_t *aStrides) {
  int32_t i, pb, pbStride, dataBands, strOffset;

  pbStride = pbStrideTable[freqResStride];
  dataBands = (stopBand - startBand - 1) / pbStride + 1;

  aStrides[0] = startBand;
  for (pb = 1; pb <= dataBands; pb++) {
    aStrides[pb] = aStrides[pb - 1] + pbStride;
  }
  strOffset = 0;
  while (aStrides[dataBands] > stopBand) {
    if (strOffset < dataBands) strOffset++;
    for (i = strOffset; i <= dataBands; i++) {
      aStrides[i]--;
    }
  }

  return dataBands;
}

/*******************************************************************************
 Functionname: ecDataDec
 *******************************************************************************

 Description:
   Do delta decoding and dequantization

 Arguments:

Input:

Output:


*******************************************************************************/

static SACDEC_ERROR ecDataDec(
    const SPATIAL_BS_FRAME *frame, uint32_t syntaxFlags,
    HANDLE_FDK_BITSTREAM bitstream, LOSSLESSDATA *const llData,
    int8_t (*data)[MAX_PARAMETER_SETS][MAX_PARAMETER_BANDS], int8_t **lastdata,
    int32_t datatype, int32_t boxIdx, int32_t startBand, int32_t stopBand, int8_t defaultValue) {
  SACDEC_ERROR err = MPS_OK;
  int32_t i, j, pb, dataSets, setIdx, bsDataPair, dataBands, oldQuantCoarseXXX;
  int32_t aStrides[MAX_PARAMETER_BANDS + 1] = {0};

  dataSets = 0;
  for (i = 0; i < frame->numParameterSets; i++) {
    llData->bsXXXDataMode[i] = (int8_t)FDKreadBits(bitstream, 2);

    if ((frame->bsIndependencyFlag == 1) && (i == 0) &&
        (llData->bsXXXDataMode[i] == 1 ||
         llData->bsXXXDataMode[i] == 2)) { /* This check catches bitstreams
                                              generated by older encoder that
                                              cause trouble */
      return MPS_PARSE_ERROR;
    }
    if ((i >= frame->numParameterSets - 1) &&
        (llData->bsXXXDataMode[i] ==
         2)) { /* The interpolation mode must not be active for the last
                  parameter set */
      return MPS_PARSE_ERROR;
    }

    if (llData->bsXXXDataMode[i] == 3) {
      dataSets++;
    }
  }

  setIdx = 0;
  bsDataPair = 0;
  oldQuantCoarseXXX = llData->state->bsQuantCoarseXXXprevParse;

  for (i = 0; i < frame->numParameterSets; i++) {
    if (llData->bsXXXDataMode[i] == 0) {
      for (pb = startBand; pb < stopBand; pb++) {
        lastdata[boxIdx][pb] = defaultValue;
      }

      oldQuantCoarseXXX = 0;
    }

    if (llData->bsXXXDataMode[i] == 3) {
      if (bsDataPair) {
        bsDataPair = 0;
      } else {
        bsDataPair = FDKreadBits(bitstream, 1);
        llData->bsQuantCoarseXXX[setIdx] = (uint8_t)FDKreadBits(bitstream, 1);
        llData->bsFreqResStrideXXX[setIdx] = (uint8_t)FDKreadBits(bitstream, 2);

        if (llData->bsQuantCoarseXXX[setIdx] != oldQuantCoarseXXX) {
          if (oldQuantCoarseXXX) {
            coarse2fine(lastdata[boxIdx], (DATA_TYPE)datatype, startBand,
                        stopBand - startBand);
          } else {
            fine2coarse(lastdata[boxIdx], (DATA_TYPE)datatype, startBand,
                        stopBand - startBand);
          }
        }

        dataBands = getStrideMap(llData->bsFreqResStrideXXX[setIdx], startBand,
                                 stopBand, aStrides);

        for (pb = 0; pb < dataBands; pb++) {
          lastdata[boxIdx][startBand + pb] = lastdata[boxIdx][aStrides[pb]];
        }

        if (boxIdx > MAX_NUM_OTT) return MPS_INVALID_BOXIDX;
        if ((setIdx + bsDataPair) > MAX_PARAMETER_SETS)
          return MPS_INVALID_SETIDX;

        /* DECODER_TYPE defined in FDK_tools */
        DECODER_TYPE this_decoder_type = SAC_DECODER;
        if (syntaxFlags & (SACDEC_SYNTAX_USAC | SACDEC_SYNTAX_RSVD50)) {
          this_decoder_type = USAC_DECODER;
        } else if (syntaxFlags & SACDEC_SYNTAX_LD) {
          this_decoder_type = SAOC_DECODER;
        }

        err = (SACDEC_ERROR)EcDataPairDec(
            this_decoder_type, bitstream, data[boxIdx][setIdx + 0],
            data[boxIdx][setIdx + 1], lastdata[boxIdx], (DATA_TYPE)datatype,
            startBand, dataBands, bsDataPair, llData->bsQuantCoarseXXX[setIdx],
            !(frame->bsIndependencyFlag && (i == 0)) || (setIdx > 0));
        if (err != MPS_OK) goto bail;

        if (datatype == t_IPD) {
          const int8_t mask = (llData->bsQuantCoarseXXX[setIdx]) ? 7 : 15;
          for (pb = 0; pb < dataBands; pb++) {
            for (j = aStrides[pb]; j < aStrides[pb + 1]; j++) {
              lastdata[boxIdx][j] =
                  data[boxIdx][setIdx + bsDataPair][startBand + pb] & mask;
            }
          }
        } else {
          for (pb = 0; pb < dataBands; pb++) {
            for (j = aStrides[pb]; j < aStrides[pb + 1]; j++) {
              lastdata[boxIdx][j] =
                  data[boxIdx][setIdx + bsDataPair][startBand + pb];
            }
          }
        }

        oldQuantCoarseXXX = llData->bsQuantCoarseXXX[setIdx];

        if (bsDataPair) {
          llData->bsQuantCoarseXXX[setIdx + 1] =
              llData->bsQuantCoarseXXX[setIdx];
          llData->bsFreqResStrideXXX[setIdx + 1] =
              llData->bsFreqResStrideXXX[setIdx];
        }
        setIdx += bsDataPair + 1;
      } /* !bsDataPair */
    }   /* llData->bsXXXDataMode[i] == 3 */
  }

  llData->state->bsQuantCoarseXXXprevParse = oldQuantCoarseXXX;

bail:
  return err;
}

/*******************************************************************************
 Functionname: parseArbitraryDownmixData
 *******************************************************************************

 Description:

 Arguments:

 Return:

*******************************************************************************/
static SACDEC_ERROR parseArbitraryDownmixData(
    spatialDec *self, const SPATIAL_SPECIFIC_CONFIG *pSSC,
    const uint32_t syntaxFlags, const SPATIAL_BS_FRAME *frame,
    HANDLE_FDK_BITSTREAM bitstream) {
  SACDEC_ERROR err = MPS_OK;
  int32_t ch;
  int32_t offset = pSSC->nOttBoxes;

  /* CLD (arbitrary down-mix gains) */
  for (ch = 0; ch < pSSC->nInputChannels; ch++) {
    err = ecDataDec(frame, syntaxFlags, bitstream,
                    &frame->CLDLosslessData[offset + ch],
                    frame->cmpArbdmxGainIdx, self->cmpArbdmxGainIdxPrev, t_CLD,
                    ch, 0, pSSC->freqRes, arbdmxGainDefault);
    if (err != MPS_OK) return err;
  }

  return err;

} /* parseArbitraryDownmixData */

/*******************************************************************************
 Functionname: SpatialDecParseFrame
 *******************************************************************************

 Description:

 Arguments:

 Input:

 Output:

*******************************************************************************/

static int32_t nBitsParamSlot(int32_t i) {
  int32_t bitsParamSlot;

  bitsParamSlot = fMax(0, DFRACT_BITS - 1 - fNormz((int32_t)i));
  if ((1 << bitsParamSlot) < i) {
    bitsParamSlot++;
  }
  assert((bitsParamSlot >= 0) && (bitsParamSlot <= 32));

  return bitsParamSlot;
}

SACDEC_ERROR SpatialDecParseFrameData(
    spatialDec_struct *self, SPATIAL_BS_FRAME *frame,
    HANDLE_FDK_BITSTREAM bitstream,
    const SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig, UPMIXTYPE upmixType,
    int32_t fGlobalIndependencyFlag) {
  SACDEC_ERROR err = MPS_OK;
  int32_t bsFramingType, dataBands, ps, pg, i;
  int32_t pb;
  int32_t numTempShapeChan = 0;
  int32_t bsNumOutputChannels =
      treePropertyTable[pSpatialSpecificConfig->treeConfig]
          .numOutputChannels; /* CAUTION: Maybe different to
                                 pSpatialSpecificConfig->treeConfig in some
                                 modes! */
  int32_t paramSetErr = 0;
  uint32_t alignAnchor = FDKgetValidBits(
      bitstream); /* Anchor for ByteAlign() function. See comment below. */
  uint32_t syntaxFlags;

  syntaxFlags = pSpatialSpecificConfig->syntaxFlags;

  if ((syntaxFlags & (SACDEC_SYNTAX_USAC | SACDEC_SYNTAX_RSVD50)) &&
      pSpatialSpecificConfig->bsHighRateMode == 0) {
    bsFramingType = 0; /* fixed framing */
    frame->numParameterSets = 1;
  } else {
    bsFramingType = FDKreadBits(bitstream, 1);
    if (syntaxFlags & SACDEC_SYNTAX_LD)
      frame->numParameterSets = FDKreadBits(bitstream, 1) + 1;
    else
      frame->numParameterSets = FDKreadBits(bitstream, 3) + 1;
  }

  /* Any error after this line shall trigger parameter invalidation at bail
   * label. */
  paramSetErr = 1;

  if (frame->numParameterSets >= MAX_PARAMETER_SETS) {
    goto bail;
  }

  /* Basic config check. */
  if (pSpatialSpecificConfig->nInputChannels <= 0 ||
      pSpatialSpecificConfig->nOutputChannels <= 0) {
    err = MPS_UNSUPPORTED_CONFIG;
    goto bail;
  }

  if (bsFramingType) {
    int32_t prevParamSlot = -1;
    int32_t bitsParamSlot;

    {
      bitsParamSlot = nBitsParamSlot(pSpatialSpecificConfig->nTimeSlots);

      for (i = 0; i < frame->numParameterSets; i++) {
        frame->paramSlot[i] = FDKreadBits(bitstream, bitsParamSlot);
        /* Sanity check */
        if ((frame->paramSlot[i] <= prevParamSlot) ||
            (frame->paramSlot[i] >= pSpatialSpecificConfig->nTimeSlots)) {
          err = MPS_PARSE_ERROR;
          goto bail;
        }
        prevParamSlot = frame->paramSlot[i];
      }
    }
  } else {
    for (i = 0; i < frame->numParameterSets; i++) {
      frame->paramSlot[i] = ((pSpatialSpecificConfig->nTimeSlots * (i + 1)) /
                             frame->numParameterSets) -
                            1;
    }
  }

  if ((syntaxFlags & (SACDEC_SYNTAX_USAC | SACDEC_SYNTAX_RSVD50)) &&
      fGlobalIndependencyFlag) {
    frame->bsIndependencyFlag = 1;
  } else {
    frame->bsIndependencyFlag = (uint8_t)FDKreadBits(bitstream, 1);
  }

  /*
   * OttData()
   */
  for (i = 0; i < pSpatialSpecificConfig->nOttBoxes; i++) {
    err = ecDataDec(frame, syntaxFlags, bitstream, &frame->CLDLosslessData[i],
                    frame->cmpOttCLDidx, self->cmpOttCLDidxPrev, t_CLD, i, 0,
                    pSpatialSpecificConfig->bitstreamOttBands[i],
                    pSpatialSpecificConfig->ottCLDdefault[i]);
    if (err != MPS_OK) {
      goto bail;
    }
  } /* i < numOttBoxes */

  {
    for (i = 0; i < pSpatialSpecificConfig->nOttBoxes; i++) {
      err = ecDataDec(frame, syntaxFlags, bitstream, &frame->ICCLosslessData[i],
                      frame->cmpOttICCidx, self->cmpOttICCidxPrev, t_ICC, i, 0,
                      pSpatialSpecificConfig->bitstreamOttBands[i], ICCdefault);
      if (err != MPS_OK) {
        goto bail;
      }
    } /* i < numOttBoxes */
  }   /* !oneICC */

  if ((pSpatialSpecificConfig->treeConfig == SPATIALDEC_MODE_RSVD7) &&
      (pSpatialSpecificConfig->bsPhaseCoding)) {
    frame->phaseMode = FDKreadBits(bitstream, 1);

    if (frame->phaseMode == 0) {
      for (pb = 0; pb < pSpatialSpecificConfig->numOttBandsIPD; pb++) {
        self->cmpOttIPDidxPrev[0][pb] = 0;
        for (i = 0; i < frame->numParameterSets; i++) {
          frame->cmpOttIPDidx[0][i][pb] = 0;
          // frame->ottIPDidx[0][i][pb] = 0;
        }
        /* self->ottIPDidxPrev[0][pb] = 0; */
      }
      frame->OpdSmoothingMode = 0;
    } else {
      frame->OpdSmoothingMode = FDKreadBits(bitstream, 1);
      err = ecDataDec(frame, syntaxFlags, bitstream, &frame->IPDLosslessData[0],
                      frame->cmpOttIPDidx, self->cmpOttIPDidxPrev, t_IPD, 0, 0,
                      pSpatialSpecificConfig->numOttBandsIPD, IPDdefault);
      if (err != MPS_OK) {
        goto bail;
      }
    }
  }

  /*
   * SmgData()
   */

  {
    if (!pSpatialSpecificConfig->bsHighRateMode &&
        (syntaxFlags & SACDEC_SYNTAX_USAC)) {
      for (ps = 0; ps < frame->numParameterSets; ps++) {
        frame->bsSmoothMode[ps] = 0;
      }
    } else {
      for (ps = 0; ps < frame->numParameterSets; ps++) {
        frame->bsSmoothMode[ps] = (uint8_t)FDKreadBits(bitstream, 2);
        if (frame->bsSmoothMode[ps] >= 2) {
          frame->bsSmoothTime[ps] = (uint8_t)FDKreadBits(bitstream, 2);
        }
        if (frame->bsSmoothMode[ps] == 3) {
          frame->bsFreqResStrideSmg[ps] = (uint8_t)FDKreadBits(bitstream, 2);
          dataBands = (pSpatialSpecificConfig->freqRes - 1) /
                          pbStrideTable[frame->bsFreqResStrideSmg[ps]] +
                      1;
          for (pg = 0; pg < dataBands; pg++) {
            frame->bsSmgData[ps][pg] = (uint8_t)FDKreadBits(bitstream, 1);
          }
        }
      } /* ps < numParameterSets */
    }
  }

  /*
   * TempShapeData()
   */
  if ((pSpatialSpecificConfig->tempShapeConfig == 3) &&
      (syntaxFlags & SACDEC_SYNTAX_USAC)) {
    int32_t TsdErr;
    TsdErr = TsdRead(bitstream, pSpatialSpecificConfig->nTimeSlots,
                     &frame->TsdData[0]);
    if (TsdErr) {
      err = MPS_PARSE_ERROR;
      goto bail;
    }
  } else {
    frame->TsdData[0].bsTsdEnable = 0;
  }

  for (i = 0; i < bsNumOutputChannels; i++) {
    frame->tempShapeEnableChannelSTP[i] = 0;
    frame->tempShapeEnableChannelGES[i] = 0;
  }

  if ((pSpatialSpecificConfig->tempShapeConfig == 1) ||
      (pSpatialSpecificConfig->tempShapeConfig == 2)) {
    int32_t bsTempShapeEnable = FDKreadBits(bitstream, 1);
    if (bsTempShapeEnable) {
      numTempShapeChan =
          tempShapeChanTable[pSpatialSpecificConfig->tempShapeConfig - 1]
                            [pSpatialSpecificConfig->treeConfig];
      switch (pSpatialSpecificConfig->tempShapeConfig) {
        case 1: /* STP */
          for (i = 0; i < numTempShapeChan; i++) {
            int32_t stpEnable = FDKreadBits(bitstream, 1);
            frame->tempShapeEnableChannelSTP[i] = stpEnable;
          }
          break;
        case 2: /* GES */
        {
          uint8_t gesChannelEnable[MAX_OUTPUT_CHANNELS];

          for (i = 0; i < numTempShapeChan; i++) {
            gesChannelEnable[i] = (uint8_t)FDKreadBits(bitstream, 1);
            frame->tempShapeEnableChannelGES[i] = gesChannelEnable[i];
          }
          for (i = 0; i < numTempShapeChan; i++) {
            if (gesChannelEnable[i]) {
              int32_t envShapeData_tmp[MAX_TIME_SLOTS];
              if (huff_dec_reshape(bitstream, envShapeData_tmp,
                                   pSpatialSpecificConfig->nTimeSlots) != 0) {
                err = MPS_PARSE_ERROR;
                goto bail;
              }
              for (int32_t ts = 0; ts < pSpatialSpecificConfig->nTimeSlots; ts++) {
                if (!(envShapeData_tmp[ts] >= 0) &&
                    (envShapeData_tmp[ts] <= 4)) {
                  err = MPS_PARSE_ERROR;
                  goto bail;
                }
                frame->bsEnvShapeData[i][ts] = (uint8_t)envShapeData_tmp[ts];
              }
            }
          }
        } break;
        default:
          err = MPS_INVALID_TEMPSHAPE;
          goto bail;
      }
    } /* bsTempShapeEnable */
  }   /* pSpatialSpecificConfig->tempShapeConfig != 0 */

  if (pSpatialSpecificConfig->bArbitraryDownmix != 0) {
    err = parseArbitraryDownmixData(self, pSpatialSpecificConfig, syntaxFlags,
                                    frame, bitstream);
    if (err != MPS_OK) goto bail;
  }

  if (1 && (!(syntaxFlags & (SACDEC_SYNTAX_USAC)))) {
    FDKbyteAlign(bitstream,
                 alignAnchor); /* ISO/IEC FDIS 23003-1: 5.2. ... byte alignment
                                  with respect to the beginning of the syntactic
                                  element in which ByteAlign() occurs. */
  }

bail:
  if (err != MPS_OK && paramSetErr != 0) {
    /* Since the parameter set data has already been written to the instance we
     * need to ... */
    frame->numParameterSets = 0; /* ... signal that it is corrupt ... */
  }

  return err;

} /* SpatialDecParseFrame */

/*******************************************************************************
 Functionname: createMapping
 *******************************************************************************

 Description:

 Arguments:

 Return:

*******************************************************************************/
static void createMapping(int32_t aMap[MAX_PARAMETER_BANDS + 1], int32_t startBand,
                          int32_t stopBand, int32_t stride) {
  int32_t inBands, outBands, bandsAchived, bandsDiff, incr, k, i;
  int32_t vDk[MAX_PARAMETER_BANDS + 1];
  inBands = stopBand - startBand;
  outBands = (inBands - 1) / stride + 1;

  if (outBands < 1) {
    outBands = 1;
  }

  bandsAchived = outBands * stride;
  bandsDiff = inBands - bandsAchived;
  for (i = 0; i < outBands; i++) {
    vDk[i] = stride;
  }

  if (bandsDiff > 0) {
    incr = -1;
    k = outBands - 1;
  } else {
    incr = 1;
    k = 0;
  }

  while (bandsDiff != 0) {
    vDk[k] = vDk[k] - incr;
    k = k + incr;
    bandsDiff = bandsDiff + incr;
    if (k >= outBands) {
      if (bandsDiff > 0) {
        k = outBands - 1;
      } else if (bandsDiff < 0) {
        k = 0;
      }
    }
  }
  aMap[0] = startBand;
  for (i = 0; i < outBands; i++) {
    aMap[i + 1] = aMap[i] + vDk[i];
  }
} /* createMapping */

/*******************************************************************************
 Functionname: mapFrequency
 *******************************************************************************

 Description:

 Arguments:

 Return:

*******************************************************************************/
static void mapFrequency(const int8_t *pInput, /* Input */
                         int8_t *pOutput,      /* Output */
                         int32_t *pMap,           /* Mapping function */
                         int32_t dataBands)       /* Number of data Bands */
{
  int32_t i, j;
  int32_t startBand0 = pMap[0];

  for (i = 0; i < dataBands; i++) {
    int32_t startBand, stopBand, value;

    value = pInput[i + startBand0];

    startBand = pMap[i];
    stopBand = pMap[i + 1];
    for (j = startBand; j < stopBand; j++) {
      pOutput[j] = value;
    }
  }
}

/*******************************************************************************
 Functionname: deq
 *******************************************************************************

 Description:

 Arguments:

 Return:

*******************************************************************************/
static int32_t deqIdx(int32_t value, int32_t paramType) {
  int32_t idx = -1;

  switch (paramType) {
    case t_CLD:
      if (((value + 15) >= 0) && ((value + 15) < 31)) {
        idx = (value + 15);
      }
      break;

    case t_ICC:
      if ((value >= 0) && (value < 8)) {
        idx = value;
      }
      break;

    case t_IPD:
      /* (+/-)15 * MAX_PARAMETER_BANDS for differential coding in frequency
       * domain (according to rbl) */
      if ((value >= -420) && (value <= 420)) {
        idx = (value & 0xf);
      }
      break;

    default:
      assert(0);
  }

  return idx;
}

  /*******************************************************************************
   Functionname: factorFunct
   *******************************************************************************

   Description:

   Arguments:

   Return:

  *******************************************************************************/

#define SF_IDX (7)
#define SF_FACTOR (3)
#define SCALE_FACTOR (1 << SF_FACTOR)
#define SCALE_CLD_C1C2 (1 << SF_CLD_C1C2)

static int32_t factorFunct(int32_t ottVsTotDb, int32_t quantMode) {
  int32_t factor;

  if (ottVsTotDb > FL2FXCONST_DBL(0.0)) {
    ottVsTotDb = FL2FXCONST_DBL(0.0);
  }

  ottVsTotDb = -ottVsTotDb;

  switch (quantMode) {
    case 0:
      factor = FL2FXCONST_DBL(1.0f / SCALE_FACTOR);
      break;
    case 1:
      if (ottVsTotDb >= FL2FXCONST_DBL(21.0f / SCALE_CLD_C1C2))
        factor = FL2FXCONST_DBL(5.0f / SCALE_FACTOR);
      else if (ottVsTotDb <= FL2FXCONST_DBL(1.0f / SCALE_CLD_C1C2))
        factor = FL2FXCONST_DBL(1.0f / SCALE_FACTOR);
      else
        factor = (fMult(FL2FXCONST_DBL(0.2f), ottVsTotDb) +
                  FL2FXCONST_DBL(0.8f / SCALE_CLD_C1C2))
                 << (SF_CLD_C1C2 - SF_FACTOR);
      break;
    case 2:
      if (ottVsTotDb >= FL2FXCONST_DBL(25.0f / SCALE_CLD_C1C2)) {
        assert(SF_FACTOR == 3);
        factor = (int32_t)
            MAXVAL_DBL; /* avoid warning: FL2FXCONST_DBL(8.0f/SCALE_FACTOR) */
      } else if (ottVsTotDb <= FL2FXCONST_DBL(1.0f / SCALE_CLD_C1C2))
        factor = FL2FXCONST_DBL(1.0f / SCALE_FACTOR);
      else
        factor = (fMult(FL2FXCONST_DBL(7.0f / 24.0f), ottVsTotDb) +
                  FL2FXCONST_DBL((17.0f / 24.0f) / SCALE_CLD_C1C2))
                 << (SF_CLD_C1C2 - SF_FACTOR);
      break;
    default:
      factor = FL2FXCONST_DBL(0.0f);
  }

  return (factor);
}

/*******************************************************************************
 Functionname: factorCLD
 *******************************************************************************

 Description:

 Arguments:

 Return:

*******************************************************************************/
static void factorCLD(int8_t *idx, int32_t ottVsTotDb, int32_t *ottVsTotDb1,
                      int32_t *ottVsTotDb2, int8_t ottVsTotDbMode,
                      int32_t quantMode) {
  int32_t factor;
  int32_t cldIdxFract;
  int32_t cldIdx;

  factor = factorFunct(ottVsTotDb, quantMode);

  cldIdxFract =
      fMult((int32_t)((*idx) << ((DFRACT_BITS - 1) - SF_IDX)), factor);
  cldIdxFract += FL2FXCONST_DBL(15.5f / (1 << (SF_FACTOR + SF_IDX)));
  cldIdx = fixp_truncateToInt(cldIdxFract, SF_FACTOR + SF_IDX);

  cldIdx = fMin(cldIdx, 30);
  cldIdx = fMax(cldIdx, 0);

  *idx = cldIdx - 15;

  if (ottVsTotDbMode & ottVsTotDb1Activ)
    (*ottVsTotDb1) = ottVsTotDb + dequantCLD_c1[cldIdx];

  if (ottVsTotDbMode & ottVsTotDb2Activ)
    (*ottVsTotDb2) = ottVsTotDb + dequantCLD_c1[30 - cldIdx];
}

/*******************************************************************************
 Functionname: mapIndexData
 *******************************************************************************

 Description:

 Arguments:

 Return:

*******************************************************************************/
static SACDEC_ERROR mapIndexData(
    LOSSLESSDATA *llData, int8_t ***outputDataIdx, int8_t ***outputIdxData,
    const int8_t (*cmpIdxData)[MAX_PARAMETER_SETS][MAX_PARAMETER_BANDS],
    int8_t ***diffIdxData, int8_t xttIdx, int8_t **idxPrev, int32_t paramIdx,
    int32_t paramType, int32_t startBand, int32_t stopBand, int8_t defaultValue,
    int32_t numParameterSets, const int32_t *paramSlot, int32_t extendFrame, int32_t quantMode,
    SpatialDecConcealmentInfo *concealmentInfo, int8_t ottVsTotDbMode,
    int32_t (*pOttVsTotDbIn)[MAX_PARAMETER_SETS][MAX_PARAMETER_BANDS],
    int32_t (*pOttVsTotDb1)[MAX_PARAMETER_SETS][MAX_PARAMETER_BANDS],
    int32_t (*pOttVsTotDb2)[MAX_PARAMETER_SETS][MAX_PARAMETER_BANDS]) {
  int32_t aParamSlots[MAX_PARAMETER_SETS];
  int32_t aInterpolate[MAX_PARAMETER_SETS] = {0};

  int32_t dataSets;
  int32_t aMap[MAX_PARAMETER_BANDS + 1];

  int32_t setIdx, i, band, parmSlot;
  int32_t dataBands;
  int32_t ps, pb;
  int32_t i1;

  if (numParameterSets > MAX_PARAMETER_SETS) return MPS_WRONG_PARAMETERSETS;

  dataSets = 0;
  for (i = 0; i < numParameterSets; i++) {
    if (llData->bsXXXDataMode[i] == 3) {
      aParamSlots[dataSets] = i;
      dataSets++;
    }
  }

  setIdx = 0;

  /* Main concealment stage is here: */
  SpatialDecConcealment_Apply(
      concealmentInfo, cmpIdxData[xttIdx],
      (diffIdxData != NULL) ? diffIdxData[xttIdx] : NULL, idxPrev[xttIdx],
      llData->bsXXXDataMode, startBand, stopBand, defaultValue, paramType,
      numParameterSets);

  /* Prepare data */
  for (i = 0; i < numParameterSets; i++) {
    if (llData->bsXXXDataMode[i] == 0) {
      llData->nocmpQuantCoarseXXX[i] = 0;
      for (band = startBand; band < stopBand; band++) {
        outputIdxData[xttIdx][i][band] = defaultValue;
      }
      for (band = startBand; band < stopBand; band++) {
        idxPrev[xttIdx][band] = outputIdxData[xttIdx][i][band];
      }
      /* Because the idxPrev are also set to the defaultValue -> signalize fine
       */
      llData->state->bsQuantCoarseXXXprev = 0;
    }

    if (llData->bsXXXDataMode[i] == 1) {
      for (band = startBand; band < stopBand; band++) {
        outputIdxData[xttIdx][i][band] = idxPrev[xttIdx][band];
      }
      llData->nocmpQuantCoarseXXX[i] = llData->state->bsQuantCoarseXXXprev;
    }

    if (llData->bsXXXDataMode[i] == 2) {
      for (band = startBand; band < stopBand; band++) {
        outputIdxData[xttIdx][i][band] = idxPrev[xttIdx][band];
      }
      llData->nocmpQuantCoarseXXX[i] = llData->state->bsQuantCoarseXXXprev;
      aInterpolate[i] = 1;
    } else {
      aInterpolate[i] = 0;
    }

    if (llData->bsXXXDataMode[i] == 3) {
      int32_t stride;

      parmSlot = aParamSlots[setIdx];
      stride = pbStrideTable[llData->bsFreqResStrideXXX[setIdx]];
      dataBands = (stopBand - startBand - 1) / stride + 1;
      createMapping(aMap, startBand, stopBand, stride);
      mapFrequency(&cmpIdxData[xttIdx][setIdx][0],
                   &outputIdxData[xttIdx][parmSlot][0], aMap, dataBands);
      for (band = startBand; band < stopBand; band++) {
        idxPrev[xttIdx][band] = outputIdxData[xttIdx][parmSlot][band];
      }
      llData->state->bsQuantCoarseXXXprev = llData->bsQuantCoarseXXX[setIdx];
      llData->nocmpQuantCoarseXXX[i] = llData->bsQuantCoarseXXX[setIdx];

      setIdx++;
    }
    if (diffIdxData != NULL) {
      for (band = startBand; band < stopBand; band++) {
        outputIdxData[xttIdx][i][band] += diffIdxData[xttIdx][i][band];
      }
    }
  } /* for( i = 0 ; i < numParameterSets; i++ ) */

  /* Map all coarse data to fine */
  for (i = 0; i < numParameterSets; i++) {
    if (llData->nocmpQuantCoarseXXX[i] == 1) {
      coarse2fine(outputIdxData[xttIdx][i], (DATA_TYPE)paramType, startBand,
                  stopBand - startBand);
      llData->nocmpQuantCoarseXXX[i] = 0;
    }
  }

  /* Interpolate */
  i1 = 0;
  for (i = 0; i < numParameterSets; i++) {
    if (aInterpolate[i] != 1) {
      i1 = i;
    } else {
      int32_t xi, i2, x1, x2;

      for (i2 = i; i2 < numParameterSets; i2++) {
        if (aInterpolate[i2] != 1) break;
      }
      if (i2 >= numParameterSets) return MPS_WRONG_PARAMETERSETS;

      x1 = paramSlot[i1];
      xi = paramSlot[i];
      x2 = paramSlot[i2];

      for (band = startBand; band < stopBand; band++) {
        int32_t yi, y1, y2;
        y1 = outputIdxData[xttIdx][i1][band];
        y2 = outputIdxData[xttIdx][i2][band];
        if (x1 != x2) {
          yi = y1 + (xi - x1) * (y2 - y1) / (x2 - x1);
        } else {
          yi = y1 /*+ (xi-x1)*(y2-y1)/1e-12*/;
        }
        outputIdxData[xttIdx][i][band] = yi;
      }
    }
  } /* for( i = 0 ; i < numParameterSets; i++ ) */

  /* Dequantize data and apply factorCLD if necessary */
  for (ps = 0; ps < numParameterSets; ps++) {
    if (quantMode && (paramType == t_CLD)) {
      if (pOttVsTotDbIn == 0) return MPS_WRONG_OTT;
      if ((pOttVsTotDb1 == 0) && (ottVsTotDbMode & ottVsTotDb1Activ))
        return MPS_WRONG_OTT;
      if ((pOttVsTotDb2 == 0) && (ottVsTotDbMode & ottVsTotDb2Activ))
        return MPS_WRONG_OTT;

      for (pb = startBand; pb < stopBand; pb++) {
        factorCLD(&(outputIdxData[xttIdx][ps][pb]), (*pOttVsTotDbIn)[ps][pb],
                  (pOttVsTotDb1 != NULL) ? &((*pOttVsTotDb1)[ps][pb]) : NULL,
                  (pOttVsTotDb2 != NULL) ? &((*pOttVsTotDb2)[ps][pb]) : NULL,
                  ottVsTotDbMode, quantMode);
      }
    }

    /* Dequantize data */
    for (band = startBand; band < stopBand; band++) {
      outputDataIdx[xttIdx][ps][band] =
          deqIdx(outputIdxData[xttIdx][ps][band], paramType);
      if (outputDataIdx[xttIdx][ps][band] == -1) {
        outputDataIdx[xttIdx][ps][band] = defaultValue;
      }
    }
  } /* for( i = 0 ; i < numParameterSets; i++ ) */

  if (extendFrame) {
    if (paramType == t_IPD) {
      llData->bsQuantCoarseXXX[numParameterSets] =
          llData->bsQuantCoarseXXX[numParameterSets - 1];
    }
    for (band = startBand; band < stopBand; band++) {
      outputDataIdx[xttIdx][numParameterSets][band] =
          outputDataIdx[xttIdx][numParameterSets - 1][band];
    }
  }

  return MPS_OK;
}

/*******************************************************************************
 Functionname: decodeAndMapFrameOtt
 *******************************************************************************

 Description:
   Do delta decoding and dequantization

 Arguments:

Input:

Output:

*******************************************************************************/
static SACDEC_ERROR decodeAndMapFrameOtt(HANDLE_SPATIAL_DEC self,
                                         SPATIAL_BS_FRAME *pCurBs) {
  int32_t i, ottIdx;
  int32_t numOttBoxes;

  SACDEC_ERROR err = MPS_OK;

  numOttBoxes = self->numOttBoxes;

  switch (self->treeConfig) {
    default: {
      if (self->quantMode != 0) {
        goto bail;
      }
    }
      for (i = 0; i < numOttBoxes; i++) {
        err = mapIndexData(
            &pCurBs->CLDLosslessData[i], /* LOSSLESSDATA *llData,*/
            self->ottCLD__FDK, self->outIdxData,
            pCurBs
                ->cmpOttCLDidx, /* int32_t
                                   cmpIdxData[MAX_NUM_OTT][MAX_PARAMETER_SETS][MAX_PARAMETER_BANDS],
                                 */
            NULL,               /* no differential data */
            i, /*  int32_t   xttIdx,  Which ott/ttt index to use for input and
                  output buffers */
            self->ottCLDidxPrev,                        /* int32_t
                                                           idxPrev[MAX_NUM_OTT][MAX_PARAMETER_BANDS],
                                                         */
            i, t_CLD, 0,                                /* int32_t   startBand, */
            self->pConfigCurrent->bitstreamOttBands[i], /*  int32_t   stopBand, */
            self->pConfigCurrent->ottCLDdefault[i], /* int32_t   defaultValue, */
            pCurBs->numParameterSets, /* int32_t   numParameterSets) */
            pCurBs->paramSlot, self->extendFrame, self->quantMode,
            &(self->concealInfo), ottVsTotInactiv, NULL, NULL, NULL);
        if (err != MPS_OK) goto bail;

      } /* for(i = 0; i < numOttBoxes ; i++ ) */
      break;
  } /* case */

  for (ottIdx = 0; ottIdx < numOttBoxes; ottIdx++) {
    /* Read ICC */
    err = mapIndexData(
        &pCurBs->ICCLosslessData[ottIdx], /* LOSSLESSDATA *llData,*/
        self->ottICC__FDK, self->outIdxData,
        pCurBs
            ->cmpOttICCidx,  /* int32_t
                                cmpIdxData[MAX_NUM_OTT][MAX_PARAMETER_SETS][MAX_PARAMETER_BANDS],
                              */
        self->ottICCdiffidx, /* differential data */
        ottIdx, /* int32_t   xttIdx,  Which ott/ttt index to use for input and
                   output buffers */
        self->ottICCidxPrev, /* int32_t   idxPrev[MAX_NUM_OTT][MAX_PARAMETER_BANDS],
                              */
        ottIdx, t_ICC, 0,    /* int32_t   startBand, */
        self->pConfigCurrent->bitstreamOttBands[ottIdx], /* int32_t   stopBand, */
        ICCdefault,               /* int32_t   defaultValue, */
        pCurBs->numParameterSets, /* int32_t   numParameterSets) */
        pCurBs->paramSlot, self->extendFrame, self->quantMode,
        &(self->concealInfo), ottVsTotInactiv, NULL, NULL, NULL);
    if (err != MPS_OK) goto bail;
  } /* ottIdx */

  if ((self->treeConfig == TREE_212) && (self->phaseCoding)) {
    if (pCurBs->phaseMode == 0) {
      for (int32_t pb = 0; pb < self->pConfigCurrent->numOttBandsIPD; pb++) {
        self->ottIPDidxPrev[0][pb] = 0;
      }
    }
    for (ottIdx = 0; ottIdx < numOttBoxes; ottIdx++) {
      err = mapIndexData(
          &pCurBs->IPDLosslessData[ottIdx], self->ottIPD__FDK, self->outIdxData,
          pCurBs->cmpOttIPDidx, NULL, ottIdx, self->ottIPDidxPrev, ottIdx,
          t_IPD, 0, self->numOttBandsIPD, IPDdefault, pCurBs->numParameterSets,
          pCurBs->paramSlot, self->extendFrame, self->quantMode,
          &(self->concealInfo), ottVsTotInactiv, NULL, NULL, NULL);
    }
  }

bail:

  return MPS_OK;

} /* decodeAndMapFrameOtt */

/*******************************************************************************
 Functionname: decodeAndMapFrameSmg
 *******************************************************************************

 Description:
   Decode smoothing flags

 Arguments:

Input:

Output:


*******************************************************************************/
static SACDEC_ERROR decodeAndMapFrameSmg(HANDLE_SPATIAL_DEC self,
                                         const SPATIAL_BS_FRAME *frame) {
  int32_t ps, pb, pg, pbStride, dataBands, pbStart, pbStop,
      aGroupToBand[MAX_PARAMETER_BANDS + 1];

  if (frame->numParameterSets > MAX_PARAMETER_SETS)
    return MPS_WRONG_PARAMETERSETS;
  if (self->bitstreamParameterBands > MAX_PARAMETER_BANDS)
    return MPS_WRONG_PARAMETERBANDS;

  for (ps = 0; ps < frame->numParameterSets; ps++) {
    switch (frame->bsSmoothMode[ps]) {
      case 0:
        self->smgTime[ps] = 256;
        memset(self->smgData[ps], 0,
                    self->bitstreamParameterBands * sizeof(uint8_t));
        break;

      case 1:
        if (ps > 0) {
          self->smgTime[ps] = self->smgTime[ps - 1];
          memcpy(self->smgData[ps], self->smgData[ps - 1],
                    self->bitstreamParameterBands * sizeof(uint8_t));
        } else {
          self->smgTime[ps] = self->smoothState->prevSmgTime;
          memcpy(self->smgData[ps], self->smoothState->prevSmgData,
                    self->bitstreamParameterBands * sizeof(uint8_t));
        }
        break;

      case 2:
        self->smgTime[ps] = smgTimeTable[frame->bsSmoothTime[ps]];
        for (pb = 0; pb < self->bitstreamParameterBands; pb++) {
          self->smgData[ps][pb] = 1;
        }
        break;

      case 3:
        self->smgTime[ps] = smgTimeTable[frame->bsSmoothTime[ps]];
        pbStride = pbStrideTable[frame->bsFreqResStrideSmg[ps]];
        dataBands = (self->bitstreamParameterBands - 1) / pbStride + 1;
        createMapping(aGroupToBand, 0, self->bitstreamParameterBands, pbStride);
        for (pg = 0; pg < dataBands; pg++) {
          pbStart = aGroupToBand[pg];
          pbStop = aGroupToBand[pg + 1];
          for (pb = pbStart; pb < pbStop; pb++) {
            self->smgData[ps][pb] = frame->bsSmgData[ps][pg];
          }
        }
        break;
    }
  }

  self->smoothState->prevSmgTime = self->smgTime[frame->numParameterSets - 1];
  memcpy(self->smoothState->prevSmgData,
            self->smgData[frame->numParameterSets - 1],
            self->bitstreamParameterBands * sizeof(uint8_t));

  if (self->extendFrame) {
    self->smgTime[frame->numParameterSets] =
        self->smgTime[frame->numParameterSets - 1];
    memcpy(self->smgData[frame->numParameterSets],
              self->smgData[frame->numParameterSets - 1],
              self->bitstreamParameterBands * sizeof(uint8_t));
  }

  return MPS_OK;
}

/*******************************************************************************
 Functionname: decodeAndMapFrameArbdmx
 *******************************************************************************

 Description:
   Do delta decoding and dequantization

 Arguments:

Input:

Output:

*******************************************************************************/
static SACDEC_ERROR decodeAndMapFrameArbdmx(HANDLE_SPATIAL_DEC self,
                                            const SPATIAL_BS_FRAME *frame) {
  SACDEC_ERROR err = MPS_OK;
  int32_t ch;
  int32_t offset = self->numOttBoxes;

  for (ch = 0; ch < self->numInputChannels; ch++) {
    err = mapIndexData(&frame->CLDLosslessData[offset + ch],
                       self->arbdmxGain__FDK, self->outIdxData,
                       frame->cmpArbdmxGainIdx, NULL, /* no differential data */
                       ch, self->arbdmxGainIdxPrev, offset + ch, t_CLD, 0,
                       self->bitstreamParameterBands,
                       0 /*self->arbdmxGainDefault*/, frame->numParameterSets,
                       frame->paramSlot, self->extendFrame, 0,
                       &(self->concealInfo), ottVsTotInactiv, NULL, NULL, NULL);
    if (err != MPS_OK) goto bail;
  }

bail:
  return err;
} /* decodeAndMapFrameArbdmx */

/*******************************************************************************
 Functionname: SpatialDecDecodeFrame
 *******************************************************************************

 Description:

 Arguments:

 Return:

*******************************************************************************/
SACDEC_ERROR SpatialDecDecodeFrame(spatialDec *self, SPATIAL_BS_FRAME *frame) {
  SACDEC_ERROR err = MPS_OK;

  self->extendFrame = 0;
  if (frame->paramSlot[frame->numParameterSets - 1] != self->timeSlots - 1) {
    self->extendFrame = 1;
  }

  self->TsdTs = 0;

  /****** DTDF and MAP DATA ********/
  if ((err = decodeAndMapFrameOtt(self, frame)) != MPS_OK) goto bail;

  if ((err = decodeAndMapFrameSmg(self, frame)) != MPS_OK) goto bail;

  if (self->arbitraryDownmix != 0) {
    if ((err = decodeAndMapFrameArbdmx(self, frame)) != MPS_OK) goto bail;
  }

  if (self->extendFrame) {
    frame->numParameterSets =
        fixMin(MAX_PARAMETER_SETS, frame->numParameterSets + 1);
    frame->paramSlot[frame->numParameterSets - 1] = self->timeSlots - 1;

    for (int32_t p = 0; p < frame->numParameterSets; p++) {
      if (frame->paramSlot[p] > self->timeSlots - 1) {
        frame->paramSlot[p] = self->timeSlots - 1;
        err = MPS_PARSE_ERROR;
      }
    }
    if (err != MPS_OK) {
      goto bail;
    }
  }

bail:
  return err;
} /* SpatialDecDecodeFrame() */

/*******************************************************************************
 Functionname: SpatialDecodeHeader
 *******************************************************************************

 Description:

 Arguments:

 Return:

*******************************************************************************/

SACDEC_ERROR SpatialDecDecodeHeader(
    spatialDec *self, SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig) {
  SACDEC_ERROR err = MPS_OK;
  int32_t i;

  self->samplingFreq = pSpatialSpecificConfig->samplingFreq;
  self->timeSlots = pSpatialSpecificConfig->nTimeSlots;
  self->frameLength = self->timeSlots * self->qmfBands;
  self->bitstreamParameterBands = pSpatialSpecificConfig->freqRes;

  if (self->pConfigCurrent->syntaxFlags & SACDEC_SYNTAX_LD)
    self->hybridBands = self->qmfBands;
  else
    self->hybridBands = SacGetHybridSubbands(self->qmfBands);
  self->tp_hybBandBorder = 12;

  self->numParameterBands = self->bitstreamParameterBands;

  if (self->pConfigCurrent->syntaxFlags & SACDEC_SYNTAX_LD) {
    switch (self->numParameterBands) {
      case 4:
        self->kernels = kernels_4_to_64;
        break;
      case 5:
        self->kernels = kernels_5_to_64;
        break;
      case 7:
        self->kernels = kernels_7_to_64;
        break;
      case 9:
        self->kernels = kernels_9_to_64;
        break;
      case 12:
        self->kernels = kernels_12_to_64;
        break;
      case 15:
        self->kernels = kernels_15_to_64;
        break;
      case 23:
        self->kernels = kernels_23_to_64;
        break;
      default:
        return MPS_INVALID_PARAMETERBANDS; /* unsupported numParameterBands */
    }
  } else {
    switch (self->numParameterBands) {
      case 4:
        self->kernels = kernels_4_to_71;
        break;
      case 5:
        self->kernels = kernels_5_to_71;
        break;
      case 7:
        self->kernels = kernels_7_to_71;
        break;
      case 10:
        self->kernels = kernels_10_to_71;
        break;
      case 14:
        self->kernels = kernels_14_to_71;
        break;
      case 20:
        self->kernels = kernels_20_to_71;
        break;
      case 28:
        self->kernels = kernels_28_to_71;
        break;
      default:
        return MPS_INVALID_PARAMETERBANDS; /* unsupported numParameterBands */
    }
  }

  /* create param to hyb band table */
  memset(self->param2hyb, 0, (MAX_PARAMETER_BANDS + 1) * sizeof(int32_t));
  for (i = 0; i < self->hybridBands; i++) {
    self->param2hyb[self->kernels[i] + 1] = i + 1;
  }
  {
    int32_t pb = self->kernels[i - 1] + 2;
    for (; pb < (MAX_PARAMETER_BANDS + 1); pb++) {
      self->param2hyb[pb] = i;
    }
    for (pb = 0; pb < MAX_PARAMETER_BANDS; pb += 1) {
      self->kernels_width[pb] = self->param2hyb[pb + 1] - self->param2hyb[pb];
    }
  }

  self->treeConfig = pSpatialSpecificConfig->treeConfig;

  self->numOttBoxes = pSpatialSpecificConfig->nOttBoxes;

  self->numInputChannels = pSpatialSpecificConfig->nInputChannels;

  self->numOutputChannels = pSpatialSpecificConfig->nOutputChannels;

  self->quantMode = pSpatialSpecificConfig->quantMode;

  self->arbitraryDownmix = pSpatialSpecificConfig->bArbitraryDownmix;

  self->numM2rows = self->numOutputChannels;

  {
    self->residualCoding = 0;
    if (self->arbitraryDownmix == 2)
      self->arbitraryDownmix = 1; /* no arbitrary downmix residuals */
  }
  if ((self->pConfigCurrent->syntaxFlags & SACDEC_SYNTAX_USAC)) {
    self->residualCoding = pSpatialSpecificConfig->bResidualCoding;
  }

  self->clipProtectGain__FDK =
      FX_CFG2FX_DBL(clipGainTable__FDK[pSpatialSpecificConfig->bsFixedGainDMX]);
  self->clipProtectGainSF__FDK =
      clipGainSFTable__FDK[pSpatialSpecificConfig->bsFixedGainDMX];

  self->tempShapeConfig = pSpatialSpecificConfig->tempShapeConfig;

  self->decorrConfig = pSpatialSpecificConfig->decorrConfig;

  if (self->upmixType == UPMIXTYPE_BYPASS) {
    self->numOutputChannels = self->numInputChannels;
  }

  self->numOutputChannelsAT = self->numOutputChannels;

  self->numOttBandsIPD = pSpatialSpecificConfig->numOttBandsIPD;
  self->phaseCoding = pSpatialSpecificConfig->bsPhaseCoding;
  for (i = 0; i < self->numOttBoxes; i++) {
    {
      self->pConfigCurrent->bitstreamOttBands[i] =
          self->bitstreamParameterBands;
    }
    self->numOttBands[i] = self->pConfigCurrent->bitstreamOttBands[i];
  } /* i */

  if (self->residualCoding) {
    int32_t numBoxes = self->numOttBoxes;
    for (i = 0; i < numBoxes; i++) {
      self->residualPresent[i] =
          pSpatialSpecificConfig->ResidualConfig[i].bResidualPresent;

      if (self->residualPresent[i]) {
        self->residualBands[i] =
            pSpatialSpecificConfig->ResidualConfig[i].nResidualBands;
        /* conversion from hybrid bands to qmf bands */
        self->residualQMFBands[i] =
            fMax(self->param2hyb[self->residualBands[i]] + 3 - 10,
                 3); /* simplification for the lowest 10 hybrid bands */
      } else {
        self->residualBands[i] = 0;
        self->residualQMFBands[i] = 0;
      }
    }
  } /* self->residualCoding */
  else {
    int32_t boxes = self->numOttBoxes;
    for (i = 0; i < boxes; i += 1) {
      self->residualPresent[i] = 0;
      self->residualBands[i] = 0;
    }
  }

  switch (self->treeConfig) {
    case TREE_212:
      self->numDirektSignals = 1;
      self->numDecorSignals = 1;
      self->numXChannels = 1;
      if (self->arbitraryDownmix == 2) {
        self->numXChannels += 1;
      }
      self->numVChannels = self->numDirektSignals + self->numDecorSignals;
      break;
    default:
      return MPS_INVALID_TREECONFIG;
  }

  self->highRateMode = pSpatialSpecificConfig->bsHighRateMode;
  self->decorrType = pSpatialSpecificConfig->bsDecorrType;

  SpatialDecDecodeHelperInfo(pSpatialSpecificConfig, UPMIXTYPE_NORMAL);

  return err;
}

/*******************************************************************************
 Functionname: SpatialDecCreateBsFrame
 *******************************************************************************

 Description: Create spatial bitstream structure

 Arguments:   spatialDec* self
              const SPATIAL_BS_FRAME **bsFrame

 Return:      -

*******************************************************************************/
SACDEC_ERROR SpatialDecCreateBsFrame(SPATIAL_BS_FRAME *bsFrame,
                                     BS_LL_STATE *llState) {
  SPATIAL_BS_FRAME *pBs = bsFrame;

  const int32_t maxNumOtt = MAX_NUM_OTT;
  const int32_t maxNumInputChannels = MAX_INPUT_CHANNELS;

  FDK_ALLOCATE_MEMORY_1D_P(
      pBs->cmpOttIPDidx, maxNumOtt * MAX_PARAMETER_SETS * MAX_PARAMETER_BANDS,
      int8_t, int8_t(*)[MAX_PARAMETER_SETS][MAX_PARAMETER_BANDS])

  /* Arbitrary Downmix */
  FDK_ALLOCATE_MEMORY_1D_P(
      pBs->cmpArbdmxGainIdx,
      maxNumInputChannels * MAX_PARAMETER_SETS * MAX_PARAMETER_BANDS, int8_t,
      int8_t(*)[MAX_PARAMETER_SETS][MAX_PARAMETER_BANDS])

  /* Lossless control */
  FDK_ALLOCATE_MEMORY_1D(pBs->CLDLosslessData, MAX_NUM_PARAMETERS, LOSSLESSDATA)
  FDK_ALLOCATE_MEMORY_1D(pBs->ICCLosslessData, MAX_NUM_PARAMETERS, LOSSLESSDATA)

  FDK_ALLOCATE_MEMORY_1D(pBs->IPDLosslessData, MAX_NUM_PARAMETERS, LOSSLESSDATA)

  pBs->newBsData = 0;
  pBs->numParameterSets = 1;

  /* Link lossless states */
  for (int32_t x = 0; x < MAX_NUM_PARAMETERS; x++) {
    pBs->CLDLosslessData[x].state = &llState->CLDLosslessState[x];
    pBs->ICCLosslessData[x].state = &llState->ICCLosslessState[x];

    pBs->IPDLosslessData[x].state = &llState->IPDLosslessState[x];
  }

  return MPS_OK;

bail:
  return MPS_OUTOFMEMORY;
}

/*******************************************************************************
 Functionname: SpatialDecCloseBsFrame
 *******************************************************************************

 Description: Close spatial bitstream structure

 Arguments:   spatialDec* self

 Return:      -

*******************************************************************************/
void SpatialDecCloseBsFrame(SPATIAL_BS_FRAME *pBs) {
  if (pBs != NULL) {
    /* These arrays contain the compact indices, only one value per pbstride,
     * only paramsets actually containing data. */

    FDK_FREE_MEMORY_1D(pBs->cmpOttIPDidx);

    /* Arbitrary Downmix */
    FDK_FREE_MEMORY_1D(pBs->cmpArbdmxGainIdx);

    /* Lossless control */
    FDK_FREE_MEMORY_1D(pBs->IPDLosslessData);
    FDK_FREE_MEMORY_1D(pBs->CLDLosslessData);
    FDK_FREE_MEMORY_1D(pBs->ICCLosslessData);
  }
}
