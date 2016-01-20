#pragma once

#include "Colormap.h"

namespace ImageEffects
{
	/// A colormap which interpolates linearly between two colors.

	class LinearColormap : Colormap
	{
		/// Get the first color
		/// <returns> the color corresponding to value 0 in the colormap
		/// </returns>
		/// Set the first color
		/// <param name="color1">the color corresponding to value 0 in the colormap
		/// </param>
#ifdef NOTUSED
	public:
		virtual property int Color1
		{
			int get()
			{
				return color1;
			}

			void set(int value)
			{
				color1 = value;
			}
		}

		/// Get the second color
		/// <returns> the color corresponding to value 1 in the colormap
		/// </returns>
		/// Set the second color
		/// <param name="color2">the color corresponding to value 1 in the colormap
		/// </param>
		virtual property int Color2
		{
			int get()
			{
				return color2;
			}

			void set(int value)
			{
				color2 = value;
			}
		}
#endif NOTUSED

	private:


	private:
		int color1;
		int color2;

		/// Construct a color map with a grayscale ramp from black to white
	public:
		LinearColormap()
		{
			color1 = (int)0xff000000;
			color2 = (int)0xffffffff;
		}

		/// Construct a linear color map
		/// <param name="color1">the color corresponding to value 0 in the colormap
		/// </param>
		/// <param name="color2">the color corresponding to value 1 in the colormap
		/// </param>
		LinearColormap(int newcolor1, int newcolor2)
		{
			color1 = newcolor1;
			color2 = newcolor2;
		}

		/// Convert a value in the range 0..1 to an RGB color.
		/// <param name="v">a value in the range 0..1
		/// </param>
		/// <returns> an RGB color
		/// </returns>
		virtual int getColor(float v)
		{
			return mixColors(clamp(v, 0.0, 1.0), color1, color2);
		}
	};
}