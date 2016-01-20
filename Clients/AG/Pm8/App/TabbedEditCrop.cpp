// ****************************************************************************
//
//  File Name:		TabbedEditCrop.cpp
//
//  Project:		Renaissance Application Component
//
//  Author:			John Fleischhauer
//
//  Description:	Definition of the RTabbedEditCrop class
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

#include "TabbedEditCrop.h"
#include "TabbedEditImage.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "BitmapImage.h"
#include "Flyout.h"

#include "Buffer.h"
#include "PathDocument.h"
#include "PathView.h"

#include "ComponentManager.h"
#include "CompositeSelection.h"
#include "ReplaceComponentAction.h"
#include "ScaleSelectionToParentAction.h"
#include "DeleteSelectionAction.h"
#include "NewComponentAction.h"
#include "BitmapLoad.h"

#define TEST_CROP_FILE_PATH  "\\development\\Cropping Paths\\"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////

YActionId RApplyClipPathAction::m_ActionId( "RApplyClipPathAction" );

// ****************************************************************************
//
//  Function Name:	RApplyClipPathAction::RApplyClipPathAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		kUnknownError
//
// ****************************************************************************
//
RApplyClipPathAction::RApplyClipPathAction(	RComponentDocument* pComponent,
															RClippingPath* pClipPath ) :
	RUndoableAction( m_ActionId, STRING_UNDO_CHANGE_SPECIAL_EFFECT, STRING_UNDO_CHANGE_SPECIAL_EFFECT ),
	m_pComponent( pComponent )
{
	if (pClipPath)
		m_rPath = *pClipPath;

	RComponentView* pComponentView = (RComponentView *)
		m_pComponent->GetActiveView();

	if (pComponentView)
		m_rBoundingRect = pComponentView->GetBoundingRect();
}


