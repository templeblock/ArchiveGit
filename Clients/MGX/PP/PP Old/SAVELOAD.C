// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

/***********************************************************************/
LPFRAME AstralFrameLoad( szName, depth, lpBPP, lpType )
/***********************************************************************/
LPSTR szName;
int depth;
LPINT lpBPP;
LPINT lpType;
{
LPTR	lp;
BYTE tmp[MAX_STR_LEN];
LPFRAME lpFrame;

/* check for a three charcter extension */
lp = szName + (lstrlen (szName)-4);
if (*lp != '.')
    return (FALSE);

/* put extension into local buffer, can make lower case */
lp++;
tmp[0] = tolower(*lp);
tmp[1] = tolower(*(lp+1));
tmp[2] = tolower(*(lp+2));
tmp[3] = '\0';

AstralCursor( IDC_WAIT );	/* Put up the wait cursor */
if (!lstrcmp ("bmp", tmp))
    {
    lpFrame = bmprdr(szName, depth, lpBPP);		/* Load BMP image */
    *lpType = IDC_SAVEBMP;
    }
else if (!lstrcmp ("gif", tmp))
    {
    lpFrame = gifrdr(szName, depth, lpBPP);		/* Load GIF image */
    *lpType = IDC_SAVEGIF;
    }
else if (!lstrcmp ("pcx", tmp))
    {
    lpFrame = pcxrdr(szName, depth, lpBPP);		/* Load PCX image */
    *lpType = IDC_SAVEPCX;
    }
else if (!lstrcmp ("tga", tmp))
    {
    lpFrame = tgardr(szName, depth, lpBPP);		/* Load TARGA image */
    *lpType = IDC_SAVETARGA;
    }
else if (!lstrcmp ("tif", tmp))
    {
    lpFrame = tifrdr(szName, depth, lpBPP);		/* Load TIFF image */
    *lpType = IDC_SAVETIFF;
    }
else if (!lstrcmp ("rif", tmp))
    {
    lpFrame = rifrdr(szName, depth, lpBPP);		/* Load RIFF image */
    *lpType = IDC_SAVERIFF;
    }
else
    lpFrame = NULL;

AstralCursor( NULL );		/* Revert back to the old cursor */
return(lpFrame);
}

/***********************************************************************/
BOOL AstralFrameSave( szName, lpFrame, lpRect, DataType, FileType, fCompressed )
/***********************************************************************/
LPTR szName;
LPFRAME lpFrame;
LPRECT lpRect;
int DataType, FileType;
BOOL fCompressed;
{
long lMaxSize;
int iResult;

if (!lpFrame)
    return(FALSE);

if ( lpRect )
	{
	BoundRect(lpRect, 0, 0, lpFrame->Xsize-1, lpFrame->Ysize-1);
	lMaxSize = (long)RectWidth(lpRect) * RectHeight(lpRect);
	}
else	lMaxSize = (long)lpFrame->Xsize * lpFrame->Ysize;

if ( DataType == IDC_SAVE24BITCOLOR )
	lMaxSize *= 3;
else
if ( DataType == IDC_SAVELA || DataType == IDC_SAVESP )
	lMaxSize /= 8;
if ( FileType == IDC_SAVEEPS )
	lMaxSize *= 2;
lMaxSize += 5000;

if ( lMaxSize > DiskSpace(szName) )
	{
	Message( IDS_EDISKSPACE, Lowercase(szName),
		(lMaxSize+512)/1024, (DiskSpace(szName)+512)/1024 );
	return( FALSE );
	}

AstralCursor( IDC_WAIT );	/* Put up the wait cursor */
switch (FileType) {
    case IDC_SAVETIFF:
	iResult = tifwrt( szName, lpFrame, lpRect, DataType, fCompressed );
	break;
    case IDC_SAVEBMP:
	iResult = bmpwrt( szName, lpFrame, lpRect, DataType, fCompressed );
	break;
    case IDC_SAVEGIF:
	iResult = gifwrt( szName, lpFrame, lpRect, DataType, fCompressed );
	break;
    case IDC_SAVETARGA:
	iResult = tgawrt( szName, lpFrame, lpRect, DataType, fCompressed );
	break;
    case IDC_SAVEPCX:
	iResult = pcxwrt( szName, lpFrame, lpRect, DataType, fCompressed );
	break;
    default:
	iResult = epswrt( szName, lpFrame, lpRect, DataType, fCompressed,
			(PSTR)Control.EPSSplitDir );
	break;
    }
AstralCursor( NULL );		/* Revert back to the old cursor */
return(iResult >= 0);
}

