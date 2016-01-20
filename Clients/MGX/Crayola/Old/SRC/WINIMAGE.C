//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "ppole.h"

extern HWND hActiveWindow;
extern HINSTANCE hInstAstral;
extern BOOL fAppActive;

#define LINES_PER_BLT 20

HWND hPaintWnd; // temporarily used for stitching to modify less code

// Static prototypes
static void SetupRates( HWND hWindow );
static int AstralImagePaint( HWND hWindow );
static void SetupScrollBars( HWND hWindow );

static LPTR lpBuffer;
static long lBufSize;
static BOOL fAllowPaintAbort = TRUE;

/***********************************************************************/
BOOL EnablePaintAbort(BOOL fEnable)
/***********************************************************************/
{
BOOL fSaveEnable;

fSaveEnable = fAllowPaintAbort;
fAllowPaintAbort = fEnable;
return(fSaveEnable);
}

/***********************************************************************/
void AstralUpdateWindow(HWND hWnd)
/***********************************************************************/
{
HWND hOldWnd;
RECT rUpdate;
BOOL fSaveEnable;

SetCurView(hWnd, &hOldWnd);
if ( lpImage )
	{
	rUpdate = lpImage->lpDisplay->UpdateRect;
	AstralSetRectEmpty(&lpImage->lpDisplay->UpdateRect);
	SetCurView(hOldWnd, NULL);
	fSaveEnable = EnablePaintAbort(NO);
	if (!AstralIsRectEmpty(&rUpdate))
		InvalidateRect(hWnd, AstralToWindowsRect(&rUpdate), FALSE);
	UpdateWindow(hWnd);
	EnablePaintAbort(fSaveEnable);
	}
else
	SetCurView(hOldWnd, NULL);
}

/***********************************************************************/
LPDISPLAYHOOK SetDisplayHook(HWND hWnd, LPDISPLAYHOOK lpDisplayHook)
/***********************************************************************/
{
LPDISPLAY lpDisplay;
LPDISPLAYHOOK lpOldDisplayHook;

if ( !(lpDisplay = (LPDISPLAY)GetWindowLong(hWnd, GWL_DISPLAYPTR)) )
	return( NULL );
lpOldDisplayHook = lpDisplay->lpDisplayHook;
lpDisplay->lpDisplayHook = lpDisplayHook;
return( lpOldDisplayHook );
}

/***********************************************************************/
LPDISPLAYHOOK GetDisplayHook(HWND hWnd)
/***********************************************************************/
{
LPDISPLAY lpDisplay;

lpDisplay = (LPDISPLAY)GetWindowLong(hWnd, GWL_DISPLAYPTR);
if (lpDisplay)
	return(lpDisplay->lpDisplayHook);
else
	return(NULL);
}

/***********************************************************************/
void SetImageHook(HWND hWnd, LPIMAGEHOOK lpImageHook)
/***********************************************************************/
{
LPDISPLAY lpDisplay;

lpDisplay = (LPDISPLAY)GetWindowLong(hWnd, GWL_DISPLAYPTR);
if (lpDisplay)
	lpDisplay->lpImageHook = lpImageHook;
}

/***********************************************************************/
LPIMAGEHOOK GetImageHook(HWND hWnd)
/***********************************************************************/
{
LPDISPLAY lpDisplay;

lpDisplay = (LPDISPLAY)GetWindowLong(hWnd, GWL_DISPLAYPTR);
if (lpDisplay)
	return(lpDisplay->lpImageHook);
else
	return(NULL);
}

/***********************************************************************/
static void WndImageSetCursor (HWND hWnd,LPARAM lParam)
/***********************************************************************/
{
    HintLine (0);
	CursorProc(hWnd, lParam);
}

