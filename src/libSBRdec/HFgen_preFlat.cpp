/* -----------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2019 Fraunhofer-Gesellschaft zur Förderung der angewandten
Forschung e.V. All rights reserved.

 1.    INTRODUCTION
The Fraunhofer FDK AAC Codec Library for Android ("FDK AAC Codec") is software
that implements the MPEG Advanced Audio Coding ("AAC") encoding and decoding
scheme for digital audio. This FDK AAC Codec software is intended to be used on
a wide variety of Android devices.

AAC's HE-AAC and HE-AAC v2 versions are regarded as today's most efficient
general perceptual audio codecs. AAC-ELD is considered the best-performing
full-bandwidth communications codec by independent studies and is widely
deployed. AAC has been standardized by ISO and IEC as part of the MPEG
specifications.

Patent licenses for necessary patent claims for the FDK AAC Codec (including
those of Fraunhofer) may be obtained through Via Licensing
(www.vialicensing.com) or through the respective patent owners individually for
the purpose of encoding or decoding bit streams in products that are compliant
with the ISO/IEC MPEG audio standards. Please note that most manufacturers of
Android devices already license these patent claims through Via Licensing or
directly from the patent owners, and therefore FDK AAC Codec software may
already be covered under those patent licenses when it is used for those
licensed purposes only.

Commercially-licensed AAC software libraries, including floating-point versions
with enhanced sound quality, are also available from Fraunhofer. Users are
encouraged to check the Fraunhofer website for additional applications
information and documentation.

2.    COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification,
are permitted without payment of copyright license fees provided that you
satisfy the following conditions:

You must retain the complete text of this software license in redistributions of
the FDK AAC Codec or your modifications thereto in source code form.

You must retain the complete text of this software license in the documentation
and/or other materials provided with redistributions of the FDK AAC Codec or
your modifications thereto in binary form. You must make available free of
charge copies of the complete source code of the FDK AAC Codec and your
modifications thereto to recipients of copies in binary form.

The name of Fraunhofer may not be used to endorse or promote products derived
from this library without prior written permission.

You may not charge copyright license fees for anyone to use, copy or distribute
the FDK AAC Codec software or your modifications thereto.

Your modified versions of the FDK AAC Codec must carry prominent notices stating
that you changed the software and the date of any change. For modified versions
of the FDK AAC Codec, the term "Fraunhofer FDK AAC Codec Library for Android"
must be replaced by the term "Third-Party Modified Version of the Fraunhofer FDK
AAC Codec Library for Android."

3.    NO PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without
limitation the patents of Fraunhofer, ARE GRANTED BY THIS SOFTWARE LICENSE.
Fraunhofer provides no warranty of patent non-infringement with respect to this
software.

You may use this FDK AAC Codec software or modifications thereto only for
purposes that are authorized by appropriate patent licenses.

4.    DISCLAIMER

This FDK AAC Codec software is provided by Fraunhofer on behalf of the copyright
holders and contributors "AS IS" and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
including but not limited to the implied warranties of merchantability and
fitness for a particular purpose. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE for any direct, indirect, incidental, special, exemplary,
or consequential damages, including but not limited to procurement of substitute
goods or services; loss of use, data, or profits, or business interruption,
however caused and on any theory of liability, whether in contract, strict
liability, or tort (including negligence), arising in any way out of the use of
this software, even if advised of the possibility of such damage.

5.    CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Audio and Multimedia Departments - FDK AAC LL
Am Wolfsmantel 33
91058 Erlangen, Germany

www.iis.fraunhofer.de/amm
amm-info@iis.fraunhofer.de
----------------------------------------------------------------------------- */

/**************************** SBR decoder library ******************************

   Author(s):   Oliver Moser, Manuel Jander, Matthias Hildenbrand

   Description: QMF frequency pre-whitening for SBR.
				In the documentation the terms "scale factor" and "exponent"
				mean the same. Variables containing such information have
				the suffix "_sf".

*******************************************************************************/

#include <memory.h>
#include "HFgen_preFlat.h"

#define POLY_ORDER   3
#define MAXLOWBANDS  32
#define LOG10FAC     0.752574989159953f /* == 10/log2(10) * 2^-2 */
#define LOG10FAC_INV 0.664385618977472f /* == log2(10)/20 * 2^2  */

#define FIXP_CHB         int16_t /* STB sinus Tab used in transformation */
#define FX_CHB2FX_DBL(a) FX_SGL2FX_DBL(a)

