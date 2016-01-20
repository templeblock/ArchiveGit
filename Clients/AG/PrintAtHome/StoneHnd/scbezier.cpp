/****************************************************************************
	Contains:		vectorizes beziers
****************************************************************************/

#include "scbezier.h"
#include "scmem.h"
#include <limits.h>

struct SCBezVertex {
	short	x;
	short	y;
};

/* ==================================================================== */
extern scBezBlendValue bezblend[];

static void BezCompute( scVertex*		dstV,
						const scVertex* srcV );


/* count the number of vertices in the vertex list */
inline long CountVerts( const scVertex* verts )
{
	long	numVerts;
   
	for ( numVerts = 1; verts->fPointType != eFinalPoint; verts++, numVerts++ )
		;
	return numVerts;
}

/* count the number of bezier curves in a vertex list */
inline long CountBezCurves( const scVertex* verts )
{
	long	numCurves;

	for ( numCurves = 0; verts->fPointType != eFinalPoint; ) {
		if ( verts->fPointType == eBezControlPoint && (verts+1)->fPointType == eBezControlPoint ) {
			numCurves++;
			verts += 2;
		}
		else
			verts++;
	}
	return numCurves;
}

/* process the list vectorizing the beziers into straight lines */
void BEZVectorizePoly( scVertex*&		dstV,
					   const scVertex*	srcV )
{
	scVertex*	vList;
	int 		i;
	BOOL		process;
	long		numPoints;
	long		numCurves;
	scVertex	bezVectors[ scBezBlendSize + 2 ];

	numPoints = CountVerts( srcV );
	numCurves = CountBezCurves( srcV );

	if ( numCurves == 0 )
		return;

	long	 segments = numPoints + scBezBlendSize * numCurves;
	dstV = vList = (scVertex*)MEMAllocPtr( segments * sizeof( scVertex ) );

	for ( process = true; process; ) {

		switch ( srcV->fPointType ) {

			case eFinalPoint:
				process = false;
			default:
				*vList++ = *srcV++;
				break;

			case eBezControlPoint:
					// insure we have two points - if not process normally
				if ( (srcV+1)->fPointType == eBezControlPoint ) {
					BezCompute( bezVectors, srcV - 1 );
					for ( i = 1; i < scBezBlendSize - 1; i++ )
						*vList++ = bezVectors[i];
					srcV += 2;
				}
				else
					*vList++ = *srcV++;
				break;
		}
	}
}

/* ==================================================================== */
static void BezCompute( scVertex*		dstV,
						const scVertex* srcV )
{
	int 				i;

	dstV[0] 				= srcV[0];
	dstV[scBezBlendSize-1]	= srcV[3];
	

	for ( i = 1;  i < scBezBlendSize - 1;  i++ ) {
		dstV[i].x = scRoundMP( srcV[0].x * bezblend[i].ca +
							 srcV[1].x * bezblend[i].cb +
							 srcV[2].x * bezblend[i].cc +
							 srcV[3].x * bezblend[i].cd );
		
		dstV[i].y = scRoundMP( srcV[0].y * bezblend[i].ca +
							 srcV[1].y * bezblend[i].cb +
							 srcV[2].y * bezblend[i].cc +
							 srcV[3].y * bezblend[i].cd );
		dstV[i].fPointType	= eCornerPoint;
	}
}
