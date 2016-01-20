//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

/************************************************************************/
int DrawFrame(
	LPFRAME lpFrame,
	HDC		hDC,
	LPRECT  lpSourceRect,
	LPRECT  lpDestRect,
	LPRECT	lpRepairRect )
/************************************************************************/
{
int y, yline, ystart, ylast, xstart, depth, ddx, ddy;
int SrcWidth, DstWidth, PaintWidth;
LFIXED yrate, xrate, yoffset;
RECT rPaint, rSource, rDest;
long lSizeNeeded;
LPTR lpLine;
BLTSESSION BltSession;
LPCOLORMAP lpColorMap;
static LPTR lpBuffer;
static long lBufSize;
#ifdef STATIC16  // Only in the new framelib
FRMTYPEINFO TypeInfo;
#endif

#define LINES_PER_BLT 20

if ( !(depth = FrameDepth( lpFrame )) )
	depth = 1;

rDest = *lpDestRect;
rSource = *lpSourceRect;
if ( !lpRepairRect )
	rPaint = rDest;
else
	{
	rPaint = *lpRepairRect;
	BoundRect( &rPaint, rDest.left, rDest.top, rDest.right, rDest.bottom );
	}
BoundRect( &rSource, 0, 0, FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1 );
DstWidth = RectWidth( &rDest );
SrcWidth = RectWidth( &rSource );
PaintWidth = RectWidth(&rPaint);

lSizeNeeded = (long)DstWidth * depth;
if (lSizeNeeded > lBufSize)
	{
	if (lpBuffer)
		FreeUp(lpBuffer);
	lBufSize = lSizeNeeded;
	if ( !(lpBuffer = Alloc(lBufSize)) )
		{
		lBufSize = 0;
		return(FALSE);
		}
	}

// compute x and y display rates
yrate = FGET(RectHeight( &rSource),RectHeight(&rDest));
xrate = FGET(SrcWidth, DstWidth);

// get offsets from display edge to paint rect to adjust yoffset
ddx = rPaint.left - rDest.left;
ddy = rPaint.top - rDest.top;
yoffset = (ddy*yrate)+((long)yrate>>1);

// Find x-position in file coordinates where painting begins and ends
xstart = rSource.left;
ystart = rSource.top;

lpColorMap = ( depth == 1 ? FrameGetColorMap( lpFrame ) : NULL );
ylast = -1;
#ifdef STATIC16 // Only in the new framelib
FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);
StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &rPaint,
	TypeInfo,
	( RectHeight(&rPaint) > 1000 ? LINES_PER_BLT : 0), 0, 0, YES, NULL );
#else
StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &rPaint,
	( lpColorMap ? 3 : depth ),
	( RectHeight(&rPaint) > 1000 ? LINES_PER_BLT : 0), 0, 0, YES, NULL );
#endif
for ( y=rPaint.top; y<=rPaint.bottom; y++ )
	{
#ifdef WIN32
	yline = ystart + WHOLE( yoffset );
#else
	yline = ystart + HIWORD( yoffset );
#endif
	yoffset += yrate;
	if ( yline != ylast )
		{
		ylast = yline;
		if ( lpLine = FramePointer( lpFrame, xstart, yline, NO) )
			{
			if ( xrate == UNITY )
				lpLine += (ddx * depth);
			else
				{
				FrameSample( lpFrame, lpLine, 0, lpBuffer, (WORD)ddx /*outskip*/,
					PaintWidth + ddx, xrate );
				lpLine = lpBuffer;
				}
			if ( lpColorMap )
				lpLine = FrameExpand8to24( lpLine, PaintWidth, lpColorMap );
			}
		}
	SuperBlt( &BltSession, lpLine );
	}
SuperBlt( &BltSession, NULL );
return( TRUE );
}


