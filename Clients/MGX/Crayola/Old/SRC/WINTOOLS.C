//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HWND hWndAstral;
extern HWND hClientAstral;
extern HINSTANCE hInstAstral;

typedef void (*LPDEFAULTHWNDPROC)(HWND);

/***********************************************************************/
int AstralStr(
/***********************************************************************/
int			idString,
LPSTR FAR *	lppString)
{
static LPSTR lpBuffer;
#define STRING_SIZE 256

if ( !lpBuffer )
    if ( !(lpBuffer = (LPSTR)Alloc((long)STRING_SIZE)) )
	{ // Should never happen
	*lppString = (LPSTR)&lpBuffer;
	return( 0 );
	}

/* Pass back a pointer to our allocated string */
*lppString = lpBuffer;
*lpBuffer = '\0';

/* Get the string from the string table that matches the passed ID */
/* Use this routine when you only need a string for short period of time */
if ( !LoadString( hInstAstral, idString, lpBuffer, STRING_SIZE-1 ) )
	return( 0 );

/* Pass back the string length */
return( lstrlen(lpBuffer)+1 );
}


/***********************************************************************/
BOOL StringsEqualID( LPSTR lpString1, ITEMID idString2 )
/***********************************************************************/
{
LPSTR lpString2;

if ( !AstralStr( idString2, &lpString2 ) )
	return( NO );
return( StringsEqual( lpString1, lpString2 ) );
}


/***********************************************************************/
DWORD GetWinXY(HWND hWnd)
/***********************************************************************/
{
RECT WindowRect;
POINT UpperLeft;
HWND hParent;

GetWindowRect(hWnd, &WindowRect);
UpperLeft.x = WindowRect.left;
UpperLeft.y = WindowRect.top;
if ( hParent = GetParent(hWnd) )
	ScreenToClient(hParent, &UpperLeft);
return(MAKELONG(UpperLeft.x, UpperLeft.y));
}

/***********************************************************************/
int ColorText( HDC hDC, int x, int y, LPSTR szStr, int length, COLOR Color )
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
void CenterPopup( HWND hWnd )
/***********************************************************************/
{
POINT   ptCent;
RECT	rectWnd;
int	cx, cy;

GetWindowRect(hWnd, &rectWnd);
cx = RectWidth( &rectWnd ) - 1;
cy = RectHeight( &rectWnd ) - 1;
GetScreenCenter(GetParent(hWnd), (LPPOINT)&ptCent, cx, cy);
MoveWindow(hWnd, ptCent.x - cx / 2, ptCent.y - cy / 2, cx, cy,
	IsWindowVisible(hWnd) );
}


/***********************************************************************/
void GetScreenCenter(
/***********************************************************************/
HWND	hWnd,   /* handle to window on the screen */
LPPOINT ppt,	/* ptr to point structure where center coords returned */
int		dx,		/* width of window */
int		dy)		/* height of window */
{
HDC	hdcWnd;
RECT	rect;
int	dxScr, dyScr;

hdcWnd = GetDC(hWnd);
dxScr = GetDeviceCaps(hdcWnd, HORZRES);
dyScr = GetDeviceCaps(hdcWnd, VERTRES);
ReleaseDC(hWnd, hdcWnd);

if (IsIconic(hWnd))
	{
	ppt->x = dxScr / 2;
	ppt->y = dyScr / 2;
	}
else	{
	GetWindowRect(hWnd, &rect);

	ppt->x = (rect.right + rect.left) / 2;
	ppt->y = (rect.top + rect.bottom) / 2;

	if (ppt->x + dx / 2 > dxScr)
		ppt->x = dxScr - dx / 2;
	else	if (ppt->x - dx / 2 < 0)
			ppt->x = dx / 2;

	if (ppt->y + dy / 2 > dyScr)
		ppt->y = dyScr - dy / 2;
	else	if (ppt->y - dy / 2 < 0)
			ppt->y = dy / 2;
	}
}


/***********************************************************************/
void OffsetWindow(
/***********************************************************************/
HWND 	hWnd,
int 	dx,
int		dy,
BOOL 	bRedraw)
{
RECT rect;
HWND hParent;

GetWindowRect( hWnd, &rect );
if ( hParent = GetParent(hWnd) )
	{
	ScreenToClient( hParent, (LPPOINT)&rect.left );
	ScreenToClient( hParent, (LPPOINT)&rect.right );
	}
MoveWindow( hWnd, rect.left+dx, rect.top+dy,
	RectWidth(&rect)-1, RectHeight(&rect)-1, bRedraw );
}


/***********************************************************************/
void RightPopup( HWND hWnd )
/***********************************************************************/
{
POINT   pt;
RECT	rect;
int	dx, dy;
HWND hParent;

GetWindowRect( hWnd, &rect );
dx = RectWidth( &rect ) - 1;
dy = RectHeight( &rect ) - 1;
hParent = GetParent(hWnd);
if (hParent == hWndAstral)
	{
	GetClientRect( hClientAstral, &rect );
	ClientToScreen(hClientAstral, (LPPOINT)&rect.left);
	ClientToScreen(hClientAstral, (LPPOINT)&rect.right);
	ScreenToClient(hParent, (LPPOINT)&rect.left);
	ScreenToClient(hParent, (LPPOINT)&rect.right);
	}
else
	GetClientRect( hParent, &rect );
pt.x = rect.right - dx - 1;					// Right
pt.y = (rect.top + rect.bottom - dy) / 2;	// Centered y
pt.x -= 5; // Fix the RX jumping
pt.y += 5;
MoveWindow( hWnd, pt.x, pt.y, dx, dy, IsWindowVisible(hWnd));
}


/***********************************************************************/
void LeftPopup( HWND hWnd )
/***********************************************************************/
{
POINT   pt;
RECT	rect;
int	dx, dy;
HWND hParent;

GetWindowRect( hWnd, &rect );
dx = RectWidth( &rect ) - 1;
dy = RectHeight( &rect ) - 1;
hParent = GetParent(hWnd);
if (hParent == hWndAstral)
	{
	GetClientRect( hClientAstral, &rect );
	ClientToScreen(hClientAstral, (LPPOINT)&rect.left);
	ClientToScreen(hClientAstral, (LPPOINT)&rect.right);
	ScreenToClient(hParent, (LPPOINT)&rect.left);
	ScreenToClient(hParent, (LPPOINT)&rect.right);
	}
else
	GetClientRect( hParent, &rect );
pt.x = rect.left;							// Left
pt.y = (rect.top + rect.bottom - dy) / 2;	// Centered y
MoveWindow( hWnd, pt.x, pt.y, dx, dy, IsWindowVisible(hWnd));
}


