// ****************************************************************************
//
//  File Name:			HeadlineGraphic.h
//
//  Project:			Headine Component
//
//  Author:				R. Hood
//
//  Description:		Declaration of the RHeadlineGraphic class
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
//  $Logfile:: /PM8/HeadlineComp/Include/HeadlineGraphic.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_HEADLINEGRAPHIC_H_
#define		_HEADLINEGRAPHIC_H_

#ifndef		_PATH_H_
#include		"Path.h"
#endif

#ifndef		_GLYPHGRAPHIC_H_
#include		"SingleGraphic.h"
#endif

#ifndef		_HEADLINECOLOR_H_
#include		"HeadlineColor.h"
#endif

#ifndef		_HEADLINEINTERFACE_H_
#include		"HeadlineInterface.h"
#endif

// forward referenced classes
class	RView;

// defines
const uWORD kMaxHeadlineCharacters = 1024;
const uWORD kMaxHeadlineLines = 32;

// iterators
typedef	RArray<RCharacter>							RCharacterArray;
typedef	RCharacterArray::YIterator					RCharacterArrayIterator;
typedef	RArray<RMBCString*>							RMBCStringArray;
typedef	RMBCStringArray::YIterator					RMBCStringArrayIterator;
typedef	RArray<R2dTransform>							R2dTransformArray;
typedef	R2dTransformArray::YIterator				R2dTransformArrayIterator;
typedef	RArray<RIntRect>								RIntRectArray;
typedef	RIntRectArray::YIterator					RIntRectArrayIterator;
typedef	RArray<YAngle>									YAngleArray;
typedef	YAngleArray::YIterator						YAngleArrayIterator;


// ****************************************************************************
//
// Class Name:			RHeadlineGraphic
//
// Description:		Class to 
//
// ****************************************************************************
//
class RHeadlineGraphic : public RSingleGraphic					 
{
	// enums
	private :
		enum		EPathTypes				{ 	kPathTop, kPathBottom, kPathFollow, kPathStraight, kPathConcave, kPathConvex, kPathCurved };
		enum		EScaleConstraint		{ 	kByHorizontal, kByVertical, kByHorizontalAndVertical };
		enum		EFlipDirection			{	kFlipNone, kFlipHorizontal, kFlipVertical, kFlipBoth };

	// Construction, Destruction & Initialization
	public :
												RHeadlineGraphic( );
												RHeadlineGraphic( const RHeadlineGraphic &rhs );
		virtual								~RHeadlineGraphic( );
		BOOLEAN								Initialize( );

	// Accessors
	public :
		static RIntSize  					GetDefaultHeadlineSize( BOOLEAN fHorizontal = TRUE );
		RFont*								GetFont( ) const;
		RMBCString							GetText( ) const;
		ETextCompensation					GetCharCompensation( ) const;
		YPercentage							GetCompensationAdjustment( ) const;
		ETextEscapement					GetTextEscapement( ) const;
		ETextJustification				GetTextJustification( ) const;
		ELineWeight							GetLineWeight( ) const;
		uWORD									GetPathToWarpId( ) const;
		uWORD									GetPathToFollowId( ) const;
		EVerticalPlacement				GetVerticalPlacement( ) const;
		void									GetLineScales( uWORD& line1, uWORD& line2, uWORD& line3 ) const;
		BOOLEAN								IsLineLeading( ) const;
		BOOLEAN								IsCharKerning( ) const;
		BOOLEAN								IsDistortMode( ) const;
		BOOLEAN								IsHorizontal( ) const;
		BOOLEAN								IsVertical( ) const;
		BOOLEAN								IsConstructed( ) const;
		BOOLEAN								NotDisplayEnvelop( ) const;
		static BOOLEAN						IsHorizontalWarpId( uWORD id );
		static BOOLEAN						IsVerticalWarpId( uWORD id );
		EOutlineEffects					GetOutlineEffect( ) const;
		EDistortEffects					GetDistortEffect( ) const;
		EProjectionEffects				GetProjectionEffect( ) const;
		EShadowEffects						GetShadowEffect( ) const;
		EFramedEffects						GetFrameEffect( ) const;
		RSolidColor							GetOutlineStrokeColor( ) const;
		void									GetOutlineFill( RHeadlineColor& fill ) const;
		RSolidColor							GetOutlineHiliteColor( ) const;
		RSolidColor							GetOutlineShadowColor( ) const;
		void									GetProjectionFill( RHeadlineColor& fill ) const;
		void									GetShadowFill( RHeadlineColor& fill ) const;
		void									GetFrameFill( RHeadlineColor& fill ) const;
		RSolidColor							GetFrameHiliteColor( ) const;
		RSolidColor							GetFrameShadowColor( ) const;
		RSolidColor							GetBlur2BackgroundColor( ) const;
		RSolidColor							GetShadow2BackgroundColor( ) const;
		YAngle								GetShearAngle( ) const;
		YAngle								GetShearRotation( ) const;
		YPercentage							GetBaselineAlternate( ) const;
		YCounter								GetNumStages( ) const;
		YPercentage							GetProjectionDepth( ) const;
		RIntPoint							GetVanishingPoint( ) const;
		YPercentage							GetShadowDepth( ) const;
		RIntPoint							GetShadowingPoint( ) const;
		RIntPoint							GetLightSourcePoint( ) const;
		YIntDimension						GetWindingDirection( ) const;
		YPercentage							GetMaxPathHeightScale( ) const;
		uWORD									GetNumLines( ) const;

