/* -----------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2019 Fraunhofer-Gesellschaft zur Förderung der angewandten
Forschung e.V. All rights reserved.

 1.    INTRODUCTION
The Fraunhofer FDK AAC Codec Library for Android ("FDK AAC Codec") is software
that implements the MPEG Advanced Audio Coding ("AAC") encoding and decoding
scheme for digital audio. This FDK AAC Codec software is intended to be used on
a wide variety of Android devices.

AAC's HE-AAC and HE-AAC v2 versions are regarded as today's most efficient
general perceptual audio codecs. AAC-ELD is considered the best-performing
full-bandwidth communications codec by independent studies and is widely
deployed. AAC has been standardized by ISO and IEC as part of the MPEG
specifications.

Patent licenses for necessary patent claims for the FDK AAC Codec (including
those of Fraunhofer) may be obtained through Via Licensing
(www.vialicensing.com) or through the respective patent owners individually for
the purpose of encoding or decoding bit streams in products that are compliant
with the ISO/IEC MPEG audio standards. Please note that most manufacturers of
Android devices already license these patent claims through Via Licensing or
directly from the patent owners, and therefore FDK AAC Codec software may
already be covered under those patent licenses when it is used for those
licensed purposes only.

Commercially-licensed AAC software libraries, including floating-point versions
with enhanced sound quality, are also available from Fraunhofer. Users are
encouraged to check the Fraunhofer website for additional applications
information and documentation.

2.    COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification,
are permitted without payment of copyright license fees provided that you
satisfy the following conditions:

You must retain the complete text of this software license in redistributions of
the FDK AAC Codec or your modifications thereto in source code form.

You must retain the complete text of this software license in the documentation
and/or other materials provided with redistributions of the FDK AAC Codec or
your modifications thereto in binary form. You must make available free of
charge copies of the complete source code of the FDK AAC Codec and your
modifications thereto to recipients of copies in binary form.

The name of Fraunhofer may not be used to endorse or promote products derived
from this library without prior written permission.

You may not charge copyright license fees for anyone to use, copy or distribute
the FDK AAC Codec software or your modifications thereto.

Your modified versions of the FDK AAC Codec must carry prominent notices stating
that you changed the software and the date of any change. For modified versions
of the FDK AAC Codec, the term "Fraunhofer FDK AAC Codec Library for Android"
must be replaced by the term "Third-Party Modified Version of the Fraunhofer FDK
AAC Codec Library for Android."

3.    NO PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without
limitation the patents of Fraunhofer, ARE GRANTED BY THIS SOFTWARE LICENSE.
Fraunhofer provides no warranty of patent non-infringement with respect to this
software.

You may use this FDK AAC Codec software or modifications thereto only for
purposes that are authorized by appropriate patent licenses.

4.    DISCLAIMER

This FDK AAC Codec software is provided by Fraunhofer on behalf of the copyright
holders and contributors "AS IS" and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
including but not limited to the implied warranties of merchantability and
fitness for a particular purpose. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE for any direct, indirect, incidental, special, exemplary,
or consequential damages, including but not limited to procurement of substitute
goods or services; loss of use, data, or profits, or business interruption,
however caused and on any theory of liability, whether in contract, strict
liability, or tort (including negligence), arising in any way out of the use of
this software, even if advised of the possibility of such damage.

5.    CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Audio and Multimedia Departments - FDK AAC LL
Am Wolfsmantel 33
91058 Erlangen, Germany

www.iis.fraunhofer.de/amm
amm-info@iis.fraunhofer.de
----------------------------------------------------------------------------- */

/******************* Library for basic calculation routines ********************

   Author(s):

   Description: Scaling operations

*******************************************************************************/

#ifndef SCALE_H
#define SCALE_H

#include "common_fix.h"
#include "../libSYS/genericStds.h"
#include "fixminmax.h"

#define SCALE_INLINE

// #if defined(__arm__)
// #include "arm/scale_arm.h"

// #elif defined(__mips__)
// #include "mips/scale_mips.h"

// #endif

