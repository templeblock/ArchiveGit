// ****************************************************************************
//
//  File Name:			SoftGlowSettings.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of the RSoftGlowSettings class
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef	_SOFTGLOWSETTINGS_H_
#define	_SOFTGLOWSETTINGS_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// GLOW ON
const BOOLEAN kDefaultGlowOn = FALSE;


// GLOW OPACITY
//
// range of percents of glow opacity color:
// 0% glow opacity = kMinGlowOpacityColor of glow color
// 50% glow opacity = kMidPointGlowOpacityColor of glow color
// 100% glow opacity = kMaxGlowOpacityColor of glow color
// intermediate %'s use bilinear interpolation
const YPercentage kMinGlowOpacityColor = 0.18;
const YPercentage kMidPointGlowOpacityColor = 0.45;
const YPercentage kMaxGlowOpacityColor = 1.0;
//
// range of percents of glow opacity white:
// 0% glow opacity = kMinGlowOpacityBlack of white
// 50% glow opacity = kMidPointGlowOpacityBlack of white
// 100% glow opacity = kMaxGlowOpacityBlack of white
// intermediate %'s use bilinear interpolation
const YPercentage kMinGlowOpacityWhite = 0.12;
const YPercentage kMidPointGlowOpacityWhite = 0.3;
const YPercentage kMaxGlowOpacityWhite = 0.0;
//
// range of percents of glow opacity background:
// 0% glow opacity = kMinGlowOpacityBackground of background
// 50% glow opacity = kMidPointGlowOpacityBackground of background
// 100% glow opacity = kMaxGlowOpacityBackground of background
// intermediate %'s use bilinear interpolation
const YPercentage kMinGlowOpacityBackground = 0.7;
const YPercentage kMidPointGlowOpacityBackground = 0.25;
const YPercentage kMaxGlowOpacityBackground = 0.0;
//
// default glow opacity slider percentage setting
const YPercentage kDefaultGlowOpacitySetting = 0.3;


// GLOW COLOR
const RSolidColor kDefaultGlowColor = kYellow;


// GLOW EDGE SOFTNESS
// range of percentages of min of component width,height for glow edge softness:
// 0% glow edge softness = kMinGlowEdgeSoftness of min(componentWidth,componentHeight)
// 50% glow edge softness = kMidPointGlowEdgeSoftness of min(componentWidth,componentHeight)
// 100% glow edge softness = kMaxGlowEdgeSoftness of min(componentWidth,componentHeight)
// intermediate %'s use linear interpolation
const YPercentage kMinGlowEdgeSoftness = 0.04;
const YPercentage kMidPointGlowEdgeSoftness = 0.135;
const YPercentage kMaxGlowEdgeSoftness = 0.23;
const YPercentage kDefaultGlowEdgeSoftness = 0.3;


class FrameworkLinkage RSoftGlowSettings : public RObject
{
// Construction & destruction
public:
						RSoftGlowSettings();
						RSoftGlowSettings( const RSoftGlowSettings& rhs );
						RSoftGlowSettings( BOOLEAN glowOn );

// Operators
public:
	const RSoftGlowSettings& operator=( const RSoftGlowSettings& rhs );

	BOOLEAN 							operator!=( const RSoftGlowSettings& rhs ) const;
	BOOLEAN 							operator==( const RSoftGlowSettings& rhs ) const;

// Public Member data
public:
	BOOLEAN			m_fGlowOn;				// TRUE=enable glow, FALSE=disable glow
	YPercentage		m_fGlowOpacity;		// 0.0=transparent, 1.0=opaque
	RSolidColor		m_fGlowColor;			// glow color (gets mixed with white at render time)
	YPercentage		m_fGlowEdgeSoftness;	// 0.0=kMinGlowEdgeSoftness ("hard" edge), 1.0=kMaxGlowEdgeSoftness

//	Debugging Code
#ifdef TPSDEBUG
public:
	virtual void	Validate( ) const;
#endif
};


// Operator<< (global)
inline RArchive& operator<<(RArchive& lhs, const RSoftGlowSettings& rhs)
{
	lhs << rhs.m_fGlowOn;
	lhs << rhs.m_fGlowOpacity;
	lhs << rhs.m_fGlowColor;
	lhs << rhs.m_fGlowEdgeSoftness;
	
	return lhs;
}

// Operator>> (global)
inline RArchive& operator>>(RArchive& lhs, RSoftGlowSettings& rhs)
{
	lhs >> rhs.m_fGlowOn;
	lhs >> rhs.m_fGlowOpacity;
	lhs >> rhs.m_fGlowColor;
	lhs >> rhs.m_fGlowEdgeSoftness;

	return lhs;
}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _SOFTGLOWSETTINGS_H_
