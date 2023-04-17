

/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#ifndef DRCGAINDEC_INIT_H
#define DRCGAINDEC_INIT_H
#include "../libAACdec/newAACDecoder.h"
DRC_ERROR
initGainDec(HANDLE_DRC_GAIN_DECODER hGainDec);

void initDrcGainBuffers(const int32_t frameSize, DRC_GAIN_BUFFERS* drcGainBuffers);

DRC_ERROR
initActiveDrc(HANDLE_DRC_GAIN_DECODER hGainDec,
              HANDLE_UNI_DRC_CONFIG hUniDrcConfig, const int32_t drcSetIdSelected,
              const int32_t downmixIdSelected);

DRC_ERROR
initActiveDrcOffset(HANDLE_DRC_GAIN_DECODER hGainDec);

#endif
