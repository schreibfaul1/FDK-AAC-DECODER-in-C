

/******************* MPEG transport format decoder library *********************

   Author(s):   Daniel Homm

   Description:

*******************************************************************************/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/common_fix.h"
#include "tp_data.h"
#include "tpdec_lib.h"

/**
 * The following arrays provide the IDs of the consecutive elements for each
 * channel configuration. Every channel_configuration has to be finalized with
 * ID_NONE.
 */
static const MP4_ELEMENT_ID_t channel_configuration_0[] = {ID_NONE};
static const MP4_ELEMENT_ID_t channel_configuration_1[] = {ID_SCE, ID_NONE};
static const MP4_ELEMENT_ID_t channel_configuration_2[] = {ID_CPE, ID_NONE};
static const MP4_ELEMENT_ID_t channel_configuration_3[] = {ID_SCE, ID_CPE, ID_NONE};
static const MP4_ELEMENT_ID_t channel_configuration_4[] = {ID_SCE, ID_CPE, ID_SCE, ID_NONE};
static const MP4_ELEMENT_ID_t channel_configuration_5[] = {ID_SCE, ID_CPE, ID_CPE, ID_NONE};
static const MP4_ELEMENT_ID_t channel_configuration_6[] = {ID_SCE, ID_CPE, ID_CPE, ID_LFE, ID_NONE};
static const MP4_ELEMENT_ID_t channel_configuration_7[] = {ID_SCE, ID_CPE, ID_CPE, ID_CPE, ID_LFE, ID_NONE};
static const MP4_ELEMENT_ID_t channel_configuration_8[] = {ID_NONE};  /* reserved */
static const MP4_ELEMENT_ID_t channel_configuration_9[] = {ID_NONE};  /* reserved */
static const MP4_ELEMENT_ID_t channel_configuration_10[] = {ID_NONE}; /* reserved */
static const MP4_ELEMENT_ID_t channel_configuration_11[] = {ID_SCE, ID_CPE, ID_CPE, ID_SCE, ID_LFE, ID_NONE};
static const MP4_ELEMENT_ID_t channel_configuration_12[] = {ID_SCE, ID_CPE, ID_CPE, ID_CPE, ID_LFE, ID_NONE};
static const MP4_ELEMENT_ID_t channel_configuration_13[] = {ID_SCE, ID_CPE, ID_CPE, ID_CPE, ID_CPE, ID_SCE,
                                                          ID_LFE, ID_LFE, ID_SCE, ID_CPE, ID_CPE, ID_SCE,
                                                          ID_CPE, ID_SCE, ID_SCE, ID_CPE, ID_NONE};
static const MP4_ELEMENT_ID_t channel_configuration_14[] = {ID_SCE, ID_CPE, ID_CPE, ID_LFE, ID_CPE, ID_NONE};

static const MP4_ELEMENT_ID_t *channel_configuration_array[] = {
    channel_configuration_0,  channel_configuration_1,  channel_configuration_2,  channel_configuration_3,
    channel_configuration_4,  channel_configuration_5,  channel_configuration_6,  channel_configuration_7,
    channel_configuration_8,  channel_configuration_9,  channel_configuration_10, channel_configuration_11,
    channel_configuration_12, channel_configuration_13, channel_configuration_14};

#define TP_USAC_MAX_CHANNEL_CONFIGURATION_INDEX (13)
#define SC_CHANNEL_CONFIG_TAB_SIZE              (TP_USAC_MAX_CHANNEL_CONFIGURATION_INDEX + 1)

/* channel config structure used for sanity check */
typedef struct
{
    int8_t nCh;  /* number of channels */
    int8_t nSCE; /* number of SCE's */
    int8_t nCPE; /* number of CPE's */
    int8_t nLFE; /* number of LFE's */
} SC_CHANNEL_CONFIG;

static const SC_CHANNEL_CONFIG sc_chan_config_tab[SC_CHANNEL_CONFIG_TAB_SIZE] = {
    /* nCh, nSCE, nCPE, nLFE,     cci */
    {0, 0, 0, 0}, /*  0 */
    {1, 1, 0, 0}, /*  1 */
    {2, 0, 1, 0}, /*  2 */
    {3, 1, 1, 0}, /*  3 */
    {4, 2, 1, 0}, /*  4 */
    {5, 1, 2, 0}, /*  5 */
    {6, 1, 2, 1}, /*  6 */
    {8, 1, 3, 1}, /*  7 */
    {2, 2, 0, 0}, /*  8 */
    {3, 1, 1, 0}, /*  9 */
    {4, 0, 2, 0}, /* 10 */
    {7, 2, 2, 1}, /* 11 */
    {8, 1, 3, 1}, /* 12 */
    {24, 6, 8, 2} /* 13 */
};

void CProgramConfig_Reset(CProgramConfig_t *pPce) { pPce->elCounter = 0; }

void CProgramConfig_Init(CProgramConfig_t *pPce) {
    memset(pPce, 0, sizeof(CProgramConfig_t));
    pPce->SamplingFrequencyIndex = 0xf;
}

int32_t CProgramConfig_IsValid(const CProgramConfig_t *pPce) { return ((pPce->isValid) ? 1 : 0); }

#define PCE_HEIGHT_EXT_SYNC (0xAC)

/*
 * Read the extension for height info.
 * return 0 if successfull,
 *       -1 if the CRC failed,
 *       -2 if invalid HeightInfo.
 */
static int32_t CProgramConfig_ReadHeightExt(CProgramConfig_t *pPce, HANDLE_FDK_BITSTREAM bs,
                                            int32_t *const bytesAvailable, const uint32_t alignmentAnchor) {
    int32_t     err = 0;
    FDK_CRCINFO crcInfo; /* CRC state info */
    int32_t     crcReg;
    FDKcrcInit(&crcInfo, 0x07, 0xFF, 8);
    crcReg = FDKcrcStartReg(&crcInfo, bs, 0);
    uint32_t startAnchor = FDKgetValidBits(bs);

    assert(pPce != NULL);
    assert(bs != NULL);
    assert(bytesAvailable != NULL);

    if((startAnchor >= 24) && (*bytesAvailable >= 3) && (FDKreadBits(bs, 8) == PCE_HEIGHT_EXT_SYNC)) {
        int32_t i;

        for(i = 0; i < pPce->NumFrontChannelElements; i++) {
            if((pPce->FrontElementHeightInfo[i] = (uint8_t)FDKreadBits(bs, 2)) >= PC_NUM_HEIGHT_LAYER) {
                err = -2; /* height information is out of the valid range */
            }
        }
        for(i = 0; i < pPce->NumSideChannelElements; i++) {
            if((pPce->SideElementHeightInfo[i] = (uint8_t)FDKreadBits(bs, 2)) >= PC_NUM_HEIGHT_LAYER) {
                err = -2; /* height information is out of the valid range */
            }
        }
        for(i = 0; i < pPce->NumBackChannelElements; i++) {
            if((pPce->BackElementHeightInfo[i] = (uint8_t)FDKreadBits(bs, 2)) >= PC_NUM_HEIGHT_LAYER) {
                err = -2; /* height information is out of the valid range */
            }
        }
        FDKbyteAlign(bs, alignmentAnchor);

        FDKcrcEndReg(&crcInfo, bs, crcReg);
        if((uint16_t)FDKreadBits(bs, 8) != FDKcrcGetCRC(&crcInfo)) {
            /* CRC failed */
            err = -1;
        }
        if(err != 0) {
            /* Reset whole height information in case an error occured during parsing.
               The return value ensures that pPce->isValid is set to 0 and implicit
               channel mapping is used. */
            memset(pPce->FrontElementHeightInfo, 0, sizeof(pPce->FrontElementHeightInfo));
            memset(pPce->SideElementHeightInfo, 0, sizeof(pPce->SideElementHeightInfo));
            memset(pPce->BackElementHeightInfo, 0, sizeof(pPce->BackElementHeightInfo));
        }
    }
    else {
        /* No valid extension data found -> restore the initial bitbuffer state */
        FDKpushBack(bs, (int32_t)startAnchor - (int32_t)FDKgetValidBits(bs));
    }

    /* Always report the bytes read. */
    *bytesAvailable -= ((int32_t)startAnchor - (int32_t)FDKgetValidBits(bs)) >> 3;

    return (err);
}

void CProgramConfig_Read(CProgramConfig_t *pPce, HANDLE_FDK_BITSTREAM bs, uint32_t alignmentAnchor) {
    int32_t i, err = 0;
    int32_t commentBytes;

    pPce->NumEffectiveChannels = 0;
    pPce->NumChannels = 0;
    pPce->ElementInstanceTag = (uint8_t)FDKreadBits(bs, 4);
    pPce->Profile = (uint8_t)FDKreadBits(bs, 2);
    pPce->SamplingFrequencyIndex = (uint8_t)FDKreadBits(bs, 4);
    pPce->NumFrontChannelElements = (uint8_t)FDKreadBits(bs, 4);
    pPce->NumSideChannelElements = (uint8_t)FDKreadBits(bs, 4);
    pPce->NumBackChannelElements = (uint8_t)FDKreadBits(bs, 4);
    pPce->NumLfeChannelElements = (uint8_t)FDKreadBits(bs, 2);
    pPce->NumAssocDataElements = (uint8_t)FDKreadBits(bs, 3);
    pPce->NumValidCcElements = (uint8_t)FDKreadBits(bs, 4);

    if((pPce->MonoMixdownPresent = (uint8_t)FDKreadBits(bs, 1)) != 0) {
        pPce->MonoMixdownElementNumber = (uint8_t)FDKreadBits(bs, 4);
    }

    if((pPce->StereoMixdownPresent = (uint8_t)FDKreadBits(bs, 1)) != 0) {
        pPce->StereoMixdownElementNumber = (uint8_t)FDKreadBits(bs, 4);
    }

    if((pPce->MatrixMixdownIndexPresent = (uint8_t)FDKreadBits(bs, 1)) != 0) {
        pPce->MatrixMixdownIndex = (uint8_t)FDKreadBits(bs, 2);
        pPce->PseudoSurroundEnable = (uint8_t)FDKreadBits(bs, 1);
    }

    for(i = 0; i < pPce->NumFrontChannelElements; i++) {
        pPce->FrontElementIsCpe[i] = (uint8_t)FDKreadBits(bs, 1);
        pPce->FrontElementTagSelect[i] = (uint8_t)FDKreadBits(bs, 4);
        pPce->NumChannels += pPce->FrontElementIsCpe[i] ? 2 : 1;
    }

    for(i = 0; i < pPce->NumSideChannelElements; i++) {
        pPce->SideElementIsCpe[i] = (uint8_t)FDKreadBits(bs, 1);
        pPce->SideElementTagSelect[i] = (uint8_t)FDKreadBits(bs, 4);
        pPce->NumChannels += pPce->SideElementIsCpe[i] ? 2 : 1;
    }

    for(i = 0; i < pPce->NumBackChannelElements; i++) {
        pPce->BackElementIsCpe[i] = (uint8_t)FDKreadBits(bs, 1);
        pPce->BackElementTagSelect[i] = (uint8_t)FDKreadBits(bs, 4);
        pPce->NumChannels += pPce->BackElementIsCpe[i] ? 2 : 1;
    }

    pPce->NumEffectiveChannels = pPce->NumChannels;

    for(i = 0; i < pPce->NumLfeChannelElements; i++) {
        pPce->LfeElementTagSelect[i] = (uint8_t)FDKreadBits(bs, 4);
        pPce->NumChannels += 1;
    }

    for(i = 0; i < pPce->NumAssocDataElements; i++) {
        pPce->AssocDataElementTagSelect[i] = (uint8_t)FDKreadBits(bs, 4);
    }

    for(i = 0; i < pPce->NumValidCcElements; i++) {
        pPce->CcElementIsIndSw[i] = (uint8_t)FDKreadBits(bs, 1);
        pPce->ValidCcElementTagSelect[i] = (uint8_t)FDKreadBits(bs, 4);
    }

    FDKbyteAlign(bs, alignmentAnchor);

    pPce->CommentFieldBytes = (uint8_t)FDKreadBits(bs, 8);
    commentBytes = pPce->CommentFieldBytes;

    /* Search for height info extension and read it if available */
    err = CProgramConfig_ReadHeightExt(pPce, bs, &commentBytes, alignmentAnchor);

    for(i = 0; i < commentBytes; i++) {
        uint8_t text;

        text = (uint8_t)FDKreadBits(bs, 8);

        if(i < PC_COMMENTLENGTH) { pPce->Comment[i] = text; }
    }

    pPce->isValid = (err) ? 0 : 1;
}

/*
 * Compare two program configurations.
 * Returns the result of the comparison:
 *  -1 - completely different
 *   0 - completely equal
 *   1 - different but same channel configuration
 *   2 - different channel configuration but same number of channels
 */
