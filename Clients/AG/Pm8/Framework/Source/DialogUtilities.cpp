//****************************************************************************
//
// File Name:  DialogUtilities.cpp
//
// Project:    Renaissance Framework
//
// Author:     Mike Heydlauf
//
// Description: Contains utility functions for the Headline UI
//
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//						 One Gateway Center, Suite 800
//						 Newton, Ma 02158
//						 (617)332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/DialogUtilities.cpp                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#ifndef	_WINDOWS
	#error	This file can only be compilied on Windows
#endif

#include "DialogUtilities.h"
#include "FrameworkResourceIDs.h"
#include "ResourceManager.h"
#include "ApplicationGlobals.h"
#include "Configuration.h"
#include "StandaloneApplication.h"
#include <direct.h>

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "atm.h"

const char kNewline = '\n';
const CString kEditCtrlNewline("\r\n");

//////////////////////////////////////////////////////////////////////////////////
//Function: MoveChildWindows
//
//Description: Moves all children of a parent window 
//			   by specified X and Y
//
//Paramaters:  pParent - parent window whose children are being moved.
//			   nDeltaX - amount to move children in the X direction
//			   nDeltaY - amount to move children in the Y direction
//			   fRedraw - redraw?
//
//Returns:	   VOID
//
//////////////////////////////////////////////////////////////////////////////////
void MoveChildWindows( CWnd* pParent, int nDeltaX, int nDeltaY, BOOL fRedraw )
{
	CWnd* pChild = pParent->GetWindow( GW_CHILD );
	while( pChild )
	{
		CRect	rcChild;
		pChild->GetWindowRect( &rcChild );
		pParent->ScreenToClient( &rcChild );
		rcChild.OffsetRect( nDeltaX, nDeltaY );
		pChild->MoveWindow( &rcChild, fRedraw );
		pChild = pChild->GetWindow( GW_HWNDNEXT );
	}
}



//////////////////////////////////////////////////////////////////////////////////
//Function: MyFontEnumProc
//
//Description: Call back function for EnumFontFamilies.
//			   Loads CArray of Cstrings with the passed in font's name.
//
//Paramaters:  lpelf - pointer to logical-font data 
//			   lpntm - pointer to physical-font data 
//			   FontType - type of font  
//             lParam - address of application-defined data  
//
//Returns:     TRUE so fonts keep getting retrieved.
//
//////////////////////////////////////////////////////////////////////////////////
extern "C" int CALLBACK MyFontEnumProc(
    const ENUMLOGFONT FAR *lpelf,	// pointer to logical-font data 
    const NEWTEXTMETRIC FAR *lpntm,	// pointer to physical-font data 
    unsigned long FontType,	// type of font 
    LPARAM lParam 	// address of application-defined data  
   )
{
	StringArray* pFontList = (StringArray*)lParam;
	int nFromOutline = 0;
	//
	//If the font is an Atm font or a Touch Type font, load it into the list.
    if ((ATMProperlyLoaded() && 
	     ATMFontAvailable((char*)lpelf->elfLogFont.lfFaceName, FW_NORMAL, FALSE, FALSE, FALSE, &nFromOutline)) || 
	     FontType & TRUETYPE_FONTTYPE)
	{
		pFontList->Add( CString(lpelf->elfLogFont.lfFaceName) );
	}



	//
	// we want to continue...
	return TRUE;

	//Get rid of unreferenced formal parameter warning (on warning level 4 )...
	lpntm = lpntm;
}


////////////////////////////////////////////////////////////////////////////////////
//Function: GetFontNames
//
//Description: Loads passed in CArray of CStrings with device font names
//
//Paramaters:  pDC -device from which fonts are being retrieved.
//			   parFonts -pointer to CArray of CStrings to hold font names.
//
//Returns:	   VOID
//
////////////////////////////////////////////////////////////////////////////////////
void GetFontNames( CDC* pDC, StringArray* parFonts )
{
	parFonts->SetSize( 0, kFontArrayGrowSize );
	EnumFontFamilies( pDC->GetSafeHdc(), NULL, (FONTENUMPROC)MyFontEnumProc, (LPARAM)parFonts );
}

