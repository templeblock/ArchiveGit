#pragma once
//j#include "GaussianFilter.h"

namespace ImageEffects
{
	/// A filter which subtracts Gaussian blur from an image, sharpening it.
	class UnsharpFilter //j: GaussianFilter
	{
#ifdef NOTUSED
	public:
		virtual property int Threshold
		{
			int get()
			{
				return threshold;
			}

			void set(int value)
			{
				threshold = value;
			}

		}
		virtual property float Amount
		{
			float get()
			{
				return amount;
			}

			void set(float value)
			{
				amount = value;
			}

		}
#endif NOTUSED

	private:


	private:
		float amount;
		int threshold;

	public:
		UnsharpFilter()
		{
			amount = 0.5f;
			threshold = 1;
//j			radius = 2;
		}

		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int width = src->biWidth;
			int height = src->biHeight;

			if (dst == NULL)
			{
				COLORREF tempAux = CLR_NONE;
//j				dst = createCompatibleDestImage(src, tempAux);
			}

			Array<int> *inPixels = new Array<int>(width * height);
			Array<int> *outPixels = new Array<int>(width * height);
//j			src->getRGB(0, 0, width, height, inPixels, 0, width);

//j			if (radius > 0)
			{
//j				convolveAndTranspose(kernel, inPixels, outPixels, width, height, alpha, CLAMP_EDGES);
//j				convolveAndTranspose(kernel, outPixels, inPixels, height, width, alpha, CLAMP_EDGES);
			}

//j			src->getRGB(0, 0, width, height, outPixels, 0, width);

			float a = 4 * amount;

			int index = 0;
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int rgb1 = outPixels->GetAt(index);
					int r1 = (rgb1 >> 16) & 0xff;
					int g1 = (rgb1 >> 8) & 0xff;
					int b1 = rgb1 & 0xff;

					int rgb2 = inPixels->GetAt(index);
					int r2 = (rgb2 >> 16) & 0xff;
					int g2 = (rgb2 >> 8) & 0xff;
					int b2 = rgb2 & 0xff;

					if (abs(r1 - r2) >= threshold)
					{
						r1 = PixelUtils::clamp((int)((a + 1) * (r1 - r2) + r2));
					}
					if (abs(g1 - g2) >= threshold)
					{
						g1 = PixelUtils::clamp((int)((a + 1) * (g1 - g2) + g2));
					}
					if (abs(b1 - b2) >= threshold)
					{
						b1 = PixelUtils::clamp((int)((a + 1) * (b1 - b2) + b2));
					}

					inPixels->SetValue((rgb1 & ((int)(0xff000000))) | (r1 << 16) | (g1 << 8) | b1, index);
					index++;
				}
			}

//j			dst->setRGB(0, 0, width, height, inPixels, 0, width);
			return dst;
		}

		virtual CString *ToString()
		{
			return new CString("Blur/Unsharp Mask...");
		}
	};
}
