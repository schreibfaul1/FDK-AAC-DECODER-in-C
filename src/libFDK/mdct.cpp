

/******************* Library for basic calculation routines ********************

   Author(s):   Josef Hoepfl, Manuel Jander, Youliy Ninov, Daniel Hagel

   Description: MDCT/MDST routines

*******************************************************************************/

#include <memory.h>
#include <stdio.h>
#include "../libAACdec/newAACDecoder.h"
#include "mdct.h"

#include "FDK_tools_rom.h"
#include "dct.h"
#include "fixpoint_math.h"

void mdct_init(H_MDCT hMdct, int32_t *overlap, int32_t overlapBufferSize) {
  hMdct->overlap.freq = overlap;
  // memset(overlap, overlapBufferSize*sizeof(int32_t));
  hMdct->prev_fr = 0;
  hMdct->prev_nr = 0;
  hMdct->prev_tl = 0;
  hMdct->ov_size = overlapBufferSize;
  hMdct->prevAliasSymmetry = 0;
  hMdct->prevPrevAliasSymmetry = 0;
  hMdct->pFacZir = NULL;
  hMdct->pAsymOvlp = NULL;
}

/*
This program implements the forward MDCT transform on an input block of data.
The input block is in a form (A,B,C,D) where A,B,C and D are the respective
1/4th segments of the block. The program takes the input block and folds it in
the form:
(-D-Cr,A-Br). This block is twice shorter and here the 'r' suffix denotes
flipping of the sequence (reversing the order of the samples). While folding the
input block in the above mentioned shorter block the program windows the data.
Because the two operations (windowing and folding) are not implemented
sequentially, but together the program's structure is not easy to understand.
Once the output (already windowed) block (-D-Cr,A-Br) is ready it is passed to
the DCT IV for processing.
*/
int32_t mdct_block(H_MDCT hMdct, const int16_t * timeData,
               const int32_t noInSamples, int32_t * mdctData,
               const int32_t nSpec, const int32_t tl, const FIXP_SPK_t *pRightWindowPart,
               const int32_t fr, int16_t *pMdctData_e) {
  int32_t i, n;
  /* tl: transform length
     fl: left window slope length
     nl: left window slope offset
     fr: right window slope length
     nr: right window slope offset
     See FDK_tools/doc/intern/mdct.tex for more detail. */
  int32_t fl, nl, nr;
  const FIXP_SPK_t *wls, *wrs;

  wrs = pRightWindowPart;

  /* Detect FRprevious / FL mismatches and override parameters accordingly */
  if (hMdct->prev_fr ==
      0) { /* At start just initialize and pass parameters as they are */
    hMdct->prev_fr = fr;
    hMdct->prev_wrs = wrs;
    hMdct->prev_tl = tl;
  }

  /* Derive NR */
  nr = (tl - fr) >> 1;

  /* Skip input samples if tl is smaller than block size */
  timeData += (noInSamples - tl) >> 1;

  /* windowing */
  for (n = 0; n < nSpec; n++) {
    /*
     * MDCT scale:
     * + 1: fMultDiv2() in windowing.
     * + 1: Because of factor 1/2 in Princen-Bradley compliant windowed TDAC.
     */
    int32_t mdctData_e = 1 + 1;

    /* Derive left parameters */
    wls = hMdct->prev_wrs;
    fl = hMdct->prev_fr;
    nl = (tl - fl) >> 1;

    /* Here we implement a simplified version of what happens after the this
    piece of code (see the comments below). We implement the folding of A and B
    segments to (A-Br) but A is zero, because in this part of the MDCT sequence
    the window coefficients with which A must be multiplied are zero.    */
    for (i = 0; i < nl; i++) {
#if SAMPLE_BITS == DFRACT_BITS /* SPC_BITS and DFRACT_BITS should be equal. */
      mdctData[(tl / 2) + i] = -((int32_t)timeData[tl - i - 1] >> (1));
#else
      mdctData[(tl / 2) + i] = -(int32_t)timeData[tl - i - 1]
                               << (DFRACT_BITS - SAMPLE_BITS - 1); /* 0(A)-Br */
#endif
    }

    /* Implements the folding and windowing of the left part of the sequence,
    that is segments A and B. The A segment is multiplied by the respective left
    window coefficient and placed in a temporary variable.

    tmp0 = fMultDiv2((int16_t)timeData[i+nl], pLeftWindowPart[i].v.im);

    After this the B segment taken in reverse order is multiplied by the left
    window and subtracted from the previously derived temporary variable, so
    that finally we implement the A-Br operation. This output is written to the
    right part of the MDCT output : (-D-Cr,A-Br).

    mdctData[(tl/2)+i+nl] = fMultSubDiv2(tmp0, (int16_t)timeData[tl-nl-i-1],
    pLeftWindowPart[i].v.re);//A*window-Br*window

    The (A-Br) data is written to the output buffer (mdctData) without being
    flipped.     */
    for (i = 0; i < fl / 2; i++) {
      int32_t tmp0;
      tmp0 = fMultDiv2((int16_t)timeData[i + nl], wls[i].v.im); /* a*window */
      mdctData[(tl / 2) + i + nl] =
          fMultSubDiv2(tmp0, (int16_t)timeData[tl - nl - i - 1],
                       wls[i].v.re); /* A*window-Br*window */
    }

    /* Right window slope offset */
    /* Here we implement a simplified version of what happens after the this
    piece of code (see the comments below). We implement the folding of C and D
    segments to (-D-Cr) but D is zero, because in this part of the MDCT sequence
    the window coefficients with which D must be multiplied are zero.    */
    for (i = 0; i < nr; i++) {
#if SAMPLE_BITS == \
    DFRACT_BITS /* This should be SPC_BITS instead of DFRACT_BITS. */
      mdctData[(tl / 2) - 1 - i] = -((int32_t)timeData[tl + i] >> (1));
#else
      mdctData[(tl / 2) - 1 - i] =
          -(int32_t)timeData[tl + i]
          << (DFRACT_BITS - SAMPLE_BITS - 1); /* -C flipped at placing */
#endif
    }

    /* Implements the folding and windowing of the right part of the sequence,
    that is, segments C and D. The C segment is multiplied by the respective
    right window coefficient and placed in a temporary variable.

    tmp1 = fMultDiv2((int16_t)timeData[tl+nr+i], pRightWindowPart[i].v.re);

    After this the D segment taken in reverse order is multiplied by the right
    window and added from the previously derived temporary variable, so that we
    get (C+Dr) operation. This output is negated to get (-C-Dr) and written to
    the left part of the MDCT output while being reversed (flipped) at the same
    time, so that from (-C-Dr) we get (-D-Cr)=> (-D-Cr,A-Br).

    mdctData[(tl/2)-nr-i-1] = -fMultAddDiv2(tmp1,
    (int16_t)timeData[(tl*2)-nr-i-1], pRightWindowPart[i].v.im);*/
    for (i = 0; i < fr / 2; i++) {
      int32_t tmp1;
      tmp1 = fMultDiv2((int16_t)timeData[tl + nr + i],
                       wrs[i].v.re); /* C*window */
      mdctData[(tl / 2) - nr - i - 1] =
          -fMultAddDiv2(tmp1, (int16_t)timeData[(tl * 2) - nr - i - 1],
                        wrs[i].v.im); /* -(C*window+Dr*window) and flip before
                                         placing -> -Cr - D */
    }

    /* We pass the shortened folded data (-D-Cr,A-Br) to the MDCT function */
    dct_IV(mdctData, tl, &mdctData_e);

    pMdctData_e[n] = (int16_t)mdctData_e;

    timeData += tl;
    mdctData += tl;

    hMdct->prev_wrs = wrs;
    hMdct->prev_fr = fr;
    hMdct->prev_tl = tl;
  }

  return nSpec * tl;
}

