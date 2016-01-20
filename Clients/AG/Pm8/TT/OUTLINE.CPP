/*
// $Header: /PM8/TT/OUTLINE.CPP 1     3/03/99 6:35p Gbeddow $
// $Log: /PM8/TT/OUTLINE.CPP $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:15:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:10   Fred
// Initial revision.
// 
//    Rev 1.2   01 Jul 1997 09:08:34   Fred
// More changes for PMG 4.0
// 
//    Rev 1.1   18 Apr 1997 12:49:46   Fred
// Changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:34   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:41:54   FRED
// Initial revision.
// 
//    Rev 1.1   04 Oct 1995 10:59:28   FRED
// 32 bit TrueType engine
// 
//    Rev 1.0   29 Sep 1995 13:37:00   FRED
// Initial revision.
// 
//    Rev 1.30   15 Jan 1993 15:21:50   JAY
// Added assembly version of od_mark() and define for it.
// 
//    Rev 1.29   15 Dec 1992 16:07:40   MACDUFF
// Outlined stroking of horizontal runs, for TRUEFX only.
// 
//    Rev 1.28   15 Dec 1992 12:08:00   MACDUFF
// Restored rectangle-outline optimization, fixed clipping under banding
// so it doesn't crash.
// 
//    Rev 1.27   08 Dec 1992 13:35:34   MACDUFF
// fixed typo in previous (bad ifdef).
// 
//    Rev 1.26   08 Dec 1992 12:13:02   MACDUFF
// Disabled previous change
// 
//    Rev 1.25   08 Dec 1992 11:31:34   MACDUFF
// Optimized rectangle pens under TRUEFX by calling fill_black_rectangle
// and fill_white_rectangle.
// 
//    Rev 1.24   03 Dec 1992 17:33:46   MACDUFF
// Did the previous fix correctly -- adjustments to highBand/lowBand were
// reversed.
// 
//    Rev 1.23   20 Nov 1992 12:27:44   MACDUFF
// Fixed computation of y bounds for banding in the case where y_size < 0.
// 
//    Rev 1.22   09 Nov 1992 13:32:08   MACDUFF
// Do not stroke the bounding box in background fonts if the font is
// rotated.
// 
//    Rev 1.21   05 Nov 1992 13:00:00   MACDUFF
// Removed some unused declarations.
// 
//    Rev 1.20   31 Aug 1992 14:59:14   FRED
// Uses new sc_DrawParabola() that does not take inGY parameters.
// 
// 
//    Rev 1.19   31 Aug 1992 12:01:04   FRED
// New prototype for sc_DrawParabola() is assmebly version
// is being used.
// 
// 
//    Rev 1.18   29 Aug 1992 14:51:30   FRED
// OUTLINE_region pens are now assumes to always have
// one run/row and byte run coordinates.
// 
// 
//    Rev 1.17   25 Aug 1992 23:16:42   FRED
// Strokes the "missing" corners of a background box
// with a white foreground.
// 
// 
//    Rev 1.16   24 Aug 1992 12:19:24   FRED
// fx_pen_descriptor is now a near pointer.
// 
// 
// 
//    Rev 1.15   22 Aug 1992 16:12:02   FRED
// Adjusts lowRow and highRow with pen dimensions so that
// components which do not effect the current band can be
// ignored.
// 
// 
//    Rev 1.14   16 Aug 1992 14:55:58   FRED
// Fixed bug that was causing banding to fail with shadows. To fix this,
// an optimization in OUTLINE.C was turned off. An alternative should
// be found to speed up the scan conversion process with banding.
// 
// 
//    Rev 1.13   13 Aug 1992 17:19:40   FRED
// Added #include "fsglue.h" to get rid of warning.
// 
// 
//    Rev 1.12   11 Aug 1992 12:35:16   FRED
// Fixed bug where pen offsets and row numbers were being
// handled wrong...
// 
// All pen sizes and offsets use the Geronimo convention
// (Y up is negative, Y down is positive). The functions
// that call_fill_pattern_run must maintain the TrueType
// Y coordinate which has the Y axis inerverted.
// 
// 
//    Rev 1.11   10 Aug 1992 16:57:18   FRED
// Added code to handle OUTLINE_diagonal pens.
// 
// 
//    Rev 1.10   08 Aug 1992 16:13:58   FRED
// Added prototype for set_pattern().
// 
// 
//    Rev 1.9   08 Aug 1992 16:11:28   FRED
// DrawOutline() returns NO_ERR if there is no pen descriptor
// or stroke pattern.
// 
// 
//    Rev 1.8   07 Aug 1992 17:13:26   FRED
// Handles OUTLINE_region and OUTLINE_rectangle.
// 
// 
//    Rev 1.7   07 Aug 1992 15:15:02   FRED
// Strokes pen described by fx_pen_descriptor.
// 
// 
//    Rev 1.6   07 Aug 1992 10:27:50   FRED
// Removed Bitmap field from LocalOD structure.
// 
// Calls fill_pattern_run() to stroke the outline pattern.
// 
// 
//    Rev 1.5   06 Aug 1992 18:09:44   FRED
// This version does not plot the outline on the screen,
// instead it puts a "light" outline on the bitmap.
// 
// 
//    Rev 1.4   06 Aug 1992 11:33:32   FRED
// Reworked code to be more efficient.
// 
// 
//    Rev 1.3   06 Aug 1992 10:18:56   FRED
// Working version that displays outline in 3X3 pixels.
// 
// 
//    Rev 1.2   04 Aug 1992 17:12:42   FRED
// Draw 3X3 pixels.
// 
// 
//    Rev 1.1   04 Aug 1992 16:08:00   FRED
// Fixed several syntax errors.
// 
// 
//    Rev 1.0   04 Aug 1992 15:43:28   FRED
// Initial revision.
*/

