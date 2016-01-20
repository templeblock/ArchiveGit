// ****************************************************************************
//
//  File Name:			HeadlineInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RHeadlineInterface class
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
//  $Logfile:: /PM8/Interfaces/HeadlineInterface.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_HEADLINEINTERFACE_H_
#define		_HEADLINEINTERFACE_H_

#ifndef		_COMPONENTDATAINTERFACE_H_
#include		"ComponentDataInterface.h"
#endif

enum		ETextEscapement		{ 	kLeft2RightTop2Bottom, kLeft2RightBottom2Top, 
											kRight2LeftTop2Bottom, kRight2LeftBottom2Top, 
											kTop2BottomLeft2Right, kTop2BottomRight2Left,
											kBottom2TopLeft2Right, kBottom2TopRight2Left };
enum		ETextJustification	{ 	kLeftJust, kCenterJust, kRightJust, kFullJust };
enum		ETextCompensation		{ 	kRegular, kTight, kLoose };
enum		ELineWeight				{ 	kHairLine, kThinLine, kMediumLine, kThickLine, kExtraThickLine };
enum		EVerticalPlacement	{ 	kPin2Top = 1, kPin2Middle, kPin2Bottom, kPin2Left, kPin2Right };
enum		EDistortEffects		{ 	kNonDistort, kFollowPath, kWarpToEnvelop, kAltBaseline, kShear, kAltShearRotate, kAltShear };
enum		EOutlineEffects		{ 	kSimpleStroke, kSimpleFill, kSimpleStrokeFill, 
											kHeavyOutline, kDoubleOutline, kBlurredOutline, 
											kTripleOutline, kHairlineDoubleOutline, kDoubleHairlineOutline,
											kEmbossedOutline, kDebossedOutline, kSoftEmbossedOutline, kSoftDebossedOutline, 
											kRoundedOutline, kGlintOutline, kSimpleFillStroke, kShadowedOutline,
											kBeveledOutline };
enum		EProjectionEffects	{ 	kNoProjection, kSimpleStack, kSpinStack, kPseudoPerspectiveStack, 
											kSolidExtrusion, kPseudoPerspectiveExtrusion, kIlluminateExtrusion };
enum		EShadowEffects			{ 	kNoShadow, kDropShadow, kSoftDropShadow, kCastShadow, kSoftCastShadow };
enum		EFramedEffects			{ 	kFrameNoFrame, kSilhouette, kEmboss, kDeboss, kSoftEmboss, kSoftDeboss, kSimpleFrame };
enum		EHeadlinePaths			{	kWarpPath00 = 2,												// unused
											kWarpPath10 MacCode( = 10 ),								// flat straight line
											kWarpPath20 MacCode( = 20 ),								// convex curve
											kWarpPath30 MacCode( = 30 ),								// concave curve
											kWarpPath40 MacCode( = 40 ),								// convex curve
											kWarpPath50 MacCode( = 50 ),								// concave curve
											kWarpPath60 MacCode( = 60 ) WinCode( = 23 ),			// convex curve
											kWarpPath70 MacCode( = 70 ),								// concave curve
											kWarpPath80 MacCode( = 80 ),								// convex to concave curve
											kWarpPath90 MacCode( = 90 ),								// concave to convex curve
											kWarpPath100 MacCode( = 100 ),							// diagonal up straight line
											kWarpPath110 MacCode( = 110 ),							// diagonal down straight line
											kWarpPath120 MacCode( = 120 ),							// seal convex curve
											kWarpPath130 MacCode( = 130 ),							// seal concave curve
											kWarpPathLast	};
