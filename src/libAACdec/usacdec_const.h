

/**************************** AAC decoder library ******************************

   Author(s):   Manuel Jander

   Description: USAC related constants

*******************************************************************************/
#include "newAACDecoder.h"


/* Maximum pitch lag (= 411 for fs_max = 24000) */
#define PIT_MAX_TMP                                                            \
  (PIT_MAX_12k8 +                                                              \
   (6 *                                                                        \
    ((((FAC_FSCALE_MAX * PIT_MIN_12k8) + (FSCALE_DENOM / 2)) / FSCALE_DENOM) - \
     PIT_MIN_12k8)))
#if (PIT_MAX_TMP < \
     256) /* cannot be smaller because of tcx time domain concealment */
#define PIT_MAX_MAX 256
#else
// #include "../AACCostantsOverride.h"
#ifndef PIT_MAX_MAX
#define PIT_MAX_MAX PIT_MAX_TMP
#endif



/* definitions for coreCoderFrameLength = 768 */
#define L_FRAME_PLUS_768 768
#define L_DIV_768 \
  (L_FRAME_PLUS_768 / NB_DIV) /* length of one acelp or tcx20 frame */
#define NB_SUBFR_768 \
  (L_DIV_768 / L_SUBFR) /* number of 5ms subframe per division */
#define NB_SUBFR_SUPERFR_768 \
  (L_FRAME_PLUS_768 / L_SUBFR) /* number of 5ms subframe per 80ms frame */
#define AAC_SFD_768 (NB_SUBFR_SUPERFR_768 / 2) /* AAC delay (subframe) */
#define AAC_DELAY_768 (AAC_SFD_768 * L_SUBFR)  /* AAC delay (samples) */
#define SYN_SFD_768 (AAC_SFD_768 - BPF_SFD)    /* synthesis delay (subframe) */
#define SYN_DELAY_768 (SYN_SFD_768 * L_SUBFR)  /* synthesis delay (samples) */
#define LFAC_768 (L_DIV_768 / 2)               /* FAC frame length */
#define LFAC_SHORT_768 \
  (L_DIV_768 / 4) /* for transitions EIGHT_SHORT FD->LPD and vv. */

/* maximum (used for memory allocation) */


#endif /* USACDEC_CONST_H */
