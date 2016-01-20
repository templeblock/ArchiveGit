//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "tablet.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "svrview.h"


/************************************************************************/
/* Constants								*/
/************************************************************************/
#define FADE_LEN_MULT 5
#define THREAD_BASE	__declspec(thread)
//#define THREAD_BASE
  
typedef struct _brushrect
	{
	WORD	xTile;
	WORD	yTile;
	RECT	rTile;
	RECT	rEdit;
	BOOL	fEmpty;
	} BRUSHRECT, *PBRUSHRECT;

/************************************************************************/
/* Local Functions							*/
/************************************************************************/
LOCAL BOOL SaveStrokeInfo(int dx, int dy, int sx, int sy, int Pressure);
static void NewDoDDABrushStroke(int sx, int sy, int dx, int dy);
static void DoBrushStroke(int sx, int sy, int dx, int dy);
static void SetupOverlap();
static void NewProcessRectList(int x, int y);
static void BltRect(PRECT lpRect, HPTR lpBrushMask);
static void NewBltRect(PRECT lpRect);
static void BltToFrame( int x, int y, int dh, int dv, HPTR lpData, PRECT lpRect, BOOL fUpdateDisplay, HPTR hpMsk);
static void BltToDisplay(LPRECT lpRect);
static void SetupDataProcs(FRMDATATYPE Type);
static void SetupBrushProcs(LPMGXBRUSH lpBrush, BOOL fBegin);
static BOOL SetupBrush(LPMGXBRUSH lpBrush);
static void SetupBrushOpacity(BOOL fForceSetup);
static void SetBrushOpacity(HPTR lpSrc, HPTR lpDst, WORD Opacity, DWORD dwCount);
static void SetBrushOpacitySeg(LPTR lpSrc, LPTR lpDst, WORD Opacity, int iCount);
static void SetupBrushTimer(LPMGXBRUSH lpBrush, BOOL fBegin);
static BOOL AllocBrushBuffers(LPMGXBRUSH lpBrush);
static void FreeUpBrushBuffers( void );
static LPMGXBRUSH AllocMgxBrush(void);
static void FreeUpMgxBrush(LPMGXBRUSH lpBrush);
static void SetupFade(LPMGXBRUSH lpBrush);
static void ResetBrush(LPMGXBRUSH lpBrush, BOOL fNoBrushDisplay);
static void SetupMaskState( void );
static void SetupPressurePen( void );
static BOOL SetupFrames(LPMGXBRUSH lpBrush, BOOL fBegin);
static int GetPPString(LPSTR lpSection, int idEntry, int idDefault, LPSTR lpValue, int nValueLen, LPSTR lpFileName);
static BOOL WritePPString(LPSTR lpSection, int idEntry, int idValue, LPSTR lpValue, LPSTR lpFileName);
static BOOL WritePPInt(LPSTR lpSection, int idEntry, int iValue, LPSTR lpFileName);
static BOOL Equal(LPSTR lpString1, int idString2);
static void GetMouseMoves();
static void GetPacketsFromQueue();
static void ProcessBrushStrokes();
static BOOL SetupOSFrame(BOOL fBegin, LPRECT lpStrokeRect, LPRECT lpMaxRect);
static BOOL AllocOSBrushBuffers(LPMGXBRUSH lpBrush);
static void FreeUpOSBrushBuffers();
static void SetupOSBrushProcs(LPMGXBRUSH lpBrush);
static void DoDDAOSBrushStroke(int dx, int dy);
static void DoOSBrushStroke(int dx, int dy);
static void ProcessOSBrushStroke(int x, int y);
static void	OSMergeMaskLine(LPTR lpDst, int iCount, LPTR lpSrc, int iMaxValue);
static void	OSMergeMaskLineHuge(LPTR lpDst, int iCount, HPTR hpSrc, int iMaxValue);
static void OSMergeMask(LPFRAME lpMskFrame, HPTR lpSrc, LPRECT lpSrcRect,
					 int SrcWidth, LPRECT lpSectRect, int iMaxValue);
HPTR SolidColorSource(int x, int y, int dx, int dy, int State);
HPTR CopyImageSource(int x, int y, int dx, int dy, int State);
HPTR SharpenImageSource(int x, int y, int dx, int dy, int State);
HPTR SmoothImageSource(int x, int y, int dx, int dy, int State);
HPTR LightenImageSource(int x, int y, int dx, int dy, int State);
HPTR DarkenImageSource(int x, int y, int dx, int dy, int State);
HPTR SmearImageSource(int x, int y, int dx, int dy, int State);
static void CombineData32(LPCMYK, LPRECT, int, LPCMYK, LPRECT, int, LPRECT, WORD);
static void CombineData24(LPRGB, LPRECT, int, LPRGB, LPRECT, int, LPRECT, WORD);
static void CombineData8(HPTR, LPRECT, int, HPTR, LPRECT, int, LPRECT, WORD);
static void Colorize32(LPCMYK, LPCMYK, int, LPCMYK, WORD);
static void Colorize24(LPRGB, LPRGB, int, LPRGB, WORD);
static void Colorize8(LPTR, LPTR, int, LPTR, WORD);
LOCAL void Colorize32Huge(HPCMYK hpDst, HPCMYK hpSrc, DWORD dwCount, HPCMYK hpColor, WORD sb);
LOCAL void Colorize24Huge(HPRGB hpDst, HPRGB hpSrc, DWORD dwCount, HPRGB hpColor, WORD sb);
LOCAL void Colorize8Huge(HPTR hpDst, HPTR hpSrc, DWORD dwCount, HPTR hpColor, WORD sb);
#ifdef C_CODE
static void MapLut(LPTR lpSrc, LPTR lpDst, LPTR lpLut, unsigned int wCount);
static void MapLut32(LPTR lpSrc, LPTR lpDst, LPTR lpLut, int iCount);
#else
extern "C" {
extern void MapLut(LPTR lpSrc, LPTR lpDst, LPTR lpLut, unsigned int wCount);
extern void MapLut32(LPTR lpSrc, LPTR lpDst, LPTR lpLut, int iCount);
}
#endif
static void FillInBuffer(HPTR lpBuf, int w, int h, LPRECT lpRect);
static void ProcessHugeData8(HPTR lpDst, HPTR lpSrc, HPTR lpMsk, DWORD dwCount);
static void ProcessHugeData24(HPTR lpDst, HPTR lpSrc, HPTR lpMsk, DWORD dwCount);
static void ProcessHugeData32(HPTR lpDst, HPTR lpSrc, HPTR lpMsk, DWORD dwCount);
LOCAL void SetPixelHuge(LPFRAME lpFrame, long lPixel, HPTR hpDst, int dx);
LOCAL void MgxBrushStrokeEx(int sx, int sy, int dx, int dy, int curs_x, int curs_y, WORD Pressure );

#define GetPixelHuge(lpLine, depth, colorPtr)                               \
{                                                                           \
if (depth == 4)         { CopyCMYKHuge(lpLine, colorPtr); }                 \
else if (depth == 3)    { *(colorPtr) = 0; CopyRGBHuge(lpLine, colorPtr); } \
else if (depth == 1)    { *((HPTR)(colorPtr)) = *((HPTR)(lpLine)); }        \
     else               { *((HPWORD)(colorPtr)) = *((HPWORD)(lpLine)); }    \
}

/************************************************************************/
/* static variables							*/
/************************************************************************/
	
typedef struct _brushdata
	{
	int			Dummy;
	PIXMAP_TYPE pmDstType;
	PIXMAP_TYPE pmSrcType;
	PIXMAP_TYPE pmMskType;
	FRMTYPEINFO BltFrameTypeInfo;
	} BRUSHDATA;

static THREAD_BASE  BRUSHDATA   BrushData;
static THREAD_BASE	HWND		hWnd;		        // the window containing the image
static THREAD_BASE	CServerView *pView;
static THREAD_BASE	LPIMAGE	lpImage;	        // the image
static THREAD_BASE	LPFRAME  lpMskFrame;
static THREAD_BASE	PIXMAP	dstPixmap;
static THREAD_BASE	LPPIXMAP	lpDstPixmap;
static THREAD_BASE	LPPIXMAP	lpOrigDstPixmap;
static THREAD_BASE	LPPIXMAP	lpSrcPixmap;
static THREAD_BASE	LPPIXMAP	lpMskPixmap;
static THREAD_BASE	PIXMAP	srcPixmap;
static THREAD_BASE	PIXMAP	mskPixmap;
static THREAD_BASE	PIXMAP	surfacePixmap;
//static LPPIXMAP lpSurfacePixmap;
static THREAD_BASE	LPMASK	lpMask;
static THREAD_BASE	int		iBltFrameDepth;
static THREAD_BASE	int		iBltFrameXsize;
static THREAD_BASE	int		iBltFrameYsize;
static THREAD_BASE	int		iBltFrameRes;
static THREAD_BASE	int		iSrcFrameRes;
static THREAD_BASE	int		xDstOffset;
static THREAD_BASE	int		yDstOffset;
static THREAD_BASE	int		xSrcOffset;
static THREAD_BASE	int		ySrcOffset;
static THREAD_BASE	WORD		Spacing;
static THREAD_BASE	BOOL		fMaskEditing;
static THREAD_BASE	BOOL		fNotAnImage;
static THREAD_BASE	BOOL		fUpdateMarquee;
static THREAD_BASE	BOOL		fUsePolling;	    
static THREAD_BASE	BOOL		UsingMask;	        // Using any mask?
static THREAD_BASE	BOOL		UsingShield;		// Using the Color Shield
static THREAD_BASE	BOOL		fProcessing;
static THREAD_BASE	BOOL		fUsePressure;	    // Pressure sensitive pen present
static THREAD_BASE	BOOL 	fPressureSize; 	    // Adjust brush size with pressure
static THREAD_BASE	BOOL		fPressureOpacity;   // Adjust opacity with pressure
static THREAD_BASE	BOOL		fFirstStroke;	    // First stroke?
static THREAD_BASE	BOOL		fCheckLastStroke;   // Check for overlap of last stroke
static THREAD_BASE	BOOL		fHorzStrips;
static THREAD_BASE	BOOL		fRightBottom;	    // Direction of brush stroke
static THREAD_BASE	BOOL		fRight;
static THREAD_BASE	BOOL		fBottom;
static THREAD_BASE	BOOL		fDone;		        // Done processing rects?
static THREAD_BASE	BOOL		fStrokeProcessed;   // has a stroke been processed?
static THREAD_BASE	BOOL		fDoBuffering;
static THREAD_BASE	BOOL		fCreatedUndo;
static THREAD_BASE	BOOL		fNoOverlap;
static THREAD_BASE	BOOL		fSmoothness;
static THREAD_BASE	BOOL		fUpdateDisplay;
static THREAD_BASE	LFIXED	FadeOut;	        // Fade In/Out value
static THREAD_BASE	LFIXED	FadeBrushSize;	    // Current brush size for Fade
static THREAD_BASE	LFIXED	FadeOpacity;	    // Fade value for opacity
static THREAD_BASE	LFIXED	FadeOpacityValue;   // Current opacity for Fade
static THREAD_BASE	int		BrushFull;	        // Brush Size
static THREAD_BASE	int		BrushHalf;	        // Half Brush Size
static THREAD_BASE	int		bh;		            // brush buffer size horizontally
static THREAD_BASE	int		bv;		            // brush buffer size vertically
static THREAD_BASE	DWORD 	bcount;		        // brush buffer size in bytes
static THREAD_BASE	int		brush_x;	        // Brush x-position
static THREAD_BASE	int		brush_y;	        // Brush y-position
static THREAD_BASE	int		sx_delta;	        // x-delta between source and dest
static THREAD_BASE	int		sy_delta;	        // y-delta between source and dest
static THREAD_BASE	int		start_dx;	        // Destination starting x
static THREAD_BASE	int		start_dy;	        // Destination starting y
static THREAD_BASE	int		end_dx;		        // Destination ending x
static THREAD_BASE	int		end_dy;		        // Destination ending y
static THREAD_BASE	WORD		BrushSize;	        // Original Brush Size
static THREAD_BASE	WORD		Opacity;	        // Original Brush Opacity
static THREAD_BASE	WORD		PenPressure;	    // pressure pen pressure
static THREAD_BASE	WORD		LastOpacity;	    // Original Brush Opacity
static THREAD_BASE	WORD		MaxOpacity;
static THREAD_BASE	WORD		StylePressure;	    // Original Brush Pressure
static THREAD_BASE	int 		State;		        // Current State
static THREAD_BASE	HPTR		lpColor;	        // Source color buffer pointer
static THREAD_BASE	HPTR		lpData;		        // Source data buffer pointer
static THREAD_BASE	HPTR		lpDstData;		    // used for non-overlap brushes
static THREAD_BASE	HPTR		lpMsk;		        // Mask buffer pointer
static THREAD_BASE	HPTR		lpBM;		        // BrushMask pointer
static THREAD_BASE	HPTR		lpMsk1;		        // Primary buffer for building up mask
static THREAD_BASE	LPINT16	lpErr;		        // Error buffer for scatter style

static THREAD_BASE	void     ( *lpHugeShieldProc )( HPTR, HPTR, DWORD, FRMTYPEINFO );	// Proc to load shield data
static THREAD_BASE	void	    ( FAR *lpProcessProc )( LPTR, LPTR, LPTR, int );	// Proc to process image data
static THREAD_BASE	void	    ( FAR *lpHugeProcessProc )( HPTR, HPTR, HPTR, DWORD );	// Proc to process image data
//static void ( *lpSurfaceProc )( HPTR, HPTR, int ); // Surface texture proc
static THREAD_BASE	BOOL     ( *lpGetSourceData )( LPPIXMAP, PIXMAP_TYPE, int, int, int, int,
                                      HPTR, LPRECT, BOOL, BOOL );  // Proc to get source data
static THREAD_BASE	HPTR     ( *lpCreateBrushSource )( int, int, int, int, int );  // Proc to create source data
static THREAD_BASE	LPBRUSHUPDATEPROC lpUpdateProc; // Proc for update display
static THREAD_BASE	LPSRCPOSPROC lpSrcPosProc; // Proc for update display
static THREAD_BASE	void     ( *StrokeProc )( int, int, int, int );	// Proc to handle stroking
static THREAD_BASE	void     ( *OSStrokeProc )( int, int );	      // Proc to handle stroking
static THREAD_BASE	void     ( *StylizeProc )( HPTR, HPTR, LPINT16, int, int, int );	// Proc to handle brush style
static THREAD_BASE	RECT 	rMax;		// Max area stroke can affect
static THREAD_BASE	RECT		rOffScreen;
static THREAD_BASE	RECT		rUpdate;
static THREAD_BASE	RECT		rEdited;
static THREAD_BASE	LPMGXBRUSH lpBrush;	        // The brush description
static THREAD_BASE	WORD		idTimer;	        // timer indentifier
static THREAD_BASE	DWORD	dwLastNoMoveTime;   // time last stroke finished
static THREAD_BASE	DWORD	dwMoveTime;
static THREAD_BASE	DWORD	dwLastTime;
static THREAD_BASE	DWORD	dwBrushTimer;
static THREAD_BASE	DWORD	dwSaveStrokeInfo;
static THREAD_BASE	HPSTROKEINFO hpSaveBuffer;
static THREAD_BASE	DWORD	dwNumStrokes;
static THREAD_BASE	DWORD 	dwMaxStrokes;
static THREAD_BASE	DWORD	dwStrokeInfoSize;
static THREAD_BASE	BOOL	    SaveUsePressure;
static THREAD_BASE	BRUSHRECT	bRects[4];
static THREAD_BASE	int lr[4] = {0, 1, 3, 2};
static THREAD_BASE	int ur[4] = {3, 0, 2, 1};
static THREAD_BASE	int ul[4] = {2, 1, 3, 0};
static THREAD_BASE	int ll[4] = {1, 0, 2, 3};

// new 
//static PINT pPtTable;
static THREAD_BASE	int * pPtTable;

static THREAD_BASE	LPBRUSHPT    lpMoveBuf;
static THREAD_BASE	LPBRUSHPT    lpMoveEnd;
static THREAD_BASE	LPBRUSHPT    lpMoveIn;
static THREAD_BASE	LPBRUSHPT    lpMoveOut;
static THREAD_BASE	BOOL fQuit;
static THREAD_BASE	int last_x_processed;
static THREAD_BASE	int last_y_processed;
static THREAD_BASE	MAP Map;

/************************************************************************/
BOOL SetupBrushBuffers(BOOL fInit)
/************************************************************************/
{
if (fInit)
	{
	if (!SetupBrushMaskBuffers(fInit))
		return(FALSE);
	lpMoveBuf = (LPBRUSHPT)Alloc(4096L * sizeof(BRUSHPT));
	if (!lpMoveBuf)
		{
		SetupBrushMaskBuffers(NO);
		return(FALSE);
		}
	lpMoveEnd = lpMoveBuf + 4096L;
	}
else
	{
	SetupBrushMaskBuffers(fInit);
	if (lpMoveBuf)
		{
		FreeUp((LPTR)lpMoveBuf);
		lpMoveBuf = NULL;
		}
	}
return( TRUE );
}

/************************************************************************/
void MgxBrushSetDefaults(LPMGXBRUSHPARMS lpParms,
					HWND hWndIn, LPPIXMAP lpPixmapIn, int xOffset, int yOffset,
					HMGXBRUSH hBrush)
/************************************************************************/
{
LPIMAGE lpImage = NULL;

if (hWndIn)
	lpImage = GetImagePtr(hWndIn);
MgxBrushSetAdvanced(lpParms, hWndIn, lpPixmapIn, xOffset, yOffset, hBrush,
					YES, lpImage, NULL, NULL);
}

/************************************************************************/
void MgxBrushSetAdvanced(LPMGXBRUSHPARMS lpParms, 
					HWND hWndIn, LPPIXMAP lpPixmapIn, int xOffset, int yOffset,
					HMGXBRUSH hBrush,
					BOOL fUsePolling,
					LPIMAGE lpImage,
					LPBRUSHUPDATEPROC lpUpdateProcIn,
					LPSRCPOSPROC lpSrcPosProc)
/************************************************************************/
{
    lpParms->hWnd = hWndIn;
    lpParms->lpPixmap = lpPixmapIn;
    lpParms->xOffset = xOffset;
    lpParms->yOffset = yOffset;
    lpParms->hBrush = hBrush;
    lpParms->fUsePolling = fUsePolling;
    lpParms->lpImage = lpImage;
    lpParms->fNotAnImage = lpImage == NULL;
    lpParms->lpUpdateProc = lpUpdateProcIn;
    lpParms->lpSrcPosProc = lpSrcPosProc;
    lpParms->dwSaveStrokeInfo = SSI_NONE;
}

/************************************************************************/
BOOL MgxBrushBegin( HWND hWndIn, LPPIXMAP lpPixmapIn,
				    int xOffset, int yOffset,
					HMGXBRUSH hBrush )
/************************************************************************/
{
    return(MgxBrushBeginEx( hWndIn, lpPixmapIn, xOffset, yOffset, hBrush,
	        			    YES, NO, NULL, NULL ));
}

/************************************************************************/
BOOL MgxBrushBeginEx( HWND hWndIn, LPPIXMAP lpPixmapIn,
					  int xOffset, int yOffset,
					  HMGXBRUSH hBrush, BOOL fUsePollingIn,
					  BOOL fNotAnImageSetting,
					  LPBRUSHUPDATEPROC lpUpdateProcIn,
					  LPSRCPOSPROC lpSrcPosProcIn )
/************************************************************************/
{
MGXBRUSHPARMS parms;
LPIMAGE lpImage = NULL;

if (hWndIn && !fNotAnImageSetting)
	lpImage = GetImagePtr(hWndIn);

MgxBrushSetAdvanced(&parms, hWndIn, lpPixmapIn,
					xOffset, yOffset,
					hBrush, fUsePollingIn,
					lpImage,
					lpUpdateProcIn,
					lpSrcPosProcIn);
return(MgxBrushBeginParms(&parms));
}

/************************************************************************/
BOOL MgxBrushBeginParms( LPMGXBRUSHPARMS lpParms )
/************************************************************************/
{
    // Call End function to make sure things get reset
    MgxBrushEnd(NO);

    // Convert brush handle to brush pointer for our use
    lpBrush = (LPMGXBRUSH)lpParms->hBrush;

    // Save passed in values for later use
    hWnd = lpParms->hWnd;
	if (hWnd)
		pView = (CServerView*)CWnd::FromHandle(hWnd);
	else
		pView = NULL;
    lpOrigDstPixmap = lpParms->lpPixmap;
    fUsePolling = lpParms->fUsePolling;
    xDstOffset = xSrcOffset = lpParms->xOffset;
    yDstOffset = ySrcOffset = lpParms->yOffset;
    lpUpdateProc = lpParms->lpUpdateProc;
    lpSrcPosProc = lpParms->lpSrcPosProc;
    lpImage = lpParms->lpImage;
    fNotAnImage = lpParms->fNotAnImage;
    dwSaveStrokeInfo = lpParms->dwSaveStrokeInfo;
    fUpdateDisplay = fUsePolling && !fNotAnImage && hWnd;

    if (( !lpBrush->Smoothness ))
  	    fUsePolling = NO;

    fDoBuffering = NO;

    // setup overlap
    SetupOverlap();

    // Make sure there are no outstanding WM_PAINT's to avoid strange display
    if (hWnd)
    {
	    if (fNotAnImage)
		    UpdateWindow(hWnd);
	    else
		    AstralUpdateWindow(hWnd);
    }

    if (fUpdateDisplay)
    {
 	    if (!lpImage->PaintSetup(GetDisplayPtr(hWnd), NO))
	    {
		    lpBrush = NULL;
		    return(FALSE);
	    }
	    AstralSetRectEmpty(&rUpdate);
    }

    // Set frame mode to free oldest lines first instead of newest
    // Assumption is that your typically brushing in the same area
    if (!SetupFrames(lpBrush, YES))
    {
	    lpBrush = NULL;
	    return(FALSE);
    }

    // Setup Fade for brush size and brush opacity
    SetupFade(lpBrush);

    // Allocate all memory needed for brushing
    if (!AllocBrushBuffers(lpBrush))
    {
	    lpBrush = NULL;
	    return(FALSE);
    }

    // Initialize pressure sensitive pen settings
    SetupPressurePen();

    // Setup the brush mask
    if (!SetupBrush(lpBrush))
    {
	    FreeUpBrushBuffers();
	    SetupFrames(lpBrush, NO);
	    lpBrush = NULL;
	    return(FALSE);
    }

    // Setup the Depth and Brush dependent procs
    SetupDataProcs(BrushData.BltFrameTypeInfo.DataType);
    SetupBrushProcs(lpBrush, YES);

    FRMTYPEINFO TypeInfo;
	 if (lpImage)
	    ImgGetTypeInfo(lpImage, &TypeInfo);
	 else
	    FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);
    // Load up the source color
    if (lpBrush->SourceColor == SC_ACTIVE_COLOR)
	    GetActiveColorFromTypeInfo(TypeInfo, &lpBrush->ColorStruct);
    else if (lpBrush->SourceColor == SC_ALTERNATE_COLOR)
	    GetAlternateColorFromTypeInfo(TypeInfo, &lpBrush->ColorStruct);
    LoadColor(PixmapFrame(lpDstPixmap, PMT_EDIT), lpColor, bcount,
			&lpBrush->ColorStruct);


    // Setup mask state, geometrical mask and color shield
    SetupMaskState();

    // Setup brush timer
    SetupBrushTimer(lpBrush, YES);

    // Indicate the first stroke is coming
    fFirstStroke = YES;

    AstralSetRectEmpty(&rEdited);
    SetRect(&rMax, 0, 0, iBltFrameXsize-1, iBltFrameYsize-1);

    if( fUsePolling )
    {
	    ProcessBrushStrokes();
	    MgxBrushEnd( NO );
    }
    return(TRUE);
}

/************************************************************************/
void MgxBrushStroke( int sx, int sy, int dx, int dy,
         			 int curs_x, int curs_y )