/*
// File: outline.c (based on sc.c)
//
// This module strokes a pen shape onto a bitmap using a shape
// defined by quadratic B-splines
//
// Portions (c) Apple Computer Inc. 1987, 1988, 1989, 1990.
*/

#define multlong(a,b) SHORTMUL(a,b) /* ((a)*(b)) */

#include "fscdefs.h"
#include "fontmath.h"
#include "sfnt.h"
#include "fnt.h"
#include "sc.h"
#include "fserror.h"
#include "fsglue.h"

#include "effects.h"

#ifndef PRIVATE
#define PRIVATE static
#endif 


/*
// Define "static" data that will be needed by this module. It is kept
// in the TrueType context.
//
// Note: This structure must exactly match the corresponding definition
// in FSCONTXT.DEF
*/

struct odLocalData
{
	int32 x, y;
	int32 endx, endy;
	int32 lastx, lasty, lastw;
	int32 startx, starty, startw;
	int32 lowRow, highRow;
	int32 incX, incY;
	int32 r;
	int32 hreps;		/* MH TEST */
};

/*
// This will fool the C compiler into generating the correct offsets into
// DGROUP. The postprocessor will come around and make the offsets relative
// to DS (the context).
*/

#ifdef FSCFG_REENTRANT
#define ODP0    struct odLocalData* pLocalOD
#define ODP     struct odLocalData* pLocalOD,
#define ODA0    pLocalOD
#define ODA     pLocalOD,
#define LocalOD (*pLocalOD)
#else 
#define ODP0    void
#define ODP
#define ODA0
#define ODA

struct odLocalData LocalOD;

#endif


extern FX Fx;

// We only use the 'hreps' field (horizontal repetitions) in TRUEFX, because
// it is a speed optimization that takes more space.  This define strips out
// the code that uses it from TTDOS:

#define 		HREPS		LocalOD.hreps

// The sc_DrawParabola() function in SC.C is used to generate the outline
// vectors that define the stroke path.

int sc_DrawParabola(F26Dot6 _Ax, F26Dot6 _Ay, F26Dot6 _Bx, F26Dot6 _By, F26Dot6 _Cx, F26Dot6 _Cy, F26Dot6** hX, F26Dot6** hY, uint32* count, int32 inGY);

extern void set_pattern(int16 pattern);
extern void fill_pattern_run(int32 x, int32 y, int32 l);

// Private prototypes
                         
PRIVATE int od_mark(ODP F26Dot6 *pntbx, F26Dot6 *pntby, F26Dot6 *pntx, F26Dot6 *pnty, F26Dot6 *pnte);
PRIVATE void StrokePoint(ODP int32 x, int32 y, int32 w);