		BOOLEAN								HasSoftEffects() const;

	// Configure Operations
	public :
		// text
		BOOLEAN								SetText( const RMBCString string );
		void									SetFont( RFont* pFont);

		// shape
	public :
		BOOLEAN								ApplyDistort( BOOLEAN fEnable );
		BOOLEAN								ApplyNonDistort( const RIntSize& size );
		BOOLEAN								ApplyPathToWarp( sWORD warpId );
		BOOLEAN								ApplyPathToFollow( sWORD warpId, YPercentage maxScale = 0.9 );
		void									NotDisplayEmptyEnvelop( BOOLEAN fEnable );
	private :
		BOOLEAN								CreatePathToWarp( const RIntPoint* pVertices, YPointCount nrVertices, const RIntPoint* pVertices2, YPointCount nrVertices2 );
		BOOLEAN								CreatePathToWarp( EPathOperator* pOperators, const RIntPoint* pVertices, EPathOperator* pOperators2, const RIntPoint* pVertices2 );
		BOOLEAN								CreatePathToFollow( const RIntPoint* pVertices, YPointCount nrVertices, BOOLEAN fDefineEnvelop = TRUE );

		// wacky effects
	public :
		void									ApplyShearEffect( YAngle shear );
		void									ApplyAltShearEffect( YAngle shear );
		void									ApplyAltShearEffect( YAngle shear, YAngle rotate );
		void									ApplyAlternateBaselineEffect( YPercentage scale );
		
		// color/fill stuff
		void									SetOutlineStroke( );
		void									SetOutlineStroke( const RSolidColor& color );
		void									SetOutlineFill( EOutlineEffects effect, const RHeadlineColor& fill );
		void									SetOutlineHilite( const RSolidColor& color );
		void									SetOutlineShadow( const RSolidColor& color );
		void									SetProjectionFill( EProjectionEffects effect, const RHeadlineColor& fill );
		void									SetShadowFill( EShadowEffects effect, const RHeadlineColor& fill );
		void									SetFrameFill( EFramedEffects effect, const RHeadlineColor& fill );
		void									SetFrameHilite( EFramedEffects effect, const RSolidColor& color );
		void									SetFrameShadow( EFramedEffects effect, const RSolidColor& color );
		void									SetBlur2Background( const RSolidColor& color );
		void									SetShadow2Background( const RSolidColor& color );

