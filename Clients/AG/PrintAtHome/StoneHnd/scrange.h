/*=================================================================
Contains:	Tests ranges.
=================================================================*/

#pragma once

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
					
	BOOL	Exclusive_Sect( const scRange& range ) const
				{	return fStart < range.fEnd && fEnd > range.fStart; }
	
	BOOL	Inclusive_Sect( const scRange& range ) const
				{ return fStart <= range.fEnd && fEnd >= range.fStart; }
	
	BOOL	InclusiveStart_Sect( const scRange& range ) const
				{ return fStart <= range.fEnd && fEnd > range.fStart; }
	
	BOOL	InclusiveEnd_Sect( const scRange& range ) const
				{ return fStart < range.fEnd && fEnd >= range.fStart; }

	BOOL	Inclusive( long val ) const
				{	return val >= fStart && val <= fEnd; }
	
	BOOL	Exclusive( long val ) const
				{	return val > fStart && val < fEnd; }
				
	BOOL	StartInclusive( long val ) const
				{	return val >= fStart && val < fEnd; }
	
	BOOL	EndInclusive( long val ) const
				{	return val > fStart && val <= fEnd; }
				
	BOOL	Sect( const scRange& range, eRangeSect eSect )
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
