// ****************************************************************************
//
//  File Name:			DrawingSurface.inl
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas, M. Houle, M. Dunlap
//
//  Description:		inlines for the RDrawingSurface class
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
//  $Logfile:: /Development/Framework/Include/DrawingSurface.h                $
//   $Author:: Mgd                                                            $
//     $Date:: 10/07/96 10:01a                                                $
// $Revision:: 23                                                             $
//
// ****************************************************************************


#ifdef TPSDEBUG
#define	INLINE
#else
#define	INLINE	inline
#endif

#ifdef		_LET_MIKES_TEST_GIVE_ME_HEADACHES_
	#define	TEST32BIT
#endif	//	_LET_MIKES_TEST_GIVE_ME_HEADACHES_

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::RDrawingSurface( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE RDrawingSurface::RDrawingSurface( BOOLEAN fIsPrinting )
	: m_PenColor( kBlack ),
	  m_PenWidth( 1 ),
	  m_PenStyle( kSolidPen ),
	  m_ForegroundMode( kNormal ),
	  m_FillColor( kBlack ),
	  m_FontColor( kBlack ),
	  m_fColoringBookModeOn( FALSE ),
	  m_fFullPanelBitmapPrintingModeOn( FALSE ),
	  m_fFontSet( FALSE ),
	  m_fFontDirty( TRUE ),
	  m_fIsPrinting( fIsPrinting ),
	  m_fPenDirty( TRUE ),
	  m_fBrushDirty( TRUE ),
	  m_Tint(kMaxTint),
	  m_TintColor( kBlack )
	{
	// uninitialized members:
	//   m_FontInfo -- it's a structure
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::~RDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE RDrawingSurface::~RDrawingSurface( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsVisible( const RIntPoint& pt )
//
//  Description:		return TRUE if the point is visible else FALSE
//
//  Returns:			BOOL fVisible
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsVisible( const RIntPoint& pt )
	{
	pt;
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsVisible( const RIntRect& rc )
//
//  Description:		return TRUE if the rect is visible else FALSE
//
//  Returns:			BOOL fVisible
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsVisible( const RIntRect& rc )
	{
	rc;
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsVisible( const RRealVectorRect& rc )
//
//  Description:		return TRUE if the rect is visible else FALSE
//
//  Returns:			BOOL fVisible
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsVisible( const RRealVectorRect& rc )
	{
	return IsVisible( RIntRect(rc.m_TransformedBoundingRect) );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsVisible( const RRealPoint& pt, const R2dTransform& transform )
//
//  Description:		return TRUE if the point is visible else FALSE
//
//  Returns:			BOOL fVisible
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsVisible( const RRealPoint& pt, const R2dTransform& transform )
	{
	return IsVisible( pt*transform );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsVisible( const RRealRect& rc, const R2dTransform& transform )
//
//  Description:		return TRUE if the rect is visible else FALSE
//
//  Returns:			BOOL fVisible
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsVisible( const RRealRect& rc, const R2dTransform& transform )
	{
	return IsVisible( RRealVectorRect(rc), transform );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsVisible( const RRealVectorRect& rc, const R2dTransform& transform )
//
//  Description:		return TRUE if the rect is visible else FALSE
//
//  Returns:			BOOL fVisible
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsVisible( const RRealVectorRect& rc, const R2dTransform& transform )
	{
	return IsVisible( rc*transform );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetCurrentPoint( uWORD* puwCurClip )
//
//  Description:		return a const reference to the current point
//
//  Returns:			const RIntPoint& currentpoint
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE const RIntPoint& RDrawingSurface::GetCurrentPoint( uWORD* puwCurClip ) const
	{
	if( puwCurClip )
		*puwCurClip = m_uwCurrentPointClip;
	return m_ptCurrent;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsCoord32( )
//
//  Description:		Test a coordinate for usage of the upper 16 bits...
//
//  Returns:			TRUE if the coordinate uses the upper 16 bits else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsCoord32( YIntCoordinate coord )
	{
	#ifdef TEST32BIT
	if( GetAsyncKeyState(VK_SHIFT) &0x8000 )
		return TRUE;
	#endif
	register YIntCoordinate	nHi	= coord&0xFFFF8000;
	return BOOLEAN( nHi!=0 && nHi!=0xFFFF8000 );
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsPoint32( )
//
//  Description:		Test a point for 32bit coordinates
//
//  Returns:			TRUE if the point has 32bit values else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsPoint32( const RIntPoint& pt )
	{
	return BOOLEAN(IsCoord32(pt.m_x) || IsCoord32(pt.m_y));
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsRect32( )
//
//  Description:		Test a rectangle for 32bit coordinates
//
//  Returns:			TRUE if the rectangle has 32bit values else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsRect32( const RIntRect& rc )
	{
	return BOOLEAN(IsCoord32(rc.m_Left) || IsCoord32(rc.m_Top) || IsCoord32(rc.m_Right) || IsCoord32(rc.m_Bottom) || IsCoord32(rc.Width()) || IsCoord32(rc.Height()) );
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsPolyPoly32( )
//
//  Description:		Test a polypolyline for 32bit coordinates
//
//  Returns:			TRUE if the polypolyline has 32bit values else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsPolyPoly32( const RIntPoint* pPoints, const YPointCount* pPointCounts, YPolygonCount polyCount )
	{
	//
	// First, find out how many points total...
	long	lNumPoints	= 0;
	while( polyCount-- )
		{
		lNumPoints = lNumPoints + long(*pPointCounts);
		++pPointCounts;
		}
	//
	// Now check them...
	if( lNumPoints < 1 )
		return FALSE;
	RIntRect	rcBounds( *pPoints, *pPoints );
	while( lNumPoints-- )
		{
		if( IsPoint32(*pPoints) )
			return TRUE;
		rcBounds.AddPointToRect( *pPoints );
		++pPoints;
		}
	return BOOLEAN( IsCoord32(rcBounds.Width()) || IsCoord32(rcBounds.Height()) );
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsPoly32( )
//
//  Description:		Test a polyline for 32bit coordinates
//
//  Returns:			TRUE if the polyline has 32bit values else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsPoly32( const RIntPoint* pPoints, YPointCount pointCount )
	{
	return IsPolyPoly32( pPoints, &pointCount, 1 );
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetColoringBookMode( ) const
//
//  Description:		Return m_fColoringBookModeOn
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::GetColoringBookMode( ) const
	{
	return m_fColoringBookModeOn;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetColoringBookMode( const BOOLEAN fColoringBookModeOn )
//
//  Description:		set the coloring book mode
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDrawingSurface::SetColoringBookMode( const BOOLEAN fColoringBookModeOn )
	{
	m_fColoringBookModeOn = fColoringBookModeOn;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetFullPanelBitmapPrintingMode( ) const
//
//  Description:		Return m_fFullPanelBitmapPrintingModeOn
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::GetFullPanelBitmapPrintingMode( ) const
	{
	return m_fFullPanelBitmapPrintingModeOn;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetFullPanelBitmapPrintingMode( const BOOLEAN fColoringBookModeOn )
//
//  Description:		set the full panel bitmap printing mode
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDrawingSurface::SetFullPanelBitmapPrintingMode( const BOOLEAN fFullPanelBitmapPrintingModeOn )
	{
	m_fFullPanelBitmapPrintingModeOn = fFullPanelBitmapPrintingModeOn;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetPointClipEdges( )
//
//  Description:		Get the edges of the given clip rectangle the the point is outside
//
//  Returns:			uWORD uwOutsideEdges (bit flags, see above)
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE uWORD RDrawingSurface::GetPointClipEdges( const RIntPoint& pt, const RIntRect& rc )
	{
	register uWORD	uwClip	= uWORD(0);
	if( pt.m_x < rc.m_Left )
		uwClip |= kClipLeft;
	else if( pt.m_x >= rc.m_Right )
		uwClip |= kClipRight;
	if( pt.m_y < rc.m_Top )
		uwClip |= kClipTop;
	else if( pt.m_y >= rc.m_Bottom )
		uwClip |= kClipBottom;
	return uwClip;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::MoveTo( )
//
//  Description:		Changes the current drawing position to the given point.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDrawingSurface::MoveTo( const RRealPoint& point, const R2dTransform& transform )
	{
	MoveTo( point * transform );
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::LineTo( )
//
//  Description:		Draws a line using the current pen attributes from the
//							current drawing position to the given point. The drawing
//							position is updated.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDrawingSurface::LineTo( const RRealPoint& point, const R2dTransform& transform )
	{
	LineTo( point * transform );
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsPrintingCanceled( )
//
//  Description:		Test if printing was cancelled
//
//  Returns:			returns TRUE if printing was cancelled else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsPrintingCanceled()
	{
	return FALSE;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetForegroundMode( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDrawingSurface::SetForegroundMode( YDrawMode drawMode )
	{
	m_ForegroundMode = drawMode;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetPenWidth( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDrawingSurface::SetPenWidth( YPenWidth penWidth )
	{
	TpsAssert(penWidth > 0, "Pen width is less than 1.");
	//	force pen width to be 1 if coloring book mode is on
	if( !m_fColoringBookModeOn )
		m_PenWidth = penWidth;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetPenStyle( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDrawingSurface::SetPenStyle( YPenStyle penStyle )
	{
	TpsAssert(	penStyle == kSolidPen ||
					penStyle == kDashPen ||
					penStyle == kDotPen ||
					penStyle == kDashDotPen, "Invalid pen style" );

	m_PenStyle = penStyle;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::RestoreDefaults()
//
//  Description:		Restores the default drawing surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDrawingSurface::RestoreDefaults()
	{
	SetPenColor( RSolidColor( kBlack ) );
	SetPenWidth(1);
	SetPenStyle(kSolidPen);
	SetFillColor( RSolidColor( kBlack ) );
	SetFontColor( RSolidColor( kBlack ) );
	SetTint(kMaxTint);
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetFillColor( )
//
//  Description:		Accessor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE const RColor& RDrawingSurface::GetFillColor( ) const
	{
	return m_FillColor;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetPenColor( )
//
//  Description:		Accessor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE const RColor& RDrawingSurface::GetPenColor( ) const
	{
	return m_PenColor;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetFontColor( )
//
//  Description:		Accessor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE const RColor& RDrawingSurface::GetFontColor( ) const
	{
	return m_FontColor;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::IsPrinting( )
//
//  Description:		Called to determine if this surface is printing
//
//  Returns:			TRUE if the surface is being used for printing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE BOOLEAN RDrawingSurface::IsPrinting( ) const
	{
	return m_fIsPrinting;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetForegroundMode( )
//
//  Description:		Accessor
//
//  Returns:			current foreground mode
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE YDrawMode RDrawingSurface::GetForegroundMode( ) const
	{
	return m_ForegroundMode;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetPenWidth( )
//
//  Description:		Accessor
//
//  Returns:			pen width
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE YPenWidth RDrawingSurface::GetPenWidth( ) const
	{
	return m_PenWidth;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetPenStyle( )
//
//  Description:		Accessor
//
//  Returns:			pen style
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE YPenStyle RDrawingSurface::GetPenStyle( ) const
	{
	return m_PenStyle;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetFont( )
//
//  Description:		Accessor
//
//  Returns:			font info
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE YFontInfo RDrawingSurface::GetFont( ) const
	{
	return m_FontInfo;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurfaceState::RDrawingSurfaceState()
//
//  Description:		create an empty drawing surface state
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE RDrawingSurfaceState::RDrawingSurfaceState() :
		m_pState( NULL )
	{
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurfaceState::Save( RDrawingSurface* pSurface )
//
//  Description:		Save the state of the given drawing surface
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDrawingSurfaceState::Save( RDrawingSurface* pSurface )
	{
	TpsAssert( m_pState==NULL, "Overwriting a previously saved state!" );
	m_pState	= pSurface->SaveSurfaceState();
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurfaceState::RDrawingSurfaceState( RDrawingSurface* pSurface )
//
//  Description:		Save the state of the given drawing surface
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE RDrawingSurfaceState::RDrawingSurfaceState( RDrawingSurface* pSurface ) :
		m_pState( NULL )
	{
	Save( pSurface );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurfaceState::Restore( )
//
//  Description:		restore the state held by this RDrawingSurfaceState and delete
//							any temporary data.
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDrawingSurfaceState::Restore()
	{
	TpsAssert( m_pState!=NULL, "Attempt to restore uninitialized or previously restored drawing surface state" );
	//
	// restore the state...
	m_pState->Restore();
	//
	// delete it...
	delete m_pState;
	//
	// make sure the destructor doesn't try to restore as well...
	m_pState = NULL;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurfaceState::~RDrawingSurfaceState( )
//
//  Description:		restore the state held by this RDrawingSurfaceState and delete
//							any temporary data.
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE RDrawingSurfaceState::~RDrawingSurfaceState()
	{
	if( m_pState )
		{
		//
		// restore the state...
		m_pState->Restore();
		//
		// delete it...
		delete m_pState;
		}
	}


#ifdef		TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
INLINE void RDrawingSurface::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RDrawingSurface, this );
	TpsAssert(	m_PenStyle == kSolidPen ||
					m_PenStyle == kDashPen ||
					m_PenStyle == kDotPen ||
					m_PenStyle == kDashDotPen, "Invalid pen style" );
	}

#endif		// TPSDEBUG
