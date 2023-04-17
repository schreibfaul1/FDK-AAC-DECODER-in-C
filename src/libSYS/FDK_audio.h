

/************************* System integration library **************************

   Author(s):   Manuel Jander

   Description:

*******************************************************************************/

/** \file   FDK_audio.h
 *  \brief  Global audio struct and constant definitions.
 */

#ifndef FDK_AUDIO_H
#define FDK_AUDIO_H

#include "../libAACdec/newAACDecoder.h"

#ifdef __cplusplus
extern "C" {
#endif



#define TT_IS_PACKET(x) \
    (((x) == TT_MP4_RAW) || ((x) == TT_DRM) || ((x) == TT_MP4_LATM_MCP0) || ((x) == TT_MP4_LATM_MCP1))



#define CAN_DO_PS(aot) \
    ((aot) == AOT_AAC_LC || (aot) == AOT_SBR || (aot) == AOT_PS || (aot) == AOT_ER_BSAC || (aot) == AOT_DRM_AAC)

#define IS_USAC(aot) ((aot) == AOT_USAC)

#define IS_LOWDELAY(aot) ((aot) == AOT_ER_AAC_LD || (aot) == AOT_ER_AAC_ELD)














/* usacConfigExtType q.v. ISO/IEC DIS 23008-3 Table 52  and  ISO/IEC FDIS
 * 23003-3:2011(E) Table 74*/
typedef enum
{
    /* USAC and RSVD60 3DA */
    ID_CONFIG_EXT_FILL = 0,
    /* RSVD60 3DA */
    ID_CONFIG_EXT_DOWNMIX = 1,
    ID_CONFIG_EXT_LOUDNESS_INFO = 2,
    ID_CONFIG_EXT_AUDIOSCENE_INFO = 3,
    ID_CONFIG_EXT_HOA_MATRIX = 4,
    ID_CONFIG_EXT_SIG_GROUP_INFO = 6
    /* 5-127 => reserved for ISO use */
    /* > 128 => reserved for use outside of ISO scope */
} CONFIG_EXT_ID;

#define IS_CHANNEL_ELEMENT(elementId)                                                                         \
    ((elementId) == ID_SCE || (elementId) == ID_CPE || (elementId) == ID_LFE || (elementId) == ID_USAC_SCE || \
     (elementId) == ID_USAC_CPE || (elementId) == ID_USAC_LFE)

#define IS_MP4_CHANNEL_ELEMENT(elementId) ((elementId) == ID_SCE || (elementId) == ID_CPE || (elementId) == ID_LFE)

#define EXT_ID_BITS 4 /**< Size in bits of extension payload type tags. */



#define IS_USAC_CHANNEL_ELEMENT(elementId) \
    ((elementId) == ID_USAC_SCE || (elementId) == ID_USAC_CPE || (elementId) == ID_USAC_LFE)




/*
 * ##############################################################################################
 * Library identification and error handling
 * ##############################################################################################
 */
/* \cond */

typedef enum
{
    FDK_NONE = 0,
    FDK_TOOLS = 1,
    FDK_SYSLIB = 2,
    FDK_AACDEC = 3,
    FDK_AACENC = 4,
    FDK_SBRDEC = 5,
    FDK_SBRENC = 6,
    FDK_TPDEC = 7,
    FDK_TPENC = 8,
    FDK_MPSDEC = 9,
    FDK_MPEGFILEREAD = 10,
    FDK_MPEGFILEWRITE = 11,
    FDK_PCMDMX = 31,
    FDK_MPSENC = 34,
    FDK_TDLIMIT = 35,
    FDK_UNIDRCDEC = 38,

    FDK_MODULE_LAST

} FDK_MODULE_ID;



typedef struct LIB_INFO_t
{
    const char*   title;
    const char*   build_date;
    const char*   build_time;
    FDK_MODULE_ID module_id;
    int32_t       version;
    uint32_t      flags;
    char          versionStr[32];
} LIB_INFO_t;
//----------------------------------------------------------------------------------------------------------------------
/** Initialize library info. */
static inline void FDKinitLibInfo(LIB_INFO_t* info) {
    int32_t i;
    for(i = 0; i < FDK_MODULE_LAST; i++) { info[i].module_id = FDK_NONE; }
}

/** Aquire supported features of library. */
static inline uint32_t FDKlibInfo_getCapabilities(const LIB_INFO_t* info, FDK_MODULE_ID module_id) {
    int32_t i;
    for(i = 0; i < FDK_MODULE_LAST; i++) {
        if(info[i].module_id == module_id) { return info[i].flags; }
    }
    return 0;
}

/** Search for next free tab. */
static inline int32_t FDKlibInfo_lookup(const LIB_INFO_t* info, FDK_MODULE_ID module_id) {
    int32_t i = -1;
    for(i = 0; i < FDK_MODULE_LAST; i++) {
        if(info[i].module_id == module_id) return -1;
        if(info[i].module_id == FDK_NONE) break;
    }
    if(i == FDK_MODULE_LAST) return -1;
    return i;
}
//----------------------------------------------------------------------------------------------------------------------



#ifdef __cplusplus
}
#endif

#endif /* FDK_AUDIO_H */
