//****************************************************************************
//
// File Name:   HeadlineShadowPage.cpp
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Manages RHeadlineShadowPage class which is used for the 
//				    'Shadow' Property Page in the headline user 
//				    interface.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/HeadlineShadowPage.cpp                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:06p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"
ASSERTNAME

#include "HeadlineCompResource.h"
#include "HeadlineShadowPage.h"
#include "HeadlineInterface.h"
#include "ComponentView.h"
#include "SoftShadowSettings.h"

const int kImageWidth  = 42 ;
const int kImageHeight = 43 ;
const int kNumRows     =  2 ;
const int kNumCols     =  7 ;
const int kImageCount  =  7 ;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const sWORD kDepthType        = 0 ;
const sWORD kBehindType       = 1 ;
const sWORD kShadowType       = 2 ;

const char* kResourceType = _T("HEADLINE_SHADOW");  // Do NOT localize this string

/////////////////////////////////////////////////////////////////////////////
// RHeadlineShadowPage property page

IMPLEMENT_DYNCREATE(RHeadlineShadowPage, CDialog)

//*****************************************************************************
//
// Function Name:  RHeadlineShadowPage::RHeadlineShadowPage
//
// Description:    Constructor (Default).
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlineShadowPage::RHeadlineShadowPage() : CDialog(RHeadlineShadowPage::IDD),
	m_btnColor( RWinColorBtn::kInset )
{
	//{{AFX_DATA_INIT(RHeadlineShadowPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_angle = 0.0 ;
}

//*****************************************************************************
//
// Function Name:  RHeadlineShadowPage::~RHeadlineShadowPage
//
// Description:    Destructor 
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlineShadowPage::~RHeadlineShadowPage()
{
}

//*****************************************************************************
//
// Function Name:  RHeadlineShadowPage::DoDataExchange
//
// Description:    Method for transferning the contents of the child
//				       controls to/from the data member variables.
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
void RHeadlineShadowPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RHeadlineShadowPage)
	DDX_Control( pDX, IDC_IMAGE_LIST, m_gcImageList );
	DDX_Control( pDX, IDC_COLOR, m_btnColor );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RHeadlineShadowPage, CDialog)
	//{{AFX_MSG_MAP(RHeadlineShadowPage)
	ON_LBN_SELCHANGE( IDC_IMAGE_LIST, OnSelChangeImageList )	
	ON_MESSAGE( UM_ANGLE_CHANGE, OnAngleChange )
	ON_BN_CLICKED( IDC_COLOR, OnColor ) 
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: ApplyData
//
// Description:   Applies HeadlineDataStruct effect's data
//                to the Headline depth controls
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineShadowPage::ApplyData( RHeadlineInterface* pInterface, RComponentView* pComponentView )
{
	// Set the effects data
	//////////////////////////

	RSoftShadowSettings settings;
	pComponentView->GetShadowSettings( settings );

	m_data.nShadowOn = (pComponentView->HasShadow() ? 1 : 0);

	if (m_data.nShadowOn)
	{
		m_data.nOpacity       = ::Round( settings.m_fShadowOpacity * 100 );
		m_data.nEdgeSoftness  = ::Round( settings.m_fShadowEdgeSoftness * 100 );
		m_data.nOffset        = ::Round( settings.m_fShadowOffset * 100 );

		m_wndBlendAngle.SetAngle( -settings.m_fShadowAngle );
		m_angle = m_wndBlendAngle.GetAngle();

		m_btnColor.SetColor( RColor( settings.m_fShadowColor ) ) ;
	}

	if (GetSafeHwnd())
	{
		// Set the image selection
		EnumResourceNames( 
			AfxGetResourceHandle(),				// resource-module handling
			kResourceType,							// pointer to resource type 
			(ENUMRESNAMEPROC) SelectResItem,	// pointer to callback function 
			(LONG) this ) ; 						// application-defined parameter 

		m_btnColor.EnableWindow( m_gcImageList.GetCurSel() != 0 ) ;
	}
}

//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills an HeadlineDataStruct with the Headline 
//						depth effect data
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineShadowPage::FillData( RHeadlineInterface* pInterface, RComponentView* pComponentView )
{
	RSoftShadowSettings settings;
	settings.m_fShadowOn = m_data.nShadowOn;

	if (m_data.nShadowOn)
	{
		//
		// Set the shadow effects
		//
		settings.m_fShadowOpacity      = m_data.nOpacity / 100.0;
		settings.m_fShadowEdgeSoftness = m_data.nEdgeSoftness  / 100.0;
		settings.m_fShadowOffset       = m_data.nOffset / 100.0;

		settings.m_fShadowAngle = -m_wndBlendAngle.GetAngle();
		settings.m_fShadowColor = m_btnColor.GetColor().GetSolidColor();

		// Clear any depth effects
		pInterface->SetProjectionEffect( kNoProjection ) ;

		// Clear any mutually exclusive outline effects
		//
		EOutlineEffects eOutlineEffect ;
		pInterface->GetOutlineEffect( eOutlineEffect ) ;

		//
		// The user selected a depth option which is mutually exclusive 
		// with the emboss and deboss outline options, so we need to clear 
		// the emboss/deboss option if it is set.
		//
		switch (eOutlineEffect)
		{
			case kEmbossedOutline: 
			case kDebossedOutline:
			case kBlurredOutline:
			{
				// Clear the outline effect
				pInterface->SetOutlineEffect( kSimpleFill ) ;
			}
		}
	}

	pComponentView->SetShadowSettings( settings );
}

