

/**************************** AAC decoder library ******************************

   Author(s):   Manuel Jander

   Description:

*******************************************************************************/

#ifndef AACDECODER_LIB_H
#define AACDECODER_LIB_H
#include "newAACDecoder.h"
/**
 * \file   aacdecoder_lib.h
 * \brief  FDK AAC decoder library interface header file.
 *

\page INTRO Introduction


*/

#include "../libAACdec/newAACDecoder.h"


typedef struct AAC_DECODER_INSTANCE *HANDLE_AACDECODER; /*!< Pointer to a AAC decoder instance. */
#ifdef __cplusplus
extern "C" {
#endif


AAC_DECODER_ERROR_t aacDecoder_AncDataInit(HANDLE_AACDECODER self, uint8_t *buffer, int32_t size);
AAC_DECODER_ERROR_t aacDecoder_AncDataGet(HANDLE_AACDECODER self, int32_t index, uint8_t **ptr, int32_t *size);
AAC_DECODER_ERROR_t aacDecoder_SetParam(const HANDLE_AACDECODER self, const AACDEC_PARAM_t param, const int32_t value);
AAC_DECODER_ERROR_t aacDecoder_GetFreeBytes(const HANDLE_AACDECODER self, uint32_t *pFreeBytes);
HANDLE_AACDECODER aacDecoder_Open(TRANSPORT_TYPE_t transportFmt, uint32_t nrOfLayers);
AAC_DECODER_ERROR_t aacDecoder_ConfigRaw(HANDLE_AACDECODER self, uint8_t *conf[], const uint32_t length[]);
AAC_DECODER_ERROR_t aacDecoder_RawISOBMFFData(HANDLE_AACDECODER self, uint8_t *buffer, uint32_t length);
AAC_DECODER_ERROR_t aacDecoder_Fill(HANDLE_AACDECODER self, uint8_t *pBuffer, const uint32_t bufferSize, uint32_t *bytesValid);
AAC_DECODER_ERROR_t aacDecoder_DecodeFrame(HANDLE_AACDECODER self, int16_t *pTimeData, const int32_t timeDataSize, const uint32_t flags);
void aacDecoder_Close(HANDLE_AACDECODER self);
CStreamInfo *aacDecoder_GetStreamInfo(HANDLE_AACDECODER self);
int32_t aacDecoder_GetLibInfo(LIB_INFO_t *info);

#ifdef __cplusplus
}
#endif

#endif /* AACDECODER_LIB_H */
