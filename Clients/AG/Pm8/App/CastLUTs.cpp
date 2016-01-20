//****************************************************************************
//
// File Name:			CastLUTs.cpp
//
// Project:				Renaissance
//
// Author:				John Fleischhauer
//
// Description:			RCastLUTs definition
//
// Portability:			Windows
//
// Copyright (C) 1998 Broderbund Software, Inc.  All Rights Reserved.
//
//
// *****************************************************************************

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME

#include "CastLUTs.h"

static YFloatType MaxGenericOneHumpLUT[256] = { 
.000,.004,.008,.012, .016,.020,.024,.027, .031,.035,.039,.043, .047,.051,.055,.059,
.063,.066,.069,.073, .077,.080,.083,.086, .090,.093,.097,.100, .103,.107,.110,.113, 
.117,.120,.124,.127, .131,.134,.138,.141, .145,.148,.151,.154, .157,.160,.163,.166, 
.169,.172,.175,.178, .181,.184,.187,.190, .193,.196,.199,.202, .205,.208,.211,.214, 
.217,.220,.223,.226, .229,.232,.235,.238, .240,.243,.246,.249, .251,.254,.256,.259, 
.262,.264,.267,.269, .271,.274,.276,.278, .280,.283,.285,.287, .289,.291,.293,.295,
.297,.298,.300,.302, .304,.305,.307,.309, .310,.311,.313,.314, .316,.317,.318,.319,
.320,.321,.322,.323, .324,.325,.326,.327, .328,.328,.329,.329, .329,.330,.330,.330,
.330,.330,.330,.329, .329,.329,.328,.328, .327,.326,.325,.324, .323,.322,.321,.320,
.319,.318,.317,.316, .314,.313,.311,.310, .309,.307,.305,.304, .302,.300,.298,.297,
.295,.293,.291,.289, .287,.285,.283,.280, .278,.276,.274,.271, .269,.267,.264,.262,
.259,.256,.254,.251, .249,.246,.243,.240, .238,.235,.232,.229, .226,.223,.220,.217,
.214,.211,.208,.205, .202,.199,.196,.193, .190,.187,.184,.181, .178,.175,.172,.169,
.166,.163,.160,.157, .154,.151,.148,.145, .141,.138,.134,.131, .127,.124,.120,.117,
.113,.110,.107,.103, .100,.097,.093,.090, .086,.083,.080,.077, .073,.069,.066,.063,
.059,.055,.051,.047, .043,.039,.035,.031, .027,.024,.020,.016, .012,.008,.004,.000 };

static YFloatType MaxGenericOgeeLUT[256] = { 
-.000,-.004,-.007,-.011, -.014,-.018,-.021,-.025, -.028,-.032,-.035,-.039, -.042,-.046,-.049,-.052,
-.055,-.058,-.061,-.064, -.068,-.071,-.074,-.077, -.081,-.084,-.087,-.089, -.092,-.095,-.097,-.100,
-.103,-.105,-.108,-.110, -.112,-.114,-.115,-.116, -.117,-.118,-.118,-.119, -.119,-.119,-.120,-.120,
-.120,-.120,-.119,-.119, -.119,-.118,-.118,-.117, -.117,-.116,-.116,-.115, -.114,-.113,-.112,-.111,
-.109,-.107,-.106,-.104, -.103,-.101,-.100,-.098, -.097,-.095,-.094,-.092, -.090,-.088,-.087,-.085,
-.083,-.081,-.080,-.078, -.076,-.074,-.073,-.071, -.070,-.068,-.067,-.065, -.063,-.061,-.060,-.058,
-.056,-.054,-.053,-.051, -.049,-.047,-.046,-.044, -.042,-.040,-.038,-.036, -.034,-.032,-.031,-.029,
-.027,-.025,-.024,-.022, -.020,-.018,-.017,-.015, -.013,-.011,-.010,-.008, -.006,-.004,-.003,-.001,
.001,.003,.004,.006, .008,.010,.011,.013, .015,.017,.018,.020, .022,.024,.025,.027,
.029,.031,.032,.034, .036,.038,.040,.042, .044,.046,.047,.049, .051,.053,.054,.056,
.058,.060,.061,.063, .065,.067,.068,.070, .071,.073,.074,.076, .078,.080,.081,.083,
.085,.087,.088,.090, .092,.094,.095,.097, .098,.100,.101,.103, .104,.106,.107,.109,
.111,.112,.113,.114, .115,.116,.116,.117, .117,.118,.118,.119, .119,.119,.120,.120,
.120,.120,.119,.119, .119,.118,.118,.117, .116,.115,.114,.112, .110,.108,.105,.103,
.100,.097,.095,.092, .089,.087,.084,.081, .077,.074,.071,.068, .064,.061,.058,.055,
.052,.049,.046,.042, .039,.035,.032,.028, .025,.021,.018,.014, .011,.007,.004,.000 };

