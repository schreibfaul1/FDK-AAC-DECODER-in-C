
/************************* MPEG-D DRC decoder library **************************

   Author(s):

   Description:

*******************************************************************************/

#include "drcDec_gainDecoder.h"
#include <stdlib.h>
#include "../libAACdec/newAACDecoder.h"
#include "drcDec_tools.h"
#include "drcDec_types.h"
#include "drcGainDec_init.h"
#include "drcGainDec_preprocess.h"
#include "drcGainDec_process.h"

/*******************************************/
/* static functions                        */
/*******************************************/

static int32_t _fitsLocation(DRC_INSTRUCTIONS_UNI_DRC_t* pInst, const GAIN_DEC_LOCATION drcLocation) {
    int32_t downmixId = pInst->drcApplyToDownmix ? pInst->downmixId[0] : 0;
    switch(drcLocation) {
        case GAIN_DEC_DRC1:
            return (downmixId == 0);
        case GAIN_DEC_DRC1_DRC2:
            return ((downmixId == 0) || (downmixId == DOWNMIX_ID_ANY_DOWNMIX));
        case GAIN_DEC_DRC2:
            return (downmixId == DOWNMIX_ID_ANY_DOWNMIX);
        case GAIN_DEC_DRC3:
            return ((downmixId != 0) && (downmixId != DOWNMIX_ID_ANY_DOWNMIX));
        case GAIN_DEC_DRC2_DRC3:
            return (downmixId != 0);
    }
    return 0;
}

static void _setChannelGains(HANDLE_DRC_GAIN_DECODER hGainDec, const int32_t numChannelGains, const int32_t* channelGainDb) {
    int32_t i, channelGain_e;
    int32_t channelGain;
    assert(numChannelGains <= 8);
    for(i = 0; i < numChannelGains; i++) {
        if(channelGainDb[i] == (int32_t)MINVAL_DBL) { hGainDec->channelGain[i] = (int32_t)0; }
        else {
            /* add loudness normalisation gain (dB) to channel gain (dB) */
            int32_t tmp_channelGainDb = (channelGainDb[i] >> 1) + (hGainDec->loudnessNormalisationGainDb >> 2);
            tmp_channelGainDb = SATURATE_LEFT_SHIFT(tmp_channelGainDb, 1, DFRACT_BITS);
            channelGain = dB2lin(tmp_channelGainDb, 8, &channelGain_e);
            hGainDec->channelGain[i] = scaleValue(channelGain, channelGain_e - 8);
        }
    }
}

/*******************************************/
/* public functions                        */
/*******************************************/

DRC_ERROR
drcDec_GainDecoder_Open(HANDLE_DRC_GAIN_DECODER* phGainDec) {
    DRC_GAIN_DECODER* hGainDec = NULL;

    hGainDec = (DRC_GAIN_DECODER*)FDKcalloc(1, sizeof(DRC_GAIN_DECODER));
    if(hGainDec == NULL) return DE_MEMORY_ERROR;

    hGainDec->multiBandActiveDrcIndex = -1;
    hGainDec->channelGainActiveDrcIndex = -1;

    *phGainDec = hGainDec;

    return DE_OK;
}

DRC_ERROR
drcDec_GainDecoder_Init(HANDLE_DRC_GAIN_DECODER hGainDec) {
    DRC_ERROR err = DE_OK;

    err = initGainDec(hGainDec);
    if(err) return err;

    initDrcGainBuffers(hGainDec->frameSize, &hGainDec->drcGainBuffers);

    return err;
}

DRC_ERROR
drcDec_GainDecoder_SetParam(HANDLE_DRC_GAIN_DECODER hGainDec, const GAIN_DEC_PARAM paramType, const int32_t paramValue) {
    switch(paramType) {
        case GAIN_DEC_FRAME_SIZE:
            if(paramValue < 0) return DE_PARAM_OUT_OF_RANGE;
            hGainDec->frameSize = paramValue;
            break;
        case GAIN_DEC_SAMPLE_RATE:
            if(paramValue < 0) return DE_PARAM_OUT_OF_RANGE;
            hGainDec->deltaTminDefault = getDeltaTmin(paramValue);
            break;
        default:
            return DE_PARAM_INVALID;
    }
    return DE_OK;
}

DRC_ERROR
drcDec_GainDecoder_SetCodecDependentParameters(HANDLE_DRC_GAIN_DECODER hGainDec, const DELAY_MODE delayMode, const int32_t timeDomainSupported,
                                               const SUBBAND_DOMAIN_MODE subbandDomainSupported) {
    if((delayMode != DM_REGULAR_DELAY) && (delayMode != DM_LOW_DELAY)) { return DE_NOT_OK; }
    hGainDec->delayMode = delayMode;
    hGainDec->timeDomainSupported = timeDomainSupported;
    hGainDec->subbandDomainSupported = subbandDomainSupported;

    return DE_OK;
}

