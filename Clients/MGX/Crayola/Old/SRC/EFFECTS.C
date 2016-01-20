//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "fxparms.h"

static BOOL bSwitchOn;
static int  iSlideSet;
extern HINSTANCE hInstAstral;

typedef int (*LPEFFECTSPROC)(LPFXPARMS lp);

/***********************************************************************/
BOOL DoEffects( LPIMAGE lpImage, LPSTR lpFileName, LPRECT lpEditRect,
                BOOL SwitchOn, int SlideSet)
/***********************************************************************/
{
LPFRAME lpFrame, lpNewFrame;
HWND hControl;
LPOBJECT lpObject;

if ( !lpImage )
	return( NO );
if ( !lpFileName )
	return( NO );
if ( !(lpObject = ImgGetSelObject( lpImage, NULL) ) )
	return( NO );
if ( !ImgEditInit( lpImage, ET_OBJECT, UT_NEWDATA, lpObject ) )
	return( NO );
if ( !(lpFrame = ObjGetEditFrame(lpObject)) )
	return( NO );

// Do the effect
hControl = lpImage->hWnd;// NULL
if ( !EffectLoad( hControl, lpFileName ) )
	return( NO );
bSwitchOn = SwitchOn;
iSlideSet = SlideSet;
EffectInit( lpFrame );
AstralCursor( IDC_WAIT ); /* Put up the wait cursor */
SetRect( lpEditRect, 32767, 32767, -32767, -32767);
lpNewFrame = EffectExec( lpFrame, &lpObject->rObject, ImgGetMask(lpImage),
	lpEditRect );
AstralCursor( NULL ); /* Revert back to the old cursor */
EffectUnload();

if ( !lpNewFrame || (lpNewFrame == lpFrame) ) // anything done?
	return( NO );

ImgEditedObjectFrame( lpImage, lpObject, IDS_UNDOCHANGE, lpEditRect,
	lpNewFrame, NULL);
return( YES );
}


static FXPARMS parms;
static HINSTANCE hModule;
static LPEFFECTSPROC lpStartProc, lpProcessProc, lpEndProc;

/************************************************************************/
static BOOL EffectLoad(
/************************************************************************/
HWND 	hWnd,
LPSTR 	lpFXFile)
{
EffectUnload();

if ( (hModule = AstralLoadLibrary(lpFXFile)) < (HINSTANCE)HINSTANCE_ERROR )
	goto ErrorExit;
if ( !(lpStartProc   = (LPEFFECTSPROC)GetProcAddress(hModule, "_FXStart")) )
	goto ErrorExit;
if ( !(lpProcessProc = (LPEFFECTSPROC)GetProcAddress(hModule, "_FXProcess")) )
	goto ErrorExit;
if ( !(lpEndProc     = (LPEFFECTSPROC)GetProcAddress(hModule, "_FXEnd")) )
	goto ErrorExit;
if ( !(parms.AstralClockCursor	= (LPROC)
     MakeProcInstance( (FARPROC)DLL_AstralClockCursor, hInstAstral )) )
	goto ErrorExit;
if ( !(parms.frame_set		= (LPTRPROC)
     MakeProcInstance( (FARPROC)DLL_frame_set, hInstAstral )) )
	goto ErrorExit;
if ( !(parms.frame_ptr		= (LPTRPROC)
     MakeProcInstance( (FARPROC)DLL_frame_ptr, hInstAstral )) )
	goto ErrorExit;
if ( !(parms.frame_write	= (LPROC)
     MakeProcInstance( (FARPROC)DLL_frame_write, hInstAstral)) )
	goto ErrorExit;
if ( !(parms.frame_open	   = (LPTRPROC)
     MakeProcInstance( (FARPROC)DLL_frame_open, hInstAstral)) )
	goto ErrorExit;
if ( !(parms.frame_close	= (LPTRPROC)
     MakeProcInstance( (FARPROC)DLL_frame_close, hInstAstral)) )
	goto ErrorExit;
if ( !(parms.Message	      = (LPTRPROC)
     MakeProcInstance( (FARPROC)DLL_Message, hInstAstral)) )
	goto ErrorExit;
parms.hInstance = hInstAstral;
parms.hWnd = hWnd;
return( TRUE );

ErrorExit:
EffectUnload();
MessageBeep(0);
return( FALSE );
}


