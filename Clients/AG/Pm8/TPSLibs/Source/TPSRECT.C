/******************************************************************************

	Source Name: tpsrect.c
   Description: TPS Rectangle and Point utility
   Version:     v0.01
   Author:      Michael D. Houle
   Date:        2 February 1995
   Copyright:   (c) Turning Point Software, Inc., 1995.  All Rights Reserved.

   Revision History:

   Date     Who      What
	2/2/95	MDH		Initial Version

	NOTES:

	TODO:
******************************************************************************/

#include "tpsrect.h"


ASSERTNAME


#define	ISTRECTNOTEMPTY(lptrc)		(BOOLEAN)( (lptrc->left < lptrc->right) && (lptrc->top < lptrc->bottom) )

//
//		trcOffset
//
//			Offset a TRECT strcture by the given deltas
//
//		OnEntry:
//			lptrc	-	the Original rectangle
//			swX	-	the X Delta offset
//			swY	-	the Y Delta offset
//
//		OnExit:
//			lptrc	-	the Offset Rect rectangle
//
//		Returns:
//			Nothing
//
TPSAPI_DECLP( VOID ) trcOffset( LPTRECT lptrc, TSIZE swX, TSIZE swY )
{
	lptrc->left		+= swX;
	lptrc->right	+= swX;
	lptrc->top		+= swY;
	lptrc->bottom	+= swY;
}


//
//		trcInset
//
//			Inset the rectangle by the given deltas
//			(to outset the rectangle use negative deltas)
//
//		OnEntry:
//			lptrc	- the Original rectangle
//			swX	- the X Delta
//			swY	- the Y Delta
//
//		OnExit:
//			lptrc	- the Modified rectangle
//
//		Returns:
//			Nothing
//
TPSAPI_DECLP( VOID )		trcInset( LPTRECT lptrc, TSIZE swX, TSIZE swY )
{
	lptrc->left		+= swX;
	lptrc->right	-= swX;
	lptrc->top		+= swY;
	lptrc->bottom	-= swY;
}


//
//		trcIntersect
//
//			Calculate the intersection of two rectangles
//
//		OnEntry:
//			lptrc1		-	One of the rectangles
//			lptrc2		-	the Other rectangle
//
//		OnExit:
//			lptrcDst	-	The intersection of the rectangle
//
//		Returns:
//			TRUE if the rectangles intersected
//			FALSE if they did not
//
//		Side Effects:
//			The destination CAN be one of the sources...
//
//		Note:
//			If the rectangles do not intersect, the resultant rectangle
//			is invalid.
//
TPSAPI_DECLP( BOOLEAN ) trcIntersect( LPTRECT lptrc1, LPTRECT lptrc2, LPTRECT lptrcDst )
{
	lptrcDst->left		= (lptrc1->left > lptrc2->left)?			lptrc1->left : lptrc2->left;
	lptrcDst->top		= (lptrc1->top > lptrc2->top)? 			lptrc1->top : lptrc2->top;
	lptrcDst->right	= (lptrc1->right > lptrc2->right)? 		lptrc2->right : lptrc1->right;
	lptrcDst->bottom	= (lptrc1->bottom > lptrc2->bottom)? 	lptrc2->bottom : lptrc1->bottom;
	return ISTRECTNOTEMPTY(lptrcDst);
}



//
//		TRcUnion
//
//			Calculate the union of two rectangles
//
//		OnEntry:
//			lptrc1		-	One of the rectangles
//			lptrc2		-	the Other rectangle
//
//		OnExit:
//			lptrcDst	-	The intersection of the rectangle
//
//		Returns:
//			TRUE if the rectangles intersected
//			FALSE if they did not
//
//		Side Effects:
//			The destination CAN be one of the sources...
//
//		Note:
//			If the rectangles do not intersect, the resultant rectangle
//			is invalid.
//
TPSAPI_DECLP( VOID ) trcUnion( LPTRECT lptrc1, LPTRECT lptrc2, LPTRECT lptrcDst )
{
	lptrcDst->left		= (lptrc1->left > lptrc2->left)?			lptrc2->left : lptrc1->left;
	lptrcDst->top		= (lptrc1->top > lptrc2->top)? 			lptrc2->top : lptrc1->top;
	lptrcDst->right	= (lptrc1->right > lptrc2->right)? 		lptrc1->right : lptrc2->right;
	lptrcDst->bottom	= (lptrc1->bottom > lptrc2->bottom)? 	lptrc1->bottom : lptrc2->bottom;
}



