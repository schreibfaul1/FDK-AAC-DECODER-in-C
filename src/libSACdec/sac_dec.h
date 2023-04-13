/* -----------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2019 Fraunhofer-Gesellschaft zur Förderung der angewandten
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

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: SAC Decoder Library structures

*******************************************************************************/

#ifndef SAC_DEC_H
#define SAC_DEC_H

#include "../libFDK/common_fix.h"

#include "sac_dec_interface.h"

#include "../libFDK/FDK_qmf_domain.h"
#include "sac_qmf.h"
#include "../libFDK/FDK_bitstream.h"
#include "sac_calcM1andM2.h"
#include "../libFDK/FDK_hybrid.h"
#include "../libFDK/FDK_decorrelate.h"
#include "sac_reshapeBBEnv.h"

#include "sac_dec_conceal.h"

#include "sac_tsd.h"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define ICCdefault 0
#define IPDdefault 0
#define arbdmxGainDefault 0
#define CPCdefault 10
#define tttCLD1default 15
#define tttCLD2default 0

#define IS_HQ_ONLY(aot)                                                      \
  ((aot) == AOT_ER_AAC_LD || (aot) == AOT_ER_AAC_ELD || (aot) == AOT_USAC || \
   (aot) == AOT_RSVD50)

#define SCONST(x) FL2FXCONST_DBL(x)

#define PC_NUM_BANDS (8)
#define PC_NUM_HYB_BANDS (PC_NUM_BANDS - 3 + 10)
#define ABS_THR (1e-9f * 32768 * 32768)

#define MAX_HYBRID_BANDS (MAX_NUM_QMF_BANDS - 3 + 10)
#define HYBRID_FILTER_DELAY (6)

#define MAX_RESIDUAL_FRAMES (4)
#define MAX_RESIDUAL_BISTREAM \
  (836) /*  48000 bps * 3 res / (8 * 44100 / 2048 ) */
#define MAX_MDCT_COEFFS (1024)
#define SACDEC_RESIDUAL_BS_BUF_SIZE \
  (1024) /* used to setup and check residual bitstream buffer */

#define MAX_NUM_PARAMS (MAX_NUM_OTT + 4 * MAX_NUM_TTT + MAX_INPUT_CHANNELS)
#define MAX_NUM_PARAMETERS (MAX(MAX_NUM_PARAMS, MAX_NUM_OTT))

#define MAX_PARAMETER_SETS (9)

#define MAX_M2_INPUT (MAX_OUTPUT_CHANNELS) /* 3 direct + 5 diffuse */

#define MAX_QMF_BANDS_TO_HYBRID \
  (3) /* 3 bands are filtered again in "40 bands" case */
#define PROTO_LEN (13)
#define BUFFER_LEN_LF (PROTO_LEN)
#define BUFFER_LEN_HF ((PROTO_LEN - 1) / 2)

#define MAX_NO_DECORR_CHANNELS (MAX_OUTPUT_CHANNELS)
#define HRTF_AZIMUTHS (5)

#define MAX_NUM_OTT_AT 0

/* left out */

typedef enum {
  UPMIXTYPE_BYPASS = -1, /*just bypass the input channels without processing*/
  UPMIXTYPE_NORMAL = 0   /*multichannel loudspeaker upmix with spatial data*/
} UPMIXTYPE;

static inline int32_t isTwoChMode(UPMIXTYPE upmixType) {
  int32_t retval = 0;
  return retval;
}

  /* left out end */

#define MPEGS_BYPASSMODE (0x00000001)
#define MPEGS_CONCEAL (0x00000002)

typedef struct STP_DEC *HANDLE_STP_DEC;

typedef struct {
  SCHAR bsQuantCoarseXXXprev;
  SCHAR bsQuantCoarseXXXprevParse;
} LOSSLESSSTATE;

