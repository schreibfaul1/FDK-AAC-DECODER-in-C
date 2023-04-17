
/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: int32_t/int16_t-block decoding

*******************************************************************************/

#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"

#include "channelinfo.h"
#include "../libFDK/FDK_bitstream.h"

/* PNS (of block) */
void CPns_Read(CPnsData_t *pPnsData, HANDLE_FDK_BITSTREAM bs,
               const CodeBookDescription *hcb, int16_t *pScaleFactor,
               uint8_t global_gain, int32_t band, int32_t group);

void CPns_Apply(const CPnsData_t *pPnsData, const CIcsInfo_t *pIcsInfo,
                int32_t* pSpectrum, const int16_t *pSpecScale,
                const int16_t *pScaleFactor,
                const SamplingRateInfo_t *pSamplingRateInfo,
                const int32_t granuleLength, const int32_t channel);

void CBlock_ApplyNoise(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                       SamplingRateInfo_t *pSamplingRateInfo, uint32_t *nfRandomSeed,
                       uint8_t *band_is_noise);

/* TNS (of block) */
/*!
  \brief Read tns data-present flag from bitstream

  The function reads the data-present flag for tns from
  the bitstream.

  \return  none
*/
void CTns_ReadDataPresentFlag(HANDLE_FDK_BITSTREAM bs, CTnsData_t *pTnsData);

void CTns_ReadDataPresentUsac(HANDLE_FDK_BITSTREAM hBs, CTnsData_t *pTnsData0,
                              CTnsData_t *pTnsData1, uint8_t *ptns_on_lr,
                              const CIcsInfo_t *pIcsInfo, const uint32_t flags,
                              const uint32_t elFlags, const int32_t fCommonWindow);

AAC_DECODER_ERROR_t CTns_Read(HANDLE_FDK_BITSTREAM bs, CTnsData_t *pTnsData,
                            const CIcsInfo_t *pIcsInfo, const uint32_t flags);

void CTns_Apply(CTnsData_t * pTnsData, /*!< pointer to aac decoder info */
                const CIcsInfo_t *pIcsInfo, int32_t* pSpectralCoefficient,
                const SamplingRateInfo_t *pSamplingRateInfo,
                const int32_t granuleLength, const uint8_t nbands,
                const uint8_t igf_active, const uint32_t flags);

/* Block */

int32_t CBlock_GetEscape(HANDLE_FDK_BITSTREAM bs, const int32_t q);

/**
 * \brief Read scale factor data. See chapter 4.6.2.3.2 of ISO/IEC 14496-3.
 *        The SF_OFFSET = 100 value referenced in chapter 4.6.2.3.3 is already
 * substracted from the scale factor values. Also includes PNS data reading.
 * \param bs bit stream handle data source
 * \param pAacDecoderChannelInfo channel context info were decoded data is
 * stored into.
 * \param flags the decoder flags.
 */
AAC_DECODER_ERROR_t CBlock_ReadScaleFactorData(
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, HANDLE_FDK_BITSTREAM bs,
    const uint32_t flags);

/**
 * \brief Read Huffman encoded spectral data.
 * \param pAacDecoderChannelInfo channel context info.
 * \param pSamplingRateInfo sampling rate info (sfb offsets).
 * \param flags syntax flags.
 */
AAC_DECODER_ERROR_t CBlock_ReadSpectralData(
    HANDLE_FDK_BITSTREAM bs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    const SamplingRateInfo_t *pSamplingRateInfo, const uint32_t flags);

/**
 * \brief Read Arithmetic encoded spectral data.
 * \param pAacDecoderChannelInfo channel context info.
 * \param pAacDecoderStaticChannelInfo static channel context info.
 * \param pSamplingRateInfo sampling rate info (sfb offsets).
 * \param frame_length spectral window length.
 * \param flags syntax flags.
 * \return error code.
 */
AAC_DECODER_ERROR_t CBlock_ReadAcSpectralData(
    HANDLE_FDK_BITSTREAM hBs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
    const SamplingRateInfo_t *pSamplingRateInfo, const uint32_t frame_length,
    const uint32_t flags);

AAC_DECODER_ERROR_t CBlock_ReadSectionData(
    HANDLE_FDK_BITSTREAM bs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    const SamplingRateInfo_t *pSamplingRateInfo, const uint32_t flags);

/**
 * \brief find a common exponent (shift factor) for all sfb in each Spectral
 * window, and store them into CAacDecoderChannelInfo_t::specScale.
 * \param pAacDecoderChannelInfo channel context info.
 * \param uint8_t maxSfbs maximum number of SFBs to be processed (might differ
 * from pAacDecoderChannelInfo->icsInfo.MaxSfBands)
 * \param pSamplingRateInfo sampling rate info (sfb offsets).
 */
