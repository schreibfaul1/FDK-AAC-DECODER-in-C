

/**************************** SBR decoder library ******************************

   Author(s):   Christian Griebel

   Description: Dynamic range control (DRC) decoder tool for SBR

*******************************************************************************/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "sbrdec_drc.h"

/* DRC - Offset table for QMF interpolation. Shifted by one index position.
   The table defines the (int16_t) window borders rounded to the nearest QMF
   timeslot. It has the size 16 because it is accessed with the
   drcInterpolationScheme that is read from the bitstream with 4 bit. */
static const uint8_t winBorderToColMappingTab[2][16] = {
    /*-1, 0, 1, 2,  3,  4,  5,  6,  7,  8 */
    {0, 0, 4, 8, 12, 16, 20, 24, 28, 32, 32, 32, 32, 32, 32,
     32}, /* 1024 framing */
    {0, 0, 4, 8, 11, 15, 19, 23, 26, 30, 30, 30, 30, 30, 30,
     30} /*  960 framing */
};

/*!
  \brief Initialize DRC QMF factors

  \hDrcData Handle to DRC channel data.

  \return none
*/
void sbrDecoder_drcInitChannel(HANDLE_SBR_DRC_CHANNEL hDrcData) {
  int32_t band;

  if (hDrcData == NULL) {
    return;
  }

  for (band = 0; band < (64); band++) {
    hDrcData->prevFact_mag[band] = FL2FXCONST_DBL(0.5f);
  }

  for (band = 0; band < SBRDEC_MAX_DRC_BANDS; band++) {
    hDrcData->currFact_mag[band] = FL2FXCONST_DBL(0.5f);
    hDrcData->nextFact_mag[band] = FL2FXCONST_DBL(0.5f);
  }

  hDrcData->prevFact_exp = 1;
  hDrcData->currFact_exp = 1;
  hDrcData->nextFact_exp = 1;

  hDrcData->numBandsCurr = 1;
  hDrcData->numBandsNext = 1;

  hDrcData->winSequenceCurr = 0;
  hDrcData->winSequenceNext = 0;

  hDrcData->drcInterpolationSchemeCurr = 0;
  hDrcData->drcInterpolationSchemeNext = 0;

  hDrcData->enable = 0;
}

/*!
  \brief Swap DRC QMF scaling factors after they have been applied.

  \hDrcData Handle to DRC channel data.

  \return none
*/
void sbrDecoder_drcUpdateChannel(HANDLE_SBR_DRC_CHANNEL hDrcData) {
  if (hDrcData == NULL) {
    return;
  }
  if (hDrcData->enable != 1) {
    return;
  }

  /* swap previous data */
  memcpy(hDrcData->currFact_mag, hDrcData->nextFact_mag,
            SBRDEC_MAX_DRC_BANDS * sizeof(int32_t));

  hDrcData->currFact_exp = hDrcData->nextFact_exp;

  hDrcData->numBandsCurr = hDrcData->numBandsNext;

  memcpy(hDrcData->bandTopCurr, hDrcData->bandTopNext,
            SBRDEC_MAX_DRC_BANDS * sizeof(uint16_t));

  hDrcData->drcInterpolationSchemeCurr = hDrcData->drcInterpolationSchemeNext;

  hDrcData->winSequenceCurr = hDrcData->winSequenceNext;
}

