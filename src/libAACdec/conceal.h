
/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: independent channel concealment

*******************************************************************************/

#ifndef CONCEAL_H
#define CONCEAL_H
#include "newAACDecoder.h"
#include "channelinfo.h"

#define AACDEC_CONCEAL_PARAM_NOT_SPECIFIED (0xFFFE)

void CConcealment_InitCommonData(CConcealParams *pConcealCommonData);

void CConcealment_InitChannelData(CConcealmentInfo *hConcealmentInfo,
                                  CConcealParams *pConcealCommonData,
                                  AACDEC_RENDER_MODE initRenderMode,
                                  int32_t samplesPerFrame);

CConcealmentMethod CConcealment_GetMethod(CConcealParams *pConcealCommonData);

uint32_t CConcealment_GetDelay(CConcealParams *pConcealCommonData);

AAC_DECODER_ERROR
CConcealment_SetParams(CConcealParams *concealParams, int32_t method,
                       int32_t fadeOutSlope, int32_t fadeInSlope, int32_t muteRelease,
                       int32_t comfNoiseLevel);

CConcealmentState CConcealment_GetState(CConcealmentInfo *hConcealmentInfo);

AAC_DECODER_ERROR
CConcealment_SetAttenuation(CConcealParams *concealParams,
                            const int16_t *fadeOutAttenuationVector,
                            const int16_t *fadeInAttenuationVector);

void CConcealment_Store(
    CConcealmentInfo *hConcealmentInfo,
    CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo);

int32_t CConcealment_Apply(
    CConcealmentInfo *hConcealmentInfo,
    CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo,
    const SamplingRateInfo *pSamplingRateInfo, const int32_t samplesPerFrame,
    const uint8_t lastLpdMode, const int32_t FrameOk, const uint32_t flags);

int32_t CConcealment_GetLastFrameOk(CConcealmentInfo *hConcealmentInfo,
                                const int32_t fBeforeApply);

int32_t CConcealment_TDFading(
    int32_t len, CAacDecoderStaticChannelInfo **ppAacDecoderStaticChannelInfo,
    const int32_t aacOutDataHeadroom, int32_t *pcmdata, int32_t *pcmdata_1);

#endif /* #ifndef CONCEAL_H */
