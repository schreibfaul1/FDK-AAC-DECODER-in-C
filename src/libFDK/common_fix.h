

/******************* Library for basic calculation routines ********************

   Author(s):   M. Lohwasser, M. Gayer

   Description: Flexible fixpoint library configuration

*******************************************************************************/

#ifndef COMMON_FIX_H
#define COMMON_FIX_H

#include <stdint.h>
#include "../libAACdec/newAACDecoder.h"

/* ***** Start of former fix.h ****** */

/* Scale factor from/to float/fixpoint values. DO NOT USE THESE VALUES AS
 * SATURATION LIMITS !! */

/* Max and Min values for saturation purposes. DO NOT USE THESE VALUES AS SCALE
 * VALUES !! */

/* macros for compile-time conversion of constant float values to fixedpoint */

// #define FL2FXCONST_SGL(val)                                                                                                                      \
//     (int16_t)(                                                                                                                                   \
//         ((val) >= 0)                                                                                                                             \
//             ? ((((double)(val) * (FRACT_FIX_SCALE) + 0.5) >= (double)(MAXVAL_SGL)) ? (int16_t)(MAXVAL_SGL)                                       \
//                                                                                    : (int16_t)((double)(val) * (double)(FRACT_FIX_SCALE) + 0.5)) \
//             : ((((double)(val) * (FRACT_FIX_SCALE)-0.5) <= (double)(MINVAL_SGL_CONST)) ? (int16_t)(MINVAL_SGL_CONST)                             \
//                                                                                        : (int16_t)((double)(val) * (double)(FRACT_FIX_SCALE)-0.5)))


inline int16_t FL2FXCONST_SGL(double val){
    int16_t ret = 0;
    if(val >= 0){
        if((val * FRACT_FIX_SCALE + 0.5) >= MAXVAL_SGL)  ret =  MAXVAL_SGL;
        else ret = val * (double)FRACT_FIX_SCALE + 0.5;
    }
    else{
        if((val * FRACT_FIX_SCALE - 0.5) <= MINVAL_SGL_CONST) ret = MINVAL_SGL_CONST;
        else ret = val * (double)FRACT_FIX_SCALE - 0.5;

    }
    return ret;
}



inline int32_t FX_SGL2FX_DBL(int16_t val){return (val << (DFRACT_BITS - FRACT_BITS));}
inline int16_t FX_DBL2FX_SGL(int32_t val){return (val >> (DFRACT_BITS - FRACT_BITS));}

/* ############################################################# */

/* macros for runtime conversion of integer fixedpoint values to float. */

/* #define FX_DBL2FL(val)  ((float)(pow(2.,-31.)*(float)val)) */ /* version #1
                                                                  */
#define FX_DBL2FL(val)                                                        \
    ((float)((double)(val) / (double)DFRACT_FIX_SCALE)) /* version #2 -       \
                                                           identical to class \
                                                           dfract cast from   \
                                                           dfract to float */
#define FX_DBL2DOUBLE(val) (((double)(val) / (double)DFRACT_FIX_SCALE))

/* ############################################################# */
#include "fixmul.h"

static inline int32_t fMult(int16_t a, int16_t b) { return fixmul_SS(a, b); }
static inline int32_t fMult(int16_t a, int32_t b) { return fixmul_SD(a, b); }
static inline int32_t fMult(int32_t a, int16_t b) { return fixmul_DS(a, b); }
static inline int32_t fMult(int32_t a, int32_t b) { return fixmul_DD(a, b); }
static inline int32_t fPow2(int32_t a) { return fixpow2_D(a); }
static inline int32_t fPow2(int16_t a) { return fixpow2_S(a); }

static inline int32_t fMultDiv2(int16_t a, int16_t b) { return fixmuldiv2_SS(a, b); }
static inline int32_t fMultDiv2(int16_t a, int32_t b) { return fixmuldiv2_SD(a, b); }
static inline int32_t fMultDiv2(int32_t a, int16_t b) { return fixmuldiv2_DS(a, b); }
static inline int32_t fMultDiv2(int32_t a, int32_t b) { return fixmuldiv2_DD(a, b); }
static inline int32_t fPow2Div2(int32_t a) { return fixpow2div2_D(a); }
static inline int32_t fPow2Div2(int16_t a) { return fixpow2div2_S(a); }

static inline int32_t fMultDiv2BitExact(int32_t a, int32_t b) { return fixmuldiv2BitExact_DD(a, b); }
static inline int32_t fMultDiv2BitExact(int16_t a, int32_t b) { return fixmuldiv2BitExact_SD(a, b); }
static inline int32_t fMultDiv2BitExact(int32_t a, int16_t b) { return fixmuldiv2BitExact_DS(a, b); }
static inline int32_t fMultBitExact(int32_t a, int32_t b) { return fixmulBitExact_DD(a, b); }
static inline int32_t fMultBitExact(int16_t a, int32_t b) { return fixmulBitExact_SD(a, b); }
static inline int32_t fMultBitExact(int32_t a, int16_t b) { return fixmulBitExact_DS(a, b); }

/* ********************************************************************************
 */
#include "abs.h"

static inline int32_t fAbs(int32_t x) {
    if(x > (int32_t)0) return x;
    return -x;
}
static inline int16_t fAbs(int16_t x) {
    if(x > (int16_t)0) return x;
    return -x;
}

/* ********************************************************************************
 */

#include "clz.h"

static inline int32_t fNormz(int64_t x) {
    int32_t clz = fixnormz_D((int32_t)(x >> 32));
    if(clz == 32) clz += fixnormz_D((int32_t)x);
    return clz;
}
static inline int32_t fNormz(int32_t x) { return fixnormz_D(x); }
static inline int32_t fNormz(int16_t x) { return fixnormz_S(x); }
static inline int32_t fNorm(int32_t x) { return fixnorm_D(x); }
static inline int32_t fNorm(int16_t x) { return fixnorm_S(x); }

