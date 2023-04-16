

/******************* Library for basic calculation routines ********************

   Author(s):   Matthias Hildenbrand

   Description: Module to efficiently handle QMF data for multiple channels and
				to share the data between e.g. SBR and MPS

*******************************************************************************/

#include <memory.h>
#include <stdint.h>
#include "FDK_qmf_domain.h"
#include "common_fix.h"

#define WORKBUFFER1_TAG 0
#define WORKBUFFER3_TAG 4
#define WORKBUFFER4_TAG 5
#define WORKBUFFER6_TAG 7
#define WORKBUFFER7_TAG 8

C_ALLOC_MEM_OVERLAY(QmfWorkBufferCore1, int32_t, QMF_WB_SECTION_SIZE, SECT_DATA_L1, WORKBUFFER1_TAG)
C_ALLOC_MEM_OVERLAY(QmfWorkBufferCore3, int32_t, QMF_WB_SECTION_SIZE, SECT_DATA_L2, WORKBUFFER3_TAG)
C_ALLOC_MEM_OVERLAY(QmfWorkBufferCore4, int32_t, QMF_WB_SECTION_SIZE, SECT_DATA_L2, WORKBUFFER4_TAG)
C_ALLOC_MEM_OVERLAY(QmfWorkBufferCore6, int32_t, QMF_WB_SECTION_SIZE, SECT_DATA_L2, WORKBUFFER6_TAG)
C_ALLOC_MEM_OVERLAY(QmfWorkBufferCore7, int32_t, QMF_WB_SECTION_SIZE, SECT_DATA_L2, WORKBUFFER7_TAG)




static int32_t FDK_QmfDomain_FreePersistentMemory(HANDLE_FDK_QMF_DOMAIN qd) {
	int32_t err = 0;
	int32_t ch;

	for(ch = 0; ch < ((8) + (1)); ch++) {
		if(qd->QmfDomainIn[ch].pAnaQmfStates) {
			if(qd->globalConf.nBandsAnalysis == QMF_DOMAIN_ANALYSIS_QMF_BANDS_16) {
				FDKafree(qd->QmfDomainIn[ch].pAnaQmfStates);
				qd->QmfDomainIn[ch].pAnaQmfStates = NULL;
			}
			else if(qd->globalConf.nBandsAnalysis == QMF_DOMAIN_ANALYSIS_QMF_BANDS_24) {
				FDKafree(qd->QmfDomainIn[ch].pAnaQmfStates);
				qd->QmfDomainIn[ch].pAnaQmfStates = NULL;
			}
			else if(qd->globalConf.nBandsAnalysis == QMF_DOMAIN_ANALYSIS_QMF_BANDS_32) {
				FDKafree(qd->QmfDomainIn[ch].pAnaQmfStates);
				qd->QmfDomainIn[ch].pAnaQmfStates = NULL;
			}
			else {
				FDKafree(qd->QmfDomainIn[ch].pAnaQmfStates);
				qd->QmfDomainIn[ch].pAnaQmfStates = NULL;
			}
		}

		if(qd->QmfDomainIn[ch].pOverlapBuffer) {
			if(qd->globalConf.nQmfOvTimeSlots == QMF_DOMAIN_OV_TIMESLOTS_16) {
				FDKafree(qd->QmfDomainIn[ch].pOverlapBuffer);
				qd->QmfDomainIn[ch].pOverlapBuffer = NULL;
			}
			else if(qd->globalConf.nQmfOvTimeSlots == QMF_DOMAIN_OV_TIMESLOTS_32) {
				FDKafree(qd->QmfDomainIn[ch].pOverlapBuffer);
				qd->QmfDomainIn[ch].pOverlapBuffer = NULL;
			}
			else {
				FDKafree(qd->QmfDomainIn[ch].pOverlapBuffer);
				qd->QmfDomainIn[ch].pOverlapBuffer = NULL;
			}
		}

		if(qd->QmfDomainIn[ch].hQmfSlotsReal) {
			if(qd->globalConf.nQmfTimeSlots == QMF_DOMAIN_TIMESLOTS_16) {
				FDKfree(qd->QmfDomainIn[ch].hQmfSlotsReal);
				qd->QmfDomainIn[ch].hQmfSlotsReal = NULL;
			}
			else if(qd->globalConf.nQmfTimeSlots == QMF_DOMAIN_TIMESLOTS_32) {
				FDKfree(qd->QmfDomainIn[ch].hQmfSlotsReal);
				qd->QmfDomainIn[ch].hQmfSlotsReal = NULL;
			}
			else {
				FDKfree(qd->QmfDomainIn[ch].hQmfSlotsReal);
				qd->QmfDomainIn[ch].hQmfSlotsReal = NULL;
            }
		}

		if(qd->QmfDomainIn[ch].hQmfSlotsImag) {
			if(qd->globalConf.nQmfTimeSlots == QMF_DOMAIN_TIMESLOTS_16) {
				FDKfree(qd->QmfDomainIn[ch].hQmfSlotsImag);
				qd->QmfDomainIn[ch].hQmfSlotsImag = NULL;
			}
			if(qd->globalConf.nQmfTimeSlots == QMF_DOMAIN_TIMESLOTS_32) {
				FDKfree(qd->QmfDomainIn[ch].hQmfSlotsImag);
				qd->QmfDomainIn[ch].hQmfSlotsImag = NULL;
			}
			else {
				FDKfree(qd->QmfDomainIn[ch].hQmfSlotsImag);
				qd->QmfDomainIn[ch].hQmfSlotsImag = NULL;
			}
		}
	}

	for(ch = 0; ch < ((8) + (1)); ch++) {
		if(qd->QmfDomainOut[ch].pSynQmfStates) {
			FDKafree(qd->QmfDomainOut[ch].pSynQmfStates);
			qd->QmfDomainOut[ch].pSynQmfStates = NULL;
		}
	}

	return err;
}

