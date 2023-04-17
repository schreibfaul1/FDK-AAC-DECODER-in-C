
/**************************** AAC decoder library ******************************

   Author(s):   Robert Weidner (DSP Solutions)

   Description: RVLC Decoder: Bitstream reading

*******************************************************************************/

#ifndef RVLCBIT_H
#define RVLCBIT_H
#include "newAACDecoder.h"
#include "rvlc.h"

uint8_t rvlcReadBitFromBitstream(HANDLE_FDK_BITSTREAM bs, const int32_t bsAnchor,
                               int32_t *pPosition, uint8_t readDirection);

#endif /* RVLCBIT_H */
