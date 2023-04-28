

/******************* Library for basic calculation routines ********************

   Author(s):   Markus Lohwasser

   Description: FDK Tools Decorrelator

*******************************************************************************/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "FDK_decorrelate.h"

#define PC_NUM_BANDS     (8)
#define PC_NUM_HYB_BANDS (PC_NUM_BANDS - 3 + 10)

#define DUCK_ALPHA  (0.8f)
#define DUCK_GAMMA  (1.5f)
#define ABS_THR     (1e-9f * 32768 * 32768)
#define ABS_THR_FDK ((int32_t)1)

#define DECORR_ZERO_PADDING 0

#define DECORR_FILTER_ORDER_BAND_0_MPS (20)
#define DECORR_FILTER_ORDER_BAND_1_MPS (15)
#define DECORR_FILTER_ORDER_BAND_2_MPS (6)
#define DECORR_FILTER_ORDER_BAND_3_MPS (3)

#define DECORR_FILTER_ORDER_BAND_0_USAC (10)
#define DECORR_FILTER_ORDER_BAND_1_USAC (8)
#define DECORR_FILTER_ORDER_BAND_2_USAC (3)
#define DECORR_FILTER_ORDER_BAND_3_USAC (2)

#define DECORR_FILTER_ORDER_BAND_0_LD (0)
#define DECORR_FILTER_ORDER_BAND_1_LD (DECORR_FILTER_ORDER_BAND_1_MPS)
#define DECORR_FILTER_ORDER_BAND_2_LD (DECORR_FILTER_ORDER_BAND_2_MPS)
#define DECORR_FILTER_ORDER_BAND_3_LD (DECORR_FILTER_ORDER_BAND_3_MPS)

#define MAX_DECORR_SEED_MPS (5) /* 4 is worst case for 7272 mode for low power */
                                /* 5 is worst case for 7271 and 7272 mode for high quality */
#define MAX_DECORR_SEED_USAC (1)
#define MAX_DECORR_SEED_LD   (4)

#define DECORR_FILTER_ORDER_PS (12)
#define NUM_DECORR_CONFIGS     (3) /* different configs defined by bsDecorrConfig bitstream field */

/* REV_bandOffset_... tables map (hybrid) bands to the corresponding reverb
   bands. Within each reverb band the same processing is applied. Instead of QMF
   split frequencies the corresponding hybrid band offsets are stored directly
 */
static const uint8_t REV_bandOffset_MPS_HQ[NUM_DECORR_CONFIGS][(4)] = {
    {8, 21, 30, 71}, {8, 56, 71, 71}, {0, 21, 71, 71}};
/* REV_bandOffset_USAC[] are equivalent to REV_bandOffset_MPS_HQ */
static const uint8_t REV_bandOffset_PS_HQ[(4)] = {30, 42, 71, 71};
static const uint8_t REV_bandOffset_PS_LP[(4)] = {14, 42, 71, 71};
static const uint8_t REV_bandOffset_LD[NUM_DECORR_CONFIGS][(4)] = {{0, 14, 23, 64}, {0, 49, 64, 64}, {0, 14, 64, 64}};

/* REV_delay_... tables define the number of delay elements within each reverb
 * band */
/* REV_filterOrder_... tables define the filter order within each reverb band */
static const uint8_t REV_delay_MPS[(4)] = {8, 7, 2, 1};
static const int8_t  REV_filterOrder_MPS[(4)] = {DECORR_FILTER_ORDER_BAND_0_MPS, DECORR_FILTER_ORDER_BAND_1_MPS,
                                                 DECORR_FILTER_ORDER_BAND_2_MPS, DECORR_FILTER_ORDER_BAND_3_MPS};
static const uint8_t REV_delay_PS_HQ[(4)] = {2, 14, 1, 0};
static const uint8_t REV_delay_PS_LP[(4)] = {8, 14, 1, 0};
static const int8_t  REV_filterOrder_PS[(4)] = {DECORR_FILTER_ORDER_PS, -1, -1, -1};
static const uint8_t REV_delay_USAC[(4)] = {11, 10, 5, 2};
static const int8_t  REV_filterOrder_USAC[(4)] = {DECORR_FILTER_ORDER_BAND_0_USAC, DECORR_FILTER_ORDER_BAND_1_USAC,
                                                  DECORR_FILTER_ORDER_BAND_2_USAC, DECORR_FILTER_ORDER_BAND_3_USAC};

/* REV_filtType_... tables define the type of processing (filtering with
   different properties or pure delay) done in each reverb band. This is mapped
   to specialized routines. */
static const REVBAND_FILT_TYPE REV_filtType_MPS[(4)] = {COMMON_REAL, COMMON_REAL, COMMON_REAL, COMMON_REAL};

static const REVBAND_FILT_TYPE REV_filtType_PS[(4)] = {INDEP_CPLX_PS, DELAY, DELAY, NOT_EXIST};

/* initialization values of ring buffer offsets for the 3 concatenated allpass
 * filters (PS type decorrelator). */
static const uint8_t stateBufferOffsetInit[(3)] = {0, 6, 14};

static const REVBAND_FILT_TYPE REV_filtType_LD[(4)] = {NOT_EXIST, COMMON_REAL, COMMON_REAL, COMMON_REAL};

/*** mapping of hybrid bands to processing (/parameter?) bands ***/
/* table for PS chc running in legacy PS decoder. */
static const uint8_t kernels_20_to_71_PS[(71) + 1] = {
    0,  0,  1,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 14, 15, 15, 15, 16, 16, 16,
    16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19};

/*** mapping of processing (/parameter?) bands to hybrid bands ***/
/* table for PS chc running in legacy PS decoder. */
static const uint8_t kernels_20_to_71_offset_PS[(20) + 1] = {0,  2,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                                                             13, 14, 15, 16, 18, 21, 25, 30, 42, 71};

static const uint8_t kernels_28_to_71[(71) + 1] = {
    0,  0,  1,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 17, 18, 18, 19,
    19, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25,
    26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27};

static const uint8_t kernels_28_to_71_offset[(28) + 1] = {0,  2,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                                                          17, 18, 19, 21, 23, 25, 27, 30, 33, 37, 42, 48, 55, 71};

/* LD-MPS defined in SAOC standart (mapping qmf -> param bands)*/
static const uint8_t kernels_23_to_64[(64) + 1] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16,
    16, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 21, 21, 21,
    21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
};

static const uint8_t kernels_23_to_64_offset[(23) + 1] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                                                          12, 14, 16, 18, 20, 23, 26, 30, 35, 41, 48, 64};

static inline int32_t SpatialDecGetProcessingBand(int32_t hybridBand, const uint8_t *tab) { return tab[hybridBand]; }

/* helper inline function */
static inline int32_t SpatialDecGetQmfBand(int32_t paramBand, const uint8_t *tab) { return (int32_t)tab[paramBand]; }

#define DUCKER_MAX_NRG_SCALE (24)
#define DUCKER_HEADROOM_BITS (2)

#define FILTER_SF (2)

#define FIXP_DUCK_GAIN      int16_t

#define PS_DUCK_PEAK_DECAY_FACTOR     (0.765928338364649f)
#define PS_DUCK_FILTER_COEFF          (0.25f)
#define DUCK_ALPHA_FDK                FL2FXCONST_SGL(DUCK_ALPHA)
#define DUCK_ONE_MINUS_ALPHA_X4_FDK   FL2FXCONST_SGL(4.0f * (1.0f - DUCK_ALPHA))
#define DUCK_GAMMA_FDK                FL2FXCONST_SGL(DUCK_GAMMA / 2)
#define PS_DUCK_PEAK_DECAY_FACTOR_FDK FL2FXCONST_SGL(PS_DUCK_PEAK_DECAY_FACTOR)
#define PS_DUCK_FILTER_COEFF_FDK      FL2FXCONST_SGL(PS_DUCK_FILTER_COEFF)

