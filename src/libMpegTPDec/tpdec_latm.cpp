

/******************* MPEG transport format decoder library *********************

   Author(s):   Daniel Homm

   Description:

*******************************************************************************/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "tpdec_latm.h"

#include "../libFDK/FDK_bitstream.h"

#define TPDEC_TRACKINDEX(p, l) (1 * (p) + (l))

static uint32_t CLatmDemux_GetValue(HANDLE_FDK_BITSTREAM bs) {
  uint8_t bytesForValue = 0, tmp = 0;
  int32_t value = 0;

  bytesForValue = (uint8_t)FDKreadBits(bs, 2);

  for (uint32_t i = 0; i <= bytesForValue; i++) {
    value <<= 8;
    tmp = (uint8_t)FDKreadBits(bs, 8);
    value += tmp;
  }

  return value;
}

static TRANSPORTDEC_ERROR_t CLatmDemux_ReadAudioMuxElement(
    HANDLE_FDK_BITSTREAM bs, CLatmDemux *pLatmDemux, int32_t m_muxConfigPresent,
    CSTpCallBacks_t *pTpDecCallbacks, CSAudioSpecificConfig_t *pAsc,
    int32_t *pfConfigFound) {
  TRANSPORTDEC_ERROR_t ErrorStatus = TRANSPORTDEC_OK;

  if (m_muxConfigPresent) {
    pLatmDemux->m_useSameStreamMux = FDKreadBits(bs, 1);

    if (!pLatmDemux->m_useSameStreamMux) {
      int32_t i;
      uint8_t configChanged = 0;
      uint8_t configMode = 0;

      FDK_BITSTREAM_t bsAnchor;

      FDK_BITSTREAM_t bsAnchorDummyParse;

      if (!pLatmDemux->applyAsc) {
        bsAnchorDummyParse = *bs;
        pLatmDemux->newCfgHasAudioPreRoll = 0;
        /* do dummy-parsing of ASC to determine if there is an audioPreRoll */
        configMode |= AC_CM_DET_CFG_CHANGE;
        if (TRANSPORTDEC_OK !=
            (ErrorStatus = CLatmDemux_ReadStreamMuxConfig(
                 bs, pLatmDemux, pTpDecCallbacks, pAsc, pfConfigFound,
                 configMode, configChanged))) {
          goto bail;
        }

        /* Allow flushing only when audioPreroll functionality is enabled in
         * current and new config otherwise the new config can be applied
         * immediately. */
        if (pAsc->m_sc.m_usacConfig.element[0]
                .extElement.usacExtElementHasAudioPreRoll &&
            pLatmDemux->newCfgHasAudioPreRoll) {
          pLatmDemux->newCfgHasAudioPreRoll = 0;
          /* with audioPreRoll we must flush before applying new cfg */
          pLatmDemux->applyAsc = 0;
        } else {
          *bs = bsAnchorDummyParse;
          pLatmDemux->applyAsc = 1; /* apply new config immediate */
        }
      }

      if (pLatmDemux->applyAsc) {
        for (i = 0; i < 2; i++) {
          configMode = 0;

          if (i == 0) {
            configMode |= AC_CM_DET_CFG_CHANGE;
            bsAnchor = *bs;
          } else {
            configMode |= AC_CM_ALLOC_MEM;
            *bs = bsAnchor;
          }

          if (TRANSPORTDEC_OK !=
              (ErrorStatus = CLatmDemux_ReadStreamMuxConfig(
                   bs, pLatmDemux, pTpDecCallbacks, pAsc, pfConfigFound,
                   configMode, configChanged))) {
            goto bail;
          }

          if (ErrorStatus == TRANSPORTDEC_OK) {
            if ((i == 0) && (pAsc->AacConfigChanged || pAsc->SbrConfigChanged ||
                             pAsc->SacConfigChanged)) {
              int32_t errC;

              configChanged = 1;
              errC = pTpDecCallbacks->cbFreeMem(pTpDecCallbacks->cbFreeMemData,
                                                pAsc);
              if (errC != 0) {
                ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
                goto bail;
              }
            }
          }
        }
      }
    }
  }

  /* If there was no configuration read, its not possible to parse
   * PayloadLengthInfo below. */
  if (!*pfConfigFound) {
    ErrorStatus = TRANSPORTDEC_SYNC_ERROR;
    goto bail;
  }

  if (pLatmDemux->m_AudioMuxVersionA == 0) {
    /* Do only once per call, because parsing and decoding is done in-line. */
    if (TRANSPORTDEC_OK !=
        (ErrorStatus = CLatmDemux_ReadPayloadLengthInfo(bs, pLatmDemux))) {
      *pfConfigFound = 0;
      goto bail;
    }
  } else {
    /* audioMuxVersionA > 0 is reserved for future extensions */
    ErrorStatus = TRANSPORTDEC_UNSUPPORTED_FORMAT;
    *pfConfigFound = 0;
    goto bail;
  }

bail:
  if (ErrorStatus != TRANSPORTDEC_OK) {
    pLatmDemux->applyAsc = 1;
  }

  return (ErrorStatus);
}

