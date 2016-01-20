//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "types.h"
#include "routines.h"

extern HINSTANCE hInstAstral;

static int fType;

#define OD_LOWERCASE 1
#define OD_STYLE ( CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE )

// Static prototypes
static void MeasureItem( HWND hDlg, LPMEASUREITEMSTRUCT lpMeasure, BOOL fIcon );
static void DrawItem( HWND hDlg, LPDRAWITEMSTRUCT lpDraw );

/************************************************************************/
void CheckComboItem( HWND hWnd, int idComboBox, int idFirstItem, int idLastItem, int idSelectedItem )
/************************************************************************/
{
int i, iSelected, iIndex;
STRING szString;
HWND hControl, hItem;
BOOL bIsOwnerDraw;

if ( !(hControl = GetDlgItem(hWnd, idComboBox)) )
	return;

bIsOwnerDraw = ( OD_STYLE & GetWindowLong(hControl,GWL_STYLE) );
SendMessage( hControl, CB_RESETCONTENT, 0, 0L );
iIndex = 0;
for ( i=idFirstItem; i<=idLastItem; i++ )
	{
	if (i == idSelectedItem)
		iSelected = iIndex;
	if (bIsOwnerDraw)
		{
		if (hItem = GetDlgItem(hWnd, i))
			{
			GetWindowText(hItem, szString, sizeof(szString));
			CombineStringID( szString, i );
			SendMessage( hControl, CB_ADDSTRING, 0,
				 (long)(LPTR)szString );
			++iIndex;
			}
		}
	else
		{
		szString[0] = '\0';
		GetDlgItemText( hWnd, i, szString, sizeof(szString) );
		SendMessage( hControl, CB_ADDSTRING, 0, (long)(LPTR)szString );
		++iIndex;
		}
	}
SendMessage( hControl, CB_SETCURSEL, iSelected, 0L);
}


/************************************************************************/
void TableCheckComboItem( HWND hWnd, int idComboBox,
						LPINT lpItemID, int nItems, int idSelectedItem )
/************************************************************************/
{
int i, idItem, iSelected;
LPSTR lpString;
HWND hControl;
STRING szString;

if ( !(hControl = GetDlgItem(hWnd, idComboBox)) )
	return;

SendMessage( hControl, CB_RESETCONTENT, 0, 0L );
iSelected = -1;
for ( i = 0; i < nItems; ++i )
	{
	idItem = lpItemID[i];
	if (idItem == idSelectedItem)
		iSelected = i;
	if (AstralStr( idItem, &lpString ) )
		lstrcpy(szString, lpString);
	else
		lstrcpy(szString, "No Name");
	SendMessage( hControl, CB_ADDSTRING, 0, (long)(LPTR)szString );
	}
if (iSelected >= 0)
	SendMessage( hControl, CB_SETCURSEL, iSelected, 0L);
}


/***********************************************************************/
WORD HandleCombo( HWND hDlg, WPARAM wParam, LPARAM lParam)
{
int idItem, wMsg;
HWND hControl, hItem;
BOOL bIsOwnerDraw;
STRING szString;
#define OKITEM(id) \
	( (hItem = GetDlgItem( hDlg, id )) && IsWindowEnabled( hItem ) )

wMsg = HIWORD(lParam);
if ( wMsg != CBN_SELCHANGE && wMsg != CBN_DBLCLK )
	return( NULL );
if ( !(hControl = GetDlgItem(hDlg, wParam)) )
	return(NULL);
bIsOwnerDraw = ( OD_STYLE & GetWindowLong(hControl,GWL_STYLE) );
idItem = 0;
if (bIsOwnerDraw)
	{
	idItem = SendDlgItemMessage( hDlg, wParam, CB_GETCURSEL, 0, 0L );
	szString[0] = '\0';
	SendDlgItemMessage( hDlg, wParam, CB_GETLBTEXT, idItem,
			 (long)(LPTR)szString );
	idItem = ExtractStringID( szString );
	}
if (!idItem)
	{
	idItem = wParam + 1;
	idItem += SendDlgItemMessage( hDlg, wParam, CB_GETCURSEL, 0, 0L );
	}
if ( !OKITEM(idItem) )
	{ // Item can't be selected, so select the first OK item
	for ( idItem=wParam+1; idItem<wParam+10; idItem++ )
		{
		if ( !OKITEM(idItem) )
			continue;
		SendDlgItemMessage( hDlg, wParam, CB_SETCURSEL,
			idItem-(wParam+1), 0L );
		break;
		}
	return( NULL );
	}
return( idItem );
}


