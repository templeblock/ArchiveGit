//®PL1¯®FD1¯®BT0¯®TP0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Static Prototypes
static void ScrollStart( HWND hWnd );
static void ScrollEnd( HWND hWnd );
static int ComputeZoom(int x,int y, int value, BOOL fIncremental, LFIXED *lfFileRate, int *Percentage );

extern HINSTANCE hInstAstral;
extern HWND hWndAstral;
extern HWND hClientAstral;

/***********************************************************************/
BOOL NewImageWindow(
/***********************************************************************/
LPIMAGE 	lpOldImage,
LPSTR 		szName,
LPFRAME 	lpFrame,
int 		FileType,
int 		DataType,
BOOL 		fNewView,
int 		DocumentType,
LPSTR 		ImageName,
int 		iDoZoom)
{
int cx, cy;
MDICREATESTRUCT mcs;
DWORD dXY;
LPIMAGE lpNewImage;
LPDISPLAY lpDisplay;
RECT WindowRect;

#ifdef ID_MAGIC
iDoZoom = YES; // JUST FOR JUNIOR
#endif

if ( !lpFrame )
	return( FALSE );

lpNewImage = lpOldImage;
if ( !lpNewImage || fNewView ) // If this a really a new image...
	{ // Allocate and clear a new image structure
	if (!fNewView)
		{
		if ( !(lpNewImage = (LPIMAGE)Alloc( (long)sizeof(IMAGE) )) )
			return( FALSE );
		clr( (LPTR)lpNewImage, sizeof(IMAGE) );
		}
	if ( !(lpNewImage->lpNewDisplay = 
		 (LPDISPLAY)Alloc( (long)sizeof(DISPLAY) )) )
		return( FALSE );
	clr ((LPTR)lpNewImage->lpNewDisplay, sizeof(DISPLAY));
	lpDisplay = lpNewImage->lpNewDisplay;
	}
else
	lpDisplay = lpNewImage->lpDisplay;

frame_set( lpFrame );
if (!fNewView)
	{
	if (!lpOldImage)
		{
		if (!ImgAddFrameObject(lpNewImage, lpFrame, NULL, NULL))
			{
			FreeUp((LPTR)lpNewImage->lpNewDisplay);
			FreeUp((LPTR)lpNewImage);
			return(FALSE);
			}
		lpNewImage->UseObjectMarquee = Control.UseObjectMarquee;
		}
	if ( !szName )
		{
		lpNewImage->fUntitled = TRUE;
		lstrcpy( lpNewImage->CurFile, GetUntitledName() );
		}
	else
        {
        // set this anyway so we can assign a new name at save time
		lpNewImage->fUntitled = TRUE;
		lstrcpy( lpNewImage->CurFile, szName );
        }

	SetupImagePalette(lpNewImage, YES, NO);

	lpNewImage->DataType  = DataType;
	lpNewImage->npix	  = FrameXSize(lpFrame);
	lpNewImage->nlin	  = FrameYSize(lpFrame);
	lpNewImage->FileType  = FileType;
	lpNewImage->DocumentType = DocumentType;
	if (ImageName)
		lstrcpy(lpNewImage->ImageName, ImageName);
	}

lpDisplay->FileRate = 1;
lpDisplay->ViewPercentage = 0;
SetRect( &lpDisplay->FileRect, 0,0,FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1);
SetRectEmpty( &lpDisplay->UpdateRect );

Edit.Crop = lpDisplay->FileRect;
if ( lpOldImage && !fNewView)
	{ // Resize and redisplay the window
	SaveLastView();
	dXY = GetWinXY(lpNewImage->hWnd);
	// always display from upper-left
	cx = cy = 0;
//	cx = (lpDisplay->FileRect.left + 
//		  lpDisplay->FileRect.right)/2;
//	cy = (lpDisplay->FileRect.top + 
//		  lpDisplay->FileRect.bottom)/2;
	FullViewWindow(lpNewImage->hWnd, LOWORD(dXY), HIWORD(dXY),
			cx, cy, YES);
	if ( Tool.hRibbon )
		SendMessage( Tool.hRibbon, WM_DOCCHANGED, (WPARAM)lpNewImage->hWnd, 0L );
	}
else
	{
	CalcFullViewWindowSize(&WindowRect, &lpDisplay->FileRect,
		lpDisplay->FileRate, lpNewImage->npix, lpNewImage->nlin,
		-1, -1, View.UseRulers, YES);
	mcs.szTitle = (LPSTR)"";
	mcs.szClass = (LPSTR)"ppimage";
	mcs.hOwner = hInstAstral;
	mcs.x = WindowRect.left;
	mcs.y = WindowRect.top;
	mcs.cx = RectWidth(&WindowRect);
	mcs.cy = RectHeight(&WindowRect);
	mcs.lParam = (long)lpNewImage;

	/* Set the style DWORD of the window to default */
	mcs.style =
		WS_VISIBLE |
		WS_CHILD |
		WS_CLIPSIBLINGS |
		WS_CLIPCHILDREN |
//		WS_BORDER |
//		WS_THICKFRAME |
//		WS_CAPTION |
//		WS_SYSMENU |
//		WS_MINIMIZEBOX |
//		WS_MAXIMIZEBOX |
		WS_VSCROLL |
		WS_HSCROLL;
	if (iDoZoom == MAYBE)
		{
		if (lpImage && IsZoomed(lpImage->hWnd))
			mcs.style |= WS_MAXIMIZE;
		}
	else
	if (iDoZoom == YES)
		mcs.style |= WS_MAXIMIZE;

	// Only one image at a time
	SendMessage( hWndAstral, WM_COMMAND, IDM_CLOSE, 0L );

	/* tell the MDI Client to create the child */
	SendMessage (hClientAstral, WM_MDICREATE, 0,
		(LONG)(LPMDICREATESTRUCT)&mcs);
	if ( lpImage && Tool.hRibbon )
		SendMessage( Tool.hRibbon, WM_DOCCHANGED, (WPARAM)lpImage->hWnd, 0L );
	}

return( TRUE );
}


