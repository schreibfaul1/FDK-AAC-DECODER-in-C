

/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "drcDec_types.h"
#include "drcDec_gainDecoder.h"
#include "drcGainDec_preprocess.h"
#include "drcDec_tools.h"
#include "../libFDK/FDK_matrixCalloc.h"
#include "drcDec_rom.h"

#define SLOPE_FACTOR_DB_TO_LINEAR \
  FL2FXCONST_DBL(0.1151f * (float)(1 << 3)) /* ln(10) / 20 */

typedef struct {
  int32_t drcSetEffect;
  DUCKING_MODIFICATION_t* pDMod;
  GAIN_MODIFICATION_t* pGMod;
  int32_t drcCharacteristicPresent;
  CHARACTERISTIC_FORMAT_t characteristicFormatSource[2];
  const CUSTOM_DRC_CHAR_t* pCCharSource[2];
  CHARACTERISTIC_FORMAT_t characteristicFormatTarget[2];
  const CUSTOM_DRC_CHAR_t* pCCharTarget[2];
  int32_t slopeIsNegative;
  int32_t limiterPeakTargetPresent;
  int16_t limiterPeakTarget;
  int32_t loudnessNormalizationGainDb;
  int16_t compress;
  int16_t boost;
} NODE_MODIFICATION;

static DRC_ERROR _getCicpCharacteristic(
    const int32_t cicpCharacteristic,
    CHARACTERISTIC_FORMAT_t pCharacteristicFormat[2],
    const CUSTOM_DRC_CHAR_t* pCCharSource[2]) {
  if ((cicpCharacteristic < 1) || (cicpCharacteristic > 11)) {
    return DE_NOT_OK;
  }

  if (cicpCharacteristic < 7) { /* sigmoid characteristic */
    pCharacteristicFormat[CS_LEFT] = CF_SIGMOID;
    pCCharSource[CS_LEFT] =
        (const CUSTOM_DRC_CHAR_t*)(&cicpDrcCharSigmoidLeft[cicpCharacteristic -
                                                         1]);
    pCharacteristicFormat[CS_RIGHT] = CF_SIGMOID;
    pCCharSource[CS_RIGHT] =
        (const CUSTOM_DRC_CHAR_t*)(&cicpDrcCharSigmoidRight[cicpCharacteristic -
                                                          1]);
  } else { /* nodes characteristic */
    pCharacteristicFormat[CS_LEFT] = CF_NODES;
    pCCharSource[CS_LEFT] =
        (const CUSTOM_DRC_CHAR_t*)(&cicpDrcCharNodesLeft[cicpCharacteristic - 7]);
    pCharacteristicFormat[CS_RIGHT] = CF_NODES;
    pCCharSource[CS_RIGHT] =
        (const CUSTOM_DRC_CHAR_t*)(&cicpDrcCharNodesRight[cicpCharacteristic -
                                                        7]);
  }
  return DE_OK;
}

static int32_t _getSign(int16_t in) {
  if (in > (int32_t)0) return 1;
  if (in < (int32_t)0) return -1;
  return 0;
}

static DRC_ERROR _getSlopeSign(const CHARACTERISTIC_FORMAT_t drcCharFormat,
                               const CUSTOM_DRC_CHAR_t* pCChar, int32_t* pSlopeSign) {
  if (drcCharFormat == CF_SIGMOID) {
    *pSlopeSign = (pCChar->sigmoid.flipSign ? 1 : -1);
  } else {
    int32_t k, slopeSign = 0, tmp_slopeSign;
    for (k = 0; k < pCChar->nodes.characteristicNodeCount; k++) {
      if (pCChar->nodes.nodeLevel[k + 1] > pCChar->nodes.nodeLevel[k]) {
        tmp_slopeSign =
            _getSign(pCChar->nodes.nodeGain[k + 1] - pCChar->nodes.nodeGain[k]);
      } else {
        tmp_slopeSign = -_getSign(pCChar->nodes.nodeGain[k + 1] -
                                  pCChar->nodes.nodeGain[k]);
      }
      if ((slopeSign || tmp_slopeSign) && (slopeSign == -tmp_slopeSign))
        return DE_NOT_OK; /* DRC characteristic is not invertible */
      else
        slopeSign = tmp_slopeSign;
    }
    *pSlopeSign = slopeSign;
  }
  return DE_OK;
}

