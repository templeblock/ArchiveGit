// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

extern HWND hClientAstral, hActiveWindow;
extern HANDLE hInstAstral;

static LPTR lpBuffer;
static long lBufSize;
static BOOL ConstrainingX, ConstrainingY;

static BOOL fAllowPaintAbort = TRUE;

/***********************************************************************/
void EnablePaintAbort(fEnable)
/***********************************************************************/
BOOL fEnable;
{
fAllowPaintAbort = fEnable;
}

/***********************************************************************/
void AstralUpdateWindow(hWnd)
/***********************************************************************/
HWND hWnd;
{
HWND hOldWnd;
RECT rUpdate;
//
//if (GetUpdateRect(hWnd, &rUpdate, NO))
//	{
//	PaintImage(Window.hDC, &rUpdate, NULL);
//	ValidateRect(hWnd, &rUpdate);
//	}

if (SetCurView(hWnd, &hOldWnd))
	{
	rUpdate = lpImage->lpDisplay->UpdateRect;
	lpImage->lpDisplay->UpdateRect.top = lpImage->lpDisplay->UpdateRect.left = 32767;
	lpImage->lpDisplay->UpdateRect.bottom = lpImage->lpDisplay->UpdateRect.right = -32767;
	SetCurView(hOldWnd, NULL);
	fAllowPaintAbort = NO;
	if (rUpdate.right >= rUpdate.left)
		InvalidateRect(hWnd, &rUpdate, FALSE);
	UpdateWindow(hWnd);
	fAllowPaintAbort = YES;
	}
}

