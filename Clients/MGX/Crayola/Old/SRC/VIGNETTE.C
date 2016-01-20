//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

#define RAND_BITS 7
#define RAND_SUB 3   /* changed to 3 cause no rounding up, should be (RAND_BITS+1)/2 */
#define rotx(x, y, xc, yc, cosine, sine) fix2int((long)((cosine * (x - xc)) + (sine * (yc - y))))
#define TSIZE	1000


typedef void (*LPPROCESSVIGNETTE)(LPTR, int, LPLONG, LPLONG, int, BOOL);

static ULONG lSeed = 0xDEADBABA;
static int xr, nRepetitions, x1, y1, x2, y2, xc, yc, xmin, xmax, ymin, ymax;
static long D, xD, yD, m1, m2, m3, m4, b1, b2, b3, b4, D1, D2, D3, D4, ea, eb;
static LFIXED sine, cosine;
static LPINT lpD;
static LPLONG lplD, lpld;
static LPPROCESSVIGNETTE lpProcessProc;
static LPTR lpMidpointTable;
static long lTableRange;

// static functions
static void HSLtoRGB24(LPRGB lpSrc, int iCount);
static void HSLtoCMYK32(LPCMYK lpSrc, int iCount);

static void ProcessVignette8(
	LPTR   lpSrc,
	int    iCount,
	LPLONG lpd,
	LPLONG lpD,
	int    nRepetitions,
	BOOL   SoftTransition);

static void ProcessVignette24(
	LPRGB  lpSrc,
	int    iCount,
	LPLONG lpd,
	LPLONG lpD,
	int    nRepetitions,
	BOOL   SoftTransition);

static void ProcessVignette32(
	LPCMYK lpSrc,
	int    iCount,
	LPLONG lpd,
	LPLONG lpD,
	int    nRepetitions,
	BOOL   SoftTransition);


static void ellipse_vignette_proc(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth );
static void rectangle_vignette_proc(int the_y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth );
static void linear_vignette_proc(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth );
static void radial_vignette_proc(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth );
static void invert_proc(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth );

#define DO_RANDOM \
	{ \
	d = *lpd++; \
	D = *lpD++; \
	if ( d < 0) \
		{ \
		d = 0; \
		r = 0; \
		} \
	else if ( d > D ) \
		{ \
		d = D; \
		r = 0; \
		} \
	else \
		{ \
		if ( lSeed & BIT18 ) \
			{ \
			lSeed += lSeed; \
			lSeed ^= BITMASK; \
			} \
		else \
			{ \
			lSeed += lSeed; \
			} \
		r = (lSeed & RAND_BITS) - RAND_SUB; \
		} \
	}

#define NEW_RANDOM \
{ \
	if ( lSeed & BIT18 ) \
	{ \
		lSeed += lSeed; \
		lSeed ^= BITMASK; \
	} \
	else \
	{ \
		lSeed += lSeed; \
	} \
	r = (lSeed & RAND_BITS) - RAND_SUB; \
}

#define REPS_EQUAL_ONE \
{ \
	rate = (d<<16)/D; \
}

#define REPS_NOT_ONE \
{ \
	rate = FGET( (d * nRepetitions), D ); \
	if ( rate > UNITY ) \
	{ \
		rate--; \
		if ( SoftTransition && (HIWORD(rate) & 1) ) \
		{ \
			rate = UNITY - LOWORD(rate); \
		} \
		else \
		{ \
			rate--; \
			rate = LOWORD(rate); \
			rate++; \
		} \
	} \
}

#define QUICK_BOUND(in, out) \
{ \
	int incoming = in; \
	if (incoming <   0) incoming = 0; \
	if (incoming > 255) incoming = 255; \
	out = incoming; \
}

// !!! REMOVE AT YOUR OWN RISK !!!
// this module has probably optimization problems
#pragma optimize("",off)

/************************************************************************/

