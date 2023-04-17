

/******************* Library for basic calculation routines ********************

   Author(s):   M. Lohwasser, M. Gayer

   Description:

*******************************************************************************/

#include "fft_rad2.h"
#include  <stdio.h>
#include "../libAACdec/newAACDecoder.h"
#include "scramble.h"

#define __FFT_RAD2_CPP__

// #if defined(__arm__)
// #include "arm/fft_rad2_arm.cpp"

// #elif defined(__GNUC__) && defined(__mips__) && defined(__mips_dsp) && !defined(__mips16)
// #include "mips/fft_rad2_mips.cpp"

// #endif

/*****************************************************************************

    functionname: dit_fft (analysis)
    description:  dit-tukey-algorithm
                  scrambles data at entry
                  i.e. loop is made with scrambled data
    returns:
    input:
    output:

*****************************************************************************/

#ifndef FUNCTION_dit_fft
void dit_fft(int32_t *x, const int32_t ldn, const FIXP_SPK_t *trigdata,
             const int32_t trigDataSize) {
  const int32_t n = 1 << ldn;
  int32_t trigstep, i, ldm;


  scramble(x, n);
  /*
   * 1+2 stage radix 4
   */

  for (i = 0; i < n * 2; i += 8) {
    int32_t a00, a10, a20, a30;
    a00 = (x[i + 0] + x[i + 2]) >> 1; /* Re A + Re B */
    a10 = (x[i + 4] + x[i + 6]) >> 1; /* Re C + Re D */
    a20 = (x[i + 1] + x[i + 3]) >> 1; /* Im A + Im B */
    a30 = (x[i + 5] + x[i + 7]) >> 1; /* Im C + Im D */

    x[i + 0] = a00 + a10; /* Re A' = Re A + Re B + Re C + Re D */
    x[i + 4] = a00 - a10; /* Re C' = Re A + Re B - Re C - Re D */
    x[i + 1] = a20 + a30; /* Im A' = Im A + Im B + Im C + Im D */
    x[i + 5] = a20 - a30; /* Im C' = Im A + Im B - Im C - Im D */

    a00 = a00 - x[i + 2]; /* Re A - Re B */
    a10 = a10 - x[i + 6]; /* Re C - Re D */
    a20 = a20 - x[i + 3]; /* Im A - Im B */
    a30 = a30 - x[i + 7]; /* Im C - Im D */

    x[i + 2] = a00 + a30; /* Re B' = Re A - Re B + Im C - Im D */
    x[i + 6] = a00 - a30; /* Re D' = Re A - Re B - Im C + Im D */
    x[i + 3] = a20 - a10; /* Im B' = Im A - Im B - Re C + Re D */
    x[i + 7] = a20 + a10; /* Im D' = Im A - Im B + Re C - Re D */
  }

  for (ldm = 3; ldm <= ldn; ++ldm) {
    int32_t m = (1 << ldm);
    int32_t mh = (m >> 1);
    int32_t j, r;

    trigstep = ((trigDataSize << 2) >> ldm);

    assert(trigstep > 0);

    /* Do first iteration with c=1.0 and s=0.0 separately to avoid loosing to
       much precision. Beware: The impact on the overal FFT precision is rather
       large. */
    { /* block 1 */

      j = 0;

      for (r = 0; r < n; r += m) {
        int32_t t1 = (r + j) << 1;
        int32_t t2 = t1 + (mh << 1);
        int32_t vr, vi, ur, ui;

        // cplxMultDiv2(&vi, &vr, x[t2+1], x[t2], (int16_t)1.0, (int16_t)0.0);
        vi = x[t2 + 1] >> 1;
        vr = x[t2] >> 1;

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui + vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui - vi;

        t1 += mh;
        t2 = t1 + (mh << 1);

        // cplxMultDiv2(&vr, &vi, x[t2+1], x[t2], (int16_t)1.0, (int16_t)0.0);
        vr = x[t2 + 1] >> 1;
        vi = x[t2] >> 1;

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui + vi;
      }

    } /* end of  block 1 */

    for (j = 1; j < mh / 4; ++j) {
      FIXP_SPK_t cs;

      cs = trigdata[j * trigstep];

      for (r = 0; r < n; r += m) {
        int32_t t1 = (r + j) << 1;
        int32_t t2 = t1 + (mh << 1);
        int32_t vr, vi, ur, ui;

        cplxMultDiv2(&vi, &vr, x[t2 + 1], x[t2], cs);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui + vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui - vi;

        t1 += mh;
        t2 = t1 + (mh << 1);

        cplxMultDiv2(&vr, &vi, x[t2 + 1], x[t2], cs);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui + vi;

        /* Same as above but for t1,t2 with j>mh/4 and thus cs swapped */
        t1 = (r + mh / 2 - j) << 1;
        t2 = t1 + (mh << 1);

        cplxMultDiv2(&vi, &vr, x[t2], x[t2 + 1], cs);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui + vi;

        t1 += mh;
        t2 = t1 + (mh << 1);

        cplxMultDiv2(&vr, &vi, x[t2], x[t2 + 1], cs);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur - vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur + vr;
        x[t2 + 1] = ui + vi;
      }
    }

    { /* block 2 */
      j = mh / 4;

      for (r = 0; r < n; r += m) {
        int32_t t1 = (r + j) << 1;
        int32_t t2 = t1 + (mh << 1);
        int32_t vr, vi, ur, ui;

        cplxMultDiv2(&vi, &vr, x[t2 + 1], x[t2], (int16_t)23170,  (int16_t)23170);


        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui + vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui - vi;

        t1 += mh;
        t2 = t1 + (mh << 1);

        cplxMultDiv2(&vr, &vi, x[t2 + 1], x[t2],(int16_t)23170,  (int16_t)23170);

        ur = x[t1] >> 1;
        ui = x[t1 + 1] >> 1;

        x[t1] = ur + vr;
        x[t1 + 1] = ui - vi;

        x[t2] = ur - vr;
        x[t2 + 1] = ui + vi;
      }
    } /* end of block 2 */
  }
}

#endif
