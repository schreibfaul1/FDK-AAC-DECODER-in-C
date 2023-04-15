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

   Author(s):

   Description:

*******************************************************************************/

/*!
  \file
  \brief  Definition of constant tables

  This module contains most of the constant data that can be stored in ROM.
*/

#include "sbr_rom.h"

/*!
  \name   StartStopBands
  \brief  Start and stop subbands of the highband.

  k_o = startMin + offset[bs_start_freq];
  startMin = {3000,4000,5000} * (128/FS_sbr) / FS_sbr < 32Khz, 32Khz <= FS_sbr <
  64KHz, 64KHz <= FS_sbr The stop subband can also be calculated to save memory
  by defining #CALC_STOP_BAND.
*/
//@{
/* tables were created with ../addon/octave/sbr_start_freq_table.m */
const uint8_t FDK_sbrDecoder_sbr_start_freq_16[][16] = {
    {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31},
    {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19}};
const uint8_t FDK_sbrDecoder_sbr_start_freq_22[][16] = {
    {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 26, 28, 30},
    {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22}};
const uint8_t FDK_sbrDecoder_sbr_start_freq_24[][16] = {
    {11, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 25, 27, 29, 32},
    {3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 19, 21, 24}};
const uint8_t FDK_sbrDecoder_sbr_start_freq_32[][16] = {
    {10, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 25, 27, 29, 32},
    {2, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 19, 21, 24}};
const uint8_t FDK_sbrDecoder_sbr_start_freq_40[][16] = {
    {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 26, 28, 30, 32},
    {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 19, 21, 23, 25}};
const uint8_t FDK_sbrDecoder_sbr_start_freq_44[][16] = {{8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 21, 23, 25, 28, 32},
                                                        {2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 17, 19, 22, 26}};
const uint8_t FDK_sbrDecoder_sbr_start_freq_48[][16] = {{7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 20, 22, 24, 27, 31},
                                                        {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 21, 25}};
const uint8_t FDK_sbrDecoder_sbr_start_freq_64[][16] = {{6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 21, 23, 26, 30},
                                                        {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 21, 25}};
const uint8_t FDK_sbrDecoder_sbr_start_freq_88[][16] = {{5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 18, 20, 23, 27, 31},
                                                        {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 20, 24, 28}};
const uint8_t FDK_sbrDecoder_sbr_start_freq_192[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 19, 23, 27};
const uint8_t FDK_sbrDecoder_sbr_start_freq_176[16] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 20, 24, 28};
const uint8_t FDK_sbrDecoder_sbr_start_freq_128[16] = {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 21, 25};

//@}

/*!
  \name   Whitening
  \brief  Coefficients for spectral whitening in the transposer
*/
//@{
/*! Assignment of whitening tuning depending on the crossover frequency */
const uint16_t FDK_sbrDecoder_sbr_whFactorsIndex[NUM_WHFACTOR_TABLE_ENTRIES] = {0,    5000, 6000, 6500, 7000,
                                                                                7500, 8000, 9000, 10000};

/*!
  \brief Whithening levels tuning table

  With the current tuning, there are some redundant entries:

  \li  NUM_WHFACTOR_TABLE_ENTRIES can be reduced by 3,
  \li  the first coloumn can be eliminated.

*/
const int32_t FDK_sbrDecoder_sbr_whFactorsTable[NUM_WHFACTOR_TABLE_ENTRIES][6] = {
    /* OFF_LEVEL, TRANSITION_LEVEL, LOW_LEVEL, MID_LEVEL, HIGH_LEVEL */
    {FL2FXCONST_DBL(0.00f), FL2FXCONST_DBL(0.6f), FL2FXCONST_DBL(0.75f), FL2FXCONST_DBL(0.90f),
     FL2FXCONST_DBL(0.98f)}, /* < 5000 */
    {FL2FXCONST_DBL(0.00f), FL2FXCONST_DBL(0.6f), FL2FXCONST_DBL(0.75f), FL2FXCONST_DBL(0.90f),
     FL2FXCONST_DBL(0.98f)}, /* 5000 < 6000 */
    {FL2FXCONST_DBL(0.00f), FL2FXCONST_DBL(0.6f), FL2FXCONST_DBL(0.75f), FL2FXCONST_DBL(0.90f),
     FL2FXCONST_DBL(0.98f)}, /* 6000 < 6500 */
    {FL2FXCONST_DBL(0.00f), FL2FXCONST_DBL(0.6f), FL2FXCONST_DBL(0.75f), FL2FXCONST_DBL(0.90f),
     FL2FXCONST_DBL(0.98f)}, /* 6500 < 7000 */
    {FL2FXCONST_DBL(0.00f), FL2FXCONST_DBL(0.6f), FL2FXCONST_DBL(0.75f), FL2FXCONST_DBL(0.90f),
     FL2FXCONST_DBL(0.98f)}, /* 7000 < 7500 */
    {FL2FXCONST_DBL(0.00f), FL2FXCONST_DBL(0.6f), FL2FXCONST_DBL(0.75f), FL2FXCONST_DBL(0.90f),
     FL2FXCONST_DBL(0.98f)}, /* 7500 < 8000 */
    {FL2FXCONST_DBL(0.00f), FL2FXCONST_DBL(0.6f), FL2FXCONST_DBL(0.75f), FL2FXCONST_DBL(0.90f),
     FL2FXCONST_DBL(0.98f)}, /* 8000 < 9000 */
    {FL2FXCONST_DBL(0.00f), FL2FXCONST_DBL(0.6f), FL2FXCONST_DBL(0.75f), FL2FXCONST_DBL(0.90f),
     FL2FXCONST_DBL(0.98f)}, /* 9000 < 10000 */
    {FL2FXCONST_DBL(0.00f), FL2FXCONST_DBL(0.6f), FL2FXCONST_DBL(0.75f), FL2FXCONST_DBL(0.90f),
     FL2FXCONST_DBL(0.98f)}, /* > 10000 */
};

//@}

/*!
  \name   EnvAdj
  \brief  Constants and tables used for envelope adjustment
*/
//@{

/*! Mantissas of gain limits */
const int16_t FDK_sbrDecoder_sbr_limGains_m[4] = {
    16423, /*!< -3 dB. Gain limit when limiterGains in frameData is 0 */
    16384, /*!< 0 dB.  Gain limit when limiterGains in frameData is 1 */
    32690, /*!< +3 dB. Gain limit when limiterGains in frameData is 2 */
    22204  /*!< Inf.   Gain limit when limiterGains in frameData is 3 */
};

/*! Exponents of gain limits */
const uint8_t FDK_sbrDecoder_sbr_limGains_e[4] = {0, 1, 1, 67};

