//****************************************************************************
//
// File Name:  HeadlineFacePage.cpp
//
// Project:    Renaissance user interface
//
// Author:     Lance Wilson
//
// Description:	Manages RHeadlineFacePage class which is used for the 
//						Face Property Page in the headline user 
//						interface.
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
//  $Logfile:: /PM8/App/HeadlineFacePage.cpp                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"
ASSERTNAME

#include "HeadlineCompResource.h"
#include "HeadlineFacePage.h"
#include "HeadlineShapePage.h"

const int kImageWidth  = 42 ;
const int kImageHeight = 43 ;
const int kNumRows     =  2 ;
const int kNumCols     =  7 ;
const int kImageCount  = 15 ;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RHeadlineFacePage property page

IMPLEMENT_DYNCREATE(RHeadlineFacePage, CDialog)

//*****************************************************************************
//
// Function Name:  RHeadlineFacePage::RHeadlineFacePage
//
// Description:    Constructor (Default).
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlineFacePage::RHeadlineFacePage() : CDialog(RHeadlineFacePage::IDD),
	m_btnColor( RWinColorBtn::kInset )
{
	//{{AFX_DATA_INIT(RHeadlineFacePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
//
// Function Name:  RHeadlineFacePage::~RHeadlineFacePage
//
// Description:    Destructor 
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlineFacePage::~RHeadlineFacePage()
{
}

//*****************************************************************************
//
// Function Name:  RHeadlineFacePage::DoDataExchange
//
// Description:    Method for transferning the contents of the child
//				   controls to/from the data member variables.
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
void RHeadlineFacePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RHeadlineFacePage)
	DDX_Control( pDX, IDC_IMAGE_LIST, m_gcImageList );
	DDX_Control( pDX, IDC_COLOR, m_btnColor );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RHeadlineFacePage, CDialog)
	//{{AFX_MSG_MAP(RHeadlineFacePage)
	ON_BN_CLICKED( IDC_COLOR, OnColor ) 
	ON_LBN_SELCHANGE( IDC_IMAGE_LIST, OnSelChangeImageList )	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: ApplyData
//
// Description:   Applies HeadlineDataStruct face data to the 
//						Headline face controls
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineFacePage::ApplyData( RHeadlineInterface* pInterface )
{
	pInterface->GetOutlineFillColor( m_crData ) ;
	pInterface->GetOutlineEffect( m_eOutlineEffect ) ;
	m_btnColor.SetColor( m_crData ) ;

	if (GetSafeHwnd())
	{
		m_gcImageList.SetCurSel( -1 ) ;		

		// Set the image selection
		EnumResourceNames( 
			AfxGetResourceHandle(),				// resource-module handling
			_T("HEADLINE_FACE"),					// pointer to resource type 
			(ENUMRESNAMEPROC) SelectResItem,	// pointer to callback function 
			(LONG) this ) ; 						// application-defined parameter 
	}
}

//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills an HeadlineDataStruct with the Headline 
//						face attribute data
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineFacePage::FillData( RHeadlineInterface* pInterface )
{
	pInterface->SetOutlineFillColor( m_crData ) ;

#if 0
	// 03/13/98 Lance - This distinction is on longer necessary

	//
	// If Emboss/Deboss outline is selected,  changes
	// to the face color need to be reflected in the
	// silouette color.
	//
	if (kEmbossedOutline == m_eOutlineEffect || kDebossedOutline == m_eOutlineEffect )
	{
		RHeadlineShapePage::SetBehindColor( pInterface, m_crData ) ;
	}

	else if (kBlurredOutline == m_eOutlineEffect && RColor::kGradient == m_crData.GetFillMethod())
	{
		// The user selected a gradient which is 
		// mutually exclusive with blurs, so
		// we need to clear the blur option
		pInterface->SetOutlineEffect( kSimpleFill ) ;
		m_eOutlineEffect = kSimpleFill ;
	}
#endif
}