static DRC_ERROR _isSlopeNegative(const CHARACTERISTIC_FORMAT_t drcCharFormat[2],
                                  const CUSTOM_DRC_CHAR_t* pCChar[2],
                                  int32_t* pSlopeIsNegative) {
  DRC_ERROR err = DE_OK;
  int32_t slopeSign[2] = {0, 0};

  err = _getSlopeSign(drcCharFormat[CS_LEFT], pCChar[CS_LEFT],
                      &slopeSign[CS_LEFT]);
  if (err) return err;

  err = _getSlopeSign(drcCharFormat[CS_RIGHT], pCChar[CS_RIGHT],
                      &slopeSign[CS_RIGHT]);
  if (err) return err;

  if ((slopeSign[CS_LEFT] || slopeSign[CS_RIGHT]) &&
      (slopeSign[CS_LEFT] == -slopeSign[CS_RIGHT]))
    return DE_NOT_OK; /* DRC characteristic is not invertible */

  *pSlopeIsNegative = (slopeSign[CS_LEFT] < 0);
  return DE_OK;
}

static DRC_ERROR _prepareDrcCharacteristic(const DRC_CHARACTERISTIC_t* pDChar,
                                           DRC_COEFFICIENTS_UNI_DRC_t* pCoef,
                                           const int32_t b,
                                           NODE_MODIFICATION* pNodeMod) {
  DRC_ERROR err = DE_OK;
  pNodeMod->drcCharacteristicPresent = pDChar->present;
  if (pNodeMod->drcCharacteristicPresent) {
    if (pDChar->isCICP == 1) {
      err = _getCicpCharacteristic(pDChar->cicpIndex,
                                   pNodeMod->characteristicFormatSource,
                                   pNodeMod->pCCharSource);
      if (err) return err;
    } else {
      pNodeMod->characteristicFormatSource[CS_LEFT] =
          (CHARACTERISTIC_FORMAT_t)
              pCoef->characteristicLeftFormat[pDChar->custom.left];
      pNodeMod->pCCharSource[CS_LEFT] =
          &(pCoef->customCharacteristicLeft[pDChar->custom.left]);
      pNodeMod->characteristicFormatSource[CS_RIGHT] =
          (CHARACTERISTIC_FORMAT_t)
              pCoef->characteristicRightFormat[pDChar->custom.right];
      pNodeMod->pCCharSource[CS_RIGHT] =
          &(pCoef->customCharacteristicRight[pDChar->custom.right]);
    }
    err = _isSlopeNegative(pNodeMod->characteristicFormatSource,
                           pNodeMod->pCCharSource, &pNodeMod->slopeIsNegative);
    if (err) return err;

    if (pNodeMod->pGMod != NULL) {
      if (pNodeMod->pGMod[b].targetCharacteristicLeftPresent) {
        pNodeMod->characteristicFormatTarget[CS_LEFT] =
            (CHARACTERISTIC_FORMAT_t)pCoef->characteristicLeftFormat
                [pNodeMod->pGMod[b].targetCharacteristicLeftIndex];
        pNodeMod->pCCharTarget[CS_LEFT] =
            &(pCoef->customCharacteristicLeft
                  [pNodeMod->pGMod[b].targetCharacteristicLeftIndex]);
      }
      if (pNodeMod->pGMod[b].targetCharacteristicRightPresent) {
        pNodeMod->characteristicFormatTarget[CS_RIGHT] =
            (CHARACTERISTIC_FORMAT_t)pCoef->characteristicRightFormat
                [pNodeMod->pGMod[b].targetCharacteristicRightIndex];
        pNodeMod->pCCharTarget[CS_RIGHT] =
            &(pCoef->customCharacteristicRight
                  [pNodeMod->pGMod[b].targetCharacteristicRightIndex]);
      }
    }
  }
  return DE_OK;
}

