//****************************************************************************
//
// File Name:   HeadlineShapePage.cpp
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Manages RHeadlineShapePage class which is used for the 
//				    Shape Property Page in the headline user interface.
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
//  $Logfile:: /PM8/App/HeadlineShapePage.cpp                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:06p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"
ASSERTNAME

#include "HeadlineCompResource.h"
#include "HeadlineShapePage.h"
#include "ComponentTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int kImageWidth  = 42 ;
const int kImageHeight = 43 ;
const int kNumRows     =  2 ;
const int kNumCols     =  7 ;

const int kShapesHorzCount       = 47 ;
const int kShapesVertCount       = 19 ;
const int kPathsHorzCount        = 9 ;
const int kPathsHorzSpecialCount = 1 ;
const int kPathsVertCount        = 1 ;

//	tables to convert between warp shapes in UI and the warp shape
//	resources the headline engine uses.
//	WARNING: Do NOT modify the layout of the bitmaps for the UI
//	without updating these tables.

//	Convert from index in UI to actual warp id for horizontal warp shapes
const sWORD kHorzShapePageToWarpShapeArray[kShapesHorzCount] =
{
	0, 4, 5, 6, 7, 2, 3, 9, 13, 10, 14, 11, 15, 8, 16, 20, 17, 21, 18, 22, 12, 19, 23, 24, 28, 26, 30, 40, 32, 36, 25, 29, 27, 31, 44, 33, 37, 34, 38, 41, 45, 35, 39, 42, 46, 43, 47/*, 49, 50, 48*/
};

//	Convert from index in UI to actual warp id for vertical warp shapes
const sWORD kVertShapePageToWarpShapeArray[kShapesVertCount] =
{
	51, 53, 52, 63, 62, 60, 59, 61, 68, 67, 55, 56, 58, 57, 65, 64, 54, 66, 69
};

//	Convert from index in UI to actual warp id for horizontal path shapes
const	sWORD kHorzShapePageToPathShapeArray[kPathsHorzCount] =
{
	3, 4, 5, 6, 7, 23, 24, 25, 26
};

/////////////////////////////////////////////////////////////////////////////
// RHeadlineShapePage property page

IMPLEMENT_DYNCREATE(RHeadlineShapePage, CDialog)

//*****************************************************************************
//
// Function Name:  RHeadlineShapePage::RHeadlineShapePage
//
// Description:    Constructor (Default).
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlineShapePage::RHeadlineShapePage() : CDialog(RHeadlineShapePage::IDD),
	m_btnColor( RWinColorBtn::kInset )
{
	//{{AFX_DATA_INIT(RHeadlineShapePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_yComponentType = kHeadlineComponent ;
	m_nTextLineCount = 1 ;
	m_uiImageList    = 0 ;
}

//*****************************************************************************
//
// Function Name:  RHeadlineShapePage::~RHeadlineShapePage
//
// Description:    Destructor 
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlineShapePage::~RHeadlineShapePage()
{
}

//*****************************************************************************
//
// Function Name: RHeadlineShapePage::DoDataExchange
//
// Description:   Method for transferning the contents of the child
//						controls to/from the data member variables.
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
void RHeadlineShapePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RHeadlineShapePage)
	DDX_Control( pDX, IDC_HORIZONTAL, m_rbHorizontal ) ;
	DDX_Control( pDX, IDC_VERTICAL, m_rbVertical ) ;
	DDX_Control( pDX, IDC_IMAGE_LIST, m_gcImageList );
	DDX_Control( pDX, IDC_COLOR, m_btnColor );
	DDX_Check( pDX, IDC_DISTORT, m_fDistort ) ;
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(RHeadlineShapePage, CDialog)
	//{{AFX_MSG_MAP(RHeadlineShapePage)
	ON_BN_CLICKED( IDC_COLOR, OnColor ) 
	ON_BN_CLICKED( IDC_DISTORT, OnDistortChange ) 
	ON_BN_CLICKED( IDC_HORIZONTAL, OnOrientationChange ) 
	ON_BN_CLICKED( IDC_VERTICAL, OnOrientationChange ) 
	ON_LBN_SELCHANGE( IDC_IMAGE_LIST, OnSelChangeImageList )	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: ApplyData
