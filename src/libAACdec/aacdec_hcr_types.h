

/**************************** AAC decoder library ******************************

   Author(s):   Robert Weidner (DSP Solutions)

   Description: HCR Decoder: Common defines and structures; defines for
                switching error-generator, -detector, and -concealment;

*******************************************************************************/

#ifndef AACDEC_HCR_TYPES_H
#define AACDEC_HCR_TYPES_H
#include "newAACDecoder.h"
#include "../libFDK/FDK_bitstream.h"
#include "overlapadd.h"

/* ------------------------------------------------ */
/* ------------------------------------------------ */

#define LINES_PER_UNIT 4

/* ------------------------------------------------ */
/* ------------------------------------------------ */
/* ----------- basic HCR configuration ------------ */



/* ------------------------------------------------ */
/* ------------------------------------------------ */
/* ------------------------------------------------ */



/* when set HCR is replaced by a dummy-module which just fills the outputbuffer
 * with a dirac sequence */
/* use this if HCR is suspected to write in other modules -- if error is stell
 * there, HCR is innocent */

/* ------------------------------ */
/* -    insert HCR errors       - */
/* ------------------------------ */

/* modify input lengths -- high protected */
#define ERROR_LORSD 0 /* offset: error if different from zero */
#define ERROR_LOLC 0  /* offset: error if different from zero */

/* segments are earlier empty as expected when decoding PCWs */
#define ERROR_PCW_BODY                                                   \
  0 /* set a positive values to trigger the error (make segments earlyer \
       appear to be empty) */
#define ERROR_PCW_BODY_SIGN                                              \
  0 /* set a positive values to trigger the error (make segments earlyer \
       appear to be empty) */
#define ERROR_PCW_BODY_SIGN_ESC                                          \
  0 /* set a positive values to trigger the error (make segments earlyer \
       appear to be empty) */

/* pretend there are too many bits decoded (enlarge length of codeword) at PCWs
 * -- use a positive value */
#define ERROR_PCW_BODY_ONLY_TOO_LONG \
  0 /* set a positive values to trigger the error */
#define ERROR_PCW_BODY_SIGN_TOO_LONG \
  0 /* set a positive values to trigger the error */
#define ERROR_PCW_BODY_SIGN_ESC_TOO_LONG \
  0 /* set a positive values to trigger the error */

/* modify HCR bitstream block */

#define MODULO_DIVISOR_HCR 30

/* ------------------------------ */
/* -    detect HCR errors       - */
/* ------------------------------ */
/* check input data */

/* during decoding */

/* all the segments are checked -- therefore -- if this check passes, its a kind
   of evidence that the decoded PCWs and non-PCWs are fine */

/* if a codeword is decoded there exists a border for the number of bits, which
   are allowed to read for this codeword. This border is the minimum of the
   length of the longest codeword (for the currently used codebook) and the
   separately transmitted 'lengthOfLongestCodeword' in this frame and channel.
   The number of decoded bits is counted (for PCWs only -- there it makes really
   sense in my opinion). If this number exceeds the border (derived as minimum
   -- see above), a error is detected. */

/* -----------------------------------------------------------------------------------------------------
   This error check could be set to zero because due to a test within
   RVLC-Escape-huffman-Decoder a too int32_t codeword could not be detected -- it
   seems that for RVLC-Escape-Codeword the coderoom is used to 100%. Therefore I
   assume that the coderoom is used to 100% also for the codebooks 1..11 used at
   HCR Therefore this test is deactivated pending further notice
   -----------------------------------------------------------------------------------------------------
 */

/* test if the number of remaining bits in a segment is _below_ zero. If there
   are no errors the lowest allowed value for remainingBitsInSegment is zero.
   This check also could be set to zero (save runtime) */

/* other */
/* when set to '1', avoid setting the LAV-Flag in errorLog due to a
   previous-line-marking (at PCW decoder). A little more runtime is needed then
   when writing values out into output-buffer. */

/* ------------------------------ */
/* -    conceal HCR errors      - */
/* ------------------------------ */

#define HCR_ERROR_CONCEALMENT \
  1 /* if set to '1', HCR _mutes_ the erred quantized spectral coefficients */

// ------------------------------------------------------------------------------------------------------------------
//                                         errorLog: A word of 32 bits used for
//                                         logging possible errors within HCR
//                                                   in case of distorted
//                                                   bitstreams. Table of all
//                                                   known errors:
// ------------------------------------------------------------------------------------------------------------------------
// bit  fatal  location    meaning
// ----+-----+-----------+--------------------------------------
#define SEGMENT_OVERRIDE_ERR_PCW_BODY \
  0x80000000  //  31   no    PCW-Dec     During PCW decoding it is checked after
              //  every PCW if there are too many bits decoded (immediate
              //  check).
