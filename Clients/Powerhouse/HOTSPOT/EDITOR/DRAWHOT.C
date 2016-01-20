/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/


/*
 * This file contains an AVI DrawProc, and various functions that the
 * DrawProc calls (see the list of forward declars for a list of these
 * functions; the ICAVIDrawProc is the only function visible outside
 * this file).
 *
 * The drawproc draws red rectangles on an AVI to represent hotspots.
 * The drawproc also slows down the AVI playback, because it copies the
 * entire image onto a device-dependent bitmap for drawing the rectangles
 *
 */
 
#include <windows.h>
#include <windowsx.h>
#include <vfw.h>

#include "hotspot.h"

// unfortunately I had to use a global variable to get this to work.
extern PMOVIEINFO pMovieInfo;

// junk for AVIDrawGetInfo
#define SZCODE char _based(_segname("_CODE"))
static SZCODE szDescription[] = "Microsoft Hotspot Draw handler";
static SZCODE szName[]        = "MS Hotspot";
#define FOURCC_AVIDraw      mmioFOURCC('H','O','T','T')
#define VERSION_AVIDraw     0x00010000

#ifndef HUGE
#define HUGE _huge
#endif

// this is the structure that video for windows hangs on to for us.
// We allocate it and give vfw a pointer when the drawproc is
// opened, and it gives it back while the frames are being drawn.
// We store such things as our DC and our pen to draw the rectangles
// with in here.
typedef struct
{
	HDRAWDIB			hdd;
	HDC                 hdc;
	int                 xDst;
	int                 yDst;
	int                 dxDst;
	int                 dyDst;
	int                 xSrc;
	int                 ySrc;
	int                 dxSrc;
	int                 dySrc;
	HBITMAP				hddb;
	HDC					hMemdc;
	HBITMAP				hOldbmp;
	HBRUSH				hBrush;
	HBRUSH				hOldBrush;
	HPEN				hPen;
	HPEN				hOldPen;
	PMOVIEINFO			lpMovie;
} INSTINFO, FAR * PINSTINFO;


// forward declars
LONG FAR PASCAL _export ICAVIDrawProc(DWORD id, HDRVR hDriver,
	UINT uiMessage, LPARAM lParam1, LPARAM lParam2);
static LONG NEAR PASCAL AVIDrawOpen(ICOPEN FAR * icopen);
static LONG NEAR PASCAL AVIDrawClose(PINSTINFO pi);
static LONG NEAR PASCAL AVIDrawGetInfo(ICINFO FAR *icinfo, LONG lSize);
static LONG NEAR PASCAL AVIDrawQuery(PINSTINFO pi, LPBITMAPINFOHEADER lpbiIn);
static LONG NEAR PASCAL AVIDrawSuggestFormat(PINSTINFO pi, ICDRAWSUGGEST FAR *lpicd, LONG cbicd);
static LONG NEAR PASCAL AVIDrawBegin(PINSTINFO pi, ICDRAWBEGIN FAR *lpicd, LONG cbicd);
static LONG NEAR PASCAL AVIDraw(PINSTINFO pi, ICDRAW FAR *lpicd, LONG cbicd);
static LONG NEAR PASCAL AVIDrawEnd(PINSTINFO pi);
static LONG NEAR PASCAL AVIDrawChangePalette(PINSTINFO pi, LPBITMAPINFOHEADER lpbi);

// macros used by DIB stuff.
// hey, we're not using DIB stuff anymore...
#define WIDTHBYTES(i)     ((unsigned)((i+31)&(~31))/8)
#define DIBWIDTHBYTES(bi) (DWORD)WIDTHBYTES((int)(bi).biWidth * (int)(bi).biBitCount)

// Here we create a device-dependent bitmap in memory to draw into.
// We'll copy the DIB onto this bitmap, draw rects on it, then copy
// the result to the screen.  This can be done more efficiently using
// the DIB driver that comes with Windows 3.1, if anybody wants to 
// recode it...
// We also create our red pen here, as well as a brush for doing
// =hollow= rectangles
void CreateMemoryDC(PINSTINFO pi)
{
	LOGBRUSH lb;
	pi->hddb = CreateCompatibleBitmap(pi->hdc,pi->dxDst,pi->dyDst);
	pi->hMemdc = CreateCompatibleDC(pi->hdc);
	pi->hOldbmp = SelectObject(pi->hMemdc,pi->hddb);
	lb.lbStyle = BS_NULL;
	pi->hBrush = CreateBrushIndirect(&lb);
	pi->hOldBrush = SelectObject(pi->hMemdc,pi->hBrush);
	pi->hPen = CreatePen(PS_SOLID,0,RGB(255,0,0));
	pi->hOldPen = SelectObject(pi->hMemdc,pi->hPen);
}

