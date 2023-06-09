

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: joint stereo processing

*******************************************************************************/

#include <memory.h>
#include <stdint.h>
#include "../libAACdec/channelinfo.h"
#include "../libFDK/FDK_bitstream.h"
#include "aac_rom.h"
#include "newAACDecoder.h"

enum { L = 0, R = 1 };

#include "../libAACdec/block.h"

int32_t CJointStereo_Read(HANDLE_FDK_BITSTREAM bs, CJointStereoData_t *pJointStereoData, const int32_t windowGroups,
                          const int32_t scaleFactorBandsTransmitted, const int32_t max_sfb_ste_clear,
                          CJointStereoPersistentData_t *pJointStereoPersistentData, CCplxPredictionData_t *cplxPredictionData,
                          int32_t cplxPredictionActiv, int32_t scaleFactorBandsTotal, int32_t windowSequence, const uint32_t flags) {
    const uint8_t SFB_PER_PRED_BAND = 2;
    
    int32_t group, band;

    pJointStereoData->MsMaskPresent = (uint8_t)FDKreadBits(bs, 2);

    memset(pJointStereoData->MsUsed, 0, scaleFactorBandsTransmitted * sizeof(uint8_t));

    pJointStereoData->cplx_pred_flag = 0;
    if(cplxPredictionActiv) {
        cplxPredictionData->pred_dir = 0;
        cplxPredictionData->complex_coef = 0;
        cplxPredictionData->use_prev_frame = 0;
        cplxPredictionData->igf_pred_dir = 0;
    }

    switch(pJointStereoData->MsMaskPresent) {
        case 0: /* no M/S */
            /* all flags are already cleared */
            break;

        case 1: /* read ms_used */
            for(group = 0; group < windowGroups; group++) {
                for(band = 0; band < scaleFactorBandsTransmitted; band++) { pJointStereoData->MsUsed[band] |= (FDKreadBits(bs, 1) << group); }
            }
            break;

        case 2: /* full spectrum M/S */
            for(band = 0; band < scaleFactorBandsTransmitted; band++) { pJointStereoData->MsUsed[band] = 255; /* set all flags to 1 */ }
            break;

        case 3:
            /* M/S coding is disabled, complex stereo prediction is enabled */
            if(flags & (AC_USAC | AC_RSVD50 | AC_RSV603DA)) {
                if(cplxPredictionActiv) { /* 'if (stereoConfigIndex == 0)' */

                    pJointStereoData->cplx_pred_flag = 1;

                    /* cplx_pred_data()  cp. ISO/IEC FDIS 23003-3:2011(E)  Table 26 */
                    int32_t cplx_pred_all = 0; /* local use only */
                    cplx_pred_all = FDKreadBits(bs, 1);

                    if(cplx_pred_all) {
                        for(group = 0; group < windowGroups; group++) {
                            uint8_t groupmask = ((uint8_t)1 << group);
                            for(band = 0; band < scaleFactorBandsTransmitted; band++) { pJointStereoData->MsUsed[band] |= groupmask; }
                        }
                    }
                    else {
                        for(group = 0; group < windowGroups; group++) {
                            for(band = 0; band < scaleFactorBandsTransmitted; band += SFB_PER_PRED_BAND) {
                                pJointStereoData->MsUsed[band] |= (FDKreadBits(bs, 1) << group);
                                if((band + 1) < scaleFactorBandsTotal) {
                                    pJointStereoData->MsUsed[band + 1] |= (pJointStereoData->MsUsed[band] & ((uint8_t)1 << group));
                                }
                            }
                        }
                    }
                }
                else { return -1; }
            }
            break;
    }

    if(cplxPredictionActiv) {
        /* If all sfb are MS-ed then no complex prediction */
        if(pJointStereoData->MsMaskPresent == 3) {
            if(pJointStereoData->cplx_pred_flag) {
                int32_t delta_code_time = 0;

                /* set pointer to Huffman codebooks */
                const CodeBookDescription_t *hcb = &AACcodeBookDescriptionTable[BOOKSCL];
                /* set predictors to zero in case of a transition from int32_t to int16_t
                 * window sequences and vice versa */
                if(((windowSequence == BLOCK_SHORT) && (pJointStereoPersistentData->winSeqPrev != BLOCK_SHORT)) ||
                   ((pJointStereoPersistentData->winSeqPrev == BLOCK_SHORT) && (windowSequence != BLOCK_SHORT))) {
                    memset(pJointStereoPersistentData->alpha_q_re_prev, 0, JointStereoMaximumGroups * JointStereoMaximumBands * sizeof(int16_t));
                    memset(pJointStereoPersistentData->alpha_q_im_prev, 0, JointStereoMaximumGroups * JointStereoMaximumBands * sizeof(int16_t));
                }
                {
                    memset(cplxPredictionData->alpha_q_re, 0, JointStereoMaximumGroups * JointStereoMaximumBands * sizeof(int16_t));
                    memset(cplxPredictionData->alpha_q_im, 0, JointStereoMaximumGroups * JointStereoMaximumBands * sizeof(int16_t));
                }

                /* 0 = mid->side prediction, 1 = side->mid prediction */
                cplxPredictionData->pred_dir = FDKreadBits(bs, 1);
                cplxPredictionData->complex_coef = FDKreadBits(bs, 1);

                if(cplxPredictionData->complex_coef) {
                    if(flags & AC_INDEP) { cplxPredictionData->use_prev_frame = 0; }
                    else { cplxPredictionData->use_prev_frame = FDKreadBits(bs, 1); }
                }

                if(flags & AC_INDEP) { delta_code_time = 0; }
                else { delta_code_time = FDKreadBits(bs, 1); }

                {
                    int32_t last_alpha_q_re = 0, last_alpha_q_im = 0;

                    for(group = 0; group < windowGroups; group++) {
                        for(band = 0; band < scaleFactorBandsTransmitted; band += SFB_PER_PRED_BAND) {
                            if(delta_code_time == 1) {
                                if(group > 0) {
                                    last_alpha_q_re = cplxPredictionData->alpha_q_re[group - 1][band];
                                    last_alpha_q_im = cplxPredictionData->alpha_q_im[group - 1][band];
                                }
                                else if((windowSequence == BLOCK_SHORT) && (pJointStereoPersistentData->winSeqPrev == BLOCK_SHORT)) {
                                    /* Included for error-robustness */
                                    if(pJointStereoPersistentData->winGroupsPrev == 0) return -1;

                                    last_alpha_q_re =
                                        pJointStereoPersistentData->alpha_q_re_prev[pJointStereoPersistentData->winGroupsPrev - 1][band];
                                    last_alpha_q_im =
                                        pJointStereoPersistentData->alpha_q_im_prev[pJointStereoPersistentData->winGroupsPrev - 1][band];
                                }
                                else {
                                    last_alpha_q_re = pJointStereoPersistentData->alpha_q_re_prev[group][band];
                                    last_alpha_q_im = pJointStereoPersistentData->alpha_q_im_prev[group][band];
                                }
                            }
                            else {
                                if(band > 0) {
                                    last_alpha_q_re = cplxPredictionData->alpha_q_re[group][band - 1];
                                    last_alpha_q_im = cplxPredictionData->alpha_q_im[group][band - 1];
                                }
                                else {
                                    last_alpha_q_re = 0;
                                    last_alpha_q_im = 0;
                                }

                            } /* if (delta_code_time == 1) */

                            if(pJointStereoData->MsUsed[band] & ((uint8_t)1 << group)) {
                                int32_t dpcm_alpha_re, dpcm_alpha_im;

                                dpcm_alpha_re = CBlock_DecodeHuffmanWord(bs, hcb);
                                dpcm_alpha_re -= 60;
                                dpcm_alpha_re *= -1;

                                cplxPredictionData->alpha_q_re[group][band] = dpcm_alpha_re + last_alpha_q_re;

                                if(cplxPredictionData->complex_coef) {
                                    dpcm_alpha_im = CBlock_DecodeHuffmanWord(bs, hcb);
                                    dpcm_alpha_im -= 60;
                                    dpcm_alpha_im *= -1;

                                    cplxPredictionData->alpha_q_im[group][band] = dpcm_alpha_im + last_alpha_q_im;
                                }
                                else { cplxPredictionData->alpha_q_im[group][band] = 0; }
                            }
                            else {
                                cplxPredictionData->alpha_q_re[group][band] = 0;
                                cplxPredictionData->alpha_q_im[group][band] = 0;
                            } /* if (pJointStereoData->MsUsed[band] & ((uint8_t)1 << group)) */

                            if((band + 1) < scaleFactorBandsTransmitted) { /* <= this should be the
                                                                              correct way (cp.
                                                                              ISO_IEC_FDIS_23003-0(E) */
                                /*    7.7.2.3.2 Decoding of prediction coefficients) */
                                cplxPredictionData->alpha_q_re[group][band + 1] = cplxPredictionData->alpha_q_re[group][band];
                                cplxPredictionData->alpha_q_im[group][band + 1] = cplxPredictionData->alpha_q_im[group][band];
                            } /* if ((band+1)<scaleFactorBandsTotal) */

                            pJointStereoPersistentData->alpha_q_re_prev[group][band] = cplxPredictionData->alpha_q_re[group][band];
                            pJointStereoPersistentData->alpha_q_im_prev[group][band] = cplxPredictionData->alpha_q_im[group][band];
                        }

                        for(band = scaleFactorBandsTransmitted; band < max_sfb_ste_clear; band++) {
                            cplxPredictionData->alpha_q_re[group][band] = 0;
                            cplxPredictionData->alpha_q_im[group][band] = 0;
                            pJointStereoPersistentData->alpha_q_re_prev[group][band] = 0;
                            pJointStereoPersistentData->alpha_q_im_prev[group][band] = 0;
                        }
                    }
                }
            }
        }
        else {
            for(group = 0; group < windowGroups; group++) {
                for(band = 0; band < max_sfb_ste_clear; band++) {
                    pJointStereoPersistentData->alpha_q_re_prev[group][band] = 0;
                    pJointStereoPersistentData->alpha_q_im_prev[group][band] = 0;
                }
            }
        }

        pJointStereoPersistentData->winGroupsPrev = windowGroups;
    }

    return 0;
}

