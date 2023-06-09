

/**************************** AAC decoder library ******************************

   Author(s):   Manuel Jander

   Description:

*******************************************************************************/

#include <memory.h>
#include <stdint.h>
#include "newAACDecoder.h"
#include "aacdecoder_lib.h"

#include <stdio.h>
#include "../libFDK/FDK_core.h"
#include "../libMpegTPDec/tpdec_lib.h"
#include "../libPCMutils/pcm_utils.h"
#include "../libSACdec/sac_dec_lib.h"
#include "../libSBRdec/sbrdecoder.h"

#include "aacdecoder.h"


/* Decoder library info */
#define AACDECODER_LIB_VL0   3
#define AACDECODER_LIB_VL1   2
#define AACDECODER_LIB_VL2   0
#define AACDECODER_LIB_TITLE "AAC Decoder Lib"
#ifdef SUPPRESS_BUILD_DATE_INFO
	#define AACDECODER_LIB_BUILD_DATE ""
	#define AACDECODER_LIB_BUILD_TIME ""
#else
	#define AACDECODER_LIB_BUILD_DATE __DATE__
	#define AACDECODER_LIB_BUILD_TIME __TIME__
#endif

static AAC_DECODER_ERROR_t setConcealMethod(const HANDLE_AACDECODER self, const int32_t method);

static void aacDecoder_setMetadataExpiry(const HANDLE_AACDECODER self, const int32_t value) {
	/* check decoder handle */
	if(self != NULL) {
		int32_t mdExpFrame = 0; /* default: disable */

		if((value > 0) &&
		   (self->streamInfo.aacSamplesPerFrame > 0)) { /* Determine the corresponding number of frames: */
			int32_t frameTime = fDivNorm(self->streamInfo.aacSampleRate, self->streamInfo.aacSamplesPerFrame * 1000);
			mdExpFrame = fMultIceil(frameTime, value);
		}

		/* Configure DRC module */
		aacDecoder_drcSetParam(self->hDrcInfo, DRC_DATA_EXPIRY_FRAME, mdExpFrame);

		/* Configure PCM downmix module */
		pcmDmx_SetParam(self->hPcmUtils, DMX_BS_DATA_EXPIRY_FRAME, mdExpFrame);
	}
}

AAC_DECODER_ERROR_t
aacDecoder_GetFreeBytes(const HANDLE_AACDECODER self, uint32_t *pFreeBytes) {
	/* reset free bytes */
	*pFreeBytes = 0;

	/* check handle */
	if(!self) return AAC_DEC_INVALID_HANDLE;

	/* return nr of free bytes */
	HANDLE_FDK_BITSTREAM hBs = transportDec_GetBitstream(self->hInput, 0);
	*pFreeBytes = FDKgetFreeBits(hBs) >> 3;

	/* success */
	return AAC_DEC_OK;
}

/**
 * Config Decoder using a CSAudioSpecificConfig_t struct.
 */
static AAC_DECODER_ERROR_t aacDecoder_Config(HANDLE_AACDECODER self, const CSAudioSpecificConfig_t *pAscStruct,
										   uint8_t configMode, uint8_t *configChanged) {
	AAC_DECODER_ERROR_t err;

	/* Initialize AAC core decoder, and update self->streaminfo */
	err = CAacDecoder_Init(self, pAscStruct, configMode, configChanged);

	if(!FDK_chMapDescr_isValid(&self->mapDescr)) { return AAC_DEC_UNSUPPORTED_CHANNELCONFIG; }

	return err;
}

AAC_DECODER_ERROR_t aacDecoder_ConfigRaw(HANDLE_AACDECODER self, uint8_t *conf[], const uint32_t length[]) {
	AAC_DECODER_ERROR_t  err = AAC_DEC_OK;
	TRANSPORTDEC_ERROR_t errTp;
	uint32_t               layer, nrOfLayers = self->nrOfLayers;

	for(layer = 0; layer < nrOfLayers; layer++) {
		if(length[layer] > 0) {
			errTp = transportDec_OutOfBandConfig(self->hInput, conf[layer], length[layer], layer);
			if(errTp != TRANSPORTDEC_OK) {
				switch(errTp) {
					case TRANSPORTDEC_NEED_TO_RESTART:
						err = AAC_DEC_NEED_TO_RESTART;
						break;
					case TRANSPORTDEC_UNSUPPORTED_FORMAT:
						err = AAC_DEC_UNSUPPORTED_FORMAT;
						break;
					default:
						err = AAC_DEC_UNKNOWN;
						break;
				}
				/* if baselayer is OK we continue decoding */
				if(layer >= 1) {
					self->nrOfLayers = layer;
					err = AAC_DEC_OK;
				}
				break;
			}
		}
	}

	return err;
}

AAC_DECODER_ERROR_t aacDecoder_RawISOBMFFData(HANDLE_AACDECODER self, uint8_t *buffer, uint32_t length) {
	FDK_BITSTREAM_t        bs;
	HANDLE_FDK_BITSTREAM hBs = &bs;
	AAC_DECODER_ERROR_t    err = AAC_DEC_OK;

	if(length < 8) return AAC_DEC_UNKNOWN;

	while(length >= 8) {
		uint32_t          size = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
		DRC_DEC_ERROR uniDrcErr = DRC_DEC_OK;

		if(length < size) return AAC_DEC_UNKNOWN;
		if(size <= 8) return AAC_DEC_UNKNOWN;

		FDKinitBitStream(hBs, buffer + 8, 0x10000000, (size - 8) * 8);

		if((buffer[4] == 'l') && (buffer[5] == 'u') && (buffer[6] == 'd') && (buffer[7] == 't')) {
			uniDrcErr = FDK_drcDec_ReadLoudnessBox(self->hUniDrcDecoder, hBs);
		}
		else if((buffer[4] == 'd') && (buffer[5] == 'm') && (buffer[6] == 'i') && (buffer[7] == 'x')) {
			uniDrcErr = FDK_drcDec_ReadDownmixInstructions_Box(self->hUniDrcDecoder, hBs);
		}
		else if((buffer[4] == 'u') && (buffer[5] == 'd') && (buffer[6] == 'i') && (buffer[7] == '2')) {
			uniDrcErr = FDK_drcDec_ReadUniDrcInstructions_Box(self->hUniDrcDecoder, hBs);
		}
		else if((buffer[4] == 'u') && (buffer[5] == 'd') && (buffer[6] == 'c') && (buffer[7] == '2')) {
			uniDrcErr = FDK_drcDec_ReadUniDrcCoefficients_Box(self->hUniDrcDecoder, hBs);
		}

		if(uniDrcErr != DRC_DEC_OK) err = AAC_DEC_UNKNOWN;

		buffer += size;
		length -= size;
	}

	return err;
}

static int32_t aacDecoder_ConfigCallback(void *handle, const CSAudioSpecificConfig_t *pAscStruct, uint8_t configMode,
									 uint8_t *configChanged) {
	HANDLE_AACDECODER  self = (HANDLE_AACDECODER)handle;
	AAC_DECODER_ERROR_t  err = AAC_DEC_OK;
	TRANSPORTDEC_ERROR_t errTp;

	assert(self != NULL);
	{
		{ err = aacDecoder_Config(self, pAscStruct, configMode, configChanged); }
	}
	if(err == AAC_DEC_OK) {
		/*
		revert concealment method if either
		   - Interpolation concealment might not be meaningful
		   - Interpolation concealment is not implemented
		*/
		if((self->flags[0] & (AC_LD | AC_ELD) && (self->concealMethodUser == ConcealMethodNone) &&
			CConcealment_GetDelay(&self->concealCommonData) > 0) /* might not be meaningful but allow if user has set it
																	expicitly */
		   || (self->flags[0] & (AC_USAC | AC_RSVD50 | AC_RSV603DA) &&
			   CConcealment_GetDelay(&self->concealCommonData) > 0) /* not implemented */
		) {
			/* Revert to error concealment method Noise Substitution.
			   Because interpolation is not implemented for USAC or
			   the additional delay is unwanted for low delay codecs. */
			setConcealMethod(self, 1);
		}
		aacDecoder_setMetadataExpiry(self, self->metadataExpiry);
		errTp = TRANSPORTDEC_OK;
	}
	else {
		if(err == AAC_DEC_NEED_TO_RESTART) { errTp = TRANSPORTDEC_NEED_TO_RESTART; }
		else if(IS_INIT_ERROR(err)) { errTp = TRANSPORTDEC_UNSUPPORTED_FORMAT; } /* Fatal errors */
		else { errTp = TRANSPORTDEC_UNKOWN_ERROR; }
	}

	return errTp;
}

static int32_t aacDecoder_FreeMemCallback(void *handle, const CSAudioSpecificConfig_t *pAscStruct) {
	TRANSPORTDEC_ERROR_t errTp = TRANSPORTDEC_OK;
	HANDLE_AACDECODER  self = (HANDLE_AACDECODER)handle;

	const int32_t subStreamIndex = 0;

	assert(self != NULL);

	if(CAacDecoder_FreeMem(self, subStreamIndex) != AAC_DEC_OK) { errTp = TRANSPORTDEC_UNKOWN_ERROR; }

	/* free Ram_SbrDecoder and Ram_SbrDecChannel */
	if(self->hSbrDecoder != NULL) {
		if(sbrDecoder_FreeMem(&self->hSbrDecoder) != SBRDEC_OK) { errTp = TRANSPORTDEC_UNKOWN_ERROR; }
	}

	/* free pSpatialDec and mpsData */
	if(self->pMpegSurroundDecoder != NULL) {
		if(mpegSurroundDecoder_FreeMem((CMpegSurroundDecoder *)self->pMpegSurroundDecoder) != MPS_OK) {
			errTp = TRANSPORTDEC_UNKOWN_ERROR;
		}
	}

	/* free persistent qmf domain buffer, QmfWorkBufferCore3, QmfWorkBufferCore4,
	 * QmfWorkBufferCore5 and configuration variables */
	FDK_QmfDomain_FreeMem(&self->qmfDomain);

	return errTp;
}

static int32_t aacDecoder_CtrlCFGChangeCallback(void *handle, const CCtrlCFGChange_t *pCtrlCFGChangeStruct) {
	TRANSPORTDEC_ERROR_t errTp = TRANSPORTDEC_OK;
	HANDLE_AACDECODER  self = (HANDLE_AACDECODER)handle;

	if(self != NULL) {
		CAacDecoder_CtrlCFGChange(self, pCtrlCFGChangeStruct->flushStatus, pCtrlCFGChangeStruct->flushCnt,
								  pCtrlCFGChangeStruct->buildUpStatus, pCtrlCFGChangeStruct->buildUpCnt);
	}
	else { errTp = TRANSPORTDEC_UNKOWN_ERROR; }

	return errTp;
}

