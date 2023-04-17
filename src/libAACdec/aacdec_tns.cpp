

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: temporal noise shaping tool

*/

#include "aacdec_tns.h"
#include <memory.h>
#include "newAACDecoder.h"
#include "../libFDK/FDK_bitstream.h"
#include "../libFDK/FDK_lpc.h"
#include "aac_rom.h"
#include "channelinfo.h"

#define TNS_MAXIMUM_ORDER_AAC 12

/*!
  \brief Reset tns data

  The function resets the tns data

  \return  none
*/
void CTns_Reset(CTnsData *pTnsData) {
    /* Note: the following memset should not be required. */
    memset(pTnsData->Filter, 0, TNS_MAX_WINDOWS * TNS_MAXIMUM_FILTERS * sizeof(CFilter));
    memset(pTnsData->NumberOfFilters, 0, TNS_MAX_WINDOWS * sizeof(uint8_t));
    pTnsData->DataPresent = 0;
    pTnsData->Active = 0;
}

void CTns_ReadDataPresentFlag(HANDLE_FDK_BITSTREAM bs, /*!< pointer to bitstream */
                              CTnsData            *pTnsData)      /*!< pointer to aac decoder channel info */
{
    pTnsData->DataPresent = (uint8_t)FDKreadBits(bs, 1);
}

/*!
  \brief Read tns data from bitstream

  The function reads the elements for tns from
  the bitstream.

  \return  none
*/
AAC_DECODER_ERROR_t CTns_Read(HANDLE_FDK_BITSTREAM bs, CTnsData *pTnsData, const CIcsInfo_t *pIcsInfo,
                            const uint32_t flags) {
    uint8_t           n_filt, order;
    uint8_t           length, coef_res, coef_compress;
    uint8_t           window;
    uint8_t           wins_per_frame;
    uint8_t           isLongFlag;
    uint8_t           start_window;
    AAC_DECODER_ERROR_t ErrorStatus = AAC_DEC_OK;

    if(!pTnsData->DataPresent) { return ErrorStatus; }

    {
        start_window = 0;
        wins_per_frame = GetWindowsPerFrame(pIcsInfo);
        isLongFlag = IsLongBlock(pIcsInfo);
    }

    pTnsData->GainLd = 0;

    for(window = start_window; window < wins_per_frame; window++) {
        pTnsData->NumberOfFilters[window] = n_filt = (uint8_t)FDKreadBits(bs, isLongFlag ? 2 : 1);

        if(n_filt) {
            int32_t index;
            uint8_t nextstopband;

            coef_res = (uint8_t)FDKreadBits(bs, 1);

            nextstopband = GetScaleFactorBandsTotal(pIcsInfo);

            for(index = 0; index < n_filt; index++) {
                CFilter *filter = &pTnsData->Filter[window][index];

                length = (uint8_t)FDKreadBits(bs, isLongFlag ? 6 : 4);

                if(length > nextstopband) { length = nextstopband; }

                filter->StartBand = nextstopband - length;
                filter->StopBand = nextstopband;
                nextstopband = filter->StartBand;

                if(flags & (AC_USAC | AC_RSVD50 | AC_RSV603DA)) {
                    /* max(Order) = 15 (int32_t), 7 (int16_t) */
                    filter->Order = order = (uint8_t)FDKreadBits(bs, isLongFlag ? 4 : 3);
                }
                else {
                    filter->Order = order = (uint8_t)FDKreadBits(bs, isLongFlag ? 5 : 3);

                    if(filter->Order > TNS_MAXIMUM_ORDER) {
                        ErrorStatus = AAC_DEC_TNS_READ_ERROR;
                        return ErrorStatus;
                    }
                }

                assert(order <= TNS_MAXIMUM_ORDER); /* avoid illegal memory access */
                if(order) {
                    uint8_t coef, s_mask;
                    uint8_t i;
                    int8_t  n_mask;

                    static const uint8_t sgn_mask[] = {0x2, 0x4, 0x8};
                    static const int8_t  neg_mask[] = {~0x3, ~0x7, ~0xF};

                    filter->Direction = FDKreadBits(bs, 1) ? -1 : 1;

                    coef_compress = (uint8_t)FDKreadBits(bs, 1);

                    filter->Resolution = coef_res + 3;

                    s_mask = sgn_mask[coef_res + 1 - coef_compress];
                    n_mask = neg_mask[coef_res + 1 - coef_compress];

                    for(i = 0; i < order; i++) {
                        coef = (uint8_t)FDKreadBits(bs, filter->Resolution - coef_compress);
                        filter->Coeff[i] = (coef & s_mask) ? (coef | n_mask) : coef;
                    }
                    pTnsData->GainLd = 4;
                }
            }
        }
    }

    pTnsData->Active = 1;

    return ErrorStatus;
}