//****************************************************************************
//
// Function Name: ROutlinePage::OnInitDialog
//
// Description:   Handles dialog initialization
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RHeadlineFacePage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	try
	{
		if (!m_ilImageList.Create( IDB_HEADLINE_FACES, kImageWidth, kImageHeight, 0, CLR_NONE ))
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
			_T("HEADLINE_FACE"),					// pointer to resource type 
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
void RHeadlineFacePage::OnColor() 
{
	uWORD uwFlags = kColorDefault ;

#if 0	
	// 03/13/98 Lance - This distinction is on longer necessary

	//
	// Make sure gradients aren't allowed for blurred outlines
	//
	if (kBlurredOutline  == m_eOutlineEffect)
	{
		uwFlags &= ~kAllowGradients ;
	}
#endif

	RWinColorDlg dlg(this, uwFlags) ;
	dlg.SetColor( m_btnColor.GetColor() ) ;

	if (IDOK == dlg.DoModal())
	{
		m_crData = dlg.SelectedColor() ;
		m_btnColor.SetColor( m_crData ) ;
//		if ( m_crData.GetFillMethod() == RColor::kGradient )
//		{
//			YAngle	angle = m_crData.m_pGradient->GetGradientAngle();
//			angle	= kPI - angle;
//			m_crData.m_pGradient->SetGradientAngle( angle );
//		}

		m_gcImageList.SetCurSel( -1 ) ;		

		EnumResourceNames( 
			AfxGetResourceHandle(),				// resource-module handling
			_T("HEADLINE_FACE"),					// pointer to resource type 
			(ENUMRESNAMEPROC) SelectResItem,	// pointer to callback function 
			(LONG) this ) ; 						// application-defined parameter 

		GetParent()->PostMessage( UM_HEADLINE_FACE_CHANGE ) ;
	}
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
void RHeadlineFacePage::OnSelChangeImageList( )
{
	int nCurCell = m_gcImageList.GetCurSel() ;

	FacePageData data ;
	
	if (LoadResData( AfxGetResourceHandle(), MAKEINTRESOURCE( nCurCell + 100 ), data ) )
	{
		YPercentage pctStartOffset = 0.0 ;
		
		if (data.nGradientType > 0)
		{
			BOOLEAN fInvert = BOOLEAN(
				data.nGradientType == kCircularBurst ||
				data.nGradientType == kSquareBurst) ;

			YAngle angle = data.nGradientAngle / 180.0 * kPI ;

			if (RColor::kGradient == m_crData.GetFillMethod())
			{
				uWORD uwGradientType ; 
				YAngle yGradientAngle ;
				RWinColorDlg::GetGradientInfo( m_crData, uwGradientType, yGradientAngle ) ;

				fInvert ^= BOOLEAN( 
					uwGradientType == kCircularBurst ||
					uwGradientType == kSquareBurst) ;

				RWinColorDlg::SetGradientInfo( m_crData, data.nGradientType, angle, fInvert ) ;
			}
			else
			{
				m_crData = RWinColorDlg::ColorToWhite(	m_crData.GetSolidColor( 
					pctStartOffset ), data.nGradientType, angle, fInvert ) ;
			}
		}
		else
		{
			if (RColor::kGradient == m_crData.GetFillMethod())
			{
				uWORD uwGradientType ; 
				YAngle yGradientAngle ;
				RWinColorDlg::GetGradientInfo( m_crData, uwGradientType, yGradientAngle ) ;

				if (uwGradientType == kCircularBurst || uwGradientType == kSquareBurst)
				{
					// Use the last color in the ramp
					// for non-linear blends.
					pctStartOffset = 100.0 ;
				}
			}

			m_crData	= m_crData.GetSolidColor( pctStartOffset ) ;
		}

		m_btnColor.SetColor( m_crData ) ;
		GetParent()->PostMessage( UM_HEADLINE_FACE_CHANGE ) ;
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
BOOL RHeadlineFacePage::LoadResData( HMODULE hModule, LPTSTR lpszName, FacePageData& data )
{
	HRSRC hResInfo = FindResource( hModule, lpszName, 
		_T("HEADLINE_FACE") ) ;

	if (hResInfo)
	{
		HGLOBAL hResData = LoadResource( AfxGetResourceHandle(), hResInfo ) ; 
		FacePageData* lpData = (FacePageData *) LockResource( hResData ) ;

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

BOOL CALLBACK RHeadlineFacePage::SelectResItem (
	HANDLE  hModule,			// resource-module handle 
	LPCTSTR /*lpszType*/,	// pointer to resource type 
	LPTSTR  lpszName,			// pointer to resource name 
	LONG    lParam) 			// application-defined parameter  
{
	RHeadlineFacePage* pPage = (RHeadlineFacePage*) lParam ;

	FacePageData data ;
	FacePageData classData ; 

	if (RColor::kGradient == pPage->m_crData.GetFillMethod())
	{
		YAngle angle ;
		RWinColorDlg::GetGradientInfo( pPage->m_crData,
			classData.nGradientType, angle ) ;

		if (classData.nGradientType == 4)
		{
			//
			// If it's dual linear limit the angle from -pi/2 to pi/2
			// as a dual linear gradient at an angle beyond this range 
			// is equal to the gradient blend of the opposite angle.
			// Remember angles are adujsted with a +pi.
			//
//			angle -=kPI ;

			if (angle <= -kPI / 2)
			{
				angle += kPI ;
			}
			else if (angle > kPI / 2)
			{
				angle -= kPI ;
			}

//			angle += kPI ;
		}

		// Convert radians to degrees
		if (angle > 0.0001 || angle < 0.0001)
		{
			YFloatType round = (angle > 0.0 ? 0.5 : -0.5) ;
			classData.nGradientAngle = (sWORD)(180 * angle / kPI + round) ;
		}
	}

	pPage->LoadResData( (HMODULE) hModule, lpszName, data ) ;

	BOOLEAN fFound = TRUE ;
	
	if (classData.nGradientType  != data.nGradientType)
	{
		fFound = FALSE ;
	}
	else if (classData.nGradientType == kLinear || classData.nGradientType == 4)
	{
		// Remember to account for classData.nGradientAngle being adjusted by +pi
		fFound = BOOLEAN(classData.nGradientAngle == (data.nGradientAngle /*+ 180*/)) ;
	}

	if (fFound)
	{
		pPage->m_gcImageList.SetCurSel( int(lpszName) - 100 ) ;
	}

	return !fFound ;
}
