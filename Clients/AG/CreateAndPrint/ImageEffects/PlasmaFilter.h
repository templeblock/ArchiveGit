#pragma once
#include "WholeImageFilter.h"

namespace ImageEffects
{
	class PlasmaFilter : WholeImageFilter
	{
#ifdef NOTUSED
	public:
		virtual property float Turbulence
		{
			float get()
			{
				return turbulence;
			}

			void set(float value)
			{
				turbulence = value;
			}

		}
		virtual property float Scaling
		{
			float get()
			{
				return scaling;
			}

			void set(float value)
			{
				scaling = value;
			}

		}
		virtual property Colormap *Colormap
		{
			Colormap *get()
			{
				return colormap;
			}

			void set(Colormap *value)
			{
				colormap = value;
			}

		}
		virtual property bool UseImageColors
		{
			bool get()
			{
				return useImageColors;
			}

			void set(bool value)
			{
				useImageColors = value;
			}

		}
#endif NOTUSED

	public:
		float turbulence;
	private:
		float scaling;
		Colormap *colormap;
		bool useImageColors;

	public:
		PlasmaFilter()
		{
			turbulence = 1.0f;
			scaling = 0.0f;
			colormap = (Colormap*)new LinearColormap();
		}

		virtual void randomize()
		{
			srand(567);/*GetTicks*/;
		}

	private:
		int randomRGB(Array<int> *inPixels, int x, int y)
		{
			if (useImageColors)
			{
				return inPixels->GetAt(y * originalSpace.Width() + x);
			}
			else
			{
				int r = (int)(255 * (float)(rand/*NextDouble*/()));
				int g = (int)(255 * (float)(rand/*NextDouble*/()));
				int b = (int)(255 * (float)(rand/*NextDouble*/()));
				return ((int)(0xff000000)) | (r << 16) | (g << 8) | b;
			}
		}

		int displace(int rgb, float amount)
		{
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			r = PixelUtils::clamp(r + (int)(amount * ((float)(rand/*NextDouble*/() - 0.5))));
			g = PixelUtils::clamp(g + (int)(amount * ((float)(rand/*NextDouble*/() - 0.5))));
			b = PixelUtils::clamp(b + (int)(amount * ((float)(rand/*NextDouble*/() - 0.5))));
			return ((int)(0xff000000)) | (r << 16) | (g << 8) | b;
		}

		int average(int rgb1, int rgb2)
		{
			return PixelUtils::combinePixels(rgb1, rgb2, PixelUtils::AVERAGE);
		}

		int getPixel(int x, int y, Array<int> *pixels, int stride)
		{
			return pixels->GetAt(y * stride + x);
		}

		void putPixel(int x, int y, int rgb, Array<int> *pixels, int stride)
		{
			pixels->SetValue(rgb, y * stride + x);
		}

		bool doPixel(int x1, int y1, int x2, int y2, Array<int> *pixels, int stride, int depth, int scale)
		{
			int mx, my;

			if (depth == 0)
			{
				int ml, mr, mt, mb, mm, t;

				int tl = getPixel(x1, y1, pixels, stride);
				int bl = getPixel(x1, y2, pixels, stride);
				int tr = getPixel(x2, y1, pixels, stride);
				int br = getPixel(x2, y2, pixels, stride);

				float amount = (256.0f / (2.0f * scale)) * turbulence;

				mx = (x1 + x2) / 2;
				my = (y1 + y2) / 2;

				if (mx == x1 && mx == x2 && my == y1 && my == y2)
					return true;

				if (mx != x1 || mx != x2)
				{
					ml = average(tl, bl);
					ml = displace(ml, amount);
					putPixel(x1, my, ml, pixels, stride);

					if (x1 != x2)
					{
						mr = average(tr, br);
						mr = displace(mr, amount);
						putPixel(x2, my, mr, pixels, stride);
					}
				}

				if (my != y1 || my != y2)
				{
					if (x1 != mx || my != y2)
					{
						mb = average(bl, br);
						mb = displace(mb, amount);
						putPixel(mx, y2, mb, pixels, stride);
					}

					if (y1 != y2)
					{
						mt = average(tl, tr);
						mt = displace(mt, amount);
						putPixel(mx, y1, mt, pixels, stride);
					}
				}

				if (y1 != y2 || x1 != x2)
				{
					mm = average(tl, br);
					t = average(bl, tr);
					mm = average(mm, t);
					mm = displace(mm, amount);
					putPixel(mx, my, mm, pixels, stride);
				}

				if (x2 - x1 < 3 && y2 - y1 < 3)
					return false;
				return true;
			}

			mx = (x1 + x2) / 2;
			my = (y1 + y2) / 2;

			doPixel(x1, y1, mx, my, pixels, stride, depth - 1, scale + 1);
			doPixel(x1, my, mx, y2, pixels, stride, depth - 1, scale + 1);
			doPixel(mx, y1, x2, my, pixels, stride, depth - 1, scale + 1);
			return doPixel(mx, my, x2, y2, pixels, stride, depth - 1, scale + 1);
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			Array<int> *outPixels = new Array<int>(width * height);

			srand(567);/*GetTicks*/;

			int w1 = width - 1;
			int h1 = height - 1;
			putPixel(0, 0, randomRGB(inPixels, 0, 0), outPixels, width);
			putPixel(w1, 0, randomRGB(inPixels, w1, 0), outPixels, width);
			putPixel(0, h1, randomRGB(inPixels, 0, h1), outPixels, width);
			putPixel(w1, h1, randomRGB(inPixels, w1, h1), outPixels, width);
			putPixel(w1 / 2, h1 / 2, randomRGB(inPixels, w1 / 2, h1 / 2), outPixels, width);
			putPixel(0, h1 / 2, randomRGB(inPixels, 0, h1 / 2), outPixels, width);
			putPixel(w1, h1 / 2, randomRGB(inPixels, w1, h1 / 2), outPixels, width);
			putPixel(w1 / 2, 0, randomRGB(inPixels, w1 / 2, 0), outPixels, width);
			putPixel(w1 / 2, h1, randomRGB(inPixels, w1 / 2, h1), outPixels, width);

			int depth = 1;
			while (doPixel(0, 0, width - 1, height - 1, outPixels, width, depth, 0))
				depth++;

			if (colormap != NULL)
			{
				int index = 0;
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						outPixels->SetValue(colormap->getColor((outPixels->GetAt(index) & 0xff) / 255.0f), index);
						index++;
					}
				}
			}
			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Texture/Plasma...");
		}
	};
}
