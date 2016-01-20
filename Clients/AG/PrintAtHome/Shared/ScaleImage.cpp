#include "stdafx.h"
#include "ScaleImage.h"
#include "Dib.h"
#include <math.h>
#include <windowsx.h> // for GlobalAllocPtr

/////////////////////////////////////////////////////////////////////////////
// The following code is from Filtered Image Rescaling by Dale Schumacher in Graphics Gems III 													//

#define WHITE_PIXEL 	(255)
#define BLACK_PIXEL 	(0)
#define CLAMP(v,l,h)	((v) < (l) ? (l) : ((v) > (h) ? (h) : v))
#define LANCZOS3		3.0

struct IMAGE
{
	int xsize;	// horizontal size of the image in Pixels
	int ysize;	// vertical size of the image in Pixels
	BYTE* data;	// pointer to first scanline of image
	int span;	// byte offset between two scanlines
};

struct PIXEL
{
	BYTE b;
	BYTE g;
	BYTE r;
};

struct CONTRIB
{
	int pixel;
	double weight;
};

struct CLIST
{
	int n;			// number of contributors
	CONTRIB* p; 	// pointer to list of contributions
};

/////////////////////////////////////////////////////////////////////////////
static void GetColumn(PIXEL* column, IMAGE& image, int x)
{
	if ((x < 0) || (x >= image.xsize))
		return;

	int d = image.span;
	BYTE* p = image.data + (x * sizeof(PIXEL));
	for (int i = image.ysize; i-- > 0; p += d)
	{
		column->r = p[2];
		column->g = p[1];
		column->b = p[0];
		column++;
	}
}

/////////////////////////////////////////////////////////////////////////////
static void PutPixel(IMAGE& image, int x, int y, PIXEL* data)
{
	static IMAGE* im;
	static int yy = -1;
	static BYTE* p;

	if ((x < 0) || (x >= image.xsize) || (y < 0) || (y >= image.ysize))
		return;

	if ((im != &image) || (yy != y))
	{
		im = &image;
		yy = y;
		p = image.data + ((long) y * image.span);
	}

	BYTE* pTemp = p + (x * sizeof(PIXEL));
	*pTemp++ = data->b;
	*pTemp++ = data->g;
	*pTemp = data->r;
}

/////////////////////////////////////////////////////////////////////////////
inline double sinc(double x)
{
	if (!x)
		return 1.0;

	x *= 3.14159265/*pi*/;
	return sin(x) / x;
}

/////////////////////////////////////////////////////////////////////////////
inline double Lanczos3_filter(double t)
{
	if (t < 0)
		t = -t;
	if (t < 3.0)
		return (sinc(t) * sinc(t/3.0));

	return 0.0;
}

