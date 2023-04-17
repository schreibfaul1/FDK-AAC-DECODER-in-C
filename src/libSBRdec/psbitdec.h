

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

#ifndef PSBITDEC_H
#define PSBITDEC_H
#include "../libAACdec/newAACDecoder.h"
#include "sbrdecoder.h"
#include "psdec.h"

uint32_t ReadPsData(struct PS_DEC *h_ps_d, HANDLE_FDK_BITSTREAM hBs,
                        int32_t nBitsLeft);

int32_t DecodePs(struct PS_DEC *h_ps_d, const uint8_t frameError,
             PS_DEC_COEFFICIENTS *pCoef);

#endif /* PSBITDEC_H */
