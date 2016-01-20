/** msadpcm.h
 *
     (C) Copyright Microsoft Corp. 1991, 1992.  All rights reserved.

     You have a royalty-free right to use, modify, reproduce and 
     distribute the Sample Files (and/or any modified version) in 
     any way you find useful, provided that you agree that 
     Microsoft has no warranty obligations or liability for any 
     Sample Application Files which are modified. 
	 
     If you did not get this from Microsoft Sources, then it may not be the
     most current version.  This sample code in particular will be updated
     and include more documentation.  

     Sources are:
     	The MM Sys File Transfer BBS: The phone number is 206 936-4082.
	CompuServe: WINSDK forum, MDK section.
	Anonymous FTP from ftp.uu.net vendors\microsoft\multimedia
 *
 **/

#ifndef _MSADPCM_H_
#define _MSADPCM_H_

#define MSADPCMAPI  FAR PASCAL
//
//  function prototypes from MSADPCM.C
//
DWORD MSADPCMAPI msadpcmDecode4Bit(LPADPCMWAVEFORMAT lpwfADPCM, HPSTR hpSrc, LPPCMWAVEFORMAT lpwfPCM, HPSTR hpDst, DWORD dwSrcLen);
DWORD MSADPCMAPI msadpcmEncode4Bit(LPPCMWAVEFORMAT lpwfPCM, HPSTR hpSrc, LPADPCMWAVEFORMAT lpwfADPCM, HPSTR hpDst, DWORD dwSrcLen);
BOOL  MSADPCMAPI msadpcmIsValidFormat(LPWAVEFORMATEX lpwfx);
BOOL  MSADPCMAPI msadpcmBuildFormatHeader(LPWAVEFORMATEX lpwfxSrc, LPWAVEFORMATEX lpwfxDst);

#endif

/** EOF: msadpcm.h **/
