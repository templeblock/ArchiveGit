// ****************************************************************************
//
//  File Name:			PostScriptDrawingSurface.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas & R. Hood
//
//  Description:		Declaration of the RPostScriptDrawingSurface class
//
//  Portability:		Win32 or Mac dependent
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
//  $Logfile:: /PM8/Framework/Include/PostScriptDrawingSurface.h              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_POSTSCRIPTDRAWINGSURFACE_H_
#define		_POSTSCRIPTDRAWINGSURFACE_H_

#ifdef _WINDOWS
	#ifndef		_DCDRAWINGSURFACE_H_
	#include		"DcDrawingSurface.h"
	#endif
	#define	PostscriptBaseClass	RDcDrawingSurface
#endif

#ifdef MAC
	#ifndef		_PRINTERDRAWINGSURFACE_H_
	#include		"PrinterDrawingSurface.h"
	#endif
	#define	PostscriptBaseClass	RPrinterDrawingSurface
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RPostScriptDrawingSurface
//
//  Description:	
//
// ****************************************************************************
//
class FrameworkLinkage RPostScriptDrawingSurface : public PostscriptBaseClass
	{
	// Enums
	public :
#ifdef	MAC
		enum EPicComments					{ kPicLParen 					= 0, 
												  kPicRParen,
												  kPicDwgBeg 					= 130,
												  kPicDwgEnd,
												  kPicGrpBeg 					= 140,
												  kPicGrpEnd,
												  kPicBitBeg,
												  kPicBitEnd,
												  kPicTextBegin				= 150,
												  kPicTextEnd 					= 151,
												  kPicStringBegin,
												  kPicStringEnd,
										  		  kPicTextCenter,
												  kPicLineLayoutOff 			= 155,
												  kPicLineLayoutOn,
												  kPicPolyBegin 				= 160,
												  kPicPolyEnd,
												  kPicPlyByt,
												  kPicPolyIgnore,
												  kPicPlyClose 				= 165,
												  kPicArrw1 					= 170,
												  kPicArrw2,
												  kPicArrw3,
												  kPicArrwEnd,
												  kPicDashedStop 				= 181,
												  kPicPostScriptBegin 		= 190, 
												  kPicPostScriptEnd, 
												  kPicPostScriptHandle, 
												  kPicTextIsPostScript 		= 194,
												  kPicNewPostScriptBegin 	= 196,
											  	  kPicRotateBegin				= 200,
											  	  kPicRotateEnd 				= 201,
											  	  kPicRotateCenter,
												  kPicFormsPrinting 			= 210,
												  kPicEndFormsPrinting,
												  kPicAutoWake 				= 215,
												  kPicManWake 					= 217,
												  kPicBegBitmapThin 			= 1000, 
												  kPicEndBitmapThin,
												  kPicLasso 					= 12345 };
#endif	//	MAC

	// Construction, destruction & initialization
	public :
#ifdef MAC
												RPostScriptDrawingSurface( const THPrint hPrintInfo );
#else
												RPostScriptDrawingSurface( );
#endif	//	MAC

		virtual								~RPostScriptDrawingSurface( );
#ifdef MAC
		virtual BOOLEAN					Initialize( const GrafPtr pPort, 
																const RIntRect& boundingRect, 
																const BOOLEAN fUseMaxResolution );
#else
		virtual BOOLEAN					Initialize( HDC hOutputDC, HDC hAttributeDC = NULL );
#endif	//	MAC

	// Drawing operations
//	public :
		virtual void						MoveTo( const RIntPoint& point );
		virtual void						MoveTo( YIntCoordinate x, YIntCoordinate y );
		virtual void						LineTo( const RIntPoint& point );
		virtual void						LineTo( YIntCoordinate x, YIntCoordinate y );

		void									RMoveTo( const RIntPoint& point );
		void									RMoveTo( YIntCoordinate x, YIntCoordinate y );
		void									RLineTo( const RIntPoint& point );
		void									RLineTo( YIntCoordinate x, YIntCoordinate y );

		virtual void						FrameRectangle( const RIntRect& rect );
		virtual void						FrameRectangle( YIntCoordinate left, YIntCoordinate top, YIntCoordinate right, YIntCoordinate bottom );

		virtual void						FillRectangle( const RIntRect& rect );
		virtual void						FillRectangle( YIntCoordinate left, YIntCoordinate top, YIntCoordinate right, YIntCoordinate bottom );

		virtual void						InvertRectangle( const RIntRect& rect );
		virtual void						InvertRectangle( YIntCoordinate left, YIntCoordinate top, YIntCoordinate right, YIntCoordinate bottom );

		virtual void						Polyline( const RIntPoint* pPoints, YPointCount numPoints );
		virtual void						PolylineTo( const RIntPoint* pPoints, YPointCount numPoints );

		virtual void						Polygon( const RIntPoint* pVertices, YPointCount numVertices );
		virtual void						FramePolygon( const RIntPoint* pVertices, YPointCount numVertices );
		virtual void						FillPolygon( const RIntPoint* pVertices, YPointCount numVertices );
		virtual void						InvertPolygon( const RIntPoint* pVertices, YPointCount numVertices );

		virtual void						PolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		virtual void						FramePolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		virtual void						FillPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		virtual void						InvertPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );

		virtual void						PolyBezier( const RIntPoint* pPoints, YPointCount numPoints );
		virtual void						FramePolyBezier( const RIntPoint* pPoints, YPointCount numPoints );
		virtual void						FillPolyBezier( const RIntPoint* pPoints, YPointCount numPoints );

		virtual void						BeginPath( );
		virtual void						EndPath( );
		virtual void						BeginOpenPath( );
		virtual void						EndOpenPath( );
		virtual void						IntersectClipPath( );
		virtual void						FillPath( );
		virtual void						FramePath( );
		virtual void						FrameAndFillPath( );
		virtual void						FillAndFramePath( );

		virtual void						PolyBezierTo( const RIntPoint* pPoints, YPointCount pointCount );
		virtual void						PolyQuadraticSplineTo( const RIntPoint* pPoints, YPointCount pointCount );
		
	// Functions to set the state of the device
	public :
		virtual void						SetForegroundMode( YDrawMode drawMode );
		virtual void						SetPenWidth( YPenWidth penWidth );
		virtual void						SetPenStyle( YPenStyle penStype );
		virtual void						SetPenColor( const RColor& penColor );
		virtual void						SetFillColor( const RColor& fillColor );
		virtual BOOLEAN					BeginPage( );
		virtual BOOLEAN					EndPage( );

		virtual RIntRect					GetClipRect( ) const;
		virtual void						SetClipRect( const RIntRect& clipRect );
		virtual void						IntersectClipRect( const RIntRect& clipRect );
		virtual void						SetClipVectorRect( const RIntVectorRect& clipRect );
		virtual void						IntersectClipVectorRect( const RIntVectorRect& clipRect );

	// Surface capabilities
	public :
		virtual BOOLEAN					HasPathSupport( ) const;

	// Implementation
	private :
		void									SetColorRGB( RColor rgbColor );
		void									CreatePen( );
		void									CreateBrush( );
		void									CurveTo( const RIntPoint* pPoints );
		void									StartupPostScript( );
		void									ShutdownPostScript( );
		void									Stroke( const RIntPoint& point );
		void									SendPostScript( );

		void									Append( const char* szCommand );
		void									Append( const int nArg0 );
		void									Append( const RColor& clr );
		void									Append( const RIntPoint& pt );
		void									Append( const RIntPoint* pPoints, YPointCount pointCount, YPointCount pointsPerCommand = 1, const char* szCommand = NULL );

	// Surface State
	protected:
		//
		// save the current state.
		virtual RSurfaceState*	SaveSurfaceState();

	// Attributes & Member Data
	private :
		enum	ClassConstants {	kCommandBufferSize	= 512 };
		uBYTE									m_ubCommandBuffer[ kCommandBufferSize ];
		PUBYTE								m_pubCommandEnd;
		BOOLEAN								m_fPathOpen;
		BOOLEAN								m_fPolyOpen;
		BOOLEAN								m_fPostScripting;
		BOOLEAN								m_fPrefixed;
		RIntPoint							m_ptPenPosition; 

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG

	//
	// Allow our state to be saved/restored
	friend class RPostScriptSurfaceState;
	};