// here we undo everything CreateMemoryDC did.
void DisintegrateMemoryDC(PINSTINFO pi)
{
	SelectObject(pi->hMemdc,pi->hOldPen);
	DeleteObject(pi->hPen);
	SelectObject(pi->hMemdc,pi->hOldBrush);
	DeleteObject(pi->hBrush);
	SelectObject(pi->hMemdc,pi->hOldbmp);
	DeleteObject(pi->hddb); pi->hddb = NULL;
	DeleteDC(pi->hMemdc); pi->hMemdc = NULL;
}

// this is the drawproc that vfw calls.  It basically just hands
// stuff off to other functions later in this file.  A few things
// are handled here.  This is message-handling proc just like a
// Window Proc or a VBX Control Proc...
LONG FAR PASCAL _export ICAVIDrawProc(DWORD id, HDRVR hDriver,
	UINT uiMessage, LPARAM lParam1, LPARAM lParam2)
{
	PINSTINFO pi = (PINSTINFO)id;
	switch (uiMessage)
	{
	case DRV_LOAD:
	case DRV_FREE:
		return 1;
	case DRV_OPEN:
		if (lParam2 == 0L)
		{
			return 1;
		}
		return AVIDrawOpen((ICOPEN FAR *)lParam2);
	case DRV_CLOSE:
		return AVIDrawClose(pi);
	case DRV_QUERYCONFIGURE:
		return 0;
	case DRV_CONFIGURE:
		return 1;
	case ICM_CONFIGURE:
	case ICM_ABOUT:
		return ICERR_UNSUPPORTED;
	case ICM_GETSTATE:
		return 0L;
	case ICM_GETINFO:
		return AVIDrawGetInfo((ICINFO FAR *)lParam1, lParam2);
	case ICM_DRAW_QUERY:
		return AVIDrawQuery(pi, (LPBITMAPINFOHEADER)lParam1);
	case ICM_DRAW_SUGGESTFORMAT:
		return AVIDrawSuggestFormat(pi, (ICDRAWSUGGEST FAR *) lParam1, lParam2);
	case ICM_DRAW_BEGIN:
		return AVIDrawBegin(pi, (ICDRAWBEGIN FAR *) lParam1, lParam2);
	case ICM_DRAW_REALIZE:
		pi->hdc = (HDC) lParam1;
		if (!pi->hdc || !pi->hdd)
			break;
		return DrawDibRealize(pi->hdd, pi->hdc, (BOOL) lParam2);
	case ICM_DRAW_GET_PALETTE:
		if (!pi->hdd)
			break;
		return (LONG) (UINT) DrawDibGetPalette(pi->hdd);
	case ICM_DRAW:
		return AVIDraw(pi, (ICDRAW FAR *)lParam1, lParam2);
	case ICM_DRAW_CHANGEPALETTE:
		return AVIDrawChangePalette(pi, (LPBITMAPINFOHEADER) lParam1);
	case ICM_DRAW_END:
		return AVIDrawEnd(pi);
	case DRV_DISABLE:
	case DRV_ENABLE:
		return 1;
	case DRV_INSTALL:
	case DRV_REMOVE:
		return 1;
	}
	if (uiMessage < DRV_USER)
	{
		return DefDriverProc(id,hDriver,uiMessage,lParam1,lParam2);
	}
	else
	{
		return ICERR_UNSUPPORTED;
	}
}


// This basically allocates our "instance info" (that struct defined
// at the beginning of this file).
static LONG NEAR PASCAL AVIDrawOpen(ICOPEN FAR * icopen)
{
	PINSTINFO pinst;
	if (icopen->fccType != streamtypeVIDEO)
	{
		return 0;
	}
	if (icopen->dwFlags == ICMODE_COMPRESS)
	{
		return 0;
	}
	if (icopen->dwFlags == ICMODE_DECOMPRESS)
	{
		return 0;
	}
	pinst = (PINSTINFO)GlobalAllocPtr(GHND, sizeof(INSTINFO));
	if (!pinst)
	{
		icopen->dwError = ICERR_MEMORY;
		return NULL;
	}
	pinst->hdd = DrawDibOpen();
	pinst->hddb = NULL;
	icopen->dwError = ICERR_OK;
	return (LONG) pinst;
}

