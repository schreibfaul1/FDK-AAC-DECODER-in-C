

/*********************** MPEG surround decoder library *************************

   Author(s):   Matthias Hildenbrand

   Description: USAC MPS212 Transient Steering Decorrelator (TSD)

*******************************************************************************/
#include "../libAACdec/newAACDecoder.h"
#include "sac_tsd.h"

#define TSD_START_BAND (7)
#define SIZE_S (4)
#define SIZE_C (5)

/*** Tables ***/


static const uint8_t nBitsTsdCW_32slots[32] = {
    5,  9,  13, 16, 18, 20, 22, 24, 25, 26, 27, 28, 29, 29, 30, 30,
    30, 29, 29, 28, 27, 26, 25, 24, 22, 20, 18, 16, 13, 9,  5,  0};



static const uint8_t nBitsTsdCW_64slots[64] = {
    6,  11, 16, 20, 23, 27, 30, 33, 35, 38, 40, 42, 44, 46, 48, 49,
    51, 52, 53, 55, 56, 57, 58, 58, 59, 60, 60, 60, 61, 61, 61, 61,
    61, 61, 61, 60, 60, 60, 59, 58, 58, 57, 56, 55, 53, 52, 51, 49,
    48, 46, 44, 42, 40, 38, 35, 33, 30, 27, 23, 20, 16, 11, 6,  0};



static const FIXP_DPK_t phiTsd[8] = {
    {{(int32_t)0x7fffffff, (int32_t)0x00000000}},
    {{(int32_t)0x5a82799a, (int32_t)0x5a82799a}},
    {{(int32_t)0x00000000, (int32_t)0x7fffffff}},
    {{(int32_t)0xa57d8666, (int32_t)0x5a82799a}},
    {{(int32_t)0x80000000, (int32_t)0x00000000}},
    {{(int32_t)0xa57d8666, (int32_t)0xa57d8666}},
    {{(int32_t)0x00000000, (int32_t)0x80000000}},
    {{(int32_t)0x5a82799a, (int32_t)0xa57d8666}}};

/*** Static Functions ***/
static void longmult1(uint16_t a[], uint16_t b, uint16_t d[], int32_t len) {
  int32_t k;
  uint32_t tmp;
  uint32_t b0 = (uint32_t)b;

  tmp = ((uint32_t)a[0]) * b0;
  d[0] = (uint16_t)tmp;

  for (k = 1; k < len; k++) {
    tmp = (tmp >> 16) + ((uint32_t)a[k]) * b0;
    d[k] = (uint16_t)tmp;
  }
}

static void longdiv(uint16_t b[], uint16_t a, uint16_t d[], uint16_t *pr, int32_t len) {
  uint32_t r;
  uint32_t tmp;
  int32_t k;

  assert(a != 0);

  r = 0;

  for (k = len - 1; k >= 0; k--) {
    tmp = ((uint32_t)b[k]) + (r << 16);

    if (tmp) {
      d[k] = (uint16_t)(tmp / a);
      r = tmp - d[k] * a;
    } else {
      d[k] = 0;
    }
  }
  *pr = (uint16_t)r;
}

static void longsub(uint16_t a[], uint16_t b[], int32_t lena, int32_t lenb) {
  int32_t h;
  int32_t carry = 0;

  assert(lena >= lenb);
  for (h = 0; h < lenb; h++) {
    carry += ((int32_t)a[h]) - ((int32_t)b[h]);
    a[h] = (uint16_t)carry;
    carry = carry >> 16;
  }

  for (; h < lena; h++) {
    carry = ((int32_t)a[h]) + carry;
    a[h] = (uint16_t)carry;
    carry = carry >> 16;
  }

  assert(carry ==
             0); /* carry != 0 indicates subtraction underflow, e.g. b > a */
  return;
}

static int32_t longcompare(uint16_t a[], uint16_t b[], int32_t len) {
  int32_t i;

  for (i = len - 1; i > 0; i--) {
    if (a[i] != b[i]) break;
  }
  return (a[i] >= b[i]) ? 1 : 0;
}

static inline int32_t isTrSlot(const TSD_DATA *pTsdData, const int32_t ts) {
  return (pTsdData->bsTsdTrPhaseData[ts] >= 0);
}