const FIXP_SPK_t DecorrPsCoeffsCplx[][4] = {{23913, 22403, -21043, 3587, -17708, 5372, -15893, 2179},
                                          {23913, -22403, -21043, -3587, -17708, -5372, -15893, -2179},
                                          {-20798, 25322, -18665, 10357, -11739, 14304, -14719, 6377},
                                          {-20798, -25322, -18665, -10357, -11739, -14304, -14719, -6377},
                                          {-26623, 19104, -14179, -15957, -1814, -18416, -12460, -10103},
                                          {17329, 27811, -8091, -19753, 8723, -16320, -9280, -13084},
                                          {10492, -31043, 2509, -21198, 18149, -3610, -3315, -15695},
                                          {-14439, 29415, 15212, -14975, 10281, 15386, 5303, -15139},
                                          {18141, -27288, 21235, -2176, -10281, 15386, 12384, -10196},
                                          {-21534, 24698, 17933, 11579, -18149, -3610, 15876, -2298},
                                          {-20043, -25923, -335, 21344, 7081, -17096, 12535, 10010},
                                          {-12682, -30214, -19858, 4112, 6727, 16241, -2926, 14956},
                                          {-4464, -32462, -7906, -17510, -15386, -6373, -13845, 4093},
                                          {4056, -32516, 14510, -10894, 14532, -6019, -9473, -9807},
                                          {12302, -30371, 12986, 11091, -5665, 13677, 4061, -12173},
                                          {19716, -26173, -7491, 14149, -5311, -12822, 11880, -1901},
                                          {25797, -20206, -14413, -3943, 11967, 4957, 6700, 9011},
                                          {30134, -12872, 654, -13860, -11112, 4603, -4543, 9385},
                                          {32434, -4668, 12617, -2202, 4249, -10258, -9620, 287},
                                          {32541, 3851, 4493, 10847, 3895, 9403, -4311, -7698},
                                          {30448, 12111, -8732, 6137, -8548, -3541, 4393, -6711},
                                          {26296, 19551, -7105, -6465, 7693, -3187, 7183, 713},
                                          {20367, 25669, 4230, -7417, -2833, 6838, 2390, 5955},
                                          {13061, 30053, 7141, 2197, -2479, -5984, -3653, 4264},
                                          {4872, 32404, -502, 6384, 5129, 2124, -4688, -1087},
                                          {-3647, 32564, -5283, 752, -4274, 1770, -1003, -3883},
                                          {-11919, 30523, -1509, -3994, 1416, -3419, 2383, -2148},
                                          {-19385, 26419, 2676, -1758, 1062, 2564, 2255, 840},
                                          {-25541, 20528, 1533, 1486, -1710, -708, 199, 1592},
                                          {-29970, 13250, -558, 910, 855, -354, -660, 456}};

const FIXP_DECORR DecorrNumeratorReal0_USAC[MAX_DECORR_SEED_USAC][DECORR_FILTER_ORDER_BAND_0_USAC + 1] = {
    {1471, 2098, -5839, 2010, 546, 10, -1885, -2845, -4208, -5158, 16384}};

const FIXP_DECORR DecorrNumeratorReal1_USAC[MAX_DECORR_SEED_USAC][DECORR_FILTER_ORDER_BAND_1_USAC + 1] = {
    {-2000, -266, 750, 1052, -2066, 2019, -1457, -4704, 16384}};

/* identical to MPS coeffs for reverb band 3: DecorrNumeratorReal3[0] */
const FIXP_DECORR DecorrNumeratorReal2_USAC[MAX_DECORR_SEED_USAC][DECORR_FILTER_ORDER_BAND_2_USAC + 1] = {
    {585, -535, 2120, 16384}};

const FIXP_DECORR DecorrNumeratorReal3_USAC[MAX_DECORR_SEED_USAC][DECORR_FILTER_ORDER_BAND_3_USAC + 1] = {
    {-213, 569, 16384}};

/* const FIXP_DECORR DecorrNumeratorReal0_LD[MAX_DECORR_SEED_LD][] does not
 * exist */


const FIXP_DECORR DecorrNumeratorReal1_LD[MAX_DECORR_SEED_LD][DECORR_FILTER_ORDER_BAND_1_LD + 1] = {
    {
        -3311,
        6451,
        3267,
        -8738,
        -3531,
        6533,
        5908,
        -3431,
        -4199,
        2538,
        10492,
        6791,
        -8464,
        -12829,
        5086,
        16384,
    },
    {
        1053,
        439,
        -1190,
        2352,
        -3331,
        -8878,
        -2512,
        2838,
        -3599,
        -4836,
        850,
        6157,
        -4879,
        -2521,
        6526,
        16384,
    },
    {
        5498,
        41,
        -2576,
        4600,
        -3183,
        6292,
        -6692,
        -2868,
        9109,
        -8087,
        930,
        -1078,
        5536,
        3743,
        -11510,
        16384,
    },
    {
        -7986,
        -6903,
        -4298,
        -24,
        12250,
        18034,
        15360,
        9095,
        -11760,
        -19484,
        -21295,
        -14584,
        2904,
        12050,
        14922,
        16384,
    },
};


const FIXP_DECORR DecorrNumeratorReal2_LD[MAX_DECORR_SEED_LD][DECORR_FILTER_ORDER_BAND_2_LD + 1 + DECORR_ZERO_PADDING] =
    {
        {
            -75,
            428,
            -3395,
            -451,
            2367,
            -3386,
            16384,
        },
        {
            -7420,
            -12943,
            -3613,
            3216,
            8696,
            15624,
            16384,
        },
        {
            -7576,
            3836,
            8460,
            -415,
            -4704,
            251,
            16384,
        },
        {
            7900,
            -6733,
            -5813,
            12455,
            -4018,
            -7410,
            16384,
        },
};


const FIXP_DECORR DecorrNumeratorReal3_LD[MAX_DECORR_SEED_LD][DECORR_FILTER_ORDER_BAND_3_LD + 1] = {
    {
        585,
        -535,
        2120,
        16384,
    },
    {
        7602,
        -905,
        3714,
        16384,
    },
    {
        4044,
        341,
        -6387,
        16384,
    },
    {
        -7265,
        -8082,
        6647,
        16384,
    },
};
int32_t *getAddrDirectSignalMaxVal(HANDLE_DECORR_DEC self) { return &(self->ducker.maxValDirectData); }

static int32_t DecorrFilterInit(DECORR_FILTER_INSTANCE *const self, FIXP_MPS *pStateBufferCplx,
                                int32_t *pDelayBufferCplx, int32_t *offsetStateBuffer, int32_t *offsetDelayBuffer,
                                int32_t const decorr_seed, int32_t const reverb_band, int32_t const useFractDelay,
                                int32_t const noSampleDelay, int32_t const filterOrder,
                                FDK_DECORR_TYPE const decorrType) {
    int32_t errorCode = 0;
    switch(decorrType) {
        case DECORR_USAC:
            if(useFractDelay) { return 1; }
            else {
                assert(decorr_seed == 0);

                switch(reverb_band) {
                    case 0:
                        self->numeratorReal = DecorrNumeratorReal0_USAC[decorr_seed];
                        break;
                    case 1:
                        self->numeratorReal = DecorrNumeratorReal1_USAC[decorr_seed];
                        break;
                    case 2:
                        self->numeratorReal = DecorrNumeratorReal2_USAC[decorr_seed];
                        break;
                    case 3:
                        self->numeratorReal = DecorrNumeratorReal3_USAC[decorr_seed];
                        break;
                }
            }
            break;
        case DECORR_LD:
            assert(decorr_seed < MAX_DECORR_SEED_LD);
            switch(reverb_band) {
                case 0:
                    self->numeratorReal = NULL;
                    break;
                case 1:
                    self->numeratorReal = DecorrNumeratorReal1_LD[decorr_seed];
                    break;
                case 2:
                    self->numeratorReal = DecorrNumeratorReal2_LD[decorr_seed];
                    break;
                case 3:
                    self->numeratorReal = DecorrNumeratorReal3_LD[decorr_seed];
                    break;
            }
            break;
        default:
            return 1;
    }

    self->stateCplx = pStateBufferCplx + (*offsetStateBuffer);
    *offsetStateBuffer += 2 * filterOrder;
    self->DelayBufferCplx = pDelayBufferCplx + (*offsetDelayBuffer);
    *offsetDelayBuffer += 2 * noSampleDelay;

    return errorCode;
}