enum		EHeadlineShapes		{	kNoWarpShape = 1,												// nondistort
											kWarpShape1 = 8,												// oval 
											kWarpShape2,													// half oval
											kWarpShape3,													// half oval
											kWarpShape4,													// trapezoid
											kWarpShape5,													// trapezoid
											kWarpShape6,													// trapezoid
											kWarpShape7,													// trapezoid
											kWarpShape8,													// tall rect
											kWarpShape9,													// slant left
											kWarpShape10,													// slant right
											kWarpShape11,													//	diamond
											kWarpShape12,													//	triangle
											kWarpShape13,													//	bow tie
											kWarpShape14,													//	arc
											kWarpShape110 	MacCode( = 110 ) WinCode( = 30 ),	// rect
											kWarpShape120 	MacCode( = 120 ),							// flat topped oval
											kWarpShape130 	MacCode( = 130 ),							// trapezoid
											kWarpShape140 	MacCode( = 140 ),							// trapezoid
											kWarpShape150 	MacCode( = 150 ),							// slant right
											kWarpShape160 	MacCode( = 160 ),							// slant left
											kWarpShape170 	MacCode( = 170 ),							// trapezoid
											kWarpShape180 	MacCode( = 180 ),							// trapezoid
											kWarpShape210 	MacCode( = 210 ),							// bow tie
											kWarpShape220 	MacCode( = 220 ),							// 
											kWarpShape230 	MacCode( = 230 ),							// 
											kWarpShape240 	MacCode( = 240 ),							// 
											kWarpShape250 	MacCode( = 250 ),							// 
											kWarpShape260 	MacCode( = 260 ),							// 
											kWarpShape270 	MacCode( = 270 ),							// 
											kWarpShape280 	MacCode( = 280 ),							//  
											kWarpShape310 	MacCode( = 310 ),							//  
											kWarpShape320 	MacCode( = 320 ),							// 
											kWarpShape330 	MacCode( = 330 ),							// 
											kWarpShape340 	MacCode( = 340 ),							// 
											kWarpShape350 	MacCode( = 350 ),							// 
											kWarpShape360 	MacCode( = 360 ),							// 
											kWarpShape370 	MacCode( = 370 ),							// 
											kWarpShape380 	MacCode( = 380 ),							// 
											kWarpShape410 	MacCode( = 410 ),							// 
											kWarpShape420 	MacCode( = 420 ),							// 
											kWarpShape430 	MacCode( = 430 ),							// 
											kWarpShape440 	MacCode( = 440 ),							//
											kWarpShape450 	MacCode( = 450 ),							// 
											kWarpShape460 	MacCode( = 460 ),							// 
											kWarpShape470 	MacCode( = 470 ),							// 
											kWarpShape480 	MacCode( = 480 ),							// 
											kWarpShape510 	MacCode( = 510 ),							// 
											kWarpShape520 	MacCode( = 520 ),							// 
											kWarpShape530 	MacCode( = 530 ),							// 
											kWarpShape540 	MacCode( = 540 ),							// 
											kWarpShape550 	MacCode( = 550 ),							// 
											kWarpShape560 	MacCode( = 560 ),							// 
											kWarpShape570 	MacCode( = 570 ),							// 
											kWarpShape580 	MacCode( = 580 ),							// 
											kWarpShape610 	MacCode( = 610 ),							// 
											kWarpShape620 	MacCode( = 620 ),							// 
											kWarpShape630 	MacCode( = 630 ),							// 
											kWarpShape640 	MacCode( = 640 ),							// 
											kWarpShape650 	MacCode( = 650 ),							// 
											kWarpShape660 	MacCode( = 660 ),							// 
											kWarpShape670 	MacCode( = 670 ),							// 
											kWarpShape680 	MacCode( = 680 ),							// 
											kWarpShape710 	MacCode( = 710 ),							// 
											kWarpShape720 	MacCode( = 720 ),							// 
											kWarpShape730 	MacCode( = 730 ),							// 
											kWarpShape810 	MacCode( = 810 ),							// vertical shape
											kWarpShape820 	MacCode( = 820 ),							// vertical shape
											kWarpShape830 	MacCode( = 830 ),							// vertical shape
											kWarpShape840 	MacCode( = 840 ),							// vertical shape
											kWarpShape850 	MacCode( = 850 ),							// vertical shape
											kWarpShape860 	MacCode( = 860 ),							// vertical shape
											kWarpShape910 	MacCode( = 910 ),							// vertical shape
											kWarpShape920 	MacCode( = 920 ),							// vertical shape
											kWarpShape930 	MacCode( = 930 ),							// vertical shape
											kWarpShape940 	MacCode( = 940 ),							// vertical shape
											kWarpShape950 	MacCode( = 950 ),							// vertical shape
											kWarpShape960 	MacCode( = 960 ),							// vertical shape
											kWarpShape970 	MacCode( = 970 ),							// vertical shape
											kWarpShape1010 MacCode( = 1010 ),						// vertical shape
											kWarpShape1020 MacCode( = 1020 ),						// vertical shape
											kWarpShape1030 MacCode( = 1030 ),						// vertical shape
											kWarpShape1040 MacCode( = 1040 ),						// vertical shape
											kWarpShape1050 MacCode( = 1050 ),						// vertical shape
											kWarpShape1060 MacCode( = 1060 ),						// vertical shape
											kWarpShapeLast	};


