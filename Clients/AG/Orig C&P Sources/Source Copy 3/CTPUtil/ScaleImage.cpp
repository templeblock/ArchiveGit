#include "StdAfx.h"
#include <math.h>
#include <windowsx.h> // for GlobalAlloc stuff
#include "ScaleImage.h"

////////////////////////////////////////////////////////////////////////////
// The following code is from Filtered Image Rescaling by Dale Schumacher	//
// in Graphics Gems III 													//
/////////////////////////////////////////////////////////////////////////////

#define M_PI			3.141592
#define WHITE_PIXEL 	(255)
#define BLACK_PIXEL 	(0)
#define CLAMP(v,l,h)	((v)< (l) ?(l) :(v) > (h) ?(h) : v)

typedef struct
{
	BYTE b;
	BYTE g;
	BYTE r;
} Pixel;

typedef struct
{
	int pixel;
	double weight;
} CONTRIB;

typedef struct
{
	int n;			/* number of contributors */
	CONTRIB* p; 	/* pointer to list of contributions */
} CLIST;

/////////////////////////////////////////////////////////////////////////////
static void get_row(Pixel* row, Image* image, int y)
{
	if ((y < 0) || (y >= image->ysize))
		return;

	MoveMemory(row, image->data + ((long) y * image->span), (sizeof(Pixel) * image->xsize));
}

/////////////////////////////////////////////////////////////////////////////
static void get_column(Pixel* column, Image* image, int x)
{
	if ((x < 0) || (x >= image->xsize))
		return;

	int d = image->span;
	BYTE* p = image->data + (x * sizeof(Pixel));
	for (int i = image->ysize; i-- > 0; p += d)
	{
		column->r = p[2];
		column->g = p[1];
		column->b = p[0];
		column++;
	}
}

/////////////////////////////////////////////////////////////////////////////
static void put_pixel(Image* image, int x, int y, Pixel* data)
{
	static Image* im = NULL;
	static int yy = -1;
	static BYTE* p = NULL;

	if ((x < 0) || (x >= image->xsize) || (y < 0) || (y >= image->ysize))
		return;

	if ((im != image) || (yy != y))
	{
		im = image;
		yy = y;
		p = image->data + ((long) y * image->span);
	}

	BYTE* pTemp = p + (x * sizeof(Pixel));
	*pTemp++ = data->b;
	*pTemp++ = data->g;
	*pTemp = data->r;
}

/////////////////////////////////////////////////////////////////////////////
inline double sinc(double x)
{
	x *= M_PI;
	if (x)
		return (sin(x) / x);

	return (1.0);
}

#define Lanczos3_support	(3.0)

/////////////////////////////////////////////////////////////////////////////
inline double Lanczos3_filter(double t)
{
	if (t < 0)
		t = -t;
	if (t < 3.0)
		return (sinc(t) * sinc(t/3.0));

	return (0.0);
}

