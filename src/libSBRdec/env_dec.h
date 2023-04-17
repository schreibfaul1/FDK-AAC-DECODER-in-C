
/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  Envelope decoding
*/
#ifndef ENV_DEC_H
#define ENV_DEC_H

#include "../libAACdec/newAACDecoder.h"
#include "sbrdecoder.h"
#include "env_extr.h"

void decodeSbrData(HANDLE_SBR_HEADER_DATA hHeaderData,
                   HANDLE_SBR_FRAME_DATA h_data_left,
                   HANDLE_SBR_PREV_FRAME_DATA h_prev_data_left,
                   HANDLE_SBR_FRAME_DATA h_data_right,
                   HANDLE_SBR_PREV_FRAME_DATA h_prev_data_right);

#endif
