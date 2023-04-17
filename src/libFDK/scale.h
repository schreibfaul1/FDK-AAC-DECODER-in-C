
/******************* Library for basic calculation routines ********************

   Author(s):

   Description: Scaling operations

*******************************************************************************/

#ifndef SCALE_H
#define SCALE_H
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"
#include "fixminmax.h"

#define SCALE_INLINE

// #if defined(__arm__)
// #include "arm/scale_arm.h"

// #elif defined(__mips__)
// #include "mips/scale_mips.h"

// #endif

void scaleValues(int16_t *vector, int32_t len, int32_t scalefactor);
void scaleValues(int32_t *vector, int32_t len, int32_t scalefactor);
void scaleValues(int32_t *dst, const int32_t *src, int32_t len, int32_t scalefactor);
#if(SAMPLE_BITS == 16)
void scaleValues(int16_t *dst, const int32_t *src, int32_t len, int32_t scalefactor);
#endif
void    scaleValues(int16_t *dst, const int16_t *src, int32_t len, int32_t scalefactor);
void    scaleCplxValues(int32_t *r_dst, int32_t *i_dst, const int32_t *r_src, const int32_t *i_src, int32_t len,
                        int32_t scalefactor);
void    scaleValuesWithFactor(int32_t *vector, int32_t factor, int32_t len, int32_t scalefactor);
void    scaleValuesSaturate(int32_t *vector, int32_t len, int32_t scalefactor);
void    scaleValuesSaturate(int32_t *dst, const int32_t *src, int32_t len, int32_t scalefactor);
void    scaleValuesSaturate(int16_t *dst, const int32_t *src, int32_t len, int32_t scalefactor);
void    scaleValuesSaturate(int16_t *vector, int32_t len, int32_t scalefactor);
void    scaleValuesSaturate(int16_t *dst, const int16_t *src, int32_t len, int32_t scalefactor);
int32_t getScalefactorShort(const int16_t *vector, int32_t len);
int32_t getScalefactorPCM(const int16_t *vector, int32_t len, int32_t stride);
int32_t getScalefactor(const int32_t *vector, int32_t len);
int32_t getScalefactor(const int16_t *vector, int32_t len);

#ifndef FUNCTION_scaleValue
    /*!
     *
     *  \brief Multiply input by \f$ 2^{scalefactor} \f$
     *
     *  \return Scaled input
     *
     */
    #define FUNCTION_scaleValue
inline int32_t scaleValue(const int32_t value,      /*!< Value */
                          int32_t       scalefactor /*!< Scalefactor */
) {
    if(scalefactor > 0) return (value << scalefactor);
    else
        return (value >> (-scalefactor));
}
inline int16_t scaleValue(const int16_t value,      /*!< Value */
                          int32_t       scalefactor /*!< Scalefactor */
) {
    if(scalefactor > 0) return (value << scalefactor);
    else
        return (value >> (-scalefactor));
}
#endif

#ifndef FUNCTION_scaleValueSaturate
    /*!
     *
     *  \brief Multiply input by \f$ 2^{scalefactor} \f$
     *  \param value The value to be scaled.
     *  \param the shift amount
     *  \return \f$ value * 2^scalefactor \f$
     *
     */
    #define FUNCTION_scaleValueSaturate
inline int32_t scaleValueSaturate(const int32_t value, int32_t scalefactor /* in range -31 ... +31 */
) {
    int32_t headroom = fixnormz_D((int32_t)value ^ (int32_t)((value >> 31))); /* headroom in range 1...32 */
    if(scalefactor >= 0) {
        /* shift left: saturate in case of headroom less/equal scalefactor */
        if(headroom <= scalefactor) {
            if(value > (int32_t)0) return (int32_t)MAXVAL_DBL; /* 0x7FFF.FFFF */
            else
                return (int32_t)MINVAL_DBL + (int32_t)1; /* 0x8000.0001 */
        }
        else { return fMax((value << scalefactor), (int32_t)MINVAL_DBL + (int32_t)1); }
    }
    else {
        scalefactor = -scalefactor;
        /* shift right: clear in case of 32-headroom greater/equal -scalefactor */
        if((DFRACT_BITS - headroom) <= scalefactor) { return (int32_t)0; }
        else { return fMax((value >> scalefactor), (int32_t)MINVAL_DBL + (int32_t)1); }
    }
}
#endif

