//****************************************************************************
//
// File Name:  HeadlineDlg.cpp
//
// Project:    Renaissance headline user interface
//
// Author:     Lance Wilson
//
// Description: Defines classes RHeadlineDlg and RHeadlinePreview
//				    which are used to provide an interface to the Headline 
//				    UI.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software, Inc.
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/HeadlineDlg.cpp                                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"

ASSERTNAME

#ifndef	_WINDOWS
	#error	This file must be compilied only on Windows
#endif	//	_WINDOWS

#include "HeadlineDlg.h"
#include "HeadlineInterface.h"

#include "ApplicationGlobals.h"
#include "DcDrawingSurface.h"
#include "Storage.h"
#include "BitmapLoad.h"

#include "ComponentTypes.h"
#include "ComponentAttributes.h"
#include "ComponentManager.h"
#include "ComponentDocument.h"
#include "ComponentView.h"

#include "SoftShadowSettings.h"
#include "SoftGlowSettings.h"

const int  kElapsedTimeBeforeUpdate  = 1000 ;		// 1 - Second
const UINT kTimerOne						 =	 100 ;
const int  kMaxLineLength            =  100 ;

const int  kDefaultHeadlineHeight    = 500 ;
const int  kDefaultHeadlineWidth     = 1000 ;
const int  kMaxTabIndex              = 5 ;

const YFloatType kHeadlineAspectRatio = 0.37 ;

// TODO: change to configuration setting
static BOOLEAN fSoftEffectsSupported = FALSE;
static BOOLEAN fShadowTabSupported   = FALSE;

static CMap<WORD, WORD, WORD, WORD> cExclusionMap;

/////////////////////////////////////////////////////////////////////////////
// RHeadlineEdit
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(RHeadlineEdit, CEdit)
	//{{AFX_MSG_MAP(RHeadlineEdit)
	ON_WM_CHAR()
	ON_MESSAGE( WM_PASTE, OnPaste )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: RHeadlineEdit::OnChar
//
// Description:   WM_CHAR message handler
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineEdit::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
	// Is control key is down and nChar a printable character?
	if (VK_RETURN == nChar || '\n' == nChar)
	{
		if (GetLineCount() >= m_nLineLimit)
		{
			// No more lines allowed
			MessageBeep( 0xFFFFFFFF ) ;
			return ;
		}
	}
	else if (nChar >= ' ' && GetAsyncKeyState( VK_CONTROL ) >= 0)		
	{
		int nCurLineLength = LineLength() ;

		if (nCurLineLength + nRepCnt > kMaxLineLength)
		{
			// Determine a new rep count
			nRepCnt = kMaxLineLength - nCurLineLength ;
			
			if (!nRepCnt)
			{
				// No more characters allowed
				MessageBeep( 0xFFFFFFFF ) ;
				return ;
			}
		}
	}
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

//****************************************************************************
//
// Function Name: RHeadlineEdit::OnPaste
//
// Description:   WM_PASTE message handler
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
LRESULT RHeadlineEdit::OnPaste( WPARAM wParam, LPARAM lParam )
{
	uWORD uwCharsToAdd = 0 ;
	uWORD uwLinesToAdd = 0 ;

	if (OpenClipboard())
	{
		HANDLE hText = ::GetClipboardData( CF_TEXT ) ;

		if (hText)
		{
			// Determine the number of characters to be added.
			char *pszText = (char *) GlobalLock( hText ) ;
			uwCharsToAdd  = lstrlen( pszText ) ;

			// Determine the number lines to be added
			for (int i = 0; i < uwCharsToAdd; i++)
			{
				if ('\n' == pszText[i])
				{
					uwLinesToAdd++ ;
				}
			}

			GlobalUnlock( hText ) ;
		}
		
		CloseClipboard() ;
	}

	// See if there is enough room for the complete text.  Note, 
	// LineLength() subtracts for currently hilited text that will
	// be replaced on pasting, so we don't need to.
	if (LineLength() + uwCharsToAdd > kMaxLineLength || GetLineCount() + uwLinesToAdd > m_nLineLimit)
	{
		MessageBeep( 0xFFFFFFFF ) ;
		return 0L ;
	}

	return CEdit::DefWindowProc( WM_PASTE, wParam, lParam ) ;
}

//****************************************************************************
//
// Function Name: RHeadlineEdit::SetLineLimit
//
// Description:   Sets the maximum number of lines allowed in the edit control
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineEdit::SetLineLimit( int nNumLines )
{
	m_nLineLimit = nNumLines ;
}


/////////////////////////////////////////////////////////////////////////////
// RHeadlineReadyMades dialog
/////////////////////////////////////////////////////////////////////////////

//****************************************************************************
//
// Function Name: RHeadlineReadyMades::RHeadlineReadyMades
//
// Description:   Constructor 
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
RHeadlineReadyMades::RHeadlineReadyMades() :
	RGridCtrl<UINT>( kPushLike | k3dStyle | kIntegral )
{
	m_nFirstVertical = -1;
}

//****************************************************************************
//
// Function Name: RHeadlineReadyMades::~RHeadlineReadyMades
//
// Description:   Destructor 
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
RHeadlineReadyMades::~RHeadlineReadyMades()
{
}

BOOL CALLBACK EnumReadyMadeProc( HANDLE, LPCTSTR, LPTSTR lpszName, LONG lParam )
{
	RHeadlineReadyMades* pList = (RHeadlineReadyMades*) lParam;
	TpsAssert((int) lpszName < 0xFFFF, "Invalid Ready-Made Resource!");

	WORD wValue = 0;
	if (!cExclusionMap.Lookup( (UINT) lpszName, wValue ))
		pList->AddItem( (int) lpszName );

	return TRUE;
}
  
//****************************************************************************
//
// Function Name: RHeadlineReadyMades::Initialize
//
// Description:   Loads the ReadyMade names into the control 
//
// Returns:       TRUE if successful; otherwise FALSE. 
//
// Exceptions:	   None
//
//****************************************************************************
BOOLEAN RHeadlineReadyMades::Initialize( BOOLEAN fHorz, BOOLEAN fVert )
{
	SetWindowLong( m_hWnd, GWL_STYLE, GetStyle() & ~LBS_SORT ) ;

	if (!fSoftEffectsSupported && cExclusionMap.IsEmpty())
	{
		//
		// Build the exclusion map from the soft effect exclusion index
		//

		HMODULE hModule = AfxGetResourceHandle();
		HRSRC hrsrc = FindResource( hModule, MAKEINTRESOURCE( ID_SOFTEFFECT_EXCLUSIONS ), _T("HEADLINE_INDEX") ) ;

		if (hrsrc)
		{
			HGLOBAL hData = LoadResource( AfxGetResourceHandle(), hrsrc ) ;

			if (hData)
			{	
				WORD* data = (WORD*) LockResource( hData );

				while (*data)
				{
					cExclusionMap.SetAt( *data, TRUE );
					data++;
				}

				UnlockResource( hData );
			}

			FreeResource( hData );
		}
		
	}

	if (fHorz)
	{
		EnumResourceNames( 
			AfxGetResourceHandle(),						// resource-module handling 
			_T("HEADLINE"),								// pointer to resource type 
			(ENUMRESNAMEPROC) EnumReadyMadeProc,	// pointer to callback function 
			(LONG) this );									// application-defined parameter 
	}
 
	m_nFirstVertical = GetCount();

	if (fVert)
	{
		EnumResourceNames( 
			AfxGetResourceHandle(),						// resource-module handling 
			_T("VERT_HEADLINE"),							// pointer to resource type 
			(ENUMRESNAMEPROC) EnumReadyMadeProc,	// pointer to callback function 
			(LONG) this );									// application-defined parameter 
	}

	return TRUE ;
}

