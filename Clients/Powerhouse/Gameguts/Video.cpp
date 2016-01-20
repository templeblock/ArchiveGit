#include "windows.h"
#include "mmsystem.h"
#include "digitalv.h"
#include "proto.h"
#include "mciavi.h"
#include "scene.h"
#include "vidobsrv.h"

//#define PROFILE

// Profiling (mc) //
#ifdef PROFILE
long lStartAVIDraw;
long lTotalAVIDraw=0;
long lCountAVIDraw=0;

long lStartNotify;
long lTotalNotify=0;
long lCountNotify=0;

long lStartDibDraw;
long lTotalDibDraw=0;
long lCountDibDraw=0;
#endif 
////////////////// end profiling vars

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

static PINSTINFO pinst = NULL;

typedef struct
{
	HWND hNotifyWnd;
	WORD wTimerId;
} VideoTimer;

// Locals

// Video Observer vars
static const int nMaxObservers = 5;					// Maximum number of video observers
static int nNumObservers = 0;						// Number of observers
static VideoObserver *pObservers[nMaxObservers];	// Video observer array

// Video timer vars
static const int nMaxVideoTimers = 5;				// Maximum nuber of video timers
static int nNumVideoTimers = 0;						// Number of video timers
static VideoTimer Timers[nMaxVideoTimers];			// Video timer array

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

//************************************************************************
BOOL VideoSetObserver(VideoObserver *pObserver)
//************************************************************************
{
	if (nNumObservers == nMaxObservers)
		return FALSE;

	pObservers[nNumObservers] = pObserver;

	++nNumObservers;
	return TRUE;
}

//***************************************************************************
BOOL VideoClearObserver(VideoObserver *pObserver) 
//***************************************************************************
{
	// Find observer
	for (int i=0; i<nNumObservers; i++)
	{
		if (pObservers[i] == pObserver)
			break;
	}

	// If found then clear 
	if (i < nNumObservers )
	{
		for (int j=i+1; j<nNumObservers; j++, i++)
		{
			pObservers[i] = pObservers[j];
		}

		--nNumObservers;

		return TRUE;
	}

	return FALSE;
}

//***************************************************************************
void VideoClearAllObservers(void) 
//***************************************************************************
{
	// Clear all observers
	for (int i=0; i<nMaxObservers; i++)
	{
		pObservers[i] = NULL;
	}

	nNumObservers = 0;
}

//***************************************************************************
static void VideoNotifyOpen() 
//***************************************************************************
{
	for (int i=0; i<nNumObservers; i++)
	{
		pObservers[i]->OnVideoOpen();
	}
}

//***************************************************************************
static void	VideoNotifyDraw(LPBITMAPINFOHEADER lpFormat, LPTR lpData) 
//***************************************************************************
{
	for (int i=0; i<nNumObservers; i++)
	{
		pObservers[i]->OnVideoDraw(lpFormat, lpData);
	}
}

//***************************************************************************
static void VideoNotifyClose()
//***************************************************************************
{
	for (int i=0; i<nNumObservers; i++)
	{
		pObservers[i]->OnVideoClose();
	}
}

//************************************************************************
BOOL VideoSetTimer(HWND hNotifyWnd, WORD wId)
//************************************************************************
{
	if (nNumVideoTimers == nMaxVideoTimers)
		return FALSE;
	
	Timers[nNumVideoTimers].hNotifyWnd = hNotifyWnd;
	Timers[nNumVideoTimers].wTimerId = wId;

	++nNumVideoTimers;

	return TRUE;
}

//************************************************************************
BOOL VideoKillTimer(HWND hNotifyWnd, WORD wId)
//************************************************************************
{
	// Find timer
	for (int i=0; i<nNumVideoTimers; i++)
	{
		if (Timers[i].hNotifyWnd == hNotifyWnd && Timers[i].wTimerId == wId)
			break;
	}

	// If found then clear
	if (i < nNumVideoTimers)
	{
		for(int j=i+1; j<nNumVideoTimers; j++, i++)
		{
			Timers[i].hNotifyWnd = Timers[j].hNotifyWnd;
			Timers[i].wTimerId = Timers[j].wTimerId;
		}

		--nNumVideoTimers;

		return TRUE;
	}

	// Not found
	return FALSE;
}

//***************************************************************************
void VideoKillAllTimers(void) 
//***************************************************************************
{
	// Clear all timers
	for (int i=0; i<nMaxVideoTimers; i++)
	{
		Timers[i].hNotifyWnd = NULL;
		Timers[i].wTimerId = NULL;
		pObservers[i] = NULL;
	}

	nNumVideoTimers = 0;
}


