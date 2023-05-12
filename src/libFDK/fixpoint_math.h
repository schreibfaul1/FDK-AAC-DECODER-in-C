

/******************* Library for basic calculation routines ********************

   Author(s):   M. Gayer

   Description: Fixed point specific mathematical functions

*******************************************************************************/

#ifndef FIXPOINT_MATH_H
#define FIXPOINT_MATH_H
#include <assert.h>
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"
#include "scale.h"

#define LD_DATA_SCALING      (64.0f)
#define LD_DATA_SHIFT        6 /* pow(2, LD_DATA_SHIFT) = LD_DATA_SCALING */
#define MAX_LD_PRECISION     10
#define LD_PRECISION         10
#define SQRT_BITS            7
#define SQRT_VALUES          (128 + 2)
#define SQRT_BITS_MASK       0x7f
#define SQRT_FRACT_BITS_MASK 0x007FFFFF

static const int16_t ldCoeff[MAX_LD_PRECISION] = {-32768, -16384, -10923, -8192, -6554, -5461, -4681, -4096, -3641, -3277};

extern const int32_t invSqrtTab[SQRT_VALUES];

static inline int32_t fIsLessThan(int32_t a_m, int32_t a_e, int32_t b_m, int32_t b_e) {
    int32_t n;
    n = fixnorm_D(a_m);
    a_m <<= n;
    a_e -= n;
    n = fixnorm_D(b_m);
    b_m <<= n;
    b_e -= n;

    if(a_m == (int32_t)0) a_e = b_e;
    if(b_m == (int32_t)0) b_e = a_e;
    if(a_e > b_e) { return ((b_m >> fMin(a_e - b_e, DFRACT_BITS - 1)) > a_m); }
    else { return ((a_m >> fMin(b_e - a_e, DFRACT_BITS - 1)) < b_m); }
}

static inline int32_t fIsLessThan(int16_t a_m, int32_t a_e, int16_t b_m, int32_t b_e) {
    int32_t n;
    n = fixnorm_S(a_m);
    a_m <<= n;
    a_e -= n;
    n = fixnorm_S(b_m);
    b_m <<= n;
    b_e -= n;

    if(a_m == (int16_t)0) a_e = b_e;
    if(b_m == (int16_t)0) b_e = a_e;

    if(a_e > b_e) { return ((b_m >> fMin(a_e - b_e, FRACT_BITS - 1)) > a_m); }
    else { return ((a_m >> fMin(b_e - a_e, FRACT_BITS - 1)) < b_m); }
}

#define CalcLdData(op) fLog2(op, 0)

void LdDataVector(int32_t *srcVector, int32_t *destVector, int32_t number);

extern const uint32_t exp2_tab_long[32];
extern const uint32_t exp2w_tab_long[32];
extern const uint32_t exp2x_tab_long[32];

static inline int32_t CalcInvLdData(const int32_t x) {
    int32_t set_zero = (x < FL2FXCONST_DBL(-31.0 / 64.0)) ? 0 : 1;
    int32_t set_max = (x >= FL2FXCONST_DBL(31.0 / 64.0)) | (x == FL2FXCONST_DBL(0.0));

    int16_t  frac = (int16_t)((int32_t)x & 0x3FF);
    uint32_t index3 = (uint32_t)(int32_t)(x >> 10) & 0x1F;
    uint32_t index2 = (uint32_t)(int32_t)(x >> 15) & 0x1F;
    uint32_t index1 = (uint32_t)(int32_t)(x >> 20) & 0x1F;
    int32_t  exp = fMin(31, ((x > FL2FXCONST_DBL(0.0f)) ? (31 - (int32_t)(x >> 25)) : (int32_t)(-(x >> 25))));

    uint32_t lookup1 = exp2_tab_long[index1] * set_zero;
    uint32_t lookup2 = exp2w_tab_long[index2];
    uint32_t lookup3 = exp2x_tab_long[index3];
    uint32_t lookup3f = lookup3 + (uint32_t)(int32_t)fMultDiv2((int32_t)(0x0016302F), (int16_t)frac);

    uint32_t lookup12 = (uint32_t)(int32_t)fMult((int32_t)lookup1, (int32_t)lookup2);
    uint32_t lookup = (uint32_t)(int32_t)fMult((int32_t)lookup12, (int32_t)lookup3f);

    int32_t retVal = (lookup << 3) >> exp;

    if(set_max) { retVal = (int32_t)MAXVAL_DBL; }

    return retVal;
}

