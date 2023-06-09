

/******************* Library for basic calculation routines ********************

   Author(s):   Omer Osman

   Description: SAC/SAOC Dec Noiseless Coding

*******************************************************************************/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "nlc_dec.h"
#include "FDK_tools_rom.h"

/* MAX_PARAMETER_BANDS defines array length in huffdec */

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

ERROR_t sym_restoreIPD(HANDLE_FDK_BITSTREAM strm, int32_t lav, int8_t data[2]) {
  int32_t sum_val = data[0] + data[1];
  int32_t diff_val = data[0] - data[1];

  if (sum_val > lav) {
    data[0] = -sum_val + (2 * lav + 1);
    data[1] = -diff_val;
  } else {
    data[0] = sum_val;
    data[1] = diff_val;
  }

  if (data[0] - data[1] != 0) {
    uint32_t sym_bit;
    sym_bit = FDKreadBits(strm, 1);
    if (sym_bit) {
      int32_t tmp;
      tmp = data[0];
      data[0] = data[1];
      data[1] = tmp;
    }
  }

  return HUFFDEC_OK;
}

static int32_t ilog2(uint32_t i) {
  int32_t l = 0;

  if (i) i--;
  while (i > 0) {
    i >>= 1;
    l++;
  }

  return l;
}

static ERROR_t pcm_decode(HANDLE_FDK_BITSTREAM strm, int8_t* out_data_1,
                          int8_t* out_data_2, int32_t offset, int32_t num_val,
                          int32_t num_levels) {
  int32_t i = 0, j = 0, idx = 0;
  int32_t max_grp_len = 0, next_val = 0;
  uint32_t tmp;

  int32_t pcm_chunk_size[7] = {0};

  switch (num_levels) {
    case 3:
      max_grp_len = 5;
      break;
    case 7:
      max_grp_len = 6;
      break;
    case 11:
      max_grp_len = 2;
      break;
    case 13:
      max_grp_len = 4;
      break;
    case 19:
      max_grp_len = 4;
      break;
    case 25:
      max_grp_len = 3;
      break;
    case 51:
      max_grp_len = 4;
      break;
    case 4:
    case 8:
    case 15:
    case 16:
    case 26:
    case 31:
      max_grp_len = 1;
      break;
    default:
      return HUFFDEC_NOTOK;
  }

  tmp = 1;
  for (i = 1; i <= max_grp_len; i++) {
    tmp *= num_levels;
    pcm_chunk_size[i] = ilog2(tmp);
  }

  for (i = 0; i < num_val; i += max_grp_len) {
    int32_t grp_len, grp_val, data;
    grp_len = min(max_grp_len, num_val - i);
    data = FDKreadBits(strm, pcm_chunk_size[grp_len]);

    grp_val = data;

    for (j = 0; j < grp_len; j++) {
      idx = i + (grp_len - j - 1);
      next_val = grp_val % num_levels;

      if (out_data_2 == NULL) {
        out_data_1[idx] = next_val - offset;
      } else if (out_data_1 == NULL) {
        out_data_2[idx] = next_val - offset;
      } else {
        if (idx % 2) {
          out_data_2[idx / 2] = next_val - offset;
        } else {
          out_data_1[idx / 2] = next_val - offset;
        }
      }

      grp_val = (grp_val - next_val) / num_levels;
    }
  }

  return HUFFDEC_OK;
}

static ERROR_t huff_read(HANDLE_FDK_BITSTREAM strm,
                         const int16_t (*nodeTab)[MAX_ENTRIES][2],
                         int32_t* out_data) {
  int32_t node = 0;
  int32_t len = 0;

  do {
    uint32_t next_bit;
    next_bit = FDKreadBits(strm, 1);
    len++;
    node = (*nodeTab)[node][next_bit];
  } while (node > 0);

  *out_data = node;

  return HUFFDEC_OK;
}