typedef struct backsubst_data
{
	FIXP_CHB Lnorm1d[3]; /*!< Normalized L matrix */
	int8_t   Lnorm1d_sf[3];
	FIXP_CHB Lnormii[3]; /*!< The diagonal data points [i][i] of the normalized L matrix */
	int8_t   Lnormii_sf[3];
	FIXP_CHB Bmul0[4]; /*!< To normalize L*x=b, Bmul0 is what we need to multiply b with. */
	int8_t   Bmul0_sf[4];
	FIXP_CHB LnormInv1d[6]; /*!< Normalized inverted L matrix (L') */
	int8_t   LnormInv1d_sf[6];
	FIXP_CHB
	Bmul1[4]; /*!< To normalize L'*x=b, Bmul1 is what we need to multiply b
				 with. */
	int8_t Bmul1_sf[4];
} backsubst_data;

/* for each element n do, f(n) = trunc(log2(n))+1  */
const uint8_t getLog2[32] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
							 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

/** \def  BSD_IDX_OFFSET
 *
 *  bsd[] begins at index 0 with data for numBands=5. The correct bsd[] is
 *  indexed like bsd[numBands-BSD_IDX_OFFSET].
 */
#define BSD_IDX_OFFSET 5

#define N_NUMBANDS MAXLOWBANDS - BSD_IDX_OFFSET + 1 /*!< Number of backsubst_data elements in bsd */

