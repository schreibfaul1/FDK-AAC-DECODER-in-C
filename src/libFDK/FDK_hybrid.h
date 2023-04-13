/* -----------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2018 Fraunhofer-Gesellschaft zur Förderung der angewandten
Forschung e.V. All rights reserved.

 1.    INTRODUCTION
The Fraunhofer FDK AAC Codec Library for Android ("FDK AAC Codec") is software
that implements the MPEG Advanced Audio Coding ("AAC") encoding and decoding
scheme for digital audio. This FDK AAC Codec software is intended to be used on
a wide variety of Android devices.

AAC's HE-AAC and HE-AAC v2 versions are regarded as today's most efficient
general perceptual audio codecs. AAC-ELD is considered the best-performing
full-bandwidth communications codec by independent studies and is widely
deployed. AAC has been standardized by ISO and IEC as part of the MPEG
specifications.

Patent licenses for necessary patent claims for the FDK AAC Codec (including
those of Fraunhofer) may be obtained through Via Licensing
(www.vialicensing.com) or through the respective patent owners individually for
the purpose of encoding or decoding bit streams in products that are compliant
with the ISO/IEC MPEG audio standards. Please note that most manufacturers of
Android devices already license these patent claims through Via Licensing or
directly from the patent owners, and therefore FDK AAC Codec software may
already be covered under those patent licenses when it is used for those
licensed purposes only.

Commercially-licensed AAC software libraries, including floating-point versions
with enhanced sound quality, are also available from Fraunhofer. Users are
encouraged to check the Fraunhofer website for additional applications
information and documentation.

2.    COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification,
are permitted without payment of copyright license fees provided that you
satisfy the following conditions:

You must retain the complete text of this software license in redistributions of
the FDK AAC Codec or your modifications thereto in source code form.

You must retain the complete text of this software license in the documentation
and/or other materials provided with redistributions of the FDK AAC Codec or
your modifications thereto in binary form. You must make available free of
charge copies of the complete source code of the FDK AAC Codec and your
modifications thereto to recipients of copies in binary form.

The name of Fraunhofer may not be used to endorse or promote products derived
from this library without prior written permission.

You may not charge copyright license fees for anyone to use, copy or distribute
the FDK AAC Codec software or your modifications thereto.

Your modified versions of the FDK AAC Codec must carry prominent notices stating
that you changed the software and the date of any change. For modified versions
of the FDK AAC Codec, the term "Fraunhofer FDK AAC Codec Library for Android"
must be replaced by the term "Third-Party Modified Version of the Fraunhofer FDK
AAC Codec Library for Android."

3.    NO PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without
limitation the patents of Fraunhofer, ARE GRANTED BY THIS SOFTWARE LICENSE.
Fraunhofer provides no warranty of patent non-infringement with respect to this
software.

You may use this FDK AAC Codec software or modifications thereto only for
purposes that are authorized by appropriate patent licenses.

4.    DISCLAIMER

This FDK AAC Codec software is provided by Fraunhofer on behalf of the copyright
holders and contributors "AS IS" and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
including but not limited to the implied warranties of merchantability and
fitness for a particular purpose. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE for any direct, indirect, incidental, special, exemplary,
or consequential damages, including but not limited to procurement of substitute
goods or services; loss of use, data, or profits, or business interruption,
however caused and on any theory of liability, whether in contract, strict
liability, or tort (including negligence), arising in any way out of the use of
this software, even if advised of the possibility of such damage.

5.    CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Audio and Multimedia Departments - FDK AAC LL
Am Wolfsmantel 33
91058 Erlangen, Germany

www.iis.fraunhofer.de/amm
amm-info@iis.fraunhofer.de
----------------------------------------------------------------------------- */

/******************* Library for basic calculation routines ********************

   Author(s):   Markus Lohwasser

   Description: FDK Tools Hybrid Filterbank

*******************************************************************************/

#ifndef FDK_HYBRID_H
#define FDK_HYBRID_H

#include "common_fix.h"

/*--------------- enums -------------------------------*/

/**
 * Hybrid Filterband modes.
 */
typedef enum {
  THREE_TO_TEN,
  THREE_TO_TWELVE,
  THREE_TO_SIXTEEN

} FDK_HYBRID_MODE;

/*--------------- structure definitions ---------------*/
typedef const struct FDK_HYBRID_SETUP *HANDLE_FDK_HYBRID_SETUP;

typedef struct {
  int32_t *bufferLFReal[3];  /*!< LF real filter states. */
  int32_t *bufferLFImag[3];  /*!< LF imag filter states. */
  int32_t *bufferHFReal[13]; /*!< HF real delay lines. */
  int32_t *bufferHFImag[13]; /*!< HF imag delay lines. */

  int32_t bufferLFpos; /*!< Position to write incoming data into ringbuffer. */
  int32_t bufferHFpos; /*!< Delay line positioning. */
  int32_t nrBands;     /*!< Number of QMF bands. */
  int32_t cplxBands;   /*!< Number of complex QMF bands.*/
  uint8_t hfMode;    /*!< Flag signalizes treatment of HF bands. */

  int32_t *pLFmemory; /*!< Pointer to LF states buffer. */
  int32_t *pHFmemory; /*!< Pointer to HF states buffer. */

  uint32_t LFmemorySize; /*!< Size of LF states buffer. */
  uint32_t HFmemorySize; /*!< Size of HF states buffer. */

  HANDLE_FDK_HYBRID_SETUP pSetup; /*!< Pointer to filter setup. */

} FDK_ANA_HYB_FILTER;