static ERROR_t huff_read_2D(HANDLE_FDK_BITSTREAM strm,
                            const int16_t (*nodeTab)[MAX_ENTRIES][2],
                            int8_t out_data[2], int32_t* escape) {
  ERROR_t err = HUFFDEC_OK;

  int32_t huff_2D_8bit = 0;
  int32_t node = 0;

  if ((err = huff_read(strm, nodeTab, &node)) != HUFFDEC_OK) {
    goto bail;
  }
  *escape = (node == 0);

  if (*escape) {
    out_data[0] = 0;
    out_data[1] = 1;
  } else {
    huff_2D_8bit = -(node + 1);
    out_data[0] = huff_2D_8bit >> 4;
    out_data[1] = huff_2D_8bit & 0xf;
  }

bail:
  return err;
}

static ERROR_t sym_restore(HANDLE_FDK_BITSTREAM strm, int32_t lav, int8_t data[2]) {
  uint32_t sym_bit = 0;

  int32_t sum_val = data[0] + data[1];
  int32_t diff_val = data[0] - data[1];

  if (sum_val > lav) {
    data[0] = -sum_val + (2 * lav + 1);
    data[1] = -diff_val;
  } else {
    data[0] = sum_val;
    data[1] = diff_val;
  }

  if (data[0] + data[1] != 0) {
    sym_bit = FDKreadBits(strm, 1);
    if (sym_bit) {
      data[0] = -data[0];
      data[1] = -data[1];
    }
  }

  if (data[0] - data[1] != 0) {
    sym_bit = FDKreadBits(strm, 1);
    if (sym_bit) {
      int32_t tmp;
      tmp = data[0];
      data[0] = data[1];
      data[1] = tmp;
    }
  }

  return HUFFDEC_OK;
}

static ERROR_t huff_dec_1D(HANDLE_FDK_BITSTREAM strm, const DATA_TYPE data_type,
                           const int32_t dim1, int8_t* out_data, const int32_t num_val,
                           const int32_t p0_flag)

{
  ERROR_t err = HUFFDEC_OK;
  int32_t i = 0, node = 0, offset = 0;
  int32_t od = 0, od_sign = 0;
  uint32_t data = 0;
  int32_t bitsAvail = 0;

  const int16_t(*partTab)[MAX_ENTRIES][2] = NULL;
  const int16_t(*nodeTab)[MAX_ENTRIES][2] = NULL;

  switch (data_type) {
    case t_CLD:
      partTab = (HANDLE_HUFF_NODE)&FDK_huffPart0Nodes.cld[0][0];
      nodeTab = (HANDLE_HUFF_NODE)&FDK_huffCLDNodes.h1D[dim1]->nodeTab[0][0];
      break;
    case t_ICC:
      partTab = (HANDLE_HUFF_NODE)&FDK_huffPart0Nodes.icc[0][0];
      nodeTab = (HANDLE_HUFF_NODE)&FDK_huffICCNodes.h1D[dim1]->nodeTab[0][0];
      break;
    case t_OLD:
      partTab = (HANDLE_HUFF_NODE)&FDK_huffPart0Nodes.old[0][0];
      nodeTab = (HANDLE_HUFF_NODE)&huffOLDNodes.h1D[dim1]->nodeTab[0][0];
      break;
    case t_IPD:
      partTab = (HANDLE_HUFF_NODE)&FDK_huffPart0Nodes.ipd[0][0];
      nodeTab = (HANDLE_HUFF_NODE)&FDK_huffIPDNodes.h1D[dim1].nodeTab[0][0];
      break;
    default:
      assert(0);
      err = HUFFDEC_NOTOK;
      goto bail;
  }

  if (p0_flag) {
    if ((err = huff_read(strm, partTab, &node)) != HUFFDEC_OK) {
      goto bail;
    }

    out_data[0] = -(node + 1);
    offset = 1;
  }

  for (i = offset; i < num_val; i++) {
    bitsAvail = FDKgetValidBits(strm);
    if (bitsAvail < 1) {
      err = HUFFDEC_NOTOK;
      goto bail;
    }

    if ((err = huff_read(strm, nodeTab, &node)) != HUFFDEC_OK) {
      goto bail;
    }
    od = -(node + 1);

    if (data_type != t_IPD) {
      if (od != 0) {
        bitsAvail = FDKgetValidBits(strm);
        if (bitsAvail < 1) {
          err = HUFFDEC_NOTOK;
          goto bail;
        }

        data = FDKreadBits(strm, 1);
        od_sign = data;

        if (od_sign) od = -od;
      }
    }

    out_data[i] = od;
  }

bail:
  return err;
}

