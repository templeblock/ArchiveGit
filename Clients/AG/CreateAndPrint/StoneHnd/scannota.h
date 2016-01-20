/*=================================================================
Contains:	Generalized annotation structure.
=================================================================*/

#pragma once

#include "sctypes.h"

class scAnnotation
{
public:
				scAnnotation( UCS2*, int, int, int );
				scAnnotation();
				
	void		Set( UCS2 *, int, int, int );
	void		Clear( void );

	UCS2		fCharStr[32];	
	BOOL		fAnnotate;			// if true apply, else clear
	
	int 		fParaOffset;
	int 		fStartOffset;
	int 		fEndOffset;
};

/* ==================================================================== */
inline void scAnnotation::Clear()
{
	fAnnotate		= false;
	fParaOffset 	= -1;
	fStartOffset	= -1;
	fEndOffset		= -1;
}

/* ==================================================================== */
inline scAnnotation::scAnnotation( UCS2 *ch, int paraoffset, int start, int end )
{
	Set( ch, paraoffset, start, end );
}

/* ==================================================================== */
inline scAnnotation::scAnnotation()
{
	Clear();
}
