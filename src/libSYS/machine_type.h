

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

#define WAV_BITS 16
#define SAMPLE_BITS 16
#define SAMPLE_MAX ((int16_t)(((uint32_t)1 << (SAMPLE_BITS - 1)) - 1))
#define SAMPLE_MIN (~SAMPLE_MAX)

#define ALIGNMENT_DEFAULT 8
// #define  __attribute__((aligned(ALIGNMENT_DEFAULT)))


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