void    InitLdInt();
int32_t CalcLdInt(int32_t i);

extern const uint16_t sqrt_tab[49];

inline int32_t sqrtFixp_lookup(int32_t x) {
    uint32_t y = (int32_t)x;
    uint8_t  is_zero = (y == 0);
    int32_t  zeros = fixnormz_D(y) & 0x1e;
    y <<= zeros;
    uint32_t idx = (y >> 26) - 16;
    uint16_t frac = (y >> 10) & 0xffff;
    uint16_t nfrac = 0xffff ^ frac;
    uint32_t t = (uint32_t)nfrac * sqrt_tab[idx] + (uint32_t)frac * sqrt_tab[idx + 1];
    t = t >> (zeros >> 1);
    return (is_zero ? 0 : t);
}

inline int32_t sqrtFixp_lookup(int32_t x, int32_t *x_e) {
    uint32_t y = (int32_t)x;
    int32_t  e;

    if(x == (int32_t)0) { return x; }

    /* Normalize */
    e = fixnormz_D(y);
    y <<= e;
    e = *x_e - e + 2;

    /* Correct odd exponent. */
    if(e & 1) {
        y >>= 1;
        e++;
    }
    /* Get square root */
    uint32_t idx = (y >> 26) - 16;
    uint16_t frac = (y >> 10) & 0xffff;
    uint16_t nfrac = 0xffff ^ frac;
    uint32_t t = (uint32_t)nfrac * sqrt_tab[idx] + (uint32_t)frac * sqrt_tab[idx + 1];

    /* Write back exponent */
    *x_e = e >> 1;
    return (int32_t)(int32_t)(t >> 1);
}


static inline int32_t invSqrtNorm2(int32_t op, int32_t *shift) {
    int32_t val = op;
    int32_t reg1, reg2;

    if(val == FL2FXCONST_DBL(0.0)) {
        *shift = 16;
        return ((int32_t)MAXVAL_DBL); /* maximum positive value */
    }

    #define INVSQRTNORM2_LINEAR_INTERPOLATE
    #define INVSQRTNORM2_LINEAR_INTERPOLATE_HQ

    /* normalize input, calculate shift value */
    assert(val > FL2FXCONST_DBL(0.0));
    *shift = fNormz(val) - 1; /* CountLeadingBits() is not necessary here since
                                 test value is always > 0 */
    val <<= *shift;           /* normalized input V */
    *shift += 2;              /* bias for exponent */

    int32_t index = (int32_t)(val >> (DFRACT_BITS - 1 - (SQRT_BITS + 1))) & SQRT_BITS_MASK;
    int32_t Fract = (int32_t)(((int32_t)val & SQRT_FRACT_BITS_MASK) << (SQRT_BITS + 1));
    int32_t diff = invSqrtTab[index + 1] - invSqrtTab[index];
    reg1 = invSqrtTab[index] + (fMultDiv2(diff, Fract) << 1);

    /* reg1 = t[i] + (t[i+1]-t[i])*fract ... already computed ...
                                         + (1-fract)fract*(t[i+2]-t[i+1])/2 */
    if(Fract != (int32_t)0) {
        /* fract = fract * (1 - fract) */
        Fract = fMultDiv2(Fract, (int32_t)((uint32_t)0x80000000 - (uint32_t)Fract)) << 1;
        diff = diff - (invSqrtTab[index + 2] - invSqrtTab[index + 1]);
        reg1 = fMultAddDiv2(reg1, Fract, diff);
    }

    /* calculate the output exponent = input exp/2 */
    if(*shift & 0x00000001) { /* odd shift values ? */
        /* Note: Do not use rounded value 0x5A82799A to avoid overflow with
         * shift-by-2 */
        reg2 = (int32_t)0x5A827999;
        /* FL2FXCONST_DBL(0.707106781186547524400844362104849f);*/ /* 1/sqrt(2);
                                                                    */
        reg1 = fMultDiv2(reg1, reg2) << 2;
    }

    *shift = *shift >> 1;

    return (reg1);
}

