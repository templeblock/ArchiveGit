// ****************************************************************************
//
//  File Name:			FileCloseAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RFileCloseAction class
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
//  $Logfile:: /PM8/Framework/Source/FileCloseAction.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "FileCloseAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include "StandaloneDocument.h"
#include "ApplicationGlobals.h"
#include "View.h"

YActionId	RFileCloseAction::m_ActionId( "RFileCloseAction" );

// ****************************************************************************
//
//  Function Name:	RFileCloseAction::RFileCloseAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RFileCloseAction::RFileCloseAction( const RMBCString& rInitialFilename )
	: RAction( m_ActionId ), m_rInitialFilename( rInitialFilename )
	{ 
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RFileCloseAction::RFileCloseAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RFileCloseAction::RFileCloseAction( RScript& script )
	: RAction( m_ActionId, script )
	{
	script >> m_rInitialFilename;
	}

// ****************************************************************************
//
//  Function Name:	RFileCloseAction::~RFileCloseAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFileCloseAction::~RFileCloseAction( )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RFileCloseAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RFileCloseAction::Do( )
	{
	TpsAssert( !m_rInitialFilename.IsEmpty( ), "invalid filename for new document" )

	try
		{
		// get the current document
		RView *pView = GetActiveView();
		TpsAssert( pView, "no active view to close" );
		RStandaloneDocument* pDocument = dynamic_cast<RStandaloneDocument*>( pView->GetRDocument() );
		TpsAssert( pDocument, "no active document to close" );

		//	verify that it has the same name that it did before it was closed when we created the script action
		TpsAssert( m_rInitialFilename == pDocument->GetDocumentFilename(), "closing the wrong document in RFileCloseAction::Do" );
		
		//	close it
		pDocument->OnClose( );
		}

	catch( ... )
		{
		// REVIEW: What else do we do here?
		return FALSE;
		}

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RFileCloseAction::GetIntialFileName( )
//
//  Description:		returns the filename before the user may have changed it
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString& RFileCloseAction::GetInitialFileName( ) 
	{
	return m_rInitialFilename;
	}

// ****************************************************************************
//
//  Function Name:	RFileCloseAction::WriteScript( ) const
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RFileCloseAction::WriteScript( RScript& script ) const
	{
	script << const_cast<RFileCloseAction*>(this)->GetInitialFileName();
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RFileCloseAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
///
// ****************************************************************************
//
void RFileCloseAction::Validate( ) const
	{
	RAction::Validate( );
	TpsAssertIsObject( RFileCloseAction, this );
	}

#endif	//	TPSDEBUG
