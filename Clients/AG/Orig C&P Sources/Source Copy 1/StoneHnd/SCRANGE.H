/*=================================================================
 
	File:		crange.h

	$Header: /Projects/Toolbox/ct/SCRANGE.H 2     5/30/97 8:45a Wmanis $

	Contains:	Tests ranges.

	Written by:	Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.


=================================================================*/

#ifndef _H_SCRANGE
#define _H_SCRANGE

#include "sctypes.h"

typedef enum eRangeSects {
	eExclusive,
	eInclusive,
	eStartInclusive,
	eEndInclusive
} eRangeSect;

class scRange {
public:
			scRange( long start, long end ) :
				fStart( start ),
				fEnd( end ) {}
					
	Bool	Exclusive_Sect( const scRange& range ) const
				{	return fStart < range.fEnd && fEnd > range.fStart; }
	
	Bool	Inclusive_Sect( const scRange& range ) const
				{ return fStart <= range.fEnd && fEnd >= range.fStart; }
	
	Bool	InclusiveStart_Sect( const scRange& range ) const
				{ return fStart <= range.fEnd && fEnd > range.fStart; }
	
	Bool	InclusiveEnd_Sect( const scRange& range ) const
				{ return fStart < range.fEnd && fEnd >= range.fStart; }

	Bool	Inclusive( long val ) const
				{ 	return val >= fStart && val <= fEnd; }
	
	Bool	Exclusive( long val ) const
				{ 	return val > fStart && val < fEnd; }
				
	Bool	StartInclusive( long val ) const
				{ 	return val >= fStart && val < fEnd; }
	
	Bool	EndInclusive( long val ) const
				{ 	return val > fStart && val <= fEnd; }
				
	Bool	Sect( const scRange& range, eRangeSect eSect )
				{
					if ( eSect == eExclusive )
						return Exclusive_Sect( range );
					else if ( eSect == eStartInclusive )
						return InclusiveStart_Sect( range );
					else if ( eSect == eEndInclusive )
						return InclusiveEnd_Sect( range );
					else 
						return Inclusive_Sect( range );
				}
	
private:
	const long		fStart;
	const long		fEnd;
};

#endif /* _H_SCRANGE */