/************************************************************************/
{
    LPTABLETDATA lpPacket;
    POINT pt;

    // if tablet active 
    if( fUsePressure )
    {
        lpPacket = GetNextPacket( YES );
        PenPressure = lpPacket->Pressure;   

        if( fUsePolling )                   
        {
            pt.x = lpPacket->x;
            pt.y = lpPacket->y;

            curs_x = pt.x;
            curs_y = pt.y;

           	Display2File( hWnd, (LPINT)&pt.x, (LPINT)&pt.y );
            sx = dx = pt.x;
            sy = dy = pt.y;
        }
    }
    MgxBrushStrokeEx( sx, sy, dx, dy, curs_x, curs_y, PenPressure );
}


/************************************************************************/
void MgxBrushStrokeEx( int sx, int sy, int dx, int dy,
			           int curs_x, int curs_y, WORD StylusPressure )
/************************************************************************/
{
    int i;

    if (!lpBrush)
	    return;

    if( fUpdateDisplay && !fUsePolling )
    {
	    lpImage->PaintSetup( GetDisplayPtr(hWnd), NO );

   	    brush_x = curs_x;
        brush_y = curs_y;

	    DisplayBrush( 0, 0, 0, OFF );
	}

    if( lpSrcPosProc ) 
	    ( *lpSrcPosProc )( hWnd, dx, dy, lpBrush->Size, &sx, &sy );

    if( dwSaveStrokeInfo )
	    SaveStrokeInfo( dx, dy, sx, sy, StylusPressure );

    sx -= xSrcOffset;
    sy -= ySrcOffset;
    dx -= xDstOffset;
    dy -= yDstOffset;

    ( *StrokeProc )( sx, sy, dx, dy );

    if( !fUsePolling )
	{
	    for( i = 0; i < 4; ++i )
	    {
		    if( bRects[i].fEmpty )
			    continue;
		    NewBltRect( &bRects[i].rEdit );
		    bRects[i].fEmpty = YES;
	    }
	    if( fUpdateDisplay )
		    DisplayBrush( hWnd, brush_x, brush_y, ON );
    }
}

/************************************************************************/
BOOL MgxBrushActive()
/************************************************************************/
{
    return(lpBrush != NULL);
}

/************************************************************************/
void MgxGetEditRect(LPRECT lpEditRect)
/************************************************************************/
{
    *lpEditRect = rEdited;
	OffsetRect(lpEditRect, xDstOffset, yDstOffset);
}

/************************************************************************/
void MgxBrushEnd(BOOL fNoBrushDisplay)
/************************************************************************/
{
    int i;

    if( !lpBrush )
	    return;

    for( i = 0; i < 4; ++i )
    {
	    if( bRects[i].fEmpty )
		    continue;
	    NewBltRect( &bRects[i].rEdit );
	    bRects[i].fEmpty = YES;
    }
    ResetBrush( lpBrush, fNoBrushDisplay );
    SetupBrushProcs( lpBrush, NO );
    SetupFrames( lpBrush, NO );
    SetupBrushTimer( lpBrush, NO );
    FreeUpBrushBuffers();
    lpBrush = NULL;
    if( fUpdateDisplay && !AstralIsRectEmpty( &rUpdate ))
	    UpdateImageEx( GetImagePtr( hWnd ), &rUpdate, YES, NO, YES );
}

/************************************************************************/
static void ProcessBrushStrokes()
// Called when fUsePolling is TRUE
/************************************************************************/
{
    int fx, fy, i;
    DWORD pos, dwNewTime;
    POINT pt;
    LPTABLETDATA lpPacket;

    fDoBuffering = YES;

    if( lpBrush->Smoothness )
    {
	    fSmoothness = YES;
	    dwMoveTime = ((( 100L-( long )lpBrush->Smoothness ) * 500 ) + 50 ) / 100;
    }
    else
	    fSmoothness = NO;

    dwLastNoMoveTime = 0;
    dwLastTime = GetTickCount();

    if( fUsePressure )      // get position and pressure from packet
    {
        lpPacket = GetNextPacket( YES );
        PenPressure = lpPacket->Pressure;
        pt.x = lpPacket->x;
        pt.y = lpPacket->y;
    }
    else
    {
        pos = GetMessagePos();
        pt.x = LOWORD(pos);
        pt.y = HIWORD(pos);
    }
    ScreenToClient( hWnd, &pt );

    fx = brush_x = pt.x;
    fy = brush_y = pt.y;

    if( !fNotAnImage )
	    Display2File( hWnd, &fx, &fy );

	ConstrainXY(&fx, &fy, TRUE, WM_LBUTTONDOWN, CONSTRAINXY);

    fProcessing = YES;
    MgxBrushStrokeEx( fx, fy, fx, fy, brush_x, brush_y, PenPressure );
    fProcessing = NO;

    fQuit = FALSE;

    while( !fQuit )
    {
        if( fUsePressure )    
            GetPacketsFromQueue();
        else
            GetMouseMoves();

	    while( lpMoveOut < lpMoveIn )
	    {
		    if( fQuit ) // make sure the mouse continues to move
            {
                if( fUsePressure  )    
                    GetPacketsFromQueue();
                else
                    GetMouseMoves();
            }
		    fx = lpMoveOut->x;
		    fy = lpMoveOut->y;
		    PenPressure = lpMoveOut->Pressure;
		    ++lpMoveOut;

			ConstrainXY(&fx, &fy, TRUE, 0/*not WM_LBUTTONDOWN*/, FALSE/*nu in this case*/);
			
		    fProcessing = YES;
		    MgxBrushStrokeEx( fx, fy, fx, fy, brush_x, brush_y, PenPressure );
		    fProcessing = NO;

		    if( !fQuit )
			    break;
	    }
	    // if we have caught up to the user:
	    // 1. reset buffer pointers
	    // 2. output any pending rectangles
	    if( lpMoveOut >= lpMoveIn )
	    {
		    if( lpMoveOut >= lpMoveIn )
		    {
			    lpMoveIn = lpMoveBuf;
			    lpMoveOut = lpMoveBuf;
		    }

		    fProcessing = YES;
		    for( i = 0; i < 4; ++i )
		    {
			    if( bRects[i].fEmpty )
				    continue;

			    NewBltRect( &bRects[i].rEdit );
			    bRects[i].fEmpty = YES;
		    }
		    fProcessing = NO;

		    if( !fQuit )
		    {
                if( fUsePressure )    
                    GetPacketsFromQueue();
                else
                    GetMouseMoves();

			    if( dwBrushTimer && ( lpMoveOut >= lpMoveIn ))
			    {
				    dwNewTime = GetTickCount();
				    if(( dwNewTime - dwLastNoMoveTime ) >= dwBrushTimer )
				    {
					    if( fUsePressure )
                        {
                            lpPacket = GetNextPacket( YES );
                            PenPressure = lpPacket->Pressure;
                            pt.x = lpPacket->x;
                            pt.y = lpPacket->y;
                        }
                        else
                        {
					        GetCursorPos( &pt );
                        }
					    ScreenToClient(hWnd, &pt );
					    fx = pt.x;
					    fy = pt.y;

					    if( !fNotAnImage )
						    Display2File( hWnd, &fx, &fy );

						ConstrainXY(&fx, &fy, TRUE, 0/*not WM_LBUTTONDOWN*/, FALSE/*nu in this case*/);
						
					    fFirstStroke = YES;
					    fProcessing = YES;
					    MgxBrushStrokeEx(fx, fy, fx, fy, brush_x, brush_y, PenPressure );
					    fProcessing = NO;
					    dwLastNoMoveTime = dwNewTime;
				    }
			    }
		    }
	    }
    }
    fDoBuffering = NO;
}

/************************************************************************/
static void GetMouseMoves()
// called when NOT polling
/************************************************************************/
{
    DWORD dwNewTime;
    POINT pt, aspt;
    RECT rUpdate;
    LPTABLETDATA lpPacket;

    if (!fDoBuffering)
	    return;

    if( !LBUTTON )
	    fQuit = TRUE;

    if (!fSmoothness)
    {
	    if (fProcessing)
		    return;
    }
    else
    if (!fQuit && dwMoveTime)
    {
	    dwNewTime = GetTickCount();
	    if ((dwNewTime - dwLastTime) < dwMoveTime)
		    return;
	    dwLastTime = dwNewTime;
    }
    GetCursorPos( &pt );
    ScreenToClient( hWnd, &pt );

    if (pt.x != brush_x || pt.y != brush_y)
    {
	    brush_x = pt.x;
	    brush_y = pt.y;

	    if (!fQuit && lpMoveIn < lpMoveEnd)
	    {
		    aspt = pt;
		    if (!fNotAnImage)
			    Display2File(hWnd, ( LPINT )&pt.x, ( LPINT )&pt.y);

		    if( fUsePressure )
            {
                lpPacket = GetNextPacket( YES );
                lpMoveIn->Pressure = lpPacket->Pressure;
            }
   		    lpMoveIn->x = pt.x;
    	    lpMoveIn->y = pt.y;
		    ++lpMoveIn;

		    if (!fNotAnImage &&
			    !Control.DisableAutoScroll &&
				pView &&
			    pView->AutoScroll(aspt.x, aspt.y))
		    {
			    lpImage->PaintSetup(GetDisplayPtr(hWnd), NO);
			    if (!fNotAnImage)
				    File2Display(hWnd, ( LPINT )&pt.x, ( LPINT )&pt.y);

   			    brush_x = pt.x;
    		    brush_y = pt.y;

			    ClientToScreen( hWnd, &pt );
			    SetCursorPos( pt.x, pt.y );
		    }
	    }
	    if (lpUpdateProc)
	  	{
	  	    rUpdate.left = brush_x;
	  	    rUpdate.top = brush_y;
	  	    (*lpUpdateProc)(hWnd, &rUpdate, YES);
	  	}
	    else
	    if (fUpdateDisplay)
		{
		    DisplayBrush(0, 0, 0, OFF);
		    DisplayBrush(hWnd, brush_x, brush_y, ON);
		}
	}
}

/************************************************************************/
static void GetPacketsFromQueue()
/************************************************************************/
{
    UINT First, Last;
    POINT pt, aspt;
    RECT rUpdate;
    LPTABLETDATA lpPacket;
    DWORD dwNewTime;


    if( !fDoBuffering )
	    return;

    if( !IsPenPressed())
	    fQuit = TRUE;

    if( !fSmoothness )
    {
	    if( fProcessing )
		    return;
    }
    else
    if( !fQuit && dwMoveTime )
    {
	    dwNewTime = GetTickCount();
	    if(( dwNewTime - dwLastTime ) < dwMoveTime )
		    return;
	    dwLastTime = dwNewTime;
    }

    lpPacket = GetNextPacket( NO );
    pt.x = lpPacket->x;
    pt.y = lpPacket->y;

    ScreenToClient( hWnd, &pt );
    if( pt.x != brush_x || pt.y != brush_y )
    {
	    brush_x = pt.x;
	    brush_y = pt.y;

    	if( !fQuit && lpMoveIn < lpMoveEnd )
	    {
		    aspt = pt;
		    if( !fNotAnImage )
			    Display2File( hWnd, ( LPINT )&pt.x, ( LPINT )&pt.y );

            lpPacket = GetNextPacket( YES );
            lpMoveIn->Pressure = lpPacket->Pressure;
   		    lpMoveIn->x = pt.x;
    	    lpMoveIn->y = pt.y;
		    ++lpMoveIn;

		    if( !fNotAnImage &&
				!Control.DisableAutoScroll &&
				pView &&
                pView->AutoScroll( aspt.x, aspt.y ))
		    {
			    lpImage->PaintSetup( GetDisplayPtr(hWnd), NO );
			    if( !fNotAnImage )
				    File2Display( hWnd, ( LPINT )&pt.x, ( LPINT )&pt.y );

   			    brush_x = pt.x;
    		    brush_y = pt.y;

			    ClientToScreen( hWnd, &pt );
			    SetCursorPos( pt.x, pt.y );
		    }
		}
	    if( lpUpdateProc )
	  	{
	  	    rUpdate.left = brush_x;
	  	    rUpdate.top  = brush_y;
	  	    ( *lpUpdateProc )( hWnd, &rUpdate, YES );
	  	}
	    else
	    if( fUpdateDisplay )
        {
	        DisplayBrush( 0, 0, 0, OFF );
	        DisplayBrush( hWnd, brush_x, brush_y, ON );
        }
    }
}

/************************************************************************/
LOCAL BOOL SaveStrokeInfo(int dx, int dy, int sx, int sy, int Pressure)
/************************************************************************/
{
HPSTROKEINFO hpTempBuffer;

#define MAX_STROKES 250

if (!hpSaveBuffer)
 	{
	SaveUsePressure = fUsePressure;
	dwNumStrokes = 0;
	dwMaxStrokes = MAX_STROKES;
	if (dwSaveStrokeInfo & SSI_SRC)
		{
		if (fUsePressure)
			dwStrokeInfoSize = sizeof(STROKEINFO4);
		else
			dwStrokeInfoSize = sizeof(STROKEINFO3);
		}
	else
		{
		if (fUsePressure)
			dwStrokeInfoSize = sizeof(STROKEINFO2);
		else
			dwStrokeInfoSize = sizeof(STROKEINFO1);
		}
	hpSaveBuffer = Alloc(dwMaxStrokes*dwStrokeInfoSize);
	}
else
if (dwNumStrokes == dwMaxStrokes)
	{
	dwMaxStrokes += MAX_STROKES;
	hpTempBuffer = AllocExtend((LPTR)hpSaveBuffer, dwMaxStrokes*dwStrokeInfoSize);
	if (!hpTempBuffer)
		{
		FreeUp(hpSaveBuffer);
		hpSaveBuffer = NULL;
		dwNumStrokes = 0;
		Message(IDS_EMEMALLOC);
		return(FALSE);
		}
	else
		hpSaveBuffer = hpTempBuffer;
	}

if (dwSaveStrokeInfo & SSI_SRC)
	{
	if (fUsePressure)
		{
		HPSTROKEINFO4 hpSavePtr = (HPSTROKEINFO4)hpSaveBuffer + dwNumStrokes;
		hpSavePtr->dx = dx;
		hpSavePtr->dy = dy;
		hpSavePtr->sx = sx;
		hpSavePtr->sy = sy;
		hpSavePtr->Pressure = Pressure;
		}
	else
		{
		HPSTROKEINFO3 hpSavePtr = (HPSTROKEINFO3)hpSaveBuffer + dwNumStrokes;
		hpSavePtr->dx = dx;
		hpSavePtr->dy = dy;
		hpSavePtr->sx = sx;
		hpSavePtr->sy = sy;
		}
	}
else
	{
	if (fUsePressure)
		{
		HPSTROKEINFO2 hpSavePtr = (HPSTROKEINFO2)hpSaveBuffer + dwNumStrokes;
		hpSavePtr->dx = dx;
		hpSavePtr->dy = dy;
		hpSavePtr->Pressure = Pressure;
		}
	else
		{
		HPSTROKEINFO1 hpSavePtr = (HPSTROKEINFO1)hpSaveBuffer + dwNumStrokes;
		hpSavePtr->dx = dx, iBltFrameRes;
		hpSavePtr->dy = dy, iBltFrameRes;
		}
	}
++dwNumStrokes;
return(TRUE);
}

/************************************************************************/
HPSTROKEINFO MgxBrushGetStrokeInfo(LPINT nPoints, LPBOOL fSavePressure,
									LPINT lpDstRes, LPINT lpSrcRes)
/************************************************************************/
{
HPSTROKEINFO hpSavePtr;

*nPoints = dwNumStrokes;
*fSavePressure = SaveUsePressure;
*lpDstRes = iBltFrameRes;
*lpSrcRes = iSrcFrameRes;
// do a realloc on the buffer, cause it's most likely way
// bigger than is needed
hpSavePtr = AllocExtend((LPTR)hpSaveBuffer, dwNumStrokes*dwStrokeInfoSize);
if (!hpSavePtr)
	hpSavePtr = hpSaveBuffer;
hpSaveBuffer = NULL;
return(hpSavePtr);
}

/************************************************************************/
LPMGXBRUSH CopyMgxBrush(HMGXBRUSH hBrush)
/************************************************************************/
{
LPMGXBRUSH lpBrush;

if (!hBrush)
	return(NULL);

// Allocate memory for brush
if (!(lpBrush = AllocMgxBrush()))
	{
	Message(IDS_EMEMALLOC);
	return(NULL);
	}
copy((LPTR)hBrush, (LPTR)lpBrush, sizeof(MGXBRUSH));
if (lpBrush->BrushMask)
	{
	lpBrush->BrushMask = Alloc((long)lpBrush->Size*lpBrush->Size);
	if (!lpBrush->BrushMask)
		{
		Message(IDS_EMEMALLOC);
		FreeUpMgxBrush(lpBrush);
		return(NULL);
		}
	hcopy(((LPMGXBRUSH)hBrush)->BrushMask, lpBrush->BrushMask,
				(DWORD)lpBrush->Size*(DWORD)lpBrush->Size);
	}

// Check for pointers which are not allowed to copy
lpBrush->SourceFrame = NULL;
lpBrush->SurfaceFrame = NULL;

return(lpBrush);
}

/************************************************************************/
HMGXBRUSH CreateMgxBrush( LPSTR lpExtName, ITEMID wType, FRMTYPEINFO TypeInfo )
/************************************************************************/
{
FNAME	      szFileName;
STRING	   szSection, szValue;
LPMGXBRUSH  lpBrush;
LPSTR       lpString;

if (TypeInfo.DataType == FDT_LINEART)
	TypeInfo.DataType = FDT_GRAYSCALE;

if ( !LookupExtFileN( lpExtName, szFileName, wType, NO ) )
	{
	Message(IDS_BRUSHNOTFOUND, lpExtName);
	return(NULL);
	}
if (!FileExists(szFileName))
	{
	Message(IDS_BRUSHNOTFOUND, lpExtName);
	return(NULL);
	}

// Allocate memory for brush
if (!(lpBrush = AllocMgxBrush()))
	{
	Message(IDS_EMEMALLOC);
	return(NULL);
	}
clr((LPTR)lpBrush, sizeof(MGXBRUSH));

if (AstralStr(IDS_BS_SECTION, &lpString))
	lstrcpy(szSection, lpString);
else
	lstrcpy(szSection, "");

// Initialize read-only mask
lpBrush->ReadOnlyMask = 0;

// Get the source color
GetPPString(szSection, IDS_BS_SOURCECOLOR, IDS_BS_ACTIVECOLOR, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_ACTIVECOLOR))
	lpBrush->SourceColor = SC_ACTIVE_COLOR;
else
if (Equal(szValue, IDS_BS_ALTERNATECOLOR))
	lpBrush->SourceColor = SC_ALTERNATE_COLOR;
else
	{
	lpBrush->SourceColor = SC_RGB_COLOR;
	AsciiRGB(szValue, &lpBrush->ColorStruct.rgb);
	SetColorInfo(&lpBrush->ColorStruct, &lpBrush->ColorStruct, CS_RGB);
	}

// Get the destination data
//GetPPString(szSection, IDS_BS_DESTDATA, IDS_BS_CURRENTIMAGE, szValue,
//			sizeof(szValue), szFileName);
//if (Equal(szValue, IDS_BS_CURRENTUNDO))
//	lpBrush->DestData = DD_CURRENT_UNDO;
//else
lpBrush->DestData = DD_CURRENT_IMAGE;

// Get the source creation function
GetPPString(szSection, IDS_BS_SOURCEFUNC, IDS_BS_SOLIDCOLOR, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_COPY))
	lpBrush->SourceFunction = SF_COPY_IMAGE;
else
if (Equal(szValue, IDS_BS_SHARPEN))
	lpBrush->SourceFunction = SF_SHARPEN_IMAGE;
else
if (Equal(szValue, IDS_BS_SMOOTH))
	lpBrush->SourceFunction = SF_SMOOTH_IMAGE;
else
if (Equal(szValue, IDS_BS_LIGHTEN))
	lpBrush->SourceFunction = SF_LIGHTEN_IMAGE;
else
if (Equal(szValue, IDS_BS_DARKEN))
	lpBrush->SourceFunction = SF_DARKEN_IMAGE;
else
if (Equal(szValue, IDS_BS_SMEAR))
	lpBrush->SourceFunction = SF_SMEAR_IMAGE;
else
	lpBrush->SourceFunction = SF_SOLID_COLOR;

// Get the source data
GetPPString(szSection, IDS_BS_SOURCEDATA, IDS_BS_CURRENTDEST, szValue,
			sizeof(szValue), szFileName);
lpBrush->SourceFrame = NULL;
if (Equal(szValue, IDS_BS_DEFAULT))
	{
	switch (lpBrush->SourceFunction)
		{
		case SF_SHARPEN_IMAGE:
		case SF_SMOOTH_IMAGE:
		case SF_LIGHTEN_IMAGE:
		case SF_DARKEN_IMAGE:
			lpBrush->SourceData = SD_MODIFIED_DEST;
			break;
		case SF_COPY_IMAGE:
			lpBrush->SourceData = SD_CURRENT_DEST;
			break;
		case SF_SMEAR_IMAGE:
			lpBrush->SourceData = SD_LAST_DEST;
			break;
		case SF_SOLID_COLOR:
		default:
			lpBrush->SourceData = SD_NONE;
			break;
		}
	}
else
if (Equal(szValue, IDS_BS_CURRENTDEST))
	lpBrush->SourceData = SD_CURRENT_DEST;
else
if (Equal(szValue, IDS_BS_CURRENTUNDO))
	lpBrush->SourceData = SD_CURRENT_UNDO;
else
	{
	lstrcpy(lpBrush->szSourceExtName, szValue);
	lpBrush->SourceData = SD_FILE;
	if ( !LookupExtFileN(szValue, lpBrush->szSource, IDN_TEXTURE, NO))
		{
		Message(IDS_BRUSHINVALID, (LPTR)szValue);
		FreeUpMgxBrush(lpBrush);
		return(NULL);
		}
	CReadImage read(lpBrush->szSource);
	if (!(lpBrush->SourceFrame = read.ReadFrame(TypeInfo)))
		{
		Message(IDS_BRUSHINVALID, (LPTR)szValue);
		FreeUpMgxBrush(lpBrush);
		return(NULL);
		}
	lpBrush->ReadOnlyMask |= RO_SOURCE_DATA;
	}

// Get the flip mode
GetPPString(szSection, IDS_BS_FLIPMODE, IDS_BS_FLIPNONE, szValue,
			sizeof(szValue), szFileName);
lpBrush->SourceFlipMode = SFM_NO_FLIP;
lpBrush->SourceFlipHorz = lpBrush->SourceFlipVert = NO;
//lpBrush->ReadOnlyMask |= RO_FLIPMODE; // TMR - never make read only
if (Equal(szValue, IDS_BS_MIRRORHORZ))
	{lpBrush->SourceFlipMode = SFM_MIRROR; lpBrush->SourceFlipHorz = YES;}
else
if (Equal(szValue, IDS_BS_MIRRORVERT))
	{lpBrush->SourceFlipMode = SFM_MIRROR; lpBrush->SourceFlipVert = YES;}
else
if (Equal(szValue, IDS_BS_MIRRORBOTH))
	{
	lpBrush->SourceFlipMode = SFM_MIRROR;
	lpBrush->SourceFlipHorz = YES;
	lpBrush->SourceFlipVert = YES;
	}
else
if (Equal(szValue, IDS_BS_TILE))
	lpBrush->SourceFlipMode = SFM_TILE;
else
if (Equal(szValue, IDS_BS_TILEHORZ))
	{lpBrush->SourceFlipMode = SFM_TILE; lpBrush->SourceFlipHorz = YES;}
else
if (Equal(szValue, IDS_BS_TILEVERT))
	{lpBrush->SourceFlipMode = SFM_TILE; lpBrush->SourceFlipVert = YES;}
else
if (Equal(szValue, IDS_BS_TILEBOTH))
	{
	lpBrush->SourceFlipMode = SFM_TILE;
	lpBrush->SourceFlipHorz = YES;
	lpBrush->SourceFlipVert = YES;
	}
