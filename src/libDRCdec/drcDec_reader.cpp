

/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/fixpoint_math.h"
#include "drcDec_reader.h"
#include "drcDec_tools.h"
#include "drcDec_rom.h"
#include "drcDecoder.h"

/* MPEG-D DRC AMD 1 */

#define UNIDRCCONFEXT_PARAM_DRC 0x1
#define UNIDRCCONFEXT_V1 0x2
#define UNIDRCLOUDEXT_EQ 0x1

#define UNIDRCGAINEXT_TERM 0x0
#define UNIDRCLOUDEXT_TERM 0x0
#define UNIDRCCONFEXT_TERM 0x0

static int32_t _getZ(const int32_t nNodesMax) {
  /* Z is the minimum codeword length that is needed to encode all possible
   * timeDelta values */
  /* Z = ceil(log2(2*nNodesMax)) */
  int32_t Z = 1;
  while ((1 << Z) < (2 * nNodesMax)) {
    Z++;
  }
  return Z;
}

static int32_t _getTimeDeltaMin(const GAIN_SET_t* pGset, const int32_t deltaTminDefault) {
  if (pGset->timeDeltaMinPresent) {
    return pGset->timeDeltaMin;
  } else {
    return deltaTminDefault;
  }
}

/* compare and assign */
static inline int32_t _compAssign(uint8_t* dest, const uint8_t src) {
  int32_t diff = 0;
  if (*dest != src) diff = 1;
  *dest = src;
  return diff;
}

static inline int32_t _compAssign(uint32_t* dest, const uint32_t src) {
  int32_t diff = 0;
  if (*dest != src) diff = 1;
  *dest = src;
  return diff;
}

typedef const int8_t (*Huffman)[2];

int32_t _decodeHuffmanCW(Huffman h, /*!< pointer to huffman codebook table */
                     HANDLE_FDK_BITSTREAM hBs) /*!< Handle to bitbuffer */
{
  int8_t index = 0;
  int32_t value, bit;

  while (index >= 0) {
    bit = FDKreadBits(hBs, 1);
    index = h[index][bit];
  }

  value = index + 64; /* Add offset */

  return value;
}

/**********/
/* uniDrc */
/**********/

DRC_ERROR
drcDec_readUniDrc(HANDLE_FDK_BITSTREAM hBs, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
                  HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
                  const int32_t frameSize, const int32_t deltaTminDefault,
                  HANDLE_UNI_DRC_GAIN hUniDrcGain) {
  DRC_ERROR err = DE_OK;
  int32_t loudnessInfoSetPresent, uniDrcConfigPresent;

  loudnessInfoSetPresent = FDKreadBits(hBs, 1);
  if (loudnessInfoSetPresent) {
    uniDrcConfigPresent = FDKreadBits(hBs, 1);
    if (uniDrcConfigPresent) {
      err = drcDec_readUniDrcConfig(hBs, hUniDrcConfig);
      if (err) {
        /* clear config, if parsing error occured */
        memset(hUniDrcConfig, 0, sizeof(UNI_DRC_CONFIG));
        hUniDrcConfig->diff = 1;
      }
    }
    err = drcDec_readLoudnessInfoSet(hBs, hLoudnessInfoSet);
    if (err) {
      /* clear config, if parsing error occured */
      memset(hLoudnessInfoSet, 0, sizeof(LOUDNESS_INFO_SET));
      hLoudnessInfoSet->diff = 1;
    }
  }

  err = drcDec_readUniDrcGain(hBs, hUniDrcConfig, frameSize, deltaTminDefault,
                              hUniDrcGain);

  return err;
}

/**************/
/* uniDrcGain */
/**************/

static int16_t _decodeGainInitial(
    HANDLE_FDK_BITSTREAM hBs, const GAIN_CODING_PROFILE_t gainCodingProfile) {
  int32_t sign, magn;
  int16_t gainInitial = (int16_t)0;
  switch (gainCodingProfile) {
    case GCP_REGULAR:
      sign = FDKreadBits(hBs, 1);
      magn = FDKreadBits(hBs, 8);

      gainInitial =
          (int16_t)(magn << (FRACT_BITS - 1 - 3 - 7)); /* magn * 0.125; */
      if (sign) gainInitial = -gainInitial;
      break;
    case GCP_FADING:
      sign = FDKreadBits(hBs, 1);
      if (sign == 0)
        gainInitial = (int16_t)0;
      else {
        magn = FDKreadBits(hBs, 10);
        gainInitial = -(int16_t)(
            (magn + 1) << (FRACT_BITS - 1 - 3 - 7)); /* - (magn + 1) * 0.125; */
      }
      break;
    case GCP_CLIPPING_DUCKING:
      sign = FDKreadBits(hBs, 1);
      if (sign == 0)
        gainInitial = (int16_t)0;
      else {
        magn = FDKreadBits(hBs, 8);
        gainInitial = -(int16_t)(
            (magn + 1) << (FRACT_BITS - 1 - 3 - 7)); /* - (magn + 1) * 0.125; */
      }
      break;
    case GCP_CONSTANT:
      break;
  }
  return gainInitial;
}

static int32_t _decodeNNodes(HANDLE_FDK_BITSTREAM hBs) {
  int32_t nNodes = 0, endMarker = 0;

  /* decode number of nodes */
  while (endMarker != 1) {
    nNodes++;
    if (nNodes >= 128) break;
    endMarker = FDKreadBits(hBs, 1);
  }
  return nNodes;
}

static void _decodeGains(HANDLE_FDK_BITSTREAM hBs,
                         const GAIN_CODING_PROFILE_t gainCodingProfile,
                         const int32_t nNodes, GAIN_NODE_t* pNodes) {
  int32_t k, deltaGain;
  Huffman deltaGainCodebook;

  pNodes[0].gainDb = _decodeGainInitial(hBs, gainCodingProfile);

  if (gainCodingProfile == GCP_CLIPPING_DUCKING) {
    deltaGainCodebook = (Huffman)&deltaGain_codingProfile_2_huffman;
  } else {
    deltaGainCodebook = (Huffman)&deltaGain_codingProfile_0_1_huffman;
  }

  for (k = 1; k < nNodes; k++) {
    deltaGain = _decodeHuffmanCW(deltaGainCodebook, hBs);
    if (k >= 16) continue;
    /* gain_dB_e = 7 */
    pNodes[k].gainDb =
        pNodes[k - 1].gainDb +
        (int16_t)(deltaGain << (FRACT_BITS - 1 - 7 -
                                 3)); /* pNodes[k-1].gainDb + 0.125*deltaGain */
  }
}

static void _decodeSlopes(HANDLE_FDK_BITSTREAM hBs,
                          const GAIN_INTERPOLATION_TYPE_t gainInterpolationType,
                          const int32_t nNodes, GAIN_NODE_t* pNodes) {
  int32_t k = 0;

  if (gainInterpolationType == GIT_SPLINE) {
    /* decode slope steepness */
    for (k = 0; k < nNodes; k++) {
      _decodeHuffmanCW((Huffman)&slopeSteepness_huffman, hBs);
    }
  }
}

static int32_t _decodeTimeDelta(HANDLE_FDK_BITSTREAM hBs, const int32_t Z) {
  int32_t prefix, mu;

  prefix = FDKreadBits(hBs, 2);
  switch (prefix) {
    case 0x0:
      return 1;
    case 0x1:
      mu = FDKreadBits(hBs, 2);
      return mu + 2;
    case 0x2:
      mu = FDKreadBits(hBs, 3);
      return mu + 6;
    case 0x3:
      mu = FDKreadBits(hBs, Z);
      return mu + 14;
    default:
      return 0;
  }
}

static void _decodeTimes(HANDLE_FDK_BITSTREAM hBs, const int32_t deltaTmin,
                         const int32_t frameSize, const int32_t fullFrame,
                         const int32_t timeOffset, const int32_t Z, const int32_t nNodes,
                         GAIN_NODE_t* pNodes) {
  int32_t timeDelta, k;
  int32_t timeOffs = timeOffset;
  int32_t frameEndFlag, nodeTimeTmp, nodeResFlag;

  if (fullFrame == 0) {
    frameEndFlag = FDKreadBits(hBs, 1);
  } else {
    frameEndFlag = 1;
  }

  if (frameEndFlag ==
      1) { /* frameEndFlag == 1 signals that the last node is at the end of the
              DRC frame */
    nodeResFlag = 0;
    for (k = 0; k < nNodes - 1; k++) {
      /* decode a delta time value */
      timeDelta = _decodeTimeDelta(hBs, Z);
      if (k >= (16 - 1)) continue;
      /* frameEndFlag == 1 needs special handling for last node with node
       * reservoir */
      nodeTimeTmp = timeOffs + timeDelta * deltaTmin;
      if (nodeTimeTmp > frameSize + timeOffset) {
        if (nodeResFlag == 0) {
          pNodes[k].time = frameSize + timeOffset;
          nodeResFlag = 1;
        }
        pNodes[k + 1].time = nodeTimeTmp;
      } else {
        pNodes[k].time = nodeTimeTmp;
      }
      timeOffs = nodeTimeTmp;
    }
    if (nodeResFlag == 0) {
      k = fMin(k, 16 - 1);
      pNodes[k].time = frameSize + timeOffset;
    }
  } else {
    for (k = 0; k < nNodes; k++) {
      /* decode a delta time value */
      timeDelta = _decodeTimeDelta(hBs, Z);
      if (k >= 16) continue;
      pNodes[k].time = timeOffs + timeDelta * deltaTmin;
      timeOffs = pNodes[k].time;
    }
  }
}

static void _readNodes(HANDLE_FDK_BITSTREAM hBs, GAIN_SET_t* gainSet,
                       const int32_t frameSize, const int32_t timeDeltaMin,
                       uint8_t* pNNodes, GAIN_NODE_t* pNodes) {
  int32_t timeOffset, drcGainCodingMode, nNodes;
  int32_t Z = _getZ(frameSize / timeDeltaMin);
  if (gainSet->timeAlignment == 0) {
    timeOffset = -1;
  } else {
    timeOffset = -timeDeltaMin +
                 (timeDeltaMin - 1) /
                     2; /* timeOffset = - deltaTmin + floor((deltaTmin-1)/2); */
  }

  drcGainCodingMode = FDKreadBits(hBs, 1);
  if (drcGainCodingMode == 0) {
    /* "simple" mode: only one node at the end of the frame with slope = 0 */
    nNodes = 1;
    pNodes[0].gainDb = _decodeGainInitial(
        hBs, (GAIN_CODING_PROFILE_t)gainSet->gainCodingProfile);
    pNodes[0].time = frameSize + timeOffset;
  } else {
    nNodes = _decodeNNodes(hBs);

    _decodeSlopes(hBs, (GAIN_INTERPOLATION_TYPE_t)gainSet->gainInterpolationType,
                  nNodes, pNodes);

    _decodeTimes(hBs, timeDeltaMin, frameSize, gainSet->fullFrame, timeOffset,
                 Z, nNodes, pNodes);

    _decodeGains(hBs, (GAIN_CODING_PROFILE_t)gainSet->gainCodingProfile, nNodes,
                 pNodes);
  }
  *pNNodes = (uint8_t)nNodes;
}