/************************************************************************/
void ViewXY(int x, int y)
/************************************************************************/
{
if (!lpImage)
	return;
ComputeFileRect(x, y, lpImage->lpDisplay->FileRate);
SetupRulers();
InvalidateRect( lpImage->hWnd, NULL, TRUE );
}


/************************************************************************/
void ViewRect(LPRECT lpRect,  BOOL fResizeWindow)
/************************************************************************/
{
DWORD dXY;
int x, y;
RECT rMax;

if (fResizeWindow) // if allowed to change size use a max size
	GetClientRect(hClientAstral, &rMax);
else // if window size will not change use current DispRect
	rMax = lpImage->lpDisplay->DispRect;
if (!CanZoom(0L, lpRect, &rMax))
	return;
SaveLastView();
lpImage->lpDisplay->FileRect = *lpRect;
lpImage->lpDisplay->FileRate = 0;
lpImage->lpDisplay->ViewPercentage = 0;
x = (lpImage->lpDisplay->FileRect.left 
	+ lpImage->lpDisplay->FileRect.right+1) / 2;
y = ( lpImage->lpDisplay->FileRect.top 
	+ lpImage->lpDisplay->FileRect.bottom+1) / 2;
if (fResizeWindow)
	{
	dXY = GetWinXY(lpImage->hWnd);
	FullViewWindow(lpImage->hWnd, LOWORD(dXY), HIWORD(dXY), x, y, NO);
	}
else
	{
	ComputeDispRect();
	ComputeFileRect(x, y, lpImage->lpDisplay->FileRate);
	SetupRulers();
	InvalidateRect(lpImage->hWnd, NULL, TRUE);
	}
}

/************************************************************************/
void NewImageSizeWindow( HWND hWindow )
/************************************************************************/
{
int stagger, x, y, cx, cy;

stagger = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME) - 2;
y = Control.yImage + (((NumDocs()-1)%5)*stagger);
x = Control.xImage + (((NumDocs()-1)%5)*stagger) + (((NumDocs()-1)/5)*stagger);
cx = (lpImage->lpDisplay->FileRect.left+lpImage->lpDisplay->FileRect.right)/2;
cy = (lpImage->lpDisplay->FileRect.top+lpImage->lpDisplay->FileRect.bottom)/2;
FullViewWindow(hWindow, x, y, cx, cy, YES);
}


/************************************************************************/
void FullViewWindow(
/************************************************************************/
HWND 	hWindow,
int 	x,
int		y, /* client coordinates - hClientAstral */
int 	cx,
int		cy,
BOOL 	fClipToImage)
{
RECT NewWindowRect;

// if window is iconic, do nothing here
if (IsIconic(hWindow))
	return;

// if window is maximized, do not change the size of the window
if ( !IsZoomed(hWindow) )
	{
	// get the difference between the client area and the window area
	// not including scrollbars (i.e. - pretend scrollbars aren't there)
	// this is so we know how much extra size is needed for caption,
	// borders, etc. when we size the window
	CalcFullViewWindowSize( &NewWindowRect, &lpImage->lpDisplay->FileRect,
		lpImage->lpDisplay->FileRate, lpImage->npix, lpImage->nlin,
		x, y, lpImage->lpDisplay->HasRulers, fClipToImage );
	// now actually size the window, it's possible nothing will happen here
	MoveWindow(hWindow, NewWindowRect.left, NewWindowRect.top,
		RectWidth(&NewWindowRect), RectHeight(&NewWindowRect), TRUE);
	}

// recompute DispRect and FileRect for cx and cy
// and also it's possible there was no WM_SIZE
ComputeDispRect();
ComputeFileRect(cx, cy, lpImage->lpDisplay->FileRate );
SetupRulers();
InvalidateRect(hWindow, NULL, TRUE);
}