static int32_t aacDecoder_SbrCallback(void *handle, HANDLE_FDK_BITSTREAM hBs, const int32_t sampleRateIn,
								  const int32_t sampleRateOut, const int32_t samplesPerFrame, const AUDIO_OBJECT_TYPE_t coreCodec,
								  const MP4_ELEMENT_ID_t elementID, const int32_t elementIndex, const uint8_t harmonicSBR,
								  const uint8_t stereoConfigIndex, const uint8_t configMode, uint8_t *configChanged,
								  const int32_t downscaleFactor) {
	HANDLE_SBRDECODER self = (HANDLE_SBRDECODER)handle;

	int32_t errTp =
		sbrDecoder_Header(self, hBs, sampleRateIn, sampleRateOut, samplesPerFrame, coreCodec, elementID, elementIndex,
						  harmonicSBR, stereoConfigIndex, configMode, configChanged, downscaleFactor);

	return errTp;
}

static int32_t aacDecoder_SscCallback(void *handle, HANDLE_FDK_BITSTREAM hBs, const AUDIO_OBJECT_TYPE_t coreCodec,
								  const int32_t samplingRate, const int32_t frameSize, const int32_t stereoConfigIndex,
								  const int32_t coreSbrFrameLengthIndex, const int32_t configBytes, const uint8_t configMode,
								  uint8_t *configChanged) {
	SACDEC_ERROR       err;
	TRANSPORTDEC_ERROR_t errTp;
	HANDLE_AACDECODER  hAacDecoder = (HANDLE_AACDECODER)handle;

	err = mpegSurroundDecoder_Config((CMpegSurroundDecoder *)hAacDecoder->pMpegSurroundDecoder, hBs, coreCodec,
									 samplingRate, frameSize, stereoConfigIndex, coreSbrFrameLengthIndex, configBytes,
									 configMode, configChanged);

	switch(err) {
		case MPS_UNSUPPORTED_CONFIG:
			/* MPS found but invalid or not decodable by this instance            */
			/* We switch off MPS and keep going                                   */
			hAacDecoder->mpsEnableCurr = 0;
			hAacDecoder->mpsApplicable = 0;
			errTp = TRANSPORTDEC_OK;
			break;
		case MPS_PARSE_ERROR:
			/* MPS found but invalid or not decodable by this instance            */
			hAacDecoder->mpsEnableCurr = 0;
			hAacDecoder->mpsApplicable = 0;
			if((coreCodec == AOT_USAC) || (coreCodec == AOT_DRM_USAC) || IS_LOWDELAY(coreCodec)) {
				errTp = TRANSPORTDEC_PARSE_ERROR;
			}
			else { errTp = TRANSPORTDEC_OK; }
			break;
		case MPS_OK:
			hAacDecoder->mpsApplicable = 1;
			errTp = TRANSPORTDEC_OK;
			break;
		default:
			/* especially Parsing error is critical for transport layer          */
			hAacDecoder->mpsApplicable = 0;
			errTp = TRANSPORTDEC_UNKOWN_ERROR;
	}

	return (int32_t)errTp;
}

static int32_t aacDecoder_UniDrcCallback(void *handle, HANDLE_FDK_BITSTREAM hBs, const int32_t fullPayloadLength,
									 const int32_t payloadType, const int32_t subStreamIndex, const int32_t payloadStart,
									 const AUDIO_OBJECT_TYPE_t aot) {
	DRC_DEC_ERROR      err = DRC_DEC_OK;
	TRANSPORTDEC_ERROR_t errTp;
	HANDLE_AACDECODER  hAacDecoder = (HANDLE_AACDECODER)handle;
	DRC_DEC_CODEC_MODE drcDecCodecMode = DRC_DEC_CODEC_MODE_UNDEFINED;

	if(subStreamIndex != 0) { return TRANSPORTDEC_OK; }

	else if(aot == AOT_USAC) { drcDecCodecMode = DRC_DEC_MPEG_D_USAC; }

	err = FDK_drcDec_SetCodecMode(hAacDecoder->hUniDrcDecoder, drcDecCodecMode);
	if(err) return (int32_t)TRANSPORTDEC_UNKOWN_ERROR;

	if(payloadType == 0) /* uniDrcConfig */
	{
		err = FDK_drcDec_ReadUniDrcConfig(hAacDecoder->hUniDrcDecoder, hBs);
	}
	else /* loudnessInfoSet */
	{
		err = FDK_drcDec_ReadLoudnessInfoSet(hAacDecoder->hUniDrcDecoder, hBs);
		hAacDecoder->loudnessInfoSetPosition[1] = payloadStart;
		hAacDecoder->loudnessInfoSetPosition[2] = fullPayloadLength;
	}

	if(err == DRC_DEC_OK) errTp = TRANSPORTDEC_OK;
	else
		errTp = TRANSPORTDEC_UNKOWN_ERROR;

	return (int32_t)errTp;
}

AAC_DECODER_ERROR_t aacDecoder_AncDataInit(HANDLE_AACDECODER self, uint8_t *buffer, int32_t size) {
	CAncData *ancData = &self->ancData;

	return CAacDecoder_AncDataInit(ancData, buffer, size);
}

AAC_DECODER_ERROR_t aacDecoder_AncDataGet(HANDLE_AACDECODER self, int32_t index, uint8_t **ptr, int32_t *size) {
	CAncData *ancData = &self->ancData;

	return CAacDecoder_AncDataGet(ancData, index, ptr, size);
}

/* If MPS is present in stream, but not supported by this instance, we'll
   have to switch off MPS and use QMF synthesis in the SBR module if required */
static int32_t isSupportedMpsConfig(AUDIO_OBJECT_TYPE_t aot, uint32_t numInChannels, uint32_t fMpsPresent) {
	LIB_INFO_t libInfo[FDK_MODULE_LAST];
	uint32_t     mpsCaps;
	int32_t      isSupportedCfg = 1;

	FDKinitLibInfo(libInfo);

	mpegSurroundDecoder_GetLibInfo(libInfo);

	mpsCaps = FDKlibInfo_getCapabilities(libInfo, FDK_MPSDEC);

	if(!(mpsCaps & CAPF_MPS_LD) && IS_LOWDELAY(aot)) {
		/* We got an LD AOT but MPS decoder does not support LD. */
		isSupportedCfg = 0;
	}
	if((mpsCaps & CAPF_MPS_LD) && IS_LOWDELAY(aot) && !fMpsPresent) {
		/* We got an LD AOT and the MPS decoder supports it.
		 * But LD-MPS is not explicitly signaled. */
		isSupportedCfg = 0;
	}
	if(!(mpsCaps & CAPF_MPS_USAC) && IS_USAC(aot)) {
		/* We got an USAC AOT but MPS decoder does not support USAC. */
		isSupportedCfg = 0;
	}
	if(!(mpsCaps & CAPF_MPS_STD) && !IS_LOWDELAY(aot) && !IS_USAC(aot)) {
		/* We got an GA AOT but MPS decoder does not support it. */
		isSupportedCfg = 0;
	}
	/* Check whether the MPS modul supports the given number of input channels: */
	switch(numInChannels) {
		case 1:
			if(!(mpsCaps & CAPF_MPS_1CH_IN)) {
				/* We got a one channel input to MPS decoder but it does not support it.
				 */
				isSupportedCfg = 0;
			}
			break;
		case 2:
			if(!(mpsCaps & CAPF_MPS_2CH_IN)) {
				/* We got a two channel input to MPS decoder but it does not support it.
				 */
				isSupportedCfg = 0;
			}
			break;
		case 5:
		case 6:
			if(!(mpsCaps & CAPF_MPS_6CH_IN)) {
				/* We got a six channel input to MPS decoder but it does not support it.
				 */
				isSupportedCfg = 0;
			}
			break;
		default:
			isSupportedCfg = 0;
	}

	return (isSupportedCfg);
}

static AAC_DECODER_ERROR_t setConcealMethod(const HANDLE_AACDECODER self, /*!< Handle of the decoder instance */
										  const int32_t               method) {
	AAC_DECODER_ERROR_t  errorStatus = AAC_DEC_OK;
	CConcealParams_t    *pConcealData = NULL;
	int32_t                method_revert = 0;
	HANDLE_SBRDECODER  hSbrDec = NULL;
	HANDLE_AAC_DRC     hDrcInfo = NULL;
	HANDLE_PCM_DOWNMIX hPcmDmx = NULL;
	CConcealmentMethod_t backupMethod = ConcealMethodNone;
	int32_t                backupDelay = 0;
	int32_t                bsDelay = 0;

	/* check decoder handle */
	if(self != NULL) {
		pConcealData = &self->concealCommonData;
		hSbrDec = self->hSbrDecoder;
		hDrcInfo = self->hDrcInfo;
		hPcmDmx = self->hPcmUtils;
		if(self->flags[0] & (AC_USAC | AC_RSVD50 | AC_RSV603DA) && method >= 2) {
			/* Interpolation concealment is not implemented for USAC/RSVD50 */
			/* errorStatus = AAC_DEC_SET_PARAM_FAIL;
			   goto bail; */
			method_revert = 1;
		}
		if(self->flags[0] & (AC_USAC | AC_RSVD50 | AC_RSV603DA) && method >= 2) {
			/* Interpolation concealment is not implemented for USAC/RSVD50 */
			errorStatus = AAC_DEC_SET_PARAM_FAIL;
			goto bail;
		}
	}

	/* Get current method/delay */
	backupMethod = CConcealment_GetMethod(pConcealData);
	backupDelay = CConcealment_GetDelay(pConcealData);

	/* Be sure to set AAC and SBR concealment method simultaneously! */
	errorStatus = CConcealment_SetParams(pConcealData,
										 (method_revert == 0) ? (int32_t)method : (int32_t)1,  // concealMethod
										 AACDEC_CONCEAL_PARAM_NOT_SPECIFIED,           // concealFadeOutSlope
										 AACDEC_CONCEAL_PARAM_NOT_SPECIFIED,           // concealFadeInSlope
										 AACDEC_CONCEAL_PARAM_NOT_SPECIFIED,           // concealMuteRelease
										 AACDEC_CONCEAL_PARAM_NOT_SPECIFIED            // concealComfNoiseLevel
	);
	if((errorStatus != AAC_DEC_OK) && (errorStatus != AAC_DEC_INVALID_HANDLE)) { goto bail; }

	/* Get new delay */
	bsDelay = CConcealment_GetDelay(pConcealData);

	{
		SBR_ERROR sbrErr = SBRDEC_OK;

		/* set SBR bitstream delay */
		sbrErr = sbrDecoder_SetParam(hSbrDec, SBR_SYSTEM_BITSTREAM_DELAY, bsDelay);

		switch(sbrErr) {
			case SBRDEC_OK:
			case SBRDEC_NOT_INITIALIZED:
				if(self != NULL) {
					/* save the param value and set later
					   (when SBR has been initialized) */
					self->sbrParams.bsDelay = bsDelay;
				}
				break;
			default:
				errorStatus = AAC_DEC_SET_PARAM_FAIL;
				goto bail;
		}
	}

	errorStatus = aacDecoder_drcSetParam(hDrcInfo, DRC_BS_DELAY, bsDelay);
	if((errorStatus != AAC_DEC_OK) && (errorStatus != AAC_DEC_INVALID_HANDLE)) { goto bail; }

	if(errorStatus == AAC_DEC_OK) {
		PCMDMX_ERROR err = pcmDmx_SetParam(hPcmDmx, DMX_BS_DATA_DELAY, bsDelay);
		switch(err) {
			case PCMDMX_INVALID_HANDLE:
				errorStatus = AAC_DEC_INVALID_HANDLE;
				break;
			case PCMDMX_OK:
				break;
			default:
				errorStatus = AAC_DEC_SET_PARAM_FAIL;
				goto bail;
		}
	}

bail:
	if((errorStatus != AAC_DEC_OK) && (errorStatus != AAC_DEC_INVALID_HANDLE)) {
		/* Revert to the initial state */
		CConcealment_SetParams(pConcealData, (int32_t)backupMethod, AACDEC_CONCEAL_PARAM_NOT_SPECIFIED,
							   AACDEC_CONCEAL_PARAM_NOT_SPECIFIED, AACDEC_CONCEAL_PARAM_NOT_SPECIFIED,
							   AACDEC_CONCEAL_PARAM_NOT_SPECIFIED);
		/* Revert SBR bitstream delay */
		sbrDecoder_SetParam(hSbrDec, SBR_SYSTEM_BITSTREAM_DELAY, backupDelay);
		/* Revert DRC bitstream delay */
		aacDecoder_drcSetParam(hDrcInfo, DRC_BS_DELAY, backupDelay);
		/* Revert PCM mixdown bitstream delay */
		pcmDmx_SetParam(hPcmDmx, DMX_BS_DATA_DELAY, backupDelay);
	}

	return errorStatus;
}