static ERROR_t huff_dec_2D(HANDLE_FDK_BITSTREAM strm, const DATA_TYPE data_type,
                           const int32_t dim1, const int32_t dim2, int8_t out_data[][2],
                           const int32_t num_val, const int32_t stride,
                           int8_t* p0_data[2]) {
  ERROR_t err = HUFFDEC_OK;
  int32_t i = 0, lav = 0, escape = 0, escCntr = 0;
  int32_t node = 0;
  uint32_t data = 0;

  int8_t esc_data[2][28] = {{0}};
  int32_t escIdx[28] = {0};
  const int16_t(*nodeTab)[MAX_ENTRIES][2] = NULL;

  /* LAV */
  if ((err =
           huff_read(strm, (HANDLE_HUFF_NODE)&FDK_huffLavIdxNodes.nodeTab[0][0],
                     &node)) != HUFFDEC_OK) {
    goto bail;
  }
  data = -(node + 1);

  switch (data_type) {
    case t_CLD:
      lav = 2 * data + 3; /* 3, 5, 7, 9 */
      nodeTab = (HANDLE_HUFF_NODE)&FDK_huffPart0Nodes.cld[0][0];
      break;
    case t_ICC:
      lav = 2 * data + 1; /* 1, 3, 5, 7 */
      nodeTab = (HANDLE_HUFF_NODE)&FDK_huffPart0Nodes.icc[0][0];
      break;
    case t_OLD:
      lav = 3 * data + 3;
      nodeTab = (HANDLE_HUFF_NODE)&FDK_huffPart0Nodes.old[0][0];
      break;
    case t_IPD:
      if (data == 0)
        data = 3;
      else
        data--;
      lav = 2 * data + 1; /* 1, 3, 5, 7 */
      nodeTab = (HANDLE_HUFF_NODE)&FDK_huffPart0Nodes.ipd[0][0];
      break;
    default:
      assert(0);
      err = HUFFDEC_NOTOK;
      goto bail;
  }

  /* Partition 0 */
  if (p0_data[0] != NULL) {
    if ((err = huff_read(strm, nodeTab, &node)) != HUFFDEC_OK) {
      goto bail;
    }
    *p0_data[0] = -(node + 1);
  }
  if (p0_data[1] != NULL) {
    if ((err = huff_read(strm, nodeTab, &node)) != HUFFDEC_OK) {
      goto bail;
    }
    *p0_data[1] = -(node + 1);
  }

  switch (data_type) {
    case t_CLD:
      switch (lav) {
        case 3:
          nodeTab =
              (HANDLE_HUFF_NODE)&FDK_huffCLDNodes.h2D[dim1][dim2]->lav3[0][0];
          break;
        case 5:
          nodeTab =
              (HANDLE_HUFF_NODE)&FDK_huffCLDNodes.h2D[dim1][dim2]->lav5[0][0];
          break;
        case 7:
          nodeTab =
              (HANDLE_HUFF_NODE)&FDK_huffCLDNodes.h2D[dim1][dim2]->lav7[0][0];
          break;
        case 9:
          nodeTab =
              (HANDLE_HUFF_NODE)&FDK_huffCLDNodes.h2D[dim1][dim2]->lav9[0][0];
          break;
      }
      break;
    case t_ICC:
      switch (lav) {
        case 1:
          nodeTab =
              (HANDLE_HUFF_NODE)&FDK_huffICCNodes.h2D[dim1][dim2]->lav1[0][0];
          break;
        case 3:
          nodeTab =
              (HANDLE_HUFF_NODE)&FDK_huffICCNodes.h2D[dim1][dim2]->lav3[0][0];
          break;
        case 5:
          nodeTab =
              (HANDLE_HUFF_NODE)&FDK_huffICCNodes.h2D[dim1][dim2]->lav5[0][0];
          break;
        case 7:
          nodeTab =
              (HANDLE_HUFF_NODE)&FDK_huffICCNodes.h2D[dim1][dim2]->lav7[0][0];
          break;
      }
      break;
    case t_OLD:
      switch (lav) {
        case 3:
          nodeTab = (HANDLE_HUFF_NODE)&huffOLDNodes.h2D[dim1][dim2]->lav3[0][0];
          break;
        case 6:
          nodeTab = (HANDLE_HUFF_NODE)&huffOLDNodes.h2D[dim1][dim2]->lav6[0][0];
          break;
        case 9:
          nodeTab = (HANDLE_HUFF_NODE)&huffOLDNodes.h2D[dim1][dim2]->lav9[0][0];
          break;
        case 12:
          nodeTab =
              (HANDLE_HUFF_NODE)&huffOLDNodes.h2D[dim1][dim2]->lav12[0][0];
          break;
      }
      break;
    case t_IPD:
      switch (lav) {
        case 1:
          nodeTab =
              (HANDLE_HUFF_NODE)&FDK_huffIPDNodes.h2D[dim1][dim2].lav1[0][0];
          break;
        case 3:
          nodeTab =
              (HANDLE_HUFF_NODE)&FDK_huffIPDNodes.h2D[dim1][dim2].lav3[0][0];
          break;
        case 5:
          nodeTab =
              (HANDLE_HUFF_NODE)&FDK_huffIPDNodes.h2D[dim1][dim2].lav5[0][0];
          break;
        case 7:
          nodeTab =
              (HANDLE_HUFF_NODE)&FDK_huffIPDNodes.h2D[dim1][dim2].lav7[0][0];
          break;
      }
      break;
    default:
      break;
  }

  for (i = 0; i < num_val; i += stride) {
    if ((err = huff_read_2D(strm, nodeTab, out_data[i], &escape)) !=
        HUFFDEC_OK) {
      goto bail;
    }

    if (escape) {
      escIdx[escCntr++] = i;
    } else {
      if (data_type == t_IPD) {
        if ((err = sym_restoreIPD(strm, lav, out_data[i])) != HUFFDEC_OK) {
          goto bail;
        }
      } else {
        if ((err = sym_restore(strm, lav, out_data[i])) != HUFFDEC_OK) {
          goto bail;
        }
      }
    }
  } /* i */

  if (escCntr > 0) {
    if ((err = pcm_decode(strm, esc_data[0], esc_data[1], 0, 2 * escCntr,
                          (2 * lav + 1))) != HUFFDEC_OK) {
      goto bail;
    }

    for (i = 0; i < escCntr; i++) {
      out_data[escIdx[i]][0] = esc_data[0][i] - lav;
      out_data[escIdx[i]][1] = esc_data[1][i] - lav;
    }
  }
bail:
  return err;
}