//else
//	lpBrush->ReadOnlyMask &= ~RO_FLIPMODE; // beware the squiggle in Xywrite

// Get the stroke method
GetPPString(szSection, IDS_BS_STROKEMETHOD, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_UNCONNECTED))
	lpBrush->StrokeMethod = SM_UNCONNECTED;
else
	lpBrush->StrokeMethod = SM_CONNECTED;

// Get the brush style
GetPPString(szSection, IDS_BS_STYLE, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_SCATTERED))
	lpBrush->Style = IDC_BRUSHSCATTER;
else
	lpBrush->Style = IDC_BRUSHSOLID;

// Get the brush size
GetPPString(szSection, IDS_BS_SIZE, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);

lpBrush->Size = 50;
#ifdef NOT_IN_UI
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->Size = 50;
else
	{
	lpBrush->ReadOnlyMask |= RO_SIZE;
	lpBrush->Size = Ascii2Int(szValue);
	}
#endif

lpBrush->Size = bound(lpBrush->Size, 1, MAX_BRUSH_SIZE);

// Get the brush shape
GetPPString(szSection, IDS_BS_SHAPE, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
lpBrush->Shape = IDC_BRUSHCIRCLE;

#ifdef NOT_IN_UI
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->Shape = IDC_BRUSHCIRCLE;
else
	{
	lpBrush->ReadOnlyMask |= RO_SHAPE;
	if (Equal(szValue, IDS_BS_CIRCLE))
		lpBrush->Shape = IDC_BRUSHCIRCLE;
	else if (Equal(szValue, IDS_BS_SQUARE))
		lpBrush->Shape = IDC_BRUSHSQUARE;
	else if (Equal(szValue, IDS_BS_VERTICAL))
		lpBrush->Shape = IDC_BRUSHVERTICAL;
	else if (Equal(szValue, IDS_BS_HORIZONTAL))
		lpBrush->Shape = IDC_BRUSHHORIZONTAL;
	else if (Equal(szValue, IDS_BS_SLASH))
		lpBrush->Shape = IDC_BRUSHSLASH;
	else if (Equal(szValue, IDS_BS_BACKSLASH))
		lpBrush->Shape = IDC_BRUSHBACKSLASH;
	else
		{
		lstrcpy(lpBrush->ShapeName, szValue);
		lpBrush->Shape = IDC_BRUSHCUSTOM;
		if (!SetupCustomBrushMask(lpBrush, szValue))
			{
			Message(IDS_BRUSHINVALID, (LPTR)szValue);
			if (lpBrush->SourceFrame)
				FrameClose(lpBrush->SourceFrame);
			FreeUpMgxBrush(lpBrush);
			return(NULL);
			}
		lpBrush->ReadOnlyMask |= (RO_SIZE|RO_SHAPE);
		}
	}
#endif

// Get the pressure value
GetPPString(szSection, IDS_BS_PRESSURE, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->Pressure = 50;
else
	{
	lpBrush->ReadOnlyMask |= RO_PRESSURE;
	lpBrush->Pressure = Ascii2Int(szValue);
	}
lpBrush->Pressure = bound(TOGRAY(lpBrush->Pressure), 0, 255);

// Get the pressure value
GetPPString(szSection, IDS_BS_WEIGHT, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->Weight = 0;
else
	lpBrush->Weight = Ascii2Int(szValue);
lpBrush->Weight = bound(lpBrush->Weight, 0, 255);

// Get the feathered edge type
GetPPString(szSection, IDS_BS_EDGETYPE, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_AIRBRUSH))
	lpBrush->EdgeType = ET_AIRBRUSH;
else
	lpBrush->EdgeType = ET_PAINT;

// Get the feather percentage
GetPPString(szSection, IDS_BS_FEATHER, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->Feather = 100;
else
	{
	lpBrush->ReadOnlyMask |= RO_FEATHER;
	lpBrush->Feather = Ascii2Int(szValue);
	}
lpBrush->Feather = bound(lpBrush->Feather, 0, 99);

// Get the fade in/out percentage
GetPPString(szSection, IDS_BS_FADE, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->FadeSize = 0;
else
	lpBrush->FadeSize = Ascii2Int(szValue);
lpBrush->FadeSize = bound(lpBrush->FadeSize, -99, 99);

// Get the brush transparency
GetPPString(szSection, IDS_BS_TRANSPARENCY, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->Opacity = 255;
else
	{
	lpBrush->ReadOnlyMask |= RO_OPACITY;
	lpBrush->Opacity = 255-TOGRAY(Ascii2Int(szValue));
	}
lpBrush->Opacity = bound(lpBrush->Opacity, 0, 255);

// Get the brush transparency scale factor
GetPPString(szSection, IDS_BS_OPACITYSCALE, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->OpacityScale = 100;
else
	lpBrush->OpacityScale = bound(Ascii2Int(szValue), 0, 100);

// Get the tilt percentage - TBD
GetPPString(szSection, IDS_BS_TILT, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->Tilt = 0;
else
	lpBrush->Tilt = Ascii2Int(szValue);
lpBrush->Tilt = bound(TOGRAY(lpBrush->Tilt), 0, 255);

// Get the timer value
GetPPString(szSection, IDS_BS_TIMER, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->Timer = 0;
else
	lpBrush->Timer = Ascii2Int(szValue);

// Get the maximum flow rate
GetPPString(szSection, IDS_BS_MAXFLOWRATE, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->MaxFlowRate = 0;
else
	lpBrush->MaxFlowRate = FGET(Ascii2Int(szValue), 1);

GetPPString(szSection, IDS_BS_SENSITIVITY, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->Sensitivity = 1;
else
	lpBrush->Sensitivity = bound(Ascii2Int(szValue), 1, INT_MAX);

// Get the maximum flow change allowed
GetPPString(szSection, IDS_BS_MAXFLOWCHANGE, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->MaxFlowChange = 20;
else
	lpBrush->MaxFlowChange = Ascii2Int(szValue);
lpBrush->MaxFlowChange = bound(TOGRAY(lpBrush->MaxFlowChange), 0, 255);

// Get the style pressure value
GetPPString(szSection, IDS_BS_STYLEPRESSURE, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->StylePressure = 50;
else
	lpBrush->StylePressure = Ascii2Int(szValue);
lpBrush->StylePressure = bound(TOGRAY(lpBrush->StylePressure), 0, 255);

#ifdef UNUSED
// Get the surface texture
GetPPString(szSection, IDS_BS_SURFACE, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->SurfaceFrame = NULL;
else 
	{
	lstrcpy(lpBrush->SurfaceName, szValue);
	if ( !LookupExtFileN(szValue, szFile, IDN_TEXTURE, NO))
		{
		Message(IDS_BRUSHINVALID, (LPTR)szValue);
		if (lpBrush->SourceFrame)
				FrameClose(lpBrush->SourceFrame);
		FreeUpMgxBrush(lpBrush);
		return(NULL);
		}
	FRMTYPEINFO TypeInfo;

	FrameSetTypeInfo(&TypeInfo, FDT_GRAYSCALE, NULL);
	CReadImage read(szFile);
	if (!(lpBrush->SurfaceFrame = read.ReadFrame(TypeInfo) ))
		{
		Message(IDS_BRUSHINVALID, (LPTR)szValue);
		if (lpBrush->SourceFrame)
				FrameClose(lpBrush->SourceFrame);
		FreeUpMgxBrush(lpBrush);
		return(NULL);
		}
	}
#endif

// Get the flip mode
GetPPString(szSection, IDS_BS_SURFACEFLIP, IDS_BS_FLIPNONE, szValue,
			sizeof(szValue), szFileName);
lpBrush->SurfaceFlipHorz = lpBrush->SurfaceFlipVert = NO;
if (Equal(szValue, IDS_BS_TILEHORZ))
	lpBrush->SurfaceFlipHorz = YES;
else if (Equal(szValue, IDS_BS_TILEVERT))
	lpBrush->SurfaceFlipVert = YES;
else if (Equal(szValue, IDS_BS_TILEBOTH))
	{
	lpBrush->SurfaceFlipHorz = YES;
	lpBrush->SurfaceFlipVert = YES;
	}

// Get the Merge Mode to be used
GetPPString(szSection, IDS_BS_MERGEMODE, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
lpBrush->ReadOnlyMask |= RO_MERGEMODE;
if (Equal(szValue, IDS_BS_ADDITIVE))
	lpBrush->MergeMode = MM_ADDITIVE;
else if (Equal(szValue, IDS_BS_SUBTRACTIVE))
	lpBrush->MergeMode = MM_SUBTRACTIVE;
else if (Equal(szValue, IDS_BS_IFLIGHTER))
	lpBrush->MergeMode = MM_IFLIGHTER;
else if (Equal(szValue, IDS_BS_IFDARKER))
	lpBrush->MergeMode = MM_IFDARKER;
else if (Equal(szValue, IDS_BS_HUESAT))
	lpBrush->MergeMode = MM_HUESAT;
else if (Equal(szValue, IDS_BS_HUE))
	lpBrush->MergeMode = MM_HUE;
else if (Equal(szValue, IDS_BS_SAT))
	lpBrush->MergeMode = MM_SAT;
else if (Equal(szValue, IDS_BS_LUM))
	lpBrush->MergeMode = MM_LUM;
else if (Equal(szValue, IDS_BS_RED))
	lpBrush->MergeMode = MM_RED;
else if (Equal(szValue, IDS_BS_GREEN))
	lpBrush->MergeMode = MM_GREEN;
else if (Equal(szValue, IDS_BS_BLUE))
	lpBrush->MergeMode = MM_BLUE;
else if (Equal(szValue, IDS_BS_CYAN))
	lpBrush->MergeMode = MM_CYAN;
else if (Equal(szValue, IDS_BS_MAGENTA))
	lpBrush->MergeMode = MM_MAGENTA;
else if (Equal(szValue, IDS_BS_YELLOW))
	lpBrush->MergeMode = MM_YELLOW;
else if (Equal(szValue, IDS_BS_BLACK))
	lpBrush->MergeMode = MM_BLACK;
else if (Equal(szValue, IDS_BS_SCREEN))
	lpBrush->MergeMode = MM_SCREEN;
else if (Equal(szValue, IDS_BS_MULTIPLY))
	lpBrush->MergeMode = MM_MULTIPLY;
else if (Equal(szValue, IDS_BS_DIFFERENCE))
	lpBrush->MergeMode = MM_DIFFERENCE;
else if (Equal(szValue, IDS_BS_TEXTURIZE))
	lpBrush->MergeMode = MM_TEXTURIZE;
else 
	{
	lpBrush->ReadOnlyMask &= ~RO_MERGEMODE; // beware the squiggle in Xywrite
	lpBrush->MergeMode = MM_NORMAL;
	}
// Get the spacing value
GetPPString(szSection, IDS_BS_SPACING, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->Spacing = 1;
else
	lpBrush->Spacing = Ascii2Int(szValue);
// Get the smoothness value
GetPPString(szSection, IDS_BS_SMOOTHNESS, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_DEFAULT))
	lpBrush->Smoothness = 100;
else
	lpBrush->Smoothness = Ascii2Int(szValue);
// Get the overlap mode
GetPPString(szSection, IDS_BS_OVERLAP, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_ON))
	lpBrush->NoOverlap = NO;
else
	lpBrush->NoOverlap = YES;

// Get the protection
GetPPString(szSection, IDS_BS_PROTECT, IDS_BS_DEFAULT, szValue,
			sizeof(szValue), szFileName);
if (Equal(szValue, IDS_BS_READONLY))
	lpBrush->Protection = PS_READONLY;
else
if (Equal(szValue, IDS_BS_NODELETE))
	lpBrush->Protection = PS_NODELETE;
else
	lpBrush->Protection = PS_NORMAL;

return((HMGXBRUSH)lpBrush);
}

/************************************************************************/
BOOL WriteMgxBrush(HMGXBRUSH hBrush, LPSTR lpFileName)
/************************************************************************/
{
int      id;
STRING   szSection, szValue;
LPSTR    lpString;
LPMGXBRUSH lpBrush;
BOOL     fRet, fExists;

if (!hBrush)
	return(FALSE);
lpBrush = (LPMGXBRUSH)hBrush;

fExists = FileExists(lpFileName);
if (fExists && (lpBrush->Protection & PS_READONLY))
	{
	Message(IDS_BRUSH_READONLY);
	return(FALSE);
	}

if (AstralStr(IDS_BS_SECTION, &lpString))
	lstrcpy(szSection, lpString);
else
	lstrcpy(szSection, "");

// Write the source color
if (lpBrush->SourceColor == SC_RGB_COLOR)
	{
	RGBAscii(&lpBrush->ColorStruct.rgb, szValue);
	fRet = WritePPString(szSection, IDS_BS_SOURCECOLOR, 0, szValue,
			lpFileName);
	}
else
	{
	if (lpBrush->SourceColor == SC_ACTIVE_COLOR)
		id = IDS_BS_ACTIVECOLOR;
	else
		id = IDS_BS_ALTERNATECOLOR;
	fRet = WritePPString(szSection, IDS_BS_SOURCECOLOR, id, NULL,
			lpFileName);
	}
	
// Write the destination data
if (lpBrush->DestData == DD_CURRENT_UNDO)
	id = IDS_BS_CURRENTUNDO;
else
	id = IDS_BS_CURRENTIMAGE;
fRet = WritePPString(szSection, IDS_BS_DESTDATA, id, NULL,
			lpFileName);

// Get the source data
if (lpBrush->SourceData == SD_FILE)
	{
	fRet = WritePPString(szSection, IDS_BS_SOURCEDATA, 0,
				lpBrush->szSourceExtName, lpFileName);
	}
else
	{
	if (lpBrush->SourceData == SD_NONE ||
		lpBrush->SourceData == SD_LAST_DEST ||
		lpBrush->SourceData == SD_MODIFIED_DEST)
		id = IDS_BS_DEFAULT;
	else
	if (lpBrush->SourceData == SD_CURRENT_DEST)
		id = IDS_BS_CURRENTDEST;
	else
		id = IDS_BS_CURRENTUNDO;
	fRet = WritePPString(szSection, IDS_BS_SOURCEDATA, id, NULL,
			lpFileName);
	}

// Write the flip mode
if (lpBrush->SourceFlipMode == SFM_MIRROR && lpBrush->SourceFlipHorz &&
	lpBrush->SourceFlipVert)
	id = IDS_BS_MIRRORBOTH;
else
if (lpBrush->SourceFlipMode == SFM_MIRROR && lpBrush->SourceFlipHorz)
	id = IDS_BS_MIRRORHORZ;
else
if (lpBrush->SourceFlipMode == SFM_MIRROR && lpBrush->SourceFlipVert)
	id = IDS_BS_MIRRORVERT;
else
if (lpBrush->SourceFlipMode == SFM_TILE && lpBrush->SourceFlipHorz &&
	lpBrush->SourceFlipVert)
	id = IDS_BS_TILEBOTH;
else
if (lpBrush->SourceFlipMode == SFM_TILE && lpBrush->SourceFlipHorz)
	id = IDS_BS_TILEHORZ;
else
if (lpBrush->SourceFlipMode == SFM_TILE && lpBrush->SourceFlipVert)
	id = IDS_BS_TILEVERT;
else
if (lpBrush->SourceFlipMode == SFM_TILE)
	id = IDS_BS_TILE;
else
	id  = IDS_BS_FLIPNONE;
fRet = WritePPString(szSection, IDS_BS_FLIPMODE, id, NULL,
			lpFileName);

// Write the source creation function
if (lpBrush->SourceFunction == SF_COPY_IMAGE)
	id = IDS_BS_COPY;
else
if (lpBrush->SourceFunction == SF_SHARPEN_IMAGE)
	id = IDS_BS_SHARPEN;
else
if (lpBrush->SourceFunction == SF_SMOOTH_IMAGE)
	id = IDS_BS_SMOOTH;
else
if (lpBrush->SourceFunction == SF_LIGHTEN_IMAGE)
	id = IDS_BS_LIGHTEN;
else
if (lpBrush->SourceFunction == SF_DARKEN_IMAGE)
	id = IDS_BS_DARKEN;
else
if (lpBrush->SourceFunction == SF_SMEAR_IMAGE)
	id = IDS_BS_SMEAR;
else
	id = IDS_BS_SOLIDCOLOR;
fRet = WritePPString(szSection, IDS_BS_SOURCEFUNC, id, NULL,
			lpFileName);

// Get the stroke method
if (lpBrush->StrokeMethod == SM_CONNECTED)
	id = IDS_BS_CONNECTED;
else
	id = IDS_BS_UNCONNECTED;
fRet = WritePPString(szSection, IDS_BS_STROKEMETHOD, id, NULL,
			lpFileName);

// Write the brush style
if (lpBrush->Style == IDC_BRUSHSCATTER)
	id = IDS_BS_SCATTERED;
else
	id = IDS_BS_SOLID;
fRet = WritePPString(szSection, IDS_BS_STYLE, id, NULL,
			lpFileName);

// Write the brush size
if (lpBrush->ReadOnlyMask & RO_SIZE)
	fRet = WritePPInt(szSection, IDS_BS_SIZE, lpBrush->Size, lpFileName);
else
	fRet = WritePPString(szSection, IDS_BS_SIZE, IDS_BS_DEFAULT, NULL,
			lpFileName);

if (lpBrush->Shape != IDC_BRUSHCUSTOM || !(lpBrush->ReadOnlyMask & RO_SHAPE))
	{
	if (!(lpBrush->ReadOnlyMask & RO_SHAPE))
		id = IDS_BS_DEFAULT;
	else
	if (lpBrush->Shape == IDC_BRUSHCIRCLE)
		id = IDS_BS_CIRCLE;
	else
	if (lpBrush->Shape == IDC_BRUSHSQUARE)
		id = IDS_BS_SQUARE;
	else
	if (lpBrush->Shape == IDC_BRUSHVERTICAL)
		id = IDS_BS_VERTICAL;
	else
	if (lpBrush->Shape == IDC_BRUSHHORIZONTAL)
		id = IDS_BS_HORIZONTAL;
	else
	if (lpBrush->Shape == IDC_BRUSHSLASH)
		id = IDS_BS_SLASH;
	else
		id = IDS_BS_BACKSLASH;
	fRet = WritePPString(szSection, IDS_BS_SHAPE, id, NULL,
			lpFileName);
	}
else
	{
	fRet = WritePPString(szSection, IDS_BS_SHAPE, 0, lpBrush->ShapeName,
			lpFileName);
	}

// Write the pressure value
if (lpBrush->ReadOnlyMask & RO_PRESSURE)
	fRet = WritePPInt(szSection, IDS_BS_PRESSURE, TOPERCENT(lpBrush->Pressure),
			lpFileName);
else
	fRet = WritePPString(szSection, IDS_BS_PRESSURE, IDS_BS_DEFAULT, NULL,
			lpFileName);
	
// Write the Weight value
fRet = WritePPInt(szSection, IDS_BS_WEIGHT, lpBrush->Weight,
			lpFileName);

// Get the feathered edge type
if (lpBrush->EdgeType == ET_AIRBRUSH)
	id = IDS_BS_AIRBRUSH;
else
	id = IDS_BS_PAINT;
fRet = WritePPString(szSection, IDS_BS_EDGETYPE, id, NULL,
			lpFileName);

// Get the feather percentage
if (lpBrush->ReadOnlyMask & RO_FEATHER)
	fRet = WritePPInt(szSection, IDS_BS_FEATHER, lpBrush->Feather,
			lpFileName);
else
	fRet = WritePPString(szSection, IDS_BS_FEATHER, IDS_BS_DEFAULT, NULL,
			lpFileName);

// Get the fade in/out percentage
fRet = WritePPInt(szSection, IDS_BS_FADE, lpBrush->FadeSize,
			lpFileName);

// Get the brush transparency
if (lpBrush->ReadOnlyMask & RO_OPACITY)
	fRet = WritePPInt(szSection, IDS_BS_TRANSPARENCY, 100-TOPERCENT(lpBrush->Opacity),
			lpFileName);
else
	fRet = WritePPString(szSection, IDS_BS_TRANSPARENCY, IDS_BS_DEFAULT, NULL,
			lpFileName);

// Get the brush transparency scale factor
fRet = WritePPInt(szSection, IDS_BS_OPACITYSCALE, lpBrush->OpacityScale,
			lpFileName);

// Get the tilt percentage - TBD
fRet = WritePPInt(szSection, IDS_BS_TILT, TOPERCENT(lpBrush->Tilt),
			lpFileName);

// Get the timer value
fRet = WritePPInt(szSection, IDS_BS_TIMER, lpBrush->Timer,
			lpFileName);

// Get the maximum flow rate
fRet = WritePPInt(szSection, IDS_BS_MAXFLOWRATE,
			(int)FMUL(1, lpBrush->MaxFlowRate), lpFileName);

// Get the flow rate sensitivity
fRet = WritePPInt(szSection, IDS_BS_SENSITIVITY, lpBrush->Sensitivity,
			lpFileName);

// Get the maximum flow change allowed
fRet = WritePPInt(szSection, IDS_BS_MAXFLOWCHANGE,
					TOPERCENT(lpBrush->MaxFlowChange),
			lpFileName);

// Get the style pressure value
fRet = WritePPInt(szSection, IDS_BS_STYLEPRESSURE, TOPERCENT(lpBrush->StylePressure),
			lpFileName);

// Get the surface texture
if (lpBrush->SurfaceFrame)
	fRet = WritePPString(szSection, IDS_BS_SURFACE, 0, lpBrush->SurfaceName,
			lpFileName);
else
	fRet = WritePPString(szSection, IDS_BS_SURFACE, IDS_BS_DEFAULT, NULL,
			lpFileName);

// Get the surface flip mode
if (lpBrush->SurfaceFlipHorz && lpBrush->SurfaceFlipVert)
	id = IDS_BS_TILEBOTH;
else
if (lpBrush->SurfaceFlipHorz)
	id = IDS_BS_TILEHORZ;
else
if (lpBrush->SurfaceFlipVert)
	id = IDS_BS_TILEVERT;
else
	id = IDS_BS_FLIPNONE;
fRet = WritePPString(szSection, IDS_BS_SURFACEFLIP, id, NULL,
			lpFileName);

// Get the Merge Mode to be used
if (lpBrush->MergeMode == MM_ADDITIVE)
	id = IDS_BS_ADDITIVE;
else
if (lpBrush->MergeMode == MM_SUBTRACTIVE)
	id = IDS_BS_SUBTRACTIVE;
else
if (lpBrush->MergeMode == MM_IFLIGHTER)
	id = IDS_BS_IFLIGHTER;
else
if (lpBrush->MergeMode == MM_IFDARKER)
	id = IDS_BS_IFDARKER;
else
if (lpBrush->MergeMode == MM_HUESAT)
	id = IDS_BS_HUESAT;
else
if (lpBrush->MergeMode == MM_HUE)
	id = IDS_BS_HUE;
else
if (lpBrush->MergeMode == MM_SAT)
	id = IDS_BS_SAT;
else
if (lpBrush->MergeMode == MM_LUM)
	id = IDS_BS_LUM;
else
if (lpBrush->MergeMode == MM_RED)
	id = IDS_BS_RED;
else
if (lpBrush->MergeMode == MM_GREEN)
	id = IDS_BS_GREEN;
else
if (lpBrush->MergeMode == MM_BLUE)
	id = IDS_BS_BLUE;
else
if (lpBrush->MergeMode == MM_CYAN)
	id = IDS_BS_CYAN;
else
if (lpBrush->MergeMode == MM_MAGENTA)
	id = IDS_BS_MAGENTA;
else
if (lpBrush->MergeMode == MM_YELLOW)
	id = IDS_BS_YELLOW;
else
if (lpBrush->MergeMode == MM_BLACK)
	id = IDS_BS_BLACK;
else
if (lpBrush->MergeMode == MM_SCREEN)
	id = IDS_BS_SCREEN;
else
if (lpBrush->MergeMode == MM_MULTIPLY)
	id = IDS_BS_MULTIPLY;
else
if (lpBrush->MergeMode == MM_DIFFERENCE)
	id = IDS_BS_DIFFERENCE;
else
if (lpBrush->MergeMode == MM_TEXTURIZE)
	id = IDS_BS_TEXTURIZE;
else  // MM_NORMAL
	id = IDS_BS_DEFAULT;
fRet = WritePPString(szSection, IDS_BS_MERGEMODE, id, NULL,
			lpFileName);
// write the spacing value
fRet = WritePPInt(szSection, IDS_BS_SPACING, lpBrush->Spacing, lpFileName);
// Write the smoothness value
fRet = WritePPInt(szSection, IDS_BS_SMOOTHNESS, lpBrush->Smoothness,
				lpFileName);
// Write the overlap mode
if (lpBrush->NoOverlap)
	id = IDS_BS_DEFAULT;
else
	id = IDS_BS_ON;
fRet = WritePPString(szSection, IDS_BS_OVERLAP, id, NULL, lpFileName);
if (fExists)
	{
	if (lpBrush->Protection & PS_READONLY)
		id = IDS_BS_READONLY;
	else
	if (lpBrush->Protection & PS_NODELETE)
		id = IDS_BS_NODELETE;
	else
		id = IDS_BS_DEFAULT; 
	}
else
	id = IDS_BS_DEFAULT;
fRet = WritePPString(szSection, IDS_BS_PROTECT, id, NULL, lpFileName);
return(fRet);
}

/************************************************************************/
LPMGXBRUSH CreateDefaultBrush()
/************************************************************************/
{
LPMGXBRUSH lpBrush;

// Allocate memory for brush
if (!(lpBrush = AllocMgxBrush()))
	{
	Message(IDS_EMEMALLOC);
	return(NULL);
	}
clr((LPTR)lpBrush, sizeof(MGXBRUSH));

lpBrush->DestData = DD_CURRENT_IMAGE;
lpBrush->SourceColor = SC_RGB_COLOR;
lpBrush->ColorStruct.gray = 0;
lpBrush->ColorStruct.ColorSpace = CS_GRAY;
SetColorInfo(&lpBrush->ColorStruct, &lpBrush->ColorStruct,
			lpBrush->ColorStruct.ColorSpace);
lpBrush->SourceData = SD_NONE;
lpBrush->SourceFrame = NULL;
lpBrush->SourceFlipMode = SFM_NO_FLIP;
lpBrush->SourceFlipHorz = lpBrush->SourceFlipVert = NO;
lpBrush->SourceFunction = SF_SOLID_COLOR;
lpBrush->StrokeMethod = SM_CONNECTED;
lpBrush->Shape = IDC_BRUSHCIRCLE;
lpBrush->ShapeName[0] = '\0';
lpBrush->Style = IDC_BRUSHSOLID;
lpBrush->Size = 50;
lpBrush->Pressure = 50;
lpBrush->EdgeType = ET_PAINT;
lpBrush->Feather = 100;
lpBrush->FadeSize = 0;
lpBrush->FadeOpacity = 0;
lpBrush->Opacity = 255;
lpBrush->OpacityScale = 100;
lpBrush->Tilt = 0;
lpBrush->MergeMode = MM_NORMAL;
lpBrush->SurfaceName[0] = '\0';
lpBrush->SurfaceFrame = NULL;
lpBrush->SurfaceFlipHorz = lpBrush->SurfaceFlipVert = NO;
lpBrush->Timer = 0;
lpBrush->MaxFlowRate = 0;
lpBrush->Sensitivity = 1;
lpBrush->MaxFlowChange = 0;
lpBrush->StylePressure = 50;
lpBrush->ReadOnlyMask = 0;
lpBrush->Weight = 0;
lpBrush->szSourceExtName[0] = '\0';
lpBrush->szSource[0] = '\0';
lpBrush->xSrcOffset = 0;
lpBrush->ySrcOffset = 0;
lpBrush->Spacing = 15;
lpBrush->Smoothness = 0;
lpBrush->NoOverlap = NO;
lpBrush->Protection = PS_NORMAL;

return(lpBrush);
}

/************************************************************************/
HMGXBRUSH CreateMgxEraser( ITEMID wEraserType )
/************************************************************************/
{
LPMGXBRUSH lpBrush;

if (!(lpBrush = CreateDefaultBrush()))
	{
	Message(IDS_EMEMALLOC);
	return(NULL);
	}

lpBrush->SourceData = SD_CURRENT_UNDO;
lpBrush->SourceFunction = SF_COPY_IMAGE;
return((HMGXBRUSH)lpBrush);
}

/************************************************************************/
HMGXBRUSH CreateMgxColorBrush(LPCOLORINFO lpColorStruct, int Feather)
/************************************************************************/
{
LPMGXBRUSH lpBrush;

if (!(lpBrush = CreateDefaultBrush()))
	{
	Message(IDS_EMEMALLOC);
	return(NULL);
	}
lpBrush->SourceColor = SC_RGB_COLOR;
lpBrush->ColorStruct = *lpColorStruct;
SetColorInfo(&lpBrush->ColorStruct, &lpBrush->ColorStruct,
			lpBrush->ColorStruct.ColorSpace);
lpBrush->SourceFunction = SF_SOLID_COLOR;
lpBrush->Feather = Feather;
return((HMGXBRUSH)lpBrush);
}

/************************************************************************/
HMGXBRUSH CreateFeatherMaskBrush(int Size, BYTE gray)
/************************************************************************/
{
LPMGXBRUSH lpBrush;
COLORINFO Color;

Color.gray = gray;
SetColorInfo(&Color,&Color,CS_GRAY);
lpBrush = (LPMGXBRUSH)CreateMgxColorBrush(&Color, 0);
if (!lpBrush)
	return(NULL);
lpBrush->Size = Size;
lpBrush->Shape = IDC_BRUSHCIRCLE;
lpBrush->Opacity = 255;
lpBrush->StrokeMethod = SM_UNCONNECTED;

return((HMGXBRUSH)lpBrush);
}

/************************************************************************/
void DestroyMgxBrush(HMGXBRUSH hBrush)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (!hBrush)
	return;
lpBrush = (LPMGXBRUSH)hBrush;
if (lpBrush->SourceFrame && (lpBrush->SourceData == SD_FILE))
	FrameClose(lpBrush->SourceFrame);
if (lpBrush->SurfaceFrame)
	FrameClose(lpBrush->SurfaceFrame);
FreeUpMgxBrush(lpBrush);
}

/************************************************************************/
void FreeUpMgxBrushMask(HMGXBRUSH hBrush)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (!hBrush)
	return;
lpBrush = (LPMGXBRUSH)hBrush;
if (lpBrush->BrushMask)
	{
	FreeUp(lpBrush->BrushMask);
	lpBrush->BrushMask = NULL;
	}
}

/************************************************************************/
void SetMgxBrushSource(HMGXBRUSH hBrush, LPFRAME lpFrame,
						int xSrcOffset, int ySrcOffset)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (!(lpBrush->ReadOnlyMask & RO_SOURCE_DATA))
		{
		lpBrush->SourceFrame = lpFrame;
		lpBrush->xSrcOffset = xSrcOffset;
		lpBrush->ySrcOffset = ySrcOffset;
		}
	}
}

/************************************************************************/
int GetMgxBrushSource(HMGXBRUSH hBrush)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	return(lpBrush->SourceData);
	}
return(SD_CURRENT_DEST);
}

/************************************************************************/
void GetMgxBrushSourceColor(HMGXBRUSH hBrush, LPCOLORINFO lpColor)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	*lpColor = lpBrush->ColorStruct;
	}
}

/************************************************************************/
LPFRAME GetMgxBrushSourceFrame(HMGXBRUSH hBrush)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	return(lpBrush->SourceFrame);
	}
return(NULL);
}

/************************************************************************/
void SetMgxBrushSize(HMGXBRUSH hBrush, int BrushSize)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (!(lpBrush->ReadOnlyMask & RO_SIZE))
		{
		lpBrush->Size = BrushSize;
		if (lpBrush->BrushMask)
			{
			FreeUp(lpBrush->BrushMask);
			lpBrush->BrushMask = NULL;
			}
		}
	}
}

