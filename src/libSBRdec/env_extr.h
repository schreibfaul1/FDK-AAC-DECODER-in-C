

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  Envelope extraction prototypes
*/

#ifndef ENV_EXTR_H
#define ENV_EXTR_H

#include "../libAACdec/newAACDecoder.h"
#include "sbrdecoder.h"

#include "../libFDK/FDK_bitstream.h"
#include "lpp_tran.h"

#include "psdec.h"
#include "pvc_dec.h"

#define ENV_EXP_FRACT 0
/*!< Shift raw envelope data to support fractional numbers.
  Can be set to 8 instead of 0 to enhance accuracy during concealment.
  This is not required for conformance and #requantizeEnvelopeData() will
  become more expensive.
*/

#define EXP_BITS 6
/*!< Size of exponent-part of a pseudo float envelope value (should be at least
  6). The remaining bits in each word are used for the mantissa (should be at
  least 10). This format is used in the arrays iEnvelope[] and
  sbrNoiseFloorLevel[] in the FRAME_DATA struct which must fit in a certain part
  of the output buffer (See buffer management in sbr_dec.cpp). Exponents and
  mantissas could also be stored in separate arrays. Accessing the exponent or
  the mantissa would be simplified and the masks #MASK_E resp. #MASK_M would
  no longer be required.
*/

#define MASK_M                                                          \
  (((1 << (FRACT_BITS - EXP_BITS)) - 1)                                 \
   << EXP_BITS) /*!< Mask for extracting the mantissa of a pseudo float \
                   envelope value */
#define MASK_E                                                            \
  ((1 << EXP_BITS) - 1) /*!< Mask for extracting the exponent of a pseudo \
                           float envelope value */

#define SIGN_EXT \
  (((int8_t)-1) ^ \
   MASK_E) /*!< a CHAR-constant with all bits above our sign-bit set */
#define ROUNDING                                                           \
  ((int16_t)(                                                             \
      1 << (EXP_BITS - 1))) /*!< 0.5-offset for rounding the mantissa of a \
                               pseudo-float envelope value */
#define NRG_EXP_OFFSET                                                         \
  16 /*!< Will be added to the reference energy's exponent to prevent negative \
        numbers */
#define NOISE_EXP_OFFSET                                                \
  38 /*!< Will be added to the noise level exponent to prevent negative \
        numbers */

#define ADD_HARMONICS_FLAGS_SIZE 2 /* ceil(MAX_FREQ_COEFFS/32) */

typedef enum {
  HEADER_NOT_PRESENT,
  HEADER_ERROR,
  HEADER_OK,
  HEADER_RESET
} SBR_HEADER_STATUS;

typedef enum {
  SBR_NOT_INITIALIZED = 0,
  UPSAMPLING = 1,
  SBR_HEADER = 2,
  SBR_ACTIVE = 3
} SBR_SYNC_STATE;

typedef enum { COUPLING_OFF = 0, COUPLING_LEVEL, COUPLING_BAL } COUPLING_MODE;

typedef struct {
  uint8_t nSfb[2]; /*!< Number of SBR-bands for low and high freq-resolution */
  uint8_t nNfb;    /*!< Actual number of noise bands to read from the bitstream*/
  uint8_t numMaster;      /*!< Number of SBR-bands in v_k_master */
  uint8_t lowSubband;     /*!< QMF-band where SBR frequency range starts */
  uint8_t highSubband;    /*!< QMF-band where SBR frequency range ends */
  uint8_t ov_highSubband; /*!< if headerchange applies this value holds the old
                           highband value -> highband value of overlap area;
                             required for overlap in usac when headerchange
                           occurs between XVAR and VARX frame */
  uint8_t limiterBandTable[MAX_NUM_LIMITERS + 1]; /*!< Limiter band table. */
  uint8_t noLimiterBands;                         /*!< Number of limiter bands. */
  uint8_t nInvfBands; /*!< Number of bands for inverse filtering */
  uint8_t
  *freqBandTable[2]; /*!< Pointers to freqBandTableLo and freqBandTableHi */
  uint8_t freqBandTableLo[MAX_FREQ_COEFFS / 2 + 1];
  /*!< Mapping of SBR bands to QMF bands for low frequency resolution */
  uint8_t freqBandTableHi[MAX_FREQ_COEFFS + 1];
  /*!< Mapping of SBR bands to QMF bands for high frequency resolution */
  uint8_t freqBandTableNoise[MAX_NOISE_COEFFS + 1];
  /*!< Mapping of SBR noise bands to QMF bands */
  uint8_t v_k_master[MAX_FREQ_COEFFS + 1];
  /*!< Master BandTable which freqBandTable is derived from */
} FREQ_BAND_DATA;

