

/******************* Library for basic calculation routines ********************

   Author(s):   M. Lohwasser

   Description: common bitbuffer read/write routines

*******************************************************************************/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include <stdint.h>


#include "common_fix.h"
#include "fixminmax.h"


void FDK_CreateBitBuffer(HANDLE_FDK_BITBUF *hBitBuf, uint8_t *pBuffer,
                         uint32_t bufSize) {
  FDK_InitBitBuffer(*hBitBuf, pBuffer, bufSize, 0);

  memset((*hBitBuf)->Buffer, 0, bufSize * sizeof(uint8_t));
}

void FDK_DeleteBitBuffer(HANDLE_FDK_BITBUF hBitBuf) { ; }

void FDK_InitBitBuffer(HANDLE_FDK_BITBUF hBitBuf, uint8_t *pBuffer, uint32_t bufSize,
                       uint32_t validBits) {
  hBitBuf->ValidBits = validBits;
  hBitBuf->ReadOffset = 0;
  hBitBuf->WriteOffset = 0;
  hBitBuf->BitNdx = 0;

  hBitBuf->Buffer = pBuffer;
  hBitBuf->bufSize = bufSize;
  hBitBuf->bufBits = (bufSize << 3);
  /*assure bufsize (2^n) */
  assert(hBitBuf->ValidBits <= hBitBuf->bufBits);
  assert((bufSize > 0) && (bufSize <= MAX_BUFSIZE_BYTES));
  {
    uint32_t x = 0, n = bufSize;
    for (x = 0; n > 0; x++, n >>= 1) {
    }
    if (bufSize != ((uint32_t)1 << (x - 1))) {
      assert(0);
    }
  }
}

void FDK_ResetBitBuffer(HANDLE_FDK_BITBUF hBitBuf) {
  hBitBuf->ValidBits = 0;
  hBitBuf->ReadOffset = 0;
  hBitBuf->WriteOffset = 0;
  hBitBuf->BitNdx = 0;
}

#ifndef FUNCTION_FDK_get
int32_t FDK_get(HANDLE_FDK_BITBUF hBitBuf, const uint32_t numberOfBits) {
  uint32_t byteOffset = hBitBuf->BitNdx >> 3;
  uint32_t bitOffset = hBitBuf->BitNdx & 0x07;

  hBitBuf->BitNdx = (hBitBuf->BitNdx + numberOfBits) & (hBitBuf->bufBits - 1);
  hBitBuf->ValidBits -= numberOfBits;

  uint32_t byteMask = hBitBuf->bufSize - 1;

  uint32_t tx = (hBitBuf->Buffer[byteOffset & byteMask] << 24) |
            (hBitBuf->Buffer[(byteOffset + 1) & byteMask] << 16) |
            (hBitBuf->Buffer[(byteOffset + 2) & byteMask] << 8) |
            hBitBuf->Buffer[(byteOffset + 3) & byteMask];

  if (bitOffset) {
    tx <<= bitOffset;
    tx |= hBitBuf->Buffer[(byteOffset + 4) & byteMask] >> (8 - bitOffset);
  }

  return (tx >> (32 - numberOfBits));
}
#endif /* #ifndef FUNCTION_FDK_get */

#ifndef FUNCTION_FDK_get32
int32_t FDK_get32(HANDLE_FDK_BITBUF hBitBuf) {
  uint32_t BitNdx = hBitBuf->BitNdx + 32;
  hBitBuf->BitNdx = BitNdx & (hBitBuf->bufBits - 1);
  hBitBuf->ValidBits = (uint32_t)((int32_t)hBitBuf->ValidBits - (int32_t)32);

  uint32_t byteOffset = (BitNdx - 1) >> 3;
  if (BitNdx <= hBitBuf->bufBits) {
    uint32_t cache = (hBitBuf->Buffer[(byteOffset - 3)] << 24) |
                 (hBitBuf->Buffer[(byteOffset - 2)] << 16) |
                 (hBitBuf->Buffer[(byteOffset - 1)] << 8) |
                 hBitBuf->Buffer[(byteOffset - 0)];

    if ((BitNdx = (BitNdx & 7)) != 0) {
      cache = (cache >> (8 - BitNdx)) |
              ((uint32_t)hBitBuf->Buffer[byteOffset - 4] << (24 + BitNdx));
    }
    return (cache);
  } else {
    uint32_t byte_mask = hBitBuf->bufSize - 1;
    uint32_t cache = (hBitBuf->Buffer[(byteOffset - 3) & byte_mask] << 24) |
                 (hBitBuf->Buffer[(byteOffset - 2) & byte_mask] << 16) |
                 (hBitBuf->Buffer[(byteOffset - 1) & byte_mask] << 8) |
                 hBitBuf->Buffer[(byteOffset - 0) & byte_mask];

    if ((BitNdx = (BitNdx & 7)) != 0) {
      cache = (cache >> (8 - BitNdx)) |
              ((uint32_t)hBitBuf->Buffer[(byteOffset - 4) & byte_mask]
               << (24 + BitNdx));
    }
    return (cache);
  }
}
#endif