static int32_t FDK_QmfDomain_AllocatePersistentMemory(HANDLE_FDK_QMF_DOMAIN qd) {
	int32_t                      err = 0;
	int32_t                      ch;
	HANDLE_FDK_QMF_DOMAIN_GC gc = &qd->globalConf;

	if((gc->nInputChannels > ((8) + (1))) || (gc->nOutputChannels > ((8) + (1)))) return err = 1;
	for(ch = 0; ch < gc->nInputChannels; ch++) {
		int32_t size;

		size = gc->nBandsAnalysis * 10;
		if(size > 0) {
			if(gc->nBandsAnalysis == QMF_DOMAIN_ANALYSIS_QMF_BANDS_16) {
				if(qd->QmfDomainIn[ch].pAnaQmfStates == NULL) {
					qd->QmfDomainIn[ch].pAnaQmfStates = (int32_t *)FDKaalloc((10 * (16)) * sizeof(int32_t), 8);
					if(qd->QmfDomainIn[ch].pAnaQmfStates == NULL) goto bail;
				}
			}
			else if(gc->nBandsAnalysis == QMF_DOMAIN_ANALYSIS_QMF_BANDS_24) {
				if(qd->QmfDomainIn[ch].pAnaQmfStates == NULL) {
					qd->QmfDomainIn[ch].pAnaQmfStates = (int32_t *)FDKaalloc((10 * (24)) * sizeof(int32_t), 8);
					if(qd->QmfDomainIn[ch].pAnaQmfStates == NULL) goto bail;
				}
			}
			else if(gc->nBandsAnalysis == QMF_DOMAIN_ANALYSIS_QMF_BANDS_32) {
				if(qd->QmfDomainIn[ch].pAnaQmfStates == NULL) {
					qd->QmfDomainIn[ch].pAnaQmfStates = (int32_t *)FDKaalloc((10 * (32)) * sizeof(int32_t), 8);
					if(qd->QmfDomainIn[ch].pAnaQmfStates == NULL) goto bail;
				}
			}
			else {
				if(qd->QmfDomainIn[ch].pAnaQmfStates == NULL) {
					qd->QmfDomainIn[ch].pAnaQmfStates = (int32_t *)FDKaalloc((10 * (64)) * sizeof(int32_t), 8);
					if(qd->QmfDomainIn[ch].pAnaQmfStates == NULL) goto bail;
				}
			}
		}
		else { qd->QmfDomainIn[ch].pAnaQmfStates = NULL; }

		size = gc->nQmfOvTimeSlots + gc->nQmfTimeSlots;
		if(size > 0) {
			if(gc->nQmfTimeSlots == QMF_DOMAIN_TIMESLOTS_16) {
				if(qd->QmfDomainIn[ch].hQmfSlotsReal == NULL) {
					qd->QmfDomainIn[ch].hQmfSlotsReal = (int32_t * *)FDKcalloc((16) + (3), sizeof(int32_t *));
					if(qd->QmfDomainIn[ch].hQmfSlotsReal == NULL) goto bail;
				}
				if(qd->QmfDomainIn[ch].hQmfSlotsImag == NULL) {
					qd->QmfDomainIn[ch].hQmfSlotsImag = (int32_t * *)FDKcalloc((16) + (3), sizeof(int32_t *));
					if(qd->QmfDomainIn[ch].hQmfSlotsImag == NULL) goto bail;
				}
			}
			else if(gc->nQmfTimeSlots == QMF_DOMAIN_TIMESLOTS_32) {
				if(qd->QmfDomainIn[ch].hQmfSlotsReal == NULL) {
					qd->QmfDomainIn[ch].hQmfSlotsReal = (int32_t * *)FDKcalloc((32) + (6), sizeof(int32_t *));
					if(qd->QmfDomainIn[ch].hQmfSlotsReal == NULL) goto bail;
				}
				if(qd->QmfDomainIn[ch].hQmfSlotsImag == NULL) {
					qd->QmfDomainIn[ch].hQmfSlotsImag = (int32_t * *)FDKcalloc((32) + (6), sizeof(int32_t *));
					if(qd->QmfDomainIn[ch].hQmfSlotsImag == NULL) goto bail;
				}
			}
			else {
				if(qd->QmfDomainIn[ch].hQmfSlotsReal == NULL) {
					qd->QmfDomainIn[ch].hQmfSlotsReal = (int32_t * *)FDKcalloc((64) + (12), sizeof(int32_t *));
					if(qd->QmfDomainIn[ch].hQmfSlotsReal == NULL)goto bail;
				}
				if(qd->QmfDomainIn[ch].hQmfSlotsImag == NULL) {
					qd->QmfDomainIn[ch].hQmfSlotsImag = (int32_t * *)FDKcalloc((64) + (12), sizeof(int32_t *));
					if(qd->QmfDomainIn[ch].hQmfSlotsImag == NULL) goto bail;
				}
			}
		}
		else {
			qd->QmfDomainIn[ch].hQmfSlotsReal = NULL;
			qd->QmfDomainIn[ch].hQmfSlotsImag = NULL;
		}

		size = gc->nQmfOvTimeSlots * gc->nQmfProcBands * CMPLX_MOD;
		if(size > 0) {
			if(gc->nQmfOvTimeSlots == QMF_DOMAIN_OV_TIMESLOTS_16) {
				if(qd->QmfDomainIn[ch].pOverlapBuffer == NULL) {
					qd->QmfDomainIn[ch].pOverlapBuffer = (int32_t *)FDKaalloc((2 * (3) * (64)) * sizeof(int32_t), 8);
					if(qd->QmfDomainIn[ch].pOverlapBuffer == NULL) goto bail;
				}
			}
			else if(gc->nQmfOvTimeSlots == QMF_DOMAIN_OV_TIMESLOTS_32) {
				if(qd->QmfDomainIn[ch].pOverlapBuffer == NULL) {
					qd->QmfDomainIn[ch].pOverlapBuffer = (int32_t*)FDKaalloc((2 * (6) * (64)) * sizeof(int32_t), 8);
					if(qd->QmfDomainIn[ch].pOverlapBuffer == NULL) goto bail;
				}
			}
			else {
				if(qd->QmfDomainIn[ch].pOverlapBuffer == NULL) {
					qd->QmfDomainIn[ch].pOverlapBuffer = (int32_t *)FDKaalloc((2 * (12) * (64)) * sizeof(int32_t), 8);
					if(qd->QmfDomainIn[ch].pOverlapBuffer == NULL) goto bail;
				}
			}
		}
		else { qd->QmfDomainIn[ch].pOverlapBuffer = NULL; }
	}

	for(ch = 0; ch < gc->nOutputChannels; ch++) {
		int32_t size = gc->nBandsSynthesis * 9;
		if(size > 0) {
			if(qd->QmfDomainOut[ch].pSynQmfStates == NULL) {
				qd->QmfDomainOut[ch].pSynQmfStates = (int32_t *)FDKaalloc((9 * ((64))) * sizeof(int32_t), 8);
				if(qd->QmfDomainOut[ch].pSynQmfStates == NULL) goto bail;
			}
		}
		else { qd->QmfDomainOut[ch].pSynQmfStates = NULL; }
	}

	return err;

bail:
	FDK_QmfDomain_FreePersistentMemory(qd);
	return -1;
}

