// ****************************************************************************
//
//  File Name:			DcDrawingSurface.inl
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas, M. Dunlap
//
//  Description:		Inlines for the RDcDrawingSurface class
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /Development/Framework/Include/DcDrawingSurface.h              $
//   $Author:: Mgd                                                            $
//     $Date:: 10/07/96 10:01a                                                $
// $Revision:: 20                                                             $
//
// ****************************************************************************


#ifdef TPSDEBUG
#define	INLINE
#else
#define	INLINE	inline
#endif

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::IsVisible( const RIntPoint& pt )
//
//  Description:		return TRUE if the point is visible else FALSE
//
//  Returns:			BOOL fVisible
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDcDrawingSurface::IsVisible( const RIntPoint& pt )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );
	return ::PtVisible( m_hOutputDC, pt.m_x, pt.m_y );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::IsVisible( const RIntRect& rc )
//
//  Description:		return TRUE if the rect is visible else FALSE
//
//  Returns:			BOOL fVisible
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDcDrawingSurface::IsVisible( const RIntRect& rc )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );
	CRect	rc2( rc );
	return ::RectVisible( m_hOutputDC, &rc2 );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::HasPathSupport( )
//
//  Description:		Called to determine if this surface has path support.
//							Path support includes calls to PolyBezierTo.
//
//							Win32 contains support for paths, however we do not always
//							want to use it, for two reasons. First it is a bit slow,
//							and second it does not allow us to cache points. However,
//							in the case of a printer or metafile, we do want to use the
//							Win32 path APIs. So we are using m_fUsePathSupport to
//							distinguish the two cases.
//
//  Returns:			TRUE if the surface has path support, FALSE otherwise.
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDcDrawingSurface::HasPathSupport( ) const
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	return m_fUsePathSupport;
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::BeginPath( )
//
//  Description:		Begins a path
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDcDrawingSurface::BeginPath( )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );
	TpsAssert( m_fUsePathSupport == TRUE, "Path support should not be used on this surface." );

	::BeginPath( m_hOutputDC );
	}


// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::BeginOpenPath( )
//
//  Description:		Not used on Windows
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDcDrawingSurface::BeginOpenPath( )
	{
	;
	}


// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::EndPath( )
//
//  Description:		Ends a path
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDcDrawingSurface::EndPath( )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );
	TpsAssert( m_fUsePathSupport == TRUE, "Path support should not be used on this surface." );

	::EndPath( m_hOutputDC );
	}


// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::EndOpenPath( )
//
//  Description:		Not used on Windows
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDcDrawingSurface::EndOpenPath( )
	{
	;
	}


// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::GetSurface( )
//
//  Description:		Return an identifier to the Output Drawing Surface.
//
//  Returns:			m_hOutputDC
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE long RDcDrawingSurface::GetSurface( ) const
	{
	return (long)m_hOutputDC;
	}
