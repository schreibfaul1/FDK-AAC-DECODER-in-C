

/******************* Library for basic calculation routines ********************

   Author(s):   M. Lohwasser

   Description: fixed point abs definitions

*******************************************************************************/

#ifndef AUTOCORR2ND_H
#define AUTOCORR2ND_H
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"

typedef struct {
  int32_t r00r;
  int32_t r11r;
  int32_t r22r;
  int32_t r01r;
  int32_t r02r;
  int32_t r12r;
  int32_t r01i;
  int32_t r02i;
  int32_t r12i;
  int32_t det;
  int32_t det_scale;
} ACORR_COEFS;

#define LPC_ORDER 2

int32_t autoCorr2nd_real(
    ACORR_COEFS *ac,          /*!< Pointer to autocorrelation coeffs */
    const int32_t *reBuffer, /*!< Pointer to to real part of spectrum */
    const int32_t len             /*!< Number of qmf slots */
);

int32_t autoCorr2nd_cplx(
    ACORR_COEFS *ac,          /*!< Pointer to autocorrelation coeffs */
    const int32_t *reBuffer, /*!< Pointer to to real part of spectrum */
    const int32_t *imBuffer, /*!< Pointer to imag part of spectrum */
    const int32_t len             /*!< Number of qmf slots */
);

#endif /* AUTOCORR2ND_H */
