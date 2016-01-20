/*****************************************************************************

	File:		SCPOLYGO.C


	$Header: /Projects/Toolbox/ct/SCPOLYGO.CPP 2     5/30/97 8:45a Wmanis $

	Contains:	Polygon code.

	Written by:	Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
*****************************************************************************/

#include "scpolygo.h" 
#include "scmem.h" 
#include "scfileio.h"

/* ==================================================================== */

void POLYDuplicate( scVertHandle*	dstOutHP,
					ushort&			dstNumVerts,
					scVertHandle	srcOutH,
					ushort			srcNumVerts )
{
    scVertex*	srcVert;
	scVertex*	dstVert;

    *dstOutHP = (scVertHandle)MEMAllocHnd( (size_t)srcNumVerts * sizeof(scVertex) );

	scAutoUnlock	h1( *dstOutHP );
	scAutoUnlock	h2( srcOutH );
	
	dstVert = (scVertex*)*h1;
	srcVert = (scVertex*)*h2;
    
	SCmemcpy( dstVert, srcVert, srcNumVerts * sizeof( scVertex ) );
	
	dstNumVerts = srcNumVerts;
}

/* ==================================================================== */

ushort POLYCountVerts( const scVertex* verts )
{
    ushort numVerts;
    
    for ( numVerts = 1; verts->fPointType != eFinalPoint; verts++, numVerts++ ) 
        ;
    return numVerts;
}

/* ==================================================================== */

MicroPoint POLYMaxDepth( scVertHandle vertH )
{
	scVertex*	verts;
	MicroPoint  maxDepth	= LONG_MIN,
				minHeight	= LONG_MAX;
    scAutoUnlock	h( vertH );

	verts = (scVertex*)*h;
	for ( ; verts->fPointType != eFinalPoint; verts++ ) {
		maxDepth	= MAX( maxDepth, verts->y );
		minHeight	= MIN( minHeight, verts->y );
    }
    
	return minHeight < 0 ? maxDepth - minHeight : maxDepth;
}

/* ==================================================================== */

long POLYExternalSize( scVertHandle	vertH,
					   long			size )
{
    return size * sizeof( scVertex );
}

/* ==================================================================== */

void POLYtoFile( APPCtxPtr	ctxPtr,
				   IOFuncPtr	writeFunc,
				   scVertHandle	vertH,
				   ushort		size )
{
    scVertex*	verts;
	long		xsize = POLYExternalSize( vertH, size );
	scAutoUnlock	h( vertH );

	verts = (scVertex*)*h;
	
//	SCPIO_WritePolygon( ctxPtr, writeFunc, verts, (size_t)size );
}

/* ==================================================================== */

scVertHandle POLYfromFile( APPCtxPtr	ctxPtr,
						   IOFuncPtr	readFunc,
						   ushort		size )
{
    scVertHandle volatile	vertH = 0;
    scVertex*				verts;  

    vertH = (scVertHandle)MEMAllocHnd( (size_t)size * sizeof( scVertex ) );

	try {
		scAutoUnlock	h( vertH );		
		verts = (scVertex*)*h;
	
//		SCPIO_ReadPolygon( ctxPtr, readFunc, verts, (size_t)size );
	} catch ( ... ) {
        MEMFreeHnd( vertH );
		throw;
    } 

    return vertH;
}

/* ==================================================================== */

