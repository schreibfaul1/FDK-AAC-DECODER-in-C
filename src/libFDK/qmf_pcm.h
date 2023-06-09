
/******************* Library for basic calculation routines ********************

   Author(s):   Markus Lohwasser, Josef Hoepfl, Manuel Jander

   Description: QMF filterbank

*******************************************************************************/
#include <assert.h>
#ifndef QMF_PCM_H
    #define QMF_PCM_H
    #include <memory.h>
    #include <stdint.h>
    #include "../libAACdec/newAACDecoder.h"
    #include "qmf.h"
/*
   All Synthesis functions dependent on datatype INT_PCM_QMFOUT
   Should only be included by qmf.cpp, but not compiled separately, please
   exclude compilation from project, if done otherwise. Is optional included
   twice to duplicate all functions with two different pre-definitions, as:
        #define INT_PCM_QMFOUT int32_t
    and ...
        #define INT_PCM_QMFOUT int16_t
    needed to run QMF synthesis in both 16bit and 32bit sample output format.
*/

    #define QSSCALE          (0)
    #define FX_DBL2FX_QSS(x) (x)
    #define FX_QSS2FX_DBL(x) (x)

    /*!
      \brief Perform Synthesis Prototype Filtering on a single slot of input data.

      The filter takes 2 * qmf->no_channels of input data and
      generates qmf->no_channels time domain output samples.
    */
    /* static */
    #ifndef FUNCTION_qmfSynPrototypeFirSlot