/*!
  \brief Apply DRC factors slot based.

  \hDrcData Handle to DRC channel data.
  \qmfRealSlot Pointer to real valued QMF data of one time slot.
  \qmfImagSlot Pointer to the imaginary QMF data of one time slot.
  \col Number of the time slot.
  \numQmfSubSamples Total number of time slots for one frame.
  \scaleFactor Pointer to the out scale factor of the time slot.

  \return None.
*/
void sbrDecoder_drcApplySlot(HANDLE_SBR_DRC_CHANNEL hDrcData,
                             int32_t *qmfRealSlot, int32_t *qmfImagSlot,
                             int32_t col, int32_t numQmfSubSamples, int32_t maxShift) {
  const uint8_t *winBorderToColMap;

  int32_t band, bottomMdct, topMdct, bin, useLP;
  int32_t indx = numQmfSubSamples - (numQmfSubSamples >> 1) - 10; /* l_border */
  int32_t frameLenFlag = (numQmfSubSamples == 30) ? 1 : 0;
  int32_t frameSize = (frameLenFlag == 1) ? 960 : 1024;

  const int32_t *fact_mag = NULL;
  int32_t fact_exp = 0;
  uint32_t numBands = 0;
  uint16_t *bandTop = NULL;
  int32_t shortDrc = 0;

  int32_t alphaValue = FL2FXCONST_DBL(0.0f);

  if (hDrcData == NULL) {
    return;
  }
  if (hDrcData->enable != 1) {
    return;
  }

  winBorderToColMap = winBorderToColMappingTab[frameLenFlag];

  useLP = (qmfImagSlot == NULL) ? 1 : 0;

  col += indx;
  bottomMdct = 0;

  /* get respective data and calc interpolation factor */
  if (col < (numQmfSubSamples >> 1)) {    /* first half of current frame */
    if (hDrcData->winSequenceCurr != 2) { /* int32_t window */
      int32_t j = col + (numQmfSubSamples >> 1);

      if (hDrcData->drcInterpolationSchemeCurr == 0) {
        int32_t k = (frameLenFlag) ? 0x4444445 : 0x4000000;

        alphaValue = (int32_t)(j * k);
      } else {
        if (j >= (int32_t)winBorderToColMap[hDrcData->drcInterpolationSchemeCurr]) {
          alphaValue = (int32_t)MAXVAL_DBL;
        }
      }
    } else { /* int16_t windows */
      shortDrc = 1;
    }

    fact_mag = hDrcData->currFact_mag;
    fact_exp = hDrcData->currFact_exp;
    numBands = hDrcData->numBandsCurr;
    bandTop = hDrcData->bandTopCurr;
  } else if (col < numQmfSubSamples) {    /* second half of current frame */
    if (hDrcData->winSequenceNext != 2) { /* next: int32_t window */
      int32_t j = col - (numQmfSubSamples >> 1);

      if (hDrcData->drcInterpolationSchemeNext == 0) {
        int32_t k = (frameLenFlag) ? 0x4444445 : 0x4000000;

        alphaValue = (int32_t)(j * k);
      } else {
        if (j >= (int32_t)winBorderToColMap[hDrcData->drcInterpolationSchemeNext]) {
          alphaValue = (int32_t)MAXVAL_DBL;
        }
      }

      fact_mag = hDrcData->nextFact_mag;
      fact_exp = hDrcData->nextFact_exp;
      numBands = hDrcData->numBandsNext;
      bandTop = hDrcData->bandTopNext;
    } else {                                /* next: int16_t windows */
      if (hDrcData->winSequenceCurr != 2) { /* current: int32_t window */
        alphaValue = (int32_t)0;

        fact_mag = hDrcData->nextFact_mag;
        fact_exp = hDrcData->nextFact_exp;
        numBands = hDrcData->numBandsNext;
        bandTop = hDrcData->bandTopNext;
      } else { /* current: int16_t windows */
        shortDrc = 1;

        fact_mag = hDrcData->currFact_mag;
        fact_exp = hDrcData->currFact_exp;
        numBands = hDrcData->numBandsCurr;
        bandTop = hDrcData->bandTopCurr;
      }
    }
  } else {                                /* first half of next frame */
    if (hDrcData->winSequenceNext != 2) { /* int32_t window */
      int32_t j = col - (numQmfSubSamples >> 1);

      if (hDrcData->drcInterpolationSchemeNext == 0) {
        int32_t k = (frameLenFlag) ? 0x4444445 : 0x4000000;

        alphaValue = (int32_t)(j * k);
      } else {
        if (j >= (int32_t)winBorderToColMap[hDrcData->drcInterpolationSchemeNext]) {
          alphaValue = (int32_t)MAXVAL_DBL;
        }
      }
    } else { /* int16_t windows */
      shortDrc = 1;
    }

    fact_mag = hDrcData->nextFact_mag;
    fact_exp = hDrcData->nextFact_exp;
    numBands = hDrcData->numBandsNext;
    bandTop = hDrcData->bandTopNext;

    col -= numQmfSubSamples;
  }

  /* process bands */
  for (band = 0; band < (int32_t)numBands; band++) {
    int32_t bottomQmf, topQmf;

    int32_t drcFact_mag = (int32_t)MAXVAL_DBL;

    topMdct = (bandTop[band] + 1) << 2;

    if (!shortDrc) { /* int32_t window */
      if (frameLenFlag) {
        /* 960 framing */
        bottomQmf = fMultIfloor((int32_t)0x4444445, bottomMdct);
        topQmf = fMultIfloor((int32_t)0x4444445, topMdct);

        topMdct = 30 * topQmf;
      } else {
        /* 1024 framing */
        topMdct &= ~0x1f;

        bottomQmf = bottomMdct >> 5;
        topQmf = topMdct >> 5;
      }

      if (band == ((int32_t)numBands - 1)) {
        topQmf = (64);
      }

      for (bin = bottomQmf; bin < topQmf; bin++) {
        int32_t drcFact1_mag = hDrcData->prevFact_mag[bin];
        int32_t drcFact2_mag = fact_mag[band];

        /* normalize scale factors */
        if (hDrcData->prevFact_exp < maxShift) {
          drcFact1_mag >>= maxShift - hDrcData->prevFact_exp;
        }
        if (fact_exp < maxShift) {
          drcFact2_mag >>= maxShift - fact_exp;
        }

        /* interpolate */
        if (alphaValue == (int32_t)0) {
          drcFact_mag = drcFact1_mag;
        } else if (alphaValue == (int32_t)MAXVAL_DBL) {
          drcFact_mag = drcFact2_mag;
        } else {
          drcFact_mag =
              fMult(alphaValue, drcFact2_mag) +
              fMult(((int32_t)MAXVAL_DBL - alphaValue), drcFact1_mag);
        }

        /* apply scaling */
        qmfRealSlot[bin] = fMult(qmfRealSlot[bin], drcFact_mag);
        if (!useLP) {
          qmfImagSlot[bin] = fMult(qmfImagSlot[bin], drcFact_mag);
        }

        /* save previous factors */
        if (col == (numQmfSubSamples >> 1) - 1) {
          hDrcData->prevFact_mag[bin] = fact_mag[band];
        }
      }
    } else { /* int16_t windows */
      unsigned startWinIdx, stopWinIdx;
      int32_t startCol, stopCol;
      int32_t invFrameSizeDiv8 =
          (frameLenFlag) ? (int32_t)0x1111112 : (int32_t)0x1000000;

      /* limit top at the frame borders */
      if (topMdct < 0) {
        topMdct = 0;
      }
      if (topMdct >= frameSize) {
        topMdct = frameSize - 1;
      }

      if (frameLenFlag) {
        /*  960 framing */
        topMdct = fMultIfloor((int32_t)0x78000000,
                              fMultIfloor((int32_t)0x22222223, topMdct) << 2);

        startWinIdx = fMultIfloor(invFrameSizeDiv8, bottomMdct) +
                      1; /* winBorderToColMap table has offset of 1 */
        stopWinIdx = fMultIceil(invFrameSizeDiv8 - (int32_t)1, topMdct) + 1;
      } else {
        /* 1024 framing */
        topMdct &= ~0x03;

        startWinIdx = fMultIfloor(invFrameSizeDiv8, bottomMdct) + 1;
        stopWinIdx = fMultIceil(invFrameSizeDiv8, topMdct) + 1;
      }

      /* startCol is truncated to the nearest corresponding start subsample in
         the QMF of the int16_t window bottom is present in:*/
      startCol = (int32_t)winBorderToColMap[startWinIdx];

      /* stopCol is rounded upwards to the nearest corresponding stop subsample
         in the QMF of the int16_t window top is present in. */
      stopCol = (int32_t)winBorderToColMap[stopWinIdx];

      bottomQmf = fMultIfloor(invFrameSizeDiv8,
                              ((bottomMdct % (numQmfSubSamples << 2)) << 5));
      topQmf = fMultIfloor(invFrameSizeDiv8,
                           ((topMdct % (numQmfSubSamples << 2)) << 5));

      /* extend last band */
      if (band == ((int32_t)numBands - 1)) {
        topQmf = (64);
        stopCol = numQmfSubSamples;
        stopWinIdx = 10;
      }

      if (topQmf == 0) {
        if (frameLenFlag) {
          int32_t rem = fMult(invFrameSizeDiv8,
                               (int32_t)(topMdct << (DFRACT_BITS - 12)));
          if ((int32_t)rem & (int32_t)0x1F) {
            stopWinIdx -= 1;
            stopCol = (int32_t)winBorderToColMap[stopWinIdx];
          }
        }
        topQmf = (64);
      }

      /* save previous factors */
      if (stopCol == numQmfSubSamples) {
        int32_t tmpBottom = bottomQmf;

        if ((int32_t)winBorderToColMap[8] > startCol) {
          tmpBottom = 0; /* band starts in previous int16_t window */
        }

        for (bin = tmpBottom; bin < topQmf; bin++) {
          hDrcData->prevFact_mag[bin] = fact_mag[band];
        }
      }

      /* apply */
      if ((col >= startCol) && (col < stopCol)) {
        if (col >= (int32_t)winBorderToColMap[startWinIdx + 1]) {
          bottomQmf = 0; /* band starts in previous int16_t window */
        }
        if (col < (int32_t)winBorderToColMap[stopWinIdx - 1]) {
          topQmf = (64); /* band ends in next int16_t window */
        }

        drcFact_mag = fact_mag[band];

        /* normalize scale factor */
        if (fact_exp < maxShift) {
          drcFact_mag >>= maxShift - fact_exp;
        }

        /* apply scaling */
        for (bin = bottomQmf; bin < topQmf; bin++) {
          qmfRealSlot[bin] = fMult(qmfRealSlot[bin], drcFact_mag);
          if (!useLP) {
            qmfImagSlot[bin] = fMult(qmfImagSlot[bin], drcFact_mag);
          }
        }
      }
    }

    bottomMdct = topMdct;
  } /* end of bands loop */

  if (col == (numQmfSubSamples >> 1) - 1) {
    hDrcData->prevFact_exp = fact_exp;
  }
}

