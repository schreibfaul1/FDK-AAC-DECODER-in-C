// usacdec.cpp

#include "../libArithCoding/ac_arith_coder.h"
#include "../libFDK/FDK_tools_rom.h"
#include "../libFDK/fft.h"
#include "../libFDK/mdct.h"
#include "block.h"
#include "conceal.h"
#include "newAACDecoder.h"

#include "../libAACdec/aac_rom.h"
#include "../libFDK/FDK_lpc.h"
#include "../libFDK/FDK_trigFcts.h"


#include "../libFDK/dct.h"


#include "aac_rom.h"



//----------------------------------------------------------------------------------------------------------------------
void filtLP(const int32_t *syn, int32_t *syn_out, int32_t *noise, const int16_t *filt, const int32_t aacOutDataHeadroom,
            int32_t stop, int32_t len) {
    int32_t i, j;
    int32_t tmp;

    assert((aacOutDataHeadroom - 1) >= -(MDCT_OUTPUT_SCALE));

    for(i = 0; i < stop; i++) {
        tmp = fMultDiv2(noise[i], filt[0]);  // Filt in Q-1.16
        for(j = 1; j <= len; j++) { tmp += fMult((noise[i - j] >> 1) + (noise[i + j] >> 1), filt[j]); }
        syn_out[i] = (int32_t)(IMDCT_SCALE((syn[i] >> 1) - (tmp >> 1), aacOutDataHeadroom - 1));
    }
}
//----------------------------------------------------------------------------------------------------------------------
void bass_pf_1sf_delay(int32_t       *syn,  /* (i) : 12.8kHz synthesis to postfilter              */
                       const int32_t *T_sf, /* (i) : Pitch period for all subframes (T_sf[16])    */
                       int32_t *pit_gain, const int32_t frame_length, /* (i) : frame length (should be 768|1024) */
                       const int32_t l_frame, const int32_t l_next,   /* (i) : look ahead for symmetric filtering   */
                       int32_t      *synth_out,          /* (o) : filtered synthesis (with delay of 1 subfr)   */
                       const int32_t aacOutDataHeadroom, /* (i) : headroom of the output time signal to
                                                        prevent clipping */
                       int32_t mem_bpf[])                /* i/o : memory state [L_FILT+L_SUBFR]                */
{
    int32_t i, sf, i_subfr, T, T2, lg;

    int32_t        tmp, ener, corr, gain;
    int32_t       *noise, *noise_in;
    int32_t        noise_buf[L_FILT + (2 * L_SUBFR)];  // L_FILT = 12, L_SUBFR = 64 => 140
    const int32_t *x, *y;

    {
        noise = noise_buf + L_FILT;  // L_FILT = 12 delay of upsampling filter
        noise_in = noise_buf + L_FILT + L_SUBFR;
        /* Input scaling of the BPF memory */
        scaleValues(mem_bpf, (L_FILT + L_SUBFR), 1);
    }

    int32_t gain_exp = 17;

    sf = 0;
    for(i_subfr = 0; i_subfr < l_frame; i_subfr += L_SUBFR, sf++) {
        T = T_sf[sf];
        gain = pit_gain[sf];

        /* Gain is in Q17.14 */
        /* If gain > 1 set to 1 */
        if(gain > (int32_t)(1 << 14)) gain = (int32_t)(1 << 14);

        /* If gain < 0 set to 0 */
        if(gain < (int32_t)0) gain = (int32_t)0;

        if(gain > (int32_t)0) {
            /* pitch tracker: test pitch/2 to avoid continuous pitch doubling */
            /* Note: pitch is limited to PIT_MIN (34 = 376Hz) at the encoder  */
            T2 = T >> 1;
            x = &syn[i_subfr - L_EXTRA];
            y = &syn[i_subfr - T2 - L_EXTRA];

            ener = (int32_t)0;
            corr = (int32_t)0;
            tmp = (int32_t)0;

            int32_t headroom_x = getScalefactor(x, L_SUBFR + L_EXTRA);
            int32_t headroom_y = getScalefactor(y, L_SUBFR + L_EXTRA);

            int32_t width_shift = 7;

            for(i = 0; i < (L_SUBFR + L_EXTRA); i++) {
                ener += fPow2Div2((x[i] << headroom_x)) >> width_shift;
                corr += fMultDiv2((x[i] << headroom_x), (y[i] << headroom_y)) >> width_shift;
                tmp += fPow2Div2((y[i] << headroom_y)) >> width_shift;
            }

            int32_t exp_ener = ((17 - headroom_x) << 1) + width_shift + 1;
            int32_t exp_corr = (17 - headroom_x) + (17 - headroom_y) + width_shift + 1;
            int32_t exp_tmp = ((17 - headroom_y) << 1) + width_shift + 1;

            /* Add 0.01 to "ener". Adjust exponents */
            int32_t point_zero_one = (int32_t)0x51eb851f; /* In Q-6.37 */
            int32_t diff;
            ener = fAddNorm(ener, exp_ener, point_zero_one, -6, &exp_ener);
            corr = fAddNorm(corr, exp_corr, point_zero_one, -6, &exp_corr);
            tmp = fAddNorm(tmp, exp_tmp, point_zero_one, -6, &exp_tmp);

            /* use T2 if normalized correlation > 0.95 */
            int32_t s1, s2;
            s1 = CntLeadingZeros(ener) - 1;
            s2 = CntLeadingZeros(tmp) - 1;

            int32_t ener_by_tmp = fMultDiv2(ener << s1, tmp << s2);
            int32_t ener_by_tmp_exp = (exp_ener - s1) + (exp_tmp - s2) + 1;

            if(ener_by_tmp_exp & 1) {
                ener_by_tmp <<= 1;
                ener_by_tmp_exp -= 1;
            }

            int32_t temp_exp = 0;

            int32_t temp1 = invSqrtNorm2(ener_by_tmp, &temp_exp);

            int32_t temp1_exp = temp_exp - (ener_by_tmp_exp >> 1);

            int32_t tmp_result = fMult(corr, temp1);

            int32_t tmp_result_exp = exp_corr + temp1_exp;

            diff = tmp_result_exp - 0;
            int32_t point95 = FL2FXCONST_DBL(0.95f);
            if(diff >= 0) {
                diff = fMin(diff, 31);
                point95 = FL2FXCONST_DBL(0.95f) >> diff;
            }
            else {
                diff = fMax(diff, -31);
                tmp_result >>= (-diff);
            }

            if(tmp_result > point95) T = T2;

            /* prevent that noise calculation below reaches into not defined signal
               parts at the end of the synth_buf or in other words  the below
               used index (i+i_subfr+T) < l_frame + l_next
            */
            lg = l_frame + l_next - T - i_subfr;

            if(lg > L_SUBFR) lg = L_SUBFR;
            else if(lg < 0)
                lg = 0;

            /* limit gain to avoid problem on burst */
            if(lg > 0) {
                int32_t tmp1;

                /* max(lg) = 64 => scale with 6 bits minus 1 (fPow2Div2) */

                s1 = getScalefactor(&syn[i_subfr], lg);
                s2 = getScalefactor(&syn[i_subfr + T], lg);
                int32_t s = fixMin(s1, s2);

                tmp = (int32_t)0;
                ener = (int32_t)0;
                for(i = 0; i < lg; i++) {
                    tmp += fPow2Div2(syn[i + i_subfr] << s1) >> (SF_PITCH_TRACK);
                    ener += fPow2Div2(syn[i + i_subfr + T] << s2) >> (SF_PITCH_TRACK);
                }
                tmp = tmp >> fMin(DFRACT_BITS - 1, (2 * (s1 - s)));
                ener = ener >> fMin(DFRACT_BITS - 1, (2 * (s2 - s)));

                /* error robustness: for the specific case syn[...] == -1.0f for all 64
                   samples ener or tmp might overflow and become negative. For all sane
                   cases we have enough headroom.
                */
                if(ener <= (int32_t)0) { ener = (int32_t)1; }
                if(tmp <= (int32_t)0) { tmp = (int32_t)1; }
                assert(ener > (int32_t)0);

                /* tmp = sqrt(tmp/ener) */
                int32_t result_e = 0;
                tmp1 = fDivNorm(tmp, ener, &result_e);
                if(result_e & 1) {
                    tmp1 >>= 1;
                    result_e += 1;
                }
                tmp = sqrtFixp(tmp1);
                result_e >>= 1;

                gain_exp = 17;

                diff = result_e - gain_exp;

                int32_t gain1 = gain;

                if(diff >= 0) {
                    diff = fMin(diff, 31);
                    gain1 >>= diff;
                }
                else {
                    result_e += (-diff);
                    diff = fMax(diff, -31);
                    tmp >>= (-diff);
                }

                if(tmp < gain1) {
                    gain = tmp;
                    gain_exp = result_e;
                }
            }

            /* calculate noise based on voiced pitch */
            /* fMultDiv2() replaces weighting of gain with 0.5 */
            diff = gain_exp - 17;
            if(diff >= 0) { gain <<= diff; }
            else { gain >>= (-diff); }

            s1 = CntLeadingZeros(gain) - 1;
            s1 -= 16; /* Leading bits for SGL */

            int16_t gainSGL = FX_DBL2FX_SGL(gain << 16);

            gainSGL = gainSGL << s1;

            {
                for(i = 0; i < lg; i++) {
                    /* scaled with SF_SYNTH + gain_sf + 1; composition of scalefactor 2:
                     * one additional shift of syn values + fMult => fMultDiv2 */
                    noise_in[i] = scaleValue(fMultDiv2(gainSGL, (syn[i + i_subfr] >> 1) - (syn[i + i_subfr - T] >> 2) -
                                                                    (syn[i + i_subfr + T] >> 2)),
                                             2 - s1);
                }

                for(i = lg; i < L_SUBFR; i++) {
                    /* scaled with SF_SYNTH + gain_sf + 1; composition of scalefactor 2:
                     * one additional shift of syn values + fMult => fMultDiv2 */
                    noise_in[i] =
                        scaleValue(fMultDiv2(gainSGL, (syn[i + i_subfr] >> 1) - (syn[i + i_subfr - T] >> 1)), 2 - s1);
                }
            }
        }
        else { memset(noise_in, (int32_t)0, L_SUBFR * sizeof(int32_t)); }

        {
            memcpy(noise_buf, mem_bpf, (L_FILT + L_SUBFR) * sizeof(int32_t));

            memcpy(mem_bpf, noise_buf + L_SUBFR, (L_FILT + L_SUBFR) * sizeof(int32_t));
        }

        {
            filtLP(&syn[i_subfr - L_SUBFR], &synth_out[i_subfr], noise, fdk_dec_filt_lp, aacOutDataHeadroom, L_SUBFR,
                   L_FILT);
        }
    }

    {

    }

    // To be determined (info from Ben)
    {
        /* Output scaling of the BPF memory */
        scaleValues(mem_bpf, (L_FILT + L_SUBFR), -1);
        /* Copy the rest of the signal (after the fac) */
        scaleValuesSaturate((int32_t *)&synth_out[l_frame], (int32_t *)&syn[l_frame - L_SUBFR],
                            (frame_length - l_frame), MDCT_OUT_HEADROOM - aacOutDataHeadroom);
    }

    return;
}
//----------------------------------------------------------------------------------------------------------------------
/*
 * Frequency Domain Noise Shaping
 */

/**
 * \brief Adaptive Low Frequencies Deemphasis of spectral coefficients.
 *
 * Ensure quantization of low frequencies in case where the
 * signal dynamic is higher than the LPC noise shaping.
 * This is the inverse operation of adap_low_freq_emph().
 * Output gain of all blocks.
 *
 * \param x pointer to the spectral coefficients, requires 1 bit headroom.
 * \param lg length of x.
 * \param bUseNewAlfe if set, apply ALFD for fullband lpd.
 * \param gainLpc1 pointer to gain based on old input LPC coefficients.
 * \param gainLpc2 pointer to gain based on new input LPC coefficients.
 * \param alfd_gains pointer to output gains.
 * \param s current scale shift factor of x.
 */
