/*=================================================================
 
	File:		CBAG.C

	$Header: /Projects/Toolbox/ct/SCSET.CPP 2	  5/30/97 8:45a Wmanis $

	Contains:	Implementation of a set of objects.

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.


=================================================================*/

#include "scset.h"


/* ==================================================================== */

scSet::scSet( const scSet& set ) :
				scMemArray( sizeof( void* ), true )
{
	scAssert( fNumItems == 0 );
	AppendData( set.GetMem(), set.GetNumItems() );
	scAssert( fNumItems == set.GetNumItems() );
}
			
/* ==================================================================== */
						  
long scSet::Index( const scObject* ptr ) const
{
	long				i;
	const scObject**	arr = (const scObject**)fItems;

	for ( i = 0; i < fNumItems; i++ )
		if ( *arr++ == ptr )
			return i;
	
	return -1;
}

/* ==================================================================== */

long	scSet::Add( const scObject* obj )
{
	long index = Index( obj );

	if ( index >= 0 )
		return index;

	AppendData( (ElementPtr)&obj, 1 );
	return GetNumItems() - 1;
}

/* ==================================================================== */

void scSet::Remove( const scObject* obj )
{
	long		index = Index( obj );
	scObject*	ptr;
	
	if ( index >= 0 )
		*( ( (scObject **)fItems) + index ) = 0;
	
	while ( fNumItems > 0 && ( ptr = Get( fNumItems - 1 ) ) == 0 )
		fNumItems--;
		
	ShrinkSlots();
}

/* ==================================================================== */

void scSet::Set( long index, const scObject* obj )
{
	if ( fElemSlots <= index )
		SetNumSlots( ( ( index / fBlockSize ) + 1 ) * fBlockSize );

	fNumItems = MAX( index + 1, fNumItems );
	
	scObject**	arr = (scObject**)fItems;
	
	scAssert( arr[index] == 0 );

	arr[index] = (scObject*)obj;
}

/* ==================================================================== */

void scSet::DeleteAll( void )
{
	int 		i;
	scObject**	arr = (scObject**)fItems;
	scObject*	obj;
	
	for ( i = 0; i < fNumItems; i++ ) {
		obj 	= *arr;
		*arr++	= 0;

		delete obj;
	}
	RemoveAll();
}

/* ==================================================================== */
