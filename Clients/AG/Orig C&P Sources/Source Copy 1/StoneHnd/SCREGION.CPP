/****************************************************************************

	File:		SCREGION.C


	$Header: /Projects/Toolbox/ct/SCREGION.CPP 3     5/30/97 8:45a Wmanis $

	Contains:	HiRes region implementation.

	Written by: Lucas

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
***************************************************************************/

#include "scbezier.h"
#include "scmem.h"
#include "scfileio.h"
#include "scregion.h"
#include <limits.h>

#define edge_growSize		   64


/****************************************************************************/
/***********************  REGION  *******************************************/
/****************************************************************************/


struct HEdge {					/* used in scan line conversion			*/
	MicroPoint	y1; 			/* y-coord of first vertex				*/
	MicroPoint	y2; 			/* y-coord of second vertex				*/
	MicroPoint	x1; 			/* x-coord of first vertex				*/
	MicroPoint	x;				/* current x-coord (moving along edge)	*/
	MicroPoint	dx;
	MicroPoint	dy;
	
};

typedef scMemHandle HEdgeHandle;


/*******************************************************************/
/*******************************************************************/
/* LOCAL PROTOTYPES */

static MicroPoint	Massage( MicroPoint, MicroPoint, int );

static void			PlainInsetHRgn( HRgnHandle, MicroPoint, MicroPoint );
static void			XGrowHRgn( HRgnHandle, MicroPoint );
static void			XShrinkHRgn( HRgnHandle, MicroPoint );
static void			YGrowHRgn( HRgnHandle, MicroPoint );
static void			YShrinkHRgn( HRgnHandle, MicroPoint );

static void			ConstructEdge( const scVertex*,
								   HEdgeHandle*,
								   int*,
								   MicroPoint* );

static int			RemInactive( HEdge*, MicroPoint, int );


/****************************************************************************/
/* Relocate top or bottom edge of an enclosing rectangle so that its		*/
/* y-position is evenly divisible by sliver_size. If mode is -1, we are		*/
/* dealing with the top edge; if it is 1, the bottom edge.					*/
/* Err on the side of making region larger. 								*/

typedef enum eEdgeModes {
	eTopEdge,
	eBottomEdge
} eEdgeMode;

static MicroPoint Massage ( MicroPoint	pos,
							MicroPoint	size,
							int		mode )
{
	MicroPoint		rem;

	if ( ( rem = ABS( pos % size ) ) != 0 ) {	
		/* take abs because sign of result of % is machine dependent. */
		if ( pos < 0 ) {						/* negative y-coordinate */
			if ( mode < 0 )						/* top edge */
				return( pos - ( size - rem ) );
			else								/* bottom edge */
				return( pos + rem );
		}
		else {									/* positive y-coordinate */
			if ( mode < 0 ) 					/* top edge */
				return( pos - rem );
			else								/* bottom edge */
				return( pos + size - rem );
		}
	}
	else										/* no remainder */
		return pos;
}

/***************************************************************************/
/* Like Massage, but simpler, since it is used for offset points,	*/
/* and we don't have to worry about which edge we are looking at.   */

static MicroPoint PointMassage( MicroPoint	ypt,
								MicroPoint	size )
{
	MicroPoint 	rem;

		/* take abs because sign of result of % is machine dependent. */
	if ( ypt ) {	
		rem = ABS( ypt % size );
	
		if ( rem <= ( size/2 ) ) {
			if ( ypt < 0 )
				ypt += rem;
			else
				ypt -= rem;
		}
		else {
			if ( ypt < 0)
				ypt -= size - rem;
			else
				ypt += size - rem;
		}
	}
	return ypt;
}


/****************************************************************************/
/* Allocate space for a region and initialize everything to zero.  */

