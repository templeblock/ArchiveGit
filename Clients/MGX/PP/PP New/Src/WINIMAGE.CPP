//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "svrdoc.h"
#include "svrview.h"
#include "cpo.h"
#include "custools.h"

extern BOOL fAppActive;
//extern BOOL IsInPreview;
__declspec( dllimport ) LPKCMSEVALPROC	lpKCMSEvalProc;

#define LINES_PER_BLT 20

// Static prototypes
static void MaskDataLine( LPTR lpData, LPTR lpMask, int dx, FRMDATATYPE Type,
                LPCOLORINFO lpColor);

static LPTR lpBuffer;
static long lBufSize;
static BOOL fAllowPaintAbort = TRUE;

/************************************************************************/
void UpdateImage(LPIMAGE lpImage, LPRECT lpRepairRect, BOOL fInvalidate )
/************************************************************************/
{
	UpdateImageEx(lpImage, lpRepairRect, fInvalidate, YES, YES);
}

/************************************************************************/
void UpdateImageEx(LPIMAGE lpTheImage, LPRECT lpRepairRect, BOOL fInvalidate,
                    BOOL fUpdateActive, BOOL fUpdateNonActive )
/************************************************************************/
{
	HWND hWnd, hTheWnd;
	RECT DispRect;
	LPDISPLAY lpDisplay;

	hTheWnd = GetActiveDoc();
	if (lpTheImage != GetImagePtr(hTheWnd))
   	{
    	fUpdateActive = NO;
    	hTheWnd = NULL;
   	}
	if (fUpdateActive)
   	{
    	lpDisplay = GetDisplayPtr(hTheWnd);
    	if (lpRepairRect)
        	File2DispRect(hTheWnd, lpRepairRect, &DispRect);
    	else
        	DispRect = lpDisplay->DispRect;
    	if (fInvalidate)
        	InvalidateRect(hTheWnd, AstralToWindowsRect(&DispRect), FALSE);
    	else
       	{
			ASSERT(FALSE);	
       	}
   	}

	if (!fUpdateNonActive)
    	return;

	CServerDoc *pDoc = PictPubApp.GetDocument(lpTheImage);
	if (!pDoc)
		return;

	POSITION ViewPos = pDoc->GetFirstViewPosition();
	hWnd = NULL;
	while (TRUE)
	{
		// If we just update the zoom window then we are all done
		if (Window.fHasZoom && hZoomWindow && hZoomWindow == hWnd)
			break;
		hWnd = NULL;
		if (ViewPos)
	    {
	       	CView* pFirstView = pDoc->GetNextView( ViewPos );
	       	if (pFirstView)
	           	hWnd = pFirstView->GetSafeHwnd();
	    }
		// once we hit the end of the list try the zoom window
		if (!hWnd)
		{
	    	if (Window.fHasZoom && hZoomWindow)
			{
				if (lpTheImage == GetImagePtr(hZoomWindow))
		       		hWnd = hZoomWindow;
			}

		}
		// if no window, then nothing left to do
		if (!hWnd)
			break;

       	if (hWnd != hTheWnd)
	    {
	    	lpDisplay = GetDisplayPtr(hWnd);
	    	if (lpRepairRect)
	    		File2DispRect(hWnd, lpRepairRect, &DispRect);
	    	else
	    		DispRect = lpDisplay->DispRect;
	    	if (fInvalidate)
	    		InvalidateRect(hWnd, AstralToWindowsRect(&DispRect), FALSE);
	    	else
	    	{
				ASSERT(FALSE);	
	    	}
		}
	}
}

/************************************************************************/
void UpdateImageSize(LPIMAGE lpImage)
/************************************************************************/
{
	int cx, cy, left, top;
	CServerDoc *pDoc;
    HWND hActiveMDIWnd = GetActiveDoc(); // Active MDIChild
	LPFRAME lpFrame = ImgGetBaseEditFrame(lpImage);
        
    if (!lpImage || !lpFrame)                    
        return;

	if (pDoc = PictPubApp.GetDocument (lpImage))
	{
		pDoc->UpdateImageSize();
		return;
	}

    // try ZOOM Window?
    {
        LPIMAGE lpSearch;
        LPDISPLAY lpDisplay;
        HWND hWnd;
        
        if (Window.fHasZoom && hZoomWindow)
            hWnd = hZoomWindow;
        lpSearch = GetImagePtr(hWnd);        
        if (lpSearch == lpImage)
        {
           if (hWnd == hActiveMDIWnd)
                ZoomMainViewChanged(hWnd);
            if (IsIconic(hWnd))
            {
                // Compute new DispRect and FileRect
                ComputeDispRect(hWnd);
                lpDisplay = GetDisplayPtr(hWnd);
                ComputeFileRect(hWnd, -1, -1, lpDisplay->FileRate );
                InvalidateRect(hWnd, NULL, TRUE);
            }
            else
			{
				lpDisplay = GetDisplayPtr(hWnd);

				lpImage->npix = FrameXSize(lpFrame);
				lpImage->nlin = FrameYSize(lpFrame);
				lpDisplay->FileRate = TINYRATE;
				lpDisplay->ViewPercentage = 0;
				SetRect( &lpDisplay->FileRect, 0,0,FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1);
				AstralSetRectEmpty( &lpDisplay->UpdateRect );
				AstralSetRectEmpty( &lpDisplay->PaintRect );
				SaveLastView(hWnd);
				GetWinXY(hWnd, &left, &top);
				cx = cy = 0;
				FullViewWindow(hWnd, left, top,
				            cx, cy, YES);
				if ( Tool.hRibbon && (hWnd == GetActiveDoc()))
				    POST_WM_DOCCHANGED( Tool.hRibbon );
			}
        }                
    }
}


