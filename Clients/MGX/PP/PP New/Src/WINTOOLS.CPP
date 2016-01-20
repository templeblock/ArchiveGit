//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include <string.h>
#include "id.h"

extern HINSTANCE hInstStringLib;
extern HINSTANCE hInstHintLib;

void SetSpinBuddy(HWND hDlg, int idCtl)
{
	CWnd *pDlg = CWnd::FromHandle(hDlg);
	CWnd *pEdit = pDlg->GetDlgItem(idCtl);
	if (pEdit)
	{
		CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *)pEdit->GetWindow(GW_HWNDNEXT);
		if (pSpin)
		{
			CWnd *pBuddy = pSpin->GetBuddy();
			if (pBuddy->GetSafeHwnd() != pEdit->GetSafeHwnd())
			{
				pSpin->SetBuddy(pEdit);
				pSpin->ShowWindow(SW_SHOW);
			}
		}
	}
}

void SetSmallCaption(HWND hWnd)
{
	RECT rWindow;
	RECT rClient, rClient2;
	int	dx, dy, iWidth1, iWidth2, iHeight1, iHeight2;

	GetClientRect(hWnd, &rClient);
	iWidth1 = rClient.right - rClient.left;
	iHeight1 = rClient.bottom - rClient.top;
	SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_SMCAPTION);
	// cause a size to happen so stuff gets recalculated
	GetWindowRect(hWnd, &rWindow);
	SetWindowPos(	hWnd, NULL, 0, 0,
					rWindow.right-rWindow.left+1,
					rWindow.bottom-rWindow.top+1,
					SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
	// see how much the client area has grown
	GetClientRect(hWnd, &rClient);
	iWidth2 = rClient.right - rClient.left;
	iHeight2 = rClient.bottom - rClient.top;
 	dx = iWidth2 - iWidth1;	 // 2 should always be bigger
	dy = iHeight2 - iHeight1;

	// size to make client area right again
	GetWindowRect(hWnd, &rWindow);
	SetWindowPos(	hWnd, NULL, 0, 0,
					rWindow.right-rWindow.left-dx,
					rWindow.bottom-rWindow.top-dy,
					SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

	GetClientRect(hWnd, &rClient2);
}

/***********************************************************************/

int SetUnitResolution( int res )
{
	int OldUnitRes;
	LPIMAGE lpImage = GetActiveImage();

	OldUnitRes = Control.UnitRes;

	if ( !(Control.UnitRes = res) )
	{
		Control.UnitRes =
			( lpImage ? FrameResolution(ImgGetBaseEditFrame(lpImage)) : 1000 );
	}
	// setup control library
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);

	return( OldUnitRes );
}

/***********************************************************************/
int AstralStr( int idString, LPPTSTR lppString )
/***********************************************************************/
{
static LPTSTR lpBuffer;
#define STRING_SIZE 256

if( hInstStringLib == NULL )
    return( 0 );

if ( !lpBuffer )
    if ( !(lpBuffer = (LPTSTR)Alloc((long)STRING_SIZE)) )
	{ // Should never happen
	*lppString = (LPTSTR)&lpBuffer;
	return( 0 );
	}

/* Pass back a pointer to our allocated string */
*lppString = lpBuffer;
*lpBuffer = '\0';

/* Get the string from the string table that matches the passed ID */
/* Use this routine when you only need a string for short period of time */
return(LoadString( hInstStringLib, idString, lpBuffer, STRING_SIZE-1 ));
}

/***********************************************************************/
int AstralStrEx( int idString, LPTSTR lpBuffer, int nMaxLen )
/***********************************************************************/
{
*lpBuffer = '\0';

if( hInstStringLib == NULL )
    return( 0 );

/* Get the string from the string table that matches the passed ID */
/* Use this routine when you only need a string for short period of time */
return(LoadString( hInstStringLib, idString, lpBuffer, nMaxLen-1 ));
}

/////////////////////////////////////////////////////////////////////////
int AstralHint( int idString, LPPTSTR lppString )
/////////////////////////////////////////////////////////////////////////
{
static LPTSTR lpBuffer;
#define STRING_SIZE 256

if( hInstHintLib == NULL )
    return( 0 );

if ( !lpBuffer )
    if ( !(lpBuffer = (LPTSTR)Alloc((long)STRING_SIZE)) )
	{ // Should never happen
	*lppString = (LPTSTR)&lpBuffer;
	return( 0 );
	}

// Pass back a pointer to our allocated string
*lppString = lpBuffer;
*lpBuffer = '\0';

// Get the hint from the string table that matches the passed ID 
// Use this routine when you only need a string for short period of time 
return(LoadString( hInstHintLib, idString, lpBuffer, STRING_SIZE-1 ));
}

/////////////////////////////////////////////////////////////////////////
int AstralHintEx( int idString, LPTSTR lpBuffer, int nMaxLen )
/////////////////////////////////////////////////////////////////////////
{
*lpBuffer = '\0';

if( hInstHintLib == NULL )
    return( 0 );

// Get the hint from the string table that matches the passed ID 
// Use this routine when you only need a string for short period of time 
return(LoadString( hInstHintLib, idString, lpBuffer, STRING_SIZE-1 ));
}

/***********************************************************************/
BOOL StringsEqualID( LPTSTR lpString1, ITEMID idString2 )
/***********************************************************************/
{
LPTSTR lpString2;

if ( !AstralStr( idString2, &lpString2 ) )
	return( NO );
return( StringsEqual( lpString1, lpString2 ) );
}