TRANSPORTDEC_ERROR_t CLatmDemux_Read(HANDLE_FDK_BITSTREAM bs,
                                   CLatmDemux *pLatmDemux, TRANSPORT_TYPE_t tt,
                                   CSTpCallBacks_t *pTpDecCallbacks,
                                   CSAudioSpecificConfig_t *pAsc,
                                   int32_t *pfConfigFound,
                                   const int32_t ignoreBufferFullness) {
  uint32_t cntBits;
  uint32_t cmpBufferFullness;
  uint32_t audioMuxLengthBytesLast = 0;
  TRANSPORTDEC_ERROR_t ErrorStatus = TRANSPORTDEC_OK;

  cntBits = FDKgetValidBits(bs);

  if ((int32_t)cntBits < MIN_LATM_HEADERLENGTH) {
    return TRANSPORTDEC_NOT_ENOUGH_BITS;
  }

  if (TRANSPORTDEC_OK != (ErrorStatus = CLatmDemux_ReadAudioMuxElement(
                              bs, pLatmDemux, (tt != TT_MP4_LATM_MCP0),
                              pTpDecCallbacks, pAsc, pfConfigFound)))
    return (ErrorStatus);

  if (!ignoreBufferFullness) {
    cmpBufferFullness =
        24 + audioMuxLengthBytesLast * 8 +
        pLatmDemux->m_linfo[0][0].m_bufferFullness *
            pAsc[TPDEC_TRACKINDEX(0, 0)].m_channelConfiguration * 32;

    /* evaluate buffer fullness */

    if (pLatmDemux->m_linfo[0][0].m_bufferFullness != 0xFF) {
      if (!pLatmDemux->BufferFullnessAchieved) {
        if (cntBits < cmpBufferFullness) {
          /* condition for start of decoding is not fulfilled */

          /* the current frame will not be decoded */
          return TRANSPORTDEC_NOT_ENOUGH_BITS;
        } else {
          pLatmDemux->BufferFullnessAchieved = 1;
        }
      }
    }
  }

  return (ErrorStatus);
}

