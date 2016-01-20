#pragma once
#include "PointFilter.h"

namespace ImageEffects
{
	class OvalFilter : PointFilter
	{
	private:
		float centreX;
		float centreY;
		float a;
		float b;
		float a2;
		float b2;

	public:
		OvalFilter()
		{
		}

		virtual void setDimensions(int width, int height)
		{
			PointFilter::setDimensions(width, height);
			centreX = a = (float)width / 2;
			centreY = b = (float)height / 2;
			a2 = a * a;
			b2 = b * b;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			float dx = x - centreX;
			float dy = y - centreY;
			float x2 = dx * dx;
			float y2 = dy * dy;
			if (y2 >= (b2 - (b2 * x2) / a2))
				return 0x00000000;
			return rgb;
		}

		virtual CString *ToString()
		{
			return new CString("Stylize/Oval...");
		}
	};
}
