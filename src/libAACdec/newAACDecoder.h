#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "lookup.h"

#define WAV_BITS               16
#define SAMPLE_BITS            16
#define SAMPLE_MAX             ((int16_t)(((uint32_t)1 << (SAMPLE_BITS - 1)) - 1))
#define SAMPLE_MIN             (~SAMPLE_MAX)
#define ALIGNMENT_DEFAULT      8
#define M_PI                   3.14159265358979323846  // Pi. Only used in example projects.
#define DFLT_CH_MAP_TAB_LEN    15                      // Length of the default channel map info table.
#define USAC_ID_BIT            16                      /** USAC element IDs start at USAC_ID_BIT */
#define CAPF_AAC_LC            0x00000001              // Support flag for AAC Low Complexity.
#define CAPF_ER_AAC_LD         0x00000002              // Support flag for AAC Low Delay with Error Resilience tools.
#define CAPF_ER_AAC_SCAL       0x00000004              // Support flag for AAC Scalable.
#define CAPF_ER_AAC_LC         0x00000008              // Support flag for AAC Low Complexity with Error Resilience tools.
#define CAPF_AAC_480           0x00000010              // Support flag for AAC with 480 framelength.
#define CAPF_AAC_512           0x00000020              // Support flag for AAC with 512 framelength.
#define CAPF_AAC_960           0x00000040              // Support flag for AAC with 960 framelength.
#define CAPF_AAC_1024          0x00000080              // Support flag for AAC with 1024 framelength.
#define CAPF_AAC_HCR           0x00000100              // Support flag for AAC with Huffman Codeword Reordering.
#define CAPF_AAC_VCB11         0x00000200              // Support flag for AAC Virtual Codebook 11.
#define CAPF_AAC_RVLC          0x00000400              // Support flag for AAC Reversible Variable Length Coding.
#define CAPF_AAC_MPEG4         0x00000800              // Support flag for MPEG file format.
#define CAPF_AAC_DRC           0x00001000              // Support flag for AAC Dynamic Range Control.
#define CAPF_AAC_CONCEALMENT   0x00002000              // Support flag for AAC concealment.
#define CAPF_AAC_DRM_BSFORMAT  0x00004000              // Support flag for AAC DRM bistream format.
#define CAPF_ER_AAC_ELD        0x00008000              // Support flag for AAC Enhanced Low Delay with Error Resilience tools.
#define CAPF_ER_AAC_BSAC       0x00010000              // Support flag for AAC BSAC.
#define CAPF_AAC_ELD_DOWNSCALE 0x00040000              // Support flag for AAC-ELD Downscaling
#define CAPF_AAC_USAC_LP       0x00100000              // Support flag for USAC low power mode.
#define CAPF_AAC_USAC          0x00200000              // Support flag for Unified Speech and Audio Coding (USAC).
#define CAPF_ER_AAC_ELDV2      0x00800000              // Support flag for AAC Enhanced Low Delay with MPS 212.
#define CAPF_AAC_UNIDRC        0x01000000              // Support flag for MPEG-D Dynamic Range Control (uniDrc).
#define CAPF_ADTS              0x00000001              // Support flag for ADTS transport format.
#define CAPF_ADIF              0x00000002              // Support flag for ADIF transport format.
#define CAPF_LATM              0x00000004              // Support flag for LATM transport format.
#define CAPF_LOAS              0x00000008              // Support flag for LOAS transport format.
#define CAPF_RAWPACKETS        0x00000010              // Support flag for RAW PACKETS transport format.
#define CAPF_DRM               0x00000020              // Support flag for DRM/DRM+ transport format.
#define CAPF_RSVD50            0x00000040              // Support flag for RSVD50 transport format
#define CAPF_SBR_LP            0x00000001              // Support flag for SBR Low Power mode.
#define CAPF_SBR_HQ            0x00000002              // Support flag for SBR High Quality mode.
#define CAPF_SBR_DRM_BS        0x00000004              // Support flag for
#define CAPF_SBR_CONCEALMENT   0x00000008              // Support flag for SBR concealment.
#define CAPF_SBR_DRC           0x00000010              // Support flag for SBR Dynamic Range Control.
#define CAPF_SBR_PS_MPEG       0x00000020              // Support flag for MPEG Parametric Stereo.
#define CAPF_SBR_PS_DRM        0x00000040              // Support flag for DRM Parametric Stereo.
#define CAPF_SBR_ELD_DOWNSCALE 0x00000080              // Support flag for ELD reduced delay mode
#define CAPF_SBR_HBEHQ         0x00000100              // Support flag for HQ HBE
#define CAPF_DMX_BLIND         0x00000001              // Support flag for blind downmixing.
#define CAPF_DMX_PCE           0x00000002              // Support flag for guided downmix with data from MPEG-2/4 PCE
#define CAPF_DMX_ARIB          0x00000004              // PCE guided downmix with slightly different equations to fulfill ARIB standard.
#define CAPF_DMX_DVB           0x00000008              // Support flag for guided downmix with data from DVB ancillary  data fields.
#define CAPF_DMX_CH_EXP        0x00000010              // flag for simple upmixing by dublicating channels or  adding zero channels.
#define CAPF_DMX_6_CH          0x00000020
#define CAPF_DMX_8_CH          0x00000040
#define CAPF_DMX_24_CH         0x00000080
#define CAPF_LIMITER           0x00002000
#define CAPF_MPS_STD           0x00000001
#define CAPF_MPS_LD            0x00000002
#define CAPF_MPS_USAC          0x00000004
#define CAPF_MPS_HQ            0x00000010
#define CAPF_MPS_LP            0x00000020
#define CAPF_MPS_1CH_IN        0x00001000
#define CAPF_MPS_2CH_IN        0x00002000
#define CAPF_MPS_6CH_IN        0x00004000
#define AC_ER_VCB11            0x000001   // aacSectionDataResilienceFlag  flag (from ASC): 1 means use  virtual codebooks
#define AC_ER_RVLC             0x000002   // aacSpectralDataResilienceFlag (from ASC): 1 means use huffman codeword reordering
#define AC_ER_HCR              0x000004   // aacSectionDataResilienceFlag flag (from ASC): 1 means use  virtual codebooks
#define AC_SCALABLE            0x000008   // AAC Scalable
#define AC_ELD                 0x000010   // AAC-ELD
#define AC_LD                  0x000020   // AAC-LD
#define AC_ER                  0x000040   // ER syntax
#define AC_BSAC                0x000080   // BSAC
#define AC_USAC                0x000100   // USAC
#define AC_RSV603DA            0x000200   // RSVD60 3D audio
#define AC_HDAAC               0x000400   // HD-AAC
#define AC_RSVD50              0x004000   // Rsvd50
#define AC_SBR_PRESENT         0x008000   // SBR present flag (from ASC)
#define AC_SBRCRC              0x010000   // SBR CRC present flag. Only relevant for AAC-ELD for now.
#define AC_PS_PRESENT          0x020000   // PS present flag (from ASC or implicit)
#define AC_MPS_PRESENT         0x040000   // MPS present flag (from ASC or implicit)
#define AC_DRM                 0x080000   // DRM bit stream syntax
#define AC_INDEP               0x100000   // Independency flag
#define AC_MPEGD_RES           0x200000   // MPEG-D residual individual channel data.
#define AC_SAOC_PRESENT        0x400000   // SAOC Present Flag
#define AC_DAB                 0x800000   // DAB bit stream syntax
#define AC_ELD_DOWNSCALE       0x1000000  // ELD Downscaled playout
#define AC_LD_MPS              0x2000000  // Low Delay MPS.
#define AC_DRC_PRESENT         0x4000000  // Dynamic Range Control (DRC) data found.
#define AC_USAC_SCFGI3         0x8000000  // USAC flag: If stereoConfigIndex is 3 the flag is set.
#define AC_CM_DET_CFG_CHANGE   0x000001   // Config mode signalizes the callback to work in config change  detection mode
#define AC_CM_ALLOC_MEM        0x000002   // Config mode signalizes the callback to work in memory allocation mode
#define AC_EL_USAC_TW          0x000001   // USAC time warped filter bank is active
#define AC_EL_USAC_NOISE       0x000002   // USAC noise filling is active
#define AC_EL_USAC_ITES        0x000004   // USAC SBR inter-TES tool is active
#define AC_EL_USAC_PVC         0x000008   // USAC SBR predictive vector coding tool is active
#define AC_EL_USAC_MPS212      0x000010   // USAC MPS212 tool is active
#define AC_EL_USAC_LFE         0x000020   // USAC element is LFE
#define AC_EL_USAC_CP_POSSIBLE 0x000040   // USAC may use Complex Stereo Prediction in this channel element
#define AC_EL_ENHANCED_NOISE   0x000080   // Enhanced noise filling
#define AC_EL_IGF_AFTER_TNS    0x000100   // IGF after TNS
#define AC_EL_IGF_INDEP_TILING 0x000200   // IGF independent tiling
#define AC_EL_IGF_USE_ENF      0x000400   // IGF use enhanced noise filling
#define AC_EL_FULLBANDLPD      0x000800   // enable fullband LPD tools
#define AC_EL_LPDSTEREOIDX     0x001000   // LPD-stereo-tool stereo index
#define AC_EL_LFE              0x002000   // The element is of type LFE.
#define CC_MPEG_ID             0x00100000
#define CC_IS_BASELAYER        0x00200000
#define CC_PROTECTION          0x00400000
#define CC_SBR                 0x00800000
#define CC_SBRCRC              0x00010000
#define CC_SAC                 0x00020000
#define CC_RVLC                0x01000000
#define CC_VCB11               0x02000000
#define CC_HCR                 0x04000000
#define CC_PSEUDO_SURROUND     0x08000000
#define CC_USAC_NOISE          0x10000000
#define CC_USAC_TW             0x20000000
#define CC_USAC_HBE            0x40000000

#define FX_CFG2FX_DBL
#define FX_CFG2FX_SGL    FX_DBL2FX_SGL
#define FL2FXCONST_CFG   FL2FXCONST_DBL
#define FX_DBL2FX_CFG(x) ((int32_t)(x))

#define CLIP_PROTECT_GAIN_0(x) FL2FXCONST_CFG(((x) / (float)(1 << 0)))
#define CLIP_PROTECT_GAIN_1(x) FL2FXCONST_CFG(((x) / (float)(1 << 1)))
#define CLIP_PROTECT_GAIN_2(x) FL2FXCONST_CFG(((x) / (float)(1 << 2)))

#define FRACT_BITS                   16 /* single precision */
#define DFRACT_BITS                  32 /* double precision */

#define MAXVAL_SGL ((signed)0x00007FFF) /* this has to be synchronized to FRACT_BITS */
#define MINVAL_SGL ((signed)0xFFFF8000) /* this has to be synchronized to FRACT_BITS */
#define MAXVAL_DBL ((signed)0x7FFFFFFF) /* this has to be synchronized to DFRACT_BITS */
#define MINVAL_DBL ((signed)0x80000000) /* this has to be synchronized to DFRACT_BITS */

#define SF_CLD_C1C2 (9)
#define FRACT_FIX_SCALE  ((int64_t(1) << (FRACT_BITS - 1)))
#define DFRACT_FIX_SCALE ((int64_t(1) << (DFRACT_BITS - 1)))
#define IS_USAC(aot)     ((aot) == AOT_USAC)
#define IS_LOWDELAY(aot) ((aot) == AOT_ER_AAC_LD || (aot) == AOT_ER_AAC_ELD)
#define IS_CHANNEL_ELEMENT(elementId)     ((elementId) == ID_SCE || (elementId) == ID_CPE || (elementId) == ID_LFE || (elementId) == ID_USAC_SCE || \
                                           (elementId) == ID_USAC_CPE || (elementId) == ID_USAC_LFE)
#define IS_MP4_CHANNEL_ELEMENT(elementId) ((elementId) == ID_SCE || (elementId) == ID_CPE || (elementId) == ID_LFE)
#define IS_USAC_CHANNEL_ELEMENT(elementId)((elementId) == ID_USAC_SCE || (elementId) == ID_USAC_CPE || (elementId) == ID_USAC_LFE)

#define FL2FXCONST_SPC               FL2FXCONST_DBL
#define MINVAL_DBL_CONST             MINVAL_DBL
#define MINVAL_SGL_CONST             MINVAL_SGL
#define MSB_31_MASK                  0x80000000 /* masks MSB (= Bit 31) in a 32 bit word */
#define DIMENSION_OF_ESCAPE_CODEBOOK 2          /* for cb >= 11 is dimension 2 */
#define ESCAPE_CODEBOOK              11
#define MASK_ESCAPE_PREFIX_UP        0x000F0000
#define LSB_ESCAPE_PREFIX_UP         16
#define MASK_ESCAPE_PREFIX_DOWN      0x0000F000
#define LSB_ESCAPE_PREFIX_DOWN       12
#define MASK_ESCAPE_WORD             0x00000FFF
#define MASK_FLAG_A                  0x00200000
#define MASK_FLAG_B                  0x00100000

