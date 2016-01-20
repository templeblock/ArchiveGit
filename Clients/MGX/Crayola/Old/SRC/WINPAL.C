//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"				 
#include "routines.h"

// Static Prototypes
static void Map8(  LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount );
static void Map24( LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount );
static void Map32( LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount );
static void map_proc(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth );
static void MapPreviewProc(int x, int y, int dx, int depth,
				LPTR lpSrc, LPTR lpDst, LPTR lpMsk, LPMASK lpMask);

static LPTR lpHLut, lpSLut, lpMLut, lpRLut, lpGLut, lpBLut;
static BOOL fMyLuts;		// true if the lut memory was setup locally

static HPALETTE hSystemPalette;

/************************************************************************/

void SaveSystemPalette(HWND hWnd)
{
	if ( hSystemPalette )
		RestoreSystemPalette( hWnd );

//	return;
//	DeleteObject( hSystemPalette );

	hSystemPalette = CopySystemPalette();
}


/************************************************************************/

void RestoreSystemPalette( HWND hWnd )
{
	HDC hDC;

	if ( !hSystemPalette )
		return;

	if ( !(hDC = GetDC( hWnd )) )
		return;

	// Using FALSE below is key to having the background restore properly
	SelectPalette( hDC, hSystemPalette, FALSE );
	RealizePalette( hDC );
	ReleaseDC( hWnd, hDC );
	DeleteObject( hSystemPalette );
	hSystemPalette = NULL;
}

/***********************************************************************/

HPALETTE CopySystemPalette()
{
#define NUMENTRIES 256

	LPLOGPALETTE lpPal;
	LPPALETTEENTRY lpEntry;
	PALETTEENTRY   TempEntries[256+1];
	HWND hWnd;
	HDC hDC;
	int i, n;
	
	lpPal = (LPLOGPALETTE)TempEntries;
	lpPal->palVersion = 0x300;
	lpPal->palNumEntries = 256;
	lpEntry = lpPal->palPalEntry;

	if ( !(hWnd = GetDesktopWindow()) )
		return( NULL );

	if ( !(hDC = GetDC( hWnd )) )
		return( NULL );

	n = GetSystemPaletteEntries( hDC, 0, NUMENTRIES, lpEntry );
	ReleaseDC( hWnd, hDC );

	if ( n != NUMENTRIES )
		return( NULL );

	for ( i=0; i<NUMENTRIES; i++ )
		(lpEntry+i)->peFlags = PC_RESERVED;

	return( CreatePalette( lpPal ) );
}

/***********************************************************************/

HPALETTE CopyPalette( HPALETTE hPal )
{
	LPLOGPALETTE lpPal;
	LPPALETTEENTRY lpEntry;
	PALETTEENTRY TempEntries[256+1];
	short iNumEntries;

	GetObject(hPal, 2, (LPSTR)&iNumEntries);
	lpPal = (LPLOGPALETTE)TempEntries;
	lpPal->palVersion = 0x300;
	lpPal->palNumEntries = iNumEntries;
	lpEntry = lpPal->palPalEntry;
	GetPaletteEntries(hPal, 0, iNumEntries, lpEntry);
	return( CreatePalette( lpPal ) );
}

/***********************************************************************/

HPALETTE CreateCustomPalette( LPRGB lpRGBmap, int iEntries )
{
	LPLOGPALETTE   lpPal;
	LPPALETTEENTRY lpEntry;
	PALETTEENTRY   TempEntries[256+1];
	LPRGB lpRGB;
	int i;

	lpPal = (LPLOGPALETTE)TempEntries;
	lpPal->palVersion = 0x300;
	lpPal->palNumEntries = iEntries;
	lpEntry = lpPal->palPalEntry;

	lpRGB = lpRGBmap;
	for (i = 0; i < iEntries; ++i)
	{
		lpEntry->peRed   = lpRGB->red;
		lpEntry->peGreen = lpRGB->green;
		lpEntry->peBlue  = lpRGB->blue;
		lpEntry->peFlags = PC_RESERVED;
		lpEntry++;
		lpRGB++;
	}
	return( CreatePalette( lpPal ) );
}

