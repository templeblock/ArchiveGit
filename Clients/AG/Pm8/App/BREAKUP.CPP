/*
// BREAKUP.CPP
//
//		Copyright (c) 1993 MicroLogic Software, Inc.
//
// Code to break up two overlapping rectangles into disjoint pieces.
//
// $Header: /PM8/App/BREAKUP.CPP 1     3/03/99 6:03p Gbeddow $
//
// $Log: /PM8/App/BREAKUP.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:12   Fred
// Initial revision.
// 
//    Rev 1.1   07 Feb 1997 08:43:18   Fred
//  
// 
//    Rev 1.0   14 Mar 1996 13:25:06   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:26:32   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:33:30   JAY
// Reverted. New series.
# 
#    Rev 1.0   16 Dec 1993 16:28:04   JAY
#  
// 
//    Rev 1.0   12 Mar 1993 13:54:50   MACDUFF
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "pmwview.h"

#include "utils.h"

static int16 breakup_contain(PBOX_PTR r1, PBOX_PTR r2, PBOX_PTR output);

//
// breakup_overlap()
//
//		This function takes two overlapping rectangles and breaks them up into
// the minimum number of disjoint rectangles.  The new rectangles are stored
// in output[].
//
//		On entry, r1 and r2 point to the two overlapping rectangles.  They are 
// not modified by this function.
//
//		On exit:
//		*r1_pieces is the number of pieces r1 was broken into, NOT including 
// the overlapping part.
//		*r2_pieces is the corresponding value for r2.  
//		output[0] is the overlapping portion of the rectangles.
// 	output[1]..output[*r1_pieces] are the pieces of r1; 
//		output[*r1_pieces+1]..output[*r1_pieces+*r2_pieces] are the pieces of r2.
//
//	The maximum number of pieces that can be generated is five, i.e.
// 1 + *r1_pieces + *r2_pieces <= 5.
//
// This function does not verify that r1 and r2 overlap.  If they do not,
// results are undetermined.
//

PUBLIC void breakup_overlap(
	PBOX_PTR r1,
	PBOX_PTR r2,
	PBOX_PTR output,
	int16 *r1_pieces,
	int16 *r2_pieces)
{
	PBOX r_overlap;

	r_overlap = *r1;
	IntersectBox(&r_overlap, &r_overlap, r2);

	output[0] = r_overlap;

	*r1_pieces = breakup_contain(r1, &r_overlap, output + 1);
	*r2_pieces = breakup_contain(r2, &r_overlap, output + 1 + *r1_pieces);
}

//
// breakup_contain()
//
//		Given rectangles r1 and r2, where r1 wholly contains r2, this function
// breaks r1 up into rectangles disjoint from one another and from r2, so
// that the union of the new pieces and r2 equals r1.  The new pieces are
// stored in output[].  r1 and r2 are not modified.
//
//		The return value is the number of pieces r1 was broken into.  It can
// range from zero (if r1 equals r2) to four (if r2 is entirely in the
// interior of r1).
//
//		The code here makes horizontal strips as large as possible because
// text tends to be horizontal, so updating wide regions is likely to be
// more efficient than updating tall ones.
//

static int16 breakup_contain(PBOX_PTR r1, PBOX_PTR r2, PBOX_PTR output)
{
	PBOX r;
	int16 count;

	count = 0;
	if (r2->y0 > r1->y0)
	{
		// Break off top rectangle
		r.x0 = r1->x0;
		r.y0 = r1->y0;
		r.x1 = r1->x1;
		r.y1 = r2->y0;
		output[count++] = r;
	}
	if (r2->y1 < r1->y1)
	{
		// Break off bottom rectangle
		r.x0 = r1->x0;
		r.y0 = r2->y1;
		r.x1 = r1->x1;
		r.y1 = r1->y1;
		output[count++] = r;
	}
	if (r2->x0 > r1->x0)
	{
		// Break off left rectangle
		r.x0 = r1->x0;
		r.y0 = r2->y0;
		r.x1 = r2->x0;
		r.y1 = r2->y1;
		output[count++] = r;
	}
	if (r2->x1 < r1->x1)
	{
		// Break off right rectangle
		r.x0 = r2->x1;
		r.y0 = r2->y0;
		r.x1 = r1->x1;
		r.y1 = r2->y1;
		output[count++] = r;
	}
	return (count);
}


