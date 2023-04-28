

/**************************** AAC decoder library ******************************

   Author(s):   Matthias Hildenbrand

   Description: USAC ACELP LTP filter

*******************************************************************************/
#include "../libFDK/common_fix.h"
#include "newAACDecoder.h"
#include "aac_rom.h"



//----------------------------------------------------------------------------------------------------------------------------------------------------
void Pred_lt4(int32_t exc[],int32_t T0, int32_t frac) {
    int32_t        j;
    int32_t       *x;
    const int32_t *interpol;
    int32_t        L_sumb, L_sumt;

    x = &exc[-T0 - L_INTERPOL2 + 1];

    /* remap frac and x:
             0 -> 3   x (unchanged)
             1 -> 0   x--
             2 -> 1   x--
             3 -> 2   x--
    */

    if(--frac < 0) frac += UP_SAMP;
    else
        x--;

    j = L_SUBFR + 1;
    do {
        int32_t  filt;
        int32_t  x0, x1;
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
        } while(--i != 0);

        L_sumb <<= 1;
        L_sumb = fAddSaturate(L_sumt << 1, L_sumb);
        *exc++ = L_sumb;
    } while(--j != 0);
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void Pred_lt4_postfilter(int32_t exc[]) {
    /*
    exc[i]   = A*exc[i-1] + B*exc[i] + A*exc[i+1]
    exc[i+1] =              A*exc[i] + B*exc[i+1] + A*exc[i+2] ; i = 0:2:62
    */
    int32_t i;
    int32_t sum0, sum1, a_exc0, a_exc1;
    a_exc0 = fMultDiv2(A2, exc[-1]);
    a_exc1 = fMultDiv2(A2, exc[0]);

    /* ARM926: 22 cycles/iteration */
    for(i = 0; i < L_SUBFR; i += 2) {
        sum0 = a_exc0 + fMult(B, exc[i]);
        sum1 = a_exc1 + fMult(B, exc[i + 1]);
        a_exc0 = fMultDiv2(A2, exc[i + 1]);
        a_exc1 = fMultDiv2(A2, exc[i + 2]);
        exc[i] = sum0 + a_exc0;
        exc[i + 1] = sum1 + a_exc1;
    }
    return;
}