//****************************************************************************
//
// Function Name: RHeadlineReadyMades::DrawItem
//
// Description:   Draws the specified bitmap, in the specified
//                state, in the specified area.
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineReadyMades::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	UINT itemData ;
	GetItemData( lpDrawItemStruct->itemID, itemData ) ;

	BITMAP bm ;
	HBITMAP hBitmap = ::Load256Bitmap( itemData ) ;
	GetObject( hBitmap, sizeof( bm ), &bm ) ;

	CRect rcCellRect( lpDrawItemStruct->rcItem ) ;
	RIntRect rcDestRect( 0, 0, bm.bmWidth - 1, bm.bmHeight - 1 ) ;
	rcDestRect.CenterRectInRect( RIntRect( rcCellRect ) ) ;

	CDC dc ;
	dc.Attach( lpDrawItemStruct->hDC ) ;

	CDC dcMem ;
	dcMem.CreateCompatibleDC( &dc ) ;
	HBITMAP hOldBitmap = (HBITMAP) dcMem.SelectObject( hBitmap ) ;
	
	dc.BitBlt( rcDestRect.m_Left, rcDestRect.m_Top, bm.bmWidth, bm.bmHeight,
		&dcMem, 0, 0, SRCCOPY ) ;
	
	rcCellRect = rcDestRect ;
	rcCellRect.InflateRect( 1, 1 ) ;
	dc.Draw3dRect( rcCellRect, GetSysColor( COLOR_BTNSHADOW ),
		GetSysColor( COLOR_BTNHILIGHT ) ) ;

	// Clean up.
	dcMem.SelectObject( hOldBitmap ) ;

	if (lpDrawItemStruct->itemState   & ODS_FOCUS)
	{
		rcCellRect.DeflateRect( 2, 2 ) ;
		dc.DrawFocusRect( rcCellRect ) ;
	}

	dc.Detach() ;
	DeleteObject( hBitmap );
}

//****************************************************************************
//
// Function Name: RHeadlineReadyMades::MeasureItem
//
// Description:   Determines the size of the specified item.
//
// Returns:       Nothing
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineReadyMades::MeasureItem( LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/ )
{
}

//****************************************************************************
//
// Function Name: RHeadlineReadyMades::LoadReadyMade
//
// Description:   Loads the ready made data from the resource table.
//
// Returns:       Pointer to a data holder that must be deleted by
//                the caller.
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineReadyMades::LoadReadyMade( int nSel, RHeadlineInterface*	pInterface, RComponentView* pView )
{
	TpsAssert( nSel > LB_ERR, "Invalid index into ReadyMade collection!" );
	TpsAssert( pInterface, "Ivalid headline interface!" );

	CString strResType = (nSel < m_nFirstVertical ? 
		_T( "HEADLINE" ) : _T( "VERT_HEADLINE" ));

	UINT itemData ;
	GetItemData( nSel, itemData );

	HMODULE hModule = AfxGetResourceHandle();
	HRSRC hrsrc = FindResource( hModule, MAKEINTRESOURCE( itemData ), strResType ) ;

	if (hrsrc)
	{
		HGLOBAL hHeadline = LoadResource( AfxGetResourceHandle(), hrsrc ) ;

		try
		{
			LPUBYTE data = (LPUBYTE) LockResource( hHeadline ) ;

			int nLen = 0;
			
			if (data[1] != '\0')
				nLen = lstrlen( (char *) data ) + 1;

			RBufferStream bufferStream( data + nLen, SizeofResource( hModule, hrsrc) - nLen ) ;
			RStorage      rStorage( &bufferStream ) ;

			RHeadlineHolder* pData = (RHeadlineHolder *) pInterface->GetData() ; 
			pData->Read( rStorage ) ;

			pInterface->SetData( pData ) ;
			delete pData ;

			RSoftShadowSettings shadowSettings;
			RSoftGlowSettings glowSettings;

			try
			{
				rStorage >> shadowSettings;
				rStorage >> glowSettings;
			}
			catch (...)
			{
				// No soft effects serialized, so use default 
				// values making sure effects are turned off.
				shadowSettings.m_fShadowOn = FALSE;
				glowSettings.m_fGlowOn = FALSE;
			}

			if (!fSoftEffectsSupported)
			{
				if (shadowSettings.m_fShadowOn)
				{
					shadowSettings.m_fShadowOpacity = 1.0;
					shadowSettings.m_fShadowEdgeSoftness = 0.0;
				}
			}

			pView->SetShadowSettings( shadowSettings );
			pView->SetGlowSettings( glowSettings );

		}
		catch (...)
		{
			UnlockResource( hHeadline ) ;
			FreeResource( hHeadline ) ;

			throw ;
		}

		UnlockResource( hHeadline ) ;
		FreeResource( hHeadline ) ;
	}
}

void RHeadlineReadyMades::SelectReadyMade( CString& strID )
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (strID == GetID( i ))
		{
			SetCurSel( i );
			break;
		}
	}
}

CString RHeadlineReadyMades::GetID( int nIndex )
{
	TpsAssert( nIndex > LB_ERR, "Invalid index into ReadyMade collection!" );

	CString strResult;
	CString strResType = (nIndex < m_nFirstVertical ? 
		_T( "HEADLINE" ) : _T( "VERT_HEADLINE" ));

	UINT itemData ;
	GetItemData( nIndex, itemData );

	HMODULE hModule = AfxGetResourceHandle();
	HRSRC hrsrc = FindResource( hModule, MAKEINTRESOURCE( itemData ), strResType ) ;

	if (hrsrc)
	{
		HGLOBAL hHeadline = LoadResource( AfxGetResourceHandle(), hrsrc ) ;
		const char* data = (const char*) LockResource( hHeadline ) ;

		if (hHeadline)
		{
			if (data[1] != '\0')
				strResult = data;
			else
				strResult.Format( "HEADLINE_READYMADE%03d", itemData );

			UnlockResource( hHeadline ) ;
			FreeResource( hHeadline ) ;
		}
	}

	return strResult;
}

/////////////////////////////////////////////////////////////////////////////
// RHeadlineDlg dialog
/////////////////////////////////////////////////////////////////////////////

//****************************************************************************
//
// Function Name: RHeadlineDlg::RHeadlineDlg
//
// Description:   Constructor that accepts a pointer to a RComponentView	
//                interface object for use in dialog initialization 
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
RHeadlineDlg::RHeadlineDlg( RComponentDocument* pEditedComponent )
	: CDialog(RHeadlineDlg::IDD, AfxGetMainWnd()),
	  m_pEditedComponent( pEditedComponent )
{
	//{{AFX_DATA_INIT(RHeadlineDlg)
	m_nMode = kReadyMade ;
	//}}AFX_DATA_INIT

	m_pCurrentPage = NULL ;
	m_uiTimer = 0 ;
	
	m_fMultiLine = FALSE ;
	m_fVertText  = FALSE ;

	m_pControlView       = NULL ;
	m_pControlDocument   = NULL ;
	m_pComponentDocument = NULL ;
	m_pInterface         = NULL ;
	m_pInitialData       = NULL ;

	m_rContextData.m_wMode  = (sWORD) kReadyMade;
	m_rContextData.m_wIndex = (sWORD) 0;
	m_rContextData.m_strReadyMade = "";

	try
	{
		// Obtain the initial headline data from the component passed in.
		if( pEditedComponent )
		{
			YComponentType componentType = pEditedComponent->GetComponentType( );
			m_pageShape.SetComponentType( componentType ) ;

			RHeadlineInterface* pInterface = dynamic_cast<RHeadlineInterface*>(
				static_cast<RComponentView*>( pEditedComponent->GetActiveView( ) )->GetInterface( kHeadlineInterfaceId )) ;

			m_pInitialData = (RHeadlineHolder *) pInterface->GetData() ;
			delete pInterface ;
		}
	}
	catch (...)
	{
	}
}