static ERROR_t huff_decode(HANDLE_FDK_BITSTREAM strm, int8_t* out_data_1,
                           int8_t* out_data_2, DATA_TYPE data_type,
                           DIFF_TYPE diff_type_1, DIFF_TYPE diff_type_2,
                           int32_t num_val, int32_t* cdg_scheme, int32_t ldMode) {
  ERROR_t err = HUFFDEC_OK;
  DIFF_TYPE diff_type;

  int32_t i = 0;
  uint32_t data = 0;

  int8_t pair_vec[28][2];

  int8_t* p0_data_1[2] = {NULL, NULL};
  int8_t* p0_data_2[2] = {NULL, NULL};

  int32_t p0_flag[2];

  int32_t num_val_1_int = num_val;
  int32_t num_val_2_int = num_val;

  int8_t* out_data_1_int = out_data_1;
  int8_t* out_data_2_int = out_data_2;

  int32_t df_rest_flag_1 = 0;
  int32_t df_rest_flag_2 = 0;

  int32_t hufYY1;
  int32_t hufYY2;
  int32_t hufYY;

  /* Coding scheme */
  data = FDKreadBits(strm, 1);
  *cdg_scheme = (data << PAIR_SHIFT);

  if (*cdg_scheme >> PAIR_SHIFT == HUFF_2D) {
    if ((out_data_1 != NULL) && (out_data_2 != NULL) && (ldMode == 0)) {
      data = FDKreadBits(strm, 1);
      *cdg_scheme = (*cdg_scheme | data);
    } else {
      *cdg_scheme = (*cdg_scheme | FREQ_PAIR);
    }
  }

  {
    hufYY1 = diff_type_1;
    hufYY2 = diff_type_2;
  }

  switch (*cdg_scheme >> PAIR_SHIFT) {
    case HUFF_1D:
      p0_flag[0] = (diff_type_1 == DIFF_FREQ);
      p0_flag[1] = (diff_type_2 == DIFF_FREQ);
      if (out_data_1 != NULL) {
        if ((err = huff_dec_1D(strm, data_type, hufYY1, out_data_1,
                               num_val_1_int, p0_flag[0])) != HUFFDEC_OK) {
          goto bail;
        }
      }
      if (out_data_2 != NULL) {
        if ((err = huff_dec_1D(strm, data_type, hufYY2, out_data_2,
                               num_val_2_int, p0_flag[1])) != HUFFDEC_OK) {
          goto bail;
        }
      }

      break; /* HUFF_1D */

    case HUFF_2D:

      switch (*cdg_scheme & PAIR_MASK) {
        case FREQ_PAIR:

          if (out_data_1 != NULL) {
            if (diff_type_1 == DIFF_FREQ) {
              p0_data_1[0] = &out_data_1[0];
              p0_data_1[1] = NULL;

              num_val_1_int -= 1;
              out_data_1_int += 1;
            }
            df_rest_flag_1 = num_val_1_int % 2;
            if (df_rest_flag_1) num_val_1_int -= 1;
            if (num_val_1_int < 0) {
              err = HUFFDEC_NOTOK;
              goto bail;
            }
          }
          if (out_data_2 != NULL) {
            if (diff_type_2 == DIFF_FREQ) {
              p0_data_2[0] = NULL;
              p0_data_2[1] = &out_data_2[0];

              num_val_2_int -= 1;
              out_data_2_int += 1;
            }
            df_rest_flag_2 = num_val_2_int % 2;
            if (df_rest_flag_2) num_val_2_int -= 1;
            if (num_val_2_int < 0) {
              err = HUFFDEC_NOTOK;
              goto bail;
            }
          }

          if (out_data_1 != NULL) {
            if ((err = huff_dec_2D(strm, data_type, hufYY1, FREQ_PAIR, pair_vec,
                                   num_val_1_int, 2, p0_data_1)) !=
                HUFFDEC_OK) {
              goto bail;
            }
            if (df_rest_flag_1) {
              if ((err = huff_dec_1D(strm, data_type, hufYY1,
                                     out_data_1_int + num_val_1_int, 1, 0)) !=
                  HUFFDEC_OK) {
                goto bail;
              }
            }
          }
          if (out_data_2 != NULL) {
            if ((err = huff_dec_2D(strm, data_type, hufYY2, FREQ_PAIR,
                                   pair_vec + 1, num_val_2_int, 2,
                                   p0_data_2)) != HUFFDEC_OK) {
              goto bail;
            }
            if (df_rest_flag_2) {
              if ((err = huff_dec_1D(strm, data_type, hufYY2,
                                     out_data_2_int + num_val_2_int, 1, 0)) !=
                  HUFFDEC_OK) {
                goto bail;
              }
            }
          }

          if (out_data_1 != NULL) {
            for (i = 0; i < num_val_1_int - 1; i += 2) {
              out_data_1_int[i] = pair_vec[i][0];
              out_data_1_int[i + 1] = pair_vec[i][1];
            }
          }
          if (out_data_2 != NULL) {
            for (i = 0; i < num_val_2_int - 1; i += 2) {
              out_data_2_int[i] = pair_vec[i + 1][0];
              out_data_2_int[i + 1] = pair_vec[i + 1][1];
            }
          }
          break; /* FREQ_PAIR */

        case TIME_PAIR:
          if (((diff_type_1 == DIFF_FREQ) || (diff_type_2 == DIFF_FREQ))) {
            p0_data_1[0] = &out_data_1[0];
            p0_data_1[1] = &out_data_2[0];

            out_data_1_int += 1;
            out_data_2_int += 1;

            num_val_1_int -= 1;
          }

          if ((diff_type_1 == DIFF_TIME) || (diff_type_2 == DIFF_TIME)) {
            diff_type = DIFF_TIME;
          } else {
            diff_type = DIFF_FREQ;
          }
          { hufYY = diff_type; }

          if ((err = huff_dec_2D(strm, data_type, hufYY, TIME_PAIR, pair_vec,
                                 num_val_1_int, 1, p0_data_1)) != HUFFDEC_OK) {
            goto bail;
          }

          for (i = 0; i < num_val_1_int; i++) {
            out_data_1_int[i] = pair_vec[i][0];
            out_data_2_int[i] = pair_vec[i][1];
          }

          break; /* TIME_PAIR */

        default:
          break;
      }

      break; /* HUFF_2D */

    default:
      break;
  }
bail:
  return err;
}

