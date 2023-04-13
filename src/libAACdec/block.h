/* -----------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2019 Fraunhofer-Gesellschaft zur Förderung der angewandten
Forschung e.V. All rights reserved.

 1.    INTRODUCTION
The Fraunhofer FDK AAC Codec Library for Android ("FDK AAC Codec") is software
that implements the MPEG Advanced Audio Coding ("AAC") encoding and decoding
scheme for digital audio. This FDK AAC Codec software is intended to be used on
a wide variety of Android devices.

AAC's HE-AAC and HE-AAC v2 versions are regarded as today's most efficient
general perceptual audio codecs. AAC-ELD is considered the best-performing
full-bandwidth communications codec by independent studies and is widely
deployed. AAC has been standardized by ISO and IEC as part of the MPEG
specifications.

Patent licenses for necessary patent claims for the FDK AAC Codec (including
those of Fraunhofer) may be obtained through Via Licensing
(www.vialicensing.com) or through the respective patent owners individually for
the purpose of encoding or decoding bit streams in products that are compliant
with the ISO/IEC MPEG audio standards. Please note that most manufacturers of
Android devices already license these patent claims through Via Licensing or
directly from the patent owners, and therefore FDK AAC Codec software may
already be covered under those patent licenses when it is used for those
licensed purposes only.

Commercially-licensed AAC software libraries, including floating-point versions
with enhanced sound quality, are also available from Fraunhofer. Users are
encouraged to check the Fraunhofer website for additional applications
information and documentation.

2.    COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification,
are permitted without payment of copyright license fees provided that you
satisfy the following conditions:

You must retain the complete text of this software license in redistributions of
the FDK AAC Codec or your modifications thereto in source code form.

You must retain the complete text of this software license in the documentation
and/or other materials provided with redistributions of the FDK AAC Codec or
your modifications thereto in binary form. You must make available free of
charge copies of the complete source code of the FDK AAC Codec and your
modifications thereto to recipients of copies in binary form.

The name of Fraunhofer may not be used to endorse or promote products derived
from this library without prior written permission.

You may not charge copyright license fees for anyone to use, copy or distribute
the FDK AAC Codec software or your modifications thereto.

Your modified versions of the FDK AAC Codec must carry prominent notices stating
that you changed the software and the date of any change. For modified versions
of the FDK AAC Codec, the term "Fraunhofer FDK AAC Codec Library for Android"
must be replaced by the term "Third-Party Modified Version of the Fraunhofer FDK
AAC Codec Library for Android."

3.    NO PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without
limitation the patents of Fraunhofer, ARE GRANTED BY THIS SOFTWARE LICENSE.
Fraunhofer provides no warranty of patent non-infringement with respect to this
software.

You may use this FDK AAC Codec software or modifications thereto only for
purposes that are authorized by appropriate patent licenses.

4.    DISCLAIMER

This FDK AAC Codec software is provided by Fraunhofer on behalf of the copyright
holders and contributors "AS IS" and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
including but not limited to the implied warranties of merchantability and
fitness for a particular purpose. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE for any direct, indirect, incidental, special, exemplary,
or consequential damages, including but not limited to procurement of substitute
goods or services; loss of use, data, or profits, or business interruption,
however caused and on any theory of liability, whether in contract, strict
liability, or tort (including negligence), arising in any way out of the use of
this software, even if advised of the possibility of such damage.

5.    CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Audio and Multimedia Departments - FDK AAC LL
Am Wolfsmantel 33
91058 Erlangen, Germany

www.iis.fraunhofer.de/amm
amm-info@iis.fraunhofer.de
----------------------------------------------------------------------------- */

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: int32_t/int16_t-block decoding

*******************************************************************************/

#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include "../libFDK/common_fix.h"

#include "channelinfo.h"
#include "../libFDK/FDK_bitstream.h"

/* PNS (of block) */
void CPns_Read(CPnsData *pPnsData, HANDLE_FDK_BITSTREAM bs,
               const CodeBookDescription *hcb, int16_t *pScaleFactor,
               uint8_t global_gain, int32_t band, int32_t group);

void CPns_Apply(const CPnsData *pPnsData, const CIcsInfo *pIcsInfo,
                SPECTRAL_PTR pSpectrum, const int16_t *pSpecScale,
                const int16_t *pScaleFactor,
                const SamplingRateInfo *pSamplingRateInfo,
                const int32_t granuleLength, const int32_t channel);

void CBlock_ApplyNoise(CAacDecoderChannelInfo *pAacDecoderChannelInfo,
                       SamplingRateInfo *pSamplingRateInfo, uint32_t *nfRandomSeed,
                       uint8_t *band_is_noise);

/* TNS (of block) */
/*!
  \brief Read tns data-present flag from bitstream

  The function reads the data-present flag for tns from
  the bitstream.

  \return  none
*/
void CTns_ReadDataPresentFlag(HANDLE_FDK_BITSTREAM bs, CTnsData *pTnsData);

void CTns_ReadDataPresentUsac(HANDLE_FDK_BITSTREAM hBs, CTnsData *pTnsData0,
                              CTnsData *pTnsData1, uint8_t *ptns_on_lr,
                              const CIcsInfo *pIcsInfo, const uint32_t flags,
                              const uint32_t elFlags, const int32_t fCommonWindow);