/////////////////////////////////////////////////////////////////////////////
void ScaleImage(const BITMAPINFOHEADER* pDibSrc, const BITMAPINFOHEADER* pDibDst)
{
	IMAGE src;
	src.xsize = (int)DibWidth(pDibSrc);
	src.ysize = (int)DibHeight(pDibSrc);
	src.data = (BYTE*)DibPtr(pDibSrc);
	src.span = DibWidthBytes(pDibSrc);

	IMAGE dst;
	dst.xsize = (int)DibWidth(pDibDst);
	dst.ysize = (int)DibHeight(pDibDst);
	dst.data = (BYTE*)DibPtr(pDibDst);
	dst.span = DibWidthBytes(pDibDst);

	// create intermediate image to hold horizontal zoom
	IMAGE tmp;
	tmp.xsize = dst.xsize;
	tmp.ysize = src.ysize;
	tmp.data = (BYTE*)GlobalAllocPtr(GHND, (long)tmp.xsize * tmp.ysize * sizeof(PIXEL));
	if (!tmp.data)
		return;
	tmp.span = tmp.xsize * sizeof(PIXEL);

	double xscale = (double)dst.xsize / (double)src.xsize;
	double yscale = (double)dst.ysize / (double)src.ysize;

	// pre-calculate filter contributions for a row

	// array of contribution lists
	CLIST* contrib = (CLIST*)GlobalAllocPtr(GHND, dst.xsize * sizeof(CLIST));
	if (xscale < 1.0)
	{
		double width = LANCZOS3 / xscale;
		double fscale = 1.0 / xscale;
		for (int i = 0; i < dst.xsize; ++i)
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
				if (j >= src.xsize)
					n = (src.xsize - j) + src.xsize - 1;
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
		for (int i = 0; i < dst.xsize; ++i)
		{
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB*)GlobalAllocPtr(GHND, (DWORD)(LANCZOS3 * 2 + 1) * sizeof(CONTRIB));
			double center = (double)i / xscale;
			double left = ceil(center - LANCZOS3);
			double right = floor(center + LANCZOS3);
			for (int j = (int)left; j <= (int)right; ++j)
			{
				double weight = center - (double)j;
				weight = Lanczos3_filter(weight);
				int n = 0;
				if (j < 0)
					n = -j;
				else
				if (j >= src.xsize)
					n = (src.xsize - j) + src.xsize - 1;
				else
					n = j;

				int k = contrib[i].n++;
				contrib[i].p[k].pixel = n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

	// apply filter to zoom horizontally from src to tmp
	PIXEL* raster = (PIXEL*)GlobalAllocPtr(GHND, src.xsize * sizeof(PIXEL));
	for (int k = 0; k < tmp.ysize; ++k)
	{
		// Get the row data
		if (k < src.ysize)
			MoveMemory(raster, src.data + ((long) k * src.span), (sizeof(PIXEL) * src.xsize));

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

			PIXEL p;
			p.r = (BYTE)CLAMP(rweight, BLACK_PIXEL, WHITE_PIXEL);
			p.g = (BYTE)CLAMP(gweight, BLACK_PIXEL, WHITE_PIXEL);
			p.b = (BYTE)CLAMP(bweight, BLACK_PIXEL, WHITE_PIXEL);

			// Since this output is destined for the printer, force to white
			if (p.r > 250 && p.g > 250 && p.b > 250)
				p.r = p.g = p.b = 255;
			PutPixel(tmp, i, k, &p);
		}
	}

	GlobalFreePtr(raster);

	// free the memory allocated for horizontal filter weights
	for (int i = 0; i < tmp.xsize; ++i)
		GlobalFreePtr(contrib[i].p);

	GlobalFreePtr(contrib);

	// pre-calculate filter contributions for a column
	contrib = (CLIST*)GlobalAllocPtr(GHND, dst.ysize * sizeof(CLIST));
	if (yscale < 1.0)
	{
		double width = LANCZOS3 / yscale;
		double fscale = 1.0 / yscale;
		int k = 0;
		for (int i = 0; i < dst.ysize; ++i)
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
		int k = 0;
		for (int i = 0; i < dst.ysize; ++i)
		{
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB*)GlobalAllocPtr(GHND, (DWORD)(LANCZOS3 * 2 + 1) * sizeof(CONTRIB));
			double center = (double)i / yscale;
			double left = ceil(center - LANCZOS3);
			double right = floor(center + LANCZOS3);
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

	// apply filter to zoom vertically from tmp to dst
	raster = (PIXEL*)GlobalAllocPtr(GHND, tmp.ysize * sizeof(PIXEL));
	for (int x = 0; x < dst.xsize; ++x)
	{
		GetColumn(raster, tmp, x);
		for (int i = 0; i < dst.ysize; ++i)
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

			PIXEL p;
			p.r = (BYTE)CLAMP(rweight, BLACK_PIXEL, WHITE_PIXEL);
			p.g = (BYTE)CLAMP(gweight, BLACK_PIXEL, WHITE_PIXEL);
			p.b = (BYTE)CLAMP(bweight, BLACK_PIXEL, WHITE_PIXEL);

			// Since this output is destined for the printer, force to white
			if (p.r > 250 && p.g > 250 && p.b > 250)
				p.r = p.g = p.b = 255;
			PutPixel(dst, x, i, &p);
		}
	}

	GlobalFreePtr(raster);

	// free the memory allocated for vertical filter weights
	for (int i = 0; i < dst.ysize; ++i)
		GlobalFreePtr(contrib[i].p);

	GlobalFreePtr(contrib);
	GlobalFreePtr(tmp.data);
}