/************************************************************************/
BOOL AbortUpdate()
/************************************************************************/
{ // Only interrupt on things that the user initiates
MSG msg;

if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE|PM_NOYIELD ) )
    return(TRUE);
return(FALSE);
}

/************************************************************************/
int DrawFrameEx(
    LPFRAME lpFrame,
    HDC     hDC,
    LPRECT  lpSourceRect,
    LPRECT  lpDestRect,
    LPRECT  lpRepairRect,
	LPVOID	lpCmsXform)
/************************************************************************/
{
FRMTYPEINFO TypeInfo;
int y, yline, ystart, ylast, dxCount, xstart, depth, ddx, ddy, sdx;
int SrcWidth, DstWidth, PaintWidth;
LFIXED yrate, xrate, yoffset, xoffset;
RECT rPaint, rSource, rDest;
long lSizeNeeded, lOffset;
LPTR lpLine;
BLTSESSION BltSession;

#define LINES_PER_BLT 20

depth = FrameDepth( lpFrame );
if (depth == 0) depth = 1;
FrameGetTypeInfo(lpFrame, &TypeInfo);

rDest = *lpDestRect;
rSource = *lpSourceRect;
if ( !lpRepairRect )
    rPaint = rDest;
else
    {
    rPaint = *lpRepairRect;
    BoundRect( &rPaint, rDest.left, rDest.top, rDest.right, rDest.bottom );
    }
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

// get offsets from display edge to paint rect to adjust xoffset/yoffset
ddx = rPaint.left - rDest.left;
ddy = rPaint.top - rDest.top;
dxCount = ddx + PaintWidth;
yoffset = (yrate*ddy)+FDIV2(yrate);
xoffset = (xrate*ddx)+FDIV2(xrate);

// Find x-position in file coordinates where painting begins and ends
sdx = WHOLE(xoffset);
lOffset = ((long)sdx * (long)depth);
xstart = rSource.left;

ystart = rSource.top;
ylast = -1;
StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &rPaint, TypeInfo,
    LINES_PER_BLT, 0, 0, YES, lpCmsXform, lpKCMSEvalProc  );
for ( y=rPaint.top; y<=rPaint.bottom; y++ )
    {
    yline = ystart + WHOLE(yoffset);
    yoffset += yrate;
    if ( yline != ylast )
        {
        ylast = yline;
        if ( lpLine = FramePointer( lpFrame, xstart, yline, NO) )
            {
            FrameSample( lpFrame, lpLine, 0, lpBuffer, (WORD)ddx, dxCount,
                    xrate );
            }
        }
    SuperBlt( &BltSession, lpBuffer );
    }
SuperBlt( &BltSession, NULL );
return( TRUE );
}

/************************************************************************/
int DrawObject( HDC hDC, LPOBJECT lpObject,
            LPRECT lpSourceRect, LPRECT lpDestRect, DWORD dwFlags)
/************************************************************************/
{
int y, yline, ystart, ylast, DispWidth, count, sx, depth, PaintWidth;
LFIXED yrate, xrate, yoffset;
RECT rPaint, rSource, rDest;
long lSizeNeeded;
LPTR lpLine, lpAlphaLine, lpAlphaBuf;
long dx, dy;
LPRECT lpRepairRect;
BLTSESSION BltSession;
LPFRAME lpFrame, lpAlphaFrame;
LPALPHA lpAlpha;
COLORINFO Color;
FRMTYPEINFO TypeInfo;

// gray for masked out area

if (dwFlags & DRAWOBJ_WHITE)
	Color.gray = 0xff;
else if (dwFlags & DRAWOBJ_LIGHT_GRAY)
	Color.gray = 0xC0;
else if (dwFlags & DRAWOBJ_BUTTON_DOWN)
	Color.gray = 0xA5;
else if (dwFlags & DRAWOBJ_MEDIUM_GRAY)
	Color.gray = 0x80;
else if (dwFlags & DRAWOBJ_DARK_GRAY)
	Color.gray = 0x55;
else if (dwFlags & DRAWOBJ_BLACK)
	Color.gray = 0x00;
	

SetColorInfo(&Color, &Color, CS_GRAY);

lpFrame = ObjGetEditFrame(lpObject);
lpAlpha = ObjGetAlpha(lpObject);
lpAlphaFrame = lpAlpha ? lpAlpha->Pixmap.EditFrame : NULL;

FrameGetTypeInfo(lpFrame, &TypeInfo);
depth = FrameDepth( lpFrame );
if (depth == 0) depth = 1;
rDest = *lpDestRect;
rSource = *lpSourceRect;
lpRepairRect = NULL;
DispWidth = RectWidth( &rDest );
lSizeNeeded = (long)DispWidth * (depth+1);
if (lSizeNeeded > lBufSize)
    {
    if (lpBuffer)
        FreeUp(lpBuffer);
    lBufSize = lSizeNeeded;
    if ( !(lpBuffer = Alloc(lBufSize)) )
        {
        lBufSize = 0;
        ProgressEnd();
        return(FALSE);
        }
    }
lpAlphaBuf = lpBuffer + ((long)DispWidth * depth);

if ( !lpRepairRect )
    rPaint = rDest;
else
    {
    rPaint = *lpRepairRect;
    BoundRect( &rPaint, rDest.left, rDest.top, rDest.right, rDest.bottom );
    }

sx = rSource.left;
yrate = FGET( RectHeight(&rSource), RectHeight(&rDest) );
xrate = FGET( RectWidth(&rSource), RectWidth(&rDest) );
ystart = rSource.top;
ylast = -1;

dx = rPaint.left - rDest.left;
dy = rPaint.top - rDest.top;

yoffset = (yrate*dy)+FDIV2(yrate);
PaintWidth = RectWidth(&rPaint);
count = dx + PaintWidth;

StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &rPaint, TypeInfo,
    LINES_PER_BLT, 0, 0, YES, NULL, NULL );