// ****************************************************************************
//
//  Class Name:	RPostScriptSurfaceState
//
//  Description:	This is the base class for Renaissance drawing surface states.
//						This only stores the drawing surface settings, any other
//						data (i.e. clip regions) must be stored in derived classes
//						specific to a given type of drawing surface...
//
// ****************************************************************************
//
class RPostScriptSurfaceState : public RSurfaceState
{
//
// State data
protected:
	BOOLEAN		m_fPathOpen;
	BOOLEAN		m_fPolyOpen;
	BOOLEAN		m_fPostScripting;
	BOOLEAN		m_fPrefixed;
	RIntPoint	m_ptPenPosition; 
//
// Construction/Destruction
protected:
	RPostScriptSurfaceState( RPostScriptDrawingSurface* pSurface );
//
// State restoration
public:
	virtual void Restore();

//
// Allow the PostScriptDrawingSurface to create us
friend class RPostScriptDrawingSurface;
};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::HasPathSupport( )
//
//  Description:		Called to determine if this surface has path support.
//							Path support includes calls to PolyBezierTo.
//
//  Returns:			TRUE if the surface has path support, FALSE otherwise.
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RPostScriptDrawingSurface::HasPathSupport( ) const
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::InvertRectangle( )
//
//  Description:		Invert the given rectangle.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RPostScriptDrawingSurface::InvertRectangle( const RIntRect& /* rect */ )
	{
	TpsAssertAlways( "InvertRectangle not implemented for a postscript drawing surface" );
	}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::InvertRectangle( )
