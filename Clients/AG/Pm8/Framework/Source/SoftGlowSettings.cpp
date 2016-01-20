// ****************************************************************************
//
//  File Name:			SoftGlowSettings.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				Greg Beddow
//
//  Description:		Definition of the RSoftGlowSettings class
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "SoftGlowSettings.h"


/****************************************************************************
	RSoftGlowSettings::RSoftGlowSettings()

	Default constructor for an RSoftGlowSettings
*****************************************************************************/

RSoftGlowSettings::RSoftGlowSettings()
	:	m_fGlowOn( kDefaultGlowOn ),
		m_fGlowOpacity( kDefaultGlowOpacitySetting ),
		m_fGlowColor( kDefaultGlowColor ),
		m_fGlowEdgeSoftness( kDefaultGlowEdgeSoftness )
{
#ifdef TPSDEBUG
	Validate();
#endif
}

/****************************************************************************
	RSoftGlowSettings::RSoftGlowSettings( const RSoftGlowSettings& rhs )

	Copy constructor for an RSoftGlowSettings
*****************************************************************************/

RSoftGlowSettings::RSoftGlowSettings( const RSoftGlowSettings& rhs )
	:	m_fGlowOn( rhs.m_fGlowOn ),
		m_fGlowOpacity( rhs.m_fGlowOpacity ),
		m_fGlowColor( rhs.m_fGlowColor ),
		m_fGlowEdgeSoftness( rhs.m_fGlowEdgeSoftness )
{
#ifdef TPSDEBUG
	Validate();
#endif
}

/****************************************************************************
	RSoftGlowSettings::RSoftGlowSettings( BOOLEAN glowOn )

	Constructor for an RSoftGlowSettings from a BOOLEAN
*****************************************************************************/

RSoftGlowSettings::RSoftGlowSettings( BOOLEAN glowOn )
	:	m_fGlowOn( glowOn ),
		m_fGlowOpacity( kDefaultGlowOpacitySetting ),
		m_fGlowColor( kDefaultGlowColor ),
		m_fGlowEdgeSoftness( kDefaultGlowEdgeSoftness )
{
#ifdef TPSDEBUG
	Validate();
#endif
}

/****************************************************************************
	const RSoftGlowSettings& RSoftGlowSettings::operator=( const RSoftGlowSettings& rhs )

	Assignment operator for an RSoftGlowSettings
*****************************************************************************/

const RSoftGlowSettings& RSoftGlowSettings::operator=( const RSoftGlowSettings& rhs )
{
#ifdef TPSDEBUG
	rhs.Validate();
	Validate();
#endif
	if (&rhs != this)
	{
		m_fGlowOn = rhs.m_fGlowOn;
		m_fGlowOpacity = rhs.m_fGlowOpacity;
		m_fGlowColor = rhs.m_fGlowColor;
		m_fGlowEdgeSoftness = rhs.m_fGlowEdgeSoftness;
	}
	return *this;
}

/****************************************************************************
	BOOLEAN RSoftGlowSettings::operator==( const RSoftGlowSettings& rhs ) const

	Equality operator for an RSoftGlowSettings
*****************************************************************************/

BOOLEAN RSoftGlowSettings::operator==( const RSoftGlowSettings& rhs ) const
{
#ifdef TPSDEBUG
	rhs.Validate();
	Validate();
#endif
	if (&rhs != this)
	{
		if (m_fGlowOn == rhs.m_fGlowOn &&
			m_fGlowOpacity == rhs.m_fGlowOpacity &&
			m_fGlowColor == rhs.m_fGlowColor &&
			m_fGlowEdgeSoftness == rhs.m_fGlowEdgeSoftness)
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}

/****************************************************************************
	BOOLEAN RSoftGlowSettings::operator!=( const RSoftGlowSettings& rhs ) const

	Inequality operator for an RSoftGlowSettings
*****************************************************************************/

BOOLEAN RSoftGlowSettings::operator!=( const RSoftGlowSettings& rhs ) const
{
#ifdef TPSDEBUG
	rhs.Validate();
	Validate();
#endif
	if (&rhs != this)
	{
		if (m_fGlowOn != rhs.m_fGlowOn ||
			m_fGlowOpacity != rhs.m_fGlowOpacity ||
			m_fGlowColor != rhs.m_fGlowColor ||
			m_fGlowEdgeSoftness != rhs.m_fGlowEdgeSoftness)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

#ifdef TPSDEBUG

/****************************************************************************
	void RSoftGlowSettings::Validate() const

	Validation member function for an RSoftGlowSettings
*****************************************************************************/

void RSoftGlowSettings::Validate() const
{
	RObject::Validate();
	TpsAssertIsObject( RSoftGlowSettings, this );
	TpsAssert( 0.0 <= m_fGlowOpacity && m_fGlowOpacity <= 1.0, "Soft glow opacity not in range 0 - 1" );
	TpsAssert( 0.0 <= m_fGlowEdgeSoftness && m_fGlowEdgeSoftness <= 1.0, "Soft glow edge softness not in range 0 - 1" );
}

#endif // TPSDEBUG

