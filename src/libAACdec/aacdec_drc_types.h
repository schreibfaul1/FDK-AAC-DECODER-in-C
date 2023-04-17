

/**************************** AAC decoder library ******************************

   Author(s):   Christian Griebel

   Description: Dynamic range control (DRC) global data types

*******************************************************************************/

#ifndef AACDEC_DRC_TYPES_H
#define AACDEC_DRC_TYPES_H
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"



/**
 * \brief DRC module global data types
 */
typedef enum {
  UNKNOWN_PAYLOAD = 0,
  MPEG_DRC_EXT_DATA = 1,
  DVB_DRC_ANC_DATA = 2

} AACDEC_DRC_PAYLOAD_TYPE;

/**
 * \brief Options for parameter handling / presentation mode
 */


typedef CDrcInfo_t *HANDLE_AAC_DRC;

#endif /* AACDEC_DRC_TYPES_H */
