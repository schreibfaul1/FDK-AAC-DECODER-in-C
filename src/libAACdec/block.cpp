

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: int32_t/int16_t-block decoding

*******************************************************************************/

#include <memory.h>

#include <assert.h>
#include "newAACDecoder.h"
#include "block.h"

#include "aac_rom.h"
#include "../libFDK/FDK_bitstream.h"
#include "../libFDK/scale.h"
#include "../libFDK/FDK_tools_rom.h"
#include "../libFDK/FDK_lpc.h"
#include "../libFDK/FDK_trigFcts.h"

#include "../libArithCoding/ac_arith_coder.h"

// #if defined(__arm__)
// #include "arm/block_arm.cpp"
// #endif

/*!
  \brief Read escape sequence of codeword

  The function reads the escape sequence from the bitstream,
  if the absolute value of the quantized coefficient has the
  value 16.
  A limitation is implemented to maximal 21 bits according to
  ISO/IEC 14496-3:2009(E) 4.6.3.3.
  This limits the escape prefix to a maximum of eight 1's.
  If more than eight 1's are read, MAX_QUANTIZED_VALUE + 1 is
  returned, independent of the sign of parameter q.

  \return  quantized coefficient
*/
int32_t CBlock_GetEscape(HANDLE_FDK_BITSTREAM bs, /*!< pointer to bitstream */
                      const int32_t q)            /*!< quantized coefficient */
{
  if (fAbs(q) != 16) return (q);

  int32_t i, off;
  for (i = 4; i < 13; i++) {
    if (FDKreadBit(bs) == 0) break;
  }

  if (i == 13) return (MAX_QUANTIZED_VALUE + 1);

  off = FDKreadBits(bs, i);
  i = off + (1 << i);

  if (q < 0) i = -i;

  return i;
}

AAC_DECODER_ERROR_t CBlock_ReadScaleFactorData(
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, HANDLE_FDK_BITSTREAM bs,
    uint32_t flags) {
  int32_t temp;
  int32_t band;
  int32_t group;
  int32_t position = 0; /* accu for intensity delta coding */
  int32_t factor = pAacDecoderChannelInfo->pDynData->RawDataInfo
                   .GlobalGain; /* accu for scale factor delta coding */
  uint8_t *pCodeBook = pAacDecoderChannelInfo->pDynData->aCodeBook;
  int16_t *pScaleFactor = pAacDecoderChannelInfo->pDynData->aScaleFactor;
  const CodeBookDescription_t *hcb = &AACcodeBookDescriptionTable[BOOKSCL];

  const uint16_t(*CodeBook)[HuffmanEntries] = hcb->CodeBook;

  int32_t ScaleFactorBandsTransmitted =
      GetScaleFactorBandsTransmitted(&pAacDecoderChannelInfo->icsInfo);
  for (group = 0; group < GetWindowGroups(&pAacDecoderChannelInfo->icsInfo);
       group++) {
    for (band = 0; band < ScaleFactorBandsTransmitted; band++) {
      switch (pCodeBook[band]) {
        case ZERO_HCB: /* zero book */
          pScaleFactor[band] = 0;
          break;

        default: /* decode scale factor */
          if (!((flags & (AC_USAC | AC_RSVD50 | AC_RSV603DA)) && band == 0 &&
                group == 0)) {
            temp = CBlock_DecodeHuffmanWordCB(bs, CodeBook);
            factor += temp - 60; /* MIDFAC 1.5 dB */
          }
          pScaleFactor[band] = factor - 100;
          break;

        case INTENSITY_HCB: /* intensity steering */
        case INTENSITY_HCB2:
          temp = CBlock_DecodeHuffmanWordCB(bs, CodeBook);
          position += temp - 60;
          pScaleFactor[band] = position - 100;
          break;

        case NOISE_HCB: /* PNS */
          if (flags & (AC_MPEGD_RES | AC_USAC | AC_RSVD50 | AC_RSV603DA)) {
            return AAC_DEC_PARSE_ERROR;
          }
          CPns_Read(&pAacDecoderChannelInfo->data.aac.PnsData, bs, hcb,
                    pAacDecoderChannelInfo->pDynData->aScaleFactor,
                    pAacDecoderChannelInfo->pDynData->RawDataInfo.GlobalGain,
                    band, group);
          break;
      }
    }
    pCodeBook += 16;
    pScaleFactor += 16;
  }

  return AAC_DEC_OK;
}

