

/******************* MPEG transport format decoder library *********************

   Author(s):   Josef Hoepfl

   Description: ADIF reader

*******************************************************************************/

#ifndef TPDEC_ADIF_H
#define TPDEC_ADIF_H
#include "../libAACdec/newAACDecoder.h"
#include "tpdec_lib.h"

#define MIN_ADIF_HEADERLENGTH 63 /* in bits */

typedef struct {
  int32_t NumProgramConfigElements;
  uint32_t BitRate;
  uint8_t CopyrightIdPresent;
  uint8_t OriginalCopy;
  uint8_t Home;
  uint8_t BitstreamType;
} CAdifHeader;

/**
 * \brief Parse a ADIF header at the given bitstream and store the parsed data
 * into a given CAdifHeader and CProgramConfig struct
 *
 * \param pAdifHeader pointer to a CAdifHeader structure to hold the parsed ADIF
 * header data.
 * \param pPce pointer to a CProgramConfig structure where the last PCE will
 * remain.
 *
 * \return TRANSPORTDEC_ERROR error code
 */
TRANSPORTDEC_ERROR adifRead_DecodeHeader(CAdifHeader *pAdifHeader,
                                         CProgramConfig *pPce,
                                         HANDLE_FDK_BITSTREAM bs);

#endif /* TPDEC_ADIF_H */
