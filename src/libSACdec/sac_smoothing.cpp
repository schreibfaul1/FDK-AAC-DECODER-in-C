

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: SAC Dec parameter smoothing

*******************************************************************************/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "sac_dec.h"
#include "sac_bitdec.h"
#include "sac_smoothing.h"
#include "sac_rom.h"

/*******************************************************************************
 Functionname: calcFilterCoeff
 *******************************************************************************

 Description:

 Arguments:

 Input:

 Output:


*******************************************************************************/
static int32_t calcFilterCoeff__FDK(spatialDec *self, int32_t ps,
                                     const SPATIAL_BS_FRAME *frame) {
  int32_t dSlots;
  int32_t delta;

  dSlots = frame->paramSlot[ps] - self->smoothState->prevParamSlot;

  if (dSlots <= 0) {
    dSlots += self->timeSlots;
  }

  delta = fDivNorm(dSlots, self->smgTime[ps]);

  return delta;
}

/*******************************************************************************
 Functionname: getSmoothOnOff
 *******************************************************************************

 Description:

 Arguments:

 Input:

 Output:


*******************************************************************************/
static int32_t getSmoothOnOff(spatialDec *self, int32_t ps, int32_t pb) {
  int32_t smoothBand = 0;

  smoothBand = self->smgData[ps][pb];

  return smoothBand;
}

void SpatialDecSmoothM1andM2(spatialDec *self, const SPATIAL_BS_FRAME *frame,
                             int32_t ps) {
  int32_t delta__FDK;
  int32_t one_minus_delta__FDK;

  int32_t pb, row, col;
  int32_t residualBands = 0;

  if (self->residualCoding) {
    int32_t i;
    int32_t boxes = self->numOttBoxes;
    for (i = 0; i < boxes; i++) {
      if (self->residualBands[i] > residualBands) {
        residualBands = self->residualBands[i];
      }
    }
  }

  delta__FDK = calcFilterCoeff__FDK(self, ps, frame);
  if (delta__FDK == /*FL2FXCONST_DBL(1.0f)*/ (int32_t)MAXVAL_DBL)
    one_minus_delta__FDK = FL2FXCONST_DBL(0.0f);
  else if (delta__FDK == FL2FXCONST_DBL(0.0f))
    one_minus_delta__FDK = /*FL2FXCONST_DBL(1.0f)*/ (int32_t)MAXVAL_DBL;
  else
    one_minus_delta__FDK = (FL2FXCONST_DBL(0.5f) - (delta__FDK >> 1)) << 1;

  for (pb = 0; pb < self->numParameterBands; pb++) {
    int32_t smoothBand;

    smoothBand = getSmoothOnOff(self, ps, pb);

    if (smoothBand && (pb >= residualBands)) {
      for (row = 0; row < self->numM2rows; row++) {
        for (col = 0; col < self->numVChannels; col++) {
          self->M2Real__FDK[row][col][pb] =
              ((fMultDiv2(delta__FDK, self->M2Real__FDK[row][col][pb]) +
                fMultDiv2(one_minus_delta__FDK,
                          self->M2RealPrev__FDK[row][col][pb]))
               << 1);
          if (0 || (self->phaseCoding == 3)) {
            self->M2Imag__FDK[row][col][pb] =
                ((fMultDiv2(delta__FDK, self->M2Imag__FDK[row][col][pb]) +
                  fMultDiv2(one_minus_delta__FDK,
                            self->M2ImagPrev__FDK[row][col][pb]))
                 << 1);
          }
        }
      }
    }
  }
  self->smoothState->prevParamSlot = frame->paramSlot[ps];
}

/* init states */
void initParameterSmoothing(spatialDec *self) {
  self->smoothState->prevParamSlot = 0;
}

