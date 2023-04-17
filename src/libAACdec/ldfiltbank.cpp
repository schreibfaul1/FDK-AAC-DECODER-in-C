

/**************************** AAC decoder library ******************************

   Author(s):

   Description: low delay filterbank

*******************************************************************************/
#include "newAACDecoder.h"
#include "ldfiltbank.h"

#include "aac_rom.h"
#include "../libFDK/dct.h"
#include "../libFDK/FDK_tools_rom.h"
#include "../libFDK/mdct.h"

#define LDFB_HEADROOM 2

#if defined(__arm__)
#endif

static void multE2_DinvF_fdk(int32_t *output, int32_t *x, const int16_t *fb,
                             int32_t *z, const int32_t N) {
  int32_t i;

  /*  scale for int32_t -> int32_t conversion:       */
  const int32_t scale = (DFRACT_BITS - PCM_OUT_BITS) - LDFB_HEADROOM + (3);

#if ((DFRACT_BITS - PCM_OUT_BITS - LDFB_HEADROOM + (3) - 1) > 0)
  int32_t rnd_val_wts0 = (int32_t)0;
  int32_t rnd_val_wts1 = (int32_t)0;
#if ((DFRACT_BITS - PCM_OUT_BITS - LDFB_HEADROOM + (3) - WTS0 - 1) > 0)
  if (-WTS0 - 1 + scale)
    rnd_val_wts0 = (int32_t)(1 << (-WTS0 - 1 + scale - 1));
#endif
  if (-WTS1 - 1 + scale)
    rnd_val_wts1 = (int32_t)(1 << (-WTS1 - 1 + scale - 1));
#endif

  for (i = 0; i < N / 4; i++) {
    int32_t z0, z2, tmp;

    z2 = x[N / 2 + i];
    z0 = fAddSaturate(z2,
                      (fMultDiv2(z[N / 2 + i], fb[2 * N + i]) >> (-WTS2 - 1)));

    z[N / 2 + i] = fAddSaturate(
        x[N / 2 - 1 - i],
        (fMultDiv2(z[N + i], fb[2 * N + N / 2 + i]) >> (-WTS2 - 1)));

    tmp = (fMultDiv2(z[N / 2 + i], fb[N + N / 2 - 1 - i]) +
           fMultDiv2(z[i], fb[N + N / 2 + i]));

#if ((DFRACT_BITS - PCM_OUT_BITS - LDFB_HEADROOM + (3) - 1) > 0)
    assert((-WTS1 - 1 + scale) >= 0);
    assert(tmp <= ((int32_t)0x7FFFFFFF -
                       rnd_val_wts1)); /* rounding must not cause overflow */
    output[(N * 3 / 4 - 1 - i)] = (int32_t)SATURATE_RIGHT_SHIFT(
        tmp + rnd_val_wts1, -WTS1 - 1 + scale, PCM_OUT_BITS);
#else
    assert((WTS1 + 1 - scale) >= 0);
    output[(N * 3 / 4 - 1 - i)] =
        (int32_t)SATURATE_LEFT_SHIFT(tmp, WTS1 + 1 - scale, PCM_OUT_BITS);
#endif

    z[i] = z0;
    z[N + i] = z2;
  }

  for (i = N / 4; i < N / 2; i++) {
    int32_t z0, z2, tmp0, tmp1;

    z2 = x[N / 2 + i];
    z0 = fAddSaturate(z2,
                      (fMultDiv2(z[N / 2 + i], fb[2 * N + i]) >> (-WTS2 - 1)));

    z[N / 2 + i] = fAddSaturate(
        x[N / 2 - 1 - i],
        (fMultDiv2(z[N + i], fb[2 * N + N / 2 + i]) >> (-WTS2 - 1)));

    tmp0 = (fMultDiv2(z[N / 2 + i], fb[N / 2 - 1 - i]) +
            fMultDiv2(z[i], fb[N / 2 + i]));
    tmp1 = (fMultDiv2(z[N / 2 + i], fb[N + N / 2 - 1 - i]) +
            fMultDiv2(z[i], fb[N + N / 2 + i]));

#if ((DFRACT_BITS - PCM_OUT_BITS - LDFB_HEADROOM + (3) - 1) > 0)
    assert((-WTS0 - 1 + scale) >= 0);
    assert(tmp0 <= ((int32_t)0x7FFFFFFF -
                        rnd_val_wts0)); /* rounding must not cause overflow */
    assert(tmp1 <= ((int32_t)0x7FFFFFFF -
                        rnd_val_wts1)); /* rounding must not cause overflow */
    output[(i - N / 4)] = (int32_t)SATURATE_RIGHT_SHIFT(
        tmp0 + rnd_val_wts0, -WTS0 - 1 + scale, PCM_OUT_BITS);
    output[(N * 3 / 4 - 1 - i)] = (int32_t)SATURATE_RIGHT_SHIFT(
        tmp1 + rnd_val_wts1, -WTS1 - 1 + scale, PCM_OUT_BITS);
#else
    assert((WTS0 + 1 - scale) >= 0);
    output[(i - N / 4)] =
        (int32_t)SATURATE_LEFT_SHIFT(tmp0, WTS0 + 1 - scale, PCM_OUT_BITS);
    output[(N * 3 / 4 - 1 - i)] =
        (int32_t)SATURATE_LEFT_SHIFT(tmp1, WTS1 + 1 - scale, PCM_OUT_BITS);
#endif
    z[i] = z0;
    z[N + i] = z2;
  }

  /* Exchange quarter parts of x to bring them in the "right" order */
  for (i = 0; i < N / 4; i++) {
    int32_t tmp0 = fMultDiv2(z[i], fb[N / 2 + i]);

#if ((DFRACT_BITS - PCM_OUT_BITS - LDFB_HEADROOM + (3) - 1) > 0)
    assert((-WTS0 - 1 + scale) >= 0);
    assert(tmp0 <= ((int32_t)0x7FFFFFFF -
                        rnd_val_wts0)); /* rounding must not cause overflow */
    output[(N * 3 / 4 + i)] = (int32_t)SATURATE_RIGHT_SHIFT(
        tmp0 + rnd_val_wts0, -WTS0 - 1 + scale, PCM_OUT_BITS);
#else
    assert((WTS0 + 1 - scale) >= 0);
    output[(N * 3 / 4 + i)] =
        (int32_t)SATURATE_LEFT_SHIFT(tmp0, WTS0 + 1 - scale, PCM_OUT_BITS);
#endif
  }
}

