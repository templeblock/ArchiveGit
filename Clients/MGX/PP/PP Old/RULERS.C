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

extern HANDLE hInstAstral;

#define RULER_STYLE(dwStyle) ((dwStyle) & 0x0F)

/***********************************************************************/
BOOL CreateRulers()
/***********************************************************************/
{
RECT ClientRect;

if (lpImage->lpDisplay->HasRulers)
	return;
GetClientRect(lpImage->hWnd, &ClientRect);
if (!CreateWindow("ruler", "", WS_CHILD|WS_DISABLED|WS_VISIBLE|RS_HORZ, 0, 0, RectWidth(&ClientRect), RULER_SIZE, lpImage->hWnd, NULL, hInstAstral, NULL))
	return(FALSE);
if (!CreateWindow("ruler", "", WS_CHILD|WS_DISABLED|WS_VISIBLE|RS_VERT, 0, 0, RULER_SIZE, RectHeight(&ClientRect), lpImage->hWnd, NULL, hInstAstral, NULL))
	{
	DestroyRulers();
	return(FALSE);
	}
lpImage->lpDisplay->HasRulers = YES;
ComputeDispRect();
ComputeFileRect(0, 0, lpImage->lpDisplay->FileRate );
SetupRulers();
InvalidateRect(lpImage->hWnd, NULL, TRUE);
return(TRUE);
}

/***********************************************************************/
void DestroyRulers()
/***********************************************************************/
{
HWND hChildWnd, hDestroyWnd;
STRING szClassName;

if (!lpImage->lpDisplay->HasRulers)
	return;

hChildWnd = GetWindow(lpImage->hWnd, GW_CHILD);
while (hChildWnd)
	{
	GetClassName(hChildWnd, szClassName, MAX_STR_LEN);
	if (StringsEqual(szClassName, "ruler"))
		{
		hDestroyWnd = hChildWnd;
		hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
		DestroyWindow(hDestroyWnd);
		}
	else
		hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
	}
lpImage->lpDisplay->HasRulers = NO;
ComputeDispRect();
ComputeFileRect(0, 0, lpImage->lpDisplay->FileRate );
InvalidateRect(lpImage->hWnd, NULL, TRUE);
}

/***********************************************************************/
void SetupAllRulers()
/***********************************************************************/
{
int i;
HWND hWnd, hOldWnd;

for (i = 0; i < NumDocs(); ++i)
	{
	hWnd = GetDoc(i);
	if (SetCurView(hWnd, &hOldWnd))
		{
		if (lpImage->lpDisplay->HasRulers)
			SetupRulers();
		SetCurView(hOldWnd, NULL);
		}
	}
}