/***********************************************************************/
void SavePopup( HWND hWnd, LPSTR lpKeyName )
/***********************************************************************/
{
HWND hParent;
RECT rect, rParent;
int d1, d2;
STRING szString;
POINT pt;
BOOL bChild;

if ( !hWnd || !(hParent = GetParent(hWnd)) )
	return;

if ( bChild = ( GetWindowLong( hWnd, GWL_STYLE ) & WS_POPUP ) == 0 )
	{
	GetClientRect( hParent, &rParent );
	ClientToScreen( hParent, (LPPOINT)&rParent.left );
	ClientToScreen( hParent, (LPPOINT)&rParent.right );
	}
else
	GetWindowRect( hParent, &rParent );

GetWindowRect( hWnd, &rect );

d1 = rect.left - rParent.right;
d2 = rect.right - rParent.left;
if ( abs( d1 ) > abs( d2 ) ) // if its closer to the left...
		pt.x = d2;
else	pt.x = d1;

d1 = rect.top - rParent.bottom;
d2 = rect.bottom - rParent.top;
if ( abs( d1 ) > abs( d2 ) ) // if its closer to the top...
		pt.y = d2;
else	pt.y = d1;

PutDefaultString( lpKeyName, Int4ToAscii( szString, pt.x, pt.y, 0, 0 ) );
}

/***********************************************************************/
BOOL RestorePopup( HWND hWnd, LPSTR lpKeyName, LPDEFAULTHWNDPROC lpDefaultRoutine )
/***********************************************************************/
{
RECT rect, rParent;
int dx, dy, z, dummy;
STRING szString;
POINT pt;
HWND hParent;
BOOL bChild;
int delta;

GetDefaultString( lpKeyName, "0,0,0,0", szString, sizeof(STRING) );
AsciiToInt4( szString, (LPINT)&pt.x, (LPINT)&pt.y, (LPINT)&z, (LPINT)&dummy );
hParent = GetParent(hWnd);
if ( IsIconic(hParent) || !(pt.x + pt.y + dummy) )
	{
	if ( lpDefaultRoutine )
		(*lpDefaultRoutine)( hWnd );
	return(FALSE);
	}

if ( bChild = ( GetWindowLong( hWnd, GWL_STYLE ) & WS_POPUP ) == 0 )
	{
	GetClientRect( hParent, &rParent );
	ClientToScreen( hParent, (LPPOINT)&rParent.left );
	ClientToScreen( hParent, (LPPOINT)&rParent.right );
	}
else
	GetWindowRect( hParent, &rParent );

GetWindowRect( hWnd, &rect );

dx = RectWidth(&rect) - 1;
dy = RectHeight(&rect) - 1;

if ( pt.x < 0 )
		pt.x += rParent.right;
else	pt.x += (rParent.left - dx);

if ( pt.y < 0 )
		pt.y += rParent.bottom;
else	pt.y += (rParent.top - dy);

SetRect( &rect, pt.x, pt.y, pt.x + dx, pt.y + dy );

if ( bChild )
	{
	// Keep it completely in the window
	if ( (delta = rParent.right - rect.right) < 0 )
		{
		rect.left += delta;
		rect.right += delta;
		}
	if ( (delta = rParent.left - rect.left) > 0 )
		{
		rect.left += delta;
		rect.right += delta;
		}
//	if ( (delta = rParent.bottom - rect.bottom) < 0 )
//		{
//		rect.top += delta;
//		rect.bottom += delta;
//		}
	if ( (delta = rParent.top - rect.top) > 0 )
		{
		rect.top += delta;
		rect.bottom += delta;
		}
	ScreenToClient( hParent, (LPPOINT)&rect.left );
	ScreenToClient( hParent, (LPPOINT)&rect.right );
	}

MoveWindow( hWnd, rect.left, rect.top,
	RectWidth(&rect)-1, RectHeight(&rect)-1, IsWindowVisible(hWnd) );
return(TRUE);
}

/***********************************************************************/
// restores size and position
/***********************************************************************/
BOOL RestoreFullPopup( HWND hWnd, LPSTR lpKeyPosName, LPSTR lpKeySizeName, LPDEFAULTHWNDPROC lpDefaultRoutine )
/***********************************************************************/
{
RECT rect, rParent;
int dx, dy, z, dummy;
STRING szString;
POINT pt;
HWND hParent;

GetDefaultString( lpKeyPosName, "0,0,0,0", szString, sizeof(STRING) );
AsciiToInt4( szString, (LPINT)&pt.x, (LPINT)&pt.y, (LPINT)&z, (LPINT)&dummy );
hParent = GetParent(hWnd);
if ( IsIconic(hParent) || !(pt.x + pt.y + dummy) )
	{
	if ( lpDefaultRoutine )
		(*lpDefaultRoutine)( hWnd );
	return(FALSE);
	}

GetDefaultString( lpKeySizeName, "0,0", szString, sizeof(STRING) );
AsciiToInt2( szString, &dx, &dy);
GetClientRect( GetParent(hWnd), &rParent );
GetWindowRect( hWnd, &rect );
if ( !(dx +dy))
	{
	dx = RectWidth(&rect);
	dy = RectHeight(&rect);
	}
if ( pt.x < 0 )
		pt.x += rParent.right;
else	pt.x -= dx;
if ( pt.y < 0 )
		pt.y += rParent.bottom;
else	pt.y -= dy;
MoveWindow( hWnd, pt.x, pt.y, dx-1, dy-1, IsWindowVisible(hWnd) );
return(TRUE);
}

/***********************************************************************/
BOOL StyleOn(
/***********************************************************************/
HWND 	hWnd,
DWORD 	dwFlag)
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


static WORD origstyle;
/***********************************************************************/
void DisableSavebits( HWND hWnd )
/***********************************************************************/
{
WORD style;

origstyle = style = GET_CLASS_STYLE( hWnd );
style &= (~CS_SAVEBITS); /* beware the squiggle in XyWrite */
SET_CLASS_STYLE( hWnd, style );
}


/***********************************************************************/
void EnableSavebits( HWND hWnd )
/***********************************************************************/
{
SET_CLASS_STYLE( hWnd, origstyle );
}


/***********************************************************************/
void SetClientRect(
/***********************************************************************/
HWND 	hWnd,
LPRECT 	lpRect)
{
RECT Crect, Wrect;

GetWindowRect( hWnd, &Wrect );
GetClientRect( hWnd, &Crect );
Wrect.right -= RectWidth(&Crect);
Wrect.right += RectWidth(lpRect);
Wrect.bottom -= RectHeight(&Crect);
Wrect.bottom += RectHeight(lpRect);
SizeWindow( hWnd, RectWidth(&Wrect), RectHeight(&Wrect) );
}


/***********************************************************************/
void GetDrawClientRect(
/***********************************************************************/
HWND 	hWnd,
LPRECT 	lpRect)
{ // Calculates the client rectangle taking into account the actual paint area
GetClientRect( hWnd, lpRect );
lpRect->bottom--;
lpRect->right--;
}


/***********************************************************************/
void GetRealClientRect(
/***********************************************************************/
HWND 	hWnd,
LPRECT 	lpRect)
{ // Calculates the client rectangle taking scrollbars into account
DWORD dwStyle;
int cx, cy;

GetClientRect( hWnd, lpRect );
if (!IsIconic(hWnd))
	{
	dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	GetScrollBarSize(&cx, &cy);
	if (dwStyle & WS_HSCROLL)
		lpRect->bottom += cy;
	if (dwStyle & WS_VSCROLL)
		lpRect->right += cx;
	}
}


/***********************************************************************/
void GetScrollBarSize(
/***********************************************************************/
LPINT 	lpCx,
LPINT	lpCy)
{
*lpCx = GetSystemMetrics(SM_CXVSCROLL) - 1;
*lpCy = GetSystemMetrics(SM_CYHSCROLL) - 1;
}


