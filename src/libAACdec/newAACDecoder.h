#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>


#define WAV_BITS          	   16
#define SAMPLE_BITS       	   16
#define SAMPLE_MAX        	   ((int16_t)(((uint32_t)1 << (SAMPLE_BITS - 1)) - 1))
#define SAMPLE_MIN        	   (~SAMPLE_MAX)
#define ALIGNMENT_DEFAULT 	   8
#define M_PI              	   3.14159265358979323846 // Pi. Only used in example projects.
#define DFLT_CH_MAP_TAB_LEN    15          // Length of the default channel map info table.

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
#define CAPF_ADTS              0x00000001  // Support flag for ADTS transport format.
#define CAPF_ADIF              0x00000002  // Support flag for ADIF transport format.
#define CAPF_LATM              0x00000004  // Support flag for LATM transport format.
#define CAPF_LOAS              0x00000008  // Support flag for LOAS transport format.
#define CAPF_RAWPACKETS        0x00000010  // Support flag for RAW PACKETS transport format.
#define CAPF_DRM               0x00000020  // Support flag for DRM/DRM+ transport format.
#define CAPF_RSVD50            0x00000040  // Support flag for RSVD50 transport format
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
#define CAPF_DMX_BLIND         0x00000001  // Support flag for blind downmixing.
#define CAPF_DMX_PCE           0x00000002  // Support flag for guided downmix with data from MPEG-2/4 PCE
#define CAPF_DMX_ARIB          0x00000004  // PCE guided downmix with slightly different equations to fulfill ARIB standard.
#define CAPF_DMX_DVB           0x00000008  // Support flag for guided downmix with data from DVB ancillary  data fields.
#define CAPF_DMX_CH_EXP        0x00000010  // flag for simple upmixing by dublicating channels or  adding zero channels.
#define CAPF_DMX_6_CH          0x00000020
#define CAPF_DMX_8_CH          0x00000040
#define CAPF_DMX_24_CH         0x00000080
#define CAPF_LIMITER           0x00002000
/* MPEG Surround capability flags */
#define CAPF_MPS_STD           0x00000001
#define CAPF_MPS_LD            0x00000002
#define CAPF_MPS_USAC          0x00000004
#define CAPF_MPS_HQ            0x00000010
#define CAPF_MPS_LP            0x00000020
#define CAPF_MPS_1CH_IN        0x00001000
#define CAPF_MPS_2CH_IN        0x00002000
#define CAPF_MPS_6CH_IN        0x00004000

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