void qmfSynPrototypeFirSlot(
    #else
void qmfSynPrototypeFirSlot_fallback(
    #endif
    HANDLE_QMF_FILTER_BANK qmf, int32_t *realSlot, /*!< Input: Pointer to real Slot */
    int32_t        *imagSlot,                      /*!< Input: Pointer to imag Slot */
    INT_PCM_QMFOUT *timeOut,                       /*!< Time domain data */
    int32_t         stride) {
    FIXP_QSS       *FilterStates = (FIXP_QSS *)qmf->FilterStates;
    int32_t         no_channels = qmf->no_channels;
    const FIXP_PFT *p_Filter = qmf->p_filter;
    int32_t         p_stride = qmf->p_stride;
    int32_t         j;
    FIXP_QSS       *sta = FilterStates;
    const FIXP_PFT *p_flt, *p_fltm;
    int32_t         scale = (DFRACT_BITS - SAMPLE_BITS_QMFOUT) - 1 - qmf->outScalefactor - qmf->outGain_e;

    p_flt = p_Filter + p_stride * QMF_NO_POLY;                          /*                     5th of 330 */
    p_fltm = p_Filter + (qmf->FilterSize / 2) - p_stride * QMF_NO_POLY; /* 5 + (320 - 2*5) = 315th of 330 */

    int16_t gain = FX_DBL2FX_SGL(qmf->outGain_m);

    int32_t rnd_val = 0;

    if(scale > 0) {
        if(scale < (DFRACT_BITS - 1)) rnd_val = int32_t(1 << (scale - 1));
        else
            scale = (DFRACT_BITS - 1);
    }
    else { scale = fMax(scale, -(DFRACT_BITS - 1)); }

    for(j = no_channels - 1; j >= 0; j--) {
        int32_t imag = imagSlot[j]; /* no_channels-1 .. 0 */
        int32_t real = realSlot[j]; /* no_channels-1 .. 0 */
        {
            INT_PCM_QMFOUT tmp;
            int32_t        Are = fMultAddDiv2(FX_QSS2FX_DBL(sta[0]), p_fltm[0], real);

            /* This PCM formatting performs:
               - multiplication with 16-bit gain, if not -1.0f
               - rounding, if shift right is applied
               - apply shift left (or right) with saturation to 32 (or 16) bits
               - store output with --stride in 32 (or 16) bit format
            */
            if(gain != (int16_t)(-32768)) /* -1.0f */
            {
                Are = fMult(Are, gain);
            }
            if(scale >= 0) {
                assert(Are <= (Are + rnd_val)); /* Round-addition must not overflow, might be
                                                   equal for rnd_val=0 */
                tmp = (INT_PCM_QMFOUT)(SATURATE_RIGHT_SHIFT(Are + rnd_val, scale, SAMPLE_BITS_QMFOUT));
            }
            else { tmp = (INT_PCM_QMFOUT)(SATURATE_LEFT_SHIFT(Are, -scale, SAMPLE_BITS_QMFOUT)); }

            { timeOut[(j)*stride] = tmp; }
        }

        sta[0] = FX_DBL2FX_QSS(fMultAddDiv2(FX_QSS2FX_DBL(sta[1]), p_flt[4], imag));
        sta[1] = FX_DBL2FX_QSS(fMultAddDiv2(FX_QSS2FX_DBL(sta[2]), p_fltm[1], real));
        sta[2] = FX_DBL2FX_QSS(fMultAddDiv2(FX_QSS2FX_DBL(sta[3]), p_flt[3], imag));
        sta[3] = FX_DBL2FX_QSS(fMultAddDiv2(FX_QSS2FX_DBL(sta[4]), p_fltm[2], real));
        sta[4] = FX_DBL2FX_QSS(fMultAddDiv2(FX_QSS2FX_DBL(sta[5]), p_flt[2], imag));
        sta[5] = FX_DBL2FX_QSS(fMultAddDiv2(FX_QSS2FX_DBL(sta[6]), p_fltm[3], real));
        sta[6] = FX_DBL2FX_QSS(fMultAddDiv2(FX_QSS2FX_DBL(sta[7]), p_flt[1], imag));
        sta[7] = FX_DBL2FX_QSS(fMultAddDiv2(FX_QSS2FX_DBL(sta[8]), p_fltm[4], real));
        sta[8] = FX_DBL2FX_QSS(fMultDiv2(p_flt[0], imag));
        p_flt += (p_stride * QMF_NO_POLY);
        p_fltm -= (p_stride * QMF_NO_POLY);
        sta += 9;  // = (2*QMF_NO_POLY-1);
    }
}

    #ifndef FUNCTION_qmfSynPrototypeFirSlot_NonSymmetric
/*!
  \brief Perform Synthesis Prototype Filtering on a single slot of input data.

  The filter takes 2 * qmf->no_channels of input data and
  generates qmf->no_channels time domain output samples.
*/
static void qmfSynPrototypeFirSlot_NonSymmetric(HANDLE_QMF_FILTER_BANK qmf,
                                                int32_t               *realSlot, /*!< Input: Pointer to real Slot */
                                                int32_t               *imagSlot, /*!< Input: Pointer to imag Slot */
                                                INT_PCM_QMFOUT        *timeOut,  /*!< Time domain data */
                                                int32_t                stride) {
    FIXP_QSS       *FilterStates = (FIXP_QSS *)qmf->FilterStates;
    int32_t         no_channels = qmf->no_channels;
    const FIXP_PFT *p_Filter = qmf->p_filter;
    int32_t         p_stride = qmf->p_stride;
    int32_t         j;
    FIXP_QSS       *sta = FilterStates;
    const FIXP_PFT *p_flt, *p_fltm;
    int32_t         scale = (DFRACT_BITS - SAMPLE_BITS_QMFOUT) - 1 - qmf->outScalefactor - qmf->outGain_e;

    p_flt = p_Filter;                     /*!< Pointer to first half of filter coefficients */
    p_fltm = &p_flt[qmf->FilterSize / 2]; /* at index 320, overall 640 coefficients */

    int16_t gain = FX_DBL2FX_SGL(qmf->outGain_m);

    int32_t rnd_val = (int32_t)0;

    if(scale > 0) {
        if(scale < (DFRACT_BITS - 1)) rnd_val = int32_t(1 << (scale - 1));
        else
            scale = (DFRACT_BITS - 1);
    }
    else { scale = fMax(scale, -(DFRACT_BITS - 1)); }

    for(j = no_channels - 1; j >= 0; j--) {
        int32_t imag = imagSlot[j]; /* no_channels-1 .. 0 */
        int32_t real = realSlot[j]; /* no_channels-1 .. 0 */
        {
            INT_PCM_QMFOUT tmp;
            int32_t        Are = sta[0] + FX_DBL2FX_QSS(fMultDiv2(p_fltm[4], real));

            /* This PCM formatting performs:
               - multiplication with 16-bit gain, if not -1.0f
               - rounding, if shift right is applied
               - apply shift left (or right) with saturation to 32 (or 16) bits
               - store output with --stride in 32 (or 16) bit format
            */
            if(gain != (int16_t)(-32768)) /* -1.0f */
            {
                Are = fMult(Are, gain);
            }
            if(scale > 0) {
                assert(Are < (Are + rnd_val)); /* Round-addition must not overflow */
                tmp = (INT_PCM_QMFOUT)(SATURATE_RIGHT_SHIFT(Are + rnd_val, scale, SAMPLE_BITS_QMFOUT));
            }
            else { tmp = (INT_PCM_QMFOUT)(SATURATE_LEFT_SHIFT(Are, -scale, SAMPLE_BITS_QMFOUT)); }
            timeOut[j * stride] = tmp;
        }

        sta[0] = sta[1] + FX_DBL2FX_QSS(fMultDiv2(p_flt[4], imag));
        sta[1] = sta[2] + FX_DBL2FX_QSS(fMultDiv2(p_fltm[3], real));
        sta[2] = sta[3] + FX_DBL2FX_QSS(fMultDiv2(p_flt[3], imag));

        sta[3] = sta[4] + FX_DBL2FX_QSS(fMultDiv2(p_fltm[2], real));
        sta[4] = sta[5] + FX_DBL2FX_QSS(fMultDiv2(p_flt[2], imag));
        sta[5] = sta[6] + FX_DBL2FX_QSS(fMultDiv2(p_fltm[1], real));
        sta[6] = sta[7] + FX_DBL2FX_QSS(fMultDiv2(p_flt[1], imag));

        sta[7] = sta[8] + FX_DBL2FX_QSS(fMultDiv2(p_fltm[0], real));
        sta[8] = FX_DBL2FX_QSS(fMultDiv2(p_flt[0], imag));

        p_flt += (p_stride * QMF_NO_POLY);
        p_fltm += (p_stride * QMF_NO_POLY);
        sta += 9;  // = (2*QMF_NO_POLY-1);
    }
}
    #endif /* FUNCTION_qmfSynPrototypeFirSlot_NonSymmetric */

void qmfSynthesisFilteringSlot(HANDLE_QMF_FILTER_BANK synQmf, const int32_t *realSlot, const int32_t *imagSlot,
                               const int32_t scaleFactorLowBand, const int32_t scaleFactorHighBand,
                               INT_PCM_QMFOUT *timeOut, const int32_t stride, int32_t *pWorkBuffer) {
    if(!(synQmf->flags & QMF_FLAG_LP))
        qmfInverseModulationHQ(synQmf, realSlot, imagSlot, scaleFactorLowBand, scaleFactorHighBand, pWorkBuffer);
    else {
        if(synQmf->flags & QMF_FLAG_CLDFB) {
            qmfInverseModulationLP_odd(synQmf, realSlot, scaleFactorLowBand, scaleFactorHighBand, pWorkBuffer);
        }
        else { qmfInverseModulationLP_even(synQmf, realSlot, scaleFactorLowBand, scaleFactorHighBand, pWorkBuffer); }
    }

    if(synQmf->flags & QMF_FLAG_NONSYMMETRIC) {
        qmfSynPrototypeFirSlot_NonSymmetric(synQmf, pWorkBuffer, pWorkBuffer + synQmf->no_channels, timeOut, stride);
    }
    else { qmfSynPrototypeFirSlot(synQmf, pWorkBuffer, pWorkBuffer + synQmf->no_channels, timeOut, stride); }
}

/*!
 *
 * \brief Perform complex-valued subband synthesis of the
 *        low band and the high band and store the
 *        time domain data in timeOut
 *
 * First step: Calculate the proper scaling factor of current
 * spectral data in qmfReal/qmfImag, old spectral data in the overlap
 * range and filter states.
 *
 * Second step: Perform Frequency-to-Time mapping with inverse
 * Modulation slot-wise.
 *
 * Third step: Perform FIR-filter slot-wise. To save space for filter
 * states, the MAC operations are executed directly on the filter states
 * instead of accumulating several products in the accumulator. The
 * buffer shift at the end of the function should be replaced by a
 * modulo operation, which is available on some DSPs.
 *
 * Last step: Copy the upper part of the spectral data to the overlap buffer.
 *
 * The qmf coefficient table is symmetric. The symmetry is exploited by
 * shrinking the coefficient table to half the size. The addressing mode
 * takes care of the symmetries.  If the #define #QMFTABLE_FULL is set,
 * coefficient addressing works on the full table size. The code will be
 * slightly faster and slightly more compact.
 *
 * Workbuffer requirement: 2 x sizeof(**QmfBufferReal) * synQmf->no_channels
 * The workbuffer must be aligned
 */
void qmfSynthesisFiltering(HANDLE_QMF_FILTER_BANK  synQmf,        /*!< Handle of Qmf Synthesis Bank  */
                           int32_t               **QmfBufferReal, /*!< Low and High band, real */
                           int32_t               **QmfBufferImag, /*!< Low and High band, imag */
                           const QMF_SCALE_FACTOR *scaleFactor,
                           const int32_t           ov_len,     /*!< split Slot of overlap and actual slots */
                           INT_PCM_QMFOUT         *timeOut,    /*!< Pointer to output */
                           const int32_t           stride,     /*!< stride factor of output */
                           int32_t                *pWorkBuffer /*!< pointer to temporal working buffer */
) {
    int32_t i;
    int32_t L = synQmf->no_channels;
    int32_t scaleFactorHighBand;
    int32_t scaleFactorLowBand_ov, scaleFactorLowBand_no_ov;

    assert(synQmf->no_channels >= synQmf->lsb);
    assert(synQmf->no_channels >= synQmf->usb);

    /* adapt scaling */
    scaleFactorHighBand = -ALGORITHMIC_SCALING_IN_ANALYSIS_FILTERBANK - scaleFactor->hb_scale - synQmf->filterScale;
    scaleFactorLowBand_ov =
        -ALGORITHMIC_SCALING_IN_ANALYSIS_FILTERBANK - scaleFactor->ov_lb_scale - synQmf->filterScale;
    scaleFactorLowBand_no_ov =
        -ALGORITHMIC_SCALING_IN_ANALYSIS_FILTERBANK - scaleFactor->lb_scale - synQmf->filterScale;

    for(i = 0; i < synQmf->no_col; i++) /* ----- no_col loop ----- */
    {
        const int32_t *QmfBufferImagSlot = NULL;

        int32_t scaleFactorLowBand = (i < ov_len) ? scaleFactorLowBand_ov : scaleFactorLowBand_no_ov;

        if(!(synQmf->flags & QMF_FLAG_LP)) QmfBufferImagSlot = QmfBufferImag[i];

        qmfSynthesisFilteringSlot(synQmf, QmfBufferReal[i], QmfBufferImagSlot, scaleFactorLowBand, scaleFactorHighBand,
                                  timeOut + (i * L * stride), stride, pWorkBuffer);
    } /* no_col loop  i  */
}

/*!
 *
 * \brief Create QMF filter bank instance
 *
 *
 * \return 0 if successful
 *
 */
int32_t qmfInitAnalysisFilterBank(HANDLE_QMF_FILTER_BANK h_Qmf,         /*!< Returns handle */
                                  FIXP_QAS              *pFilterStates, /*!< Handle to filter states */
                                  int32_t                noCols,        /*!< Number of timeslots per frame */
                                  int32_t                lsb,           /*!< lower end of QMF */
                                  int32_t                usb,           /*!< upper end of QMF */
                                  int32_t                no_channels,   /*!< Number of channels (bands) */
                                  int32_t                flags)                        /*!< Low Power flag */
{
    int32_t err = qmfInitFilterBank(h_Qmf, pFilterStates, noCols, lsb, usb, no_channels, flags, 0);
    if(!(flags & QMF_FLAG_KEEP_STATES) && (h_Qmf->FilterStates != NULL)) {
        memset(h_Qmf->FilterStates, 0, (2 * QMF_NO_POLY - 1) * h_Qmf->no_channels * sizeof(FIXP_QAS));
    }

    assert(h_Qmf->no_channels >= h_Qmf->lsb);

    return err;
}

    #ifndef FUNCTION_qmfAnaPrototypeFirSlot
/*!
  \brief Perform Analysis Prototype Filtering on a single slot of input data.
*/
static void qmfAnaPrototypeFirSlot(int32_t        *analysisBuffer,
                                   int32_t         no_channels, /*!< Number channels of analysis filter */
                                   const FIXP_PFT *p_filter, int32_t p_stride, /*!< Stride of analysis filter    */
                                   FIXP_QAS *pFilterStates) {
    int32_t k;

    int32_t         accu;
    const FIXP_PFT *p_flt = p_filter;
    int32_t        *pData_0 = analysisBuffer + 2 * no_channels - 1;
    int32_t        *pData_1 = analysisBuffer;

    FIXP_QAS *sta_0 = (FIXP_QAS *)pFilterStates;
    FIXP_QAS *sta_1 = (FIXP_QAS *)pFilterStates + (2 * QMF_NO_POLY * no_channels) - 1;
    int32_t   pfltStep = QMF_NO_POLY * (p_stride);
    int32_t   staStep1 = no_channels << 1;
    int32_t   staStep2 = (no_channels << 3) - 1; /* Rewind one less */

    /* FIR filters 127..64 0..63 */
    for(k = 0; k < no_channels; k++) {
        accu = fMultDiv2(p_flt[0], *sta_1);
        sta_1 -= staStep1;
        accu += fMultDiv2(p_flt[1], *sta_1);
        sta_1 -= staStep1;
        accu += fMultDiv2(p_flt[2], *sta_1);
        sta_1 -= staStep1;
        accu += fMultDiv2(p_flt[3], *sta_1);
        sta_1 -= staStep1;
        accu += fMultDiv2(p_flt[4], *sta_1);
        *pData_1++ = (accu << 1);
        sta_1 += staStep2;

        p_flt += pfltStep;
        accu = fMultDiv2(p_flt[0], *sta_0);
        sta_0 += staStep1;
        accu += fMultDiv2(p_flt[1], *sta_0);
        sta_0 += staStep1;
        accu += fMultDiv2(p_flt[2], *sta_0);
        sta_0 += staStep1;
        accu += fMultDiv2(p_flt[3], *sta_0);
        sta_0 += staStep1;
        accu += fMultDiv2(p_flt[4], *sta_0);
        *pData_0-- = (accu << 1);
        sta_0 -= staStep2;
    }
}
    #endif /* !defined(FUNCTION_qmfAnaPrototypeFirSlot) */

    #ifndef FUNCTION_qmfAnaPrototypeFirSlot_NonSymmetric
/*!
  \brief Perform Analysis Prototype Filtering on a single slot of input data.
*/
static void qmfAnaPrototypeFirSlot_NonSymmetric(int32_t        *analysisBuffer,
                                                int32_t         no_channels, /*!< Number channels of analysis filter */
                                                const FIXP_PFT *p_filter,
                                                int32_t         p_stride, /*!< Stride of analysis filter    */
                                                FIXP_QAS       *pFilterStates) {
    const FIXP_PFT *p_flt = p_filter;
    int32_t         p, k;

    for(k = 0; k < 2 * no_channels; k++) {
        int32_t accu = (int32_t)0;

        p_flt += QMF_NO_POLY * (p_stride - 1);

        /*
          Perform FIR-Filter
        */
        for(p = 0; p < QMF_NO_POLY; p++) { accu += fMultDiv2(*p_flt++, pFilterStates[2 * no_channels * p]); }
        analysisBuffer[2 * no_channels - 1 - k] = (accu << 1);
        pFilterStates++;
    }
}
    #endif /* FUNCTION_qmfAnaPrototypeFirSlot_NonSymmetric */

/*
 * \brief Perform one QMF slot analysis of the time domain data of timeIn
 *        with specified stride and stores the real part of the subband
 *        samples in rSubband, and the imaginary part in iSubband
 *
 *        Note: anaQmf->lsb can be greater than anaQmf->no_channels in case
 *        of implicit resampling (USAC with reduced 3/4 core frame length).
 */
void qmfAnalysisFilteringSlot(HANDLE_QMF_FILTER_BANK anaQmf,     /*!< Handle of Qmf Synthesis Bank  */
                              int32_t               *qmfReal,    /*!< Low and High band, real */
                              int32_t               *qmfImag,    /*!< Low and High band, imag */
                              const INT_PCM_QMFIN   *timeIn,     /*!< Pointer to input */
                              const int32_t          stride,     /*!< stride factor of input */
                              int32_t               *pWorkBuffer /*!< pointer to temporal working buffer */
) {
    int32_t offset = anaQmf->no_channels * (QMF_NO_POLY * 2 - 1);
    /*
      Feed time signal into oldest anaQmf->no_channels states
    */
    {
        FIXP_QAS *FilterStatesAnaTmp = ((FIXP_QAS *)anaQmf->FilterStates) + offset;

        /* Feed and scale actual time in slot */
        for(int32_t i = anaQmf->no_channels >> 1; i != 0; i--) {
            /* Place int16_t value left aligned in scaledTimeIn */
            *FilterStatesAnaTmp++ = (FIXP_QAS)*timeIn;
            timeIn += stride;
            *FilterStatesAnaTmp++ = (FIXP_QAS)*timeIn;
            timeIn += stride;
        }
    }

    if(anaQmf->flags & QMF_FLAG_NONSYMMETRIC) {
        qmfAnaPrototypeFirSlot_NonSymmetric(pWorkBuffer, anaQmf->no_channels, anaQmf->p_filter, anaQmf->p_stride,
                                            (FIXP_QAS *)anaQmf->FilterStates);
    }
    else {
        qmfAnaPrototypeFirSlot(pWorkBuffer, anaQmf->no_channels, anaQmf->p_filter, anaQmf->p_stride,
                               (FIXP_QAS *)anaQmf->FilterStates);
    }

    if(anaQmf->flags & QMF_FLAG_LP) {
        if(anaQmf->flags & QMF_FLAG_CLDFB) qmfForwardModulationLP_odd(anaQmf, pWorkBuffer, qmfReal);
        else
            qmfForwardModulationLP_even(anaQmf, pWorkBuffer, qmfReal);
    }
    else { qmfForwardModulationHQ(anaQmf, pWorkBuffer, qmfReal, qmfImag); }
    /*
      Shift filter states

      Should be realized with modulo addressing on a DSP instead of a true buffer
      shift
    */
    memmove(anaQmf->FilterStates, (FIXP_QAS *)anaQmf->FilterStates + anaQmf->no_channels, offset * sizeof(FIXP_QAS));
}

/*!
 *
 * \brief Perform complex-valued subband filtering of the time domain
 *        data of timeIn and stores the real part of the subband
 *        samples in rAnalysis, and the imaginary part in iAnalysis
 * The qmf coefficient table is symmetric. The symmetry is expoited by
 * shrinking the coefficient table to half the size. The addressing mode
 * takes care of the symmetries.
 *
 *
 * \sa PolyphaseFiltering
 */
void qmfAnalysisFiltering(HANDLE_QMF_FILTER_BANK anaQmf,  /*!< Handle of Qmf Analysis Bank */
                          int32_t              **qmfReal, /*!< Pointer to real subband slots */
                          int32_t              **qmfImag, /*!< Pointer to imag subband slots */
                          QMF_SCALE_FACTOR *scaleFactor, const INT_PCM_QMFIN *timeIn, /*!< Time signal */
                          const int32_t timeIn_e, const int32_t stride,
                          int32_t *pWorkBuffer /*!< pointer to temporal working buffer */
) {
    int32_t i;
    int32_t no_channels = anaQmf->no_channels;

    scaleFactor->lb_scale = -ALGORITHMIC_SCALING_IN_ANALYSIS_FILTERBANK - timeIn_e;
    scaleFactor->lb_scale -= anaQmf->filterScale;

    for(i = 0; i < anaQmf->no_col; i++) {
        int32_t *qmfImagSlot = NULL;

        if(!(anaQmf->flags & QMF_FLAG_LP)) { qmfImagSlot = qmfImag[i]; }

        qmfAnalysisFilteringSlot(anaQmf, qmfReal[i], qmfImagSlot, timeIn, stride, pWorkBuffer);

        timeIn += no_channels * stride;

    } /* no_col loop  i  */
}
#endif /* QMF_PCM_H */