////////////////////////////////////////////////////////////////////////////////////
//Function: SortFontNames
//
//Description: Alphabetically sorts passed in CArray of CStrings.
//
//Paramaters:  parFonts -pointer to CArray of CStrings that holds font names.
//
//Returns:	   VOID
//
////////////////////////////////////////////////////////////////////////////////////
void SortFontNames( StringArray* parFonts )
{
	int nArrayLen = parFonts->GetSize();
	int nIdxOut = 0, nIdxIn = 0;
	BOOLEAN fDone = FALSE;
	CString sSwitch;

	for ( nIdxOut = 0; nIdxOut < nArrayLen && !fDone; nIdxOut++ )
	{
		fDone = TRUE;
		for ( nIdxIn = 0; nIdxIn < nArrayLen-1; nIdxIn++ )
		{
			if ( parFonts->GetAt( nIdxIn ) > parFonts->GetAt( nIdxIn + 1) )
			{
				fDone = FALSE;
				sSwitch = parFonts->GetAt( nIdxIn );
				parFonts->SetAt( nIdxIn, CString(parFonts->GetAt( nIdxIn + 1)));
				parFonts->SetAt( nIdxIn + 1, sSwitch);
			}
		}
	}
}

//*****************************************************************************************************
// Function Name:	FormatStringForEditCtrl
//
// Description:	Formats given string for an edit control (ie. converts \n ot \r\l
//						nWordStartPos can be used to maintain an index to a certain words
//						(ie. Keeps track how offset is affected by \n conversion)
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void FormatStringForEditCtrl( RMBCString& rString, int& nWordStartPos )
{
	CString cString((LPCSZ)rString), cNewString = "";
	int nIdx = cString.Find( kNewline );
	if (nIdx == -1) return;
	int nNewLinePos = 0, nOrigStartPos = nWordStartPos;
	while( nIdx != -1)
	{
		nNewLinePos += nIdx;
		cNewString += cString.Left(nIdx) + kEditCtrlNewline;
		cString = cString.Right((cString.GetLength() - nIdx) - 1);
		nIdx = cString.Find( kNewline );
		//
		// Make sure index to the word stays accurate for word hilighting.
		if( nNewLinePos <= nOrigStartPos )// && nIdx != -1 )
		{
			nWordStartPos++;
		}
	}
	//nWordStartPos++;
	cNewString += cString;
	rString = cNewString;
}

//*****************************************************************************************************
// Function Name:	FormatStringForEditCtrl
//
// Description:	Formats given string for an edit control (ie. converts \n ot \r\l
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void FormatStringForEditCtrl( RMBCString& rString )
{
	CString cString((LPCSZ)rString), cNewString = "";
	int nIdx = cString.Find( kNewline );
	if (nIdx == -1) return;
	while( nIdx != -1)
	{
		cNewString += cString.Left(nIdx) + kEditCtrlNewline;
		cString = cString.Right((cString.GetLength() - nIdx) - 1);
		nIdx = cString.Find( kNewline );
	}
	cNewString += cString;
	rString = cNewString;
}

//*****************************************************************************************************
// Function Name:	StripOutNewlineChars
//
// Description:	Removes all new line chars from given string
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void StripOutNewlineChars( CString& cString )
{
	CString cOldString(cString), cNewString = "";
	int nIdx = cOldString.Find( kNewline );
	if (nIdx == -1) return;
	while( nIdx != -1)
	{
		cNewString += cOldString.Left(nIdx);
		cOldString = cOldString.Right((cOldString.GetLength() - nIdx) - 1);
		nIdx = cOldString.Find( kNewline );
	}
	cNewString += cOldString;
	cString = cNewString;
}

// ****************************************************************************
//
//  Function Name:	ParseAndPutInListBox
//
//  Description:		Parses tokens out of passed in string, and loads tokens into listbox.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void ParseAndPutInListBox( RMBCString& rListToParse, CListBox& cListBox )
{
	char* pList = NULL;
	pList = new char[ rListToParse.GetStringLength() + 1 ];
	try
	{
		cListBox.ResetContent();
		strcpy( pList, rListToParse );
		char* pListItem = NULL;
		pListItem = strtok( pList, kListFieldSeperator );
		while( pListItem )
		{
			cListBox.AddString( pListItem );
			pListItem = strtok( NULL, kListFieldSeperator );
		}
	}
	catch(...)
	{
		delete pList;
		throw;
	}
	delete pList;

}

