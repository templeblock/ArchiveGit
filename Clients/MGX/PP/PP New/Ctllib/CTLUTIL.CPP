// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "ctlutil.h"

static LPBLT Ctl_lpBltScreen;
static int iLabelFont;
static HINSTANCE Ctl_hInstIcon;
static BOOL Ctl_fNewLook;

/************************************************************************/
void Ctl_SetBltScreen(LPBLT lpBltScreen)
/************************************************************************/
{
Ctl_lpBltScreen = lpBltScreen;
}

/************************************************************************/
LPBLT Ctl_GetBltScreen()
/************************************************************************/
{
return(Ctl_lpBltScreen);
}

/************************************************************************/
void Ctl_SetNewLook(BOOL fNewLook)
/************************************************************************/
{
Ctl_fNewLook = fNewLook;
}

/************************************************************************/
BOOL Ctl_IsNewLook()
/************************************************************************/
{
return(Ctl_fNewLook);
}

/************************************************************************/
void Ctl_SetLabelFont(int iFont)
/************************************************************************/
{
iLabelFont = iFont;
}

/************************************************************************/
int Ctl_GetLabelFont()
/************************************************************************/
{
return(iLabelFont);
}

/************************************************************************/
void Ctl_SetIconInstance(HINSTANCE hInstIcon)
/************************************************************************/
{
Ctl_hInstIcon = hInstIcon;
}

/************************************************************************/
HINSTANCE Ctl_GetIconInstance()
/************************************************************************/
{
return(Ctl_hInstIcon);
}

/************************************************************************/
LPTR Ctl_Alloc( long lCount )
/************************************************************************/
{
return( Ctl_AllocX( lCount, 0 ) );
}

/************************************************************************/
LPTR Ctl_AllocX( long lCount, WORD wFlags )
/************************************************************************/
{
HGLOBAL  hMemory;
LPTR     lpMemory;

// Round up to the next 16 byte boundary
lCount++; // Protection against a possible C7.0 bug
lCount = ((lCount + 15) / 16) * 16;

// Allocate the memory from the global heap - NOT limited to 64K
if ( !(hMemory = GlobalAlloc(GMEM_MOVEABLE | wFlags, lCount )) )
	return( NULL );

if ( !( lpMemory = ( LPTR )GlobalLock( hMemory ) ) )
	GlobalFree( hMemory );

return( lpMemory );
}

/************************************************************************/
void Ctl_FreeUp( LPVOID lpMemory )
/************************************************************************/
{
HGLOBAL  hMemory;

if ( !lpMemory )
	return;

#ifdef WIN32
if (!(hMemory = GlobalHandle(lpMemory)))
	return;
#else
{
long lValue;

lValue = GlobalHandle( HIWORD(lpMemory) );
if ( HIWORD(lValue) != HIWORD(lpMemory) ) /* segment portions should match */
	return;
hMemory = (HGLOBAL)LOWORD( lValue );
}
#endif
GlobalUnlock( hMemory );
GlobalFree( hMemory );
}

/***********************************************************************/
void Ctl_EraseBackground( HWND hWnd, HDC hDC, LPRECT lpRect, WORD wType )
/***********************************************************************/
{
	HBRUSH hBrush;
	BOOL fCreatedBrush = FALSE;

	hBrush = Ctl_GetBackgroundBrush( hWnd, hDC, wType );
	if ((int)hBrush-1 <= 20/*COLOR_ENDCOLORS*/)
	{
	    hBrush = CreateSolidBrush( GetSysColor((int)hBrush-1) );
		fCreatedBrush = TRUE;
	}
	FillRect( hDC, lpRect, hBrush );
	if (fCreatedBrush)
		DeleteObject(hBrush);
}

HBRUSH Ctl_GetBackgroundBrush( HWND hWnd, HDC hDC, WORD wType )
{
#ifdef DLGEDIT
return((HBRUSH)GetStockObject(LTGRAY_BRUSH));	
#else
HBRUSH  hBrush;

#ifdef WIN32
if (wType == CTLCOLOR_MSGBOX)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLORMSGBOX, (WPARAM)hDC, (LPARAM)hWnd);
else
if (wType == CTLCOLOR_EDIT)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLOREDIT, (WPARAM)hDC, (LPARAM)hWnd);
else
if (wType == CTLCOLOR_LISTBOX)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLORLISTBOX, (WPARAM)hDC, (LPARAM)hWnd);
else
if (wType == CTLCOLOR_BTN)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLORBTN, (WPARAM)hDC, (LPARAM)hWnd);
else
if (wType == CTLCOLOR_DLG)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLORDLG, (WPARAM)hDC, (LPARAM)hWnd);
else
if (wType == CTLCOLOR_SCROLLBAR)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLORSCROLLBAR, (WPARAM)hDC, (LPARAM)hWnd);
else
if (wType == CTLCOLOR_STATIC)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLORSTATIC, (WPARAM)hDC, (LPARAM)hWnd);
else
	hBrush = NULL;
#else
hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLOR, (WPARAM)hDC, MAKELONG (hWnd,wType));
#endif
if (!hBrush)
	{
	hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	SetBkColor (hDC,GetSysColor (COLOR_WINDOW));
        SetTextColor (hDC,GetSysColor (COLOR_WINDOWTEXT));
	}
return( ( HBRUSH )hBrush );
#endif
}

COLOR Ctl_GetDisabledBtnTextColor()
{
#ifdef CHICAGO
return(GetSysColor(COLOR_GRAYTEXT));
#else
return(GetSysColor(COLOR_BTNSHADOW));
#endif
}

COLOR Ctl_GetBtnTextColor()
{
return(GetSysColor(COLOR_BTNTEXT));
}

COLOR Ctl_GetBtnFaceColor()
{
return(GetSysColor(COLOR_BTNFACE));
}

COLOR Ctl_GetTextColor()
{
return(GetSysColor(COLOR_WINDOWTEXT));
}

COLOR Ctl_GetBtnShadowColor()
{
return(GetSysColor(COLOR_BTNSHADOW));
}

COLOR Ctl_GetBtnHilightColor()
{
return(GetSysColor(COLOR_BTNHIGHLIGHT));
}

COLOR Ctl_GetBtnBorderColor()
{
return(RGB(0, 0, 0));
}




 