AAC_DECODER_ERROR_t aacDecoder_SetParam(const HANDLE_AACDECODER self,  /*!< Handle of the decoder instance */
									  const AACDEC_PARAM_t      param, /*!< Parameter to set               */
									  const int32_t               value)               /*!< Parameter valued               */
{
	AAC_DECODER_ERROR_t   errorStatus = AAC_DEC_OK;
	HANDLE_TRANSPORTDEC hTpDec = NULL;
	TRANSPORTDEC_ERROR_t  errTp = TRANSPORTDEC_OK;
	HANDLE_AAC_DRC      hDrcInfo = NULL;
	HANDLE_PCM_DOWNMIX  hPcmDmx = NULL;
	PCMDMX_ERROR        dmxErr = PCMDMX_OK;
	TDLimiterPtr        hPcmTdl = NULL;
	DRC_DEC_ERROR       uniDrcErr = DRC_DEC_OK;

	/* check decoder handle */
	if(self != NULL) {
		hTpDec = self->hInput;
		hDrcInfo = self->hDrcInfo;
		hPcmDmx = self->hPcmUtils;
		hPcmTdl = self->hLimiter;
	}
	else {
		errorStatus = AAC_DEC_INVALID_HANDLE;
		goto bail;
	}

	/* configure the subsystems */
	switch(param) {
		case AAC_PCM_MIN_OUTPUT_CHANNELS:
			if(value < -1 || value > (8)) { return AAC_DEC_SET_PARAM_FAIL; }
			dmxErr = pcmDmx_SetParam(hPcmDmx, MIN_NUMBER_OF_OUTPUT_CHANNELS, value);
			break;

		case AAC_PCM_MAX_OUTPUT_CHANNELS:
			if(value < -1 || value > (8)) { return AAC_DEC_SET_PARAM_FAIL; }
			dmxErr = pcmDmx_SetParam(hPcmDmx, MAX_NUMBER_OF_OUTPUT_CHANNELS, value);

			if(dmxErr != PCMDMX_OK) { goto bail; }
			errorStatus = aacDecoder_drcSetParam(hDrcInfo, MAX_OUTPUT_CHANNELS, value);
			if(value > 0) {
				uniDrcErr =
					FDK_drcDec_SetParam(self->hUniDrcDecoder, DRC_DEC_TARGET_CHANNEL_COUNT_REQUESTED, (int32_t)value);
			}
			break;

		case AAC_PCM_DUAL_CHANNEL_OUTPUT_MODE:
			dmxErr = pcmDmx_SetParam(hPcmDmx, DMX_DUAL_CHANNEL_MODE, value);
			break;

		case AAC_PCM_LIMITER_ENABLE:
			if(value < -2 || value > 1) { return AAC_DEC_SET_PARAM_FAIL; }
			self->limiterEnableUser = value;
			break;

		case AAC_PCM_LIMITER_ATTACK_TIME:
			if(value <= 0) { /* module function converts value to unsigned */
				return AAC_DEC_SET_PARAM_FAIL;
			}
			switch(pcmLimiter_SetAttack(hPcmTdl, value)) {
				case TDLIMIT_OK:
					break;
				case TDLIMIT_INVALID_HANDLE:
					return AAC_DEC_INVALID_HANDLE;
				case TDLIMIT_INVALID_PARAMETER:
				default:
					return AAC_DEC_SET_PARAM_FAIL;
			}
			break;

		case AAC_PCM_LIMITER_RELEAS_TIME:
			if(value <= 0) { /* module function converts value to unsigned */
				return AAC_DEC_SET_PARAM_FAIL;
			}
			switch(pcmLimiter_SetRelease(hPcmTdl, value)) {
				case TDLIMIT_OK:
					break;
				case TDLIMIT_INVALID_HANDLE:
					return AAC_DEC_INVALID_HANDLE;
				case TDLIMIT_INVALID_PARAMETER:
				default:
					return AAC_DEC_SET_PARAM_FAIL;
			}
			break;

		case AAC_METADATA_PROFILE: {
			DMX_PROFILE_TYPE dmxProfile;
			int32_t              mdExpiry = -1; /* in ms (-1: don't change) */

			switch((AAC_MD_PROFILE_t)value) {
				case AAC_MD_PROFILE_MPEG_STANDARD:
					dmxProfile = DMX_PRFL_STANDARD;
					break;
				case AAC_MD_PROFILE_MPEG_LEGACY:
					dmxProfile = DMX_PRFL_MATRIX_MIX;
					break;
				case AAC_MD_PROFILE_MPEG_LEGACY_PRIO:
					dmxProfile = DMX_PRFL_FORCE_MATRIX_MIX;
					break;
				case AAC_MD_PROFILE_ARIB_JAPAN:
					dmxProfile = DMX_PRFL_ARIB_JAPAN;
					mdExpiry = 550; /* ms */
					break;
				default:
					return AAC_DEC_SET_PARAM_FAIL;
			}
			dmxErr = pcmDmx_SetParam(hPcmDmx, DMX_PROFILE_SETTING, (int32_t)dmxProfile);
			if(dmxErr != PCMDMX_OK) { goto bail; }
			if((self != NULL) && (mdExpiry >= 0)) {
				self->metadataExpiry = mdExpiry;
				/* Determine the corresponding number of frames and configure all
				 * related modules. */
				aacDecoder_setMetadataExpiry(self, mdExpiry);
			}
		} break;

		case AAC_METADATA_EXPIRY_TIME:
			if(value < 0) { return AAC_DEC_SET_PARAM_FAIL; }
			if(self != NULL) {
				self->metadataExpiry = value;
				/* Determine the corresponding number of frames and configure all
				 * related modules. */
				aacDecoder_setMetadataExpiry(self, value);
			}
			break;

		case AAC_PCM_OUTPUT_CHANNEL_MAPPING:
			if(value < 0 || value > 1) { return AAC_DEC_SET_PARAM_FAIL; }
			/* CAUTION: The given value must be inverted to match the logic! */
			FDK_chMapDescr_setPassThrough(&self->mapDescr, !value);
			break;

		case AAC_QMF_LOWPOWER:
			if(value < -1 || value > 1) { return AAC_DEC_SET_PARAM_FAIL; }

			/**
			 * Set QMF mode (might be overriden)
			 *  0:HQ (complex)
			 *  1:LP (partially complex)
			 */
			self->qmfModeUser = (QMF_MODE)value;
			break;

		case AAC_DRC_ATTENUATION_FACTOR:
			/* DRC compression factor (where 0 is no and 127 is max compression) */
			errorStatus = aacDecoder_drcSetParam(hDrcInfo, DRC_CUT_SCALE, value);
			uniDrcErr =
				FDK_drcDec_SetParam(self->hUniDrcDecoder, DRC_DEC_COMPRESS, value * (FL2FXCONST_DBL(0.5f / 127.0f)));
			break;

		case AAC_DRC_BOOST_FACTOR:
			/* DRC boost factor (where 0 is no and 127 is max boost) */
			errorStatus = aacDecoder_drcSetParam(hDrcInfo, DRC_BOOST_SCALE, value);
			uniDrcErr =
				FDK_drcDec_SetParam(self->hUniDrcDecoder, DRC_DEC_BOOST, value * (FL2FXCONST_DBL(0.5f / 127.0f)));
			break;

		case AAC_DRC_REFERENCE_LEVEL:
			if((value >= 0) && ((value < 40) || (value > 127))) /* allowed range: -10 to -31.75 dB */
				return AAC_DEC_SET_PARAM_FAIL;
			/* DRC target reference level quantized in 0.25dB steps using values
			   [40..127]. Negative values switch off loudness normalisation. Negative
			   values also switch off MPEG-4 DRC, while MPEG-D DRC can be separately
			   switched on/off with AAC_UNIDRC_SET_EFFECT */
			errorStatus = aacDecoder_drcSetParam(hDrcInfo, TARGET_REF_LEVEL, value);
			uniDrcErr =
				FDK_drcDec_SetParam(self->hUniDrcDecoder, DRC_DEC_LOUDNESS_NORMALIZATION_ON, (int32_t)(value >= 0));
			/* set target loudness also for MPEG-D DRC */
			self->defaultTargetLoudness = (int8_t)value;
			break;

		case AAC_DRC_HEAVY_COMPRESSION:
			/* Don't need to overwrite cut/boost values */
			errorStatus = aacDecoder_drcSetParam(hDrcInfo, APPLY_HEAVY_COMPRESSION, value);
			break;

		case AAC_DRC_DEFAULT_PRESENTATION_MODE:
			/* DRC default presentation mode */
			errorStatus = aacDecoder_drcSetParam(hDrcInfo, DEFAULT_PRESENTATION_MODE, value);
			break;

		case AAC_DRC_ENC_TARGET_LEVEL:
			/* Encoder target level for light (i.e. not heavy) compression:
			   Target reference level assumed at encoder for deriving limiting gains
			 */
			errorStatus = aacDecoder_drcSetParam(hDrcInfo, ENCODER_TARGET_LEVEL, value);
			break;

		case AAC_UNIDRC_SET_EFFECT:
			if((value < -1) || (value > 6)) return AAC_DEC_SET_PARAM_FAIL;
			uniDrcErr = FDK_drcDec_SetParam(self->hUniDrcDecoder, DRC_DEC_EFFECT_TYPE, (int32_t)value);
			break;
		case AAC_UNIDRC_ALBUM_MODE:
			uniDrcErr = FDK_drcDec_SetParam(self->hUniDrcDecoder, DRC_DEC_ALBUM_MODE, (int32_t)value);
			break;

		case AAC_TPDEC_CLEAR_BUFFER:
			errTp = transportDec_SetParam(hTpDec, TPDEC_PARAM_RESET, 1);
			self->streamInfo.numLostAccessUnits = 0;
			self->streamInfo.numBadBytes = 0;
			self->streamInfo.numTotalBytes = 0;
			/* aacDecoder_SignalInterruption(self); */
			break;
		case AAC_CONCEAL_METHOD:
			/* Changing the concealment method can introduce additional bitstream
			   delay. And that in turn affects sub libraries and modules which makes
			   the whole thing quite complex.  So the complete changing routine is
			   packed into a helper function which keeps all modules and libs in a
			   consistent state even in the case an error occures. */
			errorStatus = setConcealMethod(self, value);
			if(errorStatus == AAC_DEC_OK) { self->concealMethodUser = (CConcealmentMethod_t)value; }
			break;

		default:
			return AAC_DEC_SET_PARAM_FAIL;
	} /* switch(param) */

bail:

	if(errorStatus == AAC_DEC_OK) {
		/* Check error code returned by DMX module library: */
		switch(dmxErr) {
			case PCMDMX_OK:
				break;
			case PCMDMX_INVALID_HANDLE:
				errorStatus = AAC_DEC_INVALID_HANDLE;
				break;
			default:
				errorStatus = AAC_DEC_SET_PARAM_FAIL;
		}
	}

	if(errTp != TRANSPORTDEC_OK && errorStatus == AAC_DEC_OK) { errorStatus = AAC_DEC_SET_PARAM_FAIL; }

	if(errorStatus == AAC_DEC_OK) {
		/* Check error code returned by MPEG-D DRC decoder library: */
		switch(uniDrcErr) {
			case 0:
				break;
			case -9998:
				errorStatus = AAC_DEC_INVALID_HANDLE;
				break;
			default:
				errorStatus = AAC_DEC_SET_PARAM_FAIL;
				break;
		}
	}

	return (errorStatus);
}