// ****************************************************************************
//
//  Function Name:	StringIsInList
//
//  Description:		Looks for given string in given list.
//
//  Returns:			TRUE if the string is in the list, FALSE otherwise.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN ExactStringIsInList( const RMBCString& rListToParse, const RMBCString& rString )
{
	// This module is only compiled under windows, so I can use a CString
	CString cList( (LPCSZ)rListToParse );
	CString cString( (LPCSZ)rString );
	// Add the list field seperator so we find exact matches not substrings

	CString temp = kListFieldSeperator.operator CString( );

	cString += temp;
	cList	  += temp;
	return (cList.Find( cString ) == -1) ? FALSE : TRUE;
}

// ****************************************************************************
//
//  Function Name:	ParseAndPutInListBox
//
//  Description:		Parses tokens out of passed in string, and loads tokens into listbox.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void ParseAndPutInComboBox( RMBCString& rListToParse, CComboBox& cComboBox )
{
	char* pList = NULL;
	pList = new char[ rListToParse.GetStringLength() + 1 ];
	try
	{
		cComboBox.ResetContent();
		strcpy( pList, rListToParse );
		char* pListItem = NULL;
		pListItem = strtok( pList, kListFieldSeperator );
		while( pListItem )
		{
			cComboBox.AddString( pListItem );
			pListItem = strtok( NULL, kListFieldSeperator );
		}
	}
	catch(...)
	{
		delete pList;
		throw;
	}
	delete pList;

}

// ****************************************************************************
//
//  Function Name:	GetDefaultFont
//
//  Description:		Get the default font from the registry or string table if 
///					   not available.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
RMBCString GetDefaultFont()
{
	//	Default Font
	RMBCString rDefFont;
	RRenaissanceUserPreferences rConfig;
	if ( !rConfig.GetStringValue( RRenaissanceUserPreferences::kDefaultFont, rDefFont ) )
		rDefFont = GetResourceManager( ).GetResourceString( STRING_TEXT_COMPONENT_DEFAULT_FONT );
	
	return rDefFont;
}

// ****************************************************************************
//
//  Function Name:	SubclassControl( )
//
//  Description:		Subclass control obtains the default style information
//                   from the control to subclass, and uses this to create
//                   a similar control based on CWnd, instead of the type
//                   of control in the dialog template.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN SubclassControl( CWnd* pParent, UINT nControlID, CWnd& ctlChild ) 
{
	// Get the control we're stealing...
	CWnd*	pCtl	= pParent->GetDlgItem( nControlID );
	
	CRect	rcCtl;
	CString strText ;

	//
	// Determine all the information of the control we're
	// stealing, so we can use it in the creation of the new control.
	//
	DWORD dwStyle   = pCtl->GetStyle() ;								// Style
	DWORD dwExStyle = pCtl->GetExStyle() ;								// Extended
	pCtl->GetWindowText( strText ) ;										// Window Text
	pCtl->GetWindowRect( rcCtl ) ;										// Screen Position	
	pParent->ScreenToClient( rcCtl ) ;


	BOOLEAN fResult = FALSE ;

	//
	// Create the new control with all the settings 
	// of the control we are replacing.
	//
	if (ctlChild.CreateEx( dwExStyle, NULL, strText, dwStyle, rcCtl.left, rcCtl.top, 
			rcCtl.Width(), rcCtl.Height(), pParent->m_hWnd, (HMENU) nControlID ) )
	{
		// Set the z order of the control
		ctlChild.SetWindowPos( pCtl, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE ) ;
	}

	//
	// Okay, we have everything we need so destroy 
	// the control that we are replacing.
	//
	pCtl->DestroyWindow();

	return fResult ;
}

