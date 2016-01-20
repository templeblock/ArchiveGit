//®PL1¯®FD1¯®BT0¯®TP0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "id.h"
#include "string.h"

#ifdef BROKEN
#include "mdaapi.h"

BOOL WINPROC EXPORT MDAProc(UINT msg, LPSTR  lpName, int  Num, LPVOID lpUser);
#endif

extern HANDLE	hInstAstral;	/* handle to calling application instance */

static int GetStubInfo(LPSTR lpName,LPSTR lpFile,LPSTR lpPackage,LPSTR lpManifest,char far *lpcType, LPSTR lpLabel);


static STRING szOldPath = "";
/************************************************************************
		STUB file Format: STUB*Filename*Packagename*#*Type[*]
		  
			# is the disk number (as chars) which conatins the MANIFEST for the file
				(for Type M files the .exe	home dir is checked for manifest first)
 			Type is:
 						'S' for a Supplemental disk (No trailing * required)
					 	'M' for Main (1st release) PhotoMagic disk (No trailing * required)
					 	'L' must be followed by the 'disk label' and a trailing *
					   	
************************************************************************/

/************************************************************************
		RetriveJPG
			Checks if the specified file is a stub pointing to a JPG file
				on an archive disk.

			If the type is 'M' then it will look for the maifest in the app's
				main directory.
			If the file is a stub then the real jpg is copied over it.

			Returns FALSE if it fails.
			Retruns TRUE if file is not a STUB or real jpg was loaded.

************************************************************************/
BOOL RetrieveJPG( LPSTR lpName )
/************************************************************************/
{
#ifdef BROKEN
LPTR lpPackage;
STRING szPackage;
FNAME fnFile, fnDirectory;
int result;
char cType;
STRING szManifestDisk, szLabel;
LPMDAINFO lpMDA;
MDA_PROC MyMDAProc;

// Get Info from stub file
result = GetStubInfo(lpName, fnFile, szPackage, szManifestDisk, &cType, szLabel);
if (result==1)
	return(TRUE); 			// not a stub
if (result==-1)
	{
	Message(IDS_EREAD, lpName);    // bad stub
	return(FALSE);
	}

if (lstrlen(szPackage) > 0)
	lpPackage = (LPTR)szPackage;
else
	lpPackage = NULL;

// copy real JPEG file
if (cType == 'S')
	{																		
	lstrcpy(fnDirectory, Names.SupManifest);
	lpMDA = NULL;
  	while (!lpMDA)
		{
		result = GetDlgString(fnDirectory, fnDirectory, IDS_ASKFORMANIFEST, (LPTR)fnFile, (LPTR)szLabel, (LPTR)szManifestDisk);
		if (result == IDCANCEL)
			return(FALSE);
		AstralCursor(IDC_WAIT);
		lpMDA = OpenMDA(fnDirectory);
		if (lpMDA)
			if (GetMemberSize (lpMDA,(LPTR)lpPackage,(LPTR)fnFile) == (DWORD)-1)
				{
				CloseMDA(lpMDA);
				lpMDA=NULL;
				}
		AstralCursor(NULL);
		}
	lstrcpy(Names.SupManifest, fnDirectory);
	}
else				// default to assuming that manifest may be in PM directory
{
	GetModuleFileName(hInstAstral, fnDirectory, MAX_FNAME_LEN);
	stripfile(fnDirectory); 								// app home directory
	lpMDA = OpenMDA(fnDirectory);
	if (lpMDA)
		if (GetMemberSize (lpMDA,(LPTR)lpPackage,(LPTR)fnFile) == (DWORD)-1)
			{
			CloseMDA(lpMDA);
			lpMDA=NULL;
			}
	if (!lpMDA)
		{
		lstrcpy(fnDirectory, Names.MainManifest);
		while (!lpMDA)
			{
			result = GetDlgString(fnDirectory, fnDirectory, IDS_ASKFORMANIFEST, (LPTR)fnFile, (LPTR)szLabel, (LPTR)szManifestDisk);
			if (result == IDCANCEL)
				return(FALSE);
			AstralCursor(IDC_WAIT);
			lpMDA = OpenMDA(fnDirectory);
			if (lpMDA)
				if (GetMemberSize (lpMDA,(LPTR)lpPackage,(LPTR)fnFile) == (DWORD)-1)
					{
					CloseMDA(lpMDA);
					lpMDA=NULL;
					}
			AstralCursor(NULL);
			}
		lstrcpy(Names.MainManifest, fnDirectory);
		}
	}

MyMDAProc = (MDA_PROC)MakeProcInstance((FARPROC)MDAProc, (HINSTANCE)hInstAstral);
lstrcpy(fnDirectory, lpName);
stripfile(fnDirectory);

if (!ExtractMember(lpMDA,
					(LPTR)lpPackage,
					(LPTR)fnFile,
				  	(LPTR)fnDirectory,
					(LPTR)filename(lpName),
					MyMDAProc,
					(LPVOID)szLabel))
	{
	switch(MDAAPI_LastError)
		{
		case MDAERROR_ARCHIVE_CORRUPT:
		case MDAERROR_ARCHIVE_NOT_FOUND: 
		case MDAERROR_MEMBER_NOT_FOUND:
			Message(IDS_EBADINSTALL);
		break;

      case MDAERROR_DISK_FULL:
			Message(IDS_EDISKSPACE,
						(LPTR)lpName,
						(GetMemberSize(lpMDA,lpPackage,fnFile)+512)/1024,
						(DiskSpace(lpName)+512)/1024);
		break;
      
		case MDAERROR_MEMORY:
			Message(IDS_EMEMALLOC);
		break;

      case MDAERROR_USER_CANCEL:
		break;

		case MDAERROR_DISK_READ:
		default:
			Message(IDS_EREAD, (LPTR)lpName);
		break;											  
		}
	return(FALSE);
	}
CloseMDA(lpMDA);
FreeProcInstance(MyMDAProc);
if (szOldPath[0])
	lstrcpy(Names.InstallerDisk, szOldPath);
#endif
return(TRUE);


}

