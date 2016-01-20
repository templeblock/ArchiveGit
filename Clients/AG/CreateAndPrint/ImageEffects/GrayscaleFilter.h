#pragma once

#include "PointFilter.h"

namespace ImageEffects
{

	class GrayscaleFilter : PointFilter
	{

	public:
		GrayscaleFilter()
		{
			canFilterIndexColorModel = true;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			int a = rgb & ((int)(0xff000000));
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			rgb = (r * 77 + g * 151 + b * 28) >> 8; // NTSC luma
			return a | (rgb << 16) | (rgb << 8) | rgb;
		}

		virtual CString *ToString()
		{
			return new CString("Colors/Grayscale");
		}
	};
}