HANDLE_AACDECODER aacDecoder_Open(TRANSPORT_TYPE_t transportFmt, uint32_t nrOfLayers) {
	AAC_DECODER_INSTANCE *aacDec = NULL;
	HANDLE_TRANSPORTDEC   pIn;
	int32_t                   err = 0;
	int32_t                   stereoConfigIndex = -1;

	uint32_t nrOfLayers_min = fMin(nrOfLayers, (uint32_t)1);

	/* Allocate transport layer struct. */
	pIn = transportDec_Open(transportFmt, TP_FLAG_MPEG4, nrOfLayers_min);
	if(pIn == NULL) { return NULL; }

	transportDec_SetParam(pIn, TPDEC_PARAM_IGNORE_BUFFERFULLNESS, 1);

	/* Allocate AAC decoder core struct. */
	aacDec = CAacDecoder_Open(transportFmt);

	if(aacDec == NULL) {
		transportDec_Close(&pIn);
		goto bail;
	}
	aacDec->hInput = pIn;

	aacDec->nrOfLayers = nrOfLayers_min;

	/* Setup channel mapping descriptor. */
	FDK_chMapDescr_init(&aacDec->mapDescr, NULL, 0, 0);

	/* Register Config Update callback. */
	transportDec_RegisterAscCallback(pIn, aacDecoder_ConfigCallback, (void *)aacDec);

	/* Register Free Memory callback. */
	transportDec_RegisterFreeMemCallback(pIn, aacDecoder_FreeMemCallback, (void *)aacDec);

	/* Register config switch control callback. */
	transportDec_RegisterCtrlCFGChangeCallback(pIn, aacDecoder_CtrlCFGChangeCallback, (void *)aacDec);

	memset(&aacDec->qmfDomain, 0, sizeof(FDK_QMF_DOMAIN));
	/* open SBR decoder */
	if(SBRDEC_OK != sbrDecoder_Open(&aacDec->hSbrDecoder, &aacDec->qmfDomain)) {
		err = -1;
		goto bail;
	}
	aacDec->qmfModeUser = NOT_DEFINED;
	transportDec_RegisterSbrCallback(aacDec->hInput, aacDecoder_SbrCallback, (void *)aacDec->hSbrDecoder);

	if(mpegSurroundDecoder_Open((CMpegSurroundDecoder **)&aacDec->pMpegSurroundDecoder, stereoConfigIndex,
								&aacDec->qmfDomain)) {
		err = -1;
		goto bail;
	}
	/* Set MPEG Surround defaults */
	aacDec->mpsEnableUser = 0;
	aacDec->mpsEnableCurr = 0;
	aacDec->mpsApplicable = 0;
	aacDec->mpsOutputMode = (int8_t)SACDEC_OUT_MODE_NORMAL;
	transportDec_RegisterSscCallback(pIn, aacDecoder_SscCallback, (void *)aacDec);

	{
		if(FDK_drcDec_Open(&(aacDec->hUniDrcDecoder), DRC_DEC_ALL) != 0) {
			err = -1;
			goto bail;
		}
	}

	transportDec_RegisterUniDrcConfigCallback(pIn, aacDecoder_UniDrcCallback, (void *)aacDec,
											  aacDec->loudnessInfoSetPosition);
	aacDec->defaultTargetLoudness = (int8_t)96;

	pcmDmx_Open(&aacDec->hPcmUtils);
	if(aacDec->hPcmUtils == NULL) {
		err = -1;
		goto bail;
	}

	aacDec->hLimiter =
		pcmLimiter_Create(TDL_ATTACK_DEFAULT_MS, TDL_RELEASE_DEFAULT_MS, (int32_t)MAXVAL_DBL, (8), 96000);
	if(NULL == aacDec->hLimiter) {
		err = -1;
		goto bail;
	}
	aacDec->limiterEnableUser = (uint8_t)-1;
	aacDec->limiterEnableCurr = 0;

	/* Assure that all modules have same delay */
	if(setConcealMethod(aacDec, CConcealment_GetMethod(&aacDec->concealCommonData))) {
		err = -1;
		goto bail;
	}

bail:
	if(err == -1) {
		aacDecoder_Close(aacDec);
		aacDec = NULL;
	}
	return aacDec;
}

AAC_DECODER_ERROR_t aacDecoder_Fill(HANDLE_AACDECODER self, uint8_t *pBuffer, const uint32_t bufferSize, uint32_t *pBytesValid) {
	TRANSPORTDEC_ERROR_t tpErr;
	/* loop counter for layers; if not TT_MP4_RAWPACKETS used as index for only
	   available layer */
	int32_t layer = 0;
	int32_t nrOfLayers = self->nrOfLayers;

	{
		for(layer = 0; layer < nrOfLayers; layer++) {
			{
				tpErr = transportDec_FillData(self->hInput, pBuffer, bufferSize, &pBytesValid[layer], layer);
				if(tpErr != TRANSPORTDEC_OK) { return AAC_DEC_UNKNOWN; /* Must be an internal error */ }
			}
		}
	}

	return AAC_DEC_OK;
}

static void aacDecoder_SignalInterruption(HANDLE_AACDECODER self) {
	CAacDecoder_SignalInterruption(self);

	if(self->hSbrDecoder != NULL) { sbrDecoder_SetParam(self->hSbrDecoder, SBR_BS_INTERRUPTION, 1); }
	if(self->mpsEnableUser) {
		mpegSurroundDecoder_SetParam((CMpegSurroundDecoder *)self->pMpegSurroundDecoder, SACDEC_BS_INTERRUPTION, 1);
	}
}

static void aacDecoder_UpdateBitStreamCounters(CStreamInfo *pSi, HANDLE_FDK_BITSTREAM hBs, int32_t nBits,
											   AAC_DECODER_ERROR_t ErrorStatus) {
	/* calculate bit difference (amount of bits moved forward) */
	nBits = nBits - (int32_t)FDKgetValidBits(hBs);

	/* Note: The amount of bits consumed might become negative when parsing a
	   bit stream with several sub frames, and we find out at the last sub frame
	   that the total frame length does not match the sum of sub frame length.
	   If this happens, the transport decoder might want to rewind to the supposed
	   ending of the transport frame, and this position might be before the last
	   access unit beginning. */

	/* Calc bitrate. */
	if(pSi->frameSize > 0) {
		/* bitRate = nBits * sampleRate / frameSize */
		int32_t      ratio_e = 0;
		int32_t ratio_m = fDivNorm(pSi->sampleRate, pSi->frameSize, &ratio_e);
		pSi->bitRate = (int32_t)fMultNorm(nBits, DFRACT_BITS - 1, ratio_m, ratio_e, DFRACT_BITS - 1);
	}

	/* bit/byte counters */
	{
		int32_t nBytes;

		nBytes = nBits >> 3;
		pSi->numTotalBytes += nBytes;
		if(IS_OUTPUT_VALID(ErrorStatus)) { pSi->numTotalAccessUnits++; }
		if(IS_DECODE_ERROR(ErrorStatus)) {
			pSi->numBadBytes += nBytes;
			pSi->numBadAccessUnits++;
		}
	}
}

static int32_t aacDecoder_EstimateNumberOfLostFrames(HANDLE_AACDECODER self) {
	int32_t n;

	transportDec_GetMissingAccessUnitCount(&n, self->hInput);

	return n;
}

