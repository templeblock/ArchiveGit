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

static HDC hDC, hMemDC;
static HANDLE hBitmap;
static BLT Blt;
static LPTR lpBltBuf, lpError;
static int dxConverted, xBlt, yBlt, dyBlt, dxBlt, LinesInBuffer, xDiva, yDiva;
static WORD wUsage;
static LPROC fpConvert;
static HPALETTE hPal, hOldPal;
static BOOL bDIB;

//#define METHOD (Text.Size>2?0:Text.Size)
#define METHOD 0

/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)	((i+31)/32*4)

/***********************************************************************/
void InitSuperBlt( hDC, lpBlt, nTotalBits )
/***********************************************************************/
HDC hDC;
LPBLT lpBlt;
int nTotalBits;
{
int nColors, nEntries, nBits, nPlanes, nReserved, fPal;
//WORD GetPal, SetPal;

fPal = GetDeviceCaps( hDC, RASTERCAPS ) & RC_PALETTE;
bDIB = GetDeviceCaps( hDC, RASTERCAPS ) & RC_DIBTODEV;
nColors = GetDeviceCaps( hDC, NUMCOLORS );
nEntries = GetDeviceCaps( hDC, SIZEPALETTE );
nBits = GetDeviceCaps( hDC, BITSPIXEL );
nPlanes = GetDeviceCaps( hDC, PLANES );
nReserved = GetDeviceCaps( hDC, NUMRESERVED );
//SetPal = SetSystemPaletteUse( hDC, SYSPAL_NOSTATIC );
//GetPal = GetSystemPaletteUse( hDC );
//SetSystemPaletteUse( hDC, SetPal );
//Print("pal=%d,dib=%d,colors=%d,entries=%d,planes=%d,bits=%d,reserved=%d,setpal=%d,getpal=%d",
//	fPal,bDIB,nColors,nEntries,nPlanes,nBits,nReserved,SetPal,GetPal );

//for ( i=0; i<20; i++ )
//	{ // This is only the Windows interface colors
//	Color = GetSysColor(i);
//	Print("Color %d (%d,%d,%d)",
//		i, GetRValue(Color), GetGValue(Color), GetBValue(Color) );
//	Color = RGB( 255-GetRValue(Color), 255-GetGValue(Color),
//		255-GetBValue(Color) );
//	SetSysColors( 1, &i, &Color );
//	}
//
//for ( i=0; i<20; i++ )
//	{ // This does nothing on 16 color displays
//	n = GetSystemPaletteEntries( hDC, i, 1, &Entry );
//	Print("GetSystemPaletteEntries %d returns %d (%d,%d,%d,%d)", i, n,
//		Entry.peRed, Entry.peGreen, Entry.peBlue, Entry.peFlags );
//	}

dbg("nTotalBits = %d nBits = %d nPlanes = %d nColors = %d nEntries = %d",
nTotalBits, nBits, nPlanes, nColors, nEntries);dbg(NULL);
if ( !nTotalBits )
	{
	nTotalBits = nBits * nPlanes;
	}

if ( nTotalBits <= 4 )
	Init16ColorDevice( lpBlt );
else
if ( nTotalBits <= 8 )
	Init256ColorDevice( lpBlt );
//else
//if ( nTotalBits <= 16)
//    	Init16BitDevice( lpBlt );
else
	Init24BitDevice( lpBlt );
}


/***********************************************************************/
static void Init16BitDevice( lpBlt )
/***********************************************************************/
LPBLT lpBlt;
{
lpBlt->BitMapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
lpBlt->BitMapInfo.bmiHeader.biWidth = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biHeight = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biPlanes = 1; // Always set to 1
lpBlt->BitMapInfo.bmiHeader.biBitCount = 16; // Always 1, 4, 8, or 24
lpBlt->BitMapInfo.bmiHeader.biCompression = BI_RGB; // Uncompressed
lpBlt->BitMapInfo.bmiHeader.biSizeImage = 0L;
lpBlt->BitMapInfo.bmiHeader.biXPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biYPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biClrUsed = 0;
lpBlt->BitMapInfo.bmiHeader.biClrImportant = 0; // 0 = all colors are important

/* stuff the bmiColor entries */
lpBlt->BitMapInfo.bmi.Index[0] = 0;

lpBlt->hPal = NULL;
lpBlt->hGrayPal = Create256GrayPalette();
lpBlt->hColorPal = Create256ColorPalette();
lpBlt->fpConvert1 = Convert8to16;
lpBlt->fpConvert2 = Convert16to16;
lpBlt->fpConvert3 = Convert24to16;
lpBlt->fpConvert1C = Convert8to16;
lpBlt->fpConvert2G = Convert16to16;
lpBlt->fpConvert3G = Convert24to16;
}