static void diff_freq_decode(const int8_t* const diff_data,
                             int8_t* const out_data, const int32_t num_val) {
  int32_t i = 0;
  out_data[0] = diff_data[0];

  for (i = 1; i < num_val; i++) {
    out_data[i] = out_data[i - 1] + diff_data[i];
  }
}

static void diff_time_decode_backwards(const int8_t* const prev_data,
                                       const int8_t* const diff_data,
                                       int8_t* const out_data,
                                       const int32_t mixed_diff_type,
                                       const int32_t num_val) {
  int32_t i = 0; /* default start value*/

  if (mixed_diff_type) {
    out_data[0] = diff_data[0];
    i = 1; /* new start value */
  }
  for (; i < num_val; i++) {
    out_data[i] = prev_data[i] + diff_data[i];
  }
}

static void diff_time_decode_forwards(const int8_t* const prev_data,
                                      const int8_t* const diff_data,
                                      int8_t* const out_data,
                                      const int32_t mixed_diff_type,
                                      const int32_t num_val) {
  int32_t i = 0; /* default start value*/

  if (mixed_diff_type) {
    out_data[0] = diff_data[0];
    i = 1; /* new start value */
  }
  for (; i < num_val; i++) {
    out_data[i] = prev_data[i] - diff_data[i];
  }
}

