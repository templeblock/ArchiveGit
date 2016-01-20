/*
// UTILS.CPP
//
// $Header: /PM8/App/UTILS.CPP 1     3/03/99 6:13p Gbeddow $
//
// Utility routines.
//
// $Log: /PM8/App/UTILS.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
 * 
 * 4     1/19/99 2:57p Lwilson
 * Print Shop color dialog integration.
// 
// 3     10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 2     5/26/98 2:39p Jayn
// New table code.
// 
//    Rev 1.0   14 Aug 1997 15:26:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:56   Fred
// Initial revision.
// 
//    Rev 1.7   27 May 1997 11:08:58   Fred
// Uses LONG definition for PI
// 
//    Rev 1.6   12 Mar 1997 15:52:18   Jay
// Rotation, etc.
// 
//    Rev 1.5   14 Feb 1997 09:35:20   Jay
// Made scale_number() use ints
// 
//    Rev 1.4   24 Jan 1997 12:39:20   Jay
// Initial line palette; toggle tweaks, etc.
// 
//    Rev 1.3   08 Jan 1997 15:22:20   Fred
// More text box changes
// 
//    Rev 1.2   01 Nov 1996 17:11:48   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.1   08 Jul 1996 08:49:10   Jay
// Bigger handles. Better pt_in_object
// 
//    Rev 1.0   14 Mar 1996 13:42:30   Jay
// Initial revision.
// 
//    Rev 1.2   05 Mar 1996 18:12:36   VSI
// fixed bug in _NEWPORTABLE section re: operator precedence error
// 
//    Rev 1.1   27 Feb 1996 17:07:16   VSI
// _NEWPORTABLE 'C' code replaces .asm -- not yet working
// 
//    Rev 1.0   16 Feb 1996 12:09:42   FRED
// Initial revision.
// 
//    Rev 2.4   19 Sep 1995 15:42:56   JAY
// Some changes for 32-bit version.
// 
//    Rev 2.3   02 May 1995 15:36:46   JAY
// Put in security check in access_file() (Sean's syndrome).
// 
//    Rev 2.2   25 Apr 1995 16:02:28   FRED
// 
//    Rev 2.1   25 Apr 1995 10:33:46   FRED
// 
//    Rev 2.0   07 Feb 1995 15:48:16   JAY
// Initial revision.
// 
//    Rev 1.15   30 Nov 1994 16:33:44   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.14   04 Aug 1994 14:22:06   JAY
// Fixed access_file() for 16-bit version.
// 
//    Rev 1.13   04 Aug 1994 07:33:52   JAY
// Large model and 32-bit changes
// 
//    Rev 1.12   18 Jul 1994 09:56:08   JAY
// Added access_file() to replace C library access routine.
// 
//    Rev 1.11   15 Jul 1994 11:29:04   JAY
// access_file().
// 
//    Rev 1.10   20 Jun 1994 13:26:56   JAY
// build_path_name() now handles network share prefixes.
// 
//    Rev 1.9   26 May 1994 13:40:38   JAY
// Added append_slash() and remove_slash().
// 
//    Rev 1.8   19 May 1994 08:47:28   JAY
// Changed get_base_path() to reference LPCSTRs instead of LPSTRs.
// 
//    Rev 1.7   31 Mar 1994 16:02:50   JAY
// Added get_base_name()
// 
//    Rev 1.6   14 Feb 1994 09:20:30   MACDUFF
// Fixed offbyone error in my change to bits_required_by()
// 
//    Rev 1.5   11 Feb 1994 15:58:38   MACDUFF
// Minutely faster bits_required_by().
// 
//    Rev 1.4   07 Feb 1994 16:01:42   MACDUFF
// Support clipping in xor_line and do_XorBox
// 
//    Rev 1.3   30 Jan 1994 09:01:16   JAY
// Added build_file_path for general use to UTILS.
// 
//    Rev 1.2   19 Jan 1994 17:06:34   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:05:38   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:24:54   JAY
// 
*/

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dos.h>

#include "redisp.h"
#include "utils.h"

PUBLIC BYTE near bit_masks[] =
{
	0x80,
	0x40,
	0x20,
	0x10,
	0x08,
	0x04,
	0x02,
	0x01
};

/*
// Compute the midpoint of two numbers.
*/

