

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

/** Channel Mode ( 1-7 equals MPEG channel configurations, others are
 * arbitrary). */
typedef enum
{
    MODE_INVALID = -1,
    MODE_UNKNOWN = 0,
    MODE_1 = 1,         /**< C */
    MODE_2 = 2,         /**< L+R */
    MODE_1_2 = 3,       /**< C, L+R */
    MODE_1_2_1 = 4,     /**< C, L+R, Rear */
    MODE_1_2_2 = 5,     /**< C, L+R, LS+RS */
    MODE_1_2_2_1 = 6,   /**< C, L+R, LS+RS, LFE */
    MODE_1_2_2_2_1 = 7, /**< C, LC+RC, L+R, LS+RS, LFE */

    MODE_6_1 = 11,           /**< C, L+R, LS+RS, Crear, LFE */
    MODE_7_1_BACK = 12,      /**< C, L+R, LS+RS, Lrear+Rrear, LFE */
    MODE_7_1_TOP_FRONT = 14, /**< C, L+R, LS+RS, LFE, Ltop+Rtop */

    MODE_7_1_REAR_SURROUND = 33, /**< C, L+R, LS+RS, Lrear+Rrear, LFE */
    MODE_7_1_FRONT_CENTER = 34,  /**< C, LC+RC, L+R, LS+RS, LFE */

    MODE_212 = 128 /**< 212 configuration, used in ELDv2 */

} CHANNEL_MODE;

/**
 * Speaker description tags.
 * Do not change the enumeration values unless it keeps the following
 * segmentation:
 * - Bit 0-3: Horizontal postion (0: none, 1: front, 2: side, 3: back, 4: lfe)
 * - Bit 4-7: Vertical position (0: normal, 1: top, 2: bottom)
 */
typedef enum
{
    ACT_NONE = 0x00,
    ACT_FRONT = 0x01, /*!< Front speaker position (at normal height) */
    ACT_SIDE = 0x02,  /*!< Side speaker position (at normal height) */
    ACT_BACK = 0x03,  /*!< Back speaker position (at normal height) */
    ACT_LFE = 0x04,   /*!< Low frequency effect speaker postion (front) */

    ACT_TOP = 0x10,       /*!< Top speaker area (for combination with speaker positions) */
    ACT_FRONT_TOP = 0x11, /*!< Top front speaker = (ACT_FRONT|ACT_TOP) */
    ACT_SIDE_TOP = 0x12,  /*!< Top side speaker  = (ACT_SIDE |ACT_TOP) */
    ACT_BACK_TOP = 0x13,  /*!< Top back speaker  = (ACT_BACK |ACT_TOP) */

    ACT_BOTTOM = 0x20,       /*!< Bottom speaker area (for combination with speaker positions) */
    ACT_FRONT_BOTTOM = 0x21, /*!< Bottom front speaker = (ACT_FRONT|ACT_BOTTOM) */
    ACT_SIDE_BOTTOM = 0x22,  /*!< Bottom side speaker  = (ACT_SIDE |ACT_BOTTOM) */
    ACT_BACK_BOTTOM = 0x23   /*!< Bottom back speaker  = (ACT_BACK |ACT_BOTTOM) */

} AUDIO_CHANNEL_TYPE;

typedef enum
{
    SIG_UNKNOWN = -1,
    SIG_IMPLICIT = 0,
    SIG_EXPLICIT_BW_COMPATIBLE = 1,
    SIG_EXPLICIT_HIERARCHICAL = 2

} SBR_PS_SIGNALING;

/**
 * Audio Codec flags.
 */
#define AC_ER_VCB11                                                             \
    0x000001 /*!< aacSectionDataResilienceFlag     flag (from ASC): 1 means use \
                virtual codebooks  */
#define AC_ER_RVLC                                                               \
    0x000002 /*!< aacSpectralDataResilienceFlag     flag (from ASC): 1 means use \
                huffman codeword reordering */
#define AC_ER_HCR                                                                                  \
    0x000004                    /*!< aacSectionDataResilienceFlag     flag (from ASC): 1 means use \
                                   virtual codebooks  */
#define AC_SCALABLE    0x000008 /*!< AAC Scalable*/
#define AC_ELD         0x000010 /*!< AAC-ELD */
#define AC_LD          0x000020 /*!< AAC-LD */
#define AC_ER          0x000040 /*!< ER syntax */
#define AC_BSAC        0x000080 /*!< BSAC */
#define AC_USAC        0x000100 /*!< USAC */
#define AC_RSV603DA    0x000200 /*!< RSVD60 3D audio */
#define AC_HDAAC       0x000400 /*!< HD-AAC */
#define AC_RSVD50      0x004000 /*!< Rsvd50 */
#define AC_SBR_PRESENT 0x008000 /*!< SBR present flag (from ASC) */
#define AC_SBRCRC      0x010000 /*!< SBR CRC present flag. Only relevant for AAC-ELD for now. */
#define AC_PS_PRESENT  0x020000 /*!< PS present flag (from ASC or implicit)  */
#define AC_MPS_PRESENT                                                          \
    0x040000                       /*!< MPS present flag (from ASC or implicit) \
                                    */
