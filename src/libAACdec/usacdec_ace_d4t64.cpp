

/**************************** AAC decoder library ******************************

   Author(s):

   Description: ACELP

*******************************************************************************/

#include <memory.h>
#include "newAACDecoder.h"

#define L_SUBFR 64 /* Subframe size              */

/*
 * D_ACELP_add_pulse
 *
 * Parameters:
 *    pos         I: position of pulse
 *    nb_pulse    I: number of pulses
 *    track       I: track
 *    code        O: fixed codebook
 *
 * Function:
 *    Add pulses to fixed codebook
 *
 * Returns:
 *    void
 */
static void D_ACELP_add_pulse(int16_t pos[], int16_t nb_pulse, int16_t track,
                              FIXP_COD code[]) {
  int16_t i, k;
  for (k = 0; k < nb_pulse; k++) {
    /* i = ((pos[k] & (16-1))*NB_TRACK) + track; */
    i = ((pos[k] & (16 - 1)) << 2) + track;
    if ((pos[k] & 16) == 0) {
      code[i] = code[i] + (FIXP_COD)(512 << (COD_BITS - FRACT_BITS));
    } else {
      code[i] = code[i] - (FIXP_COD)(512 << (COD_BITS - FRACT_BITS));
    }
  }
  return;
}
/*
 * D_ACELP_decode_1p_N1
 *
 * Parameters:
 *    index    I: pulse index
 *    N        I: number of bits for position
 *    offset   I: offset
 *    pos      O: position of the pulse
 *
 * Function:
 *    Decode 1 pulse with N+1 bits
 *
 * Returns:
 *    void
 */
static void D_ACELP_decode_1p_N1(int32_t index, int16_t N, int16_t offset,
                                 int16_t pos[]) {
  int16_t pos1;
  int32_t i, mask;

  mask = ((1 << N) - 1);
  /*
   * Decode 1 pulse with N+1 bits
   */
  pos1 = (int16_t)((index & mask) + offset);
  i = ((index >> N) & 1);
  if (i == 1) {
    pos1 += 16;
  }
  pos[0] = pos1;
  return;
}
/*
 * D_ACELP_decode_2p_2N1
 *
 * Parameters:
 *    index    I: pulse index
 *    N        I: number of bits for position
 *    offset   I: offset
 *    pos      O: position of the pulse
 *
 * Function:
 *    Decode 2 pulses with 2*N+1 bits
 *
 * Returns:
 *    void
 */
static void D_ACELP_decode_2p_2N1(int32_t index, int16_t N, int16_t offset,
                                  int16_t pos[]) {
  int16_t pos1, pos2;
  int32_t mask, i;
  mask = ((1 << N) - 1);
  /*
   * Decode 2 pulses with 2*N+1 bits
   */
  pos1 = (int16_t)(((index >> N) & mask) + offset);
  i = (index >> (2 * N)) & 1;
  pos2 = (int16_t)((index & mask) + offset);
  if ((pos2 - pos1) < 0) {
    if (i == 1) {
      pos1 += 16;
    } else {
      pos2 += 16;
    }
  } else {
    if (i == 1) {
      pos1 += 16;
      pos2 += 16;
    }
  }
  pos[0] = pos1;
  pos[1] = pos2;
  return;
}
/*
 * D_ACELP_decode_3p_3N1
 *
 * Parameters:
 *    index    I: pulse index
 *    N        I: number of bits for position
 *    offset   I: offset
 *    pos      O: position of the pulse
 *
 * Function:
 *    Decode 3 pulses with 3*N+1 bits
 *
 * Returns:
 *    void
 */