/*******************************************************************************
*******************************************************************************/
static int32_t DecorrFilterInitPS(DECORR_FILTER_INSTANCE *const self, FIXP_MPS *pStateBufferCplx,
                                  int32_t *pDelayBufferCplx, int32_t *offsetStateBuffer, int32_t *offsetDelayBuffer,
                                  int32_t const hybridBand, int32_t const reverbBand, int32_t const noSampleDelay) {
    int32_t errorCode = 0;

    if(reverbBand == 0) {
        self->coeffsPacked = DecorrPsCoeffsCplx[hybridBand];

        self->stateCplx = pStateBufferCplx + (*offsetStateBuffer);
        *offsetStateBuffer += 2 * DECORR_FILTER_ORDER_PS;
    }

    self->DelayBufferCplx = pDelayBufferCplx + (*offsetDelayBuffer);
    *offsetDelayBuffer += 2 * noSampleDelay;

    return errorCode;
}


static int32_t DecorrFilterApplyPASS(DECORR_FILTER_INSTANCE const filter[], int32_t *dataRealIn, int32_t *dataImagIn,
                                     int32_t *dataRealOut, int32_t *dataImagOut, int32_t start, int32_t stop,
                                     int32_t reverbBandNoSampleDelay, int32_t reverbBandDelayBufferIndex) {
    int32_t   i;
    int32_t   offset = 2 * reverbBandNoSampleDelay;
    FIXP_MPS *pDelayBuffer = &filter[start].DelayBufferCplx[reverbBandDelayBufferIndex];

    /* Memory for the delayline has been allocated in a consecutive order, so we
       can address from filter to filter with a constant length.
       Be aware that real and imaginary part of the delayline are stored in
       interleaved order.
    */
    if(dataImagIn == NULL) {
        for(i = start; i < stop; i++) {
            int32_t tmp;

            tmp = *pDelayBuffer;
            *pDelayBuffer = dataRealIn[i];
            dataRealOut[i] = tmp;
            pDelayBuffer += offset;
        }
    }
    else {
        if((i = stop - start) != 0) {
            dataRealIn += start;
            dataImagIn += start;
            dataRealOut += start;
            dataImagOut += start;
            do {
                int32_t delay_re, delay_im, real, imag;

                real = *dataRealIn++;
                imag = *dataImagIn++;
                delay_re = pDelayBuffer[0];
                delay_im = pDelayBuffer[1];
                pDelayBuffer[0] = real;
                pDelayBuffer[1] = imag;
                *dataRealOut++ = delay_re;
                *dataImagOut++ = delay_im;
                pDelayBuffer += offset;
            } while(--i != 0);
        }
    }

    return (int32_t)0;
}

#ifndef FUNCTION_DecorrFilterApplyREAL

static int32_t DecorrFilterApplyREAL(DECORR_FILTER_INSTANCE const filter[], int32_t *dataRealIn, int32_t *dataImagIn,
                                     int32_t *dataRealOut, int32_t *dataImagOut, int32_t start, int32_t stop,
                                     int32_t reverbFilterOrder, int32_t reverbBandNoSampleDelay,
                                     int32_t reverbBandDelayBufferIndex) {
    int32_t i, j;
    int32_t xReal, xImag, yReal, yImag;

    const FIXP_DECORR *pFilter = filter[start].numeratorReal;

    int32_t   offsetDelayBuffer = (2 * reverbBandNoSampleDelay) - 1;
    FIXP_MPS *pDelayBuffer = &filter[start].DelayBufferCplx[reverbBandDelayBufferIndex];

    int32_t  offsetStates = 2 * reverbFilterOrder;
    int32_t *pStates = filter[start].stateCplx;

    /* Memory for the delayline has been allocated in a consecutive order, so we
       can address from filter to filter with a constant length. The same is valid
       for the states.
       Be aware that real and imaginary part of the delayline and the states are
       stored in interleaved order.
       All filter in a reverb band have the same filter coefficients.
       Exploit symmetry: numeratorReal[i] =
       denominatorReal[reverbFilterLength-1-i] Do not accumulate the highest
       states which are always zero.
    */
    if(reverbFilterOrder == 2) {
        FIXP_DECORR nFilt0L, nFilt0H;

        nFilt0L = pFilter[0];
        nFilt0H = pFilter[1];

        for(i = start; i < stop; i++) {
            xReal = *pDelayBuffer;
            *pDelayBuffer = dataRealIn[i];
            pDelayBuffer++;

            xImag = *pDelayBuffer;
            *pDelayBuffer = dataImagIn[i];
            pDelayBuffer += offsetDelayBuffer;

            yReal = (pStates[0] + fMultDiv2(xReal, nFilt0L)) << FILTER_SF;
            yImag = (pStates[1] + fMultDiv2(xImag, nFilt0L)) << FILTER_SF;

            dataRealOut[i] = yReal;
            dataImagOut[i] = yImag;

            pStates[0] = pStates[2] + fMultDiv2(xReal, nFilt0H) - fMultDiv2(yReal, nFilt0H);
            pStates[1] = pStates[3] + fMultDiv2(xImag, nFilt0H) - fMultDiv2(yImag, nFilt0H);
            pStates[2] = (xReal >> FILTER_SF) - fMultDiv2(yReal, nFilt0L);
            pStates[3] = (xImag >> FILTER_SF) - fMultDiv2(yImag, nFilt0L);
            pStates += offsetStates;
        }
    }
    else if(reverbFilterOrder == 3) {
        FIXP_DECORR nFilt0L, nFilt0H, nFilt1L;

        nFilt0L = pFilter[0];
        nFilt0H = pFilter[1];
        nFilt1L = pFilter[2];

        for(i = start; i < stop; i++) {
            xReal = *pDelayBuffer;
            *pDelayBuffer = dataRealIn[i];
            pDelayBuffer++;

            xImag = *pDelayBuffer;
            *pDelayBuffer = dataImagIn[i];
            pDelayBuffer += offsetDelayBuffer;

            yReal = (pStates[0] + fMultDiv2(xReal, nFilt0L)) << FILTER_SF;
            yImag = (pStates[1] + fMultDiv2(xImag, nFilt0L)) << FILTER_SF;

            dataRealOut[i] = yReal;
            dataImagOut[i] = yImag;

            pStates[0] = pStates[2] + fMultDiv2(xReal, nFilt0H) - fMultDiv2(yReal, nFilt1L);
            pStates[1] = pStates[3] + fMultDiv2(xImag, nFilt0H) - fMultDiv2(yImag, nFilt1L);
            pStates[2] = pStates[4] + fMultDiv2(xReal, nFilt1L) - fMultDiv2(yReal, nFilt0H);
            pStates[3] = pStates[5] + fMultDiv2(xImag, nFilt1L) - fMultDiv2(yImag, nFilt0H);
            pStates[4] = (xReal >> FILTER_SF) - fMultDiv2(yReal, nFilt0L);
            pStates[5] = (xImag >> FILTER_SF) - fMultDiv2(yImag, nFilt0L);
            pStates += offsetStates;
        }
    }
    else if(reverbFilterOrder == 6) {
        FIXP_DECORR nFilt0L, nFilt0H, nFilt1L, nFilt1H, nFilt2L, nFilt2H;

        nFilt0L = pFilter[0];
        nFilt0H = pFilter[1];
        nFilt1L = pFilter[2];
        nFilt1H = pFilter[3];
        nFilt2L = pFilter[4];
        nFilt2H = pFilter[5];

        for(i = start; i < stop; i++) {
            xReal = *pDelayBuffer;
            *pDelayBuffer = dataRealIn[i];
            pDelayBuffer++;

            xImag = *pDelayBuffer;
            *pDelayBuffer = dataImagIn[i];
            pDelayBuffer += offsetDelayBuffer;

            yReal = (pStates[0] + fMultDiv2(xReal, nFilt0L)) << FILTER_SF;
            yImag = (pStates[1] + fMultDiv2(xImag, nFilt0L)) << FILTER_SF;
            dataRealOut[i] = yReal;
            dataImagOut[i] = yImag;

            pStates[0] = pStates[2] + fMultDiv2(xReal, nFilt0H) - fMultDiv2(yReal, nFilt2H);
            pStates[1] = pStates[3] + fMultDiv2(xImag, nFilt0H) - fMultDiv2(yImag, nFilt2H);
            pStates[2] = pStates[4] + fMultDiv2(xReal, nFilt1L) - fMultDiv2(yReal, nFilt2L);
            pStates[3] = pStates[5] + fMultDiv2(xImag, nFilt1L) - fMultDiv2(yImag, nFilt2L);
            pStates[4] = pStates[6] + fMultDiv2(xReal, nFilt1H) - fMultDiv2(yReal, nFilt1H);
            pStates[5] = pStates[7] + fMultDiv2(xImag, nFilt1H) - fMultDiv2(yImag, nFilt1H);
            pStates[6] = pStates[8] + fMultDiv2(xReal, nFilt2L) - fMultDiv2(yReal, nFilt1L);
            pStates[7] = pStates[9] + fMultDiv2(xImag, nFilt2L) - fMultDiv2(yImag, nFilt1L);
            pStates[8] = pStates[10] + fMultDiv2(xReal, nFilt2H) - fMultDiv2(yReal, nFilt0H);
            pStates[9] = pStates[11] + fMultDiv2(xImag, nFilt2H) - fMultDiv2(yImag, nFilt0H);
            pStates[10] = (xReal >> FILTER_SF) - fMultDiv2(yReal, nFilt0L);
            pStates[11] = (xImag >> FILTER_SF) - fMultDiv2(yImag, nFilt0L);
            pStates += offsetStates;
        }
    }
    else {
        FIXP_DECORR nFilt0L, nFilt0H;
        for(i = start; i < stop; i++) {
            xReal = *pDelayBuffer;
            *pDelayBuffer = dataRealIn[i];
            pDelayBuffer++;

            xImag = *pDelayBuffer;
            *pDelayBuffer = dataImagIn[i];
            pDelayBuffer += offsetDelayBuffer;

            nFilt0L = pFilter[0];
            yReal = (pStates[0] + fMultDiv2(xReal, nFilt0L)) << 2;
            yImag = (pStates[1] + fMultDiv2(xImag, nFilt0L)) << 2;
            dataRealOut[i] = yReal;
            dataImagOut[i] = yImag;

            for(j = 1; j < reverbFilterOrder; j++) {
                nFilt0L = pFilter[j];
                nFilt0H = pFilter[reverbFilterOrder - j];
                pStates[2 * j - 2] = pStates[2 * j] + fMultDiv2(xReal, nFilt0L) - fMultDiv2(yReal, nFilt0H);
                pStates[2 * j - 1] = pStates[2 * j + 1] + fMultDiv2(xImag, nFilt0L) - fMultDiv2(yImag, nFilt0H);
            }
            nFilt0L = pFilter[j];
            nFilt0H = pFilter[reverbFilterOrder - j];
            pStates[2 * j - 2] = fMultDiv2(xReal, nFilt0L) - fMultDiv2(yReal, nFilt0H);
            pStates[2 * j - 1] = fMultDiv2(xImag, nFilt0L) - fMultDiv2(yImag, nFilt0H);

            pStates += offsetStates;
        }
    }

    return (int32_t)0;
}
#endif /* #ifndef FUNCTION_DecorrFilterApplyREAL */

