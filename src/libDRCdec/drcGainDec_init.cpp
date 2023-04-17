

/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#include <stdlib.h>
#include "../libAACdec/newAACDecoder.h"
#include "drcDec_types.h"
#include "drcDec_tools.h"
#include "drcDec_gainDecoder.h"
#include "drcGainDec_init.h"

static DRC_ERROR _generateDrcInstructionsDerivedData(
    HANDLE_DRC_GAIN_DECODER hGainDec, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
    DRC_INSTRUCTIONS_UNI_DRC_t* pInst, DRC_COEFFICIENTS_UNI_DRC_t* pCoef,
    ACTIVE_DRC* pActiveDrc) {
  DRC_ERROR err = DE_OK;
  int32_t g;
  int32_t gainElementCount = 0;
  uint8_t nDrcChannelGroups = 0;
  int8_t gainSetIndexForChannelGroup[8];

  err = deriveDrcChannelGroups(
      pInst->drcSetEffect, pInst->drcChannelCount, pInst->gainSetIndex,
      pInst->drcSetEffect & (EB_DUCK_OTHER | EB_DUCK_SELF)
          ? pInst->duckingModificationForChannel
          : NULL,
      &nDrcChannelGroups, gainSetIndexForChannelGroup,
      pActiveDrc->channelGroupForChannel,
      pInst->drcSetEffect & (EB_DUCK_OTHER | EB_DUCK_SELF)
          ? pActiveDrc->duckingModificationForChannelGroup
          : NULL);
  if (err) return (err);

  /* sanity check */
  if (nDrcChannelGroups != pInst->nDrcChannelGroups) return DE_NOT_OK;
  for (g = 0; g < pInst->nDrcChannelGroups; g++) {
    if (gainSetIndexForChannelGroup[g] != pInst->gainSetIndexForChannelGroup[g])
      return DE_NOT_OK;
  }

  for (g = 0; g < pInst->nDrcChannelGroups; g++) {
    int32_t seq = pInst->gainSetIndexForChannelGroup[g];
    if (seq != -1 && (hUniDrcConfig->drcCoefficientsUniDrcCount == 0 ||
                      seq >= pCoef->gainSetCount)) {
      pActiveDrc->channelGroupIsParametricDrc[g] = 1;
    } else {
      pActiveDrc->channelGroupIsParametricDrc[g] = 0;
      if (seq >= pCoef->gainSetCount) {
        return DE_NOT_OK;
      }
    }
  }

  /* gainElementCount */
  if (pInst->drcSetEffect & (EB_DUCK_OTHER | EB_DUCK_SELF)) {
    for (g = 0; g < pInst->nDrcChannelGroups; g++) {
      pActiveDrc->bandCountForChannelGroup[g] = 1;
    }
    pActiveDrc->gainElementCount =
        pInst->nDrcChannelGroups; /* one gain element per channel group */
  } else {
    for (g = 0; g < pInst->nDrcChannelGroups; g++) {
      if (pActiveDrc->channelGroupIsParametricDrc[g]) {
        gainElementCount++;
        pActiveDrc->bandCountForChannelGroup[g] = 1;
      } else {
        int32_t seq, bandCount;
        seq = pInst->gainSetIndexForChannelGroup[g];
        bandCount = pCoef->gainSet[seq].bandCount;
        pActiveDrc->bandCountForChannelGroup[g] = bandCount;
        gainElementCount += bandCount;
      }
    }
    pActiveDrc->gainElementCount = gainElementCount;
  }

  /* prepare gainElementForGroup (cumulated sum of bandCountForChannelGroup) */
  pActiveDrc->gainElementForGroup[0] = 0;
  for (g = 1; g < pInst->nDrcChannelGroups; g++) {
    pActiveDrc->gainElementForGroup[g] =
        pActiveDrc->gainElementForGroup[g - 1] +
        pActiveDrc->bandCountForChannelGroup[g - 1]; /* index of first gain
                                                        sequence in channel
                                                        group */
  }

  return DE_OK;
}

DRC_ERROR
initGainDec(HANDLE_DRC_GAIN_DECODER hGainDec) {
  int32_t i, j, k;

  /* sanity check */
  if (hGainDec->deltaTminDefault > hGainDec->frameSize) return DE_NOT_OK;

  for (i = 0; i < MAX_ACTIVE_DRCS; i++) {
    for (j = 0; j < 8; j++) {
      /* use startup node at the beginning */
      hGainDec->activeDrc[i].lnbIndexForChannel[j][0] = 0;
      for (k = 1; k < NUM_LNB_FRAMES; k++) {
        hGainDec->activeDrc[i].lnbIndexForChannel[j][k] = -1;
      }
    }
  }

  for (j = 0; j < 8; j++) {
    hGainDec->channelGain[j] = FL2FXCONST_DBL(1.0f / (float)(1 << 8));
  }

  for (i = 0; i < 4 * 1024 / 256; i++) {
    hGainDec->dummySubbandGains[i] = FL2FXCONST_DBL(1.0f / (float)(1 << 7));
  }

  hGainDec->status = 0; /* startup */

  return DE_OK;
}

