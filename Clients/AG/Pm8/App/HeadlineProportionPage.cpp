//****************************************************************************
//
// File Name:   HeadlineProportionPage.cpp
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Manages RHeadlineProportionPage class which is used for the 
//					 proportion property page in the headline user 
//					 interface.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//					  500 Redwood Blvd.
//					  Novato, CA 94948
//					  (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/HeadlineProportionPage.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"
ASSERTNAME

#include "HeadlineCompResource.h"
#include "HeadlineProportionPage.h"
#include "HeadlineInterface.h"

#include "FrameworkResourceIDs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int kImageWidth  = 42 ;
const int kImageHeight = 43 ;
const int kNumRows     =  2 ;
const int kNumCols     =  7 ;
const int kImageCount  = 11 ;

/////////////////////////////////////////////////////////////////////////////
// RHeadlineProportionPage property page

IMPLEMENT_DYNCREATE(RHeadlineProportionPage, CDialog)

//****************************************************************************
//
// Function Name: RHeadlineProportionPage::RHeadlineProportionPage
//
// Description:   Constructor
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
RHeadlineProportionPage::RHeadlineProportionPage() : CDialog(RHeadlineProportionPage::IDD)
{
	//{{AFX_DATA_INIT(RHeadlineProportionPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_uwScale1 = 0 ;				//Headline Scale 1
	m_uwScale2 = 0 ;				//Headline Scale 2
	m_uwScale3 = 0 ;				//Headline Scale 3

	m_uiResID  = IDB_HEADLINE_PROPORTION ;  // Default image list
}

//****************************************************************************
//
// Function Name: RHeadlineProportionPage::~RHeadlineProportionPage
//
// Description:   Destructor 
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
RHeadlineProportionPage::~RHeadlineProportionPage()
{
}

//****************************************************************************
//
// Function Name: DoDataExchange
//
// Description:   Handles the data exchange to and from the dialogs 	
//                child controls 
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineProportionPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RHeadlineProportionPage)
	DDX_Control( pDX, IDC_IMAGE_LIST, m_gcImageList );
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(RHeadlineProportionPage, CDialog)
	//{{AFX_MSG_MAP(RHeadlineProportionPage)
	ON_LBN_SELCHANGE( IDC_IMAGE_LIST, OnSelChangeImageList )	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: ApplyData
//
// Description:   Applies an HeadlineDataStruct attribute data
//                to the Headline proportion controls
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineProportionPage::ApplyData( RHeadlineInterface* pInterface )
{
	TpsAssert( pInterface, "Invalid Interface!" ) ;

	pInterface->GetScale1( m_uwScale1 ) ;
	pInterface->GetScale2( m_uwScale2 ) ;
	pInterface->GetScale3( m_uwScale3 ) ;

	UINT uiResID = m_uiResID ;
	m_uiResID    = IDB_HEADLINE_PROPORTION ;

	BOOLEAN fDistort ;
	pInterface->GetDistort( fDistort ) ;

	if (fDistort)
	{
		m_uiResID = IDB_HEADLINE_PROPORTION_DISTORT ;
	}

	if (GetSafeHwnd())
	{
		if (uiResID != m_uiResID)
		{
			if (!m_ilImageList.Create( (uWORD) m_uiResID, kImageWidth, kImageHeight, 0, CLR_NONE ))
			{
				// Let the user know there is
				// a reason there staring at
				// a blank page.
				RAlert rAlert ;
				rAlert.AlertUser( STRING_ERROR_ACC_OUT_OF_MEMORY ) ;
			}
		}

		// Set the image selection
		EnumResourceNames( 
			AfxGetResourceHandle(),				// resource-module handling
			_T("HEADLINE_PROPORTION"),			// pointer to resource type 
			(ENUMRESNAMEPROC) SelectResItem,	// pointer to callback function 
			(LONG) this ) ; 						// application-defined parameter 
	}
}

//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills an HeadlineDataStruct with the Headline 
//						proportion attribute data
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineProportionPage::FillData( RHeadlineInterface* pInterface )
{
	TpsAssert( pInterface, "Invalid Interface!" ) ;

	pInterface->SetScale1( m_uwScale1 ) ;
	pInterface->SetScale2( m_uwScale2 ) ;
	pInterface->SetScale3( m_uwScale3 ) ;
}

//****************************************************************************
//
// Function Name: LoadResData
//
// Description:   Fills an AttribDataStruct with the Headline 
//						proportion attribute data from a specified 
//						resource.
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RHeadlineProportionPage::LoadResData( HMODULE hModule, LPTSTR lpszName, ProportionPageData& data )
{
	HRSRC hResInfo = FindResource( hModule, lpszName, 
		_T("HEADLINE_PROPORTION") ) ;

	if (hResInfo)
	{
		HGLOBAL hResData = LoadResource( AfxGetResourceHandle(), hResInfo ) ; 
		uWORD*  lpData   = (uWORD *) LockResource( hResData ) ;

		data.uwScale1 = lpData[0] ;
		data.uwScale2 = lpData[1] ;
		data.uwScale3 = lpData[2] ;

		FreeResource( hResData ) ;
		return TRUE ;
	}
	else
	{
		data.uwScale1 = 0 ;
		data.uwScale2 = 0 ;
		data.uwScale3 = 0 ;
	}

	return FALSE ;
}

BOOL CALLBACK RHeadlineProportionPage::SelectResItem (
	HANDLE  hModule,			// resource-module handle 
	LPCTSTR /*lpszType*/,	// pointer to resource type 
	LPTSTR  lpszName,			// pointer to resource name 
	LONG    lParam) 			// application-defined parameter  
{
	RHeadlineProportionPage* pPage = (RHeadlineProportionPage*) lParam ;

	ProportionPageData data ;
	pPage->LoadResData( (HMODULE) hModule, lpszName, data ) ;

	if (data.uwScale1 == pPage->m_uwScale1 &&
		 data.uwScale2 == pPage->m_uwScale2 &&
		 data.uwScale3 == pPage->m_uwScale3 )
	{
		pPage->m_gcImageList.SetCurSel( int(lpszName) - 100 ) ;

		return FALSE ;
	}

	return TRUE ;
}

//****************************************************************************
//
// Function Name: RHeadlineProportionPage::OnInitDialog
//
// Description:   Handles dialog initialization
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RHeadlineProportionPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	try
	{
		if (!m_ilImageList.Create( (uWORD) m_uiResID, kImageWidth, kImageHeight, 0, CLR_NONE ))
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
			_T("HEADLINE_PROPORTION"),			// pointer to resource type 
			(ENUMRESNAMEPROC) SelectResItem,	// pointer to callback function 
			(LONG) this ) ; 						// application-defined parameter 
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
// Function Name: RHeadlineProportionPage::OnSelChangeImageList
//
// Description:   Selection change handler for the image list
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineProportionPage::OnSelChangeImageList( )
{
	int nCurCell = m_gcImageList.GetCurSel() ;

	ProportionPageData data ;
	LoadResData( AfxGetResourceHandle(), MAKEINTRESOURCE( nCurCell + 100 ), data ) ;

	m_uwScale1 = data.uwScale1 ;
	m_uwScale2 = data.uwScale2 ;
	m_uwScale3 = data.uwScale3 ;

	GetParent()->SendMessage( UM_HEADLINE_PROPORTION_CHANGE ) ;
}
