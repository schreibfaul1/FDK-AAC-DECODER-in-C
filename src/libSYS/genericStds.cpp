/************************* System integration library **************************

   Author(s):

   Description: - Generic memory, stdio, string, etc. function wrappers or
                builtins.
                - OS dependant function wrappers.

*******************************************************************************/

#include "genericStds.h"
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ESP32
    #include "esp_heap_caps.h"
#endif

//----------------------------------------------------------------------------------------------------------------------

#ifdef ESP32

void *FDKcalloc(const uint32_t n, const uint32_t size) { return FDKcallocExt(n, size, 1); }

// allocate memory with an optional alignment information
void *FDKcallocExt(const uint32_t n, const uint32_t size, const uint8_t alignment) {
    uint8_t alignment_effective = alignment;
    void   *ptr = nullptr;
    //	  if (alignment%4 == 0 || size==4 || size==8 || size>20000){
    if(alignment % 4 == 0) {
        alignment_effective = 4;
        ptr = heap_caps_calloc(n, size, MALLOC_CAP_32BIT);
    }
    else { ptr = heap_caps_calloc(n, size, MALLOC_CAP_8BIT); }
    LOG_FDK(FDKInfo, "==> calloc_align_%d(%d,%d) -> 0x%x [available MEMORY 8BIT : %d ; 32BIT : %d]",
            alignment_effective, n, size, (uint32_t)ptr, heap_caps_get_free_size(MALLOC_CAP_8BIT),
            heap_caps_get_free_size(MALLOC_CAP_32BIT));
    if(ptr == nullptr) {
        LOG_FDK(FDKError, "Memory allocations error!!! -> largest free block [8BIT MEMORY: %d | 32BIT MEMORY: %d]",
                heap_caps_get_largest_free_block(MALLOC_CAP_8BIT), heap_caps_get_largest_free_block(MALLOC_CAP_32BIT));
    }
    return ptr;
}

#else

void *FDKcalloc(const uint32_t n, const uint32_t size) {
    void *ptr;

    ptr = calloc(n, size);
    printf(" ==> calloc(%d,%d) -> %p\n", n, size, ptr);
    if(ptr == nullptr) { printf("Memory allocations error!!! calloc(%d,%d) -> %p\n", n, size, ptr); }

    return ptr;
}

#endif
//----------------------------------------------------------------------------------------------------------------------

void *FDKmalloc(const uint32_t size) {
    void *ptr;

    ptr = malloc(size);
    printf(" ==> malloc(%d) -> %p\n", size, ptr);
    if(ptr == nullptr) { printf("Memory allocations error!!! malloc(%d) -> %p\n", size, ptr); }

    return ptr;
}
//----------------------------------------------------------------------------------------------------------------------
void FDKfree(void *ptr) { free((int32_t *)ptr); }
//----------------------------------------------------------------------------------------------------------------------
void *FDKaalloc(const uint32_t size, const uint32_t alignment) {
    void *addr, *result = NULL;
    addr = FDKcalloc(1, size + alignment + (uint32_t)sizeof(void *)); /* Malloc and clear memory. */
    if(addr != NULL) {
        result = ALIGN_PTR((unsigned char *)addr + sizeof(void *)); /* Get aligned memory base address. */
        *(((void **)result) - 1) = addr;                            /* Save malloc'ed memory pointer.   */
    }

    return result; /* Return aligned address.          */
}
//----------------------------------------------------------------------------------------------------------------------
void FDKafree(void *ptr) {
    void *addr;
    addr = *(((void **)ptr) - 1); /* Get pointer to malloc'ed memory. */
    FDKfree(addr);                /* Free malloc'ed memory area.      */
}
//----------------------------------------------------------------------------------------------------------------------
void *FDKcalloc_L(const uint32_t dim, const uint32_t size, MEMORY_SECTION s) { return FDKcalloc(dim, size); }
//----------------------------------------------------------------------------------------------------------------------
void FDKfree_L(void *p) { FDKfree(p); }
//----------------------------------------------------------------------------------------------------------------------
void *FDKaalloc_L(const uint32_t size, const uint32_t alignment, MEMORY_SECTION s) {
    void *addr, *result = NULL;
    addr = FDKcalloc_L(1, size + alignment + (uint32_t)sizeof(void *), s); /* Malloc and clear memory.         */

    if(addr != NULL) {
        result = ALIGN_PTR((unsigned char *)addr + sizeof(void *)); /* Get aligned memory base address. */
        *(((void **)result) - 1) = addr;                            /* Save malloc'ed memory pointer.   */
    }

    return result; /* Return aligned address.          */
}
//----------------------------------------------------------------------------------------------------------------------
void FDKafree_L(void *ptr) {
    void *addr;

    addr = *(((void **)ptr) - 1); /* Get pointer to malloc'ed memory. */
    FDKfree_L(addr);              /* Free malloc'ed memory area.      */
}

//----------------------------------------------------------------------------------------------------------------------
// FUNCTION:    FDKmemcpy
// DESCRIPTION: - copies memory from "src" to "dst" with length "size" bytes
//              - compiled with FDK_DEBUG will give you warnings