LPTR BuildMidPointTable(
	BOOL        bDoHSL,
	int         depth)
{
	LPTR lpNewTable;
	LPTR lpEntry;
	int  iTableEntries, i;
	int  iFirstHalf, iSecondHalf;
	int  start1, start2, start3, start4;
	int  end1,   end2,   end3,   end4;
	long range1, range2, range3, range4;
	COLORINFO startColor, endColor;
	FRMDATATYPE type;

	type = FrameType(ImgGetBaseEditFrame(lpImage));
	GetActiveColorFromType(type, &startColor);
	GetAlternateColorFromType(type, &endColor);

	switch( depth )
	{
		case 0 :
		case 1 :
			start1 = startColor.gray;
			end1   = endColor.gray;
			range1 = end1 - start1;

			iTableEntries = abs(range1);
		break;

		case 3 :
			if (bDoHSL)
			{
				start1 = startColor.hsl.hue;
				start2 = startColor.hsl.sat;
				start3 = startColor.hsl.lum;
				end1   = endColor.hsl.hue;
				end2   = endColor.hsl.sat;
				end3   = endColor.hsl.lum;
				range1 = end1 - start1;
				range2 = end2 - start2;
				range3 = end3 - start3;
			}
			else
			{
				start1 = startColor.rgb.red;
				start2 = startColor.rgb.green;
				start3 = startColor.rgb.blue;
				end1   = endColor.rgb.red;
				end2   = endColor.rgb.green;
				end3   = endColor.rgb.blue;
				range1 = end1 - start1;
				range2 = end2 - start2;
				range3 = end3 - start3;
			}

			iTableEntries = max(max(abs(range1), abs(range2)), abs(range3));
		break;

		case 4 :
			if (bDoHSL)
			{
				start1 = startColor.hsl.hue;
				start2 = startColor.hsl.sat;
				start3 = startColor.hsl.lum;
				start4 = 0;
				end1   = endColor.hsl.hue;
				end2   = endColor.hsl.sat;
				end3   = endColor.hsl.lum;
				end4   = 0;
				range1 = end1 - start1;
				range2 = end2 - start2;
				range3 = end3 - start3;
				range4 = 0;
			}
			else
			{
				start1 = startColor.cmyk.c;
				start2 = startColor.cmyk.m;
				start3 = startColor.cmyk.y;
				start4 = startColor.cmyk.k;
				end1   = endColor.cmyk.c;
				end2   = endColor.cmyk.m;
				end3   = endColor.cmyk.y;
				end4   = endColor.cmyk.k;
				range1 = end1 - start1;
				range2 = end2 - start2;
				range3 = end3 - start3;
				range4 = end4 - start4;
			}

			iTableEntries = max(max(abs(range1), abs(range2)), max(abs(range3), abs(range4)));
		break;
	}

	iTableEntries = 4096;

	if (iTableEntries < 2)
		iTableEntries = 2;

	lpNewTable = Alloc( (iTableEntries+4) * (long)depth );

	if (!lpNewTable)
		return(NULL);

	iFirstHalf = ((long)Vignette.Midpoint * iTableEntries) >> 8;

	if (iFirstHalf <= 0)
		iFirstHalf = 1;

	if (iFirstHalf >= iTableEntries)
		iFirstHalf = iTableEntries-1;

	iSecondHalf = iTableEntries - iFirstHalf;

	lpEntry = lpNewTable;

	switch(depth)
	{
		case 0 :
		case 1 :
		{
			*lpEntry++ = start1;

			for(i=1; i < iFirstHalf; i++)
			{
				*lpEntry++ = start1 + (i * range1)/(iFirstHalf*2);
			}

			for(i=0; i < iSecondHalf-1; i++)
			{
				*lpEntry++ = start1 + (range1/2) + (i * range1)/(iSecondHalf*2);
			}

			*lpEntry++ = end1;
		}
		break;

		case 3 :
		{
			*lpEntry++ = start1;
			*lpEntry++ = start2;
			*lpEntry++ = start3;

			for(i=1; i < iFirstHalf; i++)
			{
				*lpEntry++ = start1 + (i * range1)/(iFirstHalf*2);
				*lpEntry++ = start2 + (i * range2)/(iFirstHalf*2);
				*lpEntry++ = start3 + (i * range3)/(iFirstHalf*2);
			}

			for(i=0; i < iSecondHalf-1; i++)
			{
				*lpEntry++ = start1 + (range1/2) + (i * range1)/(iSecondHalf*2);
				*lpEntry++ = start2 + (range2/2) + (i * range2)/(iSecondHalf*2);
				*lpEntry++ = start3 + (range3/2) + (i * range3)/(iSecondHalf*2);
			}

			*lpEntry++ = end1;
			*lpEntry++ = end2;
			*lpEntry++ = end3;

			if (bDoHSL)
				HSLtoRGB24( (LPRGB)lpNewTable, iTableEntries );
		}
		break;

		case 4 :
		{
			*lpEntry++ = start1;
			*lpEntry++ = start2;
			*lpEntry++ = start3;
			*lpEntry++ = start4;

			for(i=1; i < iFirstHalf; i++)
			{
				*lpEntry++ = start1 + (i * range1)/(iFirstHalf*2);
				*lpEntry++ = start2 + (i * range2)/(iFirstHalf*2);
				*lpEntry++ = start3 + (i * range3)/(iFirstHalf*2);
				*lpEntry++ = start4 + (i * range4)/(iFirstHalf*2);
			}

			for(i=0; i < iSecondHalf-1; i++)
			{
				*lpEntry++ = start1 + (range1/2) + (i * range1)/(iSecondHalf*2);
				*lpEntry++ = start2 + (range2/2) + (i * range2)/(iSecondHalf*2);
				*lpEntry++ = start3 + (range3/2) + (i * range3)/(iSecondHalf*2);
				*lpEntry++ = start4 + (range4/2) + (i * range4)/(iSecondHalf*2);
			}

			*lpEntry++ = end1;
			*lpEntry++ = end2;
			*lpEntry++ = end3;
			*lpEntry++ = end4;

			if (bDoHSL)
				HSLtoCMYK32( (LPCMYK)lpNewTable, iTableEntries );
		}
		break;
	}

	lTableRange = iTableEntries-1;

	return(lpNewTable);
}