static void _readDrcGainSequence(HANDLE_FDK_BITSTREAM hBs, GAIN_SET_t* gainSet,
                                 const int32_t frameSize, const int32_t timeDeltaMin,
                                 uint8_t* pNNodes, GAIN_NODE_t pNodes[16]) {
  int16_t timeBufPrevFrame[16], timeBufCurFrame[16];
  int32_t nNodesNodeRes, nNodesCur, k, m;

  if (gainSet->gainCodingProfile == GCP_CONSTANT) {
    *pNNodes = 1;
    pNodes[0].time = frameSize - 1;
    pNodes[0].gainDb = (int16_t)0;
  } else {
    _readNodes(hBs, gainSet, frameSize, timeDeltaMin, pNNodes, pNodes);

    /* count number of nodes in node reservoir */
    nNodesNodeRes = 0;
    nNodesCur = 0;
    /* count and buffer nodes from node reservoir */
    for (k = 0; k < *pNNodes; k++) {
      if (k >= 16) continue;
      if (pNodes[k].time >= frameSize) {
        /* write node reservoir times into buffer */
        timeBufPrevFrame[nNodesNodeRes] = pNodes[k].time;
        nNodesNodeRes++;
      } else { /* times from current frame */
        timeBufCurFrame[nNodesCur] = pNodes[k].time;
        nNodesCur++;
      }
    }
    /* compose right time order (bit reservoir first) */
    for (k = 0; k < nNodesNodeRes; k++) {
      /* subtract two time frameSize: one to remove node reservoir offset and
       * one to get the negative index relative to the current frame
       */
      pNodes[k].time = timeBufPrevFrame[k] - 2 * frameSize;
    }
    /* ...and times from current frame */
    for (m = 0; m < nNodesCur; m++, k++) {
      pNodes[k].time = timeBufCurFrame[m];
    }
  }
}

static DRC_ERROR _readUniDrcGainExtension(HANDLE_FDK_BITSTREAM hBs,
                                          UNI_DRC_GAIN_EXTENSION_t* pExt) {
  DRC_ERROR err = DE_OK;
  int32_t k, bitSizeLen, extSizeBits, bitSize;

  k = 0;
  pExt->uniDrcGainExtType[k] = FDKreadBits(hBs, 4);
  while (pExt->uniDrcGainExtType[k] != UNIDRCGAINEXT_TERM) {
    if (k >= (8 - 1)) return DE_MEMORY_ERROR;
    bitSizeLen = FDKreadBits(hBs, 3);
    extSizeBits = bitSizeLen + 4;

    bitSize = FDKreadBits(hBs, extSizeBits);
    pExt->extBitSize[k] = bitSize + 1;

    switch (pExt->uniDrcGainExtType[k]) {
      /* add future extensions here */
      default:
        FDKpushFor(hBs, pExt->extBitSize[k]);
        break;
    }
    k++;
    pExt->uniDrcGainExtType[k] = FDKreadBits(hBs, 4);
  }

  return err;
}

DRC_ERROR
drcDec_readUniDrcGain(HANDLE_FDK_BITSTREAM hBs,
                      HANDLE_UNI_DRC_CONFIG hUniDrcConfig, const int32_t frameSize,
                      const int32_t deltaTminDefault,
                      HANDLE_UNI_DRC_GAIN hUniDrcGain) {
  DRC_ERROR err = DE_OK;
  int32_t seq, gainSequenceCount;
  DRC_COEFFICIENTS_UNI_DRC_t* pCoef =
      selectDrcCoefficients(hUniDrcConfig, LOCATION_SELECTED);
  if (hUniDrcGain == NULL) return DE_NOT_OK;
  hUniDrcGain->status = 0;
  if (pCoef) {
    gainSequenceCount = fMin(pCoef->gainSequenceCount, (uint8_t)12);
  } else {
    gainSequenceCount = 0;
  }

  for (seq = 0; seq < gainSequenceCount; seq++) {
    uint8_t index = pCoef->gainSetIndexForGainSequence[seq];
    GAIN_SET_t* gainSet;
    int32_t timeDeltaMin;
    uint8_t tmpNNodes = 0;
    GAIN_NODE_t tmpNodes[16];

    if ((index >= pCoef->gainSetCount) || (index >= 12)) return DE_NOT_OK;
    gainSet = &(pCoef->gainSet[index]);

    timeDeltaMin = _getTimeDeltaMin(gainSet, deltaTminDefault);

    _readDrcGainSequence(hBs, gainSet, frameSize, timeDeltaMin, &tmpNNodes,
                         tmpNodes);

    hUniDrcGain->nNodes[seq] = tmpNNodes;
    memcpy(hUniDrcGain->gainNode[seq], tmpNodes,
              fMin(tmpNNodes, (uint8_t)16) * sizeof(GAIN_NODE_t));
  }

  hUniDrcGain->uniDrcGainExtPresent = FDKreadBits(hBs, 1);
  if (hUniDrcGain->uniDrcGainExtPresent == 1) {
    err = _readUniDrcGainExtension(hBs, &(hUniDrcGain->uniDrcGainExtension));
    if (err) return err;
  }

  if (err == DE_OK && gainSequenceCount > 0) {
    hUniDrcGain->status = 1;
  }
  return err;
}

/****************/
/* uniDrcConfig */
/****************/

static void _decodeDuckingModification(HANDLE_FDK_BITSTREAM hBs,
                                       DUCKING_MODIFICATION_t* pDMod, int32_t isBox) {
  int32_t bsDuckingScaling, sigma, mu;

  if (isBox) FDKpushFor(hBs, 7); /* reserved */
  pDMod->duckingScalingPresent = FDKreadBits(hBs, 1);

  if (pDMod->duckingScalingPresent) {
    if (isBox) FDKpushFor(hBs, 4); /* reserved */
    bsDuckingScaling = FDKreadBits(hBs, 4);
    sigma = bsDuckingScaling >> 3;
    mu = bsDuckingScaling & 0x7;

    if (sigma) {
      pDMod->duckingScaling = (int16_t)(
          (7 - mu) << (FRACT_BITS - 1 - 3 - 2)); /* 1.0 - 0.125 * (1 + mu); */
    } else {
      pDMod->duckingScaling = (int16_t)(
          (9 + mu) << (FRACT_BITS - 1 - 3 - 2)); /* 1.0 + 0.125 * (1 + mu); */
    }
  } else {
    pDMod->duckingScaling = (int16_t)(1 << (FRACT_BITS - 1 - 2)); /* 1.0 */
  }
}

static void _decodeGainModification(HANDLE_FDK_BITSTREAM hBs, const int32_t version,
                                    int32_t bandCount, GAIN_MODIFICATION_t* pGMod,
                                    int32_t isBox) {
  int32_t sign, bsGainOffset, bsAttenuationScaling, bsAmplificationScaling;

  if (version > 0) {
    int32_t b, shapeFilterPresent;

    if (isBox) {
      FDKpushFor(hBs, 4); /* reserved */
      bandCount = FDKreadBits(hBs, 4);
    }

    for (b = 0; b < bandCount; b++) {
      if (isBox) {
        FDKpushFor(hBs, 4); /* reserved */
        pGMod[b].targetCharacteristicLeftPresent = FDKreadBits(hBs, 1);
        pGMod[b].targetCharacteristicRightPresent = FDKreadBits(hBs, 1);
        pGMod[b].gainScalingPresent = FDKreadBits(hBs, 1);
        pGMod[b].gainOffsetPresent = FDKreadBits(hBs, 1);
      }

      if (!isBox)
        pGMod[b].targetCharacteristicLeftPresent = FDKreadBits(hBs, 1);
      if (pGMod[b].targetCharacteristicLeftPresent) {
        if (isBox) FDKpushFor(hBs, 4); /* reserved */
        pGMod[b].targetCharacteristicLeftIndex = FDKreadBits(hBs, 4);
      }
      if (!isBox)
        pGMod[b].targetCharacteristicRightPresent = FDKreadBits(hBs, 1);
      if (pGMod[b].targetCharacteristicRightPresent) {
        if (isBox) FDKpushFor(hBs, 4); /* reserved */
        pGMod[b].targetCharacteristicRightIndex = FDKreadBits(hBs, 4);
      }
      if (!isBox) pGMod[b].gainScalingPresent = FDKreadBits(hBs, 1);
      if (pGMod[b].gainScalingPresent) {
        bsAttenuationScaling = FDKreadBits(hBs, 4);
        pGMod[b].attenuationScaling = (int16_t)(
            bsAttenuationScaling
            << (FRACT_BITS - 1 - 3 - 2)); /* bsAttenuationScaling * 0.125; */
        bsAmplificationScaling = FDKreadBits(hBs, 4);
        pGMod[b].amplificationScaling = (int16_t)(
            bsAmplificationScaling
            << (FRACT_BITS - 1 - 3 - 2)); /* bsAmplificationScaling * 0.125; */
      }
      if (!isBox) pGMod[b].gainOffsetPresent = FDKreadBits(hBs, 1);
      if (pGMod[b].gainOffsetPresent) {
        if (isBox) FDKpushFor(hBs, 2); /* reserved */
        sign = FDKreadBits(hBs, 1);
        bsGainOffset = FDKreadBits(hBs, 5);
        pGMod[b].gainOffset = (int16_t)(
            (1 + bsGainOffset)
            << (FRACT_BITS - 1 - 2 - 4)); /* (1+bsGainOffset) * 0.25; */
        if (sign) {
          pGMod[b].gainOffset = -pGMod[b].gainOffset;
        }
      }
    }
    if (bandCount == 1) {
      shapeFilterPresent = FDKreadBits(hBs, 1);
      if (shapeFilterPresent) {
        if (isBox) FDKpushFor(hBs, 3); /* reserved */
        FDKpushFor(hBs, 4);            /* pGMod->shapeFilterIndex */
      } else {
        if (isBox) FDKpushFor(hBs, 7); /* reserved */
      }
    }
  } else {
    int32_t b, gainScalingPresent, gainOffsetPresent;
    int16_t attenuationScaling = FL2FXCONST_SGL(1.0f / (float)(1 << 2)),
             amplificationScaling = FL2FXCONST_SGL(1.0f / (float)(1 << 2)),
             gainOffset = (int16_t)0;
    if (isBox) FDKpushFor(hBs, 7); /* reserved */
    gainScalingPresent = FDKreadBits(hBs, 1);
    if (gainScalingPresent) {
      bsAttenuationScaling = FDKreadBits(hBs, 4);
      attenuationScaling = (int16_t)(
          bsAttenuationScaling
          << (FRACT_BITS - 1 - 3 - 2)); /* bsAttenuationScaling * 0.125; */
      bsAmplificationScaling = FDKreadBits(hBs, 4);
      amplificationScaling = (int16_t)(
          bsAmplificationScaling
          << (FRACT_BITS - 1 - 3 - 2)); /* bsAmplificationScaling * 0.125; */
    }
    if (isBox) FDKpushFor(hBs, 7); /* reserved */
    gainOffsetPresent = FDKreadBits(hBs, 1);
    if (gainOffsetPresent) {
      if (isBox) FDKpushFor(hBs, 2); /* reserved */
      sign = FDKreadBits(hBs, 1);
      bsGainOffset = FDKreadBits(hBs, 5);
      gainOffset =
          (int16_t)((1 + bsGainOffset) << (FRACT_BITS - 1 - 2 -
                                            4)); /* (1+bsGainOffset) * 0.25; */
      if (sign) {
        gainOffset = -gainOffset;
      }
    }
    for (b = 0; b < 4; b++) {
      pGMod[b].targetCharacteristicLeftPresent = 0;
      pGMod[b].targetCharacteristicRightPresent = 0;
      pGMod[b].gainScalingPresent = gainScalingPresent;
      pGMod[b].attenuationScaling = attenuationScaling;
      pGMod[b].amplificationScaling = amplificationScaling;
      pGMod[b].gainOffsetPresent = gainOffsetPresent;
      pGMod[b].gainOffset = gainOffset;
    }
  }
}