#ifndef FUNCTION_DecorrFilterApplyCPLX_PS

static int32_t DecorrFilterApplyCPLX_PS(DECORR_FILTER_INSTANCE const filter[], int32_t *dataRealIn, int32_t *dataImagIn,
                                        int32_t *dataRealOut, int32_t *dataImagOut, int32_t start, int32_t stop,
                                        int32_t reverbFilterOrder, int32_t reverbBandNoSampleDelay,
                                        int32_t reverbBandDelayBufferIndex, uint8_t *stateBufferOffset) {
    /* r = real, j = imaginary */
    int32_t  r_data_a, j_data_a, r_data_b, j_data_b, r_stage_mult, j_stage_mult;
    FIXP_SPK_t rj_coeff;

    /* get pointer to current position in input delay buffer of filter with
     * starting-index */
    int32_t *pDelayBuffer = &filter[start].DelayBufferCplx[reverbBandDelayBufferIndex]; /* increases
                                                                                           by 2 every
                                                                                           other call
                                                                                           of this
                                                                                           function */
    /* determine the increment for this pointer to get to the correct position in
     * the delay buffer of the next filter */
    int32_t offsetDelayBuffer = (2 * reverbBandNoSampleDelay) - 1;

    /* pointer to current position in state buffer */
    int32_t *pStates = filter[start].stateCplx;
    int32_t  pStatesIncrement = 2 * reverbFilterOrder;

    /* stateBufferOffset-pointers */
    int32_t *pStateBufferOffset0 = pStates + stateBufferOffset[0];
    int32_t *pStateBufferOffset1 = pStates + stateBufferOffset[1];
    int32_t *pStateBufferOffset2 = pStates + stateBufferOffset[2];

    /* traverse all hybrid-bands inbetween start- and stop-index */
    for(int32_t i = start; i < stop; i++) {
        /* 1. input delay (real/imaginary values interleaved) */

        /* load delayed real input value */
        r_data_a = *pDelayBuffer;
        /* store incoming real data value to delay buffer and increment pointer */
        *pDelayBuffer++ = dataRealIn[i];

        /* load delayed imaginary input value */
        j_data_a = *pDelayBuffer;
        /* store incoming imaginary data value to delay buffer */
        *pDelayBuffer = dataImagIn[i];
        /* increase delay buffer by offset */
        pDelayBuffer += offsetDelayBuffer;

        /* 2. Phi(k)-stage */

        /* create pointer to coefficient table (real and imaginary coefficients
         * interleaved) */
        const FIXP_SPK_t *pCoeffs = filter[i].coeffsPacked;

        /* the first two entries of the coefficient table are the
         * Phi(k)-multiplicants */
        rj_coeff = *pCoeffs++;
        /* multiply value from input delay buffer by looked-up values */
        cplxMultDiv2(&r_data_b, &j_data_b, r_data_a, j_data_a, rj_coeff);

        /* 3. process all three filter stages */

        /* stage 0 */

        /* get coefficients from lookup table */
        rj_coeff = *pCoeffs++;

        /* multiply output of last stage by coefficient */
        cplxMultDiv2(&r_stage_mult, &j_stage_mult, r_data_b, j_data_b, rj_coeff);
        r_stage_mult <<= 1;
        j_stage_mult <<= 1;

        /* read and add value from state buffer (this is the input for the next
         * stage) */
        r_data_a = r_stage_mult + pStateBufferOffset0[0];
        j_data_a = j_stage_mult + pStateBufferOffset0[1];

        /* negate r_data_a to perform multiplication with complex conjugate of
         * rj_coeff */
        cplxMultDiv2(&r_stage_mult, &j_stage_mult, -r_data_a, j_data_a, rj_coeff);

        /* add stage input to shifted result */
        r_stage_mult = r_data_b + (r_stage_mult << 1);
        j_stage_mult = j_data_b - (j_stage_mult << 1);

        /* store result to state buffer */
        pStateBufferOffset0[0] = r_stage_mult;
        pStateBufferOffset0[1] = j_stage_mult;
        pStateBufferOffset0 += pStatesIncrement;

        /* stage 1 */

        /* get coefficients from lookup table */
        rj_coeff = *pCoeffs++;

        /* multiply output of last stage by coefficient */
        cplxMultDiv2(&r_stage_mult, &j_stage_mult, r_data_a, j_data_a, rj_coeff);
        r_stage_mult <<= 1;
        j_stage_mult <<= 1;

        /* read and add value from state buffer (this is the input for the next
         * stage) */
        r_data_b = r_stage_mult + pStateBufferOffset1[0];
        j_data_b = j_stage_mult + pStateBufferOffset1[1];

        /* negate r_data_b to perform multiplication with complex conjugate of
         * rj_coeff */
        cplxMultDiv2(&r_stage_mult, &j_stage_mult, -r_data_b, j_data_b, rj_coeff);

        /* add stage input to shifted result */
        r_stage_mult = r_data_a + (r_stage_mult << 1);
        j_stage_mult = j_data_a - (j_stage_mult << 1);

        /* store result to state buffer */
        pStateBufferOffset1[0] = r_stage_mult;
        pStateBufferOffset1[1] = j_stage_mult;
        pStateBufferOffset1 += pStatesIncrement;

        /* stage 2 */

        /* get coefficients from lookup table */
        rj_coeff = *pCoeffs++;

        /* multiply output of last stage by coefficient */
        cplxMultDiv2(&r_stage_mult, &j_stage_mult, r_data_b, j_data_b, rj_coeff);
        r_stage_mult <<= 1;
        j_stage_mult <<= 1;

        /* read and add value from state buffer (this is the input for the next
         * stage) */
        r_data_a = r_stage_mult + pStateBufferOffset2[0];
        j_data_a = j_stage_mult + pStateBufferOffset2[1];

        /* negate r_data_a to perform multiplication with complex conjugate of
         * rj_coeff */
        cplxMultDiv2(&r_stage_mult, &j_stage_mult, -r_data_a, j_data_a, rj_coeff);

        /* add stage input to shifted result */
        r_stage_mult = r_data_b + (r_stage_mult << 1);
        j_stage_mult = j_data_b - (j_stage_mult << 1);

        /* store result to state buffer */
        pStateBufferOffset2[0] = r_stage_mult;
        pStateBufferOffset2[1] = j_stage_mult;
        pStateBufferOffset2 += pStatesIncrement;

        /* write filter output */
        dataRealOut[i] = r_data_a << 1;
        dataImagOut[i] = j_data_a << 1;

    } /* end of band/filter loop (outer loop) */

    /* update stateBufferOffset with respect to ring buffer boundaries */
    if(stateBufferOffset[0] == 4) stateBufferOffset[0] = 0;
    else
        stateBufferOffset[0] += 2;

    if(stateBufferOffset[1] == 12) stateBufferOffset[1] = 6;
    else
        stateBufferOffset[1] += 2;

    if(stateBufferOffset[2] == 22) stateBufferOffset[2] = 14;
    else
        stateBufferOffset[2] += 2;

    return (int32_t)0;
}

