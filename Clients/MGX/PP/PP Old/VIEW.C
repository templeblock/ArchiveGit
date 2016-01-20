// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®PL1¯®FD1¯®BT0¯®TP0¯*/

#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

extern HWND hClientAstral;

/************************************************************************/
void Zoom( x, y, value, fIncremental, fAllowSizeChange )
/************************************************************************/
int x, y, value;
BOOL fIncremental;
BOOL fAllowSizeChange;
{
FIXED DispRate, FileRate;
int Percentage, OldPercentage, iWidth, iHeight, iImageWidth, iImageHeight, dx, dy;
HWND hWnd;
RECT ClientRect, WindowRect, DispRect;
DWORD dXY;

if (IsIconic(lpImage->hWnd))
    return;

// Compute the viewing percentage, if necessary
if ( !(Percentage = lpImage->lpDisplay->ViewPercentage) )
	Percentage = FMUL( 100, lpImage->lpDisplay->DispRate );
SaveLastView();

// Compute the NEW viewing percentage, and DispRate
if ( fIncremental )
	{
	if (value < 0)
	    {
	    if (Percentage > 100)
		{
		if (Percentage % 100 != 0)
		    Percentage -= Percentage % 100;
		else
		    Percentage -= 100;
		}
	    else
 		{
		OldPercentage = Percentage;
		Percentage = 100;
		while (Percentage >= OldPercentage)
		    Percentage = (Percentage+1)/2;
	        }
	    }
	else if (value > 0)
 	    {
	    if (Percentage >= 50)
		Percentage = ((Percentage+100)/100) * 100;
	    else
		{
		OldPercentage = (Percentage * 2) + 20;
		Percentage = 100;
		while (Percentage > OldPercentage)
		    Percentage = (Percentage+1)/2;
		}
	    }
	}
else	
	Percentage  = value;

DispRate = FGET( Percentage, 100 );  /* DispRate */
FileRate = FGET( 100, Percentage );  /* FileRate */
iImageWidth = FMUL(lpImage->npix, DispRate);
iImageHeight = FMUL(lpImage->nlin, DispRate);
// below our minimum window size?
if (iImageWidth < MIN_WINDOW_SIZE || iImageHeight < MIN_WINDOW_SIZE)
	return;

lpImage->lpDisplay->ViewPercentage = Percentage;
lpImage->lpDisplay->FileRate = FileRate;

GetClientRect(lpImage->hWnd, &ClientRect);
iWidth = RectWidth(&ClientRect)-1; /* -1 cause Windows rects are different */
iHeight = RectHeight(&ClientRect)-1;
if (((iImageWidth < iWidth || iImageHeight < iHeight) ||
   (iImageWidth > iWidth || iImageHeight > iHeight)) && fAllowSizeChange)
	{
	dXY = GetWinXY(lpImage->hWnd);
	FullViewWindow(lpImage->hWnd, LOWORD(dXY), HIWORD(dXY), x, y, YES);
	}
else
	{
	ComputeDispRect();
	// Change the FileRect to accomodate
	ComputeFileRect( x, y, FileRate);
	SetupRulers();
	InvalidateRect( lpImage->hWnd, NULL, TRUE );
	}
}

/************************************************************************/
void ViewRect(lpRect)
/************************************************************************/
LPRECT lpRect;
{
DWORD dXY;

SaveLastView();
lpImage->lpDisplay->FileRect = *lpRect;
lpImage->lpDisplay->FileRate = 0;
lpImage->lpDisplay->ViewPercentage = 0;
dXY = GetWinXY(lpImage->hWnd);
FullViewWindow(lpImage->hWnd, LOWORD(dXY), HIWORD(dXY), 0, 0, NO);
}

/************************************************************************/
void NewImageSizeWindow( hWindow )
/************************************************************************/
HWND hWindow;
{
int stagger, x, y, cx, cy;

stagger = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME) - 2;
y = Control.ImageWhere.y + (((NumDocs()-1)%5)*stagger);
x = Control.ImageWhere.x + (((NumDocs()-1)%5)*stagger) + (((NumDocs()-1)/5)*stagger);
cx = (lpImage->lpDisplay->FileRect.left+lpImage->lpDisplay->FileRect.right)/2;
cy = (lpImage->lpDisplay->FileRect.top+lpImage->lpDisplay->FileRect.bottom)/2;
FullViewWindow(hWindow, x, y, cx, cy, YES);
}

