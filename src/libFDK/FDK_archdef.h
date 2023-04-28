

/******************* Library for basic calculation routines ********************

   Author(s):

   Description:

*******************************************************************************/

#ifndef FDK_ARCHDEF_H
#define FDK_ARCHDEF_H
#include "../libAACdec/newAACDecoder.h"
/* Unify some few toolchain specific defines to avoid having large "or" macro
 * contraptions all over the source code. */

/* Use single macro (the GCC built in macro) for architecture identification
 * independent of the particular toolchain */
#if defined(__i386__) || defined(__i486__) || defined(__i586__) ||  \
    defined(__i686__) || (defined(_MSC_VER) && defined(_M_IX86)) || \
    (defined(_MSC_VER) && defined(_M_X64)) || defined(__x86_64__)
#define __x86__
#endif

#if defined(_M_ARM) && !defined(__arm__) || defined(__aarch64__) || defined(_M_ARM64)
#define __arm__
#endif

#if defined(_ARCH_PPC) && !defined(__powerpc__)
#define __powerpc__ 1
#endif

#if (__TARGET_ARCH_ARM == 5) || defined(__TARGET_FEATURE_DSPMUL) || \
    (_M_ARM == 5) || defined(__ARM_ARCH_5TEJ__) || defined(__ARM_ARCH_7EM__)
/* Define __ARM_ARCH_5TE__ if armv5te features are supported  */
#define __ARM_ARCH_5TE__
#endif

#if (__TARGET_ARCH_ARM == 6) || defined(__ARM_ARCH_6J__) || \
    defined(__ARM_ARCH_6ZK__)
/* Define __ARM_ARCH_6__ if the armv6 intructions are being supported. */
#define __ARM_ARCH_5TE__
#define __ARM_ARCH_6__
#endif

#if defined(__TARGET_ARCH_7_R) || defined(__ARM_ARCH_7R__)
/* Define __ARM_ARCH_7_A__ if the armv7 intructions are being supported. */
#define __ARM_ARCH_5TE__
#define __ARM_ARCH_6__
#define __ARM_ARCH_7_R__
#endif

#if defined(__TARGET_ARCH_7_A) || defined(__ARM_ARCH_7A__) || \
    ((__ARM_ARCH == 8) && (__ARM_32BIT_STATE == 1))
/* Define __ARM_ARCH_7_A__ if the armv7 intructions are being supported. */
#define __ARM_ARCH_5TE__
#define __ARM_ARCH_6__
#define __ARM_ARCH_7_A__
#endif

#if defined(__TARGET_ARCH_7_M) || defined(__ARM_ARCH_7_M__)
/* Define __ARM_ARCH_7M__ if the ARMv7-M instructions are being supported, e.g.
 * Cortex-M3. */
#define __ARM_ARCH_7M__
#endif

#if defined(__TARGET_ARCH_7E_M) || defined(__ARM_ARCH_7E_M__)
/* Define __ARM_ARCH_7EM__ if the ARMv7-ME instructions are being supported,
 * e.g. Cortex-M4. */
#define __ARM_ARCH_7EM__
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define __ARM_ARCH_8__
#endif

// #ifdef _M_ARM
// #include "armintr.h"
// #endif

/* Define preferred Multiplication type */

#if defined(__mips__)
#define ARCH_PREFER_MULT_16x16
#undef SINETABLE_16BIT
#undef POW2COEFF_16BIT
#undef LDCOEFF_16BIT
#undef WINDOWTABLE_16BIT

#elif defined(__arm__) && defined(__ARM_ARCH_8__)
#define ARCH_PREFER_MULT_32x16
#define SINETABLE_16BIT
#define POW2COEFF_16BIT
#define LDCOEFF_16BIT
#define WINDOWTABLE_16BIT

#elif defined(__arm__) && defined(__ARM_ARCH_5TE__)
#define ARCH_PREFER_MULT_32x16
#define SINETABLE_16BIT
#define POW2COEFF_16BIT
#define LDCOEFF_16BIT
#define WINDOWTABLE_16BIT

#elif defined(__arm__) && defined(__ARM_ARCH_7M__)
#define ARCH_PREFER_MULT_32x16
#define SINETABLE_16BIT
#define POW2COEFF_16BIT
#define LDCOEFF_16BIT
#define WINDOWTABLE_16BIT

#elif defined(__arm__) && defined(__ARM_ARCH_7EM__)
#define ARCH_PREFER_MULT_32x32
#define ARCH_PREFER_MULT_32x16
#define SINETABLE_16BIT
#define POW2COEFF_16BIT
#define LDCOEFF_16BIT
#define WINDOWTABLE_16BIT

#elif defined(__arm__) && !defined(__ARM_ARCH_5TE__)
#define ARCH_PREFER_MULT_16x16
#undef SINETABLE_16BIT
#undef WINDOWTABLE_16BIT
#undef POW2COEFF_16BIT
#undef LDCOEFF_16BIT

#elif defined(__x86__)
#define ARCH_PREFER_MULT_32x16
#define SINETABLE_16BIT
#define WINDOWTABLE_16BIT
#define POW2COEFF_16BIT
#define LDCOEFF_16BIT

#elif defined(__powerpc__)
#define ARCH_PREFER_MULT_32x32
#define ARCH_PREFER_MULT_32x16
#define SINETABLE_16BIT
#define POW2COEFF_16BIT
#define LDCOEFF_16BIT
#define WINDOWTABLE_16BIT

#elif defined(ESP32)
#define ARCH_PREFER_MULT_16x16
#undef SINETABLE_16BIT
#undef WINDOWTABLE_16BIT
#undef POW2COEFF_16BIT
#undef LDCOEFF_16BIT

#else
#warning >>>> Please set architecture characterization defines for your platform (FDK_HIGH_PERFORMANCE)! <<<<

#endif /* Architecture switches */


#endif /* FDK_ARCHDEF_H */
