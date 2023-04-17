#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "newAACDecoder.h"

#define WAV_BITS          	   16
#define SAMPLE_BITS       	   16
#define SAMPLE_MAX        	   ((int16_t)(((uint32_t)1 << (SAMPLE_BITS - 1)) - 1))
#define SAMPLE_MIN        	   (~SAMPLE_MAX)
#define ALIGNMENT_DEFAULT 	   8
#define M_PI              	   3.14159265358979323846 // Pi. Only used in example projects.
#define DFLT_CH_MAP_TAB_LEN    15          // Length of the default channel map info table.
#define USAC_ID_BIT            16 /** USAC element IDs start at USAC_ID_BIT */

/* AAC capability flags */
#define CAPF_AAC_LC            0x00000001  // Support flag for AAC Low Complexity.
#define CAPF_ER_AAC_LD         0x00000002  // Support flag for AAC Low Delay with Error Resilience tools.
#define CAPF_ER_AAC_SCAL       0x00000004  // Support flag for AAC Scalable.
#define CAPF_ER_AAC_LC         0x00000008  // Support flag for AAC Low Complexity with Error Resilience tools.
#define CAPF_AAC_480           0x00000010  // Support flag for AAC with 480 framelength.
#define CAPF_AAC_512           0x00000020  // Support flag for AAC with 512 framelength.
#define CAPF_AAC_960           0x00000040  // Support flag for AAC with 960 framelength.
#define CAPF_AAC_1024          0x00000080  // Support flag for AAC with 1024 framelength.
#define CAPF_AAC_HCR           0x00000100  // Support flag for AAC with Huffman Codeword Reordering.
#define CAPF_AAC_VCB11         0x00000200  // Support flag for AAC Virtual Codebook 11.
#define CAPF_AAC_RVLC          0x00000400  // Support flag for AAC Reversible Variable Length Coding.
#define CAPF_AAC_MPEG4         0x00000800  // Support flag for MPEG file format.
#define CAPF_AAC_DRC           0x00001000  // Support flag for AAC Dynamic Range Control.
#define CAPF_AAC_CONCEALMENT   0x00002000  // Support flag for AAC concealment.
#define CAPF_AAC_DRM_BSFORMAT  0x00004000  // Support flag for AAC DRM bistream format.
#define CAPF_ER_AAC_ELD        0x00008000  // Support flag for AAC Enhanced Low Delay with Error Resilience tools.
#define CAPF_ER_AAC_BSAC       0x00010000  // Support flag for AAC BSAC.
#define CAPF_AAC_ELD_DOWNSCALE 0x00040000  // Support flag for AAC-ELD Downscaling
#define CAPF_AAC_USAC_LP       0x00100000  // Support flag for USAC low power mode.
#define CAPF_AAC_USAC          0x00200000  // Support flag for Unified Speech and Audio Coding (USAC).
#define CAPF_ER_AAC_ELDV2      0x00800000  // Support flag for AAC Enhanced Low Delay with MPS 212.
#define CAPF_AAC_UNIDRC        0x01000000  // Support flag for MPEG-D Dynamic Range Control (uniDrc).
/* Transport capability flags */
#define CAPF_ADTS       0x00000001  // Support flag for ADTS transport format.
#define CAPF_ADIF       0x00000002  // Support flag for ADIF transport format.
#define CAPF_LATM       0x00000004  // Support flag for LATM transport format.
#define CAPF_LOAS       0x00000008  // Support flag for LOAS transport format.
#define CAPF_RAWPACKETS 0x00000010  // Support flag for RAW PACKETS transport format.
#define CAPF_DRM        0x00000020  // Support flag for DRM/DRM+ transport format.
#define CAPF_RSVD50     0x00000040  // Support flag for RSVD50 transport format
/* SBR capability flags */
#define CAPF_SBR_LP            0x00000001  // Support flag for SBR Low Power mode.
#define CAPF_SBR_HQ            0x00000002  // Support flag for SBR High Quality mode.
#define CAPF_SBR_DRM_BS        0x00000004  // Support flag for
#define CAPF_SBR_CONCEALMENT   0x00000008  // Support flag for SBR concealment.
#define CAPF_SBR_DRC           0x00000010  // Support flag for SBR Dynamic Range Control.
#define CAPF_SBR_PS_MPEG       0x00000020  // Support flag for MPEG Parametric Stereo.
#define CAPF_SBR_PS_DRM        0x00000040  // Support flag for DRM Parametric Stereo.
#define CAPF_SBR_ELD_DOWNSCALE 0x00000080  // Support flag for ELD reduced delay mode
#define CAPF_SBR_HBEHQ         0x00000100  // Support flag for HQ HBE
/* PCM utils capability flags */
#define CAPF_DMX_BLIND  0x00000001  // Support flag for blind downmixing.
#define CAPF_DMX_PCE    0x00000002  // Support flag for guided downmix with data from MPEG-2/4 PCE
#define CAPF_DMX_ARIB   0x00000004  // PCE guided downmix with slightly different equations to fulfill ARIB standard.
#define CAPF_DMX_DVB    0x00000008  // Support flag for guided downmix with data from DVB ancillary  data fields.
#define CAPF_DMX_CH_EXP 0x00000010  // flag for simple upmixing by dublicating channels or  adding zero channels.
#define CAPF_DMX_6_CH   0x00000020
#define CAPF_DMX_8_CH   0x00000040
#define CAPF_DMX_24_CH  0x00000080
#define CAPF_LIMITER    0x00002000
/* MPEG Surround capability flags */
#define CAPF_MPS_STD    0x00000001
#define CAPF_MPS_LD     0x00000002
#define CAPF_MPS_USAC   0x00000004
#define CAPF_MPS_HQ     0x00000010
#define CAPF_MPS_LP     0x00000020
#define CAPF_MPS_1CH_IN 0x00001000
#define CAPF_MPS_2CH_IN 0x00002000
#define CAPF_MPS_6CH_IN 0x00004000