/***********************************************************************/
int AstralImageLoad( szName )
/***********************************************************************/
LPSTR szName;
{
int i, bpp, depth, type;
LPTR   lpName, lp;
LPFRAME lpFrame;
BYTE tmp[MAX_STR_LEN];

if (!OKToOpen(szName, &lpName))
    return(FALSE);

/* check for a three charcter extension */
lp = szName + (lstrlen (szName)-4);
if (*lp != '.')
    return (FALSE);

/* put extension into local buffer, can make lower case */
lp++;
tmp[0] = tolower(*lp);
tmp[1] = tolower(*(lp+1));
tmp[2] = tolower(*(lp+2));
tmp[3] = '\0';

depth = -1;
lpFrame = AstralFrameLoad(szName, depth, &bpp, &type);
if ( !lpFrame )
	return( FALSE );

/* Setup the new image and bring up the new image window */
NewImageWindow( NULL, lpName, lpFrame, type, (bpp == 1), FALSE, IMG_DOCUMENT, NULL );
// Add the file name to our recall list
AddRecallImageName( szName );
return( TRUE );
}


/***********************************************************************/
void AddRecallImageName( lpFileName )
/***********************************************************************/
LPTR lpFileName;
{
int i;
BYTE tmp[10];

// See if we have this image name in our recall list
for ( i=0; i<MAX_RECALL; i++ )
	{
	// If so, leave now
	if ( StringsEqual( lpFileName, Control.RecallImage[i] ) )
		return;
	}
// Otherwise, add it to the list
Lowercase( lpFileName );
lstrcpy( Control.RecallImage[ Control.RecallNext ], lpFileName );
PutDefaultString( itoa(Control.RecallNext,tmp,10), lpFileName );
if ( ++Control.RecallCount >= MAX_RECALL )
	Control.RecallCount--;
if ( ++Control.RecallNext >= MAX_RECALL )
	Control.RecallNext = 0;
PutDefaultString( "Control.RecallNext", itoa(Control.RecallNext,tmp,10) );
}


/***********************************************************************/
BOOL AstralImageSave( szName )
/***********************************************************************/
LPTR szName;
{
if ( Control.Crippled )
	{
	Message( Control.Crippled );
	return( FALSE );
	}

if (!AstralFrameSave(szName, frame_set(NULL), NULL, lpImage->DataType, lpImage->FileType, Save.Compressed))
	return( FALSE );

lpImage->fUntitled = NO;
lpImage->fChanged = NO;
if ( lpImage->FileType != IDC_SAVEEPS )
	{
	// Add the file name to our recall list
	AddRecallImageName( szName );
	lstrcpy( lpImage->CurFile, szName );
	/* Force the non-client area (title bar ) of image to be painted */
        SetTitleBar(lpImage->hWnd);
	}
if ( lpImage->ImageType == IMG_BRUSH )
	{
	if (StringsEqual(lpImage->ImageName, Retouch.CustomBrushName))
		SetupCustomBrushMask();
	}
return( TRUE );
}


/***********************************************************************/
BOOL LoadMap( lpMap0, lpMap1, lpMap2, lpMap3, lpFileName )
/***********************************************************************/
LPMAP lpMap0, lpMap1, lpMap2, lpMap3;
LPTR lpFileName;
{
#define MAX_MAP_SIZE ((4*sizeof(MAP))+3+10)
LPOLDMAP lpOldMap;
LPMAP lpMap;
int i, fp, len;
BYTE buffer[MAX_MAP_SIZE];

if ( ( fp = _lopen( lpFileName, OF_READ ) ) < 0 )
	{
	Message( IDS_EOPEN, Lowercase(lpFileName) );
	return( FALSE );
	}

AstralCursor( IDC_WAIT );	/* Put up the wait cursor */
len = _lread( fp, buffer, sizeof(buffer) );
_lclose( fp );
AstralCursor( NULL );		/* Revert back to the old cursor */

if ( strncmp( "MAP", buffer, 3 ) )
	{
	Message( IDS_EBADMAP, Lowercase(lpFileName) );
	return( FALSE );
	}

len -= 3;
if ( len == sizeof(OLDMAP) )
	{
	if ( lpMap1 ) ResetMap( lpMap1, MAPPOINTS );
	if ( lpMap2 ) ResetMap( lpMap2, MAPPOINTS );
	if ( lpMap3 ) ResetMap( lpMap3, MAPPOINTS );
	if ( !lpMap0 )
		return( TRUE );
	lpOldMap = (LPOLDMAP)&buffer[3];
	clr( (LPTR)lpMap0, sizeof(MAP) );
	lpMap0->Color	   = lpOldMap->Color;
	lpMap0->Levels     = lpOldMap->Levels;
	lpMap0->Negate	   = lpOldMap->Negate;
	lpMap0->Threshold  = lpOldMap->Threshold;
	lpMap0->Points	   = lpOldMap->Points;
	lpMap0->Contrast   = 0;
	lpMap0->Brightness = 0;
	lpMap0->bStretch   = lpOldMap->bStretch;
	lpMap0->bModified  = NO;
	for ( i=0; i<lpOldMap->Points; i++ )
		{
		lpMap0->Pnt[i].x = lpOldMap->PntX[i];
		lpMap0->Pnt[i].y = lpOldMap->PntY[i];
		}
	copy( lpOldMap->Lut, lpMap0->Lut, 256 );
	return( TRUE );
	}
else
if ( len == (4 * sizeof(MAP)) )
	{
	lpMap = (LPMAP)&buffer[3];
	if ( lpMap0 )
		copy( (LPTR)lpMap, (LPTR)lpMap0, sizeof(MAP) );
	lpMap++;
	if ( lpMap1 )
		copy( (LPTR)lpMap, (LPTR)lpMap1, sizeof(MAP) );
	lpMap++;
	if ( lpMap2 )
		copy( (LPTR)lpMap, (LPTR)lpMap2, sizeof(MAP) );
	lpMap++;
	if ( lpMap3 )
		copy( (LPTR)lpMap, (LPTR)lpMap3, sizeof(MAP) );
	return( TRUE );
	}

Message( IDS_EREAD, Lowercase(lpFileName) );
return( FALSE );
}