/***********************************************************************/
void SetFullClientRect(
/***********************************************************************/
HWND hWnd,
BOOL fFull)
{
int xExcess, yExcess;
DWORD style;
HMENU hMenu;
static RECT Crect, Wrect;
static DWORD origstyle;

if ( !fFull )
	{
	if ( !origstyle )
		return;
	SetWindowLong( hWnd, GWL_STYLE, origstyle );
	origstyle = NULL;
	MoveWindow( hWnd,
		Wrect.left,
		Wrect.top,
		RectWidth( &Crect ),
		RectHeight( &Crect ),
		IsWindowVisible(hWnd) );
	}
else	{
	if ( origstyle )
		return;
	origstyle = style = GetWindowLong( hWnd, GWL_STYLE );
	style &= (~WS_OVERLAPPEDWINDOW); // beware the squiggle in XyWrite
	SetWindowLong( hWnd, GWL_STYLE, style );
	if ( hMenu = GetMenu( hWndAstral ) )
//	hMenu = GetMenu( hWndAstral );
		// after getting the style but b4 setting the style
		SetMenu( hWndAstral, NULL );
	GetWindowRect( hWnd, &Wrect );
	GetClientRect( hWnd, &Crect );
	xExcess = RectWidth(&Wrect)  - RectWidth(&Crect);
	yExcess = RectHeight(&Wrect) - RectHeight(&Crect);
	ClientToScreen( hWnd, (LPPOINT)&Crect.left );
	MoveWindow( hWnd,
		Wrect.left - Crect.left,
		Wrect.top  - Crect.top,
		GetSystemMetrics(SM_CXSCREEN) + xExcess,
		GetSystemMetrics(SM_CYSCREEN) + yExcess,
		IsWindowVisible(hWnd) );
	if ( hMenu )
		SetMenu( hWndAstral, hMenu ); // retore the menu
	}
}


/***********************************************************************/
VOID AstralMenuCheck(
/***********************************************************************/
int		menuID,
int		*oldmenuID)
{
CheckMenuItem( GetMenu( hWndAstral ), *oldmenuID, MF_UNCHECKED );
CheckMenuItem( GetMenu( hWndAstral ), menuID, MF_CHECKED );
*oldmenuID = menuID;
}


/***********************************************************************/

VOID AstralSetDlgItemFixed(
HWND   hDlg,
int    idDlg,
LFIXED Fixed,
BOOL   bNoConvert)
{
	char szFixed[32];

	if ( bNoConvert )
		FixedAscii( Fixed, szFixed, -1 );
	else
		Unit2String( Fixed, szFixed );

	SetDlgItemText( hDlg, idDlg, szFixed );
}

/***********************************************************************/

int SetUnitResolution( int res )
{
	int OldUnitRes;

	OldUnitRes = Control.UnitRes;

	if ( !(Control.UnitRes = res) )
	{
		Control.UnitRes =
			( lpImage ? FrameResolution(ImgGetBaseEditFrame(lpImage)) : 1000 );
	}

	return( OldUnitRes );
}

/***********************************************************************/

static int cDecimal = 0;

static void DoubleToString(double d, LPSTR lpString, int iPrecision)
{
	double dScale, dPrecision;
	LPTR lpData = (LPTR)lpString;

	if (iPrecision > 0)
	{
		dPrecision = 1.0 / pow(10.0, (double)iPrecision);
	}
	else
	{
		dPrecision = 1.0;
	}

	d += (dPrecision/2.0);

	dScale = 1.0;

	while(dScale < (d/10.0))
		dScale *= 10.0;

	while(dScale >= 1.0)
	{
		if (dScale <= d)
		{
			*lpData++ = (int)'0' + (int)(d / dScale);

			while(d >= dScale)
				d -= dScale;
		}
		else
		{
			*lpData++ = '0';
		}
		dScale /= 10.0;
	}

	if (iPrecision > 0)
	{
		if (!cDecimal)
		{
			char szDecimal[2];

			GetProfileString( "intl", "sDecimal", ".", szDecimal, 2);

			cDecimal = szDecimal[0];
		}

		*lpData++ = cDecimal;

		dScale = dPrecision;
	
		while(dScale < (d/10.0))
			dScale *= 10.0;

		while(dScale >= dPrecision)
		{
			if (dScale <= d)
			{
				*lpData++ = (int)'0' + (int)(d / dScale);

				while(d >= dScale)
					d -= dScale;
			}
			else
			{
				*lpData++ = '0';
			}
			dScale /= 10.0;
		}
	}

	*lpData++ = 0;
}

/***********************************************************************/

LPSTR Unit2String( LFIXED Fixed, LPSTR lpFixed )
{
	long points, picas;
	double d;

	switch ( Control.Units )
	{
		case IDC_PREF_UNITMM:
			d = ((double)Fixed * (double) 25.4)/65536.0;
			DoubleToString(d, lpFixed, 2);
		break;

		case IDC_PREF_UNITPICAS:
			// d is now in points
			d = ((double)Fixed * (double)Control.Points)/(double)655360.0;
			points  = (long)(d + 0.5);
			picas   = points/12L;
			points -= (picas*12L);
			wsprintf(lpFixed, "%ld,%ld", picas, points);
		break;

		case IDC_PREF_UNITCM:
			d  = ((double)Fixed * (double)2.54)/65536.0;
			DoubleToString(d, lpFixed, 3);
		break;

		case IDC_PREF_UNITPIXELS:
			d = ((double)Fixed * (double)Control.UnitRes)/65536.0;
			DoubleToString(d, lpFixed, 0);
		break;

		case IDC_PREF_UNITINCHES:
		default:
			FixedAscii( Fixed, lpFixed, 3 );
		break;
	}

	return( lpFixed );
}

/***********************************************************************/

static BOOL isValidFixed(LPSTR lpFixed)
{
	BOOL bValid = TRUE;
	int iCount = lstrlen(lpFixed);

	if (!cDecimal)
	{
		char szDecimal[2];

		GetProfileString( "intl", "sDecimal", ".", szDecimal, 2);

		cDecimal = szDecimal[0];
	}

	while(iCount--)
	{
		if (*lpFixed != cDecimal)
		{
			switch(*lpFixed)
			{
				case '0' :
				case '1' :
				case '2' :
				case '3' :
				case '4' :
				case '5' :
				case '6' :
				case '7' :
				case '8' :
				case '9' :
				case '.' :
				case ',' :
				case '+' :
				case '-' :
				break;

				default :
					bValid = FALSE;
				break;
			}
		}
		lpFixed++;
	}

	return(bValid);
}

/***********************************************************************/