static ERROR_t attach_lsb(HANDLE_FDK_BITSTREAM strm, int8_t* in_data_msb,
                          int32_t offset, int32_t num_lsb, int32_t num_val,
                          int8_t* out_data) {
  int32_t i = 0, lsb = 0;
  uint32_t data = 0;

  for (i = 0; i < num_val; i++) {
    int32_t msb;
    msb = in_data_msb[i];

    if (num_lsb > 0) {
      data = FDKreadBits(strm, num_lsb);
      lsb = data;

      out_data[i] = ((msb << num_lsb) | lsb) - offset;
    } else
      out_data[i] = msb - offset;
  }

  return HUFFDEC_OK; /* dummy */
}

ERROR_t EcDataPairDec(DECODER_TYPE DECODER, HANDLE_FDK_BITSTREAM strm,
                      int8_t* aaOutData1, int8_t* aaOutData2, int8_t* aHistory,
                      DATA_TYPE data_type, int32_t startBand, int32_t dataBands,
                      int32_t pair_flag, int32_t coarse_flag,
                      int32_t allowDiffTimeBack_flag)

{
  ERROR_t err = HUFFDEC_OK;

  // int32_t allowDiffTimeBack_flag = !independency_flag || (setIdx > 0);
  int32_t attachLsb_flag = 0;
  int32_t pcmCoding_flag = 0;

  int32_t mixed_time_pair = 0, numValPcm = 0;
  int32_t quant_levels = 0, quant_offset = 0;
  uint32_t data = 0;

  int8_t aaDataPair[2][28] = {{0}};
  int8_t aaDataDiff[2][28] = {{0}};

  int8_t aHistoryMsb[28] = {0};

  int8_t* pDataVec[2] = {NULL, NULL};

  DIFF_TYPE diff_type[2] = {DIFF_FREQ, DIFF_FREQ};
  int32_t cdg_scheme = HUFF_1D;
  DIRECTION direction = BACKWARDS;

  switch (data_type) {
    case t_CLD:
      if (coarse_flag) {
        attachLsb_flag = 0;
        quant_levels = 15;
        quant_offset = 7;
      } else {
        attachLsb_flag = 0;
        quant_levels = 31;
        quant_offset = 15;
      }

      break;

    case t_ICC:
      if (coarse_flag) {
        attachLsb_flag = 0;
        quant_levels = 4;
        quant_offset = 0;
      } else {
        attachLsb_flag = 0;
        quant_levels = 8;
        quant_offset = 0;
      }

      break;

    case t_OLD:
      if (coarse_flag) {
        attachLsb_flag = 0;
        quant_levels = 8;
        quant_offset = 0;
      } else {
        attachLsb_flag = 0;
        quant_levels = 16;
        quant_offset = 0;
      }
      break;

    case t_NRG:
      if (coarse_flag) {
        attachLsb_flag = 0;
        quant_levels = 32;
        quant_offset = 0;
      } else {
        attachLsb_flag = 0;
        quant_levels = 64;
        quant_offset = 0;
      }
      break;

    case t_IPD:
      if (!coarse_flag) {
        attachLsb_flag = 1;
        quant_levels = 16;
        quant_offset = 0;
      } else {
        attachLsb_flag = 0;
        quant_levels = 8;
        quant_offset = 0;
      }
      break;

    default:
      return HUFFDEC_NOTOK;
  }

  data = FDKreadBits(strm, 1);
  pcmCoding_flag = data;

  if (pcmCoding_flag) {
    if (pair_flag) {
      pDataVec[0] = aaDataPair[0];
      pDataVec[1] = aaDataPair[1];
      numValPcm = 2 * dataBands;
    } else {
      pDataVec[0] = aaDataPair[0];
      pDataVec[1] = NULL;
      numValPcm = dataBands;
    }

    err = pcm_decode(strm, pDataVec[0], pDataVec[1], quant_offset, numValPcm,
                     quant_levels);
    if (err != HUFFDEC_OK) return HUFFDEC_NOTOK;

  } else { /* Differential/Huffman/LSB Coding */

    if (pair_flag) {
      pDataVec[0] = aaDataDiff[0];
      pDataVec[1] = aaDataDiff[1];
    } else {
      pDataVec[0] = aaDataDiff[0];
      pDataVec[1] = NULL;
    }

    diff_type[0] = DIFF_FREQ;
    diff_type[1] = DIFF_FREQ;

    direction = BACKWARDS;
    {
      if (pair_flag || allowDiffTimeBack_flag) {
        data = FDKreadBits(strm, 1);
        diff_type[0] = (DIFF_TYPE)data;
      }

      if (pair_flag &&
          ((diff_type[0] == DIFF_FREQ) || allowDiffTimeBack_flag)) {
        data = FDKreadBits(strm, 1);
        diff_type[1] = (DIFF_TYPE)data;
      }
    }
    /* Huffman decoding */
    err = huff_decode(strm, pDataVec[0], pDataVec[1], data_type, diff_type[0],
                      diff_type[1], dataBands, &cdg_scheme,
                      (DECODER == SAOC_DECODER));
    if (err != HUFFDEC_OK) {
      return HUFFDEC_NOTOK;
    }

    {
      /* Differential decoding */
      if ((diff_type[0] == DIFF_TIME) || (diff_type[1] == DIFF_TIME)) {
        if (DECODER == SAOC_DECODER) {
          direction = BACKWARDS;
        } else {
          if (pair_flag) {
            if ((diff_type[0] == DIFF_TIME) && !allowDiffTimeBack_flag) {
              direction = FORWARDS;
            } else if (diff_type[1] == DIFF_TIME) {
              direction = BACKWARDS;
            } else {
              data = FDKreadBits(strm, 1);
              direction = (DIRECTION)data;
            }
          } else {
            direction = BACKWARDS;
          }
        }
      }

      mixed_time_pair = (diff_type[0] != diff_type[1]) &&
                        ((cdg_scheme & PAIR_MASK) == TIME_PAIR);

      if (direction == BACKWARDS) {
        if (diff_type[0] == DIFF_FREQ) {
          diff_freq_decode(aaDataDiff[0], aaDataPair[0], dataBands);
        } else {
          int32_t i;
          for (i = 0; i < dataBands; i++) {
            aHistoryMsb[i] = aHistory[i + startBand] + quant_offset;
            if (attachLsb_flag) {
              aHistoryMsb[i] >>= 1;
            }
          }
          diff_time_decode_backwards(aHistoryMsb, aaDataDiff[0], aaDataPair[0],
                                     mixed_time_pair, dataBands);
        }
        if (diff_type[1] == DIFF_FREQ) {
          diff_freq_decode(aaDataDiff[1], aaDataPair[1], dataBands);
        } else {
          diff_time_decode_backwards(aaDataPair[0], aaDataDiff[1],
                                     aaDataPair[1], mixed_time_pair, dataBands);
        }
      } else {
        /* diff_type[1] MUST BE DIFF_FREQ */
        diff_freq_decode(aaDataDiff[1], aaDataPair[1], dataBands);

        if (diff_type[0] == DIFF_FREQ) {
          diff_freq_decode(aaDataDiff[0], aaDataPair[0], dataBands);
        } else {
          diff_time_decode_forwards(aaDataPair[1], aaDataDiff[0], aaDataPair[0],
                                    mixed_time_pair, dataBands);
        }
      }
    }

    /* LSB decoding */
    err = attach_lsb(strm, aaDataPair[0], quant_offset, attachLsb_flag ? 1 : 0,
                     dataBands, aaDataPair[0]);
    if (err != HUFFDEC_OK) goto bail;

    if (pair_flag) {
      err = attach_lsb(strm, aaDataPair[1], quant_offset,
                       attachLsb_flag ? 1 : 0, dataBands, aaDataPair[1]);
      if (err != HUFFDEC_OK) goto bail;
    }
  } /* End: Differential/Huffman/LSB Coding */

  /* Copy data to output arrays */
  memcpy(aaOutData1 + startBand, aaDataPair[0], sizeof(int8_t) * dataBands);
  if (pair_flag) {
    memcpy(aaOutData2 + startBand, aaDataPair[1], sizeof(int8_t) * dataBands);
  }

bail:
  return err;
}

ERROR_t huff_dec_reshape(HANDLE_FDK_BITSTREAM strm, int32_t* out_data,
                         int32_t num_val) {
  ERROR_t err = HUFFDEC_OK;
  int32_t val_rcvd = 0, dummy = 0, i = 0, val = 0, len = 0;
  int8_t rl_data[2] = {0};

  while (val_rcvd < num_val) {
    err = huff_read_2D(strm,
                       (HANDLE_HUFF_NODE)&FDK_huffReshapeNodes.nodeTab[0][0],
                       rl_data, &dummy);
    if (err != HUFFDEC_OK) goto bail;
    val = rl_data[0];
    len = rl_data[1] + 1;
    if (val_rcvd + len > num_val) {
      err = HUFFDEC_NOTOK;
      goto bail;
    }
    for (i = val_rcvd; i < val_rcvd + len; i++) {
      out_data[i] = val;
    }
    val_rcvd += len;
  }
bail:
  return err;
}
