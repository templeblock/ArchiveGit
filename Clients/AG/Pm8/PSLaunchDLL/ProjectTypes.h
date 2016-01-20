// ****************************************************************************
//
//  File Name:			ProjectTypes.h
//
//  Project:			Renaissance Application
//
//  Author:				Michael Houle
//
//  Description:		Declaration of the Project Types / Enums and Consts
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/PSLaunchDLL/ProjectTypes.h                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:26p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PROJECTTYPES_H_
#define		_PROJECTTYPES_H_

// ****************************************************************************
//			ENUMs and other CONSTs
// ****************************************************************************
//
//		REVIEW - esv 7/12/96 - the order of the EProjType enum must currently match the order of the ProjInfo array in ProjInfo.h; I will change this soon so don't fret
enum	EProjType
{
	kSign=0, kCertificate, kLetterHead,
	kYearlyCalendar, kMonthlyCalendar, kWeeklyCalendar, kDailyCalendar,
	kGreetingCardSideFoldHalfPage, kGreetingCardSideFoldSpreadHalfPage, 
	kGreetingCardTopFoldHalfPage, kGreetingCardTopFoldSpreadHalfPage,
	kGreetingCardSideFoldQuarterPage, kGreetingCardSideFoldSpreadQuarterPage, 
	kGreetingCardTopFoldQuarterPage, kGreetingCardTopFoldSpreadQuarterPage,
	kGreetingCardSideFoldNotecard, kGreetingCardSideFoldSpreadNotecard, 
	kGreetingCardTopFoldNotecard, kGreetingCardTopFoldSpreadNotecard,
	kNotepad, kPostCard, kBanner, 
	kBusinessCard, 
	kBusinessEnvelope, k9x6Envelope, kNotecardEnvelope, k6x4p75Envelope, 
	kPrintShopEnvelope, 
	kTrifoldBrochure, kPamphlet, kPostItNote,  
	kLabel4030, kLabel4031, kLabel4060,
	kLabel4061, kLabel4062, kLabel4065, kLabel4067,
	kLabel4088, kLabel4143, kLabel4145, kLabel4240, kLabel4241, 
	kLabel5160, kLabel5161, kLabel5162, kLabel5163, kLabel5164,
	kLabel5196, kLabel5197, kLabel5198, kLabel5199face, kLabel5199spine, 
	kLabel5267, kLabel5361, kLabel5385, 
	kLabel5386, kLabel5390, kLabel5395, kLabel5824, 
	kLabel7160, kLabel7161, kLabel7162, kLabel7163, kLabel7164,
	kLabel7165, kLabel7166, kLabel7168, kLabel7651, 
	kLabel7655,	kLabel7664, kLabel7671face, kLabel7674spine,
	kLabel7418, 
	kEGreeting, kTestPage, kPostCard2Panel,
	kGreetingCard4PanelSideFoldNotecard, kGreetingCard4PanelSideFoldSpreadNotecard, 
	kGreetingCard4PanelTopFoldNotecard, kGreetingCard4PanelTopFoldSpreadNotecard,
	kA4PostCard, kA4PostCard2Panel, kA4BusinessCard,
	kLabelC2160, kLabelC2163, kLabelC2166, kLabelC2180,
	kLabelC2241, kLabelC2242, kLabelC2245, kLabelC2351,
	kLabelC2352, kLabelC2354, kLabelC2355, kLabel7413, kA4Postit, kLabel7421, 
	kEnvelopeDL, kEnvelopeC4, kEnvelopeC5, kEnvelopeC6,
	kA4GreetingCard4PanelSideFoldNotecard, kA4GreetingCard4PanelSideFoldSpreadNotecard, 
	kA4GreetingCard4PanelTopFoldNotecard, kA4GreetingCard4PanelTopFoldSpreadNotecard,
	kA4PlainPaperPostCard, kGreetingCardPhotoFrameSideFold, kGreetingCardPhotoFrameTopFold, 
	kGreetingCardEmbossedSideFold, kGreetingCardEmbossedTopFold, 
	kGreetingCardHalfFoldEmbossedSideFold, kGreetingCardHalfFoldEmbossedTopFold,
	kLabel3261Small, kLabel3261Large, kGreetingCardMiniFoldCardSideFold, kGreetingCardMiniFoldCardTopFold, 
	kPhotoLayout1, kPhotoLayout2, kPhotoLayout3, kPhotoLayout4, kPhotoLayout5, kPhotoLayout6, 
	kPhotoLayout7, kPhotoLayout8, kPhotoAlbum, kPhotoCollage, kPhotoNovelty, 
	kGreetingCardSmallPhotoFrameSideFold, kGreetingCardSmallPhotoFrameTopFold, 
	kGreetingCardLargePhotoFrameSideFold, kGreetingCardLargePhotoFrameTopFold, 
	kAvery3263PostCard, kAvery3258Envelope, kAvery3259Envelope, kAvery3260Envelope,
	kGreetingCardMiniFoldCardSideFoldSpread, kGreetingCardMiniFoldCardTopFoldSpread, 
	kLabelPrintList, kA4AveryC2354BusinessCard, kBlankPageTransfer,
	kNumberOfBuiltInProjTypes
};
																