#define FL2FXCONST_DBL(val)                                                                            \
    (int32_t)(((val) >= 0) ? ((((double)(val) * (DFRACT_FIX_SCALE) + 0.5) >= (double)(MAXVAL_DBL))     \
                                  ? (int32_t)(MAXVAL_DBL)                                              \
                                  : (int32_t)((double)(val) * (double)(DFRACT_FIX_SCALE) + 0.5))       \
                           : ((((double)(val) * (DFRACT_FIX_SCALE)-0.5) <= (double)(MINVAL_DBL_CONST)) \
                                  ? (int32_t)(MINVAL_DBL_CONST)                                        \
                                  : (int32_t)((double)(val) * (double)(DFRACT_FIX_SCALE)-0.5)))

/* macros for runtime conversion of float values to integer fixedpoint. NO * OVERFLOW CHECK!!! */
#define FL2FX_SPC      FL2FX_DBL
#define FL2FX_SGL(val) ((val) > 0.0f ? (int16_t)((val) * (float)(FRACT_FIX_SCALE) + 0.5f) : (int16_t)((val) * (float)(FRACT_FIX_SCALE)-0.5f))
#define FL2FX_DBL(val) ((val) > 0.0f ? (int32_t)((val) * (float)(DFRACT_FIX_SCALE) + 0.5f) : (int32_t)((val) * (float)(DFRACT_FIX_SCALE)-0.5f))

#define OverlapBufferSize        (768)
#define SPEC_LONG(ptr)           (ptr)
#define SPEC(ptr, w, gl)         ((ptr) + ((w) * (gl)))
#define SPEC_TCX(ptr, f, gl, fb) ((ptr) + ((f) * (gl * 2) * (((fb) == 0) ? 1 : 2)))

/** Macro to identify decode errors. */
#define TPDEC_DECODE_ERROR_START   1024
#define TPDEC_DECODE_ERROR_END     1028
#define TPDEC_IS_DECODE_ERROR(err) (((err >= TPDEC_DECODE_ERROR_START) && (err <= TPDEC_DECODE_ERROR_END)) ? 1 : 0)
/** Macro to identify fatal errors. */
#define TPDEC_FATAL_ERROR_START    512
#define TPDEC_FATAL_ERROR_END      517
#define TPDEC_IS_FATAL_ERROR(err) (((err >= TPDEC_FATAL_ERROR_START) && (err <= TPDEC_FATAL_ERROR_END)) ? 1 : 0)

#define AAC_DEC_INIT_ERROR_START   8192
#define AAC_DEC_INIT_ERROR_END     12287
#define IS_INIT_ERROR(err)       ((((err) >= AAC_DEC_INIT_ERROR_START) && ((err) <= AAC_DEC_INIT_ERROR_END)) ? 1 : 0)

#define AAC_DEC_DECODE_ERROR_START 16384
#define AAC_DEC_DECODE_ERROR_END   20479
#define IS_DECODE_ERROR(err)       ((((err) >= AAC_DEC_DECODE_ERROR_START) && ((err) <= AAC_DEC_DECODE_ERROR_END)) ? 1 : 0)
#define IS_OUTPUT_VALID(err) (((err) == AAC_DEC_OK) || IS_DECODE_ERROR(err))

#define CAPF_TPDEC_ADIF       0x00001000 /**< Flag indicating support for ADIF transport format.        */
#define CAPF_TPDEC_ADTS       0x00002000 /**< Flag indicating support for ADTS transport format.        */
#define CAPF_TPDEC_LOAS       0x00004000 /**< Flag indicating support for LOAS transport format.        */
#define CAPF_TPDEC_LATM       0x00008000 /**< Flag indicating support for LATM transport format.        */
#define CAPF_TPDEC_RAWPACKETS 0x00010000 /**< Flag indicating support for raw packets transport format. */

#define EXT_ID_BITS                             4         /**< Size in bits of extension payload type tags. */
#define MAX_DRC_THREADS                         ((8) + 1) /* Heavy compression value is handled just like MPEG DRC data */
#define MAX_DRC_BANDS                           (16)      /* 2^LEN_DRC_BAND_INCR (LEN_DRC_BAND_INCR = 4) */
#define MAX_NOISE_ENVELOPES                     2
#define MAX_NOISE_COEFFS                        5
#define MAX_NUM_NOISE_VALUES                    (MAX_NOISE_ENVELOPES * MAX_NOISE_COEFFS)
#define MAX_NUM_LIMITERS                        12
#define MAX_ENVELOPES_LEGACY                    5
#define MAX_ENVELOPES_USAC                      8
#define MAX_ENVELOPES                           MAX_ENVELOPES_USAC
#define MAX_FREQ_COEFFS_DUAL_RATE               48
#define MAX_FREQ_COEFFS_QUAD_RATE               56
#define MAX_FREQ_COEFFS                         MAX_FREQ_COEFFS_QUAD_RATE
#define SF_PITCH_TRACK                          6
#define SF_GAIN                                 3
#define MIN_VAL                                 FL2FXCONST_DBL(0.0f)
#define MAX_VAL                                 (int32_t) MAXVAL_DBL
#define MAX_FREQ_COEFFS_FS44100                 35
#define MAX_FREQ_COEFFS_FS48000                 32
#define MAX_NUM_ENVELOPE_VALUES                 (MAX_ENVELOPES * MAX_FREQ_COEFFS)
#define MAX_SFB_NRG_HEADROOM                    (1)
#define MAX_VAL_NRG_HEADROOM                    ((((int32_t)MAXVAL_DBL) >> MAX_SFB_NRG_HEADROOM))
#define ACCU_BITS                               40 /* double precision plus overflow */
#define AACDECODER_LIB_VL0                      3
#define AACDECODER_LIB_VL1                      2
#define AACDECODER_LIB_VL2                      0
#define PIT_MAX_MAX                             512
#define UP_SAMP                                 4
#define L_INTERPOL2                             16
#define L_SUBFR                                 64
#define NB_DIV                                  4                             /* number of division (20ms) per 80ms frame */
#define L_SUBFR                                 64                            /* subframe size (5ms) */
#define BPF_SFD                                 1                             /* bass postfilter delay (subframe) */
#define BPF_DELAY                               (BPF_SFD * L_SUBFR)           /* bass postfilter delay (samples) */
#define L_FILT                                  12                            /* Delay of up-sampling filter (bass post-filter) */
#define L_EXTRA                                 96                            /* for bass post-filter */
#define L_INTERPOL                              (16 + 1)                      /* Length of filter for interpolation (acelp decoder) */
#define L_FRAME_PLUS_1024                       1024                          /* length of one 80ms superframe */
#define L_DIV_1024                              (L_FRAME_PLUS_1024 / NB_DIV)  /* length of one acelp or tcx20 frame */
#define NB_SUBFR_1024                           (L_DIV_1024 / L_SUBFR)        /* number of 5ms subframe per division */
#define NB_SUBFR_SUPERFR_1024                   (L_FRAME_PLUS_1024 / L_SUBFR) /* number of 5ms subframe per 80ms frame */
#define AAC_SFD_1024                            (NB_SUBFR_SUPERFR_1024 / 2)   /* AAC delay (subframe) */
#define AAC_DELAY_1024                          (AAC_SFD_1024 * L_SUBFR)      /* AAC delay (samples) */
#define SYN_SFD_1024                            (AAC_SFD_1024 - BPF_SFD)      /* synthesis delay (subframe) */
#define SYN_DELAY_1024                          (SYN_SFD_1024 * L_SUBFR)      /* synthesis delay (samples) */
#define LFAC_1024                               (L_DIV_1024 / 2)              /* FAC frame length */
#define LFAC_SHORT_1024                         (L_DIV_1024 / 4)              /* for transitions EIGHT_SHORT FD->LPD and vv. */
#define FDNS_NPTS_1024                          64                            /* FD noise shaping resolution (64=100Hz/point) */
#define L_FRAME_PLUS                            L_FRAME_PLUS_1024
#define L_DIV                                   L_DIV_1024
#define NB_SUBFR                                NB_SUBFR_1024
#define NB_SUBFR_SUPERFR                        NB_SUBFR_SUPERFR_1024
#define AAC_SFD                                 AAC_SFD_1024
#define AAC_DELAY                               AAC_DELAY_1024
#define SYN_SFD                                 SYN_SFD_1024
#define SYN_DELAY                               SYN_DELAY_1024
#define LFAC                                    LFAC_1024
#define LFAC_SHORT                              LFAC_SHORT_1024
#define FDNS_NPTS                               FDNS_NPTS_1024
#define SF_CODE                                 6               /* exponent of code[], fixed codebook vector */
#define SF_GAIN_C                               16              /* exponent of gain code and smoothed gain code */
#define SF_EXC                                  16              /* exponent of exc[] and exc2[], excitation buffer */
#define SF_GAIN_P                               1               /* exponent of gain_pit */
#define SF_PFAC                                 0               /* exponent of period/voicing factor */
#define SF_SYNTH                                SF_EXC          /* exponent of synthesis buffer */
#define SF_A_COEFFS                             3               /* exponent of LP domain synthesis filter coefficient */
#define SF_STAB                                 1               /* exponent of stability factor */
#define M_LP_FILTER_ORDER                       16              /* LP filter order */
#define LP_FILTER_SCALE                         4               /* LP filter scale */
#define PIT_MIN_12k8                            34              /* Minimum pitch lag with resolution 1/4 */
#define PIT_MAX_12k8                            231             /* Maximum pitch lag for fs=12.8kHz */
#define FSCALE_DENOM                            12800           /* Frequency scale denominator */
#define FAC_FSCALE_MIN                          6000            /* Minimum allowed frequency scale for acelp decoder */
#define LPD_MAX_CORE_SR                         24000           /* Default value from ref soft */
#define FAC_FSCALE_MAX                          LPD_MAX_CORE_SR /* Maximum allowed frequency scale for acelp decoder */
#define A2                                      FL2FX_SGL(2 * 0.18f)
#define B                                       FL2FX_SGL(0.64f)
#define FWD                                     0  /* bitstream decoding direction forward (RVL coded part) */
#define BWD                                     1  /* bitstream decoding direction backward (RVL coded part) */
#define MAX_RVL                                 7  /* positive RVLC escape */
#define MIN_RVL                                 -7 /* negative RVLC escape */
#define MAX_ALLOWED_DPCM_INDEX                  14 /* the maximum allowed index of a decoded dpcm value (offset 'TABLE_OFFSET' incl --> must be subtracted) */
#define TABLE_OFFSET                            7  /* dpcm offset of valid output values of rvl table decoding */
#define MAX_LEN_RVLC_CODE_WORD                  9  /* max length of a RVL codeword in bits */
#define MAX_LEN_RVLC_ESCAPE_WORD                20 /* max length of huffman coded RVLC escape word in bits */
#define DPCM_NOISE_NRG_BITS                     9
#define SF_OFFSET                               100   /* offset for correcting scf value */
#define CONCEAL_MAX_INIT                        1311  /* arbitrary value */
#define CONCEAL_MIN_INIT                        -1311 /* arbitrary value */
#define RVLC_MAX_SFB                            ((8) * (16))
#define LINES_PER_UNIT                          4
#define MAX_SFB_HCR                             (((1024 / 8) / LINES_PER_UNIT) * 8) /* (8 * 16) is not enough because sfbs are split in units for blocktype int16_t */
#define NUMBER_OF_UNIT_GROUPS                   (LINES_PER_UNIT * 8)
#define LINES_PER_UNIT_GROUP                    (1024 / NUMBER_OF_UNIT_GROUPS) /* 15 16 30 32 */
#define WB_SECTION_SIZE                         (1024 * 2)
#define DRM_BS_BUFFER_SIZE                      (512) /* size of the dynamic buffer which is used to reverse the bits of  the DRM SBR payload */
#define CONCEAL_MAX_NUM_FADE_FACTORS            (32)
#define FIXP_CNCL                               int32_t
#define FL2FXCONST_CNCL                         FL2FXCONST_DBL
#define CNCL_FRACT_BITS                         DFRACT_BITS
#define TNS_MAXIMUM_ORDER                       (20)  // 12 for AAC-LC and AAC-SSR. Set to 20 for AAC-Main (AOT 1).
#define N_MAX_LINES                             4
#define NO_OFBANDS                              ((8 * 16))
#define FROM_LEFT_TO_RIGHT                      0
#define FROM_RIGHT_TO_LEFT                      1
#define MAX_CB_PAIRS                            23
#define MAX_HCR_SETS                            14
#define ESCAPE_VALUE                            16
#define POSITION_OF_FLAG_A                      21
#define POSITION_OF_FLAG_B                      20
#define MAX_CB                                  32 /* last used CB is cb #31 when VCB11 is used */
#define MAX_CB_CHECK                            32 /* support for VCB11 available -- is more general, could therefore used in both cases */
#define NUMBER_OF_BIT_IN_WORD                   32
#define THIRTYTWO_LOG_DIV_TWO_LOG               5
#define EIGHT_LOG_DIV_TWO_LOG                   3
#define FOUR_LOG_DIV_TWO_LOG                    2
#define CPE_TOP_LENGTH                          12288
#define SCE_TOP_LENGTH                          6144
#define LEN_OF_LONGEST_CW_TOP_LENGTH            49
#define Q_VALUE_INVALID                         8192 /* mark a invalid line with this value (to be concealed later on) */
#define HCR_DIRAC                               500  /* a line of high level */
#define MASK_LEFT                               0xFFF000
#define MASK_RIGHT                              0xFFF
#define CLR_BIT_10                              0x3FF
#define TEST_BIT_10                             0x400
#define LEFT_OFFSET                             12
#define MAX_BUFSIZE_BYTES                       (0x10000000)
#define FIXP_DBL2PCM_DEC(x)                     (x)
#define PCM_DEC2FIXP_DBL(x)                     (x)
#define PCM_DEC_BITS                            DFRACT_BITS
#define PCM_DEC2FX_PCM(x)                       FX_DBL2FX_SGL(x)(x)
#define FX_PCM2PCM_DEC(x)                       FX_SGL2FX_DBL((int16_t)(x))(x)
#define AACDEC_MAX_CH_CONF                      14
#define AACDEC_CH_ELEMENTS_TAB_SIZE             7   /*!< Size of element tables */
#define AAC_NF_NO_RANDOM_VAL                    512 /*!< Size of random number array for noise floor */
#define INV_QUANT_TABLESIZE                     256
#define NUM_LD_COEF_512                         1536
#define NUM_LD_COEF_480                         1440
#define WTS0                                    (1)
#define WTS1                                    (0)
#define WTS2                                    (-2)
#define SF_FNA_COEFFS                           1 /* Compile-time prescaler for MDST-filter coefficients. */
#define FILT(a)                                 ((FL2FXCONST_DBL(a)) >> SF_FNA_COEFFS)
#define AACDEC_DRC_GAIN_SCALING                 (11)                                                    /* Scaling of DRC gains */
#define AACDEC_DRC_GAIN_INIT_VALUE              (FL2FXCONST_DBL(1.0f / (1 << AACDEC_DRC_GAIN_SCALING))) /* Init value for DRC gains */
#define AACDEC_DRC_DFLT_EXPIRY_FRAMES           (0)
#define STOP_THIS_STATE                         0
#define BODY_ONLY                               1
#define BODY_SIGN__BODY                         2 /* [stop if no sign]  */
#define BODY_SIGN__SIGN                         3 /* [stop if sign bits decoded]  */
#define BODY_SIGN_ESC__BODY                     4 /* [stop if no sign] */
#define BODY_SIGN_ESC__SIGN                     5 /* [stop if no escape sequence] */
#define BODY_SIGN_ESC__ESC_PREFIX               6
#define BODY_SIGN_ESC__ESC_WORD                 7 /* [stop if abs(second qsc) != 16] */ /* Default DRC data expiry time in AAC frames   */
#define TPDEC_SYNCOK                            1
#define TPDEC_MINIMIZE_DELAY                    2
#define TPDEC_IGNORE_BUFFERFULLNESS             4
#define TPDEC_EARLY_CONFIG                      8
#define TPDEC_LOST_FRAMES_PENDING               16
#define TPDEC_CONFIG_FOUND                      32
#define TPDEC_USE_ELEM_SKIPPING                 64
#define TPDEC_FORCE_CONFIG_CHANGE               1
#define TPDEC_FORCE_CONTENT_CHANGE              2
#define TPDEC_SKIP_PACKET                       1
#define PC_FSB_CHANNELS_MAX                     16 /* Front/Side/Back channels */
#define PC_LFE_CHANNELS_MAX                     4
#define PC_ASSOCDATA_MAX                        8
#define PC_CCEL_MAX                             16 /* CC elements */
#define PC_COMMENTLENGTH                        256
#define PC_NUM_HEIGHT_LAYER                     3
#define TP_USAC_MAX_SPEAKERS                    (24)
#define TP_USAC_MAX_EXT_ELEMENTS                ((24))
#define TP_USAC_MAX_ELEMENTS                    ((24) + TP_USAC_MAX_EXT_ELEMENTS)
#define TP_USAC_MAX_CONFIG_LEN                  512 /* next power of two of maximum of escapedValue(hBs, 4, 4, 8) in  AudioPreRoll() (285) */
#define TPDEC_USAC_NUM_CONFIG_CHANGE_FRAMES     (1) /* Number of frames for config change in USAC */
#define LINES_PER_UNIT                          4
#define ERROR_LORSD                             0 /* offset: error if different from zero */
#define ERROR_LOLC                              0 /* offset: error if different from zero */
#define ERROR_PCW_BODY                          0 /* set a positive values to trigger the error (make segments earlyer appear to be empty) */
#define ERROR_PCW_BODY_SIGN                     0 /* set a positive values to trigger the error (make segments earlyer appear to be empty) */
#define ERROR_PCW_BODY_SIGN_ESC                 0 /* set a positive values to trigger the error (make segments earlyer appear to be empty) */
#define ERROR_PCW_BODY_ONLY_TOO_LONG            0 /* set a positive values to trigger the error */
#define ERROR_PCW_BODY_SIGN_TOO_LONG            0 /* set a positive values to trigger the error */
#define ERROR_PCW_BODY_SIGN_ESC_TOO_LONG        0 /* set a positive values to trigger the error */
#define MODULO_DIVISOR_HCR                      30
#define HCR_ERROR_CONCEALMENT                   1 /* if set to '1', HCR _mutes_ the erred quantized spectral coefficients */

