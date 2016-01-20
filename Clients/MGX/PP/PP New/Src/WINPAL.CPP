//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1995 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"


// Static Prototypes
static HPALETTE CopySystemPalette( void );
static void Map8P(  LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int iCount );




static HPALETTE hSystemPalette;

/************************************************************************/

void SaveSystemPalette(HWND hWnd)
{
	if ( hSystemPalette )
		RestoreSystemPalette( hWnd );

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
	lpPal->palNumEntries = 226;
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
void MapLine24HSL(LPTR lpH,LPTR lpS,LPTR lpL, LPTR lpData, int count)
/************************************************************************/
{
	HSLS hsl;
	while (count--) 
	{
		RGBtoHSL( *lpData, lpData[1], lpData[2], &hsl );
		hsl.hue =  lpH[ hsl.hue ];
		hsl.sat =  lpS[ hsl.sat ];
		hsl.lum =  lpL[ hsl.lum ];
		HSLtoRGB( hsl.hue, hsl.sat, hsl.lum, (LPRGB)lpData);
		lpData+=3;
	}
}


/************************************************************************/
void MapLine32HSL(LPTR lpH,LPTR lpS,LPTR lpL, LPTR lpData, int count)
/************************************************************************/
{
	HSLS hsl;
	while (count--) 
	{
		CMYKtoHSL( *lpData, lpData[1], lpData[2], lpData[3], &hsl );
		hsl.hue =  lpH[ hsl.hue ];
		hsl.sat =  lpS[ hsl.sat ];
		hsl.lum =  lpL[ hsl.lum ];
		HSLtoCMYK( hsl.hue, hsl.sat, hsl.lum, (LPCMYK)lpData);
		lpData+=4;
	}
}

	




#ifdef C_CODE
//the following MapLine* functions are clean!
/************************************************************************/
void MapLine8(LPTR lpMLut, LPTR lpData, int count)
/************************************************************************/
{
	while (count--) 
	{
		*lpData = 255-lpMLut[ 255-*lpData ];
		*lpData++;
	}
}

/************************************************************************/
void MapLine8by2(LPTR lpMLut, LPTR lpLut, LPTR lpData, int count)
/************************************************************************/
{
	while (count--) 
	{
		*lpData = lpLut[ 255-lpMLut[255-*lpData] ];
		*lpData++;
	}
}

/************************************************************************/
void MapLine24(LPTR lpMLut,LPTR lpRLut,LPTR lpGLut, LPTR lpBLut, LPTR lpData, int count)
/************************************************************************/
{
	while (count--) 
	{
		*lpData   = lpRLut[ 255-lpMLut[255-*lpData]   ];
		*lpData++;
		*lpData   = lpGLut[ 255-lpMLut[255-*lpData]   ];
		*lpData++;
		*lpData   = lpBLut[ 255-lpMLut[255-*lpData]   ];
		*lpData++;
	}
}

/************************************************************************/
void MapLine32(LPTR lpMLut,LPTR lpRLut,LPTR lpGLut, LPTR lpBLut, LPTR lpKLut, LPTR lpData, int count)
/************************************************************************/
{
	while (count--) 
	{
		*lpData   = 255 - lpRLut[ 255-lpMLut[*lpData] ];
		*lpData++;
		*lpData   = 255 - lpGLut[ 255-lpMLut[*lpData] ];
		*lpData++;
		*lpData   = 255 - lpBLut[ 255-lpMLut[*lpData] ];
		*lpData++;
		*lpData   = lpKLut[*lpData];
		*lpData++;
	}
}

/************************************************************************/
void MapLine32MToK(LPTR lpMLut,LPTR lpRLut,LPTR lpGLut, LPTR lpBLut, LPTR lpKLut, LPTR lpData, int count)
/************************************************************************/
{
	while (count--) 
	{
		*lpData   = 255 - lpRLut[ 255-lpMLut[*lpData] ];
		*lpData++;
		*lpData   = 255 - lpGLut[ 255-lpMLut[*lpData] ];
		*lpData++;
		*lpData   = 255 - lpBLut[ 255-lpMLut[*lpData] ];
		*lpData++;
		*lpData   = lpKLut[lpMLut[*lpData]];
		*lpData++;
	}
}



#endif
