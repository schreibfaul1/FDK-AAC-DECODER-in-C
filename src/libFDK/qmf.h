

/******************* Library for basic calculation routines ********************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file   qmf.h
  \brief  Complex qmf analysis/synthesis
  \author Markus Werner

*/

#ifndef QMF_H
#define QMF_H
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"
#include "FDK_tools_rom.h"
#include "dct.h"

#define FIXP_QAS int16_t
#define QAS_BITS SAMPLE_BITS
#define INT_PCM_QMFIN int16_t

#define FIXP_QSS int32_t
#define QSS_BITS DFRACT_BITS

/* Flags for QMF intialization */
/* Low Power mode flag */
#define QMF_FLAG_LP 1
/* Filter is not symmetric. This flag is set internally in the QMF
 * initialization as required. */
/* DO NOT PASS THIS FLAG TO qmfInitAnalysisFilterBank or
 * qmfInitSynthesisFilterBank */
#define QMF_FLAG_NONSYMMETRIC 2
/* Complex Low Delay Filter Bank (or std symmetric filter bank) */
#define QMF_FLAG_CLDFB 4
/* Flag indicating that the states should be kept. */
#define QMF_FLAG_KEEP_STATES 8
/* Complex Low Delay Filter Bank used in MPEG Surround Encoder */
#define QMF_FLAG_MPSLDFB 16
/* Complex Low Delay Filter Bank used in MPEG Surround Encoder allows a
 * optimized calculation of the modulation in qmfForwardModulationHQ() */
#define QMF_FLAG_MPSLDFB_OPTIMIZE_MODULATION 32
/* Flag to indicate HE-AAC down-sampled SBR mode (decoder) -> adapt analysis
 * post twiddling */
#define QMF_FLAG_DOWNSAMPLED 64

#define QMF_MAX_SYNTHESIS_BANDS (64)

/*!
 * \brief Algorithmic scaling in sbrForwardModulation()
 *
 * The scaling in sbrForwardModulation() is caused by:
 *
 *   \li 1 R_SHIFT in sbrForwardModulation()
 *   \li 5/6 R_SHIFT in dct3() if using 32/64 Bands
 *   \li 1 omitted gain of 2.0 in qmfForwardModulation()
 */
#define ALGORITHMIC_SCALING_IN_ANALYSIS_FILTERBANK 7

/*!
 * \brief Algorithmic scaling in cplxSynthesisQmfFiltering()
 *
 * The scaling in cplxSynthesisQmfFiltering() is caused by:
 *
 *   \li  5/6 R_SHIFT in dct2() if using 32/64 Bands
 *   \li  1 omitted gain of 2.0 in qmfInverseModulation()
 *   \li -6 division by 64 in synthesis filterbank
 *   \li x bits external influence
 */
#define ALGORITHMIC_SCALING_IN_SYNTHESIS_FILTERBANK 1

typedef struct {
  int32_t lb_scale;    /*!< Scale of low band area                   */
  int32_t ov_lb_scale; /*!< Scale of adjusted overlap low band area  */
  int32_t hb_scale;    /*!< Scale of high band area                  */
  int32_t ov_hb_scale; /*!< Scale of adjusted overlap high band area */
} QMF_SCALE_FACTOR;

struct QMF_FILTER_BANK {
  const FIXP_PFT *p_filter; /*!< Pointer to filter coefficients */

  void *FilterStates;    /*!< Pointer to buffer of filter states
                              int16_t in analyse and
                              int32_t in synthesis filter */
  int32_t FilterSize;        /*!< Size of prototype filter. */
  const FIXP_QTW *t_cos; /*!< Modulation tables. */
  const FIXP_QTW *t_sin;
  int32_t filterScale; /*!< filter scale */

  int32_t no_channels; /*!< Total number of channels (subbands) */
  int32_t no_col;      /*!< Number of time slots       */
  int32_t lsb;         /*!< Top of low subbands */
  int32_t usb;         /*!< Top of high subbands */