int32_t CProgramConfig_Compare(const CProgramConfig_t *const pPce1, const CProgramConfig_t *const pPce2) {
    int32_t result = 0; /* Innocent until proven false. */

    if(memcmp(pPce1, pPce2, sizeof(CProgramConfig_t)) !=
       0) { /* Configurations are not completely equal.
               So look into details and analyse the channel configurations: */
        result = -1;

        if(pPce1->NumChannels == pPce2->NumChannels) { /* Now the logic changes. We first assume to have
                                                          the same channel configuration and then prove
                                                          if this assumption is true. */
            result = 1;

            /* Front channels */
            if(pPce1->NumFrontChannelElements != pPce2->NumFrontChannelElements) {
                result = 2; /* different number of front channel elements */
            }
            else {
                int32_t el, numCh1 = 0, numCh2 = 0;
                for(el = 0; el < pPce1->NumFrontChannelElements; el += 1) {
                    if(pPce1->FrontElementHeightInfo[el] != pPce2->FrontElementHeightInfo[el]) {
                        result = 2; /* different height info */
                        break;
                    }
                    numCh1 += pPce1->FrontElementIsCpe[el] ? 2 : 1;
                    numCh2 += pPce2->FrontElementIsCpe[el] ? 2 : 1;
                }
                if(numCh1 != numCh2) { result = 2; /* different number of front channels */ }
            }
            /* Side channels */
            if(pPce1->NumSideChannelElements != pPce2->NumSideChannelElements) {
                result = 2; /* different number of side channel elements */
            }
            else {
                int32_t el, numCh1 = 0, numCh2 = 0;
                for(el = 0; el < pPce1->NumSideChannelElements; el += 1) {
                    if(pPce1->SideElementHeightInfo[el] != pPce2->SideElementHeightInfo[el]) {
                        result = 2; /* different height info */
                        break;
                    }
                    numCh1 += pPce1->SideElementIsCpe[el] ? 2 : 1;
                    numCh2 += pPce2->SideElementIsCpe[el] ? 2 : 1;
                }
                if(numCh1 != numCh2) { result = 2; /* different number of side channels */ }
            }
            /* Back channels */
            if(pPce1->NumBackChannelElements != pPce2->NumBackChannelElements) {
                result = 2; /* different number of back channel elements */
            }
            else {
                int32_t el, numCh1 = 0, numCh2 = 0;
                for(el = 0; el < pPce1->NumBackChannelElements; el += 1) {
                    if(pPce1->BackElementHeightInfo[el] != pPce2->BackElementHeightInfo[el]) {
                        result = 2; /* different height info */
                        break;
                    }
                    numCh1 += pPce1->BackElementIsCpe[el] ? 2 : 1;
                    numCh2 += pPce2->BackElementIsCpe[el] ? 2 : 1;
                }
                if(numCh1 != numCh2) { result = 2; /* different number of back channels */ }
            }
            /* LFE channels */
            if(pPce1->NumLfeChannelElements != pPce2->NumLfeChannelElements) {
                result = 2; /* different number of lfe channels */
            }
            /* LFEs are always SCEs so we don't need to count the channels. */
        }
    }

    return result;
}

void CProgramConfig_GetDefault(CProgramConfig_t *pPce, const uint32_t channelConfig) {
    assert(pPce != NULL);

    /* Init PCE */
    CProgramConfig_Init(pPce);
    pPce->Profile = 1; /* Set AAC LC because it is the only supported object type. */

    switch(channelConfig) {
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        case 32: /* 7.1 side channel configuration as defined
            pPce->NumFrontChannelElements = 2;
            pPce->FrontElementIsCpe[0] = 0;
            pPce->FrontElementIsCpe[1] = 1;
            pPce->NumSideChannelElements = 1;
            pPce->SideElementIsCpe[0] = 1;
            pPce->NumBackChannelElements = 1;
            pPce->BackElementIsCpe[0] = 1;
            pPce->NumLfeChannelElements = 1;
            pPce->NumChannels = 8;
            pPce->NumEffectiveChannels = 7;
            pPce->isValid = 1;
            break;
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        case 12: /* 3/0/4.1ch surround back */
            pPce->BackElementIsCpe[1] = 1;
            pPce->NumChannels += 1;
            pPce->NumEffectiveChannels += 1;
            /* fall through */;
        case 11: /* 3/0/3.1ch */
            pPce->NumFrontChannelElements += 2;
            pPce->FrontElementIsCpe[0] = 0;
            pPce->FrontElementIsCpe[1] = 1;
            pPce->NumBackChannelElements += 2;
            pPce->BackElementIsCpe[0] = 1;
            pPce->BackElementIsCpe[1] += 0;
            pPce->NumLfeChannelElements += 1;
            pPce->NumChannels += 7;
            pPce->NumEffectiveChannels += 6;
            pPce->isValid = 1;
            break;
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        case 14:                                 /* 2/0/0-3/0/2-0.1ch front height */
            pPce->FrontElementHeightInfo[2] = 1; /* Top speaker */
            /* fall through */;
        case 7: /* 5/0/2.1ch front */
            pPce->NumFrontChannelElements += 1;
            pPce->FrontElementIsCpe[2] = 1;
            pPce->NumChannels += 2;
            pPce->NumEffectiveChannels += 2;
            /* fall through */;
        case 6: /* 3/0/2.1ch */
            pPce->NumLfeChannelElements += 1;
            pPce->NumChannels += 1;
            /* fall through */;
        case 5: /* 3/0/2.0ch */
        case 4: /* 3/0/1.0ch */
            pPce->NumBackChannelElements += 1;
            pPce->BackElementIsCpe[0] = (channelConfig > 4) ? 1 : 0;
            pPce->NumChannels += (channelConfig > 4) ? 2 : 1;
            pPce->NumEffectiveChannels += (channelConfig > 4) ? 2 : 1;
            /* fall through */;
        case 3: /* 3/0/0.0ch */
            pPce->NumFrontChannelElements += 1;
            pPce->FrontElementIsCpe[1] = 1;
            pPce->NumChannels += 2;
            pPce->NumEffectiveChannels += 2;
            /* fall through */;
        case 1: /* 1/0/0.0ch */
            pPce->NumFrontChannelElements += 1;
            pPce->FrontElementIsCpe[0] = 0;
            pPce->NumChannels += 1;
            pPce->NumEffectiveChannels += 1;
            pPce->isValid = 1;
            break;
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        case 2: /* 2/0/0.ch */
            pPce->NumFrontChannelElements = 1;
            pPce->FrontElementIsCpe[0] = 1;
            pPce->NumChannels += 2;
            pPce->NumEffectiveChannels += 2;
            pPce->isValid = 1;
            break;
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        default:
            pPce->isValid = 0; /* To be explicit! */
            break;
    }

    if(pPce->isValid) {
        /* Create valid element instance tags */
        int32_t el, elTagSce = 0, elTagCpe = 0;

        for(el = 0; el < pPce->NumFrontChannelElements; el += 1) {
            pPce->FrontElementTagSelect[el] = (pPce->FrontElementIsCpe[el]) ? elTagCpe++ : elTagSce++;
        }
        for(el = 0; el < pPce->NumSideChannelElements; el += 1) {
            pPce->SideElementTagSelect[el] = (pPce->SideElementIsCpe[el]) ? elTagCpe++ : elTagSce++;
        }
        for(el = 0; el < pPce->NumBackChannelElements; el += 1) {
            pPce->BackElementTagSelect[el] = (pPce->BackElementIsCpe[el]) ? elTagCpe++ : elTagSce++;
        }
        elTagSce = 0;
        for(el = 0; el < pPce->NumLfeChannelElements; el += 1) { pPce->LfeElementTagSelect[el] = elTagSce++; }
    }
}

/**
 * \brief get implicit audio channel type for given channelConfig and MPEG
 * ordered channel index
 * \param channelConfig MPEG channelConfiguration from 1 upto 14
 * \param index MPEG channel order index
 * \return audio channel type.
 */
static void getImplicitAudioChannelTypeAndIndex(AUDIO_CHANNEL_TYPE_t *chType, uint8_t *chIndex, uint32_t channelConfig,
                                                uint32_t index) {
    if(index < 3) {
        *chType = ACT_FRONT;
        *chIndex = index;
    }
    else {
        switch(channelConfig) {
            case 4: /* SCE, CPE, SCE */
            case 5: /* SCE, CPE, CPE */
            case 6: /* SCE, CPE, CPE, LFE */
                switch(index) {
                    case 3:
                    case 4:
                        *chType = ACT_BACK;
                        *chIndex = index - 3;
                        break;
                    case 5:
                        *chType = ACT_LFE;
                        *chIndex = 0;
                        break;
                }
                break;
            case 7: /* SCE,CPE,CPE,CPE,LFE */
                switch(index) {
                    case 3:
                    case 4:
                        *chType = ACT_FRONT;
                        *chIndex = index;
                        break;
                    case 5:
                    case 6:
                        *chType = ACT_BACK;
                        *chIndex = index - 5;
                        break;
                    case 7:
                        *chType = ACT_LFE;
                        *chIndex = 0;
                        break;
                }
                break;
            case 11: /* SCE,CPE,CPE,SCE,LFE */
                if(index < 6) {
                    *chType = ACT_BACK;
                    *chIndex = index - 3;
                }
                else {
                    *chType = ACT_LFE;
                    *chIndex = 0;
                }
                break;
            case 12: /* SCE,CPE,CPE,CPE,LFE */
                if(index < 7) {
                    *chType = ACT_BACK;
                    *chIndex = index - 3;
                }
                else {
                    *chType = ACT_LFE;
                    *chIndex = 0;
                }
                break;
            case 14: /* SCE,CPE,CPE,LFE,CPE */
                switch(index) {
                    case 3:
                    case 4:
                        *chType = ACT_BACK;
                        *chIndex = index - 3;
                        break;
                    case 5:
                        *chType = ACT_LFE;
                        *chIndex = 0;
                        break;
                    case 6:
                    case 7:
                        *chType = ACT_FRONT_TOP;
                        *chIndex = index - 6; /* handle the top layer independently */
                        break;
                }
                break;
            default:
                *chType = ACT_NONE;
                break;
        }
    }
}

