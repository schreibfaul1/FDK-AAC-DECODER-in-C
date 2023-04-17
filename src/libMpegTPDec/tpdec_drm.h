

/******************* MPEG transport format decoder library *********************

   Author(s):   Josef Hoepfl

   Description: DRM interface

*******************************************************************************/

#ifndef TPDEC_DRM_H
#define TPDEC_DRM_H
#include "../libAACdec/newAACDecoder.h"
#include "tpdec_lib.h"

#include "../libFDK/FDK_crc.h"

typedef struct {
  FDK_CRCINFO crcInfo; /* CRC state info */
  uint16_t crcReadValue; /* CRC value read from bitstream data */

} STRUCT_DRM;

typedef STRUCT_DRM *HANDLE_DRM;

/*!
  \brief Initialize DRM CRC

  The function initialzes the crc buffer and the crc lookup table.

  \return  none
*/
void drmRead_CrcInit(HANDLE_DRM pDrm);

/**
 * \brief Starts CRC region with a maximum number of bits
 *        If mBits is positive zero padding will be used for CRC calculation, if
 * there are less than mBits bits available. If mBits is negative no zero
 * padding is done. If mBits is zero the memory for the buffer is
 * allocated dynamically, the number of bits is not limited.
 *
 * \param pDrm DRM data handle
 * \param hBs bitstream handle, on which the CRC region referes to
 * \param mBits max number of bits in crc region to be considered
 *
 * \return  ID for the created region, -1 in case of an error
 */
int32_t drmRead_CrcStartReg(HANDLE_DRM pDrm, HANDLE_FDK_BITSTREAM hBs, int32_t mBits);

/**
 * \brief Ends CRC region identified by reg
 *
 * \param pDrm DRM data handle
 * \param hBs bitstream handle, on which the CRC region referes to
 * \param reg CRC regions ID returned by drmRead_CrcStartReg()
 *
 * \return  none
 */
void drmRead_CrcEndReg(HANDLE_DRM pDrm, HANDLE_FDK_BITSTREAM hBs, int32_t reg);

/**
 * \brief Check CRC
 *
 * Checks if the currently calculated CRC matches the CRC field read from the
 * bitstream Deletes all CRC regions.
 *
 * \param pDrm DRM data handle
 *
 * \return Returns 0 if they are identical otherwise 1
 */
TRANSPORTDEC_ERROR drmRead_CrcCheck(HANDLE_DRM pDrm);

/**
 * \brief Check if we have a valid DRM frame at the current bitbuffer position
 *
 * This function assumes enough bits in buffer for the current frame.
 * It reads out the header bits to prepare the bitbuffer for the decode loop.
 * In case the header bits show an invalid bitstream/frame, the whole frame is
 * skipped.
 *
 * \param pDrm DRM data handle which is filled with parsed DRM header data
 * \param bs handle of bitstream from whom the DRM header is read
 *
 * \return  error status
 */
TRANSPORTDEC_ERROR drmRead_DecodeHeader(HANDLE_DRM pDrm,
                                        HANDLE_FDK_BITSTREAM bs);

/**
 * \brief   Parse a Drm specific SDC audio config from a given bitstream handle.
 *
 * \param pAsc                         A pointer to an allocated
 * CSAudioSpecificConfig struct.
 * \param hBs                          Bitstream handle.
 * \param cb                           A pointer to structure holding callback
 * information Note: A NULL pointer for cb can be used to signal a "Check Config
 * only functionality"
 * \param configMode                   Config modes: memory allocation mode or
 * config change detection mode
 * \param configChanged                Indicates a config change
 *
 * \return  Total element count including all SCE, CPE and LFE.
 */
TRANSPORTDEC_ERROR DrmRawSdcAudioConfig_Parse(CSAudioSpecificConfig *pAsc,
                                              HANDLE_FDK_BITSTREAM hBs,
                                              CSTpCallBacks *cb,
                                              const uint8_t configMode,
                                              const uint8_t configChanged);

#endif /* TPDEC_DRM_H */
