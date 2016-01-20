#pragma once

namespace ImageEffects
{
	/// An abstract superclass for point filters.
	class PointFilter abstract
	{

	public:
		bool canFilterIndexColorModel;

	public:
		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int width = src->biWidth;
			int height = src->biHeight;
//j			int type = (int)(src->PixelFormat);
			BITMAPINFOHEADER *srcRaster = src;

			if (dst == NULL)
			{
//j				COLORREF tempAux = CLR_NONE;
//j				dst = createCompatibleDestImage(src, tempAux);
			}
			BITMAPINFOHEADER *dstRaster = dst;

			setDimensions(width, height);

//j			Array<int> *inPixels = new Array<int>(width);
			for (int y = 0; y < height; y++)
			{
//j				src->getRGB(0, y, width, 1, inPixels, 0, width);
//j				for (int x = 0; x < width; x++)
//j					inPixels[x] = filterRGB(x, y, inPixels[x]);
//j				dst->setRGB(0, y, width, 1, inPixels, 0, width);
			}

			return dst;
		}

		virtual void setDimensions(int width, int height)
		{
		}

		virtual int filterRGB(int, int, int) abstract;
	};
}