/***********************************************************************/
long WINPROC EXPORT WndImageProc(
/***********************************************************************/
HWND 	hWnd,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
HWND hOldWnd;
LPDISPLAY lpDisplay;
RECT rWin;
int x, y, i;
long lRet;
LPMDICREATESTRUCT lpMcs;
LPCREATESTRUCT lpCs;
LPIMAGE lpNewImage, lpImageToDelete;
BOOL bOnImage, bIconic, bZoomed, bFound;
POINT pt, fpt;
static BOOL fCapture;
static HWND hLastActivated;
static int iSizeCallDepth;

switch (msg)
	{
	case WM_CREATE:
	/* Add the document to the list */
	lpCs = (LPCREATESTRUCT)lParam;
	lpMcs = (LPMDICREATESTRUCT)lpCs->lpCreateParams;
	lpNewImage = (LPIMAGE)lpMcs->lParam;
	SetWindowLong( hWnd, GWL_IMAGEPTR, (long)lpNewImage );
	SetWindowLong( hWnd, GWL_DISPLAYPTR, (long)lpNewImage->lpNewDisplay);
	SET_CLASS_CURSOR( hWnd, Window.hCursor );
	if ( !AddDoc( hWnd ) )
		Message( IDS_EMEMALLOC );
	if (SetCurView(hWnd, &hOldWnd))
		{
		if ( View.UseRulers )
			CreateRulers();
		SetupScrollBars(hWnd);
		GetWindowRect(hWnd, &lpImage->lpDisplay->WindowRect);
			SaveLastView();
		SetCurView(hOldWnd, NULL);
		}
	PPOLERegisterDocument( hWnd );
	break;

	case WM_DESTROY:
//	if ( lpImageToDelete = (LPIMAGE)GetWindowLong( hWnd, GWL_IMAGEPTR ) )
//		&& owned by client )
//		PPOLEClientNotify( (LPIMAGEOBJECT)lpImageToDelete->pObj,
//			OLE_CLOSED );
	PPOLERevokeDocument( hWnd );
	RemDoc( hWnd );
	if ( !NumDocs() )
	{
		Window.fHasZoom = FALSE;
		if ( Tool.hRibbon )
			SendMessage( Tool.hRibbon, WM_DOCACTIVATED, (WPARAM)0, 1L );
	}
	if ( lpDisplay = (LPDISPLAY)GetWindowLong( hWnd, GWL_DISPLAYPTR ) )
		FreeUp((LPTR)lpDisplay);

	// If there are no other windows with this image pointer,
	// free up the memory associated with the image
	lpImageToDelete = (LPIMAGE)GetWindowLong( hWnd, GWL_IMAGEPTR );
	bFound = NO;
	for ( i = 0; i < NumDocs(); i++ )
		{
		lpNewImage = (LPIMAGE)GetWindowLong( GetDoc(i), GWL_IMAGEPTR );
		if ( lpNewImage != lpImageToDelete )
			continue;
		bFound = YES;
		break;
		}

	if ( !bFound )
		{ // If we found NO other matching image windows...
		ImgDeleteObjects(lpImageToDelete, NULL, NO);
		FreeUp( (LPTR)lpImageToDelete );
		}

	if ( Tool.hRibbon )
		SendMessage( Tool.hRibbon, WM_DOCCHANGED, (WPARAM)Tool.hRibbon, 0L);

	return( DefMDIChildProc( hWnd, msg, wParam, lParam ) );
	break;

	case WM_ERASEBKGND:
	// Turn off our brush display for painting, make sure it
	// happens before erase background
//	if (hWnd == hActiveWindow)
		DisplayBrush(0, 0, 0, OFF);
	return( DefMDIChildProc( hWnd, msg, wParam, lParam ) );

	case WM_PALETTECHANGED:
	InvalidateRect(hWnd, NULL, FALSE);
	break;

	case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hWnd, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

//	case WM_PAINTICON:
	case WM_PAINT:
	if ( SuperPaint( hWnd ) )
		break; // Printing in progress... (SuperBlt not reentrant)
	AstralImagePaint( hWnd );
	break;

	case WM_MOUSEACTIVATE:
	if (!fAppActive)
		hLastActivated = hWnd;
	return( DefMDIChildProc( hWnd, msg, wParam, lParam ) );

#ifdef WIN32
// Differences in this message for Win32
	case WM_MDIACTIVATE:
		if ( Tool.hRibbon )
			SendMessage( Tool.hRibbon, WM_DOCACTIVATED, wParam, lParam );
		ActivateDocument((HWND)lParam);
	break;
#else
	case WM_MDIACTIVATE:
	/* If we're activating this child, remember it */
	if (wParam)
		{
		hLastActivated = hWnd;
		ActivateDocument(hWnd);
		if ( Tool.hRibbon )
			SendMessage( Tool.hRibbon, WM_DOCACTIVATED, wParam, lParam );
		}
	else
		{
		if ( Tool.hRibbon )
			SendMessage( Tool.hRibbon, WM_DOCACTIVATED, wParam, lParam );
		ActivateDocument(NULL);
		}
	break;
#endif	

	case WM_CLOSE:
	/* Check to see if the image needs to be saved */
	if ( !ConfirmClose(NO, YES) )
		break;
	CloseImage(NO, NULL);
	break;

	case WM_MOVE:
	// the 2 lines below we necessary because I believe there
	// is a bug in the compiler.  The line '!IsIconic(hWnd) &&
	// !IsZoomed(hWnd)' would not work correctly, it always
	// evaluated to FALSE.
	bIconic = IsIconic(hWnd);
	bZoomed = IsZoomed(hWnd);
	if ( bIconic || bZoomed )
		return( DefMDIChildProc(hWnd, msg, wParam, lParam) );
	if ( !SetCurView(hWnd, &hOldWnd) )
		return( DefMDIChildProc(hWnd, msg, wParam, lParam) );
	GetWindowRect(hWnd, &rWin);
	OffsetRect(&lpImage->lpDisplay->WindowRect,
		-lpImage->lpDisplay->WindowRect.left,
		-lpImage->lpDisplay->WindowRect.top);
	OffsetRect(&lpImage->lpDisplay->WindowRect, rWin.left, rWin.top);
	OffsetRect(&lpImage->lpDisplay->LastWindowRect,
		-lpImage->lpDisplay->LastWindowRect.left,
		-lpImage->lpDisplay->LastWindowRect.top);
	OffsetRect(&lpImage->lpDisplay->LastWindowRect, rWin.left, rWin.top);
	SetCurView(hOldWnd, NULL);
	return( DefMDIChildProc(hWnd, msg, wParam, lParam) );

	case WM_SIZE:
	if ( !SetCurView(hWnd, &hOldWnd) ) // error!!! - this shouldn't happen
		return(DefMDIChildProc(hWnd, msg, wParam, lParam));
	++iSizeCallDepth;
	// going from iconic to normal or maximized
	if ( wParam != SIZEICONIC &&
		 lpImage->lpDisplay->ResizeType == SIZEICONIC )
		{ // Restoring an iconic image
		RevertLastView();
			GetWindowRect(hWnd, &lpImage->lpDisplay->WindowRect);
		SaveLastView();
		}
	else
		{ // sizing either normal, maximized, or iconic
		x = -1;
		y = -1;
		if ( wParam == SIZEICONIC ) // Minimizing an image
			{ // Force a full view below with a small FileRate
			SaveLastView();
			lpImage->lpDisplay->FileRate = 1;
			}
		// Compute new DispRect and FileRect
		ComputeDispRect();
		ComputeFileRect(x, y, lpImage->lpDisplay->FileRate );
		}
	// save current save state and window size
	lpImage->lpDisplay->ResizeType = wParam;
	GetWindowRect(hWnd, &lpImage->lpDisplay->WindowRect);
	// only setup scroll bars once, because when they are added or
	// taken away they can cause WM_SIZE events
	if (iSizeCallDepth == 1)
		SetupScrollBars(hWnd);
	// setup rulers for new window size
	SizeRulers();
	SetupRulers();
	// reset lpImage
	SetCurView(hOldWnd, NULL);
	// area is always invalid for a resize
	InvalidateRect( hWnd, NULL, TRUE );
	--iSizeCallDepth;
	lRet = DefMDIChildProc(hWnd, msg, wParam, lParam);
	return (lRet);

	case WM_KILLFOCUS:
	// Fall through...

	case WM_SETFOCUS:
	case WM_CHAR:
	if (Tool.bActive)
		MsgProc(hWnd, (long)wParam, msg);
	return(FALSE);

	case WM_KEYDOWN:
	if ( Tool.bActive )
		{
		if (wParam == VK_RETURN && Tool.id != IDC_TEXT)
				DeactivateTool();
		else	MsgProc(hWnd, (long)wParam, msg);
		break;
		}
	/* Translate keyboard messages to scroll commands */
	switch (wParam)
		{
		case VK_DELETE:
			MsgProc(hWnd, (long)wParam, msg);
			break;
		case VK_UP:		PostMessage (hWnd, WM_VSCROLL, SB_LINEUP,   0L);
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
	if ( Tool.bActive )
		{
		if (wParam == VK_ESCAPE)
				DestroyProc( hWnd, 1L );
		else	MsgProc(hWnd, (long)wParam, msg);
		break;
		}
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
		case VK_ESCAPE:
			break;
		}
	break;

	case WM_HSCROLL:
	case WM_VSCROLL:
	return( ScrollImage( hWnd, msg, wParam, lParam ) );
	break;

	case WM_RBUTTONDOWN:
	if ( fCapture || Tool.bActive || !Tool.Alt.lpToolProc )
		break;
	if ( !(bOnImage = ONIMAGE( LOWORD(lParam), HIWORD(lParam) )) )
		break;
	if (Tool.bActive)
		DeactivateTool();
	InstallTool( !Tool.bAltTool );
	if ( !Tool.bPassRButton )
		break;

	case WM_LBUTTONDOWN:
	if ( fCapture )
		break;
	bOnImage = ONIMAGE( LOWORD(lParam), HIWORD(lParam) );
	if (hLastActivated == hWnd)
		{
		hLastActivated = 0;
		if (Window.hCursor == Window.hNullCursor)
	 		{
			DisplayBrush(0, 0, 0, OFF);
			if ( bOnImage )
				DisplayBrush(hWnd, LOWORD(lParam), HIWORD(lParam), ON);
			}
		break;
		}
////if ( !bOnImage )
////	break;
	ConstrainXY(&lParam, TRUE, msg, CONSTRAINXY && Tool.fConstrain);
	SoundStartID( Tool.id + TS_TOOLSOUND_OFFSET, YES/*bLoop*/, NULL/*hInstance*/ );
	if ( !Tool.bActive )
		CreateProc( hWnd, lParam );
	if (Tool.bActive)
		ButtonDownProc( hWnd, lParam );
	if (Tool.bActive)
		{
		// Never display the marquee when we have capture
		EnableMarquee(NO);
		SetCapture( hWnd );
		fCapture = TRUE;
		}
	else
		SoundStop();
	break;

	case WM_RBUTTONUP:
	if (!Tool.bActive || !Tool.bAltTool || !Tool.bPassRButton)
		break;
	case WM_LBUTTONUP:
	if ( hWnd != GetFocus() )
		SetFocus(hWnd);
	SoundStop();
	if ( !fCapture )
		break;
	hLastActivated = 0;
	ConstrainXY(&lParam, fCapture, msg, FALSE);
	ReleaseCapture();
	// go back to displaying marquee cause we don't have capture any more
	EnableMarquee(YES);
	fCapture = FALSE;
	RandomizeActiveColor();
	if (Tool.bActive)
		ButtonUpProc( hWnd, lParam );
	ConstrainXY(&lParam, fCapture, msg, FALSE);
	break;

	case WM_MOUSEMOVE:
	hLastActivated = 0;
	pt = MAKEPOINT(lParam);

	if ( !(bOnImage = ONIMAGE( pt.x, pt.y )) && Tool.bActive)
		{
		if (Tool.fAutoScrollSetPos)
			{
			fpt.x = pt.x;
			fpt.y = pt.y;
			Display2File((LPINT)&fpt.x, (LPINT)&fpt.y);
			}
		if (AutoScroll( hWnd, pt.x, pt.y ) && Tool.fAutoScrollSetPos)
			{							   
			File2Display((LPINT)&fpt.x, (LPINT)&fpt.y);
			lParam = MAKELONG(fpt.x, fpt.y);
			bOnImage = ONIMAGE(fpt.x, fpt.y);		
			ClientToScreen(hWnd, &fpt);
			SetCursorPos(fpt.x, fpt.y);
			}
		}
	if (!ConstrainXY(&lParam, fCapture, msg, FALSE))
		break; /* not really a mouse move with constrain */
	Window.fLButtonDown = (wParam & MK_LBUTTON);
	Window.fMButtonDown = (wParam & MK_MBUTTON);
	Window.fRButtonDown = (wParam & MK_RBUTTON);
	if (hWnd == lpImage->hWnd)
		{
		SetRulerTicks(lParam);
		if (!Tool.bActive || !Tool.fDisplaysInfo)
			DisplayInfo(lParam, NULL);
		}
	if ( Tool.bActive )
		{
		if ( bOnImage || fCapture )
			MoveProc( hWnd, lParam );
		}
	else
	if (lpImage->hWnd == hWnd && Window.hCursor == Window.hNullCursor)
	 	{
		DisplayBrush(0, 0, 0, OFF);
		if ( bOnImage )
			DisplayBrush(hWnd, LOWORD(lParam), HIWORD(lParam), ON);
		}
	break;

	case WM_LBUTTONDBLCLK:
	DoubleClickProc( hWnd, lParam );
	break;

	case WM_SETCURSOR:
	WndImageSetCursor(hWnd, lParam);
	return( DefMDIChildProc( hWnd, msg, wParam, lParam ) );
	break;

	case WM_TIMER:
	if (Tool.bActive)
		TimerProc(hWnd, lParam);
	break;

	default:
	return( DefMDIChildProc( hWnd, msg, wParam, lParam ) );
	}

return( TRUE );
} /* end WndImageProc */