static void _readDrcCharacteristic(HANDLE_FDK_BITSTREAM hBs, const int32_t version,
                                   DRC_CHARACTERISTIC_t* pDChar, int32_t isBox) {
  if (version == 0) {
    if (isBox) FDKpushFor(hBs, 1); /* reserved */
    pDChar->cicpIndex = FDKreadBits(hBs, 7);
    if (pDChar->cicpIndex > 0) {
      pDChar->present = 1;
      pDChar->isCICP = 1;
    } else {
      pDChar->present = 0;
    }
  } else {
    pDChar->present = FDKreadBits(hBs, 1);
    if (isBox) pDChar->isCICP = FDKreadBits(hBs, 1);
    if (pDChar->present) {
      if (!isBox) pDChar->isCICP = FDKreadBits(hBs, 1);
      if (pDChar->isCICP) {
        if (isBox) FDKpushFor(hBs, 1); /* reserved */
        pDChar->cicpIndex = FDKreadBits(hBs, 7);
      } else {
        pDChar->custom.left = FDKreadBits(hBs, 4);
        pDChar->custom.right = FDKreadBits(hBs, 4);
      }
    }
  }
}

static void _readBandBorder(HANDLE_FDK_BITSTREAM hBs, BAND_BORDER_t* pBBord,
                            int32_t drcBandType, int32_t isBox) {
  if (drcBandType) {
    if (isBox) FDKpushFor(hBs, 4); /* reserved */
    pBBord->crossoverFreqIndex = FDKreadBits(hBs, 4);
  } else {
    if (isBox) FDKpushFor(hBs, 6); /* reserved */
    pBBord->startSubBandIndex = FDKreadBits(hBs, 10);
  }
}

static DRC_ERROR _readGainSet(HANDLE_FDK_BITSTREAM hBs, const int32_t version,
                              int32_t* gainSequenceIndex, GAIN_SET_t* pGSet,
                              int32_t isBox) {
  if (isBox) FDKpushFor(hBs, 2); /* reserved */
  pGSet->gainCodingProfile = FDKreadBits(hBs, 2);
  pGSet->gainInterpolationType = FDKreadBits(hBs, 1);
  pGSet->fullFrame = FDKreadBits(hBs, 1);
  pGSet->timeAlignment = FDKreadBits(hBs, 1);
  pGSet->timeDeltaMinPresent = FDKreadBits(hBs, 1);

  if (pGSet->timeDeltaMinPresent) {
    int32_t bsTimeDeltaMin;
    if (isBox) FDKpushFor(hBs, 5); /* reserved */
    bsTimeDeltaMin = FDKreadBits(hBs, 11);
    pGSet->timeDeltaMin = bsTimeDeltaMin + 1;
  }

  if (pGSet->gainCodingProfile != GCP_CONSTANT) {
    int32_t i;
    if (isBox) FDKpushFor(hBs, 3); /* reserved */
    pGSet->bandCount = FDKreadBits(hBs, 4);
    if (pGSet->bandCount > 4) return DE_MEMORY_ERROR;

    if ((pGSet->bandCount > 1) || isBox) {
      pGSet->drcBandType = FDKreadBits(hBs, 1);
    }

    for (i = 0; i < pGSet->bandCount; i++) {
      if (version == 0) {
        *gainSequenceIndex = (*gainSequenceIndex) + 1;
      } else {
        int32_t indexPresent;
        indexPresent = (isBox) ? 1 : FDKreadBits(hBs, 1);
        if (indexPresent) {
          int32_t bsIndex;
          bsIndex = FDKreadBits(hBs, 6);
          *gainSequenceIndex = bsIndex;
        } else {
          *gainSequenceIndex = (*gainSequenceIndex) + 1;
        }
      }
      pGSet->gainSequenceIndex[i] = *gainSequenceIndex;
      _readDrcCharacteristic(hBs, version, &(pGSet->drcCharacteristic[i]),
                             isBox);
    }
    for (i = 1; i < pGSet->bandCount; i++) {
      _readBandBorder(hBs, &(pGSet->bandBorder[i]), pGSet->drcBandType, isBox);
    }
  } else {
    pGSet->bandCount = 1;
    *gainSequenceIndex = (*gainSequenceIndex) + 1;
    pGSet->gainSequenceIndex[0] = *gainSequenceIndex;
  }

  return DE_OK;
}

static DRC_ERROR _readCustomDrcCharacteristic(HANDLE_FDK_BITSTREAM hBs,
                                              const CHARACTERISTIC_SIDE_t side,
                                              uint8_t* pCharacteristicFormat,
                                              CUSTOM_DRC_CHAR_t* pCChar,
                                              int32_t isBox) {
  if (isBox) FDKpushFor(hBs, 7); /* reserved */
  *pCharacteristicFormat = FDKreadBits(hBs, 1);
  if (*pCharacteristicFormat == CF_SIGMOID) {
    int32_t bsGain, bsIoRatio, bsExp;
    if (isBox) FDKpushFor(hBs, 1); /* reserved */
    bsGain = FDKreadBits(hBs, 6);
    if (side == CS_LEFT) {
      pCChar->sigmoid.gain = (int16_t)(bsGain << (FRACT_BITS - 1 - 6));
    } else {
      pCChar->sigmoid.gain = (int16_t)(-bsGain << (FRACT_BITS - 1 - 6));
    }
    bsIoRatio = FDKreadBits(hBs, 4);
    /* pCChar->sigmoid.ioRatio = 0.05 + 0.15 * bsIoRatio; */
    pCChar->sigmoid.ioRatio =
        FL2FXCONST_SGL(0.05f / (float)(1 << 2)) +
        (int16_t)((((3 * bsIoRatio) << (FRACT_BITS - 1)) / 5) >> 4);
    bsExp = FDKreadBits(hBs, 4);
    if (bsExp < 15) {
      pCChar->sigmoid.exp = (int16_t)((1 + 2 * bsExp) << (FRACT_BITS - 1 - 5));
    } else {
      pCChar->sigmoid.exp = (int16_t)MAXVAL_SGL; /* represents infinity */
    }
    pCChar->sigmoid.flipSign = FDKreadBits(hBs, 1);
  } else { /* CF_NODES */
    int32_t i, bsCharacteristicNodeCount, bsNodeLevelDelta, bsNodeGain;
    if (isBox) FDKpushFor(hBs, 6); /* reserved */
    bsCharacteristicNodeCount = FDKreadBits(hBs, 2);
    pCChar->nodes.characteristicNodeCount = bsCharacteristicNodeCount + 1;
    if (pCChar->nodes.characteristicNodeCount > 4) return DE_MEMORY_ERROR;
    pCChar->nodes.nodeLevel[0] = DRC_INPUT_LOUDNESS_TARGET_SGL;
    pCChar->nodes.nodeGain[0] = (int16_t)0;
    for (i = 0; i < pCChar->nodes.characteristicNodeCount; i++) {
      if (isBox) FDKpushFor(hBs, 3); /* reserved */
      bsNodeLevelDelta = FDKreadBits(hBs, 5);
      if (side == CS_LEFT) {
        pCChar->nodes.nodeLevel[i + 1] =
            pCChar->nodes.nodeLevel[i] -
            (int16_t)((1 + bsNodeLevelDelta) << (FRACT_BITS - 1 - 7));
      } else {
        pCChar->nodes.nodeLevel[i + 1] =
            pCChar->nodes.nodeLevel[i] +
            (int16_t)((1 + bsNodeLevelDelta) << (FRACT_BITS - 1 - 7));
      }
      bsNodeGain = FDKreadBits(hBs, 8);
      pCChar->nodes.nodeGain[i + 1] = (int16_t)(
          (bsNodeGain - 128)
          << (FRACT_BITS - 1 - 1 - 7)); /* 0.5f * bsNodeGain - 64.0f; */
    }
  }
  return DE_OK;
}

static void _skipLoudEqInstructions(HANDLE_FDK_BITSTREAM hBs) {
  int32_t i;
  int32_t downmixIdPresent, additionalDownmixIdPresent,
      additionalDownmixIdCount = 0;
  int32_t drcSetIdPresent, additionalDrcSetIdPresent, additionalDrcSetIdCount = 0;
  int32_t eqSetIdPresent, additionalEqSetIdPresent, additionalEqSetIdCount = 0;
  int32_t loudEqGainSequenceCount, drcCharacteristicFormatIsCICP;

  FDKpushFor(hBs, 4); /* loudEqSetId */
  FDKpushFor(hBs, 4); /* drcLocation */
  downmixIdPresent = FDKreadBits(hBs, 1);
  if (downmixIdPresent) {
    FDKpushFor(hBs, 7); /* downmixId */
    additionalDownmixIdPresent = FDKreadBits(hBs, 1);
    if (additionalDownmixIdPresent) {
      additionalDownmixIdCount = FDKreadBits(hBs, 7);
      for (i = 0; i < additionalDownmixIdCount; i++) {
        FDKpushFor(hBs, 7); /* additionalDownmixId */
      }
    }
  }

  drcSetIdPresent = FDKreadBits(hBs, 1);
  if (drcSetIdPresent) {
    FDKpushFor(hBs, 6); /* drcSetId */
    additionalDrcSetIdPresent = FDKreadBits(hBs, 1);
    if (additionalDrcSetIdPresent) {
      additionalDrcSetIdCount = FDKreadBits(hBs, 6);
      for (i = 0; i < additionalDrcSetIdCount; i++) {
        FDKpushFor(hBs, 6); /* additionalDrcSetId; */
      }
    }
  }

  eqSetIdPresent = FDKreadBits(hBs, 1);
  if (eqSetIdPresent) {
    FDKpushFor(hBs, 6); /* eqSetId */
    additionalEqSetIdPresent = FDKreadBits(hBs, 1);
    if (additionalEqSetIdPresent) {
      additionalEqSetIdCount = FDKreadBits(hBs, 6);
      for (i = 0; i < additionalEqSetIdCount; i++) {
        FDKpushFor(hBs, 6); /* additionalEqSetId; */
      }
    }
  }

  FDKpushFor(hBs, 1); /* loudnessAfterDrc */
  FDKpushFor(hBs, 1); /* loudnessAfterEq */
  loudEqGainSequenceCount = FDKreadBits(hBs, 6);
  for (i = 0; i < loudEqGainSequenceCount; i++) {
    FDKpushFor(hBs, 6); /* gainSequenceIndex */
    drcCharacteristicFormatIsCICP = FDKreadBits(hBs, 1);
    if (drcCharacteristicFormatIsCICP) {
      FDKpushFor(hBs, 7); /* drcCharacteristic */
    } else {
      FDKpushFor(hBs, 4); /* drcCharacteristicLeftIndex */
      FDKpushFor(hBs, 4); /* drcCharacteristicRightIndex */
    }
    FDKpushFor(hBs, 6); /* frequencyRangeIndex */
    FDKpushFor(hBs, 3); /* bsLoudEqScaling */
    FDKpushFor(hBs, 5); /* bsLoudEqOffset */
  }
}

