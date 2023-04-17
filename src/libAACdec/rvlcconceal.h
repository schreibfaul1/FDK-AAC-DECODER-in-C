
/**************************** AAC decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  rvlc concealment
  \author Josef Hoepfl
*/

#ifndef RVLCCONCEAL_H
#define RVLCCONCEAL_H
#include "newAACDecoder.h"
#include "rvlc.h"

void BidirectionalEstimation_UseLowerScfOfCurrentFrame(
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo);

void BidirectionalEstimation_UseScfOfPrevFrameAsReference(
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo);

void StatisticalEstimation(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo);

void PredictiveInterpolation(
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo);

#endif /* RVLCCONCEAL_H */