for ( y=rPaint.top; y<=rPaint.bottom; y++ )
    {
    yline = ystart + WHOLE(yoffset);
    yoffset += yrate;
    if ( yline != ylast )
        {
        ylast = yline;
        if ( lpLine = FramePointer( lpFrame, sx, yline, NO) )
            {
            FrameSample( lpFrame, lpLine, 0, lpBuffer, (WORD)dx, count,
                xrate );
            }
        if (lpAlphaFrame)
            {
            if ( lpAlphaLine = FramePointer( lpAlphaFrame, sx, yline, NO) )
                {
                FrameSample( lpAlphaFrame, lpAlphaLine, 0, lpAlphaBuf,
                                (WORD)dx, count, xrate );
                MaskDataLine(lpBuffer, lpAlphaBuf, PaintWidth, TypeInfo.DataType, &Color);
                }
            }
        }
    SuperBlt( &BltSession, lpBuffer );
    }
SuperBlt( &BltSession, NULL );
return( TRUE );
}

/************************************************************************/
int DrawObjList( HDC hDC, LPOBJECT lpBase, LPRECT lpSrcRect, LPRECT lpDstRect,
				LPVOID lpCmsXform)
/************************************************************************/
{
int y, yline, ystart, ylast, DstWidth, SrcWidth, count, sx, depth, PaintWidth;
LFIXED yrate, xrate, yoffset;
RECT rPaint, rSource, rDest;
long lSizeNeeded;
LPTR lpDataBuf;
long dx, dy;
LPRECT lpRepairRect;
BLTSESSION BltSession;
LPFRAME lpFrame;
FRMTYPEINFO TypeInfo;
CHookHandler*  lpImageHook = NULL;
LPIMAGE     lpImage = NULL;

if(lpImageHook = GetImageHook(GetActiveDoc())) lpImage = GetActiveImage();
if((!lpImageHook && lpImage) || (lpImageHook && !lpImage)) {lpImage=NULL; lpImageHook=NULL;}

//if (IsInPreview)
//   {
//   lpImageHook = GetImageHook(GetActiveDoc());
//   lpImage     = GetActiveImage();
//   }

if (!lpBase)
    return(FALSE);

lpFrame = ObjGetEditFrame(lpBase);

FrameGetTypeInfo(lpFrame, &TypeInfo);
depth = FrameDepth( lpFrame );
if (depth == 0) depth = 1;
rDest = *lpDstRect;
rSource = *lpSrcRect;
lpRepairRect = NULL;
DstWidth = RectWidth( &rDest );
SrcWidth = RectWidth( &rSource );
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

lpDataBuf = Alloc((long)SrcWidth*depth);
if (!lpDataBuf)
    return(FALSE);
    
if ( !lpRepairRect )
    rPaint = rDest;
else
    {
    rPaint = *lpRepairRect;
    BoundRect( &rPaint, rDest.left, rDest.top, rDest.right, rDest.bottom );
    }

sx = rSource.left;
yrate = FGET( RectHeight(&rSource), RectHeight(&rDest) );
xrate = FGET( RectWidth(&rSource), RectWidth(&rDest) );
ystart = rSource.top;
ylast = -1;

dx = rPaint.left - rDest.left;
dy = rPaint.top - rDest.top;

yoffset = (yrate*dy)+FDIV2(yrate);
PaintWidth = RectWidth(&rPaint);
count = dx + PaintWidth;

StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &rPaint, TypeInfo,
    LINES_PER_BLT, 0, 0, YES, lpCmsXform, lpKCMSEvalProc );

for ( y=rPaint.top; y<=rPaint.bottom; y++ )
    {
    yline = ystart + WHOLE(yoffset);
    yoffset += yrate;
    if ( yline != ylast )
        {
        ylast = yline;
        ImgGetLine(lpImage, lpBase, sx, yline, SrcWidth, lpDataBuf, lpImageHook);
        FrameSample( lpFrame, lpDataBuf, 0, lpBuffer, (WORD)dx, count,
                xrate );
        }
    SuperBlt( &BltSession, lpBuffer );
    }
SuperBlt( &BltSession, NULL );

FreeUp(lpDataBuf);
return( TRUE );
}

/************************************************************************/
static void MaskDataLine( LPTR lpData, LPTR lpMask, int dx, FRMDATATYPE Type,
                LPCOLORINFO lpColor)
/************************************************************************/
{
BYTE Gray;
RGBS rgb;
CMYKS cmyk;
LPRGB lpRGB;
LPCMYK lpCMYK;

switch (Type)
    {
    case FDT_LINEART:
    case FDT_GRAYSCALE:
        Gray = lpColor->gray;
        while (--dx >= 0)
            {
            if (!(*lpMask++))
                *lpData = Gray;
            ++lpData;
            }
    break;

    case FDT_PALETTECOLOR:
        Gray = lpColor->gray;
        while (--dx >= 0)
            {
            if (!(*lpMask++))
                *lpData = Gray;
            ++lpData;
            }
	break;
	
    case FDT_RGBCOLOR:
        rgb = lpColor->rgb;
        lpRGB = (LPRGB)lpData;
        while (--dx >= 0)
            {
            if (!(*lpMask++))
                *lpRGB = rgb;
            ++lpRGB;
            }
    break;

    case FDT_CMYKCOLOR:
        cmyk = lpColor->cmyk;
        lpCMYK = (LPCMYK)lpData;
        while (--dx >= 0)
            {
            if (!(*lpMask++))
                *lpCMYK = cmyk;
            ++lpCMYK;
            }
    break;

    default:
    break;
    }
}


