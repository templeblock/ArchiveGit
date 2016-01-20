//****************************************************************************
//
// File Name:   HeadlineOutlinePage.cpp
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Manages RHeadlindOutlinePage class which is used for the 
//				    Outline Property Page in the headline user interface.
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
//  $Logfile:: /PM8/App/HeadlineOutlinePage.cpp                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"
ASSERTNAME

#include "ComponentView.h"
#include "HeadlineCompResource.h"
#include "HeadlineOutlinePage.h"
#include "HeadlineShapePage.h"
#include "HeadlineInterface.h"
#include "SoftShadowSettings.h"
#include "SoftGlowSettings.h"
#include "WinColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int kImageWidth  = 42 ;
const int kImageHeight = 43 ;
const int kNumRows     =  2 ;
const int kNumCols     =  7 ;

const int kImageCount  = 15;

// TODO: change to configuration setting
static BOOLEAN fSoftEffectsSupported = FALSE;

/////////////////////////////////////////////////////////////////////////////
// RHeadlindOutlinePage property page

IMPLEMENT_DYNCREATE(RHeadlindOutlinePage, CDialog)

//*****************************************************************************
//
// Function Name:  RHeadlindOutlinePage::RHeadlindOutlinePage
//
// Description:    Constructor (Default).
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlindOutlinePage::RHeadlindOutlinePage() : CDialog(RHeadlindOutlinePage::IDD),
	m_btnColor( RWinColorBtn::kInset ),
	m_crHairline( kWhite ),
	m_fRestoreData( FALSE ),
	m_nGlowChecked( 0 )
{
	//{{AFX_DATA_INIT(RHeadlindOutlinePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
//
// Function Name:  RHeadlindOutlinePage::~RHeadlindOutlinePage
//
// Description:    Destructor 
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RHeadlindOutlinePage::~RHeadlindOutlinePage()
{
}

//*****************************************************************************
//
// Function Name:  RHeadlindOutlinePage::DoDataExchange
//
// Description:    Method for transferning the contents of the child
//						 controls to/from the data member variables.
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
void RHeadlindOutlinePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RHeadlindOutlinePage)
	DDX_Control( pDX, IDC_IMAGE_LIST, m_gcImageList );
	DDX_Control( pDX, IDC_COLOR, m_btnColor );
	DDX_Check( pDX, IDC_OUTLINE_GLOW, m_nGlowChecked );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RHeadlindOutlinePage, CDialog)
	//{{AFX_MSG_MAP(RHeadlindOutlinePage)
	ON_BN_CLICKED( IDC_COLOR, OnColor ) 
	ON_BN_CLICKED( IDC_OUTLINE_GLOW, OnGlow ) 
	ON_LBN_SELCHANGE( IDC_IMAGE_LIST, OnSelChangeImageList )	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: ApplyData