/************************************************************************/
LPTR FrameExpand8to24( LPTR lpLineIn, int iCount, LPCOLORMAP lpColorMap )
/************************************************************************/
{
int iEntries;
BYTE index;
long lSizeNeeded;
LPRGB lpLineOut;
static LPTR lpExpandBuf;
static long lExpandBufSize;

if ( !lpLineIn || !iCount || !lpColorMap )
	return( lpLineIn );

lSizeNeeded = (long)iCount * 3;
if (lSizeNeeded > lExpandBufSize)
	{
	if (lpExpandBuf)
		FreeUp(lpExpandBuf);
	lExpandBufSize = lSizeNeeded;
	if ( !(lpExpandBuf = Alloc(lExpandBufSize)) )
		{
		lExpandBufSize = 0;
		return( lpLineIn );
		}
	}

iEntries = lpColorMap->NumEntries;
lpLineOut = (LPRGB)lpExpandBuf;
while ( --iCount >= 0 )
	{
	index = *lpLineIn++;
	if ( index >= iEntries )
		index = 0;
	*lpLineOut++ = lpColorMap->RGBData[ index ];
	}
return( lpExpandBuf );
}



/************************************************************************/
HBITMAP CALLBACK EXPORT FrameToBitmap( LPFRAME lpFrame, LPRECT lpRect )
/************************************************************************/
{
int depth;
int y, dx, dy;
HDC hDC;
HBITMAP hBitmap;
RECT rRect, DestRect;
LPTR lp;
BLTSESSION BltSession;
LPCOLORMAP lpColorMap;
#ifdef STATIC16  // Only in the new framelib
FRMTYPEINFO TypeInfo;
#endif


if ( !lpFrame )
	return( NULL );

if ( lpRect )
	rRect = *lpRect;
else
	SetRect(&rRect, 0, 0, FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1);
dx = RectWidth( &rRect );
dy = RectHeight( &rRect );

if ( !(hDC = GetDC(NULL)) )
	return( NULL );

if ( !(hBitmap = CreateCompatibleBitmap(hDC, dx, dy)) )
	{
	Message( IDS_EMEMALLOC );
	ReleaseDC( NULL, hDC );
	return( NULL );
	}

DestRect.left = DestRect.top = 0;
DestRect.right = DestRect.left + dx - 1;
DestRect.bottom = DestRect.top + dy - 1;

depth = FrameDepth(lpFrame);
if (depth == 0) depth = 1;

lpColorMap = ( depth == 1 ? FrameGetColorMap( lpFrame ) : NULL );
#ifdef STATIC16 // Only in the new framelib
FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);
StartSuperBlt( &BltSession, hDC, hBitmap, lpBltScreen, &DestRect,
	TypeInfo, 0, 0, 0, YES, NULL);
#else
StartSuperBlt( &BltSession, hDC, hBitmap, lpBltScreen, &DestRect,
	( lpColorMap ? 3 : depth ), 0, 0, 0, YES, NULL);
#endif
for (y = rRect.top; y <= rRect.bottom; ++y)
	{
	if ( !(lp = FramePointer(lpFrame, rRect.left, y, NO)) )
		break;
	if ( lpColorMap )
		lp = FrameExpand8to24( lp, dx, lpColorMap );
	SuperBlt(&BltSession, lp);
	}
SuperBlt( &BltSession, NULL );
ReleaseDC( NULL, hDC );
return( hBitmap );
}


