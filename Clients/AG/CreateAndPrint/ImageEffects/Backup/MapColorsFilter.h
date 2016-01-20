#pragma once

#include "PointFilter.h"

namespace ImageEffects
{

	class MapColorsFilter : PointFilter
	{

	private:
		int oldColor;
		int newColor;

	public:
		MapColorsFilter(int oldColor, int newColor)
		{
			canFilterIndexColorModel = true;
			oldColor = oldColor;
			newColor = newColor;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			if (rgb == oldColor)
				return newColor;
			return rgb;
		}
	};
}