static void CJointStereo_filterAndAdd(int32_t *in, int32_t len, int32_t windowLen, const int32_t *coeff, int32_t *out,
                                      uint8_t isCurrent /* output values with even index get a
                                                         positve addon (=1) or a negative addon
                                                         (=0) */
) {
    int32_t i, j;

    int32_t indices_1[] = {2, 1, 0, 1, 2, 3};
    int32_t indices_2[] = {1, 0, 0, 2, 3, 4};
    int32_t indices_3[] = {0, 0, 1, 3, 4, 5};

    int32_t subtr_1[] = {6, 5, 4, 2, 1, 1};
    int32_t subtr_2[] = {5, 4, 3, 1, 1, 2};
    int32_t subtr_3[] = {4, 3, 2, 1, 2, 3};

    if(isCurrent == 1) {
        /* exploit the symmetry of the table: coeff[6] = - coeff[0],
                                              coeff[5] = - coeff[1],
                                              coeff[4] = - coeff[2],
                                              coeff[3] = 0
        */

        for(i = 0; i < 3; i++) {
            out[0] -= (int32_t)fMultDiv2(coeff[i], in[indices_1[i]]);
            out[0] += (int32_t)fMultDiv2(coeff[i], in[indices_1[5 - i]]);
        }

        for(i = 0; i < 3; i++) {
            out[1] -= (int32_t)fMultDiv2(coeff[i], in[indices_2[i]]);
            out[1] += (int32_t)fMultDiv2(coeff[i], in[indices_2[5 - i]]);
        }

        for(i = 0; i < 3; i++) {
            out[2] -= (int32_t)fMultDiv2(coeff[i], in[indices_3[i]]);
            out[2] += (int32_t)fMultDiv2(coeff[i], in[indices_3[5 - i]]);
        }

        for(j = 3; j < (len - 3); j++) {
            for(i = 0; i < 3; i++) {
                out[j] -= (int32_t)fMultDiv2(coeff[i], in[j - 3 + i]);
                out[j] += (int32_t)fMultDiv2(coeff[i], in[j + 3 - i]);
            }
        }

        for(i = 0; i < 3; i++) {
            out[len - 3] -= (int32_t)fMultDiv2(coeff[i], in[len - subtr_1[i]]);
            out[len - 3] += (int32_t)fMultDiv2(coeff[i], in[len - subtr_1[5 - i]]);
        }

        for(i = 0; i < 3; i++) {
            out[len - 2] -= (int32_t)fMultDiv2(coeff[i], in[len - subtr_2[i]]);
            out[len - 2] += (int32_t)fMultDiv2(coeff[i], in[len - subtr_2[5 - i]]);
        }

        for(i = 0; i < 3; i++) {
            out[len - 1] -= (int32_t)fMultDiv2(coeff[i], in[len - subtr_3[i]]);
            out[len - 1] += (int32_t)fMultDiv2(coeff[i], in[len - subtr_3[5 - i]]);
        }
    }
    else {
        /* exploit the symmetry of the table: coeff[6] = coeff[0],
                                              coeff[5] = coeff[1],
                                              coeff[4] = coeff[2]
        */

        for(i = 0; i < 3; i++) {
            out[0] -= (int32_t)fMultDiv2(coeff[i], in[indices_1[i]]);
            out[0] -= (int32_t)fMultDiv2(coeff[i], in[indices_1[5 - i]]);
        }
        out[0] -= (int32_t)fMultDiv2(coeff[3], in[0]);

        for(i = 0; i < 3; i++) {
            out[1] += (int32_t)fMultDiv2(coeff[i], in[indices_2[i]]);
            out[1] += (int32_t)fMultDiv2(coeff[i], in[indices_2[5 - i]]);
        }
        out[1] += (int32_t)fMultDiv2(coeff[3], in[1]);

        for(i = 0; i < 3; i++) {
            out[2] -= (int32_t)fMultDiv2(coeff[i], in[indices_3[i]]);
            out[2] -= (int32_t)fMultDiv2(coeff[i], in[indices_3[5 - i]]);
        }
        out[2] -= (int32_t)fMultDiv2(coeff[3], in[2]);

        for(j = 3; j < (len - 4); j++) {
            for(i = 0; i < 3; i++) {
                out[j] += (int32_t)fMultDiv2(coeff[i], in[j - 3 + i]);
                out[j] += (int32_t)fMultDiv2(coeff[i], in[j + 3 - i]);
            }
            out[j] += (int32_t)fMultDiv2(coeff[3], in[j]);

            j++;

            for(i = 0; i < 3; i++) {
                out[j] -= (int32_t)fMultDiv2(coeff[i], in[j - 3 + i]);
                out[j] -= (int32_t)fMultDiv2(coeff[i], in[j + 3 - i]);
            }
            out[j] -= (int32_t)fMultDiv2(coeff[3], in[j]);
        }

        for(i = 0; i < 3; i++) {
            out[len - 3] += (int32_t)fMultDiv2(coeff[i], in[len - subtr_1[i]]);
            out[len - 3] += (int32_t)fMultDiv2(coeff[i], in[len - subtr_1[5 - i]]);
        }
        out[len - 3] += (int32_t)fMultDiv2(coeff[3], in[len - 3]);

        for(i = 0; i < 3; i++) {
            out[len - 2] -= (int32_t)fMultDiv2(coeff[i], in[len - subtr_2[i]]);
            out[len - 2] -= (int32_t)fMultDiv2(coeff[i], in[len - subtr_2[5 - i]]);
        }
        out[len - 2] -= (int32_t)fMultDiv2(coeff[3], in[len - 2]);

        for(i = 0; i < 3; i++) {
            out[len - 1] += (int32_t)fMultDiv2(coeff[i], in[len - subtr_3[i]]);
            out[len - 1] += (int32_t)fMultDiv2(coeff[i], in[len - subtr_3[5 - i]]);
        }
        out[len - 1] += (int32_t)fMultDiv2(coeff[3], in[len - 1]);
    }
}

