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
//  $Logfile:: /PM8/Framework/Source/WinPostScriptDrawingSurface.cpp          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************


#include "FrameworkIncludes.h"

ASSERTNAME

#include "PostScriptDrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

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
RPostScriptDrawingSurface::RPostScriptDrawingSurface( ) :
												PostscriptBaseClass( TRUE, TRUE ),
												m_pubCommandEnd( m_ubCommandBuffer + sizeof(uWORD) ),
												m_fPrefixed( FALSE ),
												m_fPostScripting( FALSE ),
												m_fPolyOpen( FALSE ),
												m_fPathOpen( FALSE ),
												m_ptPenPosition( 0, 0 )
	{
	*m_pubCommandEnd	= 0;//	This is null terminated for Windows
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
BOOLEAN RPostScriptDrawingSurface::Initialize( HDC hOutputDC, HDC hAttributeDC ) 
{
	if ( !PostscriptBaseClass::Initialize( hOutputDC, hAttributeDC ) )
		return FALSE;

	::MoveToEx( hOutputDC, 0, 0, NULL );

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
	NULL;
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
	return PostscriptBaseClass::GetClipRect();
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
	NULL;
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
void RPostScriptDrawingSurface::SendPostScript(  )
{
int		cnt	= int(m_pubCommandEnd-m_ubCommandBuffer);

	if( cnt > sizeof(uWORD) )
	{
		//
		// We have something to send...
		// Replace the space at the end of the line with a linefeed
		m_ubCommandBuffer[cnt-1]	= 0x0A;
		*reinterpret_cast<uWORD*>( m_ubCommandBuffer ) = static_cast<uWORD>( cnt - sizeof(uWORD) );
		int	escCmd					= POSTSCRIPT_PASSTHROUGH;
		if (Escape(m_hOutputDC, escCmd, cnt-sizeof(uWORD), (const char*)m_ubCommandBuffer, NULL) <= 0)
			throw	kUnknownError;
	}
		//
		//		Reset the command buffer and command end pointer + 2bytes for the length
		//		that must be written into the buffer
	m_pubCommandEnd	= m_ubCommandBuffer + sizeof(uWORD);
	*m_pubCommandEnd	= 0;
}

