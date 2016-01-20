// ****************************************************************************
//
//  File Name:		TabbedEditAdjustCol.cpp
//
//  Project:		Renaissance Application Component
//
//  Author:			John Fleischhauer
//
//  Description:	Definition of the RTabbedEditAdjustCol class
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

#include "TabbedEditAdjustCol.h"
#include "TabbedEditImage.h"
#include "ComponentManager.h"
#include "ComponentTypes.h"
#include "ComponentView.h"
#include "BitmapImage.h"
#include "OffscreenDrawingSurface.h"
#include "Path.h"
#include "BitmapLoad.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  Class:	RCastLUTsCtrl - container class for the preview controls
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// ****************************************************************************
//
//  Function Name:	RCastLUTsCtrl::RCastLUTsCtrl()
//
//  Description:	constructor
//
//  Returns:		nothing
//
// ****************************************************************************
//
RCastLUTsCtrl::RCastLUTsCtrl()
	: m_pComponent(NULL)
{
}

// ****************************************************************************
//
//  Function Name:	RCastLUTsCtrl::~RCastLUTsCtrl()
//
//  Description:	destructor
//
//  Returns:		nothing
//
// ****************************************************************************
//
RCastLUTsCtrl::~RCastLUTsCtrl()
{
	delete m_pComponent;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  Class:	RTabbedAdjustCol - the class for the Adjust Color dialog tab
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
BEGIN_MESSAGE_MAP(RTabbedEditAdjustCol, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_PARENTNOTIFY()
	ON_BN_CLICKED( IDC_FINE_RADIO, OnFineRadio )
	ON_BN_CLICKED( IDC_COARSE_RADIO, OnCoarseRadio )
	ON_BN_CLICKED( IDC_CONVERTBW_BTN, OnConvertToGrayscale )
	ON_BN_CLICKED( IDC_UNDO_BTN, OnUndo )
	ON_MESSAGE( WM_USER_APPLY_EDITS_TO_IED_BITMAP, OnApplyEditsToIEDBitmap )
	ON_MESSAGE( WM_USER_RESET_IED_TAB, OnResetUpdate )
	ON_MESSAGE( WM_USER_ON_OK_FROM_IED, OnOKFromIED )
	ON_MESSAGE( WM_USER_UNDO_FROM_IED, OnUndoFromIED )
END_MESSAGE_MAP()


// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::RTabbedEditAdjustCol()
//
//  Description:	constructor
//
//  Returns:		nothing
//
// ****************************************************************************
//
RTabbedEditAdjustCol::RTabbedEditAdjustCol(CWnd* pParent /*=NULL*/)
	: CDialog(RTabbedEditAdjustCol::IDD, pParent),
	  m_hexColorGrid(5),
	  m_angle(0),
	  m_effectFraction(0.0),
	  m_bCoarseFlag(FALSE),
	  m_nGrayscaleCount(0),
	  m_pGrayscaleButtonBitmap(NULL),
	  m_pBaseComponent(NULL),
	  m_pBeforeComponent(NULL),
	  m_pUndoManager(NULL)
{
	for ( INT nDir = kDirectionRed;  nDir < kDirectionNone + 1; nDir++ )
	{
		m_pCastLUTsCtrlArray[nDir] = new RCastLUTsCtrl();
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::~RTabbedEditAdjustCol()
//
//  Description:	destructor
//
//  Returns:		nothing
//
// ****************************************************************************
//
RTabbedEditAdjustCol::~RTabbedEditAdjustCol()
{
	// cleanup
	for ( INT nDir = kDirectionRed;  nDir < kDirectionNone + 1; nDir++ )
	{
		delete m_pCastLUTsCtrlArray[nDir];
	}

	delete m_pBaseComponent;
	delete m_pBeforeComponent;
	delete m_pUndoManager;
	delete m_pGrayscaleButtonBitmap;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::DoDataExchange()
//
//  Description:	DDX/DDV support
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MORE_RED,			m_pCastLUTsCtrlArray[kDirectionRed]->m_rPreviewCtrl);
	DDX_Control(pDX, IDC_MORE_GRN,			m_pCastLUTsCtrlArray[kDirectionGreen]->m_rPreviewCtrl);
	DDX_Control(pDX, IDC_MORE_BLUE,			m_pCastLUTsCtrlArray[kDirectionBlue]->m_rPreviewCtrl);
	DDX_Control(pDX, IDC_MORE_CYAN,			m_pCastLUTsCtrlArray[kDirectionCyan]->m_rPreviewCtrl);
	DDX_Control(pDX, IDC_MORE_MAG,			m_pCastLUTsCtrlArray[kDirectionMagenta]->m_rPreviewCtrl);
	DDX_Control(pDX, IDC_MORE_YEL,			m_pCastLUTsCtrlArray[kDirectionYellow]->m_rPreviewCtrl);
	DDX_Control(pDX, IDC_CUR_PREVIEW,		m_pCastLUTsCtrlArray[kDirectionNone]->m_rPreviewCtrl);

	DDX_Control(pDX, IDC_BEFORE_PREVIEW,	m_rBeforePreviewCtrl);
	DDX_Control(pDX, IDC_UNDO_BTN, m_cButtonUndo);
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::PreTranslateMessage()
//
//  Description:		Handles various keystrokes for this dialog
//
//  Returns:			TRUE if the message is processed here
//
// ****************************************************************************
//
BOOL RTabbedEditAdjustCol::PreTranslateMessage( MSG* pMsg )
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
			if ( pFocusWnd == GetDlgItem( IDC_CONVERTBW_BTN ) )
			{
				OnConvertToGrayscale();
				bRet = TRUE;
			}
			else if ( pFocusWnd == &m_cButtonUndo )
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
//  Function Name:	RTabbedEditAdjustCol::OnInitDialog() 
//
//  Description:	Sets up the dialog
//
//  Returns:		BOOLEAN
//
// ****************************************************************************
//
BOOL RTabbedEditAdjustCol::OnInitDialog() 
{
	// Call the base method
	BOOLEAN bRet = CDialog::OnInitDialog();
	if ( bRet )
	{
		for ( INT nDir = kDirectionRed;  nDir < kDirectionNone + 1; nDir++ )
		{
			// sanity check
			if ( ! m_pCastLUTsCtrlArray[nDir] )
			{
				bRet = FALSE;
				break;
			}
		}

		// our undo mechanism
		m_pUndoManager = new REditUndoManager();

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
//  Function Name:	RTabbedEditAdjustCol::OnShowWindow()
//
//  Description:	sets up display of the dialog window
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::OnShowWindow(  BOOLEAN bShow, UINT nStatus )
{
	// every time we come into this tab
	if ( bShow && ! nStatus )
	{
		RWaitCursor rWaitCursor;

		// clean-slate time
		m_hexColorGrid.Reset();
		m_angle = 0;
		m_effectFraction = 0.0;

		// set radio buttons to the last state
		if ( m_bCoarseFlag )
		{
			((CButton *)GetDlgItem( IDC_COARSE_RADIO ))->SetCheck( 1 );
			((CButton *)GetDlgItem( IDC_FINE_RADIO ))->SetCheck( 0 );
		}
		else
		{
			((CButton *)GetDlgItem( IDC_FINE_RADIO ))->SetCheck( 1 );
			((CButton *)GetDlgItem( IDC_COARSE_RADIO ))->SetCheck( 0 );
		}

		// no grayscale
		m_nGrayscaleCount = 0;

		// reset preview components to force update
		RTabbedEditImage *pParentDlg = (RTabbedEditImage *)GetParent();
		if ( pParentDlg )
		{
			// get a current-preview-sized copy of the current IED image that we can munge locally in this tab
			delete m_pBaseComponent;
			m_pBaseComponent   = pParentDlg->GetPreviewCopyFromIEDComponent( &m_rBeforePreviewCtrl );

			// get the rest of our component copies
			delete m_pBeforeComponent;
			m_pBeforeComponent = pParentDlg->GetPreviewComponentCopy( m_pBaseComponent, &m_rBeforePreviewCtrl );

			m_rBeforePreviewCtrl.SetDocument( m_pBeforeComponent, FALSE);
		
			for ( INT nDir = kDirectionRed;  nDir < kDirectionNone + 1; nDir++ )
			{
				m_pCastLUTsCtrlArray[nDir]->m_rLUTs.Uninitialize();
			}

			// grayscale button gets a unique image
			SetGrayscaleButtonImage();
		}

		// force the initial updates
		if ( UpdateLUTs( kDirectionNone ) )
		{
			UpdatePreviews();
		}

		UpdateUndoButton();
	}
	//	when bShow is FALSE, we are switching to another tab; this is where we "burn" our changes
	//	into the local image component owned by the parent dialog
	else
	{
		// is there something to do?
		if ( m_effectFraction > 0.0 || m_nGrayscaleCount > 0 )
		{
			// apply edits to the intermediate bitmap owned by the IED 
			OnApplyEditsToIEDBitmap( 0, 0 );

			// this forces the modified bitmap to be set into the IED component
			((RTabbedEditImage *)GetParent())->ApplyImageEffects();
		}

		// handle the Revert and OK buttons in the parent, too
		YCounter nUndoCount = m_pUndoManager->CountUndoableActions();
		if ( nUndoCount > 0 )
		{
			// if we are laeving this tab with a change, lock the buttons enabled
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
//  Function Name:	RTabbedEditAdjustCol::UpdatePreviews()
//
//  Description:	refreshes the preview windows with new info
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::UpdatePreviews( BOOLEAN bUpdateAll )
{
	// update the preview control windows
	m_pCastLUTsCtrlArray[kDirectionNone]->m_rPreviewCtrl.SetDocument(m_pCastLUTsCtrlArray[kDirectionNone]->m_pComponent, FALSE);
	if ( bUpdateAll )
	{
		for ( INT nDir = kDirectionRed;  nDir < kDirectionNone; nDir++ )
		{
			m_pCastLUTsCtrlArray[nDir]->m_rPreviewCtrl.SetDocument(m_pCastLUTsCtrlArray[nDir]->m_pComponent, FALSE);
		}
	}
}


// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::OnParentNotify()
//
//  Description:	Handles WM_PARENTNOTIFY messages from child controls
//
//					Note that this message is passed to the tab from the parent
//					RTabbedEditImage dialog. ('Cause that's where the message
//					gets sent, that's why.)
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::OnParentNotify( UINT message, LPARAM lParam )
{
	if ( message == WM_LBUTTONDOWN )
	{
		RWaitCursor rWaitCursor;

		// who hit whom where?
		POINT point;
		point.x = LOWORD(lParam);
		point.y = HIWORD(lParam);

		BOOLEAN bHit = FALSE;
		RImagePreviewCtrl *pCtrl = (RImagePreviewCtrl *)ChildWindowFromPoint( point );

		// was the hit ina control?
		for ( INT nDir = kDirectionRed;  nDir < kDirectionNone + 1; nDir++ )
		{
			if ( pCtrl == &(m_pCastLUTsCtrlArray[nDir]->m_rPreviewCtrl) )
			{
				bHit = TRUE;
				break;
			}
		}

		if ( bHit && nDir != kDirectionNone )
		{
			// for undo
			RHexColorGrid oldGrid(m_hexColorGrid);

			// update our LUTs
			if ( UpdateLUTs( (EDirection)nDir ) )
			{
#ifdef DYNAMIC_PREVIEW_UPDATES
				UpdatePreviews();
#else
				UpdatePreviews( FALSE );
#endif

				// Create the undo action for changing the image.
				// Note, this action is an undo only.  It saves the current
				// state variables, and will use them to restore on undo. 
				// Any changes to be done to the image need to be done manually.
				if ( m_pUndoManager )
				{
					RUndoableAction* pAction = new RAdjustColorAction( this, RAdjustColorAction::kDirection, oldGrid );
					m_pUndoManager->SendAction( pAction );
					UpdateUndoButton();
				}
			}
		}
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::UpdateLUTs()
//
//  Description:	
//
//  Returns:		BOOLEAN
//
// ****************************************************************************
//
BOOLEAN RTabbedEditAdjustCol::UpdateLUTs( EDirection direction, BOOLEAN bForceUpdate )
{
	BOOLEAN bOK = TRUE;

	INT nDir = kDirectionNone;
	RCastLUTsCtrl *pCtrl = m_pCastLUTsCtrlArray[nDir];

	// if current is not initialized, this is our first time in, need to initialize all LUTs
	if ( ! pCtrl->m_rLUTs.Initialized() )
	{
		// these better be 0
		TpsAssert( m_angle == 0 && m_effectFraction == 0.0, "Unexpected uninitialized LUTs" );

		if ( BuildLUTs( pCtrl, 0, 0.0 ) )
		{
#ifdef DYNAMIC_PREVIEW_UPDATES
			// get our fraction to increment
			YHueAngle angle;
			YFloatType fraction;
			m_hexColorGrid.NextPosition( kDirectionRed );
			m_hexColorGrid.GetAngle( &angle, &fraction );
			m_hexColorGrid.Reset();
#endif
			if ( ApplyLUTsToPreviewControl( pCtrl ) )
			{
				// build all the surrounding LUTs
				for ( nDir = kDirectionRed;  nDir < kDirectionNone; nDir++ )
				{
					pCtrl = m_pCastLUTsCtrlArray[nDir];
#ifdef DYNAMIC_PREVIEW_UPDATES
					if ( ! BuildLUTs( pCtrl, kPrimaryAngle[nDir], fraction ) )
#else
					if ( ! BuildLUTs( pCtrl, kPrimaryAngle[nDir], 1.0 ) )
#endif
					{
						bOK = FALSE;
						break;
					}

					if ( ! ApplyLUTsToPreviewControl( pCtrl ) )
					{
						bOK = FALSE;
						break;
					}
				}
			}
		}
	}
	// we already have an initialized table, so we can just move some of them
	else
	{
		bOK = FALSE;
		YHueAngle angle;
		YFloatType fraction;

		// update our hex color grid to the next position and get the variables
		m_hexColorGrid.NextPosition( direction );
		m_hexColorGrid.GetAngle( &angle, &fraction );

		// if we actually moved, move or regenerate LUTs accordingly
		if ( angle != m_angle || fraction != m_effectFraction || bForceUpdate )
		{
#ifdef DYNAMIC_PREVIEW_UPDATES
			switch ( direction )
			{
				case kDirectionRed:
					bOK = RedShift();
					break;
					
				case kDirectionYellow:
					bOK = YellowShift();
					break;

				case kDirectionGreen:
					bOK = GreenShift();
					break;

				case kDirectionCyan:
					bOK = CyanShift();
					break;

				case kDirectionBlue:
					bOK = BlueShift();
					break;

				case kDirectionMagenta:
					bOK = MagentaShift();
					break;

				case kDirectionNone:		// used by undo
					bOK = TRUE;
					break;

				default:
					break;
			}

			// everything went as expected
			if ( bOK )
			{
				for ( nDir = kDirectionRed;  nDir < kDirectionNone + 1; nDir++ )
				{
					pCtrl = m_pCastLUTsCtrlArray[nDir];
					if ( ! ApplyLUTsToPreviewControl( pCtrl ) )
					{
						bOK = FALSE;
						break;
					}
				}
			}
#endif
			// record the current values
			m_angle = angle;
			m_effectFraction = fraction;

#ifndef DYNAMIC_PREVIEW_UPDATES
			bOK = BuildLUTs( m_pCastLUTsCtrlArray[ kDirectionNone ], m_angle, m_effectFraction );
			if ( bOK )
			{
				bOK = ApplyLUTsToPreviewControl( m_pCastLUTsCtrlArray[ kDirectionNone ] );
			}
#endif
		}
	}

	return bOK;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::CopyLUTs()
//
//  Description:	uhhhhh, copies LUTs, I guess
//
//  Returns:		TRUEnothing
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::CopyLUTs( EDirection srcDirection, EDirection destDirection )
{
	m_pCastLUTsCtrlArray[ destDirection ]->m_rLUTs = m_pCastLUTsCtrlArray[ srcDirection ]->m_rLUTs;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::BuildLUTs()
//
//  Description:	builds new LUTs, based on passed angle and fraction
//
//  Returns:		nothing
//
// ****************************************************************************
//
BOOLEAN RTabbedEditAdjustCol::BuildLUTs( RCastLUTsCtrl *pCtrl, YHueAngle angle, YFloatType fraction )
{
	BOOLEAN bRet = FALSE;

	pCtrl->m_rLUTs.SetHueAngle( angle );
	pCtrl->m_rLUTs.SetEffectFraction( fraction );
	pCtrl->m_rLUTs.Coarse( m_bCoarseFlag );

	return pCtrl->m_rLUTs.BuildLUTs();

}

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::ApplyLUTsToPreviewControl()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
BOOLEAN RTabbedEditAdjustCol::ApplyLUTsToPreviewControl( RCastLUTsCtrl *pCtrl )
{
	// get a new preview copy from our already-scaled down base image copy
	delete pCtrl->m_pComponent;
	RTabbedEditImage *pParentDlg = (RTabbedEditImage *)GetParent();
	pCtrl->m_pComponent = pParentDlg->GetPreviewComponentCopy( m_pBaseComponent, &pCtrl->m_rPreviewCtrl );

	// adjust it
	return ApplyLUTsToComponent( pCtrl->m_pComponent, &pCtrl->m_rLUTs );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::ApplyLUTsToComponent()
//
//  Description:	applies lookup tables to image within a component
//
//  Returns:		TRUE if successful, False on error
//
// ****************************************************************************
//
BOOLEAN RTabbedEditAdjustCol::ApplyLUTsToComponent( RComponentDocument *pComponent, RCastLUTs *pLUTs )
{
	BOOLEAN bRet = FALSE;

//	if ( pLUTs->GetEffectFraction() == 0.0 )					// no adjust
//	{
//		bRet = TRUE;
//	}
//	else
	{
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
	}

	return bRet;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::ApplyLUTsToBitmap()
//
//  Description:	calls Image Library routine to apply LUTs to the bitmap
//
//  Returns:		nothing, but pDestBitmap is filled with the modified data
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::ApplyLUTsToBitmap( RBitmapImage *pSrcBitmap, RBitmapImage *pDestBitmap, RCastLUTs *pLUTs )
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
//  Function Name:	RTabbedEditAdjustCol::OnFineRadio()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::OnFineRadio()
{
	if ( m_bCoarseFlag )
	{
		// set the flag and regenerate the LUTs
		m_bCoarseFlag = FALSE;
		for ( INT nDir = kDirectionRed;  nDir < kDirectionNone + 1; nDir++ )
		{
			RCastLUTsCtrl *pCtrl = m_pCastLUTsCtrlArray[nDir];
			pCtrl->m_rLUTs.Coarse( m_bCoarseFlag );

			if ( pCtrl->m_rLUTs.BuildLUTs() )
			{
				ApplyLUTsToPreviewControl( pCtrl );
			}
		}

		// set up for undo
		if ( m_pUndoManager )
		{
			RUndoableAction* pAction = new RAdjustColorAction( this, RAdjustColorAction::kCoarse,  m_hexColorGrid );
			m_pUndoManager->SendAction( pAction );
			UpdateUndoButton();
		}

		UpdatePreviews();
	}
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::OnCoarseRadio()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::OnCoarseRadio()
{
	if ( ! m_bCoarseFlag )
	{
		// set the flag and regenerate the LUTs
		m_bCoarseFlag = TRUE;
		for ( INT nDir = kDirectionRed;  nDir < kDirectionNone + 1; nDir++ )
		{
			RCastLUTsCtrl *pCtrl = m_pCastLUTsCtrlArray[nDir];
			pCtrl->m_rLUTs.Coarse( m_bCoarseFlag );

			if ( pCtrl->m_rLUTs.BuildLUTs() )
			{
				ApplyLUTsToPreviewControl( pCtrl );
			}
		}

		// set up for undo
		if ( m_pUndoManager )
		{
			RUndoableAction* pAction = new RAdjustColorAction( this, RAdjustColorAction::kCoarse,  m_hexColorGrid );
			m_pUndoManager->SendAction( pAction );
			UpdateUndoButton();
		}

		UpdatePreviews();
	}
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::OnConvertToGrayscale()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::OnConvertToGrayscale()
{
	BOOLEAN bUpdate = FALSE;
	
	//	if we have not applied grayscale yet in this session
	//	our base component contains a colored image
	if ( m_nGrayscaleCount == 0 )
	{
		RWaitCursor rWaitCursor;

		// make the base component a grayscale image
		RImageInterface* pImageInterface = dynamic_cast<RImageInterface*>(static_cast<RComponentView *>(m_pBaseComponent->GetActiveView())->GetInterface(kImageInterfaceId));
		RImage *pImage = pImageInterface->GetImage();
		if ( pImage )
		{
			RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>(pImage);
			if( pBitmap )
			{
				// call the library grayscale method
				RImageLibrary().ConvertToGrayscale( *pBitmap );
				pImageInterface->SetImage( *pImage );

				bUpdate = TRUE;
			}
		}

		// clean up the interface
		delete pImageInterface;
	}
	else		// our base component is already gray
	{
		// if our current preview is already showing the gray image, do nothing
		if ( m_effectFraction > 0.0 )
		{
			bUpdate = TRUE;
		}
	}

	if ( bUpdate )
	{
		// set up for undo
		if ( m_pUndoManager )
		{
			RUndoableAction* pAction = new RAdjustColorAction( this, RAdjustColorAction::kGrayscale,  m_hexColorGrid );
			m_pUndoManager->SendAction( pAction );
			UpdateUndoButton();
		}

		// reset our color adjust level to none
		m_hexColorGrid.Reset();
		m_angle = 0;
		m_effectFraction = 0.0;

		// update the LUTs
		if ( UpdateLUTs( kDirectionNone, TRUE ) )
		{
			for ( INT nDir = kDirectionRed;  nDir < kDirectionNone; nDir++ )
			{
				RCastLUTsCtrl *pCtrl = m_pCastLUTsCtrlArray[nDir];
				if ( pCtrl->m_rLUTs.BuildLUTs() )
				{
					ApplyLUTsToPreviewControl( pCtrl );
				}
			}

			UpdatePreviews();
			m_nGrayscaleCount++;
		}
	}
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::ProcessUndo()
//
//  Description:	callback invoked by the undo manager
//					this is where the undo really happens
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::ProcessUndo( RAdjustColorAction::EActionType eAction, RHexColorGrid& rHexColorGrid )
{
	BOOLEAN bOK = FALSE;
	RWaitCursor rWaitCursor;

	switch ( eAction )
	{
		case RAdjustColorAction::kDirection:
		{
#ifdef DYNAMIC_PREVIEW_UPDATES
#error	Adjust Color Undo logic not yet fully implemented with Dynamic previews!
#endif
			// restore the previous settings
			m_hexColorGrid = rHexColorGrid;

			// update the LUTs
			if ( UpdateLUTs( kDirectionNone ) )
			{
				bOK = TRUE;
			}
			break;
		}

		case RAdjustColorAction::kCoarse:
		{
			// reverse the flag
			m_bCoarseFlag = ! m_bCoarseFlag;
			for ( INT nDir = kDirectionRed;  nDir < kDirectionNone + 1; nDir++ )
			{
				RCastLUTsCtrl *pCtrl = m_pCastLUTsCtrlArray[nDir];
				pCtrl->m_rLUTs.Coarse( m_bCoarseFlag );

				if ( pCtrl->m_rLUTs.BuildLUTs() )
				{
					ApplyLUTsToPreviewControl( pCtrl );
					bOK = TRUE;
				}
			}
	
			// reset the radio buttons
			if ( m_bCoarseFlag )
			{
				((CButton *)GetDlgItem( IDC_COARSE_RADIO ))->SetCheck( 1 );
				((CButton *)GetDlgItem( IDC_FINE_RADIO ))->SetCheck( 0 );
			}
			else
			{
				((CButton *)GetDlgItem( IDC_FINE_RADIO ))->SetCheck( 1 );
				((CButton *)GetDlgItem( IDC_COARSE_RADIO ))->SetCheck( 0 );
			}
			break;
		}

		case RAdjustColorAction::kGrayscale:
		{
			TpsAssert( m_nGrayscaleCount >= 0, "Grayscale count < 0" );

			// decrement the count of number of times grayscale has been applied this session
			m_nGrayscaleCount--;
			if ( m_nGrayscaleCount <= 0 )		// if we need to undo the initial application
			{
				m_nGrayscaleCount = 0;			// (just in case)

				// get an un-grayed copy of the image into our base component
				delete m_pBaseComponent;
				m_pBaseComponent = ((RTabbedEditImage *)GetParent())->GetPreviewCopyFromIEDComponent( &m_pCastLUTsCtrlArray[ kDirectionNone ]->m_rPreviewCtrl );
			}

			// restore the previous color adjust settings
			m_hexColorGrid = rHexColorGrid;

			// update the LUTs
			if ( UpdateLUTs( kDirectionNone, TRUE ) )
			{
				for ( INT nDir = kDirectionRed;  nDir < kDirectionNone; nDir++ )
				{
					RCastLUTsCtrl *pCtrl = m_pCastLUTsCtrlArray[nDir];
					if ( pCtrl->m_rLUTs.BuildLUTs() )
					{
						ApplyLUTsToPreviewControl( pCtrl );
					}
				}

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
#ifdef DYNAMIC_PREVIEW_UPDATES
		UpdatePreviews();
#else
		if ( eAction == RAdjustColorAction::kDirection )
		{
			UpdatePreviews( FALSE );
		}
		else
		{
			UpdatePreviews( TRUE );
		}
#endif
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::OnUndo()
//
//  Description:	responds to the Undo button being pressed; invokes undo of the last action
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::OnUndo()
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
//  Function Name:	RTabbedEditAdjustCol::UpdateUndoButton()
//
//  Description:	enables or disables the Undo button
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::UpdateUndoButton()
{
	CWnd *cwndUndoButton = GetDlgItem( IDC_UNDO_BTN );
	if ( cwndUndoButton && m_pUndoManager )
	{
		uWORD nID = m_pUndoManager->GetUndoStringId();
		if ( nID != STRING_CANT_UNDO )
		{
			cwndUndoButton->EnableWindow( TRUE );

			// handle the Revert and OK buttons in the parent, too
			((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( TRUE );
		}
		else //if ( nUndoCount == 0 )
		{
			if ( GetFocus() == &m_cButtonUndo )
			{
				CWnd *pWnd;
				if ( m_bCoarseFlag )
				{
					pWnd = GetDlgItem( IDC_COARSE_RADIO );
				}
				else
				{
					pWnd = GetDlgItem( IDC_FINE_RADIO );
				}
				pWnd->SetFocus();
			}
			cwndUndoButton->EnableWindow( FALSE );

			// handle the Revert and OK buttons in the parent, too
			((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( FALSE );
		}
	}
}

#if 0
// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::OnOK()
//
//  Description:		IDOK message handler
//
//  Returns:			nothing
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::OnOK()
{
	// do the same burn that we do when leaving the tab
	OnShowWindow( FALSE, 0 );

	CDialog::OnOK();
}
#endif

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::OnOKFromIED()
//
//  Description:		handles M_USER_ON_OK_FROM_IED message, sent by parent
//							RTabbedEditImage dialog when user presses the OK button
//
//  Returns:			1 (A-OK)
//
// ****************************************************************************
//
LONG RTabbedEditAdjustCol::OnOKFromIED( WPARAM, LPARAM )
{
	// do the same burn that we do when leaving the tab
	OnShowWindow( FALSE, 0 );
	return 1L;
}


// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::OnApplyEditsToIEDBitmap()
//
//  Description:	using the current settings, apply the edits to the
//					intermediate bitmap owned by the base IED dialog
//
//  Returns:		nothing
//
// ****************************************************************************
//
LONG RTabbedEditAdjustCol::OnApplyEditsToIEDBitmap( WPARAM, LPARAM )
{
	RWaitCursor rWaitCursor;

	// get the intermediate bitmap owned by the IED
	RTabbedEditImage *pParentDlg = (RTabbedEditImage *)GetParent();
	RBitmapImage *pBitmap = pParentDlg->GetImage();

	// apply grayscale if appropriate
	if ( m_nGrayscaleCount > 0 )
	{
		RImageLibrary().ConvertToGrayscale( *pBitmap );
	}

	// if we have adjusted color
	if ( m_effectFraction > 0.0 )
	{
		// create a destination bitmap
		RBitmapImage *pNewBitmap = new RBitmapImage( *pBitmap );
		if ( pNewBitmap )
		{
			// create LUTs from our current settings
			RCastLUTs rLUTs( m_angle, m_effectFraction, m_bCoarseFlag );

			// and apply them to the bitmap
			ApplyLUTsToBitmap( pBitmap, pNewBitmap, &rLUTs);

			// pass the modified bitmap back to the parent
			// Note: parent's SetImage() will cleanup old bitmap; but it does not
			//       copy the new one, so don't delete it!
			pParentDlg->SetImage( pNewBitmap );
		}
	}

	return 0;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::OnResetUpdate()
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
LONG RTabbedEditAdjustCol::OnResetUpdate( WPARAM, LPARAM )
{
	// reset flag
	m_bCoarseFlag = FALSE;

	// purge the undo buffer
	m_pUndoManager->FreeAllActions();

	// reinitialize the window
	OnShowWindow( TRUE, 0 );

	return 0;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::SetGrayscaleButtonImage()
//
//  Description:		Puts a grayscale thumbnail of the current image on the
//							"Convert to Black and White" button
//
//  Returns:			nothing
//
// ****************************************************************************
//
void RTabbedEditAdjustCol::SetGrayscaleButtonImage()
{
	// get the grayscale button
	CButton *pButton = (CButton *)GetDlgItem( IDC_CONVERTBW_BTN );
	if ( pButton )
	{
		// determine size of image
		CRect cClientRect;
		pButton->GetClientRect( cClientRect );

		CRect cInsetRect( cClientRect );
		cInsetRect.DeflateRect( 4, 4 );
		
		RTabbedEditImage *pParentDlg = (RTabbedEditImage *)GetParent();
		RComponentView *pView = (RComponentView *)pParentDlg->GetComponent()->GetActiveView();
		if (! pView)
		{
			return;
		}

		RImageInterface* pImageInterface = dynamic_cast<RImageInterface*>(pView->GetInterface(kImageInterfaceId));
		RImage *pImage = pImageInterface->GetImage();
		delete pImageInterface;
		if ( ! pImage )
		{
			return;
		}

	 	// get the preview image dimensions
		CSize cDisplaySize = pParentDlg->GetPreviewImageSize( pImage, cInsetRect );

		CRect cDisplayRect;
		cDisplayRect.left   = cInsetRect.TopLeft().x + cInsetRect.Width()  / 2 - cDisplaySize.cx / 2;
		cDisplayRect.top    = cInsetRect.TopLeft().y + cInsetRect.Height() / 2 - cDisplaySize.cy / 2;
		cDisplayRect.right  = cDisplayRect.left + cDisplaySize.cx;
		cDisplayRect.bottom = cDisplayRect.top + cDisplaySize.cy;

		
		// get a scaled copy of the IED image
		RComponentDocument *pTempComponent = pParentDlg->GetComponentCopy( m_pBaseComponent, cDisplaySize );
		if ( pTempComponent )
		{
			try
			{
				pView = (RComponentView *)pTempComponent->GetActiveView();
				pImageInterface = dynamic_cast<RImageInterface*>(pView->GetInterface(kImageInterfaceId));
				pImage = pImageInterface->GetImage();
				delete pImageInterface;
				if ( pImage )
				{
					RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>(pImage);
					if( pBitmap )
					{
						// call the library grayscale method
						RImageLibrary().ConvertToGrayscale( *pBitmap );

						// create an offscreen drawing surface
						ROffscreenDrawingSurface rODS;
						CDC *pDC = pButton->GetDC();
						rODS.Initialize( pDC->m_hDC, pDC->m_hDC );

						// create an RScratchBitmapImage compatible with the drawing surface
						delete m_pGrayscaleButtonBitmap;
						m_pGrayscaleButtonBitmap = new RScratchBitmapImage;
						m_pGrayscaleButtonBitmap->Initialize(rODS, cClientRect.Width(), cClientRect.Height());		

						// set the bitmap into an offscreen DS		
						rODS.SetImage( m_pGrayscaleButtonBitmap );		

						// Clear the background
						rODS.SetFillColor( RSolidColor( ::GetSysColor(COLOR_3DFACE) ) );
						rODS.FillRectangle( RIntRect( RIntSize( cClientRect.Width(), cClientRect.Height() ) ) );

						// create a transform to scale image to the preview dimensions
						RRealSize rImageSize = pView->GetBoundingRect().WidthHeight();
						YScaleFactor sX = (YFloatType)cDisplayRect.Width() / (YFloatType)rImageSize.m_dx;		
						YScaleFactor sY = (YFloatType)cDisplayRect.Height() / (YFloatType)rImageSize.m_dy;		

						R2dTransform transform;
						transform.PreScale( sX, sY );
						transform.PostTranslate( (YRealDimension)cDisplayRect.left, (YRealDimension)cDisplayRect.top );

						// render into our offscreen
						pView->Render( rODS, transform, RIntRect( cClientRect ) );

						rODS.ReleaseImage();
						pButton->ReleaseDC( pDC );

						// get the HBITMAP and set it to the button
						HBITMAP hBitmap = (HBITMAP)m_pGrayscaleButtonBitmap->GetSystemHandle();

						pButton->SetBitmap( hBitmap );
					}
				}

				delete pTempComponent;
			}
			catch(...)
			{
			}

		}
	}
}


#ifdef DYNAMIC_PREVIEW_UPDATES

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::GenerateNewLUTs()
//
//  Description:	given a direction, generate the appropriate LUTs
//
//  Returns:		nothing
//
// ****************************************************************************
//
BOOLEAN RTabbedEditAdjustCol::GenerateNewLUTs( EDirection direction )
{
	BOOLEAN bRet = FALSE;

	YHueAngle angle;
	YFloatType fraction;

	// given our current position
	RHexColorGrid rTempGrid( m_hexColorGrid );

	// what's the next one?
	rTempGrid.NextPosition( direction );
	rTempGrid.GetAngle( &angle, &fraction );

	// generate the LUTs
	return BuildLUTs( m_pCastLUTsCtrlArray[ direction ], angle, fraction );
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::RedShift()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RTabbedEditAdjustCol::RedShift()
{
	BOOLEAN bEDGE = FALSE;
	BOOLEAN bOK = FALSE;

	if ( m_effectFraction == 1.0 )
	{
		// we are already between 0 and 60
		if ( m_angle > kPrimaryAngle[ kDirectionRed ]  && m_angle <= kPrimaryAngle[ kDirectionYellow ]  )
		{
			bEDGE =TRUE;
			bOK = MagentaShift();
		}
		// between 300 and 0
		else if ( m_angle >= kPrimaryAngle[ kDirectionMagenta ] )
		{
			bEDGE =TRUE;
			bOK = YellowShift();
		}
	}

	if ( ! bEDGE )
	{
		CopyLUTs( kDirectionNone, kDirectionCyan );
		CopyLUTs( kDirectionRed, kDirectionNone );
		CopyLUTs( kDirectionYellow, kDirectionGreen );
		CopyLUTs( kDirectionMagenta, kDirectionBlue );

		if ( GenerateNewLUTs( kDirectionRed ) )
		{
			// we are on an outer facet between 60 and 120
			if ( (m_angle > kPrimaryAngle[ kDirectionYellow ]  && m_angle <= kPrimaryAngle[ kDirectionGreen ]) && m_effectFraction == 1.0 )
			{
				CopyLUTs( kDirectionRed, kDirectionYellow );
				bOK = GenerateNewLUTs( kDirectionMagenta );
			}
			// between 240 and 300
			else if ( (m_angle >= kPrimaryAngle[ kDirectionBlue ] && m_angle < kPrimaryAngle[ kDirectionMagenta ]) && m_effectFraction == 1.0 )
			{
				CopyLUTs( kDirectionRed, kDirectionMagenta );
				bOK = GenerateNewLUTs( kDirectionYellow );
			}
			else
			{
				if ( GenerateNewLUTs( kDirectionYellow ) )
				{
					if ( GenerateNewLUTs( kDirectionMagenta ) )
					{
						bOK = TRUE;
					}
				}
			}
		}
	}

	return bOK;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::YellowShift()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RTabbedEditAdjustCol::YellowShift()
{
	BOOLEAN bEDGE = FALSE;
	BOOLEAN bOK = FALSE;

	if ( m_effectFraction == 1.0 )
	{
		// we are already between 0 and 60
		if ( m_angle >= kPrimaryAngle[ kDirectionRed ]  && m_angle < kPrimaryAngle[ kDirectionYellow ] )
		{
			bEDGE =TRUE;
			bOK = GreenShift();
		}
		// between 60 and 120
		else if ( m_angle > kPrimaryAngle[ kDirectionYellow ] && m_angle <= kPrimaryAngle[ kDirectionGreen ] )
		{
			bEDGE =TRUE;
			bOK = RedShift();
		}
	}

	if ( ! bEDGE )
	{
		CopyLUTs( kDirectionNone, kDirectionBlue );
		CopyLUTs( kDirectionYellow, kDirectionNone );
		CopyLUTs( kDirectionGreen, kDirectionCyan );
		CopyLUTs( kDirectionRed, kDirectionMagenta );

		if ( GenerateNewLUTs( kDirectionYellow ) )
		{
			// we are on an outer facet between 120 and 180
			if ( (m_angle > kPrimaryAngle[ kDirectionGreen ]  && m_angle <= kPrimaryAngle[ kDirectionCyan ]) && m_effectFraction == 1.0 )
			{
				CopyLUTs( kDirectionYellow, kDirectionGreen );
				bOK = GenerateNewLUTs( kDirectionRed );
			}
			// between 300 and 360
			else if ( (m_angle >= kPrimaryAngle[ kDirectionMagenta ] && m_angle < 360) && m_effectFraction == 1.0 )
			{
				CopyLUTs( kDirectionYellow, kDirectionRed );
				bOK = GenerateNewLUTs( kDirectionGreen );
			}
			else
			{
				if ( GenerateNewLUTs( kDirectionGreen ) )
				{
					if ( GenerateNewLUTs( kDirectionRed ) )
					{
						bOK = TRUE;
					}
				}
			}
		}
	}

	return bOK;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::GreenShift()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RTabbedEditAdjustCol::GreenShift()
{
	BOOLEAN bEDGE = FALSE;
	BOOLEAN bOK = FALSE;

	if ( m_effectFraction == 1.0 )
	{
		// we are already between 60 and 120
		if ( m_angle >= kPrimaryAngle[ kDirectionYellow ]  && m_angle < kPrimaryAngle[ kDirectionGreen ] )
		{
			bEDGE =TRUE;
			bOK = CyanShift();
		}
		// between 120 and 180
		else if ( m_angle > kPrimaryAngle[ kDirectionGreen ] && m_angle <= kPrimaryAngle[ kDirectionCyan ] )
		{
			bEDGE =TRUE;
			bOK = YellowShift();
		}
	}

	if ( ! bEDGE )
	{
		CopyLUTs( kDirectionNone, kDirectionMagenta );
		CopyLUTs( kDirectionGreen, kDirectionNone );
		CopyLUTs( kDirectionCyan, kDirectionBlue );
		CopyLUTs( kDirectionYellow, kDirectionRed );

		if ( GenerateNewLUTs( kDirectionGreen ) )
		{
			// we are on an outer facet between 180 and 240
			if ( (m_angle > kPrimaryAngle[ kDirectionCyan ]  && m_angle <= kPrimaryAngle[ kDirectionBlue ]) && m_effectFraction == 1.0 )
			{
				CopyLUTs( kDirectionGreen, kDirectionCyan );
				bOK = GenerateNewLUTs( kDirectionYellow );
			}
			// between 0 and 60
			else if ( (m_angle >= kPrimaryAngle[ kDirectionRed ] && m_angle < kPrimaryAngle[ kDirectionYellow ]) && m_effectFraction == 1.0 )
			{
				CopyLUTs( kDirectionGreen, kDirectionYellow );
				bOK = GenerateNewLUTs( kDirectionCyan );
			}
			else
			{
				if ( GenerateNewLUTs( kDirectionCyan ) )
				{
					if ( GenerateNewLUTs( kDirectionYellow ) )
					{
						bOK = TRUE;
					}
				}
			}
		}
	}

	return bOK;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::CyanShift()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RTabbedEditAdjustCol::CyanShift()
{
	BOOLEAN bEDGE = FALSE;
	BOOLEAN bOK = FALSE;

	if ( m_effectFraction == 1.0 )
	{
		// we are already between 120 and 180
		if ( m_angle >= kPrimaryAngle[ kDirectionGreen ]  && m_angle < kPrimaryAngle[ kDirectionCyan ] )
		{
			bEDGE =TRUE;
			bOK = BlueShift();
		}
		// between 180 and 240
		else if ( m_angle > kPrimaryAngle[ kDirectionCyan ] && m_angle <= kPrimaryAngle[ kDirectionBlue ] )
		{
			bEDGE =TRUE;
			bOK = GreenShift();
		}
	}

	if ( ! bEDGE )
	{
		CopyLUTs( kDirectionNone, kDirectionRed );
		CopyLUTs( kDirectionCyan, kDirectionNone );
		CopyLUTs( kDirectionGreen, kDirectionYellow );
		CopyLUTs( kDirectionBlue, kDirectionMagenta );

		if ( GenerateNewLUTs( kDirectionCyan ) )
		{
			// we are on an outer facet between 240 and 300
			if ( (m_angle > kPrimaryAngle[ kDirectionBlue ]  && m_angle <= kPrimaryAngle[ kDirectionMagenta ]) && m_effectFraction == 1.0 )
			{
				CopyLUTs( kDirectionCyan, kDirectionBlue );
				bOK = GenerateNewLUTs( kDirectionGreen );
			}
			// between 60 and 120
			else if ( (m_angle >= kPrimaryAngle[ kDirectionYellow ] && m_angle < kPrimaryAngle[ kDirectionGreen ]) && m_effectFraction == 1.0 )
			{
				CopyLUTs( kDirectionCyan, kDirectionGreen );
				bOK = GenerateNewLUTs( kDirectionBlue );
			}
			else
			{
				if ( GenerateNewLUTs( kDirectionBlue ) )
				{
					if ( GenerateNewLUTs( kDirectionGreen ) )
					{
						bOK = TRUE;
					}
				}
			}
		}
	}

	return bOK;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::BlueShift()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RTabbedEditAdjustCol::BlueShift()
{
	BOOLEAN bEDGE = FALSE;
	BOOLEAN bOK = FALSE;

	if ( m_effectFraction == 1.0 )
	{
		// we are already between 180 and 240
		if ( m_angle >= kPrimaryAngle[ kDirectionCyan ]  && m_angle < kPrimaryAngle[ kDirectionBlue ] )
		{
			bEDGE =TRUE;
			bOK = MagentaShift();
		}
		// between 240 and 300
		else if ( m_angle > kPrimaryAngle[ kDirectionBlue ] && m_angle <= kPrimaryAngle[ kDirectionMagenta ] )
		{
			bEDGE =TRUE;
			bOK = CyanShift();
		}
	}

	if ( ! bEDGE )
	{
		CopyLUTs( kDirectionNone, kDirectionYellow );
		CopyLUTs( kDirectionBlue, kDirectionNone );
		CopyLUTs( kDirectionCyan, kDirectionGreen );
		CopyLUTs( kDirectionMagenta, kDirectionRed );

		if ( GenerateNewLUTs( kDirectionBlue ) )
		{
			// we are on an outer facet between 300 and 0
			if ( ((m_angle > kPrimaryAngle[ kDirectionMagenta ]  && m_angle <= 360)  || m_angle == 0) && m_effectFraction == 1.0 )
			{
				CopyLUTs( kDirectionBlue, kDirectionMagenta );
				bOK = GenerateNewLUTs( kDirectionCyan );
			}
			// between 120 and 180
			else if ( (m_angle >= kPrimaryAngle[ kDirectionGreen ] && m_angle < kPrimaryAngle[ kDirectionCyan ]) && m_effectFraction == 1.0 )
			{
				CopyLUTs( kDirectionBlue, kDirectionCyan );
				bOK = GenerateNewLUTs( kDirectionMagenta );
			}
			else
			{
				if ( GenerateNewLUTs( kDirectionMagenta ) )
				{
					if ( GenerateNewLUTs( kDirectionCyan ) )
					{
						bOK = TRUE;
					}
				}
			}
		}
	}

	return bOK;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditAdjustCol::MagentaShift()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RTabbedEditAdjustCol::MagentaShift()
{
	BOOLEAN bEDGE = FALSE;
	BOOLEAN bOK = FALSE;

	if ( m_effectFraction == 1.0 )
	{
		// we are already between 240 and 300
		if ( m_angle >= kPrimaryAngle[ kDirectionBlue ]  && m_angle < kPrimaryAngle[ kDirectionMagenta ] )
		{
			bEDGE =TRUE;
			bOK = RedShift();
		}
		// between 300 and 0
		else if ( (m_angle > kPrimaryAngle[ kDirectionMagenta ] && m_angle <= 360) || m_angle == 0 )
		{
			bEDGE =TRUE;
			bOK = BlueShift();
		}
	}

	if ( ! bEDGE )
	{
		CopyLUTs( kDirectionNone, kDirectionGreen );
		CopyLUTs( kDirectionMagenta, kDirectionNone );
		CopyLUTs( kDirectionBlue, kDirectionCyan );
		CopyLUTs( kDirectionRed, kDirectionYellow );

		if ( GenerateNewLUTs( kDirectionMagenta ) )
		{
			// we are on an outer facet between 240 and 300
			if ( (m_angle > kPrimaryAngle[ kDirectionRed ]  && m_angle <= kPrimaryAngle[ kDirectionYellow ]) && m_effectFraction == 1.0 )
			{
				CopyLUTs( kDirectionMagenta, kDirectionRed );
				bOK = GenerateNewLUTs( kDirectionBlue );
			}
			// between 60 and 120
			else if ( (m_angle >= kPrimaryAngle[ kDirectionCyan ] && m_angle < kPrimaryAngle[ kDirectionBlue ]) && m_effectFraction == 1.0 )
			{
				CopyLUTs( kDirectionMagenta, kDirectionBlue );
				bOK = GenerateNewLUTs( kDirectionRed );
			}
			else
			{
				if ( GenerateNewLUTs( kDirectionRed ) )
				{
					if ( GenerateNewLUTs( kDirectionBlue ) )
					{
						bOK = TRUE;
					}
				}
			}
		}
	}

	return bOK;
}	

#endif	// DYNAMIC_PREVIEW_UPDATES


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  Class:	RAdjustColorAction - undoable action for Adjust Color
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
YActionId RAdjustColorAction::m_ActionId( "RAdjustColorAction" );

// ****************************************************************************
//
//  Function Name:	RAdjustColorAction::RAdjustColorAction( )
//
//  Description:	Constructor
//
//  Returns:		Nothing
//
// ****************************************************************************
//
RAdjustColorAction::RAdjustColorAction( RTabbedEditAdjustCol* pOwner, EActionType eAction, RHexColorGrid& rHexColorGrid )
	: RUndoableAction( m_ActionId, STRING_UNDO_CHANGE_SPECIAL_EFFECT, STRING_UNDO_CHANGE_SPECIAL_EFFECT ),
	  m_pOwnerDlg( pOwner ),
	  m_eAction(eAction),
	  m_hexColorGrid(rHexColorGrid)
{
}

// ****************************************************************************
//
//  Function Name:	RAdjustColorAction::Undo( )
//
//  Description:	Process a Undo command.
//
//  Returns:		Nothing
//
// ****************************************************************************
//
void RAdjustColorAction::Undo( )
{
	RUndoableAction::Undo( );

	if ( m_pOwnerDlg )
	{
		// call the dialog's undo callback
		m_pOwnerDlg->ProcessUndo( m_eAction, m_hexColorGrid );
	}
}

// ****************************************************************************
//
//  Function Name:	RAdjustColorAction::WriteScript( )
//
//  Description:	Fills in the action from the script
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RAdjustColorAction::WriteScript( RScript& /* script */) const
{
	return TRUE;
}

#ifdef	TPSDEBUG
// ****************************************************************************
//
//  Function Name:	RAdjustColorAction::Validate( )
//
//  Description:	Validate the object.
//
//  Returns:		Nothing
//
// ****************************************************************************
//
void RAdjustColorAction::Validate( ) const
{
	RUndoableAction::Validate( );
	TpsAssertIsObject( RAdjustColorAction, this );
}
#endif	//	TPSDEBUG