static DRC_ERROR _compressorIO_sigmoid_common(
    const int32_t tmp,               /* e = 7 */
    const int32_t gainDbLimit,       /* e = 6 */
    const int32_t exp,               /* e = 5 */
    const int32_t inverse, int32_t* out) /* e = 7 */
{
  int32_t x, tmp1, tmp2, invExp, denom;
  int32_t e_x, e_tmp1, e_tmp2, e_invExp, e_denom, e_out;

  if (exp < FL2FXCONST_DBL(1.0f / (float)(1 << 5))) {
    return DE_NOT_OK;
  }

  /* x = tmp / gainDbLimit; */
  x = fDivNormSigned(tmp, gainDbLimit, &e_x);
  e_x += 7 - 6;
  if (x < (int32_t)0) {
    return DE_NOT_OK;
  }

  /* out = tmp / pow(1.0f +/- pow(x, exp), 1.0f/exp); */
  tmp1 = fPow(x, e_x, exp, 5, &e_tmp1);
  if (inverse) tmp1 = -tmp1;
  tmp2 = fAddNorm(FL2FXCONST_DBL(1.0f / (float)(1 << 1)), 1, tmp1, e_tmp1,
                  &e_tmp2);
  invExp = fDivNorm(FL2FXCONST_DBL(1.0f / (float)(1 << 1)), exp, &e_invExp);
  e_invExp += 1 - 5;
  if (tmp2 < (int32_t)0) {
    return DE_NOT_OK;
  }
  denom = fPow(tmp2, e_tmp2, invExp, e_invExp, &e_denom);
  *out = fDivNormSigned(tmp, denom, &e_out);
  e_out += 7 - e_denom;
  *out = scaleValueSaturate(*out, e_out - 7);
  return DE_OK;
}

static DRC_ERROR _compressorIO_sigmoid(const CUSTOM_DRC_CHAR_SIGMOID_t* pCChar,
                                       const int32_t inLevelDb, /* e = 7 */
                                       int32_t* outGainDb)      /* e = 7 */
{
  int32_t tmp;
  int16_t exp = pCChar->exp;
  DRC_ERROR err = DE_OK;

  tmp = fMultDiv2((DRC_INPUT_LOUDNESS_TARGET >> 1) - (inLevelDb >> 1),
                  pCChar->ioRatio);
  tmp = SATURATE_LEFT_SHIFT(tmp, 2 + 1 + 1, DFRACT_BITS);
  if (exp < (int16_t)MAXVAL_SGL) {
    /* x = tmp / gainDbLimit; */
    /* *outGainDb = tmp / pow(1.0f + pow(x, exp), 1.0f/exp); */
    err = _compressorIO_sigmoid_common(tmp, FX_SGL2FX_DBL(pCChar->gain),
                                       FX_SGL2FX_DBL(exp), 0, outGainDb);
    if (err) return err;
  } else {
    *outGainDb =
        tmp; /* scaling of outGainDb (7) is equal to scaling of tmp (7) */
  }
  if (pCChar->flipSign == 1) {
    *outGainDb = -*outGainDb;
  }
  return err;
}

