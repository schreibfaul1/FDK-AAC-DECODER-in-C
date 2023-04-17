

/******************* Library for basic calculation routines ********************

   Author(s):   Manuel Jander

   Description: FDK tools versioning support

*******************************************************************************/

#ifndef FDK_CORE_H
#define FDK_CORE_H


#include "../libAACdec/newAACDecoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Get FDK_tools library information.
 *  @return     Return 0 on success and a negative errorcode on failure (see
 * errorcodes.h).
 */
int32_t FDK_toolsGetLibInfo(LIB_INFO_t *info);

#ifdef __cplusplus
}
#endif

#endif
