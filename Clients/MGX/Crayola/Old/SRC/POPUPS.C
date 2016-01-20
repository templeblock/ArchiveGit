//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HINSTANCE hInstAstral;

/***********************************************************************/
void PopupMenuEx( HWND hWnd, ITEMID idControl, ITEMID idMenu, ITEMID idMenuChange )
/***********************************************************************/
{
HMENU hMenu, hPopup;
HWND hTool;
RECT Rect;
STRING szMenuChange;

if ( !(hMenu = LoadMenu( hInstAstral, MAKEINTRESOURCE(idMenu) )) )
	return;
if ( !(hPopup = GetSubMenu( hMenu, 0 )) )
	{
	DestroyMenu( hMenu );
	return;
	}
if ( idMenuChange )
	{
	if ( GetDlgItemText( hWnd, idMenuChange, szMenuChange, sizeof(STRING) ))
		ModifyMenu( hPopup, IDC_FILENAME, MF_BYCOMMAND | MF_STRING,
			IDC_FILENAME, szMenuChange );
	else
		{
		EnableMenuItem( hPopup, IDC_DELETEEXT, MF_BYCOMMAND|MF_GRAYED );
		EnableMenuItem( hPopup, IDC_RENAMEEXT, MF_BYCOMMAND|MF_GRAYED );
		}
	}

/* Get the handle to the control we'll popup beside */
if ( !(hTool = GetDlgItem( hWnd, idControl ) ) )
	{
	DestroyMenu( hMenu );
	return;
	}

/* Get screen coordinates of the tool */
GetWindowRect( hTool, &Rect );

/* Draw and track the "floating" popup */
TrackPopupMenu( hPopup, 0, Rect.right, Rect.top, 0, hWnd, NULL );
DestroyMenu( hMenu );
}

/***********************************************************************/
void PopupMenu( HWND hWnd, ITEMID idControl, ITEMID idMenuChange )
/***********************************************************************/
{ // The control id and the menu id are one and the same
PopupMenuEx( hWnd, idControl, idControl, idMenuChange );
}

/***********************************************************************/
BOOL PopupWindow( HWND hParent, ITEMID idControl, ITEMID idDlg, DLGPROC lpDlgControl)
/***********************************************************************/
{
HWND hControl, hPopup;
RECT Rect;

hControl = GetDlgItem(hParent, idControl);
if (!hControl)
	return(FALSE);
GetWindowRect(hControl, &Rect);
if (!LBUTTON)
	return(FALSE);
hPopup = (HWND)AstralDlg( YES, hInstAstral, hParent, idDlg, lpDlgControl );
if (!hPopup)
	return(FALSE);
SetWindowPos(hPopup, NULL, Rect.right, Rect.top, 0, 0,
		 SWP_NOSIZE|SWP_NOZORDER);
SetWindowPos(hPopup, NULL, Rect.right, Rect.top, 0, 0,
		 SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW|SWP_NOZORDER);
UpdateWindow(hPopup);
while (LBUTTON)
	;
AstralDlgEnd( hPopup, TRUE );
return(TRUE);
}