/************************************************************************/
HGLOBAL FrameToMetafile( LPFRAME lpFrame, LPRECT lpRect )
/************************************************************************/
{
int y, dx, dy;
HDC hDC;
HGLOBAL hMem;
RECT rRect, DestRect;
HMETAFILE hMF;
LPMETAFILEPICT lpMF;
LPTR lp;
BLTSESSION BltSession;
#define HIMETRIC_PER_INCH 2540
#ifdef STATIC16  // Only in the new framelib
FRMTYPEINFO TypeInfo;
#endif

if ( !lpFrame )
	return( NULL );

if ( lpRect )
	rRect = *lpRect;
else
	SetRect(&rRect, 0, 0, FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1);
dx = RectWidth( &rRect );
dy = RectHeight( &rRect );

// Create a memory metafile and return its handle.
if ( !(hDC = CreateMetaFile( NULL )) )
	{
	Message( IDS_EMEMALLOC );
	return( NULL );
	}

// This is absolutely essential to the metafile so it can be
// scaled in the clipboard and any destination application.
SetMapMode( hDC, MM_TEXT );

DestRect.left   = DestRect.top = 0;
DestRect.right  = DestRect.left + dx - 1;
DestRect.bottom = DestRect.top + dy - 1;

#ifdef STATIC16 // Only in the new framelib
FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);
StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &DestRect,
	TypeInfo, 0, 0, 0, YES, NULL );
#else
StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &DestRect,
	FrameDepth(lpFrame), 0, 0, 0, YES, NULL);
#endif
for (y = rRect.top; y <= rRect.bottom; ++y)
	{
	if ( !(lp = FramePointer(lpFrame, rRect.left, y, NO)) )
		break;
	SuperBlt(&BltSession, lp);
	}
SuperBlt(&BltSession, NULL);

if ( !(hMF = CloseMetaFile( hDC )) )
	{
	Message( IDS_EMEMALLOC );
	return( NULL );
	}

// Allocate the METAFILEPICT structure.
if ( !(hMem = GlobalAlloc( GMEM_SHARE | GMEM_MOVEABLE, sizeof(METAFILEPICT) )) )
	{
	Message( IDS_EMEMALLOC );
	DeleteMetaFile( hMF );
	return( NULL );
	}

// Convert from device to HIMETRIC units
dx = MulDiv( dx, HIMETRIC_PER_INCH, FrameResolution(lpFrame) );
dy = MulDiv( dy, HIMETRIC_PER_INCH, FrameResolution(lpFrame) );

lpMF = (LPMETAFILEPICT)GlobalLock(hMem);
lpMF->hMF = hMF;
lpMF->mm = MM_ANISOTROPIC; // Required by OLE libraries.
lpMF->xExt = dx;
lpMF->yExt = dy;

GlobalUnlock( hMem );
return( hMem );
}


#define WIDTHBYTES(i) \
	((unsigned)((i+31)&(~31))/8)  /* ULONG aligned ! */

//**********************************************************************
LPBITMAPINFOHEADER BitmapToDIB( HDC hDC, HBITMAP hBitmap, HPALETTE hPal )
//**********************************************************************
{
LPTR lp;
LPBITMAPINFOHEADER lpDIB;
DWORD dwLen;
BITMAP bm;
BITMAPINFOHEADER bi;
BOOL bDCPassedIn;
HPALETTE hOldPal;

if ( !GetObject( hBitmap, sizeof(BITMAP), (LPSTR)&bm ) )
	{
	Message( IDS_EMEMALLOC );
	return( NULL );
	}

bi.biSize			= sizeof(BITMAPINFOHEADER);
bi.biWidth			= bm.bmWidth;
bi.biHeight			= bm.bmHeight;
bi.biPlanes			= 1;
bi.biCompression	= BI_RGB;
bi.biXPelsPerMeter	= (10000L*(long)72)/254L;
bi.biYPelsPerMeter	= (10000L*(long)72)/254L;
bi.biClrImportant	= 0;
bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
if ( bi.biBitCount >= 16 )
	bi.biBitCount = 24;
if ( bi.biBitCount <= 8 )
	bi.biClrUsed = (int)(1 << (int)bi.biBitCount);
else
	bi.biClrUsed = 0;

// fill in the size based on the width, height and bit depth.
bi.biSizeImage = (DWORD)WIDTHBYTES(bm.bmWidth * bi.biBitCount) * bm.bmHeight;

// Alloc the buffer to be big enough to hold all the bits
dwLen = (DWORD)bi.biSize + (bi.biClrUsed * sizeof(RGBQUAD)) + bi.biSizeImage;
if ( !(lpDIB = (LPBITMAPINFOHEADER)Alloc( dwLen )) )
	{
	Message( IDS_EMEMALLOC );
	return( NULL );
	}

if ( !hDC )
	{
	bDCPassedIn = NO;
	hDC = GetDC( NULL );
	}
else
	bDCPassedIn = YES;

if ( hPal )
	{
	hOldPal = SelectPalette( hDC, hPal, FALSE );
	RealizePalette( hDC );
	}
else
	hOldPal = NULL;

// call GetDIBits and get the bits
lp = (LPTR)lpDIB + bi.biSize + (bi.biClrUsed * sizeof(RGBQUAD));
*lpDIB = bi;
if ( !GetDIBits( hDC, hBitmap, 0, (WORD)bm.bmHeight, lp,
	(LPBITMAPINFO)lpDIB, DIB_RGB_COLORS ) )
	{
	Message( IDS_EMEMALLOC );
	FreeUp( (LPTR)lpDIB );
	return( NULL );
	}

if ( hOldPal )
	{
//	SelectPalette( hDC, hOldPal, FALSE );
//	RealizePalette( hDC );
	}

if ( !bDCPassedIn )
	ReleaseDC( NULL, hDC );

lpDIB->biClrUsed = bi.biClrUsed;
return( lpDIB );
}


