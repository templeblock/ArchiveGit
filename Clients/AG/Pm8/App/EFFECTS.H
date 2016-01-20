/*
// $Workfile: EFFECTS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/EFFECTS.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 6     5/12/98 11:34a Rlovejoy
// Added radial fills.
// 
// 5     4/30/98 6:44p Jayn
// Further fix for the FillFormat changes.
// 
// 4     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 3     4/29/98 5:58p Rlovejoy
// New fill format.
// 
// 2     4/28/98 10:45a Rlovejoy
// Added gradient fill types.
// 
//    Rev 1.0   14 Aug 1997 15:20:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:40   Fred
// Initial revision.
// 
//    Rev 1.4   26 Jun 1997 16:14:58   Jay
// Fixed old text style changes
// 
//    Rev 1.3   11 Jun 1997 17:28:18   Fred
// Bitmap drawing optimizations
// 
//    Rev 1.2   29 May 1997 08:43:52   Fred
// New effects support
// 
//    Rev 1.1   28 May 1997 11:05:26   Fred
// Added SetColor methods.
// 
//    Rev 1.0   23 May 1997 17:08:56   Fred
// New effects
*/

struct FillFormatV1;
struct FillFormatV2;
struct OutlineFormat;
struct ShadowFormat;

#ifndef	__EFFECTS_H__
#define	__EFFECTS_H__

#include "color.h"
#include "cfixed.h"

/////////////////////////////////////////////////////////////////////////////
// Formatting structures.
// Note that nearly all attributes need an "undefined" value.

struct FillFormatV1
{
	enum
	{
		// Predefined fill types known by SetType().
		FillNone,
		FillSolid,
	};

	enum
	{
		// Style
		Undefined		=	0x7fff,
		None				=	-1,
		Black				=	0,
		White				=	1,
		Gray2Percent	=	2,
		Gray5Percent	=	3,
		Gray15Percent	=	4,
		Gray30Percent	=	5,
		Gray50Percent	=	6,
		Gray70Percent	=	7,
		Gray90Percent	=	8,
		Horizontal		=	9,
		Vertical			=	10,
		GraphPaper		=	11,
		WoodGrain		=	12,
		StarryNight		=	13,
		Sunset			=	14,
		Fountain			=	15,
		Rattan			=	16,
		Sunrise			=	17,
		Starburst		=	18,
		Radial			=	19,
		Swirl				=	20,
		Stripes			=	21,
		Zigzag			=	22,
		Plaid				=	23,
		Brick				=	24,
		LondonFog		=	25,
		Cloud				=	26,
		Tubular			=	27
	};

	SHORT m_nPattern;
	COLOR m_ForegroundColor;

	void FromOld(int nOldStyle, COLOR OldColor);
	BOOL IsVisible(void)
		{ return (m_nPattern != None) && (m_ForegroundColor != TRANSPARENT_COLOR); }
	BOOL IsUndefined(void) const
		{ return (m_nPattern == Undefined) || (m_ForegroundColor == UNDEFINED_COLOR); }
	BOOL IsInvisibleOrWhite(void)
		{ return (m_nPattern == None) || (m_ForegroundColor == TRANSPARENT_COLOR) || (m_ForegroundColor == COLOR_WHITE); }
	BOOL IsSolid(void)
		{ return (m_nPattern == Black) && (m_ForegroundColor != TRANSPARENT_COLOR); }
	BOOL IsSameAsMask(void)
		{ return !IsVisible() || (m_nPattern == Black); }
	void SetType(int nType, COLOR Color = TRANSPARENT_COLOR);
	void SetColor(SHORT nPattern, COLOR Color);
};

// Fill description.
struct FillFormatV2
{
	enum
	{
		// Predefined fill types known by SetType().
		FillNone = FillFormatV1::FillNone,
		FillSolid = FillFormatV1::FillSolid,
		FillSweepRight,
		FillSweepDown,
		FillRadialCenter,
		FillRadialCorner,