#define MAX_DRAMATIC_BRIGHTCAST_VAL .5
#define MAX_POS_CONTRAST_END 2.0
#define MAX_POS_CONTRAST_START -2.0
#define MAX_NEG_CONTRAST_END .4
#define MAX_NEG_CONTRAST_START -.4
#define LIKELY_OVERFLO_INDEX 216
#define LIKELY_UNDRFLO_INDEX 40

#define REDANGLE 0
#define GRNANGLE 120
#define BLUANGLE 240


RCastLUTs::RCastLUTs()
	: m_hueAngle(0),
	  m_effectFraction(0.0),
	  m_bCoarseFlag(FALSE),
	  m_bInitialized(FALSE)
{
}	

RCastLUTs::RCastLUTs( YHueAngle hueAngle, YFloatType effectFraction, BOOL bCoarseFlag )
	: m_hueAngle( hueAngle ),
	  m_effectFraction( effectFraction ),
	  m_bCoarseFlag( bCoarseFlag ),
	  m_bInitialized( FALSE )
{
	BuildLUTs();
}	

RCastLUTs::~RCastLUTs()
{
}	

const RCastLUTs& RCastLUTs::operator=( RCastLUTs& src )
{
	m_hueAngle       = src.m_hueAngle      ;
	m_effectFraction = src.m_effectFraction;
	m_bCoarseFlag    = src.m_bCoarseFlag   ;
	m_bInitialized   = src.m_bInitialized  ;
	if ( m_bInitialized )
	{
		memcpy( m_redLUT,   src.m_redLUT,   kLUTSize );
		memcpy( m_greenLUT, src.m_greenLUT, kLUTSize );
		memcpy( m_blueLUT,  src.m_blueLUT,  kLUTSize );
	}
	return *this;
}	

BOOL RCastLUTs::BuildLUTs()
{
	m_bInitialized = FALSE;

	YRealRGB componentProportions;
	YFloatType *pMaxLUT = NULL;
	
	SetComponentProportions ( &componentProportions);

	// get the appropriate LUT for the coarse/fine mode
	if ( m_bCoarseFlag )
	{
		// manufacture Dramatic Max LUT
		pMaxLUT = new YFloatType[256];
		if ( pMaxLUT )
		{
			for (int i = 0; i < 256; i++)
			{
				pMaxLUT[i] = MAX_DRAMATIC_BRIGHTCAST_VAL;
			}
			m_bInitialized = TRUE;
		}
	}
	else
	{	// use the corrective generic LUT
		pMaxLUT = MaxGenericOneHumpLUT;
		m_bInitialized = TRUE;
	}
	
	if ( m_bInitialized )
	{
		CastComponentLUTs (	componentProportions, pMaxLUT );
			
		if ( m_bCoarseFlag )
		{
			delete pMaxLUT;
		}
	}
		
	return m_bInitialized;

#if 0
	m_bInitialized = FALSE;

	RealRGB componentProportions;
	YFloatType *pMaxLUT = NULL;
	
	SetComponentProportions ( &componentProportions);

	// get the appropriate LUT for the coarse/fine mode
	if ( m_bCoarseFlag )
	{
		// manufacture Dramatic Max LUT
		pMaxLUT = new YFloatType[256];
		if (! pMaxLUT)
		{
			return FALSE;
		}
		for (int i = 0; i < 256; i++)
		{
			pMaxLUT[i] = MAX_DRAMATIC_BRIGHTCAST_VAL;
		}
	}
	else
	{	// use the corrective generic LUT
		pMaxLUT = MaxGenericOneHumpLUT;
	}
	
	CastComponentLUTs (	componentProportions, pMaxLUT );
			
	if ( m_bCoarseFlag )
	{
		delete pMaxLUT;
	}
		
	m_bInitialized = TRUE;
	return TRUE;
#endif
}

