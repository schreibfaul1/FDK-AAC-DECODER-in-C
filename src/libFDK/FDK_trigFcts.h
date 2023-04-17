

/******************* Library for basic calculation routines ********************

   Author(s):   Haricharan Lakshman, Manuel Jander

   Description: Trigonometric functions fixed point fractional implementation.

*******************************************************************************/

#if !defined(FDK_TRIGFCTS_H)
#define FDK_TRIGFCTS_H
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"

#include "FDK_tools_rom.h"

/* Fixed point precision definitions */
#define Q(format) ((int32_t)(((int32_t)1) << (format)))

#ifndef M_PI
#define M_PI (3.14159265358979323846f)
#endif

/*!
 * Inverse tangent function.
 */

/* --- fixp_atan() ----    */
#define Q_ATANINP (25)  // Input in q25, Output in q30
#define Q_ATANOUT (30)
#define ATI_SF ((DFRACT_BITS - 1) - Q_ATANINP) /* 6  */
#define ATI_SCALE ((float)(1 << ATI_SF))
#define ATO_SF ((DFRACT_BITS - 1) - Q_ATANOUT) /* 1   ] -pi/2 .. pi/2 [ */
#define ATO_SCALE ((float)(1 << ATO_SF))
/* --- fixp_atan2() ---    */
#define Q_ATAN2OUT (29)
#define AT2O_SF ((DFRACT_BITS - 1) - Q_ATAN2OUT) /* 2   ] -pi   .. pi   ] */
#define AT2O_SCALE ((float)(1 << AT2O_SF))
// --------------------

int32_t fixp_atan(int32_t x);
int32_t fixp_atan2(int32_t y, int32_t x);

int32_t fixp_cos(int32_t x, int32_t scale);
int32_t fixp_sin(int32_t x, int32_t scale);

#define FIXP_COS_SIN

#include "FDK_tools_rom.h"

#define SINETAB SineTable512
#define LD 9

#ifndef FUNCTION_inline_fixp_cos_sin

#define FUNCTION_inline_fixp_cos_sin

/*
 * Calculates coarse lookup index and sign for sine.
 * Returns delta x residual.
 */
static inline int32_t fixp_sin_cos_residual_inline(int32_t x, int32_t scale,
                                                    int32_t *sine,
                                                    int32_t *cosine) {
  int32_t residual;
  int32_t s;
  int32_t shift = (31 - scale - LD - 1);
  int32_t ssign = 1;
  int32_t csign = 1;

  residual = fMult(x, FL2FXCONST_DBL(1.0 / M_PI));
  s = ((int32_t)residual) >> shift;

  residual &= ((1 << shift) - 1);
  residual = fMult(residual, FL2FXCONST_DBL(M_PI / 4.0)) << 2;
  residual <<= scale;

  /* Sine sign symmetry */
  if (s & ((1 << LD) << 1)) {
    ssign = -ssign;
  }
  /* Cosine sign symmetry */
  if ((s + (1 << LD)) & ((1 << LD) << 1)) {
    csign = -csign;
  }

  s = fAbs(s);

  s &= (((1 << LD) << 1) - 1); /* Modulo PI */

  if (s > (1 << LD)) {
    s = ((1 << LD) << 1) - s;
  }

  {
    int32_t sl, cl;
    /* Because of packed table */
    if (s > (1 << (LD - 1))) {
      FIXP_SPK_t tmp;
      /* Cosine/Sine simetry for angles greater than PI/4 */
      s = (1 << LD) - s;
      tmp = SINETAB[s];
      sl = (int32_t)tmp.v.re;
      cl = (int32_t)tmp.v.im;
    } else {
      FIXP_SPK_t tmp;
      tmp = SINETAB[s];
      sl = (int32_t)tmp.v.im;
      cl = (int32_t)tmp.v.re;
    }

#ifdef SINETABLE_16BIT
    *sine = (int32_t)((sl * ssign) << (DFRACT_BITS - FRACT_BITS));
    *cosine = (int32_t)((cl * csign) << (DFRACT_BITS - FRACT_BITS));
#else
    /* scale down by 1 for overflow prevention. This is undone at the calling
     * function. */
    *sine = (int32_t)(sl * ssign) >> 1;
    *cosine = (int32_t)(cl * csign) >> 1;
#endif
  }

  return residual;
}

/**
 * \brief Calculate cosine and sine value each of 2 angles different angle
 * values.
 * \param x1 first angle value
 * \param x2 second angle value
 * \param scale exponent of x1 and x2
 * \param out pointer to 4 int32_t locations, were the values cos(x1), sin(x1),
 * cos(x2), sin(x2) will be stored into.
 */
static inline void inline_fixp_cos_sin(int32_t x1, int32_t x2,
                                       const int32_t scale, int32_t *out) {
  int32_t residual, error0, error1, sine, cosine;
  residual = fixp_sin_cos_residual_inline(x1, scale, &sine, &cosine);
  error0 = fMultDiv2(sine, residual);
  error1 = fMultDiv2(cosine, residual);

#ifdef SINETABLE_16BIT
  *out++ = cosine - (error0 << 1);
  *out++ = sine + (error1 << 1);
#else
  /* Undo downscaling by 1 which was done at fixp_sin_cos_residual_inline */
  *out++ = SATURATE_LEFT_SHIFT(cosine - (error0 << 1), 1, DFRACT_BITS);
  *out++ = SATURATE_LEFT_SHIFT(sine + (error1 << 1), 1, DFRACT_BITS);
#endif

  residual = fixp_sin_cos_residual_inline(x2, scale, &sine, &cosine);
  error0 = fMultDiv2(sine, residual);
  error1 = fMultDiv2(cosine, residual);

#ifdef SINETABLE_16BIT
  *out++ = cosine - (error0 << 1);
  *out++ = sine + (error1 << 1);
#else
  *out++ = SATURATE_LEFT_SHIFT(cosine - (error0 << 1), 1, DFRACT_BITS);
  *out++ = SATURATE_LEFT_SHIFT(sine + (error1 << 1), 1, DFRACT_BITS);
#endif
}
#endif

#endif /* !defined(FDK_TRIGFCTS_H) */