/************************************************************************/
 void SetupScrollBars( HWND hWindow )
/************************************************************************/
{
int dx, dy, x, y, xMinPos, yMinPos, xMaxPos, yMaxPos, xPos, yPos;
LPIMAGE lpImage;
LPDISPLAY lpDisplay;

lpImage = GetImagePtr(hWindow);
lpDisplay = GetDisplayPtr(hWindow);

// Setup the scroll bar values based on the lpDisplay->FileRect
dx = lpImage->npix - RectWidth( &lpDisplay->FileRect );
dx = max(dx, 0);
x = lpDisplay->FileRect.left;
GetDisplayScrollRange( hWindow, lpDisplay, SB_HORZ, &xMinPos, &xMaxPos);
xPos = GetDisplayScrollPos(hWindow, lpDisplay, SB_HORZ);
if (xMinPos != 0 || xMaxPos != dx || xPos != x)
    {
    SetDisplayScrollRange( hWindow, lpDisplay, SB_HORZ, 0, dx, NO );
    if (hZoomWindow)                         // update zoom window first
        UpdateWindow(hZoomWindow);
    }
dy = lpImage->nlin - RectHeight( &lpDisplay->FileRect );
dy = max(dy, 0);
y = lpDisplay->FileRect.top;
GetDisplayScrollRange( hWindow, lpDisplay, SB_VERT, &yMinPos, &yMaxPos);
yPos = GetDisplayScrollPos(hWindow, lpDisplay, SB_VERT);
if (yMinPos != 0 || yMaxPos != dy || yPos != y)
    {
    SetDisplayScrollRange( hWindow, lpDisplay, SB_VERT, 0, dy, NO );
    SetDisplayScrollPos( hWindow, lpDisplay, SB_VERT, y, YES );
    }
if (xMinPos != 0 || xMaxPos != dx || xPos != x)
    SetDisplayScrollPos( hWindow, lpDisplay, SB_HORZ, x, YES );
}

// This routine returns the file pixel which is underneath the
// passed in display pixel. (i.e. - no rounding)
/************************************************************************/
void Display2File( HWND hWnd, LPINT lpx, LPINT lpy )
/************************************************************************/
{
int x, y;
LFIXED  fx, fy;
LFIXED xrate, yrate;
RECT rDest, rSource;
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);

/* determine location on the display image */
x = *lpx - lpDisplay->DispRect.left;
y = *lpy - lpDisplay->DispRect.top;

rDest = lpDisplay->DispRect;
rSource = lpDisplay->FileRect;
xrate = FGET(RectWidth(&rSource), RectWidth(&rDest));
yrate = FGET(RectHeight(&rSource), RectHeight(&rDest));

fx = xrate * x;
fy = yrate * y;
x = WHOLE(fx);
y = WHOLE(fy);

x += lpDisplay->FileRect.left;
y += lpDisplay->FileRect.top;

*lpx = x;
*lpy = y;
}

/************************************************************************/
void File2Display( HWND hWnd, LPINT lpx, LPINT lpy )
/************************************************************************/
{
File2DisplayEx(hWnd, lpx, lpy, NO);
}

/************************************************************************/
void File2DisplayEx( HWND hWnd, LPLONG lpx, LPLONG lpy, BOOL fCenter )
/************************************************************************/
{
File2DisplayEx( hWnd, ( LPINT )lpx, ( LPINT )lpy, fCenter );
}

/************************************************************************/
void File2DisplayEx( HWND hWnd, LPINT lpx, LPINT lpy, BOOL fCenter )
/************************************************************************/
{
int x, y;
LFIXED xrate, yrate;
RECT rDest, rSource;
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);

/* determine location on hi-res image */
x = *lpx - lpDisplay->FileRect.left;
y = *lpy - lpDisplay->FileRect.top;

rDest = lpDisplay->DispRect;
rSource = lpDisplay->FileRect;
xrate = FGET(RectWidth(&rDest), RectWidth(&rSource));
yrate = FGET(RectHeight(&rDest), RectHeight(&rSource));

if (fCenter)
    {
    x = FMUL( 1, (xrate * x) + FDIV2(xrate) );
    y = FMUL( 1, (yrate * y) + FDIV2(yrate) );
    }
else
    {
    x = FMUL( x, xrate );
    y = FMUL( y, yrate );
    }

x += lpDisplay->DispRect.left;
y += lpDisplay->DispRect.top;

*lpx = x;
*lpy = y;
}



/************************************************************************/
void File2DispRect( HWND hWnd, LPRECT fileRect, LPRECT dispRect )
/************************************************************************/
{
File2DispRectEx(hWnd, fileRect, dispRect);
}

/************************************************************************/
void File2DispRectEx( HWND hWnd, LPRECT fileRect, LPRECT dispRect )
/************************************************************************/
{
RECT rDisp, rFile;
LFIXED xrate, yrate;
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);

rDisp = lpDisplay->DispRect;
rFile = lpDisplay->FileRect;
xrate = FGET(RectWidth(&rDisp), RectWidth(&rFile));
yrate = FGET(RectHeight(&rDisp), RectHeight(&rFile));