typedef struct {
  SCHAR bsXXXDataMode[MAX_PARAMETER_SETS];
  SCHAR bsQuantCoarseXXX[MAX_PARAMETER_SETS];
  SCHAR bsFreqResStrideXXX[MAX_PARAMETER_SETS];
  SCHAR nocmpQuantCoarseXXX[MAX_PARAMETER_SETS];
  LOSSLESSSTATE *state; /* Link to persistent state information */
} LOSSLESSDATA;

struct SPATIAL_BS_FRAME_struct {
  UCHAR bsIndependencyFlag;
  UCHAR newBsData;
  UCHAR numParameterSets;

  /*
  If bsFramingType == 0, then the paramSlot[ps] for 0 <= ps < numParamSets is
  calculated as follows: paramSlot[ps] = ceil(numSlots*(ps+1)/numParamSets) - 1
  Otherwise, it is
    paramSlot[ps] = bsParamSlot[ps]
  */
  int32_t paramSlot[MAX_PARAMETER_SETS];

  /* These arrays contain the compact indices, only one value per pbstride, only
   * paramsets actually containing data. */
  /* These values are written from the parser in ecDataDec() and read during
   * decode in mapIndexData() */
  SCHAR cmpOttCLDidx[MAX_NUM_OTT + MAX_NUM_OTT_AT][MAX_PARAMETER_SETS]
                    [MAX_PARAMETER_BANDS];
  SCHAR cmpOttICCidx[MAX_NUM_OTT][MAX_PARAMETER_SETS][MAX_PARAMETER_BANDS];

  /* Smoothing */
  UCHAR bsSmoothMode[MAX_PARAMETER_SETS];
  UCHAR bsSmoothTime[MAX_PARAMETER_SETS];
  UCHAR bsFreqResStrideSmg[MAX_PARAMETER_SETS];
  UCHAR bsSmgData[MAX_PARAMETER_SETS]
                 [MAX_PARAMETER_BANDS]; /* smoothing flags, one if band is
                                           smoothed, otherwise zero */

  /* Arbitrary Downmix */
  SCHAR (*cmpArbdmxGainIdx)[MAX_PARAMETER_SETS][MAX_PARAMETER_BANDS];

  /* Lossless control */
  LOSSLESSDATA *CLDLosslessData;
  LOSSLESSDATA *ICCLosslessData;
  /* LOSSLESSDATA *ADGLosslessData; -> is stored in CLDLosslessData[offset] */

  LOSSLESSDATA *IPDLosslessData;
  SCHAR (*cmpOttIPDidx)[MAX_PARAMETER_SETS][MAX_PARAMETER_BANDS];
  int32_t phaseMode;
  int32_t OpdSmoothingMode;

  UCHAR tempShapeEnableChannelGES[MAX_OUTPUT_CHANNELS]; /*!< GES side info. */
  UCHAR bsEnvShapeData[MAX_OUTPUT_CHANNELS]
                      [MAX_TIME_SLOTS]; /*!< GES side info (quantized). */

  UCHAR tempShapeEnableChannelSTP[MAX_OUTPUT_CHANNELS]; /*!< STP side info. */

  TSD_DATA TsdData[1]; /*!< TSD data structure. */
};

typedef struct {
  /* Lossless state */
  LOSSLESSSTATE CLDLosslessState[MAX_NUM_PARAMETERS];
  LOSSLESSSTATE ICCLosslessState[MAX_NUM_PARAMETERS];
  LOSSLESSSTATE IPDLosslessState[MAX_NUM_PARAMETERS];
} BS_LL_STATE;

typedef struct {
  int32_t prevParamSlot;
  int32_t prevSmgTime;
  UCHAR prevSmgData[MAX_PARAMETER_BANDS];

  int32_t opdLeftState__FDK[MAX_PARAMETER_BANDS];
  int32_t opdRightState__FDK[MAX_PARAMETER_BANDS];

} SMOOTHING_STATE;

