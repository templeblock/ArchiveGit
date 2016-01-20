//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

#define CURRENT_VERSION 3
#define PALFILE_VERSION 1
#define PALFILE_COUNT   2
#define PALFILE_COLORS  3
#define PALFILE_NAME    4
#define PALFILE_GROUP   5
#define PALFILE_LABELS  6
#define PALFILE_FORMAT  7
#define PALFILE_END     254
#define PALFILE_ERR     (-1)

/* ******************************* Typedefs ********************************* */

typedef struct                          /* Record information header */
{
	WORD    Length;
    BYTE    Type;
}
RECINFO;

typedef struct                          /* Version number record */
{
	WORD    Length;
	BYTE    Type;
	WORD    Number;
}
VERSIONINFO;

typedef struct                          /* Group information record */
{
	WORD    Length;
	BYTE    Type;
    DWORD   dwGroup;                    /* LOSHORT is major axis, HI is group */
} GROUPINFO;

/************************************************************************/
BOOL WritePalette(
/************************************************************************/
LPSTR 		lpFileName,
LPPALETTE 	lpPalette)
{
int ofh;		/* file handle( unbuffered) */
FILEBUF ofd;		/* file descriptor( buffered) */
int i;
VERSIONINFO	version;
RECINFO rec;

if (!lpPalette)
	return(FALSE);

/* open the output file */
if ( ( ofh = _lcreat( lpFileName, 0)) < 0 )
	{
	Message( IDS_EWRITE, lpFileName);
	return( NO);
	}
/* create a buffered stream to speed access */
FileFDOpenWrt( &ofd, ofh, (LPTR)LineBuffer[0], 16*1024);

// write palette version
version.Length = sizeof(version.Number);
version.Type = PALFILE_VERSION;
version.Number = CURRENT_VERSION;
FileWrite(&ofd, (LPTR)&version, sizeof(version));

while (lpPalette)
	{
	if (lpPalette->iColors || TRUE)
		{
		// write palette name
		rec.Length = lstrlen(lpPalette->szName)+1;
		rec.Type = PALFILE_NAME;
		FileWrite(&ofd, (LPTR)&rec, sizeof(rec));
		FileWrite(&ofd, (LPTR)lpPalette->szName, rec.Length);

		// write palette colors
		rec.Length = sizeof(WORD)+(lpPalette->iColors * sizeof(COLOR));
		rec.Type = PALFILE_COLORS;
		FileWrite(&ofd, (LPTR)&rec, sizeof(rec));
		intelWriteWord( &ofd, lpPalette->iColors);
		for (i = 0; i < lpPalette->iColors; ++i)
		{	
			intelWriteDWord(&ofd, RGB2long(lpPalette->lpColorInfo[i].rgb));
		}

		// write color lables
		if (lpPalette->lpLabels && lpPalette->LabelsLength)
			{
			rec.Length = lpPalette->LabelsLength;
			rec.Type = PALFILE_LABELS;
			FileWrite(&ofd, (LPTR)&rec, sizeof(rec));
			FileWrite(&ofd, lpPalette->lpLabels,
				rec.Length);
			}

		// write formating information
		if (lstrlen(lpPalette->szFormat) > 0)
			{
			rec.Length = lstrlen(lpPalette->szFormat)+1;
			rec.Type = PALFILE_FORMAT;
			FileWrite(&ofd, (LPTR)&rec, sizeof(rec));
			FileWrite(&ofd, (LPTR)lpPalette->szFormat,
				rec.Length);
			}

		// write palette grouping
		rec.Length = 4;
		rec.Type = PALFILE_GROUP;
		FileWrite(&ofd, (LPTR)&rec, sizeof(rec));
		intelWriteDWord( &ofd, lpPalette->dwGroup);

		lpPalette = lpPalette->lpNext;
		}
	}
// write palette end record
rec.Length = 0;
rec.Type = PALFILE_END;
FileWrite(&ofd, (LPTR)&rec, sizeof(rec));

FileFlush(&ofd);

_lclose(ofh);

if ( ofd.err)
	{
	Message( IDS_EWRITE, lpFileName);
	return( NO);
	}
return( YES);
}

