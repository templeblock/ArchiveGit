/***************************************************************************

	File:		SCBEZIER.H


	$Header: /Projects/Toolbox/ct/SCBEZIER.H 2     5/30/97 8:44a Wmanis $

	Contains:	size of bezier sub-division factors

	Written by:	Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
****************************************************************************/

#ifndef _H_SCBEZIER
#define _H_SCBEZIER

#include "sctypes.h"

void			BEZVectorizePoly( scVertex *&, const scVertex * );


/* last one of these defined determines the number of vectors
 * that will be created by sub-dividing the bezier curver
 */
#define SubDiv2			2
#define SubDiv4			(SubDiv2 * 2)
#define SubDiv8			(SubDiv4 * 2)
#define SubDiv16		(SubDiv8 * 2)

#if 0
#define SubDiv32		(SubDiv16 * 2)
#define SubDiv64		(SubDiv32 * 2)
#define SubDiv128		(SubDiv64 * 2)
#define SubDiv256		(SubDiv128 * 2)
#endif


//#define scBezFixed
#define scBezREAL

#ifdef scBezFixed
	#define scBezFactor( x, y )	x
	struct scBezBlendValue {
		ushort	ca;
		ushort	cb;
		ushort	cc;
		ushort	cd;
	};
	#ifdef scBezREAL
		#error "can't define both"
	#endif
#endif

#ifdef scBezREAL	
	#define scBezFactor( x, y )	y
	struct scBezBlendValue {
		REAL	ca;
		REAL	cb;
		REAL	cc;
		REAL	cd;
	};
	#ifdef scBezFixed
		#error "can't define both"
	#endif
#endif

#if defined( SubDiv256 )
#define scBezBlendSize    (SubDiv256 + 1)
#elif defined( SubDiv128 )
#define scBezBlendSize    (SubDiv128 + 1)
#elif defined( SubDiv64 )
#define scBezBlendSize    (SubDiv64 + 1)
#elif defined( SubDiv32 )
#define scBezBlendSize    (SubDiv32 + 1)
#elif defined( SubDiv16 )
#define scBezBlendSize    (SubDiv16 + 1)
#elif defined( SubDiv8 )
#define scBezBlendSize    (SubDiv8 + 1)
#elif defined( SubDiv4 )
#define scBezBlendSize    (SubDiv4 + 1)
#elif defined( SubDiv2 )
#define scBezBlendSize    (SubDiv2 + 1)
#elif !defined( scBezBlendSize )
#define scBezBlendSize    0
#endif

#endif /* _H_SCBEZIER */