#ifdef UNUSED_FOR_INTERIM
// Interim stuff
static int	nInterimFrames = 0;						// Number of interim video frames (between video shots)
static int	nCurInterimFrame;						// Current interim video frame
static UINT uTimerId = 0;							// Timer ID;
static HWND hVideoWnd = NULL;						// Video hWnd used by interim video to get DC
static BOOL bVideoEnd = TRUE;						// Video has ended flag
static LPBITMAPINFOHEADER lpbiInterim;				// Interim video bitmap header
static LPTR lpInterimBits;							// Interim video bitmap bits

VOID CALLBACK VideoInterimTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);

//***************************************************************************
BOOL VideoStartInterim(int nType, int nFrames, HWND hWnd, LPBITMAPINFOHEADER lpbi, LPTR lpDestBits)
//***************************************************************************
{
	// If this is already going on
	if (nInterimFrames)
		return FALSE;

	if (!pinst)
		return FALSE;

	// Save video window to get DC from later
	hVideoWnd = hWnd;

	lpbiInterim = (LPBITMAPINFOHEADER)AllocX(sizeof(BITMAPINFOHEADER), 0);
	if (!lpbiInterim)
		return FALSE;

	// Copy the bitmap header
	hmemcpy((LPBYTE)lpbiInterim, (LPBYTE)lpbi, sizeof(BITMAPINFOHEADER));

	// Set flags for the draw proc
	if (lpbiInterim->biSizeImage == 0)
	{
		long lWidthBytes = (((lpbi->biBitCount * (int)lpbi->biWidth) + 31) & (~31) ) / 8;
		lpbiInterim->biSizeImage = lpbiInterim->biWidth * lWidthBytes;
	}

	lpInterimBits = AllocX(lpbiInterim->biSizeImage, 0);
	if (!lpInterimBits)
	{
		FreeUp(lpbiInterim);
		return FALSE;
	}

	// Copy the bitmap data
	hmemcpy((LPBYTE)lpInterimBits, (LPBYTE)lpDestBits, lpbiInterim->biSizeImage);
	
	//DrawDibBegin(pinst->hdd, pinst->hdc, pinst->dxDst, pinst->dyDst, 
	//	lpbiInterim, pinst->dxSrc, pinst->dySrc, DDF_SAME_HDC | DDF_UPDATE | DDF_SAME_DRAW);
	
	//bRet = DrawDibDraw(pinst->hdd, pinst->hdc, pinst->xDst, pinst->yDst, 
	//	pinst->dxDst, pinst->dyDst, lpbiInterim, lpInterimBits, 
	//	pinst->xSrc, pinst->ySrc, pinst->dxSrc, pinst->dySrc, 
	//	DDF_SAME_DRAW|DDF_SAME_HDC);
	/*
	DrwBegin.hpal; 
    DrwBegin.hwnd; 
    DrwBegin.hdc; 
    DrwBegin.xDst; 
    DrwBegin.yDst; 
    DrwBegin.dxDst; 
    DrwBegin.dyDst; 
    DrwBegin.lpbi; 
    DrwBegin.xSrc; 
    DrwBegin.ySrc; 
    DrwBegin.dxSrc; 
    DrwBegin.dySrc; 
    DrwBegin.dwRate; 
    DrwBegin.dwScale; 
	*/

	// Set up the timer
	if ((uTimerId = SetTimer(NULL, 1, 66, VideoInterimTimerProc)) == 0)
		return FALSE;

	// Set flags, data
	nInterimFrames = nFrames;
	nCurInterimFrame = 0;

	return TRUE;
}

//***************************************************************************
VOID CALLBACK VideoInterimTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, 
									DWORD dwTime)
//***************************************************************************
{
	BOOL bRet; 

	//Debug("VideoInterimTimerProc\n");

	// Make sure there are frames, we can still get this after calling KillTimer
	if (nInterimFrames == 0)
		return;

	// If the video has not ended yet then don't draw yet
	if (!bVideoEnd)
		return;

	VideoNotifyDraw(lpbiInterim, lpInterimBits);

	//DrawDibBegin(pinst->hdd, pinst->hdc, pinst->dxDst, pinst->dyDst, 
	//	lpbiInterim, pinst->dxSrc, pinst->dySrc, DDF_SAME_HDC | DDF_UPDATE | DDF_SAME_DRAW);

	//Debug("VTP draw\n");

	bRet = DrawDibDraw(pinst->hdd, pinst->hdc, pinst->xDst, pinst->yDst, 
		pinst->dxDst, pinst->dyDst, lpbiInterim, lpInterimBits, 
		pinst->xSrc, pinst->ySrc, pinst->dxSrc, pinst->dySrc, 
		DDF_SAME_DRAW);

	//Delay(2000); // Debug

	if (bRet == 0)
	{
		DebugBreak();
	}

	++nCurInterimFrame;

	if (nCurInterimFrame >= nInterimFrames)
	{
		nInterimFrames = 0;
		VideoEndInterim();
	}
}

