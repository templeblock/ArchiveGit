// ****************************************************************************
//
//  File Name:		TabbedEditArtEffect.cpp
//
//  Project:		Renaissance Application Component
//
//  Author:			Wynn Bailey / John Fleischhauer
//
//  Description:	Definition of the RTabbedEditArtEffect class
//
//	 Portability:	Win32
//
//	 Developed by:	Broderbund Software
//						500 Redwood Blvd
//						Novato, CA 94948
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
// ****************************************************************************

//#include "RenaissanceIncludes.h"
#include "stdafx.h"


ASSERTNAME

#include "TabbedEditArtEffect.h"
#include "TabbedEditImage.h"
#include "BitmapLoad.h"
//#include "ImageEditResource.h"
#include "PhotoWorkshopResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// first 7 are accusoft filters, sepia tone is different
/*	effects order	None = 0,
					Blur = 1	// we don't use this one
					Diffuse	= 2	// indexing should begin at this one
					Emboss = 3
					Noise = 4
					Pixelate = 5
					Posterize = 6
					Stitch = 7
					Sepia Tone = 8
*/

static int _nFirstArtEffectBmp		= IDB_ARTEFFECT_01;
static int _nLastArtEffectBmp		= IDB_ARTEFFECT_21;

static int _nFirstArtEffectStr		= IDS_AE_01;
static int _nLastArtEffectStr		= IDS_AE_21;


BEGIN_MESSAGE_MAP(RTabbedEditArtEffect, CDialog)
	//{{AFX_MSG_MAP(RTabbedEditArtEffect)
	// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_UNDO_BTN, OnUndoEffect)
	ON_BN_CLICKED(IDC_NO_ARTEFFECT, OnNoEffect)
	ON_LBN_SELCHANGE(IDC_EFFECTS_GRID, OnEffectSelChange)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE( WM_USER_APPLY_EDITS_TO_IED_BITMAP, OnApplyEditsToIEDBitmap )
	ON_MESSAGE( WM_USER_RESET_IED_TAB, OnResetUpdate )
	ON_MESSAGE( WM_USER_ON_OK_FROM_IED, OnOKFromIED )
	ON_MESSAGE( WM_USER_UNDO_FROM_IED, OnUndoFromIED )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::RTabbedEditArtEffect()
