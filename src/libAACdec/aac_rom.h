

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

extern const int32_t InverseQuantTable[INV_QUANT_TABLESIZE + 1];
extern const int32_t MantissaTable[4][14];
extern const int8_t ExponentTable[4][14];
extern const int16_t LowDelaySynthesis512[1536];
extern const int16_t LowDelaySynthesis480[1440];
extern const int16_t LowDelaySynthesis256[768];
extern const int16_t LowDelaySynthesis240[720];
extern const int16_t LowDelaySynthesis160[480];
extern const int16_t LowDelaySynthesis128[384];
extern const int16_t LowDelaySynthesis120[360];
extern const SFB_INFO_t sfbOffsetTables[5][16];
extern const CodeBookDescription_t AACcodeBookDescriptionTable[13];
extern const CodeBookDescription_t AACcodeBookDescriptionSCL;
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
extern const int32_t FDKaacDec_tnsCoeff3[8];
extern const int32_t FDKaacDec_tnsCoeff4[16];
extern const uint8_t FDKaacDec_tnsCoeff3_gain_ld[];
extern const uint8_t FDKaacDec_tnsCoeff4_gain_ld[];
extern const uint16_t AacDec_randomSign[AAC_NF_NO_RANDOM_VAL / 16];
extern const int32_t pow2_div24minus1[47];
extern const int32_t offsetTab[2][16];
extern const uint8_t channelMappingTablePassthrough[15][8];
extern const uint8_t channelMappingTableWAV[15][8];
extern const MP4_ELEMENT_ID_t elementsTab[AACDEC_MAX_CH_CONF] [AACDEC_CH_ELEMENTS_TAB_SIZE];
extern const int32_t mdst_filt_coef_curr[20][3]; /* MDST-filter coefficient tables used for current window  */
extern const int32_t mdst_filt_coef_prev[6][4];  /* MDST-filter coefficient tables used for previous window */
extern void DecodeNonPCWs(HANDLE_FDK_BITSTREAM bs, H_HCR_INFO hHcr);
extern const uint32_t fdk_dec_tab_factorial[8];
extern const uint8_t fdk_dec_Ia[NB_LEADER];
extern const uint8_t fdk_dec_Ds[NB_LDSIGN];
extern const uint16_t fdk_dec_Is[NB_LDSIGN];
extern const uint8_t fdk_dec_Ns[], fdk_dec_A3[], fdk_dec_A4[];
extern const uint8_t fdk_dec_Da[][8];
extern const uint16_t fdk_dec_I3[], fdk_dec_I4[];
extern const int16_t fdk_dec_lsf_init[16];
extern const int16_t fdk_dec_dico_lsf_abs_8b[16 * 256];
extern const int16_t fdk_t_qua_gain7b[128 * 2];
extern const int16_t lsp_interpol_factor[2][NB_SUBFR];
extern const int16_t fdk_dec_filt_lp[1 + L_FILT];
extern const int16_t FacWindowSynth128[128];
extern const int16_t FacWindowZir128[128];
extern const int16_t FacWindowSynth64[64];
extern const int16_t FacWindowZir64[64];
extern const int16_t FacWindowSynth32[32];
extern const int16_t FacWindowZir32[32];
extern const int16_t FacWindowSynth96[96];
extern const int16_t FacWindowZir96[96];
extern const int16_t FacWindowSynth48[48];
extern const int16_t FacWindowZir48[48];
extern const int32_t Pred_lt4_inter4_2[UP_SAMP][L_INTERPOL2];

#endif /* #ifndef AAC_ROM_H */