#define AC_DRM           0x080000  /*!< DRM bit stream syntax */
#define AC_INDEP         0x100000  /*!< Independency flag */
#define AC_MPEGD_RES     0x200000  /*!< MPEG-D residual individual channel data. */
#define AC_SAOC_PRESENT  0x400000  /*!< SAOC Present Flag */
#define AC_DAB           0x800000  /*!< DAB bit stream syntax */
#define AC_ELD_DOWNSCALE 0x1000000 /*!< ELD Downscaled playout */
#define AC_LD_MPS        0x2000000 /*!< Low Delay MPS. */
#define AC_DRC_PRESENT                                                        \
    0x4000000                    /*!< Dynamic Range Control (DRC) data found. \
                                  */
#define AC_USAC_SCFGI3 0x8000000 /*!< USAC flag: If stereoConfigIndex is 3 the flag is set. */
/**
 * Audio Codec flags (reconfiguration).
 */
#define AC_CM_DET_CFG_CHANGE                                                   \
    0x000001 /*!< Config mode signalizes the callback to work in config change \
                detection mode */
#define AC_CM_ALLOC_MEM                                                 \
    0x000002 /*!< Config mode signalizes the callback to work in memory \
                allocation mode */

/**
 * Audio Codec flags (element specific).
 */
#define AC_EL_USAC_TW     0x000001 /*!< USAC time warped filter bank is active */
#define AC_EL_USAC_NOISE  0x000002 /*!< USAC noise filling is active */
#define AC_EL_USAC_ITES   0x000004 /*!< USAC SBR inter-TES tool is active */
#define AC_EL_USAC_PVC    0x000008 /*!< USAC SBR predictive vector coding tool is active */
#define AC_EL_USAC_MPS212 0x000010 /*!< USAC MPS212 tool is active */
#define AC_EL_USAC_LFE    0x000020 /*!< USAC element is LFE */
#define AC_EL_USAC_CP_POSSIBLE                                                                              \
    0x000040                            /*!< USAC may use Complex Stereo Prediction in this channel element \
                                         */
#define AC_EL_ENHANCED_NOISE   0x000080 /*!< Enhanced noise filling*/
#define AC_EL_IGF_AFTER_TNS    0x000100 /*!< IGF after TNS */
#define AC_EL_IGF_INDEP_TILING 0x000200 /*!< IGF independent tiling */
#define AC_EL_IGF_USE_ENF      0x000400 /*!< IGF use enhanced noise filling */
#define AC_EL_FULLBANDLPD      0x000800 /*!< enable fullband LPD tools */
#define AC_EL_LPDSTEREOIDX     0x001000 /*!< LPD-stereo-tool stereo index */
#define AC_EL_LFE              0x002000 /*!< The element is of type LFE. */

/* CODER_CONFIG::flags */
#define CC_MPEG_ID         0x00100000
#define CC_IS_BASELAYER    0x00200000
#define CC_PROTECTION      0x00400000
#define CC_SBR             0x00800000
#define CC_SBRCRC          0x00010000
#define CC_SAC             0x00020000
#define CC_RVLC            0x01000000
#define CC_VCB11           0x02000000
#define CC_HCR             0x04000000
#define CC_PSEUDO_SURROUND 0x08000000
#define CC_USAC_NOISE      0x10000000
#define CC_USAC_TW         0x20000000
#define CC_USAC_HBE        0x40000000

/** Generic audio coder configuration structure. */
typedef struct
{
    AUDIO_OBJECT_TYPE_t aot;               /**< Audio Object Type (AOT).           */
    AUDIO_OBJECT_TYPE_t extAOT;            /**< Extension Audio Object Type (SBR). */
    CHANNEL_MODE      channelMode;       /**< Channel mode.                      */
    uint8_t           channelConfigZero; /**< Use channel config zero + pce although a
                                          standard channel config could be signaled. */
    int32_t samplingRate;                /**< Sampling rate.                     */
    int32_t extSamplingRate;             /**< Extended samplerate (SBR).         */
    int32_t downscaleSamplingRate;       /**< Downscale sampling rate (ELD downscaled mode)
                                          */
    int32_t bitRate;                     /**< Average bitrate.                   */
    int32_t samplesPerFrame;             /**< Number of PCM samples per codec frame and audio
                                        channel. */
    int32_t noChannels;                  /**< Number of audio channels.          */
    int32_t bitsFrame;
    int32_t nSubFrames;        /**< Amount of encoder subframes. 1 means no subframing. */
    int32_t BSACnumOfSubFrame; /**< The number of the sub-frames which are grouped and
                              transmitted in a super-frame (BSAC). */
    int32_t BSAClayerLength;   /**< The average length of the large-step layers in bytes
                              (BSAC).                            */
    uint32_t flags;            /**< flags */
    uint8_t  matrixMixdownA;   /**< Matrix mixdown index to put into PCE. Default value
                                0 means no mixdown coefficient, valid values are 1-4
                                which correspond to matrix_mixdown_idx 0-3. */
    uint8_t headerPeriod;      /**< Frame period for sending in band configuration
                                buffers in the transport layer. */

    uint8_t          stereoConfigIndex; /**< USAC MPS stereo mode */
    uint8_t          sbrMode;           /**< USAC SBR mode */
    SBR_PS_SIGNALING sbrSignaling;      /**< 0: implicit signaling, 1: backwards
                                           compatible explicit signaling, 2:
                                           hierarcical explicit signaling */

    uint8_t rawConfig[64]; /**< raw codec specific config as bit stream */
    int32_t rawConfigBits; /**< Size of rawConfig in bits */

    uint8_t sbrPresent;
    uint8_t psPresent;
} CODER_CONFIG;

