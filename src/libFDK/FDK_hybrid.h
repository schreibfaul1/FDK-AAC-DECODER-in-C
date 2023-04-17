

/******************* Library for basic calculation routines ********************

   Author(s):   Markus Lohwasser

   Description: FDK Tools Hybrid Filterbank

*******************************************************************************/

#ifndef FDK_HYBRID_H
#define FDK_HYBRID_H
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"

/*--------------- enums -------------------------------*/

/**
 * Hybrid Filterband modes.
 */
typedef enum {
  THREE_TO_TEN,
  THREE_TO_TWELVE,
  THREE_TO_SIXTEEN

} FDK_HYBRID_MODE;

/*--------------- structure definitions ---------------*/
typedef const struct FDK_HYBRID_SETUP *HANDLE_FDK_HYBRID_SETUP;

typedef struct {
  int32_t *bufferLFReal[3];  /*!< LF real filter states. */
  int32_t *bufferLFImag[3];  /*!< LF imag filter states. */
  int32_t *bufferHFReal[13]; /*!< HF real delay lines. */
  int32_t *bufferHFImag[13]; /*!< HF imag delay lines. */

  int32_t bufferLFpos; /*!< Position to write incoming data into ringbuffer. */
  int32_t bufferHFpos; /*!< Delay line positioning. */
  int32_t nrBands;     /*!< Number of QMF bands. */
  int32_t cplxBands;   /*!< Number of complex QMF bands.*/
  uint8_t hfMode;    /*!< Flag signalizes treatment of HF bands. */

  int32_t *pLFmemory; /*!< Pointer to LF states buffer. */
  int32_t *pHFmemory; /*!< Pointer to HF states buffer. */

  uint32_t LFmemorySize; /*!< Size of LF states buffer. */
  uint32_t HFmemorySize; /*!< Size of HF states buffer. */

  HANDLE_FDK_HYBRID_SETUP pSetup; /*!< Pointer to filter setup. */

} FDK_ANA_HYB_FILTER;

typedef struct {
  int32_t nrBands;   /*!< Number of QMF bands. */
  int32_t cplxBands; /*!< Number of complex QMF bands.*/

  HANDLE_FDK_HYBRID_SETUP pSetup; /*!< Pointer to filter setup. */

} FDK_SYN_HYB_FILTER;

typedef FDK_ANA_HYB_FILTER *HANDLE_FDK_ANA_HYB_FILTER;
typedef FDK_SYN_HYB_FILTER *HANDLE_FDK_SYN_HYB_FILTER;

/**
 * \brief  Create one instance of Hybrid Analyis Filterbank.
 *
 * \param hAnalysisHybFilter  Pointer to an outlying allocated Hybrid Analysis
 * Filterbank structure.
 * \param pLFmemory           Pointer to outlying buffer used LF filtering.
 * \param LFmemorySize        Size of pLFmemory in bytes.
 * \param pHFmemory           Pointer to outlying buffer used HF delay line.
 * \param HFmemorySize        Size of pLFmemory in bytes.
 *
 * \return  0 on success.
 */
int32_t FDKhybridAnalysisOpen(HANDLE_FDK_ANA_HYB_FILTER hAnalysisHybFilter,
                          int32_t *const pLFmemory, const uint32_t LFmemorySize,
                          int32_t *const pHFmemory, const uint32_t HFmemorySize);

/**
 * \brief  Initialize and configure Hybrid Analysis Filterbank instance.
 *
 * \param hAnalysisHybFilter  A Hybrid Analysis Filterbank handle.
 * \param mode                Select hybrid filter configuration.
 * \param qmfBands            Number of qmf bands to be processed.
 * \param cplxBands           Number of complex qmf bands to be processed.
 * \param initStatesFlag      Indicates whether the states buffer has to be
 * cleared.
 *
 * \return  0 on success.
 */
int32_t FDKhybridAnalysisInit(HANDLE_FDK_ANA_HYB_FILTER hAnalysisHybFilter,
                          const FDK_HYBRID_MODE mode, const int32_t qmfBands,
                          const int32_t cplxBands, const int32_t initStatesFlag);

/**
 * \brief  Adjust Hybrid Analysis Filterbank states.
 *
 * \param hAnalysisHybFilter  A Hybrid Analysis Filterbank handle.
 * \param scalingValue        Scaling value to be applied on filter states.
 *
 * \return  0 on success.
 */
int32_t FDKhybridAnalysisScaleStates(HANDLE_FDK_ANA_HYB_FILTER hAnalysisHybFilter,
                                 const int32_t scalingValue);

/**
 * \brief  Apply Hybrid Analysis Filterbank on Qmf input data.
 *
 * \param hAnalysisHybFilter  A Hybrid Analysis Filterbank handle.
 * \param pQmfReal            Qmf input data.
 * \param pQmfImag            Qmf input data.
 * \param pHybridReal         Hybrid output data.
 * \param pHybridImag         Hybrid output data.
 *
 * \return  0 on success.
 */
int32_t FDKhybridAnalysisApply(HANDLE_FDK_ANA_HYB_FILTER hAnalysisHybFilter,
                           const int32_t *const pQmfReal,
                           const int32_t *const pQmfImag,
                           int32_t *const pHybridReal,
                           int32_t *const pHybridImag);

/**
 * \brief  Close a Hybrid Analysis Filterbank instance.
 *
 * \param hAnalysisHybFilter  Pointer to a Hybrid Analysis Filterbank instance.
 *
 * \return  0 on success.
 */
int32_t FDKhybridAnalysisClose(HANDLE_FDK_ANA_HYB_FILTER hAnalysisHybFilter);

/**
 * \brief  Initialize and configure Hybrdid Synthesis Filterbank instance.
 *
 * \param hSynthesisHybFilter A Hybrid Synthesis Filterbank handle.
 * \param mode                Select hybrid filter configuration.
 * \param qmfBands            Number of qmf bands to be processed.
 * \param cplxBands           Number of complex qmf bands to be processed.
 *
 * \return  0 on success.
 */
int32_t FDKhybridSynthesisInit(HANDLE_FDK_SYN_HYB_FILTER hSynthesisHybFilter,
                           const FDK_HYBRID_MODE mode, const int32_t qmfBands,
                           const int32_t cplxBands);

/**
 * \brief  Apply Hybrid Analysis Filterbank on Hybrid data.
 *
 * \param hSynthesisHybFilter  A Hybrid Analysis Filterbandk handle.
 * \param pHybridReal          Hybrid input data.
 * \param pHybridImag          Hybrid input data.
 * \param pQmfReal             Qmf output data.
 * \param pQmfImag             Qmf output data.
 *
 */
void FDKhybridSynthesisApply(HANDLE_FDK_SYN_HYB_FILTER hSynthesisHybFilter,
                             const int32_t *const pHybridReal,
                             const int32_t *const pHybridImag,
                             int32_t *const pQmfReal,
                             int32_t *const pQmfImag);

#endif /* FDK_HYBRID_H */