/************************************************************************/
LPPALETTE ReadPalette( LPSTR lpFileName, BOOL bCombine )
/************************************************************************/
{
int ifh;		/* file handle( unbuffered) */
FILEBUF ifd;		/* file descriptor( buffered) */
short i, iColors;
VERSIONINFO version;
RECINFO rec;
LPTR lpByte;
LPWORD lpWord;
LPLONG lpLong;
BYTE Previous;
BOOL bError;
FNAME szName;
LPPALETTE lpHeadPalette, lpPalette, lpBigPalette;

/* open the palette file */
if ( ( ifh = _lopen( lpFileName, OF_READ)) < 0 )
	{
	Message( IDS_EOPEN, lpFileName);
	return( NULL );
	}

/* create a buffered stream to speed access */
FileFDOpenRdr( &ifd, ifh, (LPTR)LineBuffer[0], 16*1024);

// read palette version
version.Length = sizeof(version.Number);
version.Type = PALFILE_VERSION;
FileRead(&ifd, (LPTR)&version, sizeof(version));
#ifdef _MAC
swapw(&version.Length);
swapw(&version.Number);
#endif
if (ifd.err ||
    version.Type != PALFILE_VERSION ||
    version.Length != sizeof(version.Number) ||
    version.Number > CURRENT_VERSION)
	{
	Message(IDS_INVALIDPALETTE, lpFileName);
	_lclose(ifh);
	return(NULL);
	}
rec.Type = 0;
lpHeadPalette = NULL;
lpPalette = NULL;
bError = NO;
while (!bError && !ifd.err && rec.Type != PALFILE_END)
	{
	Previous = rec.Type;
	FileRead(&ifd, (LPTR)&rec, sizeof(RECINFO));
	#ifdef _MAC
	swapw(&rec.Length);
	#endif
	if (ifd.err)
		break;
	switch (rec.Type)
		{
		case PALFILE_VERSION:
			bError = YES;
			break;
		case PALFILE_COUNT:
			lpByte = Alloc((long)rec.Length);
			FileRead(&ifd, lpByte, rec.Length);
			FreeUp(lpByte);
			break;
		case PALFILE_COLORS:
			if (!lpPalette)
				{
				bError = YES;
				break;
				}
			lpWord = (LPWORD)Alloc((long)rec.Length);
			if (!lpWord)
				{
				Message (IDS_EMEMALLOC);
				bError = YES;
				break;
				}
			FileRead(&ifd, (LPTR)lpWord, rec.Length);
			if (ifd.err)
				{
				FreeUp((LPTR)lpWord);
				break;
				}
			#ifdef _MAC
			swapw(lpWord);
			#endif
			iColors = *lpWord;
			if ((iColors * sizeof(COLOR) + sizeof(WORD)) != rec.Length)
				{
				FreeUp((LPTR)lpWord);
				bError = YES;
				break;
				}
			lpPalette->iColors = iColors;
			if (!iColors)
				{
				FreeUp((LPTR)lpWord);
				break;
				}
			lpLong = (LPLONG)(lpWord+1);
			lpPalette->lpColorInfo =
				(LPCOLORINFO)Alloc((long)(sizeof(COLORINFO)*iColors));
			if (!lpPalette->lpColorInfo)
				{
			    	Message(IDS_EMEMALLOC);
				FreeUp((LPTR)lpWord);
				bError = YES;
				break;
				}
			for (i = 0; i < iColors; ++i)
			{
                LPVOID lp = &lpPalette->lpColorInfo[i].rgb;

				#ifdef _MAC
				swapl((LPDWORD)lp);
				#endif
				
				CopyRGB(lpLong+i, lp);
				SetColorInfo( 
					&lpPalette->lpColorInfo[i],
					&lpPalette->lpColorInfo[i],
					CS_RGB);
			}

			FreeUp((LPTR)lpWord);
			break;
		case PALFILE_NAME:
			if (Previous == PALFILE_NAME)
				{
				bError = YES;
				break;
				}
			if (lpPalette)
				{
				lpHeadPalette = LinkPalette(lpHeadPalette, lpPalette);
				lpPalette = NULL;
				}
			rec.Length = bound(rec.Length, 0, MAX_FNAME_LEN);
			FileRead(&ifd, (LPTR)szName, rec.Length);
			if (ifd.err)
				break;
			lpPalette = NewPalette(szName);
			if (!lpPalette)
				{
			    	Message (IDS_EMEMALLOC);
				bError = YES;
				break;
				}
			break;
		case PALFILE_GROUP:
			if (!lpPalette || (rec.Length != sizeof(lpPalette->dwGroup)))
				{
				bError = YES;
				break;
				}
			FileRead(&ifd, (LPTR)&lpPalette->dwGroup, rec.Length);
			#ifdef _MAC
			swapl((LPDWORD)(&lpPalette->dwGroup));
			#endif
			break;
		case PALFILE_LABELS:
			if (!lpPalette)
				{
				bError = YES;
				break;
				}
			lpPalette->lpLabels = Alloc((long)rec.Length);
			if (!lpPalette->lpLabels)
				{
			    	Message (IDS_EMEMALLOC);
				bError = YES;
				break;
				}
			lpPalette->LabelsLength = rec.Length;
			FileRead(&ifd, lpPalette->lpLabels, rec.Length);
			break;
		case PALFILE_FORMAT:
			if (!lpPalette || (rec.Length > MAX_STR_LEN))
				{
				bError = YES;
				break;
				}
			FileRead(&ifd, (LPTR)lpPalette->szFormat, rec.Length);
			break;
		case PALFILE_END:
			if (lpPalette)
				{
				lpHeadPalette = LinkPalette(lpHeadPalette, lpPalette);
				lpPalette = NULL;
				}
			break;
		default:
			bError = YES;
			break;
		}
	}
_lclose(ifh);
if (lpPalette)
	{
	if (lpPalette->lpColorInfo)
		FreeUp((LPTR)lpPalette->lpColorInfo);
	if (lpPalette->lpLabels)
		FreeUp((LPTR)lpPalette->lpLabels);
	FreeUp((LPTR)lpPalette);
	}
if (ifd.err || bError || rec.Type != PALFILE_END || lpPalette)
	{
	Message(IDS_INVALIDPALETTE, lpFileName);
	FreeUpPalette(lpHeadPalette);
	return(NULL);
	}

if ( !bCombine || !lpHeadPalette )
	return( lpHeadPalette );

if ( !(lpBigPalette = CombinePalettes(lpHeadPalette)) )
	Message (IDS_EMEMALLOC);
FreeUpPalette(lpHeadPalette);
return( lpBigPalette );
}
