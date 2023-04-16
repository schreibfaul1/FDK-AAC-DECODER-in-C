/* -----------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2018 Fraunhofer-Gesellschaft zur Förderung der angewandten
Forschung e.V. All rights reserved.

 1.    INTRODUCTION
The Fraunhofer FDK AAC Codec Library for Android ("FDK AAC Codec") is software
that implements the MPEG Advanced Audio Coding ("AAC") encoding and decoding
scheme for digital audio. This FDK AAC Codec software is intended to be used on
a wide variety of Android devices.

AAC's HE-AAC and HE-AAC v2 versions are regarded as today's most efficient
general perceptual audio codecs. AAC-ELD is considered the best-performing
full-bandwidth communications codec by independent studies and is widely
deployed. AAC has been standardized by ISO and IEC as part of the MPEG
specifications.

Patent licenses for necessary patent claims for the FDK AAC Codec (including
those of Fraunhofer) may be obtained through Via Licensing
(www.vialicensing.com) or through the respective patent owners individually for
the purpose of encoding or decoding bit streams in products that are compliant
with the ISO/IEC MPEG audio standards. Please note that most manufacturers of
Android devices already license these patent claims through Via Licensing or
directly from the patent owners, and therefore FDK AAC Codec software may
already be covered under those patent licenses when it is used for those
licensed purposes only.

Commercially-licensed AAC software libraries, including floating-point versions
with enhanced sound quality, are also available from Fraunhofer. Users are
encouraged to check the Fraunhofer website for additional applications
information and documentation.

2.    COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification,
are permitted without payment of copyright license fees provided that you
satisfy the following conditions:

You must retain the complete text of this software license in redistributions of
the FDK AAC Codec or your modifications thereto in source code form.

You must retain the complete text of this software license in the documentation
and/or other materials provided with redistributions of the FDK AAC Codec or
your modifications thereto in binary form. You must make available free of
charge copies of the complete source code of the FDK AAC Codec and your
modifications thereto to recipients of copies in binary form.

The name of Fraunhofer may not be used to endorse or promote products derived
from this library without prior written permission.

You may not charge copyright license fees for anyone to use, copy or distribute
the FDK AAC Codec software or your modifications thereto.

Your modified versions of the FDK AAC Codec must carry prominent notices stating
that you changed the software and the date of any change. For modified versions
of the FDK AAC Codec, the term "Fraunhofer FDK AAC Codec Library for Android"
must be replaced by the term "Third-Party Modified Version of the Fraunhofer FDK
AAC Codec Library for Android."

3.    NO PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without
limitation the patents of Fraunhofer, ARE GRANTED BY THIS SOFTWARE LICENSE.
Fraunhofer provides no warranty of patent non-infringement with respect to this
software.

You may use this FDK AAC Codec software or modifications thereto only for
purposes that are authorized by appropriate patent licenses.

4.    DISCLAIMER

This FDK AAC Codec software is provided by Fraunhofer on behalf of the copyright
holders and contributors "AS IS" and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
including but not limited to the implied warranties of merchantability and
fitness for a particular purpose. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE for any direct, indirect, incidental, special, exemplary,
or consequential damages, including but not limited to procurement of substitute
goods or services; loss of use, data, or profits, or business interruption,
however caused and on any theory of liability, whether in contract, strict
liability, or tort (including negligence), arising in any way out of the use of
this software, even if advised of the possibility of such damage.

5.    CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Audio and Multimedia Departments - FDK AAC LL
Am Wolfsmantel 33
91058 Erlangen, Germany

www.iis.fraunhofer.de/amm
amm-info@iis.fraunhofer.de
----------------------------------------------------------------------------- */

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: perceptual noise substitution tool

*******************************************************************************/

#include <memory.h>
#include <stdint.h>
#include "aacdec_pns.h"
#include "aac_rom.h"
#include "channelinfo.h"
#include "block.h"
#include "../libFDK/FDK_bitstream.h"

#include "../libSYS/genericStds.h"

#define NOISE_OFFSET 90 /* cf. ISO 14496-3 p. 175 */

/*!
  \brief Reset InterChannel and PNS data

  The function resets the InterChannel and PNS data
*/
void CPns_ResetData(CPnsData *pPnsData,
                    CPnsInterChannelData *pPnsInterChannelData) {
  assert(pPnsData != NULL);
  assert(pPnsInterChannelData != NULL);
  /* Assign pointer always, since pPnsData is not persistent data */
  pPnsData->pPnsInterChannelData = pPnsInterChannelData;
  pPnsData->PnsActive = 0;
  pPnsData->CurrentEnergy = 0;

  memset(pPnsData->pnsUsed ,0, (8 * 16) * sizeof(uint8_t) );
  memset(pPnsInterChannelData->correlated , 0, (8 * 16) * sizeof(uint8_t) );
}

