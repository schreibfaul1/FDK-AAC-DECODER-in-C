

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
\file
\brief Memory layout
*/
#ifndef SBR_RAM_H
#define SBR_RAM_H
#include "../libAACdec/newAACDecoder.h"
#include "sbrdecoder.h"
#include "env_extr.h"
#include "sbr_dec.h"

#define SBRDEC_MAX_CH_PER_ELEMENT (2)

#define FRAME_OK (0)
#define FRAME_ERROR (1)
#define FRAME_ERROR_ALLSLOTS (2)

typedef struct {
  SBR_CHANNEL *pSbrChannel[SBRDEC_MAX_CH_PER_ELEMENT];
  TRANSPOSER_SETTINGS
  transposerSettings; /* Common transport settings for each individual
                         channel of an element */
  HANDLE_FDK_BITSTREAM hBs;

  MP4_ELEMENT_ID_t
  elementID;     /* Element ID set during initialization. Can be used for
                    concealment */
  int32_t nChannels; /* Number of elements output channels (=2 in case of PS) */

  uint8_t frameErrorFlag[(1) + 1]; /* Frame error status (for every slot in the
                                    delay line). Will be copied into header at
                                    the very beginning of decodeElement()
                                    routine. */

  uint8_t useFrameSlot; /* Index which defines which slot will be decoded/filled
                         next (used with additional delay) */
  uint8_t useHeaderSlot[(1) + 1]; /* Index array that provides the link between
                                   header and frame data (important when
                                   processing with additional delay). */
} SBR_DECODER_ELEMENT;

struct SBR_DECODER_INSTANCE {
  SBR_DECODER_ELEMENT *pSbrElement[(8)];
  SBR_HEADER_DATA sbrHeader[(
      8)][(1) + 1]; /* Sbr header for each individual channel of an element */

  HANDLE_FDK_QMF_DOMAIN pQmfDomain;

  HANDLE_PS_DEC hParametricStereoDec;

  /* Global parameters */
  AUDIO_OBJECT_TYPE_t coreCodec; /* AOT of core codec */
  int32_t numSbrElements;
  int32_t numSbrChannels;
  int32_t sampleRateIn;  /* SBR decoder input sampling rate; might be different than
                        the transposer input sampling rate. */
  int32_t sampleRateOut; /* Sampling rate of the SBR decoder output audio samples.
                      */
  uint16_t codecFrameSize;
  uint8_t synDownsampleFac;
  int32_t downscaleFactor;
  uint8_t numDelayFrames; /* The current number of additional delay frames used
                           for processing. */
  uint8_t harmonicSBR;
  uint8_t
  numFlushedFrames; /* The variable counts the number of frames which are
                       flushed consecutively. */

  uint32_t flags;

  int32_t sbrInDataHeadroom; /* Headroom of the SBR input time signal to prevent
                            clipping */
};









int32_t *GetRam_sbr_QmfStatesSynthesis(int32_t n = 0);
void FreeRam_sbr_QmfStatesSynthesis(int32_t **p);
uint32_t GetRequiredMemRam_sbr_QmfStatesSynthesis(void);

int32_t *GetRam_sbr_OverlapBuffer(int32_t n = 0);
void FreeRam_sbr_OverlapBuffer(int32_t **p);
uint32_t GetRequiredMemRam_sbr_OverlapBuffer(void);

int32_t *GetRam_sbr_HBEOverlapBuffer(int32_t n = 0);
void FreeRam_sbr_HBEOverlapBuffer(int32_t **p);
uint32_t GetRequiredMemRam_sbr_HBEOverlapBuffer(void);






#endif /* SBR_RAM_H */