void imdct_gain(int32_t *pGain_m, int32_t *pGain_e, int32_t tl) {
  int32_t gain_m = *pGain_m;
  int32_t gain_e = *pGain_e;
  int32_t log2_tl;

  gain_e += -MDCT_OUTPUT_GAIN - MDCT_OUT_HEADROOM + 1;
  if (tl == 0) {
    /* Dont regard the 2/N factor from the IDCT. It is compensated for somewhere
     * else. */
    *pGain_e = gain_e;
    return;
  }

  log2_tl = DFRACT_BITS - 1 - fNormz((int32_t)tl);
  gain_e += -log2_tl;

  /* Detect non-radix 2 transform length and add amplitude compensation factor
     which cannot be included into the exponent above */
  switch ((tl) >> (log2_tl - 2)) {
    case 0x7: /* 10 ms, 1/tl = 1.0/(FDKpow(2.0, -log2_tl) *
                 0.53333333333333333333) */
      if (gain_m == (int32_t)0) {
        gain_m = FL2FXCONST_DBL(0.53333333333333333333f);
      } else {
        gain_m = fMult(gain_m, FL2FXCONST_DBL(0.53333333333333333333f));
      }
      break;
    case 0x6: /* 3/4 of radix 2, 1/tl = 1.0/(FDKpow(2.0, -log2_tl) * 2.0/3.0) */
      if (gain_m == (int32_t)0) {
        gain_m = FL2FXCONST_DBL(2.0 / 3.0f);
      } else {
        gain_m = fMult(gain_m, FL2FXCONST_DBL(2.0 / 3.0f));
      }
      break;
    case 0x5: /* 0.8 of radix 2 (e.g. tl 160), 1/tl = 1.0/(FDKpow(2.0, -log2_tl)
               * 0.8/1.5) */
      if (gain_m == (int32_t)0) {
        gain_m = FL2FXCONST_DBL(0.53333333333333333333f);
      } else {
        gain_m = fMult(gain_m, FL2FXCONST_DBL(0.53333333333333333333f));
      }
      break;
    case 0x4:
      /* radix 2, nothing to do. */
      break;
    default:
      /* unsupported */
      assert(0);
      break;
  }

  *pGain_m = gain_m;
  *pGain_e = gain_e;
}