static DRC_ERROR _compressorIO_sigmoid_inverse(
    const CUSTOM_DRC_CHAR_SIGMOID_t* pCChar, const int16_t gainDb,
    int32_t* inLev) {
  DRC_ERROR err = DE_OK;
  int16_t ioRatio = pCChar->ioRatio;
  int16_t exp = pCChar->exp;
  int32_t tmp = FX_SGL2FX_DBL(gainDb), tmp_out;
  int32_t e_out;

  if (pCChar->flipSign == 1) {
    tmp = -tmp;
  }
  if (exp < (int16_t)MAXVAL_SGL) {
    /* x = tmp / gainDbLimit; */
    /* tmp = tmp / pow(1.0f - pow(x, exp), 1.0f / exp); */
    err = _compressorIO_sigmoid_common(tmp, FX_SGL2FX_DBL(pCChar->gain),
                                       FX_SGL2FX_DBL(exp), 1, &tmp);
    if (err) return err;
  }
  if (ioRatio == (int16_t)0) {
    return DE_NOT_OK;
  }
  tmp_out = fDivNormSigned(tmp, FX_SGL2FX_DBL(ioRatio), &e_out);
  e_out += 7 - 2;
  tmp_out = fAddNorm(DRC_INPUT_LOUDNESS_TARGET, 7, -tmp_out, e_out, &e_out);
  *inLev = scaleValueSaturate(tmp_out, e_out - 7);

  return err;
}

static DRC_ERROR _compressorIO_nodes(const CUSTOM_DRC_CHAR_NODES_t* pCChar,
                                     const int32_t inLevelDb, /* e = 7 */
                                     int32_t* outGainDb)      /* e = 7 */
{
  int32_t n;
  int32_t w;
  const int16_t* nodeLevel = pCChar->nodeLevel;
  const int16_t* nodeGain = pCChar->nodeGain;

  if (inLevelDb < DRC_INPUT_LOUDNESS_TARGET) {
    for (n = 0; n < pCChar->characteristicNodeCount; n++) {
      if ((inLevelDb <= FX_SGL2FX_DBL(nodeLevel[n])) &&
          (inLevelDb > FX_SGL2FX_DBL(nodeLevel[n + 1]))) {
        w = fDivNorm(inLevelDb - FX_SGL2FX_DBL(nodeLevel[n + 1]),
                     FX_SGL2FX_DBL(nodeLevel[n] - nodeLevel[n + 1]));
        *outGainDb = fMult(w, nodeGain[n]) +
                     fMult((int32_t)MAXVAL_DBL - w, nodeGain[n + 1]);
        /* *outGainDb = (w * nodeGain[n] + (1.0-w) * nodeGain[n+1]); */
        return DE_OK;
      }
    }
  } else {
    for (n = 0; n < pCChar->characteristicNodeCount; n++) {
      if ((inLevelDb >= FX_SGL2FX_DBL(nodeLevel[n])) &&
          (inLevelDb < FX_SGL2FX_DBL(nodeLevel[n + 1]))) {
        w = fDivNorm(FX_SGL2FX_DBL(nodeLevel[n + 1]) - inLevelDb,
                     FX_SGL2FX_DBL(nodeLevel[n + 1] - nodeLevel[n]));
        *outGainDb = fMult(w, nodeGain[n]) +
                     fMult((int32_t)MAXVAL_DBL - w, nodeGain[n + 1]);
        /* *outGainDb = (w * nodeGain[n] + (1.0-w) * nodeGain[n+1]); */
        return DE_OK;
      }
    }
  }
  *outGainDb = FX_SGL2FX_DBL(nodeGain[pCChar->characteristicNodeCount]);
  return DE_OK;
}

