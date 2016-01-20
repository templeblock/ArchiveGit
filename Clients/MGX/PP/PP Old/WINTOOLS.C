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

extern HWND hWndAstral;

/***********************************************************************/
DWORD GetWinXY(hWnd)
/***********************************************************************/
HWND hWnd;
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
int ColorText( hDC, x, y, szStr, length, Color )
/***********************************************************************/
HDC hDC;
int x, y, length;
LPTR szStr;
COLOR Color;
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
void CenterPopup( hWnd )
/***********************************************************************/
HWND	hWnd;
{
POINT   ptCent;
RECT	rectWnd;
int	cx, cy;

GetWindowRect(hWnd, &rectWnd);
cx = RectWidth( &rectWnd );
cy = RectHeight( &rectWnd );
GetScreenCenter(GetParent(hWnd), (LPPOINT)&ptCent, cx, cy);
MoveWindow(hWnd, ptCent.x - cx / 2, ptCent.y - cy / 2, cx, cy, FALSE);
}


/***********************************************************************/
void GetScreenCenter(hWnd, ppt, dx, dy)
/***********************************************************************/
HWND	hWnd;   /* handle to window on the screen */
LPPOINT ppt;	/* ptr to point structure where center coords returned */
int	dx;	/* width of window */
int	dy;	/* height of window */
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
void RightPopup( hWnd )
/***********************************************************************/
HWND	hWnd;
{
POINT   pt;
RECT	rect;
int	dx, dy;

GetWindowRect( hWnd, &rect );
dx = RectWidth( &rect );
dy = RectHeight( &rect );
GetClientRect( GetParent(hWnd), &rect );
pt.x = rect.right - dx - 1;			// Right
pt.y = rect.top + 1;				// Top
// pt.x = (rect.right + rect.left - dx) / 2;	// Centered x
pt.y = (rect.top + rect.bottom - dy) / 2;	// Centered y
MoveWindow( hWnd, pt.x, pt.y, dx, dy, FALSE);
}


/***********************************************************************/
void StretchPopup( hWnd )
/***********************************************************************/
HWND hWnd;
{
int Height;
RECT rect;

GetClientRect( hWnd, &rect );
Height = RectHeight( &rect );
GetClientRect( hWndAstral, &rect );
InflateRect( &rect, 1, 1 );
rect.bottom = rect.top + Height;
MoveWindow( hWnd, rect.left, rect.top, /* New position */
	RectWidth( &rect ), RectHeight( &rect ), NO /* New size */ );
}


static WORD origstyle;
/***********************************************************************/
void DisableSavebits( hWnd )
/***********************************************************************/
HWND hWnd;
{
WORD style;

origstyle = style = GetClassWord( hWnd, GCW_STYLE );
style &= (~CS_SAVEBITS); /* beware the squiggle in XyWrite */
SetClassWord( hWnd, GCW_STYLE, style );
}


/***********************************************************************/
void EnableSavebits( hWnd )
/***********************************************************************/
HWND hWnd;
{
SetClassWord( hWnd, GCW_STYLE, origstyle );
}


/***********************************************************************/
void SetClientRect( hWnd, lpRect )
/***********************************************************************/
HWND hWnd;
LPRECT lpRect;
{
RECT Crect, Wrect;
HWND hParent;
POINT UpperLeft;

GetWindowRect( hWnd, &Wrect );
GetClientRect( hWnd, &Crect );
Wrect.right -= RectWidth(&Crect);
Wrect.right += RectWidth(lpRect);
Wrect.bottom -= RectHeight(&Crect);
Wrect.bottom += RectHeight(lpRect);
UpperLeft.x = Wrect.left;
UpperLeft.y = Wrect.top;
if ( hParent = GetParent( hWnd ) )
	ScreenToClient( hParent, &UpperLeft );
MoveWindow( hWnd, UpperLeft.x, UpperLeft.y,
	RectWidth(&Wrect), RectHeight(&Wrect), NO );
}


/***********************************************************************/
void GetDrawClientRect( hWnd, lpRect )
/***********************************************************************/
HWND hWnd;
LPRECT lpRect;
{ // Calculates the client rectangle taking into account the actual paint area
GetClientRect( hWnd, lpRect );
lpRect->bottom--;
lpRect->right--;
}