void CBlock_ScaleSpectralData(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                              uint8_t maxSfbs,
                              SamplingRateInfo_t *pSamplingRateInfo) {
  int32_t band;
  int32_t window;
  const int16_t * pSfbScale = pAacDecoderChannelInfo->pDynData->aSfbScale;
  int16_t * pSpecScale = pAacDecoderChannelInfo->specScale;
  int32_t groupwin, group;
  const int16_t * BandOffsets = GetScaleFactorBandOffsets(
      &pAacDecoderChannelInfo->icsInfo, pSamplingRateInfo);
  int32_t*  pSpectralCoefficient =
      pAacDecoderChannelInfo->pSpectralCoefficient;

  memset(pSpecScale, 0, 8 * sizeof(int16_t));

  for (window = 0, group = 0;
       group < GetWindowGroups(&pAacDecoderChannelInfo->icsInfo); group++) {
    for (groupwin = 0; groupwin < GetWindowGroupLength(
                                      &pAacDecoderChannelInfo->icsInfo, group);
         groupwin++, window++) {
      int32_t SpecScale_window = pSpecScale[window];
      int32_t *pSpectrum = SPEC(pSpectralCoefficient, window,
                                 pAacDecoderChannelInfo->granuleLength);

      /* find scaling for current window */
      for (band = 0; band < maxSfbs; band++) {
        SpecScale_window =
            fMax(SpecScale_window, (int32_t)pSfbScale[window * 16 + band]);
      }

      if (pAacDecoderChannelInfo->pDynData->TnsData.Active &&
          pAacDecoderChannelInfo->pDynData->TnsData.NumberOfFilters[window] >
              0) {
        int32_t filter_index, SpecScale_window_tns;
        int32_t tns_start, tns_stop;

        /* Find max scale of TNS bands */
        SpecScale_window_tns = 0;
        tns_start = GetMaximumTnsBands(&pAacDecoderChannelInfo->icsInfo,
                                       pSamplingRateInfo->samplingRateIndex);
        tns_stop = 0;
        for (filter_index = 0;
             filter_index < (int32_t)pAacDecoderChannelInfo->pDynData->TnsData
                                .NumberOfFilters[window];
             filter_index++) {
          for (band = pAacDecoderChannelInfo->pDynData->TnsData
                          .Filter[window][filter_index]
                          .StartBand;
               band < pAacDecoderChannelInfo->pDynData->TnsData
                          .Filter[window][filter_index]
                          .StopBand;
               band++) {
            SpecScale_window_tns =
                fMax(SpecScale_window_tns, (int32_t)pSfbScale[window * 16 + band]);
          }
          /* Find TNS line boundaries for all TNS filters */
          tns_start =
              fMin(tns_start, (int32_t)pAacDecoderChannelInfo->pDynData->TnsData
                                  .Filter[window][filter_index]
                                  .StartBand);
          tns_stop =
              fMax(tns_stop, (int32_t)pAacDecoderChannelInfo->pDynData->TnsData
                                 .Filter[window][filter_index]
                                 .StopBand);
        }
        SpecScale_window_tns = SpecScale_window_tns +
                               pAacDecoderChannelInfo->pDynData->TnsData.GainLd;
        assert(tns_stop >= tns_start);
        /* Consider existing headroom of all MDCT lines inside the TNS bands. */
        SpecScale_window_tns -=
            getScalefactor(pSpectrum + BandOffsets[tns_start],
                           BandOffsets[tns_stop] - BandOffsets[tns_start]);
        if (SpecScale_window <= 17) {
          SpecScale_window_tns++;
        }
        /* Add enough mantissa head room such that the spectrum is still
           representable after applying TNS. */
        SpecScale_window = fMax(SpecScale_window, SpecScale_window_tns);
      }

      /* store scaling of current window */
      pSpecScale[window] = SpecScale_window;

#ifdef FUNCTION_CBlock_ScaleSpectralData_func1

      CBlock_ScaleSpectralData_func1(pSpectrum, maxSfbs, BandOffsets,
                                     SpecScale_window, pSfbScale, window);

#else  /* FUNCTION_CBlock_ScaleSpectralData_func1 */
      for (band = 0; band < maxSfbs; band++) {
        int32_t scale = fMin(DFRACT_BITS - 1,
                         SpecScale_window - pSfbScale[window * 16 + band]);
        if (scale) {
          assert(scale > 0);

          /* following relation can be used for optimizations:
           * (BandOffsets[i]%4) == 0 for all i */
          int32_t max_index = BandOffsets[band + 1];
          for (int32_t index = BandOffsets[band]; index < max_index; index++) {
            pSpectrum[index] >>= scale;
          }
        }
      }
#endif /* FUNCTION_CBlock_ScaleSpectralData_func1 */
    }
  }
}

AAC_DECODER_ERROR_t CBlock_ReadSectionData(
    HANDLE_FDK_BITSTREAM bs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    const SamplingRateInfo_t *pSamplingRateInfo, const uint32_t flags) {
  int32_t top, band;
  int32_t sect_len, sect_len_incr;
  int32_t group;
  uint8_t sect_cb;
  uint8_t *pCodeBook = pAacDecoderChannelInfo->pDynData->aCodeBook;
  /* HCR input (int32_t) */
  int16_t *pNumLinesInSec =
      pAacDecoderChannelInfo->pDynData->specificTo.aac.aNumLineInSec4Hcr;
  int32_t numLinesInSecIdx = 0;
  uint8_t *pHcrCodeBook =
      pAacDecoderChannelInfo->pDynData->specificTo.aac.aCodeBooks4Hcr;
  const int16_t *BandOffsets = GetScaleFactorBandOffsets(
      &pAacDecoderChannelInfo->icsInfo, pSamplingRateInfo);
  pAacDecoderChannelInfo->pDynData->specificTo.aac.numberSection = 0;
  AAC_DECODER_ERROR_t ErrorStatus = AAC_DEC_OK;

  memset(pCodeBook, 0, sizeof(uint8_t) * (8 * 16));

  const int32_t nbits =
      (IsLongBlock(&pAacDecoderChannelInfo->icsInfo) == 1) ? 5 : 3;

  int32_t sect_esc_val = (1 << nbits) - 1;

  uint8_t ScaleFactorBandsTransmitted =
      GetScaleFactorBandsTransmitted(&pAacDecoderChannelInfo->icsInfo);
  for (group = 0; group < GetWindowGroups(&pAacDecoderChannelInfo->icsInfo);
       group++) {
    for (band = 0; band < ScaleFactorBandsTransmitted;) {
      sect_len = 0;
      if (flags & AC_ER_VCB11) {
        sect_cb = (uint8_t)FDKreadBits(bs, 5);
      } else
        sect_cb = (uint8_t)FDKreadBits(bs, 4);

      if (((flags & AC_ER_VCB11) == 0) || (sect_cb < 11) ||
          ((sect_cb > 11) && (sect_cb < 16))) {
        sect_len_incr = FDKreadBits(bs, nbits);
        while (sect_len_incr == sect_esc_val) {
          sect_len += sect_esc_val;
          sect_len_incr = FDKreadBits(bs, nbits);
        }
      } else {
        sect_len_incr = 1;
      }

      sect_len += sect_len_incr;

      top = band + sect_len;

      if (flags & AC_ER_HCR) {
        /* HCR input (int32_t) -- collecting sideinfo (for HCR-_long_ only) */
        if (numLinesInSecIdx >= MAX_SFB_HCR) {
          return AAC_DEC_PARSE_ERROR;
        }
        if (top > (int32_t)GetNumberOfScaleFactorBands(
                      &pAacDecoderChannelInfo->icsInfo, pSamplingRateInfo)) {
          return AAC_DEC_PARSE_ERROR;
        }
        pNumLinesInSec[numLinesInSecIdx] = BandOffsets[top] - BandOffsets[band];
        numLinesInSecIdx++;
        if (sect_cb == BOOKSCL) {
          return AAC_DEC_INVALID_CODE_BOOK;
        } else {
          *pHcrCodeBook++ = sect_cb;
        }
        pAacDecoderChannelInfo->pDynData->specificTo.aac.numberSection++;
      }

      /* Check spectral line limits */
      if (IsLongBlock(&(pAacDecoderChannelInfo->icsInfo))) {
        if (top > 64) {
          return AAC_DEC_DECODE_FRAME_ERROR;
        }
      } else { /* int16_t block */
        if (top + group * 16 > (8 * 16)) {
          return AAC_DEC_DECODE_FRAME_ERROR;
        }
      }

      /* Check if decoded codebook index is feasible */
      if ((sect_cb == BOOKSCL) ||
          ((sect_cb == INTENSITY_HCB || sect_cb == INTENSITY_HCB2) &&
           pAacDecoderChannelInfo->pDynData->RawDataInfo.CommonWindow == 0)) {
        return AAC_DEC_INVALID_CODE_BOOK;
      }

      /* Store codebook index */
      for (; band < top; band++) {
        pCodeBook[group * 16 + band] = sect_cb;
      }
    }
  }

  return ErrorStatus;
}

