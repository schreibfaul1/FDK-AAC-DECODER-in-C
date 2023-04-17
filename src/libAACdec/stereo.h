

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description: joint stereo processing

*******************************************************************************/

#ifndef STEREO_H
#define STEREO_H

#include <stdint.h>
#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/FDK_bitstream.h"
#include "../libFDK/common_fix.h"

#define SFB_PER_PRED_BAND 2

#define SR_FNA_OUT                                                           \
  0 /* Additional scaling of the CJointStereo_filterAndAdd()-output to avoid \
       overflows.    */
    /* The scaling factor can be set to 0 if the coefficients are prescaled
     * appropriately. */
/* Prescaling via factor SF_FNA_COEFFS is done at compile-time but should only
 * be      */
/* utilized if the coefficients are stored as int32_t. (cp. aac_rom.cpp/.h) */

/* The NO_CPLX_PRED_BUGFIX-switch was introduced to enable decoding of
   conformance-streams in way that they are comparable to buggy
   reference-streams. This is established by storing the prediction direction
   for computation of the "downmix MDCT of previous frame". This is not standard
   compliant. Once correct reference-streams for complex-stereo-prediction are
   available this switch becomes obsolete.
*/
/*#define NO_CPLX_PRED_BUGFIX*/

enum { JointStereoMaximumGroups = 8, JointStereoMaximumBands = 64 };

typedef struct {
  uint8_t pred_dir;  // 0 = prediction from mid to side channel, 1 = vice versa
  uint8_t
  igf_pred_dir;  // 0 = prediction from mid to side channel, 1 = vice versa
  uint8_t complex_coef;    // 0 = alpha_q_im[x] is 0 for all prediction bands, 1 =
                         // alpha_q_im[x] is transmitted via bitstream
  uint8_t use_prev_frame;  // 0 = use current frame for MDST estimation, 1 = use
                         // current and previous frame

  int16_t alpha_q_re[JointStereoMaximumGroups][JointStereoMaximumBands];
  int16_t alpha_q_im[JointStereoMaximumGroups][JointStereoMaximumBands];
} CCplxPredictionData;

/* joint stereo scratch memory (valid for this frame) */
typedef struct {
  uint8_t MsMaskPresent;
  uint8_t MsUsed[JointStereoMaximumBands]; /*!< every arry element contains flags
                                            for up to 8 groups. this array is
                                            also utilized for complex stereo
                                            prediction. */
  uint8_t IGF_MsMaskPresent;

  uint8_t cplx_pred_flag; /* stereo complex prediction was signalled for this
                           frame */
  uint8_t igf_cplx_pred_flag;

  /* The following array and variable are needed for the case  when INF is
   * active */
  int32_t store_dmx_re_prev[1024];
  int16_t store_dmx_re_prev_e;

} CJointStereoData;

/* joint stereo persistent memory */
typedef struct {
  uint8_t clearSpectralCoeffs; /* indicates that the spectral coeffs must be
                                cleared because the transform splitting active
                                flag of the left and right channel was different
                              */

  int32_t *scratchBuffer; /* pointer to scratch buffer */

  int32_t
  *spectralCoeffs[2]; /* spectral coefficients of this channel utilized by
                         complex stereo prediction */
  int16_t *specScale[2];

  int16_t alpha_q_re_prev[JointStereoMaximumGroups][JointStereoMaximumBands];
  int16_t alpha_q_im_prev[JointStereoMaximumGroups][JointStereoMaximumBands];

  uint8_t winSeqPrev;
  uint8_t winShapePrev;
  uint8_t winGroupsPrev;

} CJointStereoPersistentData;

/*!
  \brief Read joint stereo data from bitstream

  The function reads joint stereo data from bitstream.

  \param bs bit stream handle data source.
  \param pJointStereoData pointer to stereo data structure to receive decoded
  data. \param windowGroups number of window groups. \param
  scaleFactorBandsTransmitted number of transmitted scalefactor bands. \param
  flags decoder flags

  \return  0 on success, -1 on error.
*/
int32_t CJointStereo_Read(HANDLE_FDK_BITSTREAM bs,
                      CJointStereoData *pJointStereoData,
                      const int32_t windowGroups,
                      const int32_t scaleFactorBandsTransmitted,
                      const int32_t max_sfb_ste_clear,
                      CJointStereoPersistentData *pJointStereoPersistentData,
                      CCplxPredictionData *cplxPredictionData,
                      int32_t cplxPredictionActiv, int32_t scaleFactorBandsTotal,
                      int32_t windowSequence, const uint32_t flags);

#endif /* #ifndef STEREO_H */
