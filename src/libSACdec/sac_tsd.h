

/*********************** MPEG surround decoder library *************************

   Author(s):   Matthias Hildenbrand

   Description: USAC MPS212 Transient Steering Decorrelator (TSD)

*******************************************************************************/

#ifndef SAC_TSD_H
#define SAC_TSD_H
#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/FDK_bitstream.h"
#include "../libFDK/common_fix.h"

#define MAX_TSD_TIME_SLOTS (64)

/** Structure which holds the data needed to apply TSD to current frame. */
typedef struct {
  uint8_t bsTsdEnable; /**< for current frame TSD is (0:disabled, 1:enabled) */
  uint8_t numSlots;    /**< total number of QMF slots per frame */
  int8_t
  bsTsdTrPhaseData[MAX_TSD_TIME_SLOTS]; /**< -1 => TsdSepData[ts]=0; 0-7:
                                           values of bsTsdTrPhaseData[ts]
                                           and TsdSepData[ts]=1 */
} TSD_DATA;

static inline int32_t isTsdActive(const TSD_DATA *pTsdData) {
  return (int32_t)pTsdData->bsTsdEnable;
}

/**
 * \brief Parse and Decode TSD data.
 * \param[in] hBs bitstream handle to read data from.
 * \param[in] numSlots number of QMF slots per frame.
 * \param[out] pTsdData pointer to TSD data structure.
 * \return 0 on succes, 1 on error.
 */
int32_t TsdRead(HANDLE_FDK_BITSTREAM hBs, const int32_t numSlots, TSD_DATA *pTsdData);

/**
 * \brief Perform transient seperation (v_{x,nonTr} signal).
 * \param[in] numHybridBands number of hybrid bands.
 * \param[in] pTsdData pointer to TSD data structure.
 * \param[in] pVdirectReal pointer to array with direct signal.
 * \param[in] pVdirectImag pointer to array with direct signal.
 * \param[out] pVnonTrReal pointer to array with nonTr signal.
 * \param[out] pVnonTrImag pointer to array with nonTr signal.
 * \param[out] ppDecorrInReal handle to array where allpass based decorrelator
 * should read from (modified by this function).
 * \param[out] ppDecorrInImag handle to array where allpass based decorrelator
 * should read from (modified by this function).
 */
void TsdGenerateNonTr(const int32_t numHybridBands, const TSD_DATA *pTsdData,
                      const int32_t ts, int32_t *pVdirectReal,
                      int32_t *pVdirectImag, int32_t *pVnonTrReal,
                      int32_t *pVnonTrImag, int32_t **ppDecorrInReal,
                      int32_t **ppDecorrInImag);

/**
 * \brief Generate d_{x,Tr} signal and add to d_{x,nonTr} signal
 * \param[in] numHybridBands
 * \param[in,out] pTsdData
 * \param pTsdTs pointer to persistent time slot counter
 * \param[in] pVdirectReal
 * \param[in] pVdirectImag
 * \param[out] pDnonTrReal
 * \param[out] pDnonTrImag
 */
void TsdApply(const int32_t numHybridBands, const TSD_DATA *pTsdData, int32_t *pTsdTs,
              const int32_t *pVdirectReal, const int32_t *pVdirectImag,
              int32_t *pDnonTrReal, int32_t *pDnonTrImag);

#endif /* #ifndef SAC_TSD_H */