// Audio Codec flags.
#define AC_ER_VCB11      0x000001   // aacSectionDataResilienceFlag  flag (from ASC): 1 means use  virtual codebooks
#define AC_ER_RVLC       0x000002   // aacSpectralDataResilienceFlag (from ASC): 1 means use huffman codeword reordering
#define AC_ER_HCR        0x000004   // aacSectionDataResilienceFlag flag (from ASC): 1 means use  virtual codebooks
#define AC_SCALABLE      0x000008   // AAC Scalable
#define AC_ELD           0x000010   // AAC-ELD
#define AC_LD            0x000020   // AAC-LD
#define AC_ER            0x000040   // ER syntax
#define AC_BSAC          0x000080   // BSAC
#define AC_USAC          0x000100   // USAC
#define AC_RSV603DA      0x000200   // RSVD60 3D audio
#define AC_HDAAC         0x000400   // HD-AAC
#define AC_RSVD50        0x004000   // Rsvd50
#define AC_SBR_PRESENT   0x008000   // SBR present flag (from ASC)
#define AC_SBRCRC        0x010000   // SBR CRC present flag. Only relevant for AAC-ELD for now.
#define AC_PS_PRESENT    0x020000   // PS present flag (from ASC or implicit)
#define AC_MPS_PRESENT   0x040000   // MPS present flag (from ASC or implicit)
#define AC_DRM           0x080000   // DRM bit stream syntax
#define AC_INDEP         0x100000   // Independency flag
#define AC_MPEGD_RES     0x200000   // MPEG-D residual individual channel data.
#define AC_SAOC_PRESENT  0x400000   // SAOC Present Flag
#define AC_DAB           0x800000   // DAB bit stream syntax
#define AC_ELD_DOWNSCALE 0x1000000  // ELD Downscaled playout
#define AC_LD_MPS        0x2000000  // Low Delay MPS.
#define AC_DRC_PRESENT   0x4000000  // Dynamic Range Control (DRC) data found.
#define AC_USAC_SCFGI3   0x8000000  // USAC flag: If stereoConfigIndex is 3 the flag is set.
// Audio Codec flags (reconfiguration).
#define AC_CM_DET_CFG_CHANGE 0x000001  // Config mode signalizes the callback to work in config change  detection mode
#define AC_CM_ALLOC_MEM      0x000002  // Config mode signalizes the callback to work in memory allocation mode
// Audio Codec flags (element specific).
#define AC_EL_USAC_TW          0x000001  // USAC time warped filter bank is active
#define AC_EL_USAC_NOISE       0x000002  // USAC noise filling is active
#define AC_EL_USAC_ITES        0x000004  // USAC SBR inter-TES tool is active
#define AC_EL_USAC_PVC         0x000008  // USAC SBR predictive vector coding tool is active
#define AC_EL_USAC_MPS212      0x000010  // USAC MPS212 tool is active
#define AC_EL_USAC_LFE         0x000020  // USAC element is LFE
#define AC_EL_USAC_CP_POSSIBLE 0x000040  // USAC may use Complex Stereo Prediction in this channel element
#define AC_EL_ENHANCED_NOISE   0x000080  // Enhanced noise filling
#define AC_EL_IGF_AFTER_TNS    0x000100  // IGF after TNS
#define AC_EL_IGF_INDEP_TILING 0x000200  // IGF independent tiling
#define AC_EL_IGF_USE_ENF      0x000400  // IGF use enhanced noise filling
#define AC_EL_FULLBANDLPD      0x000800  // enable fullband LPD tools
#define AC_EL_LPDSTEREOIDX     0x001000  // LPD-stereo-tool stereo index
#define AC_EL_LFE              0x002000  // The element is of type LFE.
// CODER_CONFIG_t::flags
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

