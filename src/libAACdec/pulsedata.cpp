
#include "newAACDecoder.h"
#include "pulsedata.h"

#include "channelinfo.h"

int32_t CPulseData_Read(HANDLE_FDK_BITSTREAM bs, CPulseData *const PulseData,
                    const int16_t *sfb_startlines, const void *pIcsInfo,
                    const int16_t frame_length) {
  int32_t i, k = 0;
  const uint32_t MaxSfBands =
      GetScaleFactorBandsTransmitted((const CIcsInfo *)pIcsInfo);

  /* reset pulse data flag */
  PulseData->PulseDataPresent = 0;

  if ((PulseData->PulseDataPresent = (uint8_t)FDKreadBit(bs)) != 0) {
    if (!IsLongBlock((const CIcsInfo *)pIcsInfo)) {
      return AAC_DEC_DECODE_FRAME_ERROR;
    }

    PulseData->NumberPulse = (uint8_t)FDKreadBits(bs, 2);
    PulseData->PulseStartBand = (uint8_t)FDKreadBits(bs, 6);

    if (PulseData->PulseStartBand >= MaxSfBands) {
      return AAC_DEC_DECODE_FRAME_ERROR;
    }

    k = sfb_startlines[PulseData->PulseStartBand];

    for (i = 0; i <= PulseData->NumberPulse; i++) {
      PulseData->PulseOffset[i] = (uint8_t)FDKreadBits(bs, 5);
      PulseData->PulseAmp[i] = (uint8_t)FDKreadBits(bs, 4);
      k += PulseData->PulseOffset[i];
    }

    if (k >= frame_length) {
      return AAC_DEC_DECODE_FRAME_ERROR;
    }
  }

  return 0;
}

void CPulseData_Apply(
    CPulseData *PulseData, /*!< pointer to pulse data side info */
    const int16_t
        *pScaleFactorBandOffsets, /*!< pointer to scalefactor band offsets */
    int32_t *coef)               /*!< pointer to spectrum */
{
  int32_t i, k;

  if (PulseData->PulseDataPresent) {
    k = pScaleFactorBandOffsets[PulseData->PulseStartBand];

    for (i = 0; i <= PulseData->NumberPulse; i++) {
      k += PulseData->PulseOffset[i];
      if (coef[k] > (int32_t)0)
        coef[k] += (int32_t)(int32_t)PulseData->PulseAmp[i];
      else
        coef[k] -= (int32_t)(int32_t)PulseData->PulseAmp[i];
    }
  }
}
