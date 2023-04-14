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

   Description: individual channel stream info

*******************************************************************************/

#ifndef CHANNELINFO_H
#define CHANNELINFO_H

#include "../libFDK/common_fix.h"

#include "aac_rom.h"
#include "aacdecoder_lib.h"
#include "../libFDK/FDK_bitstream.h"
#include "overlapadd.h"

#include "../libFDK/mdct.h"
#include "stereo.h"
#include "pulsedata.h"
#include "aacdec_tns.h"

#include "aacdec_pns.h"

#include "aacdec_hcr_types.h"
#include "rvlc_info.h"

#include "usacdec_acelp.h"
#include "usacdec_const.h"
#include "usacdec_rom.h"

#include "../libArithCoding/ac_arith_coder.h"

#include "conceal_types.h"

#include "aacdec_drc_types.h"

#define WB_SECTION_SIZE (1024 * 2)

#define DRM_BS_BUFFER_SIZE                                                 \
  (512) /* size of the dynamic buffer which is used to reverse the bits of \
           the DRM SBR payload */

/* Output rendering mode */
typedef enum {
  AACDEC_RENDER_INVALID = 0,
  AACDEC_RENDER_IMDCT,
  AACDEC_RENDER_ELDFB,
  AACDEC_RENDER_LPD,
  AACDEC_RENDER_INTIMDCT
} AACDEC_RENDER_MODE;

enum { MAX_QUANTIZED_VALUE = 8191 };

typedef enum { FD_LONG, FD_SHORT, LPD } USAC_COREMODE;

typedef struct {
  const int16_t *ScaleFactorBands_Long;
  const int16_t *ScaleFactorBands_Short;
  uint8_t NumberOfScaleFactorBands_Long;
  uint8_t NumberOfScaleFactorBands_Short;
  uint32_t samplingRateIndex;
  uint32_t samplingRate;
} SamplingRateInfo;

typedef struct {
  uint8_t CommonWindow;
  uint8_t GlobalGain;

} CRawDataInfo;

typedef struct {
  uint8_t WindowGroupLength[8];
  uint8_t WindowGroups;
  uint8_t Valid;

  uint8_t WindowShape;         /* 0: sine window, 1: KBD, 2: low overlap */
  BLOCK_TYPE WindowSequence; /* mdct.h; 0: int32_t, 1: start, 2: int16_t, 3: stop */
  uint8_t MaxSfBands;
  uint8_t max_sfb_ste;
  uint8_t ScaleFactorGrouping;

  uint8_t TotalSfBands;

} CIcsInfo;

enum {
  ZERO_HCB = 0,
  ESCBOOK = 11,
  NSPECBOOKS = ESCBOOK + 1,
  BOOKSCL = NSPECBOOKS,
  NOISE_HCB = 13,
  INTENSITY_HCB2 = 14,
  INTENSITY_HCB = 15,
  LAST_HCB
};

/* This struct holds the persistent data shared by both channels of a CPE.
   It needs to be allocated for each CPE. */
typedef struct {
  CJointStereoPersistentData jointStereoPersistentData;
} CpePersistentData;

/*
 * This struct must be allocated one for every channel and must be persistent.
 */
typedef struct {
  int32_t *pOverlapBuffer;
  mdct_t IMdct;

  CArcoData *hArCo;

  int32_t pnsCurrentSeed;

  /* LPD memory */
  int32_t old_synth[PIT_MAX_MAX - L_SUBFR];
  int32_t old_T_pf[SYN_SFD];
  int32_t old_gain_pf[SYN_SFD];
  int32_t mem_bpf[L_FILT + L_SUBFR];
  uint8_t
  old_bpf_control_info; /* (1: enable, 0: disable) bpf for past superframe
                         */

  USAC_COREMODE last_core_mode; /* core mode used by the decoder in previous
                                   frame. (not signalled by the bitstream, see
                                   CAacDecoderChannelInfo::core_mode_last !! )
                                 */
  uint8_t last_lpd_mode;      /* LPD mode used by the decoder in last LPD subframe
                                (not signalled by the bitstream, see
                               CAacDecoderChannelInfo::lpd_mode_last !! ) */
  uint8_t last_last_lpd_mode; /* LPD mode used in second last LPD subframe
                                (not signalled by the bitstream) */
  uint8_t last_lpc_lost;      /* Flag indicating that the previous LPC is lost */

  FIXP_LPC
  lpc4_lsf[M_LP_FILTER_ORDER]; /* Last LPC4 coefficients in LSF domain. */
  FIXP_LPC lsf_adaptive_mean[M_LP_FILTER_ORDER]; /* Adaptive mean of LPC
                                                    coefficients in LSF domain
                                                    for concealment. */
  FIXP_LPC lp_coeff_old[2][M_LP_FILTER_ORDER];   /* Last LPC coefficients in LP
                                    domain. lp_coeff_old[0] is lpc4 (coeffs for
                                    right folding point of last tcx frame),
                                    lp_coeff_old[1] are coeffs for left folding
                                    point of last tcx frame */
  int32_t lp_coeff_old_exp[2];

  int16_t
  oldStability; /* LPC coeff stability value from last frame (required for
                   TCX concealment). */
  uint32_t numLostLpdFrames; /* Number of consecutive lost subframes. */

  /* TCX memory */
  int32_t last_tcx_gain;
  int32_t last_tcx_gain_e;
  int32_t last_alfd_gains[32]; /* Scaled by one bit. */
  int16_t last_tcx_pitch;
  uint8_t last_tcx_noise_factor;

  /* ACELP memory */
  CAcelpStaticMem acelp;

  uint32_t nfRandomSeed; /* seed value for USAC noise filling random generator */

  CDrcChannelData drcData;
  CConcealmentInfo concealmentInfo;

  CpePersistentData *pCpeStaticData;

} CAacDecoderStaticChannelInfo;