		// character/line spacing
		void									EnableLineLeading( BOOLEAN fEnable );
		void									EnableCharKerning( BOOLEAN fEnable );
		void									ApplyCharCompensation( ETextCompensation type );
		void									AdjustCharCompensation( YPercentage scale );
		void									ApplyTextEscapement( ETextEscapement type );
		void									ApplyTextJustification( ETextJustification type );
		void									ApplyVerticalPlacement( EVerticalPlacement type );
		void									AdjustLineScale( uWORD line1, uWORD line2, uWORD line3 );
		void									ApplyLineWeight( ELineWeight type );

		// character rendering
		void									ApplyOutlineEffect( EOutlineEffects type );
		void									ApplyProjectionEffect( EProjectionEffects type,
															 				  YCounter nrStages,
																			  YPercentage depth,
																			  const RIntPoint& vanishing );
		void									ApplyShadowEffect( EShadowEffects type,
															 			 YPercentage depth,
																		 const RIntPoint& shadowing );
		void									ApplyFrameEffect( EFramedEffects type );
		void									SetWindingDirection( YIntDimension direction );

		// background
		BOOLEAN								ApplyFramePath( const RIntPoint* pVertices, YPointCount nrVertices );
		BOOLEAN								ApplyFramePath( EPathOperator* pOperators, const RIntPoint* pVertices );
		BOOLEAN								ApplyFrameRect( const RIntRect& rect );

		// Trim white space
		void									SetTrimWhiteSpace( BOOLEAN fTrimWhiteSpace );

	// Operations
	public :
		RRealSize							CalcNominalBoundingSize( BOOLEAN fDistort = FALSE );
		void									Construct( const RRealSize& size );
		void									Construct( const RRealSize& size, const R2dTransform& toLogicalUnits, const R2dTransform& toDeviceUnits );
		void									Construct( const RRealSize& size, const RView& view );
		void									Construct( const RRealSize& size, const RDrawingSurface& drawingSurface );
		void									Construct( const RView& view );
		void									Construct( const RDrawingSurface& drawingSurface );
		virtual void						Render( RDrawingSurface& drawingSurface,
														  const RRealSize& size,
														  const R2dTransform& transform,
														  const RIntRect& rRender,
														  const RColor& monochromeColor = RSolidColor( kBlack ),
														  BOOLEAN fImageAsMask = FALSE );
		virtual void						RenderBehindSoftEffect( RDrawingSurface& drawingSurface,
														  const RRealSize& size,
														  const R2dTransform& transform,
														  const RIntRect& rRender );
		void									RenderFeedback( RDrawingSurface& drawingSurface,
															  	  	 const RRealSize& size,
																	 const R2dTransform& transform );
		void									Redefine( );

	// Implementation
	protected :
		void									ApplyTransform( const R2dTransform& transform );

	// Rendering
	private :
		void									RenderOutline( RDrawingSurface& drawingSurface,
														  			const RRealSize& size,
														  			const R2dTransform& transform,
																	const RIntRect& rRender,
														  			const RColor& monochromeColor,
														  			BOOLEAN fImageAsMask,
														  			EOutlineEffects effect,
										 		  					YIntDimension weight, 
														  			const RSolidColor& strokeColor,
										 		  					RHeadlineColor& fillColor, 
										 		  					const RSolidColor& backColor, 
														  			R2dTransform preXform );
		void									RenderProjection( RDrawingSurface& drawingSurface,
															  			const RRealSize& size,
															  			const R2dTransform& transform,
																		const RIntRect& rRender,
															  			const RColor& monochromeColor,
															  			BOOLEAN fImageAsMask );
		void									RenderShadow( RDrawingSurface& drawingSurface,
															  	  const RRealSize& size,
															  	  const R2dTransform& transform,
																  const RIntRect& rRender,
															  	  const RColor& monochromeColor,
															  	  BOOLEAN fImageAsMask );
		void									RenderFrame( RDrawingSurface& drawingSurface,
															  	 const RRealSize& size,
															  	 const RRealSize& one,
															  	 const R2dTransform& transform,
																 const RIntRect& rRender );
		void									RenderSilhouette( RDrawingSurface& drawingSurface,
															  	 		const RRealSize& size,
															  	 		const R2dTransform& transform,
																 		const RIntRect& rRender );
		YPenWidth  							CalcTransformedPenWidth( YIntDimension logicalWidth, const R2dTransform& transform ) const;
		void									ConfigureDrawingCharacteristics( const R2dTransform& transform, 
																							YFillMethod method, 
																							YIntDimension logicalWidth, 
																							const RSolidColor& stroke, 
																							RHeadlineColor& fill );
		void									DefaultDrawingCharacteristics( RDrawingSurface& drawingSurface );

