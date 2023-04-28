

/******************* Library for basic calculation routines ********************

   Author(s):   Manuel Jander

   Description: LPC related functions

*******************************************************************************/

#ifndef FDK_LPC_H
#define FDK_LPC_H
#include "../libAACdec/newAACDecoder.h"
#include "common_fix.h"

#define LPC_MAX_ORDER 24

/*
 * Experimental solution for lattice filter substitution.
 * LPC_SYNTHESIS_IIR macro must be activated in aacdec_tns.cpp.
 * When LPC_SYNTHESIS_IIR enabled, there will be a substitution of the default
 * lpc synthesis lattice filter by an IIR synthesis filter (with a conversionof
 * the filter coefs). LPC_TNS related macros are intended to implement the data
 * types used by the CLpc_Synthesis variant which is used for this solution.
 * */

/* #define LPC_TNS_LOWER_PRECISION */

typedef int32_t FIXP_LPC_TNS;
#define FX_DBL2FX_LPC_TNS(x) (x)
#define FX_DBL2FXCONST_LPC_TNS(x) (x)
#define FX_LPC_TNS2FX_DBL(x) (x)
#define FL2FXCONST_LPC_TNS(val) FL2FXCONST_DBL(val)
#define MAXVAL_LPC_TNS MAXVAL_DBL

#define FX_LPC2FX_DBL(x) FX_SGL2FX_DBL(x)
#define FL2FXCONST_LPC(val) FL2FXCONST_SGL(val)
#define MAXVAL_LPC MAXVAL_SGL

/**
 * \brief Obtain residual signal through LPC analysis.
 * \param signal pointer to buffer holding signal to be analysed. Residual is
 * returned there (in place)
 * \param signal_size the size of the input data in pData
 * \param lpcCoeff_m the LPC filter coefficient mantissas
 * \param lpcCoeff_e the LPC filter coefficient exponent
 * \param order the LPC filter order (size of coeff)
 * \param filtState Pointer to state buffer of size order
 * \param filtStateIndex pointer to state index storage
 */
void CLpc_Analysis(int32_t signal[], const int32_t signal_size,
                   const int16_t lpcCoeff_m[], const int32_t lpcCoeff_e,
                   const int32_t order, int32_t *filtState, int32_t *filtStateIndex);

/**
 * \brief Synthesize signal fom residual through LPC synthesis, using LP
 * coefficients.
 * \param signal pointer to buffer holding the residual signal. The synthesis is
 * returned there (in place)
 * \param signal_size the size of the input data in pData
 * \param inc buffer traversal increment for signal
 * \param coeff the LPC filter coefficients
 * \param coeff_e exponent of coeff
 * \param order the LPC filter order (size of coeff)
 * \param state state buffer of size LPC_MAX_ORDER
 * \param pStateIndex pointer to state index storage
 */
void CLpc_Synthesis(int32_t *signal, const int32_t signal_size, const int32_t signal_e,
                    const int32_t inc, const FIXP_LPC_TNS *lpcCoeff_m,
                    const int32_t lpcCoeff_e, const int32_t order, int32_t *state,
                    int32_t *pStateIndex);
void CLpc_Synthesis(int32_t *signal, const int32_t signal_size, const int32_t signal_e,
                    const int32_t inc, const int16_t coeff[], const int32_t coeff_e,
                    const int32_t order, int32_t *filtState, int32_t *pStateIndex);

/**
 * \brief Synthesize signal fom residual through LPC synthesis, using ParCor
 * coefficients. The algorithm assumes a filter gain of max 1.0. If the filter
 * gain is higher, this must be accounted into the values of signal_e
 * and/or signal_e_out to avoid overflows.
 * \param signal pointer to buffer holding the residual signal. The synthesis is
 * returned there (in place)
 * \param signal_size the size of the input data in pData
 * \param inc buffer traversal increment for signal
 * \param coeff the LPC filter coefficients
 * \param coeff_e exponent of coeff
 * \param order the LPC filter order (size of coeff)
 * \param state state buffer of size LPC_MAX_ORDER
 */
void CLpc_SynthesisLattice(int32_t *signal, const int32_t signal_size,
                           const int32_t signal_e, const int32_t signal_e_out,
                           const int32_t inc, const int16_t *coeff,
                           const int32_t order, int32_t *state);

void CLpc_SynthesisLattice(int32_t * signal, const int32_t signal_size,
                           const int32_t signal_e, const int32_t signal_e_out,
                           const int32_t inc, const int32_t * coeff,
                           const int32_t order, int32_t * state);

/**
 * \brief
 */
int32_t CLpc_ParcorToLpc(const FIXP_LPC_TNS reflCoeff[], FIXP_LPC_TNS LpcCoeff[],
                     int32_t numOfCoeff, int32_t workBuffer[]);
int32_t CLpc_ParcorToLpc(const int16_t reflCoeff[], int16_t LpcCoeff[],
                     const int32_t numOfCoeff, int32_t workBuffer[]);

/**
 * \brief Calculate ParCor (Partial autoCorrelation, reflection) coefficients
 * from autocorrelation coefficients using the Schur algorithm (instead of
 * Levinson Durbin).
 * \param acorr order+1 autocorrelation coefficients
 * \param reflCoeff output reflection /ParCor coefficients. The first
 * coefficient which is always 1.0 is ommitted.
 * \param order number of acorr / reflCoeff coefficients.
 * \param pPredictionGain_m prediction gain mantissa
 * \param pPredictionGain_e prediction gain exponent
 */
void CLpc_AutoToParcor(int32_t acorr[], const int32_t acorr_e,
                       int16_t reflCoeff[], const int32_t order,
                       int32_t *pPredictionGain_m, int32_t *pPredictionGain_e);

#endif /* FDK_LPC_H */