#define	Y_POINT_ON(x)	StrokePoint(ODA x, LocalOD.y, 1)
#define	Y_POINT_OFF(x)	StrokePoint(ODA x-1, LocalOD.y, -1)
#define	X_POINT(x, y)	StrokePoint(ODA x, y, 0)

// Stroke a pen onto the bitmap.
//
// Assumes that (*handle)->bbox.xmin,...xmax,...ymin,...ymax
// are already set by sc_FindExtrema ()
//
// Parameters:
//
//		glyphPtr	is a pointer to sc_CharDataType.
// 	scPtr 	is a pointer to sc_GlobalData.
//		bbox		is a pointer to sc_BitMapData.
// 	lowBand	is lowest scan line to be included in the band.
// 	highBand	is one greater than the highest scan line to be
//					included in the band.

int DrawOutline(sc_CharDataType *glyphPtr, sc_GlobalData *scPtr, int32 lowBand, int32 highBand)
{
	register F26Dot6 *x = glyphPtr->x;
	register F26Dot6 *y = glyphPtr->y;
	register ArrayIndex i, endPt, nextPt;
	register uint8 *onCurve = glyphPtr->onCurve;
	ArrayIndex startPt, j;
	LoopCount ctr;
	sc_GlobalData *p;
	F26Dot6 *xp, *yp, *x0p, *y0p;
	register F26Dot6 xx, yy, xx0, yy0;
	int quit;
	uint32 vecCount;

#ifdef FSCFG_REENTRANT
	struct odLocalData thisLocalOD;
	struct odLocalData* pLocalOD = &thisLocalOD;
#endif 

	if (Fx.fx_pen_descriptor == 0)
	{
		return (NO_ERR);
	}

	set_pattern(Fx.fx_stroke_pattern);

	if (Fx.fx_pattern_processor == 0)
	{
		return (NO_ERR);
	}

	{
		// When banding, determine the range of y values of points we need to stroke.

		int32 ySize, yOffset, tmp;

		ySize = Fx.fx_pen_descriptor->nYSize;
		yOffset = Fx.fx_pen_descriptor->nYOffset;

		// Adjust lowRow

		tmp = yOffset;
		if (ySize < 0)
		{
			tmp -= ySize;
		}
		if (tmp < 0)
		{
			tmp = 0;
		}
		LocalOD.lowRow = lowBand - tmp;

		// Adjust highRow

		tmp = -(yOffset + 1);
		if (ySize > 0)
		{
			tmp += ySize;
		}
		if (tmp < 0)
		{
			tmp = 0;
		}
		LocalOD.highRow = highBand + tmp;
	}

	if (glyphPtr->nc == 0)
	{
		return (NO_ERR);
	}

	p = scPtr;

	LocalOD.hreps = 0;

	for (ctr = 0; ctr < glyphPtr->nc; ctr++)
	{
		LocalOD.lastw  = 0;
		LocalOD.startw = 0;

		x0p = xp = p->xPoints;
		y0p = yp = p->yPoints;
		startPt = i = glyphPtr->sp[ctr];
		endPt = glyphPtr->ep[ctr];

		if (startPt == endPt) continue;

		quit = 0;
		vecCount = 1;

		if (onCurve[i] & ONCURVE) 
		{
			*xp++ = xx = x[i];
			*yp++ = yy = y[i++];
		}

		else 
		{
			if (onCurve[endPt] & ONCURVE) 
			{
				startPt = endPt--;
				*xp++ = xx = x[startPt];
				*yp++ = yy = y[startPt];
			}

			else 
			{
				*xp++ = xx = (F26Dot6)(((long) x[i] + x[endPt] + 1) >> 1);
				*yp++ = yy = (F26Dot6)(((long) y[i] + y[endPt] + 1) >> 1);
				goto Offcurve;
			}
		}

		while (true) 
		{
			while (onCurve[i] & ONCURVE)
			{
				if (++vecCount > MAXVECTORS)
				{
					// Ran out of local memory. Consume data and continue.
					if (!od_mark(ODA x0p, y0p, x0p+1, y0p+1, yp-1))
					{
						return (SCAN_ERR);
					}

					// save data in points 0 and 1 for final
					x0p = p->xPoints+2;
					y0p = p->yPoints+2;

					// save last vector to be future previous vector
					*x0p++ = *(xp-2);
					*x0p++ = *(xp-1);
					*y0p++ = *(yp-2);
					*y0p++ = *(yp-1);

					// start next processing with last vector
					xp       = x0p;
					x0p      = p->xPoints+2;
					yp       = y0p;
					y0p      = p->yPoints+2;
					vecCount = 5;
				}

				*xp++ = xx = x[i];
				*yp++ = yy = y[i];

				if (quit) 
				{
					goto sc_exit;
				}

				else
				{
					i = (i == endPt) ? quit = 1, startPt : i+1;
				}
	      }

			do 
			{
Offcurve:               
				xx0 = xx;
				yy0 = yy;

				j = i;
				nextPt = (i == endPt) ? quit = 1, startPt : i+1;

				if (onCurve[nextPt] & ONCURVE) 
				{
					xx = x[nextPt];
					yy = y[nextPt];
					i = nextPt;
				}

				else
				{
					xx = (F26Dot6) (((long) x[i] + x[nextPt] + 1) >> 1);
					yy = (F26Dot6) (((long) y[i] + y[nextPt] + 1) >> 1);
				}

				if (sc_DrawParabola(xx0, yy0, x[j], y[j], xx, yy, &xp, &yp, &vecCount, -1L))
				{
					/* not enough room to create parabola vectors */
					if (!od_mark(ODA x0p, y0p, x0p+1, y0p+1, yp-1))
					{
						return (SCAN_ERR);
					}

					x0p = p->xPoints+2;
					y0p = p->yPoints+2;

					*x0p++ = *(xp-2);
					*x0p++ = *(xp-1);
					*y0p++ = *(yp-2);
					*y0p++ = *(yp-1);

					xp       = x0p;
					x0p      = p->xPoints+2;
					yp       = y0p;
					y0p      = p->yPoints+2;
					vecCount = 5;

					// recaptured some memory, try again, if still wont work, MAXVEC is too small
					if (sc_DrawParabola(xx0, yy0, x[j], y[j], xx, yy, &xp, &yp, &vecCount, -1L))
					{
						return (SCAN_ERR);
					}
				}

				if (quit) 
				{
					goto sc_exit;
				}

				else 
				{
					i = (i == endPt) ? quit = 1, startPt : i+1;
				}
			} while (!(onCurve[i] & ONCURVE));
		}

sc_exit:

		if ((!od_mark(ODA x0p, y0p, x0p+1, y0p+1, yp-1))
		 || (!od_mark(ODA xp-2, yp-2, p->xPoints, p->yPoints, p->yPoints+1)))
		{
			return (SCAN_ERR);
		}

		if (LocalOD.startw != 0)
		{
			StrokePoint(ODA LocalOD.startx, LocalOD.starty, LocalOD.startw);
		}
	}

	return (NO_ERR);
}


