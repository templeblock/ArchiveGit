// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "mclib.h"
#include "routines.h"


/************************************************************************/
static BOOL fill_filt_buf (filt_buf, yline, filter_size, cur_off)
/************************************************************************/
LPPTR filt_buf;
int yline, filter_size;
LPINT cur_off;
{
    int pix_off, ystart, yend, i, y;

    pix_off = filter_size / 2;
    ystart = yline - pix_off;
    ystart = bound (ystart, 0, lpImage->nlin);
    if ((ystart == yline) || (yline < 0)) {
	*cur_off = 0;
	return (TRUE);
    }
    yend = yline + pix_off;
    yend = bound (yend, 0, lpImage->nlin);
    for (y = ystart, i = 0; y <= yend; ++y, ++i) {
	if (y == yline)
	    *cur_off = i;
	if (!readimage (y, 0, lpImage->npix-1, lpImage->npix, filt_buf [i])) {
	    Print ("error reading line %d", y);
	    return (FALSE);
	}
    }
}


/************************************************************************/
static VOID BlendOr (src1, src2, dst, xstart, xend)
/************************************************************************/
LPTR src1, src2, dst;
int xstart, xend;
{
    int count;

    src1 += xstart;
    src2 += xstart;
    dst  += xstart;
    count = xend - xstart + 1;
    while (count--)
	*dst++ |= (*src1++ | *src2++);
}

/************************************************************************/
LPPOINT BlendBuildOutline (LPMASK lpMask, int *nPoints, int xmin, int xmax)
/************************************************************************/
{
    LPPOINT	lpOutline;
    LPPOINT	lpPntIn, lpPntInEnd, lpPntOut;
    POINT	pnt;
    int		i, j, pass;
    int		x1, y1, x2, y2;

/* build list of points on the outline */
    for (pass = 1; pass < 3; pass++) {
	if (pass == 2) {

	/* Allocate the space for the outline (sorted) */
	    lpOutline = (LPPOINT) Alloc ((long)sizeof(POINT) * (*nPoints));
	    if (!lpOutline)
		return (lpOutline);

	    lpPntOut = lpOutline;
	}
	*nPoints = 0;
	lpPntIn = lpMask->lpMarquee;
	lpPntInEnd = lpPntIn + lpMask->nMarqueePoints;
	while (lpPntIn < lpPntInEnd) {

	    /* Extract the x and y values to draw */
	    x1 = lpPntIn->x;
	    y1 = lpPntIn->y;
	    lpPntIn++;
	    x2 = lpPntIn->x;
	    y2 = lpPntIn->y;

	    /* skip over "pen up" markers */
	    if ((x2 == PENUP_MARKER) && (y2 == PENUP_MARKER)) {
		lpPntIn++;
		continue;
	    }

	/* vertical line */
	    if (x1 == x2) {

	    /* discard points to the left and right of the image */
		if ((lpPntIn->x < xmin) || (xmax < lpPntIn->x))
		    continue;

		if (y1 > y2) {
		    j = y1;
		    y1 = y2;
		    y2 = j;
		}
		pnt.x = x1;
		for (pnt.y = y1 ; pnt.y < y2; pnt.y++) {
		    if (pass == 2) {
			if ((lpPntOut->x == pnt.x) && (lpPntOut->y == pnt.y))
			    continue;

			*lpPntOut++ = pnt;
		    }
		    (*nPoints)++;
		}
	    }

	/* horizontal line */
	    else if (y1 == y2) {
		if (x1 > x2) {
		    j = x1;
		    x1 = x2;
		    x2 = j;
		}
		pnt.y = y1;
		for (pnt.x = x1; pnt.x < x2; pnt.x++) {
		    if ((pnt.x < xmin) || (xmax < pnt.x))
			continue;
		    if (pass == 2) {
			if ((lpPntOut->x == pnt.x) && (lpPntOut->y == pnt.y))
			    continue;

			*lpPntOut++ = pnt;
		    }
		    (*nPoints)++;
		}
	    }
	}
    }

/* sort the points */
    qsort ((LPTR)lpOutline, (long)*nPoints, sizeof(POINT), CompareEdges);

    return (lpOutline);
}


