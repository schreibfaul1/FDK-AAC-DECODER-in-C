

/**************************** AAC decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  Defines structures and prototypes for RVLC
  \author Robert Weidner
*/

#ifndef RVLC_H
#define RVLC_H
#include "newAACDecoder.h"
#include "aacdecoder.h"
#include "channel.h"
#include "rvlc_info.h"

/* ------------------------------------------------------------------- */
/*    errorLogRvlc: A word of 32 bits used for logging possible errors */
/*                  within RVLC in case of distorted bitstreams.       */
/* ------------------------------------------------------------------- */
#define RVLC_ERROR_ALL_ESCAPE_WORDS_INVALID                                \
  0x80000000 /*  ESC-Dec  During RVLC-Escape-decoding there have been more \
                bits decoded as there are available */
#define RVLC_ERROR_RVL_SUM_BIT_COUNTER_BELOW_ZERO_FWD                     \
  0x40000000 /*  RVL-Dec  negative sum-bitcounter during RVL-fwd-decoding \
                (int32_t+shrt) */
#define RVLC_ERROR_RVL_SUM_BIT_COUNTER_BELOW_ZERO_BWD                     \
  0x20000000 /*  RVL-Dec  negative sum-bitcounter during RVL-fwd-decoding \
                (int32_t+shrt) */
#define RVLC_ERROR_FORBIDDEN_CW_DETECTED_FWD \
  0x08000000 /*  RVL-Dec  forbidden codeword detected fwd (int32_t+shrt) */
#define RVLC_ERROR_FORBIDDEN_CW_DETECTED_BWD \
  0x04000000 /*  RVL-Dec  forbidden codeword detected bwd (int32_t+shrt) */

void CRvlc_Read(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                HANDLE_FDK_BITSTREAM bs);

void CRvlc_Decode(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                  CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
                  HANDLE_FDK_BITSTREAM bs);

/**
 * \brief performe sanity checks to the channel data corresponding to one
 * channel element.
 * \param pAacDecoderChannelInfo
 * \param pAacDecoderStaticChannelInfo
 * \param elChannels amount of channels of the channel element.
 */
void CRvlc_ElementCheck(
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo[],
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo[],
    const uint32_t flags, const int32_t elChannels);

#endif /* RVLC_H */