// 4/4/90
//    Version that distinguishes between On transitions and Off transitions.
// 3/23/90
//    A procedure to find and mark all of the scan lines (both row and
// column) that are crossed by a vector. Many different cases must be
// considered according to the direction of the vector, whether it is
// vertical or slanted, etc. In each case, the vector is first examined
// to see if it starts on a scan-line. If so, special markings are made
// and the starting conditions are adjusted. If the vector ends on a
// scan line, the ending conditions must be adjusted. Then the body
// of the case is done.
//    Special adjustments must be made when a vector starts or ends
// on a scan line. Whenever one vector starts on a scan line, the
// previous vector must have ended on a scan line. Generally, this
// should result in the line being marked as crossed only once
// (conceptually by the vector that starts on the line. But, if the
// two lines form a vertex that includes the vertex in a colored region,
// the line should be marked twice. If the vertex is also on a
// perpendicular scan line, the marked scan line should be marked once
// on each side of the perpendicular line. If the vertex defines a
// point that is jutting into a colored region, then the line should not
// be marked at all. In order to make these vertex crossing decisions,
// the previous vector must be examined.
//
//    Because many vectors are short with respect to the grid for small
// resolutions, the procedure first looks for simple cases in which no
// lines are crossed.
//
//    xb, x0, and x1 are x coordinates of previous point, current point
// and next point similaryly yb, y0 and y1.

