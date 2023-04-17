

/**************************** PCM utility library ******************************

   Author(s):   Matthias Neusinger

   Description: Hard limiter for clipping prevention

*******************************************************************************/

#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include "../libAACdec/newAACDecoder.h"
#include "limiter.h"
#include "../libFDK/FDK_core.h"

/* library version */
#include "version.h"
/* library title */
#define TDLIMIT_LIB_TITLE "TD Limiter Lib"

/* create limiter */
TDLimiterPtr pcmLimiter_Create(uint32_t maxAttackMs, uint32_t releaseMs,
                               int32_t threshold, uint32_t maxChannels,
                               uint32_t maxSampleRate) {
  TDLimiterPtr limiter = NULL;
  uint32_t attack, release;
  int32_t attackConst, releaseConst, exponent;
  int32_t e_ans;

  /* calc attack and release time in samples */
  attack = (uint32_t)(maxAttackMs * maxSampleRate / 1000);
  release = (uint32_t)(releaseMs * maxSampleRate / 1000);

  /* alloc limiter struct */
  limiter = (TDLimiterPtr)FDKcalloc(1, sizeof(struct TDLimiter));
  if (!limiter) return NULL;

  /* alloc max and delay buffers */
  limiter->maxBuf = (int32_t*)FDKcalloc(attack + 1, sizeof(int32_t));
  limiter->delayBuf =
      (int32_t*)FDKcalloc(attack * maxChannels, sizeof(int32_t));

  if (!limiter->maxBuf || !limiter->delayBuf) {
    pcmLimiter_Destroy(limiter);
    return NULL;
  }

  /* attackConst = pow(0.1, 1.0 / (attack + 1)) */
  exponent = invFixp(attack + 1);
  attackConst = fPow(FL2FXCONST_DBL(0.1f), 0, exponent, 0, &e_ans);
  attackConst = scaleValue(attackConst, e_ans);

  /* releaseConst  = (float)pow(0.1, 1.0 / (release + 1)) */
  exponent = invFixp(release + 1);
  releaseConst = fPow(FL2FXCONST_DBL(0.1f), 0, exponent, 0, &e_ans);
  releaseConst = scaleValue(releaseConst, e_ans);

  /* init parameters */
  limiter->attackMs = maxAttackMs;
  limiter->maxAttackMs = maxAttackMs;
  limiter->releaseMs = releaseMs;
  limiter->attack = attack;
  limiter->attackConst = attackConst;
  limiter->releaseConst = releaseConst;
  limiter->threshold = threshold;
  limiter->channels = maxChannels;
  limiter->maxChannels = maxChannels;
  limiter->sampleRate = maxSampleRate;
  limiter->maxSampleRate = maxSampleRate;

  pcmLimiter_Reset(limiter);

  return limiter;
}

