

/**************************** AAC decoder library ******************************

   Author(s):   Manuel Jander

   Description:

*******************************************************************************/

#ifndef AACDECODER_LIB_H
#define AACDECODER_LIB_H
#include "newAACDecoder.h"
/**
 * \file   aacdecoder_lib.h
 * \brief  FDK AAC decoder library interface header file.
 *

\page INTRO Introduction


*/

#include "../libAACdec/newAACDecoder.h"


typedef struct AAC_DECODER_INSTANCE *HANDLE_AACDECODER; /*!< Pointer to a AAC decoder instance. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initialize ancillary data buffer.
 *
 * \param self    AAC decoder handle.
 * \param buffer  Pointer to (external) ancillary data buffer.
 * \param size    Size of the buffer pointed to by buffer.
 * \return        Error code.
 */
AAC_DECODER_ERROR_t aacDecoder_AncDataInit(HANDLE_AACDECODER self, uint8_t *buffer, int32_t size);

/**
 * \brief Get one ancillary data element.
 *
 * \param self   AAC decoder handle.
 * \param index  Index of the ancillary data element to get.
 * \param ptr    Pointer to a buffer receiving a pointer to the requested
 * ancillary data element.
 * \param size   Pointer to a buffer receiving the length of the requested
 * ancillary data element.
 * \return       Error code.
 */
AAC_DECODER_ERROR_t aacDecoder_AncDataGet(HANDLE_AACDECODER self, int32_t index, uint8_t **ptr, int32_t *size);

/**
 * \brief Set one single decoder parameter.
 *
 * \param self   AAC decoder handle.
 * \param param  Parameter to be set.
 * \param value  Parameter value.
 * \return       Error code.
 */
AAC_DECODER_ERROR_t aacDecoder_SetParam(const HANDLE_AACDECODER self, const AACDEC_PARAM_t param, const int32_t value);

/**
 * \brief              Get free bytes inside decoder internal buffer.
 * \param self         Handle of AAC decoder instance.
 * \param pFreeBytes   Pointer to variable receiving amount of free bytes inside
 * decoder internal buffer.
 * \return             Error code.
 */
AAC_DECODER_ERROR_t aacDecoder_GetFreeBytes(const HANDLE_AACDECODER self, uint32_t *pFreeBytes);

/**
 * \brief               Open an AAC decoder instance.
 * \param transportFmt  The transport type to be used.
 * \param nrOfLayers    Number of transport layers.
 * \return              AAC decoder handle.
 */
HANDLE_AACDECODER aacDecoder_Open(TRANSPORT_TYPE_t transportFmt, uint32_t nrOfLayers);
int32_t           myfunction();
/**
 * \brief Explicitly configure the decoder by passing a raw AudioSpecificConfig
 * (ASC) or a StreamMuxConfig (SMC), contained in a binary buffer. This is
 * required for MPEG-4 and Raw Packets file format bitstreams as well as for
 * LATM bitstreams with no in-band SMC. If the transport format is LATM with or
 * without LOAS, configuration is assumed to be an SMC, for all other file
 * formats an ASC.
 *
 * \param self    AAC decoder handle.
 * \param conf    Pointer to an unsigned char buffer containing the binary
 * configuration buffer (either ASC or SMC).
 * \param length  Length of the configuration buffer in bytes.
 * \return        Error code.
 */
AAC_DECODER_ERROR_t aacDecoder_ConfigRaw(HANDLE_AACDECODER self, uint8_t *conf[], const uint32_t length[]);

/**
 * \brief Submit raw ISO base media file format boxes to decoder for parsing
 * (only some box types are recognized).
 *
 * \param self    AAC decoder handle.
 * \param buffer  Pointer to an unsigned char buffer containing the binary box
 * data (including size and type, can be a sequence of multiple boxes).
 * \param length  Length of the data in bytes.
 * \return        Error code.
 */
AAC_DECODER_ERROR_t aacDecoder_RawISOBMFFData(HANDLE_AACDECODER self, uint8_t *buffer, uint32_t length);

