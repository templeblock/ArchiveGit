// ****************************************************************************
//
//  File Name:		TabbedEditBrightFocus.cpp
//
//  Project:		Renaissance Application Component
//
//  Authors:			Bob Gotsch & John Fleischhauer
//
//  Description:	Definition of the RTabbedEditBrightFocus class
//
//	Portability:	Win32
//
//	Developed by:	Broderbund Software
//					500 Redwood Blvd
//					Novato, CA 94948
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
// ****************************************************************************

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME

#include "CastLUTs.h"
#include "ImageLibrary.h"

#include "TabbedEditBrightFocus.h"
#include "TabbedEditImage.h"
#include "BitmapLoad.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP(RTabbedEditBrightFocus, CDialog)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED( IDC_FINE_RADIO, OnFineRadio )
	ON_BN_CLICKED( IDC_COARSE_RADIO, OnCoarseRadio )
	ON_BN_CLICKED( IDC_UNDO_BTN, OnUndo )
	ON_MESSAGE( WM_USER_APPLY_EDITS_TO_IED_BITMAP, OnApplyEditsToIEDBitmap )
	ON_MESSAGE( WM_USER_RESET_IED_TAB, OnResetUpdate )
	ON_MESSAGE( WM_USER_ON_OK_FROM_IED, OnOKFromIED )
	ON_MESSAGE( WM_USER_UNDO_FROM_IED, OnUndoFromIED )
END_MESSAGE_MAP()


// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::RTabbedEditBrightFocus()
//
//  Description:	constructor
//
//  Returns:		nothing
//
// ****************************************************************************
//
RTabbedEditBrightFocus::RTabbedEditBrightFocus(CWnd* pParent /*=NULL*/)
	: CDialog(RTabbedEditBrightFocus::IDD, pParent),
	  m_pBeforeComponent(NULL),
	  m_pAfterComponent(NULL),
	  m_sharpness(0),
	  m_pUndoManager(NULL)
{
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::~RTabbedEditBrightFocus()
//
//  Description:	destructor
//
//  Returns:		nothing
//
// ****************************************************************************
//
RTabbedEditBrightFocus::~RTabbedEditBrightFocus()
{
	delete m_pBeforeComponent;
	delete m_pAfterComponent;
	delete m_pUndoManager;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::DoDataExchange()
//
//  Description:	DDX/DDV support
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditBrightFocus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BEFORE_AREA, m_rImageBeforeCtrl);
	DDX_Control(pDX, IDC_AFTER_AREA,  m_rImageAfterCtrl);
	DDX_Control(pDX, CONTROL_SLIDER_BRIGHTNESS, m_cBrightnessSlider);
	DDX_Control(pDX, CONTROL_SLIDER_CONTRAST, m_cContrastSlider);
	DDX_Control(pDX, CONTROL_SLIDER_SHARPNESS, m_cSharpnessSlider);
	DDX_Control(pDX, IDC_UNDO_BTN, m_cButtonUndo);
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocust::PreTranslateMessage()
//
//  Description:		Handles various keystrokes for this dialog
//
//  Returns:			TRUE if the message is processed here
//
// ****************************************************************************
//
BOOL RTabbedEditBrightFocus::PreTranslateMessage( MSG* pMsg )
{
	BOOL bRet = FALSE;

	// handle character messages
	if ( pMsg->message == WM_CHAR )
	{
		// Ctrl-Z = undo
		if (( ::GetAsyncKeyState( VK_CONTROL ) < 0 ) && pMsg->wParam == 26 )
		{
			OnUndo();
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
				OnUndo();
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
//  Function Name:	RTabbedEditBrightFocus::OnInitDialog() 
//
//  Description:	Sets up the dialog
//
//  Returns:		BOOL
//
// ****************************************************************************
//
BOOL RTabbedEditBrightFocus::OnInitDialog() 
{
	// Call the base method
	BOOL bRet = CDialog::OnInitDialog();

	if( bRet )
	{
		// our undo mechanism
		m_pUndoManager = new REditUndoManager();

	// Set bitmap buttons
//	AutoLoad fails under Win95 / Win98 with VC6 build!
//		m_cButtonUndo.AutoLoad( IDC_UNDO_BTN, this );
		bRet = m_cButtonUndo.LoadBitmaps( IDB_PHOTOWORKSHOP_UNDO_U, IDB_PHOTOWORKSHOP_UNDO_D, IDB_PHOTOWORKSHOP_UNDO_F, IDB_PHOTOWORKSHOP_UNDO_X );
		m_cButtonUndo.SizeToContent();
	}

	return bRet;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::OnShowWindow()
//
//  Description:	sets up display of the dialog window
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditBrightFocus::OnShowWindow( BOOL bShow, UINT nStatus )
{
	if (bShow && ! nStatus)
	{
		RWaitCursor rWaitCursor;

		// first time and every task in
		m_sharpness = 0;

		// Set up slider controls
		m_cBrightnessSlider.SetRange(-100, 100, TRUE);
	//	m_cBrightnessSlider.SetTic (-100);
	//	m_cBrightnessSlider.SetTic (100);
	//	m_cBrightnessSlider.SetTic (0);			// default center
		m_cBrightnessSlider.SetTicFreq (100);
		m_cBrightnessSlider.SetPageSize (2);	// 100 steps
		m_cBrightnessSlider.SetPos (0);			// center every time into tab set back to zero

		m_cContrastSlider.SetRange (-100, 100, TRUE);
	//	m_cContrastSlider.SetTic (-100);
	//	m_cContrastSlider.SetTic (100);
	//	m_cContrastSlider.SetTic (0);			// default center
		m_cContrastSlider.SetTicFreq (100);
		m_cContrastSlider.SetPageSize (2);		// 100 steps
		m_cContrastSlider.SetPos (0);			// center every time into tab set back to zero

		m_cSharpnessSlider.SetRange(kMinSharpness, kMaxSharpness, TRUE);	// -4, 4
		m_cSharpnessSlider.SetTic(kMinSharpness);
		m_cSharpnessSlider.SetTic(kMaxSharpness);
		m_cSharpnessSlider.SetTic(kDefaultSharpness);
		m_cSharpnessSlider.SetPageSize(kSharpnessIncrement);				// 8 steps
		m_cSharpnessSlider.SetPos(kDefaultSharpness);

		if (m_rLUTs.IsCoarse())
		{
			((CButton *)GetDlgItem( IDC_COARSE_RADIO ))->SetCheck( 1 );
			((CButton *)GetDlgItem( IDC_FINE_RADIO ))->SetCheck( 0 );
		} 
		else
		{
			((CButton *)GetDlgItem( IDC_FINE_RADIO ))->SetCheck( 1 );
			((CButton *)GetDlgItem( IDC_COARSE_RADIO ))->SetCheck( 0 );
		}

		m_rLUTs.SetBrightnessFraction( 0.0 );
		m_rLUTs.SetContrastFraction( 0.0 );
		m_rLUTs.BuildLUTs();

		//	get the Before preview component;
		//	Note: the After component is created in ApplyLUTsToPreviewControls()
		RTabbedEditImage *pParentDlg = (RTabbedEditImage *)GetParent();
		if ( pParentDlg )
		{
			delete m_pBeforeComponent;
			m_pBeforeComponent = pParentDlg->GetPreviewCopyFromIEDComponent( &m_rImageBeforeCtrl );
		}

		UpdatePreviews();

		UpdateUndoButton();
	}
	else
	{	// when bShow is FALSE, this is when we are switching to another tab;
		// this is where we "burn" our changes into the local component 
		// owned by the parent dialog
		// is there something to do?
		if (	m_sharpness ||
				m_rLUTs.GetBrightnessFraction() ||
				m_rLUTs.GetContrastFraction())
		{
			// apply edits to the intermediate bitmap owned by the IED 
			OnApplyEditsToIEDBitmap( 0, 0 );

			// this forces the modified bitmap to be set into the IED component
			((RTabbedEditImage *)GetParent())->ApplyImageEffects();
		}

		// handle the Revert and OK buttons in the parent, too
		YCounter rUndoCount = m_pUndoManager->CountUndoableActions();
		if ( rUndoCount > 0 )
		{
			// if we are leaving this tab with a change, lock the buttons enabled
			((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( TRUE, TRUE );
		}
		else
		{
			((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( FALSE );
		}
	
		// purge the undo buffer
		m_pUndoManager->FreeAllActions();
	}
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::UpdatePreviews()
//
//  Description:	refreshes the preview windows with new info
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditBrightFocus::UpdatePreviews()
{
	// apply tables gets a fresh copy of the beforeComponent before adjusting color
	ApplyLUTsToPreviewControls();
	// so apply sharpen continues to modify that copy
	if (m_sharpness)
		ApplySharpen ( m_pAfterComponent, m_sharpness);

	// update the preview control windows
	m_rImageBeforeCtrl.SetDocument(m_pBeforeComponent, FALSE);
	m_rImageAfterCtrl.SetDocument(m_pAfterComponent, FALSE);
}


void RTabbedEditBrightFocus::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_ENDSCROLL)
	{
		YFloatType brightness, contrast;
		YSharpness sharpness;
		brightness = static_cast<YBrightness>(m_cBrightnessSlider.GetPos());// -100 to +100
		brightness *= .01;													// -1.0 to +1.0
		contrast =	static_cast<YContrast>(m_cContrastSlider.GetPos());		// -100 to +100
		contrast *= .01;													// -1.0 to +1.0
		sharpness =	static_cast<YSharpness>(m_cSharpnessSlider.GetPos());	// -4 to +4

		// Create the undo action for changing the image.
		// Note, this action is an undo only.  It saves the current
		// state variables, and will use them to restore on undo. 
		// Any changes to be done to the image need to be done manually.
		if ( m_pUndoManager )
		{
			RBrightFocusAction::EActionType eAction = RBrightFocusAction::kNone;
			switch ( pScrollBar->GetDlgCtrlID() )
			{
				case CONTROL_SLIDER_BRIGHTNESS:
					eAction = RBrightFocusAction::kBrightness;
					break;

				case CONTROL_SLIDER_CONTRAST:
					eAction = RBrightFocusAction::kContrast;
					break;

				case CONTROL_SLIDER_SHARPNESS:
					eAction = RBrightFocusAction::kSharpness;
					break;
		
				default:
					TpsAssert( 0, "Unknown action type in OnHScroll!" );
					break;
			}

			RUndoableAction* pAction = new RBrightFocusAction( this, eAction, m_rLUTs.GetBrightnessFraction(), m_rLUTs.GetContrastFraction(), m_sharpness );
			m_pUndoManager->SendAction( pAction );
			UpdateUndoButton();
		}

		m_sharpness = sharpness;

		if (UpdateLUTs (brightness, contrast))
		{
			UpdatePreviews();
		}
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


// Unconditionally re-BuildLuts and UpdatePreviews upon change in coarseness
void RTabbedEditBrightFocus::OnFineRadio()
{
	m_rLUTs.Coarse( FALSE );

	// set up for undo
	if ( m_pUndoManager )
	{
		RUndoableAction* pAction = new RBrightFocusAction( this, RBrightFocusAction::kCoarse, 0, 0, 0 );
		m_pUndoManager->SendAction( pAction );
		UpdateUndoButton();
	}

	if ( m_rLUTs.BuildLUTs() )
	{
		UpdatePreviews();
	}	
}

// Unconditionally re-BuildLuts and UpdatePreviews upon change in coarseness
void RTabbedEditBrightFocus::OnCoarseRadio()
{
	m_rLUTs.Coarse( TRUE );

	// set up for undo
	if ( m_pUndoManager )
	{
		RUndoableAction* pAction = new RBrightFocusAction( this, RBrightFocusAction::kCoarse, 0, 0, 0 );
		m_pUndoManager->SendAction( pAction );
		UpdateUndoButton();
	}

	if ( m_rLUTs.BuildLUTs() )
	{
		UpdatePreviews();
	}
}

// Conditionally re-BuildsLUTs upon change in brightness or contrast
BOOL RTabbedEditBrightFocus::UpdateLUTs(YFloatType brightnessFraction, YFloatType contrastFraction)
{
	if ( ( m_rLUTs.GetBrightnessFraction() != brightnessFraction) ||
			 (m_rLUTs.GetContrastFraction() != contrastFraction))
	{	
		m_rLUTs.SetBrightnessFraction(brightnessFraction);
		m_rLUTs.SetContrastFraction(contrastFraction);
		return (m_rLUTs.BuildLUTs ());
	}
	else
		return TRUE;
}


BOOL RTabbedEditBrightFocus::ApplyLUTsToPreviewControls()
{
	// get a new preview copy from our already-scaled down base image copy
	delete m_pAfterComponent;
	RTabbedEditImage *pParentDlg = (RTabbedEditImage *)GetParent();
	m_pAfterComponent = pParentDlg->GetPreviewComponentCopy( m_pBeforeComponent, &m_rImageAfterCtrl );

	// adjust it
	return ApplyLUTsToComponent( m_pAfterComponent, &m_rLUTs );
}

BOOL RTabbedEditBrightFocus::ApplySharpen(RComponentDocument *pComponent, YSharpness sharpness)
{
	BOOL bRet = FALSE;
	RImageInterface* pImageInterface = dynamic_cast<RImageInterface*>(static_cast<RComponentView *>(pComponent->GetActiveView())->GetInterface(kImageInterfaceId));
	RImage *pImage = pImageInterface->GetImage();
	if ( pImage )
	{
		RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>(pImage);
		if( pBitmap )
		{
			RImageLibrary().Sharpen(*pBitmap, sharpness);
			if (m_rLUTs.IsCoarse())
			{	// this is a really ugly kludge to fake a coarse setting by multiple sharpening
				if (sharpness < 0)
				{
					RImageLibrary().Sharpen(*pBitmap, -2);
					if (sharpness == -3)
						RImageLibrary().Sharpen(*pBitmap, -2);
					if (sharpness == -4)
						RImageLibrary().Sharpen(*pBitmap, -4);
				}
				if (sharpness > 0)
					RImageLibrary().Sharpen(*pBitmap, 4);
			}
		}
		bRet = TRUE;
	}
	// clean up the interface
	delete pImageInterface;

	return bRet;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::ApplyLUTsToComponent
//
//  Description:	applies lookup tables to image within a component
//
//  Returns:		TRUE if successful, False on error
//
// ****************************************************************************
//
BOOL RTabbedEditBrightFocus::ApplyLUTsToComponent( RComponentDocument *pComponent, RToneLUTs *pLUTs )
{
	BOOL bRet = FALSE;
	RImageInterface* pImageInterface = dynamic_cast<RImageInterface*>(static_cast<RComponentView *>(pComponent->GetActiveView())->GetInterface(kImageInterfaceId));
	RImage *pImage = pImageInterface->GetImage();
	if ( pImage )
	{
		RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>(pImage);
		if( pBitmap )
		{
			// create a destination bitmap
			RBitmapImage rNewBitmap( *pBitmap );

			ApplyLUTsToBitmap( pBitmap, &rNewBitmap, pLUTs );

			pImageInterface->SetImage( rNewBitmap );
					
			bRet = TRUE;
		}
	}
	// clean up the interface
	delete pImageInterface;

	return bRet;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::ApplyLUTsToBitmap()
//
//  Description:	calls Image Library routine to apply LUTs to the bitmap
//
//  Returns:		nothing, but pDestBitmap is filled with the modified data
//
// ****************************************************************************
//
void RTabbedEditBrightFocus::ApplyLUTsToBitmap( RBitmapImage *pSrcBitmap, RBitmapImage *pDestBitmap, RToneLUTs *pLUTs )
{
	TpsAssert( pSrcBitmap,  "Bad source image in ApplyLUTsToBitmapImage()" ); 
	TpsAssert( pDestBitmap, "Bad destination image in ApplyLUTsToBitmapImage()" ); 

	TpsAssert( pSrcBitmap->GetWidthInPixels()  == pDestBitmap->GetWidthInPixels(),  "DestBitmap not same size as SrcBitmap." );
	TpsAssert( pSrcBitmap->GetHeightInPixels() == pDestBitmap->GetHeightInPixels(), "DestBitmap not same size as SrcBitmap." );

	// get the look up tables
	YLUTComponent *pRedLUT, *pGreenLUT, *pBlueLUT;
	pLUTs->GetLUTs( &pRedLUT, &pGreenLUT, &pBlueLUT );

	RImageLibrary().ApplyLUTsRGB( *pSrcBitmap, *pDestBitmap, pRedLUT, pGreenLUT, pBlueLUT );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::ProcessUndo()
//
//  Description:	callback invoked by the undo manager
//					this is where the undo really happens
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditBrightFocus::ProcessUndo( RBrightFocusAction::EActionType eAction, YFloatType yBrightness, YFloatType yContrast, YSharpness ySharpness)
{
	BOOLEAN bOK = FALSE;
	RWaitCursor rWaitCursor;

	switch ( eAction )
	{
		case RBrightFocusAction::kBrightness:
		{
			if ( UpdateLUTs( yBrightness, m_rLUTs.GetContrastFraction() ) )
			{
				bOK = TRUE;
			}
			m_cBrightnessSlider.SetPos( m_rLUTs.GetBrightnessFraction() * 100.0 );

			break;
		}

		case RBrightFocusAction::kContrast:
		{
			if ( UpdateLUTs( m_rLUTs.GetBrightnessFraction(), yContrast ) )
			{
				bOK = TRUE;
			}
			m_cContrastSlider.SetPos( m_rLUTs.GetContrastFraction() * 100.0 );
			break;
		}

		case RBrightFocusAction::kSharpness:
		{
			m_sharpness = ySharpness;

			if ( UpdateLUTs( m_rLUTs.GetBrightnessFraction(), m_rLUTs.GetContrastFraction() ) )
			{
				bOK = TRUE;
			}
			m_cSharpnessSlider.SetPos( m_sharpness );
			break;
		}

		case RBrightFocusAction::kCoarse:
		{
			// reverse the flag
			if ( m_rLUTs.IsCoarse() )
			{
				m_rLUTs.Coarse( FALSE );
				((CButton *)GetDlgItem( IDC_FINE_RADIO ))->SetCheck( 1 );
				((CButton *)GetDlgItem( IDC_COARSE_RADIO ))->SetCheck( 0 );
			}
			else
			{
				m_rLUTs.Coarse( TRUE );
				((CButton *)GetDlgItem( IDC_COARSE_RADIO ))->SetCheck( 1 );
				((CButton *)GetDlgItem( IDC_FINE_RADIO ))->SetCheck( 0 );
			}
	
			if ( m_rLUTs.BuildLUTs() )
			{
				bOK = TRUE;
			}
			break;
		}

		default:
			TpsAssert( 0, "Unknown action type in Undo!" );
			break;
	}

	if ( bOK )
	{
		UpdatePreviews();
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::OnUndo()
//
//  Description:	responds to the Undo button being pressed; invokes undo of the last action
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditBrightFocus::OnUndo()
{
	if ( m_pUndoManager )
	{
		// call the undo manager to get the last undoable action
		// the undo manager will call out ProcessUndo() callback
		m_pUndoManager->UndoAction();
		UpdateUndoButton();
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::UpdateUndoButton()
//
//  Description:	enables or disables the Undo button
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditBrightFocus::UpdateUndoButton()
{
	if ( m_pUndoManager )
	{
		uWORD nID = m_pUndoManager->GetUndoStringId();
		if ( nID != STRING_CANT_UNDO )
		{
			m_cButtonUndo.EnableWindow( TRUE );

			// handle the Revert and OK buttons in the parent, too
			((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( TRUE );
		}
		else
		{
			if ( GetFocus() == &m_cButtonUndo )
			{
				m_cBrightnessSlider.SetFocus();
			}
			m_cButtonUndo.EnableWindow( FALSE );

			// handle the Revert and OK buttons in the parent, too
			((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( FALSE );
		}
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::OnApplyEditsToIEDBitmap()
//
//  Description:	using the current settings, apply the edits to the
//					intermediate bitmap owned by the base IED dialog
//
//  Returns:		nothing
//
// ****************************************************************************
//
LONG RTabbedEditBrightFocus::OnApplyEditsToIEDBitmap( WPARAM, LPARAM )
{
	RWaitCursor rWaitCursor;

	// get pointer to parent IED dialog
	RTabbedEditImage *pParentDlg = (RTabbedEditImage *)GetParent();
	RBitmapImage *pBitmap;

	// apply the effects where appropriate
	if (m_rLUTs.GetBrightnessFraction() || m_rLUTs.GetContrastFraction())
	{
		// get the intermediate bitmap owned by the IED
		pBitmap = pParentDlg->GetImage();
	
		// create a destination bitmap
		RBitmapImage *pNewBitmap = new RBitmapImage( *pBitmap );
		if ( pNewBitmap )
		{
			ApplyLUTsToBitmap( pBitmap, pNewBitmap, &m_rLUTs);
					
			// pass the modified bitmap back to the parent
			// Note: parent's SetImage() will cleanup old bitmap; but it does not
			//       copy the new one, so don't delete it!
			pParentDlg->SetImage( pNewBitmap );
		}
	}

	if (m_sharpness)
	{
		// get the intermediate bitmap owned by the IED
		pBitmap = pParentDlg->GetImage();

		RImageLibrary().Sharpen(*pBitmap, m_sharpness);
		if (m_rLUTs.IsCoarse())
		{	// this is a kludge to fake a coarse setting by a 2nd sharpening
			if (m_sharpness < 0)
				RImageLibrary().Sharpen(*pBitmap, -2);
			if (m_sharpness > 0)
				RImageLibrary().Sharpen(*pBitmap, 4);
		}
	}

	return 0;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::Reset()
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
LONG RTabbedEditBrightFocus::OnResetUpdate( WPARAM, LPARAM )
{
	// reset LUTs
	m_rLUTs.Coarse( FALSE );
	m_rLUTs.SetBrightnessFraction( 0.0 );
	m_rLUTs.SetContrastFraction( 0.0 );
	m_rLUTs.BuildLUTs();

	// purge the undo buffer
	m_pUndoManager->FreeAllActions();

	// reinitialize the window
	OnShowWindow( TRUE, 0 );

	return 0;
}

#if 0
// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::OnOK()
//
//  Description:		IDOK message handler
//
//  Returns:			nothing
//
// ****************************************************************************
//
void RTabbedEditBrightFocus::OnOK()
{
	// do the same burn that we do when leaving the tab
	OnShowWindow( FALSE, 0 );

	CDialog::OnOK();
}
#endif

// ****************************************************************************
//
//  Function Name:	RTabbedEditBrightFocus::OnOKFromIED()
//
//  Description:		handles M_USER_ON_OK_FROM_IED message, sent by parent
//							RTabbedEditImage dialog when user presses the OK button
//
//  Returns:			1 (A-OK)
//
// ****************************************************************************
//
LONG RTabbedEditBrightFocus::OnOKFromIED( WPARAM, LPARAM )
{
	// do the same burn that we do when leaving the tab
	OnShowWindow( FALSE, 0 );
	return 1L;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  Class:	RBrightFocusAction - undoable action for Adjust Color
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
YActionId RBrightFocusAction::m_ActionId( "RBrightFocusAction" );

// ****************************************************************************
//
//  Function Name:	RBrightFocusAction::RBrightFocusAction( )
//
//  Description:	Constructor
//
//  Returns:		Nothing
//
// ****************************************************************************
//
RBrightFocusAction::RBrightFocusAction( RTabbedEditBrightFocus* pOwner, EActionType eAction, YFloatType yBrightness, YFloatType yContrast, YSharpness ySharpness)
	: RUndoableAction( m_ActionId, STRING_UNDO_CHANGE_SPECIAL_EFFECT, STRING_UNDO_CHANGE_SPECIAL_EFFECT ),
	  m_pOwnerDlg( pOwner ),
	  m_eAction(eAction),
	  m_yBrightness(yBrightness),
	  m_yContrast(yContrast),
	  m_ySharpness(ySharpness)
{
}

// ****************************************************************************
//
//  Function Name:	RBrightFocusAction::Undo( )
//
//  Description:	Process a Undo command.
//
//  Returns:		Nothing
//
// ****************************************************************************
//
void RBrightFocusAction::Undo( )
{
	RUndoableAction::Undo( );

	if ( m_pOwnerDlg )
	{
		// call the dialog's undo callback
		m_pOwnerDlg->ProcessUndo( m_eAction, m_yBrightness, m_yContrast, m_ySharpness );
	}
}

// ****************************************************************************
//
//  Function Name:	RBrightFocusAction::WriteScript( )
//
//  Description:	Fills in the action from the script
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RBrightFocusAction::WriteScript( RScript& /* script */) const
{
	return TRUE;
}

#ifdef	TPSDEBUG
// ****************************************************************************
//
//  Function Name:	RBrightFocusAction::Validate( )
//
//  Description:	Validate the object.
//
//  Returns:		Nothing
//
// ****************************************************************************
//
void RBrightFocusAction::Validate( ) const
{
	RUndoableAction::Validate( );
	TpsAssertIsObject( RBrightFocusAction, this );
}
#endif	//	TPSDEBUG
