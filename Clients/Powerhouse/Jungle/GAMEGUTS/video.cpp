#include "windows.h"
#include "mmsystem.h"
#include "digitalv.h"
#include "proto.h"
#include "mciavi.h"
#include "scene.h"

#define NOAVIFILE
//#define NOCOMPMAN
//#define NODRAWDIB
#define NOVIDEO
//#define NOAVIFMT
#define NOMMREG
#define NOMCIWND
#define NOAVICAP
#define NOMSACM
#include "vfw.h"

long FAR PASCAL EXPORT MCIDrawProc( DWORD id, HDRVR hDriver,
	UINT uiMessage, LPARAM lParam1, LPARAM lParam2 );

static VIDEO_DRAWDIB_PROC lpMyDrawDibProc;
static VIDEO_DRAWDC_PROC lpMyDrawDCProc;

//************************************************************************
void CALLBACK TestDrawDCProc( HDC hDC )
//************************************************************************
{
	MoveToEx( hDC, 0, 0, NULL );
	LineTo( hDC, 640, 480 );
	MoveToEx( hDC, 640, 0, NULL );
	LineTo( hDC, 0, 480 );	
}

//************************************************************************
void VideoInit( BOOL bZoomBy2, BOOL bFullScreen )
//************************************************************************
{
if ( bFullScreen )
	WriteProfileString( "MCIAVI", "DefaultVideo", "240 Line Fullscreen" );
else
	WriteProfileString( "MCIAVI", "DefaultVideo", "Window" );

//STRING szString;
//wsprintf( szString, "%d", bZoomBy2 );
//WriteProfileString( "MCIAVI", "ZoomBy2",	szString );
WriteProfileString( "MCIAVI", "ZoomBy2", "0" );
WriteProfileString( "MCIAVI", "SkipFrames", "1" );
WriteProfileString( "MCIAVI", "UseAVIFile", "0" );
}

//************************************************************************
void VideoDone(void)
//************************************************************************
{
}

//***************************************************************************
BOOL MCISetVideoDrawProc( WORD wDeviceID, VIDEO_DRAWDIB_PROC lpDrawDibProc, VIDEO_DRAWDC_PROC lpDrawDCProc )
//***************************************************************************
{
DWORD dwRes;
MCI_DGV_SETVIDEO_PARMS mciSet;
DWORD dwDrawProc;

if ( !wDeviceID )
	return FALSE;

lpMyDrawDibProc = lpDrawDibProc;
lpMyDrawDCProc	= lpDrawDCProc;

if ( !lpMyDrawDibProc && !lpMyDrawDCProc )
	dwDrawProc = NULL;
else
	dwDrawProc = (DWORD)MCIDrawProc;

mciSet.dwValue = dwDrawProc; //MakeProcInstance( lpDrawProc, GetApp()->GetInstance() );
mciSet.dwItem = MCI_AVI_SETVIDEO_DRAW_PROCEDURE;

if ( (dwRes = mciSendCommand( wDeviceID, MCI_SETVIDEO,
		(DWORD)MCI_DGV_SETVIDEO_ITEM | MCI_DGV_SETVIDEO_VALUE,
		(DWORD)(LPMCI_DGV_SETVIDEO_PARMS)&mciSet )) )
	{
	MCIError(dwRes);
	return FALSE;
	}

return TRUE;
}



// this is the structure that video for windows hangs on to for us.
// We allocate it and give vfw a pointer when the drawproc is
// opened, and it gives it back while the frames are being drawn.
typedef struct
{
	HDRAWDIB			hdd;
	HDC					hdc;
	int					xDst;
	int					yDst;
	int					dxDst;
	int					dyDst;
	int					xSrc;
	int					ySrc;
	int					dxSrc;
	int					dySrc;
} INSTINFO, FAR * PINSTINFO;

// This basically allocates our "instance info" (that struct defined above)
//***************************************************************************
static long AVIDrawOpen(ICOPEN FAR * icopen)
//***************************************************************************
{
	PINSTINFO pinst;

	if (!icopen)
	   return 1;
	if (icopen->fccType != streamtypeVIDEO)
		return NULL;
	if (icopen->dwFlags == ICMODE_COMPRESS || icopen->dwFlags == ICMODE_DECOMPRESS)
		return NULL;
	if ( !(pinst = (PINSTINFO)AllocX(sizeof(INSTINFO), GMEM_ZEROINIT)) )
	{
		icopen->dwError = ICERR_MEMORY;
		return NULL;
	}
	pinst->hdd = DrawDibOpen();
	icopen->dwError = ICERR_OK;
	return (LONG) pinst;
}