/***********************************************************************/
void SetupRulers()
/***********************************************************************/
{
HWND hChildWnd;
STRING szClassName;
DWORD dwStyle;
RECT ClientRect;
LONG lRW, lRH, lFW, lFH, lDW, lDH;

if (!lpImage->lpDisplay->HasRulers)
	return;

hChildWnd = GetWindow(lpImage->hWnd, GW_CHILD);
while (hChildWnd)
	{
	GetClassName(hChildWnd, szClassName, MAX_STR_LEN);
	if (StringsEqual(szClassName, "ruler"))
		{
		GetClientRect(hChildWnd, &ClientRect);
		lRW = ClientRect.right-(ClientRect.left+RULER_SIZE-1);
		lRH = ClientRect.bottom-(ClientRect.top+RULER_SIZE-1);
		lFW = RectWidth(&lpImage->lpDisplay->FileRect);
		lFH = RectHeight(&lpImage->lpDisplay->FileRect);
		lDW = RectWidth(&lpImage->lpDisplay->DispRect);
		lDH = RectHeight(&lpImage->lpDisplay->DispRect);
		dwStyle = GetWindowLong(hChildWnd, GWL_STYLE);
		if (Control.Units == IDC_PREF_UNITMM)
			{
			SetWindowWord(hChildWnd, GWW_TICKSPERUNIT, 10);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH0, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH1, 2);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH2, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH3, 2);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH4, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH5, 2);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH6, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH7, 2);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH8, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH9, -1);
			SetWindowWord(hChildWnd, GWW_TICKSIZE, 3);
			SetWindowLong(hChildWnd, GWL_UNITSIZE, 100L);
			SetWindowLong(hChildWnd, GWL_UNITFACTOR, 10L);
			if (RULER_STYLE(dwStyle) == RS_HORZ)
				{
				SetWindowLong(hChildWnd, GWL_RULERSIZE, 			
(lFW*254L*lRW)/((long)lpImage->xres*lDW));
				SetWindowLong(hChildWnd, GWL_UNITSTART, ((long)lpImage->lpDisplay->FileRect.left * 254L)/lpImage->xres);
				}
			else if (RULER_STYLE(dwStyle) == RS_VERT)
				{
				SetWindowLong(hChildWnd, GWL_RULERSIZE, 			
(lFH*254L*lRH)/((long)lpImage->yres*lDH));
				SetWindowLong(hChildWnd, GWL_UNITSTART, ((long)lpImage->lpDisplay->FileRect.top * 254L)/lpImage->yres);
				}
			}
		else if (Control.Units == IDC_PREF_UNITPICAS)
			{
			SetWindowWord(hChildWnd, GWW_TICKSPERUNIT, 5);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH0, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH1, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH2, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH3, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH4, -1);
			SetWindowWord(hChildWnd, GWW_TICKSIZE, 6);
			SetWindowLong(hChildWnd, GWL_UNITSIZE, 50L);
			SetWindowLong(hChildWnd, GWL_UNITFACTOR, 10L);
			if (RULER_STYLE(dwStyle) == RS_HORZ)
				{
				SetWindowLong(hChildWnd, GWL_RULERSIZE, (lFW*723L*lRW)/((long)lpImage->xres*12L*lDW));
				SetWindowLong(hChildWnd, GWL_UNITSTART, ((long)lpImage->lpDisplay->FileRect.left * 723L)/((long)lpImage->xres*12L));
				}
			else if (RULER_STYLE(dwStyle) == RS_VERT)
				{
				SetWindowLong(hChildWnd, GWL_RULERSIZE, (lFH*723L*lRH)/((long)lpImage->yres*12L*lDH));
				SetWindowLong(hChildWnd, GWL_UNITSTART, ((long)lpImage->lpDisplay->FileRect.top * 723L)/((long)lpImage->yres*12L));
				}
			}
		else if (Control.Units == IDC_PREF_UNITCM)
			{
			SetWindowWord(hChildWnd, GWW_TICKSPERUNIT, 2);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH0, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH1, -1);
			SetWindowWord(hChildWnd, GWW_TICKSIZE, 6);
			SetWindowLong(hChildWnd, GWL_UNITSIZE, 100L);
			SetWindowLong(hChildWnd, GWL_UNITFACTOR, 100L);
			if (RULER_STYLE(dwStyle) == RS_HORZ)
				{
				SetWindowLong(hChildWnd, GWL_RULERSIZE, (lFW*254L*lRW)/((long)lpImage->xres*lDW));
				SetWindowLong(hChildWnd, GWL_UNITSTART, ((long)lpImage->lpDisplay->FileRect.left * 254L)/lpImage->xres);
				}
			else if (RULER_STYLE(dwStyle) == RS_VERT)
				{
				SetWindowLong(hChildWnd, GWL_RULERSIZE, (lFH*254L*lRH)/((long)lpImage->yres*lDH));
				SetWindowLong(hChildWnd, GWL_UNITSTART, ((long)lpImage->lpDisplay->FileRect.top * 254L)/lpImage->yres);
				}
			}
		else // IDC_PREF_UNITINCHES
			{
			SetWindowWord(hChildWnd, GWW_TICKSPERUNIT, 8);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH0, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH1, 2);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH2, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH3, 3);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH4, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH5, 2);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH6, 1);
			SetWindowWord(hChildWnd, GWW_TICKLENGTH7, -1);
			SetWindowWord(hChildWnd, GWW_TICKSIZE, 3);
			SetWindowLong(hChildWnd, GWL_UNITSIZE, 1000L);
			SetWindowLong(hChildWnd, GWL_UNITFACTOR, 1000L);
			if (RULER_STYLE(dwStyle) == RS_HORZ)
				{
				SetWindowLong(hChildWnd, GWL_RULERSIZE, (lFW * 1000L*lRW)/((long)lpImage->xres*lDW));
				SetWindowLong(hChildWnd, GWL_UNITSTART, ((long)lpImage->lpDisplay->FileRect.left * 1000L)/lpImage->xres);
				}
			else if (RULER_STYLE(dwStyle) == RS_VERT)
				{
				SetWindowLong(hChildWnd, GWL_RULERSIZE, (lFH * 1000L*lRH)/(lpImage->yres*lDH));
				SetWindowLong(hChildWnd, GWL_UNITSTART, ((long)lpImage->lpDisplay->FileRect.top * 1000L)/lpImage->yres);
				}
			}
		InvalidateRect(hChildWnd, NULL, TRUE);
		UpdateWindow(hChildWnd);
		}
	hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
	}
}

