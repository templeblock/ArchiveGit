//****************************************************************************
//
// File Name:			HexColorGrid.h
//
// Project:				Renaissance
//
// Author:				John Fleischhauer
//
// Description:			RHexColorGrid declaration
//
// Portability:			Windows
//
// Copyright (C) 1998 Broderbund Software, Inc.  All Rights Reserved.
//
//
// *****************************************************************************

#ifndef	_HEXCOLORGRID_H_
#define	_HEXCOLORGRID_H_

#include "CastLUTs.h"
// should some of these be in RImageLibrary?
//typedef sLONG	YHueAngle;

// definition of a RGB triplet array for denoting a position on the grid
typedef uLONG YHexCoord[3];

// and some indices	into the array
typedef enum EHexCoordIndex
{
	kIndexRed = 0,
	kIndexGreen,
	kIndexBlue
};	

// the 6 primary directions on the grid
typedef enum EDirection
{
	kDirectionRed = 0,
	kDirectionYellow,
	kDirectionGreen,
	kDirectionCyan,
	kDirectionBlue,
	kDirectionMagenta,
	kDirectionNone
};	

// the primary angles on the hex color grid
const YHueAngle kPrimaryAngle[] =
{
	  0,	// Red
	 60,	// Yellow
	120,	// Green
	180,	// Cyan
	240,	// Blue
	300		// Magenta
};

const YHueAngle kAngleBetweenPrimaries = 60;


class RHexColorGrid : public RObject
{
	public:
				RHexColorGrid( int levels );
				RHexColorGrid( RHexColorGrid& src );
				~RHexColorGrid();

		void 	NextPosition( EDirection direction );
//		void 	GetAngle( double *pAngle, double *pEffectFraction );
		void 	GetAngle( YHueAngle *pAngle, YFloatType *pEffectFraction );

		void 	Reset();

		const RHexColorGrid& RHexColorGrid::operator=( RHexColorGrid& src );

	private:
		YHexCoord	m_hexCoord;
		uLONG		m_maxIndex;

#ifdef	TPSDEBUG
	public:
		virtual void Validate() const;
#endif

};						

#endif		// _HEXCOLORGRID_H_
