

/******************* Library for basic calculation routines ********************

   Author(s):

   Description: CRC calculation

*******************************************************************************/

#ifndef FDK_CRC_H
#define FDK_CRC_H
#include "../libAACdec/newAACDecoder.h"
#include "FDK_bitstream.h"

#define MAX_CRC_REGS                                                           \
  3 /*!< Maximal number of overlapping crc region in ADTS channel pair element \
       is two. Select three independent regions preventively. */

/**
 *  This structure describes single crc region used for crc calculation.
 */
typedef struct {
  uint8_t isActive;
  int32_t maxBits;
  int32_t bitBufCntBits;
  int32_t validBits;

} CCrcRegData;

/**
 *  CRC info structure.
 */
typedef struct {
  CCrcRegData crcRegData[MAX_CRC_REGS]; /*!< Multiple crc region description. */
  const uint16_t*
      pCrcLookup; /*!< Pointer to lookup table filled in FDK_crcInit(). */

  uint16_t crcPoly;    /*!< CRC generator polynom. */
  uint16_t crcMask;    /*!< CRC mask. */
  uint16_t startValue; /*!< CRC start value. */
  uint8_t crcLen;      /*!< CRC length. */

  uint32_t regStart; /*!< Start region marker for synchronization. */
  uint32_t regStop;  /*!< Stop region marker for synchronization. */

  uint16_t crcValue; /*!< Crc value to be calculated. */

} FDK_CRCINFO;

/**
 *  CRC info handle.
 */
typedef FDK_CRCINFO* HANDLE_FDK_CRCINFO;

/**
 * \brief  Initialize CRC structure.
 *
 * The function initializes existing crc info structure with denoted
 * configuration.
 *
 * \param hCrcInfo              Pointer to an outlying allocated crc info
 * structure.
 * \param crcPoly               Configure crc polynom.
 * \param crcStartValue         Configure crc start value.
 * \param crcLen                Configure crc length.
 *
 * \return  none
 */
void FDKcrcInit(HANDLE_FDK_CRCINFO hCrcInfo, const uint32_t crcPoly,
                const uint32_t crcStartValue, const uint32_t crcLen);

/**
 * \brief  Reset CRC info structure.
 *
 * This function clears all intern states of the crc structure.
 *
 * \param hCrcInfo              Pointer to crc info stucture.
 *
 * \return  none
 */
void FDKcrcReset(HANDLE_FDK_CRCINFO hCrcInfo);

/**
 * \brief  Start CRC region with maximum number of bits.
 *
 * This function marks position in bitstream to be used as start point for crc
 * calculation. Bitstream range for crc calculation can be limited or kept
 * dynamic depending on mBits parameter. The crc region has to be terminated
 * with FDKcrcEndReg() in each case.
 *
 * \param hCrcInfo              Pointer to crc info stucture.
 * \param hBs                   Pointer to current bit buffer structure.
 * \param mBits                 Number of bits in crc region to be calculated.
 *                              - mBits > 0: Zero padding will be used for CRC
 * calculation, if there are less than mBits bits available.
 *                              - mBits < 0: No zero padding is done.
 *                              - mBits = 0: The number of bits used in crc
 * calculation is dynamically, depending on bitstream position between
 * FDKcrcStartReg() and FDKcrcEndReg()
 * call.
 *
 * \return  ID for the created region, -1 in case of an error
 */
int32_t FDKcrcStartReg(HANDLE_FDK_CRCINFO hCrcInfo, const HANDLE_FDK_BITSTREAM hBs,
                   const int32_t mBits);

/**
 * \brief  Ends CRC region.
 *
 * This function terminates crc region specified with FDKcrcStartReg(). The
 * number of bits in crc region depends on mBits parameter of FDKcrcStartReg().
 * This function calculates and updates crc in info structure.
 *
 * \param hCrcInfo              Pointer to crc info stucture.
 * \param hBs                   Pointer to current bit buffer structure.
 * \param reg                   Crc region ID created in FDKcrcStartReg().
 *
 * \return  0 on success
 */
int32_t FDKcrcEndReg(HANDLE_FDK_CRCINFO hCrcInfo, const HANDLE_FDK_BITSTREAM hBs,
                 const int32_t reg);

/**
 * \brief  This function returns crc value from info struct.
 *
 * \param hCrcInfo              Pointer to crc info stucture.
 *
 * \return  CRC value masked with crc length.
 */
uint16_t FDKcrcGetCRC(const HANDLE_FDK_CRCINFO hCrcInfo);

#endif /* FDK_CRC_H */
