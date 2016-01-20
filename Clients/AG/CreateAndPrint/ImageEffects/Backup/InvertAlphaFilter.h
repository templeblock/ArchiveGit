#pragma once

#include "PointFilter.h"

namespace ImageEffects
{
	/// A Filter to invert the alpha channel of an image. This is really only useful for inverting selections, where we only use the alpha channel.
	class InvertAlphaFilter : PointFilter
	{

	public:
		InvertAlphaFilter()
		{
			canFilterIndexColorModel = true;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			return rgb * ((int)(0xff000000));
		}

		virtual CString *ToString()
		{
			return new CString("Alpha/Invert");
		}
	};
}