void scaleValues(int16_t *vector, int32_t len, int32_t scalefactor);
void scaleValues(int32_t *vector, int32_t len, int32_t scalefactor);
void scaleValues(int32_t *dst, const int32_t *src, int32_t len, int32_t scalefactor);
#if (SAMPLE_BITS == 16)
void scaleValues(FIXP_PCM *dst, const int32_t *src, int32_t len, int32_t scalefactor);
#endif
void scaleValues(int16_t *dst, const int16_t *src, int32_t len, int32_t scalefactor);
void scaleCplxValues(int32_t *r_dst, int32_t *i_dst, const int32_t *r_src,
                     const int32_t *i_src, int32_t len, int32_t scalefactor);
void scaleValuesWithFactor(int32_t *vector, int32_t factor, int32_t len,
                           int32_t scalefactor);
void scaleValuesSaturate(int32_t *vector, int32_t len, int32_t scalefactor);
void scaleValuesSaturate(int32_t *dst, const int32_t *src, int32_t len,
                         int32_t scalefactor);
void scaleValuesSaturate(int16_t *dst, const int32_t *src, int32_t len,
                         int32_t scalefactor);
void scaleValuesSaturate(int16_t *vector, int32_t len, int32_t scalefactor);
void scaleValuesSaturate(int16_t *dst, const int16_t *src, int32_t len,
                         int32_t scalefactor);
int32_t getScalefactorShort(const int16_t *vector, int32_t len);
int32_t getScalefactorPCM(const int16_t *vector, int32_t len, int32_t stride);
int32_t getScalefactor(const int32_t *vector, int32_t len);
int32_t getScalefactor(const int16_t *vector, int32_t len);

#ifndef FUNCTION_scaleValue
/*!
 *
 *  \brief Multiply input by \f$ 2^{scalefactor} \f$
 *
 *  \return Scaled input
 *
 */
#define FUNCTION_scaleValue
inline int32_t scaleValue(const int32_t value, /*!< Value */
                           int32_t scalefactor       /*!< Scalefactor */
) {
  if (scalefactor > 0)
    return (value << scalefactor);
  else
    return (value >> (-scalefactor));
}
inline int16_t scaleValue(const int16_t value, /*!< Value */
                           int32_t scalefactor       /*!< Scalefactor */
) {
  if (scalefactor > 0)
    return (value << scalefactor);
  else
    return (value >> (-scalefactor));
}
#endif

#ifndef FUNCTION_scaleValueSaturate
/*!
 *
 *  \brief Multiply input by \f$ 2^{scalefactor} \f$
 *  \param value The value to be scaled.
 *  \param the shift amount
 *  \return \f$ value * 2^scalefactor \f$
 *
 */
#define FUNCTION_scaleValueSaturate
inline int32_t scaleValueSaturate(const int32_t value,
                                   int32_t scalefactor /* in range -31 ... +31 */
) {
  int32_t headroom = fixnormz_D(
      (int32_t)value ^ (int32_t)((value >> 31))); /* headroom in range 1...32 */
  if (scalefactor >= 0) {
    /* shift left: saturate in case of headroom less/equal scalefactor */
    if (headroom <= scalefactor) {
      if (value > (int32_t)0)
        return (int32_t)MAXVAL_DBL; /* 0x7FFF.FFFF */
      else
        return (int32_t)MINVAL_DBL + (int32_t)1; /* 0x8000.0001 */
    } else {
      return fMax((value << scalefactor), (int32_t)MINVAL_DBL + (int32_t)1);
    }
  } else {
    scalefactor = -scalefactor;
    /* shift right: clear in case of 32-headroom greater/equal -scalefactor */
    if ((DFRACT_BITS - headroom) <= scalefactor) {
      return (int32_t)0;
    } else {
      return fMax((value >> scalefactor), (int32_t)MINVAL_DBL + (int32_t)1);
    }
  }
}
#endif

#ifndef FUNCTION_scaleValueInPlace
/*!
 *
 *  \brief Multiply input by \f$ 2^{scalefactor} \f$ in place
 *
 *  \return void
 *
 */