TRANSPORTDEC_ERROR_t CLatmDemux_ReadStreamMuxConfig(
    HANDLE_FDK_BITSTREAM bs, CLatmDemux *pLatmDemux,
    CSTpCallBacks_t *pTpDecCallbacks, CSAudioSpecificConfig_t *pAsc,
    int32_t *pfConfigFound, uint8_t configMode, uint8_t configChanged) {
  CSAudioSpecificConfig_t ascDummy; /* the actual config is needed for flushing,
                                     after that new config can be parsed */
  CSAudioSpecificConfig_t *pAscDummy;
  pAscDummy = &ascDummy;
  pLatmDemux->usacExplicitCfgChanged = 0;
  LATM_LAYER_INFO *p_linfo = NULL;
  TRANSPORTDEC_ERROR_t ErrorStatus = TRANSPORTDEC_OK;
  uint8_t updateConfig[1 * 1] = {0};

  pLatmDemux->m_AudioMuxVersion = FDKreadBits(bs, 1);

  if (pLatmDemux->m_AudioMuxVersion == 0) {
    pLatmDemux->m_AudioMuxVersionA = 0;
  } else {
    pLatmDemux->m_AudioMuxVersionA = FDKreadBits(bs, 1);
  }

  if (pLatmDemux->m_AudioMuxVersionA == 0) {
    if (pLatmDemux->m_AudioMuxVersion == 1) {
      pLatmDemux->m_taraBufferFullness = CLatmDemux_GetValue(bs);
    }
    pLatmDemux->m_allStreamsSameTimeFraming = FDKreadBits(bs, 1);
    pLatmDemux->m_noSubFrames = FDKreadBits(bs, 6) + 1;
    pLatmDemux->m_numProgram = FDKreadBits(bs, 4) + 1;

    if (pLatmDemux->m_numProgram > LATM_MAX_PROG) {
      ErrorStatus = TRANSPORTDEC_UNSUPPORTED_FORMAT;
      goto bail;
    }

    int32_t idCnt = 0;
    for (uint32_t prog = 0; prog < pLatmDemux->m_numProgram; prog++) {
      pLatmDemux->m_numLayer[prog] = FDKreadBits(bs, 3) + 1;
      if (pLatmDemux->m_numLayer[prog] > LATM_MAX_LAYER) {
        ErrorStatus = TRANSPORTDEC_UNSUPPORTED_FORMAT;
        goto bail;
      }

      for (uint32_t lay = 0; lay < pLatmDemux->m_numLayer[prog]; lay++) {
        int32_t useSameConfig;
        p_linfo = &pLatmDemux->m_linfo[prog][lay];

        p_linfo->m_streamID = idCnt++;
        p_linfo->m_frameLengthInBits = 0;

        if ((prog == 0) && (lay == 0)) {
          useSameConfig = 0;
        } else {
          useSameConfig = FDKreadBits(bs, 1);
        }

        if (useSameConfig) {
          if (lay > 0) {
            memcpy(&pAsc[TPDEC_TRACKINDEX(prog, lay)],
                      &pAsc[TPDEC_TRACKINDEX(prog, lay - 1)],
                      sizeof(CSAudioSpecificConfig_t));
          } else {
            ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
            goto bail;
          }
        } else {
          uint32_t usacConfigLengthPrev = 0;
          uint8_t usacConfigPrev[TP_USAC_MAX_CONFIG_LEN];

          if (!(pLatmDemux->applyAsc) &&
              (pAsc[TPDEC_TRACKINDEX(prog, lay)].m_aot == AOT_USAC)) {
            usacConfigLengthPrev =
                (uint32_t)(pAsc[TPDEC_TRACKINDEX(prog, lay)]
                           .m_sc.m_usacConfig.UsacConfigBits +
                       7) >>
                3; /* store previous USAC config length */
            if (usacConfigLengthPrev > TP_USAC_MAX_CONFIG_LEN) {
              ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
              goto bail;
            }
            memset(usacConfigPrev, 0, TP_USAC_MAX_CONFIG_LEN);
            memcpy(
                usacConfigPrev,
                &pAsc[TPDEC_TRACKINDEX(prog, lay)].m_sc.m_usacConfig.UsacConfig,
                usacConfigLengthPrev); /* store previous USAC config */
          }
          if (pLatmDemux->m_AudioMuxVersion == 1) {
            FDK_BITSTREAM_t tmpBs;
            int32_t ascLen = 0;
            ascLen = CLatmDemux_GetValue(bs);
            /* The ascLen could be wrong, so check if validBits<=bufBits*/
            if (ascLen < 0 || ascLen > (int32_t)FDKgetValidBits(bs)) {
              ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
              goto bail;
            }
            FDKsyncCache(bs);
            tmpBs = *bs;
            tmpBs.hBitBuf.ValidBits = ascLen;

            /* Read ASC */
            if (pLatmDemux->applyAsc) {
              if (TRANSPORTDEC_OK !=
                  (ErrorStatus = AudioSpecificConfig_Parse(
                       &pAsc[TPDEC_TRACKINDEX(prog, lay)], &tmpBs, 1,
                       pTpDecCallbacks, configMode, configChanged,
                       AOT_NULL_OBJECT)))
                goto bail;
            } else {
              if (TRANSPORTDEC_OK !=
                  (ErrorStatus = AudioSpecificConfig_Parse(
                       pAscDummy, &tmpBs, 1, pTpDecCallbacks, configMode,
                       configChanged, AOT_NULL_OBJECT)))
                goto bail;
            }

            /* The field p_linfo->m_ascLen could be wrong, so check if */
            if (0 > (int32_t)FDKgetValidBits(&tmpBs)) {
              ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
              goto bail;
            }
            FDKpushFor(bs, ascLen); /* position bitstream after ASC */
          } else {
            /* Read ASC */
            if (pLatmDemux->applyAsc) {
              if (TRANSPORTDEC_OK != (ErrorStatus = AudioSpecificConfig_Parse(
                                          &pAsc[TPDEC_TRACKINDEX(prog, lay)],
                                          bs, 0, pTpDecCallbacks, configMode,
                                          configChanged, AOT_NULL_OBJECT)))
                goto bail;
            } else {
              if (TRANSPORTDEC_OK !=
                  (ErrorStatus = AudioSpecificConfig_Parse(
                       pAscDummy, bs, 0, pTpDecCallbacks, configMode,
                       configChanged, AOT_NULL_OBJECT)))
                goto bail;
            }
          }
          if (!pLatmDemux->applyAsc) {
            updateConfig[TPDEC_TRACKINDEX(prog, lay)] = 0;
          } else {
            updateConfig[TPDEC_TRACKINDEX(prog, lay)] = 1;
          }

          if (!pLatmDemux->applyAsc) {
            if (pAscDummy[TPDEC_TRACKINDEX(prog, lay)].m_aot ==
                AOT_USAC) { /* flush in case SMC has changed */
              const uint32_t usacConfigLength =
                  (uint32_t)(pAscDummy->m_sc.m_usacConfig.UsacConfigBits + 7) >> 3;
              if (usacConfigLength > TP_USAC_MAX_CONFIG_LEN) {
                ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
                goto bail;
              }
              if (usacConfigLength != usacConfigLengthPrev) {
                memset(&pAsc[TPDEC_TRACKINDEX(prog, lay)]
                                 .m_sc.m_usacConfig.UsacConfig, 0,
                            TP_USAC_MAX_CONFIG_LEN);
                memcpy(&pAsc[TPDEC_TRACKINDEX(prog, lay)]
                               .m_sc.m_usacConfig.UsacConfig,
                          &pAscDummy->m_sc.m_usacConfig.UsacConfig,
                          usacConfigLength); /* store new USAC config */
                pAsc[TPDEC_TRACKINDEX(prog, lay)]
                    .m_sc.m_usacConfig.UsacConfigBits =
                    pAscDummy->m_sc.m_usacConfig.UsacConfigBits;
                pLatmDemux->usacExplicitCfgChanged = 1;
              } else {
                if (memcmp(usacConfigPrev,
                              pAscDummy->m_sc.m_usacConfig.UsacConfig,
                              usacConfigLengthPrev)) {
                  memset(&pAsc[TPDEC_TRACKINDEX(prog, lay)]
                                   .m_sc.m_usacConfig.UsacConfig, 0,
                              TP_USAC_MAX_CONFIG_LEN);
                  memcpy(&pAsc[TPDEC_TRACKINDEX(prog, lay)]
                                 .m_sc.m_usacConfig.UsacConfig,
                            &pAscDummy->m_sc.m_usacConfig.UsacConfig,
                            usacConfigLength); /* store new USAC config */
                  pAsc[TPDEC_TRACKINDEX(prog, lay)]
                      .m_sc.m_usacConfig.UsacConfigBits =
                      pAscDummy->m_sc.m_usacConfig.UsacConfigBits;
                  pLatmDemux->usacExplicitCfgChanged = 1;
                }
              }

              if (pAscDummy[TPDEC_TRACKINDEX(prog, lay)]
                      .m_sc.m_usacConfig.m_usacNumElements) {
                if (pAscDummy[TPDEC_TRACKINDEX(prog, lay)]
                        .m_sc.m_usacConfig.element[0]
                        .extElement.usacExtElementHasAudioPreRoll) {
                  pLatmDemux->newCfgHasAudioPreRoll =
                      1; /* if dummy parsed cfg has audioPreRoll we first flush
                            before applying new cfg */
                }
              }
            }
          }
        }

        p_linfo->m_frameLengthType = FDKreadBits(bs, 3);
        switch (p_linfo->m_frameLengthType) {
          case 0:
            p_linfo->m_bufferFullness = FDKreadBits(bs, 8);

            if (!pLatmDemux->m_allStreamsSameTimeFraming) {
              if ((lay > 0) &&
                  (pAsc[TPDEC_TRACKINDEX(prog, lay)].m_aot == AOT_AAC_SCAL ||
                   pAsc[TPDEC_TRACKINDEX(prog, lay)].m_aot ==
                       AOT_ER_AAC_SCAL) &&
                  (pAsc[TPDEC_TRACKINDEX(prog, lay - 1)].m_aot == AOT_CELP ||
                   pAsc[TPDEC_TRACKINDEX(prog, lay - 1)].m_aot ==
                       AOT_ER_CELP)) { /* The layer maybe
                                          ignored later so
                                          read it anyway: */
                /* coreFrameOffset = */ FDKreadBits(bs, 6);
              }
            }
            break;
          case 1:
            p_linfo->m_frameLengthInBits = FDKreadBits(bs, 9);
            break;
          case 3:
          case 4:
          case 5:
            /* CELP */
          case 6:
          case 7:
            /* HVXC */
          default:
            ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
            goto bail;
        } /* switch framelengthtype*/

      } /* layer loop */
    }   /* prog loop */

    pLatmDemux->m_otherDataPresent = FDKreadBits(bs, 1);
    pLatmDemux->m_otherDataLength = 0;

    if (pLatmDemux->m_otherDataPresent) {
      if (pLatmDemux->m_AudioMuxVersion == 1) {
        pLatmDemux->m_otherDataLength = CLatmDemux_GetValue(bs);
      } else {
        int32_t otherDataLenEsc = 0;
        do {
          pLatmDemux->m_otherDataLength <<= 8;  // *= 256
          otherDataLenEsc = FDKreadBits(bs, 1);
          pLatmDemux->m_otherDataLength += FDKreadBits(bs, 8);
        } while (otherDataLenEsc);
      }
      if (pLatmDemux->m_audioMuxLengthBytes <
          (pLatmDemux->m_otherDataLength >> 3)) {
        ErrorStatus = TRANSPORTDEC_PARSE_ERROR;
        goto bail;
      }
    }

    pLatmDemux->m_crcCheckPresent = FDKreadBits(bs, 1);

    if (pLatmDemux->m_crcCheckPresent) {
      FDKreadBits(bs, 8);
    }

  } else {
    /* audioMuxVersionA > 0 is reserved for future extensions */
    ErrorStatus = TRANSPORTDEC_UNSUPPORTED_FORMAT;
  }

  /* Configure source decoder: */
  if (ErrorStatus == TRANSPORTDEC_OK) {
    uint32_t prog;
    for (prog = 0; prog < pLatmDemux->m_numProgram; prog++) {
      uint32_t lay;
      for (lay = 0; lay < pLatmDemux->m_numLayer[prog]; lay++) {
        if (updateConfig[TPDEC_TRACKINDEX(prog, lay)] != 0) {
          int32_t cbError;
          cbError = pTpDecCallbacks->cbUpdateConfig(
              pTpDecCallbacks->cbUpdateConfigData,
              &pAsc[TPDEC_TRACKINDEX(prog, lay)],
              pAsc[TPDEC_TRACKINDEX(prog, lay)].configMode,
              &pAsc[TPDEC_TRACKINDEX(prog, lay)].AacConfigChanged);
          if (cbError == TRANSPORTDEC_NEED_TO_RESTART) {
            *pfConfigFound = 0;
            ErrorStatus = TRANSPORTDEC_NEED_TO_RESTART;
            goto bail;
          }
          if (cbError != 0) {
            *pfConfigFound = 0;
            if (lay == 0) {
              ErrorStatus = TRANSPORTDEC_SYNC_ERROR;
              goto bail;
            }
          } else {
            *pfConfigFound = 1;
          }
        } else {
          *pfConfigFound = 1;
        }
      }
    }
  }

bail:
  if (ErrorStatus != TRANSPORTDEC_OK) {
    uint8_t applyAsc = pLatmDemux->applyAsc;
    memset(pLatmDemux, 0, sizeof(CLatmDemux)); /* reset structure */
    pLatmDemux->applyAsc = applyAsc;
  } else {
    /* no error and config parsing is finished */
    if (configMode == AC_CM_ALLOC_MEM) pLatmDemux->applyAsc = 0;
  }

  return (ErrorStatus);
}