/************************************************************************/
void UpdateImage(
/************************************************************************/
LPRECT 	lpRepairRect,
BOOL 	fInvalidate)
{
UpdateImageEx(lpImage, lpRepairRect, fInvalidate, YES, YES);
}

/************************************************************************/
void UpdateImageEx(
/************************************************************************/
LPIMAGE 	lpTheImage,
LPRECT 		lpRepairRect,
BOOL 		fInvalidate,
BOOL		fUpdateActive,
BOOL		fUpdateNonActive)
{
int i;
HWND hWnd, hTheWnd, hOldWnd;
RECT DispRect;
HDC hDC;
BOOL fTurnOn;

if (!lpImage)
	return;
hTheWnd = lpTheImage->hWnd;
if (fUpdateActive)
	{
	if (lpRepairRect)
		File2DispRect(lpRepairRect, &DispRect);
	else
		DispRect = lpTheImage->lpDisplay->DispRect;
	if (fInvalidate)
		InvalidateRect(hTheWnd, AstralToWindowsRect(&DispRect), FALSE);
	else
		{
		if (fTurnOn = (Window.hCursor == Window.hNullCursor))
			fTurnOn = DisplayBrushEx(hTheWnd, 0, 0, OFF, &DispRect);
		ImgPaintRaster(lpTheImage, Window.hDC, &DispRect, NULL);
		if (fTurnOn)
			DisplayBrushEx(hTheWnd, 32767, 32767, ON, NULL);
		}
	}

if (!fUpdateNonActive)
	return;

for (i = 0; i < NumDocs(); ++i)
	{
	hWnd = GetDoc(i);
	if (SetCurView(hWnd, &hOldWnd))
		{
		if (lpImage == lpTheImage && hWnd != hTheWnd)
			{
			if (lpRepairRect)
				File2DispRect(lpRepairRect, &DispRect);
			else
				DispRect = lpImage->lpDisplay->DispRect;
			if (fInvalidate)
				InvalidateRect(hWnd, AstralToWindowsRect(&DispRect), FALSE);
			else
				{
				hDC = GetDC(hWnd);
				ImgPaintRaster(lpImage, hDC, &DispRect, NULL);
				ReleaseDC(hWnd, hDC);
				}
			}
		SetCurView(hOldWnd, NULL);
 		}
	}
}

