

/******************* MPEG transport format decoder library *********************

   Author(s):   Manuel Jander

   Description: MPEG Transport decoder

*******************************************************************************/

#ifndef TPDEC_LIB_H
#define TPDEC_LIB_H
#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/FDK_bitstream.h"
#include "tp_data.h"

/* CELP stuff */
enum { MPE = 0, RPE = 1, fs8KHz = 0, fs16KHz = 1 };

/* Defintion of flags that can be passed to transportDecOpen() */
#define TP_FLAG_MPEG4 1

/* Capability flags */

typedef struct TRANSPORTDEC *HANDLE_TRANSPORTDEC;

TRANSPORTDEC_ERROR_t transportDec_OutOfBandConfig(const HANDLE_TRANSPORTDEC hTp, uint8_t *conf, const uint32_t length, const uint32_t layer);
TRANSPORTDEC_ERROR_t transportDec_InBandConfig(const HANDLE_TRANSPORTDEC hTp, uint8_t *newConfig, const uint32_t newConfigLength,
                                               const uint8_t buildUpStatus, uint8_t *configChanged, const uint32_t layer,
                                               uint8_t *implicitExplicitCfgDiff);
HANDLE_TRANSPORTDEC  transportDec_Open(TRANSPORT_TYPE_t transportDecFmt, const uint32_t flags, const uint32_t nrOfLayer);
int32_t              transportDec_RegisterAscCallback(HANDLE_TRANSPORTDEC hTp, const cbUpdateConfig_t cbUpdateConfig, void *user_data);
int32_t              transportDec_RegisterFreeMemCallback(HANDLE_TRANSPORTDEC hTp, const cbFreeMem_t cbFreeMem, void *user_data);
int32_t              transportDec_RegisterCtrlCFGChangeCallback(HANDLE_TRANSPORTDEC hTp, const cbCtrlCFGChange_t cbCtrlCFGChange, void *user_data);
int32_t              transportDec_RegisterSscCallback(HANDLE_TRANSPORTDEC hTp, const cbSsc_t cbSscParse, void *user_data);
int32_t              transportDec_RegisterSbrCallback(HANDLE_TRANSPORTDEC hTpDec, const cbSbr_t cbSbr, void *user_data);
int32_t              transportDec_RegisterUsacCallback(HANDLE_TRANSPORTDEC hTpDec, const cbUsac_t cbUsac, void *user_data);
int32_t              transportDec_RegisterUniDrcConfigCallback(HANDLE_TRANSPORTDEC hTpDec, const cbUniDrc_t cbUniDrc, void *user_data,
                                                               uint32_t *pLoudnessInfoSetPosition);
TRANSPORTDEC_ERROR_t transportDec_FillData(const HANDLE_TRANSPORTDEC hTp, uint8_t *pBuffer, const uint32_t bufferSize, uint32_t *pBytesValid,
                                           const int32_t layer);
HANDLE_FDK_BITSTREAM transportDec_GetBitstream(const HANDLE_TRANSPORTDEC hTp, const uint32_t layer);
TRANSPORT_TYPE_t     transportDec_GetFormat(const HANDLE_TRANSPORTDEC hTp);
int32_t              transportDec_GetBufferFullness(const HANDLE_TRANSPORTDEC hTp);
void                 transportDec_Close(HANDLE_TRANSPORTDEC *phTp);
TRANSPORTDEC_ERROR_t transportDec_ReadAccessUnit(const HANDLE_TRANSPORTDEC hTp, const uint32_t layer);
TRANSPORTDEC_ERROR_t transportDec_GetAsc(const HANDLE_TRANSPORTDEC hTp, const uint32_t layer, CSAudioSpecificConfig_t *asc);
int32_t              transportDec_GetAuBitsRemaining(const HANDLE_TRANSPORTDEC hTp, const uint32_t layer);
int32_t              transportDec_GetAuBitsTotal(const HANDLE_TRANSPORTDEC hTp, const uint32_t layer);
TRANSPORTDEC_ERROR_t transportDec_EndAccessUnit(const HANDLE_TRANSPORTDEC hTp);
TRANSPORTDEC_ERROR_t transportDec_GetMissingAccessUnitCount(int32_t *pNAccessUnits, HANDLE_TRANSPORTDEC hTp);
TRANSPORTDEC_ERROR_t transportDec_SetParam(const HANDLE_TRANSPORTDEC hTp, const TPDEC_PARAM param, const int32_t value);
uint32_t             transportDec_GetNrOfSubFrames(HANDLE_TRANSPORTDEC hTp);
TRANSPORTDEC_ERROR_t transportDec_GetLibInfo(LIB_INFO_t *info);
int32_t              transportDec_CrcStartReg(const HANDLE_TRANSPORTDEC hTp, const int32_t mBits);
void                 transportDec_CrcEndReg(const HANDLE_TRANSPORTDEC hTp, const int32_t reg);
TRANSPORTDEC_ERROR_t transportDec_CrcCheck(const HANDLE_TRANSPORTDEC hTp);
TRANSPORTDEC_ERROR_t transportDec_DrmRawSdcAudioConfig_Check(uint8_t *conf, const uint32_t length);

#endif /* #ifndef TPDEC_LIB_H */
