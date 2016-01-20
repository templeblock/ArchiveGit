//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"

static BOOL DADAccept(HWND hWnd);
static void DragDraw(LPDRAG lpDrag, BOOL on);
static void DragWhichWindow(LPDRAG lpDrag);

/***********************************************************************/
LPDRAG DragAndDropBegin(HWND hWnd, WORD wType, LPTR lpData, LPPOINT lpStartPos,
		HRGN hRgnIn, LPRECT lpRect)
/***********************************************************************/
{
LPDRAG lpDrag;

if (!(lpDrag = (LPDRAG)Alloc((long)sizeof(DRAG))))
	return(NULL);
lpDrag->hWnd = hWnd;
lpDrag->wType = wType;
lpDrag->lpData = lpData;
if (!(lpDrag->hRgn = hRgnIn))
	{
	if (lpRect)
		{
		if (!(lpDrag->hRgn = CreateRectRgnIndirect(lpRect)))
			{
			FreeUp((LPTR)lpDrag);
			return(NULL);
			}
		}
	}
if (lpStartPos)
	lpDrag->pos = *lpStartPos;
else
	GetCursorPos(&lpDrag->pos);
if (lpDrag->hRgn)
	ShowCursor(NO);
else
	{
	lpDrag->hOldCursor = GetCursor();
	lpDrag->hDragCopy = LoadCursor(PictPubApp.GetResourceHandle(),
				MAKEINTRESOURCE(ID_DRAGCOPY));
	lpDrag->hDragMove = LoadCursor(PictPubApp.GetResourceHandle(),
				MAKEINTRESOURCE(ID_DRAGMOVE));
	lpDrag->hDragNot = LoadCursor(PictPubApp.GetResourceHandle(),
				MAKEINTRESOURCE(ID_DRAGNOT));
	}
DragWhichWindow(lpDrag);
DragDraw(lpDrag, ON);
return(lpDrag);
}

/***********************************************************************/
BOOL DragAndDropMove(LPDRAG lpDrag, int x, int y)
/***********************************************************************/
{
POINT	NewPos;
int	dx, dy;

NewPos.x = x; NewPos.y = y;
ClientToScreen(lpDrag->hWnd, &NewPos);
dx = NewPos.x - lpDrag->pos.x;
dy = NewPos.y - lpDrag->pos.y;
if (dx || dy)
	{
	DragDraw(lpDrag, OFF);
	lpDrag->pos = NewPos;
	DragWhichWindow(lpDrag);
	if (lpDrag->hRgn)
		OffsetRgn(lpDrag->hRgn, dx, dy);
	DragDraw(lpDrag, ON);
	}
return(lpDrag->fAccept);
}

/***********************************************************************/
BOOL DragAndDropEnd(LPDRAG lpDrag)
/***********************************************************************/
{
BOOL	fAccept;

ScreenToClient(lpDrag->hDragWnd, &lpDrag->pos);
DragDraw(lpDrag, OFF);
if (lpDrag->fAccept)
	fAccept = SendMessage(lpDrag->hDragWnd, WM_DRAGACCEPT, 0,
		 	(long)lpDrag);
if (lpDrag->hRgn)
	{
	ShowCursor(YES);
	if (!lpDrag->hRgn)
		DeleteObject(lpDrag->hRgn);
	}
else
	SetCursor(lpDrag->hOldCursor);
FreeUp((LPTR)lpDrag);
return(fAccept);
}

/***********************************************************************/
static void DragWhichWindow(LPDRAG lpDrag)
/***********************************************************************/
{
POINT	ClientPos;

lpDrag->hDragWnd = WindowFromPoint(lpDrag->pos);
if (lpDrag->hDragWnd && DADAccept(lpDrag->hDragWnd))
	{
	ClientPos = lpDrag->pos;
	ScreenToClient(lpDrag->hDragWnd, &ClientPos);
	lpDrag->fAccept = SendMessage(lpDrag->hDragWnd, WM_DRAGQUERY,
				lpDrag->wType,
				MAKELONG(ClientPos.x, ClientPos.y));
	}
else
	lpDrag->fAccept = NO;
}

/***********************************************************************/
static void DragDraw(LPDRAG lpDrag, BOOL on)
/***********************************************************************/
{
HDC	hDC;
short	OldROP;

if (lpDrag->hRgn)
	{
	hDC = GetDC(NULL);
	OldROP = SetROP2(hDC, R2_NOT);
	FrameRgn(hDC, lpDrag->hRgn, (HBRUSH)GetStockObject(BLACK_BRUSH),
				1, 1); // turn it off
	SetROP2(hDC, OldROP);
	ReleaseDC(NULL, hDC);
	}
else if (on)
	{
	if (!lpDrag->fAccept)
		SetCursor(lpDrag->hDragNot);
	else if (lpDrag->hDragWnd == lpDrag->hWnd)
		SetCursor(lpDrag->hDragMove);
	else
		SetCursor(lpDrag->hDragCopy);
	}
}