#define FX_CFG2FX_DBL
#define FX_CFG2FX_SGL    FX_DBL2FX_SGL
#define FL2FXCONST_CFG   FL2FXCONST_DBL
#define FX_DBL2FX_CFG(x) ((int32_t)(x))

#define CLIP_PROTECT_GAIN_0(x) FL2FXCONST_CFG(((x) / (float)(1 << 0)))
#define CLIP_PROTECT_GAIN_1(x) FL2FXCONST_CFG(((x) / (float)(1 << 1)))
#define CLIP_PROTECT_GAIN_2(x) FL2FXCONST_CFG(((x) / (float)(1 << 2)))
#define MAXVAL_SGL             ((signed)0x00007FFF) /* this has to be synchronized to FRACT_BITS */
#define MINVAL_SGL             ((signed)0xFFFF8000) /* this has to be synchronized to FRACT_BITS */
#define MAXVAL_DBL             ((signed)0x7FFFFFFF) /* this has to be synchronized to DFRACT_BITS */
#define MINVAL_DBL             ((signed)0x80000000) /* this has to be synchronized to DFRACT_BITS */

#define SF_CLD_C1C2 (9)
#define FRACT_FIX_SCALE  ((int64_t(1) << (FRACT_BITS - 1)))
#define DFRACT_FIX_SCALE ((int64_t(1) << (DFRACT_BITS - 1)))
#define IS_USAC(aot)     ((aot) == AOT_USAC)
#define IS_LOWDELAY(aot) ((aot) == AOT_ER_AAC_LD || (aot) == AOT_ER_AAC_ELD)
#define IS_CHANNEL_ELEMENT(elementId)                                                                         \
    ((elementId) == ID_SCE || (elementId) == ID_CPE || (elementId) == ID_LFE || (elementId) == ID_USAC_SCE || \
     (elementId) == ID_USAC_CPE || (elementId) == ID_USAC_LFE)
#define IS_MP4_CHANNEL_ELEMENT(elementId) ((elementId) == ID_SCE || (elementId) == ID_CPE || (elementId) == ID_LFE)
#define EXT_ID_BITS                       4 /**< Size in bits of extension payload type tags. */
#define IS_USAC_CHANNEL_ELEMENT(elementId) \
    ((elementId) == ID_USAC_SCE || (elementId) == ID_USAC_CPE || (elementId) == ID_USAC_LFE)