static void D_ACELP_decode_3p_3N1(int32_t index, int16_t N, int16_t offset,
                                  int16_t pos[]) {
  int16_t j;
  int32_t mask, idx;

  /*
   * Decode 3 pulses with 3*N+1 bits
   */
  mask = ((1 << ((2 * N) - 1)) - 1);
  idx = index & mask;
  j = offset;
  if (((index >> ((2 * N) - 1)) & 1) == 1) {
    j += (1 << (N - 1));
  }
  D_ACELP_decode_2p_2N1(idx, N - 1, j, pos);
  mask = ((1 << (N + 1)) - 1);
  idx = (index >> (2 * N)) & mask;
  D_ACELP_decode_1p_N1(idx, N, offset, pos + 2);
  return;
}
/*
 * D_ACELP_decode_4p_4N1
 *
 * Parameters:
 *    index    I: pulse index
 *    N        I: number of bits for position
 *    offset   I: offset
 *    pos      O: position of the pulse
 *
 * Function:
 *    Decode 4 pulses with 4*N+1 bits
 *
 * Returns:
 *    void
 */
static void D_ACELP_decode_4p_4N1(int32_t index, int16_t N, int16_t offset,
                                  int16_t pos[]) {
  int16_t j;
  int32_t mask, idx;
  /*
   * Decode 4 pulses with 4*N+1 bits
   */
  mask = ((1 << ((2 * N) - 1)) - 1);
  idx = index & mask;
  j = offset;
  if (((index >> ((2 * N) - 1)) & 1) == 1) {
    j += (1 << (N - 1));
  }
  D_ACELP_decode_2p_2N1(idx, N - 1, j, pos);
  mask = ((1 << ((2 * N) + 1)) - 1);
  idx = (index >> (2 * N)) & mask;
  D_ACELP_decode_2p_2N1(idx, N, offset, pos + 2);
  return;
}
/*
 * D_ACELP_decode_4p_4N
 *
 * Parameters:
 *    index    I: pulse index
 *    N        I: number of bits for position
 *    offset   I: offset
 *    pos      O: position of the pulse
 *
 * Function:
 *    Decode 4 pulses with 4*N bits
 *
 * Returns:
 *    void
 */
static void D_ACELP_decode_4p_4N(int32_t index, int16_t N, int16_t offset,
                                 int16_t pos[]) {
  int16_t j, n_1;
  /*
   * Decode 4 pulses with 4*N bits
   */
  n_1 = N - 1;
  j = offset + (1 << n_1);
  switch ((index >> ((4 * N) - 2)) & 3) {
    case 0:
      if (((index >> ((4 * n_1) + 1)) & 1) == 0) {
        D_ACELP_decode_4p_4N1(index, n_1, offset, pos);
      } else {
        D_ACELP_decode_4p_4N1(index, n_1, j, pos);
      }
      break;
    case 1:
      D_ACELP_decode_1p_N1((index >> ((3 * n_1) + 1)), n_1, offset, pos);
      D_ACELP_decode_3p_3N1(index, n_1, j, pos + 1);
      break;
    case 2:
      D_ACELP_decode_2p_2N1((index >> ((2 * n_1) + 1)), n_1, offset, pos);
      D_ACELP_decode_2p_2N1(index, n_1, j, pos + 2);
      break;
    case 3:
      D_ACELP_decode_3p_3N1((index >> (n_1 + 1)), n_1, offset, pos);
      D_ACELP_decode_1p_N1(index, n_1, j, pos + 3);
      break;
  }
  return;
}

