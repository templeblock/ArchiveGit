// ****************************************************************************
//
//  File Name:			AutoDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the ROffscreenDrawingSurface class
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
//  $Logfile:: /PM8/Framework/Source/AutoDrawingSurface.cpp                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "AutoDrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ApplicationGlobals.h"

// ****************************************************************************
//
//  Function Name:	RAutoDrawingSurface::RAutoDrawingSurface( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAutoDrawingSurface::RAutoDrawingSurface( )
	: BaseDrawingSurface( ),
	  m_rOffscreenBitmap( FALSE ),
	  m_OffscreenSize( 0, 0 ),
	  m_Bounds( 0,0,0,0 ),
	  m_pDestinationDrawingSurface( NULL ),
	  m_fInUse( FALSE ),
	  m_fSetup( FALSE )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RAutoDrawingSurface::~RAutoDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAutoDrawingSurface::~RAutoDrawingSurface( )
{
	TpsAssert(!m_pDestinationDrawingSurface, "Prepare called, but not Release...");
}
												

// ****************************************************************************
//
//  Function Name:	RAutoDrawingSurface::Prepare( )
//
//  Description:		Setups the AutoDrawingSurface to the proper internal state
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RAutoDrawingSurface::Prepare( RDrawingSurface* pSurface, R2dTransform& transform, const RRealRect& rcRender )
{
	//	Check application globals
	if ( !::GetUseOffscreens( ) )
		return FALSE;

	TpsAssert( pSurface, "Initializing a AutoDrawingSurface with a NULL surface" );
	TpsAssertIsObject( RDcDrawingSurface, pSurface );

	//	Don't use the offscreen for rotated or flipped drawing. We would have to
	// rotate the bitmap. This would be too expensive.
	YAngle			rotation;
	YRealDimension	xScale;
	YRealDimension	yScale;
	BOOLEAN	fFlipped = transform.Decompose( rotation, xScale, yScale );
	if ( m_fInUse || (rotation != 0.0) || fFlipped )
		return FALSE;

	if ( !m_fSetup )
	{
#ifdef	_WINDOWS
		HDC	hdc				= GetDC( HWND_DESKTOP );
		if ( hdc )
		{
			HDC	hdcOffscreen	= ::CreateCompatibleDC( hdc );
			if ( hdcOffscreen )
			{
				m_OffscreenSurface.Initialize( hdcOffscreen, hdcOffscreen );
				Initialize( hdcOffscreen, hdcOffscreen );
				::DeleteDC( hdcOffscreen );
			}
			::ReleaseDC( HWND_DESKTOP, hdc );
		}
#endif	//	_WINDOWS
	//	m_fSetup = TRUE;
	}

	m_fInUse = TRUE;

	//	See if we need to create a New offscreen
	RIntSize	offscreenSize	= rcRender.WidthHeight( );
	if ( (m_OffscreenSize.m_dx < offscreenSize.m_dx) || (m_OffscreenSize.m_dy < offscreenSize.m_dy) )
	{
		try
		{
			m_rOffscreenBitmap.Initialize( *pSurface, offscreenSize.m_dx, offscreenSize.m_dy );
			m_OffscreenSize = offscreenSize;
		}
		catch( ... )
		{
			m_fInUse = FALSE;
			return FALSE;
		}
	}

	//
	//	The offscreen is now setup so adjust the transform and remember the bounds
	//	Remember our drawing state.
	m_OffscreenSurface.SetImage( &m_rOffscreenBitmap );
	m_pDestinationDrawingSurface	= pSurface;
	m_Bounds								= rcRender;

	//	Convert rcRender into logical units
	R2dTransform invTransform		= transform;
	invTransform.Invert();
	RIntRect logicalRect	= rcRender * invTransform;

#if 0
	//	calculate offscreen W/H
	RRealRect	transformedBounds = logicalRect * transform;
	RIntRect		intBounds(YIntDimension(transformedBounds.m_Left), YIntDimension(transformedBounds.m_Top),
								::Round(transformedBounds.m_Right), ::Round(transformedBounds.m_Bottom));

	//	Remember the transformed bounds for output later.
	m_Bounds = intBounds;
#endif

	RRealRect	psudoBounds(m_Bounds);
	psudoBounds *= invTransform;

	//	Compute transform for offscreen imaging
	R2dTransform	rOffscreenTransform;
	rOffscreenTransform.PostTranslate(-logicalRect.m_Left, -logicalRect.m_Top);
	rOffscreenTransform.PostTranslate(logicalRect.m_Left-psudoBounds.m_Left, logicalRect.m_Top-psudoBounds.m_Top);
	rOffscreenTransform.PostScale(xScale, yScale);
	transform = rOffscreenTransform;

#ifdef _WINDOWS
	HDC hdc = (HDC)m_OffscreenSurface.GetSurface();
	Initialize( hdc, hdc );
	RestoreDefaults( );
#endif // _WINDOWS

#ifdef MAC
	#error No Mac Implementation for now
#endif // MAC

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RAutoDrawingSurface::Release( )
//
//  Description:		Done with the AutoDrawingSurface, blit it to the screen
//							if it was actually an offscreen.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RAutoDrawingSurface::Release()
	{
	TpsAssert( m_fInUse && (m_pDestinationDrawingSurface != NULL), "Release called but the drawing surface was not prepared.");

	try
		{
		if ( m_pDestinationDrawingSurface )
			{
			//	Blit offscreen to screen		
			RIntRect	srcIntRect( 0, 0, m_Bounds.Width(), m_Bounds.Height() );
			m_pDestinationDrawingSurface->BlitDrawingSurface( *this, srcIntRect, m_Bounds, kBlitSourceCopy );
			m_pDestinationDrawingSurface = NULL;
			DetachDCs( );
			RImage* pImage = m_OffscreenSurface.ReleaseImage( );
			TpsAssert( pImage == &m_rOffscreenBitmap, "Image releases was not the one I set" );
			}
		else
			DetachDCs( );
		}
	catch( ... )
		{
		//	REVIEW ESV 2/4/97 - what do we do here?
		}
	
	//	Our resources have been deallocated so it is alright to try again with this surface.
	m_fInUse = FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RAutoDrawingSurface::Reset( )
//
//  Description:		Resets the autodrawing surface. Used when the display 
//							changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RAutoDrawingSurface::Reset( )
	{
	//	REVIEW ESV 2/4/97 what do we do here?
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RAutoDrawingSurface::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RAutoDrawingSurface::Validate() const
{
	BaseDrawingSurface::Validate( );
	TpsAssertIsObject( RAutoDrawingSurface, this );
}

#endif TPSDEBUG
