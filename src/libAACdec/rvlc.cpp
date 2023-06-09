

/**************************** AAC decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  RVLC Decoder
  \author Robert Weidner
*/
#include "newAACDecoder.h"
#include "block.h"

#include "aac_rom.h"



/*---------------------------------------------------------------------------------------------
     function:     rvlcInit

     description:  init RVLC by data from channelinfo, which was decoded
previously and set up pointers
-----------------------------------------------------------------------------------------------
        input:     - pointer rvlc structure
                   - pointer channel info structure
                   - pointer bitstream structure
-----------------------------------------------------------------------------------------------
        return:    -
--------------------------------------------------------------------------------------------
*/

static void rvlcInit(CErRvlcInfo_t *pRvlc,
                     CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                     HANDLE_FDK_BITSTREAM bs) {
  /* RVLC common initialization part 2 of 2 */
  int16_t *pScfEsc = pAacDecoderChannelInfo->pComData->overlay.aac.aRvlcScfEsc;
  int16_t *pScfFwd = pAacDecoderChannelInfo->pComData->overlay.aac.aRvlcScfFwd;
  int16_t *pScfBwd = pAacDecoderChannelInfo->pComData->overlay.aac.aRvlcScfBwd;
  int16_t *pScaleFactor = pAacDecoderChannelInfo->pDynData->aScaleFactor;
  int32_t bnds;

  pAacDecoderChannelInfo->pDynData->specificTo.aac.rvlcIntensityUsed = 0;

  pRvlc->numDecodedEscapeWordsEsc = 0;
  pRvlc->numDecodedEscapeWordsFwd = 0;
  pRvlc->numDecodedEscapeWordsBwd = 0;

  pRvlc->intensity_used = 0;
  pRvlc->errorLogRvlc = 0;

  pRvlc->conceal_max = CONCEAL_MAX_INIT;
  pRvlc->conceal_min = CONCEAL_MIN_INIT;

  pRvlc->conceal_max_esc = CONCEAL_MAX_INIT;
  pRvlc->conceal_min_esc = CONCEAL_MIN_INIT;

  pRvlc->pHuffTreeRvlcEscape = aHuffTreeRvlcEscape;
  pRvlc->pHuffTreeRvlCodewds = aHuffTreeRvlCodewds;

  /* init scf arrays (for savety (in case of there are only zero codebooks)) */
  for (bnds = 0; bnds < RVLC_MAX_SFB; bnds++) {
    pScfFwd[bnds] = 0;
    pScfBwd[bnds] = 0;
    pScfEsc[bnds] = 0;
    pScaleFactor[bnds] = 0;
  }

  /* set base bitstream ptr to the RVL-coded part (start of RVLC data (ESC 2))
   */
  FDKsyncCache(bs);
  pRvlc->bsAnchor = (int32_t)FDKgetValidBits(bs);

  pRvlc->bitstreamIndexRvlFwd =
      0; /* first bit within RVL coded block as start address for  forward
            decoding */
  pRvlc->bitstreamIndexRvlBwd =
      pRvlc->length_of_rvlc_sf - 1; /* last bit within RVL coded block as start
                                       address for backward decoding */

  /* skip RVLC-bitstream-part -- pointing now to escapes (if present) or to TNS
   * data (if present) */
  FDKpushFor(bs, pRvlc->length_of_rvlc_sf);

  if (pRvlc->sf_escapes_present != 0) {
    /* locate internal bitstream ptr at escapes (which is the second part) */
    FDKsyncCache(bs);
    pRvlc->bitstreamIndexEsc = pRvlc->bsAnchor - (int32_t)FDKgetValidBits(bs);

    /* skip escapeRVLC-bitstream-part -- pointing to TNS data (if present)   to
     * make decoder continue */
    /* decoding of RVLC should work despite this second pushFor during
     * initialization because        */
    /* bitstream initialization is valid for both ESC2 data parts (RVL-coded
     * values and ESC-coded values) */
    FDKpushFor(bs, pRvlc->length_of_rvlc_escapes);
  }
}

/*---------------------------------------------------------------------------------------------
     function:     rvlcCheckIntensityCb

     description:  Check if a intensity codebook is used in the current channel.
-----------------------------------------------------------------------------------------------
        input:     - pointer rvlc structure
                   - pointer channel info structure
-----------------------------------------------------------------------------------------------
        output:    - intensity_used: 0 no intensity codebook is used
                                     1 intensity codebook is used
-----------------------------------------------------------------------------------------------
        return:    -
--------------------------------------------------------------------------------------------
*/

static void rvlcCheckIntensityCb(
    CErRvlcInfo_t *pRvlc, CAacDecoderChannelInfo_t *pAacDecoderChannelInfo) {
  int32_t group, band, bnds;

  pRvlc->intensity_used = 0;

  for (group = 0; group < pRvlc->numWindowGroups; group++) {
    for (band = 0; band < pRvlc->maxSfbTransmitted; band++) {
      bnds = 16 * group + band;
      if ((pAacDecoderChannelInfo->pDynData->aCodeBook[bnds] ==
           INTENSITY_HCB) ||
          (pAacDecoderChannelInfo->pDynData->aCodeBook[bnds] ==
           INTENSITY_HCB2)) {
        pRvlc->intensity_used = 1;
        break;
      }
    }
  }
}

/*---------------------------------------------------------------------------------------------
     function:     rvlcDecodeEscapeWord

     description:  Decode a huffman coded RVLC Escape-word. This value is part
of a DPCM coded scalefactor.
-----------------------------------------------------------------------------------------------
        input:     - pointer rvlc structure
-----------------------------------------------------------------------------------------------
        return:    - a single RVLC-Escape value which had to be applied to a
DPCM value (which has a absolute value of 7)
--------------------------------------------------------------------------------------------
*/