/************************************************************************/

void ApplyMap(int dirty)
{
	ENGINE Engine;

	if (!lpImage)
		return;

	MapSetupLuts(
		HueMap.Lut,
		SatMap.Lut,
		MasterMap.Lut,
		RedMap.Lut,
		GreenMap.Lut,
		BlueMap.Lut);

	SetEngineDef(&Engine);
	Engine.lpDataProc = map_proc;
	LineEngineSelObj(lpImage, &Engine, dirty);
	MapSetupLuts(NULL, NULL, NULL, NULL, NULL, NULL);
}

/************************************************************************\
	Applys the map to lpFrame.
\************************************************************************/

void ApplyMapToFrame(LPFRAME lpFrame)
{
	int y, dy, dx, depth;
	LPTR lp;

	MapSetupLuts(
		HueMap.Lut,
		SatMap.Lut,
		MasterMap.Lut,
		RedMap.Lut,
		GreenMap.Lut,
		BlueMap.Lut);

	dy = FrameYSize(lpFrame);
	dx = FrameXSize(lpFrame);
	depth = FrameDepth(lpFrame);

	if (!depth) depth = 1;
	for (y = 0; y < dy; ++y)
	{
		if (!(lp = FramePointer(lpFrame, 0, y, YES)))
			break;
		map_proc(y, 0, dx-1, lp, lp, depth );
	}
	MapSetupLuts(NULL, NULL, NULL, NULL, NULL, NULL);
}

/************************************************************************\
	Same as setup luts, exept it puts the packed lut into lpDst.
	All values must be non-null.
	Returns lpDst.
\************************************************************************/

LPTR MapGetPackedLut(LPTR lpDst,LPTR HLut,LPTR SLut,LPTR MLut,LPTR RLut,LPTR GLut,LPTR BLut)
{
	LPTR lpTmpHLut, lpTmpSLut, lpTmpMLut, lpTmpRLut, lpTmpGLut, lpTmpBLut;
 	
 	lpTmpHLut = lpDst;
 	lpTmpSLut = lpTmpHLut + 256;
 	lpTmpMLut = lpTmpSLut + 256;
 	lpTmpRLut = lpTmpMLut + 256;
 	lpTmpGLut = lpTmpRLut + 256;
 	lpTmpBLut = lpTmpGLut + 256;
	copy(HLut, lpTmpHLut, 256);
	copy(SLut, lpTmpSLut, 256);
	copy(MLut, lpTmpMLut, 256);
	copy(RLut, lpTmpRLut, 256);
	copy(GLut, lpTmpGLut, 256);
	copy(BLut, lpTmpBLut, 256);
	return(lpTmpHLut);
}

/************************************************************************\
	Same as setup luts, exept it gets the packed lut from lpLuts.
	if lpLuts = NULL, then clears luts
\************************************************************************/

void MapSetPackedLut(LPTR lpLuts)
{
	if (lpHLut && fMyLuts)		 
	{
		FreeUp(lpHLut);
		lpHLut = NULL;
	}
	lpHLut = lpLuts;
	if (!lpHLut)
		return;
	fMyLuts = FALSE;	
	lpSLut = lpHLut + 256;
	lpMLut = lpSLut + 256;
	lpRLut = lpMLut + 256;
	lpGLut = lpRLut + 256;
	lpBLut = lpGLut + 256;
}

/************************************************************************/

BOOL MapSetupLuts( LPTR HLut, LPTR SLut, LPTR MLut, LPTR RLut, LPTR GLut,
                     LPTR BLut )
{
	if (!HLut)
	{
		if (lpHLut && fMyLuts)
		{
			FreeUp(lpHLut);
			lpHLut = NULL;
		}
		return(TRUE);
	}
	else
	{
		if (!lpHLut || !fMyLuts)
		{
			lpHLut = Alloc(256L*6L);
			if (!lpHLut)
				return(FALSE);
			fMyLuts = TRUE;
			lpSLut = lpHLut + 256;
			lpMLut = lpSLut + 256;
			lpRLut = lpMLut + 256;
			lpGLut = lpRLut + 256;
			lpBLut = lpGLut + 256;
		}
		copy(HLut, lpHLut, 256);
		copy(SLut, lpSLut, 256);
		copy(MLut, lpMLut, 256);
		copy(RLut, lpRLut, 256);
		copy(GLut, lpGLut, 256);
		copy(BLut, lpBLut, 256);
		return(TRUE);
	}
}