//
// Description:   Applies HeadlineDataStruct outline's data
//                to the Headline outline controls
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlindOutlinePage::ApplyData( RHeadlineInterface* pInterface, RComponentView* pComponentView, UINT /*uiHints*/ )
{
	//
	// Set the data
	//
	EOutlineEffects eOutlineEffect ;
	ELineWeight eOutlineWeight ;

	pInterface->GetOutlineEffect( eOutlineEffect ) ;
	pInterface->GetLineWeight( eOutlineWeight ) ;

	m_data.nOutlineEffect   = eOutlineEffect ;
	m_data.nLineWeight      = eOutlineWeight ;
	
	if (pComponentView->HasGlow())
		m_nGlowChecked = 1;

	pInterface->GetStrokeColor( m_crStroke ) ;

	switch (m_data.nOutlineEffect)
	{
	case kDoubleOutline:
	case kHairlineDoubleOutline:

		pInterface->GetShadowColor( m_crStroke ) ;
		break ;

	case kEmbossedOutline:
	case kDebossedOutline:
	case kBlurredOutline:
		{
//			RColor crEffectsColor ;
//			pInterface->GetOutlineFillColor( crEffectsColor ) ;
//			m_crStroke = crEffectsColor.GetSolidColor() ;
		}

		break ;
	}

	// Set the buttons color
	m_btnColor.SetColor( RColor( m_crStroke ) ) ;

/*	if (m_fRestoreData)
	{
		//
		// Keep up with the latest changes to these values,
		// in case we end up needing to restore them.
		//
		if (uiHints & kFaceChanged)
		{
			pInterface->GetOutlineFillColor( m_crFaceColor ) ;
		}

		if (uiHints & kShapeChanged)
		{
			pInterface->GetBehindColor( m_crBehindColor ) ;
		}

		if (uiHints & kDepthChanged && kBlurredOutline != m_data.nOutlineEffect)
		{
			// Note: If the restore data flag is set, that means that
			// the outline effect was blurred, emboss, or deboss.  Emboss
			// and Deboss are mutually exclusive with depths.  So if the
			// user selected a depth from emboss or deboss, the outline 
			// effect is cleared, and we want to stick with the then
			// current face and behind colors.   So, clear the restore flag.
			m_fRestoreData = FALSE ;
		}
	}
*/
	if (GetSafeHwnd())
	{
		// Set the image selection
		EnumResourceNames( 
			AfxGetResourceHandle(),				// resource-module handling
			_T("HEADLINE_OUTLINE"),			   // pointer to resource type 
			(ENUMRESNAMEPROC) SelectResItem,	// pointer to callback function 
			(LONG) this ) ; 						// application-defined parameter 

		BOOLEAN fDisabled = BOOLEAN(
			eOutlineEffect == kSimpleFill      ||
			eOutlineEffect == kEmbossedOutline ||
			eOutlineEffect == kDebossedOutline ||
			eOutlineEffect == kBeveledOutline  ||
			eOutlineEffect == kBlurredOutline );

		m_btnColor.EnableWindow( !fDisabled || m_nGlowChecked ) ;
	}
}