/***********************************************************************/
void SetRulerTicks(lParam)
/***********************************************************************/
LONG lParam;
{
HWND hChildWnd;
STRING szClassName;

if (!lpImage->lpDisplay->HasRulers)
	return;

hChildWnd = GetWindow(lpImage->hWnd, GW_CHILD);
while (hChildWnd)
	{
	GetClassName(hChildWnd, szClassName, MAX_STR_LEN);
	if (StringsEqual(szClassName, "ruler"))
		SendMessage(hChildWnd, WM_MOUSEMOVE, 0, lParam);
	hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
	}
}

/***********************************************************************/
void SizeRulers()
/***********************************************************************/
{
HWND hChildWnd;
STRING szClassName;
DWORD dwStyle;
RECT ClientRect;

if (!lpImage->lpDisplay->HasRulers)
	return;

hChildWnd = GetWindow(lpImage->hWnd, GW_CHILD);
while (hChildWnd)
	{
	GetClassName(hChildWnd, szClassName, MAX_STR_LEN);
	if (StringsEqual(szClassName, "ruler"))
		{
		dwStyle = GetWindowLong(hChildWnd, GWL_STYLE);
		GetClientRect(lpImage->hWnd, &ClientRect);
		if (RULER_STYLE(dwStyle) == RS_HORZ)
			{
			MoveWindow(hChildWnd, 0, 0, RectWidth(&ClientRect), RULER_SIZE, NO);
			}
		else if (RULER_STYLE(dwStyle) == RS_VERT)
			{
			MoveWindow(hChildWnd, 0, 0, RULER_SIZE, RectHeight(&ClientRect), NO);
			}
		}
	hChildWnd = GetWindow(hChildWnd, GW_HWNDNEXT);
	}
}

/***********************************************************************/
long FAR PASCAL RulerControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
RECT ClientRect;
PAINTSTRUCT ps;
HDC hDC;
DWORD dwStyle;
int x, y;

