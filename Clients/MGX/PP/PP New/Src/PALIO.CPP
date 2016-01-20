//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

#define CURRENT_VERSION    3
#define PALFILE_VERSION    1
#define PALFILE_COUNT      2
#define PALFILE_COLORS     3
#define PALFILE_NAME       4
#define PALFILE_GROUP      5
#define PALFILE_LABELS     6
#define PALFILE_FORMAT     7
#define PALFILE_NEWCOLORS  8
#define PALFILE_END        254
#define PALFILE_ERR        (-1)

/* ******************************* Typedefs ********************************* */

#define CMYK(c,m,y,k) ((long)(((BYTE)(c)|((WORD)(m)<<8))|(((DWORD)(BYTE)(y)|((WORD)(k)<<8))<<16)))
#define HSL2long(hsl) RGB( (hsl).hue, (hsl).sat, (hsl).lum)
#define CMYK2long(cmyk) CMYK( (cmyk).c, (cmyk).m, (cmyk).y, (cmyk).k)

#define CopyHSL(s, d)                                                   \
{                                                                       \
*((LPTR)(d)) = *((LPTR)(s));                                        \
*((LPTR)(d)+1) = *((LPTR)(s)+1);                                        \
*((LPTR)(d)+2) = *((LPTR)(s)+2);                                        \
}


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
BOOL Palette_WriteColorMap( LPSTR lpFileName, LPCOLORMAP lpColorMap, LPSTR
							lpPaletteName)
/************************************************************************/
{
LPPALETTE lpPalette;
BOOL fRet;

lpPalette = Palette_CreateFromColorMap(lpColorMap);
if (!lpPalette)
	{
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}
lstrcpy(lpPalette->szName, lpPaletteName);
fRet = Palette_Write(lpFileName, lpPalette);
Palette_FreeUp(lpPalette);
return(fRet);
}