#ifdef BROKEN
/************************************************************************/
BOOL WINPROC EXPORT MDAProc ( UINT Msg, LPSTR lpName, int Num, LPVOID lpUser )
/************************************************************************/
{
#ifdef BROKEN
STRING szNum;

static STRING szFileToGet = "";
static BOOL bLoading;

switch (Msg)
	{
  	case MDAMSG_PATH:
		bLoading = FALSE;
		if (Num > 0)
			itoa(Num, szNum, 10);
		else 
			szNum[0] = '\0';
		if (!szOldPath[0])
			lstrcpy(szOldPath, Names.InstallerDisk);
		if (GetDlgString(szOldPath,lpName,
					IDS_ASKFORDISK,
					(LPTR)szFileToGet,
					(LPTR)lpUser,
					(LPTR)szNum) == IDOK)
			{
			lstrcpy(szOldPath, lpName);
			return(TRUE);
			}
		else
			return(FALSE);
		break;

	case MDAMSG_FILE:
		lstrcpy(szFileToGet, lpName);
		return(TRUE);
		break;

	case MDAMSG_PCT:
		if (!bLoading)
			{
			AstralCursor(IDC_WAIT);
			bLoading = TRUE;
			}
		if (Num == 100)
			{
			AstralCursor(NULL);
			bLoading = FALSE;
			}
		return(FALSE);
		break;
	
	default:
		return(TRUE);
		break;
	}
#endif
return(TRUE);
}
#endif

/*************************************************************************/
static int GetStubInfo( LPSTR lpName, LPSTR lpFile, LPSTR lpPackage,
                           LPSTR lpManifest, char far *lpcType, LPSTR lpLabel )