#define SEGMENT_OVERRIDE_ERR_PCW_BODY           0x80000000
#define SEGMENT_OVERRIDE_ERR_PCW_BODY_SIGN      0x40000000
#define SEGMENT_OVERRIDE_ERR_PCW_BODY_SIGN_ESC  0x20000000
#define EXTENDED_SORTED_COUNTER_OVERFLOW        0x10000000
#define TOO_MANY_PCW_BODY_BITS_DECODED          0x00080000
#define TOO_MANY_PCW_BODY_SIGN_BITS_DECODED     0x00040000
#define TOO_MANY_PCW_BODY_SIGN_ESC_BITS_DECODED 0x00020000
#define STATE_ERROR_BODY_ONLY                   0x00008000
#define STATE_ERROR_BODY_SIGN__BODY             0x00004000
#define STATE_ERROR_BODY_SIGN__SIGN             0x00002000
#define STATE_ERROR_BODY_SIGN_ESC__BODY         0x00001000
#define STATE_ERROR_BODY_SIGN_ESC__SIGN         0x00000800
#define STATE_ERROR_BODY_SIGN_ESC__ESC_PREFIX   0x00000400
#define STATE_ERROR_BODY_SIGN_ESC__ESC_WORD     0x00000200
#define HCR_SI_LENGTHS_FAILURE                  0x00000100
#define NUM_SECT_OUT_OF_RANGE_SHORT_BLOCK       0x00000080
#define NUM_SECT_OUT_OF_RANGE_LONG_BLOCK        0x00000040
#define LINE_IN_SECT_OUT_OF_RANGE_SHORT_BLOCK   0x00000020
#define CB_OUT_OF_RANGE_SHORT_BLOCK             0x00000010
#define LINE_IN_SECT_OUT_OF_RANGE_LONG_BLOCK    0x00000008
#define CB_OUT_OF_RANGE_LONG_BLOCK              0x00000004
#define LAV_VIOLATION                           0x00000002
#define BIT_IN_SEGMENTATION_ERROR               0x00000001
#define HCR_FATAL_PCW_ERROR_MASK                0x100E01FC

#define NB_SPHERE     32
#define NB_LEADER     37
#define NB_LDSIGN     226
#define NB_LDQ3       9
#define NB_LDQ4       28
#define LSF_SCALE     13
#define L_FILT        12 /* Delay of up-sampling filter                */
#define SF_QUA_GAIN7B 4
#define NB_SPHERE     32
#define NB_LEADER     37
#define NB_LDSIGN     226
#define NB_LDQ3       9
#define NB_LDQ4       28

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
} CHANNEL_MODE_t;

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
} FDK_MODULE_ID_t;

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
} AACDEC_DRC_PARAM_tETER_HANDLING_t;

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

typedef enum { /* Output rendering mode */
    AACDEC_RENDER_INVALID = 0,
    AACDEC_RENDER_IMDCT,
    AACDEC_RENDER_ELDFB,
    AACDEC_RENDER_LPD,
    AACDEC_RENDER_INTIMDCT
} AACDEC_RENDER_MODE_t;

typedef enum {
    ConcealMethodNone = -1,
    ConcealMethodMute = 0,
    ConcealMethodNoise = 1,
    ConcealMethodInter = 2,
    ConcealMethodTonal = 3
} CConcealmentMethod_t;

typedef enum {
    ConcealState_Ok,
    ConcealState_Single,
    ConcealState_FadeIn,
    ConcealState_Mute,
    ConcealState_FadeOut,
} CConcealmentState_t;

typedef enum {
    FADE_TIMEDOMAIN_TOSPECTRALMUTE = 1,
    FADE_TIMEDOMAIN_FROMSPECTRALMUTE,
    FADE_TIMEDOMAIN,
} TDfadingType_t;

typedef enum {
    FD_LONG,
    FD_SHORT,
    LPD,
} USAC_COREMODE_t;

enum { MAX_QUANTIZED_VALUE = 8191 };

enum { JointStereoMaximumGroups = 8, JointStereoMaximumBands = 64 };

enum { HuffmanBits = 2, HuffmanEntries = (1 << HuffmanBits) };

enum {
    ZERO_HCB = 0,
    ESCBOOK = 11,
    NSPECBOOKS = ESCBOOK + 1,
    BOOKSCL = NSPECBOOKS,
    NOISE_HCB = 13,
    INTENSITY_HCB2 = 14,
    INTENSITY_HCB = 15,
    LAST_HCB
};

enum {
    TNS_MAX_WINDOWS = 8, /* 8 */
    TNS_MAXIMUM_FILTERS = 3
};

typedef enum {
    UNKNOWN_PAYLOAD = 0,
    MPEG_DRC_EXT_DATA = 1,
    DVB_DRC_ANC_DATA = 2,
} AACDEC_DRC_PAYLOAD_TYPE_t;

typedef enum {
    DRC_CUT_SCALE = 0,
    DRC_BOOST_SCALE,
    TARGET_REF_LEVEL,
    DRC_BS_DELAY,
    DRC_DATA_EXPIRY_FRAME,
    APPLY_HEAVY_COMPRESSION,
    DEFAULT_PRESENTATION_MODE,
    ENCODER_TARGET_LEVEL,
    MAX_OUTPUT_CHANNELS,
    UNIDRC_PRECEDENCE
} AACDEC_DRC_PARAM_t;