/************************************************************************/

void DoVignette( LPIMAGE lpImage, int sx1, int sy1, int sx2, int sy2 )
{
	LPFRAME lpFrame;
	RECT rMask;
	int dx, dy, depth;
	int index, prev, next, pi, ni;
	long ldx, ldy, x, y, xs, xe, ys, ye, asqrd, bsqrd, r;
	LPDATAPROC lpVignetteProc;
	ENGINE Engine;
	BOOL   DoHSL;

	if (!(lpFrame = ImgGetBaseEditFrame(lpImage)))
		return;

	x1 = sx1; y1 = sy1; x2 = sx2; y2 = sy2;
	dx = x2 - x1;
	dy = y2 - y1;

	if (Vignette.Gradient == IDC_VIGLINEAR || Vignette.Gradient == IDC_VIGRADIAL)
	{
		if (abs(dx) <= 3 && abs(dy) <= 3)
			return;
	}
	else
	{
		if (abs(dx) <= 3 || abs(dy) <= 3)
			return;
	}

	xc = (x1 + x2) / 2;
	yc = (y1 + y2) / 2;

	if ( (nRepetitions = Vignette.RepeatCount) <= 0 )
		nRepetitions = 1;

	depth = FrameDepth(lpFrame);
	if (depth == 0) depth = 1;

	DoHSL = (Vignette.VigColorModel+IDC_FIRST_MODEL) != IDC_MODEL_RGB &&
				(depth != 1);

	switch (Vignette.Gradient)
	{
		case IDC_VIGLINEAR:
			D = lsqrt(((long)dx*(long)dx)+((long)dy*(long)dy));
			sine = FGET(-dy, D);
			cosine = FGET(dx, D);
			xr = rotx(x2, y2, x1, y1, cosine, sine);
			lpVignetteProc = linear_vignette_proc;
		break;

		case IDC_VIGRADIAL:
			D = lsqrt(((long)dx*(long)dx)+((long)dy*(long)dy));
			lpVignetteProc = radial_vignette_proc;
		break;

		case IDC_VIGCIRCLE:
			x1 = xc;
			y1 = yc;
			y2 = yc;
			dx = x2 - x1;
			dy = y2 - y1;
			if (!dx && !dy)
				return;
			D = lsqrt(((long)dx*(long)dx)+((long)dy*(long)dy));
			lpVignetteProc = radial_vignette_proc;
		break;

		case IDC_VIGSQUARE:
		case IDC_VIGRECTANGLE:
			ymin = min(y1, y2);
			ymax = max(y1, y2);
			xmin = min(x1, x2);
			xmax = max(x1, x2);

			ldx = xmin-xc;		// upper left
			ldy = ymin-yc;
			m1 = (256L * ldy) / ldx;
			b1 = ymin - ((m1 * xmin)/256L);
			D1 = lsqrt((ldx*ldx)+(ldy*ldy));

			ldx = xmax-xc;		// upper right
			ldy = ymin-yc;
			m2 = (256L * ldy) / ldx;
			b2 = ymin - ((m2 * xmax)/256L);
			D2 = lsqrt((ldx*ldx)+(ldy*ldy));

			ldx = xmax-xc;		// lower right
			ldy = ymax-yc;
			m3 = (256L * ldy) / ldx;
			b3 = ymax - ((m3 * xmax)/256L);
			D3 = lsqrt((ldx*ldx)+(ldy*ldy));

			ldx = xmin-xc;		// lower left
			ldy = ymax-yc;
			m4 = (256L * ldy) / ldx;
			b4 = ymax - ((m4 * xmin)/256L);
			D4 = lsqrt((ldx*ldx)+(ldy*ldy));

			lpVignetteProc = rectangle_vignette_proc;
		break;

		case IDC_VIGELLIPSE:
			if ( !(lpD = (LPINT)Alloc((long)sizeof(int)*(TSIZE+1))) )
			{
				Message(IDS_EMEMALLOC);
				return;
			}
			set16((LPWORD)lpD, TSIZE+1, -1);
			ymin = min(y1, y2);
			ymax = max(y1, y2);
			xmin = min(x1, x2);
			xmax = max(x1, x2);
			ea = dx/2;
			eb = dy/2;
			if (!ea || !eb)
			{
				FreeUp((LPTR)lpD);
				return;
			}
			asqrd = ea*ea;
			bsqrd = eb*eb;

			// fill in a table with radius information for the
			// ellipse.  The radius for a given point would be
			// starting from the center of the ellipse, going 
			// through to point, and where it intersects the 
			// edge of the ellipse.  We need the radius for
			// the D value used in the ellipse_proc, which is
			// the maximum distance used for determining how to
			// calculate the gradient, which is d/D.  d is the 
			// distance of the point from the center, D is extracted
			// from the table built below.  The index of the table is
			// formed from the ratio of sides of the triangle formed.
			// This is like looking up the angle to see where the
			// point would intersect the circle.  But we calculate
			// the radii ahead of time to speed things up.
			if (ea > eb)	// step in x
			{
				xs = xc - xc;
				xe = xmax - xc;
				for (x = xs; x <= xe; ++x)
				{
					y = ((eb*(long)lsqrt(asqrd - (x*x)))+(ea/2))/ea;
					r = (x*x)+(y*y);
					if (r <= 0)
						r = 1;
					index = ((x * x * (long)TSIZE)+(r/2)) / r;
					index = bound(abs(index), 0, TSIZE);
					lpD[index] = lsqrt(r);
				}
			}
			else		// step in y
			{
				ys = yc - yc;
				ye = ymax - yc;
				for (y = ys; y <= ye; ++y)
				{
					x = ((ea*(long)lsqrt(bsqrd - (y*y)))+(eb/2))/eb;
					r = (x*x)+(y*y);
					if (r <= 0)
						r = 1;
					index = ((y * y * (long)TSIZE)+(r/2)) / r;
					index = bound(abs(index), 0, TSIZE);
					lpD[index] = lsqrt(r);
				}
			}
			// find the first valid entry in our table
			for (index = 0; index <= TSIZE && lpD[index] < 0; ++index)
				;

			// see if we have any entries
			if (index > TSIZE)
			{
				FreeUp((LPTR)lpD);
				return;
			}

			// fill in all entries before first with value of first
			while (--index >= 0)
				lpD[index] = lpD[index+1];

			// find last valid entry in table
			for (index = TSIZE; index >= 0 && lpD[index] < 0; --index)
				;

			// see if we have any entries
			if (index < 0)
			{
				FreeUp((LPTR)lpD);
				return;
			}

			// fill in all entries after last with value of last
			while (++index <= TSIZE)
				lpD[index] = lpD[index-1];

			// interpolate values of all empty cells
			for (index = 0; index <= TSIZE; ++index)
			{
				if (lpD[index] < 0)
				{
					pi = index - 1;
					prev = lpD[pi];
					ni = index;
					while (lpD[ni] < 0)
						++ni;
					next = lpD[ni];
					// remember here that (index-pi) == 1
					lpD[index] = prev + ((next-prev)/(ni-pi));
				}
			}
			lpVignetteProc = ellipse_vignette_proc;
		break;

		default:
			return;
		break;
	}

	switch(depth)
	{
		case 0 :
		case 1 :
			lpProcessProc = (LPPROCESSVIGNETTE)ProcessVignette8;
		break;

		case 3 :
			lpProcessProc = (LPPROCESSVIGNETTE)ProcessVignette24;
		break;

		case 4 :
			lpProcessProc = (LPPROCESSVIGNETTE)ProcessVignette32;
		break;
	}

	lpMidpointTable = BuildMidPointTable( DoHSL, depth );

	ImgGetMaskRect( lpImage, &rMask );
	lplD = (LPLONG)Alloc((long)sizeof(long)*(long)RectWidth(&rMask));
	lpld = (LPLONG)Alloc((long)sizeof(long)*(long)RectWidth(&rMask));

	if (!lpld || !lplD || !lpMidpointTable)
	{
		if (Vignette.Gradient == IDC_VIGELLIPSE)
			FreeUp((LPTR)lpD);
		if (lplD)
			FreeUp((LPTR)lplD);
		if (lpld)
			FreeUp((LPTR)lpld);
		if (lpMidpointTable)
			FreeUp((LPTR)lpMidpointTable);
		return;
	}

	SetEngineDraw(&Engine,lpVignetteProc,Vignette.VigOpacity,Vignette.VigMergeMode);
	LineEngineSelObj(lpImage,&Engine,IDS_UNDOCHANGE);

	FreeUp((LPTR)lpld);
	FreeUp((LPTR)lplD);
	FreeUp((LPTR)lpMidpointTable);

	if (Vignette.Gradient == IDC_VIGELLIPSE)
		FreeUp((LPTR)lpD);
}