/*
 * D_ACELP_decode_4t
 *
 * Parameters:
 *    index          I: index
 *    mode           I: speech mode
 *    code           I: (Q9) algebraic (fixed) codebook excitation
 *
 * Function:
 *    20, 36, 44, 52, 64, 72, 88 bits algebraic codebook.
 *    4 tracks x 16 positions per track = 64 samples.
 *
 *    20 bits 5+5+5+5 --> 4 pulses in a frame of 64 samples.
 *    36 bits 9+9+9+9 --> 8 pulses in a frame of 64 samples.
 *    44 bits 13+9+13+9 --> 10 pulses in a frame of 64 samples.
 *    52 bits 13+13+13+13 --> 12 pulses in a frame of 64 samples.
 *    64 bits 2+2+2+2+14+14+14+14 --> 16 pulses in a frame of 64 samples.
 *    72 bits 10+2+10+2+10+14+10+14 --> 18 pulses in a frame of 64 samples.
 *    88 bits 11+11+11+11+11+11+11+11 --> 24 pulses in a frame of 64 samples.
 *
 *    All pulses can have two (2) possible amplitudes: +1 or -1.
 *    Each pulse can sixteen (16) possible positions.
 *
 *    codevector length    64
 *    number of track      4
 *    number of position   16
 *
 * Returns:
 *    void
 */
void D_ACELP_decode_4t64(int16_t index[], int32_t nbits, FIXP_COD code[]) {
  int32_t L_index;
  int16_t k, pos[6];

  memset(code, 0, L_SUBFR * sizeof(FIXP_COD));

  /* decode the positions and signs of pulses and build the codeword */
  switch (nbits) {
    case 12:
      for (k = 0; k < 4; k += 2) {
        L_index = index[2 * (k / 2) + 1];
        D_ACELP_decode_1p_N1(L_index, 4, 0, pos);
        D_ACELP_add_pulse(pos, 1, 2 * (index[2 * (k / 2)]) + k / 2, code);
      }
      break;
    case 16: {
      int32_t i = 0;
      int32_t offset = index[i++];
      offset = (offset == 0) ? 1 : 3;
      for (k = 0; k < 4; k++) {
        if (k != offset) {
          L_index = index[i++];
          D_ACELP_decode_1p_N1(L_index, 4, 0, pos);
          D_ACELP_add_pulse(pos, 1, k, code);
        }
      }
    } break;
    case 20:
      for (k = 0; k < 4; k++) {
        L_index = (int32_t)index[k];
        D_ACELP_decode_1p_N1(L_index, 4, 0, pos);
        D_ACELP_add_pulse(pos, 1, k, code);
      }
      break;
    case 28:
      for (k = 0; k < 4 - 2; k++) {
        L_index = (int32_t)index[k];
        D_ACELP_decode_2p_2N1(L_index, 4, 0, pos);
        D_ACELP_add_pulse(pos, 2, k, code);
      }
      for (k = 2; k < 4; k++) {
        L_index = (int32_t)index[k];
        D_ACELP_decode_1p_N1(L_index, 4, 0, pos);
        D_ACELP_add_pulse(pos, 1, k, code);
      }
      break;
    case 36:
      for (k = 0; k < 4; k++) {
        L_index = (int32_t)index[k];
        D_ACELP_decode_2p_2N1(L_index, 4, 0, pos);
        D_ACELP_add_pulse(pos, 2, k, code);
      }
      break;
    case 44:
      for (k = 0; k < 4 - 2; k++) {
        L_index = (int32_t)index[k];
        D_ACELP_decode_3p_3N1(L_index, 4, 0, pos);
        D_ACELP_add_pulse(pos, 3, k, code);
      }
      for (k = 2; k < 4; k++) {
        L_index = (int32_t)index[k];
        D_ACELP_decode_2p_2N1(L_index, 4, 0, pos);
        D_ACELP_add_pulse(pos, 2, k, code);
      }
      break;
    case 52:
      for (k = 0; k < 4; k++) {
        L_index = (int32_t)index[k];
        D_ACELP_decode_3p_3N1(L_index, 4, 0, pos);
        D_ACELP_add_pulse(pos, 3, k, code);
      }
      break;
    case 64:
      for (k = 0; k < 4; k++) {
        L_index = (((int32_t)index[k] << 14) + (int32_t)index[k + 4]);
        D_ACELP_decode_4p_4N(L_index, 4, 0, pos);
        D_ACELP_add_pulse(pos, 4, k, code);
      }
      break;
    default:
      assert(0);
  }
  return;
}
