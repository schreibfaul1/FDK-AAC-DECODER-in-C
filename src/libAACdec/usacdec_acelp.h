

/**************************** AAC decoder library ******************************

   Author(s):   Matthias Hildenbrand

   Description: USAC ACELP frame decoder

*******************************************************************************/

#ifndef USACDEC_ACELP_H
#define USACDEC_ACELP_H
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"
#include "../libFDK/FDK_bitstream.h"
#include "usacdec_const.h"


//#define ENHANCED_TCX_TD_CONCEAL_ENABLE

/** Structure which holds the ACELP internal persistent memory */


/** Structure which holds the parameter data needed to decode one ACELP frame.
 */

/**
 * \brief Read the acelp_coding() bitstream part.
 * \param[in] hBs bitstream handle to read data from.
 * \param[out] acelpData pointer to structure to store the parsed data of one
 * ACELP frame.
 * \param[in] acelp_core_mode the ACELP core mode index.
 * \param[in] coreCoderFrameLength length of core coder frame (1024|768)
 */
int32_t CLpd_AcelpRead(HANDLE_FDK_BITSTREAM hBs, CAcelpChannelData_t *acelpData,
                   int32_t acelp_core_mode, int32_t i_offset, int32_t coreCoderFrameLength);
/**
 * \brief Initialization of memory before one LPD frame is decoded
 * \param[out] synth_buf synthesis buffer to be initialized, exponent = SF_SYNTH
 * \param[in] old_synth past synthesis of previous LPD frame, exponent =
 * SF_SYNTH
 * \param[out] synth_buf_fb fullband synthesis buffer to be initialized,
 * exponent = SF_SYNTH
 * \param[in] old_synth_fb past fullband synthesis of previous LPD frame,
 * exponent = SF_SYNTH
 * \param[out] pitch vector where decoded pitch lag values are stored
 * \param[in] old_T_pf past pitch lag values of previous LPD frame
 * \param[in] samplingRate sampling rate for pitch lag offset calculation
 * \param[out] i_offset pitch lag offset for the decoding of the pitch lag
 * \param[in] coreCoderFrameLength length of core coder frame (1024|768)
 */
void Acelp_PreProcessing(int32_t *synth_buf, int32_t *old_synth, int32_t *pitch,
                         int32_t *old_T_pf, int32_t *pit_gain,
                         int32_t *old_gain_pf, int32_t samplingRate, int32_t *i_offset,
                         int32_t coreCoderFrameLength, int32_t synSfd,
                         int32_t nbSubfrSuperfr);

/**
 * \brief Save tail of buffers for the initialization of the next LPD frame
 * \param[in] synth_buf synthesis of current LPD frame, exponent = SF_SYNTH
 * \param[out] old_synth memory where tail of fullband synth_buf is stored,
 * exponent = SF_SYNTH
 * \param[in] synth_buf_fb fullband synthesis of current LPD frame, exponent =
 * SF_SYNTH
 * \param[out] old_synth_fb memory where tail of fullband synth_buf is stored,
 * exponent = SF_SYNTH
 * \param[in] pitch decoded pitch lag values of current LPD frame
 * \param[out] old_T_pf memory where last SYN_SFD pitch lag values are stored
 */
void Acelp_PostProcessing(int32_t *synth_buf, int32_t *old_synth, int32_t *pitch,
                          int32_t *old_T_pf, int32_t coreCoderFrameLength, int32_t synSfd,
                          int32_t nbSubfrSuperfr);

/**
 * \brief Decode one ACELP frame (three or four ACELP subframes with 64 samples
 * each)
 * \param[in,out] acelp_mem pointer to ACELP memory structure
 * \param[in] i_offset pitch lag offset
 * \param[in] lsp_old LPC filter in LSP domain corresponding to previous frame
 * \param[in] lsp_new LPC filter in LSP domain corresponding to current frame
 * \param[in] stab_fac stability factor constrained by 0<=stab_fac<=1.0,
 * exponent = SF_STAB
 * \param[in] acelpData pointer to struct with data which is needed for decoding
 * one ACELP frame
 * \param[out] synth ACELP output signal
 * \param[out] pT four decoded pitch lag values
 * \param[in] coreCoderFrameLength length of core coder frame (1024|768)
 */
