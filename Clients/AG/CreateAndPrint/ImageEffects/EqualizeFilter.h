#pragma once
#include "Histogram.h"
#include "WholeImageFilter.h"

namespace ImageEffects
{
	class EqualizeFilter : WholeImageFilter
	{
	public:
		int lut[3][256];
		bool bOkLut;

	public:
		EqualizeFilter()
		{
			bOkLut = false;
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels)
		{
			Histogram *histogram = new Histogram(inPixels, width, height, 0, width);

			int i, j;

			if (histogram->numSamples > 0)
			{
				float scale = 255.0f / histogram->numSamples;
				for (i = 0; i < 3; i++)
				{
					lut[i][0] = histogram->getFrequency(i, 0);
					for (j = 1; j < 256; j++)
						lut[i][j] = lut[i][j - 1] + histogram->getFrequency(i, j);
					for (j = 0; j < 256; j++)
					{
						lut[i][j] = (int)(lut[i][j] * scale + 0.5);
					}
				}

				bOkLut = true;
			}
			else
				bOkLut = false;

			i = 0;
			for (int y = 0; y < height; y++)
				for (int x = 0; x < width; x++)
				{
					inPixels->SetValue(filterRGB(x, y, inPixels->GetAt(i)), i);
					i++;
				}
			bOkLut = false;

			return inPixels;
		}

	public:
		virtual int filterRGB(int x, int y, int rgb)
		{
			if (bOkLut)
			{
				int a = rgb & ((int)(0xff000000));
				int r = lut[Histogram::RED][(rgb >> 16) & 0xff];
				int g = lut[Histogram::GREEN][(rgb >> 8) & 0xff];
				int b = lut[Histogram::BLUE][rgb & 0xff];

				return a | (r << 16) | (g << 8) | b;
			}
			return rgb;
		}

		virtual CString *ToString()
		{
			return new CString("Colors/Equalize");
		}
	};
}