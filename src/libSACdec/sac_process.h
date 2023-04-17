

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: SAC Processing

*******************************************************************************/

/*!
  \file
  \brief  Polyphase Filterbank
*/

#ifndef SAC_PROCESS_H
#define SAC_PROCESS_H
#include "../libAACdec/newAACDecoder.h"
#include "sac_dec.h"

void SpatialDecApplyPhase(spatialDec *self, int16_t alpha,
                          int32_t lastSlotOfParamSet);

/**
 * \brief  Apply QMF Analysis Filterbank.
 *
 * Calculates qmf data on downmix input time data.
 * Delaylines will be applied if necessaray.
 *
 * \param self                A spatial decoder handle.
 * \param inData              Downmix channel time data as input.
 * \param ts                  Signals time slot offset for input buffer.
 * \param qmfReal             Downmix channel qmf output data.
 * \param qmfImag             Downmix channel qmf output data.
 *
 * \return  Error status.
 */
SACDEC_ERROR SpatialDecQMFAnalysis(spatialDec *self, const PCM_MPS *inData,
                                   const int32_t ts, const int32_t bypassMode,
                                   int32_t **qmfReal, int32_t **qmfImag,
                                   const int32_t numInputChannels);

/**
 * \brief  Feed spatial decoder with external qmf data.
 *
 * \param self                A spatial decoder handle.
 * \param qmfInDataReal       External qmf downmix data as input.
 * \param qmfInDataImag       External qmf downmix data as input.
 * \param ts                  Signals time slot in input buffer to process.
 * \param qmfReal             Downmix channel qmf output data.
 * \param qmfImag             Downmix channel qmf output data.
 * \param numInputChannels    Number of input channels. Might differ from
 * self->numInputChannels.
 *
 * \return  Error status.
 */
SACDEC_ERROR SpatialDecFeedQMF(spatialDec *self, int32_t **qmfInDataReal,
                               int32_t **qmfInDataImag, const int32_t ts,
                               const int32_t bypassMode, int32_t **qmfReal,
                               int32_t **qmfImag, const int32_t numInputChannels);

/**
 * \brief  Apply Hybrdid Analysis Filterbank.
 *
 * Calculates hybrid data on downmix input data.
 * Residual hybrid signals will also be calculated on current slot if available.
 *
 * \param self                A spatial decoder handle.
 * \param qmfInputReal        Downmix channel qmf data as input.
 * \param qmfInputImag        Downmix channel qmf data as input.
 * \param hybOutputReal       Downmix channel hybrid output data.
 * \param hybOutputImag       Downmix channel hybrid output data.
 * \param ts                  Signals time slot in spatial frame to process.
 * \param numInputChannels    Number of input channels. Might differ from
 * self->numInputChannels.
 *
 * \return  Error status.
 */
SACDEC_ERROR SpatialDecHybridAnalysis(spatialDec *self, int32_t **qmfInputReal,
                                      int32_t **qmfInputImag,
                                      int32_t **hybOutputReal,
                                      int32_t **hybOutputImag, const int32_t ts,
                                      const int32_t numInputChannels);

/**
 * \brief  Create X data.
 *
 * Returns a pointer list over Xchannels pointing to downmix input channels
 * and to residual channels when provided.
 *
 * \param self                A spatial decoder handle.
 * \param hybInputReal        Downmix channel hybrid data as input.
 * \param hybInputImag        Downmix channel hybrid data as input.
 * \param pxReal              Pointer to hybrid and residual data as output.
 * \param pxImag              Pointer to hybrid and residual data as output.
 *
 * \return  Error status.
 */
SACDEC_ERROR SpatialDecCreateX(spatialDec *self, int32_t **hybInputReal,
                               int32_t **hybInputImag, int32_t **pxReal,
                               int32_t **pxImag);

/**
 * \brief  MPS212 combined version of apply M1 parameters and create wet signal
 *
 * \param self                A spatial decoder handle.
 * \param xReal               Downmix and residual X data as input.
 * \param xImag               Downmix and residual X data as input.
 * \param vReal               output data: [0] direct signal (V); [1] wet signal
 * (W).
 * \param vImag               output data: [0] direct signal (V); [1] wet signal
 * (W).
 *
 * \return  Error status.
 */