/************************************************************************/
void UpdateImageSize()
/************************************************************************/
{
int i;
LPIMAGE lpTheImage;
HWND hWnd, hTheWnd, hOldWnd;

if (!lpImage)
	return;

lpTheImage = lpImage;
hTheWnd = lpImage->hWnd;
NewImageWindow(
	lpImage,				// lpOldFrame
	lpImage->CurFile,		// Name
	ImgGetBaseEditFrame(lpImage), // lpNewFrame
	lpImage->FileType, 		// lpImage->FileType
	lpImage->DataType, 		// lpImage->DataType
	FALSE,					// New view?
	lpImage->DocumentType,	// lpImage->DocumentType
	lpImage->ImageName,		// lpImage->ImageName
	MAYBE					// Maximized?
	);
for (i = 0; i < NumDocs(); ++i)
	{
	hWnd = GetDoc(i);
	if (SetCurView(hWnd, &hOldWnd))
		{
		if (lpImage == lpTheImage && hWnd != hTheWnd)
			{
			if (IsIconic(hWnd))
				{
				// Compute new DispRect and FileRect
				ComputeDispRect();
				ComputeFileRect(-1, -1, lpImage->lpDisplay->FileRate );
				InvalidateRect(hWnd, NULL, TRUE);
				}
			else
				{
				NewImageWindow(
					lpImage,				// lpOldFrame
					lpImage->CurFile,		// Name
					ImgGetBaseEditFrame(lpImage), 	// lpNewFrame
					lpImage->FileType, 		// lpImage->FileType
					lpImage->DataType, 		// lpImage->DataType
					FALSE,					// New view?
					lpImage->DocumentType,	// lpImage->DocumentType
					lpImage->ImageName,		// lpImage->ImageName
					MAYBE					// Maximized?
					);
				}
			}
		}
	SetCurView(hOldWnd, NULL);
	}
}

