

/******************* Library for basic calculation routines ********************

   Author(s):

   Description: matrix memory allocation

*******************************************************************************/

#ifndef FDK_MATRIXCALLOC_H
#define FDK_MATRIXCALLOC_H

#include "../libAACdec/newAACDecoder.h"


/* It is recommended to use FDK_ALLOCATE_MEMORY_1D instead of fdkCallocMatrix1D
 */
void* fdkCallocMatrix1D(uint32_t dim1, uint32_t size);
void* fdkCallocMatrix1D_aligned(uint32_t dim1, uint32_t size);
/* It is recommended to use FDK_ALLOCATE_MEMORY_1D_INT instead of
 * fdkCallocMatrix1D_int */
void* fdkCallocMatrix1D_int(uint32_t dim1, uint32_t size, MEMORY_SECTION_t s);
void* fdkCallocMatrix1D_int_aligned(uint32_t dim1, uint32_t size, MEMORY_SECTION_t s);
/* It is recommended to use FDK_FREE_MEMORY_1D instead of fdkFreeMatrix1D */
void fdkFreeMatrix1D(void* p);
void fdkFreeMatrix1D_aligned(void* p);

/* It is recommended to use FDK_ALLOCATE_MEMORY_2D instead of fdkCallocMatrix2D
 */
void** fdkCallocMatrix2D(uint32_t dim1, uint32_t dim2, uint32_t size);
void** fdkCallocMatrix2D_aligned(uint32_t dim1, uint32_t dim2, uint32_t size);
/* It is recommended to use FDK_ALLOCATE_MEMORY_2D_INT instead of
 * fdkCallocMatrix2D_int */
void** fdkCallocMatrix2D_int(uint32_t dim1, uint32_t dim2, uint32_t size, MEMORY_SECTION_t s);
/* It is recommended to use FDK_ALLOCATE_MEMORY_2D_INT_ALIGNED instead of
 * fdkCallocMatrix2D_int_aligned */
void** fdkCallocMatrix2D_int_aligned(uint32_t dim1, uint32_t dim2, uint32_t size,
                                     MEMORY_SECTION_t s);
/* It is recommended to use FDK_FREE_MEMORY_2D instead of fdkFreeMatrix2D */
void fdkFreeMatrix2D(void** p);
/* It is recommended to use FDK_FREE_MEMORY_2D_ALIGNED instead of
 * fdkFreeMatrix2D_aligned */
void fdkFreeMatrix2D_aligned(void** p);

/* It is recommended to use FDK_ALLOCATE_MEMORY_3D instead of fdkCallocMatrix3D
 */
void*** fdkCallocMatrix3D(uint32_t dim1, uint32_t dim2, uint32_t dim3, uint32_t size);
/* It is recommended to use FDK_ALLOCATE_MEMORY_3D_INT instead of
 * fdkCallocMatrix3D_int */
void*** fdkCallocMatrix3D_int(uint32_t dim1, uint32_t dim2, uint32_t dim3, uint32_t size,
                              MEMORY_SECTION_t s);
/* It is recommended to use FDK_FREE_MEMORY_3D instead of fdkFreeMatrix3D */
void fdkFreeMatrix3D(void*** p);

#define FDK_ALLOCATE_MEMORY_1D(a, dim1, type)                           \
  if (((a) = (type*)fdkCallocMatrix1D((dim1), sizeof(type))) == NULL) { \
    goto bail;                                                          \
  }

#define FDK_ALLOCATE_MEMORY_1D_ALIGNED(a, dim1, type)                   \
  if (((a) = (type*)fdkCallocMatrix1D_aligned((dim1), sizeof(type))) == \
      NULL) {                                                           \
    goto bail;                                                          \
  }

#define FDK_ALLOCATE_MEMORY_1D_P(a, dim1, type, ptype)                  \
  if (((a) = (ptype)fdkCallocMatrix1D((dim1), sizeof(type))) == NULL) { \
    goto bail;                                                          \
  }

#define FDK_ALLOCATE_MEMORY_1D_INT(a, dim1, type, s)                     \
  if (((a) = (type*)fdkCallocMatrix1D_int((dim1), sizeof(type), (s))) == \
      NULL) {                                                            \
    goto bail;                                                           \
  }

#define FDK_FREE_MEMORY_1D(a)    \
  do {                           \
    fdkFreeMatrix1D((void*)(a)); \
    (a) = NULL;                  \
  } while (0)

#define FDK_FREE_MEMORY_1D_ALIGNED(a)    \
  do {                                   \
    fdkFreeMatrix1D_aligned((void*)(a)); \
    (a) = NULL;                          \
  } while (0)

#define FDK_ALLOCATE_MEMORY_2D(a, dim1, dim2, type)                      \
  if (((a) = (type**)fdkCallocMatrix2D((dim1), (dim2), sizeof(type))) == \
      NULL) {                                                            \
    goto bail;                                                           \
  }

#define FDK_ALLOCATE_MEMORY_2D_INT(a, dim1, dim2, type, s)               \
  if (((a) = (type**)fdkCallocMatrix2D_int((dim1), (dim2), sizeof(type), \
                                           (s))) == NULL) {              \
    goto bail;                                                           \
  }

#define FDK_ALLOCATE_MEMORY_2D_INT_ALIGNED(a, dim1, dim2, type, s) \
  if (((a) = (type**)fdkCallocMatrix2D_int_aligned(                \
           (dim1), (dim2), sizeof(type), (s))) == NULL) {          \
    goto bail;                                                     \
  }

#define FDK_FREE_MEMORY_2D(a)     \
  do {                            \
    fdkFreeMatrix2D((void**)(a)); \
    (a) = NULL;                   \
  } while (0)

#define FDK_FREE_MEMORY_2D_ALIGNED(a)     \
  do {                                    \
    fdkFreeMatrix2D_aligned((void**)(a)); \
    (a) = NULL;                           \
  } while (0)

#define FDK_ALLOCATE_MEMORY_3D(a, dim1, dim2, dim3, type)         \
  if (((a) = (type***)fdkCallocMatrix3D((dim1), (dim2), (dim3),   \
                                        sizeof(type))) == NULL) { \
    goto bail;                                                    \
  }

#define FDK_ALLOCATE_MEMORY_3D_INT(a, dim1, dim2, dim3, type, s)           \
  if (((a) = (type***)fdkCallocMatrix3D_int((dim1), (dim2), (dim3),        \
                                            sizeof(type), (s))) == NULL) { \
    goto bail;                                                             \
  }

#define FDK_FREE_MEMORY_3D(a)      \
  do {                             \
    fdkFreeMatrix3D((void***)(a)); \
    (a) = NULL;                    \
  } while (0)

#endif /* FDK_MATRIXCALLOC_H */