static void _skipEqSubbandGainSpline(HANDLE_FDK_BITSTREAM hBs) {
  int32_t nEqNodes, k, bits;
  nEqNodes = FDKreadBits(hBs, 5);
  nEqNodes += 2;
  for (k = 0; k < nEqNodes; k++) {
    bits = FDKreadBits(hBs, 1);
    if (!bits) {
      FDKpushFor(hBs, 4);
    }
  }
  FDKpushFor(hBs, 4 * (nEqNodes - 1));
  bits = FDKreadBits(hBs, 2);
  switch (bits) {
    case 0:
      FDKpushFor(hBs, 5);
      break;
    case 1:
    case 2:
      FDKpushFor(hBs, 4);
      break;
    case 3:
      FDKpushFor(hBs, 3);
      break;
  }
  FDKpushFor(hBs, 5 * (nEqNodes - 1));
}

static void _skipEqCoefficients(HANDLE_FDK_BITSTREAM hBs) {
  int32_t j, k;
  int32_t eqDelayMaxPresent;
  int32_t uniqueFilterBlockCount, filterElementCount, filterElementGainPresent;
  int32_t uniqueTdFilterElementCount, eqFilterFormat, bsRealZeroRadiusOneCount,
      realZeroCount, genericZeroCount, realPoleCount, complexPoleCount,
      firFilterOrder;
  int32_t uniqueEqSubbandGainsCount, eqSubbandGainRepresentation,
      eqSubbandGainCount;
  int32_t eqSubbandGainFormat;

  eqDelayMaxPresent = FDKreadBits(hBs, 1);
  if (eqDelayMaxPresent) {
    FDKpushFor(hBs, 8); /* bsEqDelayMax */
  }

  uniqueFilterBlockCount = FDKreadBits(hBs, 6);
  for (j = 0; j < uniqueFilterBlockCount; j++) {
    filterElementCount = FDKreadBits(hBs, 6);
    for (k = 0; k < filterElementCount; k++) {
      FDKpushFor(hBs, 6); /* filterElementIndex */
      filterElementGainPresent = FDKreadBits(hBs, 1);
      if (filterElementGainPresent) {
        FDKpushFor(hBs, 10); /* bsFilterElementGain */
      }
    }
  }
  uniqueTdFilterElementCount = FDKreadBits(hBs, 6);
  for (j = 0; j < uniqueTdFilterElementCount; j++) {
    eqFilterFormat = FDKreadBits(hBs, 1);
    if (eqFilterFormat == 0) { /* pole/zero */
      bsRealZeroRadiusOneCount = FDKreadBits(hBs, 3);
      realZeroCount = FDKreadBits(hBs, 6);
      genericZeroCount = FDKreadBits(hBs, 6);
      realPoleCount = FDKreadBits(hBs, 4);
      complexPoleCount = FDKreadBits(hBs, 4);
      FDKpushFor(hBs, 2 * bsRealZeroRadiusOneCount * 1);
      FDKpushFor(hBs, realZeroCount * 8);
      FDKpushFor(hBs, genericZeroCount * 14);
      FDKpushFor(hBs, realPoleCount * 8);
      FDKpushFor(hBs, complexPoleCount * 14);
    } else { /* FIR coefficients */
      firFilterOrder = FDKreadBits(hBs, 7);
      FDKpushFor(hBs, 1);
      FDKpushFor(hBs, (firFilterOrder / 2 + 1) * 11);
    }
  }
  uniqueEqSubbandGainsCount = FDKreadBits(hBs, 6);
  if (uniqueEqSubbandGainsCount > 0) {
    eqSubbandGainRepresentation = FDKreadBits(hBs, 1);
    eqSubbandGainFormat = FDKreadBits(hBs, 4);
    switch (eqSubbandGainFormat) {
      case GF_QMF32:
        eqSubbandGainCount = 32;
        break;
      case GF_QMFHYBRID39:
        eqSubbandGainCount = 39;
        break;
      case GF_QMF64:
        eqSubbandGainCount = 64;
        break;
      case GF_QMFHYBRID71:
        eqSubbandGainCount = 71;
        break;
      case GF_QMF128:
        eqSubbandGainCount = 128;
        break;
      case GF_QMFHYBRID135:
        eqSubbandGainCount = 135;
        break;
      case GF_UNIFORM:
      default:
        eqSubbandGainCount = FDKreadBits(hBs, 8);
        eqSubbandGainCount++;
        break;
    }
    for (k = 0; k < uniqueEqSubbandGainsCount; k++) {
      if (eqSubbandGainRepresentation == 1) {
        _skipEqSubbandGainSpline(hBs);
      } else {
        FDKpushFor(hBs, eqSubbandGainCount * 9);
      }
    }
  }
}

static void _skipTdFilterCascade(HANDLE_FDK_BITSTREAM hBs,
                                 const int32_t eqChannelGroupCount) {
  int32_t i, eqCascadeGainPresent, filterBlockCount, eqPhaseAlignmentPresent;
  for (i = 0; i < eqChannelGroupCount; i++) {
    eqCascadeGainPresent = FDKreadBits(hBs, 1);
    if (eqCascadeGainPresent) {
      FDKpushFor(hBs, 10); /* bsEqCascadeGain */
    }
    filterBlockCount = FDKreadBits(hBs, 4);
    FDKpushFor(hBs, filterBlockCount * 7); /* filterBlockIndex */
  }
  eqPhaseAlignmentPresent = FDKreadBits(hBs, 1);
  {
    if (eqPhaseAlignmentPresent) {
      for (i = 0; i < eqChannelGroupCount; i++) {
        FDKpushFor(hBs, (eqChannelGroupCount - i - 1) * 1);
      }
    }
  }
}

static DRC_ERROR _skipEqInstructions(HANDLE_FDK_BITSTREAM hBs,
                                     HANDLE_UNI_DRC_CONFIG hUniDrcConfig) {
  DRC_ERROR err = DE_OK;
  int32_t c, i, k, channelCount;
  int32_t downmixIdPresent, downmixId, eqApplyToDownmix, additionalDownmixIdPresent,
      additionalDownmixIdCount = 0;
  int32_t additionalDrcSetIdPresent, additionalDrcSetIdCount;
  int32_t dependsOnEqSetPresent, eqChannelGroupCount, tdFilterCascadePresent,
      subbandGainsPresent, eqTransitionDurationPresent;
  uint8_t eqChannelGroupForChannel[8];

  FDKpushFor(hBs, 6); /* eqSetId */
  FDKpushFor(hBs, 4); /* eqSetComplexityLevel */
  downmixIdPresent = FDKreadBits(hBs, 1);
  if (downmixIdPresent) {
    downmixId = FDKreadBits(hBs, 7);
    eqApplyToDownmix = FDKreadBits(hBs, 1);
    additionalDownmixIdPresent = FDKreadBits(hBs, 1);
    if (additionalDownmixIdPresent) {
      additionalDownmixIdCount = FDKreadBits(hBs, 7);
      FDKpushFor(hBs, additionalDownmixIdCount * 7); /* additionalDownmixId */
    }
  } else {
    downmixId = 0;
    eqApplyToDownmix = 0;
  }
  FDKpushFor(hBs, 6); /* drcSetId */
  additionalDrcSetIdPresent = FDKreadBits(hBs, 1);
  if (additionalDrcSetIdPresent) {
    additionalDrcSetIdCount = FDKreadBits(hBs, 6);
    for (i = 0; i < additionalDrcSetIdCount; i++) {
      FDKpushFor(hBs, 6); /* additionalDrcSetId */
    }
  }
  FDKpushFor(hBs, 16); /* eqSetPurpose */
  dependsOnEqSetPresent = FDKreadBits(hBs, 1);
  if (dependsOnEqSetPresent) {
    FDKpushFor(hBs, 6); /* dependsOnEqSet */
  } else {
    FDKpushFor(hBs, 1); /* noIndependentEqUse */
  }

  channelCount = hUniDrcConfig->channelLayout.baseChannelCount;
  if ((downmixIdPresent == 1) && (eqApplyToDownmix == 1) && (downmixId != 0) &&
      (downmixId != DOWNMIX_ID_ANY_DOWNMIX) &&
      (additionalDownmixIdCount == 0)) {
    DOWNMIX_INSTRUCTIONS* pDown =
        selectDownmixInstructions(hUniDrcConfig, downmixId);
    if (pDown == NULL) return DE_NOT_OK;

    channelCount =
        pDown->targetChannelCount; /* targetChannelCountFromDownmixId*/
  } else if ((downmixId == DOWNMIX_ID_ANY_DOWNMIX) ||
             (additionalDownmixIdCount > 1)) {
    channelCount = 1;
  }

  eqChannelGroupCount = 0;
  for (c = 0; c < channelCount; c++) {
    int32_t newGroup = 1;
    if (c >= 8) return DE_MEMORY_ERROR;
    eqChannelGroupForChannel[c] = FDKreadBits(hBs, 7);
    for (k = 0; k < c; k++) {
      if (eqChannelGroupForChannel[c] == eqChannelGroupForChannel[k]) {
        newGroup = 0;
      }
    }
    if (newGroup == 1) {
      eqChannelGroupCount += 1;
    }
  }
  tdFilterCascadePresent = FDKreadBits(hBs, 1);
  if (tdFilterCascadePresent) {
    _skipTdFilterCascade(hBs, eqChannelGroupCount);
  }
  subbandGainsPresent = FDKreadBits(hBs, 1);
  if (subbandGainsPresent) {
    FDKpushFor(hBs, eqChannelGroupCount * 6); /* subbandGainsIndex */
  }
  eqTransitionDurationPresent = FDKreadBits(hBs, 1);
  if (eqTransitionDurationPresent) {
    FDKpushFor(hBs, 5); /* bsEqTransitionDuration */
  }
  return err;
}

static void _skipDrcCoefficientsBasic(HANDLE_FDK_BITSTREAM hBs) {
  FDKpushFor(hBs, 4); /* drcLocation */
  FDKpushFor(hBs, 7); /* drcCharacteristic */
}