AAC_DECODER_ERROR_t aacDecoder_DecodeFrame(HANDLE_AACDECODER self, int16_t *pTimeData, const int32_t timeDataSize,
										 const uint32_t flags) {
	AAC_DECODER_ERROR_t    ErrorStatus;
	int32_t                  layer;
	int32_t                  nBits;
	int32_t                  timeData2Size;
	int32_t                  timeData3Size;
	int32_t                  timeDataHeadroom;
	HANDLE_FDK_BITSTREAM hBs;
	int32_t                  fTpInterruption = 0; /* Transport originated interruption detection. */
	int32_t                  fTpConceal = 0;      /* Transport originated concealment. */
	uint32_t                 accessUnit = 0;
	uint32_t                 numAccessUnits = 1;
	uint32_t                 numPrerollAU = 0;
	int32_t                  fEndAuNotAdjusted = 0; /* The end of the access unit was not adjusted */
	int32_t                  applyCrossfade = 1;    /* flag indicates if flushing was possible */
	int32_t             *pTimeData2;
	int32_t             *pTimeData3;

	if(self == NULL) { return AAC_DEC_INVALID_HANDLE; }

	if(flags & AACDEC_INTR) { self->streamInfo.numLostAccessUnits = 0; }

	hBs = transportDec_GetBitstream(self->hInput, 0);

	/* Get current bits position for bitrate calculation. */
	nBits = FDKgetValidBits(hBs);

	if(flags & AACDEC_CLRHIST) {
		if(self->flags[0] & AC_USAC) {
			/* 1) store AudioSpecificConfig always in AudioSpecificConfig_Parse() */
			/* 2) free memory of dynamic allocated data */
			CSAudioSpecificConfig_t asc;
			transportDec_GetAsc(self->hInput, 0, &asc);
			aacDecoder_FreeMemCallback(self, &asc);
			self->streamInfo.numChannels = 0;
			/* 3) restore AudioSpecificConfig */
			transportDec_OutOfBandConfig(self->hInput, asc.config, (asc.configBits + 7) >> 3, 0);
		}
	}

	if(!((flags & (AACDEC_CONCEAL | AACDEC_FLUSH)) || (self->flushStatus == AACDEC_RSV60_DASH_IPF_ATSC_FLUSH_ON) ||
		 (self->flushStatus == AACDEC_USAC_DASH_IPF_FLUSH_ON) ||
		 (self->buildUpStatus == AACDEC_RSV60_BUILD_UP_IDLE_IN_BAND))) {
		TRANSPORTDEC_ERROR_t err;

		for(layer = 0; layer < self->nrOfLayers; layer++) {
			err = transportDec_ReadAccessUnit(self->hInput, layer);

			if(err != TRANSPORTDEC_OK) {
				switch(err) {
					case TRANSPORTDEC_NOT_ENOUGH_BITS:
						ErrorStatus = AAC_DEC_NOT_ENOUGH_BITS;
						goto bail;
					case TRANSPORTDEC_SYNC_ERROR:
						self->streamInfo.numLostAccessUnits = aacDecoder_EstimateNumberOfLostFrames(self);
						fTpInterruption = 1;
						break;
					case TRANSPORTDEC_NEED_TO_RESTART:
						ErrorStatus = AAC_DEC_NEED_TO_RESTART;
						goto bail;
					case TRANSPORTDEC_CRC_ERROR:
						fTpConceal = 1;
						break;
					case TRANSPORTDEC_UNSUPPORTED_FORMAT:
						ErrorStatus = AAC_DEC_UNSUPPORTED_FORMAT;
						goto bail;
					default:
						ErrorStatus = AAC_DEC_UNKNOWN;
						goto bail;
				}
			}
		}
	}
	else {
		if(self->streamInfo.numLostAccessUnits > 0) { self->streamInfo.numLostAccessUnits--; }
	}

	self->frameOK = 1;

	uint32_t prerollAUOffset[AACDEC_MAX_NUM_PREROLL_AU_USAC];
	uint32_t prerollAULength[AACDEC_MAX_NUM_PREROLL_AU_USAC];
	for(int32_t i = 0; i < AACDEC_MAX_NUM_PREROLL_AU_USAC + 1; i++) self->prerollAULength[i] = 0;

	int32_t                  auStartAnchor;
	HANDLE_FDK_BITSTREAM hBsAu;

	/* Process preroll frames and current frame */
	do {
		if(!(flags & (AACDEC_CONCEAL | AACDEC_FLUSH)) && (self->flushStatus != AACDEC_RSV60_CFG_CHANGE_ATSC_FLUSH_ON) &&
		   (accessUnit == 0) &&
		   (self->hasAudioPreRoll || (self->buildUpStatus == AACDEC_RSV60_BUILD_UP_IDLE_IN_BAND)) && !fTpInterruption &&
		   !fTpConceal /* Bit stream pointer needs to be at the beginning of a
						  (valid) AU. */
		) {
			ErrorStatus =
				CAacDecoder_PreRollExtensionPayloadParse(self, &numPrerollAU, prerollAUOffset, prerollAULength);

			if(ErrorStatus != AAC_DEC_OK) {
				switch(ErrorStatus) {
					case AAC_DEC_NOT_ENOUGH_BITS:
						goto bail;
					case AAC_DEC_PARSE_ERROR:
						self->frameOK = 0;
						break;
					default:
						break;
				}
			}

			numAccessUnits += numPrerollAU;
		}

		hBsAu = transportDec_GetBitstream(self->hInput, 0);
		auStartAnchor = (int32_t)FDKgetValidBits(hBsAu);

		self->accessUnit = accessUnit;
		if(accessUnit < numPrerollAU) { FDKpushFor(hBsAu, prerollAUOffset[accessUnit]); }

		/* Signal bit stream interruption to other modules if required. */
		if(fTpInterruption || ((flags & AACDEC_INTR) && (accessUnit == 0))) {
			aacDecoder_SignalInterruption(self);
			if(!((flags & AACDEC_INTR) && (accessUnit == 0))) {
				ErrorStatus = AAC_DEC_TRANSPORT_SYNC_ERROR;
				goto bail;
			}
		}

		/* Clearing core data will be done in CAacDecoder_DecodeFrame() below.
		   Tell other modules to clear states if required. */
		if(flags & AACDEC_CLRHIST) {
			if(!(self->flags[0] & AC_USAC)) {
				sbrDecoder_SetParam(self->hSbrDecoder, SBR_CLEAR_HISTORY, 1);
				mpegSurroundDecoder_SetParam((CMpegSurroundDecoder *)self->pMpegSurroundDecoder, SACDEC_CLEAR_HISTORY,
											 1);
				if(FDK_QmfDomain_ClearPersistentMemory(&self->qmfDomain) != 0) {
					ErrorStatus = AAC_DEC_UNKNOWN;
					goto bail;
				}
			}
		}

		/* Empty bit buffer in case of flush request. */
		if(flags & AACDEC_FLUSH && !(flags & AACDEC_CONCEAL)) {
			if(!self->flushStatus) {
				transportDec_SetParam(self->hInput, TPDEC_PARAM_RESET, 1);
				self->streamInfo.numLostAccessUnits = 0;
				self->streamInfo.numBadBytes = 0;
				self->streamInfo.numTotalBytes = 0;
			}
		}
		/* Reset the output delay field. The modules will add their figures one
		 * after another. */
		self->streamInfo.outputDelay = 0;

		if(self->limiterEnableUser == (uint8_t)-2) {
			/* Enable limiter only for RSVD60. */
			self->limiterEnableCurr = (self->flags[0] & AC_RSV603DA) ? 1 : 0;
		}
		else if(self->limiterEnableUser == (uint8_t)-1) {
			/* Enable limiter for all non-lowdelay AOT's. */
			self->limiterEnableCurr = (self->flags[0] & (AC_LD | AC_ELD)) ? 0 : 1;
		}
		else {
			/* Use limiter configuration as requested. */
			self->limiterEnableCurr = self->limiterEnableUser;
		}

		/* reset DRC level normalization gain on a per frame basis */
		self->extGain[0] = AACDEC_DRC_GAIN_INIT_VALUE;

		pTimeData2 = self->pTimeData2;
		timeData2Size = self->timeData2Size / sizeof(int32_t);
		pTimeData3 = (int32_t *)self->pTimeData2;
		timeData3Size = self->timeData2Size / sizeof(int32_t);

		ErrorStatus = CAacDecoder_DecodeFrame(self,
											  flags | (fTpConceal ? AACDEC_CONCEAL : 0) |
												  ((self->flushStatus && !(flags & AACDEC_CONCEAL)) ? AACDEC_FLUSH : 0),
											  pTimeData2 + 0, timeData2Size, self->streamInfo.aacSamplesPerFrame + 0);

		timeDataHeadroom = self->aacOutDataHeadroom;

		/* if flushing for USAC DASH IPF was not possible go on with decoding
		 * preroll */
		if((self->flags[0] & AC_USAC) && (self->flushStatus == AACDEC_USAC_DASH_IPF_FLUSH_ON) &&
		   !(flags & AACDEC_CONCEAL) && (ErrorStatus != AAC_DEC_OK)) {
			applyCrossfade = 0;
		}
		else /* USAC DASH IPF flushing possible begin */
		{
			if(!((flags & (AACDEC_CONCEAL | AACDEC_FLUSH)) || fTpConceal || self->flushStatus) &&
			   (!(IS_OUTPUT_VALID(ErrorStatus)) || !(accessUnit < numPrerollAU))) {
				TRANSPORTDEC_ERROR_t tpErr;
				tpErr = transportDec_EndAccessUnit(self->hInput);
				if(tpErr != TRANSPORTDEC_OK) { self->frameOK = 0; }
			}
			else { /* while preroll processing later possibly an error in the
					  renderer part occurrs */
				if(IS_OUTPUT_VALID(ErrorStatus)) { fEndAuNotAdjusted = 1; }
			}

			/* If the current pTimeData2 does not contain a valid signal, there
			 * nothing else we can do, so bail. */
			if(!IS_OUTPUT_VALID(ErrorStatus)) { goto bail; }

			{
				self->streamInfo.sampleRate = self->streamInfo.aacSampleRate;
				self->streamInfo.frameSize = self->streamInfo.aacSamplesPerFrame;
			}

			self->streamInfo.numChannels = self->streamInfo.aacNumChannels;

			{
				FDK_Delay_Apply(&self->usacResidualDelay,
								pTimeData2 + 1 * (self->streamInfo.aacSamplesPerFrame + 0) + 0,
								self->streamInfo.frameSize, 0);
			}

			/* Setting of internal MPS state; may be reset in CAacDecoder_SyncQmfMode
			   if decoder is unable to decode with user defined qmfMode */
			if(!(self->flags[0] & (AC_USAC | AC_RSVD50 | AC_RSV603DA | AC_ELD))) {
				self->mpsEnableCurr =
					(self->mpsEnableUser && isSupportedMpsConfig(self->streamInfo.aot, self->streamInfo.numChannels,
																 (self->flags[0] & AC_MPS_PRESENT) ? 1 : 0));
			}

			if(!self->qmfDomain.globalConf.qmfDomainExplicitConfig && self->mpsEnableCurr) {
				/* if not done yet, allocate full MPEG Surround decoder instance */
				if(mpegSurroundDecoder_IsFullMpegSurroundDecoderInstanceAvailable(
					   (CMpegSurroundDecoder *)self->pMpegSurroundDecoder) == SAC_INSTANCE_NOT_FULL_AVAILABLE) {
					if(mpegSurroundDecoder_Open((CMpegSurroundDecoder **)&self->pMpegSurroundDecoder, -1,
												&self->qmfDomain)) {
						return AAC_DEC_OUT_OF_MEMORY;
					}
				}
			}

			CAacDecoder_SyncQmfMode(self);

			if(!self->qmfDomain.globalConf.qmfDomainExplicitConfig && self->mpsEnableCurr) {
				SAC_INPUT_CONFIG sac_interface =
					(self->sbrEnabled && self->hSbrDecoder) ? SAC_INTERFACE_QMF : SAC_INTERFACE_TIME;
				/* needs to be done before first SBR apply. */
				mpegSurroundDecoder_ConfigureQmfDomain((CMpegSurroundDecoder *)self->pMpegSurroundDecoder,
													   sac_interface, (uint32_t)self->streamInfo.aacSampleRate,
													   self->streamInfo.aot);
				if(self->qmfDomain.globalConf.nBandsAnalysis_requested > 0) {
					self->qmfDomain.globalConf.nQmfTimeSlots_requested =
						self->streamInfo.aacSamplesPerFrame / self->qmfDomain.globalConf.nBandsAnalysis_requested;
				}
				else { self->qmfDomain.globalConf.nQmfTimeSlots_requested = 0; }
			}

			switch(FDK_QmfDomain_Configure(&self->qmfDomain)) {
				default:
				case QMF_DOMAIN_INIT_ERROR:
					ErrorStatus = AAC_DEC_UNKNOWN;
					goto bail;
				case QMF_DOMAIN_OUT_OF_MEMORY:
					ErrorStatus = AAC_DEC_OUT_OF_MEMORY;
					goto bail;
				case QMF_DOMAIN_OK:
					break;
			}

			/* sbr decoder */

			if((ErrorStatus != AAC_DEC_OK) || (flags & AACDEC_CONCEAL) ||
			   self->pAacDecoderStaticChannelInfo[0]->concealmentInfo.concealState > ConcealState_FadeIn) {
				self->frameOK = 0; /* if an error has occured do concealment in the SBR
									  decoder too */
			}

			if(self->sbrEnabled && (!(self->flags[0] & AC_USAC_SCFGI3))) {
				SBR_ERROR sbrError = SBRDEC_OK;
				int32_t       chIdx, numCoreChannel = self->streamInfo.numChannels;

				/* set params */
				sbrDecoder_SetParam(self->hSbrDecoder, SBR_SYSTEM_BITSTREAM_DELAY, self->sbrParams.bsDelay);
				sbrDecoder_SetParam(
					self->hSbrDecoder, SBR_FLUSH_DATA,
					(flags & AACDEC_FLUSH) | ((self->flushStatus && !(flags & AACDEC_CONCEAL)) ? AACDEC_FLUSH : 0));

				if(self->streamInfo.aot == AOT_ER_AAC_ELD) {
					/* Configure QMF */
					sbrDecoder_SetParam(self->hSbrDecoder, SBR_LD_QMF_TIME_ALIGN,
										(self->flags[0] & AC_MPS_PRESENT) ? 1 : 0);
				}

				{
					PCMDMX_ERROR dmxErr;
					int32_t          maxOutCh = 0;

					dmxErr = pcmDmx_GetParam(self->hPcmUtils, MAX_NUMBER_OF_OUTPUT_CHANNELS, &maxOutCh);
					if((dmxErr == PCMDMX_OK) && (maxOutCh == 1)) {
						/* Disable PS processing if we have to create a mono output signal.
						 */
						self->psPossible = 0;
					}
				}

				sbrDecoder_SetParam(self->hSbrDecoder, SBR_SKIP_QMF, (self->mpsEnableCurr) ? 2 : 0);

				int32_t *input;
				input = (int32_t *)self->workBufferCore2;
				memcpy(input, pTimeData3,
						  sizeof(int32_t) * (self->streamInfo.numChannels) * (self->streamInfo.frameSize));

				/* apply SBR processing */
				sbrError =
					sbrDecoder_Apply(self->hSbrDecoder, input, pTimeData3, timeData3Size, &self->streamInfo.numChannels,
									 &self->streamInfo.sampleRate, &self->mapDescr, self->chMapIndex, self->frameOK,
									 &self->psPossible, self->aacOutDataHeadroom, &timeDataHeadroom);

				if(sbrError == SBRDEC_OK) {
					/* Update data in streaminfo structure. Assume that the SBR upsampling
					   factor is either 1, 2, 8/3 or 4. Maximum upsampling factor is 4
					   (CELP+SBR or USAC 4:1 SBR) */
					self->flags[0] |= AC_SBR_PRESENT;
					if(self->streamInfo.aacSampleRate != self->streamInfo.sampleRate) {
						if(self->streamInfo.aacSampleRate >> 2 == self->streamInfo.sampleRate) {
							self->streamInfo.frameSize = self->streamInfo.aacSamplesPerFrame >> 2;
							self->streamInfo.outputDelay = self->streamInfo.outputDelay >> 2;
						}
						else if(self->streamInfo.aacSampleRate >> 1 == self->streamInfo.sampleRate) {
							self->streamInfo.frameSize = self->streamInfo.aacSamplesPerFrame >> 1;
							self->streamInfo.outputDelay = self->streamInfo.outputDelay >> 1;
						}
						else if(self->streamInfo.aacSampleRate << 1 == self->streamInfo.sampleRate) {
							self->streamInfo.frameSize = self->streamInfo.aacSamplesPerFrame << 1;
							self->streamInfo.outputDelay = self->streamInfo.outputDelay << 1;
						}
						else if(self->streamInfo.aacSampleRate << 2 == self->streamInfo.sampleRate) {
							self->streamInfo.frameSize = self->streamInfo.aacSamplesPerFrame << 2;
							self->streamInfo.outputDelay = self->streamInfo.outputDelay << 2;
						}
						else if(self->streamInfo.frameSize == 768) {
							self->streamInfo.frameSize = (self->streamInfo.aacSamplesPerFrame << 3) / 3;
							self->streamInfo.outputDelay = (self->streamInfo.outputDelay << 3) / 3;
						}
						else {
							ErrorStatus = AAC_DEC_SET_PARAM_FAIL;
							goto bail;
						}
					}
					else { self->streamInfo.frameSize = self->streamInfo.aacSamplesPerFrame; }
					self->streamInfo.outputDelay += sbrDecoder_GetDelay(self->hSbrDecoder);

					if(self->psPossible) { self->flags[0] |= AC_PS_PRESENT; }
					for(chIdx = numCoreChannel; chIdx < self->streamInfo.numChannels; chIdx += 1) {
						self->channelType[chIdx] = ACT_FRONT;
						self->channelIndices[chIdx] = chIdx;
					}
				}
				if(sbrError == SBRDEC_OUTPUT_BUFFER_TOO_SMALL) {
					ErrorStatus = AAC_DEC_OUTPUT_BUFFER_TOO_SMALL;
					goto bail;
				}
			}

			if(self->mpsEnableCurr) {
				int32_t err, sac_interface, nChannels, frameSize;

				nChannels = self->streamInfo.numChannels;
				frameSize = self->streamInfo.frameSize;
				sac_interface = SAC_INTERFACE_TIME;

				if(self->sbrEnabled && self->hSbrDecoder) sac_interface = SAC_INTERFACE_QMF;
				if(self->streamInfo.aot == AOT_USAC) {
					if(self->flags[0] & AC_USAC_SCFGI3) { sac_interface = SAC_INTERFACE_TIME; }
				}
				err = mpegSurroundDecoder_SetParam((CMpegSurroundDecoder *)self->pMpegSurroundDecoder, SACDEC_INTERFACE,
												   sac_interface);

				if(err == 0) {
					err = mpegSurroundDecoder_Apply(
						(CMpegSurroundDecoder *)self->pMpegSurroundDecoder, (int32_t *)self->workBufferCore2,
						pTimeData3, timeData3Size, self->streamInfo.aacSamplesPerFrame, &nChannels, &frameSize,
						self->streamInfo.sampleRate, self->streamInfo.aot, self->channelType, self->channelIndices,
						&self->mapDescr, self->aacOutDataHeadroom, &timeDataHeadroom);
				}

				if(err == MPS_OUTPUT_BUFFER_TOO_SMALL) {
					ErrorStatus = AAC_DEC_OUTPUT_BUFFER_TOO_SMALL;
					goto bail;
				}
				if(err == 0) {
					/* Update output parameter */
					self->streamInfo.numChannels = nChannels;
					self->streamInfo.frameSize = frameSize;
					self->streamInfo.outputDelay +=
						mpegSurroundDecoder_GetDelay((CMpegSurroundDecoder *)self->pMpegSurroundDecoder);
					/* Save current parameter for possible concealment of next frame */
					self->mpsOutChannelsLast = nChannels;
					self->mpsFrameSizeLast = frameSize;
				}
				else if((self->mpsOutChannelsLast > 0) && (self->mpsFrameSizeLast > 0)) {
					/* Restore parameters of last frame ... */
					self->streamInfo.numChannels = self->mpsOutChannelsLast;
					self->streamInfo.frameSize = self->mpsFrameSizeLast;
					/* ... and clear output buffer so that potentially corrupted data does
					 * not reach the framework. */
					memset(pTimeData3, 0, self->mpsOutChannelsLast * self->mpsFrameSizeLast * sizeof(int32_t));
					/* Additionally proclaim that this frame had errors during decoding.
					 */
					ErrorStatus = AAC_DEC_DECODE_FRAME_ERROR;
				}
				else { ErrorStatus = AAC_DEC_UNKNOWN; /* no output */ }
			}

			/* SBR decoder for Unified Stereo Config (stereoConfigIndex == 3) */

			if(self->sbrEnabled && (self->flags[0] & AC_USAC_SCFGI3)) {
				SBR_ERROR sbrError = SBRDEC_OK;

				/* set params */
				sbrDecoder_SetParam(self->hSbrDecoder, SBR_SYSTEM_BITSTREAM_DELAY, self->sbrParams.bsDelay);

				sbrDecoder_SetParam(self->hSbrDecoder, SBR_SKIP_QMF, 1);

				/* apply SBR processing */
				sbrError = sbrDecoder_Apply(self->hSbrDecoder, pTimeData3, pTimeData3, timeData3Size,
											&self->streamInfo.numChannels, &self->streamInfo.sampleRate,
											&self->mapDescr, self->chMapIndex, self->frameOK, &self->psPossible,
											self->aacOutDataHeadroom, &timeDataHeadroom);

				if(sbrError == SBRDEC_OK) {
					/* Update data in streaminfo structure. Assume that the SBR upsampling
					 * factor is either 1,2 or 4 */
					self->flags[0] |= AC_SBR_PRESENT;
					if(self->streamInfo.aacSampleRate != self->streamInfo.sampleRate) {
						if(self->streamInfo.frameSize == 768) {
							self->streamInfo.frameSize = (self->streamInfo.aacSamplesPerFrame * 8) / 3;
						}
						else if(self->streamInfo.aacSampleRate << 2 == self->streamInfo.sampleRate) {
							self->streamInfo.frameSize = self->streamInfo.aacSamplesPerFrame << 2;
						}
						else { self->streamInfo.frameSize = self->streamInfo.aacSamplesPerFrame << 1; }
					}

					self->flags[0] &= ~AC_PS_PRESENT;
				}
				if(sbrError == SBRDEC_OUTPUT_BUFFER_TOO_SMALL) {
					ErrorStatus = AAC_DEC_OUTPUT_BUFFER_TOO_SMALL;
					goto bail;
				}
			}

			{
				if((int32_t)PCM_OUT_HEADROOM != timeDataHeadroom) {
					for(int32_t i = ((self->streamInfo.frameSize * self->streamInfo.numChannels) - 1); i >= 0; i--) {
						pTimeData2[i] = (int32_t)pTimeData3[i] >> (PCM_OUT_HEADROOM - timeDataHeadroom);
					}
				}
			}

			{
				if((FDK_drcDec_GetParam(self->hUniDrcDecoder, DRC_DEC_IS_ACTIVE)) && !(self->flags[0] & AC_RSV603DA)) {
					/* Apply DRC gains*/
					int32_t       ch, drcDelay = 0;
					int32_t       needsDeinterleaving = 0;
					int32_t *drcWorkBuffer = NULL;
					int32_t  channelGain[(8)];
					int32_t       reverseInChannelMap[(8)];
					int32_t       reverseOutChannelMap[(8)];
					int32_t       numDrcOutChannels =
						FDK_drcDec_GetParam(self->hUniDrcDecoder, DRC_DEC_TARGET_CHANNEL_COUNT_SELECTED);
					memset(channelGain, 0, sizeof(channelGain));
					for(ch = 0; ch < (8); ch++) {
						reverseInChannelMap[ch] = ch;
						reverseOutChannelMap[ch] = ch;
					}

					/* Update sampleRate and frameSize. This may be necessary in case of
					 * implicit SBR signaling */
					FDK_drcDec_SetParam(self->hUniDrcDecoder, DRC_DEC_SAMPLE_RATE, self->streamInfo.sampleRate);
					FDK_drcDec_SetParam(self->hUniDrcDecoder, DRC_DEC_FRAME_SIZE, self->streamInfo.frameSize);

					/* If SBR and/or MPS is active, the DRC gains are aligned to the QMF
					   domain signal before the QMF synthesis. Therefore the DRC gains
					   need to be delayed by the QMF synthesis delay. */
					if(self->sbrEnabled) drcDelay = 257;
					if(self->mpsEnableCurr) drcDelay = 257;
					/* Take into account concealment delay */
					drcDelay += CConcealment_GetDelay(&self->concealCommonData) * self->streamInfo.frameSize;

					for(ch = 0; ch < self->streamInfo.numChannels; ch++) {
						uint8_t mapValue = FDK_chMapDescr_getMapValue(&self->mapDescr, (uint8_t)ch, self->chMapIndex);
						if(mapValue < (8)) reverseInChannelMap[mapValue] = ch;
					}
					for(ch = 0; ch < (int32_t)numDrcOutChannels; ch++) {
						uint8_t mapValue = FDK_chMapDescr_getMapValue(&self->mapDescr, (uint8_t)ch, numDrcOutChannels);
						if(mapValue < (8)) reverseOutChannelMap[mapValue] = ch;
					}

					/* The output of SBR and MPS is interleaved. Deinterleaving may be
					 * necessary for FDK_drcDec_ProcessTime, which accepts deinterleaved
					 * audio only. */
					if((self->streamInfo.numChannels > 1) && (0 || (self->sbrEnabled) || (self->mpsEnableCurr))) {
						/* interleaving/deinterleaving is performed on upper part of
						 * pTimeData2. Check if this buffer is large enough. */
						if(timeData2Size < (int32_t)(2 * self->streamInfo.numChannels * self->streamInfo.frameSize)) {
							ErrorStatus = AAC_DEC_UNKNOWN;
							goto bail;
						}
						needsDeinterleaving = 1;
						drcWorkBuffer =
							(int32_t *)pTimeData2 + self->streamInfo.numChannels * self->streamInfo.frameSize;
						FDK_deinterleave(pTimeData2, drcWorkBuffer, self->streamInfo.numChannels,
										 self->streamInfo.frameSize, self->streamInfo.frameSize);
					}
					else { drcWorkBuffer = pTimeData2; }

					/* prepare Loudness Normalisation gain */
					FDK_drcDec_SetParam(self->hUniDrcDecoder, DRC_DEC_TARGET_LOUDNESS,
										(int32_t)-self->defaultTargetLoudness * FL2FXCONST_DBL(1.0f / (float)(1 << 9)));
					FDK_drcDec_SetChannelGains(self->hUniDrcDecoder, self->streamInfo.numChannels,
											   self->streamInfo.frameSize, channelGain, drcWorkBuffer,
											   self->streamInfo.frameSize);
					FDK_drcDec_Preprocess(self->hUniDrcDecoder);

					/* apply DRC1 gain sequence */
					for(ch = 0; ch < self->streamInfo.numChannels; ch++) {
						FDK_drcDec_ProcessTime(self->hUniDrcDecoder, drcDelay, DRC_DEC_DRC1, ch,
											   reverseInChannelMap[ch] - ch, 1, drcWorkBuffer,
											   self->streamInfo.frameSize);
					}
					/* apply downmix */
					FDK_drcDec_ApplyDownmix(self->hUniDrcDecoder, reverseInChannelMap, reverseOutChannelMap,
											drcWorkBuffer,
											&self->streamInfo.numChannels); /* self->streamInfo.numChannels
																			   may change here */
					/* apply DRC2/3 gain sequence */
					for(ch = 0; ch < self->streamInfo.numChannels; ch++) {
						FDK_drcDec_ProcessTime(self->hUniDrcDecoder, drcDelay, DRC_DEC_DRC2_DRC3, ch,
											   reverseOutChannelMap[ch] - ch, 1, drcWorkBuffer,
											   self->streamInfo.frameSize);
					}

					if(needsDeinterleaving) {
						FDK_interleave(drcWorkBuffer, pTimeData2, self->streamInfo.numChannels,
									   self->streamInfo.frameSize, self->streamInfo.frameSize);
					}
				}
			}
			if(FDK_drcDec_GetParam(self->hUniDrcDecoder, DRC_DEC_IS_ACTIVE)) {
				/* return output loudness information for MPEG-D DRC */
				int32_t outputLoudness = FDK_drcDec_GetParam(self->hUniDrcDecoder, DRC_DEC_OUTPUT_LOUDNESS);
				if(outputLoudness == DRC_DEC_LOUDNESS_NOT_PRESENT) {
					/* no valid MPEG-D DRC loudness value contained */
					self->streamInfo.outputLoudness = -1;
				}
				else {
					if(outputLoudness > 0) {
						/* positive output loudness values (very unusual) are limited to 0
						 * dB */
						self->streamInfo.outputLoudness = 0;
					}
					else {
						self->streamInfo.outputLoudness =
							-(int32_t)outputLoudness >> 22; /* negate and scale from e = 7 to e = (31-2) */
					}
				}
			}
			else {
				/* return output loudness information for MPEG-4 DRC */
				if(self->streamInfo.drcProgRefLev < 0) { /* no MPEG-4 DRC loudness metadata contained */
					self->streamInfo.outputLoudness = -1;
				}
				else {
					if(self->defaultTargetLoudness < 0) { /* loudness normalization is off */
						self->streamInfo.outputLoudness = self->streamInfo.drcProgRefLev;
					}
					else { self->streamInfo.outputLoudness = self->defaultTargetLoudness; }
				}
			}

			if(self->streamInfo.extAot != AOT_AAC_SLS) {
				int32_t pcmLimiterScale = 0;
				int32_t interleaved = 0;
				interleaved |= (self->sbrEnabled) ? 1 : 0;
				interleaved |= (self->mpsEnableCurr) ? 1 : 0;
				PCMDMX_ERROR dmxErr = PCMDMX_OK;
				if((flags & AACDEC_INTR) && (accessUnit == 0)) {
					/* delete data from the past (e.g. mixdown coeficients) */
					pcmDmx_Reset(self->hPcmUtils, PCMDMX_RESET_BS_DATA);
				}
				if(flags & (AACDEC_CLRHIST)) {
					if(!(self->flags[0] & AC_USAC)) {
						/* delete data from the past (e.g. mixdown coeficients) */
						pcmDmx_Reset(self->hPcmUtils, PCMDMX_RESET_BS_DATA);
					}
				}

				/* do PCM post processing */
				dmxErr = pcmDmx_ApplyFrame(self->hPcmUtils, pTimeData2, timeData2Size, self->streamInfo.frameSize,
										   &self->streamInfo.numChannels, interleaved, self->channelType,
										   self->channelIndices, &self->mapDescr, &pcmLimiterScale);
				if(dmxErr == PCMDMX_OUTPUT_BUFFER_TOO_SMALL) {
					ErrorStatus = AAC_DEC_OUTPUT_BUFFER_TOO_SMALL;
					goto bail;
				}
				if((ErrorStatus == AAC_DEC_OK) && (dmxErr == PCMDMX_INVALID_MODE)) {
					/* Announce the framework that the current combination of channel
					 * configuration and downmix settings are not know to produce a
					 * predictable behavior and thus maybe produce strange output. */
					ErrorStatus = AAC_DEC_DECODE_FRAME_ERROR;
				}

				pcmLimiterScale += PCM_OUT_HEADROOM;

				if(flags & AACDEC_CLRHIST) {
					if(!(self->flags[0] & AC_USAC)) {
						/* Reset DRC data */
						aacDecoder_drcReset(self->hDrcInfo);
						/* Delete the delayed signal. */
						pcmLimiter_Reset(self->hLimiter);
					}
				}

				/* Set applyExtGain if DRC processing is enabled and if
				   progRefLevelPresent is present for the first time. Consequences: The
				   headroom of the output signal can be set to AACDEC_DRC_GAIN_SCALING
				   only for audio formats which support legacy DRC Level Normalization.
								 For all other audio formats the headroom of the output
				   signal is set to PCM_OUT_HEADROOM. */
				if(self->hDrcInfo->enable && (self->hDrcInfo->progRefLevelPresent == 1)) {
					self->hDrcInfo->applyExtGain |= 1;
				}

				/* Check whether time data buffer is large enough. */
				if(timeDataSize < (self->streamInfo.numChannels * self->streamInfo.frameSize)) {
					ErrorStatus = AAC_DEC_OUTPUT_BUFFER_TOO_SMALL;
					goto bail;
				}

				if(self->limiterEnableCurr) {
					/* use workBufferCore2 buffer for interleaving */
					PCM_LIM *pInterleaveBuffer;
					int32_t      blockLength = self->streamInfo.frameSize;

					/* Set actual signal parameters */
					pcmLimiter_SetNChannels(self->hLimiter, self->streamInfo.numChannels);
					pcmLimiter_SetSampleRate(self->hLimiter, self->streamInfo.sampleRate);

					if((self->streamInfo.numChannels == 1) || (self->sbrEnabled) || (self->mpsEnableCurr)) {
						pInterleaveBuffer = (PCM_LIM *)pTimeData2;
					}
					else {
						pInterleaveBuffer = (PCM_LIM *)self->workBufferCore2;

						/* applyLimiter requests for interleaved data */
						/* Interleave ouput buffer */
						FDK_interleave(pTimeData2, pInterleaveBuffer, self->streamInfo.numChannels, blockLength,
									   self->streamInfo.frameSize);
					}

					int32_t *pGainPerSample = NULL;

					if(self->hDrcInfo->enable && self->hDrcInfo->applyExtGain) {
						pGainPerSample = self->workBufferCore1;

						uint32_t GRMWBC1 = sizeof(CWorkBufferCore1_t) + 8 + sizeof(void *);  // GetRequiredMemWorkBufferCore1
						GRMWBC1 += ((8 - sizeof(CWorkBufferCore1_t) + 8 + sizeof(void *) & 7) & 7);
						(void) GRMWBC1;
						if((int32_t)GRMWBC1 < (int32_t)(self->streamInfo.frameSize * sizeof(int32_t))) {
							ErrorStatus = AAC_DEC_UNKNOWN;
							goto bail;
						}

						pcmLimiterScale =
							applyDrcLevelNormalization(self->hDrcInfo, (int32_t *)pInterleaveBuffer, self->extGain,
													   pGainPerSample, pcmLimiterScale, self->extGainDelay,
													   self->streamInfo.frameSize, self->streamInfo.numChannels, 1, 1);
					}

					pcmLimiter_Apply(self->hLimiter, pInterleaveBuffer, pTimeData, pGainPerSample, pcmLimiterScale,
									 self->streamInfo.frameSize);

					{
						/* Announce the additional limiter output delay */
						self->streamInfo.outputDelay += pcmLimiter_GetDelay(self->hLimiter);
					}
				}
				else {
					if(self->hDrcInfo->enable && self->hDrcInfo->applyExtGain) {
						pcmLimiterScale = applyDrcLevelNormalization(
							self->hDrcInfo, pTimeData2, self->extGain, NULL, pcmLimiterScale, self->extGainDelay,
							self->streamInfo.frameSize, self->streamInfo.numChannels,
							(interleaved || (self->streamInfo.numChannels == 1)) ? 1 : self->streamInfo.frameSize, 0);
					}

					/* If numChannels = 1 we do not need interleaving. The same applies if
					SBR or MPS are used, since their output is interleaved already
					(resampled or not) */
					if((self->streamInfo.numChannels == 1) || (self->sbrEnabled) || (self->mpsEnableCurr)) {
						scaleValuesSaturate(pTimeData, pTimeData2,
											self->streamInfo.frameSize * self->streamInfo.numChannels, pcmLimiterScale);
					}
					else {
						scaleValuesSaturate((int16_t *)self->workBufferCore2, pTimeData2,
											self->streamInfo.frameSize * self->streamInfo.numChannels, pcmLimiterScale);
						/* Interleave ouput buffer */
						FDK_interleave((int16_t *)self->workBufferCore2, pTimeData, self->streamInfo.numChannels,
									   self->streamInfo.frameSize, self->streamInfo.frameSize);
					}
				}
			} /* if (self->streamInfo.extAot != AOT_AAC_SLS)*/

			if(self->flags[0] & AC_USAC) {
				if(self->flushStatus == AACDEC_USAC_DASH_IPF_FLUSH_ON && !(flags & AACDEC_CONCEAL)) {
					CAacDecoder_PrepareCrossFade(pTimeData, self->pTimeDataFlush, self->streamInfo.numChannels,
												 self->streamInfo.frameSize, 1);
				}

				/* prepare crossfade buffer for fade in */
				if(!applyCrossfade && self->applyCrossfade && !(flags & AACDEC_CONCEAL)) {
					for(int32_t ch = 0; ch < self->streamInfo.numChannels; ch++) {
						for(int32_t i = 0; i < TIME_DATA_FLUSH_SIZE; i++) { self->pTimeDataFlush[ch][i] = 0; }
					}
					applyCrossfade = 1;
				}

				if(applyCrossfade && self->applyCrossfade && !(accessUnit < numPrerollAU) &&
				   (self->buildUpStatus == AACDEC_USAC_BUILD_UP_ON)) {
					CAacDecoder_ApplyCrossFade(pTimeData, self->pTimeDataFlush, self->streamInfo.numChannels,
											   self->streamInfo.frameSize, 1);
					self->applyCrossfade = 0;
				}
			}

			/* Signal interruption to take effect in next frame. */
			if((flags & AACDEC_FLUSH || self->flushStatus) && !(flags & AACDEC_CONCEAL)) {
				aacDecoder_SignalInterruption(self);
			}

			/* Update externally visible copy of flags */
			self->streamInfo.flags = self->flags[0];

		} /* USAC DASH IPF flushing possible end */
		if(accessUnit < numPrerollAU) { FDKpushBack(hBsAu, auStartAnchor - (int32_t)FDKgetValidBits(hBsAu)); }
		else {
			if((self->buildUpStatus == AACDEC_RSV60_BUILD_UP_ON) ||
			   (self->buildUpStatus == AACDEC_RSV60_BUILD_UP_ON_IN_BAND) ||
			   (self->buildUpStatus == AACDEC_USAC_BUILD_UP_ON)) {
				self->buildUpCnt--;

				if(self->buildUpCnt < 0) { self->buildUpStatus = 0; }
			}

			if(self->flags[0] & AC_USAC) {
				if(self->flushStatus == AACDEC_USAC_DASH_IPF_FLUSH_ON && !(flags & AACDEC_CONCEAL)) {
					self->streamInfo.frameSize = 0;
				}
			}
		}

		if(self->flushStatus != AACDEC_USAC_DASH_IPF_FLUSH_ON) { accessUnit++; }
	} while((accessUnit < numAccessUnits) ||
			((self->flushStatus == AACDEC_USAC_DASH_IPF_FLUSH_ON) && !(flags & AACDEC_CONCEAL)));

bail:

	/* error in renderer part occurred, ErrorStatus was set to invalid output */
	if(fEndAuNotAdjusted && !IS_OUTPUT_VALID(ErrorStatus) && (accessUnit < numPrerollAU)) {
		transportDec_EndAccessUnit(self->hInput);
	}

	/* Update Statistics */
	aacDecoder_UpdateBitStreamCounters(&self->streamInfo, hBs, nBits, ErrorStatus);
	if(((self->streamInfo.numChannels <= 0) || (self->streamInfo.frameSize <= 0) ||
		(self->streamInfo.sampleRate <= 0)) &&
	   IS_OUTPUT_VALID(ErrorStatus)) {
		/* Ensure consistency of IS_OUTPUT_VALID() macro. */
		ErrorStatus = AAC_DEC_UNKNOWN;
	}

	if(!IS_OUTPUT_VALID(ErrorStatus)) { memset(pTimeData, 0, timeDataSize * sizeof(*pTimeData)); }

	return ErrorStatus;
}

