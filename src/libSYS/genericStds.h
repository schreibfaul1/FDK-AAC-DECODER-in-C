/* -----------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2018 Fraunhofer-Gesellschaft zur Förderung der angewandten
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

/************************* System integration library **************************

   Author(s):

   Description:

*******************************************************************************/

/** \file   genericStds.h
    \brief  Generic Run-Time Support function wrappers and heap allocation
   monitoring.
 */

#if !defined(GENERICSTDS_H)
#define GENERICSTDS_H

#include <stdint.h>
#include "machine_type.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846 /*!< Pi. Only used in example projects. */
#endif

/**
 * Identifiers for various memory locations. They are used along with memory
 * allocation functions like FDKcalloc_L() to specify the requested memory's
 * location.
 */
typedef enum {
  /* Internal */
  SECT_DATA_L1 = 0x2000,
  SECT_DATA_L2,
  SECT_DATA_L1_A,
  SECT_DATA_L1_B,
  SECT_CONSTDATA_L1,

  /* External */
  SECT_DATA_EXTERN = 0x4000,
  SECT_CONSTDATA_EXTERN

} MEMORY_SECTION;

/*! \addtogroup SYSLIB_MEMORY_MACROS FDK memory macros
 *
 * The \c H_ prefix indicates that the macro is to be used in a header file, the
 * \c C_ prefix indicates that the macro is to be used in a source file.
 *
 * Declaring memory areas requires to specify a unique name and a data type.
 *
 * For defining a memory area you require additionally one or two sizes,
 * depending if the memory should be organized into one or two dimensions.
 *
 * The macros containing the keyword \c AALLOC instead of \c ALLOC additionally
 * take care of returning aligned memory addresses (beyond the natural alignment
 * of its type). The preprocesor macro
 * ::ALIGNMENT_DEFAULT indicates the aligment to be used (this is hardware
 * specific).
 *
 * The \c _L suffix indicates that the memory will be located in a specific
 * section. This is useful to allocate critical memory section into fast
 * internal SRAM for example.
 *
 * @{
 */

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define H_ALLOC_MEM(name, type) \
  type *Get##name(int32_t n = 0);   \
  void Free##name(type **p);    \
  uint32_t GetRequiredMem##name(void);

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define H_ALLOC_MEM_OVERLAY(name, type) \
  type *Get##name(int32_t n = 0);           \
  void Free##name(type **p);            \
  uint32_t GetRequiredMem##name(void);

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_ALLOC_MEM(name, type, num)               \
  type *Get##name(int32_t n) {                         \
    FDK_ASSERT((n) == 0);                          \
    return ((type *)FDKcalloc(num, sizeof(type))); \
  }                                                \
  void Free##name(type **p) {                      \
    if (p != NULL) {                               \
      FDKfree(*p);                                 \
      *p = NULL;                                   \
    }                                              \
  }                                                \
  uint32_t GetRequiredMem##name(void) {                \
    return ALGN_SIZE_EXTRES((num) * sizeof(type)); \
  }

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_ALLOC_MEM2(name, type, n1, n2)                 \
  type *Get##name(int32_t n) {                               \
    FDK_ASSERT((n) < (n2));                              \
    return ((type *)FDKcalloc(n1, sizeof(type)));        \
  }                                                      \
  void Free##name(type **p) {                            \
    if (p != NULL) {                                     \
      FDKfree(*p);                                       \
      *p = NULL;                                         \
    }                                                    \
  }                                                      \
  uint32_t GetRequiredMem##name(void) {                      \
    return ALGN_SIZE_EXTRES((n1) * sizeof(type)) * (n2); \
  }

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_AALLOC_MEM(name, type, num)                                  \
  type *Get##name(int32_t n) {                                             \
    type *ap;                                                          \
    FDK_ASSERT((n) == 0);                                              \
    ap = ((type *)FDKaalloc((num) * sizeof(type), ALIGNMENT_DEFAULT)); \
    return ap;                                                         \
  }                                                                    \
  void Free##name(type **p) {                                          \
    if (p != NULL) {                                                   \
      FDKafree(*p);                                                    \
      *p = NULL;                                                       \
    }                                                                  \
  }                                                                    \
  uint32_t GetRequiredMem##name(void) {                                    \
    return ALGN_SIZE_EXTRES((num) * sizeof(type) + ALIGNMENT_DEFAULT + \
                            sizeof(void *));                           \
  }

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_AALLOC_MEM2(name, type, n1, n2)                             \
  type *Get##name(int32_t n) {                                            \
    type *ap;                                                         \
    FDK_ASSERT((n) < (n2));                                           \
    ap = ((type *)FDKaalloc((n1) * sizeof(type), ALIGNMENT_DEFAULT)); \
    return ap;                                                        \
  }                                                                   \
  void Free##name(type **p) {                                         \
    if (p != NULL) {                                                  \
      FDKafree(*p);                                                   \
      *p = NULL;                                                      \
    }                                                                 \
  }                                                                   \
  uint32_t GetRequiredMem##name(void) {                                   \
    return ALGN_SIZE_EXTRES((n1) * sizeof(type) + ALIGNMENT_DEFAULT + \
                            sizeof(void *)) *                         \
           (n2);                                                      \
  }

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_ALLOC_MEM_L(name, type, num, s)               \
  type *Get##name(int32_t n) {                              \
    FDK_ASSERT((n) == 0);                               \
    return ((type *)FDKcalloc_L(num, sizeof(type), s)); \
  }                                                     \
  void Free##name(type **p) {                           \
    if (p != NULL) {                                    \
      FDKfree_L(*p);                                    \
      *p = NULL;                                        \
    }                                                   \
  }                                                     \
  uint32_t GetRequiredMem##name(void) {                     \
    return ALGN_SIZE_EXTRES((num) * sizeof(type));      \
  }

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_ALLOC_MEM2_L(name, type, n1, n2, s)            \
  type *Get##name(int32_t n) {                               \
    FDK_ASSERT((n) < (n2));                              \
    return (type *)FDKcalloc_L(n1, sizeof(type), s);     \
  }                                                      \
  void Free##name(type **p) {                            \
    if (p != NULL) {                                     \
      FDKfree_L(*p);                                     \
      *p = NULL;                                         \
    }                                                    \
  }                                                      \
  uint32_t GetRequiredMem##name(void) {                      \
    return ALGN_SIZE_EXTRES((n1) * sizeof(type)) * (n2); \
  }

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_AALLOC_MEM_L(name, type, num, s)                                  \
  type *Get##name(int32_t n) {                                                  \
    type *ap;                                                               \
    FDK_ASSERT((n) == 0);                                                   \
    ap = ((type *)FDKaalloc_L((num) * sizeof(type), ALIGNMENT_DEFAULT, s)); \
    return ap;                                                              \
  }                                                                         \
  void Free##name(type **p) {                                               \
    if (p != NULL) {                                                        \
      FDKafree_L(*p);                                                       \
      *p = NULL;                                                            \
    }                                                                       \
  }                                                                         \
  uint32_t GetRequiredMem##name(void) {                                         \
    return ALGN_SIZE_EXTRES((num) * sizeof(type) + ALIGNMENT_DEFAULT +      \
                            sizeof(void *));                                \
  }

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_AALLOC_MEM2_L(name, type, n1, n2, s)                             \
  type *Get##name(int32_t n) {                                                 \
    type *ap;                                                              \
    FDK_ASSERT((n) < (n2));                                                \
    ap = ((type *)FDKaalloc_L((n1) * sizeof(type), ALIGNMENT_DEFAULT, s)); \
    return ap;                                                             \
  }                                                                        \
  void Free##name(type **p) {                                              \
    if (p != NULL) {                                                       \
      FDKafree_L(*p);                                                      \
      *p = NULL;                                                           \
    }                                                                      \
  }                                                                        \
  uint32_t GetRequiredMem##name(void) {                                        \
    return ALGN_SIZE_EXTRES((n1) * sizeof(type) + ALIGNMENT_DEFAULT +      \
                            sizeof(void *)) *                              \
           (n2);                                                           \
  }

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_ALLOC_MEM_OVERLAY(name, type, num, sect, tag) \
  C_AALLOC_MEM_L(name, type, num, sect)

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_AALLOC_SCRATCH_START(name, type, n)                 \
  type _##name[(n) + (ALIGNMENT_DEFAULT + sizeof(type) - 1)]; \
  type *name = (type *)ALIGN_PTR(_##name);                    \
  C_ALLOC_ALIGNED_REGISTER(name, (n) * sizeof(type));

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_ALLOC_SCRATCH_START(name, type, n) type name[n];

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_AALLOC_SCRATCH_END(name, type, n) C_ALLOC_ALIGNED_UNREGISTER(name);
/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_ALLOC_SCRATCH_END(name, type, n)

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_AALLOC_STACK_START(name, type, n)                   \
  type _##name[(n) + (ALIGNMENT_DEFAULT + sizeof(type) - 1)]; \
  type *name = (type *)ALIGN_PTR(_##name);                    \
  C_ALLOC_ALIGNED_REGISTER(name, (n) * sizeof(type));

/** See \ref SYSLIB_MEMORY_MACROS for description. */
#define C_AALLOC_STACK_END(name, type, n) C_ALLOC_ALIGNED_UNREGISTER(name);

/*! @} */

#define C_ALLOC_ALIGNED_REGISTER(x, size)
#define C_ALLOC_ALIGNED_UNREGISTER(x)
#define C_ALLOC_ALIGNED_CHECK(x)
#define C_ALLOC_ALIGNED_CHECK2(x, y)
#define FDK_showBacktrace(a, b)

/*! \addtogroup SYSLIB_EXITCODES Unified exit codes
 *  Exit codes to be used as return values of FDK software test and
 * demonstration applications. Not as return values of product modules and/or
 * libraries.
 *  @{
 */
#define FDK_EXITCODE_OK 0 /*!< Successful termination. No errors. */
#define FDK_EXITCODE_USAGE                                                  \
  64 /*!< The command/application was used incorrectly, e.g. with the wrong \
        number of arguments, a bad flag, a bad syntax in a parameter, or    \
        whatever. */
#define FDK_EXITCODE_DATAERROR                                               \
  65 /*!< The input data was incorrect in some way. This should only be used \
        for user data and not system files. */
#define FDK_EXITCODE_NOINPUT                                                   \
  66 /*!< An input file (not a system file) did not exist or was not readable. \
      */
#define FDK_EXITCODE_UNAVAILABLE                                              \
  69 /*!< A service is unavailable. This can occur if a support program or    \
        file does not exist. This can also be used as a catchall message when \
        something you wanted to do doesn't work, but you don't know why. */
#define FDK_EXITCODE_SOFTWARE                                                  \
  70 /*!< An internal software error has been detected. This should be limited \
        to non- operating system related errors as possible. */
#define FDK_EXITCODE_CANTCREATE \
  73 /*!< A (user specified) output file cannot be created. */
#define FDK_EXITCODE_IOERROR \
  74 /*!< An error occurred while doing I/O on some file. */
/*! @} */

/*--------------------------------------------
 * Runtime support declarations
 *---------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

void FDKprintf(const char *szFmt, ...);

void FDKprintfErr(const char *szFmt, ...);

/** Wrapper for <stdio.h>'s getchar(). */
int32_t FDKgetchar(void);

int32_t FDKfprintf(void *stream, const char *format, ...);
int32_t FDKsprintf(char *str, const char *format, ...);

char *FDKstrchr(char *s, int32_t c);
const char *FDKstrstr(const char *haystack, const char *needle);
char *FDKstrcpy(char *dest, const char *src);
char *FDKstrncpy(char *dest, const char *src, const uint32_t n);

#define FDK_MAX_OVERLAYS 8 /**< Maximum number of memory overlays. */

void *FDKcalloc(const uint32_t n, const uint32_t size);
void *FDKcallocExt(const uint32_t n, const uint32_t size, const UCHAR alignment);

void *FDKmalloc(const uint32_t size);
void FDKfree(void *ptr);

/**
 *  Allocate and clear an aligned memory area. Use FDKafree() instead of
 * FDKfree() for these memory areas.
 *
 * \param size       Size of requested memory in bytes.
 * \param alignment  Alignment of requested memory in bytes.
 * \return           Pointer to allocated memory.
 */
void *FDKaalloc(const uint32_t size, const uint32_t alignment);

/**
 *  Free an aligned memory area.
 *
 * \param ptr  Pointer to be freed.
 */
void FDKafree(void *ptr);

/**
 *  Allocate memory in a specific memory section.
 *  Requests can be made for internal or external memory. If internal memory is
 *  requested, FDKcalloc_L() first tries to use L1 memory, which sizes are
 * defined by ::DATA_L1_A_SIZE and ::DATA_L1_B_SIZE. If no L1 memory is
 * available, then FDKcalloc_L() tries to use L2 memory. If that fails as well,
 * the requested memory is allocated at an extern location using the fallback
 * FDKcalloc().
 *
 * \param n     See MSDN documentation on calloc().
 * \param size  See MSDN documentation on calloc().
 * \param s     Memory section.
 * \return      See MSDN documentation on calloc().
 */
void *FDKcalloc_L(const uint32_t n, const uint32_t size, MEMORY_SECTION s);

/**
 *  Allocate aligned memory in a specific memory section.
 *  See FDKcalloc_L() description for details - same applies here.
 */
void *FDKaalloc_L(const uint32_t size, const uint32_t alignment, MEMORY_SECTION s);

/**
 *  Free memory that was allocated in a specific memory section.
 */
void FDKfree_L(void *ptr);

/**
 *  Free aligned memory that was allocated in a specific memory section.
 */
void FDKafree_L(void *ptr);

/**
 * Copy memory. Source and destination memory must not overlap.
 * Either use implementation from a Standard Library, or, if no Standard Library
 * is available, a generic implementation.
 * The define ::USE_BUILTIN_MEM_FUNCTIONS in genericStds.cpp controls what to
 * use. The function arguments correspond to the standard memcpy(). Please see
 * MSDN documentation for details on how to use it.
 */
void FDKmemcpy(void *dst, const void *src, const uint32_t size);

/**
 * Copy memory. Source and destination memory are allowed to overlap.
 * Either use implementation from a Standard Library, or, if no Standard Library
 * is available, a generic implementation.
 * The define ::USE_BUILTIN_MEM_FUNCTIONS in genericStds.cpp controls what to
 * use. The function arguments correspond to the standard memmove(). Please see
 * MSDN documentation for details on how to use it.
 */
void FDKmemmove(void *dst, const void *src, const uint32_t size);

/**
 * Clear memory.
 * Either use implementation from a Standard Library, or, if no Standard Library
 * is available, a generic implementation.
 * The define ::USE_BUILTIN_MEM_FUNCTIONS in genericStds.cpp controls what to
 * use. The function arguments correspond to the standard memclear(). Please see
 * MSDN documentation for details on how to use it.
 */
void FDKmemclear(void *memPtr, const uint32_t size);

/**
 * Fill memory with values.
 * The function arguments correspond to the standard memset(). Please see MSDN
 * documentation for details on how to use it.
 */
void FDKmemset(void *memPtr, const int32_t value, const uint32_t size);

/* Compare function wrappers */
int32_t FDKmemcmp(const void *s1, const void *s2, const uint32_t size);
int32_t FDKstrcmp(const char *s1, const char *s2);
int32_t FDKstrncmp(const char *s1, const char *s2, const uint32_t size);

uint32_t FDKstrlen(const char *s);

#define FDKmax(a, b) ((a) > (b) ? (a) : (b))
#define FDKmin(a, b) ((a) < (b) ? (a) : (b))

#define FDK_INT_MAX ((int32_t)0x7FFFFFFF)
#define FDK_INT_MIN ((int32_t)0x80000000)

/* FILE I/O */

/*!
 *  Check platform for endianess.
 *
 * \return  1 if platform is little endian, non-1 if platform is big endian.
 */
int32_t IS_LITTLE_ENDIAN(void);

/*!
 *  Convert input value to little endian format.
 *
 * \param val  Value to be converted. It may be in both big or little endian.
 * \return     Value in little endian format.
 */
uint32_t TO_LITTLE_ENDIAN(uint32_t val);

/*!
 * \fn     FDKFILE *FDKfopen(const char *filename, const char *mode);
 *         Standard fopen() wrapper.
 * \fn     int32_t FDKfclose(FDKFILE *FP);
 *         Standard fclose() wrapper.
 * \fn     int32_t FDKfseek(FDKFILE *FP, int32_t OFFSET, int32_t WHENCE);
 *         Standard fseek() wrapper.
 * \fn     int32_t FDKftell(FDKFILE *FP);
 *         Standard ftell() wrapper.
 * \fn     int32_t FDKfflush(FDKFILE *fp);
 *         Standard fflush() wrapper.
 * \fn     uint32_t FDKfwrite(const void *ptrf, int32_t size, uint32_t nmemb, FDKFILE *fp);
 *         Standard fwrite() wrapper.
 * \fn     uint32_t FDKfread(void *dst, int32_t size, uint32_t nmemb, FDKFILE *fp);
 *         Standard fread() wrapper.
 */
typedef void FDKFILE;
extern const int32_t FDKSEEK_SET, FDKSEEK_CUR, FDKSEEK_END;

FDKFILE *FDKfopen(const char *filename, const char *mode);
int32_t FDKfclose(FDKFILE *FP);
int32_t FDKfseek(FDKFILE *FP, int32_t OFFSET, int32_t WHENCE);
int32_t FDKftell(FDKFILE *FP);
int32_t FDKfflush(FDKFILE *fp);
uint32_t FDKfwrite(const void *ptrf, int32_t size, uint32_t nmemb, FDKFILE *fp);
uint32_t FDKfread(void *dst, int32_t size, uint32_t nmemb, FDKFILE *fp);
char *FDKfgets(void *dst, int32_t size, FDKFILE *fp);
void FDKrewind(FDKFILE *fp);
int32_t FDKfeof(FDKFILE *fp);

/**
 * \brief        Write each member in little endian order. Convert automatically
 * to host endianess.
 * \param ptrf   Pointer to memory where to read data from.
 * \param size   Size of each item to be written.
 * \param nmemb  Number of items to be written.
 * \param fp     File pointer of type FDKFILE.
 * \return       Number of items read on success and fread() error on failure.
 */
uint32_t FDKfwrite_EL(const void *ptrf, int32_t size, uint32_t nmemb, FDKFILE *fp);

/**
 * \brief        Read variable of size "size" as little endian. Convert
 * automatically to host endianess. 4-byte alignment is enforced for 24 bit
 * data, at 32 bit full scale.
 * \param dst    Pointer to memory where to store data into.
 * \param size   Size of each item to be read.
 * \param nmemb  Number of items to be read.
 * \param fp     File pointer of type FDKFILE.
 * \return       Number of items read on success and fread() error on failure.
 */
uint32_t FDKfread_EL(void *dst, int32_t size, uint32_t nmemb, FDKFILE *fp);

/**
 * \brief  Print FDK software disclaimer.
 */
void FDKprintDisclaimer(void);

#ifdef __cplusplus
}
#endif

#endif /* GENERICSTDS_H */
