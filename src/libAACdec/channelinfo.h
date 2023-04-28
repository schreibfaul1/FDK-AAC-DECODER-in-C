

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

#include "newAACDecoder.h"


void CJointStereo_ApplyMS(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo[2], CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo[2],
                          int32_t *spectrumL, int32_t *spectrumR, int16_t *SFBleftScale, int16_t *SFBrightScale, int16_t *specScaleL,
                          int16_t *specScaleR, const int16_t *pScaleFactorBandOffsets, const uint8_t *pWindowGroupLength, const int32_t windowGroups,
                          const int32_t max_sfb_ste_outside, const int32_t scaleFactorBandsTransmittedL, const int32_t scaleFactorBandsTransmittedR,
                          int32_t *store_dmx_re_prev, int16_t *store_dmx_re_prev_e, const int32_t mainband_flag);
void CJointStereo_ApplyIS(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo[2], const int16_t *pScaleFactorBandOffsets,
                          const uint8_t *pWindowGroupLength, const int32_t windowGroups, const int32_t scaleFactorBandsTransmitted);
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
inline int32_t GetWindowsPerFrame(const CIcsInfo_t *pIcsInfo) { return (pIcsInfo->WindowSequence == BLOCK_SHORT) ? 8 : 1; }
inline uint8_t GetWindowGroups(const CIcsInfo_t *pIcsInfo) { return pIcsInfo->WindowGroups; }
inline uint8_t GetWindowGroupLength(const CIcsInfo_t *pIcsInfo, const int32_t index) { return pIcsInfo->WindowGroupLength[index]; }
inline const uint8_t *GetWindowGroupLengthTable(const CIcsInfo_t *pIcsInfo) { return pIcsInfo->WindowGroupLength; }
inline uint8_t GetScaleFactorBandsTransmitted(const CIcsInfo_t *pIcsInfo) { return pIcsInfo->MaxSfBands; }
inline uint8_t GetScaleMaxFactorBandsTransmitted(const CIcsInfo_t *pIcsInfo0, const CIcsInfo_t *pIcsInfo1) {
    return fMax(pIcsInfo0->MaxSfBands, pIcsInfo1->MaxSfBands);}
inline uint8_t GetScaleFactorBandsTotal(const CIcsInfo_t *pIcsInfo) { return pIcsInfo->TotalSfBands; }
inline uint8_t GetMaximumTnsBands(const CIcsInfo_t *pIcsInfo, const int32_t samplingRateIndex) {/* Note: This function applies to AAC-LC only ! */
    return tns_max_bands_tbl[samplingRateIndex][!IsLongBlock(pIcsInfo)];
}

#endif /* #ifndef CHANNELINFO_H */
