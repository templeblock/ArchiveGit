/****************************************************************************

	File:		SCBEZIER.C


	$Header: /Projects/Toolbox/ct/SCBEZIER.CPP 2	 5/30/97 8:45a Wmanis $

	Contains:		vectorizes beziers

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

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


/* ==================================================================== */
/* count the number of vertices in the vertex list */

inline long CountVerts( const scVertex* verts )
{
	long	numVerts;
   
	for ( numVerts = 1; verts->fPointType != eFinalPoint; verts++, numVerts++ )
		;
	return numVerts;
}


/* ==================================================================== */
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

/* ==================================================================== */
/* process the list vectorizing the beziers into straight lines */

void BEZVectorizePoly( scVertex*&		dstV,
					   const scVertex*	srcV )
{
	scVertex*	vList;
	int 		i;
	Bool		process;
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

#ifdef scBezFixed

/* it is assumed that the list has enough space before entering
 * this routine
 */

static void RenderBezier( SCVertex* 	dstV,
						  scPointType	type,
						  SCBezVertex*	draw,
						  short 		minX,
						  short 		minY )
{
	size_t	i;

	for ( i = 0; i < scBezBlendSize; i++, draw++ ) {
		if ( i == 0 )
			dstV->fPointType = type;
		else
			dstV->fPointType = eCornerPoint;

		dstV->x = (long)( draw->x - minX ) << 16;
		dstV->y = (long)( draw->y - minY ) << 16;
		dstV++;
	}
}

/* ==================================================================== */


SCBezVertex *SCBezCompDrawList( SCBezVertex*	theVerts,
								SCBezVertex*	drawList )
{
	SCBezVertex*		pDraw;
	scBezBlendValue*	pBlend;
	short				i;

	drawList[0] 				= theVerts[0];
	drawList[scBezBlendSize-1]	= theVerts[3];
	
	pBlend	= bezblend + 1;
	pDraw	= drawList + 1;

	for (i = 0;  i < scBezBlendSize-2;	i++) {
		pDraw->x =(short)(((long)theVerts[0].x * (ulong)pBlend->ca
				+ (long)theVerts[1].x * (ulong)pBlend->cb
				+ (long)theVerts[2].x * (ulong)pBlend->cc
				+ (long)theVerts[3].x * (ulong)pBlend->cd) >> 16);
		pDraw->y =(short)(((long)theVerts[0].y * (ulong)pBlend->ca
				+ (long)theVerts[1].y * (ulong)pBlend->cb
				+ (long)theVerts[2].y * (ulong)pBlend->cc
				+ (long)theVerts[3].y * (ulong)pBlend->cd) >> 16);
		pBlend++;
		pDraw++;
	}
	return drawList;
}

/* ==================================================================== */

static void BezCompute( SCVertex*		dstV,
						const SCVertex* srcV )
{
	SCBezVertex 	v[4];
	SCBezVertex 	drawList[scBezBlendSize];
	register int	i;
	scPointType 	fPointType;
	short			minX;
	short			minY;

	pointType = srcV->fPointType;

	/* put source into a 16 bit quantity so that
	 * we can perform fixed point multiplies on it,
	 * and force bezier into positive coordinate space
	 * so that the fixed point multiplies with blending
	 * values will work
	 */

	minX = minY = SHRT_MAX;
	for ( i = 0; i < 4; i++ ) {
		v[i].x	= (short)( srcV->x >> 16 );
		minX	= MIN( minX, v[i].x );
		v[i].y	= (short)(srcV->y >> 16);
		minY	= MIN( minY, v[i].y );
		srcV++;
	}

	minX = ( minX < 0 ? -minX : 0 );
	minY = ( minY < 0 ? -minY : 0 );
	if ( minX || minY ) {
		for ( i = 0; i < 4; i++ ) {
			v[i].x += minX;
			v[i].y += minY;
		}
	}
	SCBezCompDrawList( v, drawList ) ;
	RenderBezier( dstV, pointType, drawList, minX, minY );
}

#endif

/* ==================================================================== */

#ifdef scBezREAL

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

#endif

/* ==================================================================== */
