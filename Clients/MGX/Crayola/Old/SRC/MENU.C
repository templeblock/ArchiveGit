//®PL1¯®FD1¯®BT0¯®TP0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HINSTANCE hInstAstral;
extern HWND hWndAstral;

/***********************************************************************/
void SetIconMenus( HMENU hMenu )
/***********************************************************************/
{
ITEMID id;
int i, iCount;
WORD wState;
LPSTR lp;

// Set and menu item bitmaps
iCount = GetMenuItemCount( hMenu );
for ( i=0; i<iCount; i++ )
	{
	id = GetMenuItemID( hMenu, i );
	if ( id == -1 )
		continue;
	if ( lp = (LPSTR)Alloc( (long)sizeof(STRING) ) )
		GetMenuString( hMenu, i, lp, sizeof(STRING), MF_BYPOSITION );
	wState = GetMenuState( hMenu, i, MF_BYPOSITION );
	ModifyMenu( hMenu, i, MF_OWNERDRAW | MF_BYPOSITION | wState, id, lp );
	}
// Cleanup the allocations someday...
}


/***********************************************************************/
void MeasureIconMenu( LPMEASUREITEMSTRUCT lpMeasure )
/***********************************************************************/
{
DWORD dSize;
HDC hDC;
TEXTMETRIC tm;
LPSTR lp;
SIZE size;
 
size.cx = 0;
size.cy = 0;

hDC = GetDC(hWndAstral);
if ( lp = (LPSTR)lpMeasure->itemData )
		GetTextExtentPoint( hDC, lp, lstrlen(lp), &size );
GetTextMetrics(hDC, &tm);
ReleaseDC(hWndAstral, hDC);
dSize = GetMenuCheckMarkDimensions();
lpMeasure->itemWidth = 2 + 32 + size.cx;
lpMeasure->itemHeight = max(tm.tmHeight,32);
}


/***********************************************************************/
void DrawIconMenu( LPDRAWITEMSTRUCT lpDraw )
/***********************************************************************/
{
int x, y, iOldMode;
RECT ClientRect;
HICON hResource;
HDC hDC;
BOOL bSelected, bGrayed;
WORD idItem;
COLORREF lOldColor;
STRING szString;
HBRUSH hBrush;
DWORD dw;
LPSTR lp;

// Check state (ODS_??) and not action (ODA_??)
#define OD_GRAYED (ODS_GRAYED | ODS_DISABLED)
#define OD_SELECTED (ODS_SELECTED | ODS_CHECKED)

if ( lpDraw->itemID == -1 )
	return;

hDC = lpDraw->hDC;
ClientRect = lpDraw->rcItem;
idItem = lpDraw->itemID;
bGrayed = ( lpDraw->itemState & OD_GRAYED );
bSelected = ( (lpDraw->itemState & OD_SELECTED) && !bGrayed );

szString[0] = '\0';
if ( lp = (LPSTR)lpDraw->itemData )
	lstrcpy( szString, lp );

// Draw the box interior
hBrush = CreateSolidBrush( GetSysColor( bSelected?COLOR_HIGHLIGHT:COLOR_MENU ));
FillRect( hDC, &ClientRect, hBrush );
DeleteObject( hBrush );

// Draw the icon (if any)
ClientRect.left += 2;
if ( idItem && (hResource = LoadIcon( hInstAstral, MAKEINTRESOURCE(idItem) )) )
	{
	x = ClientRect.left;
	y = AVG( ClientRect.top, ClientRect.bottom ) - 16;
	DrawIcon( hDC, x, y, hResource );
	if ( bGrayed )
		MgxGrayArea(hDC, x, y, 32, 32 );
	}

dw = GetMenuCheckMarkDimensions();
ClientRect.left += LOWORD(dw);
ClientRect.left += 32;

// Draw the text
iOldMode = SetBkMode( hDC, TRANSPARENT );
lOldColor = SetTextColor( hDC, GetSysColor( bSelected ? COLOR_HIGHLIGHTTEXT :
	( bGrayed ? COLOR_GRAYTEXT : COLOR_MENUTEXT ) ) );
DrawText( hDC, szString, -1, &ClientRect, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
SetBkMode( hDC, iOldMode );
SetTextColor( hDC, lOldColor );
}
