

/******************* Library for basic calculation routines ********************

   Author(s):

   Description: Scaling operations

*******************************************************************************/
#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"

/**************************************************
 * Inline definitions
 **************************************************/

#include "scale.h"

#define SHORT_BITS 16

// #if defined(__mips__)
// #include "mips/scale_mips.cpp"

// #elif defined(__arm__)
// #include "arm/scale_arm.cpp"

// #endif

#ifndef FUNCTION_scaleValues_SGL
    /*!
     *
     *  \brief  Multiply input vector by \f$ 2^{scalefactor} \f$
     *  \param len    must be larger than 4
     *  \return void
     *
     */
    #define FUNCTION_scaleValues_SGL
void scaleValues(int16_t *vector,     /*!< Vector */
                 int32_t  len,        /*!< Length */
                 int32_t  scalefactor /*!< Scalefactor */
) {
    int32_t i;

    /* Return if scalefactor is Zero */
    if(scalefactor == 0) return;

    if(scalefactor > 0) {
        scalefactor = fixmin_I(scalefactor, (int32_t)(FRACT_BITS - 1));
        for(i = len & 3; i--;) { *(vector++) <<= scalefactor; }
        for(i = len >> 2; i--;) {
            *(vector++) <<= scalefactor;
            *(vector++) <<= scalefactor;
            *(vector++) <<= scalefactor;
            *(vector++) <<= scalefactor;
        }
    }
    else {
        int32_t negScalefactor = fixmin_I(-scalefactor, (int32_t)FRACT_BITS - 1);
        for(i = len & 3; i--;) { *(vector++) >>= negScalefactor; }
        for(i = len >> 2; i--;) {
            *(vector++) >>= negScalefactor;
            *(vector++) >>= negScalefactor;
            *(vector++) >>= negScalefactor;
            *(vector++) >>= negScalefactor;
        }
    }
}
#endif

#ifndef FUNCTION_scaleValues_DBL
    /*!
     *
     *  \brief  Multiply input vector by \f$ 2^{scalefactor} \f$
     *  \param len must be larger than 4
     *  \return void
     *
     */
    #define FUNCTION_scaleValues_DBL
SCALE_INLINE
void scaleValues(int32_t *vector,     /*!< Vector */
                 int32_t  len,        /*!< Length */
                 int32_t  scalefactor /*!< Scalefactor */
) {
    int32_t i;

    /* Return if scalefactor is Zero */
    if(scalefactor == 0) return;

    if(scalefactor > 0) {
        scalefactor = fixmin_I(scalefactor, (int32_t)DFRACT_BITS - 1);
        for(i = len & 3; i--;) { *(vector++) <<= scalefactor; }
        for(i = len >> 2; i--;) {
            *(vector++) <<= scalefactor;
            *(vector++) <<= scalefactor;
            *(vector++) <<= scalefactor;
            *(vector++) <<= scalefactor;
        }
    }
    else {
        int32_t negScalefactor = fixmin_I(-scalefactor, (int32_t)DFRACT_BITS - 1);
        for(i = len & 3; i--;) { *(vector++) >>= negScalefactor; }
        for(i = len >> 2; i--;) {
            *(vector++) >>= negScalefactor;
            *(vector++) >>= negScalefactor;
            *(vector++) >>= negScalefactor;
            *(vector++) >>= negScalefactor;
        }
    }
}
#endif

#ifndef FUNCTION_scaleValuesSaturate_DBL
    /*!
     *
     *  \brief  Multiply input vector by \f$ 2^{scalefactor} \f$
     *  \param vector      source/destination buffer
     *  \param len         length of vector
     *  \param scalefactor amount of shifts to be applied
     *  \return void
     *
     */
    #define FUNCTION_scaleValuesSaturate_DBL
SCALE_INLINE
void scaleValuesSaturate(int32_t *vector,     /*!< Vector */
                         int32_t  len,        /*!< Length */
                         int32_t  scalefactor /*!< Scalefactor */
) {
    int32_t i;

    /* Return if scalefactor is Zero */
    if(scalefactor == 0) return;

    scalefactor = fixmax_I(fixmin_I(scalefactor, (int32_t)DFRACT_BITS - 1), (int32_t) - (DFRACT_BITS - 1));

    for(i = 0; i < len; i++) { vector[i] = scaleValueSaturate(vector[i], scalefactor); }
}
#endif /* FUNCTION_scaleValuesSaturate_DBL */