	// Construction
	private :
		BOOLEAN						  		CalcBaseGraphicScale( const RRealSize& size );
		void									ParseTextLines( const RMBCString string );
		void									PrepGraphicForConstruct( );
		void									AdjustBaseGraphicAfterConstruct( RIntSize whiteSpace );
		void									AdjustBaseGraphicForWarp( BOOLEAN fHorizontal, RIntSize whiteSpace, RIntSize margin );
		void									TrimBaseGraphicWhiteSpace( RIntSize whiteSpace );
		void									ConstructBaseGraphic( BOOLEAN fHorizontal );
		void									ConstructBaseGraphicViaPath( );
		void									WarpBaseGraphic( RIntSize margin );
		void									CalcProjectionXforms( );
		void									BuildProjectionExtrusion( );
		BOOLEAN								CalcLineScaleXforms( BOOLEAN fHorizontal );
		BOOLEAN								CalcLineScaleXformsViaPath( BOOLEAN fHorizontal );
		void									CalcHorzLineStartPoints( );
		void									CalcVertLineStartPoints( );
		YRealDimension						GetEmHeight() const;
		RPath*								GetScaledGlyph( const RCharacter& character, const R2dTransform& transform, BOOLEAN fReally ) const;
		R2dTransform						CalcCharScaleXform( const RCharacter& character, R2dTransform xform, YAngle angle );
		R2dTransform						CalcFollowPathScaleXform( const RIntRect& rcIn, const RIntRect& rcOut ) const;
		R2dTransform						CalcWarpPathScaleXform( BOOLEAN fHorizontal, EPathTypes pathType, const RRealRect& rcIn1, const RRealRect& rcIn2, const RRealSize& out, BOOLEAN f1Curves, BOOLEAN f2Curves ) const;
		YIntDimension						CalcBaselineOffset( BOOLEAN inc );
		RIntSize								CalcTopLeftMarginDisplacement( YAngle angle );
		RIntSize								AdjustWhiteSpaceBasedOnEffects( const RIntSize& whiteSpace ) const;
		RIntRect								CalcBoundingBoxWithEffects( const R2dTransform& xform ) const;
		RRealRect							CalcMinRectInRect( RRealPoint pt1, RRealPoint pt2, RRealRect rect, const RRealSize& size ) const;
		YCounter								CalcNrGradientExtrusionBands( const R2dTransform& transform, BOOLEAN fPerspective ) const;
		YScaleFactor						CalcEmDebossOffsetScale( const R2dTransform& transform, const RRealSize& size, YAngle& rotation ) const;
		void									CalcFollowPathEscapementScaleFactors( YRealDimension& tighten, YRealDimension& loosen ) const;
		RIntSize								CalcMarginBasedOnEffects( ) const;
		RIntSize								CalcMarginBasedOnEffects( EOutlineEffects effect, ELineWeight line ) const;
		YIntDimension						CalcMaxPenWidthBasedOnOutlineEffect( EOutlineEffects effect, ELineWeight line ) const;
		YPenWidth							CalcLogicalPenPoints( ELineWeight line ) const;
		YPenWidth							CalcLogicalPenWidth( EOutlineEffects effect, ELineWeight line ) const;
		YPenWidth							CalcLogicalPenWidth( EOutlineEffects effect, YIntDimension width ) const;
		void									CalcCharDisplacement( const RCharacter& chCur, 
																			 const RCharacter& chNxt, 
																			 const R2dTransform& xformEffect, 
																			 const R2dTransform& xformScaledEffect, 
																			 BOOLEAN fHorizontal, 
																			 RRealSize& preAdjust, 
																			 RRealSize& postAdjust );
		void									CalcCharStartPoints( const RMBCString& string, 
																			const RIntRect& rcBaseString, 
																			const R2dTransform& xformScale, 
																			const RIntPoint& ptStart, 
																			BOOLEAN fHorizontal );
		void									CalcCharStartPointsViaPath( const RMBCString& string, 
																					 const RIntRect& rcBaseString, 
																					 const R2dTransform& xformScale, 
																					 const RIntPoint& ptStart, 
																					 BOOLEAN fHorizontal, 
																					 const RPath& path, 
																					 YScaleFactor scale );
		void									CalcDefaultMarkerPoints( RIntPoint* points ); 
		void									ReplaceMarkerPointData( YCounter nr, const RRealRect& rect ); 

