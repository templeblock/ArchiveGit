#pragma once

#include "PointFilter.h"

namespace ImageEffects
{

	/// A filter which inverts the RGB channels of an image.
	class InvertFilter : PointFilter
	{

	public:
		InvertFilter()
		{
			canFilterIndexColorModel = true;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			int a = rgb & ((int)(0xff000000));
			return a | (~ rgb & 0x00ffffff);
		}

		virtual CString *ToString()
		{
			return new CString("Colors/Invert");
		}
	};
}