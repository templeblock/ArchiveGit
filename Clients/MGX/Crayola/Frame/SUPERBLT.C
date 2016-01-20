// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "superlib.h"
#include "macros.h"

// Static prototypes
LOCAL BOOL Init24BitDevice(      struct _blt far *lpBlt);
LOCAL BOOL Init256ColorDevice(   struct _blt far *lpBlt);
LOCAL BOOL Init16ColorDevice(    struct _blt far *lpBlt);
LOCAL BOOL Init1BitColorDevice(  struct _blt far *lpBlt);
LOCAL BOOL Init1BitMonoDevice(   struct _blt far *lpBlt);
LOCAL BOOL Init1BitMonoDeviceHT( struct _blt far *lpBlt);

LOCAL HPALETTE Create256GrayPalette( void );
LOCAL HPALETTE Create256ColorPalette( void );
LOCAL HPALETTE Create16GrayPalette( void );
LOCAL HPALETTE Create16ColorPalette( void );

/* have to keep this around */
LOCAL  void BuildGammaLuts(LPBLT);
LOCAL  BOOL GammaAdjustPalettes(LPBLT);
LOCAL  BOOL MapPalette(HPALETTE, WORD, LPPALETTEENTRY, LPTR, LPTR, LPTR, BOOL);

typedef void ( FAR *LCPROC )( LPTR, int, LPINT, LPTR, int, int, struct _bltsession FAR * );      
typedef void ( FAR *LXPROC )( LPTR, LPTR, LPTR, LPTR, LPTR, int ); 

#define METHOD 0

/***********************************************************************/
LPBLT InitSuperBlt( HDC hDC, int nTotalBits, LPTR lpBltBuf, BOOL bPaletteGamma,
                           BOOL bProcessData )
/***********************************************************************/
{
int   nColors, nEntries, nBits, nPlanes;
BOOL  bBinaryPrinter, bUsePrinterScreening, fRet;
LPBLT lpBlt;

lpBlt = (LPBLT)Alloc((long)sizeof(BLT));
if (!lpBlt)
	return(NULL);
clr((LPTR)lpBlt, sizeof(BLT));

// allocate a colormap to be used for doing gamma on
// palette colored images
lpBlt->lpColorMap = FrameCreateColorMap();
if (!lpBlt->lpColorMap)
	{
	FreeUp((LPTR)lpBlt);
	return(NULL);
	}

lpBlt->lpBltBuf      = lpBltBuf;
lpBlt->bDIB          = GetDeviceCaps( hDC, RASTERCAPS ) & RC_DIBTODEV;
lpBlt->bProcessData  = bProcessData;
lpBlt->bPaletteGamma = bPaletteGamma;
lpBlt->RGamma = FUNITY;
lpBlt->GGamma = FUNITY;
lpBlt->BGamma = FUNITY;
lpBlt->nCalls = 0;
#ifdef WIN32
InitializeCriticalSection(&lpBlt->csSuperBlt);
#endif
BuildGammaLuts( lpBlt );

nColors  = GetDeviceCaps( hDC, NUMCOLORS );
nEntries = GetDeviceCaps( hDC, SIZEPALETTE );
nBits    = GetDeviceCaps( hDC, BITSPIXEL );
nPlanes  = GetDeviceCaps( hDC, PLANES );

bUsePrinterScreening = NO;
if (nTotalBits < 0)
	{
	bUsePrinterScreening = YES;
	nTotalBits = -nTotalBits;
	}
// A print call (as opposed to a display call)
if ( nTotalBits )
	{
	bBinaryPrinter = ((nColors <= 2) && ((nBits * nPlanes) <= 1));

	// A scatterprint call
	if ( nTotalBits == 1 )
		{
		if ( bBinaryPrinter )
			fRet = Init1BitMonoDevice( lpBlt );
		else
			fRet = Init1BitColorDevice( lpBlt );
		if (!fRet)
			{
			EndSuperBlt(lpBlt);
			return(NULL);
			}
		return(lpBlt);
		}

	// roll your own halftoning
	if ( bBinaryPrinter )
		{
		// Sets up halftone converters
		fRet = Init1BitMonoDeviceHT( lpBlt );
		if (!fRet)
			{
			EndSuperBlt(lpBlt);
			return(NULL);
			}
		return(lpBlt);
		}
	if (!bUsePrinterScreening)
		nTotalBits = 0; // Fall through to the display cases
	}

if ( nTotalBits < (nBits * nPlanes))
	nTotalBits = nBits * nPlanes;

if ( nTotalBits <= 4 )
	fRet = Init16ColorDevice( lpBlt );
else if ( nTotalBits <= 8 )
	fRet = Init256ColorDevice( lpBlt );
else
	fRet = Init24BitDevice( lpBlt );

if (!fRet)
	{
	EndSuperBlt(lpBlt);
	return(NULL);
	}

return(lpBlt);
}

/***********************************************************************/
void EndSuperBlt( LPBLT lpBlt )
/***********************************************************************/
{
if ( lpBlt->hGrayPal )
	DeleteObject( lpBlt->hGrayPal );

if ( lpBlt->hColorPal && ( lpBlt->hGrayPal != lpBlt->hColorPal ) )
	DeleteObject( lpBlt->hColorPal );

if ( lpBlt->lpGrayPaletteLUT )
	FreeUp(lpBlt->lpGrayPaletteLUT);

if ( lpBlt->lpColorPaletteLUT )
	FreeUp(lpBlt->lpColorPaletteLUT);

#ifdef WIN32
DeleteCriticalSection(&lpBlt->csSuperBlt);
#endif

FreeUp((LPTR)lpBlt);
}

/***********************************************************************/
LOCAL BOOL Init24BitDevice( LPBLT lpBlt )
/***********************************************************************/
{
#undef NCOLORS
#define NCOLORS 256

lpBlt->BitMapInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
lpBlt->BitMapInfo.bmiHeader.biWidth         = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biHeight        = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biPlanes        =  1; // Always set to 1
lpBlt->BitMapInfo.bmiHeader.biBitCount      = 24; // Always 1, 4, 8, or 24
lpBlt->BitMapInfo.bmiHeader.biCompression   = BI_RGB; // Uncompressed
lpBlt->BitMapInfo.bmiHeader.biSizeImage     = 0L;
lpBlt->BitMapInfo.bmiHeader.biXPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biYPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biClrUsed       = 0;
lpBlt->BitMapInfo.bmiHeader.biClrImportant  = 0; // 0 = all important

/* stuff the bmiColor entries */
lpBlt->BitMapInfo.bmi.Index[0] = 0;

lpBlt->hPal          = NULL;
lpBlt->hGrayPal      = NULL;
lpBlt->hColorPal     = NULL;
lpBlt->nGrayEntries  = 0;
lpBlt->nColorEntries = 0;
lpBlt->fpConvert1    = Convert8to24;
lpBlt->fpConvert3    = Convert24to24;
lpBlt->fpConvert4    = Convert32to24;
lpBlt->fpConvert1C   = Convert8to24;
lpBlt->fpConvert3G   = Convert24to24;
lpBlt->fpConvert4G   = Convert32to24;
lpBlt->fpConvert1P   = Convert8Pto24;
lpBlt->fpConvert1PG   = Convert8Pto24;
lpBlt->fpConvert1PC   = Convert8Pto24;
lpBlt->fpConvert1CP   = Convert8to24;
lpBlt->fpConvert3CP   = Convert24to24;
lpBlt->fpConvert4CP   = Convert32to24;

lpBlt->bNeedErrorBuf = NO;
lpBlt->bPaletteGamma = FALSE;
return(TRUE);
}


