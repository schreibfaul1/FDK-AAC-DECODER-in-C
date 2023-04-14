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
    31651, 28378, 23170, 16384, 8481, 0, -8481, -16384, -23170, -28378, -31651,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag24[] = {
    8481, 16384, 23170, 28378, 31651, 32767, 31651, 28378, 23170, 16384, 8481,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal48[] = {
    32488,  31651,  30274, 31651,  28378,  23170, 30274,  23170,  12540, 28378,  16384,
    0,      25997,  8481,  -12540, 23170,  0,     -23170, 19948,  -8481, -30274, 16384,
    -16384, -32768, 12540, -23170, -30274, 8481,  -28378, -23170, 4277,  -31651, -12540,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag48[] = {
    4277,  8481,  12540, 8481,  16384,  23170, 12540, 23170,  30274, 16384, 28378,
    32767, 19948, 31651, 30274, 23170,  32767, 23170, 25997,  31651, 12540, 28378,
    28378, 0,     30274, 23170, -12540, 31651, 16384, -23170, 32488, 8481,  -30274,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal80[] = {
    32667,  32365,  31863,  31164, 32365,  31164,  29197,  26510, 31863,  29197,  24917,  19261, 31164,  26510,  19261,
    10126,  30274,  23170,  12540, 0,      29197,  19261,  5126,  -10126, 27939,  14876,  -2571, -19261, 26510,  10126,
    -10126, -26510, 24917,  5126,  -17121, -31164, 23170,  0,     -23170, -32768, 21281,  -5126, -27939, -31164, 19261,
    -10126, -31164, -26510, 17121, -14876, -32667, -19261, 14876, -19261, -32365, -10126, 12540, -23170, -30274, 0,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag80[] = {
    2571,  5126,  7650,   10126, 5126,  10126, 14876,  19261, 7650,  14876, 21281,  26510, 10126, 19261,  26510,
    31164, 12540, 23170,  30274, 32767, 14876, 26510,  32365, 31164, 17121, 29197,  32667, 26510, 19261,  31164,
    31164, 19261, 21281,  32365, 27939, 10126, 23170,  32767, 23170, 0,     24917,  32365, 17121, -10126, 26510,
    31164, 10126, -19261, 27939, 29197, 2571,  -26510, 29197, 26510, -5126, -31164, 30274, 23170, -12540, -32768,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal96[] = {
    32698,  32488,  32488,  31651,  32138,  30274,  31651,  28378,  31029,  25997,  30274,  23170,  29389,
    19948,  28378,  16384,  27246,  12540,  25997,  8481,   24636,  4277,   23170,  0,      21605,  -4277,
    19948,  -8481,  18205,  -12540, 16384,  -16384, 14493,  -19948, 12540,  -23170, 10533,  -25997, 8481,
    -28378, 6393,   -30274, 4277,   -31651, 2143,   -32488, 0,      -32768, -2143,  -32488, -4277,  -31651,
    -6393,  -30274, -8481,  -28378, -10533, -25997, -12540, -23170, -14493, -19948,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag96[] = {
    2143,  4277,  4277,  8481,  6393,  12540,  8481,  16384,  10533, 19948,  12540, 23170,  14493, 25997,  16384, 28378,
    18205, 30274, 19948, 31651, 21605, 32488,  23170, 32767,  24636, 32488,  25997, 31651,  27246, 30274,  28378, 28378,
    29389, 25997, 30274, 23170, 31029, 19948,  31651, 16384,  32138, 12540,  32488, 8481,   32698, 4277,   32767, 0,
    32698, -4277, 32488, -8481, 32138, -12540, 31651, -16384, 31029, -19948, 30274, -23170, 29389, -25997,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal384[] = {
    32764,  32750,  32729,  32698,  32658,  32610,  32553,  32488,  32413,  32330,  32239,  32750,  32698,  32610,
    32488,  32330,  32138,  31912,  31651,  31357,  31029,  30668,  32729,  32610,  32413,  32138,  31786,  31357,
    30853,  30274,  29622,  28899,  28106,  32698,  32488,  32138,  31651,  31029,  30274,  29389,  28378,  27246,
    25997,  24636,  32658,  32330,  31786,  31029,  30064,  28899,  27540,  25997,  24279,  22400,  20371,  32610,
    32138,  31357,  30274,  28899,  27246,  25330,  23170,  20788,  18205,  15447,  32553,  31912,  30853,  29389,
    27540,  25330,  22788,  19948,  16846,  13524,  10024,  32488,  31651,  30274,  28378,  25997,  23170,  19948,
    16384,  12540,  8481,   4277,   32413,  31357,  29622,  27246,  24279,  20788,  16846,  12540,  7962,   3212,
    -1608,  32330,  31029,  28899,  25997,  22400,  18205,  13524,  8481,   3212,   -2143,  -7441,  32239,  30668,
    28106,  24636,  20371,  15447,  10024,  4277,   -1608,  -7441,  -13033, 32138,  30274,  27246,  23170,  18205,
    12540,  6393,   0,      -6393,  -12540, -18205, 32029,  29847,  26320,  21605,  15917,  9512,   2678,   -4277,
    -11039, -17304, -22788, 31912,  29389,  25330,  19948,  13524,  6393,   -1072,  -8481,  -15447, -21605, -26635,
    31786,  28899,  24279,  18205,  11039,  3212,   -4808,  -12540, -19520, -25330, -29622, 31651,  28378,  23170,
    16384,  8481,   0,      -8481,  -16384, -23170, -28378, -31651, 31508,  27827,  22006,  14493,  5866,   -3212,
    -12043, -19948, -26320, -30668, -32658, 31357,  27246,  20788,  12540,  3212,   -6393,  -15447, -23170, -28899,
    -32138, -32610, 31197,  26635,  19520,  10533,  536,    -9512,  -18648, -25997, -30853, -32750, -31508, 31029,
    25997,  18205,  8481,   -2143,  -12540, -21605, -28378, -32138, -32488, -29389, 30853,  25330,  16846,  6393,
    -4808,  -15447, -24279, -30274, -32729, -31357, -26320, 30668,  24636,  15447,  4277,   -7441,  -18205, -26635,
    -31651, -32610, -29389, -22400, 30475,  23916,  14010,  2143,   -10024, -20788, -28642, -32488, -31786, -26635,
    -17757, 30274,  23170,  12540,  0,      -12540, -23170, -30274, -32768, -30274, -23170, -12540, 30064,  22400,
    11039,  -2143,  -14972, -25330, -31508, -32488, -28106, -19087, -6918,  29847,  21605,  9512,   -4277,  -17304,
    -27246, -32330, -31651, -25330, -14493, -1072,  29622,  20788,  7962,   -6393,  -19520, -28899, -32729, -30274,
    -22006, -9512,  4808,   29389,  19948,  6393,   -8481,  -21605, -30274, -32698, -28378, -18205, -4277,  10533,
    29148,  19087,  4808,   -10533, -23546, -31357, -32239, -25997, -14010, 1072,   15917,  28899,  18205,  3212,
    -12540, -25330, -32138, -31357, -23170, -9512,  6393,   20788,  28642,  17304,  1608,   -14493, -26944, -32610,
    -30064, -19948, -4808,  11543,  24986,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag384[] = {
    536,    1072,   1608,   2143,   2678,   3212,   3745,   4277,   4808,   5338,   5866,   1072,   2143,   3212,
    4277,   5338,   6393,   7441,   8481,   9512,   10533,  11543,  1608,   3212,   4808,   6393,   7962,   9512,
    11039,  12540,  14010,  15447,  16846,  2143,   4277,   6393,   8481,   10533,  12540,  14493,  16384,  18205,
    19948,  21605,  2678,   5338,   7962,   10533,  13033,  15447,  17757,  19948,  22006,  23916,  25667,  3212,
    6393,   9512,   12540,  15447,  18205,  20788,  23170,  25330,  27246,  28899,  3745,   7441,   11039,  14493,
    17757,  20788,  23546,  25997,  28106,  29847,  31197,  4277,   8481,   12540,  16384,  19948,  23170,  25997,
    28378,  30274,  31651,  32488,  4808,   9512,   14010,  18205,  22006,  25330,  28106,  30274,  31786,  32610,
    32729,  5338,   10533,  15447,  19948,  23916,  27246,  29847,  31651,  32610,  32698,  31912,  5866,   11543,
    16846,  21605,  25667,  28899,  31197,  32488,  32729,  31912,  30064,  6393,   12540,  18205,  23170,  27246,
    30274,  32138,  32767,  32138,  30274,  27246,  6918,   13524,  19520,  24636,  28642,  31357,  32658,  32488,
    30853,  27827,  23546,  7441,   14493,  20788,  25997,  29847,  32138,  32750,  31651,  28899,  24636,  19087,
    7962,   15447,  22006,  27246,  30853,  32610,  32413,  30274,  26320,  20788,  14010,  8481,   16384,  23170,
    28378,  31651,  32767,  31651,  28378,  23170,  16384,  8481,   8998,   17304,  24279,  29389,  32239,  32610,
    30475,  25997,  19520,  11543,  2678,   9512,   18205,  25330,  30274,  32610,  32138,  28899,  23170,  15447,
    6393,   -3212,  10024,  19087,  26320,  31029,  32764,  31357,  26944,  19948,  11039,  1072,   -8998,  10533,
    19948,  27246,  31651,  32698,  30274,  24636,  16384,  6393,   -4277,  -14493, 11039,  20788,  28106,  32138,
    32413,  28899,  22006,  12540,  1608,   -9512,  -19520, 11543,  21605,  28899,  32488,  31912,  27246,  19087,
    8481,   -3212,  -14493, -23916, 12043,  22400,  29622,  32698,  31197,  25330,  15917,  4277,   -7962,  -19087,
    -27540, 12540,  23170,  30274,  32767,  30274,  23170,  12540,  0,      -12540, -23170, -30274, 13033,  23916,
    30853,  32698,  29148,  20788,  8998,   -4277,  -16846, -26635, -32029, 13524,  24636,  31357,  32488,  27827,
    18205,  5338,   -8481,  -20788, -29389, -32750, 14010,  25330,  31786,  32138,  26320,  15447,  1608,   -12540,
    -24279, -31357, -32413, 14493,  25997,  32138,  31651,  24636,  12540,  -2143,  -16384, -27246, -32488, -31029,
    14972,  26635,  32413,  31029,  22788,  9512,   -5866,  -19948, -29622, -32750, -28642, 15447,  27246,  32610,
    30274,  20788,  6393,   -9512,  -23170, -31357, -32138, -25330, 15917,  27827,  32729,  29389,  18648,  3212,
    -13033, -25997, -32413, -30668, -21199,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal60[] = {
    32588,  32052,  31164, 32052,  29935,  26510, 31164,  26510,  19261, 29935,  21926,  10126,  28378,  16384,
    0,      26510,  10126, -10126, 24351,  3425,  -19261, 21926,  -3425, -26510, 19261,  -10126, -31164, 16384,
    -16384, -32768, 13328, -21926, -31164, 10126, -26510, -26510, 6813,  -29935, -19261, 3425,   -32052, -10126,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag60[] = {
    3425,  6813,  10126, 6813,  13328,  19261, 10126, 19261,  26510, 13328, 24351,  31164, 16384, 28378,
    32767, 19261, 31164, 31164, 21926,  32588, 26510, 24351,  32588, 19261, 26510,  31164, 10126, 28378,
    28378, 0,     29935, 24351, -10126, 31164, 19261, -19261, 32052, 13328, -26510, 32588, 6813,  -31164,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal120[] = {
    32723, 32588, 32365,  32052,  31651,  31164,  30592,  32588, 32052, 31164,  29935,  28378,  26510,  24351,
    32365, 31164, 29197,  26510,  23170,  19261,  14876,  32052, 29935, 26510,  21926,  16384,  10126,  3425,
    31651, 28378, 23170,  16384,  8481,   0,      -8481,  31164, 26510, 19261,  10126,  0,      -10126, -19261,
    30592, 24351, 14876,  3425,   -8481,  -19261, -27482, 29935, 21926, 10126,  -3425,  -16384, -26510, -32052,
    29197, 19261, 5126,   -10126, -23170, -31164, -32365, 28378, 16384, 0,      -16384, -28378, -32768, -28378,
    27482, 13328, -5126,  -21926, -31651, -31164, -20622, 26510, 10126, -10126, -26510, -32768, -26510, -10126,
    25466, 6813,  -14876, -29935, -31651, -19261, 1715,   24351, 3425,  -19261, -32052, -28378, -10126, 13328,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag120[] = {
    1715,  3425,  5126,  6813,  8481,  10126,  11743,  3425,  6813,  10126, 13328, 16384,  19261,  21926,
    5126,  10126, 14876, 19261, 23170, 26510,  29197,  6813,  13328, 19261, 24351, 28378,  31164,  32588,
    8481,  16384, 23170, 28378, 31651, 32767,  31651,  10126, 19261, 26510, 31164, 32767,  31164,  26510,
    11743, 21926, 29197, 32588, 31651, 26510,  17847,  13328, 24351, 31164, 32588, 28378,  19261,  6813,
    14876, 26510, 32365, 31164, 23170, 10126,  -5126,  16384, 28378, 32767, 28378, 16384,  0,      -16384,
    17847, 29935, 32365, 24351, 8481,  -10126, -25466, 19261, 31164, 31164, 19261, 0,      -19261, -31164,
    20622, 32052, 29197, 13328, -8481, -26510, -32723, 21926, 32588, 26510, 6813,  -16384, -31164, -29935,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal192[] = {
    32750, 32698, 32610, 32488, 32330, 32138,  31912,  31651,  31357,  31029,  30668,  30274,  29847,  29389,  28899,
    32698, 32488, 32138, 31651, 31029, 30274,  29389,  28378,  27246,  25997,  24636,  23170,  21605,  19948,  18205,
    32610, 32138, 31357, 30274, 28899, 27246,  25330,  23170,  20788,  18205,  15447,  12540,  9512,   6393,   3212,
    32488, 31651, 30274, 28378, 25997, 23170,  19948,  16384,  12540,  8481,   4277,   0,      -4277,  -8481,  -12540,
    32330, 31029, 28899, 25997, 22400, 18205,  13524,  8481,   3212,   -2143,  -7441,  -12540, -17304, -21605, -25330,
    32138, 30274, 27246, 23170, 18205, 12540,  6393,   0,      -6393,  -12540, -18205, -23170, -27246, -30274, -32138,
    31912, 29389, 25330, 19948, 13524, 6393,   -1072,  -8481,  -15447, -21605, -26635, -30274, -32330, -32698, -31357,
    31651, 28378, 23170, 16384, 8481,  0,      -8481,  -16384, -23170, -28378, -31651, -32768, -31651, -28378, -23170,
    31357, 27246, 20788, 12540, 3212,  -6393,  -15447, -23170, -28899, -32138, -32610, -30274, -25330, -18205, -9512,
    31029, 25997, 18205, 8481,  -2143, -12540, -21605, -28378, -32138, -32488, -29389, -23170, -14493, -4277,  6393,
    30668, 24636, 15447, 4277,  -7441, -18205, -26635, -31651, -32610, -29389, -22400, -12540, -1072,  10533,  20788,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag192[] = {
    1072,  2143,  3212,  4277,  5338,  6393,  7441,  8481,  9512,  10533,  11543,  12540,  13524,  14493,  15447,
    2143,  4277,  6393,  8481,  10533, 12540, 14493, 16384, 18205, 19948,  21605,  23170,  24636,  25997,  27246,
    3212,  6393,  9512,  12540, 15447, 18205, 20788, 23170, 25330, 27246,  28899,  30274,  31357,  32138,  32610,
    4277,  8481,  12540, 16384, 19948, 23170, 25997, 28378, 30274, 31651,  32488,  32767,  32488,  31651,  30274,
    5338,  10533, 15447, 19948, 23916, 27246, 29847, 31651, 32610, 32698,  31912,  30274,  27827,  24636,  20788,
    6393,  12540, 18205, 23170, 27246, 30274, 32138, 32767, 32138, 30274,  27246,  23170,  18205,  12540,  6393,
    7441,  14493, 20788, 25997, 29847, 32138, 32750, 31651, 28899, 24636,  19087,  12540,  5338,   -2143,  -9512,
    8481,  16384, 23170, 28378, 31651, 32767, 31651, 28378, 23170, 16384,  8481,   0,      -8481,  -16384, -23170,
    9512,  18205, 25330, 30274, 32610, 32138, 28899, 23170, 15447, 6393,   -3212,  -12540, -20788, -27246, -31357,
    10533, 19948, 27246, 31651, 32698, 30274, 24636, 16384, 6393,  -4277,  -14493, -23170, -29389, -32488, -32138,
    11543, 21605, 28899, 32488, 31912, 27246, 19087, 8481,  -3212, -14493, -23916, -30274, -32750, -31029, -25330,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal240[] = {
    32757, 32723, 32667, 32588, 32488, 32365,  32219,  32052,  31863,  31651,  31419,  31164,  30888,  30592,  30274,
    32723, 32588, 32365, 32052, 31651, 31164,  30592,  29935,  29197,  28378,  27482,  26510,  25466,  24351,  23170,
    32667, 32365, 31863, 31164, 30274, 29197,  27939,  26510,  24917,  23170,  21281,  19261,  17121,  14876,  12540,
    32588, 32052, 31164, 29935, 28378, 26510,  24351,  21926,  19261,  16384,  13328,  10126,  6813,   3425,   0,
    32488, 31651, 30274, 28378, 25997, 23170,  19948,  16384,  12540,  8481,   4277,   0,      -4277,  -8481,  -12540,
    32365, 31164, 29197, 26510, 23170, 19261,  14876,  10126,  5126,   0,      -5126,  -10126, -14876, -19261, -23170,
    32219, 30592, 27939, 24351, 19948, 14876,  9307,   3425,   -2571,  -8481,  -14107, -19261, -23769, -27482, -30274,
    32052, 29935, 26510, 21926, 16384, 10126,  3425,   -3425,  -10126, -16384, -21926, -26510, -29935, -32052, -32768,
    31863, 29197, 24917, 19261, 12540, 5126,   -2571,  -10126, -17121, -23170, -27939, -31164, -32667, -32365, -30274,
    31651, 28378, 23170, 16384, 8481,  0,      -8481,  -16384, -23170, -28378, -31651, -32768, -31651, -28378, -23170,
    31419, 27482, 21281, 13328, 4277,  -5126,  -14107, -21926, -27939, -31651, -32757, -31164, -27005, -20622, -12540,
    31164, 26510, 19261, 10126, 0,     -10126, -19261, -26510, -31164, -32768, -31164, -26510, -19261, -10126, 0,
    30888, 25466, 17121, 6813,  -4277, -14876, -23769, -29935, -32667, -31651, -27005, -19261, -9307,  1715,   12540,
    30592, 24351, 14876, 3425,  -8481, -19261, -27482, -32052, -32365, -28378, -20622, -10126, 1715,   13328,  23170,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag240[] = {
    858,   1715,  2571,  3425,  4277,  5126,  5971,  6813,  7650,  8481,   9307,   10126,  10938,  11743,  12540,
    1715,  3425,  5126,  6813,  8481,  10126, 11743, 13328, 14876, 16384,  17847,  19261,  20622,  21926,  23170,
    2571,  5126,  7650,  10126, 12540, 14876, 17121, 19261, 21281, 23170,  24917,  26510,  27939,  29197,  30274,
    3425,  6813,  10126, 13328, 16384, 19261, 21926, 24351, 26510, 28378,  29935,  31164,  32052,  32588,  32767,
    4277,  8481,  12540, 16384, 19948, 23170, 25997, 28378, 30274, 31651,  32488,  32767,  32488,  31651,  30274,
    5126,  10126, 14876, 19261, 23170, 26510, 29197, 31164, 32365, 32767,  32365,  31164,  29197,  26510,  23170,
    5971,  11743, 17121, 21926, 25997, 29197, 31419, 32588, 32667, 31651,  29576,  26510,  22556,  17847,  12540,
    6813,  13328, 19261, 24351, 28378, 31164, 32588, 32588, 31164, 28378,  24351,  19261,  13328,  6813,   0,
    7650,  14876, 21281, 26510, 30274, 32365, 32667, 31164, 27939, 23170,  17121,  10126,  2571,   -5126,  -12540,
    8481,  16384, 23170, 28378, 31651, 32767, 31651, 28378, 23170, 16384,  8481,   0,      -8481,  -16384, -23170,
    9307,  17847, 24917, 29935, 32488, 32365, 29576, 24351, 17121, 8481,   -858,   -10126, -18560, -25466, -30274,
    10126, 19261, 26510, 31164, 32767, 31164, 26510, 19261, 10126, 0,      -10126, -19261, -26510, -31164, -32768,
    10938, 20622, 27939, 32052, 32488, 29197, 22556, 13328, 2571,  -8481,  -18560, -26510, -31419, -32723, -30274,
    11743, 21926, 29197, 32588, 31651, 26510, 17847, 6813,  -5126, -16384, -25466, -31164, -32723, -29935, -23170,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal480[] = {
    32765,  32757,  32743,  32723,  32698,  32667,  32631,  32588,  32541,  32488,  32429,  32365,  32295,  32219,
    32138,  32052,  31960,  31863,  31760,  31651,  31538,  31419,  31294,  31164,  31029,  30888,  30743,  30592,
    30435,  30274,  30107,  32757,  32723,  32667,  32588,  32488,  32365,  32219,  32052,  31863,  31651,  31419,
    31164,  30888,  30592,  30274,  29935,  29576,  29197,  28797,  28378,  27939,  27482,  27005,  26510,  25997,
    25466,  24917,  24351,  23769,  23170,  22556,  32743,  32667,  32541,  32365,  32138,  31863,  31538,  31164,
    30743,  30274,  29758,  29197,  28590,  27939,  27246,  26510,  25733,  24917,  24062,  23170,  22243,  21281,
    20286,  19261,  18205,  17121,  16011,  14876,  13719,  12540,  11342,  32723,  32588,  32365,  32052,  31651,
    31164,  30592,  29935,  29197,  28378,  27482,  26510,  25466,  24351,  23170,  21926,  20622,  19261,  17847,
    16384,  14876,  13328,  11743,  10126,  8481,   6813,   5126,   3425,   1715,   0,      -1715,  32698,  32488,
    32138,  31651,  31029,  30274,  29389,  28378,  27246,  25997,  24636,  23170,  21605,  19948,  18205,  16384,
    14493,  12540,  10533,  8481,   6393,   4277,   2143,   0,      -2143,  -4277,  -6393,  -8481,  -10533, -12540,
    -14493, 32667,  32365,  31863,  31164,  30274,  29197,  27939,  26510,  24917,  23170,  21281,  19261,  17121,
    14876,  12540,  10126,  7650,   5126,   2571,   0,      -2571,  -5126,  -7650,  -10126, -12540, -14876, -17121,
    -19261, -21281, -23170, -24917, 32631,  32219,  31538,  30592,  29389,  27939,  26255,  24351,  22243,  19948,
    17485,  14876,  12142,  9307,   6393,   3425,   429,    -2571,  -5549,  -8481,  -11342, -14107, -16754, -19261,
    -21605, -23769, -25733, -27482, -28999, -30274, -31294, 32588,  32052,  31164,  29935,  28378,  26510,  24351,
    21926,  19261,  16384,  13328,  10126,  6813,   3425,   0,      -3425,  -6813,  -10126, -13328, -16384, -19261,
    -21926, -24351, -26510, -28378, -29935, -31164, -32052, -32588, -32768, -32588, 32541,  31863,  30743,  29197,
    27246,  24917,  22243,  19261,  16011,  12540,  8895,   5126,   1286,   -2571,  -6393,  -10126, -13719, -17121,
    -20286, -23170, -25733, -27939, -29758, -31164, -32138, -32667, -32743, -32365, -31538, -30274, -28590, 32488,
    31651,  30274,  28378,  25997,  23170,  19948,  16384,  12540,  8481,   4277,   0,      -4277,  -8481,  -12540,
    -16384, -19948, -23170, -25997, -28378, -30274, -31651, -32488, -32768, -32488, -31651, -30274, -28378, -25997,
    -23170, -19948, 32429,  31419,  29758,  27482,  24636,  21281,  17485,  13328,  8895,   4277,   -429,   -5126,
    -9717,  -14107, -18205, -21926, -25193, -27939, -30107, -31651, -32541, -32757, -32295, -31164, -29389, -27005,
    -24062, -20622, -16754, -12540, -8066,  32365,  31164,  29197,  26510,  23170,  19261,  14876,  10126,  5126,
    0,      -5126,  -10126, -14876, -19261, -23170, -26510, -29197, -31164, -32365, -32768, -32365, -31164, -29197,
    -26510, -23170, -19261, -14876, -10126, -5126,  0,      5126,   32295,  30888,  28590,  25466,  21605,  17121,
    12142,  6813,   1286,   -4277,  -9717,  -14876, -19606, -23769, -27246, -29935, -31760, -32667, -32631, -31651,
    -29758, -27005, -23472, -19261, -14493, -9307,  -3851,  1715,   7232,   12540,  17485,  32219,  30592,  27939,
    24351,  19948,  14876,  9307,   3425,   -2571,  -8481,  -14107, -19261, -23769, -27482, -30274, -32052, -32757,
    -32365, -30888, -28378, -24917, -20622, -15636, -10126, -4277,  1715,   7650,   13328,  18560,  23170,  27005,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag480[] = {
    429,    858,    1286,   1715,   2143,   2571,   2998,   3425,   3851,   4277,   4702,   5126,   5549,   5971,
    6393,   6813,   7232,   7650,   8066,   8481,   8895,   9307,   9717,   10126,  10533,  10938,  11342,  11743,
    12142,  12540,  12935,  858,    1715,   2571,   3425,   4277,   5126,   5971,   6813,   7650,   8481,   9307,
    10126,  10938,  11743,  12540,  13328,  14107,  14876,  15636,  16384,  17121,  17847,  18560,  19261,  19948,
    20622,  21281,  21926,  22556,  23170,  23769,  1286,   2571,   3851,   5126,   6393,   7650,   8895,   10126,
    11342,  12540,  13719,  14876,  16011,  17121,  18205,  19261,  20286,  21281,  22243,  23170,  24062,  24917,
    25733,  26510,  27246,  27939,  28590,  29197,  29758,  30274,  30743,  1715,   3425,   5126,   6813,   8481,
    10126,  11743,  13328,  14876,  16384,  17847,  19261,  20622,  21926,  23170,  24351,  25466,  26510,  27482,
    28378,  29197,  29935,  30592,  31164,  31651,  32052,  32365,  32588,  32723,  32767,  32723,  2143,   4277,
    6393,   8481,   10533,  12540,  14493,  16384,  18205,  19948,  21605,  23170,  24636,  25997,  27246,  28378,
    29389,  30274,  31029,  31651,  32138,  32488,  32698,  32767,  32698,  32488,  32138,  31651,  31029,  30274,
    29389,  2571,   5126,   7650,   10126,  12540,  14876,  17121,  19261,  21281,  23170,  24917,  26510,  27939,
    29197,  30274,  31164,  31863,  32365,  32667,  32767,  32667,  32365,  31863,  31164,  30274,  29197,  27939,
    26510,  24917,  23170,  21281,  2998,   5971,   8895,   11743,  14493,  17121,  19606,  21926,  24062,  25997,
    27713,  29197,  30435,  31419,  32138,  32588,  32765,  32667,  32295,  31651,  30743,  29576,  28161,  26510,
    24636,  22556,  20286,  17847,  15257,  12540,  9717,   3425,   6813,   10126,  13328,  16384,  19261,  21926,
    24351,  26510,  28378,  29935,  31164,  32052,  32588,  32767,  32588,  32052,  31164,  29935,  28378,  26510,
    24351,  21926,  19261,  16384,  13328,  10126,  6813,   3425,   0,      -3425,  3851,   7650,   11342,  14876,
    18205,  21281,  24062,  26510,  28590,  30274,  31538,  32365,  32743,  32667,  32138,  31164,  29758,  27939,
    25733,  23170,  20286,  17121,  13719,  10126,  6393,   2571,   -1286,  -5126,  -8895,  -12540, -16011, 4277,
    8481,   12540,  16384,  19948,  23170,  25997,  28378,  30274,  31651,  32488,  32767,  32488,  31651,  30274,
    28378,  25997,  23170,  19948,  16384,  12540,  8481,   4277,   0,      -4277,  -8481,  -12540, -16384, -19948,
    -23170, -25997, 4702,   9307,   13719,  17847,  21605,  24917,  27713,  29935,  31538,  32488,  32765,  32365,
    31294,  29576,  27246,  24351,  20953,  17121,  12935,  8481,   3851,   -858,   -5549,  -10126, -14493, -18560,
    -22243, -25466, -28161, -30274, -31760, 5126,   10126,  14876,  19261,  23170,  26510,  29197,  31164,  32365,
    32767,  32365,  31164,  29197,  26510,  23170,  19261,  14876,  10126,  5126,   0,      -5126,  -10126, -14876,
    -19261, -23170, -26510, -29197, -31164, -32365, -32768, -32365, 5549,   10938,  16011,  20622,  24636,  27939,
    30435,  32052,  32743,  32488,  31294,  29197,  26255,  22556,  18205,  13328,  8066,   2571,   -2998,  -8481,
    -13719, -18560, -22865, -26510, -29389, -31419, -32541, -32723, -31960, -30274, -27713, 5971,   11743,  17121,
    21926,  25997,  29197,  31419,  32588,  32667,  31651,  29576,  26510,  22556,  17847,  12540,  6813,   858,
    -5126,  -10938, -16384, -21281, -25466, -28797, -31164, -32488, -32723, -31863, -29935, -27005, -23170, -18560,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorReal20[] = {
    31164, 26510, 19261, 26510, 10126, -10126, 19261, -10126, -31164, 10126, -26510, -26510,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_STB RotVectorImag20[] = {
    10126, 19261, 26510, 19261, 31164, 31164, 26510, 31164, 10126, 31164, 19261, -19261,
};
RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow8[] = {
    32610, 3212, 31357, 9512, 28899, 15447, 25330, 20788,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow12[] = {
    32698, 2143, 32138, 6393, 31029, 10533, 29389, 14493, 27246, 18205, 24636, 21605,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow16[] = {
    32729, 1608, 32413, 4808, 31786, 7962, 30853, 11039, 29622, 14010, 28106, 16846, 26320, 19520, 24279, 22006,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow20[] = {
    32743, 1286,  32541, 3851,  32138, 6393,  31538, 8895,  30743, 11342,
    29758, 13719, 28590, 16011, 27246, 18205, 25733, 20286, 24062, 22243,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow24[] = {
    32750, 1072,  32610, 3212,  32330, 5338,  31912, 7441,  31357, 9512,  30668, 11543,
    29847, 13524, 28899, 15447, 27827, 17304, 26635, 19087, 25330, 20788, 23916, 22400,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow32[] = {
    32758, 804,   32679, 2411,  32522, 4011,  32286, 5602,  31972, 7180,  31581, 8740,  31114, 10279, 30572, 11793,
    29957, 13279, 29269, 14733, 28511, 16151, 27684, 17531, 26791, 18868, 25833, 20160, 24812, 21403, 23732, 22595,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow40[] = {
    32762, 643,   32711, 1929,  32610, 3212,  32459, 4490,  32258, 5760,  32007, 7022,  31706, 8274,
    31357, 9512,  30959, 10736, 30514, 11943, 30022, 13132, 29483, 14300, 28899, 15447, 28270, 16569,
    27598, 17666, 26883, 18736, 26127, 19777, 25330, 20788, 24494, 21766, 23621, 22711,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow48[] = {
    32764, 536,   32729, 1608,  32658, 2678,  32553, 3745,  32413, 4808,  32239, 5866,  32029, 6918,  31786, 7962,
    31508, 8998,  31197, 10024, 30853, 11039, 30475, 12043, 30064, 13033, 29622, 14010, 29148, 14972, 28642, 15917,
    28106, 16846, 27540, 17757, 26944, 18648, 26320, 19520, 25667, 20371, 24986, 21199, 24279, 22006, 23546, 22788,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow64[] = {
    32766, 402,   32746, 1206,  32706, 2009,  32647, 2811,  32568, 3612,  32470, 4410,  32352, 5205,  32214, 5998,
    32058, 6787,  31881, 7571,  31686, 8351,  31471, 9127,  31238, 9896,  30986, 10660, 30715, 11417, 30425, 12167,
    30118, 12910, 29792, 13646, 29448, 14373, 29086, 15091, 28707, 15800, 28311, 16500, 27897, 17190, 27467, 17869,
    27020, 18538, 26557, 19195, 26078, 19841, 25583, 20475, 25073, 21097, 24548, 21706, 24008, 22302, 23453, 22884,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow96[] = {
    32767, 268,   32758, 804,   32741, 1340,  32714, 1876,  32679, 2411,  32635, 2945,  32583, 3479,  32522, 4011,
    32452, 4543,  32373, 5073,  32286, 5602,  32190, 6130,  32085, 6655,  31972, 7180,  31850, 7702,  31720, 8222,
    31581, 8740,  31434, 9255,  31278, 9768,  31114, 10279, 30942, 10786, 30761, 11291, 30572, 11793, 30375, 12292,
    30170, 12787, 29957, 13279, 29736, 13767, 29506, 14252, 29269, 14733, 29024, 15210, 28771, 15683, 28511, 16151,
    28243, 16616, 27967, 17075, 27684, 17531, 27394, 17981, 27096, 18427, 26791, 18868, 26478, 19304, 26159, 19735,
    25833, 20160, 25499, 20580, 25159, 20994, 24812, 21403, 24459, 21806, 24099, 22204, 23732, 22595, 23359, 22980,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow120[] = {
    32767, 214,   32762, 643,   32750, 1072,  32734, 1501,  32711, 1929,  32683, 2357,  32649, 2785,  32610,
    3212,  32565, 3638,  32515, 4064,  32459, 4490,  32397, 4914,  32330, 5338,  32258, 5760,  32180, 6182,
    32096, 6603,  32007, 7022,  31912, 7441,  31812, 7858,  31706, 8274,  31595, 8688,  31479, 9101,  31357,
    9512,  31230, 9922,  31097, 10330, 30959, 10736, 30816, 11140, 30668, 11543, 30514, 11943, 30355, 12341,
    30191, 12738, 30022, 13132, 29847, 13524, 29668, 13913, 29483, 14300, 29293, 14685, 29099, 15067, 28899,
    15447, 28694, 15824, 28485, 16198, 28270, 16569, 28051, 16938, 27827, 17304, 27598, 17666, 27364, 18026,
    27126, 18383, 26883, 18736, 26635, 19087, 26383, 19434, 26127, 19777, 25865, 20118, 25600, 20454, 25330,
    20788, 25056, 21118, 24777, 21444, 24494, 21766, 24207, 22085, 23916, 22400, 23621, 22711, 23322, 23018,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow128[] = {
    32767, 201,   32762, 603,   32753, 1005,  32738, 1407,  32718, 1809,  32693, 2210,  32664, 2611,  32629, 3012,
    32590, 3412,  32546, 3812,  32496, 4211,  32442, 4609,  32383, 5007,  32319, 5404,  32251, 5800,  32177, 6195,
    32099, 6590,  32015, 6983,  31927, 7376,  31834, 7767,  31737, 8157,  31634, 8546,  31527, 8933,  31415, 9319,
    31298, 9704,  31177, 10088, 31050, 10469, 30920, 10850, 30784, 11228, 30644, 11605, 30499, 11980, 30350, 12354,
    30196, 12725, 30038, 13095, 29875, 13463, 29707, 13828, 29535, 14192, 29359, 14553, 29178, 14912, 28993, 15269,
    28803, 15624, 28610, 15976, 28411, 16326, 28209, 16673, 28002, 17018, 27791, 17361, 27576, 17700, 27357, 18037,
    27133, 18372, 26906, 18703, 26674, 19032, 26439, 19358, 26199, 19681, 25956, 20001, 25708, 20318, 25457, 20632,
    25202, 20943, 24943, 21251, 24680, 21555, 24414, 21856, 24144, 22154, 23870, 22449, 23593, 22740, 23312, 23028,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow160[] = {
    32767, 161,   32764, 483,   32758, 804,   32749, 1126,  32736, 1447,  32720, 1768,  32701, 2090,  32679, 2411,
    32654, 2731,  32626, 3052,  32594, 3372,  32559, 3692,  32522, 4011,  32481, 4330,  32437, 4649,  32389, 4967,
    32339, 5285,  32286, 5602,  32229, 5919,  32169, 6235,  32107, 6550,  32041, 6865,  31972, 7180,  31900, 7493,
    31825, 7806,  31747, 8118,  31665, 8429,  31581, 8740,  31494, 9049,  31403, 9358,  31310, 9666,  31214, 9973,
    31114, 10279, 31012, 10584, 30906, 10888, 30798, 11191, 30687, 11492, 30572, 11793, 30455, 12093, 30335, 12391,
    30212, 12688, 30086, 12984, 29957, 13279, 29825, 13572, 29690, 13865, 29553, 14155, 29412, 14445, 29269, 14733,
    29123, 15019, 28974, 15305, 28823, 15588, 28668, 15871, 28511, 16151, 28351, 16430, 28188, 16708, 28023, 16984,
    27855, 17258, 27684, 17531, 27511, 17802, 27335, 18071, 27156, 18338, 26975, 18604, 26791, 18868, 26604, 19130,
    26415, 19390, 26223, 19649, 26029, 19905, 25833, 20160, 25633, 20413, 25432, 20663, 25228, 20912, 25021, 21159,
    24812, 21403, 24601, 21646, 24387, 21886, 24171, 22125, 23953, 22361, 23732, 22595, 23509, 22827, 23284, 23056,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow192[] = {
    32767, 134,   32766, 402,   32761, 670,   32755, 938,   32746, 1206,  32735, 1474,  32722, 1742,  32706, 2009,
    32689, 2277,  32669, 2544,  32647, 2811,  32623, 3078,  32597, 3345,  32568, 3612,  32538, 3878,  32505, 4144,
    32470, 4410,  32433, 4675,  32393, 4941,  32352, 5205,  32308, 5470,  32262, 5734,  32214, 5998,  32164, 6261,
    32112, 6524,  32058, 6787,  32001, 7049,  31942, 7310,  31881, 7571,  31818, 7832,  31753, 8092,  31686, 8351,
    31617, 8610,  31545, 8869,  31471, 9127,  31396, 9384,  31318, 9640,  31238, 9896,  31156, 10151, 31072, 10406,
    30986, 10660, 30897, 10913, 30807, 11165, 30715, 11417, 30620, 11668, 30524, 11918, 30425, 12167, 30325, 12416,
    30222, 12664, 30118, 12910, 30011, 13156, 29902, 13401, 29792, 13646, 29679, 13889, 29564, 14131, 29448, 14373,
    29329, 14613, 29209, 14852, 29086, 15091, 28962, 15328, 28835, 15565, 28707, 15800, 28577, 16035, 28445, 16268,
    28311, 16500, 28175, 16731, 28037, 16961, 27897, 17190, 27756, 17417, 27612, 17644, 27467, 17869, 27320, 18093,
    27171, 18316, 27020, 18538, 26868, 18758, 26713, 18978, 26557, 19195, 26399, 19412, 26239, 19627, 26078, 19841,
    25915, 20054, 25750, 20265, 25583, 20475, 25415, 20684, 25245, 20891, 25073, 21097, 24900, 21301, 24724, 21504,
    24548, 21706, 24369, 21906, 24189, 22105, 24008, 22302, 23824, 22498, 23640, 22692, 23453, 22884, 23265, 23075,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow240[] = {
    32767, 107,   32766, 322,   32764, 536,   32759, 751,   32754, 965,   32747, 1179,  32738, 1394,  32729, 1608,
    32717, 1822,  32705, 2036,  32691, 2250,  32675, 2464,  32658, 2678,  32640, 2892,  32621, 3105,  32600, 3319,
    32577, 3532,  32553, 3745,  32528, 3958,  32501, 4171,  32473, 4383,  32444, 4596,  32413, 4808,  32381, 5020,
    32348, 5232,  32313, 5444,  32276, 5655,  32239, 5866,  32200, 6077,  32159, 6288,  32117, 6498,  32074, 6708,
    32029, 6918,  31984, 7127,  31936, 7336,  31887, 7545,  31837, 7754,  31786, 7962,  31733, 8170,  31679, 8377,
    31624, 8585,  31567, 8791,  31508, 8998,  31449, 9204,  31388, 9409,  31326, 9615,  31262, 9819,  31197, 10024,
    31131, 10228, 31063, 10431, 30994, 10634, 30924, 10837, 30853, 11039, 30780, 11241, 30705, 11442, 30630, 11643,
    30553, 11843, 30475, 12043, 30395, 12242, 30315, 12441, 30232, 12639, 30149, 12836, 30064, 13033, 29979, 13230,
    29891, 13426, 29803, 13621, 29713, 13816, 29622, 14010, 29530, 14204, 29436, 14397, 29341, 14589, 29245, 14781,
    29148, 14972, 29049, 15162, 28949, 15352, 28848, 15541, 28746, 15730, 28642, 15917, 28537, 16105, 28431, 16291,
    28324, 16477, 28216, 16662, 28106, 16846, 27995, 17030, 27883, 17213, 27770, 17395, 27655, 17576, 27540, 17757,
    27423, 17937, 27305, 18116, 27186, 18294, 27066, 18472, 26944, 18648, 26821, 18824, 26698, 18999, 26573, 19174,
    26447, 19347, 26320, 19520, 26191, 19692, 26062, 19863, 25931, 20033, 25800, 20202, 25667, 20371, 25533, 20538,
    25398, 20705, 25262, 20871, 25125, 21035, 24986, 21199, 24847, 21363, 24707, 21525, 24565, 21686, 24423, 21846,
    24279, 22006, 24135, 22164, 23989, 22322, 23843, 22478, 23695, 22634, 23546, 22788, 23397, 22942, 23246, 23095,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow256[] = {
    32767, 101,   32767, 302,   32764, 503,   32760, 704,   32756, 905,   32749, 1106,  32742, 1307,  32733, 1507,
    32723, 1708,  32712, 1909,  32700, 2110,  32686, 2310,  32672, 2511,  32656, 2711,  32638, 2912,  32620, 3112,
    32600, 3312,  32579, 3512,  32557, 3712,  32534, 3911,  32509, 4111,  32483, 4310,  32456, 4510,  32428, 4709,
    32398, 4907,  32368, 5106,  32336, 5305,  32303, 5503,  32268, 5701,  32233, 5899,  32196, 6097,  32158, 6294,
    32119, 6491,  32078, 6688,  32037, 6885,  31994, 7081,  31950, 7278,  31904, 7473,  31858, 7669,  31810, 7864,
    31761, 8059,  31711, 8254,  31660, 8449,  31608, 8643,  31554, 8836,  31499, 9030,  31443, 9223,  31386, 9416,
    31328, 9608,  31268, 9800,  31207, 9992,  31146, 10183, 31082, 10374, 31018, 10565, 30953, 10755, 30886, 10945,
    30819, 11134, 30750, 11323, 30680, 11511, 30608, 11699, 30536, 11887, 30462, 12074, 30388, 12261, 30312, 12447,
    30235, 12633, 30157, 12818, 30078, 13003, 29997, 13187, 29916, 13371, 29833, 13554, 29750, 13737, 29665, 13919,
    29579, 14101, 29492, 14282, 29404, 14463, 29314, 14643, 29224, 14823, 29132, 15002, 29040, 15180, 28946, 15358,
    28851, 15535, 28755, 15712, 28658, 15888, 28560, 16064, 28461, 16239, 28361, 16413, 28260, 16587, 28158, 16760,
    28054, 16932, 27950, 17104, 27844, 17275, 27738, 17446, 27630, 17616, 27522, 17785, 27412, 17953, 27301, 18121,
    27190, 18288, 27077, 18455, 26963, 18621, 26848, 18786, 26733, 18950, 26616, 19114, 26498, 19277, 26379, 19439,
    26259, 19601, 26139, 19761, 26017, 19921, 25894, 20081, 25771, 20239, 25646, 20397, 25520, 20554, 25394, 20710,
    25266, 20865, 25138, 21020, 25008, 21174, 24878, 21327, 24746, 21479, 24614, 21631, 24481, 21781, 24347, 21931,
    24212, 22080, 24076, 22228, 23939, 22375, 23801, 22522, 23663, 22668, 23523, 22812, 23383, 22956, 23241, 23099,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow384[] = {
    32767, 67,    32767, 201,   32766, 335,   32765, 469,   32762, 603,   32760, 737,   32756, 871,   32753, 1005,
    32748, 1139,  32743, 1273,  32738, 1407,  32732, 1541,  32725, 1675,  32718, 1809,  32710, 1942,  32702, 2076,
    32693, 2210,  32684, 2344,  32674, 2477,  32664, 2611,  32653, 2745,  32641, 2878,  32629, 3012,  32617, 3145,
    32604, 3279,  32590, 3412,  32576, 3545,  32561, 3678,  32546, 3812,  32530, 3945,  32513, 4078,  32496, 4211,
    32479, 4344,  32461, 4476,  32442, 4609,  32423, 4742,  32403, 4874,  32383, 5007,  32362, 5139,  32341, 5272,
    32319, 5404,  32297, 5536,  32274, 5668,  32251, 5800,  32227, 5932,  32202, 6064,  32177, 6195,  32151, 6327,
    32125, 6458,  32099, 6590,  32071, 6721,  32044, 6852,  32015, 6983,  31986, 7114,  31957, 7245,  31927, 7376,
    31897, 7506,  31866, 7637,  31834, 7767,  31802, 7897,  31770, 8027,  31737, 8157,  31703, 8287,  31669, 8416,
    31634, 8546,  31599, 8675,  31563, 8804,  31527, 8933,  31490, 9062,  31453, 9191,  31415, 9319,  31376, 9448,
    31338, 9576,  31298, 9704,  31258, 9832,  31218, 9960,  31177, 10088, 31135, 10215, 31093, 10342, 31050, 10469,
    31007, 10596, 30964, 10723, 30920, 10850, 30875, 10976, 30830, 11102, 30784, 11228, 30738, 11354, 30691, 11480,
    30644, 11605, 30596, 11730, 30548, 11856, 30499, 11980, 30450, 12105, 30400, 12230, 30350, 12354, 30299, 12478,
    30248, 12602, 30196, 12725, 30144, 12849, 30091, 12972, 30038, 13095, 29984, 13218, 29930, 13340, 29875, 13463,
    29819, 13585, 29764, 13706, 29707, 13828, 29651, 13950, 29593, 14071, 29535, 14192, 29477, 14312, 29418, 14433,
    29359, 14553, 29299, 14673, 29239, 14793, 29178, 14912, 29117, 15031, 29055, 15150, 28993, 15269, 28930, 15388,
    28867, 15506, 28803, 15624, 28739, 15741, 28675, 15859, 28610, 15976, 28544, 16093, 28478, 16210, 28411, 16326,
    28344, 16442, 28277, 16558, 28209, 16673, 28140, 16789, 28072, 16904, 28002, 17018, 27932, 17133, 27862, 17247,
    27791, 17361, 27720, 17474, 27648, 17587, 27576, 17700, 27503, 17813, 27430, 17925, 27357, 18037, 27283, 18149,
    27208, 18261, 27133, 18372, 27058, 18483, 26982, 18593, 26906, 18703, 26829, 18813, 26752, 18923, 26674, 19032,
    26596, 19141, 26518, 19250, 26439, 19358, 26359, 19466, 26280, 19574, 26199, 19681, 26119, 19788, 26037, 19895,
    25956, 20001, 25874, 20107, 25791, 20213, 25708, 20318, 25625, 20423, 25541, 20528, 25457, 20632, 25372, 20736,
    25287, 20840, 25202, 20943, 25116, 21046, 25030, 21148, 24943, 21251, 24856, 21352, 24768, 21454, 24680, 21555,
    24592, 21656, 24503, 21756, 24414, 21856, 24324, 21956, 24234, 22055, 24144, 22154, 24053, 22253, 23962, 22351,
    23870, 22449, 23778, 22546, 23686, 22643, 23593, 22740, 23500, 22836, 23406, 22932, 23312, 23028, 23218, 23123,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow480[] = {
    32767, 54,    32767, 161,   32767, 268,   32766, 375,   32764, 483,   32763, 590,   32761, 697,   32758, 804,
    32755, 911,   32752, 1019,  32749, 1126,  32745, 1233,  32741, 1340,  32736, 1447,  32731, 1554,  32726, 1661,
    32720, 1768,  32714, 1876,  32708, 1983,  32701, 2090,  32694, 2197,  32687, 2304,  32679, 2411,  32671, 2517,
    32663, 2624,  32654, 2731,  32645, 2838,  32635, 2945,  32626, 3052,  32615, 3158,  32605, 3265,  32594, 3372,
    32583, 3479,  32571, 3585,  32559, 3692,  32547, 3798,  32535, 3905,  32522, 4011,  32508, 4118,  32495, 4224,
    32481, 4330,  32466, 4437,  32452, 4543,  32437, 4649,  32421, 4755,  32405, 4861,  32389, 4967,  32373, 5073,
    32356, 5179,  32339, 5285,  32322, 5391,  32304, 5496,  32286, 5602,  32267, 5708,  32248, 5813,  32229, 5919,
    32209, 6024,  32190, 6130,  32169, 6235,  32149, 6340,  32128, 6445,  32107, 6550,  32085, 6655,  32063, 6760,
    32041, 6865,  32018, 6970,  31995, 7075,  31972, 7180,  31948, 7284,  31924, 7389,  31900, 7493,  31875, 7597,
    31850, 7702,  31825, 7806,  31799, 7910,  31773, 8014,  31747, 8118,  31720, 8222,  31693, 8326,  31665, 8429,
    31638, 8533,  31609, 8636,  31581, 8740,  31552, 8843,  31523, 8946,  31494, 9049,  31464, 9152,  31434, 9255,
    31403, 9358,  31373, 9461,  31341, 9563,  31310, 9666,  31278, 9768,  31246, 9871,  31214, 9973,  31181, 10075,
    31148, 10177, 31114, 10279, 31080, 10380, 31046, 10482, 31012, 10584, 30977, 10685, 30942, 10786, 30906, 10888,
    30871, 10989, 30834, 11090, 30798, 11191, 30761, 11291, 30724, 11392, 30687, 11492, 30649, 11593, 30611, 11693,
    30572, 11793, 30534, 11893, 30494, 11993, 30455, 12093, 30415, 12192, 30375, 12292, 30335, 12391, 30294, 12490,
    30253, 12589, 30212, 12688, 30170, 12787, 30128, 12886, 30086, 12984, 30043, 13083, 30000, 13181, 29957, 13279,
    29913, 13377, 29869, 13475, 29825, 13572, 29780, 13670, 29736, 13767, 29690, 13865, 29645, 13962, 29599, 14059,
    29553, 14155, 29506, 14252, 29460, 14348, 29412, 14445, 29365, 14541, 29317, 14637, 29269, 14733, 29221, 14829,
    29172, 14924, 29123, 15019, 29074, 15115, 29024, 15210, 28974, 15305, 28924, 15399, 28873, 15494, 28823, 15588,
    28771, 15683, 28720, 15777, 28668, 15871, 28616, 15964, 28564, 16058, 28511, 16151, 28458, 16245, 28405, 16338,
    28351, 16430, 28297, 16523, 28243, 16616, 28188, 16708, 28134, 16800, 28078, 16892, 28023, 16984, 27967, 17075,
    27911, 17167, 27855, 17258, 27798, 17349, 27741, 17440, 27684, 17531, 27627, 17621, 27569, 17712, 27511, 17802,
    27452, 17892, 27394, 17981, 27335, 18071, 27275, 18160, 27216, 18249, 27156, 18338, 27096, 18427, 27035, 18516,
    26975, 18604, 26914, 18692, 26852, 18780, 26791, 18868, 26729, 18956, 26667, 19043, 26604, 19130, 26541, 19217,
    26478, 19304, 26415, 19390, 26351, 19477, 26288, 19563, 26223, 19649, 26159, 19735, 26094, 19820, 26029, 19905,
    25964, 19990, 25898, 20075, 25833, 20160, 25766, 20244, 25700, 20329, 25633, 20413, 25566, 20496, 25499, 20580,
    25432, 20663, 25364, 20746, 25296, 20829, 25228, 20912, 25159, 20994, 25090, 21077, 25021, 21159, 24952, 21240,
    24882, 21322, 24812, 21403, 24742, 21484, 24672, 21565, 24601, 21646, 24530, 21726, 24459, 21806, 24387, 21886,
    24315, 21966, 24243, 22045, 24171, 22125, 24099, 22204, 24026, 22282, 23953, 22361, 23880, 22439, 23806, 22517,
    23732, 22595, 23658, 22672, 23584, 22750, 23509, 22827, 23434, 22904, 23359, 22980, 23284, 23056, 23208, 23133,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow512[] = {
    32767, 50,    32767, 151,   32767, 251,   32766, 352,   32765, 452,   32763, 553,   32761, 653,   32759, 754,
    32757, 854,   32754, 955,   32751, 1055,  32748, 1156,  32744, 1256,  32740, 1357,  32736, 1457,  32731, 1558,
    32726, 1658,  32721, 1758,  32715, 1859,  32709, 1959,  32703, 2060,  32697, 2160,  32690, 2260,  32683, 2360,
    32675, 2461,  32668, 2561,  32660, 2661,  32651, 2761,  32643, 2861,  32634, 2962,  32625, 3062,  32615, 3162,
    32605, 3262,  32595, 3362,  32585, 3462,  32574, 3562,  32563, 3662,  32551, 3762,  32540, 3861,  32528, 3961,
    32515, 4061,  32503, 4161,  32490, 4260,  32477, 4360,  32463, 4460,  32449, 4559,  32435, 4659,  32421, 4758,
    32406, 4858,  32391, 4957,  32376, 5057,  32360, 5156,  32344, 5255,  32328, 5354,  32311, 5453,  32294, 5553,
    32277, 5652,  32259, 5751,  32242, 5850,  32224, 5948,  32205, 6047,  32186, 6146,  32167, 6245,  32148, 6343,
    32129, 6442,  32109, 6541,  32088, 6639,  32068, 6737,  32047, 6836,  32026, 6934,  32005, 7032,  31983, 7130,
    31961, 7229,  31938, 7327,  31916, 7425,  31893, 7522,  31870, 7620,  31846, 7718,  31822, 7816,  31798, 7913,
    31774, 8011,  31749, 8108,  31724, 8206,  31699, 8303,  31673, 8400,  31647, 8497,  31621, 8594,  31594, 8691,
    31568, 8788,  31540, 8885,  31513, 8982,  31485, 9078,  31457, 9175,  31429, 9271,  31400, 9368,  31372, 9464,
    31342, 9560,  31313, 9656,  31283, 9752,  31253, 9848,  31223, 9944,  31192, 10040, 31161, 10135, 31130, 10231,
    31098, 10326, 31067, 10422, 31034, 10517, 31002, 10612, 30969, 10707, 30936, 10802, 30903, 10897, 30869, 10992,
    30836, 11087, 30801, 11181, 30767, 11276, 30732, 11370, 30697, 11464, 30662, 11558, 30626, 11652, 30590, 11746,
    30554, 11840, 30518, 11934, 30481, 12027, 30444, 12121, 30407, 12214, 30369, 12307, 30331, 12400, 30293, 12493,
    30254, 12586, 30216, 12679, 30177, 12772, 30137, 12864, 30098, 12957, 30058, 13049, 30018, 13141, 29977, 13233,
    29936, 13325, 29895, 13417, 29854, 13508, 29813, 13600, 29771, 13691, 29729, 13783, 29686, 13874, 29643, 13965,
    29600, 14056, 29557, 14146, 29514, 14237, 29470, 14327, 29426, 14418, 29381, 14508, 29337, 14598, 29292, 14688,
    29247, 14778, 29201, 14867, 29155, 14957, 29109, 15046, 29063, 15136, 29016, 15225, 28970, 15314, 28922, 15402,
    28875, 15491, 28827, 15580, 28779, 15668, 28731, 15756, 28683, 15844, 28634, 15932, 28585, 16020, 28536, 16108,
    28486, 16195, 28436, 16282, 28386, 16369, 28336, 16456, 28285, 16543, 28234, 16630, 28183, 16717, 28132, 16803,
    28080, 16889, 28028, 16975, 27976, 17061, 27924, 17147, 27871, 17233, 27818, 17318, 27765, 17403, 27711, 17488,
    27657, 17573, 27603, 17658, 27549, 17743, 27494, 17827, 27440, 17911, 27384, 17995, 27329, 18079, 27273, 18163,
    27218, 18247, 27162, 18330, 27105, 18413, 27049, 18496, 26992, 18579, 26935, 18662, 26877, 18745, 26820, 18827,
    26762, 18909, 26704, 18991, 26645, 19073, 26586, 19155, 26528, 19236, 26468, 19317, 26409, 19399, 26349, 19479,
    26290, 19560, 26229, 19641, 26169, 19721, 26108, 19801, 26048, 19881, 25986, 19961, 25925, 20041, 25863, 20120,
    25802, 20200, 25739, 20279, 25677, 20357, 25615, 20436, 25552, 20515, 25489, 20593, 25425, 20671, 25362, 20749,
    25298, 20827, 25234, 20904, 25170, 20981, 25105, 21059, 25041, 21136, 24976, 21212, 24910, 21289, 24845, 21365,
    24779, 21441, 24713, 21517, 24647, 21593, 24581, 21668, 24514, 21744, 24448, 21819, 24380, 21894, 24313, 21968,
    24246, 22043, 24178, 22117, 24110, 22191, 24042, 22265, 23973, 22339, 23905, 22412, 23836, 22485, 23767, 22558,
    23697, 22631, 23628, 22704, 23558, 22776, 23488, 22848, 23418, 22920, 23348, 22992, 23277, 23064, 23206, 23135,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow768[] = {
    32767, 34,    32767, 101,   32767, 168,   32767, 235,   32767, 302,   32766, 369,   32765, 436,   32764, 503,
    32763, 570,   32762, 637,   32760, 704,   32759, 771,   32757, 838,   32756, 905,   32754, 972,   32752, 1039,
    32749, 1106,  32747, 1173,  32745, 1240,  32742, 1307,  32739, 1374,  32736, 1440,  32733, 1507,  32730, 1574,
    32727, 1641,  32723, 1708,  32720, 1775,  32716, 1842,  32712, 1909,  32708, 1976,  32704, 2043,  32700, 2110,
    32696, 2177,  32691, 2243,  32686, 2310,  32682, 2377,  32677, 2444,  32672, 2511,  32666, 2578,  32661, 2644,
    32656, 2711,  32650, 2778,  32644, 2845,  32638, 2912,  32632, 2978,  32626, 3045,  32620, 3112,  32613, 3178,
    32607, 3245,  32600, 3312,  32593, 3379,  32586, 3445,  32579, 3512,  32572, 3578,  32565, 3645,  32557, 3712,
    32549, 3778,  32542, 3845,  32534, 3911,  32526, 3978,  32517, 4044,  32509, 4111,  32501, 4177,  32492, 4244,
    32483, 4310,  32474, 4377,  32465, 4443,  32456, 4510,  32447, 4576,  32437, 4642,  32428, 4709,  32418, 4775,
    32408, 4841,  32398, 4907,  32388, 4974,  32378, 5040,  32368, 5106,  32357, 5172,  32347, 5239,  32336, 5305,
    32325, 5371,  32314, 5437,  32303, 5503,  32291, 5569,  32280, 5635,  32268, 5701,  32257, 5767,  32245, 5833,
    32233, 5899,  32221, 5965,  32208, 6031,  32196, 6097,  32183, 6162,  32171, 6228,  32158, 6294,  32145, 6360,
    32132, 6426,  32119, 6491,  32105, 6557,  32092, 6623,  32078, 6688,  32064, 6754,  32051, 6819,  32037, 6885,
    32022, 6950,  32008, 7016,  31994, 7081,  31979, 7147,  31964, 7212,  31950, 7278,  31935, 7343,  31920, 7408,
    31904, 7473,  31889, 7539,  31874, 7604,  31858, 7669,  31842, 7734,  31826, 7799,  31810, 7864,  31794, 7929,
    31778, 7994,  31761, 8059,  31745, 8124,  31728, 8189,  31711, 8254,  31694, 8319,  31677, 8384,  31660, 8449,
    31643, 8513,  31625, 8578,  31608, 8643,  31590, 8707,  31572, 8772,  31554, 8836,  31536, 8901,  31518, 8965,
    31499, 9030,  31481, 9094,  31462, 9159,  31443, 9223,  31424, 9287,  31405, 9352,  31386, 9416,  31367, 9480,
    31347, 9544,  31328, 9608,  31308, 9672,  31288, 9736,  31268, 9800,  31248, 9864,  31228, 9928,  31207, 9992,
    31187, 10056, 31166, 10119, 31146, 10183, 31125, 10247, 31104, 10311, 31082, 10374, 31061, 10438, 31040, 10501,
    31018, 10565, 30997, 10628, 30975, 10691, 30953, 10755, 30931, 10818, 30909, 10881, 30886, 10945, 30864, 11008,
    30841, 11071, 30819, 11134, 30796, 11197, 30773, 11260, 30750, 11323, 30726, 11386, 30703, 11448, 30680, 11511,
    30656, 11574, 30632, 11637, 30608, 11699, 30584, 11762, 30560, 11824, 30536, 11887, 30512, 11949, 30487, 12012,
    30462, 12074, 30438, 12136, 30413, 12198, 30388, 12261, 30363, 12323, 30337, 12385, 30312, 12447, 30286, 12509,
    30261, 12571, 30235, 12633, 30209, 12694, 30183, 12756, 30157, 12818, 30131, 12880, 30104, 12941, 30078, 13003,
    30051, 13064, 30024, 13126, 29997, 13187, 29970, 13248, 29943, 13310, 29916, 13371, 29889, 13432, 29861, 13493,
    29833, 13554, 29806, 13615, 29778, 13676, 29750, 13737, 29721, 13798, 29693, 13858, 29665, 13919, 29636, 13980,
    29608, 14040, 29579, 14101, 29550, 14161, 29521, 14222, 29492, 14282, 29462, 14342, 29433, 14403, 29404, 14463,
    29374, 14523, 29344, 14583, 29314, 14643, 29284, 14703, 29254, 14763, 29224, 14823, 29193, 14882, 29163, 14942,
    29132, 15002, 29102, 15061, 29071, 15121, 29040, 15180, 29009, 15239, 28977, 15299, 28946, 15358, 28915, 15417,
    28883, 15476, 28851, 15535, 28819, 15594, 28787, 15653, 28755, 15712, 28723, 15771, 28691, 15830, 28658, 15888,
    28626, 15947, 28593, 16005, 28560, 16064, 28528, 16122, 28494, 16180, 28461, 16239, 28428, 16297, 28395, 16355,
    28361, 16413, 28328, 16471, 28294, 16529, 28260, 16587, 28226, 16644, 28192, 16702, 28158, 16760, 28123, 16817,
    28089, 16875, 28054, 16932, 28020, 16990, 27985, 17047, 27950, 17104, 27915, 17161, 27880, 17218, 27844, 17275,
    27809, 17332, 27773, 17389, 27738, 17446, 27702, 17502, 27666, 17559, 27630, 17616, 27594, 17672, 27558, 17729,
    27522, 17785, 27485, 17841, 27449, 17897, 27412, 17953, 27375, 18009, 27338, 18065, 27301, 18121, 27264, 18177,
    27227, 18233, 27190, 18288, 27152, 18344, 27115, 18399, 27077, 18455, 27039, 18510, 27001, 18566, 26963, 18621,
    26925, 18676, 26887, 18731, 26848, 18786, 26810, 18841, 26771, 18895, 26733, 18950, 26694, 19005, 26655, 19059,
    26616, 19114, 26577, 19168, 26537, 19223, 26498, 19277, 26459, 19331, 26419, 19385, 26379, 19439, 26339, 19493,
    26300, 19547, 26259, 19601, 26219, 19654, 26179, 19708, 26139, 19761, 26098, 19815, 26058, 19868, 26017, 19921,
    25976, 19974, 25935, 20028, 25894, 20081, 25853, 20133, 25812, 20186, 25771, 20239, 25729, 20292, 25688, 20344,
    25646, 20397, 25604, 20449, 25562, 20502, 25520, 20554, 25478, 20606, 25436, 20658, 25394, 20710, 25351, 20762,
    25309, 20814, 25266, 20865, 25223, 20917, 25181, 20969, 25138, 21020, 25095, 21071, 25051, 21123, 25008, 21174,
    24965, 21225, 24921, 21276, 24878, 21327, 24834, 21378, 24790, 21429, 24746, 21479, 24702, 21530, 24658, 21580,
    24614, 21631, 24570, 21681, 24525, 21731, 24481, 21781, 24436, 21831, 24392, 21881, 24347, 21931, 24302, 21981,
    24257, 22030, 24212, 22080, 24167, 22130, 24121, 22179, 24076, 22228, 24030, 22277, 23985, 22326, 23939, 22375,
    23893, 22424, 23847, 22473, 23801, 22522, 23755, 22571, 23709, 22619, 23663, 22668, 23616, 22716, 23570, 22764,
    23523, 22812, 23476, 22860, 23430, 22908, 23383, 22956, 23336, 23004, 23289, 23052, 23241, 23099, 23194, 23147,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow960[] = {
    32767, 27,    32767, 80,    32767, 134,   32767, 188,   32767, 241,   32767, 295,   32766, 349,   32766, 402,
    32765, 456,   32764, 509,   32763, 563,   32762, 617,   32761, 670,   32760, 724,   32759, 777,   32757, 831,
    32756, 885,   32755, 938,   32753, 992,   32751, 1045,  32750, 1099,  32748, 1153,  32746, 1206,  32744, 1260,
    32742, 1313,  32739, 1367,  32737, 1420,  32735, 1474,  32732, 1528,  32730, 1581,  32727, 1635,  32724, 1688,
    32722, 1742,  32719, 1795,  32716, 1849,  32713, 1902,  32710, 1956,  32706, 2009,  32703, 2063,  32700, 2116,
    32696, 2170,  32692, 2223,  32689, 2277,  32685, 2330,  32681, 2384,  32677, 2437,  32673, 2491,  32669, 2544,
    32665, 2598,  32661, 2651,  32656, 2705,  32652, 2758,  32647, 2811,  32643, 2865,  32638, 2918,  32633, 2972,
    32628, 3025,  32623, 3078,  32618, 3132,  32613, 3185,  32608, 3239,  32602, 3292,  32597, 3345,  32591, 3399,
    32586, 3452,  32580, 3505,  32574, 3558,  32568, 3612,  32562, 3665,  32556, 3718,  32550, 3772,  32544, 3825,
    32538, 3878,  32531, 3931,  32525, 3985,  32518, 4038,  32512, 4091,  32505, 4144,  32498, 4197,  32491, 4251,
    32484, 4304,  32477, 4357,  32470, 4410,  32463, 4463,  32455, 4516,  32448, 4569,  32440, 4622,  32433, 4675,
    32425, 4728,  32417, 4782,  32409, 4835,  32401, 4888,  32393, 4941,  32385, 4994,  32377, 5047,  32369, 5100,
    32360, 5153,  32352, 5205,  32343, 5258,  32335, 5311,  32326, 5364,  32317, 5417,  32308, 5470,  32299, 5523,
    32290, 5576,  32281, 5628,  32272, 5681,  32262, 5734,  32253, 5787,  32243, 5840,  32234, 5892,  32224, 5945,
    32214, 5998,  32205, 6051,  32195, 6103,  32185, 6156,  32174, 6209,  32164, 6261,  32154, 6314,  32144, 6366,
    32133, 6419,  32123, 6472,  32112, 6524,  32101, 6577,  32090, 6629,  32080, 6682,  32069, 6734,  32058, 6787,
    32046, 6839,  32035, 6891,  32024, 6944,  32012, 6996,  32001, 7049,  31989, 7101,  31978, 7153,  31966, 7206,
    31954, 7258,  31942, 7310,  31930, 7362,  31918, 7415,  31906, 7467,  31894, 7519,  31881, 7571,  31869, 7623,
    31856, 7676,  31844, 7728,  31831, 7780,  31818, 7832,  31805, 7884,  31792, 7936,  31779, 7988,  31766, 8040,
    31753, 8092,  31740, 8144,  31726, 8196,  31713, 8248,  31700, 8300,  31686, 8351,  31672, 8403,  31658, 8455,
    31645, 8507,  31631, 8559,  31617, 8610,  31602, 8662,  31588, 8714,  31574, 8765,  31559, 8817,  31545, 8869,
    31530, 8920,  31516, 8972,  31501, 9023,  31486, 9075,  31471, 9127,  31456, 9178,  31441, 9229,  31426, 9281,
    31411, 9332,  31396, 9384,  31380, 9435,  31365, 9486,  31349, 9538,  31334, 9589,  31318, 9640,  31302, 9691,
    31286, 9743,  31270, 9794,  31254, 9845,  31238, 9896,  31222, 9947,  31205, 9998,  31189, 10049, 31172, 10100,
    31156, 10151, 31139, 10202, 31123, 10253, 31106, 10304, 31089, 10355, 31072, 10406, 31055, 10457, 31038, 10508,
    31020, 10558, 31003, 10609, 30986, 10660, 30968, 10710, 30951, 10761, 30933, 10812, 30915, 10862, 30897, 10913,
    30880, 10963, 30862, 11014, 30843, 11064, 30825, 11115, 30807, 11165, 30789, 11216, 30770, 11266, 30752, 11316,
    30733, 11367, 30715, 11417, 30696, 11467, 30677, 11517, 30658, 11568, 30639, 11618, 30620, 11668, 30601, 11718,
    30582, 11768, 30563, 11818, 30543, 11868, 30524, 11918, 30504, 11968, 30485, 12018, 30465, 12068, 30445, 12118,
    30425, 12167, 30405, 12217, 30385, 12267, 30365, 12317, 30345, 12366, 30325, 12416, 30304, 12465, 30284, 12515,
    30263, 12565, 30243, 12614, 30222, 12664, 30201, 12713, 30181, 12762, 30160, 12812, 30139, 12861, 30118, 12910,
    30096, 12960, 30075, 13009, 30054, 13058, 30032, 13107, 30011, 13156, 29989, 13205, 29968, 13254, 29946, 13303,
    29924, 13352, 29902, 13401, 29880, 13450, 29858, 13499, 29836, 13548, 29814, 13597, 29792, 13646, 29769, 13694,
    29747, 13743, 29724, 13792, 29702, 13840, 29679, 13889, 29656, 13937, 29633, 13986, 29610, 14034, 29587, 14083,
    29564, 14131, 29541, 14180, 29518, 14228, 29495, 14276, 29471, 14324, 29448, 14373, 29424, 14421, 29401, 14469,
    29377, 14517, 29353, 14565, 29329, 14613, 29305, 14661, 29281, 14709, 29257, 14757, 29233, 14805, 29209, 14852,
    29184, 14900, 29160, 14948, 29135, 14996, 29111, 15043, 29086, 15091, 29061, 15139, 29037, 15186, 29012, 15234,
    28987, 15281, 28962, 15328, 28937, 15376, 28911, 15423, 28886, 15470, 28861, 15518, 28835, 15565, 28810, 15612,
    28784, 15659, 28759, 15706, 28733, 15753, 28707, 15800, 28681, 15847, 28655, 15894, 28629, 15941, 28603, 15988,
    28577, 16035, 28551, 16081, 28524, 16128, 28498, 16175, 28471, 16221, 28445, 16268, 28418, 16314, 28391, 16361,
    28365, 16407, 28338, 16454, 28311, 16500, 28284, 16546, 28257, 16593, 28229, 16639, 28202, 16685, 28175, 16731,
    28147, 16777, 28120, 16823, 28092, 16869, 28065, 16915, 28037, 16961, 28009, 17007, 27981, 17053, 27953, 17098,
    27925, 17144, 27897, 17190, 27869, 17235, 27841, 17281, 27812, 17326, 27784, 17372, 27756, 17417, 27727, 17463,
    27698, 17508, 27670, 17553, 27641, 17599, 27612, 17644, 27583, 17689, 27554, 17734, 27525, 17779, 27496, 17824,
    27467, 17869, 27438, 17914, 27408, 17959, 27379, 18004, 27349, 18049, 27320, 18093, 27290, 18138, 27260, 18183,
    27231, 18227, 27201, 18272, 27171, 18316, 27141, 18361, 27111, 18405, 27081, 18449, 27050, 18494, 27020, 18538,
    26990, 18582, 26959, 18626, 26929, 18670, 26898, 18714, 26868, 18758, 26837, 18802, 26806, 18846, 26775, 18890,
    26744, 18934, 26713, 18978, 26682, 19021, 26651, 19065, 26620, 19108, 26588, 19152, 26557, 19195, 26526, 19239,
    26494, 19282, 26463, 19326, 26431, 19369, 26399, 19412, 26367, 19455, 26335, 19498, 26304, 19541, 26272, 19584,
    26239, 19627, 26207, 19670, 26175, 19713, 26143, 19756, 26110, 19799, 26078, 19841, 26045, 19884, 26013, 19927,
    25980, 19969, 25948, 20012, 25915, 20054, 25882, 20096, 25849, 20139, 25816, 20181, 25783, 20223, 25750, 20265,
    25717, 20308, 25683, 20350, 25650, 20392, 25617, 20434, 25583, 20475, 25550, 20517, 25516, 20559, 25482, 20601,
    25449, 20642, 25415, 20684, 25381, 20726, 25347, 20767, 25313, 20809, 25279, 20850, 25245, 20891, 25211, 20933,
    25176, 20974, 25142, 21015, 25107, 21056, 25073, 21097, 25038, 21138, 25004, 21179, 24969, 21220, 24934, 21261,
    24900, 21301, 24865, 21342, 24830, 21383, 24795, 21423, 24760, 21464, 24724, 21504, 24689, 21545, 24654, 21585,
    24619, 21626, 24583, 21666, 24548, 21706, 24512, 21746, 24477, 21786, 24441, 21826, 24405, 21866, 24369, 21906,
    24333, 21946, 24297, 21986, 24261, 22026, 24225, 22065, 24189, 22105, 24153, 22144, 24117, 22184, 24080, 22223,
    24044, 22263, 24008, 22302, 23971, 22341, 23935, 22380, 23898, 22420, 23861, 22459, 23824, 22498, 23788, 22537,
    23751, 22575, 23714, 22614, 23677, 22653, 23640, 22692, 23602, 22730, 23565, 22769, 23528, 22808, 23490, 22846,
    23453, 22884, 23416, 22923, 23378, 22961, 23340, 22999, 23303, 23037, 23265, 23075, 23227, 23114, 23189, 23152,
};
RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP SineWindow1024[] = {
    32767, 25,    32767, 75,    32767, 126,   32767, 176,   32767, 226,   32767, 276,   32766, 327,   32766, 377,
    32765, 427,   32765, 478,   32764, 528,   32763, 578,   32762, 628,   32761, 679,   32760, 729,   32759, 779,
    32758, 829,   32756, 880,   32755, 930,   32753, 980,   32752, 1030,  32750, 1081,  32748, 1131,  32747, 1181,
    32745, 1231,  32743, 1281,  32741, 1332,  32739, 1382,  32737, 1432,  32734, 1482,  32732, 1533,  32730, 1583,
    32727, 1633,  32725, 1683,  32722, 1733,  32719, 1784,  32717, 1834,  32714, 1884,  32711, 1934,  32708, 1984,
    32705, 2034,  32702, 2085,  32698, 2135,  32695, 2185,  32692, 2235,  32688, 2285,  32685, 2335,  32681, 2385,
    32677, 2436,  32674, 2486,  32670, 2536,  32666, 2586,  32662, 2636,  32658, 2686,  32654, 2736,  32649, 2786,
    32645, 2836,  32641, 2887,  32636, 2937,  32632, 2987,  32627, 3037,  32622, 3087,  32618, 3137,  32613, 3187,
    32608, 3237,  32603, 3287,  32598, 3337,  32592, 3387,  32587, 3437,  32582, 3487,  32577, 3537,  32571, 3587,
    32566, 3637,  32560, 3687,  32554, 3737,  32548, 3787,  32543, 3836,  32537, 3886,  32531, 3936,  32525, 3986,
    32518, 4036,  32512, 4086,  32506, 4136,  32500, 4186,  32493, 4236,  32487, 4285,  32480, 4335,  32473, 4385,
    32467, 4435,  32460, 4485,  32453, 4534,  32446, 4584,  32439, 4634,  32432, 4684,  32424, 4733,  32417, 4783,
    32410, 4833,  32402, 4883,  32395, 4932,  32387, 4982,  32379, 5032,  32372, 5081,  32364, 5131,  32356, 5181,
    32348, 5230,  32340, 5280,  32332, 5329,  32323, 5379,  32315, 5429,  32307, 5478,  32298, 5528,  32290, 5577,
    32281, 5627,  32273, 5676,  32264, 5726,  32255, 5775,  32246, 5825,  32237, 5874,  32228, 5924,  32219, 5973,
    32210, 6023,  32201, 6072,  32191, 6121,  32182, 6171,  32172, 6220,  32163, 6269,  32153, 6319,  32143, 6368,
    32133, 6417,  32124, 6467,  32114, 6516,  32104, 6565,  32093, 6614,  32083, 6664,  32073, 6713,  32063, 6762,
    32052, 6811,  32042, 6860,  32031, 6910,  32021, 6959,  32010, 7008,  31999, 7057,  31988, 7106,  31977, 7155,
    31966, 7204,  31955, 7253,  31944, 7302,  31933, 7351,  31921, 7400,  31910, 7449,  31899, 7498,  31887, 7547,
    31875, 7596,  31864, 7645,  31852, 7694,  31840, 7742,  31828, 7791,  31816, 7840,  31804, 7889,  31792, 7938,
    31780, 7986,  31768, 8035,  31755, 8084,  31743, 8133,  31730, 8181,  31718, 8230,  31705, 8279,  31692, 8327,
    31679, 8376,  31667, 8424,  31654, 8473,  31641, 8521,  31627, 8570,  31614, 8618,  31601, 8667,  31588, 8715,
    31574, 8764,  31561, 8812,  31547, 8861,  31534, 8909,  31520, 8957,  31506, 9006,  31492, 9054,  31478, 9102,
    31464, 9151,  31450, 9199,  31436, 9247,  31422, 9295,  31408, 9344,  31393, 9392,  31379, 9440,  31364, 9488,
    31350, 9536,  31335, 9584,  31320, 9632,  31305, 9680,  31291, 9728,  31276, 9776,  31261, 9824,  31246, 9872,
    31230, 9920,  31215, 9968,  31200, 10016, 31184, 10064, 31169, 10112, 31153, 10159, 31138, 10207, 31122, 10255,
    31106, 10303, 31090, 10350, 31074, 10398, 31059, 10446, 31042, 10493, 31026, 10541, 31010, 10588, 30994, 10636,
    30977, 10684, 30961, 10731, 30945, 10779, 30928, 10826, 30911, 10873, 30895, 10921, 30878, 10968, 30861, 11016,
    30844, 11063, 30827, 11110, 30810, 11157, 30793, 11205, 30776, 11252, 30758, 11299, 30741, 11346, 30723, 11393,
    30706, 11441, 30688, 11488, 30671, 11535, 30653, 11582, 30635, 11629, 30617, 11676, 30599, 11723, 30581, 11770,
    30563, 11816, 30545, 11863, 30527, 11910, 30509, 11957, 30490, 12004, 30472, 12051, 30453, 12097, 30435, 12144,
    30416, 12191, 30397, 12237, 30378, 12284, 30360, 12330, 30341, 12377, 30322, 12424, 30302, 12470, 30283, 12517,
    30264, 12563, 30245, 12609, 30225, 12656, 30206, 12702, 30186, 12748, 30167, 12795, 30147, 12841, 30127, 12887,
    30108, 12933, 30088, 12980, 30068, 13026, 30048, 13072, 30028, 13118, 30008, 13164, 29987, 13210, 29967, 13256,
    29947, 13302, 29926, 13348, 29906, 13394, 29885, 13440, 29864, 13485, 29844, 13531, 29823, 13577, 29802, 13623,
    29781, 13668, 29760, 13714, 29739, 13760, 29718, 13805, 29697, 13851, 29675, 13896, 29654, 13942, 29633, 13987,
    29611, 14033, 29590, 14078, 29568, 14124, 29546, 14169, 29525, 14214, 29503, 14260, 29481, 14305, 29459, 14350,
    29437, 14395, 29415, 14440, 29392, 14485, 29370, 14530, 29348, 14576, 29325, 14621, 29303, 14665, 29280, 14710,
    29258, 14755, 29235, 14800, 29212, 14845, 29190, 14890, 29167, 14935, 29144, 14979, 29121, 15024, 29098, 15069,
    29075, 15113, 29051, 15158, 29028, 15202, 29005, 15247, 28981, 15291, 28958, 15336, 28934, 15380, 28911, 15425,
    28887, 15469, 28863, 15513, 28839, 15557, 28815, 15602, 28791, 15646, 28767, 15690, 28743, 15734, 28719, 15778,
    28695, 15822, 28671, 15866, 28646, 15910, 28622, 15954, 28597, 15998, 28573, 16042, 28548, 16086, 28523, 16129,
    28499, 16173, 28474, 16217, 28449, 16261, 28424, 16304, 28399, 16348, 28374, 16391, 28349, 16435, 28323, 16478,
    28298, 16522, 28273, 16565, 28247, 16608, 28222, 16652, 28196, 16695, 28170, 16738, 28145, 16781, 28119, 16825,
    28093, 16868, 28067, 16911, 28041, 16954, 28015, 16997, 27989, 17040, 27963, 17083, 27937, 17126, 27910, 17168,
    27884, 17211, 27858, 17254, 27831, 17297, 27805, 17339, 27778, 17382, 27751, 17425, 27724, 17467, 27698, 17510,
    27671, 17552, 27644, 17594, 27617, 17637, 27590, 17679, 27562, 17721, 27535, 17764, 27508, 17806, 27481, 17848,
    27453, 17890, 27426, 17932, 27398, 17974, 27371, 18016, 27343, 18058, 27315, 18100, 27287, 18142, 27260, 18184,
    27232, 18226, 27204, 18268, 27176, 18309, 27147, 18351, 27119, 18393, 27091, 18434, 27063, 18476, 27034, 18517,
    27006, 18559, 26977, 18600, 26949, 18641, 26920, 18683, 26892, 18724, 26863, 18765, 26834, 18806, 26805, 18848,
    26776, 18889, 26747, 18930, 26718, 18971, 26689, 19012, 26660, 19053, 26630, 19093, 26601, 19134, 26572, 19175,
    26542, 19216, 26513, 19256, 26483, 19297, 26454, 19338, 26424, 19378, 26394, 19419, 26364, 19459, 26334, 19500,
    26305, 19540, 26275, 19580, 26244, 19621, 26214, 19661, 26184, 19701, 26154, 19741, 26124, 19781, 26093, 19821,
    26063, 19861, 26032, 19901, 26002, 19941, 25971, 19981, 25940, 20021, 25910, 20061, 25879, 20100, 25848, 20140,
    25817, 20180, 25786, 20219, 25755, 20259, 25724, 20298, 25693, 20338, 25662, 20377, 25630, 20416, 25599, 20456,
    25567, 20495, 25536, 20534, 25504, 20573, 25473, 20612, 25441, 20652, 25410, 20691, 25378, 20729, 25346, 20768,
    25314, 20807, 25282, 20846, 25250, 20885, 25218, 20923, 25186, 20962, 25154, 21001, 25121, 21039, 25089, 21078,
    25057, 21116, 25024, 21155, 24992, 21193, 24959, 21231, 24927, 21270, 24894, 21308, 24861, 21346, 24829, 21384,
    24796, 21422, 24763, 21460, 24730, 21498, 24697, 21536, 24664, 21574, 24631, 21612, 24598, 21649, 24564, 21687,
    24531, 21725, 24498, 21762, 24464, 21800, 24431, 21838, 24397, 21875, 24364, 21912, 24330, 21950, 24296, 21987,
    24263, 22024, 24229, 22061, 24195, 22099, 24161, 22136, 24127, 22173, 24093, 22210, 24059, 22247, 24025, 22284,
    23991, 22320, 23956, 22357, 23922, 22394, 23888, 22431, 23853, 22467, 23819, 22504, 23784, 22540, 23749, 22577,
    23715, 22613, 23680, 22649, 23645, 22686, 23610, 22722, 23576, 22758, 23541, 22794, 23506, 22830, 23471, 22866,
    23436, 22902, 23400, 22938, 23365, 22974, 23330, 23010, 23295, 23046, 23259, 23081, 23224, 23117, 23188, 23153,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow96[] = {
    32767, 2,     32767, 5,     32767, 12,    32767, 21,    32767, 35,    32767, 54,    32767, 79,    32767, 113,
    32767, 155,   32767, 210,   32767, 277,   32766, 359,   32765, 459,   32763, 578,   32760, 718,   32756, 883,
    32750, 1073,  32743, 1292,  32732, 1542,  32717, 1825,  32698, 2142,  32673, 2496,  32640, 2888,  32599, 3320,
    32548, 3792,  32484, 4306,  32405, 4861,  32310, 5458,  32196, 6096,  32060, 6775,  31900, 7493,  31713, 8248,
    31497, 9039,  31248, 9864,  30966, 10718, 30647, 11599, 30289, 12503, 29891, 13427, 29451, 14365, 28969, 15315,
    28443, 16270, 27874, 17228, 27261, 18182, 26606, 19128, 25909, 20062, 25172, 20979, 24397, 21875, 23588, 22746,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow120[] = {
    32767, 1,     32767, 4,     32767, 8,     32767, 14,    32767, 22,    32767, 33,    32767, 48,    32767,
    66,    32767, 89,    32767, 117,   32767, 151,   32767, 193,   32767, 242,   32767, 301,   32766, 369,
    32765, 449,   32764, 541,   32762, 646,   32759, 766,   32756, 902,   32751, 1054,  32745, 1225,  32737,
    1414,  32728, 1625,  32715, 1856,  32700, 2110,  32681, 2387,  32658, 2689,  32629, 3015,  32595, 3367,
    32553, 3744,  32504, 4149,  32446, 4580,  32379, 5037,  32299, 5521,  32208, 6032,  32103, 6569,  31983,
    7131,  31846, 7717,  31692, 8328,  31519, 8960,  31326, 9615,  31111, 10289, 30873, 10981, 30612, 11690,
    30326, 12413, 30014, 13149, 29676, 13895, 29311, 14650, 28918, 15411, 28497, 16176, 28049, 16941, 27572,
    17706, 27068, 18467, 26537, 19223, 25980, 19970, 25397, 20706, 24789, 21430, 24158, 22139, 23505, 22831,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow128[] = {
    32767, 1,     32767, 4,     32767, 8,     32767, 13,    32767, 20,    32767, 29,    32767, 41,    32767, 57,
    32767, 76,    32767, 99,    32767, 128,   32767, 162,   32767, 203,   32767, 251,   32767, 307,   32766, 372,
    32765, 446,   32764, 532,   32762, 629,   32760, 739,   32757, 862,   32753, 1000,  32748, 1153,  32741, 1323,
    32733, 1511,  32723, 1716,  32710, 1941,  32695, 2186,  32676, 2451,  32653, 2738,  32626, 3047,  32593, 3378,
    32555, 3733,  32509, 4110,  32456, 4511,  32394, 4935,  32323, 5383,  32241, 5854,  32147, 6348,  32041, 6864,
    31921, 7403,  31786, 7963,  31635, 8543,  31467, 9143,  31281, 9761,  31075, 10396, 30850, 11047, 30603, 11712,
    30335, 12390, 30044, 13080, 29730, 13778, 29393, 14485, 29031, 15197, 28645, 15913, 28234, 16630, 27799, 17348,
    27340, 18064, 26856, 18775, 26349, 19480, 25818, 20178, 25266, 20866, 24692, 21542, 24098, 22205, 23484, 22853,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow256[] = {
    32767, 1,     32767, 2,     32767, 3,     32767, 5,     32767, 7,     32767, 9,     32767, 11,    32767, 14,
    32767, 18,    32767, 22,    32767, 27,    32767, 32,    32767, 38,    32767, 45,    32767, 53,    32767, 61,
    32767, 71,    32767, 82,    32767, 93,    32767, 106,   32767, 121,   32767, 136,   32767, 153,   32767, 172,
    32767, 192,   32767, 215,   32767, 239,   32767, 265,   32767, 293,   32766, 323,   32766, 355,   32766, 390,
    32765, 427,   32765, 467,   32764, 510,   32763, 556,   32762, 604,   32761, 656,   32760, 711,   32759, 769,
    32757, 831,   32756, 896,   32754, 965,   32752, 1038,  32749, 1115,  32746, 1195,  32743, 1280,  32739, 1370,
    32735, 1463,  32731, 1562,  32726, 1664,  32720, 1772,  32714, 1884,  32707, 2002,  32699, 2124,  32691, 2252,
    32681, 2385,  32671, 2523,  32659, 2666,  32647, 2815,  32633, 2969,  32618, 3130,  32602, 3295,  32584, 3467,
    32565, 3644,  32544, 3827,  32521, 4015,  32496, 4210,  32470, 4410,  32441, 4617,  32410, 4829,  32377, 5047,
    32341, 5271,  32303, 5500,  32262, 5736,  32218, 5977,  32171, 6224,  32122, 6477,  32068, 6735,  32012, 6999,
    31952, 7268,  31888, 7543,  31821, 7823,  31749, 8108,  31674, 8398,  31594, 8693,  31510, 8993,  31421, 9297,
    31328, 9606,  31230, 9920,  31128, 10237, 31020, 10559, 30907, 10884, 30790, 11214, 30666, 11546, 30538, 11882,
    30404, 12221, 30264, 12563, 30119, 12908, 29967, 13255, 29810, 13604, 29648, 13956, 29479, 14309, 29304, 14664,
    29123, 15020, 28936, 15377, 28743, 15735, 28544, 16093, 28339, 16452, 28127, 16811, 27910, 17170, 27686, 17528,
    27456, 17886, 27220, 18243, 26979, 18598, 26731, 18953, 26477, 19305, 26218, 19656, 25953, 20005, 25682, 20351,
    25406, 20695, 25124, 21036, 24837, 21374, 24545, 21709, 24248, 22041, 23946, 22368, 23639, 22692, 23328, 23012,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow768[] = {
    32767, 11,    32767, 16,    32767, 21,    32767, 26,    32767, 30,    32767, 34,    32767, 39,    32767, 43,
    32767, 48,    32767, 53,    32767, 57,    32767, 62,    32767, 67,    32767, 73,    32767, 78,    32767, 84,
    32767, 89,    32767, 95,    32767, 101,   32767, 107,   32767, 113,   32767, 120,   32767, 126,   32767, 133,
    32767, 140,   32767, 147,   32767, 155,   32767, 162,   32767, 170,   32767, 178,   32767, 186,   32767, 195,
    32767, 203,   32767, 212,   32767, 221,   32767, 230,   32767, 240,   32767, 249,   32767, 259,   32767, 269,
    32767, 279,   32767, 290,   32767, 301,   32767, 312,   32766, 323,   32766, 334,   32766, 346,   32766, 358,
    32766, 370,   32766, 383,   32766, 395,   32765, 408,   32765, 422,   32765, 435,   32765, 449,   32765, 463,
    32765, 477,   32764, 492,   32764, 507,   32764, 522,   32764, 537,   32763, 553,   32763, 569,   32763, 585,
    32762, 602,   32762, 619,   32762, 636,   32761, 653,   32761, 671,   32761, 689,   32760, 707,   32760, 726,
    32760, 745,   32759, 764,   32759, 784,   32758, 804,   32758, 824,   32757, 845,   32757, 866,   32756, 887,
    32755, 909,   32755, 931,   32754, 953,   32753, 975,   32753, 998,   32752, 1022,  32751, 1045,  32751, 1069,
    32750, 1094,  32749, 1118,  32748, 1143,  32747, 1169,  32746, 1195,  32745, 1221,  32744, 1247,  32743, 1274,
    32742, 1301,  32741, 1329,  32740, 1357,  32739, 1385,  32737, 1414,  32736, 1443,  32735, 1473,  32734, 1503,
    32732, 1533,  32731, 1564,  32729, 1595,  32728, 1626,  32726, 1658,  32724, 1690,  32723, 1723,  32721, 1756,
    32719, 1789,  32717, 1823,  32715, 1857,  32713, 1892,  32711, 1927,  32709, 1963,  32707, 1999,  32705, 2035,
    32702, 2072,  32700, 2109,  32698, 2146,  32695, 2184,  32693, 2223,  32690, 2262,  32687, 2301,  32684, 2341,
    32681, 2381,  32678, 2421,  32675, 2462,  32672, 2504,  32669, 2545,  32666, 2588,  32662, 2630,  32659, 2673,
    32655, 2717,  32651, 2761,  32648, 2806,  32644, 2850,  32640, 2896,  32636, 2942,  32632, 2988,  32627, 3034,
    32623, 3081,  32618, 3129,  32614, 3177,  32609, 3225,  32604, 3274,  32599, 3324,  32594, 3373,  32589, 3424,
    32583, 3474,  32578, 3525,  32572, 3577,  32566, 3629,  32561, 3681,  32555, 3734,  32548, 3788,  32542, 3842,
    32536, 3896,  32529, 3951,  32522, 4006,  32515, 4061,  32508, 4117,  32501, 4174,  32494, 4231,  32486, 4288,
    32478, 4346,  32471, 4404,  32463, 4463,  32454, 4522,  32446, 4582,  32438, 4642,  32429, 4703,  32420, 4764,
    32411, 4825,  32402, 4887,  32392, 4949,  32382, 5012,  32373, 5075,  32363, 5139,  32352, 5203,  32342, 5268,
    32331, 5333,  32320, 5398,  32309, 5464,  32298, 5530,  32286, 5597,  32275, 5664,  32263, 5732,  32251, 5800,
    32238, 5868,  32226, 5937,  32213, 6006,  32200, 6076,  32186, 6146,  32173, 6217,  32159, 6288,  32145, 6359,
    32131, 6431,  32116, 6503,  32101, 6576,  32086, 6649,  32071, 6723,  32055, 6797,  32040, 6871,  32023, 6946,
    32007, 7021,  31990, 7096,  31973, 7172,  31956, 7248,  31939, 7325,  31921, 7402,  31903, 7480,  31885, 7558,
    31866, 7636,  31847, 7714,  31828, 7793,  31808, 7873,  31788, 7953,  31768, 8033,  31748, 8113,  31727, 8194,
    31706, 8275,  31684, 8357,  31663, 8439,  31641, 8521,  31618, 8604,  31596, 8687,  31572, 8770,  31549, 8854,
    31525, 8938,  31501, 9023,  31477, 9107,  31452, 9192,  31427, 9278,  31402, 9363,  31376, 9449,  31350, 9536,
    31323, 9622,  31296, 9709,  31269, 9797,  31242, 9884,  31214, 9972,  31185, 10060, 31157, 10149, 31128, 10237,
    31098, 10327, 31068, 10416, 31038, 10505, 31008, 10595, 30977, 10686, 30945, 10776, 30914, 10867, 30882, 10958,
    30849, 11049, 30816, 11140, 30783, 11232, 30749, 11324, 30715, 11416, 30681, 11508, 30646, 11601, 30610, 11694,
    30575, 11787, 30538, 11880, 30502, 11974, 30465, 12068, 30428, 12162, 30390, 12256, 30352, 12350, 30313, 12445,
    30274, 12539, 30234, 12634, 30194, 12729, 30154, 12825, 30113, 12920, 30072, 13016, 30030, 13112, 29988, 13207,
    29946, 13304, 29903, 13400, 29860, 13496, 29816, 13593, 29772, 13689, 29727, 13786, 29682, 13883, 29636, 13980,
    29590, 14077, 29544, 14174, 29497, 14272, 29449, 14369, 29402, 14467, 29353, 14564, 29305, 14662, 29255, 14760,
    29206, 14858, 29156, 14956, 29105, 15054, 29054, 15152, 29003, 15250, 28951, 15348, 28899, 15447, 28846, 15545,
    28793, 15643, 28739, 15742, 28685, 15840, 28630, 15939, 28575, 16037, 28520, 16136, 28464, 16234, 28408, 16333,
    28351, 16431, 28293, 16529, 28236, 16628, 28177, 16726, 28119, 16825, 28060, 16923, 28000, 17022, 27940, 17120,
    27880, 17218, 27819, 17317, 27757, 17415, 27695, 17513, 27633, 17611, 27570, 17709, 27507, 17807, 27444, 17905,
    27380, 18003, 27315, 18101, 27250, 18198, 27185, 18296, 27119, 18393, 27053, 18491, 26986, 18588, 26919, 18685,
    26851, 18782, 26783, 18879, 26715, 18976, 26646, 19072, 26576, 19169, 26506, 19265, 26436, 19361, 26366, 19458,
    26295, 19553, 26223, 19649, 26151, 19745, 26079, 19840, 26006, 19936, 25933, 20031, 25859, 20126, 25785, 20220,
    25711, 20315, 25636, 20409, 25561, 20503, 25485, 20597, 25409, 20691, 25333, 20784, 25256, 20878, 25179, 20971,
    25101, 21064, 25023, 21156, 24945, 21249, 24866, 21341, 24787, 21433, 24707, 21524, 24627, 21616, 24547, 21707,
    24466, 21798, 24385, 21888, 24304, 21979, 24222, 22069, 24140, 22159, 24057, 22248, 23975, 22337, 23891, 22426,
    23808, 22515, 23724, 22603, 23640, 22691, 23555, 22779, 23470, 22867, 23385, 22954, 23299, 23041, 23214, 23127,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow960[] = {
    32767, 10,    32767, 15,    32767, 19,    32767, 22,    32767, 26,    32767, 29,    32767, 33,    32767, 37,
    32767, 40,    32767, 44,    32767, 47,    32767, 51,    32767, 55,    32767, 59,    32767, 63,    32767, 67,
    32767, 71,    32767, 75,    32767, 80,    32767, 84,    32767, 89,    32767, 93,    32767, 98,    32767, 103,
    32767, 108,   32767, 113,   32767, 118,   32767, 123,   32767, 128,   32767, 134,   32767, 140,   32767, 145,
    32767, 151,   32767, 157,   32767, 163,   32767, 169,   32767, 176,   32767, 182,   32767, 189,   32767, 195,
    32767, 202,   32767, 209,   32767, 216,   32767, 224,   32767, 231,   32767, 239,   32767, 246,   32767, 254,
    32767, 262,   32767, 270,   32767, 278,   32767, 287,   32767, 295,   32767, 304,   32767, 313,   32766, 322,
    32766, 331,   32766, 340,   32766, 350,   32766, 359,   32766, 369,   32766, 379,   32766, 389,   32766, 399,
    32765, 410,   32765, 420,   32765, 431,   32765, 442,   32765, 453,   32765, 464,   32765, 476,   32764, 487,
    32764, 499,   32764, 511,   32764, 523,   32764, 536,   32763, 548,   32763, 561,   32763, 574,   32763, 587,
    32763, 600,   32762, 613,   32762, 627,   32762, 641,   32761, 655,   32761, 669,   32761, 684,   32761, 698,
    32760, 713,   32760, 728,   32760, 743,   32759, 759,   32759, 774,   32758, 790,   32758, 806,   32758, 822,
    32757, 839,   32757, 855,   32756, 872,   32756, 889,   32755, 906,   32755, 924,   32754, 942,   32754, 960,
    32753, 978,   32753, 996,   32752, 1015,  32752, 1033,  32751, 1052,  32750, 1072,  32750, 1091,  32749, 1111,
    32748, 1131,  32748, 1151,  32747, 1171,  32746, 1192,  32746, 1213,  32745, 1234,  32744, 1255,  32743, 1277,
    32742, 1299,  32741, 1321,  32740, 1343,  32740, 1365,  32739, 1388,  32738, 1411,  32737, 1434,  32736, 1458,
    32734, 1482,  32733, 1506,  32732, 1530,  32731, 1554,  32730, 1579,  32729, 1604,  32727, 1629,  32726, 1655,
    32725, 1680,  32724, 1706,  32722, 1733,  32721, 1759,  32719, 1786,  32718, 1813,  32716, 1840,  32715, 1868,
    32713, 1896,  32711, 1924,  32710, 1952,  32708, 1981,  32706, 2009,  32705, 2039,  32703, 2068,  32701, 2098,
    32699, 2128,  32697, 2158,  32695, 2188,  32693, 2219,  32691, 2250,  32689, 2281,  32686, 2313,  32684, 2345,
    32682, 2377,  32679, 2409,  32677, 2442,  32674, 2475,  32672, 2508,  32669, 2541,  32667, 2575,  32664, 2609,
    32661, 2643,  32658, 2678,  32656, 2713,  32653, 2748,  32650, 2783,  32647, 2819,  32643, 2855,  32640, 2891,
    32637, 2928,  32634, 2965,  32630, 3002,  32627, 3039,  32623, 3077,  32620, 3115,  32616, 3153,  32612, 3192,
    32608, 3230,  32604, 3269,  32601, 3309,  32596, 3348,  32592, 3388,  32588, 3429,  32584, 3469,  32579, 3510,
    32575, 3551,  32570, 3593,  32566, 3634,  32561, 3676,  32556, 3718,  32551, 3761,  32546, 3804,  32541, 3847,
    32536, 3890,  32531, 3934,  32526, 3978,  32520, 4022,  32515, 4067,  32509, 4112,  32503, 4157,  32497, 4202,
    32491, 4248,  32485, 4294,  32479, 4340,  32473, 4387,  32467, 4434,  32460, 4481,  32454, 4528,  32447, 4576,
    32440, 4624,  32433, 4672,  32426, 4721,  32419, 4770,  32412, 4819,  32404, 4868,  32397, 4918,  32389, 4968,
    32381, 5019,  32374, 5069,  32366, 5120,  32357, 5171,  32349, 5222,  32341, 5274,  32332, 5326,  32324, 5378,
    32315, 5431,  32306, 5484,  32297, 5537,  32288, 5590,  32278, 5644,  32269, 5698,  32259, 5752,  32249, 5807,
    32240, 5861,  32229, 5916,  32219, 5972,  32209, 6027,  32198, 6083,  32188, 6139,  32177, 6196,  32166, 6252,
    32155, 6309,  32144, 6366,  32132, 6424,  32121, 6482,  32109, 6540,  32097, 6598,  32085, 6657,  32073, 6715,
    32060, 6774,  32048, 6834,  32035, 6893,  32022, 6953,  32009, 7013,  31995, 7074,  31982, 7134,  31968, 7195,
    31955, 7256,  31941, 7317,  31926, 7379,  31912, 7441,  31897, 7503,  31883, 7565,  31868, 7628,  31853, 7691,
    31837, 7754,  31822, 7817,  31806, 7881,  31790, 7945,  31774, 8009,  31758, 8073,  31741, 8138,  31725, 8202,
    31708, 8267,  31691, 8333,  31674, 8398,  31656, 8464,  31638, 8530,  31620, 8596,  31602, 8662,  31584, 8729,
    31566, 8795,  31547, 8862,  31528, 8930,  31509, 8997,  31489, 9065,  31470, 9133,  31450, 9201,  31430, 9269,
    31409, 9338,  31389, 9406,  31368, 9475,  31347, 9544,  31326, 9614,  31305, 9683,  31283, 9753,  31261, 9823,
    31239, 9893,  31217, 9963,  31194, 10034, 31171, 10104, 31148, 10175, 31125, 10246, 31101, 10318, 31077, 10389,
    31053, 10461, 31029, 10532, 31005, 10604, 30980, 10677, 30955, 10749, 30930, 10821, 30904, 10894, 30878, 10967,
    30852, 11040, 30826, 11113, 30800, 11186, 30773, 11259, 30746, 11333, 30718, 11407, 30691, 11481, 30663, 11555,
    30635, 11629, 30607, 11703, 30578, 11778, 30549, 11852, 30520, 11927, 30491, 12002, 30461, 12077, 30431, 12152,
    30401, 12228, 30371, 12303, 30340, 12379, 30309, 12454, 30278, 12530, 30246, 12606, 30214, 12682, 30182, 12758,
    30150, 12834, 30117, 12911, 30085, 12987, 30051, 13064, 30018, 13140, 29984, 13217, 29950, 13294, 29916, 13371,
    29881, 13448, 29846, 13525, 29811, 13602, 29776, 13680, 29740, 13757, 29704, 13835, 29668, 13912, 29632, 13990,
    29595, 14067, 29558, 14145, 29520, 14223, 29483, 14301, 29445, 14379, 29406, 14457, 29368, 14535, 29329, 14613,
    29290, 14692, 29251, 14770, 29211, 14848, 29171, 14927, 29131, 15005, 29090, 15083, 29049, 15162, 29008, 15240,
    28967, 15319, 28925, 15398, 28883, 15476, 28841, 15555, 28798, 15634, 28755, 15712, 28712, 15791, 28669, 15870,
    28625, 15948, 28581, 16027, 28537, 16106, 28492, 16185, 28447, 16264, 28402, 16342, 28356, 16421, 28311, 16500,
    28265, 16579, 28218, 16658, 28172, 16736, 28125, 16815, 28077, 16894, 28030, 16972, 27982, 17051, 27934, 17130,
    27886, 17208, 27837, 17287, 27788, 17366, 27739, 17444, 27689, 17523, 27639, 17601, 27589, 17680, 27539, 17758,
    27488, 17836, 27437, 17915, 27386, 17993, 27334, 18071, 27283, 18149, 27231, 18227, 27178, 18306, 27125, 18383,
    27072, 18461, 27019, 18539, 26966, 18617, 26912, 18695, 26858, 18772, 26803, 18850, 26749, 18927, 26694, 19005,
    26639, 19082, 26583, 19159, 26527, 19236, 26471, 19313, 26415, 19390, 26359, 19467, 26302, 19544, 26245, 19621,
    26187, 19697, 26130, 19773, 26072, 19850, 26013, 19926, 25955, 20002, 25896, 20078, 25837, 20154, 25778, 20230,
    25718, 20305, 25659, 20381, 25599, 20456, 25538, 20531, 25478, 20607, 25417, 20682, 25356, 20756, 25294, 20831,
    25233, 20906, 25171, 20980, 25109, 21054, 25047, 21128, 24984, 21202, 24921, 21276, 24858, 21350, 24795, 21423,
    24731, 21497, 24667, 21570, 24603, 21643, 24539, 21716, 24474, 21789, 24410, 21861, 24345, 21934, 24279, 22006,
    24214, 22078, 24148, 22150, 24082, 22221, 24016, 22293, 23950, 22364, 23883, 22435, 23816, 22506, 23749, 22577,
    23682, 22647, 23614, 22718, 23547, 22788, 23479, 22858, 23411, 22928, 23342, 22997, 23274, 23067, 23205, 23136,
};

RAM_ALIGN
LNK_SECTION_CONSTDATA
const FIXP_WTP KBDWindow1024[] = {
    32767, 10,    32767, 14,    32767, 18,    32767, 21,    32767, 25,    32767, 28,    32767, 32,    32767, 35,
    32767, 38,    32767, 42,    32767, 45,    32767, 48,    32767, 52,    32767, 56,    32767, 59,    32767, 63,
    32767, 67,    32767, 71,    32767, 75,    32767, 79,    32767, 83,    32767, 87,    32767, 91,    32767, 96,
    32767, 100,   32767, 105,   32767, 109,   32767, 114,   32767, 119,   32767, 124,   32767, 129,   32767, 134,
    32767, 139,   32767, 145,   32767, 150,   32767, 156,   32767, 161,   32767, 167,   32767, 173,   32767, 179,
    32767, 185,   32767, 191,   32767, 198,   32767, 204,   32767, 211,   32767, 218,   32767, 224,   32767, 231,
    32767, 238,   32767, 246,   32767, 253,   32767, 260,   32767, 268,   32767, 275,   32767, 283,   32767, 291,
    32767, 299,   32767, 307,   32766, 316,   32766, 324,   32766, 333,   32766, 342,   32766, 351,   32766, 360,
    32766, 369,   32766, 378,   32766, 387,   32766, 397,   32765, 407,   32765, 417,   32765, 427,   32765, 437,
    32765, 447,   32765, 458,   32765, 468,   32764, 479,   32764, 490,   32764, 501,   32764, 512,   32764, 524,
    32764, 535,   32763, 547,   32763, 559,   32763, 571,   32763, 583,   32763, 595,   32762, 608,   32762, 621,
    32762, 634,   32762, 647,   32761, 660,   32761, 673,   32761, 687,   32761, 700,   32760, 714,   32760, 728,
    32760, 743,   32759, 757,   32759, 772,   32759, 786,   32758, 801,   32758, 817,   32757, 832,   32757, 847,
    32757, 863,   32756, 879,   32756, 895,   32755, 911,   32755, 928,   32754, 944,   32754, 961,   32753, 978,
    32753, 995,   32752, 1013,  32752, 1030,  32751, 1048,  32751, 1066,  32750, 1084,  32749, 1103,  32749, 1121,
    32748, 1140,  32747, 1159,  32747, 1178,  32746, 1198,  32745, 1217,  32745, 1237,  32744, 1257,  32743, 1277,
    32742, 1298,  32741, 1319,  32741, 1339,  32740, 1360,  32739, 1382,  32738, 1403,  32737, 1425,  32736, 1447,
    32735, 1469,  32734, 1491,  32733, 1514,  32732, 1537,  32731, 1560,  32730, 1583,  32729, 1606,  32727, 1630,
    32726, 1654,  32725, 1678,  32724, 1702,  32722, 1727,  32721, 1752,  32720, 1777,  32718, 1802,  32717, 1827,
    32716, 1853,  32714, 1879,  32713, 1905,  32711, 1932,  32709, 1958,  32708, 1985,  32706, 2012,  32704, 2039,
    32703, 2067,  32701, 2095,  32699, 2123,  32697, 2151,  32695, 2180,  32694, 2208,  32692, 2237,  32690, 2266,
    32687, 2296,  32685, 2326,  32683, 2356,  32681, 2386,  32679, 2416,  32677, 2447,  32674, 2478,  32672, 2509,
    32669, 2540,  32667, 2572,  32664, 2604,  32662, 2636,  32659, 2668,  32657, 2701,  32654, 2734,  32651, 2767,
    32648, 2800,  32645, 2834,  32642, 2867,  32639, 2901,  32636, 2936,  32633, 2970,  32630, 3005,  32627, 3040,
    32623, 3076,  32620, 3111,  32617, 3147,  32613, 3183,  32609, 3219,  32606, 3256,  32602, 3293,  32598, 3330,
    32595, 3367,  32591, 3405,  32587, 3443,  32583, 3481,  32578, 3519,  32574, 3558,  32570, 3596,  32566, 3636,
    32561, 3675,  32557, 3715,  32552, 3754,  32548, 3794,  32543, 3835,  32538, 3875,  32533, 3916,  32528, 3957,
    32523, 3999,  32518, 4040,  32513, 4082,  32507, 4124,  32502, 4167,  32496, 4209,  32491, 4252,  32485, 4296,
    32479, 4339,  32474, 4383,  32468, 4426,  32462, 4471,  32455, 4515,  32449, 4560,  32443, 4605,  32436, 4650,
    32430, 4695,  32423, 4741,  32416, 4787,  32410, 4833,  32403, 4879,  32396, 4926,  32388, 4973,  32381, 5020,
    32374, 5067,  32366, 5115,  32359, 5163,  32351, 5211,  32343, 5260,  32335, 5308,  32327, 5357,  32319, 5406,
    32311, 5456,  32302, 5505,  32294, 5555,  32285, 5605,  32276, 5656,  32267, 5706,  32258, 5757,  32249, 5808,
    32240, 5860,  32230, 5911,  32221, 5963,  32211, 6015,  32201, 6067,  32191, 6120,  32181, 6173,  32171, 6226,
    32161, 6279,  32150, 6332,  32140, 6386,  32129, 6440,  32118, 6494,  32107, 6549,  32096, 6603,  32084, 6658,
    32073, 6713,  32061, 6769,  32049, 6824,  32038, 6880,  32025, 6936,  32013, 6992,  32001, 7049,  31988, 7106,
    31976, 7163,  31963, 7220,  31950, 7277,  31937, 7335,  31923, 7393,  31910, 7451,  31896, 7509,  31882, 7567,
    31868, 7626,  31854, 7685,  31840, 7744,  31825, 7803,  31811, 7863,  31796, 7923,  31781, 7983,  31766, 8043,
    31750, 8103,  31735, 8164,  31719, 8225,  31703, 8286,  31687, 8347,  31671, 8408,  31654, 8470,  31638, 8532,
    31621, 8594,  31604, 8656,  31587, 8718,  31570, 8781,  31552, 8844,  31534, 8907,  31516, 8970,  31498, 9033,
    31480, 9097,  31462, 9160,  31443, 9224,  31424, 9288,  31405, 9353,  31386, 9417,  31366, 9482,  31347, 9547,
    31327, 9612,  31307, 9677,  31286, 9742,  31266, 9808,  31245, 9873,  31224, 9939,  31203, 10005, 31182, 10071,
    31160, 10138, 31139, 10204, 31117, 10271, 31095, 10338, 31072, 10405, 31050, 10472, 31027, 10539, 31004, 10607,
    30981, 10674, 30957, 10742, 30934, 10810, 30910, 10878, 30886, 10946, 30861, 11015, 30837, 11083, 30812, 11152,
    30787, 11220, 30762, 11289, 30736, 11358, 30711, 11428, 30685, 11497, 30659, 11566, 30632, 11636, 30606, 11706,
    30579, 11775, 30552, 11845, 30525, 11915, 30497, 11986, 30470, 12056, 30442, 12126, 30413, 12197, 30385, 12268,
    30356, 12338, 30327, 12409, 30298, 12480, 30269, 12551, 30239, 12622, 30209, 12694, 30179, 12765, 30149, 12837,
    30118, 12908, 30088, 12980, 30057, 13052, 30025, 13124, 29994, 13196, 29962, 13268, 29930, 13340, 29898, 13412,
    29865, 13484, 29832, 13557, 29799, 13629, 29766, 13701, 29732, 13774, 29699, 13847, 29665, 13919, 29630, 13992,
    29596, 14065, 29561, 14138, 29526, 14211, 29491, 14284, 29455, 14357, 29420, 14430, 29384, 14503, 29347, 14577,
    29311, 14650, 29274, 14723, 29237, 14797, 29200, 14870, 29162, 14944, 29124, 15017, 29086, 15091, 29048, 15164,
    29009, 15238, 28971, 15312, 28932, 15385, 28892, 15459, 28853, 15533, 28813, 15607, 28773, 15680, 28732, 15754,
    28692, 15828, 28651, 15902, 28610, 15976, 28568, 16049, 28527, 16123, 28485, 16197, 28443, 16271, 28400, 16345,
    28358, 16419, 28315, 16493, 28272, 16566, 28228, 16640, 28185, 16714, 28141, 16788, 28097, 16862, 28052, 16936,
    28008, 17009, 27963, 17083, 27917, 17157, 27872, 17231, 27826, 17304, 27780, 17378, 27734, 17452, 27688, 17525,
    27641, 17599, 27594, 17672, 27547, 17746, 27499, 17819, 27452, 17893, 27404, 17966, 27355, 18039, 27307, 18113,
    27258, 18186, 27209, 18259, 27160, 18332, 27111, 18405, 27061, 18478, 27011, 18551, 26961, 18624, 26910, 18697,
    26859, 18770, 26809, 18843, 26757, 18915, 26706, 18988, 26654, 19060, 26602, 19133, 26550, 19205, 26498, 19277,
    26445, 19349, 26392, 19422, 26339, 19494, 26286, 19565, 26232, 19637, 26178, 19709, 26124, 19781, 26070, 19852,
    26015, 19924, 25960, 19995, 25905, 20066, 25850, 20137, 25795, 20208, 25739, 20279, 25683, 20350, 25627, 20421,
    25570, 20491, 25514, 20562, 25457, 20632, 25400, 20703, 25342, 20773, 25285, 20843, 25227, 20913, 25169, 20982,
    25111, 21052, 25052, 21121, 24994, 21191, 24935, 21260, 24876, 21329, 24817, 21398, 24757, 21467, 24697, 21536,
    24637, 21604, 24577, 21673, 24517, 21741, 24456, 21809, 24395, 21877, 24334, 21945, 24273, 22013, 24212, 22080,
    24150, 22147, 24088, 22215, 24026, 22282, 23964, 22349, 23902, 22415, 23839, 22482, 23776, 22548, 23713, 22614,
    23650, 22680, 23587, 22746, 23523, 22812, 23460, 22878, 23396, 22943, 23332, 23008, 23267, 23073, 23203, 23138,
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
    32767, 0,     32758, 804,   32729, 1608,  32679, 2411,  32610, 3212,  32522, 4011,  32413, 4808,  32286, 5602,
    32138, 6393,  31972, 7180,  31786, 7962,  31581, 8740,  31357, 9512,  31114, 10279, 30853, 11039, 30572, 11793,
    30274, 12540, 29957, 13279, 29622, 14010, 29269, 14733, 28899, 15447, 28511, 16151, 28106, 16846, 27684, 17531,
    27246, 18205, 26791, 18868, 26320, 19520, 25833, 20160, 25330, 20788, 24812, 21403, 24279, 22006, 23732, 22595,
    23170, 23170, 22595, 23732, 22006, 24279, 21403, 24812, 20788, 25330, 20160, 25833, 19520, 26320, 18868, 26791,
    18205, 27246, 17531, 27684, 16846, 28106, 16151, 28511, 15447, 28899, 14733, 29269, 14010, 29622, 13279, 29957,
    12540, 30274, 11793, 30572, 11039, 30853, 10279, 31114, 9512,  31357, 8740,  31581, 7962,  31786, 7180,  31972,
    6393,  32138, 5602,  32286, 4808,  32413, 4011,  32522, 3212,  32610, 2411,  32679, 1608,  32729, 804,   32758};

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