TRANSPORTDEC_ERROR_t CLatmDemux_ReadPayloadLengthInfo(HANDLE_FDK_BITSTREAM bs,
                                                    CLatmDemux *pLatmDemux) {
  TRANSPORTDEC_ERROR_t ErrorStatus = TRANSPORTDEC_OK;
  int32_t totalPayloadBits = 0;

  if (pLatmDemux->m_allStreamsSameTimeFraming == 1) {
    assert(pLatmDemux->m_numProgram <= LATM_MAX_PROG);
    for (uint32_t prog = 0; prog < pLatmDemux->m_numProgram; prog++) {
      assert(pLatmDemux->m_numLayer[prog] <= LATM_MAX_LAYER);
      for (uint32_t lay = 0; lay < pLatmDemux->m_numLayer[prog]; lay++) {
        LATM_LAYER_INFO *p_linfo = &pLatmDemux->m_linfo[prog][lay];

        switch (p_linfo->m_frameLengthType) {
          case 0:
            p_linfo->m_frameLengthInBits = CLatmDemux_ReadAuChunkLengthInfo(bs);
            totalPayloadBits += p_linfo->m_frameLengthInBits;
            break;
          case 3:
          case 5:
          case 7:
          default:
            return TRANSPORTDEC_PARSE_ERROR;  // AAC_DEC_LATM_INVALIDFRAMELENGTHTYPE;
        }
      }
    }
  } else {
    ErrorStatus = TRANSPORTDEC_PARSE_ERROR;  // AAC_DEC_LATM_TIMEFRAMING;
  }
  if (pLatmDemux->m_audioMuxLengthBytes > (uint32_t)0 &&
      totalPayloadBits > (int32_t)pLatmDemux->m_audioMuxLengthBytes * 8) {
    return TRANSPORTDEC_PARSE_ERROR;
  }

  return (ErrorStatus);
}