		FillNumTypes
	};
	enum
	{
		// Style
		Undefined		= FillFormatV1::Undefined,
		None				= FillFormatV1::None,
		Black				= FillFormatV1::Black,
		White				= FillFormatV1::White,
		Gray2Percent	= FillFormatV1::Gray2Percent,
		Gray5Percent	= FillFormatV1::Gray5Percent,
		Gray15Percent	= FillFormatV1::Gray15Percent,
		Gray30Percent	= FillFormatV1::Gray30Percent,
		Gray50Percent	= FillFormatV1::Gray50Percent,
		Gray70Percent	= FillFormatV1::Gray70Percent,
		Gray90Percent	= FillFormatV1::Gray90Percent,
		Horizontal		= FillFormatV1::Horizontal,
		Vertical			= FillFormatV1::Vertical,
		GraphPaper		= FillFormatV1::GraphPaper,
		WoodGrain		= FillFormatV1::WoodGrain,
		StarryNight		= FillFormatV1::StarryNight,
		Sunset			= FillFormatV1::Sunset,
		Fountain			= FillFormatV1::Fountain,
		Rattan			= FillFormatV1::Rattan,
		Sunrise			= FillFormatV1::Sunrise,
		Starburst		= FillFormatV1::Starburst,
		Radial			= FillFormatV1::Radial,
		Swirl				= FillFormatV1::Swirl,
		Stripes			= FillFormatV1::Stripes,
		Zigzag			= FillFormatV1::Zigzag,
		Plaid				= FillFormatV1::Plaid,
		Brick				= FillFormatV1::Brick,
		LondonFog		= FillFormatV1::LondonFog,
		Cloud				= FillFormatV1::Cloud,
		Tubular			= FillFormatV1::Tubular
	};

	SHORT m_nPattern;
	SHORT m_nFillType;
	COLOR m_ForegroundColor;
	COLOR m_GradientBlendColor;

	void FromOld(int nOldStyle, COLOR OldColor);
	BOOL IsVisible(void)
		{ return (m_nPattern != None) && (m_ForegroundColor != TRANSPARENT_COLOR); }
	BOOL IsUndefined(void) const
		{ return (m_nPattern == Undefined) || (m_ForegroundColor == UNDEFINED_COLOR); }
	BOOL IsInvisibleOrWhite(void)
		{ return (m_nPattern == None) || (m_ForegroundColor == TRANSPARENT_COLOR) || (m_ForegroundColor == COLOR_WHITE); }
	BOOL IsSolid(void)
		{ return (m_nPattern == Black) && (m_ForegroundColor != TRANSPARENT_COLOR); }
	BOOL IsSameAsMask(void)
		{ return !IsVisible() || (m_nPattern == Black); }
	void SetType(int nType, COLOR Color = TRANSPARENT_COLOR, COLOR Blend = UNDEFINED_COLOR);
	void SetColor(SHORT nPattern, COLOR Color, COLOR Blend = UNDEFINED_COLOR);

	const FillFormatV2& operator=(const FillFormatV1& ff);
};

// Outline description.

#define	TEXT_OUTLINE_THIN_FRACTION			(0.0075)
#define	TEXT_OUTLINE_MEDIUM_FRACTION		(0.0120)
#define	TEXT_OUTLINE_THICK_FRACTION		(0.0175)

struct OutlineFormat
{
	enum
	{
		// Predefined outline types known by SetType().
		OutlineNone				=		0,
		OutlineHairline		=		1,
		OutlineThin				=		2,
		OutlineMedium			=		4,
		OutlineThick			=		5,
		OutlineAbsolute		=		6,
		OutlineRelative		=		7,
		OutlineCalligraphy	=		8,
		OutlineContour1		=		9,
		OutlineContour2		=		10
	};
	enum
	{
		// Style
		Undefined	=		0x7fff,
		Inflate		=		-1,
		None			=		0,
		Light			=		1,
		Medium		=		2,
		Heavy			=		3,
		Calligraphy	=		4,
		Contour1		=		5,
		Contour2		=		6
	};
	enum
	{
		// Width Type
		Absolute		=		0,
		Relative		=		1
	};

