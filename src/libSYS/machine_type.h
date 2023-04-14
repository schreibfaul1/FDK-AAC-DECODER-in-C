

/************************* System integration library **************************

   Author(s):

   Description:

*******************************************************************************/
#pragma once
/** \file   machine_type.h
 *  \brief  Type defines for various processors and compiler tools.
 */



#include <stddef.h> /* Needed to define size_t */
#include <stdint.h>



typedef int16_t INT_PCM;
#define MAXVAL_PCM MAXVAL_SGL
#define MINVAL_PCM MINVAL_SGL
#define WAV_BITS 16
#define SAMPLE_BITS 16
#define SAMPLE_MAX ((INT_PCM)(((uint32_t)1 << (SAMPLE_BITS - 1)) - 1))
#define SAMPLE_MIN (~SAMPLE_MAX)



#define ALIGNMENT_DEFAULT 8

/* RAM_ALIGN keyword causes memory alignment of global variables. */
#if defined(_MSC_VER)
#define RAM_ALIGN __declspec(align(ALIGNMENT_DEFAULT))
#elif defined(__GNUC__)
#define RAM_ALIGN __attribute__((aligned(ALIGNMENT_DEFAULT)))
#else
#define RAM_ALIGN
#endif

/*-----------------------------------------------------------------------------------
 * ALIGN_SIZE
 *-----------------------------------------------------------------------------------*/
/*!

/*!
 * \brief  This macro aligns a given address depending on ::ALIGNMENT_DEFAULT.
 */
#define ALIGN_PTR(a)                                      \
  ((void *)((unsigned char *)(a) +                        \
            ((((int32_t)ALIGNMENT_DEFAULT -                   \
               ((size_t)(a) & (ALIGNMENT_DEFAULT - 1))) & \
              (ALIGNMENT_DEFAULT - 1)))))

inline int32_t* ALIGN_PTR1(int32_t* a){
         return  a + ((int32_t)8 - ((size_t)a & 7) & 7);
}





/* Alignment macro for libSYS heap implementation */
#define ALIGNMENT_EXTRES (ALIGNMENT_DEFAULT)
#define ALGN_SIZE_EXTRES(a)                                               \
  ((a) + (((int32_t)ALIGNMENT_EXTRES - ((int32_t)(a) & (ALIGNMENT_EXTRES - 1))) & \
          (ALIGNMENT_EXTRES - 1)))

/*!
 * \def  FDK_FORCEINLINE
 *       Sometimes compiler do not do what they are told to do, and in case of
 * inlining some additional command might be necessary depending on the
 * platform.
 *
 * \def  FDK_INLINE
 *       Defines how the compiler is told to inline stuff.
 */
#ifndef FDK_FORCEINLINE
#if defined(__GNUC__) && !defined(__SDE_MIPS__)
#define FDK_FORCEINLINE inline __attribute((always_inline))
#else
#define FDK_FORCEINLINE inline
#endif
#endif

#define FDK_INLINE static inline

/*!
 * \def  LNK_SECTION_DATA_L1
 *       The LNK_SECTION_* defines allow memory to be drawn from specific memory
 *       sections. Used as prefix before variable declaration.
 *
 * \def  LNK_SECTION_DATA_L2
 *       See ::LNK_SECTION_DATA_L1
 * \def  LNK_SECTION_L1_DATA_A
 *       See ::LNK_SECTION_DATA_L1
 * \def  LNK_SECTION_L1_DATA_B
 *       See ::LNK_SECTION_DATA_L1
 * \def  LNK_SECTION_CONSTDATA_L1
 *       See ::LNK_SECTION_DATA_L1
 * \def  LNK_SECTION_CONSTDATA
 *       See ::LNK_SECTION_DATA_L1
 * \def  LNK_SECTION_CODE_L1
 *       See ::LNK_SECTION_DATA_L1
 * \def  LNK_SECTION_CODE_L2
 *       See ::LNK_SECTION_DATA_L1
 * \def  LNK_SECTION_INITCODE
 *       See ::LNK_SECTION_DATA_L1
 */
/**************************************************
 * Code Section macros
 **************************************************/
#define LNK_SECTION_CODE_L1
#define LNK_SECTION_CODE_L2
#define LNK_SECTION_INITCODE

/* Memory section macros. */

/* default fall back */
#define LNK_SECTION_DATA_L1
#define LNK_SECTION_DATA_L2
#define LNK_SECTION_CONSTDATA
#define LNK_SECTION_CONSTDATA_L1

#define LNK_SECTION_L1_DATA_A
#define LNK_SECTION_L1_DATA_B

/**************************************************
 * Macros regarding static code analysis
 **************************************************/
#ifdef __cplusplus
#if !defined(__has_cpp_attribute)
#define __has_cpp_attribute(x) 0
#endif
#if defined(__clang__) && __has_cpp_attribute(clang::fallthrough)
#define FDK_FALLTHROUGH [[clang::fallthrough]]
#endif
#endif

#ifndef FDK_FALLTHROUGH
#if defined(__GNUC__) && (__GNUC__ >= 7)
#define FDK_FALLTHROUGH __attribute__((fallthrough))
#else
#define FDK_FALLTHROUGH
#endif
#endif

#ifdef _MSC_VER
/*
 * Sometimes certain features are excluded from compilation and therefore the
 * warning 4065 may occur: "switch statement contains 'default' but no 'case'
 * labels" We consider this warning irrelevant and disable it.
 */
#pragma warning(disable : 4065)
#endif