const backsubst_data bsd[N_NUMBANDS] = {
	{
		/* numBands=5 */
		{26312, 17017, 27006},
		{-1, 0, 0},
		{26278, 21076, 23262},
		{3, 4, 1},
		{29990, 28202, 25892, 22914},
		{-6, -4, -2, 0},
		{24734, 22984, 26655, 17423, 18977, 21443},
		{1, 0, -1, -1, -3, -5},
		{29990, 22616, 16653, 32533},
		{-6, -1, 2, 0},
	},
	{
		/* numBands=6 */
		{26772, 18498, 27234},
		{-1, 0, 0},
		{25542, 20112, 25222},
		{3, 4, 1},
		{29284, 16970, 19176, 20859},
		{-7, -4, -2, 0},
		{20875, 18468, 17270, 28272, 25020, 23001},
		{1, 0, -1, -2, -4, -6},
		{29284, 26456, 23540, 32111},
		{-7, -2, 1, 0},
	},
	{
		/* numBands=7 */
		{27035, 19339, 27365},
		{-1, 0, 0},
		{25146, 19661, 26864},
		{3, 4, 1},
		{32367, 22274, 29864, 19258},
		{-8, -5, -3, 0},
		{17994, 30948, 24289, 23808, 17757, 27537},
		{1, -1, -2, -2, -4, -7},
		{32367, 17093, 17918, 31576},
		{-8, -2, 1, 0},
	},
	{
		/* numBands=8 */
		{27200, 19866, 27447},
		{-1, 0, 0},
		{24908, 19408, 28250},
		{3, 4, 1},
		{19513, 31080, 24101, 17968},
		{-8, -6, -3, 0},
		{31565, 26663, 18044, 20572, 26529, 17792},
		{0, -1, -2, -2, -5, -7},
		{19513, 23626, 28550, 30982},
		{-8, -3, 0, 0},
	},
	{
		/* numBands=9 */
		{27310, 20217, 27502},
		{-1, 0, 0},
		{24753, 19252, 29429},
		{3, 4, 1},
		{25095, 22696, 19979, 16904},
		{-9, -6, -3, 0},
		{28078, 23435, 27894, 18116, 20579, 24323},
		{0, -1, -3, -2, -5, -8},
		{25095, 17145, 23476, 30363},
		{-9, -3, 0, 0},
	},
	{
		/* numBands=10 */
		{27386, 20463, 27540},
		{-1, 0, 0},
		{24646, 19148, 30443},
		{3, 4, 1},
		{16961, 17169, 16911, 32012},
		{-9, -6, -3, -1},
		{25267, 20912, 22219, 32373, 16433, 17362},
		{0, -1, -3, -3, -5, -8},
		{16961, 25827, 19764, 29741},
		{-9, -4, 0, 0},
	},
	{
		/* numBands=11 */
		{27442, 20642, 27568},
		{-1, 0, 0},
		{24570, 19074, 31323},
		{3, 4, 1},
		{23856, 26716, 29110, 30473},
		{-10, -7, -4, -1},
		{22957, 18884, 18122, 29260, 26854, 25653},
		{0, -1, -3, -3, -6, -9},
		{23856, 20032, 16945, 29129},
		{-10, -4, 0, 0},
	},
	{
		/* numBands=12 */
		{27484, 20777, 27589},
		{-1, 0, 0},
		{24512, 19021, 32092},
		{3, 4, 1},
		{17328, 21269, 25398, 29134},
		{-10, -7, -4, -1},
		{21027, 17217, 30134, 26695, 22355, 19489},
		{0, -1, -4, -3, -6, -9},
		{17328, 31821, 29486, 28533},
		{-10, -5, -1, 0},
	},
	{
		/* numBands=13 */
		{27517, 20880, 27605},
		{-1, 0, 0},
		{24469, 18981, 16385},
		{3, 4, 2},
		{25857, 17260, 22413, 27957},
		{-11, -7, -4, -1},
		{19393, 31645, 25452, 24545, 18901, 30307},
		{0, -2, -4, -3, -6, -10},
		{25857, 25776, 25966, 27959},
		{-11, -5, -1, 0},
	},
	{
		/* numBands=14 */
		{27542, 20962, 27618},
		{-1, 0, 0},
		{24434, 18949, 16686},
		{3, 4, 2},
		{19736, 28468, 19971, 26911},
		{-11, -8, -4, -1},
		{17991, 29276, 21786, 22717, 32382, 24031},
		{0, -2, -4, -3, -7, -10},
		{19736, 21228, 23097, 27407},
		{-11, -5, -1, 0},
	},
	{
		/* numBands=15 */
		{27562, 21027, 27628},
		{-1, 0, 0},
		{24407, 18925, 16955},
		{3, 4, 2},
		{30717, 23804, 17941, 25973},
		{-12, -8, -4, -1},
		{16777, 27238, 18860, 21142, 28051, 19376},
		{0, -2, -4, -3, -7, -10},
		{30717, 17730, 20723, 26879},
		{-12, -5, -1, 0},
	},
	{
		/* numBands=16 */
		{27579, 21080, 27636},
		{-1, 0, 0},
		{24385, 18905, 17197},
		{3, 4, 2},
		{24310, 20144, 32467, 25127},
		{-12, -8, -5, -1},
		{31428, 25467, 16487, 19772, 24535, 31702},
		{-1, -2, -4, -3, -7, -11},
		{24310, 29981, 18731, 26374},
		{-12, -6, -1, 0},
	},
	{
		/* numBands=17 */
		{27592, 21124, 27643},
		{-1, 0, 0},
		{24367, 18888, 17416},
		{3, 4, 2},
		{19524, 17226, 29562, 24358},
		{-12, -8, -5, -1},
		{29554, 23912, 29074, 18570, 21641, 26264},
		{-1, -2, -5, -3, -7, -11},
		{19524, 25619, 17040, 25892},
		{-12, -6, -1, 0},
	},
	{
		/* numBands=18 */
		{27604, 21160, 27649},
		{-1, 0, 0},
		{24352, 18875, 17614},
		{3, 4, 2},
		{31769, 29734, 27065, 23656},
		{-13, -9, -5, -1},
		{27890, 22537, 25827, 17505, 19232, 22003},
		{-1, -2, -5, -3, -7, -11},
		{31769, 22097, 31180, 25431},
		{-13, -6, -2, 0},
	},
	{
		/* numBands=19 */
		{27613, 21191, 27654},
		{-1, 0, 0},
		{24339, 18864, 17794},
		{3, 4, 2},
		{26146, 25874, 24901, 23011},
		{-13, -9, -5, -1},
		{26402, 21312, 23097, 16556, 17203, 18616},
		{-1, -2, -5, -3, -7, -11},
		{26146, 19219, 28669, 24992},
		{-13, -6, -2, 0},
	},
	{
		/* numBands=20 */
		{27621, 21217, 27658},
		{-1, 0, 0},
		{24328, 18854, 17960},
		{3, 4, 2},
		{21740, 22681, 23010, 22415},
		{-13, -9, -5, -1},
		{25064, 20214, 20778, 31409, 30960, 31781},
		{-1, -2, -5, -4, -8, -12},
		{21740, 16840, 26479, 24571},
		{-13, -6, -2, 0},
	},
	{
		/* numBands=21 */
		{27628, 21239, 27661},
		{-1, 0, 0},
		{24319, 18846, 18112},
		{3, 4, 2},
		{18245, 20014, 21347, 21864},
		{-13, -9, -5, -1},
		{23854, 19223, 18792, 29873, 28006, 27345},
		{-1, -2, -5, -4, -8, -12},
		{18245, 29707, 24555, 24169},
		{-13, -7, -2, 0},
	},
	{
		/* numBands=22 */
		{27634, 21258, 27664},
		{-1, 0, 0},
		{24311, 18839, 18252},
		{3, 4, 2},
		{30882, 17766, 19876, 21351},
		{-14, -9, -5, -1},
		{22755, 18326, 17078, 28481, 25457, 23698},
		{-1, -2, -5, -4, -8, -12},
		{30882, 26361, 22854, 23785},
		{-14, -7, -2, 0},
	},
	{
		/* numBands=23 */
		{27639, 21275, 27667},
		{-1, 0, 0},
		{24304, 18833, 18381},
		{3, 4, 2},
		{26335, 31712, 18566, 20873},
		{-14, -10, -5, -1},
		{21752, 17508, 31177, 27212, 23240, 20672},
		{-1, -2, -6, -4, -8, -12},
		{26335, 23521, 21341, 23417},
		{-14, -7, -2, 0},
	},
	{
		/* numBands=24 */
		{27644, 21290, 27669},
		{-1, 0, 0},
		{24298, 18828, 18501},
		{3, 4, 2},
		{22614, 28443, 17393, 20425},
		{-14, -10, -5, -1},
		{20834, 16761, 28572, 26052, 21301, 18140},
		{-1, -2, -6, -4, -8, -12},
		{22614, 21091, 19988, 23065},
		{-14, -7, -2, 0},
	},
	{
		/* numBands=25 */
		{27648, 21303, 27671},
		{-1, 0, 0},
		{24293, 18823, 18613},
		{3, 4, 2},
		{19542, 25628, 32679, 20005},
		{-14, -10, -6, -1},
		{19990, 32149, 26280, 24987, 19595, 32010},
		{-1, -3, -6, -4, -8, -13},
		{19542, 18999, 18772, 22727},
		{-14, -7, -2, 0},
	},
	{
		/* numBands=26 */
		{27651, 21314, 27673},
		{-1, 0, 0},
		{24288, 18819, 18717},
		{3, 4, 2},
		{16987, 23187, 30776, 19610},
		{-14, -10, -6, -1},
		{19211, 30884, 24254, 24005, 18086, 28385},
		{-1, -3, -6, -4, -8, -13},
		{16987, 17187, 17675, 22402},
		{-14, -7, -2, 0},
	},
	{
		/* numBands=27 */
		{27655, 21324, 27674},
		{-1, 0, 0},
		{24284, 18815, 18814},
		{3, 4, 2},
		{29691, 21061, 29051, 19237},
		{-15, -10, -6, -1},
		{18491, 29716, 22454, 23098, 16745, 25288},
		{-1, -3, -6, -4, -8, -13},
		{29691, 31216, 16681, 22091},
		{-15, -8, -2, 0},
	},
	{
		/* numBands=28 */
		{27657, 21334, 27676},
		{-1, 0, 0},
		{24280, 18812, 18906},
		{3, 4, 2},
		{26079, 19197, 27481, 18885},
		{-15, -10, -6, -1},
		{17822, 28632, 20846, 22257, 31096, 22625},
		{-1, -3, -6, -4, -9, -13},
		{26079, 28449, 31553, 21791},
		{-15, -8, -3, 0},
	},
	{
		/* numBands=29 */
		{27660, 21342, 27677},
		{-1, 0, 0},
		{24277, 18809, 18991},
		{3, 4, 2},
		{23012, 17557, 26047, 18551},
		{-15, -10, -6, -1},
		{17200, 27625, 19406, 21475, 28950, 20324},
		{-1, -3, -6, -4, -9, -13},
		{23012, 26015, 29903, 21503},
		{-15, -8, -3, 0},
	},
	{
		/* numBands=30 */
		{27662, 21349, 27678},
		{-1, 0, 0},
		{24274, 18806, 19071},
		{3, 4, 2},
		{20394, 32212, 24734, 18234},
		{-15, -11, -6, -1},
		{16620, 26687, 18110, 20746, 27019, 18325},
		{-1, -3, -6, -4, -9, -13},
		{20394, 23862, 28391, 21225},
		{-15, -8, -3, 0},
	},
	{
		/* numBands=31 */
		{27664, 21356, 27679},
		{-1, 0, 0},
		{24271, 18804, 19147},
		{3, 4, 2},
		{18147, 29635, 23528, 17934},
		{-15, -11, -6, -1},
		{32156, 25810, 16940, 20066, 25274, 16579},
		{-2, -3, -6, -4, -9, -13},
		{18147, 21951, 27003, 20958},
		{-15, -8, -3, 0},
	},
	{
		// numBands=32
		{27666, 21362, 27680},
		{-1, 0, 0},
		{24269, 18802, 19219},
		{3, 4, 2},
		{32417, 27338, 22416, 17647},
		{-16, -11, -6, -1},
		{31140, 24989, 31759, 19428, 23693, 30097},
		{-2, -3, -7, -4, -9, -14},
		{32417, 20248, 25725, 20700},
		{-16, -8, -3, 0},
	},
};

