

/************************** Arithmetic coder library ***************************

   Author(s):   Oliver Weiss

   Description: Interface for Spectral Noiseless Coding Scheme based on an
                Arithmetic Coder in Conjunction with an Adaptive Context

*******************************************************************************/

#ifndef AC_ARITH_CODER_H
#define AC_ARITH_CODER_H
#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/common_fix.h"
#include "../libFDK/FDK_bitstream.h"

typedef enum { ARITH_CODER_OK = 0, ARITH_CODER_ERROR = 5 } ARITH_CODING_ERROR;



/* prototypes */

CArcoData_t *CArco_Create(void);

void CArco_Destroy(CArcoData_t *pArcoData);

/**
 * \brief decode a spectral data element by using an adaptive context dependent
 * arithmetic coding scheme
 * \param hBs bit stream handle
 * \param spectrum pointer to quantized data output.
 * \param lg number of quantized spectral coefficients (output by the arithmetic
 * decoder).
 * \param lg_max max number of quantized spectral coefficients.
 * \param arith_reset_flag flag which indicates if the spectral noiseless
 * context must be reset
 * \return void
 */
ARITH_CODING_ERROR CArco_DecodeArithData(CArcoData_t *pArcoData,
                                         HANDLE_FDK_BITSTREAM hBs,
                                         int32_t * spectrum, int32_t lg,
                                         int32_t lg_max, int32_t arith_reset_flag);

#endif /* AC_ARITH_CODER_H */