/************************************************************************/

void ResetAllMaps()
{
	ResetMapEx( &MasterMap, MAPPOINTS, NO, YES);
	ResetMap( &RedMap,    MAPPOINTS, NO ); // sometimes Cyan
	ResetMap( &GreenMap,  MAPPOINTS, NO ); // sometimes Magenta
	ResetMap( &BlueMap,   MAPPOINTS, NO ); // sometimes Yellow
	ResetMap( &HueMap,    MAXPOINTS, NO );
	ResetMap( &SatMap,    MAPPOINTS, NO );
}

/************************************************************************/

void ResetBandAllMaps(WORD wBand)
{
	MasterMap.Brightness[wBand] = 0;
	RedMap.Brightness[wBand] = 0;
	GreenMap.Brightness[wBand] = 0;
	BlueMap.Brightness[wBand] = 0;
	HueMap.Brightness[wBand] = 0;
	SatMap.Brightness[wBand] = 0;
	MakeMap(&MasterMap);
	MasterMap.bModified = NO;
	MakeMap(&BlueMap);
	BlueMap.bModified = NO;
	MakeMap(&GreenMap);
	GreenMap.bModified = NO;
	MakeMap(&RedMap);
	RedMap.bModified = NO;
	MakeMap(&HueMap);
	HueMap.bModified = NO;
	MakeMap(&SatMap);
	SatMap.bModified = NO;
	
}


/************************************************************************/

static void map_proc(int y, int left, int right, LPTR lpSrc, LPTR lpDst, int depth )
{
	int iCount = right - left + 1;

	switch(depth)
	{
		case 0 :
		case 1 :
			while( iCount-- > 0 )
			{
				*lpDst = *lpSrc++;
				MapGray( lpDst );
				lpDst++;
			}
		break;

		case 3 :
			while( iCount-- > 0 )
			{
				lpDst[0] = *lpSrc++;
				lpDst[1] = *lpSrc++;
				lpDst[2] = *lpSrc++;
				MapRGB( &lpDst[0], &lpDst[1], &lpDst[2]);
				lpDst += 3;
			}
		break;

		case 4 :
			while( iCount-- > 0 )
			{
				lpDst[0] = *lpSrc++;
				lpDst[1] = *lpSrc++;
				lpDst[2] = *lpSrc++;
				lpDst[3] = *lpSrc++;
				MapCMYK( &lpDst[0], &lpDst[1], &lpDst[2], &lpDst[3]);
				lpDst += 4;
			}
		break;
	}
}

/***********************************************************************/

void MapGray( LPTR lpGray )
{
	if (!lpHLut)
		return;

	if ( HueMap.bModified || SatMap.bModified )
	{
		*lpGray = lpMLut[ *lpGray ];
	}
	else
	{
		*lpGray = lpGLut[ 255-lpMLut[255-*lpGray] ];
	}
}

/***********************************************************************/

void MapRGB( LPTR lpRed, LPTR lpGreen, LPTR lpBlue )
{
	RGBS rgb;
	HSLS hsl;

	if (!lpHLut)
		return;

	if ( HueMap.bModified || SatMap.bModified )
	{
		RGBtoHSL( *lpRed, *lpGreen, *lpBlue, &hsl );
		hsl.hue =  lpHLut[ hsl.hue ];
		hsl.sat =  lpSLut[ hsl.sat ];
		hsl.lum =  lpMLut[ hsl.lum ];
		HSLtoRGB( hsl.hue, hsl.sat, hsl.lum, &rgb );
		*lpRed   = rgb.red;
		*lpGreen = rgb.green;
		*lpBlue  = rgb.blue;
	}
	else
	{
		*lpRed   = lpRLut[ 255-lpMLut[255-*lpRed]   ];
		*lpGreen = lpGLut[ 255-lpMLut[255-*lpGreen] ];
		*lpBlue  = lpBLut[ 255-lpMLut[255-*lpBlue]  ];
	}
}