/*! Constants for calculating the number of limiter bands */
const int16_t FDK_sbrDecoder_sbr_limiterBandsPerOctaveDiv4[4] = {
    FL2FXCONST_SGL(1.0f / 4.0f), FL2FXCONST_SGL(1.2f / 4.0f), FL2FXCONST_SGL(2.0f / 4.0f), FL2FXCONST_SGL(3.0f / 4.0f)};

/*! Constants for calculating the number of limiter bands */
const int32_t FDK_sbrDecoder_sbr_limiterBandsPerOctaveDiv4_DBL[4] = {
    FL2FXCONST_DBL(1.0f / 4.0f), FL2FXCONST_DBL(1.2f / 4.0f), FL2FXCONST_DBL(2.0f / 4.0f), FL2FXCONST_DBL(3.0f / 4.0f)};

/*! Ratio of old gains and noise levels for the first 4 timeslots of an envelope
 */
const int16_t FDK_sbrDecoder_sbr_smoothFilter[4] = {21845, 11966, 4817, 1043};

/*! Real and imaginary part of random noise which will be modulated
  to the desired level. An accuracy of 13 bits is sufficient for these
  random numbers.
*/
const int16_t FDK_sbrDecoder_sbr_randomPhase[SBR_NF_NO_RANDOM_VAL][2] = {
    {-32751, -19492}, {31822, -22128},     {4630, -31159},   {-15403, -12236}, {26445, 9717},    {-12773, 29351},
    {-345, -21941},   {-29906, -3776},     {17970, 24649},   {13110, -32417},  {-32725, -28884}, {-31304, 29789},
    {-14983, -18585}, {-23898, -32115},    {24780, 6865},    {2317, -25640},   {24411, -29874},  {-31602, -31044},
    {9970, -16200},   {21812, 21185},      {30047, 5739},    {-23192, 17219},  {-22954, -14857}, {-32603, -29515},
    {32167, -25383},  {-17915, -842},      {-554, 94},       {-28217, 13942},  {-32405, -28797}, {16960, 21931},
    {-32648, -19041}, {-32758, 32234},     {18110, 19480},   {11332, 31090},   {20534, -24380},  {-25280, -11103},
    {-30013, 1208},   {-24997, -29941},    {26145, -30534},  {17850, -3906},   {-28062, 13903},  {-30436, 9133},
    {-3837, -32703},  {6998, -29726},      {-24966, 32692},  {32149, -31410},  {-28152, 31381},  {-30575, 16198},
    {9990, -23115},   {27948, 15324},      {29926, -32715},  {-1930, 23181},   {9306, 11349},    {31214, -17987},
    {-25745, -24762}, {-31388, -6692},     {27004, 31672},   {-21360, -29077}, {-30685, 32726},  {29959, -32208},
    {-23067, 19267},  {185, 20240},        {29185, 17296},   {-22506, 26479},  {23647, -22695},  {-20620, 4465},
    {9810, -15090},   {-30075, -24252},    {32538, 13375},   {26990, -24260},  {-32281, -32759}, {-31430, -32518},
    {-7016, -30613},  {-22551, -8812},     {30098, 3067},    {-31478, 11829},  {16923, -23388},  {20031, 15385},
    {15511, -8957},   {29818, 31692},      {14695, 32510},   {21828, 31651},   {24551, -29452},  {-32628, 17297},
    {31916, -5523},   {23817, -15749},     {31271, 22561},   {-23908, -25103}, {-27971, 29078},  {-26677, -31942},
    {-28813, 24494},  {-23453, -32300},    {27369, 27428},   {-15757, -32391}, {31831, 26245},   {17037, 26296},
    {-278, -25123},   {-23034, 18140},     {-31423, -14177}, {31811, 3056},    {-30279, 28019},  {-22776, 32320},
    {8703, 24024},    {9188, 4764},        {-24294, 32542},  {-574, -27072},   {-18064, -32407}, {32100, -30809},
    {-32505, 21961},  {-22179, 4139},      {2995, -6730},    {-23481, -32043}, {26547, 17604},   {13309, -8673},
    {-22177, 30966},  {28459, -6008},      {-32604, -863},   {27633, 3410},    {-3020, 22787},   {32754, -4050},
    {-26127, -30010}, {31572, 31667},      {-26196, 21078},  {-3790, 9368},    {-13082, 30844},  {32470, -30167},
    {9382, -29830},   {-27297, -22063},    {31262, 16110},   {-2113, 1065},    {-32629, 13890},  {-21463, 27049},
    {-26625, -16917}, {-32652, 27686},     {94, 21223},      {22996, -6702},   {31576, 13339},   {-22572, 29930},
    {-11428, 23420},  {30140, 21793},      {-32443, 28137},  {22566, 18239},   {-32599, -6571},  {30872, -32669},
    {-22090, 16236},  {-15512, -28149},    {4694, -30850},   {-9591, 1887},    {14350, -25856},  {-11910, 21258},
    {-2867, 32010},   {-31620, -17682},    {18195, 25851},   {24097, 31605},   {-10122, -26432}, {1171, -31892},
    {32349, 15863},   {-26768, -29762},    {22239, 26635},   {-5180, 27944},   {26451, -8099},   {15659, -15182},
    {31578, 12611},   {-32487, -8174},     {27224, -31058},  {-19252, 423},    {31306, -28035},  {-31618, -20978},
    {-31892, 4056},   {29950, 18996},      {-32744, 23293},  {-28467, -6649},  {-8598, -22369},  {-8082, -28719},
    {792, 8911},      {26892, -27882},     {29015, -29372},  {-5955, -8570},   {3066, 17972},    {-17914, 31451},
    {12141, -19631},  {-23060, 29893},     {-11338, -32585}, {-22536, -9909},  {-8796, 27235},   {16080, -14863},
    {12772, 31298},   {-32033, 1739},      {-5677, -30399},  {32751, 19099},   {-21282, 22494},  {-3938, -18726},
    {-19316, -11419}, {-13702, 5333},      {32731, 3649},    {-18563, -29653}, {30847, 11561},   {-24814, 17580},
    {6731, -30945},   {32762, 26161},      {9528, 11598},    {-20598, 12703},  {14235, -32292},  {-32210, 6888},
    {6394, -30881},   {-31286, 32232},     {30599, -23227},  {-27930, -2734},  {-28320, -15006}, {12740, 31875},
    {30161, -20458},  {29217, 18006},      {-12070, 31607},  {30766, -29481},  {32528, -1231},   {-30823, 13544},
    {32683, -5515},   {-11764, -15281},    {1716, -8402},    {12027, -12666},  {30033, -10023},  {22610, 29803},
    {-12668, 32605},  {-9585, 12270},      {-19721, 28436},  {-31922, 31611},  {28987, 18844},   {1704, 6970},
    {-17531, 31864},  {-16197, 32173},     {-32419, -13189}, {-32139, -23874}, {-8958, 32752},   {2068, -17872},
    {-6705, -4656},   {21700, 23766},      {-27776, 777},    {-29177, 29120},  {31426, 25148},   {24086, -1228},
    {-10402, -12070}, {-11177, 13176},     {15664, -12918},  {32211, 652},     {-10146, -5923},  {32766, -8612},
    {-30523, -32215}, {32743, -26261},     {-8528, -24904},  {-11702, 6324},   {-32735, 24460},  {28363, 18217},
    {10947, 28241},   {32444, 1508},       {-21854, -30030}, {20977, 5128},    {32627, 15022},   {-20785, 6907},
    {-2525, -29354},  {32306, 28915},      {26247, -12076},  {25680, 14912},   {2853, 26522},    {-28447, 12893},
    {-12932, -21892}, {32072, -23746},     {-31142, 29348},  {5572, 17919},    {-25202, -31532}, {32684, 13991},
    {31273, 31786},   {32630, -17730},     {9194, -27971},   {27937, -21158},  {-16583, -21576}, {-31854, -7568},
    {31269, -32519},  {-31761, 24175},     {10116, 13604},   {-8036, 20712},   {-11080, -12669}, {-1909, -2274},
    {-7503, 31803},   {-6065, 15586},      {-3437, -28760},  {-23556, 25569},  {32700, 29505},   {18862, -29830},
    {9473, 31558},    {13824, 15777},      {30584, -14266},  {-31814, 28389},  {12033, 21395},   {-26573, 2876},
    {-8599, -30400},  {27524, 18298},      {-32738, -31465}, {24461, 3980},    {-24502, -8814},  {31386, -25902},
    {31284, -2814},   {15961, MAXVAL_SGL}, {15182, 3593},    {-25067, 29233},  {18808, 11564},   {24699, 31688},
    {-19390, -29296}, {24605, -9703},      {-32312, 8203},   {-13357, -29506}, {21931, 32319},   {-31937, -62},
    {29539, 32696},   {-28593, 32517},     {-29992, -5144},  {-1083, -12191},  {2367, -29100},   {32604, 31816},
    {-24545, 32763},  {1502, 32707},       {-29182, -10418}, {-27454, 31994},  {10962, -28256},  {-32672, 30552},
    {-7480, 6185},    {22036, -1195},      {-1686, -30343},  {32751, 30679},   {21939, 32409},   {-32638, -14632},
    {26904, 32618},   {32501, 23600},      {-21392, 17101},  {30764, -24542},  {31698, 29783},   {-7283, 18718},
    {-14461, -30372}, {-28081, 29113},     {30075, -15190},  {23775, -32735},  {-32673, 19075},  {25441, 30907},
    {2529, 19215},    {-18366, 27041},     {32243, 12933},   {15580, 22483},   {21520, 6007},    {1073, -24554},
    {-12676, 16822},  {-31898, -31637},    {-17459, -29958}, {32708, 20032},   {-16467, -29108}, {654, 27926},
    {32745, 30992},   {27167, -2072},      {-19222, 31733},  {-5759, -15783},  {27340, -4268},   {1949, 6721},
    {26708, -31027},  {-14738, 13400},     {-29408, 32718},  {13001, -24528},  {-2487, 24280},   {25016, 13680},
    {-24409, 31040},  {21260, 13545},      {20421, -30506},  {13833, -2527},   {886, -1775},     {26215, 29997},
    {-26002, -11865}, {20930, 2663},       {17331, 19677},   {24326, 1472},    {32472, -6374},   {-26350, -29004},
    {-21172, 23658},  {3820, -27415},      {-31147, -31765}, {-20391, 27121},  {985, -32682},    {-32108, 11969},
    {-32764, -11804}, {29200, -32078},     {3410, 25349},    {31446, -11612},  {16660, 31493},   {5573, -25184},
    {8478, 32733},    {-366, 32275},       {-26083, 31830},  {-32527, -32618}, {-32712, 615},    {-23200, 11036},
    {-23091, 30564},  {32718, -32350},     {-20763, -5398},  {-5327, -31437},  {-14302, -31066}, {-32718, 31538},
    {-5504, -32436},  {-28829, -23503},    {14478, -30661},  {30576, -32740},  {-30783, -18484}, {-29029, 15606},
    {32759, -27489},  {-24699, 267},       {30765, -3698},   {27894, 17154},   {13009, 26798},   {-12132, -28532},
    {-11805, 11356},  {-30602, -27681},    {-21397, -6042},  {3919, 32735},    {30898, 27251},   {24603, -11644},
    {18587, -7889},   {15354, -9876},      {31887, -32620},  {-12550, 32282},  {13443, 694},     {3158, 1446},
    {-27946, 29975},  {29120, -32681},     {-15795, -31721}, {9035, 19213},    {-21591, 19279},  {32387, 32766},
    {-6767, 17889},   {-20358, -19626},    {6658, -28469},   {-32044, 31553},  {3641, 7040},     {-13556, 9246},
    {7908, 16808},    {-21756, -2703},     {-17596, -31998}, {-31859, 7236},   {28637, -4193},   {6242, 525},
    {-15189, -31211}, {-2315, -30959},     {-30292, -3427},  {-27467, -555},   {24646, -32753},  {-13796, 32677},
    {-23624, -11472}, {25835, 17318},      {31914, -8748},   {32508, -18681},  {25162, -25074},  {-26871, -24094},
    {26845, 32669},   {-8756, 22578},      {-14192, 27958},  {32504, 30106},   {-26441, -10691}, {14116, -7182},
    {22187, -31286},  {18400, -23165},     {3549, -2827},    {29894, -21623},  {-16047, 18445},  {-29175, -23480},
    {21387, 21599},   {22099, -26766},     {-15654, -5502},  {-32675, -30660}, {-29783, 20327},  {-2169, -7803},
    {32581, 6164},    {32010, -9393},      {31069, -31951},  {-31272, -26086}, {-16091, 10779},  {32729, 29162},
    {16531, -28179},  {15454, -6121},      {-20343, 24576},  {-14374, 32767},  {32319, -17557},  {-20156, -29332},
    {-1259, -22901},  {-9864, -2513},      {13724, 717},     {-28225, 32423},  {22029, -4422},   {-23179, -25083},
    {30817, 2958},    {-26996, 2924},      {-10509, 16431},  {18872, -32429},  {-11903, 2438},   {32761, -4630},
    {-30266, -32106}, {-14617, -17771},    {14492, 23372},   {1203, 20843},    {17097, -27983},  {-31032, -598},
    {-32362, 26964},  {28651, 29294},      {-30609, 13557},  {31478, 30512},   {31960, 28230},   {32651, 23000},
    {-31033, -9693},  {30015, -32161}};