static DRC_ERROR _readDrcCoefficientsUniDrc(HANDLE_FDK_BITSTREAM hBs,
                                            const int32_t version,
                                            DRC_COEFFICIENTS_UNI_DRC_t* pCoef) {
  DRC_ERROR err = DE_OK;
  int32_t i, bsDrcFrameSize;
  int32_t gainSequenceIndex = -1;

  pCoef->drcLocation = FDKreadBits(hBs, 4);
  pCoef->drcFrameSizePresent = FDKreadBits(hBs, 1);

  if (pCoef->drcFrameSizePresent == 1) {
    bsDrcFrameSize = FDKreadBits(hBs, 15);
    pCoef->drcFrameSize = bsDrcFrameSize + 1;
  }
  if (version == 0) {
    int32_t gainSequenceCount = 0, gainSetCount;
    pCoef->characteristicLeftCount = 0;
    pCoef->characteristicRightCount = 0;
    gainSetCount = FDKreadBits(hBs, 6);
    pCoef->gainSetCount = fMin(gainSetCount, 12);
    for (i = 0; i < gainSetCount; i++) {
      GAIN_SET_t tmpGset;
      memset(&tmpGset, 0, sizeof(GAIN_SET_t));
      err = _readGainSet(hBs, version, &gainSequenceIndex, &tmpGset, 0);
      if (err) return err;
      gainSequenceCount += tmpGset.bandCount;

      if (i >= 12) continue;
      pCoef->gainSet[i] = tmpGset;
    }
    pCoef->gainSequenceCount = gainSequenceCount;
  } else { /* (version == 1) */
    uint8_t drcCharacteristicLeftPresent, drcCharacteristicRightPresent;
    uint8_t shapeFiltersPresent, shapeFilterCount, tmpPresent;
    int32_t gainSetCount;
    drcCharacteristicLeftPresent = FDKreadBits(hBs, 1);
    if (drcCharacteristicLeftPresent) {
      pCoef->characteristicLeftCount = FDKreadBits(hBs, 4);
      if ((pCoef->characteristicLeftCount + 1) > 16) return DE_MEMORY_ERROR;
      for (i = 0; i < pCoef->characteristicLeftCount; i++) {
        err = _readCustomDrcCharacteristic(
            hBs, CS_LEFT, &(pCoef->characteristicLeftFormat[i + 1]),
            &(pCoef->customCharacteristicLeft[i + 1]), 0);
        if (err) return err;
      }
    }
    drcCharacteristicRightPresent = FDKreadBits(hBs, 1);
    if (drcCharacteristicRightPresent) {
      pCoef->characteristicRightCount = FDKreadBits(hBs, 4);
      if ((pCoef->characteristicRightCount + 1) > 16) return DE_MEMORY_ERROR;
      for (i = 0; i < pCoef->characteristicRightCount; i++) {
        err = _readCustomDrcCharacteristic(
            hBs, CS_RIGHT, &(pCoef->characteristicRightFormat[i + 1]),
            &(pCoef->customCharacteristicRight[i + 1]), 0);
        if (err) return err;
      }
    }
    shapeFiltersPresent = FDKreadBits(hBs, 1);
    if (shapeFiltersPresent) {
      shapeFilterCount = FDKreadBits(hBs, 4);
      for (i = 0; i < shapeFilterCount; i++) {
        tmpPresent = FDKreadBits(hBs, 1);
        if (tmpPresent) /* lfCutParams */
          FDKpushFor(hBs, 5);

        tmpPresent = FDKreadBits(hBs, 1);
        if (tmpPresent) /* lfBoostParams */
          FDKpushFor(hBs, 5);

        tmpPresent = FDKreadBits(hBs, 1);
        if (tmpPresent) /* hfCutParams */
          FDKpushFor(hBs, 5);

        tmpPresent = FDKreadBits(hBs, 1);
        if (tmpPresent) /* hfBoostParams */
          FDKpushFor(hBs, 5);
      }
    }
    pCoef->gainSequenceCount = FDKreadBits(hBs, 6);
    gainSetCount = FDKreadBits(hBs, 6);
    pCoef->gainSetCount = fMin(gainSetCount, 12);
    for (i = 0; i < gainSetCount; i++) {
      GAIN_SET_t tmpGset;
      memset(&tmpGset, 0, sizeof(GAIN_SET_t));
      err = _readGainSet(hBs, version, &gainSequenceIndex, &tmpGset, 0);
      if (err) return err;

      if (i >= 12) continue;
      pCoef->gainSet[i] = tmpGset;
    }
  }
  for (i = 0; i < 12; i++) {
    pCoef->gainSetIndexForGainSequence[i] = 255;
  }
  for (i = 0; i < pCoef->gainSetCount; i++) {
    int32_t b;
    for (b = 0; b < pCoef->gainSet[i].bandCount; b++) {
      if (pCoef->gainSet[i].gainSequenceIndex[b] >= 12) continue;
      pCoef->gainSetIndexForGainSequence[pCoef->gainSet[i]
                                             .gainSequenceIndex[b]] = i;
    }
  }

  return err;
}

static void _skipDrcInstructionsBasic(HANDLE_FDK_BITSTREAM hBs) {
  int32_t drcSetEffect;
  int32_t additionalDownmixIdPresent, additionalDownmixIdCount,
      limiterPeakTargetPresent;
  int32_t drcSetTargetLoudnessPresent, drcSetTargetLoudnessValueLowerPresent;

  FDKpushFor(hBs, 6); /* drcSetId */
  FDKpushFor(hBs, 4); /* drcLocation */
  FDKpushFor(hBs, 7); /* downmixId */
  additionalDownmixIdPresent = FDKreadBits(hBs, 1);
  if (additionalDownmixIdPresent) {
    additionalDownmixIdCount = FDKreadBits(hBs, 3);
    FDKpushFor(hBs, 7 * additionalDownmixIdCount); /* additionalDownmixId */
  }

  drcSetEffect = FDKreadBits(hBs, 16);
  if (!(drcSetEffect & (EB_DUCK_OTHER | EB_DUCK_SELF))) {
    limiterPeakTargetPresent = FDKreadBits(hBs, 1);
    if (limiterPeakTargetPresent) {
      FDKpushFor(hBs, 8); /* bsLimiterPeakTarget */
    }
  }

  drcSetTargetLoudnessPresent = FDKreadBits(hBs, 1);
  if (drcSetTargetLoudnessPresent) {
    FDKpushFor(hBs, 6); /* bsDrcSetTargetLoudnessValueUpper */
    drcSetTargetLoudnessValueLowerPresent = FDKreadBits(hBs, 1);
    if (drcSetTargetLoudnessValueLowerPresent) {
      FDKpushFor(hBs, 6); /* bsDrcSetTargetLoudnessValueLower */
    }
  }
}