/***********************************************************************/
LOCAL BOOL Init256ColorDevice( LPBLT lpBlt )
/***********************************************************************/
{
int i;
#undef NCOLORS
#define NCOLORS 256

lpBlt->BitMapInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
lpBlt->BitMapInfo.bmiHeader.biWidth         = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biHeight        = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biPlanes        = 1; // Always set to 1
lpBlt->BitMapInfo.bmiHeader.biBitCount      = 8; // Always 1, 4, 8, or 24
lpBlt->BitMapInfo.bmiHeader.biCompression   = BI_RGB; // Uncompressed
lpBlt->BitMapInfo.bmiHeader.biSizeImage     = 0L;
lpBlt->BitMapInfo.bmiHeader.biXPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biYPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biClrUsed       = NCOLORS;
lpBlt->BitMapInfo.bmiHeader.biClrImportant  = 0; // 0 = all important

/* stuff the bmiColor entries */
for(i=0; i<NCOLORS; i++)
	lpBlt->BitMapInfo.bmi.Index[i] = i;

lpBlt->hPal          = NULL;
lpBlt->hColorPal     = Create256ColorPalette();
lpBlt->ColorPaletteType = PT_COLORPALETTE;
lpBlt->hGrayPal      = Create256GrayPalette();
lpBlt->GrayPaletteType = PT_GRAYPALETTE;

lpBlt->nGrayEntries  =
	GetPaletteEntries(lpBlt->hGrayPal,  0, NCOLORS, lpBlt->GrayPalette);
lpBlt->nColorEntries =
	GetPaletteEntries(lpBlt->hColorPal, 0, NCOLORS, lpBlt->ColorPalette);
#ifdef USE_PALETTELUTS
int i;
RGBS RGBmap[256];

for (i = 0; i < lpBlt->nGrayEntries; ++i)
	{
	RGBmap[i].red = lpBlt->GrayPalette[i].peRed;
	RGBmap[i].green = lpBlt->GrayPalette[i].peGreen;
	RGBmap[i].blue = lpBlt->GrayPalette[i].peBlue;
	}
lpBlt->lpGrayPaletteLUT = CreatePaletteLut15(RGBmap, lpBlt->nGrayEntries, NULL, NULL); 
if (!lpBlt->lpGrayPaletteLUT)
	return(FALSE);
for (i = 0; i < lpBlt->nColorEntries; ++i)
	{
	RGBmap[i].red = lpBlt->ColorPalette[i].peRed;
	RGBmap[i].green = lpBlt->ColorPalette[i].peGreen;
	RGBmap[i].blue = lpBlt->ColorPalette[i].peBlue;
	}
lpBlt->lpColorPaletteLUT = CreatePaletteLut15(RGBmap, lpBlt->nColorEntries, NULL, NULL); 
if (!lpBlt->lpColorPaletteLUT)
	return(FALSE);
#endif

lpBlt->fpConvert1    = Convert8to8;
lpBlt->fpConvert3    = Convert24to8;
lpBlt->fpConvert4    = Convert32to8;
lpBlt->fpConvert1C   = Convert8to8C;
lpBlt->fpConvert3G   = Convert24to8G;
lpBlt->fpConvert4G   = Convert32to8G;
lpBlt->fpConvert1P   = Convert8Pto8;
lpBlt->fpConvert1PG   = Convert8Pto8G;
lpBlt->fpConvert1PC   = Convert8Pto8C;
lpBlt->fpConvert1CP   = Convert8to8CP;
lpBlt->fpConvert3CP   = Convert24to8CP;
lpBlt->fpConvert4CP   = Convert32to8CP;
lpBlt->bNeedErrorBuf = NO;

if (lpBlt->bPaletteGamma)
	GammaAdjustPalettes(lpBlt);
return(TRUE);
}

/***********************************************************************/
LOCAL BOOL Init16ColorDevice( LPBLT lpBlt )
/***********************************************************************/
{
int i;
#undef NCOLORS
#define NCOLORS 16

lpBlt->BitMapInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
lpBlt->BitMapInfo.bmiHeader.biWidth         = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biHeight        = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biPlanes        = 1; // Always set to 1
lpBlt->BitMapInfo.bmiHeader.biBitCount      = 4; // Always 1, 4, 8, or 24
lpBlt->BitMapInfo.bmiHeader.biCompression   = BI_RGB; // Uncompressed
lpBlt->BitMapInfo.bmiHeader.biSizeImage     = 0L;
lpBlt->BitMapInfo.bmiHeader.biXPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biYPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biClrUsed       = NCOLORS;
lpBlt->BitMapInfo.bmiHeader.biClrImportant  = 0; // 0 = all important

/* stuff the bmiColor entries */
for(i=0; i<NCOLORS; i++)
	lpBlt->BitMapInfo.bmi.Index[i] = i;

lpBlt->hPal          = NULL;
lpBlt->hColorPal     = Create16ColorPalette();
lpBlt->hGrayPal      = lpBlt->hColorPal;
lpBlt->ColorPaletteType = PT_COLORPALETTE;
lpBlt->GrayPaletteType = PT_COLORPALETTE;

lpBlt->nGrayEntries  =
	GetPaletteEntries(lpBlt->hGrayPal,  0, NCOLORS, lpBlt->GrayPalette);

lpBlt->nColorEntries =
	GetPaletteEntries(lpBlt->hColorPal, 0, NCOLORS,	lpBlt->ColorPalette);

lpBlt->fpConvert1    = Convert8to4;
lpBlt->fpConvert3    = Convert24to4;
lpBlt->fpConvert4    = Convert32to4;
lpBlt->fpConvert1C   = Convert8to4;
lpBlt->fpConvert3G   = Convert24to4;
lpBlt->fpConvert4G   = Convert32to4;
lpBlt->fpConvert1P   = Convert8Pto4;
lpBlt->fpConvert1PG   = Convert8Pto4;
lpBlt->fpConvert1PC   = Convert8Pto4;
lpBlt->fpConvert1CP   = Convert8to4;
lpBlt->fpConvert3CP   = Convert24to4;
lpBlt->fpConvert4CP   = Convert32to4;
lpBlt->bNeedErrorBuf = NO;
lpBlt->bPaletteGamma = FALSE;
return(TRUE);
}

