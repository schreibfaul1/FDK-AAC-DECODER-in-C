

/******************* Library for basic calculation routines ********************

   Author(s):   Stefan Gewinner

   Description: fixed point multiplication

*******************************************************************************/

#if !defined(FIXMUL_H)
#define FIXMUL_H
#include "../libAACdec/newAACDecoder.h"


// #if defined(__arm__)
// #include "arm/fixmul_arm.h"

// #elif defined(__mips__)
// #include "mips/fixmul_mips.h"

// #elif defined(__x86__)
// #include "x86/fixmul_x86.h"

// #elif defined(__powerpc__)
// #include "ppc/fixmul_ppc.h"

// #endif /* all cores */

/*************************************************************************
 *************************************************************************
    Software fallbacks for missing functions
**************************************************************************
**************************************************************************/

#if !defined(FUNCTION_fixmuldiv2_DD)
#define FUNCTION_fixmuldiv2_DD
inline int32_t fixmuldiv2_DD(const int32_t a, const int32_t b) {
  return (int32_t)((((int64_t)a) * b) >> 32);
}
#endif

#if !defined(FUNCTION_fixmuldiv2BitExact_DD)
#define FUNCTION_fixmuldiv2BitExact_DD
inline int32_t fixmuldiv2BitExact_DD(const int32_t a, const int32_t b) {
  return (int32_t)((((int64_t)a) * b) >> 32);
}
#endif

#if !defined(FUNCTION_fixmul_DD)
#define FUNCTION_fixmul_DD
inline int32_t fixmul_DD(const int32_t a, const int32_t b) {
  return fixmuldiv2_DD(a, b) << 1;
}
#endif

#if !defined(FUNCTION_fixmulBitExact_DD)
#define FUNCTION_fixmulBitExact_DD
inline int32_t fixmulBitExact_DD(const int32_t a, const int32_t b) {
  return ((int32_t)((((int64_t)a) * b) >> 32)) << 1;
}
#endif

#if !defined(FUNCTION_fixmuldiv2_SS)
#define FUNCTION_fixmuldiv2_SS
inline int32_t fixmuldiv2_SS(const int16_t a, const int16_t b) {
  return ((int32_t)a * b);
}
#endif

#if !defined(FUNCTION_fixmul_SS)
#define FUNCTION_fixmul_SS
inline int32_t fixmul_SS(const int16_t a, const int16_t b) { return (a * b) << 1; }
#endif

#if !defined(FUNCTION_fixmuldiv2_SD)
#define FUNCTION_fixmuldiv2_SD
inline int32_t fixmuldiv2_SD(const int16_t a, const int32_t b)
#ifdef FUNCTION_fixmuldiv2_DS
{
  return fixmuldiv2_DS(b, a);
}
#else
{
  return fixmuldiv2_DD(FX_SGL2FX_DBL(a), b);
}
#endif
#endif

#if !defined(FUNCTION_fixmuldiv2_DS)
#define FUNCTION_fixmuldiv2_DS
inline int32_t fixmuldiv2_DS(const int32_t a, const int16_t b)
#ifdef FUNCTION_fixmuldiv2_SD
{
  return fixmuldiv2_SD(b, a);
}
#else
{
  return fixmuldiv2_DD(a, FX_SGL2FX_DBL(b));
}
#endif
#endif

#if !defined(FUNCTION_fixmuldiv2BitExact_SD)
#define FUNCTION_fixmuldiv2BitExact_SD
inline int32_t fixmuldiv2BitExact_SD(const int16_t a, const int32_t b)
#ifdef FUNCTION_fixmuldiv2BitExact_DS
{
  return fixmuldiv2BitExact_DS(b, a);
}
#else
{
  return (int32_t)((((int64_t)a) * b) >> 16);
}
#endif
#endif

#if !defined(FUNCTION_fixmuldiv2BitExact_DS)
#define FUNCTION_fixmuldiv2BitExact_DS
inline int32_t fixmuldiv2BitExact_DS(const int32_t a, const int16_t b)
#ifdef FUNCTION_fixmuldiv2BitExact_SD
{
  return fixmuldiv2BitExact_SD(b, a);
}
#else
{
  return (int32_t)((((int64_t)a) * b) >> 16);
}
#endif
#endif

#if !defined(FUNCTION_fixmul_SD)
#define FUNCTION_fixmul_SD
inline int32_t fixmul_SD(const int16_t a, const int32_t b) {
#ifdef FUNCTION_fixmul_DS
  return fixmul_DS(b, a);
#else
  return fixmuldiv2_SD(a, b) << 1;
#endif
}
#endif

#if !defined(FUNCTION_fixmul_DS)
#define FUNCTION_fixmul_DS
inline int32_t fixmul_DS(const int32_t a, const int16_t b) {
#ifdef FUNCTION_fixmul_SD
  return fixmul_SD(b, a);
#else
  return fixmuldiv2_DS(a, b) << 1;
#endif
}
#endif

#if !defined(FUNCTION_fixmulBitExact_SD)
#define FUNCTION_fixmulBitExact_SD
inline int32_t fixmulBitExact_SD(const int16_t a, const int32_t b)
#ifdef FUNCTION_fixmulBitExact_DS
{
  return fixmulBitExact_DS(b, a);
}
#else
{
  return (int32_t)(((((int64_t)a) * b) >> 16) << 1);
}
#endif
#endif

#if !defined(FUNCTION_fixmulBitExact_DS)
#define FUNCTION_fixmulBitExact_DS
inline int32_t fixmulBitExact_DS(const int32_t a, const int16_t b)
#ifdef FUNCTION_fixmulBitExact_SD
{
  return fixmulBitExact_SD(b, a);
}
#else
{
  return (int32_t)(((((int64_t)a) * b) >> 16) << 1);
}
#endif
#endif

#if !defined(FUNCTION_fixpow2div2_D)
#define FUNCTION_fixpow2div2_D
inline int32_t fixpow2div2_D(const int32_t a) { return fixmuldiv2_DD(a, a); }
#endif

#if !defined(FUNCTION_fixpow2_D)
#define FUNCTION_fixpow2_D
inline int32_t fixpow2_D(const int32_t a) { return fixpow2div2_D(a) << 1; }
#endif

#if !defined(FUNCTION_fixpow2div2_S)
#define FUNCTION_fixpow2div2_S
inline int32_t fixpow2div2_S(const int16_t a) { return fixmuldiv2_SS(a, a); }
#endif

#if !defined(FUNCTION_fixpow2_S)
#define FUNCTION_fixpow2_S
inline int32_t fixpow2_S(const int16_t a) {
  int32_t result = fixpow2div2_S(a) << 1;
  return result ^ (result >> 31);
}
#endif

#endif /* FIXMUL_H */