/*** Public Functions ***/
int32_t TsdRead(HANDLE_FDK_BITSTREAM hBs, const int32_t numSlots, TSD_DATA *pTsdData) {
  int32_t nBitsTrSlots = 0;
  int32_t bsTsdNumTrSlots;
  const uint8_t *nBitsTsdCW_tab = NULL;

  switch (numSlots) {
    case 32:
      nBitsTrSlots = 4;
      nBitsTsdCW_tab = nBitsTsdCW_32slots;
      break;
    case 64:
      nBitsTrSlots = 5;
      nBitsTsdCW_tab = nBitsTsdCW_64slots;
      break;
    default:
      return 1;
  }

  /*** Read TempShapeData for bsTempShapeConfig == 3 ***/
  pTsdData->bsTsdEnable = FDKreadBit(hBs);
  if (!pTsdData->bsTsdEnable) {
    return 0;
  }

  /*** Parse/Decode TsdData() ***/
  pTsdData->numSlots = numSlots;

  bsTsdNumTrSlots = FDKreadBits(hBs, nBitsTrSlots);

  /* Decode transient slot positions */
  {
    int32_t nBitsTsdCW = (int32_t)nBitsTsdCW_tab[bsTsdNumTrSlots];
    int8_t *phaseData = pTsdData->bsTsdTrPhaseData;
    int32_t p = bsTsdNumTrSlots + 1;
    int32_t k, h;
    uint16_t s[SIZE_S] = {0};
    uint16_t c[SIZE_C] = {0};
    uint16_t r[1];

    /* Init with TsdSepData[k] = 0 */
    for (k = 0; k < numSlots; k++) {
      phaseData[k] = -1; /* means TsdSepData[] = 0 */
    }

    for (h = (SIZE_S - 1); h >= 0; h--) {
      if (nBitsTsdCW > h * 16) {
        s[h] = (uint16_t)FDKreadBits(hBs, nBitsTsdCW - h * 16);
        nBitsTsdCW = h * 16;
      }
    }

    /* c = prod_{h=1}^{p} (k-p+h)/h */
    k = numSlots - 1;
    c[0] = k - p + 1;
    for (h = 2; h <= p; h++) {
      longmult1(c, (k - p + h), c, 5); /* c *= k - p + h; */
      longdiv(c, h, c, r, 5);          /* c /= h; */
      assert(*r == 0);
    }

    /* go through all slots */
    for (; k >= 0; k--) {
      if (p > k) {
        for (; k >= 0; k--) {
          phaseData[k] = 1; /* means TsdSepData[] = 1 */
        }
        break;
      }
      if (longcompare(s, c, 4)) { /* (s >= c) */
        longsub(s, c, 4, 4);      /* s -= c; */
        phaseData[k] = 1;         /* means TsdSepData[] = 1 */
        if (p == 1) {
          break;
        }
        /* Update c for next iteration: c_new = c_old * p / k */
        longmult1(c, p, c, 5);
        p--;
      } else {
        /* Update c for next iteration: c_new = c_old * (k-p) / k */
        longmult1(c, (k - p), c, 5);
      }
      longdiv(c, k, c, r, 5);
      assert(*r == 0);
    }

    /* Read phase data */
    for (k = 0; k < numSlots; k++) {
      if (phaseData[k] == 1) {
        phaseData[k] = FDKreadBits(hBs, 3);
      }
    }
  }

  return 0;
}

void TsdGenerateNonTr(const int32_t numHybridBands, const TSD_DATA *pTsdData,
                      const int32_t ts, int32_t *pVdirectReal,
                      int32_t *pVdirectImag, int32_t *pVnonTrReal,
                      int32_t *pVnonTrImag, int32_t **ppDecorrInReal,
                      int32_t **ppDecorrInImag) {
  int32_t k = 0;

  if (!isTrSlot(pTsdData, ts)) {
    /* Let allpass based decorrelator read from direct input. */
    *ppDecorrInReal = pVdirectReal;
    *ppDecorrInImag = pVdirectImag;
    return;
  }

  /* Generate nonTr input signal for allpass based decorrelator */
  for (; k < TSD_START_BAND; k++) {
    pVnonTrReal[k] = pVdirectReal[k];
    pVnonTrImag[k] = pVdirectImag[k];
  }
  for (; k < numHybridBands; k++) {
    pVnonTrReal[k] = (int32_t)0;
    pVnonTrImag[k] = (int32_t)0;
  }
  *ppDecorrInReal = pVnonTrReal;
  *ppDecorrInImag = pVnonTrImag;
}

void TsdApply(const int32_t numHybridBands, const TSD_DATA *pTsdData, int32_t *pTsdTs,
              const int32_t *pVdirectReal, const int32_t *pVdirectImag,
              int32_t *pDnonTrReal, int32_t *pDnonTrImag) {
  const int32_t ts = *pTsdTs;

  if (isTrSlot(pTsdData, ts)) {
    int32_t k;
    const FIXP_DPK_t *phi = &phiTsd[pTsdData->bsTsdTrPhaseData[ts]];
    assert((pTsdData->bsTsdTrPhaseData[ts] >= 0) &&
               (pTsdData->bsTsdTrPhaseData[ts] < 8));

    /* d = d_nonTr + v_direct * exp(j * bsTsdTrPhaseData[ts]/4 * pi ) */
    for (k = TSD_START_BAND; k < numHybridBands; k++) {
      int32_t tempReal, tempImag;
      cplxMultDiv2(&tempReal, &tempImag, pVdirectReal[k], pVdirectImag[k],
                   *phi);
      pDnonTrReal[k] = SATURATE_LEFT_SHIFT(
          (pDnonTrReal[k] >> 2) + (tempReal >> 1), 2, DFRACT_BITS);
      pDnonTrImag[k] = SATURATE_LEFT_SHIFT(
          (pDnonTrImag[k] >> 2) + (tempImag >> 1), 2, DFRACT_BITS);
    }
  }

  /* The modulo MAX_TSD_TIME_SLOTS operation is to avoid illegal memory accesses
   * in case of errors. */
  *pTsdTs = (ts + 1) & (MAX_TSD_TIME_SLOTS - 1);
  return;
}