/***********************************************************************/

void MapCMYK( LPTR lpC, LPTR lpM, LPTR lpY, LPTR lpK )
{
	CMYKS cmyk;
	HSLS  hsl;

	if (!lpHLut)
		return;

	if ( HueMap.bModified || SatMap.bModified )
	{
		CMYKtoHSL( *lpC, *lpM, *lpY, *lpK, &hsl );
		hsl.hue =  lpHLut[ hsl.hue ];
		hsl.sat =  lpSLut[ hsl.sat ];
		hsl.lum =  lpMLut[ hsl.lum ];
		HSLtoCMYK( hsl.hue, hsl.sat, hsl.lum, &cmyk );
		*lpC = cmyk.c;
		*lpM = cmyk.m;
		*lpY = cmyk.y;
		*lpK = cmyk.k;
	}
	else
	{
		*lpC = 255 - lpRLut[ 255-lpMLut[*lpC] ];
		*lpM = 255 - lpGLut[ 255-lpMLut[*lpM] ];
		*lpY = 255 - lpBLut[ 255-lpMLut[*lpY] ];
		*lpK = lpMLut[ *lpK ];
	}
}

/***********************************************************************/

BOOL AnimateMap( LPMAP lpMap, BOOL fDoGamma)
{
	int i;
	LPPALETTEENTRY lpEntry, lpNewEntry;
	PALETTEENTRY TempEntries[256+1];
	BOOL bPreview;

	if ( lpMap )
		MakeMap( lpMap );
	
	if ( !lpImage )
		return( NO );

	if ( !lpImage->hPal )
		return( NO );

	if ( lpBltScreen->BitMapInfo.bmiHeader.biBitCount != 8 )
		return( NO );

	if ( !(bPreview = Previewing()) )
	{
		MapSetupLuts(
			HueMap.Lut,
			SatMap.Lut,
			MasterMap.Lut,
			RedMap.Lut,
			GreenMap.Lut,
			BlueMap.Lut);
	}

	lpEntry = lpImage->Palette;
	lpNewEntry = TempEntries;

	for ( i=0; i<lpImage->nPaletteEntries; i++ )
	{
		*lpNewEntry = *lpEntry;
		if ( !bPreview )
			MapRGB(
				&lpNewEntry->peRed,
				&lpNewEntry->peGreen,
				&lpNewEntry->peBlue );
		lpEntry++;
		lpNewEntry++;
	}

	if (fDoGamma)
	{
		GammaAdjustPalette(
			lpBltScreen, lpImage->hPal,	lpImage->nPaletteEntries, TempEntries);
	}
	else
	{
		AnimatePalette(
			lpImage->hPal, 0, lpImage->nPaletteEntries, TempEntries );
	}

	return( YES );
}

/***********************************************************************/

void MapPreview()
{
	RECT rMask;

	if ( !lpImage )
		return;

	SetImageHook(lpImage->hWnd, MapPreviewProc);
	AnimateMap(NULL, YES);

	MapSetupLuts(
		HueMap.Lut,
		SatMap.Lut,
		MasterMap.Lut,
		RedMap.Lut,
		GreenMap.Lut,
		BlueMap.Lut);

	if (ImgGetMask(lpImage))
		ImgGetMaskRect(lpImage, &rMask);
	else
		ImgGetSelObjectRect(lpImage, &rMask, NO);

	File2DispRect(&rMask, &rMask);
	InvalidateRect(lpImage->hWnd, &rMask, FALSE);
}

/***********************************************************************/

