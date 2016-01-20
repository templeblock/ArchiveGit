// ****************************************************************************
//
//  File Name:			ComponentView.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Declaration of the RComponentView class
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
//  $Logfile:: /PM8/Framework/Include/ComponentView.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COMPONENTVIEW_H_
#define		_COMPONENTVIEW_H_

#include		"PaneView.h"
#include		"RenderCache.h"
#include		"TileAttributes.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RSingleSelection;
class RPath;
class RSettings;
class RComponentAttributes;
class RComponentDocument;
class RInterface;
class RFrame;
class RComponentRenderCache;
class RTileAttributesExt;
class RSpecialEdgeSettings;

// ****************************************************************************
//
//  Class Name:		RComponentView
//
//  Description:		Base class for Component Renaissance views.
//
// ****************************************************************************
//
class FrameworkLinkage RComponentView : public RPaneView
	{
	// Construction, destruction, & initialization
	public :
											RComponentView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView );
		virtual							~RComponentView( );
												
	// Operations							
	public :									
		virtual void					OnXEditComponent( EActivationMethod activationMethod, const RRealPoint& mousePoint ) = 0;
		virtual YDropEffect			OnXDragEnter( RDataTransferSource& dataSource, YModifierKey modifierKeys, const RRealPoint& point );
		virtual YDropEffect			OnXDragOver( RDataTransferSource& dataSource, YModifierKey modifierKeys, const RRealPoint& point );
		virtual void					OnXDragLeave( );
		virtual void					OnXDrop( RDataTransferSource& dataSource, const RRealPoint& point, YDropEffect dropEffect );
		virtual void					OnXDisplayChange( );
		virtual void					ActivateComponent( );
		virtual void					DeactivateComponent( );
		virtual void					ApplyTransform( R2dTransform& transform, BOOLEAN fApplyParentsTransform, BOOLEAN fIsPrinting ) const;
		virtual RAction*				CreateScriptAction( const YActionId& actionId, RScript& script );
		virtual BOOLEAN				HitTest( const RRealPoint& point ) const;
		BOOLEAN							IsIntersecting( const RRealRect& rect ) const;
		BOOLEAN							IsInRect( const RRealRect& rect ) const;
		virtual void					Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		RRealRect						Render( RDrawingSurface& drawingSurface, const RRealRect& destRect, const RRealSize& extraScale=RRealSize(1.0,1.0) ) const;
		virtual void					RenderDocument( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		virtual void					RenderDocumentAndSoftEffects( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, const RRealSize& size ) const;
		virtual void					RenderShadowInSingleLayer( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, const RRealSize& size, const RSolidColor& shadowColor ) const;
		virtual void					RenderFrame( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RSolidColor& frameColor ) const;
		virtual BOOLEAN				WantsGlowAndShadowInsideBounds() const;
		virtual YRealDimension		SoftEffectDimension( const RRealSize& size ) const;
		virtual void					Invalidate( BOOLEAN fErase = TRUE );
		virtual void					Read( RChunkStorage& storage );
		virtual void					Write( RChunkStorage& storage ) const;
		virtual void					Offset( const RRealSize& offset, BOOLEAN fInvalidate = TRUE );
		virtual void					Rotate( const RRealPoint& centerOfRotation, YAngle angle );
		virtual void					RotateAboutCenter( YAngle angle );
		virtual void					Scale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio );
		virtual void					AlternateScale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio );
		virtual void					Resize( const RRealSize& newSize );
		virtual void					FlipHorizontal( YRealCoordinate yAxis );
		virtual void					FlipVertical( YRealCoordinate xAxis );
		virtual void					DrawSelectionFrame( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const;
		virtual BOOLEAN				HitTestSelectionFrame( const RRealPoint& point ) const;
		virtual RRealSize				ApplyResizeConstraint( const RRealSize& scaleFactor ) const;
		virtual void					RenderTrackingFeedback( RDrawingSurface& drawingSurface,
																				const R2dTransform& transform,
																				const RView& targetView ) const;
		virtual void					RenderInternalTrackingFeedback( RDrawingSurface& drawingSurface,
																				const R2dTransform& transform,
																				const RView& targetView ) const;
		RRealRect						GetTrackingFeedbackBoundingRect( const R2dTransform& transform ) const;

		virtual void					ApplyTint( YTint tint );
		virtual BOOLEAN				GetColor( RColor& color ) const;
		virtual void					ApplyColor( const RColor& color );
		virtual RPopupMenu*			GetContextMenu( ) const;
		void								InvalidateRenderCache( );
		void								SetRenderCacheThreshold( YTickCount renderThreshold );
		virtual void					EnableRenderCache( BOOLEAN fEnable );

		virtual void					FitToParent();
		virtual void					FitInsideParent();
		virtual void					FitFullSoftEffectsInsideParent();
		virtual void					FitOutsideParent();

		void								CopyViewAttributes( RComponentView* pSource );
		virtual void					CopyBoundingRect( RComponentView* pSource );
		virtual void					CopyTransform( RComponentView* pSource );

	//	Functions for supporting extensions to the API through components
	public :
		virtual BOOLEAN				GetInterfaceId( YCommandID commandId, YInterfaceId& interfaceId ) const;
		virtual RInterface*			GetInterface( YInterfaceId id ) const;

	// Accessors
	public :
		RComponentDocument*			GetComponentDocument( ) const;
		const YComponentBoundingRect&	GetBoundingRect( ) const;
		virtual void					SetBoundingRect( const YComponentBoundingRect& boundingRect );
		const YComponentBoundingRect&	GetFullSoftEffectsBoundingRect( ) const;
		virtual RRealSize				GetSize( ) const;
		RRealSize						GetReferenceSize( ) const;
		BOOLEAN							IsActive( ) const;
		const RComponentAttributes&	GetComponentAttributes( ) const;
		EFrameType						GetFrameType( ) const;
		RFrame*							GetFrame( ) const;
		virtual void					SetFrame( EFrameType eNewFrame );
		virtual RSolidColor			GetFrameColor( ) const;
		virtual void					SetFrameColor( const RSolidColor rNewColor );
		virtual RRealRect				GetUseableArea( const BOOLEAN fInsetFrame = TRUE ) const;
		BOOLEAN							IsFlippedHorizontal( ) const;
		BOOLEAN							IsFlippedVertical( ) const;
		void								ClearFlippedFlags( );
		virtual void					AddOutline( YAvoidancePathCollection& avoidanceCollection, const R2dTransform& transform, BOOLEAN fBlock );
		virtual void					CalculateBorderPath( RPath* pPath );
		virtual void					CalculateOutlinePath( );
		virtual BOOLEAN				IntersectsRect( const RRealRect& targetRect ) const;
		BOOLEAN							IntersectsFullSoftEffectsRect( const RRealRect& targetRect ) const;

		BOOLEAN							HasShadow( ) const;
		BOOLEAN							HasShadowOnThisOrSubViews( ) const;
		void								GetShadowSettings( RSoftShadowSettings& fShadowSettings ) const;
		void								SetShadowSettings( const RSoftShadowSettings& fShadowSettings );

		BOOLEAN							HasGlow( ) const;
		BOOLEAN							HasGlowOnThisOrSubViews( ) const;
		void								GetGlowSettings( RSoftGlowSettings& fGlowSettings ) const;
		void								SetGlowSettings( const RSoftGlowSettings& fGlowSettings );

		BOOLEAN							HasVignette( ) const;
		void								GetVignetteSettings( RSoftVignetteSettings& fVignetteSettings ) const;
		void								SetVignetteSettings( const RSoftVignetteSettings& fVignetteSettings );
		BOOLEAN							HasEdgeOutline( ) const;
		void								GetEdgeOutlineSettings( REdgeOutlineSettings& edgeOutlineSettings ) const;
		void								SetEdgeOutlineSettings( const REdgeOutlineSettings& edgeOutlineSettings );
		void								UndefineOutlinePath( );

		BOOLEAN							HasAlphaMask( ) const;
		void								GetSpecialEdgeSettings( RSpecialEdgeSettings& fSpecialEdgeSettings) const;
		void								SetSpecialEdgeSettings( const RSpecialEdgeSettings& fSpecialEdgeSettings);

		YAngle							GetRotationAngle( ) const;
		BOOLEAN							IsRenderActiveFrame( ) const;
		void								SetRenderActiveFrame( BOOLEAN fRender );
		virtual RRealSize				GetMinimumSize( ) const;
		virtual RRealSize				GetMaximumSize( ) const;
		virtual BOOLEAN				MaintainAspectRatioIsDefault( ) const;
		virtual BOOLEAN				CanChangeDefaultAspect( ) const;
		virtual BOOLEAN				CanResizeHorizontal( ) const;
		virtual BOOLEAN				CanResizeVertical( ) const;
		virtual BOOLEAN				CanSingleClickActivate( const RRealPoint& point ) const;
		virtual void					SetBackgroundColor( const RColor& rBackgroundColor );
		virtual BOOLEAN				EnableRenderCacheByDefault( ) const;
		virtual BOOLEAN				HasSoftEffects( ) const;
		virtual void					AdjustRectForSoftEffect( RRealRect& rect, RRealSize& softEffectOffsetSize ) const;
		virtual void					AdjustRectForSoftEffectOrVignette( RRealRect& rect, RRealSize& softEffectOrVignetteOffsetSize ) const;

		// Tiling
		virtual void					TileView( const RTileAttributes& rTileAttributes, const RTileAttributesExt *prTileAttributesExt = NULL );
		virtual void					UntileView( );
		virtual BOOLEAN				IsViewTiled( ) const;
		virtual RTileAttributes		GetTileAttributes( ) const;
		virtual RTileAttributesExt	GetTileAttributesExt( ) const;

	// Implementation
	protected :
		YComponentBoundingRect		GetInteriorBoundingRect( ) const;
		void								RenderBackground( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;

	// Implementation
	private :
		void								DoRender( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		void								DoRenderBackground( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const;
		void								AdjustVectorRectForSoftEffect( RRealVectorRect& vectorRect ) const;
		void								GetRectOffsetsForSoftEffect( const RRealVectorRect& vectorRect, YRealDimension& leftAdjust, YRealDimension& topAdjust, YRealDimension& rightAdjust, YRealDimension& bottomAdjust ) const;

	// Member data
	protected :
		enum EActiveState { kInactive, kActive, kActivating };
		EActiveState					m_eActiveState;
		EFrameType						m_eFrameType;
		RFrame*							m_pFrame;
		RSolidColor						m_rFrameColor;
		RPath*							m_pOutlinePath;
		BOOLEAN							m_fRenderActiveFrame;
		RComponentRenderCache*		m_pRenderCache;
		RTileAttributes*				m_pTileAttributes;
		RTileAttributesExt*			m_pTileAttributesExt;
		RRealSize						m_AspectCompensation;				//	amount by which object's size is reduced to make it aspect correct when it's frame is applied.
		BOOLEAN							m_fHorizontallyFlipped;
		BOOLEAN							m_fVerticallyFlipped;

	// Command map
	public :
		virtual RCommandMapBase*	GetRCommandMap( ) const;

	private :
		static RCommandMap<RComponentView, RPaneView> m_CommandMap;
		friend class RCommandMap<RComponentView, RPaneView>;
		friend class RComponentRenderCache;

		YComponentBoundingRect		m_BoundingRect;
		YComponentBoundingRect		m_FullSoftEffectsBoundingRect;

#ifdef	TPSDEBUG								
	// Debugging stuff						
	public :										
		virtual void					Validate( ) const = 0;
#endif
	};

// ****************************************************************************
//
// Class Name:			RComponentRenderCache
//
// Description:		Render cache for components
//
// ****************************************************************************
//
class RComponentRenderCache : public RRenderCache
	{
	// Construction
	public :
										RComponentRenderCache( const RComponentView* pComponent );

	// Implementation
	private :
		virtual void					RenderData( RDrawingSurface& drawingSurface,
																const RRealSize& size,
																const R2dTransform& transform,
																const RIntRect& rcRender ) const;

	// Member data
	private :
		const RComponentView*		m_pComponent;
	};

// ****************************************************************************
//
// Class Name:			RTilingRenderCache
//
// Description:		Render cache for tiling components
//
// ****************************************************************************
//
class RTilingRenderCache : public RRenderCache
	{
	// Construction
	public :
											RTilingRenderCache( const RComponentView* pComponent );

	protected:
		
		virtual RBitmapImageBase *	AllocateCacheBitmap( RDrawingSurface& referenceSurface, uLONG uWidthInPixels, uLONG uHeightInPixels );
		virtual RBitmapImageBase *	AllocateCacheMask( uLONG uWidthInPixels, uLONG uHeightInPixels );

	// Implementation
	private :
		void								RenderData( RDrawingSurface& drawingSurface,
																const RRealSize& size,
																const R2dTransform& transform,
																const RIntRect& rcRender ) const;

	// Member data
	private :
		const RComponentView*		m_pComponent;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RView::GetRCommandMap( )
//
//  Description:		Gets a pointer to this classes command map.
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCommandMapBase* RComponentView::GetRCommandMap( ) const
	{
	return &m_CommandMap;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::IsActive( )
//
//  Description:		Accessor
//
//  Returns:			TRUE if this component view is currently active
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RComponentView::IsActive( ) const
	{
	return static_cast<BOOLEAN>( m_eActiveState == kActive );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetRotationAngle( )
//
//  Description:		Accessor
//
//  Returns:			The angle which this component is rotated about its
//							center.
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YAngle RComponentView::GetRotationAngle( ) const
	{
	return m_BoundingRect.GetRotationAngle( );
	}

#ifdef	MAC

// ****************************************************************************
//
//  Function Name:	RComponentView::GetFrameworkView( )
//
//  Description:		Converts to a TView reference. This is a virtual casting
//							operator used to convert from an RView to a TView&. This
//							can not be done automatically because of the inheritance
//							hierarchy.
//
//  Returns:			The TView&
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline LCommandoView& RComponentView::GetCommandoView( ) const
	{
	return m_pParent->GetCommandoView();
	}

#endif	// MAC

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _COMPONENTVIEW_H_