int32_t CProgramConfig_LookupElement(CProgramConfig_t *pPce, uint32_t channelConfig, const uint32_t tag,
                                     const uint32_t channelIdx, uint8_t chMapping[], AUDIO_CHANNEL_TYPE_t chType[],
                                     uint8_t chIndex[], const uint32_t chDescrLen, uint8_t *elMapping,
                                     MP4_ELEMENT_ID_t elList[], MP4_ELEMENT_ID_t elType) {
    if(channelConfig > 0) {
        /* Constant channel mapping must have
           been set during initialization. */
        if(IS_CHANNEL_ELEMENT(elType)) {
            *elMapping = pPce->elCounter;
            if(elList[pPce->elCounter] != elType && !IS_USAC_CHANNEL_ELEMENT(elType)) {
                /* Not in the list */
                if((channelConfig == 2) && (elType == ID_SCE)) { /* This scenario occurs with HE-AAC v2 streams
                                                                    of buggy encoders. In other decoder
                                                                    implementations decoding of this kind of
                                                                    streams is desired. */
                    channelConfig = 1;
                }
                else if((elList[pPce->elCounter] == ID_LFE) &&
                        (elType == ID_SCE)) { /* Decode bitstreams which wrongly use ID_SCE
                                                 instead of ID_LFE element type. */
                    ;
                }
                else { return 0; }
            }
            /* Assume all front channels */
            getImplicitAudioChannelTypeAndIndex(&chType[channelIdx], &chIndex[channelIdx], channelConfig, channelIdx);
            if(elType == ID_CPE || elType == ID_USAC_CPE) {
                chType[channelIdx + 1] = chType[channelIdx];
                chIndex[channelIdx + 1] = chIndex[channelIdx] + 1;
            }
            pPce->elCounter++;
        }
        /* Accept all non-channel elements, too. */
        return 1;
    }
    else {
        if((!pPce->isValid) || (pPce->NumChannels > chDescrLen)) {
            /* Implicit channel mapping. */
            if(IS_USAC_CHANNEL_ELEMENT(elType)) { *elMapping = pPce->elCounter++; }
            else if(IS_MP4_CHANNEL_ELEMENT(elType)) {
                /* Store all channel element IDs */
                elList[pPce->elCounter] = elType;
                *elMapping = pPce->elCounter++;
            }
        }
        else {
            /* Accept the additional channel(s), only if the tag is in the lists */
            int32_t isCpe = 0, i;
            /* Element counter */
            int32_t ec[PC_NUM_HEIGHT_LAYER] = {0};
            /* Channel counters */
            int32_t cc[PC_NUM_HEIGHT_LAYER] = {0};
            int32_t fc[PC_NUM_HEIGHT_LAYER] = {0}; /* front channel counter */
            int32_t sc[PC_NUM_HEIGHT_LAYER] = {0}; /* side channel counter */
            int32_t bc[PC_NUM_HEIGHT_LAYER] = {0}; /* back channel counter */
            int32_t lc = 0;                        /* lfe channel counter */

            /* General MPEG (PCE) composition rules:
               - Over all:
                   <normal height channels><top height channels><bottom height
               channels>
               - Within each height layer:
                   <front channels><side channels><back channels>
               - Exception:
                   The LFE channels have no height info and thus they are arranged at
               the very end of the normal height layer channels.
             */

            switch(elType) {
                case ID_CPE:
                    isCpe = 1;
                    /* fall through */;
                case ID_SCE:
                    /* search in front channels */
                    for(i = 0; i < pPce->NumFrontChannelElements; i++) {
                        int32_t heightLayer = pPce->FrontElementHeightInfo[i];
                        if(isCpe == pPce->FrontElementIsCpe[i] && pPce->FrontElementTagSelect[i] == tag) {
                            int32_t            h, elIdx = ec[heightLayer], chIdx = cc[heightLayer];
                            AUDIO_CHANNEL_TYPE_t aChType = (AUDIO_CHANNEL_TYPE_t)((heightLayer << 4) | ACT_FRONT);
                            for(h = heightLayer - 1; h >= 0; h -= 1) {
                                int32_t el;
                                /* Count front channels/elements */
                                for(el = 0; el < pPce->NumFrontChannelElements; el += 1) {
                                    if(pPce->FrontElementHeightInfo[el] == h) {
                                        elIdx += 1;
                                        chIdx += (pPce->FrontElementIsCpe[el]) ? 2 : 1;
                                    }
                                }
                                /* Count side channels/elements */
                                for(el = 0; el < pPce->NumSideChannelElements; el += 1) {
                                    if(pPce->SideElementHeightInfo[el] == h) {
                                        elIdx += 1;
                                        chIdx += (pPce->SideElementIsCpe[el]) ? 2 : 1;
                                    }
                                }
                                /* Count back channels/elements */
                                for(el = 0; el < pPce->NumBackChannelElements; el += 1) {
                                    if(pPce->BackElementHeightInfo[el] == h) {
                                        elIdx += 1;
                                        chIdx += (pPce->BackElementIsCpe[el]) ? 2 : 1;
                                    }
                                }
                                if(h == 0) { /* normal height */
                                    elIdx += pPce->NumLfeChannelElements;
                                    chIdx += pPce->NumLfeChannelElements;
                                }
                            }
                            chMapping[chIdx] = channelIdx;
                            chType[chIdx] = aChType;
                            chIndex[chIdx] = fc[heightLayer];
                            if(isCpe) {
                                chMapping[chIdx + 1] = channelIdx + 1;
                                chType[chIdx + 1] = aChType;
                                chIndex[chIdx + 1] = fc[heightLayer] + 1;
                            }
                            *elMapping = elIdx;
                            return 1;
                        }
                        ec[heightLayer] += 1;
                        if(pPce->FrontElementIsCpe[i]) {
                            cc[heightLayer] += 2;
                            fc[heightLayer] += 2;
                        }
                        else {
                            cc[heightLayer] += 1;
                            fc[heightLayer] += 1;
                        }
                    }
                    /* search in side channels */
                    for(i = 0; i < pPce->NumSideChannelElements; i++) {
                        int32_t heightLayer = pPce->SideElementHeightInfo[i];
                        if(isCpe == pPce->SideElementIsCpe[i] && pPce->SideElementTagSelect[i] == tag) {
                            int32_t            h, elIdx = ec[heightLayer], chIdx = cc[heightLayer];
                            AUDIO_CHANNEL_TYPE_t aChType = (AUDIO_CHANNEL_TYPE_t)((heightLayer << 4) | ACT_SIDE);
                            for(h = heightLayer - 1; h >= 0; h -= 1) {
                                int32_t el;
                                /* Count front channels/elements */
                                for(el = 0; el < pPce->NumFrontChannelElements; el += 1) {
                                    if(pPce->FrontElementHeightInfo[el] == h) {
                                        elIdx += 1;
                                        chIdx += (pPce->FrontElementIsCpe[el]) ? 2 : 1;
                                    }
                                }
                                /* Count side channels/elements */
                                for(el = 0; el < pPce->NumSideChannelElements; el += 1) {
                                    if(pPce->SideElementHeightInfo[el] == h) {
                                        elIdx += 1;
                                        chIdx += (pPce->SideElementIsCpe[el]) ? 2 : 1;
                                    }
                                }
                                /* Count back channels/elements */
                                for(el = 0; el < pPce->NumBackChannelElements; el += 1) {
                                    if(pPce->BackElementHeightInfo[el] == h) {
                                        elIdx += 1;
                                        chIdx += (pPce->BackElementIsCpe[el]) ? 2 : 1;
                                    }
                                }
                                if(h == 0) { /* LFE channels belong to the normal height layer */
                                    elIdx += pPce->NumLfeChannelElements;
                                    chIdx += pPce->NumLfeChannelElements;
                                }
                            }
                            chMapping[chIdx] = channelIdx;
                            chType[chIdx] = aChType;
                            chIndex[chIdx] = sc[heightLayer];
                            if(isCpe) {
                                chMapping[chIdx + 1] = channelIdx + 1;
                                chType[chIdx + 1] = aChType;
                                chIndex[chIdx + 1] = sc[heightLayer] + 1;
                            }
                            *elMapping = elIdx;
                            return 1;
                        }
                        ec[heightLayer] += 1;
                        if(pPce->SideElementIsCpe[i]) {
                            cc[heightLayer] += 2;
                            sc[heightLayer] += 2;
                        }
                        else {
                            cc[heightLayer] += 1;
                            sc[heightLayer] += 1;
                        }
                    }
                    /* search in back channels */
                    for(i = 0; i < pPce->NumBackChannelElements; i++) {
                        int32_t heightLayer = pPce->BackElementHeightInfo[i];
                        if(isCpe == pPce->BackElementIsCpe[i] && pPce->BackElementTagSelect[i] == tag) {
                            int32_t            h, elIdx = ec[heightLayer], chIdx = cc[heightLayer];
                            AUDIO_CHANNEL_TYPE_t aChType = (AUDIO_CHANNEL_TYPE_t)((heightLayer << 4) | ACT_BACK);
                            for(h = heightLayer - 1; h >= 0; h -= 1) {
                                int32_t el;
                                /* Count front channels/elements */
                                for(el = 0; el < pPce->NumFrontChannelElements; el += 1) {
                                    if(pPce->FrontElementHeightInfo[el] == h) {
                                        elIdx += 1;
                                        chIdx += (pPce->FrontElementIsCpe[el]) ? 2 : 1;
                                    }
                                }
                                /* Count side channels/elements */
                                for(el = 0; el < pPce->NumSideChannelElements; el += 1) {
                                    if(pPce->SideElementHeightInfo[el] == h) {
                                        elIdx += 1;
                                        chIdx += (pPce->SideElementIsCpe[el]) ? 2 : 1;
                                    }
                                }
                                /* Count back channels/elements */
                                for(el = 0; el < pPce->NumBackChannelElements; el += 1) {
                                    if(pPce->BackElementHeightInfo[el] == h) {
                                        elIdx += 1;
                                        chIdx += (pPce->BackElementIsCpe[el]) ? 2 : 1;
                                    }
                                }
                                if(h == 0) { /* normal height */
                                    elIdx += pPce->NumLfeChannelElements;
                                    chIdx += pPce->NumLfeChannelElements;
                                }
                            }
                            chMapping[chIdx] = channelIdx;
                            chType[chIdx] = aChType;
                            chIndex[chIdx] = bc[heightLayer];
                            if(isCpe) {
                                chMapping[chIdx + 1] = channelIdx + 1;
                                chType[chIdx + 1] = aChType;
                                chIndex[chIdx + 1] = bc[heightLayer] + 1;
                            }
                            *elMapping = elIdx;
                            return 1;
                        }
                        ec[heightLayer] += 1;
                        if(pPce->BackElementIsCpe[i]) {
                            cc[heightLayer] += 2;
                            bc[heightLayer] += 2;
                        }
                        else {
                            cc[heightLayer] += 1;
                            bc[heightLayer] += 1;
                        }
                    }
                    break;

                case ID_LFE: { /* Unfortunately we have to go through all normal height
                                  layer elements to get the position of the LFE
                                  channels. Start with counting the front
                                  channels/elements at normal height */
                    for(i = 0; i < pPce->NumFrontChannelElements; i += 1) {
                        int32_t heightLayer = pPce->FrontElementHeightInfo[i];
                        ec[heightLayer] += 1;
                        cc[heightLayer] += (pPce->FrontElementIsCpe[i]) ? 2 : 1;
                    }
                    /* Count side channels/elements at normal height */
                    for(i = 0; i < pPce->NumSideChannelElements; i += 1) {
                        int32_t heightLayer = pPce->SideElementHeightInfo[i];
                        ec[heightLayer] += 1;
                        cc[heightLayer] += (pPce->SideElementIsCpe[i]) ? 2 : 1;
                    }
                    /* Count back channels/elements at normal height */
                    for(i = 0; i < pPce->NumBackChannelElements; i += 1) {
                        int32_t heightLayer = pPce->BackElementHeightInfo[i];
                        ec[heightLayer] += 1;
                        cc[heightLayer] += (pPce->BackElementIsCpe[i]) ? 2 : 1;
                    }

                    /* search in lfe channels */
                    for(i = 0; i < pPce->NumLfeChannelElements; i++) {
                        int32_t elIdx = ec[0]; /* LFE channels belong to the normal height layer */
                        int32_t chIdx = cc[0];
                        if(pPce->LfeElementTagSelect[i] == tag) {
                            chMapping[chIdx] = channelIdx;
                            *elMapping = elIdx;
                            chType[chIdx] = ACT_LFE;
                            chIndex[chIdx] = lc;
                            return 1;
                        }
                        ec[0] += 1;
                        cc[0] += 1;
                        lc += 1;
                    }
                } break;

                /* Non audio elements */
                case ID_CCE:
                    /* search in cce channels */
                    for(i = 0; i < pPce->NumValidCcElements; i++) {
                        if(pPce->ValidCcElementTagSelect[i] == tag) { return 1; }
                    }
                    break;
                case ID_DSE:
                    /* search associated data elements */
                    for(i = 0; i < pPce->NumAssocDataElements; i++) {
                        if(pPce->AssocDataElementTagSelect[i] == tag) { return 1; }
                    }
                    break;
                default:
                    return 0;
            }
            return 0; /* not found in any list */
        }
    }

    return 1;
}

#define SPEAKER_PLANE_NORMAL 0
#define SPEAKER_PLANE_TOP    1
#define SPEAKER_PLANE_BOTTOM 2

void CProgramConfig_GetChannelDescription(const uint32_t chConfig, const CProgramConfig_t *pPce,
                                          AUDIO_CHANNEL_TYPE_t chType[], uint8_t chIndex[]) {
    assert(chType != NULL);
    assert(chIndex != NULL);

    if((chConfig == 0) && (pPce != NULL)) {
        if(pPce->isValid) {
            int32_t spkPlane, chIdx = 0;
            for(spkPlane = SPEAKER_PLANE_NORMAL; spkPlane <= SPEAKER_PLANE_BOTTOM; spkPlane += 1) {
                int32_t elIdx, grpChIdx = 0;
                for(elIdx = 0; elIdx < pPce->NumFrontChannelElements; elIdx += 1) {
                    if(pPce->FrontElementHeightInfo[elIdx] == spkPlane) {
                        chType[chIdx] = (AUDIO_CHANNEL_TYPE_t)((spkPlane << 4) | ACT_FRONT);
                        chIndex[chIdx++] = grpChIdx++;
                        if(pPce->FrontElementIsCpe[elIdx]) {
                            chType[chIdx] = (AUDIO_CHANNEL_TYPE_t)((spkPlane << 4) | ACT_FRONT);
                            chIndex[chIdx++] = grpChIdx++;
                        }
                    }
                }
                grpChIdx = 0;
                for(elIdx = 0; elIdx < pPce->NumSideChannelElements; elIdx += 1) {
                    if(pPce->SideElementHeightInfo[elIdx] == spkPlane) {
                        chType[chIdx] = (AUDIO_CHANNEL_TYPE_t)((spkPlane << 4) | ACT_SIDE);
                        chIndex[chIdx++] = grpChIdx++;
                        if(pPce->SideElementIsCpe[elIdx]) {
                            chType[chIdx] = (AUDIO_CHANNEL_TYPE_t)((spkPlane << 4) | ACT_SIDE);
                            chIndex[chIdx++] = grpChIdx++;
                        }
                    }
                }
                grpChIdx = 0;
                for(elIdx = 0; elIdx < pPce->NumBackChannelElements; elIdx += 1) {
                    if(pPce->BackElementHeightInfo[elIdx] == spkPlane) {
                        chType[chIdx] = (AUDIO_CHANNEL_TYPE_t)((spkPlane << 4) | ACT_BACK);
                        chIndex[chIdx++] = grpChIdx++;
                        if(pPce->BackElementIsCpe[elIdx]) {
                            chType[chIdx] = (AUDIO_CHANNEL_TYPE_t)((spkPlane << 4) | ACT_BACK);
                            chIndex[chIdx++] = grpChIdx++;
                        }
                    }
                }
                grpChIdx = 0;
                if(spkPlane == SPEAKER_PLANE_NORMAL) {
                    for(elIdx = 0; elIdx < pPce->NumLfeChannelElements; elIdx += 1) {
                        chType[chIdx] = ACT_LFE;
                        chIndex[chIdx++] = grpChIdx++;
                    }
                }
            }
        }
    }
    else {
        int32_t chIdx;
        for(chIdx = 0; chIdx < getNumberOfTotalChannels(chConfig); chIdx += 1) {
            getImplicitAudioChannelTypeAndIndex(&chType[chIdx], &chIndex[chIdx], chConfig, chIdx);
        }
    }
}