#define MAX_DRC_THREADS           ((8) + 1) /* Heavy compression value is handled just like MPEG DRC data */
#define MAX_DRC_BANDS             (16)      /* 2^LEN_DRC_BAND_INCR (LEN_DRC_BAND_INCR = 4) */
#define MAX_NOISE_ENVELOPES       2
#define MAX_NOISE_COEFFS          5
#define MAX_NUM_NOISE_VALUES      (MAX_NOISE_ENVELOPES * MAX_NOISE_COEFFS)
#define MAX_NUM_LIMITERS          12
#define MAX_ENVELOPES_LEGACY      5
#define MAX_ENVELOPES_USAC        8
#define MAX_ENVELOPES             MAX_ENVELOPES_USAC
#define MAX_FREQ_COEFFS_DUAL_RATE 48
#define MAX_FREQ_COEFFS_QUAD_RATE 56
#define MAX_FREQ_COEFFS           MAX_FREQ_COEFFS_QUAD_RATE
#define SF_PITCH_TRACK            6
#define SF_GAIN                   3
#define MIN_VAL                   FL2FXCONST_DBL(0.0f)
#define MAX_VAL                   (int32_t) MAXVAL_DBL
#define MAX_FREQ_COEFFS_FS44100   35
#define MAX_FREQ_COEFFS_FS48000   32
#define MAX_NUM_ENVELOPE_VALUES   (MAX_ENVELOPES * MAX_FREQ_COEFFS)
#define MAX_SFB_NRG_HEADROOM      (1)
#define MAX_VAL_NRG_HEADROOM      ((((int32_t)MAXVAL_DBL) >> MAX_SFB_NRG_HEADROOM))
#define FRACT_BITS                16 /* single precision */
#define DFRACT_BITS               32 /* double precision */
#define ACCU_BITS                 40 /* double precision plus overflow */

// Audio Object Type definitions.
typedef enum{
    AOT_NONE = -1,
    AOT_NULL_OBJECT = 0,
    AOT_AAC_MAIN = 1,          // Main profile
    AOT_AAC_LC = 2,            // Low Complexity object
    AOT_AAC_SSR = 3,
    AOT_AAC_LTP = 4,
    AOT_SBR = 5,
    AOT_AAC_SCAL = 6,
    AOT_TWIN_VQ = 7,
    AOT_CELP = 8,
    AOT_HVXC = 9,
    AOT_RSVD_10 = 10,          // (reserved)
    AOT_RSVD_11 = 11,          // (reserved)
    AOT_TTSI = 12,             // TTSI Object
    AOT_MAIN_SYNTH = 13,       // Main Synthetic object
    AOT_WAV_TAB_SYNTH = 14,    // Wavetable Synthesis object
    AOT_GEN_MIDI = 15,         // General MIDI object
    AOT_ALG_SYNTH_AUD_FX = 16, // Algorithmic Synthesis and Audio FX object
    AOT_ER_AAC_LC = 17,        // Error Resilient(ER) AAC Low Complexity
    AOT_RSVD_18 = 18,          // (reserved)
    AOT_ER_AAC_LTP = 19,       // Error Resilient(ER) AAC LTP object
    AOT_ER_AAC_SCAL = 20,      // Error Resilient(ER) AAC Scalable object
    AOT_ER_TWIN_VQ = 21,       // Error Resilient(ER) TwinVQ object
    AOT_ER_BSAC = 22,          // Error Resilient(ER) BSAC object
    AOT_ER_AAC_LD = 23,        // Error Resilient(ER) AAC LowDelay object
    AOT_ER_CELP = 24,          // Error Resilient(ER) CELP object
    AOT_ER_HVXC = 25,          // Error Resilient(ER) HVXC object
    AOT_ER_HILN = 26,          // Error Resilient(ER) HILN object
    AOT_ER_PARA = 27,          // Error Resilient(ER) Parametric object
    AOT_RSVD_28 = 28,          // might become SSC
    AOT_PS = 29,               // PS, Parametric Stereo (includes SBR)
    AOT_MPEGS = 30,            // MPEG Surround
    AOT_ESCAPE = 31,           // Signal AOT uses more than 5 bits
    AOT_MP3ONMP4_L1 = 32,      // MPEG-Layer1 in mp4
    AOT_MP3ONMP4_L2 = 33,      // MPEG-Layer2 in mp4
    AOT_MP3ONMP4_L3 = 34,      // MPEG-Layer3 in mp4
    AOT_RSVD_35 = 35,          // might become DST
    AOT_RSVD_36 = 36,          // might become ALS
    AOT_AAC_SLS = 37,          // AAC + SLS
    AOT_SLS = 38,              // SLS
    AOT_ER_AAC_ELD = 39,       // AAC Enhanced Low Delay
    AOT_USAC = 42,             // USAC
    AOT_SAOC = 43,             // SAOC
    AOT_LD_MPEGS = 44,         // Low Delay MPEG Surround
    /* Pseudo AOTs */
    AOT_MP2_AAC_LC = 129,      // Virtual AOT MP2 Low Complexity profile
    AOT_MP2_SBR = 132,         // Virtual AOT MP2 Low Complexity Profile with SBR
    AOT_DRM_AAC = 143,         // Virtual AOT for DRM (ER-AAC-SCAL without SBR)
    AOT_DRM_SBR = 144,         // Virtual AOT for DRM (ER-AAC-SCAL with SBR)
    AOT_DRM_MPEG_PS = 145,     // Virtual AOT for DRM (ER-AAC-SCAL with SBR and MPEG-PS)
    AOT_DRM_SURROUND = 146,    // Virtual AOT for DRM Surround (ER-AAC-SCAL (+SBR) +MPS)
    AOT_DRM_USAC = 147         // Virtual AOT for DRM with USAC
} AUDIO_OBJECT_TYPE_t;

