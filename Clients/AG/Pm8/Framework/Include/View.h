// ****************************************************************************
//
//  File Name:			View.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Declaration of the RView class
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
//  $Logfile:: /PM8/Framework/Include/View.h                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_VIEW_H_
#define		_VIEW_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

#ifdef	MAC

//	Since Views do not get commands and we need to have a base view
//	class that can receive commands (keys, menus, etc).  I am creating
//	a LCommandoView which is a LCommander and a LView.
//	It will be the basis for any PowerPlant extensions that
//	Renaissance needs in the X-platform framework.
//
//		Remember to always keep the Commander Target chain up to date
//	as components are activated and deactivated.
class	LCommandoView : public LView, public LCommander
	{
	//	This class has no purpose other than joining View
	//	and commander before Renaissance gets ahold of it...
	} ;

#endif	// MAC

//	Forward References
class RMouseTracker;
class RCompositeSelection;
class RPane;
class RDocument;
class RDrawingSurface;
class RChunkStorage;
class RPath;
class RComponentView;
class RComponentDocument;
class RComponentCollection;
class RDragSelectionTracker;
class RUndoableAction;
class RMergeEntry;
class RGradientParams;
class RInterface;
class RWindowView;
class RPopupMenu;
class RTrackingFeedbackRenderer;
class RSelectionTracker;
class RNudgeSelectionAction;
class RPrinterStatus;
struct RTileAttributes;
class RSettingsInterface;
class RSoftShadowSettings;
class RSoftGlowSettings;
class RSoftVignetteSettings;
class RImage;
class REdgeOutlineSettings;

//	Typedefs
typedef RList<RComponentView*>					YComponentViewCollection;
typedef YComponentViewCollection::YIterator		YComponentViewIterator;
typedef YComponentViewCollection				YZOrderState;
typedef RList<RPath*>							YAvoidancePathCollection;
typedef YAvoidancePathCollection::YIterator		YAvoidancePathIterator;
typedef YCounter								YComponentZPosition;
typedef uLONG									YComponentViewIndex;
typedef RList<RInterface*>						YInterfaceCollection;
typedef YInterfaceCollection::YIterator			YInterfaceIterator;	

// Width of the active component frame
const	int kActiveFrameWidth = 4;

// Define a resource id to indicate no supported context menu
const YResourceId kNoContextMenu = 0;

//	Cause GetUseableArea to include the frame inset in it's area calculation.
const BOOLEAN kInsetFrame = TRUE;

// Toggle selection modifier keys
const YModifierKey kToggleSelectionModifierKey = kModifierControl | kModifierShift;

// Line scroll amount
const YRealDimension kLineScrollAmount = ::InchesToLogicalUnits( 1.0 / 8.0 );

// limit resolution of printed bitmaps to reduce print times & spool files
const uLONG			kLoResBitmapXDpi =		200;
const uLONG			kLoResBitmapYDpi =		200;
const uLONG			kHiResBitmapXDpi =		300;
const uLONG			kHiResBitmapYDpi =		300;

