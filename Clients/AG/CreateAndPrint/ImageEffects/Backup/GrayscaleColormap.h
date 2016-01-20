#pragma once
#include "Colormap.h"

namespace ImageEffects
{
	/// A grayscale colormap. Black is 0, white is 1.
	class GrayscaleColormap : Colormap
	{
	public:
		GrayscaleColormap()
		{
		}

		/// Convert a value in the range 0..1 to an RGB color.
		/// <param name="v">a value in the range 0..1</param>
		/// <returns> an RGB color</returns>
		virtual int getColor(float v)
		{
			int n = (int)(v * 255);
			if (n < 0)
				n = 0;
			else if (n > 255)
				n = 255;
			return ((int)(0xff000000)) | (n << 16) | (n << 8) | n;
		}
	};
}
