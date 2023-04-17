
/******************* Library for basic calculation routines ********************

   Author(s):   Markus Lohwasser

   Description: FDK Tools Decorrelator

*******************************************************************************/

#ifndef FDK_DECORRELATE_H
#define FDK_DECORRELATE_H
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"

#define FIXP_MPS int32_t

#define FIXP_DECORR int16_t
#define FX_DECORR2FX_DBL FX_SGL2FX_DBL
#define FX_DECORR2FX_SGL
#define FX_DBL2FX_DECORR FX_DBL2FX_SGL
#define FX_SGL2FX_DECORR
#define FL2FXCONST_DECORR FL2FXCONST_SGL


/*--------------- enums -------------------------------*/

/**
 * Decorrelator types.
 */
typedef enum {
  DECORR_MPS,  /**< Decorrelator type used by MPS LP/HQ */
  DECORR_PS,   /**< Decorrelator type used by HEAACv2 and MPS LP */
  DECORR_USAC, /**< Decorrelator type used by USAC */
  DECORR_LD    /**< Decorrelator type used by MPS Low Delay */
} FDK_DECORR_TYPE;

/**
 * Ducker types.
 */
typedef enum {
  DUCKER_AUTOMATIC, /**< FDKdecorrelateInit() chooses correct ducker type
                       depending on provided parameters. */
  DUCKER_MPS,       /**< Force ducker type to MPS. */
  DUCKER_PS         /**< Force ducker type to PS. */
} FDK_DUCKER_TYPE;

/**
 * Reverb band types.
 */
typedef enum {
  NOT_EXIST, /**< Mark reverb band as non-existing (number of bands = 0). */
  DELAY, /**< Reverb bands just contains delay elements and no allpass filters.
          */
  COMMON_REAL,  /**< Real filter coeffs, common filter coeffs within one reverb
                   band */
  COMMON_CPLX,  /**< Complex filter coeffs, common filter coeffs within one
                   reverb band */
  INDEP_CPLX,   /**< Complex filter coeffs, independent filter coeffs for each
                   hybrid band */
  INDEP_CPLX_PS /**< PS optimized implementation of general INDEP_CPLX type */
} REVBAND_FILT_TYPE;

typedef struct DECORR_DEC *HANDLE_DECORR_DEC;

typedef struct DUCKER_INSTANCE {
  int32_t hybridBands;
  int32_t parameterBands;
  int32_t partiallyComplex;
  FDK_DUCKER_TYPE duckerType;

  const uint8_t *qs_next;
  const uint8_t *mapProcBands2HybBands;
  const uint8_t *mapHybBands2ProcBands;
  /* interleaved     SmoothDirectNrg[] and SmoothReverbNrg[],
     non-interleaved SmoothDirectNrg[] in case of parametric stereo */
  FIXP_MPS SmoothDirRevNrg[2 * (28)];

  /*
    parametric stereo
  */
  FIXP_MPS peakDecay[(28)];
  FIXP_MPS peakDiff[(28)];
  int32_t maxValDirectData;
  int32_t maxValReverbData;
  int8_t scaleDirectNrg;
  int8_t scaleReverbNrg;
  int8_t scaleSmoothDirRevNrg;
  int8_t headroomSmoothDirRevNrg;

} DUCKER_INSTANCE;

typedef struct DECORR_FILTER_INSTANCE {
  FIXP_MPS *stateCplx;
  int32_t *DelayBufferCplx;

  const FIXP_DECORR *numeratorReal;
  const FIXP_SPK_t *coeffsPacked;
  const FIXP_DECORR *denominatorReal;
} DECORR_FILTER_INSTANCE;

typedef struct DECORR_DEC {
  int32_t L_stateBufferCplx;
  int32_t *stateBufferCplx;
  int32_t L_delayBufferCplx;
  int32_t *delayBufferCplx;

  const REVBAND_FILT_TYPE *REV_filtType;
  const uint8_t *REV_bandOffset;
  const uint8_t *REV_delay;
  const int8_t *REV_filterOrder;
  int32_t reverbBandDelayBufferIndex[(4)];
  uint8_t stateBufferOffset[(3)];

  DECORR_FILTER_INSTANCE Filter[(71)];
  DUCKER_INSTANCE ducker;

  int32_t numbins;
  int32_t partiallyComplex;
} DECORR_DEC;

