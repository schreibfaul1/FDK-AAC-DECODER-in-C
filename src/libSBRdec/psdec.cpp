

/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  parametric stereo decoder
*/

#include <memory.h>
#include "../libAACdec/newAACDecoder.h"
#include "psdec.h"
#include "../libFDK/FDK_tools_rom.h"
#include "../libFDK/FDK_trigFcts.h"

#include "sbr_ram.h"
#include "sbr_rom.h"

/********************************************************************/
/*                       MLQUAL DEFINES                             */
/********************************************************************/

#define FRACT_ZERO FRACT_BITS - 1
/********************************************************************/

SBR_ERROR ResetPsDec(HANDLE_PS_DEC h_ps_d);

/***** HELPERS *****/

/***************************************************************************/
/*!
  \brief  Creates one instance of the PS_DEC struct

  \return Error info

****************************************************************************/
int32_t CreatePsDec(HANDLE_PS_DEC *h_PS_DEC, /*!< pointer to the module state */
					int32_t        aacSamplesPerFrame) {
	SBR_ERROR     errorInfo = SBRDEC_OK;
	HANDLE_PS_DEC h_ps_d;
	int32_t       i;

	if(*h_PS_DEC == NULL) {
		/* Get ps dec ram */
		h_ps_d = (struct PS_DEC *)FDKcalloc(1, sizeof(struct PS_DEC));
		if(h_ps_d == NULL) { goto bail; }
	}
	else {
		/* Reset an open instance */
		h_ps_d = *h_PS_DEC;
	}

	/*
	 * Create Analysis Hybrid filterbank.
	 */
	FDKhybridAnalysisOpen(&h_ps_d->specificTo.mpeg.hybridAnalysis, h_ps_d->specificTo.mpeg.pHybridAnaStatesLFdmx,
						  sizeof(h_ps_d->specificTo.mpeg.pHybridAnaStatesLFdmx), NULL, 0);

	/* initialisation */
	switch(aacSamplesPerFrame) {
		case 960:
			h_ps_d->noSubSamples = 30; /* col */
			break;
		case 1024:
			h_ps_d->noSubSamples = 32; /* col */
			break;
		default:
			h_ps_d->noSubSamples = -1;
			break;
	}

	if(h_ps_d->noSubSamples > MAX_NUM_COL || h_ps_d->noSubSamples <= 0) { goto bail; }
	h_ps_d->noChannels = NO_QMF_CHANNELS; /* row */

	h_ps_d->psDecodedPrv = 0;
	h_ps_d->procFrameBased = -1;
	for(i = 0; i < (1) + 1; i++) { h_ps_d->bPsDataAvail[i] = ppt_none; }
	{
		int32_t error;
		error = FDKdecorrelateOpen(&(h_ps_d->specificTo.mpeg.apDecor), h_ps_d->specificTo.mpeg.decorrBufferCplx,
								   (2 * ((825) + (373))));
		if(error) goto bail;
	}

	for(i = 0; i < (1) + 1; i++) { memset(&h_ps_d->bsData[i].mpeg, 0, sizeof(MPEG_PS_BS_DATA)); }

	errorInfo = ResetPsDec(h_ps_d);

	if(errorInfo != SBRDEC_OK) goto bail;

	*h_PS_DEC = h_ps_d;

	return 0;

bail:
	if(h_ps_d != NULL) { DeletePsDec(&h_ps_d); }

	return -1;
} /*END CreatePsDec */

/***************************************************************************/
/*!
  \brief  Delete one instance of the PS_DEC struct

  \return Error info

****************************************************************************/
int32_t DeletePsDec(HANDLE_PS_DEC *h_PS_DEC) /*!< pointer to the module state */
{
	if(*h_PS_DEC == NULL) { return -1; }

	{
		HANDLE_PS_DEC h_ps_d = *h_PS_DEC;
		FDKdecorrelateClose(&(h_ps_d->specificTo.mpeg.apDecor));
	}

	free(h_PS_DEC);
	h_PS_DEC = NULL;

	return 0;
} /*END DeletePsDec */