typedef enum {
    AAC_DEC_OK = 0x0000,                   /*!< No error occurred. Output buffer is valid and error free. */
    AAC_DEC_OUT_OF_MEMORY = 0x0002,        /*!< Heap returned NULL pointer. Output buffer is invalid. */
    AAC_DEC_UNKNOWN = 0x0005,              /*!< Error condition is of unknown reason, or from a another
                                              module. Output buffer is invalid. */
    aac_dec_sync_error_start = 0x1000,     /* Synchronization errors. Output buffer is invalid. */
    AAC_DEC_TRANSPORT_SYNC_ERROR = 0x1001, /*!< The transport decoder had synchronization problems. Do not exit
                                            decoding. Just feed new bitstream data. */
    AAC_DEC_NOT_ENOUGH_BITS = 0x1002,      /*!< The input buffer ran out of bits. */
    aac_dec_sync_error_end = 0x1FFF,
    aac_dec_init_error_start = 0x2000,          /* Initialization errors. Output buffer is invalid. */
    AAC_DEC_INVALID_HANDLE = 0x2001,            /*!< The handle passed to the function call was invalid (NULL). */
    AAC_DEC_UNSUPPORTED_AOT = 0x2002,           /*!< The AOT found in the configuration is not supported. */
    AAC_DEC_UNSUPPORTED_FORMAT = 0x2003,        /*!< The bitstream format is not supported.  */
    AAC_DEC_UNSUPPORTED_ER_FORMAT = 0x2004,     /*!< The error resilience tool format is not supported. */
    AAC_DEC_UNSUPPORTED_EPCONFIG = 0x2005,      /*!< The error protection format is not supported. */
    AAC_DEC_UNSUPPORTED_MULTILAYER = 0x2006,    /*!< More than one layer for AAC scalable is not supported. */
    AAC_DEC_UNSUPPORTED_CHANNELCONFIG = 0x2007, /*!< The channel configuration (either number or arrangement) is
                                                   not supported. */
    AAC_DEC_UNSUPPORTED_SAMPLINGRATE = 0x2008,  /*!< The sample rate specified in the configuration is not supported. */
    AAC_DEC_INVALID_SBR_CONFIG = 0x2009,        /*!< The SBR configuration is not supported. */
    AAC_DEC_SET_PARAM_FAIL = 0x200A,            /*!< The parameter could not be set. Either the value was out of range
                                                     or the parameter does  not exist. */
    AAC_DEC_NEED_TO_RESTART = 0x200B,           /*!< The decoder needs to be restarted, since the required configuration
                                                     change cannot be performed. */
    AAC_DEC_OUTPUT_BUFFER_TOO_SMALL = 0x200C,   /*!< The provided output buffer is too small. */
    aac_dec_init_error_end = 0x2FFF,
    aac_dec_decode_error_start = 0x4000,            /* Decode errors. Output buffer is valid but concealed. */
    AAC_DEC_TRANSPORT_ERROR = 0x4001,               /*!< The transport decoder encountered an unexpected error. */
    AAC_DEC_PARSE_ERROR = 0x4002,                   /*!< Error while parsing the bitstream. Most  probably it is corrupted, or the system crashed. */
    AAC_DEC_UNSUPPORTED_EXTENSION_PAYLOAD = 0x4003, /*!< Error while parsing the extension payload of the bitstream.
                                                         The extension payload type found is not supported. */
    AAC_DEC_DECODE_FRAME_ERROR = 0x4004,            /*!< The parsed bitstream value is out of range. Most probably the
                                                         bitstream is corrupt, or the system crashed. */
    AAC_DEC_CRC_ERROR = 0x4005,                     /*!< The embedded CRC did not match. */
    AAC_DEC_INVALID_CODE_BOOK = 0x4006,             /*!< An invalid codebook was signaled. Most probably the bitstream
                                                         is corrupt, or the system  crashed. */
    AAC_DEC_UNSUPPORTED_PREDICTION = 0x4007,        /*!< Predictor found, but not supported in the AAC Low Complexity
                                                       profile. Most probably the bitstream is corrupt, or has a wrong format. */
    AAC_DEC_UNSUPPORTED_CCE = 0x4008,               /*!< A CCE element was found which is not supported. Most probably
                                                        the bitstream is  corrupt, or has a wrong format. */
    AAC_DEC_UNSUPPORTED_LFE = 0x4009,               /*!< A LFE element was found which is not supported. Most probably
                                                       the bitstream is corrupt, or has a wrong format. */
    AAC_DEC_UNSUPPORTED_GAIN_CONTROL_DATA = 0x400A, /*!< Gain control data found but not supported. Most probably the
                                                       bitstream is corrupt, or has a wrong format. */
    AAC_DEC_UNSUPPORTED_SBA = 0x400B,               /*!< SBA found, but currently not supported in the BSAC profile.
                                                     */
    AAC_DEC_TNS_READ_ERROR = 0x400C,                /*!< Error while reading TNS data. Most probably the bitstream is
                                                         corrupt or the system crashed. */
    AAC_DEC_RVLC_ERROR = 0x400D,                    /*!< Error while decoding error resilient data. */
    aac_dec_decode_error_end = 0x4FFF,
    aac_dec_anc_data_error_start = 0x8000,  /* Ancillary data errors. Output buffer is valid. */
    AAC_DEC_ANC_DATA_ERROR = 0x8001,        /*!< Non severe error concerning the ancillary data handling. */
    AAC_DEC_TOO_SMALL_ANC_BUFFER = 0x8002,  /*!< The registered ancillary data buffer is too small to receive the parsed data. */
    AAC_DEC_TOO_MANY_ANC_ELEMENTS = 0x8003, /*!< More than the allowed number of ancillary data elements should be written to buffer. */
    aac_dec_anc_data_error_end = 0x8FFF
} AAC_DECODER_ERROR_t;

typedef enum {
    TRANSPORTDEC_OK = 0,            /*!< All fine. */
    tpdec_sync_error_start = 0x100, /* Synchronization errors. Wait for new input data and try again. */
    TRANSPORTDEC_NOT_ENOUGH_BITS,   /*!< Out of bits. Provide more bits and try again. */
    TRANSPORTDEC_SYNC_ERROR,        /*!< No sync was found or sync got lost. Keep trying. */
    tpdec_sync_error_end,
    /* Decode errors. Mostly caused due to bit errors. */
    tpdec_decode_error_start = 0x400,
    TRANSPORTDEC_PARSE_ERROR,        /*!< Bitstream data showed inconsistencies (wrong syntax). */
    TRANSPORTDEC_UNSUPPORTED_FORMAT, /*!< Unsupported format or feature found in the bitstream data. */
    TRANSPORTDEC_CRC_ERROR,          /*!< CRC error encountered in bitstream data. */
    tpdec_decode_error_end,
    /* Fatal errors. Stop immediately on one of these errors! */
    tpdec_fatal_error_start = 0x200,
    TRANSPORTDEC_UNKOWN_ERROR,      /*!< An unknown error occured.      */
    TRANSPORTDEC_INVALID_PARAMETER, /*!< An invalid parameter was passed to a function. */
    TRANSPORTDEC_NEED_TO_RESTART,   /*!< The decoder needs to be restarted, since the requiered configuration change cannot be performed. */
    TRANSPORTDEC_TOO_MANY_BITS,     /*!< In case of packet based formats: Supplied number of bits exceed the size of the internal bit buffer. */
    tpdec_fatal_error_end
} TRANSPORTDEC_ERROR_t;

typedef enum {
    TPDEC_PARAM_MINIMIZE_DELAY = 1,    /** Delay minimization strategy. 0: none, 1: discard as many frames as possible. */
    TPDEC_PARAM_EARLY_CONFIG,          /** Enable early config discovery. */
    TPDEC_PARAM_IGNORE_BUFFERFULLNESS, /** Ignore buffer fullness. */
    TPDEC_PARAM_SET_BITRATE,           /** Set average bit rate for bit stream interruption frame misses estimation. */
    TPDEC_PARAM_RESET,                 /** Reset transport decoder instance status. */
    TPDEC_PARAM_BURST_PERIOD,          /** Set data reception burst period in mili seconds. */
    TPDEC_PARAM_TARGETLAYOUT,          /** Set CICP target layout */
    TPDEC_PARAM_FORCE_CONFIG_CHANGE,   /** Force config change for next received config */
    TPDEC_PARAM_USE_ELEM_SKIPPING
} TPDEC_PARAM;

//----------------------------------------------------------------------------------------------------------------------
/** Generic audio coder configuration structure. */
typedef struct{
    AUDIO_OBJECT_TYPE_t aot;              /**< Audio Object Type (AOT).           */
    AUDIO_OBJECT_TYPE_t extAOT;           /**< Extension Audio Object Type (SBR). */
    CHANNEL_MODE_t channelMode;           /**< Channel mode.                      */
    uint8_t channelConfigZero;            // Use channel config zero + pce although a standard channel config could be signaled.
    int32_t samplingRate;                 /**< Sampling rate.                     */
    int32_t extSamplingRate;              /**< Extended samplerate (SBR).         */
    int32_t downscaleSamplingRate;        /**< Downscale sampling rate (ELD downscaled mode)*/
    int32_t bitRate;                      /**< Average bitrate.                   */
    int32_t samplesPerFrame;              /**< Number of PCM samples per codec frame and audio channel. */
    int32_t noChannels;                   /**< Number of audio channels.          */
    int32_t bitsFrame;
    int32_t nSubFrames;                   // Amount of encoder subframes. 1 means no subframing. */
    int32_t BSACnumOfSubFrame;            // The number of the sub-frames which are grouped and
                                          // transmitted in a super-frame (BSAC). */
    int32_t BSAClayerLength;              // The average length of the large-step layers in bytes  (BSAC).
    uint32_t flags;                       // flags
    uint8_t  matrixMixdownA;              // Matrix mixdown index to put into PCE. Default value
                                          // 0 means no mixdown coefficient, valid values are 1-4
                                          // which correspond to matrix_mixdown_idx 0-3.
    uint8_t headerPeriod;                 // Frame period for sending in band configuration  buffers in the transport layer.
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
    FDK_MODULE_ID_t module_id;
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
    AACDEC_DRC_PARAM_tETER_HANDLING_t defaultPresentationMode;
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
    uint32_t dvbAncDataPosition; /* Used to store the DVB ancillary data payload position in the bitstream (only one per frame) */
    uint32_t drcPayloadPosition[MAX_DRC_THREADS]; /* Used to store the DRC payload positions in the bitstream */
    uint8_t  uniDrcPrecedence;   /* Flag for signalling that uniDrc is active and takes precedence over legacy DRC */
    uint8_t  applyExtGain;       /* Flag is 1 if extGain has to be applied, otherwise 0. */
    int32_t  additionalGainPrev; /* Gain of previous frame to be applied to the  time data */
    int32_t  additionalGainFilterState;  /* Filter state for the gain smoothing */
    int32_t  additionalGainFilterState1; /* Filter state for the gain smoothing */
} CDrcInfo_t;

typedef CDrcInfo_t *HANDLE_AAC_DRC;

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

typedef struct {
    const int16_t *ScaleFactorBands_Long;
    const int16_t *ScaleFactorBands_Short;
    uint8_t        NumberOfScaleFactorBands_Long;
    uint8_t        NumberOfScaleFactorBands_Short;
    uint32_t       samplingRateIndex;
    uint32_t       samplingRate;
} SamplingRateInfo_t;

typedef struct {
    uint8_t CommonWindow;
    uint8_t GlobalGain;
} CRawDataInfo_t;

typedef enum {
    BLOCK_LONG = 0, /* normal int32_t block */
    BLOCK_START,    /* int32_t start block */
    BLOCK_SHORT,    /* 8 int16_t blocks sequence */
    BLOCK_STOP      /* int32_t stop block*/
} BLOCK_TYPE_t;

typedef struct {
    uint8_t WindowGroupLength[8];
    uint8_t WindowGroups;
    uint8_t Valid;
    uint8_t    WindowShape;    /* 0: sine window, 1: KBD, 2: low overlap */
    BLOCK_TYPE_t WindowSequence; /* mdct.h; 0: int32_t, 1: start, 2: int16_t, 3: stop */
    uint8_t    MaxSfBands;
    uint8_t    max_sfb_ste;
    uint8_t    ScaleFactorGrouping;
    uint8_t TotalSfBands;
} CIcsInfo_t;

typedef struct {               /* joint stereo persistent memory */
  uint8_t clearSpectralCoeffs; /* indicates that the spectral coeffs must be cleared because the transform splitting active
                                flag of the left and right channel was different  */
  int32_t *scratchBuffer;      /* pointer to scratch buffer */
  int32_t *spectralCoeffs[2];  /* spectral coefficients of this channel utilized by complex stereo prediction */
  int16_t *specScale[2];
  int16_t  alpha_q_re_prev[JointStereoMaximumGroups][JointStereoMaximumBands];
  int16_t  alpha_q_im_prev[JointStereoMaximumGroups][JointStereoMaximumBands];
  uint8_t  winSeqPrev;
  uint8_t  winShapePrev;
  uint8_t  winGroupsPrev;
} CJointStereoPersistentData_t;

typedef struct {/* This struct holds the persistent data shared by both channels of a CPE. It needs to be allocated for each CPE. */
    CJointStereoPersistentData_t jointStereoPersistentData;
} CpePersistentData_t;

typedef struct {
    uint8_t pred_dir;        // 0 = prediction from mid to side channel, 1 = vice versa
    uint8_t igf_pred_dir;    // 0 = prediction from mid to side channel, 1 = vice versa
    uint8_t complex_coef;    // 0 = alpha_q_im[x] is 0 for all prediction bands, 1 =
                             // alpha_q_im[x] is transmitted via bitstream
    uint8_t use_prev_frame;  // 0 = use current frame for MDST estimation, 1 = use current and previous frame

    int16_t alpha_q_re[JointStereoMaximumGroups][JointStereoMaximumBands];
    int16_t alpha_q_im[JointStereoMaximumGroups][JointStereoMaximumBands];
} CCplxPredictionData_t;

typedef struct { /* joint stereo scratch memory (valid for this frame) */
    uint8_t MsMaskPresent;
    uint8_t MsUsed[JointStereoMaximumBands]; /*!< every arry element contains flags for up to 8 groups. this array is also utilized for complex stereo
                                              prediction. */
    uint8_t IGF_MsMaskPresent;
    uint8_t cplx_pred_flag; /* stereo complex prediction was signalled for this frame */
    uint8_t igf_cplx_pred_flag;
    /* The following array and variable are needed for the case  when INF is active */
    int32_t store_dmx_re_prev[1024];
    int16_t store_dmx_re_prev_e;
} CJointStereoData_t;

