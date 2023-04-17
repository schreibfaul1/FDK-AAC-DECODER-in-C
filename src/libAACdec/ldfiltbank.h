

/**************************** AAC decoder library ******************************

   Author(s):

   Description: low delay filterbank interface

*******************************************************************************/

#ifndef LDFILTBANK_H
#define LDFILTBANK_H

#include <stdint.h>
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"
#include "aac_rom.h"

int32_t InvMdctTransformLowDelay_fdk(int32_t *mdctdata_m, const int32_t mdctdata_e,
                                 int32_t *mdctOut, int32_t *fs_buffer,
                                 const int32_t frameLength);

#endif