void UndoMapPreview()
{
	RECT rMask;

	if ( !lpImage )
		return;

	if ( !Previewing() )
		return;

	MapSetupLuts(NULL, NULL, NULL, NULL, NULL, NULL);

	SetImageHook(lpImage->hWnd, NULL);

	if (ImgGetMask(lpImage))
		ImgGetMaskRect(lpImage, &rMask);
	else
		ImgGetSelObjectRect(lpImage, &rMask, NO);

	File2DispRect(&rMask, &rMask);
	InvalidateRect(lpImage->hWnd, &rMask, FALSE);
}

/************************************************************************/

static void MapPreviewProc(
	int    x,
	int    y,
	int    dx,
	int    depth,
	LPTR   lpSrc,
	LPTR   lpDst,
	LPTR   lpMsk,
	LPMASK lpMask)
{
	void (far *lpMapProc)(LPTR,LPTR,LPTR,int);

	switch ( depth )
	{
		case 0:
		case 1:
			lpMapProc = Map8;
		break;

		case 3:
			lpMapProc = Map24;
		break;

		case 4:
			lpMapProc = Map32;
		break;

		default:
			return;
		break;
	}

	mload(x, y, dx, 1, lpMsk, lpDst, depth, lpMask);
	(*lpMapProc)( lpDst, lpSrc, lpMsk, dx );
}

/************************************************************************/

static void Map8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int dx)
{
	int iCount;
	LPTR lpGray;
	LPPROCESSPROC lpProcessProc;

	if (lpMsk)
	{
		copy(lpDst, lpSrc, dx);
		lpGray = lpSrc;
		iCount = dx;
		while (--iCount >= 0)
		{
			MapGray( lpGray );
			++lpGray;
		}
		lpProcessProc = GetProcessProc(MM_NORMAL, 1);
		(*lpProcessProc)(lpDst, lpSrc, lpMsk, dx);
	}
	else if (!HueMap.bModified && !SatMap.bModified && lpHLut)
	{
		// Quick and dirty special case
		lpGray = lpDst;
		iCount = dx;
		while (--iCount >= 0)
			*lpGray++ = lpGLut[ 255-lpMLut[255-*lpGray] ];
	}
	else
	{
		lpGray = lpDst;
		iCount = dx;
		while (--iCount >= 0)
		{
			MapGray(lpGray);
			++lpGray;
		}
	}
}

/************************************************************************/

static void Map24(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int dx)
{
	LPRGB lpRGB;
	int iCount;
	LPPROCESSPROC lpProcessProc;

	if (lpMsk)
	{
		copy(lpDst, lpSrc, dx*3);
		lpRGB = (LPRGB)lpSrc;
		iCount = dx;
		while (--iCount >= 0)
		{
			MapRGB(&lpRGB->red, &lpRGB->green, &lpRGB->blue);
			++lpRGB;
		}
		lpProcessProc = GetProcessProc(MM_NORMAL, 3);
		(*lpProcessProc)(lpDst, lpSrc, lpMsk, dx);
	}
	else if (!HueMap.bModified && !SatMap.bModified && lpHLut)
	{
		// Quick and dirty special case
		iCount = dx;
		while (--iCount >= 0)
		{
			*lpDst++ = lpRLut[ 255-lpMLut[255-*lpDst] ];
			*lpDst++ = lpGLut[ 255-lpMLut[255-*lpDst] ];
			*lpDst++ = lpBLut[ 255-lpMLut[255-*lpDst] ];
			++lpRGB;
		}
	}
	else
	{
		lpRGB = (LPRGB)lpDst;
		iCount = dx;
		while (--iCount >= 0)
		{
			MapRGB(&lpRGB->red, &lpRGB->green, &lpRGB->blue);
			++lpRGB;
		}
	}
}

/************************************************************************/

