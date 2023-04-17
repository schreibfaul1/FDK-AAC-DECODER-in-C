

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  Low Power Profile Transposer
*/

#ifndef LPP_TRAN_H
#define LPP_TRAN_H
#include "../libAACdec/newAACDecoder.h"
#include "sbrdecoder.h"
#include "hbe.h"
#include "../libFDK/qmf.h"

/*
  Common
*/
#define QMF_OUT_SCALE 8

/*
  Frequency scales
*/

/*
  Env-Adjust
*/


#define MAX_GAIN_EXP 34
/* Maximum gain will be sqrt(0.5 * 2^MAX_GAIN_EXP)
   example: 34=99dB   */
#define MAX_GAIN_CONCEAL_EXP 1
/* Maximum gain will be sqrt(0.5 * 2^MAX_GAIN_CONCEAL_EXP) in concealment case
 * (0dB) */

/*
  LPP Transposer
*/
#define LPC_ORDER 2

#define MAX_INVF_BANDS MAX_NOISE_COEFFS

#define MAX_NUM_PATCHES 6
#define SHIFT_START_SB 1 /*!< lowest subband of source range */

typedef enum {
  INVF_OFF = 0,
  INVF_LOW_LEVEL,
  INVF_MID_LEVEL,
  INVF_HIGH_LEVEL,
  INVF_SWITCHED /* not a real choice but used here to control behaviour */
} INVF_MODE;

/** parameter set for one single patch */
typedef struct {
  uint8_t sourceStartBand; /*!< first band in lowbands where to take the samples
                            from */
  uint8_t
  sourceStopBand;       /*!< first band in lowbands which is not included in the
                           patch anymore */
  uint8_t guardStartBand; /*!< first band in highbands to be filled with zeros in
                           order to reduce interferences between patches */
  uint8_t
  targetStartBand;       /*!< first band in highbands to be filled with whitened
                            lowband signal */
  uint8_t targetBandOffs;  /*!< difference between 'startTargetBand' and
                            'startSourceBand' */
  uint8_t numBandsInPatch; /*!< number of consecutive bands in this one patch */
} PATCH_PARAM;

/** whitening factors for different levels of whitening
    need to be initialized corresponding to crossover frequency */
typedef struct {
  int32_t off; /*!< bw factor for signal OFF */
  int32_t transitionLevel;
  int32_t lowLevel;  /*!< bw factor for signal LOW_LEVEL */
  int32_t midLevel;  /*!< bw factor for signal MID_LEVEL */
  int32_t highLevel; /*!< bw factor for signal HIGH_LEVEL */
} WHITENING_FACTORS;

/*! The transposer settings are calculated on a header reset and are shared by
 * both channels. */
typedef struct {
  uint8_t nCols;           /*!< number subsamples of a codec frame */
  uint8_t noOfPatches;     /*!< number of patches */
  uint8_t lbStartPatching; /*!< first band of lowbands that will be patched */
  uint8_t lbStopPatching;  /*!< first band that won't be patched anymore*/
  uint8_t bwBorders[MAX_NUM_NOISE_VALUES]; /*!< spectral bands with different
                                            inverse filtering levels */

  PATCH_PARAM
  patchParam[MAX_NUM_PATCHES]; /*!< new parameter set for patching */
  WHITENING_FACTORS
  whFactors;     /*!< the pole moving factors for certain
                    whitening levels as indicated     in the bitstream
                    depending on the crossover frequency */
  uint8_t overlap; /*!< Overlap size */
} TRANSPOSER_SETTINGS;

typedef struct {
  TRANSPOSER_SETTINGS *pSettings; /*!< Common settings for both channels */
  int32_t
  bwVectorOld[MAX_NUM_PATCHES]; /*!< pole moving factors of past frame */
  int32_t lpcFilterStatesRealLegSBR[LPC_ORDER + (3 * (4))][(
      32)]; /*!< pointer array to save filter states */

  int32_t lpcFilterStatesImagLegSBR[LPC_ORDER + (3 * (4))][(
      32)]; /*!< pointer array to save filter states */

  int32_t lpcFilterStatesRealHBE[LPC_ORDER + (3 * (4))][(
      64)]; /*!< pointer array to save filter states */
  int32_t lpcFilterStatesImagHBE[LPC_ORDER + (3 * (4))][(
      64)]; /*!< pointer array to save filter states */
} SBR_LPP_TRANS;

typedef SBR_LPP_TRANS *HANDLE_SBR_LPP_TRANS;

void lppTransposer(HANDLE_SBR_LPP_TRANS hLppTrans,
                   QMF_SCALE_FACTOR *sbrScaleFactor, int32_t **qmfBufferReal,

                   int32_t *degreeAlias, int32_t **qmfBufferImag,
                   const int32_t useLP, const int32_t fPreWhitening,
                   const int32_t v_k_master0, const int32_t timeStep,
                   const int32_t firstSlotOffset, const int32_t lastSlotOffset,
                   const int32_t nInvfBands, INVF_MODE *sbr_invf_mode,
                   INVF_MODE *sbr_invf_mode_prev);

void lppTransposerHBE(
    HANDLE_SBR_LPP_TRANS hLppTrans, /*!< Handle of lpp transposer  */
    HANDLE_HBE_TRANSPOSER hQmfTransposer,
    QMF_SCALE_FACTOR *sbrScaleFactor, /*!< Scaling factors */
    int32_t **qmfBufferReal, /*!< Pointer to pointer to real part of subband
                                 samples (source) */
    int32_t **qmfBufferImag, /*!< Pointer to pointer to imaginary part of
                                 subband samples (source) */
    const int32_t timeStep,       /*!< Time step of envelope */
    const int32_t firstSlotOffs,  /*!< Start position in time */
    const int32_t lastSlotOffs,   /*!< Number of overlap-slots into next frame */
    const int32_t nInvfBands,     /*!< Number of bands for inverse filtering */
    INVF_MODE *sbr_invf_mode, /*!< Current inverse filtering modes */
    INVF_MODE *sbr_invf_mode_prev /*!< Previous inverse filtering modes */
);

SBR_ERROR
createLppTransposer(HANDLE_SBR_LPP_TRANS hLppTrans,
                    TRANSPOSER_SETTINGS *pSettings, const int32_t highBandStartSb,
                    uint8_t *v_k_master, const int32_t numMaster, const int32_t usb,
                    const int32_t timeSlots, const int32_t nCols, uint8_t *noiseBandTable,
                    const int32_t noNoiseBands, uint32_t fs, const int32_t chan,
                    const int32_t overlap);

SBR_ERROR
resetLppTransposer(HANDLE_SBR_LPP_TRANS hLppTrans, uint8_t highBandStartSb,
                   uint8_t *v_k_master, uint8_t numMaster, uint8_t *noiseBandTable,
                   uint8_t noNoiseBands, uint8_t usb, uint32_t fs);

#endif /* LPP_TRAN_H */
