

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: error codes for mpeg surround decoder

*******************************************************************************/

#ifndef SAC_DEC_ERRORCODES_H
#define SAC_DEC_ERRORCODES_H
#include "../libAACdec/newAACDecoder.h"
typedef enum {

  __mps_error_start = -1000,

  MPS_OK = 0,

  /* generic/init errors */
  MPS_NOTOK = __mps_error_start,

  MPS_OUTOFMEMORY,
  MPS_INVALID_HANDLE,
  MPS_INVALID_PARAMETER,     /* SetParam not successfull */
  MPS_UNSUPPORTED_HRTFMODEL, /* SetHRTFModel() not successfull */
  MPS_UNSUPPORTED_HRTFFREQ,  /* SetHRTFModel() not successfull */

  MPS_UNSUPPORTED_UPMIX_TYPE, /* CheckLevelTreeUpmixType() */
  MPS_UNSUPPORTED_FORMAT, /* various functions; unknown aot or no_channels in
                             filterbank */
  MPS_OUTPUT_BUFFER_TOO_SMALL, /* Size of provided output time buffer is too
                                  small */

  /* ssc errors */
  MPS_INVALID_PARAMETERBANDS, /* unsupported numParameterBands in
                                 SpatialDecDecodeHeader() */
  MPS_INVALID_TREECONFIG,
  MPS_INVALID_HRTFSET,   /* SpatialDecDecodeHeader() */
  MPS_INVALID_TTT,       /* SpatialDecDecodeHeader() */
  MPS_INVALID_BOXIDX,    /* ecDataDec() */
  MPS_INVALID_SETIDX,    /* ecDataDec() */
  MPS_INVALID_QUANTMODE, /* SpatialDecParseSpecificConfig() */
  MPS_UNEQUAL_SSC, /* FDK_SpatialDecCompareSpatialSpecificConfigHeader() */
  MPS_UNSUPPORTED_CONFIG, /* number of core channels; 3DStereoInversion; */

  /* parse errors */
  MPS_PARSE_ERROR,
  MPS_INVALID_TEMPSHAPE, /* SpatialDecParseFrameData() */

  /* render errors */
  MPS_WRONG_PARAMETERSETS,
  MPS_WRONG_PARAMETERBANDS, /* decodeAndMapFrameSmg() */
  MPS_WRONG_TREECONFIG,
  MPS_WRONG_BLINDCONFIG,
  MPS_WRONG_OTT,
  MPS_WRONG_QUANTMODE,
  MPS_RESDEC_ERROR,
  MPS_APPLY_M2_ERROR, /* error in applyM2x()selection */

  __mps_error_end

} SACDEC_ERROR;

#endif