//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills an HeadlineDataStruct with the Headline 
//						position outine data
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlindOutlinePage::FillData( RHeadlineInterface* pInterface, RComponentView* pComponentView )
{
	EOutlineEffects eOldOutlineEffect ;
	pInterface->GetOutlineEffect( eOldOutlineEffect ) ;

	RSolidColor crOldStroke;
	pInterface->GetStrokeColor( crOldStroke ) ;

	if (kDoubleOutline == eOldOutlineEffect || kHairlineDoubleOutline == eOldOutlineEffect)
	{
		pInterface->GetShadowColor( crOldStroke ) ;
	}

	pInterface->SetStrokeColor( m_crStroke ) ;
	pInterface->SetShadowColor( m_crHairline ) ;
	pInterface->SetOutlineEffect( (EOutlineEffects) m_data.nOutlineEffect ) ;
	pInterface->SetLineWeight( (ELineWeight) m_data.nLineWeight ) ;

	// Only update the glow settings if the color has 
	// changed, or if the glow needs turned off or on.
	if (pComponentView->HasGlow() != m_nGlowChecked || crOldStroke != m_crStroke)
	{
		RSoftGlowSettings rGlowSettings;
		pComponentView->GetGlowSettings( rGlowSettings );

		if (m_nGlowChecked > 0)
		{
			// Set the color only if the color has changed, or glow is being turned on.
			if (crOldStroke != m_crStroke || !rGlowSettings.m_fGlowOn)
				rGlowSettings.m_fGlowColor = m_crStroke;	

			// Set the other settings only if 
			// the glow is not already on.
			if (!rGlowSettings.m_fGlowOn)
			{
				rGlowSettings.m_fGlowOpacity = 0.7;
				rGlowSettings.m_fGlowEdgeSoftness = 0.19;
			}
		}

		rGlowSettings.m_fGlowOn = BOOLEAN( m_nGlowChecked > 0 );
		pComponentView->SetGlowSettings( rGlowSettings );
	}

	switch (m_data.nOutlineEffect)
	{
	case kEmbossedOutline:
	case kDebossedOutline:
		{

			// Set the effects
/*			pInterface->SetBehindEffect( EFramedEffects(kEmboss +
				m_data.nOutlineEffect - kEmbossedOutline) ) ;

//			pInterface->SetOutlineEffect(	(EOutlineEffects)
//				m_data.nOutlineEffect ) ; 

			if (!m_fRestoreData)
			{
				// We are about to set some non-outline related
				// headline data.  So, store this information
				// so it can be restored by outline options that 
				// do not effect these options.
				pInterface->GetOutlineFillColor( m_crFaceColor ) ;
				pInterface->GetBehindColor( m_crBehindColor ) ;
				m_fRestoreData = TRUE ;  
			}

			// Update the face and behind colors to 
			// that of the outline color
			RColor crEffectsColor( m_crStroke ) ;
			pInterface->SetOutlineFillColor( crEffectsColor ) ;
			RHeadlineShapePage::SetBehindColor( pInterface, crEffectsColor ) ;
			ComputeEmbossDebossTints( pInterface ) ;
*/
			// Clear any depth effects
			pInterface->SetProjectionEffect( kNoProjection ) ;

			if (pComponentView->HasShadow())
			{
				RSoftShadowSettings settings;
				pComponentView->GetShadowSettings( settings );
				settings.m_fShadowOn = FALSE;
				pComponentView->SetShadowSettings( settings );
			}

			break ;
		}

	case kDoubleOutline:
	case kHairlineDoubleOutline:

		pInterface->SetStrokeColor( m_crHairline ) ;
		pInterface->SetShadowColor( m_crStroke ) ;

		break ; 

	case kBlurredOutline:
		{
/*			if (!m_fRestoreData)
			{
				// We are about to set some non-outline related
				// headline data.  So, store this information
				// so it can be restored by outline options that 
				// do not effect these options.  Note: even though
				// we only change the outline fill color, we need
				// to store the behind color too, as this value
				// gets used if any restoration is done.
				pInterface->GetOutlineFillColor( m_crFaceColor ) ;
				pInterface->GetBehindColor( m_crBehindColor ) ;
				m_fRestoreData = TRUE ;  
			}

			pInterface->SetOutlineFillColor( RColor( m_crStroke ) ) ;
*/
			// Clear any depth effects
			pInterface->SetProjectionEffect( kNoProjection ) ;
//			pInterface->SetShadowEffect( kNoShadow ) ;
			break ;
		}

	} // switch

/*	if (m_fRestoreData)
	{
		//
		// Restore each of the headline options that were
		// forced to different settings through the selection 
		// of kEmboss or kDeboss.
		//
		RHeadlineShapePage::SetBehindColor( pInterface, m_crBehindColor ) ;
		m_btnColor.SetColor( m_crStroke ) ;

		// We don't want to restore the color if the
		// blurred outline option was selected, as
		// this was just set in the switch statement above.
		// Also, we only reset the restore data flag if
		// the current selection is not kBlurredOutline,
		// because we have only done a partial restoration.
		if (kBlurredOutline != m_data.nOutlineEffect)
		{
			pInterface->SetOutlineFillColor( m_crFaceColor ) ;
			m_fRestoreData = FALSE ;  
		}
	}

	if(kEmbossedOutline == eOldOutlineEffect || kDebossedOutline == eOldOutlineEffect)
	{
		//
		// Reset the frame effect.  If the current color is transparent,
		// reset to kFrameNoFrame, otherwise reset to kSimpleFrame.
		//
		RColor crBehindColor ;
		pInterface->GetBehindColor( crBehindColor ) ;

		EFramedEffects eFrameEffect = RColor::kTransparent == 
			crBehindColor.GetFillMethod() ? kFrameNoFrame : kSimpleFrame ;
		pInterface->SetBehindEffect( eFrameEffect ) ;
	}
*/	
}