/************************************************************************/
void FullViewWindow(hWindow, x, y, cx, cy, fClipToImage)
/************************************************************************/
HWND hWindow;
int x, y; /* client coordinates - hClientAstral */
int cx, cy;
BOOL fClipToImage;
{
RECT ClientRect, WindowRect;
int dx, dy, iWidth, iHeight, iImageWidth, iImageHeight;
FIXED DispRate;
DWORD dXY;

/* if window is iconic, do nothing here */
if (IsIconic(hWindow))
    return;

/* if window is maximized, minimize before we might resize */
if (IsZoomed(hWindow))
    {
    SendMessage(hClientAstral, WM_MDIRESTORE, hWindow, 0L);
    dXY = GetWinXY(hWindow);
    x = LOWORD(dXY);
    y = HIWORD(dXY);
    }

/* get the difference between the client area and the window area */
GetRealClientRect(hWindow, &ClientRect);
GetWindowRect(hWindow, &WindowRect);
dx = RectWidth(&WindowRect) - RectWidth(&ClientRect);
dy = RectHeight(&WindowRect) - RectHeight(&ClientRect);

/* determine maximum area for window to occupy */
GetClientRect(hClientAstral, &ClientRect);
ClientRect.top = y;
ClientRect.left = x;
ClientRect.right -= (dx+GetSystemMetrics(SM_CXFRAME));
ClientRect.bottom -= (dy+GetSystemMetrics(SM_CYFRAME));

/* start with this maximum size */
iWidth = RectWidth(&ClientRect);
iHeight = RectHeight(&ClientRect);

if (lpImage->lpDisplay->FileRate)
    {
    /* if we already have a FileRate, calculate new size for window */
    /* from image size */
    DispRate = FGET(UNITY, lpImage->lpDisplay->FileRate);
    iImageWidth = FMUL(lpImage->npix, DispRate);
    iImageHeight = FMUL(lpImage->nlin, DispRate);
    if (lpImage->lpDisplay->HasRulers)
	{
	iImageWidth += RULER_SIZE;
	iImageHeight += RULER_SIZE;
	}
    if (iImageWidth < iWidth)
	iWidth = iImageWidth;
    if (iImageHeight < iHeight)
    	iHeight = iImageHeight;
    }
else
    {
    /* if we have no FileRate, calculate window size based on FileRect */
    /* I think this is what happens for full view or view all */
    iImageWidth = RectWidth(&lpImage->lpDisplay->FileRect);
    iImageHeight = RectHeight(&lpImage->lpDisplay->FileRect);
    if (lpImage->lpDisplay->HasRulers)
	{
	iImageWidth += RULER_SIZE;
	iImageHeight += RULER_SIZE;
	}
    ScaleToFit(&iWidth, &iHeight, iImageWidth, iImageHeight);
    if (iImageWidth <= iWidth && iImageHeight <= iHeight && fClipToImage)
        {
        iWidth = iImageWidth;
        iHeight = iImageHeight;
        }
    }
/* Calculate new ClientRect based on iWidth and iHeight */
ClientRect.right = ClientRect.left + iWidth + dx - 1;
ClientRect.bottom = ClientRect.top + iHeight + dy - 1;

/* Setup DispRect for this new window size */
if (lpImage->lpDisplay->HasRulers)
	{
	iWidth -= RULER_SIZE;
	iHeight -= RULER_SIZE;
	}
lpImage->lpDisplay->DispRect.top = 0;
lpImage->lpDisplay->DispRect.left = 0;
lpImage->lpDisplay->DispRect.bottom = iHeight - 1;
lpImage->lpDisplay->DispRect.right = iWidth - 1;
/* if we had a FileRate coming in, then somebody wants a specific */
/* viewing percentage, so calculate a new FileRect */
if (lpImage->lpDisplay->FileRate)
    {
    ComputeFileRect(cx, cy, lpImage->lpDisplay->FileRate);
    lpImage->lpDisplay->FileRate = 0; /* set so FileRect won't get */
				      /* recalculated in WM_SIZE */
    }
SetupRulers();
/* now actually size the window, it's possible nothing will happen here */
MoveWindow(hWindow, ClientRect.left, ClientRect.top,
	RectWidth(&ClientRect), RectHeight(&ClientRect), TRUE);
/* if size changed, cause image to get repainted (our RectWidth is */
/* different than windows, hence the -1 */
if (RectWidth(&ClientRect) == (RectWidth(&WindowRect)-1) &&
    RectHeight(&ClientRect) == (RectHeight(&WindowRect)-1))
    {
    InvalidateRect(hWindow, NULL, TRUE);
    }
}