#if SCDEBUG > 1
HRgnHandle NewHRgnDebug( MicroPoint		theSliverSize,
						 const char*	fn,
						 int			line )
{
#else
HRgnHandle NewHRgn( MicroPoint	theSliverSize )
{
#endif
	
	HRgnHandle	rgnH;
	HRgn*		r;

#if defined( MEM_DEBUG )
	rgnH = (HRgnHandle)MEMAllocHndDebug( sizeof( HRgn ), fn, line );
#else
	rgnH = (HRgnHandle)MEMAllocHnd( sizeof( HRgn ) );	
#endif

	scAutoUnlock	h( rgnH );
	r = (HRgn*)*h;
	
	r->fOrigBounds.Set( 0, 0, 0, 0 );
	r->fMaxBounds.Set( 0, 0, 0, 0 );
	
	r->fOrg.Set( 0, 0 );

	r->fVertInterval	= theSliverSize;
	r->fNumSlivers		= 0;
	r->fMaxSlivers		= sliver_growSize;

#if defined( MEM_DEBUG )
	r->fSlivers = MEMAllocHndDebug( sliver_growSize * sizeof( Sliver ), fn, line );
#else
	r->fSlivers = MEMAllocHnd( sliver_growSize * sizeof( Sliver ) );	
#endif
	
	return rgnH;
}

/***************************************************************************/
/* dispose of a region */

void DisposeHRgn( HRgnHandle rgnH )
{
	HRgn* r = (HRgn*)scMemDeref( rgnH );

	MEMFreeHnd( r->fSlivers );

	MEMFreeHnd( rgnH );
}


/***************************************************************************/
/* copy source region to destination region. Space must already be */
/* allocated for destination region.							   */

void CopyHRgn( HRgnHandle		dstRgn, 
			   const HRgnHandle	srcRgn )
{
	HRgn*			dst;
	Sliver*			to;	
	const HRgn*		src;
	const Sliver*	from;
	int				diff;

	raise_if ( ( dstRgn == 0 ) || ( srcRgn == 0 ), scERRinput );

	scAutoUnlock	h1( srcRgn );
	src = (HRgn*)*h1;

	scAutoUnlock	h2( dstRgn );
	dst = (HRgn*)*h2;

	diff			= src->fMaxSlivers - dst->fMaxSlivers;
	dst->fSlivers	= MEMResizeHnd( dst->fSlivers, (ulong)src->fMaxSlivers * sizeof(Sliver) );
	
	dst->fMaxSlivers += diff;
	
	dst->fOrigBounds	= src->fOrigBounds;
	dst->fMaxBounds		= src->fMaxBounds;

	dst->fOrg			= src->fOrg;

	dst->fVertInterval	= src->fVertInterval;
	dst->fNumSlivers	= src->fNumSlivers;

	
	scAutoUnlock	h3( dst->fSlivers );
	to		= (Sliver*)*h3;

	scAutoUnlock	h4( src->fSlivers );
	from	= (Sliver*)*h4;
	
	SCmemmove( to, from, src->fNumSlivers * sizeof( Sliver ) );
}


/****************************************************************************/
/* Destroy previous structure of region and set it to the rectangle	*/
/* defined by (0,0) (0,0).											*/

void SetEmptyHRgn( HRgnHandle	rgnH )
{
	HRgn*	r;

	scAutoUnlock	h( rgnH );
	r = (HRgn*)*h;

	r->fSlivers = MEMResizeHnd( r->fSlivers, sliver_growSize * sizeof( Sliver ) );

	r->fOrigBounds.Set( 0, 0, 0, 0 );
	r->fMaxBounds.Set( 0, 0, 0, 0 );
	
	r->fOrg.Set( 0, 0 );

	r->fNumSlivers		= 0;
	r->fMaxSlivers		= sliver_growSize;
}


/***************************************************************************/
/* Destroy previous structure of region and set it to the rectangle	*/
/* defined by rec.													*/

void RectHRgn( HRgnHandle		rgnH,
			   const scXRect&	rec )
{
	HRgn*		r;
	Sliver* 	sliverPtr;
	Sliver*		endSliver;
	MicroPoint	left;
	MicroPoint	right;
	MicroPoint	top;
	int			newMax;

	scAutoUnlock	h1( rgnH );
	r = (HRgn*)*h1;

	r->fOrigBounds		= rec;

	r->fMaxBounds.x1	= left		= rec.x1;
	r->fMaxBounds.y1	= top 		= Massage( rec.y1, r->fVertInterval, -1 );
	r->fMaxBounds.x2	= right		= rec.x2;
	r->fMaxBounds.y2	= Massage( rec.y2, r->fVertInterval, 1 );
	
	r->fOrg.x		= 0;
	r->fOrg.y		= 0;
	
		/* guaranteed no remainder */	
	r->fNumSlivers	= (int)( r->fMaxBounds.Depth() / r->fVertInterval );

	if ( r->fMaxSlivers < r->fNumSlivers ) {
		newMax			= r->fNumSlivers + sliver_growSize - ( r->fNumSlivers % sliver_growSize );
		r->fSlivers		= MEMResizeHnd( r->fSlivers, newMax * sizeof( Sliver ) );
		r->fMaxSlivers	= newMax;
	}

	scAutoUnlock	h2( r->fSlivers );
	sliverPtr	= (Sliver*)*h2;
	
	endSliver	= &sliverPtr[r->fNumSlivers];
	for ( ; sliverPtr < endSliver; top += r->fVertInterval, sliverPtr++ )
		sliverPtr->SetSliver( left, right, top );

}


/****************************************************************************/
/* Move the region, unchanged, a distance of dx horizontally and	*/
/* dy vertically on the coordinate plane.							*/

void TranslateHRgn( HRgnHandle	rgnH,
					MicroPoint	dx,
					MicroPoint	dy )
{
	scAutoUnlock	h1( rgnH );
	HRgn*	r = (HRgn*)*h1;

	dy = PointMassage( dy, r->fVertInterval );

	r->fOrg.Translate( dx, dy );
	r->fMaxBounds.Translate( dx, dy );
	r->fOrigBounds.Translate( dx, dy );
}


/**************************************************************************/
/* Redo the bounds of the region by running through the fSlivers.	*/

void HRgn::SetBounds( )
{
	Sliver*		s;
	Sliver*		end;
	MicroPoint	left,
				right,
				top,
				bottom;

	if ( fNumSlivers == 0 ) {
		fMaxBounds.Set( 0, 0, 0, 0 );
		return;
	}

	scAutoUnlock	h1( fSlivers );
	s	= (Sliver*)*h1;
	end	= &s[fNumSlivers];

	left	= s->fSLx1;
	right	= s->fSLx2;
	top		= bottom	= s->fSLTop;

	for ( s++; s < end; s++ ) {
		left	= MIN( s->fSLx1, left );
		right	= MAX( s->fSLx2, right );
		bottom	= MAX( s->fSLTop, bottom );
	}

	fMaxBounds.Set( left, top, right, bottom + fVertInterval );
	fMaxBounds.Translate( fOrg );
}


/**************************************************************************/
/* Update the bounds of the region with the new sliver coordinates. */

void HRgn::UpdateBounds( MicroPoint	x1,
						 MicroPoint	x2,
						 MicroPoint	y )
{
	if ( fNumSlivers == 0 )
		fMaxBounds.Set( x1, y, x2, y + fVertInterval );
	else {
#if 1		
		scXRect urect( x1, y, x2, y + fVertInterval );
		fMaxBounds.Union( urect );		
#else		
		MicroPoint diff = x2 - r->fMaxBounds.x1 - r->fMaxBounds.Width();
		if ( 0L < diff )
			r->fMaxBounds.x2 += diff;


		diff = r->fMaxBounds.x1 - x1;
		if ( 0L < diff ) {
			r->fMaxBounds.x1 -= diff;
			r->fMaxBounds.x2 += diff;
		}


		diff = y + r->fVertInterval - r->fMaxBounds.y1 - r->fMaxBounds.Depth();
		if ( 0L < diff ) 
			r->fMaxBounds.y2 += diff;

		diff = r->fMaxBounds.y1 - y;
		if ( 0L < diff ) { 
			r->fMaxBounds.y1 -= diff;
			r->fMaxBounds.y2 += diff;
		}
#endif		
	}
}


/**************************************************************************/
/* Make sure the original bounds have not gotten out of sync with the	  */
/* current shape of the region.											  */

void HRgn::UpdateRealBounds( )
{
	MicroPoint	left	= fOrigBounds.x1;
	MicroPoint	top		= fOrigBounds.y1;
	MicroPoint	right	= fOrigBounds.x2;
	MicroPoint	bottom	= fOrigBounds.y2;
	MicroPoint	slvSize	= fVertInterval;

	if ( left != fMaxBounds.x1 )
		left = fMaxBounds.x1;
	
	if ( top < fMaxBounds.y1 || top + slvSize > fMaxBounds.y1 )
		top = fMaxBounds.y1;
	
	if ( right != fMaxBounds.x2 )
		right = fMaxBounds.x2;
	
	if ( bottom > fMaxBounds.y2 || bottom + slvSize < fMaxBounds.y2 )
		bottom = fMaxBounds.y2;

	fOrigBounds.Set( left, top, right, bottom );
}


/**************************************************************************/
/* Update the region by adding the given sliver.			*/
/* If horizontal coordinates are not possible, AddSliver	*/
/* takes no action, and returns scSuccess. 				*/

void HRgn::AddSliver( MicroPoint	x1,
					  MicroPoint	x2,
					  MicroPoint	y )
{
	Sliver*	s;
	int		i;

	if( x2 <= x1 )
		return;

	if ( fMaxSlivers <= fNumSlivers ) {
		fSlivers = MEMResizeHnd( fSlivers, MEMGetSizeHnd( fSlivers ) +  sliver_growSize * sizeof( Sliver ) );
		fMaxSlivers += sliver_growSize;
	}

	UpdateBounds( x1, x2, y );

	x1	-= fOrg.x;
	x2	-= fOrg.y;
	y	-= fOrg.y;


	scAutoUnlock	h1( fSlivers );
	s		= (Sliver *)*h1;
	i		= fNumSlivers - 1;
	for ( ; 0 <= i && ( s[i].fSLTop > y || s[i].fSLTop == y && s[i].fSLx1 > x1 ); i-- )
		;
	i++;

	if ( i < fNumSlivers )
		SCmemmove( &s[i+1], &s[i], (long)( fNumSlivers - i ) * sizeof( Sliver ) );

	(&s[i])->SetSliver( x1, x2, y );
	fNumSlivers++;
}


/****************************************************************************/
/* Calculate the intersection of the two regions and place result		    */
/* in third region. If the regions have no intersection, or one 			*/
/* of the regions is empty, the destination is set to the empty region. 	*/
/* The destination region may be one of the source regions. 				*/

void SectHRgn( const HRgnHandle	srcHRgnA,
			   const HRgnHandle	srcHRgnB,
			   HRgnHandle		dstHRgn )
{
	volatile HRgnHandle	tempDstHRgn;
	const HRgn*			a;
	const HRgn*			b;
	HRgn*				dst;
	const Sliver*		sA;
	const Sliver*		sB;
	const Sliver*		endA;
	const Sliver*		endB;

	raise_if ( srcHRgnA == 0 || srcHRgnB == 0 || dstHRgn == 0, scERRinput );

	try {
		scAutoUnlock	h1( srcHRgnA );
		a = (const HRgn*)*h1;
	
		scAutoUnlock	h2(srcHRgnB );
		b = (const HRgn*)*h2;

		raise_if ( a->fVertInterval != b->fVertInterval, scERRinput );

		tempDstHRgn = NewHRgn( b->fVertInterval );

		scAutoUnlock	h3( tempDstHRgn );
		dst = (HRgn *)*h3;

		dst->fVertInterval = a->fVertInterval;

		if ( 	a->fNumSlivers == 0						||
				b->fNumSlivers == 0						||
				a->fMaxBounds.x1 >= b->fMaxBounds.x2	||
				b->fMaxBounds.x1 >= a->fMaxBounds.x2	||
				a->fMaxBounds.y1 >= b->fMaxBounds.y2	||
				b->fMaxBounds.y1 >= a->fMaxBounds.y2 	) {
			;;
		}
		else {
			scAutoUnlock	h4( a->fSlivers );
			sA		= (const Sliver*)*h4;
	
			scAutoUnlock	h5( b->fSlivers );
			sB		= (const Sliver*)*h5;
	
			endA	= &sA[a->fNumSlivers];
			endB	= &sB[b->fNumSlivers];

			while ( sA < endA && sB < endB ) {
				if ( sA->fSLTop + a->fOrg.y < sB->fSLTop + b->fOrg.y )
					sA++;
				else if ( sB->fSLTop + b->fOrg.y < sA->fSLTop + a->fOrg.y )
					sB++;
				else if ( sA->fSLx2 + a->fOrg.x <= sB->fSLx1 + b->fOrg.x )
					sA++;
				else if ( sB->fSLx2 + b->fOrg.x <= sA->fSLx1 + a->fOrg.x )
					sB++;
				else {			  /* fSlivers intersect */
					dst->AddSliver( MAX( sA->fSLx1 + a->fOrg.x, sB->fSLx1 + b->fOrg.x ),
									MIN( sA->fSLx2 + a->fOrg.x, sB->fSLx2 + b->fOrg.x ), sA->fSLTop + a->fOrg.y );
					if ( sA->fSLx2 + a->fOrg.x < sB->fSLx2 + b->fOrg.x )
						sA++;
					else
						sB++;
				}
			}
			
			scXRect	arect( a->fOrigBounds );
			scXRect	brect( b->fOrigBounds );
			arect.Intersect( brect );
			
			dst->fOrigBounds	= arect;

			dst->UpdateRealBounds( );
		}
		CopyHRgn( dstHRgn, tempDstHRgn );
	}

	catch ( ... ) {
		DisposeHRgn( tempDstHRgn );
		throw;
	} 
	
	DisposeHRgn( tempDstHRgn );
}


/****************************************************************************/
/* Calculate the union of the two regions and place result	in third region	*/
/* If both regions are empty, the destination is set to the empty region.	*/
/* The destination region may be one of the source regions. 				*/

void UnionHRgn( const HRgnHandle	srcHRgnA,
				const HRgnHandle	srcHRgnB,
				HRgnHandle	dstHRgn )
{
	volatile HRgnHandle	tempDstHRgn;
	const HRgn*			a;
	const HRgn*			b;
	HRgn*				dst;
	const Sliver*		sA;
	const Sliver*		sB;
	const Sliver*		endA;
	const Sliver*		endB;
	MicroPoint			left,
						right,
						yPos;

	raise_if ( srcHRgnA == 0 || srcHRgnB == 0 || dstHRgn == 0, scERRinput );

	try {
		scAutoUnlock	h1( srcHRgnA );
		a = (const HRgn*)*h1;
	
		scAutoUnlock	h2( srcHRgnB );
		b = (const HRgn *)*h2;

		raise_if ( a->fVertInterval != b->fVertInterval, scERRinput );

		tempDstHRgn = NewHRgn( b->fVertInterval );

		scAutoUnlock	h3( tempDstHRgn );
		dst = (HRgn*)*h3;

		dst->fVertInterval = a->fVertInterval;

		{
			scAutoUnlock	h4( a->fSlivers );
			sA		= (const Sliver *)*h4;

			scAutoUnlock	h5( b->fSlivers );
			sB		= (const Sliver*)*h5;
		
			endA	= &sA[a->fNumSlivers];
			endB	= &sB[b->fNumSlivers];

			while ( sA < endA && sB < endB ) {
				if ( sA->fSLTop + a->fOrg.y < sB->fSLTop + b->fOrg.y ) {
					dst->AddSliver( sA->fSLx1 + a->fOrg.x, sA->fSLx2 + a->fOrg.x, sA->fSLTop + a->fOrg.y );
					sA++;
				}
				else if ( sB->fSLTop + b->fOrg.y < sA->fSLTop + a->fOrg.y ) {
					dst->AddSliver( sB->fSLx1 + b->fOrg.x, sB->fSLx2 + b->fOrg.x, sB->fSLTop + b->fOrg.y );
					sB++;
				}			   /* we know y positions are equal */
				else if ( sA->fSLx2 + a->fOrg.x < sB->fSLx1 + b->fOrg.x ) {
					dst->AddSliver( sA->fSLx1 + a->fOrg.x, sA->fSLx2 + a->fOrg.x, sA->fSLTop + a->fOrg.y );
					sA++;
				}
				else if ( sB->fSLx2 + b->fOrg.x < sA->fSLx1 + a->fOrg.x ) {
					dst->AddSliver( sB->fSLx1 + b->fOrg.x, sB->fSLx2 + b->fOrg.x, sB->fSLTop + b->fOrg.y );
					sB++;
				}
				else {			/* build a sliver until change y-coord */
					/* or find disjoint sliver */
					/* or A's or B's fSlivers end */
					yPos	= sA->fSLTop + a->fOrg.y;
					left	= MIN( sA->fSLx1 + a->fOrg.x, sB->fSLx1 + b->fOrg.x );
					right	= MAX( sA->fSLx2 + a->fOrg.x, sB->fSLx2 + b->fOrg.x );
					sA++;
					sB++;
					
					for ( ;; ) {
						if ( sA < endA && sA->fSLTop + a->fOrg.y == yPos && sA->fSLx1 + a->fOrg.x <= right ) {
							if ( sA->fSLx2 + a->fOrg.x > right )
								right = sA->fSLx2 + a->fOrg.x;
							sA++;
						}
						else if ( sB < endB && sB->fSLTop + b->fOrg.y == yPos && sB->fSLx1 + b->fOrg.x <= right ) {
							if ( sB->fSLx2 + b->fOrg.x > right )
								right = sB->fSLx2 + b->fOrg.x;
							sB++;
						}
						else
							break;
					}
					
					dst->AddSliver( left, right, yPos );
				}
			}
			
			for ( ; sA < endA; sA++ ) {		/* sB >= endB */
				dst->AddSliver( sA->fSLx1 + a->fOrg.x, sA->fSLx2 + a->fOrg.x, sA->fSLTop + a->fOrg.y );
			}
			for ( ; sB < endB; sB++ ) {		/* sA >= endA */
				dst->AddSliver( sB->fSLx1 + b->fOrg.x, sB->fSLx2 + b->fOrg.x, sB->fSLTop + b->fOrg.y );
			}

			scXRect	arect( a->fOrigBounds );
			scXRect	brect( b->fOrigBounds );
			arect.Union( brect );
		
			dst->fOrigBounds	= arect;

			dst->UpdateRealBounds( );
		}
		CopyHRgn( dstHRgn, tempDstHRgn );
	}
	catch ( ... ) {
		DisposeHRgn( tempDstHRgn );
		throw;
	} 

	DisposeHRgn( tempDstHRgn );
}


/****************************************************************************/
/* Subtract srcHRgnB from srcHRgnA and place result in third region.		*/
/* If the first region is empty, the destination is set to the empty region.*/
/* The destination region may be one of the source regions. 				*/

void DiffHRgn( const HRgnHandle	srcHRgnA,
			   const HRgnHandle	srcHRgnB,
			   HRgnHandle		dstHRgn )
{
	volatile HRgnHandle	tempDstHRgn;
	const HRgn*			a;
	const HRgn*			b;
	HRgn*				dst;
	const Sliver*		sA;
	const Sliver*		sB;
	const Sliver*		endA;
	const Sliver*		endB;
	MicroPoint			left,
						right,
						yPos;
	int					flag;

	raise_if ( srcHRgnA == 0 || srcHRgnB == 0 || dstHRgn == 0, scERRinput );

	try {
		scAutoUnlock	h1( srcHRgnA );
		a = (HRgn *)*h1;
	
		scAutoUnlock	h2( srcHRgnB );
		b = (HRgn *)*h2;

		raise_if ( a->fVertInterval != b->fVertInterval, scERRinput );

		tempDstHRgn = NewHRgn( b->fVertInterval );

		scAutoUnlock	h3( tempDstHRgn );
		dst = (HRgn *)*h3;

		dst->fVertInterval = a->fVertInterval;

		{
			scAutoUnlock	h4( a->fSlivers );
			sA		= (Sliver *)*h4;
		
			scAutoUnlock	h5( b->fSlivers );
			sB		= (Sliver *)*h5;
		
			endA	= &sA[a->fNumSlivers];
			endB	= &sB[b->fNumSlivers];

			for ( ; sA < endA; sA++ ) {
				yPos	= sA->fSLTop + a->fOrg.y;
				left	= sA->fSLx1 + a->fOrg.x;
				right	= sA->fSLx2 + a->fOrg.x;
				for ( ; sB < endB && sB->fSLTop + b->fOrg.y < yPos; sB++ )
					;
				for ( ; sB < endB && sB->fSLTop + b->fOrg.y == yPos && sB->fSLx2 + b->fOrg.x <= left; sB++ )
					;

										/* process this sA sliver	*/
				for ( flag = 1; flag; )	{ 	/* we may have to go through	*/
										/* several sB fSlivers.		*/
					if ( sB >= endB || sB->fSLTop + b->fOrg.y > yPos || sB->fSLx1 + b->fOrg.x >= right ) {
						flag	= 0;
						dst->AddSliver( left, right, yPos );
					}
					else {
						if ( sB->fSLx1 + b->fOrg.x > left ) {
							dst->AddSliver( left, sB->fSLx1 + b->fOrg.x, yPos );
						}
						if ( sB->fSLx2 + b->fOrg.x >= right )
							flag = 0;
						else
							left = sB++->fSLx2 + b->fOrg.x;
					}
				}
			}

			dst->fOrigBounds	= a->fOrigBounds;
		
			dst->UpdateRealBounds( );
		}
		CopyHRgn( dstHRgn, tempDstHRgn );
	}
	catch ( ... ) {
		DisposeHRgn( tempDstHRgn );		
		throw;
	} 
	
	DisposeHRgn( tempDstHRgn );
}


/****************************************************************************/
/* Xor srcHRgnB and srcHRgnA and place result in third region.				*/
/* If the regions are equal, the destination is set to the empty region.	*/
/* The destination region may be one of the source regions. 				*/

void XorHRgn( const HRgnHandle	srcHRgnA,
			  const HRgnHandle	srcHRgnB,
			  HRgnHandle		dstHRgn )
{
	volatile HRgnHandle	tempDstHRgn;
	const HRgn*			a;
	const HRgn*			b;
	HRgn*				dst;
	const Sliver*		sA;
	const Sliver*		sB;
	const Sliver*		endA;
	const Sliver*		endB;
	MicroPoint			left,
						right,
						yPos;
	Bool				newA,
						newB;

	raise_if ( srcHRgnA == 0 || srcHRgnB == 0 || dstHRgn == 0, scERRinput );

	try {
		scAutoUnlock	h1( srcHRgnA );
		a = (const HRgn*)*h1;
	
		scAutoUnlock	h2( srcHRgnB );
		b = (const HRgn*)*h2;

		raise_if ( a->fVertInterval != b->fVertInterval, scERRinput );

		tempDstHRgn = NewHRgn( b->fVertInterval );

		scAutoUnlock	h3( tempDstHRgn );
		dst = (HRgn*)*h3;

		dst->fVertInterval = a->fVertInterval;
		{
			scAutoUnlock	h4( a->fSlivers );
			sA		= (Sliver *)*h4;
		
			scAutoUnlock	h5( b->fSlivers );
			sB		= (Sliver *)*h5;
		
			endA	= &sA[a->fNumSlivers];
			endB	= &sB[b->fNumSlivers];

			while ( sA < endA && sB < endB ) {
				if ( sA->fSLTop + a->fOrg.y < sB->fSLTop + b->fOrg.y ) {
					dst->AddSliver( sA->fSLx1 + a->fOrg.x, sA->fSLx2 + a->fOrg.x, sA->fSLTop + a->fOrg.y );
					sA++;
				}
				else if ( sB->fSLTop + b->fOrg.y < sA->fSLTop + a->fOrg.y ) {
					dst->AddSliver( sB->fSLx1 + b->fOrg.x, sB->fSLx2 + b->fOrg.x, sB->fSLTop + b->fOrg.y );
					sB++;
				}			   /* we know y positions are equal */
				else if ( sA->fSLx2 + a->fOrg.x <= sB->fSLx1 + b->fOrg.x ) {
					dst->AddSliver( sA->fSLx1 + a->fOrg.x, sA->fSLx2 + a->fOrg.x, sA->fSLTop + a->fOrg.y );
					sA++;
				}
				else if ( sB->fSLx2 + b->fOrg.x <= sA->fSLx1 + a->fOrg.x ) {
					dst->AddSliver( sB->fSLx1 + b->fOrg.x, sB->fSLx2 + b->fOrg.x, sB->fSLTop + b->fOrg.y );
					sB++;
				}
				else {		/* fSlivers intersect */
					yPos = sA->fSLTop + a->fOrg.y;
					if ( sA->fSLx1 + a->fOrg.x < sB->fSLx1 + b->fOrg.x ) {
						left	= sA->fSLx1 + a->fOrg.x;
						right	= sB->fSLx1 + b->fOrg.x;
					}
					else {
						left	= sB->fSLx1 + b->fOrg.x;
						right	= sA->fSLx1 + a->fOrg.x;
					}

						// the third case -- where the left edges are equal
						// is handled implicitly. If AddSliver() sees x1
						// and x2 which are equal, it will return without
						// doing anything.
						//
					newA = newB = 0;
					for ( ;; ) {
						if ( newA ) {
							if ( sA >= endA || sA->fSLTop + a->fOrg.y > yPos || sA->fSLx1 + a->fOrg.x > sB->fSLx2 + b->fOrg.x) {
								dst->AddSliver( left, sB->fSLx2 + b->fOrg.x, yPos );
								sB++;
								break;
							}
							else {
								right = sA->fSLx1 + a->fOrg.x;
							}
						}
						if ( newB ) {
							if ( sB >= endB || sB->fSLTop + b->fOrg.y > yPos || sB->fSLx1 + b->fOrg.x > sA->fSLx2 + a->fOrg.x) {
								dst->AddSliver( left, sA->fSLx2 + a->fOrg.x, yPos );
								sA++;
								break;
							}
							else {
								right = sB->fSLx1 + b->fOrg.x;
							}
						}

						dst->AddSliver( left, right, yPos );

						if ( sA->fSLx2 + a->fOrg.x == sB->fSLx2 + b->fOrg.x ) {
							sA++;
							sB++;
							break;
						}

						if ( sA->fSLx2 + a->fOrg.x < sB->fSLx2 + b->fOrg.x ) {
							newA = 1;
							newB = 0;
							left = sA->fSLx2 + a->fOrg.x;
							sA++;
						}
						else {		/* sB->fSLx2 + b->fOrg.x < sA->fSLx2 + a->fOrg.x */
							newB = 1;
							newA = 0;
							left = sB->fSLx2 + b->fOrg.x;
							sB++;
						}

					}	/* inner for	*/
				}		/* big else		*/
			}			/* while		*/

			for ( ; sA < endA; sA++ ) {	/* sB >= endB */
				dst->AddSliver( sA->fSLx1 + a->fOrg.x, sA->fSLx2 + a->fOrg.x, sA->fSLTop + a->fOrg.y );
			}
			for ( ; sB < endB; sB++ ) {	/* sA >= endA */
				dst->AddSliver( sB->fSLx1 + b->fOrg.x, sB->fSLx2 + b->fOrg.x, sB->fSLTop + b->fOrg.y );
			}

			scXRect	arect( a->fOrigBounds );
			scXRect	brect( a->fOrigBounds );
			arect.Union( brect );
		
			dst->fOrigBounds	= arect;

			dst->UpdateRealBounds( );
		}
		CopyHRgn( dstHRgn, tempDstHRgn );
	} 
	catch ( ... ) {
		DisposeHRgn( tempDstHRgn ); 
		throw;
	} 

	DisposeHRgn( tempDstHRgn );
}


/**************************************************************************/
/* PtInHRgn returns 1 if the point is in the region, 0 o.w.  */

Bool PtInHRgn( const HRgnHandle rgnH,
			   const scMuPoint&	mPt )

{
	HRgn*		r;
	Sliver*		s;
	MicroPoint	mx,
				my;
	int			i;
	int			hi,
				low;
	MicroPoint	yPos;

	scAutoUnlock	h1( rgnH );
	r	= (HRgn*)*h1;

	mx	= mPt.x;
	my	= mPt.y;

	if ( !r->fMaxBounds.PinRect( mPt ) )
		return false;

	scAutoUnlock	h2( r->fSlivers );
	s	= (Sliver *)*h2;

	mx	-= r->fOrg.x; 				 /* cancel out effect of offsets */
	my	-= r->fOrg.y;

	low	= 0;
	hi	= r->fNumSlivers - 1;
	for ( i = (hi+low) / 2; low <= hi; i = (hi+low) / 2 ) {
		if ( my < s[i].fSLTop)
			hi	= i - 1;
		else if ( my >= s[i].fSLTop + r->fVertInterval )
			low	= i + 1;
		else
			break;
	}

	if ( low > hi ) 	 /* didn't find any sliver with good y-value */
		return false;

	yPos	= s[i].fSLTop;
	if ( s[i].fSLx1 <= mx ) {
		for ( ;; i++ ) {
			if ( i >= r->fNumSlivers || s[i].fSLTop != yPos || mx < s[i].fSLx1 )
				return false;
			else if ( mx < s[i].fSLx2 )
				break;	  /* success */
			else
				;
		}
	}
	else {
		for ( ;; i-- ) {
			if ( i < 0 || s[i].fSLTop != yPos || s[i].fSLx2 <= mx )
				return false;
			else if ( s[i].fSLx1 <= mx )
				break;	  /* success */
		}
	}

	return true;
}


/**************************************************************************/
/* RectInHRgn returns 1 if the rectangle intersects the region, 0 o.w.	*/
/*	Note: RectInHRgn will sometimes return 1 when the rectangle merely	*/
/*	intersects the region's enclosing rectangle. If you need to know	*/
/*	whether a rectangle intersects the actual region, use RectHRgn to	*/
/*	set the rectangle to a region, and call SectHRgn to see whether		*/
/*	they intersect. If the result of SectHRgn is an empty region,		*/
/*	they don't intersect.												*/

Bool RectInHRgn( const HRgnHandle	rgnH,
				 const scXRect&		rec )
{
	HRgn*	r;
	Bool	val;

	scAutoUnlock	h1( rgnH );
	r = (HRgn*)*h1;

	val = r->fMaxBounds.Intersect( rec );
	
	return val;
}


/**************************************************************************/
/* EqualHRgn returns 1 if the regions have identical fSlivers, 0 o.w.	*/

Bool EqualHRgn( const HRgnHandle	rgnA,
				const HRgnHandle	rgnB )
{
	const HRgn*		rA;
	const HRgn*		rB;
	const Sliver*	a;
	const Sliver*	b;
	const Sliver*	endA;
	MicroPoint		aX,
					aY,
					bX,
					bY;
	Bool			same;

	scAutoUnlock	h1( rgnA );
	rA	= (const HRgn*)*h1;
	
	scAutoUnlock	h2( rgnB );
	rB	= (const HRgn*)*h2;

	if ( rA->fNumSlivers + rB->fNumSlivers == 0 ) 
		return true;

	if (	rA->fNumSlivers		!= rB->fNumSlivers		||
			rA->fVertInterval	!= rB->fVertInterval	||
			rA->fMaxBounds		!= rB->fMaxBounds	) {
		return false;
	}

	aX		= rA->fOrg.x;
	aY		= rA->fOrg.y;
	bX		= rB->fOrg.x;
	bY		= rB->fOrg.y;

	scAutoUnlock	h3( rA->fSlivers );
	a		= (const Sliver*)*h3;
	
	scAutoUnlock	h4( rB->fSlivers );
	b		= (const Sliver*)*h4;
	
	endA	= &a[rA->fNumSlivers];

	for ( same = true; same == true && a < endA; a++, b++ ) {
		if (	a->fSLx1 + aX != b->fSLx1 + bX	||
				a->fSLx2 + aX != b->fSLx2 + bX	||
				a->fSLTop + aY  != b->fSLTop + bY		)
		{
			same = false;
		}
	}

	return same;
}


/**************************************************************************/
/* EmptyHRgn returns non-zero if the region contains at least	*/
/* one sliver, 0 o.w.											*/

Bool EmptyHRgn( HRgnHandle	rgnH )
{
	HRgn*	r;
	int		num;

	r	= (HRgn *)scMemDeref( rgnH );
	num	= r->fNumSlivers;

	return num == 0;
}


/**************************************************************************/
/* InsetHRgn shrinks or expands the region. All points on the region	*/
/* boundary are moved inwards a distance of dy vertically and dx		*/
/* horizontally; if dx or dy is negative, the points are moved outwards	*/

static void OldInsetHRgn ( HRgnHandle	rgnH,
						   MicroPoint	dx,
						   MicroPoint	dy )
{
	HRgn  *r;

	if ( dx < 0 )
		XGrowHRgn( rgnH, dx );
	else if ( dx > 0 )
		XShrinkHRgn( rgnH, dx );

	r	= (HRgn*)scMemDeref( rgnH );
	dy	= PointMassage( dy, r->fVertInterval );

	if ( dy < 0)
		YGrowHRgn( rgnH, dy );
	else if ( dy > 0 )
		YShrinkHRgn( rgnH, dy );
}

/**************************************************************************/

void InsetHRgn( HRgnHandle	rgnH,
				MicroPoint	dx, 
				MicroPoint	dy,
				Bool		bestOption )
{
	MicroPoint	vertInt,
				distance;
	HRgn*		r;

	{
		scAutoUnlock	h1( rgnH );
		r = (HRgn *)*h1;
		
		r->fOrigBounds.x1	= r->fOrigBounds.x1 + dx;
		r->fOrigBounds.y1	= r->fOrigBounds.y1 + dy;
		r->fOrigBounds.x2	= r->fOrigBounds.x2 - dx;
		r->fOrigBounds.y2	= r->fOrigBounds.y2 - dy;
		
		vertInt	= 2 * r->fVertInterval;
	}
	// we should test the validity of the inset region, if it has no size or negative
	// size we should raise an exception
	
	if ( ( dx < 0 && dy > 0 ) || ( dx > 0 && dy < 0 ) ) {
		OldInsetHRgn( rgnH, dx, dy );
	}
	else if ( dx != dy || bestOption == 0 ) {
		PlainInsetHRgn( rgnH, dx, dy );
	}
	else {
		if ( dx > 0 ) {
			for ( distance = vertInt; distance <= dx; distance += vertInt ) {
				PlainInsetHRgn( rgnH, vertInt, vertInt );
			}
			distance	-= vertInt;
			PlainInsetHRgn( rgnH, dx - distance, dx - distance );
		}
		else if ( dx < 0 ) {
			for ( distance = - vertInt; distance >= dx; distance -= vertInt ) {
				PlainInsetHRgn( rgnH, - vertInt, - vertInt );
			}
			distance	+= vertInt;
			PlainInsetHRgn( rgnH, dx - distance, dx - distance );
		}
	}
}

/**************************************************************************/
/* IMPORTANT NOTE: This works only when sign( dx ) = sign( dy )	*/

static void PlainInsetHRgn( HRgnHandle	rgnH,
							MicroPoint	dx,
							MicroPoint	dy )
{
	volatile HRgnHandle	vertRgnH;
	HRgn*				r;
	MicroPoint			vertInt;

	if ( dx == 0 && dy == 0 )
		return;

	try {
		r		= (HRgn*)scMemDeref( rgnH );
		vertInt	= r->fVertInterval;

		vertRgnH = NewHRgn( vertInt );

		CopyHRgn( vertRgnH, rgnH );

		if ( dx < 0 )
			XGrowHRgn( rgnH, dx );
		else if ( dx > 0 )
			XShrinkHRgn( rgnH, dx );

		r	= (HRgn*)scMemDeref( vertRgnH );
		dy	= PointMassage( dy, r->fVertInterval );

		if ( dy < 0)
			YGrowHRgn( vertRgnH, dy );
		else if ( dy > 0 )
			YShrinkHRgn( vertRgnH, dy );

		if ( dx > 0 || dy > 0 )
			SectHRgn( rgnH, vertRgnH, rgnH );
		else
			UnionHRgn( rgnH, vertRgnH, rgnH );
	}
	catch ( ... ) {
		DisposeHRgn( vertRgnH );
		throw;
	} 

	DisposeHRgn( vertRgnH );
}

/**************************************************************************/
/* XGrowHRgn expands the region a distance of dx horizontally.	  */
/* dx is negative.												  */

static void XGrowHRgn( HRgnHandle	rgnH,
					   MicroPoint	dx )
{
	volatile HRgnHandle	newRH;
	HRgn*				r;
	HRgn*				newR;
	Sliver*				s;
	Sliver*				end;
	MicroPoint			left,
						right,
						yPos;

	try {
		scAutoUnlock	h1( rgnH );
		r = (HRgn*)*h1;

		newRH = NewHRgn( r->fVertInterval );

		scAutoUnlock	h2( newRH );
		newR	= (HRgn*)*h2;
	
		{
			scAutoUnlock	h3( r->fSlivers );
			s		= (Sliver*)*h3;

			newR->fOrg.x		= r->fOrg.x;
			newR->fOrg.y		= r->fOrg.y;
			newR->fVertInterval	= r->fVertInterval;

			end = &s[r->fNumSlivers];
			for ( ; s < end; ) {
				yPos	= s->fSLTop;
				left	= s->fSLx1 + dx;
				right	= s->fSLx2 - dx;

				for ( s++; s < end && s->fSLTop == yPos && right >= s->fSLx1 + dx; s++ )
					right = s->fSLx2 - dx;

				newR->AddSliver( left + r->fOrg.x, right + r->fOrg.x, yPos + r->fOrg.y );
			}
		}
		CopyHRgn( rgnH, newRH );		
	}
	catch ( ... ) {
		DisposeHRgn( newRH );
		throw;
	} 

	DisposeHRgn( newRH );
}

/**************************************************************************/
/* XShrinkHRgn shrinks the region a distance of dx horizontally.	*/
/* dx is positive.													*/

static void XShrinkHRgn( HRgnHandle	rgnH,
						 MicroPoint	dx )
{
	volatile HRgnHandle	newRH;
	HRgn*				r;
	HRgn*				newR;
	Sliver*				s;
	Sliver*				end;

	try {
		scAutoUnlock	h1( rgnH );
		r = (HRgn *)*h1;

		newRH = NewHRgn( r->fVertInterval );

		scAutoUnlock	h2( newRH );
		newR	= (HRgn *)*h2;

		{
			scAutoUnlock	h3( r->fSlivers );	// lock for the duration of AddSliver calls
			s		= (Sliver *)*h3;

			newR->fOrg.x		= r->fOrg.x;
			newR->fOrg.y		= r->fOrg.y;
			newR->fVertInterval	= r->fVertInterval;

			end = &s[r->fNumSlivers];
			for ( ; s < end; s++ ) {
				if ( s->fSLx2 - s->fSLx1 > dx * 2 )
					newR->AddSliver( s->fSLx1 + dx + r->fOrg.x, s->fSLx2 - dx + r->fOrg.x, s->fSLTop + r->fOrg.y );
			}
		}
		CopyHRgn( rgnH, newRH );
	}
	catch( ... ) {
		DisposeHRgn( newRH );
		throw;
	} 

	DisposeHRgn( newRH );
}

/**************************************************************************/
/* InsertSliver inserts the sliver into the given region at the 	*/
/* position "start". The fSlivers in positions "start" to "end" are	*/
/* removed from the region. If end < start, no fSlivers are removed.	*/

void HRgn::InsertSliver( MicroPoint	left,
						 MicroPoint	right,
						 MicroPoint	y,
						 Sliver**	sPP,
						 int		start,
						 int		end )
{
	Sliver* s;

	s = *sPP;
	if ( end < start ) {			/* don't remove, just insert    */
		if ( fNumSlivers == fMaxSlivers ) {
			MEMUnlockHnd( fSlivers );
			fSlivers = MEMResizeHnd( fSlivers, MEMGetSizeHnd( fSlivers ) + sliver_growSize * sizeof( Sliver ) );
			s	= *sPP	= (Sliver *)MEMLockHnd( fSlivers );
			fMaxSlivers += sliver_growSize;
		}
		if ( start < fNumSlivers )
			SCmemmove( &s[start+1], &s[start], ( fNumSlivers - start ) * sizeof( Sliver ) );
		fNumSlivers++;
	}
	else if ( start < end ) { 		/* remove more than one */
		if ( end + 1 < fNumSlivers )
			SCmemmove( &s[start+1], &s[end+1], ( fNumSlivers - end - 1 ) * sizeof( Sliver ) );
		fNumSlivers -= end - start;
	}

	(&s[start])->SetSliver( left, right, y );
}

/**************************************************************************/
/* CorpSliver, unlike its simplistic counterpart "AddSliver",		*/
/* judiciously incorporates the sliver into the given region,		*/
/* displaying consummate tact in smoothly integrating the sliver	*/
/* amid potentially overlapping sliver neighbors.					*/

void HRgn::CorpSliver( Sliver**		sPP,
					   MicroPoint	left,
					   MicroPoint	right,
					   MicroPoint	yPos )
{
	Sliver*	s;
	int		i;
	int		lo,
			hi;
	Bool	found;
	int		start;

	s = *sPP;

	if ( fNumSlivers == 0 ) {
		InsertSliver( left, right, yPos, sPP, 0, -1 );
		return;
	}

	lo		= 0;
	hi		= fNumSlivers - 1;
	found	= false;
	for ( i = (hi+lo)/2; lo <= hi; i = (hi+lo)/2 ) {
		if ( yPos < s[i].fSLTop )
			hi = i - 1;
		else if ( yPos > s[i].fSLTop )
			lo = i + 1;
		else {
			found = true;
			break;
		}
	}

	if ( !found ) { 	 /* don't remove, just insert   */
		if ( i < 0 )
			i = 0;
		else if ( i < fNumSlivers && s[i].fSLTop < yPos )
			i++;

		scAssert( i == fNumSlivers || s[i].fSLTop > yPos );

		InsertSliver( left, right, yPos, sPP, i, i-1 );
		return;
	}

	scAssert( i < fNumSlivers && yPos == s[i].fSLTop );

					/* find the right place to start.	*/
	if ( left <= s[i].fSLx2 ) {
		for ( ; i >= 0 && s[i].fSLTop == yPos && left <= s[i].fSLx2; i-- )
			;
		i++;
	} else {
		for ( ; i < fNumSlivers && s[i].fSLTop == yPos && left > s[i].fSLx2; i++ )
			;
	}

	if ( i == fNumSlivers || yPos < s[i].fSLTop ) {
		InsertSliver( left, right, yPos, sPP, i, i-1 );
		return;
	}

	scAssert( i < fNumSlivers && yPos == s[i].fSLTop && left <= s[i].fSLx2 );

	start = i;

	if ( left > s[i].fSLx1 )
		left = s[i].fSLx1;

	for ( ; i < fNumSlivers && yPos == s[i].fSLTop && right >= s[i].fSLx1; i++ )
		if ( right < s[i].fSLx2 )
			right = s[i].fSLx2;

	i--;
	InsertSliver( left, right, yPos, sPP, start, i );
}

/****************************************************************************/
/* Like GetBorders, but boolean; returns 1 if GetBorders would return		*/
/* nonzero value for *num (the number of borders).							*/

Bool HRgn::IsBorder( int	pos,
					 int	tb )
{
	Sliver*		s;
	int			i;
	MicroPoint	left,
				right,
				yPos;
	Bool		uncovered;

	scAutoUnlock	h1( fSlivers );
	s		= (Sliver *)*h1;

	i		= pos;
	left	= s[i].fSLx1;
	right	= s[i].fSLx2;
	yPos	= s[i].fSLTop + tb * fVertInterval;

	if ( tb == -1 ) {
		for ( ; i >= 0 && s[i].fSLTop > yPos; i-- )
			;
		if ( i < 0 || s[i].fSLTop != yPos ) {
			return true;
		}
		for ( ; i >= 0 && s[i].fSLTop == yPos; i-- )
			;
		i++;
	}
	else {
		for ( ; i < fNumSlivers && s[i].fSLTop < yPos; i++ )
			;
		if ( i == fNumSlivers || s[i].fSLTop != yPos ) {
			return true;
		}
	}

	uncovered = true;
	for ( ; i < fNumSlivers && s[i].fSLTop == yPos && s[i].fSLx1 <= right; i++ ) {	/* process fSlivers at this yLevel	*/
		if ( s[i].fSLx2 <= left )
			continue;
		if ( s[i].fSLx1 < right )
			uncovered = false;
		if ( s[i].fSLx1 > left || s[i].fSLx2 < right ) {
			return true;
		}
	}

	return uncovered;
}

/**************************************************************************/
/* YGrowHRgn expands the region a distance of dy vertically.		*/
/* dy is negative.													*/

class scAutoUnlock2 {
public:
	scAutoUnlock2( scMemHandle& hnd ) :
		handle_( hnd )
		{
			MEMLockHnd( handle_ );
		}
	
	~scAutoUnlock2( )
		{
			MEMUnlockHnd( handle_ );
		}

	void	*operator *() 
				{ 
					return scMemDeref( handle_ ); 
				}
	
private:
	scMemHandle& handle_;
};


static void YGrowHRgn( HRgnHandle	rgnH,
					   MicroPoint	dy )
{
	HRgnHandle	newRH;
	HRgn*		r;
	HRgn*		newR;
	Sliver*		s;
	Sliver*		newS;
	MicroPoint	yPos,
				j;
	int			i;

	try {
		scAutoUnlock	h1( rgnH );
		r = (HRgn*)*h1;

		newRH = NewHRgn( r->fVertInterval );

		scAutoUnlock	h3( newRH );
		newR	= (HRgn*)*h3;

		{
			scAutoUnlock2	h2( r->fSlivers );
			s		= (Sliver*)*h2;

			scAutoUnlock2	h4( newR->fSlivers );
			newS	= (Sliver*)*h4;

			newR->fOrg.x		= r->fOrg.x;
			newR->fOrg.y		= r->fOrg.y;
			newR->fVertInterval	= r->fVertInterval;

			for ( i = 0; i < r->fNumSlivers; i++ ) {
				yPos = s[i].fSLTop;

				if ( r->IsBorder( i, -1 ) ) {
					for ( j = dy; j < 0; j += r->fVertInterval ) 
						newR->CorpSliver( &newS, s[i].fSLx1, s[i].fSLx2, yPos + j );
				}

				newR->CorpSliver( &newS, s[i].fSLx1, s[i].fSLx2, yPos );

				if ( r->IsBorder( i, 1 ) ) {
					for ( j = r->fVertInterval; j <= -dy; j += r->fVertInterval )
						newR->CorpSliver( &newS, s[i].fSLx1, s[i].fSLx2, yPos + j );
				}
			}

			newR->SetBounds( );
		}
		CopyHRgn( rgnH, newRH );
		SCDebugTrace( 0, "done" );
	}
	catch( ... ) {
		DisposeHRgn( newRH );
		throw;
	} 
	
	DisposeHRgn( newRH );	
}

/**************************************************************************/
/* YShrinkHRgn shrinks the region a distance of dy vertically.	  */
/* dy is positive.												  */

static void YShrinkHRgn( HRgnHandle	rgnH,
						 MicroPoint	dy )
{
	volatile HRgnHandle			rH1;
	volatile HRgnHandle			rH2;
	volatile HRgnHandle			rH3;	
	HRgn*						r;
	MicroPoint					offset,
								totalOffset;
	MicroPoint					sliverSize;

	scAutoUnlock	h1( rgnH );
	r			= (HRgn *)*h1;

	try {
		sliverSize	= r->fVertInterval;

		rH1 = NewHRgn( sliverSize );

		rH2 = NewHRgn( sliverSize );

		rH3 = NewHRgn( sliverSize );


		for ( offset = sliverSize, totalOffset = 0; ; ) {

			CopyHRgn( rH1, rgnH );
			
			CopyHRgn( rH2, rgnH );

			TranslateHRgn( rH1, 0L, offset );
			TranslateHRgn( rH2, 0L, - offset );

			SectHRgn( rH1, rgnH, rH3);
			SectHRgn( rH2, rH3, rgnH );
			
			totalOffset += offset;

			if ( totalOffset == dy )
				break;

			offset = 3 * offset;
			if ( totalOffset + offset > dy )
				offset = dy - totalOffset;

			scAssert( totalOffset + offset <= dy );
		}
	}
	catch ( ... ) {
		DisposeHRgn( rH1 );
		DisposeHRgn( rH2 );
		DisposeHRgn( rH3 );
		throw;
	} 
	
	DisposeHRgn( rH1 );
	DisposeHRgn( rH2 );
	DisposeHRgn( rH3 );	
}

/************************************************************************/
/* SetPolyBounds sets the bounding rectangle of the polygon					*/

static void SetPolyBounds( const scVertex*	vList,
						   HRgnHandle		rgnH )
{
	HRgn*		r;
	MicroPoint	left	= LONG_MAX;
	MicroPoint	right	= LONG_MIN;
	MicroPoint	top		= LONG_MAX;
	MicroPoint	bottom	= LONG_MIN;

	for ( ; vList->fPointType != eFinalPoint; vList++ ) {
		left	= MIN( left, vList->x );
		right	= MAX( right, vList->x );
		top		= MIN( top, vList->y );
		bottom	= MAX( bottom, vList->y );
	}

	r		= (HRgn*)scMemDeref( rgnH );
	
	r->fOrigBounds.Set( left, top, right, bottom );
}

/**************************************************************************/
/* ConstructEdge inserts a new edge from the vertex list into the edge	*/
/* list. The edge list is sorted in increasing order of the lowest		*/
/* y point of the edge. Within equal minimum y values, edges are		*/
/* sorted in increasing x order, using the x value of the vertex		*/
/* with the minimum y value.											*/
/* The edge that is inserted connects the vertices vList[0]				*/
/* and vList[1].														*/
/* It is important to note that the ordering of vList depends on		*/
/* how we trace the edges, so vList[0] is encountered before			*/
/* vList[1]. This is not true, however, of e->fSLTop1 and e->fSLTop2. The			*/
/* point ( e->fSLx1, e->fSLTop1 ) is defined as the endpoint with lowest		*/
/* y-value, regardless of whether it was encountered first.				*/
/* "tricky" endpoints are those which will cause problems for the		*/
/* scanning algorithm if they are included as points along two			*/
/* two edges (which they in fact are). "nextY" is the y-value			*/
/* of vList[2], and is used to determine whether vList[1] is a			*/
/* "tricky" endpoint. We can't simply use "vList[2]" for two			*/
/* reasons: first, in the case of a horizontal edge that looks			*/
/* like this:
			  |_
				|,														*/
/* we must skip over one of the points which have equal y-value.		*/
/* Second, when vList[1] is a stopPoint, we must use the point			*/
/* right after the startPoint as our nextY. "secondY" is used to		*/
/* keep track of this over a series of calls to ConstructEdge().		*/
/* When an endpoint is found to be tricky, it is changed to shorten		*/
/* the edge by one MicroPoint.											*/

static void ConstructEdge( const scVertex*	vList,
						   HEdgeHandle*		edgeHP,
						   int*				numEdges,
						   MicroPoint*		secondY )
{
	HEdge*		edges;
	MicroPoint	y1,
				x,
				nextY;
	MicroPoint	y2,
				dx,
				dy;
	int			i;

	if ( vList->fPointType == eStartPoint ) {
		for (	i = 0; vList[i].y == vList[i+1].y	&&
				vList[i+1].fPointType != eStopPoint	&&
				vList[i+1].fPointType != eFinalPoint; i++ )
			;
		*secondY = vList[i+1].y;
	}

	if ( vList->y == vList[1].y )
		return;

//	if ( *numEdges * sizeof( HEdge ) == MEMGetSizeHnd( *edgeHP ) )
	if ( (*numEdges+1) * sizeof(HEdge) > MEMGetSizeHnd( *edgeHP ) )
		*edgeHP = MEMResizeHnd( *edgeHP, MEMGetSizeHnd( *edgeHP ) + sizeof( HEdge ) * edge_growSize );

	if ( vList->y < vList[1].y ) {
		y1	= vList->y;
		y2	= vList[1].y;
		x	= vList->x;
		dx	= vList[1].x - vList->x;
		dy	= vList[1].y - vList->y;
	}
	else {
		y1	= vList[1].y;
		y2	= vList->y;
		x	= vList[1].x;
		dx	= vList->x - vList[1].x;
		dy	= vList->y - vList[1].y;
	}

/*	  Determine which endpoints, if any, are tricky.	*/
/*	  If so, change that endpoint to shorten the		*/
/*	  edge by one MicroPoint.							*/

	for ( i = 1;	vList[i].fPointType != eStopPoint		&&
					vList[i].fPointType != eFinalPoint	&&
					vList[i].y == vList[i+1].y; 		)
		i++;

	if ( vList[i].fPointType == eFinalPoint || vList[i].fPointType == eStopPoint )
		nextY = *secondY;
	else
		nextY = vList[i+1].y;

	if ( vList->y < vList[1].y && vList[1].y < nextY )
		y2--;
	if ( vList->y > vList[1].y && vList[1].y > nextY )
		y1++;

/*	  Insert the edge and set the fields. */

	scAutoUnlock	h1( *edgeHP );
	edges	= (HEdge *)*h1;
	
	for (	i = *numEdges - 1;
			i >= 0 && ( y1 < edges[i].y1 || y1 == edges[i].y1 && x < edges[i].x );
			i-- )
		;
	i++;

	if ( i < *numEdges )
		SCmemmove( &edges[i+1], &edges[i], ( *numEdges - i ) * sizeof( HEdge ) );

	edges[i].y1		= y1;
	edges[i].y2		= y2;
	edges[i].x1		= x;
	edges[i].x		= x;
	edges[i].dx		= dx;
	edges[i].dy		= dy;

	(*numEdges)++;
}

/**************************************************************************/
/* RemInactive removes inactive edges from the active edge list */

static int RemInactive( HEdge  	*actives,
							MicroPoint		currentY,
							int			numActives )
{
	int			i;

	for ( i = numActives - 1; i >= 0; i-- )
		if ( actives[i].y2 < currentY ) {
			if ( i + 1 < numActives )
				SCmemmove( &actives[i], &actives[i+1], ( numActives - i - 1 ) * sizeof( HEdge ) );
			numActives--;
		}

	return numActives;
}

/**************************************************************************/
/* CalcNewX, given a current y position and a list of edges,	*/
/* calculates their new x position. 							*/

static void CalcNewX( HEdge*		e,
					  MicroPoint	yPos,
					  int			num )
{
	int		i;
	REAL	temp;

	for ( i = num-1; i >= 0; i-- ) {
		temp	= (REAL)( yPos - e[i].y1 );
		temp	*= (REAL)( e[i].dx );
		temp	/= (REAL)( e[i].dy );
		e[i].x	= (MicroPoint)temp + e[i].x1;
	}
}

/**************************************************************************/
/* SortActives sorts the edges in the active list by their x position	*/
/* at the current y-position of the scan line. A bubble sort is used	*/
/* because the list is almost always in sorted order or is very close.	*/

static void SortActives( HEdge*	e,
						 int	numActives )
{
	int		i,
			last;
	Bool	badOrder;
	HEdge	temp;

	badOrder	= true;
	last		= numActives - 1;
	for ( ; badOrder == true; last--) {
		badOrder = false;
		for ( i = 0; i < last; i++ )
			if ( e[i].x > e[i+1].x ) {
				badOrder	= true;
				temp		= e[i];
				e[i]		= e[i+1];
				e[i+1]		= temp;
			}
	}
}


/**************************************************************************/
/* AddActive, given a new scan line position, adds new active edges	*/
/* to the active edge list. 										*/

static void AddActive( HEdge*		e,
					   HEdge*		actives,
					   MicroPoint	currentY,
					   int		numEdges,
					   int*		numActives,
					   int*		numUsed )
{
	int	 	i;
	HEdge*	end;

	end	= &e[numEdges];
	for ( e += *numUsed; e < end && currentY >= e->y1; e++ ) {

		if ( currentY > e->y2 ) {
			(*numUsed)++;
			continue;
		}

		CalcNewX( e, currentY, 1 );


		for ( i = *numActives - 1; i >= 0 && e->x < actives[i].x; i-- )
			;
		i++;

		if ( i < *numActives )
			SCmemmove( &actives[i+1], &actives[i], ( *numActives - i ) * sizeof( HEdge ) );

		(*numActives)++;
		(*numUsed)++;
		actives[i] = *e;
	}
}

/**************************************************************************/
/* ScanEdges scans the edges at each scan line, adding fSlivers to	*/
/* the region accordingly.											*/

void HRgn::ScanEdges( HEdge*		a,
					  MicroPoint	curY,
					  int			numActives )
{
	HEdge*		end;
	MicroPoint	prevX,
				left	= 0,			/* to get compiler to shut up */
				right	= 0;			/* to get compiler to shut up */
	Bool		state,
				readySliver;

	readySliver	= false;
	state		= false;

	end	= &a[numActives];
	for( ; a < end; a++ ) {
		if ( state == false ) {
			prevX	= a->x;
			state	= true;
		}
		else {
			state	= false;
			if ( readySliver && right == prevX )
				right	= a->x;
			else {
				if ( readySliver )
					AddSliver( left, right, curY );
				else
					readySliver = true;
				left	= prevX;
				right	= a->x;
			}
		}
	}

	if ( readySliver )
		AddSliver( left, right, curY );

	raise_if( state, scERRlogical );
}

/**************************************************************************/

int emptyPoly( const scVertex* vlist )
{
	scXRect polyrect;

	const scVertex* v = vlist; 
	do {
		if ( v->fPointType == eStartPoint )
			polyrect.Invalidate();
		
		polyrect.x1 = MIN( v->x, polyrect.x1 );
		polyrect.y1 = MIN( v->y, polyrect.y1 );
		polyrect.x2 = MAX( v->x, polyrect.x2 );
		polyrect.y2 = MAX( v->y, polyrect.y2 );

		if ( v->fPointType == eStopPoint && ( !polyrect.Width() || !polyrect.Depth() ) )
			 return false;
		
	} while( (v++)->fPointType != eFinalPoint );
	
	return ( !polyrect.Width() || !polyrect.Depth() );
}

/* ==================================================================== */
/* PolyHRgn converts a polygon, represented by a list of vertices,	*/
/* into a region, using scan conversion techniques. 				*/
/* Destroys the previous structure of the region.					*/

void	PolyHRgn( HRgnHandle		rgnH,
				  const scVertex*	vList )
{
	const scVertex*	v;
	MicroPoint	currentY,
				vertInt,
				maxY,
				secondY;
	int			numEdges,
				numActives,
				numUsed;
	
	scVertex*				newVList	= 0;
	HEdgeHandle				edgeH		= 0;
	volatile HEdgeHandle	activeH		= 0;
	HEdge *volatile			actives = 0;
	HEdge *volatile			edges	= 0;
	HRgn  *volatile			r		= 0;
	
	try {
		raise_if( rgnH == 0, scERRinput );
		SetEmptyHRgn( rgnH );

		
		raise_if( emptyPoly( vList ), scERRinput );		// test for invalid poly
		
			// vectorize the beziers
		BEZVectorizePoly( newVList, vList );
		if ( newVList )
			vList = newVList;

		edgeH = (HEdgeHandle)MEMAllocHnd( edge_growSize * sizeof( HEdge ) );
		numEdges = 0;

		SetPolyBounds( vList, rgnH );

		for ( maxY = LONG_MIN, v = vList; v->fPointType != eFinalPoint; v++ )
			maxY = MAX( maxY, v->y );

		for ( ; vList->fPointType != eFinalPoint; vList++ ) {
			if ( vList->fPointType != eStopPoint ) {
				ConstructEdge( vList, &edgeH, &numEdges, &secondY );
			}
		}

		activeH = MEMResizeHnd( activeH, MEMGetSizeHnd( activeH ) + MEMGetSizeHnd( edgeH) );

		scAutoUnlock	h1( edgeH );
		edges		= (HEdge*)*h1;
		
		scAutoUnlock	h2( activeH );
		actives		= (HEdge*)*h2;
		
		scAutoUnlock	h3( rgnH );
		r			= (HRgn*)*h3;
		
		r->fOrg.x	= r->fOrg.y	= 0;

		vertInt		= r->fVertInterval;
		numActives	= 0;
		numUsed		= 0;

		currentY	= Massage( edges->y1 , vertInt, -1 );

		for( ; currentY <= maxY; currentY += vertInt ) {
			numActives = RemInactive( actives, currentY, numActives );
			CalcNewX( actives, currentY, numActives );
			SortActives( actives, numActives );
			AddActive( edges, actives, currentY, numEdges, &numActives, &numUsed );
			r->ScanEdges( actives, currentY, numActives );
		}
	} catch ( ... ) {
		MEMFreeHnd( edgeH );
		MEMFreeHnd( activeH );
		MEMFreePtr( newVList );
		throw;
	} 
	
	MEMFreeHnd( edgeH );
	MEMFreeHnd( activeH );
	MEMFreePtr( newVList );
}

/************************************************************************/
/* HRgnFirstLinePos returns the y position of the first line within			*/
/* the region which is an itegral value of leading from firstLinePos.		*/

MicroPoint	HRgn::FirstLinePos( MicroPoint	firstLinePos,
								MicroPoint	leading )
{
#undef FloatRgnFirstLine
#ifdef FloatRgnFirstLine
	return fMaxBounds.y1 - fOrg.y + firstLinePos;
#else
	if ( fMaxBounds.y1 - fOrg.y == 0 )
		return firstLinePos;

	if ( leading != 0 ) {
		while ( firstLinePos > fMaxBounds.y1 - fOrg.y )
			firstLinePos -= ABS( leading );
	}
	else
		firstLinePos = fMaxBounds.y1 - fOrg.y;

	return firstLinePos;
#endif
}

/*************************************************************************/

MicroPoint	RGNSliverSize( const HRgnHandle rgnH )
{
	HRgn*		r;
	MicroPoint	sliverSize;

	r			= (HRgn*)scMemDeref( rgnH );
	sliverSize	= r->fVertInterval;

	return sliverSize;
}

/*************************************************************************/

long RGNExternalSize( const HRgnHandle, long size )
{
	return HRgnSize( size );
}

/************************************************************************/

void RGNGetExtents( const HRgnHandle rgnH, scXRect& xRect )
{
	const HRgn* r = (const HRgn*)scMemDeref( rgnH );

	xRect	= r->fOrigBounds;
}

/************************************************************************/

MicroPoint RGNMaxDepth( const HRgnHandle rgnH )
{
	MicroPoint	depth	= LONG_MIN;
	const HRgn*	r		= (HRgn*)scMemDeref( rgnH );

	depth	= r->fOrigBounds.y2;

	return depth;
}

/************************************************************************/

#define SCCB_HRGN 		48
#define SCCB_SLIVER		12

Bool RGNtoFile( APPCtxPtr		ctxPtr,
				IOFuncPtr		writeFunc,
				HRgnHandle		rgnH,
				int				size )
{
	HRgn*	r;
	Bool	tf = false;

	scAutoUnlock	h( rgnH );
	r	= (HRgn*)*h;

	scAssert( size == r->fNumSlivers );

	uchar			rgnBuf[SCCB_HRGN];
	uchar*			pbuf = rgnBuf;
	
	pbuf = BufSet_long( pbuf, r->fOrigBounds.x1, kIntelOrder );
	pbuf = BufSet_long( pbuf, r->fOrigBounds.y1, kIntelOrder );
	pbuf = BufSet_long( pbuf, r->fOrigBounds.x2, kIntelOrder );
	pbuf = BufSet_long( pbuf, r->fOrigBounds.y2, kIntelOrder );

	pbuf = BufSet_long( pbuf, r->fMaxBounds.x1, kIntelOrder );
	pbuf = BufSet_long( pbuf, r->fMaxBounds.y1, kIntelOrder );
	pbuf = BufSet_long( pbuf, r->fMaxBounds.x2, kIntelOrder );
	pbuf = BufSet_long( pbuf, r->fMaxBounds.y2, kIntelOrder );
	
	pbuf = BufSet_long( pbuf, r->fOrg.x, kIntelOrder );
	pbuf = BufSet_long( pbuf, r->fOrg.y, kIntelOrder );
	pbuf = BufSet_long( pbuf, r->fVertInterval, kIntelOrder );
	
	pbuf = BufSet_short( pbuf, (ushort)r->fNumSlivers, kIntelOrder );
	pbuf = BufSet_short( pbuf, (ushort)r->fMaxSlivers, kIntelOrder );
	
	scAssert ((size_t)(pbuf-rgnBuf) == sizeof(rgnBuf));

	WriteBytes( rgnBuf, ctxPtr, writeFunc, sizeof( rgnBuf ) );
	

	scAutoUnlock	h2( r->fSlivers );
	Sliver*			sliver = (Sliver*)*h2;

	int			i;
	int			n;
	uchar		sliverBuf[SCCB_SLIVER];

	n = (int)r->fNumSlivers;

	for ( i = 0;  i < n; i++, sliver++ ) {
		pbuf = sliverBuf;
		pbuf = BufSet_long( pbuf, sliver->fSLx1, kIntelOrder );
		pbuf = BufSet_long( pbuf, sliver->fSLx2, kIntelOrder );
		pbuf = BufSet_long( pbuf, sliver->fSLTop, kIntelOrder );

		WriteBytes( sliverBuf, ctxPtr, writeFunc, sizeof( sliverBuf ) );		
	}

	return true;
}

/* ==================================================================== */

HRgnHandle RGNfromFile( APPCtxPtr		ctxPtr,
						IOFuncPtr		readFunc,
						int )
{
	HRgnHandle	rgnH;
	HRgn*		r = 0;
	Sliver*		s = 0;
	size_t		byteSize;
	Bool		tf = false;

	byteSize	= sizeof( HRgn );

	try {
		rgnH	= (HRgnHandle)MEMAllocHnd( byteSize );

		scAutoUnlock h( rgnH );
		
		r	= (HRgn*)*h;

		uchar			rgnBuf[SCCB_HRGN];
		const uchar*	pbuf = rgnBuf;
		
		ReadBytes( rgnBuf, ctxPtr, readFunc, sizeof( rgnBuf ) );

		ulong val;
		pbuf = BufGet_long( pbuf, val, kIntelOrder );
		r->fOrigBounds.x1	= val;
		pbuf = BufGet_long( pbuf, val, kIntelOrder );
		r->fOrigBounds.y1	= val;
		pbuf = BufGet_long( pbuf, val, kIntelOrder );
		r->fOrigBounds.x2	= val;
		pbuf = BufGet_long( pbuf, val, kIntelOrder );
		r->fOrigBounds.y2	= val;

		pbuf = BufGet_long( pbuf, val, kIntelOrder );
		r->fMaxBounds.x1	= val;
		pbuf = BufGet_long( pbuf, val, kIntelOrder );
		r->fMaxBounds.y1	= val;
		pbuf = BufGet_long( pbuf, val, kIntelOrder );
		r->fMaxBounds.x2	= val;
		pbuf = BufGet_long( pbuf, val, kIntelOrder );
		r->fMaxBounds.y2	= val;
	
		pbuf = BufGet_long( pbuf, val, kIntelOrder );
		r->fOrg.x	= val;
		pbuf = BufGet_long( pbuf, val, kIntelOrder );
		r->fOrg.y	= val;
		pbuf = BufGet_long( pbuf, val, kIntelOrder );
		r->fVertInterval	= val;
	
		ushort sval;
		pbuf = BufGet_short( pbuf, sval, kIntelOrder );
		r->fNumSlivers		= sval;
		pbuf = BufGet_short( pbuf, sval, kIntelOrder );
		r->fMaxSlivers		= sval;
	
		scAssert ((size_t)(pbuf-rgnBuf) == sizeof(rgnBuf));
	
		byteSize	= r->fMaxSlivers * sizeof( Sliver );

		r->fSlivers	= MEMAllocHnd( r->fMaxSlivers * sizeof( Sliver ) );

		scAutoUnlock	h2( r->fSlivers );
		s	= (Sliver*)*h2;

		int			i;
		int			n;
		uchar		sliverBuf[SCCB_SLIVER];

		n = r->fNumSlivers;

		for ( i = 0;  i < n; i++, s++ ) {

			ReadBytes( sliverBuf, ctxPtr, readFunc, sizeof( sliverBuf ) );	
			pbuf = sliverBuf;
			pbuf = BufGet_long( pbuf, val, kIntelOrder );
			s->fSLx1	= val;
			pbuf = BufGet_long( pbuf, val, kIntelOrder );
			s->fSLx2	= val;
			pbuf = BufGet_long( pbuf, val, kIntelOrder );
			s->fSLTop	= val;

		}
	} catch ( ... ) {
		if ( rgnH ) {
			r = (HRgn*)scMemDeref( rgnH );
			MEMFreeHnd( r->fSlivers );
			MEMFreeHnd( rgnH );
		}
		throw;
	} 

	return rgnH;
}

/* ==================================================================== */	

#if SCDEBUG > 1

static void InvertSliver( const Sliver&	sliver, 
						  MicroPoint	interval,
						  APPDrwCtx		appdc, 
						  HiliteFuncPtr	drawrect,
						  const scSize& size,
						  int vertflow )
{
	scXRect	rect;
	
	if ( vertflow ) {
		rect.Set( size.Width() - sliver.fSLTop, 
				  sliver.fSLx1, 
				  size.Width() - sliver.fSLTop + interval, 
				  sliver.fSLx2 );
	}
	else
		rect.Set( sliver.fSLx1, sliver.fSLTop, sliver.fSLx2, sliver.fSLTop + interval );
	(*drawrect)( rect, appdc, false );
}

/* ==================================================================== */

void RGNInvertSlivers( const HRgnHandle	rgnH,
					   APPDrwCtx		appdc,
					   HiliteFuncPtr	drawrect,
					   const scSize& size,
					   int vertflow )
{
	scAutoUnlock	h( rgnH );
	HRgn*		r	= (HRgn*)*h;
	
	scAutoUnlock	h2( r->fSlivers );
	Sliver*		s	= (Sliver*)*h2;
	
	int			i;
	
	for ( i = 0; i < r->fNumSlivers; i++ )
		InvertSliver( s[i], r->fVertInterval, appdc, drawrect, size, vertflow );
}

#endif

/* ==================================================================== */

/* 1 if width of rectangle fits in sliver starting at max(x1,s->x1) */
/* 0 o.w.                                                           */

inline Bool RectInSliver( const scXRect&	theRect,
						  Sliver*			s,
						  MicroPoint		x1 )
{
    if ( s->fSLx1 > x1 )
        x1 = s->fSLx1;
    return( x1 + theRect.Width() <= s->fSLx2 );
}

/********************************************************************/
/* 1 if sliverY is at the same (or a previous) sliver level as      */
/* floatingY; 0 o.w. SliverY is on a sliver boundary. FloatingY     */
/* is not.                                                          */

inline Bool SlivLE( MicroPoint vertInt, 
					MicroPoint sliverY, 
					MicroPoint floatingY )
{
    return( sliverY <= floatingY || floatingY <= sliverY + vertInt );
}

/********************************************************************/
/* Given a rectangle (only size is given as input, not location),   */
/* a region, the top right corner of the last rectangle placed,     */
/* and a leading value, return the rectangle which has been placed  */
/* in the first available location. Minimize y-position; within any */
/* given y-position, minimize x-position. If current y is           */
/* impossible, increment y by the leading value. If no position is  */
/* available, return a rectangle with 0 width and depth.            */
/* The rectangle passed as input is altered, and is returned as     */
/* output. theRect.w on input specifies minimum width               */
/* SectRectRgn, upon finding a location, returns the widest         */
/* rectangle that will fit.                                         */


void HRgn::SectRect( scXRect&	theRect,
					 MicroPoint topY, 
					 MicroPoint	leftX,
					 MicroPoint	lead )
{
	Sliver		*s;
	Sliver		*endSliv;
    MicroPoint	testY;
    MicroPoint  vertInt, 
    			bottomY,
    			bestX2;
	int		i;

	if ( lead != 0 ) {
		for ( ; topY < fMaxBounds.y1; topY += ABS( lead )  )
			leftX = fMaxBounds.x1;
	}
	else
		topY = fMaxBounds.y1;

    topY	-= fOrg.y;
    leftX	-= fOrg.x;

	scAutoUnlock	h1( fSlivers );
	s = (Sliver *)*h1;
	
    endSliv = &s[fNumSlivers];
    vertInt = fVertInterval;

    for ( ; s < endSliv; ) {

		if ( s->fSLTop + vertInt <= topY ) {
			s++;
		}

        else if ( s->fSLTop > topY ) {
			if ( lead == 0 )
				goto bad;
            topY += lead;
            leftX = fMaxBounds.x1 - fOrg.x;

        } else if ( !RectInSliver( theRect, s, leftX ) ) {
            s++;
        } else {

            if ( s->fSLx1 > leftX )
                leftX = s->fSLx1;

            bestX2 = s->fSLx2;

            scAssert( s->fSLTop <= topY &&
                    topY < s->fSLTop + vertInt &&
                    s->fSLx1 <= leftX &&
                    leftX + theRect.Width() <= s->fSLx2 );

            /* Start position is reasonable; does it fit?   */
            /* Loop through sliver levels, to make sure     */
            /* the whole rectangle fits.                    */
            /* If we encounter a y-level with no fSlivers,   */
            /* increment sliver pointer past this level     */
            /* and try again (topY will catch via the main  */
            /* for loop (2nd clause) ).                     */
            /* If we fail at some y-level because of x-     */
            /* coordinates, look for the next possible      */
            /* leftX at that sliver level and try that      */
            /* (keeping the same topY).                     */
            /* If there is no next possible leftX, we have  */
            /* failed at this topY;                         */
            /* try the next one, BUT don't go too far       */
            /* ahead, since we may go down and to the left  */
            /* to find success -- so just increment topY by */
            /* the leading value.                           */

            testY = topY + vertInt;
            bottomY = topY + theRect.Depth();
			for ( i = 1; ; testY += vertInt ) {

				if ( &s[i] >= endSliv )
					goto bad;
				if ( !SlivLE( vertInt, testY, bottomY ) )
					goto good;

                while ( &s[i] < endSliv && s[i].fSLTop + vertInt <= testY )
                    i++;
                if ( &s[i] >= endSliv )
                    goto bad;
				if ( s[i].fSLTop > testY  ) {		/* y-level missing	*/
                    s += i;
                    break;
                }

                while ( &s[i] < endSliv 
							&& s[i].fSLTop <= testY 
								&& s[i].fSLx2 <= leftX )
                    i++;
				
                if ( &s[i] >= endSliv )
                    goto bad;
                if ( s[i].fSLTop > testY ) {     /* y-level won't work */
					if ( lead == 0 )
						goto bad;
                    topY += lead;
                    leftX = fMaxBounds.x1 - fOrg.x;
					s++;
                    break;
                }
                if ( s[i].fSLx1 > leftX ) {
                    leftX = s[i].fSLx1;
                    break;
                }

                if ( !RectInSliver( theRect, &s[i], leftX ) ) {

                    i++;
                    while ( &s[i] < endSliv && s[i].fSLTop <= testY 
								&& !RectInSliver( theRect, &s[i], leftX ) )
                        i++;
                    if ( &s[i] >= endSliv )
                        goto bad;
                    if ( s[i].fSLTop > testY ) {     /* y-level won't work */
						if ( lead == 0 )
							goto bad;
                        topY += lead;
                        leftX = fMaxBounds.x1 - fOrg.x;
						s++;
                        break;
                    }

                    scAssert( s[i].fSLTop <= testY &&
                            testY < s[i].fSLTop + vertInt &&
                            RectInSliver( theRect, &s[i], leftX ) );

                    leftX = s[i].fSLx1;            /* try this at same y-level */
                    break;

                } else {

                    if ( s[i].fSLx2 < bestX2 )
                        bestX2 = s[i].fSLx2;
					if ( bottomY <= s[i].fSLTop + vertInt )
                        goto good;

                }

            }   /* inner for */

        }   /* the big else of the outer for loop */

    }   /* outer for loop */

	
good:
	{
		MicroPoint depth = theRect.Depth();
	    theRect.x1 = leftX + fOrg.x;	
		theRect.x2 = bestX2;

		
	    theRect.y1 = topY + fOrg.y;
		theRect.y2 = theRect.y1 + depth;
	}
	return;

bad:
    theRect.x1 = theRect.x2 = 0;
    theRect.y1 = theRect.y2 = 0;	
	return;
}

/********************************************************************/
