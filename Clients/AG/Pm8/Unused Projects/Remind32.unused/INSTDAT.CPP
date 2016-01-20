/*
//	PrintMaster version of INSTDAT.C
//
// $Workfile: INSTDAT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright (C) 1992-1993 by MicroLogic Software, Inc. All Rights Reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/INSTDAT.CPP $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
// 2     4/14/98 3:14p Dennis
// Removed Config hack.
// 
//    Rev 1.0   14 Aug 1997 15:21:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:50   Fred
// Initial revision.
// 
//    Rev 1.3   04 Mar 1997 17:03:36   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.2   30 Aug 1996 19:33:42   Fred
// Works with UNC and long file names
// 
//    Rev 1.1   15 May 1996 08:46:06   Jay
// From PMW2
// 
//    Rev 2.3   09 May 1996 17:22:26   JAY
// Support for trial version run counts.
// 
//    Rev 2.2   29 Jan 1996 16:53:08   JAY
// Variable configuration in INSTALL and PMW.
// 
//    Rev 2.1   08 Feb 1995 13:37:34   JAY
// Reverted. New series.
// 
//    Rev 1.3   04 Aug 1994 08:30:08   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   15 Jun 1994 12:55:10   FRED
// Changed some BYTE's to chars.
// 
// 
//    Rev 1.1   12 May 1994 07:35:28   JAY
// Changed some LPSTRs to LPCSTRs.
# 
#    Rev 1.0   16 Dec 1993 16:28:24   JAY
#  
// 
//    Rev 1.2   13 Jul 1993 15:44:34   JAY
// Fixed a bug in the delay loops.
// 
//    Rev 1.1   13 Jul 1993 14:27:00   JAY
// Support for writing installtion info.
// Support for License Access Codes.
// 
//    Rev 1.0   28 Apr 1993 13:08:50   MACDUFF
// Initial revision.
// 
//    Rev 1.10   22 Mar 1993 22:30:40   FRED
// Moved most strings to string resource.
// 
// 
//    Rev 1.9   14 Dec 1992 15:35:26   FRED
// Removed GetNetworkUserName().
// 
// 
//    Rev 1.8   14 Dec 1992 13:36:42   FRED
// First attempt at single source code for
// TrueEffects and MoreFonts.
// 
// 
//    Rev 1.7   14 Dec 1992 09:28:24   JAY
// Added FriendlySerialNumber() for converting 1/I 0/O confused pairs.
//
// 
//    Rev 1.6   26 Nov 1992 08:54:32   FRED
// TrueEffects version.
// 
// 
//    Rev 1.5   02 Oct 1992 14:29:26   FRED
// The network code is disabled if a network driver
// is not listed on the "network.drv=" line in the
// [boot] section of SYSTEM.INI. Both Windows 3.0
// and 3.1 have problems with our network code if
// no network driver is present.
// 
// 
//    Rev 1.4   23 Sep 1992 17:15:54   FRED
// Changed unknown network user name from "NONE" to "Unknown".
// 
// 
//    Rev 1.3   16 Jul 1992 09:32:48   FRED
// Changed unknown network user name from "NONAME" to "NONE".
// 
// 
//    Rev 1.2   16 Jul 1992 09:16:40   FRED
// Changes for using MSC 7.00
// 
// 
//    Rev 1.1   10 Jul 1992 10:05:32   FRED
// Added bTimeStamp[4] field to INSTALLINFO structure.
// This was needed for LRC/LAC computation.
// 
// 
//    Rev 1.0   06 Jul 1992 10:46:34   FRED
// Initial revision.
*/ 


#include "stdafx.h"

#include <string.h>

#include "system.h"
#include	"instdat.h"
#include	"pmwcfg.h"

/*
// String encoding parameters.
*/

static CODEPARAMETERS near CodeParameters_2 =	// Parameters for 2 letter encoding
{
	26*26,		// Range
	0x01ff,		// Mask
	811,			// Prime
	7777			// Seed
};

static CODEPARAMETERS near CodeParameters_3 =	// Parameters for 3 letter encoding
{
	26*26*26,	// Range
	0x3fff,		// Mask
	21101,		// Prime
	7777			// Seed
};