// ****************************************************************************
//
//  Function Name:	ChangeToDefaultProjectDirectory( )
//
//  Description:		Change working directory to default project directory
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void ChangeToDefaultProjectDirectory()
{
	//
	// Get application path and tack on the projects path...
	RMBCString rFilePath = ::GetApplication( ).GetApplicationPath( );
	rFilePath += ::GetResourceManager().GetResourceString(STRING_OPEN_DEFAULT_PATH);//"Projects\";
	_chdir( (LPCSZ)rFilePath ) ;
}

// ****************************************************************************
//
//  Function Name:	CreateDIBPalette( )
//
//  Description:		Create device independent palette from BITMAPINFO structure
//
//  Returns:			palette handle
//
//  Exceptions:		None
//
// ****************************************************************************
//
HPALETTE CreateDIBPalette (LPBITMAPINFO lpbmi, LPINT lpiNumColors)
{
   LPBITMAPINFOHEADER  lpbi;
   LPLOGPALETTE     lpPal;
   HANDLE           hLogPal;
   HPALETTE         hPal = NULL;
   int              i;
 
   lpbi = (LPBITMAPINFOHEADER)lpbmi;
   if (lpbi->biBitCount <= 8)
       *lpiNumColors = (1 << lpbi->biBitCount);
   else
       *lpiNumColors = 0;  // No palette needed for 24 BPP DIB
 
   if (*lpiNumColors)
      {
      hLogPal = GlobalAlloc (GHND, sizeof (LOGPALETTE) + sizeof (PALETTEENTRY) * (*lpiNumColors));

		if ( hLogPal != NULL )
		{
			lpPal = (LPLOGPALETTE) GlobalLock (hLogPal);

			if ( lpPal != NULL )
			{
				lpPal->palVersion    = 0x300;
				lpPal->palNumEntries = *lpiNumColors;
 
				for (i = 0;  i < *lpiNumColors;  i++)
					{
					lpPal->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
					lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
					lpPal->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
					lpPal->palPalEntry[i].peFlags = 0;
					}
				hPal = CreatePalette (lpPal);
				GlobalUnlock (hLogPal);
			}
			GlobalFree   (hLogPal);
		}
   }
   return hPal;
}

// ****************************************************************************
//
//  Function Name:	LoadResourceBitmap( )
//
//  Description:		Load DIB from resource and get it's palette
//
//  Returns:			bitmap handle or NULL on failure
//
//  Exceptions:		None
//
// ****************************************************************************
//
HBITMAP LoadResourceBitmap(HINSTANCE hInstance, LPSTR lpString, HPALETTE FAR* lphPalette)
{
	// initialise all our variables
	HRSRC			hRsrc				= NULL;
	HGLOBAL		hGlobal			= NULL;
	HGLOBAL		hTemp				= NULL;
	DWORD			dwSize			= 0;
	HBITMAP		hBitmapFinal	= NULL;
	HPALETTE		hPalOld			= NULL;
	LPSTR			lpRes				= NULL;
	LPSTR			lpNew				= NULL;
	HDC			hdc				= NULL;
	int			iNumColors		= 0;
	LPBITMAPINFOHEADER	lpbi	= NULL;
 
	 // find the resource in the resource file
	 hRsrc = FindResource(hInstance, lpString, RT_BITMAP);

    if ( hRsrc != NULL )
    {
		 // get a handle to the resource data
       hTemp = LoadResource(hInstance, hRsrc);

		 if ( hTemp != NULL )
		 {
			 // get it's size
			 dwSize = SizeofResource(hInstance, hRsrc);

			 // lock the resource data
			 lpRes = (char*) LockResource(hTemp);

			 if ( lpRes != NULL )
			 { 
				 // allocate memory to copy the data
				 hGlobal = GlobalAlloc(GHND, dwSize);

				 if ( hGlobal != NULL )
				 {
					 // lock the allocated memory
					 lpNew = (char*)GlobalLock(hGlobal);

					 if ( lpNew != NULL )
					 {
						 // copy the data
						 memcpy(lpNew, lpRes, dwSize);

						 // unlock and free the resource
						 UnlockResource(hTemp);
						 FreeResource(hTemp);
 
						 lpbi = (LPBITMAPINFOHEADER)lpNew;
 
						 // get a screen DC compatible with the screen
						 hdc = GetDC(NULL);

						 // create a palette from the bitmap info header
						 *lphPalette =  CreateDIBPalette ((LPBITMAPINFO)lpbi, &iNumColors);

						 // select the palette into the DC
						 if (*lphPalette)
						 {
							 hPalOld = SelectPalette(hdc,*lphPalette,FALSE);
							 RealizePalette(hdc);
						 }
 
						 // create the DIB bitmap using the DC's palette
						 hBitmapFinal = CreateDIBitmap(	hdc, 
																	(LPBITMAPINFOHEADER)lpbi, 
																	(LONG) CBM_INIT,
																	(LPSTR)lpbi + lpbi->biSize + iNumColors * sizeof(RGBQUAD), 
																	(LPBITMAPINFO)lpbi, 
																	DIB_RGB_COLORS );
 
						if ( *lphPalette && hPalOld )
							SelectPalette(hdc,hPalOld,FALSE);

						 // release the HDC
						 ReleaseDC(NULL,hdc);

						 // unlock the allocated bitmap data
						 GlobalUnlock(hGlobal);

					 } // GlobalLock failed

					 // free the bitmpa data
					 GlobalFree(hGlobal);

				 } // GlobalAlloc failed
			 } // LockResource failed
		 } // LoadResource failed
    } // FindResource failed

	 // return the bitmap
	 return hBitmapFinal;
}


