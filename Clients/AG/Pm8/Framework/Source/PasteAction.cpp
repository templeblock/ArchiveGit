// ****************************************************************************
//
//  File Name:			PasteAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RPasteActionBase class
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
//  $Logfile:: /PM8/Framework/Source/PasteAction.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "PasteAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include "DataTransfer.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "ApplicationGlobals.h"
#include "FrameworkResourceIds.h"
#include "ClipboardDataTransfer.h"

YActionId RPasteAction::m_ActionId( "RPasteAction" );
YActionId RPasteHereAction::m_ActionId( "RPasteHereAction" );
YActionId RPasteComponentHereAction::m_ActionId( "RPasteComponentHereAction" );
YActionId RDropAction::m_ActionId( "RDropAction" );

// ****************************************************************************
//
//  Function Name:	RPasteActionBase::RPasteActionBase( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPasteActionBase::RPasteActionBase( RDocument* pDocument, const YActionId& actionId )
	: RUndoableAction( actionId, STRING_UNDO_PASTE, STRING_REDO_PASTE ),
	  m_pDocument( pDocument ),
	  m_pCurrentSelection( pDocument->GetActiveView( )->GetSelection( ) ),
	  m_OldSelection( *m_pCurrentSelection ),
	  m_fWarnAboutConvertedComponents( FALSE )
	{
	// Validate parameters
	TpsAssertValid( pDocument );
	}

// ****************************************************************************
//
//  Function Name:	RPasteActionBase::RPasteActionBase( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPasteActionBase::RPasteActionBase( RDocument* pDocument, RScript& script, const YActionId& actionId )
	: RUndoableAction( actionId, script ),
	  m_pDocument( pDocument ),
  	  m_pCurrentSelection( pDocument->GetActiveView( )->GetSelection( ) ),
	  m_OldSelection( *m_pCurrentSelection )
	{
	// Validate parameters
	TpsAssertValid( pDocument );
	}