/************************************************************************/
int SetMgxBrushStyle(HMGXBRUSH hBrush, int BrushStyle)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldStyle = IDC_BRUSHSOLID;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	OldStyle = lpBrush->Style;
	lpBrush->Style = BrushStyle;
	}
return(OldStyle);
}

/************************************************************************/
int SetMgxStylePressure(HMGXBRUSH hBrush, int StylePressure)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldPressure = 255;
if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	OldPressure = lpBrush->StylePressure;
	lpBrush->StylePressure = StylePressure;
	}
return(OldPressure);
}

/************************************************************************/
void SetMgxBrushFeather(HMGXBRUSH hBrush, int Feather)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (!(lpBrush->ReadOnlyMask & RO_FEATHER))
		{
		lpBrush->Feather = Feather;
		if (lpBrush->Shape != IDC_BRUSHCUSTOM && lpBrush->BrushMask)
			{
			FreeUp(lpBrush->BrushMask);
			lpBrush->BrushMask = NULL;
			}
		}
	}
}

/************************************************************************/
BOOL SetMgxBrushShape(HMGXBRUSH hBrush, int BrushShape, LPSTR lpShapeName)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int         OldShape;
HPTR 		lpOldBrushMask;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (!(lpBrush->ReadOnlyMask & RO_SHAPE))
		{
		OldShape = lpBrush->Shape;
		lpBrush->Shape = BrushShape;
		if (BrushShape == IDC_BRUSHCUSTOM)
			{
			lpOldBrushMask = lpBrush->BrushMask;
			lpBrush->BrushMask = NULL;
			if (SetupCustomBrushMask(lpBrush, lpShapeName))
				{
				lstrcpy(lpBrush->ShapeName, lpShapeName);
				if (lpOldBrushMask)
					FreeUp(lpOldBrushMask);
				}
			else
				{
				lpBrush->BrushMask = lpOldBrushMask;
				lpBrush->Shape = OldShape;
				if (OldShape == IDC_BRUSHCUSTOM)
					SetupCustomBrushMask(lpBrush, lpBrush->ShapeName);
				return(FALSE);
				}
			}
		else
			{
			if (lpBrush->BrushMask)
				{
				FreeUp(lpBrush->BrushMask);
				lpBrush->BrushMask = NULL;
				}
			}
		return(TRUE);
		}
	}
return(FALSE);
}

/************************************************************************/
int SetMgxBrushOpacity(HMGXBRUSH hBrush, int Opacity)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldOpacity = 255;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	OldOpacity = lpBrush->Opacity;
	if (!(lpBrush->ReadOnlyMask & RO_OPACITY))
		lpBrush->Opacity = Opacity;
	}
return(OldOpacity);
}

/************************************************************************/
int SetMgxBrushOpacityScale(HMGXBRUSH hBrush, int OpacityScale)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldScale = 0;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	OldScale = lpBrush->OpacityScale;
	lpBrush->OpacityScale = OpacityScale;
	}
return(OldScale);
}

/************************************************************************/
void SetMgxBrushColor(HMGXBRUSH hBrush, LPCOLORINFO lpColorStruct)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	lpBrush->SourceColor = SC_RGB_COLOR;
	lpBrush->ColorStruct = *lpColorStruct;
	SetColorInfo(&lpBrush->ColorStruct, &lpBrush->ColorStruct,
			lpBrush->ColorStruct.ColorSpace);
	}
}

/************************************************************************/
void SetMgxBrushFlipMode(HMGXBRUSH hBrush, int FlipMode, BOOL FlipHorz, BOOL FlipVert)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (!(lpBrush->ReadOnlyMask & RO_FLIPMODE))
		{
		lpBrush->SourceFlipMode = FlipMode;
		lpBrush->SourceFlipHorz = FlipHorz;
		lpBrush->SourceFlipVert = FlipVert;
		}
	}
}

/************************************************************************/
void SetMgxBrushPressure(HMGXBRUSH hBrush, int Pressure)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (!(lpBrush->ReadOnlyMask & RO_PRESSURE))
		lpBrush->Pressure = Pressure;
	}
}

/************************************************************************/
MERGE_MODE SetMgxBrushMergeMode(HMGXBRUSH hBrush, MERGE_MODE MergeMode)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
MERGE_MODE oldMode = MM_NORMAL;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	oldMode = lpBrush->MergeMode;
	if (!(lpBrush->ReadOnlyMask & RO_MERGEMODE))
		lpBrush->MergeMode = MergeMode;
	}
return(oldMode);
}


/************************************************************************/
int SetMgxBrushStrokeMethod(HMGXBRUSH hBrush, int idStrokeMethod)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int idOldMethod = 0;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	idOldMethod = lpBrush->StrokeMethod;
	lpBrush->StrokeMethod = idStrokeMethod;
	}
return(idOldMethod);
}


/************************************************************************/
int GetMgxBrushSize(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Size);
else
	return(0);
}

/************************************************************************/
int GetMgxStylePressure(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->StylePressure);
else
	return(255);
}

/************************************************************************/
int GetMgxBrushStyle(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Style);
else
	return(IDC_BRUSHSOLID);
}


/************************************************************************/
int GetMgxBrushFeather(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Feather);
else
	return(0);
}

/************************************************************************/
int GetMgxBrushShape(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Shape);
else
	return(IDC_BRUSHCIRCLE);
}



/************************************************************************/
int GetMgxBrushOpacity(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Opacity);
return(255);
}

/************************************************************************/
int GetMgxBrushOpacityScale(HMGXBRUSH hBrush)
/************************************************************************/
{

if (hBrush)
	return(((LPMGXBRUSH)hBrush)->OpacityScale);
return(0);
}


/************************************************************************/
int GetMgxBrushFlipMode(HMGXBRUSH hBrush, LPINT FlipHorz, LPINT FlipVert)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	*FlipHorz = lpBrush->SourceFlipHorz;
	*FlipVert = lpBrush->SourceFlipVert;
	return(lpBrush->SourceFlipMode);
	}
return(SFM_NO_FLIP);
}

/************************************************************************/
int GetMgxBrushPressure(HMGXBRUSH hBrush)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	return(lpBrush->Pressure);
	}
return(0);
}

/************************************************************************/
MERGE_MODE GetMgxBrushMergeMode(HMGXBRUSH hBrush, LPINT lpReadOnly)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	if (lpReadOnly)
		*lpReadOnly = (lpBrush->ReadOnlyMask & RO_MERGEMODE) != 0;
	return(lpBrush->MergeMode);
	}
else
	{
	if (lpReadOnly)
		*lpReadOnly = NO;
	return(MM_NORMAL);
	}
}

/************************************************************************/
void MgxBrushMirror(HMGXBRUSH hBrush, LPRECT lpSrcRect, LPRECT lpDstRect,
			int npix, int nlin)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (!hBrush)
	return;
lpBrush = (LPMGXBRUSH)hBrush;
if (lpBrush->SourceFlipMode != SFM_MIRROR)
	return;
MgxMirrorRect(lpSrcRect, lpDstRect, npix, nlin, lpBrush->SourceFlipHorz,
		lpBrush->SourceFlipVert);
}

/************************************************************************/
void MgxMirrorRect(LPRECT lpSrcRect, LPRECT lpDstRect,
			int npix, int nlin, BOOL FlipHorz, BOOL FlipVert)
/************************************************************************/
{
int top, left;

if (FlipVert)
	{
	top = lpSrcRect->top;
	lpDstRect->top = (nlin-1) - lpSrcRect->bottom;
	lpDstRect->bottom = (nlin-1) - top;
	}
else
	{
	lpDstRect->top = lpSrcRect->top;
	lpDstRect->bottom = lpSrcRect->bottom;
	}
if (FlipHorz)
	{
	left = lpSrcRect->left;
	lpDstRect->left = (npix-1) - lpSrcRect->right;
	lpDstRect->right = (npix-1) - left;
	}
else
	{
	lpDstRect->left = lpSrcRect->left;
	lpDstRect->right = lpSrcRect->right;
	}
}

/************************************************************************/
static void NewDoDDABrushStroke(int sx, int sy, int dx, int dy)
/************************************************************************/
{
int deltax, deltay, i;

// Initialize for first stroke that we get
if (fFirstStroke)
	{
	sx_delta = sx - dx; // setup deltas between source and dest
	sy_delta = sy - dy;
	start_dx = dx;		 // setup initial starting point
	start_dy = dy;
	last_x_processed = INT_MAX;
	last_y_processed = INT_MAX;
	fFirstStroke = FALSE; // no longer first stroke
	fStrokeProcessed = FALSE;
	State |= STATE_FIRSTSTROKE;
	for (i = 0; i < 4; ++i)
		bRects[i].fEmpty = YES;
//	if (lpBrush->MaxFlowRate)
//		return;
	}
else if (dx == start_dx && dy == start_dy)
	return; // point has moved - don't do anything

// Set state for a new brush stroke
State |= STATE_NEWSTROKE;

// setup new ending point - start point was the last point
end_dx = dx;
end_dy = dy;

deltax = abs(end_dx-start_dx);
deltay = abs(end_dy-start_dy);

// Save direction of stroke based on type of strips being processed
fRight = start_dx <= end_dx;
fBottom = start_dy <= end_dy;

if (fRight && fBottom)
	pPtTable = lr;
else if (fRight)
	pPtTable = ur;
else if (fBottom)
	pPtTable = ll;
else
	pPtTable = ul;

// Setup flow rate for this stroke
//if (lpBrush->MaxFlowRate)
//	ModifyFlowRate(deltax, deltay, fStrokeProcessed);

// Setup pressure pen and fade settings for this stroke
if (fUsePressure)
	PressureSetup(max(deltax, deltay));

if (deltax || deltay)
	{
	if (deltax > 1 || deltay > 1)
		ddaline(start_dx, start_dy, end_dx, end_dy, NewProcessRectList);
	else
		{
		NewProcessRectList(start_dx, start_dy);
		NewProcessRectList(end_dx, end_dy);
		}
	}
else
	NewProcessRectList(start_dx, start_dy);

fStrokeProcessed = TRUE;

// next strokes start becomes this strokes end
start_dx = end_dx;
start_dy = end_dy;
}

/************************************************************************/
static void DoBrushStroke(int sx, int sy, int dx, int dy)
/************************************************************************/
{
RECT rBrush;

if (!BrushFull)
	return;

sx_delta = sx - dx; // setup deltas between source and dest
sy_delta = sy - dy;
rBrush.left = dx - BrushHalf;
rBrush.right = rBrush.left + BrushFull - 1;
rBrush.top = dy - BrushHalf;
rBrush.bottom = rBrush.top + BrushFull - 1;
if (fUpdateDisplay)
	DisplayBrush(0, 0, 0, OFF);
BltRect(&rBrush, lpBM);
FadeOutBrush();
if (fUpdateDisplay)
	DisplayBrush(hWnd, brush_x, brush_y, ON);
}

/************************************************************************/
static void NewProcessRectList( int x, int y )
/************************************************************************/
{
RECT bRect, iRect, rSect;
BOOL fOnImage, fAnyEmpty, fAllEmpty;
int i, j, empty, pt, pi;
WORD	xTile, yTile;
POINT pts[4];
PBRUSHRECT pbRect;

if (abs(last_x_processed-x) < Spacing &&
	abs(last_y_processed-y) < Spacing)
	{
	FadeOutBrush();
	return;
	}

last_x_processed = x;
last_y_processed = y;
if (!BrushFull)
	{
	// Adjust fade values
	FadeOutBrush();
	return;
	}

// get rectangle this brush stroke encompasses
bRect.top = y - BrushHalf;
bRect.bottom = bRect.top + BrushFull - 1;
bRect.left = x - BrushHalf;
bRect.right = bRect.left + BrushFull - 1;

// merge brush in with current brush mask
if (fOnImage = AstralIntersectRect(&iRect, &rMax, &bRect))
	{
	if (fNoOverlap)
		OSMergeMask(lpMskFrame, lpBM, &bRect, BrushFull, &iRect, MaxOpacity);
	else
		BltRect(&bRect, lpBM);
	}

// See where this brush intersects and output any rectangles that
// the brush no longer intersects
fAnyEmpty = NO;
fAllEmpty = YES;
for (i = 0; i < 4; ++i)
	{
	if (bRects[i].fEmpty)
		{
		fAnyEmpty = YES;
		continue;
		}
	if (fOnImage && AstralIntersectRect(&rSect, &bRects[i].rTile, &iRect))
		{
		fAllEmpty = NO;
		AstralUnionRect(&bRects[i].rEdit, &bRects[i].rEdit, &rSect);
		}
	else
		{
		// if we have some previous rects, put them out
//		if (!fAllEmpty && !fNoOverlap)
//	  		{
//			MessageBeep(0);
//			for (j = 0; j < i; ++j)
//			 	{
//			 	NewBltRect(&bRects[j].rEdit);
//			 	bRects[j].fEmpty = YES;
//			 	}
//			}
		NewBltRect(&bRects[i].rEdit);
		bRects[i].fEmpty = YES;
		fAnyEmpty = YES;
		}
	}

// move all rects to front of list
if (!fAllEmpty && fAnyEmpty)
	{
	for (i = 0; i < 4; ++i)
		{
		if (!bRects[i].fEmpty)
			{
			for (j = 0; j < i; ++j)
				{
				if (bRects[j].fEmpty)
					{
					bRects[j] = bRects[i];
					bRects[i].fEmpty = YES;
					break;
					}
				}
			}
		}
	}

// if not on image we are all done
if (!fOnImage || !fAnyEmpty)
	{
	// Adjust fade values
	FadeOutBrush();
	return;
	}

// The code below is used to create a new tile for processing.
// The tricky part of the code is that it tries to order the
// tiles based on the direction of mouse movement.  You want to
// do this so that the display gets updated based on the direction
// of movement.  Otherwise, you can end up with a tile that
// gets painted out of order resulting a hole type thing.
pts[0].x = iRect.left;
pts[0].y = iRect.top;
pts[1].x = iRect.right;
pts[1].y = iRect.top;
pts[2].x = iRect.right;
pts[2].y = iRect.bottom;
pts[3].x = iRect.left;
pts[3].y = iRect.bottom;
for (pt = 0; pt < 4; ++pt)
	{
	// get which corner of our brush stroke to look at base
	// on mouse direction
	pi = pPtTable[pt];
	// find out which tile that corner lies on
	xTile = pts[pi].x / bh;
	yTile = pts[pi].y / bv;
	// see if tile already exists in our list
	empty = -1;
	for (i = 0; i < 4; ++i)
		{
		// capture first empty tile for later if necessary
		if (bRects[i].fEmpty)
			{
			if (empty < 0)
				empty = i;
			continue;
			}
		// see if we are on this tile
		if (bRects[i].xTile == xTile && bRects[i].yTile == yTile)
			break;
		}
	// see if we need to create a new tile
	if (i >= 4) // not found in rect list
		{
		// we need to create a new tile
		if (empty >= 0)
			{
			// create the new tile in first empty slot
			pbRect = &bRects[empty];
			pbRect->xTile = xTile;
			pbRect->yTile = yTile;
			pbRect->rTile.left = xTile * bh;
			pbRect->rTile.right = pbRect->rTile.left + bh - 1;
			pbRect->rTile.top = yTile * bv;
			pbRect->rTile.bottom = pbRect->rTile.top + bv - 1;
			AstralIntersectRect(&pbRect->rEdit, &pbRect->rTile,
								 &iRect);
			pbRect->fEmpty = NO;
			}
		else
			{
			ASSERT(FALSE);
			}
		}
	}

// Adjust fade values
FadeOutBrush();
}