static DRC_ERROR _readDrcInstructionsUniDrc(HANDLE_FDK_BITSTREAM hBs,
                                            const int32_t version,
                                            HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
                                            DRC_INSTRUCTIONS_UNI_DRC_t* pInst) {
  DRC_ERROR err = DE_OK;
  int32_t i, g, c;
  int32_t downmixIdPresent, additionalDownmixIdPresent, additionalDownmixIdCount;
  int32_t bsLimiterPeakTarget, channelCount;
  DRC_COEFFICIENTS_UNI_DRC_t* pCoef = NULL;
  int32_t repeatParameters, bsRepeatParametersCount;
  int32_t repeatSequenceIndex, bsRepeatSequenceCount;
  int8_t* gainSetIndex = pInst->gainSetIndex;
  int8_t channelGroupForChannel[8];
  DUCKING_MODIFICATION_t duckingModificationForChannelGroup[8];

  pInst->drcSetId = FDKreadBits(hBs, 6);
  if (version == 0) {
    /* Assume all v0 DRC sets to be manageable in terms of complexity */
    pInst->drcSetComplexityLevel = 2;
  } else {
    pInst->drcSetComplexityLevel = FDKreadBits(hBs, 4);
  }
  pInst->drcLocation = FDKreadBits(hBs, 4);
  if (version == 0) {
    downmixIdPresent = 1;
  } else {
    downmixIdPresent = FDKreadBits(hBs, 1);
  }
  if (downmixIdPresent) {
    pInst->downmixId[0] = FDKreadBits(hBs, 7);
    if (version == 0) {
      if (pInst->downmixId[0] == 0)
        pInst->drcApplyToDownmix = 0;
      else
        pInst->drcApplyToDownmix = 1;
    } else {
      pInst->drcApplyToDownmix = FDKreadBits(hBs, 1);
    }

    additionalDownmixIdPresent = FDKreadBits(hBs, 1);
    if (additionalDownmixIdPresent) {
      additionalDownmixIdCount = FDKreadBits(hBs, 3);
      if ((1 + additionalDownmixIdCount) > 8) return DE_MEMORY_ERROR;
      for (i = 0; i < additionalDownmixIdCount; i++) {
        pInst->downmixId[i + 1] = FDKreadBits(hBs, 7);
      }
      pInst->downmixIdCount = 1 + additionalDownmixIdCount;
    } else {
      pInst->downmixIdCount = 1;
    }
  } else {
    pInst->downmixId[0] = 0;
    pInst->downmixIdCount = 1;
  }

  pInst->drcSetEffect = FDKreadBits(hBs, 16);

  if ((pInst->drcSetEffect & (EB_DUCK_OTHER | EB_DUCK_SELF)) == 0) {
    pInst->limiterPeakTargetPresent = FDKreadBits(hBs, 1);
    if (pInst->limiterPeakTargetPresent) {
      bsLimiterPeakTarget = FDKreadBits(hBs, 8);
      pInst->limiterPeakTarget = -(int16_t)(
          bsLimiterPeakTarget
          << (FRACT_BITS - 1 - 3 - 5)); /* - bsLimiterPeakTarget * 0.125; */
    }
  }

  pInst->drcSetTargetLoudnessPresent = FDKreadBits(hBs, 1);

  /* set default values */
  pInst->drcSetTargetLoudnessValueUpper = 0;
  pInst->drcSetTargetLoudnessValueLower = -63;

  if (pInst->drcSetTargetLoudnessPresent == 1) {
    int32_t bsDrcSetTargetLoudnessValueUpper, bsDrcSetTargetLoudnessValueLower;
    int32_t drcSetTargetLoudnessValueLowerPresent;
    bsDrcSetTargetLoudnessValueUpper = FDKreadBits(hBs, 6);
    pInst->drcSetTargetLoudnessValueUpper =
        bsDrcSetTargetLoudnessValueUpper - 63;
    drcSetTargetLoudnessValueLowerPresent = FDKreadBits(hBs, 1);
    if (drcSetTargetLoudnessValueLowerPresent == 1) {
      bsDrcSetTargetLoudnessValueLower = FDKreadBits(hBs, 6);
      pInst->drcSetTargetLoudnessValueLower =
          bsDrcSetTargetLoudnessValueLower - 63;
    }
  }

  pInst->dependsOnDrcSetPresent = FDKreadBits(hBs, 1);

  pInst->noIndependentUse = 0;
  if (pInst->dependsOnDrcSetPresent) {
    pInst->dependsOnDrcSet = FDKreadBits(hBs, 6);
  } else {
    pInst->noIndependentUse = FDKreadBits(hBs, 1);
  }

  if (version == 0) {
    pInst->requiresEq = 0;
  } else {
    pInst->requiresEq = FDKreadBits(hBs, 1);
  }

  pCoef = selectDrcCoefficients(hUniDrcConfig, pInst->drcLocation);

  pInst->drcChannelCount = channelCount =
      hUniDrcConfig->channelLayout.baseChannelCount;

  if (pInst->drcSetEffect & (EB_DUCK_OTHER | EB_DUCK_SELF)) {
    DUCKING_MODIFICATION_t* pDModForChannel =
        pInst->duckingModificationForChannel;
    c = 0;
    while (c < channelCount) {
      int32_t bsGainSetIndex;
      bsGainSetIndex = FDKreadBits(hBs, 6);
      if (c >= 8) return DE_MEMORY_ERROR;
      gainSetIndex[c] = bsGainSetIndex - 1;
      _decodeDuckingModification(hBs, &(pDModForChannel[c]), 0);

      c++;
      repeatParameters = FDKreadBits(hBs, 1);
      if (repeatParameters == 1) {
        bsRepeatParametersCount = FDKreadBits(hBs, 5);
        bsRepeatParametersCount += 1;
        for (i = 0; i < bsRepeatParametersCount; i++) {
          if (c >= 8) return DE_MEMORY_ERROR;
          gainSetIndex[c] = gainSetIndex[c - 1];
          pDModForChannel[c] = pDModForChannel[c - 1];
          c++;
        }
      }
    }
    if (c > channelCount) {
      return DE_NOT_OK;
    }

    err = deriveDrcChannelGroups(
        pInst->drcSetEffect, pInst->drcChannelCount, gainSetIndex,
        pDModForChannel, &pInst->nDrcChannelGroups,
        pInst->gainSetIndexForChannelGroup, channelGroupForChannel,
        duckingModificationForChannelGroup);
    if (err) return (err);
  } else {
    int32_t deriveChannelCount = 0;
    if (((version == 0) || (pInst->drcApplyToDownmix != 0)) &&
        (pInst->downmixId[0] != DOWNMIX_ID_BASE_LAYOUT) &&
        (pInst->downmixId[0] != DOWNMIX_ID_ANY_DOWNMIX) &&
        (pInst->downmixIdCount == 1)) {
      if (hUniDrcConfig->downmixInstructionsCount != 0) {
        DOWNMIX_INSTRUCTIONS* pDown =
            selectDownmixInstructions(hUniDrcConfig, pInst->downmixId[0]);
        if (pDown == NULL) return DE_NOT_OK;
        pInst->drcChannelCount = channelCount =
            pDown->targetChannelCount; /* targetChannelCountFromDownmixId*/
      } else {
        deriveChannelCount = 1;
        channelCount = 1;
      }
    } else if (((version == 0) || (pInst->drcApplyToDownmix != 0)) &&
               ((pInst->downmixId[0] == DOWNMIX_ID_ANY_DOWNMIX) ||
                (pInst->downmixIdCount > 1))) {
      /* Set maximum channel count as upper border. The effective channel count
       * is set at the process function. */
      pInst->drcChannelCount = 8;
      channelCount = 1;
    }

    c = 0;
    while (c < channelCount) {
      int32_t bsGainSetIndex;
      bsGainSetIndex = FDKreadBits(hBs, 6);
      if (c >= 8) return DE_MEMORY_ERROR;
      gainSetIndex[c] = bsGainSetIndex - 1;
      c++;
      repeatSequenceIndex = FDKreadBits(hBs, 1);

      if (repeatSequenceIndex == 1) {
        bsRepeatSequenceCount = FDKreadBits(hBs, 5);
        bsRepeatSequenceCount += 1;
        if (deriveChannelCount) {
          channelCount = 1 + bsRepeatSequenceCount;
        }
        for (i = 0; i < bsRepeatSequenceCount; i++) {
          if (c >= 8) return DE_MEMORY_ERROR;
          gainSetIndex[c] = bsGainSetIndex - 1;
          c++;
        }
      }
    }
    if (c > channelCount) {
      return DE_NOT_OK;
    }
    if (deriveChannelCount) {
      pInst->drcChannelCount = channelCount;
    }

    /* DOWNMIX_ID_ANY_DOWNMIX: channelCount is 1. Distribute gainSetIndex to all
     * channels. */
    if ((pInst->downmixId[0] == DOWNMIX_ID_ANY_DOWNMIX) ||
        (pInst->downmixIdCount > 1)) {
      for (c = 1; c < pInst->drcChannelCount; c++) {
        gainSetIndex[c] = gainSetIndex[0];
      }
    }

    err = deriveDrcChannelGroups(pInst->drcSetEffect, pInst->drcChannelCount,
                                 gainSetIndex, NULL, &pInst->nDrcChannelGroups,
                                 pInst->gainSetIndexForChannelGroup,
                                 channelGroupForChannel, NULL);
    if (err) return (err);

    for (g = 0; g < pInst->nDrcChannelGroups; g++) {
      int32_t set, bandCount;
      set = pInst->gainSetIndexForChannelGroup[g];

      /* get bandCount */
      if (pCoef != NULL && set < pCoef->gainSetCount) {
        bandCount = pCoef->gainSet[set].bandCount;
      } else {
        bandCount = 1;
      }

      _decodeGainModification(hBs, version, bandCount,
                              pInst->gainModificationForChannelGroup[g], 0);
    }
  }

  return err;
}

static DRC_ERROR _readChannelLayout(HANDLE_FDK_BITSTREAM hBs,
                                    CHANNEL_LAYOUT* pChan) {
  DRC_ERROR err = DE_OK;

  pChan->baseChannelCount = FDKreadBits(hBs, 7);

  if (pChan->baseChannelCount > 8) return DE_NOT_OK;

  pChan->layoutSignalingPresent = FDKreadBits(hBs, 1);

  if (pChan->layoutSignalingPresent) {
    pChan->definedLayout = FDKreadBits(hBs, 8);

    if (pChan->definedLayout == 0) {
      int32_t i;
      for (i = 0; i < pChan->baseChannelCount; i++) {
        if (i < 8) {
          pChan->speakerPosition[i] = FDKreadBits(hBs, 7);
        } else {
          FDKpushFor(hBs, 7);
        }
      }
    }
  }
  return err;
}

static DRC_ERROR _readDownmixInstructions(HANDLE_FDK_BITSTREAM hBs,
                                          const int32_t version,
                                          CHANNEL_LAYOUT* pChan,
                                          DOWNMIX_INSTRUCTIONS* pDown) {
  DRC_ERROR err = DE_OK;

  pDown->downmixId = FDKreadBits(hBs, 7);
  pDown->targetChannelCount = FDKreadBits(hBs, 7);
  pDown->targetLayout = FDKreadBits(hBs, 8);
  pDown->downmixCoefficientsPresent = FDKreadBits(hBs, 1);

  if (pDown->downmixCoefficientsPresent) {
    int32_t nDownmixCoeffs = pDown->targetChannelCount * pChan->baseChannelCount;
    int32_t i;
    if (nDownmixCoeffs > 8 * 8) return DE_NOT_OK;
    if (version == 0) {
      pDown->bsDownmixOffset = 0;
      for (i = 0; i < nDownmixCoeffs; i++) {
        /* LFE downmix coefficients are not supported. */
        pDown->downmixCoefficient[i] = downmixCoeff[FDKreadBits(hBs, 4)];
      }
    } else {
      pDown->bsDownmixOffset = FDKreadBits(hBs, 4);
      for (i = 0; i < nDownmixCoeffs; i++) {
        pDown->downmixCoefficient[i] = downmixCoeffV1[FDKreadBits(hBs, 5)];
      }
    }
  }
  return err;
}

static DRC_ERROR _readDrcExtensionV1(HANDLE_FDK_BITSTREAM hBs,
                                     HANDLE_UNI_DRC_CONFIG hUniDrcConfig) {
  DRC_ERROR err = DE_OK;
  int32_t downmixInstructionsV1Present;
  int32_t drcCoeffsAndInstructionsUniDrcV1Present;
  int32_t loudEqInstructionsPresent, loudEqInstructionsCount;
  int32_t eqPresent, eqInstructionsCount;
  int32_t i, offset;
  int32_t diff = hUniDrcConfig->diff;

  downmixInstructionsV1Present = FDKreadBits(hBs, 1);
  if (downmixInstructionsV1Present == 1) {
    diff |= _compAssign(&hUniDrcConfig->downmixInstructionsCountV1,
                        FDKreadBits(hBs, 7));
    offset = hUniDrcConfig->downmixInstructionsCountV0;
    hUniDrcConfig->downmixInstructionsCount = fMin(
        (uint8_t)(offset + hUniDrcConfig->downmixInstructionsCountV1), (uint8_t)6);
    for (i = 0; i < hUniDrcConfig->downmixInstructionsCountV1; i++) {
      DOWNMIX_INSTRUCTIONS tmpDown;
      memset(&tmpDown, 0, sizeof(DOWNMIX_INSTRUCTIONS));
      err = _readDownmixInstructions(hBs, 1, &hUniDrcConfig->channelLayout,
                                     &tmpDown);
      if (err) return err;
      if ((offset + i) >= 6) continue;
      if (!diff)
        diff |= (memcmp(&tmpDown,
                           &(hUniDrcConfig->downmixInstructions[offset + i]),
                           sizeof(DOWNMIX_INSTRUCTIONS)) != 0);
      hUniDrcConfig->downmixInstructions[offset + i] = tmpDown;
    }
  } else {
    diff |= _compAssign(&hUniDrcConfig->downmixInstructionsCountV1, 0);
  }

  drcCoeffsAndInstructionsUniDrcV1Present = FDKreadBits(hBs, 1);
  if (drcCoeffsAndInstructionsUniDrcV1Present == 1) {
    diff |= _compAssign(&hUniDrcConfig->drcCoefficientsUniDrcCountV1,
                        FDKreadBits(hBs, 3));
    offset = hUniDrcConfig->drcCoefficientsUniDrcCountV0;
    hUniDrcConfig->drcCoefficientsUniDrcCount =
        fMin((uint8_t)(offset + hUniDrcConfig->drcCoefficientsUniDrcCountV1),
             (uint8_t)2);
    for (i = 0; i < hUniDrcConfig->drcCoefficientsUniDrcCountV1; i++) {
      DRC_COEFFICIENTS_UNI_DRC_t tmpCoef;
      memset(&tmpCoef, 0, sizeof(DRC_COEFFICIENTS_UNI_DRC_t));
      err = _readDrcCoefficientsUniDrc(hBs, 1, &tmpCoef);
      if (err) return err;
      if ((offset + i) >= 2) continue;
      if (!diff)
        diff |= (memcmp(&tmpCoef,
                           &(hUniDrcConfig->drcCoefficientsUniDrc[offset + i]),
                           sizeof(DRC_COEFFICIENTS_UNI_DRC_t)) != 0);
      hUniDrcConfig->drcCoefficientsUniDrc[offset + i] = tmpCoef;
    }

    diff |= _compAssign(&hUniDrcConfig->drcInstructionsUniDrcCountV1,
                        FDKreadBits(hBs, 6));
    offset = hUniDrcConfig->drcInstructionsUniDrcCount;
    hUniDrcConfig->drcInstructionsUniDrcCount =
        fMin((uint8_t)(offset + hUniDrcConfig->drcInstructionsUniDrcCountV1),
             (uint8_t)12);
    for (i = 0; i < hUniDrcConfig->drcInstructionsUniDrcCount; i++) {
      DRC_INSTRUCTIONS_UNI_DRC_t tmpInst;
      memset(&tmpInst, 0, sizeof(DRC_INSTRUCTIONS_UNI_DRC_t));
      err = _readDrcInstructionsUniDrc(hBs, 1, hUniDrcConfig, &tmpInst);
      if (err) return err;
      if ((offset + i) >= 12) continue;
      if (!diff)
        diff |= (memcmp(&tmpInst,
                           &(hUniDrcConfig->drcInstructionsUniDrc[offset + i]),
                           sizeof(DRC_INSTRUCTIONS_UNI_DRC_t)) != 0);
      hUniDrcConfig->drcInstructionsUniDrc[offset + i] = tmpInst;
    }
  } else {
    diff |= _compAssign(&hUniDrcConfig->drcCoefficientsUniDrcCountV1, 0);
    diff |= _compAssign(&hUniDrcConfig->drcInstructionsUniDrcCountV1, 0);
  }

  loudEqInstructionsPresent = FDKreadBits(hBs, 1);
  if (loudEqInstructionsPresent == 1) {
    loudEqInstructionsCount = FDKreadBits(hBs, 4);
    for (i = 0; i < loudEqInstructionsCount; i++) {
      _skipLoudEqInstructions(hBs);
    }
  }

  eqPresent = FDKreadBits(hBs, 1);
  if (eqPresent == 1) {
    _skipEqCoefficients(hBs);
    eqInstructionsCount = FDKreadBits(hBs, 4);
    for (i = 0; i < eqInstructionsCount; i++) {
      _skipEqInstructions(hBs, hUniDrcConfig);
    }
  }

  hUniDrcConfig->diff = diff;

  return err;
}