static DRC_ERROR _compressorIO_nodes_inverse(
    const CUSTOM_DRC_CHAR_NODES_t* pCChar, const int16_t gainDb, /* e = 7 */
    int32_t* inLev)                                            /* e = 7 */
{
  int32_t n;
  int32_t k;
  int32_t w;
  int32_t gainIsNegative = 0;
  const int16_t* nodeLevel = pCChar->nodeLevel;
  const int16_t* nodeGain = pCChar->nodeGain;
  int32_t nodeCount = pCChar->characteristicNodeCount;
  for (k = 0; k < nodeCount; k++) {
    if (pCChar->nodeGain[k + 1] < (int16_t)0) {
      gainIsNegative = 1;
    }
  }
  if (gainIsNegative == 1) {
    if (gainDb <= nodeGain[nodeCount]) {
      *inLev = FX_SGL2FX_DBL(nodeLevel[nodeCount]);
    } else {
      if (gainDb >= (int16_t)0) {
        *inLev = DRC_INPUT_LOUDNESS_TARGET;
      } else {
        for (n = 0; n < nodeCount; n++) {
          if ((gainDb <= nodeGain[n]) && (gainDb > nodeGain[n + 1])) {
            int16_t gainDelta = nodeGain[n] - nodeGain[n + 1];
            if (gainDelta == (int16_t)0) {
              *inLev = FX_SGL2FX_DBL(nodeLevel[n]);
              return DE_OK;
            }
            w = fDivNorm(gainDb - nodeGain[n + 1], gainDelta);
            *inLev = fMult(w, nodeLevel[n]) +
                     fMult((int32_t)MAXVAL_DBL - w, nodeLevel[n + 1]);
            /* *inLev = (w * nodeLevel[n] + (1.0-w) * nodeLevel[n+1]); */
            return DE_OK;
          }
        }
        *inLev = FX_SGL2FX_DBL(nodeLevel[nodeCount]);
      }
    }
  } else {
    if (gainDb >= nodeGain[nodeCount]) {
      *inLev = FX_SGL2FX_DBL(nodeLevel[nodeCount]);
    } else {
      if (gainDb <= (int16_t)0) {
        *inLev = DRC_INPUT_LOUDNESS_TARGET;
      } else {
        for (n = 0; n < nodeCount; n++) {
          if ((gainDb >= nodeGain[n]) && (gainDb < nodeGain[n + 1])) {
            int16_t gainDelta = nodeGain[n + 1] - nodeGain[n];
            if (gainDelta == (int16_t)0) {
              *inLev = FX_SGL2FX_DBL(nodeLevel[n]);
              return DE_OK;
            }
            w = fDivNorm(nodeGain[n + 1] - gainDb, gainDelta);
            *inLev = fMult(w, nodeLevel[n]) +
                     fMult((int32_t)MAXVAL_DBL - w, nodeLevel[n + 1]);
            /* *inLev = (w * nodeLevel[n] + (1.0-w) * nodeLevel[n+1]); */
            return DE_OK;
          }
        }
        *inLev = FX_SGL2FX_DBL(nodeLevel[nodeCount]);
      }
    }
  }
  return DE_OK;
}

static DRC_ERROR _mapGain(const CHARACTERISTIC_FORMAT_t pCCharFormatSource,
                          const CUSTOM_DRC_CHAR_t* pCCharSource,
                          const CHARACTERISTIC_FORMAT_t pCCharFormatTarget,
                          const CUSTOM_DRC_CHAR_t* pCCharTarget,
                          const int16_t gainInDb, /* e = 7 */
                          int32_t* gainOutDb)     /* e = 7 */
{
  int32_t inLevel = (int32_t)0;
  DRC_ERROR err = DE_OK;

  switch (pCCharFormatSource) {
    case CF_SIGMOID:
      err = _compressorIO_sigmoid_inverse(
          (const CUSTOM_DRC_CHAR_SIGMOID_t*)pCCharSource, gainInDb, &inLevel);
      if (err) return err;
      break;
    case CF_NODES:
      err = _compressorIO_nodes_inverse(
          (const CUSTOM_DRC_CHAR_NODES_t*)pCCharSource, gainInDb, &inLevel);
      if (err) return err;
      break;
    default:
      return DE_NOT_OK;
  }
  switch (pCCharFormatTarget) {
    case CF_SIGMOID:
      err = _compressorIO_sigmoid((const CUSTOM_DRC_CHAR_SIGMOID_t*)pCCharTarget,
                                  inLevel, gainOutDb);
      if (err) return err;
      break;
    case CF_NODES:
      err = _compressorIO_nodes((const CUSTOM_DRC_CHAR_NODES_t*)pCCharTarget,
                                inLevel, gainOutDb);
      if (err) return err;
      break;
    default:
      break;
  }
  return DE_OK;
}