static int8_t rvlcDecodeEscapeWord(CErRvlcInfo_t *pRvlc, HANDLE_FDK_BITSTREAM bs) {
  int32_t i;
  int8_t value;
  uint8_t carryBit;
  uint32_t treeNode;
  uint32_t branchValue;
  uint32_t branchNode;

  int32_t *pBitstreamIndexEsc;
  const uint32_t *pEscTree;

  pEscTree = pRvlc->pHuffTreeRvlcEscape;
  pBitstreamIndexEsc = &(pRvlc->bitstreamIndexEsc);
  treeNode = *pEscTree; /* init at starting node */

  for (i = MAX_LEN_RVLC_ESCAPE_WORD - 1; i >= 0; i--) {
    carryBit =
        rvlcReadBitFromBitstream(bs, /* get next bit */
                                 pRvlc->bsAnchor, pBitstreamIndexEsc, FWD);

    CarryBitToBranchValue(carryBit, /* huffman decoding, do a single step in
                                       huffman decoding tree */
                          treeNode, &branchValue, &branchNode);

    if ((branchNode & TEST_BIT_10) ==
        TEST_BIT_10) { /* test bit 10 ; if set --> a RVLC-escape-word is
                          completely decoded */
      value = (int8_t)branchNode & CLR_BIT_10;
      pRvlc->length_of_rvlc_escapes -= (MAX_LEN_RVLC_ESCAPE_WORD - i);

      if (pRvlc->length_of_rvlc_escapes < 0) {
        pRvlc->errorLogRvlc |= RVLC_ERROR_ALL_ESCAPE_WORDS_INVALID;
        value = -1;
      }

      return value;
    } else {
      treeNode = *(
          pEscTree +
          branchValue); /* update treeNode for further step in decoding tree */
    }
  }

  pRvlc->errorLogRvlc |= RVLC_ERROR_ALL_ESCAPE_WORDS_INVALID;

  return -1; /* should not be reached */
}

/*---------------------------------------------------------------------------------------------
     function:     rvlcDecodeEscapes

     description:  Decodes all huffman coded RVLC Escape Words.
                   Here a difference to the pseudo-code-implementation from
standard can be found. A while loop (and not two nested for loops) is used for
two reasons:

                   1. The plain huffman encoded escapes are decoded before the
RVL-coded scalefactors. Therefore the escapes are present in the second step
                      when decoding the RVL-coded-scalefactor values in forward
and backward direction.

                      When the RVL-coded scalefactors are decoded and there a
escape is needed, then it is just taken out of the array in ascending order.

                   2. It's faster.
-----------------------------------------------------------------------------------------------
        input:     - pointer rvlc structure
                   - handle to FDK bitstream
-----------------------------------------------------------------------------------------------
        return:    - 0 ok     the decoded escapes seem to be valid
                   - 1 error  there was a error detected during decoding escapes
                              --> all escapes are invalid
--------------------------------------------------------------------------------------------
*/

static void rvlcDecodeEscapes(CErRvlcInfo_t *pRvlc, int16_t *pEsc,
                              HANDLE_FDK_BITSTREAM bs) {
  int8_t escWord;
  int8_t escCnt = 0;
  int16_t *pEscBitCntSum;

  pEscBitCntSum = &(pRvlc->length_of_rvlc_escapes);

  /* Decode all RVLC-Escape words with a plain Huffman-Decoder */
  while (*pEscBitCntSum > 0) {
    escWord = rvlcDecodeEscapeWord(pRvlc, bs);

    if (escWord >= 0) {
      pEsc[escCnt] = escWord;
      escCnt++;
    } else {
      pRvlc->errorLogRvlc |= RVLC_ERROR_ALL_ESCAPE_WORDS_INVALID;
      pRvlc->numDecodedEscapeWordsEsc = escCnt;

      return;
    }
  } /* all RVLC escapes decoded */

  pRvlc->numDecodedEscapeWordsEsc = escCnt;
}

/*---------------------------------------------------------------------------------------------
     function:     decodeRVLCodeword

     description:  Decodes a RVL-coded dpcm-word (-part).
-----------------------------------------------------------------------------------------------
        input:     - FDK bitstream handle
                   - pointer rvlc structure
-----------------------------------------------------------------------------------------------
        return:    - a dpcm value which is within range [0,1,..,14] in case of
no errors. The offset of 7 must be subtracted to get a valid dpcm scalefactor
value. In case of errors a forbidden codeword is detected --> returning -1
--------------------------------------------------------------------------------------------
*/

int8_t decodeRVLCodeword(HANDLE_FDK_BITSTREAM bs, CErRvlcInfo_t *pRvlc) {
  int32_t i;
  int8_t value;
  uint8_t carryBit;
  uint32_t branchValue;
  uint32_t branchNode;

  const uint32_t *pRvlCodeTree = pRvlc->pHuffTreeRvlCodewds;
  uint8_t direction = pRvlc->direction;
  int32_t *pBitstrIndxRvl = pRvlc->pBitstrIndxRvl_RVL;
  uint32_t treeNode = *pRvlCodeTree;

  for (i = MAX_LEN_RVLC_CODE_WORD - 1; i >= 0; i--) {
    carryBit =
        rvlcReadBitFromBitstream(bs, /* get next bit */
                                 pRvlc->bsAnchor, pBitstrIndxRvl, direction);

    CarryBitToBranchValue(carryBit, /* huffman decoding, do a single step in
                                       huffman decoding tree */
                          treeNode, &branchValue, &branchNode);

    if ((branchNode & TEST_BIT_10) ==
        TEST_BIT_10) { /* test bit 10 ; if set --> a
                          RVLC-codeword is completely decoded
                        */
      value = (int8_t)(branchNode & CLR_BIT_10);
      *pRvlc->pRvlBitCnt_RVL -= (MAX_LEN_RVLC_CODE_WORD - i);

      /* check available bits for decoding */
      if (*pRvlc->pRvlBitCnt_RVL < 0) {
        if (direction == FWD) {
          pRvlc->errorLogRvlc |= RVLC_ERROR_RVL_SUM_BIT_COUNTER_BELOW_ZERO_FWD;
        } else {
          pRvlc->errorLogRvlc |= RVLC_ERROR_RVL_SUM_BIT_COUNTER_BELOW_ZERO_BWD;
        }
        value = -1; /* signalize an error in return value, because too many bits
                       was decoded */
      }

      /* check max value of dpcm value */
      if (value > MAX_ALLOWED_DPCM_INDEX) {
        if (direction == FWD) {
          pRvlc->errorLogRvlc |= RVLC_ERROR_FORBIDDEN_CW_DETECTED_FWD;
        } else {
          pRvlc->errorLogRvlc |= RVLC_ERROR_FORBIDDEN_CW_DETECTED_BWD;
        }
        value = -1; /* signalize an error in return value, because a forbidden
                       cw was detected*/
      }

      return value; /* return a dpcm value with offset +7 or an error status */
    } else {
      treeNode = *(
          pRvlCodeTree +
          branchValue); /* update treeNode for further step in decoding tree */
    }
  }

  return -1;
}

