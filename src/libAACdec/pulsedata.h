

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: pulse data tool

*******************************************************************************/

#ifndef PULSEDATA_H
#define PULSEDATA_H
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"
#include "../libFDK/FDK_bitstream.h"

#define N_MAX_LINES 4

typedef struct {
  uint8_t PulseDataPresent;
  uint8_t NumberPulse;
  uint8_t PulseStartBand;
  uint8_t PulseOffset[N_MAX_LINES];
  uint8_t PulseAmp[N_MAX_LINES];
} CPulseData;

/**
 * \brief Read pulse data from bitstream
 *
 * The function reads the elements for pulse data from
 * the bitstream.
 *
 * \param bs bit stream handle data source.
 * \param PulseData pointer to a CPulseData were the decoded data is stored
 * into.
 * \param MaxSfBands max number of scale factor bands.
 * \return 0 on success, != 0 on parse error.
 */
int32_t CPulseData_Read(const HANDLE_FDK_BITSTREAM bs, CPulseData *const PulseData,
                    const int16_t *sfb_startlines, const void *pIcsInfo,
                    const int16_t frame_length);

/**
 * \brief Apply pulse data to spectral lines
 *
 * The function applies the pulse data to the
 * specified spectral lines.
 *
 * \param PulseData pointer to the previously decoded pulse data.
 * \param pScaleFactorBandOffsets scale factor band line offset table.
 * \param coef pointer to the spectral data were pulse data should be applied
 * to.
 * \return  none
 */
void CPulseData_Apply(CPulseData *PulseData,
                      const int16_t *pScaleFactorBandOffsets, int32_t *coef);

#endif /* #ifndef PULSEDATA_H */