int32_t imdct_drain(H_MDCT hMdct, int32_t *output, int32_t nrSamplesRoom) {
  int32_t buffered_samples = 0;

  if (nrSamplesRoom > 0) {
    buffered_samples = hMdct->ov_offset;

    assert(buffered_samples <= nrSamplesRoom);

    if (buffered_samples > 0) {
      memcpy(output, hMdct->overlap.time,
                buffered_samples * sizeof(int32_t));
      hMdct->ov_offset = 0;
    }
  }
  return buffered_samples;
}

int32_t imdct_copy_ov_and_nr(H_MDCT hMdct, int32_t *pTimeData, int32_t nrSamples) {
  int32_t *pOvl;
  int32_t nt, nf, i;

  nt = fMin(hMdct->ov_offset, nrSamples);
  nrSamples -= nt;
  nf = fMin(hMdct->prev_nr, nrSamples);
  memcpy(pTimeData, hMdct->overlap.time, nt * sizeof(int32_t));
  pTimeData += nt;

  pOvl = hMdct->overlap.freq + hMdct->ov_size - 1;
  if (hMdct->prevPrevAliasSymmetry == 0) {
    for (i = 0; i < nf; i++) {
      int32_t x = -(*pOvl--);
      *pTimeData = IMDCT_SCALE_DBL(x);
      pTimeData++;
    }
  } else {
    for (i = 0; i < nf; i++) {
      int32_t x = (*pOvl--);
      *pTimeData = IMDCT_SCALE_DBL(x);
      pTimeData++;
    }
  }

  return (nt + nf);
}