typedef struct {
  int32_t alpha__FDK;
  int32_t beta__FDK;
  int32_t partNrgPrev__FDK[2 * MAX_OUTPUT_CHANNELS + MAX_INPUT_CHANNELS]
                           [BB_ENV_SIZE];
  int32_t normNrgPrev__FDK[2 * MAX_OUTPUT_CHANNELS + MAX_INPUT_CHANNELS];
  int32_t frameNrgPrev__FDK[2 * MAX_OUTPUT_CHANNELS + MAX_INPUT_CHANNELS];
  int32_t partNrgPrevSF[2 * MAX_OUTPUT_CHANNELS + MAX_INPUT_CHANNELS];
  int32_t partNrgPrev2SF[2 * MAX_OUTPUT_CHANNELS + MAX_INPUT_CHANNELS];
  int32_t normNrgPrevSF[2 * MAX_OUTPUT_CHANNELS + MAX_INPUT_CHANNELS];
  int32_t frameNrgPrevSF[2 * MAX_OUTPUT_CHANNELS + MAX_INPUT_CHANNELS];
} RESHAPE_BBENV_STATE;

typedef struct {
  int32_t maxNumInputChannels;
  int32_t maxNumOutputChannels;
  int32_t maxNumQmfBands;
  int32_t maxNumHybridBands;
  int32_t maxNumXChannels;
  int32_t maxNumVChannels;
  int32_t maxNumDecorChannels;
  int32_t maxNumCmplxQmfBands;
  int32_t maxNumCmplxHybBands;
  int32_t maxNumResChannels;
  int32_t bProcResidual; /* process residual */
  int32_t maxNumResidualChannels;
  int32_t maxNumOttBoxes;
  int32_t maxNumParams;

} SACDEC_CREATION_PARAMS;

struct spatialDec_struct {
  SACDEC_ERROR
  errInt;             /* Field to store internal errors.
                         Will be clear at the very beginning of each process call. */
  int32_t staticDecScale; /* static scale of decoder */

  /* GENERAL */
  int32_t samplingFreq;       /* [Hz] */
  CFG_LEVEL decoderLevel; /* 0..5 */
  CFG_EXTENT decoderMode;
  CFG_BINAURAL binauralMode;

  SACDEC_CREATION_PARAMS createParams;

  int32_t numComplexProcessingBands;

  int32_t treeConfig; /* TREE_5151 = 5151, TREE_5152 = 5152, TREE_525 = 525, defined
                     in sac_bitdec.h */

  int32_t numInputChannels;  /* 1 (M) or 2 (L,R) */
  int32_t numOutputChannels; /* 6 for 3/2.1 (FL,FR,FC,LF,BL,BR) */
  int32_t numOttBoxes;       /* number of ott boxes */
  int32_t numM2rows;

  int32_t numOutputChannelsAT; /* Number of output channels after arbitrary tree
                              processing */

  int32_t quantMode; /* QUANT_FINE, QUANT_EBQ1, QUANT_EBQ2, defined in sac_bitdec.h
                  */
  int32_t arbitraryDownmix; /* (arbitraryDownmix != 0) 1 arbitrary downmix data
                           present, 2 arbitrary downmix residual data present*/
  int32_t residualCoding;   /* (residualCoding != 0) => residual coding data present
                         */
  UCHAR nrResidualFrame;
  UCHAR nrArbDownmixResidualFrame;
  FDK_BITSTREAM **hResidualBitstreams;
  int32_t tempShapeConfig; /* */
  int32_t decorrType;      /* Indicates to use PS or none PS decorrelator. */
  int32_t decorrConfig;    /* chosen decorrelator */
  int32_t envQuantMode;    /* quantization mode of envelope reshaping data */

  int32_t clipProtectGain__FDK; /* global gain for upmix */
  char clipProtectGainSF__FDK;   /* global gain for upmix */

  /* Currently ignoring center decorr
     numVChannels = numDirektSignals + numDecorSignals */
  int32_t numDirektSignals;  /* needed for W, Number of direkt signals 515 -> 1 525
                            -> 3 */
  int32_t wStartResidualIdx; /* Where to start read residuals for W, = 0 for 515, =
                            1 for 525 since one residual is used in V */
  int32_t numDecorSignals;   /* needed for W, Number of residual and decorrelated
                            signals, = 2, 3 for center deccorelation*/
  int32_t numVChannels;      /* direct signals + decorelator signals */
  int32_t numXChannels;      /* direct input signals + TTT-residuals */