int midpoint(int n1, int n2)
{
#ifdef WIN32
	return (n1+n2)/2;
#else
	int r;

	_asm
	{
		mov	ax, n1		; n1 to ax
		mov	bx, n2		; n2 to bx

		cwd					; dx:ax is (long)n1
		xchg	bx, ax				; n1 <-> n2
		mov	cx, dx				; cx:bx is (long)n1
		cwd					; dx:ax is (long)n2
		add	ax, bx				; (long)n1 + (long)n2
		adc	dx, cx
		sar	dx, 1				; /2
		rcr	ax, 1				;    /2
	; Returns AX
		mov	r, ax
	}
	return r;
#endif
}

/*
// Scale a number by the ratio of two other numbers.
// (like muldiv).
*/

int scale_number(int x, int n, int d)
{
#ifdef WIN32
	return (x*n)/d;
#else
	int r;

	_asm
	{
		mov	ax, x
		imul	n
		idiv	d
		mov	r, ax
	}

	return r;
#endif
}

/*
// Scale a pcoord by the ratio of two other pcoords.
// (like long muldiv).
*/

PUBLIC PCOORD
scale_pcoord(PCOORD x, PCOORD n, PCOORD d)
{
	return (x*n)/d;
}

/*
// Convert a 32-bit (CMYK) color to a 24-bit (RGB) color.
*/

PUBLIC RGBCOLOR
rgbcolor_from_color(COLOR color)
{
	SHORT red, green, blue, black;

	red = 255 - (SHORT)CYAN_COMPONENT_OF(color);
	green = 255 - (SHORT)MAGENTA_COMPONENT_OF(color);
	blue = 255 - (SHORT)YELLOW_COMPONENT_OF(color);

	if ((black = (SHORT)BLACK_COMPONENT_OF(color)) != 0)
	{
	/* Black component is non-zero. Account for it now. */
		if ((red -= black) < 0)
		{
			red = 0;
		}
		if ((green -= black) < 0)
		{
			green = 0;
		}
		if ((blue -= black) < 0)
		{
			blue = 0;
		}
	}
	return(MAKE_RGBCOLOR(red, green, blue));
}

/*
// Convert an RGB color to a gray shade.
// (The range of the gray is the same as the range of the RGB components).
// The weighting used is 30% red, 59% green and 11% blue.
*/

BYTE rgbcolor_to_gray(RGBCOLOR rgbcolor)
{
	int nRed = (int)RED_COMPONENT_OF(rgbcolor);
	int nGreen = (int)GREEN_COMPONENT_OF(rgbcolor);
	int nBlue = (int)BLUE_COMPONENT_OF(rgbcolor);

	return (BYTE)((30*nRed + 59*nGreen + 11*nBlue)/100);
}

int mulrem(int x, int n, int d)
{
#ifdef WIN32
	return (x*n)%d;
#else
	int r;
	_asm
	{
		mov	ax, x
		imul	n
		idiv	d
		mov	r, dx			; want remainder
	}
	return r;
#endif
}

/*
// Some math helpers.
*/

#define _PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862

PUBLIC ANGLE const PI = _PI;
PUBLIC ANGLE const PI2 = _PI*2.0;
PUBLIC ANGLE const DEG2RADC = _PI/180.0;
PUBLIC ANGLE const RAD2DEGC = 180.0/_PI;

/*
// Normalize an angle. Make it in the range [0, 2*PI).
*/

PUBLIC ANGLE
normalize_angle(ANGLE angle)
{
	while (angle < 0)
	{
		angle += PI2;
	}
	while (angle >= PI2)
	{
		angle -= PI2;
	}
	return angle;
}

/*
// Return the intersection of two boxes.
*/