/*---------------------------------------------------------------------------------------------
     function:     rvlcDecodeForward

     description:  Decode RVL-coded codewords in forward direction.
-----------------------------------------------------------------------------------------------
        input:     - pointer rvlc structure
                   - pointer channel info structure
                   - handle to FDK bitstream
-----------------------------------------------------------------------------------------------
        return:    -
--------------------------------------------------------------------------------------------
*/

static void rvlcDecodeForward(CErRvlcInfo_t *pRvlc,
                              CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                              HANDLE_FDK_BITSTREAM bs) {
  int32_t band = 0;
  int32_t group = 0;
  int32_t bnds = 0;

  int16_t dpcm;

  int16_t factor =
      pAacDecoderChannelInfo->pDynData->RawDataInfo.GlobalGain - SF_OFFSET;
  int16_t position = -SF_OFFSET;
  int16_t noisenrg = pAacDecoderChannelInfo->pDynData->RawDataInfo.GlobalGain -
                   SF_OFFSET - 90 - 256;

  int16_t *pScfFwd = pAacDecoderChannelInfo->pComData->overlay.aac.aRvlcScfFwd;
  int16_t *pScfEsc = pAacDecoderChannelInfo->pComData->overlay.aac.aRvlcScfEsc;
  uint8_t *pEscFwdCnt = &(pRvlc->numDecodedEscapeWordsFwd);

  pRvlc->pRvlBitCnt_RVL = &(pRvlc->length_of_rvlc_sf_fwd);
  pRvlc->pBitstrIndxRvl_RVL = &(pRvlc->bitstreamIndexRvlFwd);

  *pEscFwdCnt = 0;
  pRvlc->direction = FWD;
  pRvlc->noise_used = 0;
  pRvlc->sf_used = 0;
  pRvlc->lastScf = 0;
  pRvlc->lastNrg = 0;
  pRvlc->lastIs = 0;

  rvlcCheckIntensityCb(pRvlc, pAacDecoderChannelInfo);

  /* main loop fwd int32_t */
  for (group = 0; group < pRvlc->numWindowGroups; group++) {
    for (band = 0; band < pRvlc->maxSfbTransmitted; band++) {
      bnds = 16 * group + band;

      switch (pAacDecoderChannelInfo->pDynData->aCodeBook[bnds]) {
        case ZERO_HCB:
          pScfFwd[bnds] = 0;
          break;

        case INTENSITY_HCB2:
        case INTENSITY_HCB:
          /* store dpcm_is_position */
          dpcm = decodeRVLCodeword(bs, pRvlc);
          if (dpcm < 0) {
            pRvlc->conceal_max = bnds;
            return;
          }
          dpcm -= TABLE_OFFSET;
          if ((dpcm == MIN_RVL) || (dpcm == MAX_RVL)) {
            if (pRvlc->length_of_rvlc_escapes) {
              pRvlc->conceal_max = bnds;
              return;
            } else {
              if (dpcm == MIN_RVL) {
                dpcm -= *pScfEsc++;
              } else {
                dpcm += *pScfEsc++;
              }
              (*pEscFwdCnt)++;
              if (pRvlc->conceal_max_esc == CONCEAL_MAX_INIT) {
                pRvlc->conceal_max_esc = bnds;
              }
            }
          }
          position += dpcm;
          pScfFwd[bnds] = position;
          pRvlc->lastIs = position;
          break;

        case NOISE_HCB:
          if (pRvlc->noise_used == 0) {
            pRvlc->noise_used = 1;
            pRvlc->first_noise_band = bnds;
            noisenrg += pRvlc->dpcm_noise_nrg;
            pScfFwd[bnds] = 100 + noisenrg;
            pRvlc->lastNrg = noisenrg;
          } else {
            dpcm = decodeRVLCodeword(bs, pRvlc);
            if (dpcm < 0) {
              pRvlc->conceal_max = bnds;
              return;
            }
            dpcm -= TABLE_OFFSET;
            if ((dpcm == MIN_RVL) || (dpcm == MAX_RVL)) {
              if (pRvlc->length_of_rvlc_escapes) {
                pRvlc->conceal_max = bnds;
                return;
              } else {
                if (dpcm == MIN_RVL) {
                  dpcm -= *pScfEsc++;
                } else {
                  dpcm += *pScfEsc++;
                }
                (*pEscFwdCnt)++;
                if (pRvlc->conceal_max_esc == CONCEAL_MAX_INIT) {
                  pRvlc->conceal_max_esc = bnds;
                }
              }
            }
            noisenrg += dpcm;
            pScfFwd[bnds] = 100 + noisenrg;
            pRvlc->lastNrg = noisenrg;
          }
          pAacDecoderChannelInfo->data.aac.PnsData.pnsUsed[bnds] = 1;
          break;

        default:
          pRvlc->sf_used = 1;
          dpcm = decodeRVLCodeword(bs, pRvlc);
          if (dpcm < 0) {
            pRvlc->conceal_max = bnds;
            return;
          }
          dpcm -= TABLE_OFFSET;
          if ((dpcm == MIN_RVL) || (dpcm == MAX_RVL)) {
            if (pRvlc->length_of_rvlc_escapes) {
              pRvlc->conceal_max = bnds;
              return;
            } else {
              if (dpcm == MIN_RVL) {
                dpcm -= *pScfEsc++;
              } else {
                dpcm += *pScfEsc++;
              }
              (*pEscFwdCnt)++;
              if (pRvlc->conceal_max_esc == CONCEAL_MAX_INIT) {
                pRvlc->conceal_max_esc = bnds;
              }
            }
          }
          factor += dpcm;
          pScfFwd[bnds] = factor;
          pRvlc->lastScf = factor;
          break;
      }
    }
  }

  /* postfetch fwd int32_t */
  if (pRvlc->intensity_used) {
    dpcm = decodeRVLCodeword(bs, pRvlc); /* dpcm_is_last_position */
    if (dpcm < 0) {
      pRvlc->conceal_max = bnds;
      return;
    }
    dpcm -= TABLE_OFFSET;
    if ((dpcm == MIN_RVL) || (dpcm == MAX_RVL)) {
      if (pRvlc->length_of_rvlc_escapes) {
        pRvlc->conceal_max = bnds;
        return;
      } else {
        if (dpcm == MIN_RVL) {
          dpcm -= *pScfEsc++;
        } else {
          dpcm += *pScfEsc++;
        }
        (*pEscFwdCnt)++;
        if (pRvlc->conceal_max_esc == CONCEAL_MAX_INIT) {
          pRvlc->conceal_max_esc = bnds;
        }
      }
    }
    pRvlc->dpcm_is_last_position = dpcm;
  }
}

