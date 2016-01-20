// ****************************************************************************
//
//  File Name:			CompositeSelection.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RCompositeSelection class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/CompositeSelection.cpp                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "CompositeSelection.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "SingleSelection.h"
#include "ViewDrawingSurface.h"
#include "View.h"
#include "ComponentDocument.h"
#include "SelectionTracker.h"
#include "ApplicationGlobals.h"
#include "FrameworkResourceIDs.h"
#include "ChunkyStorage.h"
#include "BufferStream.h"
#include "DataTransfer.h"
#include "Cursor.h"
#include "utilities.h"

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::RCompositeSelection( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCompositeSelection::RCompositeSelection( RView* pView )
	: RSelection( pView ),
	  m_fSelectionVisible( TRUE ),
	  m_fSelectionHidden( FALSE )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::RCompositeSelection( )
//
//  Description:		Copy constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCompositeSelection::RCompositeSelection( const RCompositeSelection& rhs ) : RSelection( rhs )
	{
	*this = rhs;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::~RCompositeSelection( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCompositeSelection::~RCompositeSelection( )
	{
	DeleteSelectionCollection( );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::operator=( )
//
//  Description:		Assignment operator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCompositeSelection& RCompositeSelection::operator=( const RCompositeSelection& rhs )
	{
	// Remove any selections
	DeleteSelectionCollection( );

	// Duplicate the selections
	YSelectionCollectionIterator iterator = rhs.m_SelectionCollection.Start( );
	for( ; iterator != rhs.m_SelectionCollection.End( ); ++iterator )
		{
		// Create a new selection and add it to our list
		m_SelectionCollection.InsertAtEnd( ( *iterator )->Clone( ) );
		}

	// Tell the view the selection is changing
	m_pView->OnChangeSelection( );

	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Clone( )
//
//  Description:		virtual constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelection* RCompositeSelection::Clone( )
	{
	return new RCompositeSelection( *this );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::DeleteSelectionCollection( )
//
//  Description:		Deletes all selections from the selection collection
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCompositeSelection::DeleteSelectionCollection( )
	{
	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );
	while( iterator != m_SelectionCollection.End( ) )
		{
		YSelectionCollectionIterator temp = iterator;
		++iterator;
		delete *temp;
		m_SelectionCollection.RemoveAt( temp );
		}
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::ShouldRenderSelection( )
//
//  Description:		Called to determine if this selection should be rendered
//
//  Returns:			TRUE if the selection should be rendered
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RCompositeSelection::ShouldRenderSelection( ) const
	{
	return static_cast<BOOLEAN>( m_fSelectionVisible && !m_fSelectionHidden );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::UnselectAll( )
//
//  Description:		Removes all selections from this composite.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCompositeSelection::UnselectAll( BOOLEAN fRender )
	{
	// Create a drawing surface and transform
	RViewDrawingSurface drawingSurface( m_pView );
	R2dTransform transform;
	SetupRender( drawingSurface, transform );
	RRealRect rect( m_pView->GetSize( ) );
	
	// Remove all selections
	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );
	while( iterator != m_SelectionCollection.End( ) )
		{
		YSelectionCollectionIterator temp = iterator;
		++iterator;

		if( ShouldRenderSelection( ) && fRender )
			( *temp )->Render( drawingSurface, transform, rect );

		delete *temp;
		m_SelectionCollection.RemoveAt( temp );
		}

	// Tell the view the selection is changing
	m_pView->OnChangeSelection( );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Select( )
//
//  Description:		Adds a selection to this composite
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::Select( const RComponentView* pSelectableObject, BOOLEAN fRender )
	{
	// Validate
	TpsAssert( pSelectableObject, "NULL Selection" );
	TpsAssertValid( pSelectableObject );

	// Dont select if it is not selectable
	if( !pSelectableObject->GetComponentAttributes( ).IsSelectable( ) )
		return;

	// Check to see if it already in the selection list
	TpsAssert( IsSelected( pSelectableObject ) == FALSE, "Selection is already in this composite." );

	// Create a new selection and add it
	RSingleSelection* pNewSelection = new RSingleSelection( pSelectableObject, m_pView );
	m_SelectionCollection.InsertAtEnd( pNewSelection );

	// Create a drawing surface and transform
	RViewDrawingSurface drawingSurface( m_pView );
	R2dTransform transform;
	SetupRender( drawingSurface, transform );
	RRealRect rect( m_pView->GetSize( ) );

	if( fRender && ShouldRenderSelection( ) )
		pNewSelection->Render( drawingSurface, transform, rect );

	// Tell the view the selection is changing
	m_pView->OnChangeSelection( );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Select( )
//
//  Description:		Selects a component documents view
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::Select( const RComponentDocument* pDocument, BOOLEAN fRender )
	{
	// Select the view that is a child of our view
	Select( pDocument->GetView( m_pView ), fRender );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::ToggleSelect( )
//
//  Description:		Toggles the selection state of the specifed object
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::ToggleSelect( const RComponentView* pSelectableObject )
	{
	if( IsSelected( pSelectableObject ) )
		Unselect( pSelectableObject );
	else
		Select( pSelectableObject, TRUE );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Hide( )
//
//  Description:		Hides this selection so that it can not be rendered
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::Hide( )
	{
	m_fSelectionHidden = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Unselect( )
//
//  Description:		Removes a selection from this composite
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::Unselect( const RComponentView* pSelectableObject, BOOLEAN fRender )
	{
	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );
	for( ; iterator != m_SelectionCollection.End( ); ++iterator )
		{
		TpsAssert( dynamic_cast<RSingleSelection*>( *iterator ), "Nested compounds not implemented yet." );

		if( ( *iterator )->IsSelected( pSelectableObject ) )
			{
			// Create a drawing surface and transform
			RViewDrawingSurface drawingSurface( m_pView );
			R2dTransform transform;
			SetupRender( drawingSurface, transform );
			RRealRect rect( m_pView->GetSize( ) );

			if( ShouldRenderSelection( ) && fRender )
				( *iterator )->Render( drawingSurface, transform, rect );

			delete *iterator;
			m_SelectionCollection.RemoveAt( iterator );

			return;
			}
		}

	// Tell the view the selection is changing
	m_pView->OnChangeSelection( );

	TpsAssert( FALSE, "Object was not selected." );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Unselect( )
//
//  Description:		Removes a selection from this composite
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::Unselect( RSingleSelection* pSelection )
	{
	// Find the selection
	YSelectionCollectionIterator iterator = m_SelectionCollection.Find( pSelection );

	TpsAssert( iterator != m_SelectionCollection.End( ), "Object was not selected." );

	// Create a drawing surface and transform
	RViewDrawingSurface drawingSurface( m_pView );
	R2dTransform transform;
	SetupRender( drawingSurface, transform );
	RRealRect rect( m_pView->GetSize( ) );

	if( ShouldRenderSelection( ) )
		( *iterator )->Render( drawingSurface, transform, rect );

	delete *iterator;
	m_SelectionCollection.RemoveAt( iterator );

	// Tell the view the selection is changing
	m_pView->OnChangeSelection( );

	return;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Render( )
//
//  Description:		Draws the selection rect and handles
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, BOOLEAN fRenderIfLocked ) const
	{
	// Only do if the selection is visible or we have a deferred render pending
	if( !m_fSelectionHidden && m_fSelectionVisible )
		{
		// Dont bother rendering if there are no selected objects
		if( m_SelectionCollection.Count( ) != 0 )
			{
			drawingSurface.RestoreDefaults( );

			// Setup the drawing surface for rendering feedback
			RSolidColor color;
			YDrawMode drawMode;
			m_pView->GetFeedbackSettings( color, drawMode );
			drawingSurface.SetForegroundMode( drawMode );
			drawingSurface.SetPenColor( color );
			drawingSurface.SetFillColor( color );

			// Go through our selection collection and render each selection
			YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );
			for( ; iterator != m_SelectionCollection.End( ); ++iterator )
				( *iterator )->Render( drawingSurface, transform, rcRender, fRenderIfLocked );
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Render( )
//
//  Description:		Redraws the selection to the screen
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::Render( BOOLEAN fRenderIfLocked ) const
	{
	if( !m_fSelectionHidden )
		{
		// Make sure the selection is not visible
		TpsAssert( m_fSelectionVisible == FALSE, "Selection already visible, use Remove( ) instead." );

		// cast away constness because mutable is not supported
		const_cast<RCompositeSelection*>( this )->m_fSelectionVisible = TRUE;

		DoRender( fRenderIfLocked );
		}
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Remove( )
//
//  Description:		Removes the selection from the screen
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::Remove( BOOLEAN fRenderIfLocked  ) const
	{
	if( !m_fSelectionHidden )
		{
		// Make sure the selection is visible
		TpsAssert( m_fSelectionVisible == TRUE, "Selection not visible, use Render( ) instead." );

		if( m_fSelectionVisible )
			{
			// cast away constness because mutable is not supported
			const_cast<RCompositeSelection*>( this )->m_fSelectionVisible = FALSE;

			DoRender( fRenderIfLocked );
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Render( )
//
//  Description:		Creates a view drawingsurface and renders
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::DoRender( BOOLEAN fRenderIfLocked ) const
	{
	// Create a drawing surface and transform
	RViewDrawingSurface drawingSurface( m_pView );
	R2dTransform transform;
	SetupRender( drawingSurface, transform );
	RRealRect rect( m_pView->GetSize( ) );

	// Go through our selection collection and render each selection
	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );
	for( ; iterator != m_SelectionCollection.End( ); ++iterator )
		( *iterator )->Render( drawingSurface, transform, rect, fRenderIfLocked );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::SetupRender( )
//
//  Description:		Prepares the given drawing surface and transform for
//							rendering.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::SetupRender( RDrawingSurface& drawingSurface, R2dTransform& transform ) const
	{
	drawingSurface.RestoreDefaults( );

	// Setup the drawing surface for rendering feedback
	RSolidColor color;
	YDrawMode drawMode;
	m_pView->GetFeedbackSettings( color, drawMode );
	drawingSurface.SetForegroundMode( drawMode );
	drawingSurface.SetPenColor( color );
	drawingSurface.SetFillColor( color );

	m_pView->GetViewTransform( transform, drawingSurface, TRUE );
	//
	//	We can't set the clipping here for the components could
	//	lie outside of the view bounds
	//m_pView->SetClipRegion( drawingSurface, transform );
	}

// ****************************************************************************
//
//  Function Name:	::CompareSelection( )
//
//  Description:		Compares two single selections for z-order. Used by the
//							sort routine below.
//
//  Returns:			1 if selection1 is is higher in z-order than selection 2
//  Returns:			-1 if selection1 is is lower in z-order than selection 2
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
int CompareSelection( RSelection* pSelection1, RSelection* pSelection2 )
	{
	TpsAssert( dynamic_cast<RSingleSelection*>( pSelection1 ), "Nested compounds not implemented yet." );
	TpsAssert( dynamic_cast<RSingleSelection*>( pSelection2 ), "Nested compounds not implemented yet." );

	return( static_cast<RSingleSelection*>( pSelection1 )->IsHigherZOrder( *static_cast<RSingleSelection*>( pSelection2 ) ) ) ? 1 : -1;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::SortByZOrder( )
//
//  Description:		Sorts this composite selection by z-order, from highest
//							to lowest.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::SortByZOrder( )
	{
	Sort( m_SelectionCollection.Start( ), m_SelectionCollection.End( ), CompareSelection );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::DrawDragTrackingRect( )
//
//  Description:		Draws the selection tracking rect for dragging
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::DrawDragTrackingFeedback( RDrawingSurface& drawingSurface,
																	 const R2dTransform& transform,
																	 const RRealSize& offset ) const
	{
	// Go through the selection collection and render each selection
	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );
	for( ; iterator != m_SelectionCollection.End( ); ++iterator )
		( *iterator )->DrawDragTrackingFeedback( drawingSurface, transform, offset );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::DrawRotateTrackingRect( )
//
//  Description:		Draws the selection tracking rect for rotating.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//

void RCompositeSelection::DrawRotateTrackingFeedback( RDrawingSurface& drawingSurface,
																		const R2dTransform& transform,
																		const RRealPoint& rotationCenter,
																		YAngle rotationAngle ) const
	{
	// Go through the selection collection and render each selection
	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );
	for( ; iterator != m_SelectionCollection.End( ); ++iterator )
		( *iterator )->DrawRotateTrackingFeedback( drawingSurface, transform, rotationCenter, rotationAngle );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::DrawResizeTrackingRect( )
//
//  Description:		Draws the selection tracking rect for resizing.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::DrawResizeTrackingFeedback( RDrawingSurface& drawingSurface,
																		const R2dTransform& transform,
																		const RRealPoint& scalingCenter,
																		const RRealSize& scaleFactor,
																		BOOLEAN fMaintainAspectRatio ) const
	{
	// Go through the selection collection and render each selection
	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );
	for( ; iterator != m_SelectionCollection.End( ); ++iterator )
		( *iterator )->DrawResizeTrackingFeedback( drawingSurface, transform, scalingCenter, scaleFactor, fMaintainAspectRatio );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::GetDragTrackingFeedbackBoundingRect( )
//
//  Description:		Gets the bounding rect of drag tracking feedback
//
//  Returns:			Bounding rect
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RRealRect RCompositeSelection::GetDragTrackingFeedbackBoundingRect( const R2dTransform& transform,
																						  const RRealSize& offset ) const
	{
	TpsAssert( !IsEmpty( ), "Nothing selected" );

	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );

	// Use the first rectangle as the starting point, and union from there
	RRealRect boundingRect = ( *iterator )->GetDragTrackingFeedbackBoundingRect( transform, offset );
	++iterator;
																	
	for( ; iterator != m_SelectionCollection.End( ); ++iterator )
		boundingRect.Union( boundingRect, ( *iterator )->GetDragTrackingFeedbackBoundingRect( transform, offset ) );

	return boundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::GetRotateTrackingFeedbackBoundingRect( )
//
//  Description:		Gets the bounding rect of resize tracking feedback
//
//  Returns:			Bounding Rect
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RRealRect RCompositeSelection::GetRotateTrackingFeedbackBoundingRect( const R2dTransform& transform,
																							 const RRealPoint& rotationCenter,
																							 YAngle rotationAngle ) const
	{
	TpsAssert( !IsEmpty( ), "Nothing selected" );

	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );

	// Use the first rectangle as the starting point, and union from there
	RRealRect boundingRect = ( *iterator )->GetRotateTrackingFeedbackBoundingRect( transform, rotationCenter, rotationAngle );
	++iterator;
																	
	for( ; iterator != m_SelectionCollection.End( ); ++iterator )
		boundingRect.Union( boundingRect, ( *iterator )->GetRotateTrackingFeedbackBoundingRect( transform, rotationCenter, rotationAngle ) );

	return boundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::GetResizeTrackingFeedbackBoundingRect( )
//
//  Description:		Gets the bounding rect of resize tracking feedback
//
//  Returns:			Bounding rect
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RRealRect RCompositeSelection::GetResizeTrackingFeedbackBoundingRect( const R2dTransform& transform,
																							 const RRealPoint& scalingCenter,
																							 const RRealSize& scaleFactor,
																							 BOOLEAN fMaintainAspectRatio ) const
	{	
	TpsAssert( !IsEmpty( ), "Nothing selected" );

	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );

	// Use the first rectangle as the starting point, and union from there
	RRealRect boundingRect = ( *iterator )->GetResizeTrackingFeedbackBoundingRect( transform, scalingCenter, scaleFactor, fMaintainAspectRatio );
	++iterator;
																	
	for( ; iterator != m_SelectionCollection.End( ); ++iterator )
		boundingRect.Union( boundingRect, ( *iterator )->GetResizeTrackingFeedbackBoundingRect( transform, scalingCenter, scaleFactor, fMaintainAspectRatio ) );

	return boundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::GetSelectionBoundingRect( )
//
//  Description:		Retrieves the bounding rect of this selection
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::GetSelectionBoundingRect( YSelectionBoundingRect& boundingRect ) const
	{
	// Get the first bounding rect
	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );
	( *iterator )->GetSelectionBoundingRect( boundingRect );
	RRealRect unionRect = boundingRect.m_TransformedBoundingRect;
	++iterator;

	// Union in the rest of the bounding rects
	for( ; iterator != m_SelectionCollection.End( ); ++iterator )
		{
		( *iterator )->GetSelectionBoundingRect( boundingRect );
		unionRect.Union( unionRect, boundingRect.m_TransformedBoundingRect );
		}

	// Set the rect
	boundingRect.Set( unionRect );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::HitTest( )
//
//  Description:		Hits test the bounding rect of this selection
//
//  Returns:			The location of this selection that was hit
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RSelection::EHitLocation RCompositeSelection::HitTest( const RRealPoint& point ) const
	{
	// Get the hit object
	RSingleSelection* pHitObject = GetHitSingleSelection( point );

	// Now hit test it
	if( pHitObject )
		return pHitObject->HitTest( point );
	else
		return kNothing;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::IsSelected( )
//
//  Description:		Determines if the given selectable object is selected by
//							this selection.
//
//  Returns:			TRUE if the object is selected
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
BOOLEAN RCompositeSelection::IsSelected( const RComponentView* pSelectableObject )
	{
	// Go through our selection collection and test each selection
	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );
	for( ; iterator != m_SelectionCollection.End( ); ++iterator )
		if( ( *iterator )->IsSelected( pSelectableObject ) )
			return TRUE;

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Invalidate( )
//
//  Description:		Invalidates the region occupied by this selection in the
//							given view.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RCompositeSelection::Invalidate( BOOLEAN fInvalidateIfLocked ) const
	{
	const_cast<RCompositeSelection*>( this )->m_fSelectionVisible = TRUE;

	// Go through our selection collection and invalidate each selection
	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );
	for( ; iterator != m_SelectionCollection.End( ); ++iterator )
		( *iterator )->Invalidate( fInvalidateIfLocked );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::GetSelectionTracker( )
//
//  Description:		Creates and returns a mouse tracker appropriate for
//							tracking at the specified mouse down point.
//
//  Returns:			pointer to a new mouse tracker
//
//  Exceptions:		kMemoryException
//
// ****************************************************************************
//
RMouseTracker* RCompositeSelection::GetSelectionTracker( const RRealPoint& mouseDownPoint )
	{
	// Hit test to figure out what kind of tracking we are doing
	EHitLocation eHitLocation = HitTest( mouseDownPoint );

	// Create the appropriate tracker
	try
		{
		if( eHitLocation == kNothing )
			return NULL;

		else if( eHitLocation == kInside )
			return NULL;

		else if( eHitLocation == kRotateHandle )
			return m_pView->GetRotateTracker( );

		else
			return m_pView->GetResizeTracker( );
		}
	
	catch( ... )
		{
		;
		}
	
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::GetHitObject( )
//
//  Description:		Finds the selected object which contains the specified
//							point
//
//  Returns:			The hit object
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSingleSelection*	RCompositeSelection::GetHitSingleSelection( const RRealPoint& point ) const
	{
	// Local enum for hit test priority
	enum EHitPriority { kPriorityNothing, kPriorityInside, kPriorityResize, kPriorityRotate };

	RSingleSelection* pHitObject = NULL;
	EHitPriority eBestHitPriority = kPriorityNothing;

	YSelectionCollectionIterator iterator = m_SelectionCollection.Start( );
	for( ; iterator != m_SelectionCollection.End( ); ++iterator )
		{
		// Get the single selection
		TpsAssert( dynamic_cast<RSingleSelection*>( *iterator ), "Nested composites not supported yet." );
		RSingleSelection* pSelection = (RSingleSelection*)*iterator;

		// Hit test it
		EHitLocation eHitLocation = pSelection->HitTest( point );
		EHitPriority eHitPriority;

		// Convert that into a hit test priority
		switch( eHitLocation )
			{
			case kInside :
				eHitPriority = kPriorityInside;
				break;

			case kLeftResizeHandle :
			case kTopResizeHandle :
			case kRightResizeHandle :
			case kBottomResizeHandle :
			case kTopLeftResizeHandle :
			case kTopRightResizeHandle :
			case kBottomLeftResizeHandle :
			case kBottomRightResizeHandle :
				eHitPriority = kPriorityResize;
				break;

			case kRotateHandle :
				eHitPriority = kPriorityRotate;
				break;

			default :
				eHitPriority = kPriorityNothing;
			}

		// If the hit priority of the selection is greater than the current best priority,
		// set this on as the current best
		if( eHitPriority > eBestHitPriority )
			{
			eBestHitPriority = eHitPriority;
			pHitObject = pSelection;
			}

		// If the priorities were equal and not kNothing, use this object if it has a higher
		// z-order than the current best
		else if( ( eBestHitPriority != kNothing ) && ( eBestHitPriority == eHitPriority ) )
			{
			if( pSelection->IsHigherZOrder( *pHitObject ) )
				{
				pHitObject = pSelection;
				}
			}
		}

	return pHitObject;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::SetCursor( )
//
//  Description:		Sets an appropriate cursor for visual feedback
//
//  Returns:			TRUE if the selection was hit and the cursor changed.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RCompositeSelection::SetCursor( const RRealPoint& mousePoint ) const
	{
	// Get the hit object
	RSingleSelection* pHitObject = GetHitSingleSelection( mousePoint );

	// If we didnt hit an object return
	if( !pHitObject )
		return FALSE;
	
	// Find out where on the object we hit
	EHitLocation eHitLocation = pHitObject->HitTest( mousePoint );
	
	switch( eHitLocation )
		{
		case kInside :
			::GetCursorManager( ).SetCursor( CURSOR_RESOURCE_MOVE );
			break;

		case kRotateHandle :
			::GetCursorManager( ).SetCursor( CURSOR_RESOURCE_ROTATE );
			break;

		default :
			SetResizeCursor( pHitObject, eHitLocation );
		}

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::SetResizeCursor( )
//
//  Description:		Calculates which resize cursor to use, and sets it
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCompositeSelection::SetResizeCursor( RSingleSelection* pHitObject, EHitLocation eHitLocation ) const
	{
	// Get the center point of the object
	YComponentBoundingRect boundingRect;
	pHitObject->GetObjectBoundingRect( boundingRect );
	RRealPoint centerPoint = boundingRect.GetCenterPoint( );

	// Create a new bounding rect that is a square.
	YComponentBoundingRect squareRect = boundingRect;
	RRealSize size = squareRect.WidthHeight( );
	if( size.m_dx > size.m_dy )
		squareRect.UnrotateAndScaleAboutPoint( centerPoint, RRealSize( size.m_dy / size.m_dx, 1.0 ) );
	else
		squareRect.UnrotateAndScaleAboutPoint( centerPoint, RRealSize( 1.0, size.m_dx / size.m_dy ) );

	// Select a point from this square based on the hit location
	RRealPoint point;
	switch( eHitLocation )
		{
		case kLeftResizeHandle :
			::midpoint( point, squareRect.m_TopLeft,squareRect.m_BottomLeft);
			break;

		case kTopResizeHandle :
			::midpoint( point, squareRect.m_TopLeft,squareRect.m_TopRight);
			break;

		case kRightResizeHandle :
			::midpoint( point, squareRect.m_TopRight,squareRect.m_BottomRight);
			break;

		case kBottomResizeHandle :
			::midpoint( point, squareRect.m_BottomLeft,squareRect.m_BottomRight);
			break;

		case kTopLeftResizeHandle :
			point = squareRect.m_TopLeft;
			break;

		case kTopRightResizeHandle :
			point = squareRect.m_TopRight;
			break;

		case kBottomLeftResizeHandle :
			point = squareRect.m_BottomLeft;
			break;

		case kBottomRightResizeHandle :
			point = squareRect.m_BottomRight;
			break;

		default :
			TpsAssertAlways( "Invalid hit location in RCompositeSelection::SetResizeCursor( )." );
		}

	// Calculate the angle between a vertical line through the square center and
	// a line connecting the calculatated point and the square center.
	YAngle angle = ::atan2( (YFloatType)( point.m_x - centerPoint.m_x ), (YFloatType)( centerPoint.m_y - point.m_y ) );

	// atan2 returns an angle between PI and -PI. Convert this to an angle between 0 and 2PI
	if( angle < 0 )
		angle += ( 2 * kPI );

	// Select a cursor. We have 4 cursors to choose from, rotated in 45 degree
	// increments. Divide the angle by 45 deg. and use the resulting octant
	// number mod 4 to index into an array of cursor ids.
	int nOctant = ::Round( angle / ( kPI / 4 ) );

	TpsAssert( nOctant >= 0 && nOctant <= 8, "Invalid octant." );

#ifdef _WINDOWS
	static LPCTSTR cursorArray[ 4 ] = { IDC_SIZENS, IDC_SIZENESW, IDC_SIZEWE, IDC_SIZENWSE };
#else
	// REVIEW: define cursors for Mac
#endif

	// Set the cursor
	::GetCursorManager( ).SetCursor( cursorArray[ nOctant % 4 ] );
	}

// ****************************************************************************
//
// Function Name:		RCompositeSelection::Write
//
// Description:		Writes this selection to the specified archive
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RCompositeSelection::Write( RArchive& archive ) const
	{
	uLONG	count = Count( );

	//	First, store the number of items in the selection
	archive << count;

	RView* pView = GetView();
	YSelectionIterator iterator = Start( );
	for( count = 0; iterator != End( ); ++iterator, ++count )
		{
		//	Retrieve the indicies of the views and write them out
		archive << pView->GetIndexOfComponentView( *iterator );
		}

	TpsAssert( count == (uLONG)Count( ), "Count written to archive does not match count in selection" );
	}

// ****************************************************************************
//
// Function Name:		RCompositeSelection::Read
//
// Description:		Reads this selection from the specified archive
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RCompositeSelection::Read( RArchive& archive )
	{
	uLONG	selectionCount;
	uLONG	count;

	//	Remove current selection...
	UnselectAll( );
	Remove( );

	try
		{
		archive >> selectionCount;

		YComponentViewIndex	index;

		for( count = 0; count < selectionCount; ++count )
			{
			archive >> index;
			Select( GetView( )->GetComponentViewByIndex( index ), FALSE );
			}
		}

	catch( ... )
		{
		;
		}
	}
// ****************************************************************************
//
//  Function Name:	RCompositeSelection::GetBoundingRect( )
//
//  Description:		Gets the bounding rect of all objects in this selection
//
//  Returns:			The bounding rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RCompositeSelection::GetBoundingRect( ) const
	{
	TpsAssert( !IsEmpty( ), "Nothing selected" );

	YSelectionIterator iterator = Start( );

	// Use the first rectangle as the starting point, and union from there
	RRealRect boundingRect = ( *iterator )->GetBoundingRect( ).m_TransformedBoundingRect;
	++iterator;
																	
	for( ; iterator != End( ); ++iterator )
		boundingRect.Union( boundingRect, ( *iterator )->GetBoundingRect( ).m_TransformedBoundingRect );

	return boundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::GetBoundingRectIncludingHandles
//
//  Description:		Calculates a bounding rect for the specified object that
//							includes the resize and rotate handles
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RCompositeSelection::GetBoundingRectIncludingHandles( ) const
	{
	TpsAssert( !IsEmpty( ), "Nothing selected" );

	YSelectionIterator iterator = Start( );

	// Use the first rectangle as the starting point, and union from there
	RRealRect boundingRect = RSingleSelection::GetBoundingRectIncludingHandles( *iterator );
	++iterator;
																	
	for( ; iterator != End( ); ++iterator )
		boundingRect.Union( boundingRect, RSingleSelection::GetBoundingRectIncludingHandles( *iterator ) );

	return boundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Start( )
//
//  Description:		Selection iteration. For now these functions are just
//							stubs to the selection collection. If needed they can be
//							expanded to include support for nested composite selections.
//
//  Returns:			Start iterator
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCompositeSelection::YIterator RCompositeSelection::Start( ) const
	{
	return RSelectionIterator( m_SelectionCollection.Start( ) );
	}

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::End( )
//
//  Description:		Selection iteration. For now these functions are just
//							stubs to the selection collection. If needed they can be
//							expanded to include support for nested composite selections.
//
//  Returns:			End iterator
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCompositeSelection::YIterator RCompositeSelection::End( ) const
	{
	return RSelectionIterator( m_SelectionCollection.End( ) );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionIterator::RSelectionIterator( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCompositeSelection::RSelectionIterator::RSelectionIterator( YSelectionCollectionIterator iterator )
	: m_Iterator( iterator )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSelectionIterator::operator*( )
//
//  Description:		Dereference operator
//
//  Returns:			Selectable object
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RCompositeSelection::RSelectionIterator::operator*( ) const
	{
	RSingleSelection* pSelection = dynamic_cast<RSingleSelection*>( *m_Iterator );
	TpsAssert( pSelection, "Nested compound selections not implemented yet." );
	return const_cast<RComponentView*>( pSelection->m_pSelectedObject );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionIterator::operator*( )
//
//  Description:		Increment operator
//
//  Returns:			this iterator
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCompositeSelection::RSelectionIterator& RCompositeSelection::RSelectionIterator::operator++( )
	{
	m_Iterator++;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RSelectionIterator::operator*( )
//
//  Description:		Increment operator
//
//  Returns:			this iterator
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCompositeSelection::RSelectionIterator& RCompositeSelection::RSelectionIterator::operator--( )
	{
	m_Iterator--;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RSelectionIterator::operator!=( )
//
//  Description:		Inequality operator
//
//  Returns:			TRUE if this iterator != the given iterator
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RCompositeSelection::RSelectionIterator::operator!=( const RSelectionIterator& rhs ) const
	{
	return m_Iterator != rhs.m_Iterator;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RCompositeSelection::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCompositeSelection::Validate( )	const
	{
	RSelection::Validate( );
	TpsAssertIsObject( RCompositeSelection, this );
	}

#endif