/* ********************************************************************************
 */
/* ********************************************************************************
 */
/* ********************************************************************************
 */

#include "clz.h"
#define fixp_abs(x)         fAbs(x)
#define fixMin(a, b)        fMin(a, b)
#define fixMax(a, b)        fMax(a, b)
#define CntLeadingZeros(x)  fixnormz_D(x)
#define CountLeadingBits(x) fixnorm_D(x)

#include "fixmadd.h"

/* y = (x+0.5*a*b) */
static inline int32_t fMultAddDiv2(int32_t x, int32_t a, int32_t b) { return fixmadddiv2_DD(x, a, b); }
static inline int32_t fMultAddDiv2(int32_t x, int16_t a, int32_t b) { return fixmadddiv2_SD(x, a, b); }
static inline int32_t fMultAddDiv2(int32_t x, int32_t a, int16_t b) { return fixmadddiv2_DS(x, a, b); }
static inline int32_t fMultAddDiv2(int32_t x, int16_t a, int16_t b) { return fixmadddiv2_SS(x, a, b); }

static inline int32_t fPow2AddDiv2(int32_t x, int32_t a) { return fixpadddiv2_D(x, a); }
static inline int32_t fPow2AddDiv2(int32_t x, int16_t a) { return fixpadddiv2_S(x, a); }

/* y = 2*(x+0.5*a*b) = (2x+a*b) */
static inline int32_t fMultAdd(int32_t x, int32_t a, int32_t b) { return fixmadd_DD(x, a, b); }
inline int32_t        fMultAdd(int32_t x, int16_t a, int32_t b) { return fixmadd_SD(x, a, b); }
inline int32_t        fMultAdd(int32_t x, int32_t a, int16_t b) { return fixmadd_DS(x, a, b); }
inline int32_t        fMultAdd(int32_t x, int16_t a, int16_t b) { return fixmadd_SS(x, a, b); }

inline int32_t fPow2Add(int32_t x, int32_t a) { return fixpadd_D(x, a); }
inline int32_t fPow2Add(int32_t x, int16_t a) { return fixpadd_S(x, a); }

/* y = (x-0.5*a*b) */
inline int32_t fMultSubDiv2(int32_t x, int32_t a, int32_t b) { return fixmsubdiv2_DD(x, a, b); }
inline int32_t fMultSubDiv2(int32_t x, int16_t a, int32_t b) { return fixmsubdiv2_SD(x, a, b); }
inline int32_t fMultSubDiv2(int32_t x, int32_t a, int16_t b) { return fixmsubdiv2_DS(x, a, b); }
inline int32_t fMultSubDiv2(int32_t x, int16_t a, int16_t b) { return fixmsubdiv2_SS(x, a, b); }

/* y = 2*(x-0.5*a*b) = (2*x-a*b) */
static inline int32_t fMultSub(int32_t x, int32_t a, int32_t b) { return fixmsub_DD(x, a, b); }
inline int32_t        fMultSub(int32_t x, int16_t a, int32_t b) { return fixmsub_SD(x, a, b); }
inline int32_t        fMultSub(int32_t x, int32_t a, int16_t b) { return fixmsub_DS(x, a, b); }
inline int32_t        fMultSub(int32_t x, int16_t a, int16_t b) { return fixmsub_SS(x, a, b); }

static inline int32_t fMultAddDiv2BitExact(int32_t x, int32_t a, int32_t b) { return fixmadddiv2BitExact_DD(x, a, b); }
static inline int32_t fMultAddDiv2BitExact(int32_t x, int16_t a, int32_t b) { return fixmadddiv2BitExact_SD(x, a, b); }
static inline int32_t fMultAddDiv2BitExact(int32_t x, int32_t a, int16_t b) { return fixmadddiv2BitExact_DS(x, a, b); }
static inline int32_t fMultSubDiv2BitExact(int32_t x, int32_t a, int32_t b) { return fixmsubdiv2BitExact_DD(x, a, b); }
static inline int32_t fMultSubDiv2BitExact(int32_t x, int16_t a, int32_t b) { return fixmsubdiv2BitExact_SD(x, a, b); }
static inline int32_t fMultSubDiv2BitExact(int32_t x, int32_t a, int16_t b) { return fixmsubdiv2BitExact_DS(x, a, b); }

#include "fixminmax.h"

static inline int32_t fMin(int32_t a, int32_t b) { return fixmin_D(a, b); }
static inline int32_t fMax(int32_t a, int32_t b) { return fixmax_D(a, b); }

static inline int16_t fMin(int16_t a, int16_t b) { return fixmin_S(a, b); }
static inline int16_t fMax(int16_t a, int16_t b) { return fixmax_S(a, b); }

#if !defined(__LP64__)
static inline int32_t fMax(int32_t a, int32_t b) { return fixmax_I(a, b); }
static inline int32_t fMin(int32_t a, int32_t b) { return fixmin_I(a, b); }
#endif

inline uint32_t fMax(uint32_t a, uint32_t b) { return fixmax_UI(a, b); }
inline uint32_t fMin(uint32_t a, uint32_t b) { return fixmin_UI(a, b); }

inline uint8_t fMax(uint8_t a, uint8_t b) { return (uint8_t)fixmax_UI((uint32_t)a, (uint32_t)b); }
inline uint8_t fMin(uint8_t a, uint8_t b) { return (uint8_t)fixmin_UI((uint32_t)a, (uint32_t)b); }

/* Complex data types */

#include "cplx_mul.h"
#include "fixmadd.h"
#include "fixmul.h"
#include "fixpoint_math.h"

#endif