  int32_t timeSlots;    /* length of spatial frame in QMF samples */
  int32_t curTimeSlot;  /* pointer to the current time slot used for hyperframing */
  int32_t prevTimeSlot; /*  */
  int32_t curPs;
  int32_t frameLength; /* number of output waveform samples/channel/frame */
  UPMIXTYPE upmixType;
  int32_t partiallyComplex;
  int32_t useFDreverb;

  int32_t bShareDelayWithSBR;

  int32_t tp_hybBandBorder; /* Hybrid band indicating the HP filter cut-off. */

  /* FREQUENCY MAPPING */
  int32_t qmfBands;
  int32_t hybridBands;
  const SCHAR *kernels; /* Mapping hybrid band to parameter band. */

  int32_t TsdTs; /**< TSD QMF slot counter 0<= ts < numSlots */

  int32_t *param2hyb; /* Mapping parameter bands to hybrid bands */
  int32_t kernels_width[MAX_PARAMETER_BANDS]; /* Mapping parmeter band to hybrid
                                             band offsets. */

  /* Residual coding */
  int32_t residualSamplingFreq;
  UCHAR residualPresent[MAX_NUM_OTT + MAX_NUM_TTT];
  UCHAR residualBands[MAX_NUM_OTT + MAX_NUM_TTT];    /* 0, if no residual data
                                                        present for this box */
  UCHAR residualQMFBands[MAX_NUM_OTT + MAX_NUM_TTT]; /* needed for optimized
                                                        mdct2qmf calculation */
  SPATIAL_SPECIFIC_CONFIG *pConfigCurrent;

  int32_t arbdmxFramesPerSpatialFrame;
  int32_t arbdmxUpdQMF;

  int32_t numParameterBands; /* Number of parameter bands 40, 28, 20, 14, 10, ...
                            .*/
  int32_t bitstreamParameterBands;
  int32_t *numOttBands; /* number of bands for each ott, is != numParameterBands for
                       LFEs */

  /* 1 MAPPING */
  UCHAR extendFrame;
  UCHAR numParameterSetsPrev;

  int32_t *smgTime;
  UCHAR **smgData;

  /* PARAMETER DATA decoded and dequantized */

  /* Last parameters from prev frame required during decode in mapIndexData()
   * and not touched during parse */
  SCHAR **ottCLDidxPrev;
  SCHAR **ottICCidxPrev;
  SCHAR **arbdmxGainIdxPrev;
  SCHAR **ottIPDidxPrev;
  SCHAR ***outIdxData; /* is this really persistent memory ? */

  /* State mem required during parse in SpatialDecParseFrameData() */
  SCHAR **cmpOttCLDidxPrev;
  SCHAR **cmpOttICCidxPrev;
  SCHAR ***ottICCdiffidx;
  SCHAR **cmpOttIPDidxPrev;

  /* State mem required in parseArbitraryDownmixData */
  SCHAR **cmpArbdmxGainIdxPrev;

  SCHAR ***ottCLD__FDK;
  SCHAR ***ottICC__FDK;

  SCHAR ***arbdmxGain__FDK; /* Holds the artistic downmix correction index.*/

  int32_t *arbdmxAlpha__FDK;
  int32_t *arbdmxAlphaPrev__FDK;

  UCHAR stereoConfigIndex;
  int32_t highRateMode;

  int32_t phaseCoding;

  SCHAR ***ottIPD__FDK;

  int32_t PhaseLeft__FDK[MAX_PARAMETER_BANDS];
  int32_t PhaseRight__FDK[MAX_PARAMETER_BANDS];
  int32_t PhasePrevLeft__FDK[MAX_PARAMETER_BANDS];
  int32_t PhasePrevRight__FDK[MAX_PARAMETER_BANDS];
  int32_t numOttBandsIPD;

