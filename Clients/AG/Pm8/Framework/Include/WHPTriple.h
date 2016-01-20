//****************************************************************************
//
// File Name:   WHPTriple.h
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Definition of the RWHPTriple object
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
//  $Logfile:: /PM8/Framework/Include/WHPTriple.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _WHPTRIPLE_H_
#define _WHPTRIPLE_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RWHPTriple
{
	double m_flWhite ;
	double m_flHue ;
	double m_flPurity ;

public:

							RWHPTriple( 
								double flWhite  = 0.0, 
								double flHue    = 0.0, 
								double flPurity = 0.0 ) ;

							RWHPTriple( const RWHPTriple& color ) ;
							RWHPTriple( const RSolidColor& color ) ;

							operator RSolidColor() const;
	const RWHPTriple&	operator=( const RSolidColor& color ) ;
	const RWHPTriple& operator=( const RWHPTriple& rhs ) ;

	RWHPTriple			operator+( const RWHPTriple& rhs ) const ;

protected:

	double				HueBetween( double flBelowWt, double flAboveWt ) ;

} ;

struct RColorTemplateEntry
{
	RWHPTriple	m_whpColor ;
	YPercentage	m_pctMidPoint ;
	YPercentage	m_pctRampPoint ;

	RColorTemplateEntry( const RWHPTriple& whpColor, YPercentage pctMidPoint, YPercentage pctRampPoint ) :
		m_whpColor( whpColor ), m_pctMidPoint( pctMidPoint ), m_pctRampPoint( pctRampPoint ) {}
} ;

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _WHPTRIPLE_H_