static inline void CJointStereo_GenerateMSOutput(int32_t *pSpecLCurrBand, int32_t *pSpecRCurrBand, uint32_t leftScale, uint32_t rightScale,
                                                 uint32_t nSfbBands) {
    uint32_t i;

    int32_t leftCoefficient0;
    int32_t leftCoefficient1;
    int32_t leftCoefficient2;
    int32_t leftCoefficient3;

    int32_t rightCoefficient0;
    int32_t rightCoefficient1;
    int32_t rightCoefficient2;
    int32_t rightCoefficient3;

    for(i = nSfbBands; i > 0; i -= 4) {
        leftCoefficient0 = pSpecLCurrBand[i - 4];
        leftCoefficient1 = pSpecLCurrBand[i - 3];
        leftCoefficient2 = pSpecLCurrBand[i - 2];
        leftCoefficient3 = pSpecLCurrBand[i - 1];

        rightCoefficient0 = pSpecRCurrBand[i - 4];
        rightCoefficient1 = pSpecRCurrBand[i - 3];
        rightCoefficient2 = pSpecRCurrBand[i - 2];
        rightCoefficient3 = pSpecRCurrBand[i - 1];

        /* MS output generation */
        leftCoefficient0 >>= leftScale;
        leftCoefficient1 >>= leftScale;
        leftCoefficient2 >>= leftScale;
        leftCoefficient3 >>= leftScale;

        rightCoefficient0 >>= rightScale;
        rightCoefficient1 >>= rightScale;
        rightCoefficient2 >>= rightScale;
        rightCoefficient3 >>= rightScale;

        pSpecLCurrBand[i - 4] = leftCoefficient0 + rightCoefficient0;
        pSpecLCurrBand[i - 3] = leftCoefficient1 + rightCoefficient1;
        pSpecLCurrBand[i - 2] = leftCoefficient2 + rightCoefficient2;
        pSpecLCurrBand[i - 1] = leftCoefficient3 + rightCoefficient3;

        pSpecRCurrBand[i - 4] = leftCoefficient0 - rightCoefficient0;
        pSpecRCurrBand[i - 3] = leftCoefficient1 - rightCoefficient1;
        pSpecRCurrBand[i - 2] = leftCoefficient2 - rightCoefficient2;
        pSpecRCurrBand[i - 1] = leftCoefficient3 - rightCoefficient3;
    }
}