QMF_DOMAIN_ERROR FDK_QmfDomain_ClearPersistentMemory(HANDLE_FDK_QMF_DOMAIN hqd) {
	QMF_DOMAIN_ERROR err = QMF_DOMAIN_OK;
	int32_t              ch, size;
	if(hqd) {
		HANDLE_FDK_QMF_DOMAIN_GC gc = &hqd->globalConf;

		size = gc->nQmfOvTimeSlots * gc->nQmfProcBands * CMPLX_MOD;
		for(ch = 0; ch < gc->nInputChannels; ch++) {
			if(hqd->QmfDomainIn[ch].pOverlapBuffer) {
				memset(hqd->QmfDomainIn[ch].pOverlapBuffer, 0, size * sizeof(int32_t));
			}
		}
		if(FDK_QmfDomain_InitFilterBank(hqd, 0)) { err = QMF_DOMAIN_INIT_ERROR; }
	}
	else { err = QMF_DOMAIN_INIT_ERROR; }
	return err;
}

/*
   FDK_getWorkBuffer

	Parameters:

	pWorkBuffer        i: array of pointers which point to different workbuffer
   sections workBufferOffset   i: offset in the workbuffer to the requested
   memory memSize            i: size of requested memory

	Function:

	The functions returns the address to the requested memory in the workbuffer.

	The overall workbuffer is divided into several sections. There are
   QMF_MAX_WB_SECTIONS sections of size QMF_WB_SECTION_SIZE. The function
   selects the workbuffer section with the help of the workBufferOffset and than
   it verifies whether the requested amount of memory fits into the selected
   workbuffer section.

	Returns:

	address to workbuffer
*/
static int32_t *FDK_getWorkBuffer(int32_t **pWorkBuffer, uint16_t workBufferOffset, uint16_t workBufferSectSize,
								   uint16_t memSize) {
	int32_t       idx1;
	int32_t       idx2;
	int32_t *pwb;

	/* a section must be a multiple of the number of processing bands (currently
	 * always 64) */
	assert((workBufferSectSize % 64) == 0);

	/* calculate offset within the section */
	idx2 = workBufferOffset % workBufferSectSize;
	/* calculate section number */
	idx1 = (workBufferOffset - idx2) / workBufferSectSize;
	/* maximum sectionnumber is QMF_MAX_WB_SECTIONS */
	assert(idx1 < QMF_MAX_WB_SECTIONS);

	/* check, whether workbuffer is available  */
	assert(pWorkBuffer[idx1] != NULL);

	/* check, whether buffer fits into selected section */
	assert((idx2 + memSize) <= workBufferSectSize);

	/* get requested address to workbuffer */
	pwb = &pWorkBuffer[idx1][idx2];

	return pwb;
}

static int32_t FDK_QmfDomain_FeedWorkBuffer(HANDLE_FDK_QMF_DOMAIN qd, int32_t ch, int32_t **pWorkBuffer,
										uint16_t workBufferOffset, uint16_t workBufferSectSize, int32_t size) {
	int32_t err = 0;
	int32_t mem_needed;

	mem_needed = qd->QmfDomainIn[ch].workBuf_nBands * qd->QmfDomainIn[ch].workBuf_nTimeSlots * CMPLX_MOD;
	if(mem_needed > size) { return (err = 1); }
	qd->QmfDomainIn[ch].pWorkBuffer = pWorkBuffer;
	qd->QmfDomainIn[ch].workBufferOffset = workBufferOffset;
	qd->QmfDomainIn[ch].workBufferSectSize = workBufferSectSize;

	return err;
}

