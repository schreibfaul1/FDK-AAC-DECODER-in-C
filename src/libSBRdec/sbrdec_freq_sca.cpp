
/**************************** SBR decoder library ******************************

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  Frequency scale calculation
*/
#include "../libAACdec/newAACDecoder.h"
#include "sbrdec_freq_sca.h"
#include "transcendent.h"
#include "sbr_rom.h"
#include "env_extr.h"

#define MAX_OCTAVE 29
#define MAX_SECOND_REGION 50

static int32_t numberOfBands(int16_t bpo_div16, int32_t start, int32_t stop, int32_t warpFlag);
static void CalcBands(uint8_t *diff, uint8_t start, uint8_t stop, uint8_t num_bands);
static SBR_ERROR modifyBands(uint8_t max_band, uint8_t *diff, uint8_t length);
static void cumSum(uint8_t start_value, uint8_t *diff, uint8_t length,
                   uint8_t *start_adress);

/*!
  \brief     Retrieve QMF-band where the SBR range starts

  Convert startFreq which was read from the bitstream into a
  QMF-channel number.

  \return  Number of start band
*/
static uint8_t getStartBand(
    uint32_t fs,              /*!< Output sampling frequency */
    uint8_t startFreq,      /*!< Index to table of possible start bands */
    uint32_t headerDataFlags) /*!< Info to SBR mode */
{
  int32_t band;
  uint32_t fsMapped = fs;
  SBR_RATE rate = DUAL;

  if (headerDataFlags & (SBRDEC_SYNTAX_USAC | SBRDEC_SYNTAX_RSVD50)) {
    if (headerDataFlags & SBRDEC_QUAD_RATE) {
      rate = QUAD;
    }
    fsMapped = sbrdec_mapToStdSampleRate(fs, 1);
  }

  assert(2 * (rate + 1) <= (4));

  switch (fsMapped) {
    case 192000:
      band = FDK_sbrDecoder_sbr_start_freq_192[startFreq];
      break;
    case 176400:
      band = FDK_sbrDecoder_sbr_start_freq_176[startFreq];
      break;
    case 128000:
      band = FDK_sbrDecoder_sbr_start_freq_128[startFreq];
      break;
    case 96000:
    case 88200:
      band = FDK_sbrDecoder_sbr_start_freq_88[rate][startFreq];
      break;
    case 64000:
      band = FDK_sbrDecoder_sbr_start_freq_64[rate][startFreq];
      break;
    case 48000:
      band = FDK_sbrDecoder_sbr_start_freq_48[rate][startFreq];
      break;
    case 44100:
      band = FDK_sbrDecoder_sbr_start_freq_44[rate][startFreq];
      break;
    case 40000:
      band = FDK_sbrDecoder_sbr_start_freq_40[rate][startFreq];
      break;
    case 32000:
      band = FDK_sbrDecoder_sbr_start_freq_32[rate][startFreq];
      break;
    case 24000:
      band = FDK_sbrDecoder_sbr_start_freq_24[rate][startFreq];
      break;
    case 22050:
      band = FDK_sbrDecoder_sbr_start_freq_22[rate][startFreq];
      break;
    case 16000:
      band = FDK_sbrDecoder_sbr_start_freq_16[rate][startFreq];
      break;
    default:
      band = 255;
  }

  return band;
}

