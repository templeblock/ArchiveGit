//****************************************************************************
//
// File Name:		RGenericGroupedObjectDialog.cpp
//
// Project:			Renaissance application framework
//
// Author:			Mike Heydlauf
//
// Description:	Source code for RGenericGroupedObjectDialog and
//						RCommonGroupedObjectDialog
//
// Portability:	Windows Specific
//
// Developed by:  Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/GenericGroupedObjectDialog.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME

#include "GenericGroupedObjectDialog.h"

#include "ControlContainer.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "ComponentTypes.h"
#include "ComponentAttributes.h"
#include "StandaloneApplication.h"
#include "StandaloneView.h"
#include "SelectionTracker.h"
#include "CompositeSelection.h"
#include "NewComponentAction.h"
#include "ReplaceComponentAction.h"
#include "SingleSelection.h"
#include "HeadlineInterface.h"
#include "ResourceManager.h"
#include "ComponentManager.h"

#include "FrameworkResourceIDs.h"

//#ifndef	WIN		// old Renaissance flag
#ifndef	_WINDOWS
	#error	This file can only be compilied on Windows
#endif

// ****************************************************************************
//
//  Function Name:	RGenericGroupedObjectDialog::RGenericGroupedObjectDialog
//
//  Description:		Ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RGenericGroupedObjectDialog::RGenericGroupedObjectDialog( IComponentInfo* pCompInfo,
																			 RComponentDocument* pEditedComponent,
																			 int nResourceId )
:	CDialog(nResourceId, NULL),
	m_pCompInfo( pCompInfo ),
	m_pEditedComponent( pEditedComponent ),
	m_pGroupComponent( NULL ),
	m_pControlDocument( NULL ),
	m_pControlView( NULL )
{
	//{{AFX_DATA_INIT(RGenericGroupedObjectDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

// ****************************************************************************
//
//  Function Name:	RGenericGroupedObjectDialog::~RGenericGroupedObjectDialog
//
//  Description:		Dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RGenericGroupedObjectDialog::~RGenericGroupedObjectDialog()
{
	delete m_pControlDocument;
	delete m_pGroupComponent;
}

// ****************************************************************************
//
//  Function Name:	RGenericGroupedObjectDialog::DoDataExchange
//
//  Description:		dialog data exchange
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RGenericGroupedObjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RGenericGroupedObjectDialog)
	//DDX_Control(pDX, CONTROL_STATIC_GENERICGO_PREVIEW, m_cPreviewCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RGenericGroupedObjectDialog, CDialog)
	//{{AFX_MSG_MAP(RGenericGroupedObjectDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RGenericGroupedObjectDialog message handlers