/***************************************************************************/
/*!
  \brief resets some values of the PS handle to default states

  \return

****************************************************************************/
SBR_ERROR ResetPsDec(HANDLE_PS_DEC h_ps_d) /*!< pointer to the module state */
{
	SBR_ERROR errorInfo = SBRDEC_OK;
	int32_t   i;

	/* explicitly init state variables to safe values (until first ps header
	 * arrives) */

	h_ps_d->specificTo.mpeg.lastUsb = 0;

	/*
	 * Initialize Analysis Hybrid filterbank.
	 */
	FDKhybridAnalysisInit(&h_ps_d->specificTo.mpeg.hybridAnalysis, THREE_TO_TEN, NO_QMF_BANDS_HYBRID20,
						  NO_QMF_BANDS_HYBRID20, 1);

	/*
	 * Initialize Synthesis Hybrid filterbank.
	 */
	for(i = 0; i < 2; i++) {
		FDKhybridSynthesisInit(&h_ps_d->specificTo.mpeg.hybridSynthesis[i], THREE_TO_TEN, NO_QMF_CHANNELS,
							   NO_QMF_CHANNELS);
	}
	{
		int32_t error;
		error = FDKdecorrelateInit(&h_ps_d->specificTo.mpeg.apDecor, 71, DECORR_PS, DUCKER_AUTOMATIC, 0, 0, 0, 0,
								   1, /* isLegacyPS */
								   1);
		if(error) return SBRDEC_NOT_INITIALIZED;
	}

	for(i = 0; i < NO_IID_GROUPS; i++) {
		h_ps_d->specificTo.mpeg.h11rPrev[i] = FL2FXCONST_DBL(0.5f);
		h_ps_d->specificTo.mpeg.h12rPrev[i] = FL2FXCONST_DBL(0.5f);
	}

	memset(h_ps_d->specificTo.mpeg.h21rPrev, 0, sizeof(h_ps_d->specificTo.mpeg.h21rPrev));
	memset(h_ps_d->specificTo.mpeg.h22rPrev, 0, sizeof(h_ps_d->specificTo.mpeg.h22rPrev));

	return errorInfo;
}

/***************************************************************************/
/*!
  \brief  Feed delaylines when parametric stereo is switched on.
  \return
****************************************************************************/
void PreparePsProcessing(HANDLE_PS_DEC h_ps_d, const int32_t *const *const rIntBufferLeft,
						 const int32_t *const *const iIntBufferLeft, const int32_t scaleFactorLowBand) {
	if(h_ps_d->procFrameBased == 1) /* If we have switched from frame to slot based processing  */
	{                               /* fill hybrid delay buffer.                                */
		int32_t i, j;

		for(i = 0; i < HYBRID_FILTER_DELAY; i++) {
			int32_t qmfInputData[2][NO_QMF_BANDS_HYBRID20];
			int32_t hybridOutputData[2][NO_SUB_QMF_CHANNELS];

			for(j = 0; j < NO_QMF_BANDS_HYBRID20; j++) {
				qmfInputData[0][j] = scaleValue(rIntBufferLeft[i][j], scaleFactorLowBand);
				qmfInputData[1][j] = scaleValue(iIntBufferLeft[i][j], scaleFactorLowBand);
			}

			FDKhybridAnalysisApply(&h_ps_d->specificTo.mpeg.hybridAnalysis, qmfInputData[0], qmfInputData[1],
								   hybridOutputData[0], hybridOutputData[1]);
		}
		h_ps_d->procFrameBased = 0; /* switch to slot based processing. */

	} /* procFrameBased==1 */
}

