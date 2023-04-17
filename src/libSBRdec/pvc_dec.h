

/**************************** SBR decoder library ******************************

   Author(s):   Matthias Hildenbrand

   Description: Decode Predictive Vector Coding Data

*******************************************************************************/

#ifndef PVC_DEC_H
#define PVC_DEC_H
#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/common_fix.h"

#define PVC_DIVMODE_BITS 3
#define PVC_REUSEPVCID_BITS 1
#define PVC_PVCID_BITS 7
#define PVC_GRIDINFO_BITS 1

#define MAX_PVC_ENVELOPES 2
#define PVC_NTIMESLOT 16
#define PVC_NBLOW 3 /* max. number of grouped QMF subbands below SBR range */

#define PVC_NBHIGH_MODE1 8
#define PVC_NBHIGH_MODE2 6
#define PVC_NBHIGH_MAX (PVC_NBHIGH_MODE1)
#define PVC_NS_MAX 16

/** Data for each PVC instance which needs to be persistent accross SBR frames
 */
typedef struct {
  uint8_t kx_last;        /**< Xover frequency of last frame */
  uint8_t pvc_mode_last;  /**< PVC mode of last frame */
  uint8_t Esg_slot_index; /**< Ring buffer index to current Esg time slot */
  uint8_t pvcBorder0;     /**< Start SBR time slot of PVC frame */
  int32_t Esg[PVC_NS_MAX][PVC_NBLOW]; /**< Esg(ksg,t) of current and 15
                                          previous time slots (ring buffer) in
                                          logarithmical domain */
} PVC_STATIC_DATA;

/** Data for each PVC instance which is valid during one SBR frame */
typedef struct {
  uint8_t pvc_mode;   /**< PVC mode 1 or 2, 0 means legacy SBR */
  uint8_t pvcBorder0; /**< Start SBR time slot of PVC frame */
  uint8_t kx;         /**< Index of the first QMF subband in the SBR range */
  uint8_t RATE;       /**< Number of QMF subband samples per time slot (2 or 4) */
  uint8_t ns; /**< Number of time slots for time-domain smoothing of Esg(ksg,t) */
  const uint8_t
      *pPvcID; /**< Pointer to prediction coefficient matrix index table */
  uint8_t pastEsgSlotsAvail;   /**< Number of past Esg(ksg,t) which are available
                                for smoothing filter */
  const int16_t *pSCcoeffs; /**< Pointer to smoothing window table */
  int8_t
  sg_offset_low[PVC_NBLOW + 1]; /**< Offset table for PVC grouping of SBR
                                   subbands below SBR range */
  int8_t sg_offset_high_kx[PVC_NBHIGH_MAX + 1]; /**< Offset table for PVC
                                                  grouping of SBR subbands in
                                                  SBR range (relativ to kx) */
  uint8_t nbHigh; /**< Number of grouped QMF subbands in the SBR range */
  const int8_t *pScalingCoef; /**< Pointer to scaling coeff table */
  const uint8_t *pPVCTab1;     /**< PVC mode 1 table */
  const uint8_t *pPVCTab2;     /**< PVC mode 2 table */
  const uint8_t *pPVCTab1_dp;  /**< Mapping of pvcID to PVC mode 1 table */
  int32_t predEsg[PVC_NTIMESLOT]
                  [PVC_NBHIGH_MAX]; /**< Predicted Energy in linear domain */
  int32_t predEsg_exp[PVC_NTIMESLOT];   /**< Exponent of predicted Energy in linear
                                       domain */
  int32_t predEsg_expMax;               /**< Maximum of predEsg_exp[] */
} PVC_DYNAMIC_DATA;

/**
 * \brief Initialize PVC data structures for current frame (call if pvcMode =
 * 0,1,2)
 * \param[in] pPvcStaticData Pointer to PVC persistent data
 * \param[out] pPvcDynamicData Pointer to PVC dynamic data
 * \param[in] pvcMode PVC mode 1 or 2, 0 means legacy SBR
 * \param[in] ns Number of time slots for time-domain smoothing of Esg(ksg,t)
 * \param[in] RATE Number of QMF subband samples per time slot (2 or 4)
 * \param[in] kx Index of the first QMF subband in the SBR range
 * \param[in] pvcBorder0 Start SBR time slot of PVC frame
 * \param[in] pPvcID Pointer to array of PvcIDs read from bitstream
 */
