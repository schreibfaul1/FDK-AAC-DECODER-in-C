
/******************* Library for basic calculation routines ********************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file   dct.cpp
  \brief  DCT Implementations
  Library functions to calculate standard DCTs. This will most likely be
  replaced by hand-optimized functions for the specific target processor.

  Three different implementations of the dct type II and the dct type III
  transforms are provided.

  By default implementations which are based on a single, standard complex
  FFT-kernel are used (dctII_f() and dctIII_f()). These are specifically helpful
  in cases where optimized FFT libraries are already available. The FFT used in
  these implementation is FFT rad2 from FDK_tools.

  Of course, one might also use DCT-libraries should they be available. The DCT
  and DST type IV implementations are only available in a version based on a
  complex FFT kernel.
*/

#include <stdlib.h>
#include "../libAACdec/newAACDecoder.h"
#include "dct.h"

#include "FDK_tools_rom.h"
#include "fft.h"


void dct_getTables(const FIXP_SPK_t **ptwiddle, const FIXP_SPK_t **sin_twiddle,
                   int32_t *sin_step, int32_t length) {
  const FIXP_SPK_t *twiddle;
  int32_t ld2_length;

  /* Get ld2 of length - 2 + 1
      -2: because first table entry is window of size 4
      +1: because we already include +1 because of ceil(log2(length)) */
  ld2_length = DFRACT_BITS - 1 - fNormz((int32_t)length) - 1;

  /* Extract sort of "eigenvalue" (the 4 left most bits) of length. */
  switch ((length) >> (ld2_length - 1)) {
    case 0x4: /* radix 2 */
      *sin_twiddle = SineTable1024;
      *sin_step = 1 << (10 - ld2_length);
      twiddle = windowSlopes[0][0][ld2_length - 1];
      break;
    case 0x7: /* 10 ms */
      *sin_twiddle = SineTable480;
      *sin_step = 1 << (8 - ld2_length);
      twiddle = windowSlopes[0][1][ld2_length];
      break;
    case 0x6: /* 3/4 of radix 2 */
      *sin_twiddle = SineTable384;
      *sin_step = 1 << (8 - ld2_length);
      twiddle = windowSlopes[0][2][ld2_length];
      break;
    case 0x5: /* 5/16 of radix 2*/
      *sin_twiddle = SineTable80;
      *sin_step = 1 << (6 - ld2_length);
      twiddle = windowSlopes[0][3][ld2_length];
      break;
    default:
      *sin_twiddle = NULL;
      *sin_step = 0;
      twiddle = NULL;
      break;
  }

  if (ptwiddle != NULL) {
    assert(twiddle != NULL);
    *ptwiddle = twiddle;
  }

  assert(*sin_step > 0);
}

