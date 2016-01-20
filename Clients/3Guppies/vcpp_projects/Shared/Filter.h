#pragma once
#include <math.h>
#include "Dib.h"
#include "Utility.h"

/////////////////////////////////////////////////////////////////////////////
// The following code is based on General Filtered Image Rescaling by Dale Schumacher in Graphics Gems III
// http://www.acm.org/pubs/tog/GraphicsGems/gems.html

#define CLAMP(value)	(BYTE)(value < 0 ? 0 : (value > 255 ? 255 : value))

/////////////////////////////////////////////////////////////////////////////
class CFilter
{
private:
	struct IMAGE
	{
		int xsize;	// Horizontal size of the image in Pixels
		int ysize;	// Vertical size of the image in Pixels
		BYTE* data;	// Pointer to first scanline of image
		int iSpan;	// Byte offset between two scanlines
	};

	struct PIXEL
	{
		BYTE b;
		BYTE g;
		BYTE r;
	};

	struct Contributor
	{
		int iPixel;
		double fWeight;
	};

	struct ContributorEntry
	{
		int n;			// Number of contributors
		Contributor* p; // Pointer to list of contributions
		double fWeightSum;
	};

	/////////////////////////////////////////////////////////////////////////////
	static double sinc(double t)
	{
		if (!t)
			return 1.0;

		t *= 3.14159265/*pi*/;
		return sin(t) / t;
	}

public:
	/////////////////////////////////////////////////////////////////////////////
	class CResamplingFilter
	{
		public: double m_fRadius;
		public: double m_fValueScale;
		public: double m_fStepDown;

		public: CResamplingFilter()
		{
			m_fRadius = 1;
			m_fValueScale = 1;
			m_fStepDown = 0;
		}

