//****************************************************************************
//
// File Name:			ColorLUTs.h
//
// Project:				Renaissance
//
// Author:				John Fleischhauer
//
// Description:			RColorLUTs declaration
//
// Portability:			Windows
//
// Copyright (C) 1998 Broderbund Software, Inc.  All Rights Reserved.
//
//
// *****************************************************************************

#ifndef	_CASTLUTS_H_
#define	_CASTLUTS_H_

#include "ImageLibrary.h"

typedef sLONG	YHueAngle;

struct YRealRGB
{
	YFloatType red;
	YFloatType green;
	YFloatType blue;
};

const int kLUTSize = 256;

// This class for color adjust
class RCastLUTs : public RObject
{
	public:
						RCastLUTs();
						RCastLUTs( YHueAngle hueAngle, YFloatType effectFraction, BOOL bCoarseFlag );
						~RCastLUTs();

		void			SetHueAngle( YHueAngle angle ) { m_hueAngle = angle; }
		YHueAngle		GetHueAngle() { return m_hueAngle; }

		void			SetEffectFraction( YFloatType fraction ) { m_effectFraction = fraction; }
		YFloatType		GetEffectFraction() { return m_effectFraction; }

		void			Coarse( BOOL bCoarse ){ m_bCoarseFlag = bCoarse; }
		BOOL			IsCoarse(){ return m_bCoarseFlag; }

		BOOL			BuildLUTs();
		BOOL			GetLUTs( YLUTComponent **pRedLUT, YLUTComponent **pGreenLUT, YLUTComponent **pBlueLUT );

		BOOL			Initialized() { return m_bInitialized; }
		void			Uninitialize() { m_bInitialized = FALSE; }

		const RCastLUTs& RCastLUTs::operator=( RCastLUTs& src );

	private:
		BOOL			m_bInitialized;

		YLUTComponent	m_redLUT[ kLUTSize ];
		YLUTComponent	m_greenLUT[ kLUTSize ];
		YLUTComponent	m_blueLUT[ kLUTSize ];

		YHueAngle		m_hueAngle;
		YFloatType		m_effectFraction;

		BOOL			m_bCoarseFlag;

		void			SetComponentProportions ( YRealRGB *pComponentProportions );
		void			CastComponentLUTs( YRealRGB hueProportions, YFloatType *maxLUT );

		YFloatType		DegreesToRadians( YHueAngle angle );
		YHueAngle		RoundToInt( YFloatType fltval );


#ifdef	TPSDEBUG
	public:
		virtual void Validate() const;
#endif

};

// Parallel class for brightness and contrast
class RToneLUTs : public RObject
{
	public:
						RToneLUTs();
						RToneLUTs( YFloatType brightnessFraction, YFloatType contrastFraction, BOOL bCoarseFlag );
						~RToneLUTs();

		//void			SetHueAngle( YHueAngle angle ) { m_hueAngle = angle; }
		//YHueAngle		GetHueAngle() { return m_hueAngle; }

		//void			SetEffectFraction( YFloatType fraction ) { m_effectFraction = fraction; }
		//YFloatType		GetEffectFraction() { return m_effectFraction; }

		void			SetBrightnessFraction( YFloatType fraction ) { m_brightnessFraction = fraction; }
		YFloatType		GetBrightnessFraction() { return m_brightnessFraction; }

		void			SetContrastFraction( YFloatType fraction ) { m_contrastFraction = fraction; }
		YFloatType		GetContrastFraction() { return m_contrastFraction; }



		void			Coarse( BOOL bCoarse ){ m_bCoarseFlag = bCoarse; }
		BOOL			IsCoarse(){ return m_bCoarseFlag; }

		BOOL			BuildLUTs();
		BOOL			GetLUTs( YLUTComponent **pRedLUT, YLUTComponent **pGreenLUT, YLUTComponent **pBlueLUT );

		BOOL			Initialized() { return m_bInitialized; }
		void			Uninitialize() { m_bInitialized = FALSE; }

		const RToneLUTs& RToneLUTs::operator=( RToneLUTs& src );

	private:
		BOOL			m_bInitialized;

		YLUTComponent	m_redLUT[ kLUTSize ];
		YLUTComponent	m_greenLUT[ kLUTSize ];
		YLUTComponent	m_blueLUT[ kLUTSize ];

		//YHueAngle		m_hueAngle;
		//YFloatType	m_effectFraction;
		YFloatType		m_brightnessFraction;
		YFloatType		m_contrastFraction;


		BOOL			m_bCoarseFlag;

		//void			SetComponentProportions ( YRealRGB *pComponentProportions );
		//void			CastComponentLUTs( YRealRGB hueProportions, YFloatType *maxLUT );

		//YFloatType		DegreesToRadians( YHueAngle angle );
		YHueAngle		RoundToInt( YFloatType fltval );


#ifdef	TPSDEBUG
	public:
		virtual void Validate() const;
#endif

};

#endif		// _CASTLUTS_H_