#if !defined(FUNCTION_dct_III)
void dct_III(int32_t *pDat, /*!< pointer to input/output */
             int32_t *tmp,  /*!< pointer to temporal working buffer */
             int32_t L,          /*!< lenght of transform */
             int32_t *pDat_e) {
  const FIXP_SPK_t *sin_twiddle;
  int32_t i;
  int32_t xr, accu1, accu2;
  int32_t inc, index;
  int32_t M = L >> 1;

  assert(L % 4 == 0);
  dct_getTables(NULL, &sin_twiddle, &inc, L);
  inc >>= 1;

  int32_t *pTmp_0 = &tmp[2];
  int32_t *pTmp_1 = &tmp[(M - 1) * 2];

  index = 4 * inc;

  /* This loop performs multiplication for index i (i*inc) */
  for (i = 1; i<M>> 1; i++, pTmp_0 += 2, pTmp_1 -= 2) {
    int32_t accu3, accu4, accu5, accu6;

    cplxMultDiv2(&accu2, &accu1, pDat[L - i], pDat[i], sin_twiddle[i * inc]);
    cplxMultDiv2(&accu4, &accu3, pDat[M + i], pDat[M - i],
                 sin_twiddle[(M - i) * inc]);
    accu3 >>= 1;
    accu4 >>= 1;

    /* This method is better for ARM926, that uses operand2 shifted right by 1
     * always */
    if (2 * i < (M / 2)) {
      cplxMultDiv2(&accu6, &accu5, (accu3 - (accu1 >> 1)),
                   ((accu2 >> 1) + accu4), sin_twiddle[index]);
    } else {
      cplxMultDiv2(&accu6, &accu5, ((accu2 >> 1) + accu4),
                   (accu3 - (accu1 >> 1)), sin_twiddle[index]);
      accu6 = -accu6;
    }
    xr = (accu1 >> 1) + accu3;
    pTmp_0[0] = (xr >> 1) - accu5;
    pTmp_1[0] = (xr >> 1) + accu5;

    xr = (accu2 >> 1) - accu4;
    pTmp_0[1] = (xr >> 1) - accu6;
    pTmp_1[1] = -((xr >> 1) + accu6);

    /* Create index helper variables for (4*i)*inc indexed equivalent values of
     * int16_t tables. */
    if (2 * i < ((M / 2) - 1)) {
      index += 4 * inc;
    } else if (2 * i >= ((M / 2))) {
      index -= 4 * inc;
    }
  }

  xr = fMultDiv2(pDat[M], sin_twiddle[M * inc].v.re); /* cos((PI/(2*L))*M); */
  tmp[0] = ((pDat[0] >> 1) + xr) >> 1;
  tmp[1] = ((pDat[0] >> 1) - xr) >> 1;

  cplxMultDiv2(&accu2, &accu1, pDat[L - (M / 2)], pDat[M / 2],
               sin_twiddle[M * inc / 2]);
  tmp[M] = accu1 >> 1;
  tmp[M + 1] = accu2 >> 1;

  /* dit_fft expects 1 bit scaled input values */
  fft(M, tmp, pDat_e);

  /* ARM926: 12 cycles per 2-iteration, no overhead code by compiler */
  pTmp_1 = &tmp[L];
  for (i = M >> 1; i--;) {
    int32_t tmp1, tmp2, tmp3, tmp4;
    tmp1 = *tmp++;
    tmp2 = *tmp++;
    tmp3 = *--pTmp_1;
    tmp4 = *--pTmp_1;
    *pDat++ = tmp1;
    *pDat++ = tmp3;
    *pDat++ = tmp2;
    *pDat++ = tmp4;
  }

  *pDat_e += 2;
}

void dst_III(int32_t *pDat, /*!< pointer to input/output */
             int32_t *tmp,  /*!< pointer to temporal working buffer */
             int32_t L,          /*!< lenght of transform */
             int32_t *pDat_e) {
  int32_t L2 = L >> 1;
  int32_t i;
  int32_t t;

  /* note: DCT III is reused here, direct DST III implementation might be more
   * efficient */

  /* mirror input */
  for (i = 0; i < L2; i++) {
    t = pDat[i];
    pDat[i] = pDat[L - 1 - i];
    pDat[L - 1 - i] = t;
  }

  /* DCT-III */
  dct_III(pDat, tmp, L, pDat_e);

  /* flip signs at odd indices */
  for (i = 1; i < L; i += 2) pDat[i] = -pDat[i];
}

#endif