// This frees our instance structure, and everything within the structure.
//***************************************************************************
static long AVIDrawClose(PINSTINFO pi)
//***************************************************************************
{
	if (pi->hdd)
		DrawDibClose(pi->hdd);
	FreeUp((LPTR)pi);
	return ICERR_OK;
}

// When AVI gets nosy, it goes here.  We pass it those strings and
// stuff defined at the top of this file.
//***************************************************************************
static long AVIDrawGetInfo(ICINFO FAR *icinfo, LONG lSize)
//***************************************************************************
{
#define FOURCC_AVIDraw		mmioFOURCC('v','i','d','c')
#define VERSION_AVIDraw		0x00010000

	if (!icinfo)
		return sizeof(ICINFO);
	if (lSize < sizeof(ICINFO))
		return 0;
	icinfo->dwSize		 = sizeof(ICINFO);
	icinfo->fccType		 = ICTYPE_VIDEO;
	icinfo->fccHandler	 = FOURCC_AVIDraw;
	icinfo->dwFlags		 = VIDCF_DRAW;
	icinfo->dwVersion	 = VERSION_AVIDraw;
	icinfo->dwVersionICM = ICVERSION;
	lstrcpy((LPSTR)icinfo->szDescription, "Huh?");
	lstrcpy((LPSTR)icinfo->szName, "Huh?");
	return sizeof(ICINFO);
}

// Make sure we can handle the format given.
//***************************************************************************
static long AVIDrawQuery(LPBITMAPINFOHEADER lpbiIn)
//***************************************************************************
{
	return ICERR_OK;
}

// suggest BI_RGB
//***************************************************************************
static long AVIDrawSuggestFormat(ICDRAWSUGGEST FAR *lpicd, LONG cbicd)
//***************************************************************************
{
	return ICERR_OK;
}

// Here, we grab DC's and stuff from vfw and stash it into our own structure
//***************************************************************************
static long AVIDrawBegin(PINSTINFO pi, ICDRAWBEGIN FAR *lpicd, LONG cbicd)
//***************************************************************************
{
	if (lpicd->dwFlags & ICDRAW_QUERY)
		return ICERR_BADFORMAT;
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
	if (!DrawDibBegin(pi->hdd, pi->hdc, pi->dxDst, pi->dyDst, lpicd->lpbi, pi->dxSrc, pi->dySrc, 0))
		return ICERR_UNSUPPORTED;
	return ICERR_OK;
}

//***************************************************************************
static long AVIDraw(PINSTINFO pi, ICDRAW FAR *lpicd, LONG cbicd)
//***************************************************************************
{
	UINT  wFlags;

	wFlags = DDF_SAME_HDC;
	if ((lpicd->dwFlags & ICDRAW_NULLFRAME) || !lpicd->lpData)
	{
		if (lpicd->dwFlags & ICDRAW_UPDATE)
			wFlags |= DDF_UPDATE;
		else
			return ICERR_OK;
	}
	if (lpicd->dwFlags & ICDRAW_PREROLL)
		wFlags |= DDF_DONTDRAW;
	if (lpicd->dwFlags & ICDRAW_HURRYUP)
		wFlags |= DDF_HURRYUP;

//	if ( lpMyDrawDibProc )
//	{
//		CDib MyDib( (LPBITMAPINFOHEADER)lpicd->lpFormat, ((LPBITMAPINFO)lpicd->lpFormat)->bmiColors, (LPTR)lpicd->lpData );
//		(*lpMyDrawDibProc)( &MyDib );
//	}
#ifdef UNUSED
	LPSCENE lpScene = CScene::GetScene(GetParent(GetFocus()));
	if ( lpScene )
	{
		PDIB lpDib;
		lpScene->GetDIBs( NULL, &lpDib, NULL );
		if ( lpDib )
		{
			CDib MyDib( (LPBITMAPINFOHEADER)lpicd->lpFormat, ((LPBITMAPINFO)lpicd->lpFormat)->bmiColors, (LPTR)lpicd->lpData );
			lpDib->DibBlt( &MyDib,	100/*dstLeft*/, 100/*dstTop*/, 100/*dstWidth*/, 100/*dstHeight*/,
									100/*srcLeft*/, 100/*srcTop*/, 100/*srcWidth*/, 100/*srcHeight*/,
									YES/*bTransparent*/, NULL/*lpRGB*/, NULL/*lpLut*/, NULL/*hPal*/ );
		}
	}
#endif

	if (!DrawDibDraw(pi->hdd, pi->hdc, pi->xDst, pi->yDst, pi->dxDst, pi->dyDst,
			(LPBITMAPINFOHEADER)lpicd->lpFormat, lpicd->lpData, pi->xSrc, pi->ySrc,
			pi->dxSrc, pi->dySrc, wFlags))
	{
		if (wFlags & DDF_UPDATE)
			return ICERR_CANTUPDATE;
		else
			return ICERR_UNSUPPORTED;
	}

//	if ( lpMyDrawDCProc )
//		(*lpMyDrawDCProc)( pi->hdc );

	TestDrawDCProc( pi->hdc );

	return ICERR_OK;
}