#ifndef FUNCTION_scaleValuesSaturate_DBL_DBL
    /*!
     *
     *  \brief  Multiply input vector by \f$ 2^{scalefactor} \f$
     *  \param dst         destination buffer
     *  \param src         source buffer
     *  \param len         length of vector
     *  \param scalefactor amount of shifts to be applied
     *  \return void
     *
     */
    #define FUNCTION_scaleValuesSaturate_DBL_DBL
SCALE_INLINE
void scaleValuesSaturate(int32_t       *dst,        /*!< Output */
                         const int32_t *src,        /*!< Input   */
                         int32_t        len,        /*!< Length */
                         int32_t        scalefactor /*!< Scalefactor */
) {
    int32_t i;

    /* Return if scalefactor is Zero */
    if(scalefactor == 0) {
        memmove(dst, src, len * sizeof(int32_t));
        return;
    }

    scalefactor = fixmax_I(fixmin_I(scalefactor, (int32_t)DFRACT_BITS - 1), (int32_t) - (DFRACT_BITS - 1));

    for(i = 0; i < len; i++) { dst[i] = scaleValueSaturate(src[i], scalefactor); }
}
#endif /* FUNCTION_scaleValuesSaturate_DBL_DBL */

#ifndef FUNCTION_scaleValuesSaturate_SGL_DBL
    /*!
     *
     *  \brief  Multiply input vector by \f$ 2^{scalefactor} \f$
     *  \param dst         destination buffer (int16_t)
     *  \param src         source buffer (int32_t)
     *  \param len         length of vector
     *  \param scalefactor amount of shifts to be applied
     *  \return void
     *
     */
    #define FUNCTION_scaleValuesSaturate_SGL_DBL
SCALE_INLINE
void scaleValuesSaturate(int16_t       *dst,  /*!< Output */
                         const int32_t *src,  /*!< Input   */
                         int32_t        len,  /*!< Length */
                         int32_t        scalefactor) /*!< Scalefactor */
{
    int32_t i;
    scalefactor = fixmax_I(fixmin_I(scalefactor, (int32_t)DFRACT_BITS - 1), (int32_t) - (DFRACT_BITS - 1));

    for(i = 0; i < len; i++) {
        dst[i] = FX_DBL2FX_SGL(fAddSaturate(scaleValueSaturate(src[i], scalefactor), (int32_t)0x8000));
    }
}
#endif /* FUNCTION_scaleValuesSaturate_SGL_DBL */

#ifndef FUNCTION_scaleValuesSaturate_SGL
    /*!
     *
     *  \brief  Multiply input vector by \f$ 2^{scalefactor} \f$
     *  \param vector      source/destination buffer
     *  \param len         length of vector
     *  \param scalefactor amount of shifts to be applied
     *  \return void
     *
     */
    #define FUNCTION_scaleValuesSaturate_SGL
SCALE_INLINE
void scaleValuesSaturate(int16_t *vector,     /*!< Vector */
                         int32_t  len,        /*!< Length */
                         int32_t  scalefactor /*!< Scalefactor */
) {
    int32_t i;

    /* Return if scalefactor is Zero */
    if(scalefactor == 0) return;

    scalefactor = fixmax_I(fixmin_I(scalefactor, (int32_t)DFRACT_BITS - 1), (int32_t) - (DFRACT_BITS - 1));

    for(i = 0; i < len; i++) { vector[i] = FX_DBL2FX_SGL(scaleValueSaturate(FX_SGL2FX_DBL(vector[i]), scalefactor)); }
}
#endif /* FUNCTION_scaleValuesSaturate_SGL */

#ifndef FUNCTION_scaleValuesSaturate_SGL_SGL
    /*!
     *
     *  \brief  Multiply input vector by \f$ 2^{scalefactor} \f$
     *  \param dst         destination buffer
     *  \param src         source buffer
     *  \param len         length of vector
     *  \param scalefactor amount of shifts to be applied
     *  \return void
     *
     */
    #define FUNCTION_scaleValuesSaturate_SGL_SGL
SCALE_INLINE
void scaleValuesSaturate(int16_t       *dst,        /*!< Output */
                         const int16_t *src,        /*!< Input */
                         int32_t        len,        /*!< Length */
                         int32_t        scalefactor /*!< Scalefactor */
) {
    int32_t i;

    /* Return if scalefactor is Zero */
    if(scalefactor == 0) {
        memmove(dst, src, len * sizeof(int16_t));
        return;
    }

    scalefactor = fixmax_I(fixmin_I(scalefactor, (int32_t)DFRACT_BITS - 1), (int32_t) - (DFRACT_BITS - 1));

    for(i = 0; i < len; i++) { dst[i] = FX_DBL2FX_SGL(scaleValueSaturate(FX_SGL2FX_DBL(src[i]), scalefactor)); }
}
#endif /* FUNCTION_scaleValuesSaturate_SGL_SGL */

