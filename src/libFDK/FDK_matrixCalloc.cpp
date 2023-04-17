

/******************* Library for basic calculation routines ********************

   Author(s):

   Description: matrix memory allocation

*******************************************************************************/

#include <stdint.h>
#include "../libAACdec/newAACDecoder.h"
#include "FDK_matrixCalloc.h"



void *fdkCallocMatrix1D_aligned(uint32_t dim1, uint32_t size) {
  return FDKaalloc(dim1 * size, ALIGNMENT_DEFAULT);
}

void *fdkCallocMatrix1D_int(uint32_t dim, uint32_t size, MEMORY_SECTION_t s) {
  return FDKcalloc_L(dim, size, s);
}

void *fdkCallocMatrix1D_int_aligned(uint32_t dim, uint32_t size, MEMORY_SECTION_t s) {
  return FDKaalloc_L(dim * size, ALIGNMENT_DEFAULT, s);
}

void fdkFreeMatrix1D(void *p) {
  if (p != NULL) {
    FDKfree_L(p);
  }
}

void fdkFreeMatrix1D_aligned(void *p) {
  if (p != NULL) {
    FDKafree_L(p);
  }
}

void *fdkCallocMatrix1D(uint32_t dim1, uint32_t size) { return FDKcalloc(dim1, size); }

/* 2D */
void **fdkCallocMatrix2D(uint32_t dim1, uint32_t dim2, uint32_t size) {
  void **p1;
  uint32_t i;
  char *p2;
  if (!dim1 || !dim2) return NULL;
  if ((p1 = (void **)fdkCallocMatrix1D(dim1, sizeof(void *))) == NULL) {
    goto bail;
  }
  if ((p2 = (char *)fdkCallocMatrix1D(dim1 * dim2, size)) == NULL) {
    fdkFreeMatrix1D(p1);
    p1 = NULL;
    goto bail;
  }
  for (i = 0; i < dim1; i++) {
    p1[i] = p2;
    p2 += dim2 * size;
  }
bail:
  return p1;
}

void **fdkCallocMatrix2D_aligned(uint32_t dim1, uint32_t dim2, uint32_t size) {
  void **p1;
  uint32_t i;
  char *p2;
  if (!dim1 || !dim2) return NULL;
  if ((p1 = (void **)fdkCallocMatrix1D(dim1, sizeof(void *))) == NULL) {
    goto bail;
  }
  if ((p2 = (char *)fdkCallocMatrix1D_aligned(dim1 * dim2, size)) == NULL) {
    fdkFreeMatrix1D(p1);
    p1 = NULL;
    goto bail;
  }
  for (i = 0; i < dim1; i++) {
    p1[i] = p2;
    p2 += dim2 * size;
  }
bail:
  return p1;
}

void fdkFreeMatrix2D(void **p) {
  if (!p) return;
  fdkFreeMatrix1D(p[0]);
  fdkFreeMatrix1D(p);
}

void fdkFreeMatrix2D_aligned(void **p) {
  if (!p) return;
  fdkFreeMatrix1D_aligned(p[0]);
  fdkFreeMatrix1D(p);
}

void **fdkCallocMatrix2D_int(uint32_t dim1, uint32_t dim2, uint32_t size,
                             MEMORY_SECTION_t s) {
  void **p1;
  uint32_t i;
  char *p2;

  if (!dim1 || !dim2) return NULL;
  if ((p1 = (void **)fdkCallocMatrix1D_int(dim1, sizeof(void *), s)) == NULL) {
    goto bail;
  }
  if ((p2 = (char *)fdkCallocMatrix1D_int(dim1 * dim2, size, s)) == NULL) {
    fdkFreeMatrix1D(p1);
    p1 = NULL;
    goto bail;
  }
  for (i = 0; i < dim1; i++) {
    p1[i] = p2;
    p2 += dim2 * size;
  }
bail:
  return p1;
}

void **fdkCallocMatrix2D_int_aligned(uint32_t dim1, uint32_t dim2, uint32_t size,
                                     MEMORY_SECTION_t s) {
  void **p1;
  uint32_t i;
  char *p2;

  if (!dim1 || !dim2) return NULL;
  if ((p1 = (void **)fdkCallocMatrix1D_int(dim1, sizeof(void *), s)) == NULL) {
    goto bail;
  }
  if ((p2 = (char *)fdkCallocMatrix1D_int_aligned(dim1 * dim2, size, s)) ==
      NULL) {
    fdkFreeMatrix1D(p1);
    p1 = NULL;
    goto bail;
  }
  for (i = 0; i < dim1; i++) {
    p1[i] = p2;
    p2 += dim2 * size;
  }
bail:
  return p1;
}

/* 3D */
void ***fdkCallocMatrix3D(uint32_t dim1, uint32_t dim2, uint32_t dim3, uint32_t size) {
  void ***p1;
  uint32_t i, j;
  void **p2;
  char *p3;

  if (!dim1 || !dim2 || !dim3) return NULL;
  if ((p1 = (void ***)fdkCallocMatrix1D(dim1, sizeof(void **))) == NULL) {
    goto bail;
  }
  if ((p2 = (void **)fdkCallocMatrix1D(dim1 * dim2, sizeof(void *))) == NULL) {
    fdkFreeMatrix1D(p1);
    p1 = NULL;
    goto bail;
  }
  p1[0] = p2;
  if ((p3 = (char *)fdkCallocMatrix1D(dim1 * dim2 * dim3, size)) == NULL) {
    fdkFreeMatrix1D(p1);
    fdkFreeMatrix1D(p2);
    p1 = NULL;
    p2 = NULL;
    goto bail;
  }
  for (i = 0; i < dim1; i++) {
    p1[i] = p2;
    for (j = 0; j < dim2; j++) {
      p2[j] = p3;
      p3 += dim3 * size;
    }
    p2 += dim2;
  }
bail:
  return p1;
}

void fdkFreeMatrix3D(void ***p) {
  if (!p) return;
  if (p[0] != NULL) fdkFreeMatrix1D(p[0][0]);
  fdkFreeMatrix1D(p[0]);
  fdkFreeMatrix1D(p);
}

void ***fdkCallocMatrix3D_int(uint32_t dim1, uint32_t dim2, uint32_t dim3, uint32_t size,
                              MEMORY_SECTION_t s) {
  void ***p1;
  uint32_t i, j;
  void **p2;
  char *p3;

  if (!dim1 || !dim2 || !dim3) return NULL;
  if ((p1 = (void ***)fdkCallocMatrix1D_int(dim1, sizeof(void **), s)) ==
      NULL) {
    goto bail;
  }
  if ((p2 = (void **)fdkCallocMatrix1D_int(dim1 * dim2, sizeof(void *), s)) ==
      NULL) {
    fdkFreeMatrix1D(p1);
    p1 = NULL;
    goto bail;
  }
  p1[0] = p2;
  if ((p3 = (char *)fdkCallocMatrix1D_int(dim1 * dim2 * dim3, size, s)) ==
      NULL) {
    fdkFreeMatrix1D(p1);
    fdkFreeMatrix1D(p2);
    p1 = NULL;
    p2 = NULL;
    goto bail;
  }
  for (i = 0; i < dim1; i++) {
    p1[i] = p2;
    for (j = 0; j < dim2; j++) {
      p2[j] = p3;
      p3 += dim3 * size;
    }
    p2 += dim2;
  }
bail:
  return p1;
}
