// ****************************************************************************
//
//  File Name:		TabbedEditFixFlaw.cpp
//
//  Project:		Renaissance Application Component
//
//  Author:			John Fleischhauer
//
//  Description:	Definition of the RTabbedEditFixFlaw class
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

#include "Resource.h"
//#include "RenaissanceResource.h"
#include "flyout.h"
#include "TabbedEditFixFlaw.h"
#include "TabbedEditImage.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "BitmapImage.h"
#include "BitmapLoad.h"
#include "UndoManager.h"
#include "ResourceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Class Name:		RTabbedEditFixFlaw
//
//  Description:	Fix Flaw dialog
//
///////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(RTabbedEditFixFlaw, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_COMMAND(ID_REDEYE_SM, OnRedEyeSm)
	ON_COMMAND(ID_REDEYE_MED, OnRedEyeMed)
	ON_COMMAND(ID_REDEYE_LG, OnRedEyeLg)
	ON_COMMAND(ID_PETEYE_SM, OnPetEyeSm)
	ON_COMMAND(ID_PETEYE_MED, OnPetEyeMed)
	ON_COMMAND(ID_PETEYE_LG, OnPetEyeLg)
	ON_COMMAND(ID_DUST_CIR_SM, OnDustCirSm)
	ON_COMMAND(ID_DUST_CIR_MED, OnDustCirMed)
	ON_COMMAND(ID_DUST_CIR_LG, OnDustCirLg)
	ON_COMMAND(ID_DUST_OVAL_SM, OnDustOvalSm)
	ON_COMMAND(ID_DUST_OVAL_MED, OnDustOvalMed)
	ON_COMMAND(ID_DUST_OVAL_LG, OnDustOvalLg)
	ON_COMMAND(ID_SHINE_SM, OnShineSm)
	ON_COMMAND(ID_SHINE_MED, OnShineMed)
	ON_COMMAND(ID_SHINE_LG, OnShineLg)
	ON_BN_CLICKED(IDC_REDEYE_BTN, OnRedEye)
	ON_BN_CLICKED(IDC_PETEYE_BTN, OnPetEye)
	ON_BN_CLICKED(IDC_DUST_BTN, OnDust)
	ON_BN_CLICKED(IDC_SHINE_BTN, OnShine)
	ON_BN_CLICKED(IDC_ZOOMIN_BTN, OnZoomIn)
	ON_BN_CLICKED(IDC_ZOOMOUT_BTN, OnZoomOut)
	ON_BN_CLICKED(IDC_UNDO_BTN, OnUndo)
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
//  Function Name:	RTabbedEditFixFlaw::RTabbedEditFixFlaw()
//
//  Description:	Constructor
//
//  Returns:		Nothing
//
// ****************************************************************************
//
RTabbedEditFixFlaw::RTabbedEditFixFlaw(CWnd* pParent /*=NULL*/)
	: CDialog(RTabbedEditFixFlaw::IDD, pParent),
	  m_pControlDocument(NULL),
	  m_pControlView(NULL),
	  m_pFlyOut(NULL),
	  m_nCurrentTool(TOOL_NONE),
	  m_nTempTool(TOOL_NONE),
	  m_nToolSel(0)
{
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::~RTabbedEditFixFlaw()
//
//  Description:	Destructor
//
//  Returns:		Nothing
//
// ****************************************************************************
//
RTabbedEditFixFlaw::~RTabbedEditFixFlaw()
{
	delete m_pControlDocument;

	// note: these get cleaned up by the document destructor:
	//		m_pComponent
	//		m_pControlView

	if(m_pFlyOut)
		delete m_pFlyOut;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::DoDataExchange()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ZOOMIN_BTN, m_cButtonZoomIn);
	DDX_Control(pDX, IDC_ZOOMOUT_BTN, m_cButtonZoomOut);
	DDX_Control(pDX, IDC_UNDO_BTN, m_cButtonUndo);
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::PreTranslateMessage()
//
//  Description:		Handles various keystrokes for this dialog
//
//  Returns:			TRUE if the message is processed here
//
// ****************************************************************************
//
BOOL RTabbedEditFixFlaw::PreTranslateMessage( MSG* pMsg )
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
			if ( pFocusWnd == GetDlgItem( IDC_REDEYE_BTN ) )
			{
				OnRedEye();
				bRet = TRUE;
			}
			else if ( pFocusWnd == GetDlgItem( IDC_PETEYE_BTN ) )
			{
				OnPetEye();
				bRet = TRUE;
			}
			else if ( pFocusWnd == GetDlgItem( IDC_DUST_BTN ) )
			{
				OnDust();
				bRet = TRUE;
			}
			else if ( pFocusWnd == GetDlgItem( IDC_SHINE_BTN ) )
			{
				OnShine();
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
//  Function Name:	RTabbedEditFixFlaw::OnInitDialog() 
//
//  Description:	Sets up the dialog
//
//  Returns:		BOOL
//
// ****************************************************************************
//
BOOL RTabbedEditFixFlaw::OnInitDialog() 
{
	// Call the base method
	BOOL bRet = CDialog::OnInitDialog();

	if ( bRet )
	{
		RWaitCursor waitCursor;
		InitializeDocument();

		// Set bitmap buttons
		SetButtonBitmap( this, IDC_REDEYE_BTN, IDB_PHOTOWORKSHOP_REDEYE );
		SetButtonBitmap( this, IDC_PETEYE_BTN, IDB_PHOTOWORKSHOP_PETEYE );
		SetButtonBitmap( this, IDC_DUST_BTN, IDB_PHOTOWORKSHOP_DUST_SCRATCH );
		SetButtonBitmap( this, IDC_SHINE_BTN, IDB_PHOTOWORKSHOP_SHINE );

//	AutoLoad fails under Win95 / Win98 with VC6 build!
//		m_cButtonZoomIn.AutoLoad( IDC_ZOOMIN_BTN, this );
//		m_cButtonZoomOut.AutoLoad( IDC_ZOOMOUT_BTN, this );
//		m_cButtonUndo.AutoLoad( IDC_UNDO_BTN, this );

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
// Function Name:  RTabbedEditFixFlaw::InitializeDocument
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
void RTabbedEditFixFlaw::InitializeDocument()
{
	try
	{
		// Create our document...
		//
		m_pControlDocument	= new REditFixFlawDocument();

		// Create a view for the document...
		//
		m_pControlView = (REditFixFlawView*) m_pControlDocument->CreateView( this, IDC_PREVIEW_AREA ) ;
		m_pControlDocument->AddRView( m_pControlView );

		RRealRect rectArea( m_pControlView->GetViewableArea() );
		RComponentAttributes attr ;
		attr.SetSelectable( FALSE ) ;

		// get a pointer to the IED component
		RTabbedEditImage* pParentDlg = (RTabbedEditImage *) GetParent();
		m_pComponentDocument = pParentDlg->GetComponent();
	
		if ( m_pComponentDocument )
		{
			// add the component to our preview window control
			m_pControlDocument->AddComponent( m_pComponentDocument );

			// get the transform and invert it to cancel out rotations, flips, etc.
			RComponentView* pComponentView = (RComponentView *)m_pComponentDocument->GetActiveView();
			m_rBoundingRect = pComponentView->GetBoundingRect();
			R2dTransform xform = m_rBoundingRect.GetTransform();
			xform.Invert();
			m_rBoundingRect *= xform;
			pComponentView->SetBoundingRect( m_rBoundingRect );

			// fit and center the component in the control
			pComponentView->FitInsideParent();
			RRealRect rViewableArea = m_pControlView->GetViewableArea();
			pComponentView->Offset( RIntSize( rViewableArea.m_Left, rViewableArea.m_Top ) );
			
			// remember the bounding rect for OnShowWindow()
			m_rBoundingRect = pComponentView->GetBoundingRect();

			// we're done with this for now
			m_pControlDocument->RemoveComponent( m_pComponentDocument );

			// also remember the crop area and image size for OnShowWindow()
			REditImageInterface* pEditImageInterface = static_cast<REditImageInterface*>(pComponentView->GetInterface( kEditImageInterfaceId ));
			const RImageEffects& rEffects = pEditImageInterface->GetImageEffects();
			m_rCropArea = rEffects.m_rCropArea;
			delete pEditImageInterface;

			m_rImageSize = pParentDlg->GetImage()->GetSizeInLogicalUnits();

			// set the attributes
			m_pComponentDocument->SetComponentAttributes( attr );
			m_pComponentDocument = NULL;
		}
	}
	catch (...)
	{
		throw kUnknownError ;
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnShowWindow()
//
//  Description:	sets up display of the dialog window
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnShowWindow(  BOOL bShow, UINT nStatus )
{
	if ( bShow && ! nStatus )		// comin' in
	{
		// get the mother component and add it to our preview control
		RTabbedEditImage* pParentDlg = (RTabbedEditImage *)GetParent();
		m_pComponentDocument = pParentDlg->GetComponent();
		m_pControlDocument->AddComponent( m_pComponentDocument );

		// get the crop area of the component
		RComponentView*  pComponentView = (RComponentView*) m_pComponentDocument->GetActiveView();
		REditImageInterface* pEditImageInterface = static_cast<REditImageInterface*>(pComponentView->GetInterface( kEditImageInterfaceId ));
		const RImageEffects& rEffects = pEditImageInterface->GetImageEffects();
		delete pEditImageInterface;
		
		//	if the crop area or the image dimensions have changed, it must be
		//	because the user recropped or rotated the image in the Crop tab,
		//	so lets reset zoom to 1 and center the cropped image
		if (	m_rCropArea  != rEffects.m_rCropArea ||
				m_rImageSize != pParentDlg->GetImage()->GetSizeInLogicalUnits() )
		{
			m_pControlView->SetZoomLevel( 1.0 );
			m_pControlView->UpdateScrollBars( kResize );
			m_cButtonZoomIn.EnableWindow( TRUE );
			m_cButtonZoomOut.EnableWindow( TRUE );

			pComponentView->FitInsideParent() ;
			RRealRect rViewableArea = m_pControlView->GetViewableArea();
			pComponentView->Offset( RIntSize( rViewableArea.m_Left, rViewableArea.m_Top ) );
		}
		// otherwise, no new crop, so let's restore the image to it's last appearance
		else
		{
			pComponentView->SetBoundingRect( m_rBoundingRect );
		}

		// reset the undo mechanism
		m_pControlDocument->FreeAllActions();
		UpdateUndoButton();
	}
	else			// goin' out
	{
		// remember the bounding rect, crop area and image size for the next visit to this tab
		RComponentView* pComponentView = (RComponentView *)m_pComponentDocument->GetActiveView();
		m_rBoundingRect = pComponentView->GetBoundingRect();

		REditImageInterface* pEditImageInterface = static_cast<REditImageInterface*>(pComponentView->GetInterface( kEditImageInterfaceId ));
		const RImageEffects& rEffects = pEditImageInterface->GetImageEffects();
		m_rCropArea = rEffects.m_rCropArea;
		delete pEditImageInterface;

		m_rImageSize = ((RTabbedEditImage *)GetParent())->GetImage()->GetSizeInLogicalUnits();

		// do this so tabs can share the mother component
		if (m_pControlDocument && m_pComponentDocument)
		{
			m_pControlDocument->RemoveComponent( m_pComponentDocument );
		}
		m_pComponentDocument = NULL;
	}

	// handle the Revert and OK buttons in the parent, too
	if ( m_pControlDocument->UndoCount() > 0 )
	{
		// if we are laeving this tab with a change, lock the buttons enabled
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
//  Function Name:	RTabbedEditFixFlaw::UpdatePreview()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::UpdatePreview()
{
	// update the preview control window
	((RView *)m_pControlView)->Invalidate();
	m_pControlView->GetCWnd().UpdateWindow() ;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::ApplyTool()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::ApplyTool( const RRealPoint& rMousePoint, RIntSize rToolSize )
{
	if ( m_nCurrentTool == TOOL_NONE )
	{
		return;
	}

	// make sure we should be doing anything
	if ( ! ToolInImageArea( rMousePoint, rToolSize ) )
	{
		return;
	}

	R2dTransform xform;

	RComponentView* pComponentView = (RComponentView*)m_pComponentDocument->GetActiveView();
	pComponentView->ApplyTransform( xform, FALSE, FALSE );

	RImageInterface* pInterface = static_cast<RImageInterface *>(pComponentView->GetInterface( kImageInterfaceId ) );
	RImage* pImage = pInterface->GetImage();
	delete pInterface;

	RRealSize szImageSize = pImage->GetSizeInLogicalUnits();

	// The component scaling doesn't always happen
	// through the bounding rect transform.  So, we
	// need to add any additional scaling base on
	// the bounding rects untransformed size.
	YComponentBoundingRect rBoundingRect( pComponentView->GetBoundingRect() );
	RRealRect rectImage( rBoundingRect.GetUntransformedSize() );
	RRealPoint ptCenter = rectImage.GetCenterPoint();

	xform.PreScale( rectImage.Width()  / szImageSize.m_dx, rectImage.Height() / szImageSize.m_dy );
	xform.Invert();

	// get our tool size in logical units
	RRealSize rRealToolSize( rToolSize.m_dx, rToolSize.m_dy );
	ScreenUnitsToLogicalUnits( rRealToolSize );

	// account for zooming in the preview window
	YZoomLevel yZoom = 1.0 / m_pControlView->GetZoomLevel();
	rRealToolSize.Scale( RRealSize(yZoom, yZoom) );

	// create a rectangular area for processing, centered on the mouse point
	YRealCoordinate dx = rRealToolSize.m_dx / 2;
	YRealCoordinate dy = rRealToolSize.m_dy / 2;
	RRealRect rToolRect( rMousePoint.m_x - dx, rMousePoint.m_y - dy, rMousePoint.m_x + dx, rMousePoint.m_y + dy );

	// Okay, the points are based on the center of the
	// image.  We need to transform the points to position
	// them correctly in the image.
	rectImage = szImageSize;
	RRealPoint pt = rectImage.GetCenterPoint();

	// We need to ignore the original image offset
	// as the positioning translation used below is
	// base on the actual image center, not the 
	// location of the image within its parent.
	YRealDimension xOffset, yOffset;
	xform.GetTranslation( xOffset, yOffset );

	xform.PostTranslate( pt.m_x - xOffset, pt.m_y - yOffset );
	xform.PostScale( 200.0 / kSystemDPI, 200.0 / kSystemDPI );

	// transform our processing area to match the image
	RIntRect rIntToolRect( rToolRect * xform );

	// make sure the tool size is at least 1 pixel
	if ( rIntToolRect.Width() < 1 )
	{
		if ( rIntToolRect.m_TopLeft.m_x < 1 )
		{
			rIntToolRect.m_BottomRight.m_x++;
		}
		else
		{
			rIntToolRect.m_TopLeft.m_x--;
		}
	}
	if ( rIntToolRect.Height() < 1 )
	{
		if ( rIntToolRect.m_TopLeft.m_y < 1 )
		{
			rIntToolRect.m_BottomRight.m_y++;
		}
		else
		{
			rIntToolRect.m_TopLeft.m_y--;
		}
	}

	// get a pointer to the intermediate bitmap and apply the effect
	RTabbedEditImage *pParentDlg = (RTabbedEditImage *)GetParent();
	RBitmapImage* pBitmap = pParentDlg->GetImage();
	if ( pBitmap )
	{
		// get the cropped area as it applies to the IED intermediate bitmap
		RIntRect rCropRect;
		pParentDlg->GetCropRect( &rCropRect );

		// offset our tool rectangle into the bitmap
		rIntToolRect.Offset( RIntSize ( rCropRect.m_Left, rCropRect.m_Top ) );

		//	Create the undo action for changing the image.
		//	Note, this action is undo only.  In other words it saves the image
		//	and then will restore it on undo.  Any changes to be done to the
		//	image need to be done manually.
		RUndoableAction* pAction = new RModifyImageAction( m_pComponentDocument, &rIntToolRect, pBitmap );
		m_pControlDocument->SendAction( pAction );

		BOOL bSuccess = FALSE;
		switch ( m_nCurrentTool )
		{
			case TOOL_RED_EYE:
				bSuccess = RImageLibrary().RemoveEncircledChromaRed( *pBitmap, rIntToolRect );
				break;

			case TOOL_PET_EYE:
				bSuccess = RImageLibrary().RemoveEncircledChromaAny( *pBitmap, rIntToolRect );
				break;

			case TOOL_DUST:
				bSuccess = RImageLibrary().RemoveScratch( *pBitmap, rIntToolRect );
				break;

			case TOOL_SHINE:
				bSuccess = RImageLibrary().RemoveShine( *pBitmap, rIntToolRect );
				break;

			case TOOL_NONE:
			default:
				break;
		}

		if ( bSuccess )
		{
			// update the component image
			pParentDlg->ApplyImageEffects();
	
			// reset the bounding rect for our view
			pComponentView->SetBoundingRect( rBoundingRect );

			UpdatePreview();
			UpdateUndoButton();
		}
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnZoomIn()
//
//  Description:	passes zoom in command to the preview control
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnZoomIn()
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
//  Function Name:	RTabbedEditFixFlaw::OnZoomOut()
//
//  Description:	passes zoom out command to the preview control
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnZoomOut()
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
//  Function Name:	RTabbedEditFixFlaw::ToolInImageArea()
//
//  Description:		determines if the entire tool lies within the bounding
//							rect of the image, and the tool's hotspot is within the
//							clipping path (if any)
//
//  Returns:			TRUE if tool falls within the image, false if not
//
// ****************************************************************************
//
BOOLEAN RTabbedEditFixFlaw::ToolInImageArea( const RRealPoint& rMousePoint, RIntSize rToolSize )
{
	RComponentView* pComponentView = (RComponentView*)m_pComponentDocument->GetActiveView();
	YComponentBoundingRect rBoundingRect( pComponentView->GetBoundingRect() );

	//	HitTest() will tell us if the cursor hotspot is inside the clipping path,
	//	or within the image bounds if there is no clipping path
	BOOLEAN bInImage = pComponentView->HitTest( rMousePoint );
	if ( bInImage )
	{
		// now see if the entire tool diameter is within image bounds, since the
		//	Fix Flaw tools don't handle image-edge conditions well;
		// get our tool size in logical units
		RRealSize rRealToolSize( rToolSize );
		ScreenUnitsToLogicalUnits( rRealToolSize );

		// account for zooming in the preview window
		YZoomLevel yZoom = 1.0 / m_pControlView->GetZoomLevel();
		rRealToolSize.Scale( RRealSize(yZoom, yZoom) );

		// see any part of our tool lies outside the bounding rect
		YRealCoordinate dx = rRealToolSize.m_dx / 2;
		YRealCoordinate dy = rRealToolSize.m_dy / 2;
		if (	rMousePoint.m_x - dx < rBoundingRect.m_TopLeft.m_x - 1	||
				rMousePoint.m_y - dy < rBoundingRect.m_TopLeft.m_y			||
				rMousePoint.m_x + dx > rBoundingRect.m_BottomRight.m_x	||
				rMousePoint.m_y + dy > rBoundingRect.m_BottomRight.m_y	) 
		{
			bInImage = FALSE;
		}
	}

	return bInImage;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnRedEye()
//
//  Description:	Brings up flyout
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnRedEye()
{
	CWnd* pButtonWnd =  GetDlgItem(IDC_REDEYE_BTN);
	CRect rcButton;
	pButtonWnd->GetWindowRect( &rcButton );
	CPoint ptLoc(rcButton.right, rcButton.top);

	if(m_pFlyOut)
		delete m_pFlyOut;
	m_pFlyOut = new RFlyout(1, TOOLBAR_REDEYE, this);

	m_pFlyOut->Create(IDD_FLYOUT, this );	
	m_pFlyOut->SetWindowPos(&wndTop, ptLoc.x, ptLoc.y , 0,0,SWP_NOSIZE | SWP_NOZORDER );
	if( m_nCurrentTool == TOOL_RED_EYE )
	{
		m_pFlyOut->SetButtonState(m_nToolSel, TBSTATE_CHECKED | TBSTATE_ENABLED );
	}
	m_nTempTool = TOOL_RED_EYE;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnPetEye()
//
//  Description:	Brings up flyout
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnPetEye()
{
	CWnd* pButtonWnd =  GetDlgItem(IDC_PETEYE_BTN);
	CRect rcButton;
	pButtonWnd->GetWindowRect( &rcButton );
	CPoint ptLoc(rcButton.right, rcButton.top);

	if(m_pFlyOut)
		delete m_pFlyOut;
	m_pFlyOut = new RFlyout(1, TOOLBAR_PETEYE, this);

	m_pFlyOut->Create(IDD_FLYOUT, this );	
	m_pFlyOut->SetWindowPos(&wndTop, ptLoc.x, ptLoc.y , 0,0,SWP_NOSIZE | SWP_NOZORDER );
	if( m_nCurrentTool == TOOL_PET_EYE )
	{
		m_pFlyOut->SetButtonState(m_nToolSel, TBSTATE_CHECKED | TBSTATE_ENABLED );
	}
	m_nTempTool = TOOL_PET_EYE;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnDust()
//
//  Description:	Brings up flyout
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnDust()
{
	CWnd* pButtonWnd =  GetDlgItem(IDC_DUST_BTN);
	CRect rcButton;
	pButtonWnd->GetWindowRect( &rcButton );
	CPoint ptLoc(rcButton.right, rcButton.top);

	if(m_pFlyOut)
		delete m_pFlyOut;
//	m_pFlyOut = new RFlyout(2, TOOLBAR_DUST, this);
	m_pFlyOut = new RFlyout(1, TOOLBAR_DUST, this);

	m_pFlyOut->Create(IDD_FLYOUT, this );	
	m_pFlyOut->SetWindowPos(&wndTop, ptLoc.x, ptLoc.y , 0,0,SWP_NOSIZE | SWP_NOZORDER );
	if( m_nCurrentTool == TOOL_DUST )
	{
		m_pFlyOut->SetButtonState(m_nToolSel, TBSTATE_CHECKED | TBSTATE_ENABLED );
	}
	m_nTempTool = TOOL_DUST;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnShine()
//
//  Description:	Brings up flyout
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnShine()
{
	CWnd* pButtonWnd =  GetDlgItem(IDC_SHINE_BTN);
	CRect rcButton;
	pButtonWnd->GetWindowRect( &rcButton );
	CPoint ptLoc(rcButton.right, rcButton.top);

	if(m_pFlyOut)
		delete m_pFlyOut;
	m_pFlyOut = new RFlyout(1, TOOLBAR_SHINE, this);

	m_pFlyOut->Create(IDD_FLYOUT, this );	
	m_pFlyOut->SetWindowPos(&wndTop, ptLoc.x, ptLoc.y , 0,0,SWP_NOSIZE | SWP_NOZORDER );
	if( m_nCurrentTool == TOOL_SHINE )
	{
		m_pFlyOut->SetButtonState(m_nToolSel, TBSTATE_CHECKED | TBSTATE_ENABLED );
	}
	m_nTempTool = TOOL_SHINE;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnUndo()
//
//  Description:	Undoes the last action
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnUndo()
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
				CWnd *pWnd = GetDlgItem( IDC_REDEYE_BTN );
				pWnd->SetFocus();
			}
		}

		if ( m_pControlDocument->UndoCount() > 0 )
		{
			m_pControlDocument->Undo();
		}

		UpdateUndoButton();
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::UpdateUndoButton()
//
//  Description:		handles enabling / disabling of the Undo button
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::UpdateUndoButton()
{
//	m_cButtonUndo.EnableWindow( m_pControlDocument->UndoCount() > 0 );

	if ( m_pControlDocument->UndoCount() > 0 )
	{
		m_cButtonUndo.EnableWindow( TRUE );

		// handle the Revert and OK buttons in the parent, too
		((RTabbedEditImage *)GetParent())->EnableRevertAndOkButtons( TRUE );
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
//  Function Name:	RTabbedEditFixFlaw::OnApplyEditsToIEDBitmap()
//
//  Description:	using the current settings, apply the edits to the
//					intermediate bitmap owned by the base IED dialog
//
//  Returns:		nothing
//
// ****************************************************************************
//
LONG RTabbedEditFixFlaw::OnApplyEditsToIEDBitmap( WPARAM, LPARAM )
{
	RWaitCursor rWaitCursor;

	// since fix flaw operates directly on the component, we need to update the
	// IED's intermediate bitmap with a copy of the component's current image
	RImageInterface* pInterface = dynamic_cast<RImageInterface*>(static_cast<RComponentView*>(m_pComponentDocument->GetActiveView())->GetInterface(kImageInterfaceId));
	RBitmapImage* pBitmapCopy = dynamic_cast<RBitmapImage *>(pInterface->CopySourceImage() );

	((RTabbedEditImage *)GetParent())->SetImage( pBitmapCopy );
	
	delete pInterface;
	return 0;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnCleanupForReset()
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
LONG RTabbedEditFixFlaw::OnCleanupForReset( WPARAM, LPARAM )
{
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
//  Function Name:	RTabbedEditFixFlaw::OnResetUpdate()
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
LONG RTabbedEditFixFlaw::OnResetUpdate( WPARAM, LPARAM )
{
	// reinitialize the window
	OnShowWindow( TRUE, 0 );

	return 0;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnOKFromIED()
//
//  Description:		handles M_USER_ON_OK_FROM_IED message, sent by parent
//							RTabbedEditImage dialog when user presses the OK button
//
//  Returns:			1 (A-OK)
//
// ****************************************************************************
//
LONG RTabbedEditFixFlaw::OnOKFromIED( WPARAM, LPARAM )
{
	return 1L;			// nothing to do, but say ok, how's your day been?
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnDestroy()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnDestroy( )
{
	if (m_pControlDocument && m_pComponentDocument)
	{
		m_pControlDocument->RemoveComponent( m_pComponentDocument );
	}

	CDialog::OnDestroy();
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnRedEyeSm()
//
//  Description:	 
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnRedEyeSm()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_REDEYE_SM;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundSmall );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnRedEyeMed()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnRedEyeMed()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_REDEYE_MED;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundMedium );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnRedEyeLg()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnRedEyeLg()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_REDEYE_LG;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundLarge );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnPetEyeSm()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnPetEyeSm()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_PETEYE_SM;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundSmall );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnPetEyeMed()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnPetEyeMed()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_PETEYE_MED;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundMedium );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnPetEyeLg()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnPetEyeLg()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_PETEYE_LG;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundLarge );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnDustCirSm()
//
//  Description:	 
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnDustCirSm()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_DUST_CIR_SM;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundSmall );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnDustCirMed()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnDustCirMed()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_DUST_CIR_MED;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundMedium );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnDustCirLg()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnDustCirLg()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_DUST_CIR_LG;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundLarge );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnDustOvalSm()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnDustOvalSm()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_DUST_OVAL_SM;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundSmall );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnDustOvalMed()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnDustOvalMed()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_DUST_OVAL_MED;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundMedium );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnDustOvalLg()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnDustOvalLg()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_DUST_OVAL_LG;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundLarge );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnShineSm()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnShineSm()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_SHINE_SM;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundSmall );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnShineMed()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnShineMed()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_SHINE_MED;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundMedium );
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditFixFlaw::OnShineLg()
//
//  Description:	
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditFixFlaw::OnShineLg()
{
	m_nCurrentTool = m_nTempTool;
	m_nToolSel = ID_SHINE_LG;
	if(m_pFlyOut)
	{
		delete m_pFlyOut;
		m_pFlyOut = NULL;
	}

	m_pControlView->SetEditCursor( kCursorRoundLarge );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Class Name:		REditFixFlawView
//
//  Description:	View for fix flaw preview control
//
///////////////////////////////////////////////////////////////////////////////
//
// ****************************************************************************
//
//  Function Name:	REditFixFlawView::REditFixFlawView( )
//
//  Description:	Constructor
//
//  Returns:		Nothing
//
// ****************************************************************************
//
REditFixFlawView::REditFixFlawView( CDialog* pDialog, int nControlId, RControlDocument* pDocument )
	: RScrollControlView( pDialog, nControlId, pDocument )
{
	m_rCursors[ kCursorRoundSmall  ].m_hCursor = AfxGetApp()->LoadCursor( IDC_FIXFLAW_CIRCLE10 );
	m_rCursors[ kCursorRoundSmall  ].m_rSize.m_dx = 10;
	m_rCursors[ kCursorRoundSmall  ].m_rSize.m_dy = 10;
	m_rCursors[ kCursorRoundMedium ].m_hCursor = AfxGetApp()->LoadCursor( IDC_FIXFLAW_CIRCLE20 );
	m_rCursors[ kCursorRoundMedium ].m_rSize.m_dx = 20;
	m_rCursors[ kCursorRoundMedium ].m_rSize.m_dy = 20;
	m_rCursors[ kCursorRoundLarge  ].m_hCursor = AfxGetApp()->LoadCursor( IDC_FIXFLAW_CIRCLE30 );
	m_rCursors[ kCursorRoundLarge  ].m_rSize.m_dx = 30;
	m_rCursors[ kCursorRoundLarge  ].m_rSize.m_dy = 30;

//	m_nCurrentCursorType = (EFixFlawCursorType)-1;
	m_nCurrentCursorType = kCursorNone;
}

// ****************************************************************************
//
//  Function Name:	REditFixFlawView::SetEditCursor()
//
//  Description:	handler for the SetCursor XEvent.
//
//  Returns:		nothing
//
// ****************************************************************************
//
void REditFixFlawView::SetEditCursor( EFixFlawCursorType rType )
{
	m_nCurrentCursorType = rType;
}

// ****************************************************************************
//
//  Function Name:	REditFixFlawView::OnXMouseMessage()
//
//  Description:		handler for the mouse messages to this view
//
//  Returns:			nothing
//
// ****************************************************************************
//
void REditFixFlawView::OnXMouseMessage( EMouseMessages eMessage, RRealPoint devicePoint, YModifierKey /*modifierKeys*/ )
{
	if ( eMessage == kMouseMove || eMessage == kLeftButtonUp )
	{
		// get position relative to the current view
		R2dTransform transform;
		GetViewTransform( transform, TRUE );
		transform.Invert();
		devicePoint *= transform;

		// handle mouse move to change cursers
		if ( eMessage == kMouseMove )
		{
			// if we're within the image area, show the tool cursor
			if (	m_nCurrentCursorType != kCursorNone &&
					m_nCurrentCursorType <	kCursorTypesMax &&
					((RTabbedEditFixFlaw *)GetParent())->ToolInImageArea( devicePoint, m_rCursors[ m_nCurrentCursorType ].m_rSize ) )
			{
				::SetCursor( m_rCursors[ m_nCurrentCursorType ].m_hCursor );
			}
			else
			{
				::SetCursor( ::LoadCursor( NULL, IDC_ARROW ) );
			}
		}
		else		// the other message we handle is LButtonUp to apply the tool
		{
			((RTabbedEditFixFlaw *)GetParent())->ApplyTool( devicePoint, m_rCursors[ m_nCurrentCursorType ].m_rSize );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//  Class Name:		REditFixFlawDocument
//
//  Description:	Document for fix flaw preview control
//
///////////////////////////////////////////////////////////////////////////////
//
// ****************************************************************************
//
//  Function Name:	REditFixFlawDocument::CreateView( )
//
//  Description:	Creates a new Edit Image view
//
//  Returns:		New view
//
//  Exceptions:		None
//
// ****************************************************************************
//
RControlView* REditFixFlawDocument::CreateView( CDialog* pDialog, int nControlId )
{
	return new REditFixFlawView( pDialog, nControlId, this );
}

// ****************************************************************************
//
//  Function Name:	REditFixFlawDocument::FreeAllActions()
//
//  Description:	purges undo buffer by going directly through the Undo Manager
//
//  Returns:		nothing
//
// ****************************************************************************
//
void REditFixFlawDocument::FreeAllActions()
{
	TpsAssertValid( m_pUndoManager );
	
	m_pUndoManager->FreeAllActions();
}

// ****************************************************************************
//
//  Function Name:	REditFixFlawDocument::Undo( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditFixFlawDocument::Undo( )
{
	TpsAssertValid( m_pUndoManager );

	m_pUndoManager->UndoAction( );
	m_pUndoManager->FreeUndoneActions( );
}