#define SEGMENT_OVERRIDE_ERR_PCW_BODY_SIGN \
  0x40000000  //  30   no    PCW-Dec     During PCW decoding it is checked after
              //  every PCW if there are too many bits decoded (immediate
              //  check).
#define SEGMENT_OVERRIDE_ERR_PCW_BODY_SIGN_ESC \
  0x20000000  //  29   no    PCW-Dec     During PCW decoding it is checked after
              //  every PCW if there are too many bits decoded (immediate
              //  check).
#define EXTENDED_SORTED_COUNTER_OVERFLOW \
  0x10000000  //  28   yes   Init-Dec    Error during extending sideinfo
              //  (neither a PCW nor a nonPCW was decoded so far)
              // 0x08000000 //  27                     reserved
              // 0x04000000 //  26                     reserved
              // 0x02000000 //  25                     reserved
              // 0x01000000 //  24                     reserved
              // 0x00800000 //  23                     reserved
              // 0x00400000 //  22                     reserved
              // 0x00200000 //  21                     reserved
              // 0x00100000 //  20                     reserved

/* special errors */
#define TOO_MANY_PCW_BODY_BITS_DECODED \
  0x00080000  //  19   yes   PCW-Dec     During PCW-body-decoding too many bits
              //  have been read from bitstream -- advice: skip non-PCW decoding
#define TOO_MANY_PCW_BODY_SIGN_BITS_DECODED \
  0x00040000  //  18   yes   PCW-Dec     During PCW-body-sign-decoding too many
              //  bits have been read from bitstream -- advice: skip non-PCW
              //  decoding
#define TOO_MANY_PCW_BODY_SIGN_ESC_BITS_DECODED \
  0x00020000  //  17   yes   PCW-Dec     During PCW-body-sign-esc-decoding too
              //  many bits have been read from bitstream -- advice: skip
              //  non-PCW decoding

// 0x00010000 //  16                     reserved
#define STATE_ERROR_BODY_ONLY \
  0x00008000  //  15   no    NonPCW-Dec  State machine returned with error
#define STATE_ERROR_BODY_SIGN__BODY \
  0x00004000  //  14   no    NonPCW-Dec  State machine returned with error
#define STATE_ERROR_BODY_SIGN__SIGN \
  0x00002000  //  13   no    NonPCW-Dec  State machine returned with error
#define STATE_ERROR_BODY_SIGN_ESC__BODY \
  0x00001000  //  12   no    NonPCW-Dec  State machine returned with error
#define STATE_ERROR_BODY_SIGN_ESC__SIGN \
  0x00000800  //  11   no    NonPCW-Dec  State machine returned with error
#define STATE_ERROR_BODY_SIGN_ESC__ESC_PREFIX \
  0x00000400  //  10   no    NonPCW-Dec  State machine returned with error
#define STATE_ERROR_BODY_SIGN_ESC__ESC_WORD \
  0x00000200  //   9   no    NonPCW-Dec  State machine returned with error
#define HCR_SI_LENGTHS_FAILURE \
  0x00000100  //   8   yes   Init-Dec    LengthOfLongestCodeword must not be
              //   less than lenghtOfReorderedSpectralData
#define NUM_SECT_OUT_OF_RANGE_SHORT_BLOCK \
  0x00000080  //   7   yes   Init-Dec    The number of sections is not within
              //   the allowed range (int16_t block)
#define NUM_SECT_OUT_OF_RANGE_LONG_BLOCK \
  0x00000040  //   6   yes   Init-Dec    The number of sections is not within
              //   the allowed range (int32_t block)
#define LINE_IN_SECT_OUT_OF_RANGE_SHORT_BLOCK \
  0x00000020  //   5   yes   Init-Dec    The number of lines per section is not
              //   within the allowed range (int16_t block)
#define CB_OUT_OF_RANGE_SHORT_BLOCK \
  0x00000010  //   4   yes   Init-Dec    The codebook is not within the allowed
              //   range (int16_t block)
#define LINE_IN_SECT_OUT_OF_RANGE_LONG_BLOCK \
  0x00000008  //   3   yes   Init-Dec    The number of lines per section is not
              //   within the allowed range (int32_t block)
#define CB_OUT_OF_RANGE_LONG_BLOCK \
  0x00000004  //   2   yes   Init-Dec    The codebook is not within the allowed
              //   range (int32_t block)
#define LAV_VIOLATION \
  0x00000002  //   1   no    Final       The absolute value of at least one
              //   decoded line was too high for the according codebook.
#define BIT_IN_SEGMENTATION_ERROR \
  0x00000001  //   0   no    Final       After PCW and non-PWC-decoding at least
              //   one segment is not zero (global check).

/*----------*/
#define HCR_FATAL_PCW_ERROR_MASK 0x100E01FC

typedef enum { PCW_BODY, PCW_BODY_SIGN, PCW_BODY_SIGN_ESC } PCW_TYPE;

/* interface Decoder <---> HCR */

#endif /* AACDEC_HCR_TYPES_H */
