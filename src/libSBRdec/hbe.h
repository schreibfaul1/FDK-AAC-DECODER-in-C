

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

#ifndef HBE_H
#define HBE_H

#include "../libAACdec/newAACDecoder.h"
#include "sbrdecoder.h"

#ifndef QMF_SYNTH_CHANNELS
#define QMF_SYNTH_CHANNELS (64)
#endif

#define HBE_QMF_FILTER_STATE_ANA_SIZE (400)
#define HBE_QMF_FILTER_STATE_SYN_SIZE (200)

#ifndef MAX_NUM_PATCHES_HBE
#define MAX_NUM_PATCHES_HBE (6)
#endif
#define MAX_STRETCH_HBE (4)

typedef enum {
  KEEP_STATES_SYNCED_OFF = 0,    /*!< normal QMF transposer behaviour */
  KEEP_STATES_SYNCED_NORMAL = 1, /*!< QMF transposer called for syncing of
                                    states the last 8/14 slots are calculated in
                                    case next frame is HBE */
  KEEP_STATES_SYNCED_OUTDIFF =
      2, /*!< QMF transposer behaviour as in normal case, but the calculated
              slots are directly written to overlap area of buffer; only used in
              resetSbrDec function */
  KEEP_STATES_SYNCED_NOOUT =
      3 /*!< QMF transposer is called for syncing of states only, not output
             is generated at all; only used in resetSbrDec function */
} KEEP_STATES_SYNCED_MODE;

struct hbeTransposer {
  int32_t anaQmfStates[HBE_QMF_FILTER_STATE_ANA_SIZE];
  FIXP_QSS synQmfStates[HBE_QMF_FILTER_STATE_SYN_SIZE];

  int32_t xOverQmf[MAX_NUM_PATCHES_HBE];

  int32_t maxStretch;
  int32_t timeDomainWinLen;
  int32_t qmfInBufSize;
  int32_t qmfOutBufSize;
  int32_t noCols;
  int32_t noChannels;
  int32_t startBand;
  int32_t stopBand;
  int32_t bSbr41;

  int32_t *inBuf_F;
  int32_t **qmfInBufReal_F;
  int32_t **qmfInBufImag_F;

  int32_t *qmfBufferCodecTempSlot_F;

  QMF_FILTER_BANK HBEAnalysiscQMF;
  QMF_FILTER_BANK HBESynthesisQMF;

  int32_t const *synthesisQmfPreModCos_F;
  int32_t const *synthesisQmfPreModSin_F;

  int32_t **qmfHBEBufReal_F;
  int32_t **qmfHBEBufImag_F;

  int32_t bXProducts[MAX_STRETCH_HBE];

  int32_t kstart;
  int32_t synthSize;

  int32_t highband_exp[2];
  int32_t target_exp[2];
};

typedef struct hbeTransposer *HANDLE_HBE_TRANSPOSER;

SBR_ERROR QmfTransposerCreate(HANDLE_HBE_TRANSPOSER *hQmfTransposer,
                              const int32_t frameSize, int32_t bDisableCrossProducts,
                              int32_t bSbr41);

SBR_ERROR QmfTransposerReInit(HANDLE_HBE_TRANSPOSER hQmfTransposer,
                              uint8_t *FreqBandTable[2], uint8_t NSfb[2]);

void QmfTransposerClose(HANDLE_HBE_TRANSPOSER hQmfTransposer);

void QmfTransposerApply(HANDLE_HBE_TRANSPOSER hQmfTransposer,
                        int32_t **qmfBufferCodecReal,
                        int32_t **qmfBufferCodecImag, int32_t nColsIn,
                        int32_t **ppQmfBufferOutReal_F,
                        int32_t **ppQmfBufferOutImag_F,
                        int32_t lpcFilterStatesReal[2 + (3 * (4))][(64)],
                        int32_t lpcFilterStatesImag[2 + (3 * (4))][(64)],
                        int32_t pitchInBins, int32_t scale_lb, int32_t scale_hbe,
                        int32_t *scale_hb, int32_t timeStep, int32_t firstSlotOffsset,
                        int32_t ov_len,
                        KEEP_STATES_SYNCED_MODE keepStatesSyncedMode);

int32_t *GetxOverBandQmfTransposer(HANDLE_HBE_TRANSPOSER hQmfTransposer);

int32_t Get41SbrQmfTransposer(HANDLE_HBE_TRANSPOSER hQmfTransposer);
#endif /* HBE_H */