/***********************************************************************/
void GetWinXY(HWND hWnd, LPINT lpX, LPINT lpY)
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
*lpX = UpperLeft.x;
*lpY = UpperLeft.y;
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
HWND hParent = GetParent(hWnd);
if (!hParent)
	hParent = GetWindow(hWnd, GW_OWNER);

GetScreenCenter(hParent, (LPPOINT)&ptCent, cx, cy);
MoveWindow(hWnd, ptCent.x - cx / 2, ptCent.y - cy / 2, cx, cy,
	IsWindowVisible(hWnd) );
}


/***********************************************************************/
void GetScreenCenter( HWND hWnd, LPPOINT ppt, int dx, int dy )
/***********************************************************************/
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
void OffsetWindow( HWND hWnd, int dx, int dy, BOOL bRedraw )
/***********************************************************************/
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
if (hParent == PictPubApp.Get_hWndAstral())
	{
	GetClientRect( PictPubApp.Get_hClientAstral(), &rect );
	ClientToScreen(PictPubApp.Get_hClientAstral(), (LPPOINT)&rect.left);
	ClientToScreen(PictPubApp.Get_hClientAstral(), (LPPOINT)&rect.right);
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
if (hParent == PictPubApp.Get_hWndAstral())
	{
	GetClientRect( PictPubApp.Get_hClientAstral(), &rect );
	ClientToScreen(PictPubApp.Get_hClientAstral(), (LPPOINT)&rect.left);
	ClientToScreen(PictPubApp.Get_hClientAstral(), (LPPOINT)&rect.right);
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
void SavePopup( HWND hWnd, LPTSTR lpKeyName )
/***********************************************************************/
{
HWND hParent;
RECT rect, rParent;
int x1, x2, y1, y2;
STRING szString;
POINT pt;
BOOL bChild;

if ( !hWnd || !(hParent = GetParent(hWnd)) )
	return;

GetWindowRect( hWnd, &rect );

if ( !(bChild = ( GetWindowLong( hWnd, GWL_STYLE ) & WS_POPUP ) == 0 ))
	{
	//GetWindowRect( hParent, &rParent );
	PutDefaultString( lpKeyName, Int4ToAscii( szString, rect.left, rect.top, 0, 0 ) );
	return;
	}
	
GetClientRect( hParent, &rParent );
ClientToScreen( hParent, (LPPOINT)&rParent.left );
ClientToScreen( hParent, (LPPOINT)&rParent.right );

x1 = rect.left - rParent.right;
x2 = rect.right - rParent.left;
y1 = rect.top - rParent.bottom;
y2 = rect.bottom - rParent.top;

if ( abs( x1 ) > abs( x2 ) ) // if its closer to the left...
		pt.x = abs(x2);
else	pt.x = -abs(x1);

if ( abs( y1 ) > abs( y2 ) ) // if its closer to the top...
		pt.y = abs(y2);
else	pt.y = -abs(y1);

PutDefaultString( lpKeyName, Int4ToAscii( szString, pt.x, pt.y, 0, 0 ) );
}

/***********************************************************************/
BOOL RestorePopup( HWND hWnd, LPTSTR lpKeyName, LPPOPUPPROC lpDefaultRoutine )
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
AsciiToInt4( szString, (LPINT)&pt.x, (LPINT)&pt.y, &z, &dummy );
hParent = GetParent(hWnd);
if ( IsIconic(hParent) || !(pt.x + pt.y + dummy) )
	{
	if ( lpDefaultRoutine )
		(*lpDefaultRoutine)( hWnd );
	return(FALSE);
	}

GetWindowRect( hWnd, &rect );

bChild = (( GetWindowLong( hWnd, GWL_STYLE ) & WS_POPUP ) == 0 );

if ( bChild )
{
	GetClientRect( hParent, &rParent );
	ClientToScreen( hParent, (LPPOINT)&rParent.left );
	ClientToScreen( hParent, (LPPOINT)&rParent.right );
	
	dx = RectWidth(&rect) - 1;
	dy = RectHeight(&rect) - 1;
	
	if ( pt.x < 0 )
			pt.x += rParent.right;
	else	pt.x += (rParent.left - dx);
	
	if ( pt.y < 0 )
			pt.y += rParent.bottom;
	else	pt.y += (rParent.top - dy);
	
	SetRect( &rect, pt.x, pt.y, pt.x + dx, pt.y + dy );
	
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
	} //endif child
else
	{
	// not child, use window coords
	hParent = GetDesktopWindow();
	GetClientRect( hParent, &rParent );
	ClientToScreen( hParent, (LPPOINT)&rParent.left );
	ClientToScreen( hParent, (LPPOINT)&rParent.right );
	pt.y = mbound(pt.y, rParent.top, rParent.bottom-20);
	pt.x = mbound(pt.x, rParent.left, rParent.right-20);
	OffsetRect(&rect, pt.x - rect.left, pt.y - rect.top);		
	}

MoveWindow( hWnd, rect.left, rect.top,
	RectWidth(&rect)-1, RectHeight(&rect)-1, IsWindowVisible(hWnd) );
return(TRUE);
}

/***********************************************************************/
// restores size and position
/***********************************************************************/
BOOL RestoreFullPopup( HWND hWnd, LPTSTR lpKeyPosName, LPTSTR lpKeySizeName, LPPOPUPPROC lpDefaultRoutine )
/***********************************************************************/
{
RECT rect, rParent;
int dx, dy, z, dummy;
STRING szString;
POINT pt;
HWND hParent;
BOOL bChild;
int delta;

GetDefaultString( lpKeyPosName, "0,0,0,0", szString, sizeof(STRING) );
AsciiToInt4( szString, (LPINT)&pt.x, (LPINT)&pt.y, &z, &dummy );
hParent = GetParent(hWnd);
if ( IsIconic(hParent) || !(pt.x + pt.y + dummy) )
	{
	if ( lpDefaultRoutine )
		(*lpDefaultRoutine)( hWnd );
	return(FALSE);
	}

GetDefaultString( lpKeySizeName, "0,0", szString, sizeof(STRING) );
AsciiToInt2( szString, &dx, &dy);

GetWindowRect( hWnd, &rect );

if (!(dx+dy))
{
	dx = RectWidth(&rect) - 1;
	dy = RectHeight(&rect) - 1;
}

bChild = (( GetWindowLong( hWnd, GWL_STYLE ) & WS_POPUP ) == 0 );

if ( bChild )
{
	GetClientRect( hParent, &rParent );
	ClientToScreen( hParent, (LPPOINT)&rParent.left );
	ClientToScreen( hParent, (LPPOINT)&rParent.right );
	
	if ( pt.x < 0 )
			pt.x += rParent.right;
	else	pt.x += (rParent.left - dx);
	
	if ( pt.y < 0 )
			pt.y += rParent.bottom;
	else	pt.y += (rParent.top - dy);
	
	SetRect( &rect, pt.x, pt.y, pt.x + dx, pt.y + dy );
	
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
	} //endif child
else
	{
	// not child, use window coords
	hParent = GetDesktopWindow();
	GetClientRect( hParent, &rParent );
	ClientToScreen( hParent, (LPPOINT)&rParent.left );
	ClientToScreen( hParent, (LPPOINT)&rParent.right );
	pt.y = mbound(pt.y, rParent.top, rParent.bottom-20);
	pt.x = mbound(pt.x, rParent.left, rParent.right-20);
	OffsetRect(&rect, pt.x - rect.left, pt.y - rect.top);		
	}

MoveWindow( hWnd, rect.left, rect.top, dx-1, dy-1, IsWindowVisible(hWnd) );
return(TRUE);
}

#ifdef WIN32
static DWORD origstyle;
#else
static WORD origstyle;
#endif

/***********************************************************************/
void DisableSavebits( HWND hWnd )
/***********************************************************************/
{
#ifdef WIN32
DWORD style;

if (!hWnd)
	return;
origstyle = style = GetClassLong( hWnd, GCL_STYLE );
style &= (~CS_SAVEBITS); /* beware the squiggle in XyWrite */
SetClassLong( hWnd, GCL_STYLE, style );
#else
WORD style;

if (!hWnd)
	return;
origstyle = style = GetClassWord( hWnd, GCW_STYLE );
style &= (~CS_SAVEBITS); /* beware the squiggle in XyWrite */
SetClassWord( hWnd, GCW_STYLE, style );
#endif
}


/***********************************************************************/
void EnableSavebits( HWND hWnd )
/***********************************************************************/
{
#ifdef WIN32
SetClassLong( hWnd, GCL_STYLE, origstyle );
#else
SetClassWord( hWnd, GCW_STYLE, origstyle );
#endif
}


/***********************************************************************/
void SetClientRect( HWND hWnd, LPRECT lpRect )
/***********************************************************************/
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
void GetDrawClientRect( HWND hWnd, LPRECT lpRect )
/***********************************************************************/
{ // Calculates the client rectangle taking into account the actual paint area
GetClientRect( hWnd, lpRect );
lpRect->bottom--;
lpRect->right--;
}


/***********************************************************************/
void GetRealClientRect( HWND hWnd, LPRECT lpRect )
/***********************************************************************/
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
void GetScrollBarSize( LPINT lpCx, LPINT lpCy )
/***********************************************************************/
{
*lpCx = GetSystemMetrics(SM_CXVSCROLL) - 1;
*lpCy = GetSystemMetrics(SM_CYHSCROLL) - 1;
}


/***********************************************************************/
void SetFullClientRect( HWND hWnd, BOOL fFull )
/***********************************************************************/
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
 	origstyle = 0;
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
	if ( hMenu = GetMenu( PictPubApp.Get_hWndAstral() ) )
//	hMenu = GetMenu( PictPubApp.Get_hWndAstral() );
		// after getting the style but b4 setting the style
		SetMenu( PictPubApp.Get_hWndAstral(), NULL );
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
		SetMenu( PictPubApp.Get_hWndAstral(), hMenu ); // retore the menu
	}
}