/*!
  \brief Apply DRC factors frame based.

  \hDrcData Handle to DRC channel data.
  \qmfRealSlot Pointer to real valued QMF data of the whole frame.
  \qmfImagSlot Pointer to the imaginary QMF data of the whole frame.
  \numQmfSubSamples Total number of time slots for one frame.
  \scaleFactor Pointer to the out scale factor of the frame.

  \return None.
*/
void sbrDecoder_drcApply(HANDLE_SBR_DRC_CHANNEL hDrcData,
                         int32_t **QmfBufferReal, int32_t **QmfBufferImag,
                         int32_t numQmfSubSamples, int32_t *scaleFactor) {
  int32_t col;
  int32_t maxShift = 0;

  if (hDrcData == NULL) {
    return;
  }
  if (hDrcData->enable == 0) {
    return; /* Avoid changing the scaleFactor even though the processing is
               disabled. */
  }

  /* get max scale factor */
  if (hDrcData->prevFact_exp > maxShift) {
    maxShift = hDrcData->prevFact_exp;
  }
  if (hDrcData->currFact_exp > maxShift) {
    maxShift = hDrcData->currFact_exp;
  }
  if (hDrcData->nextFact_exp > maxShift) {
    maxShift = hDrcData->nextFact_exp;
  }

  for (col = 0; col < numQmfSubSamples; col++) {
    int32_t *qmfSlotReal = QmfBufferReal[col];
    int32_t *qmfSlotImag = (QmfBufferImag == NULL) ? NULL : QmfBufferImag[col];

    sbrDecoder_drcApplySlot(hDrcData, qmfSlotReal, qmfSlotImag, col,
                            numQmfSubSamples, maxShift);
  }

  *scaleFactor += maxShift;
}