		public: virtual double GetValue(double t)
		{
			return 0.0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class HermiteFilter : CResamplingFilter
	{
		double t2, t3;

		public: HermiteFilter()
		{
			m_fRadius = 1;
		}

		public: virtual double GetValue(double t)
		{
			if (t < 0) t = -t;
			t2 = t*t;
			t3 = t2*t;
			if (t < 1) return (2*t3 - 3*t2 + 1);
			return 0;
		}
	};
		
	/////////////////////////////////////////////////////////////////////////////
	class BoxFilter : CResamplingFilter
	{
		public: BoxFilter()
		{
			m_fRadius = 0.5;
		}

		public: virtual double GetValue(double t)
		{
			if (t > -0.5 && t <= 0.5) return 1;
			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class TriangleFilter : CResamplingFilter
	{
		public: TriangleFilter()
		{
			m_fRadius = 1;
		}

		public: virtual double GetValue(double t)
		{
			if (t < 0) t = -t;
			if (t < 1) return (1 - t);
			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class BellFilter : CResamplingFilter
	{
		double t2;

		public: BellFilter()
		{
			m_fRadius = 1.5;
		}

		public: virtual double GetValue(double t)
		{
			if (t < 0) t = -t;
			if (t < 0.5)
			{
				t2 = t*t;
				return (0.75 - t2);
			}
			if (t < 1.5)
			{
				t = t - 1.5;
				t2 = t*t;
				return (0.5*t2);
			}

			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class CubicBSplineFilter : CResamplingFilter
	{
		double t2, t3;

		public: CubicBSplineFilter()
		{
			m_fRadius = 2;
		}

		public: virtual double GetValue(double t)
		{
			if (t < 0) t = -t;
			if (t < 1)
			{
				t2 = t*t;
				t3 = t2*t;
				return ((0.5*t3) - t2 + (2.0/3.0));
			}
			if (t < 2)
			{
				t = 2.0 - t;
				t2 = t*t;
				t3 = t2*t;
				return t3 / 6.0;
			}
			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class MitchellFilter : CResamplingFilter
	{
		double C;
		double t2, t3;

		public: MitchellFilter()
		{
			m_fRadius = 2;
			C = 1/3;
		}

		public: virtual double GetValue(double t)
		{
			if (t < 0) t = -t;
			if (t < 1)
			{
				t2 = t*t;
				t3 = t2*t;
				t = (((12 - 9*C - 6*C)*t3) + ((-18 + 12*C + 6*C)*t2) + (6 - 2*C));
				return (t/6);
			}
			if (t < 2)
			{
				t2 = t*t;
				t3 = t2*t;
				t = (((-C - 6*C)*t3) + ((6*C + 30*C)*t2) + ((-12*C - 48*C)*t) + (8*C + 24*C));
				return (t/6);
			}

			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class CosineFilter : CResamplingFilter
	{
		public: CosineFilter()
		{
			m_fRadius = 1;
		}

		public: virtual double GetValue(double t)
		{
			if (t >= -1 && t <= 1) return ((cos(t*3.14159265/*pi*/) + 1) / 2.0);
			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class CatmullRomFilter : CResamplingFilter
	{
		double t2, t3;

		public: CatmullRomFilter()
		{
			m_fRadius = 2;
		}

		public: virtual double GetValue(double t)
		{
			if (t < 0) t = -t;
			t2 = t*t;
			t3 = t2*t;
			if (t <= 1) return (1.5*t3 - 2.5*t2 + 1);
			if (t <= 2) return (-0.5*t3 + 2.5*t2 - 4*t + 2);
			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class QuadraticFilter : CResamplingFilter
	{
		double t2;

		public: QuadraticFilter()
		{
			m_fRadius = 1.5;
		}

		public: virtual double GetValue(double t)
		{
			if (t < 0) t = -t;
			t2 = t*t;
			if (t <= 0.5) return (-2*t2 + 1);
			if (t <= 1.5) return (t2 - 2.5*t + 1.5);
			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class QuadraticBSplineFilter : CResamplingFilter
	{
		double t2;

		public: QuadraticBSplineFilter()
		{
			m_fRadius = 1.5;
		}

		public: virtual double GetValue(double t)
		{
			if (t < 0) t = -t;
			t2 = t*t;
			if (t <= 0.5) return (-t2 + 0.75);
			if (t <= 1.5) return (0.5*t2 - 1.5*t + 1.125);
			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class CubicConvolutionFilter : CResamplingFilter
	{
		double t2, t3;

		public: CubicConvolutionFilter()
		{
			m_fRadius = 3;
		}

		public: virtual double GetValue(double t)
		{
			if (t < 0) t = -t;
			t2 = t*t;
			t3 = t2*t;
			if (t <= 1) return ((4.0/3.0)*t3 - (7.0/3.0)*t2 + 1);
			if (t <= 2) return (-(7.0/12.0)*t3 + 3*t2 - (59.0/12.0)*t + 2.5);
			if (t <= 3) return ((1.0/12.0)*t3 - (2.0/3.0)*t2 + 1.75*t - 1.5);
			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class Lanczos3Filter : CResamplingFilter
	{
		public: Lanczos3Filter()
		{
			m_fRadius = 3;
		}

		public: virtual double GetValue(double t)
		{
			if (t < 0) t = -t;
			if (t < 3) return (sinc(t)*sinc(t/3.0));
			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class Lanczos8Filter : CResamplingFilter
	{
		public: Lanczos8Filter()
		{
			m_fRadius = 8;
		}

		public: virtual double GetValue(double t)
		{
			if (t < 0) t = -t;
			if (t < 8) return (sinc(t)*sinc(t/8.0));
			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	class GaussianBlur : CResamplingFilter
	{ //j Triangular for now; just testing
		public: GaussianBlur(double fRadius)
		{
			m_fRadius = fRadius; // Dynamic
			m_fValueScale = 1 / fRadius;
//j			m_fStepDown = fRadius / 2; // Number of samples to take; default is 2*fRadius samples
		}

		public: virtual double GetValue(double t)
		{
			if (t >= -1 && t <= 1) return ((cos(t*3.14159265/*pi*/) + 1) / 2.0);
			return 0;
		}
	};

	/////////////////////////////////////////////////////////////////////////////
	CFilter()
	{
	}

	/////////////////////////////////////////////////////////////////////////////
	void Resample(const BITMAPINFOHEADER* pDibSrc, const BITMAPINFOHEADER* pDibDst, CResamplingFilter& filter)
	{
		if (!pDibSrc || !pDibDst)
			return;
		CDib DibSrc = pDibSrc;
		CDib DibDst = pDibDst;
		if (!DibSrc.Width() || !DibSrc.Height())
			return;
		if (!DibDst.Width() || !DibDst.Height())
			return;

		bool bAborting = false;
		float fPercentage = 0;
		int iProgress = 0;
		int iProgressTotal = DibDst.Width() + DibSrc.Height();

		IMAGE src;
		src.xsize = DibSrc.Width();
		src.ysize = DibSrc.Height();
		src.iSpan = DibSrc.WidthBytes();
		src.data = DibSrc.Ptr();

		IMAGE dst;
		dst.xsize = DibDst.Width();
		dst.ysize = DibDst.Height();
		dst.iSpan = DibDst.WidthBytes();
		dst.data = DibDst.Ptr();

		// Create intermediate image to hold horizontal zoom
		IMAGE work;
		work.xsize = dst.xsize;
		work.ysize = src.ysize;
		work.iSpan = work.xsize * sizeof(PIXEL);
		work.data = new BYTE[work.iSpan * work.ysize];
		if (!work.data)
			return;

		double xScale = (double)dst.xsize / (double)src.xsize;
		double yScale = (double)dst.ysize / (double)src.ysize;

		// Pre-calculate filter contributions for a row
		// Create an array of contributions
		ContributorEntry* contrib = new ContributorEntry[dst.xsize];

		double fRadius = filter.m_fRadius;
		double fValueScale = filter.m_fValueScale;
		if (xScale < 1.0) // Horizontal downsampling
		{
			fRadius /= xScale;
			fValueScale *= xScale;
		}
		
		double fDiameter = 2*fRadius + 1;
		int iArraySize = dtoi(fDiameter) + 1/*Slop*/;
		double fStepSize = (!filter.m_fStepDown ? 1 : (fDiameter - 1) / filter.m_fStepDown);
		for (int i = 0; i < dst.xsize; i++)
		{
			contrib[i].n = 0;
			contrib[i].p = new Contributor[iArraySize];
			contrib[i].fWeightSum = 0;
			double fCenter = (double)i / xScale;
			double fLeft = fCenter - fRadius;
			double fRight = fLeft + fDiameter - 1;

			for (double fj = fLeft; fj <= fRight; fj += fStepSize)
			{
				int j = dtoi(fj);
				if (j < 0 || j >= src.xsize)
					continue;

				double fWeight = 0;
				if (fValueScale == 1)
					fWeight = filter.GetValue(j - fCenter);
				else
					fWeight = filter.GetValue((j - fCenter) * fValueScale) * fValueScale;
				if (!fWeight)
					continue;

				int k = contrib[i].n++;
				contrib[i].p[k].iPixel = j;
				contrib[i].p[k].fWeight = fWeight;
				contrib[i].fWeightSum += fWeight;
			}

			if (bAborting)
				return;
		}

		// Apply filter horizontally from src to work
		PIXEL* pPixelData = new PIXEL[src.xsize];
		for (int y = 0; y < work.ysize; y++)
		{
			// Get the row data
			CopyMemory(pPixelData, src.data + (y * src.iSpan), src.xsize * sizeof(PIXEL));
			for (int x = 0; x < work.xsize; x++)
			{
				double r = 0.0;
				double g = 0.0;
				double b = 0.0;
				for (int k = 0; k < contrib[x].n; k++)
				{
					int n = contrib[x].p[k].iPixel;
					double fWeight = contrib[x].p[k].fWeight;
		            if (!fWeight)
						continue;
					r += fWeight * pPixelData[n].r;
					g += fWeight * pPixelData[n].g;
					b += fWeight * pPixelData[n].b;
				}

				r /= contrib[x].fWeightSum;
				g /= contrib[x].fWeightSum;
				b /= contrib[x].fWeightSum;

				PIXEL p;
				p.r = CLAMP(r);
				p.g = CLAMP(g);
				p.b = CLAMP(b);
				PutPixel(&p, work, x, y);
			}

			if (bAborting)
				return;

			iProgress++;
			fPercentage = (float)iProgress / iProgressTotal * 100;
		}

		delete [] pPixelData;

		// Free the memory allocated for horizontal filter weights
		for (int i = 0; i < dst.xsize; i++)
			delete [] contrib[i].p;

		delete [] contrib;

		// Pre-calculate filter contributions for a column
		// Create an array of contributions
		contrib = new ContributorEntry[dst.ysize];

		fRadius = filter.m_fRadius;
		fValueScale = filter.m_fValueScale;
		if (yScale < 1.0) // Vertical downsampling
		{	
			fRadius /= yScale;
			fValueScale *= yScale;
		}

		fDiameter = 2*fRadius + 1;
		iArraySize = dtoi(fDiameter) + 1/*Slop*/;
		fStepSize = (!filter.m_fStepDown ? 1 : (fDiameter - 1) / filter.m_fStepDown);
		for (int i = 0; i < dst.ysize; i++)
		{
			contrib[i].n = 0;
			contrib[i].p = new Contributor[iArraySize];
			contrib[i].fWeightSum = 0;
			double fCenter = (double)i / yScale;
			double fLeft = fCenter - fRadius;
			double fRight = fLeft + fDiameter - 1;

			for (double fj = fLeft; fj <= fRight; fj += fStepSize)
			{
				int j = dtoi(fj);
				if (j < 0 || j >= work.ysize)
					continue;

				double fWeight = 0;
				if (fValueScale == 1)
					fWeight = filter.GetValue(j - fCenter);
				else
					fWeight = filter.GetValue((j - fCenter) * fValueScale) * fValueScale;
				if (!fWeight)
					continue;

				int k = contrib[i].n++;
				contrib[i].p[k].iPixel = j;
				contrib[i].p[k].fWeight = fWeight;
				contrib[i].fWeightSum += fWeight;
			}

			if (bAborting)
				return;
		}

		// Apply filter to zoom vertically from work to dst
		pPixelData = new PIXEL[work.ysize];
		for (int x = 0; x < dst.xsize; x++)
		{
			// Get the column data
			GetWorkColumn(pPixelData, work, x);
			for (int y = 0; y < dst.ysize; y++)
			{
				double r = 0.0;
				double g = 0.0;
				double b = 0.0;
				for (int k = 0; k < contrib[y].n; k++)
				{
					int n = contrib[y].p[k].iPixel;
					double fWeight = contrib[y].p[k].fWeight;
		            if (!fWeight)
						continue;
					r += fWeight * pPixelData[n].r;
					g += fWeight * pPixelData[n].g;
					b += fWeight * pPixelData[n].b;
				}

				r /= contrib[y].fWeightSum;
				g /= contrib[y].fWeightSum;
				b /= contrib[y].fWeightSum;

				PIXEL p;
				p.r = CLAMP(r);
				p.g = CLAMP(g);
				p.b = CLAMP(b);
				PutPixel(&p, dst, x, y);
			}

			if (bAborting)
				return;

			iProgress++;
			fPercentage = (float)iProgress / iProgressTotal * 100;            
		}

		delete [] pPixelData;

		// Free the memory allocated for vertical filter weights
		for (int i = 0; i < dst.ysize; i++)
			delete [] contrib[i].p;

		delete [] contrib;
		delete [] work.data;
	}

private:
	/////////////////////////////////////////////////////////////////////////////
	bool GetWorkColumn(PIXEL* pPixelData, IMAGE& work, int x)
	{
		BYTE* p = work.data + (x * sizeof(PIXEL));
		int y = work.ysize;
		while (--y >= 0)
		{
			BYTE* pPixel = p;
			pPixelData->b = *pPixel++;
			pPixelData->g = *pPixel++;
			pPixelData->r = *pPixel;
			pPixelData++;
			p += work.iSpan;
		}

		return true;
	}

	/////////////////////////////////////////////////////////////////////////////
	static void PutPixel(PIXEL* pPixelData, IMAGE& image, int x, int y)
	{
		static IMAGE* im;
		static int yy = -1;
		static BYTE* p;

		if (x < 0 || x >= image.xsize || y < 0 || y >= image.ysize)
			return;

		if (im != &image || yy != y)
		{
			im = &image;
			yy = y;
			p = image.data + (y * image.iSpan);
		}

		BYTE* pPixel = p + (x * sizeof(PIXEL));
		*pPixel++ = pPixelData->b;
		*pPixel++ = pPixelData->g;
		*pPixel   = pPixelData->r;
	}
};
