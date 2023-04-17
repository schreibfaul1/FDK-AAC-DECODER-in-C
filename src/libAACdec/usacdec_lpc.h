

/**************************** AAC decoder library ******************************

   Author(s):   Matthias Hildenbrand, Manuel Jander

   Description: USAC LPC/AVQ decode

*******************************************************************************/

#ifndef USACDEC_LPC_H
#define USACDEC_LPC_H
#include "newAACDecoder.h"
#include "channelinfo.h"
#include "../libFDK/common_fix.h"
#include "../libFDK/FDK_bitstream.h"
#include "usacdec_rom.h"

#define LSPARG_SCALE 10

/**
 * \brief AVQ (refinement) decode
 * \param hBs bitstream handle
 * \param lsfq buffer for AVQ decode output.
 * \param nk_mode quantization mode.
 * \param nqn amount of split/interleaved RE8 vectors.
 * \param total amount of individual data values to decode.
 * \return 0 on success, -1 on error.
 */
int32_t CLpc_DecodeAVQ(HANDLE_FDK_BITSTREAM hBs, int32_t *lsfq, int32_t nk_mode,
                   int32_t nqn, int32_t length);

/**
 * \brief Read and decode LPC coeficient sets. First stage approximation + AVQ
 * decode.
 * \param[in]  hBs bitstream handle to read data from.
 * \param[out] lsp buffer into which the decoded LSP coefficients will be stored
 * into.
 * \param[in,out] lpc4_lsf buffer into which the decoded LCP4 LSF coefficients
 * will be stored into (persistent).
 * \param[out] lsf_adaptive_mean_cand lsf adaptive mean vector needed for
 * concealment.
 * \param[out] pStability array with stability values for the ACELP decoder (and
 * concealment).
 * \param[in]  mod array which defines modes (ACELP, TCX20|40|80) are used in
 * the current superframe.
 * \param[in]  first_lpd_flag indicates the presence of LPC0
 * \param[in]  last_lpc_lost indicate that LPC4 of previous frame was lost.
 * \param[in]  last_frame_ok indicate that the last frame was ok.
 * \return 0 on success, -1 on error.
 */
int32_t CLpc_Read(HANDLE_FDK_BITSTREAM hBs, int16_t lsp[][M_LP_FILTER_ORDER],
              int16_t lpc4_lsf[M_LP_FILTER_ORDER],
              int16_t lsf_adaptive_mean_cand[M_LP_FILTER_ORDER],
              int16_t pStability[], uint8_t *mod, int32_t first_lpd_flag,
              int32_t last_lpc_lost, int32_t last_frame_ok);

/**
 * \brief Generate LPC coefficient sets in case frame loss.
 * \param lsp buffer into which the decoded LSP coefficients will be stored
 * into.
 * \param lpc4_lsf buffer into which the decoded LCP4 LSF coefficients will be
 * stored into (persistent).
 * \param isf_adaptive_mean
 * \param first_lpd_flag indicates the previous LSF4 coefficients lpc4_lsf[] are
 * not valid.
 */
void CLpc_Conceal(int16_t lsp[][M_LP_FILTER_ORDER],
                  int16_t lpc4_lsf[M_LP_FILTER_ORDER],
                  int16_t isf_adaptive_mean[M_LP_FILTER_ORDER],
                  const int32_t first_lpd_flag);

/**
 * \brief apply absolute weighting
 * \param A weighted LPC coefficient vector output. The first coeffcient is
 * implicitly 1.0
 * \param A LPC coefficient vector. The first coeffcient is implicitly 1.0
 * \param m length of vector A
 */
/* static */
void E_LPC_a_weight(int16_t *wA, const int16_t *A, const int32_t m);

/**
 * \brief decode TCX/FAC gain. In case of TCX the lg/sqrt(rms) part
 *        must still be applied to obtain the gain value.
 * \param gain (o) pointer were the gain mantissa is stored into.
 * \param gain_e (o) pointer were the gain exponent is stored into.
 * \param gain_code (i) the 7 bit binary word from the bitstream
 *                      representing the gain.
 */
void CLpd_DecodeGain(int32_t *gain, int32_t *gain_e, int32_t gain_code);

/**
 * \brief convert LSP coefficients into LP domain.
 */
void E_LPC_f_lsp_a_conversion(int16_t *lsp, int16_t *a, int32_t *a_exp);

#endif /* USACDEC_LPC_H */