/************************************************************************/
static void EffectInit( LPFRAME lpFrame )
/************************************************************************/
{
HWND hFocusWnd;

parms.lpSrcFrame = NULL;
parms.lpDstFrame = NULL;
parms.dst_npix = parms.src_npix = 0;
parms.dst_nlin = parms.src_nlin = 0;
parms.dst_resolution = parms.src_resolution = 0;
parms.DstDepth = FrameDepth(lpFrame);
parms.Depth = ( FrameType(lpFrame) == FDT_LINEART ? 1 : parms.DstDepth );
parms.lpOut = NULL;
parms.rOut.left = parms.rOut.top = parms.rOut.right = parms.rOut.bottom = 0;
parms.SwitchSet = bSwitchOn;
parms.SlideSet = iSlideSet;

GetActiveColor(&parms.ActiveColor);
GetAlternateColor(&parms.AlternateColor);

hFocusWnd = GetFocus();
(*lpStartProc)((LPFXPARMS)&parms);
SetFocus( hFocusWnd );
}


/************************************************************************/
static void EffectUnload()
/************************************************************************/
{
if ( lpEndProc )
	(*lpEndProc)((LPFXPARMS)&parms);
lpStartProc = lpProcessProc = lpEndProc = NULL;

if ( parms.lpOut )
	FreeUp(parms.lpOut);
parms.lpOut = NULL;

if ( parms.AstralClockCursor )
	FreeProcInstance((FARPROC)parms.AstralClockCursor);
parms.AstralClockCursor = NULL;

if ( parms.frame_set )
	FreeProcInstance((FARPROC)parms.frame_set);
parms.frame_set = NULL;

if ( parms.frame_write )
	FreeProcInstance((FARPROC)parms.frame_write);
parms.frame_write = NULL;

if ( parms.frame_open )
	FreeProcInstance((FARPROC)parms.frame_open);
parms.frame_open = NULL;

if ( parms.frame_close )
	FreeProcInstance((FARPROC)parms.frame_close);
parms.frame_close = NULL;

if ( parms.Message )
	FreeProcInstance((FARPROC)parms.Message );
parms.Message = NULL;

if ( hModule >= (HINSTANCE)HINSTANCE_ERROR )
	FreeLibrary(hModule);
hModule = NULL;
}

/************************************************************************/
static LPFRAME EffectExec( LPFRAME lpFrame, LPRECT lpFrameRect, LPMASK lpMask,
						LPRECT lpApplyRect )
/************************************************************************/
{
BOOL fSuccess, fUsingMask;
RECT rMask, rFrame;
int DstDepth;

if ( !lpProcessProc )
	return( NULL );

if ( !lpFrame )
	return( NULL );

// see where this frame rectangle intersects original effect image
rFrame = *lpFrameRect;

// get mask rect - if no mask, set to the frame rectangle
if (MaskRect(lpMask, &rMask))
	{
	// intersect the mask rectangle with effect rectangle
	if (!AstralIntersectRect(&parms.rOut, &rMask, &rFrame))
		return(NULL);
	}
else
	parms.rOut = rFrame;

// offset rOut to be in frames coordinates
if (lpApplyRect)
	AstralUnionRect(lpApplyRect, lpApplyRect, &parms.rOut);
OffsetRect(&parms.rOut, -lpFrameRect->left, -lpFrameRect->top);

fUsingMask = (lpMask || (ColorMask.Mask && ColorMask.On));
parms.lpSrcFrame = lpFrame;
parms.dst_npix = parms.src_npix = FrameXSize(lpFrame);
parms.dst_nlin = parms.src_nlin = FrameYSize(lpFrame);
parms.dst_resolution = parms.src_resolution = FrameResolution(lpFrame);

DstDepth = parms.DstDepth;
if (!DstDepth)
	DstDepth = 1;

if ( !(parms.lpDstFrame = FrameOpen((FRMDATATYPE)parms.DstDepth,
     parms.dst_npix, parms.dst_nlin, parms.dst_resolution)) )
	{
	FrameError( IDS_EPROCESSOPEN );
	return( NULL );
	}

// take care of areas the effect will not edit in the destination by
// setting up a background color.  EffectMask() will always take care of
// the problem.
if ((RectWidth(&parms.rOut) < parms.dst_npix ||
	RectHeight(&parms.rOut) < parms.dst_nlin) && !fUsingMask)
	{
	if (FrameType(parms.lpDstFrame) == FDT_CMYKCOLOR)
		FrameSetBackground(parms.lpDstFrame, 0);
	else
		FrameSetBackground(parms.lpDstFrame, 255);
	}

if ( !(parms.lpOut = Alloc( (long)FrameXSize(parms.lpDstFrame) * DstDepth )) )
	return( NULL );

// Execute the effect
frame_set(parms.lpSrcFrame);
if ( fSuccess = (*lpProcessProc)((LPFXPARMS)&parms) )
	{
	if (fUsingMask)
		EffectMask(lpMask, lpFrameRect, &rFrame);
	}
else
	{
	// failure
	FrameClose( parms.lpDstFrame );
	}
return( fSuccess ? parms.lpDstFrame : NULL );
}

