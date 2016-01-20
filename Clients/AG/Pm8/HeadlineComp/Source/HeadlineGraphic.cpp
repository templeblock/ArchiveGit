// ****************************************************************************
//
//  File Name:			HeadlineGraphic.cpp
//
//  Project:			Renaissance Headline Component
//
//  Author:				R. Hood
//
//  Description:		Definition of the RHeadlineGraphic class
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
//  $Logfile:: /PM8/HeadlineComp/Source/HeadlineGraphic.cpp                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "HeadlineIncludes.h"

ASSERTNAME

#ifndef	IMAGER
	#include "ApplicationGlobals.h"
#else
	#include "GlobalBuffer.h"

	inline YPercentage 	GetGradientDisplayPrecision()	{ return( 0.98 ); }
	inline YPercentage 	GetGradientPrintPrecision() 	{ return( 1.0 ); }
	inline YIntDimension	GetMaxGradientDPI() 				{ return( 72 ); }

#endif	//	IMAGER

#include "CharacterInfo.h"
#include "HeadlineGraphic.h"
#include "HeadlineAlgorithms.h"
#include "HeadlinesCanned.h"
#include "DrawingSurface.h"
#include "ChunkyStorage.h"
#include	"QuadraticSpline.h"
#include "DefaultDrawingSurface.h"
#include "OffscreenDrawingSurface.h"
#include "BitmapImage.h"
#include "ScratchBitmapImage.h"
#include "ImageLibrary.h"

#include	"Bezier.h"

#ifndef	IMAGER
	#include "View.h"
#endif

#ifdef	IMAGER
	extern  RFontList            		gFontList;
	extern  BOOLEAN				  		gfTrueTypeAvailable;
	extern  BOOLEAN				  		gfATMAvailable;
	inline RFontList 		GetAppFontList()				{ return gFontList; }
	inline BOOLEAN 		IsTrueTypeAvailable()		{ return gfTrueTypeAvailable; }
	inline BOOLEAN 		IsATMAvailable()  	  		{ return gfATMAvailable; }
#endif	//	IMAGER

// constances
const	YCounter			kNrPathMarkerPoints				= 15;
const	YCounter			kNrExtremeMarkerPoints			= 10;
const	RCharacter		kLineFeed( 0x0A );					// REVEIW RAH needs to be internationalized
const	RCharacter		kCarrageReturn( 0x0D );
const	RCharacter		kSpace( 0x20 );

const	YIntDimension	kDefaultMargin						= ::PrinterPointToLogicalUnit( 2 );	
const YRealDimension	kDefaultLeading					= ::InchesToLogicalUnits( 0.063 );

const YPercentage		kDefaultAltBaselineScale		= 0.25;
const YAngle			kDefaultShearAngle				= 45.0;		//	Tan( 45 ) = 1.0
const YAngle			kDefaultAltShearAngle			= 30.0;		//	Tan( 30 ) = 0.577
const YAngle			kDefaultAltShearRotate			= 30.0;
const	YPercentage		kDefaultProjectionDepth			= 0.5;
const YPercentage		kDefaultShadowDepth				= 0.9;
const	YCounter			kDefaultProjectionStages		= 6;
const	RIntPoint		kDefaultVanishingPoint( 500, -500 );
const RIntPoint		kDefaultShadowingPoint( 500, -500 );

const YRealDimension kPctBevelOffset  = 0.013;
const YRealDimension kPctEmbossOffset = 0.024;

const YRealDimension kOutlineTransforms[5][2] =
{    
	//   Scalar			Offset
	{ 0.0013,			0.2286 }, // kHairLine
	{ 0.0017,			0.4575 }, // kThinLine
	{ 0.0061,			0.9725 }, // kMediumLine
	{ 0.0108,			1.7294 }, // kThickLine
	{ 0.0216,			2.4589 }  // kExtraThickLine
};

#ifdef	IMAGER
	#ifdef	TPSDEBUG
		#define  RAH_TEST_CODE		 1
	#endif	
