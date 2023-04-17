#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>


#define WAV_BITS          	16
#define SAMPLE_BITS       	16
#define SAMPLE_MAX        	((int16_t)(((uint32_t)1 << (SAMPLE_BITS - 1)) - 1))
#define SAMPLE_MIN        	(~SAMPLE_MAX)
#define ALIGNMENT_DEFAULT 	8
#define M_PI              	3.14159265358979323846 /*!< Pi. Only used in example projects. */
#define DFLT_CH_MAP_TAB_LEN 15 /* Length of the default channel map info table. */

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

typedef enum{
	/* Internal */                //Identifiers for various memory locations. They are used along with memory
	SECT_DATA_L1 = 0x2000,        // allocation functions like FDKcalloc_L() to specify the requested memory's location.
	SECT_DATA_L2,
	SECT_DATA_L1_A,
	SECT_DATA_L1_B,
	SECT_CONSTDATA_L1,
	/* External */
	SECT_DATA_EXTERN = 0x4000,
	SECT_CONSTDATA_EXTERN
} MEMORY_SECTION_t;






// prototypes

uint8_t FDK_chMapDescr_getMapValue(const FDK_channelMapDescr_t* const pMapDescr, const uint8_t chIdx, const uint32_t mapIdx);
int32_t FDK_chMapDescr_setPassThrough(FDK_channelMapDescr_t* const pMapDescr, uint32_t fPassThrough);
int32_t FDK_chMapDescr_isValid(const FDK_channelMapDescr_t* const pMapDescr);
void FDK_chMapDescr_init(FDK_channelMapDescr_t* const pMapDescr, const CHANNEL_MAP_INFO_t* const pMapInfoTab, const uint32_t mapInfoTabLen, const uint32_t fPassThrough);

void *FDKcalloc(const uint32_t n, const uint32_t size);
void *FDKmalloc(const uint32_t size);
void *FDKaalloc(const uint32_t size, const uint32_t alignment);
void FDKafree(void *ptr);
void *FDKcalloc_L(const uint32_t n, const uint32_t size, MEMORY_SECTION_t s);
void *FDKaalloc_L(const uint32_t size, const uint32_t alignment, MEMORY_SECTION_t s);
void FDKfree_L(void *ptr);
void FDKafree_L(void *ptr);
