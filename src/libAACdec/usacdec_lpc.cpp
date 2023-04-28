
/**************************** AAC decoder library ******************************

   Author(s):   Matthias Hildenbrand, Manuel Jander

   Description: USAC LPC/AVQ decode

*******************************************************************************/

#include <memory.h>
#include "../libAACdec/aac_rom.h"
#include "../libFDK/FDK_lpc.h"
#include "../libFDK/FDK_trigFcts.h"
#include "newAACDecoder.h"

/*
 * Helper functions.
 */
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief Read unary code.
 * \param hBs bitstream handle as data source.
 * \return decoded value.
 */
static int32_t get_vlclbf(HANDLE_FDK_BITSTREAM hBs) {
    const uint8_t NQ_MAX = 36;
    int32_t result = 0;

    while(FDKreadBits(hBs, 1) && result <= NQ_MAX) { result++; }
    return result;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief Read bit count limited unary code.
 * \param hBs bitstream handle as data source
 * \param n max amount of bits to be read.
 * \return decoded value.
 */
static int32_t get_vlclbf_n(HANDLE_FDK_BITSTREAM hBs, int32_t n) {
    int32_t result = 0;

    while(FDKreadBits(hBs, 1)) {
        result++;
        n--;
        if(n <= 0) { break; }
    }

    return result;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void nearest_neighbor_2D8(int32_t x[8], int32_t y[8]) {
    int32_t s, em, e[8];
    int32_t i, j, sum;

    /* round x into 2Z^8 i.e. compute y=(y1,...,y8) such that yi = 2[xi/2]
       where [.] is the nearest integer operator
       in the mean time, compute sum = y1+...+y8
    */
    sum = 0;
    for(i = 0; i < 8; i++) {
        int32_t tmp;
        /* round to ..., -2, 0, 2, ... ([-1..1[ --> 0) */
        if(x[i] < (int32_t)0) {
            tmp = (int32_t)1 << DFRACT_BITS / 2 - x[i];
            y[i] = -2 * ((tmp >> DFRACT_BITS / 2) >> 1);
        }
        else {
            tmp = (int32_t)1 << DFRACT_BITS / 2 + x[i];
            y[i] = 2 * ((tmp >> DFRACT_BITS / 2) >> 1);
        }
        sum += y[i];
    }
    /* check if y1+...+y8 is a multiple of 4
       if not, y is not round xj in the wrong way where j is defined by
          j = arg max_i | xi -yi|
       (this is called the Wagner rule)
    */
    if(sum % 4) {
        /* find j = arg max_i | xi -yi| */
        em = (int16_t)0;
        j = 0;
        for(i = 0; i < 8; i++) {
            /* compute ei = xi-yi */
            e[i] = x[i] - (y[i] << (DFRACT_BITS / 2));
        }
        for(i = 0; i < 8; i++) {
            /* compute |ei| = | xi-yi | */
            if(e[i] < (int32_t)0) { s = -e[i]; }
            else { s = e[i]; }
            /* check if |ei| is maximal, if so, set j=i */
            if(em < s) {
                em = s;
                j = i;
            }
        }
        /* round xj in the "wrong way" */
        if(e[j] < (int32_t)0) { y[j] -= 2; }
        else { y[j] += 2; }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void RE8_PPV(int32_t x[], int16_t y[], int32_t r) {
    int32_t i, y0[8], y1[8];
    int32_t x1[8], tmp;
    int64_t e;

    /* find the nearest neighbor y0 of x in 2D8 */
    nearest_neighbor_2D8(x, y0);
    /* find the nearest neighbor y1 of x in 2D8+(1,...,1) (by coset decoding) */
    for(i = 0; i < 8; i++) { x1[i] = x[i] - (int32_t)1 << DFRACT_BITS / 2; }
    nearest_neighbor_2D8(x1, y1);
    for(i = 0; i < 8; i++) { y1[i] += 1; }

    /* compute e0=||x-y0||^2 and e1=||x-y1||^2 */
    e = 0;
    for(i = 0; i < 8; i++) {
        tmp = x[i] - (y0[i] << (DFRACT_BITS / 2));
        e += (int64_t)fPow2Div2(tmp << r); /* shift left to ensure that no fract part bits get lost. */
        tmp = x[i] - (y1[i] << (DFRACT_BITS / 2));
        e -= (int64_t)fPow2Div2(tmp << r);
    }
    /* select best candidate y0 or y1 to minimize distortion */
    if(e < 0) {
        for(i = 0; i < 8; i++) { y[i] = y0[i]; }
    }
    else {
        for(i = 0; i < 8; i++) { y[i] = y1[i]; }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/* table look-up of unsigned value: find i where index >= table[i]
   Note: range must be >= 2, index must be >= table[0] */
static int32_t table_lookup(const uint16_t *table, uint32_t index, int32_t range) {
    int32_t i;

    for(i = 4; i < range; i += 4) {
        if(index < table[i]) { break; }
    }
    if(i > range) { i = range; }

    if(index < table[i - 2]) { i -= 2; }
    if(index < table[i - 1]) { i--; }
    i--;

    return (i); /* index >= table[i] */
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
//  re8_decode_rank_of_permutation(rank, xs, x)
//  DECODING OF THE RANK OF THE PERMUTATION OF xs
//  (i) rank: index (rank) of a permutation
//  (i) xs:   signed leader in RE8 (8-dimensional integer vector)
//  (o) x:    point in RE8 (8-dimensional integer vector)
//  --------------------------------------------------------------------------
static void re8_decode_rank_of_permutation(int32_t rank, int32_t *xs, int16_t x[8]) {
    int32_t a[8], w[8], B1, fac, fac_B, target;
    int32_t i, j;

    /* --- pre-processing based on the signed leader xs ---
       - compute the alphabet a=[a[0] ... a[q-1]] of x (q elements)
         such that a[0]!=...!=a[q-1]
         it is assumed that xs is sorted in the form of a signed leader
         which can be summarized in 2 requirements:
            a) |xs[0]| >= |xs[1]| >= |xs[2]| >= ... >= |xs[7]|
            b) if |xs[i]|=|xs[i-1]|, xs[i]>=xs[i+1]
         where |.| indicates the absolute value operator
       - compute q (the number of symbols in the alphabet)
       - compute w[0..q-1] where w[j] counts the number of occurences of
         the symbol a[j] in xs
       - compute B = prod_j=0..q-1 (w[j]!) where .! is the factorial */
    /* xs[i], xs[i-1] and ptr_w/a*/
    j = 0;
    w[j] = 1;
    a[j] = xs[0];
    B1 = 1;
    for(i = 1; i < 8; i++) {
        if(xs[i] != xs[i - 1]) {
            j++;
            w[j] = 1;
            a[j] = xs[i];
        }
        else {
            w[j]++;
            B1 *= w[j];
        }
    }

    /* --- actual rank decoding ---
       the rank of x (where x is a permutation of xs) is based on
       Schalkwijk's formula
       it is given by rank=sum_{k=0..7} (A_k * fac_k/B_k)
       the decoding of this rank is sequential and reconstructs x[0..7]
       element by element from x[0] to x[7]
       [the tricky part is the inference of A_k for each k...]
     */

    if(w[0] == 8) {
        for(i = 0; i < 8; i++) { x[i] = a[0]; /* avoid fac of 40320 */ }
    }
    else {
        target = rank * B;
        fac_B = 1;
        /* decode x element by element */
        for(i = 0; i < 8; i++) {
            fac = fac_B * fdk_dec_tab_factorial[i]; /* fac = 1..5040 */
            j = -1;
            do { target -= w[++j] * fac; } while(target >= 0); /* max of 30 tests / SV */
            x[i] = a[j];
            /* update rank, denominator B (B_k) and counter w[j] */
            target += w[j] * fac; /* target = fac_B*B*rank */
            fac_B *= w[j];
            w[j]--;
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
//  re8_decode_base_index(n, I, y)
//  DECODING OF AN INDEX IN Qn (n=0,2,3 or 4)
//  (i) n: codebook number (*n is an integer defined in {0,2,3,4})
//  (i) I: index of c (pointer to unsigned 16-bit word)
//  (o) y: point in RE8 (8-dimensional integer vector)
//  note: the index I is defined as a 32-bit word, but only
//  16 bits are required (int32_t can be replaced by unsigned integer)
//  --------------------------------------------------------------------------
static void re8_decode_base_index(int32_t *n, uint32_t index, int16_t y[8]) {
    int32_t i, im, t, sign_code, ka, ks, rank, leader[8];

    if(*n < 2) {
        for(i = 0; i < 8; i++) { y[i] = 0; }
    }
    else {
        // index = (unsigned int32_t)*I;
        /* search for the identifier ka of the absolute leader (table-lookup)
           Q2 is a subset of Q3 - the two cases are considered in the same branch
         */
        switch(*n) {
            case 2:
            case 3:
                i = table_lookup(fdk_dec_I3, index, NB_LDQ3);
                ka = fdk_dec_A3[i];
                break;
            case 4:
                i = table_lookup(fdk_dec_I4, index, NB_LDQ4);
                ka = fdk_dec_A4[i];
                break;
            default:
                assert(0);
                return;
        }
        /* reconstruct the absolute leader */
        for(i = 0; i < 8; i++) { leader[i] = fdk_dec_Da[ka][i]; }
        /* search for the identifier ks of the signed leader (table look-up)
           (this search is focused based on the identifier ka of the absolute
            leader)*/
        t = fdk_dec_Ia[ka];
        im = fdk_dec_Ns[ka];
        ks = table_lookup(fdk_dec_Is + t, index, im);

        /* reconstruct the signed leader from its sign code */
        sign_code = 2 * fdk_dec_Ds[t + ks];
        for(i = 7; i >= 0; i--) {
            leader[i] *= (1 - (sign_code & 2));
            sign_code >>= 1;
        }

        /* compute and decode the rank of the permutation */
        rank = index - fdk_dec_Is[t + ks]; /* rank = index - cardinality offset */

        re8_decode_rank_of_permutation(rank, leader, y);
    }
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/* re8_y2k(y,m,k)
   VORONOI INDEXING (INDEX DECODING) k -> y
   (i) k: Voronoi index k[0..7]
   (i) m: Voronoi modulo (m = 2^r = 1<<r, where r is integer >=2)
   (i) r: Voronoi order  (m = 2^r = 1<<r, where r is integer >=2)
   (o) y: 8-dimensional point y[0..7] in RE8
 */
static void re8_k2y(int32_t *k, int32_t r, int16_t *y) {
    int32_t i, tmp, sum;
    int16_t v[8];
    int32_t zf[8];

    assert(r <= (DFRACT_BITS / 2));

    /* compute y = k M and z=(y-a)/m, where
       M = [4        ]
           [2 2      ]
           [|   \    ]
           [2     2  ]
           [1 1 _ 1 1]
       a=(2,0,...,0)
       m = 1<<r
    */
    for(i = 0; i < 8; i++) { y[i] = k[7]; }
    zf[7] = y[7] << (DFRACT_BITS / 2 - r);
    sum = 0;
    for(i = 6; i >= 1; i--) {
        tmp = 2 * k[i];
        sum += tmp;
        y[i] += tmp;
        zf[i] = y[i] << (DFRACT_BITS / 2 - r);
    }
    y[0] += (4 * k[0] + sum);
    zf[0] = (y[0] - 2) << (DFRACT_BITS / 2 - r);
    /* find nearest neighbor v of z in infinite RE8 */
    RE8_PPV(zf, v, r);
    /* compute y -= m v */
    for(i = 0; i < 8; i++) { y[i] -= (int16_t)(v[i] << r); }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
//
//  RE8_dec(n, I, k, y)
//  MULTI-RATE INDEXING OF A POINT y in THE LATTICE RE8 (INDEX DECODING)
//  (i) n: codebook number (*n is an integer defined in {0,2,3,4,..,n_max}). n_max
//  = 36 (i) I: index of c (pointer to unsigned 16-bit word) (i) k: index of v
//  (8-dimensional vector of binary indices) = Voronoi index (o) y: point in RE8
//  (8-dimensional integer vector) note: the index I is defined as a 32-bit word,
//  but only 16 bits are required (int32_t can be replaced by unsigned integer)
//  return 0 on success, -1 on error.
//--------------------------------------------------------------------------
static int32_t RE8_dec(int32_t n, int32_t I, int32_t *k, int32_t *y) {
    int16_t  v[8];
    int16_t  _y[8];
    uint32_t r;
    int32_t  i;
    const uint8_t NQ_MAX = 36;
    /* Check bound of codebook qn */
    if(n > NQ_MAX) { return -1; }

    /* decode the sub-indices I and kv[] according to the codebook number n:
       if n=0,2,3,4, decode I (no Voronoi extension)
       if n>4, Voronoi extension is used, decode I and kv[] */
    if(n <= 4) {
        re8_decode_base_index(&n, I, _y);
        for(i = 0; i < 8; i++) { y[i] = (int32_t)_y[i]; }
    }
    else {
        /* compute the Voronoi modulo m = 2^r where r is extension order */
        r = ((n - 3) >> 1);

        while(n > 4) { n -= 2; }
        /* decode base codebook index I into c (c is an element of Q3 or Q4)
           [here c is stored in y to save memory] */
        re8_decode_base_index(&n, I, _y);
        /* decode Voronoi index k[] into v */
        re8_k2y(k, r, v);
        /* reconstruct y as y = m c + v (with m=2^r, r integer >=1) */
        for(i = 0; i < 8; i++) { y[i] = (int32_t)((_y[i] << r) + v[i]); }
    }
    return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief calculate inverse weighting factor and add non-weighted residual
 *        LSF vector to first stage LSF approximation
 * \param lsfq first stage LSF approximation values.
 * \param xq weighted residual LSF vector
 * \param nk_mode code book number coding mode.
 */
static void lsf_weight_2st(int16_t *lsfq, int32_t *xq, int32_t nk_mode) {
    int16_t d[M_LP_FILTER_ORDER + 1];
    int16_t factor;
    int32_t w; /* inverse weight factor */
    int32_t i;
    const uint16_t FREQ_MAX = 6400;
    const uint16_t FREQ_DIV = 400;

    /* compute lsf distance */
    d[0] = lsfq[0];
    d[M_LP_FILTER_ORDER] = FL2FXCONST_LPC((double)FREQ_MAX / (1 << LSF_SCALE)) - lsfq[M_LP_FILTER_ORDER - 1];
    for(i = 1; i < M_LP_FILTER_ORDER; i++) { d[i] = lsfq[i] - lsfq[i - 1]; }

    switch(nk_mode) {
        case 0:
            factor = FL2FXCONST_SGL(2.0f * 60.0f / FREQ_DIV);
            break; /* abs */
        case 1:
            factor = FL2FXCONST_SGL(2.0f * 65.0f / FREQ_DIV);
            break; /* mid */
        case 2:
            factor = FL2FXCONST_SGL(2.0f * 64.0f / FREQ_DIV);
            break; /* rel1 */
        default:
            factor = FL2FXCONST_SGL(2.0f * 63.0f / FREQ_DIV);
            break; /* rel2 */
    }
    /* add non-weighted residual LSF vector to LSF1st */
    for(i = 0; i < M_LP_FILTER_ORDER; i++) {
        w = (int32_t)fMultDiv2(factor, sqrtFixp(fMult(d[i], d[i + 1])));
        lsfq[i] = fAddSaturate(lsfq[i], FX_DBL2FX_LPC((int32_t)((int64_t)w * (int32_t)xq[i])));
    }

    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief decode nqn amount of code book numbers. These values determine the
 * amount of following bits for nqn AVQ RE8 vectors.
 * \param nk_mode quantization mode.
 * \param nqn amount code book number to read.
 * \param qn pointer to output buffer to hold decoded code book numbers qn.
 */
static void decode_qn(HANDLE_FDK_BITSTREAM hBs, int32_t nk_mode, int32_t nqn, int32_t qn[]) {
    int32_t n;

    if(nk_mode == 1) { /* nk mode 1 */
        /* Unary code for mid LPC1/LPC3 */
        /* Q0=0, Q2=10, Q3=110, ... */
        for(n = 0; n < nqn; n++) {
            qn[n] = get_vlclbf(hBs);
            if(qn[n] > 0) { qn[n]++; }
        }
    }
    else { /* nk_mode 0, 3 and 2 */
        /* 2 bits to specify Q2,Q3,Q4,ext */
        for(n = 0; n < nqn; n++) { qn[n] = 2 + FDKreadBits(hBs, 2); }
        if(nk_mode == 2) {
            /* Unary code for rel LPC1/LPC3 */
            /* Q0 = 0, Q5=10, Q6=110, ... */
            for(n = 0; n < nqn; n++) {
                if(qn[n] > 4) {
                    qn[n] = get_vlclbf(hBs);
                    if(qn[n] > 0) qn[n] += 4;
                }
            }
        }
        else { /* nk_mode == (0 and 3) */
            /* Unary code for abs and rel LPC0/LPC2 */
            /* Q5 = 0, Q6=10, Q0=110, Q7=1110, ... */
            for(n = 0; n < nqn; n++) {
                if(qn[n] > 4) {
                    qn[n] = get_vlclbf(hBs);
                    switch(qn[n]) {
                        case 0:
                            qn[n] = 5;
                            break;
                        case 1:
                            qn[n] = 6;
                            break;
                        case 2:
                            qn[n] = 0;
                            break;
                        default:
                            qn[n] += 4;
                            break;
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief reorder LSF coefficients to minimum distance.
 * \param lsf pointer to buffer containing LSF coefficients and where reordered
 * LSF coefficients will be stored into, scaled by LSF_SCALE.
 * \param min_dist min distance scaled by LSF_SCALE
 * \param n number of LSF/LSP coefficients.
 */
static void reorder_lsf(int16_t *lsf, int16_t min_dist, int32_t n) {
    int16_t lsf_min;
    int32_t i;
    const uint16_t FREQ_MAX = 6400;

    lsf_min = min_dist;
    for(i = 0; i < n; i++) {
        if(lsf[i] < lsf_min) { lsf[i] = lsf_min; }
        lsf_min = fAddSaturate(lsf[i], min_dist);
    }

    /* reverse */
    lsf_min = FL2FXCONST_LPC((double)FREQ_MAX / (1 << LSF_SCALE)) - min_dist;
    for(i = n - 1; i >= 0; i--) {
        if(lsf[i] > lsf_min) { lsf[i] = lsf_min; }

        lsf_min = lsf[i] - min_dist;
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief First stage approximation
 * \param hBs bitstream handle as data source
 * \param lsfq pointer to output buffer to hold LPC coefficients scaled by
 * LSF_SCALE.
 */
static void vlpc_1st_dec(HANDLE_FDK_BITSTREAM hBs, /* input:  codebook index                  */
                         int16_t             *lsfq /* i/o:    i:prediction   o:quantized lsf  */
) {
    const int16_t *p_dico;
    int32_t        i, index;

    index = FDKreadBits(hBs, 8);
    p_dico = &fdk_dec_dico_lsf_abs_8b[index * M_LP_FILTER_ORDER];
    for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsfq[i] = p_dico[i]; }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief Do first stage approximation weighting and multiply with AVQ
 * refinement.
 * \param hBs bitstream handle data ssource.
 * \param lsfq buffer holding 1st stage approx, 2nd stage approx is added to
 * this values.
 * \param nk_mode quantization mode.
 * \return 0 on success, -1 on error.
 */
static int32_t vlpc_2st_dec(HANDLE_FDK_BITSTREAM hBs, int16_t *lsfq, /* i/o:    i:1st stage   o:1st+2nd stage   */
                            int32_t nk_mode                          /* input:  0=abs, >0=rel                   */
) {
    int32_t err;
    int32_t xq[M_LP_FILTER_ORDER]; /* weighted residual LSF vector */
    const uint8_t LSF_GAP = 50;

    /* Decode AVQ refinement */
    { err = CLpc_DecodeAVQ(hBs, xq, nk_mode, 2, 8); }
    if(err != 0) { return -1; }

    /* add non-weighted residual LSF vector to LSF1st */
    lsf_weight_2st(lsfq, xq, nk_mode);

    /* reorder */
    reorder_lsf(lsfq, FL2FXCONST_LPC((double)LSF_GAP / (1 << LSF_SCALE)), M_LP_FILTER_ORDER);

    return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
int32_t CLpc_DecodeAVQ(HANDLE_FDK_BITSTREAM hBs, int32_t *pOutput, int32_t nk_mode, int32_t no_qn, int32_t length) {
    int32_t i, l;

    for(i = 0; i < length; i += 8 * no_qn) {
        int32_t qn[2], nk, n, I;
        int32_t kv[8] = {0};

        decode_qn(hBs, nk_mode, no_qn, qn);

        for(l = 0; l < no_qn; l++) {
            if(qn[l] == 0) { memset(&pOutput[i + l * 8], 0, 8 * sizeof(int32_t)); }

            /* Voronoi extension order ( nk ) */
            nk = 0;
            n = qn[l];
            if(qn[l] > 4) {
                nk = (qn[l] - 3) >> 1;
                n = qn[l] - nk * 2;
            }
            /* Base codebook index, in reverse bit group order (!) */
            I = FDKreadBits(hBs, 4 * n);
            if(nk > 0) {
                int32_t j;

                for(j = 0; j < 8; j++) { kv[j] = FDKreadBits(hBs, nk); }
            }
            if(RE8_dec(qn[l], I, kv, &pOutput[i + l * 8]) != 0) { return -1; }
        }
    }
    return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
int32_t CLpc_Read(HANDLE_FDK_BITSTREAM hBs, int16_t lsp[][M_LP_FILTER_ORDER], int16_t lpc4_lsf[M_LP_FILTER_ORDER],
                  int16_t lsf_adaptive_mean_cand[M_LP_FILTER_ORDER], int16_t pStability[], uint8_t *mod, int32_t first_lpd_flag,
                  int32_t last_lpc_lost, int32_t last_frame_ok) {
    int32_t       i, k, err;
    int32_t       mode_lpc_bin = 0; /* mode_lpc bitstream representation */
    int32_t       lpc_present[5] = {0, 0, 0, 0, 0};
    int32_t       lpc0_available = 1;
    int32_t       s = 0;
    int32_t       l = 3;
    const int32_t nbDiv = NB_DIV;

    lpc_present[4 >> s] = 1; /* LPC4 */

    /* Decode LPC filters in the following order: LPC 4,0,2,1,3 */

    /*** Decode LPC4 ***/
    vlpc_1st_dec(hBs, lsp[4 >> s]);
    err = vlpc_2st_dec(hBs, lsp[4 >> s], 0); /* nk_mode = 0 */
    if(err != 0) { return err; }

    /*** Decode LPC0 and LPC2 ***/
    k = 0;
    if(!first_lpd_flag) {
        lpc_present[0] = 1;
        lpc0_available = !last_lpc_lost;
        /* old LPC4 is new LPC0 */
        for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[0][i] = lpc4_lsf[i]; }
        /* skip LPC0 and continue with LPC2 */
        k = 2;
    }

    for(; k < l; k += 2) {
        int32_t nk_mode = 0;

        if((k == 2) && (mod[0] == 3)) { break; /* skip LPC2 */ }

        lpc_present[k >> s] = 1;

        mode_lpc_bin = FDKreadBit(hBs);

        if(mode_lpc_bin == 0) {
            /* LPC0/LPC2: Abs */
            vlpc_1st_dec(hBs, lsp[k >> s]);
        }
        else {
            /* LPC0/LPC2: RelR */
            for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[k >> s][i] = lsp[4 >> s][i]; }
            nk_mode = 3;
        }

        err = vlpc_2st_dec(hBs, lsp[k >> s], nk_mode);
        if(err != 0) { return err; }
    }

    /*** Decode LPC1 ***/
    if(mod[0] < 2) { /* else: skip LPC1 */
        lpc_present[1] = 1;
        mode_lpc_bin = get_vlclbf_n(hBs, 2);

        switch(mode_lpc_bin) {
            case 1:
                /* LPC1: abs */
                vlpc_1st_dec(hBs, lsp[1]);
                err = vlpc_2st_dec(hBs, lsp[1], 0);
                if(err != 0) { return err; }
                break;
            case 2:
                /* LPC1: mid0 (no second stage AVQ quantizer in this case) */
                if(lpc0_available) { /* LPC0/lsf[0] might be zero some times */
                    for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[1][i] = (lsp[0][i] >> 1) + (lsp[2][i] >> 1); }
                }
                else {
                    for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[1][i] = lsp[2][i]; }
                }
                break;
            case 0:
                /* LPC1: RelR */
                for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[1][i] = lsp[2][i]; }
                err = vlpc_2st_dec(hBs, lsp[1], 2 << s);
                if(err != 0) { return err; }
                break;
        }
    }

    /*** Decode LPC3 ***/
    if((mod[2] < 2)) { /* else: skip LPC3 */
        int32_t nk_mode = 0;
        lpc_present[3] = 1;

        mode_lpc_bin = get_vlclbf_n(hBs, 3);

        switch(mode_lpc_bin) {
            case 1:
                /* LPC3: abs */
                vlpc_1st_dec(hBs, lsp[3]);
                break;
            case 0:
                /* LPC3: mid */
                for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[3][i] = (lsp[2][i] >> 1) + (lsp[4][i] >> 1); }
                nk_mode = 1;
                break;
            case 2:
                /* LPC3: relL */
                for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[3][i] = lsp[2][i]; }
                nk_mode = 2;
                break;
            case 3:
                /* LPC3: relR */
                for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[3][i] = lsp[4][i]; }
                nk_mode = 2;
                break;
        }
        err = vlpc_2st_dec(hBs, lsp[3], nk_mode);
        if(err != 0) { return err; }
    }

    if(!lpc0_available && !last_frame_ok) {
        /* LPC(0) was lost. Use next available LPC(k) instead */
        for(k = 1; k < (nbDiv + 1); k++) {
            if(lpc_present[k]) {
                const double LSF_INIT_TILT = 0.25;
                for(i = 0; i < M_LP_FILTER_ORDER; i++) {
                    if(mod[0] > 0) {
                        lsp[0][i] = FX_DBL2FX_LPC(fMult(lsp[k][i], FL2FXCONST_SGL(1.0f - LSF_INIT_TILT)) +
                                                  fMult(fdk_dec_lsf_init[i], FL2FXCONST_SGL(LSF_INIT_TILT)));
                    }
                    else { lsp[0][i] = lsp[k][i]; }
                }
                break;
            }
        }
    }
    for(i = 0; i < M_LP_FILTER_ORDER; i++) { lpc4_lsf[i] = lsp[4 >> s][i]; }
    {
        int32_t divFac;
        int32_t last, numLpc = 0;

        i = nbDiv;
        do { numLpc += lpc_present[i--]; } while(i >= 0 && numLpc < 3);

        last = i;

        switch(numLpc) {
            case 3:
                divFac = FL2FXCONST_DBL(1.0f / 3.0f);
                break;
            case 2:
                divFac = FL2FXCONST_DBL(1.0f / 2.0f);
                break;
            default:
                divFac = FL2FXCONST_DBL(1.0f);
                break;
        }

        /* get the adaptive mean for the next (bad) frame */
        for(k = 0; k < M_LP_FILTER_ORDER; k++) {
            int32_t tmp = (int32_t)0;
            for(i = nbDiv; i > last; i--) {
                if(lpc_present[i]) { tmp = fMultAdd(tmp >> 1, lsp[i][k], divFac); }
            }
            lsf_adaptive_mean_cand[k] = FX_DBL2FX_LPC(tmp);
        }
    }

    /* calculate stability factor Theta. Needed for ACELP decoder and concealment
     */
    {
        int16_t *lsf_prev, *lsf_curr;
        k = 0;

        assert(lpc_present[0] == 1 && lpc_present[4 >> s] == 1);
        lsf_prev = lsp[0];
        for(i = 1; i < (nbDiv + 1); i++) {
            if(lpc_present[i]) {
                int32_t tmp = (int32_t)0;
                int32_t j;
                lsf_curr = lsp[i];

                /* sum = tmp * 2^(LSF_SCALE*2 + 4) */
                for(j = 0; j < M_LP_FILTER_ORDER; j++) { tmp += fPow2Div2((int16_t)(lsf_curr[j] - lsf_prev[j])) >> 3; }

                /* tmp = (float)(FL2FXCONST_DBL(1.25f) - fMult(tmp,
                 * FL2FXCONST_DBL(1/400000.0f))); */
                tmp = FL2FXCONST_DBL(1.25f / (1 << LSF_SCALE)) - fMult(tmp, FL2FXCONST_DBL((1 << (LSF_SCALE + 4)) / 400000.0f));
                if(tmp >= FL2FXCONST_DBL(1.0f / (1 << LSF_SCALE))) { pStability[k] = FL2FXCONST_SGL(1.0f / 2.0f); }
                else if(tmp < FL2FXCONST_DBL(0.0f)) { pStability[k] = 0; }
                else { pStability[k] = FX_DBL2FX_SGL(tmp << (LSF_SCALE - 1)); }

                lsf_prev = lsf_curr;
                k = i;
            }
            else {
                /* Mark stability value as undefined. */
                pStability[i] = (int16_t)-1;
            }
        }
    }

    /* convert into LSP domain */
    for(i = 0; i < (nbDiv + 1); i++) {
        if(lpc_present[i]) {
            for(k = 0; k < M_LP_FILTER_ORDER; k++) {
                lsp[i][k] = FX_DBL2FX_LPC(fixp_cos(fMult(lsp[i][k], FL2FXCONST_SGL((1 << LSPARG_SCALE) * M_PI / 6400.0)), LSF_SCALE - LSPARG_SCALE));
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void CLpc_Conceal(int16_t lsp[][M_LP_FILTER_ORDER], int16_t lpc4_lsf[M_LP_FILTER_ORDER], int16_t lsf_adaptive_mean[M_LP_FILTER_ORDER],
                  const int32_t first_lpd_flag) {
    int32_t i, j;

    const uint16_t BETA        = 8192;
    const uint16_t ONE_BETA    = 24576;
    const uint16_t BFI_FAC     = 29491;
    const uint16_t ONE_BFI_FAC = 3277;

    /* Frame loss concealment (could be improved) */

    if(first_lpd_flag) {
        /* Reset past LSF values */
        for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[0][i] = lpc4_lsf[i] = fdk_dec_lsf_init[i]; }
    }
    else {
        /* old LPC4 is new LPC0 */
        for(i = 0; i < M_LP_FILTER_ORDER; i++) { lsp[0][i] = lpc4_lsf[i]; }
    }

    /* LPC1 */
    for(i = 0; i < M_LP_FILTER_ORDER; i++) {
        int16_t lsf_mean = FX_DBL2FX_LPC(fMult(BETA, fdk_dec_lsf_init[i]) + fMult(ONE_BETA, lsf_adaptive_mean[i]));

        lsp[1][i] = FX_DBL2FX_LPC(fMult(BFI_FAC, lpc4_lsf[i]) + fMult(ONE_BFI_FAC, lsf_mean));
    }

    /* LPC2 - LPC4 */
    for(j = 2; j <= 4; j++) {
        for(i = 0; i < M_LP_FILTER_ORDER; i++) {
            /* lsf_mean[i] =  FX_DBL2FX_LPC(fMult((int16_t)(BETA + j *
               FL2FXCONST_LPC(0.1f)), fdk_dec_lsf_init[i])
                                          + fMult((int16_t)(ONE_BETA - j *
               FL2FXCONST_LPC(0.1f)), lsf_adaptive_mean[i])); */

            int16_t lsf_mean = FX_DBL2FX_LPC(fMult((int16_t)(BETA + (int16_t)(j * (int32_t)3277)), (int16_t)fdk_dec_lsf_init[i]) +
                                             fMult((int16_t)(ONE_BETA - (int16_t)(j * (int32_t)3277)), lsf_adaptive_mean[i]));

            lsp[j][i] = FX_DBL2FX_LPC(fMult(BFI_FAC, lsp[j - 1][i]) + fMult(ONE_BFI_FAC, lsf_mean));
        }
    }

    /* Update past values for the future */
    for(i = 0; i < M_LP_FILTER_ORDER; i++) { lpc4_lsf[i] = lsp[4][i]; }

    /* convert into LSP domain */
    for(j = 0; j < 5; j++) {
        for(i = 0; i < M_LP_FILTER_ORDER; i++) {
            lsp[j][i] = FX_DBL2FX_LPC(fixp_cos(fMult(lsp[j][i], FL2FXCONST_SGL((1 << LSPARG_SCALE) * M_PI / 6400.0)), LSF_SCALE - LSPARG_SCALE));
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void E_LPC_a_weight(int16_t *wA, const int16_t *A, int32_t m) {
    int32_t f;
    int32_t i;

    f = FL2FXCONST_DBL(0.92f);
    for(i = 0; i < m; i++) {
        wA[i] = FX_DBL2FX_LPC(fMult(A[i], f));
        f = fMult(f, FL2FXCONST_DBL(0.92f));
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void CLpd_DecodeGain(int32_t *gain, int32_t *gain_e, int32_t gain_code) {
    /* gain * 2^(gain_e) = 10^(gain_code/28) */
    *gain = fLdPow(FL2FXCONST_DBL(3.3219280948873623478703194294894 / 4.0), /* log2(10)*/
                   2, fMultDiv2((int32_t)gain_code << (DFRACT_BITS - 1 - 7), FL2FXCONST_DBL(2.0f / 28.0f)), 7, gain_e);
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief *   Find the polynomial F1(z) or F2(z) from the LSPs.
 * This is performed by expanding the product polynomials:
 *
 * F1(z) =   product   ( 1 - 2 LSP_i z^-1 + z^-2 )
 *         i=0,2,4,6,8
 * F2(z) =   product   ( 1 - 2 LSP_i z^-1 + z^-2 )
 *         i=1,3,5,7,9
 *
 * where LSP_i are the LSPs in the cosine domain.
 * R.A.Salami    October 1990
 * \param lsp input, line spectral freq. (cosine domain)
 * \param f output, the coefficients of F1 or F2, scaled by 8 bits
 * \param n no of coefficients (m/2)
 * \param flag 1 : F1(z) ; 2 : F2(z)
 */

static void get_lsppol(int16_t lsp[], int32_t f[], int32_t n, int32_t flag) {
    int32_t  b;
    int16_t *plsp;
    int32_t  i, j;
    const uint8_t SF_F = 8;

    plsp = lsp + flag - 1;
    f[0] = FL2FXCONST_DBL(1.0f / (1 << SF_F));
    b = -FX_LPC2FX_DBL(*plsp);
    f[1] = b >> (SF_F - 1);
    for(i = 2; i <= n; i++) {
        plsp += 2;
        b = -FX_LPC2FX_DBL(*plsp);
        f[i] = SATURATE_LEFT_SHIFT((fMultDiv2(b, f[i - 1]) + (f[i - 2] >> 1)), 2, DFRACT_BITS);
        for(j = i - 1; j > 1; j--) { f[j] = SATURATE_LEFT_SHIFT(((f[j] >> 2) + fMultDiv2(b, f[j - 1]) + (f[j - 2] >> 2)), 2, DFRACT_BITS); }
        f[1] = f[1] + (b >> (SF_F - 1));
    }
    return;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * \brief lsp input LSP vector
 * \brief a output LP filter coefficient vector scaled by SF_A_COEFFS.
 */
void E_LPC_f_lsp_a_conversion(int16_t *lsp, int16_t *a, int32_t *a_exp) {
    const uint8_t SF_F = 8;
    const uint8_t NC = M_LP_FILTER_ORDER / 2;
    int32_t f1[NC + 1], f2[NC + 1];
    int32_t i, k;

    /*-----------------------------------------------------*
     *  Find the polynomials F1(z) and F2(z)               *
     *-----------------------------------------------------*/

    get_lsppol(lsp, f1, NC, 1);
    get_lsppol(lsp, f2, NC, 2);

    /*-----------------------------------------------------*
     *  Multiply F1(z) by (1+z^-1) and F2(z) by (1-z^-1)   *
     *-----------------------------------------------------*/
    scaleValues(f1, NC + 1, -2);
    scaleValues(f2, NC + 1, -2);

    for(i = NC; i > 0; i--) {
        f1[i] += f1[i - 1];
        f2[i] -= f2[i - 1];
    }

    int32_t aDBL[M_LP_FILTER_ORDER];

    for(i = 1, k = M_LP_FILTER_ORDER - 1; i <= NC; i++, k--) {
        aDBL[i - 1] = f1[i] + f2[i];
        aDBL[k] = f1[i] - f2[i];
    }

    int32_t headroom_a = getScalefactor(aDBL, M_LP_FILTER_ORDER);

    for(i = 0; i < M_LP_FILTER_ORDER; i++) { a[i] = FX_DBL2FX_LPC(aDBL[i] << headroom_a); }

    *a_exp = SF_F + (2 - 1) - headroom_a;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