/***********************************************************************/
LOCAL BOOL Init1BitColorDevice( LPBLT lpBlt )
/***********************************************************************/
{
int i;
#undef NCOLORS
#define NCOLORS 16

lpBlt->BitMapInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
lpBlt->BitMapInfo.bmiHeader.biWidth         = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biHeight        = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biPlanes        = 1; // Always set to 1
lpBlt->BitMapInfo.bmiHeader.biBitCount      = 4; // Always 1, 4, 8, or 24
lpBlt->BitMapInfo.bmiHeader.biCompression   = BI_RGB; // Uncompressed
lpBlt->BitMapInfo.bmiHeader.biSizeImage     = 0L;
lpBlt->BitMapInfo.bmiHeader.biXPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biYPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biClrUsed       = NCOLORS;
lpBlt->BitMapInfo.bmiHeader.biClrImportant  = 0; // 0 = all important

/* stuff the bmiColor entries */
for(i=0; i<NCOLORS; i++)
	lpBlt->BitMapInfo.bmi.Index[i] = i;

lpBlt->hPal         = NULL;
lpBlt->hColorPal    = Create16ColorPalette();
lpBlt->hGrayPal     = lpBlt->hColorPal;
lpBlt->ColorPaletteType = PT_COLORPALETTE;
lpBlt->GrayPaletteType = PT_COLORPALETTE;

lpBlt->nGrayEntries =
	GetPaletteEntries(lpBlt->hGrayPal,  0, NCOLORS, lpBlt->GrayPalette);

lpBlt->nColorEntries =
	GetPaletteEntries(lpBlt->hColorPal, 0, NCOLORS, lpBlt->ColorPalette);

lpBlt->fpConvert1    = Convert8to4S;
lpBlt->fpConvert3    = Convert24to4S;
lpBlt->fpConvert4    = Convert32to4S;
lpBlt->fpConvert1C   = Convert8to4S;
lpBlt->fpConvert3G   = Convert24to4S;
lpBlt->fpConvert4G   = Convert32to4S;
lpBlt->fpConvert1P   = Convert8Pto4S;
lpBlt->fpConvert1PG   = Convert8Pto4S;
lpBlt->fpConvert1PC   = Convert8Pto4S;
lpBlt->fpConvert1CP   = Convert8to4S;
lpBlt->fpConvert3CP   = Convert24to4S;
lpBlt->fpConvert4CP   = Convert32to4S;
lpBlt->bNeedErrorBuf = YES;
lpBlt->bPaletteGamma = FALSE;
return(TRUE);
}

/***********************************************************************/
LOCAL BOOL Init1BitMonoDevice( LPBLT lpBlt )
/***********************************************************************/
{
int i;
#undef NCOLORS
#define NCOLORS 16

lpBlt->BitMapInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
lpBlt->BitMapInfo.bmiHeader.biWidth         = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biHeight        = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biPlanes        = 1; // Always set to 1
lpBlt->BitMapInfo.bmiHeader.biBitCount      = 4; // Always 1, 4, 8, or 24
lpBlt->BitMapInfo.bmiHeader.biCompression   = BI_RGB; // Uncompressed
lpBlt->BitMapInfo.bmiHeader.biSizeImage     = 0L;
lpBlt->BitMapInfo.bmiHeader.biXPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biYPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biClrUsed       = NCOLORS;
lpBlt->BitMapInfo.bmiHeader.biClrImportant  = 0; // 0 = all important

/* stuff the bmiColor entries */
for(i=0; i<NCOLORS; i++)
	lpBlt->BitMapInfo.bmi.Index[i] = i;

lpBlt->hPal         = NULL;
lpBlt->hColorPal    = Create16ColorPalette();
lpBlt->hGrayPal     = lpBlt->hColorPal;
lpBlt->ColorPaletteType = PT_COLORPALETTE;
lpBlt->GrayPaletteType = PT_COLORPALETTE;

lpBlt->nGrayEntries =
	GetPaletteEntries(lpBlt->hGrayPal,  0, NCOLORS, lpBlt->GrayPalette);

lpBlt->nColorEntries =
	GetPaletteEntries(lpBlt->hColorPal, 0, NCOLORS, lpBlt->ColorPalette);

lpBlt->fpConvert1    = Convert8to4S;
lpBlt->fpConvert3    = Convert24to4MS;
lpBlt->fpConvert4    = Convert32to4MS;
lpBlt->fpConvert1C   = Convert8to4S;
lpBlt->fpConvert3G   = Convert24to4MS;
lpBlt->fpConvert4G   = Convert32to4MS;
lpBlt->fpConvert1P   = Convert8Pto4MS;
lpBlt->fpConvert1PG   = Convert8Pto4MS;
lpBlt->fpConvert1PC   = Convert8Pto4MS;
lpBlt->fpConvert1CP   = Convert8to4S;
lpBlt->fpConvert3CP   = Convert24to4MS;
lpBlt->fpConvert4CP   = Convert32to4MS;
lpBlt->bNeedErrorBuf = YES;
lpBlt->bPaletteGamma = FALSE;
return(TRUE);
}

/***********************************************************************/
LOCAL BOOL Init1BitMonoDeviceHT( LPBLT lpBlt )
/***********************************************************************/
{
int i;
#undef NCOLORS
#define NCOLORS 16

lpBlt->BitMapInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
lpBlt->BitMapInfo.bmiHeader.biWidth         = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biHeight        = 0L; // Stuff this later
lpBlt->BitMapInfo.bmiHeader.biPlanes        = 1; // Always set to 1
lpBlt->BitMapInfo.bmiHeader.biBitCount      = 4; // Always 1, 4, 8, or 24
lpBlt->BitMapInfo.bmiHeader.biCompression   = BI_RGB; // Uncompressed
lpBlt->BitMapInfo.bmiHeader.biSizeImage     = 0L;
lpBlt->BitMapInfo.bmiHeader.biXPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biYPelsPerMeter = 0L;
lpBlt->BitMapInfo.bmiHeader.biClrUsed       = NCOLORS;
lpBlt->BitMapInfo.bmiHeader.biClrImportant  = 0; // 0 = all important

/* stuff the bmiColor entries */
for(i=0; i<NCOLORS; i++)
	lpBlt->BitMapInfo.bmi.Index[i] = i;

lpBlt->hPal         = NULL;
lpBlt->hColorPal    = Create16ColorPalette();
lpBlt->hGrayPal     = lpBlt->hColorPal;
lpBlt->ColorPaletteType = PT_COLORPALETTE;
lpBlt->GrayPaletteType = PT_COLORPALETTE;

lpBlt->nGrayEntries =
	GetPaletteEntries(lpBlt->hGrayPal,  0, NCOLORS, lpBlt->GrayPalette);
lpBlt->nColorEntries =
	GetPaletteEntries(lpBlt->hColorPal, 0, NCOLORS, lpBlt->ColorPalette);

lpBlt->fpConvert1    = Convert8to4HT;
lpBlt->fpConvert3    = Convert24to4MHT;
lpBlt->fpConvert4    = Convert32to4MHT;
lpBlt->fpConvert1C   = Convert8to4HT;
lpBlt->fpConvert3G   = Convert24to4MHT;
lpBlt->fpConvert4G   = Convert32to4MHT;
lpBlt->fpConvert1P   = Convert8Pto4MHT;
lpBlt->fpConvert1PG   = Convert8Pto4MHT;
lpBlt->fpConvert1PC   = Convert8Pto4MHT;
lpBlt->fpConvert1CP   = Convert8to4HT;
lpBlt->fpConvert3CP   = Convert24to4MHT;
lpBlt->fpConvert4CP   = Convert32to4MHT;
lpBlt->bNeedErrorBuf = NO;
lpBlt->bPaletteGamma = FALSE;
return(TRUE);
}