int32_t CProgramConfig_GetPceChMap(const CProgramConfig_t *pPce, uint8_t pceChMap[], const uint32_t pceChMapLen) {
    const uint8_t *nElements = &pPce->NumFrontChannelElements;
    const uint8_t *elHeight[3], *elIsCpe[3];
    unsigned       chIdx, plane, grp, offset, totCh[3], numCh[3][4];

    assert(pPce != NULL);
    assert(pceChMap != NULL);

    /* Init counter: */
    memset(totCh, 0, 3 * sizeof(unsigned));
    memset(numCh, 0, 3 * 4 * sizeof(unsigned));

    /* Analyse PCE: */
    elHeight[0] = pPce->FrontElementHeightInfo;
    elIsCpe[0] = pPce->FrontElementIsCpe;
    elHeight[1] = pPce->SideElementHeightInfo;
    elIsCpe[1] = pPce->SideElementIsCpe;
    elHeight[2] = pPce->BackElementHeightInfo;
    elIsCpe[2] = pPce->BackElementIsCpe;

    for(plane = 0; plane <= SPEAKER_PLANE_BOTTOM; plane += 1) {
        for(grp = 0; grp < 3; grp += 1) { /* front, side, back */
            unsigned el;
            for(el = 0; el < nElements[grp]; el += 1) {
                if(elHeight[grp][el] == plane) {
                    unsigned elCh = elIsCpe[grp][el] ? 2 : 1;
                    numCh[plane][grp] += elCh;
                    totCh[plane] += elCh;
                }
            }
        }
        if(plane == SPEAKER_PLANE_NORMAL) {
            unsigned elCh = pPce->NumLfeChannelElements;
            numCh[plane][grp] += elCh;
            totCh[plane] += elCh;
        }
    }
    /* Sanity checks: */
    chIdx = totCh[SPEAKER_PLANE_NORMAL] + totCh[SPEAKER_PLANE_TOP] + totCh[SPEAKER_PLANE_BOTTOM];
    if(chIdx > pceChMapLen) { return -1; }

    /* Create map: */
    offset = grp = 0;
    unsigned grpThresh = numCh[SPEAKER_PLANE_NORMAL][grp];
    for(chIdx = 0; chIdx < totCh[SPEAKER_PLANE_NORMAL]; chIdx += 1) {
        while((chIdx >= grpThresh) && (grp < 3)) {
            offset += numCh[1][grp] + numCh[2][grp];
            grp += 1;
            grpThresh += numCh[SPEAKER_PLANE_NORMAL][grp];
        }
        pceChMap[chIdx] = chIdx + offset;
    }
    offset = 0;
    for(grp = 0; grp < 4; grp += 1) { /* front, side, back and lfe */
        offset += numCh[SPEAKER_PLANE_NORMAL][grp];
        for(plane = SPEAKER_PLANE_TOP; plane <= SPEAKER_PLANE_BOTTOM; plane += 1) {
            unsigned mapCh;
            for(mapCh = 0; mapCh < numCh[plane][grp]; mapCh += 1) {
                pceChMap[chIdx++] = offset;
                offset += 1;
            }
        }
    }
    return 0;
}

int32_t CProgramConfig_GetElementTable(const CProgramConfig_t *pPce, MP4_ELEMENT_ID_t elList[], const int32_t elListSize,
                                       uint8_t *pChMapIdx) {
    int32_t i, el = 0;

    assert(elList != NULL);
    assert(pChMapIdx != NULL);
    assert(pPce != NULL);

    *pChMapIdx = 0;

    if((elListSize < pPce->NumFrontChannelElements + pPce->NumSideChannelElements + pPce->NumBackChannelElements +
                         pPce->NumLfeChannelElements) ||
       (pPce->NumChannels == 0)) {
        return 0;
    }

    for(i = 0; i < pPce->NumFrontChannelElements; i += 1) {
        elList[el++] = (pPce->FrontElementIsCpe[i]) ? ID_CPE : ID_SCE;
    }

    for(i = 0; i < pPce->NumSideChannelElements; i += 1) {
        elList[el++] = (pPce->SideElementIsCpe[i]) ? ID_CPE : ID_SCE;
    }

    for(i = 0; i < pPce->NumBackChannelElements; i += 1) {
        elList[el++] = (pPce->BackElementIsCpe[i]) ? ID_CPE : ID_SCE;
    }

    for(i = 0; i < pPce->NumLfeChannelElements; i += 1) { elList[el++] = ID_LFE; }

    /* Find an corresponding channel configuration if possible */
    switch(pPce->NumChannels) {
        case 1:
        case 2:
            /* One and two channels have no alternatives. */
            *pChMapIdx = pPce->NumChannels;
            break;
        case 3:
        case 4:
        case 5:
        case 6: { /* Test if the number of channels can be used as channel config:             */
            CProgramConfig_t tmpPce[1];
            /* Create a PCE for the config to test ... */
            CProgramConfig_GetDefault(tmpPce, pPce->NumChannels);
            /* ... and compare it with the given one. */
            *pChMapIdx = (!(CProgramConfig_Compare(pPce, tmpPce) & 0xE)) ? pPce->NumChannels : 0;
            /* If compare result is 0 or 1 we can be sure that it is channel
             * config 11. */
        } break;
        case 7: {
            CProgramConfig_t tmpPce[1];
            /* Create a PCE for the config to test ... */
            CProgramConfig_GetDefault(tmpPce, 11);
            /* ... and compare it with the given one. */
            *pChMapIdx = (!(CProgramConfig_Compare(pPce, tmpPce) & 0xE)) ? 11 : 0;
            /* If compare result is 0 or 1 we can be sure that it is channel
             * config 11. */
        } break;
        case 8: { /* Try the four possible 7.1ch configurations. One after the
                     other. */
            uint8_t testCfg[4] = {32, 14, 12, 7};
            CProgramConfig_t tmpPce[1];
            for(i = 0; i < 4; i += 1) {
                /* Create a PCE for the config to test ... */
                CProgramConfig_GetDefault(tmpPce, testCfg[i]);
                /* ... and compare it with the given one. */
                if(!(CProgramConfig_Compare(pPce, tmpPce) & 0xE)) {
                    /* If the compare result is 0 or 1 than the two channel configurations
                     * match. */
                    /* Explicit mapping of 7.1 side channel configuration to 7.1 rear
                     * channel mapping. */
                    *pChMapIdx = (testCfg[i] == 32) ? 12 : testCfg[i];
                }
            }
        } break;
        default:
            /* The PCE does not match any predefined channel configuration. */
            *pChMapIdx = 0;
            break;
    }

    return el;
}

static AUDIO_OBJECT_TYPE_t getAOT(HANDLE_FDK_BITSTREAM bs) {
    int32_t tmp = 0;

    tmp = FDKreadBits(bs, 5);
    if(tmp == AOT_ESCAPE) {
        int32_t tmp2 = FDKreadBits(bs, 6);
        tmp = 32 + tmp2;
    }

    return (AUDIO_OBJECT_TYPE_t)tmp;
}

static int32_t getSampleRate(HANDLE_FDK_BITSTREAM bs, uint8_t *index, int32_t nBits) {
    int32_t sampleRate;
    int32_t idx;

    idx = FDKreadBits(bs, nBits);
    if(idx == (1 << nBits) - 1) {
        if(FDKgetValidBits(bs) < 24) { return 0; }
        sampleRate = FDKreadBits(bs, 24);
    }
    else { sampleRate = SamplingRateTable[idx]; }

    *index = idx;

    return sampleRate;
}

static TRANSPORTDEC_ERROR_t GaSpecificConfig_Parse(CSGaSpecificConfig_t *self, CSAudioSpecificConfig_t *asc,
                                                 HANDLE_FDK_BITSTREAM bs, uint32_t ascStartAnchor) {
    TRANSPORTDEC_ERROR_t ErrorStatus = TRANSPORTDEC_OK;

    self->m_frameLengthFlag = FDKreadBits(bs, 1);

    self->m_dependsOnCoreCoder = FDKreadBits(bs, 1);

    if(self->m_dependsOnCoreCoder) self->m_coreCoderDelay = FDKreadBits(bs, 14);

    self->m_extensionFlag = FDKreadBits(bs, 1);

    if(asc->m_channelConfiguration == 0) { CProgramConfig_Read(&asc->m_progrConfigElement, bs, ascStartAnchor); }

    if((asc->m_aot == AOT_AAC_SCAL) || (asc->m_aot == AOT_ER_AAC_SCAL)) { self->m_layer = FDKreadBits(bs, 3); }

    if(self->m_extensionFlag) {
        if(asc->m_aot == AOT_ER_BSAC) {
            self->m_numOfSubFrame = FDKreadBits(bs, 5);
            self->m_layerLength = FDKreadBits(bs, 11);
        }

        if((asc->m_aot == AOT_ER_AAC_LC) || (asc->m_aot == AOT_ER_AAC_LTP) || (asc->m_aot == AOT_ER_AAC_SCAL) ||
           (asc->m_aot == AOT_ER_AAC_LD)) {
            asc->m_vcb11Flag = FDKreadBits(bs, 1); /* aacSectionDataResilienceFlag */
            asc->m_rvlcFlag = FDKreadBits(bs, 1);  /* aacScalefactorDataResilienceFlag */
            asc->m_hcrFlag = FDKreadBits(bs, 1);   /* aacSpectralDataResilienceFlag */
        }

        self->m_extensionFlag3 = FDKreadBits(bs, 1);
    }
    return (ErrorStatus);
}

static int32_t skipSbrHeader(HANDLE_FDK_BITSTREAM hBs, int32_t isUsac) {
    /* Dummy parse SbrDfltHeader() */
    int32_t dflt_header_extra1, dflt_header_extra2, bitsToSkip = 0;

    if(!isUsac) {
        bitsToSkip = 6;
        FDKpushFor(hBs, 6); /* amp res 1, xover freq 3, reserved 2 */
    }
    bitsToSkip += 8;
    FDKpushFor(hBs, 8); /* start / stop freq */
    bitsToSkip += 2;
    dflt_header_extra1 = FDKreadBit(hBs);
    dflt_header_extra2 = FDKreadBit(hBs);
    bitsToSkip += 5 * dflt_header_extra1 + 6 * dflt_header_extra2;
    FDKpushFor(hBs, 5 * dflt_header_extra1 + 6 * dflt_header_extra2);

    return bitsToSkip;
}

static int32_t ld_sbr_header(CSAudioSpecificConfig_t *asc, const int32_t dsFactor, HANDLE_FDK_BITSTREAM hBs,
                             CSTpCallBacks_t *cb) {
    const int32_t  channelConfiguration = asc->m_channelConfiguration;
    int32_t        i = 0, j = 0;
    int32_t        error = 0;
    MP4_ELEMENT_ID_t element = ID_NONE;

    /* check whether the channelConfiguration is defined in
     * channel_configuration_array */
    if(channelConfiguration < 0 ||
       channelConfiguration > (int32_t)(sizeof(channel_configuration_array) / sizeof(MP4_ELEMENT_ID_t **) - 1)) {
        return TRANSPORTDEC_PARSE_ERROR;
    }

    /* read elements of the passed channel_configuration until there is ID_NONE */
    while((element = channel_configuration_array[channelConfiguration][j]) != ID_NONE) {
        /* Setup LFE element for upsampling too. This is essential especially for
         * channel configs where the LFE element is not at the last position for
         * example in channel config 13 or 14. It leads to memory leaks if the setup
         * of the LFE element would be done later in the core. */
        if(element == ID_SCE || element == ID_CPE || element == ID_LFE) {
            error |= cb->cbSbr(cb->cbSbrData, hBs, asc->m_samplingFrequency / dsFactor,
                               asc->m_extensionSamplingFrequency / dsFactor, asc->m_samplesPerFrame / dsFactor,
                               AOT_ER_AAC_ELD, element, i++, 0, 0, asc->configMode, &asc->SbrConfigChanged, dsFactor);
            if(error != TRANSPORTDEC_OK) { goto bail; }
        }
        j++;
    }
bail:
    return error;
}

