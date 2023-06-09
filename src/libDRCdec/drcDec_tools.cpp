

/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#include <stdlib.h>
#include "../libAACdec/newAACDecoder.h"
#include "drcDec_types.h"
#include "drcDec_tools.h"
#include "../libFDK/fixpoint_math.h"
#include "drcDecoder.h"

int32_t getDeltaTmin(const int32_t sampleRate) {
  /* half_ms = round (0.0005 * sampleRate); */
  int32_t half_ms = (sampleRate + 1000) / 2000;
  int32_t deltaTmin = 1;
  if (sampleRate < 1000) {
    return DE_NOT_OK;
  }
  while (deltaTmin <= half_ms) {
    deltaTmin = deltaTmin << 1;
  }
  return deltaTmin;
}

DRC_COEFFICIENTS_UNI_DRC_t* selectDrcCoefficients(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig, const int32_t location) {
  int32_t n;
  int32_t c = -1;
  for (n = 0; n < hUniDrcConfig->drcCoefficientsUniDrcCount; n++) {
    if (hUniDrcConfig->drcCoefficientsUniDrc[n].drcLocation == location) {
      c = n;
    }
  }
  if (c >= 0) {
    return &(hUniDrcConfig->drcCoefficientsUniDrc[c]);
  }
  return NULL; /* possible during bitstream parsing */
}

DRC_INSTRUCTIONS_UNI_DRC_t* selectDrcInstructions(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig, const int32_t drcSetId) {
  int32_t i;
  for (i = 0; i < hUniDrcConfig->drcInstructionsCountInclVirtual; i++) {
    if (hUniDrcConfig->drcInstructionsUniDrc[i].drcSetId == drcSetId) {
      return &(hUniDrcConfig->drcInstructionsUniDrc[i]);
    }
  }
  return NULL;
}

DOWNMIX_INSTRUCTIONS* selectDownmixInstructions(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig, const int32_t downmixId) {
  int32_t i;
  for (i = 0; i < hUniDrcConfig->downmixInstructionsCount; i++) {
    if (hUniDrcConfig->downmixInstructions[i].downmixId == downmixId) {
      return &(hUniDrcConfig->downmixInstructions[i]);
    }
  }
  return NULL;
}

DRC_ERROR
deriveDrcChannelGroups(
    const int32_t drcSetEffect,                                    /* in */
    const int32_t channelCount,                                    /* in */
    const int8_t* gainSetIndex,                                 /* in */
    const DUCKING_MODIFICATION_t* duckingModificationForChannel, /* in */
    uint8_t* nDrcChannelGroups,                                  /* out */
    int8_t* uniqueIndex,     /* out (gainSetIndexForChannelGroup) */
    int8_t* groupForChannel, /* out */
    DUCKING_MODIFICATION_t* duckingModificationForChannelGroup) /* out */
{
  int32_t duckingSequence = -1;
  int32_t c, n, g, match, idx;
  int16_t factor;
  int16_t uniqueScaling[8];

  for (g = 0; g < 8; g++) {
    uniqueIndex[g] = -10;
    uniqueScaling[g] = int16_t(-1.0f);
  }

  g = 0;

  if (drcSetEffect & EB_DUCK_OTHER) {
    for (c = 0; c < channelCount; c++) {
      match = 0;
      if (c >= 8) return DE_MEMORY_ERROR;
      idx = gainSetIndex[c];
      factor = duckingModificationForChannel[c].duckingScaling;
      if (idx < 0) {
        for (n = 0; n < g; n++) {
          if (uniqueScaling[n] == factor) {
            match = 1;
            groupForChannel[c] = n;
            break;
          }
        }
        if (match == 0) {
          if (g >= 8) return DE_MEMORY_ERROR;
          uniqueIndex[g] = idx;
          uniqueScaling[g] = factor;
          groupForChannel[c] = g;
          g++;
        }
      } else {
        if ((duckingSequence > 0) && (duckingSequence != idx)) {
          return DE_NOT_OK;
        }
        duckingSequence = idx;
        groupForChannel[c] = -1;
      }
    }
    if (duckingSequence == -1) {
      return DE_NOT_OK;
    }
  } else if (drcSetEffect & EB_DUCK_SELF) {
    for (c = 0; c < channelCount; c++) {
      match = 0;
      if (c >= 8) return DE_MEMORY_ERROR;
      idx = gainSetIndex[c];
      factor = duckingModificationForChannel[c].duckingScaling;
      if (idx >= 0) {
        for (n = 0; n < g; n++) {
          if ((uniqueIndex[n] == idx) && (uniqueScaling[n] == factor)) {
            match = 1;
            groupForChannel[c] = n;
            break;
          }
        }
        if (match == 0) {
          if (g >= 8) return DE_MEMORY_ERROR;
          uniqueIndex[g] = idx;
          uniqueScaling[g] = factor;
          groupForChannel[c] = g;
          g++;
        }
      } else {
        groupForChannel[c] = -1;
      }
    }
  } else { /* no ducking */
    for (c = 0; c < channelCount; c++) {
      if (c >= 8) return DE_MEMORY_ERROR;
      idx = gainSetIndex[c];
      match = 0;
      if (idx >= 0) {
        for (n = 0; n < g; n++) {
          if (uniqueIndex[n] == idx) {
            match = 1;
            groupForChannel[c] = n;
            break;
          }
        }
        if (match == 0) {
          if (g >= 8) return DE_MEMORY_ERROR;
          uniqueIndex[g] = idx;
          groupForChannel[c] = g;
          g++;
        }
      } else {
        groupForChannel[c] = -1;
      }
    }
  }
  *nDrcChannelGroups = g;

  if (drcSetEffect & (EB_DUCK_OTHER | EB_DUCK_SELF)) {
    for (g = 0; g < *nDrcChannelGroups; g++) {
      if (drcSetEffect & EB_DUCK_OTHER) {
        uniqueIndex[g] = duckingSequence;
      }
      duckingModificationForChannelGroup[g].duckingScaling = uniqueScaling[g];
      if (uniqueScaling[g] != FL2FXCONST_SGL(1.0f / (float)(1 << 2))) {
        duckingModificationForChannelGroup[g].duckingScalingPresent = 1;
      } else {
        duckingModificationForChannelGroup[g].duckingScalingPresent = 0;
      }
    }
  }

  return DE_OK;
}

