//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "ctlutil.h"
#include "ctltools.h"

CDrawTools DrawTools;

CDrawTools::CDrawTools()
{
	m_hBtnFaceBrush = NULL;
	m_hBtnShadowBrush = NULL;
	m_hBtnHilightBrush = NULL;
	m_hBtnGrayBrush = NULL;
	m_hBtnFacePen = NULL;
	m_hBtnShadowPen = NULL;
	m_hBtnHilightPen = NULL;
	m_hFramePen = NULL;
	OnSysColorChange();	
}

CDrawTools::~CDrawTools()
{
	FreeUpTools();
}

void CDrawTools::FreeUpTools()
{
	DeleteObject(m_hBtnFaceBrush);
	DeleteObject(m_hBtnShadowBrush);
	DeleteObject(m_hBtnHilightBrush);
	DeleteObject(m_hBtnGrayBrush);
	DeleteObject(m_hBtnFacePen);
	DeleteObject(m_hBtnShadowPen);
	DeleteObject(m_hBtnHilightPen);
	DeleteObject(m_hFramePen);
}

void CDrawTools::OnSysColorChange()
{
	WORD Pattern[16];
	HBITMAP hBitmap;
	int i;

	FreeUpTools();
	m_hBtnFaceBrush = CreateSolidBrush( GetSysColor(COLOR_BTNFACE) );
	m_hBtnShadowBrush = CreateSolidBrush( GetSysColor(COLOR_BTNSHADOW) );
	m_hBtnHilightBrush = CreateSolidBrush( GetSysColor(COLOR_BTNHIGHLIGHT) );
	for ( i=0; i<16; i+=2 )
		Pattern[i] = 0xAAAA;
	for ( i=1; i<16; i+=2 )
		Pattern[i] = 0x5555;
	hBitmap = CreateBitmap( 16, 16, 1, 1, (LPTR)Pattern );
	m_hBtnGrayBrush = CreatePatternBrush( hBitmap );
	DeleteObject( hBitmap );
	m_hBtnFacePen = CreatePen( PS_SOLID, 1, GetSysColor(COLOR_BTNFACE) );
	m_hBtnShadowPen = CreatePen( PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW) );
	m_hBtnHilightPen = CreatePen( PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
	m_hFramePen = CreatePen( PS_SOLID, 1, GetSysColor(COLOR_WINDOWFRAME));
}

/***********************************************************************/
BOOL StyleOn( HWND hWnd, DWORD dwFlag )
/***********************************************************************/
{
if ( !hWnd )
	return( NO );
dwFlag &= GetWindowLong( hWnd, GWL_STYLE );
return( dwFlag != 0 );
}

/***********************************************************************/
BOOL IsWordBitSet( HWND hWnd, int nOffset, WORD wBit )
/***********************************************************************/
{
WORD	wBitsSet;

if ( !hWnd )
	return( NO );
wBitsSet = wBit & GetWindowWord( hWnd, nOffset );
return( wBit == wBitsSet );
}

/***********************************************************************/
void SetWordBit( HWND hWnd, int nOffset, WORD wBit, BOOL on )
/***********************************************************************/
{
if ( !hWnd )
	return;
if (on)
	wBit |= GetWindowWord( hWnd, nOffset );
else
	wBit = GetWindowWord( hWnd, nOffset ) & ~wBit; // beware the squiggle in xywrite
SetWindowWord(hWnd, nOffset, wBit);
}


/***********************************************************************/
void SetLongBit( HWND hWnd, int nOffset, LONG lBit, BOOL on )
/***********************************************************************/
{
if ( !hWnd )
	return;
if (on)
	lBit |= GetWindowLong( hWnd, nOffset );
else
	lBit = GetWindowLong( hWnd, nOffset ) & ~lBit; // beware the squiggle in xywrite
SetWindowLong(hWnd, nOffset, lBit);
}

/************************************************************************/
void HilightRect( HDC hDC, LPRECT lpRect, HBRUSH hBrush )
/************************************************************************/
{
COLOR OldBkColor, OldTextColor;

OldBkColor = SetBkColor( hDC, GetSysColor(COLOR_BTNFACE) );
OldTextColor = SetTextColor( hDC, GetSysColor(COLOR_BTNSHADOW) );
FillRect( hDC, lpRect, hBrush );
SetBkColor( hDC, OldBkColor );
SetTextColor( hDC, OldTextColor );
}

/************************************************************************/
void HilightFrame( HDC hDC, LPRECT lpRect, HBRUSH hBrush, int iWidth,
                     BOOL bSetColors )
