

/**************************** SBR decoder library ******************************

   Author(s):   Matthias Hildenbrand

   Description: Decode Predictive Vector Coding Data

*******************************************************************************/
#include "../libAACdec/newAACDecoder.h"
#include "pvc_dec.h"

/* PVC interal definitions */
#define PVC_DIVMODE_BITS 3
#define PVC_NSMODE_BITS 1
#define PVC_REUSEPVCID_BITS 1
#define PVC_PVCID_BITS 7
#define PVC_GRIDINFO_BITS 1
#define PVC_NQMFBAND 64
#define PVC_NBLOW 3 /* max. number of grouped QMF subbands below SBR range */

#define PVC_NTAB1 3
#define PVC_NTAB2 128
#define PVC_ID_NBIT 7

/* Exponent of pPvcStaticData->Esg and predictedEsg in dB domain.
   max(Esg) = 10*log10(2^15*2^15) = 90.30;
   min(Esg) = 10*log10(0.1) = -10
   max of predicted Esg seems to be higher than 90dB but 7 Bit should be enough.
*/
#define PVC_ESG_EXP 7

#define LOG10FAC 0.752574989159953f     /* == 10/log2(10) * 2^-2 */
#define LOG10FAC_INV 0.664385618977472f /* == log2(10)/10 * 2^1 */



static const int16_t pvc_SC_16[] = {
	5185, 5173, 5135, 5072, 4984, 4869, 4728, 4559, 4360, 4130, 3865, 3560, 3208, 2794, 2290, 1623,
};



static const int16_t pvc_SC_12[] = {
	6842, 6813, 6725, 6577, 6368, 6095, 5754, 5339, 4838, 4233, 3481, 2472,
};



static const int16_t pvc_SC_4[] = {
	19159,
	18415,
	16110,
	11852,
};



static const int16_t pvc_SC_3[] = {
	24846,
	23122,
	17569,
};

static const uint8_t g_3a_pvcTab1_mode1[PVC_NTAB1][PVC_NBLOW][PVC_NBHIGH_MODE1] =
    {{{0x4F, 0x5B, 0x57, 0x52, 0x4D, 0x65, 0x45, 0x57},
      {0xF3, 0x0F, 0x18, 0x20, 0x19, 0x4F, 0x3D, 0x23},
      {0x78, 0x57, 0x55, 0x50, 0x50, 0x20, 0x36, 0x37}},
     {{0x4C, 0x5F, 0x53, 0x37, 0x1E, 0xFD, 0x15, 0x0A},
      {0x05, 0x0E, 0x28, 0x41, 0x48, 0x6E, 0x54, 0x5B},
      {0x59, 0x47, 0x40, 0x40, 0x3D, 0x33, 0x3F, 0x39}},
     {{0x47, 0x5F, 0x57, 0x34, 0x3C, 0x2E, 0x2E, 0x31},
      {0xFA, 0x13, 0x23, 0x4E, 0x44, 0x7C, 0x34, 0x38},
      {0x63, 0x43, 0x41, 0x3D, 0x35, 0x19, 0x3D, 0x33}}};