#endif /* FUNCTION_DecorrFilterApplyCPLX_PS */

/*******************************************************************************
*******************************************************************************/
static int32_t DuckerInit(DUCKER_INSTANCE *const self, int32_t const hybridBands, int32_t partiallyComplex,
                          const FDK_DUCKER_TYPE duckerType, const int32_t nParamBands, int32_t initStatesFlag) {
    int32_t errorCode = 0;

    if(self) {
        switch(nParamBands) {
            case(20):
                assert(hybridBands == 71);
                self->mapHybBands2ProcBands = kernels_20_to_71_PS;
                self->mapProcBands2HybBands = kernels_20_to_71_offset_PS;
                self->parameterBands = (20);
                break;
            case(28):

                self->mapHybBands2ProcBands = kernels_28_to_71;
                self->mapProcBands2HybBands = kernels_28_to_71_offset;
                self->parameterBands = (28);
                break;
            case(23):
                assert(hybridBands == 64 || hybridBands == 32);
                self->mapHybBands2ProcBands = kernels_23_to_64;
                self->mapProcBands2HybBands = kernels_23_to_64_offset;
                self->parameterBands = (23);
                break;
            default:
                return 1;
        }
        self->qs_next = &self->mapProcBands2HybBands[1];

        self->maxValDirectData = FL2FXCONST_DBL(-1.0f);
        self->maxValReverbData = FL2FXCONST_DBL(-1.0f);
        self->scaleDirectNrg = 2 * DUCKER_MAX_NRG_SCALE;
        self->scaleReverbNrg = 2 * DUCKER_MAX_NRG_SCALE;
        self->scaleSmoothDirRevNrg = 2 * DUCKER_MAX_NRG_SCALE;
        self->headroomSmoothDirRevNrg = 2 * DUCKER_MAX_NRG_SCALE;
        self->hybridBands = hybridBands;
        self->partiallyComplex = partiallyComplex;

        if(initStatesFlag && (duckerType == DUCKER_PS)) {
            int32_t pb;
            for(pb = 0; pb < self->parameterBands; pb++) { self->SmoothDirRevNrg[pb] = (FIXP_MPS)0; }
        }
    }
    else
        errorCode = 1;

    return errorCode;
}

/*******************************************************************************
*******************************************************************************/

#ifndef FUNCTION_DuckerCalcEnergy
static int32_t DuckerCalcEnergy(DUCKER_INSTANCE *const self, int32_t const inputReal[(71)],
                                int32_t const inputImag[(71)], int32_t energy[(28)], int32_t inputMaxVal,
                                int8_t *nrgScale, int32_t mode, /* 1:(ps) 0:(else) */
                                int32_t startHybBand) {
    int32_t err = 0;
    int32_t qs, maxHybBand;
    int32_t maxHybridBand = self->hybridBands - 1;

    maxHybBand = maxHybridBand;

    memset(energy, 0, (28) * sizeof(int32_t));

    if(mode == 1) {
        int32_t pb;
        int32_t clz;
        int32_t maxVal = FL2FXCONST_DBL(-1.0f);

        if(maxVal == FL2FXCONST_DBL(-1.0f)) {
            clz = fMin(getScalefactor(&inputReal[startHybBand], fMax(0, maxHybridBand - startHybBand + 1)),
                       getScalefactor(&inputImag[startHybBand], fMax(0, maxHybBand - startHybBand + 1)));
        }
        else { clz = CntLeadingZeros(maxVal) - 1; }

        clz = fMin(fMax(0, clz - DUCKER_HEADROOM_BITS), DUCKER_MAX_NRG_SCALE);
        *nrgScale = (int8_t)clz << 1;

        /* Initialize pb since it would stay uninitialized for the case startHybBand
         * > maxHybBand. */
        pb = SpatialDecGetProcessingBand(maxHybBand, self->mapHybBands2ProcBands);
        for(qs = startHybBand; qs <= maxHybBand; qs++) {
            pb = SpatialDecGetProcessingBand(qs, self->mapHybBands2ProcBands);
            energy[pb] = SATURATE_LEFT_SHIFT(
                (energy[pb] >> 1) + (fPow2Div2(inputReal[qs] << clz) >> 1) + (fPow2Div2(inputImag[qs] << clz) >> 1), 1,
                DFRACT_BITS);
        }
        pb++;

        for(; pb <= SpatialDecGetProcessingBand(maxHybridBand, self->mapHybBands2ProcBands); pb++) {
            assert(pb != SpatialDecGetProcessingBand(qs - 1, self->mapHybBands2ProcBands));
            int32_t qs_next;
            int32_t nrg = 0;
            qs_next = (int32_t)self->qs_next[pb];
            for(; qs < qs_next; qs++) { nrg = fAddSaturate(nrg, fPow2Div2(inputReal[qs] << clz)); }
            energy[pb] = nrg;
        }
    }
    else {
        int32_t clz;
        int32_t maxVal = FL2FXCONST_DBL(-1.0f);

        maxVal = inputMaxVal;

        if(maxVal == FL2FXCONST_DBL(-1.0f)) {
            clz = fMin(getScalefactor(&inputReal[startHybBand], fMax(0, maxHybridBand - startHybBand + 1)),
                       getScalefactor(&inputImag[startHybBand], fMax(0, maxHybBand - startHybBand + 1)));
        }
        else { clz = CntLeadingZeros(maxVal) - 1; }

        clz = fMin(fMax(0, clz - DUCKER_HEADROOM_BITS), DUCKER_MAX_NRG_SCALE);
        *nrgScale = (int8_t)clz << 1;

        for(qs = startHybBand; qs <= maxHybBand; qs++) {
            int32_t pb = SpatialDecGetProcessingBand(qs, self->mapHybBands2ProcBands);
            energy[pb] = SATURATE_LEFT_SHIFT(
                (energy[pb] >> 1) + (fPow2Div2(inputReal[qs] << clz) >> 1) + (fPow2Div2(inputImag[qs] << clz) >> 1), 1,
                DFRACT_BITS);
        }

        for(; qs <= maxHybridBand; qs++) {
            int32_t pb = SpatialDecGetProcessingBand(qs, self->mapHybBands2ProcBands);
            energy[pb] = fAddSaturate(energy[pb], fPow2Div2(inputReal[qs] << clz));
        }
    }

    {
        /* Catch overflows which have been observed in erred bitstreams to avoid
         * assertion failures later. */
        int32_t pb;
        for(pb = 0; pb < (28); pb++) { energy[pb] = (int32_t)((int32_t)energy[pb] & (int32_t)MAXVAL_DBL); }
    }
    return err;
}
#endif /* #ifndef FUNCTION_DuckerCalcEnergy */