/***********************************************************************/
static int AstralImagePaint( HWND hWindow )
/***********************************************************************/
{
RECT rRepair;		// are we are supposed to be repairing
RECT rInvalid;		// invalid are remainging after PaintImage
RECT UpdateRect;	// area that needs to be invalidated after abort
PAINTSTRUCT ps;
HDC hDC;
HWND hOldWnd;		// save of window that has current view

// Begin AstralImagePaint - ruttsa ruck bub!!!

// Set up lpImage and lpDisplay pointers for this window
if (!SetCurView(hWindow, &hOldWnd))
	return(NO);

// Update our rulers, scroll bars, and rates from possible new view
UpdateRulers();
SetupScrollBars( hWindow );
SetupRates( hWindow );

// Turn off our brush display for painting, make sure it
// happens before erase background
// *** Moved to WM_ERASEBKGND message processing ***
if (hWindow == hActiveWindow)
	DisplayBrush(0, 0, 0, OFF);

// Need to paint the image into the image window
hDC = BeginPaint( hWindow, &ps );
SetMapMode( hDC, MM_TEXT ); // work in pixel mode

// get rectangle that needs repainting
rRepair = ps.rcPaint;
WindowsToAstralRect(&rRepair);

// Save the active window so we can tell whether a
// document activation took place during ImgPaint, caused
// by a PeekMessage call.  This abort stuff is a pain in the Meat Whistle
hPaintWnd = hWindow; // needed for stitching?
AstralSetRectEmpty(&rInvalid);
ImgPaint( lpImage, hDC, &rRepair, fAllowPaintAbort ? &rInvalid : (LPRECT)NULL);
hPaintWnd = NULL;

// make sure our window still exists - maybe PeekMessage
// caused our window to be destroyed
// we ignore abort message in this cause just because
// it is too dangerous for us to handle right now - yikes!!!
if (!IsDoc(hWindow))
	{
	EndPaint( hWindow, &ps );
	MessageBeep(0);
	return(NO);
	}

// if document activation occurred, lpImage and lpDisplay must
// have also changed, so reset for our window or we are fucked!!!
if (!lpImage || lpImage->hWnd != hWindow)
	SetCurView(hWindow, &hOldWnd);

// See if we aborted and have image left to paint
// If so, we need to save the area still in need
// of painting and process the message that aborted us
if (fAllowPaintAbort && !AstralIsRectEmpty(&rInvalid))
	{
	// Indicate that we're finished painting the entire client area
	EndPaint( hWindow, &ps );

	// save area needing repair
	AstralUnionRect(&UpdateRect, &lpImage->lpDisplay->UpdateRect, &rInvalid);

	// set the windows update area to NULL
	AstralSetRectEmpty(&lpImage->lpDisplay->UpdateRect);

	// set back lpImage and lpDisplay and invalidate our area
	// needing repair
	SetCurView(hOldWnd, NULL);
	if (!AstralIsRectEmpty(&UpdateRect))
		InvalidateRect(hWindow, AstralToWindowsRect(&UpdateRect), FALSE);
	}
else
	{
	// Indicate that we're finished painting the entire client area
	EndPaint( hWindow, &ps );

	// set the windows area to NULL
	AstralSetRectEmpty(&lpImage->lpDisplay->UpdateRect);

	// set back lpImage and lpDisplay to what they were before
	SetCurView(hOldWnd, NULL);
	}

// Successful, unbelievable!!!
return( TRUE );
}