//
//		trcPintpt
//
//			Pin the Given point into the given rectangle
//
//		OnEntry:
//			lptrc		-	The bounding rectangle
//			ptpt		-	The point to pin
//
//		OnExit:
//			ptpt		- The Pinned point
//
//		Returns:
//			Nothing
//
TPSAPI_DECLP( VOID ) trcPintpt( LPTRECT lptrc, PTPT ptpt )
{
	if (ptpt->x < lptrc->left)			ptpt->x = lptrc->left;
	else if (ptpt->x > lptrc->right)	ptpt->x = lptrc->right;

	if (ptpt->y < lptrc->top)				ptpt->y = lptrc->top;
	else if (ptpt->y > lptrc->bottom)	ptpt->y = lptrc->bottom;
}

//
//		trcPintrc
//
//			Pin a rectangle inside of another rectangle
//		lptrc2 must be smaller than lptrc1
//
//		OnEntry:
//			lptrc1	- the outer rectangle
//			lptrc2	- the rectangle to pin
//
//		OnExit:
//			lptrc2	- the pinned rectangle
//
//		Returns:
//			Nothign
//
TPSAPI_DECLP( VOID )		trcPintrc( LPTRECT lptrc1, LPTRECT lptrc2 )
{
	TpsAssert( trcWidth(lptrc1) > trcWidth(lptrc2), "trcPintrc: lptrc2 is wider than lptrc1");
	TpsAssert( trcHeight(lptrc1) > trcHeight(lptrc2), "trcPintrc: lptrc2 is wider than lptrc1");

	//	If lptrc1 left is larger than lptrc2 left - adjust by difference
	//		likewise if lptrc1 right is smaller
	if (lptrc1->left > lptrc2->left)
		trcOffset( lptrc2, lptrc1->left - lptrc2->left, 0);
	else if (lptrc1->right < lptrc2->right)
		trcOffset( lptrc2, lptrc2->right - lptrc1->right, 0);
	
	//	If lptrc1 top is larger than lptrc2 top - adjust by difference
	//		likewise if lptrc1 bottom is smaller
	if (lptrc1->top > lptrc2->top)
		trcOffset( lptrc2, lptrc1->top - lptrc2->top, 0);
	else if (lptrc1->bottom < lptrc2->bottom)
		trcOffset( lptrc2, lptrc2->bottom - lptrc1->bottom, 0);
}



//
//		trcCentertrc
//
//			Center the lptrc2 upon lptrc1.
//				If lptrc2 is larger than lptrc1, the larger parts will
//			be equally over on lptrc1's sides.
//
//		OnEntry:
//			lptrc1		- the rectangle to center on
//
//		OnExit:
//			lptrc2		- the new centered rectangle
//
//		Returns:
//			Nothing
//
TPSAPI_DECLP( VOID )		trcCentertrc( LPTRECT lptrc1, LPTRECT lptrc2 )
{
	TSIZE	swX1	= (lptrc1->left + lptrc1->right) / 2;
	TSIZE	swY1	= (lptrc1->top + lptrc1->bottom) / 2;
	TSIZE	swX2	= (lptrc2->left + lptrc2->right) / 2;
	TSIZE	swY2	= (lptrc2->top + lptrc2->bottom) / 2;
	//	the centering is accomplished by computing the center of the
	//	two rectangles and offsetting lptrc2 by the difference in the
	//	centers to align them
	trcOffset(lptrc2, swX1-swX2, swY1-swY2);
}


//
//		trcCentertpt
//
//			Return the center point of the rectangle
//
//		OnEntry:
//			lptrc		- the rectangle to compute the center of...
//
//		OnExit:
//			ptpt		- the center coordinate of the rectangle
//
//		Returns:
//			Nothing
//
TPSAPI_DECLP( VOID ) trcCentertpt( LPTRECT lptrc, PTPT ptpt )
{
	// any reasonable compiler will optimize this into a shift
	//	and not a divide the divide is just easier to read....
	ptpt->x	= (lptrc->right + lptrc->left) / 2;
	ptpt->y	= (lptrc->bottom + lptrc->top) / 2;
												
}