LFIXED AstralGetDlgItemFixed(
HWND 	hDlg,
int	 	idDlg,
BOOL 	FAR *lpTranslated,
BOOL 	bNoConvert)
{
	LFIXED Fixed;
	double d;
	char   szFixed[32];
	LPSTR    s;
	long   picas, points;
	int    units;

	*lpTranslated = TRUE;

	if ( bNoConvert )
		units = IDC_PREF_UNITINCHES;
	else
		units = Control.Units;

	GetDlgItemText( hDlg, idDlg, szFixed, sizeof(szFixed) );

	if (!isValidFixed(szFixed))
	{
		*lpTranslated = FALSE;

		return((LFIXED)0);
	}

	switch ( units )
	{
		case IDC_PREF_UNITMM:
			Fixed = AsciiFixed( szFixed );
			d = ((double)Fixed / (double)65536.0) / (double)25.4;
			Fixed = (LFIXED)(d * 65536.0);
		break;

		case IDC_PREF_UNITCM:
			Fixed = AsciiFixed( szFixed );
			d = ((double)Fixed / (double)65536.0) / (double)2.54;
			Fixed = (LFIXED)(d * 65536.0);
		break;

		case IDC_PREF_UNITPICAS:
			if (lstrchr(szFixed, '.'))
			{
				Fixed = AsciiFixed(szFixed);
				d = ((double)Fixed / (double)65536.0) * (double)12.0;
				if (Control.Points != 0)
					d = 10.0 * (d / (double)Control.Points);
				Fixed = (LFIXED)(d * 65536.0);
			}
			else
			{
				s = lstrchr(szFixed, ',');
				points = 0;
				if (s)
				{
					*s = '\0';
					++s;
					if (lstrlen(s))
						points = Ascii2Long(s);
				}

				if ( (picas = Ascii2Long(szFixed)) != 1)
					picas = 0;
				points += 12 * picas;
				d = ((double)points / (double)Control.Points) * 10.0;
				Fixed = (LFIXED)(d * 65536.0);
			}
		break;

		case IDC_PREF_UNITPIXELS:
			Fixed = AsciiFixed( szFixed );
			if (Control.UnitRes != 0)
				d = ((double)Fixed / (double)65536.0) / (double)Control.UnitRes;
			Fixed = (LFIXED)(d * 65536.0);
		break;

		case IDC_PREF_UNITINCHES:
		default:
			Fixed = AsciiFixed(szFixed);
		break;
	}

	return( Fixed );
}

/************************************************************************/
void DrawGuideLine(
/************************************************************************/
HDC		hDC,
int		x1,
int		y1,
int		x2,
int		y2,
BOOL	on)
{
File2DisplayEx(&x1, &y1, YES);
File2DisplayEx(&x2, &y2, YES);
Dline( hDC, x1, y1, x2, y2, on );
}


/************************************************************************/
void Dline(
/************************************************************************/
HDC 	hDC,
int 	x1,
int		y1,
int		x2,
int		y2,
BOOL 	flag)
{
int OldROP;
BOOL bOldClip = FALSE;

if (!hDC)
{
	hDC = Window.hDC;
	bOldClip = TRUE;
	if (lpImage)
		SelectClipRect(hDC, &lpImage->lpDisplay->DispRect, NULL);
}
OldROP = SetROP2( hDC, R2_NOT );
MoveToEx( hDC, x1, y1, NULL);
LineTo( hDC, x2, y2);
SetROP2( hDC, OldROP );
if (bOldClip)
	SelectClipRgn(hDC, NULL);
}


/************************************************************************/
void DrawLine(
/************************************************************************/
HDC 	hDC,
int 	x1,
int		y1,
int		x2,
int		y2,
HPEN 	hPen)
{
    DrawLineEx( hDC, x1, y1, x2, y2, hPen, TRUE );
}


/************************************************************************/
void DrawLineEx(
/************************************************************************/
HDC 	hDC,
int 	x1,
int		y1,
int		x2,
int		y2,
HPEN 	hPen,
BOOL 	bXor)
{
int OldROP;
HPEN hOldPen;

if (!hDC)
	hDC = Window.hDC;

hOldPen = (HPEN)SelectObject(hDC, hPen);
OldROP = SetROP2( hDC, bXor ? R2_XORPEN : R2_COPYPEN );
MoveToEx( hDC, x1, y1, NULL);
LineTo( hDC, x2, y2);
SetROP2( hDC, OldROP );
SelectObject(hDC, hOldPen);
}


/************************************************************************/
void Drect(
/************************************************************************/
HDC 	hDC,
LPRECT 	lpRect,
BOOL 	flag)
{
int OldROP;

if (!hDC)
	hDC = Window.hDC;

OldROP = SetROP2( hDC, R2_NOT );
MoveToEx( hDC, lpRect->left, lpRect->top, NULL );
LineTo( hDC, lpRect->right, lpRect->top );
LineTo( hDC, lpRect->right, lpRect->bottom );
LineTo( hDC, lpRect->left, lpRect->bottom );
LineTo( hDC, lpRect->left, lpRect->top );
SetROP2( hDC, OldROP );
}


/************************************************************************/
void DrawRect(
/************************************************************************/
HDC 	hDC,
LPRECT 	lpRect,
HPEN 	hPen)
{
int OldROP;
HPEN hOldPen;

if (!hDC)
	hDC = Window.hDC;

hOldPen = (HPEN)SelectObject(hDC, hPen);
OldROP = SetROP2( hDC, R2_XORPEN );
MoveToEx( hDC, lpRect->left, lpRect->top, NULL );
LineTo( hDC, lpRect->right, lpRect->top );
LineTo( hDC, lpRect->right, lpRect->bottom );
LineTo( hDC, lpRect->left, lpRect->bottom );
LineTo( hDC, lpRect->left, lpRect->top );
SetROP2( hDC, OldROP );
SelectObject(hDC, hOldPen);
}


/************************************************************************/
void Dbox(
/************************************************************************/
HDC 	hDC,
LPRECT 	lpRect,
BOOL 	flag)
{
if (!hDC)
	hDC = Window.hDC;

InvertRect(hDC, lpRect);
}

/************************************************************************/
void Dellipse(
/************************************************************************/
HDC 	hDC,
LPRECT 	lpRect,
BOOL 	flag)
{
int OldROP;
HBRUSH hOldBrush;

if (!hDC)
	hDC = Window.hDC;

OldROP = SetROP2( hDC, R2_NOT );
hOldBrush = (HBRUSH)SelectObject( hDC, GetStockObject(NULL_BRUSH) );
Ellipse(hDC, lpRect->left, lpRect->top, lpRect->right+1, lpRect->bottom+1);
SelectObject(hDC, hOldBrush);
SetROP2( hDC, OldROP );
}


/************************************************************************/
VOID SetKeyState(
/************************************************************************/
int 	nVirtKey,
BOOL	fDown,
BOOL	fToggle)
{
BYTE KeyState[256];

GetKeyboardState( KeyState );
if ( fToggle )
	KeyState[nVirtKey] ^= 0x80;
else
if ( fDown )
	KeyState[nVirtKey] = 0;
else	KeyState[nVirtKey] = 0x81;
SetKeyboardState( KeyState );
GetKeyboardState( KeyState );
}