/***********************************************************************/
static void Init24BitDevice( lpBlt )
/***********************************************************************/
LPBLT lpBlt;
{
int i;
#undef NCOLORS
#define NCOLORS 256

lpBlt->BitMapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
lpBlt->BitMapInfo.bmiHeader.biWidth = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biHeight = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biPlanes = 1; // Always set to 1
lpBlt->BitMapInfo.bmiHeader.biBitCount = 24; // Always 1, 4, 8, or 24
lpBlt->BitMapInfo.bmiHeader.biCompression = BI_RGB; // Uncompressed
lpBlt->BitMapInfo.bmiHeader.biSizeImage = 0L;
lpBlt->BitMapInfo.bmiHeader.biXPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biYPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biClrUsed = 0;
lpBlt->BitMapInfo.bmiHeader.biClrImportant = 0; // 0 = all colors are important

/* stuff the bmiColor entries */
lpBlt->BitMapInfo.bmi.Index[0] = 0;

lpBlt->hPal = NULL;
lpBlt->hGrayPal = NULL;
lpBlt->hColorPal = NULL;
lpBlt->fpConvert1 = Convert8to24;
lpBlt->fpConvert2 = Convert16to24;
lpBlt->fpConvert3 = Convert24to24;
lpBlt->fpConvert1C = Convert8to24;
lpBlt->fpConvert2G = Convert16to24;
lpBlt->fpConvert3G = Convert24to24;
}


/***********************************************************************/
static void Init256ColorDevice( lpBlt )
/***********************************************************************/
LPBLT lpBlt;
{
int i;
#undef NCOLORS
#define NCOLORS 256

lpBlt->BitMapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
lpBlt->BitMapInfo.bmiHeader.biWidth = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biHeight = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biPlanes = 1; // Always set to 1
lpBlt->BitMapInfo.bmiHeader.biBitCount = 8; // Always 1, 4, 8, or 24
lpBlt->BitMapInfo.bmiHeader.biCompression = BI_RGB; // Uncompressed
lpBlt->BitMapInfo.bmiHeader.biSizeImage = 0L;
lpBlt->BitMapInfo.bmiHeader.biXPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biYPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biClrUsed = NCOLORS;
lpBlt->BitMapInfo.bmiHeader.biClrImportant = 0; // 0 = all colors are important

/* stuff the bmiColor entries */
for(i=0; i<NCOLORS; i++)
	lpBlt->BitMapInfo.bmi.Index[i] = i;

lpBlt->hPal = NULL;
lpBlt->hGrayPal = Create256GrayPalette();
lpBlt->hColorPal = Create256ColorPalette();
lpBlt->fpConvert1 = Convert8to8;
lpBlt->fpConvert2 = Convert16to8;
lpBlt->fpConvert3 = Convert24to8;
lpBlt->fpConvert1C = Convert8to8C;
lpBlt->fpConvert2G = Convert16to8G;
lpBlt->fpConvert3G = Convert24to8G;
}