static const uint8_t g_2a_pvcTab2_mode1[PVC_NTAB2][PVC_NBHIGH_MODE1] = {
    {0xCB, 0xD1, 0xCC, 0xD2, 0xE2, 0xEB, 0xE7, 0xE8},
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0x84, 0x8C, 0x88, 0x83, 0x90, 0x93, 0x86, 0x80},
    {0xD7, 0xD8, 0xC0, 0xC7, 0xCF, 0xE5, 0xF1, 0xF6},
    {0xA5, 0xA6, 0xAA, 0xA8, 0xB0, 0xB1, 0xB8, 0xB8},
    {0xD7, 0xCB, 0xC1, 0xC3, 0xC5, 0xC9, 0xC9, 0xCE},
    {0xCA, 0xB5, 0xB8, 0xB3, 0xAC, 0xB6, 0xBB, 0xB8},
    {0xC1, 0xC4, 0xC3, 0xC5, 0xC6, 0xCA, 0xCA, 0xCB},
    {0xE0, 0xE1, 0xD8, 0xCD, 0xCB, 0xCB, 0xCE, 0xCC},
    {0xDB, 0xE1, 0xDF, 0xDB, 0xDC, 0xD9, 0xD9, 0xD6},
    {0xE0, 0xDE, 0xDD, 0xDD, 0xE0, 0xE3, 0xE5, 0xE6},
    {0xCA, 0xD2, 0xCD, 0xCE, 0xD5, 0xDB, 0xD9, 0xDB},
    {0xD2, 0xE0, 0xDB, 0xD5, 0xDB, 0xDE, 0xE3, 0xE1},
    {0xE5, 0xDB, 0xD0, 0xD2, 0xD8, 0xDD, 0xDB, 0xDD},
    {0xC0, 0xB5, 0xBF, 0xDD, 0xE3, 0xDC, 0xDC, 0xE4},
    {0xDB, 0xCE, 0xC6, 0xCF, 0xCF, 0xD1, 0xD3, 0xD4},
    {0xC9, 0xD7, 0xDA, 0xE2, 0xE9, 0xE7, 0xDF, 0xDC},
    {0x0A, 0x07, 0x0A, 0x08, 0x19, 0x24, 0x1F, 0x22},
    {0x1E, 0x1F, 0x11, 0x0E, 0x22, 0x2D, 0x33, 0x32},
    {0xF0, 0xDA, 0xDC, 0x18, 0x1F, 0x19, 0x0A, 0x1E},
    {0x09, 0xF8, 0xE6, 0x05, 0x19, 0x11, 0x0E, 0x0B},
    {0x09, 0x10, 0x0E, 0xE6, 0xF4, 0x20, 0x22, 0xFA},
    {0xF2, 0xE5, 0xF8, 0x0E, 0x18, 0x15, 0x0D, 0x10},
    {0x15, 0x13, 0x16, 0x0A, 0x0D, 0x1F, 0x1D, 0x1B},
    {0xFA, 0xFF, 0xFE, 0xFF, 0x09, 0x11, 0x03, 0x0B},
    {0xFE, 0xFA, 0xF2, 0xF8, 0x0C, 0x1E, 0x11, 0x12},
    {0xFA, 0xF8, 0x0B, 0x17, 0x1D, 0x17, 0x0E, 0x16},
    {0x00, 0xF3, 0xFD, 0x0A, 0x1C, 0x17, 0xFD, 0x08},
    {0xEA, 0xEA, 0x03, 0x12, 0x1E, 0x14, 0x09, 0x04},
    {0x02, 0xFE, 0x04, 0xFB, 0x0C, 0x0E, 0x07, 0x02},
    {0xF6, 0x02, 0x07, 0x0B, 0x17, 0x17, 0x01, 0xFF},
    {0xF5, 0xFB, 0xFE, 0x04, 0x12, 0x14, 0x0C, 0x0D},
    {0x10, 0x10, 0x0E, 0x04, 0x07, 0x11, 0x0F, 0x13},
    {0x0C, 0x0F, 0xFB, 0xF2, 0x0A, 0x12, 0x09, 0x0D},
    {0x0D, 0x1D, 0xF1, 0xF4, 0x2A, 0x06, 0x3B, 0x32},
    {0xFC, 0x08, 0x06, 0x02, 0x0E, 0x17, 0x08, 0x0E},
    {0x07, 0x02, 0xEE, 0xEE, 0x2B, 0xF6, 0x23, 0x13},
    {0x04, 0x02, 0x05, 0x08, 0x0B, 0x0E, 0xFB, 0xFB},
    {0x00, 0x04, 0x10, 0x18, 0x22, 0x25, 0x1D, 0x1F},
    {0xFB, 0x0D, 0x07, 0x00, 0x0C, 0x0F, 0xFC, 0x02},
    {0x00, 0x00, 0x00, 0x01, 0x05, 0x07, 0x03, 0x05},
    {0x04, 0x05, 0x08, 0x13, 0xFF, 0xEB, 0x0C, 0x06},
    {0x05, 0x13, 0x0E, 0x0B, 0x12, 0x15, 0x09, 0x0A},
    {0x09, 0x03, 0x09, 0x05, 0x12, 0x16, 0x11, 0x12},
    {0x14, 0x1A, 0x06, 0x01, 0x10, 0x11, 0xFE, 0x02},
    {0x01, 0x0B, 0x0B, 0x0C, 0x18, 0x21, 0x10, 0x13},
    {0x12, 0x0D, 0x0A, 0x10, 0x1C, 0x1D, 0x0D, 0x10},
    {0x03, 0x09, 0x14, 0x15, 0x1B, 0x1A, 0x01, 0xFF},
    {0x08, 0x12, 0x13, 0x0E, 0x16, 0x1D, 0x14, 0x1B},
    {0x07, 0x15, 0x1C, 0x1B, 0x20, 0x21, 0x11, 0x0E},
    {0x12, 0x18, 0x19, 0x17, 0x20, 0x25, 0x1A, 0x1E},
    {0x0C, 0x1A, 0x1D, 0x22, 0x2F, 0x33, 0x27, 0x28},
    {0x0E, 0x1A, 0x17, 0x10, 0x0A, 0x0E, 0xFF, 0x06},
    {0x1A, 0x1C, 0x18, 0x14, 0x1A, 0x16, 0x0A, 0x0E},
    {0x1E, 0x27, 0x25, 0x26, 0x27, 0x2A, 0x21, 0x21},
    {0xF1, 0x0A, 0x16, 0x1C, 0x28, 0x25, 0x15, 0x19},
    {0x08, 0x12, 0x09, 0x08, 0x16, 0x17, 0xEF, 0xF6},
    {0x0C, 0x0B, 0x00, 0xFC, 0x04, 0x09, 0xFC, 0x03},
    {0xFB, 0xF1, 0xF8, 0x26, 0x24, 0x18, 0x1D, 0x20},
    {0xF9, 0x01, 0x0C, 0x0F, 0x07, 0x08, 0x06, 0x07},
    {0x07, 0x06, 0x08, 0x04, 0x07, 0x0D, 0x07, 0x09},
    {0xFE, 0x01, 0x06, 0x05, 0x13, 0x1B, 0x14, 0x19},
    {0x09, 0x0C, 0x0E, 0x01, 0x08, 0x05, 0xFB, 0xFD},
    {0x07, 0x06, 0x03, 0x0A, 0x16, 0x12, 0x04, 0x07},
    {0x04, 0x01, 0x00, 0x04, 0x1F, 0x20, 0x0E, 0x0A},
    {0x03, 0xFF, 0xF6, 0xFB, 0x15, 0x1A, 0x00, 0x03},
    {0xFC, 0x18, 0x0B, 0x2D, 0x35, 0x23, 0x12, 0x09},
    {0x02, 0xFE, 0x01, 0xFF, 0x0C, 0x11, 0x0D, 0x0F},
    {0xFA, 0xE9, 0xD9, 0xFF, 0x0D, 0x05, 0x0D, 0x10},
    {0xF1, 0xE0, 0xF0, 0x01, 0x06, 0x06, 0x06, 0x10},
    {0xE9, 0xD4, 0xD7, 0x0F, 0x14, 0x0B, 0x0D, 0x16},
    {0x00, 0xFF, 0xEE, 0xE5, 0xFF, 0x08, 0x02, 0xF9},
    {0xE0, 0xDA, 0xE5, 0xFE, 0x09, 0x02, 0xF9, 0x04},
    {0xE0, 0xE2, 0xF4, 0x09, 0x13, 0x0C, 0x0D, 0x09},
    {0xFC, 0x02, 0x04, 0xFF, 0x00, 0xFF, 0xF8, 0xF7},
    {0xFE, 0xFB, 0xED, 0xF2, 0xFE, 0xFE, 0x08, 0x0C},
    {0xF3, 0xEF, 0xD0, 0xE3, 0x05, 0x11, 0xFD, 0xFF},
    {0xFA, 0xEF, 0xEA, 0xFE, 0x0D, 0x0E, 0xFE, 0x02},
    {0xF7, 0xFB, 0xDB, 0xDF, 0x14, 0xDD, 0x07, 0xFE},
    {0xFE, 0x08, 0x00, 0xDB, 0xE5, 0x1A, 0x13, 0xED},
    {0xF9, 0xFE, 0xFF, 0xF4, 0xF3, 0x00, 0x05, 0x02},
    {0xEF, 0xDE, 0xD8, 0xEB, 0xEA, 0xF5, 0x0E, 0x19},
    {0xFB, 0xFC, 0xFA, 0xEC, 0xEB, 0xED, 0xEE, 0xE8},
    {0xEE, 0xFC, 0xFD, 0x00, 0x04, 0xFC, 0xF0, 0xF5},
    {0x00, 0xFA, 0xF4, 0xF1, 0xF5, 0xFA, 0xFB, 0xF9},
    {0xEB, 0xF0, 0xDF, 0xE3, 0xEF, 0x07, 0x02, 0x05},
    {0xF7, 0xF0, 0xE6, 0xE7, 0x06, 0x15, 0x06, 0x0C},
    {0xF1, 0xE4, 0xD8, 0xEA, 0x06, 0xF2, 0x07, 0x09},
    {0xFF, 0xFE, 0xFE, 0xF9, 0xFF, 0xFF, 0x02, 0xF9},
    {0xDD, 0xF4, 0xF0, 0xF1, 0xFF, 0xFF, 0xEA, 0xF1},
    {0xF0, 0xF1, 0xFD, 0x03, 0x03, 0xFE, 0x00, 0x05},
    {0xF1, 0xF6, 0xE0, 0xDF, 0xF5, 0x01, 0xF4, 0xF8},
    {0x02, 0x03, 0xE5, 0xDC, 0xE7, 0xFD, 0x02, 0x08},
    {0xEC, 0xF1, 0xF5, 0xEC, 0xF2, 0xF8, 0xF6, 0xEE},
    {0xF3, 0xF4, 0xF6, 0xF4, 0xF5, 0xF1, 0xE7, 0xEA},
    {0xF7, 0xF3, 0xEC, 0xEA, 0xEF, 0xF0, 0xEE, 0xF1},
    {0xEB, 0xF6, 0xFB, 0xFA, 0xEF, 0xF3, 0xF3, 0xF7},
    {0x01, 0x03, 0xF1, 0xF6, 0x05, 0xF8, 0xE1, 0xEB},
    {0xF5, 0xF6, 0xF6, 0xF4, 0xFB, 0xFB, 0xFF, 0x00},
    {0xF8, 0x01, 0xFB, 0xFA, 0xFF, 0x03, 0xFE, 0x04},
    {0x04, 0xFB, 0x03, 0xFD, 0xF5, 0xF7, 0xF6, 0xFB},
    {0x06, 0x09, 0xFB, 0xF4, 0xF9, 0xFA, 0xFC, 0xFF},
    {0xF5, 0xF6, 0xF1, 0xEE, 0xF5, 0xF8, 0xF5, 0xF9},
    {0xF5, 0xF9, 0xFA, 0xFC, 0x07, 0x09, 0x01, 0xFB},
    {0xD7, 0xE9, 0xE8, 0xEC, 0x00, 0x0C, 0xFE, 0xF1},
    {0xEC, 0x04, 0xE9, 0xDF, 0x03, 0xE8, 0x00, 0xFA},
    {0xE6, 0xE2, 0xFF, 0x0A, 0x13, 0x01, 0x00, 0xF7},
    {0xF1, 0xFA, 0xF7, 0xF5, 0x01, 0x06, 0x05, 0x0A},
    {0xF6, 0xF6, 0xFC, 0xF6, 0xE8, 0x11, 0xF2, 0xFE},
    {0xFE, 0x08, 0x05, 0x12, 0xFD, 0xD0, 0x0E, 0x07},
    {0xF1, 0xFE, 0xF7, 0xF2, 0xFB, 0x02, 0xFA, 0xF8},
    {0xF4, 0xEA, 0xEC, 0xF3, 0xFE, 0x01, 0xF7, 0xF6},
    {0xFF, 0xFA, 0xFB, 0xF9, 0xFF, 0x01, 0x04, 0x03},
    {0x00, 0xF9, 0xF4, 0xFC, 0x05, 0xFC, 0xF7, 0xFB},
    {0xF8, 0xFF, 0xEF, 0xEC, 0xFB, 0x04, 0xF8, 0x03},
    {0xEB, 0xF1, 0xED, 0xF4, 0x02, 0x0E, 0x0B, 0x04},
    {0xF7, 0x01, 0xF8, 0xF4, 0xF8, 0xEF, 0xF8, 0x04},
    {0xEB, 0xF0, 0xF7, 0xFC, 0x10, 0x0D, 0xF8, 0xF8},
    {0xE8, 0xFE, 0xEE, 0xE8, 0xED, 0xF7, 0xF5, 0xF8},
    {0xED, 0xEB, 0xE9, 0xEA, 0xF2, 0xF5, 0xF4, 0xF9},
    {0xEA, 0xF2, 0xEF, 0xEE, 0xF9, 0xFE, 0xFD, 0x02},
    {0xFA, 0xFD, 0x02, 0x0D, 0xFA, 0xE4, 0x0F, 0x01},
    {0xFF, 0x08, 0x05, 0xF6, 0xF7, 0xFB, 0xF1, 0xF1},
    {0xF4, 0xEC, 0xEE, 0xF6, 0xEE, 0xEE, 0xF8, 0x06},
    {0xE8, 0xFA, 0xF8, 0xE8, 0xF8, 0xE9, 0xEE, 0xF9},
    {0xE5, 0xE9, 0xF0, 0x00, 0x00, 0xEF, 0xF3, 0xF8},
    {0xF7, 0xFB, 0xFB, 0xF7, 0xF9, 0xF9, 0xF5, 0xF0},
    {0xFD, 0xFF, 0xF2, 0xEE, 0xF2, 0xF5, 0xF1, 0xF3}};

