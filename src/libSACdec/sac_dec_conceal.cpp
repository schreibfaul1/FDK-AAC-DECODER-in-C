

/*********************** MPEG surround decoder library *************************

   Author(s):   Christian Ertel, Christian Griebel

   Description: SAC Dec error concealment

*******************************************************************************/
#include "../libAACdec/newAACDecoder.h"
#include "sac_dec_conceal.h"

void SpatialDecConcealment_Init(SpatialDecConcealmentInfo *info,
                                const uint32_t resetFlags) {
  assert(info != NULL);

  if (resetFlags & MPEGS_CONCEAL_RESET_STATE) {
    info->concealState = SpatialDecConcealState_Init;
    /* Frame counters will be initialized implicitely in function
     * SpatialDecConcealment_UpdateState(). */
  }

  if (resetFlags & MPEGS_CONCEAL_RESET_PARAMETER) {
    /* Set default params */
    info->concealParams.method = MPEGS_CONCEAL_DEFAULT_METHOD;
    info->concealParams.numKeepFrames = MPEGS_CONCEAL_DEFAULT_NUM_KEEP_FRAMES;
    info->concealParams.numFadeOutFrames =
        MPEGS_CONCEAL_DEFAULT_FADE_OUT_SLOPE_LENGTH;
    info->concealParams.numFadeInFrames =
        MPEGS_CONCEAL_DEFAULT_FADE_IN_SLOPE_LENGTH;
    info->concealParams.numReleaseFrames =
        MPEGS_CONCEAL_DEFAULT_NUM_RELEASE_FRAMES;
  }

  return;
}

int32_t SpatialDecConcealment_Apply(
    SpatialDecConcealmentInfo *info,
    const int8_t (*cmpIdxData)[MAX_PARAMETER_BANDS], int8_t **diffIdxData,
    int8_t *
        idxPrev, /* char
                    idxPrev[SPATIALDEC_MAX_NUM_OTT][SPATIALDEC_MAX_PARAMETER_BANDS],
                  */
    int8_t *bsXXXDataMode, const int32_t startBand, const int32_t stopBand,
    const int8_t defaultValue, const int32_t paramType, const int32_t numParamSets) {
  int32_t appliedProcessing = 0;
  int32_t band, dataMode = -1;

  assert(info != NULL);
  assert(cmpIdxData != NULL);
  assert(idxPrev != NULL);
  assert(bsXXXDataMode != NULL);

  /* Processing depends only on the internal state */
  switch (info->concealState) {
    case SpatialDecConcealState_Init:
      dataMode = 0; /* default */
      break;

    case SpatialDecConcealState_Ok:
      /* Nothing to do */
      break;

    case SpatialDecConcealState_Keep:
      dataMode = 1; /* keep */
      break;

    case SpatialDecConcealState_FadeToDefault: {
      /* Start simple fade out */
      int32_t fac = fDivNorm(info->cntStateFrames + 1,
                              info->concealParams.numFadeOutFrames + 1);

      for (band = startBand; band < stopBand; band += 1) {
        /*            idxPrev = fac * defaultValue + (1-fac) * idxPrev; */
        idxPrev[band] =
            fMultI(fac, defaultValue - idxPrev[band]) + idxPrev[band];
      }
      dataMode = 1; /* keep */
      appliedProcessing = 1;
    } break;

    case SpatialDecConcealState_Default:
      for (band = startBand; band < stopBand; band += 1) {
        idxPrev[band] = defaultValue;
      }
      dataMode = 1; /* keep */
      appliedProcessing = 1;
      break;

    case SpatialDecConcealState_FadeFromDefault: {
      int32_t fac = fDivNorm(info->cntValidFrames + 1,
                              info->concealParams.numFadeInFrames + 1);

      for (band = startBand; band < stopBand; band += 1) {
        /*            idxPrev = fac * cmpIdxData + (1-fac) * defaultValue; */
        idxPrev[band] =
            fMultI(fac, cmpIdxData[numParamSets - 1][band] - defaultValue) +
            defaultValue;
      }
      dataMode = 1; /* keep */
      appliedProcessing = 1;
    } break;

    default:
      assert(0); /* All valid states shall be handled above. */
      break;
  }

  if (dataMode >= 0) {
    int32_t i;
    for (i = 0; i < numParamSets; i += 1) {
      bsXXXDataMode[i] = dataMode;
      if (diffIdxData != NULL) {
        for (band = startBand; band < stopBand; band += 1) {
          diffIdxData[i][band] = 0;
        }
      }
    }
  }

  return appliedProcessing;
}