BOOL RCastLUTs::GetLUTs( YLUTComponent **pRedLUT, YLUTComponent **pGreenLUT, YLUTComponent **pBlueLUT )
{
	if ( m_bInitialized )
	{
		*pRedLUT   = m_redLUT;
		*pGreenLUT = m_greenLUT;
		*pBlueLUT  = m_blueLUT;
	}

	return m_bInitialized;
}

///////////////////////////////////////////////////////////////////
// Set normalized component proportions for cast of any hue angle,
// preserving brightness 
// (redproportion+greenproportion+blueproportion==0)
// If red == 1.0, then blue and green are -.5 each.
// Works in between primary hues as well
///////////////////////////////////////////////////////////////////
void RCastLUTs::SetComponentProportions ( YRealRGB *pComponentProportions )
{
	pComponentProportions->red   = cos(DegreesToRadians(m_hueAngle));
	pComponentProportions->green = cos(DegreesToRadians(m_hueAngle-GRNANGLE));
	pComponentProportions->blue  = cos(DegreesToRadians(m_hueAngle-BLUANGLE));
}	

////////////////////////////////////////////////////////////////////
// Build Component LUTs for cast.
// Assumes that the image manipulation library accepts 256-element 
// LUTS containing values equal to the indices (0to255) for neutral
// transfer.
// Alternately it is possible that the image manipulation library 
// expects offsets from the neutral respones value.
// The maxLUT is defined as float offsets in the range 0to1.
///////////////////////////////////////////////////////////////////
void RCastLUTs::CastComponentLUTs( YRealRGB hueProportions, YFloatType *maxLUT )
{
	YFloatType LUVal, LUValOffset, fractionOfProportion;
	for (int i = 0; i < 256; i++)
	{
		// for RED
					// neutral response value
		LUVal = i / 255.;		

					// fraction of max component proportion
		fractionOfProportion = hueProportions.red * m_effectFraction;

					// interpolated LUValOffset
		LUValOffset = maxLUT[i] * fractionOfProportion;

					// add offset to neutral response value
		LUVal += LUValOffset;

					// clip to white, black
		if (LUVal > 1.) LUVal = 1.0;
		if (LUVal < 0.) LUVal = 0.;
		
					// stuff into LUT
		m_redLUT[i] = RoundToInt(LUVal*255);

		// for GRN
		LUVal = i / 255.;		
		fractionOfProportion = hueProportions.green * m_effectFraction;
		LUValOffset = maxLUT[i] * fractionOfProportion;
		LUVal += LUValOffset;
		if (LUVal > 1.) LUVal = 1.0;
		if (LUVal < 0.) LUVal = 0.;
		m_greenLUT[i] = RoundToInt(LUVal*255);

		// for BLU
		LUVal = i / 255.;		
		fractionOfProportion = hueProportions.blue * m_effectFraction;
		LUValOffset = maxLUT[i] * fractionOfProportion;
		LUVal += LUValOffset;
		if (LUVal > 1.) LUVal = 1.0;
		if (LUVal < 0.) LUVal = 0.;
		m_blueLUT[i] = RoundToInt(LUVal*255);			
	}		
}

/////////////////////////////////////////////////////////////////////////////////////
// RoundToInt ()
// 
/////////////////////////////////////////////////////////////////////////////////////
YHueAngle RCastLUTs::RoundToInt (YFloatType fltval)
{
	if (fltval < 0.0)
		fltval -= .5;
	else
		fltval += .5;
	return ((YHueAngle)fltval);	
}