void CLpd_AcelpDecode(CAcelpStaticMem_t *acelp_mem, int32_t i_offset,
                      const int16_t lsp_old[M_LP_FILTER_ORDER],
                      const int16_t lsp_new[M_LP_FILTER_ORDER],
                      int16_t stab_fac, CAcelpChannelData_t *acelpData,
                      int32_t numLostSubframes, int32_t lastLpcLost, int32_t frameCnt,
                      int32_t synth[], int32_t pT[], int32_t *pit_gain,
                      int32_t coreCoderFrameLength);

/**
 * \brief Reset ACELP internal memory.
 * \param[out] acelp_mem pointer to ACELP memory structure
 */
void CLpd_AcelpReset(CAcelpStaticMem_t *acelp_mem);

/**
 * \brief Initialize ACELP internal memory in case of FAC before ACELP decoder
 * is called
 * \param[in] synth points to end+1 of past valid synthesis signal, exponent =
 * SF_SYNTH
 * \param[in] last_lpd_mode last lpd mode
 * \param[in] last_last_lpd_mode lpd mode before last_lpd_mode
 * \param[in] A_new LP synthesis filter coeffs corresponding to last frame,
 * exponent = SF_A_COEFFS
 * \param[in] A_old LP synthesis filter coeffs corresponding to the frame before
 * last frame, exponent = SF_A_COEFFS
 * \param[in,out] acelp_mem pointer to ACELP memory structure
 * \param[in] coreCoderFrameLength length of core coder frame (1024|768)
 */
void CLpd_AcelpPrepareInternalMem(const int32_t *synth, uint8_t last_lpd_mode,
                                  uint8_t last_last_lpd_mode,
                                  const int16_t *A_new, const int32_t A_new_exp,
                                  const int16_t *A_old, const int32_t A_old_exp,
                                  CAcelpStaticMem_t *acelp_mem,
                                  int32_t coreCoderFrameLength, int32_t clearOldExc,
                                  uint8_t lpd_mode);

/**
 * \brief Calculate zero input response (zir) of the acelp synthesis filter
 * \param[in] A LP synthesis filter coefficients, exponent = SF_A_COEFFS
 * \param[in,out] acelp_mem pointer to ACELP memory structure
 * \param[in] length length of zir
 * \param[out] zir pointer to zir output buffer, exponent = SF_SYNTH
 */
void CLpd_Acelp_Zir(const int16_t A[], const int32_t A_exp,
                    CAcelpStaticMem_t *acelp_mem, const int32_t length,
                    int32_t zir[], int32_t doDeemph);

/**
 * \brief Borrow static excitation memory from ACELP decoder
 * \param[in] acelp_mem pointer to ACELP memory structure
 * \param[in] length number of requested int32_t values
 * \return pointer to requested memory
 *
 * The caller has to take care not to overwrite valid memory areas.
 * During TCX/FAC calculations and before CLpd_AcelpPrepareInternalMem() is
 * called, the following memory size is available:
 * - 256 samples in case of ACELP -> TCX20 -> ACELP transition
 * - PIT_MAX_MAX+L_INTERPOL samples in all other cases
 */
int32_t *CLpd_ACELP_GetFreeExcMem(CAcelpStaticMem_t *acelp_mem, int32_t length);

void CLpd_TcxTDConceal(CAcelpStaticMem_t *acelp_mem, int16_t *pitch,
                       const int16_t lsp_old[M_LP_FILTER_ORDER],
                       const int16_t lsp_new[M_LP_FILTER_ORDER],
                       const int16_t stab_fac, int32_t numLostSubframes,
                       int32_t synth[], int32_t coreCoderFrameLength,
                       uint8_t last_tcx_noise_factor);

inline int16_t E_UTIL_random(int16_t *seed) {
  *seed = (int16_t)((((int32_t)*seed * (int32_t)31821) >> 1) + (int32_t)13849);
  return (*seed);
}

#endif /* USACDEC_ACELP_H */
