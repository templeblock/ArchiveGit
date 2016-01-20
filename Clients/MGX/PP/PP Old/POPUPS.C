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

#define MENUFLAGS1 (MF_OWNERDRAW | MF_BYCOMMAND)
#define MENUFLAGS2 (MF_OWNERDRAW | MF_BYCOMMAND | MF_MENUBARBREAK)
#define MENUFLAGS3 (MF_OWNERDRAW | MF_BYCOMMAND | MF_MENUBREAK)

extern HWND hWndAstral;
extern HANDLE hInstAstral;

/***********************************************************************/
void PopupMenu( hWnd, idControl, idMenuChange )
/***********************************************************************/
HWND hWnd;
WORD idControl, idMenuChange;
{
HMENU hPopup;
HWND hTool;
RECT Rect;
STRING szMenuChange;

// The control id and the menu id are one and the same
if ( !(hPopup = LoadMenu( hInstAstral, MAKEINTRESOURCE(idControl) )) )
	return;
if ( !(hPopup = GetSubMenu( hPopup, 0 )) )
	return;
if ( idMenuChange )
	{
	if ( GetDlgItemText( hWnd, idMenuChange, szMenuChange, sizeof(STRING) ))
	    ModifyMenu( hPopup, idMenuChange, MF_BYCOMMAND | MF_STRING,
		idMenuChange, szMenuChange );
	}

/* Get the handle to the control we'll popup beside */
if ( !(hTool = GetDlgItem( hWnd, idControl ) ) )
	return;

/* Get screen coordinates of the tool */
GetWindowRect( hTool, &Rect );

/* Draw and track the "floating" popup */
TrackPopupMenu( hPopup, 0, Rect.right, Rect.top, 0, hWnd, NULL );
/* Destroy the menu since were are done with it. */
}

static HMENU hMenu;

/***********************************************************************/
HMENU InitPopupTools()
/***********************************************************************/
{
HMENU hPopup;

/* Get the menu for the popup from the resource file. */
if ( !(hMenu = LoadMenu( hInstAstral, "POPUPTOOLS" )) )
	return( NULL );

// Set the ownerdraw flags; if this could happen if the RC file, it would
hPopup = GetSubMenu( hMenu, IDC_MASK - IDC_FIRSTTOOL );
ModifyMenu( hPopup, IDC_SQUARE,     MENUFLAGS1, IDC_SQUARE, NULL );
ModifyMenu( hPopup, IDC_CIRCLE,     MENUFLAGS2, IDC_CIRCLE, NULL );
ModifyMenu( hPopup, IDC_FREE,       MENUFLAGS2, IDC_FREE, NULL );
ModifyMenu( hPopup, IDC_MAGICWAND,  MENUFLAGS2, IDC_MAGICWAND, NULL );
ModifyMenu( hPopup, IDC_SHIELD,     MENUFLAGS2, IDC_SHIELD, NULL );
ModifyMenu( hPopup, IDC_TRANSFORMER,MENUFLAGS2, IDC_TRANSFORMER, NULL );
ModifyMenu( hPopup, IDC_POINTER,    MENUFLAGS2, IDC_POINTER, NULL );

hPopup = GetSubMenu( hMenu, IDC_RETOUCH - IDC_FIRSTTOOL );
ModifyMenu( hPopup, IDC_PAINT,	 MENUFLAGS1, IDC_PAINT, NULL );
ModifyMenu( hPopup, IDC_SPRAY,	 MENUFLAGS2, IDC_SPRAY, NULL );
ModifyMenu( hPopup, IDC_CLONE,	 MENUFLAGS2, IDC_CLONE, NULL );
ModifyMenu( hPopup, IDC_TEXTURE, MENUFLAGS2, IDC_TEXTURE, NULL );
ModifyMenu( hPopup, IDC_SMEAR,	 MENUFLAGS2, IDC_SMEAR, NULL );

hPopup = GetSubMenu( hMenu, IDC_FILTER - IDC_FIRSTTOOL );
ModifyMenu( hPopup, IDC_SHARP,	 MENUFLAGS1, IDC_SHARP, NULL );
ModifyMenu( hPopup, IDC_SMOOTH,	 MENUFLAGS2, IDC_SMOOTH, NULL );
ModifyMenu( hPopup, IDC_LIGHTEN, MENUFLAGS2, IDC_LIGHTEN, NULL );
ModifyMenu( hPopup, IDC_DARKEN,	 MENUFLAGS2, IDC_DARKEN, NULL );

hPopup = GetSubMenu( hMenu, IDC_FILL - IDC_FIRSTTOOL );
ModifyMenu( hPopup, IDC_VIGNETTE,   MENUFLAGS1, IDC_VIGNETTE, NULL );
ModifyMenu( hPopup, IDC_FLOOD,      MENUFLAGS2, IDC_FLOOD, NULL );
ModifyMenu( hPopup, IDC_TEXTUREFILL,MENUFLAGS2, IDC_TEXTUREFILL, NULL );
ModifyMenu( hPopup, IDC_TINTFILL,   MENUFLAGS2, IDC_TINTFILL, NULL );

hPopup = GetSubMenu( hMenu, IDC_DRAW - IDC_FIRSTTOOL );
ModifyMenu( hPopup, IDC_PENCIL,     MENUFLAGS1, IDC_PENCIL, NULL );
ModifyMenu( hPopup, IDC_DRAWFREE,   MENUFLAGS2, IDC_DRAWFREE, NULL );
ModifyMenu( hPopup, IDC_DRAWCIRCLE, MENUFLAGS2, IDC_DRAWCIRCLE, NULL );
ModifyMenu( hPopup, IDC_DRAWSQUARE, MENUFLAGS2, IDC_DRAWSQUARE, NULL );

return( hMenu );
}


