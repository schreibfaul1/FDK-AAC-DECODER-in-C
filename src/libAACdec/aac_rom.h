

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: Definition of constant tables

*******************************************************************************/

#ifndef AAC_ROM_H
#define AAC_ROM_H

#include <stdint.h>
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"
#include "aacdec_hcr_types.h"
#include "aacdec_hcrs.h"


#define FIXP_DBL2PCM_DEC(x) (x)
#define PCM_DEC2FIXP_DBL(x) (x)
#define PCM_DEC_BITS DFRACT_BITS
#define PCM_DEC2FX_PCM(x) FX_DBL2FX_SGL(x)(x)
#define FX_PCM2PCM_DEC(x) FX_SGL2FX_DBL((int16_t)(x))(x)

#define AACDEC_MAX_CH_CONF 14
#define AACDEC_CH_ELEMENTS_TAB_SIZE 7 /*!< Size of element tables */

#define AAC_NF_NO_RANDOM_VAL \
  512 /*!< Size of random number array for noise floor */

#define INV_QUANT_TABLESIZE 256

extern const int32_t InverseQuantTable[INV_QUANT_TABLESIZE + 1];
extern const int32_t MantissaTable[4][14];
extern const int8_t ExponentTable[4][14];

#define NUM_LD_COEF_512 1536
#define NUM_LD_COEF_480 1440
/* Window table partition exponents. */
#define WTS0 (1)
#define WTS1 (0)
#define WTS2 (-2)
extern const int16_t LowDelaySynthesis512[1536];
extern const int16_t LowDelaySynthesis480[1440];
extern const int16_t LowDelaySynthesis256[768];
extern const int16_t LowDelaySynthesis240[720];
extern const int16_t LowDelaySynthesis160[480];
extern const int16_t LowDelaySynthesis128[384];
extern const int16_t LowDelaySynthesis120[360];

typedef struct {
  const int16_t *sfbOffsetLong;
  const int16_t *sfbOffsetShort;
  uint8_t numberOfSfbLong;
  uint8_t numberOfSfbShort;
} SFB_INFO;

extern const SFB_INFO sfbOffsetTables[5][16];

/* Huffman tables */
enum { HuffmanBits = 2, HuffmanEntries = (1 << HuffmanBits) };

typedef struct {
  const uint16_t (*CodeBook)[HuffmanEntries];
  uint8_t Dimension;
  uint8_t numBits;
  uint8_t Offset;
} CodeBookDescription;

extern const CodeBookDescription AACcodeBookDescriptionTable[13];
extern const CodeBookDescription AACcodeBookDescriptionSCL;

extern const STATEFUNC aStateConstant2State[];

extern const int8_t aCodebook2StartInt[];

extern const uint8_t aMinOfCbPair[];
extern const uint8_t aMaxOfCbPair[];

extern const uint8_t aMaxCwLen[];
extern const uint8_t aDimCb[];
extern const uint8_t aDimCbShift[];
extern const uint8_t aSignCb[];
extern const uint8_t aCbPriority[];

extern const uint32_t *aHuffTable[];
extern const int8_t *aQuantTable[];

extern const uint16_t aLargestAbsoluteValue[];

extern const uint32_t aHuffTreeRvlcEscape[];
extern const uint32_t aHuffTreeRvlCodewds[];

extern const uint8_t tns_max_bands_tbl[13][2];

extern const uint8_t tns_max_bands_tbl_480[13];
extern const uint8_t tns_max_bands_tbl_512[13];

#define FIXP_TCC int32_t

extern const FIXP_TCC FDKaacDec_tnsCoeff3[8];
extern const FIXP_TCC FDKaacDec_tnsCoeff4[16];

extern const uint8_t FDKaacDec_tnsCoeff3_gain_ld[];
extern const uint8_t FDKaacDec_tnsCoeff4_gain_ld[];

extern const uint16_t AacDec_randomSign[AAC_NF_NO_RANDOM_VAL / 16];

extern const int32_t pow2_div24minus1[47];
extern const int32_t offsetTab[2][16];

/* Channel mapping indices for time domain I/O.
   The first dimension is the channel configuration index. */
extern const uint8_t channelMappingTablePassthrough[15][8];
extern const uint8_t channelMappingTableWAV[15][8];

/* Lookup tables for elements in ER bitstream */
extern const MP4_ELEMENT_ID_t elementsTab[AACDEC_MAX_CH_CONF]
                                       [AACDEC_CH_ELEMENTS_TAB_SIZE];

#define SF_FNA_COEFFS \
  1 /* Compile-time prescaler for MDST-filter coefficients. */
/* SF_FNA_COEFFS > 0 should only be considered for int32_t-coefficients  */
/* (i.e. if CPLX_PRED_FILTER_16BIT is not defined).                       */
/* With int32_t loss of precision is possible for SF_FNA_COEFFS > 11.    */


#define FIXP_FILT int32_t
#define FILT(a) ((FL2FXCONST_DBL(a)) >> SF_FNA_COEFFS)


extern const FIXP_FILT mdst_filt_coef_curr[20][3]; /* MDST-filter coefficient
                                                      tables used for current
                                                      window  */
extern const FIXP_FILT mdst_filt_coef_prev[6][4];  /* MDST-filter coefficient
                                                      tables used for previous
                                                      window */

#endif /* #ifndef AAC_ROM_H */
