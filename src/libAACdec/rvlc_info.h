

/**************************** AAC decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/
#include "newAACDecoder.h"
/*!
  \file
  \brief  Defines structures for RVLC
  \author Robert Weidner
*/
#ifndef RVLC_INFO_H
#define RVLC_INFO_H

#define FWD 0 /* bitstream decoding direction forward (RVL coded part) */
#define BWD 1 /* bitstream decoding direction backward (RVL coded part) */

#define MAX_RVL 7  /* positive RVLC escape */
#define MIN_RVL -7 /* negative RVLC escape */
#define MAX_ALLOWED_DPCM_INDEX                                    \
  14 /* the maximum allowed index of a decoded dpcm value (offset \
        'TABLE_OFFSET' incl --> must be subtracted) */
#define TABLE_OFFSET                                                           \
  7 /* dpcm offset of valid output values of rvl table decoding, the rvl table \
       ouly returns positive values, therefore the offset */
#define MAX_LEN_RVLC_CODE_WORD 9 /* max length of a RVL codeword in bits */
#define MAX_LEN_RVLC_ESCAPE_WORD \
  20 /* max length of huffman coded RVLC escape word in bits */

#define DPCM_NOISE_NRG_BITS 9
#define SF_OFFSET 100 /* offset for correcting scf value */

#define CONCEAL_MAX_INIT 1311  /* arbitrary value */
#define CONCEAL_MIN_INIT -1311 /* arbitrary value */

#define RVLC_MAX_SFB ((8) * (16))

/* sideinfo of RVLC */
typedef struct {
  /* ------- ESC 1 Data: --------- */ /* order of RVLC-bitstream components in
                                         bitstream (RVLC-initialization), every
                                         component appears only once in
                                         bitstream */
  int32_t sf_concealment;                 /* 1          */
  int32_t rev_global_gain;                /* 2          */
  int16_t length_of_rvlc_sf; /* 3          */ /* original value, gets modified
                                               (subtract 9) in case of noise
                                               (PNS); is kept for later use */
  int32_t dpcm_noise_nrg;                       /* 4 optional */
  int32_t sf_escapes_present;                   /* 5          */
  int16_t length_of_rvlc_escapes;             /* 6 optional */
  int32_t dpcm_noise_last_position;             /* 7 optional */

  int32_t dpcm_is_last_position;

  int16_t length_of_rvlc_sf_fwd; /* length_of_rvlc_sf used for forward decoding */
  int16_t
  length_of_rvlc_sf_bwd; /* length_of_rvlc_sf used for backward decoding */

  /* for RVL-Codeword decoder to distinguish between fwd and bwd decoding */
  int16_t *pRvlBitCnt_RVL;
  int32_t *pBitstrIndxRvl_RVL;

  uint8_t numWindowGroups;
  uint8_t maxSfbTransmitted;
  uint8_t first_noise_group;
  uint8_t first_noise_band;
  uint8_t direction;

  /* bitstream indices */
  int32_t bsAnchor;             /* hcr bit buffer reference index */
  int32_t bitstreamIndexRvlFwd; /* base address of RVL-coded-scalefactor data (ESC
                               2) for forward  decoding */
  int32_t bitstreamIndexRvlBwd; /* base address of RVL-coded-scalefactor data (ESC
                               2) for backward decoding */
  int32_t bitstreamIndexEsc;    /* base address where RVLC-escapes start (ESC 2) */

  /* decoding trees */
  const uint32_t *pHuffTreeRvlCodewds;
  const uint32_t *pHuffTreeRvlcEscape;

  /* escape counters */
  uint8_t numDecodedEscapeWordsFwd; /* when decoding RVL-codes forward  */
  uint8_t numDecodedEscapeWordsBwd; /* when decoding RVL-codes backward */
  uint8_t numDecodedEscapeWordsEsc; /* when decoding the escape-Words */

  int8_t noise_used;
  int8_t intensity_used;
  int8_t sf_used;

  int16_t firstScf;
  int16_t lastScf;
  int16_t firstNrg;
  int16_t lastNrg;
  int16_t firstIs;
  int16_t lastIs;

  /* ------ RVLC error detection ------ */
  uint32_t errorLogRvlc;     /* store RVLC errors  */
  int16_t conceal_min;     /* is set at backward decoding  */
  int16_t conceal_max;     /* is set at forward  decoding  */
  int16_t conceal_min_esc; /* is set at backward decoding  */
  int16_t conceal_max_esc; /* is set at forward  decoding  */
} CErRvlcInfo;

typedef CErRvlcInfo RVLC_INFO; /* temp */

#endif /* RVLC_INFO_H */
