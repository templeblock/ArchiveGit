// ****************************************************************************
//
//  File Name:			SoftVignetteSettings.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				Greg Beddow
//
//  Description:		Definition of the RSoftVignetteSettings class
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

#include "SoftVignetteSettings.h"


/****************************************************************************
	RSoftVignetteSettings::RSoftVignetteSettings()

	Default constructor for an RSoftVignetteSettings
*****************************************************************************/

RSoftVignetteSettings::RSoftVignetteSettings()
	:	m_fVignetteOn( kDefaultVignetteOn ),
		m_fVignetteEdgeSoftness( kDefaultVignetteEdgeSoftness )
{
#ifdef TPSDEBUG
	Validate();
#endif
}

/****************************************************************************
	RSoftVignetteSettings::RSoftVignetteSettings( const RSoftVignetteSettings& rhs )

	Copy constructor for an RSoftVignetteSettings
*****************************************************************************/

RSoftVignetteSettings::RSoftVignetteSettings( const RSoftVignetteSettings& rhs )
	:	m_fVignetteOn( rhs.m_fVignetteOn ),
		m_fVignetteEdgeSoftness( rhs.m_fVignetteEdgeSoftness )
{
#ifdef TPSDEBUG
	Validate();
#endif
}

/****************************************************************************
	RSoftVignetteSettings::RSoftVignetteSettings( BOOLEAN vignetteOn )

	Constructor for an RSoftVignetteSettings from a BOOLEAN
*****************************************************************************/

RSoftVignetteSettings::RSoftVignetteSettings( BOOLEAN vignetteOn )
	:	m_fVignetteOn( vignetteOn ),
		m_fVignetteEdgeSoftness( kDefaultVignetteEdgeSoftness )
{
#ifdef TPSDEBUG
	Validate();
#endif
}

/****************************************************************************
	const RSoftVignetteSettings& RSoftVignetteSettings::operator=( const RSoftVignetteSettings& rhs )

	Assignment operator for an RSoftVignetteSettings
*****************************************************************************/

const RSoftVignetteSettings& RSoftVignetteSettings::operator=( const RSoftVignetteSettings& rhs )
{
#ifdef TPSDEBUG
	rhs.Validate();
	Validate();
#endif
	if (&rhs != this)
	{
		m_fVignetteOn = rhs.m_fVignetteOn;
		m_fVignetteEdgeSoftness = rhs.m_fVignetteEdgeSoftness;
	}
	return *this;
}

/****************************************************************************
	BOOLEAN RSoftVignetteSettings::operator==( const RSoftVignetteSettings& rhs ) const

	Equality operator for an RSoftVignetteSettings
*****************************************************************************/

BOOLEAN RSoftVignetteSettings::operator==( const RSoftVignetteSettings& rhs ) const
{
#ifdef TPSDEBUG
	rhs.Validate();
	Validate();
#endif
	if (&rhs != this)
	{
		if (m_fVignetteOn == rhs.m_fVignetteOn &&
			m_fVignetteEdgeSoftness == rhs.m_fVignetteEdgeSoftness)
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}

/****************************************************************************
	BOOLEAN RSoftVignetteSettings::operator!=( const RSoftVignetteSettings& rhs ) const

	Inequality operator for an RSoftVignetteSettings
*****************************************************************************/

BOOLEAN RSoftVignetteSettings::operator!=( const RSoftVignetteSettings& rhs ) const
{
#ifdef TPSDEBUG
	rhs.Validate();
	Validate();
#endif
	if (&rhs != this)
	{
		if (m_fVignetteOn != rhs.m_fVignetteOn ||
			m_fVignetteEdgeSoftness != rhs.m_fVignetteEdgeSoftness)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

#ifdef TPSDEBUG

/****************************************************************************
	void RSoftVignetteSettings::Validate() const

	Validation member function for an RSoftVignetteSettings
*****************************************************************************/

void RSoftVignetteSettings::Validate() const
{
	RObject::Validate();
	TpsAssertIsObject( RSoftVignetteSettings, this );
	TpsAssert( 0.0 <= m_fVignetteEdgeSoftness && m_fVignetteEdgeSoftness <= 1.0, "Soft vignette edge softness not in range 0 - 1" );
}

#endif // TPSDEBUG