/************************************************************************/
static void BltRect( PRECT lpRect, HPTR lpBrushMask)
/************************************************************************/
{
RECT  rDest, rSource, rOutput;
int   dx, dy, w, h, sx, sy;
DWORD  bufsize;
HPTR  lpBrushSrc, lpMskData;

// get destination x,y,width, and height of rect to be blt'ed
dx = lpRect->left;
dy = lpRect->top;
w = RectWidth(lpRect);
h = RectHeight(lpRect);

// get total number of pixels in rect to be blt'ed
bufsize = (DWORD)w * (DWORD)h;

if( fUsePressure && fUsePolling )    
    GetPacketsFromQueue();
else
    GetMouseMoves();

// Get the clipping rectangle for the destination
if (!LoadSourceData(lpDstPixmap, BrushData.pmDstType, dx, dy, w, h, NULL,
				(LPRECT)&rDest, NO, NO))
	return;

// load source data
if (lpSrcPixmap)
	{
	sx = dx + sx_delta; // get location of source
	sy = dy + sy_delta;
	if (!(*lpGetSourceData)(lpSrcPixmap, BrushData.pmSrcType, sx, sy, w, h, lpData,
			(LPRECT)&rSource, lpBrush->SourceFlipHorz, lpBrush->SourceFlipVert))
		return;

    if( fUsePressure && fUsePolling )    
        GetPacketsFromQueue();
    else
        GetMouseMoves();
	}
else
	rSource = rDest;

// call function to create the brush source from source data and source color
lpBrushSrc = (*lpCreateBrushSource)(dx, dy, w, h, State);
if( fUsePressure && fUsePolling )    
    GetPacketsFromQueue();
else
    GetMouseMoves();

// Reset state
State = 0;

// If an image mask is being used, combine it with the Brush Mask
if (UsingMask)
	{
	// load image mask
#ifdef WIN32
	LoadMaskData( dx+xDstOffset, dy+yDstOffset, w, h, lpMsk, lpMask, CR_COPY); 
	ScaleDataBuf8(lpMsk, bufsize, lpBrushMask);
#else
	LoadMaskDataHuge( dx+xDstOffset, dy+yDstOffset, w, h, lpMsk, lpMask, CR_COPY); 
	ScaleDataBuf8Huge(lpMsk, bufsize, lpBrushMask);
#endif
	lpMskData = lpMsk;

    if( fUsePressure && fUsePolling )    
        GetPacketsFromQueue();
    else
        GetMouseMoves();
	}
else if (!StylizeProc && !UsingShield)
	lpMskData = lpBrushMask;
else
	{
	hcopy(lpBrushMask, lpMsk, bufsize);
	lpMskData = lpMsk;
    if( fUsePressure && fUsePolling )    
        GetPacketsFromQueue();
    else
        GetMouseMoves();
	}

// Do style modifications to the mask, such as scatter
if (StylizeProc)
	{
	(*StylizeProc)(lpMskData, lpMskData, lpErr, w, h, lpBrush->StylePressure);
    if( fUsePressure && fUsePolling )    
        GetPacketsFromQueue();
    else
        GetMouseMoves();
	}

// Blt the new data to the frame
if (AstralIntersectRect(&rOutput, &rSource, &rDest))
	{
	BltToFrame(dx, dy, w, h, lpBrushSrc, &rOutput, NO, lpMskData);
    if( fUsePressure && fUsePolling )    
        GetPacketsFromQueue();
    else
        GetMouseMoves();
	}

// source function needs last destination data
// this is really a special case for smear, if the 
// destination rectangle got clipped the fill in the rest
// of the buffer with duplicate pixels - this prevents
// smearing in of white
if (lpBrush->SourceData == SD_LAST_DEST)
	{
	if (RectWidth(&rDest) != w || RectHeight(&rDest) != h)
		{
		FillInBuffer(lpData, w, h, &rDest);
        if( fUsePressure && fUsePolling )    
            GetPacketsFromQueue();
        else
            GetMouseMoves();
		}
	}
}

/************************************************************************/
static void FillInBuffer(HPTR lpBuf, int w, int h, LPRECT lpRect)
/************************************************************************/
{
int dx, dy;
WORD width;
HPTR lpLine, lpSrcPix;
LPFRAME lpFrame;
RECT rBuf;
long lPixel;

rBuf = *lpRect;
lpFrame = lpDstPixmap->EditFrame;
width = w * iBltFrameDepth;
dx = rBuf.left;	
if (dx)
	{
	dy = RectHeight(&rBuf);
	lpLine = lpBuf + (width * rBuf.top);
	while (--dy >= 0)
		{
		lpSrcPix = lpLine + (rBuf.left*iBltFrameDepth);
#ifdef WIN32
		frame_getpixel(lpSrcPix, iBltFrameDepth, &lPixel);	
		FrameSetPixel( lpFrame, lPixel, lpLine, dx);
#else
		GetPixelHuge(lpSrcPix, iBltFrameDepth, &lPixel);
		SetPixelHuge(lpFrame, lPixel, lpLine, dx);
#endif
		lpLine += width;
		}
	}
dx = w - rBuf.right - 1;	
if (dx)
	{
	dy = RectHeight(&rBuf);
	lpSrcPix = lpBuf + (width * rBuf.top);
	lpSrcPix += (rBuf.right * iBltFrameDepth);
	lpLine = lpSrcPix + iBltFrameDepth;
	while (--dy >= 0)
		{
#ifdef WIN32
		frame_getpixel(lpSrcPix, iBltFrameDepth, &lPixel);	
		FrameSetPixel( lpFrame, lPixel, lpLine, dx);
#else
		GetPixelHuge(lpSrcPix, iBltFrameDepth, &lPixel);
		SetPixelHuge(lpFrame, lPixel, lpLine, dx);
#endif
		lpSrcPix += width;
		lpLine += width;
		}
	}
dy = rBuf.top;	
if (dy)
	{
	lpLine = lpBuf;
	lpSrcPix = lpBuf + (width * rBuf.top);
	while (--dy >= 0)
		{
		hcopy(lpSrcPix, lpLine, width);
		lpLine += width;
		}
	}
dy = h - rBuf.bottom - 1;	
if (dy)
	{
	lpSrcPix = lpBuf + (width * rBuf.bottom);
	lpLine = lpSrcPix + width;
	while (--dy >= 0)
		{
		hcopy(lpSrcPix, lpLine, width);
		lpLine += width;
		}
	}
}

/************************************************************************/
LOCAL void SetPixelHuge(LPFRAME lpFrame, long lPixel, HPTR hpDst, int dx)
/************************************************************************/
{
DWORD dwLeft;
BYTE buf[4];
int i;

while (dx)
	{
	dwLeft = 65536L - ((DWORD)hpDst & 0xFFFFL);
	dwLeft /= iBltFrameDepth;
	if (!dwLeft)
		{
		FrameSetPixel( lpFrame, lPixel, buf, 1);
		for (i = 0; i < iBltFrameDepth; ++i)
			*hpDst++ = buf[i];
		--dx;
		}
	else
		{
		if (dwLeft > (DWORD)dx)
			dwLeft = dx;
		FrameSetPixel( lpFrame, lPixel, hpDst, (int)dwLeft);
		dx -= (int)dwLeft;
		hpDst += (dwLeft * iBltFrameDepth);
		}
	}
}

/************************************************************************/
static void NewBltRect(PRECT lpRect)
/************************************************************************/
{
RECT rDest, rSource, rOutput;
int dx, dy, w, h, sx, sy;
DWORD bufsize;
HPTR lpBrushSrc, lpTmp;

if (!fNoOverlap)
	{
	BltToDisplay(lpRect);
	return;
	}

// get destination x,y,width, and height of rect to be blt'ed
dx = lpRect->left;
dy = lpRect->top;
w = RectWidth(lpRect);
h = RectHeight(lpRect);

// get total number of pixels in rect to be blt'ed
bufsize = (DWORD)w * (DWORD)h;

if( fUsePressure && fUsePolling )    
    GetPacketsFromQueue();
else
    GetMouseMoves();

// Load the destination buffer with destination data
if (!LoadSourceData(lpDstPixmap, BrushData.pmDstType, dx, dy, w, h, lpDstData,
				(LPRECT)&rDest, NO, NO))
	return;
if( fUsePressure && fUsePolling )    
    GetPacketsFromQueue();
else
    GetMouseMoves();
// Load the mask buffer with data from mask frame
if (!LoadSourceData(lpMskPixmap, BrushData.pmMskType, dx, dy, w, h, lpMsk,
				 (LPRECT)&rDest, NO, NO))
	return;
if( fUsePressure && fUsePolling )    
    GetPacketsFromQueue();
else
    GetMouseMoves();

// load source data
if (lpSrcPixmap)
	{
	sx = dx + sx_delta; // get location of source
	sy = dy + sy_delta;
	if ((lpSrcPixmap == lpDstPixmap) && !sx_delta && !sy_delta)
		{
		hcopy(lpDstData, lpData, bufsize*iBltFrameDepth);
		rSource = rDest;
		}
	else
		{
		if (!(*lpGetSourceData)(lpSrcPixmap, BrushData.pmSrcType, sx, sy, w, h, lpData,
				(LPRECT)&rSource, lpBrush->SourceFlipHorz, lpBrush->SourceFlipVert))
		return;
		}
    if( fUsePressure && fUsePolling )    
        GetPacketsFromQueue();
    else
        GetMouseMoves();
	}
else
	{
	rSource = rDest;
	}

// call function to create the brush source from source data and source color
lpBrushSrc = (*lpCreateBrushSource)(dx, dy, w, h, State);
if( fUsePressure && fUsePolling )    
    GetPacketsFromQueue();
else
    GetMouseMoves();

// Reset state
State = 0;

// If an image mask is being used, combine it with the Brush Mask
if (UsingMask)
	{
	// load image mask
#ifdef WIN32
	LoadMaskData( dx+xDstOffset, dy+yDstOffset, w, h, lpMsk, lpMask, CR_MULTIPLY); 
#else
	LoadMaskDataHuge( dx+xDstOffset, dy+yDstOffset, w, h, lpMsk, lpMask, CR_MULTIPLY); 
#endif
    if( fUsePressure && fUsePolling )    
        GetPacketsFromQueue();
    else
        GetMouseMoves();
	}
if (UsingShield)
	(*lpHugeShieldProc)(lpMsk, lpDstData, bufsize, BrushData.BltFrameTypeInfo);

// Do style modifications to the mask, such as scatter
if (StylizeProc)
	{
	(*StylizeProc)(lpMsk, lpMsk, lpErr, w, h, lpBrush->StylePressure);
    if( fUsePressure && fUsePolling )    
        GetPacketsFromQueue();
    else
        GetMouseMoves();
	}

// Process the data by combining destination, source, and mask
(*lpHugeProcessProc)(lpDstData, lpBrushSrc, lpMsk, bufsize);
if( fUsePressure && fUsePolling )    
    GetPacketsFromQueue();
else
    GetMouseMoves();

// Blt the new data to the frame
if (AstralIntersectRect(&rOutput, &rSource, &rDest))
	{
	BltToFrame(dx, dy, w, h, lpDstData, &rOutput, YES, NULL);
    if( fUsePressure && fUsePolling )    
        GetPacketsFromQueue();
    else
        GetMouseMoves();
	}

// source function needs last destination data
// this is really a special case for smear, if the 
// destination rectangle got clipped the fill in the rest
// of the buffer with duplicate pixels - this prevents
// smearing in of white
if (lpBrush->SourceData == SD_LAST_DEST)
	{
	// swap last pointer and dest pointer
	lpTmp = lpDstData;
	lpDstData = lpData;
	lpData = lpTmp;
	if (RectWidth(&rDest) != w || RectHeight(&rDest) != h)
		{
		FillInBuffer(lpData, w, h, &rDest);
        if( fUsePressure && fUsePolling )    
            GetPacketsFromQueue();
        else
            GetMouseMoves();
		}
	}
}

/************************************************************************/
static void BltToFrame( int x, int y, int dh, int dv, HPTR hpData,
						PRECT lpRect, BOOL fUpdateDisplay,
						HPTR hpMsk)
/************************************************************************/
{
int sy;
WORD bwidth, width;
HPTR lp, hpLast;
RECT StrokeRect;
DWORD offset, dwBytes;

StrokeRect = *lpRect;

// get width of buffer in bytes
bwidth = dh*iBltFrameDepth;

// advance in the number of lines and pixels
offset = ((DWORD)StrokeRect.top * (DWORD)dh) +	(DWORD)StrokeRect.left;
hpData += (offset*(DWORD)iBltFrameDepth);
if (!fNoOverlap) // if overlap
	{
	hpMsk += offset;
	if (lpBrush->SourceData == SD_LAST_DEST)
		hpLast = lpData + (offset*iBltFrameDepth);
	}

// get width of area to be blt'ed
width = (StrokeRect.right-StrokeRect.left+1);
dwBytes = (DWORD)width * (DWORD)iBltFrameDepth;

// offset stroked area from buffer to frame
StrokeRect.left += x;
StrokeRect.right += x;
StrokeRect.top += y;
StrokeRect.bottom += y;
if (fNoOverlap)
	{
	for (sy = StrokeRect.top; sy <= StrokeRect.bottom; ++sy)
		{
		PixmapCheck(lpOrigDstPixmap, PMT_EDIT, StrokeRect.left, sy, YES);
		if ((lp = PixmapPtr(lpDstPixmap, PMT_EDIT, StrokeRect.left, sy, YES, width)))
			CopyFromHuge(hpData, lp, dwBytes);
		hpData += bwidth;
		}
	}
else
	{
	for (sy = StrokeRect.top; sy <= StrokeRect.bottom; ++sy)
		{
		if ((lp = PixmapPtr(lpDstPixmap, PMT_EDIT, StrokeRect.left, sy, YES, width)))
			{
			if (UsingShield)
				(*lpHugeShieldProc)(hpMsk, lp, width, BrushData.BltFrameTypeInfo);
			(*lpHugeProcessProc)(lp, hpData, hpMsk, width);
			}
		if (lpBrush->SourceData == SD_LAST_DEST)
			{
			CopyFromHuge(lp, hpLast, dwBytes);
  			hpLast += bwidth;
			}
		hpMsk += dh;
		hpData += bwidth;
		}
	}
if( fUsePressure && fUsePolling )    
    GetPacketsFromQueue();
else
    GetMouseMoves();

PixmapAddUndoArea(lpOrigDstPixmap, &StrokeRect);
if (fMaskEditing && lpMask)
	AstralUnionRect(&lpMask->rMask, &lpMask->rMask, &StrokeRect);
AstralUnionRect(&rEdited, &rEdited, &StrokeRect);
if (fUpdateDisplay)
	BltToDisplay(&StrokeRect);
}

/************************************************************************/
static void BltToDisplay(LPRECT lpRect)
/************************************************************************/
{
RECT StrokeRect, DispRect;
BOOL fTurnOn;

StrokeRect = *lpRect;
if (lpUpdateProc)
	{
	(*lpUpdateProc)(hWnd, &StrokeRect, NO);
	}
else
if (fUpdateDisplay)
	{
	OffsetRect(&StrokeRect, xDstOffset, yDstOffset);
	AstralUnionRect(&rUpdate, &rUpdate, &StrokeRect);
	File2DispRect(hWnd, &StrokeRect, &DispRect);
	if (fTurnOn = (Window.hCursor == Window.hNullCursor))
		fTurnOn = DisplayBrushEx(hWnd, 0, 0, OFF, &DispRect);
	lpImage->PaintRaster(GetDisplayPtr(hWnd), Window.hDC, &DispRect, NULL);
	if (fUpdateMarquee)
		UpdateMarquee(hWnd, &StrokeRect, NO);
	if (fTurnOn)
		DisplayBrushEx(hWnd, brush_x, brush_y, ON, NULL);
	}
}

/************************************************************************/
static void ProcessHugeData8(HPTR lpDst, HPTR lpSrc, HPTR lpMsk, DWORD dwCount)
/************************************************************************/
{
#ifdef WIN32
	(*lpProcessProc)(lpDst, lpSrc, lpMsk, dwCount);
#else
	DWORD dwLeft, dwMskLeft;

	// get current position in segment
	while (dwCount)
	{
		// get how many bytes left in segment
		dwLeft = 65536L - ((DWORD)lpSrc & 0xFFFFL);
		dwMskLeft = 65536L - ((DWORD)lpMsk & 0xFFFFL);
		if (dwMskLeft < dwLeft)
			dwLeft = dwMskLeft;
		// see if we will cross a segment boundary
		if (dwLeft > INT_MAX)
			dwLeft = INT_MAX; // never try to copy more than maxint bytes
		if (dwLeft > dwCount)
			dwLeft = dwCount;
		(*lpProcessProc)(lpDst, lpSrc, lpMsk, (int)dwLeft);
		dwCount -= dwLeft;
		if (!dwCount)
			break;
		lpMsk += dwLeft;
		lpDst += dwLeft;
		lpSrc += dwLeft;
	}
#endif
}

/************************************************************************/
static void ProcessHugeData24(HPTR lpDst, HPTR lpSrc, HPTR lpMsk, DWORD dwCount)
/************************************************************************/
{
#ifdef WIN32
	(*lpProcessProc)(lpDst, lpSrc, lpMsk, dwCount);
#else
// It is assumed that lpSrc and lpDst are allocated pointers
// That they both point to the beginning of an allocated block
// i.e. - zero offset
	DWORD dwLeft, dwMskLeft;
	RGBS	srcRGB, dstRGB;

	// get current position in segment
	while (dwCount)
	{
		// get how many bytes left in segment
		dwLeft = 65536L - ((DWORD)lpSrc & 0xFFFFL);
		dwLeft /= 3;
		if (!dwLeft) // RGB spans segment boundary
		{
			srcRGB.red = *lpSrc++;
			srcRGB.green = *lpSrc++;
			srcRGB.blue = *lpSrc++;
			dstRGB.red = *lpDst++;
			dstRGB.green = *lpDst++;
			dstRGB.blue = *lpDst++;
			(*lpProcessProc)((LPTR)&dstRGB, (LPTR)&srcRGB, lpMsk, 1);
			++lpMsk;
			--dwCount;
		}
		else
		{
			dwMskLeft = 65536L - ((DWORD)lpMsk & 0xFFFFL);
			if (dwMskLeft < dwLeft)
				dwLeft = dwMskLeft;
			// see if we will cross a segment boundary
			if (dwLeft > INT_MAX)
				dwLeft = INT_MAX; // never try to copy more than maxint bytes
			if (dwLeft > dwCount)
				dwLeft = dwCount;
			(*lpProcessProc)(lpDst, lpSrc, lpMsk, (int)dwLeft);
			dwCount -= dwLeft;
			if (!dwCount)
				break;
			lpMsk += dwLeft;
			dwLeft *= 3;
			lpDst += dwLeft;
			lpSrc += dwLeft;
		}
	}
#endif
}

/************************************************************************/
static void ProcessHugeData32(HPTR lpDst, HPTR lpSrc, HPTR lpMsk, DWORD dwCount)
/************************************************************************/
{
#ifdef WIN32
	(*lpProcessProc)(lpDst, lpSrc, lpMsk, dwCount);
#else
// It is assumed that lpSrc and lpDst are allocated pointers
// That they both point to the beginning of an allocated block
// i.e. - zero offset
	DWORD dwLeft, dwMskLeft;

	// get current position in segment
	while (dwCount)
	{
		// get how many bytes left in segment
		dwLeft = 65536L - ((DWORD)lpSrc & 0xFFFFL);
		dwLeft /= 4;
		dwMskLeft = 65536L - ((DWORD)lpMsk & 0xFFFFL);
		if (dwMskLeft < dwLeft)
			dwLeft = dwMskLeft;
		// see if we will cross a segment boundary
		if (dwLeft > INT_MAX)
			dwLeft = INT_MAX; // never try to copy more than maxint bytes
		if (dwLeft > dwCount)
			dwLeft = dwCount;
		(*lpProcessProc)(lpDst, lpSrc, lpMsk, (int)dwLeft);
		dwCount -= dwLeft;
		if (!dwCount)
			break;
		lpMsk += dwLeft;
		dwLeft *= 4;
		lpDst += dwLeft;
		lpSrc += dwLeft;
	}
#endif
}

/************************************************************************/
static void SetupOverlap()
/************************************************************************/
{
fNoOverlap = lpBrush->NoOverlap;
// certain brush functions must have overap setup particular ways
if (lpBrush->SourceFunction == SF_SMEAR_IMAGE)
	fNoOverlap = NO;
else
if (lpBrush->SourceFunction == SF_LIGHTEN_IMAGE ||
	lpBrush->SourceFunction == SF_DARKEN_IMAGE ||
	lpBrush->SourceFunction == SF_SHARPEN_IMAGE ||
	lpBrush->SourceFunction == SF_SMOOTH_IMAGE)
	fNoOverlap = YES;
}

/************************************************************************/
static void SetupDataProcs(FRMDATATYPE Type)
/************************************************************************/
{
if (Type == FDT_RGBCOLOR)
	{
#ifdef WIN32
	lpHugeShieldProc = ( void ( * )( HPTR, HPTR, DWORD, FRMTYPEINFO ) )Shield24;
#else
	lpHugeShieldProc = ( void ( * )( HPTR, HPTR, DWORD, FRMTYPEINFO ) )Shield24Huge;
#endif
	lpHugeProcessProc = ProcessHugeData24;
	}
else
if (Type == FDT_CMYKCOLOR)
	{
#ifdef WIN32
	lpHugeShieldProc = ( void ( * )( HPTR, HPTR, DWORD, FRMTYPEINFO ) )Shield32;
#else
	lpHugeShieldProc = ( void ( * )( HPTR, HPTR, DWORD, FRMTYPEINFO ) )Shield32Huge;
#endif
	lpHugeProcessProc = ProcessHugeData32;
	}
else
if (Type == FDT_PALETTECOLOR)
	{
#ifdef WIN32
	lpHugeShieldProc = ( void ( * )( HPTR, HPTR, DWORD, FRMTYPEINFO ) )Shield8P;
#else
	lpHugeShieldProc = ( void ( * )( HPTR, HPTR, DWORD, FRMTYPEINFO ) )Shield8P;
#endif
	lpHugeProcessProc = ProcessHugeData8;
	}
else
	{
#ifdef WIN32
	lpHugeShieldProc = ( void ( * )( HPTR, HPTR, DWORD, FRMTYPEINFO ) )Shield8;
#else
	lpHugeShieldProc = ( void ( * )( HPTR, HPTR, DWORD, FRMTYPEINFO ) )Shield8Huge;
#endif
	lpHugeProcessProc = ProcessHugeData8;
	}
lpProcessProc = GetProcessProc(lpBrush->MergeMode, Type);
#ifdef WIN32
   lpHugeProcessProc = (void (FAR *)( HPTR, HPTR, HPTR, DWORD))lpProcessProc;
#endif
}

/************************************************************************/
static void SetupBrushProcs(LPMGXBRUSH lpBrush, BOOL fBegin)
/************************************************************************/
{
if (!fBegin)
	{
	(*lpCreateBrushSource)(	0, 0, 0, 0, STATE_END);
	return;
	}
if (lpBrush->SourceFlipMode == SFM_MIRROR)
	lpGetSourceData = LoadMirroredData;
else if (lpBrush->SourceFlipMode == SFM_TILE)
	lpGetSourceData = LoadTiledData;
else
	lpGetSourceData = LoadSourceData;

if (lpBrush->SourceFunction == SF_COPY_IMAGE)
	lpCreateBrushSource = CopyImageSource;
else if (lpBrush->SourceFunction == SF_SHARPEN_IMAGE)
	lpCreateBrushSource = SharpenImageSource;
else if (lpBrush->SourceFunction == SF_SMOOTH_IMAGE)
	lpCreateBrushSource = SmoothImageSource;
else if (lpBrush->SourceFunction == SF_LIGHTEN_IMAGE)
	lpCreateBrushSource = LightenImageSource;
else if (lpBrush->SourceFunction == SF_DARKEN_IMAGE)
	lpCreateBrushSource = DarkenImageSource;
else if (lpBrush->SourceFunction == SF_SMEAR_IMAGE)
	lpCreateBrushSource = SmearImageSource;
else
	lpCreateBrushSource = SolidColorSource;
(*lpCreateBrushSource)(	0, 0, 0, 0, STATE_BEGIN);

if (lpBrush->StrokeMethod == SM_CONNECTED)
	StrokeProc = NewDoDDABrushStroke;
else
	StrokeProc = DoBrushStroke;
if (lpBrush->Style == IDC_BRUSHSCATTER)
#ifdef WIN32
	StylizeProc = ScatterMask;
#else
	StylizeProc = ( void ( * )( HPTR, HPTR, LPINT16, int, int, int ))ScatterMaskHuge;
#endif
else
	StylizeProc = NULL;
}

