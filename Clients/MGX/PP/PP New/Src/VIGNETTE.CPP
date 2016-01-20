//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

#define RAND_BITS 7
#define RAND_SUB 3   /* changed to 3 cause no rounding up, should be (RAND_BITS+1)/2 */
#define rotx(x, y, xc, yc, cosine, sine) ((cosine * (x - xc)) + (sine * (yc - y)))
#define TSIZE	1000

// leave this static cause it's supposed to be random anyway
static	ULONG lSeed = 0xDEADBABA;

typedef void (FAR *LPVIGPROC)(LPTR lpSrc, int iCount, struct _gradient_data FAR *lpGrad);

typedef struct _gradient_data
	{
	int xr, nRepetitions, x1, y1, x2, y2, xc, yc, xmin, xmax, ymin, ymax;
	long D, xD, yD, m1, m2, m3, m4, b1, b2, b3, b4, D1, D2, D3, D4, ea, eb;
	LFIXED sine, cosine;
	LPINT lpD;
	LPLONG lplD, lpld;
	LPVIGPROC lpProcessProc;
	LPTR lpMidpointTable;
	long lTableRange;
	BOOL SoftTransition;
	LPTR lpPaletteLUT;
	} GRADIENT_DATA, FAR *LPGRADIENT_DATA;

// static functions
static void HSLtoRGB24(LPRGB lpSrc, int iCount);
static void HSLtoCMYK32(LPCMYK lpSrc, int iCount);
static LPTR BuildMidPointTable(
	BOOL        bDoHSL,
	FRMDATATYPE Type,
	int			Midpoint,
	LPCOLORINFO lpStartColor,
	LPCOLORINFO lpEndColor,
	LPGRADIENT_DATA lpGrad);

static void ProcessVignette8(
	LPTR   lpSrc,
	int    iCount,
	LPGRADIENT_DATA lpGrad);

static void ProcessVignette8P(
	LPTR   lpSrc,
	int    iCount,
	LPGRADIENT_DATA lpGrad);

static void ProcessVignette24(
	LPRGB  lpSrc,
	int    iCount,
	LPGRADIENT_DATA lpGrad);

static void ProcessVignette32(
	LPCMYK lpSrc,
	int    iCount,
	LPGRADIENT_DATA lpGrad);

static void ellipse_vignette_proc(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth, LPGRADIENT_DATA lpGrad );
static void rectangle_vignette_proc(int the_y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth, LPGRADIENT_DATA lpGrad );
static void linear_vignette_proc(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth, LPGRADIENT_DATA lpGrad );
static void radial_vignette_proc(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth, LPGRADIENT_DATA lpGrad );
static void invert_proc(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth, LPVOID lpParam );

