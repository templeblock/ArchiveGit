/*=================================================================
 
	File:		scset.h

	$Header: /Projects/Toolbox/ct/SCSET.H 2 	5/30/97 8:45a Wmanis $

	Contains:	Set definition.

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

@doc

=================================================================*/



#ifndef _H_SCBAG
#define _H_SCBAG

#ifdef SCMACINTOSH
#pragma once
#endif

#include "scmemarr.h"


/* ==================================================================== */
/* ==================================================================== */
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
	
	Bool		Includes( const scObject* obj ) const
					{
						return Index( obj ) >= 0;
					}

	long		Add( const scObject* );
	void		Remove( const scObject* );
	long		Index( const scObject* ) const;
	void		Set( long index, const scObject* );

					// delete all the objects in the set and set
					// the set to zero elements
	void		DeleteAll( void );
};




#endif

