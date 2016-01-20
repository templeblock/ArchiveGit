//****************************************************************************
//
// File Name:   DepthPage.cpp
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Manages RHeadlineDepthPage class which is used for the 
//				    'Depth' Property Page in the headline user 
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
//  $Logfile:: /PM8/App/HeadlineDepthPage.cpp                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"
ASSERTNAME

#include "HeadlineCompResource.h"
#include "HeadlineDepthPage.h"
#include "HeadlineInterface.h"
#include "ComponentView.h"
#include "SoftShadowSettings.h"

const int kImageWidth  = 42 ;
const int kImageHeight = 43 ;
const int kNumRows     =  2 ;
const int kNumCols     =  7 ;
const int kImageCount  = 23 ;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const sWORD kDepthType        = 0 ;
const sWORD kBehindType       = 1 ;
const sWORD kShadowType       = 2 ;

// TODO: change to configuration setting
static BOOLEAN fShadowTabSupported   = FALSE;
static CString cResourceType         = _T("HEADLINE_DEPTH2");  // Do NOT localize this string;

BOOL CALLBACK ResCount( HANDLE, LPCTSTR, LPTSTR, LONG lParam )
{
	(*((LONG *) lParam))++;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// RHeadlineDepthPage property page

IMPLEMENT_DYNCREATE(RHeadlineDepthPage, CDialog)

//*****************************************************************************
//
// Function Name:  RHeadlineDepthPage::RHeadlineDepthPage
//
// Description:    Constructor (Default).
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlineDepthPage::RHeadlineDepthPage() : CDialog(RHeadlineDepthPage::IDD),
	m_btnColor( RWinColorBtn::kInset )
{
	//{{AFX_DATA_INIT(RHeadlineDepthPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_angle = 0.0 ;
}

//*****************************************************************************
//
// Function Name:  RHeadlineDepthPage::~RHeadlineDepthPage
//
// Description:    Destructor 
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlineDepthPage::~RHeadlineDepthPage()
{
}

//*****************************************************************************
//
// Function Name:  RHeadlineDepthPage::DoDataExchange
//
// Description:    Method for transferning the contents of the child
//				       controls to/from the data member variables.
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
void RHeadlineDepthPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RHeadlineDepthPage)
	DDX_Control( pDX, IDC_IMAGE_LIST, m_gcImageList );
	DDX_Control( pDX, IDC_COLOR, m_btnColor );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RHeadlineDepthPage, CDialog)
	//{{AFX_MSG_MAP(RHeadlineDepthPage)
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
void RHeadlineDepthPage::ApplyData( RHeadlineInterface* pInterface, RComponentView* pComponentView )
{
	// Set the effects data
	//////////////////////////

	EProjectionEffects eProjEffects ;
	EShadowEffects eShadowEffects ;
	RIntPoint ptVanish ;
	RColor crFillColor ;
	int nNumStages ;


	pInterface->GetNumStages( nNumStages ) ;
	pInterface->GetProjectionEffect( eProjEffects ) ;
	pInterface->GetProjectionDepth( m_data.flDepth ) ;
	pInterface->GetShadowEffect( eShadowEffects ) ;
	pInterface->GetProjectionFillColor( crFillColor ) ;
	pInterface->GetProjectionVanishPoint( ptVanish ) ;

	if (eShadowEffects > kNoShadow)
	{
		// Currently these options are shared, but in
		// pre-existing data they may not be.
		pInterface->GetShadowFillColor( crFillColor ) ;
		pInterface->GetShadowVanishPoint( ptVanish ) ;
	}

	if (pComponentView->HasShadow())
	{
		RSoftShadowSettings settings;
		pComponentView->GetShadowSettings( settings );

		m_data.nDataType      = kShadowType;
		m_data.nOpacity       = ::Round( settings.m_fShadowOpacity * 100 );
		m_data.nEdgeSoftness  = ::Round( settings.m_fShadowEdgeSoftness * 100 );
		m_data.nOffset        = ::Round( settings.m_fShadowOffset * 65535 );

		m_wndBlendAngle.SetAngle( -settings.m_fShadowAngle );
		m_angle = m_wndBlendAngle.GetAngle();

		m_btnColor.SetColor( RColor( settings.m_fShadowColor ) ) ;
	}
	else
	{
		m_data.nDataType      = kDepthType ;
		m_data.nNumStages     = nNumStages ;
		m_data.nSelProjection = eProjEffects ;
		m_data.nSelShadow     = eShadowEffects ;
		m_data.fBlend			 = sWORD( RColor::kGradient == crFillColor.GetFillMethod() ) ;

		m_wndBlendAngle.SetAngle( atan2( YRealDimension( ptVanish.m_y ), 
			YRealDimension( ptVanish.m_x ) ) ) ;

		m_btnColor.SetColor( RColor(crFillColor.GetSolidColor()) ) ;

		m_angle = m_wndBlendAngle.GetAngle()  ;

		YAngle angle = fmod((double)m_angle, (double)(2.0 * kPI));
		if (angle < 0.0) angle += 2.0 * kPI;
		const YRealDimension kAngleDelta = 0.00001;
		
		if (::AreFloatsEqual(angle, kPI / 2.0, kAngleDelta) || ::AreFloatsEqual(angle, 3.0 * kPI / 2.0, kAngleDelta))
			m_data.nVanishPointX = 0;
		else
			m_data.nVanishPointX = ptVanish.m_x / cos( m_angle ) ;
		
		if (::AreFloatsEqual(angle, 0.0, kAngleDelta) || ::AreFloatsEqual(angle, kPI, kAngleDelta))
			m_data.nVanishPointY = 0;
		else
			m_data.nVanishPointY = ptVanish.m_y / sin( m_angle ) ;

/*		if (m_data.nSelShadow > kNoShadow)
		{
			pInterface->GetShadowDepth( m_data.flDepth ) ;

			m_data.nDataType      = kShadowType;
			m_data.nOpacity       = 100;
			m_data.nEdgeSoftness  = 0;
			m_data.nOffset        = 10; // TODO: determine from vanishing point

			FillData( pInterface, pComponentView );
		}
*/	}


	if (GetSafeHwnd())
	{
		// Set the image selection
		EnumResourceNames( 
			AfxGetResourceHandle(),				// resource-module handling
			cResourceType,							// pointer to resource type 
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
void RHeadlineDepthPage::FillData( RHeadlineInterface* pInterface, RComponentView* pComponentView )
{
	//
	// Set the depth effects
	//

	if (m_data.nDataType == kShadowType)
	{
		// Clear any depth effects
		pInterface->SetProjectionEffect( kNoProjection ) ;

		RSoftShadowSettings settings;

		settings.m_fShadowOn           = TRUE;
		settings.m_fShadowOpacity      = m_data.nOpacity / 100.0;
		settings.m_fShadowEdgeSoftness = m_data.nEdgeSoftness  / 100.0;
		settings.m_fShadowOffset       = m_data.nOffset / 65535.0;

		settings.m_fShadowAngle = -m_wndBlendAngle.GetAngle();
		settings.m_fShadowColor = m_btnColor.GetColor().GetSolidColor();
		pComponentView->SetShadowSettings( settings );
	}
	else
	{
		if (pComponentView->HasShadow())
		{
			// Clear any shadow settings
			RSoftShadowSettings settings;
			pComponentView->GetShadowSettings( settings );
			settings.m_fShadowOn = FALSE;

			pComponentView->SetShadowSettings( settings );
		}

		RIntPoint ptVanish(
			int(cos(m_angle) * m_data.nVanishPointX + 0.5),
			int(sin(m_angle) * m_data.nVanishPointY + 0.5) ) ;

		pInterface->SetNumStages( m_data.nNumStages ) ;
		pInterface->SetProjectionEffect( (EProjectionEffects) m_data.nSelProjection ) ;
		pInterface->SetProjectionDepth( m_data.flDepth ) ;
		pInterface->SetProjectionVanishPoint( ptVanish ) ;
		pInterface->SetProjectionFillColor( m_btnColor.GetColor() ) ;

		pInterface->SetShadowEffect( (EShadowEffects) m_data.nSelShadow ) ;
		pInterface->SetShadowDepth( m_data.flDepth ) ;
		pInterface->SetShadowVanishPoint( ptVanish ) ;
		pInterface->SetShadowFillColor( m_btnColor.GetColor() ) ;

		if (m_data.fBlend && m_data.nSelProjection > kNoProjection)
		{
	//		EFramedEffects eFrameEffect ;
	//		pInterface->GetBehindEffect( eFrameEffect ) ;

			RColor crBlendColor ;
			pInterface->GetBehindColor( crBlendColor ) ;

			if (RColor::kTransparent == crBlendColor.GetFillMethod())
			{
				crBlendColor = RSolidColor( kWhite ) ;
			}

			RSolidColor crBlendArray[] = 	{ 
				m_btnColor.GetColor().GetSolidColor(),
				crBlendColor.GetSolidColor() } ;

			crBlendColor = RColor( crBlendArray, 2, m_angle ) ;
			pInterface->SetProjectionFillColor( crBlendColor ) ;
		}
	}

	if (m_data.nSelProjection > kNoProjection || m_data.nDataType == kShadowType)
	{
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
//				RColor crBehindColor ;
//				pInterface->GetBehindColor( crBehindColor ) ;
//				EFramedEffects eFrameEffect = (RColor::kTransparent == 
//					crBehindColor.GetFillMethod() ? kFrameNoFrame : kSimpleFrame) ;

				// Clear the outline effect
				pInterface->SetOutlineEffect( kSimpleFill ) ;

				// Set the appropriate behind effect.
//				pInterface->SetBehindEffect( eFrameEffect ) ;
			}
		}
	}
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
BOOL RHeadlineDepthPage::LoadResData( HMODULE hModule, LPTSTR lpszName, DepthPageData& data )
{
	HRSRC hResInfo = FindResource( hModule, lpszName, cResourceType ) ;

	if (hResInfo)
	{
		HGLOBAL hResData = LoadResource( AfxGetResourceHandle(), hResInfo ) ; 
		DepthPageData*  lpData = (DepthPageData *) LockResource( hResData ) ;

		if (kBehindType == lpData->nDataType)
		{
			data.nDataType     = lpData->nDataType ;
			data.nBehindEffect = lpData->nBehindEffect ;
		}
		else if (kShadowType == lpData->nDataType)
		{
			data.nDataType     = lpData->nDataType ;
			data.nOpacity      = lpData->nOpacity;
			data.nEdgeSoftness = lpData->nEdgeSoftness;
			data.nOffset       = lpData->nOffset;
		}
		else
		{
			data = *lpData ;
		}

		FreeResource( hResData ) ;
		return TRUE ;
	}
	else
	{
		memset( &data, 0, sizeof( data ) ) ;
	}

	return FALSE ;
}

BOOL CALLBACK RHeadlineDepthPage::SelectResItem (
	HANDLE  hModule,			// resource-module handle 
	LPCTSTR /*lpszType*/,	// pointer to resource type 
	LPTSTR  lpszName,			// pointer to resource name 
	LONG    lParam) 			// application-defined parameter  
{
	RHeadlineDepthPage* pPage = (RHeadlineDepthPage*) lParam ;

	DepthPageData  data ;
	DepthPageData& classData = pPage->m_data ;
	pPage->LoadResData( (HMODULE) hModule, lpszName, data ) ;

	BOOLEAN fFound = TRUE ;

	if (classData.nDataType != data.nDataType)
	{
		return TRUE ;  // Keep looking
	} 

	if (kBehindType == classData.nDataType)
	{
		fFound = BOOLEAN(	data.nBehindEffect == classData.nBehindEffect ) ;
	}
	else if (kShadowType == classData.nDataType)
	{
		fFound = BOOLEAN( 
			data.nOpacity      == classData.nOpacity      &&
			data.nEdgeSoftness == classData.nEdgeSoftness &&
			data.nOffset       == classData.nOffset );
	}
	else
	{

		if (classData.nSelProjection || classData.nSelShadow)
		{
			fFound = BOOLEAN(
				data.nSelProjection == classData.nSelProjection &&
				data.nSelShadow     == classData.nSelShadow     &&
				data.nNumStages     == classData.nNumStages     &&
				data.flDepth        == classData.flDepth        &&
				data.fBlend         == classData.fBlend) ;
		}

		if (fFound)
		{
			classData.nVanishPointX = data.nVanishPointX ;
			classData.nVanishPointY = data.nVanishPointY ;
		}
	}

	if (fFound)
	{
		pPage->m_gcImageList.SetCurSel( int(lpszName) - 100 ) ;
	}

	return !fFound ;
}

/////////////////////////////////////////////////////////////////////////////
// RHeadlineDepthPage message handlers

BOOL RHeadlineDepthPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	try
	{
		UINT uiDepthResId = IDB_HEADLINE_DEPTHS2;
		LONG nImageCount = 0;

		if (!fShadowTabSupported)
		{
			uiDepthResId  = IDB_HEADLINE_DEPTHS;
			cResourceType = _T("HEADLINE_DEPTH");  // Do NOT localize this string;
		}

		// Get the image count
		EnumResourceNames( 
			AfxGetResourceHandle(),				// resource-module handling
			cResourceType,							// pointer to resource type 
			(ENUMRESNAMEPROC) ResCount,		// pointer to callback function 
			(LONG) &nImageCount ) ; 			// application-defined parameter 

		if (nImageCount)
		{
			if (!m_ilImageList.Create( uiDepthResId, kImageWidth, kImageHeight, 0, CLR_NONE ))
			{
				throw kResource ;
			}


			//
			// Set up the image grid control
			//
			m_gcImageList.SetImageList( &m_ilImageList, nImageCount ) ;
			m_gcImageList.SizeCellsToClient( kNumRows, kNumCols ) ;

			EnumResourceNames( 
				AfxGetResourceHandle(),				// resource-module handling
				cResourceType,							// pointer to resource type 
				(ENUMRESNAMEPROC) SelectResItem,	// pointer to callback function 
				(LONG) this ) ; 						// application-defined parameter 
		}

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
// Function Name: RHeadlineDepthPage::OnSelChangeImageList
//
// Description:   Selection change handler for the image list
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineDepthPage::OnSelChangeImageList( )
{
	int nCurCell = m_gcImageList.GetCurSel() ;
	m_btnColor.EnableWindow( nCurCell != 0 ) ;

	DepthPageData data ;
	
	if (LoadResData( AfxGetResourceHandle(), MAKEINTRESOURCE( nCurCell + 100 ), data ) )
	{
		m_data = data ;
		GetParent()->PostMessage( UM_HEADLINE_DEPTH_CHANGE ) ;
	}
}

//****************************************************************************
//
// Function Name: RHeadlineDepthPage::OnAngleChange
//
// Description:   UM_ANGLE_CHANGE message handler
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
LRESULT RHeadlineDepthPage::OnAngleChange( WPARAM, LPARAM )
{
	m_angle = m_wndBlendAngle.GetAngle() ;
	GetParent()->PostMessage( UM_HEADLINE_DEPTH_CHANGE ) ;

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
void RHeadlineDepthPage::OnColor() 
{
	RWinColorDlg dlg(this, 0) ;
	dlg.SetColor( m_btnColor.GetColor() ) ;

	if (IDOK == dlg.DoModal())
	{
		m_btnColor.SetColor( dlg.SelectedColor() ) ;
		GetParent()->PostMessage( UM_HEADLINE_DEPTH_CHANGE ) ;
	}
}