/***********************************************************************/
void PopupTools( hWnd, idTool )
/***********************************************************************/
HWND hWnd;
WORD idTool;
{
HWND hTool;
HMENU hPopup;
RECT Rect;
WORD idFunction;

if ( !hMenu )
	return;

if ( !(hPopup = GetSubMenu( hMenu, idTool - IDC_FIRSTTOOL )) )
	return;

/* Get the handle to the control we'll popup beside */
if ( !(hTool = GetDlgItem( hWnd, idTool ) ) )
	return;

/* Get screen coordinates of the tool */
GetWindowRect( hTool, &Rect );

if ( !DelayIfMouseDown( 500L ) )
	SetKeyState( VK_LBUTTON, NO/*UP*/, NO );

//if ( idTool == IDC_MASK )
//	{
//	idFunction = IDC_FREE;
//	ModifyMenu( hPopup, idFunction, MENUFLAGS1|MF_UNHILITE,idFunction,NULL);
//	idFunction = IDC_SHIELD;
//	ModifyMenu( hPopup, idFunction, MENUFLAGS2 | MF_HILITE,idFunction,NULL);
////HiliteMenuItem( hWnd, hPopup, IDC_FREE, MENUFLAGS1 | MF_UNHILITE);
////HiliteMenuItem( hWnd, hPopup, IDC_SHIELD, MENUFLAGS2 | MF_HILITE);
//	}

/* Draw and track the "floating" popup */
TrackPopupMenu( hPopup, 0, Rect.right, Rect.top, 0, hWnd, NULL );
/* Destroy the menu since were are done with it. */
}


/***********************************************************************/
void MeasurePopupMenu( lpMeasure )
/***********************************************************************/
LPMEASUREITEMSTRUCT lpMeasure;
{
DWORD dSize;

dSize = GetMenuCheckMarkDimensions();
lpMeasure->itemWidth = 32 - LOWORD(dSize);
lpMeasure->itemHeight = 32;
}


/***********************************************************************/
void DrawPopupMenu( lpDraw )
/***********************************************************************/
LPDRAWITEMSTRUCT lpDraw;
{
int x, y;
RECT ClientRect;
HICON hResource;
HDC hDC;
BOOL bFocus, bSelected, bDown;
WORD idIcon;
#define ROP_PSDPxax 0x00B8074AL

// (lpDraw->itemAction == ODA_SELECT)
// (lpDraw->itemAction == ODA_DRAWENTIRE)

bFocus = NO;
bDown = (lpDraw->itemState & ODS_SELECTED);
bSelected = (lpDraw->itemState & ODS_SELECTED);
//bSelected = (lpDraw->itemState & ODS_CHECKED);
hDC = lpDraw->hDC;
ClientRect = lpDraw->rcItem;
idIcon = lpDraw->itemID;

// Draw the box interior
//if ( !bSelected )
if ( !bDown )
	FillRect( hDC, &ClientRect, Window.hButtonBrush );
else	HilightRect( hDC, &ClientRect, Window.hHilightBrush );

// Compute the client rect center
x = ( ClientRect.right + ClientRect.left + 1 ) / 2;
if ( bDown ) x += 2;
y = ( ClientRect.bottom + ClientRect.top + 1 ) / 2;
if ( bDown ) y += 2;

// Draw the icon
if ( hResource = LoadIcon( hInstAstral, MAKEINTRESOURCE(idIcon) ) )
	DrawIcon( hDC, x-16, y-16, hResource );

InflateRect( &ClientRect, 1, 1 );
DrawSculptedBox( hDC, &ClientRect, bFocus, bDown, YES );
}


