/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description:

*******************************************************************************/

#include "aac_ram.h"
#include <stdio.h>
#include "aac_rom.h"

#define WORKBUFFER1_TAG 0
#define WORKBUFFER2_TAG 1
#define WORKBUFFER5_TAG 6
#define WORKBUFFER6_TAG 7

/*! The structure AAC_DECODER_INSTANCE is the top level structure holding all
   decoder configurations, handles and structs.
 */
struct AAC_DECODER_INSTANCE *GetAacDecoder(int n)
{
	return ((struct AAC_DECODER_INSTANCE *)FDKcalloc(1, sizeof(struct AAC_DECODER_INSTANCE)));
}

void FreeAacDecoder(struct AAC_DECODER_INSTANCE **p) {
	if(p != __null) {
		FDKfree(*p);
		*p = __null;
	}
}

CAacDecoderChannelInfo *GetAacDecoderChannelInfo(int n) {
	CAacDecoderChannelInfo *ap;
	ap = ((CAacDecoderChannelInfo *)FDKaalloc((1) * sizeof(CAacDecoderChannelInfo), 8));
	return ap;
}


// UINT GetRequiredMemWorkBufferCore1(void) {
//     UINT a= (((1) * sizeof(CWorkBufferCore1) + 8 + sizeof(void *)) + (((INT)(8) - ((INT)((1) * sizeof(CWorkBufferCore1) + 8 + sizeof(void *)) & ((8) - 1))) & ((8) - 1)));
//     UINT b = sizeof(CWorkBufferCore1) + 8 + sizeof(void *) + ((8 - sizeof(CWorkBufferCore1) + 8 + sizeof(void *) & 7) & 7);
//     printf("a %i, b%i\n", a, b);
//     return a;
// }




//C_ALLOC_MEM_OVERLAY(WorkBufferCore1, CWorkBufferCore1, 1, SECT_DATA_L1, WORKBUFFER1_TAG)

/* double buffer size needed for de-/interleaving */






//C_ALLOC_MEM_OVERLAY(WorkBufferCore5, PCM_DEC, (8) * (1024 * 4) * 2, SECT_DATA_EXTERN, WORKBUFFER5_TAG)
