

/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#ifndef DRCGAINDEC_PROCESS_H
#define DRCGAINDEC_PROCESS_H
#include "../libAACdec/newAACDecoder.h"
DRC_ERROR
processDrcTime(HANDLE_DRC_GAIN_DECODER hGainDec, const int32_t activeDrcIndex,
               const int32_t delaySamples, const int32_t channelOffset,
               const int32_t drcChannelOffset, const int32_t numChannelsProcessed,
               const int32_t timeDataChannelOffset, int32_t* deinterleavedAudio);

DRC_ERROR
processDrcSubband(HANDLE_DRC_GAIN_DECODER hGainDec, const int32_t activeDrcIndex,
                  const int32_t delaySamples, const int32_t channelOffset,
                  const int32_t drcChannelOffset, const int32_t numChannelsProcessed,
                  const int32_t processSingleTimeslot,
                  int32_t* deinterleavedAudioReal[],
                  int32_t* deinterleavedAudioImag[]);
#endif