/** \def  SUM_SAFETY
 *
 *  SUM_SAFTEY defines the bits needed to right-shift every summand in
 *  order to be overflow-safe. In the two backsubst functions we sum up 4
 *  values. Since one of which is definitely not MAXVAL_DBL (the L[x][y]),
 *  we spare just 2 safety bits instead of 3.
 */
#define SUM_SAFETY 2

/**
 * \brief  Solves L*x=b via backsubstitution according to the following
 * structure:
 *
 *  x[0] =  b[0];
 *  x[1] = (b[1]                               - x[0]) / L[1][1];
 *  x[2] = (b[2] - x[1]*L[2][1]                - x[0]) / L[2][2];
 *  x[3] = (b[3] - x[2]*L[3][2] - x[1]*L[3][1] - x[0]) / L[3][3];
 *
 * \param[in]  numBands  SBR crossover band index
 * \param[in]  b         the b in L*x=b (one-dimensional)
 * \param[out] x         output polynomial coefficients (mantissa)
 * \param[out] x_sf      exponents of x[]
 */
static void backsubst_fw(const int32_t numBands, const int32_t *const b, int32_t *x, int32_t *x_sf) {
	int32_t i, k;
	int32_t m; /* the trip counter that indexes incrementally through Lnorm1d[] */

	const FIXP_CHB *pLnorm1d = bsd[numBands - BSD_IDX_OFFSET].Lnorm1d;
	const int8_t   *pLnorm1d_sf = bsd[numBands - BSD_IDX_OFFSET].Lnorm1d_sf;
	const FIXP_CHB *pLnormii = bsd[numBands - BSD_IDX_OFFSET].Lnormii;
	const int8_t   *pLnormii_sf = bsd[numBands - BSD_IDX_OFFSET].Lnormii_sf;

	x[0] = b[0];

	for(i = 1, m = 0; i <= POLY_ORDER; ++i) {
		int32_t sum = b[i] >> SUM_SAFETY;
		int32_t sum_sf = x_sf[i];
		for(k = i - 1; k > 0; --k, ++m) {
			int32_t e;
			int32_t mult = fMultNorm(FX_CHB2FX_DBL(pLnorm1d[m]), x[k], &e);
			int32_t mult_sf = pLnorm1d_sf[m] + x_sf[k] + e;

			/* check if the new summand mult has a different sf than the sum currently
			 * has */
			int32_t diff = mult_sf - sum_sf;

			if(diff > 0) {
				/* yes, and it requires the sum to be adjusted (scaled down) */
				sum >>= diff;
				sum_sf = mult_sf;
			}
			else if(diff < 0) {
				/* yes, but here mult needs to be scaled down */
				mult >>= -diff;
			}
			sum -= (mult >> SUM_SAFETY);
		}

		/* - x[0] */
		if(x_sf[0] > sum_sf) {
			sum >>= (x_sf[0] - sum_sf);
			sum_sf = x_sf[0];
		}
		sum -= (x[0] >> (sum_sf - x_sf[0] + SUM_SAFETY));

		/* instead of the division /L[i][i], we multiply by the inverse */
		int32_t e;
		x[i] = fMultNorm(sum, FX_CHB2FX_DBL(pLnormii[i - 1]), &e);
		x_sf[i] = sum_sf + pLnormii_sf[i - 1] + e + SUM_SAFETY;
	}
}

