//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

#define MAX_FILTER_SIZE 3

// Static prototypes
static int fill_filt_buf(LPFRAME lpFrame,LPPTR filt_buf,int yline,int filter_size,LPINT cur_off);
static void BlendOr(LPTR src1, LPTR src2, LPTR dst,int xstart,int xend);
static LPPOINT BlendBuildOutline (LPMASK lpMask, int *nPoints, int xmin, int xmax);

/************************************************************************/
BOOL DoBlend( LPFRAME lpFrame, LPMASK lpMask, int Pressure )
/************************************************************************/
{
LPTR              filt_buf[MAX_FILTER_SIZE], lpFilt1, lpFilt2, lpFilt3;
LPTR              out_buf, mbuffer, cbuffer, pbuffer;
LPTR              lpDst, lpSrc, lpMaskBuffer, tlptr;
int               x, y, i, j, ystart, yend, filter_size, fsize, ofsize;
int               pix_off, cur_off, new_off, diff_off, max_off, xstart, xend;
int               nPoints, iXSize, iYSize, iDepth;
RECT              rMask, rRepair;
LPPOINT           lpOutline, lpOutlineCurr, lpOutlineEnd;
DWORD             size;
SHARPSMOOTHPROC   lpSmoothProc;

// Make sure there is a frame
if (!lpFrame)
	return(FALSE);

// Make sure we have a mask to blend
if ( !lpMask )
	return (FALSE);

ProgressBegin(1,0);

// Eliminate numerous function calls
iXSize = FrameXSize( lpFrame );
iYSize = FrameYSize( lpFrame );
iDepth = FrameDepth( lpFrame );
if (iDepth == 0) iDepth = 1;

size = sizeof(POINT);
MaskRect( lpMask, &rRepair );

filter_size = MAX_FILTER_SIZE;

out_buf = LineBuffer[0];

for (i = 0; i < filter_size; i++)
	filt_buf[i] = NULL;

// Allocate space for the source data
for (i = 0; i < filter_size; ++i)
	{
	if ( !AllocLines (&filt_buf[i], 1, iXSize, iDepth))
		goto errorMemory;
	}

xstart = rRepair.left - 2;
xstart = bound (xstart, 0, iXSize-1);

xend   = rRepair.right + 1;
xend   = bound (xend,   0, iXSize-1);

ystart = rRepair.top - 1;
ystart = bound (ystart, 0, iYSize-1);

yend   = rRepair.bottom + 1;
yend   = bound (yend,   0, iYSize-1);

rMask.left   = xstart;
rMask.right  = xend;
rMask.top	= ystart;
rMask.bottom = yend;

/* sort the points */
lpOutline = BlendBuildOutline (lpMask, &nPoints, xstart, xend);

lpOutlineCurr  = lpOutline;
//lpOutlineEnd = lpOutline + nPoints - 1;
lpOutlineEnd   = (LPPOINT)BumpPtr((LPTR)lpOutline, size*(nPoints-1));

// Get the source data
if (!fill_filt_buf (lpFrame, filt_buf, ystart, filter_size, &cur_off))
	{
	goto error;
	}
pix_off = filter_size / 2;

mbuffer = cbuffer = pbuffer = NULL;

if (!(cbuffer = Alloc((long)iXSize+4)))
	goto error;

if (!(mbuffer = Alloc((long)iXSize+4)))
	goto error;

if (!(pbuffer = Alloc((long)iXSize+4)))
	goto error;

clr (pbuffer, iXSize+4);
clr (cbuffer, iXSize+4);

switch(iDepth)
	{
	case 1: lpSmoothProc = GetSharpSmooth8( IDC_SMOOTH, Pressure); break;
	case 3: lpSmoothProc = GetSharpSmooth24(IDC_SMOOTH, Pressure); break;
	case 4: lpSmoothProc = GetSharpSmooth32(IDC_SMOOTH, Pressure); break;
	}

for (y = ystart; y <= yend;)
	{
	AstralClockCursor (y-ystart, yend-ystart, NO);

	if (cur_off > 0)
		{
		// This gets the mask for this scanline as 0 | 255
		clr (mbuffer, iXSize+4);

		// Get to the current scanline mask
		while (
				((HPTR)lpOutlineCurr <= (HPTR)lpOutlineEnd) &&
				(lpOutlineCurr->y < y+1))
			{
			// We are past the current scanline
			if (lpOutlineCurr->y > y+1)
				break;

			// lpOutlineCurr++;
			lpOutlineCurr = 
				(LPPOINT)BumpPtr((LPTR)lpOutlineCurr, size);
			}

		// We are on the current scanline
		while (((HPTR)lpOutlineCurr <= (HPTR)lpOutlineEnd) &&
			(lpOutlineCurr->y == y+1))
			{
			register int x1;

			x1 = lpOutlineCurr->x;

			mbuffer[x1  ] = 255;
			mbuffer[x1+1] = 255;
			mbuffer[x1+2] = 255;

			// lpOutlineCurr++;
			lpOutlineCurr = 
				(LPPOINT)BumpPtr((LPTR)lpOutlineCurr, size);
			}

		// Or in the two previous mask scanlines
		BlendOr(mbuffer, cbuffer, pbuffer, xstart, xend+2);

		// Get our scanline mask pointer
		lpMaskBuffer = &pbuffer[xstart+1];

		// General speed-up
		if ( !(lpDst = FramePointer( lpFrame, xstart, y, YES )) )
			break; // If we cannot get this line then do nothing

		// Get the source data pointer
		lpSrc = filt_buf[cur_off] + (xstart*iDepth);

		// Eliminate numerous comparisons
		ofsize = (2 * cur_off) + 1;
		if ((y - cur_off) < 0)
			ofsize = min (ofsize, (y * 2) + 1);

		if ((y + cur_off) >= iYSize)
			ofsize = min (ofsize, ((iYSize - y - 1) * 2) + 1);

		switch( cur_off )
			{
			case 0 :
				lpFilt1 = filt_buf[1];
				lpFilt2 = filt_buf[0];
				lpFilt3 = filt_buf[2];
			break;

			case 1 :
				lpFilt1 = filt_buf[0];
				lpFilt2 = filt_buf[1];
				lpFilt3 = filt_buf[2];
			break;

			case 2 :
				lpFilt1 = filt_buf[2];
				lpFilt2 = filt_buf[0];
				lpFilt3 = filt_buf[1];
			break;
			}

		switch( iDepth )
			{
			case 1:
				for (x = xstart; x <= xend; ++x)
					{
					if (*lpMaskBuffer++)
						{
						fsize = ofsize;

						if ((x - cur_off) < 0)
							fsize = min (fsize, (x * 2) + 1);

						if ((x + cur_off) >= iXSize)
							fsize =
								min (fsize, ((iXSize - x - 1) * 2) + 1);

						if (fsize >= 3)
							{
							(*lpSmoothProc)(
								x, 
								lpFilt1,
								lpFilt2,
								lpFilt3,
								lpDst );
							}
						else
							{
							*lpDst = *lpSrc;
							}
						}
					else
						{
						*lpDst = *lpSrc;
						}
					lpDst++;
					lpSrc++;
					}
			break;

			case 3:
				for (x = xstart; x <= xend; ++x)
					{
					if (*lpMaskBuffer++)
						{
						fsize = ofsize;

						if ((x - cur_off) < 0)
							fsize = min (fsize, (x * 2) + 1);

						if ((x + cur_off) >= iXSize)
							fsize = 
								min (fsize, ((iXSize - x - 1) * 2) + 1);

						if (fsize >= 3)
							{
							(*lpSmoothProc)(
								x, 
								lpFilt1,
								lpFilt2,
								lpFilt3,
								lpDst );
							}
						else
							{
							lpDst[0] = lpSrc[0];
							lpDst[1] = lpSrc[1];
							lpDst[2] = lpSrc[2];
							}
						}
						else
						{
						lpDst[0] = lpSrc[0];
						lpDst[1] = lpSrc[1];
						lpDst[2] = lpSrc[2];
						}
					lpDst += 3;
					lpSrc += 3;
					}
			break;

			case 4:
				for (x = xstart; x <= xend; ++x)
					{
					if (*lpMaskBuffer++)
						{
						fsize = ofsize;

						if ((x - cur_off) < 0)
							fsize = min (fsize, (x * 2) + 1);

						if ((x + cur_off) >= iXSize)
							fsize = min (fsize, ((iXSize - x - 1) * 2) + 1);

						if (fsize >= 3)
							{
							(*lpSmoothProc)(
								x, 
								lpFilt1,
								lpFilt2,
								lpFilt3,
								lpDst );
							}
						else
							{
							lpDst[0] = lpSrc[0];
							lpDst[1] = lpSrc[1];
							lpDst[2] = lpSrc[2];
							lpDst[3] = lpSrc[3];
							}
						}
					else
						{
						lpDst[0] = lpSrc[0];
						lpDst[1] = lpSrc[1];
						lpDst[2] = lpSrc[2];
						lpDst[3] = lpSrc[3];
						}
					lpDst += 4;
					lpSrc += 4;
					}
			break;
			}

		// Swap pbuffer and cbuffer
		tlptr   = pbuffer;
		pbuffer = cbuffer;
		cbuffer = tlptr;

		// Copy mbuffer into cbuffer
		copy (mbuffer, cbuffer, iXSize+4);
		}

	++y;

	/**********************************************/
	/* if currently smaller than requested and we */
	/* can enlarge in y, enlarge area			 */
	/**********************************************/

	diff_off = pix_off - cur_off;

	max_off  = iYSize - 1 - y;
	max_off  = min (max_off, y);

	if (max_off <= 0)
		cur_off = 0;
	else
	if (cur_off == 0)
		{
		if (!fill_filt_buf (lpFrame, filt_buf, y, filter_size, &cur_off))
			{
			goto error;
			}
		}
	else
	if ((diff_off > 0) && (max_off > cur_off))
		{
		if (max_off >= pix_off)
			new_off = pix_off;
		else
			new_off = max_off;

		xstart = y + 1;
		xstart = bound (ystart, 0, iYSize-1);
		xend   = ystart + new_off;
		xend   = bound (yend, 0, iYSize-1);

		for (i = ystart, j = cur_off + 1; i <= yend; ++i, ++j)
			{
			if ( !FrameRead( lpFrame, 0, i, iXSize, 
				filt_buf[j], iXSize))
				{
				Print ("error reading line %d", y);
				goto error;
				}
			}
		cur_off = new_off;
		}
	/* see if we need to shrink filter size */
	else if (max_off < cur_off)
		{
		new_off = max_off;
		diff_off = cur_off - new_off;

		for (i = diff_off; i < (cur_off*2); ++i)
			{
			tlptr = filt_buf[i];
			filt_buf[i] = filt_buf[i-diff_off];
			filt_buf[i-diff_off] = tlptr;
			}
		cur_off = new_off;
		}
	else
		{ /* leave filter the same size */
		for (i = 1; i <= (cur_off*2); ++i)
			{
			tlptr = filt_buf[i];
			filt_buf[i] = filt_buf[i-1];
			filt_buf[i-1] = tlptr;
			}
		if ( !FrameRead( lpFrame, 0, y + cur_off, iXSize,
			filt_buf[cur_off*2], iXSize))
			{
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

for (i = 0; i < filter_size; ++i)
	{
	if (filt_buf[i])
		FreeUp (filt_buf[i]);
	}

ProgressEnd();
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

for (i = 0; i < filter_size; ++i)
	{
	if (filt_buf[i])
		FreeUp (filt_buf[i]);
	}
ProgressEnd();
return (FALSE);
}

/************************************************************************/
static BOOL fill_filt_buf( LPFRAME lpFrame, LPPTR filt_buf, int yline,
               int filter_size, LPINT cur_off )
/************************************************************************/
{
int pix_off, ystart, yend, i, y;

pix_off = filter_size / 2;
ystart = yline - pix_off;
ystart = bound (ystart, 0, FrameYSize(lpFrame));

if ((ystart == yline) || (yline < 0))
	{
	*cur_off = 0;
	return (TRUE);
	}

yend = yline + pix_off;
yend = bound (yend, 0, FrameYSize(lpFrame));

for (y = ystart, i = 0; y <= yend; ++y, ++i)
	{
	if (y == yline)
		*cur_off = i;
	if ( !FrameRead( lpFrame, 0, y, FrameXSize(lpFrame), 
		filt_buf[i], FrameXSize(lpFrame)))
		{
		Print ("error reading line %d", y);
		return (FALSE);
		}
	}

return( TRUE );
}

/************************************************************************/
static VOID BlendOr( LPTR src1, LPTR src2, LPTR dst, int xstart, int xend )
/************************************************************************/
{
int count;

src1 += xstart;
src2 += xstart;
dst  += xstart;
count = xend - xstart + 1;

while(count-- > 0)
	*dst++ |= (*src1++ | *src2++);
}

/************************************************************************/
LPPOINT BlendBuildOutline( LPMASK lpMask, int *nPoints, int xmin, int xmax )
/************************************************************************/
{
LPPOINT lpOutline, lpPntOut;
int pass, width;
int x, y;
DWORD size;
LPTR lpBuffer[3];
RECT rMask;
int prev, curr, next, temp;
BYTE p, c, n;
BOOL found;
LPTR lpP, lpC, lpN;

MaskRect( lpMask, &rMask );
rMask.left--;
rMask.right++;
width = RectWidth(&rMask);
if (rMask.left < 0) rMask.left = 0;

if (!AllocLines(lpBuffer, 3, width, 1))
	return(NULL);

size = sizeof(POINT);
for (pass = 1; pass <= 2; ++pass)
	{
	if (pass == 2)
		{
		/* Allocate the space for the outline (sorted) */
		lpOutline = (LPPOINT) Alloc ((long)sizeof(POINT) * (*nPoints));
		if (!lpOutline)
			{
			FreeUp(lpBuffer[0]);
			return (lpOutline);
			}

		lpPntOut = lpOutline;
		}
	*nPoints = 0;
	prev = 0; curr = 1; next = 2;
	MaskLoad( lpMask, rMask.left, rMask.top-1, width, lpBuffer[prev] );
	MaskLoad( lpMask, rMask.left, rMask.top, width, lpBuffer[curr] );

	for (y = rMask.top; y <= rMask.bottom; ++y)
		{
		MaskLoad( lpMask, rMask.left, y+1, width, lpBuffer[next] );
		lpP = lpBuffer[prev];
		lpC = lpBuffer[curr];
		lpN = lpBuffer[next];
		p = *lpC++;
		c = *lpC++; // current is lpC + 1
		++lpP;		// set Previous line to current x
		++lpN;		// set Next line to current x

		for (x = rMask.left+1; x <= rMask.right-1; ++x)
			{
			n = *lpC++;
			found = (c && (x >= xmin) && (x <= xmax)) &&
				(!p || !n || !(*lpP) || !(*lpN));
			if (found)
				{
				if (pass == 2)
					{
					lpPntOut->x = x;
					lpPntOut->y = y;
					lpPntOut = (LPPOINT)BumpPtr((LPTR)lpPntOut, size);
					}
				(*nPoints)++;
				}
			p = c;
			c = n;
			++lpP; ++lpN;
			}
		temp = prev;
		prev = curr;
		curr = next;
		next = temp;
		}
	}

FreeUp(lpBuffer[0]);

return (lpOutline);
}