/* mso: provides a faster way to i-quantize a whole band in one go */

/**
 * \brief inverse quantize one sfb. Each value of the sfb is processed according
 * to the formula: spectrum[i] = Sign(spectrum[i]) * Matissa(spectrum[i])^(4/3)
 * * 2^(lsb/4).
 * \param spectrum pointer to first line of the sfb to be inverse quantized.
 * \param noLines number of lines belonging to the sfb.
 * \param lsb last 2 bits of the scale factor of the sfb.
 * \param scale max allowed shift scale for the sfb.
 */
static inline void InverseQuantizeBand(
    int32_t * spectrum, const int32_t * InverseQuantTabler,
    const int32_t * MantissaTabler,
    const int8_t * ExponentTabler, int32_t noLines, int32_t scale) {
  scale = scale + 1; /* +1 to compensate fMultDiv2 shift-right in loop */

  int32_t * ptr = spectrum;
  int32_t signedValue;

  for (int32_t i = noLines; i--;) {
    if ((signedValue = *ptr++) != FL2FXCONST_DBL(0)) {
      int32_t value = fAbs(signedValue);
      uint32_t freeBits = CntLeadingZeros(value);
      uint32_t exponent = 32 - freeBits;

      uint32_t x = (uint32_t)(int32_t)value << (int32_t)freeBits;
      x <<= 1; /* shift out sign bit to avoid masking later on */
      uint32_t tableIndex = x >> 24;
      x = (x >> 20) & 0x0F;

      uint32_t r0 = (uint32_t)(int32_t)InverseQuantTabler[tableIndex + 0];
      uint32_t r1 = (uint32_t)(int32_t)InverseQuantTabler[tableIndex + 1];
      uint32_t temp = (r1 - r0) * x + (r0 << 4);

      value = fMultDiv2((int32_t)temp, MantissaTabler[exponent]);

      /* + 1 compensates fMultDiv2() */
      scaleValueInPlace(&value, scale + ExponentTabler[exponent]);

      signedValue = (signedValue < (int32_t)0) ? -value : value;
      ptr[-1] = signedValue;
    }
  }
}

static inline int32_t maxabs_D(const int32_t *pSpectralCoefficient,
                                const int32_t noLines) {
  /* Find max spectral line value of the current sfb */
  int32_t locMax = (int32_t)0;
  int32_t i;

  for (i = noLines; i-- > 0;) {
    /* Expensive memory access */
    locMax = fMax(fixp_abs(pSpectralCoefficient[i]), locMax);
  }

  return locMax;
}