SACDEC_ERROR SpatialDecApplyM1_CreateW_Mode212(
    spatialDec *self, const SPATIAL_BS_FRAME *frame, int32_t **xReal,
    int32_t **xImag, int32_t **vReal, int32_t **vImag);

/**
 * \brief  Apply M2 parameters.
 *
 * \param self                A spatial decoder handle.
 * \param ps                  Signals parameter band from where M2 parameter to
 * use.
 * \param alpha               Smoothing factor between current and previous
 * parameter band. Rangeability between 0.f and 1.f.
 * \param wReal               Wet input data.
 * \param wImag               Wet input data.
 * \param hybOutputRealDry    Dry output data.
 * \param hybOutputImagDry    Dry output data.
 * \param hybOutputRealWet    Wet output data.
 * \param hybOutputImagWet    Wet output data.
 *
 * \return  Error status.
 */
SACDEC_ERROR SpatialDecApplyM2(spatialDec *self, int32_t ps, const int16_t alpha,
                               int32_t **wReal, int32_t **wImag,
                               int32_t **hybOutputRealDry,
                               int32_t **hybOutputImagDry,
                               int32_t **hybOutputRealWet,
                               int32_t **hybOutputImagWet);

/**
 * \brief  Apply M2 parameter for 212 mode with residualCoding and phaseCoding.
 *
 * \param self                [i] A spatial decoder handle.
 * \param ps                  [i] Signals parameter band from where M2 parameter
 * to use.
 * \param alpha               [i] Smoothing factor between current and previous
 * parameter band. Rangeability between 0.f and 1.f.
 * \param wReal               [i] Wet input data.
 * \param wImag               [i] Wet input data.
 * \param hybOutputRealDry    [o] Dry output data.
 * \param hybOutputImagDry    [o] Dry output data.
 *
 * \return error
 */
SACDEC_ERROR SpatialDecApplyM2_Mode212_ResidualsPlusPhaseCoding(
    spatialDec *self, int32_t ps, const int16_t alpha, int32_t **wReal,
    int32_t **wImag, int32_t **hybOutputRealDry, int32_t **hybOutputImagDry);

/**
 * \brief  Apply M2 parameter for 212 mode, upmix from mono to stereo.
 *
 * \param self                [i] A spatial decoder handle.
 * \param ps                  [i] Signals parameter band from where M2 parameter
 * to use.
 * \param alpha               [i] Smoothing factor between current and previous
 * parameter band. Rangeability between 0.f and 1.f.
 * \param wReal               [i] Wet input data.
 * \param wImag               [i] Wet input data.
 * \param hybOutputRealDry    [o] Dry output data.
 * \param hybOutputImagDry    [o] Dry output data.
 *
 * \return error
 */
SACDEC_ERROR SpatialDecApplyM2_Mode212(spatialDec *self, int32_t ps,
                                       const int16_t alpha, int32_t **wReal,
                                       int32_t **wImag,
                                       int32_t **hybOutputRealDry,
                                       int32_t **hybOutputImagDry);

/**
 * \brief  Convert Hybrid input to output audio data.
 *
 * \param hSpaceSynthesisQmf  A spatial decoder handle.
 * \param ts                  Signals time slot in spatial frame to process.
 * \param hybOutputReal       Hybrid data as input.
 * \param hybOutputImag       Hybrid data as input.
 * \param timeOut             audio output data.
 *
 * \return  Error status.
 */
SACDEC_ERROR SpatialDecSynthesis(spatialDec *self, const int32_t ts,
                                 int32_t **hybOutputReal,
                                 int32_t **hybOutputImag, PCM_MPS *timeOut,
                                 const int32_t numInputChannels,
                                 const FDK_channelMapDescr_t *const mapDescr);

void SpatialDecBufferMatrices(spatialDec *self);

int32_t getChGain(spatialDec *self, uint32_t ch, int32_t *scale);

#endif
