

/******************* Library for basic calculation routines ********************

   Author(s):   M. Lohwasser, M. Gayer

   Description:

*******************************************************************************/

#ifndef FFT_RAD2_H
#define FFT_RAD2_H
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"

/**
 * \brief Performe an inplace  complex valued FFT of 2^n length
 *
 * \param x Input/Output data buffer. The input data must have at least 1 bit
 * scale headroom. The values are interleaved, real/imag pairs.
 * \param ldn log2 of FFT length
 * \param trigdata Pointer to a sinetable of a length of at least (2^ldn)/2 sine
 * values.
 * \param trigDataSize length of the sinetable "trigdata".
 */
void dit_fft(int32_t *x, const int32_t ldn, const FIXP_SPK_t *trigdata,
             const int32_t trigDataSize);

#endif /* FFT_RAD2_H */