//****************************************************************************
//
// Function Name: LoadResData
//
// Description:   Fills an EffectsDataStruct with the Headline 
//						effects data from a specified resource.
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RHeadlineShadowPage::LoadResData( HMODULE hModule, LPTSTR lpszName, ShadowPageData& data )
{
	HRSRC hResInfo = FindResource( hModule, lpszName, kResourceType ) ;

	if (hResInfo)
	{
		HGLOBAL hResData = LoadResource( AfxGetResourceHandle(), hResInfo ) ; 
		ShadowPageData*  lpData = (ShadowPageData *) LockResource( hResData ) ;
		data = *lpData ;

		FreeResource( hResData ) ;
		return TRUE ;
	}
	else
	{
		memset( &data, 0, sizeof( data ) ) ;
	}

	return FALSE ;
}

BOOL CALLBACK RHeadlineShadowPage::SelectResItem (
	HANDLE  hModule,			// resource-module handle 
	LPCTSTR /*lpszType*/,	// pointer to resource type 
	LPTSTR  lpszName,			// pointer to resource name 
	LONG    lParam) 			// application-defined parameter  
{
	RHeadlineShadowPage* pPage = (RHeadlineShadowPage*) lParam ;

	ShadowPageData  data ;
	ShadowPageData& classData = pPage->m_data ;
	pPage->LoadResData( (HMODULE) hModule, lpszName, data ) ;

	BOOLEAN fFound = FALSE ;

	if ((!classData.nShadowOn && !data.nShadowOn) || memcmp( &classData, &data, sizeof( data ) ) == 0)
	{
		fFound = TRUE;
	} 

	if (fFound)
	{
		pPage->m_gcImageList.SetCurSel( int(lpszName) - 100 ) ;
	}

	return !fFound ;
}

/////////////////////////////////////////////////////////////////////////////
// RHeadlineShadowPage message handlers

BOOL RHeadlineShadowPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	try
	{
		if (!m_ilImageList.Create( IDB_HEADLINE_SHADOWS, kImageWidth, kImageHeight, 0, CLR_NONE ))
		{
			throw kResource ;
		}

		//
		// Set up the image grid control
		//
		m_gcImageList.SetImageList( &m_ilImageList, kImageCount ) ;
		m_gcImageList.SizeCellsToClient( kNumRows, kNumCols ) ;

		EnumResourceNames( 
			AfxGetResourceHandle(),				// resource-module handling
			kResourceType,							// pointer to resource type 
			(ENUMRESNAMEPROC) SelectResItem,	// pointer to callback function 
			(LONG) this ) ; 						// application-defined parameter 

		// Position the angle dialog
		////////////////////////////
		CRect rectAngle ;
		CWnd* pWnd = GetDlgItem( IDC_DEPTH_STATIC ) ;
		pWnd->GetWindowRect( rectAngle ) ;
		ScreenToClient( rectAngle ) ;

		CRect rect( 1, 8, 1, 3 ) ;
		MapDialogRect( rect ) ;
		rectAngle.DeflateRect( &rect ) ;

		if (!m_wndBlendAngle.Create( NULL, "", WS_CHILD | WS_TABSTOP, rectAngle, this, (UINT) -1 ))
		{
			throw kResource ;
		}

		m_wndBlendAngle.SetWindowPos( GetDlgItem( IDC_DEPTH_STATIC ), 
			0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW ) ;

		m_btnColor.EnableWindow( m_gcImageList.GetCurSel() != 0 ) ;
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

//****************************************************************************
//
// Function Name: RHeadlineShadowPage::OnSelChangeImageList
//
// Description:   Selection change handler for the image list
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineShadowPage::OnSelChangeImageList( )
{
	int nCurCell = m_gcImageList.GetCurSel() ;
	m_btnColor.EnableWindow( nCurCell != 0 ) ;

	ShadowPageData data ;
	
	if (LoadResData( AfxGetResourceHandle(), MAKEINTRESOURCE( nCurCell + 100 ), data ) )
	{
		m_data = data ;
		GetParent()->PostMessage( UM_HEADLINE_SHADOW_CHANGE ) ;
	}
}

//****************************************************************************
//
// Function Name: RHeadlineShadowPage::OnAngleChange
//
// Description:   UM_ANGLE_CHANGE message handler
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
LRESULT RHeadlineShadowPage::OnAngleChange( WPARAM, LPARAM )
{
	m_angle = m_wndBlendAngle.GetAngle() ;
	GetParent()->PostMessage( UM_HEADLINE_SHADOW_CHANGE ) ;

	return TRUE ;
}

//*****************************************************************************
//
// Function Name:  OnColor
//
// Description:    BN_CLICKED handler for the color button
//
// Returns:        VOID
//
// Exceptions:	    None
//
//*****************************************************************************
void RHeadlineShadowPage::OnColor() 
{
	RWinColorDlg dlg(this, 0) ;
	dlg.SetColor( m_btnColor.GetColor() ) ;

	if (IDOK == dlg.DoModal())
	{
		m_btnColor.SetColor( dlg.SelectedColor() ) ;
		GetParent()->PostMessage( UM_HEADLINE_SHADOW_CHANGE ) ;
	}
}