//@}

/*
static const int16_t harmonicPhase [2][4] = {
  { 1.0, 0.0, -1.0,  0.0},
  { 0.0, 1.0,  0.0, -1.0}
};
*/

/* tables for SBR and AAC LD */
/* table for 8 time slot index */
const int32_t FDK_sbrDecoder_envelopeTable_8[8][5] = {
    /* transientIndex  nEnv, tranIdx, shortEnv, border1, border2, ... */
    /* borders from left to right side; -1 = not in use */
    /*[|T-|------]*/ {2, 0, 0, 1, -1},
    /*[|-T-|-----]*/ {2, 0, 0, 2, -1},
    /*[--|T-|----]*/ {3, 1, 1, 2, 4},
    /*[---|T-|---]*/ {3, 1, 1, 3, 5},
    /*[----|T-|--]*/ {3, 1, 1, 4, 6},
    /*[-----|T--|]*/ {2, 1, 1, 5, -1},
    /*[------|T-|]*/ {2, 1, 1, 6, -1},
    /*[-------|T|]*/ {2, 1, 1, 7, -1},
};

/* table for 15 time slot index */
const int32_t FDK_sbrDecoder_envelopeTable_15[15][6] = {
    /* transientIndex  nEnv, tranIdx, shortEnv, border1, border2, ... */
    /* length from left to right side; -1 = not in use */
    /*[|T---|------------]*/ {2, 0, 0, 4, -1, -1},
    /*[|-T---|-----------]*/ {2, 0, 0, 5, -1, -1},
    /*[|--|T---|---------]*/ {3, 1, 1, 2, 6, -1},
    /*[|---|T---|--------]*/ {3, 1, 1, 3, 7, -1},
    /*[|----|T---|-------]*/ {3, 1, 1, 4, 8, -1},
    /*[|-----|T---|------]*/ {3, 1, 1, 5, 9, -1},
    /*[|------|T---|-----]*/ {3, 1, 1, 6, 10, -1},
    /*[|-------|T---|----]*/ {3, 1, 1, 7, 11, -1},
    /*[|--------|T---|---]*/ {3, 1, 1, 8, 12, -1},
    /*[|---------|T---|--]*/ {3, 1, 1, 9, 13, -1},
    /*[|----------|T----|]*/ {2, 1, 1, 10, -1, -1},
    /*[|-----------|T---|]*/ {2, 1, 1, 11, -1, -1},
    /*[|------------|T--|]*/ {2, 1, 1, 12, -1, -1},
    /*[|-------------|T-|]*/ {2, 1, 1, 13, -1, -1},
    /*[|--------------|T|]*/ {2, 1, 1, 14, -1, -1},
};