static const uint8_t g_3a_pvcTab1_mode2[PVC_NTAB1][PVC_NBLOW][PVC_NBHIGH_MODE2] =
    {{{0x11, 0x27, 0x0F, 0xFD, 0x04, 0xFC},
      {0x00, 0xBE, 0xE3, 0xF4, 0xDB, 0xF0},
      {0x09, 0x1E, 0x18, 0x1A, 0x21, 0x1B}},
     {{0x16, 0x28, 0x2B, 0x29, 0x25, 0x32},
      {0xF2, 0xE9, 0xE4, 0xE5, 0xE2, 0xD4},
      {0x0E, 0x0B, 0x0C, 0x0D, 0x0D, 0x0E}},
     {{0x2E, 0x3C, 0x20, 0x16, 0x1B, 0x1A},
      {0xE4, 0xC6, 0xE5, 0xF4, 0xDC, 0xDC},
      {0x0F, 0x1B, 0x18, 0x14, 0x1E, 0x1A}}};

static const uint8_t g_2a_pvcTab2_mode2[PVC_NTAB2][PVC_NBHIGH_MODE2] = {
    {0x26, 0x25, 0x11, 0x0C, 0xFA, 0x15}, {0x1B, 0x18, 0x11, 0x0E, 0x0E, 0x0E},
    {0x12, 0x10, 0x10, 0x10, 0x11, 0x10}, {0x1E, 0x24, 0x19, 0x15, 0x14, 0x12},
    {0x24, 0x16, 0x12, 0x13, 0x15, 0x1C}, {0xEA, 0xED, 0xEB, 0xEA, 0xEC, 0xEB},
    {0xFC, 0xFD, 0xFD, 0xFC, 0xFE, 0xFE}, {0x0F, 0x0C, 0x0B, 0x0A, 0x0B, 0x0B},
    {0x22, 0x0B, 0x16, 0x18, 0x13, 0x19}, {0x1C, 0x14, 0x1D, 0x20, 0x19, 0x1A},
    {0x10, 0x08, 0x00, 0xFF, 0x02, 0x05}, {0x06, 0x07, 0x05, 0x03, 0x05, 0x04},
    {0x2A, 0x1F, 0x12, 0x12, 0x11, 0x18}, {0x19, 0x19, 0x02, 0x04, 0x00, 0x04},
    {0x18, 0x17, 0x17, 0x15, 0x16, 0x15}, {0x21, 0x1E, 0x1B, 0x19, 0x1C, 0x1B},
    {0x3C, 0x35, 0x20, 0x1D, 0x30, 0x34}, {0x3A, 0x1F, 0x37, 0x38, 0x33, 0x31},
    {0x37, 0x34, 0x25, 0x27, 0x35, 0x34}, {0x34, 0x2E, 0x32, 0x31, 0x34, 0x31},
    {0x36, 0x33, 0x2F, 0x2F, 0x32, 0x2F}, {0x35, 0x20, 0x2F, 0x32, 0x2F, 0x2C},
    {0x2E, 0x2B, 0x2F, 0x34, 0x36, 0x30}, {0x3F, 0x39, 0x30, 0x28, 0x29, 0x29},
    {0x3C, 0x30, 0x32, 0x37, 0x39, 0x36}, {0x37, 0x36, 0x30, 0x2B, 0x26, 0x24},
    {0x44, 0x38, 0x2F, 0x2D, 0x2D, 0x2D}, {0x38, 0x2B, 0x2C, 0x2C, 0x30, 0x2D},
    {0x37, 0x36, 0x2F, 0x23, 0x2D, 0x32}, {0x3C, 0x39, 0x29, 0x2E, 0x38, 0x37},
    {0x3B, 0x3A, 0x35, 0x32, 0x31, 0x2D}, {0x32, 0x31, 0x2F, 0x2C, 0x2D, 0x28},
    {0x2C, 0x31, 0x32, 0x30, 0x32, 0x2D}, {0x35, 0x34, 0x34, 0x34, 0x35, 0x33},
    {0x34, 0x38, 0x3B, 0x3C, 0x3E, 0x3A}, {0x3E, 0x3C, 0x3B, 0x3A, 0x3C, 0x39},
    {0x3D, 0x41, 0x46, 0x41, 0x3D, 0x38}, {0x44, 0x41, 0x40, 0x3E, 0x3F, 0x3A},
    {0x47, 0x47, 0x47, 0x42, 0x44, 0x40}, {0x4C, 0x4A, 0x4A, 0x46, 0x49, 0x45},
    {0x53, 0x52, 0x52, 0x4C, 0x4E, 0x49}, {0x41, 0x3D, 0x39, 0x2C, 0x2E, 0x2E},
    {0x2D, 0x37, 0x36, 0x30, 0x28, 0x36}, {0x3B, 0x32, 0x2E, 0x2D, 0x2D, 0x29},
    {0x40, 0x39, 0x36, 0x35, 0x36, 0x32}, {0x30, 0x2D, 0x2D, 0x2E, 0x31, 0x30},
    {0x38, 0x3D, 0x3B, 0x37, 0x35, 0x34}, {0x44, 0x3D, 0x3C, 0x38, 0x37, 0x33},
    {0x3A, 0x36, 0x37, 0x37, 0x39, 0x36}, {0x32, 0x36, 0x37, 0x30, 0x2E, 0x2A},
    {0x3C, 0x33, 0x33, 0x31, 0x33, 0x30}, {0x30, 0x31, 0x36, 0x37, 0x38, 0x34},
    {0x26, 0x27, 0x2E, 0x29, 0x1C, 0x16}, {0x14, 0x15, 0x1F, 0x17, 0x15, 0x1C},
    {0x38, 0x2D, 0x18, 0x13, 0x1E, 0x2B}, {0x30, 0x22, 0x17, 0x1A, 0x26, 0x2B},
    {0x24, 0x20, 0x1F, 0x10, 0x0C, 0x11}, {0x27, 0x1F, 0x13, 0x17, 0x24, 0x2A},
    {0x2F, 0x13, 0x18, 0x13, 0x2A, 0x32}, {0x31, 0x1E, 0x1E, 0x1E, 0x21, 0x28},
    {0x2A, 0x12, 0x19, 0x17, 0x16, 0x24}, {0x27, 0x0F, 0x16, 0x1D, 0x17, 0x1C},
    {0x2F, 0x26, 0x25, 0x22, 0x20, 0x22}, {0x1E, 0x1B, 0x1E, 0x18, 0x1E, 0x24},
    {0x31, 0x26, 0x0E, 0x15, 0x15, 0x25}, {0x2D, 0x22, 0x1E, 0x14, 0x10, 0x22},
    {0x25, 0x1B, 0x18, 0x11, 0x13, 0x1F}, {0x2F, 0x1B, 0x13, 0x1B, 0x18, 0x22},
    {0x21, 0x24, 0x1D, 0x1C, 0x1D, 0x1B}, {0x23, 0x1E, 0x28, 0x29, 0x27, 0x25},
    {0x2E, 0x2A, 0x1D, 0x17, 0x26, 0x2D}, {0x31, 0x2C, 0x1A, 0x0E, 0x1A, 0x24},
    {0x26, 0x16, 0x20, 0x1D, 0x14, 0x1E}, {0x29, 0x20, 0x1B, 0x1B, 0x17, 0x17},
    {0x1D, 0x06, 0x1A, 0x1E, 0x1B, 0x1D}, {0x2B, 0x23, 0x1F, 0x1F, 0x1D, 0x1C},
    {0x27, 0x1A, 0x0C, 0x0E, 0x0F, 0x1A}, {0x29, 0x1D, 0x1E, 0x22, 0x22, 0x24},
    {0x20, 0x21, 0x1B, 0x18, 0x13, 0x21}, {0x27, 0x0E, 0x10, 0x14, 0x10, 0x1A},
    {0x26, 0x24, 0x25, 0x25, 0x26, 0x28}, {0x1A, 0x24, 0x25, 0x29, 0x26, 0x24},
    {0x1D, 0x1D, 0x15, 0x12, 0x0F, 0x18}, {0x1E, 0x14, 0x13, 0x12, 0x14, 0x18},
    {0x16, 0x13, 0x13, 0x1A, 0x1B, 0x1D}, {0x20, 0x27, 0x22, 0x24, 0x1A, 0x19},
    {0x1F, 0x17, 0x19, 0x18, 0x17, 0x18}, {0x20, 0x1B, 0x1C, 0x1C, 0x1B, 0x1A},
    {0x23, 0x19, 0x1D, 0x1F, 0x1E, 0x21}, {0x26, 0x1F, 0x1D, 0x1B, 0x19, 0x1A},
    {0x23, 0x1E, 0x1F, 0x20, 0x1F, 0x1E}, {0x29, 0x20, 0x22, 0x20, 0x20, 0x1F},
    {0x26, 0x23, 0x21, 0x22, 0x23, 0x23}, {0x29, 0x1F, 0x24, 0x25, 0x26, 0x29},
    {0x2B, 0x22, 0x25, 0x27, 0x23, 0x21}, {0x29, 0x21, 0x19, 0x0E, 0x22, 0x2D},
    {0x32, 0x29, 0x1F, 0x1C, 0x1B, 0x21}, {0x1E, 0x1A, 0x1E, 0x24, 0x25, 0x25},
    {0x24, 0x1D, 0x21, 0x22, 0x22, 0x25}, {0x2C, 0x25, 0x21, 0x22, 0x23, 0x25},
    {0x24, 0x1E, 0x21, 0x26, 0x2B, 0x2C}, {0x28, 0x24, 0x1B, 0x1F, 0x28, 0x2D},
    {0x23, 0x13, 0x16, 0x22, 0x22, 0x29}, {0x1B, 0x23, 0x1C, 0x20, 0x14, 0x0D},
    {0x1E, 0x16, 0x1A, 0x1E, 0x1C, 0x1D}, {0x2B, 0x1C, 0x1D, 0x20, 0x1B, 0x1C},
    {0x1C, 0x1B, 0x23, 0x1F, 0x19, 0x1E}, {0x21, 0x23, 0x26, 0x20, 0x20, 0x22},
    {0x1D, 0x0B, 0x19, 0x1E, 0x11, 0x19}, {0x18, 0x17, 0x16, 0x17, 0x14, 0x16},
    {0x16, 0x19, 0x1C, 0x20, 0x21, 0x22}, {0x30, 0x1E, 0x22, 0x24, 0x25, 0x26},
    {0x1B, 0x1F, 0x17, 0x1D, 0x1E, 0x21}, {0x32, 0x2B, 0x27, 0x1F, 0x1B, 0x1A},
    {0x28, 0x20, 0x1A, 0x1B, 0x1F, 0x23}, {0x32, 0x21, 0x20, 0x21, 0x1D, 0x1F},
    {0x22, 0x18, 0x12, 0x15, 0x1B, 0x20}, {0x27, 0x27, 0x2A, 0x24, 0x21, 0x21},
    {0x1E, 0x0F, 0x0D, 0x1A, 0x1D, 0x23}, {0x28, 0x25, 0x27, 0x21, 0x17, 0x25},
    {0x2B, 0x27, 0x23, 0x19, 0x13, 0x14}, {0x25, 0x2B, 0x22, 0x22, 0x20, 0x21},
    {0x27, 0x1B, 0x16, 0x17, 0x0F, 0x15}, {0x29, 0x26, 0x23, 0x15, 0x1E, 0x28},
    {0x24, 0x1C, 0x19, 0x1A, 0x18, 0x19}, {0x2D, 0x15, 0x27, 0x2B, 0x24, 0x23},
    {0x2C, 0x12, 0x1F, 0x23, 0x1F, 0x20}, {0x25, 0x0F, 0x22, 0x27, 0x1F, 0x21}};