static CHAR SerialNumber_Template[] = "AAA##-##AXN-XXXXAA";

static BOOL ValidateString(LPSTR lpszString, LPSTR lpszTemplate);
static BOOL ValidateCodeString(LPSTR lpszString, WORD Length);
static void EncodeString(LPSTR lpszString, WORD Length, LPSTR lpszCode);
static WORD CodeString(LPSTR lpszString, WORD wRange, WORD wMask, WORD wPrime, WORD wSeed);
static WORD CodeBytesToWord(LPBYTE lpData, WORD wCount);
static WORD CodeBytes(LPBYTE lpData, WORD wCount, WORD wRange, WORD wPrime, WORD wSeed, WORD wMask);
static DWORD ValueBase26(LPSTR lpszString);
static void StringBase26(DWORD dwNumber, WORD wLength, LPSTR lpszString);
static void EncodeInstallationData(LPINSTDAT lpData);
static BOOL DecodeInstallationData(LPINSTDAT lpData, BOOL wVirginOK);
static void EncryptString(LPBYTE lpData, LPBYTE lpRandomData);
static void Delay(WORD wMilliseconds);
static LPSTR l2z(LPSTR d, LPBYTE s);
static LPSTR substr(LPSTR d, LPSTR s, WORD c, WORD l);