// Transport type identifiers.
typedef enum{
    TT_UNKNOWN = -1,      // Unknown format.
    TT_MP4_RAW = 0,       // "as is" access units (packet based since there is obviously no sync layer)
    TT_MP4_ADIF = 1,      // ADIF bitstream format.
    TT_MP4_ADTS = 2,      // ADTS bitstream format.
    TT_MP4_LATM_MCP1 = 6, // Audio Mux Elements with muxConfigPresent = 1
    TT_MP4_LATM_MCP0 = 7, // Audio Mux Elements with muxConfigPresent = 0, out of band StreamMuxConfig
    TT_MP4_LOAS = 10,     // Audio Sync Stream.
    TT_DRM = 12           // Digital Radio Mondial (DRM30/DRM+) bitstream format.
} TRANSPORT_TYPE_t;

//Channel Mode ( 1-7 equals MPEG channel configurations, others are arbitrary).
typedef enum{
    MODE_INVALID = -1,
    MODE_UNKNOWN = 0,
    MODE_1 = 1,                  // C
    MODE_2 = 2,                  // L+R
    MODE_1_2 = 3,                // C, L+R
    MODE_1_2_1 = 4,              // C, L+R, Rear
    MODE_1_2_2 = 5,              // C, L+R, LS+RS
    MODE_1_2_2_1 = 6,            // C, L+R, LS+RS, LFE
    MODE_1_2_2_2_1 = 7,          // C, LC+RC, L+R, LS+RS, LFE
    MODE_6_1 = 11,               // C, L+R, LS+RS, Crear, LFE
    MODE_7_1_BACK = 12,          // C, L+R, LS+RS, Lrear+Rrear, LFE
    MODE_7_1_TOP_FRONT = 14,     // C, L+R, LS+RS, LFE, Ltop+Rtop
    MODE_7_1_REAR_SURROUND = 33, // C, L+R, LS+RS, Lrear+Rrear, LFE
    MODE_7_1_FRONT_CENTER = 34,  // C, LC+RC, L+R, LS+RS, LFE
    MODE_212 = 128               // 212 configuration, used in ELDv2
} CHANNEL_MODE;

 //Speaker description tags. Do not change the enumeration values unless it keeps the following segmentation:
 // - Bit 0-3: Horizontal postion (0: none, 1: front, 2: side, 3: back, 4: lfe)
 // - Bit 4-7: Vertical position (0: normal, 1: top, 2: bottom)
typedef enum{
    ACT_NONE = 0x00,
    ACT_FRONT = 0x01,        // Front speaker position (at normal height)
    ACT_SIDE = 0x02,         // Side speaker position (at normal height)
    ACT_BACK = 0x03,         // Back speaker position (at normal height)
    ACT_LFE = 0x04,          // Low frequency effect speaker postion (front)
    ACT_TOP = 0x10,          // Top speaker area (for combination with speaker positions)
    ACT_FRONT_TOP = 0x11,    // Top front speaker = (ACT_FRONT|ACT_TOP)
    ACT_SIDE_TOP = 0x12,     // Top side speaker  = (ACT_SIDE |ACT_TOP)
    ACT_BACK_TOP = 0x13,     // Top back speaker  = (ACT_BACK |ACT_TOP)
    ACT_BOTTOM = 0x20,       // Bottom speaker area (for combination with speaker positions)
    ACT_FRONT_BOTTOM = 0x21, // Bottom front speaker = (ACT_FRONT|ACT_BOTTOM)
    ACT_SIDE_BOTTOM = 0x22,  // Bottom side speaker  = (ACT_SIDE |ACT_BOTTOM)
    ACT_BACK_BOTTOM = 0x23   // Bottom back speaker  = (ACT_BACK |ACT_BOTTOM)
} AUDIO_CHANNEL_TYPE_t;

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