/* apply limiter */
TDLIMITER_ERROR pcmLimiter_Apply(TDLimiterPtr limiter, PCM_LIM* samplesIn,
                                 int16_t* samplesOut, int32_t* pGainPerSample,
                                 const int32_t scaling, const uint32_t nSamples) {
  uint32_t i, j;
  int32_t tmp2;
  int32_t tmp, old, gain, additionalGain = 0;
  int32_t minGain = FL2FXCONST_DBL(1.0f / (1 << 1));
  uint32_t additionalGainAvailable = 1;

  if (limiter == NULL) return TDLIMIT_INVALID_HANDLE;

  {
    uint32_t channels = limiter->channels;
    uint32_t attack = limiter->attack;
    int32_t attackConst = limiter->attackConst;
    int32_t releaseConst = limiter->releaseConst;
    int32_t threshold = limiter->threshold >> scaling;

    int32_t max = limiter->max;
    int32_t* maxBuf = limiter->maxBuf;
    uint32_t maxBufIdx = limiter->maxBufIdx;
    int32_t cor = limiter->cor;
    int32_t* delayBuf = limiter->delayBuf;
    uint32_t delayBufIdx = limiter->delayBufIdx;

    int32_t smoothState0 = limiter->smoothState0;

    if (limiter->scaling != scaling) {
      scaleValuesSaturate(delayBuf, attack * channels,
                          limiter->scaling - scaling);
      scaleValuesSaturate(maxBuf, attack + 1, limiter->scaling - scaling);
      max = scaleValueSaturate(max, limiter->scaling - scaling);
      limiter->scaling = scaling;
    }

    if (pGainPerSample == NULL) {
      additionalGainAvailable = 0;
    }

    for (i = 0; i < nSamples; i++) {
      /* get maximum absolute sample value of all channels, including the
       * additional gain. */
      tmp = (int32_t)0;
      for (j = 0; j < channels; j++) {
        tmp2 = PCM_LIM2FIXP_DBL(samplesIn[j]);
        tmp2 =
            (tmp2 == (int32_t)MINVAL_DBL) ? (int32_t)MAXVAL_DBL : fAbs(tmp2);
        tmp = fMax(tmp, tmp2);
      }

      if (additionalGainAvailable) {
        additionalGain = pGainPerSample[i];
        tmp = fMult(tmp, additionalGain);
      }

      /* set threshold as lower border to save calculations in running maximum
       * algorithm */
      tmp = fMax(tmp, threshold);

      /* running maximum */
      old = maxBuf[maxBufIdx];
      maxBuf[maxBufIdx] = tmp;

      if (tmp >= max) {
        /* new sample is greater than old maximum, so it is the new maximum */
        max = tmp;
      } else if (old < max) {
        /* maximum does not change, as the sample, which has left the window was
           not the maximum */
      } else {
        /* the old maximum has left the window, we have to search the complete
           buffer for the new max */
        max = maxBuf[0];
        for (j = 1; j <= attack; j++) {
          max = fMax(max, maxBuf[j]);
        }
      }
      maxBufIdx++;
      if (maxBufIdx >= attack + 1) maxBufIdx = 0;

      /* calc gain */
      /* gain is downscaled by one, so that gain = 1.0 can be represented */
      if (max > threshold) {
        gain = fDivNorm(threshold, max) >> 1;
      } else {
        gain = FL2FXCONST_DBL(1.0f / (1 << 1));
      }

      /* gain smoothing, method: TDL_EXPONENTIAL */
      /* first order IIR filter with attack correction to avoid overshoots */

      /* correct the 'aiming' value of the exponential attack to avoid the
       * remaining overshoot */
      if (gain < smoothState0) {
        cor = fMin(cor, fMultDiv2((gain - fMultDiv2((int16_t)6554, smoothState0)), (int16_t)18204) << 2);
      } else {
        cor = gain;
      }

      /* smoothing filter */
      if (cor < smoothState0) {
        smoothState0 =
            fMult(attackConst, (smoothState0 - cor)) + cor; /* attack */
        smoothState0 = fMax(smoothState0, gain); /* avoid overshooting target */
      } else {
        /* sign inversion twice to round towards +infinity,
           so that gain can converge to 1.0 again,
           for bit-identical output when limiter is not active */
        smoothState0 =
            -fMult(releaseConst, -(smoothState0 - cor)) + cor; /* release */
      }

      gain = smoothState0;

      int32_t* p_delayBuf = &delayBuf[delayBufIdx * channels + 0];
      if (gain < FL2FXCONST_DBL(1.0f / (1 << 1))) {
        gain <<= 1;
        /* lookahead delay, apply gain */
        for (j = 0; j < channels; j++) {
          tmp = p_delayBuf[j];

          if (additionalGainAvailable) {
            p_delayBuf[j] = fMult((FIXP_PCM_LIM)samplesIn[j], additionalGain);
          } else {
            p_delayBuf[j] = PCM_LIM2FIXP_DBL(samplesIn[j]);
          }

          /* Apply gain to delayed signal */
          tmp = fMultDiv2(tmp, gain);
#if (SAMPLE_BITS == DFRACT_BITS)
          samplesOut[j] = (int16_t)FX_DBL2FX_PCM(
              (int32_t)SATURATE_LEFT_SHIFT(tmp, scaling + 1, DFRACT_BITS));
#else
          samplesOut[j] = (int16_t)FX_DBL2FX_PCM((int32_t)SATURATE_LEFT_SHIFT(
              tmp + ((int32_t)0x8000 >> (scaling + 1)), scaling + 1,
              DFRACT_BITS));
#endif
        }
        gain >>= 1;
      } else {
        /* lookahead delay, apply gain=1.0f */
        for (j = 0; j < channels; j++) {
          tmp = p_delayBuf[j];
          if (additionalGainAvailable) {
            p_delayBuf[j] = fMult((FIXP_PCM_LIM)samplesIn[j], additionalGain);
          } else {
            p_delayBuf[j] = PCM_LIM2FIXP_DBL(samplesIn[j]);
          }

#if (SAMPLE_BITS == DFRACT_BITS)
          samplesOut[j] = (int16_t)FX_DBL2FX_PCM(
              (int32_t)SATURATE_LEFT_SHIFT(tmp, scaling, DFRACT_BITS));
#else
          samplesOut[j] = (int16_t)FX_DBL2FX_PCM((int32_t)SATURATE_LEFT_SHIFT(
              tmp + ((int32_t)0x8000 >> scaling), scaling, DFRACT_BITS));
#endif
        }
      }

      delayBufIdx++;
      if (delayBufIdx >= attack) {
        delayBufIdx = 0;
      }

      /* save minimum gain factor */
      if (gain < minGain) {
        minGain = gain;
      }

      /* advance sample pointer by <channel> samples */
      samplesIn += channels;
      samplesOut += channels;
    }

    limiter->max = max;
    limiter->maxBufIdx = maxBufIdx;
    limiter->cor = cor;
    limiter->delayBufIdx = delayBufIdx;

    limiter->smoothState0 = smoothState0;

    limiter->minGain = minGain;

    return TDLIMIT_OK;
  }
}