/************************************************************************/

BOOL TintFill(
	LPIMAGE     lpImage,
	LPCOLORINFO lpColor,
	int         iOpacity,
	MERGE_MODE  MergeMode,
	int         dirty )
{
	ENGINE Engine;

	SetEngineColor(&Engine, lpColor, iOpacity, MergeMode);
	return(LineEngineSelObj(lpImage,&Engine,dirty));
}

/************************************************************************/

BOOL TextureFill(
	LPIMAGE lpImage,
	LPFRAME lpTexture,
	BOOL    fHorzFlip,
	BOOL    fVertFlip )
{
	ENGINE Engine;

	if ( !lpTexture )
		return( FALSE );

	SetEngineOld(&Engine, NO, NULL, NULL, lpTexture, fHorzFlip, fVertFlip,
		NULL, NULL, Texture.TextureOpacity, Texture.TextureMergeMode);
	return(LineEngineSelObj(lpImage,&Engine,IDS_UNDOCHANGE));
}

/************************************************************************/

BOOL InvertFill(LPIMAGE lpImage)
{
	ENGINE Engine;

	SetEngineDef(&Engine);
	Engine.lpDataProc = invert_proc;
	return(LineEngineSelObj(lpImage,&Engine,IDS_UNDOCHANGE));
}

