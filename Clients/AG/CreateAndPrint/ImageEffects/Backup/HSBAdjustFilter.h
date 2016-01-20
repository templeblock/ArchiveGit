#pragma once

#include "PointFilter.h"

namespace ImageEffects
{


	class HSBAdjustFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property float HFactor
		{
			float get()
			{
				return hFactor;
			}

			void set(float value)
			{
				hFactor = value;
			}

		}
		virtual property float SFactor
		{
			float get()
			{
				return sFactor;
			}

			void set(float value)
			{
				sFactor = value;
			}

		}
		virtual property float BFactor
		{
			float get()
			{
				return bFactor;
			}

			void set(float value)
			{
				bFactor = value;
			}

		}
#endif NOTUSED

	private:


	public:
		float hFactor, sFactor, bFactor;
	private:
		float hsb[3];

	public:
		HSBAdjustFilter()
		{
			hFactor = sFactor = bFactor = 0.0;
			hsb[0] = hsb[1] = hsb[2] = 0.0;
		}

		HSBAdjustFilter(float r, float g, float b)
		{
			hFactor = r;
			sFactor = g;
			bFactor = b;
			canFilterIndexColorModel = true;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			int a = rgb & ((int)(0xff000000));
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
//j			RGBtoHSB(r, g, b, hsb);
			hsb[0] += hFactor;
			while (hsb[0] < 0)
				hsb[0] = (float)(hsb[0] + PI * 2);
			hsb[1] += sFactor;
			if (hsb[1] < 0)
				hsb[1] = 0;
			else if (hsb[1] > 1.0)
				hsb[1] = 1.0f;
			hsb[2] += bFactor;
			if (hsb[2] < 0)
				hsb[2] = 0;
			else if (hsb[2] > 1.0)
				hsb[2] = 1.0f;
			rgb = NULL; //j HSBtoRGB(hsb[0], hsb[1], hsb[2]);
			return a | (rgb & 0xffffff);
		}

		virtual CString *ToString()
		{
			return new CString("Colors/Adjust HSB...");
		}
	};
}