static DRC_ERROR _toLinear(
    const NODE_MODIFICATION* nodeMod, const int32_t drcBand,
    const int16_t gainDb,  /* in: gain value in dB, e = 7 */
    const int16_t slopeDb, /* in: slope value in dB/deltaTmin, e = 2 */
    int32_t* gainLin,      /* out: linear gain value, e = 7 */
    int32_t* slopeLin)     /* out: linear slope value, e = 7 */
{
  int32_t gainRatio_m = FL2FXCONST_DBL(1.0f / (float)(1 << 1));
  GAIN_MODIFICATION_t* pGMod = NULL;
  DUCKING_MODIFICATION_t* pDMod = nodeMod->pDMod;
  int32_t tmp_dbl, gainDb_modified, gainDb_offset, gainDb_out, gainLin_m,
      slopeLin_m;
  int32_t gainLin_e, gainRatio_e = 1, gainDb_out_e;
  if (nodeMod->pGMod != NULL) {
    pGMod = &(nodeMod->pGMod[drcBand]);
  }
  if (((nodeMod->drcSetEffect & (EB_DUCK_OTHER | EB_DUCK_SELF)) == 0) &&
      (nodeMod->drcSetEffect != EB_FADE) &&
      (nodeMod->drcSetEffect != EB_CLIPPING)) {
    DRC_ERROR err = DE_OK;
    int32_t gainDbMapped;

    if ((pGMod != NULL) && (nodeMod->drcCharacteristicPresent)) {
      if (((gainDb > (int16_t)0) && nodeMod->slopeIsNegative) ||
          ((gainDb < (int16_t)0) && !nodeMod->slopeIsNegative)) {
        /* left side */
        if (pGMod->targetCharacteristicLeftPresent == 1) {
          err = _mapGain(nodeMod->characteristicFormatSource[CS_LEFT],
                         nodeMod->pCCharSource[CS_LEFT],
                         nodeMod->characteristicFormatTarget[CS_LEFT],
                         nodeMod->pCCharTarget[CS_LEFT], gainDb, &gainDbMapped);
          if (err) return err;
          gainRatio_m = fDivNormSigned(
              gainDbMapped, FX_SGL2FX_DBL(gainDb),
              &gainRatio_e); /* target characteristic in payload */
        }
      }

      else { /* if (((gainDb < (int16_t)0) && nodeMod->slopeIsNegative) ||
                ((gainDb > (int16_t)0) && !nodeMod->slopeIsNegative)) */

        /* right side */
        if (pGMod->targetCharacteristicRightPresent == 1) {
          err =
              _mapGain(nodeMod->characteristicFormatSource[CS_RIGHT],
                       nodeMod->pCCharSource[CS_RIGHT],
                       nodeMod->characteristicFormatTarget[CS_RIGHT],
                       nodeMod->pCCharTarget[CS_RIGHT], gainDb, &gainDbMapped);
          if (err) return err;
          gainRatio_m = fDivNormSigned(
              gainDbMapped, FX_SGL2FX_DBL(gainDb),
              &gainRatio_e); /* target characteristic in payload */
        }
      }
    }
    if (gainDb < (int16_t)0) {
      gainRatio_m = fMultDiv2(gainRatio_m, nodeMod->compress);
    } else {
      gainRatio_m = fMultDiv2(gainRatio_m, nodeMod->boost);
    }
    gainRatio_e += 2;
  }
  if ((pGMod != NULL) && (pGMod->gainScalingPresent == 1)) {
    if (gainDb < (int16_t)0) {
      gainRatio_m = fMultDiv2(gainRatio_m, pGMod->attenuationScaling);
    } else {
      gainRatio_m = fMultDiv2(gainRatio_m, pGMod->amplificationScaling);
    }
    gainRatio_e += 3;
  }
  if ((pDMod != NULL) &&
      (nodeMod->drcSetEffect & (EB_DUCK_OTHER | EB_DUCK_SELF)) &&
      (pDMod->duckingScalingPresent == 1)) {
    gainRatio_m = fMultDiv2(gainRatio_m, pDMod->duckingScaling);
    gainRatio_e += 3;
  }

  gainDb_modified =
      fMultDiv2(gainDb, gainRatio_m); /* resulting e: 7 + gainRatio_e + 1*/
  gainDb_offset = (int32_t)0;

  if ((pGMod != NULL) && (pGMod->gainOffsetPresent == 1)) {
    /* *gainLin *= (float)pow(2.0, (double)(pGMod->gainOffset/6.0f)); */
    gainDb_offset += FX_SGL2FX_DBL(pGMod->gainOffset) >> 4; /* resulting e: 8 */
  }
  if ((nodeMod->limiterPeakTargetPresent == 1) &&
      (nodeMod->drcSetEffect ==
       EB_CLIPPING)) { /* The only drcSetEffect is "clipping prevention" */
    /* loudnessNormalizationGainModificationDb is included in
     * loudnessNormalizationGainDb */
    /* *gainLin *= (float)pow(2.0, max(0.0, -nodeModification->limiterPeakTarget
     * - nodeModification->loudnessNormalizationGainDb)/6.0); */
    gainDb_offset += fMax(
        (int32_t)0,
        (FX_SGL2FX_DBL(-nodeMod->limiterPeakTarget) >> 3) -
            (nodeMod->loudnessNormalizationGainDb >> 1)); /* resulting e: 8 */
  }
  if (gainDb_offset != (int32_t)0) {
    gainDb_out = fAddNorm(gainDb_modified, 7 + gainRatio_e + 1, gainDb_offset,
                          8, &gainDb_out_e);
  } else {
    gainDb_out = gainDb_modified;
    gainDb_out_e = 7 + gainRatio_e + 1;
  }

  /* *gainLin = (float)pow(2.0, (double)(gainDb_modified[1] / 6.0f)); */
  gainLin_m = approxDb2lin(gainDb_out, gainDb_out_e, &gainLin_e);
  *gainLin = scaleValueSaturate(gainLin_m, gainLin_e - 7);

  /* *slopeLin = SLOPE_FACTOR_DB_TO_LINEAR * gainRatio * *gainLin * slopeDb; */
  if (slopeDb == (int16_t)0) {
    *slopeLin = (int32_t)0;
  } else {
    tmp_dbl =
        fMult(slopeDb, SLOPE_FACTOR_DB_TO_LINEAR); /* resulting e: 2 - 3 = -1 */
    tmp_dbl = fMult(tmp_dbl, gainRatio_m); /* resulting e: -1 + gainRatio_e */
    if (gainDb_offset !=
        (int32_t)0) { /* recalculate gainLin from gainDb that wasn't modified
                          by gainOffset and limiterPeakTarget */
      gainLin_m = approxDb2lin(gainDb_modified, 7 + gainRatio_e, &gainLin_e);
    }
    slopeLin_m = fMult(tmp_dbl, gainLin_m);
    *slopeLin =
        scaleValueSaturate(slopeLin_m, -1 + gainRatio_e + gainLin_e - 7);
  }

  if ((nodeMod->limiterPeakTargetPresent == 1) &&
      (nodeMod->drcSetEffect == EB_CLIPPING)) {
    if (*gainLin >= FL2FXCONST_DBL(1.0f / (float)(1 << 7))) {
      *gainLin = FL2FXCONST_DBL(1.0f / (float)(1 << 7));
      *slopeLin = (int32_t)0;
    }
  }

  return DE_OK;
}