// ****************************************************************************
//
//  Class Name:	RView
//
//  Description:	The base Renaissance view class. Component and
//						standalone view classes are derived off of this.
//						User view classes should never inherit directly
//						from this class.
//
//						This class is responsible for translating Windows specific
//						messages into Renaissance xEvents.
//
// ****************************************************************************
//
class FrameworkLinkage RView : public RCommandTarget
	{
	// Enums
	public :
		enum EDirection { kNone = 0, kUp = 1, kRight = 2, kDown = 4, kLeft = 8, kUpAndRight = 3, kUpAndLeft = 9, kDownAndRight = 6, kDownAndLeft = 12, kAll = 15 };

	// Construction, destruction, & initialization
	public :
													RView( );
		virtual										~RView( );
		virtual void								Initialize( RDocument* pDocument );

	// xEvent dispatchers						
	public :											
		virtual void								OnXMouseMessage( EMouseMessages eMessage, RRealPoint devicePoint, YModifierKey modifierKeys );
		virtual void 								OnXLButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void								OnXRButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void 								OnXMouseMove( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void 								OnXLButtonUp( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual void								OnXCharacter( const RCharacter& character, YModifierKey modifierKeys );
		virtual void								OnXKeyDown( EVirtualCharacter eChar, YModifierKey modifierKeys );
		virtual void								OnXKeyUp( EVirtualCharacter eChar, YModifierKey modifierKeys );
		virtual void								OnXSetCursor( const RRealPoint& mousePoint, YModifierKey modifierKeys );
		virtual RAction*							CreateScriptAction( const YActionId& actionId, RScript& script );
		virtual void								OnXDisplayChange( );
		virtual void								OnXSetKeyFocus( );
		virtual void								OnXKillKeyFocus( );
		virtual void								XUpdateAllViews( EUpdateTypes updateType, uLONG lParam );
		virtual void								OnXUpdateView( EUpdateTypes updateType, uLONG lParam );
		void			 							OnXRender( RDrawingSurface& drawingSurface, RIntRect& rcRender );
		virtual void								OnXActivate( BOOLEAN fActivating );
		virtual void								OnXActivateApp( BOOLEAN fActivating );
		virtual void								OnXContextMenu( const RRealPoint& mousePoint );
														
	// Operations									
	public :											
		virtual void								UpdateCursor( );
		virtual RRealPoint							GetCursorPosition( ) const = 0;
		virtual void 								SetMouseCapture( );
		virtual void 								ReleaseMouseCapture( );
		virtual void								Invalidate( BOOLEAN fErase = TRUE ) = 0;
		virtual void								InvalidateVectorRect( const RRealVectorRect& rect, BOOLEAN fErase = TRUE ) = 0;
		virtual void								ValidateVectorRect( const RRealVectorRect& rect ) = 0;
		virtual void								Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		virtual void								RenderShadow( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, const RSolidColor& shadowColor ) const;
		virtual void								RenderComponentWithSoftEffects( RComponentView* pView, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, BOOLEAN renderBehind = FALSE ) const;
		virtual void								AddComponentView( RComponentView* pView );
		virtual void								RemoveComponentView( RComponentView* pView );
		YComponentViewIndex							GetIndexOfComponentView( RComponentView* pView );
		RComponentView*								GetComponentViewByIndex( YComponentViewIndex index );
		RComponentView*								GetActiveComponent( ) const;
		void										ActivateComponentView( RComponentView* pView );
		RComponentView*								DeactivateComponentView( );
		virtual void								SetKeyFocus( ) = 0;
		virtual void								SetClipRegion( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const;
		void										SelectRectangle( const RRealRect& rect );
		void										ToggleSelectRectangle( const RRealRect& rect );
		const YZOrderState&							GetZOrderState( ) const;
		void										SetZOrderState( const YZOrderState& zOrder );
		YComponentZPosition							GetComponentZPosition( const RComponentView* pView ) const;
		void										SetComponentZPosition( RComponentView* pView, YComponentZPosition );
		virtual void								GetViewTransform( R2dTransform& transform, RDrawingSurface& drawingSurface, BOOLEAN fApplyParentsTransform ) const;
		virtual void								GetViewTransform( R2dTransform& transform, BOOLEAN fApplyParentsTransform ) const;
		virtual void								ApplyTransform( R2dTransform& transform, BOOLEAN fApplyParentsTransform, BOOLEAN fPrinting ) const;
		virtual void								Read( RChunkStorage& storage );
		virtual void								Write( RChunkStorage& storage ) const;
		YModifierKey								GetModifierKeyState( ) const;
		virtual BOOLEAN								ComputeAvoidancePaths( RComponentView* pView, YAvoidancePathCollection& avoidanceCollection, const R2dTransform& transform, BOOLEAN fBlock ) const;
		virtual void								ActivateView( ) = 0;
		RRealRect									GetBoundingRectOfEmbeddedComponents( BOOLEAN fIncludeSelectionHandles ) const;
		virtual void								UpdateScrollBars( EScrollUpdateReason scrollUpdateReason ) = 0;
		virtual YDropEffect							OnXDragEnter( RDataTransferSource& dataSource, YModifierKey modifierKeys, const RRealPoint& point );
		virtual YDropEffect							OnXDragOver( RDataTransferSource& dataSource, YModifierKey modifierKeys, const RRealPoint& localPoint );
		virtual void								OnXDragLeave( );
		virtual void								OnXDrop( RDataTransferSource& dataSource, const RRealPoint& point, YDropEffect dropEffect );
		virtual RView*								GetLowestViewSupportingData( const RDataTransferSource& dataSource, const RRealPoint& inPoint, RRealPoint& outPoint );
		virtual RUndoableAction*					GetDragDropDeleteAction( ) const;
		virtual BOOLEAN								HasMergeFields( ) const;
		virtual BOOLEAN								Associate( RMergeEntry* pMergeEntry );
		RMergeEntry*								GetMergeEntry( ) const;
		virtual RColor								GetBackgroundColor( ) const;
		void										CreateInterfaceCollection( YInterfaceId interfaceId, YInterfaceCollection& collection, BOOLEAN fRecurs, BOOLEAN fSelection ) const;
		void										BringForward( RComponentView* pComponentView );
		void										BringToFront( RComponentView* pComponentView );
		void										SendBackward( RComponentView* pComponentView );
		void										SendToBack( RComponentView* pComponentView );
		void										GetComponentsToBringForward( YComponentViewCollection& collection ) const;
		void										GetComponentsToSendBackward( YComponentViewCollection& collection ) const;
		virtual RPopupMenu*							GetContextMenu( ) const;
		void										GetFeedbackSettings( RSolidColor& color, YDrawMode& drawMode ) const;
		virtual void							CallEditComponent( RComponentView* pComponent, EActivationMethod activationMethod, const RRealPoint& mousePoint );
		virtual RComponentView*						EditComponent( RComponentView* pComponent,
																			EActivationMethod activationMethod,
																			const RRealPoint& mousePoint,
																			YResourceId& editUndoRedoPairId,
																			uLONG ulEditInfo );
														
		void										CancelTracking( );
		virtual void								AutoScroll( const RRealPoint& mousePoint, YRealDimension scrollAmount = kLineScrollAmount, BOOLEAN fIgnoreTime = FALSE, EDirection allowableDirections = kAll ) = 0;
		virtual BOOLEAN								WillAutoScroll( const RRealPoint& mousePoint ) = 0;
		virtual void								SetupAutoScrollTimer( const RRealPoint& mousePoint ) = 0;
		virtual void								OnChangeSelection( );
		virtual void								ScaleComponentToFit( RComponentView* pComponent, const RRealPoint& point, const RRealSize& scaleFactor );
		virtual void								OnFrameComponent( RComponentView* pComponent );
														
		virtual void								RenderSurfaceAdornments( RComponentView* pView, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		virtual void								RenderBehindComponentsWithSoftEffects( const YComponentViewCollection& fComponentViewCollection, RComponentView* pView, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		virtual void								AdjustRectForSoftEffectsBehind( const YComponentViewCollection& fComponentViewCollection, const RComponentView* pView, const R2dTransform& transform, RRealRect& rect ) const;
		virtual void								RenderBackground( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const = 0;

		virtual void								RectInView( const RRealVectorRect& rect );

		void										SetAlphaMask( RBitmapImage * pMask, DWORD dwMaskID );
		void										ClearAlphaMask( );

	//	Accessors									
	public :											
		RDocument*									GetRDocument( ) const;
		virtual RRealSize							GetReferenceSize( ) const;
		virtual RRealSize							GetSize( ) const = 0;
		virtual RRealSize							GetDPI( ) const = 0;
		BOOLEAN										IsMouseCaptured( ) const;
		virtual RRealRect							GetUseableArea( const BOOLEAN fInsetFrame = TRUE ) const = 0;
		virtual RRealRect							GetViewableArea( ) const = 0;
		virtual RRealPoint							ConvertPointToLocalCoordinateSystem( const RRealPoint& point ) const;
		virtual RComponentView*						GetHitComponentView( const RRealPoint& point, BOOLEAN fCheckNonSelectable = FALSE ) const;
		RComponentView*								GetComponentViewWithUniqueId( YUniqueId uniqueId ) const;
		virtual BOOLEAN								IsEventPending( ) const = 0;
		virtual BOOLEAN								IsKeyEventPending( ) const = 0;
		RRealPoint									GetInsertionPoint( const RRealSize& size );
		virtual RCompositeSelection*				GetSelection( ) const;
		virtual RSelectionTracker*					GetResizeTracker( ) const;
		virtual RSelectionTracker*					GetRotateTracker( ) const;
		YCounter									GetNumComponents( ) const;
		YComponentViewIterator						GetViewCollectionStart( );
		YComponentViewIterator						GetViewCollectionEnd( );
		virtual RSettingsInterface*					GetMergedSettings( YCommandID commandId, RCompositeSelection* pSelection ) const;
		virtual RSettingsInterface*					GetMergedSettings( YInterfaceId interfaceId, RCompositeSelection* pSelection ) const;
		BOOLEAN										NeedsOffscreenDrawingEnvironment( RComponentView* pView ) const;
		virtual void								EnableRenderCache( BOOLEAN fEnable );
		
	// Implementation
	protected :
		virtual void								RenderDocument( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		virtual void								RenderSelection( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		void										RenderComponent( RComponentView* pView, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		void										RenderComponents( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, RPrinterStatus* pPrinterStatus = NULL ) const;
		void										RenderActiveTracker( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const;
		void										RenderActiveComponent( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		void										RenderActiveFrame( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const;
		void										ResetInsertionPoint ( );
		void										SetupMouseDownState( const RRealPoint& point, YModifierKey modifierKeys );
		void										GetLockState( BOOLEAN& fLockedObjectFound, BOOLEAN& fUnlockedObjectFound ) const;
		virtual void								DoDragDrop( const RRealPoint& mousePoint, BOOLEAN fUseTargetDefaultSize, YDropEffect allowableEffects = kDropEffectMove | kDropEffectCopy );
		BOOLEAN										ShouldStartTracking( const RRealPoint& mousePoint ) const;
		void										ResetComponentsToDefaultSize( RComponentCollection& collection );
		virtual RTrackingFeedbackRenderer*			GetDragDropFeedbackRenderer( );

		virtual BOOLEAN						HasOldStyleShadow() const;

	// Implementation
	private :
		void										SelectNextComponent( );
		void										SelectPreviousComponent( );
		BOOLEAN										ShouldSelectionHandleClick( const RRealPoint& point, YModifierKey modifierKeys ) const;
		BOOLEAN										IsZOrderChainHead( RComponentView* pComponentView ) const;
		BOOLEAN										IsZOrderChainTail( RComponentView* pComponentView ) const;
		BOOLEAN										GetShadowState( BOOLEAN& fUnShadowedComponentFound ) const;
		BOOLEAN										GetGlowState( BOOLEAN& fUnGlowedComponentFound ) const;
		BOOLEAN										GetVignetteState( BOOLEAN& fUnVignettedComponentFound ) const;
		void										NudgeSelection( EDirection nudgeDirection, YModifierKey modifierKeys );
														
	// Command handlers							
	private :										
		void										OnPaste( );
		void										OnUpdatePaste( RCommandUI& commandUI ) const;
		void										OnPasteHere( );
		void										OnSelectAll( );
		void										OnUpdateSelectAll( RCommandUI& commandUI ) const;
		void										OnGroup( );
		void										OnUpdateGroup( RCommandUI& commandUI ) const;
		void										OnCreateGroup( );
		void										OnUpdateCreateGroup( RCommandUI& commandUI ) const;
		void										OnBreakGroup( );
		void										OnUpdateBreakGroup( RCommandUI& commandUI ) const;
		void										OnEditComponentAttributes( );
		void										OnUpdateEditComponentAttributes( RCommandUI& commandUI ) const;
		void										OnCut( );
		void										OnUpdateCut( RCommandUI& commandUI ) const;
		void										OnCopy( );
		void										OnUpdateCopy( RCommandUI& commandUI ) const;
		void										OnDuplicate( );
		void										OnUpdateDuplicate( RCommandUI& commandUI ) const;
		void										OnScale( );
		void										OnUpdateScale( RCommandUI& commandUI ) const;
		void										OnAlign( );
		void										OnUpdateAlign( RCommandUI& commandUI ) const;
		void										OnRotate( );
		void										OnRotateLeft90( );
		void										OnRotateRight90( );
		void										OnRotate( YAngle angleRotation );
		void										OnUpdateRotate( RCommandUI& commandUI ) const;
		void										OnLock( );
		void										OnUpdateLock( RCommandUI& commandUI ) const;
		void										OnDelete( );
		void										OnUpdateDelete( RCommandUI& commandUI ) const;
		void										OnFrame( YCommandID commandId );
		void										OnUpdateFrame( RCommandUI& commandUI ) const;
		void										OnComponentCommand( YCommandID commandId );
		void										OnUpdateComponentCommand( RCommandUI& commandUI ) const;
		void										OnInsertNonstandard( YCommandID commandId );
		void										OnFlipHorizontal( );
		void										OnFlipVertical( );
		void										OnFlipBoth( );
		void										OnUpdateFlip( RCommandUI& commandUI ) const;
		void										OnShadow( );
		void										OnUpdateShadow( RCommandUI& commandUI ) const;
		void										OnGlowOnOff( );
		void										OnUpdateGlowOnOff( RCommandUI& commandUI ) const;
		void										OnVignetteOnOff( );
		void										OnUpdateVignetteOnOff( RCommandUI& commandUI ) const;
		void										OnBringForward( );
		void										OnUpdateBringForward( RCommandUI& commandUI ) const;
		void										OnSendBackward( );
		void										OnUpdateSendBackward( RCommandUI& commandUI ) const;
		void										OnBringToFront( );
		void										OnUpdateBringToFront( RCommandUI& commandUI ) const;
		void										OnSendToBack( );
		void										OnUpdateSendToBack( RCommandUI& commandUI ) const;
		void										OnEditSelected( );
		void										OnSetUseOffscreens( );
		void										OnUpdateSetUseOffscreens( RCommandUI& commandUI ) const;
		void										OnCenterInView( );
		void										OnUpdateCenterInView( RCommandUI& commandUI ) const;
		void										OnScaleToView( );
		void										OnSizeToView( );
		void										OnUpdateScaleToView( RCommandUI& commandUI ) const;
		void										OnColorBehindObject( );
		void										OnUpdateColorBehindObject( RCommandUI& commandUI ) const;
		void										OnAlignToViewLeft( );
		void										OnAlignToViewRight( );
		void										OnAlignToViewTop();
		void										OnAlignToViewBottom();

														
	// Member data									
	protected :										
		RComponentView*								m_pActiveComponent;
		RMouseTracker*								m_pActiveTracker;
		RCompositeSelection*						m_pSelection;
		YComponentViewCollection					m_ComponentViewCollection;
		YCounter									m_MouseDownCount;
		RRealPoint									m_MouseDownPoint;
		YTickCount									m_MouseDownTime;
		YModifierKey								m_MouseDownModifierKeys;
		RComponentCollection*						m_pComponentCollection;
		BOOLEAN										m_fDeleteComponents;
		RRealSize									m_DragOffset;
		static BOOLEAN								m_fDroppedInRenaissance;
		BOOLEAN										m_fTiledView;		
		RMergeEntry*								m_pMergeEntry;
		BOOLEAN										m_fTrackSelection;
		RRealPoint									m_ContextMenuPoint;
		BOOLEAN										m_fIgnoreZOrder;
		RSoftShadowSettings*						m_pShadowSettings;
		RSoftGlowSettings*							m_pGlowSettings;
		RSoftVignetteSettings*						m_pVignetteSettings;
		RBitmapImage*								m_pAlphaMask;
		DWORD										m_dwMaskID;
		int											m_kSpecialEdgeType;
		REdgeOutlineSettings*						m_pEdgeOutlineSettings;

														
														
	//	Member data									
	private:											
		BOOLEAN										m_fMouseCaptured;
		RDocument*									m_pDocument;
		RRealPoint									m_InsertionPoint;
		RTrackingFeedbackRenderer*					m_pFeedbackRenderer;
		RNudgeSelectionAction*						m_pNudgeAction;
		BOOLEAN										m_fToggleSelecting;
		BOOLEAN										m_fInitialSelection;
		BOOLEAN										m_fDragDropConvertedComponent;
		BOOLEAN										m_fHiResPhoto;
														
	// Command map									
	public :											
		virtual RCommandMapBase*					GetRCommandMap( ) const;

	private :
		static RCommandMap<RView, RCommandTarget>	m_CommandMap;

		friend class RCommandMap<RView, RCommandTarget>;

	// Mac specific stuff
#ifdef	MAC

	//	Conversions
	public :
		virtual	LCommandoView&						GetCommandoView( ) const = 0;
		RIntPoint									WindowQDPointToViewRPoint( const Point& ptQD ) const;
		Point										ViewRPointToWindowQDPoint( const RIntPoint& rptView ) const;

	protected :
		void										ProcessMouseDown( const Point ptPortMouse, YModifierKey yModifiers );

#endif	// MAC

	// Windows specific stuff
#ifdef	_WINDOWS

	// Conversion operators
	public :
		virtual CWnd&						GetCWnd( ) const = 0;

#endif	// _WINDOWS

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const = 0;
#endif
	};

RView::EDirection& operator|=( RView::EDirection& lhs, const RView::EDirection& rhs );

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
inline RCommandMapBase* RView::GetRCommandMap( ) const
	{
	return &m_CommandMap;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_VIEW_H_