/** MP4 Element IDs. */
typedef enum{
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
} MP4_ELEMENT_ID_t;

/** MPEG-D USAC & RSVD60 3D audio Extension Element Types. */
typedef enum{
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
} USAC_EXT_ELEMENT_TYPE_t;

/** Extension payload types. */
typedef enum{
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
} EXT_PAYLOAD_TYPE_t;

typedef enum {
    SIG_UNKNOWN = -1,
    SIG_IMPLICIT = 0,
    SIG_EXPLICIT_BW_COMPATIBLE = 1,
    SIG_EXPLICIT_HIERARCHICAL = 2
} SBR_PS_SIGNALING_t;

typedef enum{
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

/* usacConfigExtType q.v. ISO/IEC DIS 23008-3 Table 52  and  ISO/IEC FDIS * 23003-3:2011(E) Table 74*/
typedef enum {
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
} CONFIG_EXT_ID_t;

typedef enum {
    DISABLED_PARAMETER_HANDLING = -1, /*!< DRC parameter handling disabled, all parameters are applied as requested. */
    ENABLED_PARAMETER_HANDLING = 0,   /*!< Apply changes to requested DRC parameters to prevent clipping */
    DRC_PRESENTATION_MODE_1 = 1,      /*!< DRC Presentation mode 1*/
    DRC_PRESENTATION_MODE_2 = 2       /*!< DRC Presentation mode 2*/
} AACDEC_DRC_PARAMETER_HANDLING_t;

typedef enum {
    EB_NIGHT = 0x0001,
    EB_NOISY = 0x0002,
    EB_LIMITED = 0x0004,
    EB_LOWLEVEL = 0x0008,
    EB_DIALOG = 0x0010,
    EB_GENERAL_COMPR = 0x0020,
    EB_EXPAND = 0x0040,
    EB_ARTISTIC = 0x0080,
    EB_CLIPPING = 0x0100,
    EB_FADE = 0x0200,
    EB_DUCK_OTHER = 0x0400,
    EB_DUCK_SELF = 0x0800
} EFFECT_BIT_t;

typedef enum { GCP_REGULAR = 0, GCP_FADING = 1, GCP_CLIPPING_DUCKING = 2, GCP_CONSTANT = 3 } GAIN_CODING_PROFILE_t;
typedef enum { GIT_SPLINE = 0, GIT_LINEAR = 1 } GAIN_INTERPOLATION_TYPE_t;
typedef enum { CS_LEFT = 0, CS_RIGHT = 1 } CHARACTERISTIC_SIDE_t;
typedef enum { CF_SIGMOID = 0, CF_NODES = 1 } CHARACTERISTIC_FORMAT_t;

typedef enum {
    GF_QMF32 = 0x1,
    GF_QMFHYBRID39 = 0x2,
    GF_QMF64 = 0x3,
    GF_QMFHYBRID71 = 0x4,
    GF_QMF128 = 0x5,
    GF_QMFHYBRID135 = 0x6,
    GF_UNIFORM = 0x7
} EQ_SUBBAND_GAIN_FORMAT_t;

//----------------------------------------------------------------------------------------------------------------------
/** Generic audio coder configuration structure. */
typedef struct{
    AUDIO_OBJECT_TYPE_t aot;             /**< Audio Object Type (AOT).           */
    AUDIO_OBJECT_TYPE_t extAOT;          /**< Extension Audio Object Type (SBR). */
    CHANNEL_MODE channelMode;            /**< Channel mode.                      */
    uint8_t channelConfigZero; // Use channel config zero + pce although a standard channel config could be signaled.
    int32_t samplingRate;                /**< Sampling rate.                     */
    int32_t extSamplingRate;             /**< Extended samplerate (SBR).         */
    int32_t downscaleSamplingRate;       /**< Downscale sampling rate (ELD downscaled mode)*/
    int32_t bitRate;                     /**< Average bitrate.                   */
    int32_t samplesPerFrame;             /**< Number of PCM samples per codec frame and audio channel. */
    int32_t noChannels;                  /**< Number of audio channels.          */
    int32_t bitsFrame;
    int32_t nSubFrames;        /**< Amount of encoder subframes. 1 means no subframing. */
    int32_t BSACnumOfSubFrame; /**< The number of the sub-frames which are grouped and
                              transmitted in a super-frame (BSAC). */
    int32_t BSAClayerLength;   // The average length of the large-step layers in bytes  (BSAC).
    uint32_t flags;            // flags
    uint8_t  matrixMixdownA;   // Matrix mixdown index to put into PCE. Default value
                               // 0 means no mixdown coefficient, valid values are 1-4
                               // which correspond to matrix_mixdown_idx 0-3.
    uint8_t headerPeriod;  // Frame period for sending in band configuration  buffers in the transport layer.
    uint8_t            stereoConfigIndex; // USAC MPS stereo mode
    uint8_t            sbrMode;           // USAC SBR mode
    SBR_PS_SIGNALING_t sbrSignaling;      // 0: implicit signaling,
                                          // 1: backwards compatible explicit signaling,
                                          // 2: hierarcical explicit signaling
    uint8_t            rawConfig[64];     // raw codec specific config as bit stream
    int32_t            rawConfigBits;     // Size of rawConfig in bits
    uint8_t            sbrPresent;
    uint8_t            psPresent;
} CODER_CONFIG_t;

typedef struct LIB_INFO{
    const char*   title;
    const char*   build_date;
    const char*   build_time;
    FDK_MODULE_ID module_id;
    int32_t       version;
    uint32_t      flags;
    char          versionStr[32];
} LIB_INFO_t;

typedef struct{
    uint32_t expiryCount;
    uint32_t numBands;
    uint16_t bandTop[MAX_DRC_BANDS];
    int16_t  drcInterpolationScheme;
    uint8_t  drcValue[MAX_DRC_BANDS];
    int8_t   drcDataType;
} CDrcChannelData_t;

typedef struct {
    uint32_t          excludedChnsMask;
    int8_t            progRefLevel;
    int8_t            presMode; /* Presentation mode: 0 (not indicated), 1, 2, and 3  (reserved). */
    int8_t            pceInstanceTag;
    CDrcChannelData_t channelData;
} CDrcPayload_t;

typedef struct {
    /* DRC parameters: Latest user requests */
    int32_t usrCut;
    int32_t usrBoost;
    uint8_t usrApplyHeavyCompression;
    // DRC parameters: Currently used, possibly changed by aacDecoder_drcParameterHandling
    int32_t                       cut;                   /* attenuation scale factor */
    int32_t                       boost;                 /* boost scale factor  */
    int8_t                        targetRefLevel;        /* target reference level for loudness normalization */
    uint8_t                       applyHeavyCompression; /* heavy compression (DVB) flag */
    uint32_t                      expiryFrame;
    uint8_t                       bsDelayEnable;
    AACDEC_DRC_PARAMETER_HANDLING_t defaultPresentationMode;
    uint8_t                       encoderTargetLevel;
} CDrcParams_t;

typedef struct {
    CDrcParams_t params;              /* Module parameters that can be set by user (via SetParam API function) */
    uint8_t      enable;              /* Switch that controls dynamic range processing */
    uint8_t      digitalNorm;         /* Switch to en-/disable reference level normalization in  digital domain */
    uint8_t      update;              // Flag indicating the change of a user or bitstream parameter
    int32_t      numOutChannels;      /* Number of output channels */
    int32_t      prevAacNumChannels;  /* Previous number of channels of aac bitstream, used for update flag */
    uint16_t     numPayloads;         /* The number of DRC data payload elements found within frame */
    uint16_t     numThreads;          /* The number of DRC data threads extracted from the found payload elements */
    int8_t       progRefLevel;        /* Program reference level for all channels */
    uint8_t      progRefLevelPresent; /* Program reference level found in bitstream */
    uint32_t     prlExpiryCount; /* Counter that can be used to monitor the life time of the program reference level. */
    int8_t       presMode;       /* Presentation mode as defined in ETSI TS 101 154 */
    uint8_t      dvbAncDataAvailable; /* Flag that indicates whether DVB ancillary data is present or not */
    uint32_t dvbAncDataPosition; /* Used to store the DVB ancillary data payload position in the bitstream (only one per
                                    frame) */
    uint32_t drcPayloadPosition[MAX_DRC_THREADS]; /* Used to store the DRC payload positions in the bitstream */
    uint8_t  uniDrcPrecedence;   /* Flag for signalling that uniDrc is active and takes precedence over legacy DRC */
    uint8_t  applyExtGain;       /* Flag is 1 if extGain has to be applied, otherwise 0. */
    int32_t  additionalGainPrev; /* Gain of previous frame to be applied to the  time data */
    int32_t  additionalGainFilterState;  /* Filter state for the gain smoothing */
    int32_t  additionalGainFilterState1; /* Filter state for the gain smoothing */
} CDrcInfo_t;

typedef struct{
    int32_t nrgRef[MAX_FREQ_COEFFS];
    int32_t nrgEst[MAX_FREQ_COEFFS];
    int32_t nrgGain[MAX_FREQ_COEFFS];
    int32_t noiseLevel[MAX_FREQ_COEFFS];
    int32_t nrgSine[MAX_FREQ_COEFFS];
    int8_t nrgRef_e[MAX_FREQ_COEFFS];
    int8_t nrgEst_e[MAX_FREQ_COEFFS];
    int8_t nrgGain_e[MAX_FREQ_COEFFS];
    int8_t noiseLevel_e[MAX_FREQ_COEFFS];
    int8_t nrgSine_e[MAX_FREQ_COEFFS];
    /* yet another exponent [0]: for ts < no_cols; [1]: for ts >= no_cols */
    int8_t exponent[2];
} ENV_CALC_NRGS_t;

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

//----------------------------------------------------------------------------------------------------------------------

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
static void equalizeFiltBufferExp(int32_t *filtBuffer, int8_t *filtBuffer_e, int32_t *NrgGain, int8_t *NrgGain_e,
                                  int32_t subbands);
static void calcNrgPerSubband(int32_t **analysBufferReal, int32_t **analysBufferImag, int32_t lowSubband,
                              int32_t highSubband, int32_t start_pos, int32_t next_pos, int8_t frameExp,
                              int32_t *nrgEst, int8_t *nrgEst_e);
static void calcNrgPerSfb(int32_t **analysBufferReal, int32_t **analysBufferImag, int32_t nSfb, uint8_t *freqBandTable,
                          int32_t start_pos, int32_t next_pos, int8_t input_e, int32_t *nrg_est, int8_t *nrg_est_e);
static void calcSubbandGain(int32_t nrgRef, int8_t nrgRef_e, ENV_CALC_NRGS_t *nrgs, int32_t c, int32_t tmpNoise,
                            int8_t tmpNoise_e, uint8_t sinePresentFlag, uint8_t sineMapped, int32_t noNoiseFlag);
static void calcAvgGain(ENV_CALC_NRGS_t *nrgs, int32_t lowSubband, int32_t highSubband, int32_t *sumRef_m,
                        int8_t *sumRef_e, int32_t *ptrAvgGain_m, int8_t *ptrAvgGain_e);
static void adjustTimeSlot_EldGrid(int32_t *ptrReal, ENV_CALC_NRGS_t *nrgs, uint8_t *ptrHarmIndex, int32_t lowSubbands,
                                   int32_t noSubbands, int32_t scale_change, int32_t noNoiseFlag,
                                   int32_t *ptrPhaseIndex, int32_t scale_diff_low);
static void adjustTimeSlotLC(int32_t *ptrReal, ENV_CALC_NRGS_t *nrgs, uint8_t *ptrHarmIndex, int32_t lowSubbands,
                             int32_t noSubbands, int32_t scale_change, int32_t noNoiseFlag, int32_t *ptrPhaseIndex);

//----------------------------------------------------------------------------------------------------------------------
//          I N L I N E S
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