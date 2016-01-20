#pragma once

#include "ImageMath.h"

namespace ImageEffects
{

	/// A colormap with the colors of the spectrum.

	class SpectrumColormap : Colormap
	{

		/// Construct a spcetrum color map
	public:
		SpectrumColormap()
		{
		}

		/// Convert a value in the range 0..1 to an RGB color.
		/// <param name="v">a value in the range 0..1
		/// </param>
		/// <returns> an RGB color
		/// </returns>
		virtual int getColor(float v)
		{
			return Spectrum::wavelengthToRGB(380 + 400 * clamp(v, 0.0, 1.0));
		}
	};
}