/************************************************************************/
BOOL Palette_Write( LPSTR lpFileName, LPPALETTE lpPalette )
/************************************************************************/
{
FILE_HANDLE ofh;		/* file handle( unbuffered) */
int i;
VERSIONINFO	version;
RECINFO rec;
UINT16 word;
UINT32 dword;

if (!lpPalette)
	return(FALSE);

/* open the output file */
if ( ( ofh = FileOpen( lpFileName, FO_CREATE|FO_WRITE)) == FILE_HANDLE_INVALID )
	{
	Message( IDS_EWRITE, lpFileName);
	return( NO);
	}

// write palette version
version.Length = sizeof(version.Number);
version.Type = PALFILE_VERSION;
version.Number = CURRENT_VERSION;
if (FileWrite(ofh, (LPTR)&version.Length, sizeof(version.Length)) !=
										sizeof(version.Length))
	goto WriteError;
if (FileWrite(ofh, (LPTR)&version.Type, sizeof(version.Type)) !=
										sizeof(version.Type))
	goto WriteError;
if (FileWrite(ofh, (LPTR)&version.Number, sizeof(version.Number)) !=
										sizeof(version.Number))
	goto WriteError;

while (lpPalette)
	{
	if (lpPalette->iColors || TRUE)
		{
		// write palette name
		rec.Length = lstrlen(lpPalette->szName)+1;
		rec.Type = PALFILE_NAME;
		if (FileWrite(ofh, (LPTR)&rec.Length, sizeof(rec.Length)) !=
										sizeof(rec.Length))
			goto WriteError;
		if (FileWrite(ofh, (LPTR)&rec.Type, sizeof(rec.Type)) !=
										sizeof(rec.Type))
			goto WriteError;
		if (FileWrite(ofh, lpPalette->szName, rec.Length) != rec.Length)
			goto WriteError;

      if (GetDefaultInt("WriteOldPalFormat", 0))
         {
		   // write palette colors
		   rec.Length = sizeof(WORD)+(lpPalette->iColors * sizeof(COLOR));
		   rec.Type = PALFILE_COLORS;
		   if (FileWrite(ofh, (LPTR)&rec.Length, sizeof(rec.Length)) !=
										sizeof(rec.Length))
			   goto WriteError;
		   if (FileWrite(ofh, (LPTR)&rec.Type, sizeof(rec.Type)) !=
										sizeof(rec.Type))
			   goto WriteError;
		   word = lpPalette->iColors;
		   if (FileWrite( ofh, (LPVOID)&word, 2) != 2)
			   goto WriteError;
		   for (i = 0; i < lpPalette->iColors; ++i)
		      {	
			   dword = RGB2long(lpPalette->lpColorInfo[i].rgb);
			   if (FileWrite( ofh, (LPVOID)&dword, 4) != 4)
				   goto WriteError;
		      }
         }
      else
         {
		   // write palette colors
		   rec.Length = sizeof(WORD)+ (lpPalette->iColors * (sizeof(long)+sizeof(WORD)));
		   rec.Type = PALFILE_NEWCOLORS;
		   if (FileWrite(ofh, (LPTR)&rec.Length, sizeof(rec.Length)) !=
										sizeof(rec.Length))
			   goto WriteError;
		   if (FileWrite(ofh, (LPTR)&rec.Type, sizeof(rec.Type)) !=
										sizeof(rec.Type))
			   goto WriteError;
		   word = lpPalette->iColors;
		   if (FileWrite( ofh, (LPVOID)&word, 2) != 2)
			   goto WriteError;
		   for (i = 0; i < lpPalette->iColors; ++i)
		      {
            word  = lpPalette->lpColorInfo[i].ColorSpace;
            switch (word)
               {
               case CS_GRAY:
                  dword = lpPalette->lpColorInfo[i].gray;
                  break;
               case CS_HSL:
			         dword = HSL2long(lpPalette->lpColorInfo[i].hsl);
                  break;
               case CS_RGB:
			         dword = RGB2long(lpPalette->lpColorInfo[i].rgb);
                  break;
               case CS_CMYK:
			         dword = CMYK2long(lpPalette->lpColorInfo[i].cmyk);
                  break;
               }
			   if (FileWrite( ofh, (LPVOID)&word, 2) != 2)
				   goto WriteError;
			   if (FileWrite( ofh, (LPVOID)&dword, 4) != 4)
				   goto WriteError;
		      }
         } 		// write color lables
		if (lpPalette->lpLabels && lpPalette->LabelsLength)
			{
			rec.Length = lpPalette->LabelsLength;
			rec.Type = PALFILE_LABELS;
			if (FileWrite(ofh, (LPTR)&rec.Length, sizeof(rec.Length)) !=
											sizeof(rec.Length))
				goto WriteError;
			if (FileWrite(ofh, (LPTR)&rec.Type, sizeof(rec.Type)) !=
											sizeof(rec.Type))
				goto WriteError;
			if (FileWrite(ofh, lpPalette->lpLabels, rec.Length) != rec.Length)
				goto WriteError;
			}

		// write formating information
		if (lstrlen(lpPalette->szFormat) > 0)
			{
			rec.Length = lstrlen(lpPalette->szFormat)+1;
			rec.Type = PALFILE_FORMAT;
			if (FileWrite(ofh, (LPTR)&rec.Length, sizeof(rec.Length)) !=
											sizeof(rec.Length))
				goto WriteError;
			if (FileWrite(ofh, (LPTR)&rec.Type, sizeof(rec.Type)) !=
											sizeof(rec.Type))
				goto WriteError;
			if (FileWrite(ofh, lpPalette->szFormat, rec.Length) != rec.Length)
				goto WriteError;
			}

		// write palette grouping
		rec.Length = 4;
		rec.Type = PALFILE_GROUP;
		if (FileWrite(ofh, (LPTR)&rec.Length, sizeof(rec.Length)) !=
										sizeof(rec.Length))
			goto WriteError;
		if (FileWrite(ofh, (LPTR)&rec.Type, sizeof(rec.Type)) !=
										sizeof(rec.Type))
			goto WriteError;
		dword = lpPalette->dwGroup;
		if (FileWrite( ofh, &dword, 4) != 4)
			goto WriteError;

		lpPalette = lpPalette->lpNext;
		}
	}
// write palette end record
rec.Length = 0;
rec.Type = PALFILE_END;
if (FileWrite(ofh, (LPTR)&rec.Length, sizeof(rec.Length)) !=
								sizeof(rec.Length))
	goto WriteError;
if (FileWrite(ofh, (LPTR)&rec.Type, sizeof(rec.Type)) !=
								sizeof(rec.Type))
	goto WriteError;

FileClose(ofh);
return( YES);

WriteError:
Message( IDS_EWRITE, lpFileName);
return( NO);
}