#ifndef FUNCTION_scaleValues_DBLDBL
    /*!
     *
     *  \brief  Multiply input vector src by \f$ 2^{scalefactor} \f$
     *          and place result into dst
     *  \param dst detination buffer
     *  \param src source buffer
     *  \param len must be larger than 4
     *  \param scalefactor amount of left shifts to be applied
     *  \return void
     *
     */
    #define FUNCTION_scaleValues_DBLDBL
SCALE_INLINE
void scaleValues(int32_t       *dst,        /*!< dst Vector */
                 const int32_t *src,        /*!< src Vector */
                 int32_t        len,        /*!< Length */
                 int32_t        scalefactor /*!< Scalefactor */
) {
    int32_t i;

    /* Return if scalefactor is Zero */
    if(scalefactor == 0) {
        if(dst != src) memmove(dst, src, len * sizeof(int32_t));
    }
    else {
        if(scalefactor > 0) {
            scalefactor = fixmin_I(scalefactor, (int32_t)DFRACT_BITS - 1);
            for(i = len & 3; i--;) { *(dst++) = *(src++) << scalefactor; }
            for(i = len >> 2; i--;) {
                *(dst++) = *(src++) << scalefactor;
                *(dst++) = *(src++) << scalefactor;
                *(dst++) = *(src++) << scalefactor;
                *(dst++) = *(src++) << scalefactor;
            }
        }
        else {
            int32_t negScalefactor = fixmin_I(-scalefactor, (int32_t)DFRACT_BITS - 1);
            for(i = len & 3; i--;) { *(dst++) = *(src++) >> negScalefactor; }
            for(i = len >> 2; i--;) {
                *(dst++) = *(src++) >> negScalefactor;
                *(dst++) = *(src++) >> negScalefactor;
                *(dst++) = *(src++) >> negScalefactor;
                *(dst++) = *(src++) >> negScalefactor;
            }
        }
    }
}
#endif

#if(SAMPLE_BITS == 16)
    #ifndef FUNCTION_scaleValues_PCMDBL
        /*!
         *
         *  \brief  Multiply input vector src by \f$ 2^{scalefactor} \f$
         *          and place result into dst
         *  \param dst detination buffer
         *  \param src source buffer
         *  \param len must be larger than 4
         *  \param scalefactor amount of left shifts to be applied
         *  \return void
         *
         */
        #define FUNCTION_scaleValues_PCMDBL
SCALE_INLINE
void scaleValues(FIXP_PCM      *dst,        /*!< dst Vector */
                 const int32_t *src,        /*!< src Vector */
                 int32_t        len,        /*!< Length */
                 int32_t        scalefactor /*!< Scalefactor */
) {
    int32_t i;

    scalefactor -= DFRACT_BITS - SAMPLE_BITS;

    /* Return if scalefactor is Zero */
    {
        if(scalefactor > 0) {
            scalefactor = fixmin_I(scalefactor, (int32_t)DFRACT_BITS - 1);
            for(i = len & 3; i--;) { *(dst++) = (FIXP_PCM)(*(src++) << scalefactor); }
            for(i = len >> 2; i--;) {
                *(dst++) = (FIXP_PCM)(*(src++) << scalefactor);
                *(dst++) = (FIXP_PCM)(*(src++) << scalefactor);
                *(dst++) = (FIXP_PCM)(*(src++) << scalefactor);
                *(dst++) = (FIXP_PCM)(*(src++) << scalefactor);
            }
        }
        else {
            int32_t negScalefactor = fixmin_I(-scalefactor, (int32_t)DFRACT_BITS - 1);
            for(i = len & 3; i--;) { *(dst++) = (FIXP_PCM)(*(src++) >> negScalefactor); }
            for(i = len >> 2; i--;) {
                *(dst++) = (FIXP_PCM)(*(src++) >> negScalefactor);
                *(dst++) = (FIXP_PCM)(*(src++) >> negScalefactor);
                *(dst++) = (FIXP_PCM)(*(src++) >> negScalefactor);
                *(dst++) = (FIXP_PCM)(*(src++) >> negScalefactor);
            }
        }
    }
}
    #endif
