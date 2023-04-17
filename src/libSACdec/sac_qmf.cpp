

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: SAC Dec QMF processing

*******************************************************************************/

#include <stdint.h>
#include "../libAACdec/newAACDecoder.h"
#include "sac_qmf.h"

#include "../libFDK/FDK_matrixCalloc.h"
#include "sac_dec_interface.h"
#include "sac_rom.h"

#include "../libFDK/qmf.h"

SACDEC_ERROR CalculateSpaceSynthesisQmf(
    const HANDLE_FDK_QMF_DOMAIN_OUT hQmfDomainOutCh, const int32_t *Sr,
    const int32_t *Si, const int32_t stride, PCM_MPS *timeSig) {
  SACDEC_ERROR err = MPS_OK;

  if (hQmfDomainOutCh == NULL) {
    err = MPS_INVALID_HANDLE;
  } else {
    HANDLE_SPACE_SYNTHESIS_QMF hSpaceSynthesisQmf = &hQmfDomainOutCh->fb;

    int32_t _pWorkBuffer[(64 << 1) + (8 + sizeof(int32_t) - 1)];
    int32_t *pWorkBuffer = (int32_t *)(_pWorkBuffer + (((int32_t)8 - ((size_t)(_pWorkBuffer) & 7)) & 7));


    qmfSynthesisFilteringSlot(hSpaceSynthesisQmf, Sr, Si, 0, 0, timeSig, stride,
                              pWorkBuffer);

  }

  return err;
}

SACDEC_ERROR CalculateSpaceAnalysisQmf(
    HANDLE_SPACE_ANALYSIS_QMF hSpaceAnalysisQmf, const PCM_MPS *timeSig,
    int32_t *Sr, int32_t *Si) {
  SACDEC_ERROR err = MPS_OK;

  if (hSpaceAnalysisQmf == NULL) {
    err = MPS_INVALID_HANDLE;
  } else {
    int32_t _pWorkBuffer[(64 << 1) + (8 + sizeof(int32_t) - 1)];
    int32_t *pWorkBuffer = (int32_t *)(_pWorkBuffer + (((int32_t)8 - ((size_t)(_pWorkBuffer) & 7)) & 7));



    qmfAnalysisFilteringSlot(hSpaceAnalysisQmf, Sr, Si, timeSig, 1,
                             pWorkBuffer);
  }

  return err;
}
