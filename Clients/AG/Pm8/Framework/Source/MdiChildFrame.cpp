// ****************************************************************************
//
//  File Name:			MdiChildFrame.cpp
//
//  Project:			Renaissance application
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RMdiChildFrame class
//
//  Portability:		Windows, MFC specific
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
//  $Logfile:: /PM8/Framework/Source/MdiChildFrame.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_WINDOWS
	#error "This file should only be compiled under Windows"
#endif

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"MdiChildFrame.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "StandaloneView.h"
#include "StandaloneApplication.h"

IMPLEMENT_DYNCREATE( RMdiChildFrame, CMDIChildWnd )

BEGIN_MESSAGE_MAP( RMdiChildFrame, CMDIChildWnd )
	ON_WM_MDIACTIVATE( )
END_MESSAGE_MAP( )

// ****************************************************************************
//
//  Function Name:	RMdiChildFrame::OnMDIActivate( )
//
//  Description:		Called during window creation
//
//  Returns:			0:		Continue window creation
//							-1:	Terminate the window
//
//  Exceptions:		None
//
// ****************************************************************************
//
afx_msg void RMdiChildFrame::OnMDIActivate( BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd )
	{
	// Call the base method
	CMDIChildWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );

	// Give the message to our view
	TpsAssertIsObject( RStandaloneView, GetActiveView( ) );
	RStandaloneView* pView = static_cast<RStandaloneView*>( GetActiveView( ) );
	pView->OnXActivate( BOOLEAN( bActivate ) );
	}

// ****************************************************************************
//
//  Function Name:	RMdiChildFrame::ActivateFrame( )
//
//  Description:		Call this member function to activate and restore the frame
//							window so that it is visible and available to the user
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMdiChildFrame::ActivateFrame( int nCmdShow )
	{
	// If the show command is the MFC default, we need to do some work
	if( nCmdShow == -1 )
		{
		// If there are other documents in the system, just use the default
		if( ::AfxGetApp( )->GetOpenDocumentCount( ) == 1 )
			{
			// There are no open documents. If the last closed window was maximized,
			// maximize this window
			if( ::GetApplication( ).WasLastClosedWindowMaximized( ) )
				nCmdShow = SW_SHOWMAXIMIZED;
			}
		}

	// Call the base method
	CMDIChildWnd::ActivateFrame( nCmdShow );
	}