/************************************************************************/
{
COLOR OldBkColor, OldTextColor;
int	i;
RECT	rFrame;

if (bSetColors)
	{
	OldBkColor = SetBkColor( hDC, GetSysColor(COLOR_BTNFACE) );
	OldTextColor = SetTextColor( hDC, GetSysColor(COLOR_BTNSHADOW) );
	}
rFrame = *lpRect;
for (i = 0; i < iWidth; ++i)
	{
	FrameRect( hDC, &rFrame, hBrush );
	InflateRect(&rFrame, -1, -1);
	}
if (bSetColors)
	{
	SetBkColor( hDC, OldBkColor );
	SetTextColor( hDC, OldTextColor );
	}
}

/***********************************************************************/
void GrayWindow( HDC hDC, HWND hWindow, DWORD dwStyle )
/***********************************************************************/
{
RECT ClientRect;

if ( !StyleOn( hWindow, dwStyle ) )
	return;
GetClientRect( hWindow, &ClientRect );
InflateRect( &ClientRect, -3, -3 );
MgxGrayArea( hDC, ClientRect.left, ClientRect.top,
	ClientRect.right-ClientRect.left, ClientRect.bottom-ClientRect.top );
}


/***********************************************************************/
void GrayWholeWindow( HDC hDC, HWND hWindow, DWORD dwStyle )
/***********************************************************************/
{
RECT ClientRect;

if ( !StyleOn( hWindow, dwStyle ) )
	return;
GetClientRect( hWindow, &ClientRect );
MgxGrayArea( hDC, ClientRect.left, ClientRect.top,
	ClientRect.right-ClientRect.left, ClientRect.bottom-ClientRect.top );
}


/************************************************************************/
void MgxGrayArea( HDC hDC, int x, int y, int iWidth, int iHeight)
/************************************************************************/
{
HBRUSH hOldBrush;
COLOR OldBkColor, OldTextColor;

hOldBrush = (HBRUSH)SelectObject(hDC, DrawTools.GetBtnGrayBrush());
OldBkColor = SetBkColor( hDC, RGB(255,255,255) );
//OldBkColor = SetBkColor( hDC, GetSysColor(COLOR_BTNFACE) );
OldTextColor = SetTextColor( hDC, GetSysColor(COLOR_WINDOWFRAME) );
PatBlt(hDC, x, y, iWidth, iHeight, 0x00FA0089); // DPo
SetBkColor( hDC, OldBkColor );
SetTextColor( hDC, OldTextColor );
SelectObject(hDC, hOldBrush);
}

/***********************************************************************/
int ColorText( HDC hDC, int x, int y, LPTSTR szStr, int length, COLOR Color )
/***********************************************************************/
{
int OldMode, ret;
COLOR OldColor;

OldColor = SetTextColor( hDC, Color );
OldMode = SetBkMode( hDC, TRANSPARENT );
ret = TextOut( hDC, x, y, szStr, length );
SetTextColor( hDC, OldColor );
SetBkMode( hDC, OldMode );
return( ret );
}

/***********************************************************************/
void DrawBoxFrame( HDC hDC, LPRECT lpRect )
/***********************************************************************/
{
HGDIOBJ  hOldPen;
RECT     Rect;
POINT    p;

Rect = *lpRect;
Rect.right--;
Rect.bottom--;

// Draw the box frame
hOldPen = SelectObject( hDC, GetStockObject(BLACK_PEN) );
MoveToEx( hDC, Rect.left, Rect.top , &p);
LineTo( hDC, Rect.right, Rect.top );
LineTo( hDC, Rect.right, Rect.bottom );
LineTo( hDC, Rect.left, Rect.bottom );
LineTo( hDC, Rect.left, Rect.top );
SelectObject(hDC, hOldPen);
}

/***********************************************************************/
void DrawSculptedBox( HDC hDC, LPRECT lpRect, BOOL bFocus, BOOL bDown,
                        BOOL bDouble )