//****************************************************************************
//
// Function Name: LoadResData
//
// Description:   Fills a HeadlineOutlineData struct with the Headline 
//						outline data from a specified resource.
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RHeadlindOutlinePage::LoadResData( HMODULE hModule, LPTSTR lpszName, HeadlineOutlineData& data )
{
	HRSRC hResInfo = FindResource( hModule, lpszName, 
		_T("HEADLINE_OUTLINE") ) ;

	if (hResInfo)
	{
		HGLOBAL hResData = LoadResource( hModule, hResInfo ) ; 
		HeadlineOutlineData* lpData = (HeadlineOutlineData *) LockResource( hResData ) ;

		data = *lpData ;
		FreeResource( hResData ) ;

		return TRUE ;
	}
	else
	{
//		data.nOutlineFillType = 0 ;
		data.nOutlineEffect   = 0 ;
		data.nLineWeight      = 0 ;
	}

	return FALSE ;
}

//****************************************************************************
//
// Function Name: RHeadlindOutlinePage::SelectResItem
//
// Description:   Callback function for matching a resource to the
//						current data, and selecting the appropriate list 
//						item.
//
// Returns:       FALSE when the specified resource matches to stop
//						search; otherwise TRUE to continue search.
//
// Exceptions:	   None
//
//****************************************************************************
BOOL CALLBACK RHeadlindOutlinePage::SelectResItem (
	HANDLE  hModule,			// resource-module handle 
	LPCTSTR /*lpszType*/,	// pointer to resource type 
	LPTSTR  lpszName,			// pointer to resource name 
	LONG    lParam) 			// application-defined parameter  
{
	RHeadlindOutlinePage* pPage = (RHeadlindOutlinePage*) lParam ;

	HeadlineOutlineData data ;
	HeadlineOutlineData& classData = pPage->m_data ;

	pPage->LoadResData( (HMODULE) hModule, lpszName, data ) ;

	if (data.nOutlineEffect == classData.nOutlineEffect)
	{
		if (kSimpleFill		== classData.nOutlineEffect   ||
			 data.nLineWeight == classData.nLineWeight )
		{
			pPage->m_gcImageList.SetCurSel( int(lpszName) - 100 ) ;
			return FALSE ;
		}
	}

	return TRUE ;
}