/*!
  \brief     Retrieve QMF-band where the SBR range starts

  Convert startFreq which was read from the bitstream into a
  QMF-channel number.

  \return  Number of start band
*/
static uint8_t getStopBand(
    uint32_t fs,              /*!< Output sampling frequency */
    uint8_t stopFreq,       /*!< Index to table of possible start bands */
    uint32_t headerDataFlags, /*!< Info to SBR mode */
    uint8_t k0)             /*!< Start freq index */
{
  uint8_t k2;

  if (stopFreq < 14) {
    int32_t stopMin;
    int32_t num = 2 * (64);
    uint8_t diff_tot[MAX_OCTAVE + MAX_SECOND_REGION];
    uint8_t *diff0 = diff_tot;
    uint8_t *diff1 = diff_tot + MAX_OCTAVE;

    if (headerDataFlags & SBRDEC_QUAD_RATE) {
      num >>= 1;
    }

    if (fs < 32000) {
      stopMin = (((2 * 6000 * num) / fs) + 1) >> 1;
    } else {
      if (fs < 64000) {
        stopMin = (((2 * 8000 * num) / fs) + 1) >> 1;
      } else {
        stopMin = (((2 * 10000 * num) / fs) + 1) >> 1;
      }
    }

    stopMin = fMin(stopMin, 64);

    /*
      Choose a stop band between k1 and 64 depending on stopFreq (0..13),
      based on a logarithmic scale.
      The vectors diff0 and diff1 are used temporarily here.
    */
    CalcBands(diff0, stopMin, 64, 13);
    shellsort(diff0, 13);
    cumSum(stopMin, diff0, 13, diff1);
    k2 = diff1[stopFreq];
  } else if (stopFreq == 14)
    k2 = 2 * k0;
  else
    k2 = 3 * k0;

  /* Limit to Nyquist */
  if (k2 > (64)) k2 = (64);

  /* Range checks */
  /* 1 <= difference <= 48; 1 <= fs <= 96000 */
  {
    uint8_t max_freq_coeffs = (headerDataFlags & SBRDEC_QUAD_RATE)
                                ? MAX_FREQ_COEFFS_QUAD_RATE
                                : MAX_FREQ_COEFFS;
    if (((k2 - k0) > max_freq_coeffs) || (k2 <= k0)) {
      return 255;
    }
  }

  if (headerDataFlags & SBRDEC_QUAD_RATE) {
    return k2; /* skip other checks: (k2 - k0) must be <=
                  MAX_FREQ_COEFFS_QUAD_RATE for all fs */
  }
  if (headerDataFlags & (SBRDEC_SYNTAX_USAC | SBRDEC_SYNTAX_RSVD50)) {
    /* 1 <= difference <= 35; 42000 <= fs <= 96000 */
    if ((fs >= 42000) && ((k2 - k0) > MAX_FREQ_COEFFS_FS44100)) {
      return 255;
    }
    /* 1 <= difference <= 32; 46009 <= fs <= 96000 */
    if ((fs >= 46009) && ((k2 - k0) > MAX_FREQ_COEFFS_FS48000)) {
      return 255;
    }
  } else {
    /* 1 <= difference <= 35; fs == 44100 */
    if ((fs == 44100) && ((k2 - k0) > MAX_FREQ_COEFFS_FS44100)) {
      return 255;
    }
    /* 1 <= difference <= 32; 48000 <= fs <= 96000 */
    if ((fs >= 48000) && ((k2 - k0) > MAX_FREQ_COEFFS_FS48000)) {
      return 255;
    }
  }

  return k2;
}

