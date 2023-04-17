

/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#ifndef DRCDEC_TOOLS_H
#define DRCDEC_TOOLS_H

#include <stdint.h>
#include "../libAACdec/newAACDecoder.h"
#include "drcDec_types.h"
#include "drcDec_selectionProcess.h"

int32_t getDeltaTmin(const int32_t sampleRate);

DRC_COEFFICIENTS_UNI_DRC_t* selectDrcCoefficients(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig, const int32_t location);

DRC_INSTRUCTIONS_UNI_DRC_t* selectDrcInstructions(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig, const int32_t drcSetId);

DOWNMIX_INSTRUCTIONS* selectDownmixInstructions(
    HANDLE_UNI_DRC_CONFIG hUniDrcConfig, const int32_t downmixId);

DRC_ERROR
deriveDrcChannelGroups(
    const int32_t drcSetEffect,                                    /* in */
    const int32_t channelCount,                                    /* in */
    const int8_t* gainSetIndex,                                 /* in */
    const DUCKING_MODIFICATION_t* duckingModificationForChannel, /* in */
    uint8_t* nDrcChannelGroups,                                  /* out */
    int8_t* uniqueIndex,     /* out (gainSetIndexForChannelGroup) */
    int8_t* groupForChannel, /* out */
    DUCKING_MODIFICATION_t* duckingModificationForChannelGroup); /* out */

int32_t
dB2lin(const int32_t dB_m, const int32_t dB_e, int32_t* pLin_e);

int32_t
lin2dB(const int32_t lin_m, const int32_t lin_e, int32_t* pDb_e);

int32_t
approxDb2lin(const int32_t dB_m, const int32_t dB_e, int32_t* pLin_e);

int32_t bitstreamContainsMultibandDrc(HANDLE_UNI_DRC_CONFIG hUniDrcConfig,
                                  const int32_t downmixId);

int32_t
getDownmixOffset(DOWNMIX_INSTRUCTIONS* pDown, int32_t baseChannelCount);

#endif