PUBLIC BOOL IntersectBox(PBOX_PTR result, PBOX_PTR a, PBOX_PTR b)
{
	if (IsBoxOnBox(a,b))
	{
		result->x0 = __max(a->x0, b->x0);
		result->x1 = __min(a->x1, b->x1);
		result->y0 = __max(a->y0, b->y0);
		result->y1 = __min(a->y1, b->y1);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
// Return the union of two boxes.
*/

PUBLIC VOID UnionBox(PBOX_PTR result, PBOX_PTR a, PBOX_PTR b )
{
	result->x0 = __min(a->x0, b->x0);
	result->y0 = __min(a->y0, b->y0);
	result->x1 = __max(a->x1, b->x1);
	result->y1 = __max(a->y1, b->y1);
}

/*
// Convert a Windows RGB color to a document color.
*/

PUBLIC COLOR color_from_colorref(COLORREF color)
{
	return COLOR_FROM_RGBCOLOR(MAKE_RGBCOLOR(GetRValue(color),
 													GetGValue(color),
 													GetBValue(color)));
}

/*
// Convert a document color to a Windows RGB color.
*/

PUBLIC COLORREF colorref_from_color(COLOR color)
{
	RGBCOLOR rgbcolor = RGBCOLOR_FROM_COLOR(color);

	return RGB(RED_COMPONENT_OF(rgbcolor),
					GREEN_COMPONENT_OF(rgbcolor),
					BLUE_COMPONENT_OF(rgbcolor));
}

static long RoundToLong(double d)
{
	if (d < 0)
	{
		d -= 0.5;
	}
	else
	{
		d += 0.5;
	}
	return (long)d;
}

/*
// Rotate an array of points about a center a certain number of degrees.
*/

PUBLIC VOID
rotate_points(PPNT_PTR p, SHORT points, PPNT center, ANGLE angle)
{
/* Normalize the angle. */

	if ((angle = normalize_angle(angle)) == 0.0)
	{
		return;
	}

/* Compute sine and cosine of angle once for all points. */

	double sine = sin(angle);
	double cosine = cos(angle);

/* Rotate all points. */

	for (SHORT i = 0; i < points; i++, p++)
	{
		double x = (double)(p->x - center.x);
		double y = (double)(p->y - center.y);

		p->x = RoundToLong(x*cosine + y*sine) + center.x;
		p->y = RoundToLong(y*cosine - x*sine) + center.y;
	}
}

/*
// Rotate an array of points about a center a certain number of degrees.
// This is the POINT version.
*/

void RotatePoints(POINT* p, int nPoints, POINT center, double dAngle)
{
	// Normalize the angle.
	if ((dAngle = normalize_angle(dAngle)) == 0.0)
	{
		return;
	}

	// Compute sine and cosine of angle once for all points.
	double dSine = sin(dAngle);
	double dCosine = cos(dAngle);

	// Rotate all points.
	for (int i = 0; i < nPoints; i++, p++)
	{
		double x = (double)(p->x - center.x);
		double y = (double)(p->y - center.y);

		p->x = RoundToLong(x*dCosine + y*dSine) + center.x;
		p->y = RoundToLong(y*dCosine - x*dSine) + center.y;
	}
}

/*
// Compute the rotated bound of a bound and a rotation.
*/

PUBLIC VOID
compute_rotated_bound(PBOX bound, ANGLE rotation, PBOX_PTR out_bound, PPNT origin, PPNT_PTR verts)
{
	PPNT _verts[4];

	if (verts == NULL)
	{
		verts = _verts;
	}

/*
// We take the four corners of the unrotated bound and rotate them.
// 0 is upper left, 1 is upper right, 2 is lower right, 3 is lower left.
*/

	verts[0].x = verts[3].x = bound.x0;
	verts[1].x = verts[2].x = bound.x1;
	verts[0].y = verts[1].y = bound.y0;
	verts[2].y = verts[3].y = bound.y1;

	/* Now rotate them. */

	if (rotation != 0)
	{
		rotate_points(verts, 4, origin, rotation);
	}

	compute_enclosing_bound(verts, 4, out_bound);
}

/*
// Compute the bound enclosing all the points in the passed array.
*/

PUBLIC VOID
compute_enclosing_bound(PPNT_PTR pp, SHORT count, PBOX_PTR out_bound)
{
/*
// At this time, we also compute the enclosing bound.
*/

	PBOX bound;

	bound.x0 = bound.y0 = 0x7FFFFFFF;
	bound.x1 = bound.y1 = -bound.x0;

	for (SHORT i = 0; i < count; i++)
	{
		PPNT p = *pp++;

		if (p.x < bound.x0)
		{
			bound.x0 = p.x;
		}
		if (p.x > bound.x1)
		{
			bound.x1 = p.x;
		}
		if (p.y < bound.y0)
		{
			bound.y0 = p.y;
		}
		if (p.y > bound.y1)
		{
			bound.y1 = p.y;
		}
	}
	*out_bound = bound;
}

/*
// Xor a box.
*/

PUBLIC VOID
do_XorBox(HDC hdc, RECT r, int width, RECT *prectClip)
{
/*
// Do a little check to make sure we can do all four sides.
*/

	int dx = r.right - r.left;
	int dy = r.bottom - r.top;
	int sml = __min(dx, dy)/2;

	if (width > sml)
	{
		width = sml;
	}

	if (width == 0)
	{
	/* Fills the whole thing. */
		xor_line(hdc, r.left, r.top, r.right, r.bottom, prectClip);
	}
	else
	{
		xor_line(hdc, r.left, r.top, r.right, r.top+width, prectClip);
		xor_line(hdc, r.right-width, r.top+width, r.right, r.bottom-width, prectClip);
		xor_line(hdc, r.left, r.bottom-width, r.right, r.bottom, prectClip);
		xor_line(hdc, r.left, r.top+width, r.left+width, r.bottom-width, prectClip);
	}
}

void ControlDot(HDC hdc, int x, int y)
{
/*
// This should help explain why the following numbers are used.
// (This was created when width was 3. Use your imagination for other values.)
//
//		 -1 x   +2
//			X X X
//     y
//			X X X
//    +1
//			X X X
//    +2
*/

	PatBlt(hdc,
			 x-CONTROL_DOT_WIDTH/2, y-CONTROL_DOT_WIDTH/2,
			 CONTROL_DOT_WIDTH, CONTROL_DOT_WIDTH,
			 DSTINVERT);
}

void ControlDot(HDC hdc, int x, int y, ANGLE dAngle, int nRotateX, int nRotateY)
{
	PPNT v[4];
	x -= CONTROL_DOT_WIDTH/2;
	y -= CONTROL_DOT_WIDTH/2;
	v[0].x = x;
	v[0].y = y;
	v[1].x = x+CONTROL_DOT_WIDTH;
	v[1].y = y;
	v[2].x = x+CONTROL_DOT_WIDTH;
	v[2].y = y+CONTROL_DOT_WIDTH;
	v[3].x = x;
	v[3].y = y+CONTROL_DOT_WIDTH;

	for (int j = 0; j < 4; j++)
	{
		v[j].x *= 2;
		v[j].y *= 2;
	}

	PPNT RotateCenter;
	RotateCenter.x = nRotateX * 2 + 1;
	RotateCenter.y = nRotateY * 2 + 1;

	rotate_points(v, 4, RotateCenter, dAngle);

	POINT p[4];
	for (int i = 0; i < 4; i++)
	{
		p[i].x = (int)(v[i].x / 2);
		p[i].y = (int)(v[i].y / 2);
	}

	HPEN hPen = (HPEN)GetStockObject(NULL_PEN);

	if (hPen != NULL)
	{
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		if (hOldPen != NULL)
		{
			HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
			if (hBrush != NULL)
			{
				HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
				if (hOldBrush != NULL)
				{
					int oldROP2 = SetROP2(hdc, R2_NOT);

					Polygon(hdc, p, 4);

					SetROP2(hdc, oldROP2);
					SelectObject(hdc, hOldBrush);
				}
			}
			SelectObject(hdc, hOldPen);
		}
	}
}

/*
// Xor a gray line.
*/

void xor_line(HDC hdc, int x0, int y0, int x1, int y1, RECT *prectClip)
{
	// Quick check to see if we're going to clip
	if (prectClip != NULL)
	{
		RECT rectResult, rectSrc;
		SetRect(&rectSrc, x0, y0, x1, y1);
		if (!IntersectRect(&rectResult, prectClip, &rectSrc))
		{
			return;
		}
	}

	HBRUSH hBrush;

	if ((hBrush = CreateSolidBrush(SELECT_COLOR)) != NULL)
	{
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		PatBlt(hdc, x0, y0, x1-x0, y1-y0, PATINVERT);

		DeleteObject(SelectObject(hdc, hOldBrush));
	}
}

/*
// Xor a line which is not vertical or horizontal.
*/

void xor_polyline(HDC hdc, POINT *p, int points, BOOL close)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, SELECT_COLOR);

	if (hPen != NULL)
	{
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		int oldROP2 = SetROP2(hdc, R2_XORPEN);
		int i = points;

	/* Move to first point. */
	/* Line to rest of points. */

		for (i = points, MoveToEx(hdc, p->x, p->y, NULL);
 							--i > 0;
 							LineTo(hdc, p[i].x, p[i].y))
			;

	/* Close the figure if requested. */

		if (close && points > 2)
		{
			LineTo(hdc, p->x, p->y);
		}

		SetROP2(hdc, oldROP2);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);
	}
}

