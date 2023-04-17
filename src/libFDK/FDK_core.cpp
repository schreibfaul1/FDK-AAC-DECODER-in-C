

/******************* Library for basic calculation routines ********************

   Author(s):   Manuel Jander

   Description: FDK tools versioning support

*******************************************************************************/
#include "../libAACdec/newAACDecoder.h"
#include "FDK_core.h"

/* FDK tools library info */
#define FDK_TOOLS_LIB_VL0 3
#define FDK_TOOLS_LIB_VL1 1
#define FDK_TOOLS_LIB_VL2 0
#define FDK_TOOLS_LIB_TITLE "FDK Tools"
#ifdef SUPPRESS_BUILD_DATE_INFO
#define FDK_TOOLS_LIB_BUILD_DATE ""
#define FDK_TOOLS_LIB_BUILD_TIME ""
#else
#define FDK_TOOLS_LIB_BUILD_DATE __DATE__
#define FDK_TOOLS_LIB_BUILD_TIME __TIME__
#endif

int32_t FDK_toolsGetLibInfo(LIB_INFO_t *info) {
  uint32_t v;
  int32_t i;

  if (info == NULL) {
    return -1;
  }

  /* search for next free tab */
  i = FDKlibInfo_lookup(info, FDK_TOOLS);
  if (i < 0) return -1;

  info += i;


  return 0;
}