#endif /* (SAMPLE_BITS == 16) */

#ifndef FUNCTION_scaleValues_SGLSGL
    /*!
     *
     *  \brief  Multiply input vector src by \f$ 2^{scalefactor} \f$
     *          and place result into dst
     *  \param dst detination buffer
     *  \param src source buffer
     *  \param len must be larger than 4
     *  \param scalefactor amount of left shifts to be applied
     *  \return void
     *
     */
    #define FUNCTION_scaleValues_SGLSGL
SCALE_INLINE
void scaleValues(int16_t       *dst,        /*!< dst Vector */
                 const int16_t *src,        /*!< src Vector */
                 int32_t        len,        /*!< Length */
                 int32_t        scalefactor /*!< Scalefactor */
) {
    int32_t i;

    /* Return if scalefactor is Zero */
    if(scalefactor == 0) {
        if(dst != src) memmove(dst, src, len * sizeof(int32_t));
    }
    else {
        if(scalefactor > 0) {
            scalefactor = fixmin_I(scalefactor, (int32_t)DFRACT_BITS - 1);
            for(i = len & 3; i--;) { *(dst++) = *(src++) << scalefactor; }
            for(i = len >> 2; i--;) {
                *(dst++) = *(src++) << scalefactor;
                *(dst++) = *(src++) << scalefactor;
                *(dst++) = *(src++) << scalefactor;
                *(dst++) = *(src++) << scalefactor;
            }
        }
        else {
            int32_t negScalefactor = fixmin_I(-scalefactor, (int32_t)DFRACT_BITS - 1);
            for(i = len & 3; i--;) { *(dst++) = *(src++) >> negScalefactor; }
            for(i = len >> 2; i--;) {
                *(dst++) = *(src++) >> negScalefactor;
                *(dst++) = *(src++) >> negScalefactor;
                *(dst++) = *(src++) >> negScalefactor;
                *(dst++) = *(src++) >> negScalefactor;
            }
        }
    }
}
#endif

#ifndef FUNCTION_scaleValuesWithFactor_DBL
    /*!
     *
     *  \brief  Multiply input vector by \f$ 2^{scalefactor} \f$
     *  \param len must be larger than 4
     *  \return void
     *
     */
    #define FUNCTION_scaleValuesWithFactor_DBL
SCALE_INLINE
void scaleValuesWithFactor(int32_t *vector, int32_t factor, int32_t len, int32_t scalefactor) {
    int32_t i;

    /* Compensate fMultDiv2 */
    scalefactor++;

    if(scalefactor > 0) {
        scalefactor = fixmin_I(scalefactor, (int32_t)DFRACT_BITS - 1);
        for(i = len & 3; i--;) {
            *vector = fMultDiv2(*vector, factor) << scalefactor;
            vector++;
        }
        for(i = len >> 2; i--;) {
            *vector = fMultDiv2(*vector, factor) << scalefactor;
            vector++;
            *vector = fMultDiv2(*vector, factor) << scalefactor;
            vector++;
            *vector = fMultDiv2(*vector, factor) << scalefactor;
            vector++;
            *vector = fMultDiv2(*vector, factor) << scalefactor;
            vector++;
        }
    }
    else {
        int32_t negScalefactor = fixmin_I(-scalefactor, (int32_t)DFRACT_BITS - 1);
        for(i = len & 3; i--;) {
            *vector = fMultDiv2(*vector, factor) >> negScalefactor;
            vector++;
        }
        for(i = len >> 2; i--;) {
            *vector = fMultDiv2(*vector, factor) >> negScalefactor;
            vector++;
            *vector = fMultDiv2(*vector, factor) >> negScalefactor;
            vector++;
            *vector = fMultDiv2(*vector, factor) >> negScalefactor;
            vector++;
            *vector = fMultDiv2(*vector, factor) >> negScalefactor;
            vector++;
        }
    }
}
#endif /* FUNCTION_scaleValuesWithFactor_DBL */

/*******************************************

IMPORTANT NOTE for usage of getScalefactor()

If the input array contains negative values too, then these functions may
sometimes return the actual maximum value minus 1, due to the nature of the
applied algorithm. So be careful with possible fractional -1 values that may
lead to overflows when being fPow2()'ed.

********************************************/

#ifndef FUNCTION_getScalefactorShort
    /*!
     *
     *  \brief Calculate max possible scale factor for input vector of shorts
     *
     *  \return Maximum scale factor / possible left shift
     *
     */
    #define FUNCTION_getScalefactorShort
