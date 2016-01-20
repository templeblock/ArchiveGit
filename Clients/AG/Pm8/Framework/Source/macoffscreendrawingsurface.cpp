// ****************************************************************************
//
//  File Name:			MacOffscreenDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Declaration of the ROffscreenDrawingSurface class
//
//  Portability:		Mac platform dependent
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
//  $Logfile:: /PM8/Framework/Source/macoffscreendrawingsurface.cpp           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "OffscreenDrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "BitmapImage.h"
#include "VectorImage.h"


// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::ROffscreenDrawingSurface()
//
//  Description:		Creates an offscreen DS compatible with the screen.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
ROffscreenDrawingSurface::ROffscreenDrawingSurface( BOOLEAN fIsPrinting ) : m_pBitmapImage( NULL ),
																								 	 m_pVectorImage( NULL ),
																								 	 m_ClipRect( 0, 0, 0, 0 ),
																									 m_pSavedGrafPort( NULL )
{
	m_pImage = NULL;	
	::GetPort( &m_pSavedGrafPort );
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::~ROffscreenDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
ROffscreenDrawingSurface::~ROffscreenDrawingSurface( )
{
	TpsAssert( ( m_pImage == NULL ), "Did not release image." );	
	if ( m_pSavedGrafPort != NULL )
		::SetPort( m_pSavedGrafPort );
}
	
// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::SetImage()
//
//  Description:		Sets an image into the ODS
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
void ROffscreenDrawingSurface::SetImage( RImage* pImage )
{	
	TpsAssert( pImage, "Nil image pointer." );

	// Check to see if we've got an RBitmapImage or an RVectorImage
	RBitmapImage* pBitmapImage = dynamic_cast<RBitmapImage*>( pImage );
	if ( pBitmapImage != NULL ) 
	{
		SetBitmapImage( pBitmapImage );
	}

	RVectorImage* pVectorImage = dynamic_cast<RVectorImage*>( pImage );
	if ( pVectorImage != NULL ) 
	{
		SetVectorImage( pVectorImage );
	}

	if ( !m_ClipRect.IsEmpty() ) 
		SetClipRect( m_ClipRect );
}
	
// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::SetBitmapImage()
//
//  Description:		Sets a bitmap image into the ODS
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
void ROffscreenDrawingSurface::SetBitmapImage( RBitmapImage* pBitmapImage )
{
	TpsAssert( ( pBitmapImage != NULL ), "Nil bitmap image pointer." );
	TpsAssert( ( m_pImage == NULL ), "Previous image not released." );
	TpsAssert( ( m_pGrafPort == NULL ), "Previous port not freed." );

	// init drawing surface
	m_pGrafPort = static_cast<GrafPtr>( pBitmapImage->Lock() );
	if ( m_pGrafPort == NULL )
		throw kMemory;
	BaseDrawingSurface::Initialize( m_pGrafPort );

	// set member data
	m_pImage = pBitmapImage;
	m_pBitmapImage = pBitmapImage;
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::SetVectorImage()
//
//  Description:		Sets a vector image into the ODS
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
void ROffscreenDrawingSurface::SetVectorImage( RVectorImage* pVectorImage )
{
	TpsAssert( ( pVectorImage != NULL ), "Nil vector image pointer." );
	TpsAssert( ( m_pImage == NULL ), "Previous image not released." );
	
	UnimplementedCode();

	// set member data
	m_pImage = pVectorImage;
	m_pVectorImage = pVectorImage;
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::ReleaseImage()
//
//  Description:		Releases an RImage from the DS
//
//  Returns:			A pointer to the RImage
//
//  Exceptions:		None
//
// ****************************************************************************
RImage* ROffscreenDrawingSurface::ReleaseImage( )
{
RImage*	pTempImage = m_pImage;

	TpsAssert( ( m_pImage != NULL ), "Nil image pointer." );

	if ( m_pBitmapImage != NULL ) 
		if ( m_pGrafPort != NULL ) 
		{
			m_ClipRect = GetClipRect();
			m_pBitmapImage->Unlock();
			m_pGrafPort = NULL;
			m_pImage = m_pBitmapImage = NULL;
		}

	if ( m_pVectorImage != NULL ) 
	{



		m_pImage = m_pVectorImage = NULL;
	}

	return pTempImage;
}

// ****************************************************************************
//
//  Function Name:	RImage::ThrowExceptionOnError()
//
//  Description:		Throws a Renaissance exception based on the value
//							returned from the Win32 call ::GetLastError()
//
//  Returns:			
//
//  Exceptions:		kMemory; kDiskFull; kUnknownError
//
// ****************************************************************************
void ROffscreenDrawingSurface::ThrowExceptionOnError( )
{
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::GetBitDepth( )
//
//  Description:		Gets the bit depth of the current off screen image
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
uLONG ROffscreenDrawingSurface::GetBitDepth() const
{
	if ( m_pImage != NULL )
		if ( m_pBitmapImage != NULL )
			return m_pBitmapImage->GetBitDepth();
	return 0;
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::GetDPI( )
//
//  Description:		Gets the DPI of the offscreen image
//
//  Returns:			A size object containing the image DPI
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize ROffscreenDrawingSurface::GetDPI( ) const
{
RIntSize		dpi( 0, 0 );

	if ( m_pImage != NULL )
		if ( m_pBitmapImage != NULL )
		{
			dpi.m_dx = m_pBitmapImage->GetXDpi();
			dpi.m_dy = m_pBitmapImage->GetYDpi();
		}
		else if ( m_pVectorImage != NULL )
		{
	//		dpi.m_dx = m_pVectorImage->GetXDpi();
	//		dpi.m_dy = m_pVectorImage->GetYDpi();
		}
	return dpi;
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::GetSurface( )
//
//  Description:		Return an identifier to the Offscreen Drawing Surface.
//
//  Returns:			m_pGrafPort
//
//  Exceptions:		None
//
// ****************************************************************************
//
long ROffscreenDrawingSurface::GetSurface( ) const
{
	return (long)m_pGrafPort;
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::GetClipRect( )
//
//  Description:		Gets the tightest bounding rectangle around the current
//							clipping region.
//
//  Returns:			A rect containing the current clip rect.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect ROffscreenDrawingSurface::GetClipRect( ) const
{
GrafPtr		pGrafPort = (GrafPtr)GetSurface();
Rect 			clipRect;

	if ( pGrafPort == NULL ) 
		return m_ClipRect;
	else
		clipRect = (**pGrafPort->clipRgn).rgnBBox;

	return RIntRect( clipRect.left, clipRect.top, clipRect.right, clipRect.bottom );
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::SetClipRect( )
//
//  Description:		Sets the current clip rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void ROffscreenDrawingSurface::SetClipRect( const RIntRect& clipRect )
{
GrafPtr		pGrafPort = (GrafPtr)GetSurface();
GrafPtr		holdPort;
Rect			qdRect;

	TpsAssert( ( !clipRect.IsEmpty() ), "Empty clip rect." );
	if ( pGrafPort == NULL ) 
		m_ClipRect = clipRect;
	else
	{
		::GetPort( &holdPort );   
		::SetPort( pGrafPort );
		::SetRect( &qdRect, clipRect.m_Left, clipRect.m_Top, clipRect.m_Right, clipRect.m_Bottom );
		::ClipRect( &qdRect );
		::SetPort( holdPort );
		m_ClipRect = RIntRect( 0, 0, 0, 0 );
	}
}