typedef FREQ_BAND_DATA *HANDLE_FREQ_BAND_DATA;

#define SBRDEC_ELD_GRID 1
#define SBRDEC_SYNTAX_SCAL 2
#define SBRDEC_SYNTAX_USAC 4
#define SBRDEC_SYNTAX_RSVD50 8
#define SBRDEC_USAC_INDEP \
  16 /* Flag indicating that USAC global independency flag is active. */
#define SBRDEC_LOW_POWER \
  32 /* Flag indicating that Low Power QMF mode shall be used. */
#define SBRDEC_PS_DECODED \
  64 /* Flag indicating that PS was decoded and rendered. */
#define SBRDEC_QUAD_RATE                              \
  128 /* Flag indicating that USAC SBR 4:1 is active. \
       */
#define SBRDEC_USAC_HARMONICSBR \
  256 /* Flag indicating that USAC HBE tool is active. */
#define SBRDEC_LD_MPS_QMF \
  512 /* Flag indicating that the LD-MPS QMF shall be used. */
#define SBRDEC_USAC_ITES \
  1024 /* Flag indicating that USAC inter TES tool is active. */
#define SBRDEC_SYNTAX_DRM \
  2048 /* Flag indicating that DRM30/DRM+ reverse syntax is being used. */
#define SBRDEC_ELD_DOWNSCALE \
  4096 /* Flag indicating that ELD downscaled mode decoding is used */
#define SBRDEC_DOWNSAMPLE \
  8192 /* Flag indicating that the downsampling mode is used. */
#define SBRDEC_FLUSH 16384 /* Flag is used to flush all elements in use. */
#define SBRDEC_FORCE_RESET \
  32768 /* Flag is used to force a reset of all elements in use. */
#define SBRDEC_SKIP_QMF_ANA                                               \
  (1 << 21) /* Flag indicating that the input data is provided in the QMF \
               domain. */
#define SBRDEC_SKIP_QMF_SYN                                                \
  (1 << 22) /* Flag indicating that the output data is exported in the QMF \
               domain. */

#define SBRDEC_HDR_STAT_RESET 1
#define SBRDEC_HDR_STAT_UPDATE 2

typedef struct {
  uint8_t ampResolution; /*!< Amplitude resolution of envelope values (0: 1.5dB,
                          1: 3dB) */
  uint8_t
  xover_band; /*!< Start index in #v_k_master[] used for dynamic crossover
                 frequency */
  uint8_t sbr_preprocessing; /*!< SBR prewhitening flag. */
  uint8_t pvc_mode;          /*!< Predictive vector coding mode */
} SBR_HEADER_DATA_BS_INFO;

typedef struct {
  /* Changes in these variables causes a reset of the decoder */
  uint8_t startFreq;   /*!< Index for SBR start frequency */
  uint8_t stopFreq;    /*!< Index for SBR highest frequency */
  uint8_t freqScale;   /*!< 0: linear scale,  1-3 logarithmic scales */
  uint8_t alterScale;  /*!< Flag for coarser frequency resolution */
  uint8_t noise_bands; /*!< Noise bands per octave, read from bitstream*/

  /* don't require reset */
  uint8_t limiterBands; /*!< Index for number of limiter bands per octave */
  uint8_t limiterGains; /*!< Index to select gain limit */
  uint8_t interpolFreq; /*!< Select gain calculation method (1: per QMF channel,
                         0: per SBR band) */
  uint8_t smoothingLength; /*!< Smoothing of gains over time (0: on  1: off) */

} SBR_HEADER_DATA_BS;