void CJointStereo_ApplyMS(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo[2], CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo[2],
                          int32_t *spectrumL, int32_t *spectrumR, int16_t *SFBleftScale, int16_t *SFBrightScale, int16_t *specScaleL,
                          int16_t *specScaleR, const int16_t *pScaleFactorBandOffsets, const uint8_t *pWindowGroupLength, const int32_t windowGroups,
                          const int32_t max_sfb_ste_outside, const int32_t scaleFactorBandsTransmittedL, const int32_t scaleFactorBandsTransmittedR,
                          int32_t *store_dmx_re_prev, int16_t *store_dmx_re_prev_e, const int32_t mainband_flag) {
    int32_t                window, group, band;
    uint8_t                groupMask;
    CJointStereoData_t    *pJointStereoData = &pAacDecoderChannelInfo[L]->pComData->jointStereoData;
    CCplxPredictionData_t *cplxPredictionData = pAacDecoderChannelInfo[L]->pComStaticData->cplxPredictionData;

    int32_t max_sfb_ste = fMax(scaleFactorBandsTransmittedL, scaleFactorBandsTransmittedR);
    int32_t min_sfb_ste = fMin(scaleFactorBandsTransmittedL, scaleFactorBandsTransmittedR);
    int32_t scaleFactorBandsTransmitted = min_sfb_ste;

    if(pJointStereoData->cplx_pred_flag) {
        int32_t                       windowLen, groupwin, frameMaxScale;
        CJointStereoPersistentData_t *pJointStereoPersistentData = &pAacDecoderStaticChannelInfo[L]->pCpeStaticData->jointStereoPersistentData;
        int32_t *const staticSpectralCoeffsL = pAacDecoderStaticChannelInfo[L]->pCpeStaticData->jointStereoPersistentData.spectralCoeffs[L];
        int32_t *const staticSpectralCoeffsR = pAacDecoderStaticChannelInfo[L]->pCpeStaticData->jointStereoPersistentData.spectralCoeffs[R];
        int16_t *const staticSpecScaleL = pAacDecoderStaticChannelInfo[L]->pCpeStaticData->jointStereoPersistentData.specScale[L];
        int16_t *const staticSpecScaleR = pAacDecoderStaticChannelInfo[L]->pCpeStaticData->jointStereoPersistentData.specScale[R];

        int32_t *dmx_re = pAacDecoderStaticChannelInfo[L]->pCpeStaticData->jointStereoPersistentData.scratchBuffer;
        int32_t *dmx_re_prev = pAacDecoderStaticChannelInfo[L]->pCpeStaticData->jointStereoPersistentData.scratchBuffer + 1024;

        /* When MS is applied over the main band this value gets computed. Otherwise
         * (for the tiles) it uses the assigned value */
        int16_t dmx_re_prev_e = *store_dmx_re_prev_e;

        const int32_t *pCoeff;
        const int32_t *pCoeffPrev;
        int32_t        coeffPointerOffset;

        int32_t previousShape = (int32_t)pJointStereoPersistentData->winShapePrev;
        int32_t currentShape = (int32_t)pAacDecoderChannelInfo[L]->icsInfo.WindowShape;

        /* complex stereo prediction */

        /* 0. preparations */

        /* 0.0. get scratch buffer for downmix MDST */
        int32_t  _dmx_im[1024 + 8 + sizeof(int32_t) - 1];
        int32_t *dmx_im = (int32_t *)(_dmx_im + (((int32_t)8 - ((size_t)(_dmx_im)&7)) & 7));

        /* 0.1. window lengths */

        /* get length of int16_t window for current configuration */
        windowLen = pAacDecoderChannelInfo[L]->granuleLength; /* framelength 768 => 96,
                                                                 framelength 1024 => 128 */

        /* if this is no int16_t-block set length for int32_t-block */
        if(pAacDecoderChannelInfo[L]->icsInfo.WindowSequence != BLOCK_SHORT) { windowLen *= 8; }

        /* 0.2. set pointer to filter-coefficients for MDST excitation including
         * previous frame portions */
        /*      cp. ISO/IEC FDIS 23003-3:2011(E) table 125 */

        /* set pointer to default-position */
        pCoeffPrev = mdst_filt_coef_prev[previousShape];

        if(cplxPredictionData->complex_coef == 1) {
            switch(pAacDecoderChannelInfo[L]->icsInfo.WindowSequence) { /* current window sequence */
                case BLOCK_SHORT:
                case BLOCK_LONG:
                    pCoeffPrev = mdst_filt_coef_prev[previousShape];
                    break;

                case BLOCK_START:
                    if((pJointStereoPersistentData->winSeqPrev == BLOCK_SHORT) || (pJointStereoPersistentData->winSeqPrev == BLOCK_START)) {
                        /* a stop-start-sequence can only follow on an eight-int16_t-sequence
                         * or a start-sequence */
                        pCoeffPrev = mdst_filt_coef_prev[2 + previousShape];
                    }
                    else { pCoeffPrev = mdst_filt_coef_prev[previousShape]; }
                    break;

                case BLOCK_STOP:
                    pCoeffPrev = mdst_filt_coef_prev[2 + previousShape];
                    break;

                default:
                    pCoeffPrev = mdst_filt_coef_prev[previousShape];
                    break;
            }
        }

        /* 0.3. set pointer to filter-coefficients for MDST excitation */

        /* define offset of pointer to filter-coefficients for MDST exitation
         * employing only the current frame */
        if((previousShape == SHAPE_SINE) && (currentShape == SHAPE_SINE)) { coeffPointerOffset = 0; }
        else if((previousShape == SHAPE_SINE) && (currentShape == SHAPE_KBD)) { coeffPointerOffset = 2; }
        else if((previousShape == SHAPE_KBD) && (currentShape == SHAPE_KBD)) { coeffPointerOffset = 1; }
        else /* if ( (previousShape == SHAPE_KBD) && (currentShape == SHAPE_SINE)
                ) */
        {
            coeffPointerOffset = 3;
        }

        /* set pointer to filter-coefficient table cp. ISO/IEC FDIS 23003-3:2011(E)
         * table 124 */
        switch(pAacDecoderChannelInfo[L]->icsInfo.WindowSequence) { /* current window sequence */
            case BLOCK_SHORT:
            case BLOCK_LONG:
                pCoeff = mdst_filt_coef_curr[coeffPointerOffset];
                break;

            case BLOCK_START:
                if((pJointStereoPersistentData->winSeqPrev == BLOCK_SHORT) || (pJointStereoPersistentData->winSeqPrev == BLOCK_START)) {
                    /* a stop-start-sequence can only follow on an eight-int16_t-sequence or
                     * a start-sequence */
                    pCoeff = mdst_filt_coef_curr[12 + coeffPointerOffset];
                }
                else { pCoeff = mdst_filt_coef_curr[4 + coeffPointerOffset]; }
                break;

            case BLOCK_STOP:
                pCoeff = mdst_filt_coef_curr[8 + coeffPointerOffset];
                break;

            default:
                pCoeff = mdst_filt_coef_curr[coeffPointerOffset];
        }

        /* 0.4. find maximum common (l/r) band-scaling-factor for whole sequence
         * (all windows) */
        frameMaxScale = 0;
        for(window = 0, group = 0; group < windowGroups; group++) {
            for(groupwin = 0; groupwin < pWindowGroupLength[group]; groupwin++, window++) {
                int16_t *leftScale = &SFBleftScale[window * 16];
                int16_t *rightScale = &SFBrightScale[window * 16];
                int32_t  windowMaxScale = 0;

                /* find maximum scaling factor of all bands in this window */
                for(band = 0; band < min_sfb_ste; band++) {
                    int32_t lScale = leftScale[band];
                    int32_t rScale = rightScale[band];
                    int32_t commonScale = ((lScale > rScale) ? lScale : rScale);
                    windowMaxScale = (windowMaxScale < commonScale) ? commonScale : windowMaxScale;
                }
                if(scaleFactorBandsTransmittedL > min_sfb_ste) { /* i.e. scaleFactorBandsTransmittedL == max_sfb_ste
                                                                  */
                    for(; band < max_sfb_ste; band++) {
                        int32_t lScale = leftScale[band];
                        windowMaxScale = (windowMaxScale < lScale) ? lScale : windowMaxScale;
                    }
                }
                else {
                    if(scaleFactorBandsTransmittedR > min_sfb_ste) { /* i.e. scaleFactorBandsTransmittedR == max_sfb_ste
                                                                      */
                        for(; band < max_sfb_ste; band++) {
                            int32_t rScale = rightScale[band];
                            windowMaxScale = (windowMaxScale < rScale) ? rScale : windowMaxScale;
                        }
                    }
                }

                /* find maximum common SF of all windows */
                frameMaxScale = (frameMaxScale < windowMaxScale) ? windowMaxScale : frameMaxScale;
            }
        }

        /* add some headroom for overflow protection during filter and add operation
         */
        frameMaxScale += 2;

        /* process on window-basis (i.e. iterate over all groups and corresponding
         * windows) */
        for(window = 0, group = 0; group < windowGroups; group++) {
            groupMask = 1 << group;

            for(groupwin = 0; groupwin < pWindowGroupLength[group]; groupwin++, window++) {
                /* initialize the MDST with zeros */
                memset(&dmx_im[windowLen * window], 0, windowLen * sizeof(int32_t));

                /* 1. calculate the previous downmix MDCT. We do this once just for the
                 * Main band. */
                if(cplxPredictionData->complex_coef == 1) {
                    if((cplxPredictionData->use_prev_frame == 1) && (mainband_flag)) {
                        /* if this is a int32_t-block or the first window of a int16_t-block
                           calculate the downmix MDCT of the previous frame.
                           use_prev_frame is assumed not to change during a frame!
                        */

                        /* first determine shiftfactors to scale left and right channel */
                        if((pAacDecoderChannelInfo[L]->icsInfo.WindowSequence != BLOCK_SHORT) || (window == 0)) {
                            int32_t index_offset = 0;
                            int32_t srLeftChan = 0;
                            int32_t srRightChan = 0;
                            if(pAacDecoderChannelInfo[L]->icsInfo.WindowSequence == BLOCK_SHORT) {
                                /* use the last window of the previous frame for MDCT
                                 * calculation if this is a int16_t-block. */
                                index_offset = windowLen * 7;
                                if(staticSpecScaleL[7] > staticSpecScaleR[7]) {
                                    srRightChan = staticSpecScaleL[7] - staticSpecScaleR[7];
                                    dmx_re_prev_e = staticSpecScaleL[7];
                                }
                                else {
                                    srLeftChan = staticSpecScaleR[7] - staticSpecScaleL[7];
                                    dmx_re_prev_e = staticSpecScaleR[7];
                                }
                            }
                            else {
                                if(staticSpecScaleL[0] > staticSpecScaleR[0]) {
                                    srRightChan = staticSpecScaleL[0] - staticSpecScaleR[0];
                                    dmx_re_prev_e = staticSpecScaleL[0];
                                }
                                else {
                                    srLeftChan = staticSpecScaleR[0] - staticSpecScaleL[0];
                                    dmx_re_prev_e = staticSpecScaleR[0];
                                }
                            }

                            /* now scale channels and determine downmix MDCT of previous frame
                             */
                            if(pAacDecoderStaticChannelInfo[L]->pCpeStaticData->jointStereoPersistentData.clearSpectralCoeffs == 1) {
                                memset(dmx_re_prev, 0, windowLen * sizeof(int32_t));
                                dmx_re_prev_e = 0;
                            }
                            else {
                                if(cplxPredictionData->pred_dir == 0) {
                                    for(int32_t i = 0; i < windowLen; i++) {
                                        dmx_re_prev[i] = ((staticSpectralCoeffsL[index_offset + i] >> fMin(DFRACT_BITS - 1, srLeftChan + 1)) +
                                                          (staticSpectralCoeffsR[index_offset + i] >> fMin(DFRACT_BITS - 1, srRightChan + 1)));
                                    }
                                }
                                else {
                                    for(int32_t i = 0; i < windowLen; i++) {
                                        dmx_re_prev[i] = ((staticSpectralCoeffsL[index_offset + i] >> fMin(DFRACT_BITS - 1, srLeftChan + 1)) -
                                                          (staticSpectralCoeffsR[index_offset + i] >> fMin(DFRACT_BITS - 1, srRightChan + 1)));
                                    }
                                }
                            }

                            /* In case that we use INF we have to preserve the state of the
                            "dmx_re_prev" (original or computed). This is necessary because we
                            have to apply MS over the separate IGF tiles. */
                            memcpy(store_dmx_re_prev, &dmx_re_prev[0], windowLen * sizeof(int32_t));

                            /* Particular exponent of the computed/original "dmx_re_prev" must
                             * be kept for the tile MS calculations if necessary.*/
                            *store_dmx_re_prev_e = dmx_re_prev_e;

                        } /* if ( (pAacDecoderChannelInfo[L]->icsInfo.WindowSequence !=
                             BLOCK_SHORT) || (window == 0) ) */

                    } /* if ( pJointStereoData->use_prev_frame == 1 ) */

                } /* if ( pJointStereoData->complex_coef == 1 ) */

                /* 2. calculate downmix MDCT of current frame */

                /* set pointer to scale-factor-bands of current window */
                int16_t *leftScale = &SFBleftScale[window * 16];
                int16_t *rightScale = &SFBrightScale[window * 16];

                specScaleL[window] = specScaleR[window] = frameMaxScale;

                /* adapt scaling-factors to previous frame */
                if(cplxPredictionData->use_prev_frame == 1) {
                    if(window == 0) {
                        if(dmx_re_prev_e < frameMaxScale) {
                            if(mainband_flag == 0) {
                                scaleValues(dmx_re_prev, store_dmx_re_prev, windowLen, -fMin(DFRACT_BITS - 1, (frameMaxScale - dmx_re_prev_e)));
                            }
                            else { scaleValues(dmx_re_prev, windowLen, -fMin(DFRACT_BITS - 1, (frameMaxScale - dmx_re_prev_e))); }
                        }
                        else {
                            if(mainband_flag == 0) { memcpy(dmx_re_prev, store_dmx_re_prev, windowLen * sizeof(int32_t)); }
                            specScaleL[0] = dmx_re_prev_e;
                            specScaleR[0] = dmx_re_prev_e;
                        }
                    }
                    else { /* window != 0 */
                        assert(pAacDecoderChannelInfo[L]->icsInfo.WindowSequence == BLOCK_SHORT);
                        if(specScaleL[window - 1] < frameMaxScale) {
                            scaleValues(&dmx_re[windowLen * (window - 1)], windowLen,
                                        -fMin(DFRACT_BITS - 1, (frameMaxScale - specScaleL[window - 1])));
                        }
                        else {
                            specScaleL[window] = specScaleL[window - 1];
                            specScaleR[window] = specScaleR[window - 1];
                        }
                    }
                } /* if ( pJointStereoData->use_prev_frame == 1 ) */

                /* scaling factors of both channels ought to be equal now */
                assert(specScaleL[window] == specScaleR[window]);

                /* rescale signal and calculate downmix MDCT */
                for(band = 0; band < max_sfb_ste; band++) {
                    /* first adapt scaling of current band to scaling of current window =>
                     * shift signal right */
                    int32_t lScale = leftScale[band];
                    int32_t rScale = rightScale[band];

                    lScale = fMin(DFRACT_BITS - 1, specScaleL[window] - lScale);
                    rScale = fMin(DFRACT_BITS - 1, specScaleL[window] - rScale); /* L or R doesn't
                                                                                    matter,
                                                                                    specScales are
                                                                                    equal at this
                                                                                    point */

                    /* Write back to sfb scale to cover the case when max_sfb_ste <
                     * max_sfb */
                    leftScale[band] = rightScale[band] = specScaleL[window];

                    for(int32_t i = pScaleFactorBandOffsets[band]; i < pScaleFactorBandOffsets[band + 1]; i++) {
                        spectrumL[windowLen * window + i] >>= lScale;
                        spectrumR[windowLen * window + i] >>= rScale;
                    }

                    /* now calculate downmix MDCT */
                    if(pJointStereoData->MsUsed[band] & groupMask) {
                        for(int32_t i = pScaleFactorBandOffsets[band]; i < pScaleFactorBandOffsets[band + 1]; i++) {
                            dmx_re[windowLen * window + i] = spectrumL[windowLen * window + i];
                        }
                    }
                    else {
                        if(cplxPredictionData->pred_dir == 0) {
                            for(int32_t i = pScaleFactorBandOffsets[band]; i < pScaleFactorBandOffsets[band + 1]; i++) {
                                dmx_re[windowLen * window + i] = (spectrumL[windowLen * window + i] + spectrumR[windowLen * window + i]) >> 1;
                            }
                        }
                        else {
                            for(int32_t i = pScaleFactorBandOffsets[band]; i < pScaleFactorBandOffsets[band + 1]; i++) {
                                dmx_re[windowLen * window + i] = (spectrumL[windowLen * window + i] - spectrumR[windowLen * window + i]) >> 1;
                            }
                        }
                    }

                } /* for ( band=0; band<max_sfb_ste; band++ ) */
                /* Clean until the end */
                for(int32_t i = pScaleFactorBandOffsets[max_sfb_ste_outside]; i < windowLen; i++) { dmx_re[windowLen * window + i] = (int32_t)0; }

                /* 3. calculate MDST-portion corresponding to the current frame. */
                if(cplxPredictionData->complex_coef == 1) {
                    {
                        /* 3.1 move pointer in filter-coefficient table in case of int16_t
                         * window sequence */
                        /*     (other coefficients are utilized for the last 7 int16_t
                         * windows)            */
                        if((pAacDecoderChannelInfo[L]->icsInfo.WindowSequence == BLOCK_SHORT) && (window != 0)) {
                            pCoeff = mdst_filt_coef_curr[currentShape];
                            pCoeffPrev = mdst_filt_coef_prev[currentShape];
                        }

                        /* The length of the filter processing must be extended because of
                         * filter boundary problems */
                        int32_t extended_band = fMin(pScaleFactorBandOffsets[max_sfb_ste_outside] + 7, windowLen);

                        /* 3.2. estimate downmix MDST from current frame downmix MDCT */
                        if((pAacDecoderChannelInfo[L]->icsInfo.WindowSequence == BLOCK_SHORT) && (window != 0)) {
                            CJointStereo_filterAndAdd(&dmx_re[windowLen * window], extended_band, windowLen, pCoeff, &dmx_im[windowLen * window], 1);

                            CJointStereo_filterAndAdd(&dmx_re[windowLen * (window - 1)], extended_band, windowLen, pCoeffPrev,
                                                      &dmx_im[windowLen * window], 0);
                        }
                        else {
                            CJointStereo_filterAndAdd(dmx_re, extended_band, windowLen, pCoeff, dmx_im, 1);

                            if(cplxPredictionData->use_prev_frame == 1) {
                                CJointStereo_filterAndAdd(dmx_re_prev, extended_band, windowLen, pCoeffPrev, &dmx_im[windowLen * window], 0);
                            }
                        }

                    } /* if(pAacDecoderChannelInfo[L]->transform_splitting_active) */
                }     /* if ( pJointStereoData->complex_coef == 1 ) */

                /* 4. upmix process */
                int32_t pred_dir = cplxPredictionData->pred_dir ? -1 : 1;
                /* 0.1 in Q-3.34 */
                const int32_t pointOne = 0x66666666; /* 0.8 */
                /* Shift value for the downmix */
                const int32_t shift_dmx = SF_FNA_COEFFS + 1;

                for(band = 0; band < max_sfb_ste_outside; band++) {
                    if(pJointStereoData->MsUsed[band] & groupMask) {
                        int16_t tempRe = (int16_t)cplxPredictionData->alpha_q_re[group][band];
                        int16_t tempIm = (int16_t)cplxPredictionData->alpha_q_im[group][band];

                        /* Find the minimum common headroom for alpha_re and alpha_im */
                        int32_t alpha_re_headroom = CountLeadingBits((int32_t)tempRe) - 16;
                        if(tempRe == (int16_t)0) alpha_re_headroom = 15;
                        int32_t alpha_im_headroom = CountLeadingBits((int32_t)tempIm) - 16;
                        if(tempIm == (int16_t)0) alpha_im_headroom = 15;
                        int32_t val = fMin(alpha_re_headroom, alpha_im_headroom);

                        /* Multiply alpha by 0.1 with maximum precision */
                        assert(val >= 0);
                        int32_t alpha_re_tmp = fMult((int16_t)(tempRe << val), pointOne);
                        int32_t alpha_im_tmp = fMult((int16_t)(tempIm << val), pointOne);

                        /* Calculate alpha exponent */
                        /* (Q-3.34 * Q15.0) shifted left by "val" */
                        int32_t alpha_re_exp = -3 + 15 - val;

                        int32_t help3_shift = alpha_re_exp + 1;

                        int32_t *p2CoeffL = &(spectrumL[windowLen * window + pScaleFactorBandOffsets[band]]);
                        int32_t *p2CoeffR = &(spectrumR[windowLen * window + pScaleFactorBandOffsets[band]]);
                        int32_t *p2dmxIm = &(dmx_im[windowLen * window + pScaleFactorBandOffsets[band]]);
                        int32_t *p2dmxRe = &(dmx_re[windowLen * window + pScaleFactorBandOffsets[band]]);

                        for(int32_t i = pScaleFactorBandOffsets[band]; i < pScaleFactorBandOffsets[band + 1]; i++) {
                            /* Calculating helper term:
                                  side = specR[i] - alpha_re[i] * dmx_re[i] - alpha_im[i] *
                              dmx_im[i];

                              Here "dmx_re" may be the same as "specL" or alternatively keep
                              the downmix. "dmx_re" and "specL" are two different pointers
                              pointing to separate arrays, which may or may not contain the
                              same data (with different scaling).

                              specL[i] =   + (specL[i] + side);
                              specR[i] = -/+ (specL[i] - side);
                            */
                            int32_t side, left, right;

                            side = fMultAddDiv2(fMultDiv2(alpha_re_tmp, *p2dmxRe++), alpha_im_tmp, (*p2dmxIm++) << shift_dmx);
                            side = ((*p2CoeffR) >> 2) - (int32_t)SATURATE_SHIFT(side, -(help3_shift - 2), DFRACT_BITS - 2);

                            left = ((*p2CoeffL) >> 2) + side;
                            right = ((*p2CoeffL) >> 2) - side;
                            right = (int32_t)((int32_t)right * pred_dir);

                            *p2CoeffL++ = SATURATE_LEFT_SHIFT_ALT(left, 2, DFRACT_BITS);
                            *p2CoeffR++ = SATURATE_LEFT_SHIFT_ALT(right, 2, DFRACT_BITS);
                        }
                    }

                } /* for ( band=0; band < max_sfb_ste; band++ ) */
            }     /* for ( groupwin=0; groupwin<pWindowGroupLength[group]; groupwin++,
                     window++ ) */

        } /* for ( window = 0, group = 0; group < windowGroups; group++ ) */

        /* free scratch buffer */
    }
    else {
        /* MS stereo */

        for(window = 0, group = 0; group < windowGroups; group++) {
            groupMask = 1 << group;

            for(int32_t groupwin = 0; groupwin < pWindowGroupLength[group]; groupwin++, window++) {
                int32_t *leftSpectrum, *rightSpectrum;
                int16_t *leftScale = &SFBleftScale[window * 16];
                int16_t *rightScale = &SFBrightScale[window * 16];

                leftSpectrum = SPEC(spectrumL, window, pAacDecoderChannelInfo[L]->granuleLength);
                rightSpectrum = SPEC(spectrumR, window, pAacDecoderChannelInfo[R]->granuleLength);

                for(band = 0; band < max_sfb_ste_outside; band++) {
                    if(pJointStereoData->MsUsed[band] & groupMask) {
                        int32_t  lScale = leftScale[band];
                        int32_t  rScale = rightScale[band];
                        int32_t  commonScale = lScale > rScale ? lScale : rScale;
                        uint32_t offsetCurrBand, offsetNextBand;

                        /* ISO/IEC 14496-3 Chapter 4.6.8.1.1 :
                           M/S joint channel coding can only be used if common_window is 1.
                         */
                        assert(GetWindowSequence(&pAacDecoderChannelInfo[L]->icsInfo) == GetWindowSequence(&pAacDecoderChannelInfo[R]->icsInfo));
                        assert(GetWindowShape(&pAacDecoderChannelInfo[L]->icsInfo) == GetWindowShape(&pAacDecoderChannelInfo[R]->icsInfo));

                        commonScale++;
                        leftScale[band] = commonScale;
                        rightScale[band] = commonScale;

                        lScale = fMin(DFRACT_BITS - 1, commonScale - lScale);
                        rScale = fMin(DFRACT_BITS - 1, commonScale - rScale);

                        assert(lScale >= 0 && rScale >= 0);

                        offsetCurrBand = pScaleFactorBandOffsets[band];
                        offsetNextBand = pScaleFactorBandOffsets[band + 1];

                        CJointStereo_GenerateMSOutput(&(leftSpectrum[offsetCurrBand]), &(rightSpectrum[offsetCurrBand]), lScale, rScale,
                                                      offsetNextBand - offsetCurrBand);
                    }
                }
                if(scaleFactorBandsTransmittedL > scaleFactorBandsTransmitted) {
                    for(; band < scaleFactorBandsTransmittedL; band++) {
                        if(pJointStereoData->MsUsed[band] & groupMask) {
                            rightScale[band] = leftScale[band];

                            for(int32_t index = pScaleFactorBandOffsets[band]; index < pScaleFactorBandOffsets[band + 1]; index++) {
                                int32_t leftCoefficient = leftSpectrum[index];
                                /* int32_t rightCoefficient = (int32_t)0; */
                                rightSpectrum[index] = leftCoefficient;
                            }
                        }
                    }
                }
                else if(scaleFactorBandsTransmittedR > scaleFactorBandsTransmitted) {
                    for(; band < scaleFactorBandsTransmittedR; band++) {
                        if(pJointStereoData->MsUsed[band] & groupMask) {
                            leftScale[band] = rightScale[band];

                            for(int32_t index = pScaleFactorBandOffsets[band]; index < pScaleFactorBandOffsets[band + 1]; index++) {
                                /* int32_t leftCoefficient  = (int32_t)0; */
                                int32_t rightCoefficient = rightSpectrum[index];

                                leftSpectrum[index] = rightCoefficient;
                                rightSpectrum[index] = -rightCoefficient;
                            }
                        }
                    }
                }
            }
        }

        /* Reset MsUsed flags if no explicit signalling was transmitted. Necessary
           for intensity coding. PNS correlation signalling was mapped before
           calling CJointStereo_ApplyMS(). */
        if(pJointStereoData->MsMaskPresent == 2) { memset(pJointStereoData->MsUsed, 0, JointStereoMaximumBands * sizeof(uint8_t)); }
    }
}