/************************************************************************/
void CalcFullViewWindowSize(
/************************************************************************/
LPRECT 	lpWindowRect,
LPRECT 	lpFileRect,
LFIXED 	FileRate,
int 	npix,
int 	nlin,
int 	x,
int		y,
BOOL 	fHasRulers,
BOOL	fClipToImage)
{
int dx, dy, sx, sy, rs, iImageWidth, iImageHeight, iWidth, iHeight, stagger;
LFIXED DispRate;
int minsize;

// normal window - to goal is to size the window to fit the view
// If FileRate == 0 or 1, then we make the window as big as possible
// for the FileRect provided.  If FileRate != 0, then we make the
// the window as big as possible to fit the image at that FileRate.
// If the fClipToImage flag and FileRate == 0, then we do not let
// the FileRate go over 100%.  This is used for a caller who wants
// to display the image pixel for pixel, if possible.
if (x < 0 || y < 0)
	{
	stagger = GetSystemMetrics(SM_CYCAPTION) + 			GetSystemMetrics(SM_CYFRAME) - 2;
	y =  GetSystemMetrics(SM_CYFRAME) + (((NumDocs())%5)*stagger);
	x = Control.xImage + (((NumDocs())%5)*stagger) + (((NumDocs())/5)*stagger);
	}

dx = 2 * GetSystemMetrics(SM_CXFRAME);
dy = GetSystemMetrics(SM_CYCAPTION) + (2*GetSystemMetrics(SM_CYFRAME)) - 1;

// get size of scrollbars and rulers for use later
GetScrollBarSize(&sx, &sy);
rs = GetRulerSize();

// determine maximum area for window to occupy
if (IsIconic(hWndAstral))
	GetClientRect(hWndAstral, lpWindowRect);
else
	GetClientRect(hClientAstral, lpWindowRect);
lpWindowRect->top = y;
lpWindowRect->left = x;
lpWindowRect->right -= dx;
lpWindowRect->bottom -= (dy + GetSystemMetrics(SM_CYFRAME));

// start with this maximum Client Area size
minsize = MIN_WINDOW_SIZE+rs;
iWidth = RectWidth(lpWindowRect);
if ( iWidth < minsize )
	iWidth = minsize;
iHeight = RectHeight(lpWindowRect);
if ( iHeight < minsize )
	iHeight = minsize;

// If we already have a FileRate, calculate new size of window
// based on the image size
if (FileRate > 1)
	{
	// calculate size of area for total image at this
	// FileRate
	DispRate = FGET(UNITY, FileRate);
	iImageWidth = FMUL(npix, DispRate);
	iImageHeight = FMUL(nlin, DispRate);

	// add in additional space for rulers if necessary
	if (fHasRulers)
		{
		iImageWidth += rs;
		iImageHeight += rs;
		}
	if (iImageWidth > iWidth && iImageHeight < iHeight)
		iImageHeight += sy;
	if (iImageHeight > iHeight && iImageWidth < iWidth)
		iImageWidth += sx;

	// see if we don't need the total width
	// otherwise, use the maximum
	if (iImageWidth < iWidth)
		iWidth = iImageWidth;
	// see if we don't need the total height
	// otherwise, use the maximum
	if (iImageHeight < iHeight)
		iHeight = iImageHeight;
	}
// if we have no FileRate, then somebody wants a specific
// FileRect, figure out how big the window needs to be and
// whether we need rulers
else
	{
	// get width and height of area to be displayed
	iImageWidth = RectWidth(lpFileRect);
	iImageHeight = RectHeight(lpFileRect);

	// reduce the maximum area if we have rulers
	if (fHasRulers)
		{
		iWidth -= rs;
		iHeight -= rs;
		}

	// reduce the maximum area if we need scrollbars
	if (lpFileRect->left != 0 ||
		lpFileRect->right != npix-1 ||
		lpFileRect->top != 0 ||
		lpFileRect->bottom != nlin-1)
		{
		iWidth -= sx;
		iHeight -= sy;
		}

	// scale maximum client area size (not including rulers and
	// scrollbars) to fit aspect ratio of FileRect
	DispRate = ScaleToFit(&iWidth, &iHeight, iImageWidth, iImageHeight);

	// if caller wants to clip to hires to achieve 100% view,
	// clip.  But only if hires is smaller
	if (iImageWidth <= iWidth && iImageHeight <= iHeight && fClipToImage)
		{
		iWidth = iImageWidth;
		iHeight = iImageHeight;
		}

	// add ruler size back into client area
	if (fHasRulers)
		{
		iWidth += rs;
		iHeight += rs;
		}

	// add scrollbar size back into client area
	if (lpFileRect->left != 0 ||
		lpFileRect->right != npix-1 ||
		lpFileRect->top != 0 ||
		lpFileRect->bottom != nlin-1)
		{
		iWidth += sx;
		iHeight += sy;
		}
	}
// Calculate new window size based on iWidth and iHeight */
lpWindowRect->right = lpWindowRect->left + iWidth + dx - 1;
lpWindowRect->bottom = lpWindowRect->top + iHeight + dy - 1;
}

