//****************************************************************************
//
// File Name:   WHPTriple.cpp
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Implementation of the RWHPTriple object
//
// Portability: Platform independent
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/WHPTriple.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "WHPTriple.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Define primarys
const double kRedPrimary   = 0.0 ;
const double kGreenPrimary = 1.0 ;
const double kBluePrimary  = 2.0 ;

// ****************************************************************************
//
//  Function Name:	RWHPTriple::RWHPTriple( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RWHPTriple::RWHPTriple(	double flWhite, double flHue, double flPurity )
{
	// Initialize compentents
	m_flWhite  = flWhite ;
	m_flHue    = flHue ; 
	m_flPurity = flPurity ;
}

// ****************************************************************************
//
//  Function Name:	RWHPTriple::RWHPTriple( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RWHPTriple::RWHPTriple( const RWHPTriple& color )
{
	m_flWhite  = color.m_flWhite ;
	m_flHue    = color.m_flHue ; 
	m_flPurity = color.m_flPurity ;
}

// ****************************************************************************
//
//  Function Name:	RWHPTriple::RWHPTriple( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RWHPTriple::RWHPTriple( const RSolidColor& color  )
{
	// Initialize compentents
	m_flWhite  = m_flHue = m_flPurity = 0.0 ;

	// Convert from RGB to WHP 
	operator=( color ) ;
}

// ****************************************************************************
//
//  Function Name:	RWHPTriple::operator( )
//
//  Description:		Conversion operator
//
//  Returns:			A Renaissance RSolidColor object
//
//  Exceptions:		None
//
// ****************************************************************************
RWHPTriple::operator RSolidColor() const
{
	double flBelowHuePrimary, flHueFrac ;
	double flBelowHueWt, flAboveHueWt ;
	double flMaxWt, flScale ;

	// Get component ration from hue
	flBelowHuePrimary = (short) m_flHue ;
	flHueFrac = m_flHue - flBelowHuePrimary ;

	if (AreFloatsEqual( flHueFrac, 0.0 ))
	{
		flBelowHueWt = 1.0 ;
		flAboveHueWt = 0.0 ;
	}
	else if (flHueFrac > 0.5)
	{
		flAboveHueWt = 1.0 ;	// Simplifying assumption
		flBelowHueWt = (1.0 - flHueFrac) / 0.5 ;
	}
	else 
	{
		flBelowHueWt = 1.0 ;	// Simplifying assumption
		flAboveHueWt = flHueFrac * flBelowHueWt / 0.5 ;
	}

	// Scale to purity
	flMaxWt = flBelowHueWt ;

	if (flAboveHueWt > flMaxWt)
	{
		flMaxWt = flAboveHueWt ;
	}

	flScale = m_flPurity / flMaxWt ;
	flBelowHueWt *= flScale ;
	flAboveHueWt *= flScale ;

	// Assign to components
	double flRed(0.0), flGreen(0.0), flBlue(0.0) ;

	if (AreFloatsEqual( flBelowHuePrimary, kRedPrimary ))
	{
		flRed   = flBelowHueWt ;
		flGreen = flAboveHueWt ;
	}
	
	if (AreFloatsEqual( flBelowHuePrimary, kGreenPrimary )) 
	{
		flGreen = flBelowHueWt ;
		flBlue  = flAboveHueWt ;
	}

	if (AreFloatsEqual( flBelowHuePrimary, kBluePrimary ))
	{
		flBlue = flBelowHueWt ;
		flRed  = flAboveHueWt ;
	}

	// Add back white fraction
	flRed   += m_flWhite; TpsAssert( 0.0 <= flRed   && flRed   <= 1.001, "Invalid Component!" ) ;
	flGreen += m_flWhite; TpsAssert( 0.0 <= flGreen && flGreen <= 1.001, "Invalid Component!" ) ;
	flBlue  += m_flWhite; TpsAssert( 0.0 <= flBlue  && flBlue  <= 1.001, "Invalid Component!" ) ;

	RSolidColor crResult(
		YColorComponent( flRed   * 255.0 + 0.5 ), 
		YColorComponent( flGreen * 255.0 + 0.5 ), 
		YColorComponent( flBlue  * 255.0 + 0.5 ) ) ;

	return crResult ;
}

// ****************************************************************************
//
//  Function Name:	RWHPTriple::operator=( )
//
//  Description:		Assignment operator
//
//  Returns:			Reference to this color
//
//  Exceptions:		None
//
// ****************************************************************************
const RWHPTriple& RWHPTriple::operator=( const RWHPTriple& rhs )
{
	m_flWhite  = rhs.m_flWhite ;
	m_flHue    = rhs.m_flHue ; 
	m_flPurity = rhs.m_flPurity ;

	return *this ;
}