/***********************************************************************/
VOID AstralMenuCheck( int menuID, int *oldmenuID )
/***********************************************************************/
{
CheckMenuItem( GetMenu( PictPubApp.Get_hWndAstral() ), *oldmenuID, MF_UNCHECKED );
CheckMenuItem( GetMenu( PictPubApp.Get_hWndAstral() ), menuID, MF_CHECKED );
*oldmenuID = menuID;
}

/************************************************************************/
void Dline( HDC hDC, int x1, int y1, int x2, int y2, BOOL flag )
/************************************************************************/
{
int OldROP;
BOOL bOldClip = FALSE;

if (!hDC)
{
	hDC = Window.hDC;
	bOldClip = TRUE;
	if (GetActiveDoc())
		SelectClipRect(hDC, &GetActiveDisplay()->DispRect, NULL);
}
OldROP = SetROP2( hDC, R2_NOT );
MoveTo( hDC, x1, y1);
LineTo( hDC, x2, y2);
SetROP2( hDC, OldROP );
if (bOldClip)
	SelectClipRgn(hDC, NULL);
}


/************************************************************************/
void DrawLine( HDC hDC, int x1, int y1, int x2, int y2, HPEN hPen )
/************************************************************************/
{
int OldROP;
HPEN hOldPen;

if (!hDC)
	hDC = Window.hDC;

hOldPen = (HPEN)SelectObject(hDC, hPen);
OldROP = SetROP2( hDC, R2_XORPEN );
MoveTo( hDC, x1, y1);
LineTo( hDC, x2, y2);
SetROP2( hDC, OldROP );
SelectObject(hDC, hOldPen);
}