/*!
  \brief     Generates master frequency tables

  Frequency tables are calculated according to the selected domain
  (linear/logarithmic) and granularity.
  IEC 14496-3 4.6.18.3.2.1

  \return  errorCode, 0 if successful
*/
SBR_ERROR
sbrdecUpdateFreqScale(
    uint8_t *v_k_master, /*!< Master table to be created */
    uint8_t *numMaster,  /*!< Number of entries in master table */
    uint32_t fs,           /*!< SBR working sampling rate */
    HANDLE_SBR_HEADER_DATA hHeaderData, /*!< Control data from bitstream */
    uint32_t flags) {
  int16_t bpo_div16; /* bands_per_octave divided by 16 */
  int32_t dk = 0;

  /* Internal variables */
  uint8_t k0, k2, i;
  uint8_t num_bands0 = 0;
  uint8_t num_bands1 = 0;
  uint8_t diff_tot[MAX_OCTAVE + MAX_SECOND_REGION];
  uint8_t *diff0 = diff_tot;
  uint8_t *diff1 = diff_tot + MAX_OCTAVE;
  int32_t k2_achived;
  int32_t k2_diff;
  int32_t incr = 0;

  /*
    Determine start band
  */
  if (flags & SBRDEC_QUAD_RATE) {
    fs >>= 1;
  }

  k0 = getStartBand(fs, hHeaderData->bs_data.startFreq, flags);
  if (k0 == 255) {
    return SBRDEC_UNSUPPORTED_CONFIG;
  }

  /*
    Determine stop band
  */
  k2 = getStopBand(fs, hHeaderData->bs_data.stopFreq, flags, k0);
  if (k2 == 255) {
    return SBRDEC_UNSUPPORTED_CONFIG;
  }

  if (hHeaderData->bs_data.freqScale > 0) { /* Bark */
    int32_t k1;

    if (hHeaderData->bs_data.freqScale == 1) {
      bpo_div16 = 24576;
    } else if (hHeaderData->bs_data.freqScale == 2) {
      bpo_div16 = 20480;
    } else {
      bpo_div16 = 16384;
    }

    /* Ref: ISO/IEC 23003-3, Figure 12 - Flowchart calculation of fMaster for
     * 4:1 system when bs_freq_scale > 0 */
    if (flags & SBRDEC_QUAD_RATE) {
      if ((int16_t)k0 < (int16_t)(bpo_div16 >> ((FRACT_BITS - 1) - 4))) {
        bpo_div16 = (int16_t)(k0 & (uint8_t)0xfe)
                    << ((FRACT_BITS - 1) - 4); /* bpo_div16 = floor(k0/2)*2 */
      }
    }

    if (1000 * k2 > 2245 * k0) { /* Two or more regions */
      k1 = 2 * k0;

      num_bands0 = numberOfBands(bpo_div16, k0, k1, 0);
      num_bands1 =
          numberOfBands(bpo_div16, k1, k2, hHeaderData->bs_data.alterScale);
      if (num_bands0 < 1) {
        return SBRDEC_UNSUPPORTED_CONFIG;
      }
      if (num_bands1 < 1) {
        return SBRDEC_UNSUPPORTED_CONFIG;
      }

      CalcBands(diff0, k0, k1, num_bands0);
      shellsort(diff0, num_bands0);
      if (diff0[0] == 0) {
        return SBRDEC_UNSUPPORTED_CONFIG;
      }

      cumSum(k0, diff0, num_bands0, v_k_master);

      CalcBands(diff1, k1, k2, num_bands1);
      shellsort(diff1, num_bands1);
      if (diff0[num_bands0 - 1] > diff1[0]) {
        SBR_ERROR err;

        err = modifyBands(diff0[num_bands0 - 1], diff1, num_bands1);
        if (err) return SBRDEC_UNSUPPORTED_CONFIG;
      }

      /* Add 2nd region */
      cumSum(k1, diff1, num_bands1, &v_k_master[num_bands0]);
      *numMaster = num_bands0 + num_bands1; /* Output nr of bands */

    } else { /* Only one region */
      k1 = k2;

      num_bands0 = numberOfBands(bpo_div16, k0, k1, 0);
      if (num_bands0 < 1) {
        return SBRDEC_UNSUPPORTED_CONFIG;
      }
      CalcBands(diff0, k0, k1, num_bands0);
      shellsort(diff0, num_bands0);
      if (diff0[0] == 0) {
        return SBRDEC_UNSUPPORTED_CONFIG;
      }

      cumSum(k0, diff0, num_bands0, v_k_master);
      *numMaster = num_bands0; /* Output nr of bands */
    }
  } else { /* Linear mode */
    if (hHeaderData->bs_data.alterScale == 0) {
      dk = 1;
      /* FLOOR to get to few number of bands (next lower even number) */
      num_bands0 = (k2 - k0) & 254;
    } else {
      dk = 2;
      num_bands0 = (((k2 - k0) >> 1) + 1) & 254; /* ROUND to the closest fit */
    }

    if (num_bands0 < 1) {
      return SBRDEC_UNSUPPORTED_CONFIG;
      /* We must return already here because 'i' can become negative below. */
    }

    k2_achived = k0 + num_bands0 * dk;
    k2_diff = k2 - k2_achived;

    for (i = 0; i < num_bands0; i++) diff_tot[i] = dk;

    /* If linear scale wasn't achieved */
    /* and we got too wide SBR area */
    if (k2_diff < 0) {
      incr = 1;
      i = 0;
    }

    /* If linear scale wasn't achieved */
    /* and we got too small SBR area */
    if (k2_diff > 0) {
      incr = -1;
      i = num_bands0 - 1;
    }

    /* Adjust diff vector to get sepc. SBR range */
    while (k2_diff != 0) {
      diff_tot[i] = diff_tot[i] - incr;
      i = i + incr;
      k2_diff = k2_diff + incr;
    }

    cumSum(k0, diff_tot, num_bands0, v_k_master); /* cumsum */
    *numMaster = num_bands0;                      /* Output nr of bands */
  }

  if (*numMaster < 1) {
    return SBRDEC_UNSUPPORTED_CONFIG;
  }

  /* Ref: ISO/IEC 23003-3 Cor.3, "In 7.5.5.2, add to the requirements:"*/
  if (flags & SBRDEC_QUAD_RATE) {
    int32_t k;
    for (k = 1; k < *numMaster; k++) {
      if (!(v_k_master[k] - v_k_master[k - 1] <= k0 - 2)) {
        return SBRDEC_UNSUPPORTED_CONFIG;
      }
    }
  }

  /*
    Print out the calculated table
  */

  return SBRDEC_OK;
}

