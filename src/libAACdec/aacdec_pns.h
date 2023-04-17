

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: perceptual noise substitution tool

*******************************************************************************/

#ifndef AACDEC_PNS_H
#define AACDEC_PNS_H
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"





void CPns_UpdateNoiseState(CPnsData_t *pPnsData, int32_t *currentSeed,
                           int32_t *randomSeed);

void CPns_ResetData(CPnsData_t *pPnsData,
                    CPnsInterChannelData_t *pPnsInterChannelData);

#endif /* #ifndef AACDEC_PNS_H */