void aacDecoder_Close(HANDLE_AACDECODER self) {
	if(self == NULL) return;

	if(self->hLimiter != NULL) { pcmLimiter_Destroy(self->hLimiter); }

	if(self->hPcmUtils != NULL) { pcmDmx_Close(&self->hPcmUtils); }

	FDK_drcDec_Close(&self->hUniDrcDecoder);

	if(self->pMpegSurroundDecoder != NULL) {
		mpegSurroundDecoder_Close((CMpegSurroundDecoder *)self->pMpegSurroundDecoder);
	}

	if(self->hSbrDecoder != NULL) { sbrDecoder_Close(&self->hSbrDecoder); }

	if(self->hInput != NULL) { transportDec_Close(&self->hInput); }

	CAacDecoder_Close(self);
}

CStreamInfo *aacDecoder_GetStreamInfo(HANDLE_AACDECODER self) { return CAacDecoder_GetStreamInfo(self); }

int32_t aacDecoder_GetLibInfo(LIB_INFO_t *info) {
	int32_t i;

	if(info == NULL) { return -1; }

	sbrDecoder_GetLibInfo(info);
	mpegSurroundDecoder_GetLibInfo(info);
	transportDec_GetLibInfo(info);
	FDK_toolsGetLibInfo(info);
	pcmDmx_GetLibInfo(info);
	pcmLimiter_GetLibInfo(info);
	FDK_drcDec_GetLibInfo(info);

	/* search for next free tab */
	for(i = 0; i < FDK_MODULE_LAST; i++) {
		if(info[i].module_id == FDK_NONE) break;
	}
	if(i == FDK_MODULE_LAST) { return -1; }
	info += i;

	info->module_id = FDK_AACDEC;


	/* Set flags */
	info->flags = 0 | CAPF_AAC_LC | CAPF_ER_AAC_LC | CAPF_ER_AAC_SCAL | CAPF_AAC_VCB11 | CAPF_AAC_HCR | CAPF_AAC_RVLC |
				  CAPF_ER_AAC_LD | CAPF_ER_AAC_ELD | CAPF_AAC_CONCEALMENT | CAPF_AAC_DRC | CAPF_AAC_MPEG4 |
				  CAPF_AAC_DRM_BSFORMAT | CAPF_AAC_1024 | CAPF_AAC_960 | CAPF_AAC_512 | CAPF_AAC_480 |
				  CAPF_AAC_ELD_DOWNSCALE

				  | CAPF_AAC_USAC | CAPF_ER_AAC_ELDV2 | CAPF_AAC_UNIDRC;
	/* End of flags */

	return 0;
}
