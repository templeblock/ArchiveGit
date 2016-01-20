// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "ppafx.h"
#include "macros.h"
#include "ctxhelp.h"

#define OD_STYLE ( CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE )

/************************************************************************/
void CheckComboItem( HWND hWnd, int idComboBox, int idFirstItem,
                        int idLastItem, int idSelectedItem )
/************************************************************************/
{
int i, iSelected, iIndex;
STRING szString;
HWND hControl, hItem;
BOOL bIsOwnerDraw;

if ( !(hControl = GetDlgItem(hWnd, idComboBox)) )
	return;

bIsOwnerDraw = ( OD_STYLE & GetWindowLong(hControl,GWL_STYLE) );
ComboBox_ResetContent(hControl);
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
			CombineStringID( szString, (WORD)i );
			ComboBox_AddString(hControl, szString);
			++iIndex;
			}
		}
	else
		{
		*szString = _T('\0');
		GetDlgItemText( hWnd, i, szString, sizeof(szString) );
		ComboBox_AddString(hControl, szString);
		++iIndex;
		}
	}
ComboBox_SetCurSel(hControl, iSelected);
}

/************************************************************************/
void TableCheckComboItem( HWND hWnd, int idComboBox, LPINT lpItemID, 
						int nItems, int idSelectedItem, BOOL bUseStrings)
/************************************************************************/
{
	int      i, idItem, iSelected;
	HWND     hControl;
	STRING   szString;
	LPTSTR   lpString = NULL;

	if ( !(hControl = GetDlgItem(hWnd, idComboBox)) )
		return;

	ComboBox_ResetContent(hControl);
	iSelected = -1;
	for ( i = 0; i < nItems; ++i )
	{
		idItem = lpItemID[i];
		if (idItem == idSelectedItem)
			iSelected = i;

		if( bUseStrings )
		{
			if( AstralStr( idItem, &lpString ))
				lstrcpy( szString, lpString );
			else
			if( AstralStr( IDS_NONAME, &lpString ))
				lstrcpy( szString, lpString );
			else
				*szString = _T('\0');
		}
		else 
		{
			*szString = _T('\0');
			GetDlgItemText( hWnd, idItem, szString, sizeof(szString) );
		}
		ComboBox_AddString(hControl, szString);
	}
	if (iSelected < 0 && nItems)
	{
		idItem = lpItemID[0];
		if (idItem == idSelectedItem)
			iSelected = i;
		if (bUseStrings)
		{
			if (AstralStr( idItem, &lpString ) )
				lstrcpy(szString, lpString);
			else
			if( AstralStr( IDS_NONAME, &lpString ))
				lstrcpy( szString, lpString );
			else
				*szString = _T('\0');
		}
		else 
		{
			*szString = _T('\0');
			GetDlgItemText( hWnd, idItem, szString, sizeof(szString) );
		}
		ComboBox_AddString(hControl, szString);
		iSelected = 0;
	}
	if (iSelected >= 0)
		ComboBox_SetCurSel(hControl, iSelected);
}

/***********************************************************************/
ITEMID HandleCombo( HWND hDlg, ITEMID idCombo, UINT codeNotify )
/***********************************************************************/
{
ITEMID idItem;
HWND hControl, hItem;
BOOL bIsOwnerDraw;
STRING szString;
#define OKITEM(id) \
	( (hItem = GetDlgItem( hDlg, id )) && IsWindowEnabled( hItem ) )

if ( codeNotify != CBN_SELCHANGE && codeNotify != CBN_DBLCLK )
	return( 0 );
if ( !(hControl = GetDlgItem(hDlg, idCombo)) )
	return(0);
bIsOwnerDraw = ( OD_STYLE & GetWindowLong(hControl,GWL_STYLE) );
idItem = 0;
if (bIsOwnerDraw)
	{
	idItem = ComboBox_GetCurSel(hControl); 
	*szString = _T('\0');
	ComboBox_GetLBText(hControl, idItem, szString);
	idItem = ExtractStringID( szString );
	}
if (!idItem)
	{
	idItem = idCombo + 1;
	idItem += ComboBox_GetCurSel(hControl); 
	}
if ( !OKITEM(idItem) )
	{ // Item can't be selected, so select the first OK item
	for ( idItem=idCombo+1; idItem<idCombo+10; idItem++ )
		{
		if ( !OKITEM(idItem) )
			continue;
		ComboBox_SetCurSel(hControl, idItem-(idCombo+1));
		break;
		}
	return( 0 );
	}
return( idItem );
}

