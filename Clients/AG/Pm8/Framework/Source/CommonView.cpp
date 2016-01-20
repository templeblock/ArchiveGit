// ****************************************************************************
//
//  File Name:			CommonView.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Definition of the RView class
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
//  $Logfile:: /PM8/Framework/Source/CommonView.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "View.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ComponentView.h"
#include	"ViewDrawingSurface.h"
#include	"SelectionTracker.h"
#include "SingleSelection.h"
#include "CompositeSelection.h"
#include "DeactivateComponentAction.h"
#include "SelectionRectTracker.h"
#include "CopySelectionAction.h"
#include "DragDropDataTransfer.h"
#include "PasteAction.h"
#include "ChunkyStorage.h"
#include "ComponentDataRenderer.h"
#include "Path.h"
#include "ApplicationGlobals.h"
#include "DragDropAction.h"
#include "NewComponentAction.h"
#include "DuplicateSelectionAction.h"
#include "MergeData.h"
#include "ComponentDocument.h"
#include "WindowView.h"
#include "AutoDrawingSurface.h"
#include "Caret.h"
#include "Menu.h"
#include "FrameworkResourceIds.h"
#include "TrackingFeedbackRenderer.h"
#include "ReplaceComponentAction.h"
#include "MoveSelectionAction.h"
#include "PrinterStatus.h"
#include "ImageLibrary.h"
#include "OffscreenDrawingSurface.h"
#include "BitmapImage.h"
#include "ComponentTypes.h"
#include "SoftShadowSettings.h"
#include "SoftGlowSettings.h"
#include "SoftVignetteSettings.h"
#include "EdgeOutlineSettings.h"
#include "Configuration.h"

#undef Inherited
#define Inherited RObject

// Static flag so we can figure out if Renaissance was a drop target
BOOLEAN RView::m_fDroppedInRenaissance;

// Define the invalid insertion point
const YIntCoordinate kInvalidInsertionPoint = -1;

// Modifier keys
const YModifierKey kReverseTabOrderModifierKey = kModifierShift;
const YModifierKey kLargeNudgeModifierKey = kModifierControl;

// Nudge configuration
const YRealDimension kNudgeAmount = ::InchesToLogicalUnits( 0.01 );
const YScaleFactor kLargeNudgeScaleFactor = 10;

//	chunk tags
const uLONG kViewTag          = 'VWDT';
const uLONG kSoftShadowTag    = 'SFTS';
const uLONG kSoftGlowTag      = 'SFTG';
const uLONG kSoftVignetteTag  = 'SFTV';
const uLONG kAlphaMaskTag     = 'SFAM';
const uLONG kAlphaMaskInfoTag = 'SFAI';
const uLONG kEdgeOutlineTag   = 'SFEO';

// Constants used to do mouse debouncing
#ifdef _WINDOWS
	YIntDimension kDragMinimumDistance = ::GetProfileInt( _T( "windows" ), _T( "DragMinDist" ), DD_DEFDRAGMINDIST );
	YTickCount kDragDelay = ::GetProfileInt( _T( "windows" ), _T( "DragDelay" ), DD_DEFDRAGDELAY );
#endif

// ****************************************************************************
//
//  Function Name:	RView::RView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RView::RView( )
	: m_fMouseCaptured( FALSE ),
	  m_pActiveComponent( NULL ),
	  m_pActiveTracker( NULL ),
	  m_pDocument( NULL ),
	  m_fTrackSelection( FALSE ),
	  m_MouseDownCount( 0 ),
	  m_MouseDownTime( 0 ),
	  m_InsertionPoint( kInvalidInsertionPoint, kInvalidInsertionPoint ),
	  m_fTiledView ( FALSE ),
	  m_pComponentCollection( NULL ),
	  m_pNudgeAction( NULL ),
	  m_fToggleSelecting( FALSE ),
	  m_fInitialSelection( FALSE ),
	  m_pFeedbackRenderer( NULL ),
	  m_fDragDropConvertedComponent( FALSE ),
	  m_pMergeEntry( NULL ),
	  m_fIgnoreZOrder( FALSE ),
	  m_pShadowSettings( NULL ),
	  m_pGlowSettings( NULL ),
	  m_pVignetteSettings( NULL ),
	  m_pAlphaMask( NULL ),
      m_kSpecialEdgeType( 0 ),
	  m_dwMaskID( 0 ),
	  m_fHiResPhoto( FALSE ),
	  m_pEdgeOutlineSettings( NULL )
	{
	m_pSelection = new RCompositeSelection( this );
	m_pShadowSettings = new RSoftShadowSettings;
	m_pGlowSettings = new RSoftGlowSettings;
	m_pVignetteSettings = new RSoftVignetteSettings;
	m_pEdgeOutlineSettings = new REdgeOutlineSettings;

	uLONG uHiResPhoto;
	if (RRenaissanceUserPreferences().GetLongValue(RRenaissanceUserPreferences::kHiResPhoto, uHiResPhoto))
		m_fHiResPhoto = static_cast<BOOLEAN>(uHiResPhoto);
	}

// ****************************************************************************
//
//  Function Name:	RView::~RView( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RView::~RView( )
	{
	delete m_pSelection;	
	delete m_pShadowSettings;
	delete m_pGlowSettings;
	delete m_pVignetteSettings;
	delete m_pEdgeOutlineSettings;

	if (m_pAlphaMask != NULL)
	{
		delete m_pAlphaMask;
		m_pAlphaMask = NULL;
		m_dwMaskID   = 0;		
	}
	}

