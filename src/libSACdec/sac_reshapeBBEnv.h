

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: SAC Dec guided envelope shaping

*******************************************************************************/

#ifndef SAC_RESHAPEBBENV_H
#define SAC_RESHAPEBBENV_H
#include "../libAACdec/newAACDecoder.h"
#include "sac_dec_interface.h"

#define BB_ENV_SIZE 9 /* END_BB_ENV - START_BB_ENV */

void initBBEnv(spatialDec *self, int32_t initStatesFlag);
void SpatialDecReshapeBBEnv(spatialDec *self, const SPATIAL_BS_FRAME *frame,
                            int32_t ts);

#endif
