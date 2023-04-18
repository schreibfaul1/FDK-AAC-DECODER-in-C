

/**************************** AAC decoder library ******************************

   Author(s):   Robert Weidner (DSP Solutions)

   Description: HCR Decoder: Bitstream reading

*******************************************************************************/
#include "newAACDecoder.h"
#include "../libFDK/FDK_bitstream.h"

/*---------------------------------------------------------------------------------------------
        description:  This function toggles the read direction.
-----------------------------------------------------------------------------------------------
        input:        current read direction
-----------------------------------------------------------------------------------------------
        return:       new read direction
--------------------------------------------------------------------------------------------
*/
uint8_t ToggleReadDirection(uint8_t readDirection) {
  if (readDirection == FROM_LEFT_TO_RIGHT) {
    return FROM_RIGHT_TO_LEFT;
  } else {
    return FROM_LEFT_TO_RIGHT;
  }
}

/*---------------------------------------------------------------------------------------------
     description:   This function returns a bit from the bitstream according to
read direction. It is called very often, therefore it makes sense to inline it
(runtime).
-----------------------------------------------------------------------------------------------
        input:    - handle to FDK bitstream
                  - reference value marking start of bitfield
                  - pLeftStartOfSegment
                  - pRightStartOfSegment
                  - readDirection
-----------------------------------------------------------------------------------------------
        return:   - bit from bitstream
--------------------------------------------------------------------------------------------
*/
uint32_t HcrGetABitFromBitstream(HANDLE_FDK_BITSTREAM bs, const int32_t bsAnchor,
                             int32_t *pLeftStartOfSegment,
                             int32_t *pRightStartOfSegment, uint8_t readDirection) {
  uint32_t bit;
  int32_t readBitOffset;

  if (readDirection == FROM_LEFT_TO_RIGHT) {
    readBitOffset = (int32_t)FDKgetValidBits(bs) - bsAnchor + *pLeftStartOfSegment;
    if (readBitOffset) {
      FDKpushBiDirectional(bs, readBitOffset);
    }

    bit = FDKreadBits(bs, 1);

    *pLeftStartOfSegment += 1;
  } else {
    readBitOffset = (int32_t)FDKgetValidBits(bs) - bsAnchor + *pRightStartOfSegment;
    if (readBitOffset) {
      FDKpushBiDirectional(bs, readBitOffset);
    }

    /* to be replaced with a brother function of FDKreadBits() */
    bit = FDKreadBits(bs, 1);
    FDKpushBack(bs, 2);

    *pRightStartOfSegment -= 1;
  }

  return (bit);
}