/**
 * \brief Solves L*x=b via backsubstitution according to the following
 * structure:
 *
 *  x[3] = b[3];
 *  x[2] = b[2] - L[2][3]*x[3];
 *  x[1] = b[1] - L[1][2]*x[2] - L[1][3]*x[3];
 *  x[0] = b[0] - L[0][1]*x[1] - L[0][2]*x[2] - L[0][3]*x[3];
 *
 * \param[in]  numBands  SBR crossover band index
 * \param[in]  b         the b in L*x=b (one-dimensional)
 * \param[out] x         solution vector
 * \param[out] x_sf      exponents of x[]
 */
static void backsubst_bw(const int32_t numBands, const int32_t *const b, int32_t *x, int32_t *x_sf) {
	int32_t i, k;
	int32_t m; /* the trip counter that indexes incrementally through LnormInv1d[] */

	const FIXP_CHB *pLnormInv1d = bsd[numBands - BSD_IDX_OFFSET].LnormInv1d;
	const int8_t   *pLnormInv1d_sf = bsd[numBands - BSD_IDX_OFFSET].LnormInv1d_sf;

	x[POLY_ORDER] = b[POLY_ORDER];

	for(i = POLY_ORDER - 1, m = 0; i >= 0; i--) {
		int32_t sum = b[i] >> SUM_SAFETY;
		int32_t sum_sf = x_sf[i]; /* sum's sf but disregarding SUM_SAFETY (added at the
								 iteration's end) */

		for(k = i + 1; k <= POLY_ORDER; ++k, ++m) {
			int32_t e;
			int32_t mult = fMultNorm(FX_CHB2FX_DBL(pLnormInv1d[m]), x[k], &e);
			int32_t mult_sf = pLnormInv1d_sf[m] + x_sf[k] + e;

			/* check if the new summand mult has a different sf than sum currently has
			 */
			int32_t diff = mult_sf - sum_sf;

			if(diff > 0) {
				/* yes, and it requires the sum v to be adjusted (scaled down) */
				sum >>= diff;
				sum_sf = mult_sf;
			}
			else if(diff < 0) {
				/* yes, but here mult needs to be scaled down */
				mult >>= -diff;
			}

			/* mult has now the same sf than what it is about to be added to. */
			/* scale mult down additionally so that building the sum is overflow-safe.
			 */
			sum -= (mult >> SUM_SAFETY);
		}

		x_sf[i] = sum_sf + SUM_SAFETY;
		x[i] = sum;
	}
}

