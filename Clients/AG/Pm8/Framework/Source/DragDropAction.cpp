// ****************************************************************************
//
//  File Name:			DragDropAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RDragDropAction class
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
//  $Logfile:: /PM8/Framework/Source/DragDropAction.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "DragDropAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "DragDropDataTransfer.h"
#include "PasteAction.h"
#include "DeleteSelectionAction.h"
#include "FrameworkResourceIds.h"
#include "Document.h"
#include "MoveSelectionAction.h"

YActionId RDragDropAction::m_ActionId( "RDragDropAction" );

// ****************************************************************************
//
//  Function Name:	RDragDropAction::RDragDropAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDragDropAction::RDragDropAction( RDocument* pTargetDocument,
											 RComponentCollection& componentCollection,
											 const RRealPoint& dropPoint,
											 RDataTransferSource& dataSource,
											 YDropEffect dropEffect,
											 BOOLEAN fConvertedComponent )
	: RChainedUndoableAction( m_ActionId, STRING_UNDO_PASTE, STRING_REDO_PASTE ),
	  m_pDocument( pTargetDocument ),
	  m_fDropTarget( TRUE ),
	  m_DroppedComponentCollection( componentCollection ),
	  m_DropPoint( dropPoint ),
	  m_pInternalAction( NULL ),
	  m_fInternal( FALSE ),
	  m_pActiveView( NULL ),
	  m_fConvertedComponent( fConvertedComponent )
	{
	// Check to see if our drag&drop format is available. If it is, the data must have come
	// from another Renaissance document. We must setup a chained action.
	if( dataSource.IsFormatAvailable( kDragDropFormat ) )
		{
		// Get the dragdrop info
		RDragDropInfo dragDropInfo;
		dragDropInfo.Paste( dataSource );

		// If the source document and the target document are the same, we did an internal drag
		if( dragDropInfo.m_Data.m_pSourceView->GetRDocument( ) == pTargetDocument )
			{
			if( dropEffect & kDropEffectMove )
				{
				m_DragStartPoint = dragDropInfo.m_Data.m_DragStartPoint;
				m_DropPoint -= dragDropInfo.m_Data.m_DragOffset;
				m_fInternal = TRUE;
				//	Change Undo/Redo string IDs
				m_uwUndoStringId = STRING_UNDO_MOVE_COMPONENT;
				m_uwRedoStringId = STRING_REDO_MOVE_COMPONENT;
				}
			else
				{
				m_DroppedComponentCollection.Empty( );
				m_DroppedComponentCollection.Paste( dataSource, pTargetDocument );
				}
			}

		// Create another dragdrop action for the source end of the transaction and chain it on
		else
			{		
			if( dropEffect == kDropEffectMove )
				{
				RDocument* pSourceDocument = dragDropInfo.m_Data.m_pSourceView->GetRDocument( );
				m_pChainedAction = new RDragDropAction( pSourceDocument, this );
				//
				//	If this drag/drop
				if ( pSourceDocument->GetTopLevelDocument() == pTargetDocument->GetTopLevelDocument() )
					m_pChainedAction->Do( );
				else
					pSourceDocument->SendAction( m_pChainedAction );
				}
			//	Change Undo/Redo string IDs
			m_uwUndoStringId = STRING_UNDO_PASTE;
			m_uwRedoStringId = STRING_REDO_PASTE;
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RDragDropAction::RDragDropAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDragDropAction::RDragDropAction( RDocument* pSourceDocument, RChainedUndoableAction* pChainedAction )
	: RChainedUndoableAction( m_ActionId, STRING_UNDO_DELETE, STRING_REDO_DELETE, pChainedAction ),
	  m_pDocument( pSourceDocument ),
	  m_fDropTarget( FALSE ),
	  m_fInternal( FALSE ),
	  m_pActiveView( NULL )
	{
	}

// ****************************************************************************
//
//  Function Name:	RDragDropAction::~RDragDropAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDragDropAction::~RDragDropAction( )
	{
	delete m_pInternalAction;
	}

// ****************************************************************************
//
//  Function Name:	RDragDropAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDragDropAction::Do( )
	{
	// Call the base method to setup the state properly
	RChainedUndoableAction::Do( );

	// If this was an internal operation, do a move
	if( m_fInternal )
		m_pInternalAction = new RMoveSelectionAction( m_pDocument->GetActiveView( )->GetSelection( ), m_DropPoint - m_DragStartPoint );

	// If this is the target end of the transaction, do a drop
	else if( m_fDropTarget )
		{
		m_pInternalAction	= new RDropAction( m_pDocument, m_DroppedComponentCollection, m_DropPoint, m_fConvertedComponent );
		m_pActiveView		= m_pDocument->GetActiveView()->DeactivateComponentView( );
		}

	// Otherwise, this is the source end of the transaction, do a delete
	else
		m_pInternalAction = m_pDocument->GetActiveView( )->GetDragDropDeleteAction( );

	return m_pInternalAction->Do( );
	}

// ****************************************************************************
//
//  Function Name:	RDragDropAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDragDropAction::Undo( )
	{
	// Call the base method to setup the state properly
	RChainedUndoableAction::Undo( );

	m_pInternalAction->Undo( );

	if( m_fDropTarget == FALSE )
		{
		m_pDocument->GetActiveView( )->ActivateView( );
		}
	else 
		{
		if( m_pActiveView )
			m_pDocument->GetActiveView( )->ActivateComponentView( m_pActiveView );
		}

	}											

// ****************************************************************************
//
//  Function Name:	RDragDropAction::Redo( )
//
//  Description:		Redoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDragDropAction::Redo( )
	{
	// Call the base method to setup the state properly
	RChainedUndoableAction::Redo( );

	m_pInternalAction->Redo( );

	if( m_fDropTarget == TRUE )
		{
		m_pDocument->GetActiveView( )->ActivateView( );
		m_pDocument->GetActiveView( )->DeactivateComponentView( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RDragDropAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDragDropAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RDragDropAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDragDropAction::Validate( ) const
	{
	RChainedUndoableAction::Validate( );
	TpsAssertIsObject( RDragDropAction, this );
	}

#endif	//	TPSDEBUG