/*---------------------------------------------------------------------------------------------
     function:     rvlcDecodeBackward

     description:  Decode RVL-coded codewords in backward direction.
-----------------------------------------------------------------------------------------------
        input:     - pointer rvlc structure
                   - pointer channel info structure
                   - handle FDK bitstream
-----------------------------------------------------------------------------------------------
        return:    -
--------------------------------------------------------------------------------------------
*/

static void rvlcDecodeBackward(CErRvlcInfo_t *pRvlc,
                               CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                               HANDLE_FDK_BITSTREAM bs) {
  int16_t band, group, dpcm, offset;
  int16_t bnds = pRvlc->maxSfbTransmitted - 1;

  int16_t factor = pRvlc->rev_global_gain - SF_OFFSET;
  int16_t position = pRvlc->dpcm_is_last_position - SF_OFFSET;
  int16_t noisenrg = pRvlc->rev_global_gain + pRvlc->dpcm_noise_last_position -
                   SF_OFFSET - 90 - 256;

  int16_t *pScfBwd = pAacDecoderChannelInfo->pComData->overlay.aac.aRvlcScfBwd;
  int16_t *pScfEsc = pAacDecoderChannelInfo->pComData->overlay.aac.aRvlcScfEsc;
  uint8_t *pEscEscCnt = &(pRvlc->numDecodedEscapeWordsEsc);
  uint8_t *pEscBwdCnt = &(pRvlc->numDecodedEscapeWordsBwd);

  pRvlc->pRvlBitCnt_RVL = &(pRvlc->length_of_rvlc_sf_bwd);
  pRvlc->pBitstrIndxRvl_RVL = &(pRvlc->bitstreamIndexRvlBwd);

  *pEscBwdCnt = 0;
  pRvlc->direction = BWD;
  pScfEsc += *pEscEscCnt - 1; /* set pScfEsc to last entry */
  pRvlc->firstScf = 0;
  pRvlc->firstNrg = 0;
  pRvlc->firstIs = 0;

  /* prefetch int32_t BWD */
  if (pRvlc->intensity_used) {
    dpcm = decodeRVLCodeword(bs, pRvlc); /* dpcm_is_last_position */
    if (dpcm < 0) {
      pRvlc->dpcm_is_last_position = 0;
      pRvlc->conceal_min = bnds;
      return;
    }
    dpcm -= TABLE_OFFSET;
    if ((dpcm == MIN_RVL) || (dpcm == MAX_RVL)) {
      if (pRvlc->length_of_rvlc_escapes) {
        pRvlc->conceal_min = bnds;
        return;
      } else {
        if (dpcm == MIN_RVL) {
          dpcm -= *pScfEsc--;
        } else {
          dpcm += *pScfEsc--;
        }
        (*pEscBwdCnt)++;
        if (pRvlc->conceal_min_esc == CONCEAL_MIN_INIT) {
          pRvlc->conceal_min_esc = bnds;
        }
      }
    }
    pRvlc->dpcm_is_last_position = dpcm;
  }

  /* main loop int32_t BWD */
  for (group = pRvlc->numWindowGroups - 1; group >= 0; group--) {
    for (band = pRvlc->maxSfbTransmitted - 1; band >= 0; band--) {
      bnds = 16 * group + band;
      if ((band == 0) && (pRvlc->numWindowGroups != 1))
        offset = 16 - pRvlc->maxSfbTransmitted + 1;
      else
        offset = 1;

      switch (pAacDecoderChannelInfo->pDynData->aCodeBook[bnds]) {
        case ZERO_HCB:
          pScfBwd[bnds] = 0;
          break;

        case INTENSITY_HCB2:
        case INTENSITY_HCB:
          /* store dpcm_is_position */
          dpcm = decodeRVLCodeword(bs, pRvlc);
          if (dpcm < 0) {
            pScfBwd[bnds] = position;
            pRvlc->conceal_min = fMax(0, bnds - offset);
            return;
          }
          dpcm -= TABLE_OFFSET;
          if ((dpcm == MIN_RVL) || (dpcm == MAX_RVL)) {
            if (pRvlc->length_of_rvlc_escapes) {
              pScfBwd[bnds] = position;
              pRvlc->conceal_min = fMax(0, bnds - offset);
              return;
            } else {
              if (dpcm == MIN_RVL) {
                dpcm -= *pScfEsc--;
              } else {
                dpcm += *pScfEsc--;
              }
              (*pEscBwdCnt)++;
              if (pRvlc->conceal_min_esc == CONCEAL_MIN_INIT) {
                pRvlc->conceal_min_esc = fMax(0, bnds - offset);
              }
            }
          }
          pScfBwd[bnds] = position;
          position -= dpcm;
          pRvlc->firstIs = position;
          break;

        case NOISE_HCB:
          if (bnds == pRvlc->first_noise_band) {
            pScfBwd[bnds] =
                pRvlc->dpcm_noise_nrg +
                pAacDecoderChannelInfo->pDynData->RawDataInfo.GlobalGain -
                SF_OFFSET - 90 - 256;
            pRvlc->firstNrg = pScfBwd[bnds];
          } else {
            dpcm = decodeRVLCodeword(bs, pRvlc);
            if (dpcm < 0) {
              pScfBwd[bnds] = noisenrg;
              pRvlc->conceal_min = fMax(0, bnds - offset);
              return;
            }
            dpcm -= TABLE_OFFSET;
            if ((dpcm == MIN_RVL) || (dpcm == MAX_RVL)) {
              if (pRvlc->length_of_rvlc_escapes) {
                pScfBwd[bnds] = noisenrg;
                pRvlc->conceal_min = fMax(0, bnds - offset);
                return;
              } else {
                if (dpcm == MIN_RVL) {
                  dpcm -= *pScfEsc--;
                } else {
                  dpcm += *pScfEsc--;
                }
                (*pEscBwdCnt)++;
                if (pRvlc->conceal_min_esc == CONCEAL_MIN_INIT) {
                  pRvlc->conceal_min_esc = fMax(0, bnds - offset);
                }
              }
            }
            pScfBwd[bnds] = noisenrg;
            noisenrg -= dpcm;
            pRvlc->firstNrg = noisenrg;
          }
          break;

        default:
          dpcm = decodeRVLCodeword(bs, pRvlc);
          if (dpcm < 0) {
            pScfBwd[bnds] = factor;
            pRvlc->conceal_min = fMax(0, bnds - offset);
            return;
          }
          dpcm -= TABLE_OFFSET;
          if ((dpcm == MIN_RVL) || (dpcm == MAX_RVL)) {
            if (pRvlc->length_of_rvlc_escapes) {
              pScfBwd[bnds] = factor;
              pRvlc->conceal_min = fMax(0, bnds - offset);
              return;
            } else {
              if (dpcm == MIN_RVL) {
                dpcm -= *pScfEsc--;
              } else {
                dpcm += *pScfEsc--;
              }
              (*pEscBwdCnt)++;
              if (pRvlc->conceal_min_esc == CONCEAL_MIN_INIT) {
                pRvlc->conceal_min_esc = fMax(0, bnds - offset);
              }
            }
          }
          pScfBwd[bnds] = factor;
          factor -= dpcm;
          pRvlc->firstScf = factor;
          break;
      }
    }
  }
}