/*!
  \brief     Calculate frequency ratio of one SBR band

  All SBR bands should span a constant frequency range in the logarithmic
  domain. This function calculates the ratio of any SBR band's upper and lower
  frequency.

 \return    num_band-th root of k_start/k_stop
*/
static int16_t calcFactorPerBand(int32_t k_start, int32_t k_stop, int32_t num_bands) {
  /* Scaled bandfactor and step 1 bit right to avoid overflow
   * use double data type */
  int32_t bandfactor = FL2FXCONST_DBL(0.25f); /* Start value */
  int32_t step = FL2FXCONST_DBL(0.125f); /* Initial increment for factor */

  int32_t direction = 1;

  /* Because saturation can't be done in int32_t IIS,
   * changed start and stop data type from int16_t to int32_t */
  int32_t start = k_start << (DFRACT_BITS - 8);
  int32_t stop = k_stop << (DFRACT_BITS - 8);

  int32_t temp;

  int32_t j, i = 0;

  while (step > FL2FXCONST_DBL(0.0f)) {
    i++;
    temp = stop;

    /* Calculate temp^num_bands: */
    for (j = 0; j < num_bands; j++)
      // temp = fMult(temp,bandfactor);
      temp = fMultDiv2(temp, bandfactor) << 2;

    if (temp < start) { /* Factor too strong, make it weaker */
      if (direction == 0)
        /* Halfen step. Right shift is not done as fract because otherwise the
           lowest bit cannot be cleared due to rounding */
        step = (int32_t)((int32_t)step >> 1);
      direction = 1;
      bandfactor = bandfactor + step;
    } else { /* Factor is too weak: make it stronger */
      if (direction == 1) step = (int32_t)((int32_t)step >> 1);
      direction = 0;
      bandfactor = bandfactor - step;
    }

    if (i > 100) {
      step = FL2FXCONST_DBL(0.0f);
    }
  }
  return (bandfactor >= FL2FXCONST_DBL(0.5)) ? (int16_t)MAXVAL_SGL
                                             : FX_DBL2FX_SGL(bandfactor << 1);
}

/*!
  \brief     Calculate number of SBR bands between start and stop band

  Given the number of bands per octave, this function calculates how many
  bands fit in the given frequency range.
  When the warpFlag is set, the 'band density' is decreased by a factor
  of 1/1.3

  \return    number of bands
*/
static int32_t numberOfBands(
    int16_t bpo_div16, /*!< Input: number of bands per octave divided by 16 */
    int32_t start,          /*!< First QMF band of SBR frequency range */
    int32_t stop,           /*!< Last QMF band of SBR frequency range + 1 */
    int32_t warpFlag)       /*!< Stretching flag */
{
  int16_t num_bands_div128;
  int32_t num_bands;

  num_bands_div128 =
      FX_DBL2FX_SGL(fMult(FDK_getNumOctavesDiv8(start, stop), bpo_div16));

  if (warpFlag) {
    /* Apply the warp factor of 1.3 to get wider bands.  We use a value
       of 32768/25200 instead of the exact value to avoid critical cases
       of rounding.
    */
    num_bands_div128 = FX_DBL2FX_SGL(
        fMult(num_bands_div128, 25200));
  }

  /* add scaled 1 for rounding to even numbers: */
  num_bands_div128 = num_bands_div128 + 256;
  /* scale back to right aligned integer and double the value: */
  num_bands = 2 * ((int32_t)num_bands_div128 >> (FRACT_BITS - 7));

  return (num_bands);
}