/***********************************************************************/
void GetRealClientRect( hWnd, lpRect )
/***********************************************************************/
HWND hWnd;
LPRECT lpRect;
{ // Calculates the client rectangle taking scrollbars into account
DWORD dwStyle;
int cx, cy;

GetClientRect( hWnd, lpRect );
dwStyle = GetWindowLong( hWnd, GWL_STYLE );

GetScrollBarSize(&cx, &cy);
if (dwStyle & WS_HSCROLL)
	lpRect->bottom += cy;
if (dwStyle & WS_VSCROLL)
	lpRect->right += cx;
}


/***********************************************************************/
void GetScrollBarSize( lpCx, lpCy )
/***********************************************************************/
LPINT lpCx, lpCy;
{
*lpCx = GetSystemMetrics(SM_CXVSCROLL) - 1;
*lpCy = GetSystemMetrics(SM_CYHSCROLL) - 1;
}


/***********************************************************************/
void SetFullClientRect( hWnd )
/***********************************************************************/
HWND hWnd;
{
//RECT Crect;
HWND hParent;
int xExcess, yExcess;
POINT UpperLeft;
DWORD style;
static RECT Crect, Wrect;
static DWORD origstyle;
static HMENU hMenu;

if ( origstyle )
	{
	SetMenu( hWndAstral, hMenu );
//	DrawMenuBar( hWndAstral );
	SetWindowLong( hWnd, GWL_STYLE, origstyle );
	origstyle = NULL;
	MoveWindow( hWnd,
		Wrect.left,
		Wrect.top,
		RectWidth( &Crect ),
		RectHeight( &Crect ),
		YES );
	return;
	}
origstyle = style = GetWindowLong( hWnd, GWL_STYLE );
//style &= (~WS_THICKFRAME); /* beware the squiggle in XyWrite */
style &= (~WS_OVERLAPPEDWINDOW); /* beware the squiggle in XyWrite */
SetWindowLong( hWnd, GWL_STYLE, style );
hMenu = GetMenu( hWndAstral );
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
	YES );
}


/***********************************************************************/
VOID AstralMenuCheck( menuID, oldmenuID )
/***********************************************************************/
int	menuID;
int	*oldmenuID;
{
CheckMenuItem( GetMenu( hWndAstral ), *oldmenuID, MF_UNCHECKED );
CheckMenuItem( GetMenu( hWndAstral ), menuID, MF_CHECKED );
*oldmenuID = menuID;
}


/***********************************************************************/
VOID SetDlgItemFixed( hDlg, idDlg, Fixed, bNoConvert )
/***********************************************************************/
HWND	hDlg;
int	idDlg;
FIXED	Fixed;
BOOL	bNoConvert;
{
char	szFixed[16];

if ( bNoConvert )
	FixedAscii( Fixed, szFixed );
else	Unit2String( Fixed, szFixed );
SetDlgItemText( hDlg, idDlg, szFixed );
}


/***********************************************************************/
LPTR Unit2String( Fixed, lpFixed )
/***********************************************************************/
FIXED Fixed;
LPTR lpFixed;
{
long	points, picas;
long	pixels;

switch ( Control.Units )
    {
    case IDC_PREF_UNITMM:
	Fixed = ((Fixed * 254L)+5L) / 10L;
	FixedAscii( Fixed, lpFixed );
	break;

    case IDC_PREF_UNITPICAS:
	Fixed = ((Fixed * 723L)+5L)/10L;
	points = FMUL(1, Fixed);
	picas = points/12L;
	points -= (picas*12L);
	wsprintf(lpFixed, "%ldp%ld", picas, points);
	break;

    case IDC_PREF_UNITCM:
	Fixed = ((Fixed * 254L)+50L) / 100L;
	FixedAscii( Fixed, lpFixed );
	break;

    case IDC_PREF_UNITINCHES:
    default:
	FixedAscii( Fixed, lpFixed );
	break;
    }

return( lpFixed );
}


/***********************************************************************/
FIXED Pixels2Inches( Pixels )
/***********************************************************************/
int Pixels;
{
if ( !lpImage )
	return( 0 );
return( FGET( Pixels, lpImage->xres ) );
}