typedef union {
  /* vector representation for arithmetic */
  struct {
    int16_t re;
    int16_t im;
  } v;
  /* word representation for memory move */
  int32_t w;
}FIXP_SPK_t;

typedef union {
  /* vector representation for arithmetic */
  struct {
    int32_t re;
    int32_t im;
  } v;
  /* word representation for memory move */
  int64_t w;
}FIXP_DPK_t;

typedef struct {
    union {
        int32_t *freq;
        int32_t *time;
    } overlap; /**< Pointer to overlap memory */
    const FIXP_SPK_t *prev_wrs;  /**< pointer to previous right window slope  */
    int32_t           prev_tl;   /**< previous transform length */
    int32_t           prev_nr;   /**< previous right window offset */
    int32_t           prev_fr;   /**< previous right window slope length */
    int32_t           ov_offset; /**< overlap time data fill level */
    int32_t           ov_size;   /**< Overlap buffer size in words */
    int32_t prevAliasSymmetry;
    int32_t prevPrevAliasSymmetry;
    int32_t *pFacZir;
    int32_t *pAsymOvlp; /**< pointer to asymmetric overlap (used for stereo LPD transition) */
} mdct_t;

typedef mdct_t *H_MDCT;

typedef struct {
    int16_t m_numberLinesPrev;
    uint8_t c_prev[(1024 / 2) + 4]; /* 2-tuple context of previous frame, 4 bit */
} CArcoData_t;

typedef struct {
    int32_t  old_exc_mem[PIT_MAX_MAX + L_INTERPOL];
    int32_t  old_syn_mem[M_LP_FILTER_ORDER]; /* synthesis filter states */
    int16_t  A[M_LP_FILTER_ORDER];
    int32_t  A_exp;
    int32_t  gc_threshold;
    int32_t  de_emph_mem;
    int16_t  past_gpit;
    int32_t  past_gcode;
    uint16_t old_T0;
    uint8_t  old_T0_frac;
    int32_t  deemph_mem_wsyn;
    int32_t  wsyn_rms;
    int16_t  seed_ace;
} CAcelpStaticMem_t;

typedef struct {
    int16_t              fadeOutFactor[CONCEAL_MAX_NUM_FADE_FACTORS];
    int16_t              fadeInFactor[CONCEAL_MAX_NUM_FADE_FACTORS];
    CConcealmentMethod_t method;
    int32_t              numFadeOutFrames;
    int32_t              numFadeInFrames;
    int32_t              numMuteReleaseFrames;
    int32_t              comfortNoiseLevel;
} CConcealParams_t;

typedef struct {
    CConcealParams_t*   pConcealParams;
    FIXP_CNCL           spectralCoefficient[1024];
    int16_t             specScale[8];
    int32_t             iRandomPhase;
    int32_t             prevFrameOk[2];
    int32_t             cntValidFrames;
    int32_t             cntFadeFrames; /* State for signal fade-in/out */
    /* States for signal fade-out of frames with more than one window/subframe -
      [0] used by Update CntFadeFrames mode of CConcealment_ApplyFadeOut, [1] used by FadeOut mode */
    int32_t             winGrpOffset[2]; /* State for signal fade-out of frames with more than one window/subframe */
    int32_t             attGrpOffset[2]; /* State for faster signal fade-out of frames with transient signal parts */
    int8_t              lastRenderMode;
    uint8_t             windowShape;
    BLOCK_TYPE_t        windowSequence;
    uint8_t             lastWinGrpLen;
    CConcealmentState_t concealState;
    CConcealmentState_t concealState_old;
    int32_t             fade_old;                               /* last fading factor */
    TDfadingType_t      lastFadingType;                         /* last fading type */
    int16_t             aRvlcPreviousScaleFactor[RVLC_MAX_SFB]; /* needed once per channel */
    uint8_t             aRvlcPreviousCodebook[RVLC_MAX_SFB];    /* needed once per channel */
    int8_t              rvlcPreviousScaleFactorOK;
    int8_t              rvlcPreviousBlockType;
    int16_t             lsf4[M_LP_FILTER_ORDER];
    int32_t             last_tcx_gain;
    int32_t             last_tcx_gain_e;
    uint32_t            TDNoiseSeed;
    int32_t             TDNoiseStates[3];
    int16_t             TDNoiseCoef[3];
    int16_t             TDNoiseAtt;
} CConcealmentInfo_t;

typedef struct {  // This struct must be allocated one for every channel and must be persistent.
    int32_t     *pOverlapBuffer;
    mdct_t       IMdct;
    CArcoData_t *hArCo;
    int32_t      pnsCurrentSeed;
    /* LPD memory */
    int32_t         old_synth[PIT_MAX_MAX - L_SUBFR];
    int32_t         old_T_pf[SYN_SFD];
    int32_t         old_gain_pf[SYN_SFD];
    int32_t         mem_bpf[L_FILT + L_SUBFR];
    uint8_t         old_bpf_control_info;         /* (1: enable, 0: disable) bpf for past superframe  */
    USAC_COREMODE_t last_core_mode;               /* core mode used by the decoder in previous frame. (not signalled by the bitstream, see
                                                     CAacDecoderChannelInfo_t::core_mode_last !! ) */
    uint8_t last_lpd_mode;                        /* LPD mode used by the decoder in last LPD subframe (not signalled by the bitstream, see
                                                     CAacDecoderChannelInfo_t::lpd_mode_last !! ) */
    uint8_t last_last_lpd_mode;                   /* LPD mode used in second last LPD subframe (not signalled by the bitstream) */
    uint8_t last_lpc_lost;                        /* Flag indicating that the previous LPC is lost */
    int16_t lpc4_lsf[M_LP_FILTER_ORDER];          /* Last LPC4 coefficients in LSF domain. */
    int16_t lsf_adaptive_mean[M_LP_FILTER_ORDER]; /* Adaptive mean of LPC coefficients in LSF domain for concealment. */
    int16_t lp_coeff_old[2][M_LP_FILTER_ORDER];   /* Last LPC coefficients in LP domain. lp_coeff_old[0] is lpc4 (coeffs for right folding point of
                                                     last tcx frame), lp_coeff_old[1] are coeffs for left folding point of last tcx frame */
    int32_t  lp_coeff_old_exp[2];
    int16_t  oldStability;     /* LPC coeff stability value from last frame (required for TCX concealment). */
    uint32_t numLostLpdFrames; /* Number of consecutive lost subframes. */
    /* TCX memory */
    int32_t last_tcx_gain;
    int32_t last_tcx_gain_e;
    int32_t last_alfd_gains[32]; /* Scaled by one bit. */
    int16_t last_tcx_pitch;
    uint8_t last_tcx_noise_factor;
    /* ACELP memory */
    CAcelpStaticMem_t    acelp;
    uint32_t             nfRandomSeed; /* seed value for USAC noise filling random generator */
    CDrcChannelData_t    drcData;
    CConcealmentInfo_t   concealmentInfo;
    CpePersistentData_t *pCpeStaticData;
} CAacDecoderStaticChannelInfo_t;

typedef struct {
    int8_t  Coeff[TNS_MAXIMUM_ORDER];
    uint8_t StartBand;
    uint8_t StopBand;
    int8_t  Direction;
    int8_t  Resolution;
    uint8_t Order;
} CFilter_t;

typedef struct {
    CFilter_t Filter[TNS_MAX_WINDOWS][TNS_MAXIMUM_FILTERS];
    uint8_t NumberOfFilters[TNS_MAX_WINDOWS];
    uint8_t DataPresent;
    uint8_t Active;
    uint8_t GainLd;
} CTnsData_t;

void CTns_Reset(CTnsData_t *pTnsData);

typedef struct {
    uint8_t PulseDataPresent;
    uint8_t NumberPulse;
    uint8_t PulseStartBand;
    uint8_t PulseOffset[N_MAX_LINES];
    uint8_t PulseAmp[N_MAX_LINES];
} CPulseData_t;

typedef struct {
    /* Common bit stream data */
    int16_t        aScaleFactor[(8 * 16)]; /* Spectral scale factors for each sfb in each window. */
    int16_t        aSfbScale[(8 * 16)];    /* could be free after ApplyTools() */
    uint8_t        aCodeBook[(8 * 16)];    /* section data: codebook for each window and sfb. */
    uint8_t        band_is_noise[(8 * 16)];
    CTnsData_t     TnsData;
    CRawDataInfo_t RawDataInfo;
    union {
        struct {
            CPulseData_t PulseData;
            int16_t      aNumLineInSec4Hcr[MAX_SFB_HCR]; /* needed once for all channels except for Drm syntax */
            uint8_t      aCodeBooks4Hcr[MAX_SFB_HCR];    /* needed once for all channels except for Drm syntax. Same as "aCodeBook" ? */
            int16_t      lenOfReorderedSpectralData;
            int8_t       lenOfLongestCodeword;
            int8_t       numberSection;
            int8_t       rvlcCurrentScaleFactorOK;
            int8_t       rvlcIntensityUsed;
        } aac;
        struct {
            uint8_t fd_noise_level_and_offset;
            uint8_t tns_active;
            uint8_t tns_on_lr;
            uint8_t tcx_noise_factor[4];
            uint8_t tcx_global_gain[4];
        } usac;
    } specificTo;
} CAacDecoderDynamicData_t;

typedef union{
    uint8_t DrmBsBuffer[DRM_BS_BUFFER_SIZE];
    /* Common signal data, can be used once the bit stream data from above is not * used anymore. */
    int32_t mdctOutTemp[1024];
    int32_t synth_buf[(PIT_MAX_MAX + SYN_DELAY + L_FRAME_PLUS)];
    int32_t workBuffer[WB_SECTION_SIZE];
} CWorkBufferCore1_t;

typedef struct {
    uint8_t correlated[NO_OFBANDS];
} CPnsInterChannelData_t;

typedef struct {
    CPnsInterChannelData_t *pPnsInterChannelData;
    uint8_t                 pnsUsed[NO_OFBANDS];
    int32_t                 CurrentEnergy;
    uint8_t                 PnsActive;
    int32_t                *currentSeed;
    int32_t                *randomSeed;
} CPnsData_t;

typedef struct {
    uint32_t errorLog;
    int32_t *pQuantizedSpectralCoefficientsBase;
    int32_t  quantizedSpectralCoefficientsIdx;
    int16_t  lengthOfReorderedSpectralData;
    int16_t  numSection;
    int16_t *pNumLineInSect;
    int32_t  bitstreamAnchor;
    int8_t   lengthOfLongestCodeword;
    uint8_t *pCodebook;
} HCR_INPUT_OUTPUT_t;

typedef struct {
    const uint8_t *pMinOfCbPair;
    const uint8_t *pMaxOfCbPair;
} HCR_CB_PAIRS_t;

typedef struct {
    const uint16_t *pLargestAbsVal;
    const uint8_t  *pMaxCwLength;
    const uint8_t  *pCbDimension;
    const uint8_t  *pCbDimShift;
    const uint8_t  *pCbSign;
    const uint8_t  *pCbPriority;
} HCR_TABLE_INFO_t;

typedef struct {
    uint32_t numSegment;
    uint32_t pSegmentBitfield[((1024 >> 1) / NUMBER_OF_BIT_IN_WORD + 1)];
    uint32_t pCodewordBitfield[((1024 >> 1) / NUMBER_OF_BIT_IN_WORD + 1)];
    uint32_t segmentOffset;
    int32_t  pLeftStartOfSegment[1024 >> 1];
    int32_t  pRightStartOfSegment[1024 >> 1];
    int8_t   pRemainingBitsInSegment[1024 >> 1];
    uint8_t  readDirection;
    uint8_t  numWordForBitfield;
    uint16_t pNumBitValidInLastWord;
} HCR_SEGMENT_INFO_t;

typedef struct {
    uint32_t numCodeword;
    uint32_t numSortedSection;
    uint16_t pNumCodewordInSection[MAX_SFB_HCR];
    uint16_t pNumSortedCodewordInSection[MAX_SFB_HCR];
    uint16_t pNumExtendedSortedCodewordInSection[MAX_SFB_HCR + MAX_HCR_SETS];
    int32_t  numExtendedSortedCodewordInSectionIdx;
    uint16_t pNumExtendedSortedSectionsInSets[MAX_HCR_SETS];
    int32_t  numExtendedSortedSectionsInSetsIdx;
    uint16_t pReorderOffset[MAX_SFB_HCR];
    uint8_t  pSortedCodebook[MAX_SFB_HCR];
    uint8_t pExtendedSortedCodebook[MAX_SFB_HCR + MAX_HCR_SETS];
    int32_t extendedSortedCodebookIdx;
    uint8_t pMaxLenOfCbInExtSrtSec[MAX_SFB_HCR + MAX_HCR_SETS];
    int32_t maxLenOfCbInExtSrtSecIdx;
    uint8_t pCodebookSwitch[MAX_SFB_HCR];
} HCR_SECTION_INFO_t;

