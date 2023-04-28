

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: individual channel stream info

*******************************************************************************/

#ifndef CHANNELINFO_H
#define CHANNELINFO_H
#include "../libArithCoding/ac_arith_coder.h"
#include "../libFDK/FDK_bitstream.h"
#include "../libFDK/common_fix.h"
#include "../libFDK/mdct.h"
#include "aac_rom.h"
#include "aacdec_hcr_types.h"
#include "aacdec_pns.h"
#include "aacdec_tns.h"
#include "aacdecoder_lib.h"
#include "conceal_types.h"
#include "newAACDecoder.h"

#include "rvlc_info.h"
#include "stereo.h"
#include "usacdec_acelp.h"
















/*
 * This union must be allocated for every element (up to 2 channels).
 */



/* channelinfo.cpp */

AAC_DECODER_ERROR_t getSamplingRateInfo(SamplingRateInfo_t *t, uint32_t samplesPerFrame, uint32_t samplingRateIndex,
                                      uint32_t samplingRate);

/**
 * \brief Read max SFB from bit stream and assign TotalSfBands according
 *        to the window sequence and sample rate.
 * \param hBs bit stream handle as data source
 * \param pIcsInfo IcsInfo structure to read the window sequence and store
 * MaxSfBands and TotalSfBands
 * \param pSamplingRateInfo read only
 */
AAC_DECODER_ERROR_t IcsReadMaxSfb(HANDLE_FDK_BITSTREAM hBs, CIcsInfo_t *pIcsInfo,
                                const SamplingRateInfo_t *pSamplingRateInfo);

AAC_DECODER_ERROR_t IcsRead(HANDLE_FDK_BITSTREAM bs, CIcsInfo_t *pIcsInfo, const SamplingRateInfo_t *SamplingRateInfoTable,
                          const uint32_t flags);

/* stereo.cpp, only called from this file */

/*!
  \brief Applies MS stereo.

  The function applies MS stereo.

  \param pAacDecoderChannelInfo aac channel info.
  \param pScaleFactorBandOffsets pointer to scalefactor band offsets.
  \param pWindowGroupLength pointer to window group length array.
  \param windowGroups number of window groups.
  \param scaleFactorBandsTransmittedL number of transmitted scalefactor bands in
  left channel. \param scaleFactorBandsTransmittedR number of transmitted
  scalefactor bands in right channel. May differ from
  scaleFactorBandsTransmittedL only for USAC. \return  none
*/
void CJointStereo_ApplyMS(CAacDecoderChannelInfo_t       *pAacDecoderChannelInfo[2],
                          CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo[2], int32_t *spectrumL,
                          int32_t *spectrumR, int16_t *SFBleftScale, int16_t *SFBrightScale, int16_t *specScaleL,
                          int16_t *specScaleR, const int16_t *pScaleFactorBandOffsets,
                          const uint8_t *pWindowGroupLength, const int32_t windowGroups,
                          const int32_t max_sfb_ste_outside, const int32_t scaleFactorBandsTransmittedL,
                          const int32_t scaleFactorBandsTransmittedR, int32_t *store_dmx_re_prev,
                          int16_t *store_dmx_re_prev_e, const int32_t mainband_flag);

/*!
  \brief Applies intensity stereo

  The function applies intensity stereo.

  \param pAacDecoderChannelInfo aac channel info.
  \param pScaleFactorBandOffsets pointer to scalefactor band offsets.
  \param pWindowGroupLength pointer to window group length array.
  \param windowGroups number of window groups.
  \param scaleFactorBandsTransmitted number of transmitted scalefactor bands.
  \return  none
*/
void CJointStereo_ApplyIS(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo[2], const int16_t *pScaleFactorBandOffsets,
                          const uint8_t *pWindowGroupLength, const int32_t windowGroups,
                          const int32_t scaleFactorBandsTransmitted);

/* aacdec_pns.cpp */
int32_t CPns_IsPnsUsed(const CPnsData_t *pPnsData, const int32_t group, const int32_t band);

void CPns_SetCorrelation(CPnsData_t *pPnsData, const int32_t group, const int32_t band, const int32_t outofphase);

/****************** inline functions ******************/

inline uint8_t IsValid(const CIcsInfo_t *pIcsInfo) { return pIcsInfo->Valid; }

inline uint8_t IsLongBlock(const CIcsInfo_t *pIcsInfo) { return (pIcsInfo->WindowSequence != BLOCK_SHORT); }

inline uint8_t GetWindowShape(const CIcsInfo_t *pIcsInfo) { return pIcsInfo->WindowShape; }

inline BLOCK_TYPE_t GetWindowSequence(const CIcsInfo_t *pIcsInfo) { return pIcsInfo->WindowSequence; }

inline const int16_t *GetScaleFactorBandOffsets(const CIcsInfo_t *pIcsInfo, const SamplingRateInfo_t *SamplingRateInfo_t) {
    if(IsLongBlock(pIcsInfo)) { return SamplingRateInfo_t->ScaleFactorBands_Long; }
    else { return SamplingRateInfo_t->ScaleFactorBands_Short; }
}

inline uint8_t GetNumberOfScaleFactorBands(const CIcsInfo_t *pIcsInfo, const SamplingRateInfo_t *SamplingRateInfo_t) {
    if(IsLongBlock(pIcsInfo)) { return SamplingRateInfo_t->NumberOfScaleFactorBands_Long; }
    else { return SamplingRateInfo_t->NumberOfScaleFactorBands_Short; }
}

inline int32_t GetWindowsPerFrame(const CIcsInfo_t *pIcsInfo) {
    return (pIcsInfo->WindowSequence == BLOCK_SHORT) ? 8 : 1;
}

inline uint8_t GetWindowGroups(const CIcsInfo_t *pIcsInfo) { return pIcsInfo->WindowGroups; }

inline uint8_t GetWindowGroupLength(const CIcsInfo_t *pIcsInfo, const int32_t index) {
    return pIcsInfo->WindowGroupLength[index];
}

inline const uint8_t *GetWindowGroupLengthTable(const CIcsInfo_t *pIcsInfo) { return pIcsInfo->WindowGroupLength; }

inline uint8_t GetScaleFactorBandsTransmitted(const CIcsInfo_t *pIcsInfo) { return pIcsInfo->MaxSfBands; }

inline uint8_t GetScaleMaxFactorBandsTransmitted(const CIcsInfo_t *pIcsInfo0, const CIcsInfo_t *pIcsInfo1) {
    return fMax(pIcsInfo0->MaxSfBands, pIcsInfo1->MaxSfBands);
}

inline uint8_t GetScaleFactorBandsTotal(const CIcsInfo_t *pIcsInfo) { return pIcsInfo->TotalSfBands; }

/* Note: This function applies to AAC-LC only ! */
inline uint8_t GetMaximumTnsBands(const CIcsInfo_t *pIcsInfo, const int32_t samplingRateIndex) {
    return tns_max_bands_tbl[samplingRateIndex][!IsLongBlock(pIcsInfo)];
}

#endif /* #ifndef CHANNELINFO_H */