AAC_DECODER_ERROR CTns_Read(HANDLE_FDK_BITSTREAM bs, CTnsData *pTnsData,
                            const CIcsInfo *pIcsInfo, const uint32_t flags);

void CTns_Apply(CTnsData *RESTRICT pTnsData, /*!< pointer to aac decoder info */
                const CIcsInfo *pIcsInfo, SPECTRAL_PTR pSpectralCoefficient,
                const SamplingRateInfo *pSamplingRateInfo,
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
AAC_DECODER_ERROR CBlock_ReadScaleFactorData(
    CAacDecoderChannelInfo *pAacDecoderChannelInfo, HANDLE_FDK_BITSTREAM bs,
    const uint32_t flags);

/**
 * \brief Read Huffman encoded spectral data.
 * \param pAacDecoderChannelInfo channel context info.
 * \param pSamplingRateInfo sampling rate info (sfb offsets).
 * \param flags syntax flags.
 */
AAC_DECODER_ERROR CBlock_ReadSpectralData(
    HANDLE_FDK_BITSTREAM bs, CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    const SamplingRateInfo *pSamplingRateInfo, const uint32_t flags);

/**
 * \brief Read Arithmetic encoded spectral data.
 * \param pAacDecoderChannelInfo channel context info.
 * \param pAacDecoderStaticChannelInfo static channel context info.
 * \param pSamplingRateInfo sampling rate info (sfb offsets).
 * \param frame_length spectral window length.
 * \param flags syntax flags.
 * \return error code.
 */
AAC_DECODER_ERROR CBlock_ReadAcSpectralData(
    HANDLE_FDK_BITSTREAM hBs, CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo,
    const SamplingRateInfo *pSamplingRateInfo, const uint32_t frame_length,
    const uint32_t flags);

AAC_DECODER_ERROR CBlock_ReadSectionData(
    HANDLE_FDK_BITSTREAM bs, CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    const SamplingRateInfo *pSamplingRateInfo, const uint32_t flags);

/**
 * \brief find a common exponent (shift factor) for all sfb in each Spectral
 * window, and store them into CAacDecoderChannelInfo::specScale.
 * \param pAacDecoderChannelInfo channel context info.
 * \param uint8_t maxSfbs maximum number of SFBs to be processed (might differ
 * from pAacDecoderChannelInfo->icsInfo.MaxSfBands)
 * \param pSamplingRateInfo sampling rate info (sfb offsets).
 */
void CBlock_ScaleSpectralData(CAacDecoderChannelInfo *pAacDecoderChannelInfo,
                              uint8_t maxSfbs,
                              SamplingRateInfo *pSamplingRateInfo);

/**
 * \brief Apply TNS and PNS tools.
 */
void ApplyTools(CAacDecoderChannelInfo *pAacDecoderChannelInfo[],
                const SamplingRateInfo *pSamplingRateInfo, const uint32_t flags,
                const uint32_t elFlags, const int32_t channel, const int32_t maybe_jstereo);

/**
 * \brief Transform MDCT spectral data into time domain
 */
void CBlock_FrequencyToTime(
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo,
    CAacDecoderChannelInfo *pAacDecoderChannelInfo, int32_t outSamples[],
    const int16_t frameLen, const int32_t frameOk, int32_t *pWorkBuffer1,
    const int32_t aacOutDataHeadroom, uint32_t elFlags, int32_t elCh);

/**
 * \brief Transform double lapped MDCT (AAC-ELD) spectral data into time domain.
 */
void CBlock_FrequencyToTimeLowDelay(
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo,
    CAacDecoderChannelInfo *pAacDecoderChannelInfo, int32_t outSamples[],
    const int16_t frameLen);

AAC_DECODER_ERROR CBlock_InverseQuantizeSpectralData(
    CAacDecoderChannelInfo *pAacDecoderChannelInfo,
    SamplingRateInfo *pSamplingRateInfo, uint8_t *band_is_noise,
    uint8_t active_band_search);

/**
 * \brief Calculate 2^(lsb/4) * value^(4/3)
 * \param pValue pointer to quantized value. The inverse quantized result is
 * stored back here.
 * \param lsb 2 LSBs of the scale factor (scaleFactor % 4) applied as power 2
 * factor to the resulting inverse quantized value.
 * \return the exponent of the result (mantissa) stored into *pValue.
 */
FDK_INLINE
int32_t EvaluatePower43(int32_t *pValue, uint32_t lsb) {
  int32_t value;
  uint32_t freeBits;
  uint32_t exponent;

  value = *pValue;
  freeBits = fNormz(value);
  exponent = DFRACT_BITS - freeBits;
  FDK_ASSERT(exponent < 14);

  uint32_t x = (((int32_t)value << freeBits) >> 19);
  uint32_t tableIndex = (x & 0x0FFF) >> 4;
  int32_t invQVal;

  x = x & 0x0F;

  uint32_t r0 = (int32_t)InverseQuantTable[tableIndex + 0];
  uint32_t r1 = (int32_t)InverseQuantTable[tableIndex + 1];
  uint16_t nx = 16 - x;
  uint32_t temp = (r0)*nx + (r1)*x;
  invQVal = (int32_t)temp;

  FDK_ASSERT(lsb < 4);
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
FDK_INLINE int32_t GetScaleFromValue(int32_t value, uint32_t lsb) {
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
