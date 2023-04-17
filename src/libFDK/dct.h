

/******************* Library for basic calculation routines ********************

   Author(s):

   Description: Library functions to calculate standard DCTs. This will most
                likely be replaced by hand-optimized functions for the specific
                target processor.

*******************************************************************************/

#ifndef DCT_H
#define DCT_H
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"

void dct_getTables(const FIXP_SPK_t **ptwiddle, const FIXP_SPK_t **sin_twiddle,
                   int32_t *sin_step, int32_t length);

/**
 * \brief Calculate DCT type II of given length. The DCT IV is
 *        calculated by a complex FFT, with some pre and post twiddeling.
 *        A factor of sqrt(2/(N-1)) is NOT applied.
 * \param pDat pointer to input/output data (in place processing).
 * \param size size of pDat.
 * \param pDat_e pointer to an integer containing the exponent of the data
 *               referenced by pDat. The exponent is updated accordingly.
 */
void dct_II(int32_t *pDat, int32_t *tmp, int32_t size, int32_t *pDat_e);

/**
 * \brief Calculate DCT type III of given length. The DCT IV is
 *        calculated by a complex FFT, with some pre and post twiddeling.
 *        Note that the factor 0.5 for the sum term x[0] is 1.0 instead of 0.5.
 *        A factor of sqrt(2/N) is NOT applied.
 * \param pDat pointer to input/output data (in place processing).
 * \param size size of pDat.
 * \param pDat_e pointer to an integer containing the exponent of the data
 *               referenced by pDat. The exponent is updated accordingly.
 */
void dct_III(int32_t *pDat, int32_t *tmp, int32_t size, int32_t *pDat_e);

/**
 * \brief Calculate DST type III of given length. The DST III is
 *        calculated by a DCT III of mirrored input and sign-flipping of odd
 *        output coefficients.
 *        Note that the factor 0.5 for the sum term x[N-1] is 1.0 instead of
 * 0.5. A factor of sqrt(2/N) is NOT applied.
 * \param pDat pointer to input/output data (in place processing).
 * \param size size of pDat.
 * \param pDat_e pointer to an integer containing the exponent of the data
 *               referenced by pDat. The exponent is updated accordingly.
 */
void dst_III(int32_t *pDat, int32_t *tmp, int32_t size, int32_t *pDat_e);

/**
 * \brief Calculate DCT type IV of given length. The DCT IV is
 *        calculated by a complex FFT, with some pre and post twiddeling.
 *        A factor of sqrt(2/N) is NOT applied.
 * \param pDat pointer to input/output data (in place processing).
 * \param size size of pDat.
 * \param pDat_e pointer to an integer containing the exponent of the data
 *               referenced by pDat. The exponent is updated accordingly.
 */
void dct_IV(int32_t *pDat, int32_t size, int32_t *pDat_e);

/**
 * \brief Calculate DST type IV of given length. The DST IV is
 *        calculated by a complex FFT, with some pre and post twiddeling.
 *        A factor of sqrt(2/N) is NOT applied.
 * \param pDat pointer to input/output data (in place processing).
 * \param size size of pDat.
 * \param pDat_e pointer to an integer containing the exponent of the data
 *               referenced by pDat. The exponent is updated accordingly.
 */
void dst_IV(int32_t *pDat, int32_t size, int32_t *pDat_e);

#endif