switch ( message )
    {
    case WM_CREATE:
	break;

    case WM_SETFONT:
	SetWindowWord(hWindow, GWW_FONT, wParam);
	if (lParam)
		{
		InvalidateRect(hWindow, NULL, TRUE);
		UpdateWindow(hWindow);
		}
	break;

    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	return(TRUE);
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );
	EraseControlBackground( hDC, hWindow, &ClientRect,
		CTLCOLOR_CUSTOM );
	PaintRuler(hDC, hWindow);
	x = GetWindowWord(hWindow, GWW_XPOS);
	y = GetWindowWord(hWindow, GWW_YPOS);
	if (x && y)
		{
		PaintRulerTick(hDC, hWindow, x, y);
		}
	EndPaint( hWindow, &ps );
	break;

    case WM_MOUSEMOVE:
	x = GetWindowWord(hWindow, GWW_XPOS);
	y = GetWindowWord(hWindow, GWW_YPOS);
	hDC = GetDC(hWindow);
	if (x && y)
		{
		PaintRulerTick(hDC, hWindow, x, y);
		}
	x = LOWORD(lParam);
	y = HIWORD(lParam);
	if (x && y)
		{
		PaintRulerTick(hDC, hWindow, x, y);
		}
	SetWindowWord(hWindow, GWW_XPOS, x);
	SetWindowWord(hWindow, GWW_YPOS, y);
	ReleaseDC(hWindow, hDC);
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}

/***********************************************************************/
void PaintRuler( hDC, hWnd )
/***********************************************************************/
HDC hDC;
HWND hWnd;
{
DWORD dwStyle;

dwStyle = GetWindowLong(hWnd, GWL_STYLE);
if (dwStyle & RS_HORZ)
	HorzRulerPaint(hDC, hWnd);
else if (dwStyle & RS_VERT)
	VertRulerPaint(hDC, hWnd);
}

/***********************************************************************/
VOID HorzRulerPaint(hDC, hWindow)
/***********************************************************************/
HDC 	   hDC;
HWND	   hWindow;
{
RECT ClientRect;
char text[256];
int iTicksPerUnit;
int iTickSize;
long lRulerSize, lUnitSize, lUnitFactor;
int ticklength[16];
int x, y, num_ticks, index, i, bottom, top;
FIXED rate, offset;
long lValue, lStartValue, lTickUnit;
int num;
HPEN hOldPen;
HFONT hOldFont;
char fontname[256];
int fontsize;
TEXTMETRIC info;
DWORD dwSize;

GetDrawClientRect( hWindow, &ClientRect );
iTicksPerUnit = GetWindowWord(hWindow, GWW_TICKSPERUNIT);
if (iTicksPerUnit > 16)
	iTicksPerUnit = 16;
for (i = 0; i < iTicksPerUnit; ++i)
	{
	ticklength[i] = GetWindowWord(hWindow, GWW_TICKLENGTH0+(i*2));
	}
iTickSize = GetWindowWord(hWindow, GWW_TICKSIZE);
lUnitSize = GetWindowLong(hWindow, GWL_UNITSIZE);
lRulerSize = GetWindowLong(hWindow, GWL_RULERSIZE);
lUnitFactor = GetWindowLong(hWindow, GWL_UNITFACTOR);
lStartValue = GetWindowLong(hWindow, GWL_UNITSTART);

hOldPen = SelectObject(hDC, Window.hBlackPen);
MoveTo(hDC, ClientRect.left, ClientRect.bottom);
LineTo(hDC, ClientRect.right, ClientRect.bottom);
ClientRect.left += (RULER_SIZE-1);
MoveTo(hDC, ClientRect.left, ClientRect.top);
LineTo(hDC, ClientRect.left, ClientRect.bottom);
SelectObject(hDC, hOldPen);

hOldFont = SelectObject(hDC, Window.hLabelFont);
GetTextMetrics(hDC, &info);

hOldPen = SelectObject(hDC, Window.hBlackPen);

rate = FGET((long)(ClientRect.right-ClientRect.left)*lUnitSize, lRulerSize);
rate /= iTicksPerUnit;
while (rate < FGET(2, 1) && iTicksPerUnit > 1)
	{
	iTicksPerUnit /= 2;
	for (i = 0; i < iTicksPerUnit; ++i)
		ticklength[i] = ticklength[(i*2)+1];
	rate = FGET((long)(ClientRect.right-ClientRect.left)*lUnitSize, lRulerSize);
	rate /= iTicksPerUnit;
	}

offset = FGET(1, 2);
num_ticks = (((lRulerSize*(long)iTicksPerUnit)+(lUnitSize/2))/lUnitSize);

lTickUnit = lUnitSize / iTicksPerUnit;
lValue = 0;
i = -1;
while (lValue < lStartValue)
	{
	i++;
	lValue += lTickUnit;
	}
if (lValue == lStartValue)
	{
	i++;
	offset += rate;
	}
else
	offset += ((lValue-lStartValue)*rate)/lTickUnit;
lValue = (lValue/lUnitSize)*lUnitFactor;
i = i % iTicksPerUnit;
num_ticks += i;

bottom = ClientRect.bottom;
for (; i < num_ticks; ++i)
    {
    x = ClientRect.left + HIWORD(offset);
    offset += rate;
    index = i % iTicksPerUnit;
    if (ticklength[index] < 0)
	top = ClientRect.top;
    else
	top  = bottom - (ticklength[index]*iTickSize);
    MoveTo(hDC, x, top);
    LineTo(hDC, x, bottom);
    if (index == (iTicksPerUnit-1))
	{
	lValue += lUnitSize;
	num = (lValue+lUnitFactor/2L)/lUnitFactor;
	wsprintf(text, "%d", num);
	dwSize = GetTextExtent(hDC, text, lstrlen(text));
//	x = x - (LOWORD(dwSize)) - 1;
	x = x + 1;
//	y = ClientRect.top + ((top-ClientRect.top-info.tmAscent)/2);
	y = ClientRect.top;
        ColorText(hDC, x, y,
	  text, lstrlen(text), GetSysColor(COLOR_WINDOWTEXT));
	}
    }
SelectObject(hDC, hOldPen);
SelectObject(hDC, hOldFont);
}