// ****************************************************************************
//
//  Function Name:	RWHPTriple::operator=( )
//
//  Description:		Assignment operator
//
//  Returns:			Reference to this color
//
//  Exceptions:		None
//
// ****************************************************************************
const RWHPTriple& RWHPTriple::operator=( const RSolidColor& color ) 
{
	double flRed   = color.GetRed()   / 255.0 ;
	double flGreen = color.GetGreen() / 255.0 ;
	double flBlue  = color.GetBlue()  / 255.0 ;

	// Get the white fraction, the least component ;
	m_flWhite = min( flRed, min( flGreen, flBlue ) ) ;

	// Remove the white fraction from the components
	flRed   -= m_flWhite ;
	flGreen -= m_flWhite ;
	flBlue  -= m_flWhite ;

	// Get purity fraction, the largest of the components
	m_flPurity = max( flRed, max( flGreen, flBlue ) ) ;


	// Get Hue
	//////////

	short nCmps = 0 ;

	if (flRed > 0)
		nCmps++ ;

	if (flGreen > 0)
		nCmps++ ;

	if (flBlue)
		nCmps++ ;

	if (nCmps == 1) // Simple case, one-component hue
	{
		if (flRed > 0)
			m_flHue = kRedPrimary ;
		if (flGreen > 0)
			m_flHue = kGreenPrimary ;
		if (flBlue)
			m_flHue = kBluePrimary ;
	}
	else
	{
		if (AreFloatsEqual( flBlue, 0.0 ))
			m_flHue = kRedPrimary + HueBetween( flRed, flGreen ) ;
		if (AreFloatsEqual( flGreen, 0.0 ))
			m_flHue = kBluePrimary + HueBetween( flBlue, flRed ) ;
		if (AreFloatsEqual( flRed, 0.0 ))
			m_flHue = kGreenPrimary + HueBetween( flGreen, flBlue ) ;
	}

	return *this ;
}

// ****************************************************************************
//
//  Function Name:	RWHPTriple::operator+( )
//
//  Description:		Addition operator
//
//  Returns:			Result of the color addition
//
//  Exceptions:		None
//
// ****************************************************************************
RWHPTriple RWHPTriple::operator+( const RWHPTriple& rhs ) const
{
	double flWhite  = m_flWhite ;
	double flHue    = m_flHue ;
	double flPurity = m_flPurity ;

	//
	// White
	//
	if (rhs.m_flWhite > 0.0)
	{
		flWhite = m_flWhite + (1.0 - m_flWhite) * rhs.m_flWhite ;
	}
	else if (rhs.m_flWhite < 0.0)
	{
		flWhite = m_flWhite + m_flWhite * rhs.m_flWhite ;
	}

	// Kill whiteness for a black in model gradient
	if ((rhs.m_flPurity < 0.0) && !(rhs.m_flWhite > 0))
	{
		flWhite += flWhite * rhs.m_flPurity ;
	}

	//
	// Hue
	//
	flHue += rhs.m_flHue ;

	if (flHue >= 3.0)
	{
		flHue -= 3.0 ;
	}
	if (flHue < 0.0)
	{
		flHue += 3.0 ;
	}

	//
	// Purity
	//
	if (rhs.m_flPurity > 0.0)
	{
		flPurity += (1.0 - m_flPurity) * rhs.m_flPurity ;
	}
	else if (rhs.m_flPurity < 0.0)
	{
		flPurity += m_flPurity * rhs.m_flPurity ;
	}

	TpsAssert( 0.0 <= m_flWhite  && m_flWhite  <= 1.0, "Invalid whiteness component!") ;
	TpsAssert( 0.0 <= m_flHue    && m_flHue    <= 3.0, "Invalid hue component!") ;
	TpsAssert( 0.0 <= m_flPurity && m_flPurity <= 1.0, "Invalid purity component!") ;

	return RWHPTriple( flWhite, flHue, flPurity ) ;
}

double RWHPTriple::HueBetween( double flBelowWt, double flAboveWt )
{
	if (AreFloatsEqual( flAboveWt, 0.0 ) && AreFloatsEqual( flBelowWt, 0.0 ))
	{
		return (0.0) ;
	}

	if (flAboveWt > flBelowWt)
	{
		return (1.0 - 0.5 * flBelowWt / flAboveWt) ;
	}

	return (0.5 * flAboveWt / flBelowWt ) ;
}