/************************************************************************/
BOOL CanZoom(
/************************************************************************/
LFIXED		FileRate,
LPRECT 		lpFileRect,
LPRECT		lpDispRect)
{
LFIXED DispRate;
long lFileWidth, lFileHeight;
int	iDispWidth, iDispHeight;

if (FileRate)	// FileRate passed in
	DispRate = FGET(UNITY, FileRate);
else		// Displaying a FileRect
	{
	// calculate DispRate using this FileRect and
	// DispRect.
	iDispWidth = RectWidth(lpDispRect);
	iDispHeight = RectHeight(lpDispRect);
	DispRate = ScaleToFit(&iDispWidth, &iDispHeight,
				RectWidth(lpFileRect),
				RectHeight(lpFileRect));
	}
// Calculate eventual size in File coordinates using
// this DispRate.  This calculation is done by most
// of the viewing routines and has the possibility
// of overflow.  Usually in these routines when the
// calculation is made it is too late to turn back
// so those routines call CanZoom() before doing
// anything that is unreversible.
lFileWidth = FMUL(lpImage->npix, DispRate);
lFileHeight = FMUL(lpImage->nlin, DispRate);
// Although MAXINT is 32767, I decided to use 30000 just
// for some slop and my own paranoia
return(lFileWidth > 0L && lFileWidth <= 30000L &&
	   lFileHeight > 0L && lFileHeight <= 30000L);
}

/************************************************************************/
void ComputeDispRect()
/************************************************************************/
{
int w, h, dx, dy;
int iWidth, iHeight, iImageWidth, iImageHeight;
LFIXED DispRate;
RECT DispRect;
BOOL bZoomed, bIconic;

// get max size of client area with no scrollbars
GetDispRect(&DispRect, 1, 1);
w = RectWidth(&DispRect);
h = RectHeight(&DispRect);

// If we have no FileRate to this point, it means somebody wants
// to display a specific FileRect.  Calculate a FileRate so that
// we can calculate a DispRect.  This is necessary because it is
// possible (when the window is maximized for example) that the
// FileRect requested will not fill up the window, so it would
// need to be recalculated.  It is assumed that scrollbars would
// not be needrf and should not be needed if the calculation is
// done correctly, which hopefully it is.
if (!lpImage->lpDisplay->FileRate)
	{
	iWidth = w; iHeight = h;
	DispRate = ScaleToFit(&iWidth, &iHeight, RectWidth(&lpImage->lpDisplay->FileRect), RectHeight(&lpImage->lpDisplay->FileRect));
	lpImage->lpDisplay->FileRate = FGET(UNITY, DispRate);
	}
if (lpImage->lpDisplay->FileRate == 1) // Iconic
	{
	iWidth = w;
	iHeight = h;

	iImageWidth = lpImage->npix;
	iImageHeight = lpImage->nlin;
	DispRate = ScaleToFit(&iWidth, &iHeight, iImageWidth, iImageHeight);
	if (DispRate > FGET(1, 1))
		{
		DispRate = FGET(1, 1);
		iWidth = FMUL(iImageWidth, DispRate);
		iHeight = FMUL(iImageHeight, DispRate);
		}
	}
else // if (lpImage->lpDisplay->FileRate) // Specific FileRate
	{
	// Calulate how much space is needed at this FileRate
	DispRate = FGET(UNITY, lpImage->lpDisplay->FileRate);
	iImageWidth = FMUL(lpImage->npix, DispRate);
	iImageHeight = FMUL(lpImage->nlin, DispRate);

	// See if scrollbars will be necessary and get the
	// appropriate DispRect if so
	if (iImageWidth > w && iImageHeight > h)
		{
		GetDispRect(&DispRect, 2, 2);
		}
	else
	if (iImageWidth > w)
		{
		GetDispRect(&DispRect, 2, 1);
		h = RectHeight(&DispRect);
		if (iImageHeight > h)
			{
			GetDispRect(&DispRect, 2, 2);
			}
		}
	else
	if (iImageHeight > h)
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

	// Clip the DispRect size to the amount needed for image
	if (iImageWidth < iWidth)
		iWidth = iImageWidth;
	if (iImageHeight < iHeight)
		iHeight = iImageHeight;
	}

/* Setup DispRect for this new window size */
bIconic = IsIconic(lpImage->hWnd);
bZoomed = IsZoomed(lpImage->hWnd);
if (bZoomed || bIconic)
	{
	dy = RectHeight(&DispRect) - iHeight;
	dy /= 2;
	dx = RectWidth(&DispRect) - iWidth;
	dx /= 2;
	lpImage->lpDisplay->DispRect.top = DispRect.top + dy;
	lpImage->lpDisplay->DispRect.left = DispRect.left + dx;
	}
else
	{
	lpImage->lpDisplay->DispRect.top = DispRect.top;
	lpImage->lpDisplay->DispRect.left = DispRect.left;
	}
lpImage->lpDisplay->DispRect.bottom = lpImage->lpDisplay->DispRect.top + iHeight - 1;
lpImage->lpDisplay->DispRect.right = lpImage->lpDisplay->DispRect.left + iWidth - 1;
}