static void Map32(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int dx)
{
	LPCMYK lpCMYK;
	int iCount;
	LPPROCESSPROC lpProcessProc;

	if (lpMsk)
	{
		copy(lpDst, lpSrc, dx*4);
		lpCMYK = (LPCMYK)lpSrc;
		iCount = dx;
		while (--iCount >= 0)
		{
			MapCMYK(&lpCMYK->c, &lpCMYK->m, &lpCMYK->y, &lpCMYK->k);
			++lpCMYK;
		}
		lpProcessProc = GetProcessProc(MM_NORMAL, 4);
		(*lpProcessProc)(lpDst, lpSrc, lpMsk, dx);
	}
	else if (!HueMap.bModified && !SatMap.bModified && lpHLut)
	{
		// Quick and dirty special case
		iCount = dx;
		while (--iCount >= 0)
		{
			*lpDst++ = 255 - lpRLut[ 255-lpMLut[*lpDst] ];
			*lpDst++  = 255 - lpGLut[ 255-lpMLut[*lpDst] ];
			*lpDst++  = 255 - lpBLut[ 255-lpMLut[*lpDst] ];
			*lpDst++  = lpMLut[ *lpDst ];
		}
	}
	else
	{
		lpCMYK = (LPCMYK)lpDst;
		iCount = dx;
		while (--iCount >= 0)
		{
			MapCMYK(&lpCMYK->c, &lpCMYK->m, &lpCMYK->y, &lpCMYK->k);
			++lpCMYK;
		}
	}
}

/************************************************************************/

BOOL Previewing()
{
	if (!lpImage)
		return(FALSE);
	return(GetImageHook(lpImage->hWnd) == MapPreviewProc);
}

/************************************************************************\
 	Displays the given portion of the frame at the given location.
 	DOES NO DOWNSIZING
	Applys the given luts to the image when displaying.
 	lpOutBuffer should be NULL or should be large enough for one full line;
 	Should setup Luts before calling
\************************************************************************/

int DisplayMappedImage(
	HDC     hDC,
	LPFRAME lpFrame,
	LPRECT  lpSourceRect,
	POINT   destPoint,
	LPTR    lpOutBuffer,
	int     xDiva,
	int     yDiva)
{
#define PRLINES 20

	int y, count, depth;
	RECT rSource, rDest;
	LPTR lpLine;
	BLTSESSION BltSession;  
#ifdef STATIC16  // Only in the new framelib
	FRMTYPEINFO TypeInfo;
#endif
	
	depth = FrameDepth(lpFrame);
	if (depth == 0) depth = 1;

	rSource      = *lpSourceRect;
	rDest.left   = destPoint.x;
	rDest.top    = destPoint.y;
	rDest.right  = rDest.left+(rSource.right -rSource.left);
	rDest.bottom = rDest.top +(rSource.bottom-rSource.top);

	if (!lpOutBuffer)
		if (!(lpOutBuffer = Alloc((long)RectWidth( &rSource )* depth)) )
			return(FALSE);
	
	count = RectWidth(&rDest);

#ifdef STATIC16 // Only in the new framelib
FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);
StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &rDest,
	TypeInfo, PRLINES, xDiva, yDiva, YES, NULL );
#else
	StartSuperBlt(&BltSession, hDC, NULL, lpBltScreen, &rDest,
		depth, PRLINES, xDiva, yDiva, YES, NULL );
#endif

	for ( y=rSource.top; y<=rSource.bottom; y++ )
	{
		if (lpLine = FramePointer(lpFrame, rSource.left, y, NO))
		{
			switch(depth)
			{
				case 1:
					copy(lpLine,lpOutBuffer,count);  
					Map8(lpOutBuffer,NULL,NULL,count);
				break;

				case 3:
					copy(lpLine,lpOutBuffer,count*depth);  
					Map24(lpOutBuffer,NULL,NULL,count);
				break;

				case 4:
					copy(lpLine,lpOutBuffer,count*depth);  
					Map32(lpOutBuffer,NULL,NULL,count);
				break;

				default:
					map_proc(
						y,
						rSource.left,
						rSource.right,
						lpLine,
						lpOutBuffer,
						depth);
				break;
			}
		}
		SuperBlt( &BltSession, lpOutBuffer );
	}

	SuperBlt( &BltSession, NULL );

	return( TRUE );
}