/*---------------------------------------------------------------------------------------------
     function:     rvlcFinalErrorDetection

     description:  Call RVLC concealment if error was detected in decoding
process
-----------------------------------------------------------------------------------------------
        input:     - pointer rvlc structure
                   - pointer channel info structure
-----------------------------------------------------------------------------------------------
        return:    -
--------------------------------------------------------------------------------------------
*/

static void rvlcFinalErrorDetection(
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo) {
  CErRvlcInfo_t *pRvlc =
      &pAacDecoderChannelInfo->pComData->overlay.aac.erRvlcInfo;
  uint8_t ErrorStatusComplete = 0;
  uint8_t ErrorStatusLengthFwd = 0;
  uint8_t ErrorStatusLengthBwd = 0;
  uint8_t ErrorStatusLengthEscapes = 0;
  uint8_t ErrorStatusFirstScf = 0;
  uint8_t ErrorStatusLastScf = 0;
  uint8_t ErrorStatusFirstNrg = 0;
  uint8_t ErrorStatusLastNrg = 0;
  uint8_t ErrorStatusFirstIs = 0;
  uint8_t ErrorStatusLastIs = 0;
  uint8_t ErrorStatusForbiddenCwFwd = 0;
  uint8_t ErrorStatusForbiddenCwBwd = 0;
  uint8_t ErrorStatusNumEscapesFwd = 0;
  uint8_t ErrorStatusNumEscapesBwd = 0;
  uint8_t ConcealStatus = 1;
  uint8_t currentBlockType; /* int16_t: 0, not int16_t: 1*/

  pAacDecoderChannelInfo->pDynData->specificTo.aac.rvlcCurrentScaleFactorOK = 1;

  /* invalid escape words, bit counter unequal zero, forbidden codeword detected
   */
  if (pRvlc->errorLogRvlc & RVLC_ERROR_FORBIDDEN_CW_DETECTED_FWD)
    ErrorStatusForbiddenCwFwd = 1;

  if (pRvlc->errorLogRvlc & RVLC_ERROR_FORBIDDEN_CW_DETECTED_BWD)
    ErrorStatusForbiddenCwBwd = 1;

  /* bit counter forward unequal zero */
  if (pRvlc->length_of_rvlc_sf_fwd) ErrorStatusLengthFwd = 1;

  /* bit counter backward unequal zero */
  if (pRvlc->length_of_rvlc_sf_bwd) ErrorStatusLengthBwd = 1;

  /* bit counter escape sequences unequal zero */
  if (pRvlc->sf_escapes_present)
    if (pRvlc->length_of_rvlc_escapes) ErrorStatusLengthEscapes = 1;

  if (pRvlc->sf_used) {
    /* first decoded scf does not match to global gain in backward direction */
    if (pRvlc->firstScf !=
        (pAacDecoderChannelInfo->pDynData->RawDataInfo.GlobalGain - SF_OFFSET))
      ErrorStatusFirstScf = 1;

    /* last decoded scf does not match to rev global gain in forward direction
     */
    if (pRvlc->lastScf != (pRvlc->rev_global_gain - SF_OFFSET))
      ErrorStatusLastScf = 1;
  }

  if (pRvlc->noise_used) {
    /* first decoded nrg does not match to dpcm_noise_nrg in backward direction
     */
    if (pRvlc->firstNrg !=
        (pAacDecoderChannelInfo->pDynData->RawDataInfo.GlobalGain +
         pRvlc->dpcm_noise_nrg - SF_OFFSET - 90 - 256))
      ErrorStatusFirstNrg = 1;

    /* last decoded nrg does not match to dpcm_noise_last_position in forward
     * direction */
    if (pRvlc->lastNrg !=
        (pRvlc->rev_global_gain + pRvlc->dpcm_noise_last_position - SF_OFFSET -
         90 - 256))
      ErrorStatusLastNrg = 1;
  }

  if (pRvlc->intensity_used) {
    /* first decoded is position does not match in backward direction */
    if (pRvlc->firstIs != (-SF_OFFSET)) ErrorStatusFirstIs = 1;

    /* last decoded is position does not match in forward direction */
    if (pRvlc->lastIs != (pRvlc->dpcm_is_last_position - SF_OFFSET))
      ErrorStatusLastIs = 1;
  }

  /* decoded escapes and used escapes in forward direction do not fit */
  if ((pRvlc->numDecodedEscapeWordsFwd != pRvlc->numDecodedEscapeWordsEsc) &&
      (pRvlc->conceal_max == CONCEAL_MAX_INIT)) {
    ErrorStatusNumEscapesFwd = 1;
  }

  /* decoded escapes and used escapes in backward direction do not fit */
  if ((pRvlc->numDecodedEscapeWordsBwd != pRvlc->numDecodedEscapeWordsEsc) &&
      (pRvlc->conceal_min == CONCEAL_MIN_INIT)) {
    ErrorStatusNumEscapesBwd = 1;
  }

  if (ErrorStatusLengthEscapes ||
      (((pRvlc->conceal_max == CONCEAL_MAX_INIT) &&
        (pRvlc->numDecodedEscapeWordsFwd != pRvlc->numDecodedEscapeWordsEsc) &&
        (ErrorStatusLastScf || ErrorStatusLastNrg || ErrorStatusLastIs))

       &&

       ((pRvlc->conceal_min == CONCEAL_MIN_INIT) &&
        (pRvlc->numDecodedEscapeWordsBwd != pRvlc->numDecodedEscapeWordsEsc) &&
        (ErrorStatusFirstScf || ErrorStatusFirstNrg || ErrorStatusFirstIs))) ||
      ((pRvlc->conceal_max == CONCEAL_MAX_INIT) &&
       ((pRvlc->rev_global_gain - SF_OFFSET - pRvlc->lastScf) < -15)) ||
      ((pRvlc->conceal_min == CONCEAL_MIN_INIT) &&
       ((pAacDecoderChannelInfo->pDynData->RawDataInfo.GlobalGain - SF_OFFSET -
         pRvlc->firstScf) < -15))) {
    if ((pRvlc->conceal_max == CONCEAL_MAX_INIT) ||
        (pRvlc->conceal_min == CONCEAL_MIN_INIT)) {
      pRvlc->conceal_max = 0;
      pRvlc->conceal_min = fMax(
          0, (pRvlc->numWindowGroups - 1) * 16 + pRvlc->maxSfbTransmitted - 1);
    } else {
      pRvlc->conceal_max = fMin(pRvlc->conceal_max, pRvlc->conceal_max_esc);
      pRvlc->conceal_min = fMax(pRvlc->conceal_min, pRvlc->conceal_min_esc);
    }
  }

  ErrorStatusComplete = ErrorStatusLastScf || ErrorStatusFirstScf ||
                        ErrorStatusLastNrg || ErrorStatusFirstNrg ||
                        ErrorStatusLastIs || ErrorStatusFirstIs ||
                        ErrorStatusForbiddenCwFwd ||
                        ErrorStatusForbiddenCwBwd || ErrorStatusLengthFwd ||
                        ErrorStatusLengthBwd || ErrorStatusLengthEscapes ||
                        ErrorStatusNumEscapesFwd || ErrorStatusNumEscapesBwd;

  currentBlockType =
      (GetWindowSequence(&pAacDecoderChannelInfo->icsInfo) == BLOCK_SHORT) ? 0
                                                                           : 1;

  if (!ErrorStatusComplete) {
    int32_t band;
    int32_t group;
    int32_t bnds;
    int32_t lastSfbIndex;

    lastSfbIndex = (pRvlc->numWindowGroups > 1) ? 16 : 64;

    for (group = 0; group < pRvlc->numWindowGroups; group++) {
      for (band = 0; band < pRvlc->maxSfbTransmitted; band++) {
        bnds = 16 * group + band;
        pAacDecoderChannelInfo->pDynData->aScaleFactor[bnds] =
            pAacDecoderStaticChannelInfo->concealmentInfo
                .aRvlcPreviousScaleFactor[bnds] =
                pAacDecoderChannelInfo->pComData->overlay.aac.aRvlcScfFwd[bnds];
      }
    }

    for (group = 0; group < pRvlc->numWindowGroups; group++) {
      for (band = 0; band < pRvlc->maxSfbTransmitted; band++) {
        bnds = 16 * group + band;
        pAacDecoderStaticChannelInfo->concealmentInfo
            .aRvlcPreviousCodebook[bnds] =
            pAacDecoderChannelInfo->pDynData->aCodeBook[bnds];
      }
      for (; band < lastSfbIndex; band++) {
        bnds = 16 * group + band;
        assert(bnds >= 0 && bnds < RVLC_MAX_SFB);
        pAacDecoderStaticChannelInfo->concealmentInfo
            .aRvlcPreviousCodebook[bnds] = ZERO_HCB;
      }
    }
  } else {
    int32_t band;
    int32_t group;

    /* A single bit error was detected in decoding of dpcm values. It also could
       be an error with more bits in decoding of escapes and dpcm values whereby
       an illegal codeword followed not directly after the corrupted bits but
       just after decoding some more (wrong) scalefactors. Use the smaller
       scalefactor from forward decoding, backward decoding and previous frame.
     */
    if (((pRvlc->conceal_min != CONCEAL_MIN_INIT) ||
         (pRvlc->conceal_max != CONCEAL_MAX_INIT)) &&
        (pRvlc->conceal_min <= pRvlc->conceal_max) &&
        (pAacDecoderStaticChannelInfo->concealmentInfo.rvlcPreviousBlockType ==
         currentBlockType) &&
        pAacDecoderStaticChannelInfo->concealmentInfo
            .rvlcPreviousScaleFactorOK &&
        pRvlc->sf_concealment && ConcealStatus) {
      BidirectionalEstimation_UseScfOfPrevFrameAsReference(
          pAacDecoderChannelInfo, pAacDecoderStaticChannelInfo);
      ConcealStatus = 0;
    }

    /* A single bit error was detected in decoding of dpcm values. It also could
       be an error with more bits in decoding of escapes and dpcm values whereby
       an illegal codeword followed not directly after the corrupted bits but
       just after decoding some more (wrong) scalefactors. Use the smaller
       scalefactor from forward and backward decoding. */
    if ((pRvlc->conceal_min <= pRvlc->conceal_max) &&
        ((pRvlc->conceal_min != CONCEAL_MIN_INIT) ||
         (pRvlc->conceal_max != CONCEAL_MAX_INIT)) &&
        !(pAacDecoderStaticChannelInfo->concealmentInfo
              .rvlcPreviousScaleFactorOK &&
          pRvlc->sf_concealment &&
          (pAacDecoderStaticChannelInfo->concealmentInfo
               .rvlcPreviousBlockType == currentBlockType)) &&
        ConcealStatus) {
      BidirectionalEstimation_UseLowerScfOfCurrentFrame(pAacDecoderChannelInfo);
      ConcealStatus = 0;
    }

    /* No errors were detected in decoding of escapes and dpcm values however
       the first and last value of a group (is,nrg,sf) is incorrect */
    if ((pRvlc->conceal_min <= pRvlc->conceal_max) &&
        ((ErrorStatusLastScf && ErrorStatusFirstScf) ||
         (ErrorStatusLastNrg && ErrorStatusFirstNrg) ||
         (ErrorStatusLastIs && ErrorStatusFirstIs)) &&
        !(ErrorStatusForbiddenCwFwd || ErrorStatusForbiddenCwBwd ||
          ErrorStatusLengthEscapes) &&
        ConcealStatus) {
      StatisticalEstimation(pAacDecoderChannelInfo);
      ConcealStatus = 0;
    }

    /* A error with more bits in decoding of escapes and dpcm values was
       detected. Use the smaller scalefactor from forward decoding, backward
       decoding and previous frame. */
    if ((pRvlc->conceal_min <= pRvlc->conceal_max) &&
        pAacDecoderStaticChannelInfo->concealmentInfo
            .rvlcPreviousScaleFactorOK &&
        pRvlc->sf_concealment &&
        (pAacDecoderStaticChannelInfo->concealmentInfo.rvlcPreviousBlockType ==
         currentBlockType) &&
        ConcealStatus) {
      PredictiveInterpolation(pAacDecoderChannelInfo,
                              pAacDecoderStaticChannelInfo);
      ConcealStatus = 0;
    }

    /* Call frame concealment, because no better strategy was found. Setting the
       scalefactors to zero is done for debugging purposes */
    if (ConcealStatus) {
      for (group = 0; group < pRvlc->numWindowGroups; group++) {
        for (band = 0; band < pRvlc->maxSfbTransmitted; band++) {
          pAacDecoderChannelInfo->pDynData->aScaleFactor[16 * group + band] = 0;
        }
      }
      pAacDecoderChannelInfo->pDynData->specificTo.aac
          .rvlcCurrentScaleFactorOK = 0;
    }
  }
}