static inline int32_t sqrtFixp(int32_t op) {
    int32_t tmp_exp = 0;
    int32_t tmp_inv = invSqrtNorm2(op, &tmp_exp);

    assert(tmp_exp > 0);
    return ((int32_t)(fMultDiv2((op << (tmp_exp - 1)), tmp_inv) << 2));
}

static inline int32_t invFixp(int32_t op) {
    if((op == (int32_t)0x00000000) || (op == (int32_t)0x00000001)) { return ((int32_t)MAXVAL_DBL); }
    int32_t tmp_exp;
    int32_t tmp_inv = invSqrtNorm2(op, &tmp_exp);
    assert((31 - (2 * tmp_exp + 1)) >= 0);
    int32_t shift = 31 - (2 * tmp_exp + 1);
    tmp_inv = fPow2Div2(tmp_inv);
    if(shift) { tmp_inv = ((tmp_inv >> (shift - 1)) + (int32_t)1) >> 1; }
    return tmp_inv;
}

static inline int32_t invFixp(int32_t op_m, int32_t *op_e) {
    if((op_m == (int32_t)0x00000000) || (op_m == (int32_t)0x00000001)) {
        *op_e = 31 - *op_e;
        return ((int32_t)MAXVAL_DBL);
    }

    int32_t tmp_exp;
    int32_t tmp_inv = invSqrtNorm2(op_m, &tmp_exp);
    *op_e = (tmp_exp << 1) - *op_e + 1;
    return fPow2Div2(tmp_inv);
}

int32_t schur_div(int32_t num, int32_t denum, int32_t count);
int32_t mul_dbl_sgl_rnd(const int32_t op1, const int16_t op2);


int32_t fMultNorm(int32_t f1, int32_t f2, int32_t *result_e);


inline int32_t fMultNorm(int32_t f1, int32_t f2) {
    int32_t m;
    int32_t e;
    m = fMultNorm(f1, f2, &e);
    m = scaleValueSaturate(m, e);
    return m;
}

inline int32_t fMultNorm(int32_t f1_m, int32_t f1_e, int32_t f2_m, int32_t f2_e, int32_t result_e) {
    int32_t m;
    int32_t e;
    m = fMultNorm(f1_m, f2_m, &e);
    m = scaleValueSaturate(m, e + f1_e + f2_e - result_e);
    return m;
}


inline int32_t fMultI(int32_t a, int32_t b) {
    int32_t m, mi;
    int32_t m_e;
    m = fMultNorm(a, (int32_t)b, &m_e);
    if(m_e < (int32_t)0) {
        if(m_e > (int32_t)-DFRACT_BITS) {
            m = m >> ((-m_e) - 1);
            mi = (m + (int32_t)1) >> 1;
        }
        else { mi = (int32_t)0; }
    }
    else { mi = scaleValueSaturate(m, m_e); }
    return ((int32_t)mi);
}

inline int32_t fMultIfloor(int32_t a, int32_t b) {
    int32_t m, mi;
    int32_t m_e;
    m = fMultNorm(a, (int32_t)b, &m_e);
    if(m_e < (int32_t)0) {
        if(m_e > (int32_t)-DFRACT_BITS) { mi = m >> (-m_e); }
        else {
            mi = (int32_t)0;
            if(m < (int32_t)0) { mi = (int32_t)-1; }
        }
    }
    else { mi = scaleValueSaturate(m, m_e); }
    return ((int32_t)mi);
}