DRC_ERROR
drcDec_GainDecoder_Config(HANDLE_DRC_GAIN_DECODER hGainDec, HANDLE_UNI_DRC_CONFIG hUniDrcConfig, const uint8_t numSelectedDrcSets,
                          const int8_t* selectedDrcSetIds, const uint8_t* selectedDownmixIds) {
    DRC_ERROR err = DE_OK;
    int32_t   a;

    hGainDec->nActiveDrcs = 0;
    hGainDec->multiBandActiveDrcIndex = -1;
    hGainDec->channelGainActiveDrcIndex = -1;
    for(a = 0; a < numSelectedDrcSets; a++) {
        err = initActiveDrc(hGainDec, hUniDrcConfig, selectedDrcSetIds[a], selectedDownmixIds[a]);
        if(err) return err;
    }

    err = initActiveDrcOffset(hGainDec);
    if(err) return err;

    return err;
}

DRC_ERROR
drcDec_GainDecoder_Close(HANDLE_DRC_GAIN_DECODER* phGainDec) {
    if(*phGainDec != NULL) {
        free(*phGainDec);
        *phGainDec = NULL;
    }

    return DE_OK;
}

DRC_ERROR
drcDec_GainDecoder_Preprocess(HANDLE_DRC_GAIN_DECODER hGainDec, HANDLE_UNI_DRC_GAIN hUniDrcGain, const int32_t loudnessNormalizationGainDb,
                              const int16_t boost, const int16_t compress) {
    DRC_ERROR err = DE_OK;
    int32_t   a, c;

    /* lnbPointer is the index on the most recent node buffer */
    hGainDec->drcGainBuffers.lnbPointer++;
    if(hGainDec->drcGainBuffers.lnbPointer >= NUM_LNB_FRAMES) hGainDec->drcGainBuffers.lnbPointer = 0;

    for(a = 0; a < hGainDec->nActiveDrcs; a++) {
        /* prepare gain interpolation of sequences used by copying and modifying
         * nodes in node buffers */
        err = prepareDrcGain(hGainDec, hUniDrcGain, compress, boost, loudnessNormalizationGainDb, a);
        if(err) return err;
    }

    for(a = 0; a < MAX_ACTIVE_DRCS; a++) {
        for(c = 0; c < 8; c++) { hGainDec->activeDrc[a].lnbIndexForChannel[c][hGainDec->drcGainBuffers.lnbPointer] = -1; /* "no DRC processing" */ }
        hGainDec->activeDrc[a].subbandGainsReady = 0;
    }

    for(c = 0; c < 8; c++) { hGainDec->drcGainBuffers.channelGain[c][hGainDec->drcGainBuffers.lnbPointer] = FL2FXCONST_DBL(1.0f / (float)(1 << 8)); }

    return err;
}

/* create gain sequence out of gain sequences of last frame for concealment and
 * flushing */
DRC_ERROR
drcDec_GainDecoder_Conceal(HANDLE_DRC_GAIN_DECODER hGainDec, HANDLE_UNI_DRC_CONFIG hUniDrcConfig, HANDLE_UNI_DRC_GAIN hUniDrcGain) {
    int32_t                     seq, gainSequenceCount;
    DRC_COEFFICIENTS_UNI_DRC_t* pCoef = selectDrcCoefficients(hUniDrcConfig, LOCATION_SELECTED);
    if(pCoef && pCoef->gainSequenceCount) { gainSequenceCount = fMin(pCoef->gainSequenceCount, (uint8_t)12); }
    else { gainSequenceCount = 1; }

    for(seq = 0; seq < gainSequenceCount; seq++) {
        int32_t lastNodeIndex = 0;
        int16_t lastGainDb = (int16_t)0;

        lastNodeIndex = hUniDrcGain->nNodes[seq] - 1;
        if((lastNodeIndex >= 0) && (lastNodeIndex < 16)) { lastGainDb = hUniDrcGain->gainNode[seq][lastNodeIndex].gainDb; }

        hUniDrcGain->nNodes[seq] = 1;
        if(lastGainDb > (int16_t)0) { hUniDrcGain->gainNode[seq][0].gainDb = FX_DBL2FX_SGL(fMult(FL2FXCONST_SGL(0.9f), lastGainDb)); }
        else { hUniDrcGain->gainNode[seq][0].gainDb = FX_DBL2FX_SGL(fMult(FL2FXCONST_SGL(0.98f), lastGainDb)); }
        hUniDrcGain->gainNode[seq][0].time = hGainDec->frameSize - 1;
    }
    return DE_OK;
}