/*
 * This union must be allocated for every element (up to 2 channels).
 */
typedef struct {
  /* Common bit stream data */
  int16_t aScaleFactor[(
      8 * 16)]; /* Spectral scale factors for each sfb in each window. */
  int16_t aSfbScale[(8 * 16)]; /* could be free after ApplyTools() */
  uint8_t
  aCodeBook[(8 * 16)]; /* section data: codebook for each window and sfb. */
  uint8_t band_is_noise[(8 * 16)];
  CTnsData TnsData;
  CRawDataInfo RawDataInfo;

  shouldBeUnion {
    struct {
      CPulseData PulseData;
      int16_t aNumLineInSec4Hcr[MAX_SFB_HCR]; /* needed once for all channels
                                               except for Drm syntax */
      uint8_t
      aCodeBooks4Hcr[MAX_SFB_HCR]; /* needed once for all channels except for
                                      Drm syntax. Same as "aCodeBook" ? */
      int16_t lenOfReorderedSpectralData;
      int8_t lenOfLongestCodeword;
      int8_t numberSection;
      int8_t rvlcCurrentScaleFactorOK;
      int8_t rvlcIntensityUsed;
    } aac;
    struct {
      uint8_t fd_noise_level_and_offset;
      uint8_t tns_active;
      uint8_t tns_on_lr;
      uint8_t tcx_noise_factor[4];
      uint8_t tcx_global_gain[4];
    } usac;
  }
  specificTo;

} CAacDecoderDynamicData;

typedef shouldBeUnion {
  uint8_t DrmBsBuffer[DRM_BS_BUFFER_SIZE];

  /* Common signal data, can be used once the bit stream data from above is not
   * used anymore. */
  int32_t mdctOutTemp[1024];

  int32_t synth_buf[(PIT_MAX_MAX + SYN_DELAY + L_FRAME_PLUS)];

  int32_t workBuffer[WB_SECTION_SIZE];
}
CWorkBufferCore1;

/* Common data referenced by all channels */
typedef struct {
  CAacDecoderDynamicData pAacDecoderDynamicData[2];

  CPnsInterChannelData pnsInterChannelData;
  int32_t pnsRandomSeed[(8 * 16)];

  CJointStereoData jointStereoData; /* One for one element */

  shouldBeUnion {
    struct {
      CErHcrInfo erHcrInfo;
      CErRvlcInfo erRvlcInfo;
      int16_t aRvlcScfEsc[RVLC_MAX_SFB]; /* needed once for all channels */
      int16_t aRvlcScfFwd[RVLC_MAX_SFB]; /* needed once for all channels */
      int16_t aRvlcScfBwd[RVLC_MAX_SFB]; /* needed once for all channels */
    } aac;
  }
  overlay;

} CAacDecoderCommonData;

typedef struct {
  CWorkBufferCore1 *pWorkBufferCore1;
  CCplxPredictionData *cplxPredictionData;
} CAacDecoderCommonStaticData;

/*
 * This struct must be allocated one for every channel of every element and must
 * be persistent. Among its members, the following memory areas can be
 * overwritten under the given conditions:
 *  - pSpectralCoefficient The memory pointed to can be overwritten after time
 * signal rendering.
 *  - data can be overwritten after time signal rendering.
 *  - pDynData memory pointed to can be overwritten after each
 * CChannelElement_Decode() call.
 *  - pComData->overlay memory pointed to can be overwritten after each
 * CChannelElement_Decode() call..
 */
