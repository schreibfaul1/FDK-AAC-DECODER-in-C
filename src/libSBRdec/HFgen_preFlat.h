

/**************************** SBR decoder library ******************************

   Author(s):   Manuel Jander, Matthias Hildenbrand

   Description: QMF frequency pre whitening for SBR

*******************************************************************************/

#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/common_fix.h"

#ifndef HFGEN_PREFLAT_H
#define HFGEN_PREFLAT_H

#define GAIN_VEC_EXP 6 /* exponent of GainVec[] */

/**
 * \brief Find gain vector to flatten the QMF frequency bands whithout loosing
 * the fine structure.
 * \param[in] sourceBufferReal real part of QMF domain data.
 * \param[in] sourceBufferImag imaginary part of QMF domain data.
 * \param[in] sourceBuffer_e_overlap exponent of sourceBufferReal.
 * \param[in] sourceBuffer_e_current exponent of sourceBufferImag.
 * \param[in] overlap number of overlap samples.
 * \param[out] GainVec array of gain values (one for each QMF band).
 * \param[out] GainVec_exp exponents of GainVec (one for each QMF band).
 * \param[in] numBands number of low bands (k_0).
 * \param[in] startSample time slot start.
 * \param[in] stopSample time slot stop.
 */
void sbrDecoder_calculateGainVec(int32_t **sourceBufferReal,
                                 int32_t **sourceBufferImag,
                                 int32_t sourceBuffer_e_overlap,
                                 int32_t sourceBuffer_e_current, int32_t overlap,
                                 int32_t GainVec[], int32_t GainVec_exp[],
                                 const int32_t numBands, const int32_t startSample,
                                 const int32_t stopSample);

#endif /* __HFGEN_PREFLAT_H */