/*!
  \brief Update PNS noise generator state.

  The function sets the seed for PNS noise generation.
  It can be used to link two or more channels in terms of PNS.
*/
void CPns_UpdateNoiseState(CPnsData *pPnsData, int32_t *currentSeed,
                           int32_t *randomSeed) {
  /* use pointer because seed has to be
     same, left and right channel ! */
  pPnsData->currentSeed = currentSeed;
  pPnsData->randomSeed = randomSeed;
}

/*!
  \brief Indicates if PNS is used

  The function returns a value indicating whether PNS is used or not
  acordding to the noise energy

  \return  PNS used
*/
int32_t CPns_IsPnsUsed(const CPnsData *pPnsData, const int32_t group, const int32_t band) {
  unsigned pns_band = group * 16 + band;

  return pPnsData->pnsUsed[pns_band] & (uint8_t)1;
}

/*!
  \brief Set correlation

  The function activates the noise correlation between the channel pair
*/
void CPns_SetCorrelation(CPnsData *pPnsData, const int32_t group, const int32_t band,
                         const int32_t outofphase) {
  CPnsInterChannelData *pInterChannelData = pPnsData->pPnsInterChannelData;
  unsigned pns_band = group * 16 + band;

  pInterChannelData->correlated[pns_band] = (outofphase) ? 3 : 1;
}

/*!
  \brief Indicates if correlation is used

  The function indicates if the noise correlation between the channel pair
  is activated

  \return  PNS is correlated
*/
static int32_t CPns_IsCorrelated(const CPnsData *pPnsData, const int32_t group,
                             const int32_t band) {
  CPnsInterChannelData *pInterChannelData = pPnsData->pPnsInterChannelData;
  unsigned pns_band = group * 16 + band;

  return (pInterChannelData->correlated[pns_band] & 0x01) ? 1 : 0;
}

/*!
  \brief Indicates if correlated out of phase mode is used.

  The function indicates if the noise correlation between the channel pair
  is activated in out-of-phase mode.

  \return  PNS is out-of-phase
*/
static int32_t CPns_IsOutOfPhase(const CPnsData *pPnsData, const int32_t group,
                             const int32_t band) {
  CPnsInterChannelData *pInterChannelData = pPnsData->pPnsInterChannelData;
  unsigned pns_band = group * 16 + band;

  return (pInterChannelData->correlated[pns_band] & 0x02) ? 1 : 0;
}

/*!
  \brief Read PNS information

  The function reads the PNS information from the bitstream
*/
void CPns_Read(CPnsData *pPnsData, HANDLE_FDK_BITSTREAM bs,
               const CodeBookDescription *hcb, int16_t *pScaleFactor,
               uint8_t global_gain, int32_t band, int32_t group /* = 0 */) {
  int32_t delta;
  uint32_t pns_band = group * 16 + band;

  if (pPnsData->PnsActive) {
    /* Next PNS band case */
    delta = CBlock_DecodeHuffmanWord(bs, hcb) - 60;
  } else {
    /* First PNS band case */
    int32_t noiseStartValue = FDKreadBits(bs, 9);

    delta = noiseStartValue - 256;
    pPnsData->PnsActive = 1;
    pPnsData->CurrentEnergy = global_gain - NOISE_OFFSET;
  }

  pPnsData->CurrentEnergy += delta;
  pScaleFactor[pns_band] = pPnsData->CurrentEnergy;

  pPnsData->pnsUsed[pns_band] = 1;
}

/**
 * \brief Generate a vector of noise of given length. The noise values are
 *        scaled in order to yield a noise energy of 1.0
 * \param spec pointer to were the noise values will be written to.
 * \param size amount of noise values to be generated.
 * \param pRandomState pointer to the state of the random generator being used.
 * \return exponent of generated noise vector.
 */
