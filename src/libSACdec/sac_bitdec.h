

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: SAC Dec bitstream decoder

*******************************************************************************/

/*!
  \file
  \brief  Spatial Audio bitstream decoder
*/

#ifndef SAC_BITDEC_H
#define SAC_BITDEC_H

#include "sac_dec.h"

typedef struct {
    int8_t numInputChannels;
    int8_t numOutputChannels;
    int8_t numOttBoxes;
    int8_t numTttBoxes;
    int8_t ottModeLfe[MAX_NUM_OTT];
} TREEPROPERTIES_t;

enum { TREE_212 = 7, TREE_DUMMY = 255 };

enum { QUANT_FINE = 0, QUANT_EBQ1 = 1, QUANT_EBQ2 = 2 };

SACDEC_ERROR SpatialDecParseSpecificConfigHeader(HANDLE_FDK_BITSTREAM     bitstream,
                                                 SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig,
                                                 AUDIO_OBJECT_TYPE_t coreCodec, SPATIAL_DEC_UPMIX_TYPE upmixType);

SACDEC_ERROR SpatialDecParseMps212Config(HANDLE_FDK_BITSTREAM     bitstream,
                                         SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig, int32_t samplingRate,
                                         AUDIO_OBJECT_TYPE_t coreCodec, int32_t stereoConfigIndex,
                                         int32_t coreSbrFrameLengthIndex);

SACDEC_ERROR SpatialDecParseSpecificConfig(HANDLE_FDK_BITSTREAM     bitstream,
                                           SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig, int32_t sacHeaderLen,
                                           AUDIO_OBJECT_TYPE_t coreCodec);

int32_t SpatialDecDefaultSpecificConfig(SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig, AUDIO_OBJECT_TYPE_t coreCodec,
                                        int32_t samplingFreq, int32_t nTimeSlots, int32_t sacDecoderLevel,
                                        int32_t isBlind, int32_t coreChannels);

SACDEC_ERROR SpatialDecCreateBsFrame(SPATIAL_BS_FRAME *bsFrame, BS_LL_STATE *llState);

void SpatialDecCloseBsFrame(SPATIAL_BS_FRAME *bsFrame);

SACDEC_ERROR SpatialDecParseFrameData(spatialDec *self, SPATIAL_BS_FRAME *frame, HANDLE_FDK_BITSTREAM bitstream,
                                      const SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig, UPMIXTYPE upmixType,
                                      int32_t fGlobalIndependencyFlag);

SACDEC_ERROR SpatialDecDecodeFrame(spatialDec *self, SPATIAL_BS_FRAME *frame);

SACDEC_ERROR SpatialDecDecodeHeader(spatialDec *self, SPATIAL_SPECIFIC_CONFIG *pSpatialSpecificConfig);

#endif