#define ALFDPOW2_SCALE 3
/*static*/
void CLpd_AdaptLowFreqDeemph(int32_t x[], int32_t lg, int32_t alfd_gains[], int32_t s) {
    {
        int32_t i, j, k, i_max;
        int32_t max, fac;
        /* Note: This stack array saves temporary accumulation results to be used in
         * a second run */
        /*       The size should be limited to (1024/4)/8=32 */
        int32_t tmp_pow2[32];

        s = s * 2 + ALFDPOW2_SCALE;
        s = fMin(31, s);

        k = 8;
        i_max = lg / 4; /* ALFD range = 1600Hz (lg = 6400Hz) */

        /* find spectral peak */
        max = FL2FX_DBL(0.01f) >> s;
        for(i = 0; i < i_max; i += k) {
            int32_t tmp;

            tmp = int32_t(0);
            int32_t *pX = &x[i];

            j = 8;
            do {
                int32_t x0 = *pX++;
                int32_t x1 = *pX++;
                x0 = fPow2Div2(x0);
                x1 = fPow2Div2(x1);
                tmp = tmp + (x0 >> (ALFDPOW2_SCALE - 1));
                tmp = tmp + (x1 >> (ALFDPOW2_SCALE - 1));
            } while((j = j - 2) != 0);
            tmp = fMax(tmp, (FL2FX_DBL(0.01f) >> s));
            tmp_pow2[i >> 3] = tmp;
            if(tmp > max) { max = tmp; }
        }

        /* deemphasis of all blocks below the peak */
        fac = FL2FX_DBL(0.1f) >> 1;
        for(i = 0; i < i_max; i += k) {
            int32_t tmp;
            int32_t shifti;

            tmp = tmp_pow2[i >> 3];

            /* tmp = (float)sqrt(tmp/max); */

            /* value of tmp is between 8/2*max^2 and max^2 / 2. */
            /* required shift factor of division can grow up to 27
               (grows exponentially for values toward zero)
               thus using normalized division to assure valid result. */
            {
                int32_t sd;

                if(tmp != (int32_t)0) {
                    tmp = fDivNorm(max, tmp, &sd);
                    if(sd & 1) {
                        sd++;
                        tmp >>= 1;
                    }
                }
                else {
                    tmp = (int32_t)MAXVAL_DBL;
                    sd = 0;
                }
                tmp = invSqrtNorm2(tmp, &shifti);
                tmp = scaleValue(tmp, shifti - 1 - (sd / 2));
            }
            if(tmp > fac) { fac = tmp; }
            int32_t *pX = &x[i];

            j = 8;
            do {
                int32_t x0 = pX[0];
                int32_t x1 = pX[1];
                x0 = fMultDiv2(x0, fac);
                x1 = fMultDiv2(x1, fac);
                x0 = x0 << 2;
                x1 = x1 << 2;
                *pX++ = x0;
                *pX++ = x1;

            } while((j = j - 2) != 0);
            /* Store gains for FAC */
            *alfd_gains++ = fac;
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
/**
 * \brief Interpolated Noise Shaping for mdct coefficients.
 * This algorithm shapes temporally the spectral noise between
 * the two spectral noise represention (FDNS_NPTS of resolution).
 * The noise is shaped monotonically between the two points
 * using a curved shape to favor the lower gain in mid-frame.
 * ODFT and amplitud calculation are applied to the 2 LPC coefficients first.
 *
 * \param r pointer to spectrum data.
 * \param rms RMS of output spectrum.
 * \param lg length of r.
 * \param A1 pointer to old input LPC coefficients of length M_LP_FILTER_ORDER
 * scaled by SF_A_COEFFS.
 * \param A2 pointer to new input LPC coefficients of length M_LP_FILTER_ORDER
 * scaled by SF_A_COEFFS.
 * \param bLpc2Mdct flags control lpc2mdct conversion and noise shaping.
 * \param gainLpc1 pointer to gain based on old input LPC coefficients.
 * \param gainLpc2 pointer to gain based on new input LPC coefficients.
 * \param gLpc_e pointer to exponent of gainLpc1 and gainLpc2.
 */
/* static */
#define NSHAPE_SCALE (4)

#define LPC2MDCT_CALC         (1)
#define LPC2MDCT_GAIN_LOAD    (2)
#define LPC2MDCT_GAIN_SAVE    (4)
#define LPC2MDCT_APPLY_NSHAPE (8)
//----------------------------------------------------------------------------------------------------------------------
void lpc2mdctAndNoiseShaping(int32_t *r, int16_t *pScale, const int32_t lg, const int32_t fdns_npts, const int16_t *A1,
                             const int32_t A1_exp, const int16_t *A2a, const int32_t A2_exp) {
    int32_t *tmp2 = NULL;
    int32_t  rr_minus_one;
    int32_t  i, k, s, step;

    int32_t  _tmp1[64 * 8 + 8 + sizeof(int32_t) - 1];
    int32_t *tmp1 = (int32_t *)(_tmp1 + (((int32_t)8 - ((size_t)(_tmp1)&7)) & 7));

    {
        tmp2 = tmp1 + fdns_npts * 4;

        /* lpc2mdct() */

        /* ODFT. E_LPC_a_weight() for A1 and A2 vectors is included into the loop
         * below. */
        int32_t f = FL2FXCONST_DBL(0.92f);

        const FIXP_SPK_t *SinTab;
        int32_t           k_step;
        /* needed values: sin(phi), cos(phi); phi = i*PI/(2*fdns_npts), i = 0 ...
         * M_LP_FILTER_ORDER */
        switch(fdns_npts) {
            case 64:
                SinTab = SineTable512;
                k_step = (512 / 64);
                assert(512 >= 64);
                break;
            case 48:
                SinTab = SineTable384;
                k_step = 384 / 48;
                assert(384 >= 48);
                break;
            default:
                assert(0);
                return;
        }

        for(i = 0, k = k_step; i < M_LP_FILTER_ORDER; i++, k += k_step) {
            FIXP_SPK_t cs = SinTab[k];
            int32_t    wA1, wA2;

            wA1 = fMult(A1[i], f);
            wA2 = fMult(A2a[i], f);

            /* r[i] = A[i]*cos() */
            tmp1[2 + i * 2] = fMult(wA1, cs.v.re);
            tmp2[2 + i * 2] = fMult(wA2, cs.v.re);
            /* i[i] = A[i]*sin() */
            tmp1[3 + i * 2] = -fMult(wA1, cs.v.im);
            tmp2[3 + i * 2] = -fMult(wA2, cs.v.im);

            f = fMult(f, FL2FXCONST_DBL(0.92f));
        }

        /* Guarantee at least 2 bits of headroom for the FFT */
        /* "3" stands for 1.0 with 2 bits of headroom; (A1_exp + 2) guarantess 2
         * bits of headroom if A1_exp > 1 */
        int32_t A1_exp_fix = fMax(3, A1_exp + 2);
        int32_t A2_exp_fix = fMax(3, A2_exp + 2);

        /* Set 1.0 in the proper format */
        tmp1[0] = (int32_t)(int32_t)((uint32_t)0x80000000 >> A1_exp_fix);
        tmp2[0] = (int32_t)(int32_t)((uint32_t)0x80000000 >> A2_exp_fix);

        tmp1[1] = tmp2[1] = (int32_t)0;

        /* Clear the resto of the array */
        memset(tmp1 + 2 * (M_LP_FILTER_ORDER + 1), 0, 2 * (fdns_npts * 2 - (M_LP_FILTER_ORDER + 1)) * sizeof(int32_t));
        memset(tmp2 + 2 * (M_LP_FILTER_ORDER + 1), 0, 2 * (fdns_npts * 2 - (M_LP_FILTER_ORDER + 1)) * sizeof(int32_t));

        /* Guarantee 2 bits of headroom for FFT */
        scaleValues(&tmp1[2], (2 * M_LP_FILTER_ORDER), (A1_exp - A1_exp_fix));
        scaleValues(&tmp2[2], (2 * M_LP_FILTER_ORDER), (A2_exp - A2_exp_fix));

        int32_t s2;
        s = A1_exp_fix;
        s2 = A2_exp_fix;

        fft(2 * fdns_npts, tmp1, &s);
        fft(2 * fdns_npts, tmp2, &s2);

        /* Adjust the exponents of both fft outputs if necessary*/
        if(s > s2) {
            scaleValues(tmp2, 2 * fdns_npts, s2 - s);
            s2 = s;
        }
        else if(s < s2) {
            scaleValues(tmp1, 2 * fdns_npts, s - s2);
            s = s2;
        }

        assert(s == s2);
    }

    /* Get amplitude and apply gains */
    step = lg / fdns_npts;
    rr_minus_one = (int32_t)0;

    for(k = 0; k < fdns_npts; k++) {
        int32_t g1, g2, inv_g1_g2, a, b;
        int32_t inv_g1_g2_e;
        int32_t g_e, shift;

        {
            int32_t real, imag;
            int32_t si1, si2, sInput;

            real = tmp1[k * 2];
            imag = tmp1[k * 2 + 1];
            sInput = fMax(fMin(fNorm(real), fNorm(imag)) - 1, 0);
            real <<= sInput;
            imag <<= sInput;
            /* g1_e = si1 - 2*s/2 */
            g1 = invSqrtNorm2(fPow2(real) + fPow2(imag), &si1);
            si1 += sInput;

            real = tmp2[k * 2];
            imag = tmp2[k * 2 + 1];
            sInput = fMax(fMin(fNorm(real), fNorm(imag)) - 1, 0);
            real <<= sInput;
            imag <<= sInput;
            /* g2_e = si2 - 2*s/2 */
            g2 = invSqrtNorm2(fPow2(real) + fPow2(imag), &si2);
            si2 += sInput;

            /* Pick a common scale factor for g1 and g2 */
            if(si1 > si2) {
                g2 >>= si1 - si2;
                g_e = si1 - s;
            }
            else {
                g1 >>= si2 - si1;
                g_e = si2 - s;
            }
        }

        /* end of lpc2mdct() */

        assert(g1 >= (int32_t)0);
        assert(g2 >= (int32_t)0);

        /* mdct_IntNoiseShaping() */
        {
            /* inv_g1_g2 * 2^inv_g1_g2_e = 1/(g1+g2) */
            inv_g1_g2 = (g1 >> 1) + (g2 >> 1);
            if(inv_g1_g2 != (int32_t)0) {
                inv_g1_g2 = fDivNorm(FL2FXCONST_DBL(0.5f), inv_g1_g2, &inv_g1_g2_e);
                inv_g1_g2_e = inv_g1_g2_e - g_e;
            }
            else {
                inv_g1_g2 = (int32_t)MAXVAL_DBL;
                inv_g1_g2_e = 0;
            }

            if(g_e < 0) {
                /* a_e = g_e + inv_g1_g2_e + 1 */
                a = scaleValue(fMult(fMult(g1, g2), inv_g1_g2), g_e);
                /* b_e = g_e + inv_g1_g2_e */
                b = fMult(g2 - g1, inv_g1_g2);
                shift = g_e + inv_g1_g2_e + 1 - NSHAPE_SCALE;
            }
            else {
                /* a_e = (g_e+g_e) + inv_g1_g2_e + 1 */
                a = fMult(fMult(g1, g2), inv_g1_g2);
                /* b_e = (g_e+g_e) + inv_g1_g2_e */
                b = scaleValue(fMult(g2 - g1, inv_g1_g2), -g_e);
                shift = (g_e + g_e) + inv_g1_g2_e + 1 - NSHAPE_SCALE;
            }

            for(i = k * step; i < (k + 1) * step; i++) {
                int32_t tmp;

                /* rr[i] = 2*a*r[i] + b*rr[i-1] */
                tmp = fMult(a, r[i]);
                tmp += scaleValue(fMultDiv2(b, rr_minus_one), NSHAPE_SCALE);
                tmp = scaleValueSaturate(tmp, shift);
                rr_minus_one = tmp;
                r[i] = tmp;
            }
        }
    }

    /* end of mdct_IntNoiseShaping() */
    { *pScale += NSHAPE_SCALE; }
}
//----------------------------------------------------------------------------------------------------------------------
/**
 * \brief Calculates the energy.
 * \param r pointer to spectrum.
 * \param rs scale factor of spectrum r.
 * \param lg frame length in audio samples.
 * \param rms_e pointer to exponent of energy value.
 * \return mantissa of energy value.
 */
static int32_t calcEnergy(const int32_t *r, const int16_t rs, const int32_t lg, int32_t *rms_e) {
    int32_t headroom = getScalefactor(r, lg);

    int32_t rms_m = 0;

    /* Calculate number of growth bits due to addition */
    int32_t shift = (int32_t)(fNormz((int32_t)lg));
    shift = 31 - shift;

    /* Generate 1e-2 in Q-6.37 */
    const int32_t value0_01 = 0x51eb851e;
    const int32_t value0_01_exp = -6;

    /* Find the exponent of the resulting energy value */
    *rms_e = ((rs - headroom) << 1) + shift + 1;

    int32_t delta = *rms_e - value0_01_exp;
    if(delta > 0) {
        /* Limit shift_to 31*/
        delta = fMin(31, delta);
        rms_m = value0_01 >> delta;
    }
    else {
        rms_m = value0_01;
        *rms_e = value0_01_exp;
        shift = shift - delta;
        /* Limit shift_to 31*/
        shift = fMin(31, shift);
    }

    for(int32_t i = 0; i < lg; i++) { rms_m += fPow2Div2(r[i] << headroom) >> shift; }

    return rms_m;
}
//----------------------------------------------------------------------------------------------------------------------
/**
 * \brief TCX gain calculation.
 * \param pAacDecoderChannelInfo channel context data.
 * \param r output spectrum.
 * \param rms_e pointer to mantissa of energy value.
 * \param rms_e pointer to exponent of energy value.
 * \param frame the frame index of the LPD super frame.
 * \param lg the frame length in audio samples.
 * \param gain_m pointer to mantissa of TCX gain.
 * \param gain_e pointer to exponent of TCX gain.
 * \param elFlags element specific parser guidance flags.
 * \param lg_fb the fullband frame length in audio samples.
 * \param IGF_bgn the IGF start index.
 */
static void calcTCXGain(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, int32_t *r, int32_t rms_m, int32_t rms_e,
                        const int32_t frame, const int32_t lg) {
    if((rms_m != (int32_t)0)) {
        int32_t tcx_gain_m;
        int32_t tcx_gain_e;

        CLpd_DecodeGain(&tcx_gain_m, &tcx_gain_e,
                        pAacDecoderChannelInfo->pDynData->specificTo.usac.tcx_global_gain[frame]);

        /* rms * 2^rms_e = lg/sqrt(sum(spec^2)) */
        if(rms_e & 1) {
            rms_m >>= 1;
            rms_e++;
        }

        {
            int32_t fx_lg;
            int32_t fx_lg_e, s;
            int32_t inv_e;

            /* lg = fx_lg * 2^fx_lg_e */
            s = fNorm((int32_t)lg);
            fx_lg = (int32_t)lg << s;
            fx_lg_e = DFRACT_BITS - 1 - s;
            /* 1/sqrt(rms) */
            rms_m = invSqrtNorm2(rms_m, &inv_e);
            rms_m = fMult(rms_m, fx_lg);
            rms_e = inv_e - (rms_e >> 1) + fx_lg_e;
        }

        {
            int32_t s = fNorm(tcx_gain_m);
            tcx_gain_m = tcx_gain_m << s;
            tcx_gain_e -= s;
        }

        tcx_gain_m = fMultDiv2(tcx_gain_m, rms_m);
        tcx_gain_e = tcx_gain_e + rms_e;

        /* global_gain * 2^(global_gain_e+rms_e) = (10^(global_gain/28)) * rms *
         * 2^rms_e */
        {
            { tcx_gain_e += 1; }
        }

        pAacDecoderChannelInfo->data.usac.tcx_gain[frame] = tcx_gain_m;
        pAacDecoderChannelInfo->data.usac.tcx_gain_e[frame] = tcx_gain_e;

        pAacDecoderChannelInfo->specScale[frame] += tcx_gain_e;
    }
}
//----------------------------------------------------------------------------------------------------------------------
/**
 * \brief FDNS decoding.
 * \param pAacDecoderChannelInfo channel context data.
 * \param pAacDecoderStaticChannelInfo channel context static data.
 * \param r output spectrum.
 * \param lg the frame length in audio samples.
 * \param frame the frame index of the LPD super frame.
 * \param pScale pointer to current scale shift factor of r[].
 * \param A1 old input LPC coefficients of length M_LP_FILTER_ORDER.
 * \param A2 new input LPC coefficients of length M_LP_FILTER_ORDER.
 * \param pAlfdGains pointer for ALFD gains output scaled by 1.
 * \param fdns_npts number of lines (FDNS_NPTS).
 * \param inf_mask pointer to noise mask.
 * \param IGF_win_mode IGF window mode (int32_t, int16_t, TCX10, TCX20).
 * \param frameType (IGF_FRAME_DIVISION_AAC_OR_TCX_LONG or
 * IGF_FRAME_DIVISION_TCX_SHORT_1).
 * \param elFlags element specific parser guidance flags.
 * \param lg_fb the fullband frame length in audio samples.
 * \param IGF_bgn the IGF start index.
 * \param rms_m mantisse of energy.
 * \param rms_e exponent of energy.
 */
/* static */
void CLpd_FdnsDecode(CAacDecoderChannelInfo_t       *pAacDecoderChannelInfo,
                     CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo, int32_t r[], const int32_t lg,
                     const int32_t frame, int16_t *pScale, const int16_t A1[M_LP_FILTER_ORDER], const int32_t A1_exp,
                     const int16_t A2a[M_LP_FILTER_ORDER], const int32_t A2_exp, int32_t pAlfdGains[LFAC / 4],
                     const int32_t fdns_npts) {
    /* Weight LPC coefficients using Rm values */
    CLpd_AdaptLowFreqDeemph(r, lg, pAlfdGains, *pScale);

    int32_t rms_m = (int32_t)0;
    int32_t rms_e = 0;
    {
        /* Calculate Energy */
        rms_m = calcEnergy(r, *pScale, lg, &rms_e);
    }

    calcTCXGain(pAacDecoderChannelInfo, r, rms_m, rms_e, frame, lg);

    /* Apply ODFT and Noise Shaping. LP coefficient (A1, A2) weighting is done
     * inside on the fly. */

    lpc2mdctAndNoiseShaping(r, pScale, lg, fdns_npts, A1, A1_exp, A2a, A2_exp);
}
//----------------------------------------------------------------------------------------------------------------------
/**
 * find pitch for TCX20 (time domain) concealment.
 */
static int32_t find_mpitch(int32_t xri[], int32_t lg) {
    int32_t max, pitch;
    int32_t pitch_e;
    int32_t i, n;

    max = (int32_t)0;
    n = 2;

    /* find maximum below 400Hz */
    for(i = 2; i < (lg >> 4); i += 2) {
        int32_t tmp = fPow2Div2(xri[i]) + fPow2Div2(xri[i + 1]);
        if(tmp > max) {
            max = tmp;
            n = i;
        }
    }

    // pitch = ((float)lg<<1)/(float)n;
    pitch = fDivNorm((int32_t)lg << 1, (int32_t)n, &pitch_e);
    pitch >>= fixMax(0, DFRACT_BITS - 1 - pitch_e - 16);

    /* find pitch multiple under 20ms */
    if(pitch >= (int32_t)((256 << 16) - 1)) { /*231.0f*/
        n = 256;
    }
    else {
        int32_t mpitch = pitch;
        while(mpitch < (int32_t)(255 << 16)) { mpitch += pitch; }
        n = (int32_t)(mpitch - pitch) >> 16;
    }

    return (n);
}
//----------------------------------------------------------------------------------------------------------------------
/**
 * number of spectral coefficients / time domain samples using frame mode as
 * index.
 */
static const int32_t lg_table_ccfl[2][4] = {
    {256, 256, 512, 1024}, /* coreCoderFrameLength = 1024 */
    {192, 192, 384, 768}   /* coreCoderFrameLength = 768  */
};
//----------------------------------------------------------------------------------------------------------------------
/**
 * \brief Decode and render one MDCT-TCX frame.
 * \param pAacDecoderChannelInfo channel context data.
 * \param lg the frame length in audio samples.
 * \param frame the frame index of the LPD super frame.
 */
static void CLpd_TcxDecode(CAacDecoderChannelInfo_t       *pAacDecoderChannelInfo,
                           CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo, uint32_t flags, int32_t mod,
                           int32_t last_mod, int32_t frame, int32_t frameOk) {
    int32_t  *pAlfd_gains = pAacDecoderStaticChannelInfo->last_alfd_gains;
    uint32_t *pSeed = &pAacDecoderStaticChannelInfo->nfRandomSeed;
    int32_t lg = (pAacDecoderChannelInfo->granuleLength == 128) ? lg_table_ccfl[0][mod + 0] : lg_table_ccfl[1][mod + 0];
    int32_t next_frame = frame + (1 << (mod - 1));
    int32_t isFullBandLpd = 0;

    /* Obtain r[] vector by combining the quant[] and noise[] vectors */
    {
        int32_t  noise_level;
        int32_t *coeffs = SPEC_TCX(pAacDecoderChannelInfo->pSpectralCoefficient, frame,
                                   pAacDecoderChannelInfo->granuleLength, isFullBandLpd);
        int32_t  scale = pAacDecoderChannelInfo->specScale[frame];
        int32_t  i, nfBgn, nfEnd;
        uint8_t  tcx_noise_factor = pAacDecoderChannelInfo->pDynData->specificTo.usac.tcx_noise_factor[frame];

        /* find pitch for bfi case */
        pAacDecoderStaticChannelInfo->last_tcx_pitch = find_mpitch(coeffs, lg);

        if(frameOk) {
            /* store for concealment */
            pAacDecoderStaticChannelInfo->last_tcx_noise_factor = tcx_noise_factor;
        }
        else {
            /* restore last frames value */
            tcx_noise_factor = pAacDecoderStaticChannelInfo->last_tcx_noise_factor;
        }

        noise_level = (int32_t)((int32_t)FL2FXCONST_DBL(0.0625f) * (8 - tcx_noise_factor));
        noise_level = scaleValue(noise_level, -scale);

        const int32_t neg_noise_level = -noise_level;

        {
            nfBgn = lg / 6;
            nfEnd = lg;
        }

        for(i = nfBgn; i < nfEnd - 7; i += 8) {
            int32_t tmp;

            /* Fill all 8 consecutive zero coeffs with noise */
            tmp = coeffs[i + 0] | coeffs[i + 1] | coeffs[i + 2] | coeffs[i + 3] | coeffs[i + 4] | coeffs[i + 5] |
                  coeffs[i + 6] | coeffs[i + 7];

            if(tmp == 0) {
                for(int32_t k = i; k < i + 8; k++) {
                    UsacRandomSign(pSeed) ? (coeffs[k] = neg_noise_level) : (coeffs[k] = noise_level);
                }
            }
        }
        if((nfEnd - i) > 0) { /* noise filling for last "band" with less than 8 bins */
            int32_t tmp = (int32_t)coeffs[i];
            int32_t k;

            assert((nfEnd - i) < 8);
            for(k = 1; k < (nfEnd - i); k++) { tmp |= (int32_t)coeffs[i + k]; }
            if(tmp == 0) {
                for(k = i; k < nfEnd; k++) {
                    UsacRandomSign(pSeed) ? (coeffs[k] = neg_noise_level) : (coeffs[k] = noise_level);
                }
            }
        }
    }

    {
        /* Convert LPC to LP domain */
        if(last_mod == 0) {
            /* Note: The case where last_mod == 255 is handled by other means
             * in CLpdChannelStream_Read() */
            E_LPC_f_lsp_a_conversion(pAacDecoderChannelInfo->data.usac.lsp_coeff[frame],
                                     pAacDecoderChannelInfo->data.usac.lp_coeff[frame],
                                     &pAacDecoderChannelInfo->data.usac.lp_coeff_exp[frame]);
        }

        E_LPC_f_lsp_a_conversion(pAacDecoderChannelInfo->data.usac.lsp_coeff[next_frame],
                                 pAacDecoderChannelInfo->data.usac.lp_coeff[next_frame],
                                 &pAacDecoderChannelInfo->data.usac.lp_coeff_exp[next_frame]);

        /* FDNS decoding */
        CLpd_FdnsDecode(pAacDecoderChannelInfo, pAacDecoderStaticChannelInfo,
                        SPEC_TCX(pAacDecoderChannelInfo->pSpectralCoefficient, frame,
                                 pAacDecoderChannelInfo->granuleLength, isFullBandLpd),
                        lg, frame, pAacDecoderChannelInfo->specScale + frame,
                        pAacDecoderChannelInfo->data.usac.lp_coeff[frame],
                        pAacDecoderChannelInfo->data.usac.lp_coeff_exp[frame],
                        pAacDecoderChannelInfo->data.usac.lp_coeff[next_frame],
                        pAacDecoderChannelInfo->data.usac.lp_coeff_exp[next_frame], pAlfd_gains,
                        pAacDecoderChannelInfo->granuleLength / 2 /* == FDNS_NPTS(ccfl) */
        );
    }
}
//----------------------------------------------------------------------------------------------------------------------
/**
 * \brief Read the tcx_coding bitstream part
 * \param hBs bitstream handle to read from.
 * \param pAacDecoderChannelInfo channel context info to store data into.
 * \param lg the frame length in audio samples.
 * \param first_tcx_flag flag indicating that this is the first TCX frame.
 * \param frame the frame index of the LPD super frame.
 */
static AAC_DECODER_ERROR_t CLpd_TCX_Read(HANDLE_FDK_BITSTREAM hBs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                                       CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo, int32_t lg,
                                       int32_t first_tcx_flag, int32_t frame, uint32_t flags) {
    AAC_DECODER_ERROR_t  errorAAC = AAC_DEC_OK;
    ARITH_CODING_ERROR error = ARITH_CODER_OK;
    int32_t           *pSpec;
    int32_t            arith_reset_flag = 0;
    int32_t            isFullBandLpd = 0;

    pSpec = SPEC_TCX(pAacDecoderChannelInfo->pSpectralCoefficient, frame, pAacDecoderChannelInfo->granuleLength,
                     isFullBandLpd);

    /* TCX noise level */
    { pAacDecoderChannelInfo->pDynData->specificTo.usac.tcx_noise_factor[frame] = FDKreadBits(hBs, 3); }
    /* TCX global gain */
    pAacDecoderChannelInfo->pDynData->specificTo.usac.tcx_global_gain[frame] = FDKreadBits(hBs, 7);

    /* Arithmetic coded residual/spectrum */
    if(first_tcx_flag) {
        if(flags & AC_INDEP) { arith_reset_flag = 1; }
        else { arith_reset_flag = FDKreadBits(hBs, 1); }
    }

    /* CArco_DecodeArithData() output scale of "pSpec" is DFRACT_BITS-1 */
    error = CArco_DecodeArithData(pAacDecoderStaticChannelInfo->hArCo, hBs, pSpec, lg, lg, arith_reset_flag);

    /* Rescale residual/spectrum */
    {
        int32_t scale = getScalefactor(pSpec, lg) - 2; /* Leave 2 bits headroom */

        /* Exponent of CArco_DecodeArithData() output is DFRACT_BITS; integer
         * values. */
        scaleValues(pSpec, lg, scale);
        scale = DFRACT_BITS - 1 - scale;

        pAacDecoderChannelInfo->specScale[frame] = scale;
    }

    if(error == ARITH_CODER_ERROR) errorAAC = AAC_DEC_UNKNOWN;

    return errorAAC;
}
//----------------------------------------------------------------------------------------------------------------------
/**
 * \brief translate lpd_mode into the mod[] array which describes the mode of
 * each each LPD frame
 * \param mod[] the array that will be filled with the mode indexes of the
 * inidividual frames.
 * \param lpd_mode the lpd_mode field read from the lpd_channel_stream
 */
static AAC_DECODER_ERROR_t CLpd_ReadAndMapLpdModeToModArray(uint8_t mod[4], HANDLE_FDK_BITSTREAM hBs, uint32_t elFlags) {
    int32_t lpd_mode;

    {
        lpd_mode = FDKreadBits(hBs, 5);

        if(lpd_mode > 25 || lpd_mode < 0) { return AAC_DEC_PARSE_ERROR; }

        switch(lpd_mode) {
            case 25:
                /* 1 80MS frame */
                mod[0] = mod[1] = mod[2] = mod[3] = 3;
                break;
            case 24:
                /* 2 40MS frames */
                mod[0] = mod[1] = mod[2] = mod[3] = 2;
                break;
            default:
                switch(lpd_mode >> 2) {
                    case 4:
                        /* lpd_mode 19 - 16  => 1 40MS and 2 20MS frames */
                        mod[0] = mod[1] = 2;
                        mod[2] = (lpd_mode & 1) ? 1 : 0;
                        mod[3] = (lpd_mode & 2) ? 1 : 0;
                        break;
                    case 5:
                        /* lpd_mode 23 - 20 => 2 20MS and 1 40MS frames */
                        mod[2] = mod[3] = 2;
                        mod[0] = (lpd_mode & 1) ? 1 : 0;
                        mod[1] = (lpd_mode & 2) ? 1 : 0;
                        break;
                    default:
                        /* lpd_mode < 16 => 4 20MS frames */
                        mod[0] = (lpd_mode & 1) ? 1 : 0;
                        mod[1] = (lpd_mode & 2) ? 1 : 0;
                        mod[2] = (lpd_mode & 4) ? 1 : 0;
                        mod[3] = (lpd_mode & 8) ? 1 : 0;
                        break;
                }
                break;
        }
    }
    return AAC_DEC_OK;
}
//----------------------------------------------------------------------------------------------------------------------
static void CLpd_Reset(CAacDecoderChannelInfo_t       *pAacDecoderChannelInfo,
                       CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo, int32_t keep_past_signal) {
    int32_t i;

    /* Reset TCX / ACELP common memory */
    if(!keep_past_signal) {
        memset(pAacDecoderStaticChannelInfo->old_synth, 0, sizeof(pAacDecoderStaticChannelInfo->old_synth));
    }

    /* Initialize the LSFs */
    for(i = 0; i < M_LP_FILTER_ORDER; i++) { pAacDecoderStaticChannelInfo->lpc4_lsf[i] = fdk_dec_lsf_init[i]; }

    /* Reset memory needed by bass post-filter */
    memset(pAacDecoderStaticChannelInfo->mem_bpf, 0, sizeof(pAacDecoderStaticChannelInfo->mem_bpf));

    pAacDecoderStaticChannelInfo->old_bpf_control_info = 0;
    for(i = 0; i < SYN_SFD; i++) {
        pAacDecoderStaticChannelInfo->old_T_pf[i] = 64;
        pAacDecoderStaticChannelInfo->old_gain_pf[i] = (int32_t)0;
    }

    /* Reset ACELP memory */
    CLpd_AcelpReset(&pAacDecoderStaticChannelInfo->acelp);

    pAacDecoderStaticChannelInfo->last_lpc_lost = 0;      /* prev_lpc_lost */
    pAacDecoderStaticChannelInfo->last_tcx_pitch = L_DIV; /* pitch_tcx     */
    pAacDecoderStaticChannelInfo->numLostLpdFrames = 0;   /* nbLostCmpt    */
}
//----------------------------------------------------------------------------------------------------------------------
/*
 * Externally visible functions
 */

AAC_DECODER_ERROR_t CLpdChannelStream_Read(HANDLE_FDK_BITSTREAM hBs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                                         CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
                                         const SamplingRateInfo_t *pSamplingRateInfo, uint32_t flags) {
    AAC_DECODER_ERROR_t error = AAC_DEC_OK;
    int32_t           first_tcx_flag;
    int32_t           k, nbDiv, fFacDataPresent, first_lpd_flag, acelp_core_mode, facGetMemState = 0;
    uint8_t          *mod = pAacDecoderChannelInfo->data.usac.mod;
    int32_t           lpd_mode_last, prev_frame_was_lpd;
    USAC_COREMODE_t     core_mode_last;
    const int32_t     lg_table_offset = 0;
    const int32_t    *lg_table = (pAacDecoderChannelInfo->granuleLength == 128) ? &lg_table_ccfl[0][lg_table_offset]
                                                                                : &lg_table_ccfl[1][lg_table_offset];
    int32_t           last_lpc_lost = pAacDecoderStaticChannelInfo->last_lpc_lost;

    int32_t last_frame_ok = CConcealment_GetLastFrameOk(&pAacDecoderStaticChannelInfo->concealmentInfo, 1);

    int32_t  i_offset;
    uint32_t samplingRate;

    samplingRate = pSamplingRateInfo->samplingRate;

    i_offset = (int32_t)(samplingRate * PIT_MIN_12k8 + (FSCALE_DENOM / 2)) / FSCALE_DENOM - (int32_t)PIT_MIN_12k8;

    if((samplingRate < FAC_FSCALE_MIN) || (samplingRate > FAC_FSCALE_MAX)) {
        error = AAC_DEC_PARSE_ERROR;
        goto bail;
    }

    acelp_core_mode = FDKreadBits(hBs, 3);

    /* lpd_mode */
    error = CLpd_ReadAndMapLpdModeToModArray(mod, hBs, 0);
    if(error != AAC_DEC_OK) { goto bail; }

    /* bpf_control_info */
    pAacDecoderChannelInfo->data.usac.bpf_control_info = FDKreadBit(hBs);

    /* last_core_mode */
    prev_frame_was_lpd = FDKreadBit(hBs);
    /* fac_data_present */
    fFacDataPresent = FDKreadBit(hBs);

    /* Set valid values from
     * pAacDecoderStaticChannelInfo->{last_core_mode,last_lpd_mode} */
    pAacDecoderChannelInfo->data.usac.core_mode_last = pAacDecoderStaticChannelInfo->last_core_mode;
    lpd_mode_last = pAacDecoderChannelInfo->data.usac.lpd_mode_last = pAacDecoderStaticChannelInfo->last_lpd_mode;

    if(prev_frame_was_lpd == 0) {
        /* Last frame was FD */
        pAacDecoderChannelInfo->data.usac.core_mode_last = FD_LONG;
        pAacDecoderChannelInfo->data.usac.lpd_mode_last = 255;
    }
    else {
        /* Last frame was LPD */
        pAacDecoderChannelInfo->data.usac.core_mode_last = LPD;
        if(((mod[0] == 0) && fFacDataPresent) || ((mod[0] != 0) && !fFacDataPresent)) {
            /* Currend mod is ACELP, fac data present -> TCX, current mod TCX, no fac
             * data -> TCX */
            if(lpd_mode_last == 0) {
                /* Bit stream interruption detected. Assume last TCX mode as TCX20. */
                pAacDecoderChannelInfo->data.usac.lpd_mode_last = 1;
            }
            /* Else assume that remembered TCX mode is correct. */
        }
        else { pAacDecoderChannelInfo->data.usac.lpd_mode_last = 0; }
    }

    first_lpd_flag = (pAacDecoderChannelInfo->data.usac.core_mode_last != LPD); /* Depends on bitstream configuration */
    first_tcx_flag = 1;

    if(pAacDecoderStaticChannelInfo->last_core_mode != LPD) { /* ATTENTION: Reset depends on what we rendered before! */
        CLpd_Reset(pAacDecoderChannelInfo, pAacDecoderStaticChannelInfo, 0);

        if(!last_frame_ok) {
            /* If last rendered frame was not LPD and first lpd flag is not set, this
             * must be an error - set last_lpc_lost flag */
            last_lpc_lost |= (first_lpd_flag) ? 0 : 1;
        }
    }

    core_mode_last = pAacDecoderChannelInfo->data.usac.core_mode_last;
    lpd_mode_last = pAacDecoderChannelInfo->data.usac.lpd_mode_last;

    nbDiv = NB_DIV;

    /* k is the frame index. If a frame is of size 40MS or 80MS,
       this frame index is incremented 2 or 4 instead of 1 respectively. */

    k = 0;
    while(k < nbDiv) {
        /* Reset FAC data pointers in order to avoid applying old random FAC data.
         */
        pAacDecoderChannelInfo->data.usac.fac_data[k] = NULL;

        if((k == 0 && core_mode_last == LPD && fFacDataPresent) || (lpd_mode_last == 0 && mod[k] > 0) ||
           ((lpd_mode_last != 255) && lpd_mode_last > 0 && mod[k] == 0)) {
            int32_t err;

            /* Assign FAC memory */
            pAacDecoderChannelInfo->data.usac.fac_data[k] =
                CLpd_FAC_GetMemory(pAacDecoderChannelInfo, mod, &facGetMemState);

            /* FAC for (ACELP -> TCX) or (TCX -> ACELP) */
            err = CLpd_FAC_Read(hBs, pAacDecoderChannelInfo->data.usac.fac_data[k],
                                pAacDecoderChannelInfo->data.usac.fac_data_e,
                                pAacDecoderChannelInfo->granuleLength, /* == fac_length */
                                0, k);
            if(err != 0) {
                error = AAC_DEC_PARSE_ERROR;
                goto bail;
            }
        }

        if(mod[k] == 0) /* acelp-mode */
        {
            int32_t err;
            err = CLpd_AcelpRead(hBs, &pAacDecoderChannelInfo->data.usac.acelp[k], acelp_core_mode,
                                 pAacDecoderChannelInfo->granuleLength * 8 /* coreCoderFrameLength */, i_offset);
            if(err != 0) {
                error = AAC_DEC_PARSE_ERROR;
                goto bail;
            }

            lpd_mode_last = 0;
            k++;
        }
        else /* mode != 0  =>  TCX */
        {
            error = CLpd_TCX_Read(hBs, pAacDecoderChannelInfo, pAacDecoderStaticChannelInfo, lg_table[mod[k]],
                                  first_tcx_flag, k, flags);

            lpd_mode_last = mod[k];
            first_tcx_flag = 0;
            k += 1 << (mod[k] - 1);
        }
        if(error != AAC_DEC_OK) {
            error = AAC_DEC_PARSE_ERROR;
            goto bail;
        }
    }

    {
        int32_t err;

        /* Read LPC coefficients */
        err = CLpc_Read(hBs, pAacDecoderChannelInfo->data.usac.lsp_coeff, pAacDecoderStaticChannelInfo->lpc4_lsf,
                        pAacDecoderChannelInfo->data.usac.lsf_adaptive_mean_cand,
                        pAacDecoderChannelInfo->data.usac.aStability, mod, first_lpd_flag,
                        /* if last lpc4 is available from concealment do not extrapolate lpc0
                           from lpc2 */
                        (mod[0] & 0x3) ? 0 : (last_lpc_lost && pAacDecoderStaticChannelInfo->last_core_mode != LPD),
                        last_frame_ok);
        if(err != 0) {
            error = AAC_DEC_PARSE_ERROR;
            goto bail;
        }
    }

    /* adjust old lsp[] following to a bad frame (to avoid overshoot) (ref:
     * dec_LPD.c) */
    if(last_lpc_lost && !last_frame_ok) {
        int32_t k_next;
        k = 0;
        while(k < nbDiv) {
            int32_t i;
            k_next = k + (((mod[k] & 0x3) == 0) ? 1 : (1 << (mod[k] - 1)));
            int16_t *lsp_old = pAacDecoderChannelInfo->data.usac.lsp_coeff[k];
            int16_t *lsp_new = pAacDecoderChannelInfo->data.usac.lsp_coeff[k_next];

            for(i = 0; i < M_LP_FILTER_ORDER; i++) {
                if(lsp_new[i] < lsp_old[i]) { lsp_old[i] = lsp_new[i]; }
            }
            k = k_next;
        }
    }

    if(!CConcealment_GetLastFrameOk(&pAacDecoderStaticChannelInfo->concealmentInfo, 1)) {
        E_LPC_f_lsp_a_conversion(pAacDecoderChannelInfo->data.usac.lsp_coeff[0],
                                 pAacDecoderChannelInfo->data.usac.lp_coeff[0],
                                 &pAacDecoderChannelInfo->data.usac.lp_coeff_exp[0]);
    }
    else if(pAacDecoderStaticChannelInfo->last_lpd_mode != 0) {
        if(pAacDecoderStaticChannelInfo->last_lpd_mode == 255) {
            /* We need it for TCX decoding or ACELP excitation update */
            E_LPC_f_lsp_a_conversion(pAacDecoderChannelInfo->data.usac.lsp_coeff[0],
                                     pAacDecoderChannelInfo->data.usac.lp_coeff[0],
                                     &pAacDecoderChannelInfo->data.usac.lp_coeff_exp[0]);
        }
        else { /* last_lpd_mode was TCX */
            /* Copy old LPC4 LP domain coefficients to LPC0 LP domain buffer (to avoid
             * converting LSP coefficients again). */
            memcpy(pAacDecoderChannelInfo->data.usac.lp_coeff[0], pAacDecoderStaticChannelInfo->lp_coeff_old[0],
                   M_LP_FILTER_ORDER * sizeof(int16_t));
            pAacDecoderChannelInfo->data.usac.lp_coeff_exp[0] = pAacDecoderStaticChannelInfo->lp_coeff_old_exp[0];
        }
    } /* case last_lpd_mode was ACELP is handled by CLpd_TcxDecode() */

    if(fFacDataPresent && (core_mode_last != LPD)) {
        int32_t prev_frame_was_short;

        prev_frame_was_short = FDKreadBit(hBs);

        if(prev_frame_was_short) {
            core_mode_last = pAacDecoderChannelInfo->data.usac.core_mode_last = FD_SHORT;
            pAacDecoderChannelInfo->data.usac.lpd_mode_last = 255;

            if((pAacDecoderStaticChannelInfo->last_core_mode != FD_SHORT) &&
               CConcealment_GetLastFrameOk(&pAacDecoderStaticChannelInfo->concealmentInfo, 1)) {
                /* USAC Conformance document:
                   short_fac_flag   shall be encoded with a value of 1 if the
                   window_sequence of the previous frame was 2 (EIGHT_SHORT_SEQUENCE).
                                    Otherwise short_fac_flag shall be encoded with a
                   value of 0. */
                error = AAC_DEC_PARSE_ERROR;
                goto bail;
            }
        }

        /* Assign memory */
        pAacDecoderChannelInfo->data.usac.fac_data[0] =
            CLpd_FAC_GetMemory(pAacDecoderChannelInfo, mod, &facGetMemState);

        {
            int32_t err;

            /* FAC for FD -> ACELP */
            err = CLpd_FAC_Read(
                hBs, pAacDecoderChannelInfo->data.usac.fac_data[0], pAacDecoderChannelInfo->data.usac.fac_data_e,
                CLpd_FAC_getLength(core_mode_last != FD_SHORT, pAacDecoderChannelInfo->granuleLength), 1, 0);
            if(err != 0) {
                error = AAC_DEC_PARSE_ERROR;
                goto bail;
            }
        }
    }

bail:
    if(error == AAC_DEC_OK) {
        /* check consitency of last core/lpd mode values */
        if((pAacDecoderChannelInfo->data.usac.core_mode_last != pAacDecoderStaticChannelInfo->last_core_mode) &&
           (pAacDecoderStaticChannelInfo->last_lpc_lost == 0)) {
            /* Something got wrong! */
            /* error = AAC_DEC_PARSE_ERROR; */ /* Throwing errors does not help */
        }
        else if((pAacDecoderChannelInfo->data.usac.core_mode_last == LPD) &&
                (pAacDecoderChannelInfo->data.usac.lpd_mode_last != pAacDecoderStaticChannelInfo->last_lpd_mode) &&
                (pAacDecoderStaticChannelInfo->last_lpc_lost == 0)) {
            /* Something got wrong! */
            /* error = AAC_DEC_PARSE_ERROR; */ /* Throwing errors does not help */
        }
    }

    return error;
}
//----------------------------------------------------------------------------------------------------------------------
void CLpdChannelStream_Decode(CAacDecoderChannelInfo_t       *pAacDecoderChannelInfo,
                              CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo, uint32_t flags) {
    uint8_t *mod = pAacDecoderChannelInfo->data.usac.mod;
    int32_t  k;
    uint8_t  last_lpd_mode;
    int32_t  nbDiv = NB_DIV;

    /* k is the frame index. If a frame is of size 40MS or 80MS,
       this frame index is incremented 2 or 4 instead of 1 respectively. */
    k = 0;
    last_lpd_mode = pAacDecoderChannelInfo->data.usac.lpd_mode_last; /* could be different to what has been rendered */
    while(k < nbDiv) {
        if(mod[k] == 0) {
            /* ACELP */

            /* If FAC (fac_data[k] != NULL), and previous frame was TCX, apply (TCX)
             * gains to FAC data */
            if(last_lpd_mode > 0 && last_lpd_mode != 255 && pAacDecoderChannelInfo->data.usac.fac_data[k]) {
                CFac_ApplyGains(pAacDecoderChannelInfo->data.usac.fac_data[k], pAacDecoderChannelInfo->granuleLength,
                                pAacDecoderStaticChannelInfo->last_tcx_gain,
                                pAacDecoderStaticChannelInfo->last_alfd_gains, (last_lpd_mode < 4) ? last_lpd_mode : 3);

                pAacDecoderChannelInfo->data.usac.fac_data_e[k] += pAacDecoderStaticChannelInfo->last_tcx_gain_e;
            }
        }
        else {
            /* TCX */
            CLpd_TcxDecode(pAacDecoderChannelInfo, pAacDecoderStaticChannelInfo, flags, mod[k], last_lpd_mode, k,
                           1 /* frameOk == 1 */
            );

            /* Store TCX gain scale for next possible FAC transition. */
            pAacDecoderStaticChannelInfo->last_tcx_gain = pAacDecoderChannelInfo->data.usac.tcx_gain[k];
            pAacDecoderStaticChannelInfo->last_tcx_gain_e = pAacDecoderChannelInfo->data.usac.tcx_gain_e[k];

            /* If FAC (fac_data[k] != NULL), apply gains */
            if(last_lpd_mode == 0 && pAacDecoderChannelInfo->data.usac.fac_data[k]) {
                CFac_ApplyGains(pAacDecoderChannelInfo->data.usac.fac_data[k],
                                pAacDecoderChannelInfo->granuleLength /* == fac_length */,
                                pAacDecoderChannelInfo->data.usac.tcx_gain[k],
                                pAacDecoderStaticChannelInfo->last_alfd_gains, mod[k]);

                pAacDecoderChannelInfo->data.usac.fac_data_e[k] += pAacDecoderChannelInfo->data.usac.tcx_gain_e[k];
            }
        }

        /* remember previous mode */
        last_lpd_mode = mod[k];

        /* Increase k to next frame */
        k += (mod[k] == 0) ? 1 : (1 << (mod[k] - 1));
    }
}
//----------------------------------------------------------------------------------------------------------------------
AAC_DECODER_ERROR_t CLpd_RenderTimeSignal(CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
                                        CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, int32_t *pTimeData,
                                        int32_t lFrame, SamplingRateInfo_t *pSamplingRateInfo, uint32_t frameOk,
                                        const int32_t aacOutDataHeadroom, uint32_t flags, uint32_t strmFlags) {
    uint8_t          *mod = pAacDecoderChannelInfo->data.usac.mod;
    AAC_DECODER_ERROR_t error = AAC_DEC_OK;
    int32_t           k, i_offset;
    int32_t           last_k;
    int32_t           nrSamples = 0;
    int32_t           facFB = 1;
    int32_t           nbDiv = NB_DIV;
    int32_t           lDiv = lFrame / nbDiv; /* length of division (acelp or tcx20 frame)*/
    int32_t           lFac = lDiv / 2;
    int32_t           nbSubfr = lFrame / (nbDiv * L_SUBFR); /* number of subframes per division */
    int32_t           nbSubfrSuperfr = nbDiv * nbSubfr;
    int32_t           synSfd = (nbSubfrSuperfr / 2) - BPF_SFD;
    int32_t           SynDelay = synSfd * L_SUBFR;
    int32_t           aacDelay = lFrame / 2;

    /*
     In respect to the reference software, the synth pointer here is lagging by
     aacDelay ( == SYN_DELAY + BPF_DELAY ) samples. The corresponding old
     synthesis samples are handled by the IMDCT overlap.
     */

    int32_t *synth_buf = pAacDecoderChannelInfo->pComStaticData->pWorkBufferCore1->synth_buf;
    int32_t *synth = synth_buf + PIT_MAX_MAX - BPF_DELAY;
    uint8_t  last_lpd_mode, last_last_lpd_mode, last_lpc_lost, last_frame_lost;

    int32_t pitch[NB_SUBFR_SUPERFR + SYN_SFD];
    int32_t pit_gain[NB_SUBFR_SUPERFR + SYN_SFD];

    const int32_t *lg_table;
    int32_t        lg_table_offset = 0;

    uint32_t samplingRate = pSamplingRateInfo->samplingRate;

    memset(pitch, 0, (NB_SUBFR_SUPERFR + SYN_SFD) * sizeof(int32_t));

    if(flags & AACDEC_FLUSH) {
        CLpd_Reset(pAacDecoderChannelInfo, pAacDecoderStaticChannelInfo, flags & AACDEC_FLUSH);
        frameOk = 0;
    }

    switch(lFrame) {
        case 1024:
            lg_table = &lg_table_ccfl[0][lg_table_offset];
            break;
        case 768:
            lg_table = &lg_table_ccfl[1][lg_table_offset];
            break;
        default:
            assert(0);
            return AAC_DEC_UNKNOWN;
    }

    last_frame_lost = !CConcealment_GetLastFrameOk(&pAacDecoderStaticChannelInfo->concealmentInfo, 0);

    /* Maintain LPD mode from previous frame */
    if((pAacDecoderStaticChannelInfo->last_core_mode == FD_LONG) ||
       (pAacDecoderStaticChannelInfo->last_core_mode == FD_SHORT)) {
        pAacDecoderStaticChannelInfo->last_lpd_mode = 255;
    }

    if(!frameOk) {
        int32_t old_tcx_gain;
        int16_t old_stab;
        int8_t  old_tcx_gain_e;
        int32_t nLostSf;

        last_lpd_mode = pAacDecoderStaticChannelInfo->last_lpd_mode;
        old_tcx_gain = pAacDecoderStaticChannelInfo->last_tcx_gain;
        old_tcx_gain_e = pAacDecoderStaticChannelInfo->last_tcx_gain_e;
        old_stab = pAacDecoderStaticChannelInfo->oldStability;
        nLostSf = pAacDecoderStaticChannelInfo->numLostLpdFrames;

        /* patch the last LPD mode */
        pAacDecoderChannelInfo->data.usac.lpd_mode_last = last_lpd_mode;

        /* Do mode extrapolation and repeat the previous mode:
           if previous mode = ACELP        -> ACELP
           if previous mode = TCX-20/40    -> TCX-20
           if previous mode = TCX-80       -> TCX-80
           notes:
           - ACELP is not allowed after TCX (no pitch information to reuse)
           - TCX-40 is not allowed in the mode repetition to keep the logic simple
         */
        switch(last_lpd_mode) {
            case 0:
                mod[0] = mod[1] = mod[2] = mod[3] = 0; /* -> ACELP concealment */
                break;
            case 3:
                mod[0] = mod[1] = mod[2] = mod[3] = 3; /* -> TCX FD concealment */
                break;
            case 2:
                mod[0] = mod[1] = mod[2] = mod[3] = 2; /* -> TCX FD concealment */
                break;
            case 1:
            default:
                mod[0] = mod[1] = mod[2] = mod[3] = 4; /* -> TCX TD concealment */
                break;
        }

        /* LPC extrapolation */
        CLpc_Conceal(pAacDecoderChannelInfo->data.usac.lsp_coeff, pAacDecoderStaticChannelInfo->lpc4_lsf,
                     pAacDecoderStaticChannelInfo->lsf_adaptive_mean,
                     /*(pAacDecoderStaticChannelInfo->numLostLpdFrames == 0) ||*/
                     (last_lpd_mode == 255));

        if((last_lpd_mode > 0) && (last_lpd_mode < 255)) {
            /* Copy old LPC4 LP domain coefficients to LPC0 LP domain buffer (to avoid
             * converting LSP coefficients again). */
            memcpy(pAacDecoderChannelInfo->data.usac.lp_coeff[0], pAacDecoderStaticChannelInfo->lp_coeff_old[0],
                   M_LP_FILTER_ORDER * sizeof(int16_t));
            pAacDecoderChannelInfo->data.usac.lp_coeff_exp[0] = pAacDecoderStaticChannelInfo->lp_coeff_old_exp[0];
        } /* case last_lpd_mode was ACELP is handled by CLpd_TcxDecode() */
        /* case last_lpd_mode was Time domain TCX concealment is handled after this
         * "if (!frameOk)"-block */

        /* k is the frame index. If a frame is of size 40MS or 80MS,
           this frame index is incremented 2 or 4 instead of 1 respectively. */
        k = 0;
        while(k < nbDiv) {
            pAacDecoderChannelInfo->data.usac.tcx_gain[k] = old_tcx_gain;
            pAacDecoderChannelInfo->data.usac.tcx_gain_e[k] = old_tcx_gain_e;

            /* restore stability value from last frame */
            pAacDecoderChannelInfo->data.usac.aStability[k] = old_stab;

            /* Increase k to next frame */
            k += ((mod[k] & 0x3) == 0) ? 1 : (1 << ((mod[k] & 0x3) - 1));

            nLostSf++;
        }
    }
    else {
        if((pAacDecoderStaticChannelInfo->last_lpd_mode == 4) && (mod[0] > 0)) {
            /* Copy old LPC4 LP domain coefficients to LPC0 LP domain buffer (to avoid
             * converting LSP coefficients again). */
            memcpy(pAacDecoderChannelInfo->data.usac.lp_coeff[0], pAacDecoderStaticChannelInfo->lp_coeff_old[0],
                   M_LP_FILTER_ORDER * sizeof(int16_t));
            pAacDecoderChannelInfo->data.usac.lp_coeff_exp[0] = pAacDecoderStaticChannelInfo->lp_coeff_old_exp[0];
        }
    }

    Acelp_PreProcessing(synth_buf, pAacDecoderStaticChannelInfo->old_synth, pitch,
                        pAacDecoderStaticChannelInfo->old_T_pf, pit_gain, pAacDecoderStaticChannelInfo->old_gain_pf,
                        samplingRate, &i_offset, lFrame, synSfd, nbSubfrSuperfr);

    /* k is the frame index. If a frame is of size 40MS or 80MS,
       this frame index is incremented 2 or 4 instead of 1 respectively. */
    k = 0;
    last_k = -1; /* mark invalid */
    last_lpd_mode = pAacDecoderStaticChannelInfo->last_lpd_mode;
    last_last_lpd_mode = pAacDecoderStaticChannelInfo->last_last_lpd_mode;
    last_lpc_lost = pAacDecoderStaticChannelInfo->last_lpc_lost | last_frame_lost;

    /* This buffer must be avalable for the case of FD->ACELP transition. The
    beginning of the buffer is used after the BPF to overwrite the output signal.
    Only the FAC area must be affected by the BPF */

    while(k < nbDiv) {
        if(frameOk == 0) { pAacDecoderStaticChannelInfo->numLostLpdFrames++; }
        else {
            last_frame_lost |= (pAacDecoderStaticChannelInfo->numLostLpdFrames > 0) ? 1 : 0;
            pAacDecoderStaticChannelInfo->numLostLpdFrames = 0;
        }
        if(mod[k] == 0 || mod[k] == 4) {
            /* ACELP or TCX time domain concealment */
            int32_t *acelp_out;

            /* FAC management */
            if((last_lpd_mode != 0) && (last_lpd_mode != 4)) /* TCX TD concealment */
            {
                int32_t *pFacData = NULL;

                if(frameOk && !last_frame_lost) { pFacData = pAacDecoderChannelInfo->data.usac.fac_data[k]; }

                nrSamples += CLpd_FAC_Mdct2Acelp(
                    &pAacDecoderStaticChannelInfo->IMdct, synth + nrSamples, pFacData,
                    pAacDecoderChannelInfo->data.usac.fac_data_e[k], pAacDecoderChannelInfo->data.usac.lp_coeff[k],
                    pAacDecoderChannelInfo->data.usac.lp_coeff_exp[k], lFrame - nrSamples,
                    CLpd_FAC_getLength((pAacDecoderStaticChannelInfo->last_core_mode != FD_SHORT) || (k > 0), lFac),
                    (pAacDecoderStaticChannelInfo->last_core_mode != LPD) && (k == 0), 0);

                memcpy(synth + nrSamples, pAacDecoderStaticChannelInfo->IMdct.overlap.time,
                       pAacDecoderStaticChannelInfo->IMdct.ov_offset * sizeof(int32_t));
                {
                    int16_t *lp_prev =
                        pAacDecoderChannelInfo->data.usac.lp_coeff[0]; /* init value does not real matter */
                    int32_t lp_prev_exp = pAacDecoderChannelInfo->data.usac.lp_coeff_exp[0];

                    if(last_lpd_mode != 255) { /* last mode was tcx */
                        last_k = k - (1 << (last_lpd_mode - 1));
                        if(last_k < 0) {
                            lp_prev = pAacDecoderStaticChannelInfo->lp_coeff_old[1];
                            lp_prev_exp = pAacDecoderStaticChannelInfo->lp_coeff_old_exp[1];
                        }
                        else {
                            lp_prev = pAacDecoderChannelInfo->data.usac.lp_coeff[last_k];
                            lp_prev_exp = pAacDecoderChannelInfo->data.usac.lp_coeff_exp[last_k];
                        }
                    }

                    CLpd_AcelpPrepareInternalMem(
                        synth + aacDelay + k * lDiv, last_lpd_mode, (last_last_lpd_mode == 4) ? 0 : last_last_lpd_mode,
                        pAacDecoderChannelInfo->data.usac.lp_coeff[k],
                        pAacDecoderChannelInfo->data.usac.lp_coeff_exp[k], lp_prev, lp_prev_exp,
                        &pAacDecoderStaticChannelInfo->acelp, lFrame, (last_frame_lost && k < 2), mod[k]);
                }
            }
            else {
                if(k == 0 && pAacDecoderStaticChannelInfo->IMdct.ov_offset != lFrame / facFB / 2) {
                    pAacDecoderStaticChannelInfo->IMdct.ov_offset = lFrame / facFB / 2;
                }
                nrSamples +=
                    imdct_drain(&pAacDecoderStaticChannelInfo->IMdct, synth + nrSamples, lFrame / facFB - nrSamples);
            }

            if(nrSamples >= lFrame / facFB) {
                /* Write ACELP time domain samples into IMDCT overlap buffer at
                 * pAacDecoderStaticChannelInfo->IMdct.overlap.time +
                 * pAacDecoderStaticChannelInfo->IMdct.ov_offset
                 */
                acelp_out =
                    pAacDecoderStaticChannelInfo->IMdct.overlap.time + pAacDecoderStaticChannelInfo->IMdct.ov_offset;

                /* Account ACELP time domain output samples to overlap buffer */
                pAacDecoderStaticChannelInfo->IMdct.ov_offset += lDiv;
            }
            else {
                /* Write ACELP time domain samples into output buffer at pTimeData +
                 * nrSamples */
                acelp_out = synth + nrSamples;

                /* Account ACELP time domain output samples to output buffer */
                nrSamples += lDiv;
            }

            if(mod[k] == 4) {
                pAacDecoderStaticChannelInfo->acelp.wsyn_rms =
                    scaleValue(pAacDecoderChannelInfo->data.usac.tcx_gain[k],
                               fixMin(0, pAacDecoderChannelInfo->data.usac.tcx_gain_e[k] - SF_EXC));
                CLpd_TcxTDConceal(
                    &pAacDecoderStaticChannelInfo->acelp, &pAacDecoderStaticChannelInfo->last_tcx_pitch,
                    pAacDecoderChannelInfo->data.usac.lsp_coeff[k], pAacDecoderChannelInfo->data.usac.lsp_coeff[k + 1],
                    pAacDecoderChannelInfo->data.usac.aStability[k], pAacDecoderStaticChannelInfo->numLostLpdFrames,
                    acelp_out, lFrame, pAacDecoderStaticChannelInfo->last_tcx_noise_factor);
            }
            else {
                assert(pAacDecoderChannelInfo->data.usac.aStability[k] >= (int16_t)0);
                CLpd_AcelpDecode(
                    &pAacDecoderStaticChannelInfo->acelp, i_offset, pAacDecoderChannelInfo->data.usac.lsp_coeff[k],
                    pAacDecoderChannelInfo->data.usac.lsp_coeff[k + 1], pAacDecoderChannelInfo->data.usac.aStability[k],
                    &pAacDecoderChannelInfo->data.usac.acelp[k], pAacDecoderStaticChannelInfo->numLostLpdFrames,
                    last_lpc_lost, k, acelp_out, &pitch[(k * nbSubfr) + synSfd], &pit_gain[(k * nbSubfr) + synSfd],
                    lFrame);
            }

            if(mod[k] != 4) {
                if(last_lpd_mode != 0 &&
                   pAacDecoderChannelInfo->data.usac.bpf_control_info) { /* FD/TCX -> ACELP transition */
                    /* bass post-filter past FAC area (past two (one for FD int16_t)
                     * subframes) */
                    int32_t currentSf = synSfd + k * nbSubfr;

                    if((k > 0) ||
                       (pAacDecoderStaticChannelInfo->last_core_mode != FD_SHORT)) { /* TCX or FD int32_t -> ACELP */
                        pitch[currentSf - 2] = pitch[currentSf - 1] = pitch[currentSf];
                        pit_gain[currentSf - 2] = pit_gain[currentSf - 1] = pit_gain[currentSf];
                    }
                    else { /* FD int16_t -> ACELP */
                        pitch[currentSf - 1] = pitch[currentSf];
                        pit_gain[currentSf - 1] = pit_gain[currentSf];
                    }
                }
            }
        }
        else { /* TCX */
            int32_t lg = lg_table[mod[k]];
            int32_t isFullBandLpd = 0;

            /* FAC management */
            if((last_lpd_mode == 0) || (last_lpd_mode == 4)) /* TCX TD concealment */
            {
                int32_t  _fac_buf[(1024 / 8) + (8 + sizeof(int32_t) - 1)];
                int32_t *fac_buf = (int32_t *)(_fac_buf + (((int32_t)8 - ((size_t)(_fac_buf)&7)) & 7));

                /* pAacDecoderChannelInfo->data.usac.fac_data[k] == NULL means no FAC
                 * data available. */
                if(last_frame_lost == 1 || pAacDecoderChannelInfo->data.usac.fac_data[k] == NULL) {
                    memset(fac_buf, 9, 1024 / 8 * sizeof(int32_t));
                    pAacDecoderChannelInfo->data.usac.fac_data[k] = fac_buf;
                    pAacDecoderChannelInfo->data.usac.fac_data_e[k] = 0;
                }

                nrSamples += CLpd_FAC_Acelp2Mdct(
                    &pAacDecoderStaticChannelInfo->IMdct, synth + nrSamples,
                    SPEC_TCX(pAacDecoderChannelInfo->pSpectralCoefficient, k, pAacDecoderChannelInfo->granuleLength,
                             isFullBandLpd),
                    pAacDecoderChannelInfo->specScale + k, 1, pAacDecoderChannelInfo->data.usac.fac_data[k],
                    pAacDecoderChannelInfo->data.usac.fac_data_e[k],
                    pAacDecoderChannelInfo->granuleLength /* == fac_length */, lFrame - nrSamples, lg,
                    FDKgetWindowSlope(lDiv, GetWindowShape(&pAacDecoderChannelInfo->icsInfo)), lDiv,
                    pAacDecoderChannelInfo->data.usac.lp_coeff[k], pAacDecoderChannelInfo->data.usac.lp_coeff_exp[k],
                    &pAacDecoderStaticChannelInfo->acelp, pAacDecoderChannelInfo->data.usac.tcx_gain[k],
                    (last_frame_lost || !frameOk), 0 /* is not FD FAC */
                    ,
                    last_lpd_mode, k, pAacDecoderChannelInfo->currAliasingSymmetry /* Note: The current aliasing
                                                                                      symmetry for a TCX (i.e. LPD)
                                                                                      frame must always be 0 */
                );

                pitch[(k * nbSubfr) + synSfd + 1] = pitch[(k * nbSubfr) + synSfd] = pitch[(k * nbSubfr) + synSfd - 1];
                pit_gain[(k * nbSubfr) + synSfd + 1] = pit_gain[(k * nbSubfr) + synSfd] =
                    pit_gain[(k * nbSubfr) + synSfd - 1];
            }
            else {
                int32_t tl = lg;
                int32_t fl = lDiv;
                int32_t fr = lDiv;

                nrSamples +=
                    imlt_block(&pAacDecoderStaticChannelInfo->IMdct, synth + nrSamples,
                               SPEC_TCX(pAacDecoderChannelInfo->pSpectralCoefficient, k,
                                        pAacDecoderChannelInfo->granuleLength, isFullBandLpd),
                               pAacDecoderChannelInfo->specScale + k, 1, lFrame - nrSamples, tl,
                               FDKgetWindowSlope(fl, GetWindowShape(&pAacDecoderChannelInfo->icsInfo)), fl,
                               FDKgetWindowSlope(fr, GetWindowShape(&pAacDecoderChannelInfo->icsInfo)), fr,
                               pAacDecoderChannelInfo->data.usac.tcx_gain[k],
                               pAacDecoderChannelInfo->currAliasingSymmetry ? MLT_FLAG_CURR_ALIAS_SYMMETRY : 0);
            }
        }
        /* remember previous mode */
        last_last_lpd_mode = last_lpd_mode;
        last_lpd_mode = mod[k];
        last_lpc_lost = (frameOk == 0) ? 1 : 0;

        /* Increase k to next frame */
        last_k = k;
        k += ((mod[k] & 0x3) == 0) ? 1 : (1 << (mod[k] - 1));
    }

    if(frameOk) {
        /* assume data was ok => store for concealment */
        assert(pAacDecoderChannelInfo->data.usac.aStability[last_k] >= (int16_t)0);
        pAacDecoderStaticChannelInfo->oldStability = pAacDecoderChannelInfo->data.usac.aStability[last_k];
        memcpy(pAacDecoderStaticChannelInfo->lsf_adaptive_mean,
               pAacDecoderChannelInfo->data.usac.lsf_adaptive_mean_cand, M_LP_FILTER_ORDER * sizeof(int16_t));
    }

    /* store past lp coeffs for next superframe (they are only valid and needed if
     * last_lpd_mode was tcx) */
    if(last_lpd_mode > 0) {
        memcpy(pAacDecoderStaticChannelInfo->lp_coeff_old[0], pAacDecoderChannelInfo->data.usac.lp_coeff[nbDiv],
               M_LP_FILTER_ORDER * sizeof(int16_t));
        pAacDecoderStaticChannelInfo->lp_coeff_old_exp[0] = pAacDecoderChannelInfo->data.usac.lp_coeff_exp[nbDiv];
        memcpy(pAacDecoderStaticChannelInfo->lp_coeff_old[1], pAacDecoderChannelInfo->data.usac.lp_coeff[last_k],
               M_LP_FILTER_ORDER * sizeof(int16_t));
        pAacDecoderStaticChannelInfo->lp_coeff_old_exp[1] = pAacDecoderChannelInfo->data.usac.lp_coeff_exp[last_k];
    }

    assert(nrSamples == lFrame);

    /* check whether usage of bass postfilter was de-activated in the bitstream;
     if yes, set pitch gain to 0 */
    if(!(pAacDecoderChannelInfo->data.usac.bpf_control_info)) {
        if(mod[0] != 0 && (pAacDecoderStaticChannelInfo->old_bpf_control_info)) {
            for(int32_t i = 2; i < nbSubfrSuperfr; i++) pit_gain[synSfd + i] = (int32_t)0;
        }
        else {
            for(int32_t i = 0; i < nbSubfrSuperfr; i++) pit_gain[synSfd + i] = (int32_t)0;
        }
    }

    /* for bass postfilter */
    for(int32_t n = 0; n < synSfd; n++) {
        pAacDecoderStaticChannelInfo->old_T_pf[n] = pitch[nbSubfrSuperfr + n];
        pAacDecoderStaticChannelInfo->old_gain_pf[n] = pit_gain[nbSubfrSuperfr + n];
    }

    pAacDecoderStaticChannelInfo->old_bpf_control_info = pAacDecoderChannelInfo->data.usac.bpf_control_info;

    {
        int32_t lookahead = -BPF_DELAY;
        int32_t copySamp = (mod[nbDiv - 1] == 0) ? (aacDelay) : (aacDelay - lFac);

        /* Copy enough time domain samples from MDCT to synthesis buffer as needed
         * by the bass postfilter */

        lookahead += imdct_copy_ov_and_nr(&pAacDecoderStaticChannelInfo->IMdct, synth + nrSamples, copySamp);

        assert(lookahead == copySamp - BPF_DELAY);

        int32_t *p2_synth = synth + BPF_DELAY;

        /* recalculate pitch gain to allow postfilering on FAC area */
        for(int32_t i = 0; i < nbSubfrSuperfr; i++) {
            int32_t T = pitch[i];
            int32_t gain = pit_gain[i];

            if(gain > (int32_t)0) {
                gain = get_gain(&p2_synth[i * L_SUBFR], &p2_synth[(i * L_SUBFR) - T], L_SUBFR);
                pit_gain[i] = gain;
            }
        }

        {
            bass_pf_1sf_delay(p2_synth, pitch, pit_gain, lFrame, lFrame / facFB,
                              mod[nbDiv - 1] ? (SynDelay - (lDiv / 2)) : SynDelay, pTimeData, aacOutDataHeadroom,
                              pAacDecoderStaticChannelInfo->mem_bpf);
        }
    }

    Acelp_PostProcessing(synth_buf, pAacDecoderStaticChannelInfo->old_synth, pitch,
                         pAacDecoderStaticChannelInfo->old_T_pf, lFrame, synSfd, nbSubfrSuperfr);

    /* Store last mode for next super frame */
    { pAacDecoderStaticChannelInfo->last_core_mode = LPD; }
    pAacDecoderStaticChannelInfo->last_lpd_mode = last_lpd_mode;
    pAacDecoderStaticChannelInfo->last_last_lpd_mode = last_last_lpd_mode;
    pAacDecoderStaticChannelInfo->last_lpc_lost = last_lpc_lost;

    return error;
}
//----------------------------------------------------------------------------------------------------------------------
/**
 * \brief Read unary code.
 * \param hBs bitstream handle as data source.
 * \return decoded value.
 */
static int32_t get_vlclbf(HANDLE_FDK_BITSTREAM hBs) {
    const uint8_t NQ_MAX = 36;
    int32_t result = 0;

    while(FDKreadBits(hBs, 1) && result <= NQ_MAX) { result++; }
    return result;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief Read bit count limited unary code.
 * \param hBs bitstream handle as data source
 * \param n max amount of bits to be read.
 * \return decoded value.
 */
static int32_t get_vlclbf_n(HANDLE_FDK_BITSTREAM hBs, int32_t n) {
    int32_t result = 0;

    while(FDKreadBits(hBs, 1)) {
        result++;
        n--;
        if(n <= 0) { break; }
    }

    return result;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void nearest_neighbor_2D8(int32_t x[8], int32_t y[8]) {
    int32_t s, em, e[8];
    int32_t i, j, sum;

    /* round x into 2Z^8 i.e. compute y=(y1,...,y8) such that yi = 2[xi/2]
       where [.] is the nearest integer operator
       in the mean time, compute sum = y1+...+y8
    */
    sum = 0;
    for(i = 0; i < 8; i++) {
        int32_t tmp;
        /* round to ..., -2, 0, 2, ... ([-1..1[ --> 0) */
        if(x[i] < (int32_t)0) {
            tmp = (int32_t)1 << DFRACT_BITS / 2 - x[i];
            y[i] = -2 * ((tmp >> DFRACT_BITS / 2) >> 1);
        }
        else {
            tmp = (int32_t)1 << DFRACT_BITS / 2 + x[i];
            y[i] = 2 * ((tmp >> DFRACT_BITS / 2) >> 1);
        }
        sum += y[i];
    }
    /* check if y1+...+y8 is a multiple of 4
       if not, y is not round xj in the wrong way where j is defined by
          j = arg max_i | xi -yi|
       (this is called the Wagner rule)
    */
    if(sum % 4) {
        /* find j = arg max_i | xi -yi| */
        em = (int16_t)0;
        j = 0;
        for(i = 0; i < 8; i++) {
            /* compute ei = xi-yi */
            e[i] = x[i] - (y[i] << (DFRACT_BITS / 2));
        }
        for(i = 0; i < 8; i++) {
            /* compute |ei| = | xi-yi | */
            if(e[i] < (int32_t)0) { s = -e[i]; }
            else { s = e[i]; }
            /* check if |ei| is maximal, if so, set j=i */
            if(em < s) {
                em = s;
                j = i;
            }
        }
        /* round xj in the "wrong way" */
        if(e[j] < (int32_t)0) { y[j] -= 2; }
        else { y[j] += 2; }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void RE8_PPV(int32_t x[], int16_t y[], int32_t r) {
    int32_t i, y0[8], y1[8];
    int32_t x1[8], tmp;
    int64_t e;

    /* find the nearest neighbor y0 of x in 2D8 */
    nearest_neighbor_2D8(x, y0);
    /* find the nearest neighbor y1 of x in 2D8+(1,...,1) (by coset decoding) */
    for(i = 0; i < 8; i++) { x1[i] = x[i] - (int32_t)1 << DFRACT_BITS / 2; }
    nearest_neighbor_2D8(x1, y1);
    for(i = 0; i < 8; i++) { y1[i] += 1; }

    /* compute e0=||x-y0||^2 and e1=||x-y1||^2 */
    e = 0;
    for(i = 0; i < 8; i++) {
        tmp = x[i] - (y0[i] << (DFRACT_BITS / 2));
        e += (int64_t)fPow2Div2(tmp << r); /* shift left to ensure that no fract part bits get lost. */
        tmp = x[i] - (y1[i] << (DFRACT_BITS / 2));
        e -= (int64_t)fPow2Div2(tmp << r);
    }
    /* select best candidate y0 or y1 to minimize distortion */
    if(e < 0) {
        for(i = 0; i < 8; i++) { y[i] = y0[i]; }
    }
    else {
        for(i = 0; i < 8; i++) { y[i] = y1[i]; }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/* table look-up of unsigned value: find i where index >= table[i]
   Note: range must be >= 2, index must be >= table[0] */
static int32_t table_lookup(const uint16_t *table, uint32_t index, int32_t range) {
    int32_t i;

    for(i = 4; i < range; i += 4) {
        if(index < table[i]) { break; }
    }
    if(i > range) { i = range; }

    if(index < table[i - 2]) { i -= 2; }
    if(index < table[i - 1]) { i--; }
    i--;

    return (i); /* index >= table[i] */
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
//  re8_decode_rank_of_permutation(rank, xs, x)
//  DECODING OF THE RANK OF THE PERMUTATION OF xs
//  (i) rank: index (rank) of a permutation
//  (i) xs:   signed leader in RE8 (8-dimensional integer vector)
//  (o) x:    point in RE8 (8-dimensional integer vector)
//  --------------------------------------------------------------------------
static void re8_decode_rank_of_permutation(int32_t rank, int32_t *xs, int16_t x[8]) {
    int32_t a[8], w[8], B1, fac, fac_B, target;
    int32_t i, j;

    /* --- pre-processing based on the signed leader xs ---
       - compute the alphabet a=[a[0] ... a[q-1]] of x (q elements)
         such that a[0]!=...!=a[q-1]
         it is assumed that xs is sorted in the form of a signed leader
         which can be summarized in 2 requirements:
            a) |xs[0]| >= |xs[1]| >= |xs[2]| >= ... >= |xs[7]|
            b) if |xs[i]|=|xs[i-1]|, xs[i]>=xs[i+1]
         where |.| indicates the absolute value operator
       - compute q (the number of symbols in the alphabet)
       - compute w[0..q-1] where w[j] counts the number of occurences of
         the symbol a[j] in xs
       - compute B = prod_j=0..q-1 (w[j]!) where .! is the factorial */
    /* xs[i], xs[i-1] and ptr_w/a*/
    j = 0;
    w[j] = 1;
    a[j] = xs[0];
    B1 = 1;
    for(i = 1; i < 8; i++) {
        if(xs[i] != xs[i - 1]) {
            j++;
            w[j] = 1;
            a[j] = xs[i];
        }
        else {
            w[j]++;
            B1 *= w[j];
        }
    }

    /* --- actual rank decoding ---
       the rank of x (where x is a permutation of xs) is based on
       Schalkwijk's formula
       it is given by rank=sum_{k=0..7} (A_k * fac_k/B_k)
       the decoding of this rank is sequential and reconstructs x[0..7]
       element by element from x[0] to x[7]
       [the tricky part is the inference of A_k for each k...]
     */

    if(w[0] == 8) {
        for(i = 0; i < 8; i++) { x[i] = a[0]; /* avoid fac of 40320 */ }
    }
    else {
        target = rank * B;
        fac_B = 1;
        /* decode x element by element */
        for(i = 0; i < 8; i++) {
            fac = fac_B * fdk_dec_tab_factorial[i]; /* fac = 1..5040 */
            j = -1;
            do { target -= w[++j] * fac; } while(target >= 0); /* max of 30 tests / SV */
            x[i] = a[j];
            /* update rank, denominator B (B_k) and counter w[j] */
            target += w[j] * fac; /* target = fac_B*B*rank */
            fac_B *= w[j];
            w[j]--;
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
//  re8_decode_base_index(n, I, y)
//  DECODING OF AN INDEX IN Qn (n=0,2,3 or 4)
//  (i) n: codebook number (*n is an integer defined in {0,2,3,4})
//  (i) I: index of c (pointer to unsigned 16-bit word)
//  (o) y: point in RE8 (8-dimensional integer vector)
//  note: the index I is defined as a 32-bit word, but only
//  16 bits are required (int32_t can be replaced by unsigned integer)
//  --------------------------------------------------------------------------
static void re8_decode_base_index(int32_t *n, uint32_t index, int16_t y[8]) {
    int32_t i, im, t, sign_code, ka, ks, rank, leader[8];

    if(*n < 2) {
        for(i = 0; i < 8; i++) { y[i] = 0; }
    }
    else {
        // index = (unsigned int32_t)*I;
        /* search for the identifier ka of the absolute leader (table-lookup)
           Q2 is a subset of Q3 - the two cases are considered in the same branch
         */
        switch(*n) {
            case 2:
            case 3:
                i = table_lookup(fdk_dec_I3, index, NB_LDQ3);
                ka = fdk_dec_A3[i];
                break;
            case 4:
                i = table_lookup(fdk_dec_I4, index, NB_LDQ4);
                ka = fdk_dec_A4[i];
                break;
            default:
                assert(0);
                return;
        }
        /* reconstruct the absolute leader */
        for(i = 0; i < 8; i++) { leader[i] = fdk_dec_Da[ka][i]; }
        /* search for the identifier ks of the signed leader (table look-up)
           (this search is focused based on the identifier ka of the absolute
            leader)*/
        t = fdk_dec_Ia[ka];
        im = fdk_dec_Ns[ka];
        ks = table_lookup(fdk_dec_Is + t, index, im);

        /* reconstruct the signed leader from its sign code */
        sign_code = 2 * fdk_dec_Ds[t + ks];
        for(i = 7; i >= 0; i--) {
            leader[i] *= (1 - (sign_code & 2));
            sign_code >>= 1;
        }

        /* compute and decode the rank of the permutation */
        rank = index - fdk_dec_Is[t + ks]; /* rank = index - cardinality offset */

        re8_decode_rank_of_permutation(rank, leader, y);
    }
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/* re8_y2k(y,m,k)
   VORONOI INDEXING (INDEX DECODING) k -> y
   (i) k: Voronoi index k[0..7]
   (i) m: Voronoi modulo (m = 2^r = 1<<r, where r is integer >=2)
   (i) r: Voronoi order  (m = 2^r = 1<<r, where r is integer >=2)
   (o) y: 8-dimensional point y[0..7] in RE8
 */
static void re8_k2y(int32_t *k, int32_t r, int16_t *y) {
    int32_t i, tmp, sum;
    int16_t v[8];
    int32_t zf[8];

    assert(r <= (DFRACT_BITS / 2));

    /* compute y = k M and z=(y-a)/m, where
       M = [4        ]
           [2 2      ]
           [|   \    ]
           [2     2  ]
           [1 1 _ 1 1]
       a=(2,0,...,0)
       m = 1<<r
    */
    for(i = 0; i < 8; i++) { y[i] = k[7]; }
    zf[7] = y[7] << (DFRACT_BITS / 2 - r);
    sum = 0;
    for(i = 6; i >= 1; i--) {
        tmp = 2 * k[i];
        sum += tmp;
        y[i] += tmp;
        zf[i] = y[i] << (DFRACT_BITS / 2 - r);
    }
    y[0] += (4 * k[0] + sum);
    zf[0] = (y[0] - 2) << (DFRACT_BITS / 2 - r);
    /* find nearest neighbor v of z in infinite RE8 */
    RE8_PPV(zf, v, r);
    /* compute y -= m v */
    for(i = 0; i < 8; i++) { y[i] -= (int16_t)(v[i] << r); }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
//
//  RE8_dec(n, I, k, y)
//  MULTI-RATE INDEXING OF A POINT y in THE LATTICE RE8 (INDEX DECODING)
//  (i) n: codebook number (*n is an integer defined in {0,2,3,4,..,n_max}). n_max
//  = 36 (i) I: index of c (pointer to unsigned 16-bit word) (i) k: index of v
//  (8-dimensional vector of binary indices) = Voronoi index (o) y: point in RE8
//  (8-dimensional integer vector) note: the index I is defined as a 32-bit word,
//  but only 16 bits are required (int32_t can be replaced by unsigned integer)
//  return 0 on success, -1 on error.
//--------------------------------------------------------------------------
static int32_t RE8_dec(int32_t n, int32_t I, int32_t *k, int32_t *y) {
    int16_t  v[8];
    int16_t  _y[8];
    uint32_t r;
    int32_t  i;
    const uint8_t NQ_MAX = 36;
    /* Check bound of codebook qn */
    if(n > NQ_MAX) { return -1; }

    /* decode the sub-indices I and kv[] according to the codebook number n:
       if n=0,2,3,4, decode I (no Voronoi extension)
       if n>4, Voronoi extension is used, decode I and kv[] */
    if(n <= 4) {
        re8_decode_base_index(&n, I, _y);
        for(i = 0; i < 8; i++) { y[i] = (int32_t)_y[i]; }
    }
    else {
        /* compute the Voronoi modulo m = 2^r where r is extension order */
        r = ((n - 3) >> 1);

        while(n > 4) { n -= 2; }
        /* decode base codebook index I into c (c is an element of Q3 or Q4)
           [here c is stored in y to save memory] */
        re8_decode_base_index(&n, I, _y);
        /* decode Voronoi index k[] into v */
        re8_k2y(k, r, v);
        /* reconstruct y as y = m c + v (with m=2^r, r integer >=1) */
        for(i = 0; i < 8; i++) { y[i] = (int32_t)((_y[i] << r) + v[i]); }
    }
    return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief calculate inverse weighting factor and add non-weighted residual
 *        LSF vector to first stage LSF approximation
 * \param lsfq first stage LSF approximation values.
 * \param xq weighted residual LSF vector
 * \param nk_mode code book number coding mode.
 */
static void lsf_weight_2st(int16_t *lsfq, int32_t *xq, int32_t nk_mode) {
    int16_t d[M_LP_FILTER_ORDER + 1];
    int16_t factor;
    int32_t w; /* inverse weight factor */
    int32_t i;
    const uint16_t FREQ_MAX = 6400;
    const uint16_t FREQ_DIV = 400;

    /* compute lsf distance */
    d[0] = lsfq[0];
    d[M_LP_FILTER_ORDER] = FL2FXCONST_SGL((double)FREQ_MAX / (1 << LSF_SCALE)) - lsfq[M_LP_FILTER_ORDER - 1];
    for(i = 1; i < M_LP_FILTER_ORDER; i++) { d[i] = lsfq[i] - lsfq[i - 1]; }

    switch(nk_mode) {
        case 0:
            factor = FL2FXCONST_SGL(2.0f * 60.0f / FREQ_DIV);
            break; /* abs */
        case 1:
            factor = FL2FXCONST_SGL(2.0f * 65.0f / FREQ_DIV);
            break; /* mid */
        case 2:
            factor = FL2FXCONST_SGL(2.0f * 64.0f / FREQ_DIV);
            break; /* rel1 */
        default:
            factor = FL2FXCONST_SGL(2.0f * 63.0f / FREQ_DIV);
            break; /* rel2 */
    }
    /* add non-weighted residual LSF vector to LSF1st */
    for(i = 0; i < M_LP_FILTER_ORDER; i++) {
        w = (int32_t)fMultDiv2(factor, sqrtFixp(fMult(d[i], d[i + 1])));
        lsfq[i] = fAddSaturate(lsfq[i], FX_DBL2FX_SGL((int32_t)((int64_t)w * (int32_t)xq[i])));
    }

    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief decode nqn amount of code book numbers. These values determine the
 * amount of following bits for nqn AVQ RE8 vectors.
 * \param nk_mode quantization mode.
 * \param nqn amount code book number to read.
 * \param qn pointer to output buffer to hold decoded code book numbers qn.
 */
static void decode_qn(HANDLE_FDK_BITSTREAM hBs, int32_t nk_mode, int32_t nqn, int32_t qn[]) {
    int32_t n;

    if(nk_mode == 1) { /* nk mode 1 */
        /* Unary code for mid LPC1/LPC3 */
        /* Q0=0, Q2=10, Q3=110, ... */
        for(n = 0; n < nqn; n++) {
            qn[n] = get_vlclbf(hBs);
            if(qn[n] > 0) { qn[n]++; }
        }
    }
    else { /* nk_mode 0, 3 and 2 */
        /* 2 bits to specify Q2,Q3,Q4,ext */
        for(n = 0; n < nqn; n++) { qn[n] = 2 + FDKreadBits(hBs, 2); }
        if(nk_mode == 2) {
            /* Unary code for rel LPC1/LPC3 */
            /* Q0 = 0, Q5=10, Q6=110, ... */
            for(n = 0; n < nqn; n++) {
                if(qn[n] > 4) {
                    qn[n] = get_vlclbf(hBs);
                    if(qn[n] > 0) qn[n] += 4;
                }
            }
        }
        else { /* nk_mode == (0 and 3) */
            /* Unary code for abs and rel LPC0/LPC2 */
            /* Q5 = 0, Q6=10, Q0=110, Q7=1110, ... */
            for(n = 0; n < nqn; n++) {
                if(qn[n] > 4) {
                    qn[n] = get_vlclbf(hBs);
                    switch(qn[n]) {
                        case 0:
                            qn[n] = 5;
                            break;
                        case 1:
                            qn[n] = 6;
                            break;
                        case 2:
                            qn[n] = 0;
                            break;
                        default:
                            qn[n] += 4;
                            break;
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief reorder LSF coefficients to minimum distance.
 * \param lsf pointer to buffer containing LSF coefficients and where reordered
 * LSF coefficients will be stored into, scaled by LSF_SCALE.
 * \param min_dist min distance scaled by LSF_SCALE
 * \param n number of LSF/LSP coefficients.
 */
static void reorder_lsf(int16_t *lsf, int16_t min_dist, int32_t n) {
    int16_t lsf_min;
    int32_t i;
    const uint16_t FREQ_MAX = 6400;

    lsf_min = min_dist;
    for(i = 0; i < n; i++) {
        if(lsf[i] < lsf_min) { lsf[i] = lsf_min; }
        lsf_min = fAddSaturate(lsf[i], min_dist);
    }

    /* reverse */
    lsf_min = FL2FXCONST_SGL((double)FREQ_MAX / (1 << LSF_SCALE)) - min_dist;
    for(i = n - 1; i >= 0; i--) {
        if(lsf[i] > lsf_min) { lsf[i] = lsf_min; }

        lsf_min = lsf[i] - min_dist;
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief First stage approximation
 * \param hBs bitstream handle as data source
 * \param lsfq pointer to output buffer to hold LPC coefficients scaled by
 * LSF_SCALE.
 */
static void vlpc_1st_dec(HANDLE_FDK_BITSTREAM hBs, /* input:  codebook index                  */
                         int16_t             *lsfq /* i/o:    i:prediction   o:quantized lsf  */
) {
    const int16_t *p_dico;
    int32_t        i, index;

    index = FDKreadBits(hBs, 8);
    p_dico = &fdk_dec_dico_lsf_abs_8b[index * M_LP_FILTER_ORDER];
    for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsfq[i] = p_dico[i]; }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief Do first stage approximation weighting and multiply with AVQ
 * refinement.
 * \param hBs bitstream handle data ssource.
 * \param lsfq buffer holding 1st stage approx, 2nd stage approx is added to
 * this values.
 * \param nk_mode quantization mode.
 * \return 0 on success, -1 on error.
 */
static int32_t vlpc_2st_dec(HANDLE_FDK_BITSTREAM hBs, int16_t *lsfq, /* i/o:    i:1st stage   o:1st+2nd stage   */
                            int32_t nk_mode                          /* input:  0=abs, >0=rel                   */
) {
    int32_t err;
    int32_t xq[M_LP_FILTER_ORDER]; /* weighted residual LSF vector */
    const uint8_t LSF_GAP = 50;

    /* Decode AVQ refinement */
    { err = CLpc_DecodeAVQ(hBs, xq, nk_mode, 2, 8); }
    if(err != 0) { return -1; }

    /* add non-weighted residual LSF vector to LSF1st */
    lsf_weight_2st(lsfq, xq, nk_mode);

    /* reorder */
    reorder_lsf(lsfq, FL2FXCONST_SGL((double)LSF_GAP / (1 << LSF_SCALE)), M_LP_FILTER_ORDER);

    return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
int32_t CLpc_DecodeAVQ(HANDLE_FDK_BITSTREAM hBs, int32_t *pOutput, int32_t nk_mode, int32_t no_qn, int32_t length) {
    int32_t i, l;

    for(i = 0; i < length; i += 8 * no_qn) {
        int32_t qn[2], nk, n, I;
        int32_t kv[8] = {0};

        decode_qn(hBs, nk_mode, no_qn, qn);

        for(l = 0; l < no_qn; l++) {
            if(qn[l] == 0) { memset(&pOutput[i + l * 8], 0, 8 * sizeof(int32_t)); }

            /* Voronoi extension order ( nk ) */
            nk = 0;
            n = qn[l];
            if(qn[l] > 4) {
                nk = (qn[l] - 3) >> 1;
                n = qn[l] - nk * 2;
            }
            /* Base codebook index, in reverse bit group order (!) */
            I = FDKreadBits(hBs, 4 * n);
            if(nk > 0) {
                int32_t j;

                for(j = 0; j < 8; j++) { kv[j] = FDKreadBits(hBs, nk); }
            }
            if(RE8_dec(qn[l], I, kv, &pOutput[i + l * 8]) != 0) { return -1; }
        }
    }
    return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
int32_t CLpc_Read(HANDLE_FDK_BITSTREAM hBs, int16_t lsp[][M_LP_FILTER_ORDER], int16_t lpc4_lsf[M_LP_FILTER_ORDER],
                  int16_t lsf_adaptive_mean_cand[M_LP_FILTER_ORDER], int16_t pStability[], uint8_t *mod, int32_t first_lpd_flag,
                  int32_t last_lpc_lost, int32_t last_frame_ok) {
    int32_t       i, k, err;
    int32_t       mode_lpc_bin = 0; /* mode_lpc bitstream representation */
    int32_t       lpc_present[5] = {0, 0, 0, 0, 0};
    int32_t       lpc0_available = 1;
    int32_t       s = 0;
    int32_t       l = 3;
    const int32_t nbDiv = NB_DIV;

    lpc_present[4 >> s] = 1; /* LPC4 */

    /* Decode LPC filters in the following order: LPC 4,0,2,1,3 */

    /*** Decode LPC4 ***/
    vlpc_1st_dec(hBs, lsp[4 >> s]);
    err = vlpc_2st_dec(hBs, lsp[4 >> s], 0); /* nk_mode = 0 */
    if(err != 0) { return err; }

    /*** Decode LPC0 and LPC2 ***/
    k = 0;
    if(!first_lpd_flag) {
        lpc_present[0] = 1;
        lpc0_available = !last_lpc_lost;
        /* old LPC4 is new LPC0 */
        for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[0][i] = lpc4_lsf[i]; }
        /* skip LPC0 and continue with LPC2 */
        k = 2;
    }

    for(; k < l; k += 2) {
        int32_t nk_mode = 0;

        if((k == 2) && (mod[0] == 3)) { break; /* skip LPC2 */ }

        lpc_present[k >> s] = 1;

        mode_lpc_bin = FDKreadBit(hBs);

        if(mode_lpc_bin == 0) {
            /* LPC0/LPC2: Abs */
            vlpc_1st_dec(hBs, lsp[k >> s]);
        }
        else {
            /* LPC0/LPC2: RelR */
            for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[k >> s][i] = lsp[4 >> s][i]; }
            nk_mode = 3;
        }

        err = vlpc_2st_dec(hBs, lsp[k >> s], nk_mode);
        if(err != 0) { return err; }
    }

    /*** Decode LPC1 ***/
    if(mod[0] < 2) { /* else: skip LPC1 */
        lpc_present[1] = 1;
        mode_lpc_bin = get_vlclbf_n(hBs, 2);

        switch(mode_lpc_bin) {
            case 1:
                /* LPC1: abs */
                vlpc_1st_dec(hBs, lsp[1]);
                err = vlpc_2st_dec(hBs, lsp[1], 0);
                if(err != 0) { return err; }
                break;
            case 2:
                /* LPC1: mid0 (no second stage AVQ quantizer in this case) */
                if(lpc0_available) { /* LPC0/lsf[0] might be zero some times */
                    for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[1][i] = (lsp[0][i] >> 1) + (lsp[2][i] >> 1); }
                }
                else {
                    for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[1][i] = lsp[2][i]; }
                }
                break;
            case 0:
                /* LPC1: RelR */
                for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[1][i] = lsp[2][i]; }
                err = vlpc_2st_dec(hBs, lsp[1], 2 << s);
                if(err != 0) { return err; }
                break;
        }
    }

    /*** Decode LPC3 ***/
    if((mod[2] < 2)) { /* else: skip LPC3 */
        int32_t nk_mode = 0;
        lpc_present[3] = 1;

        mode_lpc_bin = get_vlclbf_n(hBs, 3);

        switch(mode_lpc_bin) {
            case 1:
                /* LPC3: abs */
                vlpc_1st_dec(hBs, lsp[3]);
                break;
            case 0:
                /* LPC3: mid */
                for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[3][i] = (lsp[2][i] >> 1) + (lsp[4][i] >> 1); }
                nk_mode = 1;
                break;
            case 2:
                /* LPC3: relL */
                for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[3][i] = lsp[2][i]; }
                nk_mode = 2;
                break;
            case 3:
                /* LPC3: relR */
                for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[3][i] = lsp[4][i]; }
                nk_mode = 2;
                break;
        }
        err = vlpc_2st_dec(hBs, lsp[3], nk_mode);
        if(err != 0) { return err; }
    }

    if(!lpc0_available && !last_frame_ok) {
        /* LPC(0) was lost. Use next available LPC(k) instead */
        for(k = 1; k < (nbDiv + 1); k++) {
            if(lpc_present[k]) {
                const double LSF_INIT_TILT = 0.25;
                for(i = 0; i < M_LP_FILTER_ORDER; i++) {
                    if(mod[0] > 0) {
                        lsp[0][i] = FX_DBL2FX_SGL(fMult(lsp[k][i], FL2FXCONST_SGL(1.0f - LSF_INIT_TILT)) +
                                                  fMult(fdk_dec_lsf_init[i], FL2FXCONST_SGL(LSF_INIT_TILT)));
                    }
                    else { lsp[0][i] = lsp[k][i]; }
                }
                break;
            }
        }
    }
    for(i = 0; i < M_LP_FILTER_ORDER; i++) { lpc4_lsf[i] = lsp[4 >> s][i]; }
    {
        int32_t divFac;
        int32_t last, numLpc = 0;

        i = nbDiv;
        do { numLpc += lpc_present[i--]; } while(i >= 0 && numLpc < 3);

        last = i;

        switch(numLpc) {
            case 3:
                divFac = FL2FXCONST_DBL(1.0f / 3.0f);
                break;
            case 2:
                divFac = FL2FXCONST_DBL(1.0f / 2.0f);
                break;
            default:
                divFac = FL2FXCONST_DBL(1.0f);
                break;
        }

        /* get the adaptive mean for the next (bad) frame */
        for(k = 0; k < M_LP_FILTER_ORDER; k++) {
            int32_t tmp = (int32_t)0;
            for(i = nbDiv; i > last; i--) {
                if(lpc_present[i]) { tmp = fMultAdd(tmp >> 1, lsp[i][k], divFac); }
            }
            lsf_adaptive_mean_cand[k] = FX_DBL2FX_SGL(tmp);
        }
    }

    /* calculate stability factor Theta. Needed for ACELP decoder and concealment
     */
    {
        int16_t *lsf_prev, *lsf_curr;
        k = 0;

        assert(lpc_present[0] == 1 && lpc_present[4 >> s] == 1);
        lsf_prev = lsp[0];
        for(i = 1; i < (nbDiv + 1); i++) {
            if(lpc_present[i]) {
                int32_t tmp = (int32_t)0;
                int32_t j;
                lsf_curr = lsp[i];

                /* sum = tmp * 2^(LSF_SCALE*2 + 4) */
                for(j = 0; j < M_LP_FILTER_ORDER; j++) { tmp += fPow2Div2((int16_t)(lsf_curr[j] - lsf_prev[j])) >> 3; }

                /* tmp = (float)(FL2FXCONST_DBL(1.25f) - fMult(tmp,
                 * FL2FXCONST_DBL(1/400000.0f))); */
                tmp = FL2FXCONST_DBL(1.25f / (1 << LSF_SCALE)) - fMult(tmp, FL2FXCONST_DBL((1 << (LSF_SCALE + 4)) / 400000.0f));
                if(tmp >= FL2FXCONST_DBL(1.0f / (1 << LSF_SCALE))) { pStability[k] = FL2FXCONST_SGL(1.0f / 2.0f); }
                else if(tmp < FL2FXCONST_DBL(0.0f)) { pStability[k] = 0; }
                else { pStability[k] = FX_DBL2FX_SGL(tmp << (LSF_SCALE - 1)); }

                lsf_prev = lsf_curr;
                k = i;
            }
            else {
                /* Mark stability value as undefined. */
                pStability[i] = (int16_t)-1;
            }
        }
    }

    /* convert into LSP domain */
    for(i = 0; i < (nbDiv + 1); i++) {
        if(lpc_present[i]) {
            for(k = 0; k < M_LP_FILTER_ORDER; k++) {
                lsp[i][k] = FX_DBL2FX_SGL(fixp_cos(fMult(lsp[i][k], FL2FXCONST_SGL((1 << LSPARG_SCALE) * M_PI / 6400.0)), LSF_SCALE - LSPARG_SCALE));
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void CLpc_Conceal(int16_t lsp[][M_LP_FILTER_ORDER], int16_t lpc4_lsf[M_LP_FILTER_ORDER], int16_t lsf_adaptive_mean[M_LP_FILTER_ORDER],
                  const int32_t first_lpd_flag) {
    int32_t i, j;

    const uint16_t BETA        = 8192;
    const uint16_t ONE_BETA    = 24576;
    const uint16_t BFI_FAC     = 29491;
    const uint16_t ONE_BFI_FAC = 3277;

    /* Frame loss concealment (could be improved) */

    if(first_lpd_flag) {
        /* Reset past LSF values */
        for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[0][i] = lpc4_lsf[i] = fdk_dec_lsf_init[i]; }
    }
    else {
        /* old LPC4 is new LPC0 */
        for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[0][i] = lpc4_lsf[i]; }
    }

    /* LPC1 */
    for(i = 0; i < M_LP_FILTER_ORDER; i++) {
        int16_t lsf_mean = FX_DBL2FX_SGL(fMult(BETA, fdk_dec_lsf_init[i]) + fMult(ONE_BETA, lsf_adaptive_mean[i]));

        lsp[1][i] = FX_DBL2FX_SGL(fMult(BFI_FAC, lpc4_lsf[i]) + fMult(ONE_BFI_FAC, lsf_mean));
    }

    /* LPC2 - LPC4 */
    for(j = 2; j <= 4; j++) {
        for(i = 0; i < M_LP_FILTER_ORDER; i++) {
            /* lsf_mean[i] =  FX_DBL2FX_SGL(fMult((int16_t)(BETA + j *
               FL2FXCONST_SGL(0.1f)), fdk_dec_lsf_init[i])
                                          + fMult((int16_t)(ONE_BETA - j *
               FL2FXCONST_SGL(0.1f)), lsf_adaptive_mean[i])); */

            int16_t lsf_mean = FX_DBL2FX_SGL(fMult((int16_t)(BETA + (int16_t)(j * (int32_t)3277)), (int16_t)fdk_dec_lsf_init[i]) +
                                             fMult((int16_t)(ONE_BETA - (int16_t)(j * (int32_t)3277)), lsf_adaptive_mean[i]));

            lsp[j][i] = FX_DBL2FX_SGL(fMult(BFI_FAC, lsp[j - 1][i]) + fMult(ONE_BFI_FAC, lsf_mean));
        }
    }

    /* Update past values for the future */
    for(i = 0; i < M_LP_FILTER_ORDER; i++) { lpc4_lsf[i] = lsp[4][i]; }

    /* convert into LSP domain */
    for(j = 0; j < 5; j++) {
        for(i = 0; i < M_LP_FILTER_ORDER; i++) {
            lsp[j][i] = FX_DBL2FX_SGL(fixp_cos(fMult(lsp[j][i], FL2FXCONST_SGL((1 << LSPARG_SCALE) * M_PI / 6400.0)), LSF_SCALE - LSPARG_SCALE));
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void E_LPC_a_weight(int16_t *wA, const int16_t *A, int32_t m) {
    int32_t f;
    int32_t i;

    f = FL2FXCONST_DBL(0.92f);
    for(i = 0; i < m; i++) {
        wA[i] = FX_DBL2FX_SGL(fMult(A[i], f));
        f = fMult(f, FL2FXCONST_DBL(0.92f));
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void CLpd_DecodeGain(int32_t *gain, int32_t *gain_e, int32_t gain_code) {
    /* gain * 2^(gain_e) = 10^(gain_code/28) */
    *gain = fLdPow(FL2FXCONST_DBL(3.3219280948873623478703194294894 / 4.0), /* log2(10)*/
                   2, fMultDiv2((int32_t)gain_code << (DFRACT_BITS - 1 - 7), FL2FXCONST_DBL(2.0f / 28.0f)), 7, gain_e);
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief *   Find the polynomial F1(z) or F2(z) from the LSPs.
 * This is performed by expanding the product polynomials:
 *
 * F1(z) =   product   ( 1 - 2 LSP_i z^-1 + z^-2 )
 *         i=0,2,4,6,8
 * F2(z) =   product   ( 1 - 2 LSP_i z^-1 + z^-2 )
 *         i=1,3,5,7,9
 *
 * where LSP_i are the LSPs in the cosine domain.
 * R.A.Salami    October 1990
 * \param lsp input, line spectral freq. (cosine domain)
 * \param f output, the coefficients of F1 or F2, scaled by 8 bits
 * \param n no of coefficients (m/2)
 * \param flag 1 : F1(z) ; 2 : F2(z)
 */

static void get_lsppol(int16_t lsp[], int32_t f[], int32_t n, int32_t flag) {
    int32_t  b;
    int16_t *plsp;
    int32_t  i, j;
    const uint8_t SF_F = 8;

    plsp = lsp + flag - 1;
    f[0] = FL2FXCONST_DBL(1.0f / (1 << SF_F));
    b = -FX_LPC2FX_DBL(*plsp);
    f[1] = b >> (SF_F - 1);
    for(i = 2; i <= n; i++) {
        plsp += 2;
        b = -FX_LPC2FX_DBL(*plsp);
        f[i] = SATURATE_LEFT_SHIFT((fMultDiv2(b, f[i - 1]) + (f[i - 2] >> 1)), 2, DFRACT_BITS);
        for(j = i - 1; j > 1; j--) { f[j] = SATURATE_LEFT_SHIFT(((f[j] >> 2) + fMultDiv2(b, f[j - 1]) + (f[j - 2] >> 2)), 2, DFRACT_BITS); }
        f[1] = f[1] + (b >> (SF_F - 1));
    }
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief lsp input LSP vector
 * \brief a output LP filter coefficient vector scaled by SF_A_COEFFS.
 */
void E_LPC_f_lsp_a_conversion(int16_t *lsp, int16_t *a, int32_t *a_exp) {
    const uint8_t SF_F = 8;
    const uint8_t NC = M_LP_FILTER_ORDER / 2;
    int32_t f1[NC + 1], f2[NC + 1];
    int32_t i, k;

    /*-----------------------------------------------------*
     *  Find the polynomials F1(z) and F2(z)               *
     *-----------------------------------------------------*/

    get_lsppol(lsp, f1, NC, 1);
    get_lsppol(lsp, f2, NC, 2);

    /*-----------------------------------------------------*
     *  Multiply F1(z) by (1+z^-1) and F2(z) by (1-z^-1)   *
     *-----------------------------------------------------*/
    scaleValues(f1, NC + 1, -2);
    scaleValues(f2, NC + 1, -2);

    for(i = NC; i > 0; i--) {
        f1[i] += f1[i - 1];
        f2[i] -= f2[i - 1];
    }

    int32_t aDBL[M_LP_FILTER_ORDER];

    for(i = 1, k = M_LP_FILTER_ORDER - 1; i <= NC; i++, k--) {
        aDBL[i - 1] = f1[i] + f2[i];
        aDBL[k] = f1[i] - f2[i];
    }

    int32_t headroom_a = getScalefactor(aDBL, M_LP_FILTER_ORDER);

    for(i = 0; i < M_LP_FILTER_ORDER; i++) { a[i] = FX_DBL2FX_SGL(aDBL[i] << headroom_a); }

    *a_exp = SF_F + (2 - 1) - headroom_a;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
int32_t *CLpd_FAC_GetMemory(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, uint8_t mod[NB_DIV], int32_t *pState) {
    int32_t *ptr;
    int32_t  i;
    int32_t  k = 0;
    int32_t  max_windows = 8;

    assert(*pState >= 0 && *pState < max_windows);
    /* Look for free space to store FAC data. 2 FAC data blocks fit into each TCX spectral data block. */
    for(i = *pState; i < max_windows; i++) {
        if(mod[i >> 1] == 0) { break; }
    }
    *pState = i + 1;
    if(i == max_windows) { ptr = pAacDecoderChannelInfo->data.usac.fac_data0; }
    else {
        assert(mod[(i >> 1)] == 0);
        ptr = pAacDecoderChannelInfo->pSpectralCoefficient + (i * (pAacDecoderChannelInfo->granuleLength << k));
    }
    return ptr;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
int32_t CLpd_FAC_Read(HANDLE_FDK_BITSTREAM hBs, int32_t *pFac, int8_t *pFacScale, int32_t length, int32_t use_gain, int32_t frame) {
    int32_t fac_gain;
    int32_t fac_gain_e = 0;

    if(use_gain) { CLpd_DecodeGain(&fac_gain, &fac_gain_e, FDKreadBits(hBs, 7)); }
    if(CLpc_DecodeAVQ(hBs, pFac, 1, 1, length) != 0) { return -1; }

    {
        int32_t scale;

        scale = getScalefactor(pFac, length);
        scaleValues(pFac, length, scale);
        pFacScale[frame] = DFRACT_BITS - 1 - scale;
    }

    if(use_gain) {
        int32_t i;

        pFacScale[frame] += fac_gain_e;

        for(i = 0; i < length; i++) { pFac[i] = fMult(pFac[i], fac_gain); }
    }
    return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Syn_filt_zero(const int16_t a[], const int32_t a_exp, int32_t length, int32_t x[]) {
    int32_t i, j;
    int32_t L_tmp;

    for(i = 0; i < length; i++) {
        L_tmp = (int32_t)0;

        for(j = 0; j < fMin(i, M_LP_FILTER_ORDER); j++) { L_tmp -= fMultDiv2(a[j], x[i - (j + 1)]) >> (LP_FILTER_SCALE - 1); }

        L_tmp = scaleValue(L_tmp, a_exp + LP_FILTER_SCALE);
        x[i] = fAddSaturate(x[i], L_tmp);
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/* Table is also correct for coreCoderFrameLength = 768. Factor 3/4 is canceled out: gainFac = 0.5 * sqrt(fac_length/lFrame) */
void CFac_ApplyGains(int32_t fac_data[LFAC], const int32_t fac_length, const int32_t tcx_gain, const int32_t alfd_gains[], const int32_t mod) {
    int32_t facFactor;
    int32_t i;
    static const int32_t gainFac[4] = {0x40000000, 0x2d413ccd, 0x20000000, 0x16a09e66};
    assert((fac_length == 128) || (fac_length == 96));

    /* 2) Apply gain factor to FAC data */
    facFactor = fMult(gainFac[mod], tcx_gain);
    for(i = 0; i < fac_length; i++) { fac_data[i] = fMult(fac_data[i], facFactor); }

    /* 3) Apply spectrum deshaping using alfd_gains */
    for(i = 0; i < fac_length / 4; i++) {
        int32_t k;

        k = i >> (3 - mod);
        fac_data[i] = fMult(fac_data[i], alfd_gains[k]) << 1; /* alfd_gains is scaled by one bit. */
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
static void CFac_CalcFacSignal(int32_t *pOut, int32_t *pFac, const int32_t fac_scale, const int32_t fac_length, const int16_t A[M_LP_FILTER_ORDER],
                               const int32_t A_exp, const int32_t fAddZir, const int32_t isFdFac) {
    int16_t wA[M_LP_FILTER_ORDER];
    int32_t tf_gain = (int32_t)0;
    int32_t wlength;
    int32_t scale = fac_scale;

    /* obtain tranform gain. */
    imdct_gain(&tf_gain, &scale, isFdFac ? 0 : fac_length);

    /* 4) Compute inverse DCT-IV of FAC data. Output scale of DCT IV is 16 bits.
     */
    dct_IV(pFac, fac_length, &scale);
    /* dct_IV scale = log2(fac_length). "- 7" is a factor of 2/128 */
    if(tf_gain != (int32_t)0) { /* non-radix 2 transform gain */
        int32_t i;

        for(i = 0; i < fac_length; i++) { pFac[i] = fMult(tf_gain, pFac[i]); }
    }
    scaleValuesSaturate(pOut, pFac, fac_length, scale); /* Avoid overflow issues and saturate. */

    E_LPC_a_weight(wA, A, M_LP_FILTER_ORDER);

    /* We need the output of the IIR filter to be longer than "fac_length".
    For this reason we run it with zero input appended to the end of the input
    sequence, i.e. we generate its ZIR and extend the output signal.*/
    memset(pOut + fac_length, 0, fac_length * sizeof(int32_t));
    wlength = 2 * fac_length;

    /* 5) Apply weighted synthesis filter to FAC data, including optional Zir (5.
     * item 4). */
    Syn_filt_zero(wA, A_exp, wlength, pOut);
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
int32_t CLpd_FAC_Mdct2Acelp(H_MDCT hMdct, int32_t *output, int32_t *pFac, const int32_t fac_scale, int16_t *A, int32_t A_exp, int32_t nrOutSamples,
                            const int32_t fac_length, const int32_t isFdFac, uint8_t prevWindowShape) {
    int32_t          *pOvl;
    int32_t          *pOut0;
    const FIXP_SPK_t *pWindow;
    int32_t           i, fl, nrSamples = 0;

    assert(fac_length <= 1024 / (4 * 2));

    fl = fac_length * 2;

    pWindow = FDKgetWindowSlope(fl, prevWindowShape);

    /* Adapt window slope length in case of frame loss. */
    if(hMdct->prev_fr != fl) {
        int32_t nl = 0;
        imdct_adapt_parameters(hMdct, &fl, &nl, fac_length, pWindow, nrOutSamples);
        assert(nl == 0);
    }

    if(nrSamples < nrOutSamples) {
        pOut0 = output;
        nrSamples += hMdct->ov_offset;
        /* Purge buffered output. */
        memcpy(pOut0, hMdct->overlap.time, hMdct->ov_offset * sizeof(pOut0[0]));
        hMdct->ov_offset = 0;
    }

    pOvl = hMdct->overlap.freq + hMdct->ov_size - 1;

    if(nrSamples >= nrOutSamples) {
        pOut0 = hMdct->overlap.time + hMdct->ov_offset;
        hMdct->ov_offset += hMdct->prev_nr + fl / 2;
    }
    else {
        pOut0 = output + nrSamples;
        nrSamples += hMdct->prev_nr + fl / 2;
    }
    if(hMdct->prevPrevAliasSymmetry == 0) {
        for(i = 0; i < hMdct->prev_nr; i++) {
            int32_t x = -(*pOvl--);
            *pOut0 = IMDCT_SCALE_DBL(x);
            pOut0++;
        }
    }
    else {
        for(i = 0; i < hMdct->prev_nr; i++) {
            int32_t x = (*pOvl--);
            *pOut0 = IMDCT_SCALE_DBL(x);
            pOut0++;
        }
    }
    hMdct->prev_nr = 0;
    {
        if(pFac != NULL) {
            /* Note: The FAC gain might have been applied directly after bit stream
             * parse in this case. */
            CFac_CalcFacSignal(pOut0, pFac, fac_scale, fac_length, A, A_exp, 0, isFdFac);
        }
        else {
            /* Clear buffer because of the overlap and ADD! */
            memset(pOut0, 0, fac_length * sizeof(int32_t));
        }
    }
    i = 0;
    if(hMdct->prevPrevAliasSymmetry == 0) {
        for(; i < fl / 2; i++) {
            int32_t x0;

            /* Overlap Add */
            x0 = -fMult(*pOvl--, pWindow[i].v.re);

            *pOut0 = fAddSaturate(*pOut0, IMDCT_SCALE_DBL(x0));
            pOut0++;
        }
    }
    else {
        for(; i < fl / 2; i++) {
            int32_t x0;

            /* Overlap Add */
            x0 = fMult(*pOvl--, pWindow[i].v.re);

            *pOut0 = fAddSaturate(*pOut0, IMDCT_SCALE_DBL(x0));
            pOut0++;
        }
    }
    if(hMdct->pFacZir != 0) {           /* this should only happen for ACELP -> TCX20 -> ACELP transition */
        int32_t *pOut = pOut0 - fl / 2; /* fl/2 == fac_length */
        for(i = 0; i < fl / 2; i++) { pOut[i] = fAddSaturate(pOut[i], IMDCT_SCALE_DBL(hMdct->pFacZir[i])); }
        hMdct->pFacZir = NULL;
    }
    hMdct->prev_fr = 0;
    hMdct->prev_nr = 0;
    hMdct->prev_tl = 0;
    hMdct->prevPrevAliasSymmetry = hMdct->prevAliasSymmetry;

    return nrSamples;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
int32_t CLpd_FAC_Acelp2Mdct(H_MDCT hMdct, int32_t *output, int32_t *_pSpec, const int16_t spec_scale[], const int32_t nSpec, int32_t *pFac,
                            const int32_t fac_scale, const int32_t fac_length, int32_t noOutSamples, const int32_t tl, const FIXP_SPK_t *wrs,
                            const int32_t fr, int16_t A[16], int32_t A_exp, CAcelpStaticMem_t *acelp_mem, const int32_t gain,
                            const int32_t last_frame_lost, const int32_t isFdFac, const uint8_t last_lpd_mode, const int32_t k,
                            int32_t currAliasingSymmetry) {
    int32_t          *pCurr, *pOvl, *pSpec;
    const FIXP_SPK_t *pWindow;
    const int16_t    *FacWindowZir_conceal;
    uint8_t           doFacZirConceal = 0;
    int32_t           doDeemph = 1;
    const int16_t    *FacWindowZir, *FacWindowSynth;
    int32_t          *pOut0 = output, *pOut1;
    int32_t           w, i, fl, nl, nr, f_len, nrSamples = 0, s = 0, scale, total_gain_e;
    int32_t          *pF, *pFAC_and_FAC_ZIR = NULL;
    int32_t           total_gain = gain;

    assert(fac_length <= 1024 / (4 * 2));
    switch(fac_length) {
        /* coreCoderFrameLength = 1024 */
        case 128:
            pWindow = SineWindow256;
            FacWindowZir = FacWindowZir128;
            FacWindowSynth = FacWindowSynth128;
            break;
        case 64:
            pWindow = SineWindow128;
            FacWindowZir = FacWindowZir64;
            FacWindowSynth = FacWindowSynth64;
            break;
        case 32:
            pWindow = SineWindow64;
            FacWindowZir = FacWindowZir32;
            FacWindowSynth = FacWindowSynth32;
            break;
        /* coreCoderFrameLength = 768 */
        case 96:
            pWindow = SineWindow192;
            FacWindowZir = FacWindowZir96;
            FacWindowSynth = FacWindowSynth96;
            break;
        case 48:
            pWindow = SineWindow96;
            FacWindowZir = FacWindowZir48;
            FacWindowSynth = FacWindowSynth48;
            break;
        default:
            assert(0);
            return 0;
    }

    FacWindowZir_conceal = FacWindowSynth;
    /* Derive NR and NL */
    fl = fac_length * 2;
    nl = (tl - fl) >> 1;
    nr = (tl - fr) >> 1;
    if(noOutSamples > nrSamples) {
        /* Purge buffered output. */
        memcpy(pOut0, hMdct->overlap.time, hMdct->ov_offset * sizeof(pOut0[0]));
        nrSamples = hMdct->ov_offset;
        hMdct->ov_offset = 0;
    }
    if(nrSamples >= noOutSamples) {
        pOut1 = hMdct->overlap.time + hMdct->ov_offset;
        if(hMdct->ov_offset < fac_length) { pOut0 = output + nrSamples; }
        else { pOut0 = pOut1; }
        hMdct->ov_offset += fac_length + nl;
    }
    else {
        pOut1 = output + nrSamples;
        pOut0 = output + nrSamples;
    }

    {
        pFAC_and_FAC_ZIR = CLpd_ACELP_GetFreeExcMem(acelp_mem, 2 * fac_length);
        {
            const int32_t *pTmp1, *pTmp2;

            doFacZirConceal |= ((last_frame_lost != 0) && (k == 0));
            doDeemph &= (last_lpd_mode != 4);
            if(doFacZirConceal) {
                /* ACELP contribution in concealment case:
                   Use ZIR with a modified ZIR window to preserve some more energy.
                   Dont use FAC, which contains wrong information for concealed frame
                   Dont use last ACELP samples, but double ZIR, instead (afterwards) */
                memset(pFAC_and_FAC_ZIR, 0, 2 * fac_length * sizeof(int32_t));
                FacWindowSynth = (int16_t *)pFAC_and_FAC_ZIR;
                FacWindowZir = FacWindowZir_conceal;
            }
            else { CFac_CalcFacSignal(pFAC_and_FAC_ZIR, pFac, fac_scale + s, fac_length, A, A_exp, 1, isFdFac); }
            /* 6) Get windowed past ACELP samples and ACELP ZIR signal */

            /*
             * Get ACELP ZIR (pFac[]) and ACELP past samples (pOut0[]) and add them
             * to the FAC synth signal contribution on pOut1[].
             */
            {
                {
                    CLpd_Acelp_Zir(A, A_exp, acelp_mem, fac_length, pFac, doDeemph);

                    pTmp1 = pOut0;
                    pTmp2 = pFac;
                }

                for(i = 0, w = 0; i < fac_length; i++) {
                    int32_t x;
                    /* Div2 is compensated by table scaling */
                    x = fMultDiv2(pTmp2[i], FacWindowZir[w]);
                    x += fMultDiv2(pTmp1[-i - 1], FacWindowSynth[w]);
                    pOut1[i] = fAddSaturate(x, pFAC_and_FAC_ZIR[i]);
                    w++;
                }
            }

            if(doFacZirConceal) {
                /* ZIR is the only ACELP contribution, so double it */
                scaleValues(pOut1, fac_length, 1);
            }
        }
    }
    if(nrSamples < noOutSamples) { nrSamples += fac_length + nl; }
    /* Obtain transform gain */
    total_gain = gain;
    total_gain_e = 0;
    imdct_gain(&total_gain, &total_gain_e, tl);
    /* IMDCT overlap add */
    scale = total_gain_e;
    pSpec = _pSpec;
    /* Note:when comming from an LPD frame (TCX/ACELP) the previous alisaing
     * symmetry must always be 0 */
    if(currAliasingSymmetry == 0) { dct_IV(pSpec, tl, &scale); }
    else {
        int32_t  _tmp[1024 + ALIGNMENT_DEFAULT / sizeof(int32_t)];
        int32_t *tmp = (int32_t *)_tmp + (int32_t)8 - (((size_t)(_tmp)&7) & 7);
        dst_III(pSpec, tmp, tl, &scale);
    }

    /* Optional scaling of time domain - no yet windowed - of current spectrum */
    if(total_gain != (int32_t)0) {
        for(i = 0; i < tl; i++) { pSpec[i] = fMult(pSpec[i], total_gain); }
    }
    int32_t loc_scale = fixmin_I(spec_scale[0] + scale, (int32_t)DFRACT_BITS - 1);
    scaleValuesSaturate(pSpec, tl, loc_scale);

    pOut1 += fl / 2 - 1;
    pCurr = pSpec + tl - fl / 2;

    for(i = 0; i < fl / 2; i++) {
        int32_t x1;

        /* FAC signal is already on pOut1, because of that the += operator. */
        x1 = fMult(*pCurr++, pWindow[i].v.re);
        assert((pOut1 >= hMdct->overlap.time && pOut1 < hMdct->overlap.time + hMdct->ov_size) || (pOut1 >= output && pOut1 < output + 1024));
        *pOut1 = fAddSaturate(*pOut1, IMDCT_SCALE_DBL(-x1));
        pOut1--;
    }

    /* NL output samples TL/2+FL/2..TL. - current[FL/2..0] */
    pOut1 += (fl / 2) + 1;

    pFAC_and_FAC_ZIR += fac_length; /* set pointer to beginning of FAC ZIR */

    if(nl == 0) {
        /* save pointer to write FAC ZIR data later */
        hMdct->pFacZir = pFAC_and_FAC_ZIR;
    }
    else {
        assert(nl >= fac_length);
        /* FAC ZIR will be added now ... */
        hMdct->pFacZir = NULL;
    }

    pF = pFAC_and_FAC_ZIR;
    f_len = fac_length;

    pCurr = pSpec + tl - fl / 2 - 1;
    for(i = 0; i < nl; i++) {
        int32_t x = -(*pCurr--);
        /* 5) (item 4) Synthesis filter Zir component, FAC ZIR (another one). */
        if(i < f_len) { x = fAddSaturate(x, *pF++); }

        assert((pOut1 >= hMdct->overlap.time && pOut1 < hMdct->overlap.time + hMdct->ov_size) || (pOut1 >= output && pOut1 < output + 1024));
        *pOut1 = IMDCT_SCALE_DBL(x);
        pOut1++;
    }
    hMdct->prev_nr = nr;
    hMdct->prev_fr = fr;
    hMdct->prev_wrs = wrs;
    hMdct->prev_tl = tl;
    hMdct->prevPrevAliasSymmetry = hMdct->prevAliasSymmetry;
    hMdct->prevAliasSymmetry = currAliasingSymmetry;
    fl = fr;
    nl = nr;
    pOvl = pSpec + tl / 2 - 1;
    pOut0 = pOut1;

    for(w = 1; w < nSpec; w++) /* for ACELP -> FD int16_t */
    {
        const FIXP_SPK_t *pWindow_prev;

        /* Setup window pointers */
        pWindow_prev = hMdct->prev_wrs;

        /* Current spectrum */
        pSpec = _pSpec + w * tl;

        scale = total_gain_e;

        /* For the second, third, etc. int16_t frames the alisaing symmetry is equal,
         * either (0,0) or (1,1) */
        if(currAliasingSymmetry == 0) {
            /* DCT IV of current spectrum */
            dct_IV(pSpec, tl, &scale);
        }
        else { dst_IV(pSpec, tl, &scale); }

        /* Optional scaling of time domain - no yet windowed - of current spectrum
         */
        /* and de-scale current spectrum signal (time domain, no yet windowed) */
        if(total_gain != (int32_t)0) {
            for(i = 0; i < tl; i++) { pSpec[i] = fMult(pSpec[i], total_gain); }
        }
        loc_scale = fixmin_I(spec_scale[w] + scale, (int32_t)DFRACT_BITS - 1);
        scaleValuesSaturate(pSpec, tl, loc_scale);

        if(noOutSamples <= nrSamples) {
            /* Divert output first half to overlap buffer if we already got enough
             * output samples. */
            pOut0 = hMdct->overlap.time + hMdct->ov_offset;
            hMdct->ov_offset += hMdct->prev_nr + fl / 2;
        }
        else {
            /* Account output samples */
            nrSamples += hMdct->prev_nr + fl / 2;
        }

        /* NR output samples 0 .. NR. -overlap[TL/2..TL/2-NR] */
        for(i = 0; i < hMdct->prev_nr; i++) {
            int32_t x = -(*pOvl--);
            *pOut0 = IMDCT_SCALE_DBL(x);
            pOut0++;
        }

        if(noOutSamples <= nrSamples) {
            /* Divert output second half to overlap buffer if we already got enough
             * output samples. */
            pOut1 = hMdct->overlap.time + hMdct->ov_offset + fl / 2 - 1;
            hMdct->ov_offset += fl / 2 + nl;
        }
        else {
            pOut1 = pOut0 + (fl - 1);
            nrSamples += fl / 2 + nl;
        }

        /* output samples before window crossing point NR .. TL/2.
         * -overlap[TL/2-NR..TL/2-NR-FL/2] + current[NR..TL/2] */
        /* output samples after window crossing point TL/2 .. TL/2+FL/2.
         * -overlap[0..FL/2] - current[TL/2..FL/2] */
        pCurr = pSpec + tl - fl / 2;
        if(currAliasingSymmetry == 0) {
            for(i = 0; i < fl / 2; i++) {
                int32_t x0, x1;

                cplxMultDiv2(&x1, &x0, *pCurr++, -*pOvl--, pWindow_prev[i]);
                *pOut0 = IMDCT_SCALE_DBL_LSH1(x0);
                *pOut1 = IMDCT_SCALE_DBL_LSH1(-x1);
                pOut0++;
                pOut1--;
            }
        }
        else {
            if(hMdct->prevPrevAliasSymmetry == 0) {
                /* Jump DST II -> DST IV for the second window */
                for(i = 0; i < fl / 2; i++) {
                    int32_t x0, x1;

                    cplxMultDiv2(&x1, &x0, *pCurr++, -*pOvl--, pWindow_prev[i]);
                    *pOut0 = IMDCT_SCALE_DBL_LSH1(x0);
                    *pOut1 = IMDCT_SCALE_DBL_LSH1(x1);
                    pOut0++;
                    pOut1--;
                }
            }
            else {
                /* Jump DST IV -> DST IV from the second window on */
                for(i = 0; i < fl / 2; i++) {
                    int32_t x0, x1;

                    cplxMultDiv2(&x1, &x0, *pCurr++, *pOvl--, pWindow_prev[i]);
                    *pOut0 = IMDCT_SCALE_DBL_LSH1(x0);
                    *pOut1 = IMDCT_SCALE_DBL_LSH1(x1);
                    pOut0++;
                    pOut1--;
                }
            }
        }

        if(hMdct->pFacZir != 0) {
            /* add FAC ZIR of previous ACELP -> mdct transition */
            int32_t *pOut = pOut0 - fl / 2;
            assert(fl / 2 <= 128);
            for(i = 0; i < fl / 2; i++) { pOut[i] = fAddSaturate(pOut[i], IMDCT_SCALE_DBL(hMdct->pFacZir[i])); }
            hMdct->pFacZir = NULL;
        }
        pOut0 += (fl / 2);

        /* NL output samples TL/2+FL/2..TL. - current[FL/2..0] */
        pOut1 += (fl / 2) + 1;
        pCurr = pSpec + tl - fl / 2 - 1;
        for(i = 0; i < nl; i++) {
            int32_t x = -(*pCurr--);
            *pOut1 = IMDCT_SCALE_DBL(x);
            pOut1++;
        }

        /* Set overlap source pointer for next window pOvl = pSpec + tl/2 - 1; */
        pOvl = pSpec + tl / 2 - 1;

        /* Previous window values. */
        hMdct->prev_nr = nr;
        hMdct->prev_fr = fr;
        hMdct->prev_tl = tl;
        hMdct->prev_wrs = pWindow_prev;
        hMdct->prevPrevAliasSymmetry = hMdct->prevAliasSymmetry;
        hMdct->prevAliasSymmetry = currAliasingSymmetry;
    }
    /* Save overlap */
    pOvl = hMdct->overlap.freq + hMdct->ov_size - tl / 2;
    assert(pOvl >= hMdct->overlap.time + hMdct->ov_offset);
    assert(tl / 2 <= hMdct->ov_size);
    for(i = 0; i < tl / 2; i++) { pOvl[i] = _pSpec[i + (w - 1) * tl]; }

    return nrSamples;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void E_UTIL_preemph(const int32_t *in, int32_t *out, int32_t L) {
    int32_t i;
    for(i = 0; i < L; i++) { out[i] = fAddSaturate(in[i], -fMult(PREEMPH_FAC, in[i - 1])); }
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Preemph_code(FIXP_COD x[] ) {/* (i/o)   : input signal overwritten by the output */
    int32_t i;
    int32_t L_tmp;
    /* ARM926: 12 cycles per sample */
    for(i = L_SUBFR - 1; i > 0; i--) {
        L_tmp = FX_COD2FX_DBL(x[i]);
        L_tmp -= fMultDiv2(x[i - 1], TILT_CODE2);
        x[i] = FX_DBL2FX_SGL(L_tmp) + 0x8000;
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
static void Pit_shrp(FIXP_COD x[], int32_t  pit_lag) {
    int32_t i;
    int32_t L_tmp;

    for(i = pit_lag; i < L_SUBFR; i++) {
        L_tmp = FX_COD2FX_DBL(x[i]);
        L_tmp += fMult(x[i - pit_lag], PIT_SHARP);
        x[i] = FX_DBL2FX_SGL(L_tmp) + 0x8000;
    }
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
static void D_gain2_plus(int32_t index, FIXP_COD code[], int16_t *gain_pit, int32_t *gain_code, int32_t mean_ener_bits, int32_t bfi,
                         int16_t *past_gpit, int32_t *past_gcode, int32_t *pEner_code, int32_t *pEner_code_e) {
    const uint8_t SF_MEAN_ENER_LG10 = 9; /* pow(10.0, {18, 30, 42, 54}/20.0) /(float)(1<<SF_MEAN_ENER_LG10) */
    const int32_t pow_10_mean_energy[4] = {0x01fc5ebd, 0x07e7db92, 0x1f791f65, 0x7d4bfba3};
    int32_t Ltmp;
    int32_t gcode0, gcode_inov;
    int32_t gcode0_e, gcode_inov_e;
    int32_t i;

    int32_t ener_code;
    int32_t ener_code_e;

    /* ener_code = sum(code[]^2) */
    ener_code = int32_t(0);
    for(i = 0; i < L_SUBFR; i++) { ener_code += fPow2Div2(code[i]); }
    ener_code_e = fMax(fNorm(ener_code) - 1, 0);
    ener_code <<= ener_code_e;
    ener_code_e = 2 * SF_CODE + 1 - ener_code_e;
    /* export energy of code for calc_period_factor() */
    *pEner_code = ener_code;
    *pEner_code_e = ener_code_e;
    ener_code += scaleValue(FL2FXCONST_DBL(0.01f), -ener_code_e);
    /* ener_code *= 1/L_SUBFR, and make exponent even (because of square root
     * below). */
    if(ener_code_e & 1) {
        ener_code_e -= 5;
        ener_code >>= 1;
    }
    else { ener_code_e -= 6; }
    gcode_inov = invSqrtNorm2(ener_code, &gcode0_e);
    gcode_inov_e = gcode0_e - (ener_code_e >> 1);

    if(bfi) {
        int32_t tgcode;
        int16_t tgpit;
        tgpit = *past_gpit;
        if(tgpit > FL2FXCONST_SGL(0.95f / (1 << SF_GAIN_P))) { tgpit = FL2FXCONST_SGL(0.95f / (1 << SF_GAIN_P)); }
        else if(tgpit < FL2FXCONST_SGL(0.5f / (1 << SF_GAIN_P))) { tgpit = FL2FXCONST_SGL(0.5f / (1 << SF_GAIN_P)); }
        *gain_pit = tgpit;
        tgpit = FX_DBL2FX_SGL(fMult(tgpit, FL2FXCONST_DBL(0.95f)));
        *past_gpit = tgpit;
        tgpit = FL2FXCONST_SGL(1.4f / (1 << SF_GAIN_P)) - tgpit;
        tgcode = fMult(*past_gcode, tgpit) << SF_GAIN_P;
        *gain_code = scaleValue(fMult(tgcode, gcode_inov), gcode_inov_e);
        *past_gcode = tgcode;
        return;
    }
    /*-------------- Decode gains ---------------*/
    /*
     gcode0 = pow(10.0, (float)mean_ener/20.0);
     gcode0 = gcode0 / sqrt(ener_code/L_SUBFR);
     */
    gcode0 = pow_10_mean_energy[mean_ener_bits];
    gcode0 = fMultDiv2(gcode0, gcode_inov);
    gcode0_e = gcode0_e + SF_MEAN_ENER_LG10 - (ener_code_e >> 1) + 1;

    i = index << 1;
    *gain_pit = fdk_t_qua_gain7b[i]; /* adaptive codebook gain */
    /* t_qua_gain[ind2p1] : fixed codebook gain correction factor */
    Ltmp = fMult(fdk_t_qua_gain7b[i + 1], gcode0);
    *gain_code = scaleValue(Ltmp, gcode0_e - SF_GAIN_C + SF_QUA_GAIN7B);

    /* update bad frame handler */
    *past_gpit = *gain_pit;

    /*--------------------------------------------------------
      past_gcode  = gain_code/gcode_inov
     --------------------------------------------------------*/
    {
        int32_t gcode_m;
        int32_t gcode_e;

        gcode_m = fDivNormHighPrec(Ltmp, gcode_inov, &gcode_e);
        gcode_e += (gcode0_e - SF_GAIN_C + SF_QUA_GAIN7B) - (gcode_inov_e);
        *past_gcode = scaleValue(gcode_m, gcode_e);
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
static int32_t calc_period_factor(int32_t exc[], int16_t gain_pit, int32_t gain_code, int32_t ener_code, int32_t ener_code_e) {
    int32_t ener_exc_e, L_tmp_e, s = 0;
    int32_t ener_exc, L_tmp;
    int32_t period_fac;
    /* energy of pitch excitation */
    ener_exc = (int32_t)0;
    for(int32_t i = 0; i < L_SUBFR; i++) {
        ener_exc += fPow2Div2(exc[i]) >> s;
        if(ener_exc >= FL2FXCONST_DBL(0.5f)) {
            ener_exc >>= 1;
            s++;
        }
    }
    ener_exc_e = fNorm(ener_exc);
    ener_exc = fMult(ener_exc << ener_exc_e, fPow2(gain_pit));
    if(ener_exc != (int32_t)0) { ener_exc_e = 2 * SF_EXC + 1 + 2 * SF_GAIN_P - ener_exc_e + s; }
    else { ener_exc_e = 0; }
    /* energy of innovative code excitation */
    /* L_tmp = ener_code * gain_code*gain_code; */
    L_tmp_e = fNorm(gain_code);
    L_tmp = fPow2(gain_code << L_tmp_e);
    L_tmp = fMult(ener_code, L_tmp);
    L_tmp_e = 2 * SF_GAIN_C + ener_code_e - 2 * L_tmp_e;
    /* Find common exponent */
    {
        int32_t num, den;
        int32_t exp_diff;

        exp_diff = ener_exc_e - L_tmp_e;
        if(exp_diff >= 0) {
            ener_exc >>= 1;
            if(exp_diff <= DFRACT_BITS - 2) { L_tmp >>= exp_diff + 1; }
            else { L_tmp = (int32_t)0; }
            den = ener_exc + L_tmp;
            if(ener_exc_e < DFRACT_BITS - 1) { den += scaleValue(FL2FXCONST_DBL(0.01f), -ener_exc_e - 1); }
        }
        else {
            if(exp_diff >= -(DFRACT_BITS - 2)) { ener_exc >>= 1 - exp_diff; }
            else { ener_exc = (int32_t)0; }
            L_tmp >>= 1;
            den = ener_exc + L_tmp;
            if(L_tmp_e < DFRACT_BITS - 1) { den += scaleValue(FL2FXCONST_DBL(0.01f), -L_tmp_e - 1); }
        }
        num = (ener_exc - L_tmp);
        num >>= SF_PFAC;

        if(den > (int32_t)0) {
            if(ener_exc > L_tmp) { period_fac = schur_div(num, den, 16); }
            else { period_fac = -schur_div(-num, den, 16); }
        }
        else { period_fac = (int32_t)MAXVAL_DBL; }
    }
    /* exponent = SF_PFAC */
    return period_fac;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
static int32_t noise_enhancer(int32_t gain_code, int32_t period_fac, int16_t stab_fac, int32_t *p_gc_threshold) {
    int32_t fac, L_tmp, gc_thres;

    gc_thres = *p_gc_threshold;
    L_tmp = gain_code;
    if(L_tmp < gc_thres) {
        L_tmp += fMultDiv2(gain_code, FL2FXCONST_SGL(2.0 * 0.19f)); /* +1.5dB => *(1.0+0.19) */
        if(L_tmp > gc_thres) { L_tmp = gc_thres; }
    }
    else {
        L_tmp = fMult(gain_code, FL2FXCONST_SGL(1.0f / 1.19f)); /* -1.5dB => *10^(-1.5/20) */
        if(L_tmp < gc_thres) { L_tmp = gc_thres; }
    }
    *p_gc_threshold = L_tmp;

    /* voicing factor     lambda = 0.5*(1-period_fac) */
    /* gain smoothing factor S_m = lambda*stab_fac  (=fac)
                                 = 0.5(stab_fac - stab_fac * period_fac) */
    fac = (FX_SGL2FX_DBL(stab_fac) >> (SF_PFAC + 1)) - fMultDiv2(stab_fac, period_fac);
    /* fac_e = SF_PFAC + SF_STAB */
    assert(fac >= (int32_t)0);
    /* gain_code = (float)((fac*tmp) + ((1.0-fac)*gain_code)); */
    gain_code = fMult(fac, L_tmp) - fMult(FL2FXCONST_DBL(-1.0f / (1 << (SF_PFAC + SF_STAB))) + fac, gain_code);
    gain_code <<= (SF_PFAC + SF_STAB);

    return gain_code;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------

void BuildAdaptiveExcitation(FIXP_COD code[],             /* (i) : algebraic codevector c(n)             Q9  */
                             int32_t  exc[],              /* (io): filtered adaptive codebook v(n)       Q15 */
                             int16_t  gain_pit,           /* (i) : adaptive codebook gain g_p            Q14 */
                             int32_t  gain_code,          /* (i) : innovative codebook gain g_c          Q16 */
                             int32_t  gain_code_smoothed, /* (i) : smoothed innov. codebook gain g_sc
                                                              Q16 */
                             int32_t period_fac,          /* (i) : periodicity factor r_v                Q15 */
                             int32_t exc2[]               /* (o) : post-processed excitation u(n)        Q15 */
) {
/* Note: code[L_SUBFR] and exc2[L_SUBFR] share the same memory!
         If exc2[i] is written, code[i] will be destroyed!
*/
#define SF_HEADROOM (1)
#define SF          (SF_CODE + SF_GAIN_C + 1 - SF_EXC - SF_HEADROOM)
#define SF_GAIN_P2  (SF_GAIN_P - SF_HEADROOM)

    int32_t i;
    int32_t tmp, cpe, code_smooth_prev, code_smooth;

    FIXP_COD code_i;
    int32_t  cpe_code_smooth, cpe_code_smooth_prev;

    /* cpe = (1+r_v)/8 * 2 ; ( SF = -1) */
    cpe = (period_fac >> (2 - SF_PFAC)) + FL2FXCONST_DBL(0.25f);

    /* u'(n) */
    tmp = fMultDiv2(*exc, gain_pit) << (SF_GAIN_P2 + 1); /* v(0)*g_p */
    *exc++ = (tmp + (fMultDiv2(code[0], gain_code) << SF)) << SF_HEADROOM;

    /* u(n) */
    code_smooth_prev = fMultDiv2(*code++, gain_code_smoothed) << SF; /* c(0) * g_sc */
    code_i = *code++;
    code_smooth = fMultDiv2(code_i, gain_code_smoothed) << SF; /* c(1) * g_sc */
    tmp += code_smooth_prev;                                   /* tmp = v(0)*g_p + c(0)*g_sc */
    cpe_code_smooth = fMultDiv2(cpe, code_smooth);
    *exc2++ = (tmp - cpe_code_smooth) << SF_HEADROOM;
    cpe_code_smooth_prev = fMultDiv2(cpe, code_smooth_prev);

    i = L_SUBFR - 2;
    do /* ARM926: 22 cycles per iteration */
    {
        /* u'(n) */
        tmp = fMultDiv2(*exc, gain_pit) << (SF_GAIN_P2 + 1);
        *exc++ = (tmp + (fMultDiv2(code_i, gain_code) << SF)) << SF_HEADROOM;
        /* u(n) */
        tmp += code_smooth; /* += g_sc * c(i) */
        tmp -= cpe_code_smooth_prev;
        cpe_code_smooth_prev = cpe_code_smooth;
        code_i = *code++;
        code_smooth = fMultDiv2(code_i, gain_code_smoothed) << SF;
        cpe_code_smooth = fMultDiv2(cpe, code_smooth);
        *exc2++ = (tmp - cpe_code_smooth) << SF_HEADROOM; /* tmp - c_pe * g_sc * c(i+1) */
    } while(--i != 0);

    /* u'(n) */
    tmp = fMultDiv2(*exc, gain_pit) << (SF_GAIN_P2 + 1);
    *exc = (tmp + (fMultDiv2(code_i, gain_code) << SF)) << SF_HEADROOM;
    /* u(n) */
    tmp += code_smooth;
    tmp -= cpe_code_smooth_prev;
    *exc2++ = tmp << SF_HEADROOM;

    return;
}

/**
 * \brief Interpolate LPC vector in LSP domain for current subframe and convert
 * to LP domain
 * \param[in] lsp_old LPC vector (LSP domain) corresponding to the beginning of
 * current ACELP frame.
 * \param[in] lsp_new LPC vector (LSP domain) corresponding to the end of
 * current ACELP frame.
 * \param[in] subfr_nr number of current ACELP subframe 0..3.
 * \param[in] nb_subfr total number of ACELP subframes in this frame.
 * \param[out] A LP filter coefficients for current ACELP subframe, exponent =
 * SF_A_COEFFS.
 */
/* static */
void int_lpc_acelp(const int16_t lsp_old[],                         /* input : LSPs from past frame              */
                   const int16_t lsp_new[],                         /* input : LSPs from present frame           */
                   int32_t subfr_nr, int32_t nb_subfr, int16_t A[], /* output: interpolated LP coefficients for current subframe */
                   int32_t *A_exp) {
    int32_t i;
    int16_t lsp_interpol[M_LP_FILTER_ORDER];
    int16_t fac_old, fac_new;

    assert((nb_subfr == 3) || (nb_subfr == 4));

    fac_old = lsp_interpol_factor[nb_subfr & 0x1][(nb_subfr - 1) - subfr_nr];
    fac_new = lsp_interpol_factor[nb_subfr & 0x1][subfr_nr];
    for(i = 0; i < M_LP_FILTER_ORDER; i++) {
        lsp_interpol[i] = FX_DBL2FX_SGL((fMultDiv2(lsp_old[i], fac_old) + fMultDiv2(lsp_new[i], fac_new)) << 1);
    }

    E_LPC_f_lsp_a_conversion(lsp_interpol, A, A_exp);

    return;
}

/**
 * \brief Perform LP synthesis by filtering the post-processed excitation u(n)
 *        through the LP synthesis filter 1/A(z)
 * \param[in] a LP filter coefficients, exponent = SF_A_COEFFS.
 * \param[in] length length of input/output signal.
 * \param[in] x post-processed excitation u(n).
 * \param[in,out] y LP synthesis signal and filter memory
 * y[-M_LP_FILTER_ORDER..-1].
 */

/* static */
void Syn_filt(const int16_t a[],                   /* (i) : a[m] prediction coefficients Q12 */
              const int32_t a_exp, int32_t length, /* (i) : length of input/output signal (64|128)   */
              int32_t x[],                         /* (i) : input signal Qx  */
              int32_t y[]                          /* (i/o) : filter states / output signal  Qx-s*/
) {
    int32_t i, j;
    int32_t L_tmp;

    for(i = 0; i < length; i++) {
        L_tmp = (int32_t)0;

        for(j = 0; j < M_LP_FILTER_ORDER; j++) { L_tmp -= fMultDiv2(a[j], y[i - (j + 1)]) >> (LP_FILTER_SCALE - 1); }

        L_tmp = scaleValue(L_tmp, a_exp + LP_FILTER_SCALE);
        y[i] = fAddSaturate(L_tmp, x[i]);
    }

    return;
}

/**
 * \brief Calculate de-emphasis 1/(1 - mu z^-1) on input signal.
 * \param[in] x input signal.
 * \param[out] y output signal.
 * \param[in] L length of signal.
 * \param[in,out] mem memory (signal[-1]).
 */
/* static */
void Deemph(int32_t *x, int32_t *y, int32_t L, int32_t *mem) {
    int32_t i;
    int32_t yi = *mem;

    for(i = 0; i < L; i++) {
        int32_t xi = x[i] >> 1;
        xi = fMultAddDiv2(xi, PREEMPH_FAC, yi);
        yi = SATURATE_LEFT_SHIFT(xi, 1, 32);
        y[i] = yi;
    }
    *mem = yi;
    return;
}

/**
 * \brief Compute the LP residual by filtering the input speech through the
 * analysis filter A(z).
 * \param[in] a LP filter coefficients, exponent = SF_A_COEFFS
 * \param[in] x input signal (note that values x[-m..-1] are needed), exponent =
 * SF_SYNTH
 * \param[out] y output signal (residual), exponent = SF_EXC
 * \param[in] l length of filtering
 */
/* static */
void E_UTIL_residu(const int16_t *a, const int32_t a_exp, int32_t *x, int32_t *y, int32_t l) {
    int32_t s;
    int32_t i, j;

    /* (note that values x[-m..-1] are needed) */
    for(i = 0; i < l; i++) {
        s = (int32_t)0;

        for(j = 0; j < M_LP_FILTER_ORDER; j++) { s += fMultDiv2(a[j], x[i - j - 1]) >> (LP_FILTER_SCALE - 1); }

        s = scaleValue(s, a_exp + LP_FILTER_SCALE);
        y[i] = fAddSaturate(s, x[i]);
    }

    return;
}

/* use to map subfr number to number of bits used for acb_index */
static const uint8_t num_acb_idx_bits_table[2][NB_SUBFR] = {
    {9, 6, 9, 6}, /* coreCoderFrameLength == 1024 */
    {9, 6, 6, 0}  /* coreCoderFrameLength == 768  */
};

static int32_t DecodePitchLag(HANDLE_FDK_BITSTREAM hBs, const uint8_t num_acb_idx_bits, const int32_t PIT_MIN, /* TMIN */
                              const int32_t PIT_FR2,                                                           /* TFR2 */
                              const int32_t PIT_FR1,                                                           /* TFR1 */
                              const int32_t PIT_MAX,                                                           /* TMAX */
                              int32_t *pT0, int32_t *pT0_frac, int32_t *pT0_min, int32_t *pT0_max) {
    int32_t acb_idx;
    int32_t error = 0;
    int32_t T0, T0_frac;

    assert((num_acb_idx_bits == 9) || (num_acb_idx_bits == 6));

    acb_idx = FDKreadBits(hBs, num_acb_idx_bits);

    if(num_acb_idx_bits == 6) {
        /* When the pitch value is encoded on 6 bits, a pitch resolution of 1/4 is
           always used in the range [T1-8, T1+7.75], where T1 is nearest integer to
           the fractional pitch lag of the previous subframe.
        */
        T0 = *pT0_min + acb_idx / 4;
        T0_frac = acb_idx & 0x3;
    }
    else { /* num_acb_idx_bits == 9 */
        /* When the pitch value is encoded on 9 bits, a fractional pitch delay is
           used with resolutions 0.25 in the range [TMIN, TFR2-0.25], resolutions
           0.5 in the range [TFR2, TFR1-0.5], and integers only in the range [TFR1,
           TMAX]. NOTE: for small sampling rates TMAX can get smaller than TFR1.
        */
        int32_t T0_min, T0_max;

        if(acb_idx < (PIT_FR2 - PIT_MIN) * 4) {
            /* first interval with 0.25 pitch resolution */
            T0 = PIT_MIN + (acb_idx / 4);
            T0_frac = acb_idx & 0x3;
        }
        else if(acb_idx < ((PIT_FR2 - PIT_MIN) * 4 + (PIT_FR1 - PIT_FR2) * 2)) {
            /* second interval with 0.5 pitch resolution */
            acb_idx -= (PIT_FR2 - PIT_MIN) * 4;
            T0 = PIT_FR2 + (acb_idx / 2);
            T0_frac = (acb_idx & 0x1) * 2;
        }
        else {
            /* third interval with 1.0 pitch resolution */
            T0 = acb_idx + PIT_FR1 - ((PIT_FR2 - PIT_MIN) * 4) - ((PIT_FR1 - PIT_FR2) * 2);
            T0_frac = 0;
        }
        /* find T0_min and T0_max for subframe 1 or 3 */
        T0_min = T0 - 8;
        if(T0_min < PIT_MIN) { T0_min = PIT_MIN; }
        T0_max = T0_min + 15;
        if(T0_max > PIT_MAX) {
            T0_max = PIT_MAX;
            T0_min = T0_max - 15;
        }
        *pT0_min = T0_min;
        *pT0_max = T0_max;
    }
    *pT0 = T0;
    *pT0_frac = T0_frac;

    return error;
}
static void ConcealPitchLag(CAcelpStaticMem_t *acelp_mem, const int32_t PIT_MAX, int32_t *pT0, int32_t *pT0_frac) {
    uint16_t *pold_T0 = &acelp_mem->old_T0;
    uint8_t  *pold_T0_frac = &acelp_mem->old_T0_frac;

    if((int32_t)*pold_T0 >= PIT_MAX) { *pold_T0 = (uint8_t)(PIT_MAX - 5); }
    *pT0 = (int32_t)*pold_T0;
    *pT0_frac = (int32_t)*pold_T0_frac;
}

static uint8_t tab_coremode2nbits[8] = {20, 28, 36, 44, 52, 64, 12, 16};

static int32_t MapCoreMode2NBits(int32_t core_mode) { return (int32_t)tab_coremode2nbits[core_mode]; }

void CLpd_AcelpDecode(CAcelpStaticMem_t *acelp_mem, int32_t i_offset, const int16_t lsp_old[M_LP_FILTER_ORDER],
                      const int16_t lsp_new[M_LP_FILTER_ORDER], int16_t stab_fac, CAcelpChannelData_t *pAcelpData, int32_t numLostSubframes,
                      int32_t lastLpcLost, int32_t frameCnt, int32_t synth[], int32_t pT[], int32_t *pit_gain, int32_t coreCoderFrameLength) {
    int32_t i_subfr, subfr_nr, l_div, T;
    int32_t T0 = -1, T0_frac = -1; /* mark invalid */

    int32_t pit_gain_index = 0;

    const int32_t PIT_MAX = PIT_MAX_12k8 + (6 * i_offset); /* maximum pitch lag */

    FIXP_COD *code;
    int32_t  *exc2;
    int32_t  *syn;
    int32_t  *exc;
    int16_t   A[M_LP_FILTER_ORDER];
    int32_t   A_exp;

    int32_t period_fac;
    int16_t gain_pit;
    int32_t gain_code, gain_code_smooth, Ener_code;
    int32_t Ener_code_e;
    int32_t n;
    int32_t bfi = (numLostSubframes > 0) ? 1 : 0;

    int32_t exc_buf[PIT_MAX_MAX + L_INTERPOL + L_DIV_1024 + 1]; /* 411 + 17 + 256 + 1 = 685 */
    int32_t syn_buf[M_LP_FILTER_ORDER + L_DIV_1024];            /* 16 + 256 = 272 */

    /* use same memory for code[L_SUBFR] and exc2[L_SUBFR] */
    int32_t tmp_buf[L_SUBFR]; /* 64 */
    /* make sure they don't overlap if they are accessed alternatingly in
     * BuildAdaptiveExcitation() */
#if(COD_BITS == FRACT_BITS)
    code = (FIXP_COD *)(tmp_buf + L_SUBFR / 2);
#elif(COD_BITS == DFRACT_BITS)
    code = (FIXP_COD *)tmp_buf;
#endif
    exc2 = (int32_t *)tmp_buf;

    syn = syn_buf + M_LP_FILTER_ORDER;
    exc = exc_buf + PIT_MAX_MAX + L_INTERPOL;

    memcpy(syn_buf, acelp_mem->old_syn_mem, M_LP_FILTER_ORDER * sizeof(int32_t));
    memcpy(exc_buf, acelp_mem->old_exc_mem, (PIT_MAX_MAX + L_INTERPOL) * sizeof(int32_t));

    memset(exc_buf + (PIT_MAX_MAX + L_INTERPOL), 0, (L_DIV + 1) * sizeof(int32_t));

    l_div = coreCoderFrameLength / NB_DIV;

    for(i_subfr = 0, subfr_nr = 0; i_subfr < l_div; i_subfr += L_SUBFR, subfr_nr++) {
        /*-------------------------------------------------*
         * - Decode pitch lag (T0 and T0_frac)             *
         *-------------------------------------------------*/
        if(bfi) { ConcealPitchLag(acelp_mem, PIT_MAX, &T0, &T0_frac); }
        else {
            T0 = (int32_t)pAcelpData->T0[subfr_nr];
            T0_frac = (int32_t)pAcelpData->T0_frac[subfr_nr];
        }

        /*-------------------------------------------------*
         * - Find the pitch gain, the interpolation filter *
         *   and the adaptive codebook vector.             *
         *-------------------------------------------------*/
        Pred_lt4(&exc[i_subfr], T0, T0_frac);

        if((!bfi && pAcelpData->ltp_filtering_flag[subfr_nr] == 0) || (bfi && numLostSubframes == 1 && stab_fac < FL2FXCONST_SGL(0.25f))) {
            /* find pitch excitation with lp filter: v'(n) => v(n) */
            Pred_lt4_postfilter(&exc[i_subfr]);
        }

        /*-------------------------------------------------------*
         * - Decode innovative codebook.                         *
         * - Add the fixed-gain pitch contribution to code[].    *
         *-------------------------------------------------------*/
        if(bfi) {
            for(n = 0; n < L_SUBFR; n++) { code[n] = (FX_DBL2FX_SGL((int16_t)E_UTIL_random(&acelp_mem->seed_ace)) + 0x8000) >> 4; }
        }
        else {
            int32_t nbits = MapCoreMode2NBits((int32_t)pAcelpData->acelp_core_mode);
            D_ACELP_decode_4t64(pAcelpData->icb_index[subfr_nr], nbits, &code[0]);
        }

        T = T0;
        if(T0_frac > 2) { T += 1; }

        Preemph_code(code);
        Pit_shrp(code, T);

        /* Output pitch lag for bass post-filter */
        if(T > PIT_MAX) { pT[subfr_nr] = PIT_MAX; }
        else { pT[subfr_nr] = T; }
        D_gain2_plus(pAcelpData->gains[subfr_nr], code,                              /* (i)  : Innovative code vector, exponent = SF_CODE */
                     &gain_pit,                                                      /* (o)  : Quantized pitch gain, exponent = SF_GAIN_P */
                     &gain_code,                                                     /* (o)  : Quantized codebook gain                    */
                     pAcelpData->mean_energy,                                        /* (i)  : mean_ener defined in open-loop (2 bits) */
                     bfi, &acelp_mem->past_gpit, &acelp_mem->past_gcode, &Ener_code, /* (o)  : Innovative code vector energy              */
                     &Ener_code_e);                                                  /* (o)  : Innovative code vector energy exponent     */

        pit_gain[pit_gain_index++] = FX_SGL2FX_DBL(gain_pit);

        /* calc periodicity factor r_v */
        period_fac = calc_period_factor(               /* (o) : factor (-1=unvoiced to 1=voiced)    */
                                        &exc[i_subfr], /* (i) : pitch excitation, exponent =
                                                          SF_EXC */
                                        gain_pit,      /* (i) : gain of pitch, exponent =
                                                          SF_GAIN_P */
                                        gain_code,     /* (i) : gain of code     */
                                        Ener_code,     /* (i) : Energy of code[]     */
                                        Ener_code_e);  /* (i) : Exponent of energy of code[]
                                                        */

        if(lastLpcLost && frameCnt == 0) {
            if(gain_pit > FL2FXCONST_SGL(1.0f / (1 << SF_GAIN_P))) { gain_pit = FL2FXCONST_SGL(1.0f / (1 << SF_GAIN_P)); }
        }

        gain_code_smooth = noise_enhancer(            /* (o) : smoothed gain g_sc exponent = SF_GAIN_C */
                                          gain_code,  /* (i) : Quantized codebook gain  */
                                          period_fac, /* (i) : periodicity factor (-1=unvoiced to
                                                         1=voiced)  */
                                          stab_fac,   /* (i) : stability factor (0 <= ... < 1),
                                                         exponent = 1 */
                                          &acelp_mem->gc_threshold);

        /* Compute adaptive codebook update u'(n), pitch enhancement c'(n) and
         * post-processed excitation u(n). */
        BuildAdaptiveExcitation(code, exc + i_subfr, gain_pit, gain_code, gain_code_smooth, period_fac, exc2);

        /* Interpolate filter coeffs for current subframe in lsp domain and convert
         * to LP domain */
        int_lpc_acelp(lsp_old,                         /* input : LSPs from past frame              */
                      lsp_new,                         /* input : LSPs from present frame           */
                      subfr_nr,                        /* input : ACELP subframe index              */
                      coreCoderFrameLength / L_DIV, A, /* output: LP coefficients of this subframe  */
                      &A_exp);

        Syn_filt(A,              /* (i) : a[m] prediction coefficients               */
                 A_exp, L_SUBFR, /* (i) : length */
                 exc2,           /* (i) : input signal                               */
                 &syn[i_subfr]   /* (i/o) : filter states / output signal */
        );

    } /* end of subframe loop */

    /* update pitch value for bfi procedure */
    acelp_mem->old_T0_frac = T0_frac;
    acelp_mem->old_T0 = T0;

    /* save old excitation and old synthesis memory for next ACELP frame */
    memcpy(acelp_mem->old_exc_mem, exc + l_div - (PIT_MAX_MAX + L_INTERPOL), sizeof(int32_t) * (PIT_MAX_MAX + L_INTERPOL));
    memcpy(acelp_mem->old_syn_mem, syn_buf + l_div, sizeof(int32_t) * M_LP_FILTER_ORDER);

    Deemph(syn, synth, l_div, &acelp_mem->de_emph_mem); /* ref soft: mem = synth[-1] */

    scaleValues(synth, l_div, -ACELP_OUTSCALE);
    acelp_mem->deemph_mem_wsyn = acelp_mem->de_emph_mem;
    return;
}

void CLpd_AcelpReset(CAcelpStaticMem_t *acelp) {
    acelp->gc_threshold = (int32_t)0;

    acelp->past_gpit = (int16_t)0;
    acelp->past_gcode = (int32_t)0;
    acelp->old_T0 = 64;
    acelp->old_T0_frac = 0;
    acelp->deemph_mem_wsyn = (int32_t)0;
    acelp->wsyn_rms = (int32_t)0;
    acelp->seed_ace = 0;
}

/* TCX time domain concealment */
/*   Compare to figure 13a on page 54 in 3GPP TS 26.290 */
void CLpd_TcxTDConceal(CAcelpStaticMem_t *acelp_mem, int16_t *pitch, const int16_t lsp_old[M_LP_FILTER_ORDER],
                       const int16_t lsp_new[M_LP_FILTER_ORDER], const int16_t stab_fac, int32_t nLostSf, int32_t synth[],
                       int32_t coreCoderFrameLength, uint8_t last_tcx_noise_factor) {
    /* repeat past excitation with pitch from previous decoded TCX frame */
    int32_t  exc_buf[PIT_MAX_MAX + L_INTERPOL + L_DIV_1024]; /* 411 +  17 + 256 + 1 =  */
    int32_t  syn_buf[M_LP_FILTER_ORDER + L_DIV_1024];        /* 256 +  16           =  */
    int32_t  ns_buf[L_DIV + 1];
    int32_t *syn = syn_buf + M_LP_FILTER_ORDER;
    int32_t *exc = exc_buf + PIT_MAX_MAX + L_INTERPOL;
    int32_t *ns = ns_buf + 1;
    int32_t  tmp, fact_exc;
    int32_t  T = fMin(*pitch, (int16_t)PIT_MAX_MAX);
    int32_t  i, i_subfr, subfr_nr;
    int32_t  lDiv = coreCoderFrameLength / NB_DIV;

    memcpy(syn_buf, acelp_mem->old_syn_mem, M_LP_FILTER_ORDER * sizeof(int32_t));
    memcpy(exc_buf, acelp_mem->old_exc_mem, (PIT_MAX_MAX + L_INTERPOL) * sizeof(int32_t));

    /* if we lost all packets (i.e. 1 packet of TCX-20 ms, 2 packets of
       the TCX-40 ms or 4 packets of the TCX-80ms), we lost the whole
       coded frame extrapolation strategy: repeat lost excitation and
       use extrapolated LSFs */

    /* AMR-WB+ like TCX TD concealment */

    /* number of lost frame cmpt */
    if(nLostSf < 2) { fact_exc = FL2FXCONST_DBL(0.8f); }
    else { fact_exc = FL2FXCONST_DBL(0.4f); }

    /* repeat past excitation */
    for(i = 0; i < lDiv; i++) { exc[i] = fMult(fact_exc, exc[i - T]); }

    tmp = fMult(fact_exc, acelp_mem->wsyn_rms);
    acelp_mem->wsyn_rms = tmp;

    /* init deemph_mem_wsyn */
    acelp_mem->deemph_mem_wsyn = exc[-1];

    ns[-1] = acelp_mem->deemph_mem_wsyn;

    for(i_subfr = 0, subfr_nr = 0; i_subfr < lDiv; i_subfr += L_SUBFR, subfr_nr++) {
        int32_t tRes[L_SUBFR];
        int16_t A[M_LP_FILTER_ORDER];
        int32_t A_exp;

        /* interpolate LPC coefficients */
        int_lpc_acelp(lsp_old, lsp_new, subfr_nr, lDiv / L_SUBFR, A, &A_exp);

        Syn_filt(A,              /* (i) : a[m] prediction coefficients         */
                 A_exp, L_SUBFR, /* (i) : length                               */
                 &exc[i_subfr],  /* (i) : input signal                         */
                 &syn[i_subfr]   /* (i/o) : filter states / output signal      */
        );

        E_LPC_a_weight(A, A, M_LP_FILTER_ORDER); /* overwrite A as it is not needed any longer */

        E_UTIL_residu(A, A_exp, &syn[i_subfr], tRes, L_SUBFR);

        Deemph(tRes, &ns[i_subfr], L_SUBFR, &acelp_mem->deemph_mem_wsyn);

        /* Amplitude limiter (saturate at wsyn_rms) */
        for(i = i_subfr; i < i_subfr + L_SUBFR; i++) {
            if(ns[i] > tmp) { ns[i] = tmp; }
            else {
                if(ns[i] < -tmp) { ns[i] = -tmp; }
            }
        }

        E_UTIL_preemph(&ns[i_subfr], tRes, L_SUBFR);

        Syn_filt(A,              /* (i) : a[m] prediction coefficients         */
                 A_exp, L_SUBFR, /* (i) : length                               */
                 tRes,           /* (i) : input signal                         */
                 &syn[i_subfr]   /* (i/o) : filter states / output signal      */
        );
        memmove(&synth[i_subfr], &syn[i_subfr], L_SUBFR * sizeof(int32_t));
    }

    /* save old excitation and old synthesis memory for next ACELP frame */
    memcpy(acelp_mem->old_exc_mem, exc + lDiv - (PIT_MAX_MAX + L_INTERPOL), sizeof(int32_t) * (PIT_MAX_MAX + L_INTERPOL));
    memcpy(acelp_mem->old_syn_mem, syn_buf + lDiv, sizeof(int32_t) * M_LP_FILTER_ORDER);
    acelp_mem->de_emph_mem = acelp_mem->deemph_mem_wsyn;
}

void Acelp_PreProcessing(int32_t *synth_buf, int32_t *old_synth, int32_t *pitch, int32_t *old_T_pf, int32_t *pit_gain, int32_t *old_gain_pf,
                         int32_t samplingRate, int32_t *i_offset, int32_t coreCoderFrameLength, int32_t synSfd, int32_t nbSubfrSuperfr) {
    int32_t n;

    /* init beginning of synth_buf with old synthesis from previous frame */
    memcpy(synth_buf, old_synth, sizeof(int32_t) * (PIT_MAX_MAX - BPF_DELAY));

    /* calculate pitch lag offset for ACELP decoder */
    *i_offset = (samplingRate * PIT_MIN_12k8 + (FSCALE_DENOM / 2)) / FSCALE_DENOM - PIT_MIN_12k8;

    /* for bass postfilter */
    for(n = 0; n < synSfd; n++) {
        pitch[n] = old_T_pf[n];
        pit_gain[n] = old_gain_pf[n];
    }
    for(n = 0; n < nbSubfrSuperfr; n++) {
        pitch[n + synSfd] = L_SUBFR;
        pit_gain[n + synSfd] = (int32_t)0;
    }
}

void Acelp_PostProcessing(int32_t *synth_buf, int32_t *old_synth, int32_t *pitch, int32_t *old_T_pf, int32_t coreCoderFrameLength, int32_t synSfd,
                          int32_t nbSubfrSuperfr) {
    int32_t n;

    /* store last part of synth_buf (which is not handled by the IMDCT overlap)
     * for next frame */
    memcpy(old_synth, synth_buf + coreCoderFrameLength, sizeof(int32_t) * (PIT_MAX_MAX - BPF_DELAY));

    /* for bass postfilter */
    for(n = 0; n < synSfd; n++) { old_T_pf[n] = pitch[nbSubfrSuperfr + n]; }
}

#define L_FAC_ZIR (LFAC)

void CLpd_Acelp_Zir(const int16_t A[], const int32_t A_exp, CAcelpStaticMem_t *acelp_mem, const int32_t length, int32_t zir[], int32_t doDeemph) {
    int32_t tmp_buf[LFAC + M_LP_FILTER_ORDER];
    assert(length <= L_FAC_ZIR);

    memcpy(tmp_buf, acelp_mem->old_syn_mem, M_LP_FILTER_ORDER * sizeof(int32_t));
    memset(tmp_buf + M_LP_FILTER_ORDER, 0, L_FAC_ZIR * sizeof(int32_t));

    Syn_filt(A, A_exp, length, &tmp_buf[M_LP_FILTER_ORDER], &tmp_buf[M_LP_FILTER_ORDER]);
    if(!doDeemph) {
        /* if last lpd mode was TD concealment, then bypass deemph */
        memcpy(zir, tmp_buf, length * sizeof(*zir));
    }
    else {
        Deemph(&tmp_buf[M_LP_FILTER_ORDER], &zir[0], length, &acelp_mem->de_emph_mem);
        scaleValues(zir, length, -ACELP_OUTSCALE);
    }
}

void CLpd_AcelpPrepareInternalMem(const int32_t *synth, uint8_t last_lpd_mode, uint8_t last_last_lpd_mode, const int16_t *A_new,
                                  const int32_t A_new_exp, const int16_t *A_old, const int32_t A_old_exp, CAcelpStaticMem_t *acelp_mem,
                                  int32_t coreCoderFrameLength, int32_t clearOldExc, uint8_t lpd_mode) {
    int32_t  l_div = coreCoderFrameLength / NB_DIV; /* length of one ACELP/TCX20 frame */
    int32_t  l_div_partial;
    int32_t *syn, *old_exc_mem;

    int32_t synth_buf[PIT_MAX_MAX + L_INTERPOL + M_LP_FILTER_ORDER];
    syn = &synth_buf[M_LP_FILTER_ORDER];

    l_div_partial = PIT_MAX_MAX + L_INTERPOL - l_div;
    old_exc_mem = acelp_mem->old_exc_mem;

    if(lpd_mode == 4) {
        /* Bypass Domain conversion. TCXTD Concealment does no deemphasis in the
         * end. */
        memcpy(synth_buf, &synth[-(PIT_MAX_MAX + L_INTERPOL + M_LP_FILTER_ORDER)], (PIT_MAX_MAX + L_INTERPOL + M_LP_FILTER_ORDER) * sizeof(int32_t));
        /* Set deemphasis memory state for TD concealment */
        acelp_mem->deemph_mem_wsyn = scaleValueSaturate(synth[-1], ACELP_OUTSCALE);
    }
    else {
        /* convert past [PIT_MAX_MAX+L_INTERPOL+M_LP_FILTER_ORDER] synthesis to
         * preemph domain */
        E_UTIL_preemph(&synth[-(PIT_MAX_MAX + L_INTERPOL + M_LP_FILTER_ORDER)], synth_buf, PIT_MAX_MAX + L_INTERPOL + M_LP_FILTER_ORDER);
        scaleValuesSaturate(synth_buf, PIT_MAX_MAX + L_INTERPOL + M_LP_FILTER_ORDER, ACELP_OUTSCALE);
    }

    /* Set deemphasis memory state */
    acelp_mem->de_emph_mem = scaleValueSaturate(synth[-1], ACELP_OUTSCALE);

    /* update acelp synth filter memory */
    memcpy(acelp_mem->old_syn_mem, &syn[PIT_MAX_MAX + L_INTERPOL - M_LP_FILTER_ORDER], M_LP_FILTER_ORDER * sizeof(int32_t));

    if(clearOldExc) {
        memset(old_exc_mem, 0, (PIT_MAX_MAX + L_INTERPOL) * sizeof(int32_t));
        return;
    }

    /* update past [PIT_MAX_MAX+L_INTERPOL] samples of exc memory */
    if(last_lpd_mode == 1) {          /* last frame was TCX20 */
        if(last_last_lpd_mode == 0) { /* ACELP -> TCX20 -> ACELP transition */
            /* Delay valid part of excitation buffer (from previous ACELP frame) by
             * l_div samples */
            memmove(old_exc_mem, old_exc_mem + l_div, sizeof(int32_t) * l_div_partial);
        }
        else if(last_last_lpd_mode > 0) { /* TCX -> TCX20 -> ACELP transition */
            E_UTIL_residu(A_old, A_old_exp, syn, old_exc_mem, l_div_partial);
        }
        E_UTIL_residu(A_new, A_new_exp, syn + l_div_partial, old_exc_mem + l_div_partial, l_div);
    }
    else { /* prev frame was FD, TCX40 or TCX80 */
        int32_t exc_A_new_length = (coreCoderFrameLength / 2 > PIT_MAX_MAX + L_INTERPOL) ? PIT_MAX_MAX + L_INTERPOL : coreCoderFrameLength / 2;
        int32_t exc_A_old_length = PIT_MAX_MAX + L_INTERPOL - exc_A_new_length;
        E_UTIL_residu(A_old, A_old_exp, syn, old_exc_mem, exc_A_old_length);
        E_UTIL_residu(A_new, A_new_exp, &syn[exc_A_old_length], &old_exc_mem[exc_A_old_length], exc_A_new_length);
    }
    return;
}

int32_t *CLpd_ACELP_GetFreeExcMem(CAcelpStaticMem_t *acelp_mem, int32_t length) {
    assert(length <= PIT_MAX_MAX + L_INTERPOL);
    return acelp_mem->old_exc_mem;
}

int32_t CLpd_AcelpRead(HANDLE_FDK_BITSTREAM hBs, CAcelpChannelData_t *acelp, int32_t acelp_core_mode, int32_t coreCoderFrameLength,
                       int32_t i_offset) {
    int32_t        nb_subfr = coreCoderFrameLength / L_DIV;
    const uint8_t *num_acb_index_bits = (nb_subfr == 4) ? num_acb_idx_bits_table[0] : num_acb_idx_bits_table[1];
    int32_t        nbits;
    int32_t        error = 0;

    const int32_t PIT_MIN = PIT_MIN_12k8 + i_offset;
    const int32_t PIT_FR2 = PIT_FR2_12k8 - i_offset;
    const int32_t PIT_FR1 = PIT_FR1_12k8;
    const int32_t PIT_MAX = PIT_MAX_12k8 + (6 * i_offset);
    int32_t       T0, T0_frac, T0_min = 0, T0_max;

    if(PIT_MAX > PIT_MAX_MAX) {
        error = AAC_DEC_DECODE_FRAME_ERROR;
        goto bail;
    }

    acelp->acelp_core_mode = acelp_core_mode;

    nbits = MapCoreMode2NBits(acelp_core_mode);

    /* decode mean energy with 2 bits : 18, 30, 42 or 54 dB */
    acelp->mean_energy = FDKreadBits(hBs, 2);

    for(int32_t sfr = 0; sfr < nb_subfr; sfr++) {
        /* read ACB index and store T0 and T0_frac for each ACELP subframe. */
        error = DecodePitchLag(hBs, num_acb_index_bits[sfr], PIT_MIN, PIT_FR2, PIT_FR1, PIT_MAX, &T0, &T0_frac, &T0_min, &T0_max);
        if(error) { goto bail; }
        acelp->T0[sfr] = (uint16_t)T0;
        acelp->T0_frac[sfr] = (uint8_t)T0_frac;
        acelp->ltp_filtering_flag[sfr] = FDKreadBits(hBs, 1);
        switch(nbits) {
            case 12: /* 12 bits AMR-WB codebook is used */
                acelp->icb_index[sfr][0] = FDKreadBits(hBs, 1);
                acelp->icb_index[sfr][1] = FDKreadBits(hBs, 5);
                acelp->icb_index[sfr][2] = FDKreadBits(hBs, 1);
                acelp->icb_index[sfr][3] = FDKreadBits(hBs, 5);
                break;
            case 16: /* 16 bits AMR-WB codebook is used */
                acelp->icb_index[sfr][0] = FDKreadBits(hBs, 1);
                acelp->icb_index[sfr][1] = FDKreadBits(hBs, 5);
                acelp->icb_index[sfr][2] = FDKreadBits(hBs, 5);
                acelp->icb_index[sfr][3] = FDKreadBits(hBs, 5);
                break;
            case 20: /* 20 bits AMR-WB codebook is used */
                acelp->icb_index[sfr][0] = FDKreadBits(hBs, 5);
                acelp->icb_index[sfr][1] = FDKreadBits(hBs, 5);
                acelp->icb_index[sfr][2] = FDKreadBits(hBs, 5);
                acelp->icb_index[sfr][3] = FDKreadBits(hBs, 5);
                break;
            case 28: /* 28 bits AMR-WB codebook is used */
                acelp->icb_index[sfr][0] = FDKreadBits(hBs, 9);
                acelp->icb_index[sfr][1] = FDKreadBits(hBs, 9);
                acelp->icb_index[sfr][2] = FDKreadBits(hBs, 5);
                acelp->icb_index[sfr][3] = FDKreadBits(hBs, 5);
                break;
            case 36: /* 36 bits AMR-WB codebook is used */
                acelp->icb_index[sfr][0] = FDKreadBits(hBs, 9);
                acelp->icb_index[sfr][1] = FDKreadBits(hBs, 9);
                acelp->icb_index[sfr][2] = FDKreadBits(hBs, 9);
                acelp->icb_index[sfr][3] = FDKreadBits(hBs, 9);
                break;
            case 44: /* 44 bits AMR-WB codebook is used */
                acelp->icb_index[sfr][0] = FDKreadBits(hBs, 13);
                acelp->icb_index[sfr][1] = FDKreadBits(hBs, 13);
                acelp->icb_index[sfr][2] = FDKreadBits(hBs, 9);
                acelp->icb_index[sfr][3] = FDKreadBits(hBs, 9);
                break;
            case 52: /* 52 bits AMR-WB codebook is used */
                acelp->icb_index[sfr][0] = FDKreadBits(hBs, 13);
                acelp->icb_index[sfr][1] = FDKreadBits(hBs, 13);
                acelp->icb_index[sfr][2] = FDKreadBits(hBs, 13);
                acelp->icb_index[sfr][3] = FDKreadBits(hBs, 13);
                break;
            case 64: /* 64 bits AMR-WB codebook is used */
                acelp->icb_index[sfr][0] = FDKreadBits(hBs, 2);
                acelp->icb_index[sfr][1] = FDKreadBits(hBs, 2);
                acelp->icb_index[sfr][2] = FDKreadBits(hBs, 2);
                acelp->icb_index[sfr][3] = FDKreadBits(hBs, 2);
                acelp->icb_index[sfr][4] = FDKreadBits(hBs, 14);
                acelp->icb_index[sfr][5] = FDKreadBits(hBs, 14);
                acelp->icb_index[sfr][6] = FDKreadBits(hBs, 14);
                acelp->icb_index[sfr][7] = FDKreadBits(hBs, 14);
                break;
            default:
                assert(0);
                break;
        }
        acelp->gains[sfr] = FDKreadBits(hBs, 7);
    }

bail:
    return error;
}
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
//----------------------------------------------------------------------------------------------------------------------------------------------------
static void D_ACELP_add_pulse(int16_t pos[], int16_t nb_pulse, int16_t track, FIXP_COD code[]) {
    int16_t i, k;
    for(k = 0; k < nb_pulse; k++) {
        /* i = ((pos[k] & (16-1))*NB_TRACK) + track; */
        i = ((pos[k] & (16 - 1)) << 2) + track;
        if((pos[k] & 16) == 0) { code[i] = code[i] + (FIXP_COD)(512 << (COD_BITS - FRACT_BITS)); }
        else { code[i] = code[i] - (FIXP_COD)(512 << (COD_BITS - FRACT_BITS)); }
    }
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
static void D_ACELP_decode_1p_N1(int32_t index, int16_t N, int16_t offset, int16_t pos[]) {
    int16_t pos1;
    int32_t i, mask;

    mask = ((1 << N) - 1);
    /*
     * Decode 1 pulse with N+1 bits
     */
    pos1 = (int16_t)((index & mask) + offset);
    i = ((index >> N) & 1);
    if(i == 1) { pos1 += 16; }
    pos[0] = pos1;
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
static void D_ACELP_decode_2p_2N1(int32_t index, int16_t N, int16_t offset, int16_t pos[]) {
    int16_t pos1, pos2;
    int32_t mask, i;
    mask = ((1 << N) - 1);
    /* Decode 2 pulses with 2*N+1 bits */
    pos1 = (int16_t)(((index >> N) & mask) + offset);
    i = (index >> (2 * N)) & 1;
    pos2 = (int16_t)((index & mask) + offset);
    if((pos2 - pos1) < 0) {
        if(i == 1) { pos1 += 16; }
        else { pos2 += 16; }
    }
    else {
        if(i == 1) {
            pos1 += 16;
            pos2 += 16;
        }
    }
    pos[0] = pos1;
    pos[1] = pos2;
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
static void D_ACELP_decode_3p_3N1(int32_t index, int16_t N, int16_t offset, int16_t pos[]) {
    int16_t j;
    int32_t mask, idx;

    /* Decode 3 pulses with 3*N+1 bits */
    mask = ((1 << ((2 * N) - 1)) - 1);
    idx = index & mask;
    j = offset;
    if(((index >> ((2 * N) - 1)) & 1) == 1) { j += (1 << (N - 1)); }
    D_ACELP_decode_2p_2N1(idx, N - 1, j, pos);
    mask = ((1 << (N + 1)) - 1);
    idx = (index >> (2 * N)) & mask;
    D_ACELP_decode_1p_N1(idx, N, offset, pos + 2);
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
static void D_ACELP_decode_4p_4N1(int32_t index, int16_t N, int16_t offset, int16_t pos[]) {
    int16_t j;
    int32_t mask, idx;
    /*
     * Decode 4 pulses with 4*N+1 bits
     */
    mask = ((1 << ((2 * N) - 1)) - 1);
    idx = index & mask;
    j = offset;
    if(((index >> ((2 * N) - 1)) & 1) == 1) { j += (1 << (N - 1)); }
    D_ACELP_decode_2p_2N1(idx, N - 1, j, pos);
    mask = ((1 << ((2 * N) + 1)) - 1);
    idx = (index >> (2 * N)) & mask;
    D_ACELP_decode_2p_2N1(idx, N, offset, pos + 2);
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
static void D_ACELP_decode_4p_4N(int32_t index, int16_t N, int16_t offset, int16_t pos[]) {
    int16_t j, n_1;
    /*
     * Decode 4 pulses with 4*N bits
     */
    n_1 = N - 1;
    j = offset + (1 << n_1);
    switch((index >> ((4 * N) - 2)) & 3) {
        case 0:
            if(((index >> ((4 * n_1) + 1)) & 1) == 0) { D_ACELP_decode_4p_4N1(index, n_1, offset, pos); }
            else { D_ACELP_decode_4p_4N1(index, n_1, j, pos); }
            break;
        case 1:
            D_ACELP_decode_1p_N1((index >> ((3 * n_1) + 1)), n_1, offset, pos);
            D_ACELP_decode_3p_3N1(index, n_1, j, pos + 1);
            break;
        case 2:
            D_ACELP_decode_2p_2N1((index >> ((2 * n_1) + 1)), n_1, offset, pos);
            D_ACELP_decode_2p_2N1(index, n_1, j, pos + 2);
            break;
        case 3:
            D_ACELP_decode_3p_3N1((index >> (n_1 + 1)), n_1, offset, pos);
            D_ACELP_decode_1p_N1(index, n_1, j, pos + 3);
            break;
    }
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void D_ACELP_decode_4t64(int16_t index[], int32_t nbits, FIXP_COD code[]) {
    int32_t L_index;
    int16_t k, pos[6];

    memset(code, 0, L_SUBFR * sizeof(FIXP_COD));

    /* decode the positions and signs of pulses and build the codeword */
    switch(nbits) {
        case 12:
            for(k = 0; k < 4; k += 2) {
                L_index = index[2 * (k / 2) + 1];
                D_ACELP_decode_1p_N1(L_index, 4, 0, pos);
                D_ACELP_add_pulse(pos, 1, 2 * (index[2 * (k / 2)]) + k / 2, code);
            }
            break;
        case 16: {
            int32_t i = 0;
            int32_t offset = index[i++];
            offset = (offset == 0) ? 1 : 3;
            for(k = 0; k < 4; k++) {
                if(k != offset) {
                    L_index = index[i++];
                    D_ACELP_decode_1p_N1(L_index, 4, 0, pos);
                    D_ACELP_add_pulse(pos, 1, k, code);
                }
            }
        } break;
        case 20:
            for(k = 0; k < 4; k++) {
                L_index = (int32_t)index[k];
                D_ACELP_decode_1p_N1(L_index, 4, 0, pos);
                D_ACELP_add_pulse(pos, 1, k, code);
            }
            break;
        case 28:
            for(k = 0; k < 4 - 2; k++) {
                L_index = (int32_t)index[k];
                D_ACELP_decode_2p_2N1(L_index, 4, 0, pos);
                D_ACELP_add_pulse(pos, 2, k, code);
            }
            for(k = 2; k < 4; k++) {
                L_index = (int32_t)index[k];
                D_ACELP_decode_1p_N1(L_index, 4, 0, pos);
                D_ACELP_add_pulse(pos, 1, k, code);
            }
            break;
        case 36:
            for(k = 0; k < 4; k++) {
                L_index = (int32_t)index[k];
                D_ACELP_decode_2p_2N1(L_index, 4, 0, pos);
                D_ACELP_add_pulse(pos, 2, k, code);
            }
            break;
        case 44:
            for(k = 0; k < 4 - 2; k++) {
                L_index = (int32_t)index[k];
                D_ACELP_decode_3p_3N1(L_index, 4, 0, pos);
                D_ACELP_add_pulse(pos, 3, k, code);
            }
            for(k = 2; k < 4; k++) {
                L_index = (int32_t)index[k];
                D_ACELP_decode_2p_2N1(L_index, 4, 0, pos);
                D_ACELP_add_pulse(pos, 2, k, code);
            }
            break;
        case 52:
            for(k = 0; k < 4; k++) {
                L_index = (int32_t)index[k];
                D_ACELP_decode_3p_3N1(L_index, 4, 0, pos);
                D_ACELP_add_pulse(pos, 3, k, code);
            }
            break;
        case 64:
            for(k = 0; k < 4; k++) {
                L_index = (((int32_t)index[k] << 14) + (int32_t)index[k + 4]);
                D_ACELP_decode_4p_4N(L_index, 4, 0, pos);
                D_ACELP_add_pulse(pos, 4, k, code);
            }
            break;
        default:
            assert(0);
    }
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------