#define FUNCTION_scaleValueInPlace
inline void scaleValueInPlace(int32_t *value, /*!< Value */
                              int32_t scalefactor  /*!< Scalefactor */
) {
  int32_t newscale;
  /* Note: The assignment inside the if conditional allows combining a load with
   * the compare to zero (on ARM and maybe others) */
  if ((newscale = (scalefactor)) >= 0) {
    *(value) <<= newscale;
  } else {
    *(value) >>= -newscale;
  }
}
#endif

  /*!
   *
   *  \brief  Scale input value by 2^{scale} and saturate output to 2^{dBits-1}
   *  \return scaled and saturated value
   *
   *  This macro scales src value right or left and applies saturation to
   * (2^dBits)-1 maxima output.
   */

#ifndef SATURATE_RIGHT_SHIFT
#define SATURATE_RIGHT_SHIFT(src, scale, dBits)                            \
  ((((int32_t)(src) >> (scale)) > (int32_t)(((1U) << ((dBits)-1)) - 1))          \
       ? (int32_t)(((1U) << ((dBits)-1)) - 1)                                 \
       : (((int32_t)(src) >> (scale)) < ~((int32_t)(((1U) << ((dBits)-1)) - 1))) \
             ? ~((int32_t)(((1U) << ((dBits)-1)) - 1))                        \
             : ((int32_t)(src) >> (scale)))
#endif

#ifndef SATURATE_LEFT_SHIFT
#define SATURATE_LEFT_SHIFT(src, scale, dBits)                           \
  (((int32_t)(src) > ((int32_t)(((1U) << ((dBits)-1)) - 1) >> (scale)))        \
       ? (int32_t)(((1U) << ((dBits)-1)) - 1)                               \
       : ((int32_t)(src) < ~((int32_t)(((1U) << ((dBits)-1)) - 1) >> (scale))) \
             ? ~((int32_t)(((1U) << ((dBits)-1)) - 1))                      \
             : ((int32_t)(src) << (scale)))
#endif

#ifndef SATURATE_SHIFT
#define SATURATE_SHIFT(src, scale, dBits)                        \
  (((scale) < 0) ? SATURATE_LEFT_SHIFT((src), -(scale), (dBits)) \
                 : SATURATE_RIGHT_SHIFT((src), (scale), (dBits)))
#endif

/*
 * Alternative shift and saturate left, saturates to -0.99999 instead of -1.0000
 * to avoid problems when inverting the sign of the result.
 */
#ifndef SATURATE_LEFT_SHIFT_ALT
#define SATURATE_LEFT_SHIFT_ALT(src, scale, dBits)                        \
  (((int32_t)(src) > ((int32_t)(((1U) << ((dBits)-1)) - 1) >> (scale)))         \
       ? (int32_t)(((1U) << ((dBits)-1)) - 1)                                \
       : ((int32_t)(src) <= ~((int32_t)(((1U) << ((dBits)-1)) - 1) >> (scale))) \
             ? ~((int32_t)(((1U) << ((dBits)-1)) - 2))                       \
             : ((int32_t)(src) << (scale)))
#endif

#ifndef SATURATE_RIGHT_SHIFT_ALT
#define SATURATE_RIGHT_SHIFT_ALT(src, scale, dBits)                        \
  ((((int32_t)(src) >> (scale)) > (int32_t)(((1U) << ((dBits)-1)) - 1))          \
       ? (int32_t)(((1U) << ((dBits)-1)) - 1)                                 \
       : (((int32_t)(src) >> (scale)) < ~((int32_t)(((1U) << ((dBits)-1)) - 2))) \
             ? ~((int32_t)(((1U) << ((dBits)-1)) - 2))                        \
             : ((int32_t)(src) >> (scale)))
#endif

#ifndef SATURATE_INT_PCM_RIGHT_SHIFT
#define SATURATE_INT_PCM_RIGHT_SHIFT(src, scale) \
  SATURATE_RIGHT_SHIFT(src, scale, SAMPLE_BITS)
#endif

#ifndef SATURATE_INT_PCM_LEFT_SHIFT
#define SATURATE_INT_PCM_LEFT_SHIFT(src, scale) \
  SATURATE_LEFT_SHIFT(src, scale, SAMPLE_BITS)
#endif

#endif /* #ifndef SCALE_H */