//
//		trcIsEmpty
//
//			Calculate if the right is less than or equal to the left
//			or if the bottom is less than or equal to the top.
//
//		OnEntry:
//			lptrc		-	One of the rectangles
//
//		OnExit:
//
//		Returns:
//			TRUE if the rectangle is empty
//			FALSE if it is not
//
//
TPSAPI_DECLP( BOOLEAN ) trcIsEmpty( LPTRECT lptrc )
{
	return ! ISTRECTNOTEMPTY(lptrc);
}


//
//		IstrcEqual
//
//			Are the two rectangles the same
//
//		OnEntry:
//			lptrc1		-	One of the rectangles
//			lptrc2		-	the Other rectangle
//
//		OnExit:
//			Nothing
//
//		Returns:
//			TRUE if the rectangles are the same
//			FALSE if they are not
//
//
TPSAPI_DECLP( BOOLEAN ) trcIsEqual( LPTRECT lptrc1, LPTRECT lptrc2 )
{
	PULONG	pul1	=	(PULONG)lptrc1;
	PULONG	pul2	=	(PULONG)lptrc2;

	//	rectangle is equal if all coordinates are equal and therefore
	//		the two pairs of longs are equal
	if ( (pul1[0] == pul2[0]) && (pul1[1] == pul2[1]) )
		return TRUE;
	return FALSE;
}


//
//		trcIsIntersect
//
//			check if the two rectangles intersect
//
//		OnEntry:
//			lptrc1		-	One of the rectangles
//			lptrc2		-	the Other rectangle
//
//		OnExit:
//			lptrcDst	-	The intersection of the rectangle
//
//		Returns:
//			TRUE if the rectangles intersected
//			FALSE if they did not
//
//		Side Effects:
//			The destination CAN be one of the sources...
//
TPSAPI_DECLP( BOOLEAN ) trcIsIntersect( LPTRECT lptrc1, LPTRECT lptrc2 )
{
	TRECT	trcDst;
	return trcIntersect(lptrc1, lptrc2, &trcDst);
}



//
//		trcIntrc
//
//			Returns TRUE if lptrc2 lies complete within lptrc1
//
//		OnEntry:
//			lptrc1	- the Bounding rectangle
//			lptrc2	- the rectangle to test with
//
//		OnExit:
//			Nothing
//
//		Returns:
//			TRUE on success and FALSE on failure
//
TPSAPI_DECLP( BOOLEAN ) trcIntrc( LPTRECT lptrc1, LPTRECT lptrc2 )
{
	if ((lptrc2->left < lptrc1->left)		||
		 (lptrc2->top < lptrc1->top)			||
		 (lptrc2->right >= lptrc1->right)	||
		 (lptrc2->bottom >= lptrc1->bottom) )
		 return FALSE;
	return TRUE;
}


//
//---------------------------------------------------------------
//			Point Functions
//

//
//		tptAdd
//
//			Add The two points and store the result into the thrid
//
//		OnEntry:
//			tpt1		-	One of the points
//			tpt2		-	the Other point
//
//		OnExit:
//			ptptDst	- the result (which can be one of the sources)
//
//		Returns:
//			Nothing
//
TPSAPI_DECLP( VOID ) tptAdd( TPT tpt1, TPT tpt2, PTPT ptptDst )
{
	ptptDst->x	= tpt1.x + tpt2.x;
	ptptDst->y	= tpt1.y + tpt2.y;
}


//
//		tptDelta
//
//			Returns the difference of the two points in the third
//
//		OnEntry:
//			tpt1		-	One of the points
//			tpt2		-	the Other point
//
//		OnExit:
//			ptptDst	- the result (which can be one of the sources)
//
//		Returns:
//			Nothing
//
TPSAPI_DECLP( VOID ) tptDelta( TPT tpt1, TPT tpt2, PTPT ptptDst )
{
	ptptDst->x	= tpt2.x - tpt1.x;
	ptptDst->y	= tpt2.y - tpt1.y;
}


//
//		tptIntrc
//
//			Determine if the point lies inside of the rectangle
//
//		OnEntry:
//			lptrc		- the desired rectangle to compare against
//			ptpt		- the point
//
//		OnExit:
//			None
//
//		Returns:
//			TRUE if the point lies inside the rectangle
//			FALSE if they did not
//
//
TPSAPI_DECLP( BOOLEAN ) tptIntrc( LPTRECT lptrc, TPT tpt )
{
	if ( (lptrc->top <= tpt.y) && (lptrc->bottom > tpt.y) &&
			(lptrc->left <= tpt.x) && (lptrc->right > tpt.x) )
		return TRUE;
	return FALSE;
}