	// Helper
	private :
		BOOLEAN								IsMonochrome( ) const;
		BOOLEAN								UsesBeziers( ) const;
		RIntRect								GetStringBounds( const RMBCString& string ) const;

	// Member data
	private :
		BOOLEAN								m_TextDefined;
		BOOLEAN								m_GraphicDefined;
		BOOLEAN								m_TrimWhiteSpace;
		BOOLEAN								m_DistortGraphic;
		RFont*								m_pFont;										// font face/family/style, size is irrelivant
		YRealDimension						m_FontPointSize;
		RMBCString							m_OriginalText;
		RMBCStringArray					m_LineTextList;
		uWORD									m_NrLines;
		RIntSize								m_BoundingSize;							// in headline units
		RIntRect								m_OutlineRect;								// in headline units
		RRealSize							m_ConstructedSize;						// in headline units
		YScaleFactor						m_EmScale;

		uWORD									m_LineScaleProportions[ 4 ];
		R2dTransformArray					m_LineScaleTransforms;
		RIntPoint							m_LineStartPoints[ kMaxHeadlineLines ];
		RRealRect							m_BoundingBox;								// in headline units
		RIntRectArray						m_BaseStringBoundingBoxes;				// in font units/unscaled
		RIntSize								m_WhiteSpace;								// local/static/temp
		R2dTransform						m_EmSquareTransform;
		EScaleConstraint					m_ScaleConstraint;
		EFlipDirection						m_FlipDirection;

		RIntRect								m_PathRect;
		RPath*								m_pPathTop;
		RPath*								m_pPathBottom;
		RPath*								m_pPathEnvelope;
		uWORD									m_WarpShapeOrPathId;
		BOOLEAN								m_NotDisplayEmptyEnvelop;
		YRealDimension						m_PathRadius;
		EPathTypes							m_PathType;
		EVerticalPlacement				m_Verticality;
		YAngle								m_PathAngle;								// ¡ 
		YPercentage							m_MaxPathHeightScale;					// follow path

		EDistortEffects					m_DistortEffect;
		EOutlineEffects					m_OutlineEffect;
		EProjectionEffects				m_ProjectionEffect;
		EShadowEffects						m_ShadowEffect;
		EFramedEffects						m_FrameEffect;

		R2dTransformArray					m_ProjectionTransforms;					// list of transforms to convert from base graphic outline to each projection and shadow stage
		YCounter								m_NrProjectionStages;					// number of projection stages
		YPercentage							m_ProjectionDepth;						// % deep into vanishing point vector projection goes
		YAngle								m_ProjectionAngle;						// ¡ from 0 of vanishing point vector
		RIntPoint							m_VanishingPoint;							// vector of projection based on origin
		YRealDimension						m_VanishingLength;						// length of vanishing point vector
		YIntDimension						m_SpinDirection;							// winding of spin stack angle
		YPercentage							m_ShadowDepth;								// % deep into shadow point vector projection goes
		YAngle								m_ShadowAngle;								// ¡ from 0 of shadow point vector
		YAngle								m_ShadowSlope;								// slope of above angle
		RIntPoint							m_ShadowingPoint;							// vector of shadow based on origin
		YRealDimension						m_ShadowingLength;						// length of shadow point vector
		RIntPoint							m_LightSourcePoint;
		RSingleGraphic*					m_pExtrusionGraphic;						// graphic where extrusion is created/stored
		R2dTransformArray					m_ExtrusionTransforms;					
		YRealDimension						m_MinGradBandWidth;

