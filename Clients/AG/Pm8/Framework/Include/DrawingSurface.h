// ****************************************************************************
//
//  File Name:			DrawingSurface.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas & M. Houle
//
//  Description:		Declaration of the RDrawingSurface class
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
//  $Logfile:: /PM8/Framework/Include/DrawingSurface.h                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_DRAWINGSURFACE_H_
#define		_DRAWINGSURFACE_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//
// Forward references
class	RGradientFill;
class RSurfaceState;
class	RDrawingSurfaceState;

#ifdef _WINDOWS
	const int kMaxPoints = ( USHRT_MAX / sizeof( POINT ) );
#else
	const int kMaxPoints = ( USHRT_MAX / sizeof( Point ) );
#endif

// Pen styles
#ifdef	_WINDOWS
	// When printing to a B&W PCL printer, colored PS_SOLID pens print as black. PS_INSIDEFRAME works.
	const YPenStyle		kSolidPen	= PS_INSIDEFRAME;
	const YPenStyle		kDashPen		= PS_DASH;
	const YPenStyle		kDotPen		= PS_DOT;
	const YPenStyle		kDashDotPen	= PS_DASHDOT;
#else
	const YPenStyle		kSolidPen	= 0x0000;
	const YPenStyle		kDashPen		= 0x0001;
	const YPenStyle		kDotPen		= 0x0002;
	const YPenStyle		kDashDotPen	= 0x0003;
#endif

#ifdef _WINDOWS
enum EBlitMode
{
	kBlitSourceCopy = SRCCOPY,
	kBlitSourceAnd = SRCAND,
	kBlitSourceErase = SRCERASE,
	kBlitSourcePaint = SRCPAINT,
	kBlitNotSourceAnd = 0x00220326,
	kBlitMergePaint = MERGEPAINT,
	kBlitSourceXORDest = SRCINVERT,
	kBlitSourceXNORDest = 0x00990066
};
#elif MAC
enum EBlitMode
{
	kBlitSourceCopy,
	kBlitSourceAnd,
	kBlitSourceErase,
	kBlitSourcePaint,
	kBlitNotSourceAnd,
	kBlitMergePaint,
	kBlitSourceInvert
};
#endif //_WINDOWS/MAC

const uWORD	kClipLeft	= 0x01;
const uWORD	kClipTop		= 0x02;
const uWORD	kClipRight	= 0x04;
const uWORD	kClipBottom	= 0x08;

//
// @Const Place text at the top of the rectangle
const uWORD	kPlaceTextTop				= 0x0001;
//
// @Const Place text at the bottom of the rectangle
const uWORD	kPlaceTextBottom			= 0x0002;
//
// @Const Place text centered vertically in the rectangle
const uWORD	kPlaceTextCenterV			= (kPlaceTextTop|kPlaceTextBottom);
//
// @Const Place text at the left edge of the rectangle
const uWORD	kPlaceTextLeft				= 0x0004;
//
// @Const Place text at the right edge of the rectangle
const uWORD	kPlaceTextRight			= 0x0008;
//
// @Const Place text centered horizontally in the rectangle
const uWORD	kPlaceTextCenterH			= (kPlaceTextLeft|kPlaceTextRight);
//
// @Const Include the between-line leading in the vertical computations
const uWORD	kPlaceTextWithLeading	= 0x0010;
//
// @Const Vertical text placement mask
const uWORD	kPlaceTextVerticalPos	= (kPlaceTextTop|kPlaceTextBottom);
//
// @Const Horizontal text placement mask
const uWORD	kPlaceTextHorizontalPos	= (kPlaceTextLeft|kPlaceTextRight);


