

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: SAC Dec parameter smoothing

*******************************************************************************/

#ifndef SAC_SMOOTHING_H
#define SAC_SMOOTHING_H
#include "../libAACdec/newAACDecoder.h"
#include "sac_dec.h"

void initParameterSmoothing(spatialDec *self);
void SpatialDecSmoothM1andM2(spatialDec *self, const SPATIAL_BS_FRAME *frame,
                             int32_t ps);
void SpatialDecSmoothOPD(spatialDec *self, const SPATIAL_BS_FRAME *frame,
                         int32_t ps);

#endif
