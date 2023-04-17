

/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#ifndef DRCGAINDEC_PREPROCESS_H
#define DRCGAINDEC_PREPROCESS_H
#include "../libAACdec/newAACDecoder.h"
DRC_ERROR
prepareDrcGain(HANDLE_DRC_GAIN_DECODER hGainDec,
               HANDLE_UNI_DRC_GAIN hUniDrcGain, const int16_t compress,
               const int16_t boost, const int32_t loudnessNormalizationGainDb,
               const int32_t activeDrcIndex);
#endif