// ****************************************************************************
//
//  Class:				RIndeterminateCheckBox
//
//  Description:		Supports an indeterminate bitmap for tri-state check boxes
//							with the pushbutton (bitmap) style.
//							NOTE: THIS CLASS DOES *NOT* SUPPORT THE AUTO-TRI STATE STYLE.
//
// ****************************************************************************
//

// ****************************************************************************
//
//  Function Name:	RIndeterminateCheckBox
//
//  Description:		ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//

RIndeterminateCheckBox::RIndeterminateCheckBox()
{
	m_nState = kUnChecked;
}


// ****************************************************************************
//
//  Function Name:	~RIndeterminateCheckBox
//
//  Description:		dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIndeterminateCheckBox::~RIndeterminateCheckBox()
{
}

// ****************************************************************************
//
//  Function Name:	RIndeterminateCheckBox::SetIndeterminateBitmap
//
//  Description:		nUp					- bitmap resource id for up state
//							nIndeterimate		- bitmap resource id for indeterminate state
//							nDown					- bitmap resource id for down state (optional)
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RIndeterminateCheckBox::SetIndeterminateBitmap( int nUp, int nIndeterimate, int nDown  )
{
	// load the up-state bitmap
	BOOL bRet = m_UpBitmap.LoadMappedBitmap( nUp );
	ASSERT( bRet != FALSE );

	// load the indeterminate-state bitmap
	bRet = m_IndeterminateBitmap.LoadMappedBitmap( nIndeterimate );
	ASSERT( bRet != FALSE );

	// load the (optional) down-state bitmap
	if ( nDown != 0 )
		bRet = m_DownBitmap.LoadMappedBitmap( nDown );

	ASSERT( bRet != FALSE );

	// the default state is up so set that bitmap
	ASSERT_VALID( &m_UpBitmap );
	SetBitmap( m_UpBitmap );

	// subclass the CButton, so we can intercept the BM_SETCHECK messages
	m_OriginalWndProc = (WNDPROC) ::GetWindowLong( GetSafeHwnd(), GWL_WNDPROC );
	::SetWindowLong( GetSafeHwnd(), GWL_WNDPROC, (DWORD) IndeterminateWndProc );
}

