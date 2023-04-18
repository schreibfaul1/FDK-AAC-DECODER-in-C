
/******************* MPEG transport format decoder library *********************

   Author(s):   Daniel Homm

   Description:

*******************************************************************************/

#ifndef TPDEC_LATM_H
#define TPDEC_LATM_H
#include "../libAACdec/newAACDecoder.h"
#include "tpdec_lib.h"

#include "../libFDK/FDK_bitstream.h"

#define MIN_LATM_HEADERLENGTH 9
#define MIN_LOAS_HEADERLENGTH MIN_LATM_HEADERLENGTH + 24 /* both in bits */
#define MIN_TP_BUF_SIZE_LOAS (8194)

enum {
  LATM_MAX_PROG = 1,
  LATM_MAX_LAYER = 1,
  LATM_MAX_VAR_CHUNKS = 16,
  LATM_MAX_ID = 16
};

typedef struct {
  uint32_t m_frameLengthType;
  uint32_t m_bufferFullness;
  uint32_t m_streamID;
  uint32_t m_frameLengthInBits;
} LATM_LAYER_INFO;

typedef struct {
  LATM_LAYER_INFO m_linfo[LATM_MAX_PROG][LATM_MAX_LAYER];
  uint32_t m_taraBufferFullness;
  uint32_t m_otherDataLength;
  uint32_t m_audioMuxLengthBytes; /* Length of LOAS payload */

  uint8_t m_useSameStreamMux;
  uint8_t m_AudioMuxVersion;
  uint8_t m_AudioMuxVersionA;
  uint8_t m_allStreamsSameTimeFraming;
  uint8_t m_noSubFrames;
  uint8_t m_numProgram;
  uint8_t m_numLayer[LATM_MAX_PROG];

  uint8_t m_otherDataPresent;
  uint8_t m_crcCheckPresent;

  int8_t BufferFullnessAchieved;
  uint8_t
  usacExplicitCfgChanged;      /* explicit config in case of USAC and LOAS/LATM
                                  must be compared to IPF cfg */
  uint8_t applyAsc;              /* apply ASC immediate without flushing */
  uint8_t newCfgHasAudioPreRoll; /* the new (dummy parsed) config has an
                                  AudioPreRoll */
} CLatmDemux;

int32_t CLatmDemux_ReadAuChunkLengthInfo(HANDLE_FDK_BITSTREAM bs);

TRANSPORTDEC_ERROR_t CLatmDemux_Read(HANDLE_FDK_BITSTREAM bs,
                                   CLatmDemux *pLatmDemux, TRANSPORT_TYPE_t tt,
                                   CSTpCallBacks_t *pTpDecCallbacks,
                                   CSAudioSpecificConfig_t *pAsc,
                                   int32_t *pfConfigFound,
                                   const int32_t ignoreBufferFullness);

/**
 * \brief Read StreamMuxConfig
 * \param bs bit stream handle as data source
 * \param pLatmDemux pointer to CLatmDemux struct of current LATM context
 * \param pTpDecCallbacks Call back structure for configuration callbacks
 * \param pAsc pointer to a ASC for configuration storage
 * \param pfConfigFound pointer to a flag which is set to 1 if a configuration
 * was found and processed successfully
 * \param configMode Config modes: memory allocation mode or config change
 * detection mode
 * \param configChanged Indicates a config change
 * \return error code
 */
TRANSPORTDEC_ERROR_t CLatmDemux_ReadStreamMuxConfig(
    HANDLE_FDK_BITSTREAM bs, CLatmDemux *pLatmDemux,
    CSTpCallBacks_t *pTpDecCallbacks, CSAudioSpecificConfig_t *pAsc,
    int32_t *pfConfigFound, uint8_t configMode, uint8_t configChanged);

TRANSPORTDEC_ERROR_t CLatmDemux_ReadPayloadLengthInfo(HANDLE_FDK_BITSTREAM bs,
                                                    CLatmDemux *pLatmDemux);

uint32_t CLatmDemux_GetFrameLengthInBits(CLatmDemux *pLatmDemux, const uint32_t prog,
                                     const uint32_t layer);
uint32_t CLatmDemux_GetOtherDataPresentFlag(CLatmDemux *pLatmDemux);
uint32_t CLatmDemux_GetOtherDataLength(CLatmDemux *pLatmDemux);
uint32_t CLatmDemux_GetNrOfSubFrames(CLatmDemux *pLatmDemux);
uint32_t CLatmDemux_GetNrOfLayers(CLatmDemux *pLatmDemux, const uint32_t program);

#endif /* TPDEC_LATM_H */