/////////////////////////////////////////////////////////////////////////////////////
// DegreesToRadians ()
// unit conversion
/////////////////////////////////////////////////////////////////////////////////////
YFloatType RCastLUTs::DegreesToRadians (YHueAngle angle)
{
	return ((YFloatType)angle * 2.0 * 3.14159 / 360.);
}


#ifdef TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RCastLUTs::Validate( )
//
//  Description:	Validates the object
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCastLUTs::Validate( ) const
{
	RObject::Validate( );
	TpsAssertIsObject( RCastLUTs, this );
}

#endif	//	TPSDEBUG

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define ADD_TONE
#ifdef ADD_TONE

RToneLUTs::RToneLUTs()
	: m_brightnessFraction(0.0),
	  m_contrastFraction(0.0),
	  m_bCoarseFlag(FALSE),
	  m_bInitialized(FALSE)
{
		BuildLUTs();
}	

RToneLUTs::RToneLUTs( YFloatType brightnessFraction, YFloatType contrastFraction, BOOL bCoarseFlag )
	: m_brightnessFraction( brightnessFraction),
	  m_contrastFraction( contrastFraction ),
	  m_bCoarseFlag( bCoarseFlag ),
	  m_bInitialized( FALSE )
{
	BuildLUTs();
}	

RToneLUTs::~RToneLUTs()
{
}	

const RToneLUTs& RToneLUTs::operator=( RToneLUTs& src )
{
	m_brightnessFraction = src.m_brightnessFraction      ;
	m_contrastFraction = src.m_contrastFraction;
	m_bCoarseFlag    = src.m_bCoarseFlag   ;
	m_bInitialized   = src.m_bInitialized  ;
	if ( m_bInitialized )
	{
		memcpy( m_redLUT,   src.m_redLUT,   kLUTSize );
		memcpy( m_greenLUT, src.m_greenLUT, kLUTSize );
		memcpy( m_blueLUT,  src.m_blueLUT,  kLUTSize );
	}
	return *this;
}