//***************************************************************************
void VideoEndInterim()
//***************************************************************************
{
	// do one last frame for the heck of it
	VideoInterimTimerProc(NULL, 0, 0, 0);
	
	nInterimFrames = 0;
	if (lpbiInterim)
	{
		FreeUp(lpbiInterim);
		FreeUp(lpInterimBits);
	}

	// kill timer
	if (uTimerId)
	{
		KillTimer(NULL, uTimerId);
		uTimerId = 0;
	}
}
#endif

//***************************************************************************
BOOL MCISetVideoDrawProc( HWND hVideoWnd, WORD wDeviceID )
//***************************************************************************
{
	DWORD dwRes;
	MCI_DGV_SETVIDEO_PARMS mciSet;
	DWORD dwDrawProc;

	if ( !wDeviceID )
		return FALSE;
	
	dwDrawProc = (DWORD)MCIDrawProc;

	mciSet.dwValue = dwDrawProc; //MakeProcInstance( lpDrawProc, GetApp()->GetInstance() );
	mciSet.dwItem = MCI_AVI_SETVIDEO_DRAW_PROCEDURE;

	if ( (dwRes = mciSendCommand( wDeviceID, MCI_SETVIDEO,
			(DWORD)MCI_DGV_SETVIDEO_ITEM | MCI_DGV_SETVIDEO_VALUE,
			(DWORD)(LPMCI_DGV_SETVIDEO_PARMS)&mciSet )) )
	{
		int i;

		// Previous call failed, so keep trying
		for(i=0; i<5; i++) 
		{
			// Let MMTASK have some time
			Delay(10);

			// Try again
			dwRes = mciSendCommand( wDeviceID, MCI_SETVIDEO,
					(DWORD)MCI_DGV_SETVIDEO_ITEM | MCI_DGV_SETVIDEO_VALUE,
					(DWORD)(LPMCI_DGV_SETVIDEO_PARMS)&mciSet );
			
			// If successful break out of loop
			if (dwRes == 0)
				break;
		}

		// It still failed, so put up a message
		if ( dwRes )
		{
			MCIError(dwRes);
			return FALSE;
		}
	}

	return TRUE;
}

// This basically allocates our "instance info" (that struct defined above)
//***************************************************************************
static long AVIDrawOpen(ICOPEN FAR * icopen)
//***************************************************************************
{
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
	pinst->hdc = NULL;

	icopen->dwError = ICERR_OK;

	VideoNotifyOpen();

	return (LONG) pinst;
}

