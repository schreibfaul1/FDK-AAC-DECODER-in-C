

/**************************** AAC decoder library ******************************

   Author(s):   Robert Weidner (DSP Solutions)

   Description: HCR Decoder: Defines of state-constants, masks and
                state-prototypes

*******************************************************************************/

#ifndef AACDEC_HCRS_H
#define AACDEC_HCRS_H
#include "newAACDecoder.h"
#include "../libFDK/FDK_bitstream.h"
#include "aacdec_hcr_types.h"
/* The four different kinds of types of states are:                     */
/* different states are defined as constants */ /* start   middle=self   next
                                                   stop */
#define STOP_THIS_STATE \
  0 /*                                                                      */
#define BODY_ONLY \
  1 /*   X          X                X                                      */
#define BODY_SIGN__BODY \
  2 /*   X          X         X      X [stop if no sign]                    */
#define BODY_SIGN__SIGN \
  3 /*              X                X [stop if sign bits decoded]          */
#define BODY_SIGN_ESC__BODY \
  4 /*   X          X         X      X [stop if no sign]                    */
#define BODY_SIGN_ESC__SIGN \
  5 /*              X         X      X [stop if no escape sequence]         */
#define BODY_SIGN_ESC__ESC_PREFIX \
  6 /*              X         X                                             */
#define BODY_SIGN_ESC__ESC_WORD \
  7 /*              X         X      X [stop if abs(second qsc) != 16]      */

/* examples: */

/* BODY_ONLY                    means only the codeword body will be decoded; no
 * sign bits will follow and no escapesequence will follow */

/* BODY_SIGN__BODY              means that the codeword consists of two parts;
 * body and sign part. The part '__BODY' after the two underscores shows */
/*                              that the bits which are currently decoded belong
 * to the '__BODY' of the codeword and not to the sign part. */

/* BODY_SIGN_ESC__ESC_PB        means that the codeword consists of three parts;
 * body, sign and (here: two) escape sequences;  */
/*                              P = Prefix = ones */
/*                              W = Escape Word */
/*                              A = first possible (of two) Escape sequeces */
/*                              B = second possible (of two) Escape sequeces */
/*                              The part after the two underscores shows that
 * the current bits which are decoded belong to the '__ESC_PB' - part of the */
/*                              codeword. That means the body and the sign bits
 * are decoded completely and the bits which are decoded now belong to */
/*                              the escape sequence [P = prefix; B=second
 * possible escape sequence] */

#define MSB_31_MASK 0x80000000 /* masks MSB (= Bit 31) in a 32 bit word */
#define DIMENSION_OF_ESCAPE_CODEBOOK 2 /* for cb >= 11 is dimension 2 */
#define ESCAPE_CODEBOOK 11

#define MASK_ESCAPE_PREFIX_UP 0x000F0000
#define LSB_ESCAPE_PREFIX_UP 16

#define MASK_ESCAPE_PREFIX_DOWN 0x0000F000
#define LSB_ESCAPE_PREFIX_DOWN 12

#define MASK_ESCAPE_WORD 0x00000FFF
#define MASK_FLAG_A 0x00200000
#define MASK_FLAG_B 0x00100000

extern void DecodeNonPCWs(HANDLE_FDK_BITSTREAM bs, H_HCR_INFO hHcr);

uint32_t Hcr_State_BODY_ONLY(HANDLE_FDK_BITSTREAM, void*);
uint32_t Hcr_State_BODY_SIGN__BODY(HANDLE_FDK_BITSTREAM, void*);
uint32_t Hcr_State_BODY_SIGN__SIGN(HANDLE_FDK_BITSTREAM, void*);
uint32_t Hcr_State_BODY_SIGN_ESC__BODY(HANDLE_FDK_BITSTREAM, void*);
uint32_t Hcr_State_BODY_SIGN_ESC__SIGN(HANDLE_FDK_BITSTREAM, void*);
uint32_t Hcr_State_BODY_SIGN_ESC__ESC_PREFIX(HANDLE_FDK_BITSTREAM, void*);
uint32_t Hcr_State_BODY_SIGN_ESC__ESC_WORD(HANDLE_FDK_BITSTREAM, void*);

#endif /* AACDEC_HCRS_H */