//****************************************************************************
//
// Function Name: RHeadlindOutlinePage::OnInitDialog
//
// Description:   Handles dialog initialization
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RHeadlindOutlinePage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	try
	{
		if (!m_ilImageList.Create( IDB_HEADLINE_OUTLINES, kImageWidth, kImageHeight, 0, CLR_NONE ))
		{
			throw kResource ;
		}

		//
		// Set up the glow button
		//
		CButton* pWnd = (CButton*) GetDlgItem( IDC_OUTLINE_GLOW );

		if (pWnd)
		{
			HBITMAP hBitmap = LoadBitmap( AfxGetResourceHandle(),
				MAKEINTRESOURCE( IDB_HEADLINE_GLOW ));

			pWnd->SetBitmap( hBitmap );
		}

		//
		// Set up the image grid control
		//
		UINT nImageCount = (fSoftEffectsSupported ? kImageCount : kImageCount - 3);
		m_gcImageList.SetImageList( &m_ilImageList, nImageCount ) ;
		m_gcImageList.SizeCellsToClient( kNumRows, kNumCols ) ;

		EnumResourceNames( 
			AfxGetResourceHandle(),				// resource-module handling
			_T("HEADLINE_OUTLINE"),				// pointer to resource type 
			(ENUMRESNAMEPROC) SelectResItem,	// pointer to callback function 
			(LONG) this ) ; 						// application-defined parameter 

		BOOLEAN fDisabled = BOOLEAN(
			m_data.nOutlineEffect == kSimpleFill      ||
			m_data.nOutlineEffect == kEmbossedOutline ||
			m_data.nOutlineEffect == kDebossedOutline ||
			m_data.nOutlineEffect == kBeveledOutline  ||
			m_data.nOutlineEffect == kBlurredOutline );

		m_btnColor.EnableWindow( !fDisabled || m_nGlowChecked ) ;
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
// Function Name:  OnGlow
//
// Description:    BN_CLICKED handler for the glow button
//
// Returns:        VOID
//
// Exceptions:	    None
//
//*****************************************************************************
void RHeadlindOutlinePage::OnGlow() 
{
	UpdateData( TRUE );
	GetParent()->PostMessage( UM_HEADLINE_OUTLINE_CHANGE ) ;

	BOOLEAN fDisabled = BOOLEAN(
		m_data.nOutlineEffect == kSimpleFill      ||
		m_data.nOutlineEffect == kEmbossedOutline ||
		m_data.nOutlineEffect == kDebossedOutline ||
		m_data.nOutlineEffect == kBeveledOutline  ||
		m_data.nOutlineEffect == kBlurredOutline );

	m_btnColor.EnableWindow( !fDisabled || m_nGlowChecked ) ;
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
void RHeadlindOutlinePage::OnColor() 
{
	RWinColorDlg dlg(this, 0) ;
	dlg.SetColor( m_btnColor.GetColor() ) ;

	if (IDOK == dlg.DoModal())
	{
		m_btnColor.SetColor( dlg.SelectedColor() ) ;
		m_crStroke = dlg.SelectedSolidColor() ;

		GetParent()->PostMessage( UM_HEADLINE_OUTLINE_CHANGE ) ;
	}
}

//****************************************************************************
//
// Function Name: RHeadlindOutlinePage::OnSelChangeImageList
//
// Description:   Selection change handler for the image list
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlindOutlinePage::OnSelChangeImageList( )
{
	int nCurCell = m_gcImageList.GetCurSel() ;
	HeadlineOutlineData data ;
	
	if (LoadResData( AfxGetResourceHandle(), MAKEINTRESOURCE( nCurCell + 100 ), data ) )
	{
		m_data = data ;
		GetParent()->PostMessage( UM_HEADLINE_OUTLINE_CHANGE ) ;

		BOOLEAN fDisabled = BOOLEAN(
			m_data.nOutlineEffect == kSimpleFill      ||
			m_data.nOutlineEffect == kEmbossedOutline ||
			m_data.nOutlineEffect == kDebossedOutline ||
			m_data.nOutlineEffect == kBeveledOutline  ||
			m_data.nOutlineEffect == kBlurredOutline );

		m_btnColor.EnableWindow( !fDisabled || m_nGlowChecked ) ;
	}
}

//****************************************************************************
//
// Function Name: RHeadlindOutlinePage::ComputeEmbossDebossTints
//
// Description:   Selection change handler for the image list
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
void RHeadlindOutlinePage::ComputeEmbossDebossTints( RHeadlineInterface* pInterface )
{
	// Determine the lightness, which is the average of the RGB components in a scale from 0 to 1
	RColor crColor ;
	pInterface->GetBehindColor( crColor ) ;

	RSolidColor crSolid = crColor.GetSolidColor() ;
	YFloatType flLightness = (crSolid.GetRed() + crSolid.GetGreen() + crSolid.GetBlue()) / 3.0 / 255.0 ;

	// Set the shadow color
	pInterface->SetBehindShadowColor( RSolidColor(
		crSolid,
		RSolidColor( kBlack ),
		0.35 + (1.0 - flLightness) / 2) ) ;

	// Set the hilite color
	pInterface->SetBehindHilite( RSolidColor( 
		crSolid,
		RSolidColor( kWhite ),
		0.35 + (flLightness / 2 )) ) ;
}