void initSlotBasedRotation(HANDLE_PS_DEC h_ps_d, /*!< pointer to the module state */
						   int32_t env, int32_t usb) {
	int32_t group = 0;
	int32_t bin = 0;
	int32_t noIidSteps;

	int16_t invL;
	int32_t  ScaleL, ScaleR;
	int32_t  Alpha, Beta;
	int32_t  h11r, h12r, h21r, h22r;

	const int32_t *PScaleFactors;

	if(h_ps_d->bsData[h_ps_d->processSlot].mpeg.bFineIidQ) {
		PScaleFactors = ScaleFactorsFine; /* values are shiftet right by one */
		noIidSteps = NO_IID_STEPS_FINE;
	}
	else {
		PScaleFactors = ScaleFactors; /* values are shiftet right by one */
		noIidSteps = NO_IID_STEPS;
	}

	/* dequantize and decode */
	for(group = 0; group < NO_IID_GROUPS; group++) {
		bin = bins2groupMap20[group];

		/*!
		<h3> type 'A' rotation </h3>
		mixing procedure R_a, used in baseline version<br>

		 Scale-factor vectors c1 and c2 are precalculated in initPsTables () and
		stored in scaleFactors[] and scaleFactorsFine[] = pScaleFactors []. From the
		linearized IID parameters (intensity differences), two scale factors are
		 calculated. They are used to obtain the coefficients h11... h22.
		*/

		/* ScaleR and ScaleL are scaled by 1 shift right */

		ScaleR = PScaleFactors[noIidSteps + h_ps_d->specificTo.mpeg.pCoef->aaIidIndexMapped[env][bin]];
		ScaleL = PScaleFactors[noIidSteps - h_ps_d->specificTo.mpeg.pCoef->aaIidIndexMapped[env][bin]];

		Beta = fMult(fMult(Alphas[h_ps_d->specificTo.mpeg.pCoef->aaIccIndexMapped[env][bin]], (ScaleR - ScaleL)),
					 FIXP_SQRT05);
		Alpha = Alphas[h_ps_d->specificTo.mpeg.pCoef->aaIccIndexMapped[env][bin]] >> 1;

		/* Alpha and Beta are now both scaled by 2 shifts right */

		/* calculate the coefficients h11... h22 from scale-factors and ICC
		 * parameters */

		/* h values are scaled by 1 shift right */
		{
			int32_t trigData[4];

			inline_fixp_cos_sin(Beta + Alpha, Beta - Alpha, 2, trigData);
			h11r = fMult(ScaleL, trigData[0]);
			h12r = fMult(ScaleR, trigData[2]);
			h21r = fMult(ScaleL, trigData[1]);
			h22r = fMult(ScaleR, trigData[3]);
		}
		/*****************************************************************************************/
		/* Interpolation of the matrices H11... H22: */
		/*                                                                                       */
		/* H11(k,n) = H11(k,n[e]) + (n-n[e]) * (H11(k,n[e+1] - H11(k,n[e])) /
		 * (n[e+1] - n[e])    */
		/* ... */
		/*****************************************************************************************/

		/* invL = 1/(length of envelope) */
		invL = FX_DBL2FX_SGL(GetInvInt(h_ps_d->bsData[h_ps_d->processSlot].mpeg.aEnvStartStop[env + 1] -
									   h_ps_d->bsData[h_ps_d->processSlot].mpeg.aEnvStartStop[env]));

		h_ps_d->specificTo.mpeg.pCoef->H11r[group] = h_ps_d->specificTo.mpeg.h11rPrev[group];
		h_ps_d->specificTo.mpeg.pCoef->H12r[group] = h_ps_d->specificTo.mpeg.h12rPrev[group];
		h_ps_d->specificTo.mpeg.pCoef->H21r[group] = h_ps_d->specificTo.mpeg.h21rPrev[group];
		h_ps_d->specificTo.mpeg.pCoef->H22r[group] = h_ps_d->specificTo.mpeg.h22rPrev[group];

		h_ps_d->specificTo.mpeg.pCoef->DeltaH11r[group] =
			fMult(h11r - h_ps_d->specificTo.mpeg.pCoef->H11r[group], invL);
		h_ps_d->specificTo.mpeg.pCoef->DeltaH12r[group] =
			fMult(h12r - h_ps_d->specificTo.mpeg.pCoef->H12r[group], invL);
		h_ps_d->specificTo.mpeg.pCoef->DeltaH21r[group] =
			fMult(h21r - h_ps_d->specificTo.mpeg.pCoef->H21r[group], invL);
		h_ps_d->specificTo.mpeg.pCoef->DeltaH22r[group] =
			fMult(h22r - h_ps_d->specificTo.mpeg.pCoef->H22r[group], invL);

		/* update prev coefficients for interpolation in next envelope */

		h_ps_d->specificTo.mpeg.h11rPrev[group] = h11r;
		h_ps_d->specificTo.mpeg.h12rPrev[group] = h12r;
		h_ps_d->specificTo.mpeg.h21rPrev[group] = h21r;
		h_ps_d->specificTo.mpeg.h22rPrev[group] = h22r;

	} /* group loop */
}

static const uint8_t groupTable[NO_IID_GROUPS + 1] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
													  12, 13, 14, 15, 16, 18, 21, 25, 30, 42, 71};

