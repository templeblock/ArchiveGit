#pragma once

#include "ImageMath.h"
#include "PointFilter.h"

namespace ImageEffects
{

	class JavaLnFFilter : PointFilter
	{

	public:
		JavaLnFFilter()
		{
			canFilterIndexColorModel = true;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			if ((x & 1) == (y & 1))
				return rgb;
			return mixColors(0.25f, ((int)(0xff999999)), rgb);
		}

		virtual CString *ToString()
		{
			return new CString("Stylize/Java L&F Stipple");
		}
	};
}