static int32_t DuckerApply(DUCKER_INSTANCE *const self, int32_t const directNrg[(28)], int32_t outputReal[(71)],
                           int32_t outputImag[(71)], int32_t startHybBand) {
    int32_t err = 0;
    int32_t qs = startHybBand;
    int32_t qs_next = 0;
    int32_t pb = 0;
    int32_t startParamBand = 0;
    int32_t hybBands;
    int32_t hybridBands = self->hybridBands;

    int32_t reverbNrg[28];
    int32_t       *smoothDirRevNrg = &self->SmoothDirRevNrg[0];
    FIXP_DUCK_GAIN duckGain = 0;

    int32_t doScaleNrg = 0;
    int32_t scaleDirectNrg = 0;
    int32_t scaleReverbNrg = 0;
    int32_t scaleSmoothDirRevNrg = 0;
    int32_t maxDirRevNrg = FL2FXCONST_DBL(0.0);

    hybBands = hybridBands;

    startParamBand = SpatialDecGetProcessingBand(startHybBand, self->mapHybBands2ProcBands);

    DuckerCalcEnergy(self, outputReal, outputImag, reverbNrg, self->maxValReverbData, &(self->scaleReverbNrg), 0,
                     startHybBand);

    if((self->scaleDirectNrg != self->scaleReverbNrg) || (self->scaleDirectNrg != self->scaleSmoothDirRevNrg) ||
       (self->headroomSmoothDirRevNrg == 0)) {
        int32_t scale;

        scale = fixMin(self->scaleDirectNrg, self->scaleSmoothDirRevNrg + self->headroomSmoothDirRevNrg - 1);
        scale = fixMin(scale, self->scaleReverbNrg);

        scaleDirectNrg = fMax(fMin(self->scaleDirectNrg - scale, (DFRACT_BITS - 1)), -(DFRACT_BITS - 1));
        scaleReverbNrg = fMax(fMin(self->scaleReverbNrg - scale, (DFRACT_BITS - 1)), -(DFRACT_BITS - 1));
        scaleSmoothDirRevNrg = fMax(fMin(self->scaleSmoothDirRevNrg - scale, (DFRACT_BITS - 1)), -(DFRACT_BITS - 1));

        self->scaleSmoothDirRevNrg = (int8_t)scale;

        doScaleNrg = 1;
    }
    for(pb = startParamBand; pb < self->parameterBands; pb++) {
        int32_t tmp1;
        int32_t tmp2;
        int32_t s;

        /* smoothDirRevNrg[2*pb  ] = fMult(smoothDirRevNrg[2*pb  ],DUCK_ALPHA_FDK) +
           fMultDiv2(directNrg[pb],DUCK_ONE_MINUS_ALPHA_X4_FDK);
           smoothDirRevNrg[2*pb+1] = fMult(smoothDirRevNrg[2*pb+1],DUCK_ALPHA_FDK) +
           fMultDiv2(reverbNrg[pb],DUCK_ONE_MINUS_ALPHA_X4_FDK); tmp1 =
           fMult(smoothDirRevNrg[2*pb],DUCK_GAMMA_FDK); tmp2 =
           smoothDirRevNrg[2*pb+1] >> 1;
        */
        tmp1 = smoothDirRevNrg[2 * pb + 0];
        tmp2 = smoothDirRevNrg[2 * pb + 1];
        tmp1 = fMult(tmp1, DUCK_ALPHA_FDK);
        tmp2 = fMult(tmp2, DUCK_ALPHA_FDK);

        if(doScaleNrg) {
            int32_t scaleSmoothDirRevNrg_asExponent = -scaleSmoothDirRevNrg;

            tmp1 = scaleValue(tmp1, scaleSmoothDirRevNrg_asExponent);
            tmp2 = scaleValue(tmp2, scaleSmoothDirRevNrg_asExponent);
            tmp1 = fMultAddDiv2(tmp1, scaleValue(directNrg[pb], -scaleDirectNrg), DUCK_ONE_MINUS_ALPHA_X4_FDK);
            tmp2 = fMultAddDiv2(tmp2, scaleValue(reverbNrg[pb], -scaleReverbNrg), DUCK_ONE_MINUS_ALPHA_X4_FDK);
        }
        else {
            tmp1 = fMultAddDiv2(tmp1, directNrg[pb], DUCK_ONE_MINUS_ALPHA_X4_FDK);
            tmp2 = fMultAddDiv2(tmp2, reverbNrg[pb], DUCK_ONE_MINUS_ALPHA_X4_FDK);
        }

        smoothDirRevNrg[2 * pb] = tmp1;
        smoothDirRevNrg[2 * pb + 1] = tmp2;

        maxDirRevNrg |= fAbs(tmp1);
        maxDirRevNrg |= fAbs(tmp2);

        tmp1 = fMult(tmp1, DUCK_GAMMA_FDK);
        tmp2 = tmp2 >> 1;

        qs_next = fMin((int32_t)self->qs_next[pb], self->hybridBands);

        if(tmp2 > tmp1) { /* true for about 20% */
            /* gain smaller than 1.0 */
            tmp1 = sqrtFixp(tmp1);
            tmp2 = invSqrtNorm2(tmp2, &s);
            duckGain = FX_DBL2FX_SGL(fMultDiv2(tmp1, tmp2) << s);
        }
        else { /* true for about 80 % */
            tmp2 = smoothDirRevNrg[2 * pb] >> 1;
            tmp1 = fMult(smoothDirRevNrg[2 * pb + 1], DUCK_GAMMA_FDK);
            if(tmp2 > tmp1) { /* true for about 20% */
                if(tmp1 <= (tmp2 >> 2)) {
                    /* limit gain to 2.0 */
                    if(qs < hybBands) {
                        for(; qs < qs_next; qs++) {
                            outputReal[qs] = outputReal[qs] << 1;
                            outputImag[qs] = outputImag[qs] << 1;
                        }
                    }
                    else {
                        for(; qs < qs_next; qs++) { outputReal[qs] = outputReal[qs] << 1; }
                    }
                    /* skip general gain*output section */
                    continue;
                }
                else {
                    /* gain from 1.0 to 2.0 */
                    tmp2 = sqrtFixp(tmp2 >> 2);
                    tmp1 = invSqrtNorm2(tmp1, &s);
                    duckGain = FX_DBL2FX_SGL(fMult(tmp1, tmp2) << s);
                }
            }
            else { /* true for about 60% */
                /* gain = 1.0; output does not change; update qs index */
                qs = qs_next;
                continue;
            }
        }

        /* general gain*output section */
        if(qs < hybBands) {             /* true for about 39% */
            for(; qs < qs_next; qs++) { /* runs about 2 times */
                outputReal[qs] = fMultDiv2(outputReal[qs], duckGain) << 2;
                outputImag[qs] = fMultDiv2(outputImag[qs], duckGain) << 2;
            }
        }
        else {
            for(; qs < qs_next; qs++) { outputReal[qs] = fMultDiv2(outputReal[qs], duckGain) << 2; }
        }
    } /* pb */

    self->headroomSmoothDirRevNrg = (int8_t)fixMax(0, CntLeadingZeros(maxDirRevNrg) - 1);

    return err;
}