void imdct_adapt_parameters(H_MDCT hMdct, int32_t *pfl, int32_t *pnl, int32_t tl,
                            const FIXP_SPK_t *wls, int32_t noOutSamples) {
  int32_t fl = *pfl, nl = *pnl;
  int32_t window_diff, use_current = 0, use_previous = 0;
  if (hMdct->prev_tl == 0) {
    hMdct->prev_wrs = wls;
    hMdct->prev_fr = fl;
    hMdct->prev_nr = (noOutSamples - fl) >> 1;
    hMdct->prev_tl = noOutSamples;
    hMdct->ov_offset = 0;
    use_current = 1;
  }

  window_diff = (hMdct->prev_fr - fl) >> 1;

  /* check if the previous window slope can be adjusted to match the current
   * window slope */
  if (hMdct->prev_nr + window_diff > 0) {
    use_current = 1;
  }
  /* check if the current window slope can be adjusted to match the previous
   * window slope */
  if (nl - window_diff > 0) {
    use_previous = 1;
  }

  /* if both is possible choose the larger of both window slope lengths */
  if (use_current && use_previous) {
    if (fl < hMdct->prev_fr) {
      use_current = 0;
    }
  }
  /*
   * If the previous transform block is big enough, enlarge previous window
   * overlap, if not, then shrink current window overlap.
   */
  if (use_current) {
    hMdct->prev_nr += window_diff;
    hMdct->prev_fr = fl;
    hMdct->prev_wrs = wls;
  } else {
    nl -= window_diff;
    fl = hMdct->prev_fr;
  }

  *pfl = fl;
  *pnl = nl;
}

