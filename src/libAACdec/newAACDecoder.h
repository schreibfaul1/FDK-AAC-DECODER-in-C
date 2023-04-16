#pragma once

#include <stdint.h>


typedef struct {                // Contains information needed for a single channel map.
    const uint8_t* pChannelMap; // Actual channel mapping for one single configuration.
    uint8_t        numChannels; //The number of channels for the channel map which is the maximum used channel index+1.
} CHANNEL_MAP_INFO_t;

typedef struct {                             // This is the main data struct for all channel configurations
    const CHANNEL_MAP_INFO_t* pMapInfoTab;   // Table of channel maps.
    uint32_t                  mapInfoTabLen; // Length of the channel map table array.
    uint32_t                  fPassThrough;  /*!< Flag that defines whether the specified mapping shall be applied
                                             (value: 0) or the input just gets passed through (MPEG mapping). */
} FDK_channelMapDescr_t;








// prototypes
// syslib_channelMapDescr
uint8_t FDK_chMapDescr_getMapValue(const FDK_channelMapDescr_t* const pMapDescr, const uint8_t chIdx, const uint32_t mapIdx);
int32_t FDK_chMapDescr_setPassThrough(FDK_channelMapDescr_t* const pMapDescr, uint32_t fPassThrough);
int32_t FDK_chMapDescr_isValid(const FDK_channelMapDescr_t* const pMapDescr);
void FDK_chMapDescr_init(FDK_channelMapDescr_t* const pMapDescr, const CHANNEL_MAP_INFO_t* const pMapInfoTab, const uint32_t mapInfoTabLen, const uint32_t fPassThrough);


