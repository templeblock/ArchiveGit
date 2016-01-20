// ****************************************************************************
//
//  File Name:			ViewDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RViewDrawingSurface class
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
//  $Logfile:: /PM8/Framework/Source/ViewDrawingSurface.cpp                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "ViewDrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "View.h"

// ****************************************************************************
//
//  Function Name:	RViewDrawingSurface::RViewDrawingSurface( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RViewDrawingSurface::RViewDrawingSurface( const RView* pView )
	: BaseDrawingSurface( ),
	  m_pView( pView )
	{
	TpsAssert( pView, "Initializing a ViewDrawingSurface with a NULL view" );

#ifdef	_WINDOWS
	HWND hwnd	= pView->GetCWnd( ).GetSafeHwnd( );
	TpsAssert( hwnd, "The HWND of the pView did not exist" );

	HDC hdc = ::GetDC(hwnd);
	TpsAssert(	hdc, "Could not retrieve the HDC from the HWND" );

	if( !BaseDrawingSurface::Initialize( hdc, hdc ) )
		::ThrowException( kMemory );
#else	//	!_WINDOWS must be MAC
	LCommandoView& commandoView = pView->GetCommandoView();
	GrafPtr pGrafPort = commandoView.GetMacPort();
	if(  !BaseDrawingSurface::Initialize( pGrafPort ) )
		::ThrowException( kMemory );
#endif	//	_WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RViewDrawingSurface::~RViewDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RViewDrawingSurface::~RViewDrawingSurface( )
	{
#ifdef	_WINDOWS
	if (m_pView && m_hOutputDC)
		{
		// Save copies of the DC
		HDC outputDC = m_hOutputDC;
						 
		// Detach the DCs to clean up
		DetachDCs( );

		// Release the output DC
		HWND hwnd = m_pView->GetCWnd().GetSafeHwnd();
		if( hwnd )
			::ReleaseDC( hwnd, outputDC );
		}
#else	//	!_WINDOWS must be MAC
	;
#endif	//	_WINDOWS
	}
												
#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RViewDrawingSurface::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RViewDrawingSurface::Validate( ) const
	{
	BaseDrawingSurface::Validate( );
	TpsAssertIsObject( RViewDrawingSurface, this );
	}

#endif	// TPSDEBUG
