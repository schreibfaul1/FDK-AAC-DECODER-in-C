

/**************************** SBR decoder library ******************************

   Author(s):   Christian Griebel

   Description: Dynamic range control (DRC) decoder tool for SBR

*******************************************************************************/

#ifndef SBRDEC_DRC_H
#define SBRDEC_DRC_H
#include "../libAACdec/newAACDecoder.h"
#include "sbrdecoder.h"

#define SBRDEC_MAX_DRC_CHANNELS (8)
#define SBRDEC_MAX_DRC_BANDS (16)

typedef struct {
  int32_t prevFact_mag[(64)];
  int32_t prevFact_exp;

  int32_t currFact_mag[SBRDEC_MAX_DRC_BANDS];
  int32_t nextFact_mag[SBRDEC_MAX_DRC_BANDS];
  int32_t currFact_exp;
  int32_t nextFact_exp;

  uint32_t numBandsCurr;
  uint32_t numBandsNext;
  uint16_t bandTopCurr[SBRDEC_MAX_DRC_BANDS];
  uint16_t bandTopNext[SBRDEC_MAX_DRC_BANDS];

  int16_t drcInterpolationSchemeCurr;
  int16_t drcInterpolationSchemeNext;

  int16_t enable;

  uint8_t winSequenceCurr;
  uint8_t winSequenceNext;

} SBRDEC_DRC_CHANNEL;

typedef SBRDEC_DRC_CHANNEL *HANDLE_SBR_DRC_CHANNEL;

void sbrDecoder_drcInitChannel(HANDLE_SBR_DRC_CHANNEL hDrcData);

void sbrDecoder_drcUpdateChannel(HANDLE_SBR_DRC_CHANNEL hDrcData);

void sbrDecoder_drcApplySlot(HANDLE_SBR_DRC_CHANNEL hDrcData,
                             int32_t *qmfRealSlot, int32_t *qmfImagSlot,
                             int32_t col, int32_t numQmfSubSamples, int32_t maxShift);

void sbrDecoder_drcApply(HANDLE_SBR_DRC_CHANNEL hDrcData,
                         int32_t **QmfBufferReal, int32_t **QmfBufferImag,
                         int32_t numQmfSubSamples, int32_t *scaleFactor);

#endif /* SBRDEC_DRC_H */
