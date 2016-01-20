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

extern HANDLE	hInstAstral;
extern HWND	hWndAstral;
extern STRING 	szAppName;

/***********************************************************************/
int AstralPaint(hWindow)
/***********************************************************************/
HWND hWindow;
{
PAINTSTRUCT ps;
HDC hDC;

/* Don't need to paint much in the application's client window */
hDC = BeginPaint( hWindow, &ps );
ValidateRect( hWindow, NULL );
EndPaint( hWindow, &ps );
return( TRUE );
}


#ifdef UNUSED
/***********************************************************************/
int AstralRetouchPaint(hWindow)
/***********************************************************************/
HWND hWindow;
{
PAINTSTRUCT ps;
HDC hDC;
RECT Rect, BRect;
int delta, FullBrush;
LPTR pBrush;
POINT where, point;

hDC = BeginPaint( hWindow, &ps );
SetMapMode( hDC, MM_TEXT );
AstralControlRect( hWindow, IDC_BRUSHICON, &Rect );
BRect.left = BRect.right = (Rect.left + Rect.right) / 2;
BRect.top = BRect.bottom = (Rect.top + Rect.bottom) / 2;
FullBrush = Retouch.BrushSize;
if (FullBrush < 1) FullBrush = 1;
if (FullBrush > MAX_BRUSH_SIZE)
	FullBrush = MAX_BRUSH_SIZE;
if ( !FullBrush & 1 )
	FullBrush++;
delta = FullBrush/2;
InflateRect( &BRect, delta, delta );
delta = 1;

SetupMask();
pBrush = Retouch.BrushMask;
for (where.y = BRect.top; where.y <= BRect.bottom; where.y += delta)
	{
	for (where.x = BRect.left; where.x <= BRect.right;)
		{
		if (*pBrush)	 /* start of run */
			{
			MoveTo( hDC, where.x, where.y );
			point = where;
			do { ++pBrush; where.x += delta; }
				while (*pBrush && where.x <= BRect.right);
			LineTo( hDC, where.x, where.y );
			if ( delta == 2 )
				{
				MoveTo( hDC, point.x, point.y+1 );
				LineTo( hDC, where.x, where.y+1 );
				}
			}
		else	{
			++pBrush;
			where.x += delta;
			}
		}
	}

ValidateRect( hWindow, &Rect );
EndPaint( hWindow, &ps );
}
#endif


/***********************************************************************/
void PaintIcon( hWindow, idIcon )
/***********************************************************************/
HWND hWindow;
int idIcon;
{
PAINTSTRUCT ps;
HDC hDC;
HICON hIcon;

hDC = BeginPaint( hWindow, &ps );
hIcon = LoadIcon( hInstAstral, MAKEINTRESOURCE(idIcon) );
DrawIcon( hDC, 0, 0, hIcon );
EndPaint( hWindow, &ps );
}
