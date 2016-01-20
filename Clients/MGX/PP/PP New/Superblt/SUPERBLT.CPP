// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#ifndef WIN32
#include <windows.h>
#endif
#include "superblt.h"
#include "imgutil.h"
#ifndef NO_WING
#include "WinG.h"
#endif

// Static prototypes
LOCAL BOOL Init24BitDevice(      struct _blt far *lpBlt,BOOL bPcReserved);
LOCAL BOOL Init256ColorDevice(   struct _blt far *lpBlt,BOOL bPcReserved);
LOCAL BOOL Init16ColorDevice(    struct _blt far *lpBlt,BOOL bPcReserved);
LOCAL BOOL Init1BitColorDevice(  struct _blt far *lpBlt,BOOL bPcReserved);
LOCAL BOOL Init1BitMonoDevice(   struct _blt far *lpBlt,BOOL bPcReserved);
LOCAL BOOL Init1BitMonoDeviceHT( struct _blt far *lpBlt,BOOL bPcReserved);

LOCAL HPALETTE Create256GrayPalette( BOOL bPcReserved);
LOCAL HPALETTE Create256ColorPalette(BOOL bPcReserved);
LOCAL HPALETTE Create16GrayPalette( BOOL bPcReserved);
LOCAL HPALETTE Create16ColorPalette( BOOL bPcReserved);

/* have to keep this around */
LOCAL  void BuildGammaLuts(LPBLT);
LOCAL  void BuildRgb256LUT (LPBLT lpBlt,BOOL bPcReserved = TRUE);
LOCAL  BOOL GammaAdjustPalettes(LPBLT);
LOCAL  BOOL MapPalette(HPALETTE, WORD, LPPALETTEENTRY, LPTR, LPTR, LPTR, BOOL);

typedef void ( FAR *LCPROC )( LPTR, int, LPINT, LPTR, int, int, struct _bltsession FAR * );      
typedef void ( FAR *LXPROC )( LPTR, LPTR, LPTR, LPTR, unsigned ); 

int				NumStaticColors = 0;

#ifndef NO_WING
static	PALETTEENTRY	*lpStaticColors = NULL;
static  int  			WinGLines   = 20;
static	BOOL			WinGTopDown = FALSE;
static	BOOL			WinGInit		= FALSE;
static	BOOL			WinGUse		= TRUE;

LOCAL void	WinGTerminate(LPBLT lpBlt);
LOCAL void	WinGStart(LPBLT lpBlt);
LOCAL void  WinGCreateDIB(LPBLT lpBlt);
LOCAL void  CreateIdentityPalette(LPPALETTEENTRY lpPal);
LOCAL void  ClearSystemPalette(void);
LOCAL void  WinGSyncPalette(LPBLT lpBlt, HPALETTE hPal);
#endif

/***********************************************************************/
LPBLT InitSuperBlt( HDC hDC, int nTotalBits, LPTR lpBltBuf, BOOL bPaletteGamma,
                           BOOL bProcessData, BOOL bUseRgbLUIndex, BOOL bPcReserved )
/***********************************************************************/
{
int   nColors, nEntries, nBits, nPlanes;
BOOL  bBinaryPrinter, bUsePrinterScreening, fRet;
LPBLT lpBlt;

if( !(lpBlt = (LPBLT)Alloc((long)sizeof(BLT) ) ) )
	return(NULL);
clr((LPTR)lpBlt, sizeof(BLT));

// allocate a blt buffer
if( !( lpBlt->lpCopyBuf1 = Alloc( 65536L ) ) )
	{	
	FreeUp((LPTR)lpBlt);
	return( NULL );
	}

// allocate a buffer	for CMS and/or gamma correction.
if( !( lpBlt->lpCopyBuf2 = Alloc( 65536L ) ) )
	{	
	FreeUp(lpBlt->lpCopyBuf1);
	FreeUp((LPTR)lpBlt);
	return( NULL );
	}

#ifndef NO_WING
if (WinGUse && !WinGInit)
	WinGStart(lpBlt);
#endif

// allocate a colormap to be used for doing gamma on
// palette colored images
lpBlt->lpColorMap = FrameCreateColorMap();
if (!lpBlt->lpColorMap)
	{
	FreeUp(lpBlt->lpCopyBuf1);
	FreeUp(lpBlt->lpCopyBuf2);
	FreeUp((LPTR)lpBlt);
	return(NULL);
	}

lpBlt->bDIB          = GetDeviceCaps( hDC, RASTERCAPS ) & RC_DIBTODEV;
lpBlt->bProcessData  = bProcessData;
lpBlt->bPaletteGamma = bPaletteGamma;
lpBlt->bUseRgbLUIndex = bUseRgbLUIndex;
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
			fRet = Init1BitMonoDevice( lpBlt, bPcReserved );
		else
			fRet = Init1BitColorDevice( lpBlt, bPcReserved );
		if (!fRet)
			{
			EndSuperBlt(lpBlt);
			return(NULL);
			}
		return(lpBlt);
		}

	if (!bUsePrinterScreening)
		{
		// roll your own halftoning
		if ( bBinaryPrinter )
			{
			// Sets up halftone converters
			fRet = Init1BitMonoDeviceHT( lpBlt, bPcReserved );
			if (!fRet)
				{
				EndSuperBlt(lpBlt);
				return(NULL);
				}
			return(lpBlt);
			}
		nTotalBits = 0; // Fall through to the display cases
		}
	}

if ( nTotalBits < (nBits * nPlanes))
	nTotalBits = nBits * nPlanes;

if ( nTotalBits <= 4 )
	fRet = Init16ColorDevice( lpBlt, bPcReserved );
else if ( nTotalBits <= 8 )
	fRet = Init256ColorDevice( lpBlt, bPcReserved );
else
	fRet = Init24BitDevice( lpBlt, bPcReserved );

#ifndef NO_WING
if (WinGUse && !WinGInit)
	WinGCreateDIB(lpBlt);
#endif

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

FreeUp(lpBlt->lpCopyBuf1);
FreeUp(lpBlt->lpCopyBuf2);
#ifndef NO_WING
if (WinGUse && WinGInit)
	WinGTerminate(lpBlt);
#endif
FreeUp((LPTR)lpBlt);
}