/* table for 16 time slot index */
const int32_t FDK_sbrDecoder_envelopeTable_16[16][6] = {
    /* transientIndex  nEnv, tranIdx, shortEnv, border1, border2, ... */
    /* length from left to right side; -1 = not in use */
    /*[|T---|------------|]*/ {2, 0, 0, 4, -1, -1},
    /*[|-T---|-----------|]*/ {2, 0, 0, 5, -1, -1},
    /*[|--|T---|----------]*/ {3, 1, 1, 2, 6, -1},
    /*[|---|T---|---------]*/ {3, 1, 1, 3, 7, -1},
    /*[|----|T---|--------]*/ {3, 1, 1, 4, 8, -1},
    /*[|-----|T---|-------]*/ {3, 1, 1, 5, 9, -1},
    /*[|------|T---|------]*/ {3, 1, 1, 6, 10, -1},
    /*[|-------|T---|-----]*/ {3, 1, 1, 7, 11, -1},
    /*[|--------|T---|----]*/ {3, 1, 1, 8, 12, -1},
    /*[|---------|T---|---]*/ {3, 1, 1, 9, 13, -1},
    /*[|----------|T---|--]*/ {3, 1, 1, 10, 14, -1},
    /*[|-----------|T----|]*/ {2, 1, 1, 11, -1, -1},
    /*[|------------|T---|]*/ {2, 1, 1, 12, -1, -1},
    /*[|-------------|T--|]*/ {2, 1, 1, 13, -1, -1},
    /*[|--------------|T-|]*/ {2, 1, 1, 14, -1, -1},
    /*[|---------------|T|]*/ {2, 1, 1, 15, -1, -1},
};

