

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







#define SHORT_BITS 16
#define CHAR_BITS 8

/* Define 64 bit base integer type. */
#ifdef _MSC_VER
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else


#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#if ((defined(__i686__) || defined(__i586__) || defined(__i386__) ||  \
      defined(__x86_64__)) ||                                         \
     (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64)))) && \
    !defined(FDK_ASSERT_ENABLE)
#define FDK_ASSERT_ENABLE
#endif

#if defined(FDK_ASSERT_ENABLE)
#include <assert.h>
#define FDK_ASSERT(x) assert(x)
#else
#define FDK_ASSERT(ignore)
#endif

typedef int16_t INT_PCM;
#define MAXVAL_PCM MAXVAL_SGL
#define MINVAL_PCM MINVAL_SGL
#define WAV_BITS 16
#define SAMPLE_BITS 16
#define SAMPLE_MAX ((INT_PCM)(((uint32_t)1 << (SAMPLE_BITS - 1)) - 1))
#define SAMPLE_MIN (~SAMPLE_MAX)

/*!
* \def    RAM_ALIGN
*  Used to align memory as prefix before memory declaration. For example:
   \code
   RAM_ALIGN
   int myArray[16];
   \endcode

   Note, that not all platforms support this mechanism. For example with TI
compilers a preprocessor pragma is used, but to do something like

   \code
   #define RAM_ALIGN #pragma DATA_ALIGN(x)
   \endcode

   would require the preprocessor to process this line twice to fully resolve
it. Hence, a fully platform-independant way to use alignment is not supported.

* \def    ALIGNMENT_DEFAULT
*         Default alignment in bytes.
*/

#define ALIGNMENT_DEFAULT 8

/* RAM_ALIGN keyword causes memory alignment of global variables. */
#if defined(_MSC_VER)
#define RAM_ALIGN __declspec(align(ALIGNMENT_DEFAULT))
#elif defined(__GNUC__)
#define RAM_ALIGN __attribute__((aligned(ALIGNMENT_DEFAULT)))
#else
#define RAM_ALIGN
#endif

/*!
 * \def  RESTRICT
 *       The restrict keyword is supported by some platforms and RESTRICT maps
 * to either the corresponding keyword on each platform or to void if the
 *       compiler does not provide such feature. It tells the compiler that a
 * pointer points to memory that does not overlap with other memories pointed to
 * by other pointers. If this keyword is used and the assumption of no
 * overlap is not true the resulting code might crash.
 *
 * \def  WORD_ALIGNED(x)
 *       Tells the compiler that pointer x is 16 bit aligned. It does not cause
 * the address itself to be aligned, but serves as a hint to the optimizer. The
 * alignment of the pointer must be guarranteed, if not the code might
 * crash.
 *
 * \def  DWORD_ALIGNED(x)
 *       Tells the compiler that pointer x is 32 bit aligned. It does not cause
 * the address itself to be aligned, but serves as a hint to the optimizer. The
 * alignment of the pointer must be guarranteed, if not the code might
 * crash.
 *
 */
#define RESTRICT
#define WORD_ALIGNED(x) C_ALLOC_ALIGNED_CHECK2((const void *)(x), 2);
#define DWORD_ALIGNED(x) C_ALLOC_ALIGNED_CHECK2((const void *)(x), 4);

/*-----------------------------------------------------------------------------------
 * ALIGN_SIZE
 *-----------------------------------------------------------------------------------*/
/*!
 * \brief  This macro aligns a given value depending on ::ALIGNMENT_DEFAULT.
 *
 * For example if #ALIGNMENT_DEFAULT equals 8, then:
 * - ALIGN_SIZE(3) returns 8
 * - ALIGN_SIZE(8) returns 8
 * - ALIGN_SIZE(9) returns 16
 */
#define ALIGN_SIZE(a)                                                          \
  ((a) + (((int32_t)ALIGNMENT_DEFAULT - ((size_t)(a) & (ALIGNMENT_DEFAULT - 1))) & \
          (ALIGNMENT_DEFAULT - 1)))

/*!
 * \brief  This macro aligns a given address depending on ::ALIGNMENT_DEFAULT.
 */
#define ALIGN_PTR(a)                                      \
  ((void *)((unsigned char *)(a) +                        \
            ((((int32_t)ALIGNMENT_DEFAULT -                   \
               ((size_t)(a) & (ALIGNMENT_DEFAULT - 1))) & \
              (ALIGNMENT_DEFAULT - 1)))))

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


