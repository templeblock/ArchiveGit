// ****************************************************************************
//
//  File Name:			MacPostScriptDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Definition of the RPostScriptDrawingSurface class
//
//  Portability:		Macintosh independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/MacPostScriptDrawingSurface.cpp          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************


#include "FrameworkIncludes.h"

ASSERTNAME

#include "PostScriptDrawingSurface.h"

#ifndef	IMAGER
#include "ApplicationGlobals.h"
#else
#include "GlobalBuffer.h"
#endif

#include "Bezier.h"
#include "QuadraticSpline.h"


// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::RPostScriptDrawingSurface( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPostScriptDrawingSurface::RPostScriptDrawingSurface( const THPrint hPrintInfo ) : 
												PostscriptBaseClass( hPrintInfo ),
												m_pubCommandEnd( m_ubCommandBuffer ),
												m_fPrefixed( FALSE ),
												m_fPostScripting( FALSE ),
												m_fPolyOpen( FALSE ),
												m_fPathOpen( FALSE ),
												m_ptPenPosition( 0, 0 )
	{
	TpsAssert( hPrintInfo, "NULL print info handle" );

	//	Null terminate the string as appropriate for the given platform
	m_ubCommandBuffer[0] = 0;	//	This is null terminated the string
	}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::Initialize( )
//
//  Description:		Initialization
//
//  Returns:			Boolean 
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPostScriptDrawingSurface::Initialize( const GrafPtr pPort, const RIntRect& boundingRect, const BOOLEAN fUseMaxResolution ) 
{
	if ( !PostscriptBaseClass::Initialize( pPort, boundingRect, fUseMaxResolution ) )
		return FALSE;
	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::StartupPostScript( )
//
//  Description:		Start to send postscript commands
//
//  Returns:			Nothing 
//
//  Exceptions:		throws memory exception
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::StartupPostScript( void )
{
	TpsAssert( !m_fPostScripting, "PostScript nesting is illegal." );
	::PicComment( kPicPostScriptBegin, 0, NULL );
	::PicComment( kPicLineLayoutOff, 0, NULL );
	m_fPostScripting = TRUE;
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::ShutdownPostScript( )
//
//  Description:		End of postscript commands
//
//  Returns:			Nothing 
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::ShutdownPostScript( void )
{
	TpsAssert( m_fPostScripting, "PostScript never started up." );
	if ( m_fPostScripting )
	{
		::PicComment( kPicPostScriptEnd, 0, NULL );
		m_fPostScripting = FALSE;
	}
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::GetClipRect( )
//
//  Description:		Return the ClipRect of the device
//
//  Returns:			Nothing 
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect RPostScriptDrawingSurface::GetClipRect( void ) const
{
	Rect	rect = (**m_hPrintInfo).prInfo.rPage;
	return RIntRect( rect.left, rect.top, rect.right, rect.bottom );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::SendPostScript( )
//
//  Description:		
//
//  Returns:			Nothing 
//
//  Exceptions:		throws memory exception
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::SendPostScript( )
{
Handle	hdl;
OSErr		osErr;
int		cnt	= int(m_pubCommandEnd-m_ubCommandBuffer);

	if( cnt > 0 )
	{
		//
		// We have something to send...
		// Replace the space at the end of the line with a (cr)
		m_ubCommandBuffer[cnt-1] = 0x0D;

		// create a handle from the pointer data
		osErr = ::PtrToHand( m_ubCommandBuffer, &hdl, cnt );
		if ( hdl == NULL || osErr != noErr )
			throw kMemory;

		// send the command
		::PicComment( kPicPostScriptHandle, cnt, hdl);

		// cleanup
		::DisposeHandle( hdl );
	}

	//
	//		Reset the command buffer and command end pointer
	m_ubCommandBuffer[0] = 0;
	m_pubCommandEnd		= m_ubCommandBuffer;
}

