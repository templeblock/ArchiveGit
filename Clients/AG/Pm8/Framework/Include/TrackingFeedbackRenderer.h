// ****************************************************************************
//
//  File Name:			TrackingFeedbackRenderer.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RTrackingFeedbackRenderer class
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
//  $Logfile:: /PM8/Framework/Include/TrackingFeedbackRenderer.h              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _TRACKINGFEEDBACKRENDERER_H_
#define _TRACKINGFEEDBACKRENDERER_H_

#include "DrawingSurface.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RView;
class RComponentCollection;
class RScratchBitmapImage;
class ROffscreenDrawingSurface;
class RWindowView;

// ****************************************************************************
//
//  Class Name:	RTrackingFeedbackRenderer
//
//  Description:	Class to render tracking feedback
//
// ****************************************************************************
//
class FrameworkLinkage RTrackingFeedbackRenderer
	{
	// Destruction
	public :
		virtual								~RTrackingFeedbackRenderer( );

	// Operations
	public :
		virtual void						BeginTracking( const RRealPoint& point ) = 0;
		virtual void						ContinueTracking( const RRealPoint& point ) = 0;
		virtual void						EndTracking( ) = 0;
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const = 0;
	};

// ****************************************************************************
//
//  Class Name:	RBitmapTrackingFeedbackRenderer
//
//  Description:	Class to render smooth tracking feedback using bitmaps
//
// ****************************************************************************
//
class FrameworkLinkage RBitmapTrackingFeedbackRenderer : public RTrackingFeedbackRenderer
	{
	// Construction & Destruction
	public :
												RBitmapTrackingFeedbackRenderer( RView* pView );
		virtual								~RBitmapTrackingFeedbackRenderer( );

	// Operations
	public :
		virtual void						BeginTracking( const RRealPoint& point );
		virtual void						ContinueTracking( const RRealPoint& point );
		virtual void						EndTracking( );
		virtual void						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const;

	// Implementation
	protected :
		virtual RRealRect					GetFeedbackBoundingRect( const RRealPoint& point ) const = 0;
		virtual void						RenderFeedback( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RRealPoint& point ) const = 0;

	// Implementation
	private :
		void									CreateBitmap( const RIntSize& size,
																  RScratchBitmapImage*& pBitmap,
																  ROffscreenDrawingSurface*& pOffscreen );

		void									DeleteLastBitmap( );
		void									RenderInitialFeedback( const RRealPoint& point );
		void									ComputeRenderSettings( );
		void									ApplyBitmapRenderSettings( RDrawingSurface& drawingSurface ) const;
		void									ApplyDirectToScreenRenderSettings( RDrawingSurface& drawingSurface ) const;

	// Member data
	protected :
		RView*								m_pView;

	// Member data
	private :
		RRealRect							m_LastLogicalRect;
		RIntRect								m_LastBoundingRect;
		RScratchBitmapImage*				m_pLastBitmap;
		ROffscreenDrawingSurface*		m_pLastOffscreen;
		BOOLEAN								m_fUsedBitmapLast;
		RRealPoint							m_LastPoint;
		RSolidColor							m_BitmapFillColor;
		EBlitMode							m_BitmapBlitMode;
		RSolidColor							m_FeedbackColor;
		YDrawMode							m_FeedbackDrawMode;
		BOOLEAN								m_fTimeThresholdExceeded;
	};

// ****************************************************************************
//
//  Class Name:	RDragDropTrackingFeedbackRenderer
//
//  Description:	Class to render drag&drop tracking feedback
//
// ****************************************************************************
//
class RDragDropTrackingFeedbackRenderer : public RBitmapTrackingFeedbackRenderer
	{
	// Construction
	public :
												RDragDropTrackingFeedbackRenderer( RView* pView,
																							  RComponentCollection* pComponentCollection,
																							  RRealSize dragOffset );

	// Implementation
	protected :
		virtual RRealRect					GetFeedbackBoundingRect( const RRealPoint& point ) const;

		virtual void						RenderFeedback( RDrawingSurface& drawingSurface,
																	 const R2dTransform& transform,
																	 const RRealPoint& point ) const;

	// Member data
	private :
		RComponentCollection*			m_pComponentCollection;
		RRealSize							m_DragOffset;
		RRealRect							m_CollectionBoundingRect;
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _TRACKINGFEEDBACKRENDERER_H_
