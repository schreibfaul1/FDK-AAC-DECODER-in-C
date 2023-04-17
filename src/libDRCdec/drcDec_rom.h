
/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#ifndef DRCDEC_ROM_H
#define DRCDEC_ROM_H
#include <stdint.h>
#include "../libAACdec/newAACDecoder.h"
extern const int8_t deltaGain_codingProfile_0_1_huffman[24][2];
extern const int8_t deltaGain_codingProfile_2_huffman[48][2];

extern const int16_t slopeSteepness[];
extern const int8_t slopeSteepness_huffman[14][2];

extern const int32_t downmixCoeff[];
extern const int32_t downmixCoeffV1[];

extern const CUSTOM_DRC_CHAR_SIGMOID cicpDrcCharSigmoidLeft[];
extern const CUSTOM_DRC_CHAR_SIGMOID cicpDrcCharSigmoidRight[];
extern const CUSTOM_DRC_CHAR_NODES cicpDrcCharNodesLeft[];
extern const CUSTOM_DRC_CHAR_NODES cicpDrcCharNodesRight[];

#endif
