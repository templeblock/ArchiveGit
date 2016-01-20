// ****************************************************************************
//
//  File Name:			GpDrawingSurface.h
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Declaration of the RGpDrawingSurface class
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
//  $Logfile:: /PM8/Framework/Include/GpDrawingSurface.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GPDRAWINGSURFACE_H_
#define		_GPDRAWINGSURFACE_H_

#ifndef		_DRAWINGSURFACE_H_
#include		"DrawingSurface.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RGpDrawingSurface
//
//  Description:	This is the base class for all drawing surfaces which
//						encapsulate a Macintosh device context. To use, create a new
//						class derived off of RGpDrawingSurface, and write a new
//						constructor to obtain the Graph Port.
//
// ****************************************************************************
//
class RGpDrawingSurface : public RDrawingSurface
{
	// Construction, destruction & initialization
	public :
												RGpDrawingSurface( );
		virtual								~RGpDrawingSurface( );
		BOOLEAN								Initialize( const GrafPtr pPort );

	// Drawing operations
	public :
		virtual void						MoveTo( const RIntPoint& point );

		virtual void						BeginPath( );
		virtual void						EndPath( );
		virtual void						BeginOpenPath( );
		virtual void						EndOpenPath( );
		virtual void						IntersectClipPath( );
		virtual void						FillPath( );
		virtual void						FramePath( );
		virtual void						FrameAndFillPath( );
		virtual void						FillAndFramePath( );

		virtual BOOLEAN					IsVisible( const RIntPoint& pt );
		virtual BOOLEAN					IsVisible( const RIntRect& rc );
		virtual RIntRect					GetClipRect( ) const;
		virtual void						SetClipRect( const RIntRect& clipRect );
		virtual void						IntersectClipRect( const RIntRect& clipRect );
		virtual void						SetClipVectorRect( const RIntVectorRect& clipRect );
		virtual void						IntersectClipVectorRect( const RIntVectorRect& clipRect );
		
		virtual void						DrawFocusRect( const RIntRect rcRect );

		virtual void						DrawCharacters( const RIntPoint& point, YCounter count, LPCUBYTE pCharacters, int* pWidths = NULL );
		virtual RIntSize					GetActualTextSize( YCounter count, const uBYTE* pCharacters );
		virtual YFontDimensions			GetActualFontDimensions();

		virtual void						BlitDrawingSurface(const RDrawingSurface& rSrcDS, RIntRect rSrcRect, RIntRect rDestRect, EBlitMode eBlitMode);

		virtual void						SetForegroundColor( const RSolidColor& color );
		virtual void						SetBackgroundColor( const RSolidColor& color );

	// Accessors
	public :
		virtual void						SetForegroundMode( YDrawMode drawMode );
		virtual void						SetPenWidth( YPenWidth penWidth );
		virtual void						SetPenStyle( YPenStyle penStype );
		virtual BOOLEAN					HasPathSupport( ) const;
		virtual RIntSize					GetDPI( ) const;
		virtual uLONG						GetBitDepth() const;
		virtual long						GetSurface( ) const;
		virtual void						RestoreDefaults( );

	// Implementation
	protected :
		virtual void						SolidLineTo( const RIntPoint& point );
		virtual void						SolidPolyline( const RIntPoint* pPoints, YPointCount numPoints );
		virtual void						SolidPolylineTo( const RIntPoint* pPoints, YPointCount numPoints );
		virtual void						SolidFillRectangle( const RIntRect& rect );
		virtual void						SolidPolygon( const RIntPoint* pVertices, YPointCount numVertices );
		virtual void						SolidFillPolygon( const RIntPoint* pVertices, YPointCount numVertices );
		virtual void						SolidPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		virtual void						SolidFillPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		virtual void						SolidFramePolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount ); 
		virtual void						DeviceInvertRectangle( const RIntRect& rect );
		virtual void						DeviceInvertPolygon( const RIntPoint* pVertices, YPointCount numVertices );
		virtual void						DeviceInvertPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		virtual void						CreatePen( );
		virtual void						CreateBrush( );
		void									CreateFont( );
		PolyHandle							CreatePolygon( const RIntPoint* pVertices, YPointCount numVertices );
		PolyHandle							CreatePolygonSlowly( const RIntPoint* pVertices, YPointCount numVertices );
		void 									AppendPolygonToPolygon( PolyHandle hPoly, const RIntPoint* pVertices, YPointCount numVertices );

	// Surface State
	protected:
		//
		// save the current state.
		virtual RSurfaceState*	SaveSurfaceState();

	// Attributes & Member Data
	protected :
		GrafPtr								m_pGrafPort;

	private :
		GrafPtr								m_pSavedPort;
		static Pattern						m_PatternStyles[4];
		sWORD									m_PenWidthOffset;
		BOOLEAN								m_fFontDirty;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG

	friend class RGpSurfaceState;
};

// ****************************************************************************
//
//  Class Name:	RGpSurfaceState
//
//  Description:	This is the base class for Renaissance drawing surface states.
//						This only stores the drawing surface settings, any other
//						data (i.e. clip regions) must be stored in derived classes
//						specific to a given type of drawing surface...
//
// ****************************************************************************
//
class RGpSurfaceState : public RSurfaceState
{
//
// State data
protected:

//
// Construction/Destruction
protected:
	RGpSurfaceState( RGpDrawingSurface* pSurface );

//
// State restoration
public:
	virtual void Restore();

//
// Allow the GpDrawingSurface to create us
friend class RGpDrawingSurface;
};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::RGpDrawingSurface( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RGpDrawingSurface::RGpDrawingSurface( ) : RDrawingSurface( FALSE ),
																 m_pGrafPort( NULL ),
	  															 m_pSavedPort( NULL ),
	  															 m_PenWidthOffset( 0 ),
	  															 m_fFontDirty( TRUE )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::~RGpDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RGpDrawingSurface::~RGpDrawingSurface( )
{
	if ( m_pSavedPort != NULL )
		::SetPort( m_pSavedPort );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::HasPathSupport( )
//
//  Description:		Accessor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RGpDrawingSurface::HasPathSupport( ) const
	{
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::GetSurface( )
//
//  Description:		Return an identifier to the Output Drawing Surface.
//
//  Returns:			m_pGrafPort
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline long RGpDrawingSurface::GetSurface( ) const
	{
	return (long)m_pGrafPort;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _GPDRAWINGSURFACE_H_