static int32_t GenerateRandomVector(int32_t * spec, int32_t size,
                                int32_t *pRandomState) {
  int32_t i, invNrg_e = 0, nrg_e = 0;
  int32_t invNrg_m, nrg_m = FL2FXCONST_DBL(0.0f);
  int32_t * ptr = spec;
  int32_t randomState = *pRandomState;

#define GEN_NOISE_NRG_SCALE 7

  /* Generate noise and calculate energy. */
  for (i = 0; i < size; i++) {
    randomState =
        (((int64_t)1664525 * randomState) + (int64_t)1013904223) & 0xFFFFFFFF;
    nrg_m = fPow2AddDiv2(nrg_m, (int32_t)randomState >> GEN_NOISE_NRG_SCALE);
    *ptr++ = (int32_t)randomState;
  }
  nrg_e = GEN_NOISE_NRG_SCALE * 2 + 1;

  /* weight noise with = 1 / sqrt_nrg; */
  invNrg_m = invSqrtNorm2(nrg_m << 1, &invNrg_e);
  invNrg_e += -((nrg_e - 1) >> 1);

  for (i = size; i--;) {
    spec[i] = fMult(spec[i], invNrg_m);
  }

  /* Store random state */
  *pRandomState = randomState;

  return invNrg_e;
}

static void ScaleBand(int32_t * spec, int32_t size, int32_t scaleFactor,
                      int32_t specScale, int32_t noise_e, int32_t out_of_phase) {
  int32_t i, shift, sfExponent;
  int32_t sfMatissa;

  /* Get gain from scale factor value = 2^(scaleFactor * 0.25) */
  sfMatissa = MantissaTable[scaleFactor & 0x03][0];
  /* sfExponent = (scaleFactor >> 2) + ExponentTable[scaleFactor & 0x03][0]; */
  /* Note:  ExponentTable[scaleFactor & 0x03][0] is always 1. */
  sfExponent = (scaleFactor >> 2) + 1;

  if (out_of_phase != 0) {
    sfMatissa = -sfMatissa;
  }

  /* +1 because of fMultDiv2 below. */
  shift = sfExponent - specScale + 1 + noise_e;

  /* Apply gain to noise values */
  if (shift >= 0) {
    shift = fixMin(shift, DFRACT_BITS - 1);
    for (i = size; i-- != 0;) {
      spec[i] = fMultDiv2(spec[i], sfMatissa) << shift;
    }
  } else {
    shift = fixMin(-shift, DFRACT_BITS - 1);
    for (i = size; i-- != 0;) {
      spec[i] = fMultDiv2(spec[i], sfMatissa) >> shift;
    }
  }
}

/*!
  \brief Apply PNS

  The function applies PNS (i.e. it generates noise) on the bands
  flagged as noisy bands

*/
void CPns_Apply(const CPnsData *pPnsData, const CIcsInfo *pIcsInfo,
                SPECTRAL_PTR pSpectrum, const int16_t *pSpecScale,
                const int16_t *pScaleFactor,
                const SamplingRateInfo *pSamplingRateInfo,
                const int32_t granuleLength, const int32_t channel) {
  if (pPnsData->PnsActive) {
    const int16_t *BandOffsets =
        GetScaleFactorBandOffsets(pIcsInfo, pSamplingRateInfo);

    int32_t ScaleFactorBandsTransmitted = GetScaleFactorBandsTransmitted(pIcsInfo);

    for (int32_t window = 0, group = 0; group < GetWindowGroups(pIcsInfo);
         group++) {
      for (int32_t groupwin = 0; groupwin < GetWindowGroupLength(pIcsInfo, group);
           groupwin++, window++) {
        int32_t *spectrum = SPEC(pSpectrum, window, granuleLength);

        for (int32_t band = 0; band < ScaleFactorBandsTransmitted; band++) {
          if (CPns_IsPnsUsed(pPnsData, group, band)) {
            uint32_t pns_band = window * 16 + band;

            int32_t bandWidth = BandOffsets[band + 1] - BandOffsets[band];
            int32_t noise_e;

            assert(bandWidth >= 0);

            if (channel > 0 && CPns_IsCorrelated(pPnsData, group, band)) {
              noise_e =
                  GenerateRandomVector(spectrum + BandOffsets[band], bandWidth,
                                       &pPnsData->randomSeed[pns_band]);
            } else {
              pPnsData->randomSeed[pns_band] = *pPnsData->currentSeed;

              noise_e = GenerateRandomVector(spectrum + BandOffsets[band],
                                             bandWidth, pPnsData->currentSeed);
            }

            int32_t outOfPhase = CPns_IsOutOfPhase(pPnsData, group, band);

            ScaleBand(spectrum + BandOffsets[band], bandWidth,
                      pScaleFactor[group * 16 + band], pSpecScale[window],
                      noise_e, outOfPhase);
          }
        }
      }
    }
  }
}
