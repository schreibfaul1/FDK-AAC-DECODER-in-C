

/**************************** AAC decoder library ******************************

   Author(s):   Christian Griebel

   Description: Error concealment structs and types

*******************************************************************************/

#ifndef CONCEAL_TYPES_H
#define CONCEAL_TYPES_H

#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/common_fix.h"

#include "rvlc_info.h"

#include "usacdec_lpc.h"

#define CONCEAL_MAX_NUM_FADE_FACTORS (32)

#define FIXP_CNCL int32_t
#define FL2FXCONST_CNCL FL2FXCONST_DBL
#define FX_DBL2FX_CNCL
#define FX_CNCL2FX_DBL
#define CNCL_FRACT_BITS DFRACT_BITS

/* Warning: Do not ever change these values. */
typedef enum {
  ConcealMethodNone = -1,
  ConcealMethodMute = 0,
  ConcealMethodNoise = 1,
  ConcealMethodInter = 2,
  ConcealMethodTonal = 3

} CConcealmentMethod;

typedef enum {
  ConcealState_Ok,
  ConcealState_Single,
  ConcealState_FadeIn,
  ConcealState_Mute,
  ConcealState_FadeOut

} CConcealmentState;

typedef struct {
  int16_t fadeOutFactor[CONCEAL_MAX_NUM_FADE_FACTORS];
  int16_t fadeInFactor[CONCEAL_MAX_NUM_FADE_FACTORS];

  CConcealmentMethod method;

  int32_t numFadeOutFrames;
  int32_t numFadeInFrames;
  int32_t numMuteReleaseFrames;
  int32_t comfortNoiseLevel;

} CConcealParams;

typedef enum {
  FADE_TIMEDOMAIN_TOSPECTRALMUTE = 1,
  FADE_TIMEDOMAIN_FROMSPECTRALMUTE,
  FADE_TIMEDOMAIN
} TDfadingType;

typedef struct {
  CConcealParams *pConcealParams;

  FIXP_CNCL spectralCoefficient[1024];
  int16_t specScale[8];

  int32_t iRandomPhase;
  int32_t prevFrameOk[2];
  int32_t cntValidFrames;
  int32_t cntFadeFrames; /* State for signal fade-in/out */
  /* States for signal fade-out of frames with more than one window/subframe -
    [0] used by Update CntFadeFrames mode of CConcealment_ApplyFadeOut, [1] used
    by FadeOut mode */
  int32_t winGrpOffset[2]; /* State for signal fade-out of frames with more than one
                          window/subframe */
  int32_t attGrpOffset[2]; /* State for faster signal fade-out of frames with
                          transient signal parts */

  int8_t lastRenderMode;

  uint8_t windowShape;
  BLOCK_TYPE windowSequence;
  uint8_t lastWinGrpLen;

  CConcealmentState concealState;
  CConcealmentState concealState_old;
  int32_t fade_old;           /* last fading factor */
  TDfadingType lastFadingType; /* last fading type */

  int16_t aRvlcPreviousScaleFactor[RVLC_MAX_SFB]; /* needed once per channel */
  uint8_t aRvlcPreviousCodebook[RVLC_MAX_SFB];    /* needed once per channel */
  int8_t rvlcPreviousScaleFactorOK;
  int8_t rvlcPreviousBlockType;

  FIXP_LPC lsf4[M_LP_FILTER_ORDER];
  int32_t last_tcx_gain;
  int32_t last_tcx_gain_e;
  uint32_t TDNoiseSeed;
  int32_t TDNoiseStates[3];
  int16_t TDNoiseCoef[3];
  int16_t TDNoiseAtt;

} CConcealmentInfo;

#endif /* #ifndef CONCEAL_TYPES_H */