static TRANSPORTDEC_ERROR_t EldSpecificConfig_Parse(CSAudioSpecificConfig_t *asc, HANDLE_FDK_BITSTREAM hBs,
                                                  CSTpCallBacks_t *cb) {
    TRANSPORTDEC_ERROR_t   ErrorStatus = TRANSPORTDEC_OK;
    CSEldSpecificConfig_t *esc = &asc->m_sc.m_eldSpecificConfig;
    uint32_t             eldExtType;
    int32_t              eldExtLen, len, cnt, ldSbrLen = 0, eldExtLenSum, numSbrHeader = 0, sbrIndex, eldExtCnt = 0;

    unsigned char downscale_fill_nibble;

    memset(esc, 0, sizeof(CSEldSpecificConfig_t));

    esc->m_frameLengthFlag = FDKreadBits(hBs, 1);
    if(esc->m_frameLengthFlag) { asc->m_samplesPerFrame = 480; }
    else { asc->m_samplesPerFrame = 512; }

    asc->m_vcb11Flag = FDKreadBits(hBs, 1);
    asc->m_rvlcFlag = FDKreadBits(hBs, 1);
    asc->m_hcrFlag = FDKreadBits(hBs, 1);

    esc->m_sbrPresentFlag = FDKreadBits(hBs, 1);

    if(esc->m_sbrPresentFlag == 1) {
        esc->m_sbrSamplingRate = FDKreadBits(hBs, 1); /* 0: single rate, 1: dual rate */
        esc->m_sbrCrcFlag = FDKreadBits(hBs, 1);

        asc->m_extensionSamplingFrequency = asc->m_samplingFrequency << esc->m_sbrSamplingRate;

        if(cb->cbSbr != NULL) {
            /* ELD reduced delay mode: LD-SBR initialization has to know the downscale
               information. Postpone LD-SBR initialization and read ELD extension
               information first. */
            switch(asc->m_channelConfiguration) {
                case 1:
                case 2:
                    numSbrHeader = 1;
                    break;
                case 3:
                    numSbrHeader = 2;
                    break;
                case 4:
                case 5:
                case 6:
                    numSbrHeader = 3;
                    break;
                case 7:
                case 11:
                case 12:
                case 14:
                    numSbrHeader = 4;
                    break;
                default:
                    numSbrHeader = 0;
                    break;
            }
            for(sbrIndex = 0; sbrIndex < numSbrHeader; sbrIndex++) { ldSbrLen += skipSbrHeader(hBs, 0); }
        }
        else { return TRANSPORTDEC_UNSUPPORTED_FORMAT; }
    }
    esc->m_useLdQmfTimeAlign = 0;

    /* new ELD syntax */
    eldExtLenSum = FDKgetValidBits(hBs);
    esc->m_downscaledSamplingFrequency = asc->m_samplingFrequency;
    /* parse ExtTypeConfigData */
    while(((eldExtType = FDKreadBits(hBs, 4)) != ELDEXT_TERM) && ((int32_t)FDKgetValidBits(hBs) >= 0) &&
          (eldExtCnt++ < 15)) {
        eldExtLen = len = FDKreadBits(hBs, 4);
        if(len == 0xf) {
            len = FDKreadBits(hBs, 8);
            eldExtLen += len;

            if(len == 0xff) {
                len = FDKreadBits(hBs, 16);
                eldExtLen += len;
            }
        }

        switch(eldExtType) {
            case ELDEXT_LDSAC:
                esc->m_useLdQmfTimeAlign = 1;
                if(cb->cbSsc != NULL) {
                    ErrorStatus = (TRANSPORTDEC_ERROR_t)cb->cbSsc(
                        cb->cbSscData, hBs, asc->m_aot, asc->m_samplingFrequency << esc->m_sbrSamplingRate,
                        asc->m_samplesPerFrame << esc->m_sbrSamplingRate, 1, /* stereoConfigIndex */
                        -1,                                                  /* nTimeSlots: read from bitstream */
                        eldExtLen, asc->configMode, &asc->SacConfigChanged);
                    if(ErrorStatus != TRANSPORTDEC_OK) { return TRANSPORTDEC_PARSE_ERROR; }
                    if(esc->m_downscaledSamplingFrequency != asc->m_samplingFrequency) {
                        return TRANSPORTDEC_UNSUPPORTED_FORMAT; /* ELDv2 w/ ELD downscaled
                                                                   mode not allowed */
                    }
                    break;
                }

                /* fall through */;
            default:
                for(cnt = 0; cnt < eldExtLen; cnt++) { FDKreadBits(hBs, 8); }
                break;

            case ELDEXT_DOWNSCALEINFO:
                uint8_t tmpDownscaleFreqIdx;
                esc->m_downscaledSamplingFrequency = getSampleRate(hBs, &tmpDownscaleFreqIdx, 4);
                if(esc->m_downscaledSamplingFrequency == 0 || esc->m_downscaledSamplingFrequency > 96000) {
                    return TRANSPORTDEC_PARSE_ERROR;
                }
                downscale_fill_nibble = FDKreadBits(hBs, 4);
                if(downscale_fill_nibble != 0x0) { return TRANSPORTDEC_PARSE_ERROR; }
                if(esc->m_useLdQmfTimeAlign == 1) {
                    return TRANSPORTDEC_UNSUPPORTED_FORMAT; /* ELDv2 w/ ELD downscaled
                                                               mode not allowed */
                }
                break;
        }
    }
    if(eldExtType != ELDEXT_TERM) { return TRANSPORTDEC_PARSE_ERROR; }

    if((int32_t)FDKgetValidBits(hBs) < 0) { return TRANSPORTDEC_PARSE_ERROR; }

    if(esc->m_sbrPresentFlag == 1 && numSbrHeader != 0) {
        int32_t dsFactor = 1; /* Downscale factor must be 1 or even for SBR */
        if(esc->m_downscaledSamplingFrequency != 0) {
            if(asc->m_samplingFrequency % esc->m_downscaledSamplingFrequency != 0) {
                return TRANSPORTDEC_UNSUPPORTED_FORMAT;
            }
            dsFactor = asc->m_samplingFrequency / esc->m_downscaledSamplingFrequency;
            if(dsFactor != 1 && (dsFactor)&1) {
                return TRANSPORTDEC_UNSUPPORTED_FORMAT; /* SBR needs an even downscale
                                                           factor */
            }
            if(dsFactor != 1 && dsFactor != 2 && dsFactor != 4) {
                dsFactor = 1; /* don't apply dsf for not yet supported even dsfs */
            }
            if((int32_t)asc->m_samplesPerFrame % dsFactor != 0) {
                return TRANSPORTDEC_UNSUPPORTED_FORMAT; /* frameSize/dsf must be an
                                                           integer number */
            }
        }
        eldExtLenSum = eldExtLenSum - FDKgetValidBits(hBs);
        FDKpushBack(hBs, eldExtLenSum + ldSbrLen);
        if(0 != ld_sbr_header(asc, dsFactor, hBs, cb)) { return TRANSPORTDEC_PARSE_ERROR; }
        FDKpushFor(hBs, eldExtLenSum);
    }
    return (ErrorStatus);
}

/*
Subroutine to store config in uint8_t buffer. Bit stream position does not change.
*/
static uint32_t StoreConfigAsBitstream(HANDLE_FDK_BITSTREAM hBs, const int32_t configSize_bits, /* If < 0 (> 0) config
                                                                                               to read is before
                                                                                               (after) current bit
                                                                                               stream position. */
                                       uint8_t *configTargetBuffer, const uint16_t configTargetBufferSize_bytes) {
    FDK_BITSTREAM_t  usacConf;
    uint32_t const nBits = fAbs(configSize_bits);
    uint32_t       j, tmp;

    if(nBits > 8 * (uint32_t)configTargetBufferSize_bytes) { return 1; }
    memset(configTargetBuffer, 0, configTargetBufferSize_bytes);

    FDKinitBitStream(&usacConf, configTargetBuffer, configTargetBufferSize_bytes, nBits, BS_WRITER);
    if(configSize_bits < 0) { FDKpushBack(hBs, nBits); }
    for(j = nBits; j > 31; j -= 32) {
        tmp = FDKreadBits(hBs, 32);
        FDKwriteBits(&usacConf, tmp, 32);
    }
    if(j > 0) {
        tmp = FDKreadBits(hBs, j);
        FDKwriteBits(&usacConf, tmp, j);
    }
    FDKsyncCache(&usacConf);
    if(configSize_bits > 0) { FDKpushBack(hBs, nBits); }

    return 0;
}

/* maps coreSbrFrameLengthIndex to coreCoderFrameLength */
static const uint16_t usacFrameLength[8] = {768, 1024, 2048, 2048, 4096, 0, 0, 0};
/* maps coreSbrFrameLengthIndex to sbrRatioIndex */
static const uint8_t sbrRatioIndex[8] = {0, 0, 2, 3, 1, 0, 0, 0};

/*
  subroutine for parsing extension element configuration:
  UsacExtElementConfig() q.v. ISO/IEC FDIS 23003-3:2011(E) Table 14
  rsv603daExtElementConfig() q.v. ISO/IEC DIS 23008-3 Table 13
*/
static TRANSPORTDEC_ERROR_t extElementConfig(CSUsacExtElementConfig_t *extElement, HANDLE_FDK_BITSTREAM hBs,
                                           const CSTpCallBacks_t *cb, const uint8_t numSignalsInGroup,
                                           const uint32_t coreFrameLength, const int32_t subStreamIndex,
                                           const AUDIO_OBJECT_TYPE_t aot) {
    TRANSPORTDEC_ERROR_t ErrorStatus = TRANSPORTDEC_OK;

    int32_t usacExtElementType = escapedValue(hBs, 4, 8, 16);

    /* recurve extension elements which are invalid for USAC */
    if(aot == AOT_USAC) {
        switch(usacExtElementType) {
            case ID_EXT_ELE_FILL:
            case ID_EXT_ELE_MPEGS:
            case ID_EXT_ELE_SAOC:
            case ID_EXT_ELE_AUDIOPREROLL:
            case ID_EXT_ELE_UNI_DRC:
                break;
            default:
                usacExtElementType = ID_EXT_ELE_UNKNOWN;
                break;
        }
    }

    extElement->usacExtElementType = (USAC_EXT_ELEMENT_TYPE_t)usacExtElementType;
    int32_t usacExtElementConfigLength = escapedValue(hBs, 4, 8, 16);
    extElement->usacExtElementConfigLength = (uint16_t)usacExtElementConfigLength;
    int32_t bsAnchor;

    if(FDKreadBit(hBs)) /* usacExtElementDefaultLengthPresent */
        extElement->usacExtElementDefaultLength = escapedValue(hBs, 8, 16, 0) + 1;
    else
        extElement->usacExtElementDefaultLength = 0;

    extElement->usacExtElementPayloadFrag = FDKreadBit(hBs);

    bsAnchor = (int32_t)FDKgetValidBits(hBs);

    switch(usacExtElementType) {
        case ID_EXT_ELE_UNKNOWN:
        case ID_EXT_ELE_FILL:
            break;
        case ID_EXT_ELE_AUDIOPREROLL:
            /* No configuration element */
            extElement->usacExtElementHasAudioPreRoll = 1;
            break;
        case ID_EXT_ELE_UNI_DRC: {
            if(cb->cbUniDrc != NULL) {
                ErrorStatus = (TRANSPORTDEC_ERROR_t)cb->cbUniDrc(cb->cbUniDrcData, hBs, usacExtElementConfigLength,
                                                               0, /* uniDrcConfig */
                                                               subStreamIndex, 0, aot);
                if(ErrorStatus != TRANSPORTDEC_OK) { return ErrorStatus; }
            }
        } break;
        default:
            break;
    }

    /* Adjust bit stream position. This is required because of byte alignment and
     * unhandled extensions. */
    {
        int32_t left_bits = (usacExtElementConfigLength << 3) - (bsAnchor - (int32_t)FDKgetValidBits(hBs));
        if(left_bits >= 0) { FDKpushFor(hBs, left_bits); }
        else {
            /* parsed too many bits */
            ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
        }
    }

    return ErrorStatus;
}

/*
  subroutine for parsing the USAC / RSVD60 configuration extension:
  UsacConfigExtension() q.v. ISO/IEC FDIS 23003-3:2011(E) Table 15
  rsv603daConfigExtension() q.v. ISO/IEC DIS 23008-3 Table 14
*/
static TRANSPORTDEC_ERROR_t configExtension(CSUsacConfig_t *usc, HANDLE_FDK_BITSTREAM hBs, const CSTpCallBacks_t *cb) {
    TRANSPORTDEC_ERROR_t ErrorStatus = TRANSPORTDEC_OK;

    int32_t numConfigExtensions;
    int32_t usacConfigExtType;
    int32_t usacConfigExtLength;

    numConfigExtensions = (int32_t)escapedValue(hBs, 2, 4, 8) + 1;
    for(int32_t confExtIdx = 0; confExtIdx < numConfigExtensions; confExtIdx++) {
        int32_t nbits;
        int32_t loudnessInfoSetConfigExtensionPosition = FDKgetValidBits(hBs);
        usacConfigExtType = escapedValue(hBs, 4, 8, 16);
        usacConfigExtLength = (int32_t)escapedValue(hBs, 4, 8, 16);

        /* Start bit position of config extension */
        nbits = (int32_t)FDKgetValidBits(hBs);

        /* Return an error in case the bitbuffer fill level is too low. */
        if(nbits < usacConfigExtLength * 8) { return TRANSPORTDEC_PARSE_ERROR; }

        switch(usacConfigExtType) {
            case ID_CONFIG_EXT_FILL:
                for(int32_t i = 0; i < usacConfigExtLength; i++) {
                    if(FDKreadBits(hBs, 8) != 0xa5) { return TRANSPORTDEC_PARSE_ERROR; }
                }
                break;
            case ID_CONFIG_EXT_LOUDNESS_INFO: {
                if(cb->cbUniDrc != NULL) {
                    ErrorStatus = (TRANSPORTDEC_ERROR_t)cb->cbUniDrc(cb->cbUniDrcData, hBs, usacConfigExtLength,
                                                                   1, /* loudnessInfoSet */
                                                                   0, loudnessInfoSetConfigExtensionPosition, AOT_USAC);
                    if(ErrorStatus != TRANSPORTDEC_OK) { return ErrorStatus; }
                }
            } break;
            default:
                break;
        }

        /* Skip remaining bits. If too many bits were parsed, assume error. */
        usacConfigExtLength = 8 * usacConfigExtLength - (nbits - (int32_t)FDKgetValidBits(hBs));
        if(usacConfigExtLength < 0) { return TRANSPORTDEC_PARSE_ERROR; }
        FDKpushFor(hBs, usacConfigExtLength);
    }

    return ErrorStatus;
}

