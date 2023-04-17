

/**************************** PCM utility library ******************************

   Author(s):   Matthias Neusinger

   Description: Hard limiter for clipping prevention

*******************************************************************************/

#ifndef LIMITER_H
#define LIMITER_H
#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/common_fix.h"


#define TDL_ATTACK_DEFAULT_MS (15)  /* default attack  time in ms */
#define TDL_RELEASE_DEFAULT_MS (50) /* default release time in ms */

#ifdef __cplusplus
extern "C" {
#endif

struct TDLimiter {
  uint32_t attack;
  int32_t attackConst, releaseConst;
  uint32_t attackMs, releaseMs, maxAttackMs;
  int32_t threshold;
  uint32_t channels, maxChannels;
  uint32_t sampleRate, maxSampleRate;
  int32_t cor, max;
  int32_t* maxBuf;
  int32_t* delayBuf;
  uint32_t maxBufIdx, delayBufIdx;
  int32_t smoothState0;
  int32_t minGain;
  int32_t scaling;
};

typedef enum {
  TDLIMIT_OK = 0,
  TDLIMIT_UNKNOWN = -1,

  __error_codes_start = -100,

  TDLIMIT_INVALID_HANDLE,
  TDLIMIT_INVALID_PARAMETER,

  __error_codes_end
} TDLIMITER_ERROR;

struct TDLimiter;
typedef struct TDLimiter* TDLimiterPtr;

#define PCM_LIM int32_t
#define FIXP_DBL2PCM_LIM(x) (x)
#define PCM_LIM2FIXP_DBL(x) (x)
#define PCM_LIM_BITS 32
#define FIXP_PCM_LIM int32_t

#define SAMPLE_BITS_LIM DFRACT_BITS

/******************************************************************************
 * pcmLimiter_Reset                                                            *
 * limiter: limiter handle                                                     *
 * returns: error code                                                         *
 ******************************************************************************/
TDLIMITER_ERROR pcmLimiter_Reset(TDLimiterPtr limiter);

/******************************************************************************
 * pcmLimiter_Destroy                                                          *
 * limiter: limiter handle                                                     *
 * returns: error code                                                         *
 ******************************************************************************/
TDLIMITER_ERROR pcmLimiter_Destroy(TDLimiterPtr limiter);

/******************************************************************************
 * pcmLimiter_GetDelay                                                         *
 * limiter: limiter handle                                                     *
 * returns: exact delay caused by the limiter in samples per channel           *
 ******************************************************************************/
uint32_t pcmLimiter_GetDelay(TDLimiterPtr limiter);

/******************************************************************************
 * pcmLimiter_GetMaxGainReduction                                              *
 * limiter: limiter handle                                                     *
 * returns: maximum gain reduction in last processed block in dB               *
 ******************************************************************************/
int32_t pcmLimiter_GetMaxGainReduction(TDLimiterPtr limiter);

/******************************************************************************
 * pcmLimiter_SetNChannels                                                     *
 * limiter:   limiter handle                                                   *
 * nChannels: number of channels ( <= maxChannels specified on create)         *
 * returns:   error code                                                       *
 ******************************************************************************/
TDLIMITER_ERROR pcmLimiter_SetNChannels(TDLimiterPtr limiter,
                                        uint32_t nChannels);

/******************************************************************************
 * pcmLimiter_SetSampleRate                                                    *
 * limiter:    limiter handle                                                  *
 * sampleRate: sampling rate in Hz ( <= maxSampleRate specified on create)     *
 * returns:    error code                                                      *
 ******************************************************************************/
TDLIMITER_ERROR pcmLimiter_SetSampleRate(TDLimiterPtr limiter, uint32_t sampleRate);

/******************************************************************************
 * pcmLimiter_SetAttack                                                        *
 * limiter:    limiter handle                                                  *
 * attackMs:   attack time in ms ( <= maxAttackMs specified on create)         *
 * returns:    error code                                                      *
 ******************************************************************************/
TDLIMITER_ERROR pcmLimiter_SetAttack(TDLimiterPtr limiter,
                                     uint32_t attackMs);

/******************************************************************************
 * pcmLimiter_SetRelease                                                       *
 * limiter:    limiter handle                                                  *
 * releaseMs:  release time in ms                                              *
 * returns:    error code                                                      *
 ******************************************************************************/
TDLIMITER_ERROR pcmLimiter_SetRelease(TDLimiterPtr limiter,
                                      uint32_t releaseMs);

/******************************************************************************
 * pcmLimiter_GetLibInfo                                                       *
 * info:       pointer to an allocated and initialized LIB_INFO structure      *
 * returns:    error code                                                      *
 ******************************************************************************/
TDLIMITER_ERROR pcmLimiter_GetLibInfo(LIB_INFO_t* info);

#ifdef __cplusplus
}
#endif

/******************************************************************************
 * pcmLimiter_Create                                                           *
 * maxAttackMs:   maximum and initial attack/lookahead time in milliseconds    *
 * releaseMs:     release time in milliseconds (90% time constant)             *
 * threshold:     limiting threshold                                           *
 * maxChannels:   maximum and initial number of channels                       *
 * maxSampleRate: maximum and initial sampling rate in Hz                      *
 * returns:       limiter handle                                               *
 ******************************************************************************/
TDLimiterPtr pcmLimiter_Create(uint32_t maxAttackMs, uint32_t releaseMs,
                               int32_t threshold, uint32_t maxChannels,
                               uint32_t maxSampleRate);

/******************************************************************************
 * pcmLimiter_SetThreshold                                                     *
 * limiter:    limiter handle                                                  *
 * threshold:  limiter threshold                                               *
 * returns:    error code                                                      *
 ******************************************************************************/
TDLIMITER_ERROR pcmLimiter_SetThreshold(TDLimiterPtr limiter,
                                        int32_t threshold);

/******************************************************************************
 * pcmLimiter_Apply                                                            *
 * limiter:        limiter handle                                              *
 * samplesIn:      pointer to input buffer containing interleaved samples      *
 * samplesOut:     pointer to output buffer containing interleaved samples     *
 * pGainPerSample: pointer to gains for each sample                            *
 * scaling:        scaling of output samples                                   *
 * nSamples:       number of samples per channel                               *
 * returns:    error code                                                      *
 ******************************************************************************/
TDLIMITER_ERROR pcmLimiter_Apply(TDLimiterPtr limiter, PCM_LIM* samplesIn,
                                 int16_t* samplesOut, int32_t* pGainPerSample,
                                 const int32_t scaling, const uint32_t nSamples);

#endif /* #ifndef LIMITER_H */