typedef struct {
    uint32_t ValidBits;
    uint32_t ReadOffset;
    uint32_t WriteOffset;
    uint32_t BitNdx;
    uint8_t *Buffer;
    uint32_t bufSize;
    uint32_t bufBits;
} FDK_BITBUF_t;

typedef FDK_BITBUF_t *HANDLE_FDK_BITBUF;

typedef struct {
    uint32_t   CacheWord;
    uint32_t   BitsInCache;
    FDK_BITBUF_t hBitBuf;
    uint32_t   ConfigCache;
} FDK_BITSTREAM_t;

typedef FDK_BITSTREAM_t *HANDLE_FDK_BITSTREAM;

typedef uint32_t (*STATEFUNC)(HANDLE_FDK_BITSTREAM, void *);

typedef struct {
    /* worst-case and 1024/4 non-PCWs exist in worst-case */
    int32_t  *pResultBase;               /* Base address for spectral data output target buffer */
    uint32_t  iNode[1024 >> 2];          /* Helper indices for code books */
    uint16_t  iResultPointer[1024 >> 2]; /* Helper indices for accessing pResultBase */
    uint32_t  pEscapeSequenceInfo[1024 >> 2];
    uint32_t  codewordOffset;
    STATEFUNC pState;
    uint8_t   pCodebook[1024 >> 2];
    uint8_t   pCntSign[1024 >> 2];
    /* this array holds the states coded as integer values within the range * [0,1,..,7] */
    int8_t pSta[1024 >> 2];
} HCR_NON_PCW_SIDEINFO_t;

typedef struct {
    HCR_INPUT_OUTPUT_t     decInOut;
    HCR_SEGMENT_INFO_t     segmentInfo;
    HCR_SECTION_INFO_t     sectionInfo;
    HCR_NON_PCW_SIDEINFO_t nonPcwSideinfo;
} CErHcrInfo_t;

typedef CErHcrInfo_t *H_HCR_INFO;

typedef struct {                                /* sideinfo of RVLC */
    /* ------- ESC 1 Data: --------- */         /* order of RVLC-bitstream components in bitstream (RVLC-initialization), every
                                                   component appears only once in bitstream */
    int32_t sf_concealment;                     /* 1          */
    int32_t rev_global_gain;                    /* 2          */
    int16_t length_of_rvlc_sf; /* 3          */ /* original value, gets modified (subtract 9) in case of noise (PNS); is kept for later use */
    int32_t dpcm_noise_nrg;                     /* 4 optional */
    int32_t sf_escapes_present;                 /* 5          */
    int16_t length_of_rvlc_escapes;             /* 6 optional */
    int32_t dpcm_noise_last_position;           /* 7 optional */
    int32_t dpcm_is_last_position;
    int16_t length_of_rvlc_sf_fwd; /* length_of_rvlc_sf used for forward decoding */
    int16_t length_of_rvlc_sf_bwd; /* length_of_rvlc_sf used for backward decoding */
    /* for RVL-Codeword decoder to distinguish between fwd and bwd decoding */
    int16_t *pRvlBitCnt_RVL;
    int32_t *pBitstrIndxRvl_RVL;
    uint8_t  numWindowGroups;
    uint8_t  maxSfbTransmitted;
    uint8_t  first_noise_group;
    uint8_t  first_noise_band;
    uint8_t  direction;
    /* bitstream indices */
    int32_t bsAnchor;             /* hcr bit buffer reference index */
    int32_t bitstreamIndexRvlFwd; /* base address of RVL-coded-scalefactor data (ESC 2) for forward  decoding */
    int32_t bitstreamIndexRvlBwd; /* base address of RVL-coded-scalefactor data (ESC 2) for backward decoding */
    int32_t bitstreamIndexEsc;    /* base address where RVLC-escapes start (ESC 2) */
    /* decoding trees */
    const uint32_t *pHuffTreeRvlCodewds;
    const uint32_t *pHuffTreeRvlcEscape;
    /* escape counters */
    uint8_t numDecodedEscapeWordsFwd; /* when decoding RVL-codes forward  */
    uint8_t numDecodedEscapeWordsBwd; /* when decoding RVL-codes backward */
    uint8_t numDecodedEscapeWordsEsc; /* when decoding the escape-Words */
    int8_t  noise_used;
    int8_t  intensity_used;
    int8_t  sf_used;
    int16_t firstScf;
    int16_t lastScf;
    int16_t firstNrg;
    int16_t lastNrg;
    int16_t firstIs;
    int16_t lastIs;
    /* ------ RVLC error detection ------ */
    uint32_t errorLogRvlc;    /* store RVLC errors  */
    int16_t  conceal_min;     /* is set at backward decoding  */
    int16_t  conceal_max;     /* is set at forward  decoding  */
    int16_t  conceal_min_esc; /* is set at backward decoding  */
    int16_t  conceal_max_esc; /* is set at forward  decoding  */
} CErRvlcInfo_t;

typedef CErRvlcInfo_t RVLC_INFO; /* temp */

/* Common data referenced by all channels */
typedef struct {
    CAacDecoderDynamicData_t pAacDecoderDynamicData[2];
    CPnsInterChannelData_t   pnsInterChannelData;
    int32_t                  pnsRandomSeed[(8 * 16)];
    CJointStereoData_t       jointStereoData; /* One for one element */
    union {
        struct {
            CErHcrInfo_t  erHcrInfo;
            CErRvlcInfo_t erRvlcInfo;
            int16_t       aRvlcScfEsc[RVLC_MAX_SFB]; /* needed once for all channels */
            int16_t       aRvlcScfFwd[RVLC_MAX_SFB]; /* needed once for all channels */
            int16_t       aRvlcScfBwd[RVLC_MAX_SFB]; /* needed once for all channels */
        } aac;
    } overlay;
} CAacDecoderCommonData_t;

typedef struct {
    CWorkBufferCore1_t    *pWorkBufferCore1;
    CCplxPredictionData_t *cplxPredictionData;
} CAacDecoderCommonStaticData_t;

typedef struct {
    uint8_t  acelp_core_mode; /**< mean excitation energy index for whole ACELP frame */
    uint8_t  mean_energy;     /**< acelp core mode for whole ACELP frame */
    uint16_t T0[NB_SUBFR];
    uint8_t  T0_frac[NB_SUBFR];
    uint8_t  ltp_filtering_flag[NB_SUBFR]; /**< controlls whether LTP postfilter is active for each ACELP subframe */
    int16_t  icb_index[NB_SUBFR][8];       /**< innovative codebook index for each ACELP subframe */
    uint8_t  gains[NB_SUBFR];              /**< gain index for each ACELP subframe */
} CAcelpChannelData_t;

/*
 This struct must be allocated one for every channel of every element and must be persistent. Among its members, the following memory areas can be
 overwritten under the given conditions:
   - pSpectralCoefficient The memory pointed to can be overwritten after time signal rendering.
   - data can be overwritten after time signal rendering.
   - pDynData memory pointed to can be overwritten after each CChannelElement_Decode() call.
   - pComData->overlay memory pointed to can be overwritten after each CChannelElement_Decode() call..
 */
typedef struct {
    union {
        struct {
            int32_t         fac_data0[LFAC];
            int8_t          fac_data_e[4];
            int32_t        *fac_data[4];    /* Pointers to unused parts of pSpectralCoefficient */
            uint8_t         core_mode;      /* current core mode */
            USAC_COREMODE_t core_mode_last; /* previous core mode, signalled in the bitstream (not done by the decoder, see
                                             CAacDecoderStaticChannelInfo::last_core_mode !!)*/
            uint8_t lpd_mode_last;          /* previous LPD mode, signalled in the bitstream (not done by the decoder, see
                                               CAacDecoderStaticChannelInfo::last_core_mode !!)*/
            uint8_t mod[4];
            uint8_t bpf_control_info;                /* (1: enable, 0: disable) bpf for current superframe */
            int16_t lsp_coeff[5][M_LP_FILTER_ORDER]; /* linear prediction coefficients in LSP domain */
            int16_t lp_coeff[5][M_LP_FILTER_ORDER];  /* linear prediction coefficients in LP domain */
            int32_t lp_coeff_exp[5];
            int16_t lsf_adaptive_mean_cand[M_LP_FILTER_ORDER]; /* concealment: is copied to CAacDecoderStaticChannelInfo->lsf_adaptive_mean once
                                                                   frame is assumed to be correct*/
            int16_t             aStability[4];                 /* LPC coeff stability values required for ACELP and TCX (concealment) */
            CAcelpChannelData_t acelp[4];
            int32_t             tcx_gain[4];
            int8_t              tcx_gain_e[4];
        } usac;
        struct {
            CPnsData_t PnsData; /* Not required for USAC */
        } aac;
    } data;
    int32_t                       *pSpectralCoefficient; /* Spectral coefficients of each window */
    int16_t                        specScale[8];         /* Scale shift values of each spectrum window */
    CIcsInfo_t                     icsInfo;
    int32_t                        granuleLength;        /* Size of smallest spectrum piece */
    uint8_t                        ElementInstanceTag;
    AACDEC_RENDER_MODE_t           renderMode;           /* Output signal rendering mode */
    CAacDecoderDynamicData_t      *pDynData;             /* Data required for one element and discarded after decoding */
    CAacDecoderCommonData_t       *pComData;             /* Data required for one channel at a time during decode */
    CAacDecoderCommonStaticData_t *pComStaticData;       /* Persistent data required for one channel at a time during decode */
    int32_t                        currAliasingSymmetry; /* required for RSVD60 MCT */
} CAacDecoderChannelInfo_t;

typedef struct {
    const int16_t *sfbOffsetLong;
    const int16_t *sfbOffsetShort;
    uint8_t        numberOfSfbLong;
    uint8_t        numberOfSfbShort;
} SFB_INFO_t;

typedef struct {
    const uint16_t (*CodeBook)[HuffmanEntries];
    uint8_t Dimension;
    uint8_t numBits;
    uint8_t Offset;
} CodeBookDescription_t;

typedef struct {
    /* PCE bitstream elements: */
    uint8_t ElementInstanceTag;
    uint8_t Profile;
    uint8_t SamplingFrequencyIndex;
    uint8_t NumFrontChannelElements;
    uint8_t NumSideChannelElements;
    uint8_t NumBackChannelElements;
    uint8_t NumLfeChannelElements;
    uint8_t NumAssocDataElements;
    uint8_t NumValidCcElements;
    uint8_t MonoMixdownPresent;
    uint8_t MonoMixdownElementNumber;
    uint8_t StereoMixdownPresent;
    uint8_t StereoMixdownElementNumber;
    uint8_t MatrixMixdownIndexPresent;
    uint8_t MatrixMixdownIndex;
    uint8_t PseudoSurroundEnable;
    uint8_t FrontElementIsCpe[PC_FSB_CHANNELS_MAX];
    uint8_t FrontElementTagSelect[PC_FSB_CHANNELS_MAX];
    uint8_t FrontElementHeightInfo[PC_FSB_CHANNELS_MAX];
    uint8_t SideElementIsCpe[PC_FSB_CHANNELS_MAX];
    uint8_t SideElementTagSelect[PC_FSB_CHANNELS_MAX];
    uint8_t SideElementHeightInfo[PC_FSB_CHANNELS_MAX];
    uint8_t BackElementIsCpe[PC_FSB_CHANNELS_MAX];
    uint8_t BackElementTagSelect[PC_FSB_CHANNELS_MAX];
    uint8_t BackElementHeightInfo[PC_FSB_CHANNELS_MAX];
    uint8_t LfeElementTagSelect[PC_LFE_CHANNELS_MAX];
    uint8_t AssocDataElementTagSelect[PC_ASSOCDATA_MAX];
    uint8_t CcElementIsIndSw[PC_CCEL_MAX];
    uint8_t ValidCcElementTagSelect[PC_CCEL_MAX];
    uint8_t CommentFieldBytes;
    uint8_t Comment[PC_COMMENTLENGTH];
    /* Helper variables for administration: */
    uint8_t isValid;              /*!< Flag showing if PCE has been read successfully. */
    uint8_t NumChannels;          /*!< Amount of audio channels summing all channel elements including LFEs */
    uint8_t NumEffectiveChannels; /*!< Amount of audio channels summing only SCEs and CPEs */
    uint8_t elCounter;
} CProgramConfig_t;

typedef struct {
    USAC_EXT_ELEMENT_TYPE_t usacExtElementType;
    uint16_t                usacExtElementConfigLength;
    uint16_t                usacExtElementDefaultLength;
    uint8_t                 usacExtElementPayloadFrag;
    uint8_t                 usacExtElementHasAudioPreRoll;
} CSUsacExtElementConfig_t;

typedef struct {
    MP4_ELEMENT_ID_t         usacElementType;
    uint8_t                  m_noiseFilling;
    uint8_t                  m_harmonicSBR;
    uint8_t                  m_interTes;
    uint8_t                  m_pvc;
    uint8_t                  m_stereoConfigIndex;
    CSUsacExtElementConfig_t extElement;
} CSUsacElementConfig;