/**
 * \brief Fill AAC decoder's internal input buffer with bitstream data from the
 * external input buffer. The function only copies such data as int32_t as the
 * decoder-internal input buffer is not full. So it grabs whatever it can from
 * pBuffer and returns information (bytesValid) so that at a subsequent call of
 * %aacDecoder_Fill(), the right position in pBuffer can be determined to grab
 * the next data.
 *
 * \param self        AAC decoder handle.
 * \param pBuffer     Pointer to external input buffer.
 * \param bufferSize  Size of external input buffer. This argument is required
 * because decoder-internally we need the information to calculate the offset to
 * pBuffer, where the next available data is, which is then
 * fed into the decoder-internal buffer (as much as
 * possible). Our example framework implementation fills the
 * buffer at pBuffer again, once it contains no available valid bytes anymore
 * (meaning bytesValid equal 0).
 * \param bytesValid  Number of bitstream bytes in the external bitstream buffer
 * that have not yet been copied into the decoder's internal bitstream buffer by
 * calling this function. The value is updated according to
 * the amount of newly copied bytes.
 * \return            Error code.
 */
AAC_DECODER_ERROR_t aacDecoder_Fill(HANDLE_AACDECODER self, uint8_t *pBuffer, const uint32_t bufferSize, uint32_t *bytesValid);

/** Flag for aacDecoder_DecodeFrame(): Trigger the built-in error concealment
 * module to generate a substitute signal for one lost frame. New input data
 * will not be considered.
 */
#define AACDEC_CONCEAL 1
/** Flag for aacDecoder_DecodeFrame(): Flush all filterbanks to get all delayed
 * audio without having new input data. Thus new input data will not be
 * considered.
 */
#define AACDEC_FLUSH 2
/** Flag for aacDecoder_DecodeFrame(): Signal an input bit stream data
 * discontinuity. Resync any internals as necessary.
 */
#define AACDEC_INTR 4
/** Flag for aacDecoder_DecodeFrame(): Clear all signal delay lines and history
 * buffers. CAUTION: This can cause discontinuities in the output signal.
 */
#define AACDEC_CLRHIST 8

/**
 * \brief               Decode one audio frame
 *
 * \param self          AAC decoder handle.
 * \param pTimeData     Pointer to external output buffer where the decoded PCM
 * samples will be stored into.
 * \param timeDataSize  Size of external output buffer.
 * \param flags         Bit field with flags for the decoder: \n
 *                      (flags & AACDEC_CONCEAL) == 1: Do concealment. \n
 *                      (flags & AACDEC_FLUSH) == 2: Discard input data. Flush
 * filter banks (output delayed audio). \n (flags & AACDEC_INTR) == 4: Input
 * data is discontinuous. Resynchronize any internals as
 * necessary. \n (flags & AACDEC_CLRHIST) == 8: Clear all signal delay lines and
 * history buffers.
 * \return              Error code.
 */
AAC_DECODER_ERROR_t aacDecoder_DecodeFrame(HANDLE_AACDECODER self, int16_t *pTimeData, const int32_t timeDataSize, const uint32_t flags);

/**
 * \brief       De-allocate all resources of an AAC decoder instance.
 *
 * \param self  AAC decoder handle.
 * \return      void.
 */
void aacDecoder_Close(HANDLE_AACDECODER self);

/**
 * \brief       Get CStreamInfo handle from decoder.
 *
 * \param self  AAC decoder handle.
 * \return      Reference to requested CStreamInfo.
 */
CStreamInfo *aacDecoder_GetStreamInfo(HANDLE_AACDECODER self);

/**
 * \brief       Get decoder library info.
 *
 * \param info  Pointer to an allocated LIB_INFO_t structure.
 * \return      0 on success.
 */
int32_t aacDecoder_GetLibInfo(LIB_INFO_t *info);

#ifdef __cplusplus
}
#endif

#endif /* AACDECODER_LIB_H */
