#pragma once
#include "Kernel.h"
//j#include "ConvolveFilter.h"

namespace ImageEffects
{
	/// A filter which applies Gaussian blur to an image. This is a subclass of ConvolveFilter
	/// which simply creates a kernel with a Gaussian distribution for blurring.
	class GaussianFilter //j: ConvolveFilter
	{
		/// Get the radius of the kernel.
		/// <returns> the radius</returns>
		/// Set the radius of the kernel, and hence the amount of blur. The bigger the radius, the longer this filter will take.
		/// <param name="radius">the radius of the blur in pixels.</param>
#ifdef NOTUSED
	public:
		virtual property float Radius
		{
			float get()
			{
				return radius;
			}

			void set(float value)
			{
				radius = value;
				kernel = makeKernel(value);
			}

		}
#endif NOTUSED

	public:
		float radius;
		Kernel *kernel;

		/// Construct a Gaussian filter
	public:
		GaussianFilter()
		{
			radius = 2;
		}

		/// Construct a Gaussian filter
		/// <param name="radius">blur radius in pixels</param>
		GaussianFilter(float newradius)
		{
			radius = newradius;
		}

		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int width = src->biWidth;
			int height = src->biHeight;

			if (dst == NULL)
			{
//j				COLORREF tempAux = CLR_NONE;
//j				dst = createCompatibleDestImage(src, tempAux);
			}

			Array<int> *inPixels = new Array<int>(width * height);
			Array<int> *outPixels = new Array<int>(width * height);
//j			src->getRGB(0, 0, width, height, inPixels, 0, width);

			if (radius > 0)
			{
//j				convolveAndTranspose(kernel, inPixels, outPixels, width, height, alpha, CLAMP_EDGES);
//j				convolveAndTranspose(kernel, outPixels, inPixels, height, width, alpha, CLAMP_EDGES);
			}

//j			dst->setRGB(0, 0, width, height, inPixels, 0, width);
			return dst;
		}

		static void convolveAndTranspose(Kernel *kernel, Array<int> *inPixels, Array<int> *outPixels, int width, int height, bool alpha, int edgeAction)
		{
			Array<float> *matrix = kernel->getKernelData(NULL);
			int cols = kernel->getWidth();
			int cols2 = cols / 2;

			for (int y = 0; y < height; y++)
			{
				int index = y;
				int ioffset = y * width;
				for (int x = 0; x < width; x++)
				{
					float r = 0, g = 0, b = 0, a = 0;
					int moffset = cols2;
					for (int col = - cols2; col <= cols2; col++)
					{
						float f = matrix->GetAt(moffset + col);

						if (f != 0)
						{
							int ix = x + col;
							if (ix < 0)
							{
//j								if (edgeAction == CLAMP_EDGES)
//j									ix = 0;
//j								else if (edgeAction == WRAP_EDGES)
//j									ix = (x + width)&  width;
							}
							else if (ix >= width)
							{
//j								if (edgeAction == CLAMP_EDGES)
//j									ix = width - 1;
//j								else if (edgeAction == WRAP_EDGES)
//j									ix = (x + width)&  width;
							}
							int rgb = inPixels->GetAt(ioffset + ix);
							a += f * ((rgb >> 24) & 0xff);
							r += f * ((rgb >> 16) & 0xff);
							g += f * ((rgb >> 8) & 0xff);
							b += f * (rgb & 0xff);
						}
					}
					int ia = alpha?PixelUtils::clamp((int)(a + 0.5)):0xff;
					int ir = PixelUtils::clamp((int)(r + 0.5));
					int ig = PixelUtils::clamp((int)(g + 0.5));
					int ib = PixelUtils::clamp((int)(b + 0.5));
					outPixels->SetValue((ia << 24) | (ir << 16) | (ig << 8) | ib, index);
					index += height;
				}
			}
		}

		/// Make a Gaussian blur kernel.
		static Kernel *makeKernel(float radius)
		{
			int r = (int)(ceil(radius));
			int rows = r * 2 + 1;
			Array<float> *matrix = new Array<float>(rows);
			float sigma = radius / 3;
			float sigma22 = 2 * sigma * sigma;
			float sigmaPi2 = 2 * PI * sigma;
			float sqrtSigmaPi2 = (float)(sqrt(sigmaPi2));
			float radius2 = radius * radius;
			float total = 0;
			int index = 0;
			for (int row = - r; row <= r; row++)
			{
				float distance = (float)row * row;
				if (distance > radius2)
					matrix->SetValue(0, index);
				else
				{
					matrix->SetValue((float)(exp(-distance) / sigma22) / sqrtSigmaPi2, index);
				}
				total += matrix->GetAt(index);
				index++;
			}
			for (int i = 0; i < rows; i++)
				matrix->SetValue(i / total, i);

			return new Kernel(rows, 1, matrix);
		}

		virtual CString *ToString()
		{
			return new CString("Blur/Gaussian Blur...");
		}
	};
}