/************************************************************************/

static void HSLtoRGB24(
	LPRGB lpSrc,
	int   iCount)
{
	while (--iCount >= 0)
	{
		HSLtoRGB(lpSrc->red, lpSrc->green, lpSrc->blue, lpSrc);
		++lpSrc;
	}
}

/************************************************************************/

static void HSLtoCMYK32(LPCMYK lpSrc, int iCount)
{
	while (--iCount >= 0)
	{
		HSLtoCMYK(lpSrc->c, lpSrc->m, lpSrc->y, lpSrc);
		++lpSrc;
	}
}

/************************************************************************/

static void invert_proc(
int 	y,
int		left,
int		right,
LPTR 	lpDst,
LPTR	lpSrc,
int 	depth)
{
	int iCount;

	if (depth == 4)
	{
		LPCMYK lpCMYKSrc = (LPCMYK)lpSrc;
		LPCMYK lpCMYKDst = (LPCMYK)lpDst;
		RGBS rgb;

		iCount = (right - left + 1);

		while (--iCount >= 0)
		{
			CMYKtoRGB(
				lpCMYKDst->c,
				lpCMYKDst->m,
				lpCMYKDst->y,
				lpCMYKDst->k,
				&rgb);

			RGBtoCMYK( (BYTE)(rgb.red^0xFF), (BYTE)(rgb.green^0xFF),
				(BYTE)(rgb.blue^0xFF), lpCMYKSrc );

			lpCMYKSrc++;
			lpCMYKDst++;
		}
	}
	else
	{
		iCount = (right - left + 1) * depth;

		while (--iCount >= 0)
		{
			*lpSrc++ = (*lpDst++)^0xFF;
		}
	}
}