  /* GAIN MATRICIES FOR CURRENT and PREVIOUS PARMATER SET(s)*/
  int32_t ***M2Real__FDK;
  int32_t ***M2Imag__FDK;
  int32_t ***M2RealPrev__FDK;
  int32_t ***M2ImagPrev__FDK;

  /* INPUT SIGNALS */
  int32_t ***qmfInputRealDelayBuffer__FDK;
  int32_t ***qmfInputImagDelayBuffer__FDK;

  int32_t pc_filterdelay; /* additional delay to align HQ with LP before hybird
                         analysis */
  int32_t qmfInputDelayBufPos;
  int32_t **qmfInputReal__FDK;
  int32_t **qmfInputImag__FDK;

  int32_t **hybInputReal__FDK;
  int32_t **hybInputImag__FDK;

  int32_t **binInputReverb;

  int32_t binGain, reverbGain;
  int32_t binCenterGain, reverbCenterGain;

  /* RESIDUAL SIGNALS */

  int32_t ***qmfResidualReal__FDK;
  int32_t ***qmfResidualImag__FDK;

  int32_t **hybResidualReal__FDK;
  int32_t **hybResidualImag__FDK;

  int32_t qmfOutputRealDryDelayBufPos;
  int32_t ***qmfOutputRealDryDelayBuffer__FDK;
  int32_t ***qmfOutputImagDryFilterBuffer__FDK;
  int32_t *qmfOutputImagDryFilterBufferBase__FDK;

  /* TEMPORARY SIGNALS */

  int32_t **wReal__FDK;
  int32_t **wImag__FDK;

  /* OUTPUT SIGNALS */
  int32_t **hybOutputRealDry__FDK;
  int32_t **hybOutputImagDry__FDK;
  int32_t **hybOutputRealWet__FDK;
  int32_t **hybOutputImagWet__FDK;
  PCM_MPS *timeOut__FDK;

  HANDLE_FDK_QMF_DOMAIN pQmfDomain;

  FDK_ANA_HYB_FILTER
  *hybridAnalysis; /*!< pointer Analysis hybrid filterbank array. */
  FDK_SYN_HYB_FILTER
  *hybridSynthesis; /*!< pointer Synthesis hybrid filterbank array. */
  int32_t **
      pHybridAnaStatesLFdmx; /*!< pointer to analysis hybrid filter states LF */
  int32_t **
      pHybridAnaStatesHFdmx; /*!< pointer to analysis hybrid filter states HF */
  int32_t **
      pHybridAnaStatesLFres; /*!< pointer to analysis hybrid filter states LF */
  int32_t **
      pHybridAnaStatesHFres; /*!< pointer to analysis hybrid filter states HF */

  DECORR_DEC *apDecor; /*!< pointer decorrelator array. */
  int32_t **pDecorBufferCplx;

  SMOOTHING_STATE *smoothState; /*!< Pointer to smoothing states. */

  RESHAPE_BBENV_STATE *reshapeBBEnvState; /*!< GES handle. */
  SCHAR row2channelDmxGES[MAX_OUTPUT_CHANNELS];

  HANDLE_STP_DEC hStpDec; /*!< STP handle. */

  const UCHAR *pActivM2ParamBands;

  int32_t bOverwriteM1M2prev; /* Overwrite previous M2/M2 params with first set of
                             new frame after SSC change (aka
                             decodeAfterConfigHasChangedFlag). */
  SpatialDecConcealmentInfo concealInfo;

  int32_t sacInDataHeadroom; /* Headroom of the SAC input time signal to prevent
                            clipping */
};

#define SACDEC_SYNTAX_MPS 1
#define SACDEC_SYNTAX_USAC 2
#define SACDEC_SYNTAX_RSVD50 4
#define SACDEC_SYNTAX_L2 8
#define SACDEC_SYNTAX_L3 16
#define SACDEC_SYNTAX_LD 32

static inline int32_t GetProcBand(spatialDec_struct *self, int32_t qs) {
  return self->kernels[qs];
}

#endif /* SAC_DEC_H */