int32_t
dB2lin(const int32_t dB_m, const int32_t dB_e, int32_t* pLin_e) {
  /* get linear value from dB.
     return lin_val = 10^(dB_val/20) = 2^(log2(10)/20*dB_val)
     with dB_val = dB_m *2^dB_e and lin_val = lin_m * 2^lin_e */
  int32_t lin_m =
      f2Pow(fMult(dB_m, FL2FXCONST_DBL(0.1660964f * (float)(1 << 2))), dB_e - 2,
            pLin_e);

  return lin_m;
}

int32_t
lin2dB(const int32_t lin_m, const int32_t lin_e, int32_t* pDb_e) {
  /* get dB value from linear value.
     return dB_val = 20*log10(lin_val)
     with dB_val = dB_m *2^dB_e and lin_val = lin_m * 2^lin_e */
  int32_t dB_m;

  if (lin_m == (int32_t)0) { /* return very small value representing -inf */
    dB_m = (int32_t)MINVAL_DBL;
    *pDb_e = DFRACT_BITS - 1;
  } else {
    /* 20*log10(lin_val) = 20/log2(10)*log2(lin_val) */
    dB_m = fMultDiv2(FL2FXCONST_DBL(6.02059991f / (float)(1 << 3)),
                     fLog2(lin_m, lin_e, pDb_e));
    *pDb_e += 3 + 1;
  }

  return dB_m;
}

int32_t
approxDb2lin(const int32_t dB_m, const int32_t dB_e, int32_t* pLin_e) {
  /* get linear value from approximate dB.
     return lin_val = 2^(dB_val/6)
     with dB_val = dB_m *2^dB_e and lin_val = lin_m * 2^lin_e */
  int32_t lin_m =
      f2Pow(fMult(dB_m, FL2FXCONST_DBL(0.1666667f * (float)(1 << 2))), dB_e - 2,
            pLin_e);

  return lin_m;
}

int32_t bitstreamContainsMultibandDrc(HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
                                  const int32_t downmixId) {
  int32_t i, g, d, seq;
  DRC_INSTRUCTIONS_UNI_DRC_t* pInst;
  DRC_COEFFICIENTS_UNI_DRC_t* pCoef = NULL;
  int32_t isMultiband = 0;

  pCoef = selectDrcCoefficients(hUniDrcConfig, LOCATION_SELECTED);
  if (pCoef == NULL) return 0;

  for (i = 0; i < hUniDrcConfig->drcInstructionsUniDrcCount; i++) {
    pInst = &(hUniDrcConfig->drcInstructionsUniDrc[i]);
    for (d = 0; d < pInst->downmixIdCount; d++) {
      if (downmixId == pInst->downmixId[d]) {
        for (g = 0; g < pInst->nDrcChannelGroups; g++) {
          seq = pInst->gainSetIndexForChannelGroup[g];
          if (pCoef->gainSet[seq].bandCount > 1) {
            isMultiband = 1;
          }
        }
      }
    }
  }

  return isMultiband;
}

int32_t getDownmixOffset(DOWNMIX_INSTRUCTIONS* pDown, int32_t baseChannelCount) {
  int32_t downmixOffset = FL2FXCONST_DBL(1.0f / (1 << 1)); /* e = 1 */
  if ((pDown->bsDownmixOffset == 1) || (pDown->bsDownmixOffset == 2)) {
    int32_t e_a, e_downmixOffset;
    int32_t a, q;
    if (baseChannelCount <= pDown->targetChannelCount) return downmixOffset;

    q = fDivNorm((int32_t)pDown->targetChannelCount,
                 (int32_t)baseChannelCount); /* e = 0 */
    a = lin2dB(q, 0, &e_a);
    if (pDown->bsDownmixOffset == 2) {
      e_a += 1; /* a *= 2 */
    }
    /* a = 0.5 * round (a) */
    a = fixp_round(a, e_a) >> 1;
    downmixOffset = dB2lin(a, e_a, &e_downmixOffset);
    downmixOffset = scaleValue(downmixOffset, e_downmixOffset - 1);
  }
  return downmixOffset;
}