/*!
  \brief     Calculate width of SBR bands

  Given the desired number of bands within the SBR frequency range,
  this function calculates the width of each SBR band in QMF channels.
  The bands get wider from start to stop (bark scale).
*/
static void CalcBands(uint8_t *diff,     /*!< Vector of widths to be calculated */
                      uint8_t start,     /*!< Lower end of subband range */
                      uint8_t stop,      /*!< Upper end of subband range */
                      uint8_t num_bands) /*!< Desired number of bands */
{
  int32_t i;
  int32_t previous;
  int32_t current;
  int16_t exact, temp;
  int16_t bandfactor = calcFactorPerBand(start, stop, num_bands);

  previous = stop; /* Start with highest QMF channel */
  exact = (int16_t)(
      stop << (FRACT_BITS - 8)); /* Shift left to gain some accuracy */

  for (i = num_bands - 1; i >= 0; i--) {
    /* Calculate border of next lower sbr band */
    exact = FX_DBL2FX_SGL(fMult(exact, bandfactor));

    /* Add scaled 0.5 for rounding:
       We use a value 128/256 instead of 0.5 to avoid some critical cases of
       rounding. */
    temp = exact + 128;

    /* scale back to right alinged integer: */
    current = (int32_t)temp >> (FRACT_BITS - 8);

    /* Save width of band i */
    diff[i] = previous - current;
    previous = current;
  }
}

/*!
  \brief     Calculate cumulated sum vector from delta vector
*/
static void cumSum(uint8_t start_value, uint8_t *diff, uint8_t length,
                   uint8_t *start_adress) {
  int32_t i;
  start_adress[0] = start_value;
  for (i = 1; i <= length; i++)
    start_adress[i] = start_adress[i - 1] + diff[i - 1];
}

/*!
  \brief     Adapt width of frequency bands in the second region

  If SBR spans more than 2 octaves, the upper part of a bark-frequency-scale
  is calculated separately. This function tries to avoid that the second region
  starts with a band smaller than the highest band of the first region.
*/
static SBR_ERROR modifyBands(uint8_t max_band_previous, uint8_t *diff,
                             uint8_t length) {
  int32_t change = max_band_previous - diff[0];

  /* Limit the change so that the last band cannot get narrower than the first
   * one */
  if (change > (diff[length - 1] - diff[0]) >> 1)
    change = (diff[length - 1] - diff[0]) >> 1;

  diff[0] += change;
  diff[length - 1] -= change;
  shellsort(diff, length);

  return SBRDEC_OK;
}

/*!
  \brief   Update high resolution frequency band table
*/
static void sbrdecUpdateHiRes(uint8_t *h_hires, uint8_t *num_hires,
                              uint8_t *v_k_master, uint8_t num_bands,
                              uint8_t xover_band) {
  uint8_t i;

  *num_hires = num_bands - xover_band;

  for (i = xover_band; i <= num_bands; i++) {
    h_hires[i - xover_band] = v_k_master[i];
  }
}

/*!
  \brief  Build low resolution table out of high resolution table
*/
static void sbrdecUpdateLoRes(uint8_t *h_lores, uint8_t *num_lores, uint8_t *h_hires,
                              uint8_t num_hires) {
  uint8_t i;

  if ((num_hires & 1) == 0) {
    /* If even number of hires bands */
    *num_lores = num_hires >> 1;
    /* Use every second lores=hires[0,2,4...] */
    for (i = 0; i <= *num_lores; i++) h_lores[i] = h_hires[i * 2];
  } else {
    /* Odd number of hires, which means xover is odd */
    *num_lores = (num_hires + 1) >> 1;
    /* Use lores=hires[0,1,3,5 ...] */
    h_lores[0] = h_hires[0];
    for (i = 1; i <= *num_lores; i++) {
      h_lores[i] = h_hires[i * 2 - 1];
    }
  }
}

/*!
  \brief   Derive a low-resolution frequency-table from the master frequency
  table
*/
void sbrdecDownSampleLoRes(uint8_t *v_result, uint8_t num_result,
                           uint8_t *freqBandTableRef, uint8_t num_Ref) {
  int32_t step;
  int32_t i, j;
  int32_t org_length, result_length;
  int32_t v_index[MAX_FREQ_COEFFS >> 1];

  /* init */
  org_length = num_Ref;
  result_length = num_result;

  v_index[0] = 0; /* Always use left border */
  i = 0;
  while (org_length > 0) {
    /* Create downsample vector */
    i++;
    step = org_length / result_length;
    org_length = org_length - step;
    result_length--;
    v_index[i] = v_index[i - 1] + step;
  }

  for (j = 0; j <= i; j++) {
    /* Use downsample vector to index LoResolution vector */
    v_result[j] = freqBandTableRef[v_index[j]];
  }
}

/*!
  \brief   Sorting routine
*/
void shellsort(uint8_t *in, uint8_t n) {
  int32_t i, j, v, w;
  int32_t inc = 1;

  do
    inc = 3 * inc + 1;
  while (inc <= n);

  do {
    inc = inc / 3;
    for (i = inc; i < n; i++) {
      v = in[i];
      j = i;
      while ((w = in[j - inc]) > v) {
        in[j] = w;
        j -= inc;
        if (j < inc) break;
      }
      in[j] = v;
    }
  } while (inc > 1);
}