typedef struct {
    uint8_t             m_frameLengthFlag;
    uint8_t             m_coreSbrFrameLengthIndex;
    uint8_t             m_sbrRatioIndex;
    uint8_t             m_nUsacChannels; /* number of audio channels signaled in  UsacDecoderConfig() / rsv603daDecoderConfig() */
    uint8_t             m_channelConfigurationIndex;
    uint32_t            m_usacNumElements;
    CSUsacElementConfig element[TP_USAC_MAX_ELEMENTS];
    uint8_t             numAudioChannels;
    uint8_t             m_usacConfigExtensionPresent;
    uint8_t             elementLengthPresent;
    uint8_t             UsacConfig[TP_USAC_MAX_CONFIG_LEN];
    uint16_t            UsacConfigBits;
} CSUsacConfig_t;

typedef struct {
    uint8_t  m_frameLengthFlag;
    uint8_t  m_sbrPresentFlag;
    uint8_t  m_useLdQmfTimeAlign; /* Use LD-MPS QMF in SBR to achive time alignment */
    uint8_t  m_sbrSamplingRate;
    uint8_t  m_sbrCrcFlag;
    uint32_t m_downscaledSamplingFrequency;
} CSEldSpecificConfig_t;

typedef struct {
    uint32_t m_frameLengthFlag;
    uint32_t m_dependsOnCoreCoder;
    uint32_t m_coreCoderDelay;
    uint32_t m_extensionFlag;
    uint32_t m_extensionFlag3;
    uint32_t m_layer;
    uint32_t m_numOfSubFrame;
    uint32_t m_layerLength;
} CSGaSpecificConfig_t;

typedef struct {
    /* XYZ Specific Data */
    union {
        CSGaSpecificConfig_t  m_gaSpecificConfig;  /**< General audio specific configuration. */
        CSEldSpecificConfig_t m_eldSpecificConfig; /**< ELD specific configuration. */
        CSUsacConfig_t        m_usacConfig;        /**< USAC specific configuration               */
    } m_sc;
    /* Common ASC parameters */
    CProgramConfig_t      m_progrConfigElement;              /**< Program configuration. */
    AUDIO_OBJECT_TYPE_t m_aot;                             /**< Audio Object Type.  */
    uint32_t            m_samplingFrequency;               /**< Samplerate. */
    uint32_t            m_samplesPerFrame;                 /**< Amount of samples per frame.   */
    uint32_t            m_directMapping;                   /**< Document this please !!                         */
    AUDIO_OBJECT_TYPE_t m_extensionAudioObjectType;        /**< Audio object type */
    uint32_t            m_extensionSamplingFrequency;      /**< Samplerate            */
    int8_t              m_channelConfiguration;            /**< Channel configuration index */
    int8_t              m_epConfig;                        /**< Error protection index                           */
    int8_t              m_vcb11Flag;                       /**< aacSectionDataResilienceFlag                     */
    int8_t              m_rvlcFlag;                        /**< aacScalefactorDataResilienceFlag                 */
    int8_t              m_hcrFlag;                         /**< aacSpectralDataResilienceFlag                    */
    int8_t              m_sbrPresentFlag;                  /**< Flag indicating the presence of SBR data in the bitstream */
    int8_t              m_psPresentFlag;                   /**< Flag indicating the presence of parametric stereo data in the bitstream */
    uint8_t             m_samplingFrequencyIndex;          /**< Samplerate index          */
    uint8_t             m_extensionSamplingFrequencyIndex; /**< Samplerate index */
    int8_t              m_extensionChannelConfiguration;   /**< Channel configuration index   */
    uint8_t             configMode; /**< The flag indicates if the callback shall work in memory allocation mode or in config change detection mode */
    uint8_t  AacConfigChanged; /**< The flag will be set if at least one aac config parameter has changed that requires a memory reconfiguration */
    uint8_t  SbrConfigChanged; /**< The flag will be set if at least one sbr config parameter has changed that requires a memory reconfiguration */
    uint8_t  SacConfigChanged; /**< The flag will be set if at least one sac config  parameter has changed that requires a memory reconfiguration */
    uint8_t  config[TP_USAC_MAX_CONFIG_LEN]; /**< Configuration stored as bitstream */
    uint32_t configBits;                     /**< Configuration length in bits */
} CSAudioSpecificConfig_t;

typedef struct {
    int8_t  flushCnt;       /**< Flush frame counter */
    uint8_t flushStatus;    /**< Flag indicates flush mode: on|off */
    int8_t  buildUpCnt;     /**< Build up frame counter */
    uint8_t buildUpStatus;  /**< Flag indicates build up mode: on|off */
    uint8_t cfgChanged;     /**< Flag indicates that the config changed and the decoder
                             needs to be initialized again via callback. Make sure
                             that memory is freed before initialization. */
    uint8_t contentChanged; /**< Flag indicates that the content changed i.e. a
                             right truncation occured before */
    uint8_t forceCfgChange; /**< Flag indicates if config change has to be forced
                             even if new config is the same */
} CCtrlCFGChange_t;

typedef int32_t (*cbUpdateConfig_t)(void *, const CSAudioSpecificConfig_t *, const uint8_t configMode, uint8_t *configChanged);
typedef int32_t (*cbFreeMem_t)(void *, const CSAudioSpecificConfig_t *);
typedef int32_t (*cbCtrlCFGChange_t)(void *, const CCtrlCFGChange_t *);
typedef int32_t (*cbSsc_t)(void *, HANDLE_FDK_BITSTREAM, const AUDIO_OBJECT_TYPE_t coreCodec, const int32_t samplingRate, const int32_t frameSize,
                           const int32_t stereoConfigIndex, const int32_t coreSbrFrameLengthIndex, const int32_t configBytes,
                           const uint8_t configMode, uint8_t *configChanged);

typedef int32_t (*cbSbr_t)(void *self, HANDLE_FDK_BITSTREAM hBs, const int32_t sampleRateIn, const int32_t sampleRateOut,
                           const int32_t samplesPerFrame, const AUDIO_OBJECT_TYPE_t coreCodec, const MP4_ELEMENT_ID_t elementID,
                           const int32_t elementIndex, const uint8_t harmonicSbr, const uint8_t stereoConfigIndex, const uint8_t configMode,
                           uint8_t *configChanged, const int32_t downscaleFactor);

typedef int32_t (*cbUsac_t)(void *self, HANDLE_FDK_BITSTREAM hBs);

typedef int32_t (*cbUniDrc_t)(void *self, HANDLE_FDK_BITSTREAM hBs, const int32_t fullPayloadLength, const int32_t payloadType,
                              const int32_t subStreamIndex, const int32_t payloadStart, const AUDIO_OBJECT_TYPE_t);

typedef struct {
    cbUpdateConfig_t cbUpdateConfig;   /*!< Function pointer for Config change
                                          notify callback.  */
    void *cbUpdateConfigData;          /*!< User data pointer for Config change notify
                                          callback. */
    cbFreeMem_t       cbFreeMem;       /*!< Function pointer for free memory callback.  */
    void             *cbFreeMemData;   /*!< User data pointer for free memory callback. */
    cbCtrlCFGChange_t cbCtrlCFGChange; /*!< Function pointer for config change
                                          control callback. */
    void *cbCtrlCFGChangeData;         /*!< User data pointer for config change control
                                          callback. */
    cbSsc_t    cbSsc;                  /*!< Function pointer for SSC parser callback. */
    void      *cbSscData;              /*!< User data pointer for SSC parser callback. */
    cbSbr_t    cbSbr;                  /*!< Function pointer for SBR header parser callback. */
    void      *cbSbrData;              /*!< User data pointer for SBR header parser callback. */
    cbUsac_t   cbUsac;
    void      *cbUsacData;
    cbUniDrc_t cbUniDrc; /*!< Function pointer for uniDrcConfig and
                            loudnessInfoSet parser callback. */
    void *cbUniDrcData;  /*!< User data pointer for uniDrcConfig and
                            loudnessInfoSet parser callback. */
} CSTpCallBacks_t;

//----------------------------------------------------------------------------------------------------------------------------------------------------

// prototypes

uint8_t FDK_chMapDescr_getMapValue(const FDK_channelMapDescr_t *const pMapDescr, const uint8_t chIdx, const uint32_t mapIdx);
int32_t FDK_chMapDescr_setPassThrough(FDK_channelMapDescr_t *const pMapDescr, uint32_t fPassThrough);
int32_t FDK_chMapDescr_isValid(const FDK_channelMapDescr_t *const pMapDescr);
void    FDK_chMapDescr_init(FDK_channelMapDescr_t *const pMapDescr, const CHANNEL_MAP_INFO_t *const pMapInfoTab, const uint32_t mapInfoTabLen,
                            const uint32_t fPassThrough);

void       *FDKcalloc(const uint32_t n, const uint32_t size);
void       *FDKmalloc(const uint32_t size);
void       *FDKaalloc(const uint32_t size, const uint32_t alignment);
void        FDKafree(void *ptr);
void       *FDKcalloc_L(const uint32_t n, const uint32_t size, MEMORY_SECTION_t s);
void       *FDKaalloc_L(const uint32_t size, const uint32_t alignment, MEMORY_SECTION_t s);
void        FDKfree_L(void *ptr);
void        FDKafree_L(void *ptr);
static void equalizeFiltBufferExp(int32_t *filtBuffer, int8_t *filtBuffer_e, int32_t *NrgGain, int8_t *NrgGain_e, int32_t subbands);
static void calcNrgPerSubband(int32_t **analysBufferReal, int32_t **analysBufferImag, int32_t lowSubband, int32_t highSubband, int32_t start_pos,
                              int32_t next_pos, int8_t frameExp, int32_t *nrgEst, int8_t *nrgEst_e);
static void calcNrgPerSfb(int32_t **analysBufferReal, int32_t **analysBufferImag, int32_t nSfb, uint8_t *freqBandTable, int32_t start_pos,
                          int32_t next_pos, int8_t input_e, int32_t *nrg_est, int8_t *nrg_est_e);
static void calcSubbandGain(int32_t nrgRef, int8_t nrgRef_e, ENV_CALC_NRGS_t *nrgs, int32_t c, int32_t tmpNoise, int8_t tmpNoise_e,
                            uint8_t sinePresentFlag, uint8_t sineMapped, int32_t noNoiseFlag);
static void calcAvgGain(ENV_CALC_NRGS_t *nrgs, int32_t lowSubband, int32_t highSubband, int32_t *sumRef_m, int8_t *sumRef_e, int32_t *ptrAvgGain_m,
                        int8_t *ptrAvgGain_e);
static void adjustTimeSlot_EldGrid(int32_t *ptrReal, ENV_CALC_NRGS_t *nrgs, uint8_t *ptrHarmIndex, int32_t lowSubbands, int32_t noSubbands,
                                   int32_t scale_change, int32_t noNoiseFlag, int32_t *ptrPhaseIndex, int32_t scale_diff_low);
static void adjustTimeSlotLC(int32_t *ptrReal, ENV_CALC_NRGS_t *nrgs, uint8_t *ptrHarmIndex, int32_t lowSubbands, int32_t noSubbands,
                             int32_t scale_change, int32_t noNoiseFlag, int32_t *ptrPhaseIndex);
AAC_DECODER_ERROR_t CLpdChannelStream_Read(HANDLE_FDK_BITSTREAM hBs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                                         CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo, const SamplingRateInfo_t *pSamplingRateInfo,
                                         uint32_t flags);
void CLpdChannelStream_Decode(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
                                           uint32_t flags);
AAC_DECODER_ERROR_t CLpd_RenderTimeSignal(CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
                                          CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, int32_t *pTimeData, int32_t samplesPerFrame,
                                          SamplingRateInfo_t *pSamplingRateInfo, uint32_t frameOk, const int32_t aacOutDataHeadroom, uint32_t flags,
                                          uint32_t strmFlags);
