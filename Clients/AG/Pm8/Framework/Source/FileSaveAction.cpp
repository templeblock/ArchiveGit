// ****************************************************************************
//
//  File Name:			FileSaveAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RFileSaveAction class
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
//  $Logfile:: /PM8/Framework/Source/FileSaveAction.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "FileSaveAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include "StandaloneDocument.h"
#include "ApplicationGlobals.h"
#include "View.h"

YActionId	RFileSaveAction::m_ActionId( "RFileSaveAction" );

// ****************************************************************************
//
//  Function Name:	RFileSaveAction::RFileSaveAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RFileSaveAction::RFileSaveAction( const RMBCString& rInitialFilename, const RMBCString& rFinalFilename )
	: RAction( m_ActionId ), m_rInitialFilename( rInitialFilename ), m_rFinalFilename( rFinalFilename )
	{ 
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RFileSaveAction::RFileSaveAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RFileSaveAction::RFileSaveAction( RScript& script )
	: RAction( m_ActionId, script )
	{
	script >> m_rInitialFilename;
	script >> m_rFinalFilename;
	}

// ****************************************************************************
//
//  Function Name:	RFileSaveAction::~RFileSaveAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFileSaveAction::~RFileSaveAction( )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RFileSaveAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RFileSaveAction::Do( )
	{
	TpsAssert( !m_rFinalFilename.IsEmpty( ), "invalid filename for new document" )

	try
		{
		// get the current document
		RView *pView = GetActiveView();
		TpsAssert( pView, "no active view to save" );
		RStandaloneDocument* pDocument = dynamic_cast<RStandaloneDocument*>( pView->GetRDocument() );
		TpsAssert( pDocument, "no active document to save" );

		//	verify that it has the same name that it did before it was closed when we created the script action
		TpsAssert( m_rInitialFilename == pDocument->GetDocumentFilename(), "saving the wrong document in RFileSaveAction::Do" );
		
		//	name and save it
		pDocument->SetDocumentFilename( m_rFinalFilename );
		
		pDocument->OnFileSave( FALSE );
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
//  Function Name:	RFileSaveAction::GetIntialFileName( )
//
//  Description:		returns the filename before the user may have changed it
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString& RFileSaveAction::GetInitialFileName( ) 
	{
	return m_rInitialFilename;
	}

// ****************************************************************************
//
//  Function Name:	RFileSaveAction::GetFinalFileName( )
//
//  Description:		returns the filename after the user may have changed it
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString& RFileSaveAction::GetFinalFileName( ) 
	{
	return m_rFinalFilename;
	}

// ****************************************************************************
//
//  Function Name:	RFileSaveAction::WriteScript( ) const
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RFileSaveAction::WriteScript( RScript& script ) const
	{
	script << const_cast<RFileSaveAction*>(this)->GetInitialFileName();
	script << const_cast<RFileSaveAction*>(this)->GetFinalFileName();
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RFileSaveAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
///
// ****************************************************************************
//
void RFileSaveAction::Validate( ) const
	{
	RAction::Validate( );
	TpsAssertIsObject( RFileSaveAction, this );
	}

#endif	//	TPSDEBUG