  int32_t synScalefactor; /*!< Scale factor of synthesis qmf (syn only) */
  int32_t outScalefactor; /*!< Scale factor of output data (syn only) */
  int32_t outGain_m; /*!< Mantissa of gain output data (syn only) (init with
                         0x80000000 to ignore) */
  int32_t outGain_e;      /*!< Exponent of gain output data (syn only) */

  uint32_t flags;     /*!< flags */
  uint8_t p_stride; /*!< Stride Factor of polyphase filters */
};

typedef struct QMF_FILTER_BANK *HANDLE_QMF_FILTER_BANK;

int32_t qmfInitAnalysisFilterBank(
    HANDLE_QMF_FILTER_BANK h_Qmf, /*!< QMF Handle */
    FIXP_QAS *pFilterStates,      /*!< Pointer to filter state buffer */
    int32_t noCols,                   /*!< Number of time slots  */
    int32_t lsb,                      /*!< Number of lower bands */
    int32_t usb,                      /*!< Number of upper bands */
    int32_t no_channels,              /*!< Number of critically sampled bands */
    int32_t flags);                   /*!< Flags */
#if SAMPLE_BITS == 16

int32_t qmfInitAnalysisFilterBank(
    HANDLE_QMF_FILTER_BANK h_Qmf, /*!< QMF Handle */
    int32_t *pFilterStates,      /*!< Pointer to filter state buffer */
    int32_t noCols,                   /*!< Number of time slots  */
    int32_t lsb,                      /*!< Number of lower bands */
    int32_t usb,                      /*!< Number of upper bands */
    int32_t no_channels,              /*!< Number of critically sampled bands */
    int32_t flags);                   /*!< Flags */
#endif

void qmfAnalysisFiltering(
    HANDLE_QMF_FILTER_BANK anaQmf, /*!< Handle of Qmf Analysis Bank   */
    int32_t **qmfReal,            /*!< Pointer to real subband slots */
    int32_t **qmfImag,            /*!< Pointer to imag subband slots */
    QMF_SCALE_FACTOR *scaleFactor, /*!< Scale factors of QMF data     */
    const int16_t *timeIn,         /*!< Time signal */
    const int32_t timeIn_e,            /*!< Exponent of audio data        */
    const int32_t stride,              /*!< Stride factor of audio data   */
    int32_t *pWorkBuffer          /*!< pointer to temporal working buffer */
);
#if SAMPLE_BITS == 16

void qmfAnalysisFiltering(
    HANDLE_QMF_FILTER_BANK anaQmf, /*!< Handle of Qmf Analysis Bank   */
    int32_t **qmfReal,            /*!< Pointer to real subband slots */
    int32_t **qmfImag,            /*!< Pointer to imag subband slots */
    QMF_SCALE_FACTOR *scaleFactor, /*!< Scale factors of QMF data     */
    const int32_t *timeIn,            /*!< Time signal */
    const int32_t timeIn_e,            /*!< Exponent of audio data        */
    const int32_t stride,              /*!< Stride factor of audio data   */
    int32_t *pWorkBuffer          /*!< pointer to temporary working buffer */
);
#endif

void qmfAnalysisFilteringSlot(
    HANDLE_QMF_FILTER_BANK anaQmf, /*!< Handle of Qmf Synthesis Bank  */
    int32_t *qmfReal,             /*!< Low and High band, real */
    int32_t *qmfImag,             /*!< Low and High band, imag */
    const int16_t *timeIn,         /*!< Pointer to input */
    const int32_t stride,              /*!< stride factor of input */
    int32_t *pWorkBuffer          /*!< pointer to temporal working buffer */
);
#if SAMPLE_BITS == 16

