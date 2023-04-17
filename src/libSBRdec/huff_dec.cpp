

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  Huffman Decoder
*/
#include "../libAACdec/newAACDecoder.h"
#include "huff_dec.h"

/***************************************************************************/
/*!
  \brief     Decodes one huffman code word

  Reads bits from the bitstream until a valid codeword is found.
  The table entries are interpreted either as index to the next entry
  or - if negative - as the codeword.

  \return    decoded value

  \author

****************************************************************************/
int32_t DecodeHuffmanCW(Huffman h, /*!< pointer to huffman codebook table */
                    HANDLE_FDK_BITSTREAM hBs) /*!< Handle to Bitbuffer */
{
  int8_t index = 0;
  int32_t value, bit;

  while (index >= 0) {
    bit = FDKreadBits(hBs, 1);
    index = h[index][bit];
  }

  value = index + 64; /* Add offset */

  return value;
}