/*************************************************************************/
void GetDispRect(
/************************************************************************/
LPRECT 	lpRect,
int 	iHorzMode, /* 0 = GetClientRect 1 = No Scrollbars 2 = With Scrollbars */
int 	iVertMode) /* 0 = GetClientRect 1 = No Scrollbars 2 = With Scrollbars */
{
RECT rHorz, rVert;
int cy, cx;

GetScrollBarSize(&cx, &cy);
if (iHorzMode == 2) 		// with horizontal scrollbar
	{
	GetRealClientRect(lpImage->hWnd, &rHorz);
	rHorz.bottom -= cy;
	}
else
if (iHorzMode == 1) 	// without horizontal scrollbar
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
else
if (iVertMode == 1)	// without horizontal scrollbar
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

if (lpImage->lpDisplay->HasRulers && !IsIconic(lpImage->hWnd)) 
	{
	lpRect->top += GetRulerSize();
	lpRect->left += GetRulerSize();
	}

// adjust because our rects include right and bottom
lpRect->bottom -= 1;
lpRect->right -= 1;
}

/************************************************************************/
void ComputeFileRect(
/************************************************************************/
int 	x,
int		y,
LFIXED 	FileRate)
{
int fx, fy, DispWidth, DispHeight, dx, dy;
LFIXED DispRate;

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
	fx = lpImage->npix;
	fy = lpImage->nlin;
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

if (x == -1 && y == -1) // special signal to use old XY
	{
	x = lpImage->lpDisplay->FileRect.left;
	y = lpImage->lpDisplay->FileRect.top;
	}
else
	{
	x -= ( fx / 2 );
	y -= ( fy / 2 );
	}
if ( x < 0 ) x = 0;
if ( y < 0 ) y = 0;
if ( x + fx - 1 >= lpImage->npix )	x = lpImage->npix - fx;
if ( y + fy - 1 >= lpImage->nlin )	y = lpImage->nlin - fy;
lpImage->lpDisplay->FileRect.left   = x;
lpImage->lpDisplay->FileRect.top	= y;
lpImage->lpDisplay->FileRect.right  = x + fx - 1;
lpImage->lpDisplay->FileRect.bottom = y + fy - 1;
BoundRect( &lpImage->lpDisplay->FileRect, 0, 0, lpImage->npix-1, lpImage->nlin-1 );
}

