#pragma once
#include "Histogram.h"
#include "WholeImageFilter.h"

namespace ImageEffects
{
	class LevelsFilter : WholeImageFilter
	{
#ifdef NOTUSED
	public:
		virtual property float LowLevel
		{
			float get()
			{
				return lowLevel;
			}

			void set(float value)
			{
				lowLevel = value;
			}

		}
		virtual property float HighLevel
		{
			float get()
			{
				return highLevel;
			}

			void set(float value)
			{
				highLevel = value;
			}

		}
		virtual property float LowOutputLevel
		{
			float get()
			{
				return lowOutputLevel;
			}

			void set(float value)
			{
				lowOutputLevel = value;
			}

		}
		virtual property float HighOutputLevel
		{
			float get()
			{
				return highOutputLevel;
			}

			void set(float value)
			{
				highOutputLevel = value;
			}

		}
#endif NOTUSED

	public:
		int lut[3][256];
		bool bOkLut;
		float lowLevel;
		float highLevel;
		float lowOutputLevel;
		float highOutputLevel;

	public:
		LevelsFilter()
		{
			highLevel = 1;
			highOutputLevel = 1;
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels)
		{
			Histogram *histogram = new Histogram(inPixels, width, height, 0, width);

			int i, j;

			if (histogram->numSamples > 0)
			{
				float scale = 255.0f / histogram->numSamples;
				float low = lowLevel * 255;
				float high = highLevel * 255;
				if (low == high)
					high++;
				for (i = 0; i < 3; i++)
				{
					for (j = 0; j < 256; j++)
					{
						lut[i][j] = PixelUtils::clamp((int)(255 * (lowOutputLevel + (highOutputLevel - lowOutputLevel) * (j - low) / (high - low))));
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
			return new CString("Colors/Levels...");
		}
	};
}
