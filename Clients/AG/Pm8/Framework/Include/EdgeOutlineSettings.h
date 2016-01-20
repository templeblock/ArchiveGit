// ****************************************************************************
//
//  File Name:			EdgeOutlineSettings.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				Wynn
//
//  Description:		Declaration of the REdgeOutlineSettings class
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef	_EDGEOUTLINESETTINGS_H_
#define	_EDGEOUTLINESETTINGS_H_

const BOOLEAN kDefaultEdgeOutlineOn = FALSE;

const	kDefaultEdgeOutlineWidth = 1;
const	kDefaultEdgeOutlineColor = kRed;
class FrameworkLinkage REdgeOutlineSettings : public RObject
{
// Construction & destruction
public:
						REdgeOutlineSettings();
						REdgeOutlineSettings( const REdgeOutlineSettings& rhs );
						REdgeOutlineSettings( int nEdgeOutlineWidth, RSolidColor rcEdgeOulineColor );
						REdgeOutlineSettings( BOOLEAN edgeOutlineOn );

// Operators
public:
	const REdgeOutlineSettings& operator=( const REdgeOutlineSettings& rhs );

	BOOLEAN 							operator!=( const REdgeOutlineSettings& rhs ) const;
	BOOLEAN 							operator==( const REdgeOutlineSettings& rhs ) const;

// Public Member data
public:
	BOOLEAN			m_bEdgeOutlineOn;					// TRUE=enable edgeOutline, FALSE=disable edgeOutline
	YPercentage		m_nEdgeOutlineWidth;
	RSolidColor		m_rcEdgeOutlineColor;


//	Debugging Code
#ifdef TPSDEBUG
public:
	virtual void	Validate( ) const;
#endif
};


// Operator<< (global)
inline RArchive& operator<<(RArchive& lhs, const REdgeOutlineSettings& rhs)
{
	lhs << rhs.m_bEdgeOutlineOn;
	lhs << rhs.m_nEdgeOutlineWidth;
	lhs << rhs.m_rcEdgeOutlineColor;
	
	return lhs;
}

// Operator>> (global)
inline RArchive& operator>>(RArchive& lhs, REdgeOutlineSettings& rhs)
{
	lhs >> rhs.m_bEdgeOutlineOn;
	lhs >> rhs.m_nEdgeOutlineWidth;
	lhs >> rhs.m_rcEdgeOutlineColor;
	return lhs;
}


#endif	// _EDGEOUTLINESETTINGS_H_