/***********************************************************************/
VOID VertRulerPaint(hDC, hWindow)
/***********************************************************************/
HDC 	   hDC;
HWND	   hWindow;
{
RECT ClientRect;
char text[256], achar[10];
int iTicksPerUnit;
int iTickSize;
long lRulerSize, lUnitSize, lUnitFactor;
int ticklength[16];
int x, y, num_ticks, index, i, right, left, j;
FIXED rate, offset;
long lValue, lStartValue, lTickUnit;
int num;
HPEN hOldPen;
HFONT hOldFont;
char fontname[256];
int fontsize;
TEXTMETRIC info;
DWORD dwSize;
BOOL fDoLabels;

GetDrawClientRect( hWindow, &ClientRect );
iTicksPerUnit = GetWindowWord(hWindow, GWW_TICKSPERUNIT);
if (iTicksPerUnit > 16)
	iTicksPerUnit = 16;
for (i = 0; i < iTicksPerUnit; ++i)
	ticklength[i] = GetWindowWord(hWindow, GWW_TICKLENGTH0+(i*2));

iTickSize = GetWindowWord(hWindow, GWW_TICKSIZE);
lUnitSize = GetWindowLong(hWindow, GWL_UNITSIZE);
lRulerSize = GetWindowLong(hWindow, GWL_RULERSIZE);
lUnitFactor = GetWindowLong(hWindow, GWL_UNITFACTOR);
lStartValue = GetWindowLong(hWindow, GWL_UNITSTART);

hOldPen = SelectObject(hDC, Window.hBlackPen);
MoveTo(hDC, ClientRect.right, ClientRect.top);
LineTo(hDC, ClientRect.right, ClientRect.bottom);
ClientRect.top += (RULER_SIZE-1);
MoveTo(hDC, ClientRect.left, ClientRect.top);
LineTo(hDC, ClientRect.right, ClientRect.top);
SelectObject(hDC, hOldPen);
hOldFont = SelectObject(hDC, Window.hLabelFont);
GetTextMetrics(hDC, &info);

hOldPen = SelectObject(hDC, Window.hBlackPen);

rate = FGET((long)(ClientRect.bottom-ClientRect.top)*lUnitSize, lRulerSize);
rate /= iTicksPerUnit;
while (rate < FGET(2, 1) && iTicksPerUnit > 1)
	{
	iTicksPerUnit /= 2;
	for (i = 0; i < iTicksPerUnit; ++i)
		ticklength[i] = ticklength[(i*2)+1];
	rate = FGET((long)(ClientRect.bottom-ClientRect.top)*lUnitSize, lRulerSize);
	rate /= iTicksPerUnit;
	}

offset = FGET(1, 2);
num_ticks = (((lRulerSize*(long)iTicksPerUnit)+(lUnitSize/2))/lUnitSize);

lTickUnit = lUnitSize / iTicksPerUnit;
lValue = 0;
i = -1;
while (lValue < lStartValue)
	{
	i++;
	lValue += lTickUnit;
	}
if (lValue == lStartValue)
	{
	i++;
	offset += rate;
	}
else
	offset += ((lValue-lStartValue)*rate)/lTickUnit;
lValue = (lValue/lUnitSize)*lUnitFactor;
i = i % iTicksPerUnit;
num_ticks += i;

right = ClientRect.right;
for (; i < num_ticks; ++i)
    {
    y = ClientRect.top + HIWORD(offset);
    offset += rate;
    index = i % iTicksPerUnit;
    if (ticklength[index] < 0)
	left = ClientRect.left;
    else
	left = right - (ticklength[index]*iTickSize);
    MoveTo(hDC, left, y);
    LineTo(hDC, right, y);
    if (index == (iTicksPerUnit-1))
	{
	lValue += lUnitSize;
	num = (lValue+lUnitFactor/2L)/lUnitFactor;
	wsprintf(text, "%d", num);
	dwSize = GetTextExtent(hDC, text, lstrlen(text));
//	x = left - LOWORD(dwSize)-3;
//	if (x <= ClientRect.left)
//	    x = ClientRect.left+1;
	x = ClientRect.left + 2;
//	y -= ((info.tmAscent+1));
	y += 0;
	achar[1] = '\0';
	for (j = 0; j < lstrlen(text); ++j)
		{
		achar[0] = text[j];
        	ColorText(hDC, x, y,
	  achar, 1, GetSysColor(COLOR_WINDOWTEXT));
		y += (info.tmAscent-1);
		}
	}
    }
SelectObject(hDC, hOldPen);
SelectObject(hDC, hOldFont);
}

/***********************************************************************/
void PaintRulerTick( hDC, hWnd, x, y )
/***********************************************************************/
HDC hDC;
HWND hWnd;
int x, y;
{
DWORD dwStyle;
RECT ClientRect;
HPEN hOldPen;
short OldROP;

GetDrawClientRect(hWnd, &ClientRect);
OldROP = SetROP2(hDC, R2_NOTXORPEN);
hOldPen = SelectObject(hDC, Window.hDottedPen);
dwStyle = GetWindowLong(hWnd, GWL_STYLE);
if (dwStyle & RS_HORZ)
	{
	ClientRect.left += (RULER_SIZE-1);
	if (x > ClientRect.left && y > ClientRect.bottom)
		{
		MoveTo(hDC, x, ClientRect.top);
		LineTo(hDC, x, ClientRect.bottom);
		}
	}
else if (dwStyle & RS_VERT)
	{
	ClientRect.top += (RULER_SIZE-1);
	if (y > ClientRect.top && x > ClientRect.right)
		{
		MoveTo(hDC, ClientRect.left, y);
		LineTo(hDC, ClientRect.right, y);
		}
	}
SelectObject(hDC, hOldPen);
SetROP2(hDC, OldROP);
}
