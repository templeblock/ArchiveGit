//****************************************************************************
//
// File Name:  HeadlineFontDlg.cpp
//
// Project:    Renaissance headline user interface
//
// Author:     Lance Wilson
//
// Description: Implements RHeadlineFont class
//
// Portability: Windows Specific
//
// Developed by:  Broderbund Software, Inc.
//						500 Redwood Blvd.
//						Novato, CA 94948
//						(415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/HeadlineFontDlg.cpp                                   $
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

#include "HeadlineCompResource.h"
#include "HeadlineFontDlg.h"
#include "DialogUtilities.h"
#include "ApplicationGlobals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*****************************************************************************
//
// Function Name: RHeadlineFont::RHeadlineFont
//
// Description:   Constructor 
//
// Returns:			Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlineFont::RHeadlineFont(CWnd* pParent /*=NULL*/) :
	CDialog(RHeadlineFont::IDD, pParent),
	m_gbFontList( FALSE )
{
	//{{AFX_DATA_INIT(RHeadlineFont)
	m_nJustification  = -1 ;
	m_strFontName     = _T("") ;
	//}}AFX_DATA_INIT
}

//****************************************************************************
//
// Function Name: DoDataExchange
//
// Description:   Called by the framework to exchange and validate 
//                dialog data.
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineFont::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(RHeadlineFont)
	DDX_Control(pDX, IDC_BOLD, m_cbBold);
	DDX_Control(pDX, IDC_ITALIC, m_cbItalic);
	DDX_Control(pDX, IDC_JUSTIFY_LEFT, m_rbJustifyLeft);
	DDX_Control(pDX, IDC_JUSTIFY_CENTER, m_rbJustifyCenter);
	DDX_Control(pDX, IDC_JUSTIFY_RIGHT, m_rbJustifyRight);
	DDX_Control(pDX, IDC_JUSTIFY_FULL, m_rbJustifyFull);
	DDX_Control(pDX, IDC_FONTLIST, m_gbFontList);

	DDX_Radio(pDX, IDC_JUSTIFY_LEFT, m_nJustification);
	DDX_CBString(pDX, IDC_FONTLIST, m_strFontName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RHeadlineFont, CDialog)
	//{{AFX_MSG_MAP(RHeadlineFont)
	ON_CBN_SELCHANGE(IDC_FONTLIST, OnSelChangeFontList)
	ON_BN_CLICKED(IDC_JUSTIFY_FULL, OnJustifyChange)
	ON_BN_CLICKED(IDC_BOLD, OnBold)
	ON_BN_CLICKED(IDC_ITALIC, OnItalic)
	ON_BN_CLICKED(IDC_JUSTIFY_LEFT, OnJustifyChange)
	ON_BN_CLICKED(IDC_JUSTIFY_RIGHT, OnJustifyChange)
	ON_BN_CLICKED(IDC_JUSTIFY_CENTER, OnJustifyChange)
	ON_WM_FONTCHANGE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: ApplyData
//
// Description:   Sets the control data according to the attribute
//	               data structure.
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineFont::ApplyData( RHeadlineInterface* pInterface )
{
	RMBCString strFontName ;
	YFontAttributes attributes ;
	ETextJustification eJustification ;

	pInterface->GetFontName( strFontName ) ;
	pInterface->GetFontAttributes( attributes ) ;
	pInterface->GetJustification( eJustification ) ;
	pInterface->GetDistortEffect( m_eDistortEffects ) ;
	pInterface->GetEscapement( m_eEscapement ) ;

	m_nJustification  = eJustification ; 
	m_strFontName     = strFontName.operator CString( ) ; 

	m_cbBold.SetCheck( attributes & RFont::kBold ? 1 : 0 ) ;
	m_cbItalic.SetCheck( attributes & RFont::kItalic ? 1 : 0 ) ;

	if ( ::IsWindow( m_hWnd ))
	{
		UpdateControls() ;
		UpdateData( FALSE ) ;
	}
}

//****************************************************************************
//
// Function Name: Create
//
// Description:   Creates a new headline font dialog control
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RHeadlineFont::Create( CWnd* pParentWnd /*= NULL */ )
{
	return CDialog::Create( RHeadlineFont::IDD, pParentWnd ) ;
}

//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills a HeadlineDataStruct with the Headline Dlg's data
//
// Returns:       None
//
// Exceptions:	  None
//
//****************************************************************************
void RHeadlineFont::FillData( RHeadlineInterface* pInterface )
{
	YFontAttributes attributes = 0 ;
	
	if (m_cbBold.GetCheck() == 1) 
	{
		attributes |= RFont::kBold ;
	}

	if (m_cbItalic.GetCheck() == 1)
	{
		attributes |= RFont::kItalic ;
	}

	pInterface->SetFontName( RMBCString( m_strFontName ) ) ;
	pInterface->SetFontAttributes( attributes ) ;

	if (m_nJustification >= 0)
	{
		pInterface->SetJustification( (ETextJustification) m_nJustification ) ;
	}
}

//****************************************************************************
//
// Function Name: RHeadlineFont::InitFontList
//
// Description:   Uses EnumFontFamiles API to spin through the Truetype 
//				      and vector-device (printer) fonts in the system. First 
//						attempts to get a DC for the current printer, if 
//						unsuccessful the desktop DC is used for font enumeration.
//
// Returns:       None
//
// Exceptions:		None
//
//****************************************************************************
void  RHeadlineFont::InitFontList()
{
	CComboBox* pWnd = (CComboBox *) GetDlgItem (IDC_FONTLIST) ;

	if (pWnd)
	{
		const RMBCString& rFontList = (RApplication::GetApplicationGlobals())->GetFontList();
		ParseAndPutInComboBox( const_cast<RMBCString&>(rFontList), *pWnd );
	}
}