AAC_DECODER_ERROR_t CBlock_InverseQuantizeSpectralData(
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    SamplingRateInfo_t *pSamplingRateInfo, uint8_t *band_is_noise,
    uint8_t active_band_search) {
  int32_t window, group, groupwin, band;
  int32_t ScaleFactorBandsTransmitted =
      GetScaleFactorBandsTransmitted(&pAacDecoderChannelInfo->icsInfo);
  uint8_t * pCodeBook = pAacDecoderChannelInfo->pDynData->aCodeBook;
  int16_t * pSfbScale = pAacDecoderChannelInfo->pDynData->aSfbScale;
  int16_t * pScaleFactor = pAacDecoderChannelInfo->pDynData->aScaleFactor;
  const int16_t * BandOffsets = GetScaleFactorBandOffsets(
      &pAacDecoderChannelInfo->icsInfo, pSamplingRateInfo);
  const int16_t total_bands =
      GetScaleFactorBandsTotal(&pAacDecoderChannelInfo->icsInfo);

  memset(pAacDecoderChannelInfo->pDynData->aSfbScale, 0,
              (8 * 16) * sizeof(int16_t));

  for (window = 0, group = 0;
       group < GetWindowGroups(&pAacDecoderChannelInfo->icsInfo); group++) {
    for (groupwin = 0; groupwin < GetWindowGroupLength(
                                      &pAacDecoderChannelInfo->icsInfo, group);
         groupwin++, window++) {
      /* inverse quantization */
      for (band = 0; band < ScaleFactorBandsTransmitted; band++) {
        int32_t *pSpectralCoefficient =
            SPEC(pAacDecoderChannelInfo->pSpectralCoefficient, window,
                 pAacDecoderChannelInfo->granuleLength) +
            BandOffsets[band];
        int32_t locMax;

        const int32_t noLines = BandOffsets[band + 1] - BandOffsets[band];
        const int32_t bnds = group * 16 + band;

        if ((pCodeBook[bnds] == ZERO_HCB) ||
            (pCodeBook[bnds] == INTENSITY_HCB) ||
            (pCodeBook[bnds] == INTENSITY_HCB2))
          continue;

        if (pCodeBook[bnds] == NOISE_HCB) {
          /* Leave headroom for PNS values. + 1 because ceil(log2(2^(0.25*3))) =
             1, worst case of additional headroom required because of the
             scalefactor. */
          pSfbScale[window * 16 + band] = (pScaleFactor[bnds] >> 2) + 1;
          continue;
        }

        locMax = maxabs_D(pSpectralCoefficient, noLines);

        if (active_band_search) {
          if (locMax != int32_t(0)) {
            band_is_noise[group * 16 + band] = 0;
          }
        }

        /* Cheap robustness improvement - Do not remove!!! */
        if (fixp_abs(locMax) > (int32_t)MAX_QUANTIZED_VALUE) {
          return AAC_DEC_PARSE_ERROR;
        }

        /* Added by Youliy Ninov:
        The inverse quantization operation is given by (ISO/IEC 14496-3:2009(E))
        by:

        x_invquant=Sign(x_quant). abs(x_quant)^(4/3)

        We apply a gain, derived from the scale factor for the particular sfb,
        according to the following function:

        gain=2^(0.25*ScaleFactor)

        So, after scaling we have:

        x_rescale=gain*x_invquant=Sign(x_quant)*2^(0.25*ScaleFactor)*abs(s_quant)^(4/3)

        We could represent the ScaleFactor as:

        ScaleFactor= (ScaleFactor >> 2)*4 + ScaleFactor %4

        When we substitute it we get:

        x_rescale=Sign(x_quant)*2^(ScaleFactor>>2)* (
        2^(0.25*(ScaleFactor%4))*abs(s_quant)^(4/3))

        When we set: msb=(ScaleFactor>>2) and lsb=(ScaleFactor%4), we obtain:

        x_rescale=Sign(x_quant)*(2^msb)* ( 2^(lsb/4)*abs(s_quant)^(4/3))

        The rescaled output can be represented by:
           mantissa : Sign(x_quant)*( 2^(lsb/4)*abs(s_quant)^(4/3))
           exponent :(2^msb)

        */

        int32_t msb = pScaleFactor[bnds] >> 2;

        /* Inverse quantize band only if it is not empty */
        if (locMax != int32_t(0)) {
          int32_t lsb = pScaleFactor[bnds] & 0x03;

          int32_t scale = EvaluatePower43(&locMax, lsb);

          scale = CntLeadingZeros(locMax) - scale - 2;

          pSfbScale[window * 16 + band] = msb - scale;
          InverseQuantizeBand(pSpectralCoefficient, InverseQuantTable,
                              MantissaTable[lsb], ExponentTable[lsb], noLines,
                              scale);
        } else {
          pSfbScale[window * 16 + band] = msb;
        }

      } /* for (band=0; band < ScaleFactorBandsTransmitted; band++) */

      /* Make sure the array is cleared to the end */
      int16_t start_clear = BandOffsets[ScaleFactorBandsTransmitted];
      int16_t end_clear = BandOffsets[total_bands];
      int32_t diff_clear = (int32_t)(end_clear - start_clear);
      int32_t *pSpectralCoefficient =
          SPEC(pAacDecoderChannelInfo->pSpectralCoefficient, window,
               pAacDecoderChannelInfo->granuleLength) +
          start_clear;
      memset(pSpectralCoefficient, 0, diff_clear * sizeof(int32_t));

    } /* for (groupwin=0; groupwin <
         GetWindowGroupLength(&pAacDecoderChannelInfo->icsInfo,group);
         groupwin++, window++) */
  }   /* for (window=0, group=0; group <
         GetWindowGroups(&pAacDecoderChannelInfo->icsInfo); group++)*/

  return AAC_DEC_OK;
}

