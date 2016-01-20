// ****************************************************************************
//
//  File Name:			EdgeOutlineSettings.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				Wynn
//
//  Description:		Definition of the REdgeOutlineSettings class
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "EdgeOutlineSettings.h"


/****************************************************************************
	REdgeOutlineSettings::REdgeOutlineSettings()

	Default constructor for an REdgeOutlineSettings
*****************************************************************************/

REdgeOutlineSettings::REdgeOutlineSettings()
	:	m_bEdgeOutlineOn(kDefaultEdgeOutlineOn),
		m_nEdgeOutlineWidth(kDefaultEdgeOutlineWidth),
		m_rcEdgeOutlineColor(EColors(kDefaultEdgeOutlineColor))
{
#ifdef TPSDEBUG
	Validate();
#endif
}

/****************************************************************************
	REdgeOutlineSettings::REdgeOutlineSettings(const REdgeOutlineSettings& rhs)

	Copy constructor for an REdgeOutlineSettings
*****************************************************************************/

REdgeOutlineSettings::REdgeOutlineSettings(const REdgeOutlineSettings& rhs)
	:	m_bEdgeOutlineOn(rhs.m_bEdgeOutlineOn),
		m_nEdgeOutlineWidth(rhs.m_nEdgeOutlineWidth),
		m_rcEdgeOutlineColor(rhs.m_rcEdgeOutlineColor)

{
#ifdef TPSDEBUG
	Validate();
#endif
}
/****************************************************************************
	REdgeOutlineSettings::REdgeOutlineSettings(int fOutlineEdgeWidth, RSolidColor rcOutlineEdgeColor)

	Constructor for an REdgeOutlineSettings from a width and color
*****************************************************************************/

REdgeOutlineSettings::REdgeOutlineSettings(int fOutlineEdgeWidth, RSolidColor rcOutlineEdgeColor)
	:	m_bEdgeOutlineOn(TRUE),
		m_nEdgeOutlineWidth(fOutlineEdgeWidth),
		m_rcEdgeOutlineColor(rcOutlineEdgeColor)
{
#ifdef TPSDEBUG
	Validate();
#endif
}


/****************************************************************************
	REdgeOutlineSettings::REdgeOutlineSettings(BOOLEAN egdeOutlineOn)

	Constructor for an REdgeOutlineSettings from a BOOLEAN
*****************************************************************************/

REdgeOutlineSettings::REdgeOutlineSettings(BOOLEAN outlineEdgeOn)
	:	m_bEdgeOutlineOn(outlineEdgeOn),
		m_nEdgeOutlineWidth(kDefaultEdgeOutlineWidth),
		m_rcEdgeOutlineColor(kDefaultEdgeOutlineColor)
{
#ifdef TPSDEBUG
	Validate();
#endif
}

/****************************************************************************
	const REdgeOutlineSettings& REdgeOutlineSettings::operator=(const REdgeOutlineSettings& rhs)


	Assignment operator for an REdgeOutlineSettings
*****************************************************************************/

const REdgeOutlineSettings& REdgeOutlineSettings::operator=(const REdgeOutlineSettings& rhs)
{
#ifdef TPSDEBUG
	rhs.Validate();
	Validate();
#endif
	if(&rhs != this)
	{
		m_bEdgeOutlineOn = rhs.m_bEdgeOutlineOn;
		m_nEdgeOutlineWidth = rhs.m_nEdgeOutlineWidth;
		m_rcEdgeOutlineColor = rhs.m_rcEdgeOutlineColor;
	}
	return *this;
}

/****************************************************************************
	BOOLEAN REdgeOutlineSettings::operator==(const REdgeOutlineSettings& rhs) const

	Equality operator for an REdgeOutlineSettings
*****************************************************************************/

BOOLEAN REdgeOutlineSettings::operator==(const REdgeOutlineSettings& rhs) const
{
#ifdef TPSDEBUG
	rhs.Validate();
	Validate();
#endif
	if(&rhs != this)
	{
		if(m_bEdgeOutlineOn == rhs.m_bEdgeOutlineOn &&
			m_nEdgeOutlineWidth == rhs.m_nEdgeOutlineWidth &&
			m_rcEdgeOutlineColor == rhs.m_rcEdgeOutlineColor)
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}

/****************************************************************************
	BOOLEAN REdgeOutlineSettings::operator!=(const REdgeOutlineSettings& rhs) const

	Inequality operator for an REdgeOutlineSettings
*****************************************************************************/

BOOLEAN REdgeOutlineSettings::operator!=(const REdgeOutlineSettings& rhs) const
{
#ifdef TPSDEBUG
	rhs.Validate();
	Validate();
#endif
	if(&rhs != this)
	{
		if(m_bEdgeOutlineOn != rhs.m_bEdgeOutlineOn ||
			m_nEdgeOutlineWidth != rhs.m_nEdgeOutlineWidth ||
			m_rcEdgeOutlineColor != rhs.m_rcEdgeOutlineColor)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

#ifdef TPSDEBUG

/****************************************************************************
	void REdgeOutlineSettings::Validate() const

	Validation member function for an REdgeOutlineSettings
*****************************************************************************/

void REdgeOutlineSettings::Validate() const
{
	RObject::Validate();
	TpsAssertIsObject(REdgeOutlineSettings, this);
	TpsAssert(0 <= m_nEdgeOutlineWidth && m_nEdgeOutlineWidth <= 7, "Outline edge width not in range 0 - 7");
	TpsAssert(0 <= m_rcEdgeOutlineColor && m_rcEdgeOutlineColor <= 0x00ffffff, "Outline edge color in range 0 - 0x00ffffff");
}

#endif// TPSDEBUG