typedef struct {
  shouldBeUnion {
    struct {
      int32_t fac_data0[LFAC];
      int8_t fac_data_e[4];
      int32_t
      *fac_data[4]; /* Pointers to unused parts of pSpectralCoefficient */

      uint8_t core_mode; /* current core mode */
      USAC_COREMODE
      core_mode_last;      /* previous core mode, signalled in the bitstream
                              (not done by the decoder, see
                              CAacDecoderStaticChannelInfo::last_core_mode !!)*/
      uint8_t lpd_mode_last; /* previous LPD mode, signalled in the bitstream
                              (not done by the decoder, see
                              CAacDecoderStaticChannelInfo::last_core_mode !!)*/
      uint8_t mod[4];
      uint8_t bpf_control_info; /* (1: enable, 0: disable) bpf for current
                                 superframe */

      FIXP_LPC lsp_coeff[5][M_LP_FILTER_ORDER]; /* linear prediction
                                                   coefficients in LSP domain */
      FIXP_LPC
      lp_coeff[5][M_LP_FILTER_ORDER]; /* linear prediction coefficients in
                                         LP domain */
      int32_t lp_coeff_exp[5];
      FIXP_LPC lsf_adaptive_mean_cand
          [M_LP_FILTER_ORDER]; /* concealment: is copied to
                  CAacDecoderStaticChannelInfo->lsf_adaptive_mean once frame is
                  assumed to be correct*/
      int16_t aStability[4];  /* LPC coeff stability values required for ACELP
                                  and TCX (concealment) */

      CAcelpChannelData acelp[4];

      int32_t tcx_gain[4];
      int8_t tcx_gain_e[4];
    } usac;

    struct {
      CPnsData PnsData; /* Not required for USAC */
    } aac;
  }
  data;

  SPECTRAL_PTR pSpectralCoefficient; /* Spectral coefficients of each window */
  int16_t specScale[8]; /* Scale shift values of each spectrum window */
  CIcsInfo icsInfo;
  int32_t granuleLength; /* Size of smallest spectrum piece */
  uint8_t ElementInstanceTag;

  AACDEC_RENDER_MODE renderMode; /* Output signal rendering mode */

  CAacDecoderDynamicData *
      pDynData; /* Data required for one element and discarded after decoding */
  CAacDecoderCommonData
      *pComData; /* Data required for one channel at a time during decode */
  CAacDecoderCommonStaticData *pComStaticData; /* Persistent data required for
                                                  one channel at a time during
                                                  decode */

  int32_t currAliasingSymmetry; /* required for RSVD60 MCT */

} CAacDecoderChannelInfo;

/* channelinfo.cpp */

AAC_DECODER_ERROR getSamplingRateInfo(SamplingRateInfo *t, uint32_t samplesPerFrame,
                                      uint32_t samplingRateIndex,
                                      uint32_t samplingRate);

/**
 * \brief Read max SFB from bit stream and assign TotalSfBands according
 *        to the window sequence and sample rate.
 * \param hBs bit stream handle as data source
 * \param pIcsInfo IcsInfo structure to read the window sequence and store
 * MaxSfBands and TotalSfBands
 * \param pSamplingRateInfo read only
 */
AAC_DECODER_ERROR IcsReadMaxSfb(HANDLE_FDK_BITSTREAM hBs, CIcsInfo *pIcsInfo,
                                const SamplingRateInfo *pSamplingRateInfo);

AAC_DECODER_ERROR IcsRead(HANDLE_FDK_BITSTREAM bs, CIcsInfo *pIcsInfo,
                          const SamplingRateInfo *SamplingRateInfoTable,
                          const uint32_t flags);

/* stereo.cpp, only called from this file */

/*!
  \brief Applies MS stereo.

  The function applies MS stereo.

  \param pAacDecoderChannelInfo aac channel info.
  \param pScaleFactorBandOffsets pointer to scalefactor band offsets.
  \param pWindowGroupLength pointer to window group length array.
  \param windowGroups number of window groups.
  \param scaleFactorBandsTransmittedL number of transmitted scalefactor bands in
  left channel. \param scaleFactorBandsTransmittedR number of transmitted
  scalefactor bands in right channel. May differ from
  scaleFactorBandsTransmittedL only for USAC. \return  none
*/
void CJointStereo_ApplyMS(
    CAacDecoderChannelInfo *pAacDecoderChannelInfo[2],
    CAacDecoderStaticChannelInfo *pAacDecoderStaticChannelInfo[2],
    int32_t *spectrumL, int32_t *spectrumR, int16_t *SFBleftScale,
    int16_t *SFBrightScale, int16_t *specScaleL, int16_t *specScaleR,
    const int16_t *pScaleFactorBandOffsets, const uint8_t *pWindowGroupLength,
    const int32_t windowGroups, const int32_t max_sfb_ste_outside,
    const int32_t scaleFactorBandsTransmittedL,
    const int32_t scaleFactorBandsTransmittedR, int32_t *store_dmx_re_prev,
    int16_t *store_dmx_re_prev_e, const int32_t mainband_flag);

