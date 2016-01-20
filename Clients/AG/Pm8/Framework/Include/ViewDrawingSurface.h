// ****************************************************************************
//
//  File Name:			ViewDrawingSurface.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RViewDrawingSurface class
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
//  $Logfile:: /PM8/Framework/Include/ViewDrawingSurface.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_VIEWDRAWINGSURFACE_H_
#define		_VIEWDRAWINGSURFACE_H_

#ifdef	WIN32
	#ifndef		_DCDRAWINGSURFACE_H_
	#include		"DcDrawingSurface.h"
	#endif
	#define	BaseDrawingSurface	RDcDrawingSurface

#else		//	!WIN32 well, must be MAC

	#ifndef		_GPDRAWINGSURFACE_H_
	#include		"GpDrawingSurface.h"
	#endif
	#define	BaseDrawingSurface	RGpDrawingSurface
#endif	//	WIN32 or !WIN32

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//	Forward References
class RView;

// ****************************************************************************
//
//  Class Name:	RViewDrawingSurface
//
//  Description:	This is the base class for all drawing surfaces which
//						are created with a RView based class.
//
// ****************************************************************************
//
class FrameworkLinkage RViewDrawingSurface : public BaseDrawingSurface
	{
	// Construction, destruction & initialization
	public :
												RViewDrawingSurface( const RView* pView );
		virtual								~RViewDrawingSurface( );

	// Operations
	public :
		virtual void						MoveTo( const RIntPoint& point );
		void									MoveTo( const RRealPoint& point, const R2dTransform& transform );

		virtual void						LineTo( const RIntPoint& point );
		void									LineTo( const RRealPoint& point, const R2dTransform& transform );

		void									FrameRectangle( const RRealRect& rect, const R2dTransform& transform );
		void									FillRectangle( const RRealRect& rect, const R2dTransform& transform );

	// Member data
	private :
		const RView*						m_pView;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RViewDrawingSurface::MoveTo( )
//
//  Description:		Changes the current drawing position to the given point.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RViewDrawingSurface::MoveTo( const RIntPoint& point )
	{
	BaseDrawingSurface::MoveTo( point );
	}

// ****************************************************************************
//
//  Function Name:	RViewDrawingSurface::MoveTo( )
//
//  Description:		Changes the current drawing position to the given point.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RViewDrawingSurface::MoveTo( const RRealPoint& point, const R2dTransform& transform )
	{
	RDrawingSurface::MoveTo( point, transform );
	}

// ****************************************************************************
//
//  Function Name:	RViewDrawingSurface::LineTo( )
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
inline void RViewDrawingSurface::LineTo( const RIntPoint& point )
	{
	BaseDrawingSurface::LineTo( point );
	}

// ****************************************************************************
//
//  Function Name:	RViewDrawingSurface::LineTo( )
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
inline void RViewDrawingSurface::LineTo( const RRealPoint& point, const R2dTransform& transform )
	{
	RDrawingSurface::LineTo( point, transform );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FrameRectangle( )
//
//  Description:		Renders a rectangle through a transform
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RViewDrawingSurface::FrameRectangle( const RRealRect& rect, const R2dTransform& transform )
	{
	RDrawingSurface::FrameRectangle( rect, transform );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FillRectangle( )
//
//  Description:		Renders a rectangle through a transform
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RViewDrawingSurface::FillRectangle( const RRealRect& rect, const R2dTransform& transform )
	{
	RDrawingSurface::FillRectangle( rect, transform );
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _VIEWDRAWINGSURFACE_H_