/***********************************************************************/
static void Init16ColorDevice( lpBlt )
/***********************************************************************/
LPBLT lpBlt;
{
int i;
#undef NCOLORS
#define NCOLORS 16

lpBlt->BitMapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
lpBlt->BitMapInfo.bmiHeader.biWidth = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biHeight = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biPlanes = 1; // Always set to 1
lpBlt->BitMapInfo.bmiHeader.biBitCount = 4; // Always 1, 4, 8, or 24
lpBlt->BitMapInfo.bmiHeader.biCompression = BI_RGB; // Uncompressed
lpBlt->BitMapInfo.bmiHeader.biSizeImage = 0L;
lpBlt->BitMapInfo.bmiHeader.biXPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biYPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biClrUsed = NCOLORS;
lpBlt->BitMapInfo.bmiHeader.biClrImportant = 0; // 0 = all colors are important

/* stuff the bmiColor entries */
for(i=0; i<NCOLORS; i++)
	lpBlt->BitMapInfo.bmi.Index[i] = i;

lpBlt->hPal = NULL;
//lpBlt->hGrayPal = Create16GrayPalette();
lpBlt->hColorPal = Create16ColorPalette();
lpBlt->hGrayPal = lpBlt->hColorPal;
lpBlt->fpConvert1 = Convert8to4;
lpBlt->fpConvert2 = Convert16to4;
lpBlt->fpConvert3 = Convert24to4;
lpBlt->fpConvert1C = Convert8to4;
lpBlt->fpConvert2G = Convert16to4;
lpBlt->fpConvert3G = Convert24to4;
}


