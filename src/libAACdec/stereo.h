

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
                      CJointStereoData_t *pJointStereoData,
                      const int32_t windowGroups,
                      const int32_t scaleFactorBandsTransmitted,
                      const int32_t max_sfb_ste_clear,
                      CJointStereoPersistentData_t *pJointStereoPersistentData,
                      CCplxPredictionData_t *cplxPredictionData,
                      int32_t cplxPredictionActiv, int32_t scaleFactorBandsTotal,
                      int32_t windowSequence, const uint32_t flags);

#endif /* #ifndef STEREO_H */