AAC_DECODER_ERROR_t CBlock_ReadSpectralData(
    HANDLE_FDK_BITSTREAM bs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    const SamplingRateInfo_t *pSamplingRateInfo, const uint32_t flags) {
  int32_t index, i;
  const int16_t * BandOffsets = GetScaleFactorBandOffsets(
      &pAacDecoderChannelInfo->icsInfo, pSamplingRateInfo);

  int32_t* pSpectralCoefficient =
      pAacDecoderChannelInfo->pSpectralCoefficient;

  assert(BandOffsets != NULL);

  typedef int32_t SPECTRUM[1024];
  memset(pSpectralCoefficient, 0, sizeof(SPECTRUM));

  if ((flags & AC_ER_HCR) == 0) {
    int32_t group;
    int32_t groupoffset;
    uint8_t *pCodeBook = pAacDecoderChannelInfo->pDynData->aCodeBook;
    int32_t ScaleFactorBandsTransmitted =
        GetScaleFactorBandsTransmitted(&pAacDecoderChannelInfo->icsInfo);
    int32_t granuleLength = pAacDecoderChannelInfo->granuleLength;

    groupoffset = 0;

    /* plain huffman decoder  int16_t */
    int32_t max_group = GetWindowGroups(&pAacDecoderChannelInfo->icsInfo);

    for (group = 0; group < max_group; group++) {
      int32_t max_groupwin =
          GetWindowGroupLength(&pAacDecoderChannelInfo->icsInfo, group);
      int32_t band;

      int32_t bnds = group * 16;

      int32_t bandOffset1 = BandOffsets[0];
      for (band = 0; band < ScaleFactorBandsTransmitted; band++, bnds++) {
        uint8_t currentCB = pCodeBook[bnds];
        int32_t bandOffset0 = bandOffset1;
        bandOffset1 = BandOffsets[band + 1];

        /* patch to run plain-huffman-decoder with vcb11 input codebooks
         * (LAV-checking might be possible below using the virtual cb and a
         * LAV-table) */
        if ((currentCB >= 16) && (currentCB <= 31)) {
          pCodeBook[bnds] = currentCB = 11;
        }
        if (((currentCB != ZERO_HCB) && (currentCB != NOISE_HCB) &&
             (currentCB != INTENSITY_HCB) && (currentCB != INTENSITY_HCB2))) {
          const CodeBookDescription_t *hcb =
              &AACcodeBookDescriptionTable[currentCB];
          int32_t step = hcb->Dimension;
          int32_t offset = hcb->Offset;
          int32_t bits = hcb->numBits;
          int32_t mask = (1 << bits) - 1;
          const uint16_t(*CodeBook)[HuffmanEntries] = hcb->CodeBook;
          int32_t groupwin;

          int32_t *mdctSpectrum =
              &pSpectralCoefficient[groupoffset * granuleLength];

          if (offset == 0) {
            for (groupwin = 0; groupwin < max_groupwin; groupwin++) {
              for (index = bandOffset0; index < bandOffset1; index += step) {
                int32_t idx = CBlock_DecodeHuffmanWordCB(bs, CodeBook);
                for (i = 0; i < step; i++, idx >>= bits) {
                  int32_t tmp = (int32_t)((idx & mask) - offset);
                  if (tmp != int32_t(0)) tmp = (FDKreadBit(bs)) ? -tmp : tmp;
                  mdctSpectrum[index + i] = tmp;
                }

                if (currentCB == ESCBOOK) {
                  for (int32_t j = 0; j < 2; j++)
                    mdctSpectrum[index + j] = (int32_t)CBlock_GetEscape(
                        bs, (int32_t)mdctSpectrum[index + j]);
                }
              }
              mdctSpectrum += granuleLength;
            }
          } else {
            for (groupwin = 0; groupwin < max_groupwin; groupwin++) {
              for (index = bandOffset0; index < bandOffset1; index += step) {
                int32_t idx = CBlock_DecodeHuffmanWordCB(bs, CodeBook);
                for (i = 0; i < step; i++, idx >>= bits) {
                  mdctSpectrum[index + i] = (int32_t)((idx & mask) - offset);
                }
                if (currentCB == ESCBOOK) {
                  for (int32_t j = 0; j < 2; j++)
                    mdctSpectrum[index + j] = (int32_t)CBlock_GetEscape(
                        bs, (int32_t)mdctSpectrum[index + j]);
                }
              }
              mdctSpectrum += granuleLength;
            }
          }
        }
      }
      groupoffset += max_groupwin;
    }
    /* plain huffman decoding (int16_t) finished */
  }

  /* HCR - Huffman Codeword Reordering  int16_t */
  else /* if ( flags & AC_ER_HCR ) */

  {
    H_HCR_INFO hHcr = &pAacDecoderChannelInfo->pComData->overlay.aac.erHcrInfo;

    int32_t hcrStatus = 0;

    /* advanced Huffman decoding starts here (HCR decoding :) */
    if (pAacDecoderChannelInfo->pDynData->specificTo.aac
            .lenOfReorderedSpectralData != 0) {
      /* HCR initialization int16_t */
      hcrStatus = HcrInit(hHcr, pAacDecoderChannelInfo, pSamplingRateInfo, bs);

      if (hcrStatus != 0) {
        return AAC_DEC_DECODE_FRAME_ERROR;
      }

      /* HCR decoding int16_t */
      hcrStatus =
          HcrDecoder(hHcr, pAacDecoderChannelInfo, pSamplingRateInfo, bs);

      if (hcrStatus != 0) {
#if HCR_ERROR_CONCEALMENT
        HcrMuteErroneousLines(hHcr);
#else
        return AAC_DEC_DECODE_FRAME_ERROR;
#endif /* HCR_ERROR_CONCEALMENT */
      }

      FDKpushFor(bs, pAacDecoderChannelInfo->pDynData->specificTo.aac
                         .lenOfReorderedSpectralData);
    }
  }
  /* HCR - Huffman Codeword Reordering int16_t finished */

  if (IsLongBlock(&pAacDecoderChannelInfo->icsInfo) &&
      !(flags & (AC_ELD | AC_SCALABLE))) {
    /* apply pulse data */
    CPulseData_Apply(
        &pAacDecoderChannelInfo->pDynData->specificTo.aac.PulseData,
        GetScaleFactorBandOffsets(&pAacDecoderChannelInfo->icsInfo,
                                  pSamplingRateInfo),
        SPEC_LONG(pSpectralCoefficient));
  }

  return AAC_DEC_OK;
}

static const int16_t noise_level_tab[8] = {
	/* FDKpow(2, (float)(noise_level-14)/3.0f) * 2; (*2 to compensate for
	   fMultDiv2) noise_level_tab(noise_level==0) == 0 by definition
	*/
	0, 3251, 4096, 5161, 6502, 8192, 10321, 13004};