// ****************************************************************************
//
//  Function Name:	RApplyClipPathAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RApplyClipPathAction::Do( )
{
	// Call the base method to setup the state properly
	RUndoableAction::Do( );

	try
	{
		RComponentView* pComponentView = (RComponentView *)
			m_pComponent->GetActiveView();

		REditImageInterface* pInterface = (REditImageInterface *)
			pComponentView->GetInterface( kEditImageInterfaceId );

		pInterface->GetClippingPath( m_rOldPath );

		if (m_rPath.IsDefined())
		{
			pInterface->SetClippingPath( &m_rPath );

			//	Invalidate the view
			m_pComponent->InvalidateAllViews();
		}
		else
		{
			pInterface->SetClippingPath( NULL );
		}

		delete pInterface;

		REditCropView* pCropView = dynamic_cast<REditCropView *>(
			pComponentView->GetParentView() );
		TpsAssert( pCropView, "Invalid view type!" );
		pCropView->DetermineComponentBoundingRect();
	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RApplyClipPathAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RApplyClipPathAction::Undo( )
{
	RUndoableAction::Undo( );

	try
	{
		RComponentView* pComponentView = (RComponentView *)
			m_pComponent->GetActiveView();

		REditImageInterface* pInterface = (REditImageInterface *)
			pComponentView->GetInterface( kEditImageInterfaceId );

		if (m_rOldPath.IsDefined())
		{
			pInterface->SetClippingPath( &m_rOldPath );
		}
		else
		{
			pInterface->SetClippingPath( NULL );
		}

		delete pInterface;

		pComponentView->SetBoundingRect( m_rBoundingRect );

		//	Invalidate the view
		((RView *) pComponentView)->Invalidate();
	}
	catch (...)
	{
	}
}

// ****************************************************************************
//
//  Function Name:	RApplyClipPathAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RApplyClipPathAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RApplyClipPathAction::Validate( )
//
//  Description:	Validate the object.
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RApplyClipPathAction::Validate( ) const
{
	RUndoableAction::Validate( );
	TpsAssertIsObject( RApplyClipPathAction, this );
}

#endif	//	TPSDEBUG

////////////////////////////////////////////////////////////////////////

YActionId RRotateImageAction::m_ActionId( "RRotateImageAction" );

// ****************************************************************************
//
//  Function Name:	RRotateImageAction::RRotateImageAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRotateImageAction::RRotateImageAction( RTabbedEditImage* pImageEdit, YAngle angle  ) :
	RUndoableAction( m_ActionId, STRING_UNDO_CHANGE_SPECIAL_EFFECT, STRING_UNDO_CHANGE_SPECIAL_EFFECT ),
	m_pImageEdit( pImageEdit ),
	m_yAngle( angle )
{
}


// ****************************************************************************
//
//  Function Name:	RRotateImageAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RRotateImageAction::Do( )
{
	// Call the base method to setup the state properly
	RUndoableAction::Do( );

	RComponentView* pComponentView = (RComponentView*)
		m_pImageEdit->GetComponent()->GetActiveView();
	m_rBoundingRect = pComponentView->GetBoundingRect();

	return DoRotation( m_yAngle );
}

// ****************************************************************************
//
//  Function Name:	RRotateImageAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRotateImageAction::Undo( )
{
	RUndoableAction::Undo( );

	m_pImageEdit->ResetImageToLastSavedState();

	DoRotation( -m_yAngle );

	RComponentView* pView = (RComponentView*)
		m_pImageEdit->GetComponent()->GetActiveView();

	pView->SetBoundingRect( m_rBoundingRect );
}

// ****************************************************************************
//
//  Function Name:	RRotateImageAction::DoRotation( )
//
//  Description:		Do the rotation at the specified angle
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RRotateImageAction::DoRotation( YAngle angle )
{

	try
	{
		RBitmapImage* pImage = m_pImageEdit->GetImage();
		RComponentView* pView = (RComponentView*)
			m_pImageEdit->GetComponent()->GetActiveView();

		REditImageInterface* pInterface = (REditImageInterface *)
			pView->GetInterface( kEditImageInterfaceId );

		RClippingPath rPath;
		pInterface->GetClippingPath( rPath );
		delete pInterface;

		RBitmapImage* pRotatedImage = new RBitmapImage;
		RBitmapImage rMaskImage;
		RImageLibrary().Rotate( *pImage, *pRotatedImage, rMaskImage, angle );

		if (rPath.IsDefined())
		{
			RRealVectorRect rImageRect( 0, 0, pImage->GetWidthInPixels() - 1,
				pImage->GetHeightInPixels() - 1 );

			R2dTransform xform;
			RRealPoint pt = rImageRect.GetCenterPoint();
			xform.PostRotateAboutPoint( pt.m_x, pt.m_y, angle );
			rImageRect *= xform;

			RRealRect& rRect = rImageRect.m_TransformedBoundingRect;

			xform.MakeIdentity();
			xform.PostRotateAboutPoint( pt.m_x, pt.m_y, -angle );
			xform.PostTranslate( -rRect.m_Left, -rRect.m_Top );
			
			RClippingPath rNewPath( rPath, xform );
			pRotatedImage->SetRPath( &rNewPath );
		}

		m_pImageEdit->SetImage( pRotatedImage );
		m_pImageEdit->ApplyImageEffects( pView, FALSE );

		RImageInterface* pImageInterface = static_cast<RImageInterface*>(
			pView->GetInterface( kImageInterfaceId ) );

		RRealSize rNewSize = pImageInterface->GetImage()->GetSizeInLogicalUnits();
		delete pImageInterface;

		RView* pParentView = pView->GetParentView();

		if (dynamic_cast<REditCropView *>( pParentView))
		{
			REditCropView* pCropView = (REditCropView *) pParentView;
			YScaleFactor scale = pCropView->GetScaleFactor();

			RRealSize rScale( scale, scale );
			rNewSize.Scale( rScale );

			pView->Resize( rNewSize );
			pCropView->DetermineComponentBoundingRect();
		}
		else
			pView->Resize( rNewSize );

		pParentView->UpdateScrollBars( kChangeObjectPosition );
		pParentView->Invalidate();
	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RRotateImageAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RRotateImageAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RRotateImageAction::Validate( )
//
//  Description:	Validate the object.
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRotateImageAction::Validate( ) const
{
	RUndoableAction::Validate( );
	TpsAssertIsObject( RRotateImageAction, this );
}

#endif	//	TPSDEBUG


////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(RTabbedEditCrop, CDialog)
	//{{AFX_MSG_MAP(RTabbedEditCrop)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_UNDO_BTN, OnUndo)
	ON_BN_CLICKED(IDC_ROTATE_BTN, OnRotate)
	ON_BN_CLICKED(IDC_ZOOMIN_BTN, OnZoomIn)
	ON_BN_CLICKED(IDC_ZOOMOUT_BTN, OnZoomOut)
	ON_BN_CLICKED(IDC_SHAPES_BTN, OnCutoutShapes)
	ON_BN_CLICKED(IDC_REMOVE_CROP_BTN, RemovePreviousPath)
	ON_BN_CLICKED(IDC_FREEHAND_BTN, OnCutoutFreeHand)
	ON_BN_CLICKED(IDC_CUTIT_BTN, OnApplyCropShape)
	ON_COMMAND_RANGE( ID_CUTOUT_CIRCLE, ID_CUTOUT_TRIANGLE, OnCutoutShape )
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_USER_SCRATCH_IMAGE_CHANGED, OnScratchImageChange )
	ON_MESSAGE( WM_USER_APPLY_EDITS_TO_IED_BITMAP, OnApplyEditsToIEDBitmap )
	ON_MESSAGE( WM_USER_CLEANUP_IED_TAB_FOR_RESET, OnCleanupForReset )
	ON_MESSAGE( WM_USER_RESET_IED_TAB, OnResetUpdate )
	ON_MESSAGE( WM_USER_ON_OK_FROM_IED, OnOKFromIED )
	ON_MESSAGE( WM_USER_UNDO_FROM_IED, OnUndoFromIED )
	ON_MESSAGE( WM_USER_ZOOMIN_FROM_IED, OnZoomInFromIED )
	ON_MESSAGE( WM_USER_ZOOMOUT_FROM_IED, OnZoomOutFromIED )
END_MESSAGE_MAP()

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::RTabbedEditCrop()
//
//  Description:	Constructor
//
//  Returns:		Nothing
//
// ****************************************************************************
//
RTabbedEditCrop::RTabbedEditCrop(CWnd* pParent /*=NULL*/)
	: CDialog(RTabbedEditCrop::IDD, pParent),
	  m_pComponentDocument(NULL),
	  m_pControlDocument(NULL),
	  m_pControlView(NULL),
	  m_pSelection( NULL ),
	  m_pFlyOut(NULL),
	  m_nCutoutSel(0)
{
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::~RTabbedEditCrop()
//
//  Description:	Destructor
//
//  Returns:		Nothing
//
// ****************************************************************************
//
RTabbedEditCrop::~RTabbedEditCrop()
{
	if (m_pSelection)
		delete m_pSelection;

	if (m_pControlDocument)
		delete m_pControlDocument;

	// note: these get cleaned up by the document destructor:
	//		m_pComponent
	//		m_pEditCropView
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::DoDataExchange()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
void RTabbedEditCrop::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHAPES_BTN, m_cButtonShapes);
	DDX_Control(pDX, IDC_FREEHAND_BTN, m_cButtonFreehand);
	DDX_Control(pDX, IDC_CUTIT_BTN, m_cButtonCutIt);
	DDX_Control(pDX, IDC_REMOVE_CROP_BTN, m_cButtonRemoveCrop);
	DDX_Control(pDX, IDC_ROTATE_BTN, m_cButtonRotate);
	DDX_Control(pDX, IDC_ZOOMIN_BTN, m_cButtonZoomIn);
	DDX_Control(pDX, IDC_ZOOMOUT_BTN, m_cButtonZoomOut);
	DDX_Control(pDX, IDC_UNDO_BTN, m_cButtonUndo);
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::PreTranslateMessage()
//
//  Description:		Handles various keystrokes for this dialog
//
//  Returns:			TRUE if the message is processed here
//
// ****************************************************************************
//
BOOL RTabbedEditCrop::PreTranslateMessage( MSG* pMsg )
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
		// zoom in
		else if ( pMsg->wParam == '+' )
		{
			OnZoomIn();
			bRet = TRUE;
		}
		// zoom out
		else if ( pMsg->wParam == '-' )
		{
			OnZoomOut();
			bRet = TRUE;
		}
	}
	else if ( pMsg->message == WM_KEYDOWN )
	{
		// Enter key processing
		if ( pMsg->wParam == VK_RETURN )
		{
			CWnd* pFocusWnd = GetFocus();
			if ( pFocusWnd == &m_cButtonShapes )
			{
				OnCutoutShapes();
				bRet = TRUE;
			}
			else if ( pFocusWnd == &m_cButtonFreehand )
			{
				OnCutoutFreeHand();
				bRet = TRUE;
			}
			else if ( pFocusWnd == &m_cButtonCutIt )
			{
				OnApplyCropShape();
				bRet = TRUE;
			}
			else if ( pFocusWnd == &m_cButtonRemoveCrop )
			{
				RemovePreviousPath();
				bRet = TRUE;
			}
			else if ( pFocusWnd == &m_cButtonRotate )
			{
				OnRotate();
				bRet = TRUE;
			}
			else if ( pFocusWnd == &m_cButtonZoomIn )
			{
				OnZoomIn();
				bRet = TRUE;
			}
			else if ( pFocusWnd == &m_cButtonZoomOut )
			{
				OnZoomOut();
				bRet = TRUE;
			}
			else if ( pFocusWnd == &m_cButtonUndo )
			{
				OnUndo();
				bRet = TRUE;
			}
			else if ( pFocusWnd == GetDlgItem( IDC_PREVIEW_AREA ) )
			{
				bRet = TRUE;
				GetParent()->SendMessage( WM_COMMAND, IDOK );
			}
		}
		else if ( pMsg->wParam == VK_TAB )
		{
			if ( GetFocus() == GetDlgItem( IDC_PREVIEW_AREA ) )
			{
				m_cButtonShapes.SetFocus();
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
//  Function Name:	RTabbedEditCrop::OnInitDialog() 
//
//  Description:	Sets up the dialog
//
//  Returns:		BOOL
//
// ****************************************************************************
//
BOOL RTabbedEditCrop::OnInitDialog() 
{
	// Call the base method
	BOOL bRet = CDialog::OnInitDialog();

	if ( bRet )
	{
		RWaitCursor waitCursor;
		InitializeDocument();

		// Set bitmap buttons
//	AutoLoad fails under Win95 / Win98 with VC6 build!
//		m_cButtonShapes.AutoLoad( IDC_SHAPES_BTN, this );
//		m_cButtonFreehand.AutoLoad( IDC_FREEHAND_BTN, this );
//		m_cButtonCutIt.AutoLoad( IDC_CUTIT_BTN, this );
//		m_cButtonRemoveCrop.AutoLoad( IDC_REMOVE_CROP_BTN, this );
//		m_cButtonZoomIn.AutoLoad( IDC_ZOOMIN_BTN, this );
//		m_cButtonZoomOut.AutoLoad( IDC_ZOOMOUT_BTN, this );
//		m_cButtonUndo.AutoLoad( IDC_UNDO_BTN, this );
//		m_cButtonRotate.AutoLoad( IDC_ROTATE_BTN, this );

		VERIFY( m_cButtonShapes.LoadBitmaps( IDB_PHOTOWORKSHOP_SHAPES_U, IDB_PHOTOWORKSHOP_SHAPES_D, IDB_PHOTOWORKSHOP_SHAPES_F, IDB_PHOTOWORKSHOP_SHAPES_X ) );
		m_cButtonShapes.SizeToContent();

		VERIFY( m_cButtonFreehand.LoadBitmaps( IDB_PHOTOWORKSHOP_FREEHAND_U, IDB_PHOTOWORKSHOP_FREEHAND_D, IDB_PHOTOWORKSHOP_FREEHAND_F, IDB_PHOTOWORKSHOP_FREEHAND_X ) );
		m_cButtonFreehand.SizeToContent();

		VERIFY( m_cButtonCutIt.LoadBitmaps( IDB_PHOTOWORKSHOP_CROPIT_U, IDB_PHOTOWORKSHOP_CROPIT_D, IDB_PHOTOWORKSHOP_CROPIT_F, IDB_PHOTOWORKSHOP_CROPIT_X ) );
		m_cButtonCutIt.SizeToContent();

		VERIFY( m_cButtonRemoveCrop.LoadBitmaps( IDB_PHOTOWORKSHOP_REMOVECROP_U, IDB_PHOTOWORKSHOP_REMOVECROP_D, IDB_PHOTOWORKSHOP_REMOVECROP_F, IDB_PHOTOWORKSHOP_REMOVECROP_X ) );
		m_cButtonRemoveCrop.SizeToContent();

		VERIFY( m_cButtonRotate.LoadBitmaps( IDB_PHOTOWORKSHOP_ROTATE_U, IDB_PHOTOWORKSHOP_ROTATE_D, IDB_PHOTOWORKSHOP_ROTATE_F, IDB_PHOTOWORKSHOP_ROTATE_X ) );
		m_cButtonRotate.SizeToContent();

		VERIFY( m_cButtonZoomIn.LoadBitmaps( IDB_PHOTOWORKSHOP_ZOOMIN_U, IDB_PHOTOWORKSHOP_ZOOMIN_D, IDB_PHOTOWORKSHOP_ZOOMIN_F, IDB_PHOTOWORKSHOP_ZOOMIN_X ) );
		m_cButtonZoomIn.SizeToContent();

		VERIFY( m_cButtonZoomOut.LoadBitmaps( IDB_PHOTOWORKSHOP_ZOOMOUT_U, IDB_PHOTOWORKSHOP_ZOOMOUT_D, IDB_PHOTOWORKSHOP_ZOOMOUT_F, IDB_PHOTOWORKSHOP_ZOOMOUT_X ) );
		m_cButtonZoomOut.SizeToContent();

		VERIFY( m_cButtonUndo.LoadBitmaps( IDB_PHOTOWORKSHOP_UNDO_U, IDB_PHOTOWORKSHOP_UNDO_D, IDB_PHOTOWORKSHOP_UNDO_F, IDB_PHOTOWORKSHOP_UNDO_X ) );
		m_cButtonUndo.SizeToContent();
	}

	return bRet;
}

//*****************************************************************************
//
// Function Name:  RTabbedEditCrop::InitializeDocument
//
// Description:    Initializes the control document for the preview,
//                 as well obtains an image component from the parent
//                 for display in the control document.
//
// Returns:        VOID
//
// Exceptions:	    kUnknownError
//
//*****************************************************************************
void RTabbedEditCrop::InitializeDocument()
{
	try
	{
		// Create our document...
		//
		m_pControlDocument	= new REditCropDocument( this );

		// Create a view for the document...
		//
		m_pControlView = (REditCropView*) m_pControlDocument->CreateView( this, IDC_PREVIEW_AREA ) ;
		m_pControlDocument->AddRView( m_pControlView );
		m_rPreviewArea = m_pControlView->GetViewableArea();

		RComponentAttributes attr ;
		attr.SetSelectable( FALSE ) ;

		// get a pointer to the IED component
		RTabbedEditImage* pParentDlg = (RTabbedEditImage *) GetParent();
		m_pComponentDocument = pParentDlg->GetComponent();
	
		if ( m_pComponentDocument )
		{
			m_pComponentDocument->SetComponentAttributes( attr );
			RComponentView*  pView = (RComponentView*) m_pComponentDocument->GetActiveView();

			// need to do this to set parent
			m_pControlDocument->AddComponent( m_pComponentDocument );

			// Setup the control view with the ability to draw non-cropped portions of the image
			m_pControlView->SetImageInfo( pView, pParentDlg->GetImage() );
			m_pControlView->DetermineComponentBoundingRect( TRUE );

			RImageInterface* pImageInterface = static_cast<RImageInterface*>(
				pView->GetInterface( kImageInterfaceId ) );

			if (pImageInterface)
			{
				RBitmapImage* pImage = (RBitmapImage*) pImageInterface->GetImage();
				delete pImageInterface;

				// always disable Crop It button at first
				CWnd* pWnd = GetDlgItem( IDC_CUTIT_BTN );
				pWnd->EnableWindow( FALSE );

				// enable Remove Crop button if there is an incoming path
				RClippingPath* pPath = pImage->GetClippingRPath();
				pWnd = GetDlgItem( IDC_REMOVE_CROP_BTN );
				if ( pPath && pPath->IsDefined() && ! pImage->PathFromImport() )
				{
					pWnd->EnableWindow( TRUE );
				}
				else
				{
					pWnd->EnableWindow( FALSE );
				}

				// if imported clipping path
				if (pImage->PathFromImport())
				{
					// Disable Shapes and Freehand buttons
					pWnd = GetDlgItem( IDC_SHAPES_BTN );
					pWnd->EnableWindow( FALSE );
					pWnd = GetDlgItem( IDC_FREEHAND_BTN );
					pWnd->EnableWindow( FALSE );
				}

				// we will not rotate frames / photo album pages
				if ( pParentDlg->ImageHasFramePath() )
				{
					pWnd = GetDlgItem( IDC_ROTATE_BTN );
					pWnd->EnableWindow( FALSE );
				}
			}

			// this will get reset in OnShowWindow()
			m_pControlDocument->RemoveComponent( m_pComponentDocument );
			m_pComponentDocument = NULL;

//			m_pControlView->SetImageComponent( pView );
//			m_pControlView->SetImage( pParentDlg->GetImage() );
		}
	}
	catch (...)
	{
		throw kUnknownError ;
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnShowWindow()
//
//  Description:	sets up display of the dialog window
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditCrop::OnShowWindow(  BOOL bShow, UINT nStatus )
{
	if ( bShow && ! nStatus )
	{
		// get updated edit info from parent here
		// ...

		// get the mother component and add it to our view
		RTabbedEditImage* pParentDlg = (RTabbedEditImage *) GetParent();
		m_pComponentDocument = pParentDlg->GetComponent();

		m_pControlDocument->AddComponent( m_pComponentDocument );

		RComponentView* pComponentView = (RComponentView *)
			m_pComponentDocument->GetActiveView();

		// Set Z-Order
		m_pControlView->SendToBack( pComponentView );

		m_pControlDocument->FreeAllActions();

		m_cButtonUndo.EnableWindow( FALSE );

		m_pControlView->SetImageInfo( pComponentView, pParentDlg->GetImage() );
		m_pControlView->DetermineComponentBoundingRect();
	}
	else
	{
		RTabbedEditImage* pParentDlg = (RTabbedEditImage *) GetParent();

		if (m_pControlView->IsImageTinted())
			pParentDlg->ResetImageToLastSavedState();

		m_pControlView->SetImageInfo( NULL, NULL );

		// do this so tabs can share the mother component
		if (m_pControlDocument && m_pComponentDocument)
		{
			RComponentView* pComponentView = (RComponentView *)
				m_pComponentDocument->GetActiveView();

			m_rBoundingRect = pComponentView->GetBoundingRect();

			// Set the current clipping path into the parent dialogs image
			//
			UpdateParentsClipPath();

			// Remove the component from our control 
			m_pControlDocument->RemoveComponent( m_pComponentDocument );
		}

		m_pComponentDocument = NULL;
	}

	// handle the Revert and OK buttons in the parent, too
	if ( m_pControlDocument->UndoCount() > 0 )
	{
		// if we are leaving this tab with a change, lock the buttons enabled
		((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( TRUE, TRUE );
	}
	else
	{
		m_cButtonUndo.EnableWindow( FALSE );

		// handle the Revert and OK buttons in the parent, too
		((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( FALSE );
	}
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::RemovePreviousPath()
//
//  Description:	removes existing path (if any) from the 
//                image component
//
//  Returns:		
//
// ****************************************************************************
//
void RTabbedEditCrop::RemovePreviousPath()
{
	// See if there is already a clipping path selected
	//
	RComponentView* pComponentView = (RComponentView *)
		m_pComponentDocument->GetActiveView();
	RImageInterface* pImageInterface = static_cast<RImageInterface*>(
		pComponentView->GetInterface( kImageInterfaceId ) );
	RBitmapImage* pImage = (RBitmapImage *) pImageInterface->GetImage();
	delete pImageInterface;

	RPath* pPrevPath = pImage->GetClippingRPath();

	if (pPrevPath)
	{
		if (!pImage->PathFromImport())
		{
			// Remove the existing path
			RUndoableAction* pAction = new RApplyClipPathAction( 
				m_pComponentDocument, NULL );

			m_pControlDocument->SendAction( pAction );
		}
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::ShowCropShape()
//
//  Description:	show a "cookie cutter" RPath in the preview window
//
//  Returns:		
//
// ****************************************************************************
//
BOOL RTabbedEditCrop::ShowCropShape( UINT nID )
{

	BOOLEAN fResult = FALSE;

	HMODULE hModule = AfxGetResourceHandle();
	HRSRC hrsrc = FindResource( hModule, MAKEINTRESOURCE( nID ), "CROPPATH" ) ;

	if (hrsrc)
	{
		HGLOBAL hData = LoadResource( hModule, hrsrc ) ;
		RBitmapImage* pImage = NULL;

		try
		{
			if (hData)
			{
				LPUBYTE data = (LPUBYTE) LockResource( hData ) ;

				RBuffer rBuffer;
				rBuffer.Resize( SizeofResource( hModule, hrsrc ) );
				memcpy( rBuffer.GetBuffer(), data, rBuffer.GetBufferSize() );

				RImageLibrary rLibrary;
				pImage = dynamic_cast<RBitmapImage*>(
					rLibrary.ImportImage( rBuffer ) );

				RPath* pCropPath = pImage->GetClippingRPath();

				if (pCropPath->IsDefined())
				{
					R2dTransform xform;
					xform.PostScale( kSystemDPI / 200.0, kSystemDPI / 200.0 );
					RRealRect rRect( (RRealSize) pCropPath->GetPathSize() );
					rRect *= xform;

					YComponentBoundingRect rBoundingRect( rRect );
					RComponentAttributes attr;
					attr.SetClipToParent( TRUE );
					attr.SetRotatable( FALSE );

					RPathDocument* pPathDocument = new RPathDocument( NULL, attr, kPathComponent, FALSE );
					RComponentView* pCropView = pPathDocument->CreateView( rBoundingRect, 
						m_pControlView );

					pPathDocument->AddRView( pCropView );
					pPathDocument->SetPath( *pCropPath, xform, FALSE );

					// Look for an existing path component
					RComponentDocument* pPrevDoc = m_pControlDocument->GetDocumentOfType( kPathComponent );
					RCompositeSelection* pSelection = m_pControlView->GetSelection();

					if (pPrevDoc)
					{
						RComponentView* pPrevView = (RComponentView *)
							pPrevDoc->GetActiveView();

						if (!pSelection->IsSelected( pPrevView ))
							pSelection->Select( pPrevView, FALSE );

						RUndoableAction* pAction = new RReplaceComponentAction(	\
							pSelection,	pPrevDoc, pPathDocument, 
							STRING_UNDO_CHANGE_SPECIAL_EFFECT );

						m_pControlDocument->SendAction( pAction );
					}
					else
					{

						try
						{
							m_pControlDocument->BeginMultiUndo();

							m_pControlView->RemovePoints();
							m_pControlView->SetMode( REditCropView::kDefault );

							RemovePreviousPath();

							RUndoableAction* pAction = new RNewComponentAction( 
								pPathDocument, m_pControlDocument, pSelection, 
								STRING_UNDO_CHANGE_SPECIAL_EFFECT );
							
							m_pControlDocument->SendAction( pAction );

							RRealRect rParentRect( m_pControlView->GetUseableArea(FALSE) );
							RRealSize szOffset( rParentRect.m_Left, rParentRect.m_Top );
							pCropView->FitInsideParent();
							pCropView->Offset( szOffset, FALSE );

							RComponentView* pComponentView = (RComponentView *)
								m_pComponentDocument->GetActiveView();
							RRealRect rImageRect = pComponentView->GetBoundingRect().m_TransformedBoundingRect;
							rImageRect.Inflate( RIntSize( 1, 1 ) );

							YComponentBoundingRect rect = pCropView->GetBoundingRect();

							if (!rImageRect.RectInRect( rect.m_TransformedBoundingRect ))
							{
								RRealPoint pt( rect.m_TransformedBoundingRect.GetCenterPoint() );

								RRealSize scale( 
									(rImageRect.Width()  - 2) / rect.m_TransformedBoundingRect.Width(),
									(rImageRect.Height() - 2) / rect.m_TransformedBoundingRect.Height() );

								if (scale.m_dx < 1.0 || scale.m_dy < 1.0)
								{
									if (scale.m_dx > scale.m_dy)
										scale.m_dx = scale.m_dy;
									else
										scale.m_dy = scale.m_dx;
								
									rect.ScaleAboutPoint( pt, scale );
								}

								pCropView->SetBoundingRect( rect );
							}

//							pAction = new RScaleSelectionToParentAction( 
//								pSelection, TRUE );
//
//							m_pControlDocument->SendAction( pAction );

							m_pControlDocument->EndMultiUndo( 
								STRING_UNDO_CHANGE_SPECIAL_EFFECT, 
								STRING_UNDO_CHANGE_SPECIAL_EFFECT );

						}
						catch (...)
						{
							m_pControlDocument->CancelMultiUndo();

							throw;
						}
					}
		
//					m_pControlDocument->AddComponent( m_pPathDocument );
//					m_pPathDocument->SetPath( *pCropPath, xform, TRUE );
//
//					RRealRect rRectArea = m_pControlView->GetViewableArea() ;
//					pCropView->Offset( rRectArea.GetCenterPoint() - rRect.GetCenterPoint() );

					fResult = TRUE;
					((RView *) m_pControlView)->Invalidate( TRUE );
				}
			}
		}
		catch (...)
		{
		}
		
		if (pImage)
			delete pImage;

		UnlockResource( hData ) ;
		FreeResource( hData ) ;
	}

	return fResult;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnZoomIn()
//
//  Description:	passes zoom in command to the preview control
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditCrop::OnZoomIn()
{
	if ( m_pControlView )
	{
		// do the zoom
		m_pControlView->ZoomIn();

		// if we hit the max
		if ( m_pControlView->IsMaxZoomLevel() )
		{
			// switch focus before disabling
			if ( GetFocus() == &m_cButtonZoomIn )
			{
				m_cButtonZoomOut.SetFocus();
			}
	
			m_cButtonZoomIn.EnableWindow( FALSE );
		}
		else
		{
			m_cButtonZoomIn.EnableWindow( TRUE );
		}

		// enable zoom out button
		m_cButtonZoomOut.EnableWindow( TRUE );
	}
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnZoomOut()
//
//  Description:	passes zoom out command to the preview control
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditCrop::OnZoomOut()
{
	if ( m_pControlView )
	{
		// do the zoom
		m_pControlView->ZoomOut();

		// if we hit the min
		if ( m_pControlView->IsMinZoomLevel() )
		{
			// switch focus before disabling
			if ( GetFocus() == &m_cButtonZoomOut )
			{
				m_cButtonZoomIn.SetFocus();
			}

			m_cButtonZoomOut.EnableWindow( FALSE );
		}
		else
		{
			m_cButtonZoomOut.EnableWindow( TRUE );
		}

		// enable zoom in button
		m_cButtonZoomIn.EnableWindow( TRUE );
	}
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnShapes()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditCrop::OnCutoutShapes()
{
	CWnd* pButtonWnd =  GetDlgItem(IDC_SHAPES_BTN);
	CRect rcButton;
	pButtonWnd->GetWindowRect( &rcButton );
	CPoint ptLoc(rcButton.right, rcButton.top);

//	m_pControlView->SetMode( REditCropView::kDefault );

	if(m_pFlyOut)
		delete m_pFlyOut;
	m_pFlyOut = new RFlyout(3, TOOLBAR_CUTOUT, this);

	m_pFlyOut->Create(IDD_FLYOUT, this );	
	m_pFlyOut->SetWindowPos(&wndTop, ptLoc.x, ptLoc.y , 0,0,SWP_NOSIZE | SWP_NOZORDER );
	m_pFlyOut->SetButtonState(m_nCutoutSel, TBSTATE_CHECKED | TBSTATE_ENABLED );
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnFreeHand()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditCrop::OnCutoutFreeHand()
{
	m_nCutoutSel = 0;

	m_pControlView->RemovePoints();
	m_pControlView->SetMode( REditCropView::kCollectPoints );

	RPathDocument* pPathDocument = (RPathDocument *)
		m_pControlDocument->GetDocumentOfType( kPathComponent );

	if (pPathDocument)
	{
		RCompositeSelection* pSelection = m_pControlView->GetSelection();
		RComponentView* pView = (RComponentView *) pPathDocument->GetActiveView();

		if (!pSelection->IsSelected( pView ))
			pSelection->Select( pView, FALSE );

		RUndoableAction* pAction = new RDeleteSelectionAction( pSelection );
		m_pControlDocument->SendAction( pAction );
	}
	else
	{
		RemovePreviousPath();
	}
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnCutoutShape()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditCrop::OnCutoutShape( UINT nID )
{
//	m_nCutoutSel = nID;
	m_nCutoutSel = 0;

	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	ShowCropShape( nID );
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnApplyCropShape()
//
//  Description:	applies the RPath to the image
//
//  Returns:			TRUE if the crop was applied
//
// ****************************************************************************
//
BOOLEAN RTabbedEditCrop::OnApplyCropShape()
{
	BOOLEAN bOK = TRUE;

	R2dTransform xform;
	RPath *pCropPath = NULL;
	RPath rPath;

	RComponentView* pImageView = (RComponentView*)
		m_pComponentDocument->GetActiveView();

	pImageView->ApplyTransform( xform, FALSE, FALSE );

	RImageInterface* pInterface = static_cast<RImageInterface *>(
		pImageView->GetInterface( kImageInterfaceId ) );
	RImage* pImage = pInterface->GetImage();
	delete pInterface;

	RRealSize szImageSize = pImage->GetSizeInLogicalUnits();

	// The component scaling doesn't always happen
	// through the bounding rect transform.  So, we
	// need to add any additional scaling base on
	// the bounding rects untransformed size.
	RRealRect rectImage( pImageView->GetBoundingRect().GetUntransformedSize( ) );
	RRealPoint ptCenter = rectImage.GetCenterPoint();

	xform.PreScale(
		rectImage.Width()  / szImageSize.m_dx,
		rectImage.Height() / szImageSize.m_dy );

	xform.Invert();

	rectImage = szImageSize;
	RPathDocument* pPathDocument = (RPathDocument *)
		m_pControlDocument->GetDocumentOfType( kPathComponent );

	RRealRect rcBoundingRect( 0, 0, 0, 0 );

	if (pPathDocument)
	{
		RComponentView* pPathView  = (RComponentView*)
			pPathDocument->GetActiveView();

		rcBoundingRect = (pPathView->GetBoundingRect()).m_TransformedBoundingRect;

		RRealRect rectPath( pPathView->GetReferenceSize() );
	
		pPathView->ApplyTransform( xform, FALSE, FALSE );
		rectPath *= xform;

		// Because the path is upside down, we need to 
		// negate the angle.
		YAngle angle;
		YRealDimension xScale, yScale;
		xform.Decompose( angle, xScale, yScale );

		RRealPoint pt = rectPath.GetCenterPoint();
		xform.PostRotateAboutPoint( pt.m_x, pt.m_y, -2 * angle );

		xform.PostTranslate( 0, rectImage.m_Bottom - rectPath.m_Bottom - rectPath.m_Top );
		xform.PostScale( 200.0 / kSystemDPI, 200.0 / kSystemDPI );

		// Compute initial image scale
		pCropPath = pPathDocument->GetPath();
	}
	else if (m_pControlView->PointsToPath( rPath, FALSE ))
	{
		rcBoundingRect = rPath.GetBoundingRect();
		pCropPath = &rPath;

		// Okay, the points are based on the center of the
		// image.  We need to transform the points to position
		// them correctly in the image.
		RRealRect rectPath( rPath.GetBoundingRect() );
		RRealPoint pt = rectImage.GetCenterPoint();

		// We need to ignore the original image offset
		// as the positioning translation used below is
		// base on the actual image center, not the 
		// location of the image within its parent.
		YRealDimension xOffset, yOffset;
		xform.GetTranslation( xOffset, yOffset );

		xform.PostTranslate( pt.m_x - xOffset, pt.m_y - yOffset );
		xform.PostScale( 200.0 / kSystemDPI, 200.0 / kSystemDPI );
	}

	if (pCropPath && pCropPath->IsDefined())
	{
		// apply the transform to the path
		RClippingPath rPath( *pCropPath, xform, FALSE );

		// check the path size for zero deimensions
		RIntRect rPathBounds = pCropPath->GetBoundingRect();
		if ( rPathBounds.Width() <= 1 || rPathBounds.Height() <= 1 )
		{
			bOK = FALSE;
		}
		else
		{
			// check the transformed path, too
			rPathBounds = rPath.GetBoundingRect();
			if ( rPathBounds.Width() <= 1 || rPathBounds.Height() <= 1 )
			{

				bOK = FALSE;
			}
		}

		if ( ! bOK )
		{
			// Let the user know the crop area is too small
			RAlert rAlert;
			rAlert.AlertUser( STRING_ERROR_CROP_AREA_TOO_SMALL );
			
			// get rid of the path
			RemoveUnappliedCropPath();
		}
		else
		{
			RImageInterface *pImageInterface = dynamic_cast<RImageInterface*>(pImageView->GetInterface(kImageInterfaceId));

			RImage *pImage = pImageInterface->GetImage();
			RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>(pImage);

			rPath.SetPathSize( RIntSize(
				pBitmap->GetWidthInPixels(),
				pBitmap->GetHeightInPixels() ) );

//			pBitmap->SetRPath( &rPath, FALSE );
			delete pImageInterface;

			// Begin a multi undo, as we are replacing
			// the image, and then resizing the component
			// to fit the parent.
			m_pControlDocument->BeginMultiUndo();

			try
			{
				if (!pPathDocument)
				{
					// The path must be freehand, so remove the 
					// freehand points.  Note: this function 
					// generates an undoable action, so it needs
					// to be after the BeginMultiUndo.
					m_pControlView->RemovePoints();
				}

				RUndoableAction* pAction = new RApplyClipPathAction( 
					m_pComponentDocument, &rPath );

				m_pControlDocument->SendAction( pAction );

				if (pPathDocument)
				{
					RCompositeSelection* pSelection = m_pControlView->GetSelection();
					RComponentView* pView = (RComponentView *) pPathDocument->GetActiveView();

					if (!pSelection->IsSelected( pView ))
						pSelection->Select( pView, FALSE );

					pAction = new RDeleteSelectionAction( pSelection );
					m_pControlDocument->SendAction( pAction );
				}

				m_pControlDocument->EndMultiUndo( 
					STRING_UNDO_CHANGE_SPECIAL_EFFECT, 
					STRING_UNDO_CHANGE_SPECIAL_EFFECT );
			}
			catch (...)
			{
				m_pControlDocument->CancelMultiUndo();
				bOK = FALSE;
			}
		}
	}

	((RView *) m_pControlView)->Invalidate( TRUE );
	return bOK;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnDestroy()
//
//  Description:	applies the RPath to the image
//
//  Returns:		
//
// ****************************************************************************
//
void RTabbedEditCrop::OnDestroy( )
{
	if (m_pControlDocument && m_pComponentDocument)
		m_pControlDocument->RemoveComponent( m_pComponentDocument );

	CDialog::OnDestroy();
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnRotate()
//
//  Description:	Rotates the image by 90 degrees
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditCrop::OnRotate()
{
	if ( m_pComponentDocument )
	{
		RWaitCursor waitCursor;

		// Begin a multi undo, as we are replacing
		// the image, and then resizing the component
		// to fit the parent.
//		m_pControlDocument->BeginMultiUndo();

		if (!m_pSelection)
		{
			// We need a selection object for rotating
			// the image, as the rotate action requires
			// a selection, and we can't use the control
			// view's selection object cause we don't 
			// want the image component to be selected.
			m_pSelection = new RCompositeSelection( m_pControlView );
		}
		else
		{
			m_pSelection->UnselectAll( FALSE );
		}

		try
		{
			// Create the undo action for changing the image.  Remove
			// tint from scratch image before sending the action, as
			// this action will replace the image used in the control
			RTabbedEditImage* pParentDlg = (RTabbedEditImage *) GetParent();

			if (m_pControlView->IsImageTinted())
				pParentDlg->ResetImageToLastSavedState();

			// Begin a multi undo, as we are deleting any unapplied crop shape
			// the image, and then rotating the component
			m_pControlDocument->BeginMultiUndo();

			// first get rid of unapplied path, if any
			RemoveUnappliedCropPath();

			// now do the rotate
			RUndoableAction* pAction = new RRotateImageAction(	pParentDlg, -kPI / 2 );
			m_pControlDocument->SendAction( pAction );

			m_pControlDocument->EndMultiUndo( STRING_UNDO_CHANGE_SPECIAL_EFFECT, STRING_UNDO_CHANGE_SPECIAL_EFFECT );
			
/*			//
			// Get the image and rotate it.
			//
			RComponentView* pComponentView = (RComponentView *)
				m_pComponentDocument->GetActiveView();

			RImageInterface* pInterface = (RImageInterface *)
				pComponentView->GetInterface( kImageInterfaceId );

			RBitmapImage* pImage = static_cast<RBitmapImage *>(
				pInterface->GetImage() );

			RBitmapImage rRotatedImage;
			RBitmapImage rMaskImage;
			RImageLibrary rLibrary;
			rLibrary.Rotate( *pImage, rRotatedImage, rMaskImage, kPI / 2 );

			// Set the image
			pInterface->SetImage( rRotatedImage );
			delete pInterface ;
*/			
			//
			// Scale the new component to fit the parent.
			//
//			RComponentView* pComponentView = (RComponentView *)
//				m_pComponentDocument->GetActiveView();

//			DetermineComponentBoundingRect();
//			pComponentView->SetBoundingRect( m_rBoundingRect );

#if 0
			RComponentView* pComponentView = (RComponentView *)
				m_pComponentDocument->GetActiveView();

			RComponentAttributes attr = m_pComponentDocument->GetComponentAttributes();
			
			attr.SetSelectable( TRUE );
			m_pComponentDocument->SetComponentAttributes( attr );
			m_pSelection->Select( pComponentView, FALSE );

			attr.SetSelectable( FALSE );
			m_pComponentDocument->SetComponentAttributes( attr );

			YZoomLevel oldZoom = m_pControlView->GetZoomLevel();
			m_pControlView->SetZoomLevel( 1.0, FALSE );

//			pAction = new RScaleSelectionToParentAction( 
//				m_pSelection, TRUE );
//
//			m_pControlDocument->SendAction( pAction );

			m_pControlView->SetZoomLevel( oldZoom, FALSE );
#endif

//			m_pControlDocument->EndMultiUndo( 
//				STRING_UNDO_ROTATE_SELECTION, 
//				STRING_REDO_ROTATE_SELECTION );

			m_pControlDocument->InvalidateAllViews();
		}
		catch (...)
		{
			m_pControlDocument->CancelMultiUndo();
		}
	}
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnUndo()
//
//  Description:	Undoes the last action
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditCrop::OnUndo()
{
	if (m_pControlDocument)
	{
		// if we got only one undo left
		if ( m_pControlDocument->UndoCount() == 1 )
		{
			// and focus is currently on the undo button
			if ( GetFocus() == &m_cButtonUndo )
			{
				// change focus to the default button before we disable the undo button
				m_cButtonShapes.SetFocus();
			}
		}

		if ( m_pControlDocument->UndoCount() > 0 )
		{
			m_pControlDocument->Undo();
		}
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnScratchImageChange()
//
//  Description:		WM_USER_SCRATCH_IMAGE_CHANGED message handler
//
//  Returns:			nothing
//
// ****************************************************************************
//
LONG RTabbedEditCrop::OnScratchImageChange( WPARAM, LPARAM )
{
	if (m_pComponentDocument)
	{
		RTabbedEditImage* pParentDlg = (RTabbedEditImage *) GetParent();
		RComponentView* pComponentView = (RComponentView *)
			m_pComponentDocument->GetActiveView();

		m_pControlView->SetImageInfo( pComponentView, pParentDlg->GetImage() );
	}

	return 0L;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnCleanupForReset()
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
LONG RTabbedEditCrop::OnCleanupForReset( WPARAM, LPARAM )
{
	// first get rid of unapplied path, if any
	RemoveUnappliedCropPath();

	// clean up
	if (m_pControlDocument && m_pComponentDocument)
	{
		m_pControlDocument->RemoveComponent( m_pComponentDocument );
	}
	m_pComponentDocument = NULL;

	m_pControlView->SetZoomLevel( 1.0, TRUE );
	m_cButtonZoomIn.EnableWindow( TRUE );
	m_cButtonZoomOut.EnableWindow( TRUE );

	return 0;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnResetUpdate()
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
LONG RTabbedEditCrop::OnResetUpdate( WPARAM, LPARAM )
{
	// reinitialize the window
	OnShowWindow( TRUE, 0 );
	
	return 0;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnApplyEditsToIEDBitmap()
//
//  Description:	using the current settings, apply the edits to the
//					intermediate bitmap owned by the base IED dialog
//
//  Returns:		nothing
//
// ****************************************************************************
//
LONG RTabbedEditCrop::OnApplyEditsToIEDBitmap( WPARAM, LPARAM )
{
	if (m_pControlView->IsImageTinted())
	{
		RTabbedEditImage* pParentDlg = (RTabbedEditImage *) GetParent();
		pParentDlg->ResetImageToLastSavedState();

		RComponentView* pView = (RComponentView *) m_pComponentDocument->GetActiveView();
		m_pControlView->SetImageInfo( pView, pParentDlg->GetImage() );
	}

	return 0;
}

#if 0
// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnOK()
//
//  Description:	IDOK message handler
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditCrop::OnOK()
{
	OnApplyCropShape();
	UpdateParentsClipPath();

	CDialog::OnOK();
}
#endif

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::OnOKFromIED()
//
//  Description:		handles M_USER_ON_OK_FROM_IED message, sent by parent
//							RTabbedEditImage dialog when user presses the OK button
//
//  Returns:			1 if successful, 0 if crop cannot be applied
//
// ****************************************************************************
//
LONG RTabbedEditCrop::OnOKFromIED( WPARAM, LPARAM )
{
	LONG lRet = 0;

	if ( OnApplyCropShape() )
	{
		UpdateParentsClipPath();
		lRet = 1;
	}

	return lRet;
}

void RTabbedEditCrop::UpdateParentsClipPath()
{
	RTabbedEditImage* pParentDlg = (RTabbedEditImage *) GetParent();

	try
	{
		RComponentView* pComponentView = (RComponentView *)
			m_pComponentDocument->GetActiveView();

		REditImageInterface* pImageEditInterface = static_cast<REditImageInterface*>(
			pComponentView->GetInterface( kEditImageInterfaceId ));

		RClippingPath rPath;
		pImageEditInterface->GetClippingPath( rPath );
		delete pImageEditInterface;

		pParentDlg->GetImage()->SetRPath( &rPath );
	}
	catch (...)
	{
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::RemoveUnappliedCropPath()
//
//  Description:		figure out if we have either a shape or freehand path, and remove it
//
//  Returns:			nothing
//
// ****************************************************************************
//
void RTabbedEditCrop::RemoveUnappliedCropPath()
{
	RPathDocument* pPathDocument = (RPathDocument *)m_pControlDocument->GetDocumentOfType( kPathComponent );
	if (pPathDocument)
	{
		RCompositeSelection* pSelection = m_pControlView->GetSelection();
		RComponentView* pView = (RComponentView *)pPathDocument->GetActiveView();

		if ( ! pSelection->IsSelected( pView ) )
		{
			pSelection->Select( pView, FALSE );
		}

		RUndoableAction* pAction = new RDeleteSelectionAction( pSelection );
		m_pControlDocument->SendAction( pAction );
	}
	else
	{
		m_pControlView->RemovePoints();
		m_pControlView->SetMode( REditCropView::kDefault );
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditCrop::UpdateButtonControls()
//
//  Description:		disables or enables the Crop It and Remove Crop buttons,
//							and the Revert and OK buttons	parent dialog
//
//  Returns:			nothing
//
// ****************************************************************************
//
void RTabbedEditCrop::UpdateButtonControls()
{
	//	handle enabling / disabling of Undo, Revert and OK buttons,
	//	based on whether there is something to undo
	if ( m_pControlDocument->UndoCount() > 0 )
	{
		m_cButtonUndo.EnableWindow( TRUE );
		((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( TRUE );
	}
	else
	{
		m_cButtonUndo.EnableWindow( FALSE );
		((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( FALSE );
	}

	// now see if we have an unapplied crop path,
	REditCropView *pView = (REditCropView *)m_pControlDocument->GetActiveView();
	if ( pView->HasPoints() || m_pControlDocument->GetDocumentOfType( kPathComponent ) )
	{
		// if so, we should be able to crop
		m_cButtonCutIt.EnableWindow( TRUE );
		
		// if the RemoveCrop button has focus, change it to the CropIt button before we disable
		if ( GetFocus() == &m_cButtonRemoveCrop )
		{
			m_cButtonCutIt.SetFocus();
		}
	
		// and disable the RemoveCrop button
		m_cButtonRemoveCrop.EnableWindow( FALSE );
	}
	else			// no unapplied crop
	{
		// if there is an applied crop, be sure the RemoveCrop capability is enabled
		if ( pView->IsCropped() )
		{
			m_cButtonRemoveCrop.EnableWindow( TRUE );
		}
		else
		{
			// otherwise, if the RemoveCrop button has focus, change it before we disable it
			if ( GetFocus() == &m_cButtonRemoveCrop )
			{
				m_cButtonShapes.SetFocus();
			}

			m_cButtonRemoveCrop.EnableWindow( FALSE );
		}

		// if the CropIt button has focus change it before we disable it
		if ( GetFocus() == &m_cButtonCutIt )
		{
			if ( pView->IsCropped() )
			{
				m_cButtonRemoveCrop.SetFocus();
			}
			else
			{
				m_cButtonShapes.SetFocus();
			}
		}

		// finally, disable the CropIt button
		m_cButtonCutIt.EnableWindow( FALSE );
	}
}