/************************************************************************/

static void ProcessVignette8(
	LPTR   lpSrc,
	int    iCount,
	LPLONG lpd,
	LPLONG lpD,
	int    nRepetitions,
	BOOL   SoftTransition)
{
	long d, D;
	int  r;
	LFIXED rate;
	LPTR   lpMyMidPoint = lpMidpointTable;
	int    iTableIndex;

	if (nRepetitions == 1)
	{
		while (iCount-- > 0)
		{
			DO_RANDOM;

			REPS_EQUAL_ONE;

			iTableIndex = (long)(rate*lTableRange) >> 16;

			QUICK_BOUND(lpMyMidPoint[iTableIndex] + r, *lpSrc);

			lpSrc++;
		}
	}
	else
	{
		while (iCount-- > 0)
		{
			DO_RANDOM;

			REPS_NOT_ONE;

			iTableIndex = (long)(rate*lTableRange) >> 16;

			QUICK_BOUND(lpMyMidPoint[iTableIndex] + r, *lpSrc);

			lpSrc++;
		}
	}
}

/************************************************************************/

static void ProcessVignette24(
	LPRGB  lpSrc,
	int    iCount,
	LPLONG lpd,
	LPLONG lpD,
	int    nRepetitions,
	BOOL   SoftTransition)
{
	long d, D;
	int  r;
	LFIXED rate;
	LPRGB  lpMyMidPoint = (LPRGB)lpMidpointTable;
	int    iTableIndex;

	if (nRepetitions == 1)
	{
		while (iCount-- > 0)
		{
			DO_RANDOM;

			REPS_EQUAL_ONE;

			iTableIndex = (long)(rate * lTableRange) >> 16;

			if (r)
			{
				QUICK_BOUND(lpMyMidPoint[iTableIndex].red   + r, lpSrc->red);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[iTableIndex].green + r, lpSrc->green);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[iTableIndex].blue  + r, lpSrc->blue);
			}
			else
			{
				QUICK_BOUND(lpMyMidPoint[iTableIndex].red  , lpSrc->red);
				QUICK_BOUND(lpMyMidPoint[iTableIndex].green, lpSrc->green);
				QUICK_BOUND(lpMyMidPoint[iTableIndex].blue , lpSrc->blue);
			}

			lpSrc++;
		}
	}
	else
	{
		while (iCount-- > 0)
		{
			DO_RANDOM;

			REPS_NOT_ONE;

			iTableIndex = (long)(rate*lTableRange)>>16;

			if (r)
			{
				QUICK_BOUND(lpMyMidPoint[iTableIndex].red   + r, lpSrc->red);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[iTableIndex].green + r, lpSrc->green);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[iTableIndex].blue  + r, lpSrc->blue);
			}
			else
			{
				QUICK_BOUND(lpMyMidPoint[iTableIndex].red  , lpSrc->red);
				QUICK_BOUND(lpMyMidPoint[iTableIndex].green, lpSrc->green);
				QUICK_BOUND(lpMyMidPoint[iTableIndex].blue , lpSrc->blue);
			}

			lpSrc++;
		}
	}
}