void XorBox(RedisplayContextPtr rc, PBOX pbox)
{
	RECT r;
	HDC hdc = rc->destination_hdc;

	rc->pbox_to_screen(&pbox, TRUE);

	if (rc->convert_pbox(&pbox, &r, NULL))
	{
		do_XorBox(hdc, r);
	}
}

/*
// The asm code below was converted to C code by Roger House 96/02/23
//
// bits_required_by
//
//	Count the number of bits required to hold a 32-bit number.
//
*/

PUBLIC int16 bits_required_by(uint32 nn)
{
	int16	r;

#if 0
	_asm
	{
		mov	ax, word ptr nn + 2
		mov	cx, 16
loop1a:
		shl	ax, 1
		jc		loop1z
		loop	loop1a
loop1z:
		jcxz	loword
		
		add	cx, 16
		jmp	done
loword:

		mov	cx, 16
		mov	ax, word ptr nn
loop2a:
		shl	ax, 1
		jc		loop2z
		loop	loop2a
loop2z:
done:
		mov	r, cx
	}
#else // 0

#ifdef _NEWPORTABLE

	if (nn == 0)
		return(0);

	r = 32;

	while ((nn & 0xff000000) == 0)
	{
		r -= 8;
		nn <<= 8;
	}

	while ((nn & 0x80000000) == 0)
	{
		r -= 1;
		nn <<= 1;
	}

	return(r);

#else // _NEWPORTABLE

	_asm
	{
		xor	cx, cx
		mov	ax, word ptr nn+2
		or	ah, ah
		jz	less24
		mov	cx, 33
		mov	al, ah
		jmp	short count_byte
	less24:
		or	al, al
		jz	less16
		mov	cx, 25
		jmp	short count_byte
	less16:
		mov	ax, word ptr nn+0
		or	ah, ah
		jz	less8
		mov	cx, 17
		mov	al, ah
		jmp	short count_byte
	less8:
		or	al, al
		jz	outta_here
		mov	cx, 9
	
	count_byte:
;		shl	al, 1
;		jc	outta_here
;		loop	count_byte
		; This is even faster: (MH)
		add	al, al			; Faster than shift
		dec	cx					; Can be pipelined on 486+
		jnc	count_byte		; using carry from ADD instruction
									; NOTE we depend on knowing AL<>0.
	
	outta_here:
		mov	r, cx
	}

#endif	// _NEWPORTABLE

#endif	// else 0

	return r;

} // end bits_required_by