/*!
  \brief Applies intensity stereo

  The function applies intensity stereo.

  \param pAacDecoderChannelInfo aac channel info.
  \param pScaleFactorBandOffsets pointer to scalefactor band offsets.
  \param pWindowGroupLength pointer to window group length array.
  \param windowGroups number of window groups.
  \param scaleFactorBandsTransmitted number of transmitted scalefactor bands.
  \return  none
*/
void CJointStereo_ApplyIS(CAacDecoderChannelInfo *pAacDecoderChannelInfo[2],
                          const int16_t *pScaleFactorBandOffsets,
                          const uint8_t *pWindowGroupLength,
                          const int32_t windowGroups,
                          const int32_t scaleFactorBandsTransmitted);

/* aacdec_pns.cpp */
int32_t CPns_IsPnsUsed(const CPnsData *pPnsData, const int32_t group, const int32_t band);

void CPns_SetCorrelation(CPnsData *pPnsData, const int32_t group, const int32_t band,
                         const int32_t outofphase);

/****************** inline functions ******************/

inline uint8_t IsValid(const CIcsInfo *pIcsInfo) { return pIcsInfo->Valid; }

inline uint8_t IsLongBlock(const CIcsInfo *pIcsInfo) {
  return (pIcsInfo->WindowSequence != BLOCK_SHORT);
}

inline uint8_t GetWindowShape(const CIcsInfo *pIcsInfo) {
  return pIcsInfo->WindowShape;
}

inline BLOCK_TYPE GetWindowSequence(const CIcsInfo *pIcsInfo) {
  return pIcsInfo->WindowSequence;
}

inline const int16_t *GetScaleFactorBandOffsets(
    const CIcsInfo *pIcsInfo, const SamplingRateInfo *samplingRateInfo) {
  if (IsLongBlock(pIcsInfo)) {
    return samplingRateInfo->ScaleFactorBands_Long;
  } else {
    return samplingRateInfo->ScaleFactorBands_Short;
  }
}

inline uint8_t GetNumberOfScaleFactorBands(
    const CIcsInfo *pIcsInfo, const SamplingRateInfo *samplingRateInfo) {
  if (IsLongBlock(pIcsInfo)) {
    return samplingRateInfo->NumberOfScaleFactorBands_Long;
  } else {
    return samplingRateInfo->NumberOfScaleFactorBands_Short;
  }
}

inline int32_t GetWindowsPerFrame(const CIcsInfo *pIcsInfo) {
  return (pIcsInfo->WindowSequence == BLOCK_SHORT) ? 8 : 1;
}

inline uint8_t GetWindowGroups(const CIcsInfo *pIcsInfo) {
  return pIcsInfo->WindowGroups;
}

inline uint8_t GetWindowGroupLength(const CIcsInfo *pIcsInfo, const int32_t index) {
  return pIcsInfo->WindowGroupLength[index];
}

inline const uint8_t *GetWindowGroupLengthTable(const CIcsInfo *pIcsInfo) {
  return pIcsInfo->WindowGroupLength;
}

inline uint8_t GetScaleFactorBandsTransmitted(const CIcsInfo *pIcsInfo) {
  return pIcsInfo->MaxSfBands;
}

inline uint8_t GetScaleMaxFactorBandsTransmitted(const CIcsInfo *pIcsInfo0,
                                               const CIcsInfo *pIcsInfo1) {
  return fMax(pIcsInfo0->MaxSfBands, pIcsInfo1->MaxSfBands);
}

inline uint8_t GetScaleFactorBandsTotal(const CIcsInfo *pIcsInfo) {
  return pIcsInfo->TotalSfBands;
}

/* Note: This function applies to AAC-LC only ! */
inline uint8_t GetMaximumTnsBands(const CIcsInfo *pIcsInfo,
                                const int32_t samplingRateIndex) {
  return tns_max_bands_tbl[samplingRateIndex][!IsLongBlock(pIcsInfo)];
}

#endif /* #ifndef CHANNELINFO_H */
