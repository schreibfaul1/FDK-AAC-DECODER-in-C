

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

	#define M_PI 3.14159265358979323846 /*!< Pi. Only used in example projects. */


/**
 * Identifiers for various memory locations. They are used along with memory
 * allocation functions like FDKcalloc_L() to specify the requested memory's
 * location.
 */
typedef enum
{
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
	#define C_AALLOC_SCRATCH_START(name, type, n)                    \
		type  _##name[(n) + (ALIGNMENT_DEFAULT + sizeof(type) - 1)]; \
		type *name = (type *)ALIGN_PTR(_##name);

	/** See \ref SYSLIB_MEMORY_MACROS for description. */
	#define C_ALLOC_SCRATCH_START(name, type, n) type name[n];

	/** See \ref SYSLIB_MEMORY_MACROS for description. */
	/** See \ref SYSLIB_MEMORY_MACROS for description. */
	#define C_ALLOC_SCRATCH_END(name, type, n)

	/** See \ref SYSLIB_MEMORY_MACROS for description. */
	#define C_AALLOC_STACK_START(name, type, n)                      \
		type  _##name[(n) + (ALIGNMENT_DEFAULT + sizeof(type) - 1)]; \
		type *name = (type *)ALIGN_PTR(_##name);

/*! @} */

	#define FDK_showBacktrace(a, b)

	/*! \addtogroup SYSLIB_EXITCODES Unified exit codes
	 *  Exit codes to be used as return values of FDK software test and
	 * demonstration applications. Not as return values of product modules and/or
	 * libraries.
	 *  @{
	 */
	#define FDK_EXITCODE_OK 0 /*!< Successful termination. No errors. */
	#define FDK_EXITCODE_USAGE                                                    \
		64 /*!< The command/application was used incorrectly, e.g. with the wrong \
			  number of arguments, a bad flag, a bad syntax in a parameter, or    \
			  whatever. */
	#define FDK_EXITCODE_DATAERROR                                                 \
		65 /*!< The input data was incorrect in some way. This should only be used \
			  for user data and not system files. */
	#define FDK_EXITCODE_NOINPUT                                                     \
		66 /*!< An input file (not a system file) did not exist or was not readable. \
			*/
	#define FDK_EXITCODE_UNAVAILABLE                                                \
		69 /*!< A service is unavailable. This can occur if a support program or    \
			  file does not exist. This can also be used as a catchall message when \
			  something you wanted to do doesn't work, but you don't know why. */
	#define FDK_EXITCODE_SOFTWARE                                                                                \
		70                             /*!< An internal software error has been detected. This should be limited \
										  to non- operating system related errors as possible. */
	#define FDK_EXITCODE_CANTCREATE 73 /*!< A (user specified) output file cannot be created. */
	#define FDK_EXITCODE_IOERROR    74 /*!< An error occurred while doing I/O on some file. */
	/*! @} */

	/*--------------------------------------------
	 * Runtime support declarations
	 *---------------------------------------------*/
	#ifdef __cplusplus
extern "C" {
	#endif


void *FDKcalloc(const uint32_t n, const uint32_t size);


void *FDKmalloc(const uint32_t size);
void  FDKfree(void *ptr);

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


/* Compare function wrappers */
int32_t FDKmemcmp(const void *s1, const void *s2, const uint32_t size);
int32_t FDKstrcmp(const char *s1, const char *s2);
int32_t FDKstrncmp(const char *s1, const char *s2, const uint32_t size);


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
typedef void         FDKFILE;
extern const int32_t FDKSEEK_SET, FDKSEEK_CUR, FDKSEEK_END;

FDKFILE *FDKfopen(const char *filename, const char *mode);
int32_t  FDKfclose(FDKFILE *FP);
int32_t  FDKfseek(FDKFILE *FP, int32_t OFFSET, int32_t WHENCE);
int32_t  FDKftell(FDKFILE *FP);
int32_t  FDKfflush(FDKFILE *fp);
uint32_t FDKfwrite(const void *ptrf, int32_t size, uint32_t nmemb, FDKFILE *fp);
uint32_t FDKfread(void *dst, int32_t size, uint32_t nmemb, FDKFILE *fp);
char    *FDKfgets(void *dst, int32_t size, FDKFILE *fp);
void     FDKrewind(FDKFILE *fp);
int32_t  FDKfeof(FDKFILE *fp);

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
