

/**************************** PCM utility library ******************************

   Author(s):   Arthur Tritthart, Alfonso Pino Garcia

   Description: Functions that perform (de)interleaving combined with format
change

*******************************************************************************/
#include "../libAACdec/newAACDecoder.h"
#include "pcm_utils.h"

/* library version */
#include "version.h"

void FDK_interleave(const int32_t * pIn, int32_t * pOut,
                    const uint32_t channels, const uint32_t frameSize,
                    const uint32_t length) {
  for (uint32_t sample = 0; sample < length; sample++) {
    const int32_t *In = &pIn[sample];
    for (uint32_t ch = 0; ch < channels; ch++) {
      *pOut++ = (int32_t)In[0];
      In += frameSize;
    }
  }
}

void FDK_interleave(const int32_t * pIn, int16_t * pOut,
                    const uint32_t channels, const uint32_t frameSize,
                    const uint32_t length) {
  for (uint32_t sample = 0; sample < length; sample++) {
    const int32_t *In = &pIn[sample];
    for (uint32_t ch = 0; ch < channels; ch++) {
      *pOut++ = (int16_t)FX_DBL2FX_SGL(In[0]);
      In += frameSize;
    }
  }
}

void FDK_interleave(const int16_t * pIn, int16_t * pOut,
                    const uint32_t channels, const uint32_t frameSize,
                    const uint32_t length) {
  for (uint32_t sample = 0; sample < length; sample++) {
    const int16_t *In = &pIn[sample];
    for (uint32_t ch = 0; ch < channels; ch++) {
      *pOut++ = (int16_t)In[0];
      In += frameSize;
    }
  }
}

void FDK_deinterleave(const int32_t * pIn, int16_t * _pOut,
                      const uint32_t channels, const uint32_t frameSize,
                      const uint32_t length) {
  for (uint32_t ch = 0; ch < channels; ch++) {
    int16_t *pOut = _pOut + length * ch;
    const int32_t *In = &pIn[ch];
    for (uint32_t sample = 0; sample < frameSize; sample++) {
      *pOut++ = (int16_t)(In[0] >> 16);
      In += channels;
    }
  }
}

void FDK_deinterleave(const int32_t * pIn, int32_t * _pOut,
                      const uint32_t channels, const uint32_t frameSize,
                      const uint32_t length) {
  for (uint32_t ch = 0; ch < channels; ch++) {
    int32_t *pOut = _pOut + length * ch;
    const int32_t *In = &pIn[ch];
    for (uint32_t sample = 0; sample < frameSize; sample++) {
      *pOut++ = In[0];
      In += channels;
    }
  }
}

void FDK_deinterleave(const int16_t * pIn, int16_t * _pOut,
                      const uint32_t channels, const uint32_t frameSize,
                      const uint32_t length) {
  for (uint32_t ch = 0; ch < channels; ch++) {
    int16_t *pOut = _pOut + length * ch;
    const int16_t *In = &pIn[ch];
    for (uint32_t sample = 0; sample < frameSize; sample++) {
      *pOut++ = In[0];
      In += channels;
    }
  }
}

void FDK_deinterleave(const int16_t * pIn, int32_t * _pOut,
                      const uint32_t channels, const uint32_t frameSize,
                      const uint32_t length) {
  for (uint32_t ch = 0; ch < channels; ch++) {
    int32_t *pOut = _pOut + length * ch;
    const int16_t *In = &pIn[ch];
    for (uint32_t sample = 0; sample < frameSize; sample++) {
      *pOut++ = (int32_t)In[0] << 16;
      In += channels;
    }
  }
}