dispRect->left = FMUL( fileRect->left - rFile.left, xrate ) + rDisp.left - 1;
dispRect->top = FMUL( fileRect->top - rFile.top, yrate ) + rDisp.top - 1;
dispRect->right = FMUL( fileRect->right - rFile.left + 1, xrate ) + rDisp.left;
dispRect->bottom = FMUL( fileRect->bottom - rFile.top + 1, yrate ) + rDisp.top;
}

/************************************************************************/
void File2DispRectExact( HWND hWnd, LPRECT fileRect, LPRECT dispRect )
/************************************************************************/
{
RECT rDisp, rFile;
LFIXED xrate, yrate;
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);

rDisp = lpDisplay->DispRect;
rFile = lpDisplay->FileRect;
xrate = FGET(RectWidth(&rDisp), RectWidth(&rFile));
yrate = FGET(RectHeight(&rDisp), RectHeight(&rFile));

dispRect->left = FMUL( fileRect->left - rFile.left, xrate ) + rDisp.left;
dispRect->top = FMUL( fileRect->top - rFile.top, yrate ) + rDisp.top;
dispRect->right = FMUL( fileRect->right - rFile.left + 1, xrate ) + rDisp.left - 1;
dispRect->bottom = FMUL( fileRect->bottom - rFile.top + 1, yrate ) + rDisp.top - 1;
}


/************************************************************************/
void File2DispTForm(HWND hWnd, LPTFORM lpTForm)
/************************************************************************/
{
File2DispTFormEx(hWnd, lpTForm, NO);
}

#ifdef UNUSED
/************************************************************************/
void File2DispTFormEx(HWND hWnd, LPTFORM lpTForm, BOOL fCenter)
/************************************************************************/
{
LFIXED rate;
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);
rate = lpDisplay->DispRate;
TMove( lpTForm, -lpDisplay->FileRect.left,
    -lpDisplay->FileRect.top );
TScale( lpTForm, rate, rate);
TMove( lpTForm, lpDisplay->DispRect.left,
    lpDisplay->DispRect.top );
if (fCenter)
    {
    rate = FDIV2(rate);
    TFMove(lpTForm, rate, rate);
    }
}
#endif

/************************************************************************/
void File2DispTFormEx(HWND hWnd, LPTFORM lpTForm, BOOL fCenter)
/************************************************************************/
{
LFIXED xrate, yrate;
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);
xrate = FGET(RectWidth(&lpDisplay->DispRect),
			 RectWidth( &lpDisplay->FileRect ) );
yrate = FGET(RectHeight(&lpDisplay->DispRect),
			 RectHeight( &lpDisplay->FileRect ) );
TMove( lpTForm, -lpDisplay->FileRect.left,
    -lpDisplay->FileRect.top );
TScale( lpTForm, xrate, yrate);
TMove( lpTForm, lpDisplay->DispRect.left,
    lpDisplay->DispRect.top );
if (fCenter)
    {
    xrate = FDIV2(xrate);
    yrate = FDIV2(yrate);
    TFMove(lpTForm, xrate, yrate);
    }
}

/************************************************************************/
void Disp2FileTForm(HWND hWnd, LPTFORM lpTForm)
/************************************************************************/
{
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);
TMove( lpTForm, -lpDisplay->DispRect.left,
    -lpDisplay->DispRect.top );
TScale( lpTForm, lpDisplay->FileRate, lpDisplay->FileRate);
TMove( lpTForm, lpDisplay->FileRect.left,
    lpDisplay->FileRect.top );
}

/************************************************************************/
// Same as SetScrollRange (Needed for large images)
/************************************************************************/
BOOL SetDisplayScrollRange( HWND hWindow, LPDISPLAY lpDisplay, int fnBar, int iMin, int iMax, BOOL fRedraw)
/************************************************************************/
{
#ifdef WIN32 /*SCROLL*/
    int nMinPos, nMaxPos;
    int i = (fnBar == SB_HORZ) ? 1:0;
    
    lpDisplay->iScrollOutMin[i] = iMin;
    lpDisplay->iScrollOutMax[i] = iMax;
    if (WITHIN(iMin, SHRT_MIN, SHRT_MAX) && WITHIN(iMax, SHRT_MIN, SHRT_MAX))
    {
        nMinPos = iMin;
        nMaxPos = iMax;
        lpDisplay->ScrollRate[i] = FUNITY;
    }
    else 
    {
        nMinPos = 0;
        nMaxPos = SHRT_MAX>>1;
        lpDisplay->ScrollRate[i] = FGET(iMax-iMin,nMaxPos);
    }
    lpDisplay->iScrollMin[i] = nMinPos;
    lpDisplay->iScrollMax[i] = nMaxPos;
    return (SetScrollRange(hWindow, fnBar, nMinPos, nMaxPos, fRedraw));
#else
    SetScrollRange(hWindow, fnBar, iMin, iMax, fRedraw);
    return(TRUE);
#endif
}

