

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: SAC Dec QMF processing

*******************************************************************************/

#ifndef SAC_QMF_H
#define SAC_QMF_H
#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/common_fix.h"

#include "sac_dec_interface.h"

#include "../libFDK/FDK_qmf_domain.h"
#define HANDLE_SPACE_ANALYSIS_QMF HANDLE_QMF_FILTER_BANK
#define HANDLE_SPACE_SYNTHESIS_QMF HANDLE_QMF_FILTER_BANK

/**
 * \brief  Convert Qmf input to output audio data.
 *
 * \param hSpaceSynthesisQmf  A Qmf Synthesis Filterbank handle.
 * \param Sr                  Pointer to Qmf input buffer.
 * \param Si                  Pointer to Qmf input buffer.
 * \param stride              Stride factor for output data, 1 if none.
 * \param timeSig             (None-)Interleaved audio output data.
 *
 * \return  Error status.
 */
SACDEC_ERROR CalculateSpaceSynthesisQmf(
    const HANDLE_FDK_QMF_DOMAIN_OUT hQmfDomainOutCh, const int32_t *Sr,
    const int32_t *Si, const int32_t stride, PCM_MPS *timeSig);

/**
 * \brief  Convert audio input data to qmf representation.
 *
 * \param hSpaceAnalysisQmf   A Qmf Analysis Filterbank handle.
 * \param timeSig             (None-)Interleavd audio input data.
 * \param Sr                  Pointer to Qmf output buffer.
 * \param Si                  Pointer to Qmf output buffer.
 *
 * \return  Error status.
 */
SACDEC_ERROR CalculateSpaceAnalysisQmf(
    HANDLE_SPACE_ANALYSIS_QMF hSpaceAnalysisQmf, const PCM_MPS *timeSig,
    int32_t *Sr, int32_t *Si);

#endif /* SAC_QMF_H */
