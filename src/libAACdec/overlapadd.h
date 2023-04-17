
#ifndef OVERLAPADD_H
#define OVERLAPADD_H
#include "newAACDecoder.h"
#include "../libFDK/common_fix.h"

/* ELD uses different overlap which is twice the frame size: */
#define OverlapBufferSize (768)

typedef int32_t SPECTRUM[1024];


#define SPEC_LONG(ptr) (ptr)
#define SPEC(ptr, w, gl) ((ptr) + ((w) * (gl)))

#define SPEC_TCX(ptr, f, gl, fb) \
  ((ptr) + ((f) * (gl * 2) * (((fb) == 0) ? 1 : 2)))

#endif /* #ifndef OVERLAPADD_H */