/* set limiter threshold */
TDLIMITER_ERROR pcmLimiter_SetThreshold(TDLimiterPtr limiter,
                                        int32_t threshold) {
  if (limiter == NULL) return TDLIMIT_INVALID_HANDLE;

  limiter->threshold = threshold;

  return TDLIMIT_OK;
}

/* reset limiter */
TDLIMITER_ERROR pcmLimiter_Reset(TDLimiterPtr limiter) {
  if (limiter != NULL) {
    limiter->maxBufIdx = 0;
    limiter->delayBufIdx = 0;
    limiter->max = (int32_t)0;
    limiter->cor = FL2FXCONST_DBL(1.0f / (1 << 1));
    limiter->smoothState0 = FL2FXCONST_DBL(1.0f / (1 << 1));
    limiter->minGain = FL2FXCONST_DBL(1.0f / (1 << 1));
    limiter->scaling = 0;

    memset(limiter->maxBuf, 0, (limiter->attack + 1) * sizeof(int32_t));
    memset(limiter->delayBuf, 0,
              limiter->attack * limiter->channels * sizeof(int32_t));
  } else {
    return TDLIMIT_INVALID_HANDLE;
  }

  return TDLIMIT_OK;
}

/* destroy limiter */
TDLIMITER_ERROR pcmLimiter_Destroy(TDLimiterPtr limiter) {
  if (limiter != NULL) {
    free(limiter->maxBuf);
    free(limiter->delayBuf);

    free(limiter);
  } else {
    return TDLIMIT_INVALID_HANDLE;
  }
  return TDLIMIT_OK;
}

/* get delay in samples */
uint32_t pcmLimiter_GetDelay(TDLimiterPtr limiter) {
  assert(limiter != NULL);
  return limiter->attack;
}

/* get maximum gain reduction of last processed block */
int32_t pcmLimiter_GetMaxGainReduction(TDLimiterPtr limiter) {
  /* maximum gain reduction in dB = -20 * log10(limiter->minGain)
     = -20 * log2(limiter->minGain)/log2(10) = -6.0206*log2(limiter->minGain) */
  int32_t e_ans;
  int32_t loggain, maxGainReduction;

  assert(limiter != NULL);

  loggain = fLog2(limiter->minGain, 1, &e_ans);

  maxGainReduction = fMult(loggain, FL2FXCONST_DBL(-6.0206f / (1 << 3)));

  return fixp_roundToInt(maxGainReduction, (e_ans + 3));
}

/* set number of channels */
TDLIMITER_ERROR pcmLimiter_SetNChannels(TDLimiterPtr limiter,
                                        uint32_t nChannels) {
  if (limiter == NULL) return TDLIMIT_INVALID_HANDLE;

  if (nChannels > limiter->maxChannels) return TDLIMIT_INVALID_PARAMETER;

  limiter->channels = nChannels;
  // pcmLimiter_Reset(limiter);

  return TDLIMIT_OK;
}