// Give the pallette back to vfw with DrawDibChangePallette
//***************************************************************************
static long AVIDrawChangePalette(PINSTINFO pi, LPBITMAPINFOHEADER lpbi)
//***************************************************************************
{
	PALETTEENTRY	ape[256];
	LPRGBQUAD		lprgb;
	int i;

	lprgb = (LPRGBQUAD) ((LPBYTE) lpbi + lpbi->biSize);
	for (i = 0; i < (int) lpbi->biClrUsed; i++)
	{
		ape[i].peRed = lprgb[i].rgbRed;
		ape[i].peGreen = lprgb[i].rgbGreen;
		ape[i].peBlue = lprgb[i].rgbBlue;
		ape[i].peFlags = 0;
	}
	DrawDibChangePalette(pi->hdd, 0, (int) lpbi->biClrUsed, (LPPALETTEENTRY)ape);
	return ICERR_OK;
}

//***************************************************************************
long FAR PASCAL EXPORT MCIDrawProc(DWORD id, HDRVR hDriver, UINT message, LPARAM lParam1, LPARAM lParam2)
//***************************************************************************
{
	PINSTINFO pi;

	switch (message)
	{
	// First, all the messages we ignore
	case DRV_LOAD:
	case DRV_FREE:
	case DRV_CONFIGURE:
	case DRV_DISABLE:
	case DRV_ENABLE:
	case DRV_INSTALL:
	case DRV_REMOVE:
		return 1L; //ICERR_UNSUPPORTED; // JMM try unsupported instead of 1L
	
	case ICM_CONFIGURE:
	case ICM_ABOUT:
	case ICM_DRAW_START_PLAY:
		return ICERR_UNSUPPORTED;
		
	// Next, all the messages we ignore and just return ICERR_OK (0):
	// Note, these may be used to store state information.
	// It is unclear if they will definitetly be called.
	case DRV_QUERYCONFIGURE:
	case ICM_GETSTATE:
	case ICM_SETSTATE:
	case ICM_DRAW_QUERY: // Determine if the input can be processed
	case ICM_DRAW_SUGGESTFORMAT: // Suggest the output format
	case ICM_DRAW_END: // Cleanup after a draw operation
		return ICERR_OK;

	// The draw messages we care about
	case DRV_OPEN: // Open the rendering driver
		return AVIDrawOpen((ICOPEN FAR *)lParam2);
	case ICM_DRAW_BEGIN: // Prepare to draw the video data
		return AVIDrawBegin((PINSTINFO)id, (ICDRAWBEGIN FAR *) lParam1, lParam2);
	case ICM_GETINFO:
		return AVIDrawGetInfo((ICINFO FAR *)lParam1, lParam2);
	case ICM_DRAW: // Draw the video data
		return AVIDraw((PINSTINFO)id, (ICDRAW FAR *)lParam1, lParam2);
	case ICM_DRAW_CHANGEPALETTE:
		return AVIDrawChangePalette((PINSTINFO)id, (LPBITMAPINFOHEADER) lParam1);
	case ICM_DRAW_GET_PALETTE:
		if ( !(pi = (PINSTINFO)id) )	return ICERR_UNSUPPORTED;
		if (!pi->hdc || !pi->hdd)		return ICERR_UNSUPPORTED;
		return (UINT)DrawDibGetPalette(pi->hdd);
	case ICM_DRAW_REALIZE:
		if ( !(pi = (PINSTINFO)id) )	return ICERR_UNSUPPORTED;
		pi->hdc = (HDC)lParam1;
		if (!pi->hdc || !pi->hdd)		return ICERR_UNSUPPORTED;
		return DrawDibRealize(pi->hdd, pi->hdc, (BOOL) lParam2);
	case DRV_CLOSE: // Close the rendering driver
		return AVIDrawClose((PINSTINFO)id);

	default:
		if (message < DRV_USER)
			return DefDriverProc(id, hDriver, message, lParam1, lParam2);
		else
			return ICERR_UNSUPPORTED;
	}

	return ICERR_UNSUPPORTED;
}

// Routines no longer used
// static long AVIDrawQuery(LPBITMAPINFOHEADER lpbiIn)
// static long AVIDrawSuggestFormat(ICDRAWSUGGEST FAR *lpicd, LONG cbicd)
// static long AVIDrawGetInfo(ICINFO FAR *icinfo, LONG lSize)