#define DO_RANDOM \
{ \
	d = *lpld++; \
	D = *lplD++; \
	if ( d < 0) \
	{ \
		r = 0; \
		d = 0; \
	} \
	else if ( d > D ) \
	{ \
		r = 0; \
		d = D; \
	} \
	else if (Vignette.DisableDither) \
		r = 0;	\
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
	if (Vignette.DisableDither) \
		r = 0;	\
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

#define REPS_EQUAL_ONE \
{ \
	rate = FGET(d,D); \
}

#define REPS_NOT_ONE \
{ \
	rate = FGET( (d * nRepetitions), D ); \
	if ( rate > FUNITY ) \
	{ \
		rate--; \
		if ( SoftTransition && (WHOLE(rate) & 1) ) \
		{ \
         rate = FUNITY - FRACTION(rate); \
		} \
		else \
		{ \
			rate--; \
			rate = FRACTION(rate); \
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

/************************************************************************/

#pragma optimize( "", off)
static LPTR BuildMidPointTable(
	BOOL        bDoHSL,
	FRMDATATYPE Type,
	int			Midpoint,
	LPCOLORINFO lpStartColor,
	LPCOLORINFO lpEndColor,
	LPGRADIENT_DATA lpGrad)
{
	LPTR lpNewTable;
	LPTR lpEntry;
	int  iTableEntries, i, depth;
	int  iFirstHalf, iSecondHalf;
	int  start1, start2, start3, start4;
	int  end1,   end2,   end3,   end4;
	long range1, range2, range3, range4;

	switch( Type )
	{
		case FDT_LINEART :
		case FDT_GRAYSCALE :
			depth = 1;
			start1 = lpStartColor->gray;
			end1   = lpEndColor->gray;
			range1 = end1 - start1;

			iTableEntries = abs(range1);
		break;

		case FDT_PALETTECOLOR :
		case FDT_RGBCOLOR :
			depth = 3;
			if (bDoHSL)
			{
				start1 = lpStartColor->hsl.hue;
				start2 = lpStartColor->hsl.sat;
				start3 = lpStartColor->hsl.lum;
				end1   = lpEndColor->hsl.hue;
				end2   = lpEndColor->hsl.sat;
				end3   = lpEndColor->hsl.lum;
				range1 = end1 - start1;
				range2 = end2 - start2;
				range3 = end3 - start3;
			}
			else
			{
				start1 = lpStartColor->rgb.red;
				start2 = lpStartColor->rgb.green;
				start3 = lpStartColor->rgb.blue;
				end1   = lpEndColor->rgb.red;
				end2   = lpEndColor->rgb.green;
				end3   = lpEndColor->rgb.blue;
				range1 = end1 - start1;
				range2 = end2 - start2;
				range3 = end3 - start3;
			}

			iTableEntries = max(max(abs(range1), abs(range2)), abs(range3));
		break;

		case FDT_CMYKCOLOR :
			depth = 4;
			if (bDoHSL)
			{
				start1 = lpStartColor->hsl.hue;
				start2 = lpStartColor->hsl.sat;
				start3 = lpStartColor->hsl.lum;
				start4 = 0;
				end1   = lpEndColor->hsl.hue;
				end2   = lpEndColor->hsl.sat;
				end3   = lpEndColor->hsl.lum;
				end4   = 0;
				range1 = end1 - start1;
				range2 = end2 - start2;
				range3 = end3 - start3;
				range4 = 0;
			}
			else
			{
				start1 = lpStartColor->cmyk.c;
				start2 = lpStartColor->cmyk.m;
				start3 = lpStartColor->cmyk.y;
				start4 = lpStartColor->cmyk.k;
				end1   = lpEndColor->cmyk.c;
				end2   = lpEndColor->cmyk.m;
				end3   = lpEndColor->cmyk.y;
				end4   = lpEndColor->cmyk.k;
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

	iFirstHalf = ((long)Midpoint * iTableEntries) >> 8;

	if (iFirstHalf <= 0)
		iFirstHalf = 1;

	if (iFirstHalf >= iTableEntries)
		iFirstHalf = iTableEntries-1;

	iSecondHalf = iTableEntries - iFirstHalf;

	lpEntry = lpNewTable;

	switch(Type)
	{
		case FDT_LINEART :
		case FDT_GRAYSCALE :
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

		case FDT_PALETTECOLOR :
		case FDT_RGBCOLOR :
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

		case FDT_CMYKCOLOR :
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

	lpGrad->lTableRange = iTableEntries-1;

	return(lpNewTable);
}
#pragma optimize( "", on)

/************************************************************************/

BOOL GradientImage( LPIMAGE lpImage, LPGRADIENT_PARMS lpParms)
{
	LPFRAME lpFrame;
	RECT rMask;
	LPINT lpD;
	FRMTYPEINFO TypeInfo;
	int dx, dy, iCount;
	int index, prev, next, pi, ni;
	long ldx, ldy, x, y, xs, xe, ys, ye, asqrd, bsqrd, r;
	LPDATAPROC lpVignetteProc;
	ENGINE Engine;
	BOOL   DoHSL;
	GRADIENT_DATA data;
	int res;

   FRMDATATYPE type;
	ImgGetInfo(lpImage, NULL, NULL, NULL, &type);
	if (type == FDT_LINEART)
		return(FALSE);
	if (!(lpFrame = ImgGetBaseEditFrame(lpImage)))
		return(FALSE);
	res = FrameResolution(lpFrame);
	data.x1 = lpParms->x1;
	data.y1 = lpParms->y1;
	data.x2 = lpParms->x2;
	data.y2 = lpParms->y2;
	ResConvertUL(lpParms->iBaseRes, res, &data.x1, &data.y1);
	ResConvertLR(lpParms->iBaseRes, res, &data.x2, &data.y2);
	dx = data.x2 - data.x1;
	dy = data.y2 - data.y1;
	data.SoftTransition = lpParms->SoftTransition;
	data.lpPaletteLUT = NULL;

	if (lpParms->Gradient == IDC_VIGLINEAR || lpParms->Gradient == IDC_VIGRADIAL)
	{
		if (abs(dx) <= 3 && abs(dy) <= 3)
			return(FALSE);
	}
	else
	{
		if (abs(dx) <= 3 || abs(dy) <= 3)
			return(FALSE);
	}

	data.xc = (data.x1 + data.x2) / 2;
	data.yc = (data.y1 + data.y2) / 2;

	if ( (data.nRepetitions = lpParms->RepeatCount) <= 0 )
		data.nRepetitions = 1;

	FrameGetTypeInfo(lpFrame, &TypeInfo);

	DoHSL = (lpParms->VigColorModel+IDC_FIRST_MODEL) != IDC_MODEL_RGB &&
				(TypeInfo.DataType > FDT_GRAYSCALE);

	switch (lpParms->Gradient)
	{
		case IDC_VIGLINEAR:
			data.D = lsqrt(((long)dx*(long)dx)+((long)dy*(long)dy));
			data.sine = FGET(-dy, data.D);
			data.cosine = FGET(dx, data.D);
			data.xr = WHOLE(( rotx(data.x2, data.y2, data.x1, data.y1,
            data.cosine, data.sine) ));
			lpVignetteProc = (LPDATAPROC)linear_vignette_proc;
		break;

		case IDC_VIGRADIAL:
			data.D = lsqrt(((long)dx*(long)dx)+((long)dy*(long)dy));
			lpVignetteProc = (LPDATAPROC)radial_vignette_proc;
		break;

		case IDC_VIGCIRCLE:
			data.x1 = data.xc;
			data.y1 = data.yc;
			data.y2 = data.yc;
			dx = data.x2 - data.x1;
			dy = data.y2 - data.y1;
			if (!dx && !dy)
				return(FALSE);
			data.D = lsqrt(((long)dx*(long)dx)+((long)dy*(long)dy));
			lpVignetteProc = (LPDATAPROC)radial_vignette_proc;
		break;

		case IDC_VIGSQUARE:
		case IDC_VIGRECTANGLE:
			data.ymin = min(data.y1, data.y2);
			data.ymax = max(data.y1, data.y2);
			data.xmin = min(data.x1, data.x2);
			data.xmax = max(data.x1, data.x2);

			ldx = data.xmin-data.xc;		// upper left
			ldy = data.ymin-data.yc;
			data.m1 = (256L * ldy) / ldx;
			data.b1 = data.ymin - ((data.m1 * data.xmin)/256L);
			data.D1 = lsqrt((ldx*ldx)+(ldy*ldy));

			ldx = data.xmax-data.xc;		// upper right
			ldy = data.ymin-data.yc;
			data.m2 = (256L * ldy) / ldx;
			data.b2 = data.ymin - ((data.m2 * data.xmax)/256L);
			data.D2 = lsqrt((ldx*ldx)+(ldy*ldy));

			ldx = data.xmax-data.xc;		// lower right
			ldy = data.ymax-data.yc;
			data.m3 = (256L * ldy) / ldx;
			data.b3 = data.ymax - ((data.m3 * data.xmax)/256L);
			data.D3 = lsqrt((ldx*ldx)+(ldy*ldy));

			ldx = data.xmin-data.xc;		// lower left
			ldy = data.ymax-data.yc;
			data.m4 = (256L * ldy) / ldx;
			data.b4 = data.ymax - ((data.m4 * data.xmin)/256L);
			data.D4 = lsqrt((ldx*ldx)+(ldy*ldy));

			lpVignetteProc = (LPDATAPROC)rectangle_vignette_proc;
		break;

		case IDC_VIGELLIPSE:
			if ( !(data.lpD = (LPINT)Alloc((long)sizeof(int)*(TSIZE+1))) )
			{
				Message(IDS_EMEMALLOC);
				return(FALSE);
			}
			iCount = TSIZE+1;
			lpD = data.lpD;
			while (--iCount >= 0)
				*lpD++ = -1;				
			data.ymin = min(data.y1, data.y2);
			data.ymax = max(data.y1, data.y2);
			data.xmin = min(data.x1, data.x2);
			data.xmax = max(data.x1, data.x2);
			data.ea = dx/2;
			data.eb = dy/2;
			if (!data.ea || !data.eb)
			{
				FreeUp((LPTR)data.lpD);
				return(FALSE);
			}
			asqrd = data.ea*data.ea;
			bsqrd = data.eb*data.eb;

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
			if (data.ea > data.eb)	// step in x
			{
				xs = data.xc - data.xc;
				xe = data.xmax - data.xc;
				for (x = xs; x <= xe; ++x)
				{
					y = ((data.eb*(long)lsqrt(asqrd - (x*x)))+(data.ea/2))/data.ea;
					r = (x*x)+(y*y);
					if (r <= 0)
						r = 1;
					index = ((x * x * (long)TSIZE)+(r/2)) / r;
					index = bound(abs(index), 0, TSIZE);
					data.lpD[index] = lsqrt(r);
				}
			}
			else		// step in y
			{
				ys = data.yc - data.yc;
				ye = data.ymax - data.yc;
				for (y = ys; y <= ye; ++y)
				{
					x = ((data.ea*(long)lsqrt(bsqrd - (y*y)))+(data.eb/2))/data.eb;
					r = (x*x)+(y*y);
					if (r <= 0)
						r = 1;
					index = ((y * y * (long)TSIZE)+(r/2)) / r;
					index = bound(abs(index), 0, TSIZE);
					data.lpD[index] = lsqrt(r);
				}
			}
			// find the first valid entry in our table
			for (index = 0; index <= TSIZE && data.lpD[index] < 0; ++index)
				;

			// see if we have any entries
			if (index > TSIZE)
			{
				FreeUp((LPTR)data.lpD);
				return(FALSE);
			}

			// fill in all entries before first with value of first
			while (--index >= 0)
				data.lpD[index] = data.lpD[index+1];

			// find last valid entry in table
			for (index = TSIZE; index >= 0 && data.lpD[index] < 0; --index)
				;

			// see if we have any entries
			if (index < 0)
			{
				FreeUp((LPTR)data.lpD);
				return(FALSE);
			}

			// fill in all entries after last with value of last
			while (++index <= TSIZE)
				data.lpD[index] = data.lpD[index-1];

			// interpolate values of all empty cells
			for (index = 0; index <= TSIZE; ++index)
			{
				if (data.lpD[index] < 0)
				{
					pi = index - 1;
					prev = data.lpD[pi];
					ni = index;
					while (data.lpD[ni] < 0)
						++ni;
					next = data.lpD[ni];
					// remember here that (index-pi) == 1
					data.lpD[index] = prev + ((next-prev)/(ni-pi));
				}
			}
			lpVignetteProc = (LPDATAPROC)ellipse_vignette_proc;
		break;

		default:
			return(FALSE);
		break;
	}

	switch(TypeInfo.DataType)
	{
		case FDT_LINEART :
		case FDT_GRAYSCALE :
			data.lpProcessProc = (LPVIGPROC)ProcessVignette8;
		break;

		case FDT_PALETTECOLOR:
			data.lpProcessProc = (LPVIGPROC)ProcessVignette8P;
			data.lpPaletteLUT = CreatePaletteLut15(TypeInfo.ColorMap->RGBData,
					TypeInfo.ColorMap->NumEntries, NULL, NULL);
		break;

		case FDT_RGBCOLOR :
			data.lpProcessProc = (LPVIGPROC)ProcessVignette24;
		break;

		case FDT_CMYKCOLOR :
			data.lpProcessProc = (LPVIGPROC)ProcessVignette32;
		break;
	}

	data.lpMidpointTable = BuildMidPointTable(
							DoHSL,
							TypeInfo.DataType,
							lpParms->Midpoint,
							&lpParms->StartColor,
							&lpParms->EndColor,
							&data );

	ImgGetMaskRect( lpImage, &rMask );
	data.lplD = (LPLONG)Alloc((long)sizeof(long)*(long)RectWidth(&rMask));
	data.lpld = (LPLONG)Alloc((long)sizeof(long)*(long)RectWidth(&rMask));

	if (!data.lpld || !data.lplD || !data.lpMidpointTable ||
		(TypeInfo.DataType == FDT_PALETTECOLOR && !data.lpPaletteLUT))
	{
		if (lpParms->Gradient == IDC_VIGELLIPSE)
			FreeUp((LPTR)data.lpD);
		if (data.lplD)
			FreeUp((LPTR)data.lplD);
		if (data.lpld)
			FreeUp((LPTR)data.lpld);
		if (data.lpMidpointTable)
			FreeUp((LPTR)data.lpMidpointTable);
		if (data.lpPaletteLUT)
			FreeUp(data.lpPaletteLUT);
		return(FALSE);
	}

	SetEngineDraw(&Engine,lpVignetteProc,lpParms->VigOpacity,lpParms->VigMergeMode);
	Engine.lpParam = &data;
	Engine.fThread = NO;
	lpParms->Common.StatusCode = LineEngineSelObj(lpImage,&Engine,lpParms->Common.idDirty);
	if (!AstralIsRectEmpty(&Engine.rUpdate))
	{
		lpParms->Common.UpdateType = UT_AREA;
		lpParms->Common.rUpdateArea = Engine.rUpdate;
	}

	FreeUp((LPTR)data.lpld);
	FreeUp((LPTR)data.lplD);
	FreeUp((LPTR)data.lpMidpointTable);
	if (data.lpPaletteLUT)
		FreeUp(data.lpPaletteLUT);

	if (lpParms->Gradient == IDC_VIGELLIPSE)
		FreeUp((LPTR)data.lpD);
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/************************************************************************/

BOOL TintFillImage(
	LPIMAGE     lpImage,
	LPTINTFILL_PARMS lpParms)
{
	ENGINE Engine;

	SetEngineColor(&Engine, &lpParms->Color, lpParms->FillOpacity,
					lpParms->FillMergeMode);
	lpParms->Common.StatusCode = LineEngineSelObj(lpImage,&Engine,lpParms->Common.idDirty);
	if (!AstralIsRectEmpty(&Engine.rUpdate))
	{
		lpParms->Common.UpdateType = UT_AREA;
		lpParms->Common.rUpdateArea = Engine.rUpdate;
	}
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/************************************************************************/

BOOL TextureFillImage(
	LPIMAGE lpImage,
	LPTEXTUREFILL_PARMS lpParms)
{
	ENGINE Engine;
	FNAME szFileName;
	LPFRAME lpTexture;
	FRMTYPEINFO TypeInfo;

	ImgGetTypeInfo(lpImage, &TypeInfo);

	if (!LookupExtFile(lpParms->TextureName, szFileName, IDN_TEXTURE))
	{
		lpParms->Common.StatusCode = SC_FILENOTFOUND;
		return(FALSE);
	}
	ProgressBegin(2, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
	CReadImage read(szFileName);
	if ( !(lpTexture = read.ReadFrame(TypeInfo) ))
	{
		ProgressEnd();
		lpParms->Common.StatusCode = SC_READERROR;
		return(FALSE);
	}

	SetEngineOld(&Engine, NO, NULL, NULL, lpTexture,
		lpParms->fHorzFlip, lpParms->fVertFlip,
		NULL, NULL, lpParms->TextureOpacity, lpParms->TextureMergeMode);
	lpParms->Common.StatusCode = LineEngineSelObj(lpImage,&Engine,lpParms->Common.idDirty);
	if (!AstralIsRectEmpty(&Engine.rUpdate))
	{
 		lpParms->Common.UpdateType = UT_AREA;
		lpParms->Common.rUpdateArea = Engine.rUpdate;
	}
	ProgressEnd();
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/************************************************************************/

BOOL InvertImage(LPIMAGE lpImage, LPCMD_PARMS lpParms)
{
	ENGINE Engine;
	FRMTYPEINFO TypeInfo;
	LPCOLORMAP lpColorMap;

	ImgGetTypeInfo(lpImage, &TypeInfo);
	if (TypeInfo.DataType == FDT_PALETTECOLOR)
	{
		lpColorMap = FrameCopyColorMap(TypeInfo.ColorMap);
		if (!lpColorMap)
			{
			Message(IDS_EMEMALLOC);
			lpParms->Common.StatusCode = SC_MEMERROR;
			return(FALSE);
			}
		negate((LPTR)lpColorMap->RGBData, sizeof(RGBS)*lpColorMap->NumEntries);
		lpParms->Common.StatusCode = ApplyColorMap(lpImage, lpColorMap, FALSE, DT_NONE, lpParms->Common.idDirty);
		FrameDestroyColorMap(lpColorMap);
		if (lpParms->Common.StatusCode == SC_SUCCESS)
			lpParms->Common.UpdateType = UT_DATATYPE;
	}
	else
	{
		SetEngineDef(&Engine);
		Engine.lpDataProc = invert_proc;
		lpParms->Common.StatusCode = LineEngineSelObj(lpImage,&Engine,lpParms->Common.idDirty);
		if (!AstralIsRectEmpty(&Engine.rUpdate))
		{
			lpParms->Common.UpdateType = UT_AREA;
			lpParms->Common.rUpdateArea = Engine.rUpdate;
		}
	}
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/************************************************************************/

static void HSLtoRGB24( LPRGB lpSrc, int iCount )
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

static void invert_proc( int y, int left, int right, LPTR lpDst,
                           LPTR lpSrc, int depth, LPVOID lpParms )
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
	LPGRADIENT_DATA lpGrad)
{
	long d, D, lTableRange;
	int  r;
	LFIXED rate;
	LPTR   lpMyMidPoint = lpGrad->lpMidpointTable;
	int    iTableIndex;
	int nRepetitions, SoftTransition;
	LPLONG lpld, lplD;

	lpld = lpGrad->lpld;
	lplD = lpGrad->lplD;
	lTableRange = lpGrad->lTableRange;
	nRepetitions = lpGrad->nRepetitions;
	SoftTransition = lpGrad->SoftTransition;

	if (nRepetitions == 1)
	{
		while (iCount-- > 0)
		{
			DO_RANDOM;

			REPS_EQUAL_ONE;

			iTableIndex = WHOLE(rate*lTableRange);

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

			iTableIndex = WHOLE(rate*lTableRange);

			QUICK_BOUND(lpMyMidPoint[iTableIndex] + r, *lpSrc);

			lpSrc++;
		}
	}
}

/************************************************************************/

static void ProcessVignette8P(
	LPTR  lpSrc,
	int    iCount,
	LPGRADIENT_DATA lpGrad)
{
	long d, D, lTableRange;
	int  r;
	LFIXED rate;
	LPRGB  lpMyMidPoint = (LPRGB)lpGrad->lpMidpointTable;
	LPTR lpPaletteLUT = lpGrad->lpPaletteLUT;
	int    iTableIndex;
	int nRepetitions, SoftTransition;
	LPLONG lpld, lplD;
	RGBS rgb;
	WORD wRGB;

	lpld = lpGrad->lpld;
	lplD = lpGrad->lplD;
	lTableRange = lpGrad->lTableRange;
	nRepetitions = lpGrad->nRepetitions;
	SoftTransition = lpGrad->SoftTransition;

	if (nRepetitions == 1)
	{
		while (iCount-- > 0)
		{
			DO_RANDOM;

			REPS_EQUAL_ONE;

			iTableIndex = WHOLE(rate * lTableRange);

			if (r)
			{
				QUICK_BOUND(lpMyMidPoint[iTableIndex].red   + r, rgb.red);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[iTableIndex].green + r, rgb.green);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[iTableIndex].blue  + r, rgb.blue);
			}
			else
			{
				QUICK_BOUND(lpMyMidPoint[iTableIndex].red  , rgb.red);
				QUICK_BOUND(lpMyMidPoint[iTableIndex].green, rgb.green);
				QUICK_BOUND(lpMyMidPoint[iTableIndex].blue , rgb.blue);
			}
			wRGB = RGBtoMiniRGB(&rgb);
			*lpSrc++ = lpPaletteLUT[wRGB];
		}
	}
	else
	{
		while (iCount-- > 0)
		{
			DO_RANDOM;

			REPS_NOT_ONE;

			iTableIndex = WHOLE(rate*lTableRange);

			if (r)
			{
				QUICK_BOUND(lpMyMidPoint[iTableIndex].red   + r, rgb.red);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[iTableIndex].green + r, rgb.green);
				NEW_RANDOM;
				QUICK_BOUND(lpMyMidPoint[iTableIndex].blue  + r, rgb.blue);
			}
			else
			{
				QUICK_BOUND(lpMyMidPoint[iTableIndex].red  , rgb.red);
				QUICK_BOUND(lpMyMidPoint[iTableIndex].green, rgb.green);
				QUICK_BOUND(lpMyMidPoint[iTableIndex].blue , rgb.blue);
			}

			wRGB = RGBtoMiniRGB(&rgb);
			*lpSrc++ = lpPaletteLUT[wRGB];
		}
	}
}

/************************************************************************/

static void ProcessVignette24(
	LPRGB  lpSrc,
	int    iCount,
	LPGRADIENT_DATA lpGrad)
{
	long d, D, lTableRange;
	int  r;
	LFIXED rate;
	LPRGB  lpMyMidPoint = (LPRGB)lpGrad->lpMidpointTable;
	int    iTableIndex;
	int nRepetitions, SoftTransition;
	LPLONG lpld, lplD;

	lpld = lpGrad->lpld;
	lplD = lpGrad->lplD;
	lTableRange = lpGrad->lTableRange;
	nRepetitions = lpGrad->nRepetitions;
	SoftTransition = lpGrad->SoftTransition;

	if (nRepetitions == 1)
	{
		while (iCount-- > 0)
		{
			DO_RANDOM;

			REPS_EQUAL_ONE;

			iTableIndex = WHOLE(rate * lTableRange);

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

			iTableIndex = WHOLE(rate*lTableRange);

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
	LPGRADIENT_DATA lpGrad)
{
	long   d, D, lTableRange;
	int    r;
	LFIXED rate;
	LPCMYK lpMyMidPoint = (LPCMYK)lpGrad->lpMidpointTable;
	int    iTableIndex;
	int nRepetitions, SoftTransition;
	LPLONG lpld, lplD;

	lpld = lpGrad->lpld;
	lplD = lpGrad->lplD;
	lTableRange = lpGrad->lTableRange;
	nRepetitions = lpGrad->nRepetitions;
	SoftTransition = lpGrad->SoftTransition;

	if (nRepetitions == 1)
	{
		while (iCount-- > 0)
		{
			DO_RANDOM;

			REPS_EQUAL_ONE;

			iTableIndex = WHOLE(rate*lTableRange);

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

			iTableIndex = WHOLE(rate*lTableRange);

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

static void ellipse_vignette_proc( int y, int left, int right, LPTR lpDst,
                                    LPTR lpSrc, int depth,
                                    LPGRADIENT_DATA lpGrad )
{
	int xn, index;
	LPLONG lptld, lptlD;
	long dx, dy, d, dx2, dy2;

	dy = y - lpGrad->yc;
	lptld = lpGrad->lpld;
	lptlD = lpGrad->lplD;

	for (xn = left; xn <= right; ++xn)
	{
		dx = xn - lpGrad->xc;
		dx2 = dx*dx;
		dy2 = dy*dy;
		d = dx2 + dy2;
		*lptld++ = lsqrt(d);
		if (d > 0)
		{
			if (lpGrad->ea > lpGrad->eb)
				index = ((dx2 * (long)TSIZE)+(d/2)) / d;
			else
				index = ((dy2 * (long)TSIZE)+(d/2)) / d;
			index = bound(abs(index), 0, TSIZE);
			*lptlD++ = lpGrad->lpD[index];
		}
		else
			*lptlD++ = 1;
	}

	(*lpGrad->lpProcessProc)(lpSrc, right-left+1, lpGrad);
}

/************************************************************************/

static void rectangle_vignette_proc( int the_y, int left, int right,
                                       LPTR lpDst, LPTR lpSrc, int depth,
                                       LPGRADIENT_DATA lpGrad )
{
	int xn, px, py, the_x, y;
	long xc2, yc2, dy, m, ldx, ldy;
	LPLONG lptld, lptlD;

	xc2 = 2L * lpGrad->xc;
	yc2 = 2L * lpGrad->yc;

	if (the_y > lpGrad->yc)  // bottom side
		y = yc2 - the_y;  // flip y over to top side
	else
		y = the_y;

	dy = y - lpGrad->yc;
	lptld = lpGrad->lpld;
	lptlD = lpGrad->lplD;

	for (the_x = left; the_x <= right; ++the_x)
	{
		if (the_x > lpGrad->xc)   // right side
			xn = xc2 - the_x;   // flip x over to left side
		else
			xn = the_x;

		if (xn < lpGrad->xmin || xn > lpGrad->xmax || y < lpGrad->ymin || y > lpGrad->ymax)
		{
			*lptld++ = 2;
			*lptlD++ = 1;
			continue;
		}

		if (xn > lpGrad->xc)		// right side
		{
			m = (256L * dy) / (xn-lpGrad->xc);
			if (y > lpGrad->yc)	// bottom-right corner
			{
				*lptlD++ = lpGrad->D3;
				if (m < lpGrad->m3)
				{
					px = xn;
					py = (((lpGrad->m3 * px)+128L)/256L)+lpGrad->b3;
				}
				else
				{
					py = y;
					px = (((py-lpGrad->b3)*256L)+128L)/lpGrad->m3;
				}
			}
			else		// top-right corner
			{
				*lptlD++ = lpGrad->D2;
				if (m > lpGrad->m2)
				{
					px = xn;
					py = (((lpGrad->m2 * px)+128L)/256L)+lpGrad->b2;
				}
				else
				{
					py = y;
					px = (((py-lpGrad->b2)*256L)+128L)/lpGrad->m2;
				}
			}
		}
		else if (xn < lpGrad->xc)	// left side
		{
			m = (256L * dy) / (xn-lpGrad->xc);
			if (y > lpGrad->yc)	// bottom-left corner
			{
				*lptlD++ = lpGrad->D4;
				if (m > lpGrad->m4)
				{
					px = xn;
					py = (((lpGrad->m4 * px)+128L)/256L)+lpGrad->b4;
				}
				else
				{
					py = y;
					px = (((py-lpGrad->b4)*256L)+128L)/lpGrad->m4;
				}
			}
			else		// top-left corner
			{
				*lptlD++ = lpGrad->D1;
				if (m < lpGrad->m1)
				{
					px = xn;
					py = (((lpGrad->m1 * px)+128L)/256L)+lpGrad->b1;
				}
				else
				{
					py = y;
					px = (((py-lpGrad->b1)*256L)+128L)/lpGrad->m1;
				}
			}
		}
		else	// x = lpGrad->xc - infinite slope
		{
			if (y > lpGrad->yc)
			{
				*lptlD++ = lpGrad->D3;
				py = y;
				px = (((py-lpGrad->b3)*256L)+128L)/lpGrad->m3;
			}
			else
			{
				*lptlD++ = lpGrad->D1;
				py = y;
				px = (((py-lpGrad->b1)*256L)+128L)/lpGrad->m1;
			}
		}
		ldx = px - lpGrad->xc;
		ldy = py - lpGrad->yc;
		*lptld++ = lsqrt((ldx*ldx)+(ldy*ldy));
	}
	(*lpGrad->lpProcessProc)(lpSrc, right-left+1, lpGrad);
}

/************************************************************************/

static void linear_vignette_proc( int y, int left, int right, LPTR lpDst,
                                    LPTR lpSrc, int depth,
                                    LPGRADIENT_DATA lpGrad )
{
	int xn;
	LPLONG lptld, lptlD;
	long D;

	lptld = lpGrad->lpld;
	lptlD = lpGrad->lplD;
	D = lpGrad->D;

	for (xn = left; xn <= right; ++xn)
	{
		*lptld++ = WHOLE( rotx(xn, y, lpGrad->x1, lpGrad->y1, lpGrad->cosine,
         lpGrad->sine) );
		*lptlD++ = D;
	}
	(*lpGrad->lpProcessProc)(lpSrc, right-left+1, lpGrad);
}

/************************************************************************/

static void radial_vignette_proc( int y, int left, int right, LPTR lpDst,
                                    LPTR lpSrc, int depth,
                                    LPGRADIENT_DATA lpGrad )
{
	int xn, tdx, tdy;
	LPLONG lptld, lptlD;
	long D;

	tdy = y - lpGrad->y1;
	lptld = lpGrad->lpld;
	lptlD = lpGrad->lplD;
	D = lpGrad->D;

	for (xn = left; xn <= right; ++xn, ++lptld, ++lptlD)
	{
		tdx = xn - lpGrad->x1;
		*lptld = lsqrt(((long)tdx*(long)tdx)+((long)tdy*(long)tdy));
		*lptlD = D;
	}
	(*lpGrad->lpProcessProc)(lpSrc, right-left+1, lpGrad);
}

/************************************************************************/