//
// Description:   Applies HeadlineDataStruct data to the page's
//						child controls
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineShapePage::ApplyData( RHeadlineInterface* pInterface )
{
	BOOLEAN fDistort ;
	pInterface->GetDistortEffect( m_eDistortEffect ) ;
	pInterface->GetShapeIndex( m_nSelShapeIdx ) ;
	pInterface->GetDistort( fDistort ) ;
	pInterface->GetEscapement( m_eEscapement ) ;
	m_fDistort = fDistort ;

	RColor crColor ;
	pInterface->GetBehindColor( crColor ) ;
	m_btnColor.SetColor( crColor ) ;

	if (GetSafeHwnd())
	{
		UpdateData( FALSE ) ;

		if (!UpdateControls())
		{
			// Let the user know there is a reason 
			// they're staring at a blank page.
			RAlert rAlert ;
			rAlert.AlertUser( STRING_ERROR_ACC_OUT_OF_MEMORY ) ;
		}
	}
}

//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills an HeadlineDataStruct with the page's 
//						current settings
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineShapePage::FillData( RHeadlineInterface* pInterface )
{
	RColor crColor( m_btnColor.GetColor() ) ;

	pInterface->SetDistortEffect( m_eDistortEffect ) ;
	pInterface->SetShapeIndex( m_nSelShapeIdx ) ;
	pInterface->SetDistort( m_fDistort ) ;
	pInterface->SetEscapement( m_eEscapement ) ;

	SetBehindColor( pInterface, crColor ) ;

	if (kFollowPath == m_eDistortEffect)
	{
		// Force the justifcation to center
		pInterface->SetJustification( kCenterJust ) ;
	}
	else if (m_eEscapement >= kTop2BottomLeft2Right)
	{
		// Force the justifcation to it's default value (left)
		pInterface->SetJustification( kCenterJust ) ;
	}

	EFramedEffects eBehindEffect ;
	pInterface->GetBehindEffect( eBehindEffect ) ;

	switch (eBehindEffect)
	{
	case kSimpleFrame:
	
		if (RColor::kTransparent == crColor.GetFillMethod())
		{
			pInterface->SetBehindEffect( kFrameNoFrame ) ;
		}

		break ;

	case kFrameNoFrame:

		if (RColor::kTransparent != crColor.GetFillMethod())
		{
			pInterface->SetBehindEffect( kSimpleFrame ) ;
		}

		break ;
	}

#if 0	
	//
	// If Emboss/Deboss is selected, changes to the
	// silouette color need to be reflected in the
	// face color
	//
	EOutlineEffects eOutlineEffect ;
	pInterface->GetOutlineEffect( eOutlineEffect ) ;

	if (kEmbossedOutline == eOutlineEffect || kDebossedOutline == eOutlineEffect)
	{
		pInterface->SetOutlineFillColor( crColor ) ;
	}
#endif
}

//****************************************************************************
//
// Function Name: SetBehindColor
//
// Description:   Sets the headlines behind color, as well as any interface
//						options that are related to the behind color.  For instance
//                the outline blur 2 color, as well as the shadow 2 color.
//                
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlineShapePage::SetBehindColor( RHeadlineInterface* pInterface, const RColor& crColor )
{
	// Set the headline's behind color
	pInterface->SetBehindColor( crColor ) ;

	//
	// If the behind color has changed, and the projection
	// fill color is a gradient, we need to update the
	// projection fill color to blend to the new
	// background color.
	//
	RColor crProjColor ;
	pInterface->GetProjectionFillColor( crProjColor ) ;

	RSolidColor crBlendArray[] = 	{ 
		crProjColor.GetSolidColor(),
		crColor.GetSolidColor() } ;

	if (RColor::kTransparent == crColor.GetFillMethod())
	{
		crBlendArray[1] = RSolidColor( kWhite ) ;
	}

	if (RColor::kGradient == crProjColor.GetFillMethod())
	{
		crProjColor = RColor( crBlendArray, 2, 0.0 ) ;
		pInterface->SetProjectionFillColor( crProjColor ) ;
	}

	pInterface->SetShadowColor2( crBlendArray[1] ) ;
	pInterface->SetBlur2Color( crBlendArray[1] ) ;
}

//****************************************************************************
//
// Function Name: UpdateControls
//
// Description:   Sets the controls state to match the current
//						page settings.
//
// Returns:       TRUE if successful; otherwise FALSE ;
//
// Exceptions:	   None
//
//****************************************************************************
BOOLEAN RHeadlineShapePage::UpdateControls()
{
	UINT uiItemChecked = IDC_HORIZONTAL ;

	if (kTop2BottomLeft2Right <= m_eEscapement )
	{
		uiItemChecked = IDC_VERTICAL ;
	}

	CheckRadioButton( IDC_HORIZONTAL, IDC_VERTICAL, uiItemChecked ) ;

	return UpdateImageList() ;
}