// ****************************************************************************
//
//  Function Name:	RView::Initialize
//
//  Description:		Initializer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::Initialize( RDocument* pDocument )
	{
	TpsAssertValid( pDocument );

	m_pDocument = pDocument;
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXRender( )
//
//  Description:		Called in response to a render message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnXRender( RDrawingSurface& drawingSurface, RIntRect& rcRender )
	{
	RAutoCaret	autoCaret;
	RWaitCursor	waitCursor( FALSE );	//	Delay wait cursor

	// Get a transform
	R2dTransform transform;
	GetViewTransform( transform, drawingSurface, TRUE );

	// Setup appropriate clipping
	SetClipRegion( drawingSurface, transform );

	//	Prepare the offscreen
	RAutoDrawingSurface&	autoSurface = GetAutoDrawingSurface( );

	// Save the transform and rcRender
	R2dTransform screenTransform = transform;
	RIntRect screenrcRender = rcRender;

	if( !drawingSurface.IsPrinting( ) && autoSurface.Prepare( &drawingSurface, transform, rcRender ) )
		{
		//	Convert rcRender to be relative to the autoSurface
		rcRender	= RIntRect( rcRender.WidthHeight( ) );

		try
			{
			Render( autoSurface, transform, rcRender );
			}

		catch( ... )
			{
			TpsAssertAlways( "Someone did not properly catch during a paint message" );
			}

		autoSurface.Release( );
		}

		// Try without the offscreen
	else
		Render( drawingSurface, transform, rcRender );

	RenderSelection( drawingSurface, screenTransform, screenrcRender );
	}

// ****************************************************************************
//
//  Function Name:	RView::AddComponentView
//
//  Description:		Adds a new component view to this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::AddComponentView( RComponentView* pView )
	{
	TpsAssertValid( pView );

	// Set ourself as the component views parent
	pView->SetParentView( this );

	// If z-order isnt meaningful in this view, just insert it at the end
	if( m_fIgnoreZOrder )
		m_ComponentViewCollection.InsertAtEnd( pView );

	else
		{
		// Get the views Z-Layer
		YZLayer zLayer = pView->GetComponentAttributes( ).GetZLayer( );

		// Iterate the component view collection, looking for the first view with a Z-Layer
		// less than or equal to this view. Insert after that component view
		YComponentViewIterator iterator = m_ComponentViewCollection.End( );

		// Make sure there is at least 1 node
		if( iterator != m_ComponentViewCollection.Start( ) )
			{
			do
				{
				--iterator;
				if( ( *iterator )->GetComponentAttributes( ).GetZLayer( ) <= zLayer )
					{
					m_ComponentViewCollection.InsertAt( ++iterator, pView );
					return;
					}
				}
			while( iterator != m_ComponentViewCollection.Start( ) );
			}

		// Didnt find anything; just insert at the start of the list
		m_ComponentViewCollection.InsertAtStart( pView );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::RemoveComponentView
//
//  Description:		Removes a component view from this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RemoveComponentView( RComponentView* pView )
	{
	TpsAssertValid( pView );

	// Make sure its in our view list
	TpsAssert( m_ComponentViewCollection.Find( pView ) != m_ComponentViewCollection.End( ), "Attempt to remove a view that is not a child of this view." );

	m_ComponentViewCollection.Remove( pView );
	pView->SetParentView( NULL );
	}

// ****************************************************************************
//
//  Function Name:	RView::GetIndexOfComponentView
//
//  Description:		Return the index of the given component view
//
//  Returns:			The index
//
//  Exceptions:		None
//
// ****************************************************************************
//
YComponentViewIndex RView::GetIndexOfComponentView( RComponentView* pView )
	{
	return static_cast<YComponentViewIndex>( ::GetIndex( m_ComponentViewCollection, pView ) );
	}

// ****************************************************************************
//
//  Function Name:	RView::GetComponentViewByIndex
//
//  Description:		Return the component view identified by the given index
//
//  Returns:			The desired view (if valid)
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RView::GetComponentViewByIndex( YComponentViewIndex index )
	{
	RComponentView*	pFoundView = NULL;
	::GetByIndex( m_ComponentViewCollection, static_cast<YCollectionIndex>(index), pFoundView );
	return pFoundView;
	}


// ****************************************************************************
//
//  Function Name:	RView::GetActiveComponent
//
//  Description:		Return the active component View
//
//  Returns:			m_pActiveComponent
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RView::GetActiveComponent( ) const
	{
	return m_pActiveComponent;
	}

// ****************************************************************************
//
//  Function Name:	RView::ActivateComponentView
//
//  Description:		Activate the given component View.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::ActivateComponentView( RComponentView* pView )
	{
	//	If we are already active. do nothing...
	if ( m_pActiveComponent == pView )
		return;

	DeactivateComponentView( );

	try
		{
		OnXKillKeyFocus( );
		m_pActiveComponent = pView;
		m_pActiveComponent->ActivateComponent( );
		}
	catch ( ... )
		{
		//		Error must be reported by the component
		m_pActiveComponent = NULL;
		//	Reset KeyFocus to this view since the component is not active.
		OnXSetKeyFocus();
		throw;
		}

		//	Assume that this Standalone already has key focus therefore,
		//	just internal send OnXKillKeyFocus and OnXSetKeyFocus to
		//	cause the keyfocus changing
		m_pActiveComponent->OnXSetKeyFocus( );
		m_pActiveComponent->OnXActivate( TRUE );
		m_pActiveComponent->UpdateCursor( );
	}


// ****************************************************************************
//
//  Function Name:	RView::DeactivateComponentView
//
//  Description:		Deactivate the currently active component view.
//
//  Returns:			Pointer to the view that used to be active
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RView::DeactivateComponentView( )
	{
	// If no view is active, just bail
	if( m_pActiveComponent == NULL )
		return NULL;

	// Save the old view so we can return it
	RComponentView* pOldActiveView = m_pActiveComponent;

	//	Assume that this StandaloneView has KeyFocus already, just transfer
	//	internal keyfocus with OnXKillKeyFocus and OnXSetKeyFocus messages
	m_pActiveComponent->OnXActivate( FALSE );
	m_pActiveComponent->OnXKillKeyFocus( );
	m_pActiveComponent->DeactivateComponent( );
	m_pActiveComponent = NULL;

	OnXSetKeyFocus( );
	UpdateCursor( );

	return pOldActiveView;
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXDisplayChange( )
//
//  Description:		Called when the display resolution changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnXDisplayChange( )
	{
	// Tell our embedded component views
	YComponentViewIterator iterator = m_ComponentViewCollection.Start( );
	for( ; iterator != m_ComponentViewCollection.End( ); ++iterator )
		( *iterator )->OnXDisplayChange( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXMouseMessage( )
//
//  Description:		Handles XPlatform mouse messages
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnXMouseMessage( EMouseMessages eMessage, RRealPoint devicePoint, YModifierKey modifierKeys )
	{
	// If a component is active, adjust the mouse point
	if( m_pActiveComponent )
		{
		// if the component has captured the mouse, or it is in its bounding rect, give it the event
		YComponentBoundingRect boundingRect = m_pActiveComponent->GetBoundingRect( );
		R2dTransform transform;
		m_pActiveComponent->GetParentView( )->GetViewTransform( transform, TRUE );
		boundingRect *= transform;

		if( m_pActiveComponent->IsMouseCaptured( ) || boundingRect.PointInRect( devicePoint ) )
			{
			static_cast<RView*>( m_pActiveComponent )->OnXMouseMessage( eMessage, devicePoint, modifierKeys );
			return;
			}
		}

	// Convert the device coordinates to logical coordinates in this views coordinate system
	R2dTransform transform;
	GetViewTransform( transform, TRUE );
	transform.Invert( );
	devicePoint *= transform;

	// switch on the mouse message and handle it
	switch( eMessage )
		{
		case kLeftButtonDown :
		case kLeftButtonDoubleClick :
			//	Setup state variables
			SetupMouseDownState( devicePoint, modifierKeys );
			//	Make sure the double click is really a double click
			if ( eMessage == kLeftButtonDoubleClick )
				m_MouseDownCount	= Max( m_MouseDownCount, 2 );	
			OnXLButtonDown( devicePoint, modifierKeys );
			break;

		case kRightButtonDown :
			OnXRButtonDown( devicePoint, modifierKeys );
			break;

		case kLeftButtonUp :
			OnXLButtonUp( devicePoint, modifierKeys );
			break;

		case kMouseMove :
			OnXMouseMove( devicePoint, modifierKeys );
			break;

		case kContextMenu :
			OnXContextMenu( devicePoint );
			break;

		case kRightButtonDoubleClick :
		case kRightButtonUp :
		case kMiddleButtonDown :
		case kMiddleButtonDoubleClick :
		case kMiddleButtonUp :
			break;

		case kSetCursor :
			OnXSetCursor( devicePoint, modifierKeys );
			break;

		default :
			TpsAssert( FALSE, "Unknown mouse message." );
			break;
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::SetupMouseDownState( )
//
//  Description:		Sets up the mouse down state so that multiple clicks can
//							be captured.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
#ifdef	MAC
const	int	doubleClickDelta = 2;
#endif
void RView::SetupMouseDownState( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	BOOLEAN		fIncrement	= FALSE;
	YTickCount	curTime		= ::RenaissanceGetTickCount();

	//	If mouse down count is greater than 0, check for double/triple/etc.
	if (m_MouseDownCount > 0)
		{
		YTickCount	nDTime	= MacWinDos( ::GetDblTime(), ::GetDoubleClickTime( ), xxx );

		//	First check time delta for doubleclick		
		if ( (curTime - m_MouseDownTime) < nDTime )
			{
			//	Now create a delta rectangle for double click and call PointInRect
			int			nDx	= MacWinDos( doubleClickDelta,	::GetSystemMetrics(SM_CXDOUBLECLK),	xxx );
			int			nDy	= MacWinDos( doubelClickDelta,	::GetSystemMetrics(SM_CYDOUBLECLK),	xxx );
			RRealRect		doubleClickRect( m_MouseDownPoint.m_x-nDx, m_MouseDownPoint.m_y-nDy,
												  m_MouseDownPoint.m_x+nDx, m_MouseDownPoint.m_y+nDy );
			if (doubleClickRect.PointInRect( mousePoint ))
				fIncrement = TRUE;
			}
		}

	//	Setup mouse counter
	if (fIncrement)
		++m_MouseDownCount;
	else
		m_MouseDownCount = 1;

	//	Setup new values for
	m_MouseDownPoint			= mousePoint;
	m_MouseDownTime			= curTime;
	m_MouseDownModifierKeys = modifierKeys;
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXLButtonDown( )
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
void RView::OnXLButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	TpsAssert( m_pActiveTracker == NULL, "Tracking in button down." );

	//	If click count is 2, this is a double click...
	if (m_MouseDownCount == 2)
		{
		// Get the hit component view
		RComponentView* pHitComponent = GetHitComponentView( mousePoint );

		// Make sure we didnt hit an active component. This should never happen.
		TpsAssert( ( !pHitComponent ) || ( pHitComponent != m_pActiveComponent ), "Received mouse event for an active component." );

		// if we double clicked a component, let it process an edit command
		//	reset clickCount and return
		if( pHitComponent && pHitComponent->GetComponentAttributes( ).IsEditable( ) )
			{
			m_MouseDownCount = 0;	//	we don't want the click to ever be above 2
			CallEditComponent( pHitComponent, kDoubleClick, pHitComponent->ConvertPointToLocalCoordinateSystem( mousePoint ) );
			return;
			}
		m_MouseDownCount = 1;		//	continue processing as a single click
		}

	// Set the mouse capture
	SetMouseCapture( );

	//	Must be a single click OR a double click on nothing
	// if there is a component currently active, deactivate it
	if( m_pActiveComponent )
		{
		//	If nested active components are allowed, This would have to be rewritten to
		//		make sure the Deactivates nest all the way from the most active to here.
		RDeactivateComponentAction*	pAction = new RDeactivateComponentAction( this );
		GetRDocument()->SendAction( pAction );
		}

	// Get the hit component view
	RComponentView* pHitComponent = GetHitComponentView( mousePoint );

	// Make sure we didnt hit an active component. This should never happen.
	TpsAssert( ( !pHitComponent ) || ( pHitComponent != m_pActiveComponent ), "Received mouse event for an active component." );

	// Check to see if the click should go to the selection
	if( ShouldSelectionHandleClick( mousePoint, modifierKeys ) )
		m_fTrackSelection = TRUE;

	else
		{
		// Check to see if we hit a component
		if( pHitComponent )
			{
			RRealPoint localPoint = pHitComponent->ConvertPointToLocalCoordinateSystem( mousePoint );

			// If the modifier key is down, we are toggle selecting
			if( modifierKeys & kToggleSelectionModifierKey )
				{
				// If the hit component is not selected, select it immediately
				if( !GetSelection( )->IsSelected( pHitComponent ) )
					{
					//	Select the object
					GetSelection( )->Select( pHitComponent, TRUE );

					//	We are selecting it for the first time
					m_fInitialSelection	= TRUE;
					}
				// If it is selected, we should only toggle select on a click, so as not to interfere with
				// control dragging (which copies). Remember that we are toggle selecting.
				else
					m_fToggleSelecting = TRUE;
				}

			// Check to see if the hit component can activate on a single click
			else if( pHitComponent->CanSingleClickActivate( localPoint ) )
				{
				CallEditComponent( pHitComponent, kSingleClick, localPoint );
				return;
				}

			// otherwise, we just select it if it isnt already
			else if( !GetSelection( )->IsSelected( pHitComponent ) )
				{
				// Unselect everything
				GetSelection( )->UnselectAll( );

				// Select the object
				GetSelection( )->Select( pHitComponent, TRUE );

				//	We are selecting it for the first time
				m_fInitialSelection = TRUE;
				}

			// Check the selection again
			if( GetSelection( )->HitTest( mousePoint ) != RSelection::kNothing )
				m_fTrackSelection = TRUE;
			}
		else
			{
			// If we didnt hit the component, and we are not tracking the selection, then either we hit nothing,
			// or we are toggle selected the area between the edge of the component and the edge of the selection
			// frame
			RSingleSelection* pSingleSelection = GetSelection( )->GetHitSingleSelection( mousePoint );
			if( pSingleSelection )
				{
				TpsAssert( modifierKeys & kToggleSelectionModifierKey, "Why are we here?" );
				GetSelection( )->Unselect( pSingleSelection );
				}
			}
		}
	
	// if no component was hit, and no selection was hit, begin selection rect tracking
	if( !pHitComponent && !m_fTrackSelection )
		{
		// Create a selection rect tracker
		m_pActiveTracker = new RSelectionRectTracker( this );
		m_pActiveTracker->BeginTracking( mousePoint, modifierKeys );
		}

	// Set the cursor
	OnXSetCursor( mousePoint, modifierKeys );
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
void RView::OnXRButtonDown( const RRealPoint& mousePoint, YModifierKey )
	{
	// If we are tracking, ignore the message
	if( m_pActiveTracker != NULL )
		return;

	//	If there is an active component, deactivate it
	if( m_pActiveComponent )
		{
		RDeactivateComponentAction* pAction = new RDeactivateComponentAction( this );
		GetRDocument( )->SendAction( pAction );
		}

	// Get the hit component view
	RComponentView* pHitComponent = GetHitComponentView( mousePoint );

	// Make sure we didnt hit an active component. This should never happen.
	TpsAssert( ( !pHitComponent ) || ( pHitComponent != m_pActiveComponent ), "Received mouse event for an active component." );

	// If we didnt hit a component, unselect everything
	if( !pHitComponent )
		GetSelection( )->UnselectAll( );

	// If we hit an unselected component is not selected, select it
	else if( !GetSelection( )->IsSelected( pHitComponent ) )
		{
		// Unselect everything
		GetSelection( )->UnselectAll( );

		// Select the object
		GetSelection( )->Select( pHitComponent, TRUE );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXLButtonUp( )
//
//  Description:		Default handler for the button up XEvent.
//
//							Will end any current mouse tracking.  If a component was hit
//							but never moved, it will activate it.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnXLButtonUp( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Release the mouse capture
	ReleaseMouseCapture( );

	// if we are tracking, end
	if( m_pActiveTracker )
		{
		TpsAssertValid( m_pActiveTracker );
		m_pActiveTracker->EndTracking( mousePoint, modifierKeys );
		delete m_pActiveTracker;
		m_pActiveTracker = NULL;
		}

	else if( m_fToggleSelecting )
		{
		// Get the component view that was originally hit
		RComponentView* pHitComponent = GetHitComponentView( m_MouseDownPoint );
		TpsAssert( pHitComponent, "No hit component to toggle!" );

		// Unselect it
		GetSelection( )->Unselect( pHitComponent );

		// Reset the toggle selecting flag
		m_fToggleSelecting = FALSE;
		}

	m_fTrackSelection		= FALSE;
	m_fInitialSelection	= FALSE;

	// Set the cursor
	OnXSetCursor( mousePoint, modifierKeys );
	}

// ****************************************************************************
//
//  Function Name:	RView::ShouldStartTracking( )
//
//  Description:		Determines if this view should start tracking
//
//  Returns:			TRUE if we should start tracking
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::ShouldStartTracking( const RRealPoint& mousePoint ) const
	{
	// First check to see if we are even set up for tracking
	if( !m_fTrackSelection )
		return FALSE;

	// Convert the drag minimum distance from pixels to logical units
	RRealSize dragMinimumDistance( kDragMinimumDistance, kDragMinimumDistance );
	::DeviceUnitsToLogicalUnits( dragMinimumDistance, *this );

	// Compute the time delta and the drag delta
	YTickCount timeDelta = ::RenaissanceGetTickCount( ) - m_MouseDownTime;
	RRealSize dragDelta = mousePoint - m_MouseDownPoint;

	// If we have moved the mouse enough, or enough time has elapsed, begin trackiing
	if( timeDelta >= kDragDelay || ::Abs( dragDelta.m_dx ) >= dragMinimumDistance.m_dx || ::Abs( dragDelta.m_dy ) >= dragMinimumDistance.m_dy )
		return TRUE;

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXMouseMove( )
//
//  Description:		Default handler for the mouse move XEvent.
//
//							Will continue processing any current tracking or start tracking
//							any component hit if the delta move is larger than a defined slop
//							distance.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void RView::OnXMouseMove( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// If we have an active tracker, continue
	if( m_pActiveTracker )
		{
		TpsAssertValid( m_pActiveTracker );

		// Setup the autoscrolling timer
		SetupAutoScrollTimer( mousePoint );
		
		try
			{
			m_pActiveTracker->ContinueTracking( mousePoint, modifierKeys );
			}

		catch( ... )
			{
			CancelTracking( );
			}
		}

	// Check to see if we should start tracking
	else if( ShouldStartTracking( mousePoint ) )
		{
		// We are not toggle selecting any more
		m_fToggleSelecting = FALSE;

		// Try to get a selection tracker if we are not initially selecting the object
		if ( !m_fInitialSelection )
			m_pActiveTracker = GetSelection( )->GetSelectionTracker( m_MouseDownPoint );
		//	I am not in the initial selection anymore, I am now doing something
		m_fInitialSelection	= FALSE;

		// If we got a tracker, begin tracking
		if( m_pActiveTracker )
			m_pActiveTracker->BeginTracking( m_MouseDownPoint, m_MouseDownModifierKeys );

		// If we didnt get a tracker, and we hit the selection, we must do drag&drop
		else if( GetSelection( )->HitTest( m_MouseDownPoint ) != RSelection::kNothing )
			DoDragDrop( m_MouseDownPoint, FALSE );
		}

	//	If we are not tracking or setting up for a track, process a SetCursor...
	else if ( !m_fTrackSelection )
		OnXSetCursor( mousePoint, modifierKeys );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXContextMenu( )
//
//  Description:		Called to display a context menu
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnXContextMenu( const RRealPoint& mousePoint )
	{
	RPopupMenu* pContextMenu = NULL;

	// If nothing is selected, get the context menu for this view
	if( GetSelection( )->Count( ) == 0 )
		pContextMenu = GetContextMenu( );

	// If there is a single object selected, get its context menu
	else if( GetSelection( )->Count( ) == 1 )
		pContextMenu = ( *GetSelection( )->Start( ) )->GetContextMenu( );

	// Otherwise, just use the standard multiple selection context menu
	else
		pContextMenu = new RPopupMenu( MENU_CONTEXT_MULTIPLE_SELECTION );

	// If we have a popup menu, track it
	if( pContextMenu )
		{
		// Save the point where the context menu was displayed
		m_ContextMenuPoint = mousePoint;

		// Change the cursor so that we always track with an arrow
		GetCursorManager( ).SetCursor( MacWinDos( &qd.arrow, IDC_ARROW, xxx ) );

		// Track the context menu
		pContextMenu->TrackPopupMenu( this, mousePoint );

		// Delete the menu
		delete pContextMenu;
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::GetContextMenu( )
//
//  Description:		Gets the context menu to use when this is the only view
//							selected
//
//  Returns:			Pointer to a context menu. The caller adoptes this pointer.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPopupMenu* RView::GetContextMenu( ) const
	{
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXActivate( )
//
//  Description:		Called when this view is being activated or deactivated
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnXActivate( BOOLEAN fActivating )
	{
	// Give the message to any activate component
	if( GetActiveComponent( ) )
		GetActiveComponent( )->OnXActivate( fActivating );

	// If we are deactivating, kill any tracking
	if( fActivating == FALSE )
		CancelTracking( );
	}

// ****************************************************************************
//
//  Function Name:	RView::CancelTracking( )
//
//  Description:		Cancels any active tracker
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::CancelTracking( )
	{
	// Pass on to any active component
	if( GetActiveComponent( ) )
		GetActiveComponent( )->CancelTracking( );

	m_fTrackSelection = FALSE;

	if( m_pActiveTracker )
		{
		// Cancel tracking
		m_pActiveTracker->CancelTracking( );

		// Delete the tracker
		delete m_pActiveTracker;
		m_pActiveTracker = NULL;

		ReleaseMouseCapture( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXActivateApp( )
//
//  Description:		Called when the app is being activated or deactivated
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnXActivateApp( BOOLEAN fActivating )
	{
	// Do the same thing was when the view deactivates
	RView::OnXActivate( fActivating );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXCharacter( )
//
//  Description:		Default handler for the Character message.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnXCharacter( const RCharacter& character, YModifierKey modifierKeys )
	{
	// If a component is active, route to that component
	if( m_pActiveComponent )
		{
		m_pActiveComponent->OnXCharacter( character, modifierKeys );
		return;
		}

	// Otherwise, we need to manipulate the selection on various keys
	switch( character )
		{
		case kTabCharacter :
			if( modifierKeys & kReverseTabOrderModifierKey )
				SelectPreviousComponent( );
			else
				SelectNextComponent( );
 			break;

		case kReturnCharacter:
			// If only one item in the selection
			if( GetSelection( )->Count( ) == 1)
				{
				RCompositeSelection::YIterator iterator = GetSelection( )->Start( );
				CallEditComponent( (*iterator), kEnterKey, RRealPoint( -1, -1 ) );
				}
			break;

		default :
			UpdateCursor( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXKeyDown( )
//
//  Description:		Key down handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnXKeyDown( EVirtualCharacter eChar, YModifierKey modifierKeys )
	{
	// If a component is active, route to that component
	if( m_pActiveComponent )
		{
		static_cast<RView*>( m_pActiveComponent )->OnXKeyDown( eChar, modifierKeys );
		return;
		}

	// Switch on the character
	switch( eChar )
		{
		//	Delete and backspace delete the current selection
		case kBackspaceCharacter :
		case kDeleteCharacter :
			OnDelete( );
			break;

		// Arrow keys nudge the current selection
		case kLeftArrowCharacter :
			NudgeSelection( kLeft, modifierKeys );
			break;

		case kUpArrowCharacter :
			NudgeSelection( kUp, modifierKeys );
			break;

		case kRightArrowCharacter :
			NudgeSelection( kRight, modifierKeys );
			break;

		case kDownArrowCharacter :
			NudgeSelection( kDown, modifierKeys );
			break;

		default :
			UpdateCursor( );
		};
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXKeyUp( )
//
//  Description:		Called when a key goes up
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnXKeyUp( EVirtualCharacter /* eChar */, YModifierKey /* modifierKeys */ )
	{
	UpdateCursor( );
	}

// ****************************************************************************
//
//  Function Name:	RView::NudgeSelection( )
//
//  Description:		Nudges the selection in the specified direction
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::NudgeSelection( EDirection nudgeDirection, YModifierKey modifierKeys )
	{
	// If there are no movable components selected, we can't nudge
	BOOLEAN fMovableComponentFound = FALSE;

	YSelectionIterator iterator = GetSelection( )->Start( );
	for( ; iterator != GetSelection( )->End( ); ++iterator )
		{
		if( ( *iterator )->GetComponentAttributes( ).IsMovable( ) )
			{
			fMovableComponentFound = TRUE;
			break;
			}
		}

	if( fMovableComponentFound == FALSE )
		return;

	// Nudge variables
	RRealSize nudgeAmount( 0, 0 );
	RRealPoint autoscrollPoint;
	RRealRect selectionBoundingRect = GetSelection( )->GetBoundingRectIncludingHandles( );

	// Switch on the direction
	switch( nudgeDirection )
		{
		case kLeft :
			nudgeAmount.m_dx = -kNudgeAmount;
			autoscrollPoint = selectionBoundingRect.m_TopLeft;
			break;

		case kUp :
			nudgeAmount.m_dy = -kNudgeAmount;
			autoscrollPoint = selectionBoundingRect.m_TopLeft;
			break;

		case kRight :
			nudgeAmount.m_dx = kNudgeAmount;
			autoscrollPoint = selectionBoundingRect.m_BottomRight;
			break;

		case kDown :
			nudgeAmount.m_dy = kNudgeAmount;
			autoscrollPoint = selectionBoundingRect.m_BottomRight;
			break;
		}

	// If we dont already have a nudge action, or if the last action was not a nudge, create and do one
	if( m_pNudgeAction == NULL || GetRDocument( )->GetNextActionId( ) != RNudgeSelectionAction::m_ActionId )
		{
		m_pNudgeAction = new RNudgeSelectionAction( GetSelection( ) );
		GetRDocument( )->SendAction( m_pNudgeAction );
		}

	// If the appropriate modifier key is down, scale up the nudge
	if( modifierKeys & kLargeNudgeModifierKey )
		nudgeAmount.Scale( RRealSize( kLargeNudgeScaleFactor, kLargeNudgeScaleFactor ) );

	// Nudge the selection
	m_pNudgeAction->Nudge( nudgeAmount );

	// Autoscroll
	AutoScroll( autoscrollPoint, Max( Abs( nudgeAmount.m_dx ), Abs( nudgeAmount.m_dy ) ), TRUE, nudgeDirection );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnChangeSelection( )
//
//  Description:		Called when the selection changes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnChangeSelection( )
	{
	// NULL out the nudge action
	m_pNudgeAction = NULL;
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXSetCursor( )
//
//  Description:		Default handler for the SetCursor XEvent.
//
//							Will set the cursor to be the generic arrow cursor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnXSetCursor( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Dont set the cursor if there is an active tracker
	if( !m_pActiveTracker )
		{
		BOOLEAN	fSet = FALSE;
		// Check to see whether the selection would handle a click at this point. If so,
		// let it handle the cursor
		if( !m_fInitialSelection && ShouldSelectionHandleClick( mousePoint, modifierKeys ) )
			fSet = GetSelection( )->SetCursor( mousePoint );
		if ( !fSet )
			GetCursorManager( ).SetCursor( MacWinDos( &qd.arrow, IDC_ARROW, xxx ) );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::GiveClickToSelection( )
//
//  Description:		Determines if a click at the specified point would be
//							handled by the selection or not.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::ShouldSelectionHandleClick( const RRealPoint& point, YModifierKey modifierKeys ) const
	{
	// Find out where we hit the selection
	RSelection::EHitLocation eHitLocation = GetSelection( )->HitTest( point );

	// If we hit a rotate or resize handle, always let the selection handle the click
	if( eHitLocation != RSelection::kNothing && eHitLocation != RSelection::kInside )
		return TRUE;

	// Otherwise only let the selection handle the click if the hit object is selected
	else
		{
		// Get the hit component view
		RComponentView* pHitComponent = GetHitComponentView( point );

		// If the hit component will single click activate, we dont handle it
		if( pHitComponent && pHitComponent->CanSingleClickActivate( pHitComponent->ConvertPointToLocalCoordinateSystem( point ) ) )
			return FALSE;

		// If the hit component is selected, then the selection gets the click, unless we
		// are toggle selecting.
		if( GetSelection( )->IsSelected( pHitComponent ) )
			return static_cast<BOOLEAN>( !( modifierKeys & kToggleSelectionModifierKey ) );

		else
			{
			// Get the hit selection object
			RSingleSelection* pHitSingleSelection = GetSelection( )->GetHitSingleSelection( point );

			// If we hit an object in the selection, and it has higher z-order then the hit component,
			// let the selection handle the click, unless we are toggle selecting
			if( pHitSingleSelection && ( !pHitComponent || pHitSingleSelection->IsHigherZOrder( GetComponentZPosition( pHitComponent ) ) ) )
				  return static_cast<BOOLEAN>( !( modifierKeys & kToggleSelectionModifierKey ) );

			// Otherwise dont give the click to the selection
			else
				return FALSE;
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::SetClipRegion( )
//
//  Description:		Creates a new clip region that is the intersection of the
//							current clip region and a region that will clip all output
//							to the bounds of this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::SetClipRegion( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const
	{
	// Intersect our useable area
	drawingSurface.IntersectClipRect( GetUseableArea( !kInsetFrame ), transform );
	}

// ****************************************************************************
//
//  Function Name:	RView::GetViewTransform( )
//
//  Description:		Gets the transform associated with this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::GetViewTransform( R2dTransform& transform, RDrawingSurface& drawingSurface, BOOLEAN fApplyParentsTransform ) const
	{
	// Scale to the device DPI
	RRealSize deviceDPI;

	if( drawingSurface.IsPrinting( ) )
		deviceDPI = drawingSurface.GetDPI( );
	else
		deviceDPI = GetDPI( );

	transform.PreScale( (YFloatType)deviceDPI.m_dx / kSystemDPI, (YFloatType)deviceDPI.m_dy / kSystemDPI );

	// Apply the views transforms
	ApplyTransform( transform, fApplyParentsTransform, drawingSurface.IsPrinting( ) );
	}

// ****************************************************************************
//
//  Function Name:	RView::GetViewTransform( )
//
//  Description:		Gets the transform associated with this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::GetViewTransform( R2dTransform& transform, BOOLEAN fApplyParentsTransform ) const
	{
	// Scale to the screen DPI
	RRealSize deviceDPI = GetDPI( );
	transform.PreScale( (YFloatType)deviceDPI.m_dx / kSystemDPI, (YFloatType)deviceDPI.m_dy / kSystemDPI );

	// Apply the views transforms
	ApplyTransform( transform, fApplyParentsTransform, FALSE );
	}

// ****************************************************************************
//
//  Function Name:	RView::ApplyTransform( )
//
//  Description:		Applys this views operations to the given transform. Call
//							the base method first when overriding!!
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::ApplyTransform( R2dTransform& /* transform */, BOOLEAN /* fApplyParentsTransform */, BOOLEAN /* fIsPrinting */ ) const
	{
	;
	}

#ifndef TPSDEBUG
// Turn off global optimizations to fix tiling offset problem (At least for Leapfrog)
#pragma optimize( "g", off )
#endif
// ****************************************************************************
//
//  Function Name:	RView::Render( )
//
//  Description:		Render this view.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
	{
	RenderBackground( drawingSurface, transform, rcRender );
	RenderDocument( drawingSurface, transform, rcRender );
	RenderComponents( drawingSurface, transform, rcRender );
	RenderActiveTracker( drawingSurface, transform );
	RenderActiveComponent( drawingSurface, transform, rcRender );
	}
#ifndef TPSDEBUG
// Turn global optimizations back on now that we are beyond the trouble point.
#pragma optimize( "g", on )
#endif

// ****************************************************************************
//
//  Function Name:	RView::RenderDocument( )
//
//  Description:		Render this view's document.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RenderDocument( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
	{
	drawingSurface.SetForegroundMode( kNormal );
	
	GetRDocument( )->Render( drawingSurface, transform, rcRender, GetReferenceSize( ) );
	}

// ****************************************************************************
//
//  Function Name:	RView::RenderShadow( )
//
//  Description:		Renders this view's shadow
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RenderShadow( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, const RSolidColor& shadowColor ) const
	{
	GetRDocument( )->RenderShadow( drawingSurface, transform, rcRender, rcRender.WidthHeight( ), shadowColor );
	}

// ****************************************************************************
//
//  Function Name:	RView::RenderSelection( )
//
//  Description:		Render this view's selection.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RenderSelection( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
	{
	// Only render the selection if we are not printing
	if( !drawingSurface.IsPrinting( ) )
		GetSelection( )->Render( drawingSurface, transform, rcRender );
	}

// ****************************************************************************
//
//  Function Name:	RView::RenderActiveTracker( )
//
//  Description:		Render the currently active tracker
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RenderActiveTracker( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const
	{
	// Only render trackers if we are not printing
	if( !drawingSurface.IsPrinting( ) )
		{
		// If there is a tracker active, render it
		if( m_pActiveTracker )
			m_pActiveTracker->Render( drawingSurface, transform );

		// If we have a drag drop feedback renderer, render feedback
		if( m_pFeedbackRenderer )
			m_pFeedbackRenderer->Render( drawingSurface, transform );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::RenderComponents( )
//
//  Description:		Render this view's components
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RenderComponents( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, RPrinterStatus* pPrinterStatus ) const
	{
	// Iterate the components, rendering them
	YComponentViewIterator iterator = m_ComponentViewCollection.Start( );
	for( ; iterator != m_ComponentViewCollection.End( ); ++iterator )
		{
		RCursor( ).SpinCursor( );
		// Don't render the active component unless we are printing; it will be rendered later
		if( ( m_pActiveComponent != *iterator ) || drawingSurface.IsPrinting( ) )
			{
			// Render the component
			RenderComponentWithSoftEffects( *iterator, drawingSurface, transform, rcRender );

			// If we have a print status object, rendering a component counts as an element.
			if( pPrinterStatus )
				pPrinterStatus->PrintElement( );
			}

		//	Stop if printing is canceled.
		if ( drawingSurface.IsPrintingCanceled( ) )
			break;
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::RenderActiveComponent( )
//
//  Description:		Render the currently active Component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RenderActiveComponent( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
	{
	if ( m_pActiveComponent && !drawingSurface.IsPrinting() )
		{
		//	Render the component
		if ( m_pActiveComponent->IsRenderActiveFrame( ) )
			RenderActiveFrame( drawingSurface, transform, rcRender );
		RenderComponent( m_pActiveComponent, drawingSurface, transform, rcRender );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::RenderActiveFrame( )
//
//  Description:		Render an 'Active Frame' around the currently active component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RenderActiveFrame( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& /* rcRender */ ) const
{
	TpsAssert( m_pActiveComponent, "RenderActiveFrame called when no component is active.... Boom!" );
	//	Render a frame around the component
	RRealVectorRect		viewRect( m_pActiveComponent->GetBoundingRect() );
	YAngle				rotation;
	YRealDimension		xScale;
	YRealDimension		yScale;
	transform.Decompose( rotation, xScale, yScale );
	RRealSize			frameWidth( (YRealDimension)kActiveFrameWidth / xScale, (YRealDimension)kActiveFrameWidth / xScale );

	//	Setup drawing state to be normal
	drawingSurface.SetForegroundMode( kNormal );
	drawingSurface.SetPenStyle( kSolidPen );

	//	Calculate the face rect and the outter rectangle
	RRealVectorRect		faceRect	= viewRect;
	RRealVectorRect		outRect	= viewRect;
	faceRect.Inflate( RRealSize( frameWidth.m_dx / 2.0, frameWidth.m_dy / 2.0 ) );
	outRect.Inflate( frameWidth );
	//
	//	Transform all rects to drawingSurface coordinates
	viewRect	*= transform;
	faceRect	*= transform;
	outRect	*= transform;

	//	Draw the face of the bounding rectangle
	drawingSurface.SetPenColor( RColor( MacWinDos( kBlack, ::GetSysColor(COLOR_3DFACE), xxx ) ) );
	drawingSurface.SetPenWidth( kActiveFrameWidth );
	drawingSurface.FrameVectorRect( faceRect, R2dTransform( ) );
	drawingSurface.SetPenWidth( 1 );

	//	Draw the light edges of the frame
	drawingSurface.SetPenColor( RColor( MacWinDos( kBlack, ::GetSysColor(COLOR_3DHILIGHT), xxx ) ) );
	drawingSurface.MoveTo( RRealPoint( outRect.m_BottomLeft.m_x, outRect.m_BottomLeft.m_y - 1 ) );
	drawingSurface.LineTo( outRect.m_TopLeft );
	drawingSurface.LineTo( outRect.m_TopRight );
	drawingSurface.MoveTo( RRealPoint( viewRect.m_TopRight.m_x, viewRect.m_TopRight.m_y - 1 ) );
	drawingSurface.LineTo( viewRect.m_BottomRight );
	drawingSurface.LineTo( RRealPoint( viewRect.m_BottomLeft.m_x - 1, viewRect.m_BottomLeft.m_y ) );

	//	Draw the dark edges of the frame
	drawingSurface.SetPenColor( RColor( MacWinDos( kBlack, ::GetSysColor(COLOR_3DSHADOW), xxx ) ) );
	drawingSurface.MoveTo( RRealPoint( viewRect.m_BottomLeft.m_x - 1, viewRect.m_BottomLeft.m_y ) );
	drawingSurface.LineTo( RRealPoint( viewRect.m_TopLeft.m_x - 1, viewRect.m_TopLeft.m_y - 1 ) );
	drawingSurface.LineTo( RRealPoint( viewRect.m_TopRight.m_x + 1, viewRect.m_TopRight.m_y - 1 ) );
	drawingSurface.MoveTo( RRealPoint( outRect.m_TopRight.m_x - 1, outRect.m_TopRight.m_y + 1 ) );
	drawingSurface.LineTo( RRealPoint( outRect.m_BottomRight.m_x - 1, outRect.m_BottomRight.m_y - 1 ) );
	drawingSurface.LineTo( RRealPoint( outRect.m_BottomLeft.m_x, outRect.m_BottomLeft.m_y - 1 ) );
}

// ****************************************************************************
//
//  Function Name:	RView::RenderComponentWithSoftEffects( )
//
//  Description:		Render this view's component with soft effects, if any
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RenderComponentWithSoftEffects( RComponentView* pView, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, BOOLEAN renderBehind /* = FALSE */ ) const
{
	// determine transformed component bounding rect
	RRealVectorRect tempBounds( pView->GetBoundingRect() );
	tempBounds *= transform;
	RRealRect transformedBounds( tempBounds.m_TransformedBoundingRect );

	if (!NeedsOffscreenDrawingEnvironment( pView ))
	{
		// the component does not have soft effects and
		// the component is not a text block with overlapping objects with soft effects

		// don't bother if we are clipped out
		if (rcRender.IsIntersecting( RIntRect( transformedBounds ) ))
			RenderComponent( pView, drawingSurface, transform, rcRender );
	}
	else
	{
		// the component has soft effects or
		// the component is a text block with overlapping objects with soft effects

		if (renderBehind)
		{
			// GCB 8/25/98 performance improvement:
			// We're being asked to render this component behind another one.
			// This means that components behind *this* one have already been
			// rendered into drawingSurface so we don't need to create another
			// offscreen drawing surface or render surface adornments or
			// components behind this one again

			RenderComponent( pView, drawingSurface, transform, rcRender );
		}
		else
		{
			// We're being asked to do a "normal" render with soft effect, i.e.,
			// we're not being asked to render this component behind another one

			// determine transformed bounding rect
			// adjusted for soft effects, if any, falling outside bounds
			RRealSize softEffectOffsetSize;
			RRealRect transformedFullEffectBounds( transformedBounds );
			pView->AdjustRectForSoftEffect( transformedFullEffectBounds, softEffectOffsetSize );

			// don't bother if we are clipped out
			if (rcRender.IsIntersecting( RIntRect( transformedFullEffectBounds ) ))
			{
				ROffscreenDrawingSurface*	offscreenDS = NULL;
				RBitmapImage*					bitmapImage = NULL;

				try
				{
					RRealRect rcRenderFullEffectBounds;
					RRealSize softEffectOrVignetteOffsetSize;

					transformedFullEffectBounds = transformedBounds;
					pView->AdjustRectForSoftEffectOrVignette( transformedFullEffectBounds, softEffectOrVignetteOffsetSize );

					// GCB 8/25/98 performance improvement:
					// adjust bitmap rect outward to fully include any objects behind
					// this one that have soft effects and partially intersect
					RRealRect tmpRealRect( transformedFullEffectBounds );
					pView->AdjustRectForSoftEffectsBehind(
						m_ComponentViewCollection, pView, transform, tmpRealRect );
					softEffectOrVignetteOffsetSize.m_dx +=
						transformedFullEffectBounds.m_TopLeft.m_x - tmpRealRect.m_TopLeft.m_x;
					softEffectOrVignetteOffsetSize.m_dy +=
						transformedFullEffectBounds.m_TopLeft.m_y - tmpRealRect.m_TopLeft.m_y;
					transformedFullEffectBounds = tmpRealRect;

#define OPTIMIZE_SOFT_EFFECTS 1

#if ! OPTIMIZE_SOFT_EFFECTS
					rcRenderFullEffectBounds = transformedFullEffectBounds;
#else
					// Performance optimization: limit bitmap size to the intersection
					// of the full component with the rcRender visible/requested area
					// plus the area, if any, required for soft effects.
					// rcRender could be smaller than the full visible area (or full
					// printed project in the case of a banner or poster) which can
					// disrupt vignette (soft edge) rendering, so make sure the
					// offscreen bitmap is inflated outward enough to account for the
					// soft edge then display/print only the area within rcRender.
					rcRenderFullEffectBounds.Intersect( RRealRect( rcRender ), transformedFullEffectBounds );
					if (rcRenderFullEffectBounds.m_Left != transformedFullEffectBounds.m_Left)
						rcRenderFullEffectBounds.m_Left -= softEffectOrVignetteOffsetSize.m_dx;
					if (rcRenderFullEffectBounds.m_Top != transformedFullEffectBounds.m_Top)
						rcRenderFullEffectBounds.m_Top -= softEffectOrVignetteOffsetSize.m_dy;
					if (rcRenderFullEffectBounds.m_Right != transformedFullEffectBounds.m_Right)
						rcRenderFullEffectBounds.m_Right += softEffectOrVignetteOffsetSize.m_dx;
					if (rcRenderFullEffectBounds.m_Bottom != transformedFullEffectBounds.m_Bottom)
						rcRenderFullEffectBounds.m_Bottom += softEffectOrVignetteOffsetSize.m_dy;
#endif

					// quantize transformed rcRender full effect bounding rect
					// into final bitmap destination rect and determine position
					// quantization error to add back into transform
					RIntRect destRect( rcRenderFullEffectBounds );
					RRealSize positionQuantizationErrorSize(
						rcRenderFullEffectBounds.m_Left - destRect.m_Left,
						rcRenderFullEffectBounds.m_Top - destRect.m_Top );

					// transform for correctly positioning data into offscreen bitmap
					// and for limiting the printed resolution of the soft effect
					R2dTransform tempTransform( transform );

					// position the data correctly in the bitmap
					tempTransform.PostTranslate(
						-rcRenderFullEffectBounds.m_Left + positionQuantizationErrorSize.m_dx,
						-rcRenderFullEffectBounds.m_Top + positionQuantizationErrorSize.m_dy );

					// if we're printing limit the bitmap resolution to either
					// kLoResBitmapXDpi, kLoResBitmapYDpi or kHiResBitmapXDpi, kHiResBitmapYDpi
					// depending on user preference in registry by scaling transform smaller
					if (drawingSurface.IsPrinting())
					{
						YRealDimension bitmapXDpi = kLoResBitmapXDpi;
						YRealDimension bitmapYDpi = kLoResBitmapYDpi;
						if (m_fHiResPhoto)
						{
							bitmapXDpi = kHiResBitmapXDpi;
							bitmapYDpi = kHiResBitmapYDpi;
						}

						RRealSize rSize( rcRenderFullEffectBounds.WidthHeight() );
						RIntSize rPrinterDpi( drawingSurface.GetDPI() );
						RRealSize rSizeInInches( rSize.m_dx / rPrinterDpi.m_dx, rSize.m_dy / rPrinterDpi.m_dy );
						RRealSize rDpi( (YIntDimension)((YRealDimension)rcRenderFullEffectBounds.Width() / rSizeInInches.m_dx), (YIntDimension)((YRealDimension)rcRenderFullEffectBounds.Height() / rSizeInInches.m_dy) );
						if (rDpi.m_dx > bitmapXDpi)
							tempTransform.PostScale( (YFloatType)bitmapXDpi / (YFloatType)rDpi.m_dx, 1.0 );
						if (rDpi.m_dy > bitmapYDpi)
							tempTransform.PostScale( 1.0, (YFloatType)bitmapYDpi / (YFloatType)rDpi.m_dy );

						// Limiting resolution means bitmap size smaller than destRect.
						// Recalculate rcRenderFullEffectBounds (which determines the
						// bitmap size).
						tempBounds = pView->GetBoundingRect();
						tempBounds *= tempTransform;
						transformedBounds = tempBounds.m_TransformedBoundingRect;
						transformedFullEffectBounds = transformedBounds;
						pView->AdjustRectForSoftEffectOrVignette( transformedFullEffectBounds, softEffectOrVignetteOffsetSize );

						// GCB 8/25/98 performance improvement:
						// adjust bitmap rect outward to fully include any objects behind
						// this one that have soft effects and partially intersect
						RRealRect tmpRealRect( transformedFullEffectBounds );
						pView->AdjustRectForSoftEffectsBehind(
							m_ComponentViewCollection, pView, tempTransform, tmpRealRect );
						softEffectOrVignetteOffsetSize.m_dx +=
							transformedFullEffectBounds.m_TopLeft.m_x - tmpRealRect.m_TopLeft.m_x;
						softEffectOrVignetteOffsetSize.m_dy +=
							transformedFullEffectBounds.m_TopLeft.m_y - tmpRealRect.m_TopLeft.m_y;
						transformedFullEffectBounds = tmpRealRect;

#if ! OPTIMIZE_SOFT_EFFECTS
						rcRenderFullEffectBounds = transformedFullEffectBounds;
#else
						R2dTransform inverseTransform( transform );
						inverseTransform.Invert();
						RRealRect tempRealRcRender( rcRender );
						RRealVectorRect tempRealVectorRcRender( tempRealRcRender );
						tempRealVectorRcRender *= inverseTransform;
						tempRealVectorRcRender *= tempTransform;
						rcRenderFullEffectBounds.Intersect( tempRealVectorRcRender.m_TransformedBoundingRect, transformedFullEffectBounds );
						if (rcRenderFullEffectBounds.m_Left != transformedFullEffectBounds.m_Left)
							rcRenderFullEffectBounds.m_Left -= softEffectOrVignetteOffsetSize.m_dx;
						if (rcRenderFullEffectBounds.m_Top != transformedFullEffectBounds.m_Top)
							rcRenderFullEffectBounds.m_Top -= softEffectOrVignetteOffsetSize.m_dy;
						if (rcRenderFullEffectBounds.m_Right != transformedFullEffectBounds.m_Right)
							rcRenderFullEffectBounds.m_Right += softEffectOrVignetteOffsetSize.m_dx;
						if (rcRenderFullEffectBounds.m_Bottom != transformedFullEffectBounds.m_Bottom)
							rcRenderFullEffectBounds.m_Bottom += softEffectOrVignetteOffsetSize.m_dy;
#endif

						// adjust destRect for scaling quantization error
						RRealSize scaledRcRenderFullEffectSize( rcRenderFullEffectBounds.WidthHeight() );
						if (rDpi.m_dx > bitmapXDpi)
						{
							scaledRcRenderFullEffectSize.m_dx *= rDpi.m_dx / bitmapXDpi;
							destRect.m_Right = destRect.m_Left + ::Round( scaledRcRenderFullEffectSize.m_dx );
						}
						if (rDpi.m_dy > bitmapYDpi)
						{
							scaledRcRenderFullEffectSize.m_dy *= rDpi.m_dy / bitmapYDpi;
							destRect.m_Bottom = destRect.m_Top + ::Round( scaledRcRenderFullEffectSize.m_dy );
						}
					}

					// create 24-bit offscreenDS,bitmapImage and init to white
					RIntSize bitmapSize( rcRenderFullEffectBounds.WidthHeight() );
					RIntRect bitmapRect( bitmapSize );
					offscreenDS = new ROffscreenDrawingSurface( drawingSurface.IsPrinting(), TRUE, &drawingSurface );
					bitmapImage = new RBitmapImage;
					bitmapImage->Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 24 );
					offscreenDS->SetImage( bitmapImage );
					::memset( RBitmapImage::GetImageData( bitmapImage->GetRawData()), 0xFF, RBitmapImage::GetImageDataSize( bitmapImage->GetRawData() ) );

					// render any "surface adornments" such as desktop, panel surface,
					// background color/gradient and all components behind current
					// component into offscreenDS
					RenderSurfaceAdornments( pView, *offscreenDS, tempTransform, bitmapRect );
					RenderBehindComponentsWithSoftEffects( m_ComponentViewCollection, pView, *offscreenDS, tempTransform, bitmapRect );

					// render current component into offscreenDS
					RenderComponent( pView, *offscreenDS, tempTransform, bitmapRect );

					// if we're rendering to the screen make sure we only draw the
					// rcRender visible/requested area that needs updating
					RIntRect rcRenderDestRect;
					RIntRect rcRenderBitmapRect;
					if (drawingSurface.IsPrinting())
					{
						rcRenderDestRect = destRect;
						rcRenderBitmapRect = bitmapRect;
					}
					else
					{
						rcRenderDestRect.Intersect( rcRender, destRect );
						rcRenderBitmapRect = rcRenderDestRect;
						RIntSize offsetSize(
							-rcRenderBitmapRect.m_Left + (rcRenderDestRect.m_Left - destRect.m_Left),
							-rcRenderBitmapRect.m_Top + (rcRenderDestRect.m_Top - destRect.m_Top) );
						rcRenderBitmapRect.m_Left += offsetSize.m_dx;
						rcRenderBitmapRect.m_Right += offsetSize.m_dx;
						rcRenderBitmapRect.m_Top += offsetSize.m_dy;
						rcRenderBitmapRect.m_Bottom += offsetSize.m_dy;
					}

					// display offscreenDS to drawingSurface
					// if we're limiting print resolution this will scale from
					// rcRenderBitmapRect to rcRenderDestRect
					bitmapImage->RenderToScreenOrPrinter( *offscreenDS, drawingSurface, rcRenderBitmapRect, rcRenderDestRect );
				}
				catch (...)
				{
					TpsAssertAlways( "Couldn't render with soft effects." );
				}

				// clean up
				if (offscreenDS)
					offscreenDS->ReleaseImage( );
				delete offscreenDS;
				delete bitmapImage;
			}
		}
	}
}

// ****************************************************************************
//
//  Function Name:	BOOLEAN RView::NeedsOffscreenDrawingEnvironment(
//								RComponentView* pView ) const
//
//  Description:		Returns TRUE if pView has soft effects or is a text block
//							with overlapping objects that need an offscreen drawing
//							environment; otherwise returns FALSE.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::NeedsOffscreenDrawingEnvironment( RComponentView* pView ) const
{
	if (pView->HasSoftEffects())
		return TRUE;
	else
	{
		if (pView->GetComponentDocument()->GetComponentType() != kTextComponent)
			return FALSE;
		YComponentViewIterator iterator = m_ComponentViewCollection.Start();
		for (; iterator != m_ComponentViewCollection.End() && *iterator != pView; ++iterator)
			;
		if (iterator != m_ComponentViewCollection.End())
			++iterator;
		for (; iterator != m_ComponentViewCollection.End(); ++iterator)
			if (NeedsOffscreenDrawingEnvironment( *iterator ) &&
				(*iterator)->IntersectsFullSoftEffectsRect( pView->GetFullSoftEffectsBoundingRect().m_TransformedBoundingRect ))
				return TRUE;
		return FALSE;
	}
}

// ****************************************************************************
//
//  Function Name:	RView::EnableRenderCache( )
//
//  Description:		Enables/disables the render cache
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::EnableRenderCache( BOOLEAN /* fEnable */ )
	{
	// subclasses should override if they need to
	}

// ****************************************************************************
//
//  Function Name:	RView::RenderSurfaceAdornments( )
//
//  Description:		Render any "surface adornments" such as desktop, panel
//							surface, background color/gradient
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RenderSurfaceAdornments( RComponentView* /* pView */, RDrawingSurface& /* drawingSurface */, const R2dTransform& /* transform */, const RIntRect& /* rcRender */ ) const
{
	// subclasses should override if they need to
}

// ****************************************************************************
//
//  Function Name:	RView::RenderBehindComponentsWithSoftEffects( )
//
//  Description:		Render all components, if any, behind the current one
//							with soft effects
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RenderBehindComponentsWithSoftEffects( const YComponentViewCollection& fComponentViewCollection, RComponentView* pView, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
{
	// for each component view behind pView...
	YComponentViewIterator iterator = fComponentViewCollection.Start();
	for (; iterator != fComponentViewCollection.End() && *iterator != pView; ++iterator )
	{
		// render the *iterator component view, letting it know it's being asked
		// to render because we're rendering a "behind" component

		BOOLEAN renderBehind = TRUE;
		RComponentDocument* pThisComponentDocument = (*iterator)->GetComponentDocument();
		YComponentType thisComponentType = pThisComponentDocument->GetComponentType();
		if (thisComponentType == kGroupComponent)
			renderBehind = FALSE; // let group components do "normal" rendering

		RenderComponentWithSoftEffects( *iterator, drawingSurface, transform, rcRender, renderBehind );
	}
}

// ****************************************************************************
//
//  Function Name:	void AdjustRectForSoftEffectsBehind(
//								const YComponentViewCollection&	fComponentViewCollection, 
//								const RComponentView*				pView, 
//								const R2dTransform&					transform,
//								RRealRect&								rect ) const
//
//  Description:		If this component has objects behind it with soft effects
//							that render outside their bounds, adjust rect outward such
//							that the portion of their soft effects intersecting the
//							area behind this component can be properly rendered.
//
//							Useful for setting up an offscreen bitmap of the correct
//							size for components with soft effects that render outside
//							their bounds.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::AdjustRectForSoftEffectsBehind( const YComponentViewCollection& fComponentViewCollection, const RComponentView* pView, const R2dTransform& transform, RRealRect& rect ) const
{
	RRealRect adjustedRect( rect );

	// for each component view behind pView...
	YComponentViewIterator iterator = fComponentViewCollection.Start();
	for (; iterator != fComponentViewCollection.End() && *iterator != pView; ++iterator )
	{
		if ((*iterator)->HasSoftEffects())
		{
			RRealVectorRect fullEffectsRect( (*iterator)->GetFullSoftEffectsBoundingRect() );
			fullEffectsRect *= transform;
			if (fullEffectsRect.m_TransformedBoundingRect.IsIntersecting( rect ))
			{
				adjustedRect.m_Left = ::Min( fullEffectsRect.m_TransformedBoundingRect.m_Left, adjustedRect.m_Left );
				adjustedRect.m_Top = ::Min( fullEffectsRect.m_TransformedBoundingRect.m_Top, adjustedRect.m_Top );
				adjustedRect.m_Right = ::Max( fullEffectsRect.m_TransformedBoundingRect.m_Right, adjustedRect.m_Right );
				adjustedRect.m_Bottom = ::Max( fullEffectsRect.m_TransformedBoundingRect.m_Bottom, adjustedRect.m_Bottom );
			}
		}
	}

	rect = adjustedRect;
}

// ****************************************************************************
//
//  Function Name:	RView::RenderComponent( )
//
//  Description:		Render this components
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RenderComponent( RComponentView* pView, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
	{
	RDrawingSurfaceState	curState( &drawingSurface );
	// Save the clip rect
	//RIntRect oldClipRect = drawingSurface.GetClipRect( );

	// If we are clipping this component to its parent, do it
	if( pView->GetComponentAttributes( ).ClipToParent( ) )
		SetClipRegion( drawingSurface, transform );

	// Render the component if part of its view is visible
	RRealVectorRect	componentRect( pView->GetBoundingRect( ) );
	componentRect *= transform;
	if ( componentRect.m_TransformedBoundingRect.IsIntersecting( RRealRect( rcRender ) ) )
		{
		// Make a copy of the transform
		R2dTransform temp = transform;
		// Apply this views transform.
		pView->ApplyTransform( temp, FALSE, drawingSurface.IsPrinting( ) );
		//	Let the view render
		pView->Render( drawingSurface, temp, rcRender );
		}

	// Put the clip rect back
	//drawingSurface.SetClipRect( oldClipRect );
	}

// ****************************************************************************
//
//  Function Name:	RView::GetHitComponentView( )
//
//  Description:		Determines which component view contains the given point
//
//  Returns:			Pointer to the view which was hit, NULL if no component was hit.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RView::GetHitComponentView( const RRealPoint& point, BOOLEAN fCheckNonSelectable ) const
	{
	//	First check with all selected components to see if the selection has a hit.
	RCompositeSelection*	pSelection = GetSelection( );
	if ( pSelection && (pSelection->Count( ) > 0) )
		{
		YSelectionIterator iterator		= pSelection->Start( );
		YSelectionIterator iteratorEnd	= pSelection->End( );
		for( ; iterator != iteratorEnd; ++iterator )
			{
			if( ( *iterator )->HitTest(point) )
				return ( *iterator );
			}
		}

	// Go through the component view list and call HitTest on each one that is selectable.
	if( m_ComponentViewCollection.Count( ) )
		{
		YComponentViewIterator iterator = m_ComponentViewCollection.End( );
		do
			{
			RComponentView* pView = *( --iterator );
			TpsAssertValid( pView );
			if( ( pView->GetComponentAttributes( ).IsSelectable( ) || fCheckNonSelectable ) && pView->HitTest(point) )
				return pView;
			}
		while( iterator != m_ComponentViewCollection.Start( ) );
		}

	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RView::ConvertPointToLocalCoordinateSystem( )
//
//  Description:		Converts a point in our parents coordinate system to one
//							in our coordinate system
//
//  Returns:			The converted point
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealPoint RView::ConvertPointToLocalCoordinateSystem( const RRealPoint& point ) const
	{
	R2dTransform transform;
	ApplyTransform( transform, FALSE, FALSE );
	transform.Invert( );
	return point * transform;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetInsertionPoint( )
//
//  Description:		Get the current paste point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealPoint RView::GetInsertionPoint( const RRealSize& size )
	{
	const YRealDimension kInsertionOffset = ::InchesToLogicalUnits( 0.1 );

	// Array of actions that will not result in the insertion point being reset
	static const YActionId dontResetActions[ ] =
		{
		RPasteAction::m_ActionId,
		RNewComponentAction::m_ActionId,
		RDuplicateSelectionAction::m_ActionId
		};

	static const YActionId* pDontResetActionsEnd = dontResetActions + NumElements( dontResetActions );

	// Since the window has not actually been created in its ctor or initializer, we just set the
	// insertion point to something invalid. Check for that now and reset if necessary.
	if( m_InsertionPoint.m_x == kInvalidInsertionPoint && m_InsertionPoint.m_y == kInvalidInsertionPoint )
		{
		ResetInsertionPoint( );
		return m_InsertionPoint;
		}

	// Get the rectangle of the view which is on screen
	RRealRect viewableRect = GetViewableArea( );

	// If the component bounding rect is larger than the viewable rect in any
	// dimension, punt and just use the center
	if( size.m_dx > viewableRect.Width( ) || size.m_dy > viewableRect.Height( ) )
		{
		ResetInsertionPoint( );
		return m_InsertionPoint;
		}

	if( ::FindMatch( &dontResetActions[ 0 ], pDontResetActionsEnd, GetRDocument( )->GetNextActionId( ) ) != pDontResetActionsEnd ) 
		{
		// If this would not put the component off the screen, return it
		RRealPoint newInsertionPoint( m_InsertionPoint.m_x + kInsertionOffset, m_InsertionPoint.m_y + kInsertionOffset );
		if( newInsertionPoint.m_x + ( size.m_dx / 2 ) < viewableRect.m_Right &&
			 newInsertionPoint.m_y + ( size.m_dy / 2 ) < viewableRect.m_Bottom )
			{
			m_InsertionPoint = newInsertionPoint;
			return m_InsertionPoint;
			}

		// The component would be put off the screen, so reset back up to the top
		RRealPoint tempPoint = m_InsertionPoint;
		m_InsertionPoint.m_y = viewableRect.m_Top + ( viewableRect.m_Bottom - tempPoint.m_y );
		m_InsertionPoint.m_x = tempPoint.m_x - ( tempPoint.m_y - m_InsertionPoint.m_y );

		// Offset down a bit
		m_InsertionPoint.m_y += kInsertionOffset;

		// Now check to see if we are on the screen
		if( m_InsertionPoint.m_y + ( size.m_dx / 2 ) < viewableRect.m_Bottom &&
			 m_InsertionPoint.m_x - ( size.m_dy / 2 ) > viewableRect.m_Left )
			return m_InsertionPoint;

		// If not, reset back to the center
		else
			{
			ResetInsertionPoint( );
			return m_InsertionPoint;
			}
		}

	else
		{
		ResetInsertionPoint( );
		return m_InsertionPoint;
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::ResetInsertionPoint( )
//
//  Description:		Resets the current insertion point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::ResetInsertionPoint( )
	{
	// Get the rectangle of the view which is on screen
	RRealRect viewableRect = GetViewableArea( );

	// Use center of the area as the initial insertion point
	m_InsertionPoint =  viewableRect.GetCenterPoint( );
	}

// ****************************************************************************
//
//  Function Name:	RView::CreateScriptAction( )
//
//  Description:		Try to create an action from the script
//
//  Returns:			The pointer to the action or NULL
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAction* RView::CreateScriptAction( const YActionId& , RScript& )
	{
	RAction*	pAction = NULL;
	return pAction;
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXSetKeyFocus( )
//
//  Description:		Notification that the KeyFocus was just set to this view.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnXSetKeyFocus( )
	{
	// Route to the active component
	if( m_pActiveComponent )
		m_pActiveComponent->OnXSetKeyFocus( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXKillKeyFocus( )
//
//  Description:		Notification that the KeyFocus was just set to this view.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnXKillKeyFocus( )
	{
	//	Redirect the key focus to any active components.
	if (m_pActiveComponent)
		m_pActiveComponent->OnXKillKeyFocus( );
	}

// ****************************************************************************
//
//  Function Name:	RView::XUpdateAllViews( )
//
//  Description:		Tell All views (components) that the give UpdateType occured
//							and for them to do whatever they deem necessary
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::XUpdateAllViews( EUpdateTypes updateType, uLONG lParam )
	{
	switch( updateType )
		{
			case kLayoutChanged :
			case kViewZoomChanged :
				{
				YComponentViewIterator	iter		= m_ComponentViewCollection.Start();
				YComponentViewIterator	iterEnd	= m_ComponentViewCollection.End();
				for ( ; iter != iterEnd; ++iter )
					{
					//	Let any components in this view update themselves
					(*iter)->XUpdateAllViews( updateType, lParam );
					//	Update the component
					(*iter)->OnXUpdateView( updateType, lParam );
					}
				}
				break;

			default:
				break;
		}

	}

// ****************************************************************************
//
//  Function Name:	RView::OnXUpdateView( )
//
//  Description:		Retrieves the current mouse position, and sends an
//							OnXSetCursor with those coordinates. Used to update the 
//							cursor when something other than a mouse event may have
//							changed it.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::OnXUpdateView( EUpdateTypes updateType, uLONG /* lParam */ )
	{
	switch( updateType )
		{
			case kLayoutChanged :
				//	Do Nothing in this case.
				break;

			default:
				break;
		}
	}


// ****************************************************************************
//
//  Function Name:	RView::UpdateCursor( )
//
//  Description:		Retrieves the current mouse position, and sends an
//							OnXSetCursor with those coordinates. Used to update the 
//							cursor when something other than a mouse event may have
//							changed it.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RView::UpdateCursor( )
	{
	OnXSetCursor( GetCursorPosition( ), GetModifierKeyState( ) );
	}

// ****************************************************************************
//
//  Function Name:	RView::GetZOrderState( )
//
//  Description:		Accessor
//
//  Returns:			Z-Order of the components embedded in this view
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YZOrderState& RView::GetZOrderState( ) const
	{
	return m_ComponentViewCollection;
	}

// ****************************************************************************
//
//  Function Name:	RView::SetZOrderState( )
//
//  Description:		Accessor
//
//  Returns:			Z-Order of the components embedded in this view
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::SetZOrderState( const YZOrderState& zOrder )
	{
	m_ComponentViewCollection = zOrder;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetComponentZPosition( )
//
//  Description:		Accessor
//
//  Returns:			The z-order of the specified component
//
//  Exceptions:		None
//
// ****************************************************************************
//
YComponentZPosition RView::GetComponentZPosition( const RComponentView* pView ) const
	{
	return ::GetIndex( m_ComponentViewCollection, pView );
	}

// ****************************************************************************
//
//  Function Name:	RView::SetComponentZPosition( )
//
//  Description:		Sets the z-order of the specified component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::SetComponentZPosition( RComponentView* pView, YComponentZPosition z )
	{
	TpsAssert( z < m_ComponentViewCollection.Count( ), "Invalid z-order" );

	// If it is the only component, dont bother
	if( m_ComponentViewCollection.Count( ) == 1 )
		return;

	// Remove the component
	m_ComponentViewCollection.Remove( pView );

	// Reinsert it in the correct position
	YComponentViewIterator iterator = m_ComponentViewCollection.Start( ) + z;
	m_ComponentViewCollection.InsertAt( iterator, pView );
	}

// ****************************************************************************
//
//  Function Name:	RView::SelectRectangle( )
//
//  Description:		Selects any components which intersect the specified rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::SelectRectangle( const RRealRect& rect )
	{
	// Select anything that is within the rectangle
	YComponentViewIterator viewIterator = m_ComponentViewCollection.Start( );
	for( ; viewIterator != m_ComponentViewCollection.End( ); ++viewIterator )
		if( ( *viewIterator )->IsInRect( rect ) )
			if( !GetSelection( )->IsSelected( *viewIterator ) )
				GetSelection( )->Select( *viewIterator, TRUE );

	// Now unselect anything that isnt in the rectangle. Doing this as a two step
	// process is a little inefficent, but it eliminates some flicker
	RCompositeSelection::YIterator selectionIterator = GetSelection( )->Start( );
	while( selectionIterator != GetSelection( )->End( ) )
		{
		if( !( *selectionIterator )->IsInRect( rect ) )
			{
			// Make a copy of the iterator, as we are about to delete from the selection
			RCompositeSelection::YIterator temp = selectionIterator;
			++temp;
			GetSelection( )->Unselect( *selectionIterator );
			selectionIterator = temp;
			}
		else
			++selectionIterator;
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::ToggleSelectRectangle( )
//
//  Description:		Selects any components which intersect the specified rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::ToggleSelectRectangle( const RRealRect& rect )
	{
	// Toggle Select anything that is within the rectangle
	YComponentViewIterator viewIterator = m_ComponentViewCollection.Start( );
	for( ; viewIterator != m_ComponentViewCollection.End( ); ++viewIterator )
		if( ( *viewIterator )->IsInRect( rect ) )
			GetSelection( )->ToggleSelect( *viewIterator );
	}

// ****************************************************************************
//
//  Function Name:	RView::SelectNextComponent( )
//
//  Description:		Selects the next component in the z-order. Called when the
//							tab key is pressed.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::SelectNextComponent( )
	{
	YComponentZPosition highestZPosition = -1;
	RComponentView* pLastSelectedView = NULL;

	// Iterate the selection, looking for the highest component in the z-order
	YSelectionIterator selectionIterator = GetSelection( )->Start( );

	for( ; selectionIterator != GetSelection( )->End( ); ++selectionIterator )
		{
		YComponentZPosition componentZPosition = GetComponentZPosition( *selectionIterator );
		if( componentZPosition > highestZPosition )
			{
			highestZPosition = componentZPosition;
			pLastSelectedView = *selectionIterator;
			}
		}

	// OK, now we have the last selected component. Get an iterator at its position
	YComponentViewIterator componentIterator;

	if( pLastSelectedView )
		{
		componentIterator = m_ComponentViewCollection.Find( pLastSelectedView );
		TpsAssert( componentIterator != m_ComponentViewCollection.End( ), "Why wasnt this view found?" );
		++componentIterator;
		}
	else
		componentIterator = m_ComponentViewCollection.Start( );

	// Now go find the next selectable object
	RComponentView* pNextComponent = NULL;

	for( ; componentIterator != m_ComponentViewCollection.End( ); ++componentIterator )
		{
		if( ( *componentIterator )->GetComponentAttributes( ).IsSelectable( ) )
			{
			pNextComponent = *componentIterator;
			break;
			}
		}

	// Unselect all components
	GetSelection( )->UnselectAll( );

	// Select the component we found
	if( pNextComponent )
		GetSelection( )->Select( pNextComponent, TRUE );
	}

// ****************************************************************************
//
//  Function Name:	RView::SelectPreviousComponent( )
//
//  Description:		Selects the previous component in the z-order. Called when 
//							shift-tab is pressed.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::SelectPreviousComponent( )
	{
	YComponentZPosition lowestZPosition = INT_MAX;
	RComponentView* pFirstSelectedView = NULL;

	// Iterate the selection, looking for the lowest component in the z-order
	YSelectionIterator selectionIterator = GetSelection( )->Start( );

	for( ; selectionIterator != GetSelection( )->End( ); ++selectionIterator )
		{
		YComponentZPosition componentZPosition = GetComponentZPosition( *selectionIterator );
		if( componentZPosition < lowestZPosition )
			{
			lowestZPosition = componentZPosition;
			pFirstSelectedView = *selectionIterator;
			}
		}

	// OK, now we have the first selected component. Get an iterator at its position
	YComponentViewIterator componentIterator;

	if( pFirstSelectedView )
		{
		componentIterator = m_ComponentViewCollection.Find( pFirstSelectedView );
		TpsAssert( componentIterator != m_ComponentViewCollection.End( ), "Why wasnt this view found?" );
		}
	else
		componentIterator = m_ComponentViewCollection.End( );

	// Now go find the previous selectable object
	RComponentView* pPreviousComponent = NULL;

	if( componentIterator != m_ComponentViewCollection.Start( ) )
		{
		do
			{
			--componentIterator;

			if( ( *componentIterator )->GetComponentAttributes( ).IsSelectable( ) )
				{
				pPreviousComponent = *componentIterator;
				break;
				}
			}
			while( componentIterator != m_ComponentViewCollection.Start( ) );
		}

	// Unselect all components
	GetSelection( )->UnselectAll( );

	// Select the component we found
	if( pPreviousComponent )
		GetSelection( )->Select( pPreviousComponent, TRUE );
	}

// ****************************************************************************
//
//  Function Name:	RView::Write( )
//
//  Description:	Writes this views data to the specified storage
//
//  Returns:		Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RView::Write( RChunkStorage& storage ) const
	{
	//	Put the chunk's data in a chunk.
	storage.AddChunk( kViewTag );

	//	Save the shadow on/off state.
	// (PS 6.0 and later extended soft shadow settings written below)
	storage << static_cast<uLONG>( HasOldStyleShadow() );

	// Save the z order state of the component list
	storage << static_cast<uLONG>( m_ComponentViewCollection.Count( ) );

	RDocument* pDocument = GetRDocument( );

	YComponentViewIterator iterator = m_ComponentViewCollection.Start( );
	for( ; iterator != m_ComponentViewCollection.End( ); ++iterator )
		storage << static_cast<uLONG>( pDocument->GetComponentIndex( static_cast<RComponentDocument*>( ( *iterator )->GetRDocument( ) ) ) );

	// PS 6.0 and later: write an "extended" soft shadow settings chunk
	// ignored by Print Shop 5.x since it's at the end of the kViewTag chunk
	storage.AddChunk( kSoftShadowTag );
	storage << *m_pShadowSettings;
	storage.SelectParentChunk( );
	//
	// PS 6.0 and later: write a soft glow settings chunk
	// ignored by Print Shop 5.x since it's at the end of the kViewTag chunk
	storage.AddChunk( kSoftGlowTag );
	storage << *m_pGlowSettings;
	storage.SelectParentChunk( );
	//
	// PS 6.0 and later: write a soft vignette settings chunk
	// ignored by Print Shop 5.x since it's at the end of the kViewTag chunk
	storage.AddChunk( kSoftVignetteTag );
	storage << *m_pVignetteSettings;
	storage.SelectParentChunk( );
	//
	// PS 6.0 and later: read the Alpha Mask chunk
	// Only write the chunk if the view has a valid alpha mask set
	// 
	if (m_pAlphaMask != NULL && m_dwMaskID != 0)
	{
		storage.AddChunk( kAlphaMaskTag );
		storage << m_dwMaskID;
		m_pAlphaMask->Write(storage);
		storage.SelectParentChunk( );

		DWORD dwMoreData = 0;
		storage.AddChunk( kAlphaMaskInfoTag );
		storage << (DWORD) m_kSpecialEdgeType;
		storage << (DWORD) dwMoreData;
		storage.SelectParentChunk();
	}
	// PS 6.0 and later: write an edge outline settings chunk
	// ignored by Print Shop 5.x since it's at the end of the kViewTag chunk
	storage.AddChunk( kEdgeOutlineTag );
	storage << *m_pEdgeOutlineSettings;
	storage.SelectParentChunk( );

	//	Go back to writing at the prior level.
	storage.SelectParentChunk( );
	}

// ****************************************************************************
//
//  Function Name:	RView::Read( )
//
//  Description:	Reads from the specified storage
//
//  Returns:		Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RView::Read( RChunkStorage& storage )
	{
	// Read the view's data. 
	//	Save the current chunk so we may return to it below.
	//	Select the view's data chunk.
	YChunkStorageId yCurrentChunk = storage.GetSelectedChunk( );
	RChunkStorage::RChunkSearcher searcher = storage.Start( kViewTag, FALSE );

	// Restore the shadow on/off state
	// (PS 6.0 and later extended soft shadow settings read below)
	uLONG ulTemp;
	storage >> ulTemp;
	m_pShadowSettings->m_fShadowOn = static_cast<BOOLEAN>( ulTemp );

	// We now need to restore the z order state. First make sure the view collection is empty
	m_ComponentViewCollection.Empty( );

	// Get our document
	RDocument* pDocument = GetRDocument( );

	// Get the number of component views embedded in us
	uLONG numViews;
	storage >> numViews;
	
	// Read the component indices
	for( uLONG i = 0; i < numViews; i++ )
		{
		// Read the component index
		uLONG componentIndex;
		storage >> componentIndex;

		// Get the component from the index
		RComponentDocument* pComponentDocument = pDocument->GetComponentByIndex( componentIndex );

		// If that component exits, get its component view and insert it
		if( pComponentDocument )
			m_ComponentViewCollection.InsertAtEnd( static_cast<RComponentView*>( pComponentDocument->GetActiveView( ) ) );
		}
	
	RChunkStorage::RChunkSearcher yIter;
	//
	// PS 6.0 and later: read the "extended" soft shadow settings chunk
	// if the chunk wasn't found don't complain - the file was written by PS 5.x
	yIter = storage.Start( kSoftShadowTag, FALSE );
	if (!yIter.End())
	{
		storage >> *m_pShadowSettings;
		storage.SelectParentChunk();
	}
	//
	// PS 6.0 and later: read the soft glow settings chunk
	// if the chunk wasn't found don't complain - the file was written by PS 5.x
	yIter = storage.Start( kSoftGlowTag, FALSE );
	if (!yIter.End())
	{
		storage >> *m_pGlowSettings;
		storage.SelectParentChunk();
	}
	//
	// PS 6.0 and later: read the soft vignette settings chunk
	// if the chunk wasn't found don't complain - the file was written by PS 5.x
	yIter = storage.Start( kSoftVignetteTag, FALSE );
	if (!yIter.End())
	{
		storage >> *m_pVignetteSettings;
		storage.SelectParentChunk();
	}


	//
	// PS 6.0 and later: read the Alpha Mask chunk
	// 
	yIter = storage.Start( kAlphaMaskTag, FALSE );
	if (!yIter.End())
	{
		storage >> m_dwMaskID;
		m_pAlphaMask = new RBitmapImage();
		m_pAlphaMask->Read( storage );
		storage.SelectParentChunk();
	}
	yIter = storage.Start( kAlphaMaskInfoTag, FALSE );
	if (!yIter.End())
	{
		DWORD	dwTemp, dwMoreData;

		storage >> dwTemp;

		m_kSpecialEdgeType = (int) dwTemp;

		storage >> dwMoreData;
		storage.SelectParentChunk();
	}

	//
	// PS 6.0 and later: read the edge outline settings chunk
	// if the chunk wasn't found don't complain - the file was written by PS 5.x
	yIter = storage.Start( kEdgeOutlineTag, FALSE );
	if (!yIter.End())
	{
		storage >> *m_pEdgeOutlineSettings;
		storage.SelectParentChunk();
	}
	//	Go back to reading at the prior level.
	storage.SetSelectedChunk( yCurrentChunk );

	XUpdateAllViews( kLayoutChanged, 0 );
	}

// ****************************************************************************
//
//  Function Name:	RView::ComputeAvoidancePath( )
//
//  Description:		Compute the runaround
//
//  Returns:			path that intrudes into pView
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::ComputeAvoidancePaths( RComponentView* pView, YAvoidancePathCollection& avoidanceCollection, const R2dTransform& transform, BOOLEAN fBlock ) const
	{
	YComponentViewIterator	iterator		= m_ComponentViewCollection.Start( );
	YComponentViewIterator	iteratorEnd	= m_ComponentViewCollection.End( );
	RRealVectorRect			targetVectorRect;

	//	If given a view, Start at that view
	if ( pView )
		{
		TpsAssert( avoidanceCollection.Count() == 0, "Avoidance Collection count != 0, Please empty before calling ComputeAvoidancePaths." );

		//	First, get the bounds of the target view
		targetVectorRect	= pView->GetBoundingRect( );

		//	First, find the given view in the list of views...
		for ( ; iterator != iteratorEnd; ++iterator )
			{
			if ( *iterator == pView )
				break;
			}

		//	If we are at the end. we have nothing to do. It is going to be read
		if ( iterator == iteratorEnd )
			return TRUE;

		//	Move past this iterator to start of items to avoid.
		++iterator;
		}
	else
		targetVectorRect	= RRealVectorRect( GetSize() );

	//	Compute relatives...
	RRealRect	targetRect		= targetVectorRect.m_TransformedBoundingRect;
	RRealPoint	targetTopLeft	= targetVectorRect.m_TopLeft;

	//	Compute the generic transform for all paths to be created relative to
	R2dTransform	newTransform = transform;
	//	Offset to move from view specific to be relative to given component
	newTransform.PreTranslate( -targetTopLeft.m_x, -targetTopLeft.m_y );


	try
		{
		for( ; iterator != iteratorEnd; ++iterator )
			{
			//	If there is an intersection between the target bounds and the iterated
			//		component bounds, ask for its outline
			//RIntVectorRect		componentRect		= (*iterator)->GetBoundingRect( );
			//RIntRect				componentBounds	= componentRect.m_TransformedBoundingRect;
			//if ( targetRect.IsIntersecting( componentBounds ) )
			if ( (*iterator)->IntersectsRect( targetRect ) )
				(*iterator)->AddOutline( avoidanceCollection, newTransform, fBlock );
			}
		}
	catch( ... )
		{
		return FALSE;
		}

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RView::CallEditComponent( )
//
//  Description:		Calls EditComponent
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::CallEditComponent( RComponentView* pComponent, EActivationMethod activationMethod, const RRealPoint& mousePoint )
	{
	YResourceId editUndoRedoPairId;

	RComponentView* pNewComponent = EditComponent( pComponent, activationMethod, mousePoint, editUndoRedoPairId, 0 );
	
	//	If the component was replaced, do a replace component action
	if ( pNewComponent )
		{
		RReplaceComponentAction*	pAction	= new RReplaceComponentAction( GetSelection( ),
																								 pComponent->GetComponentDocument( ),
																								 pNewComponent->GetComponentDocument( ),
																								 editUndoRedoPairId );
		GetRDocument( )->SendAction( pAction );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::EditComponent( )
//
//  Description:		Edits a component
//
//  Returns:			The new component if it had been replaced
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RView::EditComponent( RComponentView* pComponent,
												  EActivationMethod activationMethod,
												  const RRealPoint& mousePoint,
												  YResourceId&,
												  uLONG )
	{
	pComponent->OnXEditComponent( activationMethod, mousePoint );
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RView::SetMouseCapture( )
//
//  Description:		Capture the mouse (at least acknowledge that it is captured)
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::SetMouseCapture( )
	{
	m_fMouseCaptured = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RView::ReleaseMouseCapture( )
//
//  Description:		Releases the mouse capture. Unnecessary on the Mac
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::ReleaseMouseCapture( )
	{
	m_fMouseCaptured = FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RView::IsMouseCaptured( )
//
//  Description:		Returns whether the Mouse is being captured.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::IsMouseCaptured( ) const
	{
	return m_fMouseCaptured;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetDocument( )
//
//  Description:		Return the private member function RDocument
//
//  Returns:			RDocument*
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDocument* RView::GetRDocument( ) const
	{
	return m_pDocument;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetSelection( )
//
//  Description:		Accessor
//
//  Returns:			The current selection
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCompositeSelection* RView::GetSelection( ) const
	{
	return m_pSelection;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetBackgroundColor( )
//
//  Description:		Accessor
//
//  Returns:			The background color of this view
//
//  Exceptions:		None
//
// ****************************************************************************
//
RColor RView::GetBackgroundColor( ) const
	{
	RColor backgroundColor;

#ifdef _WINDOWS
	backgroundColor = RSolidColor( ::GetSysColor( COLOR_WINDOW ) );
#endif

	return backgroundColor;
	}

// ****************************************************************************
//
//  Function Name:	RView::CreateInterfaceCollection( )
//
//  Description:		Create a collection of interfaces with the given interfaceId
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::CreateInterfaceCollection( YInterfaceId interfaceId, YInterfaceCollection& collection, BOOLEAN fRecurs, BOOLEAN fSelection ) const
	{
	RInterface*					pInterface	= NULL;
	RCompositeSelection*		pSelection	= GetSelection( );
	//	
	//		If there is a collection, iterate through that...
	if ( fSelection && pSelection && (pSelection->Count( )>0) )
		{
		YSelectionIterator	iterator		= pSelection->Start( );
		YSelectionIterator	iteratorEnd	= pSelection->End( );
		for( ; iterator != iteratorEnd; ++iterator )
			{
			TpsAssertIsObject( RComponentView, (*iterator) );
			RComponentView*	pView	= static_cast<RComponentView*>( (*iterator) );
			pInterface	= pView->GetInterface( interfaceId );
			if ( pInterface )
				collection.InsertAtEnd( pInterface );
			if ( fRecurs )
				pView->CreateInterfaceCollection( interfaceId, collection, fRecurs, fSelection );
			pInterface	= NULL;
			}
		}
	else
		{
		YComponentViewIterator	iterator		= m_ComponentViewCollection.Start( );
		YComponentViewIterator	iteratorEnd	= m_ComponentViewCollection.End( );
		for ( ; iterator != iteratorEnd; ++iterator )
			{
			pInterface	= (*iterator)->GetInterface( interfaceId );
			if ( pInterface )
				collection.InsertAtEnd( pInterface );
			if ( fRecurs )
				(*iterator)->CreateInterfaceCollection( interfaceId, collection, fRecurs, fSelection );
			pInterface	= NULL;
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::OnDragEnter( )
//
//  Description:		Called by the framework when the mouse first enters the
//							non-scrolling region of the drop target window.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YDropEffect RView::OnXDragEnter( RDataTransferSource& dataSource, YModifierKey modifierKeys, const RRealPoint& point )
	{
	// Initialize some data
	m_MouseDownPoint = point;
	m_pComponentCollection = NULL;
	BOOLEAN fUseTargetDefaultSize = FALSE;
	m_fDragDropConvertedComponent = FALSE;

	// Compute a default return value, based on the key flags
	YDropEffect returnValue = ( modifierKeys & kCopyModifierKey ) ? kDropEffectCopy : kDropEffectMove;

	try
		{
		// If our drag&drop info is available, Renaissance started the operation
		if( dataSource.IsFormatAvailable( kDragDropFormat ) )
			{
			RDragDropInfo dragDropInfo;
			dragDropInfo.Paste( dataSource );
			m_DragOffset = dragDropInfo.m_Data.m_DragOffset;

			// If drag&drop data is available and the source view is the same as this view,
			// we are doing an internal drag.
			if( dragDropInfo.m_Data.m_pSourceView == this )
				{
				m_pComponentCollection = new RComponentCollection( *GetSelection( ) );
				m_fDeleteComponents = FALSE;
				}

			else
				fUseTargetDefaultSize = dragDropInfo.m_Data.m_fUseTargetDefaultSize;
			}

		// Otherwise the data is coming from somewhere else. Set the drag offset to zero
		else
			m_DragOffset = RRealSize( 0, 0 );

		// If we don't have a component collection yet, get the data down now
		if( !m_pComponentCollection )
			{
			// Get the component data
			m_pComponentCollection = new RComponentCollection;
			m_fDragDropConvertedComponent = m_pComponentCollection->Paste( dataSource, GetRDocument( ) );

			// If we got some data, scale the components to fit their new home
			if( m_pComponentCollection->Count( ) )
				{
				m_fDeleteComponents = TRUE;

				// If we need to use the default size, do so
				if( fUseTargetDefaultSize )
					ResetComponentsToDefaultSize( *m_pComponentCollection );

				// Scale the components to fit the useable area of the target view
				RRealSize scaleFactor = m_pComponentCollection->ScaleToFitInView( this );

				// Scale the drag offset by the amount the components were scaled
				m_DragOffset.Scale( scaleFactor );
				}
			}

		// If there isnt any data we can handle, just return
		if( m_pComponentCollection->Count( ) == 0 )
			returnValue = kDropEffectNone;
	
		// Otherwise, draw the initial dragging feedback
		else
			{
			// Remove any non-moveable components from the collection, we dont want them
			YComponentIterator iterator = m_pComponentCollection->Start( );
			while( iterator != m_pComponentCollection->End( ) )
				{
				YComponentIterator temp = iterator;
				++iterator;

				if( !( *temp )->GetComponentAttributes( ).IsMovable( ) )
					{
					if( m_fDeleteComponents )
						delete *temp;
					m_pComponentCollection->RemoveAt( temp );
					}
				}

			m_pFeedbackRenderer = GetDragDropFeedbackRenderer( );
			m_pFeedbackRenderer->BeginTracking( point );
			}
		}

	catch( ... )
		{
		returnValue = kDropEffectNone;
		}

	return returnValue;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetDragDropFeedbackRenderer
//
//  Description:		Gets the feedback renderer to use when dragging in this
//							view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RTrackingFeedbackRenderer*	RView::GetDragDropFeedbackRenderer( ) 
	{
	return new RDragDropTrackingFeedbackRenderer( this, m_pComponentCollection, m_DragOffset );
	}

// ****************************************************************************
//
//  Function Name:	RView::HasOldStyleShadow
//
//  Description:		Determines if the view has an old-style shadow
//
//  Returns:			TRUE if shadowed; otherwise FALSE.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::HasOldStyleShadow() const
{
	return m_pShadowSettings->m_fShadowOn;
}

// ****************************************************************************
//
//  Function Name:	RView::ResetComponentsToDefaultSize
//
//  Description:		Resets the components in the specified collection to
//							their default size. This is used for drag and drop.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::ResetComponentsToDefaultSize( RComponentCollection& collection )
	{
	// Get the original collection bounding rect
	RRealRect originalBoundingRect = collection.GetBoundingRect( );

	// Iterate the component collection reseting the components
	YComponentIterator iterator = collection.Start( );
	for( ; iterator != collection.End( ); ++iterator )
		{
		// Get the component view
		RComponentView* pView = static_cast<RComponentView*>( ( *iterator )->GetActiveView( ) );

		// Get its initial size
		RRealSize initialSize = pView->GetBoundingRect( ).WidthHeight( );

		// Reset the component
		( *iterator )->ResetToDefaultSize( GetRDocument( ) );

		// Get the new size
		RRealSize newSize = pView->GetBoundingRect( ).WidthHeight( );

		// Calculate how much the component was scaled
		RRealSize scaleFactor( newSize.m_dx / initialSize.m_dx, newSize.m_dy / initialSize.m_dy );

		// Get the offset from the components center point to the collection center point
		RRealSize offset( pView->GetBoundingRect( ).GetCenterPoint( ) - originalBoundingRect.GetCenterPoint( ) );

		// Calculate a new offset that is the scaled original offset
		RRealSize newOffset( offset.m_dx * scaleFactor.m_dx, offset.m_dy * scaleFactor.m_dy );

		// Move the component so that it is this distance from the collection center point. This
		// makes sure that we scale about the collection center, even though ResetToDefaultSize
		// doesnt do it.
		pView->Offset( RRealSize( offset.m_dx - newOffset.m_dx, offset.m_dy - newOffset.m_dy ) );
		}

	// Get the new collection bounding rect
	RRealRect newBoundingRect = collection.GetBoundingRect( );

	// Calculate how much the collection scaled
	RRealSize scaleFactor( newBoundingRect.Width( ) / originalBoundingRect.Width( ), newBoundingRect.Height( ) / originalBoundingRect.Height( ) );

	// Scale the drag offset by this much
	m_DragOffset.Scale( scaleFactor );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnDragOver( )
//
//  Description:		Called by the framework during a drag operation when the
//							mouse is moved over the drop target window.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YDropEffect RView::OnXDragOver( RDataTransferSource&, YModifierKey modifierKeys, const RRealPoint& localPoint )
	{
	// Make sure there is actually some data
	if( !m_pComponentCollection || m_pComponentCollection->Count( ) == 0 )
		return kDropEffectNone;

	BOOLEAN fAutoScroll = WillAutoScroll( localPoint );
	YDropEffect returnValue = fAutoScroll ? kDropEffectScroll : 0;

	// If the mouse has moved, or we will be scrolling, draw feedback
	m_pFeedbackRenderer->ContinueTracking( localPoint );

	// Figure out what to return based on the modifier key state
	return ( modifierKeys & kCopyModifierKey ) ? returnValue | kDropEffectCopy : returnValue | kDropEffectMove;
	}

// ****************************************************************************
//
//  Function Name:	RView::OnDragLeave( )
//
//  Description:		Called by the framework during a drag operation when the
//							mouse is moved out of the valid drop area for that window.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnXDragLeave( )
	{
	// Erase the old feedback
	if( m_pFeedbackRenderer )
		{
		m_pFeedbackRenderer->EndTracking( );
		delete m_pFeedbackRenderer;
		m_pFeedbackRenderer = NULL;
		}

	// Clean up
	if( m_pComponentCollection && m_fDeleteComponents )
		m_pComponentCollection->DeleteAllComponents( );

	delete m_pComponentCollection;
	m_pComponentCollection = NULL;
	}

// ****************************************************************************
//
//  Function Name:	RView::OnXDrop( )
//
//  Description:		Called by the framework when the user releases a data
//							object over a valid drop target.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnXDrop( RDataTransferSource& dataSource, const RRealPoint& point, YDropEffect dropEffect )
	{
	TpsAssert( m_pComponentCollection->Count( ) > 0, "Nothing to drop." );

	// Erase the old feedback
	m_pFeedbackRenderer->EndTracking( );
	delete m_pFeedbackRenderer;
	m_pFeedbackRenderer = NULL;

	// Create and do a drag&drop action
	if( dropEffect != kDropEffectNone )
		{
		RDragDropAction* pAction = new RDragDropAction( GetRDocument( ), *m_pComponentCollection, point + m_DragOffset, dataSource, dropEffect, m_fDragDropConvertedComponent );
		GetRDocument( )->SendAction( pAction );
		}

	// Set a flag so that if the data source is Renaissance, it knows not to delete the data.
	m_fDroppedInRenaissance = TRUE;

	// Clean up
	delete m_pComponentCollection;
	m_pComponentCollection = NULL;
	}

// ****************************************************************************
//
//  Function Name:	RView::DoDragDrop( )
//
//  Description:		Begins a drag&drop operation using the drag&drop protocol
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::DoDragDrop( const RRealPoint& mousePoint, BOOLEAN fUseTargetDefaultSize, YDropEffect allowableEffects )
	{
	ReleaseMouseCapture( );

	// Create a data target
	RDragDropDataTarget dataTarget( new RComponentDataRenderer );

	// Remove the current selection
	GetSelection( )->Remove( );

	// Create a component collection from the current selection
	RComponentCollection componentCollection( *GetSelection( ) );

	// Remove any non-moveable components from the collection, we dont want them
	YComponentIterator iterator = componentCollection.Start( );
	while( iterator != componentCollection.End( ) )
		{
		YComponentIterator temp = iterator;
		++iterator;

		if( !( *temp )->GetComponentAttributes( ).IsMovable( ) )
			componentCollection.RemoveAt( temp );
		}

	// Get the center of the bounding rect of the component collection
	RRealPoint centerPoint = componentCollection.GetBoundingRect( ).GetCenterPoint( );

	// Put up the drag&drop data
	RDragDropInfo dragDropInfo( this, mousePoint, centerPoint - mousePoint, fUseTargetDefaultSize );
	dragDropInfo.Copy( dataTarget );

	// Put up the selection
	RCopySelectionAction action( GetSelection( ), &dataTarget, TRUE );
	action.Do( );

	m_fDroppedInRenaissance = FALSE;

	// Do the drag&drop operation
	YDropEffect dropEffect = dataTarget.DoDragDrop( allowableEffects );

	// If the data was not dropped in Renaissance, and it was a move operation, delete it
	if( !m_fDroppedInRenaissance && dropEffect & kDropEffectMove )
		{
		RDragDropDeleteSelectionAction* pAction = new RDragDropDeleteSelectionAction( GetSelection( ) );
		GetRDocument( )->SendAction( pAction );
		}

	if( dropEffect == kDropEffectNone )
		GetSelection( )->Render( );
	else
		GetSelection( )->Invalidate( );

	m_fTrackSelection = FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetLowestViewSupportingData( )
//
//  Description:		Figures out the lowest hit view which can incorporate the
//							data in this data source.
//
//  Returns:			The hit view
//
//  Exceptions:		None
//
// ****************************************************************************
//
RView* RView::GetLowestViewSupportingData( const RDataTransferSource& dataSource, const RRealPoint& inPoint, RRealPoint& outPoint )
	{
	RView* pLowestView = NULL;
	RView* pCurrentView = this;
	RRealPoint localPoint = inPoint;

	while( TRUE )
		{
		// If the current view can incorporate the data, it becomes the lowest view found
		if( pCurrentView->GetRDocument( )->CanIncorporateData( dataSource ) )
			{
			pLowestView = pCurrentView;
			outPoint = localPoint;
			}

		// Check to see if we hit a component in the current view
		RComponentView* pComponentView = pCurrentView->GetHitComponentView( localPoint, TRUE );

		// If we did, convert the point to its coordinate system and check it
		if( pComponentView )
			{
			localPoint = pComponentView->ConvertPointToLocalCoordinateSystem( localPoint );
			pCurrentView = pComponentView;
			}

		// Otherwise, break out of the loop, we are done
		else
			break;
		}

	return pLowestView;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetDragDropDeleteAction( )
//
//  Description:		Return this views delete selection action
//
//  Returns:			DeleteSelectionAction( )
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RUndoableAction* RView::GetDragDropDeleteAction( ) const
	{
	return new RDragDropDeleteSelectionAction( GetSelection( ) );
	}

// ****************************************************************************
//
//  Function Name:	RView::HasMergeField( )
//
//  Description:		Return TRUE if this view contains any merge fields
//
//  Returns:			see above
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::HasMergeFields( ) const
	{
	YComponentViewIterator	iterator		= m_ComponentViewCollection.Start( );
	YComponentViewIterator	iteratorEnd	= m_ComponentViewCollection.End( );

	for( ; iterator != iteratorEnd; ++iterator )
		{
		if ( (*iterator)->HasMergeFields( ) )
			return TRUE;
		}

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RView::Associate( )
//
//  Description:		Associate the given merge entry with the given data
//
//  Returns:			TRUE if there is nothing wrong with the association
//							FALSE if something is wrong.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN	RView::Associate( RMergeEntry* pMergeEntry )
	{
	m_pMergeEntry = pMergeEntry;

	YComponentViewIterator	iterator		= m_ComponentViewCollection.Start( );
	YComponentViewIterator	iteratorEnd	= m_ComponentViewCollection.End( );

	for( ; iterator != iteratorEnd; ++iterator )
		{
		if ( !(*iterator)->Associate( pMergeEntry ) )
			return FALSE;
		}

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetMergeEntry( )
//
//  Description:		Return the stored Merge Entry
//
//  Returns:			m_pMergeEntry
//
//  Exceptions:		None
//
// ****************************************************************************
RMergeEntry*	RView::GetMergeEntry( ) const
	{
	return m_pMergeEntry;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetBoundingRectOfEmbeddedComponents( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RView::GetBoundingRectOfEmbeddedComponents( BOOLEAN fIncludeSelectionHandles ) const
	{
	// Get our useable area in case we need it; some components will clip to it
	RRealRect viewableArea = GetUseableArea( );

	BOOLEAN		fBoundsSet = FALSE;
	RRealRect	boundingRect( 0, 0, 0, 0 );

	YComponentViewIterator	iterator		= m_ComponentViewCollection.Start( );
	YComponentViewIterator	iteratorEnd	= m_ComponentViewCollection.End( );
	for( ; iterator != iteratorEnd; ++iterator )
		{
		const RComponentAttributes& componentAttributes = ( *iterator )->GetComponentAttributes( );

		//	Get the rectangle that we want to union/set into the bounding rect
		RRealRect tempRect;
		if( fIncludeSelectionHandles && componentAttributes.IsSelectable( ) )
			tempRect = RSingleSelection::GetBoundingRectIncludingHandles( *iterator );
		else
			{
			tempRect = ( *iterator )->GetBoundingRect( ).m_TransformedBoundingRect;
			if( componentAttributes.ClipToParent( ) )
				tempRect.Intersect( tempRect, viewableArea );
			}		

		if ( fBoundsSet )
			boundingRect.Union( boundingRect, tempRect );
		else
			boundingRect = tempRect;
		fBoundsSet = TRUE;
		}

	return boundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetComponentWithUniqueId( )
//
//  Description:		Looks for a component with the specified unique id.
//
//  Returns:			The component, if one exists, NULL if not
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RView::GetComponentViewWithUniqueId( YUniqueId uniqueId ) const
	{
	YComponentViewIterator iterator = m_ComponentViewCollection.Start( );
	for( ; iterator != m_ComponentViewCollection.End( ); ++iterator )
		{
		if( ( *iterator )->GetComponentAttributes( ).GetUniqueId( ) == uniqueId )
			return *iterator;
		}

	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetComponentsToBringForward( )
//
//  Description:		Fills in the specified collection with the currently
//							selected component views that can move forward.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::GetComponentsToBringForward( YComponentViewCollection& collection ) const
	{
	// First check for an empty selection
	if( GetSelection( )->Count( ) == 0 )
		return;

	// It is easier to find the components that can *not* move forward, so lets do that
	YComponentViewCollection inverseCollection;

	// First sort the selection by z-order
	GetSelection( )->SortByZOrder( );

	// Iterator through the selection
	YSelectionIterator selectionIterator = GetSelection( )->End( );
	do
		{
		--selectionIterator;
		RComponentView* pComponentView = *( selectionIterator );

		BOOLEAN fMoveable = TRUE;

		// Is it locked?
		if( pComponentView->GetComponentAttributes( ).IsLocked( ) )
			fMoveable = FALSE;

		// Is it at the head of a z-order chain?
		else if( IsZOrderChainHead( pComponentView ) )
			fMoveable = FALSE;

		// Is it directly down stream from an object that can not move?
		else
			{
			YComponentViewIterator tempIterator = m_ComponentViewCollection.Find( pComponentView );
			++tempIterator;
			if( !( *tempIterator )->GetComponentAttributes( ).IsLocked( ) )
				if( inverseCollection.Find( *tempIterator ) != inverseCollection.End( ) )
					fMoveable = FALSE;
			}

		if( fMoveable == FALSE )
			inverseCollection.InsertAtEnd( pComponentView );
		}

	while( selectionIterator != GetSelection( )->Start( ) );

	// Now we know the component that can not move forward. Use anything not in that list
	if( GetSelection( )->Count( ) == inverseCollection.Count( ) )
		return;

	selectionIterator = GetSelection( )->End( );
	do
		{
		--selectionIterator;
		if( inverseCollection.Find( *selectionIterator ) == inverseCollection.End( ) )
			collection.InsertAtEnd( *selectionIterator );
		}
	while( selectionIterator != GetSelection( )->Start( ) );
	}

// ****************************************************************************
//
//  Function Name:	RView::GetComponentsToSendBackward( )
//
//  Description:		Fills in the specified collection with the currently
//							selected component views that can move backward.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::GetComponentsToSendBackward( YComponentViewCollection& collection ) const
	{
	// First check for an empty selection
	if( GetSelection( )->Count( ) == 0 )
		return;

	// It is easier to find the components that can *not* move backward, so lets do that
	YComponentViewCollection inverseCollection;

	// First sort the selection by z-order
	GetSelection( )->SortByZOrder( );

	// Iterator through the selection
	YSelectionIterator selectionIterator = GetSelection( )->Start( );
	for( ; selectionIterator != GetSelection( )->End( ); ++selectionIterator )
		{
		RComponentView* pComponentView = *( selectionIterator );

		BOOLEAN fMoveable = TRUE;

		// Is it locked?
		if( pComponentView->GetComponentAttributes( ).IsLocked( ) )
			fMoveable = FALSE;

		// Is it at the tail of a z-order chain?
		else if( IsZOrderChainTail( pComponentView ) )
			fMoveable = FALSE;

		// Is it directly up stream from an object that can not move?
		else
			{
			YComponentViewIterator tempIterator = m_ComponentViewCollection.Find( pComponentView );
			--tempIterator;
			if( !( *tempIterator )->GetComponentAttributes( ).IsLocked( ) )
				if( inverseCollection.Find( *tempIterator ) != inverseCollection.End( ) )
					fMoveable = FALSE;
			}

		if( fMoveable == FALSE )
			inverseCollection.InsertAtEnd( pComponentView );
		}

	// Now we know the component that can not move forward. Use anything not in that list
	if( GetSelection( )->Count( ) == inverseCollection.Count( ) )
		return;

	selectionIterator = GetSelection( )->Start( );
	for( ; selectionIterator != GetSelection( )->End( ); ++selectionIterator )
		if( inverseCollection.Find( *selectionIterator ) == inverseCollection.End( ) )
			collection.InsertAtEnd( *selectionIterator );
	}

// ****************************************************************************
//
//  Function Name:	RView::IsZOrderChainHead( )
//
//  Description:		Checks to see if this object is at the head of a z order
//							chain within a layer.
//
//  Returns:			TRUE if the object is at the head of a z order chain
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::IsZOrderChainHead( RComponentView* pComponentView ) const
	{
	// First find the object
	YComponentViewIterator iterator = m_ComponentViewCollection.Find( pComponentView );
	TpsAssert( iterator != m_ComponentViewCollection.End( ), "Component not found." );

	// Is it the last object?
	YComponentViewIterator	iteratorEnd	= m_ComponentViewCollection.End( );
	if( iterator == iteratorEnd - 1 )
		return TRUE;

	// Check to see if the next object is in the same z-layer
	if( ( *iterator )->GetComponentAttributes( ).GetZLayer( ) == ( *( iterator + 1 ) )->GetComponentAttributes( ).GetZLayer( ) )
		return FALSE;

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RView::IsZOrderChainTail( )
//
//  Description:		Checks to see if this object is at the tail of a z order
//							chain within a layer.
//
//  Returns:			TRUE if the object is at the tail of a z order chain
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::IsZOrderChainTail( RComponentView* pComponentView ) const
	{
	// First find the object
	YComponentViewIterator iterator = m_ComponentViewCollection.Find( pComponentView );
	TpsAssert( iterator != m_ComponentViewCollection.End( ), "Component not found." );

	// Is it the first object?
	if( iterator == m_ComponentViewCollection.Start( ) )
		return TRUE;

	// Check to see if the previous object is in the same z-layer
	if( ( *iterator )->GetComponentAttributes( ).GetZLayer( ) == ( *( iterator - 1 ) )->GetComponentAttributes( ).GetZLayer( ) )
		return FALSE;

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RView::BringForward( )
//
//  Description:		Moves the specifed component view forward
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::BringForward( RComponentView* pComponentView )
	{
	// Find the component view in our view collection
	YComponentViewIterator iterator = m_ComponentViewCollection.Find( pComponentView );
	TpsAssert( iterator != m_ComponentViewCollection.End( ), "Component view not found." );

	// Get the z-layer of the component view
	YZLayer zLayer = pComponentView->GetComponentAttributes( ).GetZLayer( );

	// Get an iterator for the next component
	YComponentViewIterator tempIterator = iterator;
	++tempIterator;

	// Remove the component from the list
	m_ComponentViewCollection.RemoveAt( iterator );

	// Reinsert it
	m_ComponentViewCollection.InsertAt( ++tempIterator, pComponentView );
	}

// ****************************************************************************
//
//  Function Name:	RView::BringToFront( )
//
//  Description:		Moves the specifed component view to the front of its
//							z-order chain
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::BringToFront( RComponentView* pComponentView )
	{
	// Find the component view in our view collection
	YComponentViewIterator iterator = m_ComponentViewCollection.Find( pComponentView );
	TpsAssert( iterator != m_ComponentViewCollection.End( ), "Component view not found." );

	// Get the z-layer of the component view
	YZLayer zLayer = pComponentView->GetComponentAttributes( ).GetZLayer( );

	// Iterate the component list until we hit the end, or we find a component with a
	// different z-layer
	YComponentViewIterator tempIterator = iterator;

	for( ; tempIterator != m_ComponentViewCollection.End( ); ++tempIterator )
		if( ( *tempIterator )->GetComponentAttributes( ).GetZLayer( ) != zLayer )
			break;

	if( tempIterator != iterator )
		{
		// Remove the component from the list
		m_ComponentViewCollection.RemoveAt( iterator );

		// Reinsert it
		m_ComponentViewCollection.InsertAt( tempIterator, pComponentView );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::SendBackward( )
//
//  Description:		Moves the specifed component view back
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::SendBackward( RComponentView* pComponentView )
	{
	// Find the component view in our view collection
	YComponentViewIterator iterator = m_ComponentViewCollection.Find( pComponentView );
	TpsAssert( iterator != m_ComponentViewCollection.End( ), "Component view not found." );

	// Get the z-layer of the component view
	YZLayer zLayer = pComponentView->GetComponentAttributes( ).GetZLayer( );

	// Get an iterator for the previous component
	YComponentViewIterator tempIterator = iterator;
	--tempIterator;

	// Remove the component from the list
	m_ComponentViewCollection.RemoveAt( iterator );

	// Reinsert it
	m_ComponentViewCollection.InsertAt( tempIterator, pComponentView );
	}

// ****************************************************************************
//
//  Function Name:	RView::SendToBack( )
//
//  Description:		Moves the specifed component view to the back of its
//							z-order chain
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::SendToBack( RComponentView* pComponentView )
	{
	// Find the component view in our view collection
	YComponentViewIterator iterator = m_ComponentViewCollection.Find( pComponentView );
	TpsAssert( iterator != m_ComponentViewCollection.End( ), "Component view not found." );

	if( iterator == m_ComponentViewCollection.Start( ) )
		return;

	// Get the z-layer of the component view
	YZLayer zLayer = pComponentView->GetComponentAttributes( ).GetZLayer( );

	// Iterate the component list until we hit the end, or we find a component with a
	// different z-layer
	YComponentViewIterator tempIterator = iterator;
	do
		{
		--tempIterator;
		if( ( *tempIterator )->GetComponentAttributes( ).GetZLayer( ) != zLayer )
			{
			++tempIterator;
			break;
			}
		}
	while( tempIterator != m_ComponentViewCollection.Start( ) );

	if( tempIterator != iterator )
		{
		// Remove the component from the list
		m_ComponentViewCollection.RemoveAt( iterator );

		// Reinsert it
		m_ComponentViewCollection.InsertAt( tempIterator, pComponentView );
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::GetFeedbackSettings( )
//
//  Description:		Gets the color and draw mode we should use to draw feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::GetFeedbackSettings( RSolidColor& color, YDrawMode& drawMode ) const
	{
	// Get the background color of this view
	RColor backgroundColor = GetBackgroundColor( );

	// If the color is solid, get high contrast draw mode and use it
	if( backgroundColor.GetFillMethod( ) == RColor::kSolid )
		{
		color = backgroundColor.GetSolidColor( );
		drawMode = color.GetHighContrastDrawMode( );
		}

	// Otherwise, just set it to XNOR black
	else
		{
		color = kBlack;
		drawMode = kXNOR;
		}
	}

// ****************************************************************************
//
//  Function Name:	RView::GetResizeTracker( )
//
//  Description:		Gets a tracker to use for resizing
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelectionTracker* RView::GetResizeTracker( ) const
	{
	return new RResizeSelectionTracker( GetSelection( ) );
	}

// ****************************************************************************
//
//  Function Name:	RView::GetRotateTracker( )
//
//  Description:		Gets a tracker to use for rotating
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelectionTracker* RView::GetRotateTracker( ) const
	{
	return new RRotateSelectionTracker( GetSelection( ) );
	}

// ****************************************************************************
//
//  Function Name:	RView::ScaleComponentToFit( )
//
//  Description:		Allows a view to control how components are scaled to 
//							fit within it.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::ScaleComponentToFit( RComponentView* pComponent, const RRealPoint& point, const RRealSize& scaleFactor )
	{
	// Apply the resize constraint
	RRealSize constrainedScaleFactor = pComponent->ApplyResizeConstraint( scaleFactor );

	// Do an alternate scale
	pComponent->AlternateScale( point, constrainedScaleFactor, TRUE );
	}

// ****************************************************************************
//
//  Function Name:	RView::GetReferenceSize( )
//
//  Description:		Accessor
//
//  Returns:			The view size against which the view transform is applied.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RView::GetReferenceSize( ) const
	{
	// Use the real size as the default
	return GetSize( );
	}

// ****************************************************************************
//
//  Function Name:	RView::GetViewCollectionStart( )
//
//  Description:		Accessor
//
//  Returns:			view collection start
//
//  Exceptions:		None
//
// ****************************************************************************
//
YComponentViewIterator RView::GetViewCollectionStart( )
	{
	return m_ComponentViewCollection.Start( );
	}

// ****************************************************************************
//
//  Function Name:	RView::GetViewCollectionEnd( )
//
//  Description:		Accessor
//
//  Returns:			view collection end
//
//  Exceptions:		None
//
// ****************************************************************************
//
YComponentViewIterator RView::GetViewCollectionEnd( )
	{
	return m_ComponentViewCollection.End( );
	}

// ****************************************************************************
//
//  Function Name:	RView::OnFrameComponent( )
//
//  Description:		Called when a component is framed
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::OnFrameComponent( RComponentView* )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RView::GetNumComponents( )
//
//  Description:		Accessor
//
//  Returns:			The number of components in this view
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCounter RView::GetNumComponents( ) const
	{
	return m_ComponentViewCollection.Count( );
	}

// ****************************************************************************
//
//  Function Name:	operator|=
//
//  Description:		Global operator to allow |=ing of EDirections
//
//  Returns:			reference to the left hand operand
//
//  Exceptions:		None
//
// ****************************************************************************
//
RView::EDirection& operator|=( RView::EDirection& lhs, const RView::EDirection& rhs )
	{
	lhs = static_cast<RView::EDirection>( lhs | rhs );

	// Assert that we haven't set lhs to an illegal value.
	TpsAssert( !( ( lhs & RView::kLeft ) && ( lhs & RView::kRight ) ), "Invalid enum" );
	TpsAssert( !( ( lhs & RView::kUp ) && ( lhs & RView::kDown ) ), "Invalid enum" );

	return lhs;
	}


// ****************************************************************************
//
//  Function Name:	RView::RectInView( )
//
//  Description:	virtual function override to ensure a rectangle is visible   
//					in a scrolled view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::RectInView( const RRealVectorRect& /* rect */ )
{
}

#ifdef _WINDOWS

// ****************************************************************************
//
//  Function Name:	RView::GetModifierKeyState( )
//
//  Description:		Gets the state of the modifier keys (shift, control, and
//							the mouse buttons.)
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YModifierKey RView::GetModifierKeyState( ) const
	{
	const WORD kMask = 0x8000;

	YModifierKey modifierKeys = 0;

	// Check the shift key
	if( ::GetKeyState( VK_SHIFT ) & kMask )
		modifierKeys |= MK_SHIFT;

	// Check the control key
	if( ::GetKeyState( VK_CONTROL ) & kMask )
		modifierKeys |= MK_CONTROL;

	// Check the left button
	if( ::GetKeyState( VK_LBUTTON ) & kMask )
		modifierKeys |= MK_LBUTTON;

	// Check the middle button
	if( ::GetKeyState( VK_MBUTTON ) & kMask )
		modifierKeys |= MK_MBUTTON;

	// Check the right button
	if( ::GetKeyState( VK_RBUTTON ) & kMask )
		modifierKeys |= MK_RBUTTON;

	return modifierKeys;
	}

#endif	// _WINDOWS

#ifdef	TPSDEBUG
// ****************************************************************************
//
//  Function Name:	RView::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::Validate( )	const
	{
	RObject::Validate( );
	TpsAssertIsObject( RView, this );
	}

#endif	// TPSDEBUG



// ****************************************************************************
//
//  Function Name:	RView::SetAlphaMask( )
//
//  Description:	Sets the alpha mask "artistic effect" for the view
//					Expects a pointer to an RBitmap Image and a MaskID.
//					The caller should 'new' the RBitmapImage but should
//					not delete it. The View will take ownership of the
//					pointer and will delete it when the mask is replaced
//					or when the view is deleted.
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void  RView::SetAlphaMask( RBitmapImage * pImage, DWORD dwMaskID )
{
	if (m_pAlphaMask)
	{
		delete m_pAlphaMask;
		m_pAlphaMask = NULL;
	}

	m_pAlphaMask = pImage;
	m_dwMaskID   = dwMaskID;
}


// ****************************************************************************
//
//  Function Name:	RView::ClearAlphaMask( )
//
//  Description:	Removes the alpha mask effect from the view
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::ClearAlphaMask( )
{
	if (m_pAlphaMask)
	{
		delete m_pAlphaMask;
		m_pAlphaMask = NULL;
	}
	m_dwMaskID = 0;
}