typedef struct {
  SBR_SYNC_STATE
  syncState; /*!< The current initialization status of the header */

  uint8_t status; /*!< Flags field used for signaling a reset right before the
                   processing starts and an update from config (e.g. ASC). */
  uint8_t
  frameErrorFlag; /*!< Frame data valid flag. CAUTION: This variable will be
                     overwritten by the flag stored in the element
                     structure. This is necessary because of the frame
                     delay. There it might happen that different slots use
                     the same header. */
  uint8_t numberTimeSlots;       /*!< AAC: 16,15 */
  uint8_t numberOfAnalysisBands; /*!< Number of QMF analysis bands */
  uint8_t timeStep;              /*!< Time resolution of SBR in QMF-slots */
  uint32_t
      sbrProcSmplRate; /*!< SBR processing sampling frequency (!=
                          OutputSamplingRate)        (always: CoreSamplingRate *
                          UpSamplingFactor; even in single rate mode) */

  SBR_HEADER_DATA_BS bs_data;      /*!< current SBR header. */
  SBR_HEADER_DATA_BS bs_dflt;      /*!< Default sbr header. */
  SBR_HEADER_DATA_BS_INFO bs_info; /*!< SBR info. */

  FREQ_BAND_DATA freqBandData; /*!< Pointer to struct #FREQ_BAND_DATA */
  uint8_t pvcIDprev;
} SBR_HEADER_DATA;

typedef SBR_HEADER_DATA *HANDLE_SBR_HEADER_DATA;

typedef struct {
  uint8_t frameClass;                 /*!< Select grid type */
  uint8_t nEnvelopes;                 /*!< Number of envelopes */
  uint8_t borders[MAX_ENVELOPES + 1]; /*!< Envelope borders (in SBR-timeslots,
                                       e.g. mp3PRO: 0..11) */
  uint8_t freqRes[MAX_ENVELOPES];     /*!< Frequency resolution for each envelope
                                       (0=low, 1=high) */
  int8_t tranEnv;                    /*!< Transient envelope, -1 if none */
  uint8_t nNoiseEnvelopes;            /*!< Number of noise envelopes */
  uint8_t
  bordersNoise[MAX_NOISE_ENVELOPES + 1]; /*!< borders of noise envelopes */
  uint8_t pvcBorders[MAX_PVC_ENVELOPES + 1];
  uint8_t noisePosition;
  uint8_t varLength;
} FRAME_INFO;

typedef struct {
  int16_t sfb_nrg_prev[MAX_FREQ_COEFFS]; /*!< Previous envelope (required for
                                             differential-coded values) */
  int16_t
  prevNoiseLevel[MAX_NOISE_COEFFS]; /*!< Previous noise envelope (required
                                       for differential-coded values) */
  COUPLING_MODE coupling;           /*!< Stereo-mode of previous frame */
  INVF_MODE sbr_invf_mode[MAX_INVF_BANDS]; /*!< Previous strength of filtering
                                              in transposer */
  uint8_t ampRes;         /*!< Previous amplitude resolution (0: 1.5dB, 1: 3dB) */
  uint8_t stopPos;        /*!< Position in time where last envelope ended */
  uint8_t frameErrorFlag; /*!< Previous frame status */
  uint8_t prevSbrPitchInBins; /*!< Previous frame pitchInBins */
  FRAME_INFO prevFrameInfo;
} SBR_PREV_FRAME_DATA;

typedef SBR_PREV_FRAME_DATA *HANDLE_SBR_PREV_FRAME_DATA;