// ****************************************************************************
//
//  Function Name:	RIndeterminateCheckBox::IndeterminateWndProc( HWND hWnd, 
//							UINT msg, WPARAM wParam, LPARAM lParam )
//
//  Description:		Subclassed WNDPROC for the CButton.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
LRESULT WINAPI RIndeterminateCheckBox::IndeterminateWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	ASSERT( ::IsWindow( hWnd ) );

	RIndeterminateCheckBox* pThis = (RIndeterminateCheckBox*) CWnd::FromHandlePermanent( hWnd );
	ASSERT_VALID( pThis );

	if ( hWnd == pThis->GetSafeHwnd() )
	{
		if( msg == BM_SETCHECK  )
		{
			int nOldState = pThis->m_nState;
			pThis->m_nState = wParam;

//			if ( nOldState == kIndeterminate &&  ( wParam == BST_INDETERMINATE ) )
			if ( wParam == BST_UNCHECKED )
			{
				// trying to go indeterminate when we are already indeterminate
				// modify the message so that we go UP, and switch the bitmap to up
				ASSERT_VALID( &(pThis->m_UpBitmap) );
				pThis->SetBitmap( (HBITMAP) pThis->m_UpBitmap );
				wParam = BST_UNCHECKED;
				TRACE("Up State\n");
			}
			else if (wParam == BST_INDETERMINATE )
			{
				// trying to go indeterminate - switch the bitmap
				ASSERT_VALID( &(pThis->m_IndeterminateBitmap) );
				pThis->SetBitmap( pThis->m_IndeterminateBitmap );
				wParam = BST_CHECKED;
				TRACE("Indeterminate State\n");
			}
			else
			{
				// down - use the down image if we have one.
				if ( pThis->m_DownBitmap.m_hObject != NULL )
				{
					ASSERT_VALID( &(pThis->m_DownBitmap) );
					pThis->SetBitmap( pThis->m_DownBitmap );
				}
				else
				{
					ASSERT_VALID( &(pThis->m_UpBitmap) );
					pThis->SetBitmap( pThis->m_UpBitmap );
				}

				TRACE("Down State\n");
			}
		}
		else if ( msg == BM_GETCHECK  )
		{
			// because we never let the button go into the indeterminate state
			// we have to return our own state variable.
			return pThis->m_nState;
		}
	}

	// defer all the processing to the original WNDPROC for the CButton
	return (*pThis->m_OriginalWndProc)(hWnd, msg, wParam, lParam );	
}


/////////////////////////////////////////////////////////////////////////////
// @func Assigns a COLORREF into an RGBQUAD pointer. Direct assignment
//       is impossible because the BYTE order in an RGBQUAD is BGR and
//       in a COLORREF is RGB. Each member must be assigned individually.
//
inline void SetDwordRGB(DWORD * pdw, COLORREF rgb)
{
    RGBQUAD * prgbq = (RGBQUAD *) pdw;
    prgbq->rgbBlue  = GetBValue(rgb);
    prgbq->rgbRed   = GetRValue(rgb);
    prgbq->rgbGreen = GetGValue(rgb);
    prgbq->rgbReserved = 0;
}