void CBlock_ApplyNoise(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                       SamplingRateInfo_t *pSamplingRateInfo, uint32_t *nfRandomSeed,
                       uint8_t *band_is_noise) {
  const int16_t *swb_offset = GetScaleFactorBandOffsets(
      &pAacDecoderChannelInfo->icsInfo, pSamplingRateInfo);
  int32_t g, win, gwin, sfb, noiseFillingStartOffset, nfStartOffset_sfb;

  /* Obtain noise level and scale factor offset. */
  int32_t noise_level = pAacDecoderChannelInfo->pDynData->specificTo.usac
                        .fd_noise_level_and_offset >>
                    5;
  const int16_t noiseVal_pos = noise_level_tab[noise_level];

  /* noise_offset can change even when noise_level=0. Neccesary for IGF stereo
   * filling */
  const int32_t noise_offset = (pAacDecoderChannelInfo->pDynData->specificTo.usac
                                .fd_noise_level_and_offset &
                            0x1f) -
                           16;

  int32_t max_sfb =
      GetScaleFactorBandsTransmitted(&pAacDecoderChannelInfo->icsInfo);

  noiseFillingStartOffset =
      (GetWindowSequence(&pAacDecoderChannelInfo->icsInfo) == BLOCK_SHORT)
          ? 20
          : 160;
  if (pAacDecoderChannelInfo->granuleLength == 96) {
    noiseFillingStartOffset =
        (3 * noiseFillingStartOffset) /
        4; /* scale offset with 3/4 for coreCoderFrameLength == 768 */
  }

  /* determine sfb from where on noise filling is applied */
  for (sfb = 0; swb_offset[sfb] < noiseFillingStartOffset; sfb++)
    ;
  nfStartOffset_sfb = sfb;

  /* if (noise_level!=0) */
  {
    for (g = 0, win = 0; g < GetWindowGroups(&pAacDecoderChannelInfo->icsInfo);
         g++) {
      int32_t windowGroupLength =
          GetWindowGroupLength(&pAacDecoderChannelInfo->icsInfo, g);
      for (sfb = nfStartOffset_sfb; sfb < max_sfb; sfb++) {
        int32_t bin_start = swb_offset[sfb];
        int32_t bin_stop = swb_offset[sfb + 1];

        int32_t flagN = band_is_noise[g * 16 + sfb];

        /* if all bins of one sfb in one window group are zero modify the scale
         * factor by noise_offset */
        if (flagN) {
          /* Change scaling factors for empty signal bands */
          pAacDecoderChannelInfo->pDynData->aScaleFactor[g * 16 + sfb] +=
              noise_offset;
          /* scale factor "sf" implied gain "g" is g = 2^(sf/4) */
          for (gwin = 0; gwin < windowGroupLength; gwin++) {
            pAacDecoderChannelInfo->pDynData
                ->aSfbScale[(win + gwin) * 16 + sfb] += (noise_offset >> 2);
          }
        }

        uint32_t seed = *nfRandomSeed;
        /* + 1 because exponent of MantissaTable[lsb][0] is always 1. */
        int32_t scale =
            (pAacDecoderChannelInfo->pDynData->aScaleFactor[g * 16 + sfb] >>
             2) +
            1;
        int32_t lsb =
            pAacDecoderChannelInfo->pDynData->aScaleFactor[g * 16 + sfb] & 3;
        int32_t mantissa = MantissaTable[lsb][0];

        for (gwin = 0; gwin < windowGroupLength; gwin++) {
          int32_t *pSpec =
              SPEC(pAacDecoderChannelInfo->pSpectralCoefficient, win + gwin,
                   pAacDecoderChannelInfo->granuleLength);

          int32_t scale1 = scale - pAacDecoderChannelInfo->pDynData
                                   ->aSfbScale[(win + gwin) * 16 + sfb];
          int32_t scaled_noiseVal_pos =
              scaleValue(fMultDiv2(noiseVal_pos, mantissa), scale1);
          int32_t scaled_noiseVal_neg = -scaled_noiseVal_pos;

          /* If the whole band is zero, just fill without checking */
          if (flagN) {
            for (int32_t bin = bin_start; bin < bin_stop; bin++) {
              seed = (uint32_t)(
                  (uint64_t)seed * 69069 +
                  5);
              pSpec[bin] =
                  (seed & 0x10000) ? scaled_noiseVal_neg : scaled_noiseVal_pos;
            } /* for (bin...) */
          }
          /*If band is sparsely filled, check for 0 and fill */
          else {
            for (int32_t bin = bin_start; bin < bin_stop; bin++) {
              if (pSpec[bin] == (int32_t)0) {
                seed = (uint32_t)(
                    (uint64_t)seed * 69069 +
                    5);
                pSpec[bin] = (seed & 0x10000) ? scaled_noiseVal_neg
                                              : scaled_noiseVal_pos;
              }
            } /* for (bin...) */
          }

        } /* for (gwin...) */
        *nfRandomSeed = seed;
      } /* for (sfb...) */
      win += windowGroupLength;
    } /* for (g...) */

  } /* ... */
}

AAC_DECODER_ERROR_t CBlock_ReadAcSpectralData(
    HANDLE_FDK_BITSTREAM hBs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
    const SamplingRateInfo_t *pSamplingRateInfo, const uint32_t frame_length,
    const uint32_t flags) {
  AAC_DECODER_ERROR_t errorAAC = AAC_DEC_OK;
  ARITH_CODING_ERROR error = ARITH_CODER_OK;
  int32_t arith_reset_flag, lg, numWin, win, winLen;
  const int16_t * BandOffsets;

  /* number of transmitted spectral coefficients */
  BandOffsets = GetScaleFactorBandOffsets(&pAacDecoderChannelInfo->icsInfo,
                                          pSamplingRateInfo);
  lg = BandOffsets[GetScaleFactorBandsTransmitted(
      &pAacDecoderChannelInfo->icsInfo)];

  numWin = GetWindowsPerFrame(&pAacDecoderChannelInfo->icsInfo);
  winLen = (IsLongBlock(&pAacDecoderChannelInfo->icsInfo))
               ? (int32_t)frame_length
               : (int32_t)frame_length / numWin;

  if (flags & AC_INDEP) {
    arith_reset_flag = 1;
  } else {
    arith_reset_flag = (uint16_t)FDKreadBits(hBs, 1);
  }

  for (win = 0; win < numWin; win++) {
    error =
        CArco_DecodeArithData(pAacDecoderStaticChannelInfo->hArCo, hBs,
                              SPEC(pAacDecoderChannelInfo->pSpectralCoefficient,
                                   win, pAacDecoderChannelInfo->granuleLength),
                              lg, winLen, arith_reset_flag && (win == 0));
    if (error != ARITH_CODER_OK) {
      goto bail;
    }
  }

bail:
  if (error == ARITH_CODER_ERROR) {
    errorAAC = AAC_DEC_PARSE_ERROR;
  }

  return errorAAC;
}

void ApplyTools(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo[],
                const SamplingRateInfo_t *pSamplingRateInfo, const uint32_t flags,
                const uint32_t elFlags, const int32_t channel,
                const int32_t common_window) {
  if (!(flags & (AC_USAC | AC_RSVD50 | AC_MPEGD_RES | AC_RSV603DA))) {
    CPns_Apply(&pAacDecoderChannelInfo[channel]->data.aac.PnsData,
               &pAacDecoderChannelInfo[channel]->icsInfo,
               pAacDecoderChannelInfo[channel]->pSpectralCoefficient,
               pAacDecoderChannelInfo[channel]->specScale,
               pAacDecoderChannelInfo[channel]->pDynData->aScaleFactor,
               pSamplingRateInfo,
               pAacDecoderChannelInfo[channel]->granuleLength, channel);
  }

  uint8_t nbands =
      GetScaleFactorBandsTransmitted(&pAacDecoderChannelInfo[channel]->icsInfo);

  CTns_Apply(&pAacDecoderChannelInfo[channel]->pDynData->TnsData,
             &pAacDecoderChannelInfo[channel]->icsInfo,
             pAacDecoderChannelInfo[channel]->pSpectralCoefficient,
             pSamplingRateInfo, pAacDecoderChannelInfo[channel]->granuleLength,
             nbands, (elFlags & AC_EL_ENHANCED_NOISE) ? 1 : 0, flags);
}