void SpatialDecSmoothOPD(spatialDec *self, const SPATIAL_BS_FRAME *frame,
                         int32_t ps) {
  int32_t pb;
  int32_t dSlots;
  int32_t delta__FDK;
  int32_t one_minus_delta__FDK;
  int32_t *phaseLeftSmooth__FDK = self->smoothState->opdLeftState__FDK;
  int32_t *phaseRightSmooth__FDK = self->smoothState->opdRightState__FDK;
  int32_t quantCoarse;

  quantCoarse = frame->IPDLosslessData[0].bsQuantCoarseXXX[ps];

  if (frame->OpdSmoothingMode == 0) {
    memcpy(phaseLeftSmooth__FDK, self->PhaseLeft__FDK,
              self->numParameterBands * sizeof(int32_t));
    memcpy(phaseRightSmooth__FDK, self->PhaseRight__FDK,
              self->numParameterBands * sizeof(int32_t));
  } else {
    if (ps == 0) {
      dSlots = frame->paramSlot[ps] + 1;
    } else {
      dSlots = frame->paramSlot[ps] - frame->paramSlot[ps - 1];
    }

    delta__FDK = (int32_t)((int32_t)(FL2FXCONST_DBL(0.0078125f)) * dSlots);

    if (delta__FDK == (int32_t)MAXVAL_DBL /*FL2FXCONST_DBL(1.0f)*/)
      one_minus_delta__FDK = FL2FXCONST_DBL(0.0f);
    else if (delta__FDK == FL2FXCONST_DBL(0.0f))
      one_minus_delta__FDK = (int32_t)MAXVAL_DBL /*FL2FXCONST_DBL(1.0f)*/;
    else
      one_minus_delta__FDK = (FL2FXCONST_DBL(0.5f) - (delta__FDK >> 1)) << 1;

    for (pb = 0; pb < self->numParameterBands; pb++) {
      int32_t tmpL, tmpR, tmp;

      tmpL = self->PhaseLeft__FDK[pb];
      tmpR = self->PhaseRight__FDK[pb];

      while (tmpL > phaseLeftSmooth__FDK[pb] + PI__IPD) tmpL -= PI__IPD << 1;
      while (tmpL < phaseLeftSmooth__FDK[pb] - PI__IPD) tmpL += PI__IPD << 1;
      while (tmpR > phaseRightSmooth__FDK[pb] + PI__IPD) tmpR -= PI__IPD << 1;
      while (tmpR < phaseRightSmooth__FDK[pb] - PI__IPD) tmpR += PI__IPD << 1;

      phaseLeftSmooth__FDK[pb] =
          fMult(delta__FDK, tmpL) +
          fMult(one_minus_delta__FDK, phaseLeftSmooth__FDK[pb]);
      phaseRightSmooth__FDK[pb] =
          fMult(delta__FDK, tmpR) +
          fMult(one_minus_delta__FDK, phaseRightSmooth__FDK[pb]);

      tmp = (((tmpL >> 1) - (tmpR >> 1)) - ((phaseLeftSmooth__FDK[pb] >> 1) -
                                            (phaseRightSmooth__FDK[pb] >> 1)))
            << 1;
      while (tmp > PI__IPD) tmp -= PI__IPD << 1;
      while (tmp < -PI__IPD) tmp += PI__IPD << 1;
      if (fixp_abs(tmp) > fMult((quantCoarse ? FL2FXCONST_DBL(50.f / 180.f)
                                             : FL2FXCONST_DBL(25.f / 180.f)),
                                PI__IPD)) {
        phaseLeftSmooth__FDK[pb] = tmpL;
        phaseRightSmooth__FDK[pb] = tmpR;
      }

      while (phaseLeftSmooth__FDK[pb] > PI__IPD << 1)
        phaseLeftSmooth__FDK[pb] -= PI__IPD << 1;
      while (phaseLeftSmooth__FDK[pb] < (int32_t)0)
        phaseLeftSmooth__FDK[pb] += PI__IPD << 1;
      while (phaseRightSmooth__FDK[pb] > PI__IPD << 1)
        phaseRightSmooth__FDK[pb] -= PI__IPD << 1;
      while (phaseRightSmooth__FDK[pb] < (int32_t)0)
        phaseRightSmooth__FDK[pb] += PI__IPD << 1;

      self->PhaseLeft__FDK[pb] = phaseLeftSmooth__FDK[pb];
      self->PhaseRight__FDK[pb] = phaseRightSmooth__FDK[pb];
    }
  }
  return;
}
