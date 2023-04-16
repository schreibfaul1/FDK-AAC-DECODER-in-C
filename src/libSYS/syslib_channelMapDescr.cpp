/************************* System integration library **************************

   Author(s):   Thomas Dietzen

   Description:

*******************************************************************************/

/** \file   syslib_channelMapDescr.cpp
 *  \brief  Implementation of routines that handle the channel map descriptor.
 */
#include  "../libAACdec/newAACDecoder.h"
#include "machine_type.h"
#include <assert.h>
#include <stdint.h>

#define DFLT_CH_MAP_TAB_LEN (15) /* Length of the default channel map info table. */

//----------------------------------------------------------------------------------------------------------------------
// The following arrays provide a channel map for each channel config (0 to 14).
static const uint8_t mapFallback[] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                                      12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
static const uint8_t mapCfg1[] = {0, 1};
static const uint8_t mapCfg2[] = {0, 1};
static const uint8_t mapCfg3[] = {2, 0, 1};
static const uint8_t mapCfg4[] = {2, 0, 1, 3};
static const uint8_t mapCfg5[] = {2, 0, 1, 3, 4};
static const uint8_t mapCfg6[] = {2, 0, 1, 4, 5, 3};
static const uint8_t mapCfg7[] = {2, 6, 7, 0, 1, 4, 5, 3};
static const uint8_t mapCfg11[] = {2, 0, 1, 4, 5, 6, 3};
static const uint8_t mapCfg12[] = {2, 0, 1, 6, 7, 4, 5, 3};
static const uint8_t mapCfg13[] = {2,  6,  7,  0,  1,  10, 11, 4,  5,  8,  3,  9,
                                   14, 12, 13, 18, 19, 15, 16, 17, 20, 21, 22, 23};
static const uint8_t mapCfg14[] = {2, 0, 1, 4, 5, 3, 6, 7};

// Default table comprising channel map information for each channel config (0 to 14).
static const CHANNEL_MAP_INFO_t mapInfoTabDflt[DFLT_CH_MAP_TAB_LEN] = {/* chCfg,  map,         numCh */
                                                                       /*  0 */ {mapFallback, 24},
                                                                       /*  1 */ {mapCfg1, 2},
                                                                       /*  2 */ {mapCfg2, 2},
                                                                       /*  3 */ {mapCfg3, 3},
                                                                       /*  4 */ {mapCfg4, 4},
                                                                       /*  5 */ {mapCfg5, 5},
                                                                       /*  6 */ {mapCfg6, 6},
                                                                       /*  7 */ {mapCfg7, 8},
                                                                       /*  8 */ {mapFallback, 24},
                                                                       /*  9 */ {mapFallback, 24},
                                                                       /* 10 */ {mapFallback, 24},
                                                                       /* 11 */ {mapCfg11, 7},
                                                                       /* 12 */ {mapCfg12, 8},
                                                                       /* 13 */ {mapCfg13, 24},
                                                                       /* 14 */ {mapCfg14, 8}};

static const uint8_t mapWg4Cfg1[] = {0, 1};
static const uint8_t mapWg4Cfg2[] = {0, 1};
static const uint8_t mapWg4Cfg3[] = {2, 0, 1};
static const uint8_t mapWg4Cfg4[] = {3, 0, 1, 2};
static const uint8_t mapWg4Cfg5[] = {4, 0, 1, 2, 3};
static const uint8_t mapWg4Cfg6[] = {4, 0, 1, 2, 3, 5};
static const uint8_t mapWg4Cfg7[] = {6, 0, 1, 2, 3, 4, 5, 7};
static const uint8_t mapWg4Cfg14[] = {6, 0, 1, 2, 3, 4, 5, 7};

const CHANNEL_MAP_INFO_t FDK_mapInfoTabWg4[] = {/* chCfg,  map,         numCh */
                                                /*  0 */ {mapFallback, 24},
                                                /*  1 */ {mapWg4Cfg1, 2},
                                                /*  2 */ {mapWg4Cfg2, 2},
                                                /*  3 */ {mapWg4Cfg3, 3},
                                                /*  4 */ {mapWg4Cfg4, 4},
                                                /*  5 */ {mapWg4Cfg5, 5},
                                                /*  6 */ {mapWg4Cfg6, 6},
                                                /*  7 */ {mapWg4Cfg7, 8},
                                                /*  8 */ {mapFallback, 24},
                                                /*  9 */ {mapFallback, 24},
                                                /* 10 */ {mapFallback, 24},
                                                /* 11 */ {mapFallback, 24},   // Unhandled for Wg4 yet
                                                /* 12 */ {mapFallback, 24},   // Unhandled for Wg4 yet
                                                /* 13 */ {mapFallback, 24},   // Unhandled for Wg4 yet
                                                /* 14 */ {mapFallback, 24}};  // Unhandled for Wg4 yet