typedef struct {
  int32_t nrBands;   /*!< Number of QMF bands. */
  int32_t cplxBands; /*!< Number of complex QMF bands.*/

  HANDLE_FDK_HYBRID_SETUP pSetup; /*!< Pointer to filter setup. */

} FDK_SYN_HYB_FILTER;

typedef FDK_ANA_HYB_FILTER *HANDLE_FDK_ANA_HYB_FILTER;
typedef FDK_SYN_HYB_FILTER *HANDLE_FDK_SYN_HYB_FILTER;

/**
 * \brief  Create one instance of Hybrid Analyis Filterbank.
 *
 * \param hAnalysisHybFilter  Pointer to an outlying allocated Hybrid Analysis
 * Filterbank structure.
 * \param pLFmemory           Pointer to outlying buffer used LF filtering.
 * \param LFmemorySize        Size of pLFmemory in bytes.
 * \param pHFmemory           Pointer to outlying buffer used HF delay line.
 * \param HFmemorySize        Size of pLFmemory in bytes.
 *
 * \return  0 on success.
 */
int32_t FDKhybridAnalysisOpen(HANDLE_FDK_ANA_HYB_FILTER hAnalysisHybFilter,
                          int32_t *const pLFmemory, const uint32_t LFmemorySize,
                          int32_t *const pHFmemory, const uint32_t HFmemorySize);

/**
 * \brief  Initialize and configure Hybrid Analysis Filterbank instance.
 *
 * \param hAnalysisHybFilter  A Hybrid Analysis Filterbank handle.
 * \param mode                Select hybrid filter configuration.
 * \param qmfBands            Number of qmf bands to be processed.
 * \param cplxBands           Number of complex qmf bands to be processed.
 * \param initStatesFlag      Indicates whether the states buffer has to be
 * cleared.
 *
 * \return  0 on success.
 */
int32_t FDKhybridAnalysisInit(HANDLE_FDK_ANA_HYB_FILTER hAnalysisHybFilter,
                          const FDK_HYBRID_MODE mode, const int32_t qmfBands,
                          const int32_t cplxBands, const int32_t initStatesFlag);

/**
 * \brief  Adjust Hybrid Analysis Filterbank states.
 *
 * \param hAnalysisHybFilter  A Hybrid Analysis Filterbank handle.
 * \param scalingValue        Scaling value to be applied on filter states.
 *
 * \return  0 on success.
 */
int32_t FDKhybridAnalysisScaleStates(HANDLE_FDK_ANA_HYB_FILTER hAnalysisHybFilter,
                                 const int32_t scalingValue);

/**
 * \brief  Apply Hybrid Analysis Filterbank on Qmf input data.
 *
 * \param hAnalysisHybFilter  A Hybrid Analysis Filterbank handle.
 * \param pQmfReal            Qmf input data.
 * \param pQmfImag            Qmf input data.
 * \param pHybridReal         Hybrid output data.
 * \param pHybridImag         Hybrid output data.
 *
 * \return  0 on success.
 */
int32_t FDKhybridAnalysisApply(HANDLE_FDK_ANA_HYB_FILTER hAnalysisHybFilter,
                           const int32_t *const pQmfReal,
                           const int32_t *const pQmfImag,
                           int32_t *const pHybridReal,
                           int32_t *const pHybridImag);

/**
 * \brief  Close a Hybrid Analysis Filterbank instance.
 *
 * \param hAnalysisHybFilter  Pointer to a Hybrid Analysis Filterbank instance.
 *
 * \return  0 on success.
 */
int32_t FDKhybridAnalysisClose(HANDLE_FDK_ANA_HYB_FILTER hAnalysisHybFilter);

/**
 * \brief  Initialize and configure Hybrdid Synthesis Filterbank instance.
 *
 * \param hSynthesisHybFilter A Hybrid Synthesis Filterbank handle.
 * \param mode                Select hybrid filter configuration.
 * \param qmfBands            Number of qmf bands to be processed.
 * \param cplxBands           Number of complex qmf bands to be processed.
 *
 * \return  0 on success.
 */
int32_t FDKhybridSynthesisInit(HANDLE_FDK_SYN_HYB_FILTER hSynthesisHybFilter,
                           const FDK_HYBRID_MODE mode, const int32_t qmfBands,
                           const int32_t cplxBands);

/**
 * \brief  Apply Hybrid Analysis Filterbank on Hybrid data.
 *
 * \param hSynthesisHybFilter  A Hybrid Analysis Filterbandk handle.
 * \param pHybridReal          Hybrid input data.
 * \param pHybridImag          Hybrid input data.
 * \param pQmfReal             Qmf output data.
 * \param pQmfImag             Qmf output data.
 *
 */
void FDKhybridSynthesisApply(HANDLE_FDK_SYN_HYB_FILTER hSynthesisHybFilter,
                             const int32_t *const pHybridReal,
                             const int32_t *const pHybridImag,
                             int32_t *const pQmfReal,
                             int32_t *const pQmfImag);

#endif /* FDK_HYBRID_H */
