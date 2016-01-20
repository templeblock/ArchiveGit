// ****************************************************************************
//
//  File Name:			MacPictureDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Declaration of the RPictureDrawingSurface class
//
//  Portability:		Macintosh dependent
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
//  $Logfile:: /PM8/Framework/Source/MacPictureDrawingSurface.cpp             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "PictureDrawingSurface.h"

// ****************************************************************************
//
//  Function Name:	RPictureDrawingSurface::~RPictureDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPictureDrawingSurface::~RPictureDrawingSurface( )
{
	Close();
	if( m_hPict != NULL )
	{
		::KillPicture( m_hPict );
		m_hPict = NULL;
	}
	if( m_pGrafPort != NULL )
	{
		::CloseCPort( m_pGrafPort );
		delete m_pGrafPort;
		m_pGrafPort = NULL;
	}
}

// ****************************************************************************
//
//  Function Name:	RPictureDrawingSurface::Initialize( )
//
//  Description:		Initialization
//
//  Returns:			Boolean indicating successful completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPictureDrawingSurface::Initialize( GrafPtr pPort, const RIntRect& boundingRect ) 
{
OpenCPicParams	info;
Rect				rect;
RIntSize 		dpi = GetDPI();

	TpsAssert( pPort, "NULL graf port ptr" );
	if( !RGpDrawingSurface::Initialize( pPort ) )
		return FALSE;
	m_rcBounds = boundingRect;
	::SetRect( &rect, boundingRect.m_Left, boundingRect.m_Top, boundingRect.m_Right, boundingRect.m_Bottom);

	m_pGrafPort = new CGrafPort;
	::OpenCPort( m_pGrafPort );
	::ClipRect( &rect );													// see Tech Note QD 11

	memset( &info, 0, sizeof( OpenCPicParams ) );				// always zero param blocks
	info.srcRect = rect;
	info.hRes = Long2Fix( dpi.m_dx );
	info.vRes = Long2Fix( dpi.m_dy );
	info.version = -2;

	if ( (m_hPict = ::OpenCPicture( &info )) == NULL )
	{
	 	::CloseCPort( m_pGrafPort );
		delete m_pGrafPort;
		m_pGrafPort = NULL;
		::SetPort( pPort );
	}

	return( m_hPict != NULL );
}


// ****************************************************************************
//
//  Function Name:	RPictureDrawingSurface::Close( )
//
//  Description:		Closes the pict. Once this function has been called no 
//							further drawing into the pict is allowed.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPictureDrawingSurface::Close( )
{
	// Dont close if we have already closed
	if( m_hPict != NULL )
		if( m_pGrafPort != NULL )
		{
			::ClosePicture();
			::CloseCPort( m_pGrafPort );
			delete m_pGrafPort;
			m_pGrafPort = NULL;
		}
}


// ****************************************************************************
//
//  Function Name:	RPictureDrawingSurface::CopyToClipboard( )
//
//  Description:		Copies this pict to the mac clipboard.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
//  TODO:				When the data transform architecture is defined, rewrite
//							this code.
//
// ****************************************************************************
//
void RPictureDrawingSurface::CopyToClipboard( )
{
	Close();
	::ZeroScrap();
	if( m_hPict != NULL )
	{
		::HLock( (Handle)m_hPict );
		(void)::PutScrap( ::GetHandleSize( (Handle)m_hPict ), 'PICT', *m_hPict );
		::HUnlock( (Handle)m_hPict );
	}
}


#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RPictureDrawingSurface::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPictureDrawingSurface::Validate( ) const
{
	RGpDrawingSurface::Validate( );
	TpsAssertIsObject( RPictureDrawingSurface, this );
}

#endif	// TPSDEBUG
