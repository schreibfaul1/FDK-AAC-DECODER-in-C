

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  Sbr decoder
*/
#ifndef PSDEC_H
#define PSDEC_H
#include "../libAACdec/newAACDecoder.h"
#include "sbrdecoder.h"
#include "../libFDK/FDK_hybrid.h"

#include "../libFDK/FDK_decorrelate.h"

/* This PS decoder implements the baseline version. So it always uses the     */
/* hybrid filter structure for 20 stereo bands and does not implemet IPD/OPD  */
/* synthesis. The baseline version has to support the complete PS bitstream   */
/* syntax. But IPD/OPD data is ignored and set to 0. If 34 stereo band config */
/* is used in the bitstream for IIS/ICC the decoded parameters are mapped to  */
/* 20 stereo bands.                                                           */

#include "../libFDK/FDK_bitstream.h"

#define SCAL_HEADROOM (2)

#define PS_EXTENSION_SIZE_BITS (4)
#define PS_EXTENSION_ESC_COUNT_BITS (8)

#define NO_QMF_CHANNELS (64)
#define MAX_NUM_COL (32)

#define NO_QMF_BANDS_HYBRID20 (3)
#define NO_SUB_QMF_CHANNELS (12)
#define HYBRID_FILTER_DELAY (6)

#define MAX_NO_PS_ENV (4 + 1) /* +1 needed for VAR_BORDER */

#define NO_HI_RES_BINS (34)
#define NO_MID_RES_BINS (20)
#define NO_LOW_RES_BINS (10)

#define NO_HI_RES_IID_BINS (NO_HI_RES_BINS)
#define NO_HI_RES_ICC_BINS (NO_HI_RES_BINS)

#define NO_MID_RES_IID_BINS (NO_MID_RES_BINS)
#define NO_MID_RES_ICC_BINS (NO_MID_RES_BINS)

#define NO_LOW_RES_IID_BINS (NO_LOW_RES_BINS)
#define NO_LOW_RES_ICC_BINS (NO_LOW_RES_BINS)

#define SUBQMF_GROUPS (10)
#define QMF_GROUPS (12)

//#define SUBQMF_GROUPS_HI_RES            ( 32 )
//#define QMF_GROUPS_HI_RES               ( 18 )

#define NO_IID_GROUPS (SUBQMF_GROUPS + QMF_GROUPS)
//#define NO_IID_GROUPS_HI_RES            ( SUBQMF_GROUPS_HI_RES +
// QMF_GROUPS_HI_RES )

#define NO_IID_STEPS (7)       /* 1 .. + 7 */
#define NO_IID_STEPS_FINE (15) /* 1 .. +15 */
#define NO_ICC_STEPS (8)       /* 0 .. + 7 */

#define NO_IID_LEVELS (2 * NO_IID_STEPS + 1)           /* - 7 ..  + 7 */
#define NO_IID_LEVELS_FINE (2 * NO_IID_STEPS_FINE + 1) /* -15 ..  +15 */
#define NO_ICC_LEVELS (NO_ICC_STEPS)                   /*   0 ..  + 7 */

#define FIXP_SQRT05 ((int32_t)0x5a827980) /* 1/SQRT2 */

struct PS_DEC_COEFFICIENTS {
  int32_t H11r[NO_IID_GROUPS]; /*!< coefficients of the sub-subband groups */
  int32_t H12r[NO_IID_GROUPS]; /*!< coefficients of the sub-subband groups */
  int32_t H21r[NO_IID_GROUPS]; /*!< coefficients of the sub-subband groups */
  int32_t H22r[NO_IID_GROUPS]; /*!< coefficients of the sub-subband groups */

  int32_t
  DeltaH11r[NO_IID_GROUPS]; /*!< coefficients of the sub-subband groups */
  int32_t
  DeltaH12r[NO_IID_GROUPS]; /*!< coefficients of the sub-subband groups */
  int32_t
  DeltaH21r[NO_IID_GROUPS]; /*!< coefficients of the sub-subband groups */
  int32_t
  DeltaH22r[NO_IID_GROUPS]; /*!< coefficients of the sub-subband groups */

  int8_t
  aaIidIndexMapped[MAX_NO_PS_ENV]
                  [NO_HI_RES_IID_BINS]; /*!< The mapped IID index for all
                                           envelopes and all IID bins */
  int8_t
  aaIccIndexMapped[MAX_NO_PS_ENV]
                  [NO_HI_RES_ICC_BINS]; /*!< The mapped ICC index for all
                                           envelopes and all ICC bins */
};

typedef enum { ppt_none = 0, ppt_mpeg = 1, ppt_drm = 2 } PS_PAYLOAD_TYPE;