int32_t FDK_QmfDomain_IsInitialized(const HANDLE_FDK_QMF_DOMAIN qd) {
	assert(qd != NULL);
	return ((qd->QmfDomainIn[0].pAnaQmfStates == NULL) && (qd->QmfDomainOut[0].pSynQmfStates == NULL)) ? 0 : 1;
}

int32_t FDK_QmfDomain_InitFilterBank(HANDLE_FDK_QMF_DOMAIN qd, uint32_t extra_flags) {
	assert(qd != NULL);
	int32_t                      err = 0;
	int32_t                      ch, ts;
	HANDLE_FDK_QMF_DOMAIN_GC gc = &qd->globalConf;
	int32_t                      noCols = gc->nQmfTimeSlots;
	int32_t                      lsb = gc->nBandsAnalysis;
	int32_t                      usb = fMin((int32_t)gc->nBandsSynthesis, 64);
	int32_t                      nProcBands = gc->nQmfProcBands;
	assert(nProcBands % ALIGNMENT_DEFAULT == 0);

	if(extra_flags & QMF_FLAG_MPSLDFB) {
		gc->flags &= ~QMF_FLAG_CLDFB;
		gc->flags |= QMF_FLAG_MPSLDFB;
	}
	for(ch = 0; ch < gc->nInputChannels; ch++) {
		/* distribute memory to slots array */
		int32_t *ptrOv = qd->QmfDomainIn[ch].pOverlapBuffer; /* persistent memory for overlap */
		if((ptrOv == NULL) && (gc->nQmfOvTimeSlots != 0)) {
			err = 1;
			return err;
		}
		/* This assumes the workbuffer defined for ch0 is the big one being used to
		 * hold one full frame of QMF data. */
		int32_t **ptr = qd->QmfDomainIn[fMin(ch, fMax((int32_t)gc->nQmfProcChannels - 1, 0))]
							 .pWorkBuffer; /* non-persistent workbuffer */
		uint16_t workBufferOffset = qd->QmfDomainIn[fMin(ch, fMax((int32_t)gc->nQmfProcChannels - 1, 0))].workBufferOffset;
		uint16_t workBufferSectSize =
			qd->QmfDomainIn[fMin(ch, fMax((int32_t)gc->nQmfProcChannels - 1, 0))].workBufferSectSize;

		if((ptr == NULL) && (gc->nQmfTimeSlots != 0)) {
			err = 1;
			return err;
		}

		qd->QmfDomainIn[ch].pGlobalConf = gc;
		for(ts = 0; ts < gc->nQmfOvTimeSlots; ts++) {
			qd->QmfDomainIn[ch].hQmfSlotsReal[ts] = ptrOv;
			ptrOv += nProcBands;
			qd->QmfDomainIn[ch].hQmfSlotsImag[ts] = ptrOv;
			ptrOv += nProcBands;
		}
		for(; ts < (gc->nQmfOvTimeSlots + gc->nQmfTimeSlots); ts++) {
			qd->QmfDomainIn[ch].hQmfSlotsReal[ts] =
				FDK_getWorkBuffer(ptr, workBufferOffset, workBufferSectSize, nProcBands);
			workBufferOffset += nProcBands;
			qd->QmfDomainIn[ch].hQmfSlotsImag[ts] =
				FDK_getWorkBuffer(ptr, workBufferOffset, workBufferSectSize, nProcBands);
			workBufferOffset += nProcBands;
		}
		err |= qmfInitAnalysisFilterBank(&qd->QmfDomainIn[ch].fb, qd->QmfDomainIn[ch].pAnaQmfStates, noCols,
										 (qd->QmfDomainIn[ch].fb.lsb == 0) ? lsb : qd->QmfDomainIn[ch].fb.lsb,
										 (qd->QmfDomainIn[ch].fb.usb == 0) ? usb : qd->QmfDomainIn[ch].fb.usb,
										 gc->nBandsAnalysis, gc->flags | extra_flags);
	}

	for(ch = 0; ch < gc->nOutputChannels; ch++) {
		int32_t outGain_m = qd->QmfDomainOut[ch].fb.outGain_m;
		int32_t      outGain_e = qd->QmfDomainOut[ch].fb.outGain_e;
		int32_t      outScale = qmfGetOutScalefactor(&qd->QmfDomainOut[ch].fb);
		err |= qmfInitSynthesisFilterBank(&qd->QmfDomainOut[ch].fb, qd->QmfDomainOut[ch].pSynQmfStates, noCols,
										  (qd->QmfDomainOut[ch].fb.lsb == 0) ? lsb : qd->QmfDomainOut[ch].fb.lsb,
										  (qd->QmfDomainOut[ch].fb.usb == 0) ? usb : qd->QmfDomainOut[ch].fb.usb,
										  gc->nBandsSynthesis, gc->flags | extra_flags);
		if(outGain_m != (int32_t)0) { qmfChangeOutGain(&qd->QmfDomainOut[ch].fb, outGain_m, outGain_e); }
		if(outScale) { qmfChangeOutScalefactor(&qd->QmfDomainOut[ch].fb, outScale); }
	}

	return err;
}

