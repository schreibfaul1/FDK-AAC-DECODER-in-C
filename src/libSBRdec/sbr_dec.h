

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  Sbr decoder
*/
#ifndef SBR_DEC_H
#define SBR_DEC_H

#include "../libAACdec/newAACDecoder.h"
#include "sbrdecoder.h"
#include "lpp_tran.h"
#include "../libFDK/qmf.h"
#include "env_calc.h"
#include "sbrdec_drc.h"
#include "pvc_dec.h"
#include "hbe.h"

enum SBRDEC_QMF_SKIP {
  qmfSkipNothing = 0,
  qmfSkipAnalysis = 1 << 0,
  qmfSkipSynthesis = 1 << 1
};

typedef struct {
  SBR_CALCULATE_ENVELOPE SbrCalculateEnvelope;
  SBR_LPP_TRANS LppTrans;
  PVC_STATIC_DATA PvcStaticData;

  /* do scale handling in sbr an not in qmf */
  int16_t scale_ov;
  int16_t scale_lb;
  int16_t scale_hbe;

  int16_t prev_frame_lSbr;
  int16_t prev_frame_hbeSbr;

  int32_t codecFrameSize;

  HANDLE_HBE_TRANSPOSER hHBE;

  HANDLE_FDK_QMF_DOMAIN_IN qmfDomainInCh;
  HANDLE_FDK_QMF_DOMAIN_OUT qmfDomainOutCh;

  SBRDEC_DRC_CHANNEL sbrDrcChannel;

#if (SBRDEC_MAX_HB_FADE_FRAMES > 0)
  int32_t highBandFadeCnt; /* counter for fading in high-band signal smoothly */

#endif
  int32_t **tmp_memory; /* shared memory between hbeLightTimeDelayBuffer and
                            hQmfHBESlotsReal */

  int32_t **hQmfHBESlotsReal;
  int32_t **hQmfHBESlotsImag;

  int32_t **codecQMFBufferReal;
  int32_t **codecQMFBufferImag;
  uint8_t savedStates;
  int32_t applySbrProc_old;
} SBR_DEC;

typedef SBR_DEC *HANDLE_SBR_DEC;

typedef struct {
  SBR_FRAME_DATA frameData[(1) + 1];
  SBR_PREV_FRAME_DATA prevFrameData;
  SBR_DEC SbrDec;
} SBR_CHANNEL;

typedef SBR_CHANNEL *HANDLE_SBR_CHANNEL;

void sbr_dec(
    HANDLE_SBR_DEC hSbrDec,             /*!< handle to Decoder channel */
    int32_t *timeIn,                       /*!< pointer to input time signal */
    int32_t *timeOut,                      /*!< pointer to output time signal */
    HANDLE_SBR_DEC hSbrDecRight,        /*!< handle to Decoder channel right */
    int32_t *timeOutRight,                 /*!< pointer to output time signal */
    int32_t strideOut,                      /*!< Time data traversal strideOut */
    HANDLE_SBR_HEADER_DATA hHeaderData, /*!< Static control data */
    HANDLE_SBR_FRAME_DATA hFrameData,   /*!< Control data of current frame */
    HANDLE_SBR_PREV_FRAME_DATA
        hPrevFrameData,        /*!< Some control data of last frame */
    const int32_t applyProcessing, /*!< Flag for SBR operation */
    HANDLE_PS_DEC h_ps_d, const uint32_t flags, const int32_t codecFrameSize,
    const int32_t sbrInDataHeadroom);

SBR_ERROR
createSbrDec(SBR_CHANNEL *hSbrChannel, HANDLE_SBR_HEADER_DATA hHeaderData,
             TRANSPOSER_SETTINGS *pSettings, const int32_t downsampleFac,
             const uint32_t qmfFlags, const uint32_t flags, const int32_t overlap, int32_t chan,
             int32_t codecFrameSize);

int32_t deleteSbrDec(SBR_CHANNEL *hSbrChannel);

SBR_ERROR
resetSbrDec(HANDLE_SBR_DEC hSbrDec, HANDLE_SBR_HEADER_DATA hHeaderData,
            HANDLE_SBR_PREV_FRAME_DATA hPrevFrameData, const int32_t downsampleFac,
            const uint32_t flags, HANDLE_SBR_FRAME_DATA hFrameData);

#endif
