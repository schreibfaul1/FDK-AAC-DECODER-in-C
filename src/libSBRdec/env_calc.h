

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  Envelope calculation prototypes
*/
#ifndef ENV_CALC_H
#define ENV_CALC_H

#include "../libAACdec/newAACDecoder.h"
#include "sbrdecoder.h"
#include "env_extr.h"

typedef struct {
  int32_t filtBuffer[MAX_FREQ_COEFFS];      /*!< previous gains (required for
                                                smoothing) */
  int32_t filtBufferNoise[MAX_FREQ_COEFFS]; /*!< previous noise levels
                                                (required for smoothing) */
  int8_t filtBuffer_e[MAX_FREQ_COEFFS];       /*!< Exponents of previous gains */
  int8_t filtBufferNoise_e; /*!< Common exponent of previous noise levels */

  int32_t startUp;     /*!< flag to signal initial conditions in buffers */
  int32_t phaseIndex;  /*!< Index for randomPase array */
  int32_t prevTranEnv; /*!< The transient envelope of the previous frame. */

  uint32_t harmFlagsPrev[ADD_HARMONICS_FLAGS_SIZE];
  /*!< Words with 16 flags each indicating where a sine was added in the
   * previous frame.*/
  uint8_t harmIndex;     /*!< Current phase of synthetic sine */
  int32_t sbrPatchingMode; /*!< Current patching mode           */

  int16_t prevSbrNoiseFloorLevel[MAX_NOISE_COEFFS];
  uint8_t prevNNfb;
  uint8_t prevNSfb[2];
  uint8_t prevLoSubband;
  uint8_t prevHiSubband;
  uint8_t prev_ov_highSubband;
  uint8_t *prevFreqBandTable[2];
  uint8_t prevFreqBandTableLo[MAX_FREQ_COEFFS / 2 + 1];
  uint8_t prevFreqBandTableHi[MAX_FREQ_COEFFS + 1];
  uint8_t prevFreqBandTableNoise[MAX_NOISE_COEFFS + 1];
  int8_t sinusoidal_positionPrev;
  uint32_t harmFlagsPrevActive[ADD_HARMONICS_FLAGS_SIZE];
} SBR_CALCULATE_ENVELOPE;

typedef SBR_CALCULATE_ENVELOPE *HANDLE_SBR_CALCULATE_ENVELOPE;

void calculateSbrEnvelope(
    QMF_SCALE_FACTOR *sbrScaleFactor,
    HANDLE_SBR_CALCULATE_ENVELOPE h_sbr_cal_env,
    HANDLE_SBR_HEADER_DATA hHeaderData, HANDLE_SBR_FRAME_DATA hFrameData,
    PVC_DYNAMIC_DATA *pPvcDynamicData, int32_t **analysBufferReal,
    int32_t *
        *analysBufferImag, /*!< Imag part of subband samples to be processed */
    const int32_t useLP,
    int32_t *degreeAlias, /*!< Estimated aliasing for each QMF channel */
    const uint32_t flags, const int32_t frameErrorFlag);

SBR_ERROR
createSbrEnvelopeCalc(HANDLE_SBR_CALCULATE_ENVELOPE hSbrCalculateEnvelope,
                      HANDLE_SBR_HEADER_DATA hHeaderData, const int32_t chan,
                      const uint32_t flags);

int32_t deleteSbrEnvelopeCalc(HANDLE_SBR_CALCULATE_ENVELOPE hSbrCalculateEnvelope);

void resetSbrEnvelopeCalc(HANDLE_SBR_CALCULATE_ENVELOPE hCalEnv);

SBR_ERROR
ResetLimiterBands(uint8_t *limiterBandTable, uint8_t *noLimiterBands,
                  uint8_t *freqBandTable, int32_t noFreqBands,
                  const PATCH_PARAM *patchParam, int32_t noPatches,
                  int32_t limiterBands, uint8_t sbrPatchingMode,
                  int32_t xOverQmf[MAX_NUM_PATCHES], int32_t sbrRatio);

void rescaleSubbandSamples(int32_t **re, int32_t **im, int32_t lowSubband,
                           int32_t noSubbands, int32_t start_pos, int32_t next_pos,
                           int32_t shift);

int32_t maxSubbandSample(int32_t **analysBufferReal_m,
                          int32_t **analysBufferImag_m, int32_t lowSubband,
                          int32_t highSubband, int32_t start_pos, int32_t stop_pos);

#endif  // ENV_CALC_H