//
//  Description:		constructor
//
//  Returns:			nothing
//
// ****************************************************************************
//
RTabbedEditArtEffect::RTabbedEditArtEffect(CWnd* pParent /*=NULL*/)
	: CDialog(RTabbedEditArtEffect::IDD, pParent),
	  m_pPreviewComponent(NULL)
{
	m_nEffect = kDefaultArtisticEffect;
	m_nIntensityIndex = kMedium;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::~RTabbedEditArtEffect()
//
//  Description:		destructor
//
//  Returns:			nothing
//
// ****************************************************************************
//
RTabbedEditArtEffect::~RTabbedEditArtEffect()
{
	delete m_pPreviewComponent;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::PreTranslateMessage()
//
//  Description:		Handles various keystrokes for this dialog
//
//  Returns:			TRUE if the message is processed here
//
// ****************************************************************************
//
BOOL RTabbedEditArtEffect::PreTranslateMessage( MSG* pMsg )
{
	BOOL bRet = FALSE;

	// handle character messages
	if ( pMsg->message == WM_CHAR )
	{
		// Ctrl-Z = undo
		if (( ::GetAsyncKeyState( VK_CONTROL ) < 0 ) && pMsg->wParam == 26 )
		{
			OnUndoEffect();
			bRet = TRUE;
		}
	}
	else if ( pMsg->message == WM_KEYDOWN )
	{
		// Enter key processing
		if ( pMsg->wParam == VK_RETURN )
		{
			CWnd* pFocusWnd = GetFocus();
			if ( pFocusWnd == &m_cButtonUndo )
			{
				OnUndoEffect();
				bRet = TRUE;
			}
		}
	}

	if ( ! bRet )
	{
		bRet = CDialog::PreTranslateMessage( pMsg );
	}

	return bRet;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::OnInitDialog() 
//
//  Description:		Sets up the dialog.
//
//  Returns:			FALSE because we set the focus
//
// ****************************************************************************
//
BOOL RTabbedEditArtEffect::OnInitDialog() 
{
	// Call the base method
	BOOLEAN bRet = CDialog::OnInitDialog();
	if ( bRet )
	{
		// set up the grid control
		m_effectsGridCtrl.SizeCellsToClient( 2, 3 ) ;
		m_effectsGridCtrl.LoadBitmaps(_nFirstArtEffectBmp, _nLastArtEffectBmp);
		m_effectsGridCtrl.SetData(_nFirstArtEffectStr, _nLastArtEffectStr);
		m_effectsGridCtrl.SetCurSel(-1);	// no selection
		m_effectsGridCtrl.SetFocus();

	// Set bitmap button
//	AutoLoad fails under Win95 / Win98 with VC6 build!
//		m_cButtonUndo.AutoLoad( IDC_UNDO_BTN, this );
		bRet = m_cButtonUndo.LoadBitmaps( IDB_PHOTOWORKSHOP_UNDO_U, IDB_PHOTOWORKSHOP_UNDO_D, IDB_PHOTOWORKSHOP_UNDO_F, IDB_PHOTOWORKSHOP_UNDO_X );
		m_cButtonUndo.SizeToContent();
	}

	return bRet;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::OnShowWindow()
//
//  Description:		On entry, we set the preview image to match the current dlg image.
//							On exit we set the current dlg image to match the effects that
//							were selected.
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void RTabbedEditArtEffect::OnShowWindow(  BOOL bShow, UINT nStatus )
{
	if (bShow && ! nStatus) 
	{
		OnNoEffect();
		m_effectsGridCtrl.SetFocus();

		m_cButtonUndo.EnableWindow( FALSE );

		UpdatePreview();
	}
	//	When bShow is FALSE, we are switching to another tab; this is where we "burn" our changes
	//	into the local image component owned by the parent dialog
	else
	{
		// apply edits to the intermediate bitmap owned by the IED 
		OnApplyEditsToIEDBitmap( 0, 0 );

		// this forces the modified bitmap to be set into the IED component
		((RTabbedEditImage *)GetParent())->ApplyImageEffects();

		// handle the Revert and OK buttons in the parent, too
		//	if the undo button is enabled, then we have a change
		if ( m_cButtonUndo.IsWindowEnabled() )
		{
			// if we are leaving this tab with a change, lock the buttons enabled
			((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( TRUE, TRUE );
		}
		else
		{
			((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( FALSE );
		}
	}
}	
// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::OnUndoEffect()
//
//  Description:		Resores the preview window and intensity
//							to dlg entry conditions
//
//  Returns:			Nothing
//
// ****************************************************************************
void RTabbedEditArtEffect::OnUndoEffect()
{
	// Set the image to no artistic effect
	if(m_nEffect != kDefaultArtisticEffect)
	{
		OnNoEffect();
	}

	m_cButtonUndo.EnableWindow( FALSE );
	m_effectsGridCtrl.SetFocus();

	// handle the Revert and OK buttons in the parent, too
	((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( FALSE );
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::OnNoEffect()
//
//  Description:	no effect selected
//
//  Returns:			Nothing
//
// ****************************************************************************
void RTabbedEditArtEffect::OnNoEffect()
{
	m_nEffect = kDefaultArtisticEffect;
//	m_effectsGridCtrl.SetCurSel(-1);	// no selection in grid
	UpdatePreview();
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::OnEffectSelChange()
//
//  Description:		Keeps the preview window in sync with the
//							currently selected artistic effect.
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void RTabbedEditArtEffect::OnEffectSelChange()
{
	int		gridSel			= m_effectsGridCtrl.GetCurSel();
	uLONG	effectSel		= (uLONG)(gridSel / 3 + 2);// skipping none and blur
	int		intensitySetting = gridSel % 3;	// do a mod since we show strong, med, soft
	
	if( effectSel != m_nEffect || m_nIntensityIndex != intensitySetting )
	{
		m_nEffect = effectSel;	
		m_nIntensityIndex = intensitySetting;
		UpdatePreview();
		
		m_cButtonUndo.EnableWindow( TRUE );
		
		// handle the Revert and OK buttons in the parent, too
		((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( TRUE );
	}
}	


// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::DoDataExchange()
//
//  Description:		DDX/DDV support
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void RTabbedEditArtEffect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(REdgeEffectsDlg)
	DDX_Control(pDX, IDC_PREVIEW_AREA,	m_rImagePreviewCtrl);
	DDX_Control(pDX, IDC_EFFECTS_GRID,	m_effectsGridCtrl);
	DDX_Control(pDX, IDC_UNDO_BTN, m_cButtonUndo);
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::UpdatePreview()
//
//  Description:		Posts the preview image with the current effects
//
//  Returns:			nothing
//
// ****************************************************************************
void RTabbedEditArtEffect::UpdatePreview()
{
	// Get the preview component
	RTabbedEditImage *pParentDlg = (RTabbedEditImage *)GetParent();
	if(pParentDlg)
	{
		// Clean up prior to acquiring a new component (we don't accumlate effects)
		delete m_pPreviewComponent;
		m_pPreviewComponent = pParentDlg->GetPreviewCopyFromIEDComponent( &m_rImagePreviewCtrl );
	}
	RImageInterface* pImageInterface = dynamic_cast<RImageInterface*>(static_cast<RComponentView*>(m_pPreviewComponent->GetActiveView())->GetInterface(kImageInterfaceId));
	RImage* pImage = pImageInterface->GetImage();
	if(pImage)
	{
		RBitmapImage* rBitmap = dynamic_cast<RBitmapImage*>(pImage);
		if(rBitmap)
		{
			RBitmapImage *pModifiedBitmap = ApplyEffect( rBitmap );

			pImageInterface->SetImage( *pModifiedBitmap );

			//	ApplyLutColorEffect() creates a new bitmap
			//	we must delete it once it's been copied to the component
			if ( pModifiedBitmap != rBitmap )
			{
				delete pModifiedBitmap;
			}

			m_rImagePreviewCtrl.SetDocument(m_pPreviewComponent, FALSE);
		}
	}
	delete pImageInterface;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::ApplyEffect()
//
//  Description:		Applies the current effect to the RBitmapImage
//
//  Returns:		pointer to the modified bitmap (may or may not be the same
//					as the incoming bitmap
//
// ****************************************************************************
RBitmapImage *RTabbedEditArtEffect::ApplyEffect( RBitmapImage *pBitmap )
{
	RBitmapImage *pModifiedBitmap = pBitmap;
	if(m_nEffect <= kLastGearEffect)
	{
		ApplyGearEffect( *pBitmap );
		pModifiedBitmap = pBitmap;			// Accusoft does in place edits
	}			
	else if(m_nEffect == kSepiaEffect)
	{
		pModifiedBitmap = ApplyLutColorEffect( *pBitmap );
	}
	else
	{
		TpsAssertAlways("Invalid effects index");	
	}
	return pModifiedBitmap;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::ApplyGearEffect()
//
//  Description:		Applies ImageGear effect to the RBitmapImage
//
//  Returns:			nothing
//
// ****************************************************************************
void RTabbedEditArtEffect::ApplyGearEffect( RBitmapImage& rBitmap )
{
	// Set up the Strong, Medium and Light intensity levels.
	static const YArtisticEffect m_aIntensity[10][3] =	{
		{0,		0,		    0},	// defualt is no effect	
		{2,		2,		    1},	// blur (only two levels...
		{16,		10,		6},	// difusion 16 max
		{5,		 3,		2},	// emboss 5 is max
		{127,		 80,		40},	// noise 127 is max		 
		{7,		 12,		20},	// pixelate block size (storng = more)
		{3,		 5,		8},	// posterize to number of colors
		{4,		 3,		2}	   // emboss w/stich
	};

	uLONG nIntensity = m_aIntensity[m_nEffect][m_nIntensityIndex];
	RImageLibrary().ArtisticEffects(rBitmap, m_nEffect, nIntensity);
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::ApplyLutColorEffect()
//
//  Description:		Applies a color effect to the RBitmapImage and sets the image
//							into the interface. Currently the only effect is sepia tone.
//
//  Returns:			pointer to a new modified bitmap
//
// ****************************************************************************
RBitmapImage *RTabbedEditArtEffect::ApplyLutColorEffect( RBitmapImage& rBitmap )
{
	RBitmapImage *pDestBitmap = NULL;

	if(m_nEffect == kSepiaEffect)
	{
		YHueAngle	yCastAngle( 0 );
		YFloatType	yCastFraction( 0 );
		BOOL		bCastCoarseFlag( FALSE );
		YFloatType	yToneBrightness( 0 );
		YFloatType	yToneContrast( 0 );
		BOOL		bToneCoarseFlag( FALSE );
		switch(m_nIntensityIndex)
		{
			case kStrong:
				yCastAngle = 36;
				yCastFraction = .35;
				bCastCoarseFlag = TRUE;
				yToneBrightness = .24;		// lighten and
				yToneContrast = -.24;		// flatten
				bToneCoarseFlag = TRUE;
				break;
			case kMedium:
				yCastAngle = 30;
				yCastFraction = .4;
				bCastCoarseFlag = FALSE;
				yToneBrightness = 0;		// not used today
				yToneContrast = 0;			// "
				bToneCoarseFlag = FALSE;	// "
				break;
			case kLight:
				yCastAngle = 40;
				yCastFraction = .31;
				bCastCoarseFlag = FALSE;
				yToneBrightness = 0;		// not needed at all for kLight
				yToneContrast = 0;			//	"
				bToneCoarseFlag = FALSE;	//  "
				break;
			default:
				TpsAssertAlways("Bad case statement value");
			
		}
		RCastLUTs rLUTs(yCastAngle, yCastFraction, bCastCoarseFlag);
		
		try
		{
			if  ((yToneContrast != 0.0) || (yToneBrightness != 0.0))
			{	// inject an extra step to modify tonal range and brightness
				RBitmapImage *pTempBitmap = NULL;
				// create a temp destination bitmap
				pTempBitmap = new RBitmapImage( rBitmap);
				YLUTComponent *pRedLUT, *pGreenLUT, *pBlueLUT;
				// set the colors into the temp image
				RToneLUTs toneLUTs (yToneBrightness, yToneContrast, bToneCoarseFlag);
				toneLUTs.GetLUTs(&pRedLUT, &pGreenLUT, &pBlueLUT);
				RImageLibrary().ApplyLUTsRGB(rBitmap, *pTempBitmap, 
											pRedLUT, pGreenLUT, pBlueLUT);
				//create a destination bitmap
				pDestBitmap = new RBitmapImage( *pTempBitmap );
				// set the colors into the dest image
				rLUTs.GetLUTs(&pRedLUT, &pGreenLUT, &pBlueLUT);
				RImageLibrary().ConvertToGrayscale(*pTempBitmap);
				RImageLibrary().ApplyLUTsRGB(*pTempBitmap, *pDestBitmap, 
											pRedLUT, pGreenLUT, pBlueLUT);
				// clean up temp
				delete pTempBitmap;
			}
			else	// the simple case of grayscale and cast
			{
				// Create a destination bitmap
				pDestBitmap = new RBitmapImage( rBitmap );

				// Set the colors into the image
				YLUTComponent *pRedLUT, *pGreenLUT, *pBlueLUT;
				rLUTs.GetLUTs(&pRedLUT, &pGreenLUT, &pBlueLUT);
				RImageLibrary().ConvertToGrayscale(rBitmap);
				RImageLibrary().ApplyLUTsRGB(rBitmap, *pDestBitmap, 
											pRedLUT, pGreenLUT, pBlueLUT);
			}
		}
		catch (...)
		{
		}
	}

	return pDestBitmap;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::OnApplyEditsToIEDBitmap()
//
//  Description:	using the current settings, apply the edits to the
//					intermediate bitmap owned by the base IED dialog
//
//  Returns:		nothing
//
// ****************************************************************************
//
LONG RTabbedEditArtEffect::OnApplyEditsToIEDBitmap( WPARAM, LPARAM )
{
	RWaitCursor rWaitCursor;

	// get the intermediate bitmap owned by the IED
	RTabbedEditImage *pParentDlg = (RTabbedEditImage *)GetParent();
	RBitmapImage *pBitmap = pParentDlg->GetImage();
	if ( pBitmap )
	{
		RBitmapImage *pModifiedBitmap = ApplyEffect( pBitmap );

		// pass the modified bitmap back to the parent
		// Note: parent's SetImage() will cleanup old bitmap; but it does not
		//       copy the new one, so don't delete it!
		pParentDlg->SetImage( pModifiedBitmap );
	}				

	return 0;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::Reset()
//
//  Description:	Resets this tab's settings to their default state
//
//					This method is called when the Reset button on the base IED
//					is pressed.
//
//  Returns:		nothing
//
// ****************************************************************************
//
LONG RTabbedEditArtEffect::OnResetUpdate( WPARAM, LPARAM )
{
	// Set the image to no artistic effect
	OnNoEffect();
	m_effectsGridCtrl.SetFocus();

	// reset the undo button
	m_cButtonUndo.EnableWindow( FALSE );

	// reinitialize the window
	OnShowWindow( TRUE, 0 );

	return 0;
}

#if 0
// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::OnOK()
//
//  Description:		IDOK message handler
//
//  Returns:			nothing
//
// ****************************************************************************
//
void RTabbedEditArtEffect::OnOK()
{
	// do the same burn that we do when leaving the tab
	OnShowWindow( FALSE, 0 );

	CDialog::OnOK();
}
#endif

// ****************************************************************************
//
//  Function Name:	RTabbedEditArtEffect::OnOKFromIED()
//
//  Description:		handles M_USER_ON_OK_FROM_IED message, sent by parent
//							RTabbedEditImage dialog when user presses the OK button
//
//  Returns:			1 (A-OK)
//
// ****************************************************************************
//
LONG RTabbedEditArtEffect::OnOKFromIED( WPARAM, LPARAM )
{
	// do the same burn that we do when leaving the tab
	OnShowWindow( FALSE, 0 );
	return 1L;
}