/************************************************************************/
void ShowControl(
/************************************************************************/
HWND 	hDlg,
int 	idControl1,
int		idControl2,
BOOL 	bShow,
BOOL	bDefer)
{
HWND hControl;
RECT Rect;
BOOL bBreakFlag;
DWORD dwStyle;
int id;
static HDWP hDefer;
#define BIG 10000

if ( !hDlg )
	{
	if ( hDefer )
		EndDeferWindowPos( hDefer );
	hDefer = NULL;
	return;
	}

/* Get the handle to the first control */
if ( !(hControl = GetDlgItem( hDlg, idControl1 )) )
	return;

bBreakFlag = NO;
while ( 1 )
	{
	id = GetDlgCtrlID( hControl );
	if ( bBreakFlag && id != idControl2 )
		break;
	if ( id == idControl2 )
		bBreakFlag = YES;

	/* Get screen coordinates of the control */
	GetWindowRect( hControl, &Rect );

	/* Convert the control's coordinates to be relative to the parent */
	ScreenToClient( hDlg, (LPPOINT)&Rect.left );
	ScreenToClient( hDlg, (LPPOINT)&Rect.right );

	if ( bShow )
		{
		if ( Rect.left < BIG || Rect.top < BIG )
			return;
		Rect.left -= BIG;
		Rect.top -= BIG;
		dwStyle = GetWindowLong( hControl, GWL_STYLE );
		if ( dwStyle & WS_CAPTION )
			{ // Turn the Tabstop back on
			dwStyle &= (~WS_CAPTION);
			dwStyle |= WS_TABSTOP;
			SetWindowLong( hControl, GWL_STYLE, dwStyle );
			}
		}
	else	{
		if ( Rect.left >= BIG && Rect.top >= BIG )
			return;
		Rect.left += BIG;
		Rect.top += BIG;
		dwStyle = GetWindowLong( hControl, GWL_STYLE );
		if ( dwStyle & WS_TABSTOP )
			{ // Turn off the Tabstop
			dwStyle &= (~WS_TABSTOP);
			dwStyle |= WS_CAPTION;
			SetWindowLong( hControl, GWL_STYLE, dwStyle );
			}
		}

	if ( bDefer )
		{
		if ( !hDefer )
			hDefer = BeginDeferWindowPos( 40 );
		hDefer = DeferWindowPos( hDefer, hControl, 0, /* Same z order */
		Rect.left, Rect.top, /* New location */
		0, 0, /* Same size */
		SWP_NOZORDER | SWP_NOSIZE /* Flags */ );
		}
	else	{
		SetWindowPos( hControl, 0, /* Same z order */
		Rect.left, Rect.top, /* New location */
		0, 0, /* Same size */
		SWP_NOZORDER | SWP_NOSIZE /* Flags */ );
		UpdateWindow(hControl);
		}

	// Get the handle to the next window
	if ( !(hControl = GetWindow( hControl, GW_HWNDNEXT ) ) )
		break;
	}
}


/************************************************************************/
void ShowDlgItems(
/************************************************************************/
HWND 	hDlg,
int 	idControl1,
int		idControl2,
BOOL 	bShow,
BOOL	bDefer)
{
HWND hControl;
BOOL bBreakFlag;
DWORD dwStyle;
int id;
WORD wShowFlag;
static HDWP hDefer;

if ( !hDlg )
	{
	if ( hDefer )
		EndDeferWindowPos( hDefer );
	hDefer = NULL;
	return;
	}

wShowFlag = ( bShow ? SWP_SHOWWINDOW : SWP_HIDEWINDOW );
if ( !IsWindowVisible( hDlg ) )
	wShowFlag |= SWP_NOREDRAW;

/* Get the handle to the first control */
if ( !(hControl = GetDlgItem( hDlg, idControl1 )) )
	return;

bBreakFlag = NO;
while ( 1 )
	{
	id = GetDlgCtrlID( hControl );
	if ( bBreakFlag && id != idControl2 )
		break;
	if ( id == idControl2 )
		bBreakFlag = YES;

	if ( bShow == IsWindowVisible( hControl ) )
		{
		// Get the handle to the next window
		if ( !(hControl = GetWindow( hControl, GW_HWNDNEXT ) ) )
			break;
		continue;
		}

	// Show or Hide the control
	dwStyle = GetWindowLong( hControl, GWL_STYLE );
	if ( bShow )
		{
		if ( dwStyle & WS_CAPTION )
			{ // Turn the Tabstop back on
			dwStyle &= (~WS_CAPTION);
			dwStyle |= WS_TABSTOP;
			SetWindowLong( hControl, GWL_STYLE, dwStyle );
			}
		}
	else
		{
		if ( dwStyle & WS_TABSTOP )
			{ // Turn off the Tabstop
			dwStyle &= (~WS_TABSTOP);
			dwStyle |= WS_CAPTION;
			SetWindowLong( hControl, GWL_STYLE, dwStyle );
			}
		}

	if ( bDefer )
		{
		if ( !hDefer )
			hDefer = BeginDeferWindowPos( 40 );
		hDefer = DeferWindowPos( hDefer, hControl, 0, /* Same z order */
			0, 0, /* Same location */
			0, 0, /* Same size */
			wShowFlag | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
			SWP_NOACTIVATE | SWP_NOOWNERZORDER );
		}
	else	{
		SetWindowPos( hControl, 0, /* Same z order */
			0, 0, /* Same location */
			0, 0, /* Same size */
			wShowFlag | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
			SWP_NOACTIVATE | SWP_NOOWNERZORDER );
//		UpdateWindow(hControl);
		}

	// Get the handle to the next window
	if ( !(hControl = GetWindow( hControl, GW_HWNDNEXT ) ) )
		break;
	}
}


/************************************************************************/
void ShowDlgItem(
/************************************************************************/
HWND 	hDlg,
ITEMID 	idControl,
BOOL 	bShow)
{
HWND hControl;
WORD wShowFlag;

if ( !hDlg )
	return;

// Get the handle to the control
if ( !(hControl = GetDlgItem( hDlg, idControl )) )
	return;

if ( bShow == IsWindowVisible( hControl ) )
	return;

wShowFlag = ( bShow ? SWP_SHOWWINDOW : SWP_HIDEWINDOW );
if ( !IsWindowVisible( hDlg ) )
	wShowFlag |= SWP_NOREDRAW;

SetWindowPos( hControl, 0, /* Same z order */
	0, 0, /* Same location */
	0, 0, /* Same size */
	wShowFlag | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
	SWP_NOACTIVATE | SWP_NOOWNERZORDER );
}

/************************************************************************/
void ControlEnableGroup(
/************************************************************************/
HWND 	hDlg,
int 	idControl1,
int		idControl2,
BOOL 	bEnable)
{
HWND hControl;
int id;

if ( !hDlg )
	return;

/* Get the handle to the first control */
if ( !(hControl = GetDlgItem( hDlg, idControl1 )) )
	return;

// Enable the controls BETWEEN idControl1 and idControl2 (EXCLUSIZE!)
while ( 1 )
	{
	// Get the handle to the next window
	if ( !(hControl = GetWindow( hControl, GW_HWNDNEXT ) ) )
		break;
	if ( !(id = GetDlgCtrlID( hControl )) )
		break;
	if ( id == idControl2 )
		break;
	if ( id > 0 )
		ControlEnable( hDlg, id, bEnable );
	}
}