/************************************************************************/
ComputeDispRect()
/************************************************************************/
{
int w, h;
int dx, dy;
int iWidth, iHeight, iImageWidth, iImageHeight, iNewHeight, iNewWidth;
FIXED DispRate, AspectRatio;
RECT DispRect;

// get max size of client area with no scrollbars
GetDispRect(&DispRect, 1, 1);
w = RectWidth(&DispRect);
h = RectHeight(&DispRect);
if (lpImage->lpDisplay->FileRate == 1)
	{
	iWidth = w;
	iHeight = h;

	/* if we have no FileRate, calculate window size based on FileRect */
	/* I think this is what happens for full view or view all */
	iImageWidth = lpImage->npix;
	iImageHeight = lpImage->nlin;
        if (iImageWidth < iWidth)
		iWidth = iImageWidth;
	if (iImageHeight < iHeight)
		iHeight = iImageHeight;
	AspectRatio = FGET(iImageWidth, iImageHeight);
	iNewWidth = FMUL(iHeight, AspectRatio);
	AspectRatio = FGET(iImageHeight, iImageWidth);
	iNewHeight = FMUL(iWidth, AspectRatio);
	if (iNewWidth < iWidth)
		iWidth = iNewWidth;
	else if (iNewHeight < iHeight)
		iHeight = iNewHeight;
	}
else if (lpImage->lpDisplay->FileRate)
	{
    	DispRate = FGET(UNITY, lpImage->lpDisplay->FileRate);
    	iImageWidth = FMUL(lpImage->npix, DispRate);
    	iImageHeight = FMUL(lpImage->nlin, DispRate);
	if (iImageWidth > w && iImageHeight > h)
		{
		GetDispRect(&DispRect, 2, 2);
		}
	else if (iImageWidth > w)
		{
		GetDispRect(&DispRect, 2, 1);
		h = RectHeight(&DispRect);
		if (iImageHeight > h)
			{
			GetDispRect(&DispRect, 2, 2);
			}
		}
	else if (iImageHeight > h)
		{
		GetDispRect(&DispRect, 1, 2);
		w = RectWidth(&DispRect);
		if (iImageWidth > w)
			{
			GetDispRect(&DispRect, 2, 2);
			}
		}
	w = RectWidth(&DispRect);
	h = RectHeight(&DispRect);
	iWidth = w;
	iHeight = h;

	/* if we already have a FileRate, calculate new size for window */
	/* from image size */
    	if (iImageWidth < iWidth)
		iWidth = iImageWidth;
    	if (iImageHeight < iHeight)
		iHeight = iImageHeight;
    	}
else // should never need scrollbars
	{
	iWidth = w;
	iHeight = h;

	/* if we have no FileRate, calculate window size based on FileRect */
	/* I think this is what happens for full view or view all */
	iImageWidth = RectWidth(&lpImage->lpDisplay->FileRect);
	iImageHeight = RectHeight(&lpImage->lpDisplay->FileRect);
	AspectRatio = FGET(iImageWidth, iImageHeight);
	iNewWidth = FMUL(iHeight, AspectRatio);
	AspectRatio = FGET(iImageHeight, iImageWidth);
	iNewHeight = FMUL(iWidth, AspectRatio);
	if (iNewWidth < iWidth)
		iWidth = iNewWidth;
	else if (iNewHeight < iHeight)
		iHeight = iNewHeight;
	}

/* Setup DispRect for this new window size */
lpImage->lpDisplay->DispRect.top = DispRect.top;
lpImage->lpDisplay->DispRect.left = DispRect.left;
lpImage->lpDisplay->DispRect.bottom = lpImage->lpDisplay->DispRect.top + iHeight - 1;
lpImage->lpDisplay->DispRect.right = lpImage->lpDisplay->DispRect.left + iWidth - 1;
}

