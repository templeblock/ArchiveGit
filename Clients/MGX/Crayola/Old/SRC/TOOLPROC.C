//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Retouch statics
static POINT StartPoint;
static RECT ClientRect, SelectRect;

static BOOL bLeft = TRUE;  // cursor direction


/************************************************************************/
int CustomProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
int x, y;
BOOL fGotaRect;
static BOOL fSelection;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Tool.bActive = YES;
	break;

	case WM_LBUTTONDOWN:
	ClientRect = lpImage->lpDisplay->DispRect;
	StartPoint = MAKEPOINT(lParam);
	fSelection = NO;
	break;

	case WM_LBUTTONUP:
	Tool.bActive = NO;
	if (fSelection)
		{
		EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
		if (abs(SelectRect.right-SelectRect.left) <= SMALL_MOVEMENT &&
			abs(SelectRect.bottom-SelectRect.top) <= SMALL_MOVEMENT)
				fGotaRect = NO;
		else	fGotaRect = YES;
		}
	else
		{
		fGotaRect = NO;
		Display2File( (LPINT)&StartPoint.x, (LPINT)&StartPoint.y );
		SelectRect.left = SelectRect.right = StartPoint.x;
		SelectRect.top = SelectRect.bottom = StartPoint.y;
		}

	if ( fGotaRect )
		ViewRect(&SelectRect, View.ZoomWindow);
	else
		{
		SaveLastView();
		Zoom( SelectRect.left, SelectRect.top,
			( View.ZoomOut ^ SHIFT ) ? -100 : +100, YES,
			( View.ZoomWindow ^ CONTROL ) );
		}
	Tool.bActive = NO;
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	if (!fSelection)
		{
		if (fSelection = (abs(StartPoint.x-x) > SMALL_MOVEMENT ||
			abs(StartPoint.y-y) > SMALL_MOVEMENT))
			{
			AstralUpdateWindow(hWindow);
			SetRect( &SelectRect, 32767, 32767, -32767, -32767 );
			StartSelection( hWindow, NULL, &SelectRect,
				SL_BOX | SL_SPECIAL, StartPoint, 0L );
			}
		}
	if (fSelection)
		UpdateSelection( hWindow, NULL, &SelectRect, SL_BOX,
			MAKEPOINT(lParam), CONSTRAINASPECT, 1L, 1L, Window.fRButtonDown,FROMCENTER);
	break;

	case WM_LBUTTONDBLCLK:
	break;

	case WM_DESTROY:	// The cancel operation message
	if ( !Tool.bActive )
		break;
	Tool.bActive = NO;
	if (fSelection)
		EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
	break;
	}
if (!Tool.bActive)
	InstallTool(NO);
return(TRUE);
}


/************************************************************************/
int GrabberProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
int x, y, dx, dy, fx, fy, fxleft, fxright, fytop, fybottom;
RECT rScroll;
static POINT Anchor;

x = LOWORD( lParam );
y = HIWORD( lParam );
switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Tool.bActive = YES;
	break;

	case WM_LBUTTONDOWN:
	Anchor.x = x;
	Anchor.y = y;
	Display2File((LPINT)&Anchor.x, (LPINT)&Anchor.y);
	EnableMarquee( NO );
	RemoveMarquee();
	break;

	case WM_LBUTTONUP:
	EnableMarquee( YES );
	Tool.bActive = NO;
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	if ( !Window.fLButtonDown )
		break;
	Display2File(&x, &y);
	fx = x - Anchor.x;
	fy = y - Anchor.y;
	fxleft = lpImage->lpDisplay->FileRect.left;
	fxright = lpImage->lpDisplay->FileRect.right - lpImage->npix + 1; 				fytop = lpImage->lpDisplay->FileRect.top;
	fybottom = lpImage->lpDisplay->FileRect.bottom - lpImage->nlin + 1;
	if (fx > 0 && fx > fxleft)
		fx = fxleft;
	if (fx < 0 && fx < fxright)
		fx = fxright;
	if (fy > 0 && fy > fytop)
		fy = fytop;
	if (fy < 0 && fy < fybottom)
		fy = fybottom;
	dx = FMUL( fx, lpImage->lpDisplay->DispRate );
	dy = FMUL( fy, lpImage->lpDisplay->DispRate );
	if ( !dx && !dy )
		break;
	rScroll = lpImage->lpDisplay->DispRect;
	rScroll.bottom++; rScroll.right++;
	if (dx)
		{
		lpImage->lpDisplay->FileRect.left -= fx;
		lpImage->lpDisplay->FileRect.right -= fx;
		lpImage->lpDisplay->xDiva -= dx;
		ScrollWindow( hWindow, dx, 0, &rScroll, &rScroll);
		}
	if (dy)
		{
		lpImage->lpDisplay->FileRect.top -= fy;
		lpImage->lpDisplay->FileRect.bottom -= fy;
		lpImage->lpDisplay->yDiva -= dy;
		ScrollWindow( hWindow, 0, dy, &rScroll, &rScroll);
		}
	AstralUpdateWindow( hWindow );
	SetupRulers();
	Anchor.x = x-fx;
	Anchor.y = y-fy;
	break;

	case WM_LBUTTONDBLCLK:
	break;

	case WM_DESTROY:	// The cancel operation message
	Tool.bActive = NO;
	break;
	}
return(TRUE);
}
