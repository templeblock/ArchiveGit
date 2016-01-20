// ****************************************************************************
//
//  File Name:			WindowsPictureDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RPictureDrawingSurface class
//
//  Portability:		Win32 dependent
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
//  $Logfile:: /PM8/Framework/Source/WindowsPictDrawingSurface.cpp            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "PictureDrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

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
	Close( );
	if( m_hMetafile )
		::DeleteEnhMetaFile( m_hMetafile );
	}

// ****************************************************************************
//
//  Function Name:	RPictureDrawingSurface::Initialize( )
//
//  Description:		Init
//
//  Returns:			TRUE if the metafile was sucessfully created and inited,
//							FALSE otherwise
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPictureDrawingSurface::Initialize( HDC hReferenceDC, const RIntRect& boundingRect )
	{
	// Create an enhanced metafile. The reference dc defines the DPI, and will be used
	// to handle attribute calls like ::GetDeviceCaps.
	HDC hMetafileDC = ::CreateEnhMetaFile( hReferenceDC, NULL, (const RECT*)&boundingRect, NULL );

	if( hMetafileDC )
		return RDcDrawingSurface::Initialize( hMetafileDC );
	else
		return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RPictureDrawingSurface::Close( )
//
//  Description:		Closes the metafile and gets the hMetafile. Once this
//							function has been called, the output and attribute DC's
//							are set to NULL, and no further drawing into the metafile
//							is allowed.
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
	if( !m_hMetafile )
		{
		m_hMetafile = ::CloseEnhMetaFile( m_hOutputDC );
		m_hOutputDC = m_hAttributeDC = NULL;
		}
	}

// ****************************************************************************
//
//  Function Name:	RPictureDrawingSurface::CopyToClipboard( )
//
//  Description:		Copies this metafile to the Windows clipboard. This
//							function closes the metafile, thus no more drawing operations
//							are allowed.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
//  TODO:				When the data transfer architecture is defined, rewrite
//							this code.
//
// ****************************************************************************
//
void RPictureDrawingSurface::CopyToClipboard( )
	{
	// Close the metafile
	Close( );

	// Open the clipboard
	if( ::OpenClipboard( NULL ) )
		{
		::EmptyClipboard( );

		// Make a copy of the metafile
		HENHMETAFILE hNewMetafile = ::CopyEnhMetaFile( m_hMetafile, NULL );

		// Set it into the clipboard
		::SetClipboardData( CF_ENHMETAFILE, hNewMetafile );

		::CloseClipboard( );
		}
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RPictureDrawingSurface::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPictureDrawingSurface::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RPictureDrawingSurface, this );
	}

#endif	// TPSDEBUG