//****************************************************************************
//
// Function Name: RHeadlineDlg::~RHeadlineDlg
//
// Description:   Destructor 
//
// Returns:       Nothing 
//
// Exceptions:	  None
//
//****************************************************************************
RHeadlineDlg::~RHeadlineDlg()
{
	if (m_pControlDocument)
	{
		delete m_pControlDocument ;
	}

	if (m_pInterface)
	{
		delete m_pInterface ;
	}

	if (m_pInitialData)
	{
		delete m_pInitialData ;
	}
}

//****************************************************************************
// Function Name: RHeadlineDlg::DoDataExchange
//
// Description:   Handles the data exchange to and from the dialogs 	
//                child controls 
//
// Returns:       Nothing 
//
// Exceptions:	  None
//
//****************************************************************************
void RHeadlineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(RHeadlineDlg)
	DDX_Control (pDX, IDC_STATIC_TEXT, m_ctlStaticText);
	DDX_Control (pDX, IDC_HEADLINE_TEXT, m_ecHeadlineText) ;
	DDX_Text(pDX, IDC_HEADLINE_TEXT, m_strHeadlineText);
	DDX_Control (pDX, IDC_TABS, m_ctlTabCtrl) ;
	DDX_Control( pDX, IDC_READYMADES, m_gcImageList );
	DDX_Radio( pDX, IDC_READYMADE, m_nMode ) ;
	//}}AFX_DATA_MAP
}

//****************************************************************************
//
// Function Name: RHeadlineDlg::InitControlData
//
// Description:   Transfers the data from the HeadlineDataStruct 
//						data member m_HlData into the individual data
//						members used for child control data transfer.
//
// Returns:       Nothing 
//
// Exceptions:		None
//
//****************************************************************************
void RHeadlineDlg::InitControlData ()
{
	RMBCString strHeadlineText ;
	m_pInterface->GetHeadlineText( strHeadlineText ) ;
	m_strHeadlineText = strHeadlineText.operator CString( ) ;
}

BEGIN_MESSAGE_MAP(RHeadlineDlg, CDialog)
	//{{AFX_MSG_MAP(RHeadlineDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABS, OnSelChangeTabs)
	ON_EN_CHANGE(IDC_HEADLINE_TEXT, OnChangeHeadlineText)
	ON_EN_KILLFOCUS(IDC_HEADLINE_TEXT, OnKillfocusHeadlineText)
	ON_MESSAGE(UM_HEADLINE_FONT_CHANGE, OnFontChange)
	ON_MESSAGE(UM_HEADLINE_FACE_CHANGE, OnFaceChange)
	ON_MESSAGE(UM_HEADLINE_SHAPE_CHANGE, OnShapeChange)
	ON_MESSAGE(UM_HEADLINE_POSITION_CHANGE, OnPositionChange)
	ON_MESSAGE(UM_HEADLINE_OUTLINE_CHANGE, OnOutlineChange)
	ON_MESSAGE(UM_HEADLINE_SHADOW_CHANGE, OnShadowChange)
	ON_MESSAGE(UM_HEADLINE_DEPTH_CHANGE, OnDepthChange)
	ON_MESSAGE(UM_HEADLINE_PROPORTION_CHANGE, OnProportionChange)
	ON_WM_MENUCHAR( )
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_BN_CLICKED(IDC_READYMADE, OnReadyMade)
	ON_BN_CLICKED(CONTROL_CUSTOMIZE, OnCustomize)
	ON_LBN_SELCHANGE( IDC_READYMADES, OnSelChangeImageList )	
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP

#ifdef TPSDEBUG
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveAs)
#endif

END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: RHeadlineDlg::CreateComponent
//
// Description:   Creates a component from the headline data
//
// Returns:       RComponentDocument*
//
// Exceptions:	   None
//
//****************************************************************************
RComponentDocument* RHeadlineDlg::CreateNewComponent( RDocument* pParentDocument )
{
	RComponentDocument* pComponentDocument = NULL ;

	//
	// Get the type of component to create
	//
	ETextEscapement eEscapement ;
	m_pInterface->GetEscapement( eEscapement ) ;
	YComponentType componentType = (eEscapement < kTop2BottomLeft2Right ?
		kHeadlineComponent : kVerticalHeadlineComponent) ;

	if( m_pEditedComponent)
	{
		YComponentType curComponentType = m_pEditedComponent->GetComponentType( );

		if (kSpecialHeadlineComponent         == curComponentType ||
			 kVerticalSpecialHeadlineComponent == curComponentType)
		{
			componentType = curComponentType ;
		}
	}

	try
	{
		// Default to placeholder size
		RRealSize size( kDefaultHeadlineWidth, kDefaultHeadlineHeight ) ;
		
		if (pParentDocument)
		{ 
			// Get the default object size from the parent
			size = pParentDocument->GetDefaultObjectSize( componentType ) ;
		}

		// Note: to keep the code generic, the caller
		// will have to set specific component attributes.
		RComponentAttributes attr ;

		// Create a new component. 
		pComponentDocument = ::GetComponentManager( ).CreateNewComponent( 
			componentType, 
			pParentDocument, 
			NULL, 
			attr,
			size,
			FALSE ) ;

		RComponentView* pComponentView = (RComponentView*) pComponentDocument->GetActiveView() ;

		if( m_pEditedComponent)
			pComponentView->CopyViewAttributes( (RComponentView *)
				m_pEditedComponent->GetActiveView() );

		RSoftShadowSettings settings;
		m_pComponentView->GetShadowSettings( settings );
		pComponentView->SetShadowSettings( settings );

		RSoftGlowSettings rGlowSettings;
		m_pComponentView->GetGlowSettings( rGlowSettings );
		pComponentView->SetGlowSettings( rGlowSettings );

		// Obtain the headline data 
		RHeadlineInterface* pInterface = dynamic_cast<RHeadlineInterface*>(
			pComponentView->GetInterface( kHeadlineInterfaceId )) ;

		RComponentDataHolder *pData = m_pInterface->GetData() ;
		pInterface->SetData( pData ) ;


		// Add in persistent data
		//

		// Get the UI Persistant data holder
		RUIContextData& contextData = pComponentDocument->GetUIContextData( );

		// Write the headline persistant object
		m_rContextData.Write( contextData );

		delete pInterface ;
		delete pData ;
	}
	catch (...)
	{
		if (pComponentDocument)
		{
			delete pComponentDocument ;
			return NULL ;
		}
	}

	return pComponentDocument ;
}

//*****************************************************************************
//
// Function Name:  RHeadlineDlg::ShowPage
//
// Description:    Helper function for creating/displaying the correct
//				   property sheet in the tab control.
//
// Returns:        VOID
//
// Exceptions:	   None
//
//*****************************************************************************
void RHeadlineDlg::ShowPage(int nPage)
{
	CDialog* pageArray[] = 
	{ 
		&m_pageFace,
		&m_pageShape,
		&m_pagePosition, 
		&m_pageOutline,
		&m_pageShadow,
		&m_pageDepth,
		&m_pageProportion
	} ;

	int idArray[] =
	{
		m_pageFace.IDD,
		m_pageShape.IDD,
		m_pagePosition.IDD, 
		m_pageOutline.IDD,
		m_pageShadow.IDD,
		m_pageDepth.IDD,
		m_pageProportion.IDD
	} ;

	if (!fShadowTabSupported)
	{
		pageArray[4] = pageArray[5];
		pageArray[5] = pageArray[6];

		idArray[4] = idArray[5];
		idArray[5] = idArray[6];
	}

	// Save a pointer to the current page,
	// so if the creation process fails,
	// we can reset to the previous page.
	CDialog* pPrevPage = m_pCurrentPage ;

	if (m_pCurrentPage != pageArray[nPage] /*&& m_ctlTabCtrl.IsWindowVisible()*/)
	{

		if (m_pCurrentPage && IsWindow (m_pCurrentPage->m_hWnd))
			m_pCurrentPage->ShowWindow(SW_HIDE) ;

		CRect   rectAdjust(0,0,0,0);
		CRect   rectWindow(0,0,0,0);

		m_ctlTabCtrl.AdjustRect( TRUE, &rectAdjust );
		m_ctlTabCtrl.GetWindowRect( &rectWindow );

		rectWindow.left -= rectAdjust.left;
		rectWindow.top  -= rectAdjust.top;
		ScreenToClient( &rectWindow );

		m_pCurrentPage = pageArray[nPage] ;

		if (!IsWindow (m_pCurrentPage->m_hWnd))
		{
			if (!m_pCurrentPage->Create(idArray[nPage], this))
			{
				// Let the user know there is a reason 
				// they're staring at a blank page.
				RAlert rAlert ;
				rAlert.AlertUser( STRING_ERROR_ACC_OUT_OF_MEMORY ) ;
				if (!pPrevPage) return ;

				// Restore the current page pointer to the previous one.
				m_pCurrentPage = pPrevPage ;

				// Find the index of the previous page. (For setting the tab)
				for (int i = 0; i < m_ctlTabCtrl.GetItemCount(); i++)
				{
					if (m_pCurrentPage == pageArray[nPage])
					{
						m_ctlTabCtrl.SetCurSel( i ) ;
						break ;
					}
				}
			}
			
			m_pCurrentPage->SetWindowPos (&(m_pControlView->GetCWnd()), rectWindow.left,
				rectWindow.top, 0, 0, SWP_NOSIZE) ;
		}
	}

	if (kCustomize == m_nMode)
	{
		m_pCurrentPage->ShowWindow (SW_SHOW) ;
	}
}

