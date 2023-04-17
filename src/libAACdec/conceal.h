
/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: independent channel concealment

*******************************************************************************/

#ifndef CONCEAL_H
#define CONCEAL_H
#include "newAACDecoder.h"
#include "channelinfo.h"

#define AACDEC_CONCEAL_PARAM_NOT_SPECIFIED (0xFFFE)

void CConcealment_InitCommonData(CConcealParams_t *pConcealCommonData);

void CConcealment_InitChannelData(CConcealmentInfo_t *hConcealmentInfo,
                                  CConcealParams_t *pConcealCommonData,
                                  AACDEC_RENDER_MODE_t initRenderMode,
                                  int32_t samplesPerFrame);

CConcealmentMethod_t CConcealment_GetMethod(CConcealParams_t *pConcealCommonData);

uint32_t CConcealment_GetDelay(CConcealParams_t *pConcealCommonData);

AAC_DECODER_ERROR_t
CConcealment_SetParams(CConcealParams_t *concealParams, int32_t method,
                       int32_t fadeOutSlope, int32_t fadeInSlope, int32_t muteRelease,
                       int32_t comfNoiseLevel);

CConcealmentState_t CConcealment_GetState(CConcealmentInfo_t *hConcealmentInfo);

AAC_DECODER_ERROR_t
CConcealment_SetAttenuation(CConcealParams_t *concealParams,
                            const int16_t *fadeOutAttenuationVector,
                            const int16_t *fadeInAttenuationVector);

void CConcealment_Store(
    CConcealmentInfo_t *hConcealmentInfo,
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo);

int32_t CConcealment_Apply(
    CConcealmentInfo_t *hConcealmentInfo,
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
    const SamplingRateInfo_t *pSamplingRateInfo, const int32_t samplesPerFrame,
    const uint8_t lastLpdMode, const int32_t FrameOk, const uint32_t flags);

int32_t CConcealment_GetLastFrameOk(CConcealmentInfo_t *hConcealmentInfo,
                                const int32_t fBeforeApply);

int32_t CConcealment_TDFading(
    int32_t len, CAacDecoderStaticChannelInfo_t **ppAacDecoderStaticChannelInfo,
    const int32_t aacOutDataHeadroom, int32_t *pcmdata, int32_t *pcmdata_1);

#endif /* #ifndef CONCEAL_H */
