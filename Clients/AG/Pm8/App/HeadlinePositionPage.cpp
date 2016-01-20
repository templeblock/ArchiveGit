//****************************************************************************
//
// File Name:  PositionPage.cpp
//
// Project:    Renaissance user interface
//
// Author:     Lance Wilson
//
// Description:	Manages RHeadlinePositionPage class which is used for the 
//						'Position' Property Page in the headline user interface.
//
// Portability: Windows Specific
//
// Developed by:  Broderbund Software
//						500 Redwood Blvd.
//						Novato, CA 94948
//						(415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/HeadlinePositionPage.cpp                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"
ASSERTNAME

#include "HeadlineCompResource.h"
#include "HeadlinePositionPage.h"
#include "HeadlineInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int kImageWidth    = 42 ;
const int kImageHeight   = 43 ;
const int kNumRows       =  2 ;
const int kNumCols       =  7 ;

const int kPositionCount = 11 ;

/////////////////////////////////////////////////////////////////////////////
// RHeadlinePositionPage property page

IMPLEMENT_DYNCREATE(RHeadlinePositionPage, CDialog)

//*****************************************************************************
//
// Function Name:  RHeadlinePositionPage::RHeadlinePositionPage
//
// Description:    Constructor (Default).
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlinePositionPage::RHeadlinePositionPage() : CDialog(RHeadlinePositionPage::IDD)
{
	//{{AFX_DATA_INIT(RHeadlinePositionPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
//
// Function Name:  RHeadlinePositionPage::~RHeadlinePositionPage
//
// Description:    Destructor 
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlinePositionPage::~RHeadlinePositionPage()
{
}

//*****************************************************************************
//
// Function Name:  RHeadlinePositionPage::DoDataExchange
//
// Description:    Method for transferning the contents of the child
//				   controls to/from the data member variables.
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
void RHeadlinePositionPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RHeadlinePositionPage)
	DDX_Control( pDX, IDC_IMAGE_LIST, m_gcImageList );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RHeadlinePositionPage, CDialog)
	//{{AFX_MSG_MAP(RHeadlinePositionPage)
	ON_LBN_SELCHANGE( IDC_IMAGE_LIST, OnSelChangeImageList )	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: ApplyData
//
// Description:   Applies HeadlineDataStruct data to the 
//						pages child controls
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlinePositionPage::ApplyData( RHeadlineInterface* pInterface )
{
	// Set the data
	//////////////////////////
	ETextCompensation eCompensation ;
	EDistortEffects eDistortEffect ;
	BOOLEAN fKerning ;
	int nShearAngle ;
	int nRotation ;
	float flPercent ;

	pInterface->GetCompensation( eCompensation ) ;
	pInterface->GetKerning( fKerning ) ;
	pInterface->GetDistortEffect( eDistortEffect ) ;
	pInterface->GetShearAngle( nShearAngle ) ;
	pInterface->GetRotation( nRotation ) ;
	pInterface->GetAltPercent( flPercent ) ;

	m_data.nSelComp       = sWORD( eCompensation ) ;
	m_data.nKerning		 = sWORD( fKerning ) ;
	m_data.nDistortEffect = sWORD( eDistortEffect ) ;
	m_data.nShearAngle    = sWORD( nShearAngle ) ;
	m_data.nRotation      = sWORD( nRotation ) ;
	m_data.nAltPercent    = sWORD( flPercent * 100 ) ;

	if (GetSafeHwnd())
	{
		// Set the image selection
		EnumResourceNames( 
			AfxGetResourceHandle(),				// resource-module handling
			_T("HEADLINE_POSITION"),			// pointer to resource type 
			(ENUMRESNAMEPROC) SelectResItem,	// pointer to callback function 
			(LONG) this ) ; 						// application-defined parameter 
	}
}

//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills an HeadlineDataStruct with the pages 
//						current data settings
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlinePositionPage::FillData( RHeadlineInterface* pInterface )
{
	pInterface->SetCompensation( (ETextCompensation) m_data.nSelComp ) ;
	pInterface->SetKerning( (BOOLEAN) m_data.nKerning ) ;
	pInterface->SetDistortEffect( (EDistortEffects) m_data.nDistortEffect ) ;
	pInterface->SetShearAngle( m_data.nShearAngle ) ;
	pInterface->SetRotation( m_data.nRotation ) ;
	pInterface->SetAltPercent( m_data.nAltPercent / 100.0 ) ;
}