void initDrcGainBuffers(const int32_t frameSize, DRC_GAIN_BUFFERS* drcGainBuffers) {
  int32_t i, c, j;
  /* prepare 12 instances of node buffers */
  for (i = 0; i < 12; i++) {
    for (j = 0; j < NUM_LNB_FRAMES; j++) {
      drcGainBuffers->linearNodeBuffer[i].nNodes[j] = 1;
      drcGainBuffers->linearNodeBuffer[i].linearNode[j][0].gainLin =
          FL2FXCONST_DBL(1.0f / (float)(1 << 7));
      if (j == 0) {
        drcGainBuffers->linearNodeBuffer[i].linearNode[j][0].time =
            0; /* initialize last node with startup node */
      } else {
        drcGainBuffers->linearNodeBuffer[i].linearNode[j][0].time =
            frameSize - 1;
      }
    }
  }

  /* prepare dummyLnb, a linearNodeBuffer containing a constant gain of 0 dB,
   * for the "no DRC processing" case */
  drcGainBuffers->dummyLnb.gainInterpolationType = GIT_LINEAR;
  for (i = 0; i < NUM_LNB_FRAMES; i++) {
    drcGainBuffers->dummyLnb.nNodes[i] = 1;
    drcGainBuffers->dummyLnb.linearNode[i][0].gainLin =
        FL2FXCONST_DBL(1.0f / (float)(1 << 7));
    drcGainBuffers->dummyLnb.linearNode[i][0].time = frameSize - 1;
  }

  /* prepare channelGain delay line */
  for (c = 0; c < 8; c++) {
    for (i = 0; i < NUM_LNB_FRAMES; i++) {
      drcGainBuffers->channelGain[c][i] =
          FL2FXCONST_DBL(1.0f / (float)(1 << 8));
    }
  }

  drcGainBuffers->lnbPointer = 0;
}

DRC_ERROR
initActiveDrc(HANDLE_DRC_GAIN_DECODER hGainDec,
              HANDLE_UNI_DRC_CONFIG hUniDrcConfig, const int32_t drcSetIdSelected,
              const int32_t downmixIdSelected) {
  int32_t g, isMultiband = 0;
  DRC_ERROR err = DE_OK;
  DRC_INSTRUCTIONS_UNI_DRC_t* pInst = NULL;
  DRC_COEFFICIENTS_UNI_DRC_t* pCoef = NULL;

  pInst = selectDrcInstructions(hUniDrcConfig, drcSetIdSelected);
  if (pInst == NULL) {
    return DE_NOT_OK;
  }

  if (pInst->drcSetId >= 0) {
    pCoef = selectDrcCoefficients(hUniDrcConfig, pInst->drcLocation);
    if (pCoef == NULL) {
      return DE_NOT_OK;
    }

    if (pCoef->drcFrameSizePresent) {
      if (pCoef->drcFrameSize != hGainDec->frameSize) {
        return DE_NOT_OK;
      }
    }

    err = _generateDrcInstructionsDerivedData(
        hGainDec, hUniDrcConfig, pInst, pCoef,
        &(hGainDec->activeDrc[hGainDec->nActiveDrcs]));
    if (err) return err;
  }

  hGainDec->activeDrc[hGainDec->nActiveDrcs].pInst = pInst;
  hGainDec->activeDrc[hGainDec->nActiveDrcs].pCoef = pCoef;

  for (g = 0; g < pInst->nDrcChannelGroups; g++) {
    if (hGainDec->activeDrc[hGainDec->nActiveDrcs].bandCountForChannelGroup[g] >
        1) {
      if (hGainDec->multiBandActiveDrcIndex != -1) {
        return DE_NOT_OK;
      }
      isMultiband = 1;
    }
  }

  if (isMultiband) {
    /* Keep activeDrc index of multiband DRC set */
    hGainDec->multiBandActiveDrcIndex = hGainDec->nActiveDrcs;
  }

  if ((hGainDec->channelGainActiveDrcIndex == -1) &&
      (downmixIdSelected == DOWNMIX_ID_BASE_LAYOUT) &&
      (hUniDrcConfig->drcInstructionsUniDrcCount >
       0)) { /* use this activeDrc to apply channelGains */
    hGainDec->channelGainActiveDrcIndex = hGainDec->nActiveDrcs;
  }

  hGainDec->nActiveDrcs++;
  if (hGainDec->nActiveDrcs > MAX_ACTIVE_DRCS) return DE_NOT_OK;

  return DE_OK;
}

DRC_ERROR
initActiveDrcOffset(HANDLE_DRC_GAIN_DECODER hGainDec) {
  int32_t a, accGainElementCount;

  accGainElementCount = 0;
  for (a = 0; a < hGainDec->nActiveDrcs; a++) {
    hGainDec->activeDrc[a].activeDrcOffset = accGainElementCount;
    accGainElementCount += hGainDec->activeDrc[a].gainElementCount;
    if (accGainElementCount > 12) {
      hGainDec->nActiveDrcs = a;
      return DE_NOT_OK;
    }
  }

  return DE_OK;
}
