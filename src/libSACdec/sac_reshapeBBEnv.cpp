

/*********************** MPEG surround decoder library *************************

   Author(s):

   Description: SAC Dec guided envelope shaping

*******************************************************************************/
#include "../libAACdec/newAACDecoder.h"
#include "sac_reshapeBBEnv.h"
#include "sac_bitdec.h"
#include "sac_calcM1andM2.h"
#include "sac_dec.h"
#include "sac_reshapeBBEnv.h"
#include "sac_rom.h"

#define INP_DRY_WET 0
#define INP_DMX     1

#define SF_SHAPE       1
#define SF_DIV32       6
#define SF_FACTOR_SLOT 5

#define START_BB_ENV 0 /* 10 */
#define END_BB_ENV   9 /* 18 */

#define SF_ALPHA1 8
#define SF_BETA1  4

void initBBEnv(spatialDec *self, int32_t initStatesFlag) {
    int32_t ch, k;

    for(ch = 0; ch < self->numOutputChannels; ch++) {
        k = row2channelGES[self->treeConfig][ch];
        self->row2channelDmxGES[ch] = k;
        if(k == -1) continue;

        switch(self->treeConfig) {
            case TREE_212:
                self->row2channelDmxGES[ch] = 0;
                break;
            default:;
        }
    }

    if(initStatesFlag) {
        for(k = 0; k < 2 * MAX_OUTPUT_CHANNELS + MAX_INPUT_CHANNELS; k++) {
            self->reshapeBBEnvState->normNrgPrev__FDK[k] = FL2FXCONST_DBL(0.5f); /* 32768.f*32768.f */
            self->reshapeBBEnvState->normNrgPrevSF[k] = DFRACT_BITS - 1;
            self->reshapeBBEnvState->partNrgPrevSF[k] = 0;
            self->reshapeBBEnvState->partNrgPrev2SF[k] = 0;
            self->reshapeBBEnvState->frameNrgPrevSF[k] = 0;
        }
    }

    self->reshapeBBEnvState->alpha__FDK = FL2FXCONST_DBL(0.99637845575f); /* FDKexp(-64 / (0.4f  * 44100)) */
    self->reshapeBBEnvState->beta__FDK = FL2FXCONST_DBL(0.96436909488f);  /* FDKexp(-64 / (0.04f * 44100)) */
}