void FDK_QmfDomain_SaveOverlap(HANDLE_FDK_QMF_DOMAIN_IN qd_ch, int32_t offset) {
	assert(qd_ch != NULL);
	int32_t                      ts;
	HANDLE_FDK_QMF_DOMAIN_GC gc = qd_ch->pGlobalConf;
	int32_t                      ovSlots = gc->nQmfOvTimeSlots;
	int32_t                      nCols = gc->nQmfTimeSlots;
	int32_t                      nProcBands = gc->nQmfProcBands;
	int32_t               **qmfReal = qd_ch->hQmfSlotsReal;
	int32_t               **qmfImag = qd_ch->hQmfSlotsImag;
	QMF_SCALE_FACTOR        *pScaling = &qd_ch->scaling;

	/* for high part it would be enough to save only used part of overlap area */
	if(qmfImag != NULL) {
		for(ts = offset; ts < ovSlots; ts++) {
			memcpy(qmfReal[ts], qmfReal[nCols + ts], sizeof(int32_t) * nProcBands);
			memcpy(qmfImag[ts], qmfImag[nCols + ts], sizeof(int32_t) * nProcBands);
		}
	}
	else {
		for(ts = 0; ts < ovSlots; ts++) { memcpy(qmfReal[ts], qmfReal[nCols + ts], sizeof(int32_t) * nProcBands); }
	}
	pScaling->ov_lb_scale = pScaling->lb_scale;
}

/* Convert headroom bits to exponent */
#define SCALE2EXP(s) (15 - (s))
#define EXP2SCALE(e) (15 - (e))

void FDK_QmfDomain_GetSlot(const HANDLE_FDK_QMF_DOMAIN_IN qd_ch, const int32_t ts, const int32_t start_band,
						   const int32_t stop_band, int32_t *pQmfOutReal, int32_t *pQmfOutImag, const int32_t exp_out) {
	assert(qd_ch != NULL);
	assert(pQmfOutReal != NULL);
	HANDLE_FDK_QMF_DOMAIN_GC gc = qd_ch->pGlobalConf;
	const int32_t          *real = qd_ch->hQmfSlotsReal[ts];
	const int32_t          *imag = qd_ch->hQmfSlotsImag[ts];
	const int32_t                ovSlots = gc->nQmfOvTimeSlots;
	const int32_t                exp_lb = SCALE2EXP((ts < ovSlots) ? qd_ch->scaling.ov_lb_scale : qd_ch->scaling.lb_scale);
	const int32_t                exp_hb = SCALE2EXP(qd_ch->scaling.hb_scale);
	const int32_t                lsb = qd_ch->fb.lsb;
	const int32_t                usb = qd_ch->fb.usb;
	int32_t                      b = start_band;
	int32_t                      lb_sf, hb_sf;

	int32_t target_exp = ALGORITHMIC_SCALING_IN_ANALYSIS_FILTERBANK + qd_ch->fb.filterScale;

	assert(ts < (gc->nQmfTimeSlots + gc->nQmfOvTimeSlots));
	assert(start_band >= 0);
	assert(stop_band <= gc->nQmfProcBands);

	if(qd_ch->fb.no_channels == 24) { target_exp -= 1; }

	/* Limit scaling factors to maximum negative value to avoid faulty behaviour
	   due to right-shifts. Corresponding asserts were observed during robustness
	   testing.
	 */
	lb_sf = fMax(exp_lb - target_exp - exp_out, -31);
	assert(lb_sf < 32);
	hb_sf = fMax(exp_hb - target_exp - exp_out, -31);
	assert(hb_sf < 32);

	if(pQmfOutImag == NULL) {
		for(; b < fMin(lsb, stop_band); b++) { pQmfOutReal[b] = scaleValueSaturate(real[b], lb_sf); }
		for(; b < fMin(usb, stop_band); b++) { pQmfOutReal[b] = scaleValueSaturate(real[b], hb_sf); }
		for(; b < stop_band; b++) { pQmfOutReal[b] = (int32_t)0; }
	}
	else {
		assert(imag != NULL);
		for(; b < fMin(lsb, stop_band); b++) {
			pQmfOutReal[b] = scaleValueSaturate(real[b], lb_sf);
			pQmfOutImag[b] = scaleValueSaturate(imag[b], lb_sf);
		}
		for(; b < fMin(usb, stop_band); b++) {
			pQmfOutReal[b] = scaleValueSaturate(real[b], hb_sf);
			pQmfOutImag[b] = scaleValueSaturate(imag[b], hb_sf);
		}
		for(; b < stop_band; b++) {
			pQmfOutReal[b] = (int32_t)0;
			pQmfOutImag[b] = (int32_t)0;
		}
	}
}

void FDK_QmfDomain_GetWorkBuffer(const HANDLE_FDK_QMF_DOMAIN_IN qd_ch, const int32_t ts, int32_t **ppQmfReal,
								 int32_t **ppQmfImag) {
	assert(qd_ch != NULL);
	assert(ppQmfReal != NULL);
	assert(ppQmfImag != NULL);
	const int32_t  bands = qd_ch->workBuf_nBands;
	int32_t **pWorkBuf = qd_ch->pWorkBuffer;
	uint16_t     workBufferOffset = qd_ch->workBufferOffset;
	uint16_t     workBufferSectSize = qd_ch->workBufferSectSize;

	assert(bands > 0);
	assert(ts < qd_ch->workBuf_nTimeSlots);

	*ppQmfReal =
		FDK_getWorkBuffer(pWorkBuf, workBufferOffset + (ts * CMPLX_MOD + 0) * bands, workBufferSectSize, bands);
	*ppQmfImag =
		FDK_getWorkBuffer(pWorkBuf, workBufferOffset + (ts * CMPLX_MOD + 1) * bands, workBufferSectSize, bands);
}