/***********************************************************************/
ITEMID TableHandleCombo( HWND hDlg, LPINT lpItemID, int nItems,
					ITEMID idCombo, UINT codeNotify )
/***********************************************************************/
{
ITEMID idItem;
HWND hControl;

if ( codeNotify != CBN_SELCHANGE && codeNotify != CBN_DBLCLK )
	return( 0 );
if ( !(hControl = GetDlgItem(hDlg, idCombo)) )
	return(0);
if (!nItems || !lpItemID)
	return(0);
idItem = ComboBox_GetCurSel( hControl );
idItem = lpItemID[idItem];
return( idItem );
}

/***********************************************************************/
void MeasureItem( HWND hDlg, LPMEASUREITEMSTRUCT lpMeasure, BOOL fIcon )
/***********************************************************************/
{
	HWND        hControl;
	HDC         hDC;
	HFONT     	hFont, hOldFont;
	TEXTMETRIC  tm;
	int         Height, ComboHeight;
	DWORD       dwStyle;

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
	else	
	{ // With RX ?->itemHeight was 16 before it gets set here
		hControl = GetDlgItem( hDlg, lpMeasure->CtlID );
		dwStyle = GetWindowLong(hControl, GWL_STYLE);
		if (dwStyle & CBS_THINFONT)
			hFont = (HFONT)GetStockObject(Window.iLabelFont);
		else
		if ( !(hFont = GetWindowFont( hControl ) ))
			hFont = (HFONT)GetStockObject( GetSystemMetrics( SM_DBCSENABLED ) ? SYSTEM_FONT : ANSI_VAR_FONT);

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
void DrawItem( HWND hDlg, const DRAWITEMSTRUCT FAR *lpDraw, int fType )
/***********************************************************************/
{
	DrawItem(hDlg, lpDraw, fType, NO);
}

/***********************************************************************/
void DrawItem( HWND hDlg, const DRAWITEMSTRUCT FAR *lpDraw, int fType,
			BOOL fNoCategories )
/***********************************************************************/
{
int      x, y, i;
RECT     ClientRect;
HICON    hResource;
HWND     hControl, hItem;
HDC      hDC;
BOOL     bFocus, bHilighted, bGrayed;
int      idItem;
STRING   szString, szTemp;
int      iOldMode;
COLORREF lOldColor;
HBRUSH   hBrush;
LPTSTR   lpString;
BOOL     fCategorize, fCategory;
DWORD    dwStyle;
HFONT	  hOldFont;

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

dwStyle = GetWindowLong(hControl, GWL_STYLE);

if (lpDraw->CtlType == ODT_COMBOBOX)
	ComboBox_GetLBText(hControl, lpDraw->itemID, szString);
else
	ListBox_GetText(hControl, lpDraw->itemID, szString);

fCategory = fCategorize = !fNoCategories;

if (fCategorize)
	{
	// determine whether this string is a category or an actual entry
	if (fCategory = IsCategory(szString))
		{
		GetCategory(szString, szString);
		fCategorize = YES;
		}
	else 
	if (fCategorize = GetCategoryName(szString, szTemp))
		{
		for( i = 0; i < INDENT_SPACES; ++i )
			*MyCharNextN( szString, i ) = _T(' ');

		*MyCharNextN( szString, i ) = _T('\0');

		lstrcat(szString, szTemp);
		}
	}

idItem = ExtractStringID( szString );
hItem = (idItem ? GetDlgItem( hDlg, idItem ) : NULL);
bGrayed = ( (lpDraw->itemState & OD_GRAYED) ||
			(lpDraw->hwndItem && !IsWindowEnabled( lpDraw->hwndItem )) ||
			(hItem && !IsWindowEnabled( hItem )) );
bHilighted = ( (lpDraw->itemState & OD_SELECTED) && !bGrayed );
bFocus = ( (lpDraw->itemState & ODS_FOCUS) && !bGrayed );

if (fCategory)
	bHilighted = NO;

// Draw the box interior
hBrush = CreateSolidBrush( 
	GetSysColor( bHilighted ? COLOR_HIGHLIGHT : COLOR_WINDOW ) );
FillRect( hDC, &ClientRect, hBrush );
DeleteObject( hBrush );

// Draw the icon (if any)
if ( idItem && (hResource = LoadIcon( hInstIconLib, MAKEINTRESOURCE(idItem) )) )
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
	DrawIcon( hDC, x, y, hResource );
#ifndef DLL
	if ( bGrayed )
		MgxGrayArea(hDC, ClientRect.left, ClientRect.top,
			RectWidth(&ClientRect), RectHeight(&ClientRect) );
#endif
	}

lpString = szString;
if (fType & OD_LOWERCASE)
	CharLower( szString );
if( !fCategorize )
	if( *lpString )
		lpString = SkipSpaces( lpString );

if (lstrlen(lpString))
	{
	// Draw the text
	iOldMode = SetBkMode( hDC, TRANSPARENT );
	if (fCategory)
		lOldColor = SetTextColor( hDC, RGB(255, 0, 0));
	else
		lOldColor = SetTextColor( hDC,
			GetSysColor( bHilighted ? COLOR_HIGHLIGHTTEXT :
		   	( bGrayed ? COLOR_BTNSHADOW : COLOR_WINDOWTEXT ) ) );

	if (dwStyle & CBS_THINFONT)
		hOldFont = (HFONT)SelectObject(hDC, GetStockObject(Window.iLabelFont));
	else
		hOldFont = NULL;
	DrawText( hDC, lpString, -1, &ClientRect,
		DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	if (hOldFont)
		SelectObject(hDC, hOldFont);
	SetBkMode( hDC, iOldMode );
	SetTextColor( hDC, lOldColor );
	}
if ( bFocus )
	DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );

#ifndef DLL
if ( bHilighted )
{
    HelpContext.OnHints( NULL, 0 );
	HintLine( idItem );
}
#endif
}


/***********************************************************************/
void CombineStringID( LPTSTR lpString, WORD wId )
/***********************************************************************/
{
STRING szNum;

wsprintf( szNum, "%d", wId );

if ( lstrlen(lpString) >= MAX_STR_LEN-8 )
	*( lpString + ( MAX_STR_LEN-8 )) = _T('\0');
lstrcat( lpString, _T(",") );
lstrcat( lpString, szNum );
}

/***********************************************************************/
int ExtractStringID( LPTSTR lpString )
/***********************************************************************/
{
	LPTSTR lp;
	WORD wChar;

	lp = lpString;

	while( TRUE ) 	// look for the comma
	{
		if( IsDBCSLeadByte(( BYTE )*lp ))
			wChar = *(( LPTSTR )lp );
		else
			wChar = *lp;

		if( !( wChar ))
			return( 0 );

		if( wChar == _T(',') )
			break;

		lp = MyCharNext( lp );
	}
	lp = MyCharNext( lp );					// if wChar == _T(',') lp points to the numeric ID
	*MyCharPrev( lpString, lp ) = _T('\0');	// Terminate the prev. string
	return( Ascii2Int( lp ));
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
//		(LONG)(LPTR)_T("*.*");
//wCount = SendDlgItemMessage( hWnd, idComboBox, CB_GETLBTEXTLEN, wIndex, 0L );
//	 SendDlgItemMessage( hWnd, idComboBox, CB_GETLBTEXT, wIndex,
//		(LONG)(LPTR)szString );

/***********************************************************************/
void InfoMeasureItem( HWND hDlg, MEASUREITEMSTRUCT FAR *lpMeasure )
/***********************************************************************/
{
int     i = 0;
RECT    ClientRect;
HDC		hDC;
HWND	hListBox;
TCHAR	szName[256];
HFONT	hOldFont, hFont;
LPTSTR 	pszFaceName;
WORD 	wChar;

hListBox = GetDlgItem( hDlg, lpMeasure->CtlID );

hDC = GetDC( hListBox );

ListBox_GetItemRect( hListBox, lpMeasure->itemID, &ClientRect );

ListBox_GetText( hListBox, lpMeasure->itemID, szName );

while( i < lstrlen( szName ))
{
	if( IsDBCSLeadByte(( BYTE )szName[i] ))
	{
		wChar = *(( LPWORD )( szName + i ));
		i += 2;
	}
	else
		wChar = szName[i++];

	if( wChar == _T(':'))
		break;
}
// Draw the text
ClientRect.left = ( ClientRect.right / 2 ) + 1;

AstralStr( IDS_FONTFACENAME, &pszFaceName );
hFont = CreateFont( -10, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0,
	PROOF_QUALITY, 0, pszFaceName );

hOldFont = ( HFONT )SelectObject( hDC, hFont );

DrawText( hDC, &szName[i], -1, &ClientRect,	DT_LEFT | DT_WORDBREAK | DT_CALCRECT );

if( hOldFont )
	SelectObject( hDC, hOldFont );
	
lpMeasure->itemHeight = RectHeight( &ClientRect ) -1;

DeleteObject( hFont );

ReleaseDC( hListBox, hDC );
}

/***********************************************************************/
void InfoDrawItem( HWND hDlg, const DRAWITEMSTRUCT FAR *lpDraw )
/***********************************************************************/
{
	int      i, j;
	RECT     ClientRect;
	HWND     hControl;
	HDC      hDC;
	TCHAR	 szString[256], szTemp[256];
	int      iOldMode;
	COLORREF lOldColor;
	HBRUSH   hBrush;
	LPTSTR   lpString, pszFaceName;
	HFONT	 hOldFont, hFont;

	if ( lpDraw->itemID == -1 )
		return;

	// Check to see if its focus draw only
	if ( lpDraw->itemAction == ODA_FOCUS )
		return;

	hDC = lpDraw->hDC;
	ClientRect = lpDraw->rcItem;
	hControl = GetDlgItem( hDlg, lpDraw->CtlID );

	if ( lpDraw->CtlType == ODT_COMBOBOX )
		ComboBox_GetLBText( hControl, lpDraw->itemID, szString );
	else
		ListBox_GetText( hControl, lpDraw->itemID, szString );

	// Draw the box interior
	hBrush = CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
	FillRect( hDC, &ClientRect, hBrush );
	DeleteObject( hBrush );

	lpString = szTemp;

	i = j = 0;

	// remove carriage return from szString
	while( *( szString + j ))
	{
		if( IsDBCSLeadByte(( BYTE )*( szString + i )))
		{
			if( *(( LPWORD )( szString + i )) != _T('\r'))
			{
				*(( LPWORD )( szString + j )) = *(( LPWORD )( szString + i ));
				j += 2;
			}
			i += 2;
		}
		else
		{
			if( *( szString + i ) != _T('\r'))
			{
				*( szString + j ) = *( szString + i );
				j++;
			}
			i++;
		}
	}

	i = 0;
	while( TRUE )
	{
		if( IsDBCSLeadByte(( BYTE )*( szString + i )))
		{
			*(( LPWORD )( szTemp + i )) = *(( LPWORD )( szString + i ));
			if( *(( LPWORD )( szString + i )) == _T(':'))
				break;
			i += 2;
		}
		else
		{
			*( szTemp + i ) = *( szString + i );
			if( *( szString + i ) == _T(':'))
				break;
			i++;
		}
	}
	szTemp[++i] = _T('\0');

	if( lstrlen( lpString ))
	{
		// Draw the text
		iOldMode = SetBkMode( hDC, TRANSPARENT );
		lOldColor = SetTextColor( hDC, COLOR_BTNSHADOW );

		ClientRect.right /= 2;

		hOldFont = ( HFONT )SelectObject( hDC, GetStockObject( Window.iLabelFont ) );

		DrawText( hDC, lpString, -1, &ClientRect,	DT_RIGHT | DT_WORDBREAK );

		ClientRect = lpDraw->rcItem;
		ClientRect.left = ( ClientRect.right / 2 ) + 1;

		AstralStr( IDS_FONTFACENAME, &pszFaceName );
		hFont = CreateFont( -10, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0,
			PROOF_QUALITY, 0, pszFaceName );

		SelectObject( hDC, hFont );

		SetTextColor( hDC, RGB( 0, 0, 0 ) );

		DrawText( hDC, MyCharNextN( szString, i ), -1, &ClientRect, DT_LEFT | DT_WORDBREAK );

		if ( hOldFont )
			SelectObject( hDC, hOldFont );

		DeleteObject( hFont );

		SetTextColor( hDC, lOldColor );
		SetBkMode( hDC, iOldMode );
	}
}