/**
 * \brief  Solves a system of linear equations (L*x=b) with the Cholesky
 * algorithm.
 *
 * \param[in]     numBands  SBR crossover band index
 * \param[in,out] b         input: vector b, output: solution vector p.
 * \param[in,out] b_sf      input: exponent of b; output: exponent of solution
 * p.
 */
static void choleskySolve(const int32_t numBands, int32_t *b, int32_t *b_sf) {
	int32_t i, e;

	const FIXP_CHB *pBmul0 = bsd[numBands - BSD_IDX_OFFSET].Bmul0;
	const int8_t   *pBmul0_sf = bsd[numBands - BSD_IDX_OFFSET].Bmul0_sf;
	const FIXP_CHB *pBmul1 = bsd[numBands - BSD_IDX_OFFSET].Bmul1;
	const int8_t   *pBmul1_sf = bsd[numBands - BSD_IDX_OFFSET].Bmul1_sf;

	/* normalize b */
	int32_t bnormed[POLY_ORDER + 1];
	for(i = 0; i <= POLY_ORDER; ++i) {
		bnormed[i] = fMultNorm(b[i], FX_CHB2FX_DBL(pBmul0[i]), &e);
		b_sf[i] += pBmul0_sf[i] + e;
	}

	backsubst_fw(numBands, bnormed, b, b_sf);

	/* normalize b again */
	for(i = 0; i <= POLY_ORDER; ++i) {
		bnormed[i] = fMultNorm(b[i], FX_CHB2FX_DBL(pBmul1[i]), &e);
		b_sf[i] += pBmul1_sf[i] + e;
	}

	backsubst_bw(numBands, bnormed, b, b_sf);
}

/**
 * \brief  Find polynomial approximation of vector y with implicit abscisas
 * x=0,1,2,3..n-1
 *
 *  The problem (V^T * V * p = V^T * y) is solved with Cholesky.
 *  V is the Vandermode Matrix constructed with x = 0...n-1;
 *  A = V^T * V; b = V^T * y;
 *
 * \param[in]  numBands  SBR crossover band index (BSD_IDX_OFFSET <= numBands <=
 * MAXLOWBANDS)
 * \param[in]  y         input vector (mantissa)
 * \param[in]  y_sf      exponents of y[]
 * \param[out] p         output polynomial coefficients (mantissa)
 * \param[out] p_sf      exponents of p[]
 */
static void polyfit(const int32_t numBands, const int32_t *const y, const int32_t y_sf, int32_t *p, int32_t *p_sf) {
	int32_t i, k;
	int32_t v[POLY_ORDER + 1];
	int32_t sum_saftey = getLog2[numBands - 1];

	assert((numBands >= BSD_IDX_OFFSET) && (numBands <= MAXLOWBANDS));

	/* construct vector b[] temporarily stored in array p[] */
	memset(p, 0, (POLY_ORDER + 1) * sizeof(int32_t));

	/* p[] are the sums over n values and each p[i] has its own sf */
	for(i = 0; i <= POLY_ORDER; ++i) p_sf[i] = 1 - DFRACT_BITS;

	for(k = 0; k < numBands; k++) {
		v[0] = (int32_t)1;
		for(i = 1; i <= POLY_ORDER; i++) { v[i] = k * v[i - 1]; }

		for(i = 0; i <= POLY_ORDER; i++) {
			if(v[POLY_ORDER - i] != 0 && y[k] != int32_t(0)) {
				int32_t e;
				int32_t mult = fMultNorm((int32_t)v[POLY_ORDER - i], y[k], &e);
				int32_t sf = DFRACT_BITS - 1 + y_sf + e;

				/* check if the new summand has a different sf than the sum p[i]
				 * currently has */
				int32_t diff = sf - p_sf[i];

				if(diff > 0) {
					/* yes, and it requires the sum p[i] to be adjusted (scaled down) */
					p[i] >>= fMin(DFRACT_BITS - 1, diff);
					p_sf[i] = sf;
				}
				else if(diff < 0) {
					/* yes, but here mult needs to be scaled down */
					mult >>= -diff;
				}

				/* mult has now the same sf than what it is about to be added to.
				   scale mult down additionally so that building the sum is
				   overflow-safe. */
				p[i] += mult >> sum_saftey;
			}
		}
	}

	p_sf[0] += sum_saftey;
	p_sf[1] += sum_saftey;
	p_sf[2] += sum_saftey;
	p_sf[3] += sum_saftey;

	choleskySolve(numBands, p, p_sf);
}