/***********************************************************************/
HBRUSH SetControlColors(
/***********************************************************************/
HDC		hDC,
HWND	hDlg,
HWND	hControl,
WORD	wType)
{
HBRUSH 	hBrush;
DWORD 	dwStyle;
LOGBRUSH brush;
int	nCount;

dwStyle = GetWindowLong(hControl, GWL_STYLE);
if (wType == CTLCOLOR_LISTBOX && (dwStyle & WS_DISABLED))
	SetTextColor( hDC, GetSysColor(COLOR_BTNFACE) );
else	SetTextColor( hDC,GetSysColor (COLOR_WINDOWTEXT) );

switch( wType )
    {
    case CTLCOLOR_EDIT:
    case CTLCOLOR_MSGBOX: // Used in 3.0 edit boxes (not 3.1)
    case CTLCOLOR_LISTBOX:
	hBrush = (HBRUSH)(COLOR_WINDOW + 1);
	break;
    case CTLCOLOR_CUSTOM:
	hBrush = (HBRUSH)GET_CLASS_HBRBACKGROUND(hControl);
	break;
    case CTLCOLOR_BTN:
    case CTLCOLOR_STATIC:
    case CTLCOLOR_DLG:
    case CTLCOLOR_SCROLLBAR:
    default:
 	hBrush = NULL;
	break;
    }

if ( !hBrush ) // Try using the dialog's background
	hBrush = (HBRUSH)GET_CLASS_HBRBACKGROUND(hDlg);
if ( !hBrush ) // if no class background brush, use white
	hBrush = (HBRUSH)(COLOR_WINDOW + 1);

if (hBrush <= (HBRUSH)(20+1)/*COLOR_ENDCOLORS*/)
	{
	--hBrush;
	SetBkColor (hDC, GetSysColor((int)hBrush));
	switch ((int)hBrush)
		{
		case COLOR_BTNFACE:
			hBrush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
			break;
		case COLOR_WINDOWFRAME:
			hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
			break;
		case COLOR_BTNSHADOW:
			hBrush = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
			break;
		case COLOR_MENU:	// edit background
		case COLOR_WINDOW:
		case COLOR_ACTIVEBORDER:
		case COLOR_INACTIVEBORDER:
		case COLOR_SCROLLBAR:
		case COLOR_ACTIVECAPTION:
		case COLOR_INACTIVECAPTION:
		case COLOR_MENUTEXT:
		case COLOR_WINDOWTEXT:
		case COLOR_CAPTIONTEXT:
		case COLOR_APPWORKSPACE:
		case COLOR_HIGHLIGHT:
		case COLOR_HIGHLIGHTTEXT:
		case COLOR_GRAYTEXT:
		case COLOR_BTNTEXT:
			hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
			break;

		case COLOR_BACKGROUND:
		default:
			hBrush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
			break;
		}
	}
else
	{
	nCount = sizeof(LOGBRUSH);
	if (GetObject(hBrush, nCount, (LPTR)&brush) == nCount)
		SetBkColor(hDC, brush.lbColor);
	else
		SetBkColor (hDC, GetSysColor (COLOR_BTNFACE));
	}
return(hBrush);
}


/************************************************************************/
void HilightRect(
/************************************************************************/
HDC 	hDC,
LPRECT 	lpRect,
HBRUSH 	hBrush)
{
COLOR OldBkColor, OldTextColor;

OldBkColor = SetBkColor( hDC, GetSysColor(COLOR_BTNFACE) );
OldTextColor = SetTextColor( hDC, GetSysColor(COLOR_BTNSHADOW) );
FillRect( hDC, lpRect, hBrush );
SetBkColor( hDC, OldBkColor );
SetTextColor( hDC, OldTextColor );
}

/************************************************************************/
void HilightFrame(
/************************************************************************/
HDC 	hDC,
LPRECT 	lpRect,
HBRUSH 	hBrush,
int		iWidth,
BOOL	bSetColors)
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
void GrayWindow(
/***********************************************************************/
HDC 	hDC,
HWND 	hWindow,
DWORD 	dwStyle)
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
void GrayWholeWindow(
/***********************************************************************/
HDC 	hDC,
HWND 	hWindow,
DWORD 	dwStyle)
{
RECT ClientRect;

if ( !StyleOn( hWindow, dwStyle ) )
	return;
GetClientRect( hWindow, &ClientRect );
MgxGrayArea( hDC, ClientRect.left, ClientRect.top,
	ClientRect.right-ClientRect.left, ClientRect.bottom-ClientRect.top );
}


/************************************************************************/
void MgxGrayArea(
/************************************************************************/
HDC		hDC,
int		x,
int		y,
int 	iWidth,
int		iHeight)
{
HBRUSH hOldBrush;
COLOR OldBkColor, OldTextColor;

hOldBrush = (HBRUSH)SelectObject(hDC, Window.hHilightBrush);
OldBkColor = SetBkColor( hDC, RGB(255,255,255) );
//OldBkColor = SetBkColor( hDC, GetSysColor(COLOR_BTNFACE) );
OldTextColor = SetTextColor( hDC, GetSysColor(COLOR_WINDOWFRAME) );
PatBlt(hDC, x, y, iWidth, iHeight, 0x00FA0089); // DPo
SetBkColor( hDC, OldBkColor );
SetTextColor( hDC, OldTextColor );
SelectObject(hDC, hOldBrush);
}


/************************************************************************/
HWND CopyWindow( HWND hWnd )
/************************************************************************/
{
STRING szClassName, szWindowName;
RECT Rect;

if ( !hWnd )
	return( NULL );
GetWindowRect( hWnd, &Rect );
GetClassName( hWnd, szClassName, sizeof(STRING) );
GetWindowText( hWnd, szWindowName, sizeof(STRING) );
return( CreateWindow(
	szClassName,
	szWindowName,
	GetWindowLong( hWnd, GWL_STYLE ),
	Rect.left, Rect.top, RectWidth(&Rect), RectHeight(&Rect),
	(HWND)GET_WINDOW_PARENT( hWnd ),
	(HMENU)GET_WINDOW_ID( hWnd)+1,
	(HINSTANCE)GET_WINDOW_INSTANCE( hWnd ),
	NULL /* lpParam */ ) );
}


/************************************************************************/
BOOL EnableOverlappedWindow(
/************************************************************************/
HWND	hWnd,
BOOL	bEnable)
{
#ifndef IDD_PALETTE
return(FALSE);
#else

HWND 	hPalette, hParent;
static int iNumTimesDisabled = 0;

if (!hWnd)
	return(FALSE);
if ( !(hPalette = AstralDlgGet(IDD_PALETTE)) )
	return(FALSE);
hParent = hWnd;
while ((hParent != hPalette) && (hParent = GetParent(hParent)))
	;
if (bEnable)
	--iNumTimesDisabled;
if (iNumTimesDisabled)
	{
	if (!bEnable)
		++iNumTimesDisabled;
	return(TRUE);
	}
if (!bEnable)
	++iNumTimesDisabled;
if (hParent == hPalette)
	return(EnableWindow(hWndAstral, bEnable));

return(EnableWindow(hPalette, bEnable));
#endif
}


/************************************************************************/
HINSTANCE AstralLoadLibrary(LPSTR lpLibraryName)
/************************************************************************/
{
LPSTR lpName;
FNAME szLibraryDir, szCurrentDir;
HINSTANCE hModule;

lpName = filename(lpLibraryName);
lstrcpy(szLibraryDir, lpLibraryName);
stripfile(szLibraryDir);
GetCurrentDir(szCurrentDir, MAX_FNAME_LEN);
if (lstrlen(szLibraryDir) > 0)
	SetCurrentDir(szLibraryDir);
hModule = LoadLibrary(lpName);
SetCurrentDir(szCurrentDir);
return(hModule);
}