void FDK_CreateBitBuffer(HANDLE_FDK_BITBUF *hBitBuffer, uint8_t *pBuffer, uint32_t bufSize);
void FDK_InitBitBuffer(HANDLE_FDK_BITBUF hBitBuffer, uint8_t *pBuffer, uint32_t bufSize, uint32_t validBits);
void FDK_ResetBitBuffer(HANDLE_FDK_BITBUF hBitBuffer);
void FDK_DeleteBitBuffer(HANDLE_FDK_BITBUF hBitBuffer);
int32_t FDK_get(HANDLE_FDK_BITBUF hBitBuffer, const uint32_t numberOfBits);
int32_t FDK_get32(HANDLE_FDK_BITBUF hBitBuf);
void FDK_put(HANDLE_FDK_BITBUF hBitBuffer, uint32_t value, const uint32_t numberOfBits);
int32_t FDK_getBwd(HANDLE_FDK_BITBUF hBitBuffer, const uint32_t numberOfBits);
void FDK_putBwd(HANDLE_FDK_BITBUF hBitBuffer, uint32_t value, const uint32_t numberOfBits);
void FDK_pushBack(HANDLE_FDK_BITBUF hBitBuffer, const uint32_t numberOfBits, uint8_t config);
void FDK_pushForward(HANDLE_FDK_BITBUF hBitBuffer, const uint32_t numberOfBits,  uint8_t config);
uint32_t FDK_getValidBits(HANDLE_FDK_BITBUF hBitBuffer);
int32_t FDK_getFreeBits(HANDLE_FDK_BITBUF hBitBuffer);
void FDK_Feed(HANDLE_FDK_BITBUF hBitBuffer, const uint8_t inputBuffer[], const uint32_t bufferSize, uint32_t *bytesValid);
void FDK_Copy(HANDLE_FDK_BITBUF hBitBufDst, HANDLE_FDK_BITBUF hBitBufSrc, uint32_t *bytesValid);
void FDK_Fetch(HANDLE_FDK_BITBUF hBitBuffer, uint8_t outBuf[], uint32_t *writeBytes);
void                aacDecoder_drcReset(HANDLE_AAC_DRC self);
void                aacDecoder_drcInit(HANDLE_AAC_DRC self);
void                aacDecoder_drcInitChannelData(CDrcChannelData_t *pDrcChannel);
AAC_DECODER_ERROR_t aacDecoder_drcSetParam(HANDLE_AAC_DRC self, AACDEC_DRC_PARAM_t param, int32_t value);
int32_t             aacDecoder_drcMarkPayload(HANDLE_AAC_DRC self, HANDLE_FDK_BITSTREAM hBs, AACDEC_DRC_PAYLOAD_TYPE_t type);
int32_t aacDecoder_drcProlog(HANDLE_AAC_DRC self, HANDLE_FDK_BITSTREAM hBs, CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo[],
                             uint8_t pceInstanceTag, uint8_t channelMapping[], int32_t validChannels);
void    aacDecoder_drcApply(HANDLE_AAC_DRC self, void *pSbrDec, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, CDrcChannelData_t *pDrcDat,
                            int32_t *extGain, int32_t ch, int32_t aacFrameSize, int32_t bSbrPresent);
int32_t aacDecoder_drcEpilog(HANDLE_AAC_DRC self, HANDLE_FDK_BITSTREAM hBs, CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo[],
                             uint8_t pceInstanceTag, uint8_t channelMapping[], int32_t validChannels);
void aacDecoder_drcGetInfo(HANDLE_AAC_DRC self, int8_t *pPresMode, int8_t *pProgRefLevel);
int32_t applyDrcLevelNormalization(HANDLE_AAC_DRC hDrcInfo, int32_t *samplesIn, int32_t *pGain, int32_t *pGainPerSample, const int32_t gain_scale,
                                   const uint32_t gain_delay, const uint32_t nSamples, const uint32_t channels, const uint32_t stride,
                                   const uint32_t limiterEnabled);
uint32_t Hcr_State_BODY_ONLY(HANDLE_FDK_BITSTREAM, void*);
uint32_t Hcr_State_BODY_SIGN__BODY(HANDLE_FDK_BITSTREAM, void*);
uint32_t Hcr_State_BODY_SIGN__SIGN(HANDLE_FDK_BITSTREAM, void*);
uint32_t Hcr_State_BODY_SIGN_ESC__BODY(HANDLE_FDK_BITSTREAM, void*);
uint32_t Hcr_State_BODY_SIGN_ESC__SIGN(HANDLE_FDK_BITSTREAM, void*);
uint32_t Hcr_State_BODY_SIGN_ESC__ESC_PREFIX(HANDLE_FDK_BITSTREAM, void*);
uint32_t Hcr_State_BODY_SIGN_ESC__ESC_WORD(HANDLE_FDK_BITSTREAM, void*);
void CPns_UpdateNoiseState(CPnsData_t *pPnsData, int32_t *currentSeed, int32_t *randomSeed);
void CPns_ResetData(CPnsData_t *pPnsData, CPnsInterChannelData_t *pPnsInterChannelData);
void CProgramConfig_Reset(CProgramConfig_t *pPce);
void CProgramConfig_Init(CProgramConfig_t *pPce);
int32_t CProgramConfig_IsValid(const CProgramConfig_t *pPce);
void CProgramConfig_Read(CProgramConfig_t *pPce, HANDLE_FDK_BITSTREAM bs, uint32_t alignAnchor);
int32_t CProgramConfig_Compare(const CProgramConfig_t *const pPce1, const CProgramConfig_t *const pPce2);
void CProgramConfig_GetDefault(CProgramConfig_t *pPce, const uint32_t channelConfig);
int32_t CProgramConfig_LookupElement(CProgramConfig_t *pPce, uint32_t chConfig, const uint32_t tag, const uint32_t channelIdx, uint8_t chMapping[],
                                     AUDIO_CHANNEL_TYPE_t chType[], uint8_t chIndex[], const uint32_t chDescrLen, uint8_t *elMapping,
                                     MP4_ELEMENT_ID_t elList[], MP4_ELEMENT_ID_t elType);
int32_t CProgramConfig_GetPceChMap(const CProgramConfig_t *pPce, uint8_t pceChMap[], const uint32_t pceChMapLen);
int32_t CProgramConfig_GetElementTable(const CProgramConfig_t *pPce, MP4_ELEMENT_ID_t table[], const int32_t elListSize, uint8_t *pChMapIdx);
void CProgramConfig_GetChannelDescription(const uint32_t chConfig, const CProgramConfig_t *pPce, AUDIO_CHANNEL_TYPE_t chType[], uint8_t chIndex[]);
void AudioSpecificConfig_Init(CSAudioSpecificConfig_t *pAsc);
TRANSPORTDEC_ERROR_t AudioSpecificConfig_Parse(CSAudioSpecificConfig_t *pAsc, HANDLE_FDK_BITSTREAM hBs, int32_t fExplicitBackwardCompatible,
                                             CSTpCallBacks_t *cb, uint8_t configMode, uint8_t configChanged, AUDIO_OBJECT_TYPE_t m_aot);
uint8_t ToggleReadDirection(uint8_t readDirection);
uint32_t HcrGetABitFromBitstream(HANDLE_FDK_BITSTREAM bs, const int32_t bsAnchor, int32_t *pLeftStartOfSegment, int32_t *pRightStartOfSegment,
                                 uint8_t readDirection);
int32_t InvMdctTransformLowDelay_fdk(int32_t *mdctdata_m, const int32_t mdctdata_e, int32_t *mdctOut, int32_t *fs_buffer, const int32_t frameLength);
void CPns_Read(CPnsData_t *pPnsData, HANDLE_FDK_BITSTREAM bs, const CodeBookDescription_t *hcb, int16_t *pScaleFactor, uint8_t global_gain,
               int32_t band, int32_t group);
void CPns_Apply(const CPnsData_t *pPnsData, const CIcsInfo_t *pIcsInfo, int32_t *pSpectrum, const int16_t *pSpecScale, const int16_t *pScaleFactor,
                const SamplingRateInfo_t *pSamplingRateInfo, const int32_t granuleLength, const int32_t channel);
void CBlock_ApplyNoise(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, SamplingRateInfo_t *pSamplingRateInfo, uint32_t *nfRandomSeed,
                       uint8_t *band_is_noise);
void CTns_ReadDataPresentFlag(HANDLE_FDK_BITSTREAM bs, CTnsData_t *pTnsData);

void CTns_ReadDataPresentUsac(HANDLE_FDK_BITSTREAM hBs, CTnsData_t *pTnsData0, CTnsData_t *pTnsData1, uint8_t *ptns_on_lr, const CIcsInfo_t *pIcsInfo,
                              const uint32_t flags, const uint32_t elFlags, const int32_t fCommonWindow);
AAC_DECODER_ERROR_t CTns_Read(HANDLE_FDK_BITSTREAM bs, CTnsData_t *pTnsData, const CIcsInfo_t *pIcsInfo, const uint32_t flags);
void                CTns_Apply(CTnsData_t       *pTnsData, /*!< pointer to aac decoder info */
                               const CIcsInfo_t *pIcsInfo, int32_t *pSpectralCoefficient, const SamplingRateInfo_t *pSamplingRateInfo, const int32_t granuleLength,
                               const uint8_t nbands, const uint8_t igf_active, const uint32_t flags);
int32_t             CBlock_GetEscape(HANDLE_FDK_BITSTREAM bs, const int32_t q);
AAC_DECODER_ERROR_t CBlock_ReadScaleFactorData(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, HANDLE_FDK_BITSTREAM bs, const uint32_t flags);
AAC_DECODER_ERROR_t CBlock_ReadSpectralData(HANDLE_FDK_BITSTREAM bs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                                            const SamplingRateInfo_t *pSamplingRateInfo, const uint32_t flags);
AAC_DECODER_ERROR_t CBlock_ReadAcSpectralData(HANDLE_FDK_BITSTREAM hBs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                                              CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
                                              const SamplingRateInfo_t *pSamplingRateInfo, const uint32_t frame_length, const uint32_t flags);
AAC_DECODER_ERROR_t CBlock_ReadSectionData(HANDLE_FDK_BITSTREAM bs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                                           const SamplingRateInfo_t *pSamplingRateInfo, const uint32_t flags);
void CBlock_ScaleSpectralData(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, uint8_t maxSfbs, SamplingRateInfo_t *pSamplingRateInfo);
void ApplyTools(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo[], const SamplingRateInfo_t *pSamplingRateInfo, const uint32_t flags,
                const uint32_t elFlags, const int32_t channel, const int32_t maybe_jstereo);
void CBlock_FrequencyToTime(CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                            int32_t outSamples[], const int16_t frameLen, const int32_t frameOk, int32_t *pWorkBuffer1,
                            const int32_t aacOutDataHeadroom, uint32_t elFlags, int32_t elCh);
void CBlock_FrequencyToTimeLowDelay(CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                                    int32_t outSamples[], const int16_t frameLen);
AAC_DECODER_ERROR_t CBlock_InverseQuantizeSpectralData(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, SamplingRateInfo_t *pSamplingRateInfo,
                                                       uint8_t *band_is_noise, uint8_t active_band_search);
int32_t CPulseData_Read(const HANDLE_FDK_BITSTREAM bs, CPulseData_t *const PulseData, const int16_t *sfb_startlines, const void *pIcsInfo,
                        const int16_t frame_length);
void    CPulseData_Apply(CPulseData_t *PulseData, const int16_t *pScaleFactorBandOffsets, int32_t *coef);
uint8_t             rvlcReadBitFromBitstream(HANDLE_FDK_BITSTREAM bs, const int32_t bsAnchor, int32_t *pPosition, uint8_t readDirection);
void BidirectionalEstimation_UseLowerScfOfCurrentFrame(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo);
void BidirectionalEstimation_UseScfOfPrevFrameAsReference(CAacDecoderChannelInfo_t       *pAacDecoderChannelInfo,
                                                          CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo);
void StatisticalEstimation(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo);
void PredictiveInterpolation(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo);
int32_t CJointStereo_Read(HANDLE_FDK_BITSTREAM bs, CJointStereoData_t *pJointStereoData, const int32_t windowGroups,
                          const int32_t scaleFactorBandsTransmitted, const int32_t max_sfb_ste_clear,
                          CJointStereoPersistentData_t *pJointStereoPersistentData, CCplxPredictionData_t *cplxPredictionData,
                          int32_t cplxPredictionActiv, int32_t scaleFactorBandsTotal, int32_t windowSequence, const uint32_t flags);



//----------------------------------------------------------------------------------------------------------------------
//          I N L I N E S
//----------------------------------------------------------------------------------------------------------------------
/** Initialize library info. */
static inline void FDKinitLibInfo(LIB_INFO_t* info) {
    int32_t i;
    for(i = 0; i < FDK_MODULE_LAST; i++) { info[i].module_id = FDK_NONE; }
}

/** Aquire supported features of library. */
static inline uint32_t FDKlibInfo_getCapabilities(const LIB_INFO_t* info, FDK_MODULE_ID_t module_id) {
    int32_t i;
    for(i = 0; i < FDK_MODULE_LAST; i++) {
        if(info[i].module_id == module_id) { return info[i].flags; }
    }
    return 0;
}

/** Search for next free tab. */
static inline int32_t FDKlibInfo_lookup(const LIB_INFO_t* info, FDK_MODULE_ID_t module_id) {
    int32_t i = -1;
    for(i = 0; i < FDK_MODULE_LAST; i++) {
        if(info[i].module_id == module_id) return -1;
        if(info[i].module_id == FDK_NONE) break;
    }
    if(i == FDK_MODULE_LAST) return -1;
    return i;
}

static inline int32_t CLpd_FAC_getLength(int32_t fNotShortBlock, int32_t fac_length_long) {
    if(fNotShortBlock) { return (fac_length_long); }
    else { return fac_length_long / 2; }
}