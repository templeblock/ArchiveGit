/*=================================================================
Contains:	Set definition.
=================================================================*/

#pragma once

#include "scmemarr.h"

// @class scSet contains an array of 32 bit pointers. Insertion appends
// the pointer and deletion zeros out the slot. This way we may
// insure that all inserted pointers will get a unique index and a valid
// pointer will never get the index zero.
class scSet : public scMemArray {
public:
	scSet() :
		scMemArray( sizeof( scObject* ), true ){}
	
	scSet( const scSet& );

	scObject*	Get( long offset ) const
	{
		scAssert( offset < fNumItems );
		return ((scObject**)fItems)[offset];
	}
	
	BOOL		Includes( const scObject* obj ) const
	{
		return Index( obj ) >= 0;
	}

	long		Add( const scObject* );
	void		Remove( const scObject* );
	long		Index( const scObject* ) const;
	void		Set( long index, const scObject* );

	// delete all the objects in the set and zero the elements
	void		DeleteAll( void );
};