#define MAX_FILTER_SIZE 3
/************************************************************************/
BOOL DoBlend ()
/************************************************************************/
{
    int		x, y, i, j, ystart, yend;
    int		x1, y1;
    LPTR	filt_buf [MAX_FILTER_SIZE];
    LPTR	out_buf, mbuffer, cbuffer, pbuffer;
    LPTR	tlptr;
    int		filter_size, fsize;
    LPFRAME	lpFrame;
    LPMASK	lpMask;
    int		pix_off, cur_off, new_off, diff_off, max_off, xstart, xend;
    RECT	rMask;
    long	sval;
    int		nPoints;
    LPPOINT	lpOutline, lpOutlineCurr, lpOutlineEnd;

    lpFrame = frame_set (NULL);
    if (! (lpMask = lpFrame->WriteMask))
	return (FALSE);
    if (!lpMask->lpMarquee)
	return (FALSE);

    AstralCursor( IDC_WAIT );
    AstralClockCursor (1, 100);

    Mask.RepairRect = lpMask->rMask;
    filter_size = MAX_FILTER_SIZE;
    Mask.BlendPressure = IDC_PRESSHEAVY;

    out_buf = LineBuffer [0];
    for (i = 0; i < filter_size; i++)
	filt_buf [i] = NULL;
    mbuffer = cbuffer = pbuffer = NULL;
    lpOutline = NULL;

    for (i = 0; i < filter_size; ++i) {
	if ( !AllocLines (&filt_buf [i], 1, (long)lpImage->npix, DEPTH)) {
	    goto errorMemory;
	}
    }

    xstart = Mask.RepairRect.left - 2;
    xstart = bound (xstart, 0, lpImage->npix-1);
    xend = Mask.RepairRect.right + 1;
    xend = bound (xend, 0, lpImage->npix-1);
    ystart = Mask.RepairRect.top - 1;
    ystart = bound (ystart, 0, lpImage->nlin-1);
    yend = Mask.RepairRect.bottom + 1;
    yend = bound (yend, 0, lpImage->nlin-1);

    rMask.left = xstart;
    rMask.right = xend;
    rMask.top = ystart;
    rMask.bottom = yend;
    AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &rMask);