/* set sampling rate */
TDLIMITER_ERROR pcmLimiter_SetSampleRate(TDLimiterPtr limiter,
                                         uint32_t sampleRate) {
  uint32_t attack, release;
  int32_t attackConst, releaseConst, exponent;
  int32_t e_ans;

  if (limiter == NULL) return TDLIMIT_INVALID_HANDLE;

  if (sampleRate > limiter->maxSampleRate) return TDLIMIT_INVALID_PARAMETER;

  /* update attack and release time in samples */
  attack = (uint32_t)(limiter->attackMs * sampleRate / 1000);
  release = (uint32_t)(limiter->releaseMs * sampleRate / 1000);

  /* attackConst = pow(0.1, 1.0 / (attack + 1)) */
  exponent = invFixp(attack + 1);
  attackConst = fPow(FL2FXCONST_DBL(0.1f), 0, exponent, 0, &e_ans);
  attackConst = scaleValue(attackConst, e_ans);

  /* releaseConst  = (float)pow(0.1, 1.0 / (release + 1)) */
  exponent = invFixp(release + 1);
  releaseConst = fPow(FL2FXCONST_DBL(0.1f), 0, exponent, 0, &e_ans);
  releaseConst = scaleValue(releaseConst, e_ans);

  limiter->attack = attack;
  limiter->attackConst = attackConst;
  limiter->releaseConst = releaseConst;
  limiter->sampleRate = sampleRate;

  /* reset */
  // pcmLimiter_Reset(limiter);

  return TDLIMIT_OK;
}

/* set attack time */
TDLIMITER_ERROR pcmLimiter_SetAttack(TDLimiterPtr limiter,
                                     uint32_t attackMs) {
  uint32_t attack;
  int32_t attackConst, exponent;
  int32_t e_ans;

  if (limiter == NULL) return TDLIMIT_INVALID_HANDLE;

  if (attackMs > limiter->maxAttackMs) return TDLIMIT_INVALID_PARAMETER;

  /* calculate attack time in samples */
  attack = (uint32_t)(attackMs * limiter->sampleRate / 1000);

  /* attackConst = pow(0.1, 1.0 / (attack + 1)) */
  exponent = invFixp(attack + 1);
  attackConst = fPow(FL2FXCONST_DBL(0.1f), 0, exponent, 0, &e_ans);
  attackConst = scaleValue(attackConst, e_ans);

  limiter->attack = attack;
  limiter->attackConst = attackConst;
  limiter->attackMs = attackMs;

  return TDLIMIT_OK;
}

/* set release time */
TDLIMITER_ERROR pcmLimiter_SetRelease(TDLimiterPtr limiter,
                                      uint32_t releaseMs) {
  uint32_t release;
  int32_t releaseConst, exponent;
  int32_t e_ans;

  if (limiter == NULL) return TDLIMIT_INVALID_HANDLE;

  /* calculate  release time in samples */
  release = (uint32_t)(releaseMs * limiter->sampleRate / 1000);

  /* releaseConst  = (float)pow(0.1, 1.0 / (release + 1)) */
  exponent = invFixp(release + 1);
  releaseConst = fPow(FL2FXCONST_DBL(0.1f), 0, exponent, 0, &e_ans);
  releaseConst = scaleValue(releaseConst, e_ans);

  limiter->releaseConst = releaseConst;
  limiter->releaseMs = releaseMs;

  return TDLIMIT_OK;
}

/* Get library info for this module. */
TDLIMITER_ERROR pcmLimiter_GetLibInfo(LIB_INFO_t* info) {
  int32_t i;

  if (info == NULL) {
    return TDLIMIT_INVALID_PARAMETER;
  }

  /* Search for next free tab */
  for (i = 0; i < FDK_MODULE_LAST; i++) {
    if (info[i].module_id == FDK_NONE) break;
  }
  if (i == FDK_MODULE_LAST) {
    return TDLIMIT_UNKNOWN;
  }


  /* Set flags */
  info[i].flags = CAPF_LIMITER;

  /* Add lib info for FDK tools (if not yet done). */
  FDK_toolsGetLibInfo(info);

  return TDLIMIT_OK;
}