		RIntRect								m_FrameRect;
		RPath*								m_pFrameEnvelope;

		RHeadlineColor						m_OutlineFillDesc;
		RSolidColor							m_OutlineStrokeColor;
		RSolidColor							m_OutlineHiliteColor;
		RSolidColor							m_OutlineShadowColor;
		RHeadlineColor						m_ProjectionFillDesc;
		RHeadlineColor						m_ShadowFillDesc;
		RHeadlineColor						m_FrameFillDesc;
		RSolidColor							m_BackgroundBlur2Color;
		RSolidColor							m_BackgroundShadow2Color;
		YTint									m_Tint;										// local/static/temp

		ETextEscapement					m_Escape;
		ETextJustification				m_Justification;
		BOOLEAN								m_fIsHorizontal;
		BOOLEAN								m_fUseLeading;
		BOOLEAN								m_fUseKerning;								// kerning and compensation are not mutually exclusive
		ETextCompensation					m_Compensate;								// kerning and compensation are not mutually exclusive
		uWORD									m_Compensation;							// in font units
		YPercentage							m_CompensationDelta;
		ELineWeight							m_LineWeight;

		YAngle								m_ShearAngle;								// ¡
		YAngle								m_ShearSlope;								// slope of above angle
		YAngle								m_ShearRotaion;							// ¡
		YPercentage							m_BaselineAlternate;						// %
		sWORD									m_ShearDirection;							// local/static/temp
		sWORD									m_BaselineDirection;						// local/static/temp
		RIntPointArray						m_TempPoints;								// local/static/temp
		YAngleArray							m_TempAngles;								// local/static/temp

#ifdef	TPSDEBUG
	// Debugging support  
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::IsHorizontal( )
//
// Description:		
//
// Returns:				Boolean
//
// Exceptions:			None
//
// ****************************************************************************
//
inline BOOLEAN RHeadlineGraphic::IsHorizontal( ) const
{
	return( m_Escape == kLeft2RightTop2Bottom || m_Escape == kLeft2RightBottom2Top || m_Escape == kRight2LeftTop2Bottom || m_Escape == kRight2LeftBottom2Top );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::IsVertical( )
//
// Description:		
//
// Returns:				Boolean
//
// Exceptions:			None
//
// ****************************************************************************
//
inline BOOLEAN RHeadlineGraphic::IsVertical( ) const
{
	return( m_Escape == kTop2BottomLeft2Right || m_Escape == kTop2BottomRight2Left || m_Escape == kBottom2TopLeft2Right || m_Escape == kBottom2TopRight2Left );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::IsLineLeading( )
//
// Description:		Return value of m_fUseLeading
//
// Returns:				m_fUseLeading
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
inline BOOLEAN RHeadlineGraphic::IsLineLeading( ) const
{
	return m_fUseLeading;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::IsCharKerning( )
//
// Description:		Return value of m_fUseKerning
//
// Returns:				m_fUseKerning
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
inline BOOLEAN RHeadlineGraphic::IsCharKerning( ) const
{
	return m_fUseKerning;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::IsDistortMode( )
//
// Description:		Return value of m_DistortGraphic
//
// Returns:				m_DistortGraphic
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
inline BOOLEAN RHeadlineGraphic::IsDistortMode( ) const
{
	return m_DistortGraphic;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::IsConstructed( )
//
// Description:		Return value of m_GraphicDefined
//
// Returns:				m_GraphicDefined
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
inline BOOLEAN RHeadlineGraphic::IsConstructed( ) const
{
	return m_GraphicDefined;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::NotDisplayEnvelop( )
//
// Description:		Return value of m_NotDisplayEmptyEnvelop
//
// Returns:				m_NotDisplayEmptyEnvelop
//
// Exceptions:			Nothing
//
// ****************************************************************************
//
inline BOOLEAN RHeadlineGraphic::NotDisplayEnvelop( ) const
{
	return m_NotDisplayEmptyEnvelop;
}

#endif		// _HEADLINEGRAPHIC_H_
