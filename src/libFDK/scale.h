
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
void    scaleCplxValues(int32_t *r_dst, int32_t *i_dst, const int32_t *r_src, const int32_t *i_src, int32_t len, int32_t scalefactor);
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

#define FUNCTION_scaleValue
#define FUNCTION_scaleValueSaturate
#define FUNCTION_scaleValueInPlace


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


inline void scaleValueInPlace(int32_t *value,      /*!< Value */
                              int32_t  scalefactor /*!< Scalefactor */
) {
    int32_t newscale;
    /* Note: The assignment inside the if conditional allows combining a load with
     * the compare to zero (on ARM and maybe others) */
    if((newscale = (scalefactor)) >= 0) { *(value) <<= newscale; }
    else { *(value) >>= -newscale; }
}







#endif /* #ifndef SCALE_H */