/////////////////////////////////////////////////////////////////////////////
void ScaleImage(Image* dst, Image* src)
{
	/* create intermediate image to hold horizontal zoom */
	Image tmp;
	tmp.xsize = dst->xsize;
	tmp.ysize = src->ysize;
	tmp.data = (BYTE*)GlobalAllocPtr(GHND, (long)tmp.xsize * tmp.ysize * sizeof(Pixel));
	if (!tmp.data)
		return;
	tmp.span = tmp.xsize * sizeof(Pixel);

	double xscale = (double)dst->xsize / (double)src->xsize;
	double yscale = (double)dst->ysize / (double)src->ysize;

	/* pre-calculate filter contributions for a row */

	// array of contribution lists
	CLIST* contrib = (CLIST*)GlobalAllocPtr(GHND, dst->xsize * sizeof(CLIST));
	if (xscale < 1.0)
	{
		double width = Lanczos3_support / xscale;
		double fscale = 1.0 / xscale;
		for (int i = 0; i < dst->xsize; ++i)
		{
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB*)GlobalAllocPtr(GHND, (long)(width * 2 + 1) * sizeof(CONTRIB));
			double center = (double)i / xscale;
			double left = ceil(center - width);
			double right = floor(center + width);
			for (int j = (int)left; j <= (int)right; ++j)
			{
				double weight = center - (double)j;
				weight = Lanczos3_filter(weight / fscale) / fscale;
				int n = 0;
				if (j < 0)
					n = -j;
				else
				if (j >= src->xsize)
					n = (src->xsize - j) + src->xsize - 1;
				else
					n = j;

				int k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}
	else
	{
		for (int i = 0; i < dst->xsize; ++i)
		{
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB*)GlobalAllocPtr(GHND, (DWORD)(Lanczos3_support * 2 + 1) * sizeof(CONTRIB));
			double center = (double)i / xscale;
			double left = ceil(center - Lanczos3_support);
			double right = floor(center + Lanczos3_support);
			for (int j = (int)left; j <= (int)right; ++j)
			{
				double weight = center - (double)j;
				weight = Lanczos3_filter(weight);
				int n = 0;
				if (j < 0)
					n = -j;
				else
				if (j >= src->xsize)
					n = (src->xsize - j) + src->xsize - 1;
				else
					n = j;

				int k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

	/* apply filter to zoom horizontally from src to tmp */
	Pixel* raster = (Pixel*)GlobalAllocPtr(GHND, src->xsize * sizeof(Pixel));
	for (int k = 0; k < tmp.ysize; ++k)
	{
		get_row(raster, src, k);
		for (int i = 0; i < tmp.xsize; ++i)
		{
			double rweight = 0.0;
			double gweight = 0.0;
			double bweight = 0.0;
			for (int j = 0; j < contrib[i].n; ++j)
			{
				rweight += raster[contrib[i].p[j].pixel].r * contrib[i].p[j].weight;
				gweight += raster[contrib[i].p[j].pixel].g * contrib[i].p[j].weight;
				bweight += raster[contrib[i].p[j].pixel].b * contrib[i].p[j].weight;
			}

			Pixel p;
			p.r = (BYTE)CLAMP(rweight, BLACK_PIXEL, WHITE_PIXEL);
			p.g = (BYTE)CLAMP(gweight, BLACK_PIXEL, WHITE_PIXEL);
			p.b = (BYTE)CLAMP(bweight, BLACK_PIXEL, WHITE_PIXEL);

			// Since this output is destined for the printer, force to white
			if (p.r > 250 && p.g > 250 && p.b > 250)
				p.r = p.g = p.b = 255;
			put_pixel(&tmp, i, k, &p);
		}
	}

	GlobalFreePtr(raster);

	/* free the memory allocated for horizontal filter weights */
	for (int i = 0; i < tmp.xsize; ++i)
		GlobalFreePtr(contrib[i].p);

	GlobalFreePtr(contrib);

	/* pre-calculate filter contributions for a column */
	contrib = (CLIST*)GlobalAllocPtr(GHND, dst->ysize * sizeof(CLIST));
	if (yscale < 1.0)
	{
		double width = Lanczos3_support / yscale;
		double fscale = 1.0 / yscale;
		for (int i = 0; i < dst->ysize; ++i)
		{
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB*)GlobalAllocPtr(GHND, (long)(width * 2 + 1) * sizeof(CONTRIB));
			double center = (double)i / yscale;
			double left = ceil(center - width);
			double right = floor(center + width);
			for (int j = (int)left; j <= (int)right; ++j)
			{
				double weight = center - (double)j;
				weight = Lanczos3_filter(weight / fscale) / fscale;
				int n = 0;
				if (j < 0)
					n = -j;
				else
				if (j >= tmp.ysize)
					n = (tmp.ysize - j) + tmp.ysize - 1;
				else
					n = j;

				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}
	else
	{
		for (int i = 0; i < dst->ysize; ++i)
		{
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB*)GlobalAllocPtr(GHND, (DWORD)(Lanczos3_support * 2 + 1) * sizeof(CONTRIB));
			double center = (double)i / yscale;
			double left = ceil(center - Lanczos3_support);
			double right = floor(center + Lanczos3_support);
			for (int j = (int)left; j <= (int)right; ++j)
			{
				double weight = center - (double)j;
				weight = Lanczos3_filter(weight);
				int n = 0;
				if (j < 0)
					n = -j;
				else
				if (j >= tmp.ysize)
					n = (tmp.ysize - j) + tmp.ysize - 1;
				else
					n = j;

				k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

	/* apply filter to zoom vertically from tmp to dst */
	raster = (Pixel*)GlobalAllocPtr(GHND, tmp.ysize * sizeof(Pixel));
	for (int x = 0; x < dst->xsize; ++x)
	{
		get_column(raster, &tmp, x);
		for (int i = 0; i < dst->ysize; ++i)
		{
			double rweight = 0.0;
			double gweight = 0.0;
			double bweight = 0.0;
			for (int j = 0; j < contrib[i].n; ++j)
			{
				rweight += raster[contrib[i].p[j].pixel].r * contrib[i].p[j].weight;
				gweight += raster[contrib[i].p[j].pixel].g * contrib[i].p[j].weight;
				bweight += raster[contrib[i].p[j].pixel].b * contrib[i].p[j].weight;
			}

			Pixel p;
			p.r = (BYTE)CLAMP(rweight, BLACK_PIXEL, WHITE_PIXEL);
			p.g = (BYTE)CLAMP(gweight, BLACK_PIXEL, WHITE_PIXEL);
			p.b = (BYTE)CLAMP(bweight, BLACK_PIXEL, WHITE_PIXEL);

			// Since this output is destined for the printer, force to white
			if (p.r > 250 && p.g > 250 && p.b > 250)
				p.r = p.g = p.b = 255;
			put_pixel(dst, x, i, &p);
		}
	}

	GlobalFreePtr(raster);

	/* free the memory allocated for vertical filter weights */
	for (i = 0; i < dst->ysize; ++i)
		GlobalFreePtr(contrib[i].p);

	GlobalFreePtr(contrib);
	GlobalFreePtr(tmp.data);
}
