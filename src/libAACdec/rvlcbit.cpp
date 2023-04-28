

/**************************** AAC decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  RVLC bitstream reading
  \author Robert Weidner
*/
#include "newAACDecoder.h"

/*---------------------------------------------------------------------------------------------
     function:     rvlcReadBitFromBitstream

     description:  This function returns a bit from the bitstream according to
read direction. It is called very often, therefore it makes sense to inline it
(runtime).
-----------------------------------------------------------------------------------------------
        input:     - bitstream
                   - pPosition
                   - readDirection
-----------------------------------------------------------------------------------------------
        return:    - bit from bitstream
--------------------------------------------------------------------------------------------
*/

uint8_t rvlcReadBitFromBitstream(HANDLE_FDK_BITSTREAM bs, const int32_t bsAnchor,
                               int32_t *pPosition, uint8_t readDirection) {
  uint32_t bit;
  int32_t readBitOffset = (int32_t)FDKgetValidBits(bs) - bsAnchor + *pPosition;

  if (readBitOffset) {
    FDKpushBiDirectional(bs, readBitOffset);
  }

  if (readDirection == FWD) {
    bit = FDKreadBits(bs, 1);

    *pPosition += 1;
  } else {
    /* to be replaced with a brother function of FDKreadBits() */
    bit = FDKreadBits(bs, 1);
    FDKpushBack(bs, 2);

    *pPosition -= 1;
  }

  return (bit);
}
