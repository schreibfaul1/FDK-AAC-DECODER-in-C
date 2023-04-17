

/******************* Library for basic calculation routines ********************

   Author(s):

   Description:

*******************************************************************************/

#ifndef SCRAMBLE_H
#define SCRAMBLE_H
#include <assert.h>
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"

// #if defined(__arm__)
// #include "arm/scramble_arm.h"

// #elif defined(__mips__) && defined(__mips_dsp) && !defined(__mips16)
// #include "mips/scramble_mips.h"

// #endif

/*****************************************************************************

    functionname: scramble
    description:  bitreversal of input data
    returns:
    input:
    output:

*****************************************************************************/
#if !defined(FUNCTION_scramble)

/* default scramble functionality */
inline void scramble(int32_t *x, int32_t length) {
  int32_t m, k, j;
  assert(!(((int32_t)(int64_t)x) & (ALIGNMENT_DEFAULT - 1)));

  for (m = 1, j = 0; m < length - 1; m++) {
    {
      for (k = length >> 1; (!((j ^= k) & k)); k >>= 1)
        ;
    }

    if (j > m) {
      int32_t tmp;
      tmp = x[2 * m];
      x[2 * m] = x[2 * j];
      x[2 * j] = tmp;

      tmp = x[2 * m + 1];
      x[2 * m + 1] = x[2 * j + 1];
      x[2 * j + 1] = tmp;
    }
  }
}
#endif /* !defined(FUNCTION_scramble) */

#endif /* SCRAMBLE_H */
