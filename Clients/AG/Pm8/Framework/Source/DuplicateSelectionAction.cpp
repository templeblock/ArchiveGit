// ****************************************************************************
//
//  File Name:			DuplicateSelectionAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RDuplicateSelectionAction class
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
//  $Logfile:: /PM8/Framework/Source/DuplicateSelectionAction.cpp             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "DuplicateSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ComponentView.h"
#include "DataTransfer.h"
#include "ComponentAttributes.h"

YActionId RDuplicateSelectionAction::m_ActionId( "RDuplicateSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RDuplicateSelectionAction::RDuplicateSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDuplicateSelectionAction::RDuplicateSelectionAction( RCompositeSelection* pCurrentSelection )
	: RPasteActionBase( pCurrentSelection->GetView( )->GetRDocument( ), m_ActionId )
	{
	Initialize( );
	}

// ****************************************************************************
//
//  Function Name:	RDuplicateSelectionAction::RDuplicateSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDuplicateSelectionAction::RDuplicateSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: RPasteActionBase( pCurrentSelection->GetView( )->GetRDocument( ), script, m_ActionId )
	{
	Initialize( );
	}

// ****************************************************************************
//
//  Function Name:	RDuplicateSelectionAction::Initialize( )
//
//  Description:		Common initialization code. Should only be called from a
//							ctor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDuplicateSelectionAction::Initialize( )
	{
	// Sort the selection by z-order
	m_pCurrentSelection->SortByZOrder( );

	RComponentCollection sourceComponentCollection;

	// Iterate through the selection, looking for components which are not unique
	YSelectionIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( ( *iterator )->GetComponentAttributes( ).GetUniqueId( ) == kNotUnique )
			sourceComponentCollection.InsertAtEnd( ( *iterator )->GetComponentDocument( ) );

	// Copy the components
	BOOLEAN fUnused;
	m_PastedComponentCollection = RComponentCollection( sourceComponentCollection, m_pDocument, fUnused );

	RPasteActionBase::Initialize( m_pCurrentSelection->GetView( )->GetInsertionPoint( m_PastedComponentCollection.GetBoundingRect( ).WidthHeight( ) ) );
	}