int32_t FDK_getBwd(HANDLE_FDK_BITBUF hBitBuf, const uint32_t numberOfBits) {
  uint32_t byteOffset = hBitBuf->BitNdx >> 3;
  uint32_t bitOffset = hBitBuf->BitNdx & 0x07;
  uint32_t byteMask = hBitBuf->bufSize - 1;
  int32_t i;

  hBitBuf->BitNdx = (hBitBuf->BitNdx - numberOfBits) & (hBitBuf->bufBits - 1);
  hBitBuf->ValidBits += numberOfBits;

  uint32_t tx = hBitBuf->Buffer[(byteOffset - 3) & byteMask] << 24 |
            hBitBuf->Buffer[(byteOffset - 2) & byteMask] << 16 |
            hBitBuf->Buffer[(byteOffset - 1) & byteMask] << 8 |
            hBitBuf->Buffer[byteOffset & byteMask];
  uint32_t txa = 0x0;

  tx >>= (8 - bitOffset);

  if (bitOffset && numberOfBits > 24) {
    tx |= hBitBuf->Buffer[(byteOffset - 4) & byteMask] << (24 + bitOffset);
  }

  /* in place turn around */
  for (i = 0; i < 16; i++) {
    uint32_t bitMaskR = 0x00000001 << i;
    uint32_t bitMaskL = 0x80000000 >> i;

    txa |= (tx & bitMaskR) << (31 - (i << 1));
    txa |= (tx & bitMaskL) >> (31 - (i << 1));
  }

  return (txa >> (32 - numberOfBits));
}

void FDK_put(HANDLE_FDK_BITBUF hBitBuf, uint32_t value, const uint32_t numberOfBits) {
  if (numberOfBits != 0) {
    uint32_t byteOffset0 = hBitBuf->BitNdx >> 3;
    uint32_t bitOffset = hBitBuf->BitNdx & 0x7;

    hBitBuf->BitNdx = (hBitBuf->BitNdx + numberOfBits) & (hBitBuf->bufBits - 1);
    hBitBuf->ValidBits += numberOfBits;

    uint32_t byteMask = hBitBuf->bufSize - 1;

    uint32_t byteOffset1 = (byteOffset0 + 1) & byteMask;
    uint32_t byteOffset2 = (byteOffset0 + 2) & byteMask;
    uint32_t byteOffset3 = (byteOffset0 + 3) & byteMask;

    // Create tmp containing free bits at the left border followed by bits to
    // write, LSB's are cleared, if available Create mask to apply upon all
    // buffer bytes
    uint32_t tmp = (value << (32 - numberOfBits)) >> bitOffset;
    uint32_t mask = ~((BitMask[numberOfBits] << (32 - numberOfBits)) >> bitOffset);

    // read all 4 bytes from buffer and create a 32-bit cache
    uint32_t cache = (((uint32_t)hBitBuf->Buffer[byteOffset0]) << 24) |
                 (((uint32_t)hBitBuf->Buffer[byteOffset1]) << 16) |
                 (((uint32_t)hBitBuf->Buffer[byteOffset2]) << 8) |
                 (((uint32_t)hBitBuf->Buffer[byteOffset3]) << 0);

    cache = (cache & mask) | tmp;
    hBitBuf->Buffer[byteOffset0] = (uint8_t)(cache >> 24);
    hBitBuf->Buffer[byteOffset1] = (uint8_t)(cache >> 16);
    hBitBuf->Buffer[byteOffset2] = (uint8_t)(cache >> 8);
    hBitBuf->Buffer[byteOffset3] = (uint8_t)(cache >> 0);

    if ((bitOffset + numberOfBits) > 32) {
      uint32_t byteOffset4 = (byteOffset0 + 4) & byteMask;
      // remaining bits: in range 1..7
      // replace MSBits of next byte in buffer by LSBits of "value"
      int32_t bits = (bitOffset + numberOfBits) & 7;
      cache =
          (uint32_t)hBitBuf->Buffer[byteOffset4] & (~(BitMask[bits] << (8 - bits)));
      cache |= value << (8 - bits);
      hBitBuf->Buffer[byteOffset4] = (uint8_t)cache;
    }
  }
}