void FDK_QmfDomain_WorkBuffer2ProcChannel(const HANDLE_FDK_QMF_DOMAIN_IN qd_ch) {
	assert(qd_ch != NULL);
	HANDLE_FDK_QMF_DOMAIN_GC gc = qd_ch->pGlobalConf;
	int32_t               **pWorkBuf = qd_ch->pWorkBuffer;
	uint16_t                   workBufferOffset = qd_ch->workBufferOffset;
	uint16_t                   workBufferSectSize = qd_ch->workBufferSectSize;

	if(FDK_getWorkBuffer(pWorkBuf, workBufferOffset, workBufferSectSize, qd_ch->workBuf_nBands) ==
	   qd_ch->hQmfSlotsReal[gc->nQmfOvTimeSlots]) {
		/* work buffer is part of processing channel => nothing to do */
		return;
	}
	else {
		/* copy parked new QMF data to processing channel */
		const int32_t bands = qd_ch->workBuf_nBands;
		const int32_t slots = qd_ch->workBuf_nTimeSlots;
		int32_t       ts;
		for(ts = 0; ts < slots; ts++) {
			memcpy(qd_ch->hQmfSlotsReal[gc->nQmfOvTimeSlots + ts],
					  FDK_getWorkBuffer(pWorkBuf, workBufferOffset, workBufferSectSize, bands),
					  sizeof(int32_t) * bands);  // parkBuf_to_anaMatrix
			workBufferOffset += bands;
			memcpy(qd_ch->hQmfSlotsImag[gc->nQmfOvTimeSlots + ts],
					  FDK_getWorkBuffer(pWorkBuf, workBufferOffset, workBufferSectSize, bands),
					  sizeof(int32_t) * bands);
			workBufferOffset += bands;
		}
	}
}

void FDK_QmfDomain_QmfData2HBE(HANDLE_FDK_QMF_DOMAIN_IN qd_ch, int32_t **ppQmfReal, int32_t **ppQmfImag) {
	assert(qd_ch != NULL);
	assert(ppQmfReal != NULL);
	assert(ppQmfImag != NULL);
	HANDLE_FDK_QMF_DOMAIN_GC gc = qd_ch->pGlobalConf;
	int32_t               **pWorkBuf = qd_ch->pWorkBuffer;
	uint16_t                   workBufferOffset = qd_ch->workBufferOffset;
	uint16_t                   workBufferSectSize = qd_ch->workBufferSectSize;

	if(FDK_getWorkBuffer(pWorkBuf, workBufferOffset, workBufferSectSize,
						 qd_ch->workBuf_nBands) ==
	   qd_ch->hQmfSlotsReal[gc->nQmfOvTimeSlots]) {  // left channel (anaMatrix)
		int32_t       ts;
		const int32_t bands = gc->nBandsAnalysis;
		const int32_t slots = qd_ch->workBuf_nTimeSlots;
		assert(bands <= 64);
		for(ts = 0; ts < slots; ts++) {
			/* copy current data of processing channel */
			int32_t tmp[64];  // one slot
			/* real */
			memcpy(tmp, qd_ch->hQmfSlotsReal[gc->nQmfOvTimeSlots + ts],
					  sizeof(int32_t) * bands);  // anaMatrix_to_tmp
			memcpy(qd_ch->hQmfSlotsReal[gc->nQmfOvTimeSlots + ts], ppQmfReal[ts],
					  sizeof(int32_t) * bands);                      // HBE_to_anaMatrix
			memcpy(ppQmfReal[ts], tmp, sizeof(int32_t) * bands);  // tmp_to_HBE
			/* imag */
			memcpy(tmp, qd_ch->hQmfSlotsImag[gc->nQmfOvTimeSlots + ts], sizeof(int32_t) * bands);
			memcpy(qd_ch->hQmfSlotsImag[gc->nQmfOvTimeSlots + ts], ppQmfImag[ts], sizeof(int32_t) * bands);
			memcpy(ppQmfImag[ts], tmp, sizeof(int32_t) * bands);
		}
	}
	else {  // right channel (parkBuf)
		const int32_t bands = qd_ch->workBuf_nBands;
		const int32_t slots = qd_ch->workBuf_nTimeSlots;
		int32_t       ts;
		assert(qd_ch->workBuf_nBands == gc->nBandsAnalysis);
		for(ts = 0; ts < slots; ts++) {
			/* copy HBE QMF data buffer to processing channel */
			memcpy(qd_ch->hQmfSlotsReal[gc->nQmfOvTimeSlots + ts], ppQmfReal[ts],
					  sizeof(int32_t) * bands);  // HBE_to_anaMatrix
			memcpy(qd_ch->hQmfSlotsImag[gc->nQmfOvTimeSlots + ts], ppQmfImag[ts], sizeof(int32_t) * bands);
			/* copy parked new QMF data to HBE QMF data buffer */
			memcpy(ppQmfReal[ts], FDK_getWorkBuffer(pWorkBuf, workBufferOffset, workBufferSectSize, bands),
					  sizeof(int32_t) * bands);  // parkBuf_to_HBE
			workBufferOffset += bands;
			memcpy(ppQmfImag[ts], FDK_getWorkBuffer(pWorkBuf, workBufferOffset, workBufferSectSize, bands),
					  sizeof(int32_t) * bands);
			workBufferOffset += bands;
		}
	}
}

void FDK_QmfDomain_ClearRequested(HANDLE_FDK_QMF_DOMAIN_GC hgc) {
	hgc->qmfDomainExplicitConfig = 0;
	hgc->flags_requested = 0;
	hgc->nInputChannels_requested = 0;
	hgc->nOutputChannels_requested = 0;
	hgc->parkChannel_requested = 0;
	hgc->nBandsAnalysis_requested = 0;
	hgc->nBandsSynthesis_requested = 0;
	hgc->nQmfTimeSlots_requested = 0;
	hgc->nQmfOvTimeSlots_requested = 0;
	hgc->nQmfProcBands_requested = 0;
	hgc->nQmfProcChannels_requested = 0;
}