//****************************************************************************
//
// Function Name: LoadResData
//
// Description:   Fills a PositionPageData struct with the Headline 
//						data from a specified resource.
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RHeadlinePositionPage::LoadResData( HMODULE hModule, LPTSTR lpszName, PositionPageData& data )
{
	HRSRC hResInfo = FindResource( hModule, lpszName, 
		_T("HEADLINE_POSITION") ) ;

	if (hResInfo)
	{
		HGLOBAL hResData = LoadResource( AfxGetResourceHandle(), hResInfo ) ; 
		PositionPageData*  lpData = (PositionPageData *) LockResource( hResData ) ;
		data = *lpData ;

		FreeResource( hResData ) ;
		return TRUE ;
	}
	else
	{
		data.nSelComp         = (sWORD) kRegular ;
		data.nDistortEffect   = (sWORD) kNonDistort ;
		data.nShearAngle      = 0 ;
		data.nRotation        = 0 ;
		data.nAltPercent      = 0 ;
		data.nKerning         = 0 ;
	}

	return FALSE ;
}

//****************************************************************************
//
// Function Name: RHeadlinePositionPage::SelectResItem
//
// Description:   Callback function for iterating through the
//					   canned effect resources to determine the current
//                image selection
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
BOOL CALLBACK RHeadlinePositionPage::SelectResItem (
	HANDLE  hModule,			// resource-module handle 
	LPCTSTR /*lpszType*/,	// pointer to resource type 
	LPTSTR  lpszName,			// pointer to resource name 
	LONG    lParam) 			// application-defined parameter  
{
	RHeadlinePositionPage* pPage = (RHeadlinePositionPage*) lParam ;

	PositionPageData  data ;
	PositionPageData& classData = pPage->m_data ;
	pPage->LoadResData( (HMODULE) hModule, lpszName, data ) ;

	BOOLEAN fFound = TRUE ;

	if (classData.nSelComp != data.nSelComp)
	{
		fFound = FALSE ;
	}
	else if (kShear == classData.nDistortEffect)
	{
		fFound = (BOOLEAN)(
			data.nDistortEffect == classData.nDistortEffect &&
			data.nShearAngle    == classData.nShearAngle) ;
	}
	else if (kAltBaseline == classData.nDistortEffect)
	{
		fFound = (BOOLEAN)(
			data.nDistortEffect == classData.nDistortEffect &&
			data.nAltPercent    == classData.nAltPercent) ;
	}
	else if (kAltShearRotate == classData.nDistortEffect)
	{
		fFound = (BOOLEAN)(
			data.nDistortEffect == classData.nDistortEffect &&
			data.nShearAngle    == classData.nShearAngle    &&
			data.nRotation      == classData.nRotation) ;
	}

	if (fFound)
	{
		pPage->m_gcImageList.SetCurSel( int(lpszName) - 100 ) ;
	}

	return !fFound ;
}

//****************************************************************************
//
// Function Name: RHeadlinePositionPage::OnInitDialog
//
// Description:   Handles dialog initialization
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RHeadlinePositionPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	try
	{
		if (!m_ilImageList.Create( IDB_HEADLINE_POSITIONS, kImageWidth, kImageHeight, 0, CLR_NONE ))
		{
			throw kResource ;
		}

		m_gcImageList.SetImageList( &m_ilImageList, kPositionCount ) ;
		m_gcImageList.SizeCellsToClient( kNumRows, kNumCols ) ;

		EnumResourceNames( 
			AfxGetResourceHandle(),				// resource-module handling
			_T("HEADLINE_POSITION"),			// pointer to resource type 
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
// Function Name: RHeadlinePositionPage::OnSelChangeImageList
//
// Description:   Selection change handler for the image list
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlinePositionPage::OnSelChangeImageList( )
{
	int nCurCell = m_gcImageList.GetCurSel() ;

	PositionPageData data ;
	
	if (LoadResData( AfxGetResourceHandle(), MAKEINTRESOURCE( nCurCell + 100 ), data ) )
	{
		// Copy the selected compensation
		m_data.nSelComp = data.nSelComp ;
		m_data.nKerning = data.nKerning ;

		// Don't step on the distort mode unless it's one set in this page
		if (m_data.nDistortEffect >= kAltBaseline ||
			 data.nDistortEffect   >= kAltBaseline)
		{
			// Copy the distort method and data
			m_data.nDistortEffect = data.nDistortEffect ;
			m_data.nShearAngle    = data.nShearAngle ;
			m_data.nRotation      = data.nRotation ;
			m_data.nAltPercent    = data.nAltPercent ;
		}

		GetParent()->SendMessage( UM_HEADLINE_POSITION_CHANGE ) ;
	}
}
