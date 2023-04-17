

/**************************** AAC decoder library ******************************

   Author(s):   Manuel Jander

   Description: USAC FAC

*******************************************************************************/

#ifndef USACDEC_FAC_H
#define USACDEC_FAC_H
#include "newAACDecoder.h"
#include "channelinfo.h"
#include "../libFDK/FDK_bitstream.h"

/**
 * \brief Get the address of a memory area of the spectral data memory were the
 * FAC data can be stored into.
 * \param spec int32_t* pointing to the current spectral data.
 * \param mod the current LPD mod array.
 * \param pState pointer to a private state variable which must be 0 for the
 * first call and not changed externally.
 * \param isFullbandLPD is 1 if fullband LPD mode is on, otherwise it is 0.
 */
int32_t *CLpd_FAC_GetMemory(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                             uint8_t mod[NB_SUBFR], int32_t *pState);

/**
 * \brief read a fac bitstream data block.
 * \param hBs a bit stream handle, where the fac bitstream data is located.
 * \param pFac pointer to were the FAC data will be stored into.
 * \param pFacScale pointer to were the FAC data scale value will be stored
 * into.
 * \param tcx_gain value to be used as FAC gain. If zero, read fac_gain from
 * bitstream.
 * \param tcx_gain_e exponen value of tcx_gain.
 * \param frame the subframe to be considered from the current superframe.
 * Always 0 for FD case.
 * \return 0 on success, -1 on error.
 */
int32_t CLpd_FAC_Read(HANDLE_FDK_BITSTREAM hBs, int32_t *pFac, int8_t *pFacScale,
                  int32_t length, int32_t use_gain, int32_t frame);

/**
 * \brief Apply TCX and ALFD gains to FAC data.
 * \param fac_data pointer to FAC data.
 * \param fac_length FAC length (128 or 96).
 * \param tcx_gain TCX gain
 * \param alfd_gains pointer to alfd gains.
 * \param mod mod value (1,2,3) of TCX frame where the FAC signal needs to be
 * applied.
 */
void CFac_ApplyGains(int32_t fac_data[LFAC], const int32_t fac_length,
                     const int32_t tcx_gain, const int32_t alfd_gains[],
                     const int32_t mod);

/**
 * \brief Do FAC transition from frequency domain to ACELP domain.
 */
int32_t CLpd_FAC_Mdct2Acelp(H_MDCT hMdct, int32_t *output, int32_t *pFac_data,
                        const int32_t fac_data_e, int16_t *A, int32_t A_exp,
                        int32_t nrOutSamples, const int32_t fac_length,
                        const int32_t isFdFac, uint8_t prevWindowShape);

/**
 * \brief Do FAC transition from ACELP domain to frequency domain.
 * \param hMdct MDCT context.
 * \param output pointer for time domain output.
 * \param pSpec pointer to MDCT spectrum input.
 * \param spec_scale MDCT spectrum exponents.
 * \param nSpec amount of contiguos MDCT spectra.
 * \param pFac pointer to FAC MDCT domain data.
 * \param fac_scale exponent of FAC data.
 * \param fac_length length of FAC data.
 * \param nrSamples room in samples in output buffer.
 * \param tl MDCT transform length of pSpec.
 * \param wrs right MDCT window slope.
 * \param fr right MDCT window slope length.
 * \param A LP domain filter coefficients.
 * \param deemph_mem deemphasis filter state.
 * \param gain gain to be applied to FAC data before overlap add.
 * \param old_syn_mem Synthesis filter state.
 * \param isFdFac indicates fac processing from or to FD.
 * \param pFacData fac data stored for fullband LPD.
 * \param elFlags element specific parser guidance flags.
 * \param isFacForFullband indicates that fac is processed for fullband LPD.
 */
int32_t CLpd_FAC_Acelp2Mdct(H_MDCT hMdct, int32_t *output, int32_t *pSpec,
                        const int16_t spec_scale[], const int32_t nSpec,
                        int32_t *pFac_data, const int32_t fac_data_e,
                        const int32_t fac_length, int32_t nrSamples, const int32_t tl,
                        const FIXP_SPK_t *wrs, const int32_t fr, int16_t A[16],
                        int32_t A_exp, CAcelpStaticMem_t *acelp_mem,
                        const int32_t gain, const int32_t last_frame_lost,
                        const int32_t isFdFac, const uint8_t last_lpd, const int32_t k,
                        int32_t currAliasingSymmetry);

#endif /* USACDEC_FAC_H */