//----------------------------------------------------------------------------------------------------------------------
// Get the mapping value for a specific channel and map index.
uint8_t FDK_chMapDescr_getMapValue(const FDK_channelMapDescr_t* const pMapDescr, const uint8_t chIdx,
                                   const uint32_t mapIdx) {
    uint8_t mapValue = chIdx; /* Pass through by default. */

    assert(pMapDescr != NULL);

    if((pMapDescr->fPassThrough == 0) && (pMapDescr->pMapInfoTab != NULL) && (pMapDescr->mapInfoTabLen > mapIdx)) {
        // Nest sanity check to avoid possible memory access violation.
        if(chIdx < pMapDescr->pMapInfoTab[mapIdx].numChannels) {
            mapValue = pMapDescr->pMapInfoTab[mapIdx].pChannelMap[chIdx];
        }
    }
    return mapValue;
}
//----------------------------------------------------------------------------------------------------------------------
/**
 * \brief  Evaluate whether single channel map is reasonable or not.
 *
 * \param  pMapInfo  Pointer to channel map.
 * \return           Value unequal to zero if map is valid, otherwise zero.
 */
static int32_t fdk_chMapDescr_isValidMap(const CHANNEL_MAP_INFO_t* const pMapInfo) {
    int32_t  result = 1;
    uint32_t i;

    if(pMapInfo == NULL) { result = 0; }
    else {
        uint32_t numChannels = pMapInfo->numChannels;

        /* Check for all map values if they are inside the range 0 to numChannels-1
         * and unique. */
        if(numChannels < 32) { /* Optimized version for less than 32 channels. Needs only one loop. */
            uint32_t mappedChMask = 0x0;
            for(i = 0; i < numChannels; i += 1) { mappedChMask |= 1 << pMapInfo->pChannelMap[i]; }
            if(mappedChMask != (((uint32_t)1 << numChannels) - 1)) { result = 0; }
        }
        else { /* General case that can handle all number of channels but needs one more loop. */
            for(i = 0; (i < numChannels) && result; i += 1) {
                uint32_t j;
                uint8_t  value0 = pMapInfo->pChannelMap[i];

                if(value0 > numChannels - 1) { /* out of range? */
                    result = 0;
                }
                for(j = numChannels - 1; (j > i) && result; j -= 1) {
                    if(value0 == pMapInfo->pChannelMap[j]) { /* not unique */
                        result = 0;
                    }
                }
            }
        }
    }
    return result;
}
//----------------------------------------------------------------------------------------------------------------------
 // Evaluate whether channel map descriptor is reasonable or not.
int32_t FDK_chMapDescr_isValid(const FDK_channelMapDescr_t* const pMapDescr) {
    int32_t  result = 0;
    uint32_t i;

    if(pMapDescr != NULL) {
        result = 1;
        for(i = 0; (i < pMapDescr->mapInfoTabLen) && result; i += 1) {
            if(!fdk_chMapDescr_isValidMap(&pMapDescr->pMapInfoTab[i])) { result = 0; }
        }
    }
    return result;
}
//----------------------------------------------------------------------------------------------------------------------
// Initialize the complete channel map descriptor.
void FDK_chMapDescr_init(FDK_channelMapDescr_t* const pMapDescr, const CHANNEL_MAP_INFO_t* const pMapInfoTab,
                         const uint32_t mapInfoTabLen, const uint32_t fPassThrough) {
    if(pMapDescr != NULL) {
        int32_t useDefaultTab = 1;

        pMapDescr->fPassThrough = (fPassThrough == 0) ? 0 : 1;

        if((pMapInfoTab != NULL) && (mapInfoTabLen > 0)) {
            /* Set the valid custom mapping table. */
            pMapDescr->pMapInfoTab = pMapInfoTab;
            pMapDescr->mapInfoTabLen = mapInfoTabLen;
            /* Validate the complete descriptor. */
            useDefaultTab = (FDK_chMapDescr_isValid(pMapDescr) == 0) ? 1 : 0;
        }
        if(useDefaultTab != 0) {
            /* Set default table. */
            pMapDescr->pMapInfoTab = mapInfoTabDflt;
            pMapDescr->mapInfoTabLen = DFLT_CH_MAP_TAB_LEN;
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
// Set channel mapping bypass flag in a given channel map descriptor.
int32_t FDK_chMapDescr_setPassThrough(FDK_channelMapDescr_t* const pMapDescr, uint32_t fPassThrough) {
    int32_t err = 1;

    if(pMapDescr != NULL) {
        if((pMapDescr->pMapInfoTab != NULL) && (pMapDescr->mapInfoTabLen > 0)) {
            pMapDescr->fPassThrough = (fPassThrough == 0) ? 0 : 1;
            err = 0;
        }
    }

    return err;
}
//----------------------------------------------------------------------------------------------------------------------
