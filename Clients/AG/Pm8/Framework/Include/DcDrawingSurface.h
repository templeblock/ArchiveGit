// ****************************************************************************
//
//  File Name:			DcDrawingSurface.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RDcDrawingSurface class
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
//  $Logfile:: /PM8/Framework/Include/DcDrawingSurface.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_DCDRAWINGSURFACE_H_
#define		_DCDRAWINGSURFACE_H_

#ifndef		_DRAWINGSURFACE_H_
#include		"DrawingSurface.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RDcDrawingSurface
//
//  Description:	This is the base class for all drawing surfaces which
//						encapsulate a Windows device context. To use, create a new
//						class derived off of RDcDrawingSurface, and write a new
//						initializer to obtain the DC.
//
// ****************************************************************************
//
class FrameworkLinkage RDcDrawingSurface : public RDrawingSurface
	{
	// Construction, destruction & initialization
	public :
												RDcDrawingSurface( BOOLEAN fUsePathSupport = FALSE, BOOLEAN fIsPrinting = FALSE );
		virtual								~RDcDrawingSurface( );
		virtual BOOLEAN					Initialize( HDC hOutputDC, HDC hAttributeDC = NULL );
		void									DetachDCs( );

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
		
		virtual void						DrawFocusRect( const RIntRect rcRect ) ;

		virtual void						DrawCharacters( const RIntPoint& point, YCounter count, LPCUBYTE pCharacters, int* pWidths = NULL );
		virtual RIntSize					GetActualTextSize( YCounter count, const uBYTE* pCharacters );
		virtual YFontDimensions			GetActualFontDimensions();

		virtual void						BlitDrawingSurface(const RDrawingSurface& rSrcDS, RIntRect rSrcRect, RIntRect rDestRect, EBlitMode eBlitMode);
		virtual void						BlitDrawingSurface(const RDrawingSurface& rSrcDS, RIntRect rSrcRect, RIntRect rDestRect, const RSolidColor& transColor);


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

	// Surface State
	protected:
		//
		// save the current state.
		virtual RSurfaceState*	SaveSurfaceState();

	// Attributes & Member Data
	protected :
		HDC									m_hOutputDC;
		HDC									m_hAttributeDC;
		BOOLEAN								m_fUsePathSupport;
		HFONT									m_OrigFont;
		BOOLEAN								m_fUseScreenPalette;
		HPALETTE								m_OrigPalette;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG

	friend class RDcSurfaceState;
	};

// ****************************************************************************
//
//  Class Name:	RDcSurfaceState
//
//  Description:	This is the base class for Renaissance drawing surface states.
//						This only stores the drawing surface settings, any other
//						data (i.e. clip regions) must be stored in derived classes
//						specific to a given type of drawing surface...
//
// ****************************************************************************
//
class RDcSurfaceState : public RSurfaceState
{
//
// State data
protected:
	int		m_nSaveID;
	BOOLEAN	m_fPenDirty;
	BOOLEAN	m_fBrushDirty;
	BOOLEAN	m_fFontDirty;
	BOOLEAN	m_fFontSet;

//
// Construction/Destruction
protected:
	RDcSurfaceState( RDcDrawingSurface* pSurface );

//
// State restoration
public:
	virtual void Restore();

//
// Allow the DcDrawingSurface to create us
friend class RDcDrawingSurface;
};

#ifndef		TPSDEBUG
#include	"DcDrawingSurface.inl"
#endif	// TPSDEBUG

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _DCDRAWINGSURFACE_H_
