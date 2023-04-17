

/******************* Library for basic calculation routines ********************

   Author(s):   M. Lohwasser

   Description: common bitbuffer read/write routines

*******************************************************************************/

#ifndef FDK_BITBUFFER_H
#define FDK_BITBUFFER_H

#include <stdint.h>
#include "../libAACdec/newAACDecoder.h"
#include "FDK_archdef.h"


/* leave 3 bits headroom so MAX_BUFSIZE can be represented in bits as well. */
#define MAX_BUFSIZE_BYTES (0x10000000)

typedef struct {
  uint32_t ValidBits;
  uint32_t ReadOffset;
  uint32_t WriteOffset;
  uint32_t BitNdx;

  uint8_t *Buffer;
  uint32_t bufSize;
  uint32_t bufBits;
} FDK_BITBUF;

typedef FDK_BITBUF *HANDLE_FDK_BITBUF;

#ifdef __cplusplus
extern "C" {
#endif

extern const uint32_t BitMask[32 + 1];

/**  The BitBuffer Functions are called straight from FDK_bitstream Interface.
     For Functions functional survey look there.
*/

void FDK_CreateBitBuffer(HANDLE_FDK_BITBUF *hBitBuffer, uint8_t *pBuffer,
                         uint32_t bufSize);

void FDK_InitBitBuffer(HANDLE_FDK_BITBUF hBitBuffer, uint8_t *pBuffer,
                       uint32_t bufSize, uint32_t validBits);

void FDK_ResetBitBuffer(HANDLE_FDK_BITBUF hBitBuffer);

void FDK_DeleteBitBuffer(HANDLE_FDK_BITBUF hBitBuffer);

int32_t FDK_get(HANDLE_FDK_BITBUF hBitBuffer, const uint32_t numberOfBits);

int32_t FDK_get32(HANDLE_FDK_BITBUF hBitBuf);

void FDK_put(HANDLE_FDK_BITBUF hBitBuffer, uint32_t value, const uint32_t numberOfBits);

int32_t FDK_getBwd(HANDLE_FDK_BITBUF hBitBuffer, const uint32_t numberOfBits);
void FDK_putBwd(HANDLE_FDK_BITBUF hBitBuffer, uint32_t value,
                const uint32_t numberOfBits);

void FDK_pushBack(HANDLE_FDK_BITBUF hBitBuffer, const uint32_t numberOfBits,
                  uint8_t config);

void FDK_pushForward(HANDLE_FDK_BITBUF hBitBuffer, const uint32_t numberOfBits,
                     uint8_t config);

uint32_t FDK_getValidBits(HANDLE_FDK_BITBUF hBitBuffer);

int32_t FDK_getFreeBits(HANDLE_FDK_BITBUF hBitBuffer);

void FDK_Feed(HANDLE_FDK_BITBUF hBitBuffer, const uint8_t inputBuffer[],
              const uint32_t bufferSize, uint32_t *bytesValid);

void FDK_Copy(HANDLE_FDK_BITBUF hBitBufDst, HANDLE_FDK_BITBUF hBitBufSrc,
              uint32_t *bytesValid);

void FDK_Fetch(HANDLE_FDK_BITBUF hBitBuffer, uint8_t outBuf[], uint32_t *writeBytes);

#ifdef __cplusplus
}
#endif

#endif