typedef struct {
  uint8_t bPsHeaderValid; /*!< set if new header is available from bitstream */

  uint8_t bEnableIid; /*!< One bit denoting the presence of IID parameters */
  uint8_t bEnableIcc; /*!< One bit denoting the presence of ICC parameters */
  uint8_t bEnableExt; /*!< The PS extension layer is enabled using the enable_ext
                       bit. If it is set to %1 the IPD and OPD parameters are
                       sent. If it is disabled, i.e. %0, the extension layer is
                       skipped.   */

  uint8_t
  modeIid;       /*!< The configuration of IID parameters (number of bands and
                      quantisation grid, iid_quant) is determined by iid_mode.   */
  uint8_t modeIcc; /*!< The configuration of Inter-channel Coherence parameters
                      (number of bands and quantisation grid) is determined by
                      icc_mode. */

  uint8_t freqResIid; /*!< 0=low, 1=mid or 2=high frequency resolution for iid */
  uint8_t freqResIcc; /*!< 0=low, 1=mid or 2=high frequency resolution for icc */

  uint8_t bFineIidQ; /*!< Use fine Iid quantisation. */

  uint8_t bFrameClass; /*!< The frame_class bit determines whether the parameter
                          positions of the current frame are uniformly spaced
                          accross the frame or they are defined using the
                        positions described by border_position.
                      */

  uint8_t noEnv; /*!< The number of envelopes per frame */
  uint8_t aEnvStartStop[MAX_NO_PS_ENV + 1]; /*!< In case of variable parameter
                                             spacing the parameter positions are
                                             determined by border_position */

  int8_t abIidDtFlag[MAX_NO_PS_ENV]; /*!< Deltacoding time/freq flag for IID, 0
                                       => freq           */
  int8_t abIccDtFlag[MAX_NO_PS_ENV]; /*!< Deltacoding time/freq flag for ICC, 0
                                       => freq           */

  int8_t
  aaIidIndex[MAX_NO_PS_ENV]
            [NO_HI_RES_IID_BINS]; /*!< The IID index for all envelopes and
                                     all IID bins        */
  int8_t
  aaIccIndex[MAX_NO_PS_ENV]
            [NO_HI_RES_ICC_BINS]; /*!< The ICC index for all envelopes and
                                     all ICC bins        */

} MPEG_PS_BS_DATA;

struct PS_DEC {
  int8_t noSubSamples;
  int8_t noChannels;

  int8_t procFrameBased; /*!< Helper to detected switching from frame based to
                           slot based processing
                         */

  PS_PAYLOAD_TYPE
  bPsDataAvail[(1) + 1]; /*!< set if new data available from bitstream */
  uint8_t psDecodedPrv;    /*!< set if PS has been processed in the last frame */

  /* helpers for frame delay line */
  uint8_t bsLastSlot;  /*!< Index of last read slot.  */
  uint8_t bsReadSlot;  /*!< Index of current read slot for additional delay.  */
  uint8_t processSlot; /*!< Index of current slot for processing (need for add.
                        delay).   */

  union { /* Bitstream data */
    MPEG_PS_BS_DATA
    mpeg; /*!< Struct containing all MPEG specific PS data from bitstream.
           */
  } bsData[(1) + 1];

  union { /* Static data */
    struct {
      int8_t aIidPrevFrameIndex[NO_HI_RES_IID_BINS]; /*!< The IID index for
                                                       previous frame */
      int8_t aIccPrevFrameIndex[NO_HI_RES_ICC_BINS]; /*!< The ICC index for
                                                       previous frame */
      uint8_t
      bPrevFrameFineIidQ;   /*!< The IID quantization of the previous frame */
      uint8_t prevFreqResIid; /*!< Frequency resolution for IID of the previous
                               frame            */
      uint8_t prevFreqResIcc; /*!< Frequency resolution for ICC of the previous
                               frame            */
      uint8_t lastUsb; /*!< uppermost WMF delay band of last frame          */

      int32_t pHybridAnaStatesLFdmx
          [2 * 13 * NO_QMF_BANDS_HYBRID20]; /*!< Memory used in hybrid analysis
                                                 for filter states. */
      FDK_ANA_HYB_FILTER hybridAnalysis;
      FDK_SYN_HYB_FILTER hybridSynthesis[2];

      DECORR_DEC apDecor; /*!< Decorrelator instance. */
      int32_t decorrBufferCplx[(2 * ((825) + (373)))];

      int32_t h11rPrev[NO_IID_GROUPS]; /*!< previous calculated h(xy)
                                           coefficients */
      int32_t h12rPrev[NO_IID_GROUPS]; /*!< previous calculated h(xy)
                                           coefficients */
      int32_t h21rPrev[NO_IID_GROUPS]; /*!< previous calculated h(xy)
                                           coefficients */
      int32_t h22rPrev[NO_IID_GROUPS]; /*!< previous calculated h(xy)
                                           coefficients */

      PS_DEC_COEFFICIENTS
      *pCoef; /*!< temporal coefficients are on reusable scratch memory */

    } mpeg;
  }
  specificTo;
};

typedef struct PS_DEC *HANDLE_PS_DEC;

int32_t CreatePsDec(HANDLE_PS_DEC *h_PS_DEC, int32_t aacSamplesPerFrame);

int32_t DeletePsDec(HANDLE_PS_DEC *h_PS_DEC);

void PreparePsProcessing(HANDLE_PS_DEC h_ps_d,
                         const int32_t *const *const rIntBufferLeft,
                         const int32_t *const *const iIntBufferLeft,
                         const int32_t scaleFactorLowBand);

void initSlotBasedRotation(HANDLE_PS_DEC h_ps_d, int32_t env, int32_t usb);

void ApplyPsSlot(
    HANDLE_PS_DEC h_ps_d,      /* parametric stereo decoder handle    */
    int32_t **rIntBufferLeft, /* real values of left qmf timeslot    */
    int32_t **iIntBufferLeft, /* imag values of left qmf timeslot    */
    int32_t *rIntBufferRight, /* real values of right qmf timeslot   */
    int32_t *iIntBufferRight, /* imag values of right qmf timeslot   */
    const int32_t scaleFactorLowBand_no_ov, const int32_t scaleFactorLowBand,
    const int32_t scaleFactorHighBand, const int32_t lsb, const int32_t usb);

#endif /* PSDEC_H */