/************************************************************************/
// Same as SetScrollPos (Needed for large images)
/************************************************************************/
int SetDisplayScrollPos( HWND hWindow, LPDISPLAY lpDisplay, int fnBar, int iVal, BOOL fRedraw)
/************************************************************************/
{
#ifdef WIN32 /*SCROLL*/
    int nPos;
    int i = (fnBar == SB_HORZ) ? 1:0;
    
    // convert to scroll bar values
    if (iVal == lpDisplay->iScrollOutMin[i])
        nPos = lpDisplay->iScrollMin[i];
    else if (iVal == lpDisplay->iScrollOutMax[i])
        nPos = lpDisplay->iScrollMax[i];
    else
    {
        nPos = FROUND(FGET(iVal-lpDisplay->iScrollOutMin[i], lpDisplay->ScrollRate[i]));
        nPos += lpDisplay->iScrollMin[i];
        nPos = bound(nPos, lpDisplay->iScrollMin[i], lpDisplay->iScrollMax[i]);
    }
    nPos = SetScrollPos(hWindow, fnBar, nPos, fRedraw);
    // convert from scroll bar values
    iVal = FROUND((lpDisplay->ScrollRate[i] * (nPos - lpDisplay->iScrollMin[i])));
    iVal += lpDisplay->iScrollOutMin[i];
    iVal = bound(iVal, lpDisplay->iScrollOutMin[i], lpDisplay->iScrollOutMax[i]);
    return(iVal);
#else
    return(SetScrollPos(hWindow, fnBar, iVal, fRedraw));
#endif
}

/************************************************************************/
// Same as GetScrollRange (Needed for large images)
/************************************************************************/
BOOL GetDisplayScrollRange(HWND hWindow, LPDISPLAY lpDisplay, int fnBar, LPINT lpMin, LPINT lpMax)
/************************************************************************/
{
#ifdef WIN32 /*SCROLL*/
    BOOL bRet;
    int iVal;
    int i = (fnBar == SB_HORZ) ? 1:0;
    
    
    bRet = GetScrollRange(hWindow, fnBar, lpMin, lpMax);
    if (!bRet)
        return(FALSE);
    // Convert lpMin from scroll bar values
    iVal = FROUND((lpDisplay->ScrollRate[i] * (*lpMin - lpDisplay->iScrollMin[i])));
    iVal += lpDisplay->iScrollOutMin[i];
    iVal = bound(iVal, lpDisplay->iScrollOutMin[i], lpDisplay->iScrollOutMax[i]);
    *lpMin = iVal;
        
    // Convert lpMax from scroll bar values
    iVal = FROUND((lpDisplay->ScrollRate[i] * (*lpMax - lpDisplay->iScrollMin[i])));
    iVal += lpDisplay->iScrollOutMin[i];
    iVal = bound(iVal, lpDisplay->iScrollOutMin[i], lpDisplay->iScrollOutMax[i]);
    *lpMax = iVal;
    return(TRUE);
#else
    GetScrollRange(hWindow, fnBar, lpMin, lpMax);
    return(TRUE);
#endif
}


/************************************************************************/
// Same as GetScrollPos (Needed for large images)
/************************************************************************/
int GetDisplayScrollPos(HWND hWindow, LPDISPLAY lpDisplay, int fnBar)
/************************************************************************/
{
#ifdef WIN32 /*SCROLL*/
    int iVal;
    int i = (fnBar == SB_HORZ) ? 1:0;
    
    iVal = GetScrollPos(hWindow, fnBar);
    // Convert iVal from scroll bar values
    iVal = FROUND((lpDisplay->ScrollRate[i] * (iVal - lpDisplay->iScrollMin[i])));
    iVal += lpDisplay->iScrollOutMin[i];
    iVal = bound(iVal, lpDisplay->iScrollOutMin[i], lpDisplay->iScrollOutMax[i]);
    return(iVal);
#else
    return(GetScrollPos(hWindow, fnBar));
#endif
}

/************************************************************************/
// Same as GetScrollPos (Needed for large images)
//  NOTE: adjust is in the scroll bar coords.  Used only for thumb tracking
/************************************************************************/
int GetDisplayScrollDelta(HWND hWindow, LPDISPLAY lpDisplay, int fnBar, int adjust)
/************************************************************************/
{
#ifdef WIN32 /*SCROLL*/
    int iVal;
    int i = (fnBar == SB_HORZ) ? 1:0;
    
    iVal = GetScrollPos(hWindow, fnBar) - adjust;
    // Convert iVal from scroll bar values
    iVal = FROUND((lpDisplay->ScrollRate[i] * (iVal - lpDisplay->iScrollMin[i])));
    return(iVal);
#else
    return(GetScrollPos(hWindow, fnBar) - adjust);
#endif
}

/************************************************************************/
void SetDocSystemMenu( HWND hWnd, HMENU hMenu )
/************************************************************************/
{
int state;
LPSTR lpString;
LPIMAGE lpImage;

lpImage = GetImagePtr(hWnd);
if (!lpImage)
    return;
if (ImageIsBusy(lpImage) && (NumImageWindows(lpImage) <= 1))
    EnableMenuItem(hMenu, SC_CLOSE, MF_GRAYED);

if (lpImage->DocumentType == IMG_DOCPIECE)
    {
    state = GetMenuState(hMenu, IDM_FASTBITS, MF_BYCOMMAND);
    if (state < 0)
        {
        if (!AstralStr(IDS_FASTBITS, &lpString))
            return;
        hMenu = GetSystemMenu(hWnd, NO);
        if (!hMenu)
            return;
        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hMenu, MF_BYCOMMAND|MF_STRING, IDM_FASTBITS, lpString);
        }
    }
else if (lpImage->DocumentType == IMG_AVIDOC)
    {
        if ((int)GetMenuState(hMenu, IDM_AVI_PREVIOUS, MF_BYCOMMAND) < 0)
        {
            if (!AstralStr(IDS_AVI_PREVIOUS, &lpString))
                return;
            hMenu = GetSystemMenu(hWnd, NO);
            if (!hMenu)
                return;
            AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenu(hMenu, MF_BYCOMMAND|MF_STRING, IDM_AVI_PREVIOUS, lpString);
        }
        if ((int)GetMenuState(hMenu, IDM_AVI_NEXT, MF_BYCOMMAND) < 0)
        {
            if (!AstralStr(IDS_AVI_NEXT, &lpString))
                return;
            hMenu = GetSystemMenu(hWnd, NO);
            if (!hMenu)
                return;
            AppendMenu(hMenu, MF_BYCOMMAND|MF_STRING, IDM_AVI_NEXT, lpString);
        }
    }
}


