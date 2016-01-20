#pragma once
//j#include "GaussianFilter.h"

namespace ImageEffects
{
	class StampFilter : PointFilter
	{
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
			}

		}
		virtual property float Threshold
		{
			float get()
			{
				return threshold;
			}

			void set(float value)
			{
				threshold = value;
			}

		}
		virtual property float Softness
		{
			float get()
			{
				return softness;
			}

			void set(float value)
			{
				softness = value;
			}

		}
		virtual property int White
		{
			int get()
			{
				return white;
			}

			void set(int value)
			{
				white = value;
			}

		}
		virtual property int Black
		{
			int get()
			{
				return black;
			}

			void set(int value)
			{
				black = value;
			}

		}
#endif NOTUSED

	private:
		float threshold;
		float softness;
	public:
		float radius;
	private:
		float lowerThreshold3;
		float upperThreshold3;
		int white;
		int black;

	public:
		StampFilter()
		{
			threshold = 0.5;
			radius = 5;
			white = (int)0xffffffff;
			black = (int)0xff000000;
		}

		StampFilter(float newthreshold)
		{
			threshold = newthreshold;
			radius = 5;
			white = (int)0xffffffff;
			black = (int)0xff000000;
		}

		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
//j			dst = new GaussianFilter((int)(radius))->filter(src, NULL);
			lowerThreshold3 = 255 * 3 * (threshold - softness * 0.5f);
			upperThreshold3 = 255 * 3 * (threshold + softness * 0.5f);
			return PointFilter::filter(dst, dst);
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			int a = rgb & ((int)(0xff000000));
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			int l = r + g + b;
			float f = smoothStep(lowerThreshold3, upperThreshold3, (float)l);
			return mixColors(f, black, white);
		}

		virtual CString *ToString()
		{
			return new CString("Stylize/Stamp...");
		}
	};
}
