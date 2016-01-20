/***************************************************************************
Contains:	size of bezier sub-division factors
****************************************************************************/

#ifndef _H_SCBEZIER
#define _H_SCBEZIER

#include "sctypes.h"

void BEZVectorizePoly( scVertex *&, const scVertex * );

// last one of these defined determines the number of vectors
// that will be created by sub-dividing the bezier curver
#define SubDiv2 		2
#define SubDiv4 		(SubDiv2 * 2)
#define SubDiv8 		(SubDiv4 * 2)
#define SubDiv16		(SubDiv8 * 2)

#define scBezFactor( x, y ) y
struct scBezBlendValue {
	REAL	ca;
	REAL	cb;
	REAL	cc;
	REAL	cd;
};

#if defined( SubDiv256 )
	#define scBezBlendSize	  (SubDiv256 + 1)
#elif defined( SubDiv128 )
	#define scBezBlendSize	  (SubDiv128 + 1)
#elif defined( SubDiv64 )
	#define scBezBlendSize	  (SubDiv64 + 1)
#elif defined( SubDiv32 )
	#define scBezBlendSize	  (SubDiv32 + 1)
#elif defined( SubDiv16 )
	#define scBezBlendSize	  (SubDiv16 + 1)
#elif defined( SubDiv8 )
	#define scBezBlendSize	  (SubDiv8 + 1)
#elif defined( SubDiv4 )
	#define scBezBlendSize	  (SubDiv4 + 1)
#elif defined( SubDiv2 )
	#define scBezBlendSize	  (SubDiv2 + 1)
#elif !defined( scBezBlendSize )
	#define scBezBlendSize	  0
#endif

#endif /* _H_SCBEZIER */