//
//  Description:		Invert the given rectangle.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RPostScriptDrawingSurface::InvertRectangle( YIntCoordinate /*left*/, YIntCoordinate /*top*/, YIntCoordinate /*right*/, YIntCoordinate /*bottom*/ )
	{
	TpsAssertAlways( "InvertRectangle not implemented for a postscript drawing surface" );
	}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::InvertPolygon( )
//
//  Description:		Invert the polygon.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RPostScriptDrawingSurface::InvertPolygon( const RIntPoint* /*pVertices*/, YPointCount /*numVertices*/ )
	{
	TpsAssertAlways( "InvertPolygon not implemented for a postscript drawing surface" );
	}


// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::InvertPolyPolygon( )
//
//  Description:		Invert the polygon.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RPostScriptDrawingSurface::InvertPolyPolygon( const RIntPoint* /*pVertices*/, YPointCount* /*pointCountArray*/, YPolygonCount /*polygonCount*/ )
	{
	TpsAssertAlways( "InvertPolyPolygon not implemented for a postscript drawing surface" );
	}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::MoveTo( )
//
//  Description:		Changes the current drawing position to the given point.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RPostScriptDrawingSurface::MoveTo( YIntCoordinate x, YIntCoordinate y )
{
	MoveTo( RIntPoint( x, y ) );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::RMoveTo( )
//
//  Description:		Changes the current drawing position to the given point
//							relative to the current drawing position.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RPostScriptDrawingSurface::RMoveTo( YIntCoordinate x, YIntCoordinate y )
{
	RMoveTo( RIntPoint( x, y ) );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::LineTo( )
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
inline void RPostScriptDrawingSurface::LineTo( YIntCoordinate x, YIntCoordinate y )
{
	LineTo( RIntPoint( x, y ) );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::RLineTo( )
//
//  Description:		Draws a line using the current pen attributes from the
//							current drawing position to the given point relative to 
//							the current drawing position. The drawing position is updated.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RPostScriptDrawingSurface::RLineTo( YIntCoordinate x, YIntCoordinate y )
{
	RLineTo( RIntPoint( x, y ) );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FrameRectangle( )
//
//  Description:		Draws a rectangle using the current pen attributes. The
//							rectangle is not filled.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RPostScriptDrawingSurface::FrameRectangle( const RIntRect& rect )
{
	FrameRectangle( rect.m_Left, rect.m_Top, rect.m_Right, rect.m_Bottom );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FillRectangle( )
//
//  Description:		Draws a filled rectangle using the current fill color
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RPostScriptDrawingSurface::FillRectangle( const RIntRect& psRect )
{
	FillRectangle( psRect.m_Left, psRect.m_Top, psRect.m_Right, psRect.m_Bottom );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::SetClipRect( )
//
//  Description:		Sets the current clip rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RPostScriptDrawingSurface::SetClipRect( const RIntRect& clipRect )
	{
	SetClipVectorRect( RIntVectorRect(clipRect) );
	}


// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::IntersectClipRect( )
//
//  Description:		Sets a new clip rect that is the interesection of the
//							current clip rect and the given clip rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RPostScriptDrawingSurface::IntersectClipRect( const RIntRect& clipRect )
	{
	IntersectClipVectorRect( RIntVectorRect(clipRect) );
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _POSTSCRIPTDRAWINGSURFACE_H_