void FDK_putBwd(HANDLE_FDK_BITBUF hBitBuf, uint32_t value,
                const uint32_t numberOfBits) {
  uint32_t byteOffset = hBitBuf->BitNdx >> 3;
  uint32_t bitOffset = 7 - (hBitBuf->BitNdx & 0x07);
  uint32_t byteMask = hBitBuf->bufSize - 1;

  uint32_t mask = ~(BitMask[numberOfBits] << bitOffset);
  uint32_t tmp = 0x0000;
  int32_t i;

  hBitBuf->BitNdx = (hBitBuf->BitNdx - numberOfBits) & (hBitBuf->bufBits - 1);
  hBitBuf->ValidBits -= numberOfBits;

  /* in place turn around */
  for (i = 0; i < 16; i++) {
    uint32_t bitMaskR = 0x00000001 << i;
    uint32_t bitMaskL = 0x80000000 >> i;

    tmp |= (value & bitMaskR) << (31 - (i << 1));
    tmp |= (value & bitMaskL) >> (31 - (i << 1));
  }
  value = tmp;
  tmp = value >> (32 - numberOfBits) << bitOffset;

  hBitBuf->Buffer[byteOffset & byteMask] =
      (hBitBuf->Buffer[byteOffset & byteMask] & (mask)) | (uint8_t)(tmp);
  hBitBuf->Buffer[(byteOffset - 1) & byteMask] =
      (hBitBuf->Buffer[(byteOffset - 1) & byteMask] & (mask >> 8)) |
      (uint8_t)(tmp >> 8);
  hBitBuf->Buffer[(byteOffset - 2) & byteMask] =
      (hBitBuf->Buffer[(byteOffset - 2) & byteMask] & (mask >> 16)) |
      (uint8_t)(tmp >> 16);
  hBitBuf->Buffer[(byteOffset - 3) & byteMask] =
      (hBitBuf->Buffer[(byteOffset - 3) & byteMask] & (mask >> 24)) |
      (uint8_t)(tmp >> 24);

  if ((bitOffset + numberOfBits) > 32) {
    hBitBuf->Buffer[(byteOffset - 4) & byteMask] =
        (uint8_t)(value >> (64 - numberOfBits - bitOffset)) |
        (hBitBuf->Buffer[(byteOffset - 4) & byteMask] &
         ~(BitMask[bitOffset] >> (32 - numberOfBits)));
  }
}

#ifndef FUNCTION_FDK_pushBack
void FDK_pushBack(HANDLE_FDK_BITBUF hBitBuf, const uint32_t numberOfBits,
                  uint8_t config) {
  hBitBuf->ValidBits =
      (config == 0) ? (uint32_t)((int32_t)hBitBuf->ValidBits + (int32_t)numberOfBits)
                    : ((uint32_t)((int32_t)hBitBuf->ValidBits - (int32_t)numberOfBits));
  hBitBuf->BitNdx = ((uint32_t)((int32_t)hBitBuf->BitNdx - (int32_t)numberOfBits)) &
                    (hBitBuf->bufBits - 1);
}
#endif

void FDK_pushForward(HANDLE_FDK_BITBUF hBitBuf, const uint32_t numberOfBits,
                     uint8_t config) {
  hBitBuf->ValidBits =
      (config == 0) ? ((uint32_t)((int32_t)hBitBuf->ValidBits - (int32_t)numberOfBits))
                    : (uint32_t)((int32_t)hBitBuf->ValidBits + (int32_t)numberOfBits);
  hBitBuf->BitNdx =
      (uint32_t)((int32_t)hBitBuf->BitNdx + (int32_t)numberOfBits) & (hBitBuf->bufBits - 1);
}

#ifndef FUNCTION_FDK_getValidBits
uint32_t FDK_getValidBits(HANDLE_FDK_BITBUF hBitBuf) { return hBitBuf->ValidBits; }
#endif /* #ifndef FUNCTION_FDK_getValidBits */

int32_t FDK_getFreeBits(HANDLE_FDK_BITBUF hBitBuf) {
  return (hBitBuf->bufBits - hBitBuf->ValidBits);
}

void FDK_Feed(HANDLE_FDK_BITBUF hBitBuf, const uint8_t * inputBuffer,
              const uint32_t bufferSize, uint32_t *bytesValid) {
  inputBuffer = &inputBuffer[bufferSize - *bytesValid];

  uint32_t bTotal = 0;

  uint32_t bToRead =
      fMin(hBitBuf->bufBits,
           (uint32_t)fMax(0, ((int32_t)hBitBuf->bufBits - (int32_t)hBitBuf->ValidBits))) >>
      3;
  uint32_t noOfBytes =
      fMin(bToRead,
           *bytesValid);  //(bToRead < *bytesValid) ? bToRead : *bytesValid ;

  while (noOfBytes > 0) {
    /* split read to buffer size */
    bToRead = hBitBuf->bufSize - hBitBuf->ReadOffset;
    bToRead = fMin(bToRead,
                   noOfBytes);  //(bToRead < noOfBytes) ? bToRead : noOfBytes ;

    /* copy 'bToRead' bytes from 'ptr' to inputbuffer */
    memcpy(&hBitBuf->Buffer[hBitBuf->ReadOffset], inputBuffer,
              bToRead * sizeof(uint8_t));

    /* add noOfBits to number of valid bits in buffer */
    hBitBuf->ValidBits = (uint32_t)((int32_t)hBitBuf->ValidBits + (int32_t)(bToRead << 3));
    bTotal += bToRead;
    inputBuffer += bToRead;

    hBitBuf->ReadOffset =
        (hBitBuf->ReadOffset + bToRead) & (hBitBuf->bufSize - 1);
    noOfBytes -= bToRead;
  }

  *bytesValid -= bTotal;
}