void CTns_ReadDataPresentUsac(HANDLE_FDK_BITSTREAM hBs, CTnsData *pTnsData0, CTnsData *pTnsData1, uint8_t *ptns_on_lr,
                              const CIcsInfo_t *pIcsInfo, const uint32_t flags, const uint32_t elFlags,
                              const int32_t fCommonWindow) {
    int32_t common_tns = 0;

    if(fCommonWindow) { common_tns = FDKreadBit(hBs); }
    { *ptns_on_lr = FDKreadBit(hBs); }
    if(common_tns) {
        pTnsData0->DataPresent = 1;
        CTns_Read(hBs, pTnsData0, pIcsInfo, flags);

        pTnsData0->DataPresent = 0;
        pTnsData0->Active = 1;
        *pTnsData1 = *pTnsData0;
    }
    else {
        int32_t tns_present_both;

        tns_present_both = FDKreadBit(hBs);
        if(tns_present_both) {
            pTnsData0->DataPresent = 1;
            pTnsData1->DataPresent = 1;
        }
        else {
            pTnsData1->DataPresent = FDKreadBit(hBs);
            pTnsData0->DataPresent = !pTnsData1->DataPresent;
        }
    }
}

/*!
  \brief Apply tns to spectral lines

  The function applies the tns to the spectrum,

  \return  none
*/
void CTns_Apply(CTnsData       *pTnsData, /*!< pointer to aac decoder info */
                const CIcsInfo_t *pIcsInfo, int32_t* pSpectralCoefficient, const SamplingRateInfo_t *pSamplingRateInfo,
                const int32_t granuleLength, const uint8_t nbands, const uint8_t igf_active, const uint32_t flags) {
    int32_t window, index, start, stop, size, start_window, wins_per_frame;

    if(pTnsData->Active) {
        int32_t  _coeff[20 + 8 + sizeof(int32_t) - 1];
        int32_t *coeff = (int32_t *)(_coeff + (((int32_t)8 - ((size_t)(_coeff)&7)) & 7));

        {
            start_window = 0;
            wins_per_frame = GetWindowsPerFrame(pIcsInfo);
        }

        for(window = start_window; window < wins_per_frame; window++) {
            int32_t *pSpectrum;

            { pSpectrum = SPEC(pSpectralCoefficient, window, granuleLength); }

            for(index = 0; index < pTnsData->NumberOfFilters[window]; index++) {
                CFilter *filter = &pTnsData->Filter[window][index];

                if(filter->Order > 0) {
                    FIXP_TCC *pCoeff;
                    uint8_t   tns_max_bands;

                    pCoeff = coeff;
                    if(filter->Resolution == 3) {
                        int32_t i;
                        for(i = 0; i < filter->Order; i++) *pCoeff++ = FDKaacDec_tnsCoeff3[filter->Coeff[i] + 4];
                    }
                    else {
                        int32_t i;
                        for(i = 0; i < filter->Order; i++) *pCoeff++ = FDKaacDec_tnsCoeff4[filter->Coeff[i] + 8];
                    }

                    switch(granuleLength) {
                        case 480:
                            tns_max_bands = tns_max_bands_tbl_480[pSamplingRateInfo->samplingRateIndex];
                            break;
                        case 512:
                            tns_max_bands = tns_max_bands_tbl_512[pSamplingRateInfo->samplingRateIndex];
                            break;
                        default:
                            tns_max_bands = GetMaximumTnsBands(pIcsInfo, pSamplingRateInfo->samplingRateIndex);
                            /* See redefinition of TNS_MAX_BANDS table */
                            if((flags & (AC_USAC | AC_RSVD50 | AC_RSV603DA)) &&
                               (pSamplingRateInfo->samplingRateIndex > 5)) {
                                tns_max_bands += 1;
                            }
                            break;
                    }

                    start = fixMin(fixMin(filter->StartBand, tns_max_bands), nbands);

                    start = GetScaleFactorBandOffsets(pIcsInfo, pSamplingRateInfo)[start];

                    if(igf_active) { stop = fixMin(filter->StopBand, nbands); }
                    else { stop = fixMin(fixMin(filter->StopBand, tns_max_bands), nbands); }

                    stop = GetScaleFactorBandOffsets(pIcsInfo, pSamplingRateInfo)[stop];

                    size = stop - start;

                    if(size) {
                        int32_t state[TNS_MAXIMUM_ORDER];
                        memset(state, 0, TNS_MAXIMUM_ORDER * sizeof(int32_t));
                        CLpc_SynthesisLattice(pSpectrum + start, size, 0, 0, filter->Direction, coeff, filter->Order,
                                              state);
                    }
                }
            }
        }
    }
}