#endif	


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::RHeadlineGraphic()
//
// Description:		Constructor
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RHeadlineGraphic::RHeadlineGraphic( ) : m_TextDefined( FALSE ),
													 m_GraphicDefined( FALSE ),
													 m_TrimWhiteSpace( FALSE ),
													 m_DistortGraphic( TRUE ),
	  												 m_pFont( NULL ),
	  												 m_NrLines( 0 ),
	  												 m_FontPointSize( 0.0 ),
	  												 m_Justification( kCenterJust ),
	  												 m_Verticality( kPin2Middle ),
	  												 m_MaxPathHeightScale( 0.9 ),
													 m_BoundingBox( 0.0, 0.0, 0.0, 0.0 ),
													 m_OutlineRect( 0, 0, 0, 0 ),
													 m_FrameRect( 0, 0, 0, 0 ),
	  												 m_WarpShapeOrPathId( kNoWarpShape ),
	  												 m_pPathTop( NULL ),
	  												 m_pPathBottom( NULL ),
	  												 m_pPathEnvelope( NULL ),
													 m_pFrameEnvelope( NULL ),
													 m_pExtrusionGraphic( NULL ),
													 m_DistortEffect( kNonDistort ),
	  												 m_OutlineStrokeColor( kRed ),
	  												 m_OutlineHiliteColor( kWhite ),
	  												 m_OutlineShadowColor( kBlack ),
	  												 m_BackgroundBlur2Color( kWhite ),
	  												 m_BackgroundShadow2Color( kWhite ),
													 m_fUseKerning( FALSE ),
													 m_fUseLeading( FALSE ),
													 m_Escape( kLeft2RightTop2Bottom ),
													 m_fIsHorizontal( TRUE ),
													 m_NotDisplayEmptyEnvelop( FALSE ),
													 m_Compensate( kRegular ),
													 m_Compensation( 0 ),
													 m_CompensationDelta( 1.0 ),
													 m_LineWeight( kThinLine ),
													 m_MinGradBandWidth( 1.0 ),
													 m_FlipDirection( kFlipNone ),
													 m_BaselineAlternate( kDefaultAltBaselineScale ),
													 m_ShearAngle( kDefaultAltShearAngle ),					// 30¡
													 m_ShearRotaion( kDefaultAltShearRotate ),
													 m_SpinDirection( -1 ),
	  												 m_LightSourcePoint( 0, 0 ),
													 m_EmScale( 1.0 )
{
	(void)ApplyNonDistort( GetDefaultHeadlineSize( IsHorizontal() ) );
	m_BoundingSize = GetDefaultHeadlineSize( IsHorizontal() );
	m_DistortGraphic = TRUE;
	// Force ApplyOutlineEffect to change the setting and thereby guarantee
	// proper initialization.
	//
	// * In the future it would be better to have an kInvalidOutline value. -RIP-
	m_OutlineEffect = (EOutlineEffects)-1;
	ApplyOutlineEffect( kSimpleFill );
	// As above, guarantee full initialization of projection effects.
	m_ProjectionEffect = (EProjectionEffects)-1;
	ApplyProjectionEffect( kNoProjection, kDefaultProjectionStages, kDefaultProjectionDepth, kDefaultVanishingPoint );
	// Guarantee initialization of shadow effects.
	m_ShadowEffect = (EShadowEffects)-1;
	ApplyShadowEffect( kNoShadow, kDefaultShadowDepth, kDefaultShadowingPoint );
	// Guarantee frame effect initialization.
	m_FrameEffect = (EFramedEffects)-1;
	ApplyFrameEffect( kFrameNoFrame );
	m_LineScaleProportions[0] = m_LineScaleProportions[1] = m_LineScaleProportions[2] = 1;
	m_LineScaleProportions[4] = 0;
	m_fMonochrome = FALSE;
	m_FrameFillDesc		= RColor();		// Transparent
	m_ShadowFillDesc		= RSolidColor( kBlack );
	m_ProjectionFillDesc	= RSolidColor( kBlack );
	m_OutlineFillDesc		= RSolidColor( kBlack );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineGraphic::RHeadlineGraphic( )
//
//  Description:		Copy constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RHeadlineGraphic::RHeadlineGraphic( const RHeadlineGraphic &rhs )
{
	m_TextDefined = rhs.m_TextDefined;
	m_GraphicDefined = rhs.m_GraphicDefined;


	UnimplementedCode();


}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::~RHeadlineGraphic( )
//
// Description:		Destructor
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RHeadlineGraphic::~RHeadlineGraphic( )
{
	// delete glyphs
	delete m_pPathTop;
	delete m_pPathBottom;
	delete m_pPathEnvelope;
	delete m_pFrameEnvelope;
	for ( RMBCStringArrayIterator c = m_LineTextList.Start(); c != m_LineTextList.End(); ++c )
	{
		RMBCString* pMBString = *c;
		delete pMBString;
	}
	m_LineTextList.Empty();
	delete m_pExtrusionGraphic;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineGraphic::Initialize( )
//
//  Description:		Initialization (think this got superseceeded by construct)
//
//  Returns:			Boolean
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::Initialize( ) 
{
	UnimplementedCode();
	return FALSE;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::Render( )
//
// Description:		Render the headline into the given drawing surface.
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::Render( RDrawingSurface& drawingSurface,
										 const RRealSize& size,
										 const R2dTransform& transform,
										 const RIntRect& rRender,
										 const RColor& monochromeColor,
										 BOOLEAN fImageAsMask )

{
R2dTransform			baseXform;
RPath						outline;
RRealSize				envelopeSize, oneUnit( 1.0, 1.0 );
BOOLEAN 					fUseGradient = FALSE;
RSolidColor				fillColor( kLightGray );
RSolidColor				strokeColor( kBlack );
#ifndef	IMAGER
RWaitCursor				waitCursor;
#endif	

	// validations/initializations
	::DeviceUnitsToLogicalUnits( oneUnit, drawingSurface );
	envelopeSize.m_dx = size.m_dx - oneUnit.m_dx;
	envelopeSize.m_dy = size.m_dy - oneUnit.m_dy;
	m_Tint = drawingSurface.GetTint();

	// RAH test code
#ifdef	TPSDEBUG
	#ifdef	IMAGER
		#define  FRAME_TEST_CODE		 1
	#endif	
#endif	
#ifdef  FRAME_TEST_CODE
	if ( m_GraphicSize.m_dx != 0 && m_GraphicSize.m_dy != 0 )
	{
		outline.Define( RRealRect( m_GraphicSize ) );
		outline.Render( drawingSurface, envelopeSize, transform, rRender, monochromeColor, fImageAsMask );
	}
#endif 

	// render the graphic outline and projection effects
	if ( m_TextDefined && m_GraphicDefined )
	{
		TpsAssert( ( RIntSize( m_ConstructedSize ) == RIntSize( size ) ), "Size at construct not equal to render." );

		if (fImageAsMask)
		{
			RHeadlineColor	oldColor = m_FrameFillDesc;
			m_FrameFillDesc = monochromeColor;
		
			DefaultDrawingCharacteristics( drawingSurface );
			RenderFrame( drawingSurface, size, oneUnit, transform, rRender );
			m_FrameFillDesc = oldColor;

			if (m_FrameEffect != kFrameNoFrame)
				return;
		}

		DefaultDrawingCharacteristics( drawingSurface );
		RenderFrame( drawingSurface, size, oneUnit, transform, rRender );
//		DefaultDrawingCharacteristics( drawingSurface );
//		RenderShadow( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
		DefaultDrawingCharacteristics( drawingSurface );
		RenderProjection( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
		if ( m_ProjectionTransforms.Count() == 0 )
			baseXform.MakeIdentity();
		else
			baseXform = *(m_ProjectionTransforms.Start());
		DefaultDrawingCharacteristics( drawingSurface );
		
		YPenWidth penWidth = CalcLogicalPenWidth( m_OutlineEffect, m_LineWeight ); 
		RenderOutline( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask, 
							m_OutlineEffect, penWidth, 
							m_OutlineStrokeColor, m_OutlineFillDesc, m_BackgroundBlur2Color, baseXform );
		DefaultDrawingCharacteristics( drawingSurface );
	}
	//	no text or graphic so render the current envelope
	else if ( m_pPathEnvelope != NULL )
	{
		if ( !drawingSurface.IsPrinting() && !m_NotDisplayEmptyEnvelop )
		{
			RIntSize		newSize = m_pPathEnvelope->GetPathSize();
			DefaultDrawingCharacteristics( drawingSurface );
			if ( m_DistortEffect == kFollowPath )
			{
				baseXform = CalcFollowPathScaleXform( m_pPathEnvelope->GetBoundingRect(), RIntRect( RIntSize( envelopeSize ) ) );
				outline = RPath( *m_pPathEnvelope, baseXform );
				if ( m_GraphicSize.m_dx >= newSize.m_dx && m_GraphicSize.m_dy >= newSize.m_dy )
					outline.SetPathSize( m_GraphicSize );
			}
			else
				outline = *m_pPathEnvelope;
			outline.Render( drawingSurface, envelopeSize, transform, rRender, kFilled, strokeColor, fillColor );
#ifdef  FRAME_TEST_CODE
			newSize = outline.GetPathSize();
			if ( m_pPathTop != NULL )
			{
				strokeColor = RSolidColor( kRed );
				if ( m_DistortEffect == kFollowPath )
					outline = RPath( *m_pPathTop, baseXform );
				else
					outline = *m_pPathTop;
				outline.SetPathSize( newSize );
				outline.Render( drawingSurface, envelopeSize, transform, rRender, kStroked, strokeColor, fillColor );
			}
			if ( m_pPathBottom != NULL )
			{
				strokeColor = RSolidColor( kBlue );
				if ( m_DistortEffect == kFollowPath )
					outline = RPath( *m_pPathBottom, baseXform );
				else
					outline = *m_pPathBottom;
				outline.SetPathSize( newSize );
				outline.Render( drawingSurface, envelopeSize, transform, rRender, kStroked, strokeColor, fillColor );
			}
#endif 
		}
	}
	//	no text or graphic or envelope so render the current bounding box
	else if ( !drawingSurface.IsPrinting() && !m_NotDisplayEmptyEnvelop )
	{
		DefaultDrawingCharacteristics( drawingSurface );
		outline.Define( RIntRect( m_BoundingSize ) );
		outline.Render( drawingSurface, envelopeSize, transform, rRender, kFilled, strokeColor, fillColor );
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::RenderBehindSoftEffect( )
//
// Description:		Render things that are suppose to appear behind
//                   the shadow.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::RenderBehindSoftEffect( RDrawingSurface& drawingSurface,
														  const RRealSize& size,
														  const R2dTransform& transform,
														  const RIntRect& rRender )
{
	RRealSize oneUnit( 1.0, 1.0 );
	::DeviceUnitsToLogicalUnits( oneUnit, drawingSurface );
	m_Tint = drawingSurface.GetTint();
	
	// render the graphic outline and projection effects
	if ( m_TextDefined && m_GraphicDefined )
	{
		TpsAssert( ( RIntSize( m_ConstructedSize ) == RIntSize( size ) ), "Size at construct not equal to render." );
		DefaultDrawingCharacteristics( drawingSurface );
		RenderFrame( drawingSurface, size, oneUnit, transform, rRender );
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::RenderOutline( )
//
// Description:		Render the outline, based on the effect, of the headline.
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::RenderOutline( RDrawingSurface& drawingSurface,
										 		  const RRealSize& size,
										 		  const R2dTransform& transform,
												  const RIntRect& rRender,
										 		  const RColor& monochromeColor,
										 		  BOOLEAN fImageAsMask,
										 		  EOutlineEffects effect,
										 		  YIntDimension penWeight, 					// logical pen width
										 		  const RSolidColor& strokeColor,
										 		  RHeadlineColor& fillColor,
										 		  const RSolidColor& backColor,
										 		  R2dTransform preXform )

{
R2dTransform				baseXform;
R2dTransform				xform;
int							i;
RHeadlineColor				IrrelevantColor;
YIntDimension 				penHairline = CalcLogicalPenWidth( effect, kHairLine ); 
RRealPointArray			origPoints;
RIntPointArrayIterator	ptz1;
RRealPointArrayIterator	ptz2, ptz3;

	// validations/initializations
	TpsAssertValid( this );
	TpsAssert( m_TextDefined, "Render of undefined headline." );
	TpsAssert( m_GraphicDefined, "Render of undefined headline." );
	if ( m_ProjectionTransforms.Count() == 0 )
		baseXform.MakeIdentity();
	else
		baseXform = *(m_ProjectionTransforms.Start());

	TpsAssert( m_TransformedRealPointsStream.Count()==m_OriginalPointsStream.Count(), "real point array size differs from original point array" );

	// calculate graphic bounding box and size
	RIntRect rcBoundingBox( 0.0, 0.0, 0.0, 0.0 );
	YPointCountArrayIterator	cntz = m_PointCountsStream.Start();
	EPathOperatorArrayIterator	opz = m_PathOperatorsStream.Start();
	RRealPointArrayIterator	   ptz = m_OriginalPointsStream.Start();

	if (effect < kEmbossedOutline || effect > kSoftDebossedOutline)
	{
		while ( cntz != m_PointCountsStream.End() )
		{
			RIntRect rcBounds = RPath::CalcBoundingRect( &(*opz), &(*ptz) );
			opz += RPath::GetOperatorCount( &(*opz) );
			ptz += *cntz;
			++cntz;
			if ( rcBoundingBox.IsEmpty() )
				rcBoundingBox = rcBounds;
			else
				rcBoundingBox.Union( rcBoundingBox, rcBounds );
		}

		fillColor.SetBoundingRect( rcBoundingBox );
	}
	else
	{
		fillColor.SetBoundingRect( RIntRect( RIntSize( GetGraphicSize() ) ) );
	}

	//
	// pre transform graphic
	xform.MakeIdentity();
	if ( preXform != xform )
	{
		origPoints = m_OriginalPointsStream;
		ptz = m_OriginalPointsStream.Start();
m_TransformedIntPointsStream.Empty();
//		ptz1 = m_TransformedIntPointsStream.Start();
		ptz3 = m_TransformedRealPointsStream.Start();
		while ( ptz != m_OriginalPointsStream.End() )
		{
			*ptz *= preXform;
//			*ptz1 = *ptz;
			*ptz3 = *ptz;
			++ptz;
//			++ptz1;
			++ptz3;
		}
		m_LastTransform.MakeIdentity();					// 'cause we messed with the points stream
	}
	// render outline
	switch ( effect )
	{
		case kSimpleStroke :
			{
			YIntDimension	penWidth = ( (m_LineWeight == kHairLine) ? 1 : penWeight );
				ConfigureDrawingCharacteristics( baseXform, kStroked, penWidth, strokeColor, IrrelevantColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
			}
			break;
		case kSimpleFill :
			if ( !fillColor.IsTransparent() )
			{
				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, strokeColor, fillColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
			}
			break;
		case kSimpleStrokeFill :
			{
				if ( m_LineWeight == kHairLine ) 
				{
					ConfigureDrawingCharacteristics( baseXform, kFilledAndStroked, 1, strokeColor, fillColor );
					RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				}
				else
				{
					ConfigureDrawingCharacteristics( baseXform, kStroked, penWeight, strokeColor, fillColor );
					RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
					ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, strokeColor, fillColor );
					RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				}
			}
			break;
		case kSimpleFillStroke :									// used internally
			{
				ConfigureDrawingCharacteristics( baseXform, kFilledAndStroked, penWeight, strokeColor, fillColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
			}
			break;
		case kHeavyOutline :
			{
				ConfigureDrawingCharacteristics( baseXform, kStroked, ( penWeight + penWeight ), strokeColor, IrrelevantColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, strokeColor, fillColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
			}
			break;
		case kDoubleOutline :
			{
				ConfigureDrawingCharacteristics( baseXform, kStroked, ( penWeight * 2 ), m_OutlineShadowColor, IrrelevantColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				ConfigureDrawingCharacteristics( baseXform, kStroked, penWeight, strokeColor, IrrelevantColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, strokeColor, fillColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
			}
			break;
		case kHairlineDoubleOutline :
			{
				ConfigureDrawingCharacteristics( baseXform, kStroked, ( penWeight + penHairline ), m_OutlineShadowColor, IrrelevantColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				ConfigureDrawingCharacteristics( baseXform, kFilledAndStroked, 1, strokeColor, fillColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
			}
			break;
		case kDoubleHairlineOutline :
			{
				ConfigureDrawingCharacteristics( baseXform, kStroked, ( penWeight + penHairline ), m_OutlineShadowColor, IrrelevantColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				ConfigureDrawingCharacteristics( baseXform, kStroked, penWeight, strokeColor, IrrelevantColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, strokeColor, fillColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
			}
			break;
		case kTripleOutline :
			{
				ConfigureDrawingCharacteristics( baseXform, kStroked, ( penWeight * 3 ), m_OutlineShadowColor, IrrelevantColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				ConfigureDrawingCharacteristics( baseXform, kStroked, ( penWeight * 2 ), m_OutlineHiliteColor, IrrelevantColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				ConfigureDrawingCharacteristics( baseXform, kStroked, penWeight, strokeColor, IrrelevantColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, strokeColor, fillColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
			}
			break;
		case kBlurredOutline :
		case kShadowedOutline :
			{

				ROffscreenDrawingSurface* pDS = dynamic_cast<ROffscreenDrawingSurface*>(&drawingSurface);

				if (pDS && effect == kBlurredOutline)
				{
					// Create the mask bitmap
					//
					RBitmapImage rMaskBitmap;
					rMaskBitmap.Initialize( rRender.Width(), rRender.Height(), 8, 
						kDefaultXDpi, kDefaultYDpi, kUseGrayScalePalette );

					RIntRect rcBounds( rRender );
					rcBounds.Offset( RIntSize( -rRender.m_Left, -rRender.m_Top ) );

					// Render the mask
					//
					const YScaleFactor kLutComponent = 0.6 * 255;

					RSolidColor	crWhite( kWhite );
					RSolidColor crBlack( kBlack );

					xform = transform;
					xform.PostTranslate( -rRender.m_Left, -rRender.m_Top ); 

					ROffscreenDrawingSurface dsMem;
					dsMem.SetImage( &rMaskBitmap );
					dsMem.SetFillColor( crBlack );
					dsMem.FillRectangle( rcBounds );

					// Determine the Em height.
					//
					RIntRect rBounds;
					m_pFont->GetMaxCharBounds( &rBounds );
					rBounds.Offset( RIntSize( -rBounds.m_Left, -rBounds.m_Top ) );		// zero base rect
					if( rBounds.m_Bottom < 0 ) rBounds.m_Bottom = -rBounds.m_Bottom;	// flip it into positive coords

					YAngle rotation;
					YScaleFactor scale = CalcEmDebossOffsetScale( transform, size, rotation );
					YRealDimension	yEmHeight = rBounds.Height() * scale;

					RSolidColor crSolid = monochromeColor.GetSolidColor();
					ConfigureDrawingCharacteristics( baseXform, kFilled, penWeight, crSolid, RHeadlineColor( crWhite ) );
					RSingleGraphic::Render( dsMem, size, xform, rcBounds, monochromeColor, fImageAsMask );
					dsMem.ReleaseImage();

					// Determine the spread
					YRealDimension	spread = yEmHeight * 0.024; 

					// Blur the mask
					//
					RImageLibrary rLibrary;
					rLibrary.Blur( rMaskBitmap, rMaskBitmap, spread, blurInAndOut, boxFilter );

					if (fImageAsMask)
					{
						rMaskBitmap.Render( drawingSurface, rcBounds, rRender );
						break;	// We don't need to go any further.
					}

					// Create a working space for applying the mask
					RBitmapImage rFinalBitmap;
					rFinalBitmap.Initialize( 
						rRender.Width(), 
						rRender.Height(), 24 );

					dsMem.SetImage( &rFinalBitmap );
					dsMem.BlitDrawingSurface( drawingSurface, rRender, rcBounds, kBlitSourceCopy );
					dsMem.ReleaseImage();

					RBitmapImage rTempBitmap;
					rTempBitmap.Initialize( 
						rRender.Width(), 
						rRender.Height(), 24 );

					RColor crFillColor = fillColor.GetColor();
					if (drawingSurface.GetColoringBookMode())
						crFillColor = crBlack;

					crFillColor *= transform;

					dsMem.SetImage( &rTempBitmap );
					dsMem.SetFillColor( crFillColor );
					dsMem.FillRectangle( rcBounds );
					dsMem.ReleaseImage();

					if (RColor::kSolid != crFillColor.GetFillMethod())
						rLibrary.Sharpen( rTempBitmap, -2 );

					rLibrary.AlphaBlend( rTempBitmap, rcBounds, rFinalBitmap, 
						rcBounds, rMaskBitmap, rcBounds );

					rFinalBitmap.Render( drawingSurface, rcBounds, rRender );

					if (drawingSurface.GetColoringBookMode())
					{
						YFillMethod fillMethod = kFilled;

						if (spread < 2)
							fillMethod = kFilledAndStroked;

						drawingSurface.SetColoringBookMode( FALSE );
						ConfigureDrawingCharacteristics( baseXform, fillMethod, penWeight, crBlack, RHeadlineColor( crWhite ) );
						RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, FALSE );
						drawingSurface.SetColoringBookMode( TRUE );
					}
				}
				else
				{
					RIntRect					rcBounds;
					YRealDimension			spread;
					YIntDimension			steps;
					YAngle					baseRotation;
					YScaleFactor			scale;
					YScaleFactor			baseXScale;
					YScaleFactor			baseYScale;
					YRealDimension			rFractionalWidth;
					YPercentage				pctFractionalWidth;
					YFillMethod				fillMethod = ( ( effect == kBlurredOutline ) ? kFilled : kStrokedAndFilled );
					RSolidColor				startColor( fillColor.GetSolidColor() );
					const RSolidColor&	endColor	= backColor;

					TpsAssert( !fillColor.UsesGradient(), "Blurred outlines and soft shadows may not have a gradient fill." );
					m_pFont->GetMaxCharBounds( &rcBounds );
					rcBounds.Offset( RIntSize( -rcBounds.m_Left, -rcBounds.m_Top ) );		// zero base rect
					if ( rcBounds.m_Bottom < 0 )
						rcBounds.m_Bottom = -rcBounds.m_Bottom;									// flip it into positive coords
					spread = CalcLogicalPenWidth( effect, ::PrinterPointToLogicalUnit( ::Round( rcBounds.Height() * 0.024 ) ) );
					spread *= 12.0 / m_FontPointSize;												// based on a 12pt font
					baseXform.Decompose( baseRotation, baseXScale, baseYScale );
					spread = ::Round( spread * baseYScale ) + penHairline;
					if ( effect == kBlurredOutline ) 
						spread += penWeight;
					else
						spread += penHairline;
					steps = ::Max( ::Round( spread / (YRealDimension)penHairline ), 2 );
					rFractionalWidth = ( spread / (YRealDimension)steps );					// spread depends on steps
					pctFractionalWidth = ( 0.8 / (YRealDimension)steps );						// we're going from 0.9 to 0.1
					scale = 0.9;
					if ( effect != kBlurredOutline ) 
						spread += penWeight - penHairline;
					for ( i = 0; i < steps; i++ )
					{
						TpsAssert( ( scale > 0.0 ), "Blur overstepped color ramp." );
						RSolidColor	color( startColor, endColor, scale );
						ConfigureDrawingCharacteristics( baseXform, kStroked, ::Round( spread ), color, IrrelevantColor );
						RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
						spread -= rFractionalWidth;
						scale -= pctFractionalWidth;
					}
					ConfigureDrawingCharacteristics( baseXform, fillMethod, ( penWeight - penHairline ), fillColor.GetSolidColor(), fillColor );
					RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				}

				break;
			}

		case kBeveledOutline:
			{
			YAngle			rotation;
			RRealPoint     ptOffset;
			R2dTransform	rRotateXForm ;

			// Determine the Em height.
			YRealDimension	yEmHeight = (RRealSize( 0, GetEmHeight()) * transform).m_dy; 
			
			CalcEmDebossOffsetScale( transform, size, rotation );
			rRotateXForm.PreRotateAboutOrigin( kPI - rotation );
			
			// Determine the colors to be used.
			//
			RSolidColor rBackgroundColor( m_FrameFillDesc.IsTransparent() ? RSolidColor( kWhite ) : m_FrameFillDesc.GetSolidColor() );
			RSolidColor rBevelFaceColor( m_OutlineFillDesc.GetSolidColor() );

			YRealDimension yBackgroundLightness = (
				rBackgroundColor.GetRed()   +
				rBackgroundColor.GetGreen() +	
				rBackgroundColor.GetBlue()) / 255.0 / 3.0;

			YRealDimension yBevelLightness = (
				rBevelFaceColor.GetRed()   +
				rBevelFaceColor.GetGreen() +	
				rBevelFaceColor.GetBlue()) / 255.0 / 3.0;

			YRealDimension yShadeFraction = (1 - min( yBevelLightness, 
				yBackgroundLightness )) * 0.65 + 0.35  ;

			YRealDimension yTintFraction = max( yBevelLightness, 
				yBackgroundLightness ) * 0.65 + 0.35  ;

			RSolidColor rBevelShadowColor( rBevelFaceColor );
			RSolidColor rBevelHiliteColor( rBevelFaceColor );
			rBevelShadowColor.DarkenBy( yShadeFraction );
			rBevelHiliteColor.LightenBy( yTintFraction );

			YPenWidth penWidth = penWeight; //CalcLogicalPenWidth( effect, 
//				::PrinterPointToLogicalUnit( m_FontPointSize ) * 0.015 );
//				::Round( yEmHeight * 0.015 ) );

			// Render the down left portion
			//
			RSolidColor rColor( rBevelShadowColor, rBevelFaceColor, 0.5 );
			ptOffset.m_x = -(ptOffset.m_y = yEmHeight * kPctBevelOffset);
			ptOffset *= rRotateXForm;

			xform = transform;
			xform.PostTranslate( ptOffset.m_x, ptOffset.m_y );
			ConfigureDrawingCharacteristics( baseXform, kFilledAndStroked, penHairline, rColor, RHeadlineColor(rColor) );
			RSingleGraphic::Render( drawingSurface, size, xform, rRender, monochromeColor, fImageAsMask );

			// Render the up right portion
			//
			rColor = RSolidColor( rBevelHiliteColor, rBevelFaceColor, 0.5 );
			ptOffset.m_x = -(ptOffset.m_y = yEmHeight * -kPctBevelOffset);
			ptOffset *= rRotateXForm;

			xform = transform;
			xform.PostTranslate( ptOffset.m_x, ptOffset.m_y );
			ConfigureDrawingCharacteristics( baseXform, kFilledAndStroked, penWidth, rColor, RHeadlineColor(rColor) );
			RSingleGraphic::Render( drawingSurface, size, xform, rRender, monochromeColor, fImageAsMask );

			// Render the down right portion
			//
			rColor = rBevelShadowColor;
			ptOffset.m_y = ptOffset.m_x = yEmHeight * kPctBevelOffset;
			ptOffset *= rRotateXForm;

			xform = transform;
			xform.PostTranslate( ptOffset.m_x, ptOffset.m_y );
			ConfigureDrawingCharacteristics( baseXform, kFilledAndStroked, penWidth, rColor, RHeadlineColor(rColor) );
			RSingleGraphic::Render( drawingSurface, size, xform, rRender, monochromeColor, fImageAsMask );

			// Render the up left portion
			//
			rColor = rBevelHiliteColor;
			ptOffset.m_x = ptOffset.m_y = yEmHeight * -kPctBevelOffset;
			ptOffset *= rRotateXForm;

			xform = transform;
			xform.PostTranslate( ptOffset.m_x, ptOffset.m_y );
			ConfigureDrawingCharacteristics( baseXform, kFilledAndStroked, penWidth, rColor, RHeadlineColor(rColor) );
			RSingleGraphic::Render( drawingSurface, size, xform, rRender, monochromeColor, fImageAsMask );

			// Render the centered face
			//
			ConfigureDrawingCharacteristics( baseXform, kFilled, penWidth, strokeColor, fillColor );
			RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );

			break;
			}
		case kEmbossedOutline :
		case kDebossedOutline :
		{
			ROffscreenDrawingSurface* pDS = dynamic_cast<ROffscreenDrawingSurface*>(&drawingSurface);

			if (pDS)
			{
				// Determine the Em height.
				//
				YRealDimension	yEmHeight = GetEmHeight(); 
				RRealSize xformedHeight = RRealSize( yEmHeight, yEmHeight ) * transform;
				yEmHeight = xformedHeight.m_dy;

				//
				// Create the mask bitmap
				//

				// Determine necessary margins for rendering soft effects
				//
				RIntSize rInflate = CalcMarginBasedOnEffects();
				rInflate *= transform;

				RIntRect rcRender( rRender );
				rcRender.Inflate( rInflate );
				rcRender.Offset( RIntSize( -rcRender.m_Left, -rcRender.m_Top ) );
				
				RBitmapImage rMaskBitmap;
				rMaskBitmap.Initialize( rcRender.Width(), rcRender.Height(), 8, 
					kDefaultXDpi, kDefaultYDpi, kUseGrayScalePalette );

				// Render the mask
				//
				RIntRect rcBounds( rRender );
				rcBounds.Offset( RIntSize( 
					-rRender.m_Left + rInflate.m_dx, 
					-rRender.m_Top  + rInflate.m_dy ) );

				const YScaleFactor kLutComponent = 0.6 * 255;

				RSolidColor crLut( RGB( kLutComponent, kLutComponent, kLutComponent ) );
				RSolidColor	crWhite( kWhite );
				RSolidColor crBlack( kBlack );

				xform = transform;
				xform.PostTranslate( 
					-rRender.m_Left + rInflate.m_dx, 
					-rRender.m_Top  + rInflate.m_dy ); 

				ROffscreenDrawingSurface dsMem;
				dsMem.SetImage( &rMaskBitmap );
				dsMem.SetFillColor( crBlack );
				dsMem.FillRectangle( rcRender );

				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, crLut, RHeadlineColor( crLut ) );
				RSingleGraphic::Render( dsMem, size, xform, rcRender, monochromeColor, FALSE );
				dsMem.ReleaseImage();

				// Blur the mask
				//

				// Determine the spread
				YRealDimension	spread = yEmHeight * 0.012;
				
				// Blur the image
				RImageLibrary rLibrary;
				RBitmapImage rTempBitmap( rMaskBitmap );
				rLibrary.Blur( rMaskBitmap, rMaskBitmap, spread, blurOut, boxFilter );

				// Create a working space for applying the mask
				//
				RBitmapImage rFinalBitmap;
				rFinalBitmap.Initialize( 
					rcRender.Width(), 
					rcRender.Height(), 24 );

				dsMem.SetImage( &rFinalBitmap );
				dsMem.BlitDrawingSurface( drawingSurface, rRender, rcBounds, kBlitSourceCopy );
				dsMem.ReleaseImage();

				RSolidColor crColorArray[] = {
					RSolidColor( RGB( 255, 255, 255 ) ),
					RSolidColor( RGB(   0,   0,   0 ) ) };

				if (kDebossedOutline == effect)
				{
					crColorArray[0] = RGB(   0,   0,   0 );
					crColorArray[1] = RGB( 255, 255, 255 );
				}

				// Stamp the top left
				//
				RIntSize rOffset; 
				rOffset.m_dx = max( 1, ::Round( yEmHeight * kPctEmbossOffset ) );
				rOffset.m_dy = max( 1, ::Round( yEmHeight * kPctEmbossOffset ) );

				RIntRect rcSrcRect( rcRender /*rcBounds*/ );
				rcSrcRect.Inset( rOffset );
				
				RIntRect rcDestRect( rcSrcRect );
				rcDestRect.Offset( RIntSize( -rcSrcRect.m_Left, -rcSrcRect.m_Top ) );

				rLibrary.AlphaBlend( crColorArray[0], rFinalBitmap, 
					rcDestRect, rMaskBitmap, rcSrcRect );


				// Stamp the bottom right
				//
				rcDestRect = rcSrcRect;
				rcDestRect.Offset( rOffset );

				rLibrary.AlphaBlend( crColorArray[1], rFinalBitmap, 
					rcDestRect, rMaskBitmap, rcSrcRect );

				// Invert the original mask and use it to render to the drawing surface.
				dsMem.SetImage( &rMaskBitmap );
				dsMem.SetFillColor( crWhite );
				dsMem.FillRectangle( rcBounds );

				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, crBlack, RHeadlineColor( crBlack ) );
				RSingleGraphic::Render( dsMem, size, xform, rcBounds, monochromeColor, TRUE );

				ROffscreenDrawingSurface dsMem2;
				dsMem2.SetImage( &rFinalBitmap );

				// 1) Cut a hole in the destination drawing surface
				drawingSurface.BlitDrawingSurface(dsMem, rcBounds, rRender, kBlitNotSourceAnd);
				
				// 2) Isolate this image using the mask
				dsMem2.BlitDrawingSurface(dsMem, rcBounds, rcBounds, kBlitSourceAnd);
						
				// 3) Copy the isolated image to the output of this drawing surface
				drawingSurface.BlitDrawingSurface(dsMem2, rcBounds, rRender, kBlitSourcePaint);		

				dsMem.ReleaseImage();
				dsMem2.ReleaseImage();
			
			} // pDS
			else
			{
				// Not an offscreen drawing surface.  What to do?
				TpsAssertAlways( "Invalid DrawingSurface" );

				RIntRect			rcBounds;
				YScaleFactor	scale;
				YAngle			rotation;
				YRealDimension	yOffset;
				RRealPoint		pt;
				RSolidColor		shadow( ( effect == kEmbossedOutline ) ? m_OutlineShadowColor : m_OutlineHiliteColor );
				RSolidColor		hilite( ( effect == kEmbossedOutline ) ? m_OutlineHiliteColor : m_OutlineShadowColor );
				m_pFont->GetMaxCharBounds( &rcBounds );
				rcBounds.Offset( RIntSize(-rcBounds.m_Left,-rcBounds.m_Top) );		// zero base rect
				if( rcBounds.m_Bottom < 0 )
					rcBounds.m_Bottom = -rcBounds.m_Bottom;								// flip it into positive coords
				scale = CalcEmDebossOffsetScale( transform, size, rotation );
				yOffset = (YRealDimension)rcBounds.Height() * scale * 0.018;
				pt.m_x = pt.m_y = yOffset;
				xform.MakeIdentity();
				xform.PreRotateAboutOrigin( kPI - rotation );
				pt *= xform;
				xform = transform;
				xform.PostTranslate( pt.m_x, pt.m_y );
				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, shadow, RHeadlineColor(shadow) );
				RSingleGraphic::Render( drawingSurface, size, xform, rRender, monochromeColor, fImageAsMask );
				pt.m_x = pt.m_y = -yOffset;
				xform.MakeIdentity();
				xform.PreRotateAboutOrigin( kPI - rotation );
				pt *= xform;
				xform = transform;
				xform.PostTranslate( pt.m_x, pt.m_y );
				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, hilite, RHeadlineColor(hilite) );
				RSingleGraphic::Render( drawingSurface, size, xform, rRender, monochromeColor, fImageAsMask );
				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, strokeColor, fillColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
			}

		}
		break;

		case kSoftEmbossedOutline :				// REVEIW RAH tbd
		case kSoftDebossedOutline :
			{
			RIntRect			rcBounds;
			YScaleFactor	scale;
			YAngle			rotation;
			YRealDimension	xOffset;
			YRealDimension	yOffset;
			RRealPoint		pt;
			RSolidColor		shadow( ( effect == kSoftEmbossedOutline ) ? m_OutlineShadowColor : m_OutlineHiliteColor );
			RSolidColor		hilite( ( effect == kSoftEmbossedOutline ) ? m_OutlineHiliteColor : m_OutlineShadowColor );
				m_pFont->GetMaxCharBounds( &rcBounds );
				rcBounds.Offset( RIntSize( -rcBounds.m_Left, -rcBounds.m_Top ) );		// zero base rect
				if ( rcBounds.m_Bottom < 0 )
					rcBounds.m_Bottom = -rcBounds.m_Bottom;									// flip it into positive coords
				scale = CalcEmDebossOffsetScale( transform, size, rotation );
				xOffset = (YRealDimension)rcBounds.Height() * scale * 0.028;
				yOffset = (YRealDimension)rcBounds.Height() * scale * 0.034;
				pt.m_x = xOffset;
				pt.m_y = yOffset;
				xform.MakeIdentity();
				xform.PreRotateAboutOrigin( kPI - rotation );
				pt *= xform;
				xform = transform;
		//		xform.PostTranslate( xOffset, yOffset );
				xform.PostTranslate( pt.m_x, pt.m_y );
				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, shadow, RHeadlineColor(shadow) );
				RSingleGraphic::Render( drawingSurface, size, xform, rRender, monochromeColor, fImageAsMask );
				pt.m_x = xOffset;
				pt.m_y = yOffset;
				xform.MakeIdentity();
				xform.PreRotateAboutOrigin( kPI - rotation );
				pt *= xform;
				xform = transform;
		//		xform.PostTranslate( -xOffset, -yOffset );
				xform.PostTranslate( pt.m_x, pt.m_y );
				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, hilite, RHeadlineColor(hilite) );
				RSingleGraphic::Render( drawingSurface, size, xform, rRender, monochromeColor, fImageAsMask );
				ConfigureDrawingCharacteristics( baseXform, kFilled, penHairline, strokeColor, fillColor );
				RSingleGraphic::Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
			}
			break;
		case kGlintOutline:
		case kRoundedOutline:
			UnimplementedCode();
			break;
		default:
			TpsAssertAlways( "Unrecognized outline effect type." );
			break;
		}
	//
	// restore original graphic points, if necessary
	if ( origPoints.Count() != 0 )
	{
		ptz = origPoints.Start();
		ptz2 = m_OriginalPointsStream.Start();
m_TransformedIntPointsStream.Empty();
//		ptz1 = m_TransformedIntPointsStream.Start();
		ptz3 = m_TransformedRealPointsStream.Start();
		while ( ptz != origPoints.End() )
		{
//			*ptz1 = *ptz;
			*ptz2 = *ptz;
			*ptz3 = *ptz;
			++ptz;
//			++ptz1;
			++ptz2;
			++ptz3;
		}
		m_LastTransform.MakeIdentity();
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::RenderProjection( )
//
// Description:		Render the projection (stack/extrusion) effects
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::RenderProjection( RDrawingSurface& drawingSurface,
										 		  	  const RRealSize& size,
											 		  const R2dTransform& transform,
													  const RIntRect& rRender,
											 		  const RColor& monochromeColor,
										 			  BOOLEAN fImageAsMask )

{
int 								i;
R2dTransform					baseXform;
YIntDimension 					maxPenWeight = CalcMaxPenWidthBasedOnOutlineEffect( m_OutlineEffect, m_LineWeight ); 
R2dTransformArrayIterator	tz = m_ProjectionTransforms.End(); 

	//
	// Validations...
	TpsAssertValid( this );
	TpsAssert( m_TextDefined, "Render of undefined headline." );
	TpsAssert( m_GraphicDefined, "Render of undefined headline." );
	//
	// Initialize the base transformation...
	if ( m_ProjectionTransforms.Count() == 0 )
		baseXform.MakeIdentity();
	else
		baseXform = *(m_ProjectionTransforms.Start());
	--tz;
	//
	// skip shadow effect transform
	switch ( m_ShadowEffect )
	{
		case kNoShadow:
			break;
		case kDropShadow:
		case kCastShadow:
		case kSoftDropShadow:
		case kSoftCastShadow:
			--tz;
			break;
		default:
			TpsAssertAlways( "Unrecognized shadow effect type." );
			break;
	}
	//
	// render the projection effects
	// color is m_ProjectionFillDesc
	switch ( m_ProjectionEffect )
	{
		case kNoProjection:
			break;
		case kSimpleStack:
			{
				YFloatType	yLastStage( YFloatType( m_NrProjectionStages - 1 ) );
				for( i = m_NrProjectionStages; i > 0; i-- )
				{
					YPercentage		yStagePercent( YPercentage( i - 1 ) / yLastStage );
					RSolidColor		strokeColor( m_ProjectionFillDesc.GetSolidColor( yStagePercent ) );
					RHeadlineColor	fillColor( strokeColor );
					RenderOutline( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask,
										kSimpleFillStroke, maxPenWeight, strokeColor, fillColor, m_BackgroundBlur2Color, ( *tz * baseXform ) );
					--tz;
				}
			}
			break;
		case kSpinStack:
			{
				YFloatType	yLastStage( YFloatType( m_NrProjectionStages - 1 ) );
				for( i = m_NrProjectionStages; i > 0; i-- )
				{
					YPercentage		yStagePercent( YPercentage( i - 1 ) / yLastStage );
					RSolidColor		strokeColor( m_ProjectionFillDesc.GetSolidColor( yStagePercent ) );
					RHeadlineColor	fillColor( strokeColor );
					RenderOutline( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask,
										kSimpleFillStroke, maxPenWeight, strokeColor, fillColor, m_BackgroundBlur2Color, ( *tz * baseXform ) );
					--tz;
				}
			}
			break;
		case kPseudoPerspectiveStack:
			{
				YFloatType	yLastStage( YFloatType( m_NrProjectionStages - 1 ) );
				for( i = m_NrProjectionStages; i > 0; i-- )
				{
					YPercentage		yStagePercent( YPercentage( i - 1 ) / yLastStage );
					RSolidColor		strokeColor( m_ProjectionFillDesc.GetSolidColor( yStagePercent ) );
					RHeadlineColor	fillColor( strokeColor );
					RenderOutline( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask,
										kSimpleFillStroke, maxPenWeight, strokeColor, fillColor, m_BackgroundBlur2Color, ( *tz * baseXform ) );
					--tz;
				}
			}
			break;
		case kSolidExtrusion :
		case kIlluminateExtrusion :
		case kPseudoPerspectiveExtrusion :
			{
				TpsAssert( ( m_NrProjectionStages == 1 ), "Bad projection data." );
				TpsAssert( ( m_pExtrusionGraphic != NULL ), "Bad projection data." );
				if ( m_ProjectionFillDesc.UsesGradient() || drawingSurface.GetColoringBookMode() /*|| kBeveledOutline == m_OutlineEffect*/ )
				{
					YCounter							nrStages = m_ExtrusionTransforms.Count();
					R2dTransformArrayIterator	xz = m_ExtrusionTransforms.End(); 
					--xz;
					YFloatType	yLastStage( YFloatType( nrStages - 1 ) );
					for( i = nrStages; i > 0; i-- )
					{
						YPercentage		yStagePercent( YPercentage( i - 1 ) / yLastStage );
						RSolidColor		strokeColor( m_ProjectionFillDesc.GetSolidColor( yStagePercent ) );
						RHeadlineColor	fillColor( strokeColor );
						RenderOutline( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask,
											kSimpleFillStroke, maxPenWeight, strokeColor, fillColor, m_BackgroundBlur2Color, ( *xz * baseXform ) );
#ifdef	TPSDEBUG
						{
							YAngle				rotation;
							YRealDimension		xScale;
							YRealDimension		yScale;
							(*xz).Decompose( rotation, xScale, yScale );
							TpsAssert( ( xScale >= 0 ), "Bad extrusion transform." );
							TpsAssert( ( yScale >= 0 ), "Bad extrusion transform." );
		//					TpsAssert( ( rotation == 0 ), "Bad extrusion transform." );	
						}
#endif
						--xz;
					}
				}
				else if ( m_pExtrusionGraphic != NULL )
				{
					m_ProjectionFillDesc.SetBoundingRect( RIntRect( RIntSize( GetGraphicSize() ) ) );
					RSolidColor		strokeColor( m_ProjectionFillDesc.GetSolidColor() );
					const RColor&	fillColor = m_ProjectionFillDesc.GetColor();
					if ( m_OutlineEffect == kSimpleFill ) 
						m_pExtrusionGraphic->SetFillMethods( kFilled );
					else
					{
						YCounter	nrMarkers = ( ( m_DistortEffect == kFollowPath ) ? ( kNrPathMarkerPoints + 1 ) : ( kNrExtremeMarkerPoints + 1 ) );
						m_pExtrusionGraphic->SetFillMethods( kFilledAndStroked );
						m_pExtrusionGraphic->SetFillMethods( nrMarkers, kFilled );					// the 1st n paths are extremes markers
					}
					m_pExtrusionGraphic->SetDefaultColors( strokeColor, fillColor );
					m_pExtrusionGraphic->SetDefaultPenWidth( CalcTransformedPenWidth( maxPenWeight, baseXform ) );
					m_pExtrusionGraphic->Render( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask );
				}
				--tz;
			}
			break;
		default:
			TpsAssertAlways( "Unrecognized projection effect type." );
			break;
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::RenderShadow( )
//
// Description:		Render the shadow effects
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::RenderShadow( RDrawingSurface& drawingSurface,
										 		 const RRealSize& size,
											 	 const R2dTransform& transform,
												 const RIntRect& rRender,
											 	 const RColor& monochromeColor,
										 		 BOOLEAN fImageAsMask )

{
R2dTransform					baseXform;
YIntDimension 					maxPenWeight = CalcMaxPenWidthBasedOnOutlineEffect( m_OutlineEffect, m_LineWeight ); 
R2dTransformArrayIterator	tz = m_ProjectionTransforms.End(); 

	//
	// Validations...
	TpsAssertValid( this );
	TpsAssert( m_TextDefined, "Render of undefined headline." );
	TpsAssert( m_GraphicDefined, "Render of undefined headline." );
	//
	// Initialize the base transformation...
	if ( m_ProjectionTransforms.Count() == 0 )
		baseXform.MakeIdentity();
	else
		baseXform = *(m_ProjectionTransforms.Start());
	--tz;
	//
	// render shadow effects
	// color is m_ShadowFillDesc
	switch ( m_ShadowEffect )
	{
		case kNoShadow:
			break;
		case kDropShadow:
		case kCastShadow:
			{
				RenderOutline( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask,
									kSimpleFillStroke, maxPenWeight, m_ShadowFillDesc.GetSolidColor(), m_ShadowFillDesc,
									m_BackgroundBlur2Color, ( *tz * baseXform ) );
				--tz;
			}
			break;
		case kSoftDropShadow:
		case kSoftCastShadow:
			{
				TpsAssert( !m_ShadowFillDesc.UsesGradient(), "Soft shadows may not have a gradient fill." );
				RenderOutline( drawingSurface, size, transform, rRender, monochromeColor, fImageAsMask,
									kShadowedOutline, maxPenWeight, m_ShadowFillDesc.GetSolidColor(), m_ShadowFillDesc,
									m_BackgroundShadow2Color, ( *tz * baseXform ) );
				--tz;
			}
			break;
		default:
			TpsAssertAlways( "Unrecognized shadow effect type." );
			break;
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::RenderFrame( )
//
// Description:		Render the frame of the headline.
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::RenderFrame( RDrawingSurface& drawingSurface,
										 		const RRealSize& size,
										 		const RRealSize& oneUnit,
										 		const R2dTransform& transform,
												const RIntRect& rRender )

{
	if ( m_FrameEffect == kSilhouette )
	{
		RenderSilhouette( drawingSurface, size, transform, rRender );
	}
	else if ( m_FrameEffect != kFrameNoFrame )
	{
		//
		// Don't bother if it's transparent 
		if ( !m_FrameFillDesc.IsTransparent() )
		{
			RPath			outline;
			RHeadlineColor		headlineColor = ( ( m_FrameEffect == kSimpleFrame ) ? m_FrameFillDesc : m_OutlineFillDesc );
			if ( m_FrameEffect == kEmboss || m_FrameEffect == kSoftEmboss || 
				  m_FrameEffect == kDeboss || m_FrameEffect == kSoftDeboss )
			{
					YAngle	angle = headlineColor.GetAngle();
					headlineColor.SetAngle( /*kPI*/ -angle );
			}
			RColor		fillColor = headlineColor.GetColor();
//			fillColor.SetBoundingRect( RIntRect( RRealRect( size ) * transform ) );
			fillColor.SetBoundingRect( RIntRect( RRealRect( size ) ) );
			fillColor	*= transform;
			RRealSize	envelopeSize( ( size.m_dx + oneUnit.m_dx ), ( size.m_dy + oneUnit.m_dy ) );
			if ( m_pFrameEnvelope != NULL )
			{
				// render the frame envelope...
				outline = *m_pFrameEnvelope;
			}
			else if ( !m_FrameRect.IsEmpty() )
			{
				// no frame envelope, so use the framing rect...
				outline.Define( m_FrameRect );
			}
			else if ( m_DistortEffect == kWarpToEnvelop )
			{
				// no frame, so use the distort envelop
				outline = *m_pPathEnvelope;
			}
			else if ( m_DistortEffect == kFollowPath )
			{
				// no frame, so use the follow path
				outline.Define( RIntRect( m_BoundingSize ) );				// REVEIW RAH this would work better as the follow path envelop
			}
			else
			{
				// no frame or distort effect, so use the bounding rect
				outline.Define( RIntRect( m_BoundingSize ) );
			}
			// render 
			outline.Render( drawingSurface, envelopeSize, transform, rRender, kFilled, RSolidColor( kBlack), fillColor );
#ifdef  FRAME_TEST_CODE
			if ( m_DistortEffect == kWarpToEnvelop )
			{
				R2dTransform	xformScale;
	//			RRealSize		graphicSize( GetGraphicSize() );
	//			RRealRect		rcTopPath( m_pPathTop->GetBoundingRect() );
	//			RRealRect		rcBottomPath( m_pPathBottom->GetBoundingRect() );
				DefaultDrawingCharacteristics( drawingSurface );
				if ( m_pPathTop != NULL )
				{
					RSolidColor	strokeColor( kRed );
					xformScale = CalcWarpPathScaleXform( IsHorizontal(), kPathTop, 
																	 m_pPathTop->GetBoundingRect(), 
																	 m_pPathBottom->GetBoundingRect(), 
																	 GetGraphicSize(), 
																	 ( m_pPathTop->UsesBeziers() || m_pPathTop->UsesSplines() ),
																	 ( m_pPathBottom->UsesBeziers() || m_pPathBottom->UsesSplines() ) );
					outline = RPath( *m_pPathTop, xformScale );
					outline.SetPathSize( GetGraphicSize() );
					outline.Render( drawingSurface, envelopeSize, transform, rRender, kStroked, strokeColor, fillColor );
				}
				if ( m_pPathBottom != NULL )
				{
					RSolidColor	strokeColor( kBlue );
					xformScale = CalcWarpPathScaleXform( IsHorizontal(), kPathBottom, 
																	 m_pPathTop->GetBoundingRect(), 
																	 m_pPathBottom->GetBoundingRect(), 
																	 GetGraphicSize(), 
																	 ( m_pPathTop->UsesBeziers() || m_pPathTop->UsesSplines() ),
																	 ( m_pPathBottom->UsesBeziers() || m_pPathBottom->UsesSplines() ) );
					outline = RPath( *m_pPathBottom, xformScale );
					outline.SetPathSize( GetGraphicSize() );
					outline.Render( drawingSurface, envelopeSize, transform, rRender, kStroked, strokeColor, fillColor );
				}
			}
#endif 
		}
	}
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::RenderSilhouette( )
//
// Description:		Render the frame as a silhouette of the headline.
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::RenderSilhouette( RDrawingSurface& drawingSurface,
										 			  const RRealSize& size,
									 		 		  const R2dTransform& transform,
													  const RIntRect& rRender )
{
ROffscreenDrawingSurface	dsFrame;
ROffscreenDrawingSurface	dsMask;
RScratchBitmapImage			xbmFrame;
RScratchBitmapImage			xbmMask;
R2dTransform					xform;
RSolidColor						whiteColor( kWhite );
RSolidColor						blackColor( kBlack );
RHeadlineColor					fillColor = blackColor;

	// paranoia
	m_OutlineEffect = kSimpleFill;
	m_OutlineFillDesc = RColor(); 	// Transparent;
	m_ProjectionEffect = kNoProjection;
	m_ShadowEffect = kNoShadow;
	// Get the bounding rect of the bitmap
	RIntVectorRect boundingRect( static_cast<RIntSize>( size ) );
	boundingRect *= transform;
	RIntSize bitmapSize = boundingRect.m_TransformedBoundingRect.WidthHeight( );
	RIntRect bitmapRect( bitmapSize );
	// Get the position of the data
	RIntPoint topLeft = RIntPoint( boundingRect.m_TransformedBoundingRect.m_Left, boundingRect.m_TransformedBoundingRect.m_Top );
	// Make a new transform that will position the data correctly in the bitmap
	R2dTransform tempTransform = transform;
	tempTransform.PostTranslate( -topLeft.m_x, -topLeft.m_y );
	// Create mask
	xbmMask.Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 1);
	// Select the mask bitmap into offscreen drawing surface
	dsMask.SetImage( &xbmMask );
	dsMask.SetForegroundColor( blackColor );
	dsMask.SetBackgroundColor( whiteColor );
	// Fill the background of the mask
	dsMask.SetFillColor( RColor( whiteColor ) );
	dsMask.FillRectangle( bitmapRect );										// REVEIW RAH should fill the warp/frame envelop if there is one
	// image the headline into the mask
	ConfigureDrawingCharacteristics( xform, kFilled, 1, blackColor, fillColor );
	RSingleGraphic::Render( dsMask, size, tempTransform, rRender, RColor( blackColor ), FALSE );
	// debug code
#ifdef	TPSDEBUG
static	BOOLEAN	fDrawMaskTest = 0;
	if ( fDrawMaskTest )
	{
		drawingSurface.BlitDrawingSurface( dsMask, bitmapRect, bitmapRect, kBlitSourceCopy );
	}
#endif	//	TPSDEBUG
	dsMask.ReleaseImage( );
	// Create the frame bitmap
	xbmFrame.Initialize( drawingSurface, bitmapSize.m_dx, bitmapSize.m_dy );
	dsFrame.SetImage( &xbmFrame );
	dsFrame.CopyAttributes( drawingSurface );
	dsFrame.SetFillColor( m_FrameFillDesc.GetColor() );
	dsFrame.FillRectangle( bitmapRect );
	dsFrame.ReleaseImage( );
	// render the frame bitmap thru the mask to the output drawing surface
	xbmFrame.RenderWithMask( drawingSurface, xbmMask, RIntRect( topLeft, bitmapSize ) );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::RenderFeedback( )
//
// Description:		Render the resize feedback of the headline.
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::RenderFeedback( RDrawingSurface& drawingSurface,
										 		 	const RRealSize& size,
										 		 	const R2dTransform& transform )

{
R2dTransform		renderXform = CalcRenderXform( size, transform );
RRealSize			envelopeSize, oneUnit( 1.0, 1.0 ), marginSize( kDefaultMargin, kDefaultMargin );
RRealRect			marginRect( GetGraphicSize() ), rcBounds;
RRealVectorRect	boundingRect;
RIntRect				renderRect;
RIntPoint			points[ 4 ];
RSolidColor			strokeColor( kBlack );
RColor				fillColor( strokeColor );
//YAngle				rotation;
//YRealDimension		xScale;
//YRealDimension		yScale;

	// validations/initializations
	::DeviceUnitsToLogicalUnits( oneUnit, drawingSurface );
	envelopeSize.m_dx = size.m_dx - oneUnit.m_dx;
	envelopeSize.m_dy = size.m_dy - oneUnit.m_dy;
	renderRect = marginRect;
	marginRect.Inset( marginSize );
	// RAH test code
#ifdef	TPSDEBUG
	#ifdef	IMAGER
		#define  FEEDBACK_TEST_CODE		 0
	#endif	
#endif	
#ifdef  FEEDBACK_TEST_CODE
	if ( m_GraphicSize.m_dx != 0 && m_GraphicSize.m_dy != 0 )
	{
		RPath	outline;
		outline.Define( RRealRect( m_GraphicSize ) );
		outline.Render( drawingSurface, envelopeSize, transform, renderRect, kStroked, strokeColor, fillColor );
	}
#endif 
	// 
	if ( m_DistortEffect == kWarpToEnvelop ) 
	{
		if ( m_pPathEnvelope == NULL )
		{
			// render the current bounding box
			boundingRect = RRealVectorRect( marginRect );
			boundingRect *= renderXform;
			drawingSurface.FrameVectorRect( RIntVectorRect( boundingRect ) );
		}
		else
		{
			R2dTransform	xform;
			RRealSize		graphicSize = GetGraphicSize();
			RRealSize		pathSize = m_pPathEnvelope->GetPathSize();
			pathSize.m_dx += oneUnit.m_dx;
			pathSize.m_dy += oneUnit.m_dy;
			xform.PreScale( ( graphicSize.m_dx / pathSize.m_dx ), ( graphicSize.m_dy / pathSize.m_dy ) );
			RPath				outline( *m_pPathEnvelope, xform );
			outline.SetPathSize( graphicSize );
//			outline.Render( drawingSurface, envelopeSize, transform, renderRect, kStroked, strokeColor, fillColor );
//			m_pPathEnvelope->Render( drawingSurface, envelopeSize, transform, renderRect, kStroked, RColor( strokeColor ) );
		}
	}
	else if ( m_DistortEffect == kFollowPath )
	{
			// TBD


			// render the current bounding box
			boundingRect = RRealVectorRect( marginRect );
			boundingRect *= renderXform;
			drawingSurface.FrameVectorRect( RIntVectorRect( boundingRect ) );



	}
	else if ( m_TextDefined && m_GraphicDefined )
	{
		int	k;
		drawingSurface.SetPenStyle( kDashPen );
		for ( k = 0; k < m_NrLines; k++ )
			if ( IsHorizontal() )
			{
				points[0].m_x = ::Round( marginRect.m_Left );
				points[0].m_y = m_LineStartPoints[k].m_y;
				points[1].m_x = ::Round( marginRect.m_Right );
				points[1].m_y = m_LineStartPoints[k].m_y;
				points[0] *= renderXform;
				points[1] *= renderXform;
				drawingSurface.Polyline( points, 2 );
			}
			else
			{
				points[0].m_x = m_LineStartPoints[k].m_x;
				points[0].m_y = ::Round( marginRect.m_Top );
				points[1].m_x = m_LineStartPoints[k].m_x;
				points[1].m_y = ::Round( marginRect.m_Bottom );
				points[0] *= renderXform;
				points[1] *= renderXform;
				drawingSurface.Polyline( points, 2 );
			}
		drawingSurface.SetPenStyle( kSolidPen );
#if 0
		R2dTransformArrayIterator	tz = m_LineScaleTransforms.Start(); 
		RIntRectArrayIterator		rz = m_BaseStringBoundingBoxes.Start(); 
		for ( k = 0; k < m_NrLines; k++ )
		{
			(*tz).Decompose( rotation, xScale, yScale );
			rcBounds = *rz;
			rcBounds.m_Top *= yScale;
			rcBounds.m_Top += m_LineStartPoints[k].m_y;
			if ( rcBounds.m_Top > marginRect.m_Top )
				rcBounds.m_Top = marginRect.m_Top;
			rcBounds.m_Left = marginRect.m_Left;
			rcBounds.m_Bottom *= yScale;
			rcBounds.m_Bottom += m_LineStartPoints[k].m_y;
			if ( rcBounds.m_Bottom < marginRect.m_Bottom )
				rcBounds.m_Bottom = marginRect.m_Bottom;
			rcBounds.m_Right = marginRect.m_Right;
			boundingRect = RRealVectorRect( rcBounds );
			boundingRect *= renderXform;
			drawingSurface.FrameVectorRect( RIntVectorRect( boundingRect ) );
			++tz;
			++rz;
		}
#endif
	}
	else
	{
		// render the current bounding box
		boundingRect = RRealVectorRect( marginRect );
		boundingRect *= renderXform;
		drawingSurface.FrameVectorRect( RIntVectorRect( boundingRect ) );
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ConfigureDrawingCharacteristics( )
//
// Description:		Configure drawing surface characteristics in preperation
//							to render graphic
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::ConfigureDrawingCharacteristics( const R2dTransform& transform,
																		  YFillMethod method, 
																		  YIntDimension logicalWidth, 
																		  const RSolidColor& stroke, 
																		  RHeadlineColor& fill ) 
{
	//
	// make sure the fill color is relative to our graphic...
//	fill.SetBoundingRect( RIntRect( RIntSize( GetGraphicSize() ) ) );
	RSingleGraphic::SetDefaultColors( stroke, ( fill.GetColor() * transform ) );
	RSingleGraphic::SetFillMethods( method );
	if ( method != kFilled ) 
	{
		YCounter	nrMarkers = ( ( m_DistortEffect == kFollowPath ) ? kNrPathMarkerPoints : kNrExtremeMarkerPoints );
		RSingleGraphic::SetFillMethods( nrMarkers, kFilled );						// the 1st n paths are extremes markers
	}
	RSingleGraphic::SetDefaultPenWidth( CalcTransformedPenWidth( logicalWidth, transform ) );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::DefaultDrawingCharacteristics( )
//
// Description:		Restore the default drawing surface characteristics in 
//							preperation to render a graphic
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::DefaultDrawingCharacteristics( RDrawingSurface& drawingSurface ) 
{
	drawingSurface.RestoreDefaults();
	drawingSurface.SetTint( m_Tint );
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ApplyTransform( )
//
// Description:		Superseeds the RSingleGraphic::ApplyTransform() to allow
//    					take effects into account.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::ApplyTransform( const R2dTransform& transform )
{
	// just call the base class untill such time as i implement this
	RSingleGraphic::ApplyTransform( transform );
}


#ifdef	TPSDEBUG
// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::Validate( ) const
{
	RSingleGraphic::Validate();	
	TpsAssertIsObject( RHeadlineGraphic, this );
	TpsAssert( ( m_Justification >= kLeftJust && m_Justification <= kFullJust ), "Bad justification type." );
//	TpsAssert( ( m_Escape >= kLeft2RightTop2Bottom && m_Escape <= kBottom2TopRight2Left ), "Bad escapement type." );
	TpsAssert( ( m_Escape == kLeft2RightTop2Bottom || m_Escape == kTop2BottomLeft2Right ), "Bad escapement type." );
	TpsAssert( ( m_Compensate >= kRegular && m_Compensate <= kLoose ), "Bad compensation type." );
	TpsAssert( ( m_DistortEffect >= kNonDistort && m_DistortEffect <= kAltShear ), "Bad distort effect type." );
	TpsAssert( ( m_OutlineEffect >= kSimpleStroke && m_OutlineEffect <= kBeveledOutline ), "Bad outline effect type." );
	TpsAssert( ( m_ProjectionEffect >= kNoProjection && m_ProjectionEffect <= kIlluminateExtrusion ), "Bad projection effect type." );
	TpsAssert( ( m_ShadowEffect >= kNoShadow && m_ShadowEffect <= kSoftCastShadow ), "Bad shadow effect type." );
	TpsAssert( ( m_FrameEffect >= kFrameNoFrame && m_FrameEffect <= kSimpleFrame ), "Bad frame effect type." );
	TpsAssert( ( m_OutlineFillDesc.IsValid() ), "Bad character fill type." );
	TpsAssert( ( m_ProjectionFillDesc.IsValid() ), "Bad projection fill type." );
	TpsAssert( ( m_ShadowFillDesc.IsValid() ), "Bad projection fill type." );
	TpsAssert( ( m_FrameFillDesc.IsValid() ), "Bad frame fill type." );
	TpsAssert( ( m_LineWeight >= kHairLine && m_LineWeight <= kExtraThickLine ), "Bad line weight type." );
	TpsAssert( ( m_MinGradBandWidth >= 1.0 ), "Bad minimum gradient band width." );
	if ( m_DistortEffect == kFollowPath )
	{
		TpsAssert( ( m_pPathBottom != NULL ), "Bad path." );
		TpsAssert( ( m_Verticality == kPin2Top || m_Verticality == kPin2Middle || m_Verticality == kPin2Bottom ), "Bad follow path placement." );
		TpsAssert( ( m_MaxPathHeightScale > 0 && m_MaxPathHeightScale <= 1 ), "Bad follow path max scale factor." );
	 	TpsAssert( ( m_Justification == kCenterJust ), "Bad follow path justification (must be center)." );
		TpsAssert( !m_DistortGraphic, "Bad distortion flag." );
	}
	else if ( m_DistortEffect == kWarpToEnvelop )
	{
		TpsAssert( m_DistortGraphic, "Bad distortion flag." );
		TpsAssert( ( m_pPathTop != NULL ), "Bad path." );
		TpsAssert( ( m_pPathBottom != NULL ), "Bad path." );
		if ( IsHorizontal() )
		{
			TpsAssert( ( m_Verticality == kPin2Top || m_Verticality == kPin2Middle || m_Verticality == kPin2Bottom ), "Bad warp placement." );
			TpsAssert( IsHorizontalWarpId( m_WarpShapeOrPathId ), "Bad warp Id." );
		}
		else if ( IsVertical() )
		{
			TpsAssert( ( m_Verticality == kPin2Left || m_Verticality == kPin2Middle || m_Verticality == kPin2Right ), "Bad warp placement." );
			TpsAssert( IsVerticalWarpId( m_WarpShapeOrPathId ), "Bad warp Id." );
		}
		else 
		{
			TpsAssert( FALSE, "Bad escapement type." );
		}
	}
	else
	{
		TpsAssert( !m_PathRect.IsEmpty(), "Bad path." );
	}
//	TpsAssertValid( ( &m_StrokeColor ) );
//	TpsAssertValid( ( &m_FillColor ) );
//	TpsAssertValid( ( &m_ShadowColor ) );
//	TpsAssertValid( ( &m_ProjectionColor ) );
//	TpsAssertValid( ( &m_FrameColor ) );
	if ( m_TextDefined )
	{
		TpsAssert( ( m_pFont != NULL ), "Undefined font." );
		TpsAssert( ( m_NrLines > 0 && m_NrLines <= kMaxHeadlineLines ), "Bad number of lines." );
		TpsAssert( ( m_LineTextList.Count() != 0 ), "Text not defined." );
	


	}
	if ( m_GraphicDefined )
	{
		TpsAssert( ( m_BoundingSize.m_dx != 0 && m_BoundingSize.m_dy != 0 ), "Bad bounding size." );
		TpsAssert( !m_OutlineRect.IsEmpty(), "Bad bounding box." );
		TpsAssert( !m_BoundingBox.IsEmpty(), "Bad bounding box." );
//		if ( m_ProjectionEffect == kNoProjection && m_ShadowEffect == kNoShadow )
		{
			TpsAssert( ( m_LineScaleTransforms.Count() != 0 ), "Scale transforms not defined." );
		}
		TpsAssert( ( m_BaseStringBoundingBoxes.Count() != 0 ), "Text bounding boxes not defined." );


	
	}
	if ( m_ProjectionEffect != kNoProjection )
	{
		TpsAssert( ( m_ProjectionTransforms.Count() != 0 ), "Projection transforms not defined." );
		TpsAssert( ( m_NrProjectionStages > 0 ), "Bad number of stages." );
		TpsAssert( ( m_ProjectionDepth >= 0 ), "Bad projection data." );
		TpsAssert( ( m_VanishingLength >= 0 ), "Bad projection data." );
		TpsAssert( ( m_ProjectionAngle > -360 && m_ProjectionAngle < 360 ), "Bad projection data." );
		if ( m_ProjectionEffect == kSolidExtrusion || m_ProjectionEffect == kIlluminateExtrusion || m_ProjectionEffect == kPseudoPerspectiveExtrusion )
		{
			TpsAssert( ( m_NrProjectionStages == 1 ), "Bad projection data." );
			TpsAssert( ( m_pExtrusionGraphic != NULL ), "Bad projection data." );
		}
	}
	if ( m_ShadowEffect != kNoShadow )
	{
		TpsAssert( ( m_ProjectionTransforms.Count() != 0 ), "Shadow transforms not defined." );
		TpsAssert( ( m_ShadowDepth >= 0 ), "Bad shadow data." );
		TpsAssert( ( m_ShadowingLength >= 0 ), "Bad shadow data." );
		TpsAssert( ( m_ShadowAngle > -360 && m_ShadowAngle < 360 ), "Bad shadow data." );
		TpsAssert( ( m_ShadowSlope > -1001 && m_ShadowSlope < 1001 ), "Bad shadow data." );
	}
	if ( m_FrameEffect != kFrameNoFrame )
	{
		// TBD
	}
	TpsAssert( ( m_SpinDirection == 1 || m_SpinDirection == 0 || m_SpinDirection == -1 ), "Bad projection data." );
	TpsAssert( ( m_ShearAngle > -90 && m_ShearAngle < 90 ), "Bad shear data." );
	TpsAssert( ( m_ShearRotaion > -90 && m_ShearRotaion < 90 ), "Bad shear data." );
	TpsAssert( ( m_BaselineAlternate >= 0 && m_BaselineAlternate <= 1 ), "Bad baseline alternate data." );
}
#endif	// TPSDEBUG


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::Redefine( )
//
// Description:		Initializes the headline graphic.
//
// Returns:				Nothing.
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::Redefine( ) 
{
int							count = 0;

	RSingleGraphic::Undefine( RSolidColor( kBlack), RSolidColor( kBlack) );
	m_GraphicDefined = m_TextDefined = FALSE;
	m_EmSquareTransform.MakeIdentity();
	m_LineScaleTransforms.Empty();
	m_BaseStringBoundingBoxes.Empty();
	m_ProjectionTransforms.Empty();
	m_GraphicSize = GetDefaultHeadlineSize( IsHorizontal() );
	m_BoundingBox = RRealRect( 0.0, 0.0, 0.0, 0.0 );
	delete m_pExtrusionGraphic;
	m_pExtrusionGraphic = NULL;
	m_MinGradBandWidth = 1.0;
	m_FlipDirection = kFlipNone;
	if ( m_pFont == NULL || m_LineTextList.Count() == 0 || m_NrLines == 0 )
		return;
	if ( m_GraphicSize.m_dx == 0 || m_GraphicSize.m_dy == 0 )
		return;
	if ( m_LineScaleProportions[0] == 0 && m_LineScaleProportions[1] == 0 && m_LineScaleProportions[2] == 0 )
		return;
	// make sure there are any characters in the headline 
	for ( RMBCStringIterator cz = m_OriginalText.Start(); cz != m_OriginalText.End(); ++cz )
	{
		if ( *cz != kCarrageReturn && *cz != kLineFeed )
			count++;
	}
	if ( m_DistortEffect == kWarpToEnvelop )
	 	m_DistortGraphic = TRUE;
	else if ( m_DistortEffect == kFollowPath )
	 	m_DistortGraphic = FALSE;
	m_TextDefined = static_cast<BOOLEAN>( count != 0 );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcNominalBoundingSize( )
//
// Description:		 
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RRealSize RHeadlineGraphic::CalcNominalBoundingSize( BOOLEAN fDistort ) 
{
RRealSize			  size = GetDefaultHeadlineSize( IsHorizontal() );
BOOLEAN				  fDistorted = m_DistortGraphic;
EDistortEffects	  eDistorted = m_DistortEffect;
RIntRect			 	  rcBoundingBox( 0, 0, 0, 0 );
RIntRect				  rcBounds;

	// validations/initializations
	TpsAssert( ( m_DistortEffect != kFollowPath ), "No nominal size for following path." );
	if ( m_DistortEffect == kWarpToEnvelop )
	 	m_DistortEffect = kNonDistort;
	m_DistortGraphic = fDistort;
	Redefine();
	if ( !CalcBaseGraphicScale( size ) )
		return size;
	// add extreme points of each line to the bounding box of the graphic
	R2dTransformArrayIterator	tz = m_LineScaleTransforms.Start(); 
	RIntRectArrayIterator		rz = m_BaseStringBoundingBoxes.Start(); 
	for ( int k = 0; k < m_NrLines; k++ )
	{
		rcBounds = *rz * *tz;
		rcBounds.m_TopLeft += m_LineStartPoints[k];
		rcBounds.m_BottomRight += m_LineStartPoints[k];
		if ( rcBoundingBox.IsEmpty() )
			rcBoundingBox = rcBounds;
		else
			rcBoundingBox.Union( rcBoundingBox, rcBounds );
		++tz;
		++rz;
	}
	// reinitialize
	if ( m_DistortEffect != eDistorted  || m_DistortGraphic != fDistorted )
	 	Redefine();
	m_DistortEffect = eDistorted;
	m_DistortGraphic = fDistorted;

	return RRealSize( ::Abs( rcBoundingBox.Width() ), ::Abs( rcBoundingBox.Height() ) );
}

#ifndef	IMAGER
// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::Construct( )
//
// Description:		Construct the base graphic and compute the effects of the 
//							Headline.
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::Construct( const RView& view ) 
{
	Construct( GetDefaultHeadlineSize( IsHorizontal() ), view );
}
#endif

#ifndef	IMAGER
// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::Construct( )
//
// Description:		Construct the base graphic and compute the effects of the 
//							Headline.
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::Construct( const RRealSize& size, const RView& view ) 
{
	// Compute the smallest band size
	RRealSize		deviceDPI( view.GetDPI() );
	YRealDimension	nDeviceDPI( (YRealDimension)::Max( deviceDPI.m_dx, deviceDPI.m_dy ) );
	YRealDimension	nMaxGradientDPI( (YRealDimension)::GetMaxGradientDPI() );
	m_MinGradBandWidth = ( ( nDeviceDPI > nMaxGradientDPI ) ? ( nDeviceDPI / nMaxGradientDPI ) : 1.0 );
	// Construct
	Construct( size );
}
#endif

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::Construct( )
//
// Description:		Construct the base graphic and compute the effects of the 
//							Headline.
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::Construct( const RDrawingSurface& drawingSurface ) 
{
	Construct( GetDefaultHeadlineSize( IsHorizontal() ), drawingSurface );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::Construct( )
//
// Description:		Construct the base graphic and compute the effects of the 
//							Headline.
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::Construct( const RRealSize& size, const RDrawingSurface& drawingSurface ) 
{
	// Compute the smallest band size
	RRealSize		deviceDPI( drawingSurface.GetDPI() );
	YRealDimension	nDeviceDPI( (YRealDimension)::Max( deviceDPI.m_dx, deviceDPI.m_dy ) );
	YRealDimension	nMaxGradientDPI( (YRealDimension)::GetMaxGradientDPI() );
	m_MinGradBandWidth = ( ( nDeviceDPI > nMaxGradientDPI ) ? ( nDeviceDPI / nMaxGradientDPI ) : 1.0 );
	// Construct
	Construct( size );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::Construct( )
//
// Description:		Construct the base graphic and compute the effects of the 
//							Headline.
//
// Returns:				Nothing.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::Construct( const RRealSize& size ) 
{
RIntSize 	delta;
BOOLEAN 		fHorizontal = IsHorizontal();
#ifndef	IMAGER
RWaitCursor	waitCursor;
#endif	

	if ( !CalcBaseGraphicScale( size ) )
		return;
	// construct the base graphic
	if ( m_LineScaleTransforms.Count() == 0 )
		m_GraphicDefined = FALSE;
	else
	{
		delta = CalcMarginBasedOnEffects();						// recompute whitespace using line widths
		PrepGraphicForConstruct();

		switch ( m_DistortEffect )
		{
			case kFollowPath :
				ConstructBaseGraphicViaPath();
				break;
			case kWarpToEnvelop :
				ConstructBaseGraphic( fHorizontal );
//				AdjustBaseGraphicForWarp( fHorizontal, m_WhiteSpace, delta );
				WarpBaseGraphic( delta );
				break;
			default :
				ConstructBaseGraphic( fHorizontal );
				break;
		}
		TpsAssert( ( delta == CalcMarginBasedOnEffects() ), "Bad margin computation." );
		AdjustBaseGraphicAfterConstruct( delta );
	}
	if ( !m_GraphicDefined )
		return;
	// trim all extranious white space from banner headlines
	if ( m_TrimWhiteSpace && m_DistortGraphic )
		TrimBaseGraphicWhiteSpace( delta );
	// calc a projection (stack/extrusion/shadow) transform for each stage
	CalcProjectionXforms( );
	BuildProjectionExtrusion();
	// trim all extranious white space from banner headlines
	if ( m_TrimWhiteSpace && !m_DistortGraphic )
		TrimBaseGraphicWhiteSpace( delta );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcBaseGraphicScale( )
//
// Description:		 
//
// Returns:				Boolean indicating sucessful completion
//
// Exceptions:			Memory
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::CalcBaseGraphicScale( const RRealSize& size ) 
{
RIntSize 	delta;
BOOLEAN 		fHorizontal = IsHorizontal();
BOOLEAN		fDefined;

	//	If the size of the graphic has changed and we are in a nondistorted mode
	//		reset the bounding size and reconstruct the graphic.
	//	else if the graphic is defined and we are in a distorted mode
	//		we don't have to do anything
	if ( m_BoundingSize.m_dx != ::Round( size.m_dx ) || m_BoundingSize.m_dy != ::Round( size.m_dy ) )
		Redefine();
	m_BoundingSize.m_dx = ::Round( size.m_dx );
	m_BoundingSize.m_dy = ::Round( size.m_dy );
	m_ConstructedSize = size;
	if ( m_GraphicDefined )
		return FALSE;
	// compute the rect to contain the base graphic outline
	if ( size.m_dx <= ( kDefaultMargin * 2 ) || size.m_dy <= ( kDefaultMargin * 2 ) )
		return FALSE;
	delta.m_dx = delta.m_dy = kDefaultMargin;
	m_OutlineRect = RIntRect( m_BoundingSize );	 
	m_OutlineRect.Inset( delta );
	if ( m_OutlineRect.IsEmpty() )
	{
		m_OutlineRect = RIntRect( m_BoundingSize );	 
		return FALSE;
	}
	// get base bounding boxes for each line of headline
	ParseTextLines( m_OriginalText );
	if ( m_BaseStringBoundingBoxes.Count() == 0 )
	{
		RIntRect	rcString; 
		for ( RMBCStringArrayIterator sz = m_LineTextList.Start(); sz != m_LineTextList.End(); ++sz )
		{
			rcString = GetStringBounds( *(*sz) );
			m_BaseStringBoundingBoxes.InsertAtEnd( rcString );	
		}
		m_pFont->GetMaxCharBounds( &rcString );
		m_BaseStringBoundingBoxes.InsertAtEnd( rcString );			// can index 1 past the end of list
	}
	// calc the scale transforms and start points per line
	if ( m_LineScaleTransforms.Count() == 0 )
	{
		if ( m_DistortEffect == kFollowPath )
		{
			fDefined = CalcLineScaleXformsViaPath( fHorizontal );
			*(m_BaseStringBoundingBoxes.Start()) = GetStringBounds( *(*m_LineTextList.Start()) );	
		}
		else
			fDefined = CalcLineScaleXforms( fHorizontal );
		if ( fDefined )
		{
			if ( fHorizontal )
				CalcHorzLineStartPoints();
			else
				CalcVertLineStartPoints();
			TpsAssert( ( IsHorizontal() || IsVertical() ), "Bad escapement." );
		}
		else
		{
			m_LineScaleTransforms.Empty();
		}
	}
	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::PrepGraphicForConstruct( )
//
// Description:		Prepare the graphic for contruction of the headline.
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::PrepGraphicForConstruct( )
{
R2dTransform			transform;
RGradientRampPoint	rampPoints[4];

	TpsAssert( ( m_pFont != NULL ), "Undefined font." );
	m_GraphicDefined = FALSE;
	RSingleGraphic::Undefine( RSolidColor( kBlack), RSolidColor( kBlack) );	// sets a default line width and colors as well
	m_BoundingBox = RRealRect( 0.0, 0.0, 0.0, 0.0 );
	m_fMonochrome = IsMonochrome();
	m_ProjectionTransforms.Empty();
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::AdjustBaseGraphicAfterConstruct( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::AdjustBaseGraphicAfterConstruct( RIntSize whiteSpace )
{
RIntSize							delta( 0, 0 );
RRealPointArrayIterator		oPtz;
R2dTransform					xform;
YRealDimension					xScale, yScale;
RIntRect							rcBoundingBox;

	// recompute the rect to contain the base graphic outline
	m_GraphicSize = m_BoundingSize;
	m_OutlineRect = RIntRect( m_BoundingSize );	 
	m_OutlineRect.Inset( whiteSpace );

	if ( m_OutlineRect.IsEmpty() )
	{
		if ( m_OutlineRect.m_Top >= m_OutlineRect.m_Bottom )
			delta.m_dy = ( m_BoundingSize.m_dy - ::PrinterPointToLogicalUnit( 1 ) ) / 2;
		else
			delta.m_dy = whiteSpace.m_dy;
		if ( m_OutlineRect.m_Left >= m_OutlineRect.m_Right )
			delta.m_dx =  ( m_BoundingSize.m_dx - ::PrinterPointToLogicalUnit( 1 ) ) / 2;
		else
			delta.m_dx = whiteSpace.m_dx;
		m_OutlineRect = RIntRect( m_BoundingSize );	 
		m_OutlineRect.Inset( delta );
	}
	TpsAssert( !m_OutlineRect.IsEmpty(), "Bad bounding box." );
	// calc the scale factors
	xScale = (YRealDimension)m_OutlineRect.Width() / ( m_BoundingBox.Width() + 1.0 );
	yScale = (YRealDimension)m_OutlineRect.Height() / ( m_BoundingBox.Height() + 1.0 );
	if ( kWarpToEnvelop == m_DistortEffect )
	{
//		TpsAssert( ( xScale > 1.0 ), "Warp envelop should not be bigger than bounding rect." );
//		TpsAssert( ( yScale > 1.0 ), "Warp envelop should not be bigger than bounding rect." );
		xScale = yScale = 1.0;
	}
	else if ( !m_DistortGraphic )
		xScale = yScale = ::Min( xScale, yScale );
	xform.MakeIdentity();
	xform.PreScaleAboutPoint( m_BoundingBox.m_Left, m_BoundingBox.m_Top, xScale, yScale );
	rcBoundingBox = m_BoundingBox * xform;
	// compute white space and delta to adjust graphic size to 
	// account for white space at top/left and bottom/right
	if ( m_GraphicDefined )
		switch ( m_DistortEffect )
		{
			case kNonDistort :
			case kAltBaseline :
			case kShear :
			case kAltShear :
			case kAltShearRotate :
				if ( m_DistortGraphic )
				{
					delta.m_dx = -( rcBoundingBox.m_Left - whiteSpace.m_dx );
					delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
				}
				else if ( IsHorizontal() )
				{
					switch ( m_Justification )
					{
						case kLeftJust :
							delta.m_dx = -( rcBoundingBox.m_Left - whiteSpace.m_dx );
							break;
						case kRightJust :
							whiteSpace.m_dx = ::Round( m_GraphicSize.m_dx ) - rcBoundingBox.Width() - whiteSpace.m_dx;
							delta.m_dx = -( rcBoundingBox.m_Left - whiteSpace.m_dx );
							break;
						default :
							whiteSpace.m_dx = ( ( ::Round( m_GraphicSize.m_dx ) - rcBoundingBox.Width() ) / 2 );
							delta.m_dx = -( rcBoundingBox.m_Left - whiteSpace.m_dx );
							break;
					}
					whiteSpace.m_dy = ( ( ::Round( m_GraphicSize.m_dy ) - rcBoundingBox.Height() ) / 2 );
					delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
				}
				else
				{
					switch ( m_Justification )
					{
						case kLeftJust :
							delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
							break;
						case kRightJust :
							whiteSpace.m_dy = ::Round( m_GraphicSize.m_dy ) - rcBoundingBox.Height() - whiteSpace.m_dy;
							delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
							break;
						default :
							whiteSpace.m_dy = ( ( ::Round( m_GraphicSize.m_dy ) - rcBoundingBox.Height() ) / 2 );
							delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
							break;
					}
					whiteSpace.m_dx = ( ( ::Round( m_GraphicSize.m_dx ) - rcBoundingBox.Width() ) / 2 );
					delta.m_dx = -( rcBoundingBox.m_Left - whiteSpace.m_dx );
				}
				break;
			case kFollowPath :
				// remember, graphics are flipped on output
				switch ( m_Verticality )
				{
					case kPin2Bottom :
						delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
						break;
					case kPin2Top :
						whiteSpace.m_dy = ::Round( m_GraphicSize.m_dy ) - rcBoundingBox.Height() - whiteSpace.m_dy;
						delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
						break;
					default :
						whiteSpace.m_dy = ( ( ::Round( m_GraphicSize.m_dy ) - rcBoundingBox.Height() ) / 2 );
						delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
						break;
				}
				whiteSpace.m_dx = ( ( ::Round( m_GraphicSize.m_dx ) - rcBoundingBox.Width() ) / 2 );
				delta.m_dx = -( rcBoundingBox.m_Left - whiteSpace.m_dx );
				break;
			case kWarpToEnvelop :
				delta.m_dx = delta.m_dy = 0;							// mostly in the right place
#if 0
				switch ( m_Verticality )
				{
					case kPin2Left :
						whiteSpace.m_dx = ::Round( m_GraphicSize.m_dx ) - rcBoundingBox.Width() - whiteSpace.m_dx;
						delta.m_dx = -( rcBoundingBox.m_Left - whiteSpace.m_dx );
				//		whiteSpace.m_dy = ( ( ::Round( m_GraphicSize.m_dy ) - rcBoundingBox.Height() ) / 2 );
				//		delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
						break;
					case kPin2Right :
						delta.m_dx = -( rcBoundingBox.m_Left - whiteSpace.m_dx );
				//		whiteSpace.m_dy = ( ( ::Round( m_GraphicSize.m_dy ) - rcBoundingBox.Height() ) / 2 );
				//		delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
						break;
					case kPin2Top :
						delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
				//		whiteSpace.m_dx = ( ( ::Round( m_GraphicSize.m_dx ) - rcBoundingBox.Width() ) / 2 );
				//		delta.m_dx = -( rcBoundingBox.m_Left - whiteSpace.m_dx );
						break;
					case kPin2Bottom :
						whiteSpace.m_dy = ::Round( m_GraphicSize.m_dy ) - rcBoundingBox.Height() - whiteSpace.m_dy;
						delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
				//		whiteSpace.m_dx = ( ( ::Round( m_GraphicSize.m_dx ) - rcBoundingBox.Width() ) / 2 );
				//		delta.m_dx = -( rcBoundingBox.m_Left - whiteSpace.m_dx );
						break;
					default :
				//		whiteSpace.m_dy = ( ( ::Round( m_GraphicSize.m_dy ) - rcBoundingBox.Height() ) / 2 );
				//		delta.m_dy = -( rcBoundingBox.m_Top - whiteSpace.m_dy );
				//		whiteSpace.m_dx = ( ( ::Round( m_GraphicSize.m_dx ) - rcBoundingBox.Width() ) / 2 );
				//		delta.m_dx = -( rcBoundingBox.m_Left - whiteSpace.m_dx );
						break;
				}
#endif
				break;
			default :
				TpsAssertAlways( "Unrecognized distort effect." );
				break;
		}
	// update the line scale transforms which could be used later for resize feedback
	R2dTransformArrayIterator	tz = m_LineScaleTransforms.Start(); 
	for ( int k = 0; k < m_NrLines; k++ )
	{
		*tz *= xform;
		++tz;
		m_LineStartPoints[k] *= xform;
		m_LineStartPoints[k].m_x += delta.m_dx;
		m_LineStartPoints[k].m_y += delta.m_dy;
	}
	// this should scale too 
	// but that screws up the line weights and margins
//	if ( m_ScaleConstraint == kByVertical )
//		if ( yScale < 1.0 )
//			m_EmSquareTransform.PostScale( xScale, yScale );
	// adjust graphic coordinates to equalize white space
	if ( delta.m_dx != 0 || delta.m_dy != 0 || xScale != 1.0 || yScale != 1.0 )
	{
		xform.PostTranslate( delta.m_dx, delta.m_dy );
		oPtz = m_OriginalPointsStream.Start();
		while ( oPtz != m_OriginalPointsStream.End() )
		{
			*oPtz = *oPtz * xform;
			++oPtz;
		}
		m_BoundingBox *= xform;
	}
	m_TransformedIntPointsStream.Empty(); 
	m_TransformedRealPointsStream.Empty(); 
//NOTE: ApplyTransform() will copy the points	::Append( m_TransformedIntPointsStream, m_OriginalPointsStream.Start(), m_OriginalPointsStream.End() );
 	m_TransformedRealPointsStream = m_OriginalPointsStream;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::AdjustBaseGraphicForWarp( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::AdjustBaseGraphicForWarp( BOOLEAN fHorizontal, RIntSize whiteSpace, RIntSize margins )
{
RIntSize							delta;
RRealRect						rcBoundingBox( m_BoundingBox );
RRealRect						rcGraphicRect, rcNewGraphicRect, rcNew2GraphicRect;
YRealDimension					xScale, yScale;
R2dTransform 					xformTop, xformBottom;
RPath								pathTop, pathBottom;
RRealPoint						pt0, pt1;
RRealRect						rcTopPath, rcBottomPath;
YRealDimension					rLength, diff;
YRealDimension					thickness, thickest, widest;
YRealDimension					height, width;
YRealCoordinate 				xCoord, yCoord;
RRealPointArrayIterator		oPtz;
R2dTransform					xform;
BOOLEAN							fCrossed, fTopCurves, fBottomCurves;

	// initializations/validations
	m_GraphicSize = m_BoundingSize;
	if ( !m_GraphicDefined )
		return;
	// compute bounding box which includes control points
	oPtz = m_OriginalPointsStream.Start();
	while ( oPtz != m_OriginalPointsStream.End() )
	{
		rcBoundingBox.AddPointToRect( *oPtz );
		++oPtz;
	}
	// compute the rect to contain the base graphic outline
	whiteSpace = margins;//CalcMarginBasedOnEffects( kHairlineDoubleOutline, kMediumLine );
	if ( fHorizontal )
	{
		whiteSpace.m_dx = ::Max( whiteSpace.m_dx, margins.m_dx );
		whiteSpace.m_dy = 0;
	}
	else
	{
		whiteSpace.m_dx = 0;
		whiteSpace.m_dy = ::Max( whiteSpace.m_dy, margins.m_dy );
	}
	rcGraphicRect = RRealRect( RRealSize( m_BoundingSize ) );	 
	rcGraphicRect.Inset( RRealSize( whiteSpace ) );
	if ( rcGraphicRect.IsEmpty() )
	{
		rcGraphicRect = RRealRect( RRealSize( m_BoundingSize ) );
		TpsAssertAlways( "Margins too large to warp graphic." );
	}
	// create envelop paths of correct size and position
	rcTopPath = m_pPathTop->GetBoundingRect();
	rcBottomPath = m_pPathBottom->GetBoundingRect();
	fTopCurves = ( m_pPathTop->UsesBeziers() || m_pPathTop->UsesSplines() );
	fBottomCurves = ( m_pPathBottom->UsesBeziers() || m_pPathBottom->UsesSplines() );
	xformTop = CalcWarpPathScaleXform( IsHorizontal(), kPathTop, rcTopPath, rcBottomPath, m_BoundingSize, fTopCurves, fBottomCurves );
	xformBottom = CalcWarpPathScaleXform( IsHorizontal(), kPathBottom, rcTopPath, rcBottomPath, m_BoundingSize, fTopCurves, fBottomCurves );
	pathTop = RPath( *m_pPathTop, xformTop );
	pathBottom = RPath( *m_pPathBottom, xformBottom );
	// check warp paths for crossings
	rLength = thickest = widest = 0.0;
	pt0 = pathTop.GetStartPoint(); 
	pt1 = pathBottom.GetStartPoint(); 
	xCoord = ::Max( pt0.m_x, pt1.m_x );
	yCoord = ::Max( pt0.m_y, pt1.m_y );
	rcNewGraphicRect = rcGraphicRect;	 
	if ( fHorizontal )
	{
		rLength = widest = xCoord / m_BoundingSize.m_dx;
		thickness = ( margins.m_dy * 2 ) + 10;
		pt0 = pathTop.PointAtX( xCoord ); 
		pt1 = pathBottom.PointAtX( xCoord ); 
		diff = thickest = ( pt1.m_y - pt0.m_y );
		fCrossed = ( diff < thickness );
		pt0 = pathTop.GetEndPoint(); 
		pt1 = pathBottom.GetEndPoint(); 
		xCoord = ::Min( pt0.m_x, pt1.m_x );
		yCoord = ::Min( pt0.m_y, pt1.m_y );
		while ( ( m_BoundingSize.m_dx * rLength ) <= xCoord )
		{
			pt0 = pathTop.PointAtX( static_cast<YRealCoordinate>( m_BoundingSize.m_dx * rLength ) ); 
			pt1 = pathBottom.PointAtX( static_cast<YRealCoordinate>( m_BoundingSize.m_dx * rLength ) ); 
			diff = ( pt1.m_y - pt0.m_y );
			if ( diff < thickness )
			{
				if ( !fCrossed )
					rcNewGraphicRect.m_Right = m_BoundingSize.m_dx * ( rLength - 0.01 );
				fCrossed = TRUE;
			}
			else if ( fCrossed )
			{
				rcNewGraphicRect.m_Left = m_BoundingSize.m_dx * rLength;
				rcNewGraphicRect.m_Right = rcGraphicRect.m_Right;
				fCrossed = FALSE;
			}
			if ( diff > thickest )
			{
				thickest = diff;
				widest = rLength;
			}
			TpsAssert( ( rLength <= 1.0 ), "This has gone too far." );
			rLength += 0.01;
		}
	}
	else
	{
		rLength = widest = yCoord / m_BoundingSize.m_dy;
		thickness = ( margins.m_dx * 2 ) + 10;
		pt0 = pathTop.PointAtY( yCoord ); 
		pt1 = pathBottom.PointAtY( yCoord ); 
		diff = thickest = ( pt1.m_x - pt0.m_x );
		fCrossed = ( diff < thickness );
		pt0 = pathTop.GetEndPoint(); 
		pt1 = pathBottom.GetEndPoint(); 
		xCoord = ::Min( pt0.m_x, pt1.m_x );
		yCoord = ::Min( pt0.m_y, pt1.m_y );
		while ( ( m_BoundingSize.m_dy * rLength ) <= yCoord )
		{
			pt0 = pathTop.PointAtY( static_cast<YRealCoordinate>( m_BoundingSize.m_dy * rLength ) ); 
			pt1 = pathBottom.PointAtY( static_cast<YRealCoordinate>( m_BoundingSize.m_dy * rLength ) ); 
			diff = ( pt1.m_x - pt0.m_x );
			if ( diff < thickness )
			{
				if ( !fCrossed )
					rcNewGraphicRect.m_Bottom = m_BoundingSize.m_dy * ( rLength - 0.01 );
				fCrossed = TRUE;
			}
			else if ( fCrossed )
			{
				rcNewGraphicRect.m_Top = m_BoundingSize.m_dy * rLength;
				rcNewGraphicRect.m_Bottom = rcGraphicRect.m_Bottom;
				fCrossed = FALSE;
			}
			if ( diff > thickest )
			{
				thickest = diff;
				widest = rLength;
			}
			TpsAssert( ( rLength <= 1.0 ), "This has gone too far." );
			rLength += 0.01;
		}
	}
	if ( fCrossed )
		if ( rcNewGraphicRect == rcGraphicRect )
			rcNewGraphicRect = RRealRect( 0, 0, 0, 0 );
	// calc min margins based on outline 
	rcNew2GraphicRect = rcGraphicRect;	 
	if ( m_OutlineEffect == kSimpleFill || ( !fTopCurves && !fBottomCurves ) || 
		( m_LineWeight == kHairLine && m_OutlineEffect != kBlurredOutline ) )
	{
		if ( fHorizontal )
		{
			rcNew2GraphicRect.m_Left += kDefaultMargin;
			rcNew2GraphicRect.m_Right -= kDefaultMargin;
		}
		else
		{
			rcNew2GraphicRect.m_Top += kDefaultMargin;
			rcNew2GraphicRect.m_Bottom -= kDefaultMargin;
		}
	}	 
	else
	{
		height = rcGraphicRect.Height();
		width = rcGraphicRect.Width();
	 	pt0 = pathTop.GetStartPoint(); 
	 	pt1 = pathBottom.GetStartPoint(); 
		if ( fHorizontal )
		{
			diff = ::Abs( pt0.m_y - pt1.m_y );
			if ( diff < ( height * 0.9 ) )
				if ( diff > ( height / 2.0 ) )
					rcNew2GraphicRect.m_Left += kDefaultMargin;
				else if ( diff > ( height / 4.0 ) )
					rcNew2GraphicRect.m_Left += margins.m_dx / 2;
				else 
					rcNew2GraphicRect.m_Left += margins.m_dx;
	//		TpsAssert( ( diff > ( height / 8.0 ) ), "Warp paths set too close." );
		}
		else
		{
			diff = ::Abs( pt0.m_x - pt1.m_x );
			if ( diff < ( width * 0.9 ) )
				if ( diff > ( width / 2.0 ) )
					rcNew2GraphicRect.m_Top += kDefaultMargin;
				else if ( diff > ( width / 4.0 ) )
					rcNew2GraphicRect.m_Top += margins.m_dy / 2;
				else 
					rcNew2GraphicRect.m_Top += margins.m_dy;
//			TpsAssert( ( diff > ( width / 8.0 ) ), "Warp paths set too close." );
		}
	 	pt0 = pathTop.GetEndPoint(); 
	 	pt1 = pathBottom.GetEndPoint(); 
		if ( fHorizontal )
		{
			diff = ::Abs( pt0.m_y - pt1.m_y );
			if ( diff < ( height * 0.9 ) )
				if ( diff > ( height / 2.0 ) )
					rcNew2GraphicRect.m_Right -= kDefaultMargin;
				else if ( diff > ( height / 4.0 ) )
					rcNew2GraphicRect.m_Right -= margins.m_dx / 2;
				else 
					rcNew2GraphicRect.m_Right -= margins.m_dx;
//			TpsAssert( ( diff > ( height / 8.0 ) ), "Warp paths set too close." );
		}
		else
		{
			diff = ::Abs( pt0.m_x - pt1.m_x );
			if ( diff < ( width * 0.9 ) )
				if ( diff > ( width / 2.0 ) )
					rcNew2GraphicRect.m_Bottom -= kDefaultMargin;
				else if ( diff > ( width / 4.0 ) )
					rcNew2GraphicRect.m_Bottom -= margins.m_dy / 2;
				else 
					rcNew2GraphicRect.m_Bottom -= margins.m_dy;
//			TpsAssert( ( diff > ( width / 8.0 ) ), "Warp paths set too close." );
		}
	}
	// adjust graphic bound box to account for path crossings and outline margins
	if ( rcNewGraphicRect.IsEmpty() )
	{
		if ( fHorizontal )
		{
			pt0 = pathTop.PointAtX( static_cast<YRealCoordinate>( m_BoundingSize.m_dx * widest ) ); 
			pt1 = pathBottom.PointAtX( static_cast<YRealCoordinate>( m_BoundingSize.m_dx * widest ) ); 
		}
		else
		{
			pt0 = pathTop.PointAtY( static_cast<YRealCoordinate>( m_BoundingSize.m_dy * widest ) ); 
			pt1 = pathBottom.PointAtY( static_cast<YRealCoordinate>( m_BoundingSize.m_dy * widest ) ); 
		}
		rcGraphicRect = CalcMinRectInRect( pt0, pt1, rcGraphicRect, RRealSize( 16, 16 ) );
	}
	else
	 	rcNewGraphicRect.Intersect( rcGraphicRect, rcNewGraphicRect );
	if ( rcNewGraphicRect.IsEmpty() )
	{
		if ( fHorizontal )
		{
			pt0 = pathTop.PointAtX( static_cast<YRealCoordinate>( m_BoundingSize.m_dx * widest ) ); 
			pt1 = pathBottom.PointAtX( static_cast<YRealCoordinate>( m_BoundingSize.m_dx * widest ) ); 
		}
		else
		{
			pt0 = pathTop.PointAtY( static_cast<YRealCoordinate>( m_BoundingSize.m_dy * widest ) ); 
			pt1 = pathBottom.PointAtY( static_cast<YRealCoordinate>( m_BoundingSize.m_dy * widest ) ); 
		}
		rcGraphicRect = CalcMinRectInRect( pt0, pt1, rcGraphicRect, RRealSize( 16, 16 ) );
	}
	else
		rcGraphicRect = rcNewGraphicRect;
	rcNew2GraphicRect.Intersect( rcGraphicRect, rcNew2GraphicRect );
	if ( !rcNew2GraphicRect.IsEmpty() )
	 	rcGraphicRect = rcNew2GraphicRect;
	TpsAssert( !rcGraphicRect.IsEmpty(), "Margins too large to warp graphic." );
	whiteSpace.m_dx = rcGraphicRect.m_Left;
	whiteSpace.m_dy = rcGraphicRect.m_Top;
	// calc the scale factors
	if ( rcGraphicRect.IsEmpty() )
		xScale = yScale = 1.0;
	else
	{
		xScale = rcGraphicRect.Width() / rcBoundingBox.Width();
		yScale = rcGraphicRect.Height() / rcBoundingBox.Height();
	}
	xform.PreScaleAboutPoint( rcBoundingBox.m_Left, rcBoundingBox.m_Top, xScale, yScale );
	rcBoundingBox = rcBoundingBox * xform;
	// compute white space translation
	if ( fHorizontal )
	{
		delta.m_dx = ( whiteSpace.m_dx - rcBoundingBox.m_Left );
		delta.m_dy = -rcBoundingBox.m_Top;
	}
	else
	{
		delta.m_dx = -rcBoundingBox.m_Left;
		delta.m_dy = ( whiteSpace.m_dy - rcBoundingBox.m_Top );
	}
	// adjust graphic coordinates to equalize white space
	xform.PostTranslate( delta.m_dx, delta.m_dy );
	oPtz = m_OriginalPointsStream.Start();
	while ( oPtz != m_OriginalPointsStream.End() )
	{
		*oPtz *= xform;
		++oPtz;
	}
	m_TransformedIntPointsStream.Empty(); 
	m_TransformedRealPointsStream.Empty(); 
//NOTE ApplyTransform() will copy the points	::Append( m_TransformedIntPointsStream, m_OriginalPointsStream.Start(), m_OriginalPointsStream.End() );
	m_TransformedRealPointsStream = m_OriginalPointsStream;
	// adjust graphic size to account for white space on bottom and right
	m_BoundingBox *= xform;
	m_GraphicSize = m_BoundingSize;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::TrimBaseGraphicWhiteSpace( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::TrimBaseGraphicWhiteSpace( RIntSize whiteSpace )
{
RRealSize						delta( 0, 0 );
BOOLEAN							fHorizontal = IsHorizontal();
RRealPointArrayIterator		oPtz;
R2dTransform					transform;
RIntRect							rcBoundingBox;

	// validations
	if ( !m_GraphicDefined )
		return;
	// initializations
	rcBoundingBox = CalcBoundingBoxWithEffects( transform );
	if ( !fHorizontal || m_DistortGraphic )
		delta.m_dx = ( whiteSpace.m_dx - rcBoundingBox.m_Left );
	if ( fHorizontal || m_DistortGraphic )
		delta.m_dy = ( whiteSpace.m_dy - rcBoundingBox.m_Top );
	// adjust graphic coordinates to equalize white space
	transform.PreTranslate( delta.m_dx, delta.m_dy );
	oPtz = m_OriginalPointsStream.Start();
	while ( oPtz != m_OriginalPointsStream.End() )
	{
		*oPtz = *oPtz + delta;
		++oPtz;
	}
	m_BoundingBox *= transform;
	m_TransformedIntPointsStream.Empty(); 
	m_TransformedRealPointsStream.Empty(); 
//NOTE: ApplyTransform() will copy the points	::Append( m_TransformedIntPointsStream, m_OriginalPointsStream.Start(), m_OriginalPointsStream.End() );
	m_TransformedRealPointsStream = m_OriginalPointsStream;
	// adjust graphic size to account for white space on bottom and right
	rcBoundingBox *= transform;
	if ( !fHorizontal || m_DistortGraphic )
		m_GraphicSize.m_dx = rcBoundingBox.m_Right + whiteSpace.m_dx;
	if ( fHorizontal || m_DistortGraphic )
		m_GraphicSize.m_dy = rcBoundingBox.m_Bottom + whiteSpace.m_dy;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ConstructBaseGraphic( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::ConstructBaseGraphic( BOOLEAN /*fHorizontal*/ )
{
RMBCStringArrayIterator		sz; 
R2dTransformArrayIterator	tz; 
RMBCStringIterator 			cz;
RIntPointArrayIterator		pz;
RIntRectArrayIterator		rz; 
RMBCString  					mbString;
R2dTransform					xformTranslate, xformScale;
RCharacter						character;
RIntSize							whiteSpace( 0, 0 );
RPath*							pGlyph = NULL;
RIntRect							rcBounds;
int								k;
RIntPoint						*p, points[16];

	// validations/initializations
	TpsAssert( ( m_pFont != NULL ), "Undefined font." );
	if ( !m_TextDefined )
		return;
	m_GraphicSize.m_dx = m_GraphicSize.m_dy = 0;
	m_BoundingBox = RRealRect( 0.0, 0.0, 0.0, 0.0 );
	// add marker points to the beginning of the graphic
	CalcDefaultMarkerPoints( points );
	p = points;
	xformTranslate.MakeIdentity();
	try
	{
		pGlyph = new RPath;
		for ( k = 0; k < kNrExtremeMarkerPoints; k++ )
		{
			rcBounds.m_TopLeft = *p;	
			rcBounds.m_BottomRight = *p;	
			(void)pGlyph->Define( rcBounds );
			rcBounds = pGlyph->MergeWith( xformTranslate, *this );
			pGlyph->Undefine();
			p++;
		}
	}
	catch ( ... )
	{
		delete pGlyph;
		throw;
	}
	delete pGlyph;
	pGlyph = NULL;
	// loop thru lines in headline
	sz = m_LineTextList.Start(); 
	tz = m_LineScaleTransforms.Start(); 
	rz = m_BaseStringBoundingBoxes.Start(); 
	for ( k = 0; k < m_NrLines; k++ )
	{
		// calc start points of each character along line
		mbString = *(*sz);
		CalcCharStartPoints( mbString, *rz, *tz, m_LineStartPoints[k], IsHorizontal() );
		m_BaselineDirection = m_ShearDirection = 1;
		// loop thru characters in line
		cz = mbString.Start(); 
		pz = m_TempPoints.Start(); 
		while ( cz != mbString.End() && pz != m_TempPoints.End() )
		{
			character = ( ( cz == mbString.End() ) ? kSpace : *cz );
			xformScale = CalcCharScaleXform( character, *tz, 0.0 );
			if ( ( pGlyph = GetScaledGlyph( character, xformScale, TRUE ) ) != NULL )
			{
				xformTranslate.MakeIdentity();
				xformTranslate.PreTranslate( (*pz).m_x, (*pz).m_y );
				try
				{
					rcBounds = pGlyph->MergeWith( xformTranslate, *this );
				}
				catch ( ... )
				{
					delete pGlyph;
					throw;
				}
				if ( m_BoundingBox.IsEmpty() )
					m_BoundingBox = rcBounds;
				else
					m_BoundingBox.Union( m_BoundingBox, RRealRect( rcBounds ) );
				m_GraphicDefined = TRUE;
				delete pGlyph;
				pGlyph = NULL;
			}
			++cz;
			++pz;
		}
		++sz;
		++tz;
		++rz;
		// tote up white space to bottom right of line 
		// m_WhiteSpace is calc in CalcCharDisplacement()
		if ( m_WhiteSpace.m_dx > whiteSpace.m_dx ) 
			whiteSpace.m_dx = m_WhiteSpace.m_dx;
		if ( m_WhiteSpace.m_dy > whiteSpace.m_dy )
			whiteSpace.m_dy = m_WhiteSpace.m_dy;
	}
	// prep for post construct adjustments
	m_WhiteSpace = AdjustWhiteSpaceBasedOnEffects( whiteSpace );
	// recalc graphic bounding box and size
	m_BoundingBox = RRealRect( 0.0, 0.0, 0.0, 0.0 );
	YPointCountArrayIterator	cntz = m_PointCountsStream.Start();
	EPathOperatorArrayIterator	opz = m_PathOperatorsStream.Start();
	RRealPointArrayIterator	ptz = m_OriginalPointsStream.Start();
	while ( cntz != m_PointCountsStream.End() )
	{
		rcBounds = RPath::CalcBoundingRect( &(*opz), &(*ptz) );
		opz += RPath::GetOperatorCount( &(*opz) );
		ptz += *cntz;
		++cntz;
		if ( m_BoundingBox.IsEmpty() )
			m_BoundingBox = rcBounds;
		else
			m_BoundingBox.Union( m_BoundingBox, RRealRect( rcBounds ) );
	}
	m_GraphicSize.m_dx = ::Max( m_GraphicSize.m_dx, m_BoundingBox.m_Right );
	m_GraphicSize.m_dy = ::Max( m_GraphicSize.m_dy, m_BoundingBox.m_Bottom );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ConstructBaseGraphicViaPath( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::ConstructBaseGraphicViaPath( )
{
R2dTransformArrayIterator	tz = m_LineScaleTransforms.Start(); 
YAngleArrayIterator			az;
RIntPointArrayIterator		pz;
RMBCStringIterator 			cz;
RMBCString  					mbString;
R2dTransform					xformScale, xformTranslate;
RCharacter						character;
RPath*							pGlyph = NULL;
RPath								follow;
RRealRect						rcBounds;
int								i;

	// validations/initializations
	TpsAssert( ( m_pFont != NULL ), "Undefined font." );
	TpsAssert( ( m_NrLines == 1 ), "Bad number of lines." );
	TpsAssert( ( m_pPathBottom != NULL ), "Bad follow path." );
	TpsAssertValid( m_pPathBottom );
	if ( !m_TextDefined )
		return;
	m_GraphicSize.m_dx = m_GraphicSize.m_dy = 0;
	m_BoundingBox = RRealRect( 0.0, 0.0, 0.0, 0.0 );
	// calc start points of each character along path of correct size and position
	mbString = *(*m_LineTextList.Start());
	xformScale = CalcFollowPathScaleXform( m_pPathBottom->GetBoundingRect(), m_OutlineRect );
	follow = RPath( *m_pPathBottom, xformScale );
	CalcCharStartPointsViaPath( mbString, 
										 *(m_BaseStringBoundingBoxes.Start()), 
										 *(m_LineScaleTransforms.Start()), 
										 m_LineStartPoints[0], 
										 IsHorizontal(), 
										 follow, 
										 1.0 );
	// add extreme path points to bounding rect
#ifdef	TPSDEBUG
	#define  FOLLOW_TEST_CODE		 0
#endif	
	pz = m_TempPoints.End(); 
	xformTranslate.MakeIdentity();
	try
	{
		--pz;
		pGlyph = new RPath;
		for ( i = 0; i < kNrPathMarkerPoints; i++ )
		{
			rcBounds.m_TopLeft = RRealPoint( *pz );
			rcBounds.m_BottomRight = RRealPoint( *pz );
#if	FOLLOW_TEST_CODE
			if ( m_ProjectionEffect != kSolidExtrusion && 					// we don't want to extrude marker points
				  m_ProjectionEffect != kIlluminateExtrusion && 
				  m_ProjectionEffect != kPseudoPerspectiveExtrusion ) 
			{
				rcBounds.Inflate( RIntSize( 10, 10 ) );
#ifdef	IMAGER
				if ( i < 6 )
					rcBounds.Inflate( RIntSize( 10, 10 ) );
#endif
			}
#endif
			(void)pGlyph->Define( rcBounds );
			rcBounds = pGlyph->MergeWith( xformTranslate, *this );
			pGlyph->Undefine();
			--pz;
		}
	}
	catch ( ... )
	{
		delete pGlyph;
		throw;
	}
	delete pGlyph;
	pGlyph = NULL;
	// loop thru characters in line, process only one line of headline
	m_BaselineDirection = m_ShearDirection = 1;
	cz = mbString.Start(); 
	pz = m_TempPoints.Start(); 
	az = m_TempAngles.Start(); 
	while ( cz != mbString.End() && pz != m_TempPoints.End() && az != m_TempAngles.End() )
	{
		character = ( ( cz == mbString.End() ) ? kSpace : *cz );
		xformScale = CalcCharScaleXform( character, *tz, -(*az) );
		if ( ( pGlyph = GetScaledGlyph( character, xformScale, TRUE ) ) != NULL )
		{
			rcBounds = pGlyph->GetBoundingRect();
			xformTranslate.MakeIdentity();
			xformTranslate.PostTranslate( (*pz).m_x, (*pz).m_y );
			try
			{
				rcBounds = pGlyph->MergeWith( xformTranslate, *this );
			}
			catch ( ... )
			{
				delete pGlyph;
				throw;
			}
			if ( m_BoundingBox.IsEmpty() )
				m_BoundingBox = rcBounds;
			else
				m_BoundingBox.Union( m_BoundingBox, rcBounds );
			m_GraphicDefined = TRUE;
			delete pGlyph;
			pGlyph = NULL;
		}
		++cz;
		++pz;
		++az;
	}
	// add extreme path points to bounding rect
	++pz;
	while ( pz != m_TempPoints.End() )
	{
		m_BoundingBox.AddPointToRect( RRealPoint( *pz ) );
		++pz;
	}
	// prep for post construct adjustments 
	// m_WhiteSpace is calc in CalcCharDisplacement()
	m_WhiteSpace = AdjustWhiteSpaceBasedOnEffects( m_WhiteSpace );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetEmHeight( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
YRealDimension RHeadlineGraphic::GetEmHeight() const
{
	YRealDimension result( 0.0 );

/*	R2dTransformArrayIterator tz = m_LineScaleTransforms.Start();
	R2dTransform xformScale;
	YRealDimension largest(0);

	for (int k = 0; k < 3; k++)
	{
		if (m_LineScaleProportions[k] > largest)
		{
			largest = m_LineScaleProportions[k];
			xformScale = m_LineScaleTransforms[k];
		}
	}
*/
//	RCharacter character( 'I' );
//	R2dTransform transform = *m_LineScaleTransforms.Start();
//	RPath* pGlyph = GetScaledGlyph( character, transform, FALSE );

//	if (!m_DistortGraphic)
//		transform = m_EmSquareTransform;
//		pGlyph = GetScaledGlyph( character, m_EmSquareTransform, FALSE );

//	if (pGlyph)
	{
		YFontDimensions& dim = m_pFont->GetDimensions( );
		result = (dim.m_Ascent + dim.m_Descent) * m_EmScale / 2;
//		result = (RRealSize( 0, dim.m_Ascent /*+ dim.m_Descent*/ ) * transform).m_dy;

//		RRealRect rcBounds = pGlyph->GetBoundingRect();
//		result = rcBounds.Height(); // / (dim.m_Ascent / (dim.m_Ascent + dim.m_Descent));

//		delete pGlyph;
	}

	return result;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::WarpBaseGraphic( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::WarpBaseGraphic( RIntSize margins )
{
R2dTransform 						xformTop, xformBottom;
RPath									pathTop, pathBottom;
RRealPoint							pt0;
RRealPointArrayIterator			ptz;
EPathOperatorArrayIterator		opz;
YPointCountArrayIterator 		cntz;
RIntPoint							pts[16];
RRealRect							rcBounds;
RRealRect							rcTopPath, rcBottomPath;
BOOLEAN								fTopCurves, fBottomCurves;

	// validations
	if ( !m_TextDefined || !m_GraphicDefined )
		return;

	TpsAssert( ( m_pPathTop != NULL ), "Bad warp path." );
	TpsAssert( ( m_pPathBottom != NULL ), "Bad warp path." );
	TpsAssertValid( m_pPathTop );
	TpsAssertValid( m_pPathBottom );

	// create envelop paths of correct size and position
	rcTopPath = m_pPathTop->GetBoundingRect();
	rcBottomPath = m_pPathBottom->GetBoundingRect();
	fTopCurves = ( m_pPathTop->UsesBeziers() || m_pPathTop->UsesSplines() );
	fBottomCurves = ( m_pPathBottom->UsesBeziers() || m_pPathBottom->UsesSplines() );
	xformTop = CalcWarpPathScaleXform( IsHorizontal(), kPathTop, rcTopPath, rcBottomPath, m_BoundingSize, fTopCurves, fBottomCurves );
	xformBottom = CalcWarpPathScaleXform( IsHorizontal(), kPathBottom, rcTopPath, rcBottomPath, m_BoundingSize, fTopCurves, fBottomCurves );
	pathTop = RPath( *m_pPathTop, xformTop );
	pathBottom = RPath( *m_pPathBottom, xformBottom );
	// force calc of segment lengths
	if ( pathTop.GetLength() == 0 || pathBottom.GetLength() == 0 )
	{
		TpsAssertAlways( "Bad warp path." );
		return;
	}

	// compute bounding box which includes control points
	RRealPointArrayIterator oPtz = m_OriginalPointsStream.Start();
	RRealRect rcBoundingBox( *oPtz, *oPtz );

	while ( oPtz != m_OriginalPointsStream.End() )
	{
		rcBoundingBox.AddPointToRect( *oPtz );
		++oPtz;
	}
	
	// The bounding box determination probably came
	// from RPath::CalcBoundingRect() which returns
	// a RIntRect, so there is possibly some rounding
	// errors in the representation.  A two font unit
	// fudge factor will remove any of these errors.
//	RRealRect rcBoundingBox( m_BoundingBox ) ;
//	rcBoundingBox.Inflate( RRealSize( 2, 2 ) ) ;

	// do the warp
	switch ( m_Escape )
	{
		case kLeft2RightTop2Bottom :
		case kLeft2RightBottom2Top :
		case kRight2LeftTop2Bottom :
		case kRight2LeftBottom2Top :
//			WarpTopBottomOn( m_OriginalPointsStream, RRealRect( static_cast<RRealSize>( m_GraphicSize ) ), pathTop, pathBottom, static_cast<YRealDimension>( margins.m_dy ) );
//			WarpTopBottomAt( m_OriginalPointsStream, RRealRect( static_cast<RRealSize>( m_GraphicSize ) ), pathTop, pathBottom, static_cast<YRealDimension>( margins.m_dy ) );
			WarpTopBottomAt( m_OriginalPointsStream, rcBoundingBox, pathTop, pathBottom, static_cast<YRealDimension>( margins.m_dy ) );
			break;
		case kTop2BottomLeft2Right :
		case kTop2BottomRight2Left :
		case kBottom2TopLeft2Right :
		case kBottom2TopRight2Left :
//			WarpLeftRightOn( m_OriginalPointsStream, RRealRect( static_cast<RRealSize>( m_GraphicSize ) ), pathTop, pathBottom, static_cast<YRealDimension>( margins.m_dx ) );
//			WarpLeftRightAt( m_OriginalPointsStream, RRealRect( static_cast<RRealSize>( m_GraphicSize ) ), pathTop, pathBottom, static_cast<YRealDimension>( margins.m_dx ) );
			WarpLeftRightAt( m_OriginalPointsStream, rcBoundingBox, pathTop, pathBottom, static_cast<YRealDimension>( margins.m_dx ) );
			break;
		default :
			TpsAssertAlways( "Unrecognized text escapement type." );
			break;
	}

	// test code to show warp paths in graphic
#ifdef	TPSDEBUG
	#define  WARP_PATH_TEST_CODE		 0
#endif	
#if WARP_PATH_TEST_CODE
	pathTop.SetPathSize( m_BoundingSize );
	pathBottom.SetPathSize( m_BoundingSize );
	rcTopPath = pathTop.GetBoundingRect();
	rcBottomPath = pathBottom.GetBoundingRect();
	rcBounds = pathTop.MergeWith( R2dTransform(), *this );
	rcBounds = pathBottom.MergeWith( R2dTransform(), *this );
#endif

/*	
   12/23/97 Lance - This method should not be used
   to calculate the bounding box, as the margins
	have been computed into the graphic and should
	not be removed, which is what would happen if
	the graphic needs scaled (say for the depth effects).

	// recalc graphic bounding box and size
	m_BoundingBox = RRealRect( 0.0, 0.0, 0.0, 0.0 );
	cntz = m_PointCountsStream.Start();
	opz = m_PathOperatorsStream.Start();
	ptz = m_OriginalPointsStream.Start();
	while ( cntz != m_PointCountsStream.End() )
	{
		rcBounds = RPath::CalcBoundingRect( &(*opz), &(*ptz) );
		opz += RPath::GetOperatorCount( &(*opz) );
		ptz += *cntz;
		++cntz;
		if ( m_BoundingBox.IsEmpty() )
			m_BoundingBox = rcBounds;
		else
			m_BoundingBox.Union( m_BoundingBox, rcBounds );
	}
*/
	// Recalc graphic bounding box and size.  Note: Since the margins
	// are computed into the graphic, so we need to use the margin 
	// rect as the bounding rect.  Anything else will cause further 
	// distortion to the graphic that will not match the warp envelope.
	m_BoundingBox = RRealRect( (RRealSize) m_BoundingSize ) ;
	m_BoundingBox.Inset( (RRealSize) margins ) ;

	m_GraphicSize.m_dx = m_BoundingBox.m_Right;
	m_GraphicSize.m_dy = m_BoundingBox.m_Bottom;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcLineScaleXforms()
//
// Description:		Calculate the scale transforms for each line based on the
//							metric data and relative proportions of each line.
//
// Returns:				Boolean indicating headline data is well defined
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::CalcLineScaleXforms( BOOLEAN fHorizontal )
{
YFontDimensions				fontDimensions;
RIntRectArrayIterator		rz; 
RRealRect						rcHeadline, rcString;
RIntRect							rcBounds, rcStrings[ 4 ];
int								k, width, height, proportion, largest;
YRealDimension					xScale, x2Scale, yScale, y2Scale, scale, scales[ 4 ];
R2dTransform 					transform, xform;

	// validations/initializations
	TpsAssert( ( m_NrLines < kMaxHeadlineLines ), "Headline line overflow." );
	m_LineScaleTransforms.Empty();
	m_EmSquareTransform.MakeIdentity();
	if ( !m_TextDefined )
		return FALSE;
	fontDimensions = m_pFont->GetDimensions();
	// get initial bounding box of each line of headline
	rz = m_BaseStringBoundingBoxes.Start(); 
	for ( k = 0; k < m_NrLines; k++ )
	{
		rcBounds = *rz;
		rcBounds.Offset( RIntSize( -rcBounds.m_Left, -rcBounds.m_Top ) );		// zero base rect
		if ( rcBounds.m_Bottom < 0 )
			rcBounds.m_Bottom = -rcBounds.m_Bottom;									// flip it into positive coords
		if ( m_fUseLeading && fHorizontal )
			rcBounds.m_Bottom += fontDimensions.m_Leading;
		if ( k < 3 )
			rcStrings[k] = rcBounds;
		else if ( rcBounds.m_Right > rcStrings[2].m_Right && fHorizontal )
			rcStrings[2].m_Right = rcBounds.m_Right;									// lines 3 thru n will all have the same composite scale
		else if ( rcBounds.m_Bottom > rcStrings[2].m_Bottom && !fHorizontal )
			rcStrings[2].m_Bottom = rcBounds.m_Bottom;								// lines 3 thru n will all have the same composite scale
		++rz;
	}
	// compute total proportions of lines in headline and find the largest
	largest = width = height = 0;
	if ( m_NrLines == 1 )
	{
		proportion = largest = 1;
		width = rcStrings[0].Width();
		height = rcStrings[0].Height();
	}
	else
	{
		for ( k = 0; ( k < 3 && k < m_NrLines ); k++ )
			if ( m_LineScaleProportions[k] > largest )
			{
				largest = m_LineScaleProportions[k];
				width = rcStrings[k].Width();
				height = rcStrings[k].Height();
			}
		proportion = m_LineScaleProportions[0] + m_LineScaleProportions[1] + ( m_LineScaleProportions[2] * ( m_NrLines - 2 ) );
	}
	// compute the relative scale factors of each line to the largest
	scales[0] = scales[1] = scales[2] = scales[3] = 1.0;
	if ( m_NrLines > 1 )
		for ( k = 0; ( k < 3 && k < m_NrLines ); k++ )
			scales[k] = (YRealDimension)m_LineScaleProportions[k] / (YRealDimension)largest;
	// compensate for very long secondary lines
	if ( m_NrLines > 1 )
		for ( k = 0; ( k < 3 && k < m_NrLines ); k++ )
		{
			if ( fHorizontal )
			{
				if ( ::Abs( ::Round( rcStrings[k].Width() * scales[k] ) ) > ::Abs( width ) )
					width = rcStrings[k].Width();
			}
			else
			{
				if ( ::Abs( ::Round( rcStrings[k].Height() * scales[k] ) ) > ::Abs( height ) )
					height = rcStrings[k].Height();
			}
		}
	// get bounding box of the whole headline and compensate to the height of the largest line
	rcHeadline = RRealRect( RRealSize( m_OutlineRect.Width(), m_OutlineRect.Height() ) );
	if ( m_NrLines == 1 )
		yScale = 1.0;
	else
		yScale = (YRealDimension)largest / (YRealDimension)proportion;
	if ( fHorizontal )
		rcHeadline.m_Bottom = ::Round( rcHeadline.m_Bottom * yScale );
	else
		rcHeadline.m_Right = ::Round( rcHeadline.m_Right * yScale );
	if ( rcHeadline.m_Bottom <= 0 || rcHeadline.m_Right <= 0  )
		return FALSE;
	TpsAssert( ( rcHeadline.m_Bottom > 0 ), "Bad bounding rect." );
	TpsAssert( ( rcHeadline.m_Right > 0 ), "Bad bounding rect." );
	// compute scale factor between graphic and largest line bound boxes 
	if ( ( xScale = rcHeadline.Width() / (YRealDimension)width ) < 0.0 )	
		xScale = -xScale;
	if ( ( yScale = rcHeadline.Height() / (YRealDimension)height ) < 0.0 ) 
		yScale = -yScale;
	if ( ::AreFloatsEqual( xScale, 0.0 ) || ::AreFloatsEqual( yScale, 0.0 ) ) 
		return FALSE;
	TpsAssert( ( xScale != 0.0 && yScale != 0.0 ), "Bad scale factor computed." );
	
	m_EmScale = yScale;

	if (!m_DistortGraphic)
		m_EmScale = Min( xScale, yScale );

	// apply scale factor to initial transform
	transform.MakeIdentity();
	if ( m_Justification == kFullJust && m_DistortGraphic )
	{
		if ( fHorizontal )
		{
			if ( xScale < yScale )
			{
				transform.PostScale( xScale, xScale );
				m_ScaleConstraint = kByHorizontal;
			}
			else
			{
				transform.PostScale( xScale, yScale );
				m_ScaleConstraint = kByHorizontalAndVertical;
			}
		}
		else
		{
			if ( yScale < xScale )
			{
				transform.PostScale( yScale, yScale );
				m_ScaleConstraint = kByVertical;
			}
			else
			{
				transform.PostScale( xScale, yScale );
				m_ScaleConstraint = kByHorizontalAndVertical;
			}
		}
	}
	else if ( xScale < yScale )
	{
		transform.PostScale( xScale, xScale );
		m_ScaleConstraint = kByHorizontal;
	}
	else 
	{
		transform.PostScale( yScale, yScale );
		m_ScaleConstraint = kByVertical;
	}

	m_EmSquareTransform = transform;						// save it off for use in scaling line weights
	// compute other line scale transforms based on the largest and store them away
	rz = m_BaseStringBoundingBoxes.Start(); 
	for ( k = 0; k < m_NrLines; k++ )
	{
		scale = ( ( k < 3 ) ? scales[k] : scales[2] );
		xform.MakeIdentity();
		if ( m_Justification == kFullJust  && m_DistortGraphic )
		{
			rcString = *rz;
			if ( fHorizontal )
			{
				if ( ( x2Scale = rcHeadline.Width() / rcString.Width() ) < 0.0 ) 
					x2Scale = -x2Scale;
				y2Scale = ( yScale * scale );
			}
			else
			{
				if ( ( y2Scale = rcHeadline.Height() / rcString.Height() ) < 0.0 ) 
					y2Scale = -y2Scale;
				x2Scale = ( xScale * scale );
			}
			xform.PostScale( x2Scale, y2Scale );
		}
		else if ( ::AreFloatsEqual( scale, 1.0 ) )
			xform = transform;
		else if ( xScale < yScale )
			xform.PostScale( ( xScale * scale ), ( xScale * scale ) );
		else 
			xform.PostScale( ( yScale * scale ), ( yScale * scale ) );
		m_LineScaleTransforms.InsertAtEnd( xform );
		++rz;
	}
	// sometimes we can index 1 past end of list
	xform.MakeIdentity();
	m_LineScaleTransforms.InsertAtEnd( xform );	
	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcLineScaleXformsViaPath()
//
// Description:		Calculate the scale transforms for each line based on the
//							metric data and relative proportions of each line.
//
// Returns:				Boolean indicating headline data is well defined
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::CalcLineScaleXformsViaPath( BOOLEAN fHorizontal )
{
YFontDimensions				fontDimensions;
RIntRectArrayIterator		rz; 
RRealRect						rcHeadline, rcPath;
RIntRect							rcString;
YRealDimension					width, height;
YRealDimension					xScale, yScale, yMinScale;
YRealDimension					xPathScale, yPathScale, rPathRotation, rPathLength;
R2dTransform 					transform;
RPath								follow;

	// validations/initializations
	TpsAssert( ( m_NrLines < kMaxHeadlineLines ), "Headline line overflow." );
	TpsAssert( ( m_DistortEffect == kFollowPath ), "Bad distort effect." );
	TpsAssert( fHorizontal, "Follow path does not work with vertical headlines." );
	m_LineScaleTransforms.Empty();
	m_EmSquareTransform.MakeIdentity();
	if ( !m_TextDefined )
		return FALSE;
	TpsAssert( ( m_NrLines == 1 ), "Bad number of lines." );
	TpsAssert( ( m_BaseStringBoundingBoxes.Count() == 2 ), "Bad number of bounding boxes." );
	fontDimensions = m_pFont->GetDimensions();
	// get initial bounding box of 1 line of headline
	rz = m_BaseStringBoundingBoxes.Start(); 
	rcString = *rz;
	rcString.Offset( RIntSize( -rcString.m_Left, -rcString.m_Top ) );			// zero base rect
	if ( rcString.m_Bottom < 0 )
		rcString.m_Bottom = -rcString.m_Bottom;										// flip it into positive coords
	if ( m_fUseLeading )
		rcString.m_Bottom += fontDimensions.m_Leading;
	width = rcString.Width();
	height = rcString.Height();
	// create an initially scaled path
	transform = CalcFollowPathScaleXform( m_pPathBottom->GetBoundingRect(), m_OutlineRect );
	follow = RPath( *m_pPathBottom, transform );
	rPathLength = follow.GetLength();
	TpsAssert( ( rPathLength > 1 ), "Bad follow path." );
	rcPath = follow.GetBoundingRect();
	transform.Decompose( rPathRotation, xPathScale, yPathScale );
	// get bounding box of the whole headline and compensate for the height of the path
	rcHeadline = RRealRect( RRealSize( m_OutlineRect.Width(), m_OutlineRect.Height() ) );
	rcHeadline.m_Right = rPathLength;
	if ( rcHeadline.m_Bottom <= rcPath.Height() )
		rcHeadline.m_Bottom = rcHeadline.m_Top + ::PrinterPointToLogicalUnit( 1 );
	else
		rcHeadline.m_Bottom -= rcPath.Height();
	if ( rcHeadline.m_Bottom <= 0 || rcHeadline.m_Right <= 0  )
		return FALSE;
	TpsAssert( ( rcHeadline.m_Bottom > 0 ), "Bad bounding rect." );
	TpsAssert( ( rcHeadline.m_Right > 0 ), "Bad bounding rect." );
	// compute scale factor between graphic and largest line bound boxes 
	if ( ( xScale = rcHeadline.Width() / width ) < 0.0 )	
		xScale = -xScale;
	yMinScale = ( 8.0 / m_FontPointSize );											// 8pt minimum y scale factor
	if ( m_PathAngle > 179 || m_PathAngle < -179 ) 								// this should be 180¡ but this is as close as I could get
	{
		if ( m_OutlineEffect == kSimpleFill )
			yScale = ( ( m_PathRadius * yPathScale ) / 2.0 ) /  height;		// no outline
		else
		{
			// 1st we need to approximate the line scaling
			YIntDimension	maxPenWeight;
			yScale = ( ( m_PathRadius * yPathScale ) / 2.0 ) /  height;		// approximate
			if ( yScale < 0.0 ) 
				yScale = -yScale;
			if ( yMinScale > yScale )
				yScale = yMinScale;
			TpsAssert( ( xScale != 0.0 && yScale != 0.0 ), "Bad scale factor computed." );
			m_EmSquareTransform.MakeIdentity();
			if ( xScale < yScale )
				m_EmSquareTransform.PostScale( xScale, xScale );
			else 
				m_EmSquareTransform.PostScale( yScale, yScale );
			// in order to compute the max line weight
			maxPenWeight = CalcMaxPenWidthBasedOnOutlineEffect( m_OutlineEffect, m_LineWeight );
			maxPenWeight = ::LogicalUnitToPrinterPoint( maxPenWeight ) / 2;// only 1/2 of a pen width goes out
			m_EmSquareTransform.MakeIdentity();
			// so that we can, more accurately, recompute the line scaling
			yScale = ( ( m_PathRadius * yPathScale ) / 2.0 ) /  ( height + maxPenWeight );
		}
	}
	else
		yScale = rcHeadline.Height() / height;
	if ( yScale < 0.0 ) 
		yScale = -yScale;
	if ( yMinScale > yScale )
		yScale = yMinScale;
	if ( ::AreFloatsEqual( xScale, 0.0 ) || ::AreFloatsEqual( yScale, 0.0 ) ) 
		return FALSE;
	TpsAssert( ( xScale != 0.0 && yScale != 0.0 ), "Bad scale factor computed." );
	m_EmScale = yScale;

	// apply scale factor to initial transform and store away
	transform.MakeIdentity();
	if ( xScale < yScale )
	{
		transform.PostScale( xScale, xScale );
		m_ScaleConstraint = kByHorizontal;
	}
	else 
	{
		transform.PostScale( yScale, yScale );
		m_ScaleConstraint = kByVertical;
	}
	m_EmSquareTransform = transform;													// save it off for use in scaling line weights
	m_LineScaleTransforms.InsertAtEnd( transform );
	// sometimes we can index 1 past end of list
	transform.MakeIdentity();
	m_LineScaleTransforms.InsertAtEnd( transform );	
	return TRUE;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcHorzLineStartPoints()
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::CalcHorzLineStartPoints( ) 
{
YFontDimensions				fontDimensions;
RIntRectArrayIterator		rz; 
R2dTransformArrayIterator	tz; 
RMBCStringArrayIterator		sz; 
R2dTransform					transform;
RIntPoint						pt, linespaces[ kMaxHeadlineLines + 1 ];
RIntRect							rcBounds, rcLast;
int								k, longest, tallest, shortest;
YIntDimension					onePoint = ::PrinterPointToLogicalUnit( 1 );

	// validations/initializations
	TpsAssert( ( m_NrLines < kMaxHeadlineLines ), "Headline line overflow." );
	TpsAssert( m_TextDefined, "Headline not properly defined." );
	TpsAssert( ( m_LineScaleTransforms.Count() != 0 ), "Headline not properly defined." );
	TpsAssert( ( m_LineTextList.Count() != 0 ), "Headline not properly defined." );
	TpsAssert( ( m_BaseStringBoundingBoxes.Count() != 0 ), "Headline not properly defined." );
	fontDimensions = m_pFont->GetDimensions();
	// calc largest line width  
	rz = m_BaseStringBoundingBoxes.Start(); 
	tz = m_LineScaleTransforms.Start(); 
	transform = *tz;
	++tz;
	longest = tallest = 0;
	for ( k = 0; k < m_NrLines; k++ )
	{
		rcBounds = *rz;
		rcBounds.Offset( RIntSize( -rcBounds.m_Left, -rcBounds.m_Top ) );		// zero base rect
		if ( rcBounds.m_Bottom < 0 )
			rcBounds.m_Bottom = -rcBounds.m_Bottom;									// flip it into positive coords
		rcBounds *= transform;
		if ( rcBounds.m_Right > longest )
			longest = rcBounds.m_Right;
		if ( rcBounds.m_Bottom > tallest )
			tallest = rcBounds.m_Bottom;
		transform = *tz;
		++tz;
		++rz;
	}
	if ( m_DistortEffect == kFollowPath )
	{
		RPath		follow;
		TpsAssert( ( m_pPathBottom != NULL ), "Bad follow path." );
		transform = CalcFollowPathScaleXform( m_pPathBottom->GetBoundingRect(), m_OutlineRect );
		follow = RPath( *m_pPathBottom, transform );
		longest = ::Max( follow.GetLength(), rcBounds.m_Right );
		TpsAssert( ( longest > 1 ), "Bad follow path." );
	}
	// calc rough headline line spacing and bounding box  
	rz = m_BaseStringBoundingBoxes.Start();
	tz = m_LineScaleTransforms.Start(); 
	transform = *tz;
	++tz;
	rcBounds = *rz;
	linespaces[0].m_x = 0;
	linespaces[0].m_y = m_OutlineRect.Height();
	for ( k = 1; k < ( m_NrLines + 1 ); k++ )
	{
		pt.m_x = 0;
		if ( m_DistortGraphic )
			pt.m_y = rcBounds.m_Top - rcBounds.m_Bottom + onePoint;
		else
			pt.m_y = fontDimensions.m_Descent + fontDimensions.m_Ascent;
		if ( m_fUseLeading )
			pt.m_y += fontDimensions.m_Leading;
		if ( m_DistortEffect == kAltBaseline )
			pt.m_y += ::Round( fontDimensions.m_Ascent * m_BaselineAlternate * 2 );
		pt *= transform;
		linespaces[k].m_x = 0;
		linespaces[k].m_y = linespaces[k - 1].m_y - pt.m_y;
		transform = *tz;
		++tz;
		++rz;
		rcBounds = *rz;
	}
	// center headline vertically within bounding box
	pt.m_x = 0;
	pt.m_y = linespaces[m_NrLines].m_y;
	if ( m_fUseLeading )
		pt.m_y += fontDimensions.m_Leading;
	if ( pt.m_y > 0 )
		pt.m_y = pt.m_y / 2;
	for ( k = 0; k < ( m_NrLines + 1 ); k++ )
		linespaces[k] -= pt;
	// calc line baselines and indents  
	rz = m_BaseStringBoundingBoxes.Start();
	tz = m_LineScaleTransforms.Start(); 
	sz = m_LineTextList.Start(); 
	transform = *tz;
	++tz;
	for ( k = 0; k < m_NrLines; k++ )
	{
		rcBounds = *rz;
		pt.m_x = 0;
		if ( m_DistortGraphic )
			pt.m_y = rcBounds.m_Top + ( onePoint / 2 );
		else
			pt.m_y = fontDimensions.m_Ascent;
		if ( m_fUseLeading )
			pt.m_y += fontDimensions.m_Leading;
		if ( m_DistortEffect == kAltBaseline )
			pt.m_y += ::Round( fontDimensions.m_Ascent * m_BaselineAlternate );
		pt *= transform;
		rcBounds.Offset( RIntSize( -rcBounds.m_Left, -rcBounds.m_Top ) );		// zero base rect
		rcBounds *= transform;
		if ( m_Justification == kCenterJust )
			pt.m_x = ( longest - rcBounds.m_Right ) / 2; 
		else if ( m_Justification == kRightJust )
			pt.m_x = longest - rcBounds.m_Right;
		else if ( m_Justification == kFullJust && (*sz)->GetStringLength() < 2 && !m_DistortGraphic )
			pt.m_x = ( longest - rcBounds.m_Right ) / 2; 
		else
			pt.m_x = 0;
		m_LineStartPoints[k].m_x = pt.m_x;
		m_LineStartPoints[k].m_y = linespaces[k].m_y - pt.m_y;
		transform = *tz;
		++tz;
		++rz;
		++sz;
	}
	// adjust graphic to draw in positive coordinate space
	//  this is a bit more complicated in the case of follow path
	//  and must be done at a later time
	if ( m_DistortEffect != kFollowPath )
	{
		RIntSize	delta = CalcTopLeftMarginDisplacement( 0.0 );
		if ( delta.m_dx != 0 || delta.m_dy != 0 )
			for ( k = 0; k < m_NrLines; k++ )
				m_LineStartPoints[k] += delta;
	}
	// adjust white space based on effects
	RIntSize	whiteSpace( kDefaultMargin, kDefaultMargin );				// after mucho problems with scaling I decided to hardcode the margin
	shortest = -1;
	for ( k = 0; k < m_NrLines; k++ )
		if ( m_LineStartPoints[k].m_x < whiteSpace.m_dx )
			shortest = m_LineStartPoints[k].m_x;
	if ( shortest >= 0 )
		for ( k = 0; k < m_NrLines; k++ )
			m_LineStartPoints[k].m_x += whiteSpace.m_dx - shortest;
	shortest = m_LineStartPoints[m_NrLines - 1].m_y;
	if ( shortest < whiteSpace.m_dy )
		for ( k = 0; k < m_NrLines; k++ )
			m_LineStartPoints[k].m_y += whiteSpace.m_dy - shortest;
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcVertLineStartPoints()
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::CalcVertLineStartPoints( ) 
{
YFontDimensions				fontDimensions;
RIntRectArrayIterator		rz; 
R2dTransformArrayIterator	tz; 
RMBCStringArrayIterator		sz; 
R2dTransform					transform;
RIntPoint						pt, linespaces[ kMaxHeadlineLines + 1 ];
RIntRect							rcBounds, rcLast;
int								k, longest, tallest, shortest;
YIntDimension					onePoint = ::PrinterPointToLogicalUnit( 1 );

	// validations/initializations
	TpsAssert( ( m_NrLines < kMaxHeadlineLines ), "Headline line overflow." );
	TpsAssert( m_TextDefined, "Headline not properly defined." );
	TpsAssert( ( m_LineScaleTransforms.Count() != 0 ), "Headline not properly defined." );
	TpsAssert( ( m_LineTextList.Count() != 0 ), "Headline not properly defined." );
	TpsAssert( ( m_BaseStringBoundingBoxes.Count() != 0 ), "Headline not properly defined." );
	TpsAssert( ( m_DistortEffect != kFollowPath ), "Follow path not allowed with vertical headlines." );
	fontDimensions = m_pFont->GetDimensions();
	// calc largest line width  
	rz = m_BaseStringBoundingBoxes.Start(); 
	tz = m_LineScaleTransforms.Start(); 
	transform = *tz;
	++tz;
	longest = tallest = 0;
	for ( k = 0; k < m_NrLines; k++ )
	{
		rcBounds = *rz;
		rcBounds.Offset( RIntSize( -rcBounds.m_Left, -rcBounds.m_Top ) );		// zero base rect
		if ( rcBounds.m_Bottom < 0 )
			rcBounds.m_Bottom = -rcBounds.m_Bottom;									// flip it into positive coords
		rcBounds *= transform;
		if ( rcBounds.m_Right > longest )
			longest = rcBounds.m_Right;
		if ( rcBounds.m_Bottom > tallest )
			tallest = rcBounds.m_Bottom;
		transform = *tz;
		++tz;
		++rz;
	}
	// calc rough headline line spacing and bounding box  
	rz = m_BaseStringBoundingBoxes.Start();
	tz = m_LineScaleTransforms.Start(); 
	transform = *tz;
	++tz;
	rcBounds = *rz;
	linespaces[0].m_x = 0;
	linespaces[0].m_y = m_OutlineRect.Height();
	for ( k = 1; k < ( m_NrLines + 1 ); k++ )
	{
		pt.m_x = rcBounds.Width() + onePoint;
		pt.m_y = 0;
		if ( m_DistortEffect == kAltBaseline )
			pt.m_x += ::Round( fontDimensions.m_Ascent * m_BaselineAlternate * 2 );
		pt *= transform;
		linespaces[k].m_x = linespaces[k - 1].m_x + pt.m_x;
		linespaces[k].m_y = linespaces[k - 1].m_y;
		transform = *tz;
		++tz;
		++rz;
		rcBounds = *rz;
	}
	// center headline horizontally within bounding box
	pt.m_x = m_OutlineRect.m_Right - linespaces[m_NrLines].m_x;
	pt.m_y = 0;
	if ( pt.m_x > 0 )
	{
		pt.m_x = pt.m_x / 2;
		for ( k = 0; k < ( m_NrLines + 1 ); k++ )
			linespaces[k] += pt;
	}
	// calc line baselines and indents  
	rz = m_BaseStringBoundingBoxes.Start();
	tz = m_LineScaleTransforms.Start(); 
	sz = m_LineTextList.Start(); 
	transform = *tz;
	++tz;
	for ( k = 0; k < m_NrLines; k++ )
	{
		pt.m_x = ( linespaces[k].m_x + linespaces[k + 1].m_x ) / 2;
		if ( m_DistortGraphic )
			pt.m_x += ( onePoint / 2 );
		rcBounds = *rz;
		rcBounds.Offset( RIntSize( -rcBounds.m_Left, -rcBounds.m_Top ) );		// zero base rect
		rcBounds *= transform;
		if ( m_Justification == kCenterJust )
			pt.m_y = ( tallest - rcBounds.m_Bottom ) / 2; 
		else if ( m_Justification == kRightJust )
			pt.m_y = tallest - rcBounds.m_Bottom;
		else if ( m_Justification == kFullJust && (*sz)->GetStringLength() < 2 && !m_DistortGraphic )
			pt.m_y = ( tallest - rcBounds.m_Bottom ) / 2; 
		else
			pt.m_y = 0;
		m_LineStartPoints[k].m_x = pt.m_x;
		m_LineStartPoints[k].m_y = linespaces[k].m_y - pt.m_y;
		transform = *tz;
		++tz;
		++rz;
		++sz;
	}
	// adjust graphic to draw in positive coordinate space
	RIntSize	delta = CalcTopLeftMarginDisplacement( 0.0 );
	if ( delta.m_dx != 0 || delta.m_dy != 0 )
		for ( k = 0; k < m_NrLines; k++ )
			m_LineStartPoints[k] += delta;
	// adjust white space based on effects
	RIntSize	whiteSpace( kDefaultMargin, kDefaultMargin );				// after mucho problems with scaling I decided to hardcode the margin
	shortest = -1;
	for ( k = 0; k < m_NrLines; k++ )
		if ( m_LineStartPoints[k].m_y < whiteSpace.m_dy )
			shortest = m_LineStartPoints[k].m_y;
	if ( shortest >= 0 )
		for ( k = 0; k < m_NrLines; k++ )
			m_LineStartPoints[k].m_y += whiteSpace.m_dy - shortest;
	shortest = m_LineStartPoints[m_NrLines - 1].m_x;
	if ( shortest < whiteSpace.m_dx )
		for ( k = 0; k < m_NrLines; k++ )
			m_LineStartPoints[k].m_x += whiteSpace.m_dx - shortest;
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::GetScaledGlyph()
//
// Description:		
//
// Returns:				RPath pointer or null
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RPath* RHeadlineGraphic::GetScaledGlyph( const RCharacter& character, const R2dTransform& xform, BOOLEAN fReally ) const
{
RPath*							pPath;

	TpsAssert( ( m_pFont != NULL ), "Undefined font." );

	// get glyph path in font units
	if ( ( pPath = m_pFont->GetGlyphPath( character ) ) == NULL )
		return NULL;

	// scale glyph path 
	if (( pPath = new RPath( *pPath, xform ) ) != NULL && fReally)
	{
		uWORD flags = (IsHorizontal() ? kHorizontalLines : kVerticalLines);
		pPath->ConvertLinesToBeziers( flags ) ;
	}

	return pPath;
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcFollowPathScaleXform()
//
// Description:		Create envelop and follow paths of correct size and position
//
// Returns:				R2dTransform
//
// Exceptions:			Memory
//
// ****************************************************************************
//
R2dTransform RHeadlineGraphic::CalcFollowPathScaleXform( const RIntRect& rcInBoundingBox, const RIntRect& rcOutBoundingBox ) const
{
R2dTransform 					transform;
YRealDimension					xScale = 1.0;
YRealDimension					yScale = 1.0;
YRealDimension					width = rcOutBoundingBox.Width();
YRealDimension					height = rcOutBoundingBox.Height();
YRealDimension					maxHeight, newHeight;

	TpsAssert( ( m_MaxPathHeightScale > 0 && m_MaxPathHeightScale <= 1 ), "Bad follow path max scale factor." );
	if ( rcInBoundingBox.m_Right != 0 )
		xScale = width / (YRealDimension)rcInBoundingBox.m_Right;
	if ( rcInBoundingBox.m_Bottom != 0 )
		yScale = height / (YRealDimension)rcInBoundingBox.m_Bottom;
	xScale = yScale = ::Min( xScale, yScale );
	if ( m_MaxPathHeightScale < 1 )
	{
		maxHeight = height * m_MaxPathHeightScale;
		newHeight = (YRealDimension)rcInBoundingBox.m_Bottom * xScale;
		if ( newHeight > maxHeight )
			xScale = yScale = xScale * ( maxHeight / newHeight );
	}
	transform.PreScaleAboutPoint( rcInBoundingBox.m_Left, rcInBoundingBox.m_Top, xScale, yScale );

	return transform;
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcWarpPathScaleXform()
//
// Description:		Create warp path of correct size and position
//
// Returns:				R2dTransform
//
// Exceptions:			Memory
//
// ****************************************************************************
//
R2dTransform RHeadlineGraphic::CalcWarpPathScaleXform( BOOLEAN fHorizontal, 
																		 EPathTypes pathType, 
																		 const RRealRect& rcInTopBoundingBox, 
																		 const RRealRect& rcInBottomBoundingBox, 
																		 const RRealSize& szOutBounds,
																		 BOOLEAN fTopCurves, 
																		 BOOLEAN fBottomCurves ) const
{
R2dTransform 					transform;
YRealDimension					xScale = 1.0;
YRealDimension					yScale = 1.0;
RRealPoint						ptBottomRight;
RRealRect						rcInEnvelopBoundingBox;
RRealRect						rcOutBoundingBox( szOutBounds );
YRealDimension					offset, xOffset, yOffset;

	if ( m_LineWeight == kHairLine || m_OutlineEffect == kSimpleFill )
		offset = 0;
	else
	{
		if ( pathType == kPathTop && !fTopCurves )
			offset = kDefaultMargin / 2;
		else if ( pathType == kPathBottom && !fBottomCurves )
			offset = kDefaultMargin / 2;
		else 
			offset = kDefaultMargin;										 // slop
	}
	xOffset = yOffset = 0;
	rcInEnvelopBoundingBox.Union( rcInTopBoundingBox, rcInBottomBoundingBox );
	ptBottomRight.m_x = ::Max( rcInTopBoundingBox.m_Right, rcInBottomBoundingBox.m_Right );
	ptBottomRight.m_y = ::Max( rcInTopBoundingBox.m_Bottom, rcInBottomBoundingBox.m_Bottom );
	if ( fHorizontal )
	{
		YRealDimension	topHeight = rcInTopBoundingBox.Height();
		YRealDimension	botHeight = rcInBottomBoundingBox.Height();
		YRealDimension	outHeight = rcOutBoundingBox.Height();
		xScale = ( ( ::AreFloatsEqual( ptBottomRight.m_x, 0.0 ) ) ? 1.0 : ( rcOutBoundingBox.Width() / ptBottomRight.m_x ) );
		switch ( pathType )
		{
			case kPathTop :
				outHeight *= ( topHeight / rcInEnvelopBoundingBox.Height() );
				if ( topHeight > 10.0 )
				{
					yScale = outHeight / topHeight;
					if ( rcOutBoundingBox.Height() > offset )
						yOffset = offset;
				}
				transform.PreTranslate( rcOutBoundingBox.m_Left, ( rcOutBoundingBox.m_Top + yOffset - rcInTopBoundingBox.m_Top ) );
				transform.PostScaleAboutPoint( rcOutBoundingBox.m_Left, ( rcOutBoundingBox.m_Top + yOffset ), xScale, yScale );
				break;
			case kPathBottom :
				outHeight *= ( botHeight / rcInEnvelopBoundingBox.Height() );
				if ( botHeight > 10.0 )
				{
					yScale = outHeight / botHeight;
					if ( rcOutBoundingBox.Height() > offset )
						yOffset = offset;
				}
				transform.PreTranslate( rcOutBoundingBox.m_Left, ( rcOutBoundingBox.m_Bottom - yOffset - rcInBottomBoundingBox.m_Bottom ) );
				transform.PostScaleAboutPoint( rcOutBoundingBox.m_Left, ( rcOutBoundingBox.m_Bottom - yOffset ), xScale, yScale );	
				break;
			default :
				TpsAssertAlways( "Invalid path type." );
				break;
		}
	}
	else
	{
		YRealDimension	topWidth = rcInTopBoundingBox.Width();
		YRealDimension	botWidth = rcInBottomBoundingBox.Width();
		YRealDimension	outWidth = rcOutBoundingBox.Width();
		yScale = ( ( ::AreFloatsEqual( ptBottomRight.m_y, 0.0 ) ) ? 1.0 : ( rcOutBoundingBox.Height() / ptBottomRight.m_y ) );
		switch ( pathType )
		{
			case kPathTop :
				outWidth *= ( topWidth / rcInEnvelopBoundingBox.Width() );
				if ( topWidth > 10.0 )
				{
					xScale = outWidth / topWidth;
					if ( rcOutBoundingBox.Width() > offset )
						xOffset = offset;
				}
				transform.PreTranslate( ( rcOutBoundingBox.m_Left + xOffset - rcInTopBoundingBox.m_Left ), rcOutBoundingBox.m_Top );
				transform.PostScaleAboutPoint( ( rcOutBoundingBox.m_Left + xOffset ), rcOutBoundingBox.m_Top, xScale, yScale );
				break;
			case kPathBottom :
				outWidth *= ( botWidth / rcInEnvelopBoundingBox.Width() );
				if ( botWidth > 10.0 )
				{
					xScale = outWidth / botWidth;
					if ( rcOutBoundingBox.Width() > offset )
						xOffset = offset;
				}
				transform.PreTranslate( ( rcOutBoundingBox.m_Right - xOffset - rcInBottomBoundingBox.m_Right ), rcOutBoundingBox.m_Top );
				transform.PostScaleAboutPoint( ( rcOutBoundingBox.m_Right - xOffset ), rcOutBoundingBox.m_Top, xScale, yScale );	
				break;
			default :
				TpsAssertAlways( "Invalid path type." );
				break;
		}
	}

	return transform;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcFollowPathEscapementScaleFactors( )
//
// Description:		Compute tighten and loosen factors for convex and concave 
//							curves based on the radius of the curve
//
// Returns:				Nothing.
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::CalcFollowPathEscapementScaleFactors( YRealDimension& tighten, YRealDimension& loosen ) const
{
YAngle					rotation;
YRealDimension			xScale;
YRealDimension			yScale;

	TpsAssert( ( m_DistortEffect == kFollowPath ), "Bad distort effect." );
	if ( m_PathType == kPathStraight )
		tighten = loosen = 1;
	else
	{
		m_EmSquareTransform.Decompose( rotation, xScale, yScale );
		YRealDimension		fontPointSize = ::PrinterPointToLogicalUnit( m_FontPointSize ) * yScale; 
		R2dTransform	pathScaleXform = CalcFollowPathScaleXform( m_pPathBottom->GetBoundingRect(), m_OutlineRect );
		pathScaleXform.Decompose( rotation, xScale, yScale );
		YRealDimension		pathRadius = m_PathRadius * yScale; 
		tighten = pathRadius / ( pathRadius + ( fontPointSize / 4 ) );
		loosen = ( pathRadius + ( fontPointSize / 3 ) ) / pathRadius;
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcBaselineOffset( )
//
// Description:		
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
//
YIntDimension RHeadlineGraphic::CalcBaselineOffset( BOOLEAN fInc ) 
{
BOOLEAN				fHorizontal = IsHorizontal();
YIntDimension 		offset;

	TpsAssert( ( m_pFont != NULL ), "Undefined font." );
	if ( fHorizontal ) 
	{
		YFontDimensions	fontDimensions = m_pFont->GetDimensions();
		offset = ::Round( static_cast<YRealDimension>( fontDimensions.m_Ascent ) * m_BaselineAlternate );
	}
	else
	{
		RIntRect				rcBounds;
		m_pFont->GetMaxCharBounds( &rcBounds );
		offset = ::Round( static_cast<YRealDimension>( rcBounds.Width() ) * m_BaselineAlternate );
	}
	if ( ( m_BaselineDirection & 0x01 ) != 0 ) 
		offset = 0;
	else if ( ( m_BaselineDirection % 4 ) == 0 ) 
		offset = -offset;
	if ( fInc ) 
		m_BaselineDirection++;
	return offset;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcCharScaleXform()
//
// Description:		Compute the scale/rotate/shear transform for a character,
//							taking into account the distortion effects applied.
//
// Returns:				R2dTransform
//
// Exceptions:			Memory
//
// ****************************************************************************
//
R2dTransform RHeadlineGraphic::CalcCharScaleXform( const RCharacter& character, 
															  		R2dTransform xform,
															  		YAngle angle ) 
{
	if ( angle != 0.0 )
	{
		xform.PreRotateAboutOrigin( angle );
	}

	if ( m_DistortEffect == kShear || m_DistortEffect == kAltShear || m_DistortEffect == kAltShearRotate )
	{
		TpsAssert( ( m_ShearAngle > -90 && m_ShearAngle < 90 ), "Bad shear data." );
		TpsAssert( ( m_pFont != NULL ), "Undefined font." );
		RPath*	pPath = m_pFont->GetGlyphPath( character );
		if ( m_ShearAngle != 0 )
		{
			YRealDimension	shear = m_ShearSlope;
			if ( m_DistortEffect == kAltShear || m_DistortEffect == kAltShearRotate )
				shear *= m_ShearDirection;
			xform.PreXShear( shear );
		}
		if ( m_DistortEffect == kAltShearRotate )
		{
			TpsAssert( ( m_ShearRotaion > -90 && m_ShearRotaion < 90 ), "Bad shear data." );
			RRealSize		pathSize;												// glyph path size in font units
			if ( pPath == NULL )
				pathSize = RRealSize( 0.0, 0.0 );
			else
				pathSize = pPath->GetPathSize();
			xform.PreRotateAboutPoint( ( pathSize.m_dx / 2.0 ), 0.0, ::DegreesToRadians( m_ShearRotaion * -m_ShearDirection ) );
		}
		if ( pPath != NULL )
			m_ShearDirection = static_cast<sWORD>( -m_ShearDirection );	// only if there is an outline
	}

	return xform;
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcCharDisplacement()
//
// Description:		
//
// Returns:				RIntSize
//
// Exceptions:			None
//
// ****************************************************************************
//
void RHeadlineGraphic::CalcCharDisplacement( const RCharacter&		chCur,
															const RCharacter&		chNxt,
															const R2dTransform&	xformScale,
															const R2dTransform&	xformScaledEffect,
															BOOLEAN					fHorizontal,
															RRealSize&				preAdjust,
															RRealSize&				postAdjust )
{
	TpsAssert( ( m_pFont != NULL ), "Undefined font" );

	RPath*				pGlyph;
	RRealRect			rcGlyph( 0.0, 0.0, 0.0, 0.0 );
	RRealRect			rcScaledGlyph;
	RRealVectorRect	rvcEffectGlyph;
	RRealRect			rcCell;
	RRealRect			rcScaledCell;
	RRealVectorRect	rvcEffectCell;
	YRealDimension		nCharWidth;
	YRealDimension		nAWidth;
	YRealDimension		nBWidth;
	YRealDimension		nCWidth;
	YRealDimension		nCellWidth;
	YRealDimension		nKernAmount;
	YRealDimension		nCompensate;
	YRealDimension		nLeading;
	YFontDimensions	fontDimensions;
	RRealSize			baselineOffset;

	pGlyph	= m_pFont->GetGlyphPath( chCur );
	if ( pGlyph )
	{
		rcGlyph	= pGlyph->GetBoundingRect();
	}
	rcScaledGlyph = rcGlyph * xformScale;

	//
	// Get the character metrics...
	fontDimensions	= m_pFont->GetDimensions();
	nCharWidth		= 											YRealDimension( m_pFont->GetCharacterWidth( chCur) );
	nKernAmount		= ( m_fUseKerning != FALSE ) ?	YRealDimension( m_pFont->GetKernAmount(chCur, chNxt ) ) :
						  /* no kerning */					YRealDimension( 0 );
	nCompensate		= ( m_Compensate == kTight ) ?	YRealDimension( -m_Compensation ) :
						  ( m_Compensate == kLoose ) ?	YRealDimension( m_Compensation ) :
						  /* no compensation */				YRealDimension( 0 );
	nCellWidth		= ( fHorizontal ? ( nCharWidth + nKernAmount + nCompensate ) : nCharWidth );
	nAWidth			= rcGlyph.m_Left;
	nBWidth			= rcGlyph.Width();
	nCWidth			= nCellWidth - rcGlyph.m_Right;
	if ( fHorizontal || !m_fUseLeading || m_DistortGraphic )
		nLeading	= YRealDimension( 0 );
	else if ( fontDimensions.m_Leading == 0 )
		nLeading	= kDefaultLeading;
	else 
		nLeading	= YRealDimension( fontDimensions.m_Leading );	// font has leading info

	rcCell			= RRealRect( 0.0, -( fontDimensions.m_Descent + nLeading ), nCellWidth, fontDimensions.m_Ascent );
	if ( !fHorizontal && rcGlyph.IsEmpty() )
		rcCell.m_Top = -nLeading;											// vertical space/empty characters only use ascent
	rcScaledCell	= rcCell * xformScale;
	rvcEffectCell	= RRealVectorRect( rcCell ) * xformScaledEffect;
	rvcEffectGlyph	= RRealVectorRect( rcGlyph ) * xformScaledEffect;

	preAdjust	= RRealSize( 0.0, 0.0 );
	postAdjust	= RRealSize( 0.0, 0.0 );

	//
	// Compute the whitespace to the right and bottom...
	m_WhiteSpace	= rvcEffectCell.m_TransformedBoundingRect.m_BottomRight -
						  rvcEffectGlyph.m_TransformedBoundingRect.m_BottomRight;

	//
	// Compute the pre and post character displacements
	if ( fHorizontal )
	{
		//
		// Handle the effect...
		switch ( m_DistortEffect )
		{
			case kAltBaseline:
				baselineOffset		= RRealSize( YRealDimension(0), YRealDimension( CalcBaselineOffset( BOOLEAN( pGlyph != NULL ) ) ) );
				baselineOffset		*= xformScaledEffect;
				postAdjust.m_dx	= rvcEffectCell.m_TransformedBoundingRect.Width();
				preAdjust.m_dy		= baselineOffset.m_dy;
				postAdjust.m_dy	= -baselineOffset.m_dy;
				break;
			case kShear:
				//
				// we only need advance by the width...
				postAdjust.m_dx	= rvcEffectCell.Width();
				postAdjust.m_dy	= YRealDimension( 0 );
				break;
			case kAltShearRotate:
			case kAltShear:
				//
				// we advance by the cell width...
#if 1
				preAdjust.m_dx		= -rvcEffectGlyph.m_TransformedBoundingRect.m_Left;
				postAdjust.m_dx	= rvcEffectGlyph.m_TransformedBoundingRect.m_Left + rvcEffectGlyph.m_TransformedBoundingRect.Width();
				//
				// include the spacing...
				postAdjust.m_dx	+= rcScaledCell.m_Right - rcScaledGlyph.m_Right;
#else
				postAdjust.m_dx	= rvcEffectCell.m_TransformedBoundingRect.Width();
				//
				// but we also need to advance far enough to keep our character
				// lined up...
				preAdjust.m_dx		= rcScaledCell.m_Left - rvcEffectCell.m_TransformedBoundingRect.m_Left;
#endif
				postAdjust.m_dy	= YRealDimension( 0 );
				break;

			case kNonDistort:
			case kFollowPath:
			case kWarpToEnvelop:
				//
				// nothing special to do...
				postAdjust.m_dx	= rvcEffectCell.m_TransformedBoundingRect.Width();
				postAdjust.m_dy	= YRealDimension( 0 );
				break;
		}
	}
	else /* vertical */
	{
		//
		// center this character...
		// the effect bounds are the bounding rect of the transformed glyph.
		// first, we offset so that the start point is unaffected...
		preAdjust.m_dx	= -rvcEffectGlyph.m_TransformedBoundingRect.m_Left;
		//
		// now center it...
		preAdjust.m_dx	-= rvcEffectGlyph.m_TransformedBoundingRect.Width() / 2.0;
		//
		// Leave the next character's start point at the center...
		postAdjust.m_dx	= -preAdjust.m_dx;
		//
		// Setup the offset to the next char position vertically.
		postAdjust.m_dy	= -(rvcEffectCell.m_TransformedBoundingRect.Height());
		if ( m_DistortGraphic )
		{
			if ( !rcGlyph.IsEmpty() )
			{
				//
				//
				preAdjust.m_dy	= -(rvcEffectGlyph.m_TransformedBoundingRect.m_Bottom);
				//
				//
				RRealSize	onePoint	= RRealSize( 0.0, ::PrinterPointToLogicalUnit( 2 ) ) * xformScale;
				postAdjust.m_dy = ( rvcEffectGlyph.m_TransformedBoundingRect.m_Top - onePoint.m_dy );
			}
		}
		//
		// Handle the effect...
		switch ( m_DistortEffect )
		{
			case kAltBaseline:
				baselineOffset		= RRealSize( YRealDimension( CalcBaselineOffset( BOOLEAN( pGlyph != NULL ) ) ), YRealDimension( 0 ) );
				baselineOffset		*= xformScaledEffect;
				preAdjust.m_dx		+= baselineOffset.m_dx;
				postAdjust.m_dx	+= -baselineOffset.m_dx;
				break;
			case kAltShearRotate:
			case kAltShear:
			case kNonDistort:
			case kFollowPath:
			case kWarpToEnvelop:
			case kShear:
				//
				// nothing to do...
				break;
		}
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::ReplaceMarkerPointData( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::ReplaceMarkerPointData( YCounter nr, const RRealRect& rect )
{
	RRealPointArrayIterator	ptz = m_OriginalPointsStream.Start();
	ptz += ( nr * 5 );
	*ptz = rect.m_TopLeft;
	++ptz;
	*ptz = RRealPoint ( rect.m_Right, rect.m_Top );
	++ptz;
	*ptz = rect.m_BottomRight;
	++ptz;
	*ptz = RRealPoint ( rect.m_Left, rect.m_Bottom );
	++ptz;
	*ptz = rect.m_TopLeft;
	++ptz;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcDefaultMarkerPoints( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::CalcDefaultMarkerPoints( RIntPoint* points )
{
RIntRect				rcBoundingBox( 0, 0, 0, 0 );
RIntRect				rcBounds;
RIntPoint			pt0;
int					k;

	TpsAssert( ( m_DistortEffect != kFollowPath ), "Illeagal contruct of follow path." );
	// add extreme points of each line to the bounding box of the graphic
	R2dTransformArrayIterator	tz = m_LineScaleTransforms.Start(); 
	RIntRectArrayIterator		rz = m_BaseStringBoundingBoxes.Start(); 
	for ( k = 0; k < m_NrLines; k++ )
	{
		rcBounds = *rz * *tz;
		rcBounds.m_TopLeft += m_LineStartPoints[k];
		rcBounds.m_BottomRight += m_LineStartPoints[k];
		if ( rcBoundingBox.IsEmpty() )
			rcBoundingBox = rcBounds;
		else
			rcBoundingBox.Union( rcBoundingBox, rcBounds );
		++tz;
		++rz;
	}
	// calc center of bounding box as default marker point 
	if ( IsHorizontal() )
	{
		pt0.m_x = ( rcBoundingBox.m_Left + rcBoundingBox.m_Right ) / 2;
		pt0.m_y = ( rcBoundingBox.m_Top + rcBoundingBox.m_Bottom ) / 2;
	}
	else
	{
		pt0.m_x = m_LineStartPoints[0].m_x;
		pt0.m_y = m_LineStartPoints[0].m_y - ( rcBoundingBox.m_Bottom - rcBoundingBox.m_Top ) / 2;
	}
	// add marker points to extreme points list
	for ( k = 0; k < kNrExtremeMarkerPoints; k++ )
	{
		*points = pt0;
		points++;
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcCharStartPoints( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::CalcCharStartPoints( const RMBCString& mbString,
														  const RIntRect& rcBaseString, 
														  const R2dTransform& xformScale,
														  const RIntPoint& ptStart,
														  BOOLEAN fHorizontal )
{
RMBCStringIterator 			itrCur;
RCharacter						chNext;
R2dTransform					xformEffect;
YIntDimension					fullness = 0;
RRealSize						preAdjust;
RRealSize						postAdjust;
RRealPoint						ptCur( ptStart );
RIntRect							rcBounds; 

	//
	// validations/initializations
	TpsAssert( ( m_pFont != NULL ), "Undefined font." );
	m_TempPoints.Empty();
	//
	// compute the extra spacing for full justification
	if ( m_Justification == kFullJust )
	{
		if ( m_NrLines == 1 || m_DistortGraphic )
			fullness = 0;
		else
		{
			YCounter		length = mbString.GetStringLength();
			RIntRect		rcString = GetStringBounds( mbString );
			rcString *= xformScale;
			if ( length > 1 ) 
			{
				if ( fHorizontal )
					fullness = ( m_OutlineRect.Width() - rcString.Width() );
				else
					fullness = ( m_OutlineRect.Height() - rcString.Height() );
				if ( fullness > 0 ) 
				{
					fullness = ( fullness / ( length - 1 ) );
					if ( !fHorizontal )
						fullness = -fullness;
				}
				else
					fullness = 0;
			}
			else if ( !fHorizontal )
				fullness = 0;
			else if ( length == 1 )
				ptCur.m_x = ( ( m_OutlineRect.Width() / 2 ) - ( rcString.Width() / 2 ) );
		}
	}
	//
	// loop thru characters in line
	m_BaselineDirection	= m_ShearDirection = 1;
	itrCur					= mbString.Start();
	while ( itrCur != mbString.End() )
	{
		if ( ( itrCur + 1 ) == mbString.End() )
			chNext = kSpace;
		else
			chNext = *( itrCur + 1 );

		xformEffect	= CalcCharScaleXform( (*itrCur), xformScale, 0.0 );
		CalcCharDisplacement( (*itrCur), chNext, xformScale, xformEffect, fHorizontal, preAdjust, postAdjust );

		ptCur	+= preAdjust;
		m_TempPoints.InsertAtEnd( ptCur );
		ptCur	+= postAdjust;

		if ( m_Justification == kFullJust )
		{
			if ( fHorizontal )
				ptCur.m_x += fullness;
			else
				ptCur.m_y += fullness;
		}
		++itrCur;
	}
	//
	// the end-of-line point...
	m_TempPoints.InsertAtEnd( ptCur );
	//
	// add points to end of list hilighting the extremes of the line
	rcBounds = rcBaseString * xformScale;
	rcBounds.m_TopLeft += ptStart;
	rcBounds.m_BottomRight += ptStart;
	m_TempPoints.InsertAtEnd( rcBounds.m_TopLeft );	 
	m_TempPoints.InsertAtEnd( rcBounds.m_BottomRight );	 
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcCharStartPointsViaPath( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::CalcCharStartPointsViaPath( const RMBCString& mbString,
																	const RIntRect& rcBaseString, 
														  			const R2dTransform& xformScale,
														  			const RIntPoint& ptStart,
														  			BOOLEAN fHorizontal,
														  			const RPath& path, 
														  			YScaleFactor scale )
{
RPath								follow;
RMBCStringIterator 			cz;
RIntPointArrayIterator		pz;
R2dTransform					transform;
RCharacter						character, next;
YIntDimension					dy, dx;
YIntDimension					distance, overflow, wise;
YIntDimension					yLastWise(0);
YRealDimension					tighten, loosen; 
YRealDimension					rLength, rPathLength;
RRealSize						adjust;
RRealSize						preAdjust;
RRealSize						postAdjust;
RIntPoint						pt0, pt1, ptEnd;
RRealPoint						pt2, pt5, pt6, pt7, pt8;
RRealRect						rcPath;
YAngle 							angle = 0.0;
RPath*							pGlyph = NULL;

	// validations/initializations
	TpsAssert( ( m_pFont != NULL ), "Undefined font." );
	TpsAssert( ( m_NrLines == 1 ), "Bad number of lines." );
	TpsAssert( ( m_pPathBottom != NULL ), "Bad follow path." );
	TpsAssert( fHorizontal, "Follow path does not work with vertical headlines." );
	m_TempPoints.Empty();
	m_TempAngles.Empty();
	// create path of, hopefully, the correct size and position
	rcPath = path.GetBoundingRect();
	transform.PreScaleAboutPoint( 0.0, rcPath.m_Bottom, scale, scale );	
	transform.PostTranslate( 0.0, ( m_OutlineRect.m_Bottom - rcPath.m_Bottom ) );
	follow = RPath( path, transform );
	// force calc of segment lengths
	if ( ( rPathLength = follow.GetLength() ) == 0 )
	{
		TpsAssertAlways( "Bad follow path." );
		return;
	}
	ptEnd = follow.GetEndPoint();
	// compute tighten and loosen factors for convex and concave curves based on the radius of the curve
	if ( m_PathType == kPathStraight )
		tighten = loosen = 1;
	else
		CalcFollowPathEscapementScaleFactors( tighten, loosen );
	// loop thru characters in line and walk the path
	dx = ::Min( ptStart.m_x, (YIntDimension)::Round( rPathLength ) );
	dy = ptStart.m_y;
	pt1.m_x = dx;
	pt1.m_y = dy;
	m_BaselineDirection = m_ShearDirection = 1;
	overflow = 0;

	YRealDimension rotation, xScale, yScale;
	xformScale.Decompose( rotation, xScale, yScale );

	YIntDimension yEmHeight = m_pFont->GetHeight() * yScale * 0.35;
	YIntDimension dxTighten = (1.0 - tighten) * yEmHeight;
	YIntDimension dxLoosen  = (loosen - 1.0) * yEmHeight;

	cz = mbString.Start(); 
	next = ( ( cz == mbString.End() ) ? kSpace : *cz );

	while ( cz != mbString.End() )
	{
		++cz;
		character = next;
		next = ( ( cz == mbString.End() ) ? kSpace : *cz );
		CalcCharDisplacement( character, next, xformScale, xformScale, TRUE, preAdjust, postAdjust );
		if ( overflow == 0 )
		{
			dx += preAdjust.m_dx;
			pt0 = follow.PointOnPath( dx );															// start point of char on path
			pt1 = follow.PointOnPath( dx, ::Round( postAdjust.m_dx ), &distance );		// start point of next char on path
			if ( m_PathType == kPathConcave )
			{
				wise = -1;
				postAdjust.m_dx += dxLoosen;
			}
			else if ( m_PathType == kPathConvex )
			{
				wise = 1;
				postAdjust.m_dx -= dxTighten;
			}
			else if ( m_PathType == kPathCurved )
			{
				wise = ::SideOfLine( pt0, pt1, RIntPoint( follow.PointOnPath( dx + ( distance / (YIntDimension)2 ) ) ) );
				if (wise) wise /= abs( wise ); // Normalize

				if ( wise > 0 )
					postAdjust.m_dx -= dxTighten;
				else if ( wise < 0 )
					postAdjust.m_dx += dxLoosen;

				if (yLastWise && wise != yLastWise)
				{
					if (wise > yLastWise)
					{
						// Switched from convex to concave, so
						// remove last character adjustment
						dx -= dxLoosen; 
					}
					else if (wise < yLastWise)
					{
						// Switched from concave to convex, so 
						// remove last character adjustment
						dx += dxTighten; 
					}

					pt0 = follow.PointOnPath( dx );															// start point of char on path
					pt1 = follow.PointOnPath( dx, ::Round( postAdjust.m_dx ), &distance );		// start point of next char on path
				}

				yLastWise = wise;
			}
			else
				wise = 0;
			if ( wise != 0 )
				pt1 = follow.PointOnPath( dx, ::Round( postAdjust.m_dx ), &distance );	// revised start point of next char on path
			pt2.m_x = pt1.m_x - pt0.m_x;
			pt2.m_y = pt1.m_y - pt0.m_y;
			if ( !( ::AreFloatsEqual( pt2.m_y, 0.0 ) && ::AreFloatsEqual( pt2.m_x, 0.0 ) ) )	
				angle = atan2( pt2.m_y, pt2.m_x );					// the last character could go on the last point on the line
			pt0.m_y += dy;
			m_TempPoints.InsertAtEnd( pt0 );
			m_TempAngles.InsertAtEnd( angle );
			if ( pt1 == ptEnd )
			{
				pt1.m_y += dy;
				overflow = ::Round( postAdjust.m_dx - pt0.Distance( pt1 ) );
				if ( overflow <= 1 )
					overflow = 0;
			}
		}
		else
		{
			distance = ::Round( postAdjust.m_dx );
			overflow += ::Round( postAdjust.m_dx );
		}
		dx += distance;
		dy = ::Round( postAdjust.m_dy + ptStart.m_y );
	}
	pt1.m_y += dy;
	m_TempPoints.InsertAtEnd( pt1 );								// eol
	m_TempAngles.InsertAtEnd( 0.0 );
	// check for an overflow or underflow of the path and recompute
	if ( overflow > 0 )
	{
		scale += (YRealDimension)overflow / rPathLength;
		CalcCharStartPointsViaPath( mbString, rcBaseString, xformScale, ptStart, TRUE, path, scale );
		return;
	}
	else if ( m_Justification == kRightJust )
	{
		if ( scale <= 1.0 )
		{
			pt1.m_y -= dy;
			overflow = ::Round( pt1.Distance( ptEnd ) ); 
			if ( overflow > distance )
			{
				pt0 = ptStart;
				pt0.m_x += ( overflow - distance );
				CalcCharStartPointsViaPath( mbString, rcBaseString, xformScale, pt0, TRUE, path, scale );
				return;
			}
		}
	}
	// add points to end of list hilighting the path used
	rLength = 0.0;
	while ( rLength <= 1.0 )
	{
		pt1 = follow.PointOnPath( rLength ); 
		pt1.m_y += ptStart.m_y;
		m_TempPoints.InsertAtEnd( pt1 );	 
		rLength += 0.125;
	}
	rcPath = rcBaseString * xformScale;
	CalcCharDisplacement( RCharacter( 'i' ), kSpace, xformScale, xformScale, TRUE, preAdjust, postAdjust );
	pt5 = follow.GetStartPoint();
	pt5.m_y += ptStart.m_y;
	pt6 = follow.PointOnPath( (YIntDimension)::Round( postAdjust.m_dx ) );
	pt6.m_y += ptStart.m_y;
	::PerpendicularLineAtMidPoint( pt5, pt6, pt7, pt8 );
	rLength = pt7.Distance( pt8 );
	if ( ::AreFloatsEqual( rLength, 0.0 ) )
		pt1 = pt7;
	else
		pt1 = ::Interpolate( pt7, pt8, ( -rcPath.m_Top / rLength ) );
	m_TempPoints.InsertAtEnd( pt1 );	 
	if ( ::AreFloatsEqual( rLength, 0.0 ) )
		pt1 = pt7;
	else
		pt1 = ::Interpolate( pt7, pt8, ( -rcPath.m_Bottom / rLength ) );
	m_TempPoints.InsertAtEnd( pt1 );	 
	pt5 = follow.PointOnPath( (YIntDimension)::Round( rPathLength / 2.0 ) );
	pt5.m_y += ptStart.m_y;
	pt6 = follow.PointOnPath( (YIntDimension)::Round( ( rPathLength / 2.0 ) + postAdjust.m_dx ) );
	pt6.m_y += ptStart.m_y;
	::PerpendicularLineAtMidPoint( pt5, pt6, pt7, pt8 );
	rLength = pt7.Distance( pt8 );
	if ( ::AreFloatsEqual( rLength, 0.0 ) )
		pt1 = pt7;
	else
		pt1 = ::Interpolate( pt7, pt8, ( -rcPath.m_Top / rLength ) );
	m_TempPoints.InsertAtEnd( pt1 );	 
	if ( ::AreFloatsEqual( rLength, 0.0 ) )
		pt1 = pt7;
	else
		pt1 = ::Interpolate( pt7, pt8, ( -rcPath.m_Bottom / rLength ) );
	m_TempPoints.InsertAtEnd( pt1 );	 
	pt5 = follow.PointOnPath( (YIntDimension)::Round( rPathLength - postAdjust.m_dx ) );
	pt5.m_y += ptStart.m_y;
	pt6 = follow.GetEndPoint();
	pt6.m_y += ptStart.m_y;
	::PerpendicularLineAtMidPoint( pt5, pt6, pt7, pt8 );
	rLength = pt7.Distance( pt8 );
	if ( ::AreFloatsEqual( rLength, 0.0 ) )
		pt1 = pt7;
	else
		pt1 = ::Interpolate( pt7, pt8, ( -rcPath.m_Top / rLength ) );
	m_TempPoints.InsertAtEnd( pt1 );	 
	if ( ::AreFloatsEqual( rLength, 0.0 ) )
		pt1 = pt7;
	else
		pt1 = ::Interpolate( pt7, pt8, ( -rcPath.m_Bottom / rLength ) );
	m_TempPoints.InsertAtEnd( pt1 );	 
	// adjust graphic to draw in positive coordinate space
	adjust = RRealSize( 0.0, 0.0 );
	rcPath = follow.GetBoundingRect();
//	delta = CalcTopLeftMarginDisplacement( -(*(m_TempAngles.Start())) );
//	delta.m_dy = -( rcPath.Height() / 2.0 );
// REVEIW RAH compute displacement using displacement function rather than fetching the 1st char
	// compute displacement for white space to left of line
	character = *(mbString.Start());
	transform = CalcCharScaleXform( character, xformScale, -(*(m_TempAngles.Start())) );
	if ( ( pGlyph = GetScaledGlyph( character, transform, FALSE ) ) != NULL )
	{
		RRealRect	rcBounds = pGlyph->GetBoundingRect();
		if ( rcBounds.m_Left < 0 )
			adjust.m_dx = -rcBounds.m_Left;
		delete pGlyph;
		pGlyph = NULL;
	}
	// loop thru start point and adjust them if necessary
	if ( adjust.m_dx != 0 || adjust.m_dy != 0 )
	{
		pz = m_TempPoints.Start(); 
		while ( pz != m_TempPoints.End() )
		{
			(*pz) += (RIntSize)adjust;
			++pz;
		}
	}
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcMinRectInRect( )
//
// Description:		
//
// Returns:				RRealRect
//
// Exceptions:			None
//
// ****************************************************************************
//
RRealRect RHeadlineGraphic::CalcMinRectInRect( RRealPoint pt1, RRealPoint pt2, RRealRect rect, const RRealSize& size ) const
{
	::midpoint( pt1, pt1, pt2 );
	pt1 = rect.PinPointInRect( pt1 );
	rect.m_TopLeft = pt1;
	rect.m_BottomRight = pt1;
	rect.Inflate( size );
	return rect;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcTopLeftMarginDisplacement( )
//
// Description:		
//
// Returns:				RIntSize
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RIntSize RHeadlineGraphic::CalcTopLeftMarginDisplacement( YAngle angle ) 
{
RMBCStringArrayIterator		sz; 
R2dTransformArrayIterator	tz; 
RMBCStringIterator 			cz;
RMBCString  					mbString;
R2dTransform					xformScale;
RCharacter						character;
RIntSize							delta;
RPath*							pGlyph = NULL;
YIntDimension					newLeft;

	// validations/initializations
	TpsAssert( m_TextDefined, "Bad text." );
	delta.m_dx = delta.m_dy = 0;
	m_BaselineDirection = m_ShearDirection = 1;
	// loop thru lines in headline
	sz = m_LineTextList.Start(); 
	tz = m_LineScaleTransforms.Start(); 
	for ( int k = 0; k < m_NrLines; k++ )
	{
		// calc bounding box of 1st character in each line
		mbString = *(*sz);
		cz = mbString.Start(); 
		character = ( ( cz == mbString.End() ) ? kSpace : *cz );
		xformScale = CalcCharScaleXform( character, *tz, angle );
		if ( ( pGlyph = GetScaledGlyph( character, xformScale, FALSE ) ) != NULL )
		{
			RIntRect			rcBounds = pGlyph->GetBoundingRect();
			if ( IsHorizontal() )
				newLeft = rcBounds.m_Left;
			else
				newLeft = m_LineStartPoints[k].m_x - ( rcBounds.m_Left + ( rcBounds.Width() / 2 ) );
			if ( newLeft < 0 )
				if ( ( delta.m_dx + newLeft ) < 0 )
					delta.m_dx = -newLeft;
			delete pGlyph;
			pGlyph = NULL;
		}
		++cz;
		++sz;
		++tz;
	}
	return delta;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcProjectionXforms()
//
// Description:		Calculate the transforms for each stage of projection from 
//							the base graphic
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::CalcProjectionXforms( )
{
R2dTransform					transform;
RRealPoint						pt, pt1;
YRealDimension					depth, distance, increm, length;
YRealDimension					sinAngle, cosAngle;
YAngle							angle;
YScaleFactor					scale;
int								i;
RRealRect						rcBounds, rcOutline;
YRealDimension					xScale, yScale;
YCounter							nrStages;
YIntDimension 					maxPenWeight;

	// calc projection depth based on line height
#if 0
	//attempt the 1st
	m_pFont->GetMaxCharBounds( &rcBounds );
	rcBounds.Offset( RRealSize( -rcBounds.m_Left, -rcBounds.m_Top ) );			// zero base rect
	if ( rcBounds.m_Bottom < 0 )
		rcBounds.m_Bottom = -rcBounds.m_Bottom;											// flip it into positive coords
	m_EmSquareTransform.Decompose( angle, xScale, yScale );
	length = rcBounds.Height() * yScale;
#endif
#if 0
	//attempt the 2nd
	if ( m_NrLines == 1 && m_DistortEffect != kFollowPath )
	{
		if ( IsHorizontal() )
			length = m_BoundingBox.Height();
		else
			length = m_BoundingBox.Width();
	}
	else
	{
		RIntRectArrayIterator		rz = m_BaseStringBoundingBoxes.Start(); 
		R2dTransformArrayIterator	tz = m_LineScaleTransforms.Start(); 
		YIntDimension					widest = 0;
		YIntDimension					tallest = 0;
		for ( i = 0; i < m_NrLines; i++ )
		{
			rcBounds = *rz;
			rcBounds.Offset( RRealSize( -rcBounds.m_Left, -rcBounds.m_Top ) );	// zero base rect
			if ( rcBounds.m_Bottom < 0 )
				rcBounds.m_Bottom = -rcBounds.m_Bottom;									// flip it into positive coords
			rcBounds *= *tz;
			if ( rcBounds.m_Right > widest )
				widest = ::Round( rcBounds.m_Right );
			if ( rcBounds.m_Bottom > tallest )
				tallest = ::Round( rcBounds.m_Bottom );
			++tz;
			++rz;
		}
		if ( IsHorizontal() )
			length = tallest;
		else
			length = widest;
	}
#endif
#if 1
	//attempt the 3rd
	if ( m_DistortEffect != kFollowPath )
	{
		if ( IsHorizontal() )
			length = ( m_BoundingBox.Height() / m_NrLines );
		else
			length = ( m_BoundingBox.Width() / m_NrLines );
	}
	else
	{
		rcBounds = *(m_BaseStringBoundingBoxes.Start());
		rcBounds.Offset( RRealSize( -rcBounds.m_Left, -rcBounds.m_Top ) );		// zero base rect
		if ( rcBounds.m_Bottom < 0 )
			rcBounds.m_Bottom = -rcBounds.m_Bottom;										// flip it into positive coords
		rcBounds *= *(m_LineScaleTransforms.Start());
		if ( IsHorizontal() )
			length = rcBounds.Height();
		else
			length = rcBounds.Width();
	}
#endif
	maxPenWeight = CalcMaxPenWidthBasedOnOutlineEffect( m_OutlineEffect, m_LineWeight );
	// compute transforms for projection or extrusion effect
	m_ProjectionTransforms.Empty();
	m_ProjectionTransforms.InsertAtEnd( transform );									// 1st stage starts off as identity
	m_ExtrusionTransforms.Empty();
	switch ( m_ProjectionEffect )
	{
		case kNoProjection :
			break;
		case kSolidExtrusion :
		case kIlluminateExtrusion :
			TpsAssert( ( m_NrProjectionStages == 1 ), "Bad projection data." );
			TpsAssert( ( m_ProjectionAngle > -360 && m_ProjectionAngle < 360 ), "Bad projection data." );

			depth = length * m_ProjectionDepth;
			//	depth += ::LogicalUnitToPrinterPoint( maxPenWeight ) / 2;				// only 1/2 of a pen width goes out
			depth += maxPenWeight / 40;		// undisguised magic calc for Leapfrog  -b0b- 1/6/98
			
			cosAngle = cos( ::DegreesToRadians( m_ProjectionAngle ) );
			sinAngle = sin( ::DegreesToRadians( m_ProjectionAngle ) );
			pt.m_x = depth * cosAngle;
			pt.m_y = depth * sinAngle;
			transform.MakeIdentity();
			transform.PreTranslate( pt.m_x, pt.m_y );
			m_ProjectionTransforms.InsertAtEnd( transform ); 
			nrStages = CalcNrGradientExtrusionBands( transform, FALSE );

			depth = length * m_ProjectionDepth;
			// depth += ::LogicalUnitToPrinterPoint( maxPenWeight ) / 2;				// only 1/2 of a pen width goes out
			depth += maxPenWeight / 40;		// undisguised magic calc for Leapfrog  -b0b- 1/6/98

			distance = depth / (YRealDimension)nrStages;
			increm = distance;
			cosAngle = cos( ::DegreesToRadians( m_ProjectionAngle ) );
			sinAngle = sin( ::DegreesToRadians( m_ProjectionAngle ) );
			for ( i = 0; i < nrStages; i++ )
			{
				pt.m_x = distance * cosAngle;
				pt.m_y = distance * sinAngle;
				transform.MakeIdentity();
				transform.PreTranslate( pt.m_x, pt.m_y );
				m_ExtrusionTransforms.InsertAtEnd( transform ); 
				distance += increm;
			}
			break;
		case kSimpleStack :
			TpsAssert( ( m_ProjectionAngle > -360 && m_ProjectionAngle < 360 ), "Bad projection data." );

			depth = length * m_ProjectionDepth;
			// depth += ::LogicalUnitToPrinterPoint( maxPenWeight ) / 2;				// only 1/2 of a pen width goes out
			depth += maxPenWeight / 40;		// undisguised magic calc for Leapfrog  -b0b- 1/6/98

			distance = depth / (YRealDimension)m_NrProjectionStages;
			increm = distance;
			cosAngle = cos( ::DegreesToRadians( m_ProjectionAngle ) );
			sinAngle = sin( ::DegreesToRadians( m_ProjectionAngle ) );
			for ( i = 0; i < m_NrProjectionStages; i++ )
			{
				pt.m_x = distance * cosAngle;
				pt.m_y = distance * sinAngle;
				transform.MakeIdentity();
				transform.PreTranslate( pt.m_x, pt.m_y );
				m_ProjectionTransforms.InsertAtEnd( transform ); 
				distance += increm;
			}
			break;
		case kSpinStack :
			TpsAssert( ( m_ProjectionAngle > -360 && m_ProjectionAngle < 360 ), "Bad projection data." );
			pt1.m_x = m_BoundingBox.m_Left;													// this would work better around the baseline
			pt1.m_y = m_BoundingBox.m_Top;													// but this is all I've got 
			angle = m_ProjectionAngle / (YRealDimension)m_NrProjectionStages;
			increm = angle;
			for ( i = 0; i < m_NrProjectionStages; i++ )
			{
				transform.MakeIdentity();
				transform.PreRotateAboutPoint( pt1.m_x, pt1.m_y, ::DegreesToRadians( angle ) );
				m_ProjectionTransforms.InsertAtEnd( transform ); 
				angle += increm;
			}
			break;
		case kPseudoPerspectiveExtrusion :
			TpsAssert( ( m_NrProjectionStages == 1 ), "Bad projection data." );
			TpsAssert( ( m_ProjectionAngle > -360 && m_ProjectionAngle < 360 ), "Bad projection data." );
			// this is scaling around the center of the graphic
			// which make the projection angle look really small
			// but anything else is inconsistant and looks really weird
#if 0
			if ( m_VanishingPoint.m_x > 0 )
				pt1.m_x = m_BoundingBox.m_Right;
			else if ( m_VanishingPoint.m_x < 0 )
				pt1.m_x = m_BoundingBox.m_Left;
			else
				pt1.m_x = ( m_BoundingBox.Width() / 2.0 ) + m_BoundingBox.m_Left;
			if ( m_VanishingPoint.m_y < 0 )
				pt1.m_y = m_BoundingBox.m_Top;
			else if ( m_VanishingPoint.m_y > 0 )											// y coordinates inverted
				pt1.m_y = m_BoundingBox.m_Bottom;
			else
				pt1.m_y = ( m_BoundingBox.Height() / 2.0 ) + m_BoundingBox.m_Top;
#else
			pt1.m_x = ( m_BoundingBox.Width() / 2.0 ) + m_BoundingBox.m_Left;
			pt1.m_y = ( m_BoundingBox.Height() / 2.0 ) + m_BoundingBox.m_Top;
#endif
			depth = length * m_ProjectionDepth;
			// depth += ::LogicalUnitToPrinterPoint( maxPenWeight ) / 2;				// only 1/2 of a pen width goes out
			depth += maxPenWeight / 40;		// undisguised magic calc for Leapfrog  -b0b- 1/6/98
			
			if ( depth > length )
				depth = length;
			cosAngle = cos( ::DegreesToRadians( m_ProjectionAngle ) );
			sinAngle = sin( ::DegreesToRadians( m_ProjectionAngle ) );
			pt.m_x = depth * cosAngle;
			pt.m_y = depth * sinAngle;
			scale = 1.0 - ( depth / length );
			transform.MakeIdentity();
			transform.PreTranslate( pt.m_x, pt.m_y );
			transform.PreScaleAboutPoint( pt1.m_x, pt1.m_y, scale, scale );
			m_ProjectionTransforms.InsertAtEnd( transform ); 
			nrStages = CalcNrGradientExtrusionBands( transform, TRUE );

			depth = length * m_ProjectionDepth;
			// depth += ::LogicalUnitToPrinterPoint( maxPenWeight ) / 2;				// only 1/2 of a pen width goes out
			depth += maxPenWeight / 40;		// undisguised magic calc for Leapfrog  -b0b- 1/6/98

			if ( depth > length )
				depth = length;
 			distance = depth / (YRealDimension)nrStages;
			increm = distance;
			cosAngle = cos( ::DegreesToRadians( m_ProjectionAngle ) );
			sinAngle = sin( ::DegreesToRadians( m_ProjectionAngle ) );
			for ( i = 0; i < nrStages; i++ )
			{
				pt.m_x = distance * cosAngle;
				pt.m_y = distance * sinAngle;
				scale = 1.0 - ( distance / length );
				transform.MakeIdentity();
				transform.PreTranslate( pt.m_x, pt.m_y );
				transform.PreScaleAboutPoint( pt1.m_x, pt1.m_y, scale, scale );
				m_ExtrusionTransforms.InsertAtEnd( transform ); 
				distance += increm;
			}
			break;
		case kPseudoPerspectiveStack :
			TpsAssert( ( m_ProjectionAngle > -360 && m_ProjectionAngle < 360 ), "Bad projection data." );
			// this is scaling around the center of the graphic
			// which make the projection angle look really small
			// but anything else is inconsistant and looks really weird
#if 0
			if ( m_VanishingPoint.m_x > 0 )
				pt1.m_x = m_BoundingBox.m_Right;
			else if ( m_VanishingPoint.m_x < 0 )
				pt1.m_x = m_BoundingBox.m_Left;
			else
				pt1.m_x = ( m_BoundingBox.Width() / 2.0 ) + m_BoundingBox.m_Left;
			if ( m_VanishingPoint.m_y < 0 )
				pt1.m_y = m_BoundingBox.m_Top;
			else if ( m_VanishingPoint.m_y > 0 )											// y coordinates inverted
				pt1.m_y = m_BoundingBox.m_Bottom;
			else
				pt1.m_y = ( m_BoundingBox.Height() / 2.0 ) + m_BoundingBox.m_Top;
#else
			pt1.m_x = ( m_BoundingBox.Width() / 2.0 ) + m_BoundingBox.m_Left;
			pt1.m_y = ( m_BoundingBox.Height() / 2.0 ) + m_BoundingBox.m_Top;
#endif
			depth = length * m_ProjectionDepth;
			// depth += ::LogicalUnitToPrinterPoint( maxPenWeight ) / 2;				// only 1/2 of a pen width goes out
			depth += maxPenWeight / 40;		// undisguised magic calc for Leapfrog  -b0b- 1/6/98

 			distance = depth / (YRealDimension)m_NrProjectionStages;
			increm = distance;
			cosAngle = cos( ::DegreesToRadians( m_ProjectionAngle ) );
			sinAngle = sin( ::DegreesToRadians( m_ProjectionAngle ) );
			for ( i = 0; i < m_NrProjectionStages; i++ )
			{
				pt.m_x = distance * cosAngle;
				pt.m_y = distance * sinAngle;
				scale = 1.0 - ( distance / length );
				transform.MakeIdentity();
				transform.PreTranslate( pt.m_x, pt.m_y );
				transform.PreScaleAboutPoint( pt1.m_x, pt1.m_y, scale, scale );
				m_ProjectionTransforms.InsertAtEnd( transform ); 
				distance += increm;
			}
			break;
		default :
			TpsAssertAlways( "Unrecognized projection effect type." );
			break;
	}

#if 0 
	// 06/01/98 Lance - Shadows no longer supported in headline
	// component, as they are now supported at the component level.

	// compute transform for shadow effect
	switch ( m_ShadowEffect )
	{
		case kNoShadow :
			break;
		case kDropShadow :
		case kSoftDropShadow :
			TpsAssert( ( m_ShadowAngle > -360 && m_ShadowAngle < 360 ), "Bad shadow data." );

			depth = ( length * m_ShadowDepth );
			// depth += ::LogicalUnitToPrinterPoint( maxPenWeight ) / 2;				// only 1/2 of a pen width goes out
			depth += maxPenWeight / 40;		// undisguised magic calc for Leapfrog  -b0b- 1/6/98

			pt.m_x = depth * cos( ::DegreesToRadians( m_ShadowAngle ) );
			pt.m_y = depth * sin( ::DegreesToRadians( m_ShadowAngle ) );
			transform.MakeIdentity();
			transform.PreTranslate( pt.m_x, pt.m_y );
			m_ProjectionTransforms.InsertAtEnd( transform ); 
			break;
		case kCastShadow :
		case kSoftCastShadow :
			TpsAssert( ( m_ShadowAngle > -360 && m_ShadowAngle < 360 ), "Bad shadow data." );
			TpsAssert( ( m_ShadowSlope > -1001 && m_ShadowSlope < 1001 ), "Bad shadow data." );
			transform.MakeIdentity();
			if ( m_ShadowSlope != 0 )
				transform.PreXShearAboutPoint( 0.0, m_BoundingBox.m_Top, m_ShadowSlope );
			if ( ( m_ShadowAngle > 180 && m_ShadowAngle < 360 ) ||
				  ( m_ShadowAngle < 0 && m_ShadowAngle > -180 ) )
				transform.PreScaleAboutPoint( m_BoundingBox.m_Left, m_BoundingBox.m_Top, 1.0, -1.0 );
			m_ProjectionTransforms.InsertAtEnd( transform ); 
			break;
		default :
			TpsAssertAlways( "Unrecognized shadow effect type." );
			break;
	}
#endif

	// adjust base outline transform so it and all effects are within output bounding rect
	transform = *(m_ProjectionTransforms.Start());
	TpsAssert( ( transform == R2dTransform() ), "Initial transform not identity");
	rcOutline = m_OutlineRect;
	rcBounds = CalcBoundingBoxWithEffects( transform );
	
	// Insetting the bounds rect by one will insure that
	// any outline rect with points that are greater then or 
	// EQUAL to the original will not be re-sized.
	RRealRect rcTestRect( rcBounds ) ;
	rcTestRect.Inset( RRealSize( 1, 1 ) ) ; 

	if ( !rcOutline.IsEmpty() /*&& ( rcOutline != rcBounds )*/ && !rcOutline.RectInRect( rcTestRect ) )
	{
		xScale = yScale = 1;
		// scale to fit within output rect
		if ( rcBounds.Width() > rcOutline.Width() || rcBounds.Height() > rcOutline.Height() )
		{
			xScale = rcOutline.Width() / ( rcBounds.Width() + 1 );
			yScale = rcOutline.Height() / ( rcBounds.Height() + 1 );
			if ( !m_DistortGraphic )
				xScale = yScale = ::Min( xScale, yScale );
			transform.PostScaleAboutPoint( m_BoundingBox.m_Left, m_BoundingBox.m_Top, xScale, yScale );
			rcBounds = CalcBoundingBoxWithEffects( transform );
		}
		// nudge to draw within output rect
		transform.MakeIdentity();
		pt1.m_x = pt1.m_y = 0;
		if ( rcBounds.m_Left < rcOutline.m_Left )
			pt1.m_x = rcOutline.m_Left - rcBounds.m_Left;
		else if ( rcBounds.m_Right > rcOutline.m_Right )
			pt1.m_x = -static_cast<YRealDimension>( rcBounds.m_Right - rcOutline.m_Right );
		if ( rcBounds.m_Top < rcOutline.m_Top )
			pt1.m_y = rcOutline.m_Top - rcBounds.m_Top;
		else if ( rcBounds.m_Bottom > rcOutline.m_Bottom )
			pt1.m_y = -static_cast<YRealDimension>( rcBounds.m_Bottom - rcOutline.m_Bottom );
		transform.PostScaleAboutPoint( m_BoundingBox.m_Left, m_BoundingBox.m_Top, xScale, yScale );
		transform.PostTranslate( pt1.m_x, pt1.m_y );				// scale 1st, otherwise translate is scaled
		*(m_ProjectionTransforms.Start()) = transform;
	}
}

// ****************************************************************************
//
//  Function Name:	RHeadlineGraphic::CalcBoundingBoxWithEffects( )
//
//  Description:		Compute bounding box of graphic taking into account the effects
//
//  Returns:			RIntRect
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect RHeadlineGraphic::CalcBoundingBoxWithEffects( const R2dTransform& baseXform ) const
{
R2dTransform					xform;
RRealRect						rcBounds = m_BoundingBox * baseXform;
RRealRect						rcTemp;
RRealPoint						pt;
R2dTransformArrayIterator	tz = m_ProjectionTransforms.Start(); 

	// for the TrimWhiteSpaceCall for Banners
	//	which do not have any projection transforms (yet...)
	if ( m_ProjectionTransforms.Count() == 0 )
		return RIntRect( rcBounds );
	// always goes thru loop at least once 
	// 'cause 1st projection transform is always the base transform
	// which should be the identity
	TpsAssert( ( m_ProjectionTransforms.Count() > 0 ), "No projection transform data." );
//	TpsAssert( ( *tz == R2dTransform() ), "Initial transform not identity");
	while ( tz != m_ProjectionTransforms.End() )
	{
		xform = *tz * baseXform;
		rcTemp = m_BoundingBox * xform;
		rcBounds.AddPointToRect( rcTemp.m_TopLeft );
		rcBounds.AddPointToRect( rcTemp.m_BottomRight );
		pt.m_x = m_BoundingBox.m_Left;							// in case the graphic was flipped
		pt.m_y = m_BoundingBox.m_Bottom;
		pt *= xform;
		rcBounds.AddPointToRect( pt );
		pt.m_x = m_BoundingBox.m_Right;							// in case the graphic was flipped
		pt.m_y = m_BoundingBox.m_Top;
		pt *= xform;
		rcBounds.AddPointToRect( pt );
		++tz;
	}
	// take into account the spread of a soft drop shadow
	if ( m_ShadowEffect == kSoftDropShadow )
	{
		YIntDimension	spread = CalcMaxPenWidthBasedOnOutlineEffect( kShadowedOutline, m_LineWeight );
		spread -= CalcMaxPenWidthBasedOnOutlineEffect( kHairlineDoubleOutline, m_LineWeight );
		spread = ::LogicalUnitToPrinterPoint( spread ) / 2;		// only 1/2 of a pen width goes out
		RIntSize	delta( spread, spread );
	//	delta.m_dy /= 2;														// REVEIW RAH still having problems with margins based on line weights
		rcTemp.Inflate( RRealSize( delta ) );
		rcBounds.AddPointToRect( rcTemp.m_TopLeft );
		rcBounds.AddPointToRect( rcTemp.m_BottomRight );
	}
	return RIntRect( rcBounds );
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::BuildProjectionExtrusion()
//
// Description:		Create 
//
// Returns:				Nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RHeadlineGraphic::BuildProjectionExtrusion( )
{
R2dTransformArrayIterator		tz = m_ProjectionTransforms.Start(); 
EPathOperatorArrayIterator 	opz = m_PathOperatorsStream.Start();
YPointCountArrayIterator 		cntz = m_PointCountsStream.Start();
RRealPointArrayIterator			ptz = m_OriginalPointsStream.Start();
EPathOperator						pathOperator;
YPointCount							pointCount;
YPathOperatorRepCount			repCount;
RRealPoint							pts[16];
RRealPoint							ptLast, ptStart;
R2dTransform						baseXform, projXform, identXform, xform;
RPath									path;
RRealRect							rcBounds, rcBoundingBox = m_OutlineRect;
int									i;

	// validations/initializations
	delete m_pExtrusionGraphic;
	m_pExtrusionGraphic = NULL;
	if ( m_ProjectionEffect != kSolidExtrusion && 
		  m_ProjectionEffect != kIlluminateExtrusion && 
		  m_ProjectionEffect != kPseudoPerspectiveExtrusion )
		return;
	TpsAssert( ( m_NrProjectionStages == 1 ), "Bad projection data." );
	TpsAssert( ( m_ProjectionTransforms.Count() > 1 ), "Bad projection data." );
	// setup graphic to contain extrusion
	m_pExtrusionGraphic = new RSingleGraphic;
	m_pExtrusionGraphic->Undefine( RSolidColor( kBlack), RSolidColor( kBlack) );
	rcBoundingBox.Inflate( RRealSize( kDefaultMargin, kDefaultMargin ) );						// slack
	TpsAssert( ( rcBoundingBox.m_Left >= 0 ), "Bad projection bounding box." );
	TpsAssert( ( rcBoundingBox.m_Top >= 0 ), "Bad projection bounding box." );
	TpsAssert( ( rcBoundingBox.m_Right <= m_GraphicSize.m_dx ), "Bad projection bounding box." );
	TpsAssert( ( rcBoundingBox.m_Bottom <= m_GraphicSize.m_dy ), "Bad projection bounding box." );
	// add extreme point to define graphic size to be the same as the base outline
	identXform.MakeIdentity();
	rcBounds.m_Left	= m_GraphicSize.m_dx;
	rcBounds.m_Right	= m_GraphicSize.m_dx;
	rcBounds.m_Top		= m_GraphicSize.m_dy;
	rcBounds.m_Bottom	= m_GraphicSize.m_dy;

	RSingleGraphic&	gr	= *m_pExtrusionGraphic;

	gr.m_fMonochrome	= TRUE;
	gr.m_LastTransform.MakeIdentity();

	EOperationArrayIterator	itrOperator = gr.m_OperationsStream.End();
	if( itrOperator != gr.m_OperationsStream.Start()  && *(itrOperator-1)==kEndGraphic )
		gr.m_OperationsStream.RemoveAt( --itrOperator );
	gr.m_OperationsStream.InsertAtEnd( kPath );
	gr.m_FillMethodsStream.InsertAtEnd( kStroked );
	gr.m_PointCountsStream.InsertAtEnd( 5 );
	gr.m_PathOperatorsStream.InsertAtEnd( kMoveTo );
	gr.m_PathOperatorsStream.InsertAtEnd( kLine );
	gr.m_PathOperatorsStream.InsertAtEnd( (EPathOperator)4 );
	gr.m_PathOperatorsStream.InsertAtEnd( kClose );
	gr.m_PathOperatorsStream.InsertAtEnd( kEnd );
	gr.m_OriginalPointsStream.InsertAtEnd( RRealPoint(m_GraphicSize.m_dx,m_GraphicSize.m_dy) );
	gr.m_OriginalPointsStream.InsertAtEnd( RRealPoint(m_GraphicSize.m_dx,m_GraphicSize.m_dy) );
	gr.m_OriginalPointsStream.InsertAtEnd( RRealPoint(m_GraphicSize.m_dx,m_GraphicSize.m_dy) );
	gr.m_OriginalPointsStream.InsertAtEnd( RRealPoint(m_GraphicSize.m_dx,m_GraphicSize.m_dy) );
	gr.m_OriginalPointsStream.InsertAtEnd( RRealPoint(m_GraphicSize.m_dx,m_GraphicSize.m_dy) );


	// setup transforms
	baseXform = *tz;															// base transform
	++tz;
	projXform = *tz;															// projection transform
	// loop thru paths 
	while ( cntz != m_PointCountsStream.End() )
	{
		pointCount = *cntz;
		// skip over empty paths (they're just markers)
		if ( RPath::IsEmpty( &(*opz), &(*ptz) ) )
		{
			gr.m_OperationsStream.InsertAtEnd( kPath );
			gr.m_FillMethodsStream.InsertAtEnd( kStroked );
			gr.m_PointCountsStream.InsertAtEnd( 5 );
			gr.m_PathOperatorsStream.InsertAtEnd( kMoveTo );
			gr.m_PathOperatorsStream.InsertAtEnd( kLine );
			gr.m_PathOperatorsStream.InsertAtEnd( (EPathOperator)4 );
			gr.m_PathOperatorsStream.InsertAtEnd( kClose );
			gr.m_PathOperatorsStream.InsertAtEnd( kEnd );
			gr.m_OriginalPointsStream.InsertAtEnd( RRealPoint((*ptz).m_x,(*ptz).m_y) );
			gr.m_OriginalPointsStream.InsertAtEnd( RRealPoint((*ptz).m_x,(*ptz).m_y) );
			gr.m_OriginalPointsStream.InsertAtEnd( RRealPoint((*ptz).m_x,(*ptz).m_y) );
			gr.m_OriginalPointsStream.InsertAtEnd( RRealPoint((*ptz).m_x,(*ptz).m_y) );
			gr.m_OriginalPointsStream.InsertAtEnd( RRealPoint((*ptz).m_x,(*ptz).m_y) );
			while ( ( pathOperator = *opz ) != kEnd )
			{
				if ( pathOperator == kLine || pathOperator == kBezier || pathOperator == kQuadraticSpline )
					++opz;
				++opz;
			}
			ptz += pointCount;
		}
	// create patches for each segment
		while ( ( pathOperator = *opz ) != kEnd )
		{
			++opz;
			switch ( pathOperator )
			{
				case kMoveTo :
					ptLast = *ptz;
					ptStart = ptLast;
					++ptz;
					break;
				case kClose :
					if ( ptLast != ptStart )
					{
						gr.m_OperationsStream.InsertAtEnd( kPath );
						gr.m_FillMethodsStream.InsertAtEnd( kStroked );
						gr.m_PointCountsStream.InsertAtEnd( 5 );
						gr.m_PathOperatorsStream.InsertAtEnd( kMoveTo );
						gr.m_PathOperatorsStream.InsertAtEnd( kLine );
						gr.m_PathOperatorsStream.InsertAtEnd( (EPathOperator)4 );
						gr.m_PathOperatorsStream.InsertAtEnd( kClose );
						gr.m_PathOperatorsStream.InsertAtEnd( kEnd );
						gr.m_OriginalPointsStream.InsertAtEnd( ptLast*baseXform );
						gr.m_OriginalPointsStream.InsertAtEnd( ptStart*baseXform );
						gr.m_OriginalPointsStream.InsertAtEnd( ptStart*projXform*baseXform );
						gr.m_OriginalPointsStream.InsertAtEnd( ptLast*projXform*baseXform );
						gr.m_OriginalPointsStream.InsertAtEnd( ptLast*baseXform );
					}
					ptLast = ptStart;
					break;
				case kLine :
					pointCount = (YPathOperatorRepCount)*opz;
					for ( i = 0; i < pointCount; i++ )
					{
						gr.m_OperationsStream.InsertAtEnd( kPath );
						gr.m_FillMethodsStream.InsertAtEnd( kStroked );
						gr.m_PointCountsStream.InsertAtEnd( 5 );
						gr.m_PathOperatorsStream.InsertAtEnd( kMoveTo );
						gr.m_PathOperatorsStream.InsertAtEnd( kLine );
						gr.m_PathOperatorsStream.InsertAtEnd( (EPathOperator)4 );
						gr.m_PathOperatorsStream.InsertAtEnd( kClose );
						gr.m_PathOperatorsStream.InsertAtEnd( kEnd );
						gr.m_OriginalPointsStream.InsertAtEnd( ptLast*baseXform );
						gr.m_OriginalPointsStream.InsertAtEnd( (*ptz)*baseXform );
						gr.m_OriginalPointsStream.InsertAtEnd( (*ptz)*projXform*baseXform );
						gr.m_OriginalPointsStream.InsertAtEnd( ptLast*projXform*baseXform );
						gr.m_OriginalPointsStream.InsertAtEnd( ptLast*baseXform );
						ptLast = *ptz;
						++ptz;
					}
					++opz;
					break;
				case kQuadraticSpline :
					repCount = (YPathOperatorRepCount)*opz++;
					for ( i = 0; i < repCount; i++ )
					{
						//
						// copy the quadratic spline points
						pts[0] = ptLast;
						pts[1] = *ptz++;
						pts[2] = ptLast = *ptz++;
						//
						// create a bezier representing the quadratic spline
						RRealPoint	ptBez[4];
						::CreateBezierFromSpline( &pts[ 0 ], &ptBez[ 0 ] );
						//
						// Extrude this segment and merge it in...
						::AppendExtrudedBezier( ptBez, projXform, baseXform, gr.m_OperationsStream, gr.m_FillMethodsStream, gr.m_PathOperatorsStream, gr.m_OriginalPointsStream, gr.m_PointCountsStream );
					}
					break;
				case kBezier :
					repCount = (YPathOperatorRepCount)*opz++;
					for ( i = 0; i < repCount; i++ )
					{
						RRealPoint	ptBez[4];
						ptBez[0] = ptLast;
						ptBez[1] = *ptz++;
						ptBez[2] = *ptz++;
						ptBez[3] = ptLast = *ptz++;
						//
						// Extrude this segment and merge it in...
						::AppendExtrudedBezier( ptBez, projXform, baseXform, gr.m_OperationsStream, gr.m_FillMethodsStream, gr.m_PathOperatorsStream, gr.m_OriginalPointsStream, gr.m_PointCountsStream );
					}
					break;
				default :
					TpsAssertAlways( "Unrecognized Path operator." );
					break;
			}
		}
		++opz;
		++cntz;
	}
	gr.m_TransformedRealPointsStream = gr.m_OriginalPointsStream;
	gr.m_OperationsStream.InsertAtEnd( kEndGraphic );
	RRealPointArrayIterator	itrPoints	= gr.m_OriginalPointsStream.Start();
	RRealSize	rightbottom( YRealDimension(0), YRealDimension(0) );
	while( itrPoints != gr.m_OriginalPointsStream.End() )
		{
		RRealPoint	pt( *itrPoints );
		if( pt.m_x > rightbottom.m_dx )
			rightbottom.m_dx	= pt.m_x;
		if( pt.m_y > rightbottom.m_dy )
			rightbottom.m_dy	= pt.m_y;
		++itrPoints;
		}
	gr.m_GraphicSize	= rightbottom;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineGraphic::CalcNrGradientExtrusionBands( )
//
//  Description:		Compute the number of bands to be use in rendering
//                   a gradient extrusion.
//
//  Returns:			YCounter
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCounter RHeadlineGraphic::CalcNrGradientExtrusionBands( const R2dTransform& transform, BOOLEAN fPerspective ) const
{
YRealDimension			width;
YCounter					nrBands = 2;
YRealDimension			xScale, yScale;
YAngle					rotation;
RRealPoint				pt, pt1;
//YRealDimension			xOffset, yOffset;

//	pt.m_x = ( ( m_VanishingPoint.m_x > 0 ) ? 0 : m_GraphicSize.m_dx );
//	pt.m_y = ( ( m_VanishingPoint.m_y > 0 ) ? 0 : m_GraphicSize.m_dy );
	pt.m_x = ( m_BoundingBox.Width() / 2.0 ) + m_BoundingBox.m_Left;
	pt.m_y = ( m_BoundingBox.Height() / 2.0 ) + m_BoundingBox.m_Top;
//	transform.GetTranslation( xOffset, yOffset );
//	transform.MakeIdentity( );
//	transform.PreTranslate( xOffset, yOffset );
	pt1 = pt;
	pt1 *= transform;
//	pt1.m_x += xOffset;
//	pt1.m_y += yOffset;
	m_EmSquareTransform.Decompose( rotation, xScale, yScale );
	width = CalcLogicalPenWidth( kSimpleStroke, kHairLine ) * ( 12.0 / m_FontPointSize );	// based on a 12pt font
	width = ::Max( width, m_MinGradBandWidth );
	nrBands = ::Max( ( ::Round( pt1.Distance( pt ) / width ) ), 2 ) / 2;
	nrBands *= ::Max( ::Round( ::Max( xScale, yScale ) ), 2 );
	transform.Decompose( rotation, xScale, yScale );
	// this really should have been done based on m_ProjectionDepth
	// but I just couldn't figure out how
	if ( nrBands < 20 ) 
		nrBands *= 2;
	else if ( nrBands > 50 ) 
	{
		if ( fPerspective ) 
		{
			TpsAssert( !::AreFloatsEqual( xScale, 1.0 ), "Really weird psuedo-perspective scale." );
			TpsAssert( !::AreFloatsEqual( yScale, 1.0 ), "Really weird psuedo-perspective scale." );
			nrBands /= 2;
		}
	}

	return nrBands;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineGraphic::CalcEmDebossOffsetScale( )
//
//  Description:		
//
//  Returns:			YScaleFactor
//
//  Exceptions:		None
//
// ****************************************************************************
//
YScaleFactor RHeadlineGraphic::CalcEmDebossOffsetScale( const R2dTransform& transform, const RRealSize& size, YAngle& rotation ) const
{
//R2dTransform		baseXform;
R2dTransform		outXform;
YAngle				outRotation, /*baseRotation,*/ emRotation;
YScaleFactor		outXScale, outYScale;
//YScaleFactor		baseXScale, baseYScale;
YScaleFactor		emXScale, emYScale;//, yScale;
RIntRect				rcBounds, rcEmFont;

	outXform = CalcRenderXform( size, transform );
	outXform.Decompose( outRotation, outXScale, outYScale );
	rotation = outRotation;

//	if ( m_ProjectionTransforms.Count() == 0 )
//		baseXform.MakeIdentity();
//	else
//		baseXform = *(m_ProjectionTransforms.Start());
	
//	baseXform.Decompose( baseRotation, baseXScale, baseYScale );
//	TpsAssert( ::AreFloatsEqual( baseXScale, 1.0 ), "Emboss/Deboss outlines should not have any depth effects." );
//	TpsAssert( ::AreFloatsEqual( baseYScale, 1.0 ), "Emboss/Deboss outlines should not have any depth effects." );
	
	if (m_DistortGraphic)
	{
		YRealDimension largest(0), proportion(0);

		for ( int k = 0; ( k < 3 && k < m_NrLines ); k++ )
		{
			if ( m_LineScaleProportions[k] > largest )
			{
				largest = m_LineScaleProportions[k];
			}

			proportion += m_LineScaleProportions[k];
		}

		YRealDimension height = (largest / proportion) * m_GraphicSize.m_dy;
	
		if (m_Escape >= kTop2BottomLeft2Right && m_Escape <= kBottom2TopRight2Left)
			height = (largest / proportion) * m_ConstructedSize.m_dx;

		m_pFont->GetMaxCharBounds( &rcEmFont );
		rcEmFont.Offset( RIntSize( -rcEmFont.m_Left, -rcEmFont.m_Top ) );			// zero base rect
		if ( rcEmFont.m_Bottom < 0 )
			rcEmFont.m_Bottom = -rcEmFont.m_Bottom;											// flip it into positive coords

		emYScale = height / rcEmFont.Height();
	}
	else
	{
		m_EmSquareTransform.Decompose( emRotation, emXScale, emYScale );
	}

//	rcBounds = RIntRect( RIntSize( GetDefaultHeadlineSize( TRUE ) ) );
//	rcBounds.Inflate( RIntSize( ( kDefaultMargin * 4 ), ( kDefaultMargin * 4 ) ) );	// slop
//	yScale = (YRealDimension)rcBounds.Height() / (YRealDimension)rcEmFont.Height();
//	emYScale = ::Min( emYScale, yScale );

	return( outYScale * emYScale );// * baseYScale );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineGraphic::GetStringBounds( )
//
//  Description:		Return a rectangle that would bound the given string of characters
//
//  Returns:			RIntRect
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RIntRect RHeadlineGraphic::GetStringBounds( const RMBCString& string ) const
{
RIntRect					rcString( 0, 0, 0, 0 );
YFontSize				width, comp;
RCharacter				character;
RMBCStringIterator	cz;
RPath*					pPath;
YCounter					length = string.GetStringLength();

	if ( !string.IsEmpty() )
		if ( IsHorizontal() )
		{
			m_pFont->GetStringBounds( string, &rcString, m_fUseKerning );

			// kerning and compensation are not mutually exclusive
			if ( m_Compensate != kRegular )
			{
				comp = length * m_Compensation;
				if ( m_Compensate == kTight )								 
					rcString.m_Right -= comp;
				else if ( m_Compensate == kLoose )
					rcString.m_Right += comp;
			}
			if ( m_DistortGraphic || m_DistortEffect == kFollowPath )
			{
				RIntRect		rcBounds, rcTemp( 0, 0, 0, 0 );
				cz = string.Start(); 
				while ( cz != string.End() )
				{
					if ( ( pPath = m_pFont->GetGlyphPath( *cz ) ) != NULL )
					{
						rcBounds = pPath->GetBoundingRect();
						if ( rcTemp.IsEmpty() )
							rcTemp = rcBounds;
						else
							rcTemp.Union( rcTemp, rcBounds );
					}
					++cz;
				}
				if ( !rcTemp.IsEmpty() )
				{
					// distorted graphics have a minimum line height of the font x height
					if ( ( pPath = m_pFont->GetGlyphPath( RCharacter( 'x' ) ) ) != NULL )
					{
						rcBounds = pPath->GetBoundingRect();
						rcTemp.Union( rcTemp, rcBounds );
					}
					rcString.m_Top = rcTemp.m_Bottom;			// glyphs coming out of the font are inverted
					rcString.m_Bottom = rcTemp.m_Top;
				}
				if ( m_DistortEffect == kFollowPath && m_PathType != kPathStraight )
				{
					YRealDimension	tighten, loosen;
					CalcFollowPathEscapementScaleFactors( tighten, loosen );

					YIntDimension yEmHeight = m_pFont->GetHeight() * 0.35;
					YIntDimension dxTighten = (1.0 - tighten) * yEmHeight;
					YIntDimension dxLoosen  = (loosen - 1.0) * yEmHeight;

					cz = string.Start(); 
					while ( cz != string.End() )
					{
						width = m_pFont->GetCharacterWidth( *cz );
						if ( m_Compensate == kTight )								 
							width -= m_Compensation;
						else if ( m_Compensate == kLoose )
							width += m_Compensation;
						if ( m_PathType == kPathConcave )
						{
							rcString.m_Right += dxLoosen;
						}
						else if ( m_PathType == kPathConvex )
						{
							rcString.m_Right -= dxTighten;
						}
						else if ( m_PathType == kPathCurved )
						{
							// REVEIW RAH I really should do something here but I have no idea what
						}
						++cz;
					}
					TpsAssert( ( rcString.m_Right > 0 ), "Bad bound box.");
				}
			}
		}
		else	/* vertical */
		{
			YFontDimensions	fontDimensions = m_pFont->GetDimensions();
			RIntRect				rcBounds;
			cz = string.Start(); 
			if ( m_DistortGraphic )
			{
				while ( cz != string.End() )
				{
					if ( ( pPath = m_pFont->GetGlyphPath( *cz ) ) == NULL )
						rcBounds = RIntRect( 0, 0, 0, 0 );
					else
						rcBounds = pPath->GetBoundingRect();
					if ( rcBounds.IsEmpty() )
						rcString.m_Bottom += fontDimensions.m_Ascent;					// space/empty characters only use ascent
					else
					{
						rcString.m_Bottom += rcBounds.Height() + ::PrinterPointToLogicalUnit( 2 );
						if ( rcBounds.Width() > rcString.Width() )
						{
							rcString.m_Left = rcBounds.m_Left;
							rcString.m_Right = rcBounds.m_Right;
						}
					}
					++cz;
				}
			}
			else
			{
				m_pFont->GetMaxCharBounds( &rcString );
				rcString.m_Top = 0;
				rcString.m_Bottom = ( fontDimensions.m_Ascent + fontDimensions.m_Descent ) * length;
				if ( m_fUseLeading )
					rcString.m_Bottom += fontDimensions.m_Leading * ( length - 1 );
				while ( cz != string.End() )
				{
					if ( ( pPath = m_pFont->GetGlyphPath( *cz ) ) == NULL )
						rcBounds = RIntRect( 0, 0, 0, 0 );
					else
						rcBounds = pPath->GetBoundingRect();
					if ( rcBounds.IsEmpty() )
						rcString.m_Bottom -= fontDimensions.m_Descent;					// space/empty characters only use ascent
					++cz;
				}
			}
		}
	return rcString;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::AdjustWhiteSpaceBasedOnEffects( )
//
// Description:		
//
// Returns:				RIntSize
//
// Exceptions:			None
//
// ****************************************************************************
//
RIntSize RHeadlineGraphic::AdjustWhiteSpaceBasedOnEffects( const RIntSize& whiteSpace ) const
{
RRealSize						delta( kDefaultMargin, kDefaultMargin ), size;
RIntSize							largest( whiteSpace );
R2dTransformArrayIterator	tz; 
int								k; 

	tz = m_LineScaleTransforms.Start(); 
	for ( k = 0; k < m_NrLines; k++ )
	{
		size = delta * (*tz);
		if ( size.m_dx > largest.m_dx )
			largest.m_dx = ::Round( size.m_dx );
		if ( size.m_dy > largest.m_dy )					// 1st line
			if ( k == 0 )
				largest.m_dy = ::Round( size.m_dy );
		++tz;
	}
	if ( size.m_dy > largest.m_dy )						// last line
		largest.m_dy = ::Round( size.m_dy );

	return largest;
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::IsMonochrome( )
//
// Description:		
//
// Returns:				boolean indicating if the graphic is a monochrome 
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::IsMonochrome( ) const
{
BOOLEAN		fMonochrome = FALSE;

#if 0
	switch ( m_OutlineEffect )
	{
		case kSimpleStroke :
			fMonochrome = TRUE;
			break;
		case kSimpleFill :
			if ( m_OutlineFillType == kColor )
				fMonochrome = TRUE;
			break;
		case kSimpleStrokeFill :
			if ( m_OutlineFillType == kColor )
				if ( m_OutlineStrokeColor == m_OutlineFillColor )
					fMonochrome = TRUE;
			break;
		case kHeavyOutline :
			if ( m_OutlineFillType == kColor )
				if ( m_OutlineStrokeColor == m_OutlineFillColor )
					fMonochrome = TRUE;
			break;
		case kDoubleOutline :
		case kHairlineDoubleOutline :
		case kDoubleHairlineOutline :
			if ( m_OutlineFillType == kColor )
				if ( m_OutlineStrokeColor == m_OutlineFillColor )
					if ( m_OutlineShadowColor == m_OutlineFillColor )
						fMonochrome = TRUE;
			break;
		case kTripleOutline :
			if ( m_OutlineFillType == kColor )
				if ( m_OutlineStrokeColor == m_OutlineFillColor )
					if ( m_OutlineShadowColor == m_OutlineFillColor )
						if ( m_OutlineHiliteColor == m_OutlineFillColor )
							fMonochrome = TRUE;
			break;
		case kEmbossedOutline :
		case kDebossedOutline :
			if ( m_OutlineFillType == kColor )
				if ( m_OutlineShadowColor == m_OutlineFillColor )
					if ( m_OutlineHiliteColor == m_OutlineFillColor )
						fMonochrome = TRUE;
			break;
		case kBlurredOutline :
		case kShadowedOutline :
		case kRoundedOutline :
		case kSoftEmbossedOutline :
		case kSoftDebossedOutline :
		case kGlintOutline :
			fMonochrome = FALSE;
			break;
		default :
			fMonochrome = FALSE;
			break;
	}
	switch ( m_ProjectionEffect )
	{
		case kNoProjection :
			break;
		case kSimpleStack :
		case kSpinStack :
		case kPseudoPerspectiveStack :
		case kSolidExtrusion :
		case kPseudoPerspectiveExtrusion :
		case kIlluminateExtrusion :
			fMonochrome = FALSE;
			break;
		default :
			fMonochrome = FALSE;
			break;
	}
	switch ( m_ShadowEffect )
	{
		case kNoShadow :
			break;
		case kDropShadow :
		case kSoftDropShadow :
		case kCastShadow :
		case kSoftCastShadow :
			fMonochrome = FALSE;
			break;
		default :
			fMonochrome = FALSE;
			break;
	}
	switch ( m_FrameEffect )
	{
		case kFrameNoFrame :
			break;
		case kSilhouette :
		case kEmboss :
		case kDeboss :
		case kSoftEmboss :
		case kSoftDeboss :
			fMonochrome = FALSE;
			break;
		default :
			fMonochrome = FALSE;
			break;
	}
#endif
	return fMonochrome;
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::UsesBeziers( )
//
// Description:		
//
// Returns:				Boolean indicating 
//
// Exceptions:			None
//
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::UsesBeziers( ) const
{
EPathOperatorArrayIterator 	opz = m_PathOperatorsStream.Start();
YPointCountArrayIterator 		cntz = m_PointCountsStream.Start();
EPathOperator						pathOperator;

	while ( cntz != m_PointCountsStream.End() )
	{
			while ( ( pathOperator = *opz ) != kEnd )
			{
				++opz;
				switch ( pathOperator )
				{
					case kMoveTo :
					case kClose :
						break;
					case kLine :
					case kQuadraticSpline :
						++opz;
						break;
					case kBezier :
						return TRUE;
						break;
					default :
						TpsAssertAlways( "Unrecognized Path operator." );
						break;
			}
		}
		++opz;
		++cntz;
	}
	return FALSE;
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcMaxPenWidthBasedOnOutlineEffect( )
//
// Description:		Recompute the white space based on the outline effects and 
//							margin using the line scale
//
// Returns:				YIntDimension
//
// Exceptions:			None
//
// ****************************************************************************
//
YIntDimension RHeadlineGraphic::CalcMaxPenWidthBasedOnOutlineEffect( EOutlineEffects effect, ELineWeight line ) const
{
YIntDimension		spread = 0;
RIntRect       	rcBounds;

	// Determine the Em height.
	//
	RIntRect rBounds;
	m_pFont->GetMaxCharBounds( &rBounds );
	rBounds.Offset( RIntSize( -rBounds.m_Left, -rBounds.m_Top ) );		// zero base rect
	if( rBounds.m_Bottom < 0 ) rBounds.m_Bottom = -rBounds.m_Bottom;	// flip it into positive coords

	YAngle rotation;
	R2dTransform identity;
	YScaleFactor scale = CalcEmDebossOffsetScale( identity, m_ConstructedSize, rotation );
	YRealDimension	yEmHeight = GetEmHeight(); //rBounds.Height() * scale;

	switch ( effect )
	{
		case kSimpleFill :
		case kGlintOutline :
		case kRoundedOutline :
			break;
		case kSimpleStroke :
		case kSimpleStrokeFill :
		case kSimpleFillStroke :
			if ( line == kHairLine )
				spread = 1;
			else
				spread = CalcLogicalPenWidth( effect, line );
			break;
		case kHeavyOutline :
		case kDoubleOutline :
			spread = ( CalcLogicalPenWidth( effect, line ) * 2 );
			break;
		case kHairlineDoubleOutline :
		case kDoubleHairlineOutline :
			spread = CalcLogicalPenWidth( effect, line ) + CalcLogicalPenWidth( effect, kHairLine );
			break;
		case kTripleOutline :
			spread = ( CalcLogicalPenWidth( effect, line ) * 3 );
			break;
		case kBlurredOutline :
		case kShadowedOutline :
//			spread = yEmHeight * 0.024;
//			spread = CalcLogicalPenWidth( effect, spread * 2 );
			spread = CalcLogicalPenWidth( effect, kThickLine );					// REVEIW RAH is this correct?
			m_pFont->GetMaxCharBounds( &rcBounds );
			rcBounds.Offset( RIntSize( -rcBounds.m_Left, -rcBounds.m_Top ) );	// zero base rect
			if ( rcBounds.m_Bottom < 0 )
				rcBounds.m_Bottom = -rcBounds.m_Bottom;								// flip it into positive coords
			spread = CalcLogicalPenWidth( effect, ::PrinterPointToLogicalUnit( ::Round( (YRealDimension)rcBounds.Height() * 0.012 ) ) );
			spread *= 12.0 / m_FontPointSize;											// based on a 12pt font
			spread += CalcLogicalPenWidth( effect, line ) + CalcLogicalPenWidth( effect, kHairLine );
			break;
		case kBeveledOutline:
			spread = yEmHeight * kPctBevelOffset + yEmHeight * 0.01;
			spread = CalcLogicalPenWidth( effect, spread * 2 );
//			spread = Max( int( yEmHeight * kPctBevelOffset + yEmHeight * 0.01 ), 20 );
//			spread = (yEmHeight * kPctBevelOffset) + (yEmHeight * 0.01);  // Offset + line weight
//			spread = CalcLogicalPenWidth( effect, spread * 2 );
			break;

		case kEmbossedOutline :
		case kDebossedOutline :
			// Determine the spread, keeping in mind that 
			// half of the blur goes inward.
			spread = yEmHeight * kPctEmbossOffset + yEmHeight * 0.012;
			spread = CalcLogicalPenWidth( effect, spread * 2 );
//			spread = CalcLogicalPenWidth( effect, kExtraThickLine );				// REVEIW RAH is this correct?
			break;
		case kSoftEmbossedOutline :
		case kSoftDebossedOutline :
			spread = CalcLogicalPenWidth( effect, kExtraThickLine );				// REVEIW RAH is this correct?
			break;
		default :
			TpsAssertAlways( "Unrecognized outline effect type." );
			break;
	}
	return spread;
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcMarginBasedOnEffects( )
//
// Description:		Recompute the white space based on the outline effects and 
//							margin using the line scale
//
// Returns:				RIntSize - the default margins plus 1/40th of the pen size
//
// Exceptions:			None
//
// Note: This function currently uses a magic fraction (1/40th) of the pen width.
//
// ****************************************************************************
//
RIntSize RHeadlineGraphic::CalcMarginBasedOnEffects( EOutlineEffects effect, ELineWeight line ) const
{
	YIntDimension		margin;

	// the following code created a small dimension in Print Shop and a large one in Leapfrog:
//	margin = ::LogicalUnitToPrinterPoint( CalcMaxPenWidthBasedOnOutlineEffect( effect, line ) );	
//	margin /= 2;											// only 1/2 of a pen width goes out

	// I've replaced it with code that creates the same magic "fudge factor" in both programs
	margin = CalcMaxPenWidthBasedOnOutlineEffect( effect, line );	
	margin /= 40;	// <-- magic number   -b0b- 9 Sept 1997

	margin += kDefaultMargin;

	return RIntSize( margin, margin );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcMarginBasedOnEffects( )
//
// Description:		Recompute the white space based on the outline effects and 
//							margin using the line scale
//
// Returns:				RIntSize
//
// Exceptions:			None
//
// ****************************************************************************
//
RIntSize RHeadlineGraphic::CalcMarginBasedOnEffects( ) const
{
	return CalcMarginBasedOnEffects( m_OutlineEffect, m_LineWeight );
}


// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcLogicalPenPoints( )
//
// Description:		Calculate the logical pen width in headline units based on
//								the line weight
//								a printer point
//
// Returns:				width of the pen in headline units
//
// Exceptions:			None
//
// ****************************************************************************
//
YPenWidth RHeadlineGraphic::CalcLogicalPenPoints( ELineWeight line ) const
{
	if (kHairLine == line)
		return ::PrinterPointToLogicalUnit( 1 );

	YFontDimensions& dim  = m_pFont->GetDimensions( );
	YRealDimension height =	::LogicalUnitToPrinterPoint( dim.m_Ascent * m_EmScale );
	height = height * kOutlineTransforms[line][0] + kOutlineTransforms[line][1];
	height *= 1.7;

	return ::Round( max( 1.0, height ) * ::PrinterPointToLogicalUnit( 1 ) );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcLogicalPenWidth( )
//
// Description:		Calculate the logical pen width in headline units based on
//								the line weight
//								a printer point
//								font em height
//
// Returns:				width of the pen in headline units
//
// Exceptions:			None
//
// ****************************************************************************
//
YPenWidth RHeadlineGraphic::CalcLogicalPenWidth( EOutlineEffects effect, ELineWeight line ) const
{
YIntDimension		pen = CalcLogicalPenPoints( line );

	return CalcLogicalPenWidth( effect, pen );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcLogicalPenWidth( )
//
// Description:		Calculate the logical pen width in headline units based on
//								the line weight
//								a printer point
//								font em height
//
// Returns:				width of the pen in headline units
//
// Exceptions:			None
//
// ****************************************************************************
//
YPenWidth RHeadlineGraphic::CalcLogicalPenWidth( EOutlineEffects effect, YIntDimension width ) const
{
YAngle							rotation;
YRealDimension					xScale;
YRealDimension					yScale;
YScaleFactor					scale = 2;								// compensate for what RSingleGraphic::CalcPenWidth() does late on

	TpsAssert( ( m_pFont != NULL ), "Undefined font." );
	TpsAssert( ( m_FontPointSize > 0 ), "Bad font." );
	TpsAssert( ( m_EmSquareTransform != R2dTransform() ), "Cannot calculate pen width without line scale." );
	if ( effect != kSimpleStroke ) 
		scale *= 2;															// filled outlines are drawn stroke then fill

	m_EmSquareTransform.Decompose( rotation, xScale, yScale );
	scale *= ( ( m_FontPointSize / 12.0 ) );			// based on a 12pt font

#if 0
	// fudge the y scale to keep the line weights reasonable 
	if ( m_DistortEffect == kWarpToEnvelop && m_NrLines == 1 )
		if ( m_LineWeight > kThinLine )
			if ( m_ScaleConstraint == kByVertical || m_ScaleConstraint == kByHorizontalAndVertical )
				scale *= 0.9;
	if ( IsVertical() )
		if ( m_ScaleConstraint == kByVertical )
			scale *= 0.9;
#endif

	return ::Round( width * scale );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::CalcTransformedPenWidth( )
//
// Description:		Get the device width of the given logical pen width
//
// Returns:				device width of the pen
//
// Exceptions:			None
//
// ****************************************************************************
//
YPenWidth RHeadlineGraphic::CalcTransformedPenWidth( YIntDimension logicalWidth,	
																	  const R2dTransform& transform ) const
{
YAngle							rotation;
YRealDimension					xScale;
YRealDimension					yScale;

	if ( logicalWidth < 1 )
		return static_cast<YPenWidth>( 0 );
	if ( logicalWidth == 1 )
		return static_cast<YPenWidth>( 1 );

	// get line scale facter
	transform.Decompose( rotation, xScale, yScale );
	TpsAssert( ( yScale > 0 ), "Bad pen width scale." );

/*	
   12/23/97 Lance - moved to CalcLogicalPenWidth() so it
	could be used in margin width determination as well. 

	// fudge the y scale to keep the line weights reasonable 
	if ( m_DistortEffect == kWarpToEnvelop && m_NrLines == 1 )
		if ( m_LineWeight > kThinLine )
			if ( m_ScaleConstraint == kByVertical || m_ScaleConstraint == kByHorizontalAndVertical )
				yScale *= 0.9;
	if ( IsVertical() )
		if ( m_ScaleConstraint == kByVertical )
			yScale *= 0.9;
*/

	// compute
	YIntDimension nWidth = ::Round( (YRealDimension)logicalWidth * yScale );	

	return static_cast<YPenWidth>( ::Max( nWidth, static_cast<YIntDimension>( 1 ) ) );
}

// ****************************************************************************
//
// Function Name:		RHeadlineGraphic::HasSoftEffects( )
//
// Description:		Determines if the headline graphic uses soft effects
//
// Returns:				TRUE if soft effects used; otherwise FALSE.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RHeadlineGraphic::HasSoftEffects() const
{
	if (kEmbossedOutline == m_OutlineEffect || 
		 kDebossedOutline == m_OutlineEffect ||
		 kBlurredOutline  == m_OutlineEffect ||
		 kShadowedOutline == m_OutlineEffect)
	{
		return TRUE;
	}

	return FALSE;
}