/***********************************************************************/
int TableHandleCombo( HWND hDlg, LPINT lpItemID, int nItems,
					WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
int idItem, wMsg;
HWND hControl;

wMsg = HIWORD(lParam);
if ( wMsg != CBN_SELCHANGE && wMsg != CBN_DBLCLK )
	return( -1 );
if ( !(hControl = GetDlgItem(hDlg, wParam)) )
	return(-1);
if (!nItems || !lpItemID)
	return(-1);
idItem = SendDlgItemMessage( hDlg, wParam, CB_GETCURSEL, 0, 0L );
idItem = lpItemID[idItem];
return( idItem );
}


/***********************************************************************/
BOOL OwnerDraw(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
LPARAM 	lParam,
BOOL 	fIcon)
{
fType = 0;
switch ( msg )
    {
    case WM_MEASUREITEM:
	MeasureItem( hDlg, (LPMEASUREITEMSTRUCT)lParam, fIcon );
	break;
    case WM_DRAWITEM:
	DrawItem( hDlg, (LPDRAWITEMSTRUCT)lParam );
	break;
    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL OwnerDrawEx(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
LPARAM 	lParam,
BOOL 	fIcon,
int 	fFlags)
{
fType = fFlags;
switch ( msg )
    {
    case WM_MEASUREITEM:
	MeasureItem( hDlg, (LPMEASUREITEMSTRUCT)lParam, fIcon );
	break;
    case WM_DRAWITEM:
	DrawItem( hDlg, (LPDRAWITEMSTRUCT)lParam );
	break;
    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
static void MeasureItem(
/***********************************************************************/
HWND hDlg,
LPMEASUREITEMSTRUCT lpMeasure,
BOOL fIcon)
{
HWND hControl;
HDC hDC;
HFONT hFont, hOldFont;
TEXTMETRIC tm;
int Height, ComboHeight;

#ifndef DLL
     ComboHeight = Control.ComboHeight;
#else
     ComboHeight = 20;
#endif

if ( lpMeasure->itemID == -1 ) // the always visible part
	{ // With RX ?->itemHeight was 18 before it gets set here
	// and ComboHeight was 21
	lpMeasure->itemHeight = ComboHeight - 7; // Normally 21-7=14
	}
else	{ // With RX ?->itemHeight was 16 before it gets set here
	hControl = GetDlgItem( hDlg, lpMeasure->CtlID );
	if ( !(hFont = (HFONT)SendMessage( hControl, WM_GETFONT, 0, 0L )) )
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);

	//GetObject( hFont, sizeof(LOGFONT), (LPTR)&logFont );
	//Height = abs(logFont.lfHeight);

	hDC = GetDC(hControl);
	hOldFont = (HFONT)SelectObject(hDC, hFont);
	GetTextMetrics(hDC, &tm);
	Height = tm.tmHeight;
	SelectObject(hDC, hOldFont);
	ReleaseDC(hControl, hDC);
	if (fIcon)
		Height += 4;
	lpMeasure->itemHeight = Height; // Normally 17+3=20
	}
// Changing the width never seems to do anything for combo boxes!!!!
lpMeasure->itemWidth = 50; // doesn't matter
}


/***********************************************************************/
static void DrawItem(
/***********************************************************************/
HWND hDlg,
LPDRAWITEMSTRUCT lpDraw)
{
int x, y;
//int i;
RECT ClientRect;
HICON hIcon;
HWND hControl, hItem;
HDC hDC;
BOOL bFocus, bHilighted, bGrayed;
int idItem, Msg;
STRING szString;
//STRING szTemp;
int iOldMode;
COLORREF lOldColor;
HBRUSH hBrush;
LPSTR lpString;

// Check state (ODS_??) and not action (ODA_??)
#define OD_GRAYED (ODS_GRAYED | ODS_DISABLED)
#define OD_SELECTED (ODS_SELECTED | ODS_CHECKED)
#define INDENT_SPACES 5

if ( lpDraw->itemID == -1 )
	return;

// Check to see if its focus draw only
if ( lpDraw->itemAction == ODA_FOCUS )
	{
	DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );
	return;
	}

hDC = lpDraw->hDC;
ClientRect = lpDraw->rcItem;
hControl = GetDlgItem( hDlg, lpDraw->CtlID );
Msg = ( lpDraw->CtlType == ODT_COMBOBOX ? CB_GETLBTEXT : LB_GETTEXT );

SendMessage( hControl, Msg, lpDraw->itemID, (long)((LPTR)szString) );

idItem = ExtractStringID( szString );
hItem = (idItem ? GetDlgItem( hDlg, idItem ) : NULL);
bGrayed = ( (lpDraw->itemState & OD_GRAYED) ||
			(lpDraw->hwndItem && !IsWindowEnabled( lpDraw->hwndItem )) ||
			(hItem && !IsWindowEnabled( hItem )) );
bHilighted = ( (lpDraw->itemState & OD_SELECTED) && !bGrayed );
bFocus = ( (lpDraw->itemState & ODS_FOCUS) && !bGrayed );

// Draw the box interior
hBrush = CreateSolidBrush( 
	GetSysColor( bHilighted ? COLOR_HIGHLIGHT : COLOR_WINDOW ) );
FillRect( hDC, &ClientRect, hBrush );
DeleteObject( hBrush );

// Draw the icon (if any)
if ( idItem && (hIcon = LoadIcon( hInstAstral, MAKEINTRESOURCE(idItem) )) )
	{
	y = AVG( ClientRect.top, ClientRect.bottom ) - 16;
	if ( !szString[0] )
		{ // No text - center the icon
		x = AVG( ClientRect.left, ClientRect.right ) - 16;
		}
	else	{ // Left justify icon, and push text rectangle to the right
		x = ClientRect.left;
		ClientRect.left += 32;
		}
	DrawIcon( hDC, x, y, hIcon );
#ifndef DLL
	if ( bGrayed )
		MgxGrayArea(hDC, ClientRect.left, ClientRect.top,
			RectWidth(&ClientRect), RectHeight(&ClientRect) );
#endif
	}

// Draw the text
iOldMode = SetBkMode( hDC, TRANSPARENT );
lOldColor = SetTextColor( hDC,
	GetSysColor( bHilighted ? COLOR_HIGHLIGHTTEXT :
	   ( bGrayed ? COLOR_BTNSHADOW : COLOR_WINDOWTEXT ) ) );

lpString = szString;
if (fType & OD_LOWERCASE)
	AnsiLower(szString);
while ( *lpString )
	{ // Skip any leading white space
	if ( *lpString > ' ' )
		break;
	lpString++;
	}
DrawText( hDC, lpString, -1, &ClientRect,
	DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_LEFT);
SetBkMode( hDC, iOldMode );
SetTextColor( hDC, lOldColor );
if ( bFocus )
	DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );
#ifndef DLL
if ( bHilighted )
	HintLine( idItem );
#endif
}


/***********************************************************************/
void CombineStringID(
/***********************************************************************/
LPSTR lpString,
WORD wId)
{
STRING szNum;

wsprintf( szNum, "%d", wId );
if ( lstrlen(lpString) >= MAX_STR_LEN-8 )
	lpString[MAX_STR_LEN-8] = '\0';
lstrcat( lpString, "," );
lstrcat( lpString, szNum );
}


/***********************************************************************/
int ExtractStringID(LPSTR lpString)
{
LPSTR lp;

lp = lpString;
while ( *lp != ',' ) // look for the comma
	{
	if ( !(*lp) )
		return( NULL );
	lp++;
	}

*lp = '\0'; // Terminate the string
lp++; // Point to the numeric ID
return( Ascii2Int( lp ) );
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