/* This function unifies decoder config parsing of USAC and RSV60:
   rsv603daDecoderConfig() ISO/IEC DIS 23008-3   Table 8
   UsacDecoderConfig()     ISO/IEC FDIS 23003-3  Table 6
  */
static TRANSPORTDEC_ERROR_t UsacRsv60DecoderConfig_Parse(CSAudioSpecificConfig_t *asc, HANDLE_FDK_BITSTREAM hBs,
                                                       const CSTpCallBacks_t *cb) {
    TRANSPORTDEC_ERROR_t ErrorStatus = TRANSPORTDEC_OK;
    CSUsacConfig_t      *usc = &asc->m_sc.m_usacConfig;
    int32_t            i, numberOfElements;
    int32_t            channelElementIdx = 0; /* index for elements which contain audio channels (sce, cpe, lfe) */
    SC_CHANNEL_CONFIG  sc_chan_config = {0, 0, 0, 0};

    numberOfElements = (int32_t)escapedValue(hBs, 4, 8, 16) + 1;
    usc->m_usacNumElements = numberOfElements;
    if(numberOfElements > TP_USAC_MAX_ELEMENTS) { return TRANSPORTDEC_UNSUPPORTED_FORMAT; }
    usc->m_nUsacChannels = 0;
    usc->m_channelConfigurationIndex = asc->m_channelConfiguration;

    if(asc->m_aot == AOT_USAC) {
        sc_chan_config = sc_chan_config_tab[usc->m_channelConfigurationIndex];

        if(sc_chan_config.nCh > (int8_t)TP_USAC_MAX_SPEAKERS) { return TRANSPORTDEC_PARSE_ERROR; }
    }

    for(i = 0; i < numberOfElements; i++) {
        MP4_ELEMENT_ID_t usacElementType = (MP4_ELEMENT_ID_t)(FDKreadBits(hBs, 2) | USAC_ID_BIT); /* set USAC_ID_BIT to map
                                                                                                 usacElementType to
                                                                                                 MP4_ELEMENT_ID_t enum */
        usc->element[i].usacElementType = usacElementType;

        /* sanity check: update element counter */
        if(asc->m_aot == AOT_USAC) {
            switch(usacElementType) {
                case ID_USAC_SCE:
                    sc_chan_config.nSCE--;
                    break;
                case ID_USAC_CPE:
                    sc_chan_config.nCPE--;
                    break;
                case ID_USAC_LFE:
                    sc_chan_config.nLFE--;
                    break;
                default:
                    break;
            }
            if(usc->m_channelConfigurationIndex) {
                /* sanity check: no element counter may be smaller zero */
                if(sc_chan_config.nCPE < 0 || sc_chan_config.nSCE < 0 || sc_chan_config.nLFE < 0) {
                    return TRANSPORTDEC_PARSE_ERROR;
                }
            }
        }

        switch(usacElementType) {
            case ID_USAC_SCE:
                /* UsacCoreConfig() ISO/IEC FDIS 23003-3  Table 10 */
                if(FDKreadBit(hBs)) { /* tw_mdct */
                    return TRANSPORTDEC_UNSUPPORTED_FORMAT;
                }
                usc->element[i].m_noiseFilling = FDKreadBits(hBs, 1);
                /* end of UsacCoreConfig() */
                if(usc->m_sbrRatioIndex > 0) {
                    if(cb->cbSbr == NULL) { return TRANSPORTDEC_UNKOWN_ERROR; }
                    /* SbrConfig() ISO/IEC FDIS 23003-3  Table 11 */
                    usc->element[i].m_harmonicSBR = FDKreadBit(hBs);
                    usc->element[i].m_interTes = FDKreadBit(hBs);
                    usc->element[i].m_pvc = FDKreadBit(hBs);
                    if(cb->cbSbr(cb->cbSbrData, hBs, asc->m_samplingFrequency, asc->m_extensionSamplingFrequency,
                                 asc->m_samplesPerFrame, asc->m_aot, ID_SCE, channelElementIdx,
                                 usc->element[i].m_harmonicSBR, usc->element[i].m_stereoConfigIndex, asc->configMode,
                                 &asc->SbrConfigChanged, 1)) {
                        return TRANSPORTDEC_PARSE_ERROR;
                    }
                    /* end of SbrConfig() */
                }
                usc->m_nUsacChannels += 1;
                channelElementIdx++;
                break;

            case ID_USAC_CPE:
                /* UsacCoreConfig() ISO/IEC FDIS 23003-3  Table 10 */
                if(FDKreadBit(hBs)) { /* tw_mdct */
                    return TRANSPORTDEC_UNSUPPORTED_FORMAT;
                }
                usc->element[i].m_noiseFilling = FDKreadBits(hBs, 1);
                /* end of UsacCoreConfig() */
                if(usc->m_sbrRatioIndex > 0) {
                    if(cb->cbSbr == NULL) return TRANSPORTDEC_UNKOWN_ERROR;
                    /* SbrConfig() ISO/IEC FDIS 23003-3 */
                    usc->element[i].m_harmonicSBR = FDKreadBit(hBs);
                    usc->element[i].m_interTes = FDKreadBit(hBs);
                    usc->element[i].m_pvc = FDKreadBit(hBs);
                    {
                        int32_t bitsToSkip = skipSbrHeader(hBs, 1);
                        /* read stereoConfigIndex */
                        usc->element[i].m_stereoConfigIndex = FDKreadBits(hBs, 2);
                        /* rewind */
                        FDKpushBack(hBs, bitsToSkip + 2);
                    }
                    {
                        MP4_ELEMENT_ID_t el_type =
                            (usc->element[i].m_stereoConfigIndex == 1 || usc->element[i].m_stereoConfigIndex == 2)
                                ? ID_SCE
                                : ID_CPE;
                        if(cb->cbSbr(cb->cbSbrData, hBs, asc->m_samplingFrequency, asc->m_extensionSamplingFrequency,
                                     asc->m_samplesPerFrame, asc->m_aot, el_type, channelElementIdx,
                                     usc->element[i].m_harmonicSBR, usc->element[i].m_stereoConfigIndex,
                                     asc->configMode, &asc->SbrConfigChanged, 1)) {
                            return TRANSPORTDEC_PARSE_ERROR;
                        }
                    }
                    /* end of SbrConfig() */

                    usc->element[i].m_stereoConfigIndex = FDKreadBits(hBs, 2); /* Needed in RM5 syntax */

                    if(usc->element[i].m_stereoConfigIndex > 0) {
                        if(cb->cbSsc != NULL) {
                            int32_t samplesPerFrame = asc->m_samplesPerFrame;

                            if(usc->m_sbrRatioIndex == 1) samplesPerFrame <<= 2;
                            if(usc->m_sbrRatioIndex == 2) samplesPerFrame = (samplesPerFrame * 8) / 3;
                            if(usc->m_sbrRatioIndex == 3) samplesPerFrame <<= 1;

                            /* Mps212Config() ISO/IEC FDIS 23003-3 */
                            if(cb->cbSsc(cb->cbSscData, hBs, asc->m_aot, asc->m_extensionSamplingFrequency,
                                         samplesPerFrame, usc->element[i].m_stereoConfigIndex,
                                         usc->m_coreSbrFrameLengthIndex, 0, /* don't know the length */
                                         asc->configMode, &asc->SacConfigChanged)) {
                                return TRANSPORTDEC_PARSE_ERROR;
                            }
                            /* end of Mps212Config() */
                        }
                        else { return TRANSPORTDEC_UNKOWN_ERROR; }
                    }
                }
                else { usc->element[i].m_stereoConfigIndex = 0; }
                usc->m_nUsacChannels += 2;

                channelElementIdx++;
                break;

            case ID_USAC_LFE:
                usc->element[i].m_noiseFilling = 0;
                usc->m_nUsacChannels += 1;
                if(usc->m_sbrRatioIndex > 0) {
                    /* Use SBR for upsampling */
                    if(cb->cbSbr == NULL) return ErrorStatus = TRANSPORTDEC_UNKOWN_ERROR;
                    usc->element[i].m_harmonicSBR = (uint8_t)0;
                    usc->element[i].m_interTes = (uint8_t)0;
                    usc->element[i].m_pvc = (uint8_t)0;
                    if(cb->cbSbr(cb->cbSbrData, hBs, asc->m_samplingFrequency, asc->m_extensionSamplingFrequency,
                                 asc->m_samplesPerFrame, asc->m_aot, ID_LFE, channelElementIdx,
                                 usc->element[i].m_harmonicSBR, usc->element[i].m_stereoConfigIndex, asc->configMode,
                                 &asc->SbrConfigChanged, 1)) {
                        return ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
                    }
                }
                channelElementIdx++;
                break;

            case ID_USAC_EXT:
                ErrorStatus =
                    extElementConfig(&usc->element[i].extElement, hBs, cb, 0, asc->m_samplesPerFrame, 0, asc->m_aot);

                if(ErrorStatus) { return ErrorStatus; }
                break;

            default:
                /* non USAC-element encountered */
                return TRANSPORTDEC_PARSE_ERROR;
        }
    }

    if(asc->m_aot == AOT_USAC) {
        if(usc->m_channelConfigurationIndex) {
            /* sanity check: all element counter must be zero */
            if(sc_chan_config.nCPE | sc_chan_config.nSCE | sc_chan_config.nLFE) { return TRANSPORTDEC_PARSE_ERROR; }
        }
        else {
            /* sanity check: number of audio channels shall be equal to or smaller
             * than the accumulated sum of all channels */
            if((int32_t)(-2 * sc_chan_config.nCPE - sc_chan_config.nSCE - sc_chan_config.nLFE) <
               (int32_t)usc->numAudioChannels) {
                return TRANSPORTDEC_PARSE_ERROR;
            }
        }
    }

    return ErrorStatus;
}

/* Mapping of coreSbrFrameLengthIndex defined by Table 70 in ISO/IEC 23003-3 */
static TRANSPORTDEC_ERROR_t UsacConfig_SetCoreSbrFrameLengthIndex(CSAudioSpecificConfig_t *asc,
                                                                int32_t                coreSbrFrameLengthIndex) {
    int32_t sbrRatioIndex_val;

    if(coreSbrFrameLengthIndex > 4) { return TRANSPORTDEC_PARSE_ERROR; /* reserved values */ }
    asc->m_sc.m_usacConfig.m_coreSbrFrameLengthIndex = coreSbrFrameLengthIndex;
    asc->m_samplesPerFrame = usacFrameLength[coreSbrFrameLengthIndex];
    sbrRatioIndex_val = sbrRatioIndex[coreSbrFrameLengthIndex];
    asc->m_sc.m_usacConfig.m_sbrRatioIndex = sbrRatioIndex_val;

    if(sbrRatioIndex_val > 0) {
        asc->m_sbrPresentFlag = 1;
        asc->m_extensionSamplingFrequency = asc->m_samplingFrequency;
        asc->m_extensionSamplingFrequencyIndex = asc->m_samplingFrequencyIndex;
        switch(sbrRatioIndex_val) {
            case 1: /* sbrRatio = 4:1 */
                asc->m_samplingFrequency >>= 2;
                asc->m_samplesPerFrame >>= 2;
                break;
            case 2: /* sbrRatio = 8:3 */
                asc->m_samplingFrequency = (asc->m_samplingFrequency * 3) / 8;
                asc->m_samplesPerFrame = (asc->m_samplesPerFrame * 3) / 8;
                break;
            case 3: /* sbrRatio = 2:1 */
                asc->m_samplingFrequency >>= 1;
                asc->m_samplesPerFrame >>= 1;
                break;
            default:
                return TRANSPORTDEC_PARSE_ERROR;
        }
        asc->m_samplingFrequencyIndex = getSamplingRateIndex(asc->m_samplingFrequency, 4);
    }

    return TRANSPORTDEC_OK;
}

static TRANSPORTDEC_ERROR_t UsacConfig_Parse(CSAudioSpecificConfig_t *asc, HANDLE_FDK_BITSTREAM hBs, CSTpCallBacks_t *cb) {
    int32_t            usacSamplingFrequency, channelConfigurationIndex, coreSbrFrameLengthIndex;
    TRANSPORTDEC_ERROR_t err = TRANSPORTDEC_OK;

    /* Start bit position of usacConfig */
    int32_t nbits = (int32_t)FDKgetValidBits(hBs);

    usacSamplingFrequency = getSampleRate(hBs, &asc->m_samplingFrequencyIndex, 5);
    if(usacSamplingFrequency == 0 || usacSamplingFrequency > 96000) { return TRANSPORTDEC_PARSE_ERROR; }
    asc->m_samplingFrequency = (uint32_t)usacSamplingFrequency;

    coreSbrFrameLengthIndex = FDKreadBits(hBs, 3);
    if(UsacConfig_SetCoreSbrFrameLengthIndex(asc, coreSbrFrameLengthIndex) != TRANSPORTDEC_OK) {
        return TRANSPORTDEC_PARSE_ERROR;
    }

    channelConfigurationIndex = FDKreadBits(hBs, 5);
    if(channelConfigurationIndex > 2) {
        return TRANSPORTDEC_PARSE_ERROR; /* only channelConfigurationIndex = [1,2]
                                            are supported */
    }

    if(channelConfigurationIndex == 0) {
        return TRANSPORTDEC_PARSE_ERROR; /* only channelConfigurationIndex = [1,2]
                                            are supported */
    }
    asc->m_channelConfiguration = channelConfigurationIndex;

    err = UsacRsv60DecoderConfig_Parse(asc, hBs, cb);
    if(err != TRANSPORTDEC_OK) { return err; }

    if(FDKreadBits(hBs, 1)) { /* usacConfigExtensionPresent */
        err = configExtension(&asc->m_sc.m_usacConfig, hBs, cb);
        if(err != TRANSPORTDEC_OK) { return err; }
    }

    /* sanity check whether number of channels signaled in UsacDecoderConfig()
       matches the number of channels required by channelConfigurationIndex */
    if((channelConfigurationIndex > 0) &&
       (sc_chan_config_tab[channelConfigurationIndex].nCh != asc->m_sc.m_usacConfig.m_nUsacChannels)) {
        return TRANSPORTDEC_PARSE_ERROR;
    }

    /* Copy UsacConfig() to asc->m_sc.m_usacConfig.UsacConfig[] buffer. */
    int32_t configSize_bits = (int32_t)FDKgetValidBits(hBs) - nbits;
    StoreConfigAsBitstream(hBs, configSize_bits, asc->m_sc.m_usacConfig.UsacConfig, TP_USAC_MAX_CONFIG_LEN);
    asc->m_sc.m_usacConfig.UsacConfigBits = fAbs(configSize_bits);

    return err;
}

