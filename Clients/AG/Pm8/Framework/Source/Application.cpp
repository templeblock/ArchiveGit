// ****************************************************************************
//
//  File Name:			Application.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Definition of the RApplication class
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
//  $Logfile:: /PM8/Framework/Source/Application.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"Application.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ApplicationGlobals.h"
#include	"ComponentDocument.h"

#ifndef MAC // GCB 10/30/96 fix compile error on Mac (View.h not required for Windows either?)
#include	"View.h"
#endif

RApplicationGlobals*	RApplication::m_gsApplicationGlobals;

// ****************************************************************************
//
//  Function Name:	RApplication::RApplication( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RApplication::RApplication( )
	{
	tintStartup();
	}

// ****************************************************************************
//
//  Function Name:	RApplication::~RApplication( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RApplication::~RApplication( )
	{
	tintShutdown();
	}

// ****************************************************************************
//
//  Function Name:	RApplication::Initialize( )
//
//  Description:		Initializes the application instance.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RApplication::Initialize( RApplicationGlobals* pGlobals )
	{
#ifdef	TPSDEBUG
	// Startup TPS debug library
	DEBUGPARMS DebugParms;
#ifdef	_WINDOWS
	DebugParms.hwndApp = AfxGetMainWnd()->GetSafeHwnd();
	DebugParms.hinstApp = AfxGetInstanceHandle( );
#endif	//	_WINDOWS
	tdb_StartupDebugLib( &DebugParms, 0 );
#endif	// TPSDEBUG

	m_gsApplicationGlobals = pGlobals;

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RApplication::Shutdown( )
//
//  Description:		Terminates the application instance.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RApplication::Shutdown( )
	{
#ifdef	TPSDEBUG
	// Shutdown the TPS debug library
	tdb_ShutdownDebugLib( );
#endif	// TPSDEBUG
	}

// ****************************************************************************
//
//  Function Name:	RApplication::CreateScriptAction( )
//
//  Description:		Try to create an action from the current script
//
//  Returns:			The created action or NULL
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAction* RApplication::CreateScriptAction( const YActionId& /* actionId */, RScript& /* script */)
	{
	// Apparently no one wants this command. Return NULL (No action created)
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RApplication::OnXPrinterChanged( const RMBCString& rPrinterChangeInfo )
//
//  Description:		Called when the given printer's charecteristics changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RApplication::OnXPrinterChanged( const RMBCString& /*rPrinterChangeInfo*/ )
	{
	UnimplementedCode();
	}

// ****************************************************************************
//
//  Function Name:	RApplication::OnXPreferencesChanged( const RMBCString& rPreferencesChangedInfo )
//
//  Description:		Called when the preferences (for example win.ini) changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RApplication::OnXPreferencesChanged( const RMBCString& /*rPreferencesChangedInfo*/ )
	{
	UnimplementedCode();
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RApplication::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RApplication::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RApplication, this );
	}

#endif	// TPSDEBUG