static DRC_ERROR _readUniDrcConfigExtension(
    HANDLE_FDK_BITSTREAM hBs, HANDLE_UNI_DRC_CONFIG hUniDrcConfig) {
  DRC_ERROR err = DE_OK;
  int32_t k, bitSizeLen, extSizeBits, bitSize;
  int32_t nBitsRemaining;
  UNI_DRC_CONFIG_EXTENSION* pExt = &(hUniDrcConfig->uniDrcConfigExt);

  k = 0;
  pExt->uniDrcConfigExtType[k] = FDKreadBits(hBs, 4);
  while (pExt->uniDrcConfigExtType[k] != UNIDRCCONFEXT_TERM) {
    if (k >= (8 - 1)) return DE_MEMORY_ERROR;
    bitSizeLen = FDKreadBits(hBs, 4);
    extSizeBits = bitSizeLen + 4;

    bitSize = FDKreadBits(hBs, extSizeBits);
    pExt->extBitSize[k] = bitSize + 1;
    nBitsRemaining = (int32_t)FDKgetValidBits(hBs);

    switch (pExt->uniDrcConfigExtType[k]) {
      case UNIDRCCONFEXT_V1:
        err = _readDrcExtensionV1(hBs, hUniDrcConfig);
        if (err) return err;
        if (nBitsRemaining !=
            ((int32_t)pExt->extBitSize[k] + (int32_t)FDKgetValidBits(hBs)))
          return DE_NOT_OK;
        break;
      case UNIDRCCONFEXT_PARAM_DRC:
      /* add future extensions here */
      default:
        FDKpushFor(hBs, pExt->extBitSize[k]);
        break;
    }
    k++;
    pExt->uniDrcConfigExtType[k] = FDKreadBits(hBs, 4);
  }

  return err;
}

DRC_ERROR
drcDec_readUniDrcConfig(HANDLE_FDK_BITSTREAM hBs,
                        HANDLE_UNI_DRC_CONFIG hUniDrcConfig) {
  DRC_ERROR err = DE_OK;
  int32_t i, diff = 0;
  int32_t drcDescriptionBasicPresent, drcCoefficientsBasicCount,
      drcInstructionsBasicCount;
  CHANNEL_LAYOUT tmpChan;
  memset(&tmpChan, 0, sizeof(CHANNEL_LAYOUT));
  if (hUniDrcConfig == NULL) return DE_NOT_OK;

  diff |= _compAssign(&hUniDrcConfig->sampleRatePresent, FDKreadBits(hBs, 1));

  if (hUniDrcConfig->sampleRatePresent == 1) {
    diff |=
        _compAssign(&hUniDrcConfig->sampleRate, FDKreadBits(hBs, 18) + 1000);
  }

  diff |= _compAssign(&hUniDrcConfig->downmixInstructionsCountV0,
                      FDKreadBits(hBs, 7));

  drcDescriptionBasicPresent = FDKreadBits(hBs, 1);
  if (drcDescriptionBasicPresent == 1) {
    drcCoefficientsBasicCount = FDKreadBits(hBs, 3);
    drcInstructionsBasicCount = FDKreadBits(hBs, 4);
  } else {
    drcCoefficientsBasicCount = 0;
    drcInstructionsBasicCount = 0;
  }

  diff |= _compAssign(&hUniDrcConfig->drcCoefficientsUniDrcCountV0,
                      FDKreadBits(hBs, 3));
  diff |= _compAssign(&hUniDrcConfig->drcInstructionsUniDrcCountV0,
                      FDKreadBits(hBs, 6));

  err = _readChannelLayout(hBs, &tmpChan);
  if (err) return err;

  if (!diff)
    diff |= (memcmp(&tmpChan, &hUniDrcConfig->channelLayout,
                       sizeof(CHANNEL_LAYOUT)) != 0);
  hUniDrcConfig->channelLayout = tmpChan;

  hUniDrcConfig->downmixInstructionsCount =
      fMin(hUniDrcConfig->downmixInstructionsCountV0, (uint8_t)6);
  for (i = 0; i < hUniDrcConfig->downmixInstructionsCountV0; i++) {
    DOWNMIX_INSTRUCTIONS tmpDown;
    memset(&tmpDown, 0, sizeof(DOWNMIX_INSTRUCTIONS));
    err = _readDownmixInstructions(hBs, 0, &hUniDrcConfig->channelLayout,
                                   &tmpDown);
    if (err) return err;
    if (i >= 6) continue;
    if (!diff)
      diff |= (memcmp(&tmpDown, &(hUniDrcConfig->downmixInstructions[i]),
                         sizeof(DOWNMIX_INSTRUCTIONS)) != 0);
    hUniDrcConfig->downmixInstructions[i] = tmpDown;
  }

  for (i = 0; i < drcCoefficientsBasicCount; i++) {
    _skipDrcCoefficientsBasic(hBs);
  }
  for (i = 0; i < drcInstructionsBasicCount; i++) {
    _skipDrcInstructionsBasic(hBs);
  }

  hUniDrcConfig->drcCoefficientsUniDrcCount =
      fMin(hUniDrcConfig->drcCoefficientsUniDrcCountV0, (uint8_t)2);
  for (i = 0; i < hUniDrcConfig->drcCoefficientsUniDrcCountV0; i++) {
    DRC_COEFFICIENTS_UNI_DRC_t tmpCoef;
    memset(&tmpCoef, 0, sizeof(DRC_COEFFICIENTS_UNI_DRC_t));
    err = _readDrcCoefficientsUniDrc(hBs, 0, &tmpCoef);
    if (err) return err;
    if (i >= 2) continue;
    if (!diff)
      diff |= (memcmp(&tmpCoef, &(hUniDrcConfig->drcCoefficientsUniDrc[i]),
                         sizeof(DRC_COEFFICIENTS_UNI_DRC_t)) != 0);
    hUniDrcConfig->drcCoefficientsUniDrc[i] = tmpCoef;
  }

  hUniDrcConfig->drcInstructionsUniDrcCount =
      fMin(hUniDrcConfig->drcInstructionsUniDrcCountV0, (uint8_t)12);
  for (i = 0; i < hUniDrcConfig->drcInstructionsUniDrcCountV0; i++) {
    DRC_INSTRUCTIONS_UNI_DRC_t tmpInst;
    memset(&tmpInst, 0, sizeof(DRC_INSTRUCTIONS_UNI_DRC_t));
    err = _readDrcInstructionsUniDrc(hBs, 0, hUniDrcConfig, &tmpInst);
    if (err) return err;
    if (i >= 12) continue;
    if (!diff)
      diff |= (memcmp(&tmpInst, &(hUniDrcConfig->drcInstructionsUniDrc[i]),
                         sizeof(DRC_INSTRUCTIONS_UNI_DRC_t)) != 0);
    hUniDrcConfig->drcInstructionsUniDrc[i] = tmpInst;
  }

  diff |=
      _compAssign(&hUniDrcConfig->uniDrcConfigExtPresent, FDKreadBits(hBs, 1));
  hUniDrcConfig->diff = diff;

  if (hUniDrcConfig->uniDrcConfigExtPresent == 1) {
    err = _readUniDrcConfigExtension(hBs, hUniDrcConfig);
    if (err) return err;
  }

  return err;
}

/*******************/
/* loudnessInfoSet */
/*******************/

static DRC_ERROR _decodeMethodValue(HANDLE_FDK_BITSTREAM hBs,
                                    const uint8_t methodDefinition,
                                    int32_t* methodValue, int32_t isBox) {
  int32_t tmp;
  int32_t val;
  switch (methodDefinition) {
    case MD_UNKNOWN_OTHER:
    case MD_PROGRAM_LOUDNESS:
    case MD_ANCHOR_LOUDNESS:
    case MD_MAX_OF_LOUDNESS_RANGE:
    case MD_MOMENTARY_LOUDNESS_MAX:
    case MD_SHORT_TERM_LOUDNESS_MAX:
      tmp = FDKreadBits(hBs, 8);
      val = FL2FXCONST_DBL(-57.75f / (float)(1 << 7)) +
            (int32_t)(
                tmp << (DFRACT_BITS - 1 - 2 - 7)); /* -57.75 + tmp * 0.25; */
      break;
    case MD_LOUDNESS_RANGE:
      tmp = FDKreadBits(hBs, 8);
      if (tmp == 0)
        val = (int32_t)0;
      else if (tmp <= 128)
        val = (int32_t)(tmp << (DFRACT_BITS - 1 - 2 - 7)); /* tmp * 0.25; */
      else if (tmp <= 204) {
        val = (int32_t)(tmp << (DFRACT_BITS - 1 - 1 - 7)) -
              FL2FXCONST_DBL(32.0f / (float)(1 << 7)); /* 0.5 * tmp - 32.0f; */
      } else {
        /* downscale by 1 more bit to prevent overflow at intermediate result */
        val = (int32_t)(tmp << (DFRACT_BITS - 1 - 8)) -
              FL2FXCONST_DBL(134.0f / (float)(1 << 8)); /* tmp - 134.0; */
        val <<= 1;
      }
      break;
    case MD_MIXING_LEVEL:
      tmp = FDKreadBits(hBs, isBox ? 8 : 5);
      val = (int32_t)(tmp << (DFRACT_BITS - 1 - 7)) +
            FL2FXCONST_DBL(80.0f / (float)(1 << 7)); /* tmp + 80.0; */
      break;
    case MD_ROOM_TYPE:
      tmp = FDKreadBits(hBs, isBox ? 8 : 2);
      val = (int32_t)(tmp << (DFRACT_BITS - 1 - 7)); /* tmp; */
      break;
    case MD_SHORT_TERM_LOUDNESS:
      tmp = FDKreadBits(hBs, 8);
      val = FL2FXCONST_DBL(-116.0f / (float)(1 << 7)) +
            (int32_t)(
                tmp << (DFRACT_BITS - 1 - 1 - 7)); /* -116.0 + tmp * 0.5; */
      break;
    default:
      return DE_NOT_OK; /* invalid methodDefinition value */
  }
  *methodValue = val;
  return DE_OK;
}