/*---------------------------------------------------------------------------------------------
     function:     CRvlc_Read

     description:  Read RVLC ESC1 data (side info) from bitstream.
-----------------------------------------------------------------------------------------------
        input:     - pointer rvlc structure
                   - pointer channel info structure
                   - pointer bitstream structure
-----------------------------------------------------------------------------------------------
        return:    -
--------------------------------------------------------------------------------------------
*/

void CRvlc_Read(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                HANDLE_FDK_BITSTREAM bs) {
  CErRvlcInfo_t *pRvlc =
      &pAacDecoderChannelInfo->pComData->overlay.aac.erRvlcInfo;

  int32_t group, band;

  /* RVLC int32_t specific initialization  Init part 1 of 2 */
  pRvlc->numWindowGroups = GetWindowGroups(&pAacDecoderChannelInfo->icsInfo);
  pRvlc->maxSfbTransmitted =
      GetScaleFactorBandsTransmitted(&pAacDecoderChannelInfo->icsInfo);
  pRvlc->noise_used = 0;               /* noise detection */
  pRvlc->dpcm_noise_nrg = 0;           /* only for debugging */
  pRvlc->dpcm_noise_last_position = 0; /* only for debugging */
  pRvlc->length_of_rvlc_escapes =
      -1; /* default value is used for error detection and concealment */

  /* read only error sensitivity class 1 data (ESC 1 - data) */
  pRvlc->sf_concealment = FDKreadBits(bs, 1);  /* #1 */
  pRvlc->rev_global_gain = FDKreadBits(bs, 8); /* #2 */

  if (GetWindowSequence(&pAacDecoderChannelInfo->icsInfo) == BLOCK_SHORT) {
    pRvlc->length_of_rvlc_sf = FDKreadBits(bs, 11); /* #3 */
  } else {
    pRvlc->length_of_rvlc_sf = FDKreadBits(bs, 9); /* #3 */
  }

  /* check if noise codebook is used */
  for (group = 0; group < pRvlc->numWindowGroups; group++) {
    for (band = 0; band < pRvlc->maxSfbTransmitted; band++) {
      if (pAacDecoderChannelInfo->pDynData->aCodeBook[16 * group + band] ==
          NOISE_HCB) {
        pRvlc->noise_used = 1;
        break;
      }
    }
  }

  if (pRvlc->noise_used)
    pRvlc->dpcm_noise_nrg = FDKreadBits(bs, 9); /* #4  PNS */

  pRvlc->sf_escapes_present = FDKreadBits(bs, 1); /* #5      */

  if (pRvlc->sf_escapes_present) {
    pRvlc->length_of_rvlc_escapes = FDKreadBits(bs, 8); /* #6      */
  }

  if (pRvlc->noise_used) {
    pRvlc->dpcm_noise_last_position = FDKreadBits(bs, 9); /* #7  PNS */
    pRvlc->length_of_rvlc_sf -= 9;
  }

  pRvlc->length_of_rvlc_sf_fwd = pRvlc->length_of_rvlc_sf;
  pRvlc->length_of_rvlc_sf_bwd = pRvlc->length_of_rvlc_sf;
}

