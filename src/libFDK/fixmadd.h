
/******************* Library for basic calculation routines ********************

   Author(s):   M. Lohwasser, M. Gayer

   Description: fixed point intrinsics

*******************************************************************************/

#if !defined(FIXMADD_H)
#define FIXMADD_H
#include "../libAACdec/newAACDecoder.h"
#include "FDK_archdef.h"

#include "fixmul.h"

// #if defined(__arm__)
// #include "arm/fixmadd_arm.h"

// #endif /* all cores */

/*************************************************************************
 *************************************************************************
    Software fallbacks for missing functions.
**************************************************************************
**************************************************************************/

/* Divide by two versions. */

#if !defined(FUNCTION_fixmadddiv2_DD)
inline int32_t fixmadddiv2_DD(int32_t x, const int32_t a, const int32_t b) {
  return (x + fMultDiv2(a, b));
}
#endif

#if !defined(FUNCTION_fixmadddiv2_SD)
inline int32_t fixmadddiv2_SD(int32_t x, const int16_t a, const int32_t b) {
#ifdef FUNCTION_fixmadddiv2_DS
  return fixmadddiv2_DS(x, b, a);
#else
  return fixmadddiv2_DD(x, FX_SGL2FX_DBL(a), b);
#endif
}
#endif

#if !defined(FUNCTION_fixmadddiv2_DS)
inline int32_t fixmadddiv2_DS(int32_t x, const int32_t a, const int16_t b) {
#ifdef FUNCTION_fixmadddiv2_SD
  return fixmadddiv2_SD(x, b, a);
#else
  return fixmadddiv2_DD(x, a, FX_SGL2FX_DBL(b));
#endif
}
#endif

#if !defined(FUNCTION_fixmadddiv2_SS)
inline int32_t fixmadddiv2_SS(int32_t x, const int16_t a, const int16_t b) {
  return x + fMultDiv2(a, b);
}
#endif

#if !defined(FUNCTION_fixmsubdiv2_DD)
inline int32_t fixmsubdiv2_DD(int32_t x, const int32_t a, const int32_t b) {
  return (x - fMultDiv2(a, b));
}
#endif

#if !defined(FUNCTION_fixmsubdiv2_SD)
inline int32_t fixmsubdiv2_SD(int32_t x, const int16_t a, const int32_t b) {
#ifdef FUNCTION_fixmsubdiv2_DS
  return fixmsubdiv2_DS(x, b, a);
#else
  return fixmsubdiv2_DD(x, FX_SGL2FX_DBL(a), b);
#endif
}
#endif

#if !defined(FUNCTION_fixmsubdiv2_DS)
inline int32_t fixmsubdiv2_DS(int32_t x, const int32_t a, const int16_t b) {
#ifdef FUNCTION_fixmsubdiv2_SD
  return fixmsubdiv2_SD(x, b, a);
#else
  return fixmsubdiv2_DD(x, a, FX_SGL2FX_DBL(b));
#endif
}
#endif

#if !defined(FUNCTION_fixmsubdiv2_SS)
inline int32_t fixmsubdiv2_SS(int32_t x, const int16_t a, const int16_t b) {
  return x - fMultDiv2(a, b);
}
#endif

#if !defined(FUNCTION_fixmadddiv2BitExact_DD)
#define FUNCTION_fixmadddiv2BitExact_DD
inline int32_t fixmadddiv2BitExact_DD(int32_t x, const int32_t a,
                                       const int32_t b) {
  return x + fMultDiv2BitExact(a, b);
}
#endif
#if !defined(FUNCTION_fixmadddiv2BitExact_SD)
#define FUNCTION_fixmadddiv2BitExact_SD
inline int32_t fixmadddiv2BitExact_SD(int32_t x, const int16_t a,
                                       const int32_t b) {
#ifdef FUNCTION_fixmadddiv2BitExact_DS
  return fixmadddiv2BitExact_DS(x, b, a);
#else
  return x + fMultDiv2BitExact(a, b);
#endif
}
#endif
#if !defined(FUNCTION_fixmadddiv2BitExact_DS)
#define FUNCTION_fixmadddiv2BitExact_DS
inline int32_t fixmadddiv2BitExact_DS(int32_t x, const int32_t a,
                                       const int16_t b) {
#ifdef FUNCTION_fixmadddiv2BitExact_SD
  return fixmadddiv2BitExact_SD(x, b, a);
#else
  return x + fMultDiv2BitExact(a, b);
#endif
}
#endif