void SpatialDecConcealment_UpdateState(SpatialDecConcealmentInfo *info,
                                       const int32_t frameOk) {
  assert(info != NULL);

  if (frameOk) {
    info->cntValidFrames += 1;
  } else {
    info->cntValidFrames = 0;
  }

  switch (info->concealState) {
    case SpatialDecConcealState_Init:
      if (frameOk) {
        /* NEXT STATE: Ok */
        info->concealState = SpatialDecConcealState_Ok;
        info->cntStateFrames = 0;
      }
      break;

    case SpatialDecConcealState_Ok:
      if (!frameOk) {
        /* NEXT STATE: Keep */
        info->concealState = SpatialDecConcealState_Keep;
        info->cntStateFrames = 0;
      }
      break;

    case SpatialDecConcealState_Keep:
      info->cntStateFrames += 1;
      if (frameOk) {
        /* NEXT STATE: Ok */
        info->concealState = SpatialDecConcealState_Ok;
      } else {
        if (info->cntStateFrames >= info->concealParams.numKeepFrames) {
          if (info->concealParams.numFadeOutFrames == 0) {
            /* NEXT STATE: Default */
            info->concealState = SpatialDecConcealState_Default;
          } else {
            /* NEXT STATE: Fade to default */
            info->concealState = SpatialDecConcealState_FadeToDefault;
            info->cntStateFrames = 0;
          }
        }
      }
      break;

    case SpatialDecConcealState_FadeToDefault:
      info->cntStateFrames += 1;
      if (info->cntValidFrames > 0) {
        /* NEXT STATE: Fade in from default */
        info->concealState = SpatialDecConcealState_FadeFromDefault;
        info->cntStateFrames = 0;
      } else {
        if (info->cntStateFrames >= info->concealParams.numFadeOutFrames) {
          /* NEXT STATE: Default */
          info->concealState = SpatialDecConcealState_Default;
        }
      }
      break;

    case SpatialDecConcealState_Default:
      if (info->cntValidFrames > 0) {
        if (info->concealParams.numFadeInFrames == 0) {
          /* NEXT STATE: Ok */
          info->concealState = SpatialDecConcealState_Ok;
        } else {
          /* NEXT STATE: Fade in from default */
          info->concealState = SpatialDecConcealState_FadeFromDefault;
          info->cntValidFrames = 0;
        }
      }
      break;

    case SpatialDecConcealState_FadeFromDefault:
      info->cntValidFrames += 1;
      if (frameOk) {
        if (info->cntValidFrames >= info->concealParams.numFadeInFrames) {
          /* NEXT STATE: Ok */
          info->concealState = SpatialDecConcealState_Ok;
        }
      } else {
        /* NEXT STATE: Fade to default */
        info->concealState = SpatialDecConcealState_FadeToDefault;
        info->cntStateFrames = 0;
      }
      break;

    default:
      assert(0); /* All valid states should be handled above! */
      break;
  }
}

SACDEC_ERROR SpatialDecConcealment_SetParam(SpatialDecConcealmentInfo *self,
                                            const SAC_DEC_CONCEAL_PARAM param,
                                            const int32_t value) {
  SACDEC_ERROR err = MPS_OK;

  switch (param) {
    case SAC_DEC_CONCEAL_METHOD:
      switch ((SpatialDecConcealmentMethod)value) {
        case SAC_DEC_CONCEAL_WITH_ZERO_VALUED_OUTPUT:
        case SAC_DEC_CONCEAL_BY_FADING_PARAMETERS:
          break;
        default:
          err = MPS_INVALID_PARAMETER;
          goto bail;
      }
      if (self != NULL) {
        /* store parameter value */
        self->concealParams.method = (SpatialDecConcealmentMethod)value;
      } else {
        err = MPS_INVALID_HANDLE;
        goto bail;
      }
      break;
    case SAC_DEC_CONCEAL_NUM_KEEP_FRAMES:
      if (value < 0) {
        err = MPS_INVALID_PARAMETER;
        goto bail;
      }
      if (self != NULL) {
        /* store parameter value */
        self->concealParams.numKeepFrames = (uint32_t)value;
      } else {
        err = MPS_INVALID_HANDLE;
        goto bail;
      }
      break;
    case SAC_DEC_CONCEAL_FADE_OUT_SLOPE_LENGTH:
      if (value < 0) {
        err = MPS_INVALID_PARAMETER;
        goto bail;
      }
      if (self != NULL) {
        /* store parameter value */
        self->concealParams.numFadeOutFrames = (uint32_t)value;
      } else {
        err = MPS_INVALID_HANDLE;
        goto bail;
      }
      break;
    case SAC_DEC_CONCEAL_FADE_IN_SLOPE_LENGTH:
      if (value < 0) {
        err = MPS_INVALID_PARAMETER;
        goto bail;
      }
      if (self != NULL) {
        /* store parameter value */
        self->concealParams.numFadeInFrames = (uint32_t)value;
      } else {
        err = MPS_INVALID_HANDLE;
        goto bail;
      }
      break;
    case SAC_DEC_CONCEAL_NUM_RELEASE_FRAMES:
      if (value < 0) {
        err = MPS_INVALID_PARAMETER;
        goto bail;
      }
      if (self != NULL) {
        /* store parameter value */
        self->concealParams.numReleaseFrames = (uint32_t)value;
      } else {
        err = MPS_INVALID_HANDLE;
        goto bail;
      }
      break;
    default:
      err = MPS_INVALID_PARAMETER;
      goto bail;
  }

bail:
  return err;
}
