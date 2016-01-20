#pragma once

#include "PointFilter.h"

namespace ImageEffects
{

	class GrayFilter : PointFilter
	{

	public:
		GrayFilter()
		{
			canFilterIndexColorModel = true;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			int a = rgb & ((int)(0xff000000));
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			r = (r + 255) / 2;
			g = (g + 255) / 2;
			b = (b + 255) / 2;
			return a | (r << 16) | (g << 8) | b;
		}

		virtual CString *ToString()
		{
			return new CString("Colors/Gray Out");
		}
	};
}