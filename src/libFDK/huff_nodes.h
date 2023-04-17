

/******************* Library for basic calculation routines ********************

   Author(s):   Omer Osman

   Description: MPEG-D SAC/USAC/SAOC Huffman Part0 Tables

*******************************************************************************/

#ifndef HUFF_NODES_H
#define HUFF_NODES_H

#include <stdint.h>
#include "../libAACdec/newAACDecoder.h"
typedef struct {
  int16_t nodeTab[39][2];

} HUFF_RES_NODES;

/* 1D Nodes */
typedef struct {
  int16_t nodeTab[30][2];

} HUFF_CLD_NOD_1D;

typedef struct {
  int16_t nodeTab[7][2];

} HUFF_ICC_NOD_1D;

typedef struct {
  int16_t nodeTab[50][2];

} HUFF_CPC_NOD_1D;

typedef struct {
  int16_t nodeTab[15][2];

} HUFF_OLD_NOD_1D;

typedef struct {
  int16_t nodeTab[63][2];

} HUFF_NRG_NOD_1D;

/* 2D Nodes */
typedef struct {
  int16_t lav3[15][2];
  int16_t lav5[35][2];
  int16_t lav7[63][2];
  int16_t lav9[99][2];

} HUFF_CLD_NOD_2D;

typedef struct {
  int16_t lav1[3][2];
  int16_t lav3[15][2];
  int16_t lav5[35][2];
  int16_t lav7[63][2];

} HUFF_ICC_NOD_2D;

typedef struct {
  int16_t lav3[15][2];
  int16_t lav6[48][2];
  int16_t lav9[99][2];
  int16_t lav12[168][2];

} HUFF_OLD_NOD_2D;

typedef struct {
  int16_t lav3[15][2];
  int16_t lav5[35][2];
  int16_t lav7[63][2];
  int16_t lav9[99][2];

} HUFF_NRG_NOD_2D_df;

typedef struct {
  int16_t lav3[15][2];
  int16_t lav6[48][2];
  int16_t lav9[99][2];
  int16_t lav12[168][2];

} HUFF_NRG_NOD_2D_dt;

typedef struct {
  HUFF_NRG_NOD_2D_df df[2];
  HUFF_NRG_NOD_2D_dt dt[2];
  HUFF_NRG_NOD_2D_df dp[2];

} HUFF_NRG_NOD_2D;

/* Complete bs Parameter Nodes */
typedef struct {
  const HUFF_CLD_NOD_1D *h1D[3];
  const HUFF_CLD_NOD_2D *h2D[3][2];

} HUFF_CLD_NODES;

typedef struct {
  const HUFF_ICC_NOD_1D *h1D[3];
  const HUFF_ICC_NOD_2D *h2D[3][2];

} HUFF_ICC_NODES;

typedef struct {
  const HUFF_OLD_NOD_1D *h1D[3];
  const HUFF_OLD_NOD_2D *h2D[3][2];

} HUFF_OLD_NODES;

typedef struct {
  const HUFF_NRG_NOD_1D *h1D[3];
  const HUFF_NRG_NOD_2D *h2D;

} HUFF_NRG_NODES;

/* parameter instance */
typedef struct {
  int16_t cld[30][2];
  int16_t icc[7][2];
  int16_t ipd[7][2];
  int16_t old[15][2];
  int16_t nrg[63][2];
} HUFF_PT0_NODES;

typedef struct {
  int16_t nodeTab[3][2];

} HUFF_LAV_NODES;

/* USAC specific */
typedef struct {
  int16_t nodeTab[7][2];

} HUFF_IPD_NOD_1D;

typedef struct {
  int16_t lav1[3][2];
  int16_t lav3[15][2];
  int16_t lav5[35][2];
  int16_t lav7[63][2];

} HUFF_IPD_NOD_2D;

typedef struct {
  HUFF_IPD_NOD_1D h1D[3];
  HUFF_IPD_NOD_2D h2D[3][2];

} HUFF_IPD_NODES;

/* non-lossy coding decoder */
extern const HUFF_PT0_NODES FDK_huffPart0Nodes;
extern const HUFF_LAV_NODES FDK_huffLavIdxNodes;

extern const HUFF_ICC_NODES FDK_huffICCNodes;
extern const HUFF_CLD_NODES FDK_huffCLDNodes;
extern const HUFF_RES_NODES FDK_huffReshapeNodes;

extern const HUFF_OLD_NODES huffOLDNodes;

extern const HUFF_IPD_NODES FDK_huffIPDNodes;

#endif /* HUFF_NODES_H */