//**********************************************************************
LPFRAME BitmapToFrame( HDC hDC, HBITMAP hBitmap, HPALETTE hPal )
//**********************************************************************
{
LPBITMAPINFOHEADER lpDIB;
LPFRAME lpFrame;

if ( !(lpDIB = BitmapToDIB( hDC, hBitmap, hPal )) )
	return( NULL );
lpFrame = DIBToFrame( (LPTR)lpDIB, YES );
FreeUp( (LPTR)lpDIB );
return( lpFrame );
}


/***********************************************************************/
void DrawBitmap( HDC hDCscreen, HBITMAP hBitmap, int cx, int cy )
/***********************************************************************/
{
BITMAP bm;
HDC hDCbitmap;

if ( !hDCscreen )
	return;
if ( !(hDCbitmap = CreateCompatibleDC(hDCscreen)) )
	return;

GetObject( hBitmap, sizeof(BITMAP), (LPSTR)&bm );
cx -= bm.bmWidth/2;
cy -= bm.bmHeight/2;

SelectObject( hDCbitmap, hBitmap );
BitBlt( hDCscreen, cx, cy, bm.bmWidth, bm.bmHeight, hDCbitmap, 0, 0, SRCCOPY );
DeleteDC( hDCbitmap );
}


//************************************************************************
HBITMAP DrawTransDIB( HDC hDCscreen, HBITMAP hBMobject, HBITMAP hBMmask, int cx, int cy )
//************************************************************************
{
BITMAP bm;
HDC hDCcompose, hDCobject, hDCmask;
HBITMAP hBMcompose;
#define SRCNAND 0x00220326

if ( !hDCscreen )
	return( NULL );
if ( !hBMobject )
	return( NULL );
GetObject( hBMobject, sizeof(BITMAP), &bm );

hBMcompose = NULL;
hDCcompose = NULL;
hDCmask    = NULL;
hDCobject  = NULL;

// Setup the compose DC and bitmap
if ( !(hDCcompose = CreateCompatibleDC( NULL )) )
	goto ErrorExit;
if ( !(hBMcompose = CreateCompatibleBitmap( hDCscreen, bm.bmWidth, bm.bmHeight)) )
	goto ErrorExit;
cx -= bm.bmWidth/2;
cy -= bm.bmHeight/2;
SelectObject( hDCcompose, hBMcompose );
BitBlt( hDCcompose, 0, 0, bm.bmWidth, bm.bmHeight, hDCscreen, cx, cy, SRCCOPY );

// Setup the object DC and bitmap
if ( !(hDCobject = CreateCompatibleDC( NULL )) )
	goto ErrorExit;
SelectObject( hDCobject, hBMobject );

// Setup the mask DC and bitmap
if ( !(hDCmask = CreateCompatibleDC( NULL )) )
	goto ErrorExit;
if ( hBMmask )
	{
	SelectObject( hDCmask, hBMmask );
	BitBlt( hDCobject, 0, 0, bm.bmWidth, bm.bmHeight, hDCmask, 0, 0, SRCNAND );
	}
else
	{
	if ( !(hBMmask = MakeBitmapMono( hDCmask, hDCobject, hBMobject, &bm )) )
		goto ErrorExit;
	}

BitBlt( hDCcompose, 0, 0, bm.bmWidth, bm.bmHeight, hDCmask, 0, 0, SRCAND );
BitBlt( hDCcompose, 0, 0, bm.bmWidth, bm.bmHeight, hDCobject, 0, 0, SRCPAINT );

// Cleanup the mask
DeleteDC( hDCmask );

// Cleanup the object
DeleteDC( hDCobject );

BitBlt( hDCscreen, cx, cy, bm.bmWidth, bm.bmHeight, hDCcompose, 0, 0, SRCCOPY );

// Cleanup the compose
DeleteDC( hDCcompose );
DeleteObject( hBMcompose );
return( hBMmask );

ErrorExit:
if ( hDCmask )		DeleteDC( hDCmask );
if ( hDCobject )	DeleteDC( hDCobject );
if ( hDCcompose )	DeleteDC( hDCcompose );
if ( hBMcompose )	DeleteObject( hBMcompose );
return( hBMmask );
}