#if !defined(FUNCTION_dct_II)
void dct_II(
    int32_t *pDat, /*!< pointer to input/output */
    int32_t *tmp,  /*!< pointer to temporal working buffer */
    int32_t L, /*!< lenght of transform (has to be a multiple of 8 (or 4 in case
              DCT_II_L_MULTIPLE_OF_4_SUPPORT is defined) */
    int32_t *pDat_e) {
  const FIXP_SPK_t *sin_twiddle;
  int32_t accu1, accu2;
  int32_t *pTmp_0, *pTmp_1;

  int32_t i;
  int32_t inc, index = 0;
  int32_t M = L >> 1;

  assert(L % 4 == 0);
  dct_getTables(NULL, &sin_twiddle, &inc, L);
  inc >>= 1;

  {
    for (i = 0; i < M; i++) {
      tmp[i] = pDat[2 * i] >> 1; /* dit_fft expects 1 bit scaled input values */
      tmp[L - 1 - i] =
          pDat[2 * i + 1] >> 1; /* dit_fft expects 1 bit scaled input values */
    }
  }

  fft(M, tmp, pDat_e);

  pTmp_0 = &tmp[2];
  pTmp_1 = &tmp[(M - 1) * 2];

  index = inc * 4;

  for (i = 1; i<M>> 1; i++, pTmp_0 += 2, pTmp_1 -= 2) {
    int32_t a1, a2;
    int32_t accu3, accu4;

    a1 = ((pTmp_0[1] >> 1) + (pTmp_1[1] >> 1));
    a2 = ((pTmp_1[0] >> 1) - (pTmp_0[0] >> 1));

    if (2 * i < (M / 2)) {
      cplxMultDiv2(&accu1, &accu2, a2, a1, sin_twiddle[index]);
    } else {
      cplxMultDiv2(&accu1, &accu2, a1, a2, sin_twiddle[index]);
      accu1 = -accu1;
    }
    accu1 <<= 1;
    accu2 <<= 1;

    a1 = ((pTmp_0[0] >> 1) + (pTmp_1[0] >> 1));
    a2 = ((pTmp_0[1] >> 1) - (pTmp_1[1] >> 1));

    cplxMultDiv2(&accu3, &accu4, (a1 + accu2), -(accu1 + a2),
                 sin_twiddle[i * inc]);
    pDat[L - i] = accu4;
    pDat[i] = accu3;

    cplxMultDiv2(&accu3, &accu4, (a1 - accu2), -(accu1 - a2),
                 sin_twiddle[(M - i) * inc]);
    pDat[M + i] = accu4;
    pDat[M - i] = accu3;

    /* Create index helper variables for (4*i)*inc indexed equivalent values of
     * int16_t tables. */
    if (2 * i < ((M / 2) - 1)) {
      index += 4 * inc;
    } else if (2 * i >= ((M / 2))) {
      index -= 4 * inc;
    }
  }

  cplxMultDiv2(&accu1, &accu2, tmp[M], tmp[M + 1], sin_twiddle[(M / 2) * inc]);
  pDat[L - (M / 2)] = accu2;
  pDat[M / 2] = accu1;

  pDat[0] = (tmp[0] >> 1) + (tmp[1] >> 1);
  pDat[M] = fMult(((tmp[0] >> 1) - (tmp[1] >> 1)),
                  sin_twiddle[M * inc].v.re); /* cos((PI/(2*L))*M); */

  *pDat_e += 2;
}
#endif

#if !defined(FUNCTION_dct_IV)

void dct_IV(int32_t *pDat, int32_t L, int32_t *pDat_e) {
  int32_t sin_step = 0;
  int32_t M = L >> 1;

  const FIXP_SPK_t *twiddle;
  const FIXP_SPK_t *sin_twiddle;

  assert(L >= 4);

  assert(L >= 4);

  dct_getTables(&twiddle, &sin_twiddle, &sin_step, L);

  {
    int32_t * pDat_0 = &pDat[0];
    int32_t * pDat_1 = &pDat[L - 2];
    int32_t i;

    /* 29 cycles on ARM926 */
    for (i = 0; i < M - 1; i += 2, pDat_0 += 2, pDat_1 -= 2) {
      int32_t accu1, accu2, accu3, accu4;

      accu1 = pDat_1[1];
      accu2 = pDat_0[0];
      accu3 = pDat_0[1];
      accu4 = pDat_1[0];

      cplxMultDiv2(&accu1, &accu2, accu1, accu2, twiddle[i]);
      cplxMultDiv2(&accu3, &accu4, accu4, accu3, twiddle[i + 1]);

      pDat_0[0] = accu2 >> 1;
      pDat_0[1] = accu1 >> 1;
      pDat_1[0] = accu4 >> 1;
      pDat_1[1] = -(accu3 >> 1);
    }
    if (M & 1) {
      int32_t accu1, accu2;

      accu1 = pDat_1[1];
      accu2 = pDat_0[0];

      cplxMultDiv2(&accu1, &accu2, accu1, accu2, twiddle[i]);

      pDat_0[0] = accu2 >> 1;
      pDat_0[1] = accu1 >> 1;
    }
  }

  fft(M, pDat, pDat_e);

  {
    int32_t * pDat_0 = &pDat[0];
    int32_t * pDat_1 = &pDat[L - 2];
    int32_t accu1, accu2, accu3, accu4;
    int32_t idx, i;

    /* Sin and Cos values are 0.0f and 1.0f */
    accu1 = pDat_1[0];
    accu2 = pDat_1[1];

    pDat_1[1] = -pDat_0[1];

    /* 28 cycles for ARM926 */
    for (idx = sin_step, i = 1; i<(M + 1)>> 1; i++, idx += sin_step) {
      FIXP_SPK_t twd = sin_twiddle[idx];
      cplxMult(&accu3, &accu4, accu1, accu2, twd);
      pDat_0[1] = accu3;
      pDat_1[0] = accu4;

      pDat_0 += 2;
      pDat_1 -= 2;

      cplxMult(&accu3, &accu4, pDat_0[1], pDat_0[0], twd);

      accu1 = pDat_1[0];
      accu2 = pDat_1[1];

      pDat_1[1] = -accu3;
      pDat_0[0] = accu4;
    }

    if ((M & 1) == 0) {
      /* Last Sin and Cos value pair are the same */
    accu1 = fMult(accu1, (int16_t) 23170);
    accu2 = fMult(accu2, (int16_t) 23170);

      pDat_1[0] = accu1 + accu2;
      pDat_0[1] = accu1 - accu2;
    }
  }

  /* Add twiddeling scale. */
  *pDat_e += 2;
}
#endif /* defined (FUNCTION_dct_IV) */

