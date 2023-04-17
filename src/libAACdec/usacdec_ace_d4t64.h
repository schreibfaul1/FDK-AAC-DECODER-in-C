

/**************************** AAC decoder library ******************************

   Author(s):

   Description: ACELP

*******************************************************************************/

#ifndef USACDEC_ACE_D4T64_H
#define USACDEC_ACE_D4T64_H
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"

/* Data type definition for the fixed codebook vector */
#define FIXP_COD int16_t
#define FX_COD2FX_DBL(x) (FX_SGL2FX_DBL(x))
#define FX_DBL2FX_COD(x) FX_DBL2FX_SGL((x) + (int32_t)0x8000)
#define FX_SGL2FX_COD(x) (x)
#define COD_BITS FRACT_BITS

void D_ACELP_decode_4t64(int16_t index[], int32_t nbits, FIXP_COD code[]);

#endif /* USACDEC_ACE_D4T64_H */