/************************************************************************/
void Delay( long lTicks )
/************************************************************************/
{
DWORD dwBaseTime;

/* Pause for a few milliseconds */
dwBaseTime = GetTickCount() + lTicks;
while ( GetTickCount() < dwBaseTime )
	;
}


/************************************************************************/
BOOL DelayIfMouseDown( long lTicks )
/************************************************************************/
{ // pop out if timeout or if button up
DWORD dwBaseTime;
POINT Point;
long lPoint;
int x, y;

lPoint = GetMessagePos();
x = LOWORD(lPoint);
y = HIWORD(lPoint);

/* Pause for a few milliseconds */
dwBaseTime = GetTickCount() + lTicks;
while ( GetTickCount() < dwBaseTime )
	{
	GetCursorPos(&Point);
	Point.x -= x;
	Point.y -= y;
	if ( !LBUTTON )
		return( NO );
	if ( abs(Point.x) > 2 || abs(Point.y) > 2 )
		return( YES );
	}
return( YES );
}

/************************************************************************/
void SendMessageToChildren(
/************************************************************************/
HWND	hParent,
WORD	msg,
WORD	wParam,
long	lParam)
{
HWND	hChild;

hChild = GetWindow(hParent, GW_CHILD);
while (hChild)
	{
	SendMessage(hChild, msg, wParam, lParam);
	SendMessageToChildren(hChild, msg, wParam, lParam);
	hChild = GetWindow(hChild, GW_HWNDNEXT);
	}
}


/***********************************************************************/
void DrawBevel(
/***********************************************************************/
HDC 	hDC,
LPRECT 	lpRect,
BOOL 	bBevelin,
int 	iWidth)
{
HPEN hOldPen;
RECT Rect;
int i;

Rect = *lpRect;
Rect.right--;
Rect.bottom--;

hOldPen = (HPEN)SelectObject( hDC, GetStockObject(WHITE_PEN) );
for ( i=0; i<iWidth; i++ )
	{
	SelectObject( hDC, ( bBevelin ? Window.hShadowPen :
					GetStockObject(WHITE_PEN) ) );
	MoveToEx( hDC, Rect.left, Rect.bottom, NULL );
	LineTo( hDC, Rect.left, Rect.top );
	LineTo( hDC, Rect.right, Rect.top );
	SelectObject( hDC, ( !bBevelin ? Window.hShadowPen :
					GetStockObject(WHITE_PEN) ) );
	LineTo( hDC, Rect.right, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.bottom );
	InflateRect( &Rect, -1, -1 );
	}

SelectObject( hDC, hOldPen );
}


/***********************************************************************/
void DrawCorners(
/***********************************************************************/
HDC 	hDC,
LPRECT 	lpRect,
BOOL 	bBevelin)
{
HPEN hPen1, hPen2, hOldPen;
RECT Rect;

Rect = *lpRect;
Rect.right--;
Rect.bottom--;

if ( bBevelin == YES )
	{
	hPen1 = Window.hShadowPen;
	hPen2 = (HPEN)GetStockObject(WHITE_PEN);
	}
else
if ( bBevelin == NO )
	{
	hPen1 = (HPEN)GetStockObject(WHITE_PEN);
	hPen2 = Window.hShadowPen;
	}
else
if ( bBevelin == -1 )
	hPen1 = hPen2 = (HPEN)GetStockObject(BLACK_PEN);
else
if ( bBevelin == -2 )
	hPen1 = hPen2 = Window.hButtonPen;

hOldPen = (HPEN)SelectObject( hDC, GetStockObject(WHITE_PEN) );
SelectObject( hDC, hPen1 );
MoveToEx( hDC, Rect.left, Rect.bottom, NULL );
LineTo( hDC, Rect.left, Rect.bottom-2 );
MoveToEx( hDC, Rect.left, Rect.top+1, NULL );
LineTo( hDC, Rect.left, Rect.top );
LineTo( hDC, Rect.left+2, Rect.top );
MoveToEx( hDC, Rect.right-1, Rect.top, NULL );
LineTo( hDC, Rect.right+1, Rect.top );
SelectObject( hDC, hPen2 );
MoveToEx( hDC, Rect.right, Rect.top, NULL );
LineTo( hDC, Rect.right, Rect.top+2 );
MoveToEx( hDC, Rect.right, Rect.bottom-1, NULL );
LineTo( hDC, Rect.right, Rect.bottom );
LineTo( hDC, Rect.right-2, Rect.bottom );
MoveToEx( hDC, Rect.left+1, Rect.bottom, NULL );
LineTo( hDC, Rect.left, Rect.bottom );

SelectObject( hDC, hOldPen );
}

/***********************************************************************/
void SetClipDC( HDC hDC, LPRECT lpRect )
/***********************************************************************/
{
RECT rRect;

rRect = *lpRect;
rRect.right--;
rRect.bottom--;
SelectClipRect( hDC, &rRect, NULL );
}


/***********************************************************************/
BOOL SelectClipRect(HDC hDC, LPRECT lpNewRect, LPRECT lpOldRect )
/***********************************************************************/
{
HRGN	hRgn;
int	iRet;

// this routines takes care of the fact that windows rects are
// different than ours.  i.e. - ours include the right/bottom pixels
if (lpOldRect)
	{
	GetClipBox(hDC, lpOldRect);
	--lpOldRect->right; --lpOldRect->bottom;
	}
++lpNewRect->right; ++lpNewRect->bottom;
hRgn = CreateRectRgnIndirect(lpNewRect);
--lpNewRect->right; --lpNewRect->bottom;
if (!hRgn)
	return(FALSE);
iRet = SelectClipRgn(hDC, hRgn);
DeleteObject(hRgn);
return(iRet != ERROR);
}


/************************************************************************
 	DelUnusedMenuItems
		Removes items from the system menu of the window.
		IF fGrayItems then it will remove all grayed items :
				The window should call GetSystemMenu in its initialization so
					that windows will setup a private system menu.
				The window can call this routine at its first WM_INITMENU message.
		ELSE all items except size and close will be deleted:
				This version can be called anytime.
************************************************************************/
//************************************************************************
void DelUnusedSysMenuItems(HWND hWnd, BOOL fGrayItems, BOOL fSize)
//************************************************************************
{
HMENU hMenu;
int item;
int state;
WORD wID;
BOOL Delete;

hMenu = GetSystemMenu(hWnd, FALSE);
item = GetMenuItemCount(hMenu);
if (fGrayItems)
	for (item--; item >= 0; item--)
		{
		state = GetMenuState(hMenu, item, MF_BYPOSITION);
		if (state & MF_GRAYED)
		 	{
			DeleteMenu(hMenu, item, MF_BYPOSITION);
			}
		}
else
	for (item--; item >= 0; item--)
		{
		wID = GetMenuItemID(hMenu, item);
		Delete = (wID != SC_CLOSE && wID != SC_MOVE);
		Delete = (Delete && (wID != SC_SIZE || !fSize));
		if (Delete)
			{
			DeleteMenu(hMenu, wID, MF_BYCOMMAND);
			}
		}
//DrawMenuBar(hWnd);
}