/*************************************************************************/
GetDispRect( lpRect, iHorzMode, iVertMode )
/************************************************************************/
LPRECT lpRect;
int iHorzMode; /* 0 = GetClientRect 1 = No Scrollbars 2 = With Scrollbars */
int iVertMode; /* 0 = GetClientRect 1 = No Scrollbars 2 = With Scrollbars */
{
RECT rHorz, rVert;
int cy, cx;

GetScrollBarSize(&cx, &cy);
if (iHorzMode == 2) 		// with horizontal scrollbar
	{
	GetRealClientRect(lpImage->hWnd, &rHorz);
	rHorz.bottom -= cy;
	}
else if (iHorzMode == 1) 	// without horizontal scrollbar
	{
	GetRealClientRect(lpImage->hWnd, &rHorz);
	}
else	
	{			// actual client size
	GetClientRect(lpImage->hWnd, &rHorz);
	}
if (iVertMode == 2)		// with vertical scrollbar
	{
	GetRealClientRect(lpImage->hWnd, &rVert);
	rVert.right -= cx;
	}
else if (iVertMode == 1)	// without horizontal scrollbar
	{
	GetRealClientRect(lpImage->hWnd, &rVert);
	}
else				// actual client size
	{
	GetClientRect(lpImage->hWnd, &rVert);
	}

lpRect->top = rHorz.top;
lpRect->bottom = rHorz.bottom;
lpRect->left = rVert.left;
lpRect->right = rVert.right;

if (lpImage->lpDisplay->HasRulers)
	{
	lpRect->top += RULER_SIZE;
	lpRect->left += RULER_SIZE;
	}

// adjust because our rects include right and bottom
lpRect->bottom -= 1;
lpRect->right -= 1;
}

/************************************************************************/
ComputeFileRect( x, y, FileRate )
/************************************************************************/
int x, y;
FIXED FileRate;
{
int fx, fy, DispWidth, DispHeight, dx, dy;
FIXED xRate, yRate, DispRate;

// A NULL FileRate is a signal that FileRect was just initialized
// and doesn't need recalculating
if ( !FileRate)
	return;

// FileRect will be recomputed from FileRate and a center point (x,y)
DispWidth = RectWidth( &lpImage->lpDisplay->DispRect );
DispHeight = RectHeight( &lpImage->lpDisplay->DispRect );

DispRate = FGET(UNITY, lpImage->lpDisplay->FileRate);
dx = FMUL(lpImage->npix, DispRate);
dy = FMUL(lpImage->nlin, DispRate);
// FileRate == 1 is a special case for handling iconic state
// otherwise handle normally
if ( FileRate == 1) // Display all of image - iconic state
	{
	xRate = FGET( lpImage->npix, DispWidth );
	yRate = FGET( lpImage->nlin, DispHeight );
	if ( xRate < yRate )
		FileRate = xRate;
	else	FileRate = yRate;
	fx = FMUL( DispWidth, FileRate );
	fy = FMUL( DispHeight, FileRate );
	lpImage->lpDisplay->ViewPercentage = 0;
	}
else 
	{
	// if image fits or is smaller than display surface
        // use maximum FileRect, otherwise use current FileRate
	if (dx <= DispWidth)
		fx = lpImage->npix;
	else
		fx = FMUL( DispWidth, FileRate );
	if (dy <= DispHeight)
		fy = lpImage->nlin;
	else
		fy = FMUL( DispHeight, FileRate );
	}

x -= ( fx / 2 );
y -= ( fy / 2 );
if ( x < 0 )				x = 0;
if ( y < 0 )				y = 0;
if ( x + fx - 1 >= lpImage->npix )	x = lpImage->npix - fx;
if ( y + fy - 1 >= lpImage->nlin )	y = lpImage->nlin - fy;
lpImage->lpDisplay->FileRect.left   = x;
lpImage->lpDisplay->FileRect.top    = y;
lpImage->lpDisplay->FileRect.right  = x + fx - 1;
lpImage->lpDisplay->FileRect.bottom = y + fy - 1;
BoundRect( &lpImage->lpDisplay->FileRect, 0, 0, lpImage->npix-1, lpImage->nlin-1 );
}