static void applySlotBasedRotation(HANDLE_PS_DEC h_ps_d, /*!< pointer to the module state */

								   int32_t *mHybridRealLeft, /*!< hybrid values real left  */
								   int32_t *mHybridImagLeft, /*!< hybrid values imag left  */

								   int32_t *mHybridRealRight, /*!< hybrid values real right  */
								   int32_t *mHybridImagRight  /*!< hybrid values imag right  */
) {
	int32_t group;
	int32_t subband;

	/**********************************************************************************************/
	/*!
	<h2> Mapping </h2>

	The number of stereo bands that is actually used depends on the number of
	availble parameters for IID and ICC: <pre> nr. of IID para.| nr. of ICC para.
	| nr. of Stereo bands
	 ----------------|------------------|-------------------
	   10,20         |     10,20        |        20
	   10,20         |     34           |        34
	   34            |     10,20        |        34
	   34            |     34           |        34
	</pre>
	In the case the number of parameters for IIS and ICC differs from the number
	of stereo bands, a mapping from the lower number to the higher number of
	parameters is applied. Index mapping of IID and ICC parameters is already done
	in psbitdec.cpp. Further mapping is not needed here in baseline version.
	**********************************************************************************************/

	/************************************************************************************************/
	/*!
	<h2> Mixing </h2>

	To generate the QMF subband signals for the subband samples n = n[e]+1 ,,,
	n_[e+1] the parameters at position n[e] and n[e+1] are required as well as the
	subband domain signals s_k(n) and d_k(n) for n = n[e]+1... n_[e+1]. n[e]
	represents the start position for envelope e. The border positions n[e] are
	handled in DecodePS().

	The stereo sub subband signals are constructed as:
	<pre>
	l_k(n) = H11(k,n) s_k(n) + H21(k,n) d_k(n)
	r_k(n) = H21(k,n) s_k(n) + H22(k,n) d_k(n)
	</pre>
	In order to obtain the matrices H11(k,n)... H22 (k,n), the vectors h11(b)...
	h22(b) need to be calculated first (b: parameter index). Depending on ICC mode
	either mixing procedure R_a or R_b is used for that. For both procedures, the
	parameters for parameter position n[e+1] is used.
	************************************************************************************************/

	/************************************************************************************************/
	/*!
	<h2>Phase parameters </h2>
	With disabled phase parameters (which is the case in baseline version), the
	H-matrices are just calculated by:

	<pre>
	H11(k,n[e+1] = h11(b(k))
	(...)
	b(k): parameter index according to mapping table
	</pre>

	<h2>Processing of the samples in the sub subbands </h2>
	this loop includes the interpolation of the coefficients Hxx
	************************************************************************************************/

	/******************************************************/
	/* construct stereo sub subband signals according to: */
	/*                                                    */
	/* l_k(n) = H11(k,n) s_k(n) + H21(k,n) d_k(n)         */
	/* r_k(n) = H12(k,n) s_k(n) + H22(k,n) d_k(n)         */
	/******************************************************/
	PS_DEC_COEFFICIENTS *pCoef = h_ps_d->specificTo.mpeg.pCoef;

	for(group = 0; group < NO_IID_GROUPS; group++) {
		pCoef->H11r[group] += pCoef->DeltaH11r[group];
		pCoef->H12r[group] += pCoef->DeltaH12r[group];
		pCoef->H21r[group] += pCoef->DeltaH21r[group];
		pCoef->H22r[group] += pCoef->DeltaH22r[group];

		const int32_t start = groupTable[group];
		const int32_t stop = groupTable[group + 1];
		for(subband = start; subband < stop; subband++) {
			int32_t tmpLeft = fMultAdd(fMultDiv2(pCoef->H11r[group], mHybridRealLeft[subband]), pCoef->H21r[group],
									   mHybridRealRight[subband]);
			int32_t tmpRight = fMultAdd(fMultDiv2(pCoef->H12r[group], mHybridRealLeft[subband]), pCoef->H22r[group],
										mHybridRealRight[subband]);
			mHybridRealLeft[subband] = tmpLeft;
			mHybridRealRight[subband] = tmpRight;

			tmpLeft = fMultAdd(fMultDiv2(pCoef->H11r[group], mHybridImagLeft[subband]), pCoef->H21r[group],
							   mHybridImagRight[subband]);
			tmpRight = fMultAdd(fMultDiv2(pCoef->H12r[group], mHybridImagLeft[subband]), pCoef->H22r[group],
								mHybridImagRight[subband]);
			mHybridImagLeft[subband] = tmpLeft;
			mHybridImagRight[subband] = tmpRight;
		} /* subband */
	}
}

