

/*********************** MPEG surround decoder library *************************

   Author(s):   Christian Ertel, Christian Griebel

   Description: SAC Dec error concealment

*******************************************************************************/

#ifndef SAC_DEC_CONCEAL_H
#define SAC_DEC_CONCEAL_H
#include "../libAACdec/newAACDecoder.h"
#include "sac_dec_interface.h"

/* Modules dynamic parameters: */
typedef enum {
  SAC_DEC_CONCEAL_METHOD = 0,
  SAC_DEC_CONCEAL_NUM_KEEP_FRAMES,
  SAC_DEC_CONCEAL_FADE_OUT_SLOPE_LENGTH,
  SAC_DEC_CONCEAL_FADE_IN_SLOPE_LENGTH,
  SAC_DEC_CONCEAL_NUM_RELEASE_FRAMES

} SAC_DEC_CONCEAL_PARAM;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* sac_dec_interface.h                                 */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - */
typedef enum {
  SAC_DEC_CONCEAL_WITH_ZERO_VALUED_OUTPUT = 0,
  SAC_DEC_CONCEAL_BY_FADING_PARAMETERS = 1

} SpatialDecConcealmentMethod;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Default dynamic parameter values: */
#define MPEGS_CONCEAL_DEFAULT_METHOD SAC_DEC_CONCEAL_BY_FADING_PARAMETERS
#define MPEGS_CONCEAL_DEFAULT_NUM_KEEP_FRAMES (10)
#define MPEGS_CONCEAL_DEFAULT_FADE_OUT_SLOPE_LENGTH (5)
#define MPEGS_CONCEAL_DEFAULT_FADE_IN_SLOPE_LENGTH (5)
#define MPEGS_CONCEAL_DEFAULT_NUM_RELEASE_FRAMES (3)

typedef enum {
  SpatialDecConcealState_Init = 0,
  SpatialDecConcealState_Ok,
  SpatialDecConcealState_Keep,
  SpatialDecConcealState_FadeToDefault,
  SpatialDecConcealState_Default,
  SpatialDecConcealState_FadeFromDefault

} SpatialDecConcealmentState;

typedef struct {
  SpatialDecConcealmentMethod method;

  uint32_t numKeepFrames;
  uint32_t numFadeOutFrames;
  uint32_t numFadeInFrames;
  uint32_t numReleaseFrames;

} SpatialDecConcealmentParams;

typedef struct {
  SpatialDecConcealmentParams concealParams; /* User set params */
  SpatialDecConcealmentState
      concealState; /* State of internal state machine (fade-in/out etc) */

  uint32_t cntStateFrames; /* Counter for fade-in/out handling */
  uint32_t cntValidFrames; /* Counter for the number of consecutive good frames*/

} SpatialDecConcealmentInfo;

/* Module reset flags */
#define MPEGS_CONCEAL_RESET_STATE (0x01)
#define MPEGS_CONCEAL_RESET_PARAMETER (0x02)
#define MPEGS_CONCEAL_RESET_ALL (0xFF)

void SpatialDecConcealment_Init(SpatialDecConcealmentInfo *info,
                                const uint32_t resetFlags);

int32_t SpatialDecConcealment_Apply(SpatialDecConcealmentInfo *info,
                                const int8_t (*cmpIdxData)[MAX_PARAMETER_BANDS],
                                int8_t **diffIdxData, int8_t *idxPrev,
                                int8_t *bsXXXDataMode, const int32_t startBand,
                                const int32_t stopBand, const int8_t defaultValue,
                                const int32_t paramType, const int32_t numParamSets);

void SpatialDecConcealment_UpdateState(SpatialDecConcealmentInfo *info,
                                       const int32_t frameOk);

SACDEC_ERROR SpatialDecConcealment_SetParam(SpatialDecConcealmentInfo *info,
                                            const SAC_DEC_CONCEAL_PARAM param,
                                            const int32_t value);

#endif /* SAC_DEC_CONCEAL_H */
