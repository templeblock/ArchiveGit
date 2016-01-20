// ****************************************************************************
//
//  File Name:			FilePrintAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RFilePrintAction class
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
//  $Logfile:: /PM8/Framework/Source/FilePrintAction.cpp                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "FilePrintAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "StandaloneDocument.h"
#include "StandaloneView.h"
#include "ApplicationGlobals.h"
#include "Script.h"

YActionId	RFilePrintAction::m_ActionId( "RFilePrintAction" );

// ****************************************************************************
//
//  Function Name:	RFilePrintAction::RFilePrintAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RFilePrintAction::RFilePrintAction( const RMBCString& rInitialFilename, const RMBCString& rFinalFilename )
	: RAction( m_ActionId ), m_rInitialFilename( rInitialFilename ), m_rFinalFilename( rFinalFilename )
	{ 
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RFilePrintAction::RFilePrintAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RFilePrintAction::RFilePrintAction( RScript& script )
	: RAction( m_ActionId, script )
	{
	script >> m_rInitialFilename;
	script >> m_rFinalFilename;
	}

// ****************************************************************************
//
//  Function Name:	RFilePrintAction::~RFilePrintAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFilePrintAction::~RFilePrintAction( )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RFilePrintAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RFilePrintAction::Do( )
	{
	TpsAssert( !m_rInitialFilename.IsEmpty( ), "invalid filename for new document" )

	try
		{
		// get the current document
		RStandaloneView *pView = dynamic_cast<RStandaloneView*>( ::GetActiveView() );
		TpsAssert( pView, "no active view to print" );
		RStandaloneDocument* pDocument = dynamic_cast<RStandaloneDocument*>( pView->GetRDocument() );
		TpsAssert( pDocument, "no active document to print" );

		//	verify that it has the same name that it did before it was printed when we created the script action
		TpsAssert( m_rInitialFilename == pDocument->GetDocumentFilename(), "printing the wrong document in RFilePrintAction::Do" );
		
		//	print it
		//WinCode( pView->OnFilePrint( ) );
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
//  Function Name:	RFilePrintAction::GetIntialFileName( )
//
//  Description:		returns the filename before the user may have changed it
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString& RFilePrintAction::GetInitialFileName( ) 
	{
	return m_rInitialFilename;
	}

// ****************************************************************************
//
//  Function Name:	RFilePrintAction::GetFinalFileName( )
//
//  Description:		returns the filename after the user may have changed it
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString& RFilePrintAction::GetFinalFileName( ) 
	{
	return m_rFinalFilename;
	}

// ****************************************************************************
//
//  Function Name:	RFilePrintAction::WriteScript( ) const
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RFilePrintAction::WriteScript( RScript& script ) const
	{
	script << const_cast<RFilePrintAction*>(this)->GetInitialFileName();
	script << const_cast<RFilePrintAction*>(this)->GetFinalFileName();
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RFilePrintAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
///
// ****************************************************************************
//
void RFilePrintAction::Validate( ) const
	{
	RAction::Validate( );
	TpsAssertIsObject( RFilePrintAction, this );
	}

#endif	//	TPSDEBUG