/************************************************************************/
void Drect( HDC hDC, LPRECT lpRect, BOOL flag )
/************************************************************************/
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
void DrawRect( HDC hDC, LPRECT lpRect, HPEN hPen )
/************************************************************************/
{
int OldROP;
HPEN hOldPen;

if (!hDC)
	hDC = Window.hDC;

hOldPen = (HPEN)SelectObject(hDC, hPen);
OldROP = SetROP2( hDC, R2_XORPEN );
MoveTo( hDC, lpRect->left, lpRect->top );
LineTo( hDC, lpRect->right, lpRect->top );
LineTo( hDC, lpRect->right, lpRect->bottom );
LineTo( hDC, lpRect->left, lpRect->bottom );
LineTo( hDC, lpRect->left, lpRect->top );
SetROP2( hDC, OldROP );
SelectObject(hDC, hOldPen);
}


/************************************************************************/
void Dbox( HDC hDC, LPRECT lpRect, BOOL flag )
/************************************************************************/
{
if (!hDC)
	hDC = Window.hDC;

InvertRect(hDC, lpRect);
}

/************************************************************************/
void Dellipse( HDC hDC, LPRECT lpRect, BOOL flag )
/************************************************************************/
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
VOID SetKeyState( int nVirtKey, BOOL fDown, BOOL fToggle )
/************************************************************************/
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
void ShowControl( HWND hDlg, ITEMID idControl1, ITEMID idControl2,
                  BOOL bShow, BOOL bDefer )
