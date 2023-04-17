

/**************************** AAC decoder library ******************************

   Author(s):   Manuel Jander

   Description: USAC Linear Prediction Domain coding

*******************************************************************************/

#ifndef USACDEC_LPD_H
#define USACDEC_LPD_H
#include "newAACDecoder.h"
#include "channelinfo.h"




static inline int32_t CLpd_FAC_getLength(int32_t fNotShortBlock, int32_t fac_length_long) {
  if (fNotShortBlock) {
    return (fac_length_long);
  } else {
    return fac_length_long / 2;
  }
}

void filtLP(const int32_t *syn, int32_t *syn_out, int32_t *noise,
            const int16_t *filt, const int32_t aacOutDataHeadroom, int32_t stop,
            int32_t len);

/**
 * \brief perform a low-frequency pitch enhancement on time domain signal
 * \param[in] syn pointer to time domain input signal
 * \param[in] synFB pointer to time domain input signal
 * \param[in] upsampling factor
 * \param[in] T_sf array with past decoded pitch period values for each subframe
 * \param[in] non_zero_gain_flags indicates whether pitch gains of past
 * subframes are zero or not, msb -> [1 BPF_DELAY subfr][7 SYN_DELAY subfr][16
 * new subfr] <- lsb
 * \param[in] l_frame length of filtering, must be multiple of L_SUBFR
 * \param[in] l_next length of allowed look ahead on syn[i], i < l_frame+l_next
 * \param[out] synth_out pointer to time domain output signal
 * \param[in] headroom of the output time signal to prevent clipping
 * \param[in,out] mem_bpf pointer to filter memory (L_FILT+L_SUBFR)
 */

void bass_pf_1sf_delay(int32_t syn[], const int32_t T_sf[], int32_t *pit_gain,
                       const int32_t frame_length, const int32_t l_frame,
                       const int32_t l_next, int32_t *synth_out,
                       const int32_t aacOutDataHeadroom, int32_t mem_bpf[]);

/**
 * \brief random sign generator for FD and TCX noise filling
 * \param[in,out] seed pointer to random seed
 * \return if return value is zero use positive sign
 * \Note: This code is also implemented as a copy in block.cpp, grep for
 * "UsacRandomSign"
 */
static inline
int32_t UsacRandomSign(uint32_t *seed) {
  *seed = (uint32_t)((uint64_t)(*seed) * 69069 + 5);

  return (int32_t)((*seed) & 0x10000);
}

void CFdp_Reset(CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo);

#endif /* USACDEC_LPD_H */