/************************************************************************/
static BOOL EffectMask( LPMASK lpMask, LPRECT lpFrameRect, LPRECT lpEditRect )
/************************************************************************/
{
BOOL fSameDepth;
LPTR lpMaskBuf, lpSrc, lpDst, lpMaskPtr;
int SrcDepth, DstDepth;
int y, dx, dy, xOff, yOff, mdx, ys, ye, xs;
LPPROCESSPROC lpProcessProc;

if (!(lpMaskBuf = Alloc( (long)FrameXSize(parms.lpDstFrame))))
	return(FALSE);

MessageStatus( IDS_PLEASEWAIT );

// get info about source and dest frames
SrcDepth = FrameDepth(parms.lpSrcFrame);
DstDepth = FrameDepth(parms.lpDstFrame);
dx = FrameXSize(parms.lpDstFrame);
dy = FrameYSize(parms.lpDstFrame);
fSameDepth = (SrcDepth == DstDepth);

xOff = lpFrameRect->left;
yOff = lpFrameRect->top;

xs = lpEditRect->left - xOff;
ys = lpEditRect->top - yOff;
ye = ys + RectHeight(lpEditRect) - 1;

mdx = RectWidth(lpEditRect);
lpMaskPtr = lpMaskBuf + xs;
xOff += xs;

lpProcessProc = GetProcessProc(MM_NORMAL, DstDepth);

// copy source data to destination data using inverted mask
// because the destination is really the source when using
// the mask stuff.  Since the destination data is the newly
// created data, it should be merged in with the source, however
// we want to return a new destination frame so the process
// must work the other way.
for (y = 0; y < dy; y++)
	{
	if (!(lpDst = FramePointer(parms.lpDstFrame, 0, y, YES)))
		continue;
	if (!(lpSrc = FramePointer(parms.lpSrcFrame, 0, y, NO)))
		continue;
	if (y >= ys && y <= ye)
		{
		clr(lpMaskBuf, dx);
		// setup mask based on src
		mload( 0+xOff, y+yOff, mdx, 1, lpMaskPtr, lpSrc,
					DstDepth, lpMask);
		// invert mask
		negate(lpMaskBuf,dx);
		}
	else
		set(lpMaskBuf, dx, 255);

	// convert src data to dst frame format
	if (!fSameDepth)
		{
		ConvertData(lpSrc, SrcDepth, dx, parms.lpOut, DstDepth);
		lpSrc = parms.lpOut;
		}
	// write the src onto the dst with inverted mask
	(*lpProcessProc)( lpDst, lpSrc, lpMaskBuf, dx );
	}	// for y
FreeUp(lpMaskBuf);
MessageStatus( NULL );

	return TRUE;
}

/************************************************************************/
int EXPORT DLL_Message( int idStr, ... )
/************************************************************************/
{
LPSTR lpArguments;

lpArguments = (LPSTR)&idStr + sizeof(idStr);
return( Message( idStr, lpArguments ) );
}


/************************************************************************/
int EXPORT DLL_AstralClockCursor( int part, int whole )
/************************************************************************/
{
	return(AstralClockCursor(part, whole, YES));
}


/************************************************************************/
LPTR EXPORT DLL_frame_ptr( int Depth, int x, int y, BOOL bModify )
/************************************************************************/
{
	if (Depth == -1)
		return(FramePointerRaw(frame_set(NULL), x, y, bModify));
	else
		return(FramePointer(frame_set(NULL), x, y, bModify));
}

/************************************************************************/
LPTR EXPORT DLL_frame_set( LPTR lpFrame )
/************************************************************************/
{
return((LPTR)frame_set((LPFRAME)lpFrame));
}


/************************************************************************/
void EXPORT DLL_frame_close( LPFRAME lpFrame )
/************************************************************************/
{
FrameClose(lpFrame);
}

/************************************************************************/
LPFRAME EXPORT DLL_frame_open( WORD Depth, WORD x_size, WORD y_size, WORD Resolution )
/************************************************************************/
{
	LPFRAME lpNewFrame = NULL;

	switch(Depth)
	{
	 	case 0 : lpNewFrame = FrameOpen(FDT_LINEART,   x_size, y_size, Resolution); break;
	 	case 1 : lpNewFrame = FrameOpen(FDT_GRAYSCALE, x_size, y_size, Resolution); break;
	 	case 3 : lpNewFrame = FrameOpen(FDT_RGBCOLOR,  x_size, y_size, Resolution); break;
	 	case 4 : lpNewFrame = FrameOpen(FDT_CMYKCOLOR, x_size, y_size, Resolution); break;
	}

	if (lpNewFrame == NULL)
		FrameError( IDS_EPROCESSOPEN );

	return(lpNewFrame);
}

/************************************************************************/
int EXPORT DLL_frame_write( int x, int y, int dx, LPTR pixels, int count )
/************************************************************************/
{
int ret;

ret = FrameWrite( parms.lpDstFrame, x, y, dx, pixels, count);
return( ret );
}