/************************************************************************/
static BOOL SetupBrush(LPMGXBRUSH lpBrush)
/************************************************************************/
{
    BOOL fSetupOpacity = FALSE;

    // if there is no brush mask, allocate one
    if( !lpBrush->BrushMask )
    {
   	    lpBrush->BrushMask = Alloc((long)BrushSize*(long)BrushSize);
	    if (!lpBrush->BrushMask)
	    {
		    Message(IDS_EMEMALLOC);
		    return(FALSE);
	    }
        BrushFull = 0;
    }

    // if brush size changes, setup brush mask
    if (lpBrush->Size != BrushFull)
    {
        SetupBrushMask(lpBrush, BrushData.BltFrameTypeInfo.DataType);
        BrushFull = lpBrush->Size;
        fSetupOpacity = TRUE; // we must reset opacity
    }

    // setup some variables dependent on brush size
    BrushHalf = BrushFull / 2;
    Spacing = (((WORD)lpBrush->Spacing * (WORD)BrushFull) + 50)/100;
	if (!Spacing)
	 	Spacing = 1;

	SetupBrushOpacity(fSetupOpacity);	
	return(TRUE);
}

/************************************************************************/
static void SetupBrushOpacity(BOOL fForceSetup)
/************************************************************************/
{
	WORD wNewOpacity;

    // if brush opacity changes, calc new opacity value
	if (BrushData.BltFrameTypeInfo.DataType == FDT_LINEART ||
 		BrushData.BltFrameTypeInfo.DataType == FDT_PALETTECOLOR)
 		wNewOpacity = 255;
	else
		wNewOpacity = ((lpBrush->Opacity * lpBrush->OpacityScale) + 50) / 100;
	if (wNewOpacity != LastOpacity || fForceSetup)
	{
		LastOpacity = wNewOpacity;
		SetBrushOpacity(lpBrush->BrushMask, lpBM, LastOpacity,
				        (DWORD)BrushFull*(DWORD)BrushFull);
	}
}

/************************************************************************/
static void SetBrushOpacity(HPTR lpSrc, HPTR lpDst, WORD Opacity,
			DWORD dwCount)
/************************************************************************/
{
#ifdef WIN32
if (Opacity == 255)
	hcopy(lpSrc, lpDst, dwCount);
else
	{
	while (dwCount--)
		*lpDst++ = ((WORD)*lpSrc++ * Opacity)/255;
	}
#else
// It is assumed that lpSrc and lpDst are allocated pointers
// That they both point to the beginning of an allocated block
// i.e. - zero offset
if (Opacity == 255)
	hcopy(lpSrc, lpDst, dwCount);
else
	{
	int iCount;
	HPTR hpSrc = lpSrc;
	HPTR hpDst = lpDst;

	while (dwCount)
		{
		if (dwCount >= 16384) // 16K chunks are the neatest
			iCount = 16384;
		else
			iCount = (int)dwCount;
		SetBrushOpacitySeg(hpSrc, hpDst, Opacity, iCount);
		dwCount -= (DWORD)iCount;
		hpSrc += iCount;
		hpDst += iCount;
		}
	}
#endif
}

/************************************************************************/
static void SetBrushOpacitySeg(LPTR lpSrc, LPTR lpDst, WORD Opacity,
			int iCount)
/************************************************************************/
{
#ifdef C_CODE
while (--iCount >= 0)
	*lpDst++ = ((WORD)*lpSrc++ * Opacity)/255;
#else
__asm	{
	push	ds
	push	si
	push	di
	pushf
	lds	si, lpSrc
	les	di, lpDst
	mov	cx, iCount
	or	cx,cx
	je	SBO_END
	mov	bx,Opacity
	cld
	cmp	bx,127
	jbe	SBO_1
	inc	bx
SBO_1:
	lodsb
	sub	ah,ah
	mul	bx
	mov	es:[di],ah
	inc	di
	loop	SBO_1
SBO_END:
	popf
	pop	di
	pop	si
	pop	ds
	}
#endif
}

/************************************************************************/
static void SetupBrushTimer(LPMGXBRUSH lpBrush, BOOL fBegin)
/************************************************************************/
{
dwBrushTimer = lpBrush->Timer;
}

#ifdef UNUSED
/************************************************************************/
static void SetupBrushTimer(LPMGXBRUSH lpBrush, BOOL fBegin)
/************************************************************************/
{
static THREAD_BASE	TIMERPROC lpTimerProc;
int	interval;

if (lpBrush->Timer <= 0)
	return;
if (fBegin)
	{
	interval = lpBrush->Timer/4;
	if (!interval)
		interval = 1;
	lpTimerProc = MakeProcInstance((FARPROC)BrushTimerProc, PictPubApp.Get_hInstAstral());
	idTimer = SetTimer(NULL, 0, interval, lpTimerProc);
	}
else
	{
	if (idTimer)
		KillTimer(NULL, idTimer);
	idTimer = 0;
	FreeProcInstance(lpTimerProc);
	}
}

/***********************************************************************/
void CALLBACK BrushTimerProc( HWND hWindow, UINT message, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
int	SaveOpacity, SavePressure;
LFIXED	SaveFlowRate;

if (!lpBrush)
	return;
if (fFirstStroke)
	return;

if ((lParam - lLastStrokeTime) >= lpBrush->Timer)
	{
	fFirstStroke = YES;
	SaveOpacity = lpBrush->Opacity;
	SavePressure = lpBrush->StylePressure;
	SaveFlowRate = lpBrush->MaxFlowRate;
	lpBrush->Opacity = Opacity;
	lpBrush->StylePressure = StylePressure;
	lpBrush->MaxFlowRate = 0L;
	FadeOpacity = 0L;
	SetupBrush(lpBrush);
	MgxBrushStroke(start_dx+sx_delta, start_dy+sy_delta, start_dx, start_dy,
		brush_x, brush_y);
	lpBrush->MaxFlowRate = SaveFlowRate;
	lpBrush->Opacity = SaveOpacity;
	lpBrush->StylePressure = SavePressure;
	SetupBrush(lpBrush);
	}
}
#endif

/************************************************************************/
static BOOL AllocBrushBuffers(LPMGXBRUSH lpBrush)
/************************************************************************/
{
long	lSize;

// "NULL" out all buffers
lpColor = NULL;	 	    // max - 3MB
lpData = NULL;		 	// max - 3BM
lpDstData = NULL;		// max - 3MB - only used for non-overlap brushes
lpMsk = NULL;			// max - 1MB
lpErr = NULL;			// max - 2MB
lpBM = NULL;			// max - 1MB

// Calculate size of buffers to be used
// double-wide strips seem to be fastest for the most brush sizes
if (fNoOverlap)
	{
	if (lpBrush->Size <= 30)
		bh = lpBrush->Size * 2;
	else if (lpBrush->Size <= 99)
		bh = lpBrush->Size + (lpBrush->Size/2);
	else
		bh = lpBrush->Size;
	}
else
	bh = lpBrush->Size;

if (bh < 10)
	bh = 10;
bv = bh;

bcount = (DWORD)bh * (DWORD)bv;

// Allocate memory for buffers used for all style brushes
lSize = iBltFrameDepth;
if (lSize == 0) lSize = 1;
lSize *= bcount;
if (!(lpColor = Alloc( lSize )))
	goto MemError;
if (!(lpMsk = Alloc( (long)bcount)))
	goto MemError;

// Allocate a source data buffer if there is a source frame
if (lpSrcPixmap || (lpBrush->SourceData != SD_NONE))
	if (!(lpData = Alloc( lSize )))
		goto MemError;

if (fNoOverlap)
	if (!(lpDstData = Alloc( lSize )))
		goto MemError;

// Allocate an error buffer for scatter style
if (lpBrush->Style == IDC_BRUSHSCATTER &&
	!(lpErr = (LPINT16)Alloc((long)(bh+1)*3L*2L)))
	goto MemError;

// allocate brush mask buffer, so opacity of brush can change
// without having to regenerate the brush mask
if (!(lpBM = Alloc( (long)BrushSize*(long)BrushSize )))
	goto MemError;

if (!lpMoveBuf)
	goto MemError;
lpMoveIn = lpMoveBuf;
lpMoveOut = lpMoveBuf;

return(TRUE);

MemError:
Message(IDS_EMEMALLOC);
// freeup buffers
FreeUpBrushBuffers();
return(FALSE);
}

/************************************************************************/
static void FreeUpBrushBuffers()
/************************************************************************/
{
FreeUp( lpColor );
FreeUp( lpData );
FreeUp( lpDstData );
FreeUp( lpMsk );
FreeUp((LPTR)lpErr);
FreeUp( lpBM );
}

/************************************************************************/
static void SetupFade(LPMGXBRUSH lpBrush)
/************************************************************************/
{
int		Delta;

// Setup the Fade In/Out values
if ( lpBrush->Shape == IDC_BRUSHCUSTOM || lpBrush->FadeSize == 0)
	FadeOut = 0L;
else	{
	Delta = ( lpBrush->FadeSize > 0 ? lpBrush->Size :
		  MAX_BRUSH_SIZE - lpBrush->Size + 1 );
	FadeOut = FGET( Delta, (100-abs(lpBrush->FadeSize))*FADE_LEN_MULT);
	if (lpBrush->FadeSize < 0)
		FadeOut = -FadeOut;
	}
// Setup values for fading brush size
BrushSize = lpBrush->Size;
BrushFull = BrushSize;              // new
FadeBrushSize = FGET(BrushSize, 1);

// Setup values for fading opacity
Opacity = lpBrush->Opacity;
LastOpacity = 0xFFFF;      // new
FadeOpacityValue = FGET(Opacity, 1);
FadeOpacity = 0L;

MaxOpacity = ((lpBrush->Opacity * lpBrush->OpacityScale) + 50) / 100;

// Setup pressure values
StylePressure = lpBrush->StylePressure;
}

/************************************************************************/
static void ResetBrush(LPMGXBRUSH lpBrush, BOOL fNoBrushDisplay)
/************************************************************************/
{
if (( FadeOut != 0) || (FadeOpacity != 0) || (fUsePressure != 0) ||
      (lpBrush->MaxFlowRate != 0))
	{
	if (fUpdateDisplay && !fNoBrushDisplay)
		DisplayBrush(0, 0, 0, OFF);
	lpBrush->Size = BrushSize;
	lpBrush->Opacity = Opacity;
	lpBrush->StylePressure = StylePressure;
	SetupBrush(lpBrush);
	if (fUpdateDisplay && !fNoBrushDisplay)
		DisplayBrush(hWnd, brush_x, brush_y, ON);
	}
}

/************************************************************************/
static void SetupMaskState()
/************************************************************************/
{
fMaskEditing = FALSE;
if (fNotAnImage)
	{
	lpMask = NULL;
	UsingMask = UsingShield = NO;
	fUpdateMarquee = NO;
	}
else
	{
	if (lpMask = ImgGetMask(lpImage))
		fMaskEditing = PixmapFrame(lpDstPixmap, PMT_EDIT) == PixmapFrame(&lpMask->Pixmap, PMT_EDIT);
	if (fMaskEditing)
		{
		UsingMask = UsingShield = FALSE;
		fUpdateMarquee = ImgGetMarqueeMask(lpImage) != NULL;
		}
	else
		{
		UsingMask = (lpMask != NULL);
		UsingShield = (ColorMask.Mask && ColorMask.On);
 		fUpdateMarquee = NO;
		}
	}
}


/************************************************************************/
static void SetupPressurePen()
/************************************************************************/
{
    fUsePressure = NO;
    if( Control.UseTablet && ( fUsePolling || fUpdateDisplay ))
	    PressureInit();
}

/************************************************************************/
static BOOL SetupFrames(LPMGXBRUSH lpBrush, BOOL fBegin)
/************************************************************************/
{
int	mode;
FRMTYPEINFO DstTypeInfo;

if (fBegin) // called from MgxBrushBegin
	{
	lpDstPixmap = lpOrigDstPixmap;
	mode = RELEASE_OLDEST; // faster mode for retouching
	PixmapGetInfo(lpDstPixmap, PMT_EDIT, &iBltFrameXsize, &iBltFrameYsize,
					&iBltFrameDepth, NULL);

	FrameGetTypeInfo(lpDstPixmap->EditFrame, &BrushData.BltFrameTypeInfo);
	DstTypeInfo = BrushData.BltFrameTypeInfo;
	if (DstTypeInfo.DataType == FDT_LINEART)
		DstTypeInfo.DataType = FDT_GRAYSCALE;

	if (lpImage)
		iBltFrameRes = FrameResolution(ImgGetBaseEditFrame(lpImage));
	else
		iBltFrameRes = FrameResolution(lpDstPixmap->EditFrame);
	BrushData.pmDstType = PMT_EDIT;
	lpMskFrame = NULL;
	fCreatedUndo = NO;
	if (fNoOverlap)
		{
		lpMskFrame = FrameOpen(FDT_GRAYSCALE, iBltFrameXsize, iBltFrameYsize, 100);
		if (!lpMskFrame)
			{
			Message(IDS_EMEMALLOC);
			return(FALSE);
			}
		FrameSetBackground(lpMskFrame, 0);

		lpMskPixmap = &mskPixmap;
		PixmapSetup(lpMskPixmap, lpMskFrame, YES);
		BrushData.pmMskType = PMT_EDIT;

		if (Control.UseApply || !lpDstPixmap->UndoFrame ||
			lpBrush->SourceData == SD_CURRENT_UNDO)
			{
			PixmapSetup(&dstPixmap, lpDstPixmap->EditFrame, NO);
			if (!PixmapInitUndo(&dstPixmap))
				{
				FrameClose(lpMskFrame);
				lpMskFrame = NULL;
				Message(IDS_EMEMALLOC);
				return(FALSE);
				}
			fCreatedUndo = YES;
			lpDstPixmap = &dstPixmap;
			}
		BrushData.pmDstType = PMT_UNDO;
		}
	lpSrcPixmap = NULL;
	if (lpBrush->SourceData == SD_FILE)
		{
		if (lpBrush->SourceFrame)
			{
			FRMTYPEINFO SrcTypeInfo;

			FrameGetTypeInfo(lpBrush->SourceFrame, &SrcTypeInfo);
			if (!FrameTypeInfoEqual(SrcTypeInfo, DstTypeInfo))
				{
				FrameClose(lpBrush->SourceFrame);
				lpBrush->SourceFrame = NULL;
				}
			}
		if (!lpBrush->SourceFrame)
			{
			if (lpBrush->SourceFrame)
				FrameClose(lpBrush->SourceFrame);

			CReadImage read(lpBrush->szSource);
			if (!(lpBrush->SourceFrame = read.ReadFrame(BrushData.BltFrameTypeInfo) ))
				{
				if (fCreatedUndo)
					PixmapFreeUndo(lpDstPixmap);
				if (lpMskFrame)
					{
					FrameClose(lpMskFrame);
					lpMskFrame = NULL;
					}
				Message(IDS_BRUSHINVALID, (LPTR)lpBrush->szSource);
				return(FALSE);
				}
			}
		lpSrcPixmap = &srcPixmap;
		PixmapSetup(lpSrcPixmap, lpBrush->SourceFrame, YES);
		BrushData.pmSrcType = PMT_EDIT;
		}
	else if (lpBrush->SourceFrame)
		{
		lpSrcPixmap = &srcPixmap;
		PixmapSetup(lpSrcPixmap, lpBrush->SourceFrame, YES);
		BrushData.pmSrcType = PMT_EDIT;
		xSrcOffset = lpBrush->xSrcOffset;
		ySrcOffset = lpBrush->ySrcOffset;
		}
	else if (lpBrush->SourceData == SD_CURRENT_UNDO)
		{
		if (!lpDstPixmap->UndoFrame || lpDstPixmap->fNewFrame)
			{
			if (fCreatedUndo)
				PixmapFreeUndo(lpDstPixmap);
			if (lpMskFrame)
				{
				FrameClose(lpMskFrame);
				lpMskFrame = NULL;
				}
			Message(IDS_BRUSHNOUNDO);
			return(FALSE);
			}
		lpSrcPixmap = lpOrigDstPixmap;
		BrushData.pmSrcType = PMT_UNDO;
		}
	else if (lpBrush->SourceData == SD_CURRENT_DEST)
		{
		lpSrcPixmap = lpDstPixmap;
		BrushData.pmSrcType = BrushData.pmDstType;
		}
//	if (lpBrush->SurfaceFrame)
//		{
//		lpSurfacePixmap = &surfacePixmap;
//		PixmapSetup(lpSurfacePixmap, lpBrush->SurfaceFrame, YES);
//		}
//	else
//		lpSurfacePixmap = NULL;
	if (lpSrcPixmap)
		iSrcFrameRes = FrameResolution(lpSrcPixmap->EditFrame);
	}
else // called from MgxBrushEnd
	{
	if (lpMskFrame)
		FrameClose(lpMskFrame);
	if (fCreatedUndo)
		{
		PixmapFreeUndo(lpDstPixmap);
		BrushData.pmDstType = PMT_EDIT;
		}
	mode = RELEASE_NEWEST; // normal mode
	}

// Set mode for Blt frame
FrameMode(PixmapFrame(lpDstPixmap, PMT_EDIT), mode);

// Set mode for Source frame
if (lpSrcPixmap)
	FrameMode(PixmapFrame(lpSrcPixmap, BrushData.pmSrcType), mode);

// Set mode for Destination frame
if (BrushData.pmDstType != PMT_EDIT)
	FrameMode(PixmapFrame(lpDstPixmap, BrushData.pmDstType), mode);

// Set mode for surface texture
//if (lpSurfacePixmap)
//	FrameMode(PixmapFrame(lpSurfacePixmap, PMT_EDIT), mode);

return(TRUE);
}

/************************************************************************/
void FadeBrush()
/************************************************************************/
{
if (FadeOut != 0)
	{
	lpBrush->Size = FMUL(1, FadeBrushSize);
	lpBrush->Size = bound(lpBrush->Size, 0, BrushSize);
	SetupBrush(lpBrush);
	}
if (FadeOpacity != 0)
	{
	lpBrush->Opacity = FMUL(1, FadeOpacityValue);
	lpBrush->Opacity = bound(lpBrush->Opacity, 0, Opacity);
	SetupBrushOpacity(FALSE);
	}
}

/************************************************************************/
void FadeOutBrush()
/************************************************************************/
{
if (FadeOut || FadeOpacity)
	{
	if (FadeOut != 0)
		FadeBrushSize -= FadeOut;
	if (FadeOpacity)
		FadeOpacityValue -= FadeOpacity;
	if (FadeBrushSize > 0)
		FadeBrush();
	}
}

/***********************************************************************/
void PressureInit()
/***********************************************************************/
{
    LPTABLETDATA lpPacket;


    // Check to see if tablet context was created successfully. 
    // If not, bail out.
    fUsePressure = TabletInit();

    if( !fUsePressure )
        return; 

    fUsePolling = NO;   // remove this when polling with tablets works

    // Get pressure from next packet in queue to determine start brush size. 
    // Leave packet in queue until ready to begin drawing.
    lpPacket = GetNextPacket( NO );  
    PenPressure = lpPacket->Pressure;

    fPressureSize = (( Control.UseTablet & 2 ) != 0 ) && FadeOut == 0L;
    fPressureOpacity = (( Control.UseTablet & 1 ) != 0 );
    fUsePressure = fUsePressure && ( fPressureSize || fPressureOpacity );

    if( fUsePressure )
    { 
	    if( fPressureSize )
	    {
		    lpBrush->Size = ( PenPressure * BrushSize ) / 100;
		    FadeBrushSize = FGET( lpBrush->Size, 1 );
	    }
	    if( fPressureOpacity )
	    {
		    lpBrush->Opacity = ( PenPressure * Opacity ) / 100;
		    FadeOpacityValue = FGET( lpBrush->Opacity, 1 );
	    }
	    SetupBrush( lpBrush );
    }
}

/***********************************************************************/
void PressureSetup( int delta )
/***********************************************************************/
{
int iPressure, iNewSize, iNewOpacity;

if (!fUsePressure)
	return;
if (fPressureSize)
	FadeOut = 0;
if (fPressureOpacity)
	FadeOpacity = 0;
if (!delta)
	return;
iPressure = PenPressure;
if (fPressureSize)
	{
	iNewSize = (iPressure * BrushSize) / 100;
	if (iNewSize != lpBrush->Size)
		FadeOut = FGET(lpBrush->Size-iNewSize, delta);
	}
if (fPressureOpacity)
	{
	iNewOpacity = (iPressure * Opacity) / 100;
//	dbg("iPressure = %d Opacity = %d iNewOpacity = %d",
//		iPressure, Opacity, iNewOpacity);
	if (iNewOpacity != lpBrush->Opacity)
		FadeOpacity = FGET(lpBrush->Opacity-iNewOpacity, delta);
	}
}

/************************************************************************/
BOOL MgxOSBrushBegin(HMGXBRUSH hBrush, LPRECT lpStrokeRect, LPRECT lpMaxRect)
/************************************************************************/
{
    // Call End function to make sure things get reset
    MgxOSBrushEnd(NULL);

    // Convert brush handle to brush pointer for our use
    lpBrush = (LPMGXBRUSH)hBrush;

    // Set frame mode to free oldest lines first instead of newest
    // Assumption is that your typically brushing in the same area
    if (!SetupOSFrame(YES, lpStrokeRect, lpMaxRect))
    {
	    lpBrush = NULL;
	    return(FALSE);
    }

    // Setup Fade for brush size and brush opacity
    SetupFade(lpBrush);

    // Allocate all memory needed for brushing
    if (!AllocOSBrushBuffers(lpBrush))
    {
	    lpBrush = NULL;
	    return(FALSE);
    }

    // setup brush dependent procs
    SetupOSBrushProcs(lpBrush);

    // Initialize pressure sensitive pen settings
    fUsePressure = NO;

    // Setup the brush mask
    SetupBrush(lpBrush);

    // Indicate the first stroke is coming
    fFirstStroke = YES;

    return(TRUE);
}

/************************************************************************/
//	Call to pickup the brush
/************************************************************************/
void MgxOSPickupBrush(void)
{
	fFirstStroke = YES;
}

/************************************************************************/
LPFRAME MgxOSBrushEnd(LPRECT lpRect)
/************************************************************************/
{
if (!lpBrush)
	return(NULL);

ResetBrush(lpBrush, YES);
FreeUpOSBrushBuffers();
SetupOSFrame(NO, NULL, NULL);
lpBrush = NULL;
if (lpRect)
	{
	OffsetRect(&rOffScreen, xDstOffset, yDstOffset);
	*lpRect = rOffScreen;
	}
else
	{
	if (lpMskFrame)
		{
		FrameClose(lpMskFrame);
		lpMskFrame = NULL;
		}
	}
return(lpMskFrame);
}

/************************************************************************/
void MgxOSBrushStroke(int dx, int dy)
/************************************************************************/
{
if (!lpBrush)
	return;

dx -= xDstOffset;
dy -= yDstOffset;
(*OSStrokeProc)(dx, dy);
}