static int32_t DuckerApplyPS(DUCKER_INSTANCE *const self, int32_t const directNrg[(28)], int32_t outputReal[(71)],
                             int32_t outputImag[(71)], int32_t startHybBand) {
    int32_t qs = startHybBand;
    int32_t pb = 0;
    int32_t startParamBand = SpatialDecGetProcessingBand(startHybBand, self->mapHybBands2ProcBands);
    int32_t hybBands;

    int32_t doScaleNrg = 0;
    int32_t scaleDirectNrg = 0;
    int32_t scaleSmoothDirRevNrg = 0;
    int32_t maxDirRevNrg = FL2FXCONST_DBL(0.0);

    if((self->scaleDirectNrg != self->scaleSmoothDirRevNrg) || (self->headroomSmoothDirRevNrg == 0)) {
        int32_t scale;

        scale = fixMin(self->scaleDirectNrg, self->scaleSmoothDirRevNrg + self->headroomSmoothDirRevNrg - 2);

        scaleDirectNrg = fMax(fMin(self->scaleDirectNrg - scale, (DFRACT_BITS - 1)), -(DFRACT_BITS - 1));
        scaleSmoothDirRevNrg = fMax(fMin(self->scaleSmoothDirRevNrg - scale, (DFRACT_BITS - 1)), -(DFRACT_BITS - 1));

        self->scaleSmoothDirRevNrg = (int8_t)scale;

        doScaleNrg = 1;
    }

    hybBands = self->hybridBands;

    assert((self->parameterBands == (28)) || (self->parameterBands == (20)));
    for(pb = startParamBand; pb < self->parameterBands; pb++) {
        int32_t directNrg2 = directNrg[pb];

        if(doScaleNrg) {
            directNrg2 = scaleValue(directNrg2, -scaleDirectNrg);
            self->peakDiff[pb] = scaleValue(self->peakDiff[pb], -scaleSmoothDirRevNrg);
            self->peakDecay[pb] = scaleValue(self->peakDecay[pb], -scaleSmoothDirRevNrg);
            self->SmoothDirRevNrg[pb] = scaleValue(self->SmoothDirRevNrg[pb], -scaleSmoothDirRevNrg);
        }
        self->peakDecay[pb] = fixMax(directNrg2, fMult(self->peakDecay[pb], PS_DUCK_PEAK_DECAY_FACTOR_FDK));
        self->peakDiff[pb] = self->peakDiff[pb] +
                             fMult(PS_DUCK_FILTER_COEFF_FDK, (self->peakDecay[pb] - directNrg2 - self->peakDiff[pb]));
        self->SmoothDirRevNrg[pb] = fixMax(
            self->SmoothDirRevNrg[pb] + fMult(PS_DUCK_FILTER_COEFF_FDK, (directNrg2 - self->SmoothDirRevNrg[pb])),
            FL2FXCONST_DBL(0));

        maxDirRevNrg |= fAbs(self->peakDiff[pb]);
        maxDirRevNrg |= fAbs(self->SmoothDirRevNrg[pb]);

        if((self->peakDiff[pb] == FL2FXCONST_DBL(0)) && (self->SmoothDirRevNrg[pb] == FL2FXCONST_DBL(0))) {
            int32_t qs_next;

            qs = fMax(qs, SpatialDecGetQmfBand(pb, self->mapProcBands2HybBands));
            qs_next = fMin((int32_t)self->qs_next[pb], self->hybridBands);

            int32_t *pOutputReal = &outputReal[qs];
            int32_t *pOutputImag = &outputImag[qs];

            if(qs < hybBands) {
                for(; qs < qs_next; qs++) {
                    *pOutputReal++ = FL2FXCONST_DBL(0);
                    *pOutputImag++ = FL2FXCONST_DBL(0);
                }
            }
            else {
                for(; qs < qs_next; qs++) { *pOutputReal++ = FL2FXCONST_DBL(0); }
            }
        }
        else if(self->peakDiff[pb] != FL2FXCONST_DBL(0)) {
            int32_t multiplication = fMult(FL2FXCONST_SGL(0.75f), self->peakDiff[pb]);
            if(multiplication > (self->SmoothDirRevNrg[pb] >> 1)) {
                int32_t num, denom, duckGain;
                int32_t scale, qs_next;

                /* implement x/y as (sqrt(x)*invSqrt(y))^2 */
                num = sqrtFixp(self->SmoothDirRevNrg[pb] >> 1);
                denom = self->peakDiff[pb] + FL2FXCONST_DBL(ABS_THR / (32768.0f * 32768.0f * 128.0f * 1.5f));
                denom = invSqrtNorm2(denom, &scale);

                /* duck output whether duckGain != 1.f */
                qs = fMax(qs, SpatialDecGetQmfBand(pb, self->mapProcBands2HybBands));
                qs_next = fMin((int32_t)self->qs_next[pb], self->hybridBands);

                duckGain = fMult(num, denom);
                duckGain = fPow2Div2(duckGain << scale);
                duckGain = fMultDiv2(FL2FXCONST_SGL(2.f / 3.f), duckGain) << 3;

                int32_t *pOutputReal = &outputReal[qs];
                int32_t *pOutputImag = &outputImag[qs];

                if(qs < hybBands) {
                    for(; qs < qs_next; qs++) {
                        *pOutputReal = fMult(*pOutputReal, duckGain);
                        pOutputReal++; /* don't move in front of "=" above, because then the
                                          fract class treats it differently and provides
                                          wrong argument to fMult() (seen on win32/msvc8) */
                        *pOutputImag = fMult(*pOutputImag, duckGain);
                        pOutputImag++;
                    }
                }
                else {
                    for(; qs < qs_next; qs++) {
                        *pOutputReal = fMult(*pOutputReal, duckGain);
                        pOutputReal++;
                    }
                }
            }
        }
    } /* pb */

    self->headroomSmoothDirRevNrg = (int8_t)fixMax(0, CntLeadingZeros(maxDirRevNrg) - 1);

    return 0;
}

int32_t FDKdecorrelateOpen(HANDLE_DECORR_DEC hDecorrDec, int32_t *bufferCplx, const int32_t bufLen) {
    HANDLE_DECORR_DEC self = hDecorrDec;

    if(bufLen < (2 * ((825) + (373)))) return 1;

    /* assign all memory to stateBufferCplx. It is reassigned during
     * FDKdecorrelateInit() */
    self->stateBufferCplx = bufferCplx;
    self->L_stateBufferCplx = 0;

    self->delayBufferCplx = NULL;
    self->L_delayBufferCplx = 0;

    return 0;
}