/*************************************************************************/
{
	// returns 1 if the file was not a stub
	// returns 0 if the file was a stub and properly read
 	// returns -1 if the file was an unreadable stub 

LPSTR  lpString;
STRING szStubId;
FILE_HANDLE hfFile;
long 	nread, npos;
int nsize, i;				   
	
if ((nsize = AstralStr(IDS_STUBHEADER, &lpString)) == 0)
	return(1);

lstrcpy(szStubId, lpString);

hfFile = FileOpen(lpName, FO_READ);
if (hfFile == FILE_HANDLE_INVALID)
	return(1);

nread =FileRead(hfFile, lpFile, nsize-1);
if (nread < nsize-1)
  	goto ExitGood;										//	 Not a Stub file
if (_fstrncmp(szStubId, lpFile, nsize-1))
 	goto ExitGood;										// Does not have stub ID
  

//  File is a STUB file
//  Get info from stub file

// Get File Name
npos = FileSeek(hfFile, nsize, 0);					// End of stubID + 1
if (npos < 0)
	goto ExitBad;	
nread = FileRead(hfFile, lpFile, sizeof(FNAME));	// get file name
if (nread != sizeof(FNAME))
 	goto ExitBad;
for (i = 0; i<nread; i++)
	if (lpFile[i] == '*')
		break;
if (i == nread) 											// No terminating *
	goto ExitBad;
lpFile[i] = '\0';

// Got all required info... now get extra info

// Get Package Name
nsize = npos + i + 1;
npos = FileSeek(hfFile, nsize, 0);					// End of filename + 1
if (npos < 0)
	goto ExitGotEssential;	
nread = FileRead(hfFile, lpPackage, sizeof(STRING));		// get package name
if (nread != sizeof(STRING))
 	goto ExitGotEssential;
for (i = 0; i<nread; i++)
	if (lpPackage[i] == '*')
		break;
if (i == nread) 											// No terminating *
	goto ExitGotEssential;
lpPackage[i] = '\0';

// Get Manifest Disk #
nsize = npos + i + 1;
npos = FileSeek(hfFile, nsize, 0);					// End of package name + 1
if (npos < 0)
	goto ExitGotPackage;  
nread = FileRead(hfFile, lpManifest, sizeof(STRING));	// get Manifest disk #
if (nread != sizeof(STRING))
	goto ExitGotPackage;  
for (i = 0; i<nread; i++)
	if (lpManifest[i] == '*')
		break;
if (i == nread) 											// No terminating *
	goto ExitGotPackage;  
lpManifest[i] = '\0';

// Get Disk Type (Main PM or Supplemental)
if (i >= nread-1)							 			 
	*lpcType = 'M';
else
	*lpcType = toupper(lpManifest[i+1]);
// get the label
switch (*lpcType)
	{
	case 'L':
		// read label
		nsize = npos + i + 2;
		npos = FileSeek(hfFile, nsize, 0);  			// End type identifier + 1
		if (npos < 0)
			goto ExitGotPackage; 
		nread = FileRead(hfFile, lpLabel, sizeof(STRING));		// get label
		if (nread != sizeof(STRING))
			goto ExitGotPackage;  
		for (i = 0; i<nread; i++)
			if (lpLabel[i] == '*')
				break;
		if (i == nread) 											// No terminating *
			goto ExitGotPackage;  
		lpLabel[i] = '\0';
	break;
	case 'S':
		if (AstralStr(IDS_SUPPLEMENTAL, &lpString) == 0)
			lstrcpy(lpLabel, "Supplemental Disk ");
		else
			lstrcpy(lpLabel, lpString);
	break;
	case 'M':
	default:
	   if (AstralStr(IDS_MAIN, &lpString) == 0)
			lstrcpy(lpLabel, "Disk ");
		else
			lstrcpy(lpLabel, lpString);
	break;
	}
FileClose(hfFile);											// done reading file
return (0);	  

ExitGood:
	FileClose(hfFile);
	return(1);
 
ExitBad:
	FileClose(hfFile);
	return(-1);

ExitGotEssential:
	FileClose(hfFile);
	lpPackage[0] = '\0';
	lstrcpy(lpManifest, "1");
	*lpcType = 'M';
	lstrcpy(lpLabel, "Disk ");
	return(0);

ExitGotPackage:
	FileClose(hfFile);
	lstrcpy(lpManifest, "1");
	*lpcType = 'M';
	lstrcpy(lpLabel, "Disk ");
	return(0);
}


