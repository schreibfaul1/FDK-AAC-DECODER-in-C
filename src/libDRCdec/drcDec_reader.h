

/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#ifndef DRCDEC_READER_H
#define DRCDEC_READER_H
#include "../libAACdec/newAACDecoder.h"
#include "drcDecoder.h"
#include "drcDec_types.h"
#include "../libFDK/FDK_bitstream.h"

DRC_ERROR
drcDec_readUniDrc(HANDLE_FDK_BITSTREAM hBs, HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
                  HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet,
                  const int32_t frameSize, const int32_t deltaTminDefault,
                  HANDLE_UNI_DRC_GAIN hUniDrcGain);

DRC_ERROR
drcDec_readUniDrcGain(HANDLE_FDK_BITSTREAM hBs,
                      HANDLE_UNI_DRC_CONFIG hUniDrcConfig, const int32_t frameSize,
                      const int32_t deltaTminDefault,
                      HANDLE_UNI_DRC_GAIN hUniDrcGain);

DRC_ERROR
drcDec_readUniDrcConfig(HANDLE_FDK_BITSTREAM hBs,
                        HANDLE_UNI_DRC_CONFIG hUniDrcConfig);

DRC_ERROR
drcDec_readLoudnessInfoSet(HANDLE_FDK_BITSTREAM hBs,
                           HANDLE_LOUDNESS_INFO_SET hLoudnessInfoSet);

#endif
