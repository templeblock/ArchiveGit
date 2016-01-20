// ****************************************************************************
//
//  File Name:			SoftShadowSettings.cpp
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Definition of the RSoftShadowSettings class
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "SoftShadowSettings.h"


/****************************************************************************
	RSoftShadowSettings::RSoftShadowSettings()

	Default constructor for an RSoftShadowSettings
*****************************************************************************/

RSoftShadowSettings::RSoftShadowSettings()
	:	m_fShadowOn( kDefaultShadowOn ),
		m_fShadowOffset( kDefaultShadowOffset ),
		m_fShadowAngle( DegreesToRadians( kDefaultShadowAngle ) ),
		m_fShadowOpacity( kDefaultShadowOpacitySetting ),
		m_fShadowColor( kDefaultShadowColor ),
		m_fShadowEdgeSoftness( kDefaultShadowEdgeSoftness )
{
#ifdef TPSDEBUG
	Validate();
#endif
}

/****************************************************************************
	RSoftShadowSettings::RSoftShadowSettings( const RSoftShadowSettings& rhs )

	Copy constructor for an RSoftShadowSettings
*****************************************************************************/

RSoftShadowSettings::RSoftShadowSettings( const RSoftShadowSettings& rhs )
	:	m_fShadowOn( rhs.m_fShadowOn ),
		m_fShadowOffset( rhs.m_fShadowOffset ),
		m_fShadowAngle( rhs.m_fShadowAngle ),
		m_fShadowOpacity( rhs.m_fShadowOpacity ),
		m_fShadowColor( rhs.m_fShadowColor ),
		m_fShadowEdgeSoftness( rhs.m_fShadowEdgeSoftness )
{
#ifdef TPSDEBUG
	Validate();
#endif
}

/****************************************************************************
	RSoftShadowSettings::RSoftShadowSettings( BOOLEAN shadowOn )

	Constructor for an RSoftShadowSettings from a BOOLEAN
*****************************************************************************/

RSoftShadowSettings::RSoftShadowSettings( BOOLEAN shadowOn )
	:	m_fShadowOn( shadowOn ),
		m_fShadowOffset( kDefaultShadowOffset ),
		m_fShadowAngle( DegreesToRadians( kDefaultShadowAngle ) ),
		m_fShadowOpacity( kDefaultShadowOpacitySetting ),
		m_fShadowColor( kDefaultShadowColor ),
		m_fShadowEdgeSoftness( kDefaultShadowEdgeSoftness )
{
#ifdef TPSDEBUG
	Validate();
#endif
}

/****************************************************************************
	const RSoftShadowSettings& RSoftShadowSettings::operator=( const RSoftShadowSettings& rhs )

	Assignment operator for an RSoftShadowSettings
*****************************************************************************/

const RSoftShadowSettings& RSoftShadowSettings::operator=( const RSoftShadowSettings& rhs )
{
#ifdef TPSDEBUG
	rhs.Validate();
	Validate();
#endif
	if (&rhs != this)
	{
		m_fShadowOn = rhs.m_fShadowOn;
		m_fShadowOffset = rhs.m_fShadowOffset;
		m_fShadowAngle = rhs.m_fShadowAngle;
		m_fShadowOpacity = rhs.m_fShadowOpacity;
		m_fShadowColor = rhs.m_fShadowColor;
		m_fShadowEdgeSoftness = rhs.m_fShadowEdgeSoftness;
	}
	return *this;
}

/****************************************************************************
	BOOLEAN RSoftShadowSettings::operator==( const RSoftShadowSettings& rhs ) const

	Equality operator for an RSoftShadowSettings
*****************************************************************************/

BOOLEAN RSoftShadowSettings::operator==( const RSoftShadowSettings& rhs ) const
{
#ifdef TPSDEBUG
	rhs.Validate();
	Validate();
#endif
	if (&rhs != this)
	{
		if (m_fShadowOn == rhs.m_fShadowOn &&
			m_fShadowOffset == rhs.m_fShadowOffset &&
			m_fShadowAngle == rhs.m_fShadowAngle &&
			m_fShadowOpacity == rhs.m_fShadowOpacity &&
			m_fShadowColor == rhs.m_fShadowColor &&
			m_fShadowEdgeSoftness == rhs.m_fShadowEdgeSoftness)
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}

/****************************************************************************
	BOOLEAN RSoftShadowSettings::operator!=( const RSoftShadowSettings& rhs ) const

	Inequality operator for an RSoftShadowSettings
*****************************************************************************/

BOOLEAN RSoftShadowSettings::operator!=( const RSoftShadowSettings& rhs ) const
{
#ifdef TPSDEBUG
	rhs.Validate();
	Validate();
#endif
	if (&rhs != this)
	{
		if (m_fShadowOn != rhs.m_fShadowOn ||
			m_fShadowOffset != rhs.m_fShadowOffset ||
			m_fShadowAngle != rhs.m_fShadowAngle ||
			m_fShadowOpacity != rhs.m_fShadowOpacity ||
			m_fShadowColor != rhs.m_fShadowColor ||
			m_fShadowEdgeSoftness != rhs.m_fShadowEdgeSoftness)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

#ifdef TPSDEBUG

/****************************************************************************
	void RSoftShadowSettings::Validate() const

	Validation member function for an RSoftShadowSettings
*****************************************************************************/

void RSoftShadowSettings::Validate() const
{
	RObject::Validate();
	TpsAssertIsObject( RSoftShadowSettings, this );
	TpsAssert( 0.0 <= m_fShadowOffset && m_fShadowOffset <= 1.0, "Soft shadow offset not in range 0 - 1" );
	TpsAssert( 0.0 <= m_fShadowOpacity && m_fShadowOpacity <= 1.0, "Soft shadow opacity not in range 0 - 1" );
	TpsAssert( 0.0 <= m_fShadowEdgeSoftness && m_fShadowEdgeSoftness <= 1.0, "Soft shadow edge softness not in range 0 - 1" );
}

#endif // TPSDEBUG

