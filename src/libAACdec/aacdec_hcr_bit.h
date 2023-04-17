

/**************************** AAC decoder library ******************************

   Author(s):   Robert Weidner (DSP Solutions)

   Description: HCR Decoder: Bitstream reading prototypes

*******************************************************************************/

#ifndef AACDEC_HCR_BIT_H
#define AACDEC_HCR_BIT_H
#include "newAACDecoder.h"
#include "aacdec_hcr.h"

uint8_t ToggleReadDirection(uint8_t readDirection);

uint32_t HcrGetABitFromBitstream(HANDLE_FDK_BITSTREAM bs, const int32_t bsAnchor,
                             int32_t *pLeftStartOfSegment,
                             int32_t *pRightStartOfSegment, uint8_t readDirection);

#endif /* AACDEC_HCR_BIT_H */
