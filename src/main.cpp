
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "libAACdec/aacdecoder_lib.h"

using namespace std;



// Deklaration der benötigten Variablen
HANDLE_AACDECODER handle;
uint8_t inBuff[1024];
int16_t pcmout[4096];
AAC_DECODER_ERROR err;
AAC_DECODER_ERROR decoderErr = AAC_DEC_NOT_ENOUGH_BITS;
int32_t samplesRead = 0;




int32_t main() {

	FILE *fptrIn = NULL;
	fptrIn = fopen("sample1.aac", "r");
	if(fptrIn == NULL) {
		printf("sample1.aac not found\n");
		exit(1);
	}

	FILE *fptrOut = NULL;
	fptrOut = fopen("out.wav", "w");
	if(fptrIn == NULL) {
		printf("out.ogg not found\n");
		exit(1);
	}

	// Write the WAV header
	fwrite("RIFF", 4, 1, fptrOut);              // Chunk ID
	fwrite("\x00\x00\x00\x00", 4, 1, fptrOut);  // Chunk Size (will be filled later)
	fwrite("WAVE", 4, 1, fptrOut);              // Format
	fwrite("fmt ", 4, 1, fptrOut);              // Subchunk 1 ID
	fwrite("\x10\x00\x00\x00", 4, 1, fptrOut);  // Subchunk 1 Size
	fwrite("\x01\x00", 2, 1, fptrOut);          // AudioFormat
	fwrite("\x02\x00", 2, 1, fptrOut);          // NumChannels
	fwrite("\x44\xAC\x00\x00", 4, 1, fptrOut);  // Sample Rate
	fwrite("\x44\xAC\x00\x00", 4, 1, fptrOut);  // Byte Rate
	fwrite("\x02\x00", 2, 1, fptrOut);          // Block Align
	fwrite("\x10\x00", 2, 1, fptrOut);          // Bits Per Sample
	fwrite("data", 4, 1, fptrOut);              // Subchunk 2 ID
	fwrite("\x00\x00\x00\x00", 4, 1, fptrOut);  // Subchunk 2 Size (will be filled later)



	handle = aacDecoder_Open(TT_MP4_ADTS, (uint32_t)1);
    if (!handle){
        printf("Fehler: Dekoder konnte nicht initialisiert werden!\n");
        exit(1);
    }


    // Konfiguration des Dekoders
    // err = aacDecoder_ConfigRaw(handle, NULL, 0);
    // if (err != AAC_DEC_OK) {
    //     printf("Fehler: Konfiguration des Dekoders fehlgeschlagen!\n");
    //     exit(1);
//    }

    // AAC-Daten aus dem Eingabe-Puffer lesen
    // Hier 4096 Bytes als Beispiel
	uint32_t bytesRead = 0;
	uint32_t bytesAvail = 10;

	fpos_t fptr;
	fgetpos(fptrIn, &fptr);
	uint32_t pos = 0;

	while(decoderErr == AAC_DEC_NOT_ENOUGH_BITS || bytesAvail > 0){
	//	fsetpos(fptrIn, &fptr + pos);

		bytesAvail = fread(inBuff, 1, 256, fptrIn);
		if(bytesAvail <=0) break;
		bytesRead = bytesAvail;
    	err = aacDecoder_Fill(handle, inBuff, bytesAvail, &bytesRead);
    	if (err != AAC_DEC_OK)	{
    	    printf("Fehler: AAC-Daten konnten nicht decodiert werden!\n");

    	    exit(1);
    	}
        // printf("bytesAvail %i,  bytesRead %i\n", bytesAvail, bytesRead);
//		pos += bytesRead;

    	// Abruf der dekodierten PCM-Daten aus dem Ausgabe-Puffer
    	decoderErr = aacDecoder_DecodeFrame(handle, pcmout, sizeof(pcmout), 0);
    	if (decoderErr == AAC_DEC_NOT_ENOUGH_BITS){
			// printf("not enough bits");
			continue;
		}
		if(decoderErr != AAC_DEC_OK){
    	    printf("Fehler: PCM-Daten konnten nicht abgerufen werden!\n");
			printf("err %X\n", err);
    	}


		if(decoderErr == AAC_DEC_OK){
			CStreamInfo *info = aacDecoder_GetStreamInfo(handle);
			if (info != NULL) {
			//    printf("Abtastrate: %d Hz\n", info->sampleRate);
			//    printf("Anzahl der Kanäle: %d\n", info->numChannels);
			//    printf("Bitrate: %d kbps\n", info->bitRate / 1000);
			//	printf("valid Samples: %d Hz\n",info->aacSamplesPerFrame);
        	    fwrite(pcmout, 1, info->aacSamplesPerFrame * 2 * info->aacNumChannels, fptrOut);
			}
			else {
			    printf("Fehler: Stream-Informationen konnten nicht abgerufen werden!\n");
			}
		}

	}  // end while(true)
	printf("ready");
	fclose(fptrIn);
	fclose(fptrOut);
	return 0;
}
