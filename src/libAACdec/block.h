
/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: int32_t/int16_t-block decoding

*******************************************************************************/

#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"

#include "channelinfo.h"
#include "../libFDK/FDK_bitstream.h"

/* PNS (of block) */


static inline
int32_t EvaluatePower43(int32_t *pValue, uint32_t lsb) {
  int32_t value;
  uint32_t freeBits;
  uint32_t exponent;

  value = *pValue;
  freeBits = fNormz(value);
  exponent = DFRACT_BITS - freeBits;
  assert(exponent < 14);

  uint32_t x = (((int32_t)value << freeBits) >> 19);
  uint32_t tableIndex = (x & 0x0FFF) >> 4;
  int32_t invQVal;

  x = x & 0x0F;

  uint32_t r0 = (int32_t)InverseQuantTable[tableIndex + 0];
  uint32_t r1 = (int32_t)InverseQuantTable[tableIndex + 1];
  uint16_t nx = 16 - x;
  uint32_t temp = (r0)*nx + (r1)*x;
  invQVal = (int32_t)temp;

  assert(lsb < 4);
  *pValue = fMultDiv2(invQVal, MantissaTable[lsb][exponent]);

  /* + 1 compensates fMultDiv2(). */
  return ExponentTable[lsb][exponent] + 1;
}

/* Recalculate gain */
int32_t get_gain(const int32_t *x, const int32_t *y, int32_t n);

/**
 * \brief determine the required shift scale for the given quantized value and
 * scale (factor % 4) value.
 */
static inline int32_t GetScaleFromValue(int32_t value, uint32_t lsb) {
  if (value != (int32_t)0) {
    int32_t scale = EvaluatePower43(&value, lsb);
    return CntLeadingZeros(value) - scale - 2;
  } else
    return 0; /* Return zero, because its useless to scale a zero value, saves
                 workload and avoids scaling overshifts. */
}

/*!
  \brief Read huffman codeword

  The function reads the huffman codeword from the bitstream and
  returns the index value.

  \return  index value
*/
inline int32_t CBlock_DecodeHuffmanWord(
    HANDLE_FDK_BITSTREAM bs,        /*!< pointer to bitstream */
    const CodeBookDescription_t *hcb) /*!< pointer to codebook description */
{
  uint32_t val;
  uint32_t index = 0;
  const uint16_t(*CodeBook)[HuffmanEntries] = hcb->CodeBook;

  while (1) {
    val = CodeBook[index]
                  [FDKreadBits(bs, HuffmanBits)]; /* Expensive memory access */

    if ((val & 1) == 0) {
      index = val >> 2;
      continue;
    } else {
      if (val & 2) {
        FDKpushBackCache(bs, 1);
      }

      val >>= 2;
      break;
    }
  }

  return val;
}
inline int32_t CBlock_DecodeHuffmanWordCB(
    HANDLE_FDK_BITSTREAM bs, /*!< pointer to bitstream */
    const uint16_t (
        *CodeBook)[HuffmanEntries]) /*!< pointer to codebook description */
{
  uint32_t index = 0;

  while (1) {
    index = CodeBook[index][FDKread2Bits(bs)]; /* Expensive memory access */
    if (index & 1) break;
    index >>= 2;
  }
  if (index & 2) {
    FDKpushBackCache(bs, 1);
  }
  return index >> 2;
}

#endif /* #ifndef BLOCK_H */