void FDKmemcpy(void *dst, const void *src, const uint32_t size) {
    /* -- check for overlapping memory areas -- */
    assert(((const unsigned char *)dst - (const unsigned char *)src) >= (ptrdiff_t)size ||
           ((const unsigned char *)src - (const unsigned char *)dst) >= (ptrdiff_t)size);

    /* do the copy */
    memcpy(dst, src, size);
}
//----------------------------------------------------------------------------------------------------------------------
void memmove(void *dst, const void *src, const uint32_t size) { memmove(dst, src, size); }

void FDKmemset(void *memPtr, const int32_t value, const uint32_t size) { memset(memPtr, value, size); }

void FDKmemclear(void *memPtr, const uint32_t size) { FDKmemset(memPtr, 0, size); }

uint32_t FDKstrlen(const char *s) { return (uint32_t)strlen(s); }

/* Compare function wrappers */
int32_t FDKmemcmp(const void *s1, const void *s2, const uint32_t size) { return memcmp(s1, s2, size); }
int32_t FDKstrcmp(const char *s1, const char *s2) { return strcmp(s1, s2); }
int32_t FDKstrncmp(const char *s1, const char *s2, const uint32_t size) { return strncmp(s1, s2, size); }

int32_t IS_LITTLE_ENDIAN(void) {
    int32_t __dummy = 1;
    return (*((uint8_t *)(&(__dummy))));
}

uint32_t TO_LITTLE_ENDIAN(uint32_t val) {
    return IS_LITTLE_ENDIAN()
               ? val
               : (((val & 0xff) << 24) | ((val & 0xff00) << 8) | ((val & 0xff0000) >> 8) | ((val & 0xff000000) >> 24));
}

/* ==================== FILE I/O ====================== */

FDKFILE      *FDKfopen(const char *filename, const char *mode) { return fopen(filename, mode); }
int32_t       FDKfclose(FDKFILE *fp) { return fclose((FILE *)fp); }
int32_t       FDKfseek(FDKFILE *fp, int32_t OFFSET, int32_t WHENCE) { return fseek((FILE *)fp, OFFSET, WHENCE); }
int32_t       FDKftell(FDKFILE *fp) { return ftell((FILE *)fp); }
int32_t       FDKfflush(FDKFILE *fp) { return fflush((FILE *)fp); }
const int32_t FDKSEEK_SET = SEEK_SET;
const int32_t FDKSEEK_CUR = SEEK_CUR;
const int32_t FDKSEEK_END = SEEK_END;

uint32_t FDKfwrite(const void *ptrf, int32_t size, uint32_t nmemb, FDKFILE *fp) {
    return (uint32_t)fwrite(ptrf, size, nmemb, (FILE *)fp);
}
uint32_t FDKfread(void *dst, int32_t size, uint32_t nmemb, FDKFILE *fp) {
    return (uint32_t)fread(dst, size, nmemb, (FILE *)fp);
}
char *FDKfgets(void *dst, int32_t size, FDKFILE *fp) { return fgets((char *)dst, size, (FILE *)fp); }
void  FDKrewind(FDKFILE *fp) { FDKfseek((FILE *)fp, 0, FDKSEEK_SET); }

uint32_t FDKfwrite_EL(const void *ptrf, int32_t size, uint32_t nmemb, FDKFILE *fp) {
    if(IS_LITTLE_ENDIAN()) { FDKfwrite(ptrf, size, nmemb, fp); }
    else {
        uint32_t n;
        int32_t  s;

        const uint8_t *ptr = (const uint8_t *)ptrf;

        for(n = 0; n < nmemb; n++) {
            for(s = size - 1; s >= 0; s--) { FDKfwrite(ptr + s, 1, 1, fp); }
            ptr = ptr + size;
        }
    }
    return nmemb;
}
//----------------------------------------------------------------------------------------------------------------------
uint32_t FDKfread_EL(void *dst, int32_t size, uint32_t nmemb, FDKFILE *fp) {
    uint32_t n, s0, s1, err;
    uint8_t  tmp, *ptr;
    uint8_t  tmp24[3];

    /* Enforce alignment of 24 bit data. */
    if(size == 3) {
        ptr = (uint8_t *)dst;
        for(n = 0; n < nmemb; n++) {
            if((err = FDKfread(tmp24, 1, 3, fp)) != 3) { return err; }
            *ptr++ = tmp24[0];
            *ptr++ = tmp24[1];
            *ptr++ = tmp24[2];
            /* Sign extension */
            if(tmp24[2] & 0x80) { *ptr++ = 0xff; }
            else { *ptr++ = 0; }
        }
        err = nmemb;
        size = sizeof(int32_t);
    }
    else {
        if((err = FDKfread(dst, size, nmemb, fp)) != nmemb) { return err; }
    }
    if(!IS_LITTLE_ENDIAN() && size > 1) {
        ptr = (uint8_t *)dst;
        for(n = 0; n < nmemb; n++) {
            for(s0 = 0, s1 = size - 1; s0 < s1; s0++, s1--) {
                tmp = ptr[s0];
                ptr[s0] = ptr[s1];
                ptr[s1] = tmp;
            }
            ptr += size;
        }
    }
    return err;
}
//----------------------------------------------------------------------------------------------------------------------
int32_t FDKfeof(FDKFILE *fp) { return feof((FILE *)fp); }