static TRANSPORTDEC_ERROR_t AudioSpecificConfig_ExtensionParse(CSAudioSpecificConfig_t *self, HANDLE_FDK_BITSTREAM bs,
                                                             CSTpCallBacks_t *cb) {
    TP_ASC_EXTENSION_ID lastAscExt, ascExtId = ASCEXT_UNKOWN;
    int32_t             bitsAvailable = (int32_t)FDKgetValidBits(bs);

    while(bitsAvailable >= 11) {
        lastAscExt = ascExtId;
        ascExtId = (TP_ASC_EXTENSION_ID)FDKreadBits(bs, 11);
        bitsAvailable -= 11;

        switch(ascExtId) {
            case ASCEXT_SBR: /* 0x2b7 */
                if((self->m_extensionAudioObjectType != AOT_SBR) && (bitsAvailable >= 5)) {
                    self->m_extensionAudioObjectType = getAOT(bs);

                    if((self->m_extensionAudioObjectType == AOT_SBR) ||
                       (self->m_extensionAudioObjectType == AOT_ER_BSAC)) { /* Get SBR extension configuration */
                        self->m_sbrPresentFlag = FDKreadBits(bs, 1);
                        if(self->m_aot == AOT_USAC && self->m_sbrPresentFlag > 0 &&
                           self->m_sc.m_usacConfig.m_sbrRatioIndex == 0) {
                            return TRANSPORTDEC_PARSE_ERROR;
                        }

                        if(self->m_sbrPresentFlag == 1) {
                            self->m_extensionSamplingFrequency =
                                getSampleRate(bs, &self->m_extensionSamplingFrequencyIndex, 4);

                            if(self->m_extensionSamplingFrequency == 0 || self->m_extensionSamplingFrequency > 96000) {
                                return TRANSPORTDEC_PARSE_ERROR;
                            }
                        }
                        if(self->m_extensionAudioObjectType == AOT_ER_BSAC) {
                            self->m_extensionChannelConfiguration = FDKreadBits(bs, 4);
                        }
                    }
                    /* Update counter because of variable length fields (AOT and sampling
                     * rate) */
                    bitsAvailable = (int32_t)FDKgetValidBits(bs);
                }
                break;
            case ASCEXT_PS: /* 0x548 */
                if((lastAscExt == ASCEXT_SBR) && (self->m_extensionAudioObjectType == AOT_SBR) &&
                   (bitsAvailable > 0)) { /* Get PS extension configuration */
                    self->m_psPresentFlag = FDKreadBits(bs, 1);
                    bitsAvailable -= 1;
                }
                break;
            case ASCEXT_MPS: /* 0x76a */
                if(self->m_extensionAudioObjectType == AOT_MPEGS) break;
                /* fall through */;
            case ASCEXT_LDMPS: /* 0x7cc */
                if((ascExtId == ASCEXT_LDMPS) && (self->m_extensionAudioObjectType == AOT_LD_MPEGS)) break;
                if(bitsAvailable >= 1) {
                    bitsAvailable -= 1;
                    if(FDKreadBits(bs, 1)) { /* self->m_mpsPresentFlag */
                        int32_t sscLen = FDKreadBits(bs, 8);
                        bitsAvailable -= 8;
                        if(sscLen == 0xFF) {
                            sscLen += FDKreadBits(bs, 16);
                            bitsAvailable -= 16;
                        }
                        FDKpushFor(bs, sscLen); /* Skip SSC to be able to read the next
                                                   extension if there is one. */

                        bitsAvailable -= sscLen * 8;
                    }
                }
                break;
            case ASCEXT_SAOC:
                if((ascExtId == ASCEXT_SAOC) && (self->m_extensionAudioObjectType == AOT_SAOC)) break;
                if(FDKreadBits(bs, 1)) { /* saocPresent */
                    int32_t saocscLen = FDKreadBits(bs, 8);
                    bitsAvailable -= 8;
                    if(saocscLen == 0xFF) {
                        saocscLen += FDKreadBits(bs, 16);
                        bitsAvailable -= 16;
                    }
                    FDKpushFor(bs, saocscLen);
                    bitsAvailable -= saocscLen * 8;
                }
                break;
            default:
                /* Just ignore anything. */
                return TRANSPORTDEC_OK;
        }
    }

    return TRANSPORTDEC_OK;
}

/*
 * API Functions
 */

void AudioSpecificConfig_Init(CSAudioSpecificConfig_t *asc) {
    memset(asc, 0, sizeof(CSAudioSpecificConfig_t));

    /* Init all values that should not be zero. */
    asc->m_aot = AOT_NONE;
    asc->m_samplingFrequencyIndex = 0xf;
    asc->m_epConfig = -1;
    asc->m_extensionAudioObjectType = AOT_NULL_OBJECT;
    CProgramConfig_Init(&asc->m_progrConfigElement);
}

TRANSPORTDEC_ERROR_t AudioSpecificConfig_Parse(CSAudioSpecificConfig_t *self, HANDLE_FDK_BITSTREAM bs,
                                             int32_t fExplicitBackwardCompatible, CSTpCallBacks_t *cb, uint8_t configMode,
                                             uint8_t configChanged, AUDIO_OBJECT_TYPE_t m_aot) {
    TRANSPORTDEC_ERROR_t ErrorStatus = TRANSPORTDEC_OK;
    uint32_t           ascStartAnchor = FDKgetValidBits(bs);
    int32_t            frameLengthFlag = -1;

    AudioSpecificConfig_Init(self);

    self->configMode = configMode;
    self->AacConfigChanged = configChanged;
    self->SbrConfigChanged = configChanged;
    self->SacConfigChanged = configChanged;

    if(m_aot != AOT_NULL_OBJECT) { self->m_aot = m_aot; }
    else {
        self->m_aot = getAOT(bs);
        self->m_samplingFrequency = getSampleRate(bs, &self->m_samplingFrequencyIndex, 4);
        if(self->m_samplingFrequency <= 0 || (self->m_samplingFrequency > 96000 && self->m_aot != 39) ||
           self->m_samplingFrequency > 4 * 96000) {
            return TRANSPORTDEC_PARSE_ERROR;
        }

        self->m_channelConfiguration = FDKreadBits(bs, 4);

        /* MPEG-04 standard ISO/IEC 14496-3: channelConfiguration == 0 is reserved
           in er_raw_data_block (table 4.19) and er_raw_data_block_eld (table 4.75)
           MPEG-04 conformance ISO/IEC 14496-4: channelConfiguration == 0 is not
           permitted for AOT_ER_AAC_LC, AOT_ER_AAC_LTP, AOT_ER_AAC_LD,
           AOT_ER_AAC_SCAL (chapter 6.6.4.1.2.1.1) */
        if((self->m_channelConfiguration == 0) &&
           ((self->m_aot == AOT_ER_AAC_LC) || (self->m_aot == AOT_ER_AAC_LTP) || (self->m_aot == AOT_ER_AAC_LD) ||
            (self->m_aot == AOT_ER_AAC_SCAL) || (self->m_aot == AOT_ER_AAC_ELD))) {
            return TRANSPORTDEC_UNSUPPORTED_FORMAT;
        }
        /* MPEG-04 conformance ISO/IEC 14496-4: channelConfiguration > 2 is not
         * permitted for AOT_AAC_SCAL and AOT_ER_AAC_SCAL (chapter 6.6.4.1.2.1.1) */
        if((self->m_channelConfiguration > 2) && ((self->m_aot == AOT_AAC_SCAL) || (self->m_aot == AOT_ER_AAC_SCAL))) {
            return TRANSPORTDEC_UNSUPPORTED_FORMAT;
        }

        /* SBR extension ( explicit non-backwards compatible mode ) */
        self->m_sbrPresentFlag = 0;
        self->m_psPresentFlag = 0;

        if(self->m_aot == AOT_SBR || self->m_aot == AOT_PS) {
            self->m_extensionAudioObjectType = AOT_SBR;

            self->m_sbrPresentFlag = 1;
            if(self->m_aot == AOT_PS) { self->m_psPresentFlag = 1; }

            self->m_extensionSamplingFrequency = getSampleRate(bs, &self->m_extensionSamplingFrequencyIndex, 4);
            if(self->m_extensionSamplingFrequency == 0 || self->m_extensionSamplingFrequency > 96000) {
                return TRANSPORTDEC_PARSE_ERROR;
            }
            self->m_aot = getAOT(bs);

            switch(self->m_aot) {
                case AOT_AAC_LC:
                    break;
                case AOT_ER_BSAC:
                    break;
                default:
                    return TRANSPORTDEC_UNSUPPORTED_FORMAT;
            }

            if(self->m_aot == AOT_ER_BSAC) { self->m_extensionChannelConfiguration = FDKreadBits(bs, 4); }
        }
        else { self->m_extensionAudioObjectType = AOT_NULL_OBJECT; }
    }

    /* Parse whatever specific configs */
    switch(self->m_aot) {
        case AOT_AAC_LC:
        case AOT_AAC_SCAL:
        case AOT_ER_AAC_LC:
        case AOT_ER_AAC_LD:
        case AOT_ER_AAC_SCAL:
        case AOT_ER_BSAC:
            if((ErrorStatus = GaSpecificConfig_Parse(&self->m_sc.m_gaSpecificConfig, self, bs, ascStartAnchor)) !=
               TRANSPORTDEC_OK) {
                return (ErrorStatus);
            }
            frameLengthFlag = self->m_sc.m_gaSpecificConfig.m_frameLengthFlag;
            break;
        case AOT_MPEGS:
            if(cb->cbSsc != NULL) {
                if(cb->cbSsc(cb->cbSscData, bs, self->m_aot, self->m_samplingFrequency, self->m_samplesPerFrame, 1,
                             -1, /* nTimeSlots: read from bitstream */
                             0,  /* don't know the length */
                             self->configMode, &self->SacConfigChanged)) {
                    return TRANSPORTDEC_UNSUPPORTED_FORMAT;
                }
            }
            else { return TRANSPORTDEC_UNSUPPORTED_FORMAT; }
            break;
        case AOT_ER_AAC_ELD:
            if((ErrorStatus = EldSpecificConfig_Parse(self, bs, cb)) != TRANSPORTDEC_OK) { return (ErrorStatus); }
            frameLengthFlag = self->m_sc.m_eldSpecificConfig.m_frameLengthFlag;
            self->m_sbrPresentFlag = self->m_sc.m_eldSpecificConfig.m_sbrPresentFlag;
            self->m_extensionSamplingFrequency =
                (self->m_sc.m_eldSpecificConfig.m_sbrSamplingRate + 1) * self->m_samplingFrequency;
            break;
        case AOT_USAC:
            if((ErrorStatus = UsacConfig_Parse(self, bs, cb)) != TRANSPORTDEC_OK) { return (ErrorStatus); }
            break;

        default:
            return TRANSPORTDEC_UNSUPPORTED_FORMAT;
    }

    /* Frame length */
    switch(self->m_aot) {
        case AOT_AAC_LC:
        case AOT_AAC_SCAL:
        case AOT_ER_AAC_LC:
        case AOT_ER_AAC_SCAL:
        case AOT_ER_BSAC:
            /*case AOT_USAC:*/
            if(!frameLengthFlag) self->m_samplesPerFrame = 1024;
            else
                self->m_samplesPerFrame = 960;
            break;
        case AOT_ER_AAC_LD:
            if(!frameLengthFlag) self->m_samplesPerFrame = 512;
            else
                self->m_samplesPerFrame = 480;
            break;
        default:
            break;
    }

    switch(self->m_aot) {
        case AOT_ER_AAC_LC:
        case AOT_ER_AAC_LD:
        case AOT_ER_AAC_ELD:
        case AOT_ER_AAC_SCAL:
        case AOT_ER_CELP:
        case AOT_ER_HVXC:
        case AOT_ER_BSAC:
            self->m_epConfig = FDKreadBits(bs, 2);

            if(self->m_epConfig > 1) {
                return TRANSPORTDEC_UNSUPPORTED_FORMAT;  // EPCONFIG;
            }
            break;
        default:
            break;
    }

    if(fExplicitBackwardCompatible &&
       (self->m_aot == AOT_AAC_LC || self->m_aot == AOT_ER_AAC_LD || self->m_aot == AOT_ER_BSAC)) {
        ErrorStatus = AudioSpecificConfig_ExtensionParse(self, bs, cb);
    }

    /* Copy config() to asc->config[] buffer. */
    if((ErrorStatus == TRANSPORTDEC_OK) && (self->m_aot == AOT_USAC)) {
        int32_t configSize_bits = (int32_t)FDKgetValidBits(bs) - (int32_t)ascStartAnchor;
        StoreConfigAsBitstream(bs, configSize_bits, self->config, TP_USAC_MAX_CONFIG_LEN);
        self->configBits = fAbs(configSize_bits);
    }

    return (ErrorStatus);
}

