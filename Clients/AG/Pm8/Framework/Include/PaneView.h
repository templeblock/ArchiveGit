// ****************************************************************************
//
//  File Name:			PaneView.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RPaneView class
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
//  $Logfile:: /PM8/Framework/Include/PaneView.h                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_PANEVIEW_H_
#define	_PANEVIEW_H_

#ifndef	_VIEW_H_
#include "View.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:		RPaneView
//
//  Description:		Base class for pane views; that is views that are embedded
//							within another view.
//
// ****************************************************************************
//
class FrameworkLinkage RPaneView : public RView
	{
	// Construction, destruction, & initialization
	public :
												RPaneView( RDocument* pDocument, RView* pParentView  );

	// Operations
	public :
		virtual void						SetMouseCapture( );
		virtual void						ReleaseMouseCapture( );
		virtual void						SetKeyFocus( );
		virtual RRealPoint				GetCursorPosition( ) const;
		virtual void						ApplyTransform( R2dTransform& transform, BOOLEAN fApplyParentsTransform, BOOLEAN fIsPrinting ) const;
		virtual void						InvalidateVectorRect( const RRealVectorRect& rect, BOOLEAN fErase = TRUE );
		virtual void						ValidateVectorRect( const RRealVectorRect& rect );
		virtual void						ActivateView( );
		virtual void						UpdateScrollBars( EScrollUpdateReason scrollUpdateReason );
		virtual void						Read( RChunkStorage& storage );
		virtual void						Write( RChunkStorage& storage ) const;
		virtual RComponentView*			EditComponent( RComponentView* pComponent,
																	EActivationMethod activationMethod,
																	const RRealPoint& mousePoint,
																	YResourceId& editUndoRedoPairId,
																	uLONG ulEditInfo );

		virtual void						AutoScroll( const RRealPoint& mousePoint, YRealDimension scrollAmount = kLineScrollAmount, BOOLEAN fIgnoreTime = FALSE, EDirection allowableDirections = kAll );
		virtual BOOLEAN					WillAutoScroll( const RRealPoint& mousePoint );
		virtual void						SetupAutoScrollTimer( const RRealPoint& mousePoint );
		virtual void						OnFrameComponent( RComponentView* pComponent );
		void									SetPaneBleed( RIntSize bleedAmount );
		RIntSize								GetPaneBleed( ) const;

		virtual void						RectInView( const RRealVectorRect& rect );

	// Implementation
	public :
		void									RenderBackground( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		virtual void						RenderSurfaceAdornments( RComponentView* pView, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		virtual void						RenderBehindComponentsWithSoftEffects( const YComponentViewCollection& fComponentViewCollection, RComponentView* pView, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		virtual void						AdjustRectForSoftEffectsBehind( const YComponentViewCollection& fComponentViewCollection, const RComponentView* pView, const R2dTransform& transform, RRealRect& rect ) const;

	// Accessors
	public :
		virtual RRealSize					GetDPI( ) const;
		void									SetParentView( RView* pParentView );
		RView*								GetParentView( ) const;
		virtual RRealRect					GetUseableArea( const BOOLEAN fInsetFrame = TRUE ) const;
		virtual RRealRect					GetViewableArea( ) const;
		virtual RColor						GetBackgroundColor( ) const;
		virtual void						SetBackgroundColor( const RColor& rBackgroundColor );
		BOOLEAN								IsEventPending( ) const;
		BOOLEAN								IsKeyEventPending( ) const;
		virtual void						SetClipRegion( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const;

	// Implementation
	public :
		BOOLEAN								HasBackgroundColor( ) const;
		BOOLEAN								HasPaneBleed( ) const;

	// Member data
	protected :
		RColor								m_rBackgroundColor;
		RIntSize								m_BleedAmount;

	// Member data
	private :
		RView*								m_pParentView;

	// Command map
	public :
		virtual RCommandMapBase*		GetRCommandMap( ) const;

	private :
		static RCommandMap<RPaneView, RView> m_CommandMap;
		friend class RCommandMap<RPaneView, RView>;

	// Platform specific conversion operators
	public :
#ifdef	_WINDOWS
		virtual CWnd&						GetCWnd( ) const;
#else
		virtual	LCommandoView&			GetCommandoView( ) const;
#endif

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const = 0;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RPaneView::GetRCommandMap( )
//
//  Description:		Gets a pointer to this classes command map.
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCommandMapBase* RPaneView::GetRCommandMap( ) const
	{
	return &m_CommandMap;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif _PANEVIEW_H_
