

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief   Frequency scale prototypes
*/
#ifndef SBRDEC_FREQ_SCA_H
#define SBRDEC_FREQ_SCA_H
#include "../libAACdec/newAACDecoder.h"
#include "sbrdecoder.h"
#include "env_extr.h"

typedef enum { DUAL, QUAD } SBR_RATE;

SBR_ERROR
sbrdecUpdateFreqScale(uint8_t *v_k_master, uint8_t *numMaster, uint32_t fs,
                      HANDLE_SBR_HEADER_DATA headerData, uint32_t flags);

void sbrdecDownSampleLoRes(uint8_t *v_result, uint8_t num_result,
                           uint8_t *freqBandTableRef, uint8_t num_Ref);

void shellsort(uint8_t *in, uint8_t n);

SBR_ERROR
resetFreqBandTables(HANDLE_SBR_HEADER_DATA hHeaderData, const uint32_t flags);

#endif