inline int32_t fMultIceil(int32_t a, int32_t b) {
    int32_t m, mi;
    int32_t m_e;
    m = fMultNorm(a, (int32_t)b, &m_e);
    if(m_e < (int32_t)0) {
        if(m_e > (int32_t) - (DFRACT_BITS - 1)) {
            mi = (m >> (-m_e));
            if((int32_t)m & ((1 << (-m_e)) - 1)) { mi = mi + (int32_t)1; }
        }
        else {
            if(m > (int32_t)0) { mi = (int32_t)1; }
            else {
                if((m_e == -(DFRACT_BITS - 1)) && (m == (int32_t)MINVAL_DBL)) { mi = (int32_t)-1; }
                else { mi = (int32_t)0; }
            }
        }
    }
    else { mi = scaleValueSaturate(m, m_e); }
    return ((int32_t)mi);
}

int32_t fDivNorm(int32_t num, int32_t denom, int32_t *result_e);
int32_t fDivNorm(int32_t num, int32_t denom);
int32_t fDivNormSigned(int32_t L_num, int32_t L_denum, int32_t *result_e);
int32_t fDivNormSigned(int32_t num, int32_t denom);

inline int32_t fAdjust(int32_t a_m, int32_t *pA_e) {
    int32_t shift;
    shift = fNorm(a_m) - 1;
    *pA_e -= shift;
    return scaleValue(a_m, shift);
}

inline int32_t fAddNorm(int32_t a_m, int32_t a_e, int32_t b_m, int32_t b_e, int32_t *pResult_e) {
    int32_t result_e;
    int32_t result_m;
    /* If one of the summands is zero, return the other. This is necessary for the summation of a very small number to zero */
    if(a_m == (int32_t)0) {
        *pResult_e = b_e;
        return b_m;
    }
    if(b_m == (int32_t)0) {
        *pResult_e = a_e;
        return a_m;
    }
    a_m = fAdjust(a_m, &a_e);
    b_m = fAdjust(b_m, &b_e);
    if(a_e > b_e) {
        result_m = a_m + (b_m >> fMin(a_e - b_e, DFRACT_BITS - 1));
        result_e = a_e;
    }
    else {
        result_m = (a_m >> fMin(b_e - a_e, DFRACT_BITS - 1)) + b_m;
        result_e = b_e;
    }
    *pResult_e = result_e;
    return result_m;
}

inline int32_t fAddNorm(int32_t a_m, int32_t a_e, int32_t b_m, int32_t b_e, int32_t result_e) {
    int32_t result_m;
    a_m = scaleValue(a_m, a_e - result_e);
    b_m = scaleValue(b_m, b_e - result_e);
    result_m = a_m + b_m;
    return result_m;
}

int32_t fDivNormHighPrec(int32_t L_num, int32_t L_denum, int32_t *result_e);

int32_t f2Pow(const int32_t exp_m, const int32_t exp_e, int32_t *result_e);
int32_t f2Pow(const int32_t exp_m, const int32_t exp_e);
int32_t fLdPow(int32_t baseLd_m, int32_t baseLd_e, int32_t exp_m, int32_t exp_e, int32_t *result_e);
int32_t fLdPow(int32_t baseLd_m, int32_t baseLd_e, int32_t exp_m, int32_t exp_e);
int32_t fPow(int32_t base_m, int32_t base_e, int32_t exp_m, int32_t exp_e, int32_t *result_e);
int32_t fPowInt(int32_t base_m, int32_t base_e, int32_t N, int32_t *result_e);


int32_t CalcLog2(int32_t arg, int32_t arg_e, int32_t *result_e);