/************************************************************************/
BOOL AbortUpdate()
/************************************************************************/
{ // Only interrupt on things that the user initiates
#ifdef _MAC
	return FALSE;
#else
	MSG msg;

	if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE|PM_NOYIELD ) )
		return(TRUE);
	return(FALSE);
#endif
}

/************************************************************************/
void SetTitleBar( HWND hWindow )
/************************************************************************/
{
int Percentage;
LPSTR lpStr;
char szTitle[MAX_STR_LEN];
char szLastTitle[MAX_STR_LEN];
// Redraw the title bar with the proper view percentage
AstralStr( IDS_PREVIEW, &lpStr );
if ( !(Percentage = lpImage->lpDisplay->ViewPercentage) )
	Percentage = FMUL( 100, lpImage->lpDisplay->DispRate );
lpImage->lpDisplay->ViewPercentage = Percentage;

Lowercase( lpImage->CurFile );
if ( IsIconic( hWindow ) )
		lstrcpy( szTitle, stripdir(lpImage->CurFile) );
else	wsprintf( szTitle, lpStr, Percentage, stripdir(lpImage->CurFile) );
GetWindowText( hWindow, szLastTitle, MAX_STR_LEN);
if (!StringsEqual(szTitle, szLastTitle))
	SetWindowText( hWindow, szTitle );
}