//****************************************************************************
//
// Function Name: RHeadlineDlg::OnInitDialog
//
// Description:   Handles dialog initialization
//
// Returns:       TRUE
//
// Exceptions:	  None
//
//****************************************************************************
BOOL RHeadlineDlg::OnInitDialog() 
{
	RWaitCursor waitCursor;
	CDialog::OnInitDialog();

	InitializeDocument() ;

	// Add in the tabs
	TC_ITEM tabItem;
	tabItem.iImage  = -1;
	tabItem.mask    = TCIF_TEXT;

	CString strTabText ;
	int nItem = 0;
	
	try
	{
		strTabText.LoadString( STRING_HEADLINE_FACE_TAB ) ;
		tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
		m_ctlTabCtrl.InsertItem (nItem++, &tabItem) ;

		strTabText.LoadString( STRING_HEADLINE_SHAPE_TAB ) ;
		tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
		m_ctlTabCtrl.InsertItem (nItem++, &tabItem) ;

		strTabText.LoadString( STRING_HEADLINE_POSITION_TAB ) ;
		tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
		m_ctlTabCtrl.InsertItem (nItem++, &tabItem) ;

		strTabText.LoadString( STRING_HEADLINE_OUTLINE_TAB ) ;
		tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
		m_ctlTabCtrl.InsertItem (nItem++, &tabItem) ;

		if (fShadowTabSupported)
		{
			strTabText.LoadString( STRING_HEADLINE_SHADOW_TAB ) ;
			tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
			m_ctlTabCtrl.InsertItem (nItem++, &tabItem) ;
		}

		strTabText.LoadString( STRING_HEADLINE_DEPTH_TAB ) ;
		tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
		m_ctlTabCtrl.InsertItem (nItem++, &tabItem) ;

		strTabText.LoadString( STRING_HEADLINE_PROPORTION_TAB ) ;
		tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
		m_ctlTabCtrl.InsertItem (nItem++, &tabItem) ;
	}

	catch (CMemoryException& e)
	{
		e.ReportError() ;
		EndDialog( IDCANCEL ) ;

		return TRUE ;
	}

	try
	{
		CRect rect ;
		m_ctlTabCtrl.GetWindowRect( rect ) ;
		m_ctlTabCtrl.SetItemSize( CSize( (rect.Width() - 2) / nItem, 0 ) ) ;

		//
		// Create and position the font child dialog that 
		// contains the font information controls.
		//
		if (!m_ctlFontDlg.Create( this ))
		{
			throw kResource ;
		}

		m_ctlStaticText.GetWindowRect( rect ) ;
		ScreenToClient( rect ) ;

		m_ctlFontDlg.SetWindowPos( &m_ctlStaticText, rect.left, rect.top, 
			0, 0,	SWP_NOSIZE ) ;

		YComponentType componentType = m_pEditedComponent ? 
			m_pEditedComponent->GetComponentType() : kHeadlineComponent;

		BOOL fLoadVert = (componentType != kSpecialHeadlineComponent);
		BOOL fLoadHorz = (componentType != kVerticalSpecialHeadlineComponent);
		m_gcImageList.SizeCellsToClient( 1, 4 );
		m_gcImageList.Initialize( fLoadHorz, fLoadVert );

		// Update the pages with the
		// current data settings
		ApplyData() ;

		//
		// Set the line limit in the
		// headline edit control
		//
		ETextEscapement eEscapement ;
		m_pInterface->GetEscapement( eEscapement ) ;
		int nLineLimit = (eEscapement < kTop2BottomLeft2Right ? 3 : 1) ;
		m_ecHeadlineText.SetLineLimit( nLineLimit ) ;

		if (m_pEditedComponent)
		{
			InitializeUIFromComponent( m_pEditedComponent );
		}

	#ifdef TPSDEBUG
		CWnd* pWnd = GetDlgItem( IDC_SAVEAS ) ;
		if (pWnd) pWnd->ShowWindow( SW_SHOW ) ;
	#endif

		// Last but not lease, update the preview
		// so the the correct view size is used.
		UpdatePreview() ;
	}
	catch (...)
	{
		// Let the user know there is a reason 
		// the dialog didn't come it.
		RAlert rAlert ;
		rAlert.AlertUser( STRING_ERROR_ACC_OUT_OF_MEMORY ) ;
		EndDialog( IDCANCEL ) ;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//*****************************************************************************
//
// Function Name:  RHeadlineDlg::InitializeDocument
//
// Description:    Initializes the headline document for the preview,
//                 as well obtains an interface pointer to the document
//                 for use throughout the dialog.
//
// Returns:        VOID
//
// Exceptions:	    kUnknownError
//
//*****************************************************************************
void RHeadlineDlg::InitializeDocument()
{
	try
	{
		//
		// Create our document...
		m_pControlDocument	= new RHeadlinePreviewDocument();


		//
		// Create a view for the document...
		m_pControlView = m_pControlDocument->CreateView( this, CONTROL_PREVIEW ) ;
		m_pControlDocument->AddRView( m_pControlView );

		RRealRect rectArea = m_pControlView->GetUseableArea() ;
		RRealRect rect(RRealSize( rectArea.Width(), rectArea.Width() * kHeadlineAspectRatio )) ;
		rect.CenterRectInRect( rectArea ) ;

		RComponentAttributes attr ;
		attr.SetSelectable( FALSE ) ;

		m_pComponentDocument = ::GetComponentManager().CreateNewComponent( 
			kHeadlineComponent, m_pControlDocument, m_pControlView, 
			attr, rect, FALSE ) ;

		// Obtain the headline interface, as this will be used
		// throughout the lifetime of the dialog.
		m_pComponentView = dynamic_cast<RComponentView*>(
			m_pComponentDocument->GetActiveView()) ;
		m_pInterface = dynamic_cast<RHeadlineInterface*>(
			m_pComponentView->GetInterface( kHeadlineInterfaceId )) ;

		if (m_pEditedComponent)
		{
			RComponentView* pEditedView = (RComponentView *)
				m_pEditedComponent->GetActiveView();

//			RSoftShadowSettings settings;
//			pEditedView->GetShadowSettings( settings );
//			m_pComponentView->SetShadowSettings( settings );

//			RSoftGlowSettings rGlowSettings;
//			pEditedView->GetGlowSettings( rGlowSettings );
//			m_pComponentView->SetGlowSettings( rGlowSettings );

			m_pComponentView->CopyViewAttributes( pEditedView );
	
			rect = RRealRect( pEditedView->GetSize( ) );
			rect.ShrinkToFit( rectArea );
			rect.CenterRectInRect( rectArea ) ;
			m_pComponentView->SetBoundingRect( rect );
		}

		if (!m_pInterface) throw kUnknownError ;

		if (m_pInitialData)
		{
			m_pInterface->SetData( m_pInitialData ) ;
			delete m_pInitialData ;
			m_pInitialData = NULL ;
		}

		m_pControlDocument->AddComponent( m_pComponentDocument );
	}
	catch (...)
	{
		throw kUnknownError ;
	}
}

//*****************************************************************************
//
// Function Name:  RHeadlineDlg::InitializeUIFromComponent
//
// Description:    Initializes the headline interface from the 
//                 UI persistent data maintained in a component.
//
// Returns:        VOID
//
// Exceptions:	    None
//
//*****************************************************************************
void RHeadlineDlg::InitializeUIFromComponent( RComponentDocument* pEditedComponent )
{
	CString strReadyMade;

	try
	{
		// Get the UI Persistant data
		RUIContextData& contextData = pEditedComponent->GetUIContextData( );
		m_rContextData.Read( contextData );

		strReadyMade = m_rContextData.m_strReadyMade.operator CString() ;
		
		if (m_rContextData.m_wIndex > kMaxTabIndex || kReadyMade == m_rContextData.m_wMode)
			m_rContextData.m_wIndex = 0;  // Safety check
	}
	catch (YException&)
	{
		//
		// For backwards compatiblity, we need to handle
		// the pre 6.0 method of storing user interface
		// data.
		//
		uLONG ulInterfaceData ;
		m_pInterface->GetInterfaceData( ulInterfaceData ) ;
		
		m_rContextData.m_wMode  = HIWORD( ulInterfaceData );
		m_rContextData.m_wIndex = LOWORD( ulInterfaceData );
		strReadyMade.Format( "%s%03d", _T("HEADLINE_READYMADE"), m_rContextData.m_wIndex );

		if (kReadyMade == m_rContextData.m_wMode)
		{
			// In previous versions the index field
			// was used to index into the ready made
			// gallery when the mode was kReadyMade.
			// Now we are using it solely for tab index
			m_rContextData.m_wIndex = 0;  
		}
	}

	m_ctlTabCtrl.SetCurSel( m_rContextData.m_wIndex ) ;

	if (kCustomize != m_rContextData.m_wMode && !strReadyMade.IsEmpty())
	{
		m_gcImageList.SelectReadyMade( strReadyMade );

		if (LB_ERR != m_gcImageList.GetCurSel())
			return;
	}

	CheckRadioButton( IDC_READYMADE, CONTROL_CUSTOMIZE, CONTROL_CUSTOMIZE ) ;
	OnCustomize() ;

	ETextEscapement eEscapement ;
	m_pInterface->GetEscapement( eEscapement ) ;

	if (eEscapement >= kTop2BottomLeft2Right)
	{
		m_gcImageList.SetTopIndexToFirstVertical();
	}
}

//*****************************************************************************
//
// Function Name:  RHeadlineDlg::OnCustomize
//
// Description:    BN_CLICKED handler for the customize radio button
//
// Returns:        VOID
//
// Exceptions:	   None
//
//*****************************************************************************
void RHeadlineDlg::OnCustomize() 
{
	if (kCustomize != m_nMode)
	{
		m_nMode = kCustomize ;

		m_gcImageList.ShowWindow( SW_HIDE ) ;
		m_ctlStaticText.ShowWindow( SW_HIDE ) ;
		
		m_ctlTabCtrl.ShowWindow( SW_SHOW ) ;
		m_ctlFontDlg.ShowWindow( SW_SHOW ) ;

		ShowPage (m_ctlTabCtrl.GetCurSel()) ;
	}
}

//*****************************************************************************
//
// Function Name:  RHeadlineDlg::OnReadyMade
//
// Description:    BN_CLICKED handler for the ready-mades radio button
//
// Returns:        VOID
//
// Exceptions:	   None
//
//*****************************************************************************
void RHeadlineDlg::OnReadyMade() 
{
	if (kReadyMade != m_nMode)
	{
		m_nMode = kReadyMade ;

		if (m_pCurrentPage)
		{
			m_pCurrentPage->ShowWindow (SW_HIDE) ;
		}

		m_ctlFontDlg.ShowWindow( SW_HIDE ) ;
		m_ctlTabCtrl.ShowWindow (SW_HIDE) ;

		m_gcImageList.ShowWindow (SW_SHOW) ;
		m_ctlStaticText.ShowWindow( SW_SHOW ) ;
	}
}

//*****************************************************************************
//
// Function Name:  RHeadlineDlg::OnSelChangeTabs
//
// Description:    ON_CBN_SELCHANGE handler for the tab control
//
// Returns:        None
//
// Exceptions:	    None
//
//*****************************************************************************
void RHeadlineDlg::OnSelChangeTabs(NMHDR*, LRESULT* pResult) 
{
	ShowPage (m_ctlTabCtrl.GetCurSel()) ;
	*pResult = 0;

	if (GetFocus() != (CWnd *) &m_ctlTabCtrl)
	{
		m_pCurrentPage->GetTopWindow()->SetFocus() ;
	}
}

//*****************************************************************************
//
// Function Name:  RHeadlineDlg::OnKillfocusHeadlineText
//
// Description:    EN_KILLFOCUS handler for headline text edit control
//
// Returns:        None
//
// Exceptions:	    None
//
//*****************************************************************************
void RHeadlineDlg::OnKillfocusHeadlineText() 
{
	OnTimer( kTimerOne ) ;

	// Update dependencies
	m_pageShape.SetNumTextLines( m_ecHeadlineText.GetLineCount() ) ;
}

//*****************************************************************************
//
// Function Name: RHeadlineDlg::OnFontChange
//
// Description:   UM_HEADLINE_FONT_CHANGE handler for receiving 
//					   data change messages from the font information.
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//*****************************************************************************
LRESULT RHeadlineDlg::OnFontChange( WPARAM, LPARAM )
{
	m_ctlFontDlg.FillData( m_pInterface ) ;
	m_gcImageList.SetCurSel( -1 );
	UpdatePreview() ;

	return TRUE ;
}

//*****************************************************************************
//
// Function Name: RHeadlineDlg::OnFaceChange
//
// Description:   UM_HEADLINE_FACE_CHANGE handler for receiving 
//					   data change messages from the depth property
//					   page.
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//*****************************************************************************
LRESULT RHeadlineDlg::OnFaceChange( WPARAM, LPARAM )
{
	//
	// Update headline data, and commit the changes
	//
	m_pageFace.FillData( m_pInterface ) ;

	EOutlineEffects eOutlineEffect ;
	m_pInterface->GetOutlineEffect( eOutlineEffect ) ;

/*	if (kEmbossedOutline == eOutlineEffect || kDebossedOutline == eOutlineEffect)
	{
		// Recompute emboss hilights and shadows
		RHeadlindOutlinePage::ComputeEmbossDebossTints( m_pInterface ) ;
	}*/

	m_gcImageList.SetCurSel( -1 );
	UpdatePreview() ;

	// Update dependencies
	m_pageShape.ApplyData( m_pInterface ) ;
//	m_pageOutline.ApplyData( m_pInterface, RHeadlindOutlinePage::kFaceChanged ) ;

	return TRUE ;
}

//*****************************************************************************
//
// Function Name: RHeadlineDlg::OnShapeChange
//
// Description:   UM_HEADLINE_SHAPE_CHANGE handler for receiving 
//					   data change messages from the depth property
//					   page.
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//*****************************************************************************
LRESULT RHeadlineDlg::OnShapeChange( WPARAM, LPARAM )
{
	m_pageShape.FillData( m_pInterface ) ;

	EOutlineEffects eOutlineEffect ;
	m_pInterface->GetOutlineEffect( eOutlineEffect ) ;

/*	if (kEmbossedOutline == eOutlineEffect || kDebossedOutline == eOutlineEffect)
	{
		// Recompute emboss hilights and shadows
		RHeadlindOutlinePage::ComputeEmbossDebossTints( m_pInterface ) ;
	}*/

	m_gcImageList.SetCurSel( -1 );
	UpdatePreview() ;

	// Update dependencies
	m_pageFace.ApplyData( m_pInterface ) ;
//	m_pageOutline.ApplyData( m_pInterface, RHeadlindOutlinePage::kShapeChanged ) ;
	m_pageProportion.ApplyData( m_pInterface ) ; 
	m_pagePosition.ApplyData( m_pInterface ) ; 
	m_pageDepth.ApplyData( m_pInterface, m_pComponentView ) ;
	m_ctlFontDlg.ApplyData( m_pInterface ) ;

	ETextEscapement eEscapement ;
	m_pInterface->GetEscapement( eEscapement ) ;
	int nLineLimit = (eEscapement < kTop2BottomLeft2Right ? 3 : 1) ;
	m_ecHeadlineText.SetLineLimit( nLineLimit ) ;
	
	return TRUE ;
}

//*****************************************************************************
//
// Function Name: RHeadlineDlg::OnPositionChange
//
// Description:   UM_HEADLINE_POSITION_CHANGE handler for receiving 
//					   data change messages from the position property
//					   page.
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//*****************************************************************************
LRESULT RHeadlineDlg::OnPositionChange( WPARAM, LPARAM )
{
	// Update component and commit the changes
	m_pagePosition.FillData( m_pInterface ) ;
	m_gcImageList.SetCurSel( -1 );
	UpdatePreview() ;

	// Update dependencies
	m_pageShape.ApplyData( m_pInterface ) ;

	return TRUE ;
}

//*****************************************************************************
//
// Function Name: RHeadlineDlg::OnOutlineChange
//
// Description:   UM_HEADLINE_OUTLINE_CHANGE handler for receiving 
//					   data change messages from the outline property
//					   page.
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//*****************************************************************************
LRESULT RHeadlineDlg::OnOutlineChange( WPARAM, LPARAM )
{
	RView* pView = m_pComponentDocument->GetActiveView() ;
	pView->Invalidate( FALSE );

	// Update headline data, and commit the changes
	m_pageOutline.FillData( m_pInterface, m_pComponentView ) ;
	m_gcImageList.SetCurSel( -1 );
	UpdatePreview() ;

	m_pageDepth.ApplyData( m_pInterface, m_pComponentView ) ;	

	if (fShadowTabSupported)
		m_pageShadow.ApplyData( m_pInterface, m_pComponentView ) ;	

	return TRUE ;
}

//*****************************************************************************
//
// Function Name: RHeadlineDlg::OnDepthChange
//
// Description:   UM_HEADLINE_DEPTH_CHANGE handler for receiving 
//					   data change messages from the depth property
//					   page.
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//*****************************************************************************
LRESULT RHeadlineDlg::OnDepthChange( WPARAM, LPARAM )
{
	RView* pView = m_pComponentDocument->GetActiveView() ;
	pView->Invalidate( FALSE );

	// Update headline data, and commit the changes
	m_pageDepth.FillData( m_pInterface, m_pComponentView ) ;
	m_gcImageList.SetCurSel( -1 );
	UpdatePreview() ;

	// Update interdependencies
	m_pageFace.ApplyData( m_pInterface ) ;
	m_pageOutline.ApplyData( m_pInterface, m_pComponentView, RHeadlindOutlinePage::kDepthChanged ) ;
	
	if (fShadowTabSupported)
		m_pageShadow.ApplyData( m_pInterface, m_pComponentView ) ;	

	return TRUE ;
}

//*****************************************************************************
//
// Function Name: RHeadlineDlg::OnShadowChange
//
// Description:   UM_HEADLINE_SHADOW_CHANGE handler for receiving 
//					   data change messages from the depth property
//					   page.
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//*****************************************************************************
LRESULT RHeadlineDlg::OnShadowChange( WPARAM, LPARAM )
{
	RView* pView = m_pComponentDocument->GetActiveView() ;
	pView->Invalidate( FALSE );

	// Update headline data, and commit the changes
	m_pageShadow.FillData( m_pInterface, m_pComponentView ) ;
	m_gcImageList.SetCurSel( -1 );
	UpdatePreview() ;

	// Update interdependencies
	m_pageFace.ApplyData( m_pInterface ) ;
	m_pageOutline.ApplyData( m_pInterface, m_pComponentView, RHeadlindOutlinePage::kDepthChanged ) ;
	m_pageDepth.ApplyData( m_pInterface, m_pComponentView ) ;	

	return TRUE ;
}

//*****************************************************************************
//
// Function Name: RHeadlineDlg::OnProportionChange
//
// Description:   UM_HEADLINE_PROPORTION_CHANGE handler for receiving 
//					   data change messages from the proportion property
//					   sheet.
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//*****************************************************************************
LRESULT RHeadlineDlg::OnProportionChange( WPARAM, LPARAM )
{
	// Update headline data, and commit the changes
	m_pageProportion.FillData( m_pInterface ) ;
	m_gcImageList.SetCurSel( -1 );
	UpdatePreview() ;

	return TRUE ;
}

//*****************************************************************************
//
// Function Name: RHeadlineDlg::OnSaveAs
//
// Description:   BN_CLICKED handler for the "save as" button.  Note:
//				      this is a temporary handler for debugging purposes
//				      only.
//
// Returns:       None
//
// Exceptions:	   None
//
//*****************************************************************************
#ifdef	TPSDEBUG
void RHeadlineDlg::OnSaveAs() 
{
	CString strFileFilter( "Ready Made Headlines|*.rmh||" ) ;
	CFileDialog dlg( FALSE, strFileFilter, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFileFilter ) ;

	if (dlg.DoModal() == IDOK)
	{
		CString strPathName = dlg.GetPathName() ;
		CString strExt = dlg.GetFileExt() ;

		if (strExt.IsEmpty())
		{
			strPathName += ".rmh" ;
		}

		// Clear the text, as there is no need to save
		// this information with the ready-made.
		m_pInterface->SetHeadlineText( RMBCString("") ) ;

		// Write out the data as a ready-made headline file
		RStorage rStorage( RMBCString( strPathName ), kWriteReplaceExisting ) ;


		CString strFileID( dlg.GetFileName() );
		strFileID = strFileID.Left( strFileID.ReverseFind( '.' ) );
		rStorage.Write( strFileID.GetLength() + 1, (const uBYTE*)(LPCTSTR) strFileID );

//		RMBCString strFileID( dlg.GetFileName() );
//		rStorage << strFileID;

		RHeadlineHolder* pData = (RHeadlineHolder *) m_pInterface->GetData() ; 
		pData->Write( rStorage ) ;
		delete pData ;

		RSoftShadowSettings shadowSettings;
		m_pComponentView->GetShadowSettings( shadowSettings );
		rStorage << shadowSettings;

		RSoftGlowSettings glowSettings;
		m_pComponentView->GetGlowSettings( glowSettings );
		rStorage << glowSettings;

		// Restore the text.
		m_pInterface->SetHeadlineText( RMBCString(m_strHeadlineText) ) ;
	}
}

//*****************************************************************************
//
// Function Name: ResaveReadyMades
//
// Description:   Opens and resaves all ready made headline files
//
// Returns:       None
//
// Exceptions:	   None
//
//*****************************************************************************

void RHeadlineDlg::ResaveReadyMades()
{
	TpsAssert( m_pInterface, "Headline dialog not completely initialized!" );
	::SetCurrentDirectory( "\\Pretzel\\Resource\\Shared\\Headlines\\ReadyMades\\" );

	WIN32_FIND_DATA data;
	HANDLE h = ::FindFirstFile( "*.rmh", &data );

	if (INVALID_HANDLE_VALUE != h)
	{
		RHeadlineHolder* pData = (RHeadlineHolder *) m_pInterface->GetData() ; 

		do 
		{
			RMBCString    fileName( data.cFileName );
			RStorage      rStorage( fileName, kReadWrite ) ;
			RStream*		  pStream = rStorage.GetStorageStream( );

			CString strFileID = pStream->GetBuffer( _MAX_FNAME );
			rStorage.SeekRelative( strFileID.GetLength() + 1 );

			pData->Read( rStorage ) ;

			RSoftShadowSettings shadowSettings;
			RSoftGlowSettings glowSettings;

			try
			{
				rStorage >> shadowSettings;
				rStorage >> glowSettings;
			}
			catch (...)
			{
			}


			// Re-write the data
			pStream->Close( );
			pStream->Open( kWriteReplaceExisting );

			rStorage.Write( strFileID.GetLength() + 1, (const uBYTE*)(LPCTSTR) strFileID );

			pData->Write( rStorage ) ;
			rStorage << shadowSettings;
			rStorage << glowSettings;

		}
		while ( ::FindNextFile( h, &data ) );

		::FindClose( h );
		delete pData ;
	}
}
#endif

//****************************************************************************
//
// Function Name: OnChangeHeadlineText
//
// Description:   EN_CHANGE notification handler for the headline text
//
// Returns:       Nothing
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineDlg::OnChangeHeadlineText() 
{
	if (m_uiTimer)
	{
		KillTimer( m_uiTimer ) ;
	}

	m_uiTimer = SetTimer( kTimerOne, kElapsedTimeBeforeUpdate, NULL ) ;
}

//****************************************************************************
//
// Function Name: OnSelChangeImageList
//
// Description:   Selection change handler for the image list
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineDlg::OnSelChangeImageList( )
{
	int nSel = m_gcImageList.GetCurSel() ;

	if (LB_ERR != nSel)
	{
		try
		{
			// Place the loaded data, our headline text,
			// and the interface data into the document
			m_gcImageList.LoadReadyMade( nSel, m_pInterface, m_pComponentView );
			m_pInterface->SetHeadlineText( RMBCString(m_strHeadlineText) ) ;

			// 01/23/97 Lance - Whatever changed, was changed back, to
			// the code has been commented out.  However, it is being
			// left in as there is still some troubles in the gradient
			// rendering code.
			//
			// 10/30/97 Lance - Something changed in the gradient code so
			// that the saved angle in the readymade is no longer valid.
			// This is a temporary fix to adjust the angle, until the 
			// readymades can be corrected and resaved.
			//
			/*
			RColor color ;
			m_pInterface->GetOutlineFillColor( color ) ;

			if (RColor::kGradient == color.GetFillMethod())
			{
				color.m_pGradient->SetGradientAngle(
					color.m_pGradient->GetGradientAngle() + kPI ) ;
				m_pInterface->SetOutlineFillColor( color ) ;
			}
			*/

#if 0
			// 05/26/98 Lance - All ready made files have been corrected
			// so no angle adjustment is necessary.

			// 04/08/98 Lance - Due to the changes in the gradinet code,
			// it is no longer necessary to add pi to the gradient angle.
			// In most cases this was fixed in the color serialization,
			// but not for the silhouette color.  So, it needs to be
			// done here.
			RColor color ;
			m_pInterface->GetBehindColor( color ) ;

			if (RColor::kGradient == color.GetFillMethod())
			{
				color.m_pGradient->SetGradientAngle(
					color.m_pGradient->GetGradientAngle() + kPI ) ;
				m_pInterface->SetBehindColor( color ) ;
			}
#endif

			ETextEscapement eEscapement ;
			m_pInterface->GetEscapement( eEscapement ) ;

			if (eEscapement >= kTop2BottomLeft2Right)
			{
				// Vertical needs to be forced to center justify
				m_pInterface->SetJustification( kCenterJust ) ;
			}

			ApplyData() ;
			UpdatePreview() ;
		}
		catch (...)
		{
			RAlert rAlert ;
			rAlert.AlertUser( STRING_ERROR_ACC_UNKNOWN ) ;
		}
	}
}

//****************************************************************************
//
// Function Name: OnMenuChar
//
// Description:   WM_MENUCHAR message handler
//
// Returns:       The high-order word of the return value should contain one of 
//                the following command codes:
//						
//                Value	Description
//
//                0	Tells Windows to discard the character that the user pressed 
//                   and creates a short beep on the system speaker.
//                1	Tells Windows to close the current menu.
//                2	Informs Windows that the low-order word of the return value 
//                   contains the item number for a specific item. This item is 
//                   selected by Windows.
//
//                The low-order word is ignored if the high-order word contains 0 
//                or 1. Applications should process this message when accelerator 
//                (shortcut) keys are used to select bitmaps placed in a menu.
//
// Exceptions:	   None
//
//****************************************************************************
LRESULT RHeadlineDlg::OnMenuChar( UINT nChar, UINT nFlags, CMenu* pMenu) 
{
	if (kCustomize == m_nMode)
	{

		try
		{
			// Setup the text string to look for
			CString strFind(_T("&")) ;
			strFind += nChar ;
			strFind.MakeUpper() ;

			// Setup the TC_ITEM struct to obtain
			// the tabs text from the tab control
			CString strText ;
			TC_ITEM item ;
			item.mask = TCIF_TEXT ;
			item.pszText = strText.GetBuffer( 256 ) ;
			item.cchTextMax = 256 ;

			//
			// Loop through each tab, and see if it contains the
			// hot-key text specified by strFind.
			//
			for (int i = 0; i < m_ctlTabCtrl.GetItemCount(); i++)
			{
				m_ctlTabCtrl.GetItem( i, &item ) ;
				strText.MakeUpper() ;

				if (strText.Find( strFind ) >= 0)
				{
					// Found it, so, set the tab control's
					// current selection, and show the 
					// corresponding page.
					m_ctlTabCtrl.SetCurSel( i ) ;
					ShowPage( i ) ;
					
					m_pCurrentPage->GetTopWindow()->SetFocus() ;
					return MAKELONG( 0, 1 ) ;
				}
			}
		}

		catch (CMemoryException&)
		{
		}
	}

	return CDialog::OnMenuChar( nChar, nFlags, pMenu ) ;
}

//****************************************************************************
//
// Function Name: ApplyData
//
// Description:   Applies a headline data struct object to each of the 
//                dialogs child controls.
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineDlg::ApplyData( ) 
{
	// Update the dialog and each of the 
	// property pages with the new data.
	//////////////////////////////////////
	RMBCString strHeadlineText ;
	m_pInterface->GetHeadlineText( strHeadlineText ) ;
	m_strHeadlineText = strHeadlineText.operator CString( ) ; 

	m_ctlFontDlg.ApplyData( m_pInterface ) ;
	m_pageFace.ApplyData( m_pInterface ) ;
	m_pageShape.ApplyData( m_pInterface ) ;
	m_pagePosition.ApplyData( m_pInterface ) ;
	m_pageOutline.ApplyData( m_pInterface, m_pComponentView ) ;
	
	if (fShadowTabSupported)
		m_pageShadow.ApplyData( m_pInterface, m_pComponentView ) ;	
	
	m_pageDepth.ApplyData( m_pInterface, m_pComponentView ) ;
	m_pageProportion.ApplyData( m_pInterface ) ;

	// Update the display
	UpdateData( FALSE ) ;
}

//****************************************************************************
//
// Function Name: OnTimer
//
// Description:   WM_TIMER message handler
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineDlg::OnTimer( UINT nIDEvent ) 
{
	if (kTimerOne == nIDEvent)
	{
		if (m_uiTimer)
		{
			KillTimer( m_uiTimer ) ;
			m_uiTimer = 0 ;
		}

		CString strHeadlineText = m_strHeadlineText ;
		UpdateData (TRUE) ;

		if (strHeadlineText != m_strHeadlineText)
		{
			m_pInterface->SetHeadlineText( RMBCString( m_strHeadlineText ) ) ;
			UpdatePreview() ;
		}

		return ;
	}
	
	CDialog::OnTimer(nIDEvent);
}

//*****************************************************************************
//
// Function Name:  RHeadlineDlg::UpdatePreview
//
// Description:    Updates the preview headline when the data changes
//
// Returns:        VOID
//
// Exceptions:	   None
//
//*****************************************************************************
//
void RHeadlineDlg::UpdatePreview()
{
	RComponentView* pView = (RComponentView*) m_pComponentDocument->GetActiveView() ;

	BeginWaitCursor() ;

	//
	// Check to see if we need to update the view size
	//
	ETextEscapement eEscapement ;
	m_pInterface->GetEscapement( eEscapement ) ;
	BOOLEAN fMultiLine = BOOLEAN( m_ecHeadlineText.GetLineCount() > 1 ) ;
	BOOLEAN fVertText  = BOOLEAN( eEscapement >= kTop2BottomLeft2Right ) ;

	if (/*fMultiLine != m_fMultiLine ||*/ fVertText != m_fVertText)
	{
		// Update the status flags
		m_fMultiLine = fMultiLine ; 
		m_fVertText  = fVertText ;

		//
		// Update the component view size
		//
		RRealRect rectArea = m_pControlView->GetUseableArea() ;
		RRealRect rect(RRealSize( rectArea.Width(), rectArea.Width() * kHeadlineAspectRatio )) ;

		if (m_pEditedComponent)
		{
			ETextEscapement eEditedEscapement ;
			RComponentView* pEditedView = (RComponentView *) m_pEditedComponent->GetActiveView( );
			RHeadlineInterface* pInterface = (RHeadlineInterface*) pEditedView->GetInterface( kHeadlineInterfaceId ) ;

			pInterface->GetEscapement( eEditedEscapement ) ;
			BOOLEAN fEditedVertText  = BOOLEAN( eEditedEscapement >= kTop2BottomLeft2Right ) ;

			if (fEditedVertText == fVertText)
			{
				rect = RRealRect( pEditedView->GetSize() ) ;

				// Don't worry about it being vertical text or not,
				// as we are using the edited components size because
				// it has the same escapement that we are now using.
				fVertText = FALSE ;
			}

			delete pInterface ;
		}

//		if (m_fMultiLine)
//		{
//			// Multi-line headlines should use the entire preview space
//			// where single-line headlines only use a portion of it.
//			rect.m_Right  = rectArea.Width() - 1 ;
//			rect.m_Bottom = rectArea.Height() - 1 ;
//		}

		if (fVertText)
		{
			// Rotate our view size for vertical text, 
			// and then shrink to fit the useable area.
			// Note: Our current preview rect's top left
			// point is (0,0), so all we need to do is
			// swap the right and bottom values.
			YFloatType temp = rect.m_Right ;
			rect.m_Right  = rect.m_Bottom ;
			rect.m_Bottom = temp ;
			rect.ShrinkToFit( rectArea ) ;
		}

		// Now center our view rect inside the
		// control container's useable area.
		rect.ShrinkToFit( rectArea ) ;
		rect.CenterRectInRect( rectArea ) ;
		pView->SetBoundingRect( rect ) ;
	}

	//
	// Update the preview
	//
	m_pInterface->Commit() ;
	pView->InvalidateRenderCache() ;
	m_pControlView->GetCWnd().UpdateWindow() ;
	EndWaitCursor() ;
}

		
//*****************************************************************************
//
// Function Name:  RHeadlineDlg::OnOK
//
// Description:    OK message handler; stores the current UI state
//                 into the headline conext data structure.
//
// Returns:        Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
//
void RHeadlineDlg::OnOK()
{
	// Store the current UI state.
	m_rContextData.m_wMode  = (sWORD) m_nMode;
	m_rContextData.m_wIndex = (sWORD)( kCustomize == m_nMode ? 
		m_ctlTabCtrl.GetCurSel() : 0 );

	CString itemData;

	if (LB_ERR != m_gcImageList.GetCurSel())
		itemData = m_gcImageList.GetID( m_gcImageList.GetCurSel() );

	m_rContextData.m_strReadyMade = itemData ;

	CDialog::OnOK();
}

//*****************************************************************************
//
// Function Name:  RHeadlineDlg::OnDestroy
//
// Description:    WM_DESTORY message handler
//
// Returns:        Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
//
void RHeadlineDlg::OnDestroy( )
{
	if (m_ctlFontDlg.m_hWnd) m_ctlFontDlg.EndDialog( IDOK );
	if (m_pageFace.m_hWnd) m_pageFace.EndDialog( IDOK ) ;
	if (m_pageShape.m_hWnd) m_pageShape.EndDialog( IDOK ) ;
	if (m_pageOutline.m_hWnd) m_pageOutline.EndDialog( IDOK ) ;
	if (m_pageShadow.m_hWnd) m_pageShadow.EndDialog( IDOK ) ;
	if (m_pageDepth.m_hWnd) m_pageDepth.EndDialog( IDOK ) ;
	if (m_pagePosition.m_hWnd) m_pagePosition.EndDialog( IDOK ) ;
	if (m_pageProportion.m_hWnd) m_pageProportion.EndDialog( IDOK ) ;
}

//*****************************************************************************
//
// Function Name:  RHeadlineDlg::OnHelp
//
// Description:    ID_HELP message handler
//
// Returns:        Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
//
void RHeadlineDlg::OnHelp( )
{
	AfxGetApp()->WinHelp( 0x20000 + IDD ) ;
}

/////////////////////////////////////////////////////////////////////////////
// RHeadlineDlg::RHeadlineContextData
/////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//
//  Function Name:	RHeadlineContextData::Write()
//
//  Description:		Writes the objects UI persistant data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineDlg::RHeadlineContextData::Write( RArchive& archive ) const
{
	archive << m_wSchema;
	archive << m_wMode;
	archive << m_wIndex;
	archive << m_strReadyMade;

	// Unused
	uLONG ulValue = 0;
	archive << ulValue;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineContextData::Read()
//
//  Description:		Reads the objects UI persistant data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineDlg::RHeadlineContextData::Read( RArchive& archive )
{
	archive >> m_wSchema;
	archive >> m_wMode;
	archive >> m_wIndex;
	archive >> m_strReadyMade;

	// Unused
	uLONG ulValue;
	archive >> ulValue;
}


/////////////////////////////////////////////////////////////////////////////
// RHeadlinePreviewView
/////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//
// Function Name:  RHeadlinePreviewView::RHeadlinePreviewView
//
// Description:    Constructor
//
// Returns:        Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
//
RHeadlinePreviewView::RHeadlinePreviewView( CDialog* pDialog, int nControlId, RControlDocument* pDocument )
	: RControlView( pDialog, nControlId, pDocument )
	{
	;
	}

//*****************************************************************************
//
// Function Name:  RHeadlinePreviewView::OnXLButtonDown
//
// Description:    Mouse move message
//
// Returns:        Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
//
void RHeadlinePreviewView::OnXLButtonDown( const RRealPoint&, YModifierKey )
	{
	;
	}

//*****************************************************************************
//
// Function Name:  RHeadlinePreviewView::OnXDragEnter
//
// Description:    Drag enter message
//
// Returns:        Drop effect
//
// Exceptions:	    None
//
//*****************************************************************************
//
YDropEffect RHeadlinePreviewView::OnXDragEnter( RDataTransferSource&, YModifierKey, const RRealPoint& )
	{
	return kDropEffectNone;
	}

//*****************************************************************************
//
// Function Name:  RHeadlinePreviewDocument::CreateView
//
// Description:    Creates a headline preview view
//
// Returns:        Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
//
RControlView* RHeadlinePreviewDocument::CreateView( CDialog* pDialog, int nControlId )
	{
	RControlView* pView = new RHeadlinePreviewView( pDialog, nControlId, this );
	return pView;
	}
