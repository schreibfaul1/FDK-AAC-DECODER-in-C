

/******************* Library for basic calculation routines ********************

   Author(s):   Manuel Jander, Josef Hoepfl, Youliy Ninov, Daniel Hagel

   Description: MDCT/MDST routines

*******************************************************************************/

#ifndef MDCT_H
#define MDCT_H
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"




typedef enum { SHAPE_SINE = 0, SHAPE_KBD, SHAPE_LOL } WINDOW_SHAPE;

/**
 * \brief MDCT persistent data
 */


/**
 * \brief Initialize as valid MDCT handle
 *
 * \param hMdct handle of an allocated MDCT handle.
 * \param overlap pointer to int32_t overlap buffer.
 * \param overlapBufferSize size in FIXP_DBLs of the given overlap buffer.
 */
void mdct_init(H_MDCT hMdct, int32_t *overlap, int32_t overlapBufferSize);

/**
 * \brief perform MDCT transform (time domain to frequency domain) with given
 * parameters.
 *
 * \param hMdct handle of an allocated MDCT handle.
 * \param pTimeData pointer to input time domain signal
 * \param noInSamples number of input samples
 * \param mdctData pointer to where the resulting MDCT spectrum will be stored
 * into.
 * \param nSpec number of spectra
 * \param pMdctData_e pointer to the input data exponent. Updated accordingly on
 * return for output data.
 * \return number of input samples processed.
 */
int32_t mdct_block(H_MDCT hMdct, const int16_t *pTimeData, const int32_t noInSamples,
               int32_t * mdctData, const int32_t nSpec, const int32_t tl,
               const FIXP_SPK_t *pRightWindowPart, const int32_t fr,
               int16_t *pMdctData_e);

/**
 * \brief add/multiply 2/N transform gain and MPEG4 part 3 defined output gain
 * (see definition of MDCT_OUTPUT_GAIN) to given mantissa factor and exponent.
 * \param pGain pointer to the mantissa of a gain factor to be applied to IMDCT
 * data.
 * \param pExponent pointer to the exponent of a gain factor to be applied to
 * IMDCT data.
 * \param tl length of the IMDCT where the gain *pGain * (2 ^ *pExponent) will
 * be applied to.
 */
void imdct_gain(int32_t *pGain, int32_t *pExponent, int32_t tl);

/**
 * \brief drain buffered output samples into given buffer. Changes the MDCT
 * state.
 */
int32_t imdct_drain(H_MDCT hMdct, int32_t *pTimeData, int32_t nrSamplesRoom);

/**
 * \brief Copy overlap time domain data to given buffer. Does not change the
 * MDCT state.
 * \return number of actually copied samples (ov + nr).
 */
int32_t imdct_copy_ov_and_nr(H_MDCT hMdct, int32_t *pTimeData, int32_t nrSamples);

/**
 * \brief Adapt MDCT parameters for non-matching window slopes.
 * \param hMdct handle of an allocated MDCT handle.
 * \param pfl pointer to left overlap window side length.
 * \param pnl pointer to length of the left n part of the window.
 * \param tl transform length.
 * \param wls pointer to the left side overlap window coefficients.
 * \param noOutSamples desired number of output samples.
 */
void imdct_adapt_parameters(H_MDCT hMdct, int32_t *pfl, int32_t *pnl, int32_t tl,
                            const FIXP_SPK_t *wls, int32_t noOutSamples);

/**
 * \brief perform several inverse MLT transforms (frequency domain to time
 * domain) with given parameters.
 *
 * \param hMdct handle of an allocated MDCT handle.
 * \param output pointer to where the output time domain signal will be stored
 * into.
 * \param spectrum pointer to the input MDCT spectra.
 * \param scalefactors exponents of the input spectrum.
 * \param nSpec number of MDCT spectrums.
 * \param noOutSamples desired number of output samples.
 * \param tl transform length.
 * \param wls pointer to the left side overlap window coefficients.
 * \param fl left overlap window side length.
 * \param wrs pointer to the right side overlap window coefficients of all
 * individual IMDCTs.
 * \param fr right overlap window side length of all individual IMDCTs.
 * \param gain factor to apply to output samples (if != 0).
 * \param flags flags controlling the type of transform
 * \return number of output samples returned.
 */
int32_t imlt_block(H_MDCT hMdct, int32_t *output, int32_t *spectrum,
               const int16_t scalefactor[], const int32_t nSpec,
               const int32_t noOutSamples, const int32_t tl, const FIXP_SPK_t *wls,
               int32_t fl, const FIXP_SPK_t *wrs, const int32_t fr, int32_t gain,
               int32_t flags);

#endif /* MDCT_H */