enum	EPPFlag
{ 
	kProjectNoFlags=0, kProjectDuplex=1, kProjectTrimLines=2, kProjectFoldLines=4,
	kProjectScalable=8, kProjectPinFed=16, kProjectInternal=32, kProjectPrintsTall=64,
	kProjectPrintsWide=128, kProjectPrintsSecondPanelWide=256+kProjectPrintsWide,
	kProjectHasBackgroundBleed=512
};
																
enum	EPPType
{
	kSignType=0, kCertificateType, kLetterHeadType, kCalendarType, 
	kGreetingCardType, kEnvelopeType,
	kLabelType, kBusinessCardType, kPostitNoteType,
	kNotepadType, kPostCardType, kBannerType, kPamphletType, kTestPageType, 
	kEGreetingType, kPhotoType, kNumberOfPPTypes
};

enum	EPShape					{ kRectangular=0, kElliptical, kNumberOfShapes };
enum	EOrientation			{ kTall=0, kWide, kNumberOfOrientations };
enum	EPossibleOrientation	{ kTallOnly, kWideOnly, kTallOrWide, kNumberOfPossibleOrientations };
enum	EPanels					{ kFrontPanel=0, kBackPanel, kInsidePanel, kNumberOfPanelTypes };
enum	EPaper					{ kPlainPaper=0, kLabelPaper, kPhotoPaper, kNumberOfPaperTypes };


										//	Project or paper preferenct when forcing a project change.
enum	EProjectPaperSwitchOrder{ kPreferPaper = 0, kPreferProject };


// justification of panel within its output area
enum	EPlace
{ 
	kPlaceLeft, kPlaceRight, kPlaceTop, kPlaceBottom, kPlaceCenter, kPlaceTopLeft,
	kPlaceBottomRight, kPlaceBottomLeft, kPlaceTopRight, kPlaceLeftCenter,
	kPlaceRightCenter, kPlaceTopCenter, kPlaceBottomCenter, kPlaceNone,
	kNumberOfPlaceTypes
};

// fill output area with . . . 
enum	EFill
{
	kOnlyOne,			// one instance of panel
	kStandardFill,	// as many as are specified
	kFixedFill, 		// as many as will fit in a fixed order and orientation
	kFreeFill, 		// as many as will fit with no order or relative orientation
	kNumberOfFillTypes
};

// scale panel (maintaining aspect ratio) or not
enum	EFit
{
	kTrim, 
	kScale,
	kAspectScale,
	kNumberOfFitTypes
};


const YCounter			kUnusedInstanceIndex( 0 );
const YIntDimension	yLogicalCropMarkSize( ::InchesToLogicalUnits( 1./16. ) );

//	Define disabled component attribute flags.
const uLONG kNoDisabledComponents			=0;
const uLONG kDisableSquareGraphic			=1;
const uLONG kDisableRowGraphic				=2;
const uLONG kDisableColumnGraphic			=4;
const uLONG kDisablePhotoGraphic				=8;
const uLONG kDisableImportImage				=16;
const uLONG kDisablePS4Library				=32;
const uLONG kDisableCustomGraphic			=64;
const uLONG kDisableBackdrop					=128;
const uLONG kDisableBannerBackdrop			=512;
const uLONG kDisableLayout						=1024;
const uLONG kDisableReadyMade					=2048;
const uLONG kDisableWatermark					=4096;
const uLONG kDisablePanelColor				=8192;
const uLONG kDisableHeadline					=16384;
const uLONG kDisableBannerHeadline			=32768;
const uLONG kDisableTextBlock					=65536;
const uLONG kDisableBorder						=131072;
const uLONG kDisableMiniBorder				=262144;
const uLONG kDisableAllGraphics				=524288;
const uLONG kDisableHorizontalLine			=1048576;
const uLONG kDisableVerticalLine				=2097152;
const uLONG kDisableCalendar					=4194304;
const uLONG kDisableLogo						=8388608;
const uLONG kDisableInitCap					=16777216;
const uLONG kDisableNumber						=33554432;
const uLONG kDisableSeal						=67108864;
const uLONG kDisableTimepiece					=134217728;
const uLONG kDisableSignature					=268435456;
const uLONG kStandardDisabledComponents	=kDisableBannerBackdrop | kDisableBannerHeadline | kDisableCalendar;
const uLONG kBannerDisabledComponents		=kDisableCalendar;
const uLONG kLabelDisabledComponents		=kStandardDisabledComponents | kDisableBackdrop;
const uLONG kPostitDisabledComponents		=kStandardDisabledComponents;
const uLONG kPhotoDisabledComponents		=kStandardDisabledComponents;

const uLONG	kBaseProjectInfoBuildNumber	= 126;



//	Minimum version of project table data. DO NOT CHANGE THIS NUMBER.
//	As of 3/3/97 we must be backward compatible with project info versions kDefaultDataVersion + 2 and greater.
const YVersion	kMinProjectRequiredVersion			= kDefaultDataVersion + 2;

//	Current version of project table data. Change this each time the project table data changes.
const	YVersion	kMergeDataRequiredVersion			= kDefaultDataVersion + 10;							//	This flag reserved as a get-out-of-crap-free card
const YVersion	kCurrentProjectRequiredVersion	= kMergeDataRequiredVersion;


#endif //_PROJECTTYPES_H_