//****************************************************************************
//
// Function Name: UpdateImageList
//
// Description:   Sets the image list to macth the corresponding
//						distort and escapement methods
//
// Returns:       TRUE if successful; otherwise FALSE ;
//
// Exceptions:	   None
//
//****************************************************************************
BOOLEAN RHeadlineShapePage::UpdateImageList()
{		  
	UINT uiImageList ;
	int  nImageCount, i;
	int  nCurSel = 0 ;

	if ( !m_fDistort )										//	if path, not distort
	{
		if (kTop2BottomLeft2Right <= m_eEscapement )
		{
			uiImageList = IDB_HEADLINE_PATHS_VERT ;	//	vertical paths
			nImageCount = kPathsVertCount ;
			if(m_nSelShapeIdx == 0)							//	if invalid shape, set to first
				m_nSelShapeIdx = kNoWarpShape;
			nCurSel = 0;										//	only 1 vertical path
		}
		else
		{
			uiImageList = IDB_HEADLINE_PATHS ;			//	horizontal paths
			nImageCount	= kPathsHorzCount ;
			
			if (kSpecialHeadlineComponent         == m_yComponentType ||
				 kVerticalSpecialHeadlineComponent == m_yComponentType)
			{
				nImageCount = kPathsHorzSpecialCount ;
			}

			if(m_nSelShapeIdx < 3)							//	if invalid shape, set to first
				m_nSelShapeIdx = kHorzShapePageToPathShapeArray[0];
			for(i = 0; i < nImageCount; i++)
				if(m_nSelShapeIdx == kHorzShapePageToPathShapeArray[i])
					break;
			nCurSel = i;
			TpsAssert(i < nImageCount, "Horz Path not found in array");
		}
	}
	else															//	distort to warp shape, not follow path
	{
		if (kTop2BottomLeft2Right <= m_eEscapement )
		{
			uiImageList = IDB_HEADLINE_SHAPES_VERT ;	//	vertical warps
			nImageCount = kShapesVertCount ;
			if(m_nSelShapeIdx < kWarpShape110 /*== 0*/)	//	if invalid shape, set to first
				m_nSelShapeIdx = kVertShapePageToWarpShapeArray[0] + kWarpShape110;
			for(i = 0; i < kShapesVertCount; i++)
				if((m_nSelShapeIdx - kWarpShape110) == kVertShapePageToWarpShapeArray[i])
					break;
			nCurSel = i;
			TpsAssert(i < kShapesVertCount, "Vert warp shape not found in array");
		}
		else
		{
			uiImageList = IDB_HEADLINE_SHAPES ;			// horizontal warps
			nImageCount = kShapesHorzCount ;
			if(m_nSelShapeIdx < kWarpShape110 /*== 0*/)	//	if invalid shape, set to first
				m_nSelShapeIdx = kHorzShapePageToWarpShapeArray[0] + kWarpShape110;
			for(i = 0; i < kShapesHorzCount; i++)
				if((m_nSelShapeIdx - kWarpShape110) == kHorzShapePageToWarpShapeArray[i])
					break;
			nCurSel = i;
			TpsAssert(i < kShapesHorzCount, "Horz warp shape not found in array");
		}
	}

	TpsAssert(nCurSel >= 0, "Bad index value in headline UI");

	if (uiImageList != m_uiImageList)
	{
		if (!m_ilShapeList.Create( (uWORD) uiImageList, kImageWidth, kImageHeight, 0, CLR_NONE ))
		{
			m_gcImageList.SetImageList( &m_ilShapeList, 0 ) ;
			m_uiImageList = 0 ;

			return FALSE ;
		}

		// Update image list info
		m_uiImageList = uiImageList ;
		m_gcImageList.SetImageList( &m_ilShapeList, nImageCount ) ;
		m_gcImageList.UpdateWindow() ;
	}

	if (m_eDistortEffect > kWarpToEnvelop)
	{
		// Distort effect, set outside of this
		// control so set to default value.
		nCurSel = 0 ;
	}

	m_gcImageList.SetCurSel( max( nCurSel, 0 ) ) ;

	return TRUE ;
}

