#pragma once

//


namespace ImageEffects
{

	/// An interface for color maps.  These are passed to filters which convert gray values to
	/// colors. This is similar to the ColorModel class but works with floating point values.
	/// 
	/*public interface*/ class Colormap
	{
		/// Convert a value in the range 0..1 to an RGB color.
		/// <param name="v">a value in the range 0..1</param>
		/// <returns> an RGB color</returns>
		public: virtual int getColor(float) = 0;
	};
}