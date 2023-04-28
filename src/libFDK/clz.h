

/******************* Library for basic calculation routines ********************

   Author(s):   Marc Gayer

   Description: fixed point intrinsics

*******************************************************************************/

#if !defined(CLZ_H)
#define CLZ_H
#include "../libAACdec/newAACDecoder.h"


// #if defined(__arm__)
// #include "arm/clz_arm.h"

// #elif defined(__mips__)
// #include "mips/clz_mips.h"

// #elif defined(__x86__)
// #include "x86/clz_x86.h"

// #elif defined(__powerpc__)
// #include "ppc/clz_ppc.h"

// #endif /* all cores */

/*************************************************************************
 *************************************************************************
    Software fallbacks for missing functions.
**************************************************************************
**************************************************************************/

#if !defined(FUNCTION_fixnormz_S)
#ifdef FUNCTION_fixnormz_D
inline int32_t fixnormz_S(int16_t a) {
  if (a < 0) {
    return 0;
  }
  return fixnormz_D((int32_t)(a)) - 16;
}
#else
inline int32_t fixnormz_S(int16_t a) {
  int32_t leadingBits = 0;
  a = ~a;
  while (a & 0x8000) {
    leadingBits++;
    a <<= 1;
  }

  return (leadingBits);
}
#endif
#endif

#if !defined(FUNCTION_fixnormz_D)
inline int32_t fixnormz_D(int32_t a) {
  int32_t leadingBits = 0;
  a = ~a;
  while (a & 0x80000000) {
    leadingBits++;
    a <<= 1;
  }

  return (leadingBits);
}
#endif

/*****************************************************************************

    functionname: fixnorm_D
    description:  Count leading ones or zeros of operand val for dfract/int32_t int32_t
values. Return this value minus 1. Return 0 if operand==0.
*****************************************************************************/
#if !defined(FUNCTION_fixnorm_S)
#ifdef FUNCTION_fixnorm_D
inline int32_t fixnorm_S(int16_t val) {
  if (val == (int16_t)0) {
    return 0;
  }
  return fixnorm_D((int32_t)(val)) - 16;
}
#else
inline int32_t fixnorm_S(int16_t val) {
  int32_t leadingBits = 0;
  if (val != (int16_t)0) {
    if (val < (int16_t)0) {
      val = ~val;
    }
    leadingBits = fixnormz_S(val) - 1;
  }
  return (leadingBits);
}
#endif
#endif

#if !defined(FUNCTION_fixnorm_D)
inline int32_t fixnorm_D(int32_t val) {
  int32_t leadingBits = 0;
  if (val != (int32_t)0) {
    if (val < (int32_t)0) {
      val = ~val;
    }
    leadingBits = fixnormz_D(val) - 1;
  }
  return (leadingBits);
}
#endif

#endif /* CLZ_H */