// ****************************************************************************
//
//  Function Name:	RToneLUTs::BuildLUTs()
//
//  Description:	Builds 3 identical component color LUTs
//
//  Returns:		Success flag
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RToneLUTs::BuildLUTs()
{
	m_bInitialized = FALSE;

	YFloatType brightnessPlusContrast;
	YFloatType scalingFactor;
	YFloatType LUVal, LUValOffset;
	YFloatType *pMaxBrightLUT = NULL;
	YFloatType *pMaxContrastLUT = NULL;

// if coarse mode, don't bother to prevent overflow
	if (! m_bCoarseFlag)
	{
		// check the point in the response curve most likely to overflow
		// and scale back fractions accordingly
		if ((m_brightnessFraction > 0.0) && (m_contrastFraction > 0.0))
		{	
			LUVal = LIKELY_OVERFLO_INDEX / 255.;
			brightnessPlusContrast = 
						m_brightnessFraction*MaxGenericOneHumpLUT[LIKELY_OVERFLO_INDEX]+ 
						m_contrastFraction*MaxGenericOgeeLUT[LIKELY_OVERFLO_INDEX];
			if (brightnessPlusContrast > (.99 - LUVal))
			{	
				scalingFactor = (.99 - LUVal)/ brightnessPlusContrast;
				m_brightnessFraction *= scalingFactor;
				m_contrastFraction *= scalingFactor;
			}
		}

		// check point in the response curve most likely to underflow
		// and scale back fractions accordingly
		if ((m_brightnessFraction < 0.0) && (m_contrastFraction > 0.0))
		{	
			LUVal = LIKELY_UNDRFLO_INDEX / 255.;
			brightnessPlusContrast = 	
						m_brightnessFraction*MaxGenericOneHumpLUT[LIKELY_UNDRFLO_INDEX]+ 
						m_contrastFraction*MaxGenericOgeeLUT[LIKELY_UNDRFLO_INDEX];
			if ((-brightnessPlusContrast) > (LUVal - .01))
			{
				scalingFactor = (LUVal - .01) / -brightnessPlusContrast;
				m_brightnessFraction *= scalingFactor;
				m_contrastFraction *= scalingFactor;
			}
		}	
		
		// check overflattening with negative contrast
		if (m_contrastFraction < 0.0)
			if (	(m_contrastFraction * m_brightnessFraction < -.8) ||
					(m_contrastFraction * m_brightnessFraction > .8))
			{
				m_contrastFraction *= .9;
				m_brightnessFraction *= .9;
			}
	}
////////
	// get the appropriate LUTs for the coarse/fine mode
	if (m_bCoarseFlag)
	{
		if (m_brightnessFraction != 0.0)
		{
			pMaxBrightLUT = new YFloatType [256];
			if (! pMaxBrightLUT)
			{
				//fprintf (PFLog, "MaxBrightLUT failed\n");
				return (m_bInitialized);
			}
			// now build max brightness LUT
			for (int i = 0; i < 256; i++)
				pMaxBrightLUT[i] = MAX_DRAMATIC_BRIGHTCAST_VAL;
		}
		else
			pMaxBrightLUT = MaxGenericOneHumpLUT; // doesn't matter what points to
			
		if (m_contrastFraction != 0.0)
		{
			pMaxContrastLUT = new YFloatType [256];
			if (! pMaxContrastLUT)
			{
				//fprintf (PFLog, "MaxContrastLUT failed\n");
				return (m_bInitialized);
			}
			// now build max contrast LUT	
			float start, end, range;
			if (m_contrastFraction > 0.0)
			{
				start = MAX_POS_CONTRAST_START;
				end = MAX_POS_CONTRAST_END;
			}
			else	// less than 0.0
			{
				start = MAX_NEG_CONTRAST_START;
				end = MAX_NEG_CONTRAST_END;
			}
			range = end - start;
			for (int i = 0; i < 256; i++)
			{
				pMaxContrastLUT[i] = start + range * i/255.;
			}
		}
		else
			pMaxContrastLUT = MaxGenericOgeeLUT; // doesn't matter what points to
	}
	else
	{
		pMaxBrightLUT = MaxGenericOneHumpLUT;
		pMaxContrastLUT = MaxGenericOgeeLUT;
	}

	m_bInitialized = TRUE;

	//populate tone LUT
	for (int i = 0; i < 256; i++)
	{
					// neutral response value
		LUVal = i / 255.;
					// combined interpolated effects
		LUValOffset = 
			m_brightnessFraction * pMaxBrightLUT[i] +
			m_contrastFraction * pMaxContrastLUT[i];
					// add offset to neutral response value
		LUVal += LUValOffset;
					// clip to white, black, just in case
		if (LUVal > 1.) LUVal = 1.0;
		if (LUVal < 0.) LUVal = 0.0;
		m_greenLUT[i] = RoundToInt(LUVal * 255);
	}
	
	if (m_bCoarseFlag)
	{
		if (m_brightnessFraction != 0.0)
			delete pMaxBrightLUT;
		if (m_contrastFraction != 0.0)
			delete pMaxContrastLUT;
	}
					// copy green to the other two component LUTs
	memcpy (m_redLUT, m_greenLUT, kLUTSize);
	memcpy (m_blueLUT, m_greenLUT, kLUTSize);

	return m_bInitialized;
}

BOOL RToneLUTs::GetLUTs( YLUTComponent **pRedLUT, YLUTComponent **pGreenLUT, YLUTComponent **pBlueLUT )
{
	if ( m_bInitialized )
	{
		*pRedLUT   = m_redLUT;
		*pGreenLUT = m_greenLUT;
		*pBlueLUT  = m_blueLUT;
	}

	return m_bInitialized;
}

/////////////////////////////////////////////////////////////////////////////////////
// RoundToInt ()
// 
/////////////////////////////////////////////////////////////////////////////////////
YHueAngle RToneLUTs::RoundToInt (YFloatType fltval)
{
	if (fltval < 0.0)
		fltval -= .5;
	else
		fltval += .5;
	return ((YHueAngle)fltval);	
}

#ifdef TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RToneLUTs::Validate( )
//
//  Description:	Validates the object
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RToneLUTs::Validate( ) const
{
	RObject::Validate( );
	TpsAssertIsObject( RToneLUTs, this );
}

#endif	//	TPSDEBUG
#endif /*ADD_TONE*/