/*---------------------------------------------------------------------------------------------
     function:     CRvlc_Decode

     description:  Decode rvlc data
                   The function reads both the escape sequences and the
scalefactors in forward and backward direction. If an error occured during
decoding process which can not be concealed with the rvlc concealment frame
concealment will be initiated. Then the element "rvlcCurrentScaleFactorOK" in
the decoder channel info is set to 0 otherwise it is set to 1.
-----------------------------------------------------------------------------------------------
        input:     - pointer rvlc structure
                   - pointer channel info structure
                   - pointer to persistent channel info structure
                   - pointer bitstream structure
-----------------------------------------------------------------------------------------------
        return:    ErrorStatus = AAC_DEC_OK
--------------------------------------------------------------------------------------------
*/

void CRvlc_Decode(CAacDecoderChannelInfo_t *pAacDecoderChannelInfo,
                  CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo,
                  HANDLE_FDK_BITSTREAM bs) {
  CErRvlcInfo_t *pRvlc =
      &pAacDecoderChannelInfo->pComData->overlay.aac.erRvlcInfo;
  int32_t bitCntOffst;
  int32_t saveBitCnt;

  rvlcInit(pRvlc, pAacDecoderChannelInfo, bs);

  /* save bitstream position */
  saveBitCnt = (int32_t)FDKgetValidBits(bs);

  if (pRvlc->sf_escapes_present)
    rvlcDecodeEscapes(
        pRvlc, pAacDecoderChannelInfo->pComData->overlay.aac.aRvlcScfEsc, bs);

  rvlcDecodeForward(pRvlc, pAacDecoderChannelInfo, bs);
  rvlcDecodeBackward(pRvlc, pAacDecoderChannelInfo, bs);
  rvlcFinalErrorDetection(pAacDecoderChannelInfo, pAacDecoderStaticChannelInfo);

  pAacDecoderChannelInfo->pDynData->specificTo.aac.rvlcIntensityUsed =
      pRvlc->intensity_used;
  pAacDecoderChannelInfo->data.aac.PnsData.PnsActive = pRvlc->noise_used;

  /* restore bitstream position */
  bitCntOffst = (int32_t)FDKgetValidBits(bs) - saveBitCnt;
  if (bitCntOffst) {
    FDKpushBiDirectional(bs, bitCntOffst);
  }
}