static inline int32_t fLog2(int32_t x_m, int32_t x_e, int32_t *result_e) {
    int32_t result_m;
    if(x_m <= FL2FXCONST_DBL(0.0f)) { /* int16_t cut for zero and negative numbers. */
        *result_e = DFRACT_BITS - 1;
        return FL2FXCONST_DBL(-1.0f);
    }
    {  /* Calculate log2() */
        int32_t x2_m;
        /* Move input value x_m * 2^x_e toward 1.0, where the taylor approximation of the function log(1-x) centered at 0 is most accurate. */
        {
            int32_t b_norm;
            b_norm = fNormz(x_m) - 1;
            x2_m = x_m << b_norm;
            x_e = x_e - b_norm;
        }
        /* map x from log(x) domain to log(1-x) domain. */
        x2_m = -(x2_m + FL2FXCONST_DBL(-1.0));
        {   /* Taylor polynomial approximation of ln(1-x) */
            int32_t px2_m;
            result_m = FL2FXCONST_DBL(0.0);
            px2_m = x2_m;
            for(int32_t i = 0; i < LD_PRECISION; i++) {
                result_m = fMultAddDiv2(result_m, ldCoeff[i], px2_m);
                px2_m = fMult(px2_m, x2_m);
            }
        }
        /* Multiply result with 1/ln(2) = 1.0 + 0.442695040888 (get log2(x) from * ln(x) result). */
        result_m = fMultAddDiv2(result_m, result_m, FL2FXCONST_DBL(2.0 * 0.4426950408889634073599246810019));
        /* Add exponent part. log2(x_m * 2^x_e) = log2(x_m) + x_e */
        if(x_e != 0) {
            int32_t enorm;
            enorm = DFRACT_BITS - fNorm((int32_t)x_e);
            /* The -1 in the right shift of result_m compensates the fMultDiv2() above * in the taylor polynomial evaluation loop.*/
            result_m = (result_m >> (enorm - 1)) + ((int32_t)x_e << (DFRACT_BITS - 1 - enorm));
            *result_e = enorm;
        }
        else {
            /* 1 compensates the fMultDiv2() above in the taylor polynomial evaluation loop.*/
            *result_e = 1;
        }
    }
    return result_m;
}

static inline int32_t fLog2(int32_t x_m, int32_t x_e) {
    if(x_m <= FL2FXCONST_DBL(0.0f)) { x_m = FL2FXCONST_DBL(-1.0f); }
    else {
        int32_t result_e;
        x_m = fLog2(x_m, x_e, &result_e);
        x_m = scaleValue(x_m, result_e - LD_DATA_SHIFT);
    }
    return x_m;
}


inline int16_t fAddSaturate(const int16_t a, const int16_t b) {
    int32_t sum;
    sum = (int32_t)(int16_t)a + (int32_t)(int16_t)b;
    sum = fMax(fMin((int32_t)sum, (int32_t)MAXVAL_SGL), (int32_t)MINVAL_SGL);
    return (int16_t)(int16_t)sum;
}

inline int32_t fAddSaturate(const int32_t a, const int32_t b) {
    int32_t sum;
    sum = (int32_t)(a >> 1) + (int32_t)(b >> 1);
    sum = fMax(fMin((int32_t)sum, (int32_t)(MAXVAL_DBL >> 1)), (int32_t)(MINVAL_DBL >> 1));
    return (int32_t)(int32_t)(sum << 1);
}


int32_t fixp_floorToInt(int32_t f_inp, int32_t sf);
int32_t fixp_floor(int32_t f_inp, int32_t sf);
int32_t fixp_ceilToInt(int32_t f_inp, int32_t sf);
int32_t fixp_ceil(int32_t f_inp, int32_t sf);
int32_t fixp_truncateToInt(int32_t f_inp, int32_t sf);
int32_t fixp_truncate(int32_t f_inp, int32_t sf);
int32_t fixp_roundToInt(int32_t f_inp, int32_t sf);
int32_t fixp_round(int32_t f_inp, int32_t sf);

extern const int32_t invCount[80];


inline int32_t GetInvInt(int32_t intValue) { return invCount[fMin(fMax(intValue, 0), 80 - 1)]; }

#endif /* FIXPOINT_MATH_H */