int32_t InvMdctTransformLowDelay_fdk(int32_t *mdctData, const int32_t mdctData_e,
                                 int32_t *output, int32_t *fs_buffer,
                                 const int32_t N) {
  const int16_t *coef;
  int32_t gain = (int32_t)0;
  int32_t scale = mdctData_e + MDCT_OUT_HEADROOM -
              LDFB_HEADROOM; /* The LDFB_HEADROOM is compensated inside
                                multE2_DinvF_fdk() below */
  int32_t i;

  /* Select LD window slope */
  switch (N) {
    case 256:
      coef = LowDelaySynthesis256;
      break;
    case 240:
      coef = LowDelaySynthesis240;
      break;
    case 160:
      coef = LowDelaySynthesis160;
      break;
    case 128:
      coef = LowDelaySynthesis128;
      break;
    case 120:
      coef = LowDelaySynthesis120;
      break;
    case 512:
      coef = LowDelaySynthesis512;
      break;
    case 480:
    default:
      coef = LowDelaySynthesis480;
      break;
  }

  /*
     Apply exponent and 1/N factor.
     Note: "scale" is off by one because for LD_MDCT the window length is twice
     the window length of a regular MDCT. This is corrected inside
     multE2_DinvF_fdk(). Refer to ISO/IEC 14496-3:2009 page 277,
     chapter 4.6.20.2 "Low Delay Window".
   */
  imdct_gain(&gain, &scale, N);

  dct_IV(mdctData, N, &scale);

  if (N == 256 || N == 240 || N == 160) {
    scale -= 1;
  } else if (N == 128 || N == 120) {
    scale -= 2;
  }

  if (gain != (int32_t)0) {
    for (i = 0; i < N; i++) {
      mdctData[i] = fMult(mdctData[i], gain);
    }
  }
  scaleValuesSaturate(mdctData, N, scale);

  /* Since all exponent and factors have been applied, current exponent is zero.
   */
  multE2_DinvF_fdk(output, mdctData, coef, fs_buffer, N);

  return (1);
}
