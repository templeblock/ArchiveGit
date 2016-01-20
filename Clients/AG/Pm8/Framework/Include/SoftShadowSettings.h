// ****************************************************************************
//
//  File Name:			SoftShadowSettings.h
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of the RSoftShadowSettings class
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef	_SOFTSHADOWSETTINGS_H_
#define	_SOFTSHADOWSETTINGS_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif


// SHADOW ON
const BOOLEAN kDefaultShadowOn = FALSE;


// SHADOW OFFSET
// range of YPercentage of min of component width,height for shadow offset:
// 0% shadow offset = kMinShadowOffset of min(componentWidth,componentHeight)
// 50% shadow offset = kMidPointShadowOffset of min(componentWidth,componentHeight)
// 100% shadow offset = kMaxShadowOffset of min(componentWidth,componentHeight)
// intermediate %'s use bilinear interpolation
const	YPercentage kMinShadowOffset = 0.029;
const	YPercentage kMidPointShadowOffset = 0.125;
const	YPercentage kMaxShadowOffset = 0.22;
const YPercentage kDefaultShadowOffset = 0.08;


// SHADOW ANGLE
// default 45 degrees (down-right) internally converted to radians
const YAngle kDefaultShadowAngle = 45.0;


// SHADOW OPACITY
//
// range of percents of shadow opacity color:
// 0% shadow opacity = kMinShadowOpacityColor of shadow color
// 50% shadow opacity = kMidPointShadowOpacityColor of shadow color
// 100% shadow opacity = kMaxShadowOpacityColor of shadow color
// intermediate %'s use bilinear interpolation
const YPercentage kMinShadowOpacityColor = 0.09;
const YPercentage kMidPointShadowOpacityColor = 0.45;
const YPercentage kMaxShadowOpacityColor = 1.0;
//
// range of percents of shadow opacity black:
// 0% shadow opacity = kMinShadowOpacityBlack of black
// 50% shadow opacity = kMidPointShadowOpacityBlack of black
// 100% shadow opacity = kMaxShadowOpacityBlack of black
// intermediate %'s use bilinear interpolation
const YPercentage kMinShadowOpacityBlack = 0.06;
const YPercentage kMidPointShadowOpacityBlack = 0.3;
const YPercentage kMaxShadowOpacityBlack = 0.0;
//
// range of percents of shadow opacity background:
// 0% shadow opacity = kMinShadowOpacityBackground of background
// 50% shadow opacity = kMidPointShadowOpacityBackground of background
// 100% shadow opacity = kMaxShadowOpacityBackground of background
// intermediate %'s use bilinear interpolation
const YPercentage kMinShadowOpacityBackground = 0.85;
const YPercentage kMidPointShadowOpacityBackground = 0.25;
const YPercentage kMaxShadowOpacityBackground = 0.0;
//
// default shadow opacity slider percentage setting
const YPercentage kDefaultShadowOpacitySetting = 0.6;


// SHADOW COLOR
const RSolidColor kDefaultShadowColor = kBlack;


// SHADOW EDGE SOFTNESS
// range of percentages of min of component width,height for shadow edge softness:
// 0% shadow edge softness = kMinShadowEdgeSoftness of min(componentWidth,componentHeight)
// 50% shadow edge softness = kMidPointShadowEdgeSoftness of min(componentWidth,componentHeight)
// 100% shadow edge softness = kMaxShadowEdgeSoftness of min(componentWidth,componentHeight)
// intermediate %'s use linear interpolation
const YPercentage kMinShadowEdgeSoftness = 0.0;
const YPercentage kMidPointShadowEdgeSoftness = 0.03;
const YPercentage kMaxShadowEdgeSoftness = 0.06;
const YPercentage kDefaultShadowEdgeSoftness = 0.1;


class FrameworkLinkage RSoftShadowSettings : public RObject
{
// Construction & destruction
public:
						RSoftShadowSettings();
						RSoftShadowSettings( const RSoftShadowSettings& rhs );
						RSoftShadowSettings( BOOLEAN shadowOn );

// Operators
public:
	const RSoftShadowSettings& operator=( const RSoftShadowSettings& rhs );

	BOOLEAN 							operator!=( const RSoftShadowSettings& rhs ) const;
	BOOLEAN 							operator==( const RSoftShadowSettings& rhs ) const;

// Public Member data
public:
	BOOLEAN			m_fShadowOn;				// TRUE=enable shadow, FALSE=disable shadow
	YPercentage		m_fShadowOffset;			// 0.0=kMinShadowOffset, 1.0=kMaxShadowOffset
	YAngle			m_fShadowAngle;			// radians, 0.0=horizontal-right, positive clockwise
	YPercentage		m_fShadowOpacity;			// 0.0=transparent, 1.0=opaque
	RSolidColor		m_fShadowColor;			// shadow color (gets mixed with black at render time)
	YPercentage		m_fShadowEdgeSoftness;	// 0.0=kMinShadowEdgeSoftness ("hard" edge), 1.0=kMaxShadowEdgeSoftness

//	Debugging Code
#ifdef TPSDEBUG
public:
	virtual void	Validate( ) const;
#endif
};


// Operator<< (global)
inline RArchive& operator<<(RArchive& lhs, const RSoftShadowSettings& rhs)
{
	lhs << rhs.m_fShadowOn;
	lhs << rhs.m_fShadowOffset;
	lhs << rhs.m_fShadowAngle;
	lhs << rhs.m_fShadowOpacity;
	lhs << rhs.m_fShadowColor;
	lhs << rhs.m_fShadowEdgeSoftness;
	
	return lhs;
}

// Operator>> (global)
inline RArchive& operator>>(RArchive& lhs, RSoftShadowSettings& rhs)
{
	lhs >> rhs.m_fShadowOn;
	lhs >> rhs.m_fShadowOffset;
	lhs >> rhs.m_fShadowAngle;
	lhs >> rhs.m_fShadowOpacity;
	lhs >> rhs.m_fShadowColor;
	lhs >> rhs.m_fShadowEdgeSoftness;

	return lhs;
}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _SOFTSHADOWSETTINGS_H_
