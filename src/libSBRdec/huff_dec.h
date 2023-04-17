

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  Huffman Decoder
*/
#ifndef HUFF_DEC_H
#define HUFF_DEC_H
#include "../libAACdec/newAACDecoder.h"
#include "sbrdecoder.h"
#include "../libFDK/FDK_bitstream.h"

typedef const int8_t (*Huffman)[2];

int32_t DecodeHuffmanCW(Huffman h, HANDLE_FDK_BITSTREAM hBitBuf);

#endif