/*!
  \name FrameInfoDefaults

  Predefined envelope positions for the FIX-FIX case (static framing)
*/
//@{
const FRAME_INFO FDK_sbrDecoder_sbr_frame_info1_15 = {
    0, 1, {0, 15, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, -1, 1, {0, 15, 0}, {0, 0, 0}, 0, 0};
const FRAME_INFO FDK_sbrDecoder_sbr_frame_info2_15 = {
    0, 2, {0, 8, 15, 0, 0, 0}, {1, 1, 0, 0, 0}, -1, 2, {0, 8, 15}, {0, 0, 0}, 0, 0};
const FRAME_INFO FDK_sbrDecoder_sbr_frame_info4_15 = {
    0, 4, {0, 4, 8, 12, 15, 0}, {1, 1, 1, 1, 0}, -1, 2, {0, 8, 15}, {0, 0, 0}, 0, 0};
#if(MAX_ENVELOPES >= 8)
const FRAME_INFO FDK_sbrDecoder_sbr_frame_info8_15 = {
    0, 8, {0, 2, 4, 6, 8, 10, 12, 14, 15}, {1, 1, 1, 1, 1, 1, 1, 1}, -1, 2, {0, 8, 15}, {0, 0, 0}, 0, 0};
#endif

const FRAME_INFO FDK_sbrDecoder_sbr_frame_info1_16 = {
    0, 1, {0, 16, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, -1, 1, {0, 16, 0}, {0, 0, 0}, 0, 0};
const FRAME_INFO FDK_sbrDecoder_sbr_frame_info2_16 = {
    0, 2, {0, 8, 16, 0, 0, 0}, {1, 1, 0, 0, 0}, -1, 2, {0, 8, 16}, {0, 0, 0}, 0, 0};
const FRAME_INFO FDK_sbrDecoder_sbr_frame_info4_16 = {
    0, 4, {0, 4, 8, 12, 16, 0}, {1, 1, 1, 1, 0}, -1, 2, {0, 8, 16}, {0, 0, 0}, 0, 0};

#if(MAX_ENVELOPES >= 8)
const FRAME_INFO FDK_sbrDecoder_sbr_frame_info8_16 = {
    0, 8, {0, 2, 4, 6, 8, 10, 12, 14, 16}, {1, 1, 1, 1, 1, 1, 1, 1}, -1, 2, {0, 8, 16}, {0, 0, 0}, 0, 0};
#endif

//@}

/*!
  \name SBR_HuffmanTables

  SBR Huffman Table Overview:        \n
                                     \n
 o envelope level,   1.5 dB:         \n
    1)  sbr_huffBook_EnvLevel10T[120][2]   \n
    2)  sbr_huffBook_EnvLevel10F[120][2]   \n
                                     \n
 o envelope balance, 1.5 dB:         \n
    3)  sbr_huffBook_EnvBalance10T[48][2]  \n
    4)  sbr_huffBook_EnvBalance10F[48][2]  \n
                                     \n
 o envelope level,   3.0 dB:         \n
    5)  sbr_huffBook_EnvLevel11T[62][2]    \n
    6)  sbr_huffBook_EnvLevel11F[62][2]    \n
                                     \n
 o envelope balance, 3.0 dB:         \n
    7)  sbr_huffBook_EnvBalance11T[24][2]  \n
    8)  sbr_huffBook_EnvBalance11F[24][2]  \n
                                     \n
 o noise level,      3.0 dB:         \n
    9)  sbr_huffBook_NoiseLevel11T[62][2]  \n
    -) (sbr_huffBook_EnvLevel11F[62][2] is used for freq dir)\n
                                     \n
 o noise balance,    3.0 dB:         \n
   10)  sbr_huffBook_NoiseBalance11T[24][2]\n
    -) (sbr_huffBook_EnvBalance11F[24][2] is used for freq dir)\n
                                     \n
  (1.5 dB is never used for noise)

*/
//@{
const int8_t FDK_sbrDecoder_sbr_huffBook_EnvLevel10T[120][2] = {
    {1, 2},       {-64, -65},   {3, 4},       {-63, -66}, {5, 6},       {-62, -67},   {7, 8},       {-61, -68},
    {9, 10},      {-60, -69},   {11, 12},     {-59, -70}, {13, 14},     {-58, -71},   {15, 16},     {-57, -72},
    {17, 18},     {-73, -56},   {19, 21},     {-74, 20},  {-55, -75},   {22, 26},     {23, 24},     {-54, -76},
    {-77, 25},    {-53, -78},   {27, 34},     {28, 29},   {-52, -79},   {30, 31},     {-80, -51},   {32, 33},
    {-83, -82},   {-81, -50},   {35, 57},     {36, 40},   {37, 38},     {-88, -84},   {-48, 39},    {-90, -85},
    {41, 46},     {42, 43},     {-49, -87},   {44, 45},   {-89, -86},   {-124, -123}, {47, 50},     {48, 49},
    {-122, -121}, {-120, -119}, {51, 54},     {52, 53},   {-118, -117}, {-116, -115}, {55, 56},     {-114, -113},
    {-112, -111}, {58, 89},     {59, 74},     {60, 67},   {61, 64},     {62, 63},     {-110, -109}, {-108, -107},
    {65, 66},     {-106, -105}, {-104, -103}, {68, 71},   {69, 70},     {-102, -101}, {-100, -99},  {72, 73},
    {-98, -97},   {-96, -95},   {75, 82},     {76, 79},   {77, 78},     {-94, -93},   {-92, -91},   {80, 81},
    {-47, -46},   {-45, -44},   {83, 86},     {84, 85},   {-43, -42},   {-41, -40},   {87, 88},     {-39, -38},
    {-37, -36},   {90, 105},    {91, 98},     {92, 95},   {93, 94},     {-35, -34},   {-33, -32},   {96, 97},
    {-31, -30},   {-29, -28},   {99, 102},    {100, 101}, {-27, -26},   {-25, -24},   {103, 104},   {-23, -22},
    {-21, -20},   {106, 113},   {107, 110},   {108, 109}, {-19, -18},   {-17, -16},   {111, 112},   {-15, -14},
    {-13, -12},   {114, 117},   {115, 116},   {-11, -10}, {-9, -8},     {118, 119},   {-7, -6},     {-5, -4}};

const int8_t FDK_sbrDecoder_sbr_huffBook_EnvLevel10F[120][2] = {
    {1, 2},       {-64, -65},   {3, 4},      {-63, -66}, {5, 6},       {-67, -62},   {7, 8},       {-68, -61},
    {9, 10},      {-69, -60},   {11, 13},    {-70, 12},  {-59, -71},   {14, 16},     {-58, 15},    {-72, -57},
    {17, 19},     {-73, 18},    {-56, -74},  {20, 23},   {21, 22},     {-55, -75},   {-54, -53},   {24, 27},
    {25, 26},     {-76, -52},   {-77, -51},  {28, 31},   {29, 30},     {-50, -78},   {-79, -49},   {32, 36},
    {33, 34},     {-48, -47},   {-80, 35},   {-81, -82}, {37, 47},     {38, 41},     {39, 40},     {-83, -46},
    {-45, -84},   {42, 44},     {-85, 43},   {-44, -43}, {45, 46},     {-88, -87},   {-86, -90},   {48, 66},
    {49, 56},     {50, 53},     {51, 52},    {-92, -42}, {-41, -39},   {54, 55},     {-105, -89},  {-38, -37},
    {57, 60},     {58, 59},     {-94, -91},  {-40, -36}, {61, 63},     {-20, 62},    {-115, -110}, {64, 65},
    {-108, -107}, {-101, -97},  {67, 89},    {68, 75},   {69, 72},     {70, 71},     {-95, -93},   {-34, -27},
    {73, 74},     {-22, -17},   {-16, -124}, {76, 82},   {77, 79},     {-123, 78},   {-122, -121}, {80, 81},
    {-120, -119}, {-118, -117}, {83, 86},    {84, 85},   {-116, -114}, {-113, -112}, {87, 88},     {-111, -109},
    {-106, -104}, {90, 105},    {91, 98},    {92, 95},   {93, 94},     {-103, -102}, {-100, -99},  {96, 97},
    {-98, -96},   {-35, -33},   {99, 102},   {100, 101}, {-32, -31},   {-30, -29},   {103, 104},   {-28, -26},
    {-25, -24},   {106, 113},   {107, 110},  {108, 109}, {-23, -21},   {-19, -18},   {111, 112},   {-15, -14},
    {-13, -12},   {114, 117},   {115, 116},  {-11, -10}, {-9, -8},     {118, 119},   {-7, -6},     {-5, -4}};

const int8_t FDK_sbrDecoder_sbr_huffBook_EnvBalance10T[48][2] = {
    {-64, 1},   {-63, 2},   {-65, 3},   {-62, 4},   {-66, 5},   {-61, 6}, {-67, 7},   {-60, 8},
    {-68, 9},   {10, 11},   {-69, -59}, {12, 13},   {-70, -58}, {14, 28}, {15, 21},   {16, 18},
    {-57, 17},  {-71, -56}, {19, 20},   {-88, -87}, {-86, -85}, {22, 25}, {23, 24},   {-84, -83},
    {-82, -81}, {26, 27},   {-80, -79}, {-78, -77}, {29, 36},   {30, 33}, {31, 32},   {-76, -75},
    {-74, -73}, {34, 35},   {-72, -55}, {-54, -53}, {37, 41},   {38, 39}, {-52, -51}, {-50, 40},
    {-49, -48}, {42, 45},   {43, 44},   {-47, -46}, {-45, -44}, {46, 47}, {-43, -42}, {-41, -40}};

const int8_t FDK_sbrDecoder_sbr_huffBook_EnvBalance10F[48][2] = {
    {-64, 1},   {-65, 2}, {-63, 3},   {-66, 4},   {-62, 5},   {-61, 6},   {-67, 7},   {-68, 8},
    {-60, 9},   {10, 11}, {-69, -59}, {-70, 12},  {-58, 13},  {14, 17},   {-71, 15},  {-57, 16},
    {-56, -73}, {18, 32}, {19, 25},   {20, 22},   {-72, 21},  {-88, -87}, {23, 24},   {-86, -85},
    {-84, -83}, {26, 29}, {27, 28},   {-82, -81}, {-80, -79}, {30, 31},   {-78, -77}, {-76, -75},
    {33, 40},   {34, 37}, {35, 36},   {-74, -55}, {-54, -53}, {38, 39},   {-52, -51}, {-50, -49},
    {41, 44},   {42, 43}, {-48, -47}, {-46, -45}, {45, 46},   {-44, -43}, {-42, 47},  {-41, -40}};

const int8_t FDK_sbrDecoder_sbr_huffBook_EnvLevel11T[62][2] = {
    {-64, 1},   {-65, 2},   {-63, 3},   {-66, 4},   {-62, 5},   {-67, 6},   {-61, 7},   {-68, 8},   {-60, 9},
    {10, 11},   {-69, -59}, {12, 14},   {-70, 13},  {-71, -58}, {15, 18},   {16, 17},   {-72, -57}, {-73, -74},
    {19, 22},   {-56, 20},  {-55, 21},  {-54, -77}, {23, 31},   {24, 25},   {-75, -76}, {26, 27},   {-78, -53},
    {28, 29},   {-52, -95}, {-94, 30},  {-93, -92}, {32, 47},   {33, 40},   {34, 37},   {35, 36},   {-91, -90},
    {-89, -88}, {38, 39},   {-87, -86}, {-85, -84}, {41, 44},   {42, 43},   {-83, -82}, {-81, -80}, {45, 46},
    {-79, -51}, {-50, -49}, {48, 55},   {49, 52},   {50, 51},   {-48, -47}, {-46, -45}, {53, 54},   {-44, -43},
    {-42, -41}, {56, 59},   {57, 58},   {-40, -39}, {-38, -37}, {60, 61},   {-36, -35}, {-34, -33}};

const int8_t FDK_sbrDecoder_sbr_huffBook_EnvLevel11F[62][2] = {
    {-64, 1},   {-65, 2},   {-63, 3},   {-66, 4},   {-62, 5},   {-67, 6},   {7, 8},     {-61, -68}, {9, 10},
    {-60, -69}, {11, 12},   {-59, -70}, {13, 14},   {-58, -71}, {15, 16},   {-57, -72}, {17, 19},   {-56, 18},
    {-55, -73}, {20, 24},   {21, 22},   {-74, -54}, {-53, 23},  {-75, -76}, {25, 30},   {26, 27},   {-52, -51},
    {28, 29},   {-77, -79}, {-50, -49}, {31, 39},   {32, 35},   {33, 34},   {-78, -46}, {-82, -88}, {36, 37},
    {-83, -48}, {-47, 38},  {-86, -85}, {40, 47},   {41, 44},   {42, 43},   {-80, -44}, {-43, -42}, {45, 46},
    {-39, -87}, {-84, -40}, {48, 55},   {49, 52},   {50, 51},   {-95, -94}, {-93, -92}, {53, 54},   {-91, -90},
    {-89, -81}, {56, 59},   {57, 58},   {-45, -41}, {-38, -37}, {60, 61},   {-36, -35}, {-34, -33}};

const int8_t FDK_sbrDecoder_sbr_huffBook_EnvBalance11T[24][2] = {
    {-64, 1}, {-63, 2}, {-65, 3},   {-66, 4},   {-62, 5},   {-61, 6},   {-67, 7},   {-68, 8},
    {-60, 9}, {10, 16}, {11, 13},   {-69, 12},  {-76, -75}, {14, 15},   {-74, -73}, {-72, -71},
    {17, 20}, {18, 19}, {-70, -59}, {-58, -57}, {21, 22},   {-56, -55}, {-54, 23},  {-53, -52}};

const int8_t FDK_sbrDecoder_sbr_huffBook_EnvBalance11F[24][2] = {
    {-64, 1},   {-65, 2}, {-63, 3},  {-66, 4},   {-62, 5},   {-61, 6}, {-67, 7},   {-68, 8},
    {-60, 9},   {10, 13}, {-69, 11}, {-59, 12},  {-58, -76}, {14, 17}, {15, 16},   {-75, -74},
    {-73, -72}, {18, 21}, {19, 20},  {-71, -70}, {-57, -56}, {22, 23}, {-55, -54}, {-53, -52}};

const int8_t FDK_sbrDecoder_sbr_huffBook_NoiseLevel11T[62][2] = {
    {-64, 1},   {-63, 2},   {-65, 3},   {-66, 4},   {-62, 5},   {-67, 6},   {7, 8},     {-61, -68}, {9, 30},
    {10, 15},   {-60, 11},  {-69, 12},  {13, 14},   {-59, -53}, {-95, -94}, {16, 23},   {17, 20},   {18, 19},
    {-93, -92}, {-91, -90}, {21, 22},   {-89, -88}, {-87, -86}, {24, 27},   {25, 26},   {-85, -84}, {-83, -82},
    {28, 29},   {-81, -80}, {-79, -78}, {31, 46},   {32, 39},   {33, 36},   {34, 35},   {-77, -76}, {-75, -74},
    {37, 38},   {-73, -72}, {-71, -70}, {40, 43},   {41, 42},   {-58, -57}, {-56, -55}, {44, 45},   {-54, -52},
    {-51, -50}, {47, 54},   {48, 51},   {49, 50},   {-49, -48}, {-47, -46}, {52, 53},   {-45, -44}, {-43, -42},
    {55, 58},   {56, 57},   {-41, -40}, {-39, -38}, {59, 60},   {-37, -36}, {-35, 61},  {-34, -33}};

const int8_t FDK_sbrDecoder_sbr_huffBook_NoiseBalance11T[24][2] = {
    {-64, 1},   {-65, 2}, {-63, 3}, {4, 9},     {-66, 5},   {-62, 6},   {7, 8},     {-76, -75},
    {-74, -73}, {10, 17}, {11, 14}, {12, 13},   {-72, -71}, {-70, -69}, {15, 16},   {-68, -67},
    {-61, -60}, {18, 21}, {19, 20}, {-59, -58}, {-57, -56}, {22, 23},   {-55, -54}, {-53, -52}};
//@}

/*!
  \name  parametric stereo
  \brief constants used by the parametric stereo part of the decoder

*/

/* constants used in psbitdec.cpp */

/* FIX_BORDER can have 0, 1, 2, 4 envelopes */
const uint8_t FDK_sbrDecoder_aFixNoEnvDecode[4] = {0, 1, 2, 4};

/* IID & ICC Huffman codebooks */
const int8_t aBookPsIidTimeDecode[28][2] = {
    {-64, 1},   {-65, 2},  {-63, 3},  {-66, 4},   {-62, 5},   {-67, 6}, {-61, 7},   {-68, 8},  {-60, 9}, {-69, 10},
    {-59, 11},  {-70, 12}, {-58, 13}, {-57, 14},  {-71, 15},  {16, 17}, {-56, -72}, {18, 21},  {19, 20}, {-55, -78},
    {-77, -76}, {22, 25},  {23, 24},  {-75, -74}, {-73, -54}, {26, 27}, {-53, -52}, {-51, -50}};

const int8_t aBookPsIidFreqDecode[28][2] = {
    {-64, 1},   {2, 3},    {-63, -65}, {4, 5},     {-62, -66}, {6, 7},     {-61, -67}, {8, 9},    {-68, -60}, {-59, 10},
    {-69, 11},  {-58, 12}, {-70, 13},  {-71, 14},  {-57, 15},  {16, 17},   {-56, -72}, {18, 19},  {-55, -54}, {20, 21},
    {-73, -53}, {22, 24},  {-74, 23},  {-75, -78}, {25, 26},   {-77, -76}, {-52, 27},  {-51, -50}};

const int8_t aBookPsIccTimeDecode[14][2] = {{-64, 1}, {-63, 2}, {-65, 3},  {-62, 4},  {-66, 5},  {-61, 6},  {-67, 7},
                                            {-60, 8}, {-68, 9}, {-59, 10}, {-69, 11}, {-58, 12}, {-70, 13}, {-71, -57}};

const int8_t aBookPsIccFreqDecode[14][2] = {{-64, 1}, {-63, 2}, {-65, 3},  {-62, 4},  {-66, 5},  {-61, 6},  {-67, 7},
                                            {-60, 8}, {-59, 9}, {-68, 10}, {-58, 11}, {-69, 12}, {-57, 13}, {-70, -71}};

/* IID-fine Huffman codebooks */

const int8_t aBookPsIidFineTimeDecode[60][2] = {
    {1, -64},   {-63, 2},   {3, -65},   {4, 59},    {5, 7},     {6, -67},   {-68, -60}, {-61, 8},   {9, 11},
    {-59, 10},  {-70, -58}, {12, 41},   {13, 20},   {14, -71},  {-55, 15},  {-53, 16},  {17, -77},  {18, 19},
    {-85, -84}, {-46, -45}, {-57, 21},  {22, 40},   {23, 29},   {-51, 24},  {25, 26},   {-83, -82}, {27, 28},
    {-90, -38}, {-92, -91}, {30, 37},   {31, 34},   {32, 33},   {-35, -34}, {-37, -36}, {35, 36},   {-94, -93},
    {-89, -39}, {38, -79},  {39, -81},  {-88, -40}, {-74, -54}, {42, -69},  {43, 44},   {-72, -56}, {45, 52},
    {46, 50},   {47, -76},  {-49, 48},  {-47, 49},  {-87, -41}, {-52, 51},  {-78, -50}, {53, -73},  {54, -75},
    {55, 57},   {56, -80},  {-86, -42}, {-48, 58},  {-44, -43}, {-66, -62}};

const int8_t aBookPsIidFineFreqDecode[60][2] = {
    {1, -64},   {2, 4},     {3, -65},  {-66, -62}, {-63, 5},   {6, 7},    {-67, -61}, {8, 9},    {-68, -60},
    {10, 11},   {-69, -59}, {12, 13},  {-70, -58}, {14, 18},   {-57, 15}, {16, -72},  {-54, 17}, {-75, -53},
    {19, 37},   {-56, 20},  {21, -73}, {22, 29},   {23, -76},  {24, -78}, {25, 28},   {26, 27},  {-85, -43},
    {-83, -45}, {-81, -47}, {-52, 30}, {-50, 31},  {32, -79},  {33, 34},  {-82, -46}, {35, 36},  {-90, -89},
    {-92, -91}, {38, -71},  {-55, 39}, {40, -74},  {41, 50},   {42, -77}, {-49, 43},  {44, 47},  {45, 46},
    {-86, -42}, {-88, -87}, {48, 49},  {-39, -38}, {-41, -40}, {-51, 51}, {52, 59},   {53, 56},  {54, 55},
    {-35, -34}, {-37, -36}, {57, 58},  {-94, -93}, {-84, -44}, {-80, -48}};

/* constants used in psdec.cpp */

/* the values of the following 3 tables are shiftet right by 1 ! */
const int32_t ScaleFactors[NO_IID_LEVELS] = {

    0x5a5ded00, 0x59cd0400, 0x58c29680, 0x564c2e80, 0x52a3d480, 0x4c8be080, 0x46df3080, 0x40000000,
    0x384ba5c0, 0x304c2980, 0x24e9f640, 0x1b4a2940, 0x11b5c0a0, 0x0b4e2540, 0x0514ea90};

const int32_t ScaleFactorsFine[NO_IID_LEVELS_FINE] = {

    0x5a825c00, 0x5a821c00, 0x5a815100, 0x5a7ed000, 0x5a76e600, 0x5a5ded00, 0x5a39b880, 0x59f1fd00,
    0x5964d680, 0x5852ca00, 0x564c2e80, 0x54174480, 0x50ea7500, 0x4c8be080, 0x46df3080, 0x40000000,
    0x384ba5c0, 0x304c2980, 0x288dd240, 0x217a2900, 0x1b4a2940, 0x13c5ece0, 0x0e2b0090, 0x0a178ef0,
    0x072ab798, 0x0514ea90, 0x02dc5944, 0x019bf87c, 0x00e7b173, 0x00824b8b, 0x00494568};
const int32_t Alphas[NO_ICC_LEVELS] = {

    0x00000000, 0x0b6b5be0, 0x12485f80, 0x1da2fa40, 0x2637ebc0, 0x3243f6c0, 0x466b7480, 0x6487ed80};

const uint8_t bins2groupMap20[NO_IID_GROUPS] = {0, 0,  1,  1,  2,  3,  4,  5,  6,  7,  8,
                                                9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

const uint8_t FDK_sbrDecoder_aNoIidBins[3] = {NO_LOW_RES_IID_BINS, NO_MID_RES_IID_BINS, NO_HI_RES_IID_BINS};

const uint8_t FDK_sbrDecoder_aNoIccBins[3] = {NO_LOW_RES_ICC_BINS, NO_MID_RES_ICC_BINS, NO_HI_RES_ICC_BINS};

/************************************************************************/
/*!
   \brief   Create lookup tables for some arithmetic functions

   The tables would normally be defined as const arrays,
   but initialization at run time allows to specify their accuracy.
*/
/************************************************************************/

/*   1/x-table:  (example for INV_TABLE_BITS 8)

     The table covers an input range from 0.5 to 1.0 with a step size of 1/512,
     starting at 0.5 + 1/512.
     Each table entry corresponds to an input interval starting 1/1024 below the
     exact value and ending 1/1024 above it.

     The table is actually a 0.5/x-table, so that the output range is again
     0.5...1.0 and the exponent of the result must be increased by 1.

     Input range           Index in table      result
     -------------------------------------------------------------------
     0.500000...0.500976          -            0.5 / 0.500000 = 1.000000
     0.500976...0.502930          0            0.5 / 0.501953 = 0.996109
     0.502930...0.500488          1            0.5 / 0.503906 = 0.992248
             ...
     0.999023...1.000000         255           0.5 / 1.000000 = 0.500000

       for (i=0; i<INV_TABLE_SIZE; i++) {
         d = 0.5f / ( 0.5f+(double)(i+1)/(INV_TABLE_SIZE*2) ) ;
         invTable[i] = FL2FX_SGL(d);
       }
*/
const int16_t FDK_sbrDecoder_invTable[INV_TABLE_SIZE] = {
    0x7f80, 0x7f01, 0x7e83, 0x7e07, 0x7d8b, 0x7d11, 0x7c97, 0x7c1e, 0x7ba6, 0x7b2f, 0x7ab9, 0x7a44, 0x79cf, 0x795c,
    0x78e9, 0x7878, 0x7807, 0x7796, 0x7727, 0x76b9, 0x764b, 0x75de, 0x7572, 0x7506, 0x749c, 0x7432, 0x73c9, 0x7360,
    0x72f9, 0x7292, 0x722c, 0x71c6, 0x7161, 0x70fd, 0x709a, 0x7037, 0x6fd5, 0x6f74, 0x6f13, 0x6eb3, 0x6e54, 0x6df5,
    0x6d97, 0x6d39, 0x6cdc, 0x6c80, 0x6c24, 0x6bc9, 0x6b6f, 0x6b15, 0x6abc, 0x6a63, 0x6a0b, 0x69b3, 0x695c, 0x6906,
    0x68b0, 0x685a, 0x6806, 0x67b1, 0x675e, 0x670a, 0x66b8, 0x6666, 0x6614, 0x65c3, 0x6572, 0x6522, 0x64d2, 0x6483,
    0x6434, 0x63e6, 0x6399, 0x634b, 0x62fe, 0x62b2, 0x6266, 0x621b, 0x61d0, 0x6185, 0x613b, 0x60f2, 0x60a8, 0x6060,
    0x6017, 0x5fcf, 0x5f88, 0x5f41, 0x5efa, 0x5eb4, 0x5e6e, 0x5e28, 0x5de3, 0x5d9f, 0x5d5a, 0x5d17, 0x5cd3, 0x5c90,
    0x5c4d, 0x5c0b, 0x5bc9, 0x5b87, 0x5b46, 0x5b05, 0x5ac4, 0x5a84, 0x5a44, 0x5a05, 0x59c6, 0x5987, 0x5949, 0x590a,
    0x58cd, 0x588f, 0x5852, 0x5815, 0x57d9, 0x579d, 0x5761, 0x5725, 0x56ea, 0x56af, 0x5675, 0x563b, 0x5601, 0x55c7,
    0x558e, 0x5555, 0x551c, 0x54e3, 0x54ab, 0x5473, 0x543c, 0x5405, 0x53ce, 0x5397, 0x5360, 0x532a, 0x52f4, 0x52bf,
    0x5289, 0x5254, 0x521f, 0x51eb, 0x51b7, 0x5183, 0x514f, 0x511b, 0x50e8, 0x50b5, 0x5082, 0x5050, 0x501d, 0x4feb,
    0x4fba, 0x4f88, 0x4f57, 0x4f26, 0x4ef5, 0x4ec4, 0x4e94, 0x4e64, 0x4e34, 0x4e04, 0x4dd5, 0x4da6, 0x4d77, 0x4d48,
    0x4d19, 0x4ceb, 0x4cbd, 0x4c8f, 0x4c61, 0x4c34, 0x4c07, 0x4bd9, 0x4bad, 0x4b80, 0x4b54, 0x4b27, 0x4afb, 0x4acf,
    0x4aa4, 0x4a78, 0x4a4d, 0x4a22, 0x49f7, 0x49cd, 0x49a2, 0x4978, 0x494e, 0x4924, 0x48fa, 0x48d1, 0x48a7, 0x487e,
    0x4855, 0x482d, 0x4804, 0x47dc, 0x47b3, 0x478b, 0x4763, 0x473c, 0x4714, 0x46ed, 0x46c5, 0x469e, 0x4677, 0x4651,
    0x462a, 0x4604, 0x45de, 0x45b8, 0x4592, 0x456c, 0x4546, 0x4521, 0x44fc, 0x44d7, 0x44b2, 0x448d, 0x4468, 0x4444,
    0x441f, 0x43fb, 0x43d7, 0x43b3, 0x4390, 0x436c, 0x4349, 0x4325, 0x4302, 0x42df, 0x42bc, 0x4299, 0x4277, 0x4254,
    0x4232, 0x4210, 0x41ee, 0x41cc, 0x41aa, 0x4189, 0x4167, 0x4146, 0x4125, 0x4104, 0x40e3, 0x40c2, 0x40a1, 0x4081,
    0x4060, 0x4040, 0x4020, 0x4000};
