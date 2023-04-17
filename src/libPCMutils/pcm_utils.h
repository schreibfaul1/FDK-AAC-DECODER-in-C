

/**************************** PCM utility library ******************************

   Author(s):   Alfonso Pino Garcia

   Description: Functions that perform (de)interleaving combined with format
change

*******************************************************************************/

#if !defined(PCM_UTILS_H)
#define PCM_UTILS_H
#include "../libAACdec/newAACDecoder.h"
#include "../libFDK/common_fix.h"

void FDK_interleave(const int32_t * pIn, int32_t * pOut,
                    const uint32_t channels, const uint32_t frameSize,
                    const uint32_t length);
void FDK_interleave(const int32_t * pIn, int16_t * pOut,
                    const uint32_t channels, const uint32_t frameSize,
                    const uint32_t length);
void FDK_interleave(const int16_t * pIn, int16_t * pOut,
                    const uint32_t channels, const uint32_t frameSize,
                    const uint32_t length);

void FDK_deinterleave(const int32_t * pIn, int16_t * pOut,
                      const uint32_t channels, const uint32_t frameSize,
                      const uint32_t length);
void FDK_deinterleave(const int32_t * pIn, int32_t * pOut,
                      const uint32_t channels, const uint32_t frameSize,
                      const uint32_t length);
void FDK_deinterleave(const int16_t * pIn, int16_t * pOut,
                      const uint32_t channels, const uint32_t frameSize,
                      const uint32_t length);
void FDK_deinterleave(const int16_t * pIn, int32_t * pOut,
                      const uint32_t channels, const uint32_t frameSize,
                      const uint32_t length);
#endif /* !defined(PCM_UTILS_H) */