void CBlock_ScaleSpectralData(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                              uint8_t maxSfbs,
                              SamplingRateInfo_t *pSamplingRateInfo);

/**
 * \brief Apply TNS and PNS tools.
 */
void ApplyTools(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo[],
                const SamplingRateInfo_t *pSamplingRateInfo, const uint32_t flags,
                const uint32_t elFlags, const int32_t channel, const int32_t maybe_jstereo);

/**
 * \brief Transform MDCT spectral data into time domain
 */
void CBlock_FrequencyToTime(
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, int32_t outSamples[],
    const int16_t frameLen, const int32_t frameOk, int32_t *pWorkBuffer1,
    const int32_t aacOutDataHeadroom, uint32_t elFlags, int32_t elCh);

/**
 * \brief Transform double lapped MDCT (AAC-ELD) spectral data into time domain.
 */
void CBlock_FrequencyToTimeLowDelay(
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, int32_t outSamples[],
    const int16_t frameLen);

AAC_DECODER_ERROR_t CBlock_InverseQuantizeSpectralData(
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    SamplingRateInfo_t *pSamplingRateInfo, uint8_t *band_is_noise,
    uint8_t active_band_search);

/**
 * \brief Calculate 2^(lsb/4) * value^(4/3)
 * \param pValue pointer to quantized value. The inverse quantized result is
 * stored back here.
 * \param lsb 2 LSBs of the scale factor (scaleFactor % 4) applied as power 2
 * factor to the resulting inverse quantized value.
 * \return the exponent of the result (mantissa) stored into *pValue.
 */
static inline
int32_t EvaluatePower43(int32_t *pValue, uint32_t lsb) {
  int32_t value;
  uint32_t freeBits;
  uint32_t exponent;

  value = *pValue;
  freeBits = fNormz(value);
  exponent = DFRACT_BITS - freeBits;
  assert(exponent < 14);

  uint32_t x = (((int32_t)value << freeBits) >> 19);
  uint32_t tableIndex = (x & 0x0FFF) >> 4;
  int32_t invQVal;

  x = x & 0x0F;

  uint32_t r0 = (int32_t)InverseQuantTable[tableIndex + 0];
  uint32_t r1 = (int32_t)InverseQuantTable[tableIndex + 1];
  uint16_t nx = 16 - x;
  uint32_t temp = (r0)*nx + (r1)*x;
  invQVal = (int32_t)temp;

  assert(lsb < 4);
  *pValue = fMultDiv2(invQVal, MantissaTable[lsb][exponent]);

  /* + 1 compensates fMultDiv2(). */
  return ExponentTable[lsb][exponent] + 1;
}

/* Recalculate gain */
int32_t get_gain(const int32_t *x, const int32_t *y, int32_t n);

/**
 * \brief determine the required shift scale for the given quantized value and
 * scale (factor % 4) value.
 */
static inline int32_t GetScaleFromValue(int32_t value, uint32_t lsb) {
  if (value != (int32_t)0) {
    int32_t scale = EvaluatePower43(&value, lsb);
    return CntLeadingZeros(value) - scale - 2;
  } else
    return 0; /* Return zero, because its useless to scale a zero value, saves
                 workload and avoids scaling overshifts. */
}

/*!
  \brief Read huffman codeword

  The function reads the huffman codeword from the bitstream and
  returns the index value.

  \return  index value
*/
inline int32_t CBlock_DecodeHuffmanWord(
    HANDLE_FDK_BITSTREAM bs,        /*!< pointer to bitstream */
    const CodeBookDescription *hcb) /*!< pointer to codebook description */
{
  uint32_t val;
  uint32_t index = 0;
  const uint16_t(*CodeBook)[HuffmanEntries] = hcb->CodeBook;

  while (1) {
    val = CodeBook[index]
                  [FDKreadBits(bs, HuffmanBits)]; /* Expensive memory access */

    if ((val & 1) == 0) {
      index = val >> 2;
      continue;
    } else {
      if (val & 2) {
        FDKpushBackCache(bs, 1);
      }

      val >>= 2;
      break;
    }
  }

  return val;
}
inline int32_t CBlock_DecodeHuffmanWordCB(
    HANDLE_FDK_BITSTREAM bs, /*!< pointer to bitstream */
    const uint16_t (
        *CodeBook)[HuffmanEntries]) /*!< pointer to codebook description */
{
  uint32_t index = 0;

  while (1) {
    index = CodeBook[index][FDKread2Bits(bs)]; /* Expensive memory access */
    if (index & 1) break;
    index >>= 2;
  }
  if (index & 2) {
    FDKpushBackCache(bs, 1);
  }
  return index >> 2;
}

#endif /* #ifndef BLOCK_H */