/***********************************************************************/
static HPALETTE Create256GrayPalette()
/***********************************************************************/
{
LPLOGPALETTE lpPal;
LPPALETTEENTRY lpEntry;
WORD r, g, b, w;
PALETTEENTRY TempEntries[256+1];

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion = 0x300;
lpPal->palNumEntries = 226;
lpEntry = lpPal->palPalEntry;

/* create the palette with 101 grays and 125 colors */
for ( w=0; w<101; w++ )
	{
	g = ((w * 255) + 50) / 100;
	lpEntry->peRed   = g;
	lpEntry->peGreen = g;
	lpEntry->peBlue  = g;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

for ( r=0; r<5; r++ )
  for ( g=0; g<5; g++ )
    for ( b=0; b<5; b++ )
	{
	lpEntry->peRed   = min(r * 64, 255);
	lpEntry->peGreen = min(g * 64, 255);
	lpEntry->peBlue  = min(b * 64, 255);
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

return( CreatePalette( lpPal ) );
}


/***********************************************************************/
static HPALETTE Create256ColorPalette()
/***********************************************************************/
{
LPLOGPALETTE lpPal;
LPPALETTEENTRY lpEntry;
WORD r, g, b, w;
PALETTEENTRY TempEntries[256+1];

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion = 0x300;
lpPal->palNumEntries = 232;
lpEntry = lpPal->palPalEntry;

/* create the palette with 216 colors and 16 grays */
for ( r=0; r<6; r++ )
  for ( g=0; g<6; g++ )
    for ( b=0; b<6; b++ )
	{
	lpEntry->peRed   = r * 51;
	lpEntry->peGreen = g * 51;
	lpEntry->peBlue  = b * 51;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

for ( w=0; w<16; w++ )
	{
	g = min(w * 16, 255);
	lpEntry->peRed   = g;
	lpEntry->peGreen = g;
	lpEntry->peBlue  = g;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

return( CreatePalette( lpPal ) );
}


/***********************************************************************/
static HPALETTE Create16GrayPalette()
/***********************************************************************/
{
LPLOGPALETTE lpPal;
LPPALETTEENTRY lpEntry;
PALETTEENTRY TempEntries[16+1];

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion = 0x300;
lpPal->palNumEntries = 16;
lpEntry = lpPal->palPalEntry;

/* create the palette */
lpEntry->peRed = lpEntry->peGreen = lpEntry->peBlue = 0;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = lpEntry->peGreen = lpEntry->peBlue = 128;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = lpEntry->peGreen = lpEntry->peBlue = 192;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = lpEntry->peGreen = lpEntry->peBlue = 255;
lpEntry->peFlags = PC_RESERVED;

return( CreatePalette( lpPal ) );
}


/***********************************************************************/
static HPALETTE Create16ColorPalette()
/***********************************************************************/
{
LPLOGPALETTE lpPal;
LPPALETTEENTRY lpEntry;
PALETTEENTRY TempEntries[16+1];

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion = 0x300;
lpPal->palNumEntries = 16;
lpEntry = lpPal->palPalEntry;

/* create the palette */
#ifdef UNUSED
lpEntry->peRed = 0; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 1; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 2; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 3; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 4; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 5; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 6; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 7; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 8; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 9; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 10; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 11; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 12; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 13; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 14; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
lpEntry->peRed = 15; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_EXPLICIT; lpEntry++;
#endif
#ifndef UNUSED
/* create the palette */
lpEntry->peRed = 0; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 128; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 0; lpEntry->peGreen = 128; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 128; lpEntry->peGreen = 128; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 0; lpEntry->peGreen = 0; lpEntry->peBlue = 128;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 128; lpEntry->peGreen = 0; lpEntry->peBlue = 128;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 0; lpEntry->peGreen = 128; lpEntry->peBlue = 128;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 192; lpEntry->peGreen = 192; lpEntry->peBlue = 192;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 128; lpEntry->peGreen = 128; lpEntry->peBlue = 128;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 255; lpEntry->peGreen = 0; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 0; lpEntry->peGreen = 255; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 255; lpEntry->peGreen = 255; lpEntry->peBlue = 0;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 0; lpEntry->peGreen = 0; lpEntry->peBlue = 255;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 255; lpEntry->peGreen = 0; lpEntry->peBlue = 255;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 0; lpEntry->peGreen = 255; lpEntry->peBlue = 255;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed = 255; lpEntry->peGreen = 255; lpEntry->peBlue = 255;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
#endif

return( CreatePalette( lpPal ) );
}


/***********************************************************************/
void StartSuperBlt( hDCDevice, lpBltDevice, lpRect, Depth, MaxLines, xDivaStart, yDivaStart )
/***********************************************************************/
HDC hDCDevice;
LPBLT lpBltDevice;
LPRECT lpRect;
int Depth, MaxLines, xDivaStart, yDivaStart;
{
int dxError;

hDC = hDCDevice;
Blt = *lpBltDevice;
xBlt = lpRect->left;
yBlt = lpRect->top;
dxBlt = RectWidth( lpRect );
dyBlt = 0;
lpBltBuf = LineBuffer[0];
lpBltBuf += 65500L;

switch ( Blt.BitMapInfo.bmiHeader.biBitCount )
    {
    case 24:
	wUsage = DIB_RGB_COLORS;
	dxConverted = dxBlt * 3;
	break;
    case 8:
	wUsage = DIB_PAL_COLORS;
	dxConverted = dxBlt;
	break;
    case 4:
	wUsage = DIB_PAL_COLORS;
	dxConverted = (dxBlt+1)/2;
	break;
    }

if ( !METHOD || METHOD == 1 ) // not correct packing for 1 and 2
	dxConverted = 4 * ((dxConverted + 3) / 4); // DIB packing
else	dxConverted = 2 * ((dxConverted + 1) / 2); // DDB packing

LinesInBuffer = 65500L/(long)dxConverted;
if ( MaxLines )
    if ( LinesInBuffer > MaxLines )
	LinesInBuffer = MaxLines;

if ( METHOD == 1 || METHOD == 2 )
	{
	hMemDC= CreateCompatibleDC( hDC );
	if ( METHOD == 2 )
		hBitmap = CreateCompatibleBitmap( hDC, dxBlt, LinesInBuffer );
	}

hPal = Blt.hPal;
switch ( Depth )
    {
    case 1:
	if ( !hPal ) // Always use the image palette if there is one
		hPal = Blt.hGrayPal;
        if (hPal == Blt.hGrayPal)
	    fpConvert = Blt.fpConvert1;
	else
   	    fpConvert = Blt.fpConvert1C;
	dxError = (dxBlt+1) * sizeof(int) * sizeof(BYTE);
	break;
    case 2:
	if ( !hPal ) // Always use the image palette if there is one
		hPal = Blt.hColorPal;
	if (hPal == Blt.hColorPal)
	    fpConvert = Blt.fpConvert2;
	else
	    fpConvert = Blt.fpConvert2G;
	dxError = (dxBlt+1) * sizeof(int) * sizeof(RGBS);
	break;
    case 3:
	if ( !hPal ) // Always use the image palette if there is one
		hPal = Blt.hColorPal;
	if (hPal == Blt.hColorPal)
	    fpConvert = Blt.fpConvert3;
	else
	    fpConvert = Blt.fpConvert3G;
	dxError = (dxBlt+1) * sizeof(int) * sizeof(RGBS);
	break;
    }

//UnrealizeObject( hPal );
if (hPal)
    {
    hOldPal = SelectPalette( hDC, hPal, FALSE );
    RealizePalette( hDC );
    }
if ( !AllocLines( &lpError, 1, dxError, 1 /*depth*/ ) )
	{
	Message( IDS_EMEMALLOC );
	return;
	}
clr( lpError, dxError );
xDiva = xDivaStart + xBlt;
yDiva = yDivaStart + yBlt;

}


/***********************************************************************/
void SuperBlt( lp )
/***********************************************************************/
LPTR lp;
{
HANDLE hOldBitmap;
HPALETTE hOldMemPal;

if ( dyBlt == LinesInBuffer || !lp )
	{
	Blt.BitMapInfo.bmiHeader.biWidth = dxBlt;
	Blt.BitMapInfo.bmiHeader.biHeight = dyBlt;
//	Blt.BitMapInfo.bmiHeader.biSizeImage =
//	    WIDTHBYTES(dxBlt*Blt.BitMapInfo.bmiHeader.biBitCount) * dyBlt;

	if ( !METHOD )
		{
		SetDIBitsToDevice( hDC, xBlt, yBlt, dxBlt, dyBlt, 0, 0,
		 0, dyBlt, lpBltBuf, (LPBITMAPINFO)&Blt.BitMapInfo, wUsage );
		lpBltBuf = LineBuffer[0] + 65500L;
		}
	else
	if ( METHOD == 1 )
		{
		hBitmap = CreateDIBitmap( hDC,
		    &Blt.BitMapInfo.bmiHeader, CBM_INIT, lpBltBuf,
		    (LPBITMAPINFO)&Blt.BitMapInfo, wUsage );
		if ( !hBitmap )
			PrintStatus("CreateBitmap is NULL");
//		hOldMemPal = SelectPalette( hMemDC, hPal, FALSE );
		hOldBitmap = SelectObject( hMemDC, hBitmap );
		BitBlt( hDC, xBlt, yBlt, dxBlt, dyBlt, hMemDC, 0, 0, SRCCOPY );
		SelectObject( hMemDC, hOldBitmap );
//		SelectPalette( hMemDC, hOldMemPal, FALSE );
		DeleteObject( hBitmap );
		lpBltBuf = LineBuffer[0] + 65500L;
		}
	else
	if ( METHOD == 2 )
		{
//		hOldMemPal = SelectPalette( hMemDC, hPal, FALSE );
		SetDIBits( hMemDC, hBitmap, 0, dyBlt, lpBltBuf,
			(LPBITMAPINFO)&Blt.BitMapInfo, wUsage );
		hOldBitmap = SelectObject( hMemDC, hBitmap );
		BitBlt( hDC, xBlt, yBlt, dxBlt, dyBlt, hMemDC, 0, 0, SRCCOPY );
		SelectObject( hMemDC, hOldBitmap );
//		SelectPalette( hMemDC, hOldMemPal, FALSE );
		lpBltBuf = LineBuffer[0] + 65500L;
		}

	yBlt += dyBlt;
	dyBlt = 0;
	}

if ( !lp )
	{
	if ( lpError )
		FreeUp( lpError );
	lpError = NULL;
//	SelectPalette( hDC, hOldPal, FALSE );
	if ( METHOD == 1 || METHOD == 2 )
		{
		DeleteDC( hMemDC );
		if ( METHOD == 2 )
			DeleteObject( hBitmap );
		}
	}
else	{
	lpBltBuf -= dxConverted;
	(*fpConvert)( (LPTR)lp, (int)dxBlt, (LPINT)lpError, (LPTR)lpBltBuf,
		xDiva, yDiva );
	dyBlt++;
	yDiva++;
	}
}
