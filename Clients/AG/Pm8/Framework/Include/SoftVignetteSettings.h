// ****************************************************************************
//
//  File Name:			SoftVignetteSettings.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of the RSoftVignetteSettings class
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef	_SOFTVIGNETTESETTINGS_H_
#define	_SOFTVIGNETTESETTINGS_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif


// VIGNETTE ON
const BOOLEAN kDefaultVignetteOn = FALSE;


// VIGNETTE EDGE SOFTNESS
// range of percentages of min of component width,height for vignette edge softness:
// 0% vignette edge softness = kMinVignetteEdgeSoftness of min(componentWidth,componentHeight)
// 50% vignette edge softness = kMidPointVignetteEdgeSoftness of min(componentWidth,componentHeight)
// 100% vignette edge softness = kMaxVignetteEdgeSoftness of min(componentWidth,componentHeight)
// intermediate %'s use linear interpolation
const YPercentage kMinVignetteEdgeSoftness = 0.0;
const YPercentage kMidPointVignetteEdgeSoftness = 0.05;
const YPercentage kMaxVignetteEdgeSoftness = 0.1;
const YPercentage kDefaultVignetteEdgeSoftness = 0.1;


class FrameworkLinkage RSoftVignetteSettings : public RObject
{
// Construction & destruction
public:
						RSoftVignetteSettings();
						RSoftVignetteSettings( const RSoftVignetteSettings& rhs );
						RSoftVignetteSettings( BOOLEAN vignetteOn );

// Operators
public:
	const RSoftVignetteSettings& operator=( const RSoftVignetteSettings& rhs );

	BOOLEAN 							operator!=( const RSoftVignetteSettings& rhs ) const;
	BOOLEAN 							operator==( const RSoftVignetteSettings& rhs ) const;

// Public Member data
public:
	BOOLEAN			m_fVignetteOn;					// TRUE=enable vignette, FALSE=disable vignette
	YPercentage		m_fVignetteEdgeSoftness;	// 0.0=kMinVignetteEdgeSoftness ("hard" edge), 1.0=kMaxVignetteEdgeSoftness

//	Debugging Code
#ifdef TPSDEBUG
public:
	virtual void	Validate( ) const;
#endif
};


// Operator<< (global)
inline RArchive& operator<<(RArchive& lhs, const RSoftVignetteSettings& rhs)
{
	lhs << rhs.m_fVignetteOn;
	lhs << rhs.m_fVignetteEdgeSoftness;
	
	return lhs;
}

// Operator>> (global)
inline RArchive& operator>>(RArchive& lhs, RSoftVignetteSettings& rhs)
{
	lhs >> rhs.m_fVignetteOn;
	lhs >> rhs.m_fVignetteEdgeSoftness;

	return lhs;
}


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _SOFTVIGNETTESETTINGS_H_
