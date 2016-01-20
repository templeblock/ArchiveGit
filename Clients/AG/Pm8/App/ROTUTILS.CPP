/*
//	ROTUTILS.CPP
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
// Rotation utility routines.
//
// $Header: /PM8/App/ROTUTILS.CPP 1     3/03/99 6:11p Gbeddow $
//
// $Log: /PM8/App/ROTUTILS.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     11/19/97 3:43p Jayn
// Rotated bitmap support.
// 
//    Rev 1.1   17 Sep 1997 09:12:34   Jay
// Bitmap rotation
// 
//    Rev 1.0   14 Aug 1997 15:25:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:18   Fred
// Initial revision.
// 
//    Rev 1.1   01 Nov 1996 17:11:48   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:47:08   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:08   FRED
// Initial revision.
// 
//    Rev 2.1   26 Jun 1995 10:08:32   JAY
// Got rid of doubles. Now uses CFixed.
// 
//    Rev 2.0   07 Feb 1995 15:48:06   JAY
// Initial revision.
// 
//    Rev 1.2   30 Nov 1994 16:33:42   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.1   04 Aug 1994 07:32:48   JAY
// Large model and 32-bit changes
//
//   Rev 1.0   16 Dec 1993 16:25:20   JAY
// 
*/

#include "stdafx.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "pagedefs.h"
#include "utils.h"
#include "rotutils.h"

static void near skew_line(LPBYTE pRow, int bytes, int offset)
{
	if (offset > 0)
	{
	/* Right skew. */

		int kept_bytes = bytes-offset;

		if (kept_bytes > 0)
		{
			memmove(pRow+offset, pRow, kept_bytes);	// Copy what we want
			memset(pRow, 0, offset);						// Zero the rest
		}
		else
		{
			memset(pRow, 0, bytes);						// Zero all
		}
	}
	else if (offset < 0)
	{
	/* Left skew. */

		int kept_bytes = bytes+offset;

		if (kept_bytes > 0)
		{
			memmove(pRow, pRow-offset, kept_bytes);	// Copy what we want
			memset(pRow+kept_bytes, 0, -offset);		// Zero the rest
		}
		else
		{
			memset(pRow, 0, bytes);						// Zero all
		}
	}
}

/*
// Column version of memmove.
// This must handle overlapping moves.
*/

static void near move_column(LPBYTE pDst, LPBYTE pSrc, int count, int bytes_per_row)
{
	if (pDst > pSrc)
	{
		UINT offset = (count-1)*bytes_per_row;
	/* Reverse direction for overlapping copy. */
		pDst += offset;
		pSrc += offset;
		bytes_per_row = -bytes_per_row;
	}

	for (int i = 0; i < count; i++)
	{
		*pDst = *pSrc;
		pDst += bytes_per_row;
		pSrc += bytes_per_row;
	}
}

/*
// Column version of memset.
*/

static void near set_column(LPBYTE pRow, int value, int count, int bytes_per_row)
{
	for (int i = 0; i < count; i++, pRow += bytes_per_row)
	{
		*pRow = (BYTE)value;
	}
}

static void near skew_column(LPBYTE pRow, int height, int bytes_per_row, int offset)
{
	if (offset > 0)
	{
	/* Down skew. */

		int kept_bytes = height-offset;

		if (kept_bytes > 0)
		{
			move_column(pRow + offset*bytes_per_row, pRow, kept_bytes, bytes_per_row);
			set_column(pRow, 0, offset, bytes_per_row);
		}
		else
		{
			set_column(pRow, 0, height, bytes_per_row);
		}
	}
	else if (offset < 0)
	{
	/* Up skew. */

		int kept_bytes = height+offset;

		if (kept_bytes > 0)
		{
			move_column(pRow, pRow-offset*bytes_per_row, kept_bytes, bytes_per_row);
			set_column(pRow+kept_bytes*bytes_per_row, 0, -offset, bytes_per_row);
		}
		else
		{
			set_column(pRow, 0, height, bytes_per_row);
		}
	}
}

/*
// Skew a number (with rounding).
*/

int skew_number(int n, double dSkew)
{
	double d = dSkew*n;

	if (d < 0)
	{
		d -= 0.5;
	}
	else
	{
		d += 0.5;
	}
	return (int)d;
}

/*
// Skew in the x direction.
*/