/***********************************************************************/
static BOOL DADAccept(HWND hWnd)
/***********************************************************************/
{
STRING szClassName;

GetClassName(hWnd, szClassName, sizeof(szClassName));
if (StringsEqual(szClassName, "color2"))
	return(TRUE);
else 
if (StringsEqual(szClassName, "palette"))
	return(TRUE);
else 
if (StringsEqual(szClassName, "ppimage"))
	return(TRUE);
else 
if (StringsEqual(szClassName, "icon"))
	return(TRUE);
return(FALSE);
}

#ifdef UNUSED
/***********************************************************************/
LPDRAG DragAndDrop(HWND hWnd, WORD wType, LPTR lpData, LPPOINT lpStartPos,
		HRGN hRgnIn, LPRECT lpRect)
/***********************************************************************/
{
LPDRAG lpDrag;
HDC hDC;
short OldROP;
POINT LastPos, NewPos, ClientPos;
int	dx, dy;
HWND	hDragWnd;
BOOL 	fAccept;

if (!(lpDrag = (LPDRAG)Alloc((long)sizeof(DRAG))))
	return(NULL);
if (!(lpDrag->hRgn = hRgnIn))
	{
	if (lpRect)
		{
		if (!(lpDrag->hRgn = CreateRectRgnIndirect(lpRect)))
			{
			FreeUp((LPTR)lpDrag);
			return(NULL);
			}
		}
	}
lpDrag->LastPos = *lpStartPos;
if (lpDrag->hRgn)
	{
	if (!(hDC = GetDC(NULL)))
		{
		if (!hRgnIn)
			DeleteObject(hRgn);
		return(FALSE);
		}
	hBrush = GetStockObject(BLACK_BRUSH);
	OldROP = SetROP2(hDC, R2_NOT);
	ShowCursor(NO);
	}
else
	{
	hOldCursor = GetCursor();
	hDragCopy = LoadCursor(PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(ID_DRAGCOPY));
	hDragMove = LoadCursor(PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(ID_DRAGMOVE));
	hDragNot = LoadCursor(PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(ID_DRAGNOT));
	}
hDragWnd = WindowFromPoint(LastPos);
if (hDragWnd && DADAccept(hDragWnd))
	{
	ClientPos = LastPos;
	ScreenToClient(hDragWnd, &ClientPos);
	fAccept = SendMessage(hDragWnd, WM_DRAGQUERY, wType,
			 MAKELONG(ClientPos.x, ClientPos.y));
	}
else
	fAccept = NO;
if (hRgn)
	{
	FrameRgn(hDC, hRgn, hBrush, 1, 1); // turn it on
	}
else
	{
	if (!fAccept)
		SetCursor(hDragNot);
	else if (hDragWnd == hWnd)
		SetCursor(hDragMove);
	else
		SetCursor(hDragCopy);
	}
while (LBUTTON)
	{
	GetCursorPos(&NewPos);
	dx = NewPos.x - LastPos.x;
	dy = NewPos.y - LastPos.y;
	if (dx || dy)
		{
		LastPos = NewPos;
		hDragWnd = WindowFromPoint(LastPos);
		if (hDragWnd && DADAccept(hDragWnd))
			{
			ClientPos = LastPos;
			ScreenToClient(hDragWnd, &ClientPos);
			fAccept = SendMessage(hDragWnd, WM_DRAGQUERY, wType,
				MAKELONG(ClientPos.x, ClientPos.y));
			}
		else
			fAccept = NO;
		if (hRgn)
			{
			FrameRgn(hDC, hRgn, hBrush, 1, 1); // turn it off
			OffsetRgn(hRgn, dx, dy);
			FrameRgn(hDC, hRgn, hBrush, 1, 1); // turn it on
			}
		else
			{
			if (!fAccept)
				SetCursor(hDragNot);
			else if (hDragWnd == hWnd)
				SetCursor(hDragMove);
			else
				SetCursor(hDragCopy);
			}
		}
	}
if (hRgn)
	{
	FrameRgn(hDC, hRgn, hBrush, 1, 1); // turn it off
	ShowCursor(YES);
	SetROP2(hDC, OldROP);
	ReleaseDC(NULL, hDC);
	if (!hRgnIn)
		DeleteObject(hRgn);
	}
else
	SetCursor(hOldCursor);
if (fAccept)
	{
	drag.hWnd = hWnd;
	drag.wType = wType;
	drag.lpData = lpData;
	drag.pos = LastPos;
	ScreenToClient(hDragWnd, &drag.pos);
	fAccept = SendMessage(hDragWnd, WM_DRAGACCEPT, 0, (long)(LPDRAG)&drag);
	}
return(fAccept);
}

#endif


