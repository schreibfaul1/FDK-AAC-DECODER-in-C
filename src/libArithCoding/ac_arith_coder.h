

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

typedef struct {
  int16_t m_numberLinesPrev;
  uint8_t c_prev[(1024 / 2) + 4]; /* 2-tuple context of previous frame, 4 bit */
} CArcoData;

/* prototypes */

CArcoData *CArco_Create(void);

void CArco_Destroy(CArcoData *pArcoData);

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
ARITH_CODING_ERROR CArco_DecodeArithData(CArcoData *pArcoData,
                                         HANDLE_FDK_BITSTREAM hBs,
                                         int32_t * spectrum, int32_t lg,
                                         int32_t lg_max, int32_t arith_reset_flag);

#endif /* AC_ARITH_CODER_H */
