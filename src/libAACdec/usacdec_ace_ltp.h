

/**************************** AAC decoder library ******************************

   Author(s):   Matthias Hildenbrand

   Description: USAC ACELP LTP filter

*******************************************************************************/

#ifndef USACDEC_ACE_LTP_H
#define USACDEC_ACE_LTP_H
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"

/**
 * \brief Compute the initial adaptive codebook excitation v'(n) by
 * interpolating the past excitation vector u'(n).
 * \param exc points to adaptive codebook of current subframe (input/output)
 * \param T0 integer part of decoded pitch lag (input)
 * \param frac fractional part of decoded pitch lag (0..3) (input)
 */
void Pred_lt4(int32_t exc[], /* in/out: excitation buffer */
              int32_t T0,         /* input : integer pitch lag */
              int32_t frac        /* input : fraction of lag   */
);

/**
 * \brief Compute the adaptive codebook excitation v(n) in case of
 * ltp_filtering_flag == 0.
 * \param exc points to adaptive codebook of current subframe (input/output)
 */
void Pred_lt4_postfilter(int32_t exc[] /* in/out: excitation buffer */
);

#endif /* USACDEC_ACE_LTP_H */