static void CompMul(long src1, long src2, long dst[2])
{
#ifdef WIN32
/* C Version */
	int negative = (src1 ^ src2) < 0;

	if (src1 < 0)
	{
		src1 = -src1;
	}
	if (src2 < 0)
	{
		src2 = -src2;
	}

	unsigned short src1hi = (unsigned short)(src1 >> 16);
	unsigned short src1lo = (unsigned short)src1;
	unsigned short src2hi = (unsigned short)(src2 >> 16);
	unsigned short src2lo = (unsigned short)src2;
	register unsigned long temp = (unsigned long)src1hi * src2lo + (unsigned long)src1lo * src2hi;

	unsigned long dsthi = (unsigned long)src1hi * src2hi + (temp >> 16);
	unsigned long dstlo = (unsigned long)src1lo * src2lo;
	temp <<= 16;
	dsthi += (dstlo += temp) < temp;
	dst[0] = dsthi;
	dst[1] = dstlo;

	if (negative)
	{
		if ((dstlo = (unsigned long)-((long)dstlo)) != 0)
		{
			dsthi = ~dsthi;
		}
		else
		{
			dsthi = (unsigned long)-((long)dsthi);
		}
	}
	dst[0] = dsthi;
	dst[1] = dstlo;
#else
/* ASM version */
	uint8 negative;

	_asm {

		xor	ax,ax

		mov	negative,al

		cmp	word ptr src1+2,ax
		jge	l0

		neg	word ptr src1+2
		neg	word ptr src1+0
		sbb	word ptr src1+2,ax

		not	byte ptr negative
	}

	l0:

	_asm {

		cmp	word ptr src2+2,ax
		jge	l1

		neg	word ptr src2+2
		neg	word ptr src2+0
		sbb	word ptr src2+2,ax

		not	byte ptr negative
	}

	l1:

	_asm {

		mov	si,ax
		mov	di,ax

		mov	ax,word ptr src1+0
		push	ax
		mul	word ptr src2+0

		mov	cx,dx
		mov	bx,ax

		pop	ax
		mul	word ptr src2+2

		add	cx,ax
		adc	si,dx

		mov	ax,word ptr src1+2
		push	ax
		mul	word ptr src2+0

		add	cx,ax
		adc	si,dx
		adc	di,0

		pop	ax
		mul	word ptr src2+2

		add	si,ax
		adc	di,dx

		xor	ax,ax

		cmp	byte ptr negative,al
		je		l2

		not	di
		not	si
		not	cx
		not	bx
		add	bx,1
		adc	cx,ax
		adc	si,ax
		adc	di,ax
	}

	l2:

	_asm {

		mov	ax,bx

		mov	bx, word ptr dst
		mov [bx+0], si
		mov [bx+2], di
		mov [bx+4], ax
		mov [bx+6], cx
	}

#endif
}