// ****************************************************************************
//
//  Function Name:	RPasteActionBase::~RPasteActionBase( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPasteActionBase::~RPasteActionBase( )
	{
	if( (m_asLastAction == kActionUndo ) || (m_asLastAction == kActionNotDone) )
		{
		m_PastedComponentCollection.DeleteAllComponents( );
		}

	else
		{
		m_ReplacedComponentCollection.DeleteAllComponents( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RPasteActionBase::Initialize( )
//
//  Description:		Initializer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPasteActionBase::Initialize( const RRealPoint& pasteCenterPoint )
	{
	m_PasteCenterPoint = pasteCenterPoint;

	// Set new positions in the parent view
	SetNewComponentPositions( m_PastedComponentCollection );
	}

// ****************************************************************************
//
//  Function Name:	RPasteActionBase::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPasteActionBase::Do( )
	{
	// Add the components to the target document
	YComponentIterator iterator = m_PastedComponentCollection.Start( );
	for( ; iterator != m_PastedComponentCollection.End( ); ++iterator )
		{
		// Is this a unique component?
		YUniqueId uniqueId = ( *iterator )->GetComponentAttributes( ).GetUniqueId( );
		if( uniqueId != kNotUnique )
			{
			// The component is unique. Look for an existing instance
			RComponentDocument* pComponentToReplace = m_pDocument->GetComponentWithUniqueId( uniqueId );
			if( pComponentToReplace )
				{
				// Another component with the same unique id exists. Remove it and save it away
				m_pDocument->RemoveComponent( pComponentToReplace );
				m_ReplacedComponentCollection.InsertAtEnd( pComponentToReplace );
				}
			}

		// Add the new component
		m_pDocument->AddComponent( *iterator );
		}

	// Select the new components
	m_pCurrentSelection->Invalidate( );
	m_pCurrentSelection->UnselectAll( FALSE );

	iterator = m_PastedComponentCollection.Start( );
	for( ; iterator != m_PastedComponentCollection.End( ); ++iterator )
		m_pCurrentSelection->Select( static_cast<RComponentView*>( ( *iterator )->GetActiveView( ) ), FALSE );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	m_pCurrentSelection->GetView( )->UpdateScrollBars( kChangeObjectPosition );

	//	Invalidate the current view
	m_pCurrentSelection->Invalidate( );

	// If any components were converted to images, inform the user now
	if( m_fWarnAboutConvertedComponents )
		{
		RAlert( ).InformUser( STRING_ALERT_OBJECT_CONVERTED );

		// Dont warn again on redo
		m_fWarnAboutConvertedComponents = FALSE;
		}

	// Call the base method to setup the state properly
	return RUndoableAction::Do( );
	}

// ****************************************************************************
//
//  Function Name:	RPasteActionBase::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPasteActionBase::Undo( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Undo( );

	// Remove the components
	YComponentIterator iterator = m_PastedComponentCollection.Start( );
	for( ; iterator != m_PastedComponentCollection.End( ); ++iterator )
		m_pDocument->RemoveComponent( *iterator );

	// Add back the replaced components
	iterator = m_ReplacedComponentCollection.Start( );
	for( ; iterator != m_ReplacedComponentCollection.End( ); ++iterator )
		m_pDocument->AddComponent( *iterator );

	m_ReplacedComponentCollection.Empty( );

	// Remove the selection
	m_pCurrentSelection->Remove( );

	// Restore the selection
	*m_pCurrentSelection = m_OldSelection;

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	m_pCurrentSelection->GetView( )->UpdateScrollBars( kChangeObjectPosition );
	}

// ****************************************************************************
//
//  Function Name:	RPasteActionBase::SetNewComponentPositions( )
//
//  Description:		Calculates and sets a new position for a pasted component. 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPasteActionBase::SetNewComponentPositions( YComponentCollection& componentCollection ) const
	{
	// Get the bounding rect of all the components
	RRealRect componentsBoundingRect = componentCollection.GetBoundingRect( );

	// Get the center of this bounding rect
	RRealPoint boundingRectCenterPoint = componentsBoundingRect.GetCenterPoint( );

	// Calculate how much the bounding rect is offset from the view center
	RRealSize offset = m_PasteCenterPoint - boundingRectCenterPoint;

	YComponentIterator iterator = componentCollection.Start( );
	for( ; iterator != componentCollection.End( ); ++iterator )
		{
		// Get the active view of the document
		TpsAssertIsObject( RComponentView, ( *iterator )->GetActiveView( ) );
		RComponentView* pComponentView = static_cast<RComponentView*>( ( *iterator )->GetActiveView( ) );

		// Offset the view to position it
		pComponentView->Offset( offset, FALSE );
		}
	}

// ****************************************************************************
//
//  Function Name:	RPasteActionBase::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPasteActionBase::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RPasteActionBase::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPasteActionBase::Validate( ) const
	{
	RAction::Validate( );
	TpsAssertIsObject( RPasteActionBase, this );
	}

#endif	//	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RPasteAction::RPasteAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPasteAction::RPasteAction( RDocument* pDocument ) : RPasteActionBase( pDocument, m_ActionId )
	{
	DoPaste( );
	}

// ****************************************************************************
//
//  Function Name:	RPasteAction::RPasteAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPasteAction::RPasteAction( RDocument* pDocument, RScript& script ) : RPasteActionBase( pDocument, script, m_ActionId )
	{
	DoPaste( );
	}

// ****************************************************************************
//
//  Function Name:	RPasteAction::RPasteAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPasteHereAction::RPasteHereAction( RDocument* pDocument, const RRealPoint& point )
	: RPasteActionBase( pDocument, m_ActionId )
	{
	// Create a clipboard transfer
	RClipboardDataSource clipboardTransfer;

	// Get the data
	m_fWarnAboutConvertedComponents = m_PastedComponentCollection.Paste( clipboardTransfer, m_pDocument );
	if( m_PastedComponentCollection.Count( ) == 0 )
		throw kClipboardError;

	// Scale the components to fit the target view
	m_PastedComponentCollection.ScaleToFitInView( m_pDocument->GetActiveView( ) );

	// Initialize
	Initialize( point );
	}

// ****************************************************************************
//
//  Function Name:	RPasteAction::RPasteAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPasteComponentHereAction::RPasteComponentHereAction( RDocument* pDocument, RComponentDocument* pComponent, const RRealPoint& point )
	: RPasteActionBase( pDocument, m_ActionId )
	{
	//	Insert the component into the collection
	m_PastedComponentCollection.InsertAtEnd( pComponent );

	// Initialize
	Initialize( point );
	}

// ****************************************************************************
//
//  Function Name:	RPasteAction::RPasteAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPasteAction::DoPaste( )
	{
	// Create a clipboard transfer
	RClipboardDataSource clipboardTransfer;

	// Get the data
	m_fWarnAboutConvertedComponents = m_PastedComponentCollection.Paste( clipboardTransfer, m_pDocument );
	if( m_PastedComponentCollection.Count( ) == 0 )
		throw kClipboardError;

	// Scale the components to fit the target view
	m_PastedComponentCollection.ScaleToFitInView( m_pDocument->GetActiveView( ) );

	// Initialize
	Initialize( m_pDocument->GetActiveView( )->GetInsertionPoint( m_PastedComponentCollection.GetBoundingRect( ).WidthHeight( ) ) );
	}

// ****************************************************************************
//
//  Function Name:	RDropAction::RDropAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDropAction::RDropAction( RDocument* pDocument, RComponentCollection& componentCollection, const RRealPoint& dropPoint, BOOLEAN fConvertedComponent )
	: RPasteActionBase( pDocument, m_ActionId )
	{
	m_PastedComponentCollection = componentCollection;
	m_fWarnAboutConvertedComponents = fConvertedComponent;

	Initialize( dropPoint );
	}