//************************************************************************
HBITMAP MakeBitmapMono( HDC hDCmask, HDC hDCobject, HBITMAP hBMobject, LPBITMAP lpBM )
//************************************************************************
{
HBITMAP hBMmask;
BITMAP  bm;
HPALETTE hPal, hOldPal;
COLORREF cOldColor, cColor;

if ( !hDCmask || !hDCobject || !lpBM )
	return( NULL );
bm = *lpBM;

if ( !(hBMmask = CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL )) )
	return( NULL );

SelectObject( hDCmask, hBMmask );
if ( hPal = CopySystemPalette() )
	{
	hOldPal = SelectPalette( hDCobject, hPal, FALSE );
	RealizePalette( hDCobject );
	cColor = GetPixel( hDCobject, 0, 0 ) | 0x02000000; // make PALETTERGB()
	}
else
	cColor = GetPixel( hDCobject, 0, 0 );
cOldColor = SetBkColor( hDCobject, cColor );
BitBlt( hDCmask, 0, 0, bm.bmWidth, bm.bmHeight, hDCobject, 0, 0, SRCCOPY );
BitBlt( hDCobject, 0, 0, bm.bmWidth, bm.bmHeight, hDCmask, 0, 0, SRCNAND );
SetBkColor( hDCobject, cOldColor );
if ( hPal )
	{
	SelectPalette( hDCobject, hOldPal, FALSE );
	RealizePalette( hDCobject );
	DeleteObject( hPal );
	}
return( hBMmask );
}


#ifdef UNUSED
//************************************************************************
static void Change16ColorDither( BOOL bOn )
//************************************************************************
{
static WORD wOldThreshold16Colors;
extern WORD wThreshold16Colors;

// Stub this routines out unless someone doesn't like the fine dithering
return;

if ( bOn )
	{ // restore the old threshold value, if any
	wThreshold16Colors = wOldThreshold16Colors;
	}
else
	{ // turn dithering off by setting the thresholds to 128's
	wOldThreshold16Colors = wThreshold16Colors;
	wThreshold16Colors = MAKEWORD( 128, 128 );
	}
}
#endif