/***************************************************************************/
/*!
  \brief  Applies IID, ICC, IPD and OPD parameters to the current frame.

  \return none

****************************************************************************/
void ApplyPsSlot(HANDLE_PS_DEC h_ps_d,          /*!< handle PS_DEC*/
				 int32_t     **rIntBufferLeft,  /*!< real bands left qmf channel (38x64)  */
				 int32_t     **iIntBufferLeft,  /*!< imag bands left qmf channel (38x64)  */
				 int32_t      *rIntBufferRight, /*!< real bands right qmf channel (38x64) */
				 int32_t      *iIntBufferRight, /*!< imag bands right qmf channel (38x64) */
				 const int32_t scaleFactorLowBand_no_ov, const int32_t scaleFactorLowBand,
				 const int32_t scaleFactorHighBand, const int32_t lsb, const int32_t usb) {
/*!
The 64-band QMF representation of the monaural signal generated by the SBR tool
is used as input of the PS tool. After the PS processing, the outputs of the
left and right hybrid synthesis filterbanks are used to generate the stereo
output signal.

<pre>

		   -------------            ----------            -------------
		  | Hybrid      | M_n[k,m] |          | L_n[k,m] | Hybrid      | l[n]
 m[n] --->| analysis    |--------->|          |--------->| synthesis   |----->
		   -------------           | Stereo   |           -------------
				 |                 | recon-   |
				 |                 | stuction |
				\|/                |          |
		   -------------           |          |
		  | De-         | D_n[k,m] |          |
		  | correlation |--------->|          |
		   -------------           |          |           -------------
								   |          | R_n[k,m] | Hybrid      | r[n]
								   |          |--------->| synthesis   |----->
 IID, ICC ------------------------>|          |          | filter bank |
(IPD, OPD)                          ----------            -------------

m[n]:      QMF represantation of the mono input
M_n[k,m]:  (sub-)sub-band domain signals of the mono input
D_n[k,m]:  decorrelated (sub-)sub-band domain signals
L_n[k,m]:  (sub-)sub-band domain signals of the left output
R_n[k,m]:  (sub-)sub-band domain signals of the right output
l[n],r[n]: left/right output signals

</pre>
*/
#define NO_HYBRID_DATA_BANDS (71)

	int32_t  i;
	int32_t  qmfInputData[2][NO_QMF_BANDS_HYBRID20];
	int32_t *hybridData[2][2];
	int32_t pHybridData[4 * NO_HYBRID_DATA_BANDS];
	hybridData[0][0] = pHybridData + 0 * NO_HYBRID_DATA_BANDS; /* left real hybrid data */
	hybridData[0][1] = pHybridData + 1 * NO_HYBRID_DATA_BANDS; /* left imag hybrid data */
	hybridData[1][0] = pHybridData + 2 * NO_HYBRID_DATA_BANDS; /* right real hybrid data */
	hybridData[1][1] = pHybridData + 3 * NO_HYBRID_DATA_BANDS; /* right imag hybrid data */

	/*!
	Hybrid analysis filterbank:
	The lower 3 (5) of the 64 QMF subbands are further split to provide better
	frequency resolution. for PS processing. For the 10 and 20 stereo bands
	configuration, the QMF band H_0(w) is split up into 8 (sub-) sub-bands and the
	QMF bands H_1(w) and H_2(w) are spit into 2 (sub-) 4th. (See figures 8.20
	and 8.22 of ISO/IEC 14496-3:2001/FDAM 2:2004(E) )
	*/

	/*
	 * Hybrid analysis.
	 */

	/* Get qmf input data and apply descaling */
	for(i = 0; i < NO_QMF_BANDS_HYBRID20; i++) {
		qmfInputData[0][i] = scaleValue(rIntBufferLeft[HYBRID_FILTER_DELAY][i], scaleFactorLowBand_no_ov);
		qmfInputData[1][i] = scaleValue(iIntBufferLeft[HYBRID_FILTER_DELAY][i], scaleFactorLowBand_no_ov);
	}

	/* LF - part */
	FDKhybridAnalysisApply(&h_ps_d->specificTo.mpeg.hybridAnalysis, qmfInputData[0], qmfInputData[1], hybridData[0][0],
						   hybridData[0][1]);

	/* HF - part */
	/* bands up to lsb */
	scaleValues(&hybridData[0][0][NO_SUB_QMF_CHANNELS - 2], &rIntBufferLeft[0][NO_QMF_BANDS_HYBRID20],
				lsb - NO_QMF_BANDS_HYBRID20, scaleFactorLowBand);
	scaleValues(&hybridData[0][1][NO_SUB_QMF_CHANNELS - 2], &iIntBufferLeft[0][NO_QMF_BANDS_HYBRID20],
				lsb - NO_QMF_BANDS_HYBRID20, scaleFactorLowBand);

	/* bands from lsb to usb */
	scaleValues(&hybridData[0][0][lsb + (NO_SUB_QMF_CHANNELS - 2 - NO_QMF_BANDS_HYBRID20)], &rIntBufferLeft[0][lsb],
				usb - lsb, scaleFactorHighBand);
	scaleValues(&hybridData[0][1][lsb + (NO_SUB_QMF_CHANNELS - 2 - NO_QMF_BANDS_HYBRID20)], &iIntBufferLeft[0][lsb],
				usb - lsb, scaleFactorHighBand);

	/* bands from usb to NO_SUB_QMF_CHANNELS which should be zero for non-overlap
	   slots but can be non-zero for overlap slots */
	memcpy(&hybridData[0][0][usb + (NO_SUB_QMF_CHANNELS - 2 - NO_QMF_BANDS_HYBRID20)], &rIntBufferLeft[0][usb],
			  sizeof(int32_t) * (NO_QMF_CHANNELS - usb));
	memcpy(&hybridData[0][1][usb + (NO_SUB_QMF_CHANNELS - 2 - NO_QMF_BANDS_HYBRID20)], &iIntBufferLeft[0][usb],
			  sizeof(int32_t) * (NO_QMF_CHANNELS - usb));

	/*!
	Decorrelation:
	By means of all-pass filtering and delaying, the (sub-)sub-band samples s_k(n)
	are converted into de-correlated (sub-)sub-band samples d_k(n).
	- k: frequency in hybrid spectrum
	- n: time index
	*/

	FDKdecorrelateApply(&h_ps_d->specificTo.mpeg.apDecor, &hybridData[0][0][0], /* left real hybrid data */
						&hybridData[0][1][0],                                   /* left imag hybrid data */
						&hybridData[1][0][0],                                   /* right real hybrid data */
						&hybridData[1][1][0],                                   /* right imag hybrid data */
						0                                                       /* startHybBand */
	);

	/*!
	Stereo Processing:
	The sets of (sub-)sub-band samples s_k(n) and d_k(n) are processed according
	to the stereo cues which are defined per stereo band.
	*/

	applySlotBasedRotation(h_ps_d, &hybridData[0][0][0], /* left real hybrid data */
						   &hybridData[0][1][0],         /* left imag hybrid data */
						   &hybridData[1][0][0],         /* right real hybrid data */
						   &hybridData[1][1][0]          /* right imag hybrid data */
	);

	/*!
	Hybrid synthesis filterbank:
	The stereo processed hybrid subband signals l_k(n) and r_k(n) are fed into the
	hybrid synthesis filterbanks which are identical to the 64 complex synthesis
	filterbank of the SBR tool. The input to the filterbank are slots of 64 QMF
	samples. For each slot the filterbank outputs one block of 64 samples of one
	reconstructed stereo channel. The hybrid synthesis filterbank is computed
	seperatly for the left and right channel.
	*/

	/*
	 * Hybrid synthesis.
	 */
	for(i = 0; i < 2; i++) {
		FDKhybridSynthesisApply(&h_ps_d->specificTo.mpeg.hybridSynthesis[i], hybridData[i][0], /* real hybrid data */
								hybridData[i][1],                                              /* imag hybrid data */
								(i == 0) ? rIntBufferLeft[0] : rIntBufferRight, /* output real qmf buffer */
								(i == 0) ? iIntBufferLeft[0] : iIntBufferRight  /* output imag qmf buffer */
		);
	}

	/* free temporary hybrid qmf values of one timeslot */
} /* END ApplyPsSlot */