#define DROUND(a)			((a+HALFM) & INTPART)
#define RSH(a)				(a>>PIXSHIFT)
#define LSH(a)				(a<<PIXSHIFT)
#define LINE(a)			(!((a & FRACPART) ^ HALF))
#define BETWEEN(a,b,c)	(a < b && b < c )
#define EQUAL(a,b,c)		(a == b && b == c) 
#define EPSILON			0x1

#define F_Q1				0x0001
#define F_Q2				0x0002
#define F_Q3				0x0004
#define F_Q4				0x0008
#define F_INTERIOR		0x0010  
#define F_V_LINEAR		0x0020
#define F_H_LINEAR		0x0040
#define QUADRANT_BITS	(F_Q1|F_Q2|F_Q3|F_Q4)

// Line orientation macros.

#define TOP_TO_BOTTOM	BETWEEN(y1,y0,yb)          
#define BOTTOM_TO_TOP	BETWEEN(yb,y0,y1)
#define LEFT_TO_RIGHT	BETWEEN(xb,x0,x1)
#define RIGHT_TO_LEFT	BETWEEN(x1,x0,xb)
#define INTERIOR			(SCMR_Flags & F_INTERIOR)
#define HORIZ				(SCMR_Flags & F_V_LINEAR)
#define VERT				(SCMR_Flags & F_H_LINEAR)
#define QUAD_1OR2			(SCMR_Flags & (F_Q1|F_Q2))
#define QUAD_3OR4			(SCMR_Flags & (F_Q3|F_Q4))
#define QUAD_2OR3			(SCMR_Flags & (F_Q2|F_Q3))
#define QUAD_1OR4			(SCMR_Flags & (F_Q1|F_Q4))
#define QUAD_1				(SCMR_Flags & F_Q1)
#define QUAD_2				(SCMR_Flags & F_Q2)
#define QUAD_3				(SCMR_Flags & F_Q3)
#define QUAD_4				(SCMR_Flags & F_Q4)