//*****************************************************************************
//
// Function Name:  OnInitDialog
//
// Description:    WM_INITDIALOG handler for the dialog
//
// Returns:        TRUE
//
// Exceptions:	    None
//
//*****************************************************************************
BOOL RHeadlineShapePage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	try 
	{
		if (!m_OrientationList.LoadImageList( IDB_HEADLINE_ORIENTATION, 26, RGB( 192, 192, 192 ) ))
		{
			throw kResource ;
		}

		m_rbHorizontal.SetImage( &m_OrientationList, 0 ) ;
		m_rbVertical.SetImage( &m_OrientationList, 1 ) ;

		m_gcImageList.SizeCellsToClient( kNumRows, kNumCols ) ;

		if (kSpecialHeadlineComponent         == m_yComponentType ||
			 kVerticalSpecialHeadlineComponent == m_yComponentType)
		{
			// Disable the orientation options
			m_rbHorizontal.EnableWindow( FALSE ) ;
			m_rbVertical.EnableWindow( FALSE ) ;
		}

		if (!UpdateControls())
		{
			throw kUnknownError ;
		}
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
void RHeadlineShapePage::OnColor() 
{
	RWinColorDlg dlg ;
	dlg.SetColor( m_btnColor.GetColor() ) ;

	if (IDOK == dlg.DoModal())
	{
		m_btnColor.SetColor( dlg.SelectedColor() ) ;
		GetParent()->PostMessage( UM_HEADLINE_SHAPE_CHANGE ) ;
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
void RHeadlineShapePage::OnSelChangeImageList( )
{
	int nCurCell = m_gcImageList.GetCurSel() ;

	//	if plain rectangle warp
	if (0 == nCurCell)	  
	{
		if (m_eDistortEffect < kAltBaseline)
		{
			// Change to non distort effect, if it was 
			// previously kWarpToEnvelop or kFollowPath
			m_eDistortEffect = kNonDistort ;
		}
	}
	else if (m_fDistort)	// Warp to shape
	{
		m_eDistortEffect = kWarpToEnvelop ;

		if (m_eEscapement == kTop2BottomLeft2Right)
			m_nSelShapeIdx = kVertShapePageToWarpShapeArray[nCurCell];
		else
			m_nSelShapeIdx = kHorzShapePageToWarpShapeArray[nCurCell];
		m_nSelShapeIdx += kWarpShape110;		//	add 30 to the base index value

	}
	else	// Follow path
	{
		m_eDistortEffect = kFollowPath ;

		if (m_eEscapement == kTop2BottomLeft2Right)
			m_nSelShapeIdx = kNoWarpShape ;
		else
			m_nSelShapeIdx = kHorzShapePageToPathShapeArray[nCurCell];
	}
	GetParent()->PostMessage( UM_HEADLINE_SHAPE_CHANGE ) ;
}

//*****************************************************************************
//
// Function Name:  OnDistortChange
//
// Description:    BN_CLICKED handler for the distort buttons
//
// Returns:        VOID
//
// Exceptions:	    None
//
//*****************************************************************************
void RHeadlineShapePage::OnDistortChange()
{
	BOOLEAN fDistort = m_fDistort ;
	UpdateData( TRUE ) ;

	if (fDistort != m_fDistort)
	{
		m_nSelShapeIdx = 0 ;

		// Reset the distort effect if it is warp to shape or follow path
		if (m_eDistortEffect < kAltBaseline)
		{
			m_eDistortEffect = kNonDistort ;
		}

		UpdateImageList() ; 
		GetParent()->PostMessage( UM_HEADLINE_SHAPE_CHANGE ) ;
	}
}

//*****************************************************************************
//
// Function Name:  OnOrientationChange
//
// Description:    BN_CLICKED handler for the orientation buttons
//
// Returns:        VOID
//
// Exceptions:	    None
//
//*****************************************************************************
void RHeadlineShapePage::OnOrientationChange()
{
	ETextEscapement eEscapement = kLeft2RightTop2Bottom ;

	if (m_rbVertical.GetCheck())
	{
		// We only want to support one line of vertical text, so
		// if the current headline text is greater then on line
		// display an error message notifying the user they have
		// to remove some of their text, and return without changing
		// to vertical text.
		if (m_nTextLineCount > 1)
		{
			// Display the error message
			RAlert rAlert ;
			rAlert.InformUser( STRING_HEADLINE_VERTICAL_TEXT_REQUIREMENT ) ;

			// Reset the radio button and return
			CheckRadioButton( IDC_HORIZONTAL, IDC_VERTICAL, IDC_HORIZONTAL ) ;
			return ;
		}

		eEscapement = kTop2BottomLeft2Right ;
	}

	if (m_eEscapement != eEscapement)
	{
		m_eEscapement    = eEscapement ;
		m_nSelShapeIdx   = 0 ;

		// Reset the distort effect if it is warp to shape or follow path
		if (m_eDistortEffect < kAltBaseline)
		{
			m_eDistortEffect = kNonDistort ;
		}

		UpdateImageList() ; 
		GetParent()->PostMessage( UM_HEADLINE_SHAPE_CHANGE ) ;
	}
}
