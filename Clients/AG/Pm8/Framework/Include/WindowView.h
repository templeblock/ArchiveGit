// ****************************************************************************
//
//  File Name:			WindowView.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RWindowView class
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
//  $Logfile:: /PM8/Framework/Include/WindowView.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_WINDOWVIEW_H_
#define		_WINDOWVIEW_H_

#include	"View.h"
#include "MfcDataTransfer.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef YFloatType YZoomLevel;

class RScrollBar;
class RDragDropDataSource;
class RAutoScrollTimer;
class RTimer;

// ****************************************************************************
//
//  Class Name:	RWindowView
//
//  Description:	Class for views that are Windows or Macintosh windows
//
// ****************************************************************************
//
class FrameworkLinkage RWindowView : public RView
	{
	// Construction, destruction, & initialization
	public :
												RWindowView( );
		virtual								~RWindowView( );
		virtual void						Initialize( RDocument* pDocument );

	// Operations
	public :
		virtual RRealPoint				GetCursorPosition( ) const;
		virtual void						OnXDisplayChange( );
		virtual void						OnXResize( const RRealSize& size );
		virtual void						OnXActivate( BOOLEAN fActivating );
		virtual void						SetMouseCapture( );
		virtual void						ReleaseMouseCapture( );
		virtual void						Invalidate( BOOLEAN fErase = TRUE );
		virtual void						InvalidateVectorRect( const RRealVectorRect& rect, BOOLEAN fErase = TRUE );
		virtual void						ValidateVectorRect( const RRealVectorRect& rect );
		virtual void						SetKeyFocus( );
		virtual void						ApplyTransform( R2dTransform& transform, BOOLEAN fApplyParentsTransform, BOOLEAN fIsPrinting ) const;
		void									ScrollWindow( const RRealSize& scrollAmount );
		virtual RView*						GetLowestViewSupportingData( const RDataTransferSource& dataSource, const RRealPoint& inPoint, RRealPoint& outPoint );
		virtual void						OnHScroll( UINT nSBCode, int nPos );
		virtual void						OnVScroll( UINT nSBCode, int nPos );
		virtual void						ActivateView( );
		virtual void						AutoScroll( const RRealPoint& mousePoint, YRealDimension scrollAmount = kLineScrollAmount, BOOLEAN fIgnoreTime = FALSE, EDirection allowableDirections = kAll );
		virtual BOOLEAN					WillAutoScroll( const RRealPoint& mousePoint );
		virtual void						SetupAutoScrollTimer( const RRealPoint& mousePoint );

	// Accessors
	public :
		virtual RRealSize					GetSize( ) const;
		virtual RRealRect					GetUseableArea( const BOOLEAN fInsetFrame = TRUE ) const;
		virtual RRealRect					GetViewableArea( ) const;
		BOOLEAN								IsEventPending( ) const;
		BOOLEAN								IsKeyEventPending( ) const;
		virtual RRealSize					GetDPI( ) const;

	// Implementation
	protected :
		virtual void						RenderBackground( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;

	// Implementation
	private :
		YIntDimension						OnScroll( RScrollBar* pScrollBar, UINT nSBCode, int nPos );
		EDirection							GetAutoScrollDirection( const RRealPoint& mousePoint ) const;
		void									OnAutoScrollTimer( );
		friend class RAutoScrollTimer;

	// Member data
	protected :
		RScrollBar*							m_pHorizontalScrollBar;
		RScrollBar*							m_pVerticalScrollBar;
		YTickCount							m_NextAutoScrollTime;
		EDirection							m_AutoScrollDirection;
		RTimer*								m_pAutoScrollTimer;
		RRealSize							m_ScreenDPI;

	public :
		RRenaissanceOleDropTarget		m_OleDropTarget;
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _WINDOWVIEW_H_
