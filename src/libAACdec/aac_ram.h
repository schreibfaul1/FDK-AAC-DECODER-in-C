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

/**************************** AAC decoder library ******************************

   Author(s):   Josef Hoepfl

   Description:

*******************************************************************************/

#ifndef AAC_RAM_H
#define AAC_RAM_H

#include "../libFDK/common_fix.h"

#include "aacdecoder.h"
#include "channel.h"

#include "../libArithCoding/ac_arith_coder.h"

#include "aacdec_hcr_types.h"
#include "aacdec_hcr.h"

/* End of formal fix.h */

#define MAX_SYNCHS 10
#define SAMPL_FREQS 12

AAC_DECODER_INSTANCE *GetAacDecoder(int n = 0);
void FreeAacDecoder(AAC_DECODER_INSTANCE **p);
UINT GetRequiredMemAacDecoder(void);

CDrcInfo *GetDrcInfo(int n = 0);
void FreeDrcInfo(CDrcInfo **p);
UINT GetRequiredMemDrcInfo(void);

CAacDecoderStaticChannelInfo *GetAacDecoderStaticChannelInfo(int n = 0);
void FreeAacDecoderStaticChannelInfo(CAacDecoderStaticChannelInfo **p);
UINT GetRequiredMemAacDecoderStaticChannelInfo(void);

CAacDecoderChannelInfo *GetAacDecoderChannelInfo(int n = 0);
void FreeAacDecoderChannelInfo(CAacDecoderChannelInfo **p);
UINT GetRequiredMemAacDecoderChannelInfo(void);

FIXP_DBL *GetOverlapBuffer(int n = 0);
void FreeOverlapBuffer(FIXP_DBL **p);
UINT GetRequiredMemOverlapBuffer(void);

CpePersistentData *GetCpePersistentData(int n = 0);
void FreeCpePersistentData(CpePersistentData **p);
UINT GetRequiredMemCpePersistentData(void);

CCplxPredictionData *GetCplxPredictionData(int n = 0);
void FreeCplxPredictionData(CCplxPredictionData **p);
UINT GetRequiredMemCplxPredictionData(void);

FIXP_DBL *GetSpectralCoeffs(int n = 0);
void FreeSpectralCoeffs(FIXP_DBL **p);
UINT GetRequiredMemSpectralCoeffs(void);

SHORT *GetSpecScale(int n = 0);
void FreeSpecScale(SHORT **p);
UINT GetRequiredMemSpecScale(void);

INT_PCM *GetTimeDataFlush(int n = 0);
void FreeTimeDataFlush(INT_PCM **p);
UINT GetRequiredMemTimeDataFlush(void);

CWorkBufferCore1 *GetWorkBufferCore1(int n = 0);
void FreeWorkBufferCore1(CWorkBufferCore1 **p);
UINT GetRequiredMemWorkBufferCore1(void);

FIXP_DBL *GetWorkBufferCore2(int n = 0);
void FreeWorkBufferCore2(FIXP_DBL **p);
UINT GetRequiredMemWorkBufferCore2(void);

FIXP_DBL *GetWorkBufferCore5(int n = 0);
void FreeWorkBufferCore5(FIXP_DBL **p);
UINT GetRequiredMemWorkBufferCore5(void);

SCHAR *GetWorkBufferCore6(int n = 0);
void FreeWorkBufferCore6(SCHAR **p);
UINT GetRequiredMemWorkBufferCore6(void);

CArcoData *GetArcoData(int n = 0);
void FreeArcoData(CArcoData **p);
UINT GetRequiredMemArcoData(void);

#endif /* #ifndef AAC_RAM_H */