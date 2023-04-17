

/**************************** AAC decoder library ******************************

   Author(s):   Matthias Hildenbrand

   Description: USAC ACELP LTP filter

*******************************************************************************/
#include "newAACDecoder.h"
#include "usacdec_ace_ltp.h"
#include "../libFDK/common_fix.h"



static const int32_t Pred_lt4_inter4_2[UP_SAMP][L_INTERPOL2] = {
    {(int32_t)0x0000FFFC, (int32_t)0x0008FFFC, (int32_t)0xFFEB004C, (int32_t)0xFF50014A,
     (int32_t)0xFDD90351, (int32_t)0xFB2A06CD, (int32_t)0xF6920D46, (int32_t)0xEBB42B35,
     (int32_t)0x6D9EEF39, (int32_t)0x0618FE0F, (int32_t)0xFFE00131, (int32_t)0xFE5501C5,
     (int32_t)0xFE5E015D, (int32_t)0xFEF700B6, (int32_t)0xFF920037, (int32_t)0xFFEC0003},
    {(int32_t)0x0002FFF2, (int32_t)0x0026FFBD, (int32_t)0x005DFF98, (int32_t)0x0055FFEF,
     (int32_t)0xFF89015F, (int32_t)0xFD3A04E5, (int32_t)0xF7D90DAA, (int32_t)0xE67A50EE,
     (int32_t)0x50EEE67A, (int32_t)0x0DAAF7D9, (int32_t)0x04E5FD3A, (int32_t)0x015FFF89,
     (int32_t)0xFFEF0055, (int32_t)0xFF98005D, (int32_t)0xFFBD0026, (int32_t)0xFFF20002},
    {(int32_t)0x0003FFEC, (int32_t)0x0037FF92, (int32_t)0x00B6FEF7, (int32_t)0x015DFE5E,
     (int32_t)0x01C5FE55, (int32_t)0x0131FFE0, (int32_t)0xFE0F0618, (int32_t)0xEF396D9E,
     (int32_t)0x2B35EBB4, (int32_t)0x0D46F692, (int32_t)0x06CDFB2A, (int32_t)0x0351FDD9,
     (int32_t)0x014AFF50, (int32_t)0x004CFFEB, (int32_t)0xFFFC0008, (int32_t)0xFFFC0000},
    {(int32_t)0x0002FFF2, (int32_t)0x002BFF9E, (int32_t)0x00B9FECE, (int32_t)0x01CFFD75,
     (int32_t)0x035EFBC1, (int32_t)0x0521FA0C, (int32_t)0x06AAF8C9, (int32_t)0x07907852,
     (int32_t)0x0790F8C9, (int32_t)0x06AAFA0C, (int32_t)0x0521FBC1, (int32_t)0x035EFD75,
     (int32_t)0x01CFFECE, (int32_t)0x00B9FF9E, (int32_t)0x002BFFF2, (int32_t)0x00020000}};

void Pred_lt4(int32_t exc[], /* in/out: excitation buffer              */
              int32_t T0,         /* input : integer pitch lag              */
              int32_t frac        /* input : fraction of lag in range 0..3  */
) {
  int32_t j;
  int32_t *x;
  const int32_t *interpol;
  int32_t L_sumb, L_sumt;

  x = &exc[-T0 - L_INTERPOL2 + 1];

  /* remap frac and x:
           0 -> 3   x (unchanged)
           1 -> 0   x--
           2 -> 1   x--
           3 -> 2   x--
  */

  if (--frac < 0)
    frac += UP_SAMP;
  else
    x--;

  j = L_SUBFR + 1;
  do {
    int32_t filt;
    int32_t x0, x1;
    int32_t *xi = x++;
    interpol = Pred_lt4_inter4_2[frac];
    int32_t i = 3;

    filt = *interpol++;
    x0 = *xi++;
    x1 = *xi++;
    L_sumt = fMultDiv2(x0, (int16_t)((int16_t)(filt >> 16)));
    L_sumb = fMultDiv2(x1, (int16_t)((int16_t)filt));
    do {
      filt = *interpol++;
      x0 = *xi++;
      x1 = *xi++;
      L_sumt = fMultAddDiv2(L_sumt, x0, (int16_t)((int16_t)(filt >> 16)));
      L_sumb = fMultAddDiv2(L_sumb, x1, (int16_t)((int16_t)filt));

      filt = *interpol++;
      x0 = *xi++;
      x1 = *xi++;
      L_sumt = fMultAddDiv2(L_sumt, x0, (int16_t)((int16_t)(filt >> 16)));
      L_sumb = fMultAddDiv2(L_sumb, x1, (int16_t)((int16_t)filt));

      filt = *interpol++;
      x0 = *xi++;
      x1 = *xi++;
      L_sumt = fMultAddDiv2(L_sumt, x0, (int16_t)((int16_t)(filt >> 16)));
      L_sumb = fMultAddDiv2(L_sumb, x1, (int16_t)((int16_t)filt));

      filt = *interpol++;
      x0 = *xi++;
      x1 = *xi++;
      L_sumt = fMultAddDiv2(L_sumt, x0, (int16_t)((int16_t)(filt >> 16)));
      L_sumb = fMultAddDiv2(L_sumb, x1, (int16_t)((int16_t)filt));

      filt = *interpol++;
      x0 = *xi++;
      x1 = *xi++;
      L_sumt = fMultAddDiv2(L_sumt, x0, (int16_t)((int16_t)(filt >> 16)));
      L_sumb = fMultAddDiv2(L_sumb, x1, (int16_t)((int16_t)filt));
    } while (--i != 0);

    L_sumb <<= 1;
    L_sumb = fAddSaturate(L_sumt << 1, L_sumb);
    *exc++ = L_sumb;
  } while (--j != 0);
  return;
}

void Pred_lt4_postfilter(int32_t exc[] /* in/out: excitation buffer */
) {
  /*
  exc[i]   = A*exc[i-1] + B*exc[i] + A*exc[i+1]
  exc[i+1] =              A*exc[i] + B*exc[i+1] + A*exc[i+2] ; i = 0:2:62
  */
  int32_t i;
  int32_t sum0, sum1, a_exc0, a_exc1;
  a_exc0 = fMultDiv2(A2, exc[-1]);
  a_exc1 = fMultDiv2(A2, exc[0]);

  /* ARM926: 22 cycles/iteration */
  for (i = 0; i < L_SUBFR; i += 2) {
    sum0 = a_exc0 + fMult(B, exc[i]);
    sum1 = a_exc1 + fMult(B, exc[i + 1]);
    a_exc0 = fMultDiv2(A2, exc[i + 1]);
    a_exc1 = fMultDiv2(A2, exc[i + 2]);
    exc[i] = sum0 + a_exc0;
    exc[i + 1] = sum1 + a_exc1;
  }
  return;
}
