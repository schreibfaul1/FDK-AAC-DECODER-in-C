
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
    CAacDecoderChannelInfo *pAacDecoderChannelInfo);

void BidirectionalEstimation_UseScfOfPrevFrameAsReference(
    CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo);

void StatisticalEstimation(CAacDecoderChannelInfo *pAacDecoderChannelInfo);

void PredictiveInterpolation(
    CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo);

#endif /* RVLCCONCEAL_H */