static void FDK_QmfDomain_ClearConfigured(HANDLE_FDK_QMF_DOMAIN_GC hgc) {
	hgc->flags = 0;
	hgc->nInputChannels = 0;
	hgc->nOutputChannels = 0;
	hgc->parkChannel = 0;
	hgc->nBandsAnalysis = 0;
	hgc->nBandsSynthesis = 0;
	hgc->nQmfTimeSlots = 0;
	hgc->nQmfOvTimeSlots = 0;
	hgc->nQmfProcBands = 0;
	hgc->nQmfProcChannels = 0;
}

static void FDK_QmfDomain_ClearFilterBank(HANDLE_FDK_QMF_DOMAIN hqd) {
	int32_t ch;

	for(ch = 0; ch < ((8) + (1)); ch++) { memset(&hqd->QmfDomainIn[ch].fb, 0, sizeof(hqd->QmfDomainIn[ch].fb)); }

	for(ch = 0; ch < ((8) + (1)); ch++) { memset(&hqd->QmfDomainOut[ch].fb, 0, sizeof(hqd->QmfDomainIn[ch].fb)); }
}

QMF_DOMAIN_ERROR FDK_QmfDomain_Configure(HANDLE_FDK_QMF_DOMAIN hqd) {
	assert(hqd != NULL);
	QMF_DOMAIN_ERROR err = QMF_DOMAIN_OK;
	int32_t              i, size_main, size, size_temp = 0;

	HANDLE_FDK_QMF_DOMAIN_GC hgc = &hqd->globalConf;
	int32_t               **pWorkBuffer = hgc->pWorkBuffer;

	int32_t hasChanged = 0;

	if((hgc->nQmfProcChannels_requested > 0) && (hgc->nQmfProcBands_requested != 64)) { return QMF_DOMAIN_INIT_ERROR; }
	if(hgc->nBandsAnalysis_requested > hgc->nQmfProcBands_requested) {
		/* In general the output of the qmf analysis is written to QMF memory slots
		   which size is defined by nQmfProcBands. nBandsSynthesis may be larger
		   than nQmfProcBands. This is e.g. the case if the QMF based resampler is
		   used.
		*/
		return QMF_DOMAIN_INIT_ERROR;
	}

	/* 1. adjust change of processing channels by comparison of current and
	 * requested parameters */
	if((hgc->nQmfProcChannels != hgc->nQmfProcChannels_requested) ||
	   (hgc->nQmfProcBands != hgc->nQmfProcBands_requested) || (hgc->nQmfTimeSlots != hgc->nQmfTimeSlots_requested)) {
		for(i = 0; i < hgc->nQmfProcChannels_requested; i++) {
			hqd->QmfDomainIn[i].workBuf_nBands = hgc->nQmfProcBands_requested;
			hgc->nQmfProcBands = hgc->nQmfProcBands_requested;

			hqd->QmfDomainIn[i].workBuf_nTimeSlots = hgc->nQmfTimeSlots_requested;
		}

		hgc->nQmfProcChannels = hgc->nQmfProcChannels_requested; /* keep highest value encountered so
																	far as allocated */

		hasChanged = 1;
	}

	/* 2. reallocate persistent memory if necessary (analysis state-buffers,
	 * timeslot-pointer-array, overlap-buffers, synthesis state-buffers) */
	if((hgc->nInputChannels != hgc->nInputChannels_requested) ||
	   (hgc->nBandsAnalysis != hgc->nBandsAnalysis_requested) || (hgc->nQmfTimeSlots != hgc->nQmfTimeSlots_requested) ||
	   (hgc->nQmfOvTimeSlots != hgc->nQmfOvTimeSlots_requested) ||
	   (hgc->nOutputChannels != hgc->nOutputChannels_requested) ||
	   (hgc->nBandsSynthesis != hgc->nBandsSynthesis_requested) || (hgc->parkChannel != hgc->parkChannel_requested)) {
		hgc->nInputChannels = hgc->nInputChannels_requested;
		hgc->nBandsAnalysis = hgc->nBandsAnalysis_requested;
		hgc->nQmfTimeSlots = hgc->nQmfTimeSlots_requested;
		hgc->nQmfOvTimeSlots = hgc->nQmfOvTimeSlots_requested;
		hgc->nOutputChannels = hgc->nOutputChannels_requested;
		hgc->nBandsSynthesis = hgc->nBandsSynthesis_requested;
		hgc->parkChannel = hgc->parkChannel_requested;

		if(FDK_QmfDomain_AllocatePersistentMemory(hqd)) {
			err = QMF_DOMAIN_OUT_OF_MEMORY;
			goto bail;
		}

		/* 3. set request-flag for downsampled SBR */
		if((hgc->nBandsAnalysis == 32) && (hgc->nBandsSynthesis == 32) &&
		   !(hgc->flags & (QMF_FLAG_CLDFB | QMF_FLAG_MPSLDFB))) {
			hgc->flags_requested |= QMF_FLAG_DOWNSAMPLED;
		}

		hasChanged = 1;
	}

	/* 4. initialize tables and buffer for QMF-resampler */

	/* 5. set requested flags */
	if(hgc->flags != hgc->flags_requested) {
		if((hgc->flags_requested & QMF_FLAG_MPSLDFB) && (hgc->flags_requested & QMF_FLAG_CLDFB)) {
			hgc->flags_requested &= ~QMF_FLAG_CLDFB;
		}
		hgc->flags = hgc->flags_requested;
		hasChanged = 1;
	}

	if(hasChanged) {
		/* 6. recalculate and check size of required workbuffer-space */

		if(hgc->parkChannel && (hqd->globalConf.nQmfProcChannels == 1)) {
			/* configure temp QMF buffer for parking right channel MPS212 output,
			 * (USAC stereoConfigIndex 3 only) */
			hqd->QmfDomainIn[1].workBuf_nBands = hqd->globalConf.nBandsAnalysis;
			hqd->QmfDomainIn[1].workBuf_nTimeSlots = hqd->globalConf.nQmfTimeSlots;
			size_temp = hqd->QmfDomainIn[1].workBuf_nBands * hqd->QmfDomainIn[1].workBuf_nTimeSlots * CMPLX_MOD;
		}

		size_main = hqd->QmfDomainIn[0].workBuf_nBands * hqd->QmfDomainIn[0].workBuf_nTimeSlots * CMPLX_MOD;

		size = size_main * hgc->nQmfProcChannels + size_temp;

		if(size > (QMF_MAX_WB_SECTIONS * QMF_WB_SECTION_SIZE)) {
			err = QMF_DOMAIN_OUT_OF_MEMORY;
			goto bail;
		}

		/* 7. allocate additional workbuffer if necessary */
		if((size > 0 /* *QMF_WB_SECTION_SIZE */) && (pWorkBuffer[0] == NULL)) {
			/* get work buffer of size QMF_WB_SECTION_SIZE */
			pWorkBuffer[0] = GetQmfWorkBufferCore6();
		}

		if((size > 1 * QMF_WB_SECTION_SIZE) && (pWorkBuffer[1] == NULL)) {
			/* get work buffer of size QMF_WB_SECTION_SIZE */
			pWorkBuffer[1] = GetQmfWorkBufferCore1();
		}

		if((size > 2 * QMF_WB_SECTION_SIZE) && (pWorkBuffer[2] == NULL)) {
			/* get work buffer of size QMF_WB_SECTION_SIZE */
			pWorkBuffer[2] = GetQmfWorkBufferCore3();
		}

		if((size > 3 * QMF_WB_SECTION_SIZE) && (pWorkBuffer[3] == NULL)) {
			/* get work buffer of size QMF_WB_SECTION_SIZE */
			pWorkBuffer[3] = GetQmfWorkBufferCore4();
		}

		if((size > 4 * QMF_WB_SECTION_SIZE) && (pWorkBuffer[4] == NULL)) {
			/* get work buffer of size QMF_WB_SECTION_SIZE */
			pWorkBuffer[4] = GetQmfWorkBufferCore7();
		}

		/* 8. distribute workbuffer over processing channels */
		for(i = 0; i < hgc->nQmfProcChannels; i++) {
			FDK_QmfDomain_FeedWorkBuffer(hqd, i, pWorkBuffer, size_main * i, QMF_WB_SECTION_SIZE, size_main);
		}
		if(hgc->parkChannel) {
			for(; i < hgc->nInputChannels; i++) {
				FDK_QmfDomain_FeedWorkBuffer(hqd, 1, pWorkBuffer, size_main * hgc->nQmfProcChannels,
											 QMF_WB_SECTION_SIZE, size_temp);
			}
		}

		/* 9. (re-)init filterbank */
		for(i = 0; i < hgc->nOutputChannels; i++) {
			if((hqd->QmfDomainOut[i].fb.lsb == 0) && (hqd->QmfDomainOut[i].fb.usb == 0)) {
				/* Although lsb and usb are set in the SBR module, they are initialized
				 * at this point due to the case of using MPS without SBR. */
				hqd->QmfDomainOut[i].fb.lsb = hgc->nBandsAnalysis_requested;
				hqd->QmfDomainOut[i].fb.usb = fMin((int32_t)hgc->nBandsSynthesis_requested, 64);
			}
		}
		if(FDK_QmfDomain_InitFilterBank(hqd, 0)) { err = QMF_DOMAIN_INIT_ERROR; }
	}

bail:
	if(err) { FDK_QmfDomain_FreeMem(hqd); }
	return err;
}

