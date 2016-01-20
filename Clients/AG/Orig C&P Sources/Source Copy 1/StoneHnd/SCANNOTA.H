/*=================================================================
 
	File:		scannota.h

	$Header: /Projects/Toolbox/ct/SCANNOTA.H 2     5/30/97 8:45a Wmanis $

	Contains:	Generalized annotation structure.

	Written by:	Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

=================================================================*/

#ifndef _H_SCANNOTA
#define _H_SCANNOTA

#include "sctypes.h"
#include <string.h>

class scAnnotation {

public:
				scAnnotation( UCS2*, int, int, int );
				scAnnotation();
				
	void		Set( UCS2 *, int, int, int );
	void		Clear( void );

	UCS2		fCharStr[32];	
	Bool		fAnnotate;			// if true apply, else clear
	
	int			fParaOffset;
	int			fStartOffset;
	int			fEndOffset;
};

/* ==================================================================== */

inline void scAnnotation::Clear()
{
	fAnnotate		= false;
	fParaOffset		= -1;
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

/* ==================================================================== */

#endif