/* sort the points */
    lpOutline = BlendBuildOutline (lpMask, &nPoints, xstart, xend);

    lpOutlineCurr = lpOutline;
    lpOutlineEnd  = lpOutline + nPoints - 1;

    if (!fill_filt_buf (filt_buf, ystart, filter_size, &cur_off)) {
	goto error;
    }
    pix_off = filter_size / 2;

    if (!(cbuffer = Alloc((long)lpImage->npix)))
	goto error;
    if (!(mbuffer = Alloc((long)lpImage->npix)))
	goto error;
    if (!(pbuffer = Alloc((long)lpImage->npix)))
	goto error;

    clr (pbuffer, lpImage->npix);
    clr (cbuffer, lpImage->npix);

    for (y = ystart; y <= yend;) {
	AstralClockCursor (y-ystart, yend-ystart);
	if (cur_off > 0) {
	    clr (mbuffer, lpImage->npix);
	    while (lpOutlineCurr <= lpOutlineEnd) {
		if (lpOutlineCurr->y > y+1)
		    break;
		if (lpOutlineCurr->y == y+1) {
		    x1 = lpOutlineCurr->x;
		    mbuffer [x1] = 255;
		    x1--;
		    if (0 <= x1)
			mbuffer [x1] = 255;
		    x1 += 2;
		    if (x1 <= xend)
			mbuffer [x1] = 255;
		}
		lpOutlineCurr++;
	    }

	    BlendOr (mbuffer, cbuffer, pbuffer, xstart, xend);

	    for (x = xstart; x <= xend; ++x) {
		if (pbuffer [x]) {
		    fsize = (2 * cur_off) + 1;
		    if ((y - cur_off) < 0)
			fsize = min (fsize, (y * 2) + 1);

		    if ((y + cur_off) >= lpImage->nlin)
			fsize = min (fsize, ((lpImage->nlin - y - 1) * 2) + 1);

		    if ((x - cur_off) < 0)
			fsize = min (fsize, (x * 2) + 1);

		    if ((x + cur_off) >= lpImage->npix)
			fsize = min (fsize, ((lpImage->npix - x - 1) * 2) + 1);

		    if (fsize >= 3) {
			switch (cur_off) {
			case 0:
			    sval = frame_sharpsmooth(IDC_SMOOTH, x, 
				       filt_buf[1], filt_buf[0], filt_buf[2],
				       Mask.BlendPressure);
			    break;
			case 1:
			    sval = frame_sharpsmooth(IDC_SMOOTH, x, 
				       filt_buf[0], filt_buf[1], filt_buf[2],
				       Mask.BlendPressure);
			    break;
			case 2:
			    sval = frame_sharpsmooth(IDC_SMOOTH, x, 
				       filt_buf[2], filt_buf[0], filt_buf[1],
				       Mask.BlendPressure);
			    break;
			}
			tlptr = out_buf + DEPTH * x;
			frame_putpixel(tlptr, sval);
		    }
		    else {
			tlptr = filt_buf[cur_off] + DEPTH * x;
			frame_getpixel (tlptr, &sval);
			tlptr = out_buf + DEPTH * x;
			frame_putpixel (tlptr, sval);
		    }
		}
		else {
		    tlptr = filt_buf[cur_off] + DEPTH * x;
		    frame_getpixel (tlptr, &sval);
		    tlptr = out_buf + DEPTH * x;
		    frame_putpixel (tlptr, sval);
		}
	    }
	    tlptr = CachePtr(0, xstart, y, YES);
	    if (tlptr)
		copy (&out_buf [DEPTH * xstart], tlptr,
					DEPTH * (xend-xstart+1));
	    tlptr = pbuffer;
	    pbuffer = cbuffer;
	    cbuffer = tlptr;
	    copy (mbuffer, cbuffer, lpImage->npix);
	}
	++y;
/*****************************************************************************/
/* if currently smaller than requested and we can enlarge in y, enlarge area */
/*****************************************************************************/
	diff_off = pix_off - cur_off;
	max_off = lpImage->nlin - 1 - y;
	max_off = min (max_off, y);

	if (max_off <= 0)
	    cur_off = 0;
	else if (cur_off == 0) {
	    if (!fill_filt_buf (filt_buf, y, filter_size, &cur_off)) {
		goto error;
	    }
	}
	else if ((diff_off > 0) && (max_off > cur_off)) {
	    if (max_off >= pix_off)
		new_off = pix_off;
	    else
		new_off = max_off;
	    ystart = y + 1;
	    ystart = bound (ystart, 0, lpImage->nlin-1);
	    yend = ystart + new_off;
	    yend = bound (yend, 0, lpImage->nlin-1);
	    for (i = ystart, j = cur_off + 1; i <= yend; ++i, ++j) {
		if (!readimage (i, 0, lpImage->npix-1,
				lpImage->npix, filt_buf [j])) {
		    Print ("error reading line %d", y);
		    goto error;
		}
	    }
	    cur_off = new_off;
	}
    /* see if we need to shrink filter size */
	else if (max_off < cur_off) {
	    new_off = max_off;
	    diff_off = cur_off - new_off;
	    for (i = diff_off; i < (cur_off*2); ++i) {
		tlptr = filt_buf[i];
		filt_buf[i] = filt_buf[i-diff_off];
		filt_buf[i-diff_off] = tlptr;
	    }
	    cur_off = new_off;
	}
	else { /* leave filter the same size */
	    for (i = 1; i <= (cur_off*2); ++i) {
		tlptr = filt_buf[i];
		filt_buf[i] = filt_buf[i-1];
		filt_buf[i-1] = tlptr;
	    }
	    if (!readimage (y + cur_off, 0, lpImage->npix-1,
				lpImage->npix, filt_buf [cur_off*2])) {
		Print ("error reading line %d", y + cur_off);
		goto error;
	    }
	}
    }

    if (cbuffer)
	FreeUp (cbuffer);
    if (mbuffer)
	FreeUp (mbuffer);
    if (pbuffer)
	FreeUp (pbuffer);
    if (lpOutline)
	FreeUp ((LPTR) lpOutline);
    for (i = 0; i < filter_size; ++i) {
	if (filt_buf [i])
	    FreeUp (filt_buf [i]);
    }
    UpdateImage (&rMask, YES);
    AstralCursor( NULL );
    return (TRUE);

errorMemory:
    Print ("not enough memory to run filter");
error:
    if (cbuffer)
	FreeUp (cbuffer);
    if (mbuffer)
	FreeUp (mbuffer);
    if (pbuffer)
	FreeUp (pbuffer);
    if (lpOutline)
	FreeUp ((LPTR) lpOutline);
    for (i = 0; i < filter_size; ++i) {
	if (filt_buf [i])
	    FreeUp (filt_buf [i]);
    }
    UpdateImage (&rMask, YES);
    AstralCursor( NULL );
    return (FALSE);
}