void skew_x(ROTATE_MAP_PTR map, double dSkew, int center_y)
{
/* We will skew about the center. */

	int bytes_per_pixel = map->bytes_per_pixel;
	int height = map->height;
	int bytes_per_row = map->bytes_per_row;
	LPBYTE pRow;
	int y;

/* Compute endpoint skew values. */

	int start = skew_number(center_y-0, dSkew);
	int end = skew_number(center_y-height, dSkew);

/* Compute source and destination ranges. */

	int src_factor = height;			// Source
	int dst_factor = end-start;		// Destination

/* Compute the rest of the DDA variables. */

	int inc = 1;
	if (dst_factor < 0)
	{
		dst_factor = -dst_factor;
		inc = -1;
	}

	int fraction;
	int value;

	if (map->bound.top == 0)
	{
		fraction = 0;
		value = start;
	}
	else
	{
		value = scale_number(map->bound.top, dst_factor, src_factor);

		if (inc < 0)
		{
			value = -value;
		}

		fraction = mulrem(map->bound.top, dst_factor, src_factor);

		value += start;
	}

//	od("X s:%d e:%d v:%d f:%d; ", start, end, value, fraction);

	int dx = (map->bound.right-map->bound.left)*bytes_per_pixel;
	int  dy = map->bound.bottom-map->bound.top;

	for (pRow = map->data, y = 0; y < dy; y++, pRow += bytes_per_row)
	{
		skew_line(pRow, dx, value*bytes_per_pixel);

	/* Update the DDA. */

		fraction += dst_factor;
		while (fraction >= src_factor)
		{
			fraction -= src_factor;
			value += inc;
		}
	}
}

/*
// Skew in the y direction.
*/

void skew_y(ROTATE_MAP_PTR map, double dSkew, int center_x, int x_offset)
{
/* We will skew about the center. */

	int width = map->width;
	int bytes_per_row = map->bytes_per_row;
	int bytes_per_pixel = map->bytes_per_pixel;

	LPBYTE pColumn;
	int x;

/* Compute endpoint skew values. */

	int start = skew_number(center_x-0, dSkew);
	int end = skew_number(center_x-width, dSkew);

/* Compute source and destination ranges. */

	int src_factor = width;			// Source
	int dst_factor = end-start;		// Destination

/* Compute the rest of the DDA variables. */

	int inc = 1;
	if (dst_factor < 0)
	{
		dst_factor = -dst_factor;
		inc = -1;
	}

/* Handle an offset rotation if necessary. */

	if (x_offset != 0)
	{
		start -= skew_number(x_offset, dSkew);
	}

/* Compute DDA value and fraction. */

	int fraction;
	int value;

	if (map->bound.left == 0)
	{
		fraction = 0;
		value = start;
	}
	else
	{
		value = scale_number(map->bound.left, dst_factor, src_factor);

		if (inc < 0)
		{
			value = -value;
		}

		fraction = mulrem(map->bound.left, dst_factor, src_factor);

		value += start;
	}

//	od("Y s:%d e:%d v:%d f:%d; ", start, end, value, fraction);

	int dx = map->bound.right-map->bound.left;
	int dy = map->bound.bottom-map->bound.top;

	for (pColumn = map->data, x = 0; x < dx; x++)
	{
		skew_column(pColumn++, dy, bytes_per_row, value);

		if (bytes_per_pixel == 3)
		{
			skew_column(pColumn++, dy, bytes_per_row, value);
			skew_column(pColumn++, dy, bytes_per_row, value);
		}

	/* Update the DDA. */

		fraction += dst_factor;
		while (fraction >= src_factor)
		{
			fraction -= src_factor;
			value += inc;
		}
	}
}

/*
// Rotate the bitmap.
*/

void rotate_it(ROTATE_MAP_PTR map, ROTATE_PARMS_PTR parms)
{
/*
// We do three skews.
//
// 1. Skew x by Alpha.
// 2. Skew y by Beta.
// 3. Skew x by Alpha.
*/

	double dAlpha = -tan(parms->angle/2);
	double dBeta = sin(parms->angle);

	skew_x(map, dAlpha, parms->center.y);
	skew_y(map, dBeta, parms->center.x, 0);
	skew_x(map, dAlpha, parms->center.y);
}

void rotate_it_offset(ROTATE_MAP_PTR map, ROTATE_PARMS_PTR parms, int offset_center_x)
{
	POINT center = parms->center;
	double dAlpha = -tan(parms->angle/2);
	double dBeta = sin(parms->angle);

/*
// We do three skews.
//
// 1. Skew x by Alpha.
// 2. Skew y by Beta.
// 3. Skew x by Alpha.
*/

	skew_x(map, dAlpha, center.y);
	skew_y(map, dBeta, center.x, center.x-offset_center_x);

/* Compute the offset y center from the offset x center. */

	center.y -= skew_number(center.x-offset_center_x, dBeta);

	skew_x(map, dAlpha, center.y);
//	od("\r\n");
}
