

/******************* MPEG transport format decoder library *********************

   Author(s):   Christian Griebel

   Description: DRM transport stuff

*******************************************************************************/
#include "../libAACdec/newAACDecoder.h"
#include "tpdec_drm.h"

#include "../libFDK/FDK_bitstream.h"

void drmRead_CrcInit(HANDLE_DRM pDrm) /*!< pointer to drm crc info stucture */
{
  assert(pDrm != NULL);

  FDKcrcInit(&pDrm->crcInfo, 0x001d, 0xFFFF, 8);
}

int32_t drmRead_CrcStartReg(
    HANDLE_DRM pDrm,          /*!< pointer to drm stucture */
    HANDLE_FDK_BITSTREAM hBs, /*!< handle to current bit buffer structure */
    int32_t mBits                 /*!< number of bits in crc region */
) {
  assert(pDrm != NULL);

  FDKcrcReset(&pDrm->crcInfo);

  pDrm->crcReadValue = FDKreadBits(hBs, 8);

  return (FDKcrcStartReg(&pDrm->crcInfo, hBs, mBits));
}

void drmRead_CrcEndReg(
    HANDLE_DRM pDrm,          /*!< pointer to drm crc info stucture */
    HANDLE_FDK_BITSTREAM hBs, /*!< handle to current bit buffer structure */
    int32_t reg                   /*!< crc region */
) {
  assert(pDrm != NULL);

  FDKcrcEndReg(&pDrm->crcInfo, hBs, reg);
}

TRANSPORTDEC_ERROR drmRead_CrcCheck(HANDLE_DRM pDrm) {
  TRANSPORTDEC_ERROR ErrorStatus = TRANSPORTDEC_OK;
  uint16_t crc;

  crc = FDKcrcGetCRC(&pDrm->crcInfo) ^ 0xFF;
  if (crc != pDrm->crcReadValue) {
    return (TRANSPORTDEC_CRC_ERROR);
  }

  return (ErrorStatus);
}