#if !defined(FUNCTION_fixmsubdiv2BitExact_DD)
#define FUNCTION_fixmsubdiv2BitExact_DD
inline int32_t fixmsubdiv2BitExact_DD(int32_t x, const int32_t a,
                                       const int32_t b) {
  return x - fMultDiv2BitExact(a, b);
}
#endif
#if !defined(FUNCTION_fixmsubdiv2BitExact_SD)
#define FUNCTION_fixmsubdiv2BitExact_SD
inline int32_t fixmsubdiv2BitExact_SD(int32_t x, const int16_t a,
                                       const int32_t b) {
#ifdef FUNCTION_fixmsubdiv2BitExact_DS
  return fixmsubdiv2BitExact_DS(x, b, a);
#else
  return x - fMultDiv2BitExact(a, b);
#endif
}
#endif
#if !defined(FUNCTION_fixmsubdiv2BitExact_DS)
#define FUNCTION_fixmsubdiv2BitExact_DS
inline int32_t fixmsubdiv2BitExact_DS(int32_t x, const int32_t a,
                                       const int16_t b) {
#ifdef FUNCTION_fixmsubdiv2BitExact_SD
  return fixmsubdiv2BitExact_SD(x, b, a);
#else
  return x - fMultDiv2BitExact(a, b);
#endif
}
#endif

  /* Normal versions */

#if !defined(FUNCTION_fixmadd_DD)
inline int32_t fixmadd_DD(int32_t x, const int32_t a, const int32_t b) {
  return fixmadddiv2_DD(x, a, b) << 1;
}
#endif
#if !defined(FUNCTION_fixmadd_SD)
inline int32_t fixmadd_SD(int32_t x, const int16_t a, const int32_t b) {
#ifdef FUNCTION_fixmadd_DS
  return fixmadd_DS(x, b, a);
#else
  return fixmadd_DD(x, FX_SGL2FX_DBL(a), b);
#endif
}
#endif
#if !defined(FUNCTION_fixmadd_DS)
inline int32_t fixmadd_DS(int32_t x, const int32_t a, const int16_t b) {
#ifdef FUNCTION_fixmadd_SD
  return fixmadd_SD(x, b, a);
#else
  return fixmadd_DD(x, a, FX_SGL2FX_DBL(b));
#endif
}
#endif
#if !defined(FUNCTION_fixmadd_SS)
inline int32_t fixmadd_SS(int32_t x, const int16_t a, const int16_t b) {
  return (x + fMultDiv2(a, b)) << 1;
}
#endif

#if !defined(FUNCTION_fixmsub_DD)
inline int32_t fixmsub_DD(int32_t x, const int32_t a, const int32_t b) {
  return fixmsubdiv2_DD(x, a, b) << 1;
}
#endif
#if !defined(FUNCTION_fixmsub_SD)
inline int32_t fixmsub_SD(int32_t x, const int16_t a, const int32_t b) {
#ifdef FUNCTION_fixmsub_DS
  return fixmsub_DS(x, b, a);
#else
  return fixmsub_DD(x, FX_SGL2FX_DBL(a), b);
#endif
}
#endif
#if !defined(FUNCTION_fixmsub_DS)
inline int32_t fixmsub_DS(int32_t x, const int32_t a, const int16_t b) {
#ifdef FUNCTION_fixmsub_SD
  return fixmsub_SD(x, b, a);
#else
  return fixmsub_DD(x, a, FX_SGL2FX_DBL(b));
#endif
}
#endif
#if !defined(FUNCTION_fixmsub_SS)
inline int32_t fixmsub_SS(int32_t x, const int16_t a, const int16_t b) {
  return (x - fMultDiv2(a, b)) << 1;
}
#endif

#if !defined(FUNCTION_fixpow2adddiv2_D)
#ifdef FUNCTION_fixmadddiv2_DD
#define fixpadddiv2_D(x, a) fixmadddiv2_DD(x, a, a)
#else
inline int32_t fixpadddiv2_D(int32_t x, const int32_t a) {
  return (x + fPow2Div2(a));
}
#endif
#endif
#if !defined(FUNCTION_fixpow2add_D)
inline int32_t fixpadd_D(int32_t x, const int32_t a) { return (x + fPow2(a)); }
#endif

#if !defined(FUNCTION_fixpow2adddiv2_S)
#ifdef FUNCTION_fixmadddiv2_SS
#define fixpadddiv2_S(x, a) fixmadddiv2_SS(x, a, a)
#else
inline int32_t fixpadddiv2_S(int32_t x, const int16_t a) {
  return (x + fPow2Div2(a));
}
#endif
#endif
#if !defined(FUNCTION_fixpow2add_S)
inline int32_t fixpadd_S(int32_t x, const int16_t a) { return (x + fPow2(a)); }
#endif

#endif /* FIXMADD_H */