static long CompDiv(long src1, long src2[2])
{
#if 1
/* C Version */
#define POSINFINITY             0x7FFFFFFFL
#define NEGINFINITY             0x80000000L
    register unsigned long src2hi = src2[0], src2lo = src2[1];
    int negative = (long)(src2hi ^ src1) < 0;

    if ((long)src2hi < 0)
        if ((src2lo = -(long)src2lo) != 0)
            src2hi = ~src2hi;
        else
            src2hi = -(long)src2hi;
    if (src1 < 0)
        src1 = -src1;
    {   register unsigned long src1hi, src1lo;
        unsigned long result = 0, place = 0x40000000L;

        if ((src1hi = src1) & 1)
            src1lo = 0x80000000L;
        else
            src1lo = 0;

        src1hi >>= 1;
        src2hi += (src2lo += src1hi) < src1hi;      /* round the result */

        if (src2hi > src1hi || src2hi == src1hi && src2lo >= src1lo)
            if (negative)
                return NEGINFINITY;
            else
                return POSINFINITY;
        while (place && src2hi)
        {   src1lo >>= 1;
            if (src1hi & 1)
                src1lo += 0x80000000L;
            src1hi >>= 1;
            if (src1hi < src2hi)
            {   src2hi -= src1hi;
                src2hi -= src1lo > src2lo;
                src2lo -= src1lo;
                result += place;
            }
            else if (src1hi == src2hi && src1lo <= src2lo)
            {   src2hi = 0;
                src2lo -= src1lo;
                result += place;
            }
            place >>= 1;
        }
        if (src2lo >= (unsigned long)src1)
            result += src2lo/src1;
        if (negative)
            return -(long)result;
        else
            return result;
    }
#else
/* ASM Version */
extern void _cdecl _aNuldiv(void);
	uint16 n3, n2, n1, n0;
	uint16 d3, d2, d1, d0;

	_asm {

		xor	cx,cx

		mov	bx,word ptr src2
		mov	di,[bx]+2
		mov	si,[bx]+0
		mov	dx,[bx]+6
		mov	ax,[bx]+4

		xor	bx,bx

		or		di,di
		jns	l0

		not	di
		not	si
		not	dx
		not	ax
		add	ax,1
		adc	dx,bx
		adc	si,bx
		adc	di,bx

		not	cx
	}

	l0:

	_asm {

		mov	n3,di
		mov	n2,si
		mov	n1,dx
		mov	n0,ax

		mov	dx,word ptr src1+2
		mov	ax,word ptr src1+0

		or		dx,dx
		jns	l1

		neg	dx
		neg	ax
		sbb	dx,bx

		mov	word ptr src1+2,dx
		mov	word ptr src1+0,ax

		not	cx
	}

	l1:

	_asm {

		shr	dx,1
		rcr	ax,1

		mov	d3,dx
		mov	d2,ax

		mov	si,bx

		rcr	bx,1

		mov	d1,bx

		add	n0,ax
		adc	n1,dx

		mov	d0,si

		adc	n2,si
		adc	n3,si

		mov	ax,n3
		cmp	ax,d3
		jb		l3
		ja		l2
		mov	ax,n2
		cmp	ax,d2
		jb		l3
		ja		l2
		mov	ax,n1
		cmp	ax,d1
		jb		l3
		ja		l2
		mov	ax,n0
		cmp	ax,d0
		jb		l3
	}

	l2:

	_asm {

		mov	ax,cx
		not	ax
		mov	dx,08000h
		xor	dx,ax

		jmp	done
	}

	l3:

	_asm {

		mov	dx,4000h
		xor	ax,ax

		mov	di,ax
		mov	si,ax
	}

	l3a:

	_asm {

		mov	bx,n3
		or		bx,n2
		jz		l6

		shr	word ptr d3,1
		rcr	word ptr d2,1
		rcr	word ptr d1,1
		rcr	word ptr d0,1

		mov	bx,n3
		cmp	bx,d3
		jb		l5
		ja		l4
		mov	bx,n2
		cmp	bx,d2
		jb		l5
		ja		l4
		mov	bx,n1
		cmp	bx,d1
		jb		l5
		ja		l4
		mov	bx,n0
		cmp	bx,d0
		jb		l5
	}

	l4:

	_asm {

		mov	bx,d0
		sub	n0,bx
		mov	bx,d1
		sbb	n1,bx
		mov	bx,d2
		sbb	n2,bx
		mov	bx,d3
		sbb	n3,bx

		add	si,ax
		adc	di,dx
	}

	l5:

	_asm {

		shr	dx,1
		rcr	ax,1

		jnc	l3a
	}

	l6:

	_asm {

		mov	dx,n1
		mov	ax,n0

		cmp	dx, word ptr src1+2
		ja		l7
		jb		l8
		cmp	ax, word ptr src1+0
		jb		l8
	}

	l7:

	_asm {

		push	cx
push si
push di
		push	word ptr src1+2
		push	word ptr src1+0
		push	dx
		push	ax
		call	_aNuldiv
		add	si,ax
		adc	di,dx

pop di
pop si
		pop	cx
	}

	l8:

	_asm {

		mov	dx,di
		mov	ax,si

		jcxz	done

		neg	dx
		neg	ax
		sbb	dx,0
	}

	done:
		;
#endif
}