/***********************************************************************/
{
	HGDIOBJ  hOldPen;
	RECT     Rect;
	POINT    p;

	Rect = *lpRect;
	Rect.right--;
	Rect.bottom--;

	hOldPen = SelectObject( hDC, DrawTools.GetFramePen() );

	if (Ctl_IsNewLook())
	{
		// draw lower right
		if (bDown)
			SelectObject(hDC, DrawTools.GetBtnHilightPen());
		else
			SelectObject(hDC, DrawTools.GetFramePen());

		MoveToEx( hDC, Rect.right, Rect.top, &p);
		LineTo( hDC, Rect.right, Rect.bottom );
		LineTo( hDC, Rect.left-1, Rect.bottom );

		// draw upper left
		if (bDown)
			SelectObject(hDC, DrawTools.GetFramePen());
		else
			SelectObject(hDC, DrawTools.GetBtnHilightPen());

		MoveToEx( hDC, Rect.left, Rect.bottom-1, &p);
		LineTo( hDC, Rect.left, Rect.top );
		LineTo( hDC, Rect.right, Rect.top );

		InflateRect( &Rect, -1, -1 );

		// draw lower right
		if (bDown)
			SelectObject(hDC, DrawTools.GetBtnFacePen());
		else
			SelectObject(hDC, DrawTools.GetBtnShadowPen());

		MoveToEx( hDC, Rect.right, Rect.top, &p);
		LineTo( hDC, Rect.right, Rect.bottom );
		LineTo( hDC, Rect.left-1, Rect.bottom );

		// draw upper left
		if (bDown)
			SelectObject(hDC, DrawTools.GetBtnShadowPen());
		else
			SelectObject(hDC, DrawTools.GetBtnFacePen());

		MoveToEx( hDC, Rect.left, Rect.bottom-1, &p);
		LineTo( hDC, Rect.left, Rect.top );
		LineTo( hDC, Rect.right, Rect.top );
	}
	else
	{
		// Draw the box frame
		MoveToEx( hDC, Rect.left+1, Rect.top, &p);
 		LineTo( hDC, Rect.right, Rect.top );
		MoveToEx( hDC, Rect.right, Rect.top+1, &p);
		LineTo( hDC, Rect.right, Rect.bottom );
		MoveToEx( hDC, Rect.right-1, Rect.bottom, &p);
		LineTo( hDC, Rect.left, Rect.bottom );
		MoveToEx( hDC, Rect.left, Rect.bottom-1, &p);
		LineTo( hDC, Rect.left, Rect.top );
		InflateRect( &Rect, -1, -1 );
		if ( bFocus)
			{ // Draw the focus rectangle
			MoveToEx( hDC, Rect.left, Rect.top, &p);
			LineTo( hDC, Rect.right, Rect.top );
			LineTo( hDC, Rect.right, Rect.bottom );
			LineTo( hDC, Rect.left, Rect.bottom );
			LineTo( hDC, Rect.left, Rect.top );
			InflateRect( &Rect, -1, -1 );
			}

		// Draw the upper left border
		if ( bDouble )
			{
			SelectObject( hDC, ( bDown ? DrawTools.GetFramePen() : DrawTools.GetBtnHilightPen() ) );
			MoveToEx( hDC, Rect.left, Rect.bottom, &p);
			LineTo( hDC, Rect.left, Rect.top );
			LineTo( hDC, Rect.right, Rect.top );
			if ( bDown )
				{
				LineTo( hDC, Rect.right, Rect.top+1 );
				Rect.left++;
				Rect.top++;
				}
			else	{ // Draw the drop shadow
				SelectObject( hDC, DrawTools.GetBtnShadowPen() );
				LineTo( hDC, Rect.right, Rect.bottom );
				LineTo( hDC, Rect.left, Rect.bottom );
				InflateRect( &Rect, -1, -1 );
				}
			}

		SelectObject( hDC, ( bDown ? DrawTools.GetBtnShadowPen() : DrawTools.GetBtnHilightPen() ) );
		MoveToEx( hDC, Rect.left, Rect.bottom, &p);
		LineTo( hDC, Rect.left, Rect.top );
		LineTo( hDC, Rect.right, Rect.top );
		if ( bDown )
			{
			LineTo( hDC, Rect.right, Rect.top+1 );
			Rect.left++;
			Rect.top++;
			}
		else	{ // Draw the drop shadow
			SelectObject( hDC, DrawTools.GetBtnShadowPen() );
			LineTo( hDC, Rect.right, Rect.bottom );
			LineTo( hDC, Rect.left, Rect.bottom );
			InflateRect( &Rect, -1, -1 );
			}

	}
	SelectObject( hDC, hOldPen );
}

/***********************************************************************/
void DrawBitmap( HDC hDC, int x, int y, HBITMAP hBitmap )
/***********************************************************************/
{
int      dx, dy;
BITMAP   Data;
HDC      hSrcDC;
short    OldMode;
DWORD    OldTextColor;
HGDIOBJ  hOldBitmap;

#define RGB_BLACK  (RGB (0x00,0x00,0x00))
#define RGB_WHITE  (RGB (0xFF,0xFF,0xFF))
#define RGB_RED    (RGB (0xFF,0x00,0x00))
#define ROP_PSDPxax 0x00B8074AL

if ( !hDC )
	return;
if ( !(hSrcDC = CreateCompatibleDC(hDC)) )
	return;

OldMode = SetBkMode( hDC, TRANSPARENT ); // single bit'rs don't copy white
OldTextColor = SetTextColor( hDC, RGB_RED ); // single bit'rs become RED
hOldBitmap = SelectObject( hSrcDC, hBitmap );

GetObject( hBitmap, sizeof(BITMAP), (LPTSTR)&Data );
dx = Data.bmWidth;
dy = Data.bmHeight;
x -= dx/2;
y -= dy/2;

BitBlt( hDC, x, y, dx, dy, hSrcDC, 0, 0, SRCCOPY/*SRCAND*/ );
SelectObject( hSrcDC, hOldBitmap );
SetTextColor( hDC, OldTextColor );
SetBkMode( hDC, OldMode );
DeleteDC( hSrcDC );
}