PRIVATE int od_mark(ODP F26Dot6 *pntbx, F26Dot6 *pntby, F26Dot6 *pntx, F26Dot6 *pnty, F26Dot6 *pnte)
{
	int16 onrow, oncol, Shift;
	F26Dot6  x0, y0, x1, y1, xb, yb, rx0, ry0, rx1, ry1, dy, dx; 
	int32 rhi, rlo;

	// Set up a flag byte and the equates for it.

	register int SCMR_Flags;

	// Loop through all the points in the contour.

	x0 = *pntbx;
	y0 = *pntby;
	x1 = *pntx++;
	y1 = *pnty++;

	// Inside the loop, we will set (xb,yb) to (x0,y0) and (x0,y0) to (x1,y1).
	// For the very first pass through the loop, if (xb,yb) = (x0,y0) and
	// (LINE y0) is true we will run into a degenerate case where we will have
	// two instead of 3 distinct points. Detect and fail this case.

	if ((x0 == x1) && (y0 == y1) && (LINE(y1)))
	{
		return (0);
	}

	while (pnty <= pnte)
	{
		// Get the next set of points.

		xb = x0;
		yb = y0;
		x0 = x1;
		y0 = y1;
		x1 = *pntx++;
		y1 = *pnty++;

		// Scan convert this line.

		SCMR_Flags = 0;
		dy = y1-y0;
		dx = x1-x0;

		if (!dy && !dx)
		{
			// We have a point and will ignore this. However, for the next 
			// iteration we will not update (xb,yb) so that we don't get 
			// (xb,yb) = (x0,y0) and end up being with 2 instead of 3 
			// distinct points.

			x0 = xb;
			y0 = yb;

			continue;
		}

		rx0          = DROUND(x0);
		LocalOD.x    = RSH(rx0);
		ry0          = DROUND(y0);
		LocalOD.y    = RSH(ry0);
		rx1          = DROUND(x1);
		LocalOD.endx = RSH(rx1);
		ry1          = DROUND(y1);
		LocalOD.endy = RSH(ry1);
		onrow        = false;
		oncol        = false;

		// Set up the quadrant that the line is in.

		     if (dx >  0 && dy >= 0)			SCMR_Flags |= F_Q1;
		else if (dx <= 0 && dy >  0)			SCMR_Flags |= F_Q2;
		else if (dx <  0 && dy <= 0)			SCMR_Flags |= F_Q3;
		else											SCMR_Flags |= F_Q4;
    
		// For platforms where we do banding, we will set onrow and oncol only
		// if the starting point is in the band.

		// 8/16/92 - FF
		//
		// This was messing up banding with shadows. Taking it out seems to
		// fix things.
#if 0
		if ((QUAD_1OR2 && (LocalOD.y >= LocalOD.lowRow))
		 || (QUAD_3OR4 && (LocalOD.y <= LocalOD.highRow)))
#endif
		{
			if (LINE(y0)) onrow = true;
			if (LINE(x0)) oncol = true;
		}

		// For the platforms where we do banding, find out if the band totally
		// excludes the current line.

		if ((QUAD_1OR2 && ((LocalOD.y > LocalOD.highRow+1) || (LocalOD.endy < LocalOD.lowRow-1)))
		 || (QUAD_3OR4 && ((LocalOD.y < LocalOD.lowRow-1)  || (LocalOD.endy > LocalOD.highRow+1))))
		{
			LocalOD.lastw = 0;
			continue;
		}

		// Compute some other flags.

		if ((long)(x0-xb)*dy < (long)(y0-yb)*dx) SCMR_Flags |= F_INTERIOR;
		if (EQUAL (yb, y0, y1))                  SCMR_Flags |= F_V_LINEAR;
		if (EQUAL (xb, x0, x1))                  SCMR_Flags |= F_H_LINEAR;
    
		// Now handle the cases where the starting point falls on a row
		// scan line and maybe also on a coloumn scan line.
		//
		// First consider the intersections with row scan lines only. After
		// this we will consider the intersections with column scan lines.
		// It is not worth while to set the column intersections yet, because
		// in any case we will have to set the intersections when a vertex
		// does not lie on a row but does lie on a column.

		Shift = 0;

		if (onrow) 
		{
			if (oncol) Shift = 1;

			if ((INTERIOR || VERT) && (((yb > y0) && QUAD_1OR2) || ((yb < y0) && QUAD_3OR4)))
			{
				Y_POINT_ON(LocalOD.x);
				Y_POINT_OFF(LocalOD.x+Shift);
			}

			else if ((INTERIOR && QUAD_1OR2) || BOTTOM_TO_TOP || (HORIZ && (xb > x0) && QUAD_1))
			{	
				Y_POINT_ON(LocalOD.x);
			}
			
			else if ((INTERIOR && QUAD_3OR4) || TOP_TO_BOTTOM || (HORIZ && (x0 > xb) && QUAD_3))
			{
				Y_POINT_OFF(LocalOD.x+Shift);
			}
		}

		// Ignore horizontal lines.

		if (LocalOD.endy == LocalOD.y)
		{
			continue;
		}

		// Now handle vertical lines.

		if (LocalOD.endx == LocalOD.x)
		{
			if (QUAD_1OR2)
			{
				if (onrow)
				{
					++LocalOD.y;
				}

				while (LocalOD.y < LocalOD.endy)	/* note oncol can't be true */
				{
  					Y_POINT_ON(LocalOD.x);
					++LocalOD.y;
				}

				continue;
			}

			else
			{
				if (QUAD_4 && oncol)
				{
					++LocalOD.x;
				}

				if LINE(y1)
				{
					++LocalOD.endy;
				}

				--LocalOD.y;

				while (LocalOD.y >= LocalOD.endy)
				{
  					Y_POINT_OFF(LocalOD.x);
					--LocalOD.y;
				}

				continue;
			}
		}

		// Set up the initial conditions for the DDA.

		if (QUAD_1OR2)	// 1st and 2nd quadrants
		{
			LocalOD.incY = LSH(dy);

			if (!onrow)
			{
				rhi = multlong((ry0 - y0 + HALF), dx);
			}

			else
			{
				rhi = LSH(dx);
				++LocalOD.y;
			}

			if (QUAD_1)	// 1st quadrant
			{
				LocalOD.incX = LSH(dx);

				if (!oncol)
				{
					rlo = multlong((rx0 - x0 + HALF), dy);
				}

				else
				{
					rlo = LocalOD.incY;
					++LocalOD.x;
				}

				LocalOD.r = rhi - rlo;
			}

			else	// 2nd quadrant
			{
				LocalOD.incX = -LSH(dx);

				if (!oncol)
				{
					rlo = multlong((rx0 - x0 - HALF), dy);
				}

				else
				{
					rlo = -LocalOD.incY;
				}

				if LINE(x1)
				{
					++LocalOD.endx;
				}

				LocalOD.r = rlo - rhi + EPSILON;
			}
		}

		else	// 3rd and 4th quadrants
		{
			LocalOD.incY = -LSH(dy);

			if (!onrow)
			{
				rhi = multlong((ry0 - y0 - HALF), dx);
			}

			else
			{
				rhi = -LSH(dx);
			}

			if (QUAD_3)	// 3rd quadrant
			{
				LocalOD.incX = -LSH(dx);

				if (!oncol)
				{
					rlo = multlong((rx0 - x0 - HALF), dy);
				}

				else
				{
					rlo = LocalOD.incY;
				}

				if LINE(y1)
				{
					++LocalOD.endy;
				}

				if LINE(x1)
				{
					++LocalOD.endx;
				}

				LocalOD.r = rhi - rlo;
			}

			else // 4th quadrant
			{
				LocalOD.incX = LSH(dx);

				if (!oncol)
				{
					rlo = multlong((rx0 - x0 + HALF), dy);
				}

				else
				{
					rlo = -LocalOD.incY;
					LocalOD.x++;
				}

				if LINE(y1)
				{
					++LocalOD.endy;
				}

				LocalOD.r = rlo - rhi + EPSILON;
			}
		}

		for (;;)
		{
			int16 ix;

			ix = (QUAD_1OR4) ? 1 : -1;

			if (LocalOD.r > 0)
			{
				if (LocalOD.x == LocalOD.endx) break;

				LocalOD.x += ix;
				LocalOD.r -= LocalOD.incY;
			}

			else
			{
				if (LocalOD.y == LocalOD.endy) break;

				if (QUAD_1OR2)
				{
					Y_POINT_ON(LocalOD.x);
					LocalOD.y++;
				}

				else
				{
					LocalOD.y--;
					Y_POINT_OFF(LocalOD.x);
				}

				LocalOD.r += LocalOD.incX;
			}           
		}

		// Go on to the next line.
	}

	return (1);
}