/***********************************************************************/
long FAR PASCAL WndImageProc(hWnd, msg, wParam, lParam)
/***********************************************************************/
HWND	 hWnd;
unsigned msg;
WORD	 wParam;
LONG	 lParam;
{
HWND hOldWnd, hVRulerWnd, hHRulerWnd;
LPDISPLAY lpDisplay;
static BOOL fCapture;
static HWND hLastActivated;
static int iSizeCallDepth;
static BOOL bRuler;
static RECT ClientRect;
RECT OldWindowRect, NewWindowRect, rInvalid;
static WORD idOldTool;
int w, h;
long lRet;

//dbg("msg = %x hWnd = %u", msg, hWnd);dbg(NULL);

switch (msg)
    {
    case WM_CREATE:
	/* Add the document to the list */
	SetWindowLong( hWnd, 12, (long)lpImage );
	SetWindowLong( hWnd, 8, (long)lpImage->lpDisplay);
        SetCurView(hWnd, NULL);
	if ( !AddDoc( hWnd ) )
		Message( IDS_EMEMALLOC );
	SetupScrollBars(hWnd);
	SetClassWord( hWnd, GCW_HCURSOR, Window.hCursor );
	NewImageSizeWindow(hWnd);
	GetWindowRect(hWnd, &lpImage->lpDisplay->WindowRect);
        SaveLastView();
	if (lpImage->lpDisplay->HasRulers)
		CreateRulers();
	break;

    case WM_PAINTICON:
    case WM_PAINT:
	AstralImagePaint( hWnd );
	break;

    case WM_MDIACTIVATE:
	/* If we're activating this child, remember it */
	if (wParam)
	    {
	    ActivateDocument(hWnd);
	    hLastActivated = hWnd;
	    }
	else
	    ActivateDocument(NULL);
	break;

    case WM_CLOSE:
	/* Check to see if the image needs to be saved */
	if ( !ConfirmClose(NO) )
		break;
	CloseImage(NO);
	break;

    case WM_MOVE:
	if (!IsIconic(hWnd))
		{
		if (SetCurView(hWnd, &hOldWnd))
            		{
	    		GetWindowRect(hWnd, &lpImage->lpDisplay->WindowRect);
	    		SetCurView(hOldWnd, NULL);
	    		}
		}
	return(DefMDIChildProc(hWnd, msg, wParam, lParam));

    case WM_SIZE:
	++iSizeCallDepth;
	if (SetCurView(hWnd, &hOldWnd))
            {
	    if (wParam == SIZENORMAL && lpImage->lpDisplay->ResizeType == SIZEICONIC)
		{
		RevertLastView();
		SaveLastView();
		}
	    else
		{
	        if ( wParam == SIZEICONIC ) // Force a full view below with a huge rate
		    {
		    SaveLastView();
		    lpImage->lpDisplay->FileRate = 1;
		    }
	    
  	        // We now have a new DispRect
	        ComputeDispRect();
	        ComputeFileRect(0, 0, lpImage->lpDisplay->FileRate );
		}
	    lpImage->lpDisplay->ResizeType = wParam;
	    OldWindowRect = lpImage->lpDisplay->WindowRect;
	    GetWindowRect(hWnd, &NewWindowRect);
	    lpImage->lpDisplay->WindowRect = NewWindowRect;
	
	    if (iSizeCallDepth == 1)
	    	SetupScrollBars(hWnd);
	    SizeRulers();
	    SetupRulers();
	    SetCurView(hOldWnd, NULL);
	    if (wParam == SIZENORMAL && FALSE)
		{
		GetClientRect(hWnd, &rInvalid);
		ValidateRect(hWnd, &rInvalid);
		if (RectWidth(&NewWindowRect) > RectWidth(&OldWindowRect))
			{
			rInvalid.left = rInvalid.right - (RectWidth(&NewWindowRect) - RectWidth(&OldWindowRect));
			InvalidateRect(hWnd, &rInvalid, TRUE);
			}
		if (RectHeight(&NewWindowRect) > RectHeight(&OldWindowRect))
			{
			GetClientRect(hWnd, &rInvalid);
			rInvalid.top = rInvalid.bottom - (RectHeight(&NewWindowRect) - RectHeight(&OldWindowRect));
			InvalidateRect(hWnd, &rInvalid, TRUE);
			}
		}
	    else
		{
	    	InvalidateRect( hWnd, NULL, TRUE );
		}
 	    }
	--iSizeCallDepth;
	return(DefMDIChildProc(hWnd, msg, wParam, lParam));

	case WM_KEYDOWN:
	    /* Translate keyboard messages to scroll commands */
	    switch (wParam)
		{
		case VK_UP:	PostMessage (hWnd, WM_VSCROLL, SB_LINEUP,   0L);
				break;
		case VK_DOWN:	PostMessage (hWnd, WM_VSCROLL, SB_LINEDOWN, 0L);
				break;
		case VK_PRIOR:	PostMessage (hWnd, WM_VSCROLL, SB_PAGEUP,   0L);
				break;
		case VK_NEXT:	PostMessage (hWnd, WM_VSCROLL, SB_PAGEDOWN, 0L);
				break;
		case VK_HOME:	PostMessage (hWnd, WM_HSCROLL, SB_PAGEUP,   0L);
				break;
		case VK_END:	PostMessage (hWnd, WM_HSCROLL, SB_PAGEDOWN, 0L);
				break;
		case VK_LEFT:	PostMessage (hWnd, WM_HSCROLL, SB_LINEUP,   0L);
				break;
		case VK_RIGHT:	PostMessage (hWnd, WM_HSCROLL, SB_LINEDOWN, 0L);
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

    case WM_HSCROLL:
    case WM_VSCROLL:
	ScrollImage( hWnd, msg, wParam, lParam );
	break;

    case WM_LBUTTONDOWN:
	if ( fCapture )
		break;
	if (hLastActivated == hWnd)
	    {
	    hLastActivated = 0;
	    if (Window.hCursor == Window.hNullCursor)
	 	{
	    	DisplayBrush(0, 0, 0, OFF);
	    	DisplayBrush(hWnd, LOWORD(lParam), HIWORD(lParam), ON);
		}
	    break;
	    }
	ConstrainXY(&lParam, TRUE, msg);
	if (Window.ModalProc)
	    ModalButtonDownProc( hWnd, lParam);
	else
	    {
	    if ( !Window.ToolActive )
		CreateProc( hWnd, lParam );
            if (Window.ToolActive)
	        ButtonDownProc( hWnd, lParam );
	    }
	if (Window.ToolActive || Window.ModalProc )
		{
		SetCapture( hWnd );
		fCapture = TRUE;
		}
	break;

    case WM_LBUTTONUP:
	if ( !fCapture )
		break;
	hLastActivated = 0;
	ConstrainXY(&lParam, fCapture, msg);
	ReleaseCapture();
	fCapture = FALSE;
	if (Window.ModalProc)
	    ModalButtonUpProc( hWnd, lParam);
        else if (Window.ToolActive)
	    ButtonUpProc( hWnd, lParam );
	ConstrainXY(&lParam, fCapture, msg);
	break;

    case WM_MOUSEMOVE:
	hLastActivated = 0;
	if (!ConstrainXY(&lParam, fCapture, msg))
		break; /* not really a mouse mouse with constrain */
	if (hWnd == lpImage->hWnd)
		SetRulerTicks(lParam);
	if (Window.ModalProc)
	    ModalMoveProc( hWnd, lParam);
	else if ( Window.ToolActive )
		{
		Window.fButtonDown = (wParam & MK_LBUTTON);
		MoveProc( hWnd, lParam );
		}
	else if (lpImage->hWnd == hWnd && Window.hCursor == Window.hNullCursor)
	 	{
	    	DisplayBrush(0, 0, 0, OFF);
	    	DisplayBrush(hWnd, LOWORD(lParam), HIWORD(lParam), ON);
		}
	break;

    case WM_LBUTTONDBLCLK:
	if (Window.ModalProc)
	    ModalDoubleClickProc(hWnd, lParam);
	else if (Window.ToolActive)
	    DoubleClickProc( hWnd, lParam );
	break;

    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONDBLCLK:
	if ( wParam != HTGROWBOX )
		return( DefMDIChildProc( hWnd, msg, wParam, lParam ) );
	if ( Control.Function == IDC_GRABBER )
		{
		ActivateTool( idOldTool );
		idOldTool = NULL;
		}
	else	{
		idOldTool = Control.Function;
		ActivateTool( IDC_GRABBER );
		}
	break;

    case WM_TIMER:
	if (Window.ToolActive)
		TimerProc(hWnd, lParam);
	break;

    case WM_DESTROY:
	lpDisplay = (LPDISPLAY)GetWindowLong( hWnd, 8 );
        if (lpDisplay)
	    FreeUp((LPTR)lpDisplay);
	RemDoc( hWnd );
    default:
	return( DefMDIChildProc( hWnd, msg, wParam, lParam ) );
    }
return( TRUE );
} /* end WndImageProc */

/************************************************************************/
int UpdateImage(lpRepairRect, fInvalidate )
/************************************************************************/
LPRECT lpRepairRect;
BOOL fInvalidate;
{
int i;
LPIMAGE lpTheImage;
HWND hWnd, hTheWnd, hOldWnd;
RECT DispRect;
HDC hDC;

if (!lpImage)
    return;
lpTheImage = lpImage;
hTheWnd = lpImage->hWnd;
if (lpRepairRect)
    {
    File2DispRect(lpRepairRect, &DispRect);
    if (fInvalidate)
	{
    	InvalidateRect(hTheWnd, &DispRect, FALSE);
    	//UpdateWindow(hTheWnd);
	}
    else
	PaintImage(Window.hDC, &DispRect, NULL);
    }
else
    {
    if (fInvalidate)
	{
    	InvalidateRect(hTheWnd, NULL, FALSE);
    	//UpdateWindow(hTheWnd);
	}
    else
	PaintImage(Window.hDC, NULL, NULL);
    }
for (i = 0; i < NumDocs(); ++i)
    {
    hWnd = GetDoc(i);
    if (SetCurView(hWnd, &hOldWnd))
        {
	if (lpImage == lpTheImage && hWnd != hTheWnd)
	    {
	    if (lpRepairRect)
		{
	        File2DispRect(lpRepairRect, &DispRect);
		if (fInvalidate)
			{
    			InvalidateRect(hWnd, &DispRect, FALSE);
    	    	        //UpdateWindow(hWnd);
			}
		else
			{
	    		hDC = GetDC(hWnd);
			PaintImage(hDC, &DispRect, NULL);
			ReleaseDC(hWnd, hDC);
			}
		}
 	    else
		{
		if (fInvalidate)
		    {
		    InvalidateRect(hWnd, NULL, FALSE);
    	    	    //UpdateWindow(hWnd);
		    }
		else
		    {
	    	    hDC = GetDC(hWnd);
		    PaintImage(hDC, NULL, NULL);
		    ReleaseDC(hWnd, hDC);
		    }
		}
 	    }
	}
    SetCurView(hOldWnd, NULL);
    }
}

/************************************************************************/
int UpdateImageSize()
/************************************************************************/
{
int i;
LPIMAGE lpTheImage;
HWND hWnd, hTheWnd, hOldWnd;

if (!lpImage)
    return;

lpTheImage = lpImage;
hTheWnd = lpImage->hWnd;
NewImageWindow( lpImage, lpImage->CurFile, lpImage->EditFrame, lpImage->FileType,
	(lpImage->DataType == IDC_SAVELA), FALSE, lpImage->ImageType, lpImage->ImageName );
for (i = 0; i < NumDocs(); ++i)
    {
    hWnd = GetDoc(i);
    if (SetCurView(hWnd, &hOldWnd))
        {
	if (lpImage == lpTheImage && hWnd != hTheWnd)
	    {
	    NewImageWindow( lpImage, lpImage->CurFile, lpImage->EditFrame,
			lpImage->FileType,
			(lpImage->DataType == IDC_SAVELA), FALSE, lpImage->ImageType, lpImage->ImageName );
 	    }
	}
    SetCurView(hOldWnd, NULL);
    }
}

/************************************************************************/
int UpdateMarquee()
/************************************************************************/
{
int i;
LPIMAGE lpTheImage;
HWND hWnd, hTheWnd, hOldWnd;

if (!lpImage)
    return;
lpTheImage = lpImage;
hTheWnd = lpImage->hWnd;
Marquee(YES);
for (i = 0; i < NumDocs(); ++i)
    {
    hWnd = GetDoc(i);
    if (SetCurView(hWnd, &hOldWnd))
        {
	if (lpImage == lpTheImage && hWnd != hTheWnd)
	    Marquee(YES);
	}
    SetCurView(hOldWnd, NULL);
    }
}

/***********************************************************************/
int AstralImagePaint( hWindow )
/***********************************************************************/
HWND hWindow;
{
RECT ClientRect, BorderRect, rInvalid, rRepair, rTemp, UpdateRect;
PAINTSTRUCT ps;
HDC hDC;
HBRUSH hBrush, hOldBrush;
MSG msg;
HWND hOldWnd;

if (!SetCurView(hWindow, &hOldWnd))
	return(NO);

SetupScrollBars( hWindow );
SetupRates( hWindow );

/* Need to paint the image into the image window */
hDC = BeginPaint( hWindow, &ps );
SetMapMode( hDC, MM_TEXT );

rRepair = ps.rcPaint;
if (hWindow == hActiveWindow)
	DisplayBrush(0, 0, 0, OFF);
if (fAllowPaintAbort)
	PaintImage( hDC, &rRepair, &rInvalid );
else
	PaintImage( hDC, &rRepair, NULL );
if (hWindow == hActiveWindow)
	Marquee(YES);

/* Indicate that we're finished painting the entire client area */
EndPaint( hWindow, &ps );

if (fAllowPaintAbort && (rInvalid.right >= rInvalid.left)) // update was aborted
	{
	rInvalid.right++; rInvalid.bottom++;
	AstralUnionRect(&lpImage->lpDisplay->UpdateRect, &lpImage->lpDisplay->UpdateRect, &rInvalid);
	SetCurView(hOldWnd, NULL);
	AstralGetMessage(&msg);
	AstralProcessMessage(&msg);
	if (IsDoc(hWindow) && SetCurView(hWindow, &hOldWnd))
		{
		UpdateRect = lpImage->lpDisplay->UpdateRect;
		lpImage->lpDisplay->UpdateRect.top = lpImage->lpDisplay->UpdateRect.left = 32767;
		lpImage->lpDisplay->UpdateRect.bottom = lpImage->lpDisplay->UpdateRect.right = -32767;
		SetCurView(hOldWnd, NULL);
		if (UpdateRect.right >= UpdateRect.left)
			InvalidateRect(hWindow, &UpdateRect, FALSE);
		}
	}
else
	{
	lpImage->lpDisplay->UpdateRect.top = lpImage->lpDisplay->UpdateRect.left = 32767;
	lpImage->lpDisplay->UpdateRect.bottom = lpImage->lpDisplay->UpdateRect.right = -32767;
	SetCurView(hOldWnd, NULL);
	DrawGrowBox( hWindow );
	}
return( TRUE );
}

/***********************************************************************/
int PaintBorder( hDC, lpRect )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
{
RECT DispRect, BorderRect;
HBRUSH hBrush, hOldBrush;

hBrush = CreateSolidBrush(RGB(127,127,127));
hOldBrush = SelectObject(hDC, hBrush);
GetDispRect(&DispRect, 0, 0);
BorderRect.top = DispRect.top;
BorderRect.bottom = lpImage->lpDisplay->DispRect.top - 1;
if (RectHeight(&BorderRect))
	{
	BorderRect.left = lpImage->lpDisplay->DispRect.left;
	BorderRect.right = lpImage->lpDisplay->DispRect.right;
	++BorderRect.right; ++BorderRect.bottom;
	FillRect(hDC, &BorderRect, hBrush);
	}
BorderRect.left = DispRect.left;
BorderRect.right = lpImage->lpDisplay->DispRect.left - 1;
if (RectWidth(&BorderRect))
	{
	BorderRect.top = DispRect.top;
	BorderRect.bottom = DispRect.bottom;
	++BorderRect.right; ++BorderRect.bottom;
	FillRect(hDC, &BorderRect, hBrush);
	}
BorderRect.left = lpImage->lpDisplay->DispRect.right + 1;
BorderRect.right = DispRect.right;
if (RectWidth(&BorderRect))
	{
	BorderRect.top = DispRect.top;
	BorderRect.bottom = DispRect.bottom;
	++BorderRect.right; ++BorderRect.bottom;
	FillRect(hDC, &BorderRect, hBrush);
	}
BorderRect.top = lpImage->lpDisplay->DispRect.bottom + 1;
BorderRect.bottom = DispRect.bottom;
if (RectHeight(&BorderRect))
	{
	BorderRect.left = lpImage->lpDisplay->DispRect.left;
	BorderRect.right = lpImage->lpDisplay->DispRect.right;
	++BorderRect.right; ++BorderRect.bottom;
	FillRect(hDC, &BorderRect, hBrush);
	}
SelectObject(hDC, hOldBrush);
DeleteObject(hBrush);
}

/************************************************************************/
BOOL AbortUpdate()
/************************************************************************/
{
MSG msg;

if (PeekMessage(&msg,NULL,WM_NULL+1,WM_PAINT-1,PM_NOREMOVE))
	{
	return(TRUE);
	}
else if (PeekMessage(&msg,NULL,WM_PAINT+1,WM_USER-1,PM_NOREMOVE))
	{
	return(TRUE);
	}
else
	return(FALSE);
}

/************************************************************************/
int PaintImage( hDC, lpRepairRect, lpInvalidRect )
/************************************************************************/
HDC hDC;
LPRECT lpRepairRect;
LPRECT lpInvalidRect;
{
int y, yline, ystart, ylast, DispWidth, count, sx, xDiva, yDiva, mwidth, width, increment;
FIXED yrate, xrate, yoffset;
RECT rPaint, rSource, rDest;
long lSizeNeeded;
LPTR lpLine, lpFloat[2], lpMask;
long dx, dy;
LPROC FloatingImageProc;
LPROC FloatingGraphicsProc;
LPFRAME lpFrame;
BOOL fCheckAbort;

if (lpInvalidRect) // set rect to empty
	{
	lpInvalidRect->left = lpInvalidRect->top = 32767;
	lpInvalidRect->right = lpInvalidRect->bottom = -32767;
	}
if (!(lpFrame = frame_set(NULL)))
	return(NO);

rDest = lpImage->lpDisplay->DispRect;
rSource = lpImage->lpDisplay->FileRect;
xDiva = lpImage->lpDisplay->xDiva;
yDiva = lpImage->lpDisplay->yDiva;
FloatingImageProc = lpImage->lpDisplay->FloatingImageProc;
FloatingGraphicsProc = lpImage->lpDisplay->FloatingGraphicsProc;
fCheckAbort = lpInvalidRect && !FloatingGraphicsProc;
if (FloatingImageProc)
	{
	mwidth = width = RectWidth(&rSource);
	if (!AllocLines(lpFloat, 2, width, 0))
		return(NO);
	width *= DEPTH;
	if (lpFrame->WriteMask || (ColorMask.Mask && !ColorMask.Off))
		{
		if (!(lpMask = Alloc((long)mwidth)))
			{
			FreeUp(lpFloat[0]);
			return(NO);
			}
		}
	else
		lpMask = NULL;
	}
DispWidth = RectWidth( &rDest );
lSizeNeeded = (long)DispWidth * (long)DEPTH;
if (lSizeNeeded != lBufSize)
    {
    if (lpBuffer)
        FreeUp(lpBuffer);
    lBufSize = lSizeNeeded;
    lpBuffer = Alloc(lBufSize);
    if (!lpBuffer)
        {
	if (FloatingImageProc)
		{
		FreeUp(lpFloat[0]);
		if (lpMask)
			FreeUp(lpMask);
		}
	lBufSize = 0;
	return(NO);
	}
    }
if ( !lpRepairRect )
	rPaint = rDest;
else	{
	rPaint = *lpRepairRect;
	BoundRect( &rPaint, rDest.left, rDest.top, rDest.right, rDest.bottom );
	}
sx = rSource.left;
yrate = FGET(RectHeight( &rSource),RectHeight(&rDest));
xrate = FGET(RectWidth( &rSource),RectWidth(&rDest));
ystart = rSource.top;
ylast = -1;

dx = rPaint.left - rDest.left;
dy = rPaint.top - rDest.top;

yoffset = (dy*yrate)+(yrate>>1);
count = dx + RectWidth(&rPaint);

increment = 10;
StartSuperBlt( hDC, &BltScreen, &rPaint, DEPTH, 10, xDiva, yDiva);
for ( y=rPaint.top; y<=rPaint.bottom; y++ )
	{
	yline = ystart + HIWORD( yoffset );
	yoffset += yrate;
	if ( yline != ylast )
		{
		ylast = yline;
		if (lpLine = frame_ptr(0, sx, yline, NO))
			{
			if (FloatingImageProc)
				{
				copy(lpLine, lpFloat[0], width);
				if ((*FloatingImageProc)(yline, sx, rSource.right, lpFloat[0], lpFloat[1], lpMask))
					lpLine = lpFloat[0];
				}
			if (lpLine)
		     		frame_sample(lpLine, 0, lpBuffer, dx, count, xrate);
			}
		}
	SuperBlt( lpBuffer );
	if (fCheckAbort && (--increment <= 0))
		{
		increment = 10;
		if (AbortUpdate())
			{
			if (y < rPaint.bottom)
				{
				lpInvalidRect->left = rPaint.left;
				lpInvalidRect->right = rPaint.right;
				lpInvalidRect->top = y+1;
				lpInvalidRect->bottom = rPaint.bottom;
				break;
				}
			}
		}
	}
SuperBlt( NULL );
if (FloatingImageProc)
	{
	FreeUp(lpFloat[0]);
	if (lpMask)
		FreeUp(lpMask);
	}
if (FloatingGraphicsProc)
	(*FloatingGraphicsProc)(hDC, lpRepairRect, ON);
return( YES );
}

#ifdef UNUSED
/************************************************************************/
int PaintImageLine( hDC, yFile, x1File, x2File, lpData )
/************************************************************************/
HDC hDC;
int yFile, x1File, x2File;
LPTR lpData;
{
int y, yline, ystart, ylast, DispWidth, count, sx, xDiva, yDiva;
FIXED yrate, xrate, yoffset;
RECT rPaint, rSource, rDest;
long lSizeNeeded;
LPTR lpLine;
long dx, dy;

rDest = lpImage->lpDisplay->DispRect;
rSource = lpImage->lpDisplay->FileRect;
xDiva = lpImage->lpDisplay->xDiva;
yDiva = lpImage->lpDisplay->yDiva;
DispWidth = RectWidth( &rDest );
lSizeNeeded = (long)DispWidth * (long)DEPTH;
if (lSizeNeeded != lBufSize)
    {
    if (lpBuffer)
        FreeUp(lpBuffer);
    lBufSize = lSizeNeeded;
    lpBuffer = Alloc(lBufSize);
    if (!lpBuffer)
        {
	lBufSize = 0;
	return;
	}
    }
rPaint.top = rPaint.bottom = yFile;
rPaint.left = x1File;
rPaint.right = x2File;
File2DispRect(&rPaint,&rPaint);
BoundRect( &rPaint, rDest.left, rDest.top, rDest.right, rDest.bottom );

sx = rSource.left;
yrate = FGET( RectHeight(&rSource), RectHeight(&rDest) );
xrate = FGET( RectWidth(&rSource), RectWidth(&rDest) );
ystart = rSource.top;
ylast = -1;

dx = rPaint.left - rDest.left;
dy = rPaint.top - rDest.top;

yoffset = (dy*yrate)+(yrate>>1);
count = dx + RectWidth(&rPaint);

for (y = rPaint.top; y <= rPaint.bottom; ++y)
    {
    yline = ystart + HIWORD( yoffset );
    if (yline >= yFile)
	break;
    ++rPaint.top;
    yoffset += yrate;
    }
if (yline > yFile) /* the supplied line is not used at this rate */
    return;

StartSuperBlt( hDC, &BltScreen, &rPaint, DEPTH, 10, xDiva, yDiva);
for ( y=rPaint.top; y<=rPaint.bottom; y++ )
	{
	yline = ystart + HIWORD( yoffset );
	yoffset += yrate;
	if ( yline != ylast )
		{
		ylast = yline;
		if (yline == yFile)
		    lpLine = lpData;
		else
		    lpLine = frame_ptr(0, sx, yline, NO);
		if (lpLine)
		     frame_sample(lpLine, 0, lpBuffer, dx, count, xrate);
		}
	SuperBlt( lpBuffer );
	}
SuperBlt( NULL );
return( TRUE );
}
#endif

/************************************************************************/
int PrintImage( hDC, lpSourceRect, lpDestRect, xDiva, yDiva, yTop, yTotal )
/************************************************************************/
HDC hDC;
LPRECT lpSourceRect, lpDestRect;
int xDiva, yDiva, yTop, yTotal;
{
int y, yline, ystart, ylast, DispWidth, count, sx, Excess;
FIXED yrate, xrate, yoffset;
RECT rPaint, rSource, rDest;
long lSizeNeeded;
LPTR lpLine;
long dx, dy;
LPRECT lpRepairRect;
BLT BltPrint;

InitSuperBlt( hDC, &BltPrint, (DEPTH == 1 ? 8 : 8) ); // NULL );
rDest = *lpDestRect;
rSource = *lpSourceRect;
lpRepairRect = NULL;
DispWidth = RectWidth( &rDest );
lSizeNeeded = (long)DispWidth * (long)DEPTH;
if (lSizeNeeded != lBufSize)
    {
    if (lpBuffer)
        FreeUp(lpBuffer);
    lBufSize = lSizeNeeded;
    lpBuffer = Alloc(lBufSize);
    if (!lpBuffer)
        {
	lBufSize = 0;
	return;
	}
    }
if ( !lpRepairRect )
	rPaint = rDest;
else	{
	rPaint = *lpRepairRect;
	BoundRect( &rPaint, rDest.left, rDest.top, rDest.right, rDest.bottom );
	}

sx = rSource.left;
yrate = FGET( RectHeight(&rSource), RectHeight(&rDest) );
xrate = FGET( RectWidth(&rSource), RectWidth(&rDest) );
ystart = rSource.top;
ylast = -1;

// Due to a bug in the postscript driver when 1 line is blt'ed
Excess = RectHeight( &rPaint ) % 10;
if ( Excess == 1 )
	rPaint.bottom--;

dx = rPaint.left - rDest.left;
dy = rPaint.top - rDest.top;

yoffset = (dy*yrate)+(yrate>>1);
count = dx + RectWidth(&rPaint);

StartSuperBlt( hDC, &BltPrint, &rPaint, DEPTH, 10, xDiva, yDiva );
for ( y=rPaint.top; y<=rPaint.bottom; y++ )
	{
	AstralClockCursor( yTop+y-rPaint.top, yTotal );
	yline = ystart + HIWORD( yoffset );
	yoffset += yrate;
	if ( yline != ylast )
		{
		ylast = yline;
		lpLine = frame_ptr(0, sx, yline, NO);
		if (lpLine)
		     frame_sample(lpLine, 0, lpBuffer, dx, count, xrate);
		}
	SuperBlt( lpBuffer );
	}
SuperBlt( NULL );
return( TRUE );
}


/************************************************************************/
void SetTitleBar( hWindow )
/************************************************************************/
HWND hWindow;
{
int Percentage;
LPTR lpStr;
BYTE szTitle[MAX_STR_LEN];
BYTE szLastTitle[MAX_STR_LEN];
// Redraw the title bar with the proper view percentage
AstralStr( IDS_PREVIEW, &lpStr );
if ( !(Percentage = lpImage->lpDisplay->ViewPercentage) )
	Percentage = FMUL( 100, lpImage->lpDisplay->DispRate );
Lowercase( lpImage->CurFile );
if ( IsIconic( hWindow ) )
	lstrcpy( szTitle, stripdir(lpImage->CurFile) );
else	wsprintf( szTitle, lpStr, Percentage, stripdir(lpImage->CurFile) );
GetWindowText( hWindow, szLastTitle, MAX_STR_LEN);
if (!StringsEqual(szTitle, szLastTitle))
	SetWindowText( hWindow, szTitle );
}


/************************************************************************/
SetupScrollBars( hWindow )
/************************************************************************/
HWND hWindow;
{
int dx, dy, x, y, xMinPos, yMinPos, xMaxPos, yMaxPos, xPos, yPos;

// Setup the scroll bar values based on the lpImage->lpDisplay->FileRect
dx = lpImage->npix - RectWidth( &lpImage->lpDisplay->FileRect );
dx = max(dx, 0);
x = lpImage->lpDisplay->FileRect.left;
GetScrollRange( hWindow, SB_HORZ, &xMinPos, &xMaxPos);
xPos = GetScrollPos(hWindow, SB_HORZ);
if (xMinPos != 0 || xMaxPos != dx || xPos != x)
	SetScrollRange( hWindow, SB_HORZ, 0, dx, NO );
dy = lpImage->nlin - RectHeight( &lpImage->lpDisplay->FileRect );
dy = max(dy, 0);
y = lpImage->lpDisplay->FileRect.top;
GetScrollRange( hWindow, SB_VERT, &yMinPos, &yMaxPos);
yPos = GetScrollPos(hWindow, SB_VERT);
if (yMinPos != 0 || yMaxPos != dy || yPos != y)
	{
	SetScrollRange( hWindow, SB_VERT, 0, dy, NO );
	SetScrollPos( hWindow, SB_VERT, y, YES );
	}
if (xMinPos != 0 || xMaxPos != dx || xPos != x)
	SetScrollPos( hWindow, SB_HORZ, x, YES );
}


/************************************************************************/
SetupRates( hWindow )
/************************************************************************/
HWND hWindow;
{
FIXED rate;
int DispHeight, DispWidth;

rate = lpImage->lpDisplay->DispRate;

DispHeight = RectHeight(&lpImage->lpDisplay->DispRect);
DispWidth = RectWidth(&lpImage->lpDisplay->DispRect);
// Always set new rates to be as accurate as possible
if (DispHeight > 0 && DispWidth > 0)
    {
    if (DispWidth > DispHeight)
        {
	lpImage->lpDisplay->FileRate = FGET(RectWidth( &lpImage->lpDisplay->FileRect ), DispWidth );
        lpImage->lpDisplay->DispRate = FGET(DispWidth, RectWidth( &lpImage->lpDisplay->FileRect ) );
	}
    else
        {
    	lpImage->lpDisplay->FileRate = FGET(RectHeight( &lpImage->lpDisplay->FileRect),DispHeight);
        lpImage->lpDisplay->DispRate = FGET(DispHeight, RectHeight(&lpImage->lpDisplay->FileRect ) );
	}
    }
// Draw the title bar with the view percentage: lpImage->lpDisplay->DispRate has changed
SetTitleBar( hWindow );
}


/************************************************************************/
void Display2File1( lpx, lpy )
/************************************************************************/
LPINT	lpx, lpy;
{
int	x, y;

/* determine location on hi-res image */
x = *lpx - lpImage->lpDisplay->DispRect.left;
y = *lpy - lpImage->lpDisplay->DispRect.top;

x = FMUL( x, lpImage->lpDisplay->FileRate );
y = FMUL( y, lpImage->lpDisplay->FileRate );

x += lpImage->lpDisplay->FileRect.left;
y += lpImage->lpDisplay->FileRect.top;

*lpx = x;
*lpy = y;
}

/************************************************************************/
void Display2File( lpx, lpy )
/************************************************************************/
LPINT	lpx, lpy;
{
int	x, y;
FIXED 	fx, fy;

/* determine location on the display image */
x = *lpx - lpImage->lpDisplay->DispRect.left;
y = *lpy - lpImage->lpDisplay->DispRect.top;

fx = (long)x * lpImage->lpDisplay->FileRate;
fy = (long)y * lpImage->lpDisplay->FileRate;
x = HIWORD(fx);
y = HIWORD(fy);

x += lpImage->lpDisplay->FileRect.left;
y += lpImage->lpDisplay->FileRect.top;

*lpx = x;
*lpy = y;
}

/************************************************************************/
void File2Display( lpx, lpy )
/************************************************************************/
LPINT	lpx, lpy;
{
int	x, y;

/* determine location on hi-res image */
x = *lpx - lpImage->lpDisplay->FileRect.left;
y = *lpy - lpImage->lpDisplay->FileRect.top;

x = FMUL( x, lpImage->lpDisplay->DispRate );
y = FMUL( y, lpImage->lpDisplay->DispRate );

x += lpImage->lpDisplay->DispRect.left;
y += lpImage->lpDisplay->DispRect.top;

*lpx = x;
*lpy = y;
}


/************************************************************************/
void File2DispRect( fileRect, dispRect )
/************************************************************************/
LPRECT fileRect, dispRect;
{
long value;

value = fileRect->left - lpImage->lpDisplay->FileRect.left;
value *= lpImage->lpDisplay->DispRate;
dispRect->left = HIWORD(value) + lpImage->lpDisplay->DispRect.left - 1;

value = fileRect->top - lpImage->lpDisplay->FileRect.top;
value *= lpImage->lpDisplay->DispRate;
dispRect->top = HIWORD(value) + lpImage->lpDisplay->DispRect.top - 1;

value = fileRect->right - lpImage->lpDisplay->FileRect.left + 1;
value *= lpImage->lpDisplay->DispRate;
dispRect->right = HIWORD(value) + lpImage->lpDisplay->DispRect.left + 1;

value = fileRect->bottom - lpImage->lpDisplay->FileRect.top + 1;
value *= lpImage->lpDisplay->DispRate;
dispRect->bottom = HIWORD(value) + lpImage->lpDisplay->DispRect.top + 1;
}


/************************************************************************/
BOOL ConstrainXY(lpMousePos, fDown, msg)
/************************************************************************/
LPLONG lpMousePos;
BOOL fDown;
unsigned msg;
{
static BOOL ConstrainLooking;
static POINT Constrain, LastPos;
BOOL fMove;
long lParam;
int dx, dy, x, y;

lParam = *lpMousePos;
x = LOWORD(lParam);
y = HIWORD(lParam);
if (msg == WM_LBUTTONDOWN)
	{
	ConstrainingX = ConstrainingY = FALSE;
	ConstrainLooking = CONTROL;
	Constrain.x = x;
	Constrain.y = y;
	}
else if (!fDown)
	{
	ConstrainLooking = FALSE;
	ConstrainingX = ConstrainingY = FALSE;
	}
else if (ConstrainLooking)
	{
	dx = abs(Constrain.x - x);
	dy = abs(Constrain.y - y);
	if (dx > 1 || dy > 1)
		{
		ConstrainLooking = FALSE;
		if (dx > dy)
			ConstrainingY = TRUE;
		else
			ConstrainingX = TRUE;
		}
	else
		{
		x = Constrain.x;
		y = Constrain.y;
		}
	}
if (ConstrainingX)
	x = Constrain.x;
if (ConstrainingY)
	y = Constrain.y;
*lpMousePos = MAKELONG(x, y);
fMove = x != LastPos.x || y != LastPos.y;
LastPos.x = x;
LastPos.y = y;
return(fMove);
}

/************************************************************************/
BOOL ConstrainX()
/************************************************************************/
{
return(ConstrainingX);
}

/************************************************************************/
BOOL ConstrainY()
/************************************************************************/
{
return(ConstrainingY);
}


/************************************************************************/
void DrawGrowBox( hWnd )
/************************************************************************/
HWND hWnd;
{
HDC hDC;
RECT rect, rGrow;
HRGN hRgn;
PAINTSTRUCT ps;
int v, v1, v2, h, h1, h2;

GetScrollRange( hWnd, SB_VERT, &v1, &v2 );
GetScrollRange( hWnd, SB_HORZ, &h1, &h2 );
v = GetScrollPos( hWnd, SB_VERT );
h = GetScrollPos( hWnd, SB_HORZ );
//dbg( "%d %d %d %d %d %d", v, v1, v2, h, h1, h2 );
if ( v1 == v2 || v == v1 || v == v2 )
	return;
if ( h1 == h2 || h == h1 || h == h2 )
	return;

SetWindowPos( hWnd, 0, /* Same z order */
	0, 0, /* Same location */
	0, 0, /* Same size */
	SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE  | SWP_NOZORDER| SWP_NOREDRAW);
hDC = GetDC( hClientAstral );
GetClientRect( hWnd, &rect );
ClientToScreen( hWnd, (LPPOINT)&rect.left );
ClientToScreen( hWnd, (LPPOINT)&rect.right );
ScreenToClient( hClientAstral, (LPPOINT)&rect.left );
ScreenToClient( hClientAstral, (LPPOINT)&rect.right );
rGrow = rect;
rGrow.left = rGrow.right + 1;
rGrow.right += ( GetSystemMetrics(SM_CXHSCROLL) - 1 );
rGrow.top = rGrow.bottom + 1;
rGrow.bottom += ( GetSystemMetrics(SM_CYVSCROLL) - 1 );
SelectClipRgn( hDC, hRgn = CreateRectRgnIndirect( &rGrow ) );
DeleteObject( hRgn );
DrawIcon( hDC, (rGrow.left+rGrow.right)/2 - 16, (rGrow.top+rGrow.bottom)/2 -16,
	LoadIcon( hInstAstral, MAKEINTRESOURCE(IDC_GRABBER) ) );
SetWindowPos( hWnd, 0, /* Same z order */
	0, 0, /* Same location */
	0, 0, /* Same size */
	SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE  | SWP_NOZORDER| SWP_NOREDRAW);
ReleaseDC( hClientAstral, hDC );
}