static const uint8_t g_a_pvcTab1_dp_mode1[PVC_NTAB1 - 1] = {17, 68};
static const uint8_t g_a_pvcTab1_dp_mode2[PVC_NTAB1 - 1] = {16, 52};
/* fractional exponent which corresponds to Q representation value */
static const int8_t g_a_scalingCoef_mode1[PVC_NBLOW + 1] = {
    -1, -1, 0, 6}; /* { 8, 8, 7, 1 }; Q scaling */
static const int8_t g_a_scalingCoef_mode2[PVC_NBLOW + 1] = {
    0, 0, 1, 7}; /* { 7, 7, 6, 0 }; Q scaling */

int32_t pvcInitFrame(PVC_STATIC_DATA *pPvcStaticData,
                 PVC_DYNAMIC_DATA *pPvcDynamicData, const uint8_t pvcMode,
                 const uint8_t ns, const int32_t RATE, const int32_t kx,
                 const int32_t pvcBorder0, const uint8_t *pPvcID) {
  int32_t lbw, hbw, i, temp;
  pPvcDynamicData->pvc_mode = pvcMode;
  pPvcDynamicData->kx = kx;
  pPvcDynamicData->RATE = RATE;

  switch (pvcMode) {
    case 0:
      /* legacy SBR, nothing to do */
      return 0;
    case 1:
      pPvcDynamicData->nbHigh = 8;
      pPvcDynamicData->pPVCTab1 = (const uint8_t *)g_3a_pvcTab1_mode1;
      pPvcDynamicData->pPVCTab2 = (const uint8_t *)g_2a_pvcTab2_mode1;
      pPvcDynamicData->pPVCTab1_dp = g_a_pvcTab1_dp_mode1;
      pPvcDynamicData->pScalingCoef = g_a_scalingCoef_mode1;
      hbw = 8 / RATE;
      break;
    case 2:
      pPvcDynamicData->nbHigh = 6;
      pPvcDynamicData->pPVCTab1 = (const uint8_t *)g_3a_pvcTab1_mode2;
      pPvcDynamicData->pPVCTab2 = (const uint8_t *)g_2a_pvcTab2_mode2;
      pPvcDynamicData->pPVCTab1_dp = g_a_pvcTab1_dp_mode2;
      pPvcDynamicData->pScalingCoef = g_a_scalingCoef_mode2;
      hbw = 12 / RATE;
      break;
    default:
      /* invalid pvcMode */
      return 1;
  }

  pPvcDynamicData->pvcBorder0 = pvcBorder0;
  uint8_t pvcBorder0_last = pPvcStaticData->pvcBorder0;
  pPvcStaticData->pvcBorder0 = pvcBorder0;
  pPvcDynamicData->pPvcID = pPvcID;

  pPvcDynamicData->ns = ns;
  switch (ns) {
    case 16:
      pPvcDynamicData->pSCcoeffs = pvc_SC_16;
      break;
    case 12:
      pPvcDynamicData->pSCcoeffs = pvc_SC_12;
      break;
    case 4:
      pPvcDynamicData->pSCcoeffs = pvc_SC_4;
      break;
    case 3:
      pPvcDynamicData->pSCcoeffs = pvc_SC_3;
      break;
    default:
      return 1;
  }

  /* in the lower part of Esg-array there are previous values of Esg (from last
     call to this function In case of an previous legay-SBR frame, or if there
     was a change in cross-over FQ the value of first PVC SBR timeslot is
     propagated to prev-values in order to have reasonable values for
     smooth-filtering
  */
  if ((pPvcStaticData->pvc_mode_last == 0) || (pPvcStaticData->kx_last != kx)) {
    pPvcDynamicData->pastEsgSlotsAvail = 0;
  } else {
    pPvcDynamicData->pastEsgSlotsAvail = PVC_NS_MAX - pvcBorder0_last;
  }

  lbw = 8 / RATE;

  temp = kx;
  for (i = PVC_NBLOW; i >= 0; i--) {
    pPvcDynamicData->sg_offset_low[i] = temp;
    temp -= lbw;
  }

  temp = 0;
  for (i = 0; i <= pPvcDynamicData->nbHigh; i++) {
    pPvcDynamicData->sg_offset_high_kx[i] = temp;
    temp += hbw;
  }

  return 0;
}