static TRANSPORTDEC_ERROR_t Drm_xHEAACDecoderConfig(
    CSAudioSpecificConfig_t *asc, HANDLE_FDK_BITSTREAM hBs, int32_t audioMode,
    CSTpCallBacks_t *cb /* use cb == NULL to signal config check only mode */
) {
    TRANSPORTDEC_ERROR_t ErrorStatus = TRANSPORTDEC_OK;
    CSUsacConfig_t      *usc = &asc->m_sc.m_usacConfig;
    int32_t            elemIdx = 0;

    usc->element[elemIdx].m_stereoConfigIndex = 0;

    usc->m_usacNumElements = 1; /* Currently all extension elements are skipped
                                   -> only one SCE or CPE. */

    switch(audioMode) {
        case 0: /* mono: ID_USAC_SCE */
            usc->element[elemIdx].usacElementType = ID_USAC_SCE;
            usc->m_nUsacChannels = 1;
            usc->element[elemIdx].m_noiseFilling = FDKreadBits(hBs, 1);
            if(usc->m_sbrRatioIndex > 0) {
                if(cb == NULL) { return ErrorStatus; }
                if(cb->cbSbr != NULL) {
                    usc->element[elemIdx].m_harmonicSBR = FDKreadBit(hBs);
                    usc->element[elemIdx].m_interTes = FDKreadBit(hBs);
                    usc->element[elemIdx].m_pvc = FDKreadBit(hBs);
                    if(cb->cbSbr(cb->cbSbrData, hBs, asc->m_samplingFrequency, asc->m_extensionSamplingFrequency,
                                 asc->m_samplesPerFrame, asc->m_aot, ID_SCE, elemIdx,
                                 usc->element[elemIdx].m_harmonicSBR, usc->element[elemIdx].m_stereoConfigIndex,
                                 asc->configMode, &asc->SbrConfigChanged, 1)) {
                        return ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
                    }
                }
            }
            break;
        case 2: /* stereo: ID_USAC_CPE */
            usc->element[elemIdx].usacElementType = ID_USAC_CPE;
            usc->m_nUsacChannels = 2;
            usc->element[elemIdx].m_noiseFilling = FDKreadBits(hBs, 1);
            if(usc->m_sbrRatioIndex > 0) {
                usc->element[elemIdx].m_harmonicSBR = FDKreadBit(hBs);
                usc->element[elemIdx].m_interTes = FDKreadBit(hBs);
                usc->element[elemIdx].m_pvc = FDKreadBit(hBs);
                {
                    int32_t bitsToSkip = skipSbrHeader(hBs, 1);
                    /* read stereoConfigIndex */
                    usc->element[elemIdx].m_stereoConfigIndex = FDKreadBits(hBs, 2);
                    /* rewind */
                    FDKpushBack(hBs, bitsToSkip + 2);
                }
                /*
                The application of the following tools is mutually exclusive per audio
                stream configuration (see clause 5.3.2, xHE-AAC codec configuration):
                - MPS212 parametric stereo tool with residual coding
                (stereoConfigIndex>1); and
                - QMF based Harmonic Transposer (harmonicSBR==1).
                */
                if((usc->element[elemIdx].m_stereoConfigIndex > 1) && usc->element[elemIdx].m_harmonicSBR) {
                    return ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
                }
                /*
                The 4:1 sbrRatio (sbrRatioIndex==1 in [11]) may only be employed:
                - in mono operation; or
                - in stereo operation if parametric stereo (MPS212) without residual
                coding is applied, i.e. if stereoConfigIndex==1 (see clause 5.3.2,
                xHE-AAC codec configuration).
                */
                if((usc->m_sbrRatioIndex == 1) && (usc->element[elemIdx].m_stereoConfigIndex != 1)) {
                    return ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
                }
                if(cb == NULL) { return ErrorStatus; }
                {
                    MP4_ELEMENT_ID_t el_type = (usc->element[elemIdx].m_stereoConfigIndex == 1 ||
                                              usc->element[elemIdx].m_stereoConfigIndex == 2)
                                                 ? ID_SCE
                                                 : ID_CPE;
                    if(cb->cbSbr == NULL) return ErrorStatus = TRANSPORTDEC_UNKOWN_ERROR;
                    if(cb->cbSbr(cb->cbSbrData, hBs, asc->m_samplingFrequency, asc->m_extensionSamplingFrequency,
                                 asc->m_samplesPerFrame, asc->m_aot, el_type, elemIdx,
                                 usc->element[elemIdx].m_harmonicSBR, usc->element[elemIdx].m_stereoConfigIndex,
                                 asc->configMode, &asc->SbrConfigChanged, 1)) {
                        return ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
                    }
                }
                /*usc->element[elemIdx].m_stereoConfigIndex =*/FDKreadBits(hBs, 2);
                if(usc->element[elemIdx].m_stereoConfigIndex > 0) {
                    if(cb->cbSsc != NULL) {
                        int32_t samplesPerFrame = asc->m_samplesPerFrame;

                        if(usc->m_sbrRatioIndex == 1) samplesPerFrame <<= 2;
                        if(usc->m_sbrRatioIndex == 2) samplesPerFrame = (samplesPerFrame * 8) / 3;
                        if(usc->m_sbrRatioIndex == 3) samplesPerFrame <<= 1;

                        ErrorStatus = (TRANSPORTDEC_ERROR_t)cb->cbSsc(
                            cb->cbSscData, hBs, AOT_DRM_USAC, /* syntax differs from MPEG Mps212Config() */
                            asc->m_extensionSamplingFrequency, samplesPerFrame,
                            usc->element[elemIdx].m_stereoConfigIndex, usc->m_coreSbrFrameLengthIndex,
                            0, /* don't know the length */
                            asc->configMode, &asc->SacConfigChanged);
                    }
                    else { /* ErrorStatus = TRANSPORTDEC_UNSUPPORTED_FORMAT; */ }
                }
            }
            break;
        default:
            return TRANSPORTDEC_PARSE_ERROR;
    }

    return ErrorStatus;
}

TRANSPORTDEC_ERROR_t Drm_xHEAACStaticConfig(CSAudioSpecificConfig_t *asc, HANDLE_FDK_BITSTREAM bs, int32_t audioMode,
                                          CSTpCallBacks_t *cb /* use cb == NULL to signal config check only mode */
) {
    int32_t coreSbrFrameLengthIndexDrm = FDKreadBits(bs, 2);
    if(UsacConfig_SetCoreSbrFrameLengthIndex(asc, coreSbrFrameLengthIndexDrm + 1) != TRANSPORTDEC_OK) {
        return TRANSPORTDEC_PARSE_ERROR;
    }

    asc->m_channelConfiguration = (audioMode) ? 2 : 1;

    if(Drm_xHEAACDecoderConfig(asc, bs, audioMode, cb) != TRANSPORTDEC_OK) { return TRANSPORTDEC_PARSE_ERROR; }

    return TRANSPORTDEC_OK;
}

/* Mapping of DRM audio sampling rate field to MPEG usacSamplingFrequencyIndex
 */
const uint8_t mapSr2MPEGIdx[8] = {
    0x1b, /*  9.6 kHz */
    0x09, /* 12.0 kHz */
    0x08, /* 16.0 kHz */
    0x17, /* 19.2 kHz */
    0x06, /* 24.0 kHz */
    0x05, /* 32.0 kHz */
    0x12, /* 38.4 kHz */
    0x03  /* 48.0 kHz */
};

TRANSPORTDEC_ERROR_t DrmRawSdcAudioConfig_Parse(CSAudioSpecificConfig_t *self, HANDLE_FDK_BITSTREAM bs,
                                              CSTpCallBacks_t *cb, /* use cb == NULL to signal config check only mode */
                                              uint8_t configMode, uint8_t configChanged) {
    TRANSPORTDEC_ERROR_t ErrorStatus = TRANSPORTDEC_OK;

    AudioSpecificConfig_Init(self);

    if((int32_t)FDKgetValidBits(bs) < 16) {
        ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
        goto bail;
    }
    else {
        /* DRM - Audio information data entity - type 9
           - int16_t Id            2 bits (not part of the config buffer)
           - Stream Id           2 bits (not part of the config buffer)
           - audio coding        2 bits
           - SBR flag            1 bit
           - audio mode          2 bits
           - audio sampling rate 3 bits
           - text flag           1 bit
           - enhancement flag    1 bit
           - coder field         5 bits
           - rfa                 1 bit  */

        int32_t audioCoding, audioMode, cSamplingFreq, coderField, sfIdx, sbrFlag;

        self->configMode = configMode;
        self->AacConfigChanged = configChanged;
        self->SbrConfigChanged = configChanged;
        self->SacConfigChanged = configChanged;

        /* Read the SDC field */
        audioCoding = FDKreadBits(bs, 2);
        sbrFlag = FDKreadBits(bs, 1);
        audioMode = FDKreadBits(bs, 2);
        cSamplingFreq = FDKreadBits(bs, 3); /* audio sampling rate */

        FDKreadBits(bs, 2); /* Text and enhancement flag */
        coderField = FDKreadBits(bs, 5);
        FDKreadBits(bs, 1); /* rfa */

        /* Evaluate configuration and fill the ASC */
        if(audioCoding == 3) {
            sfIdx = (int32_t)mapSr2MPEGIdx[cSamplingFreq];
            sbrFlag = 0; /* rfa */
        }
        else {
            switch(cSamplingFreq) {
                case 0: /*  8 kHz */
                    sfIdx = 11;
                    break;
                case 1: /* 12 kHz */
                    sfIdx = 9;
                    break;
                case 2: /* 16 kHz */
                    sfIdx = 8;
                    break;
                case 3: /* 24 kHz */
                    sfIdx = 6;
                    break;
                case 5: /* 48 kHz */
                    sfIdx = 3;
                    break;
                case 4: /* reserved */
                case 6: /* reserved */
                case 7: /* reserved */
                default:
                    ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
                    goto bail;
            }
        }

        self->m_samplingFrequencyIndex = sfIdx;
        self->m_samplingFrequency = SamplingRateTable[sfIdx];

        if(sbrFlag) {
            uint32_t i;
            int32_t  tmp = -1;
            self->m_sbrPresentFlag = 1;
            self->m_extensionAudioObjectType = AOT_SBR;
            self->m_extensionSamplingFrequency = self->m_samplingFrequency << 1;
            for(i = 0; i < (sizeof(SamplingRateTable) / sizeof(SamplingRateTable[0])); i++) {
                if(SamplingRateTable[i] == self->m_extensionSamplingFrequency) {
                    tmp = i;
                    break;
                }
            }
            self->m_extensionSamplingFrequencyIndex = tmp;
        }

        switch(audioCoding) {
            case 0: /* AAC */
                if((coderField >> 2) && (audioMode != 1)) {
                    self->m_aot = AOT_DRM_SURROUND; /* Set pseudo AOT for Drm Surround */
                }
                else { self->m_aot = AOT_DRM_AAC; /* Set pseudo AOT for Drm AAC */ }
                switch(audioMode) {
                    case 1: /* parametric stereo */
                        self->m_psPresentFlag = 1;
                        /* fall through */;
                    case 0: /* mono */
                        self->m_channelConfiguration = 1;
                        break;
                    case 2: /* stereo */
                        self->m_channelConfiguration = 2;
                        break;
                    default:
                        ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
                        goto bail;
                }
                self->m_vcb11Flag = 1;
                self->m_hcrFlag = 1;
                self->m_samplesPerFrame = 960;
                self->m_epConfig = 1;
                break;
            case 1: /* CELP */
                self->m_aot = AOT_ER_CELP;
                self->m_channelConfiguration = 1;
                break;
            case 2: /* HVXC */
                self->m_aot = AOT_ER_HVXC;
                self->m_channelConfiguration = 1;
                break;
            case 3: /* xHE-AAC */
            {
                /* payload is MPEG conform -> no pseudo DRM AOT needed */
                self->m_aot = AOT_USAC;
            }
                switch(audioMode) {
                    case 0: /* mono */
                    case 2: /* stereo */
                        /* codec specific config 8n bits */
                        ErrorStatus = Drm_xHEAACStaticConfig(self, bs, audioMode, cb);
                        break;
                    default:
                        ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
                        goto bail;
                }
                break;
            default:
                ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
                self->m_aot = AOT_NONE;
                break;
        }

        if(self->m_psPresentFlag && !self->m_sbrPresentFlag) {
            ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
            goto bail;
        }
    }

bail:
    return (ErrorStatus);
}
