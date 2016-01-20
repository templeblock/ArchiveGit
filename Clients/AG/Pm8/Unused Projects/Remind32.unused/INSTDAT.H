/*
//	PrintMaster version of INSTDAT.H
//
// $Workfile: INSTDAT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright (C) 1992, 1993 by MicroLogic Software, Inc. All Rights Reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/INSTDAT.H $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:04a Mwilson
// 
//    Rev 1.0   14 Aug 1997 15:21:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:18   Fred
// Initial revision.
// 
//    Rev 1.2   04 Mar 1997 17:04:04   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.1   15 May 1996 08:46:06   Jay
// From PMW2
// 
//    Rev 2.3   09 May 1996 17:22:24   JAY
// Support for trial version run counts.
// 
//    Rev 2.2   29 Jan 1996 16:53:28   JAY
// Variable configuration in INSTALL and PMW.
// 
//    Rev 2.1   08 Feb 1995 13:39:38   JAY
// Reverted. New series.
// 
//    Rev 1.2   04 Aug 1994 08:30:32   JAY
// Large model and 32-bit changes
// 
//    Rev 1.1   12 May 1994 07:35:14   JAY
// Changed some LPSTRs to LPCSTRs.
// 
//    Rev 1.0   16 Dec 1993 16:29:06   JAY
//  
// 
//    Rev 1.1   13 Jul 1993 14:26:44   JAY
// Support for writing installtion info.
// Support for License Access Codes.
// 
//    Rev 1.0   28 Apr 1993 13:05:26   MACDUFF
// Initial revision.
// 
//    Rev 1.3   14 Dec 1992 13:36:48   FRED
// First attempt at single source code for
// TrueEffects and MoreFonts.
// 
// 
//    Rev 1.2   26 Nov 1992 08:54:22   FRED
// TrueEffects version.
// 
// 
//    Rev 1.1   10 Jul 1992 10:05:54   FRED
// Added bTimeStamp[4] field to INSTALLINFO structure.
// This was needed for LRC/LAC computation.
// 
// 
//    Rev 1.0   06 Jul 1992 10:46:36   FRED
// Initial revision.
*/ 

#ifndef __INSTDAT_H__
#define __INSTDAT_H__

#define INSTDAT_WRITE

/*
// Structure of INST.DAT file.
*/

#pragma pack(1)
typedef struct
{
	BYTE bCopyright[96];
	WORD wVersion;
	BYTE bInstallConfiguration;			// Configuration at install (CD)
	BYTE bCurrentConfiguration;			// Configuration now
	BYTE bTrialCount;							// Count left in trial
	BYTE bCurrentConfiguration2;			// For extensions like Deluxe, etc.
	BYTE bReserved[26];
	BYTE bSerialNumber[33];
	BYTE bUserName[81];
	BYTE bCompanyName[81];
	BYTE bRandomData[182];
	BYTE bNetCount;			// Byte 505 (01f9h)
	BYTE bTimeStamp[4];
	WORD wCheckSum;
} INSTDAT;
#pragma pack()

typedef	INSTDAT *PINSTDAT;
typedef	INSTDAT far *LPINSTDAT;


/*
// Structure of data record used to pass installation information.
*/

#pragma pack(1)
struct InstallationInfo
{
private:
	VOID LACtoLRC(LPSTR lpszLAC, LPSTR lpszLRC);
	LPSTR m_last_load_name;
public:

	InstallationInfo();
	~InstallationInfo();

	char szSerialNumber[32];
	char szUserName[80];
	char szCompanyName[80];
	BYTE bNetCount;
	BYTE bTimeStamp[4];
	int nInstallConfiguration;
	int nCurrentConfiguration;
	int nTrialCount;

	BOOL Get(LPCSTR lpszFileName, BOOL wVirginOK);
	BOOL Put(LPCSTR lpszFileName = NULL);
	BOOL GetLicenseRequestCode(WORD wCount, LPSTR lpszLicenseRequestCode);
	BOOL ValidateLicenseAccessCode(LPSTR lpszLicenseAccessCode);
};
#pragma pack()

typedef	InstallationInfo *PINSTALLATIONINFO;
typedef	InstallationInfo far *LPINSTALLATIONINFO;

/*
// Structure for holding the parameters for string encoding.
*/

#pragma pack(1)
typedef struct
{
	WORD wRange;
	WORD wMask;
	WORD wPrime;
	WORD wSeed;
} CODEPARAMETERS;
#pragma pack()

typedef	CODEPARAMETERS *PCODEPARAMETERS;
typedef	CODEPARAMETERS far *LPCODEPARAMETERS;

/*
// This constant is used for encoding the network license count.
*/

#define	NETCOUNTCODING	(0xa5)


#endif				// #ifndef __INSTDAT_H__