	SHORT m_nStyle;
	SHORT m_nWidthType;
	CFixed m_lWidth;			// Percent or points
	SHORT m_nPattern;
	COLOR m_ForegroundColor;

	void FromOld(int nOldStyle, COLOR OldColor);
	BOOL IsVisible(void)
		{ return (m_nStyle != None) && (m_nPattern != FillFormatV1::None) && (m_ForegroundColor != TRANSPARENT_COLOR); }
	BOOL IsUndefined(void) const
		{ return (m_nStyle == Undefined) || (m_ForegroundColor == UNDEFINED_COLOR); }
	BOOL IsInvisibleOrWhite(void)
		{ return (m_nStyle == None) || (m_nPattern == FillFormatV2::None) || (m_ForegroundColor == TRANSPARENT_COLOR) || (m_ForegroundColor == COLOR_WHITE); }
	BOOL IsSolid(void)
		{ return (m_nStyle != None) && (m_nPattern == FillFormatV2::Black) && (m_ForegroundColor != TRANSPARENT_COLOR); }
	BOOL IsContour(void)
		{ return (m_nStyle == Contour1) || (m_nStyle == Contour2); }
	BOOL IsSameAsMask(void)
		{ return !IsVisible() || ((m_nPattern == FillFormatV2::Black) && !IsContour()); }
	void SetType(int nType, CFixed lWidth = 0);
	void SetColor(SHORT nPattern, COLOR Color);
};

// Shadow information.

#define	TEXT_SHADOW_FRACTION		(5.0/256.0)
#define	OBJECT_SHADOW_FRACTION	(1.0/30.0)

struct ShadowFormat
{
	enum
	{
		// Predefined shadow types known by SetType().
		ShadowNone					=		0,

		// Drop shadows, values must be consecutive.
		ShadowDropDownRight		=		1,
		ShadowDropDownLeft		=		2,
		ShadowDropUpRight			=		3,
		ShadowDropUpLeft			=		4,

		// Extruded shadows, values must be consecutive.
		ShadowExtrudedDownRight	=		5,
		ShadowExtrudedDownLeft	=		6,
		ShadowExtrudedUpRight	=		7,
		ShadowExtrudedUpLeft		=		8
	};
	enum
	{
		// Style
		Undefined	=		0x7fff,
		None			=		0,
		Drop			=		1,
		Extruded		=		2
	};
	enum
	{
		// Offset Type
		Absolute		=		0,
		Relative		=		1
	};

	SHORT m_nStyle;
	SHORT m_nOffsetType;
	CFixed m_lXOffset;	// Percent or points
	CFixed m_lYOffset;	// Percent or points
	SHORT m_nPattern;
	COLOR m_ForegroundColor;

	void FromOld(int nOldStyle, COLOR OldColor);
	BOOL IsVisible(void)
		{ return (m_nStyle != None) && (m_nPattern != FillFormatV1::None) && (m_ForegroundColor != TRANSPARENT_COLOR); }
	BOOL IsUndefined(void) const
		{ return (m_nStyle == Undefined) || (m_ForegroundColor == UNDEFINED_COLOR); }
	BOOL IsInvisibleOrWhite(void)
		{ return (m_nStyle == None) || (m_nPattern == FillFormatV1::None) || (m_ForegroundColor == TRANSPARENT_COLOR) || (m_ForegroundColor == COLOR_WHITE); }
	BOOL IsSolid(void)
		{ return (m_nStyle != None) && (m_nPattern == FillFormatV1::Black) && (m_ForegroundColor != TRANSPARENT_COLOR); }
	BOOL IsSameAsMask(void)
		{ return !IsVisible() || (m_nPattern == FillFormatV1::Black); }
	void SetType(int nType, CFixed lFraction = 0);
	void SetColor(SHORT nPattern, COLOR Color);
};

#endif