const	YInterfaceId	kHeadlineInterfaceId			= 70;

// ****************************************************************************
//
//  Class Name:	RGraphicHolderImp
//
//  Description:	A Pure virtual class for Holding the data of a data interface
//
// ****************************************************************************
//
class RHeadlineHolder : public RComponentDataHolder
	{
	//	Construction and Destruction
	public :
													RHeadlineHolder( ) { ; }
		virtual									~RHeadlineHolder( ) { ; }

	//	Operations
	public :
		virtual void							Read( RStorage& storage ) = 0;
		virtual void							Write( RStorage& storage ) const = 0;
	} ;

// ****************************************************************************
//
//  Class Name:	RHeadlineInterface
//
//  Description:	A pure virtual class interfacing with a headline component
//
// ****************************************************************************
//
class RHeadlineInterface : public RComponentDataInterface 
	{
	//	Construction & Destruction
	public :
													RHeadlineInterface( const RComponentView* pView ) : RComponentDataInterface( pView ) { ; }

	//	Operations
	public :
		virtual RComponentDataHolder*		GetData( ) = 0;
		virtual void							EditData( ) = 0;
		virtual void							SetData( RComponentDataHolder* ) = 0;
		virtual void							ClearData( ) = 0;
		virtual void							Read( RChunkStorage& ) = 0;
		virtual void							Write( RChunkStorage& ) = 0;
		virtual void							Render( RDrawingSurface& , const R2dTransform&, const RIntRect&, const RIntRect& ) = 0;

		virtual void							Commit( ) = 0;

		//	Attributes.
		virtual void							GetFontName( RMBCString& fontName ) = 0;
		virtual void							GetEscapement( ETextEscapement& escapement ) = 0;
		virtual void							GetFontAttributes( YFontAttributes& attributes ) = 0;
		virtual void							GetJustification( ETextJustification& justification ) = 0;
		virtual void							GetCompensation( ETextCompensation& compensation ) = 0;
		virtual void							GetCompensationDegree( float& flCompensation ) = 0;
		virtual void							GetKerning( BOOLEAN& fKerning ) = 0;
		virtual void							GetLeading( BOOLEAN& fLeading ) = 0;
		virtual void							GetScale1( uWORD& uwScale ) = 0;
		virtual void							GetScale2( uWORD& uwScale ) = 0;
		virtual void							GetScale3( uWORD& uwScale ) = 0;
		virtual void							GetNoDisplayEnvelop( BOOLEAN& fNotDisplay ) = 0;
		virtual void							GetNumLines( uWORD& numLines ) = 0;

		virtual void							SetFontName( const RMBCString& fontName ) = 0;
		virtual void							SetEscapement( ETextEscapement escapement ) = 0;
		virtual void							SetFontAttributes( YFontAttributes attributes ) = 0;
		virtual void							SetJustification( ETextJustification justification ) = 0;
		virtual void							SetCompensation( ETextCompensation compensation ) = 0;
		virtual void							SetCompensationDegree( float flCompensation ) = 0;
		virtual void							SetKerning( BOOLEAN fKerning ) = 0;
		virtual void							SetLeading( BOOLEAN fLeading ) = 0;
		virtual void							SetScale1( uWORD uwScale ) = 0;
		virtual void							SetScale2( uWORD uwScale ) = 0;
		virtual void							SetScale3( uWORD uwScale ) = 0;
		virtual void							SetNoDisplayEnvelop( BOOLEAN fNotDisplay ) = 0;

		//	Shape 
		virtual void							GetDistortEffect( EDistortEffects& effect ) = 0;
		virtual void							GetShearAngle( int& shearAngle ) = 0;
		virtual void							GetRotation( int& rotation ) = 0;
		virtual void							GetAltPercent( float& percent ) = 0;
		virtual void							GetDistort( BOOLEAN& fDistort ) = 0;
		virtual void							GetShapeIndex( int& shapeIndex ) = 0;
		virtual void							GetVerticalPlacement( EVerticalPlacement& placement ) = 0;

		virtual void							SetDistortEffect( EDistortEffects effect ) = 0;
		virtual void							SetShearAngle( int shearAngle ) = 0;
		virtual void							SetRotation( int rotation ) = 0;
		virtual void							SetAltPercent( float percent ) = 0;
		virtual void							SetDistort( BOOLEAN fDistort ) = 0;
		virtual void							SetShapeIndex( int shapeIndex ) = 0;
		virtual void							SetVerticalPlacement( EVerticalPlacement placement ) = 0;

		//	Outline
		virtual void							GetOutlineFillColor( RColor& color ) = 0;
		virtual void							GetOutlineEffect( EOutlineEffects& effect ) = 0;
		virtual void							GetLineWeight( ELineWeight& weight ) = 0;		
		virtual void							GetStrokeColor( RSolidColor& color ) = 0;
		virtual void							GetShadowColor( RSolidColor& color ) = 0;
		virtual void							GetHiliteColor( RSolidColor& color ) = 0;
		virtual void							GetBlur2Color( RSolidColor& color ) = 0;

		virtual void							SetOutlineFillColor( const RColor& color ) = 0;
		virtual void							SetOutlineEffect( EOutlineEffects effect ) = 0;
		virtual void							SetLineWeight( ELineWeight weight ) = 0;		
		virtual void							SetStrokeColor( const RSolidColor& color ) = 0;
		virtual void							SetShadowColor( const RSolidColor& color ) = 0;
		virtual void							SetHiliteColor( const RSolidColor& color ) = 0;
		virtual void							SetBlur2Color( const RSolidColor& color ) = 0;

		//	Effects
		virtual void							GetNumStages( int& stages ) = 0;
		virtual void							GetProjectionFillColor( RColor& color ) = 0;
		virtual void							GetProjectionEffect( EProjectionEffects& effect ) = 0;
		virtual void							GetProjectionVanishPoint( RIntPoint& vanishPt ) = 0;
		virtual void							GetProjectionDepth( float& flDepth ) = 0;
		virtual void							GetShadowFillColor( RColor& color ) = 0;
		virtual void							GetShadowEffect( EShadowEffects& effect ) = 0;
		virtual void							GetShadowVanishPoint( RIntPoint& vanishPt ) = 0;
		virtual void							GetShadowDepth( float& flDepth ) = 0;
		virtual void							GetShadowColor2( RSolidColor& color ) = 0;

		virtual void							SetNumStages( int stages ) = 0;
		virtual void							SetProjectionFillColor( const RColor& color ) = 0;
		virtual void							SetProjectionEffect( EProjectionEffects effect ) = 0;
		virtual void							SetProjectionVanishPoint( const RIntPoint& vanishPt ) = 0;
		virtual void							SetProjectionDepth( float flDepth ) = 0;
		virtual void							SetShadowFillColor( const RColor& color ) = 0;
		virtual void							SetShadowEffect( EShadowEffects effect ) = 0;
		virtual void							SetShadowVanishPoint( const RIntPoint& vanishPt ) = 0;
		virtual void							SetShadowDepth( float flDepth ) = 0;
		virtual void							SetShadowColor2( const RSolidColor& color ) = 0;

		//	Behinds
		virtual void							GetBehindColor( RColor& color ) = 0;
		virtual void							GetBehindEffect( EFramedEffects& effect ) = 0;
		virtual void							GetBehindShadowColor( RSolidColor& color ) = 0;
		virtual void							GetBehindHilite( RSolidColor& color ) = 0;

		virtual void							SetBehindColor( const RColor& color ) = 0;
		virtual void							SetBehindEffect( EFramedEffects effect ) = 0;
		virtual void							SetBehindShadowColor( const RSolidColor& color ) = 0;
		virtual void							SetBehindHilite( const RSolidColor& color ) = 0;

		// Interface Data
		virtual void							GetInterfaceData( uLONG& ulData ) = 0;
		virtual void							SetInterfaceData( uLONG ulData ) = 0;

		//	Text Data
		virtual void							GetHeadlineText( RMBCString& text ) = 0;
		virtual void							SetHeadlineText( const RMBCString& text ) = 0;

	//	Private Members
	private :
	} ;

#endif	//	_HEADLINEINTERFACE_H_