static void FDK_QmfDomain_FreeWorkBuffer(HANDLE_FDK_QMF_DOMAIN hqd) {
	int32_t **pWorkBuffer = hqd->globalConf.pWorkBuffer;

	if(pWorkBuffer[0]) FreeQmfWorkBufferCore6(&pWorkBuffer[0]);
	if(pWorkBuffer[1]) FreeQmfWorkBufferCore1(&pWorkBuffer[1]);
	if(pWorkBuffer[2]) FreeQmfWorkBufferCore3(&pWorkBuffer[2]);
	if(pWorkBuffer[3]) FreeQmfWorkBufferCore4(&pWorkBuffer[3]);
	if(pWorkBuffer[4]) FreeQmfWorkBufferCore7(&pWorkBuffer[4]);
}

void FDK_QmfDomain_FreeMem(HANDLE_FDK_QMF_DOMAIN hqd) {
	FDK_QmfDomain_FreeWorkBuffer(hqd);

	FDK_QmfDomain_FreePersistentMemory(hqd);

	FDK_QmfDomain_ClearFilterBank(hqd);

	FDK_QmfDomain_ClearConfigured(&hqd->globalConf);

	FDK_QmfDomain_ClearRequested(&hqd->globalConf);
}

void FDK_QmfDomain_Close(HANDLE_FDK_QMF_DOMAIN hqd) {
	FDK_QmfDomain_FreeWorkBuffer(hqd);

	FDK_QmfDomain_FreePersistentMemory(hqd);
}
