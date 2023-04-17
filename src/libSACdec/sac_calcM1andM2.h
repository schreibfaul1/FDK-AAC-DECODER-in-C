

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: SAC Dec M1 and M2 calculation

*******************************************************************************/

/* sa_calcM1andM2.h */

#ifndef SAC_CALCM1ANDM2_H
#define SAC_CALCM1ANDM2_H
#include "../libAACdec/newAACDecoder.h"
#include "sac_dec.h"

#define SCALE_PARAM_M1 3

/* Scaling of M2 matrix, but only for binaural upmix type. */
#define SCALE_PARAM_CALC_M2 (3)
#define SCALE_PARAM_M2_515X (3)
#define SCALE_PARAM_M2_525 (SCALE_PARAM_M1 + HRG_SF + 1 - SCALE_PARAM_CALC_M2)
#define SCALE_PARAM_M2_212_PRED (3)
/* Scaling of spectral data after applying M2 matrix, but only for binaural
   upmix type Scaling is compensated later in synthesis qmf filterbank */
#define SCALE_DATA_APPLY_M2 (1)
/* Applying M2 parameter in combination with phase coding needs 2 bits headroom
 * because up to a maximum of 4 spectral values can be added for USAC */
#define SCALE_DATA_APPLY_M2_PC (2)

SACDEC_ERROR initM1andM2(spatialDec* self, int32_t initStatesFlag,
                         int32_t configChanged);

int32_t SpatialDecGetResidualIndex(spatialDec* self, int32_t row);

SACDEC_ERROR SpatialDecCalculateM1andM2(spatialDec* self, int32_t ps,
                                        const SPATIAL_BS_FRAME* frame);

#endif /* SAC_CALCM1ANDM2_H */