/************************************************************************/
BOOL ImageIsDoc( LPIMAGE lpImageDoc )
/************************************************************************/
{
	return (PictPubApp.GetDocument(lpImageDoc)!=NULL);
}

static LPOBJECT lpFocusObject;
static RECT rFocusObject;
static HWND hFocusObjectWnd;
#define TIMER_ID 99
#define TIMER_DELAY 2000

/***********************************************************************/
BOOL FocusObjectStart( HWND hWnd )
/***********************************************************************/
{
LPIMAGE lpImage;

if ( !(lpImage = GetImagePtr(hWnd)))
    return( NO );
if ( ImgCountObjects( lpImage ) <= 1 )
    return( NO );

if ( hFocusObjectWnd && hWnd != hFocusObjectWnd )
    FocusObjectEnd(hWnd);

if ( hFocusObjectWnd )
    InvertSelection( hFocusObjectWnd, Window.hDC, &rFocusObject, SL_BLOCK );
else
    lpFocusObject = ImgGetBase(lpImage);

if ( !(lpFocusObject = ImgGetNextObject(lpImage, lpFocusObject, YES, YES)) )
    return( NO );
hFocusObjectWnd = hWnd;
rFocusObject = lpFocusObject->rObject;
InvertSelection( hFocusObjectWnd, Window.hDC, &rFocusObject, SL_BLOCK );
SetTimer( hFocusObjectWnd, TIMER_ID, TIMER_DELAY, NULL );
return( YES );
}

/***********************************************************************/
BOOL FocusObjectEnd( HWND hWnd )
/***********************************************************************/
{
if ( !hFocusObjectWnd )
    return( NO );
KillTimer( hFocusObjectWnd, TIMER_ID );
InvertSelection( hFocusObjectWnd, Window.hDC, &rFocusObject, SL_BLOCK );
hFocusObjectWnd = NULL;
lpFocusObject = NULL;
return( YES );
}

/***********************************************************************/
BOOL FocusObjectSelect( HWND hWnd )
/***********************************************************************/
{
RECT rDisp;

if ( !hFocusObjectWnd || !lpFocusObject )
    return( NO );
if ( !(lpFocusObject->fSelected = !lpFocusObject->fSelected) )
    { // If it is turning off, redraw due to the marquee
    InvertSelection( hFocusObjectWnd, Window.hDC, &rFocusObject, SL_BLOCK );
    File2DispRect( hFocusObjectWnd, &rFocusObject, &rDisp );
    InvalidateRect( hFocusObjectWnd, &rFocusObject, FALSE );
    AstralUpdateWindow( hFocusObjectWnd );
    InvertSelection( hFocusObjectWnd, Window.hDC, &rFocusObject, SL_BLOCK );
    }
SetTimer( hFocusObjectWnd, TIMER_ID, TIMER_DELAY, NULL );
return( YES );
}

/***********************************************************************/
BOOL FocusObjectDelete( HWND hWnd )
/***********************************************************************/
{
LPOBJECT lpObject;
RECT rObject;

if ( !lpFocusObject )
    return( NO );
lpObject = lpFocusObject;
FocusObjectEnd(hWnd);
if (AstralAffirm(IDS_OBJECTDELETE))
    {
    LPIMAGE lpImage;

    lpImage = GetImagePtr(hWnd);
    rObject = lpObject->rObject;
    ImgDeleteObject( lpImage, lpObject );
    UpdateImage(lpImage, &rObject, YES);
    }
return( YES );
}

/***********************************************************************/
void AstralUpdateWindow(HWND hWnd)
/***********************************************************************/
{
// Ted 7/26/94: this functionality is being disabled because when
// it did not work the program ran faster. So, if a bug is found that
// needs this functionality (which is to force the window to get fully
// repainted) then call AstralUpdateWindowNow()
//if (IsDoc(hWnd))
//    {
//    CServerView *pView = (CServerView*)CWnd::FromHandle(hWnd);
//	if (pView)
//		pView->UpdateWindowNow();
//    }
//else
UpdateWindow(hWnd);
}

/***********************************************************************/
void AstralUpdateWindowNow(HWND hWnd)
/***********************************************************************/
{
if (IsDoc(hWnd))
    {
    CServerView *pView = (CServerView*)CWnd::FromHandle(hWnd);
	if (pView)
		pView->UpdateWindowNow();
    }
else
    UpdateWindow(hWnd);
}

/***********************************************************************/
CHookHandler *SetDisplayHook(HWND hWnd, CHookHandler *pDisplayHook)
/***********************************************************************/
{
LPDISPLAY lpDisplay;
CHookHandler *lpOldDisplayHook;

lpDisplay = GetDisplayPtr(hWnd);
if ( !lpDisplay )
    return( NULL );
lpOldDisplayHook = lpDisplay->lpDisplayHook;
lpDisplay->lpDisplayHook = pDisplayHook;
return( lpOldDisplayHook );
}

/***********************************************************************/
CHookHandler *GetDisplayHook(HWND hWnd)
/***********************************************************************/
{
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);
if (lpDisplay)
    return(lpDisplay->lpDisplayHook);