//****************************************************************************
//
// Function Name: RHeadlineFont::UpdateControls
//
// Description:   Sets controls to an appropriate state
//
// Returns:       None
//
// Exceptions:		None
//
//****************************************************************************
void RHeadlineFont::UpdateControls( ) 
{
	//
	// Disable left, right, and full justify if follow path, 
	// or vertical text; otherwise re-enable as necessary.
	//
	BOOLEAN fEnabled = BOOLEAN( kFollowPath != m_eDistortEffects ) ;

	if (m_eEscapement >= kTop2BottomLeft2Right)
	{
		fEnabled = FALSE ;
	}

	m_rbJustifyLeft.EnableWindow( fEnabled ) ;
	m_rbJustifyRight.EnableWindow( fEnabled ) ;
	m_rbJustifyFull.EnableWindow( fEnabled ) ;
	m_rbJustifyCenter.EnableWindow( m_eEscapement < kTop2BottomLeft2Right ) ;
}

/////////////////////////////////////////////////////////////////////////////
// RHeadlineFont message handlers

//****************************************************************************
//
// Function Name: RHeadlineFont::OnInitDialog
//
// Description:  Handles dialog initialization
//
// Returns:      TRUE
//
// Exceptions:	  None
//
//****************************************************************************
BOOL RHeadlineFont::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	try
	{
		// Fill the font combo-box with
		// the available fonts.
//		InitFontList() ;
		m_gbFontList.Initialise();
		m_gbFontList.SetLinkedControl( &m_gbFontList );

		if (!m_ImageList.LoadImageList( IDB_HEADLINE_TOOLBAR_TEXT, 23, RGB( 192, 192, 192 ) ))
		{
			throw kResource ;
		}

		m_cbBold.SetImage( &m_ImageList, 2 ) ;
		m_cbItalic.SetImage( &m_ImageList, 3 ) ;

		m_rbJustifyLeft.SetImage( &m_ImageList, 5 ) ;
		m_rbJustifyCenter.SetImage( &m_ImageList, 6 ) ;
		m_rbJustifyRight.SetImage( &m_ImageList, 7 ) ;
		m_rbJustifyFull.SetImage( &m_ImageList, 8 ) ;

		UpdateControls() ;
	}
	catch (...)
	{
		// End the dialog, and let who ever
		// created us handle any errors.
		EndDialog( IDCANCEL ) ;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//*****************************************************************************
//
// Function Name: RHeadlineFont::OnSelChangeFontList
//
// Description:   CBN_SELCHANGE handler for font combo-box
//
// Returns:       VOID
//
// Exceptions:		None
//
//*****************************************************************************
void RHeadlineFont::OnSelChangeFontList() 
{
	CString strFontName = m_strFontName ;

//	CComboBox* pWnd = (CComboBox *) GetDlgItem (IDC_FONTLIST) ;
// pWnd->GetLBText(pWnd->GetCurSel(), m_strFontName);
	m_gbFontList.GetLBText( m_gbFontList.GetCurSel(), m_strFontName );

	if (strFontName != m_strFontName)
	{
		GetParent()->SendMessage (UM_HEADLINE_FONT_CHANGE, (WPARAM) IDD) ;
	}
}

//*****************************************************************************
//
// Function Name:  RHeadlineFont::OnJustifyChange
//
// Description:    BN_CLICKED handler for all justification buttons
//
// Returns:        VOID
//
// Exceptions:		 None
//
//*****************************************************************************
void RHeadlineFont::OnJustifyChange() 
{
	int nJustification = m_nJustification ;

	UpdateData (TRUE) ;

	if (m_nJustification != nJustification)
	{
		GetParent()->SendMessage (UM_HEADLINE_FONT_CHANGE, (WPARAM) IDD) ;
	}
}

//*****************************************************************************
//
// Function Name:  RHeadlineFont::OnBold
//
// Description:    BN_CLICKED handler for the bold checkbox button.
//
// Returns:        VOID
//
// Exceptions:	   None
//
//*****************************************************************************
void RHeadlineFont::OnBold() 
{
	GetParent()->SendMessage (UM_HEADLINE_FONT_CHANGE, (WPARAM) IDD) ;
}

//*****************************************************************************
//
// Function Name:  RHeadlineFont::OnItalic
//
// Description:    BN_CLICKED handler for the italic checkbox button
//
// Returns:        VOID
//
// Exceptions:	   None
//
//*****************************************************************************
void RHeadlineFont::OnItalic() 
{
	GetParent()->SendMessage( UM_HEADLINE_FONT_CHANGE, (WPARAM) IDD) ;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineFont::OnFontChange( )
//
//  Description:		Font resources have changed so we need to update the 
//						font combo box to reflect these changes
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineFont::OnFontChange() 
{
	RMBCString rFontList = (RApplication::GetApplicationGlobals())->GetFontList();
	::ParseAndPutInComboBox( rFontList, m_gbFontList );

	UpdateData( FALSE );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineFont::OnDestroy( )
//
//  Description:		WM_DESTROY message handler
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineFont::OnDestroy( )
{
	m_gbFontList.SetLinkedControl( NULL );

	CDialog::OnDestroy();
}