int32_t CLatmDemux_ReadAuChunkLengthInfo(HANDLE_FDK_BITSTREAM bs) {
  uint8_t endFlag;
  int32_t len = 0;

  do {
    uint8_t tmp = (uint8_t)FDKreadBits(bs, 8);
    endFlag = (tmp < 255);

    len += tmp;

  } while (endFlag == 0);

  len <<= 3; /* convert from bytes to bits */

  return len;
}

uint32_t CLatmDemux_GetFrameLengthInBits(CLatmDemux *pLatmDemux, const uint32_t prog,
                                     const uint32_t layer) {
  uint32_t nFrameLenBits = 0;
  if (prog < pLatmDemux->m_numProgram) {
    if (layer < pLatmDemux->m_numLayer[prog]) {
      nFrameLenBits = pLatmDemux->m_linfo[prog][layer].m_frameLengthInBits;
    }
  }
  return nFrameLenBits;
}

uint32_t CLatmDemux_GetOtherDataPresentFlag(CLatmDemux *pLatmDemux) {
  return pLatmDemux->m_otherDataPresent ? 1 : 0;
}

uint32_t CLatmDemux_GetOtherDataLength(CLatmDemux *pLatmDemux) {
  return pLatmDemux->m_otherDataLength;
}

uint32_t CLatmDemux_GetNrOfSubFrames(CLatmDemux *pLatmDemux) {
  return pLatmDemux->m_noSubFrames;
}

uint32_t CLatmDemux_GetNrOfLayers(CLatmDemux *pLatmDemux, const uint32_t prog) {
  uint32_t numLayer = 0;
  if (prog < pLatmDemux->m_numProgram) {
    numLayer = pLatmDemux->m_numLayer[prog];
  }
  return numLayer;
}