/*!
  \brief   Reset frequency band tables
  \return  errorCode, 0 if successful
*/
SBR_ERROR
resetFreqBandTables(HANDLE_SBR_HEADER_DATA hHeaderData, const uint32_t flags) {
  SBR_ERROR err = SBRDEC_OK;
  int32_t k2, kx, lsb, usb;
  int32_t intTemp;
  uint8_t nBandsLo, nBandsHi;
  HANDLE_FREQ_BAND_DATA hFreq = &hHeaderData->freqBandData;

  /* Calculate master frequency function */
  err = sbrdecUpdateFreqScale(hFreq->v_k_master, &hFreq->numMaster,
                              hHeaderData->sbrProcSmplRate, hHeaderData, flags);

  if (err || (hHeaderData->bs_info.xover_band > hFreq->numMaster)) {
    return SBRDEC_UNSUPPORTED_CONFIG;
  }

  /* Derive Hiresolution from master frequency function */
  sbrdecUpdateHiRes(hFreq->freqBandTable[1], &nBandsHi, hFreq->v_k_master,
                    hFreq->numMaster, hHeaderData->bs_info.xover_band);
  /* Derive Loresolution from Hiresolution */
  sbrdecUpdateLoRes(hFreq->freqBandTable[0], &nBandsLo, hFreq->freqBandTable[1],
                    nBandsHi);

  hFreq->nSfb[0] = nBandsLo;
  hFreq->nSfb[1] = nBandsHi;

  /* Check index to freqBandTable[0] */
  if (!(nBandsLo > 0) ||
      (nBandsLo > (((hHeaderData->numberOfAnalysisBands == 16)
                        ? MAX_FREQ_COEFFS_QUAD_RATE
                        : MAX_FREQ_COEFFS_DUAL_RATE) >>
                   1))) {
    return SBRDEC_UNSUPPORTED_CONFIG;
  }

  lsb = hFreq->freqBandTable[0][0];
  usb = hFreq->freqBandTable[0][nBandsLo];

  /* Check for start frequency border k_x:
     - ISO/IEC 14496-3 4.6.18.3.6 Requirements
     - ISO/IEC 23003-3 7.5.5.2    Modifications and additions to the MPEG-4 SBR
     tool
  */
  /* Note that lsb > as hHeaderData->numberOfAnalysisBands is a valid SBR config
   * for 24 band QMF analysis. */
  if ((lsb > ((flags & SBRDEC_QUAD_RATE) ? 16 : (32))) || (lsb >= usb)) {
    return SBRDEC_UNSUPPORTED_CONFIG;
  }

  /* Calculate number of noise bands */

  k2 = hFreq->freqBandTable[1][nBandsHi];
  kx = hFreq->freqBandTable[1][0];

  if (hHeaderData->bs_data.noise_bands == 0) {
    hFreq->nNfb = 1;
  } else /* Calculate no of noise bands 1,2 or 3 bands/octave */
  {
    /* Fetch number of octaves divided by 32 */
    intTemp = (int32_t)FDK_getNumOctavesDiv8(kx, k2) >> 2;

    /* Integer-Multiplication with number of bands: */
    intTemp = intTemp * hHeaderData->bs_data.noise_bands;

    /* Add scaled 0.5 for rounding: */
    intTemp = intTemp + (int32_t)512;

    /* Convert to right-aligned integer: */
    intTemp = intTemp >> (FRACT_BITS - 1 /*sign*/ - 5 /* rescale */);

    if (intTemp == 0) intTemp = 1;

    hFreq->nNfb = intTemp;
  }

  hFreq->nInvfBands = hFreq->nNfb;

  if (hFreq->nNfb > MAX_NOISE_COEFFS) {
    return SBRDEC_UNSUPPORTED_CONFIG;
  }

  /* Get noise bands */
  sbrdecDownSampleLoRes(hFreq->freqBandTableNoise, hFreq->nNfb,
                        hFreq->freqBandTable[0], nBandsLo);

  /* save old highband; required for overlap in usac
     when headerchange occurs at XVAR and VARX frame; */
  hFreq->ov_highSubband = hFreq->highSubband;

  hFreq->lowSubband = lsb;
  hFreq->highSubband = usb;

  return SBRDEC_OK;
}
