

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: temporal noise shaping tool

*******************************************************************************/

#ifndef AACDEC_TNS_H
#define AACDEC_TNS_H
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"

enum {
  TNS_MAX_WINDOWS = 8, /* 8 */
  TNS_MAXIMUM_FILTERS = 3
};

/* TNS_MAXIMUM_ORDER (for memory allocation)
   12 for AAC-LC and AAC-SSR. Set to 20 for AAC-Main (AOT 1). Some broken
   encoders also do order 20 for AAC-LC :( 15 for USAC (AOT 42)
*/
#define TNS_MAXIMUM_ORDER (20)

#if (TNS_MAXIMUM_ORDER < 15)
#error USAC: TNS filter order up 15 can be signaled!
#endif

typedef struct {
  int8_t Coeff[TNS_MAXIMUM_ORDER];

  uint8_t StartBand;
  uint8_t StopBand;

  int8_t Direction;
  int8_t Resolution;

  uint8_t Order;
} CFilter;

typedef struct {
  CFilter Filter[TNS_MAX_WINDOWS][TNS_MAXIMUM_FILTERS];
  uint8_t NumberOfFilters[TNS_MAX_WINDOWS];
  uint8_t DataPresent;
  uint8_t Active;

  /* log2 of the maximum total filter gains. The value is required to
     keep necessary mantissa headroom so that while applying the TNS predictor
     the mantissas do not overflow. */
  uint8_t GainLd;
} CTnsData;

void CTns_Reset(CTnsData *pTnsData);

#endif /* #ifndef AACDEC_TNS_H */