void CopyAlignedBlock(HANDLE_FDK_BITBUF h_BitBufSrc, uint8_t * dstBuffer,
                      uint32_t bToRead) {
  uint32_t byteOffset = h_BitBufSrc->BitNdx >> 3;
  const uint32_t byteMask = h_BitBufSrc->bufSize - 1;

  uint8_t * pBBB = h_BitBufSrc->Buffer;
  for (uint32_t i = 0; i < bToRead; i++) {
    dstBuffer[i] = pBBB[(byteOffset + i) & byteMask];
  }

  bToRead <<= 3;

  h_BitBufSrc->BitNdx =
      (h_BitBufSrc->BitNdx + bToRead) & (h_BitBufSrc->bufBits - 1);
  h_BitBufSrc->ValidBits -= bToRead;
}

void FDK_Copy(HANDLE_FDK_BITBUF h_BitBufDst, HANDLE_FDK_BITBUF h_BitBufSrc,
              uint32_t *bytesValid) {
  int32_t bTotal = 0;

  /* limit noOfBytes to valid bytes in src buffer and available bytes in dst
   * buffer */
  uint32_t bToRead = h_BitBufSrc->ValidBits >> 3;
  uint32_t noOfBytes =
      fMin(bToRead,
           *bytesValid);  //(*bytesValid < bToRead) ? *bytesValid : bToRead ;
  bToRead = FDK_getFreeBits(h_BitBufDst);
  noOfBytes =
      fMin(bToRead, noOfBytes);  //(bToRead < noOfBytes) ? bToRead : noOfBytes;

  while (noOfBytes > 0) {
    /* Split Read to buffer size */
    bToRead = h_BitBufDst->bufSize - h_BitBufDst->ReadOffset;
    bToRead = fMin(noOfBytes,
                   bToRead);  //(noOfBytes < bToRead) ? noOfBytes : bToRead ;

    /* copy 'bToRead' bytes from buffer to buffer */
    if (!(h_BitBufSrc->BitNdx & 0x07)) {
      CopyAlignedBlock(h_BitBufSrc,
                       h_BitBufDst->Buffer + h_BitBufDst->ReadOffset, bToRead);
    } else {
      for (uint32_t i = 0; i < bToRead; i++) {
        h_BitBufDst->Buffer[h_BitBufDst->ReadOffset + i] =
            (uint8_t)FDK_get(h_BitBufSrc, 8);
      }
    }

    /* add noOfBits to number of valid bits in buffer */
    h_BitBufDst->ValidBits += bToRead << 3;
    bTotal += bToRead;

    h_BitBufDst->ReadOffset =
        (h_BitBufDst->ReadOffset + bToRead) & (h_BitBufDst->bufSize - 1);
    noOfBytes -= bToRead;
  }

  *bytesValid -= bTotal;
}

void FDK_Fetch(HANDLE_FDK_BITBUF hBitBuf, uint8_t *outBuf, uint32_t *writeBytes) {
  uint8_t * outputBuffer = outBuf;
  uint32_t bTotal = 0;

  uint32_t bToWrite = (hBitBuf->ValidBits) >> 3;
  uint32_t noOfBytes =
      fMin(bToWrite,
           *writeBytes);  //(bToWrite < *writeBytes) ? bToWrite : *writeBytes ;

  while (noOfBytes > 0) {
    /* split write to buffer size */
    bToWrite = hBitBuf->bufSize - hBitBuf->WriteOffset;
    bToWrite = fMin(
        bToWrite, noOfBytes);  //(bToWrite < noOfBytes) ? bToWrite : noOfBytes ;

    /* copy 'bToWrite' bytes from bitbuffer to outputbuffer */
    memcpy(outputBuffer, &hBitBuf->Buffer[hBitBuf->WriteOffset],
              bToWrite * sizeof(uint8_t));

    /* sub noOfBits from number of valid bits in buffer */
    hBitBuf->ValidBits -= bToWrite << 3;
    bTotal += bToWrite;
    outputBuffer += bToWrite;

    hBitBuf->WriteOffset =
        (hBitBuf->WriteOffset + bToWrite) & (hBitBuf->bufSize - 1);
    noOfBytes -= bToWrite;
  }

  *writeBytes = bTotal;
}