/************************************************************************/
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
		if ( dwStyle & WS_DLGFRAME )
			{ // Turn the Tabstop back on
			dwStyle &= (~WS_DLGFRAME);
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
			dwStyle |= WS_DLGFRAME;
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
// Does NOT include idControl1 & idControl2
// Moves Items
/************************************************************************/
void ShowPane( HWND hDlg, ITEMID idControl1, ITEMID idControl2, BOOL bShow )
/************************************************************************/
{
HWND hControl, hFirstControl;
RECT Rect, rClient;
DWORD dwStyle;
int id, dx, dy;
BOOL bDefer;
HDWP hDefer;
POINT pt, ptPane;
#define BIG 10000

bDefer = YES;
hDefer = NULL;

/* Get the handle to the pane marker control */
if ( !(hControl = GetDlgItem( hDlg, idControl1 )) )
	return;

/* Get screen coordinates of the pane */
GetWindowRect( hControl, &Rect );

if (bShow)
	{
	/* Convert the pane's coordinates to be relative to the parent */
	ScreenToClient( hDlg, (LPPOINT)&Rect.left );
	ptPane.x = Rect.left;
	ptPane.y = Rect.top;

	// Get the handle to the next window
	// never affect the first pane marker control
	if ( !(hFirstControl = GetWindow( hControl, GW_HWNDNEXT ) ) )
		return;
	/* Get screen coordinates of the pane */
	GetWindowRect( hFirstControl, &Rect );

	/* Convert the pane's coordinates to be relative to the parent */
	ScreenToClient( hDlg, (LPPOINT)&Rect.left );
	dx = Rect.left - ptPane.x;
	dy = Rect.top - ptPane.y;
	}

GetClientRect(hDlg, &rClient);
while ( 1 )
	{
	// Get the handle to the next window
	// never affect the first pane marker control
	if ( !(hControl = GetWindow( hControl, GW_HWNDNEXT ) ) )
		break;

	id = GetDlgCtrlID( hControl );
	// break out if we hit second pane marker control
	if ( id == idControl2 )
		break;

	/* Get screen coordinates of the control */
	GetWindowRect( hControl, &Rect );

	/* Convert the control's coordinates to be relative to the parent */
	ScreenToClient( hDlg, (LPPOINT)&Rect.left );
	ScreenToClient( hDlg, (LPPOINT)&Rect.right );

	if ( bShow )
		{
		pt.x = Rect.left;
		pt.y = Rect.top;
//		if ( Rect.left < BIG || Rect.top < BIG )
		if (PtInRect(&rClient, pt))
			continue;
		Rect.left -= dx;
		Rect.top -= dy;
		dwStyle = GetWindowLong( hControl, GWL_STYLE );
		if ( dwStyle & WS_DLGFRAME )
			{ // Turn the Tabstop back on
			dwStyle &= (~WS_DLGFRAME);
			dwStyle |= WS_TABSTOP;
			SetWindowLong( hControl, GWL_STYLE, dwStyle );
			}
		}
	else	{
		pt.x = Rect.left;
		pt.y = Rect.top;
//		if ( Rect.left >= BIG && Rect.top >= BIG )
		if (!PtInRect(&rClient, pt))
			continue;
		Rect.left += BIG;
		Rect.top += BIG;
		dwStyle = GetWindowLong( hControl, GWL_STYLE );
		if ( dwStyle & WS_TABSTOP )
			{ // Turn off the Tabstop
			dwStyle &= (~WS_TABSTOP);
			dwStyle |= WS_DLGFRAME;
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
	}
if (hDefer)
	EndDeferWindowPos( hDefer );
}

/************************************************************************/
// Does not include idControl1 & idControl2
// Moves Items
/************************************************************************/
void MovePane( HWND hDlg, ITEMID idControl1, ITEMID idControl2, int dx, int dy)
/************************************************************************/
{
HWND hControl;
RECT Rect, rClient;
int id;
BOOL bDefer;
HDWP hDefer;
#define BIG 10000

bDefer = YES;
hDefer = NULL;

/* Get the handle to the pane marker control */
if ( !(hControl = GetDlgItem( hDlg, idControl1 )) )
	return;

GetClientRect(hDlg, &rClient);
while ( 1 )
	{
	// Get the handle to the next window
	// never affect the first pane marker control
	if ( !(hControl = GetWindow( hControl, GW_HWNDNEXT ) ) )
		break;

	id = GetDlgCtrlID( hControl );
	// break out if we hit second pane marker control
	if ( id == idControl2 )
		break;

	/* Get screen coordinates of the control */
	GetWindowRect( hControl, &Rect );

	/* Convert the control's coordinates to be relative to the parent */
	ScreenToClient( hDlg, (LPPOINT)&Rect.left );
	ScreenToClient( hDlg, (LPPOINT)&Rect.right );

//	pt.x = Rect.left;
//	pt.y = Rect.top;
//	if ( Rect.left < BIG || Rect.top < BIG )
//	if (PtInRect(&rClient, pt))
//		continue;
	Rect.left += dx;
	Rect.top += dy;

	if ( bDefer )
		{
		if ( !hDefer )
			hDefer = BeginDeferWindowPos( 40 );
		hDefer = DeferWindowPos( hDefer, hControl, 0, /* Same z order */
		Rect.left, Rect.top, /* New location */
		0, 0, /* Same size */
		SWP_NOZORDER | SWP_NOSIZE /* Flags */ );
		}
	else	
		{
		SetWindowPos( hControl, 0, /* Same z order */
		Rect.left, Rect.top, /* New location */
		0, 0, /* Same size */
		SWP_NOZORDER | SWP_NOSIZE /* Flags */ );
		UpdateWindow(hControl);
		}
	}
if (hDefer)
	EndDeferWindowPos( hDefer );
}

/************************************************************************/
// Includes idControl1 & idControl2
// Does not move items.
/************************************************************************/
void ShowDlgItems( HWND hDlg, ITEMID idControl1, ITEMID idControl2,
                     BOOL bShow, BOOL bDefer )
/************************************************************************/
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
		if ( dwStyle & WS_DLGFRAME )
			{ // Turn the Tabstop back on
			dwStyle &= (~WS_DLGFRAME);
			dwStyle |= WS_TABSTOP;
			SetWindowLong( hControl, GWL_STYLE, dwStyle );
			}
		}
	else
		{
		if ( dwStyle & WS_TABSTOP )
			{ // Turn off the Tabstop
			dwStyle &= (~WS_TABSTOP);
			dwStyle |= WS_DLGFRAME;
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
void ShowDlgItem( HWND hDlg, ITEMID idControl, BOOL bShow )
/************************************************************************/
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
void ControlEnableGroup( HWND hDlg, int idControl1, int idControl2,
                           BOOL bEnable )
/************************************************************************/
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


//************************************************************************
//			SetAllDlgItemsText
// PARAMETERS:
//					HWND hDlg-	The dialog. 
//					int id- The id of item(s) to change.
//					LPTSTR lpText- The text to set.
//
// DESCRIPTION:
//			Works like SetDlgItemText.. except it will set text for ALL
//			Controls with given ID.
//************************************************************************
VOID SetAllDlgItemsText(HWND hDlg, int id, LPTSTR lpText)
//************************************************************************
{
	HWND hControl;
	int idControl;
	
	if (!(hControl = GetDlgItem(hDlg, id)))
		return;
	if (!(hControl = GetWindow( hControl, GW_HWNDFIRST )))
		return;
	do
	{
		idControl = GetDlgCtrlID(hControl);
		if (idControl == id)
			SetWindowText(hControl, lpText);
		
	} while ((hControl = GetWindow(hControl, GW_HWNDNEXT)));
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
	GetParent( hWnd ),
	(HMENU)(GetDlgCtrlID( hWnd )+1),
	GetWindowInstance( hWnd ),
	NULL /* lpParam */ ) );
}


/************************************************************************/
BOOL EnableOverlappedWindow( HWND hWnd, BOOL bEnable )
/************************************************************************/
{
	HWND 	hOwner, hTopMostParent, hParent, hSibling;
	static int iNumTimesDisabled = 0;

	if (!hWnd)
		return(FALSE);

	// get top-level parent of new parent window
	// we get first ancestor that is not a child window
	hTopMostParent = hWnd;
	while (hParent = GetParent(hWnd))
	{
		if (!IsChild(hParent, hWnd))
			break;
		hTopMostParent = hParent;
		hWnd = hParent;
	}

	// use reference count so we only disable once
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

	// this call allows client to enable/disable modeless dialogs
	AfxGetApp()->EnableModeless(bEnable);

	// get owner of the topmost parent. if none, then parent is owner
	hOwner = GetWindow(hTopMostParent, GW_OWNER);
	if (!hOwner)
		hOwner = hTopMostParent;

	// if the topmost owner is not the parent then enable/disable it
	if (hOwner != hTopMostParent)
		EnableWindow(hOwner, bEnable);

	// enable/disable all topmost windows that are not child windows
	// and are owned by the owner of the window we are enable/disable ing
	hSibling = GetWindow(hOwner, GW_HWNDFIRST);
	while (hSibling)
	{
		// if the windows is owned by our topmost window
		// and if the window is not the topmost ancestor
		if ((GetWindow(hSibling, GW_OWNER) == hOwner) &&
			(hSibling != hTopMostParent) &&
			!IsChild(hOwner, hSibling))
			EnableWindow(hSibling, bEnable);
		hSibling = GetWindow(hSibling, GW_HWNDNEXT);
	}
	return(!bEnable);
}


/************************************************************************/
HANDLE AstralLoadLibrary( LPTSTR lpLibraryName )
/************************************************************************/
{
#ifdef WIN32
return(LoadLibrary(lpLibraryName));
#else
HANDLE hLib;

hLib = LoadLibrary(lpLibraryName);
if (hLib < HINSTANCE_ERROR)
	return(NULL);
return(hLib);
#endif
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
void SendMessageToChildren(HWND hParent, UINT msg, WPARAM wParam, LPARAM lParam)
/************************************************************************/
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
void DrawBevel( HDC hDC, LPRECT lpRect, BOOL bBevelin, int iWidth )
/***********************************************************************/
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
	SelectObject( hDC, ( bBevelin ? DrawTools.GetBtnShadowPen() :
					GetStockObject(WHITE_PEN) ) );
	MoveTo( hDC, Rect.left, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.top );
	LineTo( hDC, Rect.right, Rect.top );
	SelectObject( hDC, ( !bBevelin ? DrawTools.GetBtnShadowPen() :
					GetStockObject(WHITE_PEN) ) );
	LineTo( hDC, Rect.right, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.bottom );
	InflateRect( &Rect, -1, -1 );
	}

SelectObject( hDC, hOldPen );
}


/***********************************************************************/
void DrawCorners( HDC hDC, LPRECT lpRect, BOOL bBevelin )
/***********************************************************************/
{
HPEN hPen1, hPen2, hOldPen;
RECT Rect;

Rect = *lpRect;
Rect.right--;
Rect.bottom--;

if ( bBevelin == YES )
	{
	hPen1 = DrawTools.GetBtnShadowPen();
	hPen2 = (HPEN)GetStockObject(WHITE_PEN);
	}
else
if ( bBevelin == NO )
	{
	hPen1 = (HPEN)GetStockObject(WHITE_PEN);
	hPen2 = DrawTools.GetBtnShadowPen();
	}
else
if ( bBevelin == -1 )
	hPen1 = hPen2 = (HPEN)GetStockObject(BLACK_PEN);
else
if ( bBevelin == -2 )
	hPen1 = hPen2 = DrawTools.GetBtnFacePen();

hOldPen = (HPEN)SelectObject( hDC, GetStockObject(WHITE_PEN) );
SelectObject( hDC, hPen1 );
MoveTo( hDC, Rect.left, Rect.bottom );
LineTo( hDC, Rect.left, Rect.bottom-2 );
MoveTo( hDC, Rect.left, Rect.top+1 );
LineTo( hDC, Rect.left, Rect.top );
LineTo( hDC, Rect.left+2, Rect.top );
MoveTo( hDC, Rect.right-1, Rect.top );
LineTo( hDC, Rect.right+1, Rect.top );
SelectObject( hDC, hPen2 );
MoveTo( hDC, Rect.right, Rect.top );
LineTo( hDC, Rect.right, Rect.top+2 );
MoveTo( hDC, Rect.right, Rect.bottom-1 );
LineTo( hDC, Rect.right, Rect.bottom );
LineTo( hDC, Rect.right-2, Rect.bottom );
MoveTo( hDC, Rect.left+1, Rect.bottom );
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
BOOL fDelete;

hMenu = GetSystemMenu(hWnd, FALSE);
item = GetMenuItemCount(hMenu);
if (fGrayItems)
	{
	for (item--; item >= 0; item--)
		{
		state = GetMenuState(hMenu, item, MF_BYPOSITION);
		if (state & MF_GRAYED)
		 	{
			DeleteMenu(hMenu, item, MF_BYPOSITION);
			}
		}
	}
else
	{
	for (item--; item >= 0; item--)
		{
		wID = GetMenuItemID(hMenu, item);
		fDelete = (wID != SC_CLOSE && wID != SC_MOVE);
		fDelete = (fDelete && (wID != SC_SIZE || !fSize));
		if (fDelete)
			{
			DeleteMenu(hMenu, wID, MF_BYCOMMAND);
			}
		}
	}
//DrawMenuBar(hWnd);
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

/***********************************************************************/
HBRUSH SetClassBackground(HWND hWnd, HBRUSH hBGBrush)
/***********************************************************************/
{
HBRUSH hOldBGBrush;

#ifdef WIN32
hOldBGBrush = (HBRUSH)GetClassLong(hWnd, GCL_HBRBACKGROUND);
if (hBGBrush)
	SetClassLong(hWnd, GCL_HBRBACKGROUND, (long)hBGBrush);
#else
hOldBGBrush = (HBRUSH)GetClassWord(hWnd, GCW_HBRBACKGROUND);
if (hBGBrush)
	SetClassWord(hWnd, GCW_HBRBACKGROUND, (WORD)hBGBrush);
#endif
return(hOldBGBrush);
}

/***********************************************************************/
HCURSOR SetClassCursor(HWND hWnd, HCURSOR hCursor)
/***********************************************************************/
{
HCURSOR hOldCursor;

#ifdef WIN32
hOldCursor = (HCURSOR)GetClassLong(hWnd, GCL_HCURSOR);
SetClassLong(hWnd, GCL_HCURSOR, (long)hCursor);
#else
hOldCursor = (HCURSOR)GetClassWord(hWnd, GCW_HCURSOR);
SetClassWord(hWnd, GCW_HCURSOR, (WORD)hCursor);
#endif
return(hOldCursor);
}

/***********************************************************************/
int GetClassExtra(HWND hWnd)
/***********************************************************************/
{
int extra;

#ifdef WIN32
extra = (int)GetClassLong(hWnd, GCL_CBCLSEXTRA);
#else
extra = (int)GetClassWord(hWnd, GCW_CBCLSEXTRA);
#endif
return(extra);
}

#ifdef WIN32
/***********************************************************************/
void MoveTo(HDC hDC, int x, int y)
/***********************************************************************/
{
MoveToEx(hDC, x, y, NULL);
}
#endif


#ifdef UNUSED
//for debugging only
void ShowTime(LPTSTR lpString, BOOL bInit)
{
	static DWORD t1;
	STRING szTime;
	
	if (!bInit)
	{
		if (lpString)
			wsprintf(szTime, lpString, (LONG)(GetTickCount() - t1));
		else
			wsprintf(szTime, "%ld", (LONG)(GetTickCount() - t1));
		MessageBox(NULL, szTime, NULL, MB_OK);
	}
	t1 = GetTickCount();
}
#endif

LOCAL HBITMAP CreateMask(HBITMAP hBitmap)
{
	int dx, dy;
	BITMAP   Data;
	HDC hDCMono, hDCSrc;
	HBITMAP hbmMono, hbmMonoOld, hbmSrcOld;

	hDCSrc = ::CreateCompatibleDC(NULL);
	if (!hDCSrc)
		return(NULL);

	hDCMono = ::CreateCompatibleDC(NULL);
	if (!hDCMono)
	{
		DeleteDC(hDCSrc);
		return(NULL);
	}

	GetObject( hBitmap, sizeof(BITMAP), (LPTSTR)&Data );
	dx = Data.bmWidth;
	dy = Data.bmHeight;

	hbmMono = CreateBitmap(dx, dy, 1, 1, NULL);
	if (!hbmMono)
	{
		DeleteDC(hDCSrc);
		DeleteDC(hDCMono);
	}

	hbmSrcOld = (HBITMAP)SelectObject(hDCSrc, hBitmap);
	hbmMonoOld = (HBITMAP)SelectObject(hDCMono, hbmMono);

	// initalize whole area with 0's
	PatBlt(hDCMono, 0, 0, dx, dy, WHITENESS);

	// create mask based on color bitmap
	// convert this to 1's
	SetBkColor(hDCSrc, GetSysColor(COLOR_BTNFACE));

	BitBlt(hDCMono, 0, 0, dx, dy, hDCSrc, 0, 0, SRCCOPY);

	// convert this to 1's
	SetBkColor(hDCSrc, GetSysColor(COLOR_BTNHIGHLIGHT));

	// OR in the new 1's
	BitBlt(hDCMono, 0, 0, dx, dy, hDCSrc, 0, 0, SRCPAINT);

	SelectObject(hDCSrc, hbmSrcOld);
	SelectObject(hDCMono, hbmMonoOld);
	DeleteDC(hDCSrc);
	DeleteDC(hDCMono);
	return(hbmMono); 
}

/*
	DIBs use RGBQUAD format:
		0xbb 0xgg 0xrr 0x00

	Reasonably efficient code to convert a COLORREF into an
	RGBQUAD is byte-order-dependent, so we need different
	code depending on the byte order we're targeting.
*/
#define RGB_TO_RGBQUAD(r,g,b)   (RGB(b,g,r))
#define CLR_TO_RGBQUAD(clr)     (RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)))

LOCAL HBITMAP LoadSysColorBitmap(HINSTANCE hInst, HRSRC hRsrc)
{
	struct COLORMAP
	{
		// use DWORD instead of RGBQUAD so we can compare two RGBQUADs easily
		DWORD rgbqFrom;
		int iSysColorTo;
	};
	static const COLORMAP sysColorMap[] =
	{
		// mapping from color in DIB to system color
		{ RGB_TO_RGBQUAD(0x00, 0x00, 0x00),  COLOR_BTNTEXT },       // black
		{ RGB_TO_RGBQUAD(0x80, 0x80, 0x80),  COLOR_BTNSHADOW },     // dark grey
		{ RGB_TO_RGBQUAD(0xC0, 0xC0, 0xC0),  COLOR_BTNFACE },       // bright grey
		{ RGB_TO_RGBQUAD(0xFF, 0xFF, 0xFF),  COLOR_BTNHIGHLIGHT }   // white
	};
	const int nMaps = 4;

	HGLOBAL hglb;
	if ((hglb = ::LoadResource(hInst, hRsrc)) == NULL)
		return NULL;

	LPBITMAPINFOHEADER lpBitmap = (LPBITMAPINFOHEADER)LockResource(hglb);
	if (lpBitmap == NULL)
		return NULL;

	// make copy of BITMAPINFOHEADER so we can modify the color table
	const int nColorTableSize = 16;
	UINT nSize = lpBitmap->biSize + nColorTableSize * sizeof(RGBQUAD);
	LPBITMAPINFOHEADER lpBitmapInfo = (LPBITMAPINFOHEADER)::malloc(nSize);
	if (lpBitmapInfo == NULL)
		return NULL;
	memcpy(lpBitmapInfo, lpBitmap, nSize);

	// color table is in RGBQUAD DIB format
	DWORD* pColorTable =
		(DWORD*)(((LPBYTE)lpBitmapInfo) + (UINT)lpBitmapInfo->biSize);

	for (int iColor = 0; iColor < nColorTableSize; iColor++)
	{
		// look for matching RGBQUAD color in original
		for (int i = 0; i < nMaps; i++)
		{
			if (pColorTable[iColor] == sysColorMap[i].rgbqFrom)
			{
				pColorTable[iColor] =
					CLR_TO_RGBQUAD(::GetSysColor(sysColorMap[i].iSysColorTo));
				break;
			}
		}
	}

	int nWidth = (int)lpBitmapInfo->biWidth;
	int nHeight = (int)lpBitmapInfo->biHeight;
	HDC hDCScreen = ::GetDC(NULL);
	HBITMAP hbm = ::CreateCompatibleBitmap(hDCScreen, nWidth, nHeight);
	::ReleaseDC(NULL, hDCScreen);
	HDC hDCSrc = ::CreateCompatibleDC(NULL);

	if ((hbm != NULL) && (hDCSrc != NULL))
	{
		HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCSrc, hbm);

		LPBYTE lpBits;
		lpBits = (LPBYTE)(lpBitmap + 1);
		lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

		StretchDIBits(hDCSrc, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight,
			lpBits, (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		SelectObject(hDCSrc, hbmOld);

	}

	if (hDCSrc != NULL)
		DeleteDC(hDCSrc);
	// free copy of bitmap info struct and resource itself
	::free(lpBitmapInfo);
	::FreeResource(hglb);

	return hbm;
}

HBITMAP MgxLoadBitmap(HINSTANCE hInst, LPCTSTR lpszResourceName)
{
	HRSRC 		hRsrcImage;     // handle to loaded resource for image

	ASSERT(lpszResourceName != NULL);

	if ((hRsrcImage = ::FindResource(hInst,lpszResourceName, RT_BITMAP)) == NULL)
		return FALSE;

	return (LoadSysColorBitmap(hInst,hRsrcImage));
}

void MgxDrawBitmap( HDC hDC, int x, int y, HBITMAP hBitmap, BOOL fEnabled )
{
	int      dx, dy;
	BITMAP   Data;
	HDC      hSrcDC;
	short    OldMode;
	DWORD    OldTextColor, OldBkColor;
	HBITMAP  hOldBitmap;

	#define ROP_PSDPxax 0x00B8074AL

	if ( !hDC )
		return;

	GetObject( hBitmap, sizeof(BITMAP), (LPTSTR)&Data );
	dx = Data.bmWidth;
	dy = Data.bmHeight;
	x -= dx/2;
	y -= dy/2;

	if (!fEnabled)
	{
		HBITMAP hbmMono = CreateMask(hBitmap);
		if (!hbmMono)
			return;
		if ( !(hSrcDC = CreateCompatibleDC(hDC)) )
		{
			DeleteObject(hbmMono);
			return;
		}

		hOldBitmap = (HBITMAP)SelectObject( hSrcDC, hbmMono );

		OldTextColor = SetTextColor(hDC, 0L);                  // 0's in mono -> 0 (for ROP)
		OldBkColor = SetBkColor(hDC, (COLORREF)0x00FFFFFFL); // 1's in mono -> 1
		OldMode = SetBkMode( hDC, TRANSPARENT ); // single bit'rs don't copy white

		// disabled - draw the hilighted shadow
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, DrawTools.GetBtnHilightBrush());
		BitBlt( hDC, x+1, y+1, dx, dy, hSrcDC, 0, 0, ROP_PSDPxax );

		SelectObject(hDC, DrawTools.GetBtnShadowBrush());
		BitBlt( hDC, x, y, dx, dy, hSrcDC, 0, 0, ROP_PSDPxax );

		SelectObject(hDC, hOldBrush);
		SetTextColor( hDC, OldTextColor );
		SetBkColor(hDC, OldBkColor);
		SetBkMode( hDC, OldMode );
			    
		SelectObject( hSrcDC, hOldBitmap );
		DeleteDC( hSrcDC );
		DeleteObject(hbmMono);
	}
	else
	{
		if ( !(hSrcDC = CreateCompatibleDC(hDC)) )
			return;
		hOldBitmap = (HBITMAP)SelectObject( hSrcDC, hBitmap );
		BitBlt( hDC, x, y, dx, dy, hSrcDC, 0, 0, SRCCOPY/*SRCAND*/ );
		SelectObject( hSrcDC, hOldBitmap );
		DeleteDC( hSrcDC );
	}
}


