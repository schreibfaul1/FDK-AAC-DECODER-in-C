

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description:

*******************************************************************************/

#ifndef CHANNEL_H
#define CHANNEL_H
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"

#include "../libFDK/FDK_tools_rom.h"
#include "channelinfo.h"
#include "../libMpegTPDec/tpdec_lib.h"

/**
 * \brief Init codeBook SFB indices (section data) with HCB_ESC. Useful for
 * bitstreams which do not have any section data, but still SFB's (scale factor
 * bands). This has the effect that upto the amount of transmitted SFB are
 * treated as non-zero.
 * \param pAacDecoderChannelInfo channel info structure containing a valid
 * icsInfo struct.
 */
void CChannel_CodebookTableInit(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo);

/**
 * \brief decode a channel element. To be called after CChannelElement_Read()
 * \param pAacDecoderChannelInfo pointer to channel data struct. Depending on
 * el_channels either one or two.
 * \param pSamplingRateInfo pointer to sample rate information structure
 * \param el_channels amount of channels of the element to be decoded.
 * \param output pointer to time domain output buffer (ACELP)
 */
void CChannelElement_Decode(
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo[2],
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo[2],
    SamplingRateInfo_t *pSamplingRateInfo, uint32_t flags, uint32_t elFlags,
    int32_t el_channels);

/**
 * \brief Read channel element of given type from bitstream.
 * \param hBs bitstream handle to access bitstream data.
 * \param pAacDecoderChannelInfo pointer array to store channel information.
 * \param aot Audio Object Type
 * \param pSamplingRateInfo sampling rate info table.
 * \param flags common parser guidance flags
 * \param elFlags element specific parser guidance flags
 * \param numberOfChannels amoun of channels contained in the object to be
 * parsed.
 * \param epConfig the current epConfig value obtained from the Audio Specific
 * Config.
 * \param pTp transport decoder handle required for ADTS CRC checking.
 * ...
 * \return an AAC_DECODER_ERROR error code.
 */
AAC_DECODER_ERROR_t CChannelElement_Read(
    HANDLE_FDK_BITSTREAM hBs, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo[],
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo[],
    const AUDIO_OBJECT_TYPE_t aot, SamplingRateInfo_t *pSamplingRateInfo,
    const uint32_t flags, const uint32_t elFlags, const uint32_t frame_length,
    const uint8_t numberOfChannels, const int8_t epConfig,
    HANDLE_TRANSPORTDEC pTpDec);

#endif /* #ifndef CHANNEL_H */
