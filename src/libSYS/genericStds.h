

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




	#ifdef __cplusplus
}
	#endif

#endif /* GENERICSTDS_H */