// ****************************************************************************
//
//  Function Name:	RGenericGroupedObjectDialog::OnInitDialog
//
//  Description:		dialog initialization.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RGenericGroupedObjectDialog::OnInitDialog() 
{
	// Call the base method
	CDialog::OnInitDialog();

	// Create the control document
	CreateControlDocument( );

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RGenericGroupedObjectDialog::CreateControlDocument
//
//  Description:		Creates the control document for this dialog
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGenericGroupedObjectDialog::CreateControlDocument( )
	{
	// Create a control document
	m_pControlDocument = new RGroupDialogControlDocument( m_pCompInfo );

	// Create a control view
	m_pControlView = m_pControlDocument->CreateView( this, CONTROL_STATIC_GENERICGO_PREVIEW );

	// Add the control view to the control document
	m_pControlDocument->AddRView( m_pControlView );
	}

// ****************************************************************************
//
//  Function Name:	RGenericGroupedObjectDialog::AddComponent
//
//  Description:		Creates a new component and adds it to the control document
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
RComponentView* RGenericGroupedObjectDialog::AddComponent( const YComponentType& componentType,
																const YComponentBoundingRect& boundingRect,
																const RComponentAttributes& componentAttributes )
	{
	// Create the compnent
	RComponentDocument* pComponentDocument = ::GetComponentManager( ).CreateNewComponent( componentType,
																													  m_pControlDocument,
																													  m_pControlView,
																													  componentAttributes,
																													  boundingRect,
																													  FALSE );

	// Add the component to the control document
	m_pControlDocument->AddComponent( pComponentDocument );

	return pComponentDocument->GetView(m_pControlView);
	}

// ****************************************************************************
//
//  Function Name:	RGenericGroupedObjectDialog::RemoveComponent
//
//  Description:		Removes a component from the control document and deletes it
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RGenericGroupedObjectDialog::RemoveComponent( RComponentDocument* pComponentDocument )
	{
	m_pControlDocument->RemoveComponent( pComponentDocument );
	delete pComponentDocument;
	}

// ****************************************************************************
//
//  Function Name:	RGenericGroupedObjectDialog::ReplaceComponent
//
//  Description:		Adds a new component to the control document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGenericGroupedObjectDialog::NewComponent( RComponentDocument* pNewDocument, YComponentZPosition yZPosition )
	{
	RNewComponentAction rNewComponentAction( pNewDocument,
															m_pControlDocument,
															m_pControlView->GetSelection( ),
															STRING_UNDO_NEW_COMPONENT);
	rNewComponentAction.Do();
	m_pControlView->SetComponentZPosition( static_cast<RComponentView*>(pNewDocument->GetActiveView()), yZPosition );
	}

// ****************************************************************************
//
//  Function Name:	RGenericGroupedObjectDialog::ReplaceComponent
//
//  Description:		Replaces the old component with the new one
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGenericGroupedObjectDialog::ReplaceComponent( RComponentDocument* pOldDocument, 
																		RComponentDocument* pNewDocument )
	{
	RReplaceComponentAction rReplaceComponentAction( m_pControlView->GetSelection(),
																		pOldDocument,
																		pNewDocument,
																		IDS_REPLACE_PREVIEW_COMPONENT,
																		FALSE );
																		
	rReplaceComponentAction.Do();
	}

// ****************************************************************************
//
//  Function Name:	RGenericGroupedObjectDialog::OnOK
//
//  Description:		Called when the user clicks OK
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGenericGroupedObjectDialog::OnOK( )
	{
	// Calculate the bounding rect for the new component.	Just make up the numbers, as 
	// the caller will size and position the component itself.
	RRealRect groupBoundingRect( RRealPoint( 0, 0 ), RRealSize( ::InchesToLogicalUnits( 2 ), ::InchesToLogicalUnits( 2 ) ) );

	// Set up component attributes. Grouped objects are not shadowable, glowable or vignetteable
	RComponentAttributes attributes( RComponentAttributes::GetDefaultComponentAttributes( GetComponentType( ) ) );

	// Create a new group component
	m_pGroupComponent = ::GetComponentManager( ).CreateNewComponent( GetComponentType( ),
																						  NULL,
																						  NULL,
																						  attributes,
																						  groupBoundingRect,
																						  FALSE );

	// Write the UI context data
	WriteUIContextData( m_pGroupComponent->GetUIContextData( ) );

	// Make a component collection for transfering
	RComponentCollection componentCollection;
	YComponentIterator iterator = m_pControlDocument->GetComponentCollectionStart( );
	for( ; iterator != m_pControlDocument->GetComponentCollectionEnd( ); ++iterator )
		componentCollection.InsertAtEnd( *iterator );

	// Sort the collection by z-order
	componentCollection.SortByZOrder( );

	// Transfer components from the control document to the group
	m_pGroupComponent->TransferComponents( m_pControlDocument, componentCollection, m_pControlView->GetSize( ), groupBoundingRect );

	// Call the base method
	CDialog::OnOK( );
	}

// ****************************************************************************
//
//  Function Name:	RGenericGroupedObjectDialog::WriteUIContextData
//
//  Description:		Writes this dialogs UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGenericGroupedObjectDialog::WriteUIContextData( RUIContextData& ) const
	{
	// REVIEW STA - This function should be pure virtual, but I dont want to add it to all
	// the dialogs just yet.
	}

// ****************************************************************************
//
//  Function Name:	RGenericGroupedObjectDialog::GetComponent
//
//  Description:		Called to retreive the component that was created in OnOK
//
//  Returns:			Pointer to the created component
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument* RGenericGroupedObjectDialog::GetComponent( )
	{
	TpsAssert( m_pGroupComponent, "You already got the component. Don't be greedy." );

	RComponentDocument* pTemp = m_pGroupComponent;
	m_pGroupComponent = NULL;
	return pTemp;
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogControlDocument::RGroupDialogControlDocument( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGroupDialogControlDocument::RGroupDialogControlDocument( IComponentInfo* pCompInfo )
	: m_pCompInfo( pCompInfo )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogControlDocument::CreateView( )
//
//  Description:		create a control view for this document, subclassing the
//                   given window...
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RControlView* RGroupDialogControlDocument::CreateView( CDialog* pDialog, int nControlId )
	{
	return new RGroupDialogControlView( pDialog, nControlId, this );
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogControlDocument::GetMinObjectSize( )
//
//  Description:		Accessor
//
//  Returns:			Min object size
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RGroupDialogControlDocument::GetMinObjectSize( const YComponentType&, BOOLEAN )
	{
	// Lets limit the user to 1/4 inch
	const RRealSize kMinGroupedObjectSize( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ) );

	return kMinGroupedObjectSize;
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogControlDocument::GetMaxObjectSize( )
//
//  Description:		Accessor
//
//  Returns:			Max object size
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RGroupDialogControlDocument::GetMaxObjectSize( const YComponentType& componentType )
	{
	return m_pCompInfo->GetMaxObjectSize( componentType );
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogControlDocument::GetDefaultObjectSize( )
//
//  Description:		Accessor
//
//  Returns:			Default object size
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RGroupDialogControlDocument::GetDefaultObjectSize( const YComponentType& componentType )
	{
	return m_pCompInfo->GetDefaultObjectSize( componentType );
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogControlDocument::GetFontSizeInfo( )
//
//  Description:		Accessor
//
//  Returns:			Font size info
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RFontSizeInfo* RGroupDialogControlDocument::GetFontSizeInfo( ) const
	{
	RStandaloneDocument* pDoc = m_pCompInfo->GetTopLevelDocument() ;
	return pDoc->GetFontSizeInfo( );
	}

// ****************************************************************************
//
//  Function Name:	RControlView::RControlView( CDialog* pdlg, int nctlid )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGroupDialogControlView::RGroupDialogControlView( CDialog* pDialog, int nControlId, RControlDocument* pDocument )
	: RControlView( pDialog, nControlId, pDocument ),
	  m_pDialog( pDialog )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogControlView::DoDragDrop( )
//
//  Description:		Begins a drag&drop operation using a tracker
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupDialogControlView::DoDragDrop( const RRealPoint& mousePoint, BOOLEAN, YDropEffect )
	{
	// Create a drag tracker
	m_pActiveTracker = new RGroupDialogDragSelectionTracker( GetSelection( ), GetUseableArea( ) );

	// Begin the tracker
	m_pActiveTracker->BeginTracking( mousePoint, 0 );
	}

//*****************************************************************************
//
// Function Name:  RGroupDialogControlView::OnXDragEnter
//
// Description:    Drag enter message
//
// Returns:        Drop effect
//
// Exceptions:	    None
//
//*****************************************************************************
//
YDropEffect RGroupDialogControlView::OnXDragEnter( RDataTransferSource&, YModifierKey, const RRealPoint& )
	{
	return kDropEffectNone;
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogControlView::GetResizeTracker( )
//
//  Description:		Gets a tracker to use for resizing
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelectionTracker* RGroupDialogControlView::GetResizeTracker( ) const
	{
	return new RGroupDialogResizeSelectionTracker( GetSelection( ), GetUseableArea( ) );
	}


// ****************************************************************************
//
//  Function Name:	RGroupDialogControlView::EditComponent( )
//
//  Description:		Override EditComponent so doubleclicks on headlines won't 
//							activate headline dialog
//
//  Returns:			VOID
//
//  Exceptions:		None
//
// ****************************************************************************
RComponentView*	RGroupDialogControlView::EditComponent( RComponentView* /*pComponent*/,
																			EActivationMethod /*activationMethod*/,
																			const RRealPoint& /*mousePoint*/,
																			YResourceId& /*editUndoRedoPairId*/,
																			uLONG /* ulEditInfo */ )
{
	return NULL;
}

// ****************************************************************************
//
//  Function Name:	RGroupDialogControlView::OnXLButtonDown( )
//
//  Description:		Default handler for the button down XEvent.
//
//							Check for any hit components.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupDialogControlView::OnXLButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys )
{
	(m_pDialog->GetDlgItem( IDOK ))->SetFocus( );
	RControlView::OnXLButtonDown( mousePoint, modifierKeys );
}

// ****************************************************************************
//
//  Function Name:	RView::OnXRButtonDown( )
//
//  Description:		Default handler for the right button down XEvent.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupDialogControlView::OnXRButtonDown( const RRealPoint& , YModifierKey )
{
	NULL;
}


// ****************************************************************************
//
//  Function Name:	RGroupDialogDragSelectionTracker::RGroupDialogDragSelectionTracker( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGroupDialogDragSelectionTracker::RGroupDialogDragSelectionTracker( RCompositeSelection* pSelection,
																						  const RRealRect& constraintRect )
	: RDragSelectionTracker( pSelection )
	{
	// Get the bounding rect of the selection
	RRealRect selectionBoundingRect = pSelection->GetBoundingRect( );//pSelection->GetBoundingRectIncludingHandles( );

	RRealRect	unionRect;
	unionRect.Union( selectionBoundingRect, constraintRect );
	// Calculate the amount the mouse is allowed to offset from its initial point
	m_TopLeftOffset = selectionBoundingRect.m_TopLeft - unionRect.m_TopLeft;
	m_BottomRightOffset = unionRect.m_BottomRight - selectionBoundingRect.m_BottomRight;
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogDragSelectionTracker::ApplyConstraint( )
//
//  Description:		Constrains the mouse point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupDialogDragSelectionTracker::ApplyConstraint( RRealPoint& mousePoint ) const
	{
	// Constraint the point. Remember that rectangles to not contain the right and bottom
	if( m_TrackingMouseDownPoint.m_x - mousePoint.m_x > m_TopLeftOffset.m_dx )
		mousePoint.m_x = m_TrackingMouseDownPoint.m_x - m_TopLeftOffset.m_dx;

	else if( mousePoint.m_x - m_TrackingMouseDownPoint.m_x > m_BottomRightOffset.m_dx - 1 )
		mousePoint.m_x = m_TrackingMouseDownPoint.m_x + m_BottomRightOffset.m_dx - 1;

	if( m_TrackingMouseDownPoint.m_y - mousePoint.m_y > m_TopLeftOffset.m_dy )
		mousePoint.m_y = m_TrackingMouseDownPoint.m_y - m_TopLeftOffset.m_dy;

	else if( mousePoint.m_y - m_TrackingMouseDownPoint.m_y > m_BottomRightOffset.m_dy - 1 )
		mousePoint.m_y = m_TrackingMouseDownPoint.m_y + m_BottomRightOffset.m_dy - 1;
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogResizeSelectionTracker::RGroupDialogResizeSelectionTracker( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGroupDialogResizeSelectionTracker::RGroupDialogResizeSelectionTracker( RCompositeSelection* pSelection,
																								const RRealRect& constraintRect,
																								BOOLEAN fConstrainBoundingRect )
	: RResizeSelectionTracker( pSelection ),
	  m_ConstraintRect( constraintRect ),
	  m_fConstrainBoundingRect( fConstrainBoundingRect )
	{
	RRealVectorRect boundingRect;
	pSelection->GetSelectionBoundingRect( boundingRect );

	RRealRect boundingRectWithHandles = pSelection->GetBoundingRect( );//pSelection->GetBoundingRectIncludingHandles( );

	RRealSize delta;
	delta.m_dx = boundingRect.m_TransformedBoundingRect.m_Right - boundingRectWithHandles.m_Right;
	delta.m_dy = boundingRect.m_TransformedBoundingRect.m_Bottom - boundingRectWithHandles.m_Bottom;

	m_ConstraintRect.Union( boundingRectWithHandles, m_ConstraintRect );
	m_ConstraintRect.Inflate( delta );
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogResizeSelectionTracker::BeginTracking( )
//
//  Description:		Called when tracking begins; ie. when the mouse button goes
//							down.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupDialogResizeSelectionTracker::BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	RResizeSelectionTracker::BeginTracking( mousePoint, modifierKeys );
	m_eHitLocation = m_pSelection->HitTest( mousePoint );
	//m_fMaintainAspectRatio = FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogResizeSelectionTracker::ApplyConstraint( )
//
//  Description:		Constrains the mouse point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupDialogResizeSelectionTracker::ApplyConstraint( RRealPoint& mousePoint ) const
	{
	const_cast<RRealRect&>( m_ConstraintRect ).PinPointInRect( mousePoint );
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogResizeSelectionTracker::ConstrainScaleFactor( )
//
//  Description:		Constrains the scale factor within a min and a max
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupDialogResizeSelectionTracker::ConstrainScaleFactor( RRealSize& scaleFactor ) const
	{
	RResizeSelectionTracker::ConstrainScaleFactor( scaleFactor );
	if (!m_fConstrainBoundingRect) return;

	// Get its selection bounding rect
	YSelectionBoundingRect selectionBoundingRect;
	m_pSelection->GetSelectionBoundingRect( selectionBoundingRect );

	RRealRect& rect = selectionBoundingRect.m_TransformedBoundingRect;
	RRealSize rScaleFactor( m_MaximumScaleFactor );

	switch (m_eHitLocation)
		{
		case RSelection::kTopResizeHandle:
			rScaleFactor.m_dy = (rect.m_Bottom - m_ConstraintRect.m_Top) / rect.Height();
			break;

		case RSelection::kLeftResizeHandle:
			rScaleFactor.m_dx = (rect.m_Right - m_ConstraintRect.m_Left ) / rect.Width();
			break;

		case RSelection::kBottomResizeHandle:
			rScaleFactor.m_dy = (m_ConstraintRect.m_Bottom - rect.m_Top) / rect.Height();
			break;

		case RSelection::kRightResizeHandle:
			rScaleFactor.m_dx = (m_ConstraintRect.m_Right  - rect.m_Left) / rect.Width();
			break;

		case RSelection::kTopLeftResizeHandle:
			rScaleFactor.m_dx = (rect.m_Right - m_ConstraintRect.m_Left ) / rect.Width();
			rScaleFactor.m_dy = (rect.m_Bottom - m_ConstraintRect.m_Top)  / rect.Height();
			break;

		case RSelection::kTopRightResizeHandle:
			rScaleFactor.m_dx = (m_ConstraintRect.m_Right  - rect.m_Left) / rect.Width();
			rScaleFactor.m_dy = (rect.m_Bottom - m_ConstraintRect.m_Top)  / rect.Height();
			break;

		case RSelection::kBottomLeftResizeHandle:
			rScaleFactor.m_dx = (rect.m_Right - m_ConstraintRect.m_Left ) / rect.Width();
			rScaleFactor.m_dy = (m_ConstraintRect.m_Bottom - rect.m_Top)  / rect.Height();
			break;

		case RSelection::kBottomRightResizeHandle:
			rScaleFactor.m_dx = (m_ConstraintRect.m_Right  - rect.m_Left) / rect.Width();
			rScaleFactor.m_dy = (m_ConstraintRect.m_Bottom - rect.m_Top)  / rect.Height();
			break;
		}


	if (scaleFactor.m_dx > rScaleFactor.m_dx || scaleFactor.m_dy > rScaleFactor.m_dy)
		{
		scaleFactor.m_dx = Min( rScaleFactor.m_dx, rScaleFactor.m_dy );
		scaleFactor.m_dy = scaleFactor.m_dx;
		}

	}

#if 1
// ****************************************************************************
//
//  Function Name:	RGroupDialogResizeSelectionTracker::RGroupDialogResizeSelectionTracker( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGroupDialogRotateSelectionTracker::RGroupDialogRotateSelectionTracker( RCompositeSelection* pSelection,
																								const RRealRect& constraintRect )
	: RRotateSelectionTracker( pSelection ),
	  m_ConstraintRect( constraintRect ),
	  m_fIsConstrained( FALSE )
	{
	RRealVectorRect boundingRect;
	pSelection->GetSelectionBoundingRect( boundingRect );

	RRealRect boundingRectWithHandles = pSelection->GetBoundingRect( );//pSelection->GetBoundingRectIncludingHandles( );

	RRealSize delta;
	delta.m_dx = boundingRect.m_TransformedBoundingRect.m_Right - boundingRectWithHandles.m_Right;
	delta.m_dy = boundingRect.m_TransformedBoundingRect.m_Bottom - boundingRectWithHandles.m_Bottom;

	m_ConstraintRect.Union( boundingRectWithHandles, m_ConstraintRect );
	m_ConstraintRect.Inflate( delta );
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogRotateSelectionTracker::BeginTracking( )
//
//  Description:		Called when tracking begins; ie. when the mouse button goes
//							down.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupDialogRotateSelectionTracker::BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	RRotateSelectionTracker::BeginTracking( mousePoint, modifierKeys );

	// Determine angle range available
	RSingleSelection* pHitObject = m_pSelection->GetHitSingleSelection( mousePoint );
	TpsAssert( pHitObject, "No hit object!" );

	// Get its selection bounding rect
	YSelectionBoundingRect boundingRect;
	pHitObject->GetSelectionBoundingRect( boundingRect );

	// Get the bounding rect of the actual object
//	YSelectionBoundingRect objectBoundingRect;
//	pHitObject->GetObjectBoundingRect( objectBoundingRect );

	m_flStartAngle = boundingRect.GetRotationAngle();
//	boundingRect.RotateAboutCenter( -m_flStartAngle );

	// First test for no constraints
	RRealVectorRect rTemp( boundingRect );
	rTemp.RotateAboutCenter( (kPI / 4) - m_flStartAngle );
//	rTemp.Offset( objectBoundingRect.GetCenterPoint() - rTemp.GetCenterPoint() );

	m_fIsConstrained = !m_ConstraintRect.RectInRect( 
		rTemp.m_TransformedBoundingRect );

	if (m_fIsConstrained)
		{
		RRealSize szOffset( -m_ConstraintRect.m_Left, -m_ConstraintRect.m_Top );
		RRealRect rcRect( m_ConstraintRect );
		rcRect.Offset( szOffset );
		boundingRect.Offset( szOffset ); 

		RRealPoint ptCenter( boundingRect.GetCenterPoint() );
		YRealDimension r = ptCenter.Distance( boundingRect.m_TopLeft );

		m_flAngle = kPI;

		if (rTemp.m_TransformedBoundingRect.m_Top < m_ConstraintRect.m_Top)
			m_flAngle = min( m_flAngle, Abs( acos( ptCenter.m_y / r ) - kPI / 4 ) );
		if (rTemp.m_TransformedBoundingRect.m_Left < m_ConstraintRect.m_Left)
			m_flAngle = min( m_flAngle, Abs( acos( ptCenter.m_x / r ) - kPI / 4 ) );
		if (rTemp.m_TransformedBoundingRect.m_Bottom > m_ConstraintRect.m_Bottom)
			m_flAngle = min( m_flAngle, Abs( acos( (rcRect.m_Bottom - ptCenter.m_y) / r ) - kPI / 4 ) );
		if (rTemp.m_TransformedBoundingRect.m_Right > m_ConstraintRect.m_Right)
			m_flAngle = min( m_flAngle, Abs( acos( (rcRect.m_Right - ptCenter.m_x) / r ) - kPI / 4 ) );
		}
	}

// ****************************************************************************
//
//  Function Name:	RGroupDialogRotateSelectionTracker::ApplyConstraint( )
//
//  Description:		Applys the rotation constraint if the correct modifier
//							key was down when rotation started. We constrain to 45 deg.
//							angles
//
//  Returns:			The new angle
//
//  Exceptions:		None
//
// ****************************************************************************
//
YAngle RGroupDialogRotateSelectionTracker::ConstrainAngle( YAngle angle ) const
	{
	YAngle result = RRotateSelectionTracker::ConstrainAngle( angle );

	if (m_fIsConstrained)
		{
		result += m_flStartAngle;

		YAngle quadrant = int( result / (kPI / 2) ) * (kPI / 2);
//		YAngle angle = result - quadrant;
		result -= quadrant;

		if (result < -m_flAngle && result > (-kPI / 2) + m_flAngle)
			result = (result > -kPI / 4 ? -m_flAngle : -kPI / 2 + m_flAngle);
		else if (result > m_flAngle && result < (kPI / 2) - m_flAngle)
			result = (result < kPI / 4 ? m_flAngle : kPI / 2 - m_flAngle);

		result += quadrant;

//		if (result < -m_flAngle && result > -kPI + m_flAngle)
//			result = (result > -kPI / 2 ? -m_flAngle : -kPI + m_flAngle);
//		else if (AreFloatsGTE( result, m_flAngle ) && AreFloatsLTE( result, kPI - m_flAngle ))
//			result = (result < kPI / 2 ? m_flAngle : kPI - m_flAngle);

		result -= m_flStartAngle;
		}

	return result;
	}
#endif