void CJointStereo_ApplyIS(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo[2], const int16_t *pScaleFactorBandOffsets,
                          const uint8_t *pWindowGroupLength, const int32_t windowGroups, const int32_t scaleFactorBandsTransmitted) {
    CJointStereoData_t *pJointStereoData = &pAacDecoderChannelInfo[L]->pComData->jointStereoData;

    for(int32_t window = 0, group = 0; group < windowGroups; group++) {
        uint8_t *CodeBook;
        int16_t *ScaleFactor;
        uint8_t  groupMask = 1 << group;

        CodeBook = &pAacDecoderChannelInfo[R]->pDynData->aCodeBook[group * 16];
        ScaleFactor = &pAacDecoderChannelInfo[R]->pDynData->aScaleFactor[group * 16];

        for(int32_t groupwin = 0; groupwin < pWindowGroupLength[group]; groupwin++, window++) {
            int32_t *leftSpectrum, *rightSpectrum;
            int16_t *leftScale = &pAacDecoderChannelInfo[L]->pDynData->aSfbScale[window * 16];
            int16_t *rightScale = &pAacDecoderChannelInfo[R]->pDynData->aSfbScale[window * 16];
            int32_t  band;

            leftSpectrum = SPEC(pAacDecoderChannelInfo[L]->pSpectralCoefficient, window, pAacDecoderChannelInfo[L]->granuleLength);
            rightSpectrum = SPEC(pAacDecoderChannelInfo[R]->pSpectralCoefficient, window, pAacDecoderChannelInfo[R]->granuleLength);

            for(band = 0; band < scaleFactorBandsTransmitted; band++) {
                if((CodeBook[band] == INTENSITY_HCB) || (CodeBook[band] == INTENSITY_HCB2)) {
                    int32_t bandScale = -(ScaleFactor[band] + 100);

                    int32_t msb = bandScale >> 2;
                    int32_t lsb = bandScale & 0x03;

                    /* exponent of MantissaTable[lsb][0] is 1, thus msb+1 below. */
                    int32_t scale = MantissaTable[lsb][0];

                    /* ISO/IEC 14496-3 Chapter 4.6.8.2.3 :
                       The use of intensity stereo coding is signaled by the use of the
                       pseudo codebooks INTENSITY_HCB and INTENSITY_HCB2 (15 and 14) only
                       in the right channel of a channel_pair_element() having a common
                       ics_info() (common_window == 1). */
                    assert(GetWindowSequence(&pAacDecoderChannelInfo[L]->icsInfo) == GetWindowSequence(&pAacDecoderChannelInfo[R]->icsInfo));
                    assert(GetWindowShape(&pAacDecoderChannelInfo[L]->icsInfo) == GetWindowShape(&pAacDecoderChannelInfo[R]->icsInfo));

                    rightScale[band] = leftScale[band] + msb + 1;

                    if(pJointStereoData->MsUsed[band] & groupMask) {
                        if(CodeBook[band] == INTENSITY_HCB) /* _NOT_ in-phase */
                        {
                            scale = -scale;
                        }
                    }
                    else {
                        if(CodeBook[band] == INTENSITY_HCB2) /* out-of-phase */
                        {
                            scale = -scale;
                        }
                    }

                    for(int32_t index = pScaleFactorBandOffsets[band]; index < pScaleFactorBandOffsets[band + 1]; index++) {
                        rightSpectrum[index] = fMult(leftSpectrum[index], scale);
                    }
                }
            }
        }
    }
}