//************************************************************************
void SetDlgItemIntExt( HWND hDlg, ITEMID id, int iValue, LPSTR lpString )
//************************************************************************
{
STRING szString;

wsprintf( szString, "%d%ls", iValue, lpString );
SetDlgItemTextNow( hDlg, id, szString );
}


/************************************************************************/
void SetDlgItemTextNow(HWND hDlg, int idCtl, LPSTR lpText)
/************************************************************************/
{
HWND hControl;

if ( lpText && lpText[0] )
	SetDlgItemText(hDlg, idCtl, lpText);
else
	SetDlgItemText(hDlg, idCtl, " "); // This overcomes an RX deficiency
hControl = GetDlgItem(hDlg, idCtl);
InvalidateRect(hControl, NULL, YES);
UpdateWindow(hControl);
}

/************************************************************************/
void SetDlgItemTextMultiple(HWND hDlg, int idFirstCtl, int idLastCtl, LPSTR lpText)
/************************************************************************/
{
int i;

for (i = idFirstCtl; i <= idLastCtl; ++i)
	SetDlgItemText(hDlg, i, lpText);
}

/************************************************************************/
void SetDlgItemIntNow(HWND hDlg, int idCtl, UINT uValue, BOOL fSigned)
/************************************************************************/
{
HWND hControl;

SetDlgItemInt(hDlg, idCtl, uValue, fSigned);
hControl = GetDlgItem(hDlg, idCtl);
InvalidateRect(hControl, NULL, YES);
UpdateWindow(hControl);
}


/***********************************************************************/
BOOL AutoScroll( HWND hWindow, int x, int y)
/***********************************************************************/
{
RECT ClientRect;
BOOL fScrolled, fScrollStarted;

GetDrawClientRect(hWindow, &ClientRect);
fScrolled = fScrollStarted = NO;
if (x < ClientRect.left)
	{
	fScrollStarted = YES;
	fScrolled = SendMessage(hWindow, WM_HSCROLL, SB_LINEUP, 0L);
	}
else
if (x > ClientRect.right)
	{
	fScrollStarted = YES;
	fScrolled = SendMessage(hWindow, WM_HSCROLL, SB_LINEDOWN, 0L);
	}
if (y < ClientRect.top)
	{
	fScrollStarted = YES;
	fScrolled = SendMessage(hWindow, WM_VSCROLL, SB_LINEUP, 0L);
	}
else
if (y > ClientRect.bottom)
	{
	fScrollStarted = YES;
	fScrolled = SendMessage(hWindow, WM_VSCROLL, SB_LINEDOWN, 0L);
	}

if (fScrollStarted)
	SendMessage( hWindow, WM_VSCROLL, SB_ENDSCROLL, 0L );

if ( !fScrolled )
	return( NO );

UpdateWindow(hWindow);
return( YES );
}

//***********************************************************************
// 	Show one child window and hide all other in the range
//	if idShow = -1 then none is shown
//***********************************************************************
void ShowExtra(HWND hParent, int idFirst, int idLast, int idShow)
//***********************************************************************
{
	HWND hWnd;
	int id;
	
	for (id=idFirst;id<=idLast;id++)
	{
		if (id == idShow)
			continue;
		hWnd = GetDlgItem(hParent, id);
		if (hWnd)
 	  		ShowWindow(hWnd, SW_HIDE);
	}
	if (idShow == -1)
		return;
	hWnd = GetDlgItem(hParent, idShow);
	if (hWnd)
 	 	ShowWindow(hWnd, SW_SHOW);
}

//***********************************************************************
// 	Returns the child window or the parent if it fails
//***********************************************************************
HWND GetSafeChildWindow(HWND hParent, int idControl)
//***********************************************************************
{
	HWND hWnd;
	
	hWnd = GetDlgItem(hParent, idControl);
	if (!hWnd)
		return(hParent);
	return(hWnd);
}


// Uses the current background color of the DC as the transparent color.
//***********************************************************************
BOOL TransBlt( HDC hDstDC, int x, int y, int dx, int dy, HDC hSrcDC, int x0, int y0 )
//***********************************************************************
// Uses the current background color of the DC as the transparent color.
{
DWORD rgbBk, rgbFg, rgbBkS;
HBITMAP hMaskBitmap, hOldBitmap;
HDC hMaskDC;
#define ROP_PSDPxax 0x00B8074AL

// Get the current DC color's
rgbBk = GetBkColor( hDstDC );
rgbFg = GetTextColor( hDstDC );
rgbBkS= GetBkColor( hSrcDC );

// make a memory DC for use in color conversion
if ( !( hMaskDC = CreateCompatibleDC( hSrcDC ) ) )
	return( FALSE );

// create a mask bitmap and associated DC
if ( !( hMaskBitmap = CreateBitmap( dx, dy, 1, 1, NULL ) ) )
	{
	DeleteDC( hMaskDC );
	return( FALSE );
	}

// select the mask bitmap into the mono DC
hOldBitmap = (HBITMAP)SelectObject( hMaskDC, hMaskBitmap );

// do a color to mono bitblt to build the mask
// generate 1's where the source is equal to the background, else 0's
SetBkColor( hSrcDC, rgbBk );
BitBlt( hMaskDC, 0, 0, dx, dy, hSrcDC, x0, y0, SRCCOPY );

// do a MaskBlt to copy the bitmap to the dest
SetTextColor( hDstDC, RGB(0,0,0)/*Black*/ );
SetBkColor( hDstDC, RGB(255,255,255)/*White*/ );
BitBlt( hDstDC, x, y, dx, dy, hSrcDC, x0, y0, SRCINVERT );
BitBlt( hDstDC, x, y, dx, dy, hMaskDC, 0 , 0, SRCAND );
BitBlt( hDstDC, x, y, dx, dy, hSrcDC, x0, y0, SRCINVERT );
//BitBlt( hDstDC, x, y, dx, dy, hSrcDC, x0, y0, ROP_PSDPxax );

SelectObject( hMaskDC, hOldBitmap );
DeleteObject( hMaskBitmap );

// Restore the DC colors
SetBkColor( hSrcDC, rgbBkS );
SetBkColor( hDstDC, rgbBk );
SetTextColor( hDstDC, rgbFg );

DeleteDC( hMaskDC );
return( TRUE );
}

/***********************************************************************/
LPSTR GetString (UINT idString,LPSTR lpBuffer)
/***********************************************************************/
{
    #define MAX_STRING 256
    static char szBuffer[MAX_STRING];  // must be static!

    if (!lpBuffer)
        lpBuffer = (LPSTR)szBuffer;
    LoadString (hInstAstral,idString,lpBuffer,MAX_STRING);
    return lpBuffer;
}

/***********************************************************************/

BOOL SetControlFont (HWND hWnd, int iCtrlId)
{
	HFONT	hButtonFont;
	hButtonFont = GetStockFont (ANSI_VAR_FONT);
	if (hButtonFont != (HFONT)NULL)
	{
		SendDlgItemMessage (hWnd, iCtrlId, WM_SETFONT,
		  (WPARAM)hButtonFont, 0L);
		return TRUE;
	}
	return FALSE;
}