/////////////////////////////////////////////////////////////////////////////
// HBITMAP MagicLoad( LPCTSTR )
//
// @func This function accepts a LPCTSTR identifier to load a bitmap from
//       the current application. In order to munge the palette, the
//       LoadBitmap API can not be used. This function loads the resource
//       with the LoadResource API and locks the resource in memory. 
//       A RT_BITMAP locked from the resource file is accessible as a DIB
//       which has palette information available. A copy of the locked
//       DIB is made in the application's stack space and then the palette
//       is searched for the "magic" colors: <nl> <nl>
//       
//            RGB(192,192,192):  COLOR_BTNFACE <nl>
//            RGB( 0, 0, 0 ):    COLOR_BTNTEXT <nl>
//            RGB(255,255,255):  COLOR_BTNHIGHLIGHT <nl>
//            RGB(128,128,128):  COLOR_BTNSHADOW  <nl>
//
//        These colors are then substituted in the DIB's palette, 
//        a BITMAP object compatible with the system display which is
//        returned.
//
//        Note that this function only works for bitmap resources
//        with a color depth of 4 or 8 bpp.
//
HBITMAP MagicLoad( LPCTSTR nID )
{
	BITMAPINFOHEADER    * lpbih;    // pointer to original bitmap
	HRSRC                 hrsrc;    // handle returned from FindResource
	HGLOBAL               hglobal;  // handle returned from LoadResource
	BITMAPINFOHEADER    * newlpbih; // pointer to the bitmap copy

	if (nID == NULL) return NULL;

	hrsrc   = FindResource(AfxGetResourceHandle(), nID, RT_BITMAP);
	if (hrsrc == NULL) return NULL;

	hglobal = LoadResource(AfxGetResourceHandle(), hrsrc );
	if (hglobal == NULL) return NULL;

	lpbih   = (LPBITMAPINFOHEADER) LockResource( hglobal );
	if (lpbih == NULL)
	{
	  FreeResource(hglobal);
	  return NULL;
	}

	int nPalSize(0);

	if (lpbih->biBitCount == 8)
	  nPalSize = (lpbih->biClrUsed == 0) ? 256 : lpbih->biClrUsed;
	else if (lpbih->biBitCount == 4)
	  nPalSize = (lpbih->biClrUsed == 0) ? 16  : lpbih->biClrUsed;

	// Calculate the original bitmap's width and height.
	// Note that width must be a multiple of 4 for memory
	// allocation purposes.
	int width   = lpbih->biWidth;
	while (width % 4 != 0) width++;
	int height  = lpbih->biHeight;

	// Calculate the memory needed for a temporary copy of the
	// bitmap resource for palette corrections
	int biSize  = width * height 
				 + sizeof(BITMAPINFOHEADER) 
				 + sizeof(DWORD) * nPalSize;

	// Allocate the memory using GlocalAlloc(). This is a very
	// inelegant way to allocate memory for a temporary copy of the
	// bitmap resource, but the Accusoft function IMG_create_handle
	// requires global memory. This will likely be fixed in v 6.0
	// of the Accusoft Libraries which will require adjustments
	// to this code.
	//
	newlpbih = (BITMAPINFOHEADER *) malloc( biSize );
	memcpy( newlpbih, lpbih, biSize );

	DWORD * pRGB = (DWORD *) ((BYTE *)newlpbih + sizeof(BITMAPINFOHEADER));

	// Spin through the palette for the loaded bitmap resource and 
	// make substitutions for "magic colors"
	for( int i = 0; i < nPalSize; i++ )
	{
		switch (*(pRGB+i))
		{
		case RGB(192,192,192):
			SetDwordRGB( pRGB+i, GetSysColor(COLOR_BTNFACE));
			break;
		case RGB( 0, 0, 0 ):
			SetDwordRGB( pRGB+i, GetSysColor(COLOR_BTNTEXT));
			break;
		case RGB(255,255,255):
			SetDwordRGB( pRGB+i, GetSysColor(COLOR_BTNHIGHLIGHT));
			break;
		case RGB(128,128,128):
			SetDwordRGB( pRGB+i, GetSysColor(COLOR_BTNSHADOW));
			break;
		case RGB(128, 0, 0):
			SetDwordRGB( pRGB+i, GetSysColor(COLOR_ACTIVECAPTION));
			break;
		}
	}

	TpsAssert( newlpbih->biBitCount <= 8, "Bitmap more then 256 colors" );
	BYTE *lpbits = (BYTE *) newlpbih + sizeof(BITMAPINFOHEADER) + 
		nPalSize *sizeof(RGBQUAD);


	// Get a compatible DC to create an HBITMAP
	HWND    hWnd = GetDesktopWindow();
	HDC     hdcDeskTop = GetDC(hWnd);
	HBITMAP hBitmap;

	HPALETTE	hPalette = (HPALETTE) tbitGetScreenPalette();
	HPALETTE	hPalOld	= ::SelectPalette( hdcDeskTop, hPalette, FALSE );
	RealizePalette( hdcDeskTop );

	// Create the HBITMAP
	hBitmap = CreateCompatibleBitmap( hdcDeskTop, 
											  newlpbih->biWidth, 
											  newlpbih->biHeight );

	// Populate the HBITMAP with the bits from the "new" DIB
	SetDIBits(  hdcDeskTop,
		hBitmap,
		0,
		newlpbih->biHeight,
		lpbits,
		(BITMAPINFO *)  newlpbih,
      DIB_RGB_COLORS );


    // Clean up allocated resources
	SelectPalette( hdcDeskTop, hPalOld, FALSE );
   ReleaseDC( hWnd, hdcDeskTop );
   FreeResource( hglobal );

   return hBitmap;
}