// This frees our instance structure, and everything within the structure.
//***************************************************************************
static long AVIDrawClose(PINSTINFO pi)
//***************************************************************************
{
	#ifdef UNUSED_FOR_INTERIM
	// Check for a dc, we setup a temporary dc for video interim draws...
	if (pi->hdc && hVideoWnd)
		ReleaseDC(hVideoWnd, pi->hdc);
	#endif

	if (pi->hdd)
		DrawDibClose(pi->hdd);
	FreeUp((LPTR)pi);

	VideoNotifyClose();

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
static long AVIDrawQuery(PINSTINFO pi,LPBITMAPINFOHEADER lpbiIn)
//***************************************************************************
{
    // determine if the input DIB data is in a format we like.
    if (lpbiIn == NULL)
            return ICERR_BADFORMAT;

    // determine if the input DIB data is in a format we like.
    if (lpbiIn->biCompression != BI_RGB)
            return ICERR_BADFORMAT;

    return ICERR_OK;
}

// suggest BI_RGB
//***************************************************************************
static long AVIDrawSuggestFormat(PINSTINFO pi, ICDRAWSUGGEST FAR *lpicd, LONG cbicd)
//***************************************************************************
{

	HIC hic;
	LONG l;

	if (lpicd->lpbiSuggest == NULL)
			return sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

	// Call COMPMAN to get a good format to display data in....
	hic = ICGetDisplayFormat(NULL, lpicd->lpbiIn, lpicd->lpbiSuggest,
											0, lpicd->dxDst, lpicd->dyDst);

	if (hic)
			ICClose(hic);

	l = lpicd->lpbiSuggest->biClrUsed;

	if (lpicd->lpbiSuggest) 
	{
		if (lpicd->lpbiSuggest->biCompression == BI_RLE8)
		{
			lpicd->lpbiSuggest->biCompression = BI_RGB;
		}
			
	}

	// !!! Should check this format here to make sure it's RGB...
	// !!! If not, we could force it to 8-bit....
	lpicd->lpbiSuggest->biCompression = BI_RGB;
	lpicd->lpbiSuggest->biBitCount = 8;

	return ICERR_OK;
}

// Here, we grab DC's and stuff from vfw and stash it into our own structure
//***************************************************************************
static long AVIDrawBegin(PINSTINFO pi, ICDRAWBEGIN FAR *lpicd, LONG cbicd)
//***************************************************************************
{
    LONG l = AVIDrawQuery(pi, lpicd->lpbi);

    if ((l != 0) || (lpicd->dwFlags & ICDRAW_QUERY))
            return l;

	#ifdef UNUSED_FOR_INTERIM
	// See if hdc is present, if it is then the interim video had one and we 
	// need to release it
	if (pi->hdc && hVideoWnd)
	{
		// Use savedc since the interim video may try to used an incorrupt
		// dc right after we free it
		HDC hSaveDC = pi->hdc;
		pi->hdc = lpicd->hdc;
		ReleaseDC(hVideoWnd, hSaveDC);
	}
	#endif

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
static long AVIDrawEnd(void)
//***************************************************************************
{
	pinst->hdc = NULL;

	#ifdef PROFILE
		Debug("AVI Draw time = %ld\n", lTotalAVIDraw / lCountAVIDraw);
		Debug("Notify time = %ld\n", lTotalNotify / lCountNotify);
		Debug("Dib Draw time = %ld\n", lTotalDibDraw / lCountDibDraw);
	#endif

	#ifdef UNUSED_FOR_INTERIM
	// If an interim timer has been setup
	if (uTimerId)
	{
		// Setup new DC for the video timer proc
		// NOTE! Make sure this DC gets released!!!!!!!
		pinst->hdc = GetDC(hVideoWnd);
	}

	// Set video end flag so the timer can process (if there is one)
	bVideoEnd = TRUE;
	#endif

	return ICERR_OK;
}
//***************************************************************************
static long AVIDraw(PINSTINFO pi, ICDRAW FAR *lpicd, LONG cbicd)
//***************************************************************************
{
	UINT  wFlags;

	#ifdef PROFILE
		lStartAVIDraw = timeGetTime();
	#endif

	#ifdef UNUSED_FOR_INTERIM
	// Look and see if there is an interim video happening (between video shots)
	if (nInterimFrames)
	{
		VideoEndInterim();
		bVideoEnd = FALSE;
		return ICERR_OK;
	}
	#endif

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

	#ifdef PROFILE
		lStartNotify = timeGetTime();
	#endif 

	if ((LPTR)lpicd->lpData != NULL )
		VideoNotifyDraw((LPBITMAPINFOHEADER)lpicd->lpFormat, (LPTR)lpicd->lpData);

	#ifdef PROFILE
		lTotalNotify += (timeGetTime() - lStartNotify);
		++lCountNotify;
		lStartDibDraw = timeGetTime();
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

	#ifdef PROFILE
		lTotalDibDraw = (timeGetTime() - lStartDibDraw);
		++lCountDibDraw;
	#endif

	// After drawing the frame then send any timer message
	for( int i=0; i<nNumVideoTimers; i++)
	{
		PostMessage(Timers[i].hNotifyWnd, WM_TIMER, Timers[i].wTimerId, NULL);
	}

	#ifdef PROFILE
		lTotalAVIDraw += (timeGetTime() - lStartAVIDraw);
		++lCountAVIDraw;
	#endif

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
long CALLBACK MCIDrawProc(DWORD id, HDRVR hDriver, UINT message, LPARAM lParam1, LPARAM lParam2)
//***************************************************************************
{
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
	case ICM_DRAW_END: // Cleanup after a draw operation
		return AVIDrawEnd();

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
	{
		PINSTINFO pi;
		if ( !(pi = (PINSTINFO)id) )	return ICERR_UNSUPPORTED;
		if (!pi->hdc || !pi->hdd)		return ICERR_UNSUPPORTED;
		return (UINT)DrawDibGetPalette(pi->hdd);
	}

	case ICM_DRAW_REALIZE:
	{
		PINSTINFO pi;
		if ( !(pi = (PINSTINFO)id) )	return ICERR_UNSUPPORTED;
		pi->hdc = (HDC)lParam1;
		if (!pi->hdc || !pi->hdd)		return ICERR_UNSUPPORTED;
		//Debug("Realize!\n");
		return DrawDibRealize(pi->hdd, pi->hdc, (BOOL) lParam2);
	}

	case DRV_CLOSE: // Close the rendering driver
		return AVIDrawClose((PINSTINFO)id);

	case ICM_DRAW_SUGGESTFORMAT: // Suggest the output format
        return AVIDrawSuggestFormat((PINSTINFO)id, (ICDRAWSUGGEST FAR *)lParam1, lParam2);

	case ICM_DRAW_QUERY: // Determine if the input can be processed
        return AVIDrawQuery((PINSTINFO)id, (LPBITMAPINFOHEADER)lParam1);

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


