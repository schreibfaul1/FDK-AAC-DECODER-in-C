

/**************************** AAC decoder library ******************************

   Author(s):   Robert Weidner (DSP Solutions)

   Description: HCR Decoder: Interface function declaration; common defines
                and structures; defines for switching error-generator,
                -detector, and -concealment

*******************************************************************************/

#ifndef AACDEC_HCR_H
#define AACDEC_HCR_H
#include "newAACDecoder.h"
#include "channelinfo.h"
#include "../libFDK/FDK_bitstream.h"

uint32_t HcrInit(H_HCR_INFO pHcr, CAacDecoderChannelInfo *pAacDecoderChannelInfo,
             const SamplingRateInfo *pSamplingRateInfo,
             HANDLE_FDK_BITSTREAM bs);
uint32_t HcrDecoder(H_HCR_INFO hHcr, CAacDecoderChannelInfo *pAacDecoderChannelInfo,
                const SamplingRateInfo *pSamplingRateInfo,
                HANDLE_FDK_BITSTREAM bs);
void CarryBitToBranchValue(uint8_t carryBit, uint32_t treeNode, uint32_t *branchValue,
                           uint32_t *branchNode);

void CHcr_Read(HANDLE_FDK_BITSTREAM bs,
               CAacDecoderChannelInfo *pAacDecoderChannelInfo,
               const MP4_ELEMENT_ID_t globalHcrType);
void HcrMuteErroneousLines(H_HCR_INFO hHcr);

void setHcrType(H_HCR_INFO hHcr, MP4_ELEMENT_ID_t type);
int32_t getHcrType(H_HCR_INFO hHcr);

#endif /* AACDEC_HCR_H */