static int32_t distributeBuffer(HANDLE_DECORR_DEC self, const int32_t L_stateBuf, const int32_t L_delayBuf) {
    /* factor 2 because of complex values */
    if((2 * ((825) + (373))) < 2 * (L_stateBuf + L_delayBuf)) { return 1; }

    self->L_stateBufferCplx = 2 * L_stateBuf;
    self->delayBufferCplx = self->stateBufferCplx + 2 * L_stateBuf;
    self->L_delayBufferCplx = 2 * L_delayBuf;

    return 0;
}
int32_t FDKdecorrelateInit(HANDLE_DECORR_DEC hDecorrDec, const int32_t nrHybBands, const FDK_DECORR_TYPE decorrType,
                           const FDK_DUCKER_TYPE duckerType, const int32_t decorrConfig, const int32_t seed,
                           const int32_t partiallyComplex, const int32_t useFractDelay, const int32_t isLegacyPS,
                           const int32_t initStatesFlag) {
    int32_t errorCode = 0;
    int32_t i, rb, i_start;
    int32_t nParamBands = 28;

    int32_t offsetStateBuffer = 0;
    int32_t offsetDelayBuffer = 0;

    const uint8_t *REV_bandOffset;

    const int8_t *REV_filterOrder;

    hDecorrDec->partiallyComplex = partiallyComplex;
    hDecorrDec->numbins = nrHybBands;

    switch(decorrType) {
        case DECORR_PS:
            /* ignore decorrConfig, seed */
            if(partiallyComplex) {
                hDecorrDec->REV_bandOffset = REV_bandOffset_PS_LP;
                hDecorrDec->REV_delay = REV_delay_PS_LP;
                errorCode = distributeBuffer(hDecorrDec, (168), (533));
            }
            else {
                hDecorrDec->REV_bandOffset = REV_bandOffset_PS_HQ;
                hDecorrDec->REV_delay = REV_delay_PS_HQ;
                errorCode = distributeBuffer(hDecorrDec, (360), (257));
            }
            hDecorrDec->REV_filterOrder = REV_filterOrder_PS;
            hDecorrDec->REV_filtType = REV_filtType_PS;

            /* Initialize ring buffer offsets for PS specific filter implementation.
             */
            for(i = 0; i < (3); i++) hDecorrDec->stateBufferOffset[i] = stateBufferOffsetInit[i];

            break;
        case DECORR_USAC:
            if(partiallyComplex) return 1;
            if(seed != 0) return 1;
            hDecorrDec->REV_bandOffset = REV_bandOffset_MPS_HQ[decorrConfig]; /* reverb band layout is
                                                                                 inherited from MPS standard */
            hDecorrDec->REV_filterOrder = REV_filterOrder_USAC;
            hDecorrDec->REV_delay = REV_delay_USAC;
            if(useFractDelay) { return 1; /* not yet supported */ }
            else {
                hDecorrDec->REV_filtType = REV_filtType_MPS; /* the filter types are
                                                                inherited from MPS
                                                                standard */
            }
            /* bsDecorrConfig == 1 is worst case */
            errorCode = distributeBuffer(hDecorrDec, (509), (643));
            break;
        case DECORR_LD:
            if(partiallyComplex) return 1;
            if(useFractDelay) return 1;
            if(decorrConfig > 2) return 1;
            if(seed > (MAX_DECORR_SEED_LD - 1)) return 1;
            if(!(nrHybBands == 64 || nrHybBands == 32)) return 1; /* actually just qmf bands and no hybrid bands */
            hDecorrDec->REV_bandOffset = REV_bandOffset_LD[decorrConfig];
            hDecorrDec->REV_filterOrder = REV_filterOrder_MPS; /* the filter orders
                                                                  are inherited from
                                                                  MPS standard */
            hDecorrDec->REV_delay = REV_delay_MPS;             /* the delays in each reverb band are inherited from
                                                                  MPS standard */
            hDecorrDec->REV_filtType = REV_filtType_LD;
            errorCode = distributeBuffer(hDecorrDec, (825), (373));
            break;
        default:
            return 1;
    }

    if(errorCode) { return errorCode; }

    if(initStatesFlag) {
        memset(hDecorrDec->stateBufferCplx, 0, hDecorrDec->L_stateBufferCplx * sizeof(*hDecorrDec->stateBufferCplx));
        memset(hDecorrDec->delayBufferCplx, 0, hDecorrDec->L_delayBufferCplx * sizeof(*hDecorrDec->delayBufferCplx));
        memset(hDecorrDec->reverbBandDelayBufferIndex, 0, sizeof(hDecorrDec->reverbBandDelayBufferIndex));
    }

    REV_bandOffset = hDecorrDec->REV_bandOffset;

    REV_filterOrder = hDecorrDec->REV_filterOrder;

    i_start = 0;
    for(rb = 0; rb < (4); rb++) {
        int32_t i_stop;

        i_stop = REV_bandOffset[rb];

        if(i_stop <= i_start) { continue; }

        for(i = i_start; i < i_stop; i++) {
            switch(decorrType) {
                case DECORR_PS:
                    errorCode = DecorrFilterInitPS(&hDecorrDec->Filter[i], hDecorrDec->stateBufferCplx,
                                                   hDecorrDec->delayBufferCplx, &offsetStateBuffer, &offsetDelayBuffer,
                                                   i, rb, hDecorrDec->REV_delay[rb]);
                    break;
                default:
                    errorCode =
                        DecorrFilterInit(&hDecorrDec->Filter[i], hDecorrDec->stateBufferCplx,
                                         hDecorrDec->delayBufferCplx, &offsetStateBuffer, &offsetDelayBuffer, seed, rb,
                                         useFractDelay, hDecorrDec->REV_delay[rb], REV_filterOrder[rb], decorrType);
                    break;
            }
        }

        i_start = i_stop;
    } /* loop over reverbBands */

    if(!(offsetStateBuffer <= hDecorrDec->L_stateBufferCplx) || !(offsetDelayBuffer <= hDecorrDec->L_delayBufferCplx)) {
        return errorCode = 1;
    }

    if(duckerType == DUCKER_AUTOMATIC) {
        /* Choose correct ducker type according to standards: */
        switch(decorrType) {
            case DECORR_PS:
                hDecorrDec->ducker.duckerType = DUCKER_PS;
                if(isLegacyPS) { nParamBands = (20); }
                else { nParamBands = (28); }
                break;
            case DECORR_USAC:
                hDecorrDec->ducker.duckerType = DUCKER_MPS;
                nParamBands = (28);
                break;
            case DECORR_LD:
                hDecorrDec->ducker.duckerType = DUCKER_MPS;
                nParamBands = (23);
                break;
            default:
                return 1;
        }
    }

    errorCode = DuckerInit(&hDecorrDec->ducker, hDecorrDec->numbins, hDecorrDec->partiallyComplex,
                           hDecorrDec->ducker.duckerType, nParamBands, initStatesFlag);

    return errorCode;
}

int32_t FDKdecorrelateClose(HANDLE_DECORR_DEC hDecorrDec) {
    int32_t err = 0;

    if(hDecorrDec == NULL) { return 1; }

    hDecorrDec->stateBufferCplx = NULL;
    hDecorrDec->L_stateBufferCplx = 0;
    hDecorrDec->delayBufferCplx = NULL;
    hDecorrDec->L_delayBufferCplx = 0;

    return err;
}


int32_t FDKdecorrelateApply(HANDLE_DECORR_DEC hDecorrDec, int32_t *dataRealIn, int32_t *dataImagIn,
                            int32_t *dataRealOut, int32_t *dataImagOut, const int32_t startHybBand) {
    HANDLE_DECORR_DEC self = hDecorrDec;
    int32_t           err = 0;
    int32_t           rb, stop, start;

    if(self != NULL) {
        int32_t nHybBands = 0;
        /* copy new samples */
        nHybBands = self->numbins;

        int32_t directNrg[(28)];

        DuckerCalcEnergy(&self->ducker, dataRealIn, dataImagIn, directNrg, self->ducker.maxValDirectData,
                         &(self->ducker.scaleDirectNrg), (self->ducker.duckerType == DUCKER_PS) ? 1 : 0, startHybBand);

        /* complex-valued hybrid bands */
        for(stop = 0, rb = 0; rb < (4); rb++) {
            start = fMax(stop, startHybBand);
            stop = fMin(self->REV_bandOffset[rb], (uint8_t)nHybBands);

            if(start < stop) {
                switch(hDecorrDec->REV_filtType[rb]) {
                    case DELAY:
                        err = DecorrFilterApplyPASS(&self->Filter[0], dataRealIn, dataImagIn, dataRealOut, dataImagOut,
                                                    start, stop, self->REV_delay[rb],
                                                    self->reverbBandDelayBufferIndex[rb]);
                        break;
                    case INDEP_CPLX_PS:
                        err =
                            DecorrFilterApplyCPLX_PS(&self->Filter[0], dataRealIn, dataImagIn, dataRealOut, dataImagOut,
                                                     start, stop, self->REV_filterOrder[rb], self->REV_delay[rb],
                                                     self->reverbBandDelayBufferIndex[rb], self->stateBufferOffset);
                        break;
                    case COMMON_REAL:
                        err = DecorrFilterApplyREAL(&self->Filter[0], dataRealIn, dataImagIn, dataRealOut, dataImagOut,
                                                    start, stop, self->REV_filterOrder[rb], self->REV_delay[rb],
                                                    self->reverbBandDelayBufferIndex[rb]);
                        break;
                    default:
                        err = 1;
                        break;
                }
                if(err != 0) { goto bail; }
            } /* if start < stop */
        }     /* loop over reverb bands */

        for(rb = 0; rb < (4); rb++) {
            self->reverbBandDelayBufferIndex[rb] += 2;
            if(self->reverbBandDelayBufferIndex[rb] >= 2 * self->REV_delay[rb])
                self->reverbBandDelayBufferIndex[rb] = 0;
        }

        switch(self->ducker.duckerType) {
            case DUCKER_PS:
                err = DuckerApplyPS(&self->ducker, directNrg, dataRealOut, dataImagOut, startHybBand);
                if(err != 0) goto bail;
                break;
            default:
                err = DuckerApply(&self->ducker, directNrg, dataRealOut, dataImagOut, startHybBand);
                if(err != 0) goto bail;
                break;
        }
    }

bail:
    return err;
}
