#pragma once

namespace ImageEffects
{
	class RGBAdjustFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property float RFactor
		{
			float get()
			{
				return rFactor - 1;
			}

			void set(float value)
			{
				rFactor = 1 + value;
			}

		}
		virtual property float GFactor
		{
			float get()
			{
				return gFactor - 1;
			}

			void set(float value)
			{
				gFactor = 1 + value;
			}

		}
		virtual property float BFactor
		{
			float get()
			{
				return bFactor - 1;
			}

			void set(float value)
			{
				bFactor = 1 + value;
			}

		}
#endif NOTUSED

	private:


	public:
		float rFactor, gFactor, bFactor;

		RGBAdjustFilter()
		{
			rFactor = gFactor = bFactor = 0;
		}

		RGBAdjustFilter(float r, float g, float b)
		{
			rFactor = 1 + r;
			gFactor = 1 + g;
			bFactor = 1 + b;
			canFilterIndexColorModel = true;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			int a = rgb & ((int)(0xff000000));
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			r = PixelUtils::clamp((int)(r * rFactor));
			g = PixelUtils::clamp((int)(g * gFactor));
			b = PixelUtils::clamp((int)(b * bFactor));
			return a | (r << 16) | (g << 8) | b;
		}

		virtual CString *ToString()
		{
			return new CString("Colors/Adjust RGB...");
		}
	};
}