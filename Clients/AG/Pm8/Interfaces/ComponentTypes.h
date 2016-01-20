// ****************************************************************************
//
//  File Name:			ComponentTypes.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Definitions of the Components that Renaissance will have
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
//  $Logfile:: /PM8/Interfaces/ComponentTypes.h                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COMPONENTTYPES_H_
#define		_COMPONENTTYPES_H_


	//	Created by the Calendar Component
const YComponentType	kYearlyCalendarComponent				( "Yearly Calendar" );
const YComponentType	kMonthlyCalendarComponent				( "Monthly Calendar" );
const YComponentType	kWeeklyCalendarComponent				( "Weekly Calendar" );
const YComponentType	kDailyCalendarComponent					( "Daily Calendar" );
																			
	//	Created by the Graphic Component							
const YComponentType kSquareGraphicComponent					( "SquareGraphic" );
const YComponentType kRowGraphicComponent						( "RowGraphic" );
const YComponentType kColumnGraphicComponent					( "ColumnGraphic" );
const YComponentType kBackdropGraphicComponent				( "Backdrop" );
const YComponentType kBorderComponent							( "Border" );
const YComponentType kMiniBorderComponent						( "MiniBorder" );
const YComponentType kHorizontalLineComponent				( "HorizontalLine" );
const YComponentType kVerticalLineComponent					( "VerticalLine" );
const YComponentType kCGMGraphicComponent						( "CGM Graphic" );
																			
	// Created by the Group Component							
const YComponentType kGroupComponent							( "Group" );
const YComponentType kLogoComponent								( "Logo" );
const YComponentType kInitialCapComponent						( "InitCap" );
const YComponentType kSmartNumberComponent					( "SmartNumber" );
const YComponentType kSealComponent								( "Seal" );
const YComponentType kTimepieceComponent						( "Timepiece" );
const YComponentType kSignatureComponent						( "Signature" );
																			
	//	Created by the Headline Component						
const YComponentType kHeadlineComponent						( "Headline" );
const YComponentType kSpecialHeadlineComponent				( "SpecialHeadline" );
const YComponentType kVerticalHeadlineComponent				( "VerticalHeadline" );
const YComponentType kVerticalSpecialHeadlineComponent	( "VerticalSpecialHeadline" );


	//	Created by the Image Component
const YComponentType kImageComponent							( "Image" );
const YComponentType kPSDPhotoComponent							( "PSD Photo" );
const YComponentType kPhotoFrameComponent					( "Photo Frame" );
																			
	//	Created by the Text Component								
const YComponentType kTextComponent								( "Text" );
const YComponentType kReturnAddressComponent					( "Return Address" );
			 
//	Created by the Web Component
const YComponentType kNavigationButtonSetComponent	 			( "Navigation Button Sets" );
const YComponentType kIconButtonComponent	 					( "IconButtons" );
const YComponentType kBulletComponent	 						( "Bullet" );
const YComponentType kPageDividerComponent	 					( "Page Divider" );
const YComponentType kAnimationComponent	 					( "Animation" );
const YComponentType kBackgroundComponent	 					( "Backgrounds" );

// Created by the Path Component
const YComponentType kPathComponent								( "Path" );


#endif	//	_COMPONENTTYPES_H_