#if !defined(FUNCTION_dst_IV)
void dst_IV(int32_t *pDat, int32_t L, int32_t *pDat_e) {
  int32_t sin_step = 0;
  int32_t M = L >> 1;

  const FIXP_SPK_t *twiddle;
  const FIXP_SPK_t *sin_twiddle;

  assert(L >= 4);

  assert(L >= 4);

  dct_getTables(&twiddle, &sin_twiddle, &sin_step, L);

  {
    int32_t * pDat_0 = &pDat[0];
    int32_t * pDat_1 = &pDat[L - 2];
    int32_t i;

    /* 34 cycles on ARM926 */
    for (i = 0; i < M - 1; i += 2, pDat_0 += 2, pDat_1 -= 2) {
      int32_t accu1, accu2, accu3, accu4;

      accu1 = pDat_1[1] >> 1;
      accu2 = -(pDat_0[0] >> 1);
      accu3 = pDat_0[1] >> 1;
      accu4 = -(pDat_1[0] >> 1);

      cplxMultDiv2(&accu1, &accu2, accu1, accu2, twiddle[i]);
      cplxMultDiv2(&accu3, &accu4, accu4, accu3, twiddle[i + 1]);

      pDat_0[0] = accu2;
      pDat_0[1] = accu1;
      pDat_1[0] = accu4;
      pDat_1[1] = -accu3;
    }
    if (M & 1) {
      int32_t accu1, accu2;

      accu1 = pDat_1[1];
      accu2 = -pDat_0[0];

      cplxMultDiv2(&accu1, &accu2, accu1, accu2, twiddle[i]);

      pDat_0[0] = accu2 >> 1;
      pDat_0[1] = accu1 >> 1;
    }
  }

  fft(M, pDat, pDat_e);

  {
    int32_t * pDat_0;
    int32_t * pDat_1;
    int32_t accu1, accu2, accu3, accu4;
    int32_t idx, i;

    pDat_0 = &pDat[0];
    pDat_1 = &pDat[L - 2];

    /* Sin and Cos values are 0.0f and 1.0f */
    accu1 = pDat_1[0];
    accu2 = pDat_1[1];

    pDat_1[1] = -pDat_0[0];
    pDat_0[0] = pDat_0[1];

    for (idx = sin_step, i = 1; i<(M + 1)>> 1; i++, idx += sin_step) {
      FIXP_SPK_t twd = sin_twiddle[idx];

      cplxMult(&accu3, &accu4, accu1, accu2, twd);
      pDat_1[0] = -accu3;
      pDat_0[1] = -accu4;

      pDat_0 += 2;
      pDat_1 -= 2;

      cplxMult(&accu3, &accu4, pDat_0[1], pDat_0[0], twd);

      accu1 = pDat_1[0];
      accu2 = pDat_1[1];

      pDat_0[0] = accu3;
      pDat_1[1] = -accu4;
    }

    if ((M & 1) == 0) {
      /* Last Sin and Cos value pair are the same */
      accu1 = fMult(accu1, (int16_t)    23170);
      accu2 = fMult(accu2, (int16_t)    23170);

      pDat_0[1] = -accu1 - accu2;
      pDat_1[0] = accu2 - accu1;
    }
  }

  /* Add twiddeling scale. */
  *pDat_e += 2;
}
#endif /* !defined(FUNCTION_dst_IV) */