static int32_t getWindow2Nr(int32_t length, int32_t shape) {
  int32_t nr = 0;

  if (shape == 2) {
    /* Low Overlap, 3/4 zeroed */
    nr = (length * 3) >> 2;
  }

  return nr;
}

int32_t get_gain(const int32_t *x, const int32_t *y, int32_t n) {
  int32_t corr = (int32_t)0;
  int32_t ener = (int32_t)1;

  int32_t headroom_x = getScalefactor(x, n);
  int32_t headroom_y = getScalefactor(y, n);

  /*Calculate the normalization necessary due to addition*/
  /* Check for power of two /special case */
  int32_t width_shift = (int32_t)(fNormz((int32_t)n));
  /* Get the number of bits necessary minus one, because we need one sign bit
   * only */
  width_shift = 31 - width_shift;

  for (int32_t i = 0; i < n; i++) {
    corr +=
        fMultDiv2((x[i] << headroom_x), (y[i] << headroom_y)) >> width_shift;
    ener += fPow2Div2((y[i] << headroom_y)) >> width_shift;
  }

  int32_t exp_corr = (17 - headroom_x) + (17 - headroom_y) + width_shift + 1;
  int32_t exp_ener = ((17 - headroom_y) << 1) + width_shift + 1;

  int32_t temp_exp = 0;
  int32_t output = fDivNormSigned(corr, ener, &temp_exp);

  int32_t output_exp = (exp_corr - exp_ener) + temp_exp;

  int32_t output_shift = 17 - output_exp;
  output_shift = fMin(output_shift, 31);

  output = scaleValue(output, -output_shift);

  return output;
}