else
    return(NULL);
}

/***********************************************************************/
void SetMaskHook(HWND hWnd, LPMASKHOOK lpMaskHook)
/***********************************************************************/
{
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);
if (lpDisplay)
    lpDisplay->lpMaskHook = lpMaskHook;
}

/***********************************************************************/
LPMASKHOOK GetMaskHook(HWND hWnd)
/***********************************************************************/
{
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);
if (lpDisplay)
    return(lpDisplay->lpMaskHook);
else
    return(NULL);
}

/***********************************************************************/
void SetImageHook(HWND hWnd, CHookHandler *pImageHook)
/***********************************************************************/
{
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);
if (lpDisplay)
    lpDisplay->lpImageHook = pImageHook;
}

/***********************************************************************/
CHookHandler * GetImageHook(HWND hWnd)
/***********************************************************************/
{
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);
if (lpDisplay)
    return(lpDisplay->lpImageHook);
else
    return(NULL);
}

/***********************************************************************/
HWND GetActiveDoc()
/***********************************************************************/
{
	return (PictPubApp.Get_hActiveWindow());
}

/***********************************************************************/
LPIMAGE GetActiveImage()
/***********************************************************************/
{
HWND hActiveWnd;

hActiveWnd = GetActiveDoc();
if (hActiveWnd)
    return(GetImagePtr(hActiveWnd));
else
    return(NULL);
}

/***********************************************************************/
LPIMAGE GetImagePtr(HWND hWnd)
/***********************************************************************/
{        
    LPIMAGE lpImage = NULL;
    if (hWnd)
    {      
        CWnd *pWnd = CWnd::FromHandle(hWnd);
        if (!pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))        
            lpImage = (LPIMAGE)GetWindowLong(hWnd, GWL_IMAGEPTR);
        else 
        {
            CServerDoc *pDoc = ((CServerView *)pWnd)->GetDocument();
            ASSERT(pDoc);
            if (pDoc)
            	lpImage = pDoc->m_lpImage;
        }
    }
    return (lpImage);
}

/***********************************************************************/
LPDISPLAY GetDisplayPtr(HWND hWnd)
/***********************************************************************/
{     
    LPDISPLAY lpDisplay = NULL;
    if (hWnd)
    {
        CWnd *pWnd = CWnd::FromHandle(hWnd);
        if (!pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))
            lpDisplay = (LPDISPLAY)GetWindowLong(hWnd, GWL_DISPLAYPTR);
        else
        {
            lpDisplay = ((CServerView *)pWnd)->m_lpDisplay;
        }    
    }
    return(lpDisplay);
}

/***********************************************************************/
void SetImagePtr(HWND hWnd, LPIMAGE lpImage)
/***********************************************************************/
{       
    if (hWnd && lpImage)
    {
        CWnd *pWnd = CWnd::FromHandle(hWnd);             
        ASSERT(!pWnd->IsKindOf(RUNTIME_CLASS(CServerView)));
        if (!pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))
            SetWindowLong(hWnd, GWL_IMAGEPTR, (long)lpImage);
    }            
}

/***********************************************************************/
void SetDisplayPtr(HWND hWnd, LPDISPLAY lpDisplay)
/***********************************************************************/
{
    if (hWnd && lpDisplay)
    {   
        CWnd *pWnd = CWnd::FromHandle(hWnd);             
        ASSERT(!pWnd->IsKindOf(RUNTIME_CLASS(CServerView)));
        if (!pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))    
            SetWindowLong(hWnd, GWL_DISPLAYPTR, (long)lpDisplay);
    }        
}

/***********************************************************************/
LPDISPLAY GetActiveDisplay()
/***********************************************************************/
{
HWND hActiveWnd;

hActiveWnd = GetActiveDoc();
if (hActiveWnd)
    return(GetDisplayPtr(hActiveWnd));
else
    return(NULL);
}

/***********************************************************************/
BOOL ImageIsBusy(LPIMAGE lpImage)
/***********************************************************************/
{
if (!lpImage)
    return(FALSE);
return(lpImage->IsBusy());
}

/***********************************************************************/
int NumImageWindows(LPIMAGE lpImage)
/***********************************************************************/
{
	int iCount = 0;
	
	CServerDoc* pDoc = PictPubApp.GetDocument(lpImage);
	if (pDoc)
	{
		POSITION ViewPos = pDoc->GetFirstViewPosition();
		while (ViewPos)
		{
    		CView* pView = pDoc->GetNextView( ViewPos );
    		if (pView)
				++iCount;
		}
	}
	return(iCount);
}

/***********************************************************************/
BOOL IsOnImage(HWND hWnd, int x, int y)
/***********************************************************************/
{
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);
if (lpDisplay)
	return(ONIMAGE(lpDisplay, x, y));
else
	return(FALSE);
}

/***********************************************************************/
BOOL IsInFile(HWND hWnd, int x, int y)
/***********************************************************************/
{
LPIMAGE lpImage;

lpImage = GetImagePtr(hWnd);
if (lpImage)
	return(INFILE(lpImage, x, y));
else
	return(FALSE);
}

/***********************************************************************/
void EnableImageWindows(LPIMAGE lpImage, BOOL fEnable)
/***********************************************************************/
{
CServerDoc *pDoc = PictPubApp.GetDocument(lpImage);
if (pDoc)
{
	POSITION ViewPos = pDoc->GetFirstViewPosition();
	while (ViewPos)
	{
    	CView* pView = pDoc->GetNextView( ViewPos );
    	if (pView)
        	pView->EnableWindow(fEnable);
	}
}
}