/*
 *  a*b/c
 */
PUBLIC long
ext_SafeLongMulDiv(long a, long b, long c)
{
    long temp[2];
    CompMul(a, b, temp);
    return CompDiv(c, temp);
}

/*
// Convert a floating point number to a string.
*/

PUBLIC VOID
float_to_string(CHAR *buffer, double f)
{
	CHAR *s;

/* Display a real. */

	sprintf(buffer, "%.4f", f);

	if (strchr(buffer, '.') != NULL)
	{
		/* Remove trailing zeros or decimal point */
		for (s = &buffer[strlen(buffer) - 1]; s > &buffer[0]; s--)
		{
			if (*s == '0')
			{
				*s = '\0';
			}
			else
			{
				if (*s == '.')
				{
					*s = '\0';
				}
		 		break;
			}
		}
	}
}

/*
// Build a path name by applying 'path' to 'name' if 'name' is some form
// of a relative path.
*/

PUBLIC VOID
build_path_name(LPSTR out_buffer, LPCSTR path, LPCSTR name)
{
	if (path != NULL)
	{
		if (name[0] == '\\' && name[1] == '\\')
		{
		/* Network name. This is NOT relative. */
		}
		else if ((name[0] == '\\' || name[0] == '/')
					&& path[1] == ':')
		{
		/* Absolute path minus drive. Copy just the drive letter. */
			*out_buffer++ = path[0];
			*out_buffer++ = ':';
		}
		else if (name[0] == '\0' || name[1] != ':')
		{
		/* Copy the full base path over. */
			SHORT len;

			strcpy(out_buffer, path);

			if ((len = strlen(out_buffer)) != 0)
			{
				CHAR c = out_buffer[len-1];

				if (c != '\\' && c != '/')
				{
					out_buffer[len++] = '\\';
					out_buffer[len] = '\0';
				}
			}
			out_buffer += len;
		}
	}
	strcpy(out_buffer, name);
}

/*
// Get a pointer to the base name of a picture.
*/

PUBLIC LPCSTR
get_base_name(LPCSTR in_name)
{
	LPCSTR p;

	if ((p = strrchr(in_name, '\\')) == NULL)
	{
		if ((p = strrchr(in_name, ':')) == NULL)
		{
			return in_name;
		}
	}
	return p+1;
}

/*
// Append a slash on the end of a string if it needs one.
*/

PUBLIC VOID
append_slash(LPSTR name)
{
	if (name != NULL)
	{
		int len = strlen(name);

		if (len != 0)
		{
			switch (name[len-1])
			{
				case ':':
				case '\\':
				case '/':
				{
					break;
				}
				default:
				{
					name[len++] = '\\';
					name[len] = '\0';
					break;
				}
			}
		}
	}
}

/*
// Remove any trailing slash from a file name.
*/