typedef struct {
  int32_t nScaleFactors; /*!< total number of scalefactors in frame */

  FRAME_INFO frameInfo;            /*!< time grid for current frame */
  uint8_t domain_vec[MAX_ENVELOPES]; /*!< Bitfield containing direction of
                                      delta-coding for each envelope
                                      (0:frequency, 1:time) */
  uint8_t domain_vec_noise
      [MAX_NOISE_ENVELOPES]; /*!< Same as above, but for noise envelopes */

  INVF_MODE
  sbr_invf_mode[MAX_INVF_BANDS]; /*!< Strength of filtering in transposer */
  COUPLING_MODE coupling;        /*!< Stereo-mode */
  int32_t ampResolutionCurrentFrame; /*!< Amplitude resolution of envelope values
                                    (0: 1.5dB, 1: 3dB) */

  uint32_t addHarmonics[ADD_HARMONICS_FLAGS_SIZE]; /*!< Flags for synthetic sine
                                                   addition (aligned to MSB) */

  int16_t iEnvelope[MAX_NUM_ENVELOPE_VALUES];       /*!< Envelope data */
  int16_t sbrNoiseFloorLevel[MAX_NUM_NOISE_VALUES]; /*!< Noise envelope data */
  uint8_t iTESactive; /*!< One flag for each envelope to enable USAC inter-TES */
  uint8_t
  interTempShapeMode[MAX_ENVELOPES]; /*!< USAC inter-TES:
                                        bs_inter_temp_shape_mode[ch][env]
                                        value */
  uint8_t pvcID[PVC_NTIMESLOT];        /*!< One PVC ID value for each time slot */
  uint8_t ns;
  uint8_t sinusoidal_position;

  uint8_t sbrPatchingMode;
  uint8_t sbrOversamplingFlag;
  uint8_t sbrPitchInBins;
} SBR_FRAME_DATA;

typedef SBR_FRAME_DATA *HANDLE_SBR_FRAME_DATA;

/*!
\brief   Maps sampling frequencies to frequencies for which setup tables are
available

Maps arbitary sampling frequency to nearest neighbors for which setup tables
are available (e.g. 25600 -> 24000).
Used for startFreq calculation.
The mapping is defined in 14496-3 (4.6.18.2.6), fs(SBR), and table 4.82

\return  mapped sampling frequency
*/
uint32_t sbrdec_mapToStdSampleRate(uint32_t fs,
                               uint32_t isUsac); /*!< Output sampling frequency */

void initSbrPrevFrameData(HANDLE_SBR_PREV_FRAME_DATA h_prev_data,
                          int32_t timeSlots);

int32_t sbrGetChannelElement(HANDLE_SBR_HEADER_DATA hHeaderData,
                         HANDLE_SBR_FRAME_DATA hFrameDataLeft,
                         HANDLE_SBR_FRAME_DATA hFrameDataRight,
                         HANDLE_SBR_PREV_FRAME_DATA hFrameDataLeftPrev,
                         uint8_t pvc_mode_last, HANDLE_FDK_BITSTREAM hBitBuf,
                         HANDLE_PS_DEC hParametricStereoDec, const uint32_t flags,
                         const int32_t overlap);

SBR_HEADER_STATUS
sbrGetHeaderData(HANDLE_SBR_HEADER_DATA headerData,
                 HANDLE_FDK_BITSTREAM hBitBuf, const uint32_t flags,
                 const int32_t fIsSbrData, const uint8_t configMode);

/*!
  \brief     Initialize SBR header data

  Copy default values to the header data struct and patch some entries
  depending on the core codec.
*/
SBR_ERROR
initHeaderData(HANDLE_SBR_HEADER_DATA hHeaderData, const int32_t sampleRateIn,
               const int32_t sampleRateOut, const int32_t downscaleFactor,
               const int32_t samplesPerFrame, const uint32_t flags,
               const int32_t setDefaultHdr);
#endif

/* Convert headroom bits to exponent */
#define SCALE2EXP(s) (15 - (s))
#define EXP2SCALE(e) (15 - (e))