BOOL InstallationInfo::Get(LPCSTR lpszFileName, BOOL wVirginOK)
{
	CFile file;
	BOOL wResult;

	wResult = FALSE;

	TRY
	{
	/*
	// Attempt to open the file and read in the installation record.
	*/

		if (file.Open(lpszFileName,
							 CFile::modeRead | CFile::shareDenyNone,
							 NULL))
		{
			INSTDAT Data;

			if (file.Read(&Data, sizeof(Data)) == sizeof(Data))
			{
				if (DecodeInstallationData(&Data, wVirginOK))
				{
					l2z(szSerialNumber, Data.bSerialNumber);
					l2z(szUserName, Data.bUserName);
					l2z(szCompanyName, Data.bCompanyName);
					bNetCount = Data.bNetCount;
					bTimeStamp[0] = Data.bTimeStamp[0];
					bTimeStamp[1] = Data.bTimeStamp[1];
					bTimeStamp[2] = Data.bTimeStamp[2];
					bTimeStamp[3] = Data.bTimeStamp[3];

					nInstallConfiguration = (int)Data.bInstallConfiguration;
					nCurrentConfiguration = (int)Data.bCurrentConfiguration;
					nTrialCount = (int)Data.bTrialCount;

					wResult = TRUE;

				/* Remember the name we loaded with. */

					if (m_last_load_name != NULL)
					{
						system_heap->free(m_last_load_name);
					}
					m_last_load_name = system_heap->dup_string((LPSTR)lpszFileName);
				}
			}
		}
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL

	return wResult;
}

#ifdef INSTDAT_WRITE
BOOL InstallationInfo::Put(LPCSTR lpszFileName)
{
	INSTDAT Data;
	WORD wRetries;
	BOOL wResult;
	static char * pszCopyright =
		 "PrintMaster Gold Installation. Copyright 1993 MicroLogic Software, Inc.\n\r%d User License.\n\r%c";

	wResult = FALSE;

	if (lpszFileName == NULL)
	{
		lpszFileName = m_last_load_name;
		if (lpszFileName == NULL)
		{
		/* Still NULL! Can't save. */
			return FALSE;
		}
	}

	/*
	// Build the data record to write.
	*/

	sprintf((char *)Data.bCopyright, pszCopyright, bNetCount, 0x1a);

	Data.wVersion = 0;

	strcpy((char *)Data.bSerialNumber+1, szSerialNumber);
	Data.bSerialNumber[0] = strlen((char *)Data.bSerialNumber+1);

	strcpy((char *)Data.bUserName+1, szUserName);
	Data.bUserName[0] = strlen((char *)Data.bUserName+1);

	strcpy((char *)Data.bCompanyName+1, szCompanyName);
	Data.bCompanyName[0] = strlen((char *)Data.bCompanyName+1);

	Data.bNetCount = bNetCount;

	Data.bTimeStamp[0] = bTimeStamp[0];
	Data.bTimeStamp[1] = bTimeStamp[1];
	Data.bTimeStamp[2] = bTimeStamp[2];
	Data.bTimeStamp[3] = bTimeStamp[3];

	Data.bInstallConfiguration = (BYTE)nInstallConfiguration;

	// Take the configuration and split it into the data.
   Data.bCurrentConfiguration = (BYTE)nCurrentConfiguration;
	Data.bCurrentConfiguration2 = 0;
	Data.bTrialCount = (BYTE)nTrialCount;

	EncodeInstallationData(&Data);

	/*
	// Attempt to create the file and write out the installation record.
	*/

	for (wRetries = 0; wRetries < 3; wRetries++)
	{
		CFile File;

		if (File.Open(lpszFileName, CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive))
		{
			TRY
			{
				File.Write((LPSTR)(&Data), sizeof(Data));
				wResult = TRUE;
			}
			END_TRY

			TRY
			{
				File.Close();
			}
			END_TRY

			if (wResult)
			{
				break;
			}
		}

		Delay(1000);
	}

	return (wResult);
}
#endif

BOOL ValidateSerialNumber(LPSTR lpszSerialNumber)
{
	return (ValidateString(lpszSerialNumber, SerialNumber_Template)
		  && ValidateCodeString(lpszSerialNumber, 2));
}

PRIVATE BOOL ValidateString(LPSTR lpszString, LPSTR lpszTemplate)
{
	if (strlen(lpszString) != strlen(lpszTemplate))
	{
		return (FALSE);
	}

	while (*lpszString != 0)
	{
		char c;
		char t;

		c = *(lpszString++);
		t = *(lpszTemplate++);

		switch (t)
		{
			case 'A':
			{
				if ((c < 'A') || (c > 'Z')) return (FALSE);
				break;
			}

			case '#':
			{
				if ((c < '0') || (c > '9')) return (FALSE);
				break;
			}

			case 'X':
			{
				if (((c < '0') || (c > '9')) && ((c < 'A') && (c > 'F'))) return (FALSE);
				break;
			}

			case 'N':
			{
				if (((c < '0') || (c > '9')) && ((c < 'A') && (c > 'Z'))) return (FALSE);
				break;
			}

			default:
			{
				if (c != t) return (FALSE);
				break;
			}
		}
	}

	return (TRUE);
}

#ifdef INSTDAT_WRITE
static VOID FriendlyString(LPSTR lpszString, LPSTR lpszTemplate)
{
	if (strlen(lpszString) == strlen(lpszTemplate))
	{
		while (*lpszString != 0)
		{
			char c;
			char t;

			c = *lpszString;
			t = *(lpszTemplate++);

			switch (t)
			{
				case 'A':
				{
					if (c == '0')
					{
						c = 'O';
					}
					else if (c == '1')
					{
						c = 'I';
					}
					break;
				}

				case '#':
				case 'X':
				{
					if (c == 'O')
					{
						c = '0';
					}
					else if (c == 'I')
					{
						c = '1';
					}
					break;
				}

				default:
				{
					break;
				}
			}
			*(lpszString++) = c;
		}
	}
}
	
VOID FriendlySerialNumber(LPSTR lpszSerialNumber)
{
	FriendlyString(lpszSerialNumber, SerialNumber_Template);
}
#endif

static BOOL ValidateCodeString(LPSTR lpszString, WORD Length)
{
	BOOL wResult;

	wResult = FALSE;

	if (strlen(lpszString) > Length)
	{
		char szString[100];
		char szCode[100];
		char szNewCode[100];

		/*
		// We validate the coding by reencoding the string and
		// comparing the results;
		*/

		substr(szString, lpszString, 0, strlen(lpszString)-Length);
		substr(szCode, lpszString, strlen(lpszString)-Length, Length);

		EncodeString(szString, Length, szNewCode);

		wResult = (strcmp(szCode, szNewCode) == 0);
	}

	return (wResult);
}

static void EncodeString(LPSTR lpszString, WORD Length, LPSTR lpszCode)
{
	LPCODEPARAMETERS lpCodeParameters;
	WORD wCode;

	*lpszCode = 0;

	switch (Length)
	{
		case 2:
		{
			lpCodeParameters = &CodeParameters_2;
			break;
		}

		case 3:
		{
			lpCodeParameters = &CodeParameters_3;
			break;
		}

		default:
		{
			return;
		}
	}

	wCode = CodeString(
					lpszString,
					lpCodeParameters->wRange,
					lpCodeParameters->wMask,
					lpCodeParameters->wPrime,
					lpCodeParameters->wSeed);

	StringBase26(wCode, Length, lpszCode);
}

static WORD CodeString(LPSTR lpszString, WORD wRange, WORD wMask, WORD wPrime, WORD wSeed)
{
	return (CodeBytes((LPBYTE)lpszString, strlen(lpszString), wRange, wMask, wPrime, wSeed));
}

static WORD CodeBytesToWord(LPBYTE lpData, WORD wCount)
{
	return (CodeBytes(lpData, wCount, 0xffff, 0x7fff, 1, 7777));
}

/*
// CodeBytes
//
// Given a block of data, generates a 16-bit hash value, which is intended 
// to be non-obvious so that the following operations on the string are 
// unlikely to preserve the hash value:
//
//		Incrementing one character
//		Incrementing one and decrementing its neighbor
//		Exchanging two characters
//
// Arguments:
//
//		lpData	-		pointer to data bytes to encode
//		wCount	-		number of bytes to encode
// 	wRange	-		the result will be in the range [0..wRange-1]
//		wMask		-		the upper 16 bits of MASK where MASK is the largest
//							2^N-1 such that MASK/wRange <= 65535
//		wPrime	-		an arbitrary prime number that should be
//							greater than wRange and not nearly a mulitple
//							of wrange/2
//		wSeed		-		anything other than zero
*/

#pragma warning(disable:4035)

static WORD CodeBytes(LPBYTE lpData, WORD wCount, WORD wRange, WORD wMask, WORD wPrime, WORD wSeed)
{
	WORD retval;
	_asm
	{	
		mov	bx,wSeed				; BX = Checksum <-- seed

#ifdef _WIN32
		mov	esi,lpData			; ESI <-- pointer to data

		movsx	ecx,wCount			; CX <-- count
		jecxz	done

#else
		push	si
		push	ds

		cld

		lds	si,lpData			; DS:SI <-- pointer to data

		mov	cx,wCount			; CX <-- count
		jcxz	done

#endif

		xor	ax,ax

next:

		lodsb							; AL <-- next byte
		add	bx,ax					; Add...

		mov 	dl,al					; ...and scramble
		and 	dl,00fh
		xchg 	dl,cl
		rol 	bx,cl
		xchg 	dl,cl

		loop	next

done:
										; BX now contains the checksum
		mov	ax,bx					; AX <-- checksum
		mul	wPrime
		and	dx,wMask				; prevent divide overflow
		div	wRange				; DX <-- remainder=code
		mov	retval,dx			; return value

#ifndef _WIN32
		pop	ds
		pop	si
#endif
	}
	return retval;
}

#pragma warning(default:4035)

static DWORD ValueBase26(LPSTR lpszString)
{
	DWORD dwResult;

	dwResult = 0;

	while (*lpszString != 0)
	{
		BYTE ch;

		ch = *(lpszString++);

		dwResult *= 26;
		dwResult += ch-'A';
	}

	return (dwResult);
}

static void StringBase26(DWORD dwNumber, WORD wLength, LPSTR lpszString)
{
	WORD i;

	*(lpszString+wLength) = 0;

	i = wLength;

	while (i-- > 0)
	{
		BYTE bDigit;

		bDigit = (BYTE)(dwNumber%26);
		*(lpszString+i) = bDigit+'A';
		dwNumber /= 26;
	}
}

#ifdef INSTDAT_WRITE
static void EncodeInstallationData(LPINSTDAT lpData)
{
	WORD i;
	BYTE bRandom;

	lpData->wVersion = 0;

	for (i = 0; i < sizeof(lpData->bReserved); i++)
	{
		lpData->bReserved[i] = 0;
	}

	for (i = lpData->bSerialNumber[0]+1; i < sizeof(lpData->bSerialNumber); i++)
	{
		lpData->bSerialNumber[i] = bRandom; bRandom = bRandom*3+13;
	}

	for (i = lpData->bUserName[0]+1; i < sizeof(lpData->bUserName); i++)
	{
		lpData->bUserName[i] = bRandom; bRandom = bRandom*2+23;
	}

	for (i = lpData->bCompanyName[0]+1; i < sizeof(lpData->bCompanyName); i++)
	{
		lpData->bCompanyName[i] = bRandom; bRandom = bRandom*5+5;
	}

	for (i = 0; i < sizeof(lpData->bRandomData); i++)
	{
		lpData->bRandomData[i] = bRandom; bRandom = bRandom*7+1;
	}

	EncryptString(lpData->bSerialNumber, lpData->bRandomData);
	EncryptString(lpData->bUserName, lpData->bRandomData);
	EncryptString(lpData->bCompanyName, lpData->bRandomData);

	lpData->bNetCount ^= NETCOUNTCODING;
	lpData->wCheckSum = CodeBytesToWord((LPBYTE)lpData, sizeof(*lpData)-sizeof(lpData->wCheckSum));
}
#endif

static BOOL DecodeInstallationData(LPINSTDAT lpData, BOOL wVirginOK)
{
	WORD i;
	char szSerialNumber[sizeof(lpData->bSerialNumber)];

	if (lpData->wVersion != 0)
	{
		return (FALSE);
	}

	for (i = 0; i < sizeof(lpData->bReserved); i++)
	{
		if (lpData->bReserved[i] != 0)
		{
			return (FALSE);
		}
	}

	if (lpData->wCheckSum != CodeBytesToWord((LPBYTE)lpData, sizeof(*lpData)-sizeof(lpData->wCheckSum)))
	{
		return (FALSE);
	}

	EncryptString(lpData->bSerialNumber, lpData->bRandomData);
	EncryptString(lpData->bUserName, lpData->bRandomData);
	EncryptString(lpData->bCompanyName, lpData->bRandomData);

	l2z(szSerialNumber, lpData->bSerialNumber);

	lpData->bNetCount ^= NETCOUNTCODING;

	return ((wVirginOK && (szSerialNumber[0] == 0)) || ValidateSerialNumber(szSerialNumber));
}

static void EncryptString(LPBYTE lpData, LPBYTE lpRandomData)
{
	WORD i;

	for (i = 1; i <= (WORD)(*lpData); i++)
	{
		lpData[i] ^= lpRandomData[i];
	}
}

static void Delay(WORD wTicks)
{
	DWORD wStartTime;
	DWORD wCurrentTime;

	wStartTime = GetTickCount();

	do
	{
		wCurrentTime = GetTickCount();
	} while (wCurrentTime >= wStartTime && wCurrentTime < wStartTime + wTicks);
}

#pragma warning(disable:4035)

static LPSTR l2z(LPSTR d, LPBYTE s)
{
	_asm
	{
#ifdef _WIN32
		mov	edi,d
		mov	esi,s

		lodsb
		xor	ah, ah
		movzx	ecx, ax

		rep	movsb

		xor	al, al
		stosb
#else
		push	si
		push	di
		push	ds

		les	di,d
		lds	si,s

		lodsb
		mov	cl,al
		xor	ch,ch

		rep	movsb

		xor	al, al
		stosb

		pop	ds
		pop	di
		pop	si
#endif
	}
	return d;
}

#pragma warning(default:4035)

#pragma warning(disable:4035)

static LPSTR substr(LPSTR d, LPSTR s, WORD c, WORD l)
{
	_asm
	{
#ifdef _WIN32
		mov	edi,d
		mov	esi,s

		movsx eax, c
		add	esi,eax

		movsx	ecx,l

		rep	movsb

		xor	al, al
		stosb

#else
		push	si
		push	di
		push	ds

		cld

		les	di,d
		lds	si,s

		add	si,c

		mov	cx,l

		rep	movsb

		xor	al, al
		stosb

		pop	ds
		pop	di
		pop	si
#endif
	}
	return d;
}

#pragma warning(default:4035)

BOOL InstallationInfo::GetLicenseRequestCode(WORD wCount, LPSTR lpszLicenseRequestCode)
{
	BOOL wResult;

	wResult = FALSE;

	if (wCount != 0)
	{
		char szData[100];
		char szCode[3];
		LPSTR s;
		LPSTR d;

		d = szData;
		s = szSerialNumber;

		while (*s)
		{
			*(d++) = *(s++);
		}

		*(d++) = (char)wCount;

		*d = bNetCount;
		if (*d == 0) *d = (char)((unsigned char)255);
		d++;

		*(d++) = bTimeStamp[0];
		*(d++) = bTimeStamp[1];
		*(d++) = bTimeStamp[2];
		*(d++) = bTimeStamp[3];

		*(d++) = 0;

		EncodeString((LPSTR)szData, 3, (LPSTR)szCode);
		wsprintf(lpszLicenseRequestCode, (LPSTR)"%03d-%03d-%s",
			wCount, bNetCount, (LPSTR)szCode);

		wResult = TRUE;
	}

	return (wResult);
}

VOID InstallationInfo::LACtoLRC(LPSTR lpszLAC, LPSTR lpszLRC)
{
	char s[5];

	*lpszLRC = 0;
	strncat(lpszLRC, lpszLAC, 4);
	sprintf(s, "%03d-", bNetCount);
	strcat(lpszLRC, s);
	strncat(lpszLRC, lpszLAC + 4, 3);
}

PRIVATE VOID
GenerateLAC(LPSTR lpszLAC, LPSTR lpszSerialNumber, LPSTR lpszLRC)
{
	char RootLAC[8];
	char Code[4];
	char s[255];

	RootLAC[0] = 0;
	strncat(RootLAC, lpszLRC, 4);
	strncat(RootLAC, lpszLRC + 8, 3);
	strcpy(s, lpszSerialNumber);
	strcat(s, lpszLRC);
	EncodeString(s, 3, Code);
	wsprintf(lpszLAC, "%s-%s", (LPSTR)RootLAC, (LPSTR)Code);
}

BOOL InstallationInfo::ValidateLicenseAccessCode(LPSTR lpszLicenseAccessCode)
{
	BOOL wResult;

	wResult = FALSE;

	if (ValidateString(lpszLicenseAccessCode, "###-AAA-AAA"))
	{
		char szCorrectLRC[32];
		WORD wCount;

		wCount = (lpszLicenseAccessCode[0]-'0')*100
				  +(lpszLicenseAccessCode[1]-'0')*10
				  +(lpszLicenseAccessCode[2]-'0')*1;

		if (wCount > 0)
		{
			if (GetLicenseRequestCode(wCount, szCorrectLRC))
			{
				char szUserLRC[12];

				/* Get the LRC implied by the user's LAC */
				LACtoLRC(lpszLicenseAccessCode, szUserLRC);

				if (strcmp(szUserLRC, szCorrectLRC) == 0)
				{
					char szCorrectLAC[12];

					/* See if this is the correct LAC for this LRC */
					GenerateLAC(szCorrectLAC, szSerialNumber, szCorrectLRC);
					wResult = (strcmp(szCorrectLAC, lpszLicenseAccessCode) == 0);
				}
			}
		}
	}

	return (wResult);
}

InstallationInfo::InstallationInfo() : m_last_load_name(NULL)
{
	nInstallConfiguration = 
		nCurrentConfiguration = 0;
	nTrialCount = 25;			// Number of times to try the trial version.
}

InstallationInfo::~InstallationInfo()
{
	if (m_last_load_name != NULL)
	{
		system_heap->free(m_last_load_name);
	}
}