/************************************************************************/
BOOL ScrollImage(
/************************************************************************/
HWND 	hWnd,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
int fy, dy, fx, dx;
RECT rScroll;
BOOL bScrollWindow;
#define SCROLLPIXELS 10

if (IsIconic(lpImage->hWnd))
	return( NO );

bScrollWindow = NO;
rScroll = lpImage->lpDisplay->DispRect;
rScroll.bottom++; rScroll.right++;
switch (msg)
	{
	case WM_VSCROLL:
	if ( lpImage->nlin - RectHeight( &lpImage->lpDisplay->FileRect ) <= 0 )
		break;
	switch ( wParam )
		{
		case SB_ENDSCROLL:
		ScrollEnd( hWnd );
		break;

		case SB_TOP:
		case SB_BOTTOM:
		MessageBeep(0);
		break;

		case SB_PAGEUP:
		case SB_PAGEDOWN:
		case SB_LINEUP:
		case SB_LINEDOWN:
		fy = SCROLLPIXELS;
		if ( wParam == SB_PAGEUP || wParam == SB_PAGEDOWN )
			fy *= 10;
		if ( !(fy = FMUL( fy, lpImage->lpDisplay->FileRate )) )
			fy = 1;
		if ( wParam == SB_LINEDOWN || wParam == SB_PAGEDOWN )
			{
			fy = -fy;
			dy = lpImage->lpDisplay->FileRect.bottom - lpImage->nlin + 1;
			}
		else
		// ( wParam == SB_LINEUP || wParam == SB_PAGEUP )
			dy = lpImage->lpDisplay->FileRect.top;
		if ( !dy )
			break;
		if ( abs(fy) > abs(dy) )
			fy = dy;
		if ( !(dy = FMUL( fy, lpImage->lpDisplay->DispRate )) )
			break;
		ScrollStart( hWnd );
		lpImage->lpDisplay->FileRect.top -= fy;
		lpImage->lpDisplay->FileRect.bottom -= fy;
		lpImage->lpDisplay->yDiva -= dy;
		ScrollWindow( hWnd, 0 /*x*/, dy, &rScroll, &rScroll );
		AstralUpdateWindow(hWnd);
		bScrollWindow = YES;
		break;

		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
		if ( !(fy = GetScrollPos( hWnd, SB_VERT ) - (int)LOWORD(lParam)) )
			break;
		if ( !(dy = FMUL( fy, lpImage->lpDisplay->DispRate )) )
			break;
		ScrollStart( hWnd );
		lpImage->lpDisplay->FileRect.top -= fy;
		lpImage->lpDisplay->FileRect.bottom -= fy;
		lpImage->lpDisplay->yDiva -= dy;
		ScrollWindow( hWnd, 0 /*x*/, dy, &rScroll, &rScroll );
		AstralUpdateWindow(hWnd);
		bScrollWindow = YES;
		break;
		}
	break;

	case WM_HSCROLL:
	if ( lpImage->npix - RectWidth( &lpImage->lpDisplay->FileRect ) <= 0 )
		break;
	switch ( wParam )
		{
		case SB_ENDSCROLL:
		ScrollEnd( hWnd );
		break;

		case SB_TOP:
		case SB_BOTTOM:
		MessageBeep(0);
		break;

		case SB_PAGEUP:
		case SB_PAGEDOWN:
		case SB_LINEUP:
		case SB_LINEDOWN:
		fx = SCROLLPIXELS;
		if ( wParam == SB_PAGEUP || wParam == SB_PAGEDOWN )
			fx *= 10;
		if ( !(fx = FMUL( fx, lpImage->lpDisplay->FileRate )) )
			fx = 1;
		if ( wParam == SB_LINEDOWN || wParam == SB_PAGEDOWN )
			{
			fx = -fx;
			dx = lpImage->lpDisplay->FileRect.right - lpImage->npix + 1;
			}
		else
		// ( wParam == SB_LINEUP || wParam == SB_PAGEUP )
			dx = lpImage->lpDisplay->FileRect.left;
		if ( !dx )
			break;
		if ( abs(fx) > abs(dx) )
			fx = dx;
		if ( !(dx = FMUL( fx, lpImage->lpDisplay->DispRate )) )
			break;
		ScrollStart( hWnd );
		lpImage->lpDisplay->FileRect.left -= fx;
		lpImage->lpDisplay->FileRect.right -= fx;
		lpImage->lpDisplay->xDiva -= dx;
		ScrollWindow( hWnd, dx, 0 /*y*/, &rScroll, &rScroll );
		AstralUpdateWindow(hWnd);
		bScrollWindow = YES;
		break;

		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
		if ( !(fx = GetScrollPos( hWnd, SB_HORZ ) - (int)LOWORD(lParam)) )
			break;
		if ( !(dx = FMUL( fx, lpImage->lpDisplay->DispRate )) )
			break;
		ScrollStart( hWnd );
		lpImage->lpDisplay->FileRect.left -= fx;
		lpImage->lpDisplay->FileRect.right -= fx;
		lpImage->lpDisplay->xDiva -= dx;
		ScrollWindow( hWnd, dx, 0 /*y*/, &rScroll, &rScroll );
		AstralUpdateWindow(hWnd);
		bScrollWindow = YES;
		break;
		}
	break;

	case WM_KEYDOWN:
	/* Translate keyboard messages to scroll commands */
	switch (wParam)
		{
		case VK_UP:	PostMessage (hWnd, WM_VSCROLL, SB_LINEUP,      0L);
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

return( bScrollWindow );
}

static BOOL bScrolling;
static LPDISPLAYHOOK lpProc;
/************************************************************************/
static void ScrollStart( HWND hWnd )
/************************************************************************/
{
if ( bScrolling )
	return;
// Rely on the fact that we always get an SB_ENDSCROLL when done
if ( hWnd == lpImage->hWnd )
	DisplayBrush(0, 0, 0, OFF);
if ( lpProc = GetDisplayHook(hWnd) )
	{ // Call the display hook and turn it off
	(*lpProc)( Window.hDC, NULL );
	SetDisplayHook( hWnd, NULL );
	}
EnableMarquee( NO );
RemoveMarquee();
AstralUpdateWindow( hWnd );
bScrolling = YES;
}


/************************************************************************/
static void ScrollEnd( HWND hWnd )
/************************************************************************/
{
if ( !bScrolling )
	return;
if ( lpProc )
	{ // Call the display hook and turn it back on
	(*lpProc)( Window.hDC, NULL );
	SetDisplayHook( hWnd, lpProc );
	lpProc = NULL;
	}
EnableMarquee( YES );
SetupRulers();
bScrolling = NO;
}


/************************************************************************/
void SaveLastView()
/************************************************************************/
{
lpImage->lpDisplay->LastWindowRect = lpImage->lpDisplay->WindowRect;
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
LFIXED FileRate;
int ViewPercentage;
POINT UpperLeft;

if (IsIconic(lpImage->hWnd))
	return;

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
ComputeFileRect(-1, -1, lpImage->lpDisplay->FileRate);
GetWindowRect(lpImage->hWnd, &WindowRect);
if (!IsZoomed(lpImage->hWnd) && !EqualRect(&lpImage->lpDisplay->LastWindowRect, &WindowRect))
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
	SetupRulers();
	InvalidateRect(lpImage->hWnd, NULL, TRUE);
	}
}


/************************************************************************/
void Zoom(
/************************************************************************/
int 	x,
int		y,
int		value,
BOOL 	fIncremental,
BOOL 	fAllowSizeChange)
{
LFIXED FileRate;
int Percentage;
DWORD dXY;

if (IsIconic(lpImage->hWnd))
	return;

if (!ComputeZoom(x,y,value,fIncremental, &FileRate, &Percentage))
	{
	Message( IDS_BADZOOM );
//	MessageBeep(0);
	return;
	}

SaveLastView();
lpImage->lpDisplay->ViewPercentage = Percentage;
lpImage->lpDisplay->FileRate = FileRate;
if (fAllowSizeChange)
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


/***********************************************************************
	ComputeZoom
		Computes Percentage and FileRate given the same inputs as Zoom
		Returns:
				TRUE if the reusult is valid.
				FALSE if the result would be illegal (Percent & FileRate are NOT valid) 
************************************************************************/
/************************************************************************/
static int ComputeZoom(
/************************************************************************/
int 	x,
int		y,
int		value,
BOOL 	fIncremental,
LFIXED 	*lfFileRate,
int 	*Percentage)
{
LFIXED DispRate;
int OldPercentage, iImageWidth, iImageHeight;

if (IsIconic(lpImage->hWnd))
	return(FALSE);

// Compute the viewing percentage, if necessary
if ( !(*Percentage = lpImage->lpDisplay->ViewPercentage) )
	*Percentage = FMUL( 100, lpImage->lpDisplay->DispRate );

// Compute the NEW viewing percentage, and DispRate
if ( fIncremental )
	{
	if (value < 0)
		{
		if (*Percentage > 100)
			{
			if (*Percentage % 100 != 0)
				*Percentage -= *Percentage % 100;
			else
				*Percentage -= 100;
			}
		else
		if (*Percentage <= 1)
			*Percentage = 0;
		else
	 		{
			OldPercentage = *Percentage;
			*Percentage = 100;
			while (*Percentage >= OldPercentage)
				*Percentage = (*Percentage+1)/2;
			}
		}
	else
	if (value > 0)
 		{
		if (*Percentage > 32600)
			*Percentage = 0;
		else
		if (*Percentage >= 50)
			*Percentage = ((*Percentage+100)/100) * 100;
		else
		if (*Percentage)
			{
			OldPercentage = (*Percentage * 2) + 1;
			*Percentage = 100;
			while (*Percentage > OldPercentage)
				*Percentage = (*Percentage+1)/2;
			}
		else
			*Percentage = 1;
		}
	}
else
if (value > 0 && value < 32700)
	*Percentage  = value;
else
	*Percentage = 0;

// do we have a valid zoom percentage?
if (!*Percentage)
	return(FALSE);

// can we mathematically support this percentage?
DispRate = FGET( *Percentage, 100 );  /* DispRate */
*lfFileRate = FGET( 100, *Percentage );  /* FileRate */
if (!CanZoom(*lfFileRate, &lpImage->lpDisplay->FileRect,
		&lpImage->lpDisplay->DispRect))
	{
	return(FALSE);
	}

// is our display surface too small?
iImageWidth = FMUL(lpImage->npix, DispRate);
iImageHeight = FMUL(lpImage->nlin, DispRate);
if (iImageHeight < 1 || iImageWidth < 1)
	{
	return(FALSE);
	}
return(TRUE);
}


/************************************************************************/
void ViewAll()
/************************************************************************/
{
DWORD dXY;
int x, y, width, height, rs;
RECT rClient;

if (IsIconic(lpImage->hWnd))
	return;

GetClientRect(lpImage->hWnd, &rClient);
width = rClient.right - rClient.left;
height = rClient.bottom - rClient.top;
if (lpImage->lpDisplay->HasRulers)
	{
	rs = GetRulerSize();
	width -= rs;
	height -= rs;
	}
if ((width <= 0 || height <= 0) && !View.ZoomWindow)
	return;

SaveLastView();
lpImage->lpDisplay->FileRect.top = 0;
lpImage->lpDisplay->FileRect.left = 0;
lpImage->lpDisplay->FileRect.bottom = lpImage->nlin - 1;
lpImage->lpDisplay->FileRect.right = lpImage->npix - 1;
lpImage->lpDisplay->FileRate = 1;
lpImage->lpDisplay->ViewPercentage = 0;
x = ( lpImage->lpDisplay->FileRect.left 
	+ lpImage->lpDisplay->FileRect.right+1 ) / 2;
y = ( lpImage->lpDisplay->FileRect.top 
	+ lpImage->lpDisplay->FileRect.bottom+1 ) / 2;
if (View.ZoomWindow)
	{
	dXY = GetWinXY(lpImage->hWnd);
	FullViewWindow(lpImage->hWnd, LOWORD(dXY), HIWORD(dXY), x, y, YES);
	}
else
	{
	ComputeDispRect();
	ComputeFileRect(x, y, lpImage->lpDisplay->FileRate);
	SetupRulers();
	InvalidateRect(lpImage->hWnd, NULL, TRUE);
	}
}


/***********************************************************************/
BOOL WINPROC EXPORT DlgFullScreenViewProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
RECT ClientRect;
int npix, nlin, cx, cy, dx, dy;
HDC hDC;
PAINTSTRUCT ps;
HWND hControl, hWndImage;
LPOBJECT lpBase;
LPFRAME lpFrame;

switch (msg)
	{
	case WM_INITDIALOG:
	if ( !lpImage )
		break;
	if ( !ImgGetBase(lpImage) )
		break;

	// Make the window as big as the screen
	dx = GetSystemMetrics( SM_CXSCREEN );
	dy = GetSystemMetrics( SM_CYSCREEN );
	MoveWindow( hDlg, 0, 0, dx, dy, NO );

	// Compute the center of the window
	GetClientRect( hDlg, &ClientRect );
	cx = ( ClientRect.right + ClientRect.left ) / 2;
	cy = ( ClientRect.bottom + ClientRect.top ) / 2;

	// Link all of the images to image controls
	hControl = GetDlgItem( hDlg, IDC_VIEWFULL );
	hWndImage = (HWND)SendMessage( hClientAstral, WM_MDIGETACTIVE, 0, 0L );
	while ( hWndImage )
		{
		lpBase = ImgGetBase((LPIMAGE)GetWindowLong( hWndImage, GWL_IMAGEPTR ));
		if ( !lpBase || !hControl )
			continue;
		lpFrame = ObjGetEditFrame(lpBase);

		// Link the frame to the image control
		SetWindowLong( hControl, GWL_IMAGE, (long)lpBase );

		// The destination size can be no bigger the the screen
		npix = FrameXSize(lpFrame);
		nlin = FrameYSize(lpFrame);
		if ( npix > dx || nlin > dy )
			{
			npix = dx;
			nlin = dy;
			ScaleToFit( &npix, &nlin,
				FrameXSize(lpFrame), FrameYSize(lpFrame));
			}

		// Position the control in the center of the window
		ClientRect.left = cx - npix/2;
		ClientRect.right = ClientRect.left + npix;
		ClientRect.top = cy - nlin/2;
		ClientRect.bottom = ClientRect.top + nlin;
		MoveWindow( hControl,
			ClientRect.left, ClientRect.top, // New position
			RectWidth( &ClientRect ),
			RectHeight( &ClientRect ), // New size
			NO /* No repaint*/ );
		// Check to see if these get destroyed when the window does
		do	{
			hWndImage = GetWindow( hWndImage, GW_HWNDNEXT );
			} while (hWndImage && !IsDoc(hWndImage));

		if (!hWndImage)
			break;
		// Get another image control
		hControl = CopyWindow( hControl );
		}
	break;

	case WM_QUERYNEWPALETTE:
	return(HandleQueryNewPalette(hDlg, NO, NO));

	case WM_PALETTECHANGED:
	if (IsOurWindow((HWND)wParam))
		SendMessageToChildren(hDlg, msg, wParam, lParam);
	break;

	case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, IDD_VIEWFULL ) );

	case WM_ERASEBKGND:
	break; // handle the message and do nothing

	case WM_PAINT:
	hDC = BeginPaint( hDlg, &ps );
//	if ( ps.fErase )
		{ // Just draw the background
		ClientRect = ps.rcPaint;
		DrawColorPatch( hDC, &ClientRect, GetActiveRGB(NULL), YES );
		}
	EndPaint( hDlg, &ps );
	break;

//	case WM_LBUTTONDOWN: // If we want to turn on the thick frame style
//	hControl = GetDlgItem( hDlg, IDC_VIEWFULL );
//	dwStyle = GetWindowLong( hControl, GWL_STYLE );
//	if ( dwStyle & WS_THICKFRAME )
//		dwStyle &= (~(WS_THICKFRAME)); // beware the squiggle
//	else	dwStyle |= (WS_THICKFRAME);
//	ShowControl( hDlg, IDC_VIEWFULL, IDC_VIEWFULL, NO, NO );
//	SetWindowLong( hControl, GWL_STYLE, dwStyle );
//	ShowControl( hDlg, IDC_VIEWFULL, IDC_VIEWFULL, YES, NO );
//	break;

	case WM_KEYDOWN:
	case WM_COMMAND:
	case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

	default:
	return( FALSE );
	}

return( TRUE );
}
