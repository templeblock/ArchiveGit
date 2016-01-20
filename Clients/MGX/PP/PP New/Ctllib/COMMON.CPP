// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "ctlutil.h"
#include "ctltools.h"

/***********************************************************************/
/***********************************************************************/
// COMMON CONTROL IMPLEMENTATION
/***********************************************************************/
/***********************************************************************/

/***********************************************************************/
// LOCAL variables used by all controls when tracking
/***********************************************************************/
HDC Control_shDC;
BOOL Control_bTrack;

/***********************************************************************/
void Control_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if ( !Control_bTrack )
	return;
ReleaseCapture(); Control_bTrack = FALSE;
ReleaseDC( hWindow, Control_shDC );
}

/***********************************************************************/
UINT Control_OnGetDlgCode(HWND hWindow, LPMSG lpMsg)
/***********************************************************************/
{
return( DLGC_WANTARROWS );
}

/***********************************************************************/
BOOL Control_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
return(TRUE);
}

/***********************************************************************/
void Control_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
FORWARD_WM_ENABLE(hWindow, fEnable, Control_DefProc);
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow(hWindow);
}

/***********************************************************************/
void Control_OnSetText(HWND hWindow, LPCTSTR lpszText)
/***********************************************************************/
{
FORWARD_WM_SETTEXT(hWindow, lpszText, Control_DefProc);
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow(hWindow);
}

/***********************************************************************/
void Control_OnDestroy(HWND hWindow)
/***********************************************************************/
{
if ( !Control_bTrack )
	return;
ReleaseCapture();
Control_bTrack = NO;
}

/***********************************************************************/
void Control_OnSetFont(HWND hWindow, HFONT hFont, BOOL fRedraw)
/***********************************************************************/
{
SetWindowLong(hWindow, GWL_FONT, (LONG)hFont);
if (fRedraw)
	InvalidateRect(hWindow, NULL, TRUE);
}

/***********************************************************************/
void Control_OnPaletteChanged(HWND hWindow, HWND hwndPaletteChange)
/***********************************************************************/
{
InvalidateRect(hWindow, NULL, FALSE);
}

/***********************************************************************/
void Control_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
Control_HandleFocus(hWindow, TRUE);
}

/***********************************************************************/
void Control_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
Control_HandleFocus(hWindow, FALSE);
}

/***********************************************************************/
void Control_HandleFocus(HWND hWindow, BOOL fSetFocus)
/***********************************************************************/
{
if ( !StyleOn( hWindow, WS_TABSTOP ) )
	return;
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow( hWindow );
}

/***********************************************************************/
void Control_OnSetState(HWND hWindow, BOOL fState)
/***********************************************************************/
{
if ( fState == GetWindowWord( hWindow, GWW_STATE ) )
	return;
SetWindowWord( hWindow, GWW_STATE, (WORD)fState );
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow( hWindow );
}

/***********************************************************************/
void Control_OnSetCheck(HWND hWindow, BOOL fCheck)
/***********************************************************************/
{
if ( fCheck == GetWindowWord( hWindow, GWW_STATE ) )
	return;
SetWindowWord( hWindow, GWW_STATE, (WORD)fCheck );
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow( hWindow );
}


/***********************************************************************/
BOOL Control_OnGetCheck(HWND hWindow)
/***********************************************************************/
{
	BOOL fCheck;
	fCheck = GetWindowWord( hWindow, GWW_STATE );
	return(fCheck);
}