/***********************************************************************/
LOCAL HPALETTE Create256GrayPalette()
/***********************************************************************/
{
LPLOGPALETTE   lpPal;
LPPALETTEENTRY lpEntry;
PALETTEENTRY   TempEntries[256+1];
WORD r, g, b, w;

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion = 0x300;
lpPal->palNumEntries = 236;
lpEntry = lpPal->palPalEntry;

// create the palette with 101 grays, 125 colors, and 10 unused entries

// Write out the first 5 unused entries
for ( w=0; w<5; w++ )
	{
	lpEntry->peRed   = 0;
	lpEntry->peGreen = 0;
	lpEntry->peBlue  = 0;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

for ( w=0; w<50; w++ )
	{
	g = ((w * 255) + 50) / 100;
	lpEntry->peRed   = g;
	lpEntry->peGreen = g;
	lpEntry->peBlue  = g;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

for ( r=0; r<5; r++ )
	{
	for ( g=0; g<5; g++ )
		{
		for ( b=0; b<5; b++ )
			{
			lpEntry->peRed   = min(r * 64, 255);
			lpEntry->peGreen = min(g * 64, 255);
			lpEntry->peBlue  = min(b * 64, 255);
			lpEntry->peFlags = PC_RESERVED;
			lpEntry++;
			}
		}
	}

for ( w=50; w<101; w++ )
	{
	g = ((w * 255) + 50) / 100;
	lpEntry->peRed   = g;
	lpEntry->peGreen = g;
	lpEntry->peBlue  = g;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

// Write out the last 5 unused entries
for ( w=0; w<5; w++ )
	{
	lpEntry->peRed   = 0;
	lpEntry->peGreen = 0;
	lpEntry->peBlue  = 0;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

return( CreatePalette( lpPal ) );
}

/***********************************************************************/
HPALETTE CreateLogicalPalette(LPRGB lpRGBmap, int nEntries)
/***********************************************************************/
{
LPLOGPALETTE   lpPal;
LPPALETTEENTRY lpEntry;
PALETTEENTRY   TempEntries[256+1];
int i;

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion = 0x300;
lpPal->palNumEntries = nEntries;
lpEntry = lpPal->palPalEntry;

for (i = 0; i < nEntries; ++i)
	{
	lpEntry->peRed   = lpRGBmap[i].red;
	lpEntry->peGreen = lpRGBmap[i].green;
	lpEntry->peBlue  = lpRGBmap[i].blue;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

return( CreatePalette( lpPal ) );
}

/***********************************************************************/
LOCAL HPALETTE Create256ColorPalette()
/***********************************************************************/
{
LPLOGPALETTE lpPal;
LPPALETTEENTRY lpEntry;
PALETTEENTRY TempEntries[256+1];
WORD r, g, b, w;

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion = 0x300;
lpPal->palNumEntries = 236;
lpEntry = lpPal->palPalEntry;

// create the palette with 216 colors, 16 grays, and 4 unused entries

// Write out the first 2 unused entries
for ( w=0; w<2; w++ )
	{
	lpEntry->peRed   = 0;
	lpEntry->peGreen = 0;
	lpEntry->peBlue  = 0;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

// Write out the first 8 grays
for ( w=0; w<8; w++ )
	{
	g = min(w * 16, 255);
	lpEntry->peRed   = g;
	lpEntry->peGreen = g;
	lpEntry->peBlue  = g;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

// Write out the 216 colors
for ( r=0; r<6; r++ )
	{
	for ( g=0; g<6; g++ )
		{
		for ( b=0; b<6; b++ )
			{
			lpEntry->peRed   = r * 51;
			lpEntry->peGreen = g * 51;
			lpEntry->peBlue  = b * 51;
			lpEntry->peFlags = PC_RESERVED;
			lpEntry++;
			}
		}
	}

// Write out the last 8 grays
for ( w=8; w<16; w++ )
	{
	g = min(w * 16, 255);
	lpEntry->peRed   = g;
	lpEntry->peGreen = g;
	lpEntry->peBlue  = g;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

// Write out the last 2 unused entries
for ( w=0; w<2; w++ )
	{
	lpEntry->peRed   = 0;
	lpEntry->peGreen = 0;
	lpEntry->peBlue  = 0;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

return( CreatePalette( lpPal ) );
}

/***********************************************************************/
LOCAL HPALETTE Create16GrayPalette()
/***********************************************************************/
{
LPLOGPALETTE   lpPal;
LPPALETTEENTRY lpEntry;
PALETTEENTRY   TempEntries[16+1];

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion    = 0x300;
lpPal->palNumEntries = 16;
lpEntry = lpPal->palPalEntry;

/* create the palette */
lpEntry->peRed   = lpEntry->peGreen = lpEntry->peBlue = 0;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed   = lpEntry->peGreen = lpEntry->peBlue = 128;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed   = lpEntry->peGreen = lpEntry->peBlue = 192;
lpEntry->peFlags = PC_RESERVED; lpEntry++;
lpEntry->peRed   = lpEntry->peGreen = lpEntry->peBlue = 255;
lpEntry->peFlags = PC_RESERVED;

return( CreatePalette( lpPal ) );
}


/***********************************************************************/
LOCAL HPALETTE Create16ColorPalette()
/***********************************************************************/
{
STATICTABLE RGBS Table16[16] =
	{	
	  0,   0,   0,
	128,   0,   0,
	  0, 128,   0,
	128, 128,   0,
	  0,   0, 128,
	128,   0, 128,
	  0, 128, 128,
	192, 192, 192,
	128, 128, 128,
	255,   0,   0,
	  0, 255,   0,
	255, 255,   0,
	  0,   0, 255,
	255,   0, 255,
	  0, 255, 255,
	255, 255, 255
	};

int i;
LPLOGPALETTE   lpPal;
LPPALETTEENTRY lpEntry;
PALETTEENTRY   TempEntries[16+1];

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion = 0x300;
lpPal->palNumEntries = 16;
lpEntry = lpPal->palPalEntry;

/* create the palette */

for(i=0;i<16;i++)
	{
	lpEntry->peRed   = Table16[i].red;
	lpEntry->peGreen = Table16[i].green;
	lpEntry->peBlue  = Table16[i].blue;
	lpEntry->peFlags = PC_RESERVED;
	lpEntry++;
	}

return( CreatePalette( lpPal ) );
}

/***********************************************************************/
BOOL SetNewPalette(LPBLT lpBlt, PALETTE_TYPE PaletteType,
					HPALETTE hPal, LPTR lpPaletteLUT,
					LPPALETTEENTRY lpPalette, int nEntries)
/***********************************************************************/
{
// maybe we should do something like seeing if the palette changed
// and if a caller did not pass in lpPaletteLUT, then create on
// by calling CreatePaletteLUT15()
lpBlt->hPal = hPal;
lpBlt->PaletteType = PaletteType;
lpBlt->lpPaletteLUT = lpPaletteLUT;
lpBlt->nEntries = nEntries;

if ( lpBlt->BitMapInfo.bmiHeader.biBitCount != 8 )
	return( TRUE );

if ( !lpBlt->bPaletteGamma )
	return( TRUE );

if (lpBlt->hPal)
	{
	copy((LPTR)lpPalette, (LPTR)lpBlt->Palette, nEntries * sizeof(PALETTEENTRY));
	MapPalette(
		lpBlt->hPal,
		(WORD)lpBlt->nEntries,
		lpBlt->Palette,
		lpBlt->RGammaLut,
		lpBlt->GGammaLut,
		lpBlt->BGammaLut,
		NO);
	}

return(TRUE);
}

/***********************************************************************/
LPBLTSESSION StartSuperBlt( LPBLTSESSION lpSession, HDC hDC, HBITMAP hDestBitmap,
                              LPBLT lpBlt, LPRECT lpRect, FRMTYPEINFO TypeInfo,
                              int MaxLines, int xDivaStart, int yDivaStart,
                              BOOL fDoGamma, LPBLTPROCESS lpProcessProc )
/***********************************************************************/
{
HPALETTE hPal;
PALETTE_TYPE PaletteType;
int      dxError, Depth, nBits;
BOOL     fNeedBuffer;

if (lpSession)
	{
	lpSession->bAllocated = NO;
	}
else
	{
	if (!(lpSession = (LPBLTSESSION)Alloc((long)sizeof(BLTSESSION))))
		return(NULL);
	lpSession->bAllocated = YES;
	}

lpSession->hDC        = hDC;
lpSession->hBitmapOut = hDestBitmap;
lpSession->lpBlt      = lpBlt;
lpSession->BitMapInfo = lpBlt->BitMapInfo;
lpSession->xBlt       = lpRect->left;
lpSession->yBlt       = lpRect->top;
lpSession->dxBlt      = RectWidth( lpRect );
lpSession->dyBlt      = 0;
lpSession->bFailure   = NO;
lpSession->TypeInfo   = TypeInfo;
lpSession->fpMap      = NULL;

#ifdef WIN32
EnterCriticalSection(&lpBlt->csSuperBlt);
#endif

// somebody is already using the standard 64K buffer
if (lpBlt->nCalls != 0)
	{
	// allocate a 64K buffer for this session
	if (!(lpSession->lpBltBufStart = Alloc(65536L)))
		{
		if (lpSession->bAllocated)
			FreeUp((LPTR)lpSession);
		else
			lpSession->bFailure = YES;
#ifdef WIN32
		LeaveCriticalSection(&lpBlt->csSuperBlt);
#endif
		return(NULL);
		}
	lpBlt->nCalls++;
	lpSession->bBufAllocated = YES;
	}
else
	{
	// use the standard 64K buffer, and indicate that it's in use
	lpSession->lpBltBufStart = lpBlt->lpBltBuf;
	lpSession->bBufAllocated = NO;
	lpBlt->nCalls++;
	}

hPal = lpBlt->hPal;
PaletteType = lpBlt->PaletteType;

nBits = lpSession->BitMapInfo.bmiHeader.biBitCount;
switch ( nBits )
	{
	case 24:
		lpSession->wUsage      = DIB_RGB_COLORS;
		lpSession->dxConverted = lpSession->dxBlt * 3;
	break;

	case 8:
		lpSession->wUsage      = DIB_PAL_COLORS;
		lpSession->dxConverted = lpSession->dxBlt;
	break;

	case 4:
		lpSession->wUsage      = DIB_PAL_COLORS;
		lpSession->dxConverted = (lpSession->dxBlt+1)/2;
	break;
	}

// not correct packing for 1 and 2
if ( !METHOD || METHOD == 1 )
	{
	// DIB packing
	lpSession->dxConverted = 4 * ((lpSession->dxConverted + 3) / 4);
	}
else
	{
	// DDB packing
	lpSession->dxConverted = 2 * ((lpSession->dxConverted + 1) / 2);
	}

lpSession->lpBltBuf     =
	lpSession->lpBltBufStart + (65500L - lpSession->dxConverted);

lpSession->iLinesPerBlt =
	(65500L-lpSession->dxConverted)/(long)lpSession->dxConverted;

if ( (MaxLines > 0) && (lpSession->iLinesPerBlt > MaxLines))
	lpSession->iLinesPerBlt = MaxLines;

if ( METHOD == 1 || METHOD == 2 || lpSession->hBitmapOut)
	{
	lpSession->hMemDC= CreateCompatibleDC( lpSession->hDC );
	if ( METHOD == 2 )
		lpSession->hBitmap = CreateCompatibleBitmap( lpSession->hDC,
			lpSession->dxBlt, lpSession->iLinesPerBlt );
	if (lpSession->hBitmapOut)
		lpSession->hDestMemDC = CreateCompatibleDC( lpSession->hDC );
	}

fNeedBuffer = fDoGamma == 2; // always need buffer for SuperFill()

if ((lpBlt->RGamma == FUNITY &&
     lpBlt->GGamma == FUNITY &&
     lpBlt->BGamma == FUNITY) ||
     lpBlt->bPaletteGamma ) // gamma adjustment done in palette
	{
	fDoGamma = NO;
	}

if (fDoGamma)
	fNeedBuffer = YES;

switch ( TypeInfo.DataType )
	{
	case FDT_LINEART:
	case FDT_GRAYSCALE:
		Depth = 1;
		if ( !hPal ) // Always use the image palette if there is one
			{
			PaletteType = lpBlt->GrayPaletteType;
			hPal = lpBlt->hGrayPal;
			}
		if (PaletteType == PT_GRAYPALETTE)
			lpSession->fpConvert = lpBlt->fpConvert1;
		else
		if (PaletteType == PT_COLORPALETTE)
			lpSession->fpConvert = lpBlt->fpConvert1C;
		else // if (PaletteType == PT_CUSTOMPALETTE)
			lpSession->fpConvert = lpBlt->fpConvert1CP;
		dxError = (lpSession->dxBlt+1) * sizeof(int) * sizeof(BYTE);
		if (fDoGamma)
			lpSession->fpMap = MapRGB8;
	break;

	case FDT_PALETTECOLOR:
		Depth = 1;
		if ( !hPal ) // Always use the image palette if there is one
			{
			PaletteType = lpBlt->ColorPaletteType;
			hPal = lpBlt->hColorPal;
			}
		if (PaletteType == PT_CUSTOMPALETTE && lpBlt->lpPaletteLUT)
			lpSession->fpConvert = lpBlt->fpConvert1P;
		else
		if (PaletteType == PT_GRAYPALETTE)
			lpSession->fpConvert = lpBlt->fpConvert1PG;
		else
			lpSession->fpConvert = lpBlt->fpConvert1PC;
		dxError = (lpSession->dxBlt+1) * sizeof(int) * sizeof(BYTE);
		if (fDoGamma)
			{
			MapRGB24(
				lpBlt->RGammaLut,
				lpBlt->GGammaLut,
				lpBlt->BGammaLut,
				TypeInfo.ColorMap->RGBData,
				lpBlt->lpColorMap->RGBData,
				TypeInfo.ColorMap->NumEntries);
			lpBlt->lpColorMap->NumEntries = TypeInfo.ColorMap->NumEntries;
			lpSession->TypeInfo.ColorMap = lpBlt->lpColorMap;
			}
	break;

	case FDT_RGBCOLOR:
		Depth = 3;
		if ( !hPal ) // Always use the image palette if there is one
			{
			PaletteType = lpBlt->ColorPaletteType;
			hPal = lpBlt->hColorPal;
			}

		if (PaletteType == PT_COLORPALETTE)
			lpSession->fpConvert = ( LCPROC )lpBlt->fpConvert3;
		else
		if (PaletteType == PT_GRAYPALETTE)
			lpSession->fpConvert = ( LCPROC )lpBlt->fpConvert3G;
		else // if (PaletteType == PT_CUSTOMPALETTE)
			lpSession->fpConvert = ( LCPROC )lpBlt->fpConvert3CP;

		dxError = (lpSession->dxBlt+1) * sizeof(int) * sizeof(RGBS);

		if (fDoGamma)
			lpSession->fpMap = ( LXPROC )MapRGB24;
	break;

	case FDT_CMYKCOLOR:
		Depth = 4;
		if ( !hPal ) // Always use the image palette if there is one
			{
			PaletteType = lpBlt->ColorPaletteType;
			hPal = lpBlt->hColorPal;
			}

		if (PaletteType == PT_COLORPALETTE)
			lpSession->fpConvert = ( LCPROC )lpBlt->fpConvert4;
		else
		if (PaletteType == PT_GRAYPALETTE)
			lpSession->fpConvert = ( LCPROC )lpBlt->fpConvert4G;
		else // if (PaletteType == PT_CUSTOMPALETTE)
			lpSession->fpConvert = ( LCPROC )lpBlt->fpConvert4CP;

		dxError = (lpSession->dxBlt+1) * sizeof(int) * sizeof(CMYKS);

		if (fDoGamma)
			lpSession->fpMap = ( LXPROC )MapRGB32;
	break;
	}

if ( hPal )
	{
	// if it's an 8-bit device, make sure palette look-up-table
	// is in place for palettes that we know about
	if (nBits == 8)
		{
		if (hPal == lpBlt->hGrayPal)
			lpBlt->lpPaletteLUT = lpBlt->lpGrayPaletteLUT;
		else
		if (hPal == lpBlt->hColorPal)
			lpBlt->lpPaletteLUT = lpBlt->lpColorPaletteLUT;
		}
	lpSession->hOldPal = SelectPalette( lpSession->hDC, hPal, FALSE );
	RealizePalette( lpSession->hDC );
	}
else
	lpSession->hOldPal = NULL;

if ( !lpBlt->bNeedErrorBuf )
	lpSession->lpError = NULL;
else
	{
	if ( !AllocLines( &lpSession->lpError, 1, dxError, 1 /*depth*/ ) )
		{
		if (lpSession->bBufAllocated)
			FreeUp(lpSession->lpBltBufStart);
		if (lpSession->bAllocated)
			FreeUp((LPTR)lpSession);
		else
			lpSession->bFailure = YES;
		lpBlt->nCalls--;
#ifdef WIN32
		LeaveCriticalSection(&lpBlt->csSuperBlt);
#endif
		return(NULL);
		}
	else
		{
		clr( lpSession->lpError, dxError );
		}
	}

if (lpBlt->bProcessData && (lpProcessProc != NULL))
	{
	lpSession->lpProcessProc = lpProcessProc;
	if (!AllocLines(&lpSession->lpProcessBuf, 1, lpSession->dxBlt, Depth))
		{
		FreeUp(lpSession->lpError);
		if (lpSession->bBufAllocated)
			FreeUp(lpSession->lpBltBufStart);
		if (lpSession->bAllocated)
			FreeUp((LPTR)lpSession);
		else
			lpSession->bFailure = YES;
		lpBlt->nCalls--;
#ifdef WIN32
		LeaveCriticalSection(&lpBlt->csSuperBlt);
#endif
		return(NULL);
		}
	}
else
	{
	lpSession->lpProcessProc = NULL;
	lpSession->lpProcessBuf  = NULL;
	}

if (fNeedBuffer)
	{
	if (!AllocLines(&lpSession->lpGammaBuf, 1, lpSession->dxBlt, Depth))
		{
		FreeUp(lpSession->lpError);
		if (lpSession->bBufAllocated)
			FreeUp(lpSession->lpBltBufStart);
		if (lpSession->lpProcessBuf)
			FreeUp(lpSession->lpProcessBuf);
		if (lpSession->bAllocated)
			FreeUp((LPTR)lpSession);
		else
			lpSession->bFailure = YES;
		lpBlt->nCalls--;
#ifdef WIN32
		LeaveCriticalSection(&lpBlt->csSuperBlt);
#endif
		return(NULL);
		}
	}
else
	{
	lpSession->lpGammaBuf = NULL;
	}

lpSession->xDiva = xDivaStart + lpSession->xBlt;
lpSession->yDiva = yDivaStart + lpSession->yBlt;

return(lpSession);
}

/***********************************************************************/
void SuperBlt( LPBLTSESSION lpSession, LPTR lp )
/***********************************************************************/
{
HGDIOBJ  hOldBitmap, hOldDestBitmap, hBitmap;
LPTR     lpConvertBuf;

if (!lpSession || lpSession->bFailure) // StartSuperBlt successful?
	return;

if ( lpSession->dyBlt == lpSession->iLinesPerBlt || !lp )
	{
	lpSession->BitMapInfo.bmiHeader.biWidth  = lpSession->dxBlt;
	lpSession->BitMapInfo.bmiHeader.biHeight = lpSession->dyBlt;

	if ( !METHOD )
		{
		if (lpSession->hBitmapOut)
			{
			hBitmap = CreateDIBitmap(
				lpSession->hDC,
				&lpSession->BitMapInfo.bmiHeader,
				CBM_INIT,
				lpSession->lpBltBuf,
				(LPBITMAPINFO)&lpSession->BitMapInfo,
				lpSession->wUsage );

			// If no bitmap we can't continue!
			if ( !hBitmap )
				return;

			hOldBitmap = SelectObject(
				lpSession->hMemDC, hBitmap );

			hOldDestBitmap = SelectObject(
				lpSession->hDestMemDC,
				lpSession->hBitmapOut);

			BitBlt(
				lpSession->hDestMemDC,
				lpSession->xBlt, lpSession->yBlt,
				lpSession->dxBlt, lpSession->dyBlt,
				lpSession->hMemDC, 0, 0, SRCCOPY );

			SelectObject( lpSession->hMemDC, hOldBitmap );
			SelectObject( lpSession->hDestMemDC, hOldDestBitmap);
			DeleteObject( hBitmap );
			}
		else
			{
			SetDIBitsToDevice(
				lpSession->hDC,
				lpSession->xBlt,
				lpSession->yBlt,
				lpSession->dxBlt,
				lpSession->dyBlt,
				0,
				0,
				0,
				lpSession->dyBlt,
				lpSession->lpBltBuf,
				(LPBITMAPINFO)&lpSession->BitMapInfo,
				lpSession->wUsage );
			}
		}
	else if ( METHOD == 1 )
		{
		hBitmap = CreateDIBitmap(
			lpSession->hDC,
			&lpSession->BitMapInfo.bmiHeader,
			CBM_INIT,
			lpSession->lpBltBuf,
			(LPBITMAPINFO)&lpSession->BitMapInfo,
			lpSession->wUsage );

		// If no bitmap can't continue
		if ( !hBitmap )
			return;

		hOldBitmap = SelectObject( lpSession->hMemDC, hBitmap );

		BitBlt(
			lpSession->hDC,
			lpSession->xBlt,
			lpSession->yBlt,
			lpSession->dxBlt,
			lpSession->dyBlt,
			lpSession->hMemDC,
			0,
			0,
			SRCCOPY );

		SelectObject( lpSession->hMemDC, hOldBitmap );
		DeleteObject( hBitmap );
		}
	else if ( METHOD == 2 )
		{
		SetDIBits(
			lpSession->hMemDC,
			lpSession->hBitmap,
			0,
			lpSession->dyBlt,
			lpSession->lpBltBuf,
			(LPBITMAPINFO)&lpSession->BitMapInfo,
			lpSession->wUsage );

		hOldBitmap = SelectObject( lpSession->hMemDC, lpSession->hBitmap );

		BitBlt(
			lpSession->hDC,
			lpSession->xBlt,
			lpSession->yBlt,
			lpSession->dxBlt,
			lpSession->dyBlt,
			lpSession->hMemDC,
			0,
			0,
			SRCCOPY );

		SelectObject( lpSession->hMemDC, hOldBitmap );
		}

	lpSession->yBlt += lpSession->dyBlt;
	lpSession->dyBlt = 0;

	lpSession->lpBltBuf =
		lpSession->lpBltBufStart+(65500L-lpSession->dxConverted);

	// Clean up if last call
	if ( !lp )
		{
		if (lpSession->hOldPal)
			SelectPalette( lpSession->hDC, lpSession->hOldPal, FALSE );

		if ( lpSession->lpError )
			FreeUp( lpSession->lpError );

		if ( lpSession->lpProcessBuf )
			FreeUp( lpSession->lpProcessBuf );

		if ( lpSession->lpGammaBuf )
			FreeUp( lpSession->lpGammaBuf );

		if ( METHOD == 1 || METHOD == 2 || lpSession->hBitmapOut)
			{
			DeleteDC( lpSession->hMemDC );
			if ( METHOD == 2 )
				DeleteObject( lpSession->hBitmap );
			if (lpSession->hBitmapOut)
				DeleteDC(lpSession->hDestMemDC);
			}

		if (lpSession->bBufAllocated)
			FreeUp(lpSession->lpBltBufStart);
		if (lpSession->bAllocated)
			FreeUp((LPTR)lpSession);
		lpSession->lpBlt->nCalls--;
#ifdef WIN32
		LeaveCriticalSection(&lpSession->lpBlt->csSuperBlt);
#endif
		return;
		}
	}

// Set up our convert pointer
lpConvertBuf = lp;

if (!lpSession->lpBlt->bOptimized)
{
	// Process the data if necessary
	if (lpSession->lpProcessProc != NULL)
		{
		(*lpSession->lpProcessProc)(
			(LPTR)lp,
			(LPTR)lpSession->lpProcessBuf,
			(int) lpSession->dxBlt,
			(int) lpSession->TypeInfo.DataType);

		lpConvertBuf = lpSession->lpProcessBuf;
		}

	// Do our gamma correction here if necessary
	if (lpSession->fpMap != NULL)
		{
		if (lpSession->lpGammaBuf != NULL)
			{
			(*lpSession->fpMap)(
				(LPTR)lpSession->lpBlt->RGammaLut,
				(LPTR)lpSession->lpBlt->GGammaLut,
				(LPTR)lpSession->lpBlt->BGammaLut,
				(LPTR)lpConvertBuf,
				(LPTR)lpSession->lpGammaBuf,
				(int)lpSession->dxBlt);

			lpConvertBuf = lpSession->lpGammaBuf;
			}
		}

	// Back up for more space
	lpSession->lpBltBuf -= lpSession->dxConverted;

	// Convert the data to display pixels
	(*lpSession->fpConvert)(
		(LPTR) lpConvertBuf,
		(int)  lpSession->dxBlt,
		(LPINT)lpSession->lpError,
		(LPTR) lpSession->lpBltBuf,
		lpSession->xDiva,
		lpSession->yDiva,
		lpSession );
}
else
{
	// Back up for more space
	lpSession->lpBltBuf -= lpSession->dxConverted;

	// Just copy the data over ...
	copy( lpConvertBuf, lpSession->lpBltBuf, lpSession->dxBlt );
}
lpSession->dyBlt++;
lpSession->yDiva++;
}

/***********************************************************************/
void SuperFill( HDC hDC, LPBLT lpBltDevice, LPRECT lpRect, int xDivaStart,
                     int yDivaStart, COLOR Color, LPBLTPROCESS lpProcessProc )
/***********************************************************************/
{
int i, lines;
LPTR lp;
BLTSESSION BltSession;
FRMTYPEINFO TypeInfo;
#define MAX_LINES 16

if ( METHOD != 0 )
	return;

FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);
if (!StartSuperBlt(
	&BltSession,
	hDC,
	NULL,
	lpBltDevice,
	lpRect,
	TypeInfo,
	MAX_LINES,
	xDivaStart,
	yDivaStart,
	2, /* Special Gamma correct flag */
	lpProcessProc))
	{
	return;
	}

if ( !BltSession.lpGammaBuf )
	goto Exit;

if ( BltSession.iLinesPerBlt < MAX_LINES )
	goto Exit;

BltSession.iLinesPerBlt = MAX_LINES;

// Stuff a line of color into the gamma buffer, and gamma correct it
// Assuming Depth=3
set24( (LPRGB)BltSession.lpGammaBuf, BltSession.dxBlt, Color );

if (BltSession.fpMap)
	{
	(*BltSession.fpMap)(
		(LPTR)BltSession.lpBlt->RGammaLut,
		(LPTR)BltSession.lpBlt->GGammaLut,
		(LPTR)BltSession.lpBlt->BGammaLut,
		(LPTR)BltSession.lpGammaBuf,
		(LPTR)BltSession.lpGammaBuf,
		(int)BltSession.dxBlt );
	}

if (lpProcessProc != NULL)
	(*lpProcessProc)(
		(LPTR)BltSession.lpGammaBuf,
		(LPTR)BltSession.lpGammaBuf,
		(int) BltSession.dxBlt,
		(int) 3);

// Create 16 lines of converted data
i = BltSession.iLinesPerBlt;

while ( --i >= 0 )
	{
	BltSession.lpBltBuf -= BltSession.dxConverted;

	(*BltSession.fpConvert)(
		(LPTR) BltSession.lpGammaBuf,
		(int)  BltSession.dxBlt,
		(LPINT)BltSession.lpError,
		(LPTR) BltSession.lpBltBuf,
		BltSession.xDiva,
		BltSession.yDiva,
		&BltSession );

	BltSession.yDiva++;
	}

// Blt the gamma corrected, converted lines, 16 at a time
lines = RectHeight( lpRect );
while ( lines > 0 )
	{
	BltSession.dyBlt = min( BltSession.iLinesPerBlt, lines );
	lp = BltSession.lpBltBuf + (BltSession.dxConverted *
		 (BltSession.iLinesPerBlt - BltSession.dyBlt));
	BltSession.BitMapInfo.bmiHeader.biWidth  = BltSession.dxBlt;
	BltSession.BitMapInfo.bmiHeader.biHeight = BltSession.dyBlt;
	SetDIBitsToDevice(
		BltSession.hDC,
		BltSession.xBlt,
		BltSession.yBlt,
		BltSession.dxBlt,
		BltSession.dyBlt,
		0,
		0,
		0,
		BltSession.dyBlt,
		lp,
		(LPBITMAPINFO)&BltSession.BitMapInfo,
		BltSession.wUsage );
	BltSession.yBlt += BltSession.dyBlt;
	lines -= BltSession.dyBlt;
	}

Exit:

if ( BltSession.lpError )
	FreeUp( BltSession.lpError );

if ( BltSession.lpProcessBuf )
	FreeUp( BltSession.lpProcessBuf );

if ( BltSession.lpGammaBuf )
	FreeUp( BltSession.lpGammaBuf );

if (BltSession.bBufAllocated)
	FreeUp(BltSession.lpBltBufStart);

BltSession.lpBlt->nCalls--;
}


/************************************************************************/
BOOL SetSuperBltGamma( LPBLT lpBlt, LFIXED RGamma, LFIXED GGamma,
                           LFIXED BGamma )
/************************************************************************/
{
lpBlt->RGamma = RGamma;
lpBlt->GGamma = GGamma;
lpBlt->BGamma = BGamma;
BuildGammaLuts(lpBlt);

if ( lpBlt->BitMapInfo.bmiHeader.biBitCount != 8 )
	return(NO);

if ( !lpBlt->bPaletteGamma )
	return(NO);

return(GammaAdjustPalettes(lpBlt));
}

/************************************************************************/
BOOL GammaAdjustPalette( LPBLT lpBlt, HPALETTE hPal, int nEntries,
                              LPPALETTEENTRY lpEntries)
/************************************************************************/
{
if ( !hPal )
	return(NO);

if ( lpBlt->BitMapInfo.bmiHeader.biBitCount != 8 )
	return( NO );

if ( lpBlt->bPaletteGamma )
	{
	MapPalette(
		hPal,
		(WORD)nEntries,
		lpEntries,
		lpBlt->RGammaLut,
		lpBlt->GGammaLut,
		lpBlt->BGammaLut,
		YES);
	}
else
	{
	AnimatePalette(hPal, 0, nEntries, lpEntries);
	}
return(YES);
}

/************************************************************************/
LOCAL BOOL GammaAdjustPalettes( LPBLT lpBlt )
/************************************************************************/
{
if ( lpBlt->BitMapInfo.bmiHeader.biBitCount != 8 )
	return( NO );

if ( !lpBlt->bPaletteGamma )
	return( NO );

if (lpBlt->hGrayPal)
	{
	MapPalette(
		lpBlt->hGrayPal,
		(WORD)lpBlt->nGrayEntries,
		lpBlt->GrayPalette,
		lpBlt->RGammaLut,
		lpBlt->GGammaLut,
		lpBlt->BGammaLut,
		NO);
	}

if (lpBlt->hColorPal)
	{
	MapPalette(
		lpBlt->hColorPal,
		(WORD)lpBlt->nColorEntries,
		lpBlt->ColorPalette,
		lpBlt->RGammaLut,
		lpBlt->GGammaLut,
		lpBlt->BGammaLut,
		NO);
	}
if (lpBlt->hPal &&
	 lpBlt->hPal != lpBlt->hColorPal &&
	 lpBlt->hPal != lpBlt->hGrayPal)
	{
	MapPalette(
		lpBlt->hPal,
		(WORD)lpBlt->nEntries,
		lpBlt->Palette,
		lpBlt->RGammaLut,
		lpBlt->GGammaLut,
		lpBlt->BGammaLut,
		NO);
	}
return(YES);
}

/************************************************************************/
LOCAL void BuildGammaLuts( LPBLT lpBlt )
/************************************************************************/
{
BuildGammaLut(lpBlt->RGamma, lpBlt->RGammaLut);
BuildGammaLut(lpBlt->GGamma, lpBlt->GGammaLut);
BuildGammaLut(lpBlt->BGamma, lpBlt->BGammaLut);
}

/***********************************************************************/
LOCAL BOOL MapPalette( HPALETTE hPal, WORD wEntries, LPPALETTEENTRY lpEntries,
                           LPTR lpRLut, LPTR lpGLut, LPTR lpBLut, BOOL bAnimate )
/***********************************************************************/
{
LPPALETTEENTRY lpEntry, lpNewEntry;
PALETTEENTRY   TempEntries[256+1];
int i;

if (lpEntries)
	{
	lpEntry = lpEntries;
	}
else
	{
	wEntries = GetPaletteEntries( hPal, 0, 256, TempEntries );
	lpEntry = TempEntries;
	}
lpNewEntry = TempEntries;

for (i = 0; i < wEntries; ++i)
	{
	lpNewEntry->peRed   = lpRLut[lpEntry->peRed];
	lpNewEntry->peGreen = lpGLut[lpEntry->peGreen];
	lpNewEntry->peBlue  = lpBLut[lpEntry->peBlue];
	lpNewEntry->peFlags = lpEntry->peFlags;
	++lpEntry;
	++lpNewEntry;
	}

if (bAnimate)
	{
	AnimatePalette(hPal, 0, wEntries, TempEntries);
	}
else
	{
	SetPaletteEntries(hPal, 0, wEntries, TempEntries);
	}

return(YES);
}


#ifdef UNUSED
/***********************************************************************/
LOCAL void SortEntries( LPPALETTEENTRY lpEntry, int iColors, LPWORD lpIndex )
/***********************************************************************/
{
BYTE r, g, b;
HSLS hsl;
int i, j, ih, jh;
DWORD dwValue1, dwValue2;
PALETTEENTRY TempEntry;

if ( iColors > 255 )
	return;

// Initialize the index with a duplicate high byte (the orignal index)
for ( i=0; i<iColors; i++ )
	lpIndex[i] *= (256+1);

/* Sort the colors by average HSL value */
for ( i=0; i<iColors; i++ )
	{
	r = lpEntry[i].peRed;
	g = lpEntry[i].peGreen;
	b = lpEntry[i].peBlue;
	RGBtoHSL( r, g, b, &hsl );
	dwValue1 = (((DWORD) (hsl.lum << 8 | hsl.sat) << 8) | hsl.hue);

	for ( j=i+1; j<iColors; j++ )
		{
		r = lpEntry[j].peRed;
		g = lpEntry[j].peGreen;
		b = lpEntry[j].peBlue;
		RGBtoHSL( r, g, b, &hsl );
		dwValue2 = (((DWORD) (hsl.lum << 8 | hsl.sat) << 8) | hsl.hue);
		if ( dwValue2 >= dwValue1 )
			continue;
		// Swap Entries
		dwValue1 = dwValue2;
		TempEntry = lpEntry[i];
		lpEntry[i] = lpEntry[j];
		lpEntry[j] = TempEntry;

		// Get the high bytes (the entry's original start index)
		ih = lpIndex[i] >> 8;
		jh = lpIndex[j] >> 8;

		// Swap the high bytes (the entry's original start index)
		lpIndex[i] &= 0x00FF;
		lpIndex[i] |= (jh << 8);
		lpIndex[j] &= 0x00FF;
		lpIndex[j] |= (ih << 8);

		// Look up, and write the new low bytes (the new indices)
		lpIndex[ih] &= 0xFF00;
		lpIndex[ih] |= j;
		lpIndex[jh] &= 0xFF00;
		lpIndex[jh] |= i;
		}
	}

// Clear the high byte (the original position) in the index
for ( i=0; i<iColors; i++ )
	lpIndex[i] &= 0x00FF;
}
#endif