/************************************************************************/

static void ProcessVignette32(
	LPCMYK lpSrc,
	int    iCount,
	LPLONG lpd,
	LPLONG lpD,
	int    nRepetitions,
	BOOL   SoftTransition)
{
	long   d, D;
	int    r;
	LFIXED rate;
	LPCMYK lpMyMidPoint = (LPCMYK)lpMidpointTable;
	int    iTableIndex;

	if (nRepetitions == 1)
	{
		while (iCount-- > 0)
		{
			DO_RANDOM;

			REPS_EQUAL_ONE;

			iTableIndex = (long)(rate*lTableRange) >> 16;

			if (r)
			{
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].c + r, lpSrc->c);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].m + r, lpSrc->m);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].y + r, lpSrc->y);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].k + r, lpSrc->k);
			}
			else
			{
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].c, lpSrc->c);
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].m, lpSrc->m);
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].y, lpSrc->y);
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].k, lpSrc->k);
			}

			lpSrc++;
		}
	}
	else
	{
		while (iCount-- > 0)
		{
			DO_RANDOM;

			REPS_NOT_ONE;

			iTableIndex = (long)(rate*lTableRange) >> 16;

			if (r)
			{
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].c + r, lpSrc->c);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].m + r, lpSrc->m);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].y + r, lpSrc->y);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].k + r, lpSrc->k);
			}
			else
			{
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].c, lpSrc->c);
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].m, lpSrc->m);
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].y, lpSrc->y);
				QUICK_BOUND(lpMyMidPoint[ iTableIndex ].k, lpSrc->k);
			}

			lpSrc++;
		}
	}
}

/************************************************************************/

static void ellipse_vignette_proc(
int 	y,
int		left,
int		right,
LPTR 	lpDst,
LPTR 	lpSrc,
int 	depth)
{
	int xn, index;
	LPLONG lptld, lptlD;
	long dx, dy, d, dx2, dy2;

	dy = y - yc;
	lptld = lpld;
	lptlD = lplD;

	for (xn = left; xn <= right; ++xn)
	{
		dx = xn - xc;
		dx2 = dx*dx;
		dy2 = dy*dy;
		d = dx2 + dy2;
		*lptld++ = lsqrt(d);
		if (d > 0)
		{
			if (ea > eb)
				index = ((dx2 * (long)TSIZE)+(d/2)) / d;
			else
				index = ((dy2 * (long)TSIZE)+(d/2)) / d;
			index = bound(abs(index), 0, TSIZE);
			*lptlD++ = lpD[index];
		}
		else
			*lptlD++ = 1;
	}

	(*lpProcessProc)(lpSrc, right-left+1, lpld, lplD, nRepetitions, Vignette.SoftTransition);
}

/************************************************************************/