#define MAX_STANDARD_BRUSHES 6

/***********************************************************************/
void MeasurePopupBrush( hWnd, lpMeasure )
/***********************************************************************/
HWND hWnd;
LPMEASUREITEMSTRUCT lpMeasure;
{
DWORD dSize;
RECT ClientRect;
static int IconHeight;

// Changing the width never seems to do anything for combo boxes!!!!

if ( !IconHeight )
	{
	AstralControlRect( hWnd, IDC_RETOUCH, &ClientRect );
	IconHeight = RectHeight( &ClientRect );
	}
if ( lpMeasure->itemID == -1 )
	{
	AstralControlRect( hWnd, lpMeasure->CtlID, &ClientRect );
	lpMeasure->itemWidth = 100; //RectWidth( &ClientRect );
	lpMeasure->itemHeight = IconHeight;
	}
else
if ( lpMeasure->itemID < MAX_STANDARD_BRUSHES || TRUE )
	{
	AstralControlRect( hWnd, lpMeasure->CtlID, &ClientRect );
	lpMeasure->itemWidth = RectWidth( &ClientRect );
	lpMeasure->itemHeight = 20;
	}
else	{
	lpMeasure->itemWidth = 50; // doesn't matter
	lpMeasure->itemHeight = 50;
	}
}


/***********************************************************************/
void DrawPopupBrush( lpDraw )
/***********************************************************************/
LPDRAWITEMSTRUCT lpDraw;
{
int x, y;
RECT ClientRect;
HICON hResource;
HDC hDC;
BOOL bFocus, bSelected;
WORD idIcon;
#define ROP_PSDPxax 0x00B8074AL
LPTR lp;
RECT rect;
STRING szString;
extern int FullBrush;

// (lpDraw->itemAction == ODA_SELECT)
// (lpDraw->itemAction == ODA_DRAWENTIRE)

if (lpDraw->itemID == -1)
	return;

bFocus = NO;
bSelected = (lpDraw->itemState & ODS_SELECTED);
hDC = lpDraw->hDC;
ClientRect = lpDraw->rcItem;
idIcon = lpDraw->itemID + lpDraw->CtlID + 1;

// Draw the box interior
FillRect( hDC, &ClientRect, Window.hWhiteBrush );

// Compute the client rect center
x = ( ClientRect.right + ClientRect.left + 1 ) / 2;
y = ( ClientRect.bottom + ClientRect.top + 1 ) / 2;

//dbg("itemID = %d itemData = %lx", lpDraw->itemID, lpDraw->itemData);dbg(NULL);
//if ( lpDraw->itemID < MAX_STANDARD_BRUSHES )
if ( idIcon != IDC_BRUSHCUSTOM )
	{
	// Draw the icon
	if ( hResource = LoadIcon( hInstAstral, MAKEINTRESOURCE(idIcon) ) )
		DrawIcon( hDC, x-16, y-16, hResource );
	}
else if (TRUE)
	{
//	GetDlgItemText( lpDraw->hwndItem, idIcon, szString, sizeof(szString) );
//	DrawText(hDC, (LPSTR)lpDraw->itemData, -1, &ClientRect, DT_LEFT);
//	GetWindowText(lpDraw->hwndItem, szString, MAX_STR_LEN);
	DrawText(hDC, "Custom", -1, &ClientRect,
		DT_SINGLELINE | DT_VCENTER | DT_CENTER );
	}
else	{
//	lstrcpy( Retouch.CustomBrushName, (LPTR)lpDraw->itemData );
	lstrcpy( Retouch.CustomBrushName, "Custom" );
	rect = ClientRect;
	rect.right = rect.left + FullBrush - 1;
	rect.bottom = rect.top + FullBrush - 1;
	StartSuperBlt( hDC, &BltScreen, &ClientRect, 1 /*depth*/, 0, 0, 0 );
	lp = Retouch.BrushMask;
	y = FullBrush;
	while ( --y >= 0 )
		{
		SuperBlt( lp );
		lp += FullBrush;
		}
	SuperBlt( NULL );
	}

//InflateRect( &ClientRect, 1, 1 );
if ( bSelected )
	InvertSelection( NULL, hDC, &ClientRect, SL_BLOCK );
}