/************************************************************************/
LPCOLORMAP Palette_ReadColorMap( LPSTR lpFileName )
/************************************************************************/
{
LPCOLORMAP lpColorMap;
LPPALETTE lpPalette;
int i;

lpPalette = Palette_Read(lpFileName, YES);
if (!lpPalette)
	return(NULL);
if (lpPalette->iColors > 256)
	{
	Message(IDS_TOOMANYCOLORS);
	Palette_FreeUp(lpPalette);
	return(NULL);
	}
lpColorMap = FrameCreateColorMap();
if (!lpColorMap)
	{
	Message(IDS_EMEMALLOC);
	Palette_FreeUp(lpPalette);
	return(NULL);	
	}
lpColorMap->NumEntries = lpPalette->iColors;
for (i = 0; i < lpPalette->iColors; ++i)
	lpColorMap->RGBData[i] = lpPalette->lpColorInfo[i].rgb;
Palette_FreeUp(lpPalette);
return(lpColorMap);
}

/************************************************************************/
LPPALETTE Palette_Read( LPSTR lpFileName, BOOL bCombine )
/************************************************************************/
{
FILE_HANDLE ifh;		/* file handle( unbuffered) */
int i, iColors;
VERSIONINFO version;
RECINFO rec;
LPTR lpByte;
LPWORD lpWord;
LPLONG lpLong;
BYTE Previous;
BOOL bError;
FNAME szName;
LPPALETTE lpHeadPalette, lpPalette, lpBigPalette;
WORD  word;

/* open the palette file */
if ( ( ifh = FileOpen( lpFileName, FO_READ)) == FILE_HANDLE_INVALID )
	{
	Message( IDS_EOPEN, lpFileName);
	return( NULL );
	}

rec.Type = 0;
lpHeadPalette = NULL;
lpPalette = NULL;
bError = NO;

// read palette version
version.Length = sizeof(version.Number);
version.Type = PALFILE_VERSION;
if (FileRead(ifh, (LPTR)&version.Length, sizeof(version.Length)) !=
										sizeof(version.Length))
	goto ReadError;
if (FileRead(ifh, (LPTR)&version.Type, sizeof(version.Type)) !=
										sizeof(version.Type))
	goto ReadError;
if (FileRead(ifh, (LPTR)&version.Number, sizeof(version.Number)) !=
										sizeof(version.Number))
	goto ReadError;
if (version.Type != PALFILE_VERSION ||
    version.Length != sizeof(version.Number) ||
    version.Number > CURRENT_VERSION)
	{
	Message(IDS_INVALIDPALETTE, lpFileName);
	FileClose(ifh);
	return(NULL);
	}
while (!bError && rec.Type != PALFILE_END)
	{
	Previous = rec.Type;
	if (FileRead(ifh, (LPTR)&rec.Length, sizeof(rec.Length)) !=
										sizeof(rec.Length))
		goto ReadError;
	if (FileRead(ifh, (LPTR)&rec.Type, sizeof(rec.Type)) !=
										sizeof(rec.Type))
		goto ReadError;
	switch (rec.Type)
		{
		case PALFILE_VERSION:
			bError = YES;
			break;
		case PALFILE_COUNT:
			lpByte = Alloc((long)rec.Length);
			if (FileRead(ifh, lpByte, rec.Length) != rec.Length)
				{
				FreeUp(lpByte);
				goto ReadError;
				}
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
				goto MemError;
			if (FileRead(ifh, (LPTR)lpWord, rec.Length) != rec.Length)
				{
				FreeUp((LPTR)lpWord);
				goto ReadError;
				}
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
				CopyRGB(lpLong+i, &lpPalette->lpColorInfo[i].rgb);
				SetColorInfo( 
					&lpPalette->lpColorInfo[i],
					&lpPalette->lpColorInfo[i],
					CS_RGB);
			}

			FreeUp((LPTR)lpWord);
			break;
		case PALFILE_NEWCOLORS:
			if (!lpPalette)
				{
				bError = YES;
				break;
				}
			lpWord = (LPWORD)Alloc((long)rec.Length);
			if (!lpWord)
				goto MemError;
			if (FileRead(ifh, (LPTR)lpWord, rec.Length) != rec.Length)
				{
				FreeUp((LPTR)lpWord);
				goto ReadError;
				}
			iColors = *lpWord;
			if ((iColors * (sizeof(long)+sizeof(WORD)) + sizeof(WORD)) != rec.Length)
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
            word = lpPalette->lpColorInfo[i].ColorSpace = (COLORSPACE)*((LPWORD) lpLong);
            lpLong = (LPLONG)((LPWORD)lpLong + 1);
            switch (word)
               {
               case CS_GRAY:
                  lpPalette->lpColorInfo[i].gray = *lpLong;
				      SetColorInfo(&lpPalette->lpColorInfo[i],&lpPalette->lpColorInfo[i],CS_GRAY);
                  break;
               case CS_HSL:
				      CopyHSL(lpLong, &lpPalette->lpColorInfo[i].hsl);
				      SetColorInfo(&lpPalette->lpColorInfo[i],&lpPalette->lpColorInfo[i],CS_HSL);
                  break;
               case CS_RGB:
				      CopyRGB(lpLong, &lpPalette->lpColorInfo[i].rgb);
				      SetColorInfo(&lpPalette->lpColorInfo[i],&lpPalette->lpColorInfo[i],CS_RGB);
                  break;
               case CS_CMYK:
				      CopyCMYK(lpLong, &lpPalette->lpColorInfo[i].cmyk);
				      SetColorInfo(&lpPalette->lpColorInfo[i],&lpPalette->lpColorInfo[i],CS_CMYK);
                  break;
               }
            lpLong++;
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
				lpHeadPalette = Palette_Link(lpHeadPalette, lpPalette);
				lpPalette = NULL;
				}
			rec.Length = bound(rec.Length, 0, MAX_FNAME_LEN);
			if (FileRead(ifh, szName, rec.Length) != rec.Length)
				goto ReadError;
			lpPalette = Palette_New(szName);
			if (!lpPalette)
				goto MemError;
			break;
		case PALFILE_GROUP:
			if (!lpPalette || (rec.Length != sizeof(lpPalette->dwGroup)))
				{
				bError = YES;
				break;
				}
			if (FileRead(ifh, (LPTR)&lpPalette->dwGroup, rec.Length) != rec.Length)
				goto ReadError;
			break;
		case PALFILE_LABELS:
			if (!lpPalette)
				{
				bError = YES;
				break;
				}
			lpPalette->lpLabels = (LPSTR)Alloc((long)rec.Length);
			if (!lpPalette->lpLabels)
				goto MemError;
			lpPalette->LabelsLength = rec.Length;
			if (FileRead(ifh, lpPalette->lpLabels, rec.Length) != rec.Length)
				goto ReadError;
			break;
		case PALFILE_FORMAT:
			if (!lpPalette || (rec.Length > MAX_STR_LEN))
				{
				bError = YES;
				break;
				}
			if (FileRead(ifh, lpPalette->szFormat, rec.Length) != rec.Length)
				goto ReadError;
			break;
		case PALFILE_END:
			if (lpPalette)
				{
				lpHeadPalette = Palette_Link(lpHeadPalette, lpPalette);
				lpPalette = NULL;
				}
			break;
		default:
			bError = YES;
			break;
		}
	}
FileClose(ifh);
if (lpPalette)
	Palette_FreeUp(lpPalette);
if (bError || rec.Type != PALFILE_END || lpPalette)
	{
	Message(IDS_INVALIDPALETTE, lpFileName);
	Palette_FreeUp(lpHeadPalette);
	return(NULL);
	}

if ( !bCombine || !lpHeadPalette )
	return( lpHeadPalette );

if ( !(lpBigPalette = Palette_Combine(lpHeadPalette)) )
	Message (IDS_EMEMALLOC);
Palette_FreeUp(lpHeadPalette);
return( lpBigPalette );

MemError:
Message(IDS_EMEMALLOC);
goto Exit;

ReadError:
Message( IDS_EREAD, lpFileName);

Exit:
FileClose(ifh);
if (lpPalette)
	Palette_FreeUp(lpPalette);
if (lpHeadPalette)
	Palette_FreeUp(lpHeadPalette);
return(NULL);
}