/**
 * \brief  Calculates the output of a POLY_ORDER-degree polynomial function
 *         with Horner scheme:
 *
 *         y(x) = p3 + p2*x + p1*x^2 + p0*x^3
 *              = p3 + x*(p2 + x*(p1 + x*p0))
 *
 *         The for loop iterates through the mult/add parts in y(x) as above,
 *         during which regular upscaling ensures a stable exponent of the
 *         result.
 *
 * \param[in]  p       coefficients as in y(x)
 * \param[in]  p_sf    exponents of p[]
 * \param[in]  x_int   non-fractional integer representation of x as in y(x)
 * \param[out] out_sf  exponent of return value
 *
 * \return             result y(x)
 */
static int32_t polyval(const int32_t *const p, const int32_t *const p_sf, const int32_t x_int, int32_t *out_sf) {
	assert(x_int <= 31); /* otherwise getLog2[] needs more elements */

	int32_t k, x_sf;
	int32_t result_sf; /* working space to compute return value *out_sf */
	int32_t x;         /* fractional value of x_int */
	int32_t result;    /* return value */

	/* if x == 0, then y(x) is just p3 */
	if(x_int != 0) {
		x_sf = getLog2[x_int];
		x = (int32_t)x_int << (DFRACT_BITS - 1 - x_sf);
	}
	else {
		*out_sf = p_sf[3];
		return p[3];
	}

	result = p[0];
	result_sf = p_sf[0];

	for(k = 1; k <= POLY_ORDER; ++k) {
		int32_t mult = fMult(x, result);
		int32_t mult_sf = x_sf + result_sf;

		int32_t room = CountLeadingBits(mult);
		mult <<= room;
		mult_sf -= room;

		int32_t pp = p[k];
		int32_t pp_sf = p_sf[k];

		/* equalize the shift factors of pp and mult so that we can sum them up */
		int32_t diff = pp_sf - mult_sf;

		if(diff > 0) {
			diff = fMin(diff, DFRACT_BITS - 1);
			mult >>= diff;
		}
		else if(diff < 0) {
			diff = fMax(diff, 1 - DFRACT_BITS);
			pp >>= -diff;
		}

		/* downshift by 1 to ensure safe summation */
		mult >>= 1;
		mult_sf++;
		pp >>= 1;
		pp_sf++;

		result_sf = fMax(pp_sf, mult_sf);

		result = mult + pp;
		/* rarely, mult and pp happen to be almost equal except their sign,
		and then upon summation, result becomes so small, that it is within
		the inaccuracy range of a few bits, and then the relative error
		produced by this function may become HUGE */
	}

	*out_sf = result_sf;
	return result;
}