#ifndef FUNCTION_scaleValueInPlace
    /*!
     *
     *  \brief Multiply input by \f$ 2^{scalefactor} \f$ in place
     *
     *  \return void
     *
     */
    #define FUNCTION_scaleValueInPlace
inline void scaleValueInPlace(int32_t *value,      /*!< Value */
                              int32_t  scalefactor /*!< Scalefactor */
) {
    int32_t newscale;
    /* Note: The assignment inside the if conditional allows combining a load with
     * the compare to zero (on ARM and maybe others) */
    if((newscale = (scalefactor)) >= 0) { *(value) <<= newscale; }
    else { *(value) >>= -newscale; }
}
#endif

/*!
 *
 *  \brief  Scale input value by 2^{scale} and saturate output to 2^{dBits-1}
 *  \return scaled and saturated value
 *
 *  This macro scales src value right or left and applies saturation to
 * (2^dBits)-1 maxima output.
 */

#ifndef SATURATE_RIGHT_SHIFT
    #define SATURATE_RIGHT_SHIFT(src, scale, dBits)                                                                  \
        ((((int32_t)(src) >> (scale)) > (int32_t)(((1U) << ((dBits)-1)) - 1)) ? (int32_t)(((1U) << ((dBits)-1)) - 1) \
         : (((int32_t)(src) >> (scale)) < ~((int32_t)(((1U) << ((dBits)-1)) - 1)))                                   \
             ? ~((int32_t)(((1U) << ((dBits)-1)) - 1))                                                               \
             : ((int32_t)(src) >> (scale)))
#endif

#ifndef SATURATE_LEFT_SHIFT
    #define SATURATE_LEFT_SHIFT(src, scale, dBits)                                                                   \
        (((int32_t)(src) > ((int32_t)(((1U) << ((dBits)-1)) - 1) >> (scale))) ? (int32_t)(((1U) << ((dBits)-1)) - 1) \
         : ((int32_t)(src) < ~((int32_t)(((1U) << ((dBits)-1)) - 1) >> (scale)))                                     \
             ? ~((int32_t)(((1U) << ((dBits)-1)) - 1))                                                               \
             : ((int32_t)(src) << (scale)))
#endif

#ifndef SATURATE_SHIFT
    #define SATURATE_SHIFT(src, scale, dBits) \
        (((scale) < 0) ? SATURATE_LEFT_SHIFT((src), -(scale), (dBits)) : SATURATE_RIGHT_SHIFT((src), (scale), (dBits)))
#endif

/*
 * Alternative shift and saturate left, saturates to -0.99999 instead of -1.0000
 * to avoid problems when inverting the sign of the result.
 */
#ifndef SATURATE_LEFT_SHIFT_ALT
    #define SATURATE_LEFT_SHIFT_ALT(src, scale, dBits)                                                               \
        (((int32_t)(src) > ((int32_t)(((1U) << ((dBits)-1)) - 1) >> (scale))) ? (int32_t)(((1U) << ((dBits)-1)) - 1) \
         : ((int32_t)(src) <= ~((int32_t)(((1U) << ((dBits)-1)) - 1) >> (scale)))                                    \
             ? ~((int32_t)(((1U) << ((dBits)-1)) - 2))                                                               \
             : ((int32_t)(src) << (scale)))
#endif

#ifndef SATURATE_RIGHT_SHIFT_ALT
    #define SATURATE_RIGHT_SHIFT_ALT(src, scale, dBits)                                                              \
        ((((int32_t)(src) >> (scale)) > (int32_t)(((1U) << ((dBits)-1)) - 1)) ? (int32_t)(((1U) << ((dBits)-1)) - 1) \
         : (((int32_t)(src) >> (scale)) < ~((int32_t)(((1U) << ((dBits)-1)) - 2)))                                   \
             ? ~((int32_t)(((1U) << ((dBits)-1)) - 2))                                                               \
             : ((int32_t)(src) >> (scale)))
#endif

#ifndef SATURATE_INT_PCM_RIGHT_SHIFT
    #define SATURATE_INT_PCM_RIGHT_SHIFT(src, scale) SATURATE_RIGHT_SHIFT(src, scale, SAMPLE_BITS)
#endif

#ifndef SATURATE_INT_PCM_LEFT_SHIFT
    #define SATURATE_INT_PCM_LEFT_SHIFT(src, scale) SATURATE_LEFT_SHIFT(src, scale, SAMPLE_BITS)
#endif

#endif /* #ifndef SCALE_H */