void drcDec_GainDecoder_SetChannelGains(HANDLE_DRC_GAIN_DECODER hGainDec, const int32_t numChannels, const int32_t frameSize,
                                        const int32_t* channelGainDb, const int32_t audioBufferChannelOffset, int32_t* audioBuffer) {
    int32_t c, i;

    if(hGainDec->channelGainActiveDrcIndex >= 0) {
        /* channel gains will be applied in drcDec_GainDecoder_ProcessTimeDomain or
         * drcDec_GainDecoder_ProcessSubbandDomain, respectively. */
        _setChannelGains(hGainDec, numChannels, channelGainDb);

        if(!hGainDec->status) { /* overwrite previous channel gains at startup */
            DRC_GAIN_BUFFERS* pDrcGainBuffers = &hGainDec->drcGainBuffers;
            for(c = 0; c < numChannels; c++) {
                for(i = 0; i < NUM_LNB_FRAMES; i++) { pDrcGainBuffers->channelGain[c][i] = hGainDec->channelGain[c]; }
            }
            hGainDec->status = 1;
        }
    }
    else {
        /* smooth and apply channel gains */
        int32_t prevChannelGain[8];
        for(c = 0; c < numChannels; c++) { prevChannelGain[c] = hGainDec->channelGain[c]; }

        _setChannelGains(hGainDec, numChannels, channelGainDb);

        if(!hGainDec->status) { /* overwrite previous channel gains at startup */
            for(c = 0; c < numChannels; c++) prevChannelGain[c] = hGainDec->channelGain[c];
            hGainDec->status = 1;
        }

        for(c = 0; c < numChannels; c++) {
            int32_t n_min = fMin(fMin(CntLeadingZeros(prevChannelGain[c]), CntLeadingZeros(hGainDec->channelGain[c])) - 1, 9);
            int32_t gain = prevChannelGain[c] << n_min;
            int32_t stepsize = ((hGainDec->channelGain[c] << n_min) - gain);
            if(stepsize != (int32_t)0) {
                if(frameSize == 1024) stepsize = stepsize >> 10;
                else
                    stepsize = (int32_t)stepsize / frameSize;
            }
            n_min = 9 - n_min;
#ifdef FUNCTION_drcDec_GainDecoder_SetChannelGains_func1
            drcDec_GainDecoder_SetChannelGains_func1(audioBuffer, gain, stepsize, n_min, frameSize);
#else
            for(i = 0; i < frameSize; i++) {
                audioBuffer[i] = fMultDiv2(audioBuffer[i], gain) << n_min;
                gain += stepsize;
            }
#endif
            audioBuffer += audioBufferChannelOffset;
        }
    }
}

DRC_ERROR
drcDec_GainDecoder_ProcessTimeDomain(HANDLE_DRC_GAIN_DECODER hGainDec, const int32_t delaySamples, const GAIN_DEC_LOCATION drcLocation,
                                     const int32_t channelOffset, const int32_t drcChannelOffset, const int32_t numChannelsProcessed,
                                     const int32_t timeDataChannelOffset, int32_t* audioIOBuffer) {
    DRC_ERROR err = DE_OK;
    int32_t   a;

    if(!hGainDec->timeDomainSupported) { return DE_NOT_OK; }

    for(a = 0; a < hGainDec->nActiveDrcs; a++) {
        if(!_fitsLocation(hGainDec->activeDrc[a].pInst, drcLocation)) continue;

        /* Apply DRC */
        err = processDrcTime(hGainDec, a, delaySamples, channelOffset, drcChannelOffset, numChannelsProcessed, timeDataChannelOffset, audioIOBuffer);
        if(err) return err;
    }

    return err;
}

DRC_ERROR
drcDec_GainDecoder_ProcessSubbandDomain(HANDLE_DRC_GAIN_DECODER hGainDec, const int32_t delaySamples, const GAIN_DEC_LOCATION drcLocation,
                                        const int32_t channelOffset, const int32_t drcChannelOffset, const int32_t numChannelsProcessed,
                                        const int32_t processSingleTimeslot, int32_t* audioIOBufferReal[], int32_t* audioIOBufferImag[]) {
    DRC_ERROR err = DE_OK;
    int32_t   a;

    if(hGainDec->subbandDomainSupported == SDM_OFF) { return DE_NOT_OK; }

    for(a = 0; a < hGainDec->nActiveDrcs; a++) {
        if(!_fitsLocation(hGainDec->activeDrc[a].pInst, drcLocation)) continue;

        /* Apply DRC */
        err = processDrcSubband(hGainDec, a, delaySamples, channelOffset, drcChannelOffset, numChannelsProcessed, processSingleTimeslot,
                                audioIOBufferReal, audioIOBufferImag);
        if(err) return err;
    }

    return err;
}

DRC_ERROR
drcDec_GainDecoder_SetLoudnessNormalizationGainDb(HANDLE_DRC_GAIN_DECODER hGainDec, int32_t loudnessNormalizationGainDb) {
    hGainDec->loudnessNormalisationGainDb = loudnessNormalizationGainDb;

    return DE_OK;
}

int32_t drcDec_GainDecoder_GetFrameSize(HANDLE_DRC_GAIN_DECODER hGainDec) {
    if(hGainDec == NULL) return -1;

    return hGainDec->frameSize;
}

int32_t drcDec_GainDecoder_GetDeltaTminDefault(HANDLE_DRC_GAIN_DECODER hGainDec) {
    if(hGainDec == NULL) return -1;

    return hGainDec->deltaTminDefault;
}