/************************************************************************/
static BOOL SetupOSFrame(BOOL fBegin, LPRECT lpStrokeRect, LPRECT lpMaxRect)
/************************************************************************/
{
int	mode;

if (fBegin) // called from MgxBrushBegin
	{
	rOffScreen = *lpStrokeRect;
	if (lpBrush->FadeSize)
		InflateRect(&rOffScreen, MAX_BRUSH_SIZE, MAX_BRUSH_SIZE);
	else
		InflateRect(&rOffScreen, lpBrush->Size/2, lpBrush->Size/2);
	BoundRect(&rOffScreen,	lpMaxRect->left, lpMaxRect->top,
							lpMaxRect->right, lpMaxRect->bottom);

	xDstOffset = rOffScreen.left;
	yDstOffset = rOffScreen.top;
	OffsetRect(&rOffScreen, -xDstOffset, -yDstOffset);
	lpMskFrame = FrameOpen(FDT_GRAYSCALE, RectWidth(&rOffScreen),
							RectHeight(&rOffScreen), 100);
	if (!lpMskFrame)
		return(FALSE);

	FrameGetTypeInfo(lpMskFrame, &BrushData.BltFrameTypeInfo);
	iBltFrameDepth = FrameDepth(lpMskFrame);
	if (!iBltFrameDepth)
		iBltFrameDepth = 1;
	FrameSetBackground(lpMskFrame, 0);
	mode = RELEASE_OLDEST; // faster mode for retouching
	}
else // called from MgxBrushEnd
	{
	mode = RELEASE_NEWEST; // normal mode
	}

// Set mode for Blt frame
FrameMode(lpMskFrame, mode);

return(TRUE);
}

/************************************************************************/
static BOOL AllocOSBrushBuffers(LPMGXBRUSH lpBrush)
/************************************************************************/
{
if (!(lpBM = Alloc( (long)lpBrush->Size*(long)lpBrush->Size )))
	return(FALSE);
return(TRUE);
}

/************************************************************************/
static void FreeUpOSBrushBuffers()
/************************************************************************/
{
if (lpBM)
	{
	FreeUp(lpBM);
	lpBM = NULL;
	}
}

/************************************************************************/
static void SetupOSBrushProcs(LPMGXBRUSH lpBrush)
/************************************************************************/
{
if (lpBrush->StrokeMethod == SM_CONNECTED)
	OSStrokeProc = DoDDAOSBrushStroke;
else
	OSStrokeProc = DoOSBrushStroke;
}

/************************************************************************/
static void DoDDAOSBrushStroke(int dx, int dy)
/************************************************************************/
{
// Initialize for first stroke that we get
if (fFirstStroke)
	{
	fFirstStroke = FALSE;
	start_dx = dx;		 // setup initial starting point
	start_dy = dy;
	last_x_processed = INT_MAX;
	last_y_processed = INT_MAX;
	}
else if (dx == start_dx && dy == start_dy)
	return; // point has moved - don't do anything

// setup new ending point - start point was the last point
end_dx = dx;
end_dy = dy;

if (abs(end_dx-start_dx) || abs(end_dy-start_dy))
	ddaline(start_dx, start_dy, end_dx, end_dy, ProcessOSBrushStroke);
else
	ProcessOSBrushStroke(start_dx, start_dy);
start_dx = end_dx;	
start_dy = end_dy;	
}

/************************************************************************/
static void DoOSBrushStroke(int dx, int dy)
/************************************************************************/
{
ProcessOSBrushStroke(dx, dy);
}

/************************************************************************/
static void ProcessOSBrushStroke(int x, int y)
/************************************************************************/
{
RECT bRect, iRect;

if (abs(last_x_processed-x) < Spacing &&
	abs(last_y_processed-y) < Spacing)
	{
	FadeOutBrush();
	return;
	}

last_x_processed = x;
last_y_processed = y;

if (BrushFull)
	{
	// get rectangle this brush stroke encompasses
	bRect.top = y - BrushHalf;
	bRect.bottom = bRect.top + BrushFull - 1;
	bRect.left = x - BrushHalf;
	bRect.right = bRect.left + BrushFull - 1;

	if (AstralIntersectRect(&iRect, &rOffScreen, &bRect))
		OSMergeMask(lpMskFrame, lpBM, &bRect, BrushFull, &iRect, MaxOpacity);
	}

// Adjust fade values
FadeOutBrush();
}

/************************************************************************/
static void OSMergeMask(LPFRAME lpMskFrame, HPTR lpSrc, LPRECT lpSrcRect,
					 int SrcWidth, LPRECT lpSectRect, int iMaxValue)
/************************************************************************/
{
int dx, dy, iWidth, iHeight, x, y;
HPTR lpSrcBuf;
LPTR lpDstBuf;

iWidth = RectWidth(lpSectRect);
iHeight = RectHeight(lpSectRect);
if (!iWidth || !iHeight)
	return;

dy = lpSectRect->top - lpSrcRect->top;
dx = lpSectRect->left - lpSrcRect->left;
lpSrcBuf = lpSrc + ((DWORD)SrcWidth * (DWORD)dy) + (DWORD)dx;

x = lpSectRect->left;
y = lpSectRect->top;
while (--iHeight >= 0)
	{
	if (lpDstBuf = FramePointer(lpMskFrame, x, y, YES, iWidth))
#ifdef WIN32
		OSMergeMaskLine(lpDstBuf, iWidth, lpSrcBuf, iMaxValue);
#else
		OSMergeMaskLineHuge(lpDstBuf, iWidth, lpSrcBuf, iMaxValue);
#endif
	lpSrcBuf += SrcWidth;
	++y;
	}
}

/************************************************************************/
static void OSMergeMaskLineHuge(LPTR lpDst, int iCount, HPTR hpSrc, int iMaxValue)
/************************************************************************/
{
DWORD dwLeft;

while (iCount)
	{
	// get how many bytes left in segment
	dwLeft = 65536L - ((DWORD)hpSrc & 0xFFFFL);
	if (dwLeft > (DWORD)iCount)
		dwLeft = iCount;
	OSMergeMaskLine(lpDst, (int)dwLeft, hpSrc, iMaxValue);
	iCount -= (int)dwLeft;
	if (!iCount)
		break;
	hpSrc += dwLeft;
	lpDst += dwLeft;
	}
}

/************************************************************************/
static void OSMergeMaskLine(LPTR lpDst, int iCount, LPTR lpSrc, int iMaxValue)
/************************************************************************/
{
#ifdef _X86_
__asm
	{
	mov esi,DWORD PTR lpSrc	;lpSrc
	mov	edi,DWORD PTR lpDst	;lpDst
	cld
	mov		ecx,iMaxValue
	jmp		MM_DOWIDTHLOOP

MM_WIDTHLOOP:
	lodsb				; al = new value
	or		al,al		; any value?
	je		MM_NONEWVALUE
	mov		bl,al		; save new value
	mov		dl,cl
	sub		dl,al
	jne		MM_MULOLD
	sub		al,al
	jmp		MM_ADDNEW
MM_MULOLD:
	mov		al,BYTE PTR [edi] ; bl = old value
	or		al,al
	je		MM_ADDNEW
	mul		dl
	mov		dl,cl
	shr		dl,1
	sub		dh,dh
	add		ax,dx
	div		cl
MM_ADDNEW:
	add		al,bl
	jc		MM_CLIPVALUE
	cmp		al,cl
	jbe		MM_STOREBYTE
MM_CLIPVALUE:
	mov		al,cl
MM_STOREBYTE:
	stosb
	jmp		MM_DOWIDTHLOOP
MM_NONEWVALUE:
	inc		edi
MM_DOWIDTHLOOP:
	dec	iCount			; decrement iCount
	jns	MM_WIDTHLOOP
	}
#else
WORD s, d, rnd;

rnd = iMaxValue >> 1;
while (--iCount >= 0)
	{
	s = (WORD)(*lpSrc++);
	d = (WORD)(*lpDst);
	d = ((((iMaxValue - s) * d) + rnd) / iMaxValue) + s;
	*lpDst++ = (BYTE)d;
	}
#endif
}


/************************************************************************/
static BOOL Equal(LPSTR lpString1, int idString2)
/************************************************************************/
{
LPSTR lpString2;

if (!AstralStr(idString2, &lpString2))
	return(FALSE);
return(StringsEqual(lpString1, lpString2));
}

/************************************************************************/
static int GetPPString(LPSTR lpSection, int idEntry, int idDefault, LPSTR lpValue, int nValueLen, LPSTR lpFileName)
/************************************************************************/
{
STRING   szDefault;
LPSTR    lpString;

if (AstralStr(idDefault, &lpString))
	lstrcpy(szDefault, lpString);
else
	lstrcpy(szDefault, "DEFAULT");
if (!AstralStr(idEntry, &lpString))
	{
	lstrcpy(lpValue, szDefault);
	return(0);
	}
return(GetPrivateProfileString(lpSection, lpString, szDefault, lpValue,
						 nValueLen, lpFileName));
}

/************************************************************************/
static BOOL WritePPString(LPSTR lpSection, int idEntry, int idValue,
							LPSTR lpValue, LPSTR lpFileName)
/************************************************************************/
{
STRING   szValue;
LPSTR    lpString;

if (!idValue && !lpValue)
	return(0);

if (idValue)
	{
	if (AstralStr(idValue, &lpString))
		lstrcpy(szValue, lpString);
	else
		lstrcpy(szValue, "DEFAULT");
	lpValue = szValue;
	}
if (!AstralStr(idEntry, &lpString))
	return(0);
return(WritePrivateProfileString(lpSection, lpString, lpValue, lpFileName));
}

/************************************************************************/
static BOOL WritePPInt(LPSTR lpSection, int idEntry, int iValue, LPSTR lpFileName)
/************************************************************************/
{
STRING   szValue;
LPSTR    lpString;

Int2Ascii(szValue, iValue);
if (!AstralStr(idEntry, &lpString))
	return(0);
return(WritePrivateProfileString(lpSection, lpString, szValue, lpFileName));
}

/************************************************************************/
HPTR SolidColorSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
return(lpColor);
}

/************************************************************************/
HPTR CopyImageSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
return(lpData);
}

/************************************************************************/
HPTR SharpenImageSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
int                     rx, iCount;
LPTR                    lpSLine1, lpSLine2, lpSLine3, lpSrcPtr;
LPTR					 lp1, lp2, lp3, lp;
HPTR					 hpSLine1, hpSLine2, hpSLine3, hpSrcPtr;
HPTR					 hpBuf1, hpBuf2, hpBuf3;
BYTE					 buf1[12], buf2[12], buf3[12], buf[4];
DWORD                   bcount, dwSpanCount, dwSrcSpanCount, dwSpanLoop;
RECT                    rBuf;
BOOL					 fSpanSegment;
BYTE					 SpanMask;
static THREAD_BASE	SHARPSMOOTHPROC  lpSharpSmoothProc;
static THREAD_BASE	LPTR             lpBuf;

if (State & (STATE_END|STATE_BEGIN))
	{
	if (lpBuf)
		FreeUp(lpBuf);
	lpBuf = NULL;
	if (State & STATE_BEGIN)
		{
		lpBuf = Alloc((long)((DWORD)bh+2)*(long)((DWORD)bv+2)*(DWORD)iBltFrameDepth);
		switch ( iBltFrameDepth )
			{
			case 3:
				lpSharpSmoothProc = GetSharpSmooth24(IDC_SHARP, lpBrush->Pressure);
			break;

			case 4:
				lpSharpSmoothProc = GetSharpSmooth32(IDC_SHARP, lpBrush->Pressure);
			break;

			case 0:
			case 1:
			default:
				lpSharpSmoothProc = GetSharpSmooth8( IDC_SHARP, lpBrush->Pressure);
			break;
			}
		}
	return(NULL);
	}

if (!lpBuf)
	return(lpData);
if (!LoadSourceData(lpDstPixmap, BrushData.pmDstType, x-1, y-1, dx+2, dy+2, lpBuf,
						&rBuf, NO, NO))
	return(lpData);

bcount = (DWORD)(dx+2) * (DWORD)iBltFrameDepth;
#ifdef WIN32
fSpanSegment = FALSE;
#else
fSpanSegment = (bcount * (DWORD)(dy * 2)) > 65536L;
#endif
if (fSpanSegment) // more than a segment
	{
	hpBuf1 = lpBuf;
	hpBuf2 = hpBuf1 + bcount;
	hpBuf3 = hpBuf2 + bcount;
	dwSpanCount = (dx+2) * iBltFrameDepth;
	dwSrcSpanCount = (dx) * iBltFrameDepth;
	dwSpanLoop = 3 * iBltFrameDepth;
	hpSrcPtr = lpData;
	while (--dy >= 0)
		{
		hpSLine1 = hpBuf1;
		hpSLine2 = hpBuf2;
		hpSLine3 = hpBuf3;
		iCount = dx;
		SpanMask = 0;
		#ifndef WIN32
		if (SPAN64K(hpSrcPtr, dwSrcSpanCount))
			SpanMask |= 1;
		if (SPAN64K(hpSLine1, dwSpanCount))
			SpanMask |= 2;
		if (SPAN64K(hpSLine2, dwSpanCount))
			SpanMask |= 4;
		if (SPAN64K(hpSLine3, dwSpanCount))
			SpanMask |= 8;
		if (SpanMask)
			{
			while (--iCount >= 0)
				{
				if ((SpanMask & 1) && SPAN64K(hpSrcPtr, iBltFrameDepth))
					{
					hcopy(hpSrcPtr, buf, iBltFrameDepth);
					lp = buf;
					}
				else
					lp = hpSrcPtr;
				if ((SpanMask & 2) && SPAN64K(hpSLine1, dwSpanLoop))
					{
					hcopy(hpSLine1, buf1, dwSpanLoop);
					lp1 = buf1;
					}
				else
					lp1 = hpSLine1;
				if ((SpanMask & 4) && SPAN64K(hpSLine2, dwSpanLoop))
					{
					hcopy(hpSLine2, buf2, dwSpanLoop);
					lp2 = buf2;
					}
				else
					lp2 = hpSLine2;
				if ((SpanMask & 8) && SPAN64K(hpSLine3, dwSpanLoop))
					{
					hcopy(hpSLine3, buf3, dwSpanLoop);
					lp3 = buf3;
					}
				else
					lp3 = hpSLine3;
				(*lpSharpSmoothProc)( 1, lp1, lp2, lp3, lp);
				hpSLine1 += iBltFrameDepth;
				hpSLine2 += iBltFrameDepth;
				hpSLine3 += iBltFrameDepth;
				hpSrcPtr += iBltFrameDepth;
				}
			}
		else
		#endif
			{
			while (--iCount >= 0)
				{
				(*lpSharpSmoothProc)( 1, hpSLine1, hpSLine2, hpSLine3, hpSrcPtr);
				hpSLine1 += iBltFrameDepth;
				hpSLine2 += iBltFrameDepth;
				hpSLine3 += iBltFrameDepth;
				hpSrcPtr += iBltFrameDepth;
				}
			}
		hpBuf1 = hpBuf2;
		hpBuf2 = hpBuf3;
		hpBuf3 = hpBuf2 + bcount;
		}
	}
else
	{
	lpSLine1 = lpBuf;
	lpSLine2 = lpSLine1 + bcount;
	lpSLine3 = lpSLine2 + bcount;
	lpSrcPtr = lpData;
	while (--dy >= 0)
		{
		rx = 1;
		iCount = dx;
		while (--iCount >= 0)
			{
			(*lpSharpSmoothProc)( rx, lpSLine1, lpSLine2, lpSLine3, lpSrcPtr);
			++rx;
			lpSrcPtr += iBltFrameDepth;
			}
		lpSLine1 = lpSLine2;
		lpSLine2 = lpSLine3;
		lpSLine3 = lpSLine2 + bcount;
		}
	}
return(lpData);
}

/************************************************************************/
HPTR SmoothImageSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
int                     rx, iCount;
LPTR                    lpSLine1, lpSLine2, lpSLine3, lpSrcPtr;
LPTR					 lp1, lp2, lp3, lp;
HPTR					 hpSLine1, hpSLine2, hpSLine3, hpSrcPtr;
HPTR					 hpBuf1, hpBuf2, hpBuf3;
BYTE					 buf1[12], buf2[12], buf3[12], buf[4];
DWORD                   bcount, dwSpanCount, dwSrcSpanCount, dwSpanLoop;
RECT                    rBuf;
BOOL					 fSpanSegment;
BYTE					 SpanMask;
static THREAD_BASE	SHARPSMOOTHPROC  lpSharpSmoothProc;
static THREAD_BASE	LPTR             lpBuf;

if (State & (STATE_END|STATE_BEGIN))
	{
	if (lpBuf)
		FreeUp(lpBuf);
	lpBuf = NULL;
	if (State & STATE_BEGIN)
		{
		lpBuf = Alloc((long)((DWORD)bh+2)*(long)((DWORD)bv+2)*(DWORD)iBltFrameDepth);
		switch ( iBltFrameDepth )
			{
			case 3:
				lpSharpSmoothProc = GetSharpSmooth24(IDC_SMOOTH, lpBrush->Pressure);
			break;

			case 4:
				lpSharpSmoothProc = GetSharpSmooth32(IDC_SMOOTH, lpBrush->Pressure);
			break;

			case 0:
			case 1:
			default:
				lpSharpSmoothProc = GetSharpSmooth8( IDC_SMOOTH, lpBrush->Pressure);
			break;
			}
		}
	return(NULL);
	}

if (!lpBuf)
	return(lpData);
if (!LoadSourceData(lpDstPixmap, BrushData.pmDstType, x-1, y-1, dx+2, dy+2, lpBuf,
						&rBuf, NO, NO))
	return(lpData);

bcount = (DWORD)(dx+2) * (DWORD)iBltFrameDepth;
#ifdef WIN32
fSpanSegment = FALSE;
#else
fSpanSegment = (bcount * (DWORD)(dy * 2)) > 65536L;
#endif
if (fSpanSegment) // more than a segment
	{
	hpBuf1 = lpBuf;
	hpBuf2 = hpBuf1 + bcount;
	hpBuf3 = hpBuf2 + bcount;
	dwSpanCount = (dx+2) * iBltFrameDepth;
	dwSrcSpanCount = (dx) * iBltFrameDepth;
	dwSpanLoop = 3 * iBltFrameDepth;
	hpSrcPtr = lpData;
	while (--dy >= 0)
		{
		hpSLine1 = hpBuf1;
		hpSLine2 = hpBuf2;
		hpSLine3 = hpBuf3;
		iCount = dx;
		SpanMask = 0;
		#ifndef WIN32
		if (SPAN64K(hpSrcPtr, dwSrcSpanCount))
			SpanMask |= 1;
		if (SPAN64K(hpSLine1, dwSpanCount))
			SpanMask |= 2;
		if (SPAN64K(hpSLine2, dwSpanCount))
			SpanMask |= 4;
		if (SPAN64K(hpSLine3, dwSpanCount))
			SpanMask |= 8;
		if (SpanMask)
			{
			while (--iCount >= 0)
				{
				if ((SpanMask & 1) && SPAN64K(hpSrcPtr, iBltFrameDepth))
					{
					hcopy(hpSrcPtr, buf, iBltFrameDepth);
					lp = buf;
					}
				else
					lp = hpSrcPtr;
				if ((SpanMask & 2) && SPAN64K(hpSLine1, dwSpanLoop))
					{
					hcopy(hpSLine1, buf1, dwSpanLoop);
					lp1 = buf1;
					}
				else
					lp1 = hpSLine1;
				if ((SpanMask & 4) && SPAN64K(hpSLine2, dwSpanLoop))
					{
					hcopy(hpSLine2, buf2, dwSpanLoop);
					lp2 = buf2;
					}
				else
					lp2 = hpSLine2;
				if ((SpanMask & 8) && SPAN64K(hpSLine3, dwSpanLoop))
					{
					hcopy(hpSLine3, buf3, dwSpanLoop);
					lp3 = buf3;
					}
				else
					lp3 = hpSLine3;
				(*lpSharpSmoothProc)( 1, lp1, lp2, lp3, lp);
				hpSLine1 += iBltFrameDepth;
				hpSLine2 += iBltFrameDepth;
				hpSLine3 += iBltFrameDepth;
				hpSrcPtr += iBltFrameDepth;
				}
			}
		else
		#endif
			{
			while (--iCount >= 0)
				{
				(*lpSharpSmoothProc)( 1, hpSLine1, hpSLine2, hpSLine3, hpSrcPtr);
				hpSLine1 += iBltFrameDepth;
				hpSLine2 += iBltFrameDepth;
				hpSLine3 += iBltFrameDepth;
				hpSrcPtr += iBltFrameDepth;
				}
			}
		hpBuf1 = hpBuf2;
		hpBuf2 = hpBuf3;
		hpBuf3 = hpBuf2 + bcount;
		}
	}
else
	{
	lpSLine1 = lpBuf;
	lpSLine2 = lpSLine1 + bcount;
	lpSLine3 = lpSLine2 + bcount;
	lpSrcPtr = lpData;
	while (--dy >= 0)
		{
		rx = 1;
		iCount = dx;
		while (--iCount >= 0)
			{
			(*lpSharpSmoothProc)( rx, lpSLine1, lpSLine2, lpSLine3, lpSrcPtr);
			++rx;
			lpSrcPtr += iBltFrameDepth;
			}
		lpSLine1 = lpSLine2;
		lpSLine2 = lpSLine3;
		lpSLine3 = lpSLine2 + bcount;
		}
	}
return(lpData);
}

/************************************************************************/
HPTR LightenImageSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
	DWORD dwCount, dwBump;
	int iCount;
	int iBand;
	LPTR lpLut;
	HPTR hpSrc, hpDst;

	if (State & (STATE_END|STATE_BEGIN))
		{
		if (State & STATE_BEGIN)
			{
			ResetMap(&Map, MAPPOINTS, NO);
			iBand = bound(lpBrush->Weight, BAND_ALL, BAND_SHADOWS);
			Map.Brightness[iBand] = -TOPERCENT(lpBrush->Pressure);
			MakeMap(&Map);
			}
		return(NULL);
		}

	lpLut = Map.Lut;
	dwCount = (DWORD)dx * (DWORD)dy;
	if (!dwCount)
		return(lpData);

	switch(iBltFrameDepth)
	{
		case 3 :
			dwCount *= 3;
		case 0 :
		case 1 :
#ifdef WIN32
			MapLut(lpDstData, lpData, lpLut, dwCount);
#else
			hpSrc = lpDstData;
			hpDst = lpData;
			while (dwCount)
				{
				if (dwCount >= 16384L)
					iCount = 16384;
				else
					iCount = (int)dwCount;
				MapLut(hpSrc, hpDst, lpLut, iCount);
				dwCount -= (DWORD)iCount;
				hpSrc += iCount;
				hpDst += iCount;
				}
#endif
		break;
		case 4 :
#ifdef WIN32
			MapLut32(lpDstData, lpData, lpLut, (int)dwCount);
#else
			hpSrc = lpDstData;
			hpDst = lpData;
			while (dwCount)
				{
				if (dwCount >= 16384L)
					iCount = 16384;
				else
					iCount = (int)dwCount;
				MapLut32(hpSrc, hpDst, lpLut, iCount);
				dwCount -= (DWORD)iCount;
				hpSrc += ((long)iCount*4);
				hpDst += ((long)iCount*4);
				}
#endif
		break;
	}

	return(lpData);
}

/************************************************************************/
HPTR DarkenImageSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
	DWORD dwCount;
	int iCount;
	int iBand;
	LPTR lpLut;
	HPTR hpSrc, hpDst;

	if (State & (STATE_END|STATE_BEGIN))
		{
		if (State & STATE_BEGIN)
			{
			ResetMap(&Map, MAPPOINTS, NO);
			iBand = bound(lpBrush->Weight, BAND_ALL, BAND_SHADOWS);
			Map.Brightness[iBand] = TOPERCENT(lpBrush->Pressure);
			MakeMap(&Map);
			}
		return(NULL);
		}

	lpLut = Map.Lut;
	dwCount = (DWORD)dx * (DWORD)dy;
	if (!dwCount)
		return(lpData);

	switch(iBltFrameDepth)
	{
		case 3 :
			dwCount *= 3;
		case 0 :
		case 1 :
#ifdef WIN32
         MapLut(lpDstData, lpData, lpLut, dwCount);
#else
			hpSrc = lpDstData;
			hpDst = lpData;
			while (dwCount)
				{
				if (dwCount >= 16384L)
					iCount = 16384;
				else
					iCount = (int)dwCount;
				MapLut(hpSrc, hpDst, lpLut, iCount);
				dwCount -= (DWORD)iCount;
				hpSrc += iCount;
				hpDst += iCount;
				}
#endif
		break;

		case 4 :
#ifdef WIN32
			MapLut32(lpDstData, lpData, lpLut, (int)dwCount);
#else
			hpSrc = lpDstData;
			hpDst = lpData;
			while (dwCount)
				{
				if (dwCount >= 16384L)
					iCount = 16384;
				else
					iCount = (int)dwCount;
				MapLut32(hpSrc, hpDst, lpLut, iCount);
				dwCount -= (DWORD)iCount;
				hpSrc += ((long)iCount*4);
				hpDst += ((long)iCount*4);
				}
#endif
		break;
	}

	return(lpData);
}