/**
 * \brief  Create one instance of Decorrelator.
 *
 * \param hDecorrDec          A pointer to a decorrelator instance which was
 * allocated externally.
 * \param bufferCplx          Externally allocated buffer (allocate (2*( ( 825 )
 * + ( 373 ) )) int32_t values).
 * \param bufLen              Length of bufferCplx. Must be >= (2*( ( 825 ) + (
 * 373 ) )).
 *
 * \return  0 on success.
 */
int32_t FDKdecorrelateOpen(HANDLE_DECORR_DEC hDecorrDec, int32_t *bufferCplx,
                       const int32_t bufLen);

/**
 * \brief  Initialize and configure Decorrelator instance.
 *
 * \param hDecorrDec          A Decorrelator handle.
 * \param nrHybBands          Number of (hybrid) bands.
 * \param decorrType          Decorrelator type to use.
 * \param duckerType          Ducker type to use (in general use
 * DUCKER_AUTOMATIC).
 * \param decorrConfig        Depending on decorrType values of 0,1,2 are
 * allowed.
 * \param seed                Seed of decorrelator instance. Allowed maximum
 * valued depends on decorrType.
 * \param partiallyComplex    Low power or high quality processing 0: HQ, 1: LQ
 * (only allowed for DECORR_MPS | DECORR_PS).
 * \param useFractDelay       Indicate usage of fractional delay 0: off, 1: on
 * (currently not supported).
 * \param isLegacyPS          Indicate if DECORR_PS is used for HEAACv2 (for all
 * other cases: isLegacyPS = 0). The purpose of this parameter is to select the
 * correct number of param bands for the ducker.
 * \param initStatesFlag      Indicates whether the states buffer has to be
 * cleared.
 *
 * \return  0 on success.
 */
int32_t FDKdecorrelateInit(HANDLE_DECORR_DEC hDecorrDec, const int32_t nrHybBands,
                       const FDK_DECORR_TYPE decorrType,
                       const FDK_DUCKER_TYPE duckerType, const int32_t decorrConfig,
                       const int32_t seed, const int32_t partiallyComplex,
                       const int32_t useFractDelay, const int32_t isLegacyPS,
                       const int32_t initStatesFlag);

/**
 * \brief  Apply Decorrelator on input data.
 *
 * Function applies decorrelator and ducker inplace on hybrid input data.
 * Modified hybrid data will be returned inplace.
 *
 * \param hDecorrDec          A decorrelator handle.
 * \param dataRealIn          In (hybrid) data.
 * \param dataImagIn          In (hybrid) data.
 * \param dataRealOut         Out (hybrid) data (can be same as dataRealIn for
 * in-place calculation).
 * \param dataImagOut         Out (hybrid) data (can be same as dataImagIn for
 * in-place calculation).
 * \param startHybBand        Hybrid band to start with decorrelation.
 *
 * \return  0 on success.
 */
int32_t FDKdecorrelateApply(HANDLE_DECORR_DEC hDecorrDec, int32_t *dataRealIn,
                        int32_t *dataImagIn, int32_t *dataRealOut,
                        int32_t *dataImagOut, const int32_t startHybBand);

/**
 * \brief  Destroy a Decorrelator instance.
 *
 * Deallocate whole memory of decorraltor and inside ducker.
 *
 * \param hDecorrDec  Pointer to a decoderrolator handle. Null initialized on
 * return.
 *
 * \return  0 on success.
 */
int32_t FDKdecorrelateClose(HANDLE_DECORR_DEC hDecorrDec);

/**
 * \brief  Get max value address of direct signal.
 *
 * Get max value address of direct signal needed for ducker energy calculation.
 *
 * \param hDecorrDec  Pointer to a decoderrolator handle.
 *
 * \return  address of max value
 */
int32_t *getAddrDirectSignalMaxVal(HANDLE_DECORR_DEC hDecorrDec);

#endif /* FDK_DECORRELATE_H */