PUBLIC VOID
remove_slash(LPSTR name)
{
	if (name != NULL)
	{
		int len = strlen(name);

		if (len != 0)
		{
			switch (name[len-1])
			{
				case '\\':
				case '/':
				{
					name[len-1] = '\0';
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}
}

/*
// My version of access().
// This now returns a CFileException enum value (which may be 'none').
*/

PUBLIC int
access_file(PCSTR pName, int mode)
{
	long lRet;

#ifdef WIN32
	WIN32_FIND_DATA ffd;
	HANDLE hSearch;

	if ((hSearch = FindFirstFile(pName, &ffd)) == INVALID_HANDLE_VALUE)
	{
		lRet = GetLastError();
	}
	else
	{
		lRet = NO_ERROR;
		if ((mode & 2) && (ffd.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
		{
		/* We want to be able to write to it but we can't. */
			lRet = ERROR_ACCESS_DENIED;
		}
		FindClose(hSearch);
	}
#else	
	struct _find_t buffer;
	int nAccess = _A_NORMAL;

	if (!(mode & 2))
	{
	/* Doesn't require write permission. */
		nAccess |= _A_RDONLY;
	}

	lRet = (long)_dos_findfirst(pName, nAccess, &buffer);

	if (lRet == 0x12)
	{
	/* "No more files" == "file not found" */
		lRet = 2;
	}

	if (lRet == 0)
	{
	/*
	// Let's be REALLY sure we found the file, that this is not just a bogus
	// return value (it happens!).
	// We compare the name in the find buffer to the base name of what
	// we were looking for.
	*/
		if (stricmp(get_base_name(pName), buffer.name) != 0)
		{
		/* Names don't match! We didn't really find the file. */
			lRet = 2;
		}
	}

#endif

	if (lRet == 0)
	{
		return CFileException::none;			// Which should be 0.
	}
	else
	{
		return CFileException::OsErrorToException(lRet);
	}
}

BOOL SafeToConvertPboxToFixed(PBOX* p)
{
   return (p->x0 > -32000)
       && (p->x0 < 32000)
       && (p->x1 > -32000)
       && (p->x1 < 32000)
       && (p->y0 > -32000)
       && (p->y0 < 32000)
       && (p->y1 > -32000)
       && (p->y1 < 32000)
       && (p->x1-p->x0 > -32000)
       && (p->x1-p->x0 < 32000)
       && (p->y1-p->y0 > -32000)
       && (p->y1-p->y0 < 32000);
}

BOOL SafeToConvertPpntToFixed(PPNT* p)
{
   return (p->x > -32000)
       && (p->x < 32000)
       && (p->y > -32000)
       && (p->y < 32000);
}

void ScalePBOXWithRounding(PBOX_PTR pbox, SHORT x_num, SHORT x_denom, SHORT y_num, SHORT y_denom)
{
   PCOORD x0, x1;
   PCOORD y0, y1;

/* Insanity prevention. */

   if (x_denom == 0)
   {
      x_denom++;
   }
   if (y_denom == 0)
   {
      y_denom++;
   }

/* Handle the x coordinates. */

   x1 = pbox->x1 - (x0 = pbox->x0);

   if (x_num < x_denom)
   {
   /* Compression. */
      if (x0 != 0)
      {
         x0 = (x0*x_num)/x_denom;
      }
      if (x1 != 0)
      {
         x1 = ((x1-1)*x_num)/x_denom + 1;
      }
   }
   else
   {
   /* Expansion. */
      if (x0 != 0)
      {
         x0 = ((x0 * x_num)-1)/x_denom + 1;
      }
      if (x1 != 0)
      {
         x1 = ((x1 * x_num)-1)/x_denom + 1;
      }
   }

   pbox->x1 = (pbox->x0 = x0) + x1;

/* Handle the y coordinates. */

   y1 = pbox->y1 - (y0 = pbox->y0);

   if (y_num < y_denom)
   {
   /* Compression. */
      if (y0 != 0)
      {
         y0 = (y0*y_num)/y_denom;
      }
      if (y1 != 0)
      {
         y1 = ((y1-1)*y_num)/y_denom + 1;
      }
   }
   else
   {
   /* Expansion. */
      if (y0 != 0)
      {
         y0 = ((y0 * y_num)-1)/y_denom + 1;
      }
      if (y1 != 0)
      {
         y1 = ((y1 * y_num)-1)/y_denom + 1;
      }
   }
   pbox->y1 = (pbox->y0 = y0) + y1;
}

COLOR ColorFromRColor( const RColor& rColor )
{
	COLOR color = TRANSPARENT_COLOR;

	// Check for a non-transparent color
	if (rColor != RColor())
		color = COLOR_FROM_COLORREF( rColor.GetSolidColor( 0 ) );

	return color;
}

RColor RColorFromColor( COLOR color )
{
	if (color == TRANSPARENT_COLOR)
		return RColor();

	return RColor( RSolidColor( COLORREF_FROM_COLOR( color ) ) );
}