// This frees our instance structure, and everything within the
// structure.
static LONG NEAR PASCAL AVIDrawClose(PINSTINFO pi)
{
	if (pi->hdd)
	{
		DrawDibClose(pi->hdd);
	}
	if (pi->hddb)
	{
		DisintegrateMemoryDC(pi);
	}
	GlobalFreePtr(pi);
	return 1;
}

// When AVI gets nosy, it goes here.  We pass it those strings and
// stuff defined at the top of this file.
static LONG NEAR PASCAL AVIDrawGetInfo(ICINFO FAR *icinfo, LONG lSize)
{
	if (icinfo == NULL)
	{
		return sizeof(ICINFO);
	}
	if (lSize < sizeof(ICINFO))
	{
		return 0;
	}
	icinfo->dwSize	    = sizeof(ICINFO);
	icinfo->fccType	    = ICTYPE_VIDEO;
	icinfo->fccHandler      = FOURCC_AVIDraw;
	icinfo->dwFlags	    = VIDCF_DRAW;
	icinfo->dwVersion       = VERSION_AVIDraw;
	icinfo->dwVersionICM    = ICVERSION;
	lstrcpy(icinfo->szDescription, szDescription);
	lstrcpy(icinfo->szName, szName);
	return sizeof(ICINFO);
}

// Make sure we can handle the format given.
static LONG NEAR PASCAL AVIDrawQuery(PINSTINFO pi,
	LPBITMAPINFOHEADER lpbiIn)
{
	if (lpbiIn == NULL)
	{
		return ICERR_BADFORMAT;
	}
	if (lpbiIn->biCompression != BI_RGB)
	{
		return ICERR_BADFORMAT;
	}
	return ICERR_OK;
}

// suggest BI_RGB
static LONG NEAR PASCAL AVIDrawSuggestFormat(PINSTINFO pi,
	ICDRAWSUGGEST FAR *lpicd, LONG cbicd)
{
	HIC hic;
	if (lpicd->lpbiSuggest == NULL)
	{
		return sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
	}
	hic = ICGetDisplayFormat(NULL, lpicd->lpbiIn, lpicd->lpbiSuggest,
		0, lpicd->dxDst, lpicd->dyDst);
	if (hic)
	{
		ICClose(hic);
	}
	if (lpicd->lpbiSuggest)
	{
		if (lpicd->lpbiSuggest->biCompression == BI_RLE8)
		{
			lpicd->lpbiSuggest->biCompression = BI_RGB;
		}
	}
	return sizeof(BITMAPINFOHEADER) + lpicd->lpbiSuggest->biClrUsed * sizeof(RGBQUAD);
}

// Here, we grab DC's and stuff from vfw and stash it into our own
// instance info.  We also get our movieinfo, which is the structure
// that has the actual hotspots in it.  It's in a global variable, even
// though I don't like global variables...
static LONG NEAR PASCAL AVIDrawBegin(PINSTINFO pi, ICDRAWBEGIN FAR *lpicd,
	LONG cbicd)
{
	LONG    l;
	l = AVIDrawQuery(pi, lpicd->lpbi);
	if ((l != 0) || (lpicd->dwFlags & ICDRAW_QUERY))
	{
		return l;
	}
	pi->hdc = lpicd->hdc;
	pi->xDst = lpicd->xDst;
	pi->yDst = lpicd->yDst;
	pi->dxDst = lpicd->dxDst;
	pi->dyDst = lpicd->dyDst;
	pi->xSrc = lpicd->xSrc;
	pi->ySrc = lpicd->ySrc;
	pi->dxSrc = lpicd->dxSrc;
	pi->dySrc = lpicd->dySrc;
	SetStretchBltMode(pi->hdc, COLORONCOLOR);
	if (!DrawDibBegin(pi->hdd, pi->hdc,
				pi->dxDst, pi->dyDst,
			lpicd->lpbi,
			pi->dxSrc, pi->dySrc,
			0))
	{
		return ICERR_UNSUPPORTED;
	}
	if (pi->hddb)
	{
		DisintegrateMemoryDC(pi);
	}
	CreateMemoryDC(pi);
	pi->lpMovie = pMovieInfo;
	return ICERR_OK;
}