/***********************************************************************/
FIXED GetDlgItemFixed( hDlg, idDlg, lpTranslated, bNoConvert )
/***********************************************************************/
HWND	hDlg;
int	idDlg;
BOOL FAR *lpTranslated;
BOOL	bNoConvert;
{
    FIXED	Fixed;
    char	szFixed[16];
    char	*s;
    long	picas, points, pixels;
    int		units;

    if ( bNoConvert )
	units = IDC_PREF_UNITINCHES;
    else units = Control.Units;

    GetDlgItemText( hDlg, idDlg, szFixed, sizeof(szFixed) );
    switch ( units ) {
    case IDC_PREF_UNITMM:
	Fixed = AsciiFixed( szFixed );
	Fixed = ((Fixed * 10L)+127L) / 254;
	break;

    case IDC_PREF_UNITCM:
	Fixed = AsciiFixed( szFixed );
	Fixed = ((Fixed * 100L)+127L) / 254;
	break;

    case IDC_PREF_UNITPICAS:
	if (strchr(szFixed, '.')) {
	    Fixed = AsciiFixed(szFixed);
	    Fixed *= 12;
	    Fixed = ((Fixed * 10L)+361L) / 723L;
	}
	else {
	    s = strchr(szFixed, 'p');
	    points = 0;
	    if (s) {
		*s = '\0';
		++s;
		if (lstrlen(s))
		    sscanf(s, "%ld", &points);
	    }
	    if (sscanf(szFixed, "%ld", &picas) != 1)
		picas = 0;
	    points += 12 * picas;
	    Fixed = FGET(points*10L,732);
	}
	break;

    case IDC_PREF_UNITINCHES:
    default:
	Fixed = AsciiFixed(szFixed);
	break;
    }
    return( Fixed );
}


#ifdef UNUSED
/***********************************************************************/
int Spawn( lpApplication )
/***********************************************************************/
LPTR lpApplication;
{
int retc;
static struct {
	WORD environment;
	LPTR commandline;
	LPTR FCB1;
	LPTR FCB2;
	} exec;
static WORD wFCB1Contents[2];

AvailableMemory(); /* Force the memory compaction */
LockData(0);

wFCB1Contents[0] = 2;
wFCB1Contents[1] = SW_SHOWNORMAL;

exec.environment = 0;
exec.commandline = "";
/* sprintf( &exec.commandline, "%c %s", strlen(szCmd)+1, szCmd); */
exec.FCB1 = (LPTR)wFCB1Contents;
exec.FCB2 = (LPTR)NULL;

retc = Int21Function4B( 0, lpApplication, (LPTR)&exec );

UnlockData(0);
return( retc );
}
#endif


/***********************************************************************/
LPTR WhichEdgeCursor( ptPoint, lpRect )
/***********************************************************************/
POINT ptPoint;
LPRECT lpRect;
{
int Closeness;
LPTR lpCursor;

Closeness = 0;
if ( abs( ptPoint.y - lpRect->top    ) <= CLOSENESS ) Closeness |= 1;
if ( abs( ptPoint.y - lpRect->bottom ) <= CLOSENESS ) Closeness |= 2;
if ( abs( ptPoint.x - lpRect->left   ) <= CLOSENESS ) Closeness |= 4;
if ( abs( ptPoint.x - lpRect->right  ) <= CLOSENESS ) Closeness |= 8;
switch ( Closeness )
	{
	case 1: /* top only */
	case 2: /* bottom only */
		lpCursor = IDC_SIZENS; break;
	case 4: /* left only */
	case 8: /* right only */
		lpCursor = IDC_SIZEWE; break;
	case 5: /* top-left */
	case 10: /* bottom-right */
		lpCursor = IDC_SIZENWSE; break;
	case 9: /* top-right */
	case 6: /* bottom-left */
		lpCursor = IDC_SIZENESW; break;
	default:
		lpCursor = IDC_ARROW; break;
	}
return( lpCursor );
}


/************************************************************************/
void Dline( hDC, x1, y1, x2, y2, flag )
/************************************************************************/
HDC hDC;
int x1, y1, x2, y2;
BOOL flag;
{
int OldROP;

if (!hDC)
	hDC = Window.hDC;

OldROP = SetROP2( hDC, R2_NOT );
MoveTo( hDC, x1, y1);
LineTo( hDC, x2, y2);
SetROP2( hDC, OldROP );
}


/************************************************************************/
void Drect( hDC, lpRect, flag )
/************************************************************************/
HDC hDC;
LPRECT lpRect;
BOOL flag;
{
int OldROP;

if (!hDC)
	hDC = Window.hDC;

OldROP = SetROP2( hDC, R2_NOT );
MoveTo( hDC, lpRect->left, lpRect->top );
LineTo( hDC, lpRect->right, lpRect->top );
LineTo( hDC, lpRect->right, lpRect->bottom );
LineTo( hDC, lpRect->left, lpRect->bottom );
LineTo( hDC, lpRect->left, lpRect->top );
SetROP2( hDC, OldROP );
}


