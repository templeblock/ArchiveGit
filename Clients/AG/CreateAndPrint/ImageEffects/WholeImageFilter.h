#pragma once
#include "WholeImageFilter.h"

namespace ImageEffects
{
	/// A filter which acts as a superclass for filters which need to have the whole image in memory
	class WholeImageFilter abstract
	{
	public:
		CRect transformedSpace;
		CRect originalSpace;

	public:
		WholeImageFilter()
		{
		}

		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int width = src->biWidth;
			int height = src->biHeight;
//j			int type = (int)(src->PixelFormat);
			BITMAPINFOHEADER *srcRaster = src;

//j			originalSpace = CRect(0, 0, width, height);
//j			transformedSpace = CRect(0, 0, width, height);
//j			transformSpace(transformedSpace);

			if (dst == NULL)
			{
//j				COLORREF dstCM;
//j				dstCM = src.getColorModel();
//j				dst = new BufferedImage(dstCM, transformedSpace.Width(), transformedSpace.Height());
			}
			BITMAPINFOHEADER *dstRaster = dst;

//j			Array<int> *inPixels = getRGB(src, 0, 0, width, height, NULL);
//j			inPixels = filterPixels(width, height, inPixels, transformedSpace);
//j			setRGB(dst, 0, 0, transformedSpace.Width(), transformedSpace.Height(), inPixels);

			return dst;
		}

	public:
		virtual void transformSpace(CRect& rect)
		{
		}

		virtual Array<int> *filterPixels(int, int, Array<int>*, CRect&) abstract;
	};
}