// Several interesting things happen here:  1) We copy the DIB onto
// our own bitmap (hMemDC), with DrawDibDraw, then we draw rectangles
// onto that bitmap (or not) as we walk down the hotspot list (with
// Rectangle), then we BitBlt the result onto the screen.  I've heard
// rumors that this could be done with the DIB driver and get rid of
// the extra image copy.
static LONG NEAR PASCAL AVIDraw(PINSTINFO pi, ICDRAW FAR *lpicd, LONG cbicd)
{
	UINT  wFlags;
	wFlags = DDF_SAME_HDC;
	if ((lpicd->dwFlags & ICDRAW_NULLFRAME) || lpicd->lpData == NULL)
	{
		if (lpicd->dwFlags & ICDRAW_UPDATE)
		{
			wFlags |= DDF_UPDATE;
		}
		else
		{
			return ICERR_OK;
		}
	}
	if (lpicd->dwFlags & ICDRAW_PREROLL)
	{
		wFlags |= DDF_DONTDRAW;
	}
	if (lpicd->dwFlags & ICDRAW_HURRYUP)
	{
		wFlags |= DDF_HURRYUP;
	}
	if (pi->lpMovie)
	{
		if(DrawDibDraw(pi->hdd, pi->hMemdc,
				0,0,
				pi->dxDst, pi->dyDst,
				lpicd->lpFormat,
				lpicd->lpData,
				pi->xSrc, pi->ySrc,
				pi->dxSrc, pi->dySrc,
				wFlags))
		{
			PHOTSPOT pHotspot;
			pHotspot = (pi->lpMovie)->pHotspotList;
			while (pHotspot)
			{
				if ((pHotspot->BeginFrame <
					(lpicd->lTime)+1+(pi->lpMovie)->lLastSeek) &&
					(pHotspot->EndFrame > 
					(lpicd->lTime)-1+(pi->lpMovie)->lLastSeek))
				{
					Rectangle(pi->hMemdc,
						pHotspot->rc.left,
						pHotspot->rc.top,
						pHotspot->rc.right,
						pHotspot->rc.bottom);
				}
				pHotspot = pHotspot->pNext;
			}
			BitBlt(pi->hdc,pi->xDst,pi->yDst,pi->dxDst,pi->dyDst,
				pi->hMemdc,0,0,SRCCOPY);
		}
	}
	else if (!DrawDibDraw(pi->hdd, pi->hdc,
				pi->xDst, pi->yDst,
			pi->dxDst, pi->dyDst,
			lpicd->lpFormat,
			lpicd->lpData,
			pi->xSrc, pi->ySrc,
			pi->dxSrc, pi->dySrc,
			wFlags))
	{
		if (wFlags & DDF_UPDATE)
		{
			return ICERR_CANTUPDATE;
		}
		else
		{
			return ICERR_UNSUPPORTED;
		}
	}
	return ICERR_OK;
}

// Give the pallette back to vfw with DrawDibChangePallette
static LONG NEAR PASCAL AVIDrawChangePalette(PINSTINFO pi,
	LPBITMAPINFOHEADER lpbi)
{
	PALETTEENTRY    ape[256];
	LPRGBQUAD	    lprgb;
	int i;
	lprgb = (LPRGBQUAD) ((LPBYTE) lpbi + lpbi->biSize);
	for (i = 0; i < (int) lpbi->biClrUsed; i++)
	{
		ape[i].peRed = lprgb[i].rgbRed;
		ape[i].peGreen = lprgb[i].rgbGreen;
		ape[i].peBlue = lprgb[i].rgbBlue;
		ape[i].peFlags = 0;
	}
	DrawDibChangePalette(pi->hdd, 0, (int) lpbi->biClrUsed,
		(LPPALETTEENTRY)ape);
	return ICERR_OK;
}

// Everything is freed in AVIDrawClose, nothing to do here...
static LONG NEAR PASCAL AVIDrawEnd(PINSTINFO pi)
{
	return ICERR_OK;
}