/************************************************************************/
void ScrollImage( hWnd, msg, wParam, lParam )
/************************************************************************/
HWND	 hWnd;
unsigned msg;
WORD	 wParam;
LONG	 lParam;
{
int y, fy, dy, ny, sy;
int x, fx, dx, nx, sx;
LPROC lpProc;
RECT DispRect;

#define SCROLLPIXELS 10

lpProc = lpImage->lpDisplay->FloatingGraphicsProc;

if (lpImage->hWnd == hWnd)
    DisplayBrush(0, 0, 0, OFF);
DispRect = lpImage->lpDisplay->DispRect;
DispRect.bottom += 1;
DispRect.right += 1;
switch (msg)
    {
    case WM_VSCROLL:
        dy = lpImage->nlin - RectHeight( &lpImage->lpDisplay->FileRect );
        if (dy <= 0)
	    break;
	sy = GetScrollPos( hWnd, SB_VERT );
	switch ( wParam )
	    {
	    case SB_TOP:
	    case SB_BOTTOM:
		MessageBeep(0);
		break;

	    case SB_PAGEUP:
	    case SB_PAGEDOWN:
	    case SB_LINEUP:
	    case SB_LINEDOWN:
		dy = SCROLLPIXELS;
		if ( wParam == SB_PAGEUP || wParam == SB_PAGEDOWN )
			dy *= 10;
		if ( !(fy = FMUL( dy, lpImage->lpDisplay->FileRate )) )
			fy = 1;
		if ( wParam == SB_LINEDOWN || wParam == SB_PAGEDOWN )
			dy = (lpImage->nlin - 1) - lpImage->lpDisplay->FileRect.bottom;
		else
		// ( wParam == SB_LINEUP || wParam == SB_PAGEUP )
			{
			fy = -fy;
			dy = 0 - lpImage->lpDisplay->FileRect.top;
			}
		if ( !dy )
			break;
		if ( abs(fy) > abs(dy) )
			fy = dy;
		AstralUpdateWindow(hWnd);
		if (lpProc)
			(*lpProc)(Window.hDC, NULL, OFF);
		lpImage->lpDisplay->FloatingGraphicsProc = NULL;
		x = 0;
		y = ny = lpImage->lpDisplay->FileRect.bottom;
		File2Display( &x, &y );
		lpImage->lpDisplay->FileRect.top += fy;
		lpImage->lpDisplay->FileRect.bottom += fy;
		File2Display( &x, &ny );
		dy = ny - y;
		SetupRulers();
		SetScrollPos( hWnd, SB_VERT, sy+fy, YES );
		ScrollWindow( hWnd, 0 /*x*/, dy, &DispRect, &DispRect );
		lpImage->lpDisplay->yDiva -= dy;
		AstralUpdateWindow(hWnd);
		if (lpProc)
			(*lpProc)(Window.hDC, NULL, ON);
		lpImage->lpDisplay->FloatingGraphicsProc = lpProc;
			
		break;

	    case SB_THUMBPOSITION:
	    case SB_THUMBTRACK:
		fy = LOWORD(lParam);
		dy = sy - fy;
		if ( !dy )
			break;
		AstralUpdateWindow(hWnd);
		if (lpProc)
			(*lpProc)(Window.hDC, NULL, OFF);
		lpImage->lpDisplay->FloatingGraphicsProc = NULL;
		x = 0;
		y = ny = lpImage->lpDisplay->FileRect.top;
		File2Display( &x, &y );
		lpImage->lpDisplay->FileRect.top -= dy;
		lpImage->lpDisplay->FileRect.bottom -= dy;
		File2Display( &x, &ny );
		dy = ny - y;
		SetupRulers();
		SetScrollPos( hWnd, SB_VERT, fy, YES );
		ScrollWindow( hWnd, 0 /*x*/, dy, &DispRect, &DispRect );
		lpImage->lpDisplay->yDiva -= dy;
		AstralUpdateWindow(hWnd);
		if (lpProc)
			(*lpProc)(Window.hDC, NULL, ON);
		lpImage->lpDisplay->FloatingGraphicsProc = lpProc;
		break;
	    }
	break;

    case WM_HSCROLL:
        dx = lpImage->npix - RectWidth( &lpImage->lpDisplay->FileRect );
	if (dx <= 0)
	    break;
	sx = GetScrollPos( hWnd, SB_HORZ );
	switch ( wParam )
	    {
	    case SB_TOP:
	    case SB_BOTTOM:
		MessageBeep(0);
		break;

	    case SB_PAGEUP:
	    case SB_PAGEDOWN:
	    case SB_LINEUP:
	    case SB_LINEDOWN:
		dx = SCROLLPIXELS;
		if ( wParam == SB_PAGEUP || wParam == SB_PAGEDOWN )
			dx *= 10;
		if ( !(fx = FMUL( dx, lpImage->lpDisplay->FileRate )) )
			fx = 1;
		if ( wParam == SB_LINEDOWN || wParam == SB_PAGEDOWN )
			dx = (lpImage->npix - 1) - lpImage->lpDisplay->FileRect.right;
		else
		// ( wParam == SB_LINEUP || wParam == SB_PAGEUP )
			{
			fx = -fx;
			dx = 0 - lpImage->lpDisplay->FileRect.left;
			}
		if ( !dx )
			break;
		if ( abs(fx) > abs(dx) )
			fx = dx;
		AstralUpdateWindow(hWnd);
		if (lpProc)
			(*lpProc)(Window.hDC, NULL, OFF);
		lpImage->lpDisplay->FloatingGraphicsProc = NULL;
		y = 0;
		x = nx = lpImage->lpDisplay->FileRect.right;
		File2Display( &x, &y );
		lpImage->lpDisplay->FileRect.left += fx;
		lpImage->lpDisplay->FileRect.right += fx;
		File2Display( &nx, &y );
		dx = nx - x;
		SetupRulers();
		SetScrollPos( hWnd, SB_HORZ, sx+fx, YES );
		ScrollWindow( hWnd, dx, 0 /*y*/, &DispRect, &DispRect );
		lpImage->lpDisplay->xDiva -= dx;
		AstralUpdateWindow(hWnd);
		if (lpProc)
			(*lpProc)(Window.hDC, NULL, ON);
		lpImage->lpDisplay->FloatingGraphicsProc = lpProc;
		break;

	    case SB_THUMBPOSITION:
	    case SB_THUMBTRACK:
		fx = LOWORD(lParam);
		dx = sx - fx;
		if ( !dx )
			break;
		AstralUpdateWindow(hWnd);
		if (lpProc)
			(*lpProc)(Window.hDC, NULL, OFF);
		lpImage->lpDisplay->FloatingGraphicsProc = NULL;
		y = 0;
		x = nx = lpImage->lpDisplay->FileRect.left;
		File2Display( &x, &y );
		lpImage->lpDisplay->FileRect.left -= dx;
		lpImage->lpDisplay->FileRect.right -= dx;
		File2Display( &nx, &y );
		dx = nx - x;
		SetupRulers();
		SetScrollPos( hWnd, SB_HORZ, fx, YES );
		ScrollWindow( hWnd, dx, 0 /*y*/, &DispRect, &DispRect );
		lpImage->lpDisplay->xDiva -= dx;
		AstralUpdateWindow(hWnd);
		if (lpProc)
			(*lpProc)(Window.hDC, NULL, ON);
		lpImage->lpDisplay->FloatingGraphicsProc = lpProc;
		break;
	    }
	break;

    case WM_KEYDOWN:
	/* Translate keyboard messages to scroll commands */
	switch (wParam)
	    {
	    case VK_UP:    PostMessage (hWnd, WM_VSCROLL, SB_LINEUP,   0L);
		break;
	    case VK_DOWN:  PostMessage (hWnd, WM_VSCROLL, SB_LINEDOWN, 0L);
		break;
	    case VK_PRIOR: PostMessage (hWnd, WM_VSCROLL, SB_PAGEUP,   0L);
		break;
	    case VK_NEXT:  PostMessage (hWnd, WM_VSCROLL, SB_PAGEDOWN, 0L);
		break;
	    case VK_HOME:  PostMessage (hWnd, WM_HSCROLL, SB_PAGEUP,   0L);
		break;
	    case VK_END:   PostMessage (hWnd, WM_HSCROLL, SB_PAGEDOWN, 0L);
		break;
	    case VK_LEFT:  PostMessage (hWnd, WM_HSCROLL, SB_LINEUP,   0L);
		break;
	    case VK_RIGHT: PostMessage (hWnd, WM_HSCROLL, SB_LINEDOWN, 0L);
		break;
	    }
	break;

    case WM_KEYUP:
	switch (wParam)
	    {
	    case VK_UP:
	    case VK_DOWN:
	    case VK_PRIOR:
	    case VK_NEXT:
		  PostMessage (hWnd, WM_VSCROLL, SB_ENDSCROLL, 0L);
		  break;

	    case VK_HOME:
	    case VK_END:
	    case VK_LEFT:
	    case VK_RIGHT:
		  PostMessage (hWnd, WM_HSCROLL, SB_ENDSCROLL, 0L);
		  break;
	    }
	break;
    }
}