/************************************************************************/
VOID SetKeyState(nVirtKey, fDown, fToggled)
/************************************************************************/
int nVirtKey;
BOOL fDown, fToggled;
{
BYTE KeyState[256];

GetKeyboardState((LPTR)KeyState);
KeyState[nVirtKey] = 0;
if (fDown)
    KeyState[nVirtKey] |= 0x8000;
if (fToggled)
    KeyState[nVirtKey] |= 0x0001;
SetKeyboardState((LPTR)KeyState);
}


/************************************************************************/
void ShowControl( hDlg, idControl1, idControl2, bShow, bDefer )
/************************************************************************/
HWND hDlg;
WORD idControl1, idControl2;
BOOL bShow, bDefer;
{
HWND hControl;
RECT Rect;
BOOL bBreakFlag;
WORD id;
static HANDLE hDefer;
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
		}
	else	{
		if ( Rect.left >= BIG && Rect.top >= BIG )
			return;
		Rect.left += BIG;
		Rect.top += BIG;
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
		}

	// Get the handle to the next window
	if ( !(hControl = GetWindow( hControl, GW_HWNDNEXT ) ) )
		break;
	}
}


/************************************************************************/
void CheckComboItem( hWnd, idComboBox, idFirstItem, idLastItem, idSelectedItem )
/************************************************************************/
HWND hWnd;
int idComboBox, idFirstItem, idLastItem, idSelectedItem;
{
int i;
BYTE szString[MAX_STR_LEN];

SendDlgItemMessage( hWnd, idComboBox, CB_RESETCONTENT, 0, 0L );
for ( i=idFirstItem; i<=idLastItem; i++ )
	{
	if ( IsWindowEnabled( GetDlgItem(hWnd,i) ) )
		GetDlgItemText( hWnd, i, szString, sizeof(szString) );
	else	szString[0] = '\0';
	SendDlgItemMessage( hWnd, idComboBox, CB_ADDSTRING, 0,
		(long)(LPTR)szString );
	}
SendDlgItemMessage( hWnd, idComboBox, CB_SETCURSEL,
	idSelectedItem-idFirstItem, 0L);
}


/************************************************************************/
void EraseDialogBackground( hDC, hDlg )
/************************************************************************/
HDC hDC;
HWND hDlg;
{
RECT rect;
HBRUSH hBrush;
BOOL fDelete;

hBrush = (HBRUSH)GetClassWord(hDlg, GCW_HBRBACKGROUND);
if (!hBrush) // if no class background brush, use white
	hBrush = COLOR_WINDOW + 1;
if ((hBrush-1) <= COLOR_ENDCOLORS)
	{
	hBrush = CreateSolidBrush( GetSysColor(hBrush-1) );
	fDelete = YES;
	}
else	fDelete = NO;
GetClientRect( hDlg, &rect );
FillRect( hDC, &rect, hBrush );
if ( fDelete )
	DeleteObject( hBrush );
}


