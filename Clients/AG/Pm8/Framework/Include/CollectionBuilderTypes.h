// ****************************************************************************
//
//  File Name:			CollectionBuilderTypes.h
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of types from the CollectionBuilder utility
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef	_COLLECTIONBUILDERTYPES_H_
#define	_COLLECTIONBUILDERTYPES_H_


// the collection use types from the CollectionBuidler utility
enum  cbUSETYPES
{
	utGraphic, utBackdrop, utLine, utBorder, utQuote, 
	utLayout, utReadymade, utCustom, utGraphicWithRef, utBackdropWithRef,
	utNavButton, utWebObject, utAnimatedGif, utBackdropGif, utCOUNT
};

// the shape types from the CollectionBuilder utility, plus stAnyType
enum cbSHAPETYPES
{
	stAnyType = -1, stSquare, stRow, stColumn, stBackground, stBorder, stRule, 
	stTimepiece, stSpot, stSeal, stAutograph, stCertificateborder,
	stImage, stImageRef, stPSDEnvBD, stInitcap, stNumber, stLogo, stText,

	// Leapfrog-specific shape types
	stNavButton, stIconButton, stBullet, stPagedivider, stAnimation, /* stWebBackground */

	// Fine Art Types 
	stFineArt,		/* = 23 */ 
	stFineArtRef,	/* = 24 */

	stCalendarpart /* = 25 */
};

// the backdrop aspect types from the CollectionBuilder utility
enum cbBACKGROUNDTYPES
{
	btDummy, btPortrait, btLandscape, btHorzBanner, 
	btVertBanner, btExtremeTall, btExtremeWide, btTiled,
	btBusEnvelope, btPSDEnvelope, btCOUNT
};

// Clipart header
#pragma pack( push, 1 )
typedef struct
{
	long				size ;
	long				unpackedSize ;
	char				compressionScheme ;
	unsigned char	unusedColorIndex ;
	short				xSize ;
	short				ySize ;

} DS_CLIPART ;
#pragma pack( pop )


#endif	// _COLLECTIONBUILDERTYPES_H_