static DRC_ERROR _readLoudnessMeasurement(HANDLE_FDK_BITSTREAM hBs,
                                          LOUDNESS_MEASUREMENT* pMeas) {
  DRC_ERROR err = DE_OK;

  pMeas->methodDefinition = FDKreadBits(hBs, 4);
  err =
      _decodeMethodValue(hBs, pMeas->methodDefinition, &pMeas->methodValue, 0);
  if (err) return err;
  pMeas->measurementSystem = FDKreadBits(hBs, 4);
  pMeas->reliability = FDKreadBits(hBs, 2);

  return err;
}

static DRC_ERROR _readLoudnessInfo(HANDLE_FDK_BITSTREAM hBs, const int32_t version,
                                   LOUDNESS_INFO* loudnessInfo) {
  DRC_ERROR err = DE_OK;
  int32_t bsSamplePeakLevel, bsTruePeakLevel, i;
  int32_t measurementCount;

  loudnessInfo->drcSetId = FDKreadBits(hBs, 6);
  if (version >= 1) {
    loudnessInfo->eqSetId = FDKreadBits(hBs, 6);
  } else {
    loudnessInfo->eqSetId = 0;
  }
  loudnessInfo->downmixId = FDKreadBits(hBs, 7);

  loudnessInfo->samplePeakLevelPresent = FDKreadBits(hBs, 1);
  if (loudnessInfo->samplePeakLevelPresent) {
    bsSamplePeakLevel = FDKreadBits(hBs, 12);
    if (bsSamplePeakLevel == 0) {
      loudnessInfo->samplePeakLevelPresent = 0;
      loudnessInfo->samplePeakLevel = (int32_t)0;
    } else { /* 20.0 - bsSamplePeakLevel * 0.03125; */
      loudnessInfo->samplePeakLevel =
          FL2FXCONST_DBL(20.0f / (float)(1 << 7)) -
          (int32_t)(bsSamplePeakLevel << (DFRACT_BITS - 1 - 5 - 7));
    }
  }

  loudnessInfo->truePeakLevelPresent = FDKreadBits(hBs, 1);
  if (loudnessInfo->truePeakLevelPresent) {
    bsTruePeakLevel = FDKreadBits(hBs, 12);
    if (bsTruePeakLevel == 0) {
      loudnessInfo->truePeakLevelPresent = 0;
      loudnessInfo->truePeakLevel = (int32_t)0;
    } else {
      loudnessInfo->truePeakLevel =
          FL2FXCONST_DBL(20.0f / (float)(1 << 7)) -
          (int32_t)(bsTruePeakLevel << (DFRACT_BITS - 1 - 5 - 7));
    }
    loudnessInfo->truePeakLevelMeasurementSystem = FDKreadBits(hBs, 4);
    loudnessInfo->truePeakLevelReliability = FDKreadBits(hBs, 2);
  }

  measurementCount = FDKreadBits(hBs, 4);
  loudnessInfo->measurementCount = fMin(measurementCount, 8);
  for (i = 0; i < measurementCount; i++) {
    LOUDNESS_MEASUREMENT tmpMeas;
    memset(&tmpMeas, 0, sizeof(LOUDNESS_MEASUREMENT));
    err = _readLoudnessMeasurement(hBs, &tmpMeas);
    if (err) return err;
    if (i >= 8) continue;
    loudnessInfo->loudnessMeasurement[i] = tmpMeas;
  }

  return err;
}

static DRC_ERROR _readLoudnessInfoSetExtEq(
    HANDLE_FDK_BITSTREAM hBs, HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet) {
  DRC_ERROR err = DE_OK;
  int32_t i, offset;
  int32_t diff = hLoudnessInfoSet->diff;

  diff |= _compAssign(&hLoudnessInfoSet->loudnessInfoAlbumCountV1,
                      FDKreadBits(hBs, 6));
  diff |=
      _compAssign(&hLoudnessInfoSet->loudnessInfoCountV1, FDKreadBits(hBs, 6));

  offset = hLoudnessInfoSet->loudnessInfoAlbumCountV0;
  hLoudnessInfoSet->loudnessInfoAlbumCount = fMin(
      (uint8_t)(offset + hLoudnessInfoSet->loudnessInfoAlbumCountV1), (uint8_t)12);
  for (i = 0; i < hLoudnessInfoSet->loudnessInfoAlbumCountV1; i++) {
    LOUDNESS_INFO tmpLoud;
    memset(&tmpLoud, 0, sizeof(LOUDNESS_INFO));
    err = _readLoudnessInfo(hBs, 1, &tmpLoud);
    if (err) return err;
    if ((offset + i) >= 12) continue;
    if (!diff)
      diff |= (memcmp(&tmpLoud,
                         &(hLoudnessInfoSet->loudnessInfoAlbum[offset + i]),
                         sizeof(LOUDNESS_INFO)) != 0);
    hLoudnessInfoSet->loudnessInfoAlbum[offset + i] = tmpLoud;
  }

  offset = hLoudnessInfoSet->loudnessInfoCountV0;
  hLoudnessInfoSet->loudnessInfoCount =
      fMin((uint8_t)(offset + hLoudnessInfoSet->loudnessInfoCountV1), (uint8_t)12);
  for (i = 0; i < hLoudnessInfoSet->loudnessInfoCountV1; i++) {
    LOUDNESS_INFO tmpLoud;
    memset(&tmpLoud, 0, sizeof(LOUDNESS_INFO));
    err = _readLoudnessInfo(hBs, 1, &tmpLoud);
    if (err) return err;
    if ((offset + i) >= 12) continue;
    if (!diff)
      diff |=
          (memcmp(&tmpLoud, &(hLoudnessInfoSet->loudnessInfo[offset + i]),
                     sizeof(LOUDNESS_INFO)) != 0);
    hLoudnessInfoSet->loudnessInfo[offset + i] = tmpLoud;
  }
  hLoudnessInfoSet->diff = diff;
  return err;
}

static DRC_ERROR _readLoudnessInfoSetExtension(
    HANDLE_FDK_BITSTREAM hBs, HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet) {
  DRC_ERROR err = DE_OK;
  int32_t k, bitSizeLen, extSizeBits, bitSize;
  int32_t nBitsRemaining;
  LOUDNESS_INFO_SET_EXTENSION* pExt = &(hLoudnessInfoSet->loudnessInfoSetExt);

  k = 0;
  pExt->loudnessInfoSetExtType[k] = FDKreadBits(hBs, 4);
  while (pExt->loudnessInfoSetExtType[k] != UNIDRCLOUDEXT_TERM) {
    if (k >= (8 - 1)) return DE_MEMORY_ERROR;
    bitSizeLen = FDKreadBits(hBs, 4);
    extSizeBits = bitSizeLen + 4;

    bitSize = FDKreadBits(hBs, extSizeBits);
    pExt->extBitSize[k] = bitSize + 1;
    nBitsRemaining = (int32_t)FDKgetValidBits(hBs);

    switch (pExt->loudnessInfoSetExtType[k]) {
      case UNIDRCLOUDEXT_EQ:
        err = _readLoudnessInfoSetExtEq(hBs, hLoudnessInfoSet);
        if (err) return err;
        if (nBitsRemaining !=
            ((int32_t)pExt->extBitSize[k] + (int32_t)FDKgetValidBits(hBs)))
          return DE_NOT_OK;
        break;
      /* add future extensions here */
      default:
        FDKpushFor(hBs, pExt->extBitSize[k]);
        break;
    }
    k++;
    pExt->loudnessInfoSetExtType[k] = FDKreadBits(hBs, 4);
  }

  return err;
}

/* Parser for loundessInfoSet() */
DRC_ERROR
drcDec_readLoudnessInfoSet(HANDLE_FDK_BITSTREAM hBs,
                           HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet) {
  DRC_ERROR err = DE_OK;
  int32_t i, diff = 0;
  if (hLoudnessInfoSet == NULL) return DE_NOT_OK;

  diff |= _compAssign(&hLoudnessInfoSet->loudnessInfoAlbumCountV0,
                      FDKreadBits(hBs, 6));
  diff |=
      _compAssign(&hLoudnessInfoSet->loudnessInfoCountV0, FDKreadBits(hBs, 6));

  hLoudnessInfoSet->loudnessInfoAlbumCount =
      fMin(hLoudnessInfoSet->loudnessInfoAlbumCountV0, (uint8_t)12);
  for (i = 0; i < hLoudnessInfoSet->loudnessInfoAlbumCountV0; i++) {
    LOUDNESS_INFO tmpLoud;
    memset(&tmpLoud, 0, sizeof(LOUDNESS_INFO));
    err = _readLoudnessInfo(hBs, 0, &tmpLoud);
    if (err) return err;
    if (i >= 12) continue;
    if (!diff)
      diff |= (memcmp(&tmpLoud, &(hLoudnessInfoSet->loudnessInfoAlbum[i]),
                         sizeof(LOUDNESS_INFO)) != 0);
    hLoudnessInfoSet->loudnessInfoAlbum[i] = tmpLoud;
  }

  hLoudnessInfoSet->loudnessInfoCount =
      fMin(hLoudnessInfoSet->loudnessInfoCountV0, (uint8_t)12);
  for (i = 0; i < hLoudnessInfoSet->loudnessInfoCountV0; i++) {
    LOUDNESS_INFO tmpLoud;
    memset(&tmpLoud, 0, sizeof(LOUDNESS_INFO));
    err = _readLoudnessInfo(hBs, 0, &tmpLoud);
    if (err) return err;
    if (i >= 12) continue;
    if (!diff)
      diff |= (memcmp(&tmpLoud, &(hLoudnessInfoSet->loudnessInfo[i]),
                         sizeof(LOUDNESS_INFO)) != 0);
    hLoudnessInfoSet->loudnessInfo[i] = tmpLoud;
  }

  diff |= _compAssign(&hLoudnessInfoSet->loudnessInfoSetExtPresent,
                      FDKreadBits(hBs, 1));
  hLoudnessInfoSet->diff = diff;

  if (hLoudnessInfoSet->loudnessInfoSetExtPresent) {
    err = _readLoudnessInfoSetExtension(hBs, hLoudnessInfoSet);
    if (err) return err;
  }

  return err;
}