/* call if pvcMode = 1,2 */
void pvcDecodeFrame(PVC_STATIC_DATA *pPvcStaticData,
                    PVC_DYNAMIC_DATA *pPvcDynamicData, int32_t **qmfBufferReal,
                    int32_t **qmfBufferImag, const int32_t overlap,
                    const int32_t qmfExponentOverlap,
                    const int32_t qmfExponentCurrent) {
  int32_t t;
  int32_t *predictedEsgSlot;
  int32_t RATE = pPvcDynamicData->RATE;
  int32_t pvcBorder0 = pPvcDynamicData->pvcBorder0;

  for (t = pvcBorder0; t < PVC_NTIMESLOT; t++) {
    int32_t *pPredEsg_exp = &pPvcDynamicData->predEsg_exp[t];
    predictedEsgSlot = pPvcDynamicData->predEsg[t];

    pvcDecodeTimeSlot(
        pPvcStaticData, pPvcDynamicData, &qmfBufferReal[t * RATE],
        &qmfBufferImag[t * RATE],
        (t * RATE < overlap) ? qmfExponentOverlap : qmfExponentCurrent,
        pvcBorder0, t, predictedEsgSlot, pPredEsg_exp);
  }

  return;
}

void pvcDecodeTimeSlot(PVC_STATIC_DATA *pPvcStaticData,
                       PVC_DYNAMIC_DATA *pPvcDynamicData,
                       int32_t **qmfSlotReal, int32_t **qmfSlotImag,
                       const int32_t qmfExponent, const int32_t pvcBorder0,
                       const int32_t timeSlotNumber, int32_t predictedEsgSlot[],
                       int32_t *predictedEsg_exp) {
  int32_t i, band, ksg, ksg_start = 0;
  int32_t RATE = pPvcDynamicData->RATE;
  int32_t Esg_index = pPvcStaticData->Esg_slot_index;
  const int8_t *sg_borders = pPvcDynamicData->sg_offset_low;
  int32_t *pEsg = pPvcStaticData->Esg[Esg_index];
  int32_t E[PVC_NBLOW] = {0};

  /* Subband grouping in QMF subbands below SBR range */
  /* Within one timeslot ( i = [0...(RATE-1)] QMF subsamples) calculate energy
     E(ib,t) and group them to Esg(ksg,t). Then transfer values to logarithmical
     domain and store them for time domain smoothing. (7.5.6.3 Subband grouping
     in QMF subbands below SBR range)
  */
  for (ksg = 0; sg_borders[ksg] < 0; ksg++) {
    pEsg[ksg] = FL2FXCONST_DBL(-10.0 / (1 << PVC_ESG_EXP)); /* 10*log10(0.1) */
    ksg_start++;
  }

  for (i = 0; i < RATE; i++) {
    int32_t *qmfR, *qmfI;
    qmfR = qmfSlotReal[i];
    qmfI = qmfSlotImag[i];
    for (ksg = ksg_start; ksg < PVC_NBLOW; ksg++) {
      for (band = sg_borders[ksg]; band < sg_borders[ksg + 1]; band++) {
        /* The division by 8 == (RATE*lbw) is required algorithmically */
        E[ksg] +=
            ((fPow2Div2(qmfR[band]) >> 1) + (fPow2Div2(qmfI[band]) >> 1)) >> 3;
      }
    }
  }
  for (ksg = ksg_start; ksg < PVC_NBLOW; ksg++) {
    if (E[ksg] > (int32_t)0) {
      /* 10/log2(10) = 0.752574989159953 * 2^2 */
      int32_t exp_log;
      int32_t nrg = CalcLog2(E[ksg], 2 * qmfExponent + 2, &exp_log);
      nrg = fMult(nrg, (int16_t)24660);
      nrg = scaleValue(nrg, exp_log - PVC_ESG_EXP + 2);
      pEsg[ksg] = fMax(nrg, FL2FXCONST_DBL(-10.0 / (1 << PVC_ESG_EXP)));
    } else {
      pEsg[ksg] =
          FL2FXCONST_DBL(-10.0 / (1 << PVC_ESG_EXP)); /* 10*log10(0.1) */
    }
  }

  /* Time domain smoothing of subband-grouped energy */
  {
    int32_t idx = pPvcStaticData->Esg_slot_index;
    int32_t *pEsg_filt;
    int16_t SCcoeff;

    E[0] = E[1] = E[2] = (int32_t)0;
    for (i = 0; i < pPvcDynamicData->ns; i++) {
      SCcoeff = pPvcDynamicData->pSCcoeffs[i];
      pEsg_filt = pPvcStaticData->Esg[idx];
      /* Div2 is compensated by scaling of coeff table */
      E[0] = fMultAddDiv2(E[0], pEsg_filt[0], SCcoeff);
      E[1] = fMultAddDiv2(E[1], pEsg_filt[1], SCcoeff);
      E[2] = fMultAddDiv2(E[2], pEsg_filt[2], SCcoeff);
      if (i >= pPvcDynamicData->pastEsgSlotsAvail) {
        /* if past Esg values are not available use the ones from the last valid
         * slot */
        continue;
      }
      if (idx > 0) {
        idx--;
      } else {
        idx += PVC_NS_MAX - 1;
      }
    }
  }

  /* SBR envelope scalefactor prediction */
  {
    int32_t E_high_exp[PVC_NBHIGH_MAX];
    int32_t E_high_exp_max = 0;
    int32_t pvcTab1ID;
    int32_t pvcTab2ID = (int32_t)pPvcDynamicData->pPvcID[timeSlotNumber];
    const uint8_t *pTab1, *pTab2;
    if (pvcTab2ID < pPvcDynamicData->pPVCTab1_dp[0]) {
      pvcTab1ID = 0;
    } else if (pvcTab2ID < pPvcDynamicData->pPVCTab1_dp[1]) {
      pvcTab1ID = 1;
    } else {
      pvcTab1ID = 2;
    }
    pTab1 = &(pPvcDynamicData
                  ->pPVCTab1[pvcTab1ID * PVC_NBLOW * pPvcDynamicData->nbHigh]);
    pTab2 = &(pPvcDynamicData->pPVCTab2[pvcTab2ID * pPvcDynamicData->nbHigh]);
    for (ksg = 0; ksg < pPvcDynamicData->nbHigh; ksg++) {
      int16_t predCoeff;
      int32_t accu;
      int32_t predCoeff_exp, kb;
      E_high_exp[ksg] = 0;

      /* residual part */
      accu = ((int32_t)(int8_t)*pTab2++) << (DFRACT_BITS - 8 - PVC_ESG_EXP - 2 +
                                         pPvcDynamicData->pScalingCoef[3]);

      /* linear combination of lower grouped energies part */
      for (kb = 0; kb < PVC_NBLOW; kb++) {
        predCoeff = (int16_t)(
            (int16_t)(int8_t)pTab1[kb * pPvcDynamicData->nbHigh + ksg] << 8);
        predCoeff_exp = -(pPvcDynamicData->pScalingCoef[kb] + 1 -
                          2); /* +1 to compensate for Div2; -2 for accu */
        accu += fMultDiv2(E[kb], predCoeff) >> predCoeff_exp;
      }
      /* convert back to linear domain */
      accu = fMult(accu, (int16_t)21771);
      accu = f2Pow(accu, PVC_ESG_EXP - 1 + 2,
                   &predCoeff_exp); /* -1 compensates for exponent of
                                       LOG10FAC_INV; +2 for accu */
      predictedEsgSlot[ksg] = accu;
      E_high_exp[ksg] = predCoeff_exp;
      if (predCoeff_exp > E_high_exp_max) {
        E_high_exp_max = predCoeff_exp;
      }
    }

    /* rescale output vector according to largest exponent */
    for (ksg = 0; ksg < pPvcDynamicData->nbHigh; ksg++) {
      int32_t scale = fMin(E_high_exp_max - E_high_exp[ksg], DFRACT_BITS - 1);
      predictedEsgSlot[ksg] = predictedEsgSlot[ksg] >> scale;
    }
    *predictedEsg_exp = E_high_exp_max;
  }

  pPvcStaticData->Esg_slot_index =
      (pPvcStaticData->Esg_slot_index + 1) & (PVC_NS_MAX - 1);
  pPvcDynamicData->pastEsgSlotsAvail =
      fMin(pPvcDynamicData->pastEsgSlotsAvail + 1, PVC_NS_MAX - 1);
  return;
}