void sbrDecoder_calculateGainVec(int32_t **sourceBufferReal, int32_t **sourceBufferImag, int32_t sourceBuf_e_overlap,
								 int32_t sourceBuf_e_current, int32_t overlap, int32_t *GainVec, int32_t *GainVec_exp,
								 int32_t numBands, const int32_t startSample, const int32_t stopSample) {
	int32_t p[POLY_ORDER + 1];
	int32_t meanNrg;
	int32_t LowEnv[MAXLOWBANDS];
	int32_t invNumBands = GetInvInt(numBands);
	int32_t invNumSlots = GetInvInt(stopSample - startSample);
	int32_t i, loBand, exp, scale_nrg, scale_nrg_ov;
	int32_t sum_scale = 5, sum_scale_ov = 3;

	if(overlap > 8) {
		assert(overlap <= 16);
		sum_scale_ov += 1;
		sum_scale += 1;
	}

	/* exponents of energy values */
	sourceBuf_e_overlap = sourceBuf_e_overlap * 2 + sum_scale_ov;
	sourceBuf_e_current = sourceBuf_e_current * 2 + sum_scale;
	exp = fMax(sourceBuf_e_overlap, sourceBuf_e_current);
	scale_nrg = sourceBuf_e_current - exp;
	scale_nrg_ov = sourceBuf_e_overlap - exp;

	meanNrg = (int32_t)0;
	/* Calculate the spectral envelope in dB over the current copy-up frame. */
	for(loBand = 0; loBand < numBands; loBand++) {
		int32_t nrg_ov, nrg;
		int32_t reserve = 0, exp_new;
		int32_t maxVal = FL2FX_DBL(0.0f);

		for(i = startSample; i < stopSample; i++) {
			maxVal |= (int32_t)((int32_t)(sourceBufferReal[i][loBand]) ^
								((int32_t)sourceBufferReal[i][loBand] >> (DFRACT_BITS - 1)));
			maxVal |= (int32_t)((int32_t)(sourceBufferImag[i][loBand]) ^
								((int32_t)sourceBufferImag[i][loBand] >> (DFRACT_BITS - 1)));
		}

		if(maxVal != FL2FX_DBL(0.0f)) { reserve = CntLeadingZeros(maxVal) - 2; }

		nrg_ov = nrg = (int32_t)0;
		if(scale_nrg_ov > -31) {
			for(i = startSample; i < overlap; i++) {
				nrg_ov += (fPow2Div2(scaleValue(sourceBufferReal[i][loBand], reserve)) +
						   fPow2Div2(scaleValue(sourceBufferImag[i][loBand], reserve))) >>
						  sum_scale_ov;
			}
		}
		else { scale_nrg_ov = 0; }
		if(scale_nrg > -31) {
			for(i = overlap; i < stopSample; i++) {
				nrg += (fPow2Div2(scaleValue(sourceBufferReal[i][loBand], reserve)) +
						fPow2Div2(scaleValue(sourceBufferImag[i][loBand], reserve))) >>
					   sum_scale;
			}
		}
		else { scale_nrg = 0; }

		nrg = (scaleValue(nrg_ov, scale_nrg_ov) >> 1) + (scaleValue(nrg, scale_nrg) >> 1);
		nrg = fMult(nrg, invNumSlots);

		exp_new = exp - (2 * reserve) + 2; /* +1 for addition directly above, +1 for fPow2Div2 in loops above */

		/* LowEnv = 10*log10(nrg) = log2(nrg) * 10/log2(10) */
		/* exponent of logarithmic energy is 8 */
		if(nrg > (int32_t)0) {
			int32_t exp_log2;
			nrg = CalcLog2(nrg, exp_new, &exp_log2);
			nrg = scaleValue(nrg, exp_log2 - 6);
			nrg = fMult((int16_t)24660, nrg);
		}
		else { nrg = (int32_t)0; }
		LowEnv[loBand] = nrg;
		meanNrg += fMult(nrg, invNumBands);
	}
	exp = 6 + 2; /* exponent of LowEnv: +2 is exponent of LOG10FAC */

	/* subtract mean before polynomial approximation to reduce dynamic of p[] */
	for(loBand = 0; loBand < numBands; loBand++) { LowEnv[loBand] = meanNrg - LowEnv[loBand]; }

	/* For numBands < BSD_IDX_OFFSET (== POLY_ORDER+2) we dont get an
	   overdetermined equation system. The calculated polynomial will exactly fit
	   the input data and evaluating the polynomial will lead to the same vector
	   than the original input vector: lowEnvSlope[] == lowEnv[]
	*/
	if(numBands > POLY_ORDER + 1) {
		/* Find polynomial approximation of LowEnv */
		int32_t p_sf[POLY_ORDER + 1];

		polyfit(numBands, LowEnv, exp, p, p_sf);

		for(i = 0; i < numBands; i++) {
			int32_t sf;

			/* lowBandEnvSlope[i] = tmp; */
			int32_t tmp = polyval(p, p_sf, i, &sf);

			/* GainVec = 10^((mean(y)-y)/20) = 2^( (mean(y)-y) * log2(10)/20 ) */
			tmp = fMult(tmp, (int16_t)21771);
			GainVec[i] = f2Pow(tmp, sf - 2, &GainVec_exp[i]); /* -2 is exponent of LOG10FAC_INV */
		}
	}
	else { /* numBands <= POLY_ORDER+1 */
		for(i = 0; i < numBands; i++) {
			int32_t sf = exp; /* exponent of LowEnv[] */

			/* lowBandEnvSlope[i] = LowEnv[i]; */
			int32_t tmp = LowEnv[i];

			/* GainVec = 10^((mean(y)-y)/20) = 2^( (mean(y)-y) * log2(10)/20 ) */
			tmp = fMult(tmp, (int16_t)21771);
			GainVec[i] = f2Pow(tmp, sf - 2, &GainVec_exp[i]); /* -2 is exponent of LOG10FAC_INV */
		}
	}
}
