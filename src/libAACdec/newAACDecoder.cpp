#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libAACdec/newAACDecoder.h"


//----------------------------------------------------------------------------------------------------------------------
//                ******  M E M O R Y  ******
//----------------------------------------------------------------------------------------------------------------------
#ifdef ESP32
     #include "esp_heap_caps.h"
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
void free(void *ptr) { free((int32_t *)ptr); }
//----------------------------------------------------------------------------------------------------------------------
void *FDKaalloc(const uint32_t size, const uint32_t alignment) {
    void *addr, *result = NULL;
    addr = FDKcalloc(1, size + alignment + (uint32_t)sizeof(void *)); /* Malloc and clear memory. */
    if(addr != NULL) {
        uint8_t* align = (uint8_t*)addr + sizeof(void*); /* Get aligned memory base address. */
        result = (void *)((uint8_t *)(align) + ((((int32_t)8 - ((size_t)(align) & 7)) & 7)));
        *(((void **)result) - 1) = addr;                            /* Save malloc'ed memory pointer.   */
    }

    return result; /* Return aligned address.          */
}
//----------------------------------------------------------------------------------------------------------------------
void FDKafree(void *ptr) {
    void *addr;
    addr = *(((void **)ptr) - 1); /* Get pointer to malloc'ed memory. */
    free(addr);                /* Free malloc'ed memory area.      */
}
//----------------------------------------------------------------------------------------------------------------------
void *FDKcalloc_L(const uint32_t dim, const uint32_t size, MEMORY_SECTION_t s) { return FDKcalloc(dim, size); }
//----------------------------------------------------------------------------------------------------------------------
void FDKfree_L(void *p) { free(p); }
//----------------------------------------------------------------------------------------------------------------------
void *FDKaalloc_L(const uint32_t size, const uint32_t alignment, MEMORY_SECTION_t s) {
    void *addr, *result = NULL;
    addr = FDKcalloc_L(1, size + alignment + (uint32_t)sizeof(void *), s); /* Malloc and clear memory.         */

    if(addr != NULL) {
        uint8_t* align = (uint8_t*)addr + sizeof(void*); /* Get aligned memory base address. */
        result = (void *)((uint8_t *)(align) + ((((int32_t)8 - ((size_t)(align) & 7)) & 7)));
        *(((void **)result) - 1) = addr;   /* Save malloc'ed memory pointer.   */
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