/* call if pvcMode = 0,1,2 */
void pvcEndFrame(PVC_STATIC_DATA *pPvcStaticData,
                 PVC_DYNAMIC_DATA *pPvcDynamicData) {
  pPvcStaticData->pvc_mode_last = pPvcDynamicData->pvc_mode;
  pPvcStaticData->kx_last = pPvcDynamicData->kx;

  if (pPvcDynamicData->pvc_mode == 0) return;

  {
    int32_t t, max = -100;
    for (t = pPvcDynamicData->pvcBorder0; t < PVC_NTIMESLOT; t++) {
      if (pPvcDynamicData->predEsg_exp[t] > max) {
        max = pPvcDynamicData->predEsg_exp[t];
      }
    }
    pPvcDynamicData->predEsg_expMax = max;
  }
  return;
}

void expandPredEsg(const PVC_DYNAMIC_DATA *pPvcDynamicData, const int32_t timeSlot,
                   const int32_t lengthOutputVector, int32_t *pOutput,
                   int8_t *pOutput_exp) {
  int32_t k = 0, ksg;
  const int32_t *predEsg = pPvcDynamicData->predEsg[timeSlot];

  for (ksg = 0; ksg < pPvcDynamicData->nbHigh; ksg++) {
    for (; k < pPvcDynamicData->sg_offset_high_kx[ksg + 1]; k++) {
      pOutput[k] = predEsg[ksg];
      pOutput_exp[k] = (int8_t)pPvcDynamicData->predEsg_exp[timeSlot];
    }
  }
  ksg--;
  for (; k < lengthOutputVector; k++) {
    pOutput[k] = predEsg[ksg];
    pOutput_exp[k] = (int8_t)pPvcDynamicData->predEsg_exp[timeSlot];
  }

  return;
}
