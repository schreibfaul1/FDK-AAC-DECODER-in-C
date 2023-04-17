
/**************************** PCM utility library ******************************

   Author(s):

   Description:

*******************************************************************************/
#include "../libAACdec/newAACDecoder.h"
#if !defined(VERSION_H)
#define VERSION_H

/* library info */
#define PCMUTIL_LIB_VL0 3
#define PCMUTIL_LIB_VL1 1
#define PCMUTIL_LIB_VL2 0
#define PCMUTIL_LIB_TITLE "PCM Utility Lib"
#ifdef SUPPRESS_BUILD_DATE_INFO
#define PCMUTIL_LIB_BUILD_DATE ""
#define PCMUTIL_LIB_BUILD_TIME ""
#else
#define PCMUTIL_LIB_BUILD_DATE __DATE__
#define PCMUTIL_LIB_BUILD_TIME __TIME__
#endif

#endif /* !defined(VERSION_H) */
