// ****************************************************************************
//
//  File Name:			MacStandaloneApplication.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RStandaloneApplication class
//
//  Portability:		Mac Specific
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
//  $Logfile:: /PM8/Framework/Source/MacStandaloneApplication.cpp             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"StandaloneApplication.h"
#include	"StandaloneDocument.h"
#include	"StandaloneView.h"

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::~RStandaloneApplication( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneApplication::~RStandaloneApplication( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::Initialize( )
//
//  Description:		Initializes the application instance
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void RStandaloneApplication::Initialize( )
	{
	// Load the component servers
	LoadComponentServers( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::Shutdown( )
//
//  Description:		Terminates the application instance
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::Shutdown( )
	{
	// Unload the component servers
	UnloadComponentServers( );

	// Call the base class
	RApplication::Shutdown( );
	}


// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::StartUp( )
//
//  Description:		Called on the creation of the application to perform any
//							intial processing.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneApplication::StartUp()
	{
	;//ObeyCommand(cmd_New, nil);		// For starters..., do nothing
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::ObeyCommand( )
//
//  Description:		Respond to Menu Commands.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
Boolean
RStandaloneApplication::ObeyCommand(
	CommandT	inCommand,
	void		*ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {
	
		// Deal with command messages (defined in PP_Messages.h).
		// Any that you don't handle will be passed to LApplication
 			
		case cmd_New:
			CreateNewDocument( );
			break;

		default:
			cmdHandled = LApplication::ObeyCommand(inCommand, ioParam);
			break;
	}
	
	return cmdHandled;
}

// ****************************************************************************
//
//  Function Name:	RStandaloneApplication::FindCommandStatus( )
//
//  Description:		Calculate the status of the given menu items.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void
RStandaloneApplication::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	Char16		&outMark,
	Str255		outName)
{
	switch (inCommand) {
	
		// Return menu item status according to command messages.
		// Any that you don't handle will be passed to LApplication

		case cmd_New:					// EXAMPLE
			outEnabled = true;			// enable the New command
			break;

		default:
			LApplication::FindCommandStatus(inCommand, outEnabled,
												outUsesMark, outMark, outName);
			break;
	}
}