SCALE_INLINE
int32_t getScalefactorShort(const int16_t *vector, /*!< Pointer to input vector */
                            int32_t        len     /*!< Length of input vector */
) {
    int32_t i;
    int16_t temp, maxVal = 0;

    for(i = len; i != 0; i--) {
        temp = (int16_t)(*vector++);
        maxVal |= (temp ^ (temp >> (SHORT_BITS - 1)));
    }

    return fixmax_I((int32_t)0,
                    (int32_t)(fixnormz_D((int32_t)maxVal) - (int32_t)1 - (int32_t)(DFRACT_BITS - SHORT_BITS)));
}
#endif

#ifndef FUNCTION_getScalefactorPCM
    /*!
     *
     *  \brief Calculate max possible scale factor for input vector of shorts
     *
     *  \return Maximum scale factor
     *
     */
    #define FUNCTION_getScalefactorPCM
SCALE_INLINE
int32_t getScalefactorPCM(const int16_t *vector, /*!< Pointer to input vector */
                          int32_t        len,    /*!< Length of input vector */
                          int32_t        stride) {
    int32_t i;
    int16_t temp, maxVal = 0;

    for(i = len; i != 0; i--) {
        temp = (int16_t)(*vector);
        vector += stride;
        maxVal |= (temp ^ (temp >> ((sizeof(int16_t) * 8) - 1)));
    }
    return fixmax_I((int32_t)0,
                    (int32_t)(fixnormz_D((int32_t)maxVal) - (int32_t)1 - (int32_t)(DFRACT_BITS - SAMPLE_BITS)));
}
#endif

#ifndef FUNCTION_getScalefactorShort
    /*!
     *
     *  \brief Calculate max possible scale factor for input vector of shorts
     *  \param stride, item increment between vector members.
     *  \return Maximum scale factor
     *
     */
    #define FUNCTION_getScalefactorShort
SCALE_INLINE
int32_t getScalefactorShort(const int16_t *vector, /*!< Pointer to input vector */
                            int32_t        len,    /*!< Length of input vector */
                            int32_t        stride) {
    int32_t i;
    int16_t temp, maxVal = 0;

    for(i = len; i != 0; i--) {
        temp = (int16_t)(*vector);
        vector += stride;
        maxVal |= (temp ^ (temp >> (SHORT_BITS - 1)));
    }

    return fixmax_I((int32_t)0,
                    (int32_t)(fixnormz_D((int32_t)maxVal) - (int32_t)1 - (int32_t)(DFRACT_BITS - SHORT_BITS)));
}
#endif

#ifndef FUNCTION_getScalefactor_DBL
    /*!
     *
     *  \brief Calculate max possible scale factor for input vector
     *
     *  \return Maximum scale factor
     *
     *  This function can constitute a significant amount of computational
     * complexity - very much depending on the bitrate. Since it is a rather small
     * function, effective assembler optimization might be possible.
     *
     *  If all data is 0xFFFF.FFFF or 0x0000.0000 function returns 31
     *  Note: You can skip data normalization only if return value is 0
     *
     */
    #define FUNCTION_getScalefactor_DBL
SCALE_INLINE
int32_t getScalefactor(const int32_t *vector, /*!< Pointer to input vector */
                       int32_t        len)           /*!< Length of input vector */
{
    int32_t i;
    int32_t temp, maxVal = (int32_t)0;

    for(i = len; i != 0; i--) {
        temp = (int32_t)(*vector++);
        maxVal |= (int32_t)((int32_t)temp ^ (int32_t)(temp >> (DFRACT_BITS - 1)));
    }

    return fixmax_I((int32_t)0, (int32_t)(fixnormz_D(maxVal) - 1));
}
#endif

#ifndef FUNCTION_getScalefactor_SGL
    #define FUNCTION_getScalefactor_SGL
SCALE_INLINE
int32_t getScalefactor(const int16_t *vector, /*!< Pointer to input vector */
                       int32_t        len)           /*!< Length of input vector */
{
    int32_t i;
    int16_t temp, maxVal = (int16_t)0;

    for(i = len; i != 0; i--) {
        temp = (int16_t)(*vector++);
        maxVal |= (temp ^ (temp >> (FRACT_BITS - 1)));
    }

    return fixmax_I((int32_t)0, (int32_t)(fixnormz_S((int16_t)maxVal)) - 1);
}
#endif
