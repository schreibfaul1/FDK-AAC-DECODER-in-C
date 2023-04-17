
/******************* MPEG transport format decoder library *********************

   Author(s):

   Description:

*******************************************************************************/
#include "../libAACdec/newAACDecoder.h"
#if !defined(TP_VERSION_H)
#define TP_VERSION_H

/* library info */
#define TP_LIB_VL0 3
#define TP_LIB_VL1 0
#define TP_LIB_VL2 0
#define TP_LIB_TITLE "MPEG Transport"
#ifdef __ANDROID__
#define TP_LIB_BUILD_DATE ""
#define TP_LIB_BUILD_TIME ""
#else
#define TP_LIB_BUILD_DATE __DATE__
#define TP_LIB_BUILD_TIME __TIME__
#endif
#endif /* !defined(TP_VERSION_H) */