/************************************************************************/
void SaveLastView()
/************************************************************************/
{
lpImage->lpDisplay->LastWindowRect = lpImage->lpDisplay->WindowRect;
//GetWindowRect(lpImage->hWnd, &lpImage->lpDisplay->LastWindowRect);
lpImage->lpDisplay->LastFileRect = lpImage->lpDisplay->FileRect;
lpImage->lpDisplay->LastFileRate = lpImage->lpDisplay->FileRate;
lpImage->lpDisplay->LastViewPercentage = lpImage->lpDisplay->ViewPercentage;
}

/************************************************************************/
void RevertLastView()
/************************************************************************/
{
int iWidth, iHeight;
RECT FileRect, WindowRect;
FIXED FileRate;
int ViewPercentage;
POINT UpperLeft;

if (IsIconic(lpImage->hWnd))
    return;
if (IsZoomed(lpImage->hWnd))
    SendMessage(hClientAstral, WM_MDIRESTORE, lpImage->hWnd, 0L);

FileRect = lpImage->lpDisplay->FileRect;
lpImage->lpDisplay->FileRect = lpImage->lpDisplay->LastFileRect;
lpImage->lpDisplay->LastFileRect = FileRect;
FileRate = lpImage->lpDisplay->FileRate;
lpImage->lpDisplay->FileRate = lpImage->lpDisplay->LastFileRate;
lpImage->lpDisplay->LastFileRate = FileRate;
ViewPercentage = lpImage->lpDisplay->ViewPercentage;
lpImage->lpDisplay->ViewPercentage = lpImage->lpDisplay->LastViewPercentage;
lpImage->lpDisplay->LastViewPercentage = ViewPercentage;
ComputeDispRect();

GetWindowRect(lpImage->hWnd, &WindowRect);
if (!EqualRect(&lpImage->lpDisplay->LastWindowRect, &WindowRect))
    {
    iWidth = RectWidth(&lpImage->lpDisplay->LastWindowRect)-1;
    iHeight = RectHeight(&lpImage->lpDisplay->LastWindowRect)-1;
    UpperLeft.x = lpImage->lpDisplay->LastWindowRect.left;
    UpperLeft.y = lpImage->lpDisplay->LastWindowRect.top;
    ScreenToClient(GetParent(lpImage->hWnd), &UpperLeft);

    MoveWindow(lpImage->hWnd, UpperLeft.x, UpperLeft.y,
	iWidth, iHeight, TRUE);
    lpImage->lpDisplay->LastWindowRect = WindowRect;
    }
else
    {
    InvalidateRect(lpImage->hWnd, NULL, TRUE);
    }
}