void CRvlc_ElementCheck(
    CAacDecoderChannelInfo_t *pAacDecoderChannelInfo[],
    CAacDecoderStaticChannelInfo_t *pAacDecoderStaticChannelInfo[],
    const uint32_t flags, const int32_t elChannels) {
  int32_t ch;

  /* Required for MPS residuals. */
  if (pAacDecoderStaticChannelInfo == NULL) {
    return;
  }

  /* RVLC specific sanity checks */
  if ((flags & AC_ER_RVLC) && (elChannels == 2)) { /* to be reviewed */
    if (((pAacDecoderChannelInfo[0]
              ->pDynData->specificTo.aac.rvlcCurrentScaleFactorOK == 0) ||
         (pAacDecoderChannelInfo[1]
              ->pDynData->specificTo.aac.rvlcCurrentScaleFactorOK == 0)) &&
        pAacDecoderChannelInfo[0]->pComData->jointStereoData.MsMaskPresent) {
      pAacDecoderChannelInfo[0]
          ->pDynData->specificTo.aac.rvlcCurrentScaleFactorOK = 0;
      pAacDecoderChannelInfo[1]
          ->pDynData->specificTo.aac.rvlcCurrentScaleFactorOK = 0;
    }

    if ((pAacDecoderChannelInfo[0]
             ->pDynData->specificTo.aac.rvlcCurrentScaleFactorOK == 0) &&
        (pAacDecoderChannelInfo[1]
             ->pDynData->specificTo.aac.rvlcCurrentScaleFactorOK == 1) &&
        (pAacDecoderChannelInfo[1]
             ->pDynData->specificTo.aac.rvlcIntensityUsed == 1)) {
      pAacDecoderChannelInfo[1]
          ->pDynData->specificTo.aac.rvlcCurrentScaleFactorOK = 0;
    }
  }

  for (ch = 0; ch < elChannels; ch++) {
    pAacDecoderStaticChannelInfo[ch]->concealmentInfo.rvlcPreviousBlockType =
        (GetWindowSequence(&pAacDecoderChannelInfo[ch]->icsInfo) == BLOCK_SHORT)
            ? 0
            : 1;
    if (flags & AC_ER_RVLC) {
      pAacDecoderStaticChannelInfo[ch]
          ->concealmentInfo.rvlcPreviousScaleFactorOK =
          pAacDecoderChannelInfo[ch]
              ->pDynData->specificTo.aac.rvlcCurrentScaleFactorOK;
    } else {
      pAacDecoderStaticChannelInfo[ch]
          ->concealmentInfo.rvlcPreviousScaleFactorOK = 0;
    }
  }
}