/***********************************************************************/
BOOL SaveMap( lpMap0, lpMap1, lpMap2, lpMap3, lpFileName )
/***********************************************************************/
LPMAP lpMap0, lpMap1, lpMap2, lpMap3;
LPTR lpFileName;
{
int fp, len;
MAP NullMap;

if ( ( fp = _lcreat( lpFileName, 0 ) ) < 0 )
	{
	Message( IDS_ECREATE, Lowercase(lpFileName) );
	return( FALSE );
	}

AstralCursor( IDC_WAIT );	/* Put up the wait cursor */
ResetMap( &NullMap, MAPPOINTS );
if ( !lpMap0 ) lpMap0 = &NullMap;
if ( !lpMap1 ) lpMap1 = &NullMap;
if ( !lpMap2 ) lpMap2 = &NullMap;
if ( !lpMap3 ) lpMap3 = &NullMap;
len  = _lwrite( fp, "MAP", 3 );
len += _lwrite( fp, (LPTR)lpMap0, sizeof(MAP) );
len += _lwrite( fp, (LPTR)lpMap1, sizeof(MAP) );
len += _lwrite( fp, (LPTR)lpMap2, sizeof(MAP) );
len += _lwrite( fp, (LPTR)lpMap3, sizeof(MAP) );
_lclose( fp );
AstralCursor( NULL );		/* Revert back to the old cursor */

if ( len != (3+(4*sizeof(MAP))) )
	{
	Message( IDS_EWRITE, Lowercase(lpFileName) );
	return( FALSE );
	}

return( TRUE );
}


/***********************************************************************/
BOOL LoadPrintStyle( lpStyle, lpFileName )
/***********************************************************************/
LPSTYLE lpStyle;
LPTR lpFileName;
{
#define MAX_STYLE_SIZE (sizeof(STYLE)+3+10)
int fp, len;
BYTE buffer[MAX_STYLE_SIZE];

if ( ( fp = _lopen( lpFileName, OF_READ ) ) < 0 )
	{
	Message( IDS_EOPEN, Lowercase(lpFileName) );
	return( FALSE );
	}

AstralCursor( IDC_WAIT );	/* Put up the wait cursor */
len = _lread( fp, buffer, sizeof(buffer) );
_lclose( fp );
AstralCursor( NULL );		/* Revert back to the old cursor */

if ( strncmp( "STY", buffer, 3 ) )
	{
	Message( IDS_EBADSTYLE, Lowercase(lpFileName) );
	return( FALSE );
	}

len -= 3;
if ( len == sizeof(STYLE) )
	{
	copy( &buffer[3], (LPTR)lpStyle, sizeof(STYLE) );
	return( TRUE );
	}

Message( IDS_EREAD, Lowercase(lpFileName) );
return( FALSE );
}


/***********************************************************************/
BOOL SavePrintStyle( lpStyle, lpFileName )
/***********************************************************************/
LPSTYLE lpStyle;
LPTR lpFileName;
{
int fp, len;

if ( ( fp = _lcreat( lpFileName, 0 ) ) < 0 )
	{
	Message( IDS_ECREATE, Lowercase(lpFileName) );
	return( FALSE );
	}

AstralCursor( IDC_WAIT );	/* Put up the wait cursor */
len  = _lwrite( fp, "STY", 3 );
len += _lwrite( fp, (LPTR)lpStyle, sizeof(STYLE) );
_lclose( fp );
AstralCursor( NULL );		/* Revert back to the old cursor */

if ( len != (3+sizeof(STYLE)) )
	{
	Message( IDS_EWRITE, Lowercase(lpFileName) );
	return( FALSE );
	}

return( TRUE );
}