// ****************************************************************************
//
//  Class Name:	RDrawingSurface
//
//  Description:	This is the base class for Renaissance drawing surfaces.
//						Anything which can be rendered into should be encapsulated
//						by a class derived from RDrawingSurface.
//
// ****************************************************************************
//
class FrameworkLinkage RDrawingSurface : public RObject
	{
	// Construction, destruction & initialization
	public :
												RDrawingSurface( BOOLEAN fIsPrinting );
		virtual								~RDrawingSurface( );

	// Drawing operations
	public :
		virtual void						MoveTo( const RIntPoint& point ) = 0;
		void									MoveTo( const RRealPoint& point, const R2dTransform& transform );

		void									LineTo( const RIntPoint& point );
		void									LineTo( const RRealPoint& point, const R2dTransform& transform );

		void									Draw3dRect( RIntRect& rect, const RSolidColor& crTopLeft, const RSolidColor& crBottomRight ) ;
		void									Draw3dRect( int x, int y, int cx, int cy, const RSolidColor& crTopLeft, const RSolidColor& crBottomRight ) ;

		void									FrameRectangle( const RIntRect& rect );
		void									FrameRectangle( const RRealRect& rect, const R2dTransform& transform );
		void									FrameRectangleInside( const RRealRect& rect, const R2dTransform& transform );

		void									FillRectangle( const RIntRect& rect );
		void									FillRectangle( const RRealRect& rect, const R2dTransform& transform );

		void									InvertRectangle( const RIntRect& rect );
		void									InvertRectangle( const RRealRect& rect, const R2dTransform& transform );

		void									FrameVectorRect( const RIntVectorRect& vectorRect );
		void									FrameVectorRect( const RRealVectorRect& vectorRect, const R2dTransform& transform );

		void									FillVectorRect( const RIntVectorRect& vectorRect );
		void									FillVectorRect( const RRealVectorRect& vectorRect, const R2dTransform& transform );

		void									InvertVectorRect( const RIntVectorRect& vectorRect );
		void									InvertVectorRect( const RRealVectorRect& vectorRect, const R2dTransform& transform );

		void									Ellipse( const RIntRect& rect );
		void									Ellipse( const RRealRect& rect, const R2dTransform& transform );

		void									FrameEllipse( const RIntRect& rect );
		void									FrameEllipse( const RRealRect& rect, const R2dTransform& transform );

		void									FillEllipse( const RIntRect& rect );
		void									FillEllipse( const RRealRect& rect, const R2dTransform& transform );

		void									Polyline( const RIntPoint* pPoints, YPointCount numPoints );
		void									PolylineTo( const RIntPoint* pPoints, YPointCount numPoints );
		void									PolyPolyline( const RIntPoint* pPoints, YPointCount* pointCountArray, YPolylineCount polylineCount );

		void									Polygon( const RIntPoint* pVertices, YPointCount numVertices );
		void									FramePolygon( const RIntPoint* pVertices, YPointCount numVertices );
		void									FillPolygon( const RIntPoint* pVertices, YPointCount numVertices );
		void									InvertPolygon( const RIntPoint* pVertices, YPointCount numVertices );

		void									PolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		void									FramePolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		void									FillPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		void									InvertPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );

		static YPointCount				CreateEllipsePolyBezier( const RIntVectorRect& rc, RIntPoint* pptBuffer );
		static YPointCount				CreateArcPolyBezier( const RIntVectorRect& rc, RIntPoint* pptBez );
		static RIntPoint*					ExpandPolyBezier( const RIntPoint& startPoint, const RIntPoint* pPoints, YPointCount numPoints, YPointCount& numExpandedPoints );
		static RRealPoint*				ExpandPolyBezier( const RRealPoint& startPoint, const RRealPoint* pPoints, YPointCount numPoints, YPointCount& numExpandedPoints );
		void									PolyBezier( const RIntPoint* pPoints, YPointCount numPoints );
		void									PolyBezierTo( const RIntPoint* pPoints, YPointCount pointCount );
		void									FramePolyBezier( const RIntPoint* pPoints, YPointCount numPoints );
		void									FillPolyBezier( const RIntPoint* pPoints, YPointCount numPoints );
		void									PolyQuadraticSplineTo( const RIntPoint* pPoints, YPointCount pointCount );

		virtual void						BeginPath( ) = 0;
		virtual void						EndPath( ) = 0;
		virtual void						BeginOpenPath( ) = 0;
		virtual void						EndOpenPath( ) = 0;
		virtual void						IntersectClipPath() = 0;
		virtual void						FillPath( ) = 0;
		virtual void						FramePath( ) = 0;
		virtual void						FrameAndFillPath( ) = 0;
		virtual void						FillAndFramePath( ) = 0;

		virtual RIntRect					GetClipRect( ) const = 0;
		virtual void						SetClipRect( const RIntRect& clipRect ) = 0;
		virtual void						IntersectClipRect( const RIntRect& clipRect ) = 0;
		void									IntersectClipRect( const RRealRect& clipRect, const R2dTransform& transform );
		virtual void						SetClipVectorRect( const RIntVectorRect& clipRect ) = 0;
		virtual void						IntersectClipVectorRect( const RIntVectorRect& clipRect ) = 0;
		virtual BOOLEAN					IsVisible( const RIntPoint& pt );
		virtual BOOLEAN					IsVisible( const RIntRect& rc );
		BOOLEAN								IsVisible( const RRealVectorRect& rc );
		BOOLEAN								IsVisible( const RRealPoint& pt, const R2dTransform& transform );
		BOOLEAN								IsVisible( const RRealRect& rc, const R2dTransform& transform );
		BOOLEAN								IsVisible( const RRealVectorRect& rc, const R2dTransform& transform );
		
		virtual void						DrawFocusRect( const RIntRect rcRect ) = 0 ;

		virtual void						DrawCharacters( const RIntPoint& point, YCounter count, LPCUBYTE pCharacters, int* pWidths = NULL ) = 0;
		virtual void						PlaceTextInRect( const RRealRect& rcLogical, const RMBCString& szText, const R2dTransform& transform, uWORD uwOptions, RRealRect* pOutputRect=NULL );
		virtual RIntSize					GetActualTextSize( YCounter count, const uBYTE* pCharacters );
		virtual YFontDimensions			GetActualFontDimensions();

		virtual void						BlitDrawingSurface(const RDrawingSurface& rSrcDS, RIntRect rSrcRect, RIntRect rDestRect, EBlitMode eBlitMode) = 0;
		virtual void						BlitDrawingSurface(const RDrawingSurface& rSrcDS, RIntRect rSrcRect, RIntRect rDestRect, const RSolidColor& transColor) = 0;

		virtual RSurfaceState*			SaveSurfaceState() = 0;

		BOOLEAN								ClipLineSegment( RIntPoint& pt0, RIntPoint& pt1, const RIntRect& rcClip );
		BOOLEAN								ClipRectangle( RIntRect& rc, const RIntRect& rcClip );

		void									CopyAttributes( const RDrawingSurface& drawingSurface );

		virtual void						SetForegroundColor( const RSolidColor& color ) = 0;
		virtual void						SetBackgroundColor( const RSolidColor& color ) = 0;

	// Accessor functions
	public :
		BOOLEAN								SetCurrentPoint( const RIntPoint& pt );
		BOOLEAN								SetCurrentPoint( const RIntPoint& pt, uWORD uwClipEdges );
		const RIntPoint&					GetCurrentPoint( uWORD* puwCurClip = NULL ) const;
		virtual BOOLEAN					IsPrintingCanceled( );
		virtual void						SetForegroundMode( YDrawMode drawMode ) = 0;
		YDrawMode							GetForegroundMode() const;
		virtual void						SetPenWidth( YPenWidth penWidth ) = 0;
		YPenWidth							GetPenWidth() const;
		virtual void						SetPenStyle( YPenStyle penStype ) = 0;
		YPenStyle							GetPenStyle() const;
		void									SetFillColor( const RColor& fillColor );
		void									SetFillColor( const RColor& fillColor, const R2dTransform& transform );
		const RColor&						GetFillColor() const;
		void									SetPenColor( const RColor& penColor );
		void									SetPenColor( const RColor& penColor, const R2dTransform& transform );
		const RColor&						GetPenColor() const;
		void									SetFontColor( const RColor& fontColor );
		const RColor&						GetFontColor() const;
		void									SetFont( const YFontInfo& fontInfo );
		virtual void						SetFont( const YFontDescriptor& fdesc, const R2dTransform& transform );
		YFontInfo							GetFont() const;
		YPercentage							GetGradientPrecision() const;		
		void									SetTintColor( const RSolidColor& tintColor );
		const RSolidColor&				GetTintColor( );
		void									SetTint(YTint yTint);
		YTint									GetTint( ) const;
		virtual void						RestoreDefaults( );
		virtual RIntSize					GetDPI( ) const = 0;
		virtual uLONG						GetBitDepth() const = 0;
		virtual long						GetSurface( ) const = 0;
		virtual BOOLEAN					GetColoringBookMode( ) const;
		virtual void						SetColoringBookMode( const BOOLEAN fColoringBookModeOn );
		virtual BOOLEAN					GetFullPanelBitmapPrintingMode( ) const;
		virtual void						SetFullPanelBitmapPrintingMode( const BOOLEAN fFullPanelBitmapPrintingModeOn );
		virtual BOOLEAN					HasPathSupport( ) const = 0;
		BOOLEAN								IsPrinting( ) const;

	// Implementation
	private :
		virtual void						SolidLineTo( const RIntPoint& point ) = 0;
		virtual void						SolidPolyline( const RIntPoint* pPoints, YPointCount numPoints ) = 0;
		virtual void						SolidPolylineTo( const RIntPoint* pPoints, YPointCount numPoints ) = 0;
		virtual void						SolidFillRectangle( const RIntRect& rect ) = 0;
		virtual void						SolidPolygon( const RIntPoint* pVertices, YPointCount numVertices ) = 0;
		virtual void						SolidFillPolygon( const RIntPoint* pVertices, YPointCount numVertices ) = 0;
		virtual void						SolidPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount ) = 0; 
		virtual void						SolidFillPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount ) = 0; 
		virtual void						SolidFramePolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount ) = 0; 
		virtual void						DeviceInvertRectangle( const RIntRect& rect ) = 0;
		virtual void						DeviceInvertPolygon( const RIntPoint* pVertices, YPointCount numVertices ) = 0;
		virtual void						DeviceInvertPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount ) = 0;
		virtual void						CreatePen( ) = 0;
		virtual void						CreateBrush( ) = 0;

	// Attributes & Member Data
	protected :
		RIntPoint							m_ptCurrent;
		uWORD									m_uwCurrentPointClip;
		YPenWidth							m_PenWidth;
		YPenStyle							m_PenStyle;
		YDrawMode							m_ForegroundMode;
		RColor								m_FillColor;
		RColor								m_PenColor;
		RColor								m_FontColor;
		YFontInfo							m_FontInfo;
		BOOLEAN								m_fColoringBookModeOn;
		BOOLEAN								m_fFullPanelBitmapPrintingModeOn;
		BOOLEAN								m_fBrushDirty;
		BOOLEAN								m_fPenDirty;
		YTint									m_Tint;
		RSolidColor							m_TintColor;
		BOOLEAN								m_fFontDirty;
		BOOLEAN								m_fFontSet;
		BOOLEAN								m_fIsPrinting;

	// 32 bit coordinate support
	private :
		void									Polyline32( const RIntPoint* pPoints, YPointCount numPoints );
		void									PolylineTo32( const RIntPoint* pPoints, YPointCount numPoints );
		void									PolyPolyline32( const RIntPoint* pPoints, YPointCount* pointCountArray, YPolylineCount polylineCount );
		void									Polygon32( const RIntPoint* pVertices, YPointCount numVertices );
		void									FramePolygon32( const RIntPoint* pVertices, YPointCount numVertices );
		void									FillPolygon32( const RIntPoint* pVertices, YPointCount numVertices );
		void									InvertPolygon32( const RIntPoint* pVertices, YPointCount numVertices );
		void									PolyPolygon32( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		void									FramePolyPolygon32( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		void									FillPolyPolygon32( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		void									InvertPolyPolygon32( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );

	// 32bit Coordinate tests
	protected :
		BOOLEAN								IsCoord32( YIntCoordinate coord );
		BOOLEAN								IsPoint32( const RIntPoint& pt );
		BOOLEAN								IsRect32( const RIntRect& rc );
		BOOLEAN								IsPoly32( const RIntPoint* pPoints, YPointCount pointCount );
		BOOLEAN								IsPolyPoly32( const RIntPoint* pPoints, const YPointCount* pPointCounts, YPolygonCount polyCount );

	// Gradient support
	private :
		void									GradientFillRectangle( YIntCoordinate left, YIntCoordinate top, YIntCoordinate right, YIntCoordinate bottom );
		void									GradientFillRectangle( const RIntRect& rect );
		void									GradientFillRectangle( const RIntRect& rect, const R2dTransform& transform );
		void									GradientFillVectorRect( const RIntVectorRect& vectorRect );
		void									GradientFillVectorRect( const RIntVectorRect& vectorRect, const R2dTransform& transform );
		void									GradientFillPolygon( const RIntPoint* pVertices, YPointCount numVertices );
		void									GradientFillPolygon( const RIntPoint* pVertices, YPointCount numVertices, const R2dTransform& GradientTransform );
		void									GradientFillPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		void									GradientFillPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount, const R2dTransform& GradientTransform );

	// Clipping support
	protected:
		uWORD									GetPointClipEdges( const RIntPoint& pt, const RIntRect& rc );
		uWORD									ClipHorizontalEdge( RIntPoint& pt, const RIntSize& vecDir, YIntCoordinate yEdge, const RIntRect& rcClip );
		uWORD									ClipVerticalEdge( RIntPoint& pt, const RIntSize& vecDir, YIntCoordinate xEdge, const RIntRect& rcClip );
		uWORD									ClipVector( RIntPoint& pt, const RIntSize& vecDir, uWORD uwClip, const RIntRect& rcClip );
		BOOLEAN								DoClipLineSegment( RIntPoint& pt0, uWORD uwClip0, RIntPoint& pt1, uWORD uwClip1, const RIntRect& rcClip );

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const = 0;
#endif	// TPSDEBUG

	//
	// Allow the state container class to create the state
	friend class RDrawingSurfaceState;
	friend class RSurfaceState;
	};

// ****************************************************************************
//
//  Class Name:	RSurfaceState
//
//  Description:	This is the base class for Renaissance drawing surface states.
//						The ONLY reason it's public is for derived classes to be able
//						to derive their own state from it.  The base holds the internal
//						data for a surface, whereas the derived classes will hold
//						their surfaces specific data.  Anyone implementing a surfacestate
//						for their drawingsurface MUST supply a restore (along with
//						the appropriate constructor)...
//
// ****************************************************************************
//
class RSurfaceState
{
//
// State data
protected:
	RDrawingSurface*	m_pSurface;
	RColor				m_PenColor;
	YPenWidth			m_PenWidth;
	YPenStyle			m_PenStyle;
	YDrawMode			m_ForegroundMode;
	RColor				m_FillColor;
	RColor				m_FontColor;
	BOOLEAN				m_fFontSet;
	YFontInfo			m_FontInfo;
	RGradientFill*		m_pGradientFill;
//
// Construction/Destruction
protected:
	RSurfaceState( RDrawingSurface* pDrawingSurface );
	~RSurfaceState();
//
// State restoration
protected:
	virtual void	Restore();
//
// Allow the drawingsurfacestate container to create and manipulate us
friend class RDrawingSurfaceState;
//
// Allow the drawing surface to create and restore us
friend class RDrawingSurface;
};

// ****************************************************************************
//
//  Class Name:	RDrawingSurfaceState
//
//  Description:	Holder class for drawing surface states.
//
// ****************************************************************************
//
class FrameworkLinkage RDrawingSurfaceState
{
	private:
		RSurfaceState*	m_pState;
	public:
		RDrawingSurfaceState();
		RDrawingSurfaceState( RDrawingSurface* pSurface );
		~RDrawingSurfaceState();
		void	Save( RDrawingSurface* pSurface );
		void	Restore();
};

FrameworkLinkage YPenWidth	TransformedPenWidth( YRealDimension logicalWidth, const R2dTransform& xform );

#ifndef		TPSDEBUG
#include	"DrawingSurface.inl"
#endif	// TPSDEBUG


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _DRAWINGSURFACE_H_
