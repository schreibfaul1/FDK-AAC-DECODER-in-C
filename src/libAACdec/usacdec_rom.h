

/**************************** AAC decoder library ******************************

   Author(s):   M. Jander

   Description: re8.h

*******************************************************************************/

#ifndef USACDEC_ROM_H
#define USACDEC_ROM_H
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"
#include "../libFDK/FDK_lpc.h"

#include "usacdec_const.h"

/* RE8 lattice quantiser constants */
#define NB_SPHERE 32
#define NB_LEADER 37
#define NB_LDSIGN 226
#define NB_LDQ3 9
#define NB_LDQ4 28

#define LSF_SCALE 13

/* RE8 lattice quantiser tables */
extern const uint32_t fdk_dec_tab_factorial[8];
extern const uint8_t fdk_dec_Ia[NB_LEADER];
extern const uint8_t fdk_dec_Ds[NB_LDSIGN];
extern const uint16_t fdk_dec_Is[NB_LDSIGN];
extern const uint8_t fdk_dec_Ns[], fdk_dec_A3[], fdk_dec_A4[];
extern const uint8_t fdk_dec_Da[][8];
extern const uint16_t fdk_dec_I3[], fdk_dec_I4[];

/* temp float tables for LPC decoding */
extern const FIXP_LPC fdk_dec_lsf_init[16];
extern const FIXP_LPC fdk_dec_dico_lsf_abs_8b[16 * 256];

/* ACELP tables */
#define SF_QUA_GAIN7B 4
extern const int16_t fdk_t_qua_gain7b[128 * 2];
extern const int16_t lsp_interpol_factor[2][NB_SUBFR];

/* For bass post filter */
#define L_FILT 12 /* Delay of up-sampling filter                */

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

#endif /* USACDEC_ROM_H */
