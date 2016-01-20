// ****************************************************************************
//
//  File Name:			CopySelectionAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RCopySelectionAction class
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
//  $Logfile:: /PM8/Framework/Source/CopySelectionAction.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "CopySelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ComponentDocument.h"
#include "ClipboardDataTransfer.h"
#include "DragDropDataTransfer.h"
#include "ComponentDataRenderer.h"
#include "ComponentView.h"

YActionId RCopySelectionAction::m_ActionId( "RCopySelectionAction" );
YActionId RCutSelectionAction::m_ActionId( "RCutSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RCopySelectionAction::RCopySelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCopySelectionAction::RCopySelectionAction( RCompositeSelection* pCurrentSelection, RDataTransferTarget* pDataTransferTarget, BOOLEAN fDelayRenderComponents )
	: RAction( m_ActionId ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_pDataTransferTarget( pDataTransferTarget ),
	  m_fDelayRenderComponents( fDelayRenderComponents )

	{
	// Validate parameters
	TpsAssertValid( pCurrentSelection );
	}

// ****************************************************************************
//
//  Function Name:	RCopySelectionAction::RCopySelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCopySelectionAction::RCopySelectionAction( RCompositeSelection* pCurrentSelection, RDataTransferTarget* pDataTransferTarget, BOOLEAN fDelayRenderComponents, RScript& script )
	: RAction( m_ActionId, script ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_pDataTransferTarget( pDataTransferTarget ),
	  m_fDelayRenderComponents( fDelayRenderComponents )
	{
	// Validate parameters
	TpsAssertValid( pCurrentSelection );
	}

// ****************************************************************************
//
//  Function Name:	RCopySelectionAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RCopySelectionAction::Do( )
	{
	// Sort the selection by z-order
	m_pCurrentSelection->SortByZOrder( );

	// Make a component collection out of the selection
	RComponentCollection componentCollection( *m_pCurrentSelection );

	try
		{
		// If we are delay rendering the components, promise component format
		if( m_fDelayRenderComponents )
			m_pDataTransferTarget->PromiseData( kComponentFormat );

		// Otherwise, tell the component collection to copy component format
		else
			componentCollection.Copy( *m_pDataTransferTarget, kComponentFormat );

		// Now ask the collection to promise any other formats it supports
		componentCollection.PromiseFormats( *m_pDataTransferTarget );
		}

	catch( ... )
		{
		return FALSE;
		}

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RCopySelectionAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RCopySelectionAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RCopySelectionAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCopySelectionAction::Validate( ) const
	{
	RAction::Validate( );
	TpsAssertIsObject( RCopySelectionAction, this );
	}

#endif	// TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RCutSelectionAction::RCutSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCutSelectionAction::RCutSelectionAction( RCompositeSelection* pCurrentSelection )
	: RDeleteSelectionAction( pCurrentSelection )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RCutSelectionAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RCutSelectionAction::Do( )
	{
	// Create and do a copy selection action
	RClipboardDataTarget clipboard( new RComponentDataRenderer );
	RCopySelectionAction action( m_pCurrentSelection, &clipboard, FALSE );
	action.Do( );

	// Call the base method to do the rest of the work
	return RDeleteSelectionAction::Do( );
	}

// ****************************************************************************
//
//  Function Name:	RCutSelectionAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RCutSelectionAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RCutSelectionAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCutSelectionAction::Validate( ) const
	{
	RDeleteSelectionAction::Validate( );
	TpsAssertIsObject( RCutSelectionAction, this );
	}

#endif	//	TPSDEBUG