static void rectangle_vignette_proc(
int 	the_y,
int		left,
int		right,
LPTR 	lpDst,
LPTR 	lpSrc,
int 	depth)
{
	int xn, px, py, the_x, y;
	long xc2, yc2, dy, m, ldx, ldy;
	LPLONG lptld, lptlD;

	xc2 = 2L * xc;
	yc2 = 2L * yc;

	if (the_y > yc)  // bottom side
		y = yc2 - the_y;  // flip y over to top side
	else
		y = the_y;

	dy = y - yc;
	lptld = lpld;
	lptlD = lplD;

	for (the_x = left; the_x <= right; ++the_x)
	{
		if (the_x > xc)   // right side
			xn = xc2 - the_x;   // flip x over to left side
		else
			xn = the_x;

		if (xn < xmin || xn > xmax || y < ymin || y > ymax)
		{
			*lptld++ = 2;
			*lptlD++ = 1;
			continue;
		}

		if (xn > xc)		// right side
		{
			m = (256L * dy) / (xn-xc);
			if (y > yc)	// bottom-right corner
			{
				*lptlD++ = D3;
				if (m < m3)
				{
					px = xn;
					py = (((m3 * px)+128L)/256L)+b3;
				}
				else
				{
					py = y;
					px = (((py-b3)*256L)+128L)/m3;
				}
			}
			else		// top-right corner
			{
				*lptlD++ = D2;
				if (m > m2)
				{
					px = xn;
					py = (((m2 * px)+128L)/256L)+b2;
				}
				else
				{
					py = y;
					px = (((py-b2)*256L)+128L)/m2;
				}
			}
		}
		else if (xn < xc)	// left side
		{
			m = (256L * dy) / (xn-xc);
			if (y > yc)	// bottom-left corner
			{
				*lptlD++ = D4;
				if (m > m4)
				{
					px = xn;
					py = (((m4 * px)+128L)/256L)+b4;
				}
				else
				{
					py = y;
					px = (((py-b4)*256L)+128L)/m4;
				}
			}
			else		// top-left corner
			{
				*lptlD++ = D1;
				if (m < m1)
				{
					px = xn;
					py = (((m1 * px)+128L)/256L)+b1;
				}
				else
				{
					py = y;
					px = (((py-b1)*256L)+128L)/m1;
				}
			}
		}
		else	// x = xc - infinite slope
		{
			if (y > yc)
			{
				*lptlD++ = D3;
				py = y;
				px = (((py-b3)*256L)+128L)/m3;
			}
			else
			{
				*lptlD++ = D1;
				py = y;
				px = (((py-b1)*256L)+128L)/m1;
			}
		}
		ldx = px - xc;
		ldy = py - yc;
		*lptld++ = lsqrt((ldx*ldx)+(ldy*ldy));
	}
	(*lpProcessProc)(lpSrc, right-left+1, lpld, lplD, nRepetitions, Vignette.SoftTransition);
}

/************************************************************************/

static void linear_vignette_proc(
int 	y,
int		left,
int		right,
LPTR 	lpDst,
LPTR 	lpSrc,
int 	depth)
{
	int xn;
	LPLONG lptld, lptlD;

	lptld = lpld;
	lptlD = lplD;

	for (xn = left; xn <= right; ++xn)
	{
		*lptld++ = rotx(xn, y, x1, y1, cosine, sine);
		*lptlD++ = D;
	}
	(*lpProcessProc)(lpSrc, right-left+1, lpld, lplD, nRepetitions, Vignette.SoftTransition);
}

/************************************************************************/

static void radial_vignette_proc(
int 	y,
int		left,
int		right,
LPTR 	lpDst,
LPTR 	lpSrc,
int 	depth)
{
	int xn, tdx, tdy;
	LPLONG lptld, lptlD;

	tdy = y - y1;
	lptld = lpld;
	lptlD = lplD;

	for (xn = left; xn <= right; ++xn, ++lptld, ++lptlD)
	{
		tdx = xn - x1;
		*lptld = lsqrt(((long)tdx*(long)tdx)+((long)tdy*(long)tdy));
		*lptlD = D;
	}
	(*lpProcessProc)(lpSrc, right-left+1, lpld, lplD, nRepetitions, Vignette.SoftTransition);
}

/************************************************************************/