static inline void getSlotNrgHQ(int32_t *pReal, int32_t *pImag, int32_t *slotNrg, int32_t maxValSF, int32_t hybBands) {
    int32_t qs;
    int32_t nrg;

    /* qs = 12, 13, 14 */
    slotNrg[0] = ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                  (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
    slotNrg[1] = ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                  (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
    slotNrg[2] = ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                  (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
    /* qs = 15 */
    slotNrg[3] = ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                  (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
    /* qs = 16, 17 */
    nrg = ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
           (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
    slotNrg[4] = nrg + ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                        (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
    /* qs = 18, 19, 20 */
    nrg = ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
           (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
    nrg += ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
            (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
    slotNrg[5] = nrg + ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                        (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
    /* qs = 21, 22 */
    nrg = ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
           (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
    slotNrg[6] = nrg + ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                        (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
    /* qs = 23, 24 */
    if(hybBands > 23) {
        slotNrg[6] += ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                       (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
        slotNrg[6] += ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                       (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
        /* qs = 25, 26, 29, 28, 29 */
        nrg = ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
               (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
        nrg += ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
        nrg += ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
        nrg += ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
        slotNrg[7] = nrg + ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                            (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
        /* qs = 30 ... min(41,hybBands-1) */
        nrg = ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
               (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
        for(qs = 31; qs < hybBands; qs++) {
            nrg += ((fPow2Div2((*pReal++) << maxValSF) >> (SF_FACTOR_SLOT - 1)) +
                    (fPow2Div2((*pImag++) << maxValSF) >> (SF_FACTOR_SLOT - 1)));
        }
        slotNrg[8] = nrg;
    }
    else {
        slotNrg[7] = (int32_t)0;
        slotNrg[8] = (int32_t)0;
    }
}

static inline void combineDryWet(int32_t *pReal, int32_t *pImag, int32_t *pHybOutputRealDry, int32_t *pHybOutputImagDry,
                                 int32_t *pHybOutputRealWet, int32_t *pHybOutputImagWet, int32_t cplxBands,
                                 int32_t hybBands) {
    int32_t qs;

    for(qs = 12; qs < cplxBands; qs++) {
        pReal[qs] = (pHybOutputRealDry[qs] >> 1) + (pHybOutputRealWet[qs] >> 1);
        pImag[qs] = (pHybOutputImagDry[qs] >> 1) + (pHybOutputImagWet[qs] >> 1);
    }
    for(; qs < hybBands; qs++) { pReal[qs] = (pHybOutputRealDry[qs] >> 1) + (pHybOutputRealWet[qs] >> 1); }
}

static inline void slotAmp(int32_t *slotAmp_dry, int32_t *slotAmp_wet, int32_t *pHybOutputRealDry,
                           int32_t *pHybOutputImagDry, int32_t *pHybOutputRealWet, int32_t *pHybOutputImagWet,
                           int32_t cplxBands, int32_t hybBands) {
    int32_t qs;
    int32_t dry, wet;

    dry = wet = FL2FXCONST_DBL(0.0f);
    for(qs = 0; qs < cplxBands; qs++) {
        dry = fAddSaturate(dry, fPow2Div2(pHybOutputRealDry[qs] << (1)) + fPow2Div2(pHybOutputImagDry[qs] << (1)));
        wet = fAddSaturate(wet, fPow2Div2(pHybOutputRealWet[qs] << (1)) + fPow2Div2(pHybOutputImagWet[qs] << (1)));
    }
    for(; qs < hybBands; qs++) {
        dry = fAddSaturate(dry, fPow2Div2(pHybOutputRealDry[qs] << (1)));
        wet = fAddSaturate(wet, fPow2Div2(pHybOutputRealWet[qs] << (1)));
    }
    *slotAmp_dry = dry >> (2 * (1));
    *slotAmp_wet = wet >> (2 * (1));
}

#if defined(__aarch64__)
__attribute__((noinline))
#endif
static void
shapeBBEnv(int32_t *pHybOutputRealDry, int32_t *pHybOutputImagDry, int32_t dryFac, int32_t scale, int32_t cplxBands,
           int32_t hybBands) {
    int32_t qs;

    if(scale == 0) {
        for(qs = 0; qs < cplxBands; qs++) {
            pHybOutputRealDry[qs] = fMultDiv2(pHybOutputRealDry[qs], dryFac);
            pHybOutputImagDry[qs] = fMultDiv2(pHybOutputImagDry[qs], dryFac);
        }
        for(; qs < hybBands; qs++) { pHybOutputRealDry[qs] = fMultDiv2(pHybOutputRealDry[qs], dryFac); }
    }
    else {
        for(qs = 0; qs < cplxBands; qs++) {
            pHybOutputRealDry[qs] = SATURATE_LEFT_SHIFT(fMultDiv2(pHybOutputRealDry[qs], dryFac), scale, DFRACT_BITS);
            pHybOutputImagDry[qs] = SATURATE_LEFT_SHIFT(fMultDiv2(pHybOutputImagDry[qs], dryFac), scale, DFRACT_BITS);
        }
        for(; qs < hybBands; qs++) {
            pHybOutputRealDry[qs] = SATURATE_LEFT_SHIFT(fMultDiv2(pHybOutputRealDry[qs], dryFac), scale, DFRACT_BITS);
        }
    }
}

static void extractBBEnv(spatialDec *self, int32_t inp, int32_t start, int32_t channels, int32_t *pEnv,
                         const SPATIAL_BS_FRAME *frame) {
    int32_t  ch, pb, prevChOffs;
    int32_t  clz, scale, scale_min, envSF;
    int32_t  scaleCur, scalePrev, commonScale;
    int32_t  slotNrgSF, partNrgSF, frameNrgSF;
    int32_t *pPartNrgPrevSF, *pFrameNrgPrevSF;
    int32_t *pNormNrgPrevSF, *pPartNrgPrev2SF;

    int32_t  maxVal, env, frameNrg, normNrg;
    int32_t *pReal, *pImag;
    int32_t *partNrg, *partNrgPrev;

    int32_t  pScratchBuffer[(2 * 42 + MAX_PARAMETER_BANDS)];
    int32_t  resPb[END_BB_ENV - START_BB_ENV];
    int32_t  resPbSF[END_BB_ENV - START_BB_ENV];
    int32_t *slotNrg = pScratchBuffer + (2 * 42);

    RESHAPE_BBENV_STATE *pBBEnvState = self->reshapeBBEnvState;

    int32_t alpha = pBBEnvState->alpha__FDK;
    /*int32_t  alpha1 = (FL2FXCONST_DBL(1.0f) - alpha) << SF_ALPHA1;*/
    int32_t alpha1 = ((int32_t)MAXVAL_DBL - alpha) << SF_ALPHA1;
    int32_t beta = pBBEnvState->beta__FDK;
    /*int32_t  beta1  = (FL2FXCONST_DBL(1.0f) - beta) << SF_BETA1;*/
    int32_t beta1 = ((int32_t)MAXVAL_DBL - beta) << SF_BETA1;

    int32_t shapeActiv = 1;
    int32_t hybBands = fixMin(42, self->hybridBands);
    int32_t staticScale = self->staticDecScale + (1);
    int32_t cplxBands;
    cplxBands = fixMin(42, self->hybridBands);

    for(ch = start; ch < channels; ch++) {
        if(inp == INP_DRY_WET) {
            int32_t ch2 = row2channelGES[self->treeConfig][ch];
            if(ch2 == -1) { continue; }
            else {
                if(frame->tempShapeEnableChannelGES[ch2]) { shapeActiv = 1; }
                else { shapeActiv = 0; }
            }
            prevChOffs = ch;
            pReal = pScratchBuffer;
            pImag = pScratchBuffer + 42;
            combineDryWet(pReal, pImag, self->hybOutputRealDry__FDK[ch], self->hybOutputImagDry__FDK[ch],
                          self->hybOutputRealWet__FDK[ch], self->hybOutputImagWet__FDK[ch], cplxBands, hybBands);
            clz = fMin(getScalefactor(&pReal[12], fMax(0, hybBands - 12)),
                       getScalefactor(&pImag[12], fMax(0, cplxBands - 12)));
        }
        else {
            prevChOffs = ch + self->numOutputChannels;
            pReal = self->hybInputReal__FDK[ch];
            pImag = self->hybInputImag__FDK[ch];
            clz = fMin(getScalefactor(&pReal[12], fMax(0, hybBands - 12)),
                       getScalefactor(&pImag[12], fMax(0, cplxBands - 12)));
        }

        partNrg = partNrgPrev = pBBEnvState->partNrgPrev__FDK[prevChOffs];
        pPartNrgPrevSF = &pBBEnvState->partNrgPrevSF[prevChOffs];
        pFrameNrgPrevSF = &pBBEnvState->frameNrgPrevSF[prevChOffs];
        pNormNrgPrevSF = &pBBEnvState->normNrgPrevSF[prevChOffs];
        pPartNrgPrev2SF = &pBBEnvState->partNrgPrev2SF[prevChOffs];

        /* calculate slot energy */
        {
            getSlotNrgHQ(&pReal[12], &pImag[12], slotNrg, clz, fixMin(42, self->hybridBands)); /* scale slotNrg:
                                                                                                  2*(staticScale-clz) +
                                                                                                  SF_FACTOR_SLOT */
        }

        slotNrgSF = 2 * (staticScale - clz + ((inp == INP_DRY_WET) ? 1 : 0)) + SF_FACTOR_SLOT;
        frameNrgSF = 2 * (staticScale - clz + ((inp == INP_DRY_WET) ? 1 : 0)) + SF_FACTOR_SLOT;

        partNrgSF = fixMax(slotNrgSF - SF_ALPHA1 + 1, pPartNrgPrevSF[0] - pPartNrgPrev2SF[0] + 1);
        scalePrev = fixMax(fixMin(partNrgSF - pPartNrgPrevSF[0], DFRACT_BITS - 1), -(DFRACT_BITS - 1));
        scaleCur = fixMax(fixMin(partNrgSF - slotNrgSF + SF_ALPHA1, DFRACT_BITS - 1), -(DFRACT_BITS - 1));

        maxVal = FL2FXCONST_DBL(0.0f);
        frameNrg = FL2FXCONST_DBL(0.0f);
        if((scaleCur < 0) && (scalePrev < 0)) {
            scaleCur = -scaleCur;
            scalePrev = -scalePrev;
            for(pb = START_BB_ENV; pb < END_BB_ENV; pb++) {
                partNrg[pb] =
                    ((fMultDiv2(alpha1, slotNrg[pb]) << scaleCur) + (fMultDiv2(alpha, partNrgPrev[pb]) << scalePrev))
                    << 1;
                maxVal |= partNrg[pb];
                frameNrg += slotNrg[pb] >> 3;
            }
        }
        else if((scaleCur >= 0) && (scalePrev >= 0)) {
            for(pb = START_BB_ENV; pb < END_BB_ENV; pb++) {
                partNrg[pb] =
                    ((fMultDiv2(alpha1, slotNrg[pb]) >> scaleCur) + (fMultDiv2(alpha, partNrgPrev[pb]) >> scalePrev))
                    << 1;
                maxVal |= partNrg[pb];
                frameNrg += slotNrg[pb] >> 3;
            }
        }
        else if((scaleCur < 0) && (scalePrev >= 0)) {
            scaleCur = -scaleCur;
            for(pb = START_BB_ENV; pb < END_BB_ENV; pb++) {
                partNrg[pb] =
                    ((fMultDiv2(alpha1, slotNrg[pb]) << scaleCur) + (fMultDiv2(alpha, partNrgPrev[pb]) >> scalePrev))
                    << 1;
                maxVal |= partNrg[pb];
                frameNrg += slotNrg[pb] >> 3;
            }
        }
        else { /* if ( (scaleCur >= 0) && (scalePrev < 0) ) */
            scalePrev = -scalePrev;
            for(pb = START_BB_ENV; pb < END_BB_ENV; pb++) {
                partNrg[pb] =
                    ((fMultDiv2(alpha1, slotNrg[pb]) >> scaleCur) + (fMultDiv2(alpha, partNrgPrev[pb]) << scalePrev))
                    << 1;
                maxVal |= partNrg[pb];
                frameNrg += slotNrg[pb] >> 3;
            }
        }

        /* frameNrg /= (END_BB_ENV - START_BB_ENV); 0.88888888888f =
         * (1/(END_BB_ENV-START_BB_ENV)<<3; shift with 3 is compensated in loop
         * above */
        frameNrg = fMult(frameNrg, FL2FXCONST_DBL(0.88888888888f));

        /* store scalefactor and headroom for part nrg prev */
        pPartNrgPrevSF[0] = partNrgSF;
        pPartNrgPrev2SF[0] = fixMax(0, CntLeadingZeros(maxVal) - 1);

        commonScale = fixMax(frameNrgSF - SF_ALPHA1 + 1, pFrameNrgPrevSF[0] + 1);
        scalePrev = fixMin(commonScale - pFrameNrgPrevSF[0], DFRACT_BITS - 1);
        scaleCur = fixMin(commonScale - frameNrgSF + SF_ALPHA1, DFRACT_BITS - 1);
        frameNrgSF = commonScale;

        frameNrg = ((fMultDiv2(alpha1, frameNrg) >> scaleCur) +
                    (fMultDiv2(alpha, pBBEnvState->frameNrgPrev__FDK[prevChOffs]) >> scalePrev))
                   << 1;

        clz = fixMax(0, CntLeadingZeros(frameNrg) - 1);
        pBBEnvState->frameNrgPrev__FDK[prevChOffs] = frameNrg << clz;
        pFrameNrgPrevSF[0] = frameNrgSF - clz;

        env = FL2FXCONST_DBL(0.0f);
        scale = clz + partNrgSF - frameNrgSF;
        scale_min = DFRACT_BITS - 1;
        for(pb = START_BB_ENV; pb < END_BB_ENV; pb++) {
            if((partNrg[pb] | slotNrg[pb]) != FL2FXCONST_DBL(0.0f)) {
                int32_t s;
                int32_t sc = 0;
                int32_t sn = fixMax(0, CntLeadingZeros(slotNrg[pb]) - 1);
                int32_t inv_sqrt = invSqrtNorm2(partNrg[pb], &sc);
                int32_t res = fMult(slotNrg[pb] << sn, fPow2(inv_sqrt));

                s = fixMax(0, CntLeadingZeros(res) - 1);
                res = res << s;

                sc = scale - (2 * sc - sn - s);
                scale_min = fixMin(scale_min, sc);

                resPb[pb] = res;
                resPbSF[pb] = sc;
            }
            else {
                resPb[pb] = (int32_t)0;
                resPbSF[pb] = 0;
            }
        }

        scale_min = 4 - scale_min;

        for(pb = START_BB_ENV; pb < END_BB_ENV; pb++) {
            int32_t sc = fixMax(fixMin(resPbSF[pb] + scale_min, DFRACT_BITS - 1), -(DFRACT_BITS - 1));

            if(sc < 0) { env += resPb[pb] << (-sc); }
            else { env += resPb[pb] >> (sc); }
        }

        env = fMultDiv2(env, pBBEnvState->frameNrgPrev__FDK[prevChOffs]);
        envSF = slotNrgSF + scale_min + 1;

        commonScale = fixMax(envSF - SF_BETA1 + 1, pNormNrgPrevSF[0] + 1);
        scalePrev = fixMin(commonScale - pNormNrgPrevSF[0], DFRACT_BITS - 1);
        scaleCur = fixMin(commonScale - envSF + SF_BETA1, DFRACT_BITS - 1);

        normNrg = ((fMultDiv2(beta1, env) >> scaleCur) +
                   (fMultDiv2(beta, pBBEnvState->normNrgPrev__FDK[prevChOffs]) >> scalePrev))
                  << 1;

        clz = fixMax(0, CntLeadingZeros(normNrg) - 1);
        pBBEnvState->normNrgPrev__FDK[prevChOffs] = normNrg << clz;
        pNormNrgPrevSF[0] = commonScale - clz;

        if(shapeActiv) {
            if((env | normNrg) != FL2FXCONST_DBL(0.0f)) {
                int32_t sc, se, sn;
                se = fixMax(0, CntLeadingZeros(env) - 1);
                sc = commonScale + SF_DIV32 - envSF + se;
                env = fMult(sqrtFixp((env << se) >> (sc & 0x1)), invSqrtNorm2(normNrg, &sn));

                sc = fixMin((sc >> 1) - sn, DFRACT_BITS - 1);
                if(sc < 0) { env <<= (-sc); }
                else { env >>= (sc); }
            }
            /* env is scaled by SF_DIV32/2 bits */
        }
        pEnv[ch] = env;
    }
}

void SpatialDecReshapeBBEnv(spatialDec *self, const SPATIAL_BS_FRAME *frame, int32_t ts) {
    int32_t ch, scale;
    int32_t dryFacSF, slotAmpSF;
    int32_t tmp, dryFac, envShape;
    int32_t slotAmp_dry, slotAmp_wet, slotAmp_ratio;
    int32_t envDry[MAX_OUTPUT_CHANNELS], envDmx[2];

    int32_t cplxBands;
    int32_t hybBands = self->hybridBands - 6;

    cplxBands = self->hybridBands - 6;

    /* extract downmix envelope(s) */
    switch(self->treeConfig) {
        default:
            extractBBEnv(self, INP_DMX, 0, fMin(self->numInputChannels, 2), envDmx, frame);
    }

    /* extract dry and wet envelopes */
    extractBBEnv(self, INP_DRY_WET, 0, self->numOutputChannels, envDry, frame);

    for(ch = 0; ch < self->numOutputChannels; ch++) {
        int32_t ch2;

        ch2 = row2channelGES[self->treeConfig][ch];

        if(ch2 == -1) continue;

        if(frame->tempShapeEnableChannelGES[ch2]) {
            int32_t sc;

            /* reshape dry and wet signals according to transmitted envelope */

            /* De-quantize GES data */
            assert((frame->bsEnvShapeData[ch2][ts] >= 0) && (frame->bsEnvShapeData[ch2][ts] <= 4));
            assert((self->envQuantMode == 0) || (self->envQuantMode == 1));
            envShape = FX_CFG2FX_DBL(envShapeDataTable__FDK[frame->bsEnvShapeData[ch2][ts]][self->envQuantMode]);

            /* get downmix channel */
            ch2 = self->row2channelDmxGES[ch];

            /* multiply ratio with dmx envelope; tmp is scaled by SF_DIV32/2+SF_SHAPE
             * bits */
            if(ch2 == 2) { tmp = fMultDiv2(envShape, envDmx[0]) + fMultDiv2(envShape, envDmx[1]); }
            else { tmp = fMult(envShape, envDmx[ch2]); }

            /* weighting factors */
            dryFacSF = slotAmpSF = 0;
            dryFac = slotAmp_ratio = FL2FXCONST_DBL(0.0f);

            /* dryFac will be scaled by dryFacSF bits */
            if(envDry[ch] != FL2FXCONST_DBL(0.0f)) {
                envDry[ch] = invSqrtNorm2(envDry[ch], &dryFacSF);
                dryFac = fMultDiv2(tmp, fPow2Div2(envDry[ch])) << 2;
                dryFacSF = SF_SHAPE + 2 * dryFacSF;
            }

            /* calculate slotAmp_dry and slotAmp_wet */
            slotAmp(&slotAmp_dry, &slotAmp_wet, &self->hybOutputRealDry__FDK[ch][6],
                    &self->hybOutputImagDry__FDK[ch][6], &self->hybOutputRealWet__FDK[ch][6],
                    &self->hybOutputImagWet__FDK[ch][6], cplxBands, hybBands);

            /* slotAmp_ratio will be scaled by slotAmpSF bits */
            if(slotAmp_dry != FL2FXCONST_DBL(0.0f)) {
                sc = fixMax(0, CntLeadingZeros(slotAmp_wet) - 1);
                sc = sc - (sc & 1);

                slotAmp_wet = sqrtFixp(slotAmp_wet << sc);
                slotAmp_dry = invSqrtNorm2(slotAmp_dry, &slotAmpSF);

                slotAmp_ratio = fMult(slotAmp_wet, slotAmp_dry);
                slotAmpSF = slotAmpSF - (sc >> 1);
            }

            /* calculate common scale factor */
            scale = fixMax(3, fixMax(dryFacSF, slotAmpSF)); /* scale is at least with 3
                                                               bits to avoid overflows
                                                               when calculating dryFac  */
            dryFac = dryFac >> fixMin(scale - dryFacSF, DFRACT_BITS - 1);
            slotAmp_ratio = slotAmp_ratio >> fixMin(scale - slotAmpSF, DFRACT_BITS - 1);

            /* limit dryFac */
            dryFac = fixMax(FL2FXCONST_DBL(0.25f) >> (int32_t)fixMin(2 * scale, DFRACT_BITS - 1),
                            fMult(dryFac, slotAmp_ratio) - (slotAmp_ratio >> fixMin(scale, DFRACT_BITS - 1)) +
                                (dryFac >> fixMin(scale, DFRACT_BITS - 1)));
            dryFac = fixMin(FL2FXCONST_DBL(0.50f) >> (int32_t)fixMin(2 * scale - 3, DFRACT_BITS - 1),
                            dryFac); /* reduce shift bits by 3, because upper
                                        limit 4.0 is scaled with 3 bits */
            scale = 2 * scale + 1;

            /* improve precision for dryFac */
            sc = fixMax(0, CntLeadingZeros(dryFac) - 1);
            dryFac = dryFac << (int32_t)fixMin(scale, sc);
            scale = scale - fixMin(scale, sc);

            /* shaping */
            shapeBBEnv(&self->hybOutputRealDry__FDK[ch][6], &self->hybOutputImagDry__FDK[ch][6], dryFac,
                       fixMin(scale, DFRACT_BITS - 1), cplxBands, hybBands);
        }
    }
}