/* prepare buffers containing linear nodes for each gain sequence */
DRC_ERROR
prepareDrcGain(HANDLE_DRC_GAIN_DECODER hGainDec,
               HANDLE_UNI_DRC_GAIN hUniDrcGain, const int16_t compress,
               const int16_t boost, const int32_t loudnessNormalizationGainDb,
               const int32_t activeDrcIndex) {
  int32_t b, g, gainElementIndex;
  DRC_GAIN_BUFFERS* drcGainBuffers = &(hGainDec->drcGainBuffers);
  NODE_MODIFICATION nodeMod;
  memset(&nodeMod, 0, sizeof(NODE_MODIFICATION));
  ACTIVE_DRC* pActiveDrc = &(hGainDec->activeDrc[activeDrcIndex]);
  DRC_INSTRUCTIONS_UNI_DRC_t* pInst = pActiveDrc->pInst;
  if (pInst == NULL) return DE_NOT_OK;

  nodeMod.drcSetEffect = pInst->drcSetEffect;

  nodeMod.compress = compress;
  nodeMod.boost = boost;
  nodeMod.loudnessNormalizationGainDb = loudnessNormalizationGainDb;
  nodeMod.limiterPeakTargetPresent = pInst->limiterPeakTargetPresent;
  nodeMod.limiterPeakTarget = pInst->limiterPeakTarget;

  gainElementIndex = 0;
  for (g = 0; g < pInst->nDrcChannelGroups; g++) {
    int32_t gainSetIndex = 0;
    int32_t nDrcBands = 0;
    DRC_COEFFICIENTS_UNI_DRC_t* pCoef = pActiveDrc->pCoef;
    if (pCoef == NULL) return DE_NOT_OK;

    if (!pActiveDrc->channelGroupIsParametricDrc[g]) {
      gainSetIndex = pInst->gainSetIndexForChannelGroup[g];

      if (nodeMod.drcSetEffect & (EB_DUCK_OTHER | EB_DUCK_SELF)) {
        nodeMod.pDMod = &(pActiveDrc->duckingModificationForChannelGroup[g]);
        nodeMod.pGMod = NULL;
      } else {
        nodeMod.pGMod = pInst->gainModificationForChannelGroup[g];
        nodeMod.pDMod = NULL;
      }

      nDrcBands = pActiveDrc->bandCountForChannelGroup[g];
      for (b = 0; b < nDrcBands; b++) {
        DRC_ERROR err = DE_OK;
        GAIN_SET_t* pGainSet = &(pCoef->gainSet[gainSetIndex]);
        int32_t seq = pGainSet->gainSequenceIndex[b];
        DRC_CHARACTERISTIC_t* pDChar = &(pGainSet->drcCharacteristic[b]);

        /* linearNodeBuffer contains a copy of the gain sequences (consisting of
           nodes) that are relevant for decoding. It also contains gain
           sequences of previous frames. */
        LINEAR_NODE_BUFFER* pLnb =
            &(drcGainBuffers->linearNodeBuffer[pActiveDrc->activeDrcOffset +
                                               gainElementIndex]);
        int32_t i, lnbp;
        lnbp = drcGainBuffers->lnbPointer;
        pLnb->gainInterpolationType =
            (GAIN_INTERPOLATION_TYPE_t)pGainSet->gainInterpolationType;

        err = _prepareDrcCharacteristic(pDChar, pCoef, b, &nodeMod);
        if (err) return err;

        /* copy a node buffer and convert from dB to linear */
        pLnb->nNodes[lnbp] = fMin((int32_t)hUniDrcGain->nNodes[seq], 16);
        for (i = 0; i < pLnb->nNodes[lnbp]; i++) {
          int32_t gainLin, slopeLin;
          err = _toLinear(&nodeMod, b, hUniDrcGain->gainNode[seq][i].gainDb,
                          (int16_t)0, &gainLin, &slopeLin);
          if (err) return err;
          pLnb->linearNode[lnbp][i].gainLin = gainLin;
          pLnb->linearNode[lnbp][i].time = hUniDrcGain->gainNode[seq][i].time;
        }
        gainElementIndex++;
      }
    } else {
      /* parametric DRC not supported */
      gainElementIndex++;
    }
  }
  return DE_OK;
}