/*
This program implements the inverse modulated lapped transform, a generalized
version of the inverse MDCT transform. Setting none of the MLT_*_ALIAS_FLAG
flags computes the IMDCT, setting all of them computes the IMDST. Other
combinations of these flags compute type III transforms used by the RSVD60
multichannel tool for transitions between MDCT/MDST. The following description
relates to the IMDCT only.

If we pass the data block (A,B,C,D,E,F) to the FORWARD MDCT it will produce two
outputs. The first one will be over the (A,B,C,D) part =>(-D-Cr,A-Br) and the
second one will be over the (C,D,E,F) part => (-F-Er,C-Dr), since there is a
overlap between consequtive passes of the algorithm. This overlap is over the
(C,D) segments. The two outputs will be given sequentially to the DCT IV
algorithm. At the INVERSE MDCT side we get two consecutive outputs from the IDCT
IV algorithm, namely the same blocks: (-D-Cr,A-Br) and (-F-Er,C-Dr). The first
of them lands in the Overlap buffer and the second is in the working one, which,
one algorithm pass later will substitute the one residing in the overlap
register. The IMDCT algorithm has to produce the C and D segments from the two
buffers. In order to do this we take the left part of the overlap
buffer(-D-Cr,A-Br), namely (-D-Cr) and add it appropriately to the right part of
the working buffer (-F-Er,C-Dr), namely (C-Dr), so that we get first the C
segment and later the D segment. We do this in the following way: From the right
part of the working buffer(C-Dr) we subtract the flipped left part of the
overlap buffer(-D-Cr):

Result = (C-Dr) - flipped(-D-Cr) = C -Dr + Dr + C = 2C
We divide by two and get the C segment. What we did is adding the right part of
the first frame to the left part of the second one.   While applying these
operation we multiply the respective segments with the appropriate window
functions.

In order to get the D segment we do the following:
From the negated second part of the working buffer(C-Dr) we subtract the flipped
first part of the overlap buffer (-D-Cr):

Result= - (C -Dr) - flipped(-D-Cr)= -C +Dr +Dr +C = 2Dr.
After dividing by two and flipping we get the D segment.What we did is adding
the right part of the first frame to the left part of the second one.   While
applying these operation we multiply the respective segments with the
appropriate window functions.

Once we have obtained the C and D segments the overlap buffer is emptied and the
current buffer is sent in it, so that the E and F segments are available for
decoding in the next algorithm pass.*/
int32_t imlt_block(H_MDCT hMdct, int32_t *output, int32_t *spectrum,
               const int16_t scalefactor[], const int32_t nSpec,
               const int32_t noOutSamples, const int32_t tl, const FIXP_SPK_t *wls,
               int32_t fl, const FIXP_SPK_t *wrs, const int32_t fr, int32_t gain,
               int32_t flags) {
  int32_t *pOvl;
  int32_t *pOut0 = output, *pOut1;
  int32_t nl, nr;
  int32_t w, i, nrSamples = 0, specShiftScale, transform_gain_e = 0;
  int32_t currAliasSymmetry = (flags & MLT_FLAG_CURR_ALIAS_SYMMETRY);

  /* Derive NR and NL */
  nr = (tl - fr) >> 1;
  nl = (tl - fl) >> 1;

  /* Include 2/N IMDCT gain into gain factor and exponent. */
  imdct_gain(&gain, &transform_gain_e, tl);

  /* Detect FRprevious / FL mismatches and override parameters accordingly */
  if (hMdct->prev_fr != fl) {
    imdct_adapt_parameters(hMdct, &fl, &nl, tl, wls, noOutSamples);
  }

  pOvl = hMdct->overlap.freq + hMdct->ov_size - 1;

  if (noOutSamples > nrSamples) {
    /* Purge buffered output. */
    for (i = 0; i < hMdct->ov_offset; i++) {
      *pOut0 = hMdct->overlap.time[i];
      pOut0++;
    }
    nrSamples = hMdct->ov_offset;
    hMdct->ov_offset = 0;
  }

  for (w = 0; w < nSpec; w++) {
    int32_t *pSpec, *pCurr;
    const FIXP_SPK_t *pWindow;

    /* Detect FRprevious / FL mismatches and override parameters accordingly */
    if (hMdct->prev_fr != fl) {
      imdct_adapt_parameters(hMdct, &fl, &nl, tl, wls, noOutSamples);
    }

    specShiftScale = transform_gain_e;

    /* Setup window pointers */
    pWindow = hMdct->prev_wrs;

    /* Current spectrum */
    pSpec = spectrum + w * tl;

    /* DCT IV of current spectrum. */
    if (currAliasSymmetry == 0) {
      if (hMdct->prevAliasSymmetry == 0) {
        dct_IV(pSpec, tl, &specShiftScale);
      } else {
        int32_t _tmp[1024 + ALIGNMENT_DEFAULT / sizeof(int32_t)];


        int32_t *tmp = (int32_t *)(_tmp + (((int32_t)8 - ((size_t)(_tmp) & 7)) & 7));
        dct_III(pSpec, tmp, tl, &specShiftScale);
      }
    } else {
      if (hMdct->prevAliasSymmetry == 0) {
        int32_t _tmp[1024 + ALIGNMENT_DEFAULT / sizeof(int32_t)];
        int32_t *tmp = (int32_t *) _tmp + (((int32_t)8 - ((size_t)(_tmp) & 7)) & 7);
        dst_III(pSpec, tmp, tl, &specShiftScale);
      } else {
        dst_IV(pSpec, tl, &specShiftScale);
      }
    }

    /* Optional scaling of time domain - no yet windowed - of current spectrum
     */
    /* and de-scale current spectrum signal (time domain, no yet windowed) */
    if (gain != (int32_t)0) {
      for (i = 0; i < tl; i++) {
        pSpec[i] = fMult(pSpec[i], gain);
      }
    }

    {
      int32_t loc_scale =
          fixmin_I(scalefactor[w] + specShiftScale, (int32_t)DFRACT_BITS - 1);
      scaleValuesSaturate(pSpec, tl, loc_scale);
    }

    if (noOutSamples <= nrSamples) {
      /* Divert output first half to overlap buffer if we already got enough
       * output samples. */
      pOut0 = hMdct->overlap.time + hMdct->ov_offset;
      hMdct->ov_offset += hMdct->prev_nr + fl / 2;
    } else {
      /* Account output samples */
      nrSamples += hMdct->prev_nr + fl / 2;
    }

    /* NR output samples 0 .. NR. -overlap[TL/2..TL/2-NR] */
    if ((hMdct->pFacZir != 0) && (hMdct->prev_nr == fl / 2)) {
      /* In the case of ACELP -> TCX20 -> FD int16_t add FAC ZIR on nr signal part
       */
      for (i = 0; i < hMdct->prev_nr; i++) {
        int32_t x = -(*pOvl--);
        *pOut0 = fAddSaturate(x, IMDCT_SCALE_DBL(hMdct->pFacZir[i]));
        pOut0++;
      }
      hMdct->pFacZir = NULL;
    } else {
      /* Here we implement a simplified version of what happens after the this
      piece of code (see the comments below). We implement the folding of C and
      D segments from (-D-Cr) but D is zero, because in this part of the MDCT
      sequence the window coefficients with which D must be multiplied are zero.
      "pOut0" writes sequentially the C block from left to right.   */
      if (hMdct->prevPrevAliasSymmetry == 0) {
        for (i = 0; i < hMdct->prev_nr; i++) {
          int32_t x = -(*pOvl--);
          *pOut0 = IMDCT_SCALE_DBL(x);
          pOut0++;
        }
      } else {
        for (i = 0; i < hMdct->prev_nr; i++) {
          int32_t x = *pOvl--;
          *pOut0 = IMDCT_SCALE_DBL(x);
          pOut0++;
        }
      }
    }

    if (noOutSamples <= nrSamples) {
      /* Divert output second half to overlap buffer if we already got enough
       * output samples. */
      pOut1 = hMdct->overlap.time + hMdct->ov_offset + fl / 2 - 1;
      hMdct->ov_offset += fl / 2 + nl;
    } else {
      pOut1 = pOut0 + (fl - 1);
      nrSamples += fl / 2 + nl;
    }

    /* output samples before window crossing point NR .. TL/2.
     * -overlap[TL/2-NR..TL/2-NR-FL/2] + current[NR..TL/2] */
    /* output samples after window crossing point TL/2 .. TL/2+FL/2.
     * -overlap[0..FL/2] - current[TL/2..FL/2] */
    pCurr = pSpec + tl - fl / 2;

    if (hMdct->prevPrevAliasSymmetry == 0) {
      if (hMdct->prevAliasSymmetry == 0) {
        if (!hMdct->pAsymOvlp) {
          for (i = 0; i < fl / 2; i++) {
            int32_t x0, x1;
            cplxMultDiv2(&x1, &x0, *pCurr++, -*pOvl--, pWindow[i]);
            *pOut0 = IMDCT_SCALE_DBL_LSH1(x0);
            *pOut1 = IMDCT_SCALE_DBL_LSH1(-x1);
            pOut0++;
            pOut1--;
          }
        } else {
          int32_t *pAsymOvl = hMdct->pAsymOvlp + fl / 2 - 1;
          for (i = 0; i < fl / 2; i++) {
            int32_t x0, x1;
            x1 = -fMultDiv2(*pCurr, pWindow[i].v.re) +
                 fMultDiv2(*pAsymOvl, pWindow[i].v.im);
            x0 = fMultDiv2(*pCurr, pWindow[i].v.im) -
                 fMultDiv2(*pOvl, pWindow[i].v.re);
            pCurr++;
            pOvl--;
            pAsymOvl--;
            *pOut0++ = IMDCT_SCALE_DBL_LSH1(x0);
            *pOut1-- = IMDCT_SCALE_DBL_LSH1(x1);
          }
          hMdct->pAsymOvlp = NULL;
        }
      } else { /* prevAliasingSymmetry == 1 */
        for (i = 0; i < fl / 2; i++) {
          int32_t x0, x1;
          cplxMultDiv2(&x1, &x0, *pCurr++, -*pOvl--, pWindow[i]);
          *pOut0 = IMDCT_SCALE_DBL_LSH1(x0);
          *pOut1 = IMDCT_SCALE_DBL_LSH1(x1);
          pOut0++;
          pOut1--;
        }
      }
    } else { /* prevPrevAliasingSymmetry == 1 */
      if (hMdct->prevAliasSymmetry == 0) {
        for (i = 0; i < fl / 2; i++) {
          int32_t x0, x1;
          cplxMultDiv2(&x1, &x0, *pCurr++, *pOvl--, pWindow[i]);
          *pOut0 = IMDCT_SCALE_DBL_LSH1(x0);
          *pOut1 = IMDCT_SCALE_DBL_LSH1(-x1);
          pOut0++;
          pOut1--;
        }
      } else { /* prevAliasingSymmetry == 1 */
        for (i = 0; i < fl / 2; i++) {
          int32_t x0, x1;
          cplxMultDiv2(&x1, &x0, *pCurr++, *pOvl--, pWindow[i]);
          *pOut0 = IMDCT_SCALE_DBL_LSH1(x0);
          *pOut1 = IMDCT_SCALE_DBL_LSH1(x1);
          pOut0++;
          pOut1--;
        }
      }
    }

    if (hMdct->pFacZir != 0) {
      /* add FAC ZIR of previous ACELP -> mdct transition */
      int32_t *pOut = pOut0 - fl / 2;
      assert(fl / 2 <= 128);
      for (i = 0; i < fl / 2; i++) {
        pOut[i] = fAddSaturate(pOut[i], IMDCT_SCALE_DBL(hMdct->pFacZir[i]));
      }
      hMdct->pFacZir = NULL;
    }
    pOut0 += (fl / 2) + nl;

    /* NL output samples TL/2+FL/2..TL. - current[FL/2..0] */
    pOut1 += (fl / 2) + 1;
    pCurr = pSpec + tl - fl / 2 - 1;
    /* Here we implement a simplified version of what happens above the this
    piece of code (see the comments above). We implement the folding of C and D
    segments from (C-Dr) but C is zero, because in this part of the MDCT
    sequence the window coefficients with which C must be multiplied are zero.
    "pOut1" writes sequentially the D block from left to right.   */
    if (hMdct->prevAliasSymmetry == 0) {
      for (i = 0; i < nl; i++) {
        int32_t x = -(*pCurr--);
        *pOut1++ = IMDCT_SCALE_DBL(x);
      }
    } else {
      for (i = 0; i < nl; i++) {
        int32_t x = *pCurr--;
        *pOut1++ = IMDCT_SCALE_DBL(x);
      }
    }

    /* Set overlap source pointer for next window pOvl = pSpec + tl/2 - 1; */
    pOvl = pSpec + tl / 2 - 1;

    /* Previous window values. */
    hMdct->prev_nr = nr;
    hMdct->prev_fr = fr;
    hMdct->prev_tl = tl;
    hMdct->prev_wrs = wrs;

    /* Previous aliasing symmetry */
    hMdct->prevPrevAliasSymmetry = hMdct->prevAliasSymmetry;
    hMdct->prevAliasSymmetry = currAliasSymmetry;
  }

  /* Save overlap */

  pOvl = hMdct->overlap.freq + hMdct->ov_size - tl / 2;
  memcpy(pOvl, &spectrum[(nSpec - 1) * tl], (tl / 2) * sizeof(int32_t));

  return nrSamples;
}