/***********************************************************************/
HPALETTE SuperBltSetupPalette(LPBLT lpBlt, HDC hDC, HPALETTE hPal, BOOL fRealize)
/***********************************************************************/
{
	HPALETTE hOldPal = SelectPalette( hDC, hPal, FALSE );
	if (fRealize)
	{
		#ifndef NO_WING
		if (WinGUse)
			WinGSyncPalette(lpBlt, hPal);
		#endif
		RealizePalette( hDC );
	}
	return(hOldPal);
}

/***********************************************************************/
LOCAL BOOL Init24BitDevice( LPBLT lpBlt,BOOL bPcReserved )
/***********************************************************************/
{
#undef NCOLORS
#define NCOLORS 256		// 256-color palette required for WinG

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
LOCAL BOOL Init256ColorDevice( LPBLT lpBlt, BOOL bPcReserved )
/***********************************************************************/
{
int i;
#undef NCOLORS
#define NCOLORS 256		// 256-color palette required for WinG

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
if (lpBlt->bUseRgbLUIndex)
	BuildRgb256LUT (lpBlt, bPcReserved);
else
{
	for(i=0; i<NCOLORS; i++)
		lpBlt->BitMapInfo.bmi.Index[i] = i;
}

lpBlt->hPal          = NULL;
lpBlt->hColorPal     = Create256ColorPalette(bPcReserved);
lpBlt->ColorPaletteType = PT_COLORPALETTE;
lpBlt->hGrayPal      = Create256GrayPalette(bPcReserved);
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
LOCAL BOOL Init16ColorDevice( LPBLT lpBlt, BOOL bPcReserved )
/***********************************************************************/
{
int i;
#undef NCOLORS
#define NCOLORS 256		// 256-color palette required for WinG

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
lpBlt->hColorPal     = Create16ColorPalette(bPcReserved);
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
LOCAL BOOL Init1BitColorDevice( LPBLT lpBlt, BOOL bPcReserved )
/***********************************************************************/
{
int i;
#undef NCOLORS
#define NCOLORS 256		// 256-color palette required for WinG 

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
lpBlt->hColorPal    = Create16ColorPalette(bPcReserved);
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
LOCAL BOOL Init1BitMonoDevice( LPBLT lpBlt, BOOL bPcReserved )
/***********************************************************************/
{
int i;
#undef NCOLORS
#define NCOLORS 256		// 256-color palette required for WinG

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
lpBlt->hColorPal    = Create16ColorPalette(bPcReserved);
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
LOCAL BOOL Init1BitMonoDeviceHT( LPBLT lpBlt, BOOL bPcReserved )
/***********************************************************************/
{
int i;
#undef NCOLORS
#define NCOLORS 256		// 256-color palette required for WinG

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
lpBlt->hColorPal    = Create16ColorPalette(bPcReserved);
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
LOCAL HPALETTE Create256GrayPalette(BOOL bPcReserved)
/***********************************************************************/
{
LPLOGPALETTE   lpPal;
LPPALETTEENTRY lpEntry;
PALETTEENTRY   TempEntries[256+1];
WORD r, g, b, w;

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion = 0x300;
lpPal->palNumEntries = 256;
lpEntry = &lpPal->palPalEntry[NumStaticColors];

#ifndef NO_WING
CreateIdentityPalette(lpPal->palPalEntry);
#endif

// create the palette with 101 grays, 125 colors, and 10 unused entries

// Write out the first 5 unused entries
for ( w=0; w<5; w++ )
	{
	lpEntry->peRed   = 0;
	lpEntry->peGreen = 0;
	lpEntry->peBlue  = 0;
	lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
	lpEntry++;
	}

for ( w=0; w<50; w++ )
	{
	g = ((w * 255) + 50) / 100;
	lpEntry->peRed   = (BYTE)g;
	lpEntry->peGreen = (BYTE)g;
	lpEntry->peBlue  = (BYTE)g;
	lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;;
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
			lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
			lpEntry++;
			}
		}
	}

for ( w=50; w<101; w++ )
	{
	g = ((w * 255) + 50) / 100;
	lpEntry->peRed   = (BYTE)g;
	lpEntry->peGreen = (BYTE)g;
	lpEntry->peBlue  = (BYTE)g;
	lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
	lpEntry++;
	}

// Write out the last 5 unused entries
for ( w=0; w<5; w++ )
	{
	lpEntry->peRed   = lpEntry->peGreen = lpEntry->peBlue  = 0;
	lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
	lpEntry++;
	}

return( CreatePalette( lpPal ) );
}

/***********************************************************************/
HPALETTE CreateLogicalPalette(LPRGB lpRGBmap, int nEntries, BOOL bPcReserved)
/***********************************************************************/
{
LPLOGPALETTE   lpPal;
LPPALETTEENTRY lpEntry;
PALETTEENTRY   TempEntries[256+1];
int i;

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion = 0x300;
lpPal->palNumEntries = 256;	// 256-color palette required for WinG regardless of device
lpEntry = &lpPal->palPalEntry[NumStaticColors];

#ifndef NO_WING
CreateIdentityPalette(lpPal->palPalEntry);
#endif

for (i = 0; i < nEntries; ++i)
	{
	lpEntry->peRed   = lpRGBmap[i].red;
	lpEntry->peGreen = lpRGBmap[i].green;
	lpEntry->peBlue  = lpRGBmap[i].blue;
	lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
	lpEntry++;
	}

for (i += NumStaticColors; i < (256 - NumStaticColors); i++)
	{
	lpEntry->peRed   = lpEntry->peGreen = lpEntry->peBlue  = 0;
	lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
	lpEntry++;
	}
return( CreatePalette( lpPal ) );
}

/***********************************************************************/
LOCAL HPALETTE Create256ColorPalette(BOOL bPcReserved)
/***********************************************************************/
{
LPLOGPALETTE lpPal;
LPPALETTEENTRY lpEntry;
PALETTEENTRY TempEntries[256+1];
WORD r, g, b, w;

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion = 0x300;
lpPal->palNumEntries = 256;	// 256-color palette required for WinG regardless of device
lpEntry = &lpPal->palPalEntry[NumStaticColors];

#ifndef NO_WING
CreateIdentityPalette(lpPal->palPalEntry);
#endif

// create the palette with 216 colors, 16 grays, and 4 unused entries

// Write out the first 2 unused entries
for ( w=0; w<2; w++ )
	{
	lpEntry->peRed   = 0;
	lpEntry->peGreen = 0;
	lpEntry->peBlue  = 0;
	lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
	lpEntry++;
	}

// Write out the first 8 grays
for ( w=0; w<8; w++ )
	{
	g = min(w * 16, 255);
	lpEntry->peRed   = (BYTE)g;
	lpEntry->peGreen = (BYTE)g;
	lpEntry->peBlue  = (BYTE)g;
	lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
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
			lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
			lpEntry++;
			}
		}
	}

// Write out the last 8 grays
for ( w=8; w<16; w++ )
	{
	g = min(w * 16, 255);
	lpEntry->peRed   = (BYTE)g;
	lpEntry->peGreen = (BYTE)g;
	lpEntry->peBlue  = (BYTE)g;
	lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
	lpEntry++;
	}

// Write out the last 2 unused entries
for ( w=0; w<2; w++ )
	{
	lpEntry->peRed   = lpEntry->peGreen = lpEntry->peBlue  = 0;
	lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
	lpEntry++;
	}

return( CreatePalette( lpPal ) );
}

/***********************************************************************/
LOCAL HPALETTE Create16GrayPalette(BOOL bPcReserved)
/***********************************************************************/
{
LPLOGPALETTE   lpPal;
LPPALETTEENTRY lpEntry;
PALETTEENTRY   TempEntries[256+1];

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion    = 0x300;
lpPal->palNumEntries = 256;	// 256-color palette required for WinG regardless of device
lpEntry = &lpPal->palPalEntry[NumStaticColors];

#ifndef NO_WING
CreateIdentityPalette(lpPal->palPalEntry);
#endif

/* create the palette */
lpEntry->peRed   = lpEntry->peGreen = lpEntry->peBlue = 0;
lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0; lpEntry++;
lpEntry->peRed   = lpEntry->peGreen = lpEntry->peBlue = 128;
lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0; lpEntry++;
lpEntry->peRed   = lpEntry->peGreen = lpEntry->peBlue = 192;
lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0; lpEntry++;
lpEntry->peRed   = lpEntry->peGreen = lpEntry->peBlue = 255;
lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;

return( CreatePalette( lpPal ) );
}


/***********************************************************************/
LOCAL HPALETTE Create16ColorPalette(BOOL bPcReserved)
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
PALETTEENTRY   TempEntries[256+1];

lpPal = (LPLOGPALETTE)TempEntries;
lpPal->palVersion = 0x300;
lpPal->palNumEntries = 256;	// 256-color palette required for WinG regardless of device
lpEntry = &lpPal->palPalEntry[NumStaticColors];

#ifndef NO_WING
CreateIdentityPalette(lpPal->palPalEntry);
#endif

/* create the palette */

for(i=0;i<16;i++)
	{
	lpEntry->peRed   = Table16[i].red;
	lpEntry->peGreen = Table16[i].green;
	lpEntry->peBlue  = Table16[i].blue;
	lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
	lpEntry++;
	}

for (i += NumStaticColors; i < (256 - NumStaticColors); i++)
	{
	lpEntry->peRed   = lpEntry->peGreen = lpEntry->peBlue  = 0;
	lpEntry->peFlags = (bPcReserved) ? PC_RESERVED : 0;
	lpEntry++;
	}
return( CreatePalette( lpPal ) );
}

/***********************************************************************/
BOOL SetNewPalette(LPBLT lpBlt, PALETTE_TYPE PaletteType,
					HPALETTE hPal, LPTR lpPaletteLUT,
					LPPALETTEENTRY lpPalette, int nEntries,
					BOOL fApplyGamma)
/***********************************************************************/
{
// maybe we should do something like seeing if the palette changed
// and if a caller did not pass in lpPaletteLUT, then create on
// by calling CreatePaletteLUT15()
lpBlt->hPal = hPal;
lpBlt->PaletteType = PaletteType;
lpBlt->lpPaletteLUT = lpPaletteLUT;
lpBlt->nEntries = 256;

if ( lpBlt->BitMapInfo.bmiHeader.biBitCount != 8 )
	return( TRUE );

if ( !lpBlt->bPaletteGamma )
	return( TRUE );

if (lpBlt->hPal)
	{
	copy((LPTR)lpPalette, (LPTR)lpBlt->Palette, 256 * sizeof(PALETTEENTRY));

	// only need to apply gamma to a palette that is not
	// our standard gray or color palette
	if (fApplyGamma &&
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
	}

return(TRUE);
}

/***********************************************************************/
LPBLTSESSION StartSuperBlt( LPBLTSESSION lpSession, HDC hDC, HBITMAP hDestBitmap,
                              LPBLT lpBlt, LPRECT lpRect, FRMTYPEINFO TypeInfo,
                              int MaxLines, int xDivaStart, int yDivaStart,
                              BOOL fDoGamma, LPVOID cmsXform, LPKCMSEVALPROC lpKcmsEvalProc,
							  			BOOL fUseStretchDIB, BOOL bDoCmsGamma,BOOL bIsMetaDC)
/***********************************************************************/
{
HPALETTE hPal;
PALETTE_TYPE PaletteType;
int      dxError, Depth, nBits;

if (!lpSession)
	return(NULL);

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
lpSession->fUseStretchDIB = fUseStretchDIB;
lpSession->bDoCmsGamma = fDoGamma && lpKcmsEvalProc && cmsXform &&
				TypeInfo.ptInfo.frRCS && TypeInfo.ptInfo.toRCS &&
				!lpBlt->bGammaDisabled;
#ifdef WIN32
EnterCriticalSection(&lpBlt->csSuperBlt);
#endif

hPal = lpBlt->hPal;
PaletteType = lpBlt->PaletteType;

// somebody is already using the standard 64K buffer
if (lpBlt->nCalls != 0)
	{
	// allocate a 64K buffer for this session
	if (!(lpSession->lpBltBufStart = Alloc(65536L)))
		{
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
	lpSession->lpBltBufStart = lpBlt->lpCopyBuf1;
	lpSession->bBufAllocated = NO;
	lpBlt->nCalls++;
	}

// if gamma disable or we are using palette gamma and we are using
// a custom palette, then gamma is already applied
if (lpBlt->bGammaDisabled ||
	(lpBlt->bPaletteGamma && lpBlt->PaletteType == PT_CUSTOMPALETTE)) // gamma adjustment done in palette
	{
	fDoGamma = NO;
	}

nBits = lpSession->BitMapInfo.bmiHeader.biBitCount;
switch ( nBits )
	{
	case 24:
		lpSession->wUsage      = DIB_RGB_COLORS;
		lpSession->dxConverted = lpSession->dxBlt * 3;
	break;

	case 8:
		lpSession->wUsage = (lpBlt->bUseRgbLUIndex) ?
			DIB_RGB_COLORS : DIB_PAL_COLORS;
		lpSession->dxConverted = lpSession->dxBlt;
	break;

	case 4:
		lpSession->wUsage      = DIB_PAL_COLORS;
		lpSession->dxConverted = (lpSession->dxBlt+1)/2;
	break;
	}

// DIB packing
lpSession->dxConverted = 4 * ((lpSession->dxConverted + 3) / 4);

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
		else if (PaletteType == PT_COLORPALETTE)
			lpSession->fpConvert = lpBlt->fpConvert1C;
		else // if (PaletteType == PT_CUSTOMPALETTE)
			lpSession->fpConvert = lpBlt->fpConvert1CP;
		dxError = (lpSession->dxBlt+1) * sizeof(int) * sizeof(BYTE);
		if (fDoGamma)
			lpSession->fpMap = MapRGB8;
		lpSession->dxBytes = lpSession->dxBlt * Depth;
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
		else if (PaletteType == PT_GRAYPALETTE)
			lpSession->fpConvert = lpBlt->fpConvert1PG;
		else
			lpSession->fpConvert = lpBlt->fpConvert1PC;
		dxError = (lpSession->dxBlt+1) * sizeof(int) * sizeof(BYTE);
		if (fDoGamma)
			{
			FrameCopyColorMap(TypeInfo.ColorMap, lpBlt->lpColorMap);
			MapRGB24(
				lpBlt->RGammaLut,
				lpBlt->GGammaLut,
				lpBlt->BGammaLut,
				lpBlt->lpColorMap->RGBData,
				lpBlt->lpColorMap->NumEntries);
			lpSession->TypeInfo.ColorMap = lpBlt->lpColorMap;
			}
		lpSession->dxBytes = lpSession->dxBlt * Depth;
	break;

	case FDT_CMYKCOLOR: // always display CMYK the same as RGB
	case FDT_RGBCOLOR:
		if (TypeInfo.DataType == FDT_CMYKCOLOR)
			Depth = 4;
		else
			Depth = 3;
		if ( !hPal ) // Always use the image palette if there is one
			{
			PaletteType = lpBlt->ColorPaletteType;
			hPal = lpBlt->hColorPal;
			}

		if (PaletteType == PT_COLORPALETTE)
			lpSession->fpConvert = ( LCPROC )lpBlt->fpConvert3;
		else if (PaletteType == PT_GRAYPALETTE)
			lpSession->fpConvert = ( LCPROC )lpBlt->fpConvert3G;
		else // if (PaletteType == PT_CUSTOMPALETTE)
			lpSession->fpConvert = ( LCPROC )lpBlt->fpConvert3CP;

		dxError = (lpSession->dxBlt+1) * sizeof(int) * sizeof(RGBS);

		if (fDoGamma)
			lpSession->fpMap = ( LXPROC )MapRGB24;
		lpSession->dxBytes = lpSession->dxBlt * Depth;
	break;
	}

// see how many lines will fit in the output DIB buffer
lpSession->iLinesPerBlt =
	(65500L-lpSession->dxConverted)/(long)lpSession->dxConverted;
// see how many lines will fit in the data buffer
int iLinesInDataBuf = ( 65500L - lpSession->dxConverted ) /
	(long)lpSession->dxBytes;
if (iLinesInDataBuf < lpSession->iLinesPerBlt)
	lpSession->iLinesPerBlt = iLinesInDataBuf;

if ( (MaxLines > 0) && (lpSession->iLinesPerBlt > MaxLines))
	lpSession->iLinesPerBlt = MaxLines;

#ifndef NO_WING
if (WinGUse && (WinGLines < lpSession->iLinesPerBlt))
	lpSession->iLinesPerBlt = WinGLines;
#endif

#ifndef NO_WING
lpSession->lpCopyBuf2 = WinGUse? lpBlt->WinGBuf : lpBlt->lpCopyBuf2;
#else
lpSession->lpCopyBuf2 = lpBlt->lpCopyBuf2;
#endif

if ( hPal )
	{
	// if it's an 8-bit device, make sure palette look-up-table
	// is in place for palettes that we know about
	if (nBits == 8)
		{
		if (hPal == lpBlt->hGrayPal)
			lpBlt->lpPaletteLUT = lpBlt->lpGrayPaletteLUT;
		else if (hPal == lpBlt->hColorPal)
			lpBlt->lpPaletteLUT = lpBlt->lpColorPaletteLUT;
		}
	if (!bIsMetaDC)
		lpSession->hOldPal = SuperBltSetupPalette(lpBlt, lpSession->hDC, hPal, FALSE);
	else
		lpSession->hOldPal = NULL;
	}
else
	lpSession->hOldPal = NULL;

lpSession->lpCopyBuf1 =
		lpSession->lpBltBufStart +	( lpSession->dxBytes *
		lpSession->iLinesPerBlt );

if (lpSession->hBitmapOut)
	{
	lpSession->hMemDC		 = CreateCompatibleDC( lpSession->hDC );
	lpSession->hDestMemDC = CreateCompatibleDC( lpSession->hDC );
	}

if ( !lpBlt->bNeedErrorBuf )
	lpSession->lpError = NULL;
else
	{
	if ( !AllocLines( &lpSession->lpError, 1, dxError, 1 /*depth*/ ) )
		{
		if (lpSession->bBufAllocated)
			FreeUp(lpSession->lpBltBufStart);
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

if (TypeInfo.ptInfo.frRCS && TypeInfo.ptInfo.toRCS && 
	(TypeInfo.DataType == FDT_RGBCOLOR || TypeInfo.DataType == FDT_CMYKCOLOR) &&
	lpBlt->bProcessData && (cmsXform != NULL) && lpKcmsEvalProc != NULL)
	{	
	lpSession->cmsXform 	 		= cmsXform;
	lpSession->lpKcmsEvalProc 	= lpKcmsEvalProc;
	}
else
	{	
	lpSession->cmsXform 			= NULL;
	lpSession->lpKcmsEvalProc 	= NULL;
	}

lpSession->xDiva = xDivaStart + lpSession->xBlt;
lpSession->yDiva = yDivaStart + lpSession->yBlt;

return(lpSession);
}

/***********************************************************************/
void SuperBlt( LPBLTSESSION lpSession, LPTR lp, BOOL bIsMetaDC /*FALSE*/)
/***********************************************************************/
{
HGDIOBJ  	hOldBitmap, hOldDestBitmap, hBitmap;
LPTR     	lpIBuf, lpOBuf, lpBBuf;
int			y,dxBytes, yDiva, xDiva, dxBlt, dyBlt, dxConverted, iCount;
LPINT 		lpError;
LPTR		lpRLut, lpGLut, lpBLut;
FRMTYPEINFO	DstType;
void ( FAR *fpMap )( LPTR, LPTR, LPTR, LPTR, unsigned ); 

// For CMS only
DstType.DataType = FDT_RGBCOLOR;	 

if (!lpSession || lpSession->bFailure) // StartSuperBlt successful?
	return;

if ( lpSession->dyBlt == lpSession->iLinesPerBlt || !lp )
	{
	// save some variables to the stack for faster access
	// yDiva bumped backwards because data is really upside down
	xDiva = lpSession->xDiva;
	yDiva = lpSession->yDiva;
	dxBlt = lpSession->dxBlt;
	dyBlt = lpSession->dyBlt;
	dxConverted = lpSession->dxConverted;
	lpError = (LPINT)lpSession->lpError;
	if (fpMap = lpSession->fpMap)
		{
		if (lpSession->bDoCmsGamma)
			{
			lpRLut = lpSession->lpBlt->CmsRGammaLut;
			lpGLut = lpSession->lpBlt->CmsGGammaLut;
			lpBLut = lpSession->lpBlt->CmsBGammaLut;
			}
		else
			{
			lpRLut = lpSession->lpBlt->RGammaLut;
			lpGLut = lpSession->lpBlt->GGammaLut;
			lpBLut = lpSession->lpBlt->BGammaLut;
			}
		}

	lpSession->BitMapInfo.bmiHeader.biWidth  = dxBlt;
	lpSession->BitMapInfo.bmiHeader.biHeight = dyBlt;

	lpIBuf = lpSession->lpCopyBuf1;

	// Process the data if necessary
	if (lpSession->cmsXform != NULL && lpSession->lpKcmsEvalProc != NULL)
		{
		// the process proc should always convert to RGB
		lpOBuf = lpSession->lpBlt->lpCopyBuf2;
		(*lpSession->lpKcmsEvalProc)(
			&lpSession->TypeInfo,
			&DstType,
			lpSession->cmsXform,
			lpIBuf,
			lpOBuf,
			dxBlt,
			dyBlt);

		lpIBuf = lpSession->lpBlt->lpCopyBuf2;
		dxBytes = dxBlt * 3; // always RGB
		}
	else if (lpSession->TypeInfo.DataType == FDT_CMYKCOLOR)
		{
		lpOBuf = lpSession->lpBlt->lpCopyBuf2;
		CMYKtoRGBBuf(
			(LPCMYK)lpIBuf,
			(LPRGB)lpOBuf,
			dxBlt,
			lpSession->TypeInfo,
			lpSession->dyBlt);
		lpIBuf = lpSession->lpBlt->lpCopyBuf2;
		dxBytes = dxBlt * 3; // always RGB
		}
	else
		dxBytes = lpSession->dxBytes;

	lpOBuf = lpSession->lpCopyBuf2;
	// Do our gamma correction here if necessary
	if (fpMap)
		{
		(*fpMap)(
			(LPTR)lpRLut,
			(LPTR)lpGLut,
			(LPTR)lpBLut,
			(LPTR)lpIBuf,
			(unsigned)dxBlt*(unsigned)dyBlt);
		}

	lpBBuf = lpOBuf;
	#ifndef NO_WING
	if (WinGUse && WinGTopDown)
		lpOBuf += lpSession->lpBlt->WinGWidth * (dyBlt-1);
	#endif
	iCount = dyBlt;
	while (--iCount >= 0)
		{
		// Convert the data to display pixels
		(*lpSession->fpConvert)(
			(LPTR) lpIBuf,
			(int)  dxBlt,
			(LPINT)lpError,
			(LPTR) lpOBuf,
			xDiva,
			--yDiva,
			lpSession );

		// Bump to next line
		lpIBuf += dxBytes; 

		#ifndef NO_WING
		if (!WinGUse)
			lpOBuf += dxConverted; 
		else if (!WinGTopDown)
			lpOBuf += lpSession->lpBlt->WinGWidth; 
		else
			lpOBuf -= lpSession->lpBlt->WinGWidth; 
		#else
			lpOBuf += dxConverted; 
		#endif
		}

	if (!lpSession->hBitmapOut)
		{
		#ifndef NO_WING
		if (WinGUse)
			{
			y  = WinGTopDown?0 : (WinGLines - dyBlt);
	   	WinGBitBlt(lpSession->hDC,
						  lpSession->xBlt,
						  lpSession->yBlt,
						  dxBlt,
						  dyBlt,
					  	  lpSession->lpBlt->WinGDC,0,y);
#ifdef WIN32
			GdiFlush();
#endif
			}
		else
		#endif
			{
			RECT rc;
			::SetRect(&rc, 0, 0, dxBlt, dyBlt);
			if (!bIsMetaDC)
				::DPtoLP(lpSession->hDC, (LPPOINT) &rc, 2);
			::StretchDIBits(
				lpSession->hDC,
				lpSession->xBlt, 						 // logical dest x
				lpSession->yBlt, 						 // logical dest y
				rc.right-rc.left,      						 // logical extent
				rc.bottom-rc.top,     						 // logical extent
				0,				 						 // device src x
				0,				 						 // device src y
				lpSession->BitMapInfo.bmiHeader.biWidth	,// device src width
				lpSession->BitMapInfo.bmiHeader.biHeight,// device src height
				lpBBuf,									 // addr of buff w/DIBbits
				(LPBITMAPINFO)&lpSession->BitMapInfo,	 // addr of struct w/ bmp data
				lpSession->wUsage,						 // RGB or palette indices
				SRCCOPY);								 // raster op
			}
		}
	else
		{
		hBitmap = CreateDIBitmap(
			lpSession->hDC,
			&lpSession->BitMapInfo.bmiHeader,
			CBM_INIT,
			lpBBuf,
			(LPBITMAPINFO)&lpSession->BitMapInfo,
			lpSession->wUsage );

		// If no bitmap we can't continue!
		if ( !hBitmap )
			return;

		hOldBitmap 		= SelectObject(lpSession->hMemDC, hBitmap );
		hOldDestBitmap = SelectObject(lpSession->hDestMemDC,lpSession->hBitmapOut);

		BitBlt(
			lpSession->hDestMemDC,
			lpSession->xBlt, lpSession->yBlt,
			dxBlt, dyBlt,
			lpSession->hMemDC, 0, 0, SRCCOPY );

		SelectObject( lpSession->hMemDC, hOldBitmap );
		SelectObject( lpSession->hDestMemDC, hOldDestBitmap);
		DeleteObject( hBitmap );
		}

	lpSession->yBlt += lpSession->dyBlt;
	lpSession->dyBlt = 0;

lpSession->lpCopyBuf1 =
	lpSession->lpBltBufStart +	( lpSession->dxBytes *
	lpSession->iLinesPerBlt );

	// Clean up if last call
	if ( !lp )
		{
		if (!bIsMetaDC)
			{
			if (lpSession->hOldPal)
				SelectPalette( lpSession->hDC, lpSession->hOldPal, FALSE );
			}
		else	
			lpSession->hOldPal = NULL;
		if ( lpSession->lpError )
			FreeUp( lpSession->lpError );

		if (lpSession->hBitmapOut)
			{
			DeleteDC( lpSession->hMemDC );
			DeleteDC(lpSession->hDestMemDC);
			}
		if (lpSession->bBufAllocated)
			FreeUp(lpSession->lpBltBufStart);
		lpSession->lpBlt->nCalls--;
#ifdef WIN32
		LeaveCriticalSection(&lpSession->lpBlt->csSuperBlt);
#endif
		return;
		}
	}
		
// Back up for more space
lpSession->lpCopyBuf1 -= lpSession->dxBytes;

// Just copy the data over ...
copy( lp, lpSession->lpCopyBuf1, lpSession->dxBytes );

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
LPTR     lpIBuf, lpOBuf, lpOut;
#ifndef NO_WING
BOOL		SaveWinGUse = WinGUse;
#endif
#define MAX_LINES 16

#ifndef NO_WING
WinGUse = FALSE;
#endif
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
	NULL, //cmsXform
	NULL))
	{
	#ifndef NO_WING
	WinGUse = SaveWinGUse;
	#endif
	return;
	}

if ( BltSession.iLinesPerBlt < MAX_LINES )
	goto Exit;

BltSession.iLinesPerBlt = MAX_LINES;

// Stuff a line of color into the gamma buffer, and gamma correct it
// Assuming Depth=3

lpIBuf = BltSession.lpCopyBuf1;
lpOBuf = BltSession.lpCopyBuf2;
set24( (LPRGB)lpIBuf, BltSession.dxBlt, Color );

if (lpProcessProc != NULL)
	{
	(*lpProcessProc)(
		(LPTR)lpIBuf,
		(LPTR)lpOBuf,
		(int) BltSession.dxBlt,
		(int) 3,
		1);
	lpIBuf = BltSession.lpCopyBuf2;
	lpOBuf = BltSession.lpCopyBuf1;
	}

if (BltSession.fpMap)
	{
	(*BltSession.fpMap)(
		(LPTR)BltSession.lpBlt->RGammaLut,
		(LPTR)BltSession.lpBlt->GGammaLut,
		(LPTR)BltSession.lpBlt->BGammaLut,
		(LPTR)lpIBuf,
		(unsigned)BltSession.dxBlt );
	}

// Create 16 lines of converted data
i = BltSession.iLinesPerBlt;
lpOut = lpOBuf + 65500L;
while ( --i >= 0 )
	{
	lpOut -= BltSession.dxConverted;

	(*BltSession.fpConvert)(
		(LPTR) lpIBuf,
		(int)  BltSession.dxBlt,
		(LPINT)BltSession.lpError,
		(LPTR) lpOut,
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
	lp = lpOut + (BltSession.dxConverted *
		 (BltSession.iLinesPerBlt - BltSession.dyBlt));
	BltSession.BitMapInfo.bmiHeader.biWidth  = BltSession.dxBlt;
	BltSession.BitMapInfo.bmiHeader.biHeight = BltSession.dyBlt;
	if (!BltSession.fUseStretchDIB)
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
	else
		{
		RECT rc;
		::SetRect(&rc, 0, 0, BltSession.dxBlt, BltSession.dyBlt);
		::DPtoLP(BltSession.hDC, (LPPOINT) &rc, 2);
		::StretchDIBits(
			BltSession.hDC,
			BltSession.xBlt, 							 // logical dest x
			BltSession.yBlt, 							 // logical dest y
			rc.right-rc.left,      				 		 // logical extent
			rc.bottom-rc.top,     				 		 // logical extent
			0,				 							 // device src x
			0,				 							 // device src y
			BltSession.BitMapInfo.bmiHeader.biWidth	, 	 // device src width
			BltSession.BitMapInfo.bmiHeader.biHeight,    // device src height
			lp,									     	 // addr of buff w/DIBbits
			(LPBITMAPINFO)&BltSession.BitMapInfo,		 // addr of struct w/ bmp data
			BltSession.wUsage,							 // RGB or palette indices
			SRCCOPY);									 // raster op
		}
	BltSession.yBlt += BltSession.dyBlt;
	lines -= BltSession.dyBlt;
	}

Exit:

if ( BltSession.lpError )
	FreeUp( BltSession.lpError );

if (BltSession.bBufAllocated)
	FreeUp(BltSession.lpBltBufStart);

BltSession.lpBlt->nCalls--;
#ifndef NO_WING
WinGUse = SaveWinGUse;
#endif
}


/************************************************************************/
BOOL SetSuperBltGamma( LPBLT lpBlt, LFIXED RGamma, LFIXED GGamma,
                           LFIXED BGamma, BOOL bGammaDisabled )
/************************************************************************/
{

if (bGammaDisabled)
	{
	lpBlt->RGamma = FUNITY;
	lpBlt->GGamma = FUNITY;
	lpBlt->BGamma = FUNITY;
	}
else
	{
	lpBlt->RGamma = RGamma;
	lpBlt->GGamma = GGamma;
	lpBlt->BGamma = BGamma;
	}

BuildGammaLuts(lpBlt);
if( lpBlt->lpCmsGammaProc )
	( *lpBlt->lpCmsGammaProc )();

if ( lpBlt->BitMapInfo.bmiHeader.biBitCount == 8 && lpBlt->bPaletteGamma )
	GammaAdjustPalettes(lpBlt);
lpBlt->bGammaDisabled = bGammaDisabled;
return(TRUE);
}

///************************************************************************/
//BOOL SetSuperBltGammaLuts( LPBLT lpBlt, LPTR lpRLut, LPTR lpGLut, LPTR lpBLut)
///************************************************************************/
//{
//	int i;
//
//	lpBlt->RGamma = FUNITY;
//	lpBlt->GGamma = FUNITY;
//	lpBlt->BGamma = FUNITY;
//
//	if (lpRLut)
//	{
//		copy(lpRLut, lpBlt->RGammaLut, LUTSIZE);
//		copy(lpGLut, lpBlt->GGammaLut, LUTSIZE);
//		copy(lpBLut, lpBlt->BGammaLut, LUTSIZE);
//		for (i = 0; i < LUTSIZE; ++i)
//		{
//			if (lpBlt->RGammaLut[i] != i)
//				break;
//			if (lpBlt->GGammaLut[i] != i)
//				break;
//			if (lpBlt->BGammaLut[i] != i)
//				break;
//		}
//		if (i < LUTSIZE) // not FUNITY
//		{
//			lpBlt->RGamma *= 2;
//			lpBlt->GGamma *= 2;
//			lpBlt->BGamma *= 2;
//		}
//	}
//	else
//	{
//		for (i = 0; i < LUTSIZE; ++i)
//		{
//			lpBlt->RGammaLut[i] = i;
//			lpBlt->GGammaLut[i] = i;
//			lpBlt->BGammaLut[i] = i;
//		}
//	}
//
//	if ( lpBlt->BitMapInfo.bmiHeader.biBitCount != 8 )
//		return(NO);
//
//	if ( !lpBlt->bPaletteGamma )
//		return(NO);
//
//	return(GammaAdjustPalettes(lpBlt));
//}

/************************************************************************/
BOOL GammaAdjustPalette( LPBLT lpBlt, HPALETTE hPal, int nEntries,
                              LPPALETTEENTRY lpEntries)
/************************************************************************/
{
if ( !hPal )
	return(NO);

if ( lpBlt->BitMapInfo.bmiHeader.biBitCount != 8 )
	return( NO );

if ( lpBlt->bPaletteGamma && !lpBlt->bGammaDisabled &&
	hPal != lpBlt->hColorPal && hPal != lpBlt->hGrayPal)
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

#ifdef UNUSED
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
#endif

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
LOCAL void BuildRgb256LUT (LPBLT lpBlt, BOOL bPcReserved)
/***********************************************************************/
{
	WORD w,r,g,b;
	int i = 0;

	// Write out the first 2 unused entries
	for ( w=0; w<2; w++ )
	{
		lpBlt->BitMapInfo.bmi.Colors[i].rgbRed = 0;
		lpBlt->BitMapInfo.bmi.Colors[i].rgbGreen = 0;
		lpBlt->BitMapInfo.bmi.Colors[i].rgbBlue  = 0;
		lpBlt->BitMapInfo.bmi.Colors[i].rgbReserved = (bPcReserved) ? PC_RESERVED : 0;
		i++;
	}
	
	// Write out the first 8 grays
	for ( w=0; w<8; w++ )
	{
		g = min(w * 16, 255);
		lpBlt->BitMapInfo.bmi.Colors[i].rgbRed   = (BYTE)g;
		lpBlt->BitMapInfo.bmi.Colors[i].rgbGreen = (BYTE)g;
		lpBlt->BitMapInfo.bmi.Colors[i].rgbBlue  = (BYTE)g;
		lpBlt->BitMapInfo.bmi.Colors[i].rgbReserved = (bPcReserved) ? PC_RESERVED : 0;
		i++;
	}
	
	// Write out the 216 colors
	for ( r=0; r<6; r++ )
	{
		for ( g=0; g<6; g++ )
		{
			for ( b=0; b<6; b++ )
			{
				lpBlt->BitMapInfo.bmi.Colors[i].rgbRed 	 = ( BYTE )( r * 51 );
				lpBlt->BitMapInfo.bmi.Colors[i].rgbGreen = ( BYTE )( g * 51 ); 
				lpBlt->BitMapInfo.bmi.Colors[i].rgbBlue  = ( BYTE )( b * 51 ); 
				lpBlt->BitMapInfo.bmi.Colors[i].rgbReserved = (bPcReserved) ? PC_RESERVED : 0;
				i++;
			}
		}
	}
	
	// Write out the last 8 grays
	for ( w=8; w<16; w++ )
	{
		g = min(w * 16, 255);
		lpBlt->BitMapInfo.bmi.Colors[i].rgbRed   = (BYTE)g;
		lpBlt->BitMapInfo.bmi.Colors[i].rgbGreen = (BYTE)g;
		lpBlt->BitMapInfo.bmi.Colors[i].rgbBlue  = (BYTE)g;
		lpBlt->BitMapInfo.bmi.Colors[i].rgbReserved = (bPcReserved) ? PC_RESERVED : 0;
		i++;
	}
	
	// Write out the last 2 unused entries
	for ( w=0; w<2; w++ )
	{
		lpBlt->BitMapInfo.bmi.Colors[i].rgbRed = 0;
		lpBlt->BitMapInfo.bmi.Colors[i].rgbGreen = 0;
		lpBlt->BitMapInfo.bmi.Colors[i].rgbBlue  = 0;
		lpBlt->BitMapInfo.bmi.Colors[i].rgbReserved = (bPcReserved) ? PC_RESERVED : 0;
		i++;
	}
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
	wEntries = GetPaletteEntries( hPal, 0, wEntries, TempEntries );
	lpEntry = TempEntries;
	}
lpNewEntry = TempEntries;

for (i = 0; i < NumStaticColors; ++i)
	*lpNewEntry++ = *lpEntry++;
for (; i < wEntries-NumStaticColors; ++i)
	{
	lpNewEntry->peRed   = lpRLut[lpEntry->peRed];
	lpNewEntry->peGreen = lpGLut[lpEntry->peGreen];
	lpNewEntry->peBlue  = lpBLut[lpEntry->peBlue];
	lpNewEntry->peFlags = lpEntry->peFlags;
	++lpEntry;
	++lpNewEntry;
	}
for (; i < wEntries; ++i)
	*lpNewEntry++ = *lpEntry++;

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

#ifndef NO_WING
typedef struct header
{
  BITMAPINFOHEADER  Header;
  RGBQUAD           Colors[256];
} header;

void  WinGSyncPalette(LPBLT lpBlt, HPALETTE hPal)
	{
	RGBQUAD Colors[256];
	PALETTEENTRY   Entries[256];
	int	i;
	
	GetPaletteEntries( hPal, 0, 256, Entries);
	for (i=0; i < 256; i++)
		{
		Colors[i].rgbRed 			= Entries[i].peRed;
		Colors[i].rgbGreen  		= Entries[i].peGreen;
		Colors[i].rgbBlue 		= Entries[i].peBlue;
		Colors[i].rgbReserved 	= 0;
		}
	WinGSetDIBColorTable(lpBlt->WinGDC, 0, 256, Colors);
	}

void	WinGTerminate(LPBLT lpBlt)
	{
   SelectObject(lpBlt->WinGDC, lpBlt->WinGOldBM);
   DeleteObject(lpBlt->WinGBM);
   DeleteDC(lpBlt->WinGDC);
	}

extern int GetDefaultInt( LPTSTR lpKeyName, int nDefault );

void	WinGStart(LPBLT lpBlt)
	{
	header    	BufferHeader;
  	HDC 			hdc = GetDC(NULL);
	int			i;

   WinGUse = GetDefaultInt("UseWinG", 0);
   lpBlt->WinGDC = WinGCreateDC();
   if(WinGRecommendDIBFormat((BITMAPINFO far *)&BufferHeader))
   	WinGTopDown = BufferHeader.Header.biHeight == -1;
	if (GetSystemPaletteUse(hdc) == SYSPAL_STATIC)
		{
		NumStaticColors = GetDeviceCaps(hdc, NUMCOLORS);
		lpStaticColors = (PALETTEENTRY *)Alloc((long)sizeof(PALETTEENTRY) * 256); 
		if(lpStaticColors)
			{
			NumStaticColors /= 2;
			GetSystemPaletteEntries(hdc, 0, 256, lpStaticColors);
			for (i=0;i < 256; i++)
				lpStaticColors[i].peFlags = 0;
			}
		}
	else
		{
		NumStaticColors = 0;
		lpStaticColors = NULL;
		}

	ReleaseDC(NULL, hdc);
	ClearSystemPalette();
	}

void WinGCreateDIB(LPBLT lpBlt)
	{
  	HDC 			hdc = GetDC(NULL);
	int			i,ww,w = GetDeviceCaps(hdc, HORZRES);
	header    	bmi;

	WinGInit = TRUE;
	switch (lpBlt->BitMapInfo.bmiHeader.biBitCount)
		{
		case 24:
			ww = w * 3;
		break;

		case 8:
			ww = w;
			break;

		case 4:
			ww = (w+1)/2;
		break;
		}

	lpBlt->WinGWidth = 4 * ((ww + 3) / 4);
	bmi.Header = lpBlt->BitMapInfo.bmiHeader;

	for (i=0; i < 256; i++)
		{
		bmi.Colors[i].rgbRed 	= 0;
		bmi.Colors[i].rgbGreen  = 0;
		bmi.Colors[i].rgbBlue 	= 0;
		bmi.Colors[i].rgbReserved 	= 0;
		}
	
	bmi.Header.biWidth  = w;
	bmi.Header.biHeight = WinGLines * (WinGTopDown? -1 : 1);
   lpBlt->WinGBM = WinGCreateBitmap(lpBlt->WinGDC,(BITMAPINFO *)&bmi, (void **)&lpBlt->WinGBuf);
   lpBlt->WinGOldBM = (HBITMAP)SelectObject(lpBlt->WinGDC, lpBlt->WinGBM);
	ReleaseDC(NULL, hdc);
	}

void CreateIdentityPalette(LPPALETTEENTRY lpPal)
	{
	int i;

	if (!NumStaticColors)
		return;
	for (i=0; i < 256; i++)
		lpPal[i] = lpStaticColors[i];
   }

void ClearSystemPalette(void)
   {
	struct
	{
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
	} Palette =
	{
		0x300,
		256
	};

	HPALETTE ScreenPalette = 0;
	HDC ScreenDC;
	int Counter;

	//*** Reset everything in the system palette to black
	for(Counter = 0; Counter < 256; Counter++)
	   {
		Palette.aEntries[Counter].peRed = 0;
		Palette.aEntries[Counter].peGreen = 0;
		Palette.aEntries[Counter].peBlue = 0;
		Palette.aEntries[Counter].peFlags = PC_NOCOLLAPSE;
	   }

	//*** Create, select, realize, deselect, and delete the palette
	ScreenDC = GetDC(NULL);
	ScreenPalette = CreatePalette((LOGPALETTE *)&Palette);
	if (ScreenPalette)
	   {
		ScreenPalette = SelectPalette(ScreenDC,ScreenPalette,FALSE);
		RealizePalette(ScreenDC);
		ScreenPalette = SelectPalette(ScreenDC,ScreenPalette,FALSE);
		DeleteObject(ScreenPalette);
	   }
	ReleaseDC(NULL, ScreenDC);
   }
#endif


#ifdef UNUSED
// in StartSuperBlt
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

if ( METHOD == 1 || METHOD == 2 || lpSession->hBitmapOut)
	{
	lpSession->hMemDC= CreateCompatibleDC( lpSession->hDC );
	if ( METHOD == 2 )
		lpSession->hBitmap = CreateCompatibleBitmap( lpSession->hDC,
			lpSession->dxBlt, lpSession->iLinesPerBlt );
	if (lpSession->hBitmapOut)
		lpSession->hDestMemDC = CreateCompatibleDC( lpSession->hDC );
	}

// in SuperBlt
	else if ( METHOD == 1 )
		{
		hBitmap = CreateDIBitmap(
			lpSession->hDC,
			&lpSession->BitMapInfo.bmiHeader,
			CBM_INIT,
			lpBBuf,
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
			dxBlt,
			dyBlt,
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
			dyBlt,
			lpBBuf,
			(LPBITMAPINFO)&lpSession->BitMapInfo,
			lpSession->wUsage );

		hOldBitmap = SelectObject( lpSession->hMemDC, lpSession->hBitmap );

		BitBlt(
			lpSession->hDC,
			lpSession->xBlt,
			lpSession->yBlt,
			dxBlt,
			dyBlt,
			lpSession->hMemDC,
			0,
			0,
			SRCCOPY );

		SelectObject( lpSession->hMemDC, hOldBitmap );
		}
	if ( METHOD == 1 || METHOD == 2 || lpSession->hBitmapOut)
		{
		DeleteDC( lpSession->hMemDC );
		if ( METHOD == 2 )
			DeleteObject( lpSession->hBitmap );
		if (lpSession->hBitmapOut)
			DeleteDC(lpSession->hDestMemDC);
		}

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
