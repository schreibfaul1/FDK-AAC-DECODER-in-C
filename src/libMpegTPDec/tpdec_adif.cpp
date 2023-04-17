
/******************* MPEG transport format decoder library *********************

   Author(s):   Josef Hoepfl

   Description: ADIF reader

*******************************************************************************/
#include "../libAACdec/newAACDecoder.h"
#include "tpdec_adif.h"

#include "../libFDK/FDK_bitstream.h"


TRANSPORTDEC_ERROR adifRead_DecodeHeader(CAdifHeader *pAdifHeader,
                                         CProgramConfig *pPce,
                                         HANDLE_FDK_BITSTREAM bs) {
  int32_t i;
  TRANSPORTDEC_ERROR ErrorStatus = TRANSPORTDEC_OK;
  uint32_t startAnchor = FDKgetValidBits(bs);

  if ((int32_t)startAnchor < MIN_ADIF_HEADERLENGTH) {
    return (TRANSPORTDEC_NOT_ENOUGH_BITS);
  }

  if (FDKreadBits(bs, 8) != 'A') {
    return (TRANSPORTDEC_SYNC_ERROR);
  }
  if (FDKreadBits(bs, 8) != 'D') {
    return (TRANSPORTDEC_SYNC_ERROR);
  }
  if (FDKreadBits(bs, 8) != 'I') {
    return (TRANSPORTDEC_SYNC_ERROR);
  }
  if (FDKreadBits(bs, 8) != 'F') {
    return (TRANSPORTDEC_SYNC_ERROR);
  }

  if ((pAdifHeader->CopyrightIdPresent = FDKreadBits(bs, 1)) != 0)
    FDKpushBiDirectional(bs, 72); /* CopyrightId */

  pAdifHeader->OriginalCopy = FDKreadBits(bs, 1);
  pAdifHeader->Home = FDKreadBits(bs, 1);
  pAdifHeader->BitstreamType = FDKreadBits(bs, 1);

  /* pAdifHeader->BitRate = FDKreadBits(bs, 23); */
  pAdifHeader->BitRate = FDKreadBits(bs, 16);
  pAdifHeader->BitRate <<= 7;
  pAdifHeader->BitRate |= FDKreadBits(bs, 7);

  pAdifHeader->NumProgramConfigElements = FDKreadBits(bs, 4) + 1;

  if (pAdifHeader->BitstreamType == 0) {
    FDKpushBiDirectional(bs, 20); /* adif_buffer_fullness */
  }

  /* Parse all PCEs but keep only one */
  for (i = 0; i < pAdifHeader->NumProgramConfigElements; i++) {
    CProgramConfig_Read(pPce, bs, startAnchor);
  }

  FDKbyteAlign(bs, startAnchor);

  return (ErrorStatus);
}
