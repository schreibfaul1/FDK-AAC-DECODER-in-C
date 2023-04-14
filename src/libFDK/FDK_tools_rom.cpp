/* -----------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

© Copyright  1995 - 2018 Fraunhofer-Gesellschaft zur Förderung der angewandten
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

/******************* Library for basic calculation routines ********************

   Author(s):   Oliver Moser

   Description: ROM tables used by FDK tools

*******************************************************************************/

#include "FDK_tools_rom.h"


RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STP SineTable80[] = {
    STCP(0x7fffffff, 0x00000000), STCP(0x7ff9af04, 0x02835b5a), STCP(0x7fe6bcb0, 0x05067734),
    STCP(0x7fc72ae2, 0x07891418), STCP(0x7f9afcb9, 0x0a0af299), STCP(0x7f62368f, 0x0c8bd35e),
    STCP(0x7f1cde01, 0x0f0b7727), STCP(0x7ecaf9e5, 0x11899ed3), STCP(0x7e6c9251, 0x14060b68),
    STCP(0x7e01b096, 0x16807e15), STCP(0x7d8a5f40, 0x18f8b83c), STCP(0x7d06aa16, 0x1b6e7b7a),
    STCP(0x7c769e18, 0x1de189a6), STCP(0x7bda497d, 0x2051a4dd), STCP(0x7b31bbb2, 0x22be8f87),
    STCP(0x7a7d055b, 0x25280c5e), STCP(0x79bc384d, 0x278dde6e), STCP(0x78ef678f, 0x29efc925),
    STCP(0x7816a759, 0x2c4d9050), STCP(0x77320d0d, 0x2ea6f827), STCP(0x7641af3d, 0x30fbc54d),
    STCP(0x7545a5a0, 0x334bbcde), STCP(0x743e0918, 0x3596a46c), STCP(0x732af3a7, 0x37dc420c),
    STCP(0x720c8075, 0x3a1c5c57), STCP(0x70e2cbc6, 0x3c56ba70), STCP(0x6fadf2fc, 0x3e8b240e),
    STCP(0x6e6e1492, 0x40b9617d), STCP(0x6d23501b, 0x42e13ba4), STCP(0x6bcdc639, 0x45027c0c),
    STCP(0x6a6d98a4, 0x471cece7), STCP(0x6902ea1d, 0x4930590f), STCP(0x678dde6e, 0x4b3c8c12),
    STCP(0x660e9a6a, 0x4d415234), STCP(0x648543e4, 0x4f3e7875), STCP(0x62f201ac, 0x5133cc94),
    STCP(0x6154fb91, 0x53211d18), STCP(0x5fae5a55, 0x55063951), STCP(0x5dfe47ad, 0x56e2f15d),
    STCP(0x5c44ee40, 0x58b71632), STCP(0x5a82799a, 0x5a82799a),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STP SineTable384[] = {
    STCP(0x7fffffff, 0x00000000), STCP(0x7fffb9d1, 0x00860a79), STCP(0x7ffee744, 0x010c1460),
    STCP(0x7ffd885a, 0x01921d20), STCP(0x7ffb9d15, 0x02182427), STCP(0x7ff92577, 0x029e28e2),
    STCP(0x7ff62182, 0x03242abf), STCP(0x7ff2913a, 0x03aa292a), STCP(0x7fee74a2, 0x0430238f),
    STCP(0x7fe9cbc0, 0x04b6195d), STCP(0x7fe49698, 0x053c0a01), STCP(0x7fded530, 0x05c1f4e7),
    STCP(0x7fd8878e, 0x0647d97c), STCP(0x7fd1adb9, 0x06cdb72f), STCP(0x7fca47b9, 0x07538d6b),
    STCP(0x7fc25596, 0x07d95b9e), STCP(0x7fb9d759, 0x085f2137), STCP(0x7fb0cd0a, 0x08e4dda0),
    STCP(0x7fa736b4, 0x096a9049), STCP(0x7f9d1461, 0x09f0389f), STCP(0x7f92661d, 0x0a75d60e),
    STCP(0x7f872bf3, 0x0afb6805), STCP(0x7f7b65ef, 0x0b80edf1), STCP(0x7f6f141f, 0x0c066740),
    STCP(0x7f62368f, 0x0c8bd35e), STCP(0x7f54cd4f, 0x0d1131ba), STCP(0x7f46d86c, 0x0d9681c2),
    STCP(0x7f3857f6, 0x0e1bc2e4), STCP(0x7f294bfd, 0x0ea0f48c), STCP(0x7f19b491, 0x0f26162a),
    STCP(0x7f0991c4, 0x0fab272b), STCP(0x7ef8e3a6, 0x103026fe), STCP(0x7ee7aa4c, 0x10b5150f),
    STCP(0x7ed5e5c6, 0x1139f0cf), STCP(0x7ec3962a, 0x11beb9aa), STCP(0x7eb0bb8a, 0x12436f10),
    STCP(0x7e9d55fc, 0x12c8106f), STCP(0x7e896595, 0x134c9d34), STCP(0x7e74ea6a, 0x13d114d0),
    STCP(0x7e5fe493, 0x145576b1), STCP(0x7e4a5426, 0x14d9c245), STCP(0x7e34393b, 0x155df6fc),
    STCP(0x7e1d93ea, 0x15e21445), STCP(0x7e06644c, 0x1666198d), STCP(0x7deeaa7a, 0x16ea0646),
    STCP(0x7dd6668f, 0x176dd9de), STCP(0x7dbd98a4, 0x17f193c5), STCP(0x7da440d6, 0x1875336a),
    STCP(0x7d8a5f40, 0x18f8b83c), STCP(0x7d6ff3fe, 0x197c21ad), STCP(0x7d54ff2e, 0x19ff6f2a),
    STCP(0x7d3980ec, 0x1a82a026), STCP(0x7d1d7958, 0x1b05b40f), STCP(0x7d00e88f, 0x1b88aa55),
    STCP(0x7ce3ceb2, 0x1c0b826a), STCP(0x7cc62bdf, 0x1c8e3bbe), STCP(0x7ca80038, 0x1d10d5c2),
    STCP(0x7c894bde, 0x1d934fe5), STCP(0x7c6a0ef2, 0x1e15a99a), STCP(0x7c4a4996, 0x1e97e251),
    STCP(0x7c29fbee, 0x1f19f97b), STCP(0x7c09261d, 0x1f9bee8a), STCP(0x7be7c847, 0x201dc0ef),
    STCP(0x7bc5e290, 0x209f701c), STCP(0x7ba3751d, 0x2120fb83), STCP(0x7b808015, 0x21a26295),
    STCP(0x7b5d039e, 0x2223a4c5), STCP(0x7b38ffde, 0x22a4c185), STCP(0x7b1474fd, 0x2325b847),
    STCP(0x7aef6323, 0x23a6887f), STCP(0x7ac9ca7a, 0x2427319d), STCP(0x7aa3ab29, 0x24a7b317),
    STCP(0x7a7d055b, 0x25280c5e), STCP(0x7a55d93a, 0x25a83ce6), STCP(0x7a2e26f2, 0x26284422),
    STCP(0x7a05eead, 0x26a82186), STCP(0x79dd3098, 0x2727d486), STCP(0x79b3ece0, 0x27a75c95),
    STCP(0x798a23b1, 0x2826b928), STCP(0x795fd53a, 0x28a5e9b4), STCP(0x793501a9, 0x2924edac),
    STCP(0x7909a92d, 0x29a3c485), STCP(0x78ddcbf5, 0x2a226db5), STCP(0x78b16a32, 0x2aa0e8b0),
    STCP(0x78848414, 0x2b1f34eb), STCP(0x785719cc, 0x2b9d51dd), STCP(0x78292b8d, 0x2c1b3efb),
    STCP(0x77fab989, 0x2c98fbba), STCP(0x77cbc3f2, 0x2d168792), STCP(0x779c4afc, 0x2d93e1f8),
    STCP(0x776c4edb, 0x2e110a62), STCP(0x773bcfc4, 0x2e8e0048), STCP(0x770acdec, 0x2f0ac320),
    STCP(0x76d94989, 0x2f875262), STCP(0x76a742d1, 0x3003ad85), STCP(0x7674b9fa, 0x307fd401),
    STCP(0x7641af3d, 0x30fbc54d), STCP(0x760e22d1, 0x317780e2), STCP(0x75da14ef, 0x31f30638),
    STCP(0x75a585cf, 0x326e54c7), STCP(0x757075ac, 0x32e96c09), STCP(0x753ae4c0, 0x33644b76),
    STCP(0x7504d345, 0x33def287), STCP(0x74ce4177, 0x345960b7), STCP(0x74972f92, 0x34d3957e),
    STCP(0x745f9dd1, 0x354d9057), STCP(0x74278c72, 0x35c750bc), STCP(0x73eefbb3, 0x3640d627),
    STCP(0x73b5ebd1, 0x36ba2014), STCP(0x737c5d0b, 0x37332dfd), STCP(0x73424fa0, 0x37abff5d),
    STCP(0x7307c3d0, 0x382493b0), STCP(0x72ccb9db, 0x389cea72), STCP(0x72913201, 0x3915031f),
    STCP(0x72552c85, 0x398cdd32), STCP(0x7218a9a7, 0x3a04782a), STCP(0x71dba9ab, 0x3a7bd382),
    STCP(0x719e2cd2, 0x3af2eeb7), STCP(0x71603361, 0x3b69c947), STCP(0x7121bd9c, 0x3be062b0),
    STCP(0x70e2cbc6, 0x3c56ba70), STCP(0x70a35e25, 0x3cccd004), STCP(0x706374ff, 0x3d42a2ec),
    STCP(0x7023109a, 0x3db832a6), STCP(0x6fe2313c, 0x3e2d7eb1), STCP(0x6fa0d72c, 0x3ea2868c),
    STCP(0x6f5f02b2, 0x3f1749b8), STCP(0x6f1cb416, 0x3f8bc7b4), STCP(0x6ed9eba1, 0x40000000),
    STCP(0x6e96a99d, 0x4073f21d), STCP(0x6e52ee52, 0x40e79d8c), STCP(0x6e0eba0c, 0x415b01ce),
    STCP(0x6dca0d14, 0x41ce1e65), STCP(0x6d84e7b7, 0x4240f2d1), STCP(0x6d3f4a40, 0x42b37e96),
    STCP(0x6cf934fc, 0x4325c135), STCP(0x6cb2a837, 0x4397ba32), STCP(0x6c6ba43e, 0x44096910),
    STCP(0x6c242960, 0x447acd50), STCP(0x6bdc37eb, 0x44ebe679), STCP(0x6b93d02e, 0x455cb40c),
    STCP(0x6b4af279, 0x45cd358f), STCP(0x6b019f1a, 0x463d6a87), STCP(0x6ab7d663, 0x46ad5278),
    STCP(0x6a6d98a4, 0x471cece7), STCP(0x6a22e630, 0x478c395a), STCP(0x69d7bf57, 0x47fb3757),
    STCP(0x698c246c, 0x4869e665), STCP(0x694015c3, 0x48d84609), STCP(0x68f393ae, 0x494655cc),
    STCP(0x68a69e81, 0x49b41533), STCP(0x68593691, 0x4a2183c8), STCP(0x680b5c33, 0x4a8ea111),
    STCP(0x67bd0fbd, 0x4afb6c98), STCP(0x676e5183, 0x4b67e5e4), STCP(0x671f21dc, 0x4bd40c80),
    STCP(0x66cf8120, 0x4c3fdff4), STCP(0x667f6fa5, 0x4cab5fc9), STCP(0x662eedc3, 0x4d168b8b),
    STCP(0x65ddfbd3, 0x4d8162c4), STCP(0x658c9a2d, 0x4debe4fe), STCP(0x653ac92b, 0x4e5611c5),
    STCP(0x64e88926, 0x4ebfe8a5), STCP(0x6495da79, 0x4f296928), STCP(0x6442bd7e, 0x4f9292dc),
    STCP(0x63ef3290, 0x4ffb654d), STCP(0x639b3a0b, 0x5063e008), STCP(0x6346d44b, 0x50cc029c),
    STCP(0x62f201ac, 0x5133cc94), STCP(0x629cc28c, 0x519b3d80), STCP(0x62471749, 0x520254ef),
    STCP(0x61f1003f, 0x5269126e), STCP(0x619a7dce, 0x52cf758f), STCP(0x61439053, 0x53357ddf),
    STCP(0x60ec3830, 0x539b2af0), STCP(0x609475c3, 0x54007c51), STCP(0x603c496c, 0x54657194),
    STCP(0x5fe3b38d, 0x54ca0a4b), STCP(0x5f8ab487, 0x552e4605), STCP(0x5f314cba, 0x55922457),
    STCP(0x5ed77c8a, 0x55f5a4d2), STCP(0x5e7d4458, 0x5658c709), STCP(0x5e22a487, 0x56bb8a90),
    STCP(0x5dc79d7c, 0x571deefa), STCP(0x5d6c2f99, 0x577ff3da), STCP(0x5d105b44, 0x57e198c7),
    STCP(0x5cb420e0, 0x5842dd54), STCP(0x5c5780d3, 0x58a3c118), STCP(0x5bfa7b82, 0x590443a7),
    STCP(0x5b9d1154, 0x59646498), STCP(0x5b3f42ae, 0x59c42381), STCP(0x5ae10ff9, 0x5a237ffa),
    STCP(0x5a82799a, 0x5a82799a),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STP SineTable480[] = {
    STCP(0x7fffffff, 0x00000000), STCP(0x7fffd315, 0x006b3b9b), STCP(0x7fff4c54, 0x00d676eb),
    STCP(0x7ffe6bbf, 0x0141b1a5), STCP(0x7ffd3154, 0x01aceb7c), STCP(0x7ffb9d15, 0x02182427),
    STCP(0x7ff9af04, 0x02835b5a), STCP(0x7ff76721, 0x02ee90c8), STCP(0x7ff4c56f, 0x0359c428),
    STCP(0x7ff1c9ef, 0x03c4f52f), STCP(0x7fee74a2, 0x0430238f), STCP(0x7feac58d, 0x049b4f00),
    STCP(0x7fe6bcb0, 0x05067734), STCP(0x7fe25a0f, 0x05719be2), STCP(0x7fdd9dad, 0x05dcbcbe),
    STCP(0x7fd8878e, 0x0647d97c), STCP(0x7fd317b4, 0x06b2f1d2), STCP(0x7fcd4e24, 0x071e0575),
    STCP(0x7fc72ae2, 0x07891418), STCP(0x7fc0adf2, 0x07f41d72), STCP(0x7fb9d759, 0x085f2137),
    STCP(0x7fb2a71b, 0x08ca1f1b), STCP(0x7fab1d3d, 0x093516d4), STCP(0x7fa339c5, 0x09a00817),
    STCP(0x7f9afcb9, 0x0a0af299), STCP(0x7f92661d, 0x0a75d60e), STCP(0x7f8975f9, 0x0ae0b22c),
    STCP(0x7f802c52, 0x0b4b86a8), STCP(0x7f76892f, 0x0bb65336), STCP(0x7f6c8c96, 0x0c21178c),
    STCP(0x7f62368f, 0x0c8bd35e), STCP(0x7f578721, 0x0cf68662), STCP(0x7f4c7e54, 0x0d61304e),
    STCP(0x7f411c2f, 0x0dcbd0d5), STCP(0x7f3560b9, 0x0e3667ad), STCP(0x7f294bfd, 0x0ea0f48c),
    STCP(0x7f1cde01, 0x0f0b7727), STCP(0x7f1016ce, 0x0f75ef33), STCP(0x7f02f66f, 0x0fe05c64),
    STCP(0x7ef57cea, 0x104abe71), STCP(0x7ee7aa4c, 0x10b5150f), STCP(0x7ed97e9c, 0x111f5ff4),
    STCP(0x7ecaf9e5, 0x11899ed3), STCP(0x7ebc1c31, 0x11f3d164), STCP(0x7eace58a, 0x125df75b),
    STCP(0x7e9d55fc, 0x12c8106f), STCP(0x7e8d6d91, 0x13321c53), STCP(0x7e7d2c54, 0x139c1abf),
    STCP(0x7e6c9251, 0x14060b68), STCP(0x7e5b9f93, 0x146fee03), STCP(0x7e4a5426, 0x14d9c245),
    STCP(0x7e38b017, 0x154387e6), STCP(0x7e26b371, 0x15ad3e9a), STCP(0x7e145e42, 0x1616e618),
    STCP(0x7e01b096, 0x16807e15), STCP(0x7deeaa7a, 0x16ea0646), STCP(0x7ddb4bfc, 0x17537e63),
    STCP(0x7dc79529, 0x17bce621), STCP(0x7db3860f, 0x18263d36), STCP(0x7d9f1ebd, 0x188f8357),
    STCP(0x7d8a5f40, 0x18f8b83c), STCP(0x7d7547a7, 0x1961db9b), STCP(0x7d5fd801, 0x19caed29),
    STCP(0x7d4a105d, 0x1a33ec9c), STCP(0x7d33f0ca, 0x1a9cd9ac), STCP(0x7d1d7958, 0x1b05b40f),
    STCP(0x7d06aa16, 0x1b6e7b7a), STCP(0x7cef8315, 0x1bd72fa4), STCP(0x7cd80464, 0x1c3fd045),
    STCP(0x7cc02e15, 0x1ca85d12), STCP(0x7ca80038, 0x1d10d5c2), STCP(0x7c8f7ade, 0x1d793a0b),
    STCP(0x7c769e18, 0x1de189a6), STCP(0x7c5d69f7, 0x1e49c447), STCP(0x7c43de8e, 0x1eb1e9a7),
    STCP(0x7c29fbee, 0x1f19f97b), STCP(0x7c0fc22a, 0x1f81f37c), STCP(0x7bf53153, 0x1fe9d75f),
    STCP(0x7bda497d, 0x2051a4dd), STCP(0x7bbf0aba, 0x20b95bac), STCP(0x7ba3751d, 0x2120fb83),
    STCP(0x7b8788ba, 0x2188841a), STCP(0x7b6b45a5, 0x21eff528), STCP(0x7b4eabf1, 0x22574e65),
    STCP(0x7b31bbb2, 0x22be8f87), STCP(0x7b1474fd, 0x2325b847), STCP(0x7af6d7e6, 0x238cc85d),
    STCP(0x7ad8e482, 0x23f3bf7e), STCP(0x7aba9ae6, 0x245a9d65), STCP(0x7a9bfb27, 0x24c161c7),
    STCP(0x7a7d055b, 0x25280c5e), STCP(0x7a5db997, 0x258e9ce0), STCP(0x7a3e17f2, 0x25f51307),
    STCP(0x7a1e2082, 0x265b6e8a), STCP(0x79fdd35c, 0x26c1af22), STCP(0x79dd3098, 0x2727d486),
    STCP(0x79bc384d, 0x278dde6e), STCP(0x799aea92, 0x27f3cc94), STCP(0x7979477d, 0x28599eb0),
    STCP(0x79574f28, 0x28bf547b), STCP(0x793501a9, 0x2924edac), STCP(0x79125f19, 0x298a69fc),
    STCP(0x78ef678f, 0x29efc925), STCP(0x78cc1b26, 0x2a550adf), STCP(0x78a879f4, 0x2aba2ee4),
    STCP(0x78848414, 0x2b1f34eb), STCP(0x7860399e, 0x2b841caf), STCP(0x783b9aad, 0x2be8e5e8),
    STCP(0x7816a759, 0x2c4d9050), STCP(0x77f15fbc, 0x2cb21ba0), STCP(0x77cbc3f2, 0x2d168792),
    STCP(0x77a5d413, 0x2d7ad3de), STCP(0x777f903c, 0x2ddf0040), STCP(0x7758f886, 0x2e430c6f),
    STCP(0x77320d0d, 0x2ea6f827), STCP(0x770acdec, 0x2f0ac320), STCP(0x76e33b3f, 0x2f6e6d16),
    STCP(0x76bb5521, 0x2fd1f5c1), STCP(0x76931bae, 0x30355cdd), STCP(0x766a8f04, 0x3098a223),
    STCP(0x7641af3d, 0x30fbc54d), STCP(0x76187c77, 0x315ec617), STCP(0x75eef6ce, 0x31c1a43b),
    STCP(0x75c51e61, 0x32245f72), STCP(0x759af34c, 0x3286f779), STCP(0x757075ac, 0x32e96c09),
    STCP(0x7545a5a0, 0x334bbcde), STCP(0x751a8346, 0x33ade9b3), STCP(0x74ef0ebc, 0x340ff242),
    STCP(0x74c34820, 0x3471d647), STCP(0x74972f92, 0x34d3957e), STCP(0x746ac52f, 0x35352fa1),
    STCP(0x743e0918, 0x3596a46c), STCP(0x7410fb6b, 0x35f7f39c), STCP(0x73e39c49, 0x36591cea),
    STCP(0x73b5ebd1, 0x36ba2014), STCP(0x7387ea23, 0x371afcd5), STCP(0x73599760, 0x377bb2e9),
    STCP(0x732af3a7, 0x37dc420c), STCP(0x72fbff1b, 0x383ca9fb), STCP(0x72ccb9db, 0x389cea72),
    STCP(0x729d2409, 0x38fd032d), STCP(0x726d3dc6, 0x395cf3e9), STCP(0x723d0734, 0x39bcbc63),
    STCP(0x720c8075, 0x3a1c5c57), STCP(0x71dba9ab, 0x3a7bd382), STCP(0x71aa82f7, 0x3adb21a1),
    STCP(0x71790c7e, 0x3b3a4672), STCP(0x71474660, 0x3b9941b1), STCP(0x711530c2, 0x3bf8131c),
    STCP(0x70e2cbc6, 0x3c56ba70), STCP(0x70b01790, 0x3cb5376b), STCP(0x707d1443, 0x3d1389cb),
    STCP(0x7049c203, 0x3d71b14d), STCP(0x701620f5, 0x3dcfadb0), STCP(0x6fe2313c, 0x3e2d7eb1),
    STCP(0x6fadf2fc, 0x3e8b240e), STCP(0x6f79665b, 0x3ee89d86), STCP(0x6f448b7e, 0x3f45ead8),
    STCP(0x6f0f6289, 0x3fa30bc1), STCP(0x6ed9eba1, 0x40000000), STCP(0x6ea426ed, 0x405cc754),
    STCP(0x6e6e1492, 0x40b9617d), STCP(0x6e37b4b6, 0x4115ce38), STCP(0x6e010780, 0x41720d46),
    STCP(0x6dca0d14, 0x41ce1e65), STCP(0x6d92c59b, 0x422a0154), STCP(0x6d5b313b, 0x4285b5d4),
    STCP(0x6d23501b, 0x42e13ba4), STCP(0x6ceb2261, 0x433c9283), STCP(0x6cb2a837, 0x4397ba32),
    STCP(0x6c79e1c2, 0x43f2b271), STCP(0x6c40cf2c, 0x444d7aff), STCP(0x6c07709b, 0x44a8139e),
    STCP(0x6bcdc639, 0x45027c0c), STCP(0x6b93d02e, 0x455cb40c), STCP(0x6b598ea3, 0x45b6bb5e),
    STCP(0x6b1f01c0, 0x461091c2), STCP(0x6ae429ae, 0x466a36f9), STCP(0x6aa90697, 0x46c3aac5),
    STCP(0x6a6d98a4, 0x471cece7), STCP(0x6a31e000, 0x4775fd1f), STCP(0x69f5dcd3, 0x47cedb31),
    STCP(0x69b98f48, 0x482786dc), STCP(0x697cf78a, 0x487fffe4), STCP(0x694015c3, 0x48d84609),
    STCP(0x6902ea1d, 0x4930590f), STCP(0x68c574c4, 0x498838b6), STCP(0x6887b5e2, 0x49dfe4c2),
    STCP(0x6849ada3, 0x4a375cf5), STCP(0x680b5c33, 0x4a8ea111), STCP(0x67ccc1be, 0x4ae5b0da),
    STCP(0x678dde6e, 0x4b3c8c12), STCP(0x674eb271, 0x4b93327c), STCP(0x670f3df3, 0x4be9a3db),
    STCP(0x66cf8120, 0x4c3fdff4), STCP(0x668f7c25, 0x4c95e688), STCP(0x664f2f2e, 0x4cebb75c),
    STCP(0x660e9a6a, 0x4d415234), STCP(0x65cdbe05, 0x4d96b6d3), STCP(0x658c9a2d, 0x4debe4fe),
    STCP(0x654b2f10, 0x4e40dc79), STCP(0x65097cdb, 0x4e959d08), STCP(0x64c783bd, 0x4eea2670),
    STCP(0x648543e4, 0x4f3e7875), STCP(0x6442bd7e, 0x4f9292dc), STCP(0x63fff0ba, 0x4fe6756a),
    STCP(0x63bcddc7, 0x503a1fe5), STCP(0x637984d4, 0x508d9211), STCP(0x6335e611, 0x50e0cbb4),
    STCP(0x62f201ac, 0x5133cc94), STCP(0x62add7d6, 0x51869476), STCP(0x626968be, 0x51d92321),
    STCP(0x6224b495, 0x522b7859), STCP(0x61dfbb8a, 0x527d93e6), STCP(0x619a7dce, 0x52cf758f),
    STCP(0x6154fb91, 0x53211d18), STCP(0x610f3505, 0x53728a4a), STCP(0x60c92a5a, 0x53c3bcea),
    STCP(0x6082dbc1, 0x5414b4c1), STCP(0x603c496c, 0x54657194), STCP(0x5ff5738d, 0x54b5f32c),
    STCP(0x5fae5a55, 0x55063951), STCP(0x5f66fdf5, 0x555643c8), STCP(0x5f1f5ea1, 0x55a6125c),
    STCP(0x5ed77c8a, 0x55f5a4d2), STCP(0x5e8f57e2, 0x5644faf4), STCP(0x5e46f0dd, 0x5694148b),
    STCP(0x5dfe47ad, 0x56e2f15d), STCP(0x5db55c86, 0x57319135), STCP(0x5d6c2f99, 0x577ff3da),
    STCP(0x5d22c11c, 0x57ce1917), STCP(0x5cd91140, 0x581c00b3), STCP(0x5c8f203b, 0x5869aa79),
    STCP(0x5c44ee40, 0x58b71632), STCP(0x5bfa7b82, 0x590443a7), STCP(0x5bafc837, 0x595132a2),
    STCP(0x5b64d492, 0x599de2ee), STCP(0x5b19a0c8, 0x59ea5454), STCP(0x5ace2d0f, 0x5a36869f),
    STCP(0x5a82799a, 0x5a82799a),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STP SineTable512[] = {
    STCP(0x7fffffff, 0x00000000), STCP(0x7fffd886, 0x006487e3), STCP(0x7fff6216, 0x00c90f88),
    STCP(0x7ffe9cb2, 0x012d96b1), STCP(0x7ffd885a, 0x01921d20), STCP(0x7ffc250f, 0x01f6a297),
    STCP(0x7ffa72d1, 0x025b26d7), STCP(0x7ff871a2, 0x02bfa9a4), STCP(0x7ff62182, 0x03242abf),
    STCP(0x7ff38274, 0x0388a9ea), STCP(0x7ff09478, 0x03ed26e6), STCP(0x7fed5791, 0x0451a177),
    STCP(0x7fe9cbc0, 0x04b6195d), STCP(0x7fe5f108, 0x051a8e5c), STCP(0x7fe1c76b, 0x057f0035),
    STCP(0x7fdd4eec, 0x05e36ea9), STCP(0x7fd8878e, 0x0647d97c), STCP(0x7fd37153, 0x06ac406f),
    STCP(0x7fce0c3e, 0x0710a345), STCP(0x7fc85854, 0x077501be), STCP(0x7fc25596, 0x07d95b9e),
    STCP(0x7fbc040a, 0x083db0a7), STCP(0x7fb563b3, 0x08a2009a), STCP(0x7fae7495, 0x09064b3a),
    STCP(0x7fa736b4, 0x096a9049), STCP(0x7f9faa15, 0x09cecf89), STCP(0x7f97cebd, 0x0a3308bd),
    STCP(0x7f8fa4b0, 0x0a973ba5), STCP(0x7f872bf3, 0x0afb6805), STCP(0x7f7e648c, 0x0b5f8d9f),
    STCP(0x7f754e80, 0x0bc3ac35), STCP(0x7f6be9d4, 0x0c27c389), STCP(0x7f62368f, 0x0c8bd35e),
    STCP(0x7f5834b7, 0x0cefdb76), STCP(0x7f4de451, 0x0d53db92), STCP(0x7f434563, 0x0db7d376),
    STCP(0x7f3857f6, 0x0e1bc2e4), STCP(0x7f2d1c0e, 0x0e7fa99e), STCP(0x7f2191b4, 0x0ee38766),
    STCP(0x7f15b8ee, 0x0f475bff), STCP(0x7f0991c4, 0x0fab272b), STCP(0x7efd1c3c, 0x100ee8ad),
    STCP(0x7ef05860, 0x1072a048), STCP(0x7ee34636, 0x10d64dbd), STCP(0x7ed5e5c6, 0x1139f0cf),
    STCP(0x7ec8371a, 0x119d8941), STCP(0x7eba3a39, 0x120116d5), STCP(0x7eabef2c, 0x1264994e),
    STCP(0x7e9d55fc, 0x12c8106f), STCP(0x7e8e6eb2, 0x132b7bf9), STCP(0x7e7f3957, 0x138edbb1),
    STCP(0x7e6fb5f4, 0x13f22f58), STCP(0x7e5fe493, 0x145576b1), STCP(0x7e4fc53e, 0x14b8b17f),
    STCP(0x7e3f57ff, 0x151bdf86), STCP(0x7e2e9cdf, 0x157f0086), STCP(0x7e1d93ea, 0x15e21445),
    STCP(0x7e0c3d29, 0x16451a83), STCP(0x7dfa98a8, 0x16a81305), STCP(0x7de8a670, 0x170afd8d),
    STCP(0x7dd6668f, 0x176dd9de), STCP(0x7dc3d90d, 0x17d0a7bc), STCP(0x7db0fdf8, 0x183366e9),
    STCP(0x7d9dd55a, 0x18961728), STCP(0x7d8a5f40, 0x18f8b83c), STCP(0x7d769bb5, 0x195b49ea),
    STCP(0x7d628ac6, 0x19bdcbf3), STCP(0x7d4e2c7f, 0x1a203e1b), STCP(0x7d3980ec, 0x1a82a026),
    STCP(0x7d24881b, 0x1ae4f1d6), STCP(0x7d0f4218, 0x1b4732ef), STCP(0x7cf9aef0, 0x1ba96335),
    STCP(0x7ce3ceb2, 0x1c0b826a), STCP(0x7ccda169, 0x1c6d9053), STCP(0x7cb72724, 0x1ccf8cb3),
    STCP(0x7ca05ff1, 0x1d31774d), STCP(0x7c894bde, 0x1d934fe5), STCP(0x7c71eaf9, 0x1df5163f),
    STCP(0x7c5a3d50, 0x1e56ca1e), STCP(0x7c4242f2, 0x1eb86b46), STCP(0x7c29fbee, 0x1f19f97b),
    STCP(0x7c116853, 0x1f7b7481), STCP(0x7bf88830, 0x1fdcdc1b), STCP(0x7bdf5b94, 0x203e300d),
    STCP(0x7bc5e290, 0x209f701c), STCP(0x7bac1d31, 0x21009c0c), STCP(0x7b920b89, 0x2161b3a0),
    STCP(0x7b77ada8, 0x21c2b69c), STCP(0x7b5d039e, 0x2223a4c5), STCP(0x7b420d7a, 0x22847de0),
    STCP(0x7b26cb4f, 0x22e541af), STCP(0x7b0b3d2c, 0x2345eff8), STCP(0x7aef6323, 0x23a6887f),
    STCP(0x7ad33d45, 0x24070b08), STCP(0x7ab6cba4, 0x24677758), STCP(0x7a9a0e50, 0x24c7cd33),
    STCP(0x7a7d055b, 0x25280c5e), STCP(0x7a5fb0d8, 0x2588349d), STCP(0x7a4210d8, 0x25e845b6),
    STCP(0x7a24256f, 0x26483f6c), STCP(0x7a05eead, 0x26a82186), STCP(0x79e76ca7, 0x2707ebc7),
    STCP(0x79c89f6e, 0x27679df4), STCP(0x79a98715, 0x27c737d3), STCP(0x798a23b1, 0x2826b928),
    STCP(0x796a7554, 0x288621b9), STCP(0x794a7c12, 0x28e5714b), STCP(0x792a37fe, 0x2944a7a2),
    STCP(0x7909a92d, 0x29a3c485), STCP(0x78e8cfb2, 0x2a02c7b8), STCP(0x78c7aba2, 0x2a61b101),
    STCP(0x78a63d11, 0x2ac08026), STCP(0x78848414, 0x2b1f34eb), STCP(0x786280bf, 0x2b7dcf17),
    STCP(0x78403329, 0x2bdc4e6f), STCP(0x781d9b65, 0x2c3ab2b9), STCP(0x77fab989, 0x2c98fbba),
    STCP(0x77d78daa, 0x2cf72939), STCP(0x77b417df, 0x2d553afc), STCP(0x7790583e, 0x2db330c7),
    STCP(0x776c4edb, 0x2e110a62), STCP(0x7747fbce, 0x2e6ec792), STCP(0x77235f2d, 0x2ecc681e),
    STCP(0x76fe790e, 0x2f29ebcc), STCP(0x76d94989, 0x2f875262), STCP(0x76b3d0b4, 0x2fe49ba7),
    STCP(0x768e0ea6, 0x3041c761), STCP(0x76680376, 0x309ed556), STCP(0x7641af3d, 0x30fbc54d),
    STCP(0x761b1211, 0x3158970e), STCP(0x75f42c0b, 0x31b54a5e), STCP(0x75ccfd42, 0x3211df04),
    STCP(0x75a585cf, 0x326e54c7), STCP(0x757dc5ca, 0x32caab6f), STCP(0x7555bd4c, 0x3326e2c3),
    STCP(0x752d6c6c, 0x3382fa88), STCP(0x7504d345, 0x33def287), STCP(0x74dbf1ef, 0x343aca87),
    STCP(0x74b2c884, 0x34968250), STCP(0x7489571c, 0x34f219a8), STCP(0x745f9dd1, 0x354d9057),
    STCP(0x74359cbd, 0x35a8e625), STCP(0x740b53fb, 0x36041ad9), STCP(0x73e0c3a3, 0x365f2e3b),
    STCP(0x73b5ebd1, 0x36ba2014), STCP(0x738acc9e, 0x3714f02a), STCP(0x735f6626, 0x376f9e46),
    STCP(0x7333b883, 0x37ca2a30), STCP(0x7307c3d0, 0x382493b0), STCP(0x72db8828, 0x387eda8e),
    STCP(0x72af05a7, 0x38d8fe93), STCP(0x72823c67, 0x3932ff87), STCP(0x72552c85, 0x398cdd32),
    STCP(0x7227d61c, 0x39e6975e), STCP(0x71fa3949, 0x3a402dd2), STCP(0x71cc5626, 0x3a99a057),
    STCP(0x719e2cd2, 0x3af2eeb7), STCP(0x716fbd68, 0x3b4c18ba), STCP(0x71410805, 0x3ba51e29),
    STCP(0x71120cc5, 0x3bfdfecd), STCP(0x70e2cbc6, 0x3c56ba70), STCP(0x70b34525, 0x3caf50da),
    STCP(0x708378ff, 0x3d07c1d6), STCP(0x70536771, 0x3d600d2c), STCP(0x7023109a, 0x3db832a6),
    STCP(0x6ff27497, 0x3e10320d), STCP(0x6fc19385, 0x3e680b2c), STCP(0x6f906d84, 0x3ebfbdcd),
    STCP(0x6f5f02b2, 0x3f1749b8), STCP(0x6f2d532c, 0x3f6eaeb8), STCP(0x6efb5f12, 0x3fc5ec98),
    STCP(0x6ec92683, 0x401d0321), STCP(0x6e96a99d, 0x4073f21d), STCP(0x6e63e87f, 0x40cab958),
    STCP(0x6e30e34a, 0x4121589b), STCP(0x6dfd9a1c, 0x4177cfb1), STCP(0x6dca0d14, 0x41ce1e65),
    STCP(0x6d963c54, 0x42244481), STCP(0x6d6227fa, 0x427a41d0), STCP(0x6d2dd027, 0x42d0161e),
    STCP(0x6cf934fc, 0x4325c135), STCP(0x6cc45698, 0x437b42e1), STCP(0x6c8f351c, 0x43d09aed),
    STCP(0x6c59d0a9, 0x4425c923), STCP(0x6c242960, 0x447acd50), STCP(0x6bee3f62, 0x44cfa740),
    STCP(0x6bb812d1, 0x452456bd), STCP(0x6b81a3cd, 0x4578db93), STCP(0x6b4af279, 0x45cd358f),
    STCP(0x6b13fef5, 0x4621647d), STCP(0x6adcc964, 0x46756828), STCP(0x6aa551e9, 0x46c9405c),
    STCP(0x6a6d98a4, 0x471cece7), STCP(0x6a359db9, 0x47706d93), STCP(0x69fd614a, 0x47c3c22f),
    STCP(0x69c4e37a, 0x4816ea86), STCP(0x698c246c, 0x4869e665), STCP(0x69532442, 0x48bcb599),
    STCP(0x6919e320, 0x490f57ee), STCP(0x68e06129, 0x4961cd33), STCP(0x68a69e81, 0x49b41533),
    STCP(0x686c9b4b, 0x4a062fbd), STCP(0x683257ab, 0x4a581c9e), STCP(0x67f7d3c5, 0x4aa9dba2),
    STCP(0x67bd0fbd, 0x4afb6c98), STCP(0x67820bb7, 0x4b4ccf4d), STCP(0x6746c7d8, 0x4b9e0390),
    STCP(0x670b4444, 0x4bef092d), STCP(0x66cf8120, 0x4c3fdff4), STCP(0x66937e91, 0x4c9087b1),
    STCP(0x66573cbb, 0x4ce10034), STCP(0x661abbc5, 0x4d31494b), STCP(0x65ddfbd3, 0x4d8162c4),
    STCP(0x65a0fd0b, 0x4dd14c6e), STCP(0x6563bf92, 0x4e210617), STCP(0x6526438f, 0x4e708f8f),
    STCP(0x64e88926, 0x4ebfe8a5), STCP(0x64aa907f, 0x4f0f1126), STCP(0x646c59bf, 0x4f5e08e3),
    STCP(0x642de50d, 0x4faccfab), STCP(0x63ef3290, 0x4ffb654d), STCP(0x63b0426d, 0x5049c999),
    STCP(0x637114cc, 0x5097fc5e), STCP(0x6331a9d4, 0x50e5fd6d), STCP(0x62f201ac, 0x5133cc94),
    STCP(0x62b21c7b, 0x518169a5), STCP(0x6271fa69, 0x51ced46e), STCP(0x62319b9d, 0x521c0cc2),
    STCP(0x61f1003f, 0x5269126e), STCP(0x61b02876, 0x52b5e546), STCP(0x616f146c, 0x53028518),
    STCP(0x612dc447, 0x534ef1b5), STCP(0x60ec3830, 0x539b2af0), STCP(0x60aa7050, 0x53e73097),
    STCP(0x60686ccf, 0x5433027d), STCP(0x60262dd6, 0x547ea073), STCP(0x5fe3b38d, 0x54ca0a4b),
    STCP(0x5fa0fe1f, 0x55153fd4), STCP(0x5f5e0db3, 0x556040e2), STCP(0x5f1ae274, 0x55ab0d46),
    STCP(0x5ed77c8a, 0x55f5a4d2), STCP(0x5e93dc1f, 0x56400758), STCP(0x5e50015d, 0x568a34a9),
    STCP(0x5e0bec6e, 0x56d42c99), STCP(0x5dc79d7c, 0x571deefa), STCP(0x5d8314b1, 0x57677b9d),
    STCP(0x5d3e5237, 0x57b0d256), STCP(0x5cf95638, 0x57f9f2f8), STCP(0x5cb420e0, 0x5842dd54),
    STCP(0x5c6eb258, 0x588b9140), STCP(0x5c290acc, 0x58d40e8c), STCP(0x5be32a67, 0x591c550e),
    STCP(0x5b9d1154, 0x59646498), STCP(0x5b56bfbd, 0x59ac3cfd), STCP(0x5b1035cf, 0x59f3de12),
    STCP(0x5ac973b5, 0x5a3b47ab), STCP(0x5a82799a, 0x5a82799a),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STP SineTable1024[] = {
    STCP(0x7fffffff, 0x00000000), STCP(0x7ffff621, 0x003243f5), STCP(0x7fffd886, 0x006487e3),
    STCP(0x7fffa72c, 0x0096cbc1), STCP(0x7fff6216, 0x00c90f88), STCP(0x7fff0943, 0x00fb5330),
    STCP(0x7ffe9cb2, 0x012d96b1), STCP(0x7ffe1c65, 0x015fda03), STCP(0x7ffd885a, 0x01921d20),
    STCP(0x7ffce093, 0x01c45ffe), STCP(0x7ffc250f, 0x01f6a297), STCP(0x7ffb55ce, 0x0228e4e2),
    STCP(0x7ffa72d1, 0x025b26d7), STCP(0x7ff97c18, 0x028d6870), STCP(0x7ff871a2, 0x02bfa9a4),
    STCP(0x7ff75370, 0x02f1ea6c), STCP(0x7ff62182, 0x03242abf), STCP(0x7ff4dbd9, 0x03566a96),
    STCP(0x7ff38274, 0x0388a9ea), STCP(0x7ff21553, 0x03bae8b2), STCP(0x7ff09478, 0x03ed26e6),
    STCP(0x7feeffe1, 0x041f6480), STCP(0x7fed5791, 0x0451a177), STCP(0x7feb9b85, 0x0483ddc3),
    STCP(0x7fe9cbc0, 0x04b6195d), STCP(0x7fe7e841, 0x04e8543e), STCP(0x7fe5f108, 0x051a8e5c),
    STCP(0x7fe3e616, 0x054cc7b1), STCP(0x7fe1c76b, 0x057f0035), STCP(0x7fdf9508, 0x05b137df),
    STCP(0x7fdd4eec, 0x05e36ea9), STCP(0x7fdaf519, 0x0615a48b), STCP(0x7fd8878e, 0x0647d97c),
    STCP(0x7fd6064c, 0x067a0d76), STCP(0x7fd37153, 0x06ac406f), STCP(0x7fd0c8a3, 0x06de7262),
    STCP(0x7fce0c3e, 0x0710a345), STCP(0x7fcb3c23, 0x0742d311), STCP(0x7fc85854, 0x077501be),
    STCP(0x7fc560cf, 0x07a72f45), STCP(0x7fc25596, 0x07d95b9e), STCP(0x7fbf36aa, 0x080b86c2),
    STCP(0x7fbc040a, 0x083db0a7), STCP(0x7fb8bdb8, 0x086fd947), STCP(0x7fb563b3, 0x08a2009a),
    STCP(0x7fb1f5fc, 0x08d42699), STCP(0x7fae7495, 0x09064b3a), STCP(0x7faadf7c, 0x09386e78),
    STCP(0x7fa736b4, 0x096a9049), STCP(0x7fa37a3c, 0x099cb0a7), STCP(0x7f9faa15, 0x09cecf89),
    STCP(0x7f9bc640, 0x0a00ece8), STCP(0x7f97cebd, 0x0a3308bd), STCP(0x7f93c38c, 0x0a6522fe),
    STCP(0x7f8fa4b0, 0x0a973ba5), STCP(0x7f8b7227, 0x0ac952aa), STCP(0x7f872bf3, 0x0afb6805),
    STCP(0x7f82d214, 0x0b2d7baf), STCP(0x7f7e648c, 0x0b5f8d9f), STCP(0x7f79e35a, 0x0b919dcf),
    STCP(0x7f754e80, 0x0bc3ac35), STCP(0x7f70a5fe, 0x0bf5b8cb), STCP(0x7f6be9d4, 0x0c27c389),
    STCP(0x7f671a05, 0x0c59cc68), STCP(0x7f62368f, 0x0c8bd35e), STCP(0x7f5d3f75, 0x0cbdd865),
    STCP(0x7f5834b7, 0x0cefdb76), STCP(0x7f531655, 0x0d21dc87), STCP(0x7f4de451, 0x0d53db92),
    STCP(0x7f489eaa, 0x0d85d88f), STCP(0x7f434563, 0x0db7d376), STCP(0x7f3dd87c, 0x0de9cc40),
    STCP(0x7f3857f6, 0x0e1bc2e4), STCP(0x7f32c3d1, 0x0e4db75b), STCP(0x7f2d1c0e, 0x0e7fa99e),
    STCP(0x7f2760af, 0x0eb199a4), STCP(0x7f2191b4, 0x0ee38766), STCP(0x7f1baf1e, 0x0f1572dc),
    STCP(0x7f15b8ee, 0x0f475bff), STCP(0x7f0faf25, 0x0f7942c7), STCP(0x7f0991c4, 0x0fab272b),
    STCP(0x7f0360cb, 0x0fdd0926), STCP(0x7efd1c3c, 0x100ee8ad), STCP(0x7ef6c418, 0x1040c5bb),
    STCP(0x7ef05860, 0x1072a048), STCP(0x7ee9d914, 0x10a4784b), STCP(0x7ee34636, 0x10d64dbd),
    STCP(0x7edc9fc6, 0x11082096), STCP(0x7ed5e5c6, 0x1139f0cf), STCP(0x7ecf1837, 0x116bbe60),
    STCP(0x7ec8371a, 0x119d8941), STCP(0x7ec14270, 0x11cf516a), STCP(0x7eba3a39, 0x120116d5),
    STCP(0x7eb31e78, 0x1232d979), STCP(0x7eabef2c, 0x1264994e), STCP(0x7ea4ac58, 0x1296564d),
    STCP(0x7e9d55fc, 0x12c8106f), STCP(0x7e95ec1a, 0x12f9c7aa), STCP(0x7e8e6eb2, 0x132b7bf9),
    STCP(0x7e86ddc6, 0x135d2d53), STCP(0x7e7f3957, 0x138edbb1), STCP(0x7e778166, 0x13c0870a),
    STCP(0x7e6fb5f4, 0x13f22f58), STCP(0x7e67d703, 0x1423d492), STCP(0x7e5fe493, 0x145576b1),
    STCP(0x7e57dea7, 0x148715ae), STCP(0x7e4fc53e, 0x14b8b17f), STCP(0x7e47985b, 0x14ea4a1f),
    STCP(0x7e3f57ff, 0x151bdf86), STCP(0x7e37042a, 0x154d71aa), STCP(0x7e2e9cdf, 0x157f0086),
    STCP(0x7e26221f, 0x15b08c12), STCP(0x7e1d93ea, 0x15e21445), STCP(0x7e14f242, 0x16139918),
    STCP(0x7e0c3d29, 0x16451a83), STCP(0x7e0374a0, 0x1676987f), STCP(0x7dfa98a8, 0x16a81305),
    STCP(0x7df1a942, 0x16d98a0c), STCP(0x7de8a670, 0x170afd8d), STCP(0x7ddf9034, 0x173c6d80),
    STCP(0x7dd6668f, 0x176dd9de), STCP(0x7dcd2981, 0x179f429f), STCP(0x7dc3d90d, 0x17d0a7bc),
    STCP(0x7dba7534, 0x1802092c), STCP(0x7db0fdf8, 0x183366e9), STCP(0x7da77359, 0x1864c0ea),
    STCP(0x7d9dd55a, 0x18961728), STCP(0x7d9423fc, 0x18c7699b), STCP(0x7d8a5f40, 0x18f8b83c),
    STCP(0x7d808728, 0x192a0304), STCP(0x7d769bb5, 0x195b49ea), STCP(0x7d6c9ce9, 0x198c8ce7),
    STCP(0x7d628ac6, 0x19bdcbf3), STCP(0x7d58654d, 0x19ef0707), STCP(0x7d4e2c7f, 0x1a203e1b),
    STCP(0x7d43e05e, 0x1a517128), STCP(0x7d3980ec, 0x1a82a026), STCP(0x7d2f0e2b, 0x1ab3cb0d),
    STCP(0x7d24881b, 0x1ae4f1d6), STCP(0x7d19eebf, 0x1b161479), STCP(0x7d0f4218, 0x1b4732ef),
    STCP(0x7d048228, 0x1b784d30), STCP(0x7cf9aef0, 0x1ba96335), STCP(0x7ceec873, 0x1bda74f6),
    STCP(0x7ce3ceb2, 0x1c0b826a), STCP(0x7cd8c1ae, 0x1c3c8b8c), STCP(0x7ccda169, 0x1c6d9053),
    STCP(0x7cc26de5, 0x1c9e90b8), STCP(0x7cb72724, 0x1ccf8cb3), STCP(0x7cabcd28, 0x1d00843d),
    STCP(0x7ca05ff1, 0x1d31774d), STCP(0x7c94df83, 0x1d6265dd), STCP(0x7c894bde, 0x1d934fe5),
    STCP(0x7c7da505, 0x1dc4355e), STCP(0x7c71eaf9, 0x1df5163f), STCP(0x7c661dbc, 0x1e25f282),
    STCP(0x7c5a3d50, 0x1e56ca1e), STCP(0x7c4e49b7, 0x1e879d0d), STCP(0x7c4242f2, 0x1eb86b46),
    STCP(0x7c362904, 0x1ee934c3), STCP(0x7c29fbee, 0x1f19f97b), STCP(0x7c1dbbb3, 0x1f4ab968),
    STCP(0x7c116853, 0x1f7b7481), STCP(0x7c0501d2, 0x1fac2abf), STCP(0x7bf88830, 0x1fdcdc1b),
    STCP(0x7bebfb70, 0x200d888d), STCP(0x7bdf5b94, 0x203e300d), STCP(0x7bd2a89e, 0x206ed295),
    STCP(0x7bc5e290, 0x209f701c), STCP(0x7bb9096b, 0x20d0089c), STCP(0x7bac1d31, 0x21009c0c),
    STCP(0x7b9f1de6, 0x21312a65), STCP(0x7b920b89, 0x2161b3a0), STCP(0x7b84e61f, 0x219237b5),
    STCP(0x7b77ada8, 0x21c2b69c), STCP(0x7b6a6227, 0x21f3304f), STCP(0x7b5d039e, 0x2223a4c5),
    STCP(0x7b4f920e, 0x225413f8), STCP(0x7b420d7a, 0x22847de0), STCP(0x7b3475e5, 0x22b4e274),
    STCP(0x7b26cb4f, 0x22e541af), STCP(0x7b190dbc, 0x23159b88), STCP(0x7b0b3d2c, 0x2345eff8),
    STCP(0x7afd59a4, 0x23763ef7), STCP(0x7aef6323, 0x23a6887f), STCP(0x7ae159ae, 0x23d6cc87),
    STCP(0x7ad33d45, 0x24070b08), STCP(0x7ac50dec, 0x243743fa), STCP(0x7ab6cba4, 0x24677758),
    STCP(0x7aa8766f, 0x2497a517), STCP(0x7a9a0e50, 0x24c7cd33), STCP(0x7a8b9348, 0x24f7efa2),
    STCP(0x7a7d055b, 0x25280c5e), STCP(0x7a6e648a, 0x2558235f), STCP(0x7a5fb0d8, 0x2588349d),
    STCP(0x7a50ea47, 0x25b84012), STCP(0x7a4210d8, 0x25e845b6), STCP(0x7a332490, 0x26184581),
    STCP(0x7a24256f, 0x26483f6c), STCP(0x7a151378, 0x26783370), STCP(0x7a05eead, 0x26a82186),
    STCP(0x79f6b711, 0x26d809a5), STCP(0x79e76ca7, 0x2707ebc7), STCP(0x79d80f6f, 0x2737c7e3),
    STCP(0x79c89f6e, 0x27679df4), STCP(0x79b91ca4, 0x27976df1), STCP(0x79a98715, 0x27c737d3),
    STCP(0x7999dec4, 0x27f6fb92), STCP(0x798a23b1, 0x2826b928), STCP(0x797a55e0, 0x2856708d),
    STCP(0x796a7554, 0x288621b9), STCP(0x795a820e, 0x28b5cca5), STCP(0x794a7c12, 0x28e5714b),
    STCP(0x793a6361, 0x29150fa1), STCP(0x792a37fe, 0x2944a7a2), STCP(0x7919f9ec, 0x29743946),
    STCP(0x7909a92d, 0x29a3c485), STCP(0x78f945c3, 0x29d34958), STCP(0x78e8cfb2, 0x2a02c7b8),
    STCP(0x78d846fb, 0x2a323f9e), STCP(0x78c7aba2, 0x2a61b101), STCP(0x78b6fda8, 0x2a911bdc),
    STCP(0x78a63d11, 0x2ac08026), STCP(0x789569df, 0x2aefddd8), STCP(0x78848414, 0x2b1f34eb),
    STCP(0x78738bb3, 0x2b4e8558), STCP(0x786280bf, 0x2b7dcf17), STCP(0x7851633b, 0x2bad1221),
    STCP(0x78403329, 0x2bdc4e6f), STCP(0x782ef08b, 0x2c0b83fa), STCP(0x781d9b65, 0x2c3ab2b9),
    STCP(0x780c33b8, 0x2c69daa6), STCP(0x77fab989, 0x2c98fbba), STCP(0x77e92cd9, 0x2cc815ee),
    STCP(0x77d78daa, 0x2cf72939), STCP(0x77c5dc01, 0x2d263596), STCP(0x77b417df, 0x2d553afc),
    STCP(0x77a24148, 0x2d843964), STCP(0x7790583e, 0x2db330c7), STCP(0x777e5cc3, 0x2de2211e),
    STCP(0x776c4edb, 0x2e110a62), STCP(0x775a2e89, 0x2e3fec8b), STCP(0x7747fbce, 0x2e6ec792),
    STCP(0x7735b6af, 0x2e9d9b70), STCP(0x77235f2d, 0x2ecc681e), STCP(0x7710f54c, 0x2efb2d95),
    STCP(0x76fe790e, 0x2f29ebcc), STCP(0x76ebea77, 0x2f58a2be), STCP(0x76d94989, 0x2f875262),
    STCP(0x76c69647, 0x2fb5fab2), STCP(0x76b3d0b4, 0x2fe49ba7), STCP(0x76a0f8d2, 0x30133539),
    STCP(0x768e0ea6, 0x3041c761), STCP(0x767b1231, 0x30705217), STCP(0x76680376, 0x309ed556),
    STCP(0x7654e279, 0x30cd5115), STCP(0x7641af3d, 0x30fbc54d), STCP(0x762e69c4, 0x312a31f8),
    STCP(0x761b1211, 0x3158970e), STCP(0x7607a828, 0x3186f487), STCP(0x75f42c0b, 0x31b54a5e),
    STCP(0x75e09dbd, 0x31e39889), STCP(0x75ccfd42, 0x3211df04), STCP(0x75b94a9c, 0x32401dc6),
    STCP(0x75a585cf, 0x326e54c7), STCP(0x7591aedd, 0x329c8402), STCP(0x757dc5ca, 0x32caab6f),
    STCP(0x7569ca99, 0x32f8cb07), STCP(0x7555bd4c, 0x3326e2c3), STCP(0x75419de7, 0x3354f29b),
    STCP(0x752d6c6c, 0x3382fa88), STCP(0x751928e0, 0x33b0fa84), STCP(0x7504d345, 0x33def287),
    STCP(0x74f06b9e, 0x340ce28b), STCP(0x74dbf1ef, 0x343aca87), STCP(0x74c7663a, 0x3468aa76),
    STCP(0x74b2c884, 0x34968250), STCP(0x749e18cd, 0x34c4520d), STCP(0x7489571c, 0x34f219a8),
    STCP(0x74748371, 0x351fd918), STCP(0x745f9dd1, 0x354d9057), STCP(0x744aa63f, 0x357b3f5d),
    STCP(0x74359cbd, 0x35a8e625), STCP(0x74208150, 0x35d684a6), STCP(0x740b53fb, 0x36041ad9),
    STCP(0x73f614c0, 0x3631a8b8), STCP(0x73e0c3a3, 0x365f2e3b), STCP(0x73cb60a8, 0x368cab5c),
    STCP(0x73b5ebd1, 0x36ba2014), STCP(0x73a06522, 0x36e78c5b), STCP(0x738acc9e, 0x3714f02a),
    STCP(0x73752249, 0x37424b7b), STCP(0x735f6626, 0x376f9e46), STCP(0x73499838, 0x379ce885),
    STCP(0x7333b883, 0x37ca2a30), STCP(0x731dc70a, 0x37f76341), STCP(0x7307c3d0, 0x382493b0),
    STCP(0x72f1aed9, 0x3851bb77), STCP(0x72db8828, 0x387eda8e), STCP(0x72c54fc1, 0x38abf0ef),
    STCP(0x72af05a7, 0x38d8fe93), STCP(0x7298a9dd, 0x39060373), STCP(0x72823c67, 0x3932ff87),
    STCP(0x726bbd48, 0x395ff2c9), STCP(0x72552c85, 0x398cdd32), STCP(0x723e8a20, 0x39b9bebc),
    STCP(0x7227d61c, 0x39e6975e), STCP(0x7211107e, 0x3a136712), STCP(0x71fa3949, 0x3a402dd2),
    STCP(0x71e35080, 0x3a6ceb96), STCP(0x71cc5626, 0x3a99a057), STCP(0x71b54a41, 0x3ac64c0f),
    STCP(0x719e2cd2, 0x3af2eeb7), STCP(0x7186fdde, 0x3b1f8848), STCP(0x716fbd68, 0x3b4c18ba),
    STCP(0x71586b74, 0x3b78a007), STCP(0x71410805, 0x3ba51e29), STCP(0x7129931f, 0x3bd19318),
    STCP(0x71120cc5, 0x3bfdfecd), STCP(0x70fa74fc, 0x3c2a6142), STCP(0x70e2cbc6, 0x3c56ba70),
    STCP(0x70cb1128, 0x3c830a50), STCP(0x70b34525, 0x3caf50da), STCP(0x709b67c0, 0x3cdb8e09),
    STCP(0x708378ff, 0x3d07c1d6), STCP(0x706b78e3, 0x3d33ec39), STCP(0x70536771, 0x3d600d2c),
    STCP(0x703b44ad, 0x3d8c24a8), STCP(0x7023109a, 0x3db832a6), STCP(0x700acb3c, 0x3de4371f),
    STCP(0x6ff27497, 0x3e10320d), STCP(0x6fda0cae, 0x3e3c2369), STCP(0x6fc19385, 0x3e680b2c),
    STCP(0x6fa90921, 0x3e93e950), STCP(0x6f906d84, 0x3ebfbdcd), STCP(0x6f77c0b3, 0x3eeb889c),
    STCP(0x6f5f02b2, 0x3f1749b8), STCP(0x6f463383, 0x3f430119), STCP(0x6f2d532c, 0x3f6eaeb8),
    STCP(0x6f1461b0, 0x3f9a5290), STCP(0x6efb5f12, 0x3fc5ec98), STCP(0x6ee24b57, 0x3ff17cca),
    STCP(0x6ec92683, 0x401d0321), STCP(0x6eaff099, 0x40487f94), STCP(0x6e96a99d, 0x4073f21d),
    STCP(0x6e7d5193, 0x409f5ab6), STCP(0x6e63e87f, 0x40cab958), STCP(0x6e4a6e66, 0x40f60dfb),
    STCP(0x6e30e34a, 0x4121589b), STCP(0x6e174730, 0x414c992f), STCP(0x6dfd9a1c, 0x4177cfb1),
    STCP(0x6de3dc11, 0x41a2fc1a), STCP(0x6dca0d14, 0x41ce1e65), STCP(0x6db02d29, 0x41f93689),
    STCP(0x6d963c54, 0x42244481), STCP(0x6d7c3a98, 0x424f4845), STCP(0x6d6227fa, 0x427a41d0),
    STCP(0x6d48047e, 0x42a5311b), STCP(0x6d2dd027, 0x42d0161e), STCP(0x6d138afb, 0x42faf0d4),
    STCP(0x6cf934fc, 0x4325c135), STCP(0x6cdece2f, 0x4350873c), STCP(0x6cc45698, 0x437b42e1),
    STCP(0x6ca9ce3b, 0x43a5f41e), STCP(0x6c8f351c, 0x43d09aed), STCP(0x6c748b3f, 0x43fb3746),
    STCP(0x6c59d0a9, 0x4425c923), STCP(0x6c3f055d, 0x4450507e), STCP(0x6c242960, 0x447acd50),
    STCP(0x6c093cb6, 0x44a53f93), STCP(0x6bee3f62, 0x44cfa740), STCP(0x6bd3316a, 0x44fa0450),
    STCP(0x6bb812d1, 0x452456bd), STCP(0x6b9ce39b, 0x454e9e80), STCP(0x6b81a3cd, 0x4578db93),
    STCP(0x6b66536b, 0x45a30df0), STCP(0x6b4af279, 0x45cd358f), STCP(0x6b2f80fb, 0x45f7526b),
    STCP(0x6b13fef5, 0x4621647d), STCP(0x6af86c6c, 0x464b6bbe), STCP(0x6adcc964, 0x46756828),
    STCP(0x6ac115e2, 0x469f59b4), STCP(0x6aa551e9, 0x46c9405c), STCP(0x6a897d7d, 0x46f31c1a),
    STCP(0x6a6d98a4, 0x471cece7), STCP(0x6a51a361, 0x4746b2bc), STCP(0x6a359db9, 0x47706d93),
    STCP(0x6a1987b0, 0x479a1d67), STCP(0x69fd614a, 0x47c3c22f), STCP(0x69e12a8c, 0x47ed5be6),
    STCP(0x69c4e37a, 0x4816ea86), STCP(0x69a88c19, 0x48406e08), STCP(0x698c246c, 0x4869e665),
    STCP(0x696fac78, 0x48935397), STCP(0x69532442, 0x48bcb599), STCP(0x69368bce, 0x48e60c62),
    STCP(0x6919e320, 0x490f57ee), STCP(0x68fd2a3d, 0x49389836), STCP(0x68e06129, 0x4961cd33),
    STCP(0x68c387e9, 0x498af6df), STCP(0x68a69e81, 0x49b41533), STCP(0x6889a4f6, 0x49dd282a),
    STCP(0x686c9b4b, 0x4a062fbd), STCP(0x684f8186, 0x4a2f2be6), STCP(0x683257ab, 0x4a581c9e),
    STCP(0x68151dbe, 0x4a8101de), STCP(0x67f7d3c5, 0x4aa9dba2), STCP(0x67da79c3, 0x4ad2a9e2),
    STCP(0x67bd0fbd, 0x4afb6c98), STCP(0x679f95b7, 0x4b2423be), STCP(0x67820bb7, 0x4b4ccf4d),
    STCP(0x676471c0, 0x4b756f40), STCP(0x6746c7d8, 0x4b9e0390), STCP(0x67290e02, 0x4bc68c36),
    STCP(0x670b4444, 0x4bef092d), STCP(0x66ed6aa1, 0x4c177a6e), STCP(0x66cf8120, 0x4c3fdff4),
    STCP(0x66b187c3, 0x4c6839b7), STCP(0x66937e91, 0x4c9087b1), STCP(0x6675658c, 0x4cb8c9dd),
    STCP(0x66573cbb, 0x4ce10034), STCP(0x66390422, 0x4d092ab0), STCP(0x661abbc5, 0x4d31494b),
    STCP(0x65fc63a9, 0x4d595bfe), STCP(0x65ddfbd3, 0x4d8162c4), STCP(0x65bf8447, 0x4da95d96),
    STCP(0x65a0fd0b, 0x4dd14c6e), STCP(0x65826622, 0x4df92f46), STCP(0x6563bf92, 0x4e210617),
    STCP(0x6545095f, 0x4e48d0dd), STCP(0x6526438f, 0x4e708f8f), STCP(0x65076e25, 0x4e984229),
    STCP(0x64e88926, 0x4ebfe8a5), STCP(0x64c99498, 0x4ee782fb), STCP(0x64aa907f, 0x4f0f1126),
    STCP(0x648b7ce0, 0x4f369320), STCP(0x646c59bf, 0x4f5e08e3), STCP(0x644d2722, 0x4f857269),
    STCP(0x642de50d, 0x4faccfab), STCP(0x640e9386, 0x4fd420a4), STCP(0x63ef3290, 0x4ffb654d),
    STCP(0x63cfc231, 0x50229da1), STCP(0x63b0426d, 0x5049c999), STCP(0x6390b34a, 0x5070e92f),
    STCP(0x637114cc, 0x5097fc5e), STCP(0x635166f9, 0x50bf031f), STCP(0x6331a9d4, 0x50e5fd6d),
    STCP(0x6311dd64, 0x510ceb40), STCP(0x62f201ac, 0x5133cc94), STCP(0x62d216b3, 0x515aa162),
    STCP(0x62b21c7b, 0x518169a5), STCP(0x6292130c, 0x51a82555), STCP(0x6271fa69, 0x51ced46e),
    STCP(0x6251d298, 0x51f576ea), STCP(0x62319b9d, 0x521c0cc2), STCP(0x6211557e, 0x524295f0),
    STCP(0x61f1003f, 0x5269126e), STCP(0x61d09be5, 0x528f8238), STCP(0x61b02876, 0x52b5e546),
    STCP(0x618fa5f7, 0x52dc3b92), STCP(0x616f146c, 0x53028518), STCP(0x614e73da, 0x5328c1d0),
    STCP(0x612dc447, 0x534ef1b5), STCP(0x610d05b7, 0x537514c2), STCP(0x60ec3830, 0x539b2af0),
    STCP(0x60cb5bb7, 0x53c13439), STCP(0x60aa7050, 0x53e73097), STCP(0x60897601, 0x540d2005),
    STCP(0x60686ccf, 0x5433027d), STCP(0x604754bf, 0x5458d7f9), STCP(0x60262dd6, 0x547ea073),
    STCP(0x6004f819, 0x54a45be6), STCP(0x5fe3b38d, 0x54ca0a4b), STCP(0x5fc26038, 0x54efab9c),
    STCP(0x5fa0fe1f, 0x55153fd4), STCP(0x5f7f8d46, 0x553ac6ee), STCP(0x5f5e0db3, 0x556040e2),
    STCP(0x5f3c7f6b, 0x5585adad), STCP(0x5f1ae274, 0x55ab0d46), STCP(0x5ef936d1, 0x55d05faa),
    STCP(0x5ed77c8a, 0x55f5a4d2), STCP(0x5eb5b3a2, 0x561adcb9), STCP(0x5e93dc1f, 0x56400758),
    STCP(0x5e71f606, 0x566524aa), STCP(0x5e50015d, 0x568a34a9), STCP(0x5e2dfe29, 0x56af3750),
    STCP(0x5e0bec6e, 0x56d42c99), STCP(0x5de9cc33, 0x56f9147e), STCP(0x5dc79d7c, 0x571deefa),
    STCP(0x5da5604f, 0x5742bc06), STCP(0x5d8314b1, 0x57677b9d), STCP(0x5d60baa7, 0x578c2dba),
    STCP(0x5d3e5237, 0x57b0d256), STCP(0x5d1bdb65, 0x57d5696d), STCP(0x5cf95638, 0x57f9f2f8),
    STCP(0x5cd6c2b5, 0x581e6ef1), STCP(0x5cb420e0, 0x5842dd54), STCP(0x5c9170bf, 0x58673e1b),
    STCP(0x5c6eb258, 0x588b9140), STCP(0x5c4be5b0, 0x58afd6bd), STCP(0x5c290acc, 0x58d40e8c),
    STCP(0x5c0621b2, 0x58f838a9), STCP(0x5be32a67, 0x591c550e), STCP(0x5bc024f0, 0x594063b5),
    STCP(0x5b9d1154, 0x59646498), STCP(0x5b79ef96, 0x598857b2), STCP(0x5b56bfbd, 0x59ac3cfd),
    STCP(0x5b3381ce, 0x59d01475), STCP(0x5b1035cf, 0x59f3de12), STCP(0x5aecdbc5, 0x5a1799d1),
    STCP(0x5ac973b5, 0x5a3b47ab), STCP(0x5aa5fda5, 0x5a5ee79a), STCP(0x5a82799a, 0x5a82799a),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal6[] = {
        16384,
        -16384,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag6[] = {
        28378,
        28378,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal12[] = {
        28378,
        16384,
        16384,
        -16384,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag12[] = {
        16384,
        28378,
        28378,
        28378,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal24[] = {
        31651,     28378,     23170,     16384,     8481,     0,
        -8481,     -16384,     -23170,     -28378,     -31651,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag24[] = {
        8481,     16384,     23170,     28378,     31651,     32767,
        31651,     28378,     23170,     16384,     8481,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal48[] = {
        32488,     31651,     30274,     31651,     28378,     23170,
        30274,     23170,     12540,     28378,     16384,     0,
        25997,     8481,     -12540,     23170,     0,     -23170,
        19948,     -8481,     -30274,     16384,     -16384,     -32768,
        12540,     -23170,     -30274,     8481,     -28378,     -23170,
        4277,     -31651,     -12540,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag48[] = {
        4277,     8481,     12540,     8481,     16384,     23170,
        12540,     23170,     30274,     16384,     28378,     32767,
        19948,     31651,     30274,     23170,     32767,     23170,
        25997,     31651,     12540,     28378,     28378,     0,
        30274,     23170,     -12540,     31651,     16384,     -23170,
        32488,     8481,     -30274,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal80[] = {
        32667,     32365,     31863,     31164,     32365,     31164,
        29197,     26510,     31863,     29197,     24917,     19261,
        31164,     26510,     19261,     10126,     30274,     23170,
        12540,     0,     29197,     19261,     5126,     -10126,
        27939,     14876,     -2571,     -19261,     26510,     10126,
        -10126,     -26510,     24917,     5126,     -17121,     -31164,
        23170,     0,     -23170,     -32768,     21281,     -5126,
        -27939,     -31164,     19261,     -10126,     -31164,     -26510,
        17121,     -14876,     -32667,     -19261,     14876,     -19261,
        -32365,     -10126,     12540,     -23170,     -30274,     0,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag80[] = {
        2571,     5126,     7650,     10126,     5126,     10126,
        14876,     19261,     7650,     14876,     21281,     26510,
        10126,     19261,     26510,     31164,     12540,     23170,
        30274,     32767,     14876,     26510,     32365,     31164,
        17121,     29197,     32667,     26510,     19261,     31164,
        31164,     19261,     21281,     32365,     27939,     10126,
        23170,     32767,     23170,     0,     24917,     32365,
        17121,     -10126,     26510,     31164,     10126,     -19261,
        27939,     29197,     2571,     -26510,     29197,     26510,
        -5126,     -31164,     30274,     23170,     -12540,     -32768,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal96[] = {
        32698,     32488,     32488,     31651,     32138,     30274,
        31651,     28378,     31029,     25997,     30274,     23170,
        29389,     19948,     28378,     16384,     27246,     12540,
        25997,     8481,     24636,     4277,     23170,     0,
        21605,     -4277,     19948,     -8481,     18205,     -12540,
        16384,     -16384,     14493,     -19948,     12540,     -23170,
        10533,     -25997,     8481,     -28378,     6393,     -30274,
        4277,     -31651,     2143,     -32488,     0,     -32768,
        -2143,     -32488,     -4277,     -31651,     -6393,     -30274,
        -8481,     -28378,     -10533,     -25997,     -12540,     -23170,
        -14493,     -19948,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag96[] = {
        2143,     4277,     4277,     8481,     6393,     12540,
        8481,     16384,     10533,     19948,     12540,     23170,
        14493,     25997,     16384,     28378,     18205,     30274,
        19948,     31651,     21605,     32488,     23170,     32767,
        24636,     32488,     25997,     31651,     27246,     30274,
        28378,     28378,     29389,     25997,     30274,     23170,
        31029,     19948,     31651,     16384,     32138,     12540,
        32488,     8481,     32698,     4277,     32767,     0,
        32698,     -4277,     32488,     -8481,     32138,     -12540,
        31651,     -16384,     31029,     -19948,     30274,     -23170,
        29389,     -25997,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal384[] = {
        32764,     32750,     32729,     32698,     32658,     32610,
        32553,     32488,     32413,     32330,     32239,     32750,
        32698,     32610,     32488,     32330,     32138,     31912,
        31651,     31357,     31029,     30668,     32729,     32610,
        32413,     32138,     31786,     31357,     30853,     30274,
        29622,     28899,     28106,     32698,     32488,     32138,
        31651,     31029,     30274,     29389,     28378,     27246,
        25997,     24636,     32658,     32330,     31786,     31029,
        30064,     28899,     27540,     25997,     24279,     22400,
        20371,     32610,     32138,     31357,     30274,     28899,
        27246,     25330,     23170,     20788,     18205,     15447,
        32553,     31912,     30853,     29389,     27540,     25330,
        22788,     19948,     16846,     13524,     10024,     32488,
        31651,     30274,     28378,     25997,     23170,     19948,
        16384,     12540,     8481,     4277,     32413,     31357,
        29622,     27246,     24279,     20788,     16846,     12540,
        7962,     3212,     -1608,     32330,     31029,     28899,
        25997,     22400,     18205,     13524,     8481,     3212,
        -2143,     -7441,     32239,     30668,     28106,     24636,
        20371,     15447,     10024,     4277,     -1608,     -7441,
        -13033,     32138,     30274,     27246,     23170,     18205,
        12540,     6393,     0,     -6393,     -12540,     -18205,
        32029,     29847,     26320,     21605,     15917,     9512,
        2678,     -4277,     -11039,     -17304,     -22788,     31912,
        29389,     25330,     19948,     13524,     6393,     -1072,
        -8481,     -15447,     -21605,     -26635,     31786,     28899,
        24279,     18205,     11039,     3212,     -4808,     -12540,
        -19520,     -25330,     -29622,     31651,     28378,     23170,
        16384,     8481,     0,     -8481,     -16384,     -23170,
        -28378,     -31651,     31508,     27827,     22006,     14493,
        5866,     -3212,     -12043,     -19948,     -26320,     -30668,
        -32658,     31357,     27246,     20788,     12540,     3212,
        -6393,     -15447,     -23170,     -28899,     -32138,     -32610,
        31197,     26635,     19520,     10533,     536,     -9512,
        -18648,     -25997,     -30853,     -32750,     -31508,     31029,
        25997,     18205,     8481,     -2143,     -12540,     -21605,
        -28378,     -32138,     -32488,     -29389,     30853,     25330,
        16846,     6393,     -4808,     -15447,     -24279,     -30274,
        -32729,     -31357,     -26320,     30668,     24636,     15447,
        4277,     -7441,     -18205,     -26635,     -31651,     -32610,
        -29389,     -22400,     30475,     23916,     14010,     2143,
        -10024,     -20788,     -28642,     -32488,     -31786,     -26635,
        -17757,     30274,     23170,     12540,     0,     -12540,
        -23170,     -30274,     -32768,     -30274,     -23170,     -12540,
        30064,     22400,     11039,     -2143,     -14972,     -25330,
        -31508,     -32488,     -28106,     -19087,     -6918,     29847,
        21605,     9512,     -4277,     -17304,     -27246,     -32330,
        -31651,     -25330,     -14493,     -1072,     29622,     20788,
        7962,     -6393,     -19520,     -28899,     -32729,     -30274,
        -22006,     -9512,     4808,     29389,     19948,     6393,
        -8481,     -21605,     -30274,     -32698,     -28378,     -18205,
        -4277,     10533,     29148,     19087,     4808,     -10533,
        -23546,     -31357,     -32239,     -25997,     -14010,     1072,
        15917,     28899,     18205,     3212,     -12540,     -25330,
        -32138,     -31357,     -23170,     -9512,     6393,     20788,
        28642,     17304,     1608,     -14493,     -26944,     -32610,
        -30064,     -19948,     -4808,     11543,     24986,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag384[] = {
        536,     1072,     1608,     2143,     2678,     3212,
        3745,     4277,     4808,     5338,     5866,     1072,
        2143,     3212,     4277,     5338,     6393,     7441,
        8481,     9512,     10533,     11543,     1608,     3212,
        4808,     6393,     7962,     9512,     11039,     12540,
        14010,     15447,     16846,     2143,     4277,     6393,
        8481,     10533,     12540,     14493,     16384,     18205,
        19948,     21605,     2678,     5338,     7962,     10533,
        13033,     15447,     17757,     19948,     22006,     23916,
        25667,     3212,     6393,     9512,     12540,     15447,
        18205,     20788,     23170,     25330,     27246,     28899,
        3745,     7441,     11039,     14493,     17757,     20788,
        23546,     25997,     28106,     29847,     31197,     4277,
        8481,     12540,     16384,     19948,     23170,     25997,
        28378,     30274,     31651,     32488,     4808,     9512,
        14010,     18205,     22006,     25330,     28106,     30274,
        31786,     32610,     32729,     5338,     10533,     15447,
        19948,     23916,     27246,     29847,     31651,     32610,
        32698,     31912,     5866,     11543,     16846,     21605,
        25667,     28899,     31197,     32488,     32729,     31912,
        30064,     6393,     12540,     18205,     23170,     27246,
        30274,     32138,     32767,     32138,     30274,     27246,
        6918,     13524,     19520,     24636,     28642,     31357,
        32658,     32488,     30853,     27827,     23546,     7441,
        14493,     20788,     25997,     29847,     32138,     32750,
        31651,     28899,     24636,     19087,     7962,     15447,
        22006,     27246,     30853,     32610,     32413,     30274,
        26320,     20788,     14010,     8481,     16384,     23170,
        28378,     31651,     32767,     31651,     28378,     23170,
        16384,     8481,     8998,     17304,     24279,     29389,
        32239,     32610,     30475,     25997,     19520,     11543,
        2678,     9512,     18205,     25330,     30274,     32610,
        32138,     28899,     23170,     15447,     6393,     -3212,
        10024,     19087,     26320,     31029,     32764,     31357,
        26944,     19948,     11039,     1072,     -8998,     10533,
        19948,     27246,     31651,     32698,     30274,     24636,
        16384,     6393,     -4277,     -14493,     11039,     20788,
        28106,     32138,     32413,     28899,     22006,     12540,
        1608,     -9512,     -19520,     11543,     21605,     28899,
        32488,     31912,     27246,     19087,     8481,     -3212,
        -14493,     -23916,     12043,     22400,     29622,     32698,
        31197,     25330,     15917,     4277,     -7962,     -19087,
        -27540,     12540,     23170,     30274,     32767,     30274,
        23170,     12540,     0,     -12540,     -23170,     -30274,
        13033,     23916,     30853,     32698,     29148,     20788,
        8998,     -4277,     -16846,     -26635,     -32029,     13524,
        24636,     31357,     32488,     27827,     18205,     5338,
        -8481,     -20788,     -29389,     -32750,     14010,     25330,
        31786,     32138,     26320,     15447,     1608,     -12540,
        -24279,     -31357,     -32413,     14493,     25997,     32138,
        31651,     24636,     12540,     -2143,     -16384,     -27246,
        -32488,     -31029,     14972,     26635,     32413,     31029,
        22788,     9512,     -5866,     -19948,     -29622,     -32750,
        -28642,     15447,     27246,     32610,     30274,     20788,
        6393,     -9512,     -23170,     -31357,     -32138,     -25330,
        15917,     27827,     32729,     29389,     18648,     3212,
        -13033,     -25997,     -32413,     -30668,     -21199,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal60[] = {
        32588,     32052,     31164,     32052,     29935,     26510,
        31164,     26510,     19261,     29935,     21926,     10126,
        28378,     16384,     0,     26510,     10126,     -10126,
        24351,     3425,     -19261,     21926,     -3425,     -26510,
        19261,     -10126,     -31164,     16384,     -16384,     -32768,
        13328,     -21926,     -31164,     10126,     -26510,     -26510,
        6813,     -29935,     -19261,     3425,     -32052,     -10126,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag60[] = {
        3425,     6813,     10126,     6813,     13328,     19261,
        10126,     19261,     26510,     13328,     24351,     31164,
        16384,     28378,     32767,     19261,     31164,     31164,
        21926,     32588,     26510,     24351,     32588,     19261,
        26510,     31164,     10126,     28378,     28378,     0,
        29935,     24351,     -10126,     31164,     19261,     -19261,
        32052,     13328,     -26510,     32588,     6813,     -31164,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal120[] = {
        32723,     32588,     32365,     32052,     31651,     31164,
        30592,     32588,     32052,     31164,     29935,     28378,
        26510,     24351,     32365,     31164,     29197,     26510,
        23170,     19261,     14876,     32052,     29935,     26510,
        21926,     16384,     10126,     3425,     31651,     28378,
        23170,     16384,     8481,     0,     -8481,     31164,
        26510,     19261,     10126,     0,     -10126,     -19261,
        30592,     24351,     14876,     3425,     -8481,     -19261,
        -27482,     29935,     21926,     10126,     -3425,     -16384,
        -26510,     -32052,     29197,     19261,     5126,     -10126,
        -23170,     -31164,     -32365,     28378,     16384,     0,
        -16384,     -28378,     -32768,     -28378,     27482,     13328,
        -5126,     -21926,     -31651,     -31164,     -20622,     26510,
        10126,     -10126,     -26510,     -32768,     -26510,     -10126,
        25466,     6813,     -14876,     -29935,     -31651,     -19261,
        1715,     24351,     3425,     -19261,     -32052,     -28378,
        -10126,     13328,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag120[] = {
        1715,     3425,     5126,     6813,     8481,     10126,
        11743,     3425,     6813,     10126,     13328,     16384,
        19261,     21926,     5126,     10126,     14876,     19261,
        23170,     26510,     29197,     6813,     13328,     19261,
        24351,     28378,     31164,     32588,     8481,     16384,
        23170,     28378,     31651,     32767,     31651,     10126,
        19261,     26510,     31164,     32767,     31164,     26510,
        11743,     21926,     29197,     32588,     31651,     26510,
        17847,     13328,     24351,     31164,     32588,     28378,
        19261,     6813,     14876,     26510,     32365,     31164,
        23170,     10126,     -5126,     16384,     28378,     32767,
        28378,     16384,     0,     -16384,     17847,     29935,
        32365,     24351,     8481,     -10126,     -25466,     19261,
        31164,     31164,     19261,     0,     -19261,     -31164,
        20622,     32052,     29197,     13328,     -8481,     -26510,
        -32723,     21926,     32588,     26510,     6813,     -16384,
        -31164,     -29935,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal192[] = {
        32750,     32698,     32610,     32488,     32330,     32138,
        31912,     31651,     31357,     31029,     30668,     30274,
        29847,     29389,     28899,     32698,     32488,     32138,
        31651,     31029,     30274,     29389,     28378,     27246,
        25997,     24636,     23170,     21605,     19948,     18205,
        32610,     32138,     31357,     30274,     28899,     27246,
        25330,     23170,     20788,     18205,     15447,     12540,
        9512,     6393,     3212,     32488,     31651,     30274,
        28378,     25997,     23170,     19948,     16384,     12540,
        8481,     4277,     0,     -4277,     -8481,     -12540,
        32330,     31029,     28899,     25997,     22400,     18205,
        13524,     8481,     3212,     -2143,     -7441,     -12540,
        -17304,     -21605,     -25330,     32138,     30274,     27246,
        23170,     18205,     12540,     6393,     0,     -6393,
        -12540,     -18205,     -23170,     -27246,     -30274,     -32138,
        31912,     29389,     25330,     19948,     13524,     6393,
        -1072,     -8481,     -15447,     -21605,     -26635,     -30274,
        -32330,     -32698,     -31357,     31651,     28378,     23170,
        16384,     8481,     0,     -8481,     -16384,     -23170,
        -28378,     -31651,     -32768,     -31651,     -28378,     -23170,
        31357,     27246,     20788,     12540,     3212,     -6393,
        -15447,     -23170,     -28899,     -32138,     -32610,     -30274,
        -25330,     -18205,     -9512,     31029,     25997,     18205,
        8481,     -2143,     -12540,     -21605,     -28378,     -32138,
        -32488,     -29389,     -23170,     -14493,     -4277,     6393,
        30668,     24636,     15447,     4277,     -7441,     -18205,
        -26635,     -31651,     -32610,     -29389,     -22400,     -12540,
        -1072,     10533,     20788,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag192[] = {
        1072,     2143,     3212,     4277,     5338,     6393,
        7441,     8481,     9512,     10533,     11543,     12540,
        13524,     14493,     15447,     2143,     4277,     6393,
        8481,     10533,     12540,     14493,     16384,     18205,
        19948,     21605,     23170,     24636,     25997,     27246,
        3212,     6393,     9512,     12540,     15447,     18205,
        20788,     23170,     25330,     27246,     28899,     30274,
        31357,     32138,     32610,     4277,     8481,     12540,
        16384,     19948,     23170,     25997,     28378,     30274,
        31651,     32488,     32767,     32488,     31651,     30274,
        5338,     10533,     15447,     19948,     23916,     27246,
        29847,     31651,     32610,     32698,     31912,     30274,
        27827,     24636,     20788,     6393,     12540,     18205,
        23170,     27246,     30274,     32138,     32767,     32138,
        30274,     27246,     23170,     18205,     12540,     6393,
        7441,     14493,     20788,     25997,     29847,     32138,
        32750,     31651,     28899,     24636,     19087,     12540,
        5338,     -2143,     -9512,     8481,     16384,     23170,
        28378,     31651,     32767,     31651,     28378,     23170,
        16384,     8481,     0,     -8481,     -16384,     -23170,
        9512,     18205,     25330,     30274,     32610,     32138,
        28899,     23170,     15447,     6393,     -3212,     -12540,
        -20788,     -27246,     -31357,     10533,     19948,     27246,
        31651,     32698,     30274,     24636,     16384,     6393,
        -4277,     -14493,     -23170,     -29389,     -32488,     -32138,
        11543,     21605,     28899,     32488,     31912,     27246,
        19087,     8481,     -3212,     -14493,     -23916,     -30274,
        -32750,     -31029,     -25330,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal240[] = {
        32757,     32723,     32667,     32588,     32488,     32365,
        32219,     32052,     31863,     31651,     31419,     31164,
        30888,     30592,     30274,     32723,     32588,     32365,
        32052,     31651,     31164,     30592,     29935,     29197,
        28378,     27482,     26510,     25466,     24351,     23170,
        32667,     32365,     31863,     31164,     30274,     29197,
        27939,     26510,     24917,     23170,     21281,     19261,
        17121,     14876,     12540,     32588,     32052,     31164,
        29935,     28378,     26510,     24351,     21926,     19261,
        16384,     13328,     10126,     6813,     3425,     0,
        32488,     31651,     30274,     28378,     25997,     23170,
        19948,     16384,     12540,     8481,     4277,     0,
        -4277,     -8481,     -12540,     32365,     31164,     29197,
        26510,     23170,     19261,     14876,     10126,     5126,
        0,     -5126,     -10126,     -14876,     -19261,     -23170,
        32219,     30592,     27939,     24351,     19948,     14876,
        9307,     3425,     -2571,     -8481,     -14107,     -19261,
        -23769,     -27482,     -30274,     32052,     29935,     26510,
        21926,     16384,     10126,     3425,     -3425,     -10126,
        -16384,     -21926,     -26510,     -29935,     -32052,     -32768,
        31863,     29197,     24917,     19261,     12540,     5126,
        -2571,     -10126,     -17121,     -23170,     -27939,     -31164,
        -32667,     -32365,     -30274,     31651,     28378,     23170,
        16384,     8481,     0,     -8481,     -16384,     -23170,
        -28378,     -31651,     -32768,     -31651,     -28378,     -23170,
        31419,     27482,     21281,     13328,     4277,     -5126,
        -14107,     -21926,     -27939,     -31651,     -32757,     -31164,
        -27005,     -20622,     -12540,     31164,     26510,     19261,
        10126,     0,     -10126,     -19261,     -26510,     -31164,
        -32768,     -31164,     -26510,     -19261,     -10126,     0,
        30888,     25466,     17121,     6813,     -4277,     -14876,
        -23769,     -29935,     -32667,     -31651,     -27005,     -19261,
        -9307,     1715,     12540,     30592,     24351,     14876,
        3425,     -8481,     -19261,     -27482,     -32052,     -32365,
        -28378,     -20622,     -10126,     1715,     13328,     23170,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag240[] = {
        858,     1715,     2571,     3425,     4277,     5126,
        5971,     6813,     7650,     8481,     9307,     10126,
        10938,     11743,     12540,     1715,     3425,     5126,
        6813,     8481,     10126,     11743,     13328,     14876,
        16384,     17847,     19261,     20622,     21926,     23170,
        2571,     5126,     7650,     10126,     12540,     14876,
        17121,     19261,     21281,     23170,     24917,     26510,
        27939,     29197,     30274,     3425,     6813,     10126,
        13328,     16384,     19261,     21926,     24351,     26510,
        28378,     29935,     31164,     32052,     32588,     32767,
        4277,     8481,     12540,     16384,     19948,     23170,
        25997,     28378,     30274,     31651,     32488,     32767,
        32488,     31651,     30274,     5126,     10126,     14876,
        19261,     23170,     26510,     29197,     31164,     32365,
        32767,     32365,     31164,     29197,     26510,     23170,
        5971,     11743,     17121,     21926,     25997,     29197,
        31419,     32588,     32667,     31651,     29576,     26510,
        22556,     17847,     12540,     6813,     13328,     19261,
        24351,     28378,     31164,     32588,     32588,     31164,
        28378,     24351,     19261,     13328,     6813,     0,
        7650,     14876,     21281,     26510,     30274,     32365,
        32667,     31164,     27939,     23170,     17121,     10126,
        2571,     -5126,     -12540,     8481,     16384,     23170,
        28378,     31651,     32767,     31651,     28378,     23170,
        16384,     8481,     0,     -8481,     -16384,     -23170,
        9307,     17847,     24917,     29935,     32488,     32365,
        29576,     24351,     17121,     8481,     -858,     -10126,
        -18560,     -25466,     -30274,     10126,     19261,     26510,
        31164,     32767,     31164,     26510,     19261,     10126,
        0,     -10126,     -19261,     -26510,     -31164,     -32768,
        10938,     20622,     27939,     32052,     32488,     29197,
        22556,     13328,     2571,     -8481,     -18560,     -26510,
        -31419,     -32723,     -30274,     11743,     21926,     29197,
        32588,     31651,     26510,     17847,     6813,     -5126,
        -16384,     -25466,     -31164,     -32723,     -29935,     -23170,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal480[] = {
        32765,     32757,     32743,     32723,     32698,     32667,
        32631,     32588,     32541,     32488,     32429,     32365,
        32295,     32219,     32138,     32052,     31960,     31863,
        31760,     31651,     31538,     31419,     31294,     31164,
        31029,     30888,     30743,     30592,     30435,     30274,
        30107,     32757,     32723,     32667,     32588,     32488,
        32365,     32219,     32052,     31863,     31651,     31419,
        31164,     30888,     30592,     30274,     29935,     29576,
        29197,     28797,     28378,     27939,     27482,     27005,
        26510,     25997,     25466,     24917,     24351,     23769,
        23170,     22556,     32743,     32667,     32541,     32365,
        32138,     31863,     31538,     31164,     30743,     30274,
        29758,     29197,     28590,     27939,     27246,     26510,
        25733,     24917,     24062,     23170,     22243,     21281,
        20286,     19261,     18205,     17121,     16011,     14876,
        13719,     12540,     11342,     32723,     32588,     32365,
        32052,     31651,     31164,     30592,     29935,     29197,
        28378,     27482,     26510,     25466,     24351,     23170,
        21926,     20622,     19261,     17847,     16384,     14876,
        13328,     11743,     10126,     8481,     6813,     5126,
        3425,     1715,     0,     -1715,     32698,     32488,
        32138,     31651,     31029,     30274,     29389,     28378,
        27246,     25997,     24636,     23170,     21605,     19948,
        18205,     16384,     14493,     12540,     10533,     8481,
        6393,     4277,     2143,     0,     -2143,     -4277,
        -6393,     -8481,     -10533,     -12540,     -14493,     32667,
        32365,     31863,     31164,     30274,     29197,     27939,
        26510,     24917,     23170,     21281,     19261,     17121,
        14876,     12540,     10126,     7650,     5126,     2571,
        0,     -2571,     -5126,     -7650,     -10126,     -12540,
        -14876,     -17121,     -19261,     -21281,     -23170,     -24917,
        32631,     32219,     31538,     30592,     29389,     27939,
        26255,     24351,     22243,     19948,     17485,     14876,
        12142,     9307,     6393,     3425,     429,     -2571,
        -5549,     -8481,     -11342,     -14107,     -16754,     -19261,
        -21605,     -23769,     -25733,     -27482,     -28999,     -30274,
        -31294,     32588,     32052,     31164,     29935,     28378,
        26510,     24351,     21926,     19261,     16384,     13328,
        10126,     6813,     3425,     0,     -3425,     -6813,
        -10126,     -13328,     -16384,     -19261,     -21926,     -24351,
        -26510,     -28378,     -29935,     -31164,     -32052,     -32588,
        -32768,     -32588,     32541,     31863,     30743,     29197,
        27246,     24917,     22243,     19261,     16011,     12540,
        8895,     5126,     1286,     -2571,     -6393,     -10126,
        -13719,     -17121,     -20286,     -23170,     -25733,     -27939,
        -29758,     -31164,     -32138,     -32667,     -32743,     -32365,
        -31538,     -30274,     -28590,     32488,     31651,     30274,
        28378,     25997,     23170,     19948,     16384,     12540,
        8481,     4277,     0,     -4277,     -8481,     -12540,
        -16384,     -19948,     -23170,     -25997,     -28378,     -30274,
        -31651,     -32488,     -32768,     -32488,     -31651,     -30274,
        -28378,     -25997,     -23170,     -19948,     32429,     31419,
        29758,     27482,     24636,     21281,     17485,     13328,
        8895,     4277,     -429,     -5126,     -9717,     -14107,
        -18205,     -21926,     -25193,     -27939,     -30107,     -31651,
        -32541,     -32757,     -32295,     -31164,     -29389,     -27005,
        -24062,     -20622,     -16754,     -12540,     -8066,     32365,
        31164,     29197,     26510,     23170,     19261,     14876,
        10126,     5126,     0,     -5126,     -10126,     -14876,
        -19261,     -23170,     -26510,     -29197,     -31164,     -32365,
        -32768,     -32365,     -31164,     -29197,     -26510,     -23170,
        -19261,     -14876,     -10126,     -5126,     0,     5126,
        32295,     30888,     28590,     25466,     21605,     17121,
        12142,     6813,     1286,     -4277,     -9717,     -14876,
        -19606,     -23769,     -27246,     -29935,     -31760,     -32667,
        -32631,     -31651,     -29758,     -27005,     -23472,     -19261,
        -14493,     -9307,     -3851,     1715,     7232,     12540,
        17485,     32219,     30592,     27939,     24351,     19948,
        14876,     9307,     3425,     -2571,     -8481,     -14107,
        -19261,     -23769,     -27482,     -30274,     -32052,     -32757,
        -32365,     -30888,     -28378,     -24917,     -20622,     -15636,
        -10126,     -4277,     1715,     7650,     13328,     18560,
        23170,     27005,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag480[] = {
        429,     858,     1286,     1715,     2143,     2571,
        2998,     3425,     3851,     4277,     4702,     5126,
        5549,     5971,     6393,     6813,     7232,     7650,
        8066,     8481,     8895,     9307,     9717,     10126,
        10533,     10938,     11342,     11743,     12142,     12540,
        12935,     858,     1715,     2571,     3425,     4277,
        5126,     5971,     6813,     7650,     8481,     9307,
        10126,     10938,     11743,     12540,     13328,     14107,
        14876,     15636,     16384,     17121,     17847,     18560,
        19261,     19948,     20622,     21281,     21926,     22556,
        23170,     23769,     1286,     2571,     3851,     5126,
        6393,     7650,     8895,     10126,     11342,     12540,
        13719,     14876,     16011,     17121,     18205,     19261,
        20286,     21281,     22243,     23170,     24062,     24917,
        25733,     26510,     27246,     27939,     28590,     29197,
        29758,     30274,     30743,     1715,     3425,     5126,
        6813,     8481,     10126,     11743,     13328,     14876,
        16384,     17847,     19261,     20622,     21926,     23170,
        24351,     25466,     26510,     27482,     28378,     29197,
        29935,     30592,     31164,     31651,     32052,     32365,
        32588,     32723,     32767,     32723,     2143,     4277,
        6393,     8481,     10533,     12540,     14493,     16384,
        18205,     19948,     21605,     23170,     24636,     25997,
        27246,     28378,     29389,     30274,     31029,     31651,
        32138,     32488,     32698,     32767,     32698,     32488,
        32138,     31651,     31029,     30274,     29389,     2571,
        5126,     7650,     10126,     12540,     14876,     17121,
        19261,     21281,     23170,     24917,     26510,     27939,
        29197,     30274,     31164,     31863,     32365,     32667,
        32767,     32667,     32365,     31863,     31164,     30274,
        29197,     27939,     26510,     24917,     23170,     21281,
        2998,     5971,     8895,     11743,     14493,     17121,
        19606,     21926,     24062,     25997,     27713,     29197,
        30435,     31419,     32138,     32588,     32765,     32667,
        32295,     31651,     30743,     29576,     28161,     26510,
        24636,     22556,     20286,     17847,     15257,     12540,
        9717,     3425,     6813,     10126,     13328,     16384,
        19261,     21926,     24351,     26510,     28378,     29935,
        31164,     32052,     32588,     32767,     32588,     32052,
        31164,     29935,     28378,     26510,     24351,     21926,
        19261,     16384,     13328,     10126,     6813,     3425,
        0,     -3425,     3851,     7650,     11342,     14876,
        18205,     21281,     24062,     26510,     28590,     30274,
        31538,     32365,     32743,     32667,     32138,     31164,
        29758,     27939,     25733,     23170,     20286,     17121,
        13719,     10126,     6393,     2571,     -1286,     -5126,
        -8895,     -12540,     -16011,     4277,     8481,     12540,
        16384,     19948,     23170,     25997,     28378,     30274,
        31651,     32488,     32767,     32488,     31651,     30274,
        28378,     25997,     23170,     19948,     16384,     12540,
        8481,     4277,     0,     -4277,     -8481,     -12540,
        -16384,     -19948,     -23170,     -25997,     4702,     9307,
        13719,     17847,     21605,     24917,     27713,     29935,
        31538,     32488,     32765,     32365,     31294,     29576,
        27246,     24351,     20953,     17121,     12935,     8481,
        3851,     -858,     -5549,     -10126,     -14493,     -18560,
        -22243,     -25466,     -28161,     -30274,     -31760,     5126,
        10126,     14876,     19261,     23170,     26510,     29197,
        31164,     32365,     32767,     32365,     31164,     29197,
        26510,     23170,     19261,     14876,     10126,     5126,
        0,     -5126,     -10126,     -14876,     -19261,     -23170,
        -26510,     -29197,     -31164,     -32365,     -32768,     -32365,
        5549,     10938,     16011,     20622,     24636,     27939,
        30435,     32052,     32743,     32488,     31294,     29197,
        26255,     22556,     18205,     13328,     8066,     2571,
        -2998,     -8481,     -13719,     -18560,     -22865,     -26510,
        -29389,     -31419,     -32541,     -32723,     -31960,     -30274,
        -27713,     5971,     11743,     17121,     21926,     25997,
        29197,     31419,     32588,     32667,     31651,     29576,
        26510,     22556,     17847,     12540,     6813,     858,
        -5126,     -10938,     -16384,     -21281,     -25466,     -28797,
        -31164,     -32488,     -32723,     -31863,     -29935,     -27005,
        -23170,     -18560,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal20[] = {
        31164,     26510,     19261,     26510,     10126,     -10126,
        19261,     -10126,     -31164,     10126,     -26510,     -26510,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag20[] = {
        10126,     19261,     26510,     19261,     31164,     31164,
        26510,     31164,     10126,     31164,     19261,     -19261,
};
RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow8[] = {
    WTCP(0x7f62368f, 0x0c8bd35e),
    WTCP(0x7a7d055b, 0x25280c5e),
    WTCP(0x70e2cbc6, 0x3c56ba70),
    WTCP(0x62f201ac, 0x5133cc94),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow12[] = {
    WTCP(0x7fb9d759, 0x085f2137), WTCP(0x7d8a5f40, 0x18f8b83c), WTCP(0x793501a9, 0x2924edac),
    WTCP(0x72ccb9db, 0x389cea72), WTCP(0x6a6d98a4, 0x471cece7), WTCP(0x603c496c, 0x54657194),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow16[] = {
    WTCP(0x7fd8878e, 0x0647d97c), WTCP(0x7e9d55fc, 0x12c8106f), WTCP(0x7c29fbee, 0x1f19f97b),
    WTCP(0x78848414, 0x2b1f34eb), WTCP(0x73b5ebd1, 0x36ba2014), WTCP(0x6dca0d14, 0x41ce1e65),
    WTCP(0x66cf8120, 0x4c3fdff4), WTCP(0x5ed77c8a, 0x55f5a4d2),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow20[] = {
    WTCP(0x7fe6bcb0, 0x05067734), WTCP(0x7f1cde01, 0x0f0b7727), WTCP(0x7d8a5f40, 0x18f8b83c),
    WTCP(0x7b31bbb2, 0x22be8f87), WTCP(0x7816a759, 0x2c4d9050), WTCP(0x743e0918, 0x3596a46c),
    WTCP(0x6fadf2fc, 0x3e8b240e), WTCP(0x6a6d98a4, 0x471cece7), WTCP(0x648543e4, 0x4f3e7875),
    WTCP(0x5dfe47ad, 0x56e2f15d),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow24[] = {
    WTCP(0x7fee74a2, 0x0430238f), WTCP(0x7f62368f, 0x0c8bd35e), WTCP(0x7e4a5426, 0x14d9c245),
    WTCP(0x7ca80038, 0x1d10d5c2), WTCP(0x7a7d055b, 0x25280c5e), WTCP(0x77cbc3f2, 0x2d168792),
    WTCP(0x74972f92, 0x34d3957e), WTCP(0x70e2cbc6, 0x3c56ba70), WTCP(0x6cb2a837, 0x4397ba32),
    WTCP(0x680b5c33, 0x4a8ea111), WTCP(0x62f201ac, 0x5133cc94), WTCP(0x5d6c2f99, 0x577ff3da),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow32[] = {
    WTCP(0x7ff62182, 0x03242abf), WTCP(0x7fa736b4, 0x096a9049), WTCP(0x7f0991c4, 0x0fab272b),
    WTCP(0x7e1d93ea, 0x15e21445), WTCP(0x7ce3ceb2, 0x1c0b826a), WTCP(0x7b5d039e, 0x2223a4c5),
    WTCP(0x798a23b1, 0x2826b928), WTCP(0x776c4edb, 0x2e110a62), WTCP(0x7504d345, 0x33def287),
    WTCP(0x72552c85, 0x398cdd32), WTCP(0x6f5f02b2, 0x3f1749b8), WTCP(0x6c242960, 0x447acd50),
    WTCP(0x68a69e81, 0x49b41533), WTCP(0x64e88926, 0x4ebfe8a5), WTCP(0x60ec3830, 0x539b2af0),
    WTCP(0x5cb420e0, 0x5842dd54),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow40[] = {
    WTCP(0x7ff9af04, 0x02835b5a), WTCP(0x7fc72ae2, 0x07891418), WTCP(0x7f62368f, 0x0c8bd35e),
    WTCP(0x7ecaf9e5, 0x11899ed3), WTCP(0x7e01b096, 0x16807e15), WTCP(0x7d06aa16, 0x1b6e7b7a),
    WTCP(0x7bda497d, 0x2051a4dd), WTCP(0x7a7d055b, 0x25280c5e), WTCP(0x78ef678f, 0x29efc925),
    WTCP(0x77320d0d, 0x2ea6f827), WTCP(0x7545a5a0, 0x334bbcde), WTCP(0x732af3a7, 0x37dc420c),
    WTCP(0x70e2cbc6, 0x3c56ba70), WTCP(0x6e6e1492, 0x40b9617d), WTCP(0x6bcdc639, 0x45027c0c),
    WTCP(0x6902ea1d, 0x4930590f), WTCP(0x660e9a6a, 0x4d415234), WTCP(0x62f201ac, 0x5133cc94),
    WTCP(0x5fae5a55, 0x55063951), WTCP(0x5c44ee40, 0x58b71632),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow48[] = {
    WTCP(0x7ffb9d15, 0x02182427), WTCP(0x7fd8878e, 0x0647d97c), WTCP(0x7f92661d, 0x0a75d60e),
    WTCP(0x7f294bfd, 0x0ea0f48c), WTCP(0x7e9d55fc, 0x12c8106f), WTCP(0x7deeaa7a, 0x16ea0646),
    WTCP(0x7d1d7958, 0x1b05b40f), WTCP(0x7c29fbee, 0x1f19f97b), WTCP(0x7b1474fd, 0x2325b847),
    WTCP(0x79dd3098, 0x2727d486), WTCP(0x78848414, 0x2b1f34eb), WTCP(0x770acdec, 0x2f0ac320),
    WTCP(0x757075ac, 0x32e96c09), WTCP(0x73b5ebd1, 0x36ba2014), WTCP(0x71dba9ab, 0x3a7bd382),
    WTCP(0x6fe2313c, 0x3e2d7eb1), WTCP(0x6dca0d14, 0x41ce1e65), WTCP(0x6b93d02e, 0x455cb40c),
    WTCP(0x694015c3, 0x48d84609), WTCP(0x66cf8120, 0x4c3fdff4), WTCP(0x6442bd7e, 0x4f9292dc),
    WTCP(0x619a7dce, 0x52cf758f), WTCP(0x5ed77c8a, 0x55f5a4d2), WTCP(0x5bfa7b82, 0x590443a7),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow64[] = {
    WTCP(0x7ffd885a, 0x01921d20), WTCP(0x7fe9cbc0, 0x04b6195d), WTCP(0x7fc25596, 0x07d95b9e),
    WTCP(0x7f872bf3, 0x0afb6805), WTCP(0x7f3857f6, 0x0e1bc2e4), WTCP(0x7ed5e5c6, 0x1139f0cf),
    WTCP(0x7e5fe493, 0x145576b1), WTCP(0x7dd6668f, 0x176dd9de), WTCP(0x7d3980ec, 0x1a82a026),
    WTCP(0x7c894bde, 0x1d934fe5), WTCP(0x7bc5e290, 0x209f701c), WTCP(0x7aef6323, 0x23a6887f),
    WTCP(0x7a05eead, 0x26a82186), WTCP(0x7909a92d, 0x29a3c485), WTCP(0x77fab989, 0x2c98fbba),
    WTCP(0x76d94989, 0x2f875262), WTCP(0x75a585cf, 0x326e54c7), WTCP(0x745f9dd1, 0x354d9057),
    WTCP(0x7307c3d0, 0x382493b0), WTCP(0x719e2cd2, 0x3af2eeb7), WTCP(0x7023109a, 0x3db832a6),
    WTCP(0x6e96a99d, 0x4073f21d), WTCP(0x6cf934fc, 0x4325c135), WTCP(0x6b4af279, 0x45cd358f),
    WTCP(0x698c246c, 0x4869e665), WTCP(0x67bd0fbd, 0x4afb6c98), WTCP(0x65ddfbd3, 0x4d8162c4),
    WTCP(0x63ef3290, 0x4ffb654d), WTCP(0x61f1003f, 0x5269126e), WTCP(0x5fe3b38d, 0x54ca0a4b),
    WTCP(0x5dc79d7c, 0x571deefa), WTCP(0x5b9d1154, 0x59646498),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow96[] = {
    WTCP(0x7ffee744, 0x010c1460), WTCP(0x7ff62182, 0x03242abf), WTCP(0x7fe49698, 0x053c0a01),
    WTCP(0x7fca47b9, 0x07538d6b), WTCP(0x7fa736b4, 0x096a9049), WTCP(0x7f7b65ef, 0x0b80edf1),
    WTCP(0x7f46d86c, 0x0d9681c2), WTCP(0x7f0991c4, 0x0fab272b), WTCP(0x7ec3962a, 0x11beb9aa),
    WTCP(0x7e74ea6a, 0x13d114d0), WTCP(0x7e1d93ea, 0x15e21445), WTCP(0x7dbd98a4, 0x17f193c5),
    WTCP(0x7d54ff2e, 0x19ff6f2a), WTCP(0x7ce3ceb2, 0x1c0b826a), WTCP(0x7c6a0ef2, 0x1e15a99a),
    WTCP(0x7be7c847, 0x201dc0ef), WTCP(0x7b5d039e, 0x2223a4c5), WTCP(0x7ac9ca7a, 0x2427319d),
    WTCP(0x7a2e26f2, 0x26284422), WTCP(0x798a23b1, 0x2826b928), WTCP(0x78ddcbf5, 0x2a226db5),
    WTCP(0x78292b8d, 0x2c1b3efb), WTCP(0x776c4edb, 0x2e110a62), WTCP(0x76a742d1, 0x3003ad85),
    WTCP(0x75da14ef, 0x31f30638), WTCP(0x7504d345, 0x33def287), WTCP(0x74278c72, 0x35c750bc),
    WTCP(0x73424fa0, 0x37abff5d), WTCP(0x72552c85, 0x398cdd32), WTCP(0x71603361, 0x3b69c947),
    WTCP(0x706374ff, 0x3d42a2ec), WTCP(0x6f5f02b2, 0x3f1749b8), WTCP(0x6e52ee52, 0x40e79d8c),
    WTCP(0x6d3f4a40, 0x42b37e96), WTCP(0x6c242960, 0x447acd50), WTCP(0x6b019f1a, 0x463d6a87),
    WTCP(0x69d7bf57, 0x47fb3757), WTCP(0x68a69e81, 0x49b41533), WTCP(0x676e5183, 0x4b67e5e4),
    WTCP(0x662eedc3, 0x4d168b8b), WTCP(0x64e88926, 0x4ebfe8a5), WTCP(0x639b3a0b, 0x5063e008),
    WTCP(0x62471749, 0x520254ef), WTCP(0x60ec3830, 0x539b2af0), WTCP(0x5f8ab487, 0x552e4605),
    WTCP(0x5e22a487, 0x56bb8a90), WTCP(0x5cb420e0, 0x5842dd54), WTCP(0x5b3f42ae, 0x59c42381),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow120[] = {
    WTCP(0x7fff4c54, 0x00d676eb), WTCP(0x7ff9af04, 0x02835b5a), WTCP(0x7fee74a2, 0x0430238f),
    WTCP(0x7fdd9dad, 0x05dcbcbe), WTCP(0x7fc72ae2, 0x07891418), WTCP(0x7fab1d3d, 0x093516d4),
    WTCP(0x7f8975f9, 0x0ae0b22c), WTCP(0x7f62368f, 0x0c8bd35e), WTCP(0x7f3560b9, 0x0e3667ad),
    WTCP(0x7f02f66f, 0x0fe05c64), WTCP(0x7ecaf9e5, 0x11899ed3), WTCP(0x7e8d6d91, 0x13321c53),
    WTCP(0x7e4a5426, 0x14d9c245), WTCP(0x7e01b096, 0x16807e15), WTCP(0x7db3860f, 0x18263d36),
    WTCP(0x7d5fd801, 0x19caed29), WTCP(0x7d06aa16, 0x1b6e7b7a), WTCP(0x7ca80038, 0x1d10d5c2),
    WTCP(0x7c43de8e, 0x1eb1e9a7), WTCP(0x7bda497d, 0x2051a4dd), WTCP(0x7b6b45a5, 0x21eff528),
    WTCP(0x7af6d7e6, 0x238cc85d), WTCP(0x7a7d055b, 0x25280c5e), WTCP(0x79fdd35c, 0x26c1af22),
    WTCP(0x7979477d, 0x28599eb0), WTCP(0x78ef678f, 0x29efc925), WTCP(0x7860399e, 0x2b841caf),
    WTCP(0x77cbc3f2, 0x2d168792), WTCP(0x77320d0d, 0x2ea6f827), WTCP(0x76931bae, 0x30355cdd),
    WTCP(0x75eef6ce, 0x31c1a43b), WTCP(0x7545a5a0, 0x334bbcde), WTCP(0x74972f92, 0x34d3957e),
    WTCP(0x73e39c49, 0x36591cea), WTCP(0x732af3a7, 0x37dc420c), WTCP(0x726d3dc6, 0x395cf3e9),
    WTCP(0x71aa82f7, 0x3adb21a1), WTCP(0x70e2cbc6, 0x3c56ba70), WTCP(0x701620f5, 0x3dcfadb0),
    WTCP(0x6f448b7e, 0x3f45ead8), WTCP(0x6e6e1492, 0x40b9617d), WTCP(0x6d92c59b, 0x422a0154),
    WTCP(0x6cb2a837, 0x4397ba32), WTCP(0x6bcdc639, 0x45027c0c), WTCP(0x6ae429ae, 0x466a36f9),
    WTCP(0x69f5dcd3, 0x47cedb31), WTCP(0x6902ea1d, 0x4930590f), WTCP(0x680b5c33, 0x4a8ea111),
    WTCP(0x670f3df3, 0x4be9a3db), WTCP(0x660e9a6a, 0x4d415234), WTCP(0x65097cdb, 0x4e959d08),
    WTCP(0x63fff0ba, 0x4fe6756a), WTCP(0x62f201ac, 0x5133cc94), WTCP(0x61dfbb8a, 0x527d93e6),
    WTCP(0x60c92a5a, 0x53c3bcea), WTCP(0x5fae5a55, 0x55063951), WTCP(0x5e8f57e2, 0x5644faf4),
    WTCP(0x5d6c2f99, 0x577ff3da), WTCP(0x5c44ee40, 0x58b71632), WTCP(0x5b19a0c8, 0x59ea5454),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow128[] = {
    WTCP(0x7fff6216, 0x00c90f88), WTCP(0x7ffa72d1, 0x025b26d7), WTCP(0x7ff09478, 0x03ed26e6),
    WTCP(0x7fe1c76b, 0x057f0035), WTCP(0x7fce0c3e, 0x0710a345), WTCP(0x7fb563b3, 0x08a2009a),
    WTCP(0x7f97cebd, 0x0a3308bd), WTCP(0x7f754e80, 0x0bc3ac35), WTCP(0x7f4de451, 0x0d53db92),
    WTCP(0x7f2191b4, 0x0ee38766), WTCP(0x7ef05860, 0x1072a048), WTCP(0x7eba3a39, 0x120116d5),
    WTCP(0x7e7f3957, 0x138edbb1), WTCP(0x7e3f57ff, 0x151bdf86), WTCP(0x7dfa98a8, 0x16a81305),
    WTCP(0x7db0fdf8, 0x183366e9), WTCP(0x7d628ac6, 0x19bdcbf3), WTCP(0x7d0f4218, 0x1b4732ef),
    WTCP(0x7cb72724, 0x1ccf8cb3), WTCP(0x7c5a3d50, 0x1e56ca1e), WTCP(0x7bf88830, 0x1fdcdc1b),
    WTCP(0x7b920b89, 0x2161b3a0), WTCP(0x7b26cb4f, 0x22e541af), WTCP(0x7ab6cba4, 0x24677758),
    WTCP(0x7a4210d8, 0x25e845b6), WTCP(0x79c89f6e, 0x27679df4), WTCP(0x794a7c12, 0x28e5714b),
    WTCP(0x78c7aba2, 0x2a61b101), WTCP(0x78403329, 0x2bdc4e6f), WTCP(0x77b417df, 0x2d553afc),
    WTCP(0x77235f2d, 0x2ecc681e), WTCP(0x768e0ea6, 0x3041c761), WTCP(0x75f42c0b, 0x31b54a5e),
    WTCP(0x7555bd4c, 0x3326e2c3), WTCP(0x74b2c884, 0x34968250), WTCP(0x740b53fb, 0x36041ad9),
    WTCP(0x735f6626, 0x376f9e46), WTCP(0x72af05a7, 0x38d8fe93), WTCP(0x71fa3949, 0x3a402dd2),
    WTCP(0x71410805, 0x3ba51e29), WTCP(0x708378ff, 0x3d07c1d6), WTCP(0x6fc19385, 0x3e680b2c),
    WTCP(0x6efb5f12, 0x3fc5ec98), WTCP(0x6e30e34a, 0x4121589b), WTCP(0x6d6227fa, 0x427a41d0),
    WTCP(0x6c8f351c, 0x43d09aed), WTCP(0x6bb812d1, 0x452456bd), WTCP(0x6adcc964, 0x46756828),
    WTCP(0x69fd614a, 0x47c3c22f), WTCP(0x6919e320, 0x490f57ee), WTCP(0x683257ab, 0x4a581c9e),
    WTCP(0x6746c7d8, 0x4b9e0390), WTCP(0x66573cbb, 0x4ce10034), WTCP(0x6563bf92, 0x4e210617),
    WTCP(0x646c59bf, 0x4f5e08e3), WTCP(0x637114cc, 0x5097fc5e), WTCP(0x6271fa69, 0x51ced46e),
    WTCP(0x616f146c, 0x53028518), WTCP(0x60686ccf, 0x5433027d), WTCP(0x5f5e0db3, 0x556040e2),
    WTCP(0x5e50015d, 0x568a34a9), WTCP(0x5d3e5237, 0x57b0d256), WTCP(0x5c290acc, 0x58d40e8c),
    WTCP(0x5b1035cf, 0x59f3de12),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow160[] = {
    WTCP(0x7fff9aef, 0x00a0d951), WTCP(0x7ffc726f, 0x01e287fc), WTCP(0x7ff62182, 0x03242abf),
    WTCP(0x7feca851, 0x0465b9aa), WTCP(0x7fe00716, 0x05a72ccf), WTCP(0x7fd03e23, 0x06e87c3f),
    WTCP(0x7fbd4dda, 0x0829a00c), WTCP(0x7fa736b4, 0x096a9049), WTCP(0x7f8df93c, 0x0aab450d),
    WTCP(0x7f719611, 0x0bebb66c), WTCP(0x7f520de6, 0x0d2bdc80), WTCP(0x7f2f6183, 0x0e6baf61),
    WTCP(0x7f0991c4, 0x0fab272b), WTCP(0x7ee09f95, 0x10ea3bfd), WTCP(0x7eb48bfb, 0x1228e5f8),
    WTCP(0x7e85580c, 0x13671d3d), WTCP(0x7e5304f2, 0x14a4d9f4), WTCP(0x7e1d93ea, 0x15e21445),
    WTCP(0x7de50646, 0x171ec45c), WTCP(0x7da95d6c, 0x185ae269), WTCP(0x7d6a9ad5, 0x199666a0),
    WTCP(0x7d28c00c, 0x1ad14938), WTCP(0x7ce3ceb2, 0x1c0b826a), WTCP(0x7c9bc87a, 0x1d450a78),
    WTCP(0x7c50af2b, 0x1e7dd9a4), WTCP(0x7c02849f, 0x1fb5e836), WTCP(0x7bb14ac5, 0x20ed2e7b),
    WTCP(0x7b5d039e, 0x2223a4c5), WTCP(0x7b05b13d, 0x2359436c), WTCP(0x7aab55ca, 0x248e02cb),
    WTCP(0x7a4df380, 0x25c1db44), WTCP(0x79ed8cad, 0x26f4c53e), WTCP(0x798a23b1, 0x2826b928),
    WTCP(0x7923bb01, 0x2957af74), WTCP(0x78ba5524, 0x2a87a09d), WTCP(0x784df4b3, 0x2bb68522),
    WTCP(0x77de9c5b, 0x2ce45589), WTCP(0x776c4edb, 0x2e110a62), WTCP(0x76f70f05, 0x2f3c9c40),
    WTCP(0x767edfbe, 0x306703bf), WTCP(0x7603c3fd, 0x31903982), WTCP(0x7585becb, 0x32b83634),
    WTCP(0x7504d345, 0x33def287), WTCP(0x74810499, 0x35046736), WTCP(0x73fa5607, 0x36288d03),
    WTCP(0x7370cae2, 0x374b5cb9), WTCP(0x72e4668f, 0x386ccf2a), WTCP(0x72552c85, 0x398cdd32),
    WTCP(0x71c3204c, 0x3aab7fb7), WTCP(0x712e457f, 0x3bc8afa5), WTCP(0x70969fca, 0x3ce465f3),
    WTCP(0x6ffc32eb, 0x3dfe9ba1), WTCP(0x6f5f02b2, 0x3f1749b8), WTCP(0x6ebf12ff, 0x402e694c),
    WTCP(0x6e1c67c4, 0x4143f379), WTCP(0x6d770506, 0x4257e166), WTCP(0x6cceeed8, 0x436a2c45),
    WTCP(0x6c242960, 0x447acd50), WTCP(0x6b76b8d6, 0x4589bdcf), WTCP(0x6ac6a180, 0x4696f710),
    WTCP(0x6a13e7b8, 0x47a27271), WTCP(0x695e8fe5, 0x48ac2957), WTCP(0x68a69e81, 0x49b41533),
    WTCP(0x67ec1817, 0x4aba2f84), WTCP(0x672f013f, 0x4bbe71d1), WTCP(0x666f5ea6, 0x4cc0d5ae),
    WTCP(0x65ad3505, 0x4dc154bb), WTCP(0x64e88926, 0x4ebfe8a5), WTCP(0x64215fe5, 0x4fbc8b22),
    WTCP(0x6357be2a, 0x50b735f8), WTCP(0x628ba8ef, 0x51afe2f6), WTCP(0x61bd253f, 0x52a68bfb),
    WTCP(0x60ec3830, 0x539b2af0), WTCP(0x6018e6eb, 0x548db9cb), WTCP(0x5f4336a7, 0x557e3292),
    WTCP(0x5e6b2ca8, 0x566c8f55), WTCP(0x5d90ce45, 0x5758ca31), WTCP(0x5cb420e0, 0x5842dd54),
    WTCP(0x5bd529eb, 0x592ac2f7), WTCP(0x5af3eee6, 0x5a107561),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow192[] = {
    WTCP(0x7fffb9d1, 0x00860a79), WTCP(0x7ffd885a, 0x01921d20), WTCP(0x7ff92577, 0x029e28e2),
    WTCP(0x7ff2913a, 0x03aa292a), WTCP(0x7fe9cbc0, 0x04b6195d), WTCP(0x7fded530, 0x05c1f4e7),
    WTCP(0x7fd1adb9, 0x06cdb72f), WTCP(0x7fc25596, 0x07d95b9e), WTCP(0x7fb0cd0a, 0x08e4dda0),
    WTCP(0x7f9d1461, 0x09f0389f), WTCP(0x7f872bf3, 0x0afb6805), WTCP(0x7f6f141f, 0x0c066740),
    WTCP(0x7f54cd4f, 0x0d1131ba), WTCP(0x7f3857f6, 0x0e1bc2e4), WTCP(0x7f19b491, 0x0f26162a),
    WTCP(0x7ef8e3a6, 0x103026fe), WTCP(0x7ed5e5c6, 0x1139f0cf), WTCP(0x7eb0bb8a, 0x12436f10),
    WTCP(0x7e896595, 0x134c9d34), WTCP(0x7e5fe493, 0x145576b1), WTCP(0x7e34393b, 0x155df6fc),
    WTCP(0x7e06644c, 0x1666198d), WTCP(0x7dd6668f, 0x176dd9de), WTCP(0x7da440d6, 0x1875336a),
    WTCP(0x7d6ff3fe, 0x197c21ad), WTCP(0x7d3980ec, 0x1a82a026), WTCP(0x7d00e88f, 0x1b88aa55),
    WTCP(0x7cc62bdf, 0x1c8e3bbe), WTCP(0x7c894bde, 0x1d934fe5), WTCP(0x7c4a4996, 0x1e97e251),
    WTCP(0x7c09261d, 0x1f9bee8a), WTCP(0x7bc5e290, 0x209f701c), WTCP(0x7b808015, 0x21a26295),
    WTCP(0x7b38ffde, 0x22a4c185), WTCP(0x7aef6323, 0x23a6887f), WTCP(0x7aa3ab29, 0x24a7b317),
    WTCP(0x7a55d93a, 0x25a83ce6), WTCP(0x7a05eead, 0x26a82186), WTCP(0x79b3ece0, 0x27a75c95),
    WTCP(0x795fd53a, 0x28a5e9b4), WTCP(0x7909a92d, 0x29a3c485), WTCP(0x78b16a32, 0x2aa0e8b0),
    WTCP(0x785719cc, 0x2b9d51dd), WTCP(0x77fab989, 0x2c98fbba), WTCP(0x779c4afc, 0x2d93e1f8),
    WTCP(0x773bcfc4, 0x2e8e0048), WTCP(0x76d94989, 0x2f875262), WTCP(0x7674b9fa, 0x307fd401),
    WTCP(0x760e22d1, 0x317780e2), WTCP(0x75a585cf, 0x326e54c7), WTCP(0x753ae4c0, 0x33644b76),
    WTCP(0x74ce4177, 0x345960b7), WTCP(0x745f9dd1, 0x354d9057), WTCP(0x73eefbb3, 0x3640d627),
    WTCP(0x737c5d0b, 0x37332dfd), WTCP(0x7307c3d0, 0x382493b0), WTCP(0x72913201, 0x3915031f),
    WTCP(0x7218a9a7, 0x3a04782a), WTCP(0x719e2cd2, 0x3af2eeb7), WTCP(0x7121bd9c, 0x3be062b0),
    WTCP(0x70a35e25, 0x3cccd004), WTCP(0x7023109a, 0x3db832a6), WTCP(0x6fa0d72c, 0x3ea2868c),
    WTCP(0x6f1cb416, 0x3f8bc7b4), WTCP(0x6e96a99d, 0x4073f21d), WTCP(0x6e0eba0c, 0x415b01ce),
    WTCP(0x6d84e7b7, 0x4240f2d1), WTCP(0x6cf934fc, 0x4325c135), WTCP(0x6c6ba43e, 0x44096910),
    WTCP(0x6bdc37eb, 0x44ebe679), WTCP(0x6b4af279, 0x45cd358f), WTCP(0x6ab7d663, 0x46ad5278),
    WTCP(0x6a22e630, 0x478c395a), WTCP(0x698c246c, 0x4869e665), WTCP(0x68f393ae, 0x494655cc),
    WTCP(0x68593691, 0x4a2183c8), WTCP(0x67bd0fbd, 0x4afb6c98), WTCP(0x671f21dc, 0x4bd40c80),
    WTCP(0x667f6fa5, 0x4cab5fc9), WTCP(0x65ddfbd3, 0x4d8162c4), WTCP(0x653ac92b, 0x4e5611c5),
    WTCP(0x6495da79, 0x4f296928), WTCP(0x63ef3290, 0x4ffb654d), WTCP(0x6346d44b, 0x50cc029c),
    WTCP(0x629cc28c, 0x519b3d80), WTCP(0x61f1003f, 0x5269126e), WTCP(0x61439053, 0x53357ddf),
    WTCP(0x609475c3, 0x54007c51), WTCP(0x5fe3b38d, 0x54ca0a4b), WTCP(0x5f314cba, 0x55922457),
    WTCP(0x5e7d4458, 0x5658c709), WTCP(0x5dc79d7c, 0x571deefa), WTCP(0x5d105b44, 0x57e198c7),
    WTCP(0x5c5780d3, 0x58a3c118), WTCP(0x5b9d1154, 0x59646498), WTCP(0x5ae10ff9, 0x5a237ffa),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow240[] = {
    WTCP(0x7fffd315, 0x006b3b9b), WTCP(0x7ffe6bbf, 0x0141b1a5), WTCP(0x7ffb9d15, 0x02182427),
    WTCP(0x7ff76721, 0x02ee90c8), WTCP(0x7ff1c9ef, 0x03c4f52f), WTCP(0x7feac58d, 0x049b4f00),
    WTCP(0x7fe25a0f, 0x05719be2), WTCP(0x7fd8878e, 0x0647d97c), WTCP(0x7fcd4e24, 0x071e0575),
    WTCP(0x7fc0adf2, 0x07f41d72), WTCP(0x7fb2a71b, 0x08ca1f1b), WTCP(0x7fa339c5, 0x09a00817),
    WTCP(0x7f92661d, 0x0a75d60e), WTCP(0x7f802c52, 0x0b4b86a8), WTCP(0x7f6c8c96, 0x0c21178c),
    WTCP(0x7f578721, 0x0cf68662), WTCP(0x7f411c2f, 0x0dcbd0d5), WTCP(0x7f294bfd, 0x0ea0f48c),
    WTCP(0x7f1016ce, 0x0f75ef33), WTCP(0x7ef57cea, 0x104abe71), WTCP(0x7ed97e9c, 0x111f5ff4),
    WTCP(0x7ebc1c31, 0x11f3d164), WTCP(0x7e9d55fc, 0x12c8106f), WTCP(0x7e7d2c54, 0x139c1abf),
    WTCP(0x7e5b9f93, 0x146fee03), WTCP(0x7e38b017, 0x154387e6), WTCP(0x7e145e42, 0x1616e618),
    WTCP(0x7deeaa7a, 0x16ea0646), WTCP(0x7dc79529, 0x17bce621), WTCP(0x7d9f1ebd, 0x188f8357),
    WTCP(0x7d7547a7, 0x1961db9b), WTCP(0x7d4a105d, 0x1a33ec9c), WTCP(0x7d1d7958, 0x1b05b40f),
    WTCP(0x7cef8315, 0x1bd72fa4), WTCP(0x7cc02e15, 0x1ca85d12), WTCP(0x7c8f7ade, 0x1d793a0b),
    WTCP(0x7c5d69f7, 0x1e49c447), WTCP(0x7c29fbee, 0x1f19f97b), WTCP(0x7bf53153, 0x1fe9d75f),
    WTCP(0x7bbf0aba, 0x20b95bac), WTCP(0x7b8788ba, 0x2188841a), WTCP(0x7b4eabf1, 0x22574e65),
    WTCP(0x7b1474fd, 0x2325b847), WTCP(0x7ad8e482, 0x23f3bf7e), WTCP(0x7a9bfb27, 0x24c161c7),
    WTCP(0x7a5db997, 0x258e9ce0), WTCP(0x7a1e2082, 0x265b6e8a), WTCP(0x79dd3098, 0x2727d486),
    WTCP(0x799aea92, 0x27f3cc94), WTCP(0x79574f28, 0x28bf547b), WTCP(0x79125f19, 0x298a69fc),
    WTCP(0x78cc1b26, 0x2a550adf), WTCP(0x78848414, 0x2b1f34eb), WTCP(0x783b9aad, 0x2be8e5e8),
    WTCP(0x77f15fbc, 0x2cb21ba0), WTCP(0x77a5d413, 0x2d7ad3de), WTCP(0x7758f886, 0x2e430c6f),
    WTCP(0x770acdec, 0x2f0ac320), WTCP(0x76bb5521, 0x2fd1f5c1), WTCP(0x766a8f04, 0x3098a223),
    WTCP(0x76187c77, 0x315ec617), WTCP(0x75c51e61, 0x32245f72), WTCP(0x757075ac, 0x32e96c09),
    WTCP(0x751a8346, 0x33ade9b3), WTCP(0x74c34820, 0x3471d647), WTCP(0x746ac52f, 0x35352fa1),
    WTCP(0x7410fb6b, 0x35f7f39c), WTCP(0x73b5ebd1, 0x36ba2014), WTCP(0x73599760, 0x377bb2e9),
    WTCP(0x72fbff1b, 0x383ca9fb), WTCP(0x729d2409, 0x38fd032d), WTCP(0x723d0734, 0x39bcbc63),
    WTCP(0x71dba9ab, 0x3a7bd382), WTCP(0x71790c7e, 0x3b3a4672), WTCP(0x711530c2, 0x3bf8131c),
    WTCP(0x70b01790, 0x3cb5376b), WTCP(0x7049c203, 0x3d71b14d), WTCP(0x6fe2313c, 0x3e2d7eb1),
    WTCP(0x6f79665b, 0x3ee89d86), WTCP(0x6f0f6289, 0x3fa30bc1), WTCP(0x6ea426ed, 0x405cc754),
    WTCP(0x6e37b4b6, 0x4115ce38), WTCP(0x6dca0d14, 0x41ce1e65), WTCP(0x6d5b313b, 0x4285b5d4),
    WTCP(0x6ceb2261, 0x433c9283), WTCP(0x6c79e1c2, 0x43f2b271), WTCP(0x6c07709b, 0x44a8139e),
    WTCP(0x6b93d02e, 0x455cb40c), WTCP(0x6b1f01c0, 0x461091c2), WTCP(0x6aa90697, 0x46c3aac5),
    WTCP(0x6a31e000, 0x4775fd1f), WTCP(0x69b98f48, 0x482786dc), WTCP(0x694015c3, 0x48d84609),
    WTCP(0x68c574c4, 0x498838b6), WTCP(0x6849ada3, 0x4a375cf5), WTCP(0x67ccc1be, 0x4ae5b0da),
    WTCP(0x674eb271, 0x4b93327c), WTCP(0x66cf8120, 0x4c3fdff4), WTCP(0x664f2f2e, 0x4cebb75c),
    WTCP(0x65cdbe05, 0x4d96b6d3), WTCP(0x654b2f10, 0x4e40dc79), WTCP(0x64c783bd, 0x4eea2670),
    WTCP(0x6442bd7e, 0x4f9292dc), WTCP(0x63bcddc7, 0x503a1fe5), WTCP(0x6335e611, 0x50e0cbb4),
    WTCP(0x62add7d6, 0x51869476), WTCP(0x6224b495, 0x522b7859), WTCP(0x619a7dce, 0x52cf758f),
    WTCP(0x610f3505, 0x53728a4a), WTCP(0x6082dbc1, 0x5414b4c1), WTCP(0x5ff5738d, 0x54b5f32c),
    WTCP(0x5f66fdf5, 0x555643c8), WTCP(0x5ed77c8a, 0x55f5a4d2), WTCP(0x5e46f0dd, 0x5694148b),
    WTCP(0x5db55c86, 0x57319135), WTCP(0x5d22c11c, 0x57ce1917), WTCP(0x5c8f203b, 0x5869aa79),
    WTCP(0x5bfa7b82, 0x590443a7), WTCP(0x5b64d492, 0x599de2ee), WTCP(0x5ace2d0f, 0x5a36869f),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow256[] = {
    WTCP(0x7fffd886, 0x006487e3), WTCP(0x7ffe9cb2, 0x012d96b1), WTCP(0x7ffc250f, 0x01f6a297),
    WTCP(0x7ff871a2, 0x02bfa9a4), WTCP(0x7ff38274, 0x0388a9ea), WTCP(0x7fed5791, 0x0451a177),
    WTCP(0x7fe5f108, 0x051a8e5c), WTCP(0x7fdd4eec, 0x05e36ea9), WTCP(0x7fd37153, 0x06ac406f),
    WTCP(0x7fc85854, 0x077501be), WTCP(0x7fbc040a, 0x083db0a7), WTCP(0x7fae7495, 0x09064b3a),
    WTCP(0x7f9faa15, 0x09cecf89), WTCP(0x7f8fa4b0, 0x0a973ba5), WTCP(0x7f7e648c, 0x0b5f8d9f),
    WTCP(0x7f6be9d4, 0x0c27c389), WTCP(0x7f5834b7, 0x0cefdb76), WTCP(0x7f434563, 0x0db7d376),
    WTCP(0x7f2d1c0e, 0x0e7fa99e), WTCP(0x7f15b8ee, 0x0f475bff), WTCP(0x7efd1c3c, 0x100ee8ad),
    WTCP(0x7ee34636, 0x10d64dbd), WTCP(0x7ec8371a, 0x119d8941), WTCP(0x7eabef2c, 0x1264994e),
    WTCP(0x7e8e6eb2, 0x132b7bf9), WTCP(0x7e6fb5f4, 0x13f22f58), WTCP(0x7e4fc53e, 0x14b8b17f),
    WTCP(0x7e2e9cdf, 0x157f0086), WTCP(0x7e0c3d29, 0x16451a83), WTCP(0x7de8a670, 0x170afd8d),
    WTCP(0x7dc3d90d, 0x17d0a7bc), WTCP(0x7d9dd55a, 0x18961728), WTCP(0x7d769bb5, 0x195b49ea),
    WTCP(0x7d4e2c7f, 0x1a203e1b), WTCP(0x7d24881b, 0x1ae4f1d6), WTCP(0x7cf9aef0, 0x1ba96335),
    WTCP(0x7ccda169, 0x1c6d9053), WTCP(0x7ca05ff1, 0x1d31774d), WTCP(0x7c71eaf9, 0x1df5163f),
    WTCP(0x7c4242f2, 0x1eb86b46), WTCP(0x7c116853, 0x1f7b7481), WTCP(0x7bdf5b94, 0x203e300d),
    WTCP(0x7bac1d31, 0x21009c0c), WTCP(0x7b77ada8, 0x21c2b69c), WTCP(0x7b420d7a, 0x22847de0),
    WTCP(0x7b0b3d2c, 0x2345eff8), WTCP(0x7ad33d45, 0x24070b08), WTCP(0x7a9a0e50, 0x24c7cd33),
    WTCP(0x7a5fb0d8, 0x2588349d), WTCP(0x7a24256f, 0x26483f6c), WTCP(0x79e76ca7, 0x2707ebc7),
    WTCP(0x79a98715, 0x27c737d3), WTCP(0x796a7554, 0x288621b9), WTCP(0x792a37fe, 0x2944a7a2),
    WTCP(0x78e8cfb2, 0x2a02c7b8), WTCP(0x78a63d11, 0x2ac08026), WTCP(0x786280bf, 0x2b7dcf17),
    WTCP(0x781d9b65, 0x2c3ab2b9), WTCP(0x77d78daa, 0x2cf72939), WTCP(0x7790583e, 0x2db330c7),
    WTCP(0x7747fbce, 0x2e6ec792), WTCP(0x76fe790e, 0x2f29ebcc), WTCP(0x76b3d0b4, 0x2fe49ba7),
    WTCP(0x76680376, 0x309ed556), WTCP(0x761b1211, 0x3158970e), WTCP(0x75ccfd42, 0x3211df04),
    WTCP(0x757dc5ca, 0x32caab6f), WTCP(0x752d6c6c, 0x3382fa88), WTCP(0x74dbf1ef, 0x343aca87),
    WTCP(0x7489571c, 0x34f219a8), WTCP(0x74359cbd, 0x35a8e625), WTCP(0x73e0c3a3, 0x365f2e3b),
    WTCP(0x738acc9e, 0x3714f02a), WTCP(0x7333b883, 0x37ca2a30), WTCP(0x72db8828, 0x387eda8e),
    WTCP(0x72823c67, 0x3932ff87), WTCP(0x7227d61c, 0x39e6975e), WTCP(0x71cc5626, 0x3a99a057),
    WTCP(0x716fbd68, 0x3b4c18ba), WTCP(0x71120cc5, 0x3bfdfecd), WTCP(0x70b34525, 0x3caf50da),
    WTCP(0x70536771, 0x3d600d2c), WTCP(0x6ff27497, 0x3e10320d), WTCP(0x6f906d84, 0x3ebfbdcd),
    WTCP(0x6f2d532c, 0x3f6eaeb8), WTCP(0x6ec92683, 0x401d0321), WTCP(0x6e63e87f, 0x40cab958),
    WTCP(0x6dfd9a1c, 0x4177cfb1), WTCP(0x6d963c54, 0x42244481), WTCP(0x6d2dd027, 0x42d0161e),
    WTCP(0x6cc45698, 0x437b42e1), WTCP(0x6c59d0a9, 0x4425c923), WTCP(0x6bee3f62, 0x44cfa740),
    WTCP(0x6b81a3cd, 0x4578db93), WTCP(0x6b13fef5, 0x4621647d), WTCP(0x6aa551e9, 0x46c9405c),
    WTCP(0x6a359db9, 0x47706d93), WTCP(0x69c4e37a, 0x4816ea86), WTCP(0x69532442, 0x48bcb599),
    WTCP(0x68e06129, 0x4961cd33), WTCP(0x686c9b4b, 0x4a062fbd), WTCP(0x67f7d3c5, 0x4aa9dba2),
    WTCP(0x67820bb7, 0x4b4ccf4d), WTCP(0x670b4444, 0x4bef092d), WTCP(0x66937e91, 0x4c9087b1),
    WTCP(0x661abbc5, 0x4d31494b), WTCP(0x65a0fd0b, 0x4dd14c6e), WTCP(0x6526438f, 0x4e708f8f),
    WTCP(0x64aa907f, 0x4f0f1126), WTCP(0x642de50d, 0x4faccfab), WTCP(0x63b0426d, 0x5049c999),
    WTCP(0x6331a9d4, 0x50e5fd6d), WTCP(0x62b21c7b, 0x518169a5), WTCP(0x62319b9d, 0x521c0cc2),
    WTCP(0x61b02876, 0x52b5e546), WTCP(0x612dc447, 0x534ef1b5), WTCP(0x60aa7050, 0x53e73097),
    WTCP(0x60262dd6, 0x547ea073), WTCP(0x5fa0fe1f, 0x55153fd4), WTCP(0x5f1ae274, 0x55ab0d46),
    WTCP(0x5e93dc1f, 0x56400758), WTCP(0x5e0bec6e, 0x56d42c99), WTCP(0x5d8314b1, 0x57677b9d),
    WTCP(0x5cf95638, 0x57f9f2f8), WTCP(0x5c6eb258, 0x588b9140), WTCP(0x5be32a67, 0x591c550e),
    WTCP(0x5b56bfbd, 0x59ac3cfd), WTCP(0x5ac973b5, 0x5a3b47ab),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow384[] = {
    WTCP(0x7fffee74, 0x00430546), WTCP(0x7fff6216, 0x00c90f88), WTCP(0x7ffe495b, 0x014f18ee),
    WTCP(0x7ffca443, 0x01d520e4), WTCP(0x7ffa72d1, 0x025b26d7), WTCP(0x7ff7b507, 0x02e12a36),
    WTCP(0x7ff46ae8, 0x03672a6c), WTCP(0x7ff09478, 0x03ed26e6), WTCP(0x7fec31ba, 0x04731f13),
    WTCP(0x7fe742b4, 0x04f9125e), WTCP(0x7fe1c76b, 0x057f0035), WTCP(0x7fdbbfe6, 0x0604e805),
    WTCP(0x7fd52c29, 0x068ac93b), WTCP(0x7fce0c3e, 0x0710a345), WTCP(0x7fc6602c, 0x0796758f),
    WTCP(0x7fbe27fa, 0x081c3f87), WTCP(0x7fb563b3, 0x08a2009a), WTCP(0x7fac135f, 0x0927b836),
    WTCP(0x7fa2370a, 0x09ad65c8), WTCP(0x7f97cebd, 0x0a3308bd), WTCP(0x7f8cda84, 0x0ab8a082),
    WTCP(0x7f815a6b, 0x0b3e2c86), WTCP(0x7f754e80, 0x0bc3ac35), WTCP(0x7f68b6ce, 0x0c491efe),
    WTCP(0x7f5b9364, 0x0cce844e), WTCP(0x7f4de451, 0x0d53db92), WTCP(0x7f3fa9a2, 0x0dd92439),
    WTCP(0x7f30e369, 0x0e5e5db0), WTCP(0x7f2191b4, 0x0ee38766), WTCP(0x7f11b495, 0x0f68a0c8),
    WTCP(0x7f014c1e, 0x0feda943), WTCP(0x7ef05860, 0x1072a048), WTCP(0x7eded96d, 0x10f78543),
    WTCP(0x7ecccf5a, 0x117c57a2), WTCP(0x7eba3a39, 0x120116d5), WTCP(0x7ea71a20, 0x1285c249),
    WTCP(0x7e936f22, 0x130a596e), WTCP(0x7e7f3957, 0x138edbb1), WTCP(0x7e6a78d3, 0x14134881),
    WTCP(0x7e552dae, 0x14979f4e), WTCP(0x7e3f57ff, 0x151bdf86), WTCP(0x7e28f7de, 0x15a00897),
    WTCP(0x7e120d63, 0x162419f2), WTCP(0x7dfa98a8, 0x16a81305), WTCP(0x7de299c6, 0x172bf33f),
    WTCP(0x7dca10d8, 0x17afba11), WTCP(0x7db0fdf8, 0x183366e9), WTCP(0x7d976142, 0x18b6f936),
    WTCP(0x7d7d3ad3, 0x193a706a), WTCP(0x7d628ac6, 0x19bdcbf3), WTCP(0x7d475139, 0x1a410b41),
    WTCP(0x7d2b8e4a, 0x1ac42dc5), WTCP(0x7d0f4218, 0x1b4732ef), WTCP(0x7cf26cc1, 0x1bca1a2f),
    WTCP(0x7cd50e65, 0x1c4ce2f6), WTCP(0x7cb72724, 0x1ccf8cb3), WTCP(0x7c98b71f, 0x1d5216d8),
    WTCP(0x7c79be78, 0x1dd480d6), WTCP(0x7c5a3d50, 0x1e56ca1e), WTCP(0x7c3a33ca, 0x1ed8f220),
    WTCP(0x7c19a209, 0x1f5af84f), WTCP(0x7bf88830, 0x1fdcdc1b), WTCP(0x7bd6e665, 0x205e9cf6),
    WTCP(0x7bb4bccb, 0x20e03a51), WTCP(0x7b920b89, 0x2161b3a0), WTCP(0x7b6ed2c5, 0x21e30853),
    WTCP(0x7b4b12a4, 0x226437dc), WTCP(0x7b26cb4f, 0x22e541af), WTCP(0x7b01fced, 0x2366253d),
    WTCP(0x7adca7a6, 0x23e6e1fa), WTCP(0x7ab6cba4, 0x24677758), WTCP(0x7a90690f, 0x24e7e4c9),
    WTCP(0x7a698012, 0x256829c2), WTCP(0x7a4210d8, 0x25e845b6), WTCP(0x7a1a1b8c, 0x26683818),
    WTCP(0x79f1a05a, 0x26e8005b), WTCP(0x79c89f6e, 0x27679df4), WTCP(0x799f18f4, 0x27e71057),
    WTCP(0x79750d1c, 0x286656f8), WTCP(0x794a7c12, 0x28e5714b), WTCP(0x791f6605, 0x29645ec5),
    WTCP(0x78f3cb25, 0x29e31edb), WTCP(0x78c7aba2, 0x2a61b101), WTCP(0x789b07ab, 0x2ae014ae),
    WTCP(0x786ddf72, 0x2b5e4956), WTCP(0x78403329, 0x2bdc4e6f), WTCP(0x78120300, 0x2c5a236f),
    WTCP(0x77e34f2c, 0x2cd7c7cc), WTCP(0x77b417df, 0x2d553afc), WTCP(0x77845d4e, 0x2dd27c75),
    WTCP(0x77541fab, 0x2e4f8bae), WTCP(0x77235f2d, 0x2ecc681e), WTCP(0x76f21c09, 0x2f49113d),
    WTCP(0x76c05674, 0x2fc58680), WTCP(0x768e0ea6, 0x3041c761), WTCP(0x765b44d5, 0x30bdd356),
    WTCP(0x7627f939, 0x3139a9d7), WTCP(0x75f42c0b, 0x31b54a5e), WTCP(0x75bfdd83, 0x3230b461),
    WTCP(0x758b0ddb, 0x32abe75a), WTCP(0x7555bd4c, 0x3326e2c3), WTCP(0x751fec11, 0x33a1a612),
    WTCP(0x74e99a65, 0x341c30c4), WTCP(0x74b2c884, 0x34968250), WTCP(0x747b76a9, 0x35109a31),
    WTCP(0x7443a512, 0x358a77e0), WTCP(0x740b53fb, 0x36041ad9), WTCP(0x73d283a2, 0x367d8296),
    WTCP(0x73993447, 0x36f6ae91), WTCP(0x735f6626, 0x376f9e46), WTCP(0x73251981, 0x37e85130),
    WTCP(0x72ea4e96, 0x3860c6cb), WTCP(0x72af05a7, 0x38d8fe93), WTCP(0x72733ef3, 0x3950f804),
    WTCP(0x7236fabe, 0x39c8b29a), WTCP(0x71fa3949, 0x3a402dd2), WTCP(0x71bcfad6, 0x3ab76929),
    WTCP(0x717f3fa8, 0x3b2e641c), WTCP(0x71410805, 0x3ba51e29), WTCP(0x7102542f, 0x3c1b96ce),
    WTCP(0x70c3246b, 0x3c91cd88), WTCP(0x708378ff, 0x3d07c1d6), WTCP(0x70435230, 0x3d7d7337),
    WTCP(0x7002b045, 0x3df2e129), WTCP(0x6fc19385, 0x3e680b2c), WTCP(0x6f7ffc37, 0x3edcf0c0),
    WTCP(0x6f3deaa4, 0x3f519164), WTCP(0x6efb5f12, 0x3fc5ec98), WTCP(0x6eb859cc, 0x403a01dc),
    WTCP(0x6e74db1c, 0x40add0b2), WTCP(0x6e30e34a, 0x4121589b), WTCP(0x6dec72a2, 0x41949917),
    WTCP(0x6da7896e, 0x420791a8), WTCP(0x6d6227fa, 0x427a41d0), WTCP(0x6d1c4e93, 0x42eca912),
    WTCP(0x6cd5fd85, 0x435ec6f0), WTCP(0x6c8f351c, 0x43d09aed), WTCP(0x6c47f5a7, 0x4442248b),
    WTCP(0x6c003f74, 0x44b3634f), WTCP(0x6bb812d1, 0x452456bd), WTCP(0x6b6f700e, 0x4594fe58),
    WTCP(0x6b265779, 0x460559a4), WTCP(0x6adcc964, 0x46756828), WTCP(0x6a92c61f, 0x46e52967),
    WTCP(0x6a484dfc, 0x47549ce7), WTCP(0x69fd614a, 0x47c3c22f), WTCP(0x69b2005e, 0x483298c4),
    WTCP(0x69662b8a, 0x48a1202c), WTCP(0x6919e320, 0x490f57ee), WTCP(0x68cd2775, 0x497d3f93),
    WTCP(0x687ff8dc, 0x49ead6a0), WTCP(0x683257ab, 0x4a581c9e), WTCP(0x67e44436, 0x4ac51114),
    WTCP(0x6795bed3, 0x4b31b38d), WTCP(0x6746c7d8, 0x4b9e0390), WTCP(0x66f75f9b, 0x4c0a00a6),
    WTCP(0x66a78675, 0x4c75aa5a), WTCP(0x66573cbb, 0x4ce10034), WTCP(0x660682c7, 0x4d4c01c0),
    WTCP(0x65b558f1, 0x4db6ae88), WTCP(0x6563bf92, 0x4e210617), WTCP(0x6511b703, 0x4e8b07f9),
    WTCP(0x64bf3f9f, 0x4ef4b3b9), WTCP(0x646c59bf, 0x4f5e08e3), WTCP(0x641905bf, 0x4fc70704),
    WTCP(0x63c543fa, 0x502fada9), WTCP(0x637114cc, 0x5097fc5e), WTCP(0x631c7892, 0x50fff2b2),
    WTCP(0x62c76fa7, 0x51679033), WTCP(0x6271fa69, 0x51ced46e), WTCP(0x621c1937, 0x5235bef4),
    WTCP(0x61c5cc6d, 0x529c4f51), WTCP(0x616f146c, 0x53028518), WTCP(0x6117f191, 0x53685fd6),
    WTCP(0x60c0643d, 0x53cddf1d), WTCP(0x60686ccf, 0x5433027d), WTCP(0x60100ba8, 0x5497c988),
    WTCP(0x5fb74129, 0x54fc33ce), WTCP(0x5f5e0db3, 0x556040e2), WTCP(0x5f0471a8, 0x55c3f056),
    WTCP(0x5eaa6d6b, 0x562741bd), WTCP(0x5e50015d, 0x568a34a9), WTCP(0x5df52de3, 0x56ecc8af),
    WTCP(0x5d99f35f, 0x574efd62), WTCP(0x5d3e5237, 0x57b0d256), WTCP(0x5ce24acd, 0x58124720),
    WTCP(0x5c85dd88, 0x58735b56), WTCP(0x5c290acc, 0x58d40e8c), WTCP(0x5bcbd300, 0x5934605a),
    WTCP(0x5b6e3689, 0x59945054), WTCP(0x5b1035cf, 0x59f3de12), WTCP(0x5ab1d138, 0x5a53092c),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow480[] = {
    WTCP(0x7ffff4c5, 0x00359dd2), WTCP(0x7fff9aef, 0x00a0d951), WTCP(0x7ffee744, 0x010c1460),
    WTCP(0x7ffdd9c4, 0x01774eb2), WTCP(0x7ffc726f, 0x01e287fc), WTCP(0x7ffab147, 0x024dbff4),
    WTCP(0x7ff8964d, 0x02b8f64e), WTCP(0x7ff62182, 0x03242abf), WTCP(0x7ff352e8, 0x038f5cfb),
    WTCP(0x7ff02a82, 0x03fa8cb8), WTCP(0x7feca851, 0x0465b9aa), WTCP(0x7fe8cc57, 0x04d0e386),
    WTCP(0x7fe49698, 0x053c0a01), WTCP(0x7fe00716, 0x05a72ccf), WTCP(0x7fdb1dd5, 0x06124ba5),
    WTCP(0x7fd5dad8, 0x067d6639), WTCP(0x7fd03e23, 0x06e87c3f), WTCP(0x7fca47b9, 0x07538d6b),
    WTCP(0x7fc3f7a0, 0x07be9973), WTCP(0x7fbd4dda, 0x0829a00c), WTCP(0x7fb64a6e, 0x0894a0ea),
    WTCP(0x7faeed5f, 0x08ff9bc2), WTCP(0x7fa736b4, 0x096a9049), WTCP(0x7f9f2671, 0x09d57e35),
    WTCP(0x7f96bc9c, 0x0a40653a), WTCP(0x7f8df93c, 0x0aab450d), WTCP(0x7f84dc55, 0x0b161d63),
    WTCP(0x7f7b65ef, 0x0b80edf1), WTCP(0x7f719611, 0x0bebb66c), WTCP(0x7f676cc0, 0x0c56768a),
    WTCP(0x7f5cea05, 0x0cc12dff), WTCP(0x7f520de6, 0x0d2bdc80), WTCP(0x7f46d86c, 0x0d9681c2),
    WTCP(0x7f3b499d, 0x0e011d7c), WTCP(0x7f2f6183, 0x0e6baf61), WTCP(0x7f232026, 0x0ed63727),
    WTCP(0x7f16858e, 0x0f40b483), WTCP(0x7f0991c4, 0x0fab272b), WTCP(0x7efc44d0, 0x10158ed4),
    WTCP(0x7eee9ebe, 0x107feb33), WTCP(0x7ee09f95, 0x10ea3bfd), WTCP(0x7ed24761, 0x115480e9),
    WTCP(0x7ec3962a, 0x11beb9aa), WTCP(0x7eb48bfb, 0x1228e5f8), WTCP(0x7ea528e0, 0x12930586),
    WTCP(0x7e956ce1, 0x12fd180b), WTCP(0x7e85580c, 0x13671d3d), WTCP(0x7e74ea6a, 0x13d114d0),
    WTCP(0x7e642408, 0x143afe7b), WTCP(0x7e5304f2, 0x14a4d9f4), WTCP(0x7e418d32, 0x150ea6ef),
    WTCP(0x7e2fbcd6, 0x15786522), WTCP(0x7e1d93ea, 0x15e21445), WTCP(0x7e0b127a, 0x164bb40b),
    WTCP(0x7df83895, 0x16b5442b), WTCP(0x7de50646, 0x171ec45c), WTCP(0x7dd17b9c, 0x17883452),
    WTCP(0x7dbd98a4, 0x17f193c5), WTCP(0x7da95d6c, 0x185ae269), WTCP(0x7d94ca03, 0x18c41ff6),
    WTCP(0x7d7fde76, 0x192d4c21), WTCP(0x7d6a9ad5, 0x199666a0), WTCP(0x7d54ff2e, 0x19ff6f2a),
    WTCP(0x7d3f0b90, 0x1a686575), WTCP(0x7d28c00c, 0x1ad14938), WTCP(0x7d121cb0, 0x1b3a1a28),
    WTCP(0x7cfb218c, 0x1ba2d7fc), WTCP(0x7ce3ceb2, 0x1c0b826a), WTCP(0x7ccc2430, 0x1c74192a),
    WTCP(0x7cb42217, 0x1cdc9bf2), WTCP(0x7c9bc87a, 0x1d450a78), WTCP(0x7c831767, 0x1dad6473),
    WTCP(0x7c6a0ef2, 0x1e15a99a), WTCP(0x7c50af2b, 0x1e7dd9a4), WTCP(0x7c36f824, 0x1ee5f447),
    WTCP(0x7c1ce9ef, 0x1f4df93a), WTCP(0x7c02849f, 0x1fb5e836), WTCP(0x7be7c847, 0x201dc0ef),
    WTCP(0x7bccb4f8, 0x2085831f), WTCP(0x7bb14ac5, 0x20ed2e7b), WTCP(0x7b9589c3, 0x2154c2bb),
    WTCP(0x7b797205, 0x21bc3f97), WTCP(0x7b5d039e, 0x2223a4c5), WTCP(0x7b403ea2, 0x228af1fe),
    WTCP(0x7b232325, 0x22f226f8), WTCP(0x7b05b13d, 0x2359436c), WTCP(0x7ae7e8fc, 0x23c04710),
    WTCP(0x7ac9ca7a, 0x2427319d), WTCP(0x7aab55ca, 0x248e02cb), WTCP(0x7a8c8b01, 0x24f4ba50),
    WTCP(0x7a6d6a37, 0x255b57e6), WTCP(0x7a4df380, 0x25c1db44), WTCP(0x7a2e26f2, 0x26284422),
    WTCP(0x7a0e04a4, 0x268e9238), WTCP(0x79ed8cad, 0x26f4c53e), WTCP(0x79ccbf22, 0x275adcee),
    WTCP(0x79ab9c1c, 0x27c0d8fe), WTCP(0x798a23b1, 0x2826b928), WTCP(0x796855f9, 0x288c7d24),
    WTCP(0x7946330c, 0x28f224ab), WTCP(0x7923bb01, 0x2957af74), WTCP(0x7900edf2, 0x29bd1d3a),
    WTCP(0x78ddcbf5, 0x2a226db5), WTCP(0x78ba5524, 0x2a87a09d), WTCP(0x78968998, 0x2aecb5ac),
    WTCP(0x7872696a, 0x2b51ac9a), WTCP(0x784df4b3, 0x2bb68522), WTCP(0x78292b8d, 0x2c1b3efb),
    WTCP(0x78040e12, 0x2c7fd9e0), WTCP(0x77de9c5b, 0x2ce45589), WTCP(0x77b8d683, 0x2d48b1b1),
    WTCP(0x7792bca5, 0x2dacee11), WTCP(0x776c4edb, 0x2e110a62), WTCP(0x77458d40, 0x2e75065e),
    WTCP(0x771e77f0, 0x2ed8e1c0), WTCP(0x76f70f05, 0x2f3c9c40), WTCP(0x76cf529c, 0x2fa03599),
    WTCP(0x76a742d1, 0x3003ad85), WTCP(0x767edfbe, 0x306703bf), WTCP(0x76562982, 0x30ca3800),
    WTCP(0x762d2038, 0x312d4a03), WTCP(0x7603c3fd, 0x31903982), WTCP(0x75da14ef, 0x31f30638),
    WTCP(0x75b01329, 0x3255afe0), WTCP(0x7585becb, 0x32b83634), WTCP(0x755b17f2, 0x331a98ef),
    WTCP(0x75301ebb, 0x337cd7cd), WTCP(0x7504d345, 0x33def287), WTCP(0x74d935ae, 0x3440e8da),
    WTCP(0x74ad4615, 0x34a2ba81), WTCP(0x74810499, 0x35046736), WTCP(0x74547158, 0x3565eeb6),
    WTCP(0x74278c72, 0x35c750bc), WTCP(0x73fa5607, 0x36288d03), WTCP(0x73ccce36, 0x3689a348),
    WTCP(0x739ef51f, 0x36ea9346), WTCP(0x7370cae2, 0x374b5cb9), WTCP(0x73424fa0, 0x37abff5d),
    WTCP(0x73138379, 0x380c7aee), WTCP(0x72e4668f, 0x386ccf2a), WTCP(0x72b4f902, 0x38ccfbcb),
    WTCP(0x72853af3, 0x392d008f), WTCP(0x72552c85, 0x398cdd32), WTCP(0x7224cdd8, 0x39ec9172),
    WTCP(0x71f41f0f, 0x3a4c1d09), WTCP(0x71c3204c, 0x3aab7fb7), WTCP(0x7191d1b1, 0x3b0ab937),
    WTCP(0x71603361, 0x3b69c947), WTCP(0x712e457f, 0x3bc8afa5), WTCP(0x70fc082d, 0x3c276c0d),
    WTCP(0x70c97b90, 0x3c85fe3d), WTCP(0x70969fca, 0x3ce465f3), WTCP(0x706374ff, 0x3d42a2ec),
    WTCP(0x702ffb54, 0x3da0b4e7), WTCP(0x6ffc32eb, 0x3dfe9ba1), WTCP(0x6fc81bea, 0x3e5c56d8),
    WTCP(0x6f93b676, 0x3eb9e64b), WTCP(0x6f5f02b2, 0x3f1749b8), WTCP(0x6f2a00c4, 0x3f7480dd),
    WTCP(0x6ef4b0d1, 0x3fd18b7a), WTCP(0x6ebf12ff, 0x402e694c), WTCP(0x6e892772, 0x408b1a12),
    WTCP(0x6e52ee52, 0x40e79d8c), WTCP(0x6e1c67c4, 0x4143f379), WTCP(0x6de593ee, 0x41a01b97),
    WTCP(0x6dae72f7, 0x41fc15a6), WTCP(0x6d770506, 0x4257e166), WTCP(0x6d3f4a40, 0x42b37e96),
    WTCP(0x6d0742cf, 0x430eecf6), WTCP(0x6cceeed8, 0x436a2c45), WTCP(0x6c964e83, 0x43c53c44),
    WTCP(0x6c5d61f9, 0x44201cb2), WTCP(0x6c242960, 0x447acd50), WTCP(0x6beaa4e2, 0x44d54ddf),
    WTCP(0x6bb0d4a7, 0x452f9e1e), WTCP(0x6b76b8d6, 0x4589bdcf), WTCP(0x6b3c519a, 0x45e3acb1),
    WTCP(0x6b019f1a, 0x463d6a87), WTCP(0x6ac6a180, 0x4696f710), WTCP(0x6a8b58f6, 0x46f0520f),
    WTCP(0x6a4fc5a6, 0x47497b44), WTCP(0x6a13e7b8, 0x47a27271), WTCP(0x69d7bf57, 0x47fb3757),
    WTCP(0x699b4cad, 0x4853c9b9), WTCP(0x695e8fe5, 0x48ac2957), WTCP(0x69218929, 0x490455f4),
    WTCP(0x68e438a4, 0x495c4f52), WTCP(0x68a69e81, 0x49b41533), WTCP(0x6868baec, 0x4a0ba75b),
    WTCP(0x682a8e0f, 0x4a63058a), WTCP(0x67ec1817, 0x4aba2f84), WTCP(0x67ad592f, 0x4b11250c),
    WTCP(0x676e5183, 0x4b67e5e4), WTCP(0x672f013f, 0x4bbe71d1), WTCP(0x66ef6891, 0x4c14c894),
    WTCP(0x66af87a4, 0x4c6ae9f2), WTCP(0x666f5ea6, 0x4cc0d5ae), WTCP(0x662eedc3, 0x4d168b8b),
    WTCP(0x65ee3529, 0x4d6c0b4e), WTCP(0x65ad3505, 0x4dc154bb), WTCP(0x656bed84, 0x4e166795),
    WTCP(0x652a5ed6, 0x4e6b43a2), WTCP(0x64e88926, 0x4ebfe8a5), WTCP(0x64a66ca5, 0x4f145662),
    WTCP(0x6464097f, 0x4f688ca0), WTCP(0x64215fe5, 0x4fbc8b22), WTCP(0x63de7003, 0x501051ae),
    WTCP(0x639b3a0b, 0x5063e008), WTCP(0x6357be2a, 0x50b735f8), WTCP(0x6313fc90, 0x510a5340),
    WTCP(0x62cff56c, 0x515d37a9), WTCP(0x628ba8ef, 0x51afe2f6), WTCP(0x62471749, 0x520254ef),
    WTCP(0x620240a8, 0x52548d59), WTCP(0x61bd253f, 0x52a68bfb), WTCP(0x6177c53c, 0x52f8509b),
    WTCP(0x613220d2, 0x5349daff), WTCP(0x60ec3830, 0x539b2af0), WTCP(0x60a60b88, 0x53ec4032),
    WTCP(0x605f9b0b, 0x543d1a8e), WTCP(0x6018e6eb, 0x548db9cb), WTCP(0x5fd1ef59, 0x54de1db1),
    WTCP(0x5f8ab487, 0x552e4605), WTCP(0x5f4336a7, 0x557e3292), WTCP(0x5efb75ea, 0x55cde31e),
    WTCP(0x5eb37285, 0x561d5771), WTCP(0x5e6b2ca8, 0x566c8f55), WTCP(0x5e22a487, 0x56bb8a90),
    WTCP(0x5dd9da55, 0x570a48ec), WTCP(0x5d90ce45, 0x5758ca31), WTCP(0x5d47808a, 0x57a70e29),
    WTCP(0x5cfdf157, 0x57f5149d), WTCP(0x5cb420e0, 0x5842dd54), WTCP(0x5c6a0f59, 0x5890681a),
    WTCP(0x5c1fbcf6, 0x58ddb4b8), WTCP(0x5bd529eb, 0x592ac2f7), WTCP(0x5b8a566c, 0x597792a1),
    WTCP(0x5b3f42ae, 0x59c42381), WTCP(0x5af3eee6, 0x5a107561), WTCP(0x5aa85b48, 0x5a5c880a),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow512[] = {
    WTCP(0x7ffff621, 0x003243f5), WTCP(0x7fffa72c, 0x0096cbc1), WTCP(0x7fff0943, 0x00fb5330),
    WTCP(0x7ffe1c65, 0x015fda03), WTCP(0x7ffce093, 0x01c45ffe), WTCP(0x7ffb55ce, 0x0228e4e2),
    WTCP(0x7ff97c18, 0x028d6870), WTCP(0x7ff75370, 0x02f1ea6c), WTCP(0x7ff4dbd9, 0x03566a96),
    WTCP(0x7ff21553, 0x03bae8b2), WTCP(0x7feeffe1, 0x041f6480), WTCP(0x7feb9b85, 0x0483ddc3),
    WTCP(0x7fe7e841, 0x04e8543e), WTCP(0x7fe3e616, 0x054cc7b1), WTCP(0x7fdf9508, 0x05b137df),
    WTCP(0x7fdaf519, 0x0615a48b), WTCP(0x7fd6064c, 0x067a0d76), WTCP(0x7fd0c8a3, 0x06de7262),
    WTCP(0x7fcb3c23, 0x0742d311), WTCP(0x7fc560cf, 0x07a72f45), WTCP(0x7fbf36aa, 0x080b86c2),
    WTCP(0x7fb8bdb8, 0x086fd947), WTCP(0x7fb1f5fc, 0x08d42699), WTCP(0x7faadf7c, 0x09386e78),
    WTCP(0x7fa37a3c, 0x099cb0a7), WTCP(0x7f9bc640, 0x0a00ece8), WTCP(0x7f93c38c, 0x0a6522fe),
    WTCP(0x7f8b7227, 0x0ac952aa), WTCP(0x7f82d214, 0x0b2d7baf), WTCP(0x7f79e35a, 0x0b919dcf),
    WTCP(0x7f70a5fe, 0x0bf5b8cb), WTCP(0x7f671a05, 0x0c59cc68), WTCP(0x7f5d3f75, 0x0cbdd865),
    WTCP(0x7f531655, 0x0d21dc87), WTCP(0x7f489eaa, 0x0d85d88f), WTCP(0x7f3dd87c, 0x0de9cc40),
    WTCP(0x7f32c3d1, 0x0e4db75b), WTCP(0x7f2760af, 0x0eb199a4), WTCP(0x7f1baf1e, 0x0f1572dc),
    WTCP(0x7f0faf25, 0x0f7942c7), WTCP(0x7f0360cb, 0x0fdd0926), WTCP(0x7ef6c418, 0x1040c5bb),
    WTCP(0x7ee9d914, 0x10a4784b), WTCP(0x7edc9fc6, 0x11082096), WTCP(0x7ecf1837, 0x116bbe60),
    WTCP(0x7ec14270, 0x11cf516a), WTCP(0x7eb31e78, 0x1232d979), WTCP(0x7ea4ac58, 0x1296564d),
    WTCP(0x7e95ec1a, 0x12f9c7aa), WTCP(0x7e86ddc6, 0x135d2d53), WTCP(0x7e778166, 0x13c0870a),
    WTCP(0x7e67d703, 0x1423d492), WTCP(0x7e57dea7, 0x148715ae), WTCP(0x7e47985b, 0x14ea4a1f),
    WTCP(0x7e37042a, 0x154d71aa), WTCP(0x7e26221f, 0x15b08c12), WTCP(0x7e14f242, 0x16139918),
    WTCP(0x7e0374a0, 0x1676987f), WTCP(0x7df1a942, 0x16d98a0c), WTCP(0x7ddf9034, 0x173c6d80),
    WTCP(0x7dcd2981, 0x179f429f), WTCP(0x7dba7534, 0x1802092c), WTCP(0x7da77359, 0x1864c0ea),
    WTCP(0x7d9423fc, 0x18c7699b), WTCP(0x7d808728, 0x192a0304), WTCP(0x7d6c9ce9, 0x198c8ce7),
    WTCP(0x7d58654d, 0x19ef0707), WTCP(0x7d43e05e, 0x1a517128), WTCP(0x7d2f0e2b, 0x1ab3cb0d),
    WTCP(0x7d19eebf, 0x1b161479), WTCP(0x7d048228, 0x1b784d30), WTCP(0x7ceec873, 0x1bda74f6),
    WTCP(0x7cd8c1ae, 0x1c3c8b8c), WTCP(0x7cc26de5, 0x1c9e90b8), WTCP(0x7cabcd28, 0x1d00843d),
    WTCP(0x7c94df83, 0x1d6265dd), WTCP(0x7c7da505, 0x1dc4355e), WTCP(0x7c661dbc, 0x1e25f282),
    WTCP(0x7c4e49b7, 0x1e879d0d), WTCP(0x7c362904, 0x1ee934c3), WTCP(0x7c1dbbb3, 0x1f4ab968),
    WTCP(0x7c0501d2, 0x1fac2abf), WTCP(0x7bebfb70, 0x200d888d), WTCP(0x7bd2a89e, 0x206ed295),
    WTCP(0x7bb9096b, 0x20d0089c), WTCP(0x7b9f1de6, 0x21312a65), WTCP(0x7b84e61f, 0x219237b5),
    WTCP(0x7b6a6227, 0x21f3304f), WTCP(0x7b4f920e, 0x225413f8), WTCP(0x7b3475e5, 0x22b4e274),
    WTCP(0x7b190dbc, 0x23159b88), WTCP(0x7afd59a4, 0x23763ef7), WTCP(0x7ae159ae, 0x23d6cc87),
    WTCP(0x7ac50dec, 0x243743fa), WTCP(0x7aa8766f, 0x2497a517), WTCP(0x7a8b9348, 0x24f7efa2),
    WTCP(0x7a6e648a, 0x2558235f), WTCP(0x7a50ea47, 0x25b84012), WTCP(0x7a332490, 0x26184581),
    WTCP(0x7a151378, 0x26783370), WTCP(0x79f6b711, 0x26d809a5), WTCP(0x79d80f6f, 0x2737c7e3),
    WTCP(0x79b91ca4, 0x27976df1), WTCP(0x7999dec4, 0x27f6fb92), WTCP(0x797a55e0, 0x2856708d),
    WTCP(0x795a820e, 0x28b5cca5), WTCP(0x793a6361, 0x29150fa1), WTCP(0x7919f9ec, 0x29743946),
    WTCP(0x78f945c3, 0x29d34958), WTCP(0x78d846fb, 0x2a323f9e), WTCP(0x78b6fda8, 0x2a911bdc),
    WTCP(0x789569df, 0x2aefddd8), WTCP(0x78738bb3, 0x2b4e8558), WTCP(0x7851633b, 0x2bad1221),
    WTCP(0x782ef08b, 0x2c0b83fa), WTCP(0x780c33b8, 0x2c69daa6), WTCP(0x77e92cd9, 0x2cc815ee),
    WTCP(0x77c5dc01, 0x2d263596), WTCP(0x77a24148, 0x2d843964), WTCP(0x777e5cc3, 0x2de2211e),
    WTCP(0x775a2e89, 0x2e3fec8b), WTCP(0x7735b6af, 0x2e9d9b70), WTCP(0x7710f54c, 0x2efb2d95),
    WTCP(0x76ebea77, 0x2f58a2be), WTCP(0x76c69647, 0x2fb5fab2), WTCP(0x76a0f8d2, 0x30133539),
    WTCP(0x767b1231, 0x30705217), WTCP(0x7654e279, 0x30cd5115), WTCP(0x762e69c4, 0x312a31f8),
    WTCP(0x7607a828, 0x3186f487), WTCP(0x75e09dbd, 0x31e39889), WTCP(0x75b94a9c, 0x32401dc6),
    WTCP(0x7591aedd, 0x329c8402), WTCP(0x7569ca99, 0x32f8cb07), WTCP(0x75419de7, 0x3354f29b),
    WTCP(0x751928e0, 0x33b0fa84), WTCP(0x74f06b9e, 0x340ce28b), WTCP(0x74c7663a, 0x3468aa76),
    WTCP(0x749e18cd, 0x34c4520d), WTCP(0x74748371, 0x351fd918), WTCP(0x744aa63f, 0x357b3f5d),
    WTCP(0x74208150, 0x35d684a6), WTCP(0x73f614c0, 0x3631a8b8), WTCP(0x73cb60a8, 0x368cab5c),
    WTCP(0x73a06522, 0x36e78c5b), WTCP(0x73752249, 0x37424b7b), WTCP(0x73499838, 0x379ce885),
    WTCP(0x731dc70a, 0x37f76341), WTCP(0x72f1aed9, 0x3851bb77), WTCP(0x72c54fc1, 0x38abf0ef),
    WTCP(0x7298a9dd, 0x39060373), WTCP(0x726bbd48, 0x395ff2c9), WTCP(0x723e8a20, 0x39b9bebc),
    WTCP(0x7211107e, 0x3a136712), WTCP(0x71e35080, 0x3a6ceb96), WTCP(0x71b54a41, 0x3ac64c0f),
    WTCP(0x7186fdde, 0x3b1f8848), WTCP(0x71586b74, 0x3b78a007), WTCP(0x7129931f, 0x3bd19318),
    WTCP(0x70fa74fc, 0x3c2a6142), WTCP(0x70cb1128, 0x3c830a50), WTCP(0x709b67c0, 0x3cdb8e09),
    WTCP(0x706b78e3, 0x3d33ec39), WTCP(0x703b44ad, 0x3d8c24a8), WTCP(0x700acb3c, 0x3de4371f),
    WTCP(0x6fda0cae, 0x3e3c2369), WTCP(0x6fa90921, 0x3e93e950), WTCP(0x6f77c0b3, 0x3eeb889c),
    WTCP(0x6f463383, 0x3f430119), WTCP(0x6f1461b0, 0x3f9a5290), WTCP(0x6ee24b57, 0x3ff17cca),
    WTCP(0x6eaff099, 0x40487f94), WTCP(0x6e7d5193, 0x409f5ab6), WTCP(0x6e4a6e66, 0x40f60dfb),
    WTCP(0x6e174730, 0x414c992f), WTCP(0x6de3dc11, 0x41a2fc1a), WTCP(0x6db02d29, 0x41f93689),
    WTCP(0x6d7c3a98, 0x424f4845), WTCP(0x6d48047e, 0x42a5311b), WTCP(0x6d138afb, 0x42faf0d4),
    WTCP(0x6cdece2f, 0x4350873c), WTCP(0x6ca9ce3b, 0x43a5f41e), WTCP(0x6c748b3f, 0x43fb3746),
    WTCP(0x6c3f055d, 0x4450507e), WTCP(0x6c093cb6, 0x44a53f93), WTCP(0x6bd3316a, 0x44fa0450),
    WTCP(0x6b9ce39b, 0x454e9e80), WTCP(0x6b66536b, 0x45a30df0), WTCP(0x6b2f80fb, 0x45f7526b),
    WTCP(0x6af86c6c, 0x464b6bbe), WTCP(0x6ac115e2, 0x469f59b4), WTCP(0x6a897d7d, 0x46f31c1a),
    WTCP(0x6a51a361, 0x4746b2bc), WTCP(0x6a1987b0, 0x479a1d67), WTCP(0x69e12a8c, 0x47ed5be6),
    WTCP(0x69a88c19, 0x48406e08), WTCP(0x696fac78, 0x48935397), WTCP(0x69368bce, 0x48e60c62),
    WTCP(0x68fd2a3d, 0x49389836), WTCP(0x68c387e9, 0x498af6df), WTCP(0x6889a4f6, 0x49dd282a),
    WTCP(0x684f8186, 0x4a2f2be6), WTCP(0x68151dbe, 0x4a8101de), WTCP(0x67da79c3, 0x4ad2a9e2),
    WTCP(0x679f95b7, 0x4b2423be), WTCP(0x676471c0, 0x4b756f40), WTCP(0x67290e02, 0x4bc68c36),
    WTCP(0x66ed6aa1, 0x4c177a6e), WTCP(0x66b187c3, 0x4c6839b7), WTCP(0x6675658c, 0x4cb8c9dd),
    WTCP(0x66390422, 0x4d092ab0), WTCP(0x65fc63a9, 0x4d595bfe), WTCP(0x65bf8447, 0x4da95d96),
    WTCP(0x65826622, 0x4df92f46), WTCP(0x6545095f, 0x4e48d0dd), WTCP(0x65076e25, 0x4e984229),
    WTCP(0x64c99498, 0x4ee782fb), WTCP(0x648b7ce0, 0x4f369320), WTCP(0x644d2722, 0x4f857269),
    WTCP(0x640e9386, 0x4fd420a4), WTCP(0x63cfc231, 0x50229da1), WTCP(0x6390b34a, 0x5070e92f),
    WTCP(0x635166f9, 0x50bf031f), WTCP(0x6311dd64, 0x510ceb40), WTCP(0x62d216b3, 0x515aa162),
    WTCP(0x6292130c, 0x51a82555), WTCP(0x6251d298, 0x51f576ea), WTCP(0x6211557e, 0x524295f0),
    WTCP(0x61d09be5, 0x528f8238), WTCP(0x618fa5f7, 0x52dc3b92), WTCP(0x614e73da, 0x5328c1d0),
    WTCP(0x610d05b7, 0x537514c2), WTCP(0x60cb5bb7, 0x53c13439), WTCP(0x60897601, 0x540d2005),
    WTCP(0x604754bf, 0x5458d7f9), WTCP(0x6004f819, 0x54a45be6), WTCP(0x5fc26038, 0x54efab9c),
    WTCP(0x5f7f8d46, 0x553ac6ee), WTCP(0x5f3c7f6b, 0x5585adad), WTCP(0x5ef936d1, 0x55d05faa),
    WTCP(0x5eb5b3a2, 0x561adcb9), WTCP(0x5e71f606, 0x566524aa), WTCP(0x5e2dfe29, 0x56af3750),
    WTCP(0x5de9cc33, 0x56f9147e), WTCP(0x5da5604f, 0x5742bc06), WTCP(0x5d60baa7, 0x578c2dba),
    WTCP(0x5d1bdb65, 0x57d5696d), WTCP(0x5cd6c2b5, 0x581e6ef1), WTCP(0x5c9170bf, 0x58673e1b),
    WTCP(0x5c4be5b0, 0x58afd6bd), WTCP(0x5c0621b2, 0x58f838a9), WTCP(0x5bc024f0, 0x594063b5),
    WTCP(0x5b79ef96, 0x598857b2), WTCP(0x5b3381ce, 0x59d01475), WTCP(0x5aecdbc5, 0x5a1799d1),
    WTCP(0x5aa5fda5, 0x5a5ee79a),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow768[] = {
    WTCP(0x7ffffb9d, 0x002182a4), WTCP(0x7fffd886, 0x006487e3), WTCP(0x7fff9257, 0x00a78d06),
    WTCP(0x7fff2910, 0x00ea91fc), WTCP(0x7ffe9cb2, 0x012d96b1), WTCP(0x7ffded3d, 0x01709b13),
    WTCP(0x7ffd1ab2, 0x01b39f11), WTCP(0x7ffc250f, 0x01f6a297), WTCP(0x7ffb0c56, 0x0239a593),
    WTCP(0x7ff9d087, 0x027ca7f3), WTCP(0x7ff871a2, 0x02bfa9a4), WTCP(0x7ff6efa7, 0x0302aa95),
    WTCP(0x7ff54a98, 0x0345aab2), WTCP(0x7ff38274, 0x0388a9ea), WTCP(0x7ff1973b, 0x03cba829),
    WTCP(0x7fef88ef, 0x040ea55e), WTCP(0x7fed5791, 0x0451a177), WTCP(0x7feb031f, 0x04949c60),
    WTCP(0x7fe88b9c, 0x04d79608), WTCP(0x7fe5f108, 0x051a8e5c), WTCP(0x7fe33364, 0x055d854a),
    WTCP(0x7fe052af, 0x05a07abf), WTCP(0x7fdd4eec, 0x05e36ea9), WTCP(0x7fda281b, 0x062660f6),
    WTCP(0x7fd6de3d, 0x06695194), WTCP(0x7fd37153, 0x06ac406f), WTCP(0x7fcfe15d, 0x06ef2d76),
    WTCP(0x7fcc2e5d, 0x07321897), WTCP(0x7fc85854, 0x077501be), WTCP(0x7fc45f42, 0x07b7e8da),
    WTCP(0x7fc04329, 0x07facdd9), WTCP(0x7fbc040a, 0x083db0a7), WTCP(0x7fb7a1e6, 0x08809133),
    WTCP(0x7fb31cbf, 0x08c36f6a), WTCP(0x7fae7495, 0x09064b3a), WTCP(0x7fa9a96a, 0x09492491),
    WTCP(0x7fa4bb3f, 0x098bfb5c), WTCP(0x7f9faa15, 0x09cecf89), WTCP(0x7f9a75ef, 0x0a11a106),
    WTCP(0x7f951ecc, 0x0a546fc0), WTCP(0x7f8fa4b0, 0x0a973ba5), WTCP(0x7f8a079a, 0x0ada04a3),
    WTCP(0x7f84478e, 0x0b1ccaa7), WTCP(0x7f7e648c, 0x0b5f8d9f), WTCP(0x7f785e96, 0x0ba24d79),
    WTCP(0x7f7235ad, 0x0be50a23), WTCP(0x7f6be9d4, 0x0c27c389), WTCP(0x7f657b0c, 0x0c6a799b),
    WTCP(0x7f5ee957, 0x0cad2c45), WTCP(0x7f5834b7, 0x0cefdb76), WTCP(0x7f515d2d, 0x0d32871a),
    WTCP(0x7f4a62bb, 0x0d752f20), WTCP(0x7f434563, 0x0db7d376), WTCP(0x7f3c0528, 0x0dfa7409),
    WTCP(0x7f34a20b, 0x0e3d10c7), WTCP(0x7f2d1c0e, 0x0e7fa99e), WTCP(0x7f257334, 0x0ec23e7b),
    WTCP(0x7f1da77e, 0x0f04cf4c), WTCP(0x7f15b8ee, 0x0f475bff), WTCP(0x7f0da787, 0x0f89e482),
    WTCP(0x7f05734b, 0x0fcc68c2), WTCP(0x7efd1c3c, 0x100ee8ad), WTCP(0x7ef4a25d, 0x10516432),
    WTCP(0x7eec05af, 0x1093db3d), WTCP(0x7ee34636, 0x10d64dbd), WTCP(0x7eda63f3, 0x1118bb9f),
    WTCP(0x7ed15ee9, 0x115b24d1), WTCP(0x7ec8371a, 0x119d8941), WTCP(0x7ebeec89, 0x11dfe8dc),
    WTCP(0x7eb57f39, 0x12224392), WTCP(0x7eabef2c, 0x1264994e), WTCP(0x7ea23c65, 0x12a6ea00),
    WTCP(0x7e9866e6, 0x12e93594), WTCP(0x7e8e6eb2, 0x132b7bf9), WTCP(0x7e8453cc, 0x136dbd1d),
    WTCP(0x7e7a1636, 0x13aff8ed), WTCP(0x7e6fb5f4, 0x13f22f58), WTCP(0x7e653308, 0x1434604a),
    WTCP(0x7e5a8d75, 0x14768bb3), WTCP(0x7e4fc53e, 0x14b8b17f), WTCP(0x7e44da66, 0x14fad19e),
    WTCP(0x7e39ccf0, 0x153cebfb), WTCP(0x7e2e9cdf, 0x157f0086), WTCP(0x7e234a36, 0x15c10f2d),
    WTCP(0x7e17d4f8, 0x160317dc), WTCP(0x7e0c3d29, 0x16451a83), WTCP(0x7e0082cb, 0x1687170f),
    WTCP(0x7df4a5e2, 0x16c90d6e), WTCP(0x7de8a670, 0x170afd8d), WTCP(0x7ddc847a, 0x174ce75b),
    WTCP(0x7dd04003, 0x178ecac6), WTCP(0x7dc3d90d, 0x17d0a7bc), WTCP(0x7db74f9d, 0x18127e2a),
    WTCP(0x7daaa3b5, 0x18544dff), WTCP(0x7d9dd55a, 0x18961728), WTCP(0x7d90e48f, 0x18d7d993),
    WTCP(0x7d83d156, 0x1919952f), WTCP(0x7d769bb5, 0x195b49ea), WTCP(0x7d6943ae, 0x199cf7b0),
    WTCP(0x7d5bc946, 0x19de9e72), WTCP(0x7d4e2c7f, 0x1a203e1b), WTCP(0x7d406d5e, 0x1a61d69b),
    WTCP(0x7d328be6, 0x1aa367df), WTCP(0x7d24881b, 0x1ae4f1d6), WTCP(0x7d166201, 0x1b26746d),
    WTCP(0x7d08199c, 0x1b67ef93), WTCP(0x7cf9aef0, 0x1ba96335), WTCP(0x7ceb2201, 0x1beacf42),
    WTCP(0x7cdc72d3, 0x1c2c33a7), WTCP(0x7ccda169, 0x1c6d9053), WTCP(0x7cbeadc8, 0x1caee534),
    WTCP(0x7caf97f4, 0x1cf03238), WTCP(0x7ca05ff1, 0x1d31774d), WTCP(0x7c9105c3, 0x1d72b461),
    WTCP(0x7c81896f, 0x1db3e962), WTCP(0x7c71eaf9, 0x1df5163f), WTCP(0x7c622a64, 0x1e363ae5),
    WTCP(0x7c5247b6, 0x1e775743), WTCP(0x7c4242f2, 0x1eb86b46), WTCP(0x7c321c1e, 0x1ef976de),
    WTCP(0x7c21d33c, 0x1f3a79f7), WTCP(0x7c116853, 0x1f7b7481), WTCP(0x7c00db66, 0x1fbc6669),
    WTCP(0x7bf02c7b, 0x1ffd4f9e), WTCP(0x7bdf5b94, 0x203e300d), WTCP(0x7bce68b8, 0x207f07a6),
    WTCP(0x7bbd53eb, 0x20bfd656), WTCP(0x7bac1d31, 0x21009c0c), WTCP(0x7b9ac490, 0x214158b5),
    WTCP(0x7b894a0b, 0x21820c41), WTCP(0x7b77ada8, 0x21c2b69c), WTCP(0x7b65ef6c, 0x220357b6),
    WTCP(0x7b540f5b, 0x2243ef7d), WTCP(0x7b420d7a, 0x22847de0), WTCP(0x7b2fe9cf, 0x22c502cb),
    WTCP(0x7b1da45e, 0x23057e2e), WTCP(0x7b0b3d2c, 0x2345eff8), WTCP(0x7af8b43f, 0x23865816),
    WTCP(0x7ae6099b, 0x23c6b676), WTCP(0x7ad33d45, 0x24070b08), WTCP(0x7ac04f44, 0x244755b9),
    WTCP(0x7aad3f9b, 0x24879678), WTCP(0x7a9a0e50, 0x24c7cd33), WTCP(0x7a86bb68, 0x2507f9d8),
    WTCP(0x7a7346e9, 0x25481c57), WTCP(0x7a5fb0d8, 0x2588349d), WTCP(0x7a4bf93a, 0x25c84299),
    WTCP(0x7a382015, 0x26084639), WTCP(0x7a24256f, 0x26483f6c), WTCP(0x7a10094c, 0x26882e21),
    WTCP(0x79fbcbb2, 0x26c81245), WTCP(0x79e76ca7, 0x2707ebc7), WTCP(0x79d2ec30, 0x2747ba95),
    WTCP(0x79be4a53, 0x27877e9f), WTCP(0x79a98715, 0x27c737d3), WTCP(0x7994a27d, 0x2806e61f),
    WTCP(0x797f9c90, 0x28468971), WTCP(0x796a7554, 0x288621b9), WTCP(0x79552cce, 0x28c5aee5),
    WTCP(0x793fc305, 0x290530e3), WTCP(0x792a37fe, 0x2944a7a2), WTCP(0x79148bbf, 0x29841311),
    WTCP(0x78febe4e, 0x29c3731e), WTCP(0x78e8cfb2, 0x2a02c7b8), WTCP(0x78d2bfef, 0x2a4210ce),
    WTCP(0x78bc8f0d, 0x2a814e4d), WTCP(0x78a63d11, 0x2ac08026), WTCP(0x788fca01, 0x2affa646),
    WTCP(0x787935e4, 0x2b3ec09c), WTCP(0x786280bf, 0x2b7dcf17), WTCP(0x784baa9a, 0x2bbcd1a6),
    WTCP(0x7834b37a, 0x2bfbc837), WTCP(0x781d9b65, 0x2c3ab2b9), WTCP(0x78066262, 0x2c79911b),
    WTCP(0x77ef0877, 0x2cb8634b), WTCP(0x77d78daa, 0x2cf72939), WTCP(0x77bff203, 0x2d35e2d3),
    WTCP(0x77a83587, 0x2d749008), WTCP(0x7790583e, 0x2db330c7), WTCP(0x77785a2d, 0x2df1c4fe),
    WTCP(0x77603b5a, 0x2e304c9d), WTCP(0x7747fbce, 0x2e6ec792), WTCP(0x772f9b8e, 0x2ead35cd),
    WTCP(0x77171aa1, 0x2eeb973b), WTCP(0x76fe790e, 0x2f29ebcc), WTCP(0x76e5b6dc, 0x2f68336f),
    WTCP(0x76ccd411, 0x2fa66e13), WTCP(0x76b3d0b4, 0x2fe49ba7), WTCP(0x769aaccc, 0x3022bc19),
    WTCP(0x7681685f, 0x3060cf59), WTCP(0x76680376, 0x309ed556), WTCP(0x764e7e17, 0x30dccdfe),
    WTCP(0x7634d848, 0x311ab941), WTCP(0x761b1211, 0x3158970e), WTCP(0x76012b79, 0x31966753),
    WTCP(0x75e72487, 0x31d42a00), WTCP(0x75ccfd42, 0x3211df04), WTCP(0x75b2b5b2, 0x324f864e),
    WTCP(0x75984ddc, 0x328d1fcc), WTCP(0x757dc5ca, 0x32caab6f), WTCP(0x75631d82, 0x33082925),
    WTCP(0x7548550b, 0x334598de), WTCP(0x752d6c6c, 0x3382fa88), WTCP(0x751263ae, 0x33c04e13),
    WTCP(0x74f73ad7, 0x33fd936e), WTCP(0x74dbf1ef, 0x343aca87), WTCP(0x74c088fe, 0x3477f350),
    WTCP(0x74a5000a, 0x34b50db5), WTCP(0x7489571c, 0x34f219a8), WTCP(0x746d8e3a, 0x352f1716),
    WTCP(0x7451a56e, 0x356c05f0), WTCP(0x74359cbd, 0x35a8e625), WTCP(0x74197431, 0x35e5b7a3),
    WTCP(0x73fd2bd0, 0x36227a5b), WTCP(0x73e0c3a3, 0x365f2e3b), WTCP(0x73c43bb1, 0x369bd334),
    WTCP(0x73a79402, 0x36d86934), WTCP(0x738acc9e, 0x3714f02a), WTCP(0x736de58d, 0x37516807),
    WTCP(0x7350ded7, 0x378dd0b9), WTCP(0x7333b883, 0x37ca2a30), WTCP(0x7316729a, 0x3806745c),
    WTCP(0x72f90d24, 0x3842af2b), WTCP(0x72db8828, 0x387eda8e), WTCP(0x72bde3af, 0x38baf674),
    WTCP(0x72a01fc2, 0x38f702cd), WTCP(0x72823c67, 0x3932ff87), WTCP(0x726439a8, 0x396eec93),
    WTCP(0x7246178c, 0x39aac9e0), WTCP(0x7227d61c, 0x39e6975e), WTCP(0x72097560, 0x3a2254fc),
    WTCP(0x71eaf561, 0x3a5e02aa), WTCP(0x71cc5626, 0x3a99a057), WTCP(0x71ad97b9, 0x3ad52df4),
    WTCP(0x718eba22, 0x3b10ab70), WTCP(0x716fbd68, 0x3b4c18ba), WTCP(0x7150a195, 0x3b8775c2),
    WTCP(0x713166b1, 0x3bc2c279), WTCP(0x71120cc5, 0x3bfdfecd), WTCP(0x70f293d9, 0x3c392aaf),
    WTCP(0x70d2fbf6, 0x3c74460e), WTCP(0x70b34525, 0x3caf50da), WTCP(0x70936f6e, 0x3cea4b04),
    WTCP(0x70737ad9, 0x3d253479), WTCP(0x70536771, 0x3d600d2c), WTCP(0x7033353d, 0x3d9ad50b),
    WTCP(0x7012e447, 0x3dd58c06), WTCP(0x6ff27497, 0x3e10320d), WTCP(0x6fd1e635, 0x3e4ac711),
    WTCP(0x6fb1392c, 0x3e854b01), WTCP(0x6f906d84, 0x3ebfbdcd), WTCP(0x6f6f8346, 0x3efa1f65),
    WTCP(0x6f4e7a7b, 0x3f346fb8), WTCP(0x6f2d532c, 0x3f6eaeb8), WTCP(0x6f0c0d62, 0x3fa8dc54),
    WTCP(0x6eeaa927, 0x3fe2f87c), WTCP(0x6ec92683, 0x401d0321), WTCP(0x6ea7857f, 0x4056fc31),
    WTCP(0x6e85c626, 0x4090e39e), WTCP(0x6e63e87f, 0x40cab958), WTCP(0x6e41ec95, 0x41047d4e),
    WTCP(0x6e1fd271, 0x413e2f71), WTCP(0x6dfd9a1c, 0x4177cfb1), WTCP(0x6ddb439f, 0x41b15dfe),
    WTCP(0x6db8cf04, 0x41eada49), WTCP(0x6d963c54, 0x42244481), WTCP(0x6d738b99, 0x425d9c97),
    WTCP(0x6d50bcdc, 0x4296e27b), WTCP(0x6d2dd027, 0x42d0161e), WTCP(0x6d0ac584, 0x43093770),
    WTCP(0x6ce79cfc, 0x43424661), WTCP(0x6cc45698, 0x437b42e1), WTCP(0x6ca0f262, 0x43b42ce1),
    WTCP(0x6c7d7065, 0x43ed0452), WTCP(0x6c59d0a9, 0x4425c923), WTCP(0x6c361339, 0x445e7b46),
    WTCP(0x6c12381e, 0x44971aaa), WTCP(0x6bee3f62, 0x44cfa740), WTCP(0x6bca2910, 0x450820f8),
    WTCP(0x6ba5f530, 0x454087c4), WTCP(0x6b81a3cd, 0x4578db93), WTCP(0x6b5d34f1, 0x45b11c57),
    WTCP(0x6b38a8a6, 0x45e949ff), WTCP(0x6b13fef5, 0x4621647d), WTCP(0x6aef37e9, 0x46596bc1),
    WTCP(0x6aca538c, 0x46915fbb), WTCP(0x6aa551e9, 0x46c9405c), WTCP(0x6a803308, 0x47010d96),
    WTCP(0x6a5af6f5, 0x4738c758), WTCP(0x6a359db9, 0x47706d93), WTCP(0x6a102760, 0x47a80039),
    WTCP(0x69ea93f2, 0x47df7f3a), WTCP(0x69c4e37a, 0x4816ea86), WTCP(0x699f1604, 0x484e420f),
    WTCP(0x69792b98, 0x488585c5), WTCP(0x69532442, 0x48bcb599), WTCP(0x692d000c, 0x48f3d17c),
    WTCP(0x6906bf00, 0x492ad95f), WTCP(0x68e06129, 0x4961cd33), WTCP(0x68b9e692, 0x4998ace9),
    WTCP(0x68934f44, 0x49cf7871), WTCP(0x686c9b4b, 0x4a062fbd), WTCP(0x6845cab1, 0x4a3cd2be),
    WTCP(0x681edd81, 0x4a736165), WTCP(0x67f7d3c5, 0x4aa9dba2), WTCP(0x67d0ad88, 0x4ae04167),
    WTCP(0x67a96ad5, 0x4b1692a5), WTCP(0x67820bb7, 0x4b4ccf4d), WTCP(0x675a9038, 0x4b82f750),
    WTCP(0x6732f863, 0x4bb90aa0), WTCP(0x670b4444, 0x4bef092d), WTCP(0x66e373e4, 0x4c24f2e9),
    WTCP(0x66bb8750, 0x4c5ac7c4), WTCP(0x66937e91, 0x4c9087b1), WTCP(0x666b59b3, 0x4cc632a0),
    WTCP(0x664318c0, 0x4cfbc883), WTCP(0x661abbc5, 0x4d31494b), WTCP(0x65f242cc, 0x4d66b4e9),
    WTCP(0x65c9addf, 0x4d9c0b4f), WTCP(0x65a0fd0b, 0x4dd14c6e), WTCP(0x6578305a, 0x4e067837),
    WTCP(0x654f47d7, 0x4e3b8e9d), WTCP(0x6526438f, 0x4e708f8f), WTCP(0x64fd238b, 0x4ea57b01),
    WTCP(0x64d3e7d7, 0x4eda50e2), WTCP(0x64aa907f, 0x4f0f1126), WTCP(0x64811d8e, 0x4f43bbbd),
    WTCP(0x64578f0f, 0x4f785099), WTCP(0x642de50d, 0x4faccfab), WTCP(0x64041f95, 0x4fe138e5),
    WTCP(0x63da3eb1, 0x50158c39), WTCP(0x63b0426d, 0x5049c999), WTCP(0x63862ad5, 0x507df0f6),
    WTCP(0x635bf7f3, 0x50b20241), WTCP(0x6331a9d4, 0x50e5fd6d), WTCP(0x63074084, 0x5119e26b),
    WTCP(0x62dcbc0d, 0x514db12d), WTCP(0x62b21c7b, 0x518169a5), WTCP(0x628761db, 0x51b50bc4),
    WTCP(0x625c8c38, 0x51e8977d), WTCP(0x62319b9d, 0x521c0cc2), WTCP(0x62069017, 0x524f6b83),
    WTCP(0x61db69b1, 0x5282b3b4), WTCP(0x61b02876, 0x52b5e546), WTCP(0x6184cc74, 0x52e9002a),
    WTCP(0x615955b6, 0x531c0454), WTCP(0x612dc447, 0x534ef1b5), WTCP(0x61021834, 0x5381c83f),
    WTCP(0x60d65188, 0x53b487e5), WTCP(0x60aa7050, 0x53e73097), WTCP(0x607e7497, 0x5419c249),
    WTCP(0x60525e6b, 0x544c3cec), WTCP(0x60262dd6, 0x547ea073), WTCP(0x5ff9e2e5, 0x54b0ecd0),
    WTCP(0x5fcd7da4, 0x54e321f5), WTCP(0x5fa0fe1f, 0x55153fd4), WTCP(0x5f746462, 0x55474660),
    WTCP(0x5f47b07a, 0x5579358b), WTCP(0x5f1ae274, 0x55ab0d46), WTCP(0x5eedfa5a, 0x55dccd86),
    WTCP(0x5ec0f839, 0x560e763b), WTCP(0x5e93dc1f, 0x56400758), WTCP(0x5e66a617, 0x567180d0),
    WTCP(0x5e39562d, 0x56a2e295), WTCP(0x5e0bec6e, 0x56d42c99), WTCP(0x5dde68e7, 0x57055ed0),
    WTCP(0x5db0cba4, 0x5736792b), WTCP(0x5d8314b1, 0x57677b9d), WTCP(0x5d55441b, 0x57986619),
    WTCP(0x5d2759ee, 0x57c93891), WTCP(0x5cf95638, 0x57f9f2f8), WTCP(0x5ccb3905, 0x582a9540),
    WTCP(0x5c9d0260, 0x585b1f5c), WTCP(0x5c6eb258, 0x588b9140), WTCP(0x5c4048f9, 0x58bbeadd),
    WTCP(0x5c11c64f, 0x58ec2c26), WTCP(0x5be32a67, 0x591c550e), WTCP(0x5bb4754e, 0x594c6588),
    WTCP(0x5b85a711, 0x597c5d87), WTCP(0x5b56bfbd, 0x59ac3cfd), WTCP(0x5b27bf5e, 0x59dc03de),
    WTCP(0x5af8a602, 0x5a0bb21c), WTCP(0x5ac973b5, 0x5a3b47ab), WTCP(0x5a9a2884, 0x5a6ac47c),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow960[] = {
    WTCP(0x7ffffd31, 0x001aceea), WTCP(0x7fffe6bc, 0x00506cb9), WTCP(0x7fffb9d1, 0x00860a79),
    WTCP(0x7fff7671, 0x00bba822), WTCP(0x7fff1c9b, 0x00f145ab), WTCP(0x7ffeac50, 0x0126e309),
    WTCP(0x7ffe2590, 0x015c8033), WTCP(0x7ffd885a, 0x01921d20), WTCP(0x7ffcd4b0, 0x01c7b9c6),
    WTCP(0x7ffc0a91, 0x01fd561d), WTCP(0x7ffb29fd, 0x0232f21a), WTCP(0x7ffa32f4, 0x02688db4),
    WTCP(0x7ff92577, 0x029e28e2), WTCP(0x7ff80186, 0x02d3c39b), WTCP(0x7ff6c720, 0x03095dd5),
    WTCP(0x7ff57647, 0x033ef786), WTCP(0x7ff40efa, 0x037490a5), WTCP(0x7ff2913a, 0x03aa292a),
    WTCP(0x7ff0fd07, 0x03dfc109), WTCP(0x7fef5260, 0x0415583b), WTCP(0x7fed9148, 0x044aeeb5),
    WTCP(0x7febb9bd, 0x0480846e), WTCP(0x7fe9cbc0, 0x04b6195d), WTCP(0x7fe7c752, 0x04ebad79),
    WTCP(0x7fe5ac72, 0x052140b7), WTCP(0x7fe37b22, 0x0556d30f), WTCP(0x7fe13361, 0x058c6478),
    WTCP(0x7fded530, 0x05c1f4e7), WTCP(0x7fdc608f, 0x05f78453), WTCP(0x7fd9d57f, 0x062d12b4),
    WTCP(0x7fd73401, 0x06629ffe), WTCP(0x7fd47c14, 0x06982c2b), WTCP(0x7fd1adb9, 0x06cdb72f),
    WTCP(0x7fcec8f1, 0x07034101), WTCP(0x7fcbcdbc, 0x0738c998), WTCP(0x7fc8bc1b, 0x076e50eb),
    WTCP(0x7fc5940e, 0x07a3d6f0), WTCP(0x7fc25596, 0x07d95b9e), WTCP(0x7fbf00b3, 0x080edeec),
    WTCP(0x7fbb9567, 0x084460cf), WTCP(0x7fb813b0, 0x0879e140), WTCP(0x7fb47b91, 0x08af6033),
    WTCP(0x7fb0cd0a, 0x08e4dda0), WTCP(0x7fad081b, 0x091a597e), WTCP(0x7fa92cc5, 0x094fd3c3),
    WTCP(0x7fa53b09, 0x09854c66), WTCP(0x7fa132e8, 0x09bac35d), WTCP(0x7f9d1461, 0x09f0389f),
    WTCP(0x7f98df77, 0x0a25ac23), WTCP(0x7f949429, 0x0a5b1dde), WTCP(0x7f903279, 0x0a908dc9),
    WTCP(0x7f8bba66, 0x0ac5fbd9), WTCP(0x7f872bf3, 0x0afb6805), WTCP(0x7f82871f, 0x0b30d244),
    WTCP(0x7f7dcbec, 0x0b663a8c), WTCP(0x7f78fa5b, 0x0b9ba0d5), WTCP(0x7f74126b, 0x0bd10513),
    WTCP(0x7f6f141f, 0x0c066740), WTCP(0x7f69ff76, 0x0c3bc74f), WTCP(0x7f64d473, 0x0c71253a),
    WTCP(0x7f5f9315, 0x0ca680f5), WTCP(0x7f5a3b5e, 0x0cdbda79), WTCP(0x7f54cd4f, 0x0d1131ba),
    WTCP(0x7f4f48e8, 0x0d4686b1), WTCP(0x7f49ae2a, 0x0d7bd954), WTCP(0x7f43fd18, 0x0db12999),
    WTCP(0x7f3e35b0, 0x0de67776), WTCP(0x7f3857f6, 0x0e1bc2e4), WTCP(0x7f3263e9, 0x0e510bd8),
    WTCP(0x7f2c598a, 0x0e865248), WTCP(0x7f2638db, 0x0ebb962c), WTCP(0x7f2001dd, 0x0ef0d77b),
    WTCP(0x7f19b491, 0x0f26162a), WTCP(0x7f1350f8, 0x0f5b5231), WTCP(0x7f0cd712, 0x0f908b86),
    WTCP(0x7f0646e2, 0x0fc5c220), WTCP(0x7effa069, 0x0ffaf5f6), WTCP(0x7ef8e3a6, 0x103026fe),
    WTCP(0x7ef2109d, 0x1065552e), WTCP(0x7eeb274d, 0x109a807e), WTCP(0x7ee427b9, 0x10cfa8e5),
    WTCP(0x7edd11e1, 0x1104ce58), WTCP(0x7ed5e5c6, 0x1139f0cf), WTCP(0x7ecea36b, 0x116f1040),
    WTCP(0x7ec74acf, 0x11a42ca2), WTCP(0x7ebfdbf5, 0x11d945eb), WTCP(0x7eb856de, 0x120e5c13),
    WTCP(0x7eb0bb8a, 0x12436f10), WTCP(0x7ea909fc, 0x12787ed8), WTCP(0x7ea14235, 0x12ad8b63),
    WTCP(0x7e996436, 0x12e294a7), WTCP(0x7e917000, 0x13179a9b), WTCP(0x7e896595, 0x134c9d34),
    WTCP(0x7e8144f6, 0x13819c6c), WTCP(0x7e790e25, 0x13b69836), WTCP(0x7e70c124, 0x13eb908c),
    WTCP(0x7e685df2, 0x14208563), WTCP(0x7e5fe493, 0x145576b1), WTCP(0x7e575508, 0x148a646e),
    WTCP(0x7e4eaf51, 0x14bf4e91), WTCP(0x7e45f371, 0x14f43510), WTCP(0x7e3d2169, 0x152917e1),
    WTCP(0x7e34393b, 0x155df6fc), WTCP(0x7e2b3ae8, 0x1592d257), WTCP(0x7e222672, 0x15c7a9ea),
    WTCP(0x7e18fbda, 0x15fc7daa), WTCP(0x7e0fbb22, 0x16314d8e), WTCP(0x7e06644c, 0x1666198d),
    WTCP(0x7dfcf759, 0x169ae19f), WTCP(0x7df3744b, 0x16cfa5b9), WTCP(0x7de9db23, 0x170465d2),
    WTCP(0x7de02be4, 0x173921e2), WTCP(0x7dd6668f, 0x176dd9de), WTCP(0x7dcc8b25, 0x17a28dbe),
    WTCP(0x7dc299a9, 0x17d73d79), WTCP(0x7db8921c, 0x180be904), WTCP(0x7dae747f, 0x18409058),
    WTCP(0x7da440d6, 0x1875336a), WTCP(0x7d99f721, 0x18a9d231), WTCP(0x7d8f9762, 0x18de6ca5),
    WTCP(0x7d85219c, 0x191302bc), WTCP(0x7d7a95cf, 0x1947946c), WTCP(0x7d6ff3fe, 0x197c21ad),
    WTCP(0x7d653c2b, 0x19b0aa75), WTCP(0x7d5a6e57, 0x19e52ebb), WTCP(0x7d4f8a85, 0x1a19ae76),
    WTCP(0x7d4490b6, 0x1a4e299d), WTCP(0x7d3980ec, 0x1a82a026), WTCP(0x7d2e5b2a, 0x1ab71208),
    WTCP(0x7d231f70, 0x1aeb7f3a), WTCP(0x7d17cdc2, 0x1b1fe7b3), WTCP(0x7d0c6621, 0x1b544b6a),
    WTCP(0x7d00e88f, 0x1b88aa55), WTCP(0x7cf5550e, 0x1bbd046c), WTCP(0x7ce9aba1, 0x1bf159a4),
    WTCP(0x7cddec48, 0x1c25a9f6), WTCP(0x7cd21707, 0x1c59f557), WTCP(0x7cc62bdf, 0x1c8e3bbe),
    WTCP(0x7cba2ad3, 0x1cc27d23), WTCP(0x7cae13e4, 0x1cf6b97c), WTCP(0x7ca1e715, 0x1d2af0c1),
    WTCP(0x7c95a467, 0x1d5f22e7), WTCP(0x7c894bde, 0x1d934fe5), WTCP(0x7c7cdd7b, 0x1dc777b3),
    WTCP(0x7c705940, 0x1dfb9a48), WTCP(0x7c63bf2f, 0x1e2fb79a), WTCP(0x7c570f4b, 0x1e63cfa0),
    WTCP(0x7c4a4996, 0x1e97e251), WTCP(0x7c3d6e13, 0x1ecbefa4), WTCP(0x7c307cc2, 0x1efff78f),
    WTCP(0x7c2375a8, 0x1f33fa0a), WTCP(0x7c1658c5, 0x1f67f70b), WTCP(0x7c09261d, 0x1f9bee8a),
    WTCP(0x7bfbddb1, 0x1fcfe07d), WTCP(0x7bee7f85, 0x2003ccdb), WTCP(0x7be10b99, 0x2037b39b),
    WTCP(0x7bd381f1, 0x206b94b4), WTCP(0x7bc5e290, 0x209f701c), WTCP(0x7bb82d76, 0x20d345cc),
    WTCP(0x7baa62a8, 0x210715b8), WTCP(0x7b9c8226, 0x213adfda), WTCP(0x7b8e8bf5, 0x216ea426),
    WTCP(0x7b808015, 0x21a26295), WTCP(0x7b725e8a, 0x21d61b1e), WTCP(0x7b642756, 0x2209cdb6),
    WTCP(0x7b55da7c, 0x223d7a55), WTCP(0x7b4777fe, 0x227120f3), WTCP(0x7b38ffde, 0x22a4c185),
    WTCP(0x7b2a721f, 0x22d85c04), WTCP(0x7b1bcec4, 0x230bf065), WTCP(0x7b0d15d0, 0x233f7ea0),
    WTCP(0x7afe4744, 0x237306ab), WTCP(0x7aef6323, 0x23a6887f), WTCP(0x7ae06971, 0x23da0411),
    WTCP(0x7ad15a2f, 0x240d7958), WTCP(0x7ac23561, 0x2440e84d), WTCP(0x7ab2fb09, 0x247450e4),
    WTCP(0x7aa3ab29, 0x24a7b317), WTCP(0x7a9445c5, 0x24db0edb), WTCP(0x7a84cade, 0x250e6427),
    WTCP(0x7a753a79, 0x2541b2f3), WTCP(0x7a659496, 0x2574fb36), WTCP(0x7a55d93a, 0x25a83ce6),
    WTCP(0x7a460867, 0x25db77fa), WTCP(0x7a362220, 0x260eac6a), WTCP(0x7a262668, 0x2641da2d),
    WTCP(0x7a161540, 0x26750139), WTCP(0x7a05eead, 0x26a82186), WTCP(0x79f5b2b1, 0x26db3b0a),
    WTCP(0x79e5614f, 0x270e4dbd), WTCP(0x79d4fa89, 0x27415996), WTCP(0x79c47e63, 0x27745e8c),
    WTCP(0x79b3ece0, 0x27a75c95), WTCP(0x79a34602, 0x27da53a9), WTCP(0x799289cc, 0x280d43bf),
    WTCP(0x7981b841, 0x28402cce), WTCP(0x7970d165, 0x28730ecd), WTCP(0x795fd53a, 0x28a5e9b4),
    WTCP(0x794ec3c3, 0x28d8bd78), WTCP(0x793d9d03, 0x290b8a12), WTCP(0x792c60fe, 0x293e4f78),
    WTCP(0x791b0fb5, 0x29710da1), WTCP(0x7909a92d, 0x29a3c485), WTCP(0x78f82d68, 0x29d6741b),
    WTCP(0x78e69c69, 0x2a091c59), WTCP(0x78d4f634, 0x2a3bbd37), WTCP(0x78c33acb, 0x2a6e56ac),
    WTCP(0x78b16a32, 0x2aa0e8b0), WTCP(0x789f846b, 0x2ad37338), WTCP(0x788d897b, 0x2b05f63d),
    WTCP(0x787b7963, 0x2b3871b5), WTCP(0x78695428, 0x2b6ae598), WTCP(0x785719cc, 0x2b9d51dd),
    WTCP(0x7844ca53, 0x2bcfb67b), WTCP(0x783265c0, 0x2c021369), WTCP(0x781fec15, 0x2c34689e),
    WTCP(0x780d5d57, 0x2c66b611), WTCP(0x77fab989, 0x2c98fbba), WTCP(0x77e800ad, 0x2ccb3990),
    WTCP(0x77d532c7, 0x2cfd6f8a), WTCP(0x77c24fdb, 0x2d2f9d9f), WTCP(0x77af57eb, 0x2d61c3c7),
    WTCP(0x779c4afc, 0x2d93e1f8), WTCP(0x77892910, 0x2dc5f829), WTCP(0x7775f22a, 0x2df80653),
    WTCP(0x7762a64f, 0x2e2a0c6c), WTCP(0x774f4581, 0x2e5c0a6b), WTCP(0x773bcfc4, 0x2e8e0048),
    WTCP(0x7728451c, 0x2ebfedfa), WTCP(0x7714a58b, 0x2ef1d377), WTCP(0x7700f115, 0x2f23b0b9),
    WTCP(0x76ed27be, 0x2f5585b5), WTCP(0x76d94989, 0x2f875262), WTCP(0x76c55679, 0x2fb916b9),
    WTCP(0x76b14e93, 0x2fead2b0), WTCP(0x769d31d9, 0x301c863f), WTCP(0x76890050, 0x304e315d),
    WTCP(0x7674b9fa, 0x307fd401), WTCP(0x76605edb, 0x30b16e23), WTCP(0x764beef8, 0x30e2ffb9),
    WTCP(0x76376a52, 0x311488bc), WTCP(0x7622d0ef, 0x31460922), WTCP(0x760e22d1, 0x317780e2),
    WTCP(0x75f95ffc, 0x31a8eff5), WTCP(0x75e48874, 0x31da5651), WTCP(0x75cf9c3d, 0x320bb3ee),
    WTCP(0x75ba9b5a, 0x323d08c3), WTCP(0x75a585cf, 0x326e54c7), WTCP(0x75905ba0, 0x329f97f3),
    WTCP(0x757b1ccf, 0x32d0d23c), WTCP(0x7565c962, 0x3302039b), WTCP(0x7550615c, 0x33332c06),
    WTCP(0x753ae4c0, 0x33644b76), WTCP(0x75255392, 0x339561e1), WTCP(0x750fadd7, 0x33c66f40),
    WTCP(0x74f9f391, 0x33f77388), WTCP(0x74e424c5, 0x34286eb3), WTCP(0x74ce4177, 0x345960b7),
    WTCP(0x74b849aa, 0x348a498b), WTCP(0x74a23d62, 0x34bb2927), WTCP(0x748c1ca4, 0x34ebff83),
    WTCP(0x7475e772, 0x351ccc96), WTCP(0x745f9dd1, 0x354d9057), WTCP(0x74493fc5, 0x357e4abe),
    WTCP(0x7432cd51, 0x35aefbc2), WTCP(0x741c467b, 0x35dfa35a), WTCP(0x7405ab45, 0x3610417f),
    WTCP(0x73eefbb3, 0x3640d627), WTCP(0x73d837ca, 0x3671614b), WTCP(0x73c15f8d, 0x36a1e2e0),
    WTCP(0x73aa7301, 0x36d25ae0), WTCP(0x7393722a, 0x3702c942), WTCP(0x737c5d0b, 0x37332dfd),
    WTCP(0x736533a9, 0x37638908), WTCP(0x734df607, 0x3793da5b), WTCP(0x7336a42b, 0x37c421ee),
    WTCP(0x731f3e17, 0x37f45fb7), WTCP(0x7307c3d0, 0x382493b0), WTCP(0x72f0355a, 0x3854bdcf),
    WTCP(0x72d892ba, 0x3884de0b), WTCP(0x72c0dbf3, 0x38b4f45d), WTCP(0x72a91109, 0x38e500bc),
    WTCP(0x72913201, 0x3915031f), WTCP(0x72793edf, 0x3944fb7e), WTCP(0x726137a8, 0x3974e9d0),
    WTCP(0x72491c5e, 0x39a4ce0e), WTCP(0x7230ed07, 0x39d4a82f), WTCP(0x7218a9a7, 0x3a04782a),
    WTCP(0x72005242, 0x3a343df7), WTCP(0x71e7e6dc, 0x3a63f98d), WTCP(0x71cf677a, 0x3a93aae5),
    WTCP(0x71b6d420, 0x3ac351f6), WTCP(0x719e2cd2, 0x3af2eeb7), WTCP(0x71857195, 0x3b228120),
    WTCP(0x716ca26c, 0x3b52092a), WTCP(0x7153bf5d, 0x3b8186ca), WTCP(0x713ac86b, 0x3bb0f9fa),
    WTCP(0x7121bd9c, 0x3be062b0), WTCP(0x71089ef2, 0x3c0fc0e6), WTCP(0x70ef6c74, 0x3c3f1491),
    WTCP(0x70d62625, 0x3c6e5daa), WTCP(0x70bccc09, 0x3c9d9c28), WTCP(0x70a35e25, 0x3cccd004),
    WTCP(0x7089dc7e, 0x3cfbf935), WTCP(0x70704718, 0x3d2b17b3), WTCP(0x70569df8, 0x3d5a2b75),
    WTCP(0x703ce122, 0x3d893474), WTCP(0x7023109a, 0x3db832a6), WTCP(0x70092c65, 0x3de72604),
    WTCP(0x6fef3488, 0x3e160e85), WTCP(0x6fd52907, 0x3e44ec22), WTCP(0x6fbb09e7, 0x3e73bed2),
    WTCP(0x6fa0d72c, 0x3ea2868c), WTCP(0x6f8690db, 0x3ed14349), WTCP(0x6f6c36f8, 0x3efff501),
    WTCP(0x6f51c989, 0x3f2e9bab), WTCP(0x6f374891, 0x3f5d373e), WTCP(0x6f1cb416, 0x3f8bc7b4),
    WTCP(0x6f020c1c, 0x3fba4d03), WTCP(0x6ee750a8, 0x3fe8c724), WTCP(0x6ecc81be, 0x4017360e),
    WTCP(0x6eb19f64, 0x404599b9), WTCP(0x6e96a99d, 0x4073f21d), WTCP(0x6e7ba06f, 0x40a23f32),
    WTCP(0x6e6083de, 0x40d080f0), WTCP(0x6e4553ef, 0x40feb74f), WTCP(0x6e2a10a8, 0x412ce246),
    WTCP(0x6e0eba0c, 0x415b01ce), WTCP(0x6df35020, 0x418915de), WTCP(0x6dd7d2ea, 0x41b71e6f),
    WTCP(0x6dbc426e, 0x41e51b77), WTCP(0x6da09eb1, 0x42130cf0), WTCP(0x6d84e7b7, 0x4240f2d1),
    WTCP(0x6d691d87, 0x426ecd12), WTCP(0x6d4d4023, 0x429c9bab), WTCP(0x6d314f93, 0x42ca5e94),
    WTCP(0x6d154bd9, 0x42f815c5), WTCP(0x6cf934fc, 0x4325c135), WTCP(0x6cdd0b00, 0x435360de),
    WTCP(0x6cc0cdea, 0x4380f4b7), WTCP(0x6ca47dbf, 0x43ae7cb7), WTCP(0x6c881a84, 0x43dbf8d7),
    WTCP(0x6c6ba43e, 0x44096910), WTCP(0x6c4f1af2, 0x4436cd58), WTCP(0x6c327ea6, 0x446425a8),
    WTCP(0x6c15cf5d, 0x449171f8), WTCP(0x6bf90d1d, 0x44beb240), WTCP(0x6bdc37eb, 0x44ebe679),
    WTCP(0x6bbf4fcd, 0x45190e99), WTCP(0x6ba254c7, 0x45462a9a), WTCP(0x6b8546de, 0x45733a73),
    WTCP(0x6b682617, 0x45a03e1d), WTCP(0x6b4af279, 0x45cd358f), WTCP(0x6b2dac06, 0x45fa20c2),
    WTCP(0x6b1052c6, 0x4626ffae), WTCP(0x6af2e6bc, 0x4653d24b), WTCP(0x6ad567ef, 0x46809891),
    WTCP(0x6ab7d663, 0x46ad5278), WTCP(0x6a9a321d, 0x46d9fff8), WTCP(0x6a7c7b23, 0x4706a10a),
    WTCP(0x6a5eb17a, 0x473335a5), WTCP(0x6a40d527, 0x475fbdc3), WTCP(0x6a22e630, 0x478c395a),
    WTCP(0x6a04e499, 0x47b8a864), WTCP(0x69e6d067, 0x47e50ad8), WTCP(0x69c8a9a1, 0x481160ae),
    WTCP(0x69aa704c, 0x483da9e0), WTCP(0x698c246c, 0x4869e665), WTCP(0x696dc607, 0x48961635),
    WTCP(0x694f5523, 0x48c23949), WTCP(0x6930d1c4, 0x48ee4f98), WTCP(0x69123bf1, 0x491a591c),
    WTCP(0x68f393ae, 0x494655cc), WTCP(0x68d4d900, 0x497245a1), WTCP(0x68b60bee, 0x499e2892),
    WTCP(0x68972c7d, 0x49c9fe99), WTCP(0x68783ab1, 0x49f5c7ae), WTCP(0x68593691, 0x4a2183c8),
    WTCP(0x683a2022, 0x4a4d32e1), WTCP(0x681af76a, 0x4a78d4f0), WTCP(0x67fbbc6d, 0x4aa469ee),
    WTCP(0x67dc6f31, 0x4acff1d3), WTCP(0x67bd0fbd, 0x4afb6c98), WTCP(0x679d9e14, 0x4b26da35),
    WTCP(0x677e1a3e, 0x4b523aa2), WTCP(0x675e843e, 0x4b7d8dd8), WTCP(0x673edc1c, 0x4ba8d3cf),
    WTCP(0x671f21dc, 0x4bd40c80), WTCP(0x66ff5584, 0x4bff37e2), WTCP(0x66df771a, 0x4c2a55ef),
    WTCP(0x66bf86a3, 0x4c55669f), WTCP(0x669f8425, 0x4c8069ea), WTCP(0x667f6fa5, 0x4cab5fc9),
    WTCP(0x665f4929, 0x4cd64834), WTCP(0x663f10b7, 0x4d012324), WTCP(0x661ec654, 0x4d2bf091),
    WTCP(0x65fe6a06, 0x4d56b073), WTCP(0x65ddfbd3, 0x4d8162c4), WTCP(0x65bd7bc0, 0x4dac077b),
    WTCP(0x659ce9d4, 0x4dd69e92), WTCP(0x657c4613, 0x4e012800), WTCP(0x655b9083, 0x4e2ba3be),
    WTCP(0x653ac92b, 0x4e5611c5), WTCP(0x6519f010, 0x4e80720e), WTCP(0x64f90538, 0x4eaac490),
    WTCP(0x64d808a8, 0x4ed50945), WTCP(0x64b6fa66, 0x4eff4025), WTCP(0x6495da79, 0x4f296928),
    WTCP(0x6474a8e5, 0x4f538448), WTCP(0x645365b2, 0x4f7d917c), WTCP(0x643210e4, 0x4fa790be),
    WTCP(0x6410aa81, 0x4fd18206), WTCP(0x63ef3290, 0x4ffb654d), WTCP(0x63cda916, 0x50253a8b),
    WTCP(0x63ac0e19, 0x504f01ba), WTCP(0x638a619e, 0x5078bad1), WTCP(0x6368a3ad, 0x50a265c9),
    WTCP(0x6346d44b, 0x50cc029c), WTCP(0x6324f37d, 0x50f59141), WTCP(0x6303014a, 0x511f11b2),
    WTCP(0x62e0fdb8, 0x514883e7), WTCP(0x62bee8cc, 0x5171e7d9), WTCP(0x629cc28c, 0x519b3d80),
    WTCP(0x627a8b00, 0x51c484d6), WTCP(0x6258422c, 0x51edbdd4), WTCP(0x6235e816, 0x5216e871),
    WTCP(0x62137cc5, 0x524004a7), WTCP(0x61f1003f, 0x5269126e), WTCP(0x61ce7289, 0x529211c0),
    WTCP(0x61abd3ab, 0x52bb0295), WTCP(0x618923a9, 0x52e3e4e6), WTCP(0x61666289, 0x530cb8ac),
    WTCP(0x61439053, 0x53357ddf), WTCP(0x6120ad0d, 0x535e3479), WTCP(0x60fdb8bb, 0x5386dc72),
    WTCP(0x60dab365, 0x53af75c3), WTCP(0x60b79d10, 0x53d80065), WTCP(0x609475c3, 0x54007c51),
    WTCP(0x60713d84, 0x5428e980), WTCP(0x604df459, 0x545147eb), WTCP(0x602a9a48, 0x5479978a),
    WTCP(0x60072f57, 0x54a1d857), WTCP(0x5fe3b38d, 0x54ca0a4b), WTCP(0x5fc026f0, 0x54f22d5d),
    WTCP(0x5f9c8987, 0x551a4189), WTCP(0x5f78db56, 0x554246c6), WTCP(0x5f551c65, 0x556a3d0d),
    WTCP(0x5f314cba, 0x55922457), WTCP(0x5f0d6c5b, 0x55b9fc9e), WTCP(0x5ee97b4f, 0x55e1c5da),
    WTCP(0x5ec5799b, 0x56098005), WTCP(0x5ea16747, 0x56312b17), WTCP(0x5e7d4458, 0x5658c709),
    WTCP(0x5e5910d4, 0x568053d5), WTCP(0x5e34ccc3, 0x56a7d174), WTCP(0x5e10782b, 0x56cf3fde),
    WTCP(0x5dec1311, 0x56f69f0d), WTCP(0x5dc79d7c, 0x571deefa), WTCP(0x5da31773, 0x57452f9d),
    WTCP(0x5d7e80fc, 0x576c60f1), WTCP(0x5d59da1e, 0x579382ee), WTCP(0x5d3522de, 0x57ba958d),
    WTCP(0x5d105b44, 0x57e198c7), WTCP(0x5ceb8355, 0x58088c96), WTCP(0x5cc69b19, 0x582f70f3),
    WTCP(0x5ca1a295, 0x585645d7), WTCP(0x5c7c99d1, 0x587d0b3b), WTCP(0x5c5780d3, 0x58a3c118),
    WTCP(0x5c3257a0, 0x58ca6767), WTCP(0x5c0d1e41, 0x58f0fe23), WTCP(0x5be7d4ba, 0x59178543),
    WTCP(0x5bc27b14, 0x593dfcc2), WTCP(0x5b9d1154, 0x59646498), WTCP(0x5b779780, 0x598abcbe),
    WTCP(0x5b520da1, 0x59b1052f), WTCP(0x5b2c73bb, 0x59d73de3), WTCP(0x5b06c9d6, 0x59fd66d4),
    WTCP(0x5ae10ff9, 0x5a237ffa), WTCP(0x5abb4629, 0x5a498950), WTCP(0x5a956c6e, 0x5a6f82ce),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow1024[] = {
    WTCP(0x7ffffd88, 0x001921fb), WTCP(0x7fffe9cb, 0x004b65ee), WTCP(0x7fffc251, 0x007da9d4),
    WTCP(0x7fff8719, 0x00afeda8), WTCP(0x7fff3824, 0x00e23160), WTCP(0x7ffed572, 0x011474f6),
    WTCP(0x7ffe5f03, 0x0146b860), WTCP(0x7ffdd4d7, 0x0178fb99), WTCP(0x7ffd36ee, 0x01ab3e97),
    WTCP(0x7ffc8549, 0x01dd8154), WTCP(0x7ffbbfe6, 0x020fc3c6), WTCP(0x7ffae6c7, 0x024205e8),
    WTCP(0x7ff9f9ec, 0x027447b0), WTCP(0x7ff8f954, 0x02a68917), WTCP(0x7ff7e500, 0x02d8ca16),
    WTCP(0x7ff6bcf0, 0x030b0aa4), WTCP(0x7ff58125, 0x033d4abb), WTCP(0x7ff4319d, 0x036f8a51),
    WTCP(0x7ff2ce5b, 0x03a1c960), WTCP(0x7ff1575d, 0x03d407df), WTCP(0x7fefcca4, 0x040645c7),
    WTCP(0x7fee2e30, 0x04388310), WTCP(0x7fec7c02, 0x046abfb3), WTCP(0x7feab61a, 0x049cfba7),
    WTCP(0x7fe8dc78, 0x04cf36e5), WTCP(0x7fe6ef1c, 0x05017165), WTCP(0x7fe4ee06, 0x0533ab20),
    WTCP(0x7fe2d938, 0x0565e40d), WTCP(0x7fe0b0b1, 0x05981c26), WTCP(0x7fde7471, 0x05ca5361),
    WTCP(0x7fdc247a, 0x05fc89b8), WTCP(0x7fd9c0ca, 0x062ebf22), WTCP(0x7fd74964, 0x0660f398),
    WTCP(0x7fd4be46, 0x06932713), WTCP(0x7fd21f72, 0x06c5598a), WTCP(0x7fcf6ce8, 0x06f78af6),
    WTCP(0x7fcca6a7, 0x0729bb4e), WTCP(0x7fc9ccb2, 0x075bea8c), WTCP(0x7fc6df08, 0x078e18a7),
    WTCP(0x7fc3dda9, 0x07c04598), WTCP(0x7fc0c896, 0x07f27157), WTCP(0x7fbd9fd0, 0x08249bdd),
    WTCP(0x7fba6357, 0x0856c520), WTCP(0x7fb7132b, 0x0888ed1b), WTCP(0x7fb3af4e, 0x08bb13c5),
    WTCP(0x7fb037bf, 0x08ed3916), WTCP(0x7facac7f, 0x091f5d06), WTCP(0x7fa90d8e, 0x09517f8f),
    WTCP(0x7fa55aee, 0x0983a0a7), WTCP(0x7fa1949e, 0x09b5c048), WTCP(0x7f9dbaa0, 0x09e7de6a),
    WTCP(0x7f99ccf4, 0x0a19fb04), WTCP(0x7f95cb9a, 0x0a4c1610), WTCP(0x7f91b694, 0x0a7e2f85),
    WTCP(0x7f8d8de1, 0x0ab0475c), WTCP(0x7f895182, 0x0ae25d8d), WTCP(0x7f850179, 0x0b147211),
    WTCP(0x7f809dc5, 0x0b4684df), WTCP(0x7f7c2668, 0x0b7895f0), WTCP(0x7f779b62, 0x0baaa53b),
    WTCP(0x7f72fcb4, 0x0bdcb2bb), WTCP(0x7f6e4a5e, 0x0c0ebe66), WTCP(0x7f698461, 0x0c40c835),
    WTCP(0x7f64aabf, 0x0c72d020), WTCP(0x7f5fbd77, 0x0ca4d620), WTCP(0x7f5abc8a, 0x0cd6da2d),
    WTCP(0x7f55a7fa, 0x0d08dc3f), WTCP(0x7f507fc7, 0x0d3adc4e), WTCP(0x7f4b43f2, 0x0d6cda53),
    WTCP(0x7f45f47b, 0x0d9ed646), WTCP(0x7f409164, 0x0dd0d01f), WTCP(0x7f3b1aad, 0x0e02c7d7),
    WTCP(0x7f359057, 0x0e34bd66), WTCP(0x7f2ff263, 0x0e66b0c3), WTCP(0x7f2a40d2, 0x0e98a1e9),
    WTCP(0x7f247ba5, 0x0eca90ce), WTCP(0x7f1ea2dc, 0x0efc7d6b), WTCP(0x7f18b679, 0x0f2e67b8),
    WTCP(0x7f12b67c, 0x0f604faf), WTCP(0x7f0ca2e7, 0x0f923546), WTCP(0x7f067bba, 0x0fc41876),
    WTCP(0x7f0040f6, 0x0ff5f938), WTCP(0x7ef9f29d, 0x1027d784), WTCP(0x7ef390ae, 0x1059b352),
    WTCP(0x7eed1b2c, 0x108b8c9b), WTCP(0x7ee69217, 0x10bd6356), WTCP(0x7edff570, 0x10ef377d),
    WTCP(0x7ed94538, 0x11210907), WTCP(0x7ed28171, 0x1152d7ed), WTCP(0x7ecbaa1a, 0x1184a427),
    WTCP(0x7ec4bf36, 0x11b66dad), WTCP(0x7ebdc0c6, 0x11e83478), WTCP(0x7eb6aeca, 0x1219f880),
    WTCP(0x7eaf8943, 0x124bb9be), WTCP(0x7ea85033, 0x127d7829), WTCP(0x7ea1039b, 0x12af33ba),
    WTCP(0x7e99a37c, 0x12e0ec6a), WTCP(0x7e922fd6, 0x1312a230), WTCP(0x7e8aa8ac, 0x13445505),
    WTCP(0x7e830dff, 0x137604e2), WTCP(0x7e7b5fce, 0x13a7b1bf), WTCP(0x7e739e1d, 0x13d95b93),
    WTCP(0x7e6bc8eb, 0x140b0258), WTCP(0x7e63e03b, 0x143ca605), WTCP(0x7e5be40c, 0x146e4694),
    WTCP(0x7e53d462, 0x149fe3fc), WTCP(0x7e4bb13c, 0x14d17e36), WTCP(0x7e437a9c, 0x1503153a),
    WTCP(0x7e3b3083, 0x1534a901), WTCP(0x7e32d2f4, 0x15663982), WTCP(0x7e2a61ed, 0x1597c6b7),
    WTCP(0x7e21dd73, 0x15c95097), WTCP(0x7e194584, 0x15fad71b), WTCP(0x7e109a24, 0x162c5a3b),
    WTCP(0x7e07db52, 0x165dd9f0), WTCP(0x7dff0911, 0x168f5632), WTCP(0x7df62362, 0x16c0cef9),
    WTCP(0x7ded2a47, 0x16f2443e), WTCP(0x7de41dc0, 0x1723b5f9), WTCP(0x7ddafdce, 0x17552422),
    WTCP(0x7dd1ca75, 0x17868eb3), WTCP(0x7dc883b4, 0x17b7f5a3), WTCP(0x7dbf298d, 0x17e958ea),
    WTCP(0x7db5bc02, 0x181ab881), WTCP(0x7dac3b15, 0x184c1461), WTCP(0x7da2a6c6, 0x187d6c82),
    WTCP(0x7d98ff17, 0x18aec0db), WTCP(0x7d8f4409, 0x18e01167), WTCP(0x7d85759f, 0x19115e1c),
    WTCP(0x7d7b93da, 0x1942a6f3), WTCP(0x7d719eba, 0x1973ebe6), WTCP(0x7d679642, 0x19a52ceb),
    WTCP(0x7d5d7a74, 0x19d669fc), WTCP(0x7d534b50, 0x1a07a311), WTCP(0x7d4908d9, 0x1a38d823),
    WTCP(0x7d3eb30f, 0x1a6a0929), WTCP(0x7d3449f5, 0x1a9b361d), WTCP(0x7d29cd8c, 0x1acc5ef6),
    WTCP(0x7d1f3dd6, 0x1afd83ad), WTCP(0x7d149ad5, 0x1b2ea43a), WTCP(0x7d09e489, 0x1b5fc097),
    WTCP(0x7cff1af5, 0x1b90d8bb), WTCP(0x7cf43e1a, 0x1bc1ec9e), WTCP(0x7ce94dfb, 0x1bf2fc3a),
    WTCP(0x7cde4a98, 0x1c240786), WTCP(0x7cd333f3, 0x1c550e7c), WTCP(0x7cc80a0f, 0x1c861113),
    WTCP(0x7cbcccec, 0x1cb70f43), WTCP(0x7cb17c8d, 0x1ce80906), WTCP(0x7ca618f3, 0x1d18fe54),
    WTCP(0x7c9aa221, 0x1d49ef26), WTCP(0x7c8f1817, 0x1d7adb73), WTCP(0x7c837ad8, 0x1dabc334),
    WTCP(0x7c77ca65, 0x1ddca662), WTCP(0x7c6c06c0, 0x1e0d84f5), WTCP(0x7c602fec, 0x1e3e5ee5),
    WTCP(0x7c5445e9, 0x1e6f342c), WTCP(0x7c4848ba, 0x1ea004c1), WTCP(0x7c3c3860, 0x1ed0d09d),
    WTCP(0x7c3014de, 0x1f0197b8), WTCP(0x7c23de35, 0x1f325a0b), WTCP(0x7c179467, 0x1f63178f),
    WTCP(0x7c0b3777, 0x1f93d03c), WTCP(0x7bfec765, 0x1fc4840a), WTCP(0x7bf24434, 0x1ff532f2),
    WTCP(0x7be5ade6, 0x2025dcec), WTCP(0x7bd9047c, 0x205681f1), WTCP(0x7bcc47fa, 0x208721f9),
    WTCP(0x7bbf7860, 0x20b7bcfe), WTCP(0x7bb295b0, 0x20e852f6), WTCP(0x7ba59fee, 0x2118e3dc),
    WTCP(0x7b989719, 0x21496fa7), WTCP(0x7b8b7b36, 0x2179f64f), WTCP(0x7b7e4c45, 0x21aa77cf),
    WTCP(0x7b710a49, 0x21daf41d), WTCP(0x7b63b543, 0x220b6b32), WTCP(0x7b564d36, 0x223bdd08),
    WTCP(0x7b48d225, 0x226c4996), WTCP(0x7b3b4410, 0x229cb0d5), WTCP(0x7b2da2fa, 0x22cd12bd),
    WTCP(0x7b1feee5, 0x22fd6f48), WTCP(0x7b1227d3, 0x232dc66d), WTCP(0x7b044dc7, 0x235e1826),
    WTCP(0x7af660c2, 0x238e646a), WTCP(0x7ae860c7, 0x23beab33), WTCP(0x7ada4dd8, 0x23eeec78),
    WTCP(0x7acc27f7, 0x241f2833), WTCP(0x7abdef25, 0x244f5e5c), WTCP(0x7aafa367, 0x247f8eec),
    WTCP(0x7aa144bc, 0x24afb9da), WTCP(0x7a92d329, 0x24dfdf20), WTCP(0x7a844eae, 0x250ffeb7),
    WTCP(0x7a75b74f, 0x25401896), WTCP(0x7a670d0d, 0x25702cb7), WTCP(0x7a584feb, 0x25a03b11),
    WTCP(0x7a497feb, 0x25d0439f), WTCP(0x7a3a9d0f, 0x26004657), WTCP(0x7a2ba75a, 0x26304333),
    WTCP(0x7a1c9ece, 0x26603a2c), WTCP(0x7a0d836d, 0x26902b39), WTCP(0x79fe5539, 0x26c01655),
    WTCP(0x79ef1436, 0x26effb76), WTCP(0x79dfc064, 0x271fda96), WTCP(0x79d059c8, 0x274fb3ae),
    WTCP(0x79c0e062, 0x277f86b5), WTCP(0x79b15435, 0x27af53a6), WTCP(0x79a1b545, 0x27df1a77),
    WTCP(0x79920392, 0x280edb23), WTCP(0x79823f20, 0x283e95a1), WTCP(0x797267f2, 0x286e49ea),
    WTCP(0x79627e08, 0x289df7f8), WTCP(0x79528167, 0x28cd9fc1), WTCP(0x79427210, 0x28fd4140),
    WTCP(0x79325006, 0x292cdc6d), WTCP(0x79221b4b, 0x295c7140), WTCP(0x7911d3e2, 0x298bffb2),
    WTCP(0x790179cd, 0x29bb87bc), WTCP(0x78f10d0f, 0x29eb0957), WTCP(0x78e08dab, 0x2a1a847b),
    WTCP(0x78cffba3, 0x2a49f920), WTCP(0x78bf56f9, 0x2a796740), WTCP(0x78ae9fb0, 0x2aa8ced3),
    WTCP(0x789dd5cb, 0x2ad82fd2), WTCP(0x788cf94c, 0x2b078a36), WTCP(0x787c0a36, 0x2b36ddf7),
    WTCP(0x786b088c, 0x2b662b0e), WTCP(0x7859f44f, 0x2b957173), WTCP(0x7848cd83, 0x2bc4b120),
    WTCP(0x7837942b, 0x2bf3ea0d), WTCP(0x78264849, 0x2c231c33), WTCP(0x7814e9df, 0x2c52478a),
    WTCP(0x780378f1, 0x2c816c0c), WTCP(0x77f1f581, 0x2cb089b1), WTCP(0x77e05f91, 0x2cdfa071),
    WTCP(0x77ceb725, 0x2d0eb046), WTCP(0x77bcfc3f, 0x2d3db928), WTCP(0x77ab2ee2, 0x2d6cbb10),
    WTCP(0x77994f11, 0x2d9bb5f6), WTCP(0x77875cce, 0x2dcaa9d5), WTCP(0x7775581d, 0x2df996a3),
    WTCP(0x776340ff, 0x2e287c5a), WTCP(0x77511778, 0x2e575af3), WTCP(0x773edb8b, 0x2e863267),
    WTCP(0x772c8d3a, 0x2eb502ae), WTCP(0x771a2c88, 0x2ee3cbc1), WTCP(0x7707b979, 0x2f128d99),
    WTCP(0x76f5340e, 0x2f41482e), WTCP(0x76e29c4b, 0x2f6ffb7a), WTCP(0x76cff232, 0x2f9ea775),
    WTCP(0x76bd35c7, 0x2fcd4c19), WTCP(0x76aa670d, 0x2ffbe95d), WTCP(0x76978605, 0x302a7f3a),
    WTCP(0x768492b4, 0x30590dab), WTCP(0x76718d1c, 0x308794a6), WTCP(0x765e7540, 0x30b61426),
    WTCP(0x764b4b23, 0x30e48c22), WTCP(0x76380ec8, 0x3112fc95), WTCP(0x7624c031, 0x31416576),
    WTCP(0x76115f63, 0x316fc6be), WTCP(0x75fdec60, 0x319e2067), WTCP(0x75ea672a, 0x31cc7269),
    WTCP(0x75d6cfc5, 0x31fabcbd), WTCP(0x75c32634, 0x3228ff5c), WTCP(0x75af6a7b, 0x32573a3f),
    WTCP(0x759b9c9b, 0x32856d5e), WTCP(0x7587bc98, 0x32b398b3), WTCP(0x7573ca75, 0x32e1bc36),
    WTCP(0x755fc635, 0x330fd7e1), WTCP(0x754bafdc, 0x333debab), WTCP(0x7537876c, 0x336bf78f),
    WTCP(0x75234ce8, 0x3399fb85), WTCP(0x750f0054, 0x33c7f785), WTCP(0x74faa1b3, 0x33f5eb89),
    WTCP(0x74e63108, 0x3423d78a), WTCP(0x74d1ae55, 0x3451bb81), WTCP(0x74bd199f, 0x347f9766),
    WTCP(0x74a872e8, 0x34ad6b32), WTCP(0x7493ba34, 0x34db36df), WTCP(0x747eef85, 0x3508fa66),
    WTCP(0x746a12df, 0x3536b5be), WTCP(0x74552446, 0x356468e2), WTCP(0x744023bc, 0x359213c9),
    WTCP(0x742b1144, 0x35bfb66e), WTCP(0x7415ece2, 0x35ed50c9), WTCP(0x7400b69a, 0x361ae2d3),
    WTCP(0x73eb6e6e, 0x36486c86), WTCP(0x73d61461, 0x3675edd9), WTCP(0x73c0a878, 0x36a366c6),
    WTCP(0x73ab2ab4, 0x36d0d746), WTCP(0x73959b1b, 0x36fe3f52), WTCP(0x737ff9ae, 0x372b9ee3),
    WTCP(0x736a4671, 0x3758f5f2), WTCP(0x73548168, 0x37864477), WTCP(0x733eaa96, 0x37b38a6d),
    WTCP(0x7328c1ff, 0x37e0c7cc), WTCP(0x7312c7a5, 0x380dfc8d), WTCP(0x72fcbb8c, 0x383b28a9),
    WTCP(0x72e69db7, 0x38684c19), WTCP(0x72d06e2b, 0x389566d6), WTCP(0x72ba2cea, 0x38c278d9),
    WTCP(0x72a3d9f7, 0x38ef821c), WTCP(0x728d7557, 0x391c8297), WTCP(0x7276ff0d, 0x39497a43),
    WTCP(0x7260771b, 0x39766919), WTCP(0x7249dd86, 0x39a34f13), WTCP(0x72333251, 0x39d02c2a),
    WTCP(0x721c7580, 0x39fd0056), WTCP(0x7205a716, 0x3a29cb91), WTCP(0x71eec716, 0x3a568dd4),
    WTCP(0x71d7d585, 0x3a834717), WTCP(0x71c0d265, 0x3aaff755), WTCP(0x71a9bdba, 0x3adc9e86),
    WTCP(0x71929789, 0x3b093ca3), WTCP(0x717b5fd3, 0x3b35d1a5), WTCP(0x7164169d, 0x3b625d86),
    WTCP(0x714cbbeb, 0x3b8ee03e), WTCP(0x71354fc0, 0x3bbb59c7), WTCP(0x711dd220, 0x3be7ca1a),
    WTCP(0x7106430e, 0x3c143130), WTCP(0x70eea28e, 0x3c408f03), WTCP(0x70d6f0a4, 0x3c6ce38a),
    WTCP(0x70bf2d53, 0x3c992ec0), WTCP(0x70a7589f, 0x3cc5709e), WTCP(0x708f728b, 0x3cf1a91c),
    WTCP(0x70777b1c, 0x3d1dd835), WTCP(0x705f7255, 0x3d49fde1), WTCP(0x70475839, 0x3d761a19),
    WTCP(0x702f2ccd, 0x3da22cd7), WTCP(0x7016f014, 0x3dce3614), WTCP(0x6ffea212, 0x3dfa35c8),
    WTCP(0x6fe642ca, 0x3e262bee), WTCP(0x6fcdd241, 0x3e52187f), WTCP(0x6fb5507a, 0x3e7dfb73),
    WTCP(0x6f9cbd79, 0x3ea9d4c3), WTCP(0x6f841942, 0x3ed5a46b), WTCP(0x6f6b63d8, 0x3f016a61),
    WTCP(0x6f529d40, 0x3f2d26a0), WTCP(0x6f39c57d, 0x3f58d921), WTCP(0x6f20dc92, 0x3f8481dd),
    WTCP(0x6f07e285, 0x3fb020ce), WTCP(0x6eeed758, 0x3fdbb5ec), WTCP(0x6ed5bb10, 0x40074132),
    WTCP(0x6ebc8db0, 0x4032c297), WTCP(0x6ea34f3d, 0x405e3a16), WTCP(0x6e89ffb9, 0x4089a7a8),
    WTCP(0x6e709f2a, 0x40b50b46), WTCP(0x6e572d93, 0x40e064ea), WTCP(0x6e3daaf8, 0x410bb48c),
    WTCP(0x6e24175c, 0x4136fa27), WTCP(0x6e0a72c5, 0x416235b2), WTCP(0x6df0bd35, 0x418d6729),
    WTCP(0x6dd6f6b1, 0x41b88e84), WTCP(0x6dbd1f3c, 0x41e3abbc), WTCP(0x6da336dc, 0x420ebecb),
    WTCP(0x6d893d93, 0x4239c7aa), WTCP(0x6d6f3365, 0x4264c653), WTCP(0x6d551858, 0x428fbabe),
    WTCP(0x6d3aec6e, 0x42baa4e6), WTCP(0x6d20afac, 0x42e584c3), WTCP(0x6d066215, 0x43105a50),
    WTCP(0x6cec03af, 0x433b2585), WTCP(0x6cd1947c, 0x4365e65b), WTCP(0x6cb71482, 0x43909ccd),
    WTCP(0x6c9c83c3, 0x43bb48d4), WTCP(0x6c81e245, 0x43e5ea68), WTCP(0x6c67300b, 0x44108184),
    WTCP(0x6c4c6d1a, 0x443b0e21), WTCP(0x6c319975, 0x44659039), WTCP(0x6c16b521, 0x449007c4),
    WTCP(0x6bfbc021, 0x44ba74bd), WTCP(0x6be0ba7b, 0x44e4d71c), WTCP(0x6bc5a431, 0x450f2edb),
    WTCP(0x6baa7d49, 0x45397bf4), WTCP(0x6b8f45c7, 0x4563be60), WTCP(0x6b73fdae, 0x458df619),
    WTCP(0x6b58a503, 0x45b82318), WTCP(0x6b3d3bcb, 0x45e24556), WTCP(0x6b21c208, 0x460c5cce),
    WTCP(0x6b0637c1, 0x46366978), WTCP(0x6aea9cf8, 0x46606b4e), WTCP(0x6acef1b2, 0x468a624a),
    WTCP(0x6ab335f4, 0x46b44e65), WTCP(0x6a9769c1, 0x46de2f99), WTCP(0x6a7b8d1e, 0x470805df),
    WTCP(0x6a5fa010, 0x4731d131), WTCP(0x6a43a29a, 0x475b9188), WTCP(0x6a2794c1, 0x478546de),
    WTCP(0x6a0b7689, 0x47aef12c), WTCP(0x69ef47f6, 0x47d8906d), WTCP(0x69d3090e, 0x48022499),
    WTCP(0x69b6b9d3, 0x482badab), WTCP(0x699a5a4c, 0x48552b9b), WTCP(0x697dea7b, 0x487e9e64),
    WTCP(0x69616a65, 0x48a805ff), WTCP(0x6944da10, 0x48d16265), WTCP(0x6928397e, 0x48fab391),
    WTCP(0x690b88b5, 0x4923f97b), WTCP(0x68eec7b9, 0x494d341e), WTCP(0x68d1f68f, 0x49766373),
    WTCP(0x68b5153a, 0x499f8774), WTCP(0x689823bf, 0x49c8a01b), WTCP(0x687b2224, 0x49f1ad61),
    WTCP(0x685e106c, 0x4a1aaf3f), WTCP(0x6840ee9b, 0x4a43a5b0), WTCP(0x6823bcb7, 0x4a6c90ad),
    WTCP(0x68067ac3, 0x4a957030), WTCP(0x67e928c5, 0x4abe4433), WTCP(0x67cbc6c0, 0x4ae70caf),
    WTCP(0x67ae54ba, 0x4b0fc99d), WTCP(0x6790d2b6, 0x4b387af9), WTCP(0x677340ba, 0x4b6120bb),
    WTCP(0x67559eca, 0x4b89badd), WTCP(0x6737ecea, 0x4bb24958), WTCP(0x671a2b20, 0x4bdacc28),
    WTCP(0x66fc596f, 0x4c034345), WTCP(0x66de77dc, 0x4c2baea9), WTCP(0x66c0866d, 0x4c540e4e),
    WTCP(0x66a28524, 0x4c7c622d), WTCP(0x66847408, 0x4ca4aa41), WTCP(0x6666531d, 0x4ccce684),
    WTCP(0x66482267, 0x4cf516ee), WTCP(0x6629e1ec, 0x4d1d3b7a), WTCP(0x660b91af, 0x4d455422),
    WTCP(0x65ed31b5, 0x4d6d60df), WTCP(0x65cec204, 0x4d9561ac), WTCP(0x65b0429f, 0x4dbd5682),
    WTCP(0x6591b38c, 0x4de53f5a), WTCP(0x657314cf, 0x4e0d1c30), WTCP(0x6554666d, 0x4e34ecfc),
    WTCP(0x6535a86b, 0x4e5cb1b9), WTCP(0x6516dacd, 0x4e846a60), WTCP(0x64f7fd98, 0x4eac16eb),
    WTCP(0x64d910d1, 0x4ed3b755), WTCP(0x64ba147d, 0x4efb4b96), WTCP(0x649b08a0, 0x4f22d3aa),
    WTCP(0x647bed3f, 0x4f4a4f89), WTCP(0x645cc260, 0x4f71bf2e), WTCP(0x643d8806, 0x4f992293),
    WTCP(0x641e3e38, 0x4fc079b1), WTCP(0x63fee4f8, 0x4fe7c483), WTCP(0x63df7c4d, 0x500f0302),
    WTCP(0x63c0043b, 0x50363529), WTCP(0x63a07cc7, 0x505d5af1), WTCP(0x6380e5f6, 0x50847454),
    WTCP(0x63613fcd, 0x50ab814d), WTCP(0x63418a50, 0x50d281d5), WTCP(0x6321c585, 0x50f975e6),
    WTCP(0x6301f171, 0x51205d7b), WTCP(0x62e20e17, 0x5147388c), WTCP(0x62c21b7e, 0x516e0715),
    WTCP(0x62a219aa, 0x5194c910), WTCP(0x628208a1, 0x51bb7e75), WTCP(0x6261e866, 0x51e22740),
    WTCP(0x6241b8ff, 0x5208c36a), WTCP(0x62217a72, 0x522f52ee), WTCP(0x62012cc2, 0x5255d5c5),
    WTCP(0x61e0cff5, 0x527c4bea), WTCP(0x61c06410, 0x52a2b556), WTCP(0x619fe918, 0x52c91204),
    WTCP(0x617f5f12, 0x52ef61ee), WTCP(0x615ec603, 0x5315a50e), WTCP(0x613e1df0, 0x533bdb5d),
    WTCP(0x611d66de, 0x536204d7), WTCP(0x60fca0d2, 0x53882175), WTCP(0x60dbcbd1, 0x53ae3131),
    WTCP(0x60bae7e1, 0x53d43406), WTCP(0x6099f505, 0x53fa29ed), WTCP(0x6078f344, 0x542012e1),
    WTCP(0x6057e2a2, 0x5445eedb), WTCP(0x6036c325, 0x546bbdd7), WTCP(0x601594d1, 0x54917fce),
    WTCP(0x5ff457ad, 0x54b734ba), WTCP(0x5fd30bbc, 0x54dcdc96), WTCP(0x5fb1b104, 0x5502775c),
    WTCP(0x5f90478a, 0x55280505), WTCP(0x5f6ecf53, 0x554d858d), WTCP(0x5f4d4865, 0x5572f8ed),
    WTCP(0x5f2bb2c5, 0x55985f20), WTCP(0x5f0a0e77, 0x55bdb81f), WTCP(0x5ee85b82, 0x55e303e6),
    WTCP(0x5ec699e9, 0x5608426e), WTCP(0x5ea4c9b3, 0x562d73b2), WTCP(0x5e82eae5, 0x565297ab),
    WTCP(0x5e60fd84, 0x5677ae54), WTCP(0x5e3f0194, 0x569cb7a8), WTCP(0x5e1cf71c, 0x56c1b3a1),
    WTCP(0x5dfade20, 0x56e6a239), WTCP(0x5dd8b6a7, 0x570b8369), WTCP(0x5db680b4, 0x5730572e),
    WTCP(0x5d943c4e, 0x57551d80), WTCP(0x5d71e979, 0x5779d65b), WTCP(0x5d4f883b, 0x579e81b8),
    WTCP(0x5d2d189a, 0x57c31f92), WTCP(0x5d0a9a9a, 0x57e7afe4), WTCP(0x5ce80e41, 0x580c32a7),
    WTCP(0x5cc57394, 0x5830a7d6), WTCP(0x5ca2ca99, 0x58550f6c), WTCP(0x5c801354, 0x58796962),
    WTCP(0x5c5d4dcc, 0x589db5b3), WTCP(0x5c3a7a05, 0x58c1f45b), WTCP(0x5c179806, 0x58e62552),
    WTCP(0x5bf4a7d2, 0x590a4893), WTCP(0x5bd1a971, 0x592e5e19), WTCP(0x5bae9ce7, 0x595265df),
    WTCP(0x5b8b8239, 0x59765fde), WTCP(0x5b68596d, 0x599a4c12), WTCP(0x5b452288, 0x59be2a74),
    WTCP(0x5b21dd90, 0x59e1faff), WTCP(0x5afe8a8b, 0x5a05bdae), WTCP(0x5adb297d, 0x5a29727b),
    WTCP(0x5ab7ba6c, 0x5a4d1960), WTCP(0x5a943d5e, 0x5a70b258),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow96[] = {
    WTCP(0x7ffffffd, 0x0001a838), WTCP(0x7fffffe2, 0x00056e83), WTCP(0x7fffff79, 0x000b9fda),
    WTCP(0x7ffffe45, 0x00150e8e), WTCP(0x7ffffb4d, 0x0022aeeb), WTCP(0x7ffff4c6, 0x00359b36),
    WTCP(0x7fffe792, 0x004f14ff), WTCP(0x7fffce8b, 0x0070858c), WTCP(0x7fffa18f, 0x009b7d75),
    WTCP(0x7fff5439, 0x00d1b353), WTCP(0x7ffed442, 0x0115018f), WTCP(0x7ffe0775, 0x01676335),
    WTCP(0x7ffcc937, 0x01caefcb), WTCP(0x7ffae79f, 0x0241d62e), WTCP(0x7ff82019, 0x02ce567f),
    WTCP(0x7ff41ba4, 0x0372bb25), WTCP(0x7fee6ac3, 0x043150fc), WTCP(0x7fe68129, 0x050c5ec8),
    WTCP(0x7fdbb164, 0x06061c0f), WTCP(0x7fcd2894, 0x0720a779), WTCP(0x7fb9ea80, 0x085dfce2),
    WTCP(0x7fa0ce2e, 0x09bfeb4d), WTCP(0x7f807b45, 0x0b480ae2), WTCP(0x7f576880, 0x0cf7b339),
    WTCP(0x7f23db4e, 0x0ecff212), WTCP(0x7ee3e8ee, 0x10d182c0), WTCP(0x7e95791f, 0x12fcc670),
    WTCP(0x7e364a74, 0x1551bd88), WTCP(0x7dc3f864, 0x17d00238), WTCP(0x7d3c02fd, 0x1a76c47e),
    WTCP(0x7c9bd82a, 0x1d44c7ad), WTCP(0x7be0de56, 0x203861a1), WTCP(0x7b08803d, 0x234f7ba6),
    WTCP(0x7a103993, 0x26879530), WTCP(0x78f5a442, 0x29ddc854), WTCP(0x77b685de, 0x2d4ed00f),
    WTCP(0x7650dcf5, 0x30d7103d), WTCP(0x74c2ede4, 0x34729f2d), WTCP(0x730b4edb, 0x381d50ad),
    WTCP(0x7128f2c1, 0x3bd2c273), WTCP(0x6f1b32a9, 0x3f8e698f), WTCP(0x6ce1d5a0, 0x434ba0d6),
    WTCP(0x6a7d16a3, 0x4705b7e5), WTCP(0x67eda890, 0x4ab80288), WTCP(0x6534b7f8, 0x4e5de842),
    WTCP(0x6253eacd, 0x51f2f39a), WTCP(0x5f4d5de1, 0x5572e0f7), WTCP(0x5c23a04a, 0x58d9acb9),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow120[] = {
    WTCP(0x7ffffffe, 0x00017b6f), WTCP(0x7fffffef, 0x00042d2f), WTCP(0x7fffffbb, 0x000849d0),
    WTCP(0x7fffff36, 0x000e3494), WTCP(0x7ffffe0c, 0x00165efd), WTCP(0x7ffffbac, 0x002149be),
    WTCP(0x7ffff72e, 0x002f854c), WTCP(0x7fffef24, 0x0041b235), WTCP(0x7fffe167, 0x0058814f),
    WTCP(0x7fffcacd, 0x0074b3af), WTCP(0x7fffa6d0, 0x00971a67), WTCP(0x7fff6f1e, 0x00c0960e),
    WTCP(0x7fff1b12, 0x00f21602), WTCP(0x7ffe9f0b, 0x012c9775), WTCP(0x7ffdebb2, 0x01712428),
    WTCP(0x7ffced1b, 0x01c0d0f7), WTCP(0x7ffb89c2, 0x021cbc12), WTCP(0x7ff9a17c, 0x02860b05),
    WTCP(0x7ff70c39, 0x02fde875), WTCP(0x7ff398bc, 0x038581b3), WTCP(0x7fef0b3b, 0x041e040c),
    WTCP(0x7fe91bf3, 0x04c899f4), WTCP(0x7fe175ba, 0x05866803), WTCP(0x7fd7b493, 0x065889d5),
    WTCP(0x7fcb6459, 0x07400ed4), WTCP(0x7fbbff82, 0x083df6e9), WTCP(0x7fa8ee09, 0x09532f37),
    WTCP(0x7f91849a, 0x0a808ed1), WTCP(0x7f7503f2, 0x0bc6d381), WTCP(0x7f52989a, 0x0d269eb0),
    WTCP(0x7f295af4, 0x0ea07270), WTCP(0x7ef84fb6, 0x1034aeb6), WTCP(0x7ebe68c5, 0x11e38ed2),
    WTCP(0x7e7a8686, 0x13ad2733), WTCP(0x7e2b79a3, 0x1591636d), WTCP(0x7dd0053c, 0x179004a7),
    WTCP(0x7d66e18b, 0x19a8a05f), WTCP(0x7ceebef0, 0x1bda9fa2), WTCP(0x7c664953, 0x1e253ea1),
    WTCP(0x7bcc2be8, 0x20878cce), WTCP(0x7b1f1526, 0x23006d5d), WTCP(0x7a5dbb01, 0x258e9848),
    WTCP(0x7986df3e, 0x28309bc6), WTCP(0x789953e0, 0x2ae4de3e), WTCP(0x7793ff88, 0x2da9a0a8),
    WTCP(0x7675e1cc, 0x307d0163), WTCP(0x753e1763, 0x335cff72), WTCP(0x73ebde10, 0x36477e1f),
    WTCP(0x727e984e, 0x393a48f1), WTCP(0x70f5d09b, 0x3c3317f9), WTCP(0x6f513c60, 0x3f2f945c),
    WTCP(0x6d90be61, 0x422d5d18), WTCP(0x6bb468b1, 0x452a0bf3), WTCP(0x69bc7e1e, 0x48233a81),
    WTCP(0x67a97317, 0x4b16873e), WTCP(0x657bedfa, 0x4e019a9d), WTCP(0x6334c6d2, 0x50e22c0b),
    WTCP(0x60d50689, 0x53b606cb), WTCP(0x5e5de588, 0x567b0ea7), WTCP(0x5bd0c9c6, 0x592f4460),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow128[] = {
    WTCP(0x7ffffffe, 0x00016f63), WTCP(0x7ffffff1, 0x0003e382), WTCP(0x7fffffc7, 0x00078f64),
    WTCP(0x7fffff5d, 0x000cc323), WTCP(0x7ffffe76, 0x0013d9ed), WTCP(0x7ffffcaa, 0x001d3a9d),
    WTCP(0x7ffff953, 0x0029581f), WTCP(0x7ffff372, 0x0038b1bd), WTCP(0x7fffe98b, 0x004bd34d),
    WTCP(0x7fffd975, 0x00635538), WTCP(0x7fffc024, 0x007fdc64), WTCP(0x7fff995b, 0x00a219f1),
    WTCP(0x7fff5f5b, 0x00cacad0), WTCP(0x7fff0a75, 0x00fab72d), WTCP(0x7ffe9091, 0x0132b1af),
    WTCP(0x7ffde49e, 0x01739689), WTCP(0x7ffcf5ef, 0x01be4a63), WTCP(0x7ffbaf84, 0x0213b910),
    WTCP(0x7ff9f73a, 0x0274d41e), WTCP(0x7ff7acf1, 0x02e2913a), WTCP(0x7ff4a99a, 0x035de86c),
    WTCP(0x7ff0be3d, 0x03e7d233), WTCP(0x7febb2f1, 0x0481457c), WTCP(0x7fe545d4, 0x052b357c),
    WTCP(0x7fdd2a02, 0x05e68f77), WTCP(0x7fd30695, 0x06b4386f), WTCP(0x7fc675b4, 0x07950acb),
    WTCP(0x7fb703be, 0x0889d3ef), WTCP(0x7fa42e89, 0x099351e0), WTCP(0x7f8d64d8, 0x0ab230e0),
    WTCP(0x7f7205f8, 0x0be70923), WTCP(0x7f516195, 0x0d325c93), WTCP(0x7f2ab7d0, 0x0e9494ae),
    WTCP(0x7efd3997, 0x100e0085), WTCP(0x7ec8094a, 0x119ed2ef), WTCP(0x7e8a3ba7, 0x134720d8),
    WTCP(0x7e42d906, 0x1506dfdc), WTCP(0x7df0dee4, 0x16dde50b), WTCP(0x7d9341b4, 0x18cbe3f7),
    WTCP(0x7d28ef02, 0x1ad06e07), WTCP(0x7cb0cfcc, 0x1ceaf215), WTCP(0x7c29cb20, 0x1f1abc4f),
    WTCP(0x7b92c8eb, 0x215ef677), WTCP(0x7aeab4ec, 0x23b6a867), WTCP(0x7a3081d0, 0x2620b8ec),
    WTCP(0x79632c5a, 0x289beef5), WTCP(0x7881be95, 0x2b26f30b), WTCP(0x778b5304, 0x2dc0511f),
    WTCP(0x767f17c0, 0x30667aa2), WTCP(0x755c5178, 0x3317c8dd), WTCP(0x74225e50, 0x35d27f98),
    WTCP(0x72d0b887, 0x3894cff3), WTCP(0x7166f8e7, 0x3b5cdb7b), WTCP(0x6fe4d8e8, 0x3e28b770),
    WTCP(0x6e4a3491, 0x40f6702a), WTCP(0x6c970bfc, 0x43c40caa), WTCP(0x6acb8483, 0x468f9231),
    WTCP(0x68e7e994, 0x495707f5), WTCP(0x66ecad1c, 0x4c187ac7), WTCP(0x64da6797, 0x4ed200c5),
    WTCP(0x62b1d7b7, 0x5181bcea), WTCP(0x6073e1ae, 0x5425e28e), WTCP(0x5e218e16, 0x56bcb8c2),
    WTCP(0x5bbc0875, 0x59449d76),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow256[] = {
    WTCP(0x7fffffff, 0x000103c8), WTCP(0x7ffffffc, 0x000203ad), WTCP(0x7ffffff5, 0x0003410a),
    WTCP(0x7fffffe9, 0x0004c6ce), WTCP(0x7fffffd4, 0x00069ee0), WTCP(0x7fffffb2, 0x0008d376),
    WTCP(0x7fffff7d, 0x000b6f5a), WTCP(0x7fffff2e, 0x000e7dfd), WTCP(0x7ffffeba, 0x00120b83),
    WTCP(0x7ffffe16, 0x001624cd), WTCP(0x7ffffd30, 0x001ad778), WTCP(0x7ffffbf3, 0x002031e2),
    WTCP(0x7ffffa48, 0x00264330), WTCP(0x7ffff80d, 0x002d1b4b), WTCP(0x7ffff51d, 0x0034cae6),
    WTCP(0x7ffff147, 0x003d637c), WTCP(0x7fffec54, 0x0046f751), WTCP(0x7fffe5fe, 0x00519974),
    WTCP(0x7fffddf3, 0x005d5dba), WTCP(0x7fffd3d2, 0x006a58c1), WTCP(0x7fffc72a, 0x00789feb),
    WTCP(0x7fffb772, 0x0088495d), WTCP(0x7fffa40e, 0x00996bfb), WTCP(0x7fff8c46, 0x00ac1f63),
    WTCP(0x7fff6f46, 0x00c07bec), WTCP(0x7fff4c19, 0x00d69a9b), WTCP(0x7fff21a6, 0x00ee9523),
    WTCP(0x7ffeeeab, 0x010885d9), WTCP(0x7ffeb1b8, 0x012487b1), WTCP(0x7ffe692f, 0x0142b631),
    WTCP(0x7ffe1335, 0x01632d6f), WTCP(0x7ffdadb8, 0x01860a00), WTCP(0x7ffd3661, 0x01ab68f3),
    WTCP(0x7ffcaa91, 0x01d367c5), WTCP(0x7ffc075b, 0x01fe2453), WTCP(0x7ffb497e, 0x022bbcd0),
    WTCP(0x7ffa6d59, 0x025c4fba), WTCP(0x7ff96eeb, 0x028ffbc7), WTCP(0x7ff849c6, 0x02c6dfdb),
    WTCP(0x7ff6f90b, 0x03011afc), WTCP(0x7ff57760, 0x033ecc3a), WTCP(0x7ff3bee7, 0x038012a8),
    WTCP(0x7ff1c939, 0x03c50d47), WTCP(0x7fef8f5a, 0x040ddaf6), WTCP(0x7fed09b4, 0x045a9a64),
    WTCP(0x7fea300e, 0x04ab69f9), WTCP(0x7fe6f980, 0x050067c7), WTCP(0x7fe35c70, 0x0559b17b),
    WTCP(0x7fdf4e88, 0x05b76443), WTCP(0x7fdac4ad, 0x06199cc4), WTCP(0x7fd5b2f8, 0x068076fe),
    WTCP(0x7fd00caf, 0x06ec0e41), WTCP(0x7fc9c441, 0x075c7d16), WTCP(0x7fc2cb3b, 0x07d1dd2c),
    WTCP(0x7fbb1242, 0x084c4745), WTCP(0x7fb28915, 0x08cbd323), WTCP(0x7fa91e7e, 0x09509778),
    WTCP(0x7f9ec059, 0x09daa9cc), WTCP(0x7f935b87, 0x0a6a1e74), WTCP(0x7f86dbf2, 0x0aff0877),
    WTCP(0x7f792c8a, 0x0b997983), WTCP(0x7f6a3746, 0x0c3981d6), WTCP(0x7f59e520, 0x0cdf3030),
    WTCP(0x7f481e1c, 0x0d8a91c3), WTCP(0x7f34c949, 0x0e3bb222), WTCP(0x7f1fccc3, 0x0ef29b30),
    WTCP(0x7f090dbc, 0x0faf5513), WTCP(0x7ef0707d, 0x1071e629), WTCP(0x7ed5d872, 0x113a52f4),
    WTCP(0x7eb92831, 0x12089e14), WTCP(0x7e9a4183, 0x12dcc836), WTCP(0x7e790571, 0x13b6d010),
    WTCP(0x7e55544e, 0x1496b24f), WTCP(0x7e2f0dc8, 0x157c6998), WTCP(0x7e0610f1, 0x1667ee77),
    WTCP(0x7dda3c54, 0x17593760), WTCP(0x7dab6e06, 0x185038a3), WTCP(0x7d7983b3, 0x194ce46e),
    WTCP(0x7d445ab5, 0x1a4f2ac4), WTCP(0x7d0bd028, 0x1b56f981), WTCP(0x7ccfc0fd, 0x1c643c54),
    WTCP(0x7c900a11, 0x1d76dcc2), WTCP(0x7c4c8844, 0x1e8ec227), WTCP(0x7c05188d, 0x1fabd1bb),
    WTCP(0x7bb99817, 0x20cdee92), WTCP(0x7b69e455, 0x21f4f9a6), WTCP(0x7b15db1a, 0x2320d1dc),
    WTCP(0x7abd5ab8, 0x2451540c), WTCP(0x7a604213, 0x25865b09), WTCP(0x79fe70bf, 0x26bfbfaf),
    WTCP(0x7997c716, 0x27fd58ed), WTCP(0x792c2654, 0x293efbd0), WTCP(0x78bb70b0, 0x2a847b97),
    WTCP(0x78458976, 0x2bcda9bb), WTCP(0x77ca551d, 0x2d1a5608), WTCP(0x7749b965, 0x2e6a4ea6),
    WTCP(0x76c39d68, 0x2fbd6036), WTCP(0x7637e9b8, 0x311355dc), WTCP(0x75a68873, 0x326bf95a),
    WTCP(0x750f6559, 0x33c71326), WTCP(0x74726de1, 0x35246a7e), WTCP(0x73cf914f, 0x3683c582),
    WTCP(0x7326c0c8, 0x37e4e94b), WTCP(0x7277ef5f, 0x39479a08), WTCP(0x71c3122f, 0x3aab9b14),
    WTCP(0x71082063, 0x3c10af11), WTCP(0x7047134a, 0x3d769807), WTCP(0x6f7fe661, 0x3edd177c),
    WTCP(0x6eb29763, 0x4043ee92), WTCP(0x6ddf2651, 0x41aade26), WTCP(0x6d05957c, 0x4311a6e8),
    WTCP(0x6c25e98f, 0x4478097b), WTCP(0x6b402991, 0x45ddc693), WTCP(0x6a545ef0, 0x47429f13),
    WTCP(0x6962957f, 0x48a65427), WTCP(0x686adb7c, 0x4a08a764), WTCP(0x676d418d, 0x4b695ae8),
    WTCP(0x6669dac2, 0x4cc83171), WTCP(0x6560bc90, 0x4e24ee7d), WTCP(0x6451fecf, 0x4f7f5668),
    WTCP(0x633dbbb1, 0x50d72e85), WTCP(0x62240fbd, 0x522c3d3b), WTCP(0x610519c7, 0x537e4a1f),
    WTCP(0x5fe0fae3, 0x54cd1e10), WTCP(0x5eb7d65c, 0x5618834c), WTCP(0x5d89d1a5, 0x57604590),
    WTCP(0x5c57144b, 0x58a43227), WTCP(0x5b1fc7e6, 0x59e41808),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow768[] = {
    WTCP(0x7fffff85, 0x000b11d9), WTCP(0x7ffffef0, 0x00107aa9), WTCP(0x7ffffe3e, 0x0015351c),
    WTCP(0x7ffffd6c, 0x0019b0a1), WTCP(0x7ffffc77, 0x001e1656), WTCP(0x7ffffb5b, 0x00227a80),
    WTCP(0x7ffffa16, 0x0026e8d3), WTCP(0x7ffff8a4, 0x002b68c9), WTCP(0x7ffff700, 0x002fff8a),
    WTCP(0x7ffff528, 0x0034b0d9), WTCP(0x7ffff316, 0x00397f9c), WTCP(0x7ffff0c6, 0x003e6e22),
    WTCP(0x7fffee35, 0x00437e53), WTCP(0x7fffeb5b, 0x0048b1d0), WTCP(0x7fffe836, 0x004e0a05),
    WTCP(0x7fffe4be, 0x00538837), WTCP(0x7fffe0ef, 0x00592d8e), WTCP(0x7fffdcc3, 0x005efb1a),
    WTCP(0x7fffd832, 0x0064f1da), WTCP(0x7fffd337, 0x006b12c1), WTCP(0x7fffcdcb, 0x00715eb4),
    WTCP(0x7fffc7e7, 0x0077d692), WTCP(0x7fffc182, 0x007e7b30), WTCP(0x7fffba96, 0x00854d61),
    WTCP(0x7fffb31b, 0x008c4df0), WTCP(0x7fffab06, 0x00937da6), WTCP(0x7fffa251, 0x009add48),
    WTCP(0x7fff98f1, 0x00a26d98), WTCP(0x7fff8edd, 0x00aa2f57), WTCP(0x7fff840b, 0x00b22343),
    WTCP(0x7fff7870, 0x00ba4a19), WTCP(0x7fff6c02, 0x00c2a495), WTCP(0x7fff5eb5, 0x00cb3371),
    WTCP(0x7fff507e, 0x00d3f767), WTCP(0x7fff4150, 0x00dcf130), WTCP(0x7fff311f, 0x00e62183),
    WTCP(0x7fff1fde, 0x00ef8919), WTCP(0x7fff0d7f, 0x00f928a7), WTCP(0x7ffef9f4, 0x010300e5),
    WTCP(0x7ffee52f, 0x010d1288), WTCP(0x7ffecf20, 0x01175e47), WTCP(0x7ffeb7b8, 0x0121e4d6),
    WTCP(0x7ffe9ee6, 0x012ca6eb), WTCP(0x7ffe849b, 0x0137a53b), WTCP(0x7ffe68c4, 0x0142e07a),
    WTCP(0x7ffe4b50, 0x014e595c), WTCP(0x7ffe2c2c, 0x015a1095), WTCP(0x7ffe0b45, 0x016606da),
    WTCP(0x7ffde888, 0x01723cde), WTCP(0x7ffdc3df, 0x017eb353), WTCP(0x7ffd9d37, 0x018b6aed),
    WTCP(0x7ffd7479, 0x0198645f), WTCP(0x7ffd4990, 0x01a5a05b), WTCP(0x7ffd1c63, 0x01b31f92),
    WTCP(0x7ffcecdc, 0x01c0e2b8), WTCP(0x7ffcbae2, 0x01ceea7d), WTCP(0x7ffc865c, 0x01dd3793),
    WTCP(0x7ffc4f2f, 0x01ebcaaa), WTCP(0x7ffc1542, 0x01faa472), WTCP(0x7ffbd879, 0x0209c59c),
    WTCP(0x7ffb98b7, 0x02192ed7), WTCP(0x7ffb55e0, 0x0228e0d2), WTCP(0x7ffb0fd6, 0x0238dc3c),
    WTCP(0x7ffac679, 0x024921c3), WTCP(0x7ffa79ac, 0x0259b215), WTCP(0x7ffa294d, 0x026a8dde),
    WTCP(0x7ff9d53b, 0x027bb5cc), WTCP(0x7ff97d54, 0x028d2a8a), WTCP(0x7ff92175, 0x029eecc3),
    WTCP(0x7ff8c17a, 0x02b0fd23), WTCP(0x7ff85d3f, 0x02c35c53), WTCP(0x7ff7f49d, 0x02d60afd),
    WTCP(0x7ff7876e, 0x02e909ca), WTCP(0x7ff7158b, 0x02fc5960), WTCP(0x7ff69eca, 0x030ffa69),
    WTCP(0x7ff62303, 0x0323ed89), WTCP(0x7ff5a20a, 0x03383367), WTCP(0x7ff51bb3, 0x034ccca7),
    WTCP(0x7ff48fd3, 0x0361b9ed), WTCP(0x7ff3fe3c, 0x0376fbdd), WTCP(0x7ff366be, 0x038c9317),
    WTCP(0x7ff2c929, 0x03a2803e), WTCP(0x7ff2254e, 0x03b8c3f2), WTCP(0x7ff17afa, 0x03cf5ed1),
    WTCP(0x7ff0c9f9, 0x03e6517a), WTCP(0x7ff01218, 0x03fd9c8a), WTCP(0x7fef5321, 0x0415409c),
    WTCP(0x7fee8cde, 0x042d3e4d), WTCP(0x7fedbf17, 0x04459634), WTCP(0x7fece993, 0x045e48ec),
    WTCP(0x7fec0c18, 0x0477570a), WTCP(0x7feb266a, 0x0490c127), WTCP(0x7fea384e, 0x04aa87d5),
    WTCP(0x7fe94186, 0x04c4abaa), WTCP(0x7fe841d3, 0x04df2d37), WTCP(0x7fe738f4, 0x04fa0d0d),
    WTCP(0x7fe626a9, 0x05154bbc), WTCP(0x7fe50aaf, 0x0530e9d3), WTCP(0x7fe3e4c1, 0x054ce7dd),
    WTCP(0x7fe2b49b, 0x05694667), WTCP(0x7fe179f6, 0x058605fa), WTCP(0x7fe0348b, 0x05a3271e),
    WTCP(0x7fdee410, 0x05c0aa5c), WTCP(0x7fdd883b, 0x05de9038), WTCP(0x7fdc20c1, 0x05fcd935),
    WTCP(0x7fdaad53, 0x061b85d6), WTCP(0x7fd92da5, 0x063a969c), WTCP(0x7fd7a166, 0x065a0c06),
    WTCP(0x7fd60844, 0x0679e690), WTCP(0x7fd461ee, 0x069a26b6), WTCP(0x7fd2ae10, 0x06baccf2),
    WTCP(0x7fd0ec55, 0x06dbd9bd), WTCP(0x7fcf1c65, 0x06fd4d8c), WTCP(0x7fcd3de9, 0x071f28d3),
    WTCP(0x7fcb5088, 0x07416c06), WTCP(0x7fc953e6, 0x07641794), WTCP(0x7fc747a8, 0x07872bee),
    WTCP(0x7fc52b70, 0x07aaa97f), WTCP(0x7fc2fedf, 0x07ce90b4), WTCP(0x7fc0c195, 0x07f2e1f4),
    WTCP(0x7fbe732f, 0x08179da7), WTCP(0x7fbc134b, 0x083cc431), WTCP(0x7fb9a183, 0x086255f7),
    WTCP(0x7fb71d72, 0x08885359), WTCP(0x7fb486af, 0x08aebcb5), WTCP(0x7fb1dcd3, 0x08d59269),
    WTCP(0x7faf1f72, 0x08fcd4cf), WTCP(0x7fac4e21, 0x09248440), WTCP(0x7fa96873, 0x094ca111),
    WTCP(0x7fa66df8, 0x09752b98), WTCP(0x7fa35e40, 0x099e2425), WTCP(0x7fa038db, 0x09c78b09),
    WTCP(0x7f9cfd54, 0x09f16090), WTCP(0x7f99ab38, 0x0a1ba507), WTCP(0x7f964210, 0x0a4658b6),
    WTCP(0x7f92c165, 0x0a717be2), WTCP(0x7f8f28bf, 0x0a9d0ed1), WTCP(0x7f8b77a4, 0x0ac911c4),
    WTCP(0x7f87ad97, 0x0af584fb), WTCP(0x7f83ca1d, 0x0b2268b2), WTCP(0x7f7fccb5, 0x0b4fbd23),
    WTCP(0x7f7bb4e2, 0x0b7d8288), WTCP(0x7f778221, 0x0babb915), WTCP(0x7f7333f1, 0x0bda60fd),
    WTCP(0x7f6ec9cd, 0x0c097a72), WTCP(0x7f6a4330, 0x0c3905a1), WTCP(0x7f659f94, 0x0c6902b6),
    WTCP(0x7f60de70, 0x0c9971d9), WTCP(0x7f5bff3b, 0x0cca5331), WTCP(0x7f57016b, 0x0cfba6e3),
    WTCP(0x7f51e474, 0x0d2d6d0e), WTCP(0x7f4ca7c8, 0x0d5fa5d2), WTCP(0x7f474ad9, 0x0d92514a),
    WTCP(0x7f41cd17, 0x0dc56f90), WTCP(0x7f3c2df1, 0x0df900bb), WTCP(0x7f366cd5, 0x0e2d04de),
    WTCP(0x7f30892e, 0x0e617c0a), WTCP(0x7f2a8269, 0x0e96664e), WTCP(0x7f2457ef, 0x0ecbc3b5),
    WTCP(0x7f1e0929, 0x0f019449), WTCP(0x7f17957e, 0x0f37d80f), WTCP(0x7f10fc55, 0x0f6e8f0c),
    WTCP(0x7f0a3d14, 0x0fa5b940), WTCP(0x7f03571d, 0x0fdd56a8), WTCP(0x7efc49d4, 0x10156740),
    WTCP(0x7ef5149b, 0x104deb00), WTCP(0x7eedb6d2, 0x1086e1dd), WTCP(0x7ee62fda, 0x10c04bca),
    WTCP(0x7ede7f11, 0x10fa28b7), WTCP(0x7ed6a3d5, 0x11347890), WTCP(0x7ece9d81, 0x116f3b3f),
    WTCP(0x7ec66b73, 0x11aa70ac), WTCP(0x7ebe0d04, 0x11e618ba), WTCP(0x7eb5818d, 0x1222334c),
    WTCP(0x7eacc869, 0x125ec03e), WTCP(0x7ea3e0ef, 0x129bbf6e), WTCP(0x7e9aca75, 0x12d930b2),
    WTCP(0x7e918452, 0x131713e2), WTCP(0x7e880ddb, 0x135568cf), WTCP(0x7e7e6665, 0x13942f49),
    WTCP(0x7e748d43, 0x13d3671e), WTCP(0x7e6a81c8, 0x14131017), WTCP(0x7e604347, 0x145329fa),
    WTCP(0x7e55d111, 0x1493b48c), WTCP(0x7e4b2a76, 0x14d4af8e), WTCP(0x7e404ec8, 0x15161abe),
    WTCP(0x7e353d55, 0x1557f5d7), WTCP(0x7e29f56c, 0x159a4090), WTCP(0x7e1e765c, 0x15dcfaa0),
    WTCP(0x7e12bf72, 0x162023b7), WTCP(0x7e06cffc, 0x1663bb86), WTCP(0x7dfaa746, 0x16a7c1b9),
    WTCP(0x7dee449e, 0x16ec35f7), WTCP(0x7de1a74e, 0x173117e9), WTCP(0x7dd4cea3, 0x17766731),
    WTCP(0x7dc7b9e7, 0x17bc236f), WTCP(0x7dba6865, 0x18024c40), WTCP(0x7dacd968, 0x1848e13f),
    WTCP(0x7d9f0c3a, 0x188fe204), WTCP(0x7d910025, 0x18d74e22), WTCP(0x7d82b472, 0x191f252c),
    WTCP(0x7d74286c, 0x196766ae), WTCP(0x7d655b5b, 0x19b01236), WTCP(0x7d564c8a, 0x19f9274b),
    WTCP(0x7d46fb40, 0x1a42a574), WTCP(0x7d3766c8, 0x1a8c8c32), WTCP(0x7d278e6a, 0x1ad6db06),
    WTCP(0x7d17716f, 0x1b21916c), WTCP(0x7d070f22, 0x1b6caedf), WTCP(0x7cf666cb, 0x1bb832d5),
    WTCP(0x7ce577b3, 0x1c041cc2), WTCP(0x7cd44124, 0x1c506c17), WTCP(0x7cc2c269, 0x1c9d2044),
    WTCP(0x7cb0faca, 0x1cea38b2), WTCP(0x7c9ee992, 0x1d37b4cc), WTCP(0x7c8c8e0c, 0x1d8593f5),
    WTCP(0x7c79e782, 0x1dd3d592), WTCP(0x7c66f541, 0x1e227903), WTCP(0x7c53b692, 0x1e717da3),
    WTCP(0x7c402ac3, 0x1ec0e2cf), WTCP(0x7c2c5120, 0x1f10a7dc), WTCP(0x7c1828f6, 0x1f60cc21),
    WTCP(0x7c03b193, 0x1fb14eef), WTCP(0x7beeea44, 0x20022f96), WTCP(0x7bd9d259, 0x20536d61),
    WTCP(0x7bc46921, 0x20a5079a), WTCP(0x7baeadec, 0x20f6fd8a), WTCP(0x7b98a00b, 0x21494e73),
    WTCP(0x7b823ecf, 0x219bf998), WTCP(0x7b6b898b, 0x21eefe37), WTCP(0x7b547f93, 0x22425b8d),
    WTCP(0x7b3d203a, 0x229610d4), WTCP(0x7b256ad5, 0x22ea1d42), WTCP(0x7b0d5ebb, 0x233e800c),
    WTCP(0x7af4fb42, 0x23933864), WTCP(0x7adc3fc2, 0x23e8457a), WTCP(0x7ac32b95, 0x243da679),
    WTCP(0x7aa9be14, 0x24935a8d), WTCP(0x7a8ff69a, 0x24e960dd), WTCP(0x7a75d485, 0x253fb88e),
    WTCP(0x7a5b5731, 0x259660c3), WTCP(0x7a407dfe, 0x25ed589c), WTCP(0x7a25484c, 0x26449f38),
    WTCP(0x7a09b57c, 0x269c33b1), WTCP(0x79edc4f1, 0x26f41522), WTCP(0x79d1760e, 0x274c42a0),
    WTCP(0x79b4c83b, 0x27a4bb40), WTCP(0x7997badd, 0x27fd7e15), WTCP(0x797a4d5e, 0x28568a2f),
    WTCP(0x795c7f26, 0x28afde9a), WTCP(0x793e4fa3, 0x29097a63), WTCP(0x791fbe40, 0x29635c92),
    WTCP(0x7900ca6e, 0x29bd842e), WTCP(0x78e1739c, 0x2a17f03e), WTCP(0x78c1b93d, 0x2a729fc2),
    WTCP(0x78a19ac4, 0x2acd91bc), WTCP(0x788117a7, 0x2b28c52a), WTCP(0x78602f5e, 0x2b843909),
    WTCP(0x783ee163, 0x2bdfec54), WTCP(0x781d2d2f, 0x2c3bde02), WTCP(0x77fb1241, 0x2c980d0a),
    WTCP(0x77d89017, 0x2cf47862), WTCP(0x77b5a632, 0x2d511efb), WTCP(0x77925416, 0x2dadffc6),
    WTCP(0x776e9947, 0x2e0b19b3), WTCP(0x774a754d, 0x2e686bae), WTCP(0x7725e7b0, 0x2ec5f4a4),
    WTCP(0x7700effd, 0x2f23b37d), WTCP(0x76db8dbf, 0x2f81a721), WTCP(0x76b5c088, 0x2fdfce77),
    WTCP(0x768f87e8, 0x303e2863), WTCP(0x7668e375, 0x309cb3c8), WTCP(0x7641d2c4, 0x30fb6f88),
    WTCP(0x761a556e, 0x315a5a82), WTCP(0x75f26b0e, 0x31b97394), WTCP(0x75ca1341, 0x3218b99c),
    WTCP(0x75a14da8, 0x32782b74), WTCP(0x757819e4, 0x32d7c7f6), WTCP(0x754e779a, 0x33378dfc),
    WTCP(0x75246671, 0x33977c5b), WTCP(0x74f9e613, 0x33f791e9), WTCP(0x74cef62b, 0x3457cd7c),
    WTCP(0x74a3966a, 0x34b82de6), WTCP(0x7477c67f, 0x3518b1f9), WTCP(0x744b861e, 0x35795887),
    WTCP(0x741ed4ff, 0x35da205e), WTCP(0x73f1b2da, 0x363b084e), WTCP(0x73c41f6b, 0x369c0f24),
    WTCP(0x73961a71, 0x36fd33ac), WTCP(0x7367a3ac, 0x375e74b1), WTCP(0x7338bae1, 0x37bfd0ff),
    WTCP(0x73095fd7, 0x3821475f), WTCP(0x72d99257, 0x3882d699), WTCP(0x72a9522d, 0x38e47d75),
    WTCP(0x72789f28, 0x39463aba), WTCP(0x7247791b, 0x39a80d2e), WTCP(0x7215dfda, 0x3a09f397),
    WTCP(0x71e3d33d, 0x3a6becba), WTCP(0x71b1531f, 0x3acdf75a), WTCP(0x717e5f5d, 0x3b30123b),
    WTCP(0x714af7d7, 0x3b923c20), WTCP(0x71171c72, 0x3bf473cc), WTCP(0x70e2cd14, 0x3c56b7ff),
    WTCP(0x70ae09a6, 0x3cb9077b), WTCP(0x7078d215, 0x3d1b6101), WTCP(0x7043264f, 0x3d7dc353),
    WTCP(0x700d0648, 0x3de02d2e), WTCP(0x6fd671f5, 0x3e429d55), WTCP(0x6f9f694f, 0x3ea51285),
    WTCP(0x6f67ec52, 0x3f078b7f), WTCP(0x6f2ffafb, 0x3f6a0701), WTCP(0x6ef7954e, 0x3fcc83ca),
    WTCP(0x6ebebb4e, 0x402f009a), WTCP(0x6e856d05, 0x40917c2e), WTCP(0x6e4baa7e, 0x40f3f546),
    WTCP(0x6e1173c6, 0x41566aa1), WTCP(0x6dd6c8ef, 0x41b8dafc), WTCP(0x6d9baa0f, 0x421b4518),
    WTCP(0x6d60173d, 0x427da7b1), WTCP(0x6d241094, 0x42e00189), WTCP(0x6ce79632, 0x4342515e),
    WTCP(0x6caaa839, 0x43a495ef), WTCP(0x6c6d46ce, 0x4406cdfd), WTCP(0x6c2f7218, 0x4468f848),
    WTCP(0x6bf12a42, 0x44cb138f), WTCP(0x6bb26f7b, 0x452d1e94), WTCP(0x6b7341f5, 0x458f1818),
    WTCP(0x6b33a1e3, 0x45f0fede), WTCP(0x6af38f7e, 0x4652d1a6), WTCP(0x6ab30b01, 0x46b48f34),
    WTCP(0x6a7214ab, 0x4716364c), WTCP(0x6a30acbd, 0x4777c5b2), WTCP(0x69eed37c, 0x47d93c2a),
    WTCP(0x69ac8930, 0x483a987a), WTCP(0x6969ce24, 0x489bd968), WTCP(0x6926a2a8, 0x48fcfdbb),
    WTCP(0x68e3070c, 0x495e043b), WTCP(0x689efba7, 0x49beebb0), WTCP(0x685a80cf, 0x4a1fb2e5),
    WTCP(0x681596e1, 0x4a8058a4), WTCP(0x67d03e3b, 0x4ae0dbb8), WTCP(0x678a773f, 0x4b413aee),
    WTCP(0x67444253, 0x4ba17514), WTCP(0x66fd9fde, 0x4c0188f8), WTCP(0x66b6904c, 0x4c61756b),
    WTCP(0x666f140d, 0x4cc1393d), WTCP(0x66272b91, 0x4d20d341), WTCP(0x65ded74d, 0x4d80424a),
    WTCP(0x659617bb, 0x4ddf852d), WTCP(0x654ced55, 0x4e3e9ac1), WTCP(0x6503589b, 0x4e9d81dc),
    WTCP(0x64b95a0d, 0x4efc3959), WTCP(0x646ef230, 0x4f5ac010), WTCP(0x6424218d, 0x4fb914df),
    WTCP(0x63d8e8ae, 0x501736a1), WTCP(0x638d4822, 0x50752438), WTCP(0x6341407a, 0x50d2dc82),
    WTCP(0x62f4d24b, 0x51305e61), WTCP(0x62a7fe2b, 0x518da8bb), WTCP(0x625ac4b5, 0x51eaba74),
    WTCP(0x620d2686, 0x52479273), WTCP(0x61bf2440, 0x52a42fa2), WTCP(0x6170be85, 0x530090ea),
    WTCP(0x6121f5fb, 0x535cb53a), WTCP(0x60d2cb4e, 0x53b89b7e), WTCP(0x60833f28, 0x541442a8),
    WTCP(0x60335239, 0x546fa9a9), WTCP(0x5fe30533, 0x54cacf77), WTCP(0x5f9258cc, 0x5525b306),
    WTCP(0x5f414dbb, 0x55805350), WTCP(0x5eefe4bc, 0x55daaf4e), WTCP(0x5e9e1e8c, 0x5634c5fe),
    WTCP(0x5e4bfbec, 0x568e965c), WTCP(0x5df97d9e, 0x56e81f6c), WTCP(0x5da6a46a, 0x5741602e),
    WTCP(0x5d537118, 0x579a57a8), WTCP(0x5cffe474, 0x57f304e2), WTCP(0x5cabff4c, 0x584b66e4),
    WTCP(0x5c57c271, 0x58a37cbb), WTCP(0x5c032eb7, 0x58fb4576), WTCP(0x5bae44f4, 0x5952c024),
    WTCP(0x5b590602, 0x59a9ebd8), WTCP(0x5b0372bb, 0x5a00c7a8), WTCP(0x5aad8bfe, 0x5a5752ac),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow960[] = {
    WTCP(0x7fffff9e, 0x0009e6ac), WTCP(0x7fffff2b, 0x000e96d5), WTCP(0x7ffffea6, 0x0012987e),
    WTCP(0x7ffffe0e, 0x001652b6), WTCP(0x7ffffd60, 0x0019ebce), WTCP(0x7ffffc9c, 0x001d76bf),
    WTCP(0x7ffffbbf, 0x0020fe79), WTCP(0x7ffffac9, 0x002489ef), WTCP(0x7ffff9b7, 0x00281de2),
    WTCP(0x7ffff887, 0x002bbdbb), WTCP(0x7ffff737, 0x002f6c0d), WTCP(0x7ffff5c6, 0x00332ad8),
    WTCP(0x7ffff431, 0x0036fbb9), WTCP(0x7ffff276, 0x003ae004), WTCP(0x7ffff092, 0x003ed8d8),
    WTCP(0x7fffee84, 0x0042e72f), WTCP(0x7fffec48, 0x00470be3), WTCP(0x7fffe9dd, 0x004b47b8),
    WTCP(0x7fffe73f, 0x004f9b5f), WTCP(0x7fffe46b, 0x0054077a), WTCP(0x7fffe15f, 0x00588ca1),
    WTCP(0x7fffde17, 0x005d2b61), WTCP(0x7fffda91, 0x0061e442), WTCP(0x7fffd6c9, 0x0066b7c2),
    WTCP(0x7fffd2bb, 0x006ba65c), WTCP(0x7fffce65, 0x0070b087), WTCP(0x7fffc9c2, 0x0075d6b5),
    WTCP(0x7fffc4cf, 0x007b1955), WTCP(0x7fffbf87, 0x008078d5), WTCP(0x7fffb9e7, 0x0085f5a0),
    WTCP(0x7fffb3ea, 0x008b901d), WTCP(0x7fffad8c, 0x009148b4), WTCP(0x7fffa6c9, 0x00971fcb),
    WTCP(0x7fff9f9c, 0x009d15c7), WTCP(0x7fff9800, 0x00a32b0b), WTCP(0x7fff8ff0, 0x00a95ff9),
    WTCP(0x7fff8767, 0x00afb4f4), WTCP(0x7fff7e5f, 0x00b62a5c), WTCP(0x7fff74d4, 0x00bcc093),
    WTCP(0x7fff6ac0, 0x00c377f8), WTCP(0x7fff601c, 0x00ca50eb), WTCP(0x7fff54e3, 0x00d14bcb),
    WTCP(0x7fff490e, 0x00d868f7), WTCP(0x7fff3c98, 0x00dfa8ce), WTCP(0x7fff2f79, 0x00e70bad),
    WTCP(0x7fff21ac, 0x00ee91f3), WTCP(0x7fff1328, 0x00f63bfe), WTCP(0x7fff03e7, 0x00fe0a2c),
    WTCP(0x7ffef3e1, 0x0105fcd9), WTCP(0x7ffee310, 0x010e1462), WTCP(0x7ffed16a, 0x01165126),
    WTCP(0x7ffebee9, 0x011eb381), WTCP(0x7ffeab83, 0x01273bd0), WTCP(0x7ffe9731, 0x012fea6f),
    WTCP(0x7ffe81ea, 0x0138bfbc), WTCP(0x7ffe6ba4, 0x0141bc12), WTCP(0x7ffe5457, 0x014adfce),
    WTCP(0x7ffe3bfa, 0x01542b4d), WTCP(0x7ffe2282, 0x015d9ee9), WTCP(0x7ffe07e6, 0x01673b01),
    WTCP(0x7ffdec1b, 0x0170ffee), WTCP(0x7ffdcf17, 0x017aee0e), WTCP(0x7ffdb0d0, 0x018505bc),
    WTCP(0x7ffd913b, 0x018f4754), WTCP(0x7ffd704b, 0x0199b330), WTCP(0x7ffd4df7, 0x01a449ad),
    WTCP(0x7ffd2a31, 0x01af0b25), WTCP(0x7ffd04ef, 0x01b9f7f4), WTCP(0x7ffcde23, 0x01c51074),
    WTCP(0x7ffcb5c1, 0x01d05501), WTCP(0x7ffc8bbc, 0x01dbc5f5), WTCP(0x7ffc6006, 0x01e763ab),
    WTCP(0x7ffc3293, 0x01f32e7d), WTCP(0x7ffc0354, 0x01ff26c5), WTCP(0x7ffbd23b, 0x020b4cde),
    WTCP(0x7ffb9f3a, 0x0217a120), WTCP(0x7ffb6a41, 0x022423e6), WTCP(0x7ffb3342, 0x0230d58a),
    WTCP(0x7ffafa2d, 0x023db664), WTCP(0x7ffabef2, 0x024ac6ce), WTCP(0x7ffa8180, 0x02580720),
    WTCP(0x7ffa41c9, 0x026577b3), WTCP(0x7ff9ffb9, 0x027318e0), WTCP(0x7ff9bb41, 0x0280eaff),
    WTCP(0x7ff9744e, 0x028eee68), WTCP(0x7ff92acf, 0x029d2371), WTCP(0x7ff8deb1, 0x02ab8a74),
    WTCP(0x7ff88fe2, 0x02ba23c7), WTCP(0x7ff83e4d, 0x02c8efc0), WTCP(0x7ff7e9e1, 0x02d7eeb7),
    WTCP(0x7ff79288, 0x02e72101), WTCP(0x7ff7382f, 0x02f686f5), WTCP(0x7ff6dac1, 0x030620e9),
    WTCP(0x7ff67a29, 0x0315ef31), WTCP(0x7ff61651, 0x0325f224), WTCP(0x7ff5af23, 0x03362a14),
    WTCP(0x7ff5448a, 0x03469758), WTCP(0x7ff4d66d, 0x03573a42), WTCP(0x7ff464b7, 0x03681327),
    WTCP(0x7ff3ef4f, 0x0379225a), WTCP(0x7ff3761d, 0x038a682e), WTCP(0x7ff2f90a, 0x039be4f4),
    WTCP(0x7ff277fb, 0x03ad9900), WTCP(0x7ff1f2d8, 0x03bf84a3), WTCP(0x7ff16986, 0x03d1a82e),
    WTCP(0x7ff0dbec, 0x03e403f3), WTCP(0x7ff049ef, 0x03f69840), WTCP(0x7fefb373, 0x04096568),
    WTCP(0x7fef185d, 0x041c6bb8), WTCP(0x7fee7890, 0x042fab81), WTCP(0x7fedd3f1, 0x04432510),
    WTCP(0x7fed2a61, 0x0456d8b4), WTCP(0x7fec7bc4, 0x046ac6ba), WTCP(0x7febc7fb, 0x047eef70),
    WTCP(0x7feb0ee8, 0x04935322), WTCP(0x7fea506b, 0x04a7f21d), WTCP(0x7fe98c65, 0x04bcccab),
    WTCP(0x7fe8c2b7, 0x04d1e318), WTCP(0x7fe7f33e, 0x04e735af), WTCP(0x7fe71ddb, 0x04fcc4ba),
    WTCP(0x7fe6426c, 0x05129081), WTCP(0x7fe560ce, 0x0528994d), WTCP(0x7fe478df, 0x053edf68),
    WTCP(0x7fe38a7c, 0x05556318), WTCP(0x7fe29581, 0x056c24a5), WTCP(0x7fe199ca, 0x05832455),
    WTCP(0x7fe09733, 0x059a626e), WTCP(0x7fdf8d95, 0x05b1df35), WTCP(0x7fde7ccb, 0x05c99aef),
    WTCP(0x7fdd64af, 0x05e195e0), WTCP(0x7fdc451a, 0x05f9d04b), WTCP(0x7fdb1de4, 0x06124a73),
    WTCP(0x7fd9eee5, 0x062b0499), WTCP(0x7fd8b7f5, 0x0643ff00), WTCP(0x7fd778ec, 0x065d39e7),
    WTCP(0x7fd6319e, 0x0676b58f), WTCP(0x7fd4e1e2, 0x06907237), WTCP(0x7fd3898d, 0x06aa701d),
    WTCP(0x7fd22873, 0x06c4af80), WTCP(0x7fd0be6a, 0x06df309c), WTCP(0x7fcf4b44, 0x06f9f3ad),
    WTCP(0x7fcdced4, 0x0714f8f0), WTCP(0x7fcc48ed, 0x0730409f), WTCP(0x7fcab960, 0x074bcaf5),
    WTCP(0x7fc91fff, 0x0767982a), WTCP(0x7fc77c9a, 0x0783a877), WTCP(0x7fc5cf02, 0x079ffc14),
    WTCP(0x7fc41705, 0x07bc9338), WTCP(0x7fc25474, 0x07d96e19), WTCP(0x7fc0871b, 0x07f68ced),
    WTCP(0x7fbeaeca, 0x0813efe7), WTCP(0x7fbccb4c, 0x0831973d), WTCP(0x7fbadc70, 0x084f8320),
    WTCP(0x7fb8e200, 0x086db3c3), WTCP(0x7fb6dbc8, 0x088c2957), WTCP(0x7fb4c993, 0x08aae40c),
    WTCP(0x7fb2ab2b, 0x08c9e412), WTCP(0x7fb0805a, 0x08e92997), WTCP(0x7fae48e9, 0x0908b4c9),
    WTCP(0x7fac04a0, 0x092885d6), WTCP(0x7fa9b347, 0x09489ce8), WTCP(0x7fa754a6, 0x0968fa2c),
    WTCP(0x7fa4e884, 0x09899dcb), WTCP(0x7fa26ea6, 0x09aa87ee), WTCP(0x7f9fe6d1, 0x09cbb8be),
    WTCP(0x7f9d50cc, 0x09ed3062), WTCP(0x7f9aac5a, 0x0a0eef00), WTCP(0x7f97f93f, 0x0a30f4bf),
    WTCP(0x7f95373e, 0x0a5341c2), WTCP(0x7f92661b, 0x0a75d62e), WTCP(0x7f8f8596, 0x0a98b224),
    WTCP(0x7f8c9572, 0x0abbd5c7), WTCP(0x7f89956f, 0x0adf4137), WTCP(0x7f86854d, 0x0b02f494),
    WTCP(0x7f8364cd, 0x0b26effd), WTCP(0x7f8033ae, 0x0b4b338f), WTCP(0x7f7cf1ae, 0x0b6fbf67),
    WTCP(0x7f799e8b, 0x0b9493a0), WTCP(0x7f763a03, 0x0bb9b056), WTCP(0x7f72c3d2, 0x0bdf15a2),
    WTCP(0x7f6f3bb5, 0x0c04c39c), WTCP(0x7f6ba168, 0x0c2aba5d), WTCP(0x7f67f4a6, 0x0c50f9fa),
    WTCP(0x7f643529, 0x0c77828a), WTCP(0x7f6062ac, 0x0c9e5420), WTCP(0x7f5c7ce8, 0x0cc56ed1),
    WTCP(0x7f588397, 0x0cecd2ae), WTCP(0x7f547670, 0x0d147fc8), WTCP(0x7f50552c, 0x0d3c7630),
    WTCP(0x7f4c1f83, 0x0d64b5f6), WTCP(0x7f47d52a, 0x0d8d3f26), WTCP(0x7f4375d9, 0x0db611ce),
    WTCP(0x7f3f0144, 0x0ddf2dfa), WTCP(0x7f3a7723, 0x0e0893b4), WTCP(0x7f35d729, 0x0e324306),
    WTCP(0x7f31210a, 0x0e5c3bf9), WTCP(0x7f2c547b, 0x0e867e94), WTCP(0x7f27712e, 0x0eb10add),
    WTCP(0x7f2276d8, 0x0edbe0da), WTCP(0x7f1d6529, 0x0f07008e), WTCP(0x7f183bd3, 0x0f3269fc),
    WTCP(0x7f12fa89, 0x0f5e1d27), WTCP(0x7f0da0fb, 0x0f8a1a0e), WTCP(0x7f082ed8, 0x0fb660b1),
    WTCP(0x7f02a3d2, 0x0fe2f10f), WTCP(0x7efcff98, 0x100fcb25), WTCP(0x7ef741d9, 0x103ceeee),
    WTCP(0x7ef16a42, 0x106a5c66), WTCP(0x7eeb7884, 0x10981386), WTCP(0x7ee56c4a, 0x10c61447),
    WTCP(0x7edf4543, 0x10f45ea0), WTCP(0x7ed9031b, 0x1122f288), WTCP(0x7ed2a57f, 0x1151cff3),
    WTCP(0x7ecc2c1a, 0x1180f6d5), WTCP(0x7ec59699, 0x11b06720), WTCP(0x7ebee4a6, 0x11e020c8),
    WTCP(0x7eb815ed, 0x121023ba), WTCP(0x7eb12a18, 0x12406fe8), WTCP(0x7eaa20d1, 0x1271053e),
    WTCP(0x7ea2f9c2, 0x12a1e3a9), WTCP(0x7e9bb494, 0x12d30b15), WTCP(0x7e9450f0, 0x13047b6c),
    WTCP(0x7e8cce7f, 0x13363497), WTCP(0x7e852ce9, 0x1368367f), WTCP(0x7e7d6bd6, 0x139a8109),
    WTCP(0x7e758aee, 0x13cd141b), WTCP(0x7e6d89d9, 0x13ffef99), WTCP(0x7e65683d, 0x14331368),
    WTCP(0x7e5d25c1, 0x14667f67), WTCP(0x7e54c20b, 0x149a3379), WTCP(0x7e4c3cc3, 0x14ce2f7c),
    WTCP(0x7e43958e, 0x1502734f), WTCP(0x7e3acc11, 0x1536fece), WTCP(0x7e31dff2, 0x156bd1d6),
    WTCP(0x7e28d0d7, 0x15a0ec41), WTCP(0x7e1f9e63, 0x15d64de9), WTCP(0x7e16483d, 0x160bf6a5),
    WTCP(0x7e0cce08, 0x1641e64c), WTCP(0x7e032f6a, 0x16781cb4), WTCP(0x7df96c05, 0x16ae99b2),
    WTCP(0x7def837e, 0x16e55d18), WTCP(0x7de57579, 0x171c66ba), WTCP(0x7ddb419a, 0x1753b667),
    WTCP(0x7dd0e784, 0x178b4bef), WTCP(0x7dc666d9, 0x17c32721), WTCP(0x7dbbbf3e, 0x17fb47ca),
    WTCP(0x7db0f056, 0x1833adb5), WTCP(0x7da5f9c3, 0x186c58ae), WTCP(0x7d9adb29, 0x18a5487d),
    WTCP(0x7d8f9429, 0x18de7cec), WTCP(0x7d842467, 0x1917f5c1), WTCP(0x7d788b86, 0x1951b2c2),
    WTCP(0x7d6cc927, 0x198bb3b4), WTCP(0x7d60dced, 0x19c5f85a), WTCP(0x7d54c67c, 0x1a008077),
    WTCP(0x7d488574, 0x1a3b4bcb), WTCP(0x7d3c1979, 0x1a765a17), WTCP(0x7d2f822d, 0x1ab1ab18),
    WTCP(0x7d22bf32, 0x1aed3e8d), WTCP(0x7d15d02b, 0x1b291432), WTCP(0x7d08b4ba, 0x1b652bc1),
    WTCP(0x7cfb6c82, 0x1ba184f5), WTCP(0x7cedf725, 0x1bde1f86), WTCP(0x7ce05445, 0x1c1afb2c),
    WTCP(0x7cd28386, 0x1c58179c), WTCP(0x7cc48489, 0x1c95748d), WTCP(0x7cb656f3, 0x1cd311b1),
    WTCP(0x7ca7fa65, 0x1d10eebd), WTCP(0x7c996e83, 0x1d4f0b60), WTCP(0x7c8ab2f0, 0x1d8d674c),
    WTCP(0x7c7bc74f, 0x1dcc0230), WTCP(0x7c6cab44, 0x1e0adbbb), WTCP(0x7c5d5e71, 0x1e49f398),
    WTCP(0x7c4de07c, 0x1e894973), WTCP(0x7c3e3108, 0x1ec8dcf8), WTCP(0x7c2e4fb9, 0x1f08add0),
    WTCP(0x7c1e3c34, 0x1f48bba3), WTCP(0x7c0df61d, 0x1f890618), WTCP(0x7bfd7d18, 0x1fc98cd6),
    WTCP(0x7becd0cc, 0x200a4f80), WTCP(0x7bdbf0dd, 0x204b4dbc), WTCP(0x7bcadcf1, 0x208c872c),
    WTCP(0x7bb994ae, 0x20cdfb71), WTCP(0x7ba817b9, 0x210faa2c), WTCP(0x7b9665bb, 0x215192fc),
    WTCP(0x7b847e58, 0x2193b57f), WTCP(0x7b726139, 0x21d61153), WTCP(0x7b600e05, 0x2218a614),
    WTCP(0x7b4d8463, 0x225b735d), WTCP(0x7b3ac3fc, 0x229e78c7), WTCP(0x7b27cc79, 0x22e1b5eb),
    WTCP(0x7b149d82, 0x23252a62), WTCP(0x7b0136c1, 0x2368d5c2), WTCP(0x7aed97df, 0x23acb7a0),
    WTCP(0x7ad9c087, 0x23f0cf92), WTCP(0x7ac5b063, 0x24351d2a), WTCP(0x7ab1671e, 0x24799ffc),
    WTCP(0x7a9ce464, 0x24be5799), WTCP(0x7a8827e1, 0x25034391), WTCP(0x7a733142, 0x25486375),
    WTCP(0x7a5e0033, 0x258db6d2), WTCP(0x7a489461, 0x25d33d35), WTCP(0x7a32ed7c, 0x2618f62c),
    WTCP(0x7a1d0b31, 0x265ee143), WTCP(0x7a06ed2f, 0x26a4fe02), WTCP(0x79f09327, 0x26eb4bf5),
    WTCP(0x79d9fcc8, 0x2731caa3), WTCP(0x79c329c2, 0x27787995), WTCP(0x79ac19c9, 0x27bf5850),
    WTCP(0x7994cc8d, 0x2806665c), WTCP(0x797d41c1, 0x284da33c), WTCP(0x79657918, 0x28950e74),
    WTCP(0x794d7247, 0x28dca788), WTCP(0x79352d01, 0x29246dfa), WTCP(0x791ca8fc, 0x296c614a),
    WTCP(0x7903e5ee, 0x29b480f9), WTCP(0x78eae38d, 0x29fccc87), WTCP(0x78d1a191, 0x2a454372),
    WTCP(0x78b81fb1, 0x2a8de537), WTCP(0x789e5da6, 0x2ad6b155), WTCP(0x78845b29, 0x2b1fa745),
    WTCP(0x786a17f5, 0x2b68c684), WTCP(0x784f93c4, 0x2bb20e8c), WTCP(0x7834ce53, 0x2bfb7ed7),
    WTCP(0x7819c75c, 0x2c4516dc), WTCP(0x77fe7e9e, 0x2c8ed615), WTCP(0x77e2f3d7, 0x2cd8bbf7),
    WTCP(0x77c726c5, 0x2d22c7fa), WTCP(0x77ab1728, 0x2d6cf993), WTCP(0x778ec4c0, 0x2db75037),
    WTCP(0x77722f4e, 0x2e01cb59), WTCP(0x77555695, 0x2e4c6a6d), WTCP(0x77383a58, 0x2e972ce6),
    WTCP(0x771ada5a, 0x2ee21235), WTCP(0x76fd3660, 0x2f2d19cc), WTCP(0x76df4e30, 0x2f78431a),
    WTCP(0x76c12190, 0x2fc38d91), WTCP(0x76a2b047, 0x300ef89d), WTCP(0x7683fa1e, 0x305a83af),
    WTCP(0x7664fede, 0x30a62e34), WTCP(0x7645be51, 0x30f1f798), WTCP(0x76263842, 0x313ddf49),
    WTCP(0x76066c7e, 0x3189e4b1), WTCP(0x75e65ad1, 0x31d6073d), WTCP(0x75c60309, 0x32224657),
    WTCP(0x75a564f6, 0x326ea168), WTCP(0x75848067, 0x32bb17da), WTCP(0x7563552d, 0x3307a917),
    WTCP(0x7541e31a, 0x33545486), WTCP(0x75202a02, 0x33a1198e), WTCP(0x74fe29b8, 0x33edf798),
    WTCP(0x74dbe211, 0x343aee09), WTCP(0x74b952e3, 0x3487fc48), WTCP(0x74967c06, 0x34d521bb),
    WTCP(0x74735d51, 0x35225dc7), WTCP(0x744ff69f, 0x356fafcf), WTCP(0x742c47c9, 0x35bd173a),
    WTCP(0x740850ab, 0x360a9369), WTCP(0x73e41121, 0x365823c1), WTCP(0x73bf8909, 0x36a5c7a4),
    WTCP(0x739ab842, 0x36f37e75), WTCP(0x73759eab, 0x37414796), WTCP(0x73503c26, 0x378f2268),
    WTCP(0x732a9095, 0x37dd0e4c), WTCP(0x73049bda, 0x382b0aa4), WTCP(0x72de5ddb, 0x387916d0),
    WTCP(0x72b7d67d, 0x38c73230), WTCP(0x729105a6, 0x39155c24), WTCP(0x7269eb3f, 0x3963940c),
    WTCP(0x72428730, 0x39b1d946), WTCP(0x721ad964, 0x3a002b31), WTCP(0x71f2e1c5, 0x3a4e892c),
    WTCP(0x71caa042, 0x3a9cf296), WTCP(0x71a214c7, 0x3aeb66cc), WTCP(0x71793f43, 0x3b39e52c),
    WTCP(0x71501fa6, 0x3b886d14), WTCP(0x7126b5e3, 0x3bd6fde1), WTCP(0x70fd01eb, 0x3c2596f1),
    WTCP(0x70d303b2, 0x3c74379f), WTCP(0x70a8bb2e, 0x3cc2df49), WTCP(0x707e2855, 0x3d118d4c),
    WTCP(0x70534b1e, 0x3d604103), WTCP(0x70282381, 0x3daef9cc), WTCP(0x6ffcb17a, 0x3dfdb702),
    WTCP(0x6fd0f504, 0x3e4c7800), WTCP(0x6fa4ee1a, 0x3e9b3c25), WTCP(0x6f789cbb, 0x3eea02ca),
    WTCP(0x6f4c00e5, 0x3f38cb4b), WTCP(0x6f1f1a9a, 0x3f879505), WTCP(0x6ef1e9da, 0x3fd65f53),
    WTCP(0x6ec46ea9, 0x40252990), WTCP(0x6e96a90b, 0x4073f318), WTCP(0x6e689905, 0x40c2bb46),
    WTCP(0x6e3a3e9d, 0x41118176), WTCP(0x6e0b99dd, 0x41604504), WTCP(0x6ddcaacc, 0x41af054a),
    WTCP(0x6dad7177, 0x41fdc1a5), WTCP(0x6d7dede8, 0x424c7970), WTCP(0x6d4e202e, 0x429b2c06),
    WTCP(0x6d1e0855, 0x42e9d8c4), WTCP(0x6ceda66f, 0x43387f05), WTCP(0x6cbcfa8d, 0x43871e26),
    WTCP(0x6c8c04c0, 0x43d5b581), WTCP(0x6c5ac51d, 0x44244474), WTCP(0x6c293bb8, 0x4472ca5a),
    WTCP(0x6bf768a8, 0x44c14690), WTCP(0x6bc54c06, 0x450fb873), WTCP(0x6b92e5e9, 0x455e1f5f),
    WTCP(0x6b60366c, 0x45ac7ab2), WTCP(0x6b2d3dab, 0x45fac9c8), WTCP(0x6af9fbc2, 0x46490bff),
    WTCP(0x6ac670d1, 0x469740b5), WTCP(0x6a929cf6, 0x46e56747), WTCP(0x6a5e8053, 0x47337f13),
    WTCP(0x6a2a1b0a, 0x47818779), WTCP(0x69f56d3e, 0x47cf7fd6), WTCP(0x69c07715, 0x481d678a),
    WTCP(0x698b38b4, 0x486b3df3), WTCP(0x6955b243, 0x48b90272), WTCP(0x691fe3ec, 0x4906b466),
    WTCP(0x68e9cdd8, 0x49545330), WTCP(0x68b37033, 0x49a1de30), WTCP(0x687ccb29, 0x49ef54c8),
    WTCP(0x6845dee9, 0x4a3cb657), WTCP(0x680eaba3, 0x4a8a0242), WTCP(0x67d73187, 0x4ad737e9),
    WTCP(0x679f70c7, 0x4b2456af), WTCP(0x67676997, 0x4b715df7), WTCP(0x672f1c2b, 0x4bbe4d25),
    WTCP(0x66f688ba, 0x4c0b239c), WTCP(0x66bdaf7b, 0x4c57e0c2), WTCP(0x668490a6, 0x4ca483fa),
    WTCP(0x664b2c76, 0x4cf10cac), WTCP(0x66118326, 0x4d3d7a3b), WTCP(0x65d794f3, 0x4d89cc0f),
    WTCP(0x659d621a, 0x4dd6018f), WTCP(0x6562eada, 0x4e221a22), WTCP(0x65282f74, 0x4e6e1530),
    WTCP(0x64ed302b, 0x4eb9f222), WTCP(0x64b1ed40, 0x4f05b061), WTCP(0x647666f8, 0x4f514f57),
    WTCP(0x643a9d99, 0x4f9cce6f), WTCP(0x63fe916a, 0x4fe82d13), WTCP(0x63c242b2, 0x50336aaf),
    WTCP(0x6385b1bc, 0x507e86b0), WTCP(0x6348ded1, 0x50c98082), WTCP(0x630bca3f, 0x51145793),
    WTCP(0x62ce7451, 0x515f0b51), WTCP(0x6290dd57, 0x51a99b2b), WTCP(0x625305a0, 0x51f40692),
    WTCP(0x6214ed7d, 0x523e4cf5), WTCP(0x61d69541, 0x52886dc5), WTCP(0x6197fd3e, 0x52d26875),
    WTCP(0x615925c9, 0x531c3c77), WTCP(0x611a0f39, 0x5365e93e), WTCP(0x60dab9e3, 0x53af6e3e),
    WTCP(0x609b2621, 0x53f8caed), WTCP(0x605b544c, 0x5441fec0), WTCP(0x601b44bf, 0x548b092e),
    WTCP(0x5fdaf7d5, 0x54d3e9ae), WTCP(0x5f9a6deb, 0x551c9fb7), WTCP(0x5f59a761, 0x55652ac3),
    WTCP(0x5f18a494, 0x55ad8a4d), WTCP(0x5ed765e6, 0x55f5bdcd), WTCP(0x5e95ebb8, 0x563dc4c1),
    WTCP(0x5e54366d, 0x56859ea3), WTCP(0x5e12466a, 0x56cd4af3), WTCP(0x5dd01c13, 0x5714c92d),
    WTCP(0x5d8db7cf, 0x575c18d0), WTCP(0x5d4b1a05, 0x57a3395e), WTCP(0x5d08431e, 0x57ea2a56),
    WTCP(0x5cc53384, 0x5830eb3a), WTCP(0x5c81eba0, 0x58777b8e), WTCP(0x5c3e6bdf, 0x58bddad5),
    WTCP(0x5bfab4af, 0x59040893), WTCP(0x5bb6c67c, 0x594a044f), WTCP(0x5b72a1b6, 0x598fcd8e),
    WTCP(0x5b2e46ce, 0x59d563d9), WTCP(0x5ae9b634, 0x5a1ac6b8), WTCP(0x5aa4f05a, 0x5a5ff5b5),
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow1024[] = {
    WTCP(0x7fffffa4, 0x0009962f), WTCP(0x7fffff39, 0x000e16fb), WTCP(0x7ffffebf, 0x0011ea65),
    WTCP(0x7ffffe34, 0x0015750e), WTCP(0x7ffffd96, 0x0018dc74), WTCP(0x7ffffce5, 0x001c332e),
    WTCP(0x7ffffc1f, 0x001f83f5), WTCP(0x7ffffb43, 0x0022d59a), WTCP(0x7ffffa4f, 0x00262cc2),
    WTCP(0x7ffff942, 0x00298cc4), WTCP(0x7ffff81a, 0x002cf81f), WTCP(0x7ffff6d6, 0x003070c4),
    WTCP(0x7ffff573, 0x0033f840), WTCP(0x7ffff3f1, 0x00378fd9), WTCP(0x7ffff24d, 0x003b38a1),
    WTCP(0x7ffff085, 0x003ef381), WTCP(0x7fffee98, 0x0042c147), WTCP(0x7fffec83, 0x0046a2a8),
    WTCP(0x7fffea44, 0x004a9847), WTCP(0x7fffe7d8, 0x004ea2b7), WTCP(0x7fffe53f, 0x0052c283),
    WTCP(0x7fffe274, 0x0056f829), WTCP(0x7fffdf76, 0x005b4422), WTCP(0x7fffdc43, 0x005fa6dd),
    WTCP(0x7fffd8d6, 0x006420c8), WTCP(0x7fffd52f, 0x0068b249), WTCP(0x7fffd149, 0x006d5bc4),
    WTCP(0x7fffcd22, 0x00721d9a), WTCP(0x7fffc8b6, 0x0076f828), WTCP(0x7fffc404, 0x007bebca),
    WTCP(0x7fffbf06, 0x0080f8d9), WTCP(0x7fffb9bb, 0x00861fae), WTCP(0x7fffb41e, 0x008b609e),
    WTCP(0x7fffae2c, 0x0090bbff), WTCP(0x7fffa7e1, 0x00963224), WTCP(0x7fffa13a, 0x009bc362),
    WTCP(0x7fff9a32, 0x00a17009), WTCP(0x7fff92c5, 0x00a7386c), WTCP(0x7fff8af0, 0x00ad1cdc),
    WTCP(0x7fff82ad, 0x00b31da8), WTCP(0x7fff79f9, 0x00b93b21), WTCP(0x7fff70cf, 0x00bf7596),
    WTCP(0x7fff672a, 0x00c5cd57), WTCP(0x7fff5d05, 0x00cc42b1), WTCP(0x7fff525c, 0x00d2d5f3),
    WTCP(0x7fff4729, 0x00d9876c), WTCP(0x7fff3b66, 0x00e05769), WTCP(0x7fff2f10, 0x00e74638),
    WTCP(0x7fff221f, 0x00ee5426), WTCP(0x7fff148e, 0x00f58182), WTCP(0x7fff0658, 0x00fcce97),
    WTCP(0x7ffef776, 0x01043bb3), WTCP(0x7ffee7e2, 0x010bc923), WTCP(0x7ffed795, 0x01137733),
    WTCP(0x7ffec68a, 0x011b4631), WTCP(0x7ffeb4ba, 0x01233669), WTCP(0x7ffea21d, 0x012b4827),
    WTCP(0x7ffe8eac, 0x01337bb8), WTCP(0x7ffe7a61, 0x013bd167), WTCP(0x7ffe6533, 0x01444982),
    WTCP(0x7ffe4f1c, 0x014ce454), WTCP(0x7ffe3813, 0x0155a229), WTCP(0x7ffe2011, 0x015e834d),
    WTCP(0x7ffe070d, 0x0167880c), WTCP(0x7ffdecff, 0x0170b0b2), WTCP(0x7ffdd1df, 0x0179fd8b),
    WTCP(0x7ffdb5a2, 0x01836ee1), WTCP(0x7ffd9842, 0x018d0500), WTCP(0x7ffd79b3, 0x0196c035),
    WTCP(0x7ffd59ee, 0x01a0a0ca), WTCP(0x7ffd38e8, 0x01aaa70a), WTCP(0x7ffd1697, 0x01b4d341),
    WTCP(0x7ffcf2f2, 0x01bf25b9), WTCP(0x7ffccdee, 0x01c99ebd), WTCP(0x7ffca780, 0x01d43e99),
    WTCP(0x7ffc7f9e, 0x01df0597), WTCP(0x7ffc563d, 0x01e9f401), WTCP(0x7ffc2b51, 0x01f50a22),
    WTCP(0x7ffbfecf, 0x02004844), WTCP(0x7ffbd0ab, 0x020baeb1), WTCP(0x7ffba0da, 0x02173db4),
    WTCP(0x7ffb6f4f, 0x0222f596), WTCP(0x7ffb3bfd, 0x022ed6a1), WTCP(0x7ffb06d8, 0x023ae11f),
    WTCP(0x7ffacfd3, 0x02471558), WTCP(0x7ffa96e0, 0x02537397), WTCP(0x7ffa5bf2, 0x025ffc25),
    WTCP(0x7ffa1efc, 0x026caf4a), WTCP(0x7ff9dfee, 0x02798d4f), WTCP(0x7ff99ebb, 0x0286967c),
    WTCP(0x7ff95b55, 0x0293cb1b), WTCP(0x7ff915ab, 0x02a12b72), WTCP(0x7ff8cdaf, 0x02aeb7cb),
    WTCP(0x7ff88351, 0x02bc706d), WTCP(0x7ff83682, 0x02ca559f), WTCP(0x7ff7e731, 0x02d867a9),
    WTCP(0x7ff7954e, 0x02e6a6d2), WTCP(0x7ff740c8, 0x02f51361), WTCP(0x7ff6e98e, 0x0303ad9c),
    WTCP(0x7ff68f8f, 0x031275ca), WTCP(0x7ff632ba, 0x03216c30), WTCP(0x7ff5d2fb, 0x03309116),
    WTCP(0x7ff57042, 0x033fe4bf), WTCP(0x7ff50a7a, 0x034f6773), WTCP(0x7ff4a192, 0x035f1975),
    WTCP(0x7ff43576, 0x036efb0a), WTCP(0x7ff3c612, 0x037f0c78), WTCP(0x7ff35353, 0x038f4e02),
    WTCP(0x7ff2dd24, 0x039fbfeb), WTCP(0x7ff26370, 0x03b06279), WTCP(0x7ff1e623, 0x03c135ed),
    WTCP(0x7ff16527, 0x03d23a8b), WTCP(0x7ff0e067, 0x03e37095), WTCP(0x7ff057cc, 0x03f4d84e),
    WTCP(0x7fefcb40, 0x040671f7), WTCP(0x7fef3aad, 0x04183dd3), WTCP(0x7feea5fa, 0x042a3c22),
    WTCP(0x7fee0d11, 0x043c6d25), WTCP(0x7fed6fda, 0x044ed11d), WTCP(0x7fecce3d, 0x04616849),
    WTCP(0x7fec2821, 0x047432eb), WTCP(0x7feb7d6c, 0x04873140), WTCP(0x7feace07, 0x049a6388),
    WTCP(0x7fea19d6, 0x04adca01), WTCP(0x7fe960c0, 0x04c164ea), WTCP(0x7fe8a2aa, 0x04d53481),
    WTCP(0x7fe7df79, 0x04e93902), WTCP(0x7fe71712, 0x04fd72aa), WTCP(0x7fe6495a, 0x0511e1b6),
    WTCP(0x7fe57634, 0x05268663), WTCP(0x7fe49d83, 0x053b60eb), WTCP(0x7fe3bf2b, 0x05507189),
    WTCP(0x7fe2db0f, 0x0565b879), WTCP(0x7fe1f110, 0x057b35f4), WTCP(0x7fe10111, 0x0590ea35),
    WTCP(0x7fe00af3, 0x05a6d574), WTCP(0x7fdf0e97, 0x05bcf7ea), WTCP(0x7fde0bdd, 0x05d351cf),
    WTCP(0x7fdd02a6, 0x05e9e35c), WTCP(0x7fdbf2d2, 0x0600acc8), WTCP(0x7fdadc40, 0x0617ae48),
    WTCP(0x7fd9becf, 0x062ee814), WTCP(0x7fd89a5e, 0x06465a62), WTCP(0x7fd76eca, 0x065e0565),
    WTCP(0x7fd63bf1, 0x0675e954), WTCP(0x7fd501b0, 0x068e0662), WTCP(0x7fd3bfe4, 0x06a65cc3),
    WTCP(0x7fd2766a, 0x06beecaa), WTCP(0x7fd1251e, 0x06d7b648), WTCP(0x7fcfcbda, 0x06f0b9d1),
    WTCP(0x7fce6a7a, 0x0709f775), WTCP(0x7fcd00d8, 0x07236f65), WTCP(0x7fcb8ecf, 0x073d21d2),
    WTCP(0x7fca1439, 0x07570eea), WTCP(0x7fc890ed, 0x077136dd), WTCP(0x7fc704c7, 0x078b99da),
    WTCP(0x7fc56f9d, 0x07a6380d), WTCP(0x7fc3d147, 0x07c111a4), WTCP(0x7fc2299e, 0x07dc26cc),
    WTCP(0x7fc07878, 0x07f777b1), WTCP(0x7fbebdac, 0x0813047d), WTCP(0x7fbcf90f, 0x082ecd5b),
    WTCP(0x7fbb2a78, 0x084ad276), WTCP(0x7fb951bc, 0x086713f7), WTCP(0x7fb76eaf, 0x08839206),
    WTCP(0x7fb58126, 0x08a04ccb), WTCP(0x7fb388f4, 0x08bd446e), WTCP(0x7fb185ee, 0x08da7915),
    WTCP(0x7faf77e5, 0x08f7eae7), WTCP(0x7fad5ead, 0x09159a09), WTCP(0x7fab3a17, 0x0933869f),
    WTCP(0x7fa909f6, 0x0951b0cd), WTCP(0x7fa6ce1a, 0x097018b7), WTCP(0x7fa48653, 0x098ebe7f),
    WTCP(0x7fa23273, 0x09ada248), WTCP(0x7f9fd249, 0x09ccc431), WTCP(0x7f9d65a4, 0x09ec245b),
    WTCP(0x7f9aec53, 0x0a0bc2e7), WTCP(0x7f986625, 0x0a2b9ff3), WTCP(0x7f95d2e7, 0x0a4bbb9e),
    WTCP(0x7f933267, 0x0a6c1604), WTCP(0x7f908472, 0x0a8caf43), WTCP(0x7f8dc8d5, 0x0aad8776),
    WTCP(0x7f8aff5c, 0x0ace9eb9), WTCP(0x7f8827d3, 0x0aeff526), WTCP(0x7f854204, 0x0b118ad8),
    WTCP(0x7f824dbb, 0x0b335fe6), WTCP(0x7f7f4ac3, 0x0b557469), WTCP(0x7f7c38e4, 0x0b77c879),
    WTCP(0x7f7917e9, 0x0b9a5c2b), WTCP(0x7f75e79b, 0x0bbd2f97), WTCP(0x7f72a7c3, 0x0be042d0),
    WTCP(0x7f6f5828, 0x0c0395ec), WTCP(0x7f6bf892, 0x0c2728fd), WTCP(0x7f6888c9, 0x0c4afc16),
    WTCP(0x7f650894, 0x0c6f0f4a), WTCP(0x7f6177b9, 0x0c9362a8), WTCP(0x7f5dd5ff, 0x0cb7f642),
    WTCP(0x7f5a232a, 0x0cdcca26), WTCP(0x7f565f00, 0x0d01de63), WTCP(0x7f528947, 0x0d273307),
    WTCP(0x7f4ea1c2, 0x0d4cc81f), WTCP(0x7f4aa835, 0x0d729db7), WTCP(0x7f469c65, 0x0d98b3da),
    WTCP(0x7f427e13, 0x0dbf0a92), WTCP(0x7f3e4d04, 0x0de5a1e9), WTCP(0x7f3a08f9, 0x0e0c79e7),
    WTCP(0x7f35b1b4, 0x0e339295), WTCP(0x7f3146f8, 0x0e5aebfa), WTCP(0x7f2cc884, 0x0e82861a),
    WTCP(0x7f28361b, 0x0eaa60fd), WTCP(0x7f238f7c, 0x0ed27ca5), WTCP(0x7f1ed467, 0x0efad917),
    WTCP(0x7f1a049d, 0x0f237656), WTCP(0x7f151fdc, 0x0f4c5462), WTCP(0x7f1025e3, 0x0f75733d),
    WTCP(0x7f0b1672, 0x0f9ed2e6), WTCP(0x7f05f146, 0x0fc8735e), WTCP(0x7f00b61d, 0x0ff254a1),
    WTCP(0x7efb64b4, 0x101c76ae), WTCP(0x7ef5fcca, 0x1046d981), WTCP(0x7ef07e19, 0x10717d15),
    WTCP(0x7eeae860, 0x109c6165), WTCP(0x7ee53b5b, 0x10c7866a), WTCP(0x7edf76c4, 0x10f2ec1e),
    WTCP(0x7ed99a58, 0x111e9279), WTCP(0x7ed3a5d1, 0x114a7971), WTCP(0x7ecd98eb, 0x1176a0fc),
    WTCP(0x7ec77360, 0x11a30910), WTCP(0x7ec134eb, 0x11cfb1a1), WTCP(0x7ebadd44, 0x11fc9aa2),
    WTCP(0x7eb46c27, 0x1229c406), WTCP(0x7eade14c, 0x12572dbf), WTCP(0x7ea73c6c, 0x1284d7bc),
    WTCP(0x7ea07d41, 0x12b2c1ed), WTCP(0x7e99a382, 0x12e0ec42), WTCP(0x7e92aee7, 0x130f56a8),
    WTCP(0x7e8b9f2a, 0x133e010b), WTCP(0x7e847402, 0x136ceb59), WTCP(0x7e7d2d25, 0x139c157b),
    WTCP(0x7e75ca4c, 0x13cb7f5d), WTCP(0x7e6e4b2d, 0x13fb28e6), WTCP(0x7e66af7f, 0x142b1200),
    WTCP(0x7e5ef6f8, 0x145b3a92), WTCP(0x7e572150, 0x148ba281), WTCP(0x7e4f2e3b, 0x14bc49b4),
    WTCP(0x7e471d70, 0x14ed300f), WTCP(0x7e3eeea5, 0x151e5575), WTCP(0x7e36a18e, 0x154fb9c9),
    WTCP(0x7e2e35e2, 0x15815ced), WTCP(0x7e25ab56, 0x15b33ec1), WTCP(0x7e1d019e, 0x15e55f25),
    WTCP(0x7e14386e, 0x1617bdf9), WTCP(0x7e0b4f7d, 0x164a5b19), WTCP(0x7e02467e, 0x167d3662),
    WTCP(0x7df91d25, 0x16b04fb2), WTCP(0x7defd327, 0x16e3a6e2), WTCP(0x7de66837, 0x17173bce),
    WTCP(0x7ddcdc0a, 0x174b0e4d), WTCP(0x7dd32e53, 0x177f1e39), WTCP(0x7dc95ec6, 0x17b36b69),
    WTCP(0x7dbf6d17, 0x17e7f5b3), WTCP(0x7db558f9, 0x181cbcec), WTCP(0x7dab221f, 0x1851c0e9),
    WTCP(0x7da0c83c, 0x1887017d), WTCP(0x7d964b05, 0x18bc7e7c), WTCP(0x7d8baa2b, 0x18f237b6),
    WTCP(0x7d80e563, 0x19282cfd), WTCP(0x7d75fc5e, 0x195e5e20), WTCP(0x7d6aeed0, 0x1994caee),
    WTCP(0x7d5fbc6d, 0x19cb7335), WTCP(0x7d5464e6, 0x1a0256c2), WTCP(0x7d48e7ef, 0x1a397561),
    WTCP(0x7d3d453b, 0x1a70cede), WTCP(0x7d317c7c, 0x1aa86301), WTCP(0x7d258d65, 0x1ae03195),
    WTCP(0x7d1977aa, 0x1b183a63), WTCP(0x7d0d3afc, 0x1b507d30), WTCP(0x7d00d710, 0x1b88f9c5),
    WTCP(0x7cf44b97, 0x1bc1afe6), WTCP(0x7ce79846, 0x1bfa9f58), WTCP(0x7cdabcce, 0x1c33c7e0),
    WTCP(0x7ccdb8e4, 0x1c6d293f), WTCP(0x7cc08c39, 0x1ca6c337), WTCP(0x7cb33682, 0x1ce0958a),
    WTCP(0x7ca5b772, 0x1d1a9ff8), WTCP(0x7c980ebd, 0x1d54e240), WTCP(0x7c8a3c14, 0x1d8f5c21),
    WTCP(0x7c7c3f2e, 0x1dca0d56), WTCP(0x7c6e17bc, 0x1e04f59f), WTCP(0x7c5fc573, 0x1e4014b4),
    WTCP(0x7c514807, 0x1e7b6a53), WTCP(0x7c429f2c, 0x1eb6f633), WTCP(0x7c33ca96, 0x1ef2b80f),
    WTCP(0x7c24c9fa, 0x1f2eaf9e), WTCP(0x7c159d0d, 0x1f6adc98), WTCP(0x7c064383, 0x1fa73eb2),
    WTCP(0x7bf6bd11, 0x1fe3d5a3), WTCP(0x7be7096c, 0x2020a11e), WTCP(0x7bd7284a, 0x205da0d8),
    WTCP(0x7bc71960, 0x209ad483), WTCP(0x7bb6dc65, 0x20d83bd1), WTCP(0x7ba6710d, 0x2115d674),
    WTCP(0x7b95d710, 0x2153a41b), WTCP(0x7b850e24, 0x2191a476), WTCP(0x7b7415ff, 0x21cfd734),
    WTCP(0x7b62ee59, 0x220e3c02), WTCP(0x7b5196e9, 0x224cd28d), WTCP(0x7b400f67, 0x228b9a82),
    WTCP(0x7b2e578a, 0x22ca938a), WTCP(0x7b1c6f0b, 0x2309bd52), WTCP(0x7b0a55a1, 0x23491783),
    WTCP(0x7af80b07, 0x2388a1c4), WTCP(0x7ae58ef5, 0x23c85bbf), WTCP(0x7ad2e124, 0x2408451a),
    WTCP(0x7ac0014e, 0x24485d7c), WTCP(0x7aacef2e, 0x2488a48a), WTCP(0x7a99aa7e, 0x24c919e9),
    WTCP(0x7a8632f8, 0x2509bd3d), WTCP(0x7a728858, 0x254a8e29), WTCP(0x7a5eaa5a, 0x258b8c50),
    WTCP(0x7a4a98b9, 0x25ccb753), WTCP(0x7a365333, 0x260e0ed3), WTCP(0x7a21d983, 0x264f9271),
    WTCP(0x7a0d2b68, 0x269141cb), WTCP(0x79f8489e, 0x26d31c80), WTCP(0x79e330e4, 0x2715222f),
    WTCP(0x79cde3f8, 0x27575273), WTCP(0x79b8619a, 0x2799acea), WTCP(0x79a2a989, 0x27dc3130),
    WTCP(0x798cbb85, 0x281ededf), WTCP(0x7976974e, 0x2861b591), WTCP(0x79603ca5, 0x28a4b4e0),
    WTCP(0x7949ab4c, 0x28e7dc65), WTCP(0x7932e304, 0x292b2bb8), WTCP(0x791be390, 0x296ea270),
    WTCP(0x7904acb3, 0x29b24024), WTCP(0x78ed3e30, 0x29f6046b), WTCP(0x78d597cc, 0x2a39eed8),
    WTCP(0x78bdb94a, 0x2a7dff02), WTCP(0x78a5a270, 0x2ac2347c), WTCP(0x788d5304, 0x2b068eda),
    WTCP(0x7874cacb, 0x2b4b0dae), WTCP(0x785c098d, 0x2b8fb08a), WTCP(0x78430f11, 0x2bd47700),
    WTCP(0x7829db1f, 0x2c1960a1), WTCP(0x78106d7f, 0x2c5e6cfd), WTCP(0x77f6c5fb, 0x2ca39ba3),
    WTCP(0x77dce45c, 0x2ce8ec23), WTCP(0x77c2c86e, 0x2d2e5e0b), WTCP(0x77a871fa, 0x2d73f0e8),
    WTCP(0x778de0cd, 0x2db9a449), WTCP(0x777314b2, 0x2dff77b8), WTCP(0x77580d78, 0x2e456ac4),
    WTCP(0x773ccaeb, 0x2e8b7cf6), WTCP(0x77214cdb, 0x2ed1addb), WTCP(0x77059315, 0x2f17fcfb),
    WTCP(0x76e99d69, 0x2f5e69e2), WTCP(0x76cd6ba9, 0x2fa4f419), WTCP(0x76b0fda4, 0x2feb9b27),
    WTCP(0x7694532e, 0x30325e96), WTCP(0x76776c17, 0x30793dee), WTCP(0x765a4834, 0x30c038b5),
    WTCP(0x763ce759, 0x31074e72), WTCP(0x761f4959, 0x314e7eab), WTCP(0x76016e0b, 0x3195c8e6),
    WTCP(0x75e35545, 0x31dd2ca9), WTCP(0x75c4fedc, 0x3224a979), WTCP(0x75a66aab, 0x326c3ed8),
    WTCP(0x75879887, 0x32b3ec4d), WTCP(0x7568884b, 0x32fbb159), WTCP(0x754939d1, 0x33438d81),
    WTCP(0x7529acf4, 0x338b8045), WTCP(0x7509e18e, 0x33d3892a), WTCP(0x74e9d77d, 0x341ba7b1),
    WTCP(0x74c98e9e, 0x3463db5a), WTCP(0x74a906cd, 0x34ac23a7), WTCP(0x74883fec, 0x34f48019),
    WTCP(0x746739d8, 0x353cf02f), WTCP(0x7445f472, 0x3585736a), WTCP(0x74246f9c, 0x35ce0949),
    WTCP(0x7402ab37, 0x3616b14c), WTCP(0x73e0a727, 0x365f6af0), WTCP(0x73be6350, 0x36a835b5),
    WTCP(0x739bdf95, 0x36f11118), WTCP(0x73791bdd, 0x3739fc98), WTCP(0x7356180e, 0x3782f7b2),
    WTCP(0x7332d410, 0x37cc01e3), WTCP(0x730f4fc9, 0x38151aa8), WTCP(0x72eb8b24, 0x385e417e),
    WTCP(0x72c7860a, 0x38a775e1), WTCP(0x72a34066, 0x38f0b74d), WTCP(0x727eba24, 0x393a053e),
    WTCP(0x7259f331, 0x39835f30), WTCP(0x7234eb79, 0x39ccc49e), WTCP(0x720fa2eb, 0x3a163503),
    WTCP(0x71ea1977, 0x3a5fafda), WTCP(0x71c44f0c, 0x3aa9349e), WTCP(0x719e439d, 0x3af2c2ca),
    WTCP(0x7177f71a, 0x3b3c59d7), WTCP(0x71516978, 0x3b85f940), WTCP(0x712a9aaa, 0x3bcfa07e),
    WTCP(0x71038aa4, 0x3c194f0d), WTCP(0x70dc395e, 0x3c630464), WTCP(0x70b4a6cd, 0x3cacbfff),
    WTCP(0x708cd2e9, 0x3cf68155), WTCP(0x7064bdab, 0x3d4047e1), WTCP(0x703c670d, 0x3d8a131c),
    WTCP(0x7013cf0a, 0x3dd3e27e), WTCP(0x6feaf59c, 0x3e1db580), WTCP(0x6fc1dac1, 0x3e678b9b),
    WTCP(0x6f987e76, 0x3eb16449), WTCP(0x6f6ee0b9, 0x3efb3f01), WTCP(0x6f45018b, 0x3f451b3d),
    WTCP(0x6f1ae0eb, 0x3f8ef874), WTCP(0x6ef07edb, 0x3fd8d620), WTCP(0x6ec5db5d, 0x4022b3b9),
    WTCP(0x6e9af675, 0x406c90b7), WTCP(0x6e6fd027, 0x40b66c93), WTCP(0x6e446879, 0x410046c5),
    WTCP(0x6e18bf71, 0x414a1ec6), WTCP(0x6decd517, 0x4193f40d), WTCP(0x6dc0a972, 0x41ddc615),
    WTCP(0x6d943c8d, 0x42279455), WTCP(0x6d678e71, 0x42715e45), WTCP(0x6d3a9f2a, 0x42bb235f),
    WTCP(0x6d0d6ec5, 0x4304e31a), WTCP(0x6cdffd4f, 0x434e9cf1), WTCP(0x6cb24ad6, 0x4398505b),
    WTCP(0x6c84576b, 0x43e1fcd1), WTCP(0x6c56231c, 0x442ba1cd), WTCP(0x6c27adfd, 0x44753ec7),
    WTCP(0x6bf8f81e, 0x44bed33a), WTCP(0x6bca0195, 0x45085e9d), WTCP(0x6b9aca75, 0x4551e06b),
    WTCP(0x6b6b52d5, 0x459b581e), WTCP(0x6b3b9ac9, 0x45e4c52f), WTCP(0x6b0ba26b, 0x462e2717),
    WTCP(0x6adb69d3, 0x46777d52), WTCP(0x6aaaf11b, 0x46c0c75a), WTCP(0x6a7a385c, 0x470a04a9),
    WTCP(0x6a493fb3, 0x475334b9), WTCP(0x6a18073d, 0x479c5707), WTCP(0x69e68f17, 0x47e56b0c),
    WTCP(0x69b4d761, 0x482e7045), WTCP(0x6982e039, 0x4877662c), WTCP(0x6950a9c0, 0x48c04c3f),
    WTCP(0x691e341a, 0x490921f8), WTCP(0x68eb7f67, 0x4951e6d5), WTCP(0x68b88bcd, 0x499a9a51),
    WTCP(0x68855970, 0x49e33beb), WTCP(0x6851e875, 0x4a2bcb1f), WTCP(0x681e3905, 0x4a74476b),
    WTCP(0x67ea4b47, 0x4abcb04c), WTCP(0x67b61f63, 0x4b050541), WTCP(0x6781b585, 0x4b4d45c9),
    WTCP(0x674d0dd6, 0x4b957162), WTCP(0x67182883, 0x4bdd878c), WTCP(0x66e305b8, 0x4c2587c6),
    WTCP(0x66ada5a5, 0x4c6d7190), WTCP(0x66780878, 0x4cb5446a), WTCP(0x66422e60, 0x4cfcffd5),
    WTCP(0x660c1790, 0x4d44a353), WTCP(0x65d5c439, 0x4d8c2e64), WTCP(0x659f348e, 0x4dd3a08c),
    WTCP(0x656868c3, 0x4e1af94b), WTCP(0x6531610d, 0x4e623825), WTCP(0x64fa1da3, 0x4ea95c9d),
    WTCP(0x64c29ebb, 0x4ef06637), WTCP(0x648ae48d, 0x4f375477), WTCP(0x6452ef53, 0x4f7e26e1),
    WTCP(0x641abf46, 0x4fc4dcfb), WTCP(0x63e254a2, 0x500b7649), WTCP(0x63a9afa2, 0x5051f253),
    WTCP(0x6370d083, 0x5098509f), WTCP(0x6337b784, 0x50de90b3), WTCP(0x62fe64e3, 0x5124b218),
    WTCP(0x62c4d8e0, 0x516ab455), WTCP(0x628b13bc, 0x51b096f3), WTCP(0x625115b8, 0x51f6597b),
    WTCP(0x6216df18, 0x523bfb78), WTCP(0x61dc701f, 0x52817c72), WTCP(0x61a1c912, 0x52c6dbf5),
    WTCP(0x6166ea36, 0x530c198d), WTCP(0x612bd3d2, 0x535134c5), WTCP(0x60f0862d, 0x53962d2a),
    WTCP(0x60b50190, 0x53db024a), WTCP(0x60794644, 0x541fb3b1), WTCP(0x603d5494, 0x546440ef),
    WTCP(0x60012cca, 0x54a8a992), WTCP(0x5fc4cf33, 0x54eced2b), WTCP(0x5f883c1c, 0x55310b48),
    WTCP(0x5f4b73d2, 0x5575037c), WTCP(0x5f0e76a5, 0x55b8d558), WTCP(0x5ed144e5, 0x55fc806f),
    WTCP(0x5e93dee1, 0x56400452), WTCP(0x5e5644ec, 0x56836096), WTCP(0x5e187757, 0x56c694cf),
    WTCP(0x5dda7677, 0x5709a092), WTCP(0x5d9c429f, 0x574c8374), WTCP(0x5d5ddc24, 0x578f3d0d),
    WTCP(0x5d1f435d, 0x57d1ccf2), WTCP(0x5ce078a0, 0x581432bd), WTCP(0x5ca17c45, 0x58566e04),
    WTCP(0x5c624ea4, 0x58987e63), WTCP(0x5c22f016, 0x58da6372), WTCP(0x5be360f6, 0x591c1ccc),
    WTCP(0x5ba3a19f, 0x595daa0d), WTCP(0x5b63b26c, 0x599f0ad1), WTCP(0x5b2393ba, 0x59e03eb6),
    WTCP(0x5ae345e7, 0x5a214558), WTCP(0x5aa2c951, 0x5a621e56),
};

/**
 * \brief Helper table containing the length, rasterand shape mapping to
 * individual window slope tables. [0: sine ][0: radix2 raster
 * ][ceil(log2(length)) length   4    .. 1024 ] [1: 10ms raster
 * ][ceil(log2(length)) length   3.25 ..  960 ] [2: 3/4 of radix 2
 * raster][ceil(log2(length)) length   3    ..  768 ] [1: KBD  ][0:
 * radix2 raster        ][ceil(log2(length)) length 128    .. 1024 ] [1: 10ms
 * raster          ][ceil(log2(length)) length 120    ..  960 ] [2:
 * 3/4 of radix 2 raster][ceil(log2(length)) length  96    ..  768 ]
 */
const FIXP_WTP *const windowSlopes[2][4][9] = {
    { /* Sine */
     {/* Radix 2 */
      NULL, SineWindow8, SineWindow16, SineWindow32, SineWindow64, SineWindow128, SineWindow256, SineWindow512,
      SineWindow1024},
     {      /* 10ms raster */
      NULL, /* 3.25 */
      NULL, /* 7.5 */
      NULL, NULL, NULL, SineWindow120, SineWindow240, SineWindow480, SineWindow960},
     {      /* 3/4 radix2 raster */
      NULL, /* 3 */
      NULL, /* 6 */
      SineWindow12, SineWindow24, SineWindow48, SineWindow96, SineWindow192, SineWindow384, SineWindow768},
     {
         /* 3/4 radix2 raster */
         NULL,
         NULL, /* 3 */
         NULL, /* 6 */
         SineWindow20,
         SineWindow40,
         NULL,
         SineWindow160,
         NULL,
         NULL,
     }},
    { /* KBD */
     {/* Radix 2 */
      NULL, KBDWindow128, KBDWindow256, SineWindow512, KBDWindow1024},
     {/* 10ms raster */
      NULL, KBDWindow120, NULL, SineWindow480, KBDWindow960},
     {                                  /* 3/4 radix2 raster */
      NULL, KBDWindow96, SineWindow192, /* This entry might be accessed for erred bit streams. */
      NULL, KBDWindow768},
     {NULL, NULL, NULL, NULL}}};

const FIXP_WTP *FDKgetWindowSlope(int32_t length, int32_t shape) {
    const FIXP_WTP *w = NULL;
    int32_t         raster, ld2_length;

    /* Get ld2 of length - 2 + 1
       -2: because first table entry is window of size 4
       +1: because we already include +1 because of ceil(log2(length)) */
    ld2_length = DFRACT_BITS - 1 - fNormz((int32_t)length) - 1;

    /* Extract sort of "eigenvalue" (the 4 left most bits) of length. */
    switch((length) >> (ld2_length - 2)) {
        case 0x8: /* radix 2 */
            raster = 0;
            ld2_length--; /* revert + 1 because of ceil(log2(length)) from above. */
            break;
        case 0xf: /* 10 ms */
            raster = 1;
            break;
        case 0xc: /* 3/4 of radix 2 */
            raster = 2;
            break;
        default:
            raster = 0;
            break;
    }

    /* The table for sine windows (shape == 0) is 4 entries longer. */
    if(shape == 1) { ld2_length -= 4; }

    /* Look up table */
    w = windowSlopes[shape & 1][raster][ld2_length];

    assert(w != NULL);

    return w;
}

/*
 * QMF filter and twiddle tables
 */

#define QTCFL(x) FL2FXCONST_SGL(x)

/*!
  \name QMF
  \brief  QMF-Table
          64 channels, N = 640, optimized by PE 010516

  The coeffs are rearranged compared with the reference in the following
  way, exploiting symmetry :
  sbr_qmf_64[5] = p_64_640_qmf[0];
  sbr_qmf_64[6] = p_64_640_qmf[128];
  sbr_qmf_64[7] = p_64_640_qmf[256];
  sbr_qmf_64[8] = p_64_640_qmf[384];
  sbr_qmf_64[9] = p_64_640_qmf[512];

  sbr_qmf_64[10] = p_64_640_qmf[1];
  sbr_qmf_64[11] = p_64_640_qmf[129];
  sbr_qmf_64[12] = p_64_640_qmf[257];
  sbr_qmf_64[13] = p_64_640_qmf[385];
  sbr_qmf_64[14] = p_64_640_qmf[513];
  .
  .
  .
  sbr_qmf_64_640_qmf[315] = p_64_640_qmf[62];
  sbr_qmf_64_640_qmf[316] = p_64_640_qmf[190];
  sbr_qmf_64_640_qmf[317] = p_64_640_qmf[318];
  sbr_qmf_64_640_qmf[318] = p_64_640_qmf[446];
  sbr_qmf_64_640_qmf[319] = p_64_640_qmf[574];

  sbr_qmf_64_640_qmf[320] = p_64_640_qmf[63];
  sbr_qmf_64_640_qmf[321] = p_64_640_qmf[191];
  sbr_qmf_64_640_qmf[322] = p_64_640_qmf[319];
  sbr_qmf_64_640_qmf[323] = p_64_640_qmf[447];
  sbr_qmf_64_640_qmf[324] = p_64_640_qmf[575];

  sbr_qmf_64_640_qmf[319] = p_64_640_qmf[64];
  sbr_qmf_64_640_qmf[318] = p_64_640_qmf[192];
  sbr_qmf_64_640_qmf[317] = p_64_640_qmf[320];
  sbr_qmf_64_640_qmf[316] = p_64_640_qmf[448];
  sbr_qmf_64_640_qmf[315] = p_64_640_qmf[576];

  sbr_qmf_64_640_qmf[314] = p_64_640_qmf[65];
  sbr_qmf_64_640_qmf[313] = p_64_640_qmf[193];
  sbr_qmf_64_640_qmf[312] = p_64_640_qmf[321];
  sbr_qmf_64_640_qmf[311] = p_64_640_qmf[449];
  sbr_qmf_64_640_qmf[310] = p_64_640_qmf[577];
  .
  .
  .
  sbr_qmf_64[9] = p_64_640_qmf[126]
  sbr_qmf_64[8] = p_64_640_qmf[254];
  sbr_qmf_64[7] = p_64_640_qmf[382];
  sbr_qmf_64[6] = p_64_640_qmf[510];
  sbr_qmf_64[5] = p_64_640_qmf[638];

  sbr_qmf_64[4] = p_64_640_qmf[127]
  sbr_qmf_64[3] = p_64_640_qmf[255];
  sbr_qmf_64[2] = p_64_640_qmf[383];
  sbr_qmf_64[1] = p_64_640_qmf[511];
  sbr_qmf_64[0] = p_64_640_qmf[639];

  Max sum of all FIR filter absolute coefficients is: 0x7FF5B201
  thus, the filter output is not required to be scaled.

  \showinitializer
*/
RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_PFT qmf_pfilt120[] = {
    0,     435,   11834, -11834, -435,  -16,   643,   13807, -9906, -258,  -20,    876,   15786, -8053,
    -113,  -23,   1129,  17736,  -6304, 1,     -25,   1395,  19615, -4687, 85,     -25,   1663,  21386,
    -3218, 141,   -22,   1920,   23011, -1913, 173,   -15,   2151,  24452, -781,   185,   -3,    2339,
    25680, 174,   180,   13,     2468,  26667, 953,   165,   34,    2517,  27387,  1560,  142,   59,
    2469,  27827, 2007,  115,    86,    2305,  27975, 2305,  86,    -435,  -11834, 11834, 435,   0,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_PFT qmf_pfilt200[] = {
    0,     435,   11834, -11834, -435,  -16,   556,   13015, -10670, -325,  -17,    687,   14203, -9528, -226,  -19,
    827,   15392, -8416, -139,   -21,   975,   16572, -7340, -64,    -23,   1129,   17736, -6304, 1,     -25,   1288,
    18874, -5317, 55,    -26,    1449,  19979, -4381, 98,    -25,    1610,  21042,  -3499, 132,   -24,   1768,  22055,
    -2676, 156,   -22,   1920,   23011, -1913, 173,   -18,   2063,   23900, -1213,  182,   -13,   2192,  24716, -576,
    185,   -6,    2306,  25453,  -3,    182,   3,     2399,  26105,  506,   176,    13,    2468,  26667, 953,   165,
    25,    2508,  27132, 1337,   152,   39,    2516,  27498, 1662,   137,   53,     2487,  27761, 1930,  120,   69,
    2418,  27921, 2143,  103,    86,    2305,  27975, 2305,  86,     -435,  -11834, 11834, 435,   0,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_QTW qmf_phaseshift_cos40[] = {
    32751, 32618,  32352,  31954,  31426,  30770,  29989,  29086,  28065,  26929,  25683,  24333,  22884, 21342,
    19713, 18004,  16221,  14373,  12465,  10508,  8507,   6472,   4410,   2330,   241,    -1849,  -3931, -5998,
    -8040, -10049, -12018, -13937, -15800, -17599, -19326, -20974, -22537, -24008, -25381, -26651,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_QTW qmf_phaseshift_sin40[] = {
    1045,  3132,  5205,  7258,  9281,  11266, 13205, 15091, 16915, 18670, 20350, 21946, 23453, 24865,
    26175, 27379, 28471, 29448, 30304, 31038, 31645, 32123, 32470, 32685, 32767, 32716, 32531, 32214,
    31766, 31189, 30485, 29656, 28707, 27641, 26463, 25176, 23788, 22302, 20726, 19065,
};

/* This filter is scaled (0.8*pfilt) */
RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_PFT qmf_pfilt400[] = {
    0,   348,  9468,  -9468, -348, -15, 395,  9939,  -9000, -303, -13,  445,   10412, -8536, -260,
    -13, 496,  10887, -8076, -219, -13, 550,  11363, -7622, -181, -14,  605,   11838, -7174, -145,
    -15, 662,  12313, -6733, -111, -16, 720,  12787, -6298, -80,  -17,  780,   13258, -5872, -51,
    -18, 841,  13725, -5453, -24,  -19, 903,  14189, -5044, 1,    -19,  967,   14647, -4644, 23,
    -20, 1031, 15099, -4254, 44,   -20, 1095, 15545, -3874, 62,   -20,  1159,  15983, -3505, 78,
    -20, 1224, 16413, -3146, 93,   -20, 1288, 16834, -2800, 105,  -20,  1352,  17244, -2464, 116,
    -19, 1414, 17644, -2141, 125,  -19, 1476, 18032, -1830, 132,  -17,  1536,  18409, -1530, 138,
    -16, 1594, 18771, -1244, 143,  -14, 1650, 19120, -970,  146,  -12,  1703,  19454, -709,  147,
    -10, 1754, 19773, -461,  148,  -8,  1801, 20077, -225,  147,  -4,   1845,  20363, -3,    146,
    -1,  1884, 20632, 208,   143,  2,   1919, 20884, 405,   141,  6,    1949,  21118, 590,   136,
    11,  1974, 21334, 762,   132,  15,  1993, 21529, 922,   127,  20,   2006,  21706, 1070,  121,
    25,  2013, 21862, 1206,  115,  31,  2013, 21998, 1330,  109,  36,   2006,  22115, 1443,  103,
    43,  1990, 22209, 1544,  96,   49,  1967, 22284, 1635,  89,   55,   1935,  22337, 1715,  82,
    62,  1894, 22368, 1784,  76,   69,  1844, 22380, 1844,  69,   -348, -9468, 9468,  348,   0,
};
const FIXP_QTW qmf_phaseshift_cos16[] = {
    32706, 32214, 31238, 29792, 27897, 25583, 22884, 19841, 16500, 12910, 9127, 5205, 1206, -2811, -6787, -10660,
};
const FIXP_QTW qmf_phaseshift_sin16[] = {
    2009, 5998, 9896, 13646, 17190, 20475, 23453, 26078, 28311, 30118, 31471, 32352, 32746, 32647, 32058, 30986,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_PFT qmf_pfilt240[] = {
    /* FP filter implementation */
    0,   290,  7890,  -7890, -290, -11,  357,   8545,  -7242, -228, -11, 428,  9205,  -6604, -172,
    -12, 504,  9865,  -5978, -121, -13,  584,   10524, -5369, -75,  -14, 667,  11178, -4776, -35,
    -16, 753,  11824, -4203, 1,    -16,  841,   12458, -3652, 31,   -17, 930,  13077, -3125, 56,
    -17, 1020, 13678, -2622, 77,   -17,  1109,  14257, -2146, 94,   -16, 1196, 14812, -1696, 106,
    -15, 1280, 15341, -1275, 115,  -12,  1360,  15837, -884,  121,  -10, 1434, 16302, -521,  123,
    -6,  1501, 16731, -187,  123,  -2,   1559,  17120, 116,   120,  3,   1608, 17470, 390,   116,
    9,   1645, 17778, 635,   110,  15,   1669,  18041, 851,   103,  23,  1678, 18258, 1040,  94,
    30,  1671, 18429, 1202,  86,   39,   1646,  18551, 1338,  76,   48,  1602, 18625, 1449,  67,
    57,  1537, 18650, 1537,  57,   -290, -7890, 7890,  290,   0,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_QTW qmf_phaseshift_cos24[] = {
    32735, 32470, 31942, 31156, 30118, 28835, 27320, 25583, 23640, 21504,  19195,  16731,
    14131, 11417, 8610,  5734,  2811,  -134,  -3078, -5998, -8869, -11668, -14373, -16961,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_QTW qmf_phaseshift_sin24[] = {
    1474,  4410,  7310,  10151, 12910, 15565, 18093, 20475, 22692, 24724, 26557, 28175,
    29564, 30715, 31617, 32262, 32647, 32767, 32623, 32214, 31545, 30620, 29448, 28037,
};

/* qmf_pfilt640 is used with stride 2 instead of qmf_pfilt320[] */

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_QTW qmf_phaseshift_cos32[] = {
    32746, 32568, 32214, 31686, 30986,  30118,  29086,  27897,  26557,  25073,  23453,
    21706, 19841, 17869, 15800, 13646,  11417,  9127,   6787,   4410,   2009,   -402,
    -2811, -5205, -7571, -9896, -12167, -14373, -16500, -18538, -20475, -22302,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_QTW qmf_phaseshift_sin32[] = {
    1206,  3612,  5998,  8351,  10660, 12910, 15091, 17190, 19195, 21097, 22884, 24548, 26078, 27467, 28707, 29792,
    30715, 31471, 32058, 32470, 32706, 32766, 32647, 32352, 31881, 31238, 30425, 29448, 28311, 27020, 25583, 24008,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_QTW qmf_phaseshift_cos_downsamp32[] = {
    32729,  32413,  31786,  30853,  29622,  28106,  26320,  24279,  22006,  19520,  16846,
    14010,  11039,  7962,   4808,   1608,   -1608,  -4808,  -7962,  -11039, -14010, -16846,
    -19520, -22006, -24279, -26320, -28106, -29622, -30853, -31786, -32413, -32729,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_QTW qmf_phaseshift_sin_downsamp32[] = {
    1608,  4808,  7962,  11039, 14010, 16846, 19520, 22006, 24279, 26320, 28106, 29622, 30853, 31786, 32413, 32729,
    32729, 32413, 31786, 30853, 29622, 28106, 26320, 24279, 22006, 19520, 16846, 14010, 11039, 7962,  4808,  1608,
};
RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_PFT qmf_pfilt640[] = {
    0,   435,  11834, -11834, -435, -18, 472,  12202, -11469, -399, -18,  509,    12571, -11104, -365,
    -16, 548,  12941, -10742, -331, -16, 588,  13311, -10382, -299, -16,  629,    13683, -10024, -268,
    -17, 670,  14055, -9669,  -238, -17, 713,  14426, -9317,  -209, -18,  756,    14798, -8967,  -181,
    -19, 800,  15169, -8621,  -155, -19, 845,  15540, -8279,  -129, -20,  891,    15910, -7940,  -105,
    -21, 937,  16278, -7604,  -81,  -21, 985,  16646, -7273,  -59,  -22,  1032,   17011, -6946,  -38,
    -23, 1081, 17374, -6623,  -18,  -23, 1129, 17736, -6304,  1,    -24,  1179,   18095, -5991,  19,
    -24, 1228, 18451, -5681,  36,   -25, 1278, 18804, -5377,  52,   -25,  1328,   19154, -5078,  66,
    -25, 1378, 19501, -4783,  80,   -26, 1429, 19844, -4494,  93,   -25,  1479,   20182, -4211,  105,
    -26, 1530, 20517, -3932,  116,  -26, 1580, 20847, -3660,  126,  -25,  1630,   21172, -3393,  135,
    -25, 1680, 21493, -3131,  144,  -25, 1729, 21808, -2876,  151,  -24,  1778,   22117, -2626,  158,
    -24, 1826, 22421, -2382,  163,  -23, 1873, 22719, -2144,  168,  -22,  1920,   23011, -1913,  173,
    -21, 1966, 23296, -1687,  176,  -19, 2010, 23575, -1467,  179,  -18,  2054,   23847, -1254,  182,
    -17, 2096, 24111, -1047,  183,  -15, 2137, 24369, -846,   184,  -13,  2177,   24619, -652,   185,
    -11, 2215, 24861, -463,   185,  -9,  2251, 25096, -281,   184,  -7,   2286,   25322, -105,   183,
    -5,  2319, 25540, 65,     182,  -2,  2349, 25750, 228,    180,  0,    2378,   25951, 385,    178,
    4,   2404, 26144, 536,    175,  7,   2428, 26327, 682,    172,  10,   2449,   26502, 820,    168,
    13,  2468, 26667, 953,    165,  17,  2484, 26823, 1080,   161,  20,   2497,   26969, 1201,   157,
    24,  2507, 27107, 1315,   153,  28,  2514, 27234, 1425,   148,  32,   2517,   27351, 1528,   143,
    37,  2517, 27459, 1625,   138,  41,  2514, 27556, 1717,   134,  46,   2507,   27644, 1804,   128,
    51,  2496, 27721, 1885,   123,  55,  2482, 27788, 1960,   118,  60,   2463,   27846, 2030,   113,
    65,  2440, 27892, 2095,   107,  70,  2413, 27928, 2155,   102,  75,   2381,   27954, 2210,   97,
    81,  2346, 27970, 2260,   91,   86,  2305, 27975, 2305,   86,   -435, -11834, 11834, 435,    0,
};
/* This variant of the table above is used on platforms, that have vectorized
   access to the table reading 4 filter sets (each of 5 coefficients) in a
   block. Format: 1st row    flt[0] of 4 sets (e.g. set 0, 1, 2, 3) 2nd row
   flt[1] of 4 sets (e.g. set 0, 1, 2, 3) 3rd row    flt[2] of 4 sets (e.g. set
   0, 1, 2, 3) 4th row    flt[3] of 4 sets (e.g. set 0, 1, 2, 3) 5th row
   flt[4] of 4 sets (e.g. set 0, 1, 2, 3) There are 32 blocks of 20
   coefficients, in total 640. Each of the rows must be at least 64-bit aligned
   (see: RAM_ALIGN).
*/
RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_PFT qmf_pfilt640_vector[] = {
    /*------------- 1 .. 4 ---------------*/
    -18,
    -18,
    -16,
    -16,
    472,
    509,
    548,
    588,
    12202,
    12571,
    12941,
    13311,
    -11469,
    -11104,
    -10742,
    -10382,
    -399,
    -365,
    -331,
    -299,
    /*------------- 5 .. 8 ---------------*/
    -16,
    -17,
    -17,
    -18,
    629,
    670,
    713,
    756,
    13683,
    14055,
    14426,
    14798,
    -10024,
    -9669,
    -9317,
    -8967,
    -268,
    -238,
    -209,
    -181,
    /*------------- 9 .. 12 ---------------*/
    -19,
    -19,
    -20,
    -21,
    800,
    845,
    891,
    937,
    15169,
    15540,
    15910,
    16278,
    -8621,
    -8279,
    -7940,
    -7604,
    -155,
    -129,
    -105,
    -81,
    /*------------- 13 .. 16 ---------------*/
    -21,
    -22,
    -23,
    -23,
    985,
    1032,
    1081,
    1129,
    16646,
    17011,
    17374,
    17736,
    -7273,
    -6946,
    -6623,
    -6304,
    -59,
    -38,
    -18,
    1,
    /*------------- 17 .. 20 ---------------*/
    -24,
    -24,
    -25,
    -25,
    1179,
    1228,
    1278,
    1328,
    18095,
    18451,
    18804,
    19154,
    -5991,
    -5681,
    -5377,
    -5078,
    19,
    36,
    52,
    66,
    /*------------- 21 .. 24 ---------------*/
    -25,
    -26,
    -25,
    -26,
    1378,
    1429,
    1479,
    1530,
    19501,
    19844,
    20182,
    20517,
    -4783,
    -4494,
    -4211,
    -3932,
    80,
    93,
    105,
    116,
    /*------------- 25 .. 28 ---------------*/
    -26,
    -25,
    -25,
    -25,
    1580,
    1630,
    1680,
    1729,
    20847,
    21172,
    21493,
    21808,
    -3660,
    -3393,
    -3131,
    -2876,
    126,
    135,
    144,
    151,
    /*------------- 29 .. 32 ---------------*/
    -24,
    -24,
    -23,
    -22,
    1778,
    1826,
    1873,
    1920,
    22117,
    22421,
    22719,
    23011,
    -2626,
    -2382,
    -2144,
    -1913,
    158,
    163,
    168,
    173,
    /*------------- 33 .. 36 ---------------*/
    -21,
    -19,
    -18,
    -17,
    1966,
    2010,
    2054,
    2096,
    23296,
    23575,
    23847,
    24111,
    -1687,
    -1467,
    -1254,
    -1047,
    176,
    179,
    182,
    183,
    /*------------- 37 .. 40 ---------------*/
    -15,
    -13,
    -11,
    -9,
    2137,
    2177,
    2215,
    2251,
    24369,
    24619,
    24861,
    25096,
    -846,
    -652,
    -463,
    -281,
    184,
    185,
    185,
    184,
    /*------------- 41 .. 44 ---------------*/
    -7,
    -5,
    -2,
    0,
    2286,
    2319,
    2349,
    2378,
    25322,
    25540,
    25750,
    25951,
    -105,
    65,
    228,
    385,
    183,
    182,
    180,
    178,
    /*------------- 45 .. 48 ---------------*/
    4,
    7,
    10,
    13,
    2404,
    2428,
    2449,
    2468,
    26144,
    26327,
    26502,
    26667,
    536,
    682,
    820,
    953,
    175,
    172,
    168,
    165,
    /*------------- 49 .. 52 ---------------*/
    17,
    20,
    24,
    28,
    2484,
    2497,
    2507,
    2514,
    26823,
    26969,
    27107,
    27234,
    1080,
    1201,
    1315,
    1425,
    161,
    157,
    153,
    148,
    /*------------- 53 .. 56 ---------------*/
    32,
    37,
    41,
    46,
    2517,
    2517,
    2514,
    2507,
    27351,
    27459,
    27556,
    27644,
    1528,
    1625,
    1717,
    1804,
    143,
    138,
    134,
    128,
    /*------------- 57 .. 60 ---------------*/
    51,
    55,
    60,
    65,
    2496,
    2482,
    2463,
    2440,
    27721,
    27788,
    27846,
    27892,
    1885,
    1960,
    2030,
    2095,
    123,
    118,
    113,
    107,
    /*------------- 61 .. 64 ---------------*/
    70,
    75,
    81,
    86,
    2413,
    2381,
    2346,
    2305,
    27928,
    27954,
    27970,
    27975,
    2155,
    2210,
    2260,
    2305,
    102,
    97,
    91,
    86,
    /*------------- 63 .. 60 ---------------*/
    91,
    97,
    102,
    107,
    2260,
    2210,
    2155,
    2095,
    27970,
    27954,
    27928,
    27892,
    2346,
    2381,
    2413,
    2440,
    81,
    75,
    70,
    65,
    /*------------- 59 .. 56 ---------------*/
    113,
    118,
    123,
    128,
    2030,
    1960,
    1885,
    1804,
    27846,
    27788,
    27721,
    27644,
    2463,
    2482,
    2496,
    2507,
    60,
    55,
    51,
    46,
    /*------------- 55 .. 52 ---------------*/
    134,
    138,
    143,
    148,
    1717,
    1625,
    1528,
    1425,
    27556,
    27459,
    27351,
    27234,
    2514,
    2517,
    2517,
    2514,
    41,
    37,
    32,
    28,
    /*------------- 51 .. 48 ---------------*/
    153,
    157,
    161,
    165,
    1315,
    1201,
    1080,
    953,
    27107,
    26969,
    26823,
    26667,
    2507,
    2497,
    2484,
    2468,
    24,
    20,
    17,
    13,
    /*------------- 47 .. 44 ---------------*/
    168,
    172,
    175,
    178,
    820,
    682,
    536,
    385,
    26502,
    26327,
    26144,
    25951,
    2449,
    2428,
    2404,
    2378,
    10,
    7,
    4,
    0,
    /*------------- 43 .. 40 ---------------*/
    180,
    182,
    183,
    184,
    228,
    65,
    -105,
    -281,
    25750,
    25540,
    25322,
    25096,
    2349,
    2319,
    2286,
    2251,
    -2,
    -5,
    -7,
    -9,
    /*------------- 39 .. 36 ---------------*/
    185,
    185,
    184,
    183,
    -463,
    -652,
    -846,
    -1047,
    24861,
    24619,
    24369,
    24111,
    2215,
    2177,
    2137,
    2096,
    -11,
    -13,
    -15,
    -17,
    /*------------- 35 .. 32 ---------------*/
    182,
    179,
    176,
    173,
    -1254,
    -1467,
    -1687,
    -1913,
    23847,
    23575,
    23296,
    23011,
    2054,
    2010,
    1966,
    1920,
    -18,
    -19,
    -21,
    -22,
    /*------------- 31 .. 28 ---------------*/
    168,
    163,
    158,
    151,
    -2144,
    -2382,
    -2626,
    -2876,
    22719,
    22421,
    22117,
    21808,
    1873,
    1826,
    1778,
    1729,
    -23,
    -24,
    -24,
    -25,
    /*------------- 27 .. 24 ---------------*/
    144,
    135,
    126,
    116,
    -3131,
    -3393,
    -3660,
    -3932,
    21493,
    21172,
    20847,
    20517,
    1680,
    1630,
    1580,
    1530,
    -25,
    -25,
    -26,
    -26,
    /*------------- 23 .. 20 ---------------*/
    105,
    93,
    80,
    66,
    -4211,
    -4494,
    -4783,
    -5078,
    20182,
    19844,
    19501,
    19154,
    1479,
    1429,
    1378,
    1328,
    -25,
    -26,
    -25,
    -25,
    /*------------- 19 .. 16 ---------------*/
    52,
    36,
    19,
    1,
    -5377,
    -5681,
    -5991,
    -6304,
    18804,
    18451,
    18095,
    17736,
    1278,
    1228,
    1179,
    1129,
    -25,
    -24,
    -24,
    -23,
    /*------------- 15 .. 12 ---------------*/
    -18,
    -38,
    -59,
    -81,
    -6623,
    -6946,
    -7273,
    -7604,
    17374,
    17011,
    16646,
    16278,
    1081,
    1032,
    985,
    937,
    -23,
    -22,
    -21,
    -21,
    /*------------- 11 .. 8 ---------------*/
    -105,
    -129,
    -155,
    -181,
    -7940,
    -8279,
    -8621,
    -8967,
    15910,
    15540,
    15169,
    14798,
    891,
    845,
    800,
    756,
    -20,
    -19,
    -19,
    -18,
    /*------------- 7 .. 4 ---------------*/
    -209,
    -238,
    -268,
    -299,
    -9317,
    -9669,
    -10024,
    -10382,
    14426,
    14055,
    13683,
    13311,
    713,
    670,
    629,
    588,
    -17,
    -17,
    -16,
    -16,
    /*------------- 3 .. 0 ---------------*/
    -331,
    -365,
    -399,
    -435,
    -10742,
    -11104,
    -11469,
    -11834,
    12941,
    12571,
    12202,
    11834,
    548,
    509,
    472,
    435,
    -16,
    -18,
    -18,
    0,
};
RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_QTW qmf_phaseshift_cos64[] = {
    32758,  32679,  32522,  32286,  31972,  31581,  31114,  30572,  29957,  29269,  28511,  27684,  26791,
    25833,  24812,  23732,  22595,  21403,  20160,  18868,  17531,  16151,  14733,  13279,  11793,  10279,
    8740,   7180,   5602,   4011,   2411,   804,    -804,   -2411,  -4011,  -5602,  -7180,  -8740,  -10279,
    -11793, -13279, -14733, -16151, -17531, -18868, -20160, -21403, -22595, -23732, -24812, -25833, -26791,
    -27684, -28511, -29269, -29957, -30572, -31114, -31581, -31972, -32286, -32522, -32679, -32758,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_QTW qmf_phaseshift_sin64[] = {
    804,   2411,  4011,  5602,  7180,  8740,  10279, 11793, 13279, 14733, 16151, 17531, 18868, 20160, 21403, 22595,
    23732, 24812, 25833, 26791, 27684, 28511, 29269, 29957, 30572, 31114, 31581, 31972, 32286, 32522, 32679, 32758,
    32758, 32679, 32522, 32286, 31972, 31581, 31114, 30572, 29957, 29269, 28511, 27684, 26791, 25833, 24812, 23732,
    22595, 21403, 20160, 18868, 17531, 16151, 14733, 13279, 11793, 10279, 8740,  7180,  5602,  4011,  2411,  804,
};
/*
 * Low Delay QMF aka CLDFB
 */

#if defined(QMF_COEFF_16BIT)
    #define QTCFLLD(x)  FL2FXCONST_SGL(x / (float)(1 << QMF_CLDFB_PFT_SCALE))
    #define QTCFLLDT(x) FL2FXCONST_SGL(x)
#else
    #define QTCFLLD(x)  FL2FXCONST_DBL(x / (float)(1 << QMF_CLDFB_PFT_SCALE))
    #define QTCFLLDT(x) FL2FXCONST_DBL(x)
#endif

#ifndef LOW_POWER_SBR_ONLY
/*!
  \name QMF-Twiddle
  \brief QMF twiddle factors

  L=32, gain=2.0, angle = 0.75
*/
/* sin/cos (angle) / 2 */
const FIXP_QTW qmf_phaseshift_cos32_cldfb_ana[32] = {
    /* analysis twiddle table */
    QTCFLLDT(-7.071067e-01), QTCFLLDT(7.071070e-01), QTCFLLDT(7.071064e-01), QTCFLLDT(-7.071073e-01),
    QTCFLLDT(-7.071061e-01), QTCFLLDT(7.071076e-01), QTCFLLDT(7.071058e-01), QTCFLLDT(-7.071080e-01),
    QTCFLLDT(-7.071055e-01), QTCFLLDT(7.071083e-01), QTCFLLDT(7.071052e-01), QTCFLLDT(-7.071086e-01),
    QTCFLLDT(-7.071049e-01), QTCFLLDT(7.071089e-01), QTCFLLDT(7.071046e-01), QTCFLLDT(-7.071092e-01),
    QTCFLLDT(-7.071042e-01), QTCFLLDT(7.071095e-01), QTCFLLDT(7.071039e-01), QTCFLLDT(-7.071098e-01),
    QTCFLLDT(-7.071036e-01), QTCFLLDT(7.071101e-01), QTCFLLDT(7.071033e-01), QTCFLLDT(-7.071104e-01),
    QTCFLLDT(-7.071030e-01), QTCFLLDT(7.071107e-01), QTCFLLDT(7.071027e-01), QTCFLLDT(-7.071111e-01),
    QTCFLLDT(-7.071024e-01), QTCFLLDT(7.071114e-01), QTCFLLDT(7.071021e-01), QTCFLLDT(-7.071117e-01),
};

const FIXP_QTW qmf_phaseshift_cos32_cldfb_syn[32] = {
    /* synthesis twiddle table */
    QTCFLLDT(7.071067e-01), QTCFLLDT(-7.071070e-01), QTCFLLDT(-7.071064e-01), QTCFLLDT(7.071073e-01),
    QTCFLLDT(7.071061e-01), QTCFLLDT(-7.071076e-01), QTCFLLDT(-7.071058e-01), QTCFLLDT(7.071080e-01),
    QTCFLLDT(7.071055e-01), QTCFLLDT(-7.071083e-01), QTCFLLDT(-7.071052e-01), QTCFLLDT(7.071086e-01),
    QTCFLLDT(7.071049e-01), QTCFLLDT(-7.071089e-01), QTCFLLDT(-7.071046e-01), QTCFLLDT(7.071092e-01),
    QTCFLLDT(7.071042e-01), QTCFLLDT(-7.071095e-01), QTCFLLDT(-7.071039e-01), QTCFLLDT(7.071098e-01),
    QTCFLLDT(7.071036e-01), QTCFLLDT(-7.071101e-01), QTCFLLDT(-7.071033e-01), QTCFLLDT(7.071104e-01),
    QTCFLLDT(7.071030e-01), QTCFLLDT(-7.071107e-01), QTCFLLDT(-7.071027e-01), QTCFLLDT(7.071111e-01),
    QTCFLLDT(7.071024e-01), QTCFLLDT(-7.071114e-01), QTCFLLDT(-7.071021e-01), QTCFLLDT(7.071117e-01),
};

const FIXP_QTW qmf_phaseshift_sin32_cldfb[32] = {
    QTCFLLDT(7.071068e-01), QTCFLLDT(7.071065e-01), QTCFLLDT(-7.071072e-01), QTCFLLDT(-7.071062e-01),
    QTCFLLDT(7.071075e-01), QTCFLLDT(7.071059e-01), QTCFLLDT(-7.071078e-01), QTCFLLDT(-7.071056e-01),
    QTCFLLDT(7.071081e-01), QTCFLLDT(7.071053e-01), QTCFLLDT(-7.071084e-01), QTCFLLDT(-7.071050e-01),
    QTCFLLDT(7.071087e-01), QTCFLLDT(7.071047e-01), QTCFLLDT(-7.071090e-01), QTCFLLDT(-7.071044e-01),
    QTCFLLDT(7.071093e-01), QTCFLLDT(7.071041e-01), QTCFLLDT(-7.071096e-01), QTCFLLDT(-7.071038e-01),
    QTCFLLDT(7.071099e-01), QTCFLLDT(7.071034e-01), QTCFLLDT(-7.071103e-01), QTCFLLDT(-7.071031e-01),
    QTCFLLDT(7.071106e-01), QTCFLLDT(7.071028e-01), QTCFLLDT(-7.071109e-01), QTCFLLDT(-7.071025e-01),
    QTCFLLDT(7.071112e-01), QTCFLLDT(7.071022e-01), QTCFLLDT(-7.071115e-01), QTCFLLDT(-7.071019e-01),
};

/* twiddles for X=(8,16) band qmf are copied from float simpleplayer
 * implementation: qmf_phaseshift_cosX_cldfb_ana  =
 * QMFlib_twiddle3RealX_SBRLD_A qmf_phaseshift_cosX_cldfb_syn  =
 * -(QMFlib_twiddle3RealX_SBRLD_A) qmf_phaseshift_sinX_cldfb      =
 * QMFlib_twiddle3ImagX_SBRLD_A
 */

/* cos ((n + 0.5)*pi*angle/L) , order = 159, L=16 */
const FIXP_QTW qmf_phaseshift_cos16_cldfb_ana[16] = {
    QTCFLLDT(-0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812),
    QTCFLLDT(-0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812),
    QTCFLLDT(-0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812),
    QTCFLLDT(-0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812),
};

/* cos ((n + 0.5)*pi*angle/L) , order = 159, L=16 */
const FIXP_QTW qmf_phaseshift_cos16_cldfb_syn[16] = {
    QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(0.7071067812),
    QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(0.7071067812),
    QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(0.7071067812),
    QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(0.7071067812),
};

/* sin ((n + 0.5)*pi*angle/L) , order = 159, L=16 */
const FIXP_QTW qmf_phaseshift_sin16_cldfb[16] = {
    QTCFLLDT(0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(-0.7071067812),
    QTCFLLDT(0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(-0.7071067812),
    QTCFLLDT(0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(-0.7071067812),
    QTCFLLDT(0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(-0.7071067812),
};

/* cos ((n + 0.5)*pi*angle/L) , order = 79, L=8 */
const FIXP_QTW qmf_phaseshift_cos8_cldfb_ana[8] = {
    QTCFLLDT(-0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812),
    QTCFLLDT(-0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812),
};

const FIXP_QTW qmf_phaseshift_cos8_cldfb_syn[8] = {
    QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(0.7071067812),
    QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(0.7071067812),
};

/* sin ((n + 0.5)*pi*angle/L) , order = 79, L=8 */
const FIXP_QTW qmf_phaseshift_sin8_cldfb[8] = {
    QTCFLLDT(0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(-0.7071067812),
    QTCFLLDT(0.7071067812), QTCFLLDT(0.7071067812), QTCFLLDT(-0.7071067812), QTCFLLDT(-0.7071067812),
};

/* sin/cos (angle) / 128 */
const FIXP_QTW qmf_phaseshift_cos64_cldfb[64] = {
    QTCFLLDT(7.071068e-01), QTCFLLDT(-7.071066e-01), QTCFLLDT(-7.071070e-01), QTCFLLDT(7.071065e-01),
    QTCFLLDT(7.071072e-01), QTCFLLDT(-7.071063e-01), QTCFLLDT(-7.071074e-01), QTCFLLDT(7.071061e-01),
    QTCFLLDT(7.071075e-01), QTCFLLDT(-7.071059e-01), QTCFLLDT(-7.071078e-01), QTCFLLDT(7.071057e-01),
    QTCFLLDT(7.071080e-01), QTCFLLDT(-7.071055e-01), QTCFLLDT(-7.071081e-01), QTCFLLDT(7.071053e-01),
    QTCFLLDT(7.071083e-01), QTCFLLDT(-7.071052e-01), QTCFLLDT(-7.071085e-01), QTCFLLDT(7.071050e-01),
    QTCFLLDT(7.071087e-01), QTCFLLDT(-7.071048e-01), QTCFLLDT(-7.071089e-01), QTCFLLDT(7.071046e-01),
    QTCFLLDT(7.071090e-01), QTCFLLDT(-7.071044e-01), QTCFLLDT(-7.071092e-01), QTCFLLDT(7.071042e-01),
    QTCFLLDT(7.071095e-01), QTCFLLDT(-7.071040e-01), QTCFLLDT(-7.071096e-01), QTCFLLDT(7.071038e-01),
    QTCFLLDT(7.071098e-01), QTCFLLDT(-7.071037e-01), QTCFLLDT(-7.071100e-01), QTCFLLDT(7.071035e-01),
    QTCFLLDT(7.071102e-01), QTCFLLDT(-7.071033e-01), QTCFLLDT(-7.071103e-01), QTCFLLDT(7.071031e-01),
    QTCFLLDT(7.071105e-01), QTCFLLDT(-7.071030e-01), QTCFLLDT(-7.071107e-01), QTCFLLDT(7.071028e-01),
    QTCFLLDT(7.071109e-01), QTCFLLDT(-7.071025e-01), QTCFLLDT(-7.071111e-01), QTCFLLDT(7.071024e-01),
    QTCFLLDT(7.071113e-01), QTCFLLDT(-7.071022e-01), QTCFLLDT(-7.071115e-01), QTCFLLDT(7.071020e-01),
    QTCFLLDT(7.071117e-01), QTCFLLDT(-7.071018e-01), QTCFLLDT(-7.071118e-01), QTCFLLDT(7.071016e-01),
    QTCFLLDT(7.071120e-01), QTCFLLDT(-7.071015e-01), QTCFLLDT(-7.071122e-01), QTCFLLDT(7.071013e-01),
    QTCFLLDT(7.071124e-01), QTCFLLDT(-7.071011e-01), QTCFLLDT(-7.071126e-01), QTCFLLDT(7.071009e-01),
};
const FIXP_QTW qmf_phaseshift_sin64_cldfb[64] = {
    QTCFLLDT(7.071067e-01), QTCFLLDT(7.071069e-01), QTCFLLDT(-7.071065e-01), QTCFLLDT(-7.071071e-01),
    QTCFLLDT(7.071064e-01), QTCFLLDT(7.071073e-01), QTCFLLDT(-7.071062e-01), QTCFLLDT(-7.071075e-01),
    QTCFLLDT(7.071060e-01), QTCFLLDT(7.071077e-01), QTCFLLDT(-7.071058e-01), QTCFLLDT(-7.071078e-01),
    QTCFLLDT(7.071056e-01), QTCFLLDT(7.071080e-01), QTCFLLDT(-7.071055e-01), QTCFLLDT(-7.071082e-01),
    QTCFLLDT(7.071053e-01), QTCFLLDT(7.071084e-01), QTCFLLDT(-7.071050e-01), QTCFLLDT(-7.071086e-01),
    QTCFLLDT(7.071049e-01), QTCFLLDT(7.071088e-01), QTCFLLDT(-7.071047e-01), QTCFLLDT(-7.071090e-01),
    QTCFLLDT(7.071045e-01), QTCFLLDT(7.071092e-01), QTCFLLDT(-7.071043e-01), QTCFLLDT(-7.071093e-01),
    QTCFLLDT(7.071041e-01), QTCFLLDT(7.071095e-01), QTCFLLDT(-7.071040e-01), QTCFLLDT(-7.071097e-01),
    QTCFLLDT(7.071038e-01), QTCFLLDT(7.071099e-01), QTCFLLDT(-7.071036e-01), QTCFLLDT(-7.071100e-01),
    QTCFLLDT(7.071034e-01), QTCFLLDT(7.071103e-01), QTCFLLDT(-7.071032e-01), QTCFLLDT(-7.071105e-01),
    QTCFLLDT(7.071030e-01), QTCFLLDT(7.071106e-01), QTCFLLDT(-7.071028e-01), QTCFLLDT(-7.071108e-01),
    QTCFLLDT(7.071027e-01), QTCFLLDT(7.071110e-01), QTCFLLDT(-7.071025e-01), QTCFLLDT(-7.071112e-01),
    QTCFLLDT(7.071023e-01), QTCFLLDT(7.071114e-01), QTCFLLDT(-7.071021e-01), QTCFLLDT(-7.071115e-01),
    QTCFLLDT(7.071019e-01), QTCFLLDT(7.071117e-01), QTCFLLDT(-7.071017e-01), QTCFLLDT(-7.071120e-01),
    QTCFLLDT(7.071015e-01), QTCFLLDT(7.071121e-01), QTCFLLDT(-7.071013e-01), QTCFLLDT(-7.071123e-01),
    QTCFLLDT(7.071012e-01), QTCFLLDT(7.071125e-01), QTCFLLDT(-7.071010e-01), QTCFLLDT(-7.071127e-01),
};

//@}

#endif /* #ifdef LOW_POWER_SBR_ONLY */

/*!
  \name QMF
  \brief  QMF-Table
          64 channels, N = 640, optimized by PE 010516

  The coeffs are rearranged compared with the reference in the following
  way:
  sbr_qmf_64[0] = sbr_qmf_64_reference[0];
  sbr_qmf_64[1] = sbr_qmf_64_reference[128];
  sbr_qmf_64[2] = sbr_qmf_64_reference[256];
  sbr_qmf_64[3] = sbr_qmf_64_reference[384];
  sbr_qmf_64[4] = sbr_qmf_64_reference[512];

  sbr_qmf_64[5] = sbr_qmf_64_reference[1];
  sbr_qmf_64[6] = sbr_qmf_64_reference[129];
  sbr_qmf_64[7] = sbr_qmf_64_reference[257];
  sbr_qmf_64[8] = sbr_qmf_64_reference[385];
  sbr_qmf_64[9] = sbr_qmf_64_reference[513];
  .
  .
  .
  sbr_qmf_64[635] = sbr_qmf_64_reference[127]
  sbr_qmf_64[636] = sbr_qmf_64_reference[255];
  sbr_qmf_64[637] = sbr_qmf_64_reference[383];
  sbr_qmf_64[638] = sbr_qmf_64_reference[511];
  sbr_qmf_64[639] = sbr_qmf_64_reference[639];


  Symmetric properties of qmf coeffs:

       Use point symmetry:

  sbr_qmf_64_640_qmf[320..634] = p_64_640_qmf[314..0]

  Max sum of all FIR filter absolute coefficients is: 0x7FF5B201
  thus, the filter output is not required to be scaled.

  \showinitializer
*/
//@{

LNK_SECTION_CONSTDATA_L1
RAM_ALIGN
const FIXP_PFT qmf_cldfb_640[QMF640_CLDFB_PFT_TABLE_SIZE] = {
    QTCFLLD(6.571760e-07),  QTCFLLD(-8.010079e-06), QTCFLLD(-1.250743e-03), QTCFLLD(8.996371e-03),
    QTCFLLD(5.128557e-01),  QTCFLLD(4.118360e-07),  QTCFLLD(-1.469933e-05), QTCFLLD(-1.194743e-03),
    QTCFLLD(9.640299e-03),  QTCFLLD(5.299510e-01),  QTCFLLD(8.109952e-07),  QTCFLLD(4.840578e-06),
    QTCFLLD(-1.151796e-03), QTCFLLD(1.033126e-02),  QTCFLLD(5.470652e-01),  QTCFLLD(7.099633e-07),
    QTCFLLD(7.167101e-06),  QTCFLLD(-1.099001e-03), QTCFLLD(1.106959e-02),  QTCFLLD(5.641523e-01),
    QTCFLLD(6.834210e-07),  QTCFLLD(1.088325e-05),  QTCFLLD(-1.047655e-03), QTCFLLD(1.186211e-02),
    QTCFLLD(5.811993e-01),  QTCFLLD(4.292862e-07),  QTCFLLD(1.013260e-05),  QTCFLLD(-9.862027e-04),
    QTCFLLD(1.270747e-02),  QTCFLLD(5.981877e-01),  QTCFLLD(-5.426597e-09), QTCFLLD(5.869707e-06),
    QTCFLLD(-9.294665e-04), QTCFLLD(1.361072e-02),  QTCFLLD(6.151031e-01),  QTCFLLD(6.355303e-08),
    QTCFLLD(1.125135e-05),  QTCFLLD(-9.767709e-04), QTCFLLD(1.456209e-02),  QTCFLLD(6.319284e-01),
    QTCFLLD(5.490570e-07),  QTCFLLD(2.015445e-05),  QTCFLLD(-1.040598e-03), QTCFLLD(1.557759e-02),
    QTCFLLD(6.486438e-01),  QTCFLLD(1.620171e-06),  QTCFLLD(2.800456e-05),  QTCFLLD(-1.146268e-03),
    QTCFLLD(1.665188e-02),  QTCFLLD(6.652304e-01),  QTCFLLD(-6.025110e-10), QTCFLLD(8.975978e-06),
    QTCFLLD(-1.292866e-03), QTCFLLD(1.778249e-02),  QTCFLLD(6.816668e-01),  QTCFLLD(-6.325664e-10),
    QTCFLLD(8.563820e-06),  QTCFLLD(-1.196638e-03), QTCFLLD(1.897506e-02),  QTCFLLD(6.979337e-01),
    QTCFLLD(-4.013525e-09), QTCFLLD(1.168895e-05),  QTCFLLD(-9.726699e-04), QTCFLLD(2.023525e-02),
    QTCFLLD(7.140087e-01),  QTCFLLD(-4.244091e-09), QTCFLLD(7.300589e-06),  QTCFLLD(-8.029620e-04),
    QTCFLLD(2.156305e-02),  QTCFLLD(7.298746e-01),  QTCFLLD(-1.846548e-08), QTCFLLD(3.965364e-06),
    QTCFLLD(-6.754936e-04), QTCFLLD(2.296471e-02),  QTCFLLD(7.455112e-01),  QTCFLLD(-3.870537e-09),
    QTCFLLD(1.374896e-06),  QTCFLLD(-5.791145e-04), QTCFLLD(2.443434e-02),  QTCFLLD(7.609051e-01),
    QTCFLLD(-8.883499e-10), QTCFLLD(3.798520e-07),  QTCFLLD(-4.733148e-04), QTCFLLD(2.597957e-02),
    QTCFLLD(7.760386e-01),  QTCFLLD(5.303528e-08),  QTCFLLD(4.469729e-06),  QTCFLLD(-2.998740e-04),
    QTCFLLD(2.760091e-02),  QTCFLLD(7.908995e-01),  QTCFLLD(7.391974e-08),  QTCFLLD(2.461877e-05),
    QTCFLLD(7.882620e-05),  QTCFLLD(2.931526e-02),  QTCFLLD(8.054701e-01),  QTCFLLD(1.723217e-09),
    QTCFLLD(4.005269e-05),  QTCFLLD(4.708010e-04),  QTCFLLD(3.110861e-02),  QTCFLLD(8.197387e-01),
    QTCFLLD(2.443085e-07),  QTCFLLD(5.272982e-05),  QTCFLLD(8.089812e-04),  QTCFLLD(3.298151e-02),
    QTCFLLD(8.336864e-01),  QTCFLLD(1.387567e-08),  QTCFLLD(4.939392e-05),  QTCFLLD(1.127142e-03),
    QTCFLLD(3.493300e-02),  QTCFLLD(8.472987e-01),  QTCFLLD(-5.690531e-06), QTCFLLD(-4.256442e-05),
    QTCFLLD(1.417367e-03),  QTCFLLD(3.696343e-02),  QTCFLLD(8.605543e-01),  QTCFLLD(3.629067e-06),
    QTCFLLD(6.582328e-05),  QTCFLLD(1.725030e-03),  QTCFLLD(3.907138e-02),  QTCFLLD(8.734367e-01),
    QTCFLLD(-5.393556e-08), QTCFLLD(6.481921e-05),  QTCFLLD(1.948069e-03),  QTCFLLD(4.125570e-02),
    QTCFLLD(8.859232e-01),  QTCFLLD(1.349944e-07),  QTCFLLD(3.367998e-05),  QTCFLLD(2.033465e-03),
    QTCFLLD(4.355568e-02),  QTCFLLD(8.979959e-01),  QTCFLLD(7.326611e-09),  QTCFLLD(4.694252e-05),
    QTCFLLD(2.239143e-03),  QTCFLLD(4.599068e-02),  QTCFLLD(9.096311e-01),  QTCFLLD(2.399696e-07),
    QTCFLLD(6.904415e-05),  QTCFLLD(2.470456e-03),  QTCFLLD(4.849285e-02),  QTCFLLD(9.208195e-01),
    QTCFLLD(3.330982e-07),  QTCFLLD(5.643103e-05),  QTCFLLD(2.630472e-03),  QTCFLLD(5.105621e-02),
    QTCFLLD(9.315442e-01),  QTCFLLD(4.767794e-07),  QTCFLLD(7.095887e-05),  QTCFLLD(2.703019e-03),
    QTCFLLD(5.368313e-02),  QTCFLLD(9.417976e-01),  QTCFLLD(3.428661e-07),  QTCFLLD(7.872593e-05),
    QTCFLLD(2.729137e-03),  QTCFLLD(5.637219e-02),  QTCFLLD(9.515675e-01),  QTCFLLD(8.676848e-06),
    QTCFLLD(2.666445e-04),  QTCFLLD(2.719749e-03),  QTCFLLD(5.911363e-02),  QTCFLLD(9.608520e-01),
    QTCFLLD(2.722296e-05),  QTCFLLD(5.822201e-04),  QTCFLLD(2.530907e-03),  QTCFLLD(6.192693e-02),
    QTCFLLD(9.696426e-01),  QTCFLLD(3.575651e-07),  QTCFLLD(7.870355e-05),  QTCFLLD(2.225524e-03),
    QTCFLLD(6.480449e-02),  QTCFLLD(9.779405e-01),  QTCFLLD(6.293002e-07),  QTCFLLD(7.245096e-05),
    QTCFLLD(1.891972e-03),  QTCFLLD(6.771675e-02),  QTCFLLD(9.857388e-01),  QTCFLLD(1.070243e-06),
    QTCFLLD(7.194151e-05),  QTCFLLD(1.557112e-03),  QTCFLLD(7.064948e-02),  QTCFLLD(9.930380e-01),
    QTCFLLD(-3.225913e-07), QTCFLLD(-7.679955e-05), QTCFLLD(1.194731e-03),  QTCFLLD(7.360559e-02),
    QTCFLLD(9.998286e-01),  QTCFLLD(-9.597516e-09), QTCFLLD(-6.093373e-05), QTCFLLD(6.415402e-04),
    QTCFLLD(7.657650e-02),  QTCFLLD(1.006109e+00),  QTCFLLD(-8.908041e-08), QTCFLLD(-1.721347e-05),
    QTCFLLD(1.092526e-04),  QTCFLLD(7.955571e-02),  QTCFLLD(1.011868e+00),  QTCFLLD(-2.285563e-05),
    QTCFLLD(-8.882305e-05), QTCFLLD(2.934876e-04),  QTCFLLD(8.251962e-02),  QTCFLLD(1.017100e+00),
    QTCFLLD(1.013575e-05),  QTCFLLD(6.418658e-05),  QTCFLLD(5.721223e-04),  QTCFLLD(8.547716e-02),
    QTCFLLD(1.021799e+00),  QTCFLLD(-1.706941e-05), QTCFLLD(1.769262e-04),  QTCFLLD(6.976561e-04),
    QTCFLLD(8.841813e-02),  QTCFLLD(1.025967e+00),  QTCFLLD(1.356728e-06),  QTCFLLD(2.206341e-05),
    QTCFLLD(7.376101e-04),  QTCFLLD(9.133591e-02),  QTCFLLD(1.029601e+00),  QTCFLLD(-1.398913e-08),
    QTCFLLD(-6.538879e-06), QTCFLLD(7.154124e-04),  QTCFLLD(9.421624e-02),  QTCFLLD(1.032713e+00),
    QTCFLLD(3.552992e-08),  QTCFLLD(-1.052707e-05), QTCFLLD(7.139920e-04),  QTCFLLD(9.705240e-02),
    QTCFLLD(1.035312e+00),  QTCFLLD(4.211177e-07),  QTCFLLD(-9.075431e-06), QTCFLLD(6.944123e-04),
    QTCFLLD(9.982958e-02),  QTCFLLD(1.037422e+00),  QTCFLLD(5.433719e-07),  QTCFLLD(-1.748285e-05),
    QTCFLLD(6.766320e-04),  QTCFLLD(1.025398e-01),  QTCFLLD(1.039062e+00),  QTCFLLD(8.226600e-08),
    QTCFLLD(-3.498286e-05), QTCFLLD(6.887784e-04),  QTCFLLD(1.051642e-01),  QTCFLLD(1.040262e+00),
    QTCFLLD(1.272705e-07),  QTCFLLD(-4.489491e-05), QTCFLLD(6.673250e-04),  QTCFLLD(1.076972e-01),
    QTCFLLD(1.041043e+00),  QTCFLLD(2.542598e-07),  QTCFLLD(-5.449816e-05), QTCFLLD(5.970697e-04),
    QTCFLLD(1.101216e-01),  QTCFLLD(1.041434e+00),  QTCFLLD(6.322770e-07),  QTCFLLD(-5.874199e-05),
    QTCFLLD(4.749931e-04),  QTCFLLD(1.124296e-01),  QTCFLLD(1.041443e+00),  QTCFLLD(2.801882e-08),
    QTCFLLD(-7.934510e-05), QTCFLLD(3.189336e-04),  QTCFLLD(1.146042e-01),  QTCFLLD(1.041087e+00),
    QTCFLLD(5.891904e-07),  QTCFLLD(-8.039232e-05), QTCFLLD(1.218226e-04),  QTCFLLD(1.166399e-01),
    QTCFLLD(1.040350e+00),  QTCFLLD(7.301957e-07),  QTCFLLD(-9.907631e-05), QTCFLLD(-1.324292e-04),
    QTCFLLD(1.185243e-01),  QTCFLLD(1.039228e+00),  QTCFLLD(-4.518603e-06), QTCFLLD(-2.217025e-04),
    QTCFLLD(-4.268575e-04), QTCFLLD(1.202546e-01),  QTCFLLD(1.037683e+00),  QTCFLLD(-3.561585e-06),
    QTCFLLD(-2.415166e-04), QTCFLLD(-7.804546e-04), QTCFLLD(1.218184e-01),  QTCFLLD(1.035694e+00),
    QTCFLLD(-1.074717e-07), QTCFLLD(-2.123672e-04), QTCFLLD(-1.156680e-03), QTCFLLD(1.232132e-01),
    QTCFLLD(1.033206e+00),  QTCFLLD(1.323268e-06),  QTCFLLD(-2.078299e-04), QTCFLLD(-1.525819e-03),
    QTCFLLD(1.244270e-01),  QTCFLLD(1.030199e+00),  QTCFLLD(3.377815e-06),  QTCFLLD(-1.885286e-04),
    QTCFLLD(-1.914115e-03), QTCFLLD(1.254605e-01),  QTCFLLD(1.026616e+00),  QTCFLLD(5.161607e-06),
    QTCFLLD(-1.728673e-04), QTCFLLD(-2.292814e-03), QTCFLLD(1.262996e-01),  QTCFLLD(1.022470e+00),
    QTCFLLD(5.924001e-06),  QTCFLLD(-1.744842e-04), QTCFLLD(-2.658042e-03), QTCFLLD(1.269416e-01),
    QTCFLLD(1.017729e+00),  QTCFLLD(6.310208e-06),  QTCFLLD(-1.784193e-04), QTCFLLD(-3.000423e-03),
    QTCFLLD(1.273648e-01),  QTCFLLD(1.012508e+00),  QTCFLLD(3.357219e-06),  QTCFLLD(-2.131406e-04),
    QTCFLLD(-3.318858e-03), QTCFLLD(1.275561e-01),  QTCFLLD(1.006893e+00),  QTCFLLD(5.189087e-06),
    QTCFLLD(-2.078886e-04), QTCFLLD(-3.597476e-03), QTCFLLD(1.274568e-01),  QTCFLLD(1.001463e+00),
    QTCFLLD(4.178050e-06),  QTCFLLD(-4.663778e-05), QTCFLLD(-3.870852e-03), QTCFLLD(1.273591e-01),
    QTCFLLD(9.927544e-01),  QTCFLLD(5.364807e-06),  QTCFLLD(-5.889277e-06), QTCFLLD(-4.135130e-03),
    QTCFLLD(1.272499e-01),  QTCFLLD(9.807692e-01),  QTCFLLD(4.083719e-06),  QTCFLLD(-1.774108e-05),
    QTCFLLD(-4.351668e-03), QTCFLLD(1.268281e-01),  QTCFLLD(9.690017e-01),  QTCFLLD(3.567581e-06),
    QTCFLLD(-2.599468e-08), QTCFLLD(-4.517190e-03), QTCFLLD(1.261262e-01),  QTCFLLD(9.568886e-01),
    QTCFLLD(3.262754e-06),  QTCFLLD(1.260640e-05),  QTCFLLD(-4.636228e-03), QTCFLLD(1.251477e-01),
    QTCFLLD(9.443803e-01),  QTCFLLD(2.041128e-06),  QTCFLLD(2.364519e-05),  QTCFLLD(-4.704321e-03),
    QTCFLLD(1.238869e-01),  QTCFLLD(9.313874e-01),  QTCFLLD(-2.567965e-08), QTCFLLD(2.806963e-05),
    QTCFLLD(-4.722568e-03), QTCFLLD(1.223371e-01),  QTCFLLD(9.179666e-01),  QTCFLLD(2.714879e-07),
    QTCFLLD(4.493916e-05),  QTCFLLD(-4.663276e-03), QTCFLLD(1.204854e-01),  QTCFLLD(9.041286e-01),
    QTCFLLD(2.150884e-06),  QTCFLLD(5.408155e-05),  QTCFLLD(-4.554811e-03), QTCFLLD(1.183233e-01),
    QTCFLLD(8.899474e-01),  QTCFLLD(5.818595e-06),  QTCFLLD(3.759630e-05),  QTCFLLD(-4.369554e-03),
    QTCFLLD(1.158359e-01),  QTCFLLD(8.754641e-01),  QTCFLLD(-1.686137e-09), QTCFLLD(2.515118e-05),
    QTCFLLD(-4.091033e-03), QTCFLLD(1.130180e-01),  QTCFLLD(8.607492e-01),  QTCFLLD(-1.775191e-09),
    QTCFLLD(2.406517e-05),  QTCFLLD(-3.794425e-03), QTCFLLD(1.098551e-01),  QTCFLLD(8.458450e-01),
    QTCFLLD(-2.222072e-09), QTCFLLD(3.628511e-05),  QTCFLLD(-3.460363e-03), QTCFLLD(1.063455e-01),
    QTCFLLD(8.308040e-01),  QTCFLLD(-1.280675e-08), QTCFLLD(2.241546e-05),  QTCFLLD(-3.064311e-03),
    QTCFLLD(1.024805e-01),  QTCFLLD(8.156523e-01),  QTCFLLD(-6.977078e-08), QTCFLLD(1.499170e-05),
    QTCFLLD(-2.621537e-03), QTCFLLD(9.826251e-02),  QTCFLLD(8.004165e-01),  QTCFLLD(-1.409927e-08),
    QTCFLLD(5.009913e-06),  QTCFLLD(-2.124648e-03), QTCFLLD(9.368652e-02),  QTCFLLD(7.851012e-01),
    QTCFLLD(-2.986489e-09), QTCFLLD(1.277184e-06),  QTCFLLD(-1.594861e-03), QTCFLLD(8.875756e-02),
    QTCFLLD(7.697093e-01),  QTCFLLD(1.876022e-07),  QTCFLLD(1.580189e-05),  QTCFLLD(-1.061499e-03),
    QTCFLLD(8.347151e-02),  QTCFLLD(7.542294e-01),  QTCFLLD(1.737277e-07),  QTCFLLD(5.533953e-05),
    QTCFLLD(-6.169855e-04), QTCFLLD(7.783300e-02),  QTCFLLD(7.386515e-01),  QTCFLLD(3.818589e-09),
    QTCFLLD(8.870182e-05),  QTCFLLD(-2.004823e-04), QTCFLLD(7.184074e-02),  QTCFLLD(7.229599e-01),
    QTCFLLD(5.143615e-07),  QTCFLLD(1.035783e-04),  QTCFLLD(2.048499e-04),  QTCFLLD(6.550209e-02),
    QTCFLLD(7.071448e-01),  QTCFLLD(2.820292e-08),  QTCFLLD(9.990758e-05),  QTCFLLD(5.621721e-04),
    QTCFLLD(5.881297e-02),  QTCFLLD(6.911982e-01),  QTCFLLD(4.677016e-06),  QTCFLLD(1.181078e-04),
    QTCFLLD(9.373975e-04),  QTCFLLD(5.177965e-02),  QTCFLLD(6.751199e-01),  QTCFLLD(3.361682e-06),
    QTCFLLD(2.126365e-05),  QTCFLLD(1.344657e-03),  QTCFLLD(4.439684e-02),  QTCFLLD(6.589149e-01),
    QTCFLLD(-4.880845e-08), QTCFLLD(5.861800e-05),  QTCFLLD(1.812176e-03),  QTCFLLD(3.666943e-02),
    QTCFLLD(6.425940e-01),  QTCFLLD(2.267731e-07),  QTCFLLD(5.021906e-05),  QTCFLLD(2.172866e-03),
    QTCFLLD(2.857528e-02),  QTCFLLD(6.261725e-01),  QTCFLLD(5.158213e-09),  QTCFLLD(4.150075e-05),
    QTCFLLD(1.985825e-03),  QTCFLLD(2.012237e-02),  QTCFLLD(6.096690e-01),  QTCFLLD(-2.066962e-07),
    QTCFLLD(3.799972e-05),  QTCFLLD(1.697653e-03),  QTCFLLD(1.132324e-02),  QTCFLLD(5.930982e-01),
    QTCFLLD(4.883305e-07),  QTCFLLD(6.606462e-05),  QTCFLLD(1.471167e-03),  QTCFLLD(2.184257e-03),
    QTCFLLD(5.764735e-01),  QTCFLLD(8.254430e-07),  QTCFLLD(9.755685e-05),  QTCFLLD(1.232134e-03),
    QTCFLLD(-7.298198e-03), QTCFLLD(5.598052e-01),  QTCFLLD(9.464783e-07),  QTCFLLD(1.831121e-04),
    QTCFLLD(8.990256e-04),  QTCFLLD(-1.711324e-02), QTCFLLD(5.430990e-01),  QTCFLLD(-1.232693e-05),
    QTCFLLD(-5.901618e-07), QTCFLLD(6.150317e-04),  QTCFLLD(-2.726484e-02), QTCFLLD(5.263554e-01),
    QTCFLLD(3.867483e-05),  QTCFLLD(-3.595054e-04), QTCFLLD(6.307841e-04),  QTCFLLD(-3.775928e-02),
    QTCFLLD(5.095721e-01),  QTCFLLD(-9.870548e-07), QTCFLLD(-1.815837e-04), QTCFLLD(4.366447e-04),
    QTCFLLD(-4.859006e-02), QTCFLLD(4.927464e-01),  QTCFLLD(-1.089501e-06), QTCFLLD(-9.204876e-05),
    QTCFLLD(1.498232e-04),  QTCFLLD(-5.973742e-02), QTCFLLD(4.758754e-01),  QTCFLLD(-1.569003e-06),
    QTCFLLD(-5.192444e-05), QTCFLLD(-9.099723e-05), QTCFLLD(-7.120357e-02), QTCFLLD(4.589583e-01),
    QTCFLLD(-2.778618e-07), QTCFLLD(6.487880e-05),  QTCFLLD(-3.337967e-04), QTCFLLD(-8.298103e-02),
    QTCFLLD(4.420014e-01),  QTCFLLD(6.757015e-09),  QTCFLLD(5.397065e-05),  QTCFLLD(-5.599348e-04),
    QTCFLLD(-9.506967e-02), QTCFLLD(4.250144e-01),  QTCFLLD(1.496436e-07),  QTCFLLD(2.472024e-05),
    QTCFLLD(-7.677634e-04), QTCFLLD(-1.074631e-01), QTCFLLD(4.080155e-01),  QTCFLLD(2.068297e-05),
    QTCFLLD(9.711682e-05),  QTCFLLD(-9.730460e-04), QTCFLLD(-1.201629e-01), QTCFLLD(3.910244e-01),
    QTCFLLD(-9.388963e-06), QTCFLLD(5.144969e-05),  QTCFLLD(-1.131860e-03), QTCFLLD(-1.331545e-01),
    QTCFLLD(3.740644e-01),  QTCFLLD(-1.402925e-05), QTCFLLD(-1.039264e-04), QTCFLLD(-1.283281e-03),
    QTCFLLD(-1.464389e-01), QTCFLLD(3.571528e-01),  QTCFLLD(-2.757611e-06), QTCFLLD(2.853437e-06),
    QTCFLLD(-1.480543e-03), QTCFLLD(-1.600062e-01), QTCFLLD(3.403074e-01),  QTCFLLD(2.945239e-08),
    QTCFLLD(1.334091e-05),  QTCFLLD(-1.699161e-03), QTCFLLD(-1.738542e-01), QTCFLLD(3.235299e-01),
    QTCFLLD(-7.873304e-08), QTCFLLD(2.443161e-05),  QTCFLLD(-1.924845e-03), QTCFLLD(-1.879712e-01),
    QTCFLLD(3.068187e-01),  QTCFLLD(-9.897194e-07), QTCFLLD(3.568555e-05),  QTCFLLD(-2.152380e-03),
    QTCFLLD(-2.023548e-01), QTCFLLD(2.901491e-01),  QTCFLLD(-1.922074e-06), QTCFLLD(6.193370e-05),
    QTCFLLD(-2.396404e-03), QTCFLLD(-2.169926e-01), QTCFLLD(2.734977e-01),  QTCFLLD(-2.765650e-07),
    QTCFLLD(1.176237e-04),  QTCFLLD(-2.653819e-03), QTCFLLD(-2.318815e-01), QTCFLLD(2.568176e-01),
    QTCFLLD(-4.636105e-07), QTCFLLD(1.635906e-04),  QTCFLLD(-2.927159e-03), QTCFLLD(-2.470098e-01),
    QTCFLLD(2.400768e-01),  QTCFLLD(-9.607069e-07), QTCFLLD(2.060394e-04),  QTCFLLD(-3.209093e-03),
    QTCFLLD(-2.623749e-01), QTCFLLD(2.232277e-01),  QTCFLLD(-1.907927e-06), QTCFLLD(2.346981e-04),
    QTCFLLD(-3.505531e-03), QTCFLLD(-2.779638e-01), QTCFLLD(2.062605e-01),  QTCFLLD(-1.551251e-08),
    QTCFLLD(2.520607e-04),  QTCFLLD(-3.811612e-03), QTCFLLD(-2.937725e-01), QTCFLLD(1.891590e-01),
    QTCFLLD(-1.653464e-06), QTCFLLD(2.556450e-04),  QTCFLLD(-4.133640e-03), QTCFLLD(-3.097862e-01),
    QTCFLLD(1.719726e-01),  QTCFLLD(-2.043464e-06), QTCFLLD(3.157664e-04),  QTCFLLD(-4.448993e-03),
    QTCFLLD(-3.259994e-01), QTCFLLD(1.547461e-01),  QTCFLLD(1.622786e-05),  QTCFLLD(6.205676e-04),
    QTCFLLD(-4.754192e-03), QTCFLLD(-3.423942e-01), QTCFLLD(1.376150e-01),  QTCFLLD(1.395221e-05),
    QTCFLLD(7.847840e-04),  QTCFLLD(-5.063851e-03), QTCFLLD(-3.589627e-01), QTCFLLD(1.206924e-01),
    QTCFLLD(4.591010e-07),  QTCFLLD(9.019129e-04),  QTCFLLD(-5.394570e-03), QTCFLLD(-3.756822e-01),
    QTCFLLD(1.042033e-01),  QTCFLLD(-6.261944e-06), QTCFLLD(1.054963e-03),  QTCFLLD(-5.741103e-03),
    QTCFLLD(-3.925409e-01), QTCFLLD(8.829745e-02),  QTCFLLD(-1.606051e-05), QTCFLLD(1.089429e-03),
    QTCFLLD(-6.109179e-03), QTCFLLD(-4.095160e-01), QTCFLLD(7.325979e-02),  QTCFLLD(-2.464228e-05),
    QTCFLLD(1.122503e-03),  QTCFLLD(-6.500503e-03), QTCFLLD(-4.265950e-01), QTCFLLD(5.918678e-02),
    QTCFLLD(-2.976824e-05), QTCFLLD(1.177515e-03),  QTCFLLD(-6.925141e-03), QTCFLLD(-4.437530e-01),
    QTCFLLD(4.634696e-02),  QTCFLLD(-3.177468e-05), QTCFLLD(1.226113e-03),  QTCFLLD(-7.380544e-03),
    QTCFLLD(-4.609829e-01), QTCFLLD(3.450719e-02),  QTCFLLD(-4.373302e-05), QTCFLLD(1.263569e-03),
    QTCFLLD(-7.876393e-03), QTCFLLD(-4.782650e-01), QTCFLLD(2.353060e-02),  QTCFLLD(-3.299004e-05),
    QTCFLLD(1.287819e-03),  QTCFLLD(-8.407749e-03), QTCFLLD(-4.956175e-01), QTCFLLD(1.129580e-02),
};

RAM_ALIGN
const FIXP_PFT qmf_cldfb_320[QMF320_CLDFB_PFT_TABLE_SIZE] = {
    QTCFLLD(5.345060e-07),  QTCFLLD(-1.135471e-05), QTCFLLD(-1.222743e-03), QTCFLLD(9.318335e-03),
    QTCFLLD(5.214033e-01),  QTCFLLD(7.604792e-07),  QTCFLLD(6.003839e-06),  QTCFLLD(-1.125398e-03),
    QTCFLLD(1.070043e-02),  QTCFLLD(5.556087e-01),  QTCFLLD(5.563536e-07),  QTCFLLD(1.050792e-05),
    QTCFLLD(-1.016929e-03), QTCFLLD(1.228479e-02),  QTCFLLD(5.896935e-01),  QTCFLLD(2.906322e-08),
    QTCFLLD(8.560527e-06),  QTCFLLD(-9.531187e-04), QTCFLLD(1.408640e-02),  QTCFLLD(6.235157e-01),
    QTCFLLD(1.084614e-06),  QTCFLLD(2.407951e-05),  QTCFLLD(-1.093433e-03), QTCFLLD(1.611474e-02),
    QTCFLLD(6.569371e-01),  QTCFLLD(-6.175387e-10), QTCFLLD(8.769899e-06),  QTCFLLD(-1.244752e-03),
    QTCFLLD(1.837877e-02),  QTCFLLD(6.898003e-01),  QTCFLLD(-4.128808e-09), QTCFLLD(9.494767e-06),
    QTCFLLD(-8.878160e-04), QTCFLLD(2.089915e-02),  QTCFLLD(7.219416e-01),  QTCFLLD(-1.116801e-08),
    QTCFLLD(2.670130e-06),  QTCFLLD(-6.273041e-04), QTCFLLD(2.369952e-02),  QTCFLLD(7.532082e-01),
    QTCFLLD(2.607347e-08),  QTCFLLD(2.424790e-06),  QTCFLLD(-3.865944e-04), QTCFLLD(2.679024e-02),
    QTCFLLD(7.834691e-01),  QTCFLLD(3.782148e-08),  QTCFLLD(3.233573e-05),  QTCFLLD(2.748136e-04),
    QTCFLLD(3.021193e-02),  QTCFLLD(8.126044e-01),  QTCFLLD(1.290921e-07),  QTCFLLD(5.106187e-05),
    QTCFLLD(9.680615e-04),  QTCFLLD(3.395726e-02),  QTCFLLD(8.404925e-01),  QTCFLLD(-1.030732e-06),
    QTCFLLD(1.162943e-05),  QTCFLLD(1.571198e-03),  QTCFLLD(3.801740e-02),  QTCFLLD(8.669955e-01),
    QTCFLLD(4.052940e-08),  QTCFLLD(4.924960e-05),  QTCFLLD(1.990767e-03),  QTCFLLD(4.240569e-02),
    QTCFLLD(8.919595e-01),  QTCFLLD(1.236481e-07),  QTCFLLD(5.799333e-05),  QTCFLLD(2.354800e-03),
    QTCFLLD(4.724177e-02),  QTCFLLD(9.152253e-01),  QTCFLLD(4.049388e-07),  QTCFLLD(6.369496e-05),
    QTCFLLD(2.666746e-03),  QTCFLLD(5.236967e-02),  QTCFLLD(9.366709e-01),  QTCFLLD(4.509857e-06),
    QTCFLLD(1.726852e-04),  QTCFLLD(2.724443e-03),  QTCFLLD(5.774291e-02),  QTCFLLD(9.562097e-01),
    QTCFLLD(1.379026e-05),  QTCFLLD(3.304619e-04),  QTCFLLD(2.378216e-03),  QTCFLLD(6.336571e-02),
    QTCFLLD(9.737916e-01),  QTCFLLD(8.497715e-07),  QTCFLLD(7.219624e-05),  QTCFLLD(1.724542e-03),
    QTCFLLD(6.918311e-02),  QTCFLLD(9.893883e-01),  QTCFLLD(-1.660944e-07), QTCFLLD(-6.886664e-05),
    QTCFLLD(9.181354e-04),  QTCFLLD(7.509105e-02),  QTCFLLD(1.002969e+00),  QTCFLLD(-1.147235e-05),
    QTCFLLD(-5.301826e-05), QTCFLLD(2.013701e-04),  QTCFLLD(8.103766e-02),  QTCFLLD(1.014484e+00),
    QTCFLLD(-3.466829e-06), QTCFLLD(1.205564e-04),  QTCFLLD(6.348892e-04),  QTCFLLD(8.694765e-02),
    QTCFLLD(1.023883e+00),  QTCFLLD(6.713692e-07),  QTCFLLD(7.762268e-06),  QTCFLLD(7.265112e-04),
    QTCFLLD(9.277608e-02),  QTCFLLD(1.031157e+00),  QTCFLLD(2.283238e-07),  QTCFLLD(-9.801253e-06),
    QTCFLLD(7.042022e-04),  QTCFLLD(9.844099e-02),  QTCFLLD(1.036367e+00),  QTCFLLD(3.128189e-07),
    QTCFLLD(-2.623285e-05), QTCFLLD(6.827052e-04),  QTCFLLD(1.038520e-01),  QTCFLLD(1.039662e+00),
    QTCFLLD(1.907652e-07),  QTCFLLD(-4.969654e-05), QTCFLLD(6.321974e-04),  QTCFLLD(1.089094e-01),
    QTCFLLD(1.041239e+00),  QTCFLLD(3.301479e-07),  QTCFLLD(-6.904354e-05), QTCFLLD(3.969634e-04),
    QTCFLLD(1.135169e-01),  QTCFLLD(1.041265e+00),  QTCFLLD(6.596931e-07),  QTCFLLD(-8.973431e-05),
    QTCFLLD(-5.303260e-06), QTCFLLD(1.175821e-01),  QTCFLLD(1.039789e+00),  QTCFLLD(-4.040094e-06),
    QTCFLLD(-2.316096e-04), QTCFLLD(-6.036561e-04), QTCFLLD(1.210365e-01),  QTCFLLD(1.036689e+00),
    QTCFLLD(6.078980e-07),  QTCFLLD(-2.100985e-04), QTCFLLD(-1.341249e-03), QTCFLLD(1.238201e-01),
    QTCFLLD(1.031702e+00),  QTCFLLD(4.269711e-06),  QTCFLLD(-1.806979e-04), QTCFLLD(-2.103464e-03),
    QTCFLLD(1.258800e-01),  QTCFLLD(1.024543e+00),  QTCFLLD(6.117105e-06),  QTCFLLD(-1.764517e-04),
    QTCFLLD(-2.829232e-03), QTCFLLD(1.271532e-01),  QTCFLLD(1.015119e+00),  QTCFLLD(4.273153e-06),
    QTCFLLD(-2.105146e-04), QTCFLLD(-3.458167e-03), QTCFLLD(1.275064e-01),  QTCFLLD(1.004178e+00),
    QTCFLLD(4.771428e-06),  QTCFLLD(-2.626353e-05), QTCFLLD(-4.002991e-03), QTCFLLD(1.273045e-01),
    QTCFLLD(9.867618e-01),  QTCFLLD(3.825650e-06),  QTCFLLD(-8.883540e-06), QTCFLLD(-4.434429e-03),
    QTCFLLD(1.264771e-01),  QTCFLLD(9.629451e-01),  QTCFLLD(2.651941e-06),  QTCFLLD(1.812579e-05),
    QTCFLLD(-4.670274e-03), QTCFLLD(1.245173e-01),  QTCFLLD(9.378839e-01),  QTCFLLD(1.229041e-07),
    QTCFLLD(3.650440e-05),  QTCFLLD(-4.692922e-03), QTCFLLD(1.214113e-01),  QTCFLLD(9.110476e-01),
    QTCFLLD(3.984739e-06),  QTCFLLD(4.583892e-05),  QTCFLLD(-4.462183e-03), QTCFLLD(1.170796e-01),
    QTCFLLD(8.827057e-01),  QTCFLLD(-1.730664e-09), QTCFLLD(2.460818e-05),  QTCFLLD(-3.942729e-03),
    QTCFLLD(1.114366e-01),  QTCFLLD(8.532971e-01),  QTCFLLD(-7.514413e-09), QTCFLLD(2.935029e-05),
    QTCFLLD(-3.262337e-03), QTCFLLD(1.044130e-01),  QTCFLLD(8.232281e-01),  QTCFLLD(-4.193503e-08),
    QTCFLLD(1.000081e-05),  QTCFLLD(-2.373092e-03), QTCFLLD(9.597452e-02),  QTCFLLD(7.927589e-01),
    QTCFLLD(9.230786e-08),  QTCFLLD(8.539538e-06),  QTCFLLD(-1.328180e-03), QTCFLLD(8.611453e-02),
    QTCFLLD(7.619694e-01),  QTCFLLD(8.877312e-08),  QTCFLLD(7.202067e-05),  QTCFLLD(-4.087339e-04),
    QTCFLLD(7.483687e-02),  QTCFLLD(7.308058e-01),  QTCFLLD(2.712822e-07),  QTCFLLD(1.017429e-04),
    QTCFLLD(3.835110e-04),  QTCFLLD(6.215753e-02),  QTCFLLD(6.991715e-01),  QTCFLLD(4.019349e-06),
    QTCFLLD(6.968570e-05),  QTCFLLD(1.141027e-03),  QTCFLLD(4.808825e-02),  QTCFLLD(6.670174e-01),
    QTCFLLD(8.898233e-08),  QTCFLLD(5.441853e-05),  QTCFLLD(1.992521e-03),  QTCFLLD(3.262236e-02),
    QTCFLLD(6.343833e-01),  QTCFLLD(-1.007690e-07), QTCFLLD(3.975024e-05),  QTCFLLD(1.841739e-03),
    QTCFLLD(1.572281e-02),  QTCFLLD(6.013836e-01),  QTCFLLD(6.568868e-07),  QTCFLLD(8.181074e-05),
    QTCFLLD(1.351651e-03),  QTCFLLD(-2.556970e-03), QTCFLLD(5.681393e-01),  QTCFLLD(-5.690228e-06),
    QTCFLLD(9.126098e-05),  QTCFLLD(7.570286e-04),  QTCFLLD(-2.218904e-02), QTCFLLD(5.347272e-01),
    QTCFLLD(1.884389e-05),  QTCFLLD(-2.705446e-04), QTCFLLD(5.337144e-04),  QTCFLLD(-4.317467e-02),
    QTCFLLD(5.011593e-01),  QTCFLLD(-1.329252e-06), QTCFLLD(-7.198660e-05), QTCFLLD(2.941296e-05),
    QTCFLLD(-6.547049e-02), QTCFLLD(4.674168e-01),  QTCFLLD(-1.355524e-07), QTCFLLD(5.942472e-05),
    QTCFLLD(-4.468657e-04), QTCFLLD(-8.902535e-02), QTCFLLD(4.335079e-01),  QTCFLLD(1.041631e-05),
    QTCFLLD(6.091853e-05),  QTCFLLD(-8.704047e-04), QTCFLLD(-1.138130e-01), QTCFLLD(3.995200e-01),
    QTCFLLD(-1.170911e-05), QTCFLLD(-2.623833e-05), QTCFLLD(-1.207570e-03), QTCFLLD(-1.397967e-01),
    QTCFLLD(3.656086e-01),  QTCFLLD(-1.364079e-06), QTCFLLD(8.097173e-06),  QTCFLLD(-1.589852e-03),
    QTCFLLD(-1.669302e-01), QTCFLLD(3.319187e-01),  QTCFLLD(-5.342262e-07), QTCFLLD(3.005858e-05),
    QTCFLLD(-2.038612e-03), QTCFLLD(-1.951630e-01), QTCFLLD(2.984839e-01),  QTCFLLD(-1.099320e-06),
    QTCFLLD(8.977871e-05),  QTCFLLD(-2.525111e-03), QTCFLLD(-2.244371e-01), QTCFLLD(2.651577e-01),
    QTCFLLD(-7.121587e-07), QTCFLLD(1.848150e-04),  QTCFLLD(-3.068126e-03), QTCFLLD(-2.546924e-01),
    QTCFLLD(2.316523e-01),  QTCFLLD(-9.617199e-07), QTCFLLD(2.433794e-04),  QTCFLLD(-3.658572e-03),
    QTCFLLD(-2.858681e-01), QTCFLLD(1.977098e-01),  QTCFLLD(-1.848464e-06), QTCFLLD(2.857057e-04),
    QTCFLLD(-4.291316e-03), QTCFLLD(-3.178928e-01), QTCFLLD(1.633594e-01),  QTCFLLD(1.509004e-05),
    QTCFLLD(7.026758e-04),  QTCFLLD(-4.909021e-03), QTCFLLD(-3.506784e-01), QTCFLLD(1.291537e-01),
    QTCFLLD(-2.901422e-06), QTCFLLD(9.784381e-04),  QTCFLLD(-5.567837e-03), QTCFLLD(-3.841116e-01),
    QTCFLLD(9.625038e-02),  QTCFLLD(-2.035140e-05), QTCFLLD(1.105966e-03),  QTCFLLD(-6.304841e-03),
    QTCFLLD(-4.180555e-01), QTCFLLD(6.622328e-02),  QTCFLLD(-3.077146e-05), QTCFLLD(1.201814e-03),
    QTCFLLD(-7.152842e-03), QTCFLLD(-4.523680e-01), QTCFLLD(4.042707e-02),  QTCFLLD(-3.836153e-05),
    QTCFLLD(1.275694e-03),  QTCFLLD(-8.142071e-03), QTCFLLD(-4.869413e-01), QTCFLLD(1.741320e-02),
};

RAM_ALIGN
const FIXP_PFT qmf_cldfb_160[QMF160_CLDFB_PFT_TABLE_SIZE] = {
    QTCFLLD(6.114156e-07),  QTCFLLD(-4.929378e-06), QTCFLLD(-1.173270e-03), QTCFLLD(9.985781e-03),
    QTCFLLD(5.385081e-01),  QTCFLLD(2.119298e-07),  QTCFLLD(8.001152e-06),  QTCFLLD(-9.578346e-04),
    QTCFLLD(1.315910e-02),  QTCFLLD(6.066454e-01),  QTCFLLD(8.097845e-07),  QTCFLLD(1.849027e-05),
    QTCFLLD(-1.219567e-03), QTCFLLD(1.721718e-02),  QTCFLLD(6.734486e-01),  QTCFLLD(-1.135478e-08),
    QTCFLLD(5.632976e-06),  QTCFLLD(-7.392278e-04), QTCFLLD(2.226388e-02),  QTCFLLD(7.376929e-01),
    QTCFLLD(6.347751e-08),  QTCFLLD(1.454425e-05),  QTCFLLD(-1.105239e-04), QTCFLLD(2.845808e-02),
    QTCFLLD(7.981848e-01),  QTCFLLD(-2.838328e-06), QTCFLLD(3.414749e-06),  QTCFLLD(1.272254e-03),
    QTCFLLD(3.594821e-02),  QTCFLLD(8.539265e-01),  QTCFLLD(7.116049e-08),  QTCFLLD(4.031125e-05),
    QTCFLLD(2.136304e-03),  QTCFLLD(4.477318e-02),  QTCFLLD(9.038135e-01),  QTCFLLD(4.098227e-07),
    QTCFLLD(7.484240e-05),  QTCFLLD(2.716078e-03),  QTCFLLD(5.502766e-02),  QTCFLLD(9.466825e-01),
    QTCFLLD(4.934327e-07),  QTCFLLD(7.557725e-05),  QTCFLLD(2.058748e-03),  QTCFLLD(6.626062e-02),
    QTCFLLD(9.818396e-01),  QTCFLLD(-4.933896e-08), QTCFLLD(-3.907360e-05), QTCFLLD(3.753964e-04),
    QTCFLLD(7.806610e-02),  QTCFLLD(1.008988e+00),  QTCFLLD(-7.856341e-06), QTCFLLD(9.949480e-05),
    QTCFLLD(7.176331e-04),  QTCFLLD(8.987702e-02),  QTCFLLD(1.027784e+00),  QTCFLLD(4.822448e-07),
    QTCFLLD(-1.327914e-05), QTCFLLD(6.855222e-04),  QTCFLLD(1.011847e-01),  QTCFLLD(1.038242e+00),
    QTCFLLD(4.432684e-07),  QTCFLLD(-5.662008e-05), QTCFLLD(5.360314e-04),  QTCFLLD(1.112756e-01),
    QTCFLLD(1.041439e+00),  QTCFLLD(-1.894204e-06), QTCFLLD(-1.603894e-04), QTCFLLD(-2.796433e-04),
    QTCFLLD(1.193894e-01),  QTCFLLD(1.038456e+00),  QTCFLLD(2.350541e-06),  QTCFLLD(-1.981793e-04),
    QTCFLLD(-1.719967e-03), QTCFLLD(1.249437e-01),  QTCFLLD(1.028407e+00),  QTCFLLD(4.833713e-06),
    QTCFLLD(-1.957799e-04), QTCFLLD(-3.159640e-03), QTCFLLD(1.274605e-01),  QTCFLLD(1.009701e+00),
    QTCFLLD(4.724263e-06),  QTCFLLD(-1.181518e-05), QTCFLLD(-4.243399e-03), QTCFLLD(1.270390e-01),
    QTCFLLD(9.748854e-01),  QTCFLLD(1.007724e-06),  QTCFLLD(2.585741e-05),  QTCFLLD(-4.713445e-03),
    QTCFLLD(1.231120e-01),  QTCFLLD(9.246770e-01),  QTCFLLD(2.908454e-06),  QTCFLLD(3.137374e-05),
    QTCFLLD(-4.230293e-03), QTCFLLD(1.144269e-01),  QTCFLLD(8.681067e-01),  QTCFLLD(-4.128877e-08),
    QTCFLLD(1.870358e-05),  QTCFLLD(-2.842924e-03), QTCFLLD(1.003715e-01),  QTCFLLD(8.080344e-01),
    QTCFLLD(1.806649e-07),  QTCFLLD(3.557071e-05),  QTCFLLD(-8.392422e-04), QTCFLLD(8.065225e-02),
    QTCFLLD(7.464405e-01),  QTCFLLD(2.352609e-06),  QTCFLLD(1.090077e-04),  QTCFLLD(7.497848e-04),
    QTCFLLD(5.529631e-02),  QTCFLLD(6.831591e-01),  QTCFLLD(1.159657e-07),  QTCFLLD(4.585990e-05),
    QTCFLLD(2.079346e-03),  QTCFLLD(2.434883e-02),  QTCFLLD(6.179208e-01),  QTCFLLD(8.859606e-07),
    QTCFLLD(1.403345e-04),  QTCFLLD(1.065580e-03),  QTCFLLD(-1.220572e-02), QTCFLLD(5.514521e-01),
    QTCFLLD(-1.038278e-06), QTCFLLD(-1.368162e-04), QTCFLLD(2.932339e-04),  QTCFLLD(-5.416374e-02),
    QTCFLLD(4.843109e-01),  QTCFLLD(7.820030e-08),  QTCFLLD(3.934544e-05),  QTCFLLD(-6.638491e-04),
    QTCFLLD(-1.012664e-01), QTCFLLD(4.165150e-01),  QTCFLLD(-8.393432e-06), QTCFLLD(-5.053646e-05),
    QTCFLLD(-1.381912e-03), QTCFLLD(-1.532225e-01), QTCFLLD(3.487301e-01),  QTCFLLD(-1.455897e-06),
    QTCFLLD(4.880962e-05),  QTCFLLD(-2.274392e-03), QTCFLLD(-2.096737e-01), QTCFLLD(2.818234e-01),
    QTCFLLD(-1.434317e-06), QTCFLLD(2.203687e-04),  QTCFLLD(-3.357312e-03), QTCFLLD(-2.701693e-01),
    QTCFLLD(2.147441e-01),  QTCFLLD(7.092199e-06),  QTCFLLD(4.681670e-04),  QTCFLLD(-4.601593e-03),
    QTCFLLD(-3.341968e-01), QTCFLLD(1.461805e-01),  QTCFLLD(-1.116123e-05), QTCFLLD(1.072196e-03),
    QTCFLLD(-5.925141e-03), QTCFLLD(-4.010285e-01), QTCFLLD(8.077862e-02),  QTCFLLD(-3.775385e-05),
    QTCFLLD(1.244841e-03),  QTCFLLD(-7.628469e-03), QTCFLLD(-4.696240e-01), QTCFLLD(2.901889e-02),
};

RAM_ALIGN
const FIXP_PFT qmf_cldfb_80[QMF80_CLDFB_PFT_TABLE_SIZE] = {
    QTCFLLD(6.966921e-07),  QTCFLLD(9.025176e-06),  QTCFLLD(-1.073328e-03), QTCFLLD(1.146585e-02),
    QTCFLLD(5.726758e-01),  QTCFLLD(-2.323046e-09), QTCFLLD(1.012638e-05),  QTCFLLD(-1.084654e-03),
    QTCFLLD(1.960515e-02),  QTCFLLD(7.059712e-01),  QTCFLLD(1.230159e-07),  QTCFLLD(4.639126e-05),
    QTCFLLD(6.398911e-04),  QTCFLLD(3.204506e-02),  QTCFLLD(8.267125e-01),  QTCFLLD(2.865339e-07),
    QTCFLLD(6.273759e-05),  QTCFLLD(2.550464e-03),  QTCFLLD(4.977453e-02),  QTCFLLD(9.261818e-01),
    QTCFLLD(3.738257e-07),  QTCFLLD(-2.429021e-06), QTCFLLD(1.375921e-03),  QTCFLLD(7.212754e-02),
    QTCFLLD(9.964333e-01),  QTCFLLD(1.077039e-08),  QTCFLLD(-8.532976e-06), QTCFLLD(7.147022e-04),
    QTCFLLD(9.563432e-02),  QTCFLLD(1.034012e+00),  QTCFLLD(3.086046e-07),  QTCFLLD(-7.986870e-05),
    QTCFLLD(2.203781e-04),  QTCFLLD(1.156221e-01),  QTCFLLD(1.040718e+00),  QTCFLLD(5.542804e-06),
    QTCFLLD(-1.736757e-04), QTCFLLD(-2.475428e-03), QTCFLLD(1.266206e-01),  QTCFLLD(1.020100e+00),
    QTCFLLD(3.415168e-06),  QTCFLLD(6.290201e-06),  QTCFLLD(-4.576709e-03), QTCFLLD(1.256370e-01),
    QTCFLLD(9.506344e-01),  QTCFLLD(-1.998632e-09), QTCFLLD(3.017514e-05),  QTCFLLD(-3.627394e-03),
    QTCFLLD(1.081003e-01),  QTCFLLD(8.383245e-01),  QTCFLLD(2.590900e-07),  QTCFLLD(9.614004e-05),
    QTCFLLD(2.183786e-06),  QTCFLLD(6.867141e-02),  QTCFLLD(7.150523e-01),  QTCFLLD(1.408172e-07),
    QTCFLLD(5.203217e-05),  QTCFLLD(1.584410e-03),  QTCFLLD(6.753749e-03),  QTCFLLD(5.847858e-01),
    QTCFLLD(-9.234326e-07), QTCFLLD(6.477183e-06),  QTCFLLD(-2.123969e-04), QTCFLLD(-7.709230e-02),
    QTCFLLD(4.504798e-01),  QTCFLLD(-2.464033e-08), QTCFLLD(1.888626e-05),  QTCFLLD(-1.812003e-03),
    QTCFLLD(-1.809127e-01), QTCFLLD(3.151743e-01),  QTCFLLD(-8.344882e-07), QTCFLLD(2.538528e-04),
    QTCFLLD(-3.972626e-03), QTCFLLD(-3.017793e-01), QTCFLLD(1.805658e-01),  QTCFLLD(-2.720526e-05),
    QTCFLLD(1.150009e-03),  QTCFLLD(-6.712822e-03), QTCFLLD(-4.351740e-01), QTCFLLD(5.276687e-02),
};

#define QTMFLLD(x) FL2FXCONST_SGL(x / (float)(1 << QMF_MPSLDFB_PFT_SCALE))

/*!
  \name QMF
  \brief  QMF-Table
          32 channels, N = 320,

  The coefficients are derived from the MPS Low Delay coefficient set
  with 640 samples. The coefficients are interpolated and rearranged
  in the following way compared to the reference:

  qmf_mpsldfb_320[0]   = (qmf_64_reference[  0] + qmf_64_reference[  1])/2.0;
  qmf_mpsldfb_320[1]   = (qmf_64_reference[128] + qmf_64_reference[129])/2.0;
  qmf_mpsldfb_320[2]   = (qmf_64_reference[256] + qmf_64_reference[257])/2.0;
  qmf_mpsldfb_320[3]   = (qmf_64_reference[384] + qmf_64_reference[385])/2.0;
  qmf_mpsldfb_320[4]   = (qmf_64_reference[512] + qmf_64_reference[513])/2.0;

  qmf_mpsldfb_320[5]   = (qmf_64_reference[  2] + qmf_64_reference[  3])/2.0;
  qmf_mpsldfb_320[6]   = (qmf_64_reference[130] + qmf_64_reference[131])/2.0;
  qmf_mpsldfb_320[7]   = (qmf_64_reference[258] + qmf_64_reference[259])/2.0;
  qmf_mpsldfb_320[8]   = (qmf_64_reference[386] + qmf_64_reference[387])/2.0;
  qmf_mpsldfb_320[9]   = (qmf_64_reference[514] + qmf_64_reference[515])/2.0;
  .
  .
  .
  qmf_mpsldfb_320[315] = (qmf_64_reference[126] + qmf_64_reference[127])/2.0;
  qmf_mpsldfb_320[316] = (qmf_64_reference[254] + qmf_64_reference[255])/2.0;
  qmf_mpsldfb_320[317] = (qmf_64_reference[382] + qmf_64_reference[383])/2.0;
  qmf_mpsldfb_320[318] = (qmf_64_reference[510] + qmf_64_reference[511])/2.0;
  qmf_mpsldfb_320[319] = (qmf_64_reference[638] + qmf_64_reference[639])/2.0;

  The filter output is required to be scaled by 1 bit.

  \showinitializer
*/
//@{
const FIXP_PFT qmf_mpsldfb_320[QMF320_MPSLDFB_PFT_TABLE_SIZE] = {
    QTMFLLD(1.0777725402e-004),  QTMFLLD(-9.4703806099e-004), QTMFLLD(6.1286436394e-003),  QTMFLLD(-9.0161964297e-002),
    QTMFLLD(5.5554401875e-001),  QTMFLLD(1.2731316383e-004),  QTMFLLD(-1.2311334722e-003), QTMFLLD(4.9468209036e-003),
    QTMFLLD(-1.1305026710e-001), QTMFLLD(5.2990418673e-001),  QTMFLLD(1.1927412561e-004),  QTMFLLD(-1.5128203668e-003),
    QTMFLLD(3.5794533323e-003),  QTMFLLD(-1.3681203127e-001), QTMFLLD(5.0423312187e-001),  QTMFLLD(1.0006380762e-004),
    QTMFLLD(-1.7925058492e-003), QTMFLLD(2.0164034795e-003),  QTMFLLD(-1.6139641404e-001), QTMFLLD(4.7861024737e-001),
    QTMFLLD(7.2826202086e-005),  QTMFLLD(-2.0697340369e-003), QTMFLLD(2.4838969694e-004),  QTMFLLD(-1.8674756587e-001),
    QTMFLLD(4.5311337709e-001),  QTMFLLD(3.8808015233e-005),  QTMFLLD(-2.3429044522e-003), QTMFLLD(-1.7331546405e-003),
    QTMFLLD(-2.1280488372e-001), QTMFLLD(4.2781800032e-001),  QTMFLLD(-5.4359588830e-007), QTMFLLD(-2.6112669148e-003),
    QTMFLLD(-3.9357249625e-003), QTMFLLD(-2.3950359225e-001), QTMFLLD(4.0279802680e-001),  QTMFLLD(-4.3614549213e-005),
    QTMFLLD(-2.8741455171e-003), QTMFLLD(-6.3655078411e-003), QTMFLLD(-2.6677471399e-001), QTMFLLD(3.7812507153e-001),
    QTMFLLD(-8.9040157036e-005), QTMFLLD(-3.1308881007e-003), QTMFLLD(-9.0275555849e-003), QTMFLLD(-2.9454550147e-001),
    QTMFLLD(3.5386830568e-001),  QTMFLLD(-1.3519046479e-004), QTMFLLD(-3.3808732405e-003), QTMFLLD(-1.1925406754e-002),
    QTMFLLD(-3.2273942232e-001), QTMFLLD(3.3009397984e-001),  QTMFLLD(-1.8045579782e-004), QTMFLLD(-3.6236830056e-003),
    QTMFLLD(-1.5061311424e-002), QTMFLLD(-3.5127705336e-001), QTMFLLD(3.0686509609e-001),  QTMFLLD(-2.2396800341e-004),
    QTMFLLD(-3.8587960880e-003), QTMFLLD(-1.8435835838e-002), QTMFLLD(-3.8007527590e-001), QTMFLLD(2.8424069285e-001),
    QTMFLLD(-2.6416976471e-004), QTMFLLD(-4.0859002620e-003), QTMFLLD(-2.2048022598e-002), QTMFLLD(-4.0904915333e-001),
    QTMFLLD(2.6227575541e-001),  QTMFLLD(-3.0001887353e-004), QTMFLLD(-4.3045589700e-003), QTMFLLD(-2.5894984603e-002),
    QTMFLLD(-4.3811064959e-001), QTMFLLD(2.4102044106e-001),  QTMFLLD(-3.3083156450e-004), QTMFLLD(-4.5145484619e-003),
    QTMFLLD(-2.9972121119e-002), QTMFLLD(-4.6717000008e-001), QTMFLLD(2.2052007914e-001),  QTMFLLD(-3.5614447552e-004),
    QTMFLLD(-4.7155953944e-003), QTMFLLD(-3.4272894263e-002), QTMFLLD(-4.9613577127e-001), QTMFLLD(2.0081442595e-001),
    QTMFLLD(-3.7579826312e-004), QTMFLLD(-4.9072988331e-003), QTMFLLD(-3.8788780570e-002), QTMFLLD(-5.2491527796e-001),
    QTMFLLD(1.8193808198e-001),  QTMFLLD(-3.8993739872e-004), QTMFLLD(-5.0893351436e-003), QTMFLLD(-4.3509010226e-002),
    QTMFLLD(-5.5341482162e-001), QTMFLLD(1.6391974688e-001),  QTMFLLD(-3.9912899956e-004), QTMFLLD(-5.2615385503e-003),
    QTMFLLD(-4.8421185464e-002), QTMFLLD(-5.8154034615e-001), QTMFLLD(1.4678207040e-001),  QTMFLLD(-4.0421969607e-004),
    QTMFLLD(-5.4236799479e-003), QTMFLLD(-5.3510606289e-002), QTMFLLD(-6.0919785500e-001), QTMFLLD(1.3054165244e-001),
    QTMFLLD(-4.0645478293e-004), QTMFLLD(-5.5756671354e-003), QTMFLLD(-5.8760054410e-002), QTMFLLD(-6.3629388809e-001),
    QTMFLLD(1.1520925164e-001),  QTMFLLD(-4.0720938705e-004), QTMFLLD(-5.7173836976e-003), QTMFLLD(-6.4149998128e-002),
    QTMFLLD(-6.6273581982e-001), QTMFLLD(1.0078965127e-001),  QTMFLLD(-4.0812738007e-004), QTMFLLD(-5.8488911018e-003),
    QTMFLLD(-6.9658569992e-002), QTMFLLD(-6.8843221664e-001), QTMFLLD(8.7281554937e-002),  QTMFLLD(-4.1120912647e-004),
    QTMFLLD(-5.9703430161e-003), QTMFLLD(-7.5261354446e-002), QTMFLLD(-7.1329379082e-001), QTMFLLD(7.4678033590e-002),
    QTMFLLD(-4.1838851757e-004), QTMFLLD(-6.0821287334e-003), QTMFLLD(-8.0931767821e-002), QTMFLLD(-7.3723363876e-001),
    QTMFLLD(6.2966249883e-002),  QTMFLLD(-4.3148122495e-004), QTMFLLD(-6.1847940087e-003), QTMFLLD(-8.6640790105e-002),
    QTMFLLD(-7.6016783714e-001), QTMFLLD(5.2128262818e-002),  QTMFLLD(-4.5229538227e-004), QTMFLLD(-6.2791546807e-003),
    QTMFLLD(-9.2357128859e-002), QTMFLLD(-7.8201586008e-001), QTMFLLD(4.2139917612e-002),  QTMFLLD(-4.8211280955e-004),
    QTMFLLD(-6.3661932945e-003), QTMFLLD(-9.8047181964e-002), QTMFLLD(-8.0270123482e-001), QTMFLLD(3.2972395420e-002),
    QTMFLLD(-5.2196672186e-004), QTMFLLD(-6.4471233636e-003), QTMFLLD(-1.0367526114e-001), QTMFLLD(-8.2215231657e-001),
    QTMFLLD(2.4589803070e-002),  QTMFLLD(-5.7247944642e-004), QTMFLLD(-6.5232971683e-003), QTMFLLD(-1.0920339823e-001),
    QTMFLLD(-8.4030228853e-001), QTMFLLD(1.6952158883e-002),  QTMFLLD(-6.3343788497e-004), QTMFLLD(-6.5963375382e-003),
    QTMFLLD(-1.1459194124e-001), QTMFLLD(-8.5709118843e-001), QTMFLLD(1.0006074794e-002),  QTMFLLD(-7.0449430496e-004),
    QTMFLLD(-6.6681848839e-003), QTMFLLD(-1.1979964375e-001), QTMFLLD(-8.7246519327e-001), QTMFLLD(3.6968050990e-003),
    QTMFLLD(-7.9609593377e-004), QTMFLLD(-6.7403013818e-003), QTMFLLD(-1.2478165329e-001), QTMFLLD(-8.8632321358e-001),
    QTMFLLD(-1.6344460892e-003), QTMFLLD(-9.0200459817e-004), QTMFLLD(-6.8151149899e-003), QTMFLLD(-1.2949258089e-001),
    QTMFLLD(-8.9860773087e-001), QTMFLLD(-5.9283543378e-003), QTMFLLD(-1.0116943158e-003), QTMFLLD(-6.8955891766e-003),
    QTMFLLD(-1.3388808072e-001), QTMFLLD(-9.0933418274e-001), QTMFLLD(-9.6466485411e-003), QTMFLLD(-1.1244935449e-003),
    QTMFLLD(-6.9835213944e-003), QTMFLLD(-1.3791990280e-001), QTMFLLD(-9.1846722364e-001), QTMFLLD(-1.2838950381e-002),
    QTMFLLD(-1.2393904617e-003), QTMFLLD(-7.0809246972e-003), QTMFLLD(-1.4153905213e-001), QTMFLLD(-9.2597639561e-001),
    QTMFLLD(-1.5539921820e-002), QTMFLLD(-1.3542033266e-003), QTMFLLD(-7.1895248257e-003), QTMFLLD(-1.4469626546e-001),
    QTMFLLD(-9.3183851242e-001), QTMFLLD(-1.7783239484e-002), QTMFLLD(-1.4669501688e-003), QTMFLLD(-7.3110014200e-003),
    QTMFLLD(-1.4734169841e-001), QTMFLLD(-9.3603670597e-001), QTMFLLD(-1.9597738981e-002), QTMFLLD(-1.5753224725e-003),
    QTMFLLD(-7.4466220103e-003), QTMFLLD(-1.4942565560e-001), QTMFLLD(-9.3856132030e-001), QTMFLLD(-2.1011535078e-002),
    QTMFLLD(-1.6771152150e-003), QTMFLLD(-7.5972955674e-003), QTMFLLD(-1.5089863539e-001), QTMFLLD(-9.3940949440e-001),
    QTMFLLD(-2.2049814463e-002), QTMFLLD(-1.7698677257e-003), QTMFLLD(-7.7634919435e-003), QTMFLLD(-1.5171185136e-001),
    QTMFLLD(-9.3858534098e-001), QTMFLLD(-2.2738276049e-002), QTMFLLD(-1.8512960523e-003), QTMFLLD(-7.9450644553e-003),
    QTMFLLD(-1.5181747079e-001), QTMFLLD(-9.3610012531e-001), QTMFLLD(-2.3101080209e-002), QTMFLLD(-1.9192657201e-003),
    QTMFLLD(-8.1413704902e-003), QTMFLLD(-1.5116891265e-001), QTMFLLD(-9.3197190762e-001), QTMFLLD(-2.3163486272e-002),
    QTMFLLD(-1.9716904499e-003), QTMFLLD(-8.3509404212e-003), QTMFLLD(-1.4972095191e-001), QTMFLLD(-9.2622530460e-001),
    QTMFLLD(-2.2950030863e-002), QTMFLLD(-2.0066620782e-003), QTMFLLD(-8.5715763271e-003), QTMFLLD(-1.4743055403e-001),
    QTMFLLD(-9.1889131069e-001), QTMFLLD(-2.2486699745e-002), QTMFLLD(-2.0227057394e-003), QTMFLLD(-8.8005559519e-003),
    QTMFLLD(-1.4425669611e-001), QTMFLLD(-9.1000711918e-001), QTMFLLD(-2.1799135953e-002), QTMFLLD(-2.0185527392e-003),
    QTMFLLD(-9.0341167524e-003), QTMFLLD(-1.4016106725e-001), QTMFLLD(-8.9961612225e-001), QTMFLLD(-2.0914383233e-002),
    QTMFLLD(-1.9932338037e-003), QTMFLLD(-9.2674419284e-003), QTMFLLD(-1.3510815799e-001), QTMFLLD(-8.8776648045e-001),
    QTMFLLD(-1.9859094173e-002), QTMFLLD(-1.9461065531e-003), QTMFLLD(-9.4948727638e-003), QTMFLLD(-1.2906542420e-001),
    QTMFLLD(-8.7451159954e-001), QTMFLLD(-1.8660902977e-002), QTMFLLD(-1.8770052120e-003), QTMFLLD(-9.7100129351e-003),
    QTMFLLD(-1.2200380862e-001), QTMFLLD(-8.5991013050e-001), QTMFLLD(-1.7346922308e-002), QTMFLLD(-1.7859865911e-003),
    QTMFLLD(-9.9056493491e-003), QTMFLLD(-1.1389782280e-001), QTMFLLD(-8.4402561188e-001), QTMFLLD(-1.5944939107e-002),
    QTMFLLD(-1.6734169330e-003), QTMFLLD(-1.0073989630e-002), QTMFLLD(-1.0472598672e-001), QTMFLLD(-8.2692527771e-001),
    QTMFLLD(-1.4481747523e-002), QTMFLLD(-1.5399802942e-003), QTMFLLD(-1.0205906816e-002), QTMFLLD(-9.4470888376e-002),
    QTMFLLD(-8.0868041515e-001), QTMFLLD(-1.2984249741e-002), QTMFLLD(-1.3865872752e-003), QTMFLLD(-1.0291703977e-002),
    QTMFLLD(-8.3119556308e-002), QTMFLLD(-7.8936588764e-001), QTMFLLD(-1.1477986351e-002), QTMFLLD(-1.2144348584e-003),
    QTMFLLD(-1.0320962407e-002), QTMFLLD(-7.0663399994e-002), QTMFLLD(-7.6905936003e-001), QTMFLLD(-9.9884867668e-003),
    QTMFLLD(-1.0248266626e-003), QTMFLLD(-1.0282764211e-002), QTMFLLD(-5.7098604739e-002), QTMFLLD(-7.4784147739e-001),
    QTMFLLD(-8.5393209010e-003), QTMFLLD(-8.1919803051e-004), QTMFLLD(-1.0165717453e-002), QTMFLLD(-4.2426198721e-002),
    QTMFLLD(-7.2579479218e-001), QTMFLLD(-7.1533406153e-003), QTMFLLD(-5.9914286248e-004), QTMFLLD(-9.9579729140e-003),
    QTMFLLD(-2.6652012020e-002), QTMFLLD(-7.0300412178e-001), QTMFLLD(-5.8508114889e-003), QTMFLLD(-3.6626873771e-004),
    QTMFLLD(-9.6475090832e-003), QTMFLLD(-9.7871217877e-003), QTMFLLD(-6.7955517769e-001), QTMFLLD(-4.6512838453e-003),
    QTMFLLD(-1.2227181287e-004), QTMFLLD(-9.2221321538e-003), QTMFLLD(8.1523396075e-003),  QTMFLLD(-6.5553492308e-001),
    QTMFLLD(-3.5699680448e-003), QTMFLLD(1.3090072025e-004),  QTMFLLD(-8.6695179343e-003), QTMFLLD(2.7145106345e-002),
    QTMFLLD(-6.3103044033e-001), QTMFLLD(-2.6181070134e-003), QTMFLLD(3.9128778735e-004),  QTMFLLD(-7.9773496836e-003),
    QTMFLLD(4.7164849937e-002),  QTMFLLD(-6.0613000393e-001), QTMFLLD(-1.7908872105e-003), QTMFLLD(6.5761915175e-004),
    QTMFLLD(-7.1337916888e-003), QTMFLLD(6.8181537092e-002),  QTMFLLD(-5.8092808723e-001), QTMFLLD(-1.0135001503e-003)};

/*!
  \name QMF
  \brief  QMF-Table
          64 channels, N = 640,

  The coeffs are rearranged compared with the reference in the following
  way:

  qmf_64[0]   = qmf_64_reference[0];
  qmf_64[1]   = qmf_64_reference[128];
  qmf_64[2]   = qmf_64_reference[256];
  qmf_64[3]   = qmf_64_reference[384];
  qmf_64[4]   = qmf_64_reference[512];

  qmf_64[5]   = qmf_64_reference[1];
  qmf_64[6]   = qmf_64_reference[129];
  qmf_64[7]   = qmf_64_reference[257];
  qmf_64[8]   = qmf_64_reference[385];
  qmf_64[9]   = qmf_64_reference[513];
  .
  .
  .
  qmf_64[635] = qmf_64_reference[127]
  qmf_64[636] = qmf_64_reference[255];
  qmf_64[637] = qmf_64_reference[383];
  qmf_64[638] = qmf_64_reference[511];
  qmf_64[639] = qmf_64_reference[639];

  The filter output is required to be scaled by 1 bit.

  \showinitializer
*/
//@{
LNK_SECTION_CONSTDATA_L1
RAM_ALIGN
const FIXP_PFT qmf_mpsldfb_640[QMF640_MPSLDFB_PFT_TABLE_SIZE] = {
    QTMFLLD(9.3863010989e-005),  QTMFLLD(-8.7536586216e-004), QTMFLLD(6.4016343094e-003),  QTMFLLD(-8.4552817047e-002),
    QTMFLLD(5.6194400787e-001),  QTMFLLD(1.2169149704e-004),  QTMFLLD(-1.0187102016e-003), QTMFLLD(5.8556534350e-003),
    QTMFLLD(-9.5771118999e-002), QTMFLLD(5.4914402962e-001),  QTMFLLD(1.2793767382e-004),  QTMFLLD(-1.1605311884e-003),
    QTMFLLD(5.2649765275e-003),  QTMFLLD(-1.0721673071e-001), QTMFLLD(5.3632181883e-001),  QTMFLLD(1.2668863928e-004),
    QTMFLLD(-1.3017356396e-003), QTMFLLD(4.6286652796e-003),  QTMFLLD(-1.1888379604e-001), QTMFLLD(5.2348655462e-001),
    QTMFLLD(1.2296593923e-004),  QTMFLLD(-1.4426353155e-003), QTMFLLD(3.9453012869e-003),  QTMFLLD(-1.3076621294e-001),
    QTMFLLD(5.1064836979e-001),  QTMFLLD(1.1558231199e-004),  QTMFLLD(-1.5830053017e-003), QTMFLLD(3.2136053778e-003),
    QTMFLLD(-1.4285783470e-001), QTMFLLD(4.9781781435e-001),  QTMFLLD(1.0582985124e-004),  QTMFLLD(-1.7228506040e-003),
    QTMFLLD(2.4323666003e-003),  QTMFLLD(-1.5515175462e-001), QTMFLLD(4.8500382900e-001),  QTMFLLD(9.4297764008e-005),
    QTMFLLD(-1.8621610943e-003), QTMFLLD(1.6004402423e-003),  QTMFLLD(-1.6764105856e-001), QTMFLLD(4.7221666574e-001),
    QTMFLLD(8.0514568253e-005),  QTMFLLD(-2.0008818246e-003), QTMFLLD(7.1672687773e-004),  QTMFLLD(-1.8031860888e-001),
    QTMFLLD(4.5946595073e-001),  QTMFLLD(6.5137835918e-005),  QTMFLLD(-2.1385864820e-003), QTMFLLD(-2.1994746930e-004),
    QTMFLLD(-1.9317652285e-001), QTMFLLD(4.4676083326e-001),  QTMFLLD(4.8101064749e-005),  QTMFLLD(-2.2751907818e-003),
    QTMFLLD(-1.2104592752e-003), QTMFLLD(-2.0620720088e-001), QTMFLLD(4.3411090970e-001),  QTMFLLD(2.9514967537e-005),
    QTMFLLD(-2.4106178898e-003), QTMFLLD(-2.2558500059e-003), QTMFLLD(-2.1940255165e-001), QTMFLLD(4.2152509093e-001),
    QTMFLLD(9.8814107332e-006),  QTMFLLD(-2.5448307861e-003), QTMFLLD(-3.3569468651e-003), QTMFLLD(-2.3275400698e-001),
    QTMFLLD(4.0901294351e-001),  QTMFLLD(-1.0968602510e-005), QTMFLLD(-2.6777030434e-003), QTMFLLD(-4.5145032927e-003),
    QTMFLLD(-2.4625316262e-001), QTMFLLD(3.9658311009e-001),  QTMFLLD(-3.2559255487e-005), QTMFLLD(-2.8091520071e-003),
    QTMFLLD(-5.7292259298e-003), QTMFLLD(-2.5989097357e-001), QTMFLLD(3.8424444199e-001),  QTMFLLD(-5.4669842939e-005),
    QTMFLLD(-2.9391390271e-003), QTMFLLD(-7.0017897524e-003), QTMFLLD(-2.7365845442e-001), QTMFLLD(3.7200567126e-001),
    QTMFLLD(-7.7506563684e-005), QTMFLLD(-3.0675258022e-003), QTMFLLD(-8.3327051252e-003), QTMFLLD(-2.8754624724e-001),
    QTMFLLD(3.5987523198e-001),  QTMFLLD(-1.0057374311e-004), QTMFLLD(-3.1942503992e-003), QTMFLLD(-9.7224051133e-003),
    QTMFLLD(-3.0154475570e-001), QTMFLLD(3.4786140919e-001),  QTMFLLD(-1.2368557509e-004), QTMFLLD(-3.3192564733e-003),
    QTMFLLD(-1.1171258055e-002), QTMFLLD(-3.1564420462e-001), QTMFLLD(3.3597227931e-001),  QTMFLLD(-1.4669535449e-004),
    QTMFLLD(-3.4424900077e-003), QTMFLLD(-1.2679555453e-002), QTMFLLD(-3.2983466983e-001), QTMFLLD(3.2421571016e-001),
    QTMFLLD(-1.6928518016e-004), QTMFLLD(-3.5639149137e-003), QTMFLLD(-1.4247507788e-002), QTMFLLD(-3.4410607815e-001),
    QTMFLLD(3.1259948015e-001),  QTMFLLD(-1.9162640092e-004), QTMFLLD(-3.6834510975e-003), QTMFLLD(-1.5875114128e-002),
    QTMFLLD(-3.5844799876e-001), QTMFLLD(3.0113074183e-001),  QTMFLLD(-2.1345751884e-004), QTMFLLD(-3.8009947166e-003),
    QTMFLLD(-1.7562393099e-002), QTMFLLD(-3.7284970284e-001), QTMFLLD(2.8981682658e-001),  QTMFLLD(-2.3447850253e-004),
    QTMFLLD(-3.9165974595e-003), QTMFLLD(-1.9309276715e-002), QTMFLLD(-3.8730087876e-001), QTMFLLD(2.7866455913e-001),
    QTMFLLD(-2.5462667691e-004), QTMFLLD(-4.0301652625e-003), QTMFLLD(-2.1115457639e-002), QTMFLLD(-4.0179058909e-001),
    QTMFLLD(2.6768052578e-001),  QTMFLLD(-2.7371285250e-004), QTMFLLD(-4.1416347958e-003), QTMFLLD(-2.2980585694e-002),
    QTMFLLD(-4.1630774736e-001), QTMFLLD(2.5687095523e-001),  QTMFLLD(-2.9165804153e-004), QTMFLLD(-4.2509674095e-003),
    QTMFLLD(-2.4904217571e-002), QTMFLLD(-4.3084129691e-001), QTMFLLD(2.4624188244e-001),  QTMFLLD(-3.0837973463e-004),
    QTMFLLD(-4.3581505306e-003), QTMFLLD(-2.6885753497e-002), QTMFLLD(-4.4538003206e-001), QTMFLLD(2.3579898477e-001),
    QTMFLLD(-3.2378203468e-004), QTMFLLD(-4.4631510973e-003), QTMFLLD(-2.8924530372e-002), QTMFLLD(-4.5991250873e-001),
    QTMFLLD(2.2554755211e-001),  QTMFLLD(-3.3788106521e-004), QTMFLLD(-4.5659458265e-003), QTMFLLD(-3.1019711867e-002),
    QTMFLLD(-4.7442746162e-001), QTMFLLD(2.1549259126e-001),  QTMFLLD(-3.5053401371e-004), QTMFLLD(-4.6664695255e-003),
    QTMFLLD(-3.3170353621e-002), QTMFLLD(-4.8891320825e-001), QTMFLLD(2.0563863218e-001),  QTMFLLD(-3.6175493733e-004),
    QTMFLLD(-4.7647207975e-003), QTMFLLD(-3.5375438631e-002), QTMFLLD(-5.0335830450e-001), QTMFLLD(1.9599021971e-001),
    QTMFLLD(-3.7159718340e-004), QTMFLLD(-4.8605888151e-003), QTMFLLD(-3.7633713335e-002), QTMFLLD(-5.1775097847e-001),
    QTMFLLD(1.8655113876e-001),  QTMFLLD(-3.7999937194e-004), QTMFLLD(-4.9540083855e-003), QTMFLLD(-3.9943847805e-002),
    QTMFLLD(-5.3207957745e-001), QTMFLLD(1.7732504010e-001),  QTMFLLD(-3.8705617771e-004), QTMFLLD(-5.0450465642e-003),
    QTMFLLD(-4.2304381728e-002), QTMFLLD(-5.4633224010e-001), QTMFLLD(1.6831515729e-001),  QTMFLLD(-3.9281861973e-004),
    QTMFLLD(-5.1336232573e-003), QTMFLLD(-4.4713638723e-002), QTMFLLD(-5.6049734354e-001), QTMFLLD(1.5952435136e-001),
    QTMFLLD(-3.9737694897e-004), QTMFLLD(-5.2197398618e-003), QTMFLLD(-4.7170232981e-002), QTMFLLD(-5.7456302643e-001),
    QTMFLLD(1.5095503628e-001),  QTMFLLD(-4.0088107926e-004), QTMFLLD(-5.3033372387e-003), QTMFLLD(-4.9672137946e-002),
    QTMFLLD(-5.8851766586e-001), QTMFLLD(1.4260910451e-001),  QTMFLLD(-4.0338383405e-004), QTMFLLD(-5.3843962960e-003),
    QTMFLLD(-5.2217379212e-002), QTMFLLD(-6.0234934092e-001), QTMFLLD(1.3448855281e-001),  QTMFLLD(-4.0505555808e-004),
    QTMFLLD(-5.4629631341e-003), QTMFLLD(-5.4803829640e-002), QTMFLLD(-6.1604642868e-001), QTMFLLD(1.2659475207e-001),
    QTMFLLD(-4.0614881436e-004), QTMFLLD(-5.5389581248e-003), QTMFLLD(-5.7429198176e-002), QTMFLLD(-6.2959736586e-001),
    QTMFLLD(1.1892842501e-001),  QTMFLLD(-4.0676075150e-004), QTMFLLD(-5.6123761460e-003), QTMFLLD(-6.0090914369e-002),
    QTMFLLD(-6.4299046993e-001), QTMFLLD(1.1149007827e-001),  QTMFLLD(-4.0709332097e-004), QTMFLLD(-5.6832311675e-003),
    QTMFLLD(-6.2786586583e-002), QTMFLLD(-6.5621429682e-001), QTMFLLD(1.0428040475e-001),  QTMFLLD(-4.0732545312e-004),
    QTMFLLD(-5.7515366934e-003), QTMFLLD(-6.5513409674e-002), QTMFLLD(-6.6925734282e-001), QTMFLLD(9.7298897803e-002),
    QTMFLLD(-4.0770808118e-004), QTMFLLD(-5.8172862045e-003), QTMFLLD(-6.8268470466e-002), QTMFLLD(-6.8210834265e-001),
    QTMFLLD(9.0545162559e-002),  QTMFLLD(-4.0854664985e-004), QTMFLLD(-5.8804959990e-003), QTMFLLD(-7.1048669517e-002),
    QTMFLLD(-6.9475615025e-001), QTMFLLD(8.4017947316e-002),  QTMFLLD(-4.1002241778e-004), QTMFLLD(-5.9412117116e-003),
    QTMFLLD(-7.3850922287e-002), QTMFLLD(-7.0718955994e-001), QTMFLLD(7.7716566622e-002),  QTMFLLD(-4.1239586426e-004),
    QTMFLLD(-5.9994738549e-003), QTMFLLD(-7.6671779156e-002), QTMFLLD(-7.1939796209e-001), QTMFLLD(7.1639508009e-002),
    QTMFLLD(-4.1594370850e-004), QTMFLLD(-6.0553550720e-003), QTMFLLD(-7.9507902265e-002), QTMFLLD(-7.3137050867e-001),
    QTMFLLD(6.5784148872e-002),  QTMFLLD(-4.2083335575e-004), QTMFLLD(-6.1089023948e-003), QTMFLLD(-8.2355625927e-002),
    QTMFLLD(-7.4309676886e-001), QTMFLLD(6.0148354620e-002),  QTMFLLD(-4.2732476140e-004), QTMFLLD(-6.1602159403e-003),
    QTMFLLD(-8.5211075842e-002), QTMFLLD(-7.5456637144e-001), QTMFLLD(5.4730266333e-002),  QTMFLLD(-4.3563771760e-004),
    QTMFLLD(-6.2093720771e-003), QTMFLLD(-8.8070511818e-002), QTMFLLD(-7.6576924324e-001), QTMFLLD(4.9526259303e-002),
    QTMFLLD(-4.4600359979e-004), QTMFLLD(-6.2565426342e-003), QTMFLLD(-9.0929701924e-002), QTMFLLD(-7.7669566870e-001),
    QTMFLLD(4.4533081353e-002),  QTMFLLD(-4.5858716476e-004), QTMFLLD(-6.3017667271e-003), QTMFLLD(-9.3784548342e-002),
    QTMFLLD(-7.8733605146e-001), QTMFLLD(3.9746750146e-002),  QTMFLLD(-4.7345875646e-004), QTMFLLD(-6.3452622853e-003),
    QTMFLLD(-9.6630692482e-002), QTMFLLD(-7.9768097401e-001), QTMFLLD(3.5163912922e-002),  QTMFLLD(-4.9076689174e-004),
    QTMFLLD(-6.3871243037e-003), QTMFLLD(-9.9463671446e-002), QTMFLLD(-8.0772149563e-001), QTMFLLD(3.0780877918e-002),
    QTMFLLD(-5.1067111781e-004), QTMFLLD(-6.4275567420e-003), QTMFLLD(-1.0227891803e-001), QTMFLLD(-8.1744915247e-001),
    QTMFLLD(2.6590615511e-002),  QTMFLLD(-5.3326232592e-004), QTMFLLD(-6.4666904509e-003), QTMFLLD(-1.0507161170e-001),
    QTMFLLD(-8.2685548067e-001), QTMFLLD(2.2588992491e-002),  QTMFLLD(-5.5855646497e-004), QTMFLLD(-6.5047293901e-003),
    QTMFLLD(-1.0783691704e-001), QTMFLLD(-8.3593225479e-001), QTMFLLD(1.8772648647e-002),  QTMFLLD(-5.8640236966e-004),
    QTMFLLD(-6.5418654121e-003), QTMFLLD(-1.1056987941e-001), QTMFLLD(-8.4467232227e-001), QTMFLLD(1.5131668188e-002),
    QTMFLLD(-6.1692652525e-004), QTMFLLD(-6.5783206373e-003), QTMFLLD(-1.1326543987e-001), QTMFLLD(-8.5306841135e-001),
    QTMFLLD(1.1661184952e-002),  QTMFLLD(-6.4994930290e-004), QTMFLLD(-6.6143544391e-003), QTMFLLD(-1.1591844261e-001),
    QTMFLLD(-8.6111402512e-001), QTMFLLD(8.3509646356e-003),  QTMFLLD(-6.8494328298e-004), QTMFLLD(-6.6502285190e-003),
    QTMFLLD(-1.1852371693e-001), QTMFLLD(-8.6880439520e-001), QTMFLLD(5.1832948811e-003),  QTMFLLD(-7.2404538514e-004),
    QTMFLLD(-6.6861407831e-003), QTMFLLD(-1.2107557058e-001), QTMFLLD(-8.7612599134e-001), QTMFLLD(2.2103153169e-003),
    QTMFLLD(-7.7061145566e-004), QTMFLLD(-6.7221261561e-003), QTMFLLD(-1.2356808037e-001), QTMFLLD(-8.8305824995e-001),
    QTMFLLD(-4.6855807886e-004), QTMFLLD(-8.2158041187e-004), QTMFLLD(-6.7584766075e-003), QTMFLLD(-1.2599521875e-001),
    QTMFLLD(-8.8958823681e-001), QTMFLLD(-2.8003340121e-003), QTMFLLD(-8.7498105131e-004), QTMFLLD(-6.7957863212e-003),
    QTMFLLD(-1.2835204601e-001), QTMFLLD(-8.9572954178e-001), QTMFLLD(-4.9293786287e-003), QTMFLLD(-9.2902814504e-004),
    QTMFLLD(-6.8344431929e-003), QTMFLLD(-1.3063311577e-001), QTMFLLD(-9.0148586035e-001), QTMFLLD(-6.9273295812e-003),
    QTMFLLD(-9.8383461591e-004), QTMFLLD(-6.8746237084e-003), QTMFLLD(-1.3283239305e-001), QTMFLLD(-9.0685033798e-001),
    QTMFLLD(-8.7857460603e-003), QTMFLLD(-1.0395538993e-003), QTMFLLD(-6.9165546447e-003), QTMFLLD(-1.3494376838e-001),
    QTMFLLD(-9.1181802750e-001), QTMFLLD(-1.0507551953e-002), QTMFLLD(-1.0959620122e-003), QTMFLLD(-6.9604511373e-003),
    QTMFLLD(-1.3696120679e-001), QTMFLLD(-9.1638565063e-001), QTMFLLD(-1.2103702873e-002), QTMFLLD(-1.1530250777e-003),
    QTMFLLD(-7.0065916516e-003), QTMFLLD(-1.3887859881e-001), QTMFLLD(-9.2054879665e-001), QTMFLLD(-1.3574197888e-002),
    QTMFLLD(-1.2105966453e-003), QTMFLLD(-7.0552495308e-003), QTMFLLD(-1.4068968594e-001), QTMFLLD(-9.2430406809e-001),
    QTMFLLD(-1.4923358336e-002), QTMFLLD(-1.2681842782e-003), QTMFLLD(-7.1066003293e-003), QTMFLLD(-1.4238841832e-001),
    QTMFLLD(-9.2764878273e-001), QTMFLLD(-1.6156485304e-002), QTMFLLD(-1.3256429229e-003), QTMFLLD(-7.1608433500e-003),
    QTMFLLD(-1.4396859705e-001), QTMFLLD(-9.3058031797e-001), QTMFLLD(-1.7277117819e-002), QTMFLLD(-1.3827638468e-003),
    QTMFLLD(-7.2182063013e-003), QTMFLLD(-1.4542391896e-001), QTMFLLD(-9.3309664726e-001), QTMFLLD(-1.8289361149e-002),
    QTMFLLD(-1.4391905861e-003), QTMFLLD(-7.2789187543e-003), QTMFLLD(-1.4674818516e-001), QTMFLLD(-9.3519610167e-001),
    QTMFLLD(-1.9195662811e-002), QTMFLLD(-1.4947097516e-003), QTMFLLD(-7.3430840857e-003), QTMFLLD(-1.4793521166e-001),
    QTMFLLD(-9.3687731028e-001), QTMFLLD(-1.9999813288e-002), QTMFLLD(-1.5489540529e-003), QTMFLLD(-7.4108825065e-003),
    QTMFLLD(-1.4897871017e-001), QTMFLLD(-9.3813979626e-001), QTMFLLD(-2.0706148818e-002), QTMFLLD(-1.6016908921e-003),
    QTMFLLD(-7.4823615141e-003), QTMFLLD(-1.4987260103e-001), QTMFLLD(-9.3898290396e-001), QTMFLLD(-2.1316919476e-002),
    QTMFLLD(-1.6526894178e-003), QTMFLLD(-7.5576924719e-003), QTMFLLD(-1.5061059594e-001), QTMFLLD(-9.3940681219e-001),
    QTMFLLD(-2.1835187450e-002), QTMFLLD(-1.7015410122e-003), QTMFLLD(-7.6368991286e-003), QTMFLLD(-1.5118667483e-001),
    QTMFLLD(-9.3941211700e-001), QTMFLLD(-2.2264443338e-002), QTMFLLD(-1.7479787348e-003), QTMFLLD(-7.7200052328e-003),
    QTMFLLD(-1.5159477293e-001), QTMFLLD(-9.3899971247e-001), QTMFLLD(-2.2607907653e-002), QTMFLLD(-1.7917567166e-003),
    QTMFLLD(-7.8069791198e-003), QTMFLLD(-1.5182891488e-001), QTMFLLD(-9.3817096949e-001), QTMFLLD(-2.2868644446e-002),
    QTMFLLD(-1.8325200072e-003), QTMFLLD(-7.8977877274e-003), QTMFLLD(-1.5188319981e-001), QTMFLLD(-9.3692785501e-001),
    QTMFLLD(-2.3049183190e-002), QTMFLLD(-1.8700722139e-003), QTMFLLD(-7.9923402518e-003), QTMFLLD(-1.5175175667e-001),
    QTMFLLD(-9.3527245522e-001), QTMFLLD(-2.3152977228e-002), QTMFLLD(-1.9041235792e-003), QTMFLLD(-8.0905584618e-003),
    QTMFLLD(-1.5142890811e-001), QTMFLLD(-9.3320751190e-001), QTMFLLD(-2.3183524609e-002), QTMFLLD(-1.9344078610e-003),
    QTMFLLD(-8.1921815872e-003), QTMFLLD(-1.5090890229e-001), QTMFLLD(-9.3073624372e-001), QTMFLLD(-2.3143447936e-002),
    QTMFLLD(-1.9606938586e-003), QTMFLLD(-8.2970457152e-003), QTMFLLD(-1.5018628538e-001), QTMFLLD(-9.2786192894e-001),
    QTMFLLD(-2.3035895079e-002), QTMFLLD(-1.9826870412e-003), QTMFLLD(-8.4048351273e-003), QTMFLLD(-1.4925561845e-001),
    QTMFLLD(-9.2458862066e-001), QTMFLLD(-2.2864164785e-002), QTMFLLD(-2.0002126694e-003), QTMFLLD(-8.5152359679e-003),
    QTMFLLD(-1.4811170101e-001), QTMFLLD(-9.2092043161e-001), QTMFLLD(-2.2631708533e-002), QTMFLLD(-2.0131117199e-003),
    QTMFLLD(-8.6279176176e-003), QTMFLLD(-1.4674940705e-001), QTMFLLD(-9.1686213017e-001), QTMFLLD(-2.2341690958e-002),
    QTMFLLD(-2.0211567171e-003), QTMFLLD(-8.7425475940e-003), QTMFLLD(-1.4516362548e-001), QTMFLLD(-9.1241872311e-001),
    QTMFLLD(-2.1996961907e-002), QTMFLLD(-2.0242547616e-003), QTMFLLD(-8.8585643098e-003), QTMFLLD(-1.4334976673e-001),
    QTMFLLD(-9.0759557486e-001), QTMFLLD(-2.1601308137e-002), QTMFLLD(-2.0221893210e-003), QTMFLLD(-8.9755039662e-003),
    QTMFLLD(-1.4130303264e-001), QTMFLLD(-9.0239852667e-001), QTMFLLD(-2.1158147603e-002), QTMFLLD(-2.0149163902e-003),
    QTMFLLD(-9.0927295387e-003), QTMFLLD(-1.3901908696e-001), QTMFLLD(-8.9683371782e-001), QTMFLLD(-2.0670616999e-002),
    QTMFLLD(-2.0022888202e-003), QTMFLLD(-9.2095714062e-003), QTMFLLD(-1.3649365306e-001), QTMFLLD(-8.9090716839e-001),
    QTMFLLD(-2.0142132416e-002), QTMFLLD(-1.9841785543e-003), QTMFLLD(-9.3253115192e-003), QTMFLLD(-1.3372266293e-001),
    QTMFLLD(-8.8462579250e-001), QTMFLLD(-1.9576057792e-002), QTMFLLD(-1.9606270362e-003), QTMFLLD(-9.4392402098e-003),
    QTMFLLD(-1.3070219755e-001), QTMFLLD(-8.7799650431e-001), QTMFLLD(-1.8976125866e-002), QTMFLLD(-1.9315859536e-003),
    QTMFLLD(-9.5505062491e-003), QTMFLLD(-1.2742865086e-001), QTMFLLD(-8.7102663517e-001), QTMFLLD(-1.8345680088e-002),
    QTMFLLD(-1.8970289966e-003), QTMFLLD(-9.6583357081e-003), QTMFLLD(-1.2389861047e-001), QTMFLLD(-8.6372399330e-001),
    QTMFLLD(-1.7687706277e-002), QTMFLLD(-1.8569815438e-003), QTMFLLD(-9.7616901621e-003), QTMFLLD(-1.2010899931e-001),
    QTMFLLD(-8.5609632730e-001), QTMFLLD(-1.7006140202e-002), QTMFLLD(-1.8114587292e-003), QTMFLLD(-9.8597351462e-003),
    QTMFLLD(-1.1605655402e-001), QTMFLLD(-8.4815198183e-001), QTMFLLD(-1.6304368153e-002), QTMFLLD(-1.7605143366e-003),
    QTMFLLD(-9.9515644833e-003), QTMFLLD(-1.1173909158e-001), QTMFLLD(-8.3989918232e-001), QTMFLLD(-1.5585509129e-002),
    QTMFLLD(-1.7042002873e-003), QTMFLLD(-1.0036026128e-002), QTMFLLD(-1.0715358704e-001), QTMFLLD(-8.3134686947e-001),
    QTMFLLD(-1.4853162691e-002), QTMFLLD(-1.6426335787e-003), QTMFLLD(-1.0111952201e-002), QTMFLLD(-1.0229838639e-001),
    QTMFLLD(-8.2250368595e-001), QTMFLLD(-1.4110331424e-002), QTMFLLD(-1.5758809168e-003), QTMFLLD(-1.0178210214e-002),
    QTMFLLD(-9.7171187401e-002), QTMFLLD(-8.1337898970e-001), QTMFLLD(-1.3360806741e-002), QTMFLLD(-1.5040797880e-003),
    QTMFLLD(-1.0233603418e-002), QTMFLLD(-9.1770596802e-002), QTMFLLD(-8.0398184061e-001), QTMFLLD(-1.2607692741e-002),
    QTMFLLD(-1.4273397392e-003), QTMFLLD(-1.0276827961e-002), QTMFLLD(-8.6095176637e-002), QTMFLLD(-7.9432225227e-001),
    QTMFLLD(-1.1853585951e-002), QTMFLLD(-1.3458349276e-003), QTMFLLD(-1.0306579992e-002), QTMFLLD(-8.0143928528e-002),
    QTMFLLD(-7.8440952301e-001), QTMFLLD(-1.1102385819e-002), QTMFLLD(-1.2597256573e-003), QTMFLLD(-1.0321546346e-002),
    QTMFLLD(-7.3915921152e-002), QTMFLLD(-7.7425378561e-001), QTMFLLD(-1.0356968269e-002), QTMFLLD(-1.1691439431e-003),
    QTMFLLD(-1.0320378467e-002), QTMFLLD(-6.7410878837e-002), QTMFLLD(-7.6386493444e-001), QTMFLLD(-9.6200043336e-003),
    QTMFLLD(-1.0743001476e-003), QTMFLLD(-1.0301630013e-002), QTMFLLD(-6.0628447682e-002), QTMFLLD(-7.5325345993e-001),
    QTMFLLD(-8.8949296623e-003), QTMFLLD(-9.7535311943e-004), QTMFLLD(-1.0263898410e-002), QTMFLLD(-5.3568758070e-002),
    QTMFLLD(-7.4242949486e-001), QTMFLLD(-8.1837112084e-003), QTMFLLD(-8.7248592172e-004), QTMFLLD(-1.0205759667e-002),
    QTMFLLD(-4.6232450753e-002), QTMFLLD(-7.3140352964e-001), QTMFLLD(-7.4901022017e-003), QTMFLLD(-7.6591013931e-004),
    QTMFLLD(-1.0125675239e-002), QTMFLLD(-3.8619950414e-002), QTMFLLD(-7.2018599510e-001), QTMFLLD(-6.8165790290e-003),
    QTMFLLD(-6.5580842784e-004), QTMFLLD(-1.0022218339e-002), QTMFLLD(-3.0732547864e-002), QTMFLLD(-7.0878815651e-001),
    QTMFLLD(-6.1642420478e-003), QTMFLLD(-5.4247735534e-004), QTMFLLD(-9.8937284201e-003), QTMFLLD(-2.2571478039e-002),
    QTMFLLD(-6.9722014666e-001), QTMFLLD(-5.5373813957e-003), QTMFLLD(-4.2596619460e-004), QTMFLLD(-9.7389295697e-003),
    QTMFLLD(-1.4138570987e-002), QTMFLLD(-6.8549299240e-001), QTMFLLD(-4.9372608773e-003), QTMFLLD(-3.0657128082e-004),
    QTMFLLD(-9.5560895279e-003), QTMFLLD(-5.4356725886e-003), QTMFLLD(-6.7361742258e-001), QTMFLLD(-4.3653072789e-003),
    QTMFLLD(-1.8451632059e-004), QTMFLLD(-9.3438196927e-003), QTMFLLD(3.5346730147e-003),  QTMFLLD(-6.6160440445e-001),
    QTMFLLD(-3.8251809310e-003), QTMFLLD(-6.0027297877e-005), QTMFLLD(-9.1004446149e-003), QTMFLLD(1.2770005502e-002),
    QTMFLLD(-6.4946544170e-001), QTMFLLD(-3.3147553913e-003), QTMFLLD(6.6618180426e-005),  QTMFLLD(-8.8245263323e-003),
    QTMFLLD(2.2267201915e-002),  QTMFLLD(-6.3721030951e-001), QTMFLLD(-2.8387091588e-003), QTMFLLD(1.9518326735e-004),
    QTMFLLD(-8.5145104676e-003), QTMFLLD(3.2023012638e-002),  QTMFLLD(-6.2485051155e-001), QTMFLLD(-2.3975048680e-003),
    QTMFLLD(3.2545044087e-004),  QTMFLLD(-8.1687811762e-003), QTMFLLD(4.2033810169e-002),  QTMFLLD(-6.1239802837e-001),
    QTMFLLD(-1.9807203207e-003), QTMFLLD(4.5712510473e-004),  QTMFLLD(-7.7859172598e-003), QTMFLLD(5.2295893431e-002),
    QTMFLLD(-5.9986191988e-001), QTMFLLD(-1.6010539839e-003), QTMFLLD(5.9015140869e-004),  QTMFLLD(-7.3645371012e-003),
    QTMFLLD(6.2805138528e-002),  QTMFLLD(-5.8725595474e-001), QTMFLLD(-1.2320743408e-003), QTMFLLD(7.2508689482e-004),
    QTMFLLD(-6.9030462764e-003), QTMFLLD(7.3557935655e-002),  QTMFLLD(-5.7460016012e-001), QTMFLLD(-7.9492607620e-004)};

//@{
/*!
  \name DCT_II twiddle factors, L=64
*/
/*! sin (3.14159265358979323 / (2*L) * n) , L=64*/
LNK_SECTION_CONSTDATA
RAM_ALIGN
const FIXP_WTP sin_twiddle_L64[] = {
    WTCP(0x7fffffff, 0x00000000), WTCP(0x7ff62182, 0x03242abf), WTCP(0x7fd8878e, 0x0647d97c),
    WTCP(0x7fa736b4, 0x096a9049), WTCP(0x7f62368f, 0x0c8bd35e), WTCP(0x7f0991c4, 0x0fab272b),
    WTCP(0x7e9d55fc, 0x12c8106f), WTCP(0x7e1d93ea, 0x15e21445), WTCP(0x7d8a5f40, 0x18f8b83c),
    WTCP(0x7ce3ceb2, 0x1c0b826a), WTCP(0x7c29fbee, 0x1f19f97b), WTCP(0x7b5d039e, 0x2223a4c5),
    WTCP(0x7a7d055b, 0x25280c5e), WTCP(0x798a23b1, 0x2826b928), WTCP(0x78848414, 0x2b1f34eb),
    WTCP(0x776c4edb, 0x2e110a62), WTCP(0x7641af3d, 0x30fbc54d), WTCP(0x7504d345, 0x33def287),
    WTCP(0x73b5ebd1, 0x36ba2014), WTCP(0x72552c85, 0x398cdd32), WTCP(0x70e2cbc6, 0x3c56ba70),
    WTCP(0x6f5f02b2, 0x3f1749b8), WTCP(0x6dca0d14, 0x41ce1e65), WTCP(0x6c242960, 0x447acd50),
    WTCP(0x6a6d98a4, 0x471cece7), WTCP(0x68a69e81, 0x49b41533), WTCP(0x66cf8120, 0x4c3fdff4),
    WTCP(0x64e88926, 0x4ebfe8a5), WTCP(0x62f201ac, 0x5133cc94), WTCP(0x60ec3830, 0x539b2af0),
    WTCP(0x5ed77c8a, 0x55f5a4d2), WTCP(0x5cb420e0, 0x5842dd54), WTCP(0x5a82799a, 0x5a82799a),
    WTCP(0x5842dd54, 0x5cb420e0), WTCP(0x55f5a4d2, 0x5ed77c8a), WTCP(0x539b2af0, 0x60ec3830),
    WTCP(0x5133cc94, 0x62f201ac), WTCP(0x4ebfe8a5, 0x64e88926), WTCP(0x4c3fdff4, 0x66cf8120),
    WTCP(0x49b41533, 0x68a69e81), WTCP(0x471cece7, 0x6a6d98a4), WTCP(0x447acd50, 0x6c242960),
    WTCP(0x41ce1e65, 0x6dca0d14), WTCP(0x3f1749b8, 0x6f5f02b2), WTCP(0x3c56ba70, 0x70e2cbc6),
    WTCP(0x398cdd32, 0x72552c85), WTCP(0x36ba2014, 0x73b5ebd1), WTCP(0x33def287, 0x7504d345),
    WTCP(0x30fbc54d, 0x7641af3d), WTCP(0x2e110a62, 0x776c4edb), WTCP(0x2b1f34eb, 0x78848414),
    WTCP(0x2826b928, 0x798a23b1), WTCP(0x25280c5e, 0x7a7d055b), WTCP(0x2223a4c5, 0x7b5d039e),
    WTCP(0x1f19f97b, 0x7c29fbee), WTCP(0x1c0b826a, 0x7ce3ceb2), WTCP(0x18f8b83c, 0x7d8a5f40),
    WTCP(0x15e21445, 0x7e1d93ea), WTCP(0x12c8106f, 0x7e9d55fc), WTCP(0x0fab272b, 0x7f0991c4),
    WTCP(0x0c8bd35e, 0x7f62368f), WTCP(0x096a9049, 0x7fa736b4), WTCP(0x0647d97c, 0x7fd8878e),
    WTCP(0x03242abf, 0x7ff62182)};

const uint16_t sqrt_tab[49] = {0x5a82, 0x5d4b, 0x6000, 0x62a1, 0x6531, 0x67b1, 0x6a21, 0x6c84, 0x6ed9, 0x7123,
                               0x7360, 0x7593, 0x77bb, 0x79da, 0x7bef, 0x7dfb, 0x8000, 0x81fc, 0x83f0, 0x85dd,
                               0x87c3, 0x89a3, 0x8b7c, 0x8d4e, 0x8f1b, 0x90e2, 0x92a4, 0x9460, 0x9617, 0x97ca,
                               0x9977, 0x9b20, 0x9cc4, 0x9e64, 0xa000, 0xa197, 0xa32b, 0xa4ba, 0xa646, 0xa7cf,
                               0xa953, 0xaad5, 0xac53, 0xadcd, 0xaf45, 0xb0b9, 0xb22b, 0xb399, 0xb504};

LNK_SECTION_CONSTDATA_L1
const int32_t invCount[80] = /* This could be 16-bit wide */
    {0x00000000, 0x7fffffff, 0x40000000, 0x2aaaaaab, 0x20000000, 0x1999999a, 0x15555555, 0x12492492, 0x10000000,
     0x0e38e38e, 0x0ccccccd, 0x0ba2e8ba, 0x0aaaaaab, 0x09d89d8a, 0x09249249, 0x08888889, 0x08000000, 0x07878788,
     0x071c71c7, 0x06bca1af, 0x06666666, 0x06186186, 0x05d1745d, 0x0590b216, 0x05555555, 0x051eb852, 0x04ec4ec5,
     0x04bda12f, 0x04924925, 0x0469ee58, 0x04444444, 0x04210842, 0x04000000, 0x03e0f83e, 0x03c3c3c4, 0x03a83a84,
     0x038e38e4, 0x03759f23, 0x035e50d8, 0x03483483, 0x03333333, 0x031f3832, 0x030c30c3, 0x02fa0be8, 0x02e8ba2f,
     0x02d82d83, 0x02c8590b, 0x02b93105, 0x02aaaaab, 0x029cbc15, 0x028f5c29, 0x02828283, 0x02762762, 0x026a439f,
     0x025ed098, 0x0253c825, 0x02492492, 0x023ee090, 0x0234f72c, 0x022b63cc, 0x02222222, 0x02192e2a, 0x02108421,
     0x02082082, 0x02000000, 0x01f81f82, 0x01f07c1f, 0x01e9131b, 0x01e1e1e2, 0x01dae607, 0x01d41d42, 0x01cd8569,
     0x01c71c72, 0x01c0e070, 0x01bacf91, 0x01b4e81b, 0x01af286c, 0x01a98ef6, 0x01a41a42, 0x019ec8e9};

/*
 * Bitstream data lists
 */

/*
 * AOT {2,5,29}
 * epConfig = -1
 */

static const rbd_id_t el_aac_sce[] = {adtscrc_start_reg1, element_instance_tag, global_gain, ics_info, section_data,
                                      scale_factor_data, pulse, tns_data_present, tns_data, gain_control_data_present,
                                      /* gain_control_data, */
                                      spectral_data, adtscrc_end_reg1, end_of_sequence};

static const struct element_list node_aac_sce = {el_aac_sce, {NULL, NULL}};

/* CCE */
static const rbd_id_t el_aac_cce[] = {adtscrc_start_reg1, element_instance_tag, coupled_elements, /* CCE specific */
                                      global_gain, ics_info, section_data, scale_factor_data, pulse, tns_data_present,
                                      tns_data, gain_control_data_present,
                                      /* gain_control_data, */
                                      spectral_data, gain_element_lists, /* CCE specific */
                                      adtscrc_end_reg1, end_of_sequence};

static const struct element_list node_aac_cce = {el_aac_cce, {NULL, NULL}};

static const rbd_id_t el_aac_cpe[] = {adtscrc_start_reg1, element_instance_tag, common_window, link_sequence};

static const rbd_id_t el_aac_cpe0[] = {
    /*common_window = 0*/
    global_gain, ics_info, section_data, scale_factor_data, pulse, tns_data_present, tns_data,
    gain_control_data_present,
    /*gain_control_data,*/
    spectral_data, next_channel,

    adtscrc_start_reg2, global_gain, ics_info, section_data, scale_factor_data, pulse, tns_data_present, tns_data,
    gain_control_data_present,
    /*gain_control_data,*/
    spectral_data, adtscrc_end_reg1, adtscrc_end_reg2, end_of_sequence};

static const rbd_id_t el_aac_cpe1[] = {
    /* common_window = 1 */
    ics_info, ms,

    global_gain, section_data, scale_factor_data, pulse, tns_data_present, tns_data, gain_control_data_present,
    /*gain_control_data,*/
    spectral_data, next_channel,

    adtscrc_start_reg2, global_gain, section_data, scale_factor_data, pulse, tns_data_present, tns_data,
    gain_control_data_present,
    /*gain_control_data,*/
    spectral_data, adtscrc_end_reg1, adtscrc_end_reg2, end_of_sequence};

static const struct element_list node_aac_cpe0 = {el_aac_cpe0, {NULL, NULL}};

static const struct element_list node_aac_cpe1 = {el_aac_cpe1, {NULL, NULL}};

static const element_list_t node_aac_cpe = {el_aac_cpe, {&node_aac_cpe0, &node_aac_cpe1}};

/*
 * AOT C- {17,23}
 * epConfig = 0,1
 */
static const rbd_id_t el_aac_sce_epc0[] = {element_instance_tag,
                                           global_gain,
                                           ics_info,
                                           section_data,
                                           scale_factor_data,
                                           pulse,
                                           tns_data_present,
                                           gain_control_data_present,
                                           gain_control_data,
                                           esc1_hcr,  /*length_of_rvlc_escapes, length_of_rvlc_sf */
                                           esc2_rvlc, /* rvlc_cod_sf, rvlc_esc_sf */
                                           tns_data,
                                           spectral_data,
                                           end_of_sequence};

static const struct element_list node_aac_sce_epc0 = {el_aac_sce_epc0, {NULL, NULL}};

static const rbd_id_t el_aac_sce_epc1[] = {element_instance_tag, global_gain, ics_info, section_data, scale_factor_data,
                                           pulse, tns_data_present, gain_control_data_present,
                                           /*gain_control_data,*/
                                           esc1_hcr,  /*length_of_rvlc_escapes, length_of_rvlc_sf */
                                           esc2_rvlc, /* rvlc_cod_sf, rvlc_esc_sf */
                                           tns_data, spectral_data, end_of_sequence};

static const struct element_list node_aac_sce_epc1 = {el_aac_sce_epc1, {NULL, NULL}};

static const rbd_id_t el_aac_cpe_epc0[] = {element_instance_tag, common_window, link_sequence};

static const rbd_id_t el_aac_cpe0_epc0[] = {
    /* common_window = 0 */
    /* ESC 1: */
    global_gain, ics_info,
    /* ltp_data_present,
       ltp_data,
    */
    section_data, scale_factor_data, pulse, tns_data_present, gain_control_data_present,
    /*gain_control_data,*/
    esc1_hcr, /*length_of_rvlc_escapes, length_of_rvlc_sf */
    /* ESC 2: */
    esc2_rvlc, /* rvlc_cod_sf, rvlc_esc_sf */
    /* ESC 3: */
    tns_data,
    /* ESC 4: */
    spectral_data, next_channel,

    /* ESC 1: */
    global_gain, ics_info,
    /* ltp_data_present,
       ltp_data,
    */
    section_data, scale_factor_data, pulse, tns_data_present, gain_control_data_present,
    /*gain_control_data,*/
    esc1_hcr, /*length_of_rvlc_escapes, length_of_rvlc_sf */
    /* ESC 2: */
    esc2_rvlc, /* rvlc_cod_sf, rvlc_esc_sf */
    /* ESC 3: */
    tns_data,
    /* ESC 4: */
    spectral_data, end_of_sequence};

static const rbd_id_t el_aac_cpe1_epc0[] = {
    /* common_window = 1 */
    /* ESC 0: */
    ics_info,
    /* ltp_data_present,
       ltp_data,
       next_channel,
       ltp_data_present,
       ltp_data,
       next_channel,
    */
    ms,

    /* ESC 1: */
    global_gain, section_data, scale_factor_data, pulse, tns_data_present, gain_control_data_present,
    /*gain_control_data,*/
    esc1_hcr, /* length_of_reordered_spectral_data, length_of_longest_codeword
               */
    /* ESC 2: */
    esc2_rvlc, /* rvlc_cod_sf, rvlc_esc_sf */
    /* ESC 3: */
    tns_data,
    /* ESC 4: */
    spectral_data, next_channel,

    /* ESC 1: */
    global_gain, section_data, scale_factor_data, pulse, tns_data_present, gain_control_data_present,
    /*gain_control_data,*/
    esc1_hcr, /* length_of_reordered_spectral_data, length_of_longest_codeword
               */
    /* ESC 2: */
    esc2_rvlc, /* rvlc_cod_sf, rvlc_esc_sf */
    /* ESC 3: */
    tns_data,
    /* ESC 4: */
    spectral_data, end_of_sequence};

static const struct element_list node_aac_cpe0_epc0 = {el_aac_cpe0_epc0, {NULL, NULL}};

static const struct element_list node_aac_cpe1_epc0 = {el_aac_cpe1_epc0, {NULL, NULL}};

static const element_list_t node_aac_cpe_epc0 = {el_aac_cpe_epc0, {&node_aac_cpe0_epc0, &node_aac_cpe1_epc0}};

static const rbd_id_t el_aac_cpe0_epc1[] = {global_gain, ics_info, section_data, scale_factor_data, pulse,
                                            tns_data_present, gain_control_data_present,
                                            /*gain_control_data,*/
                                            next_channel, global_gain, ics_info, section_data, scale_factor_data, pulse,
                                            tns_data_present, gain_control_data_present,
                                            /*gain_control_data,*/
                                            next_channel, esc1_hcr,  /*length_of_rvlc_escapes, length_of_rvlc_sf */
                                            next_channel, esc1_hcr,  /*length_of_rvlc_escapes, length_of_rvlc_sf */
                                            next_channel, esc2_rvlc, /* rvlc_cod_sf, rvlc_esc_sf */
                                            next_channel, esc2_rvlc, /* rvlc_cod_sf, rvlc_esc_sf */
                                            next_channel, tns_data, next_channel, tns_data, next_channel, spectral_data,
                                            next_channel, spectral_data, end_of_sequence};

static const rbd_id_t el_aac_cpe1_epc1[] = {
    ics_info, ms, ltp_data_present,
    /* ltp_data, */
    global_gain, section_data, scale_factor_data, pulse, tns_data_present, gain_control_data_present,
    /*gain_control_data,*/
    next_channel,

    ltp_data_present,
    /* ltp_data, */
    global_gain, section_data, scale_factor_data, pulse, tns_data_present, gain_control_data_present,
    /*gain_control_data,*/
    next_channel, esc1_hcr,  /*length_of_rvlc_escapes, length_of_rvlc_sf */
    next_channel, esc1_hcr,  /*length_of_rvlc_escapes, length_of_rvlc_sf */
    next_channel, esc2_rvlc, /* rvlc_cod_sf, rvlc_esc_sf */
    next_channel, esc2_rvlc, /* rvlc_cod_sf, rvlc_esc_sf */

    next_channel, tns_data, next_channel, tns_data, next_channel, spectral_data, next_channel, spectral_data,
    end_of_sequence};

static const struct element_list node_aac_cpe0_epc1 = {el_aac_cpe0_epc1, {NULL, NULL}};

static const struct element_list node_aac_cpe1_epc1 = {el_aac_cpe1_epc1, {NULL, NULL}};

static const element_list_t node_aac_cpe_epc1 = {el_aac_cpe, {&node_aac_cpe0_epc1, &node_aac_cpe1_epc1}};

/*
 * AOT = 20
 * epConfig = 0
 */
static const rbd_id_t el_scal_sce_epc0[] = {ics_info, /* ESC 1 */
                                            tns_data_present, ltp_data_present,
                                            /* ltp_data, */
                                            global_gain, section_data, scale_factor_data, esc1_hcr,
                                            esc2_rvlc,     /* ESC 2 */
                                            tns_data,      /* ESC 3 */
                                            spectral_data, /* ESC 4 */
                                            end_of_sequence};

static const struct element_list node_scal_sce_epc0 = {el_scal_sce_epc0, {NULL, NULL}};

static const rbd_id_t el_scal_cpe_epc0[] = {
    ics_info,             /* ESC 0 */
    ms, tns_data_present, /* ESC 1 (ch 0) */
    ltp_data_present,
    /* ltp_data, */
    global_gain, section_data, scale_factor_data, esc1_hcr, esc2_rvlc,                   /* ESC 2 (ch 0) */
    tns_data,                                                                            /* ESC 3 (ch 0) */
    spectral_data,                                                                       /* ESC 4 (ch 0) */
    next_channel, tns_data_present,                                                      /* ESC 1 (ch 1) */
    ltp_data_present, global_gain, section_data, scale_factor_data, esc1_hcr, esc2_rvlc, /* ESC 2 (ch 1) */
    tns_data,                                                                            /* ESC 3 (ch 1) */
    spectral_data,                                                                       /* ESC 4 (ch 1) */
    end_of_sequence};

static const struct element_list node_scal_cpe_epc0 = {el_scal_cpe_epc0, {NULL, NULL}};

/*
 * AOT = 20
 * epConfig = 1
 */
static const rbd_id_t el_scal_sce_epc1[] = {ics_info, tns_data_present, ltp_data_present,
                                            /* ltp_data, */
                                            global_gain, section_data, scale_factor_data, esc1_hcr, tns_data,
                                            spectral_data, end_of_sequence};

static const struct element_list node_scal_sce_epc1 = {el_scal_sce_epc1, {NULL, NULL}};

static const rbd_id_t el_scal_cpe_epc1[] = {
    ics_info, ms, tns_data_present, ltp_data_present,
    /* ltp_data, */
    global_gain, section_data, scale_factor_data, esc1_hcr, next_channel, tns_data_present, ltp_data_present,
    /* ltp_data, */
    global_gain, section_data, scale_factor_data, esc1_hcr, next_channel, tns_data, next_channel, tns_data,
    next_channel, spectral_data, next_channel, spectral_data, end_of_sequence};

static const struct element_list node_scal_cpe_epc1 = {el_scal_cpe_epc1, {NULL, NULL}};

/*
 * Pseudo AOT for DRM/DRM+ (similar to AOT 20)
 */
static const rbd_id_t el_drm_sce[] = {drmcrc_start_reg, ics_info, tns_data_present, ltp_data_present,
                                      /* ltp_data, */
                                      global_gain, section_data, scale_factor_data, esc1_hcr, tns_data, drmcrc_end_reg,
                                      spectral_data, end_of_sequence};

static const struct element_list node_drm_sce = {el_drm_sce, {NULL, NULL}};

static const rbd_id_t el_drm_cpe[] = {
    drmcrc_start_reg, ics_info, ms, tns_data_present, ltp_data_present,
    /* ltp_data, */
    global_gain, section_data, scale_factor_data, esc1_hcr, next_channel, tns_data_present, ltp_data_present,
    /* ltp_data, */
    global_gain, section_data, scale_factor_data, esc1_hcr, next_channel, tns_data, next_channel, tns_data,
    drmcrc_end_reg, next_channel, spectral_data, next_channel, spectral_data, end_of_sequence};

static const struct element_list node_drm_cpe = {el_drm_cpe, {NULL, NULL}};

/*
 * AOT = 39
 * epConfig = 0
 */
static const rbd_id_t el_eld_sce_epc0[] = {global_gain, ics_info, section_data, scale_factor_data, tns_data_present,
                                           tns_data,    esc1_hcr, esc2_rvlc,    spectral_data,     end_of_sequence};

static const struct element_list node_eld_sce_epc0 = {el_eld_sce_epc0, {NULL, NULL}};

#define node_eld_sce_epc1 node_eld_sce_epc0

static const rbd_id_t el_eld_cpe_epc0[] = {
    ics_info,         ms,        global_gain,   section_data, scale_factor_data, tns_data_present, tns_data,
    esc1_hcr,         esc2_rvlc, spectral_data, next_channel, global_gain,       section_data,     scale_factor_data,
    tns_data_present, tns_data,  esc1_hcr,      esc2_rvlc,    spectral_data,     end_of_sequence};

static const rbd_id_t el_eld_cpe_epc1[] = {
    ics_info,      ms,           global_gain,  section_data,      scale_factor_data, tns_data_present,
    next_channel,  global_gain,  section_data, scale_factor_data, tns_data_present,  next_channel,
    tns_data,      next_channel, tns_data,     next_channel,      esc1_hcr,          esc2_rvlc,
    spectral_data, next_channel, esc1_hcr,     esc2_rvlc,         spectral_data,     end_of_sequence};

static const struct element_list node_eld_cpe_epc0 = {el_eld_cpe_epc0, {NULL, NULL}};

static const struct element_list node_eld_cpe_epc1 = {el_eld_cpe_epc1, {NULL, NULL}};

/*
 * AOT = 42
 * epConfig = 0
 */

static const rbd_id_t el_usac_coremode[] = {core_mode, next_channel, link_sequence};

static const rbd_id_t el_usac_sce0_epc0[] = {tns_data_present,
                                             /* fd_channel_stream */
                                             global_gain, noise, ics_info, tw_data, scale_factor_data_usac, tns_data,
                                             ac_spectral_data, fac_data, end_of_sequence};

static const rbd_id_t el_usac_lfe_epc0[] = {
    /* fd_channel_stream */
    global_gain, ics_info, scale_factor_data_usac, ac_spectral_data, fac_data, end_of_sequence};

static const rbd_id_t el_usac_lpd_epc0[] = {lpd_channel_stream, end_of_sequence};

static const struct element_list node_usac_sce0_epc0 = {el_usac_sce0_epc0, {NULL, NULL}};

static const struct element_list node_usac_sce1_epc0 = {el_usac_lpd_epc0, {NULL, NULL}};

static const struct element_list node_usac_sce_epc0 = {el_usac_coremode, {&node_usac_sce0_epc0, &node_usac_sce1_epc0}};

static const rbd_id_t list_usac_cpe00_epc0[] = {tns_active, common_window, link_sequence};

static const rbd_id_t el_usac_common_tw[] = {common_tw, link_sequence};

static const rbd_id_t list_usac_cpe0000_epc0[] = {
    /* core_mode0 = 0 */
    /* core_mode1 = 0 */
    /* common_window = 0 */
    /* common_tw = 0 */
    tns_data_present_usac,
    global_gain,
    noise,
    ics_info,
    tw_data,
    scale_factor_data_usac,
    tns_data,
    ac_spectral_data,
    fac_data,
    next_channel,
    global_gain,
    noise,
    ics_info,
    tw_data,
    scale_factor_data_usac,
    tns_data,
    ac_spectral_data,
    fac_data,
    end_of_sequence};

static const rbd_id_t list_usac_cpe0001_epc0[] = {
    /*
    core_mode0 = 0
    core_mode1 = 0
    common_window = 0
    common_tw = 1
    */
    tw_data,  tns_data_present_usac, global_gain, noise, ics_info, scale_factor_data_usac, tns_data, ac_spectral_data,
    fac_data, next_channel,          global_gain, noise, ics_info, scale_factor_data_usac, tns_data, ac_spectral_data,
    fac_data, end_of_sequence};

static const rbd_id_t list_usac_cpe001_epc0[] = {
    /* core_mode0 = 0 */
    /* core_mode1 = 0 */
    /* common_window = 1 */
    ics_info, common_max_sfb, ms, common_tw, link_sequence};

static const rbd_id_t list_usac_cpe0010_epc0[] = {
    /* core_mode0 = 0 */
    /* core_mode1 = 0 */
    /* common_window = 1 */
    /* common_tw = 0 */
    tns_data_present_usac, global_gain, noise, tw_data, scale_factor_data_usac, tns_data, ac_spectral_data, fac_data,
    next_channel,          global_gain, noise, tw_data, scale_factor_data_usac, tns_data, ac_spectral_data, fac_data,
    end_of_sequence};

static const rbd_id_t list_usac_cpe0011_epc0[] = {
    /* core_mode0 = 0 */
    /* core_mode1 = 0 */
    /* common_window = 1 */
    /* common_tw = 1 */
    tw_data,  tns_data_present_usac, global_gain, noise, scale_factor_data_usac, tns_data, ac_spectral_data,
    fac_data, next_channel,          global_gain, noise, scale_factor_data_usac, tns_data, ac_spectral_data,
    fac_data, end_of_sequence};

static const rbd_id_t list_usac_cpe10_epc0[] = {
    /* core_mode0 = 1 */
    /* core_mode1 = 0 */
    lpd_channel_stream,     next_channel, tns_data_present, global_gain, noise,          ics_info, tw_data,
    scale_factor_data_usac, tns_data,     ac_spectral_data, fac_data,    end_of_sequence};

static const rbd_id_t list_usac_cpe01_epc0[] = {
    /* core_mode0 = 0 */
    /* core_mode1 = 1 */
    tns_data_present, global_gain, noise,        ics_info,           tw_data,        scale_factor_data_usac, tns_data,
    ac_spectral_data, fac_data,    next_channel, lpd_channel_stream, end_of_sequence};

static const rbd_id_t list_usac_cpe11_epc0[] = {
    /* core_mode0 = 1 */
    /* core_mode1 = 1 */
    lpd_channel_stream, next_channel, lpd_channel_stream, end_of_sequence};

static const struct element_list node_usac_cpe0000_epc0 = {
    /* core_mode0 = 0 */
    /* core_mode1 = 0 */
    /* common_window = 0 */
    /* common_tw = 0 */
    list_usac_cpe0000_epc0,
    {NULL, NULL}};

static const struct element_list node_usac_cpe0010_epc0 = {
    /* core_mode0 = 0 */
    /* core_mode1 = 0 */
    /* common_window = 1 */
    /* common_tw = 0 */
    list_usac_cpe0010_epc0,
    {NULL, NULL}};

static const struct element_list node_usac_cpe0001_epc0 = {
    /* core_mode0 = 0 */
    /* core_mode1 = 0 */
    /* common_window = 0 */
    /* common_tw = 1 */
    list_usac_cpe0001_epc0,
    {NULL, NULL}};

static const struct element_list node_usac_cpe0011_epc0 = {
    /* core_mode0 = 0 */
    /* core_mode1 = 0 */
    /* common_window = 1 */
    /* common_tw = 1 */
    list_usac_cpe0011_epc0,
    {NULL, NULL}};

static const struct element_list node_usac_cpe000_epc0 = {
    /* core_mode0 = 0 */
    /* core_mode1 = 0 */
    /* common_window = 0 */
    el_usac_common_tw,
    {&node_usac_cpe0000_epc0, &node_usac_cpe0001_epc0}};

static const struct element_list node_usac_cpe001_epc0 = {list_usac_cpe001_epc0,
                                                          {&node_usac_cpe0010_epc0, &node_usac_cpe0011_epc0}};

static const struct element_list node_usac_cpe00_epc0 = {
    /* core_mode0 = 0 */
    /* core_mode1 = 0 */
    list_usac_cpe00_epc0,
    {&node_usac_cpe000_epc0, &node_usac_cpe001_epc0}};

static const struct element_list node_usac_cpe10_epc0 = {
    /* core_mode0 = 1 */
    /* core_mode1 = 0 */
    list_usac_cpe10_epc0,
    {NULL, NULL}};

static const struct element_list node_usac_cpe01_epc0 = {list_usac_cpe01_epc0, {NULL, NULL}};

static const struct element_list node_usac_cpe11_epc0 = {list_usac_cpe11_epc0, {NULL, NULL}};

static const struct element_list node_usac_cpe0_epc0 = {
    /* core_mode0 = 0 */
    el_usac_coremode,
    {&node_usac_cpe00_epc0, &node_usac_cpe01_epc0}};

static const struct element_list node_usac_cpe1_epc0 = {
    /* core_mode0 = 1 */
    el_usac_coremode,
    {&node_usac_cpe10_epc0, &node_usac_cpe11_epc0}};

static const struct element_list node_usac_cpe_epc0 = {el_usac_coremode, {&node_usac_cpe0_epc0, &node_usac_cpe1_epc0}};

static const struct element_list node_usac_lfe_epc0 = {el_usac_lfe_epc0, {NULL, NULL}};

const element_list_t *getBitstreamElementList(AUDIO_OBJECT_TYPE aot, int8_t epConfig, uint8_t nChannels, uint8_t layer,
                                              uint32_t elFlags) {
    switch(aot) {
        case AOT_AAC_LC:
        case AOT_SBR:
        case AOT_PS:
            assert(epConfig == -1);
            if(elFlags & AC_EL_GA_CCE) { return &node_aac_cce; }
            else {
                if(nChannels == 1) { return &node_aac_sce; }
                else { return &node_aac_cpe; }
            }
        case AOT_ER_AAC_LC:
        case AOT_ER_AAC_LD:
            if(nChannels == 1) {
                if(epConfig == 0) { return &node_aac_sce_epc0; }
                else { return &node_aac_sce_epc1; }
            }
            else {
                if(epConfig == 0) return &node_aac_cpe_epc0;
                else
                    return &node_aac_cpe_epc1;
            }
        case AOT_USAC:
            if(elFlags & AC_EL_USAC_LFE) {
                assert(nChannels == 1);
                return &node_usac_lfe_epc0;
            }
            if(nChannels == 1) { return &node_usac_sce_epc0; }
            else { return &node_usac_cpe_epc0; }
        case AOT_ER_AAC_SCAL:
            if(nChannels == 1) {
                if(epConfig <= 0) return &node_scal_sce_epc0;
                else
                    return &node_scal_sce_epc1;
            }
            else {
                if(epConfig <= 0) return &node_scal_cpe_epc0;
                else
                    return &node_scal_cpe_epc1;
            }
        case AOT_ER_AAC_ELD:
            if(nChannels == 1) {
                if(epConfig <= 0) return &node_eld_sce_epc0;
                else
                    return &node_eld_sce_epc1;
            }
            else {
                if(epConfig <= 0) return &node_eld_cpe_epc0;
                else
                    return &node_eld_cpe_epc1;
            }
        case AOT_DRM_AAC:
        case AOT_DRM_SBR:
        case AOT_DRM_MPEG_PS:
        case AOT_DRM_SURROUND:
            assert(epConfig == 1);
            if(nChannels == 1) { return &node_drm_sce; }
            else { return &node_drm_cpe; }
        default:
            break;
    }
    return NULL;
}

/* Inverse square root table for operands running from 0.5 to ~1.0 */
/* (int32_t) (0.5 + 1.0/sqrt((op)/FDKpow(2.0,31)));                    */
/* Note: First value is rnot rounded for accuracy reasons          */
/* Implicit exponent is 1.                                         */
/* Examples: 0x5A82799A = invSqrtNorm2 (0x4000.0000), exp=1        */
/*           0x5A82799A = invSqrtNorm2 (0x4000.0000), exp=1        */

LNK_SECTION_CONSTDATA_L1
const int32_t invSqrtTab[SQRT_VALUES] = {
    0x5A827999, 0x5A287E03, 0x59CF8CBC, 0x5977A0AC, 0x5920B4DF, 0x58CAC480, 0x5875CADE, 0x5821C364, 0x57CEA99D,
    0x577C7930, 0x572B2DE0, 0x56DAC38E, 0x568B3632, 0x563C81E0, 0x55EEA2C4, 0x55A19522, 0x55555555, 0x5509DFD0,
    0x54BF311A, 0x547545D0, 0x542C1AA4, 0x53E3AC5B, 0x539BF7CD, 0x5354F9E7, 0x530EAFA5, 0x52C91618, 0x52842A5F,
    0x523FE9AC, 0x51FC5140, 0x51B95E6B, 0x51770E8F, 0x51355F1A, 0x50F44D89, 0x50B3D768, 0x5073FA50, 0x5034B3E7,
    0x4FF601E0, 0x4FB7E1FA, 0x4F7A5202, 0x4F3D4FCF, 0x4F00D944, 0x4EC4EC4F, 0x4E8986EA, 0x4E4EA718, 0x4E144AE9,
    0x4DDA7073, 0x4DA115DA, 0x4D683948, 0x4D2FD8F4, 0x4CF7F31B, 0x4CC08605, 0x4C899000, 0x4C530F65, 0x4C1D0294,
    0x4BE767F5, 0x4BB23DF9, 0x4B7D8317, 0x4B4935CF, 0x4B1554A6, 0x4AE1DE2A, 0x4AAED0F0, 0x4A7C2B93, 0x4A49ECB3,
    0x4A1812FA, 0x49E69D16, 0x49B589BB, 0x4984D7A4, 0x49548592, 0x49249249, 0x48F4FC97, 0x48C5C34B, 0x4896E53D,
    0x48686148, 0x483A364D, 0x480C6332, 0x47DEE6E1, 0x47B1C049, 0x4784EE60, 0x4758701C, 0x472C447C, 0x47006A81,
    0x46D4E130, 0x46A9A794, 0x467EBCBA, 0x46541FB4, 0x4629CF98, 0x45FFCB80, 0x45D6128A, 0x45ACA3D5, 0x45837E88,
    0x455AA1CB, 0x45320CC8, 0x4509BEB0, 0x44E1B6B4, 0x44B9F40B, 0x449275ED, 0x446B3B96, 0x44444444, 0x441D8F3B,
    0x43F71BBF, 0x43D0E917, 0x43AAF68F, 0x43854374, 0x435FCF15, 0x433A98C6, 0x43159FDC, 0x42F0E3AE, 0x42CC6398,
    0x42A81EF6, 0x42841527, 0x4260458E, 0x423CAF8D, 0x4219528B, 0x41F62DF2, 0x41D3412A, 0x41B08BA2, 0x418E0CC8,
    0x416BC40D, 0x4149B0E5, 0x4127D2C3, 0x41062920, 0x40E4B374, 0x40C3713B, 0x40A261EF, 0x40818512, 0x4060DA22,
    0x404060A1, 0x40201814, 0x40000000, 0x3FE017EC /* , 0x3FC05F61 */
};

/* number of channels of the formats */

const int32_t format_nchan[FDK_NFORMATS + 9 - 2] = {
    0,  /* any set-up, ChConfIdx = 0 */
    1,  /* mono ChConfIdx = 1 */
    2,  /* stereo ChConfIdx = 2 */
    3,  /* 3/0.0 ChConfIdx = 3 */
    4,  /* 3/1.0 ChConfIdx = 4 */
    5,  /* 3/2.0 ChConfIdx = 5 */
    6,  /* 5.1 ChConfIdx = 6 */
    8,  /* 5/2.1 ALT ChConfIdx = 7 */
    0,  /* Empty n.a. ChConfIdx = 8 */
    3,  /* 2/1.0 ChConfIdx = 9 */
    4,  /* 2/2.0 ChConfIdx = 10 */
    7,  /* 3/3.1 ChConfIdx = 11 */
    8,  /* 3/4.1 ChConfIdx = 12 */
    24, /* 22.2 ChConfIdx = 13 */
    8,  /* 5/2.1 ChConfIdx = 14 */
    12, /* 5/5.2 ChConfIdx = 15 */
    10, /* 5/4.1 ChConfIdx = 16 */
    12, /* 6/5.1 ChConfIdx = 17 */
    14, /* 6/7.1 ChConfIdx = 18 */
    12, /* 5/6.1 ChConfIdx = 19 */
    14  /* 7/6.1 ChConfIdx = 20 */
};