/***********************************************************************/
HBRUSH SetControlColors(hDC, hDlg, hControl, wType)
/***********************************************************************/
HDC	hDC;
HWND	hDlg;
HWND	hControl;
WORD	wType;
{
HBRUSH 	hBrush = NULL;
DWORD 	dwStyle;

dwStyle = GetWindowLong(hControl, GWL_STYLE);
if (wType == CTLCOLOR_LISTBOX && (dwStyle & WS_DISABLED))
	SetTextColor(hDC, GetSysColor(COLOR_BTNFACE));
else
	SetTextColor (hDC,GetSysColor (COLOR_WINDOWTEXT));
if (wType == CTLCOLOR_CUSTOM)
	hBrush = (HBRUSH)GetClassWord(hControl, GCW_HBRBACKGROUND);
else if (wType == CTLCOLOR_EDIT || wType == CTLCOLOR_LISTBOX ||
	 wType == CTLCOLOR_MSGBOX)
	hBrush = COLOR_WINDOW + 1;
if (!hBrush)
	hBrush = (HBRUSH)GetClassWord(hDlg, GCW_HBRBACKGROUND);
if (!hBrush) // if no class background brush, use white
	hBrush = COLOR_WINDOW + 1;
if ((hBrush-1) <= COLOR_ENDCOLORS)
	{
	--hBrush;
	SetBkColor (hDC, GetSysColor (hBrush));
	switch (hBrush)
		{
		case COLOR_BTNFACE:
			hBrush = Window.hButtonBrush;
			break;
		case COLOR_WINDOWFRAME:
			hBrush = Window.hBlackBrush;
			break;
		case COLOR_BTNSHADOW:
			hBrush = Window.hShadowBrush;
			break;
		case COLOR_SCROLLBAR:
		case COLOR_BACKGROUND:
		case COLOR_ACTIVECAPTION:
		case COLOR_INACTIVECAPTION:
		case COLOR_MENU:
		case COLOR_WINDOW:
		case COLOR_MENUTEXT:
		case COLOR_WINDOWTEXT:
		case COLOR_CAPTIONTEXT:
		case COLOR_ACTIVEBORDER:
		case COLOR_INACTIVEBORDER:
		case COLOR_APPWORKSPACE:
		case COLOR_HIGHLIGHT:
		case COLOR_HIGHLIGHTTEXT:
		case COLOR_GRAYTEXT:
		case COLOR_BTNTEXT:
		default:
			hBrush = Window.hWhiteBrush;
			break;
		}
	}
else
	{
	SetBkColor (hDC, GetSysColor (COLOR_BTNFACE));
	}
return(hBrush);
}


/************************************************************************/
void DlgOptionPaint( hDlg )
/************************************************************************/
HWND hDlg;
{
PAINTSTRUCT ps;
HDC hDC;
RECT ClientRect;
HPEN hOldPen;

hDC = BeginPaint( hDlg, &ps );
EndPaint( hDlg, &ps );
return;

GetClientRect(hDlg, &ClientRect);
hOldPen = SelectObject(hDC, Window.hShadowPen);
MoveTo(hDC, ClientRect.left+1, ClientRect.bottom-2);
LineTo(hDC, ClientRect.left+1, ClientRect.top+1);
LineTo(hDC, ClientRect.right-2, ClientRect.top+1);
SelectObject(hDC, Window.hWhitePen);
LineTo(hDC, ClientRect.right-2, ClientRect.bottom-2);
LineTo(hDC, ClientRect.left+1, ClientRect.bottom-2);
SelectObject(hDC, hOldPen);
EndPaint( hDlg, &ps );
}


/************************************************************************/
void HilightRect( hDC, lpRect, hBrush )
/************************************************************************/
HDC hDC;
LPRECT lpRect;
HBRUSH hBrush;
{
COLOR OldBkColor, OldTextColor;


OldBkColor = SetBkColor( hDC, GetSysColor(COLOR_BTNFACE) );
OldTextColor = SetTextColor( hDC, GetSysColor(COLOR_WINDOW) );
FillRect( hDC, lpRect, hBrush );
SetBkColor( hDC, OldBkColor );
SetTextColor( hDC, OldTextColor );
}


// Other Combo Calls
//wCount = SendDlgItemMessage( hWnd, idComboBox, CB_GETCOUNT, 0, 0L );
//	 SendDlgItemMessage( hWnd, idComboBox, CB_RESETCONTENT, 0, 0L );
//wIndex = SendDlgItemMessage( hWnd, idComboBox, CB_GETCURSEL, 0, 0L );
//wIndex = SendDlgItemMessage( hWnd, idComboBox, CB_FINDSTRING, -1,
//		(LONG)(LPTR)szString );
//wIndex = SendDlgItemMessage( hWnd, idComboBox, CB_SELECTSTRING, -1,
//		(LONG)(LPTR)szString );
//wIndex = SendDlgItemMessage( hWnd, idComboBox, CB_INSERTSTRING, wIndex,
//		(LONG)(LPTR)szString );
//	 SendDlgItemMessage( hWnd, idComboBox, CB_SETCURSEL, wIndex, 0L );
//	 SendDlgItemMessage( hWnd, idComboBox, CB_DELSTRING, wIndex, 0L );
//wIndex = SendDlgItemMessage( hWnd, idComboBox, CB_DIR, 0x4010,
//		(LONG)(LPTR)"*.*" );
//wCount = SendDlgItemMessage( hWnd, idComboBox, CB_GETLBTEXTLEN, wIndex, 0L );
//	 SendDlgItemMessage( hWnd, idComboBox, CB_GETLBTEXT, wIndex,
//		(LONG)(LPTR)szString );