void CBlock_FrequencyToTime(
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, int32_t outSamples[],
    const int16_t frameLen, const int32_t frameOk, int32_t *pWorkBuffer1,
    const int32_t aacOutDataHeadroom, uint32_t elFlags, int32_t elCh) {
  int32_t fr, fl, tl, nSpec;
  int32_t nSamples;

  /* Determine left slope length (fl), right slope length (fr) and transform
     length (tl). USAC: The slope length may mismatch with the previous frame in
     case of LPD / FD transitions. The adjustment is handled by the imdct
     implementation.
  */
  tl = frameLen;
  nSpec = 1;

  switch (pAacDecoderChannelInfo->icsInfo.WindowSequence) {
    default:
    case BLOCK_LONG:
      fl = frameLen;
      fr = frameLen -
           getWindow2Nr(frameLen,
                        GetWindowShape(&pAacDecoderChannelInfo->icsInfo));
      /* New startup needs differentiation between sine shape and low overlap
         shape. This is a special case for the LD-AAC transformation windows,
         because the slope length can be different while using the same window
         sequence. */
      if (pAacDecoderStaticChannelInfo->IMdct.prev_tl == 0) {
        fl = fr;
      }
      break;
    case BLOCK_STOP:
      fl = frameLen >> 3;
      fr = frameLen;
      break;
    case BLOCK_START: /* or StopStartSequence */
      fl = frameLen;
      fr = frameLen >> 3;
      break;
    case BLOCK_SHORT:
      fl = fr = frameLen >> 3;
      tl >>= 3;
      nSpec = 8;
      break;
  }

  {
    int32_t last_frame_lost = pAacDecoderStaticChannelInfo->last_lpc_lost;

    if (pAacDecoderStaticChannelInfo->last_core_mode == LPD) {
      int32_t fac_FB = 1;
      if (elFlags & AC_EL_FULLBANDLPD) {
        fac_FB = 2;
      }

      int32_t *synth;

      /* Keep some free space at the beginning of the buffer. To be used for
       * past data */
      if (!(elFlags & AC_EL_LPDSTEREOIDX)) {
        synth = pWorkBuffer1 + ((PIT_MAX_MAX - (1 * L_SUBFR)) * fac_FB);
      } else {
        synth = pWorkBuffer1 + PIT_MAX_MAX * fac_FB;
      }

      int32_t fac_length =
          (pAacDecoderChannelInfo->icsInfo.WindowSequence == BLOCK_SHORT)
              ? (frameLen >> 4)
              : (frameLen >> 3);

      int32_t pitch[NB_SUBFR_SUPERFR + SYN_SFD];
      int32_t pit_gain[NB_SUBFR_SUPERFR + SYN_SFD];

      int32_t nbDiv = (elFlags & AC_EL_FULLBANDLPD) ? 2 : 4;
      int32_t lFrame = (elFlags & AC_EL_FULLBANDLPD) ? frameLen / 2 : frameLen;
      int32_t nbSubfr =
          lFrame / (nbDiv * L_SUBFR); /* number of subframes per division */
      int32_t LpdSfd = (nbDiv * nbSubfr) >> 1;
      int32_t SynSfd = LpdSfd - BPF_SFD;

      memset(
          pitch, 0,
          sizeof(
              pitch));  // added to prevent ferret errors in bass_pf_1sf_delay
      memset(pit_gain, 0, sizeof(pit_gain));

      /* FAC case */
      if (pAacDecoderStaticChannelInfo->last_lpd_mode == 0 ||
          pAacDecoderStaticChannelInfo->last_lpd_mode == 4) {
        int32_t fac_buf[LFAC];
        int16_t *A = pAacDecoderChannelInfo->data.usac.lp_coeff[0];

        if (!frameOk || last_frame_lost ||
            (pAacDecoderChannelInfo->data.usac.fac_data[0] == NULL)) {
          memset(fac_buf, 0,
                      pAacDecoderChannelInfo->granuleLength * sizeof(int32_t));
          pAacDecoderChannelInfo->data.usac.fac_data[0] = fac_buf;
          pAacDecoderChannelInfo->data.usac.fac_data_e[0] = 0;
        }

        int32_t A_exp; /* linear prediction coefficients exponent */
        {
          for (int32_t i = 0; i < M_LP_FILTER_ORDER; i++) {
            A[i] = FX_DBL2FX_SGL(fixp_cos(
                fMult(pAacDecoderStaticChannelInfo->lpc4_lsf[i],
                      FL2FXCONST_SGL((1 << 10) * M_PI / 6400.0)),
                LSF_SCALE - LSPARG_SCALE));
          }

          E_LPC_f_lsp_a_conversion(A, A, &A_exp);
        }


        nSamples = CLpd_FAC_Acelp2Mdct(
                &pAacDecoderStaticChannelInfo->IMdct, synth,
                SPEC_LONG(pAacDecoderChannelInfo->pSpectralCoefficient),
                pAacDecoderChannelInfo->specScale, nSpec,
                pAacDecoderChannelInfo->data.usac.fac_data[0],
                pAacDecoderChannelInfo->data.usac.fac_data_e[0], fac_length,
                frameLen, tl,
                FDKgetWindowSlope(
                    fr, GetWindowShape(&pAacDecoderChannelInfo->icsInfo)),
                fr, A, A_exp, &pAacDecoderStaticChannelInfo->acelp,
                (int32_t)0, /* FAC gain has already been applied. */
                (last_frame_lost || !frameOk), 1,
                pAacDecoderStaticChannelInfo->last_lpd_mode, 0,
                pAacDecoderChannelInfo->currAliasingSymmetry);

      } else {

        nSamples = imlt_block(
                &pAacDecoderStaticChannelInfo->IMdct, synth,
                SPEC_LONG(pAacDecoderChannelInfo->pSpectralCoefficient),
                pAacDecoderChannelInfo->specScale, nSpec, frameLen, tl,
                FDKgetWindowSlope(
                    fl, GetWindowShape(&pAacDecoderChannelInfo->icsInfo)),
                fl,
                FDKgetWindowSlope(
                    fr, GetWindowShape(&pAacDecoderChannelInfo->icsInfo)),
                fr, (int32_t)0,
                pAacDecoderChannelInfo->currAliasingSymmetry
                    ? MLT_FLAG_CURR_ALIAS_SYMMETRY
                    : 0);
      }
      assert(nSamples == frameLen);

      /* The "if" clause is entered both for fullbandLpd mono and
       * non-fullbandLpd*. The "else"-> just for fullbandLpd stereo*/
      if (!(elFlags & AC_EL_LPDSTEREOIDX)) {
        memcpy(pitch, pAacDecoderStaticChannelInfo->old_T_pf,
                  SynSfd * sizeof(int32_t));
        memcpy(pit_gain, pAacDecoderStaticChannelInfo->old_gain_pf,
                  SynSfd * sizeof(int32_t));

        for (int32_t i = SynSfd; i < LpdSfd + 3; i++) {
          pitch[i] = L_SUBFR;
          pit_gain[i] = (int32_t)0;
        }

        if (pAacDecoderStaticChannelInfo->last_lpd_mode == 0) {
          pitch[SynSfd] = pitch[SynSfd - 1];
          pit_gain[SynSfd] = pit_gain[SynSfd - 1];
          if (IsLongBlock(&pAacDecoderChannelInfo->icsInfo)) {
            pitch[SynSfd + 1] = pitch[SynSfd];
            pit_gain[SynSfd + 1] = pit_gain[SynSfd];
          }
        }

        /* Copy old data to the beginning of the buffer */
        {
          memcpy(
              pWorkBuffer1, pAacDecoderStaticChannelInfo->old_synth,
              ((PIT_MAX_MAX - (1 * L_SUBFR)) * fac_FB) * sizeof(int32_t));
        }

        int32_t *p2_synth = pWorkBuffer1 + (PIT_MAX_MAX * fac_FB);

        /* recalculate pitch gain to allow postfilering on FAC area */
        for (int32_t i = 0; i < SynSfd + 2; i++) {
          int32_t T = pitch[i];
          int32_t gain = pit_gain[i];

          if (gain > (int32_t)0) {
            gain = get_gain(&p2_synth[i * L_SUBFR * fac_FB],
                            &p2_synth[(i * L_SUBFR * fac_FB) - fac_FB * T],
                            L_SUBFR * fac_FB);
            pit_gain[i] = gain;
          }
        }

        bass_pf_1sf_delay(p2_synth, pitch, pit_gain, frameLen,
                          (LpdSfd + 2) * L_SUBFR + BPF_SFD * L_SUBFR,
                          frameLen - (LpdSfd + 4) * L_SUBFR, outSamples,
                          aacOutDataHeadroom,
                          pAacDecoderStaticChannelInfo->mem_bpf);
      }

    } else /* last_core_mode was not LPD */
    {
      int32_t *tmp =
          pAacDecoderChannelInfo->pComStaticData->pWorkBufferCore1->mdctOutTemp;

      nSamples = imlt_block(&pAacDecoderStaticChannelInfo->IMdct, tmp,
                     SPEC_LONG(pAacDecoderChannelInfo->pSpectralCoefficient),
                     pAacDecoderChannelInfo->specScale, nSpec, frameLen, tl,
                     FDKgetWindowSlope(
                         fl, GetWindowShape(&pAacDecoderChannelInfo->icsInfo)),
                     fl,
                     FDKgetWindowSlope(
                         fr, GetWindowShape(&pAacDecoderChannelInfo->icsInfo)),
                     fr, (int32_t)0,
                     pAacDecoderChannelInfo->currAliasingSymmetry
                         ? MLT_FLAG_CURR_ALIAS_SYMMETRY
                         : 0);

      scaleValuesSaturate(outSamples, tmp, frameLen,
                          MDCT_OUT_HEADROOM - aacOutDataHeadroom);
    }
  }

  assert(nSamples == frameLen);

  pAacDecoderStaticChannelInfo->last_core_mode =
      (pAacDecoderChannelInfo->icsInfo.WindowSequence == BLOCK_SHORT) ? FD_SHORT
                                                                      : FD_LONG;
  pAacDecoderStaticChannelInfo->last_lpd_mode = 255;
}


void CBlock_FrequencyToTimeLowDelay(
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo, int32_t outSamples[],
    const int16_t frameLen) {
  InvMdctTransformLowDelay_fdk(
      SPEC_LONG(pAacDecoderChannelInfo->pSpectralCoefficient),
      pAacDecoderChannelInfo->specScale[0], outSamples,
      pAacDecoderStaticChannelInfo->pOverlapBuffer, frameLen);
}
