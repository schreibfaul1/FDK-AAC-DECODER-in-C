

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: perceptual noise substitution tool

*******************************************************************************/

#ifndef AACDEC_PNS_H
#define AACDEC_PNS_H
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"

#define NO_OFBANDS ((8 * 16))

typedef struct {
  uint8_t correlated[NO_OFBANDS];
} CPnsInterChannelData;

typedef struct {
  CPnsInterChannelData *pPnsInterChannelData;
  uint8_t pnsUsed[NO_OFBANDS];
  int32_t CurrentEnergy;
  uint8_t PnsActive;
  int32_t *currentSeed;
  int32_t *randomSeed;
} CPnsData;

void CPns_UpdateNoiseState(CPnsData *pPnsData, int32_t *currentSeed,
                           int32_t *randomSeed);

void CPns_ResetData(CPnsData *pPnsData,
                    CPnsInterChannelData *pPnsInterChannelData);

#endif /* #ifndef AACDEC_PNS_H */