#define USAC_ID_BIT 16 /** USAC element IDs start at USAC_ID_BIT */

/** MP4 Element IDs. */
typedef enum
{
    /* mp4 element IDs */
    ID_NONE = -1, /**< Invalid Element helper ID.             */
    ID_SCE = 0,   /**< Single Channel Element.                */
    ID_CPE = 1,   /**< Channel Pair Element.                  */
    ID_CCE = 2,   /**< Coupling Channel Element.              */
    ID_LFE = 3,   /**< LFE Channel Element.                   */
    ID_DSE = 4,   /**< Currently one Data Stream Element for ancillary data is
                     supported. */
    ID_PCE = 5,   /**< Program Config Element.                */
    ID_FIL = 6,   /**< Fill Element.                          */
    ID_END = 7,   /**< Arnie (End Element = Terminator).      */
    ID_EXT = 8,   /**< Extension Payload (ER only).           */
    ID_SCAL = 9,  /**< AAC scalable element (ER only).        */
    /* USAC element IDs */
    ID_USAC_SCE = 0 + USAC_ID_BIT, /**< Single Channel Element.                */
    ID_USAC_CPE = 1 + USAC_ID_BIT, /**< Channel Pair Element.                  */
    ID_USAC_LFE = 2 + USAC_ID_BIT, /**< LFE Channel Element.                   */
    ID_USAC_EXT = 3 + USAC_ID_BIT, /**< Extension Element.                     */
    ID_USAC_END = 4 + USAC_ID_BIT, /**< Arnie (End Element = Terminator).      */
    ID_LAST
} MP4_ELEMENT_ID;

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

/** Extension payload types. */
typedef enum
{
    EXT_FIL = 0x00,
    EXT_FILL_DATA = 0x01,
    EXT_DATA_ELEMENT = 0x02,
    EXT_DATA_LENGTH = 0x03,
    EXT_UNI_DRC = 0x04,
    EXT_LDSAC_DATA = 0x09,
    EXT_SAOC_DATA = 0x0a,
    EXT_DYNAMIC_RANGE = 0x0b,
    EXT_SAC_DATA = 0x0c,
    EXT_SBR_DATA = 0x0d,
    EXT_SBR_DATA_CRC = 0x0e
} EXT_PAYLOAD_TYPE;

#define IS_USAC_CHANNEL_ELEMENT(elementId) \
    ((elementId) == ID_USAC_SCE || (elementId) == ID_USAC_CPE || (elementId) == ID_USAC_LFE)

/** MPEG-D USAC & RSVD60 3D audio Extension Element Types. */
typedef enum
{
    /* usac */
    ID_EXT_ELE_FILL = 0x00,
    ID_EXT_ELE_MPEGS = 0x01,
    ID_EXT_ELE_SAOC = 0x02,
    ID_EXT_ELE_AUDIOPREROLL = 0x03,
    ID_EXT_ELE_UNI_DRC = 0x04,
    /* rsv603da */
    ID_EXT_ELE_OBJ_METADATA = 0x05,
    ID_EXT_ELE_SAOC_3D = 0x06,
    ID_EXT_ELE_HOA = 0x07,
    ID_EXT_ELE_FMT_CNVRTR = 0x08,
    ID_EXT_ELE_MCT = 0x09,
    ID_EXT_ELE_ENHANCED_OBJ_METADATA = 0x0d,
    /* reserved for use outside of ISO scope */
    ID_EXT_ELE_VR_METADATA = 0x81,
    ID_EXT_ELE_UNKNOWN = 0xFF
} USAC_EXT_ELEMENT_TYPE;

/**
 * Proprietary raw packet file configuration data type identifier.
 */
typedef enum
{
    TC_NOTHING = 0,       /* No configuration available -> in-band configuration.   */
    TC_RAW_ADTS = 2,      /* Transfer type is ADTS. */
    TC_RAW_LATM_MCP1 = 6, /* Transfer type is LATM with SMC present.    */
    TC_RAW_SDC = 21       /* Configuration data field is Drm SDC.             */

} TP_CONFIG_TYPE;

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