void qmfAnalysisFilteringSlot(
    HANDLE_QMF_FILTER_BANK anaQmf, /*!< Handle of Qmf Synthesis Bank  */
    int32_t *qmfReal,             /*!< Low and High band, real */
    int32_t *qmfImag,             /*!< Low and High band, imag */
    const int32_t *timeIn,            /*!< Pointer to input */
    const int32_t stride,              /*!< stride factor of input */
    int32_t *pWorkBuffer          /*!< pointer to temporary working buffer */
);
#endif

int32_t qmfInitSynthesisFilterBank(
    HANDLE_QMF_FILTER_BANK h_Qmf, /*!< QMF Handle */
    FIXP_QSS *pFilterStates,      /*!< Pointer to filter state buffer */
    int32_t noCols,                   /*!< Number of time slots  */
    int32_t lsb,                      /*!< Number of lower bands */
    int32_t usb,                      /*!< Number of upper bands */
    int32_t no_channels,              /*!< Number of critically sampled bands */
    int32_t flags);                   /*!< Flags */

void qmfSynthesisFiltering(
    HANDLE_QMF_FILTER_BANK synQmf,       /*!< Handle of Qmf Synthesis Bank  */
    int32_t **QmfBufferReal,            /*!< Pointer to real subband slots */
    int32_t **QmfBufferImag,            /*!< Pointer to imag subband slots */
    const QMF_SCALE_FACTOR *scaleFactor, /*!< Scale factors of QMF data     */
    const int32_t ov_len,                    /*!< Length of band overlap        */
    int16_t *timeOut,                    /*!< Time signal */
    const int32_t stride,                    /*!< Stride factor of audio data   */
    int32_t *pWorkBuffer /*!< pointer to temporary working buffer. It must be
                             aligned */
);
#if SAMPLE_BITS == 16

void qmfSynthesisFiltering(
    HANDLE_QMF_FILTER_BANK synQmf,       /*!< Handle of Qmf Synthesis Bank  */
    int32_t **QmfBufferReal,            /*!< Pointer to real subband slots */
    int32_t **QmfBufferImag,            /*!< Pointer to imag subband slots */
    const QMF_SCALE_FACTOR *scaleFactor, /*!< Scale factors of QMF data     */
    const int32_t ov_len,                    /*!< Length of band overlap        */
    int32_t *timeOut,                       /*!< Time signal */
    const int32_t timeOut_e,                 /*!< Target exponent for timeOut  */
    int32_t *pWorkBuffer /*!< pointer to temporary working buffer */
);
#endif

void qmfSynthesisFilteringSlot(HANDLE_QMF_FILTER_BANK synQmf,
                               const int32_t *realSlot,
                               const int32_t *imagSlot,
                               const int32_t scaleFactorLowBand,
                               const int32_t scaleFactorHighBand, int16_t *timeOut,
                               const int32_t timeOut_e, int32_t *pWorkBuffer);
#if SAMPLE_BITS == 16

void qmfSynthesisFilteringSlot(HANDLE_QMF_FILTER_BANK synQmf,
                               const int32_t *realSlot,
                               const int32_t *imagSlot,
                               const int32_t scaleFactorLowBand,
                               const int32_t scaleFactorHighBand, int32_t *timeOut,
                               const int32_t timeOut_e, int32_t *pWorkBuffer);
#endif

void qmfChangeOutScalefactor(
    HANDLE_QMF_FILTER_BANK synQmf, /*!< Handle of Qmf Synthesis Bank */
    int32_t outScalefactor             /*!< New scaling factor for output data */
);

int32_t qmfGetOutScalefactor(
    HANDLE_QMF_FILTER_BANK synQmf /*!< Handle of Qmf Synthesis Bank */
);

void qmfChangeOutGain(
    HANDLE_QMF_FILTER_BANK synQmf, /*!< Handle of Qmf Synthesis Bank */
    int32_t outputGain,           /*!< New gain for output data (mantissa) */
    int32_t outputGainScale            /*!< New gain for output data (exponent) */
);

#endif /*ifndef QMF_H       */