#undef  DROUND
#undef  RSH
#undef  LSH
#undef  LINE
#undef  BETWEEN
#undef  INTERIOR
#undef  TOP_TO_BOTTOM  
#undef  BOTTOM_TO_TOP  
#undef  LEFT_TO_RIGHT
#undef  RIGHT_TO_LEFT

extern void fill_black_rectangle(int32 x, int32 y, int32 l, int32 nrows);
extern void fill_white_rectangle(int32 x, int32 y, int32 l, int32 nrows);

void StrokePoint(ODP int32 x, int32 y, int32 w)
{
	OutlineDescriptor* p;

	p = Fx.fx_pen_descriptor;

	// Stroke the current pen onto the bitmap at the
	// current point.

	switch (p->nType)
	{
		case OUTLINE_region:
		{
			uint8* r;
			int32 x0;
			int32 y0;
			uint32 n;

			n = p->nYSize;
			r = (uint8*)(((char*)p)+sizeof(Outline));

			x0 = x-p->nXOffset;
			y0 = y+p->nYOffset;

			while (n-- > 0)
			{
				int32 x1, x2;

				x1 = (uint32)*(r++);
				x2 = (uint32)*(r++);

				fill_pattern_run(x0+x1, y0, x2-x1+1 + HREPS);

				y0--;
			}

			break;
		}

		case OUTLINE_rectangle:
		{
			int32 x0;
			int32 y0;
			int32 n;

			n = p->nYSize;

			x0 = x-p->nXOffset;
			y0 = y+p->nYOffset;

			// Optimization added 12/8/92 MH, speeds up contour pens a lot.

			// clip high
			if (y0 >= Fx.fx_high_row)
			{
				n -= (y0 - Fx.fx_high_row + 1);
				y0 = Fx.fx_high_row - 1;
			}

			// clip low
			if (y0 - (int16)n + 1 < Fx.fx_low_row)
			{
				n = y0 - Fx.fx_low_row + 1;
			}

			if (y0 < Fx.fx_high_row && n > 0)
			{
				// Do the fill
				if (Fx.fx_stroke_pattern == 0)
				{
					fill_black_rectangle(x0, y0, p->nXSize + HREPS, n);
				}
				else
				{
					fill_white_rectangle(x0, y0, p->nXSize + HREPS, n);
				}
			}
			break;
		}

		case OUTLINE_diagonal:
		{
			int32 x0, y0;
			int32 dx, dy;
			int32 ix, iy;

			x0 = x-p->nXOffset;
			y0 = y+p->nYOffset;

			dx =  p->nXSize;
			dy = -p->nYSize;

			ix = 1;
			iy = 1;

			if (dx < 0)
			{
				dx = -dx;
				ix = -1;
			}

			if (dy < 0)
			{
				dy = -dy;
				iy = -1;
			}

			if (dx > dy)
			{
				int32 f;
				int32 n;
				int32 x1;

				f  = 0;
				n  = dx;
				x1 = x0;

				while (--n >= 0)
				{
					f += dy;

					if (f >= dx)
					{
						if (x0 < x1)
						{
							fill_pattern_run(x0, y0, x1-x0+1 + HREPS);
						}
						
						else
						{
							fill_pattern_run(x1, y0, x0-x1+1 + HREPS);
						}

						f  -= dx;
						x0  = x1;
						y0 += iy;
					}

					if (n != 0)
					{
						x1 += ix;
					}
				}
				if (x0 != x1)
				{
					if (x0 < x1)
					{
						fill_pattern_run(x0, y0, x1-x0+1 + HREPS);
					}
					
					else
					{
						fill_pattern_run(x1, y0, x0-x1+1 + HREPS);
					}
				}
			}

			else
			{
				int32 f;
				int32 n;
				int32 x1;

				f  = 0;
				n  = dy;
				x1 = x0;

				while (--n >= 0)
				{
					f += dx;

					if (f >= dy)
					{
						f  -= dy;

						if (n != 0)
						{
							x1 += ix;
						}
					}

					if (x0 <= x1)
					{
						fill_pattern_run(x0, y0, x1-x0+1 + HREPS);
					}

					else
					{
						fill_pattern_run(x1, y0, x0-x1+1 + HREPS);
					}

					x0  = x1;
					y0 += iy;
				}
			}

			break;
		}
	}

	// Generate any horizontal points needed to connect this point
	// with the previous point.

	if (w != 0)
	{
		// Make sure there is a previous point.

		if (LocalOD.lastw != 0)
		{
			int32 dx;
			int32 ix;

			// Compute the length and direction of the horizontal run.

			dx = x-LocalOD.lastx;
			ix = 1;

			if (dx < 0)
			{
				dx = -dx;
				ix = -1;
			}

			// Only generate a horizontal of points if there is a horizontal
			// gap between the current point and the last (dx > 1).

			if (dx > 1)
			{
				int32 x0;
				int32 y0;

				x0 = LocalOD.lastx;

				// Compute the Y coordinate of the run. This must always be
				// in the interior of the glyph.

				y0 = y;

				if (w == ix)
				{
					if (LocalOD.lastw != w)
					{
						y0 -= ix;
					}

					else
					{
						y0 = LocalOD.lasty;
					}
				}

				// Stroke the horizontal run.
				if (dx > 1)
				{
					if (ix > 0)
					{
						x0 += ix;
					}
					else
					{
						x0 += ((dx - 1) * ix);
					}

					LocalOD.hreps = dx - 2;
					X_POINT(x0, y0);
					LocalOD.hreps = 0;
				}
			}
		}

		// Save the current point as the previous point (and the
		// first point if it has not yet been set.)

		if (LocalOD.startw == 0)
		{
			LocalOD.startx = x;
			LocalOD.starty = y;
			LocalOD.startw = w;
		}

		LocalOD.lastx = x;
		LocalOD.lasty = y;
		LocalOD.lastw = w;
	}
}