/************************************************************************/
HPTR SmearImageSource(int x, int y, int dx, int dy, int State)
/************************************************************************/
{
static THREAD_BASE	WORD weight;
DWORD dwCount;
RECT rDummy;

if (State & (STATE_END|STATE_BEGIN))
	return(NULL);

if (State & STATE_FIRSTSTROKE)
	{
	// Load the destination buffer with destination data
	LoadSourceData(lpDstPixmap, BrushData.pmDstType, x, y, dx, dy, lpData,
				(LPRECT)&rDummy, NO, NO);
	if (BrushData.BltFrameTypeInfo.DataType == FDT_LINEART ||
		BrushData.BltFrameTypeInfo.DataType == FDT_PALETTECOLOR)
		weight = 0;
	else
		weight = lpBrush->Weight;
	if (weight > 127)
		++weight;
	if (weight)
		{
#ifdef WIN32
		if (iBltFrameDepth == 3)
			Colorize24((LPRGB)lpData, (LPRGB)lpData, dx*dy, (LPRGB)lpColor, weight);
		else
		if (iBltFrameDepth == 4)
			Colorize32((LPCMYK)lpData, (LPCMYK)lpData, dx*dy, (LPCMYK)lpColor, weight);
		else
			Colorize8(lpData, lpData, dx*dy, lpColor, weight);
#else
		dwCount = (DWORD)dx * (DWORD)dy;
		if (iBltFrameDepth == 3)
			Colorize24Huge((LPRGB)lpData, (LPRGB)lpData, dwCount, (LPRGB)lpColor, weight);
		else
		if (iBltFrameDepth == 4)
			Colorize32Huge((LPCMYK)lpData, (LPCMYK)lpData, dwCount, (LPCMYK)lpColor, weight);
		else
			Colorize8Huge(lpData, lpData, dwCount, lpColor, weight);
#endif
		}
 	return(lpData);
	}
else
	{
	if (weight)
		{
#ifdef WIN32
		if (iBltFrameDepth == 3)
			Colorize24((LPRGB)lpData, (LPRGB)lpData, dx*dy, (LPRGB)lpColor, weight);
		else
		if (iBltFrameDepth == 4)
			Colorize32((LPCMYK)lpData, (LPCMYK)lpData, dx*dy, (LPCMYK)lpColor, weight);
		else
			Colorize8(lpData, lpData, dx*dy, lpColor, weight);
#else
		dwCount = (DWORD)dx * (DWORD)dy;
		if (iBltFrameDepth == 3)
			Colorize24Huge((LPRGB)lpData, (LPRGB)lpData, dwCount, (LPRGB)lpColor, weight);
		else
		if (iBltFrameDepth == 4)
			Colorize32Huge((LPCMYK)lpData, (LPCMYK)lpData, dwCount, (LPCMYK)lpColor, weight);
		else
			Colorize8Huge(lpData, lpData, dwCount, lpColor, weight);
#endif
		}
	return(lpData);
	}
}

/************************************************************************/
static LPMGXBRUSH AllocMgxBrush(void)
/************************************************************************/
{
LPMGXBRUSH lpMgxBrush;

// Allocate memory for brush
if (!(lpMgxBrush = (LPMGXBRUSH)Alloc((long)sizeof(MGXBRUSH))))
	{
	Message(IDS_EMEMALLOC);
	return(NULL);
	}
clr((LPTR)lpMgxBrush, sizeof(MGXBRUSH));
return(lpMgxBrush);
}

/************************************************************************/
static void FreeUpMgxBrush(LPMGXBRUSH lpBrush)
/************************************************************************/
{
if (lpBrush)
	{
	if (lpBrush->BrushMask)
		FreeUp(lpBrush->BrushMask);
	FreeUp((LPTR)lpBrush);
	}
}

/************************************************************************/
static void CombineData24( LPRGB lpDst, LPRECT lpDstRect, int DstWidth,
                              LPRGB lpSrc, LPRECT lpSrcRect, int SrcWidth,
                              LPRECT lpSectRect, WORD sb )
/************************************************************************/
{
int iWidth, iHeight;
WORD db;

#ifdef C_CODE
int dx, dy, iCount;
LPRGB lpSrcBuf, lpDstBuf, lpSrcTmp, lpDstTmp;
BYTE r, g, b;

db = 256 - sb;

dy = lpSectRect->top - lpDstRect->top;
dx = lpSectRect->left - lpDstRect->left;
lpDstBuf = lpDst + (DstWidth * dy) + dx;

dy = lpSectRect->top - lpSrcRect->top;
dx = lpSectRect->left - lpSrcRect->left;
lpSrcBuf = lpSrc + (SrcWidth * dy) + dx;

iWidth = RectWidth(lpSectRect);
iHeight = RectHeight(lpSectRect);
while (--iHeight >= 0)
	{
	lpSrcTmp = lpSrcBuf;
	lpDstTmp = lpDstBuf;
	iCount = iWidth;
	while (--iCount >= 0)
		{
		r = ((lpDstTmp->red*db)+(lpSrcTmp->red*sb))>>8;
		lpDstTmp->red = lpSrcTmp->red = r;
		g = ((lpDstTmp->green*db)+(lpSrcTmp->green*sb))>>8;
		lpDstTmp->green = lpSrcTmp->green = g;
		b = ((lpDstTmp->blue*db)+(lpSrcTmp->blue*sb))>>8;
		lpDstTmp->blue = lpSrcTmp->blue = b;
		lpSrcTmp++;
		lpDstTmp++;
		}
	lpSrcBuf += SrcWidth;
	lpDstBuf += DstWidth;
	}
#else
__asm
	{
	push	ds
	push	si
	push	di
	pushf

;	lpDst = 6
;	lpDstRect = 10
;	DstWidth = 14
;	lpSrc = 16
;	lpSrcRect = 20
;	SrcWidth = 24
;	lpSectRect = 28
;	sb = 26

	les	si,lpSectRect		;lpSectRect
	mov	ax,WORD PTR es:[si+4]	;lpSectRect->right
	sub	ax,WORD PTR es:[si]	;lpSectRect->right-lpSectRect->left
	inc	ax			;RectWidth(lpSectRect)
	mov	cx,ax
	add	ax,ax
	add	ax,cx
	mov	iWidth,ax		; save iWidth * Depth

	mov	ax,WORD PTR es:[si+6]	;lpSectRect->bottom
	sub	ax,WORD PTR es:[si+2]	;lpSectRect->bottom-lpSectRect->top
	inc	ax			;RectHeight(lpSectRect)
	mov	iHeight,ax		; save iHeight

	mov	ax,SrcWidth		; get SrcWidth
	add	ax,ax
	add	SrcWidth,ax		; save SrcWidth * Depth

	mov	ax,DstWidth		; DstWidth
	add	ax,ax
	add	DstWidth,ax		; save DstWidth * Depth

	mov	ax,WORD PTR es:[si+2]
	lds	bx,lpDstRect		;lpDstRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	mov	cx,WORD PTR es:[si]
	sub	cx,WORD PTR ds:[bx]	; cx = dx
	mov	dx,cx
	add	cx,cx
	add	cx,dx			; dx * Depth

	mul 	DstWidth		; dy * DstWidth
	add	ax,cx			; (dy * DstWidth) + dx
	add	ax,WORD PTR [bp+6]	;lpDst
	mov	dx,WORD PTR [bp+8]
	mov	di,ax			;lpDstBuf
	push	dx			; save dst segment

	mov	ax,WORD PTR es:[si+2]
	lds	bx,lpSrcRect		;lpSrcRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	mov	cx,WORD PTR es:[si]
	sub	cx,WORD PTR ds:[bx]	; cx = dx
	mov	dx,cx
	add	cx,cx
	add	cx,dx			; dx * Depth

	mul	SrcWidth		; dy * SrcWidth
	add	ax,cx			; (dy * SrcWidth) + dx
	add	ax,WORD PTR [bp+16]	;lpSrc
	mov	dx,WORD PTR [bp+18]
	mov	si,ax			;lpSrcBuf
	mov	ds,dx

	mov	ax,iWidth
	sub	DstWidth,ax		; DstWidth
	sub	SrcWidth,ax		; SrcWidth

	mov	ax,256
	sub	ax,sb
	mov	db,ax

	pop	es			; restore dst segment

	cld
	jmp	CD24_3
CD24_1:
	mov	cx,iWidth
CD24_2:	
	mov	al,BYTE PTR ds:[si]	; get the source
	sub	ah,ah			; unsigned word conversion
	mul	sb			; source * b

	mov	bx,ax			; bx = (source * sb)
	
	mov	al,BYTE PTR es:[di]	; get the destination
	sub	ah,ah			; unsigned word conversion
	mul	db			; (dest * db)
	
	add	ax,bx			; ax = (s * b) + (d * db)

	mov	BYTE PTR es:[di],ah	; store in destination
	inc	di
	mov	BYTE PTR ds:[si],ah	; store in source
	inc	si

	loop	CD24_2

	add	si,SrcWidth		;SrcWidth
	add	di,DstWidth		;DstWidth
CD24_3:
	dec	iHeight
	jns	CD24_1

	popf
	pop	di
	pop	si
	pop	ds
	}
#endif
}

/************************************************************************/

static void CombineData32( LPCMYK lpDst, LPRECT lpDstRect, int DstWidth,
                              LPCMYK lpSrc, LPRECT lpSrcRect, int SrcWidth,
                              LPRECT lpSectRect, register WORD sb )
{
	register WORD db;
	int iWidth, iHeight;

	int dx, dy, iCount;
	LPCMYK lpSrcBuf, lpDstBuf;
	LPTR   lpSrcB = (LPTR)lpSrc;
	LPTR   lpDstB = (LPTR)lpDst;

	db = 256 - sb;

	dy = lpSectRect->top  - lpDstRect->top;
	dx = lpSectRect->left - lpDstRect->left;
	lpDstBuf = lpDst + (DstWidth * dy) + dx;

	dy = lpSectRect->top  - lpSrcRect->top;
	dx = lpSectRect->left - lpSrcRect->left;
	lpSrcBuf = lpSrc + (SrcWidth * dy) + dx;

	iWidth  = RectWidth(lpSectRect);
	iHeight = RectHeight(lpSectRect);

	while (iHeight-- > 0)
	{
		lpSrcB = (LPTR)lpSrcBuf;
		lpDstB = (LPTR)lpDstBuf;
		iCount = iWidth * 4;

		while (iCount-- > 0)
		{
			*lpDstB = *lpSrcB = (((*lpDstB)*db)+((*lpSrcB)*sb))>>8;
			lpDstB++;
			lpSrcB++;
		}

		lpSrcBuf += SrcWidth;
		lpDstBuf += DstWidth;
	}
}

/************************************************************************/
static void CombineData8( LPTR lpDst, LPRECT lpDstRect, int DstWidth,
                              LPTR lpSrc, LPRECT lpSrcRect, int SrcWidth,
                              LPRECT lpSectRect, WORD sb )
/************************************************************************/
{
WORD db;
int iWidth, iHeight;

#ifdef C_CODE
int dx, dy, iCount;
LPTR lpSrcBuf, lpDstBuf, lpSrcTmp, lpDstTmp;
BYTE g;

db = 256 - sb;

dy = lpSectRect->top - lpDstRect->top;
dx = lpSectRect->left - lpDstRect->left;
lpDstBuf = lpDst + (DstWidth * dy) + dx;

dy = lpSectRect->top - lpSrcRect->top;
dx = lpSectRect->left - lpSrcRect->left;
lpSrcBuf = lpSrc + (SrcWidth * dy) + dx;

iWidth = RectWidth(lpSectRect);
iHeight = RectHeight(lpSectRect);
while (--iHeight >= 0)
	{
	lpSrcTmp = lpSrcBuf;
	lpDstTmp = lpDstBuf;
	iCount = iWidth;
	while (--iCount >= 0)
		{
		g = ((db*(*lpDstTmp))+(sb*(*lpSrcTmp)))>>8;
		*lpDstTmp++ = *lpSrcTmp++ = g;
		}
	lpSrcBuf += SrcWidth;
	lpDstBuf += DstWidth;
	}
#else
__asm
	{
	push	ds
	push	si
	push	di
	pushf

;	lpDst = 6
;	lpDstRect = 10
;	DstWidth = 14
;	lpSrc = 16
;	lpSrcRect = 20
;	SrcWidth = 24
;	lpSectRect = 28
;	sb = 26

	les	si,lpSectRect		;lpSectRect
	mov	ax,WORD PTR es:[si+4]	;lpSectRect->right
	sub	ax,WORD PTR es:[si]	;lpSectRect->right-lpSectRect->left
	inc	ax			;RectWidth(lpSectRect)
	mov	iWidth,ax

	mov	ax,WORD PTR es:[si+6]	;lpSectRect->bottom
	sub	ax,WORD PTR es:[si+2]	;lpSectRect->bottom-lpSectRect->top
	inc	ax			;RectHeight(lpSectRect)
	mov	iHeight,ax

	mov	ax,WORD PTR es:[si+2]
	lds	bx,lpDstRect		;lpDstRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	mov	cx,WORD PTR es:[si]
	sub	cx,WORD PTR ds:[bx]	; cx = dx

	mul 	DstWidth		; dy * DstWidth
	add	ax,cx			; (dy * DstWidth) + dx
	add	ax,WORD PTR [bp+6]	;lpDst
	mov	dx,WORD PTR [bp+8]
	mov	di,ax			;lpDstBuf
	push	dx			; save dst segment

	mov	ax,WORD PTR es:[si+2]
	lds	bx,lpSrcRect		;lpSrcRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	mov	cx,WORD PTR es:[si]
	sub	cx,WORD PTR ds:[bx]	; cx = dx

	mul	SrcWidth		; dy * SrcWidth
	add	ax,cx			; (dy * SrcWidth) + dx
	add	ax,WORD PTR [bp+16]	;lpSrc
	mov	dx,WORD PTR [bp+18]
	mov	si,ax			;lpSrcBuf
	mov	ds,dx

	mov	ax,iWidth
	sub	DstWidth,ax		; DstWidth
	sub	SrcWidth,ax		; SrcWidth

	mov	ax,256
	sub	ax,sb
	mov	db,ax

	pop	es			; restore dst segment

	cld
	jmp	CD8_3
CD8_1:
	mov	cx,iWidth
CD8_2:	
	mov	al,BYTE PTR ds:[si]	; get the source
	sub	ah,ah			; unsigned word conversion
	mul	sb			; source * b

	mov	bx,ax			; bx = (source * sb)
	
	mov	al,BYTE PTR es:[di]	; get the destination
	sub	ah,ah			; unsigned word conversion
	mul	db			; (dest * db)
	
	add	ax,bx			; ax = (s * b) + (d * db)

	mov	BYTE PTR es:[di],ah	; store in destination
	inc	di
	mov	BYTE PTR ds:[si],ah	; store in source
	inc	si

	loop	CD8_2

	add	si,SrcWidth		;SrcWidth
	add	di,DstWidth		;DstWidth
CD8_3:
	dec	iHeight
	jns	CD8_1
	
	popf
	pop	di
	pop	si
	pop	ds
	}
#endif
}

/************************************************************************/
LOCAL void Colorize24Huge(HPRGB hpDst, HPRGB hpSrc, DWORD dwCount, HPRGB hpColor, WORD sb)
/************************************************************************/
{
// It is assumed that lpSrc and lpDst are allocated pointers
// That they both point to the beginning of an allocated block
// i.e. - zero offset
	DWORD dwLeft;
	RGBS	srcRGB, dstRGB, clrRGB;

	// get current position in segment
	while (dwCount)
	{
		// get how many bytes left in segment
		dwLeft = 65536L - ((DWORD)hpDst & 0xFFFFL);
		dwLeft /= 3;
		if (!dwLeft) // RGB spans segment boundary
		{
			srcRGB.red = hpSrc->red;
			srcRGB.green = hpSrc->green;
			srcRGB.blue = hpSrc->blue;
			dstRGB.red = hpDst->red;
			dstRGB.green = hpDst->green;
			dstRGB.blue = hpDst->blue;
			clrRGB.red = hpColor->red;
			clrRGB.green = hpColor->green;
			clrRGB.blue = hpColor->blue;
			Colorize24(&dstRGB, &srcRGB, 1, &clrRGB, sb);
			--dwCount;
			++hpSrc;
			++hpDst;
			++hpColor;
		}
		else
		{
			// see if we will cross a segment boundary
			if (dwLeft > INT_MAX)
				dwLeft = INT_MAX; // never try to copy more than maxint bytes
			if (dwLeft > dwCount)
				dwLeft = dwCount;
			Colorize24(hpDst, hpSrc, (int)dwLeft, hpColor, sb);
			dwCount -= dwLeft;
			hpDst += dwLeft;
			hpSrc += dwLeft;
			hpColor += dwLeft;
		}
	}
}

/************************************************************************/
LOCAL void Colorize24(LPRGB lpDst, LPRGB lpSrc, int iCount, LPRGB lpColor, WORD sb)
/************************************************************************/
{
WORD	r, g, b;

#ifdef C_CODE
WORD	db;

db = 256 - sb;
r = sb*lpColor->red;
g = sb*lpColor->green;
b = sb*lpColor->blue;
while (--iCount >= 0)
	{
	lpDst->red = ((db*lpSrc->red)+r+128)>>8;
	lpDst->green = ((db*lpSrc->green)+g+128)>>8;
	lpDst->blue = ((db*lpSrc->blue)+b+128)>>8;
	++lpDst;
	++lpSrc;
	}
#else
__asm
	{
	push	ds
	push	si
	push	di
	pushf

	mov	cx,iCount
	or	cx,cx
	je Done
	lds	si,lpColor
	sub	ah,ah
	lodsb
	mul	sb
	mov	r,ax
	sub	ah,ah
	lodsb
	mul	sb
	mov	g,ax
	sub	ah,ah
	lodsb
	mul	sb
	mov	b,ax
	mov	bx,256
	sub	bx,sb
	les	di,lpDst
	lds	si,lpSrc
	cld
C24_1:
; Process Red
	sub	ah,ah
	lodsb
	mul	bx
	add	ax,r
	add	ax,128
	mov	al,ah
	stosb

; Process Green
	sub	ah,ah
	lodsb
	mul	bx
	add	ax,g
	add	ax,128
	mov	al,ah
	stosb

; Process Blue
	sub	ah,ah
	lodsb
	mul	bx
	add	ax,b
	add	ax,128
	mov	al,ah
	stosb

	loop	C24_1

Done:
	popf
	pop	di
	pop	si
	pop	ds
	}
#endif
}

/************************************************************************/

LOCAL void Colorize32Huge(HPCMYK hpDst, HPCMYK hpSrc, DWORD dwCount,	HPCMYK hpColor,
					WORD sb)
{
// It is assumed that hpSrc and lpDst are allocated pointers
// That they both point to the beginning of an allocated block
// i.e. - zero offset
	DWORD dwLeft;

	// get current position in segment
	while (dwCount)
	{
		// get how many bytes left in segment
		dwLeft = 65536L - ((DWORD)hpDst & 0xFFFFL);
		dwLeft /= 4;
		// see if we will cross a segment boundary
		if (dwLeft > INT_MAX)
			dwLeft = INT_MAX; // never try to copy more than maxint bytes
		if (dwLeft > dwCount)
			dwLeft = dwCount;
		Colorize32(hpDst, hpSrc, (int)dwLeft, hpColor, sb);
		dwCount -= dwLeft;
		hpDst += dwLeft;
		hpSrc += dwLeft;
		hpColor += dwLeft;
	}
}

/************************************************************************/

LOCAL void Colorize32(
	LPCMYK lpDst,
	LPCMYK lpSrc,
	int    iCount,
	LPCMYK  lpColor,
	WORD sb)
{
	register WORD db, iCnt;
	WORD c, m, y, k;
	LPTR lpSrcB, lpDstB;

	lpSrcB = (LPTR)lpSrc;
	lpDstB = (LPTR)lpDst;

	iCnt = iCount;

	db = 256 - sb;
	c = sb*(lpColor->c);
	m = sb*(lpColor->m);
	y = sb*(lpColor->y);
	k = sb*(lpColor->k);

	while (--iCount >= 0)
	{
		*lpDstB++ = ((db * (*lpSrcB++)) + c + 128) >> 8;
		*lpDstB++ = ((db * (*lpSrcB++)) + m + 128) >> 8;
		*lpDstB++ = ((db * (*lpSrcB++)) + y + 128) >> 8;
		*lpDstB++ = ((db * (*lpSrcB++)) + k + 128) >> 8;
	}
}

/************************************************************************/
LOCAL void Colorize8Huge(HPTR hpDst, HPTR hpSrc, DWORD dwCount, HPTR hpColor, WORD sb)
/************************************************************************/
{
int iCount;

// get current position in segment
while (dwCount)
	{
	// get how many bytes left in segment
	if (dwCount >= 16384L)
		iCount = 16384;
	else
		iCount = (int)dwCount;
	Colorize8(hpDst, hpSrc, iCount, hpColor, sb);
	hpDst += iCount;
	hpSrc += iCount;
	hpColor += iCount;
	dwCount -= iCount;
	}
}

/************************************************************************/
LOCAL void Colorize8(LPTR lpDst, LPTR lpSrc, int iCount, LPTR lpColor, WORD sb)
/************************************************************************/
{
WORD	g;

#ifdef C_CODE
WORD	db;

db = 256 - sb;
g = sb * (*lpColor);
while (--iCount >= 0)
	*lpDst++ = ((db*(*lpSrc++))+g+128)>>8;
#else
__asm
	{
	push	ds
	push	si
	push	di
	pushf

	mov	cx,iCount
	or	cx,cx
	je 	Done

	lds	si,lpColor
	sub	ah,ah
	lodsb
	mul	sb
	mov	g,ax

	mov	bx,256
	sub	bx,sb
	les	di,lpDst
	lds	si,lpSrc
	cld
C8_1:
; Process Gray
	sub	ah,ah
	lodsb
	mul	bx
	add	ax,g
	add	ax,128
	mov	al,ah
	stosb

	loop	C8_1

Done:
	popf
	pop	di
	pop	si
	pop	ds
	}
#endif
}

#ifdef C_CODE
/************************************************************************/
static void MapLut(LPTR lpSrc, LPTR lpDst, LPTR lpLut, unsigned int wCount)
/************************************************************************/
{
if (!wCount)
	return;
while (wCount-- != 0)
	*lpDst++ = lpLut[*lpSrc++];
}

/************************************************************************/
static void MapLut32(LPTR lpSrc, LPTR lpDst, LPTR lpLut, int iCount)
/************************************************************************/
{
BYTE value;

while (--iCount >= 0)
	{
	// cyan
	value = *lpSrc++;
	value ^= 0xFF;
	value = lpLut[value];
	value ^= 0xFF;
	*lpDst++ = value;
	// magenta
	value = *lpSrc++;
	value ^= 0xFF;
	value = lpLut[value];
	value ^= 0xFF;
	*lpDst++ = value;
	// yellow
	value = *lpSrc++;
	value ^= 0xFF;
	value = lpLut[value];
	value ^= 0xFF;
	*lpDst++ = value;
	// black - don't map black
	*lpDst++ = *lpSrc++;
	}
}

#endif