int32_t pvcInitFrame(PVC_STATIC_DATA *pPvcStaticData,
                 PVC_DYNAMIC_DATA *pPvcDynamicData, const uint8_t pvcMode,
                 const uint8_t ns, const int32_t RATE, const int32_t kx,
                 const int32_t pvcBorder0, const uint8_t *pPvcID);

/**
 * \brief Wrapper function for pvcDecodeTimeSlot() to decode PVC data of one
 * frame (call if pvcMode = 1,2)
 * \param[in,out] pPvcStaticData Pointer to PVC persistent data
 * \param[in,out] pPvcDynamicData Pointer to PVC dynamic data
 * \param[in] qmfBufferReal Pointer to array with real QMF subbands
 * \param[in] qmfBufferImag Pointer to array with imag QMF subbands
 * \param[in] overlap Number of QMF overlap slots
 * \param[in] qmfExponentOverlap Exponent of qmfBuffer (low part) of overlap
 * slots
 * \param[in] qmfExponentCurrent Exponent of qmfBuffer (low part)
 */
void pvcDecodeFrame(PVC_STATIC_DATA *pPvcStaticData,
                    PVC_DYNAMIC_DATA *pPvcDynamicData, int32_t **qmfBufferReal,
                    int32_t **qmfBufferImag, const int32_t overlap,
                    const int32_t qmfExponentOverlap, const int32_t qmfExponentCurrent);

/**
 * \brief Decode PVC data for one SBR time slot (call if pvcMode = 1,2)
 * \param[in,out] pPvcStaticData Pointer to PVC persistent data
 * \param[in,out] pPvcDynamicData Pointer to PVC dynamic data
 * \param[in] qmfBufferReal Pointer to array with real QMF subbands
 * \param[in] qmfBufferImag Pointer to array with imag QMF subbands
 * \param[in] qmfExponent Exponent of qmfBuffer of current time slot
 * \param[in] pvcBorder0 Start SBR time slot of PVC frame
 * \param[in] timeSlotNumber Number of current SBR time slot (0..15)
 * \param[out] predictedEsgSlot Predicted Energy of current time slot
 * \param[out] predictedEsg_exp Exponent of predicted Energy of current time
 * slot
 */
void pvcDecodeTimeSlot(PVC_STATIC_DATA *pPvcStaticData,
                       PVC_DYNAMIC_DATA *pPvcDynamicData,
                       int32_t **qmfSlotReal, int32_t **qmfSlotImag,
                       const int32_t qmfExponent, const int32_t pvcBorder0,
                       const int32_t timeSlotNumber, int32_t predictedEsgSlot[],
                       int32_t *predictedEsg_exp);

/**
 * \brief Finish the current PVC frame (call if pvcMode = 0,1,2)
 * \param[in,out] pPvcStaticData Pointer to PVC persistent data
 * \param[in,out] pPvcDynamicData Pointer to PVC dynamic data
 */
void pvcEndFrame(PVC_STATIC_DATA *pPvcStaticData,
                 PVC_DYNAMIC_DATA *pPvcDynamicData);

/**
 * \brief Expand predicted PVC grouped energies to full QMF subband resolution
 * \param[in] pPvcDynamicData Pointer to PVC dynamic data
 * \param[in] timeSlot Number of current SBR time slot (0..15)
 * \param[in] lengthOutputVector Lenght of output vector
 * \param[out] pOutput Output array for predicted energies
 * \param[out] pOutput_exp Exponent of predicted energies
 */
void expandPredEsg(const PVC_DYNAMIC_DATA *pPvcDynamicData, const int32_t timeSlot,
                   const int32_t lengthOutputVector, int32_t *pOutput,
                   int8_t *pOutput_exp);

#endif /* PVC_DEC_H*/