/************************************************************************/
static void SetupScrollBars( HWND hWindow )
/************************************************************************/
{
int dx, dy, x, y, xMinPos, yMinPos, xMaxPos, yMaxPos, xPos, yPos;

// Setup the scroll bar values based on the lpImage->lpDisplay->FileRect
dx = lpImage->npix - RectWidth( &lpImage->lpDisplay->FileRect );
dx = max(dx, 0);
x = lpImage->lpDisplay->FileRect.left;
GetScrollRange( hWindow, SB_HORZ, &xMinPos, &xMaxPos);
xPos = GetScrollPos(hWindow, SB_HORZ);
if (xMinPos != 0 || xMaxPos != dx || xPos != x)
	{
	SetScrollRange( hWindow, SB_HORZ, 0, dx, NO );
	}
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
static void SetupRates( HWND hWindow )
/************************************************************************/
{
int DispHeight, DispWidth;

DispHeight = RectHeight(&lpImage->lpDisplay->DispRect);
DispWidth = RectWidth(&lpImage->lpDisplay->DispRect);
// Always set new rates to be as accurate as possible
if (DispHeight > 0 && DispWidth > 0)
	{
	if (DispWidth > DispHeight)
		{
		lpImage->lpDisplay->FileRate = 
			FGET(RectWidth( &lpImage->lpDisplay->FileRect ), DispWidth );
		lpImage->lpDisplay->DispRate = 
			FGET(DispWidth, RectWidth( &lpImage->lpDisplay->FileRect ) );
		}
	else
		{
		lpImage->lpDisplay->FileRate = 
			FGET(RectHeight( &lpImage->lpDisplay->FileRect),DispHeight);
		lpImage->lpDisplay->DispRate = 
			FGET(DispHeight, RectHeight(&lpImage->lpDisplay->FileRect ) );
		}
	}
// Draw the title bar with the new view percentage: lpImage->lpDisplay->DispRate
SetTitleBar( hWindow );
}

// This routine returns the file pixel which is underneath the
// passed in display pixel. (i.e. - no rounding)
/************************************************************************/
void Display2File(
/************************************************************************/
LPINT	lpx,
LPINT	lpy)
{
int	x, y;
LFIXED 	fx, fy;
LFIXED xrate, yrate;
RECT rDest, rSource;

/* determine location on the display image */
x = *lpx - lpImage->lpDisplay->DispRect.left;
y = *lpy - lpImage->lpDisplay->DispRect.top;

#ifdef _MAC
CopyRect(&lpImage->lpDisplay->DispRect, &lpImage->lpDisplay->FileRect);
#endif
rDest = lpImage->lpDisplay->DispRect;
rSource = lpImage->lpDisplay->FileRect;
xrate = FGET(RectWidth(&rSource), RectWidth(&rDest));
yrate = FGET(RectHeight(&rSource), RectHeight(&rDest));

fx = (long)x * xrate;
fy = (long)y * yrate;

#ifdef WIN32
x = WHOLE(fx);
y = WHOLE(fy);
#else
x = HIWORD(fx);
y = HIWORD(fy);
#endif

x += lpImage->lpDisplay->FileRect.left;
y += lpImage->lpDisplay->FileRect.top;

*lpx = x;
*lpy = y;
}

/************************************************************************/
void File2Display(
/************************************************************************/
LPINT	lpx,
LPINT	lpy)
{
File2DisplayEx(lpx, lpy, NO);
}

/************************************************************************/
void File2DisplayEx( LPINT lpx, LPINT lpy, BOOL fCenter )
/************************************************************************/
{
int	x, y;
LFIXED xrate, yrate;
RECT rDest, rSource;

/* determine location on hi-res image */
x = *lpx - lpImage->lpDisplay->FileRect.left;
y = *lpy - lpImage->lpDisplay->FileRect.top;

#ifdef _MAC
CopyRect(&lpImage->lpDisplay->DispRect, &lpImage->lpDisplay->FileRect);
#endif
rDest = lpImage->lpDisplay->DispRect;
rSource = lpImage->lpDisplay->FileRect;
xrate = FGET(RectWidth(&rDest), RectWidth(&rSource));
yrate = FGET(RectHeight(&rDest), RectHeight(&rSource));

if (fCenter)
	{
	x = FMUL( 1, ((long)x*xrate) + ((long)xrate>>1) );
	y = FMUL( 1, ((long)y*yrate) + ((long)yrate>>1) );
	}
else
	{
	x = FMUL( x, xrate );
	y = FMUL( y, yrate );
	}

x += lpImage->lpDisplay->DispRect.left;
y += lpImage->lpDisplay->DispRect.top;

*lpx = x;
*lpy = y;
}



/************************************************************************/
void File2DispRect( LPRECT fileRect, LPRECT dispRect )
/************************************************************************/
{
if (lpImage)
	File2DispRectEx(lpImage->hWnd, fileRect, dispRect);
}

/************************************************************************/
void File2DispRectEx( HWND hWnd, LPRECT fileRect, LPRECT dispRect )
/************************************************************************/
{
RECT rDisp, rFile;
LFIXED xrate, yrate;
LPDISPLAY lpDisplay;

lpDisplay = (LPDISPLAY)GetWindowLong(hWnd, GWL_DISPLAYPTR);

#ifdef _MAC
CopyRect(&lpDisplay->DispRect, &lpDisplay->FileRect);
#endif
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

lpDisplay = (LPDISPLAY)GetWindowLong(hWnd, GWL_DISPLAYPTR);

#ifdef _MAC
CopyRect(&lpDisplay->DispRect, &lpDisplay->FileRect);
#endif
rDisp = lpDisplay->DispRect;
rFile = lpDisplay->FileRect;
xrate = FGET(RectWidth(&rDisp), RectWidth(&rFile));
yrate = FGET(RectHeight(&rDisp), RectHeight(&rFile));

dispRect->left = FMUL( fileRect->left - rFile.left, xrate ) + rDisp.left;
dispRect->top = FMUL( fileRect->top - rFile.top, yrate ) + rDisp.top;
dispRect->right = FMUL( fileRect->right - rFile.left + 1, xrate ) + rDisp.left - 1;
dispRect->bottom = FMUL( fileRect->bottom - rFile.top + 1, yrate ) + rDisp.top - 1;
}


///************************************************************************/
//void File2DispTForm(LPTFORM lpTForm)
///************************************************************************/
//{
//File2DispTFormEx(lpTForm, NO);
//}
//
///************************************************************************/
//void File2DispTFormEx(LPTFORM lpTForm, BOOL fCenter)
///************************************************************************/
//{
//LFIXED rate;
//
//rate = lpImage->lpDisplay->DispRate;
//TMove( lpTForm, -lpImage->lpDisplay->FileRect.left,
//	-lpImage->lpDisplay->FileRect.top );
//TScale( lpTForm, rate, rate);
//TMove( lpTForm, lpImage->lpDisplay->DispRect.left,
//	lpImage->lpDisplay->DispRect.top );
//if (fCenter)
//	{
//	rate = (long)rate >> 1;
//	TFMove(lpTForm, rate, rate);
//	}
//}

/************************************************************************/
void Disp2FileTForm(LPTFORM lpTForm)
/************************************************************************/
{
TMove( lpTForm, -lpImage->lpDisplay->DispRect.left,
	-lpImage->lpDisplay->DispRect.top );
TScale( lpTForm, lpImage->lpDisplay->FileRate, lpImage->lpDisplay->FileRate);
TMove( lpTForm, lpImage->lpDisplay->FileRect.left,
	lpImage->lpDisplay->FileRect.top );
}


/************************************************************************/
BOOL ImageIsDoc( LPIMAGE lpImageDoc )
/************************************************************************/
{
int i;
LPIMAGE lpImageToCheck;
HWND hWnd;

for ( i=0; i<NumDocs(); i++ )
	{
	hWnd = GetDoc( i );
	lpImageToCheck = (LPIMAGE)GetWindowLong( hWnd, GWL_IMAGEPTR );
	if ( lpImageToCheck == lpImageDoc )
		return( YES );
	}

return( NO );
}


/***********************************************************************/
LPIMAGE ImageSet( LPIMAGE lpImage )
/***********************************************************************/
{
HWND hOldWnd;

if ( !lpImage )
	return( NULL );

if ( !SetCurView( lpImage->hWnd, &hOldWnd ) )
	return( NULL );

return( (LPIMAGE)GetWindowLong( hOldWnd, GWL_IMAGEPTR ) );
}


/***********************************************************************/
BOOL SetCurView(
/***********************************************************************/
HWND 	hWnd,
HWND 	far * lphOldWnd)
{
static HWND hSaveWnd;
LPIMAGE lpOldImage;
LPDISPLAY lpOldDisplay;

if (lphOldWnd)
	*lphOldWnd = hSaveWnd;
if ( !IsDoc(hWnd) )
	return(FALSE);
lpOldImage = lpImage;
if (hWnd)
	{
	if ( lpImage = (LPIMAGE)GetWindowLong( hWnd, GWL_IMAGEPTR ) )
		{
		lpOldDisplay = lpImage->lpDisplay;
		lpImage->lpDisplay = (LPDISPLAY)GetWindowLong(hWnd, GWL_DISPLAYPTR);
		if (lpImage->lpDisplay)
			{
			lpImage->hWnd = hWnd;
			frame_set(ImgGetBaseEditFrame(lpImage));
			hSaveWnd = hWnd;
			return(TRUE);
			}
		else
			{
			lpImage->lpDisplay = lpOldDisplay;
			lpImage = lpOldImage;
			}
		}
	else
		{
		lpImage = lpOldImage;
		}
	}
return(FALSE);
}

/***********************************************************************/
HWND ActivateDocument( HWND hWnd )
/***********************************************************************/
{
HWND hOldActiveWindow;

if ( !(hOldActiveWindow = hActiveWindow) )
	hOldActiveWindow = hWnd;

// if we have an HDC, always release it in here for activate or deactivate
if ( Window.hDC )
	{
	ReleaseDC( hActiveWindow, Window.hDC );
	Window.hDC = NULL;
	}

// Handle a deactivation
if ( !hWnd )
	{
	if ( hActiveWindow )	// any current window active?
		{
		DeactivateTool();				// deactivate current tool
		EnableMaskDisplay(lpImage, NO);	// undraw marquee or slime
		SetupMiniViews(NULL, NO);
		hActiveWindow = NULL;			// no more active window
		}
	lpImage = NULL;				// no active image
	SetUnitResolution( 0 );		// reset unit resolution - why here?
	return( hOldActiveWindow );	// return last active window
	}

// Handle an activation
Window.hDC = GetDC( hActiveWindow = hWnd );
if (SetCurView(hActiveWindow, NULL))  // setup lpImage and lpImage->lpDisplay
	{
	SetupImagePalette(lpImage, NO, YES);
	EnableMaskDisplay(lpImage, YES); // turn on marquee or redraw slime
	// setup title bar, and resolution
	SetTitleBar( hWnd );
	SetUnitResolution( 0 );
	SetupMiniViews(NULL, NO);
	DisplayInfo(MAKELONG(0, 0), NULL);	// redisplay info for this tyhpe

	}
// make sure status bar is up to data
UpdateStatusBar( YES, YES, YES, NO );

return( hOldActiveWindow );
}

/***********************************************************************/
void SetupImagePalette( LPIMAGE lpTheImage, BOOL fDepthChange, 
						BOOL fRealizePalette )
/***********************************************************************/
{
LPOBJECT lpObject;
HDC hDC;

if (fDepthChange)
	{
	lpObject = ImgGetDisplayObject(lpTheImage);
	if (!lpObject)
		return;
	if ( FrameDepth(ObjGetEditFrame(lpObject)) == 1 )
		{
		lpTheImage->hPal = lpBltScreen->hGrayPal;
		lpTheImage->nPaletteEntries = lpBltScreen->nGrayEntries;
		copy((LPTR)lpBltScreen->GrayPalette,
			 (LPTR)lpTheImage->Palette,
			 sizeof(lpTheImage->Palette));
		}
	else
		{
		lpTheImage->hPal = lpBltScreen->hColorPal;
		lpTheImage->nPaletteEntries = lpBltScreen->nColorEntries;
		copy((LPTR)lpBltScreen->ColorPalette,
			 (LPTR)lpTheImage->Palette,
			 sizeof(lpTheImage->Palette));
		}
	}
if (fRealizePalette && lpTheImage->hWnd)
	{
	if (lpBltScreen->hPal = lpTheImage->hPal) // any new palette?
		{	// select and realize palette for this image
		hDC = GetDC(lpTheImage->hWnd);
		SelectPalette( Window.hDC, lpBltScreen->hPal, FALSE );
		RealizePalette( Window.hDC );
		ReleaseDC(lpTheImage->hWnd, hDC);
		}
	// Always update the active colors
	RepaintColors();
	}
}

