#pragma once
#include "PointFilter.h"

namespace ImageEffects
{
	/// Sets the opacity (alpha) of every pixel in an image to a constant value.
	class OpacityFilter : PointFilter
	{
		/// Get the opacity setting.
		/// <returns> the opacity</returns>
		/// Set the opacity.
		/// <param name="opacity">the opacity (alpha) in the range 0..255</param>
#ifdef NOTUSED
	public:
		virtual property int Opacity
		{
			int get()
			{
				return opacity;
			}

			void set(int value)
			{
				opacity = value;
				opacity24 = value << 24;
			}
		}
#endif NOTUSED

	private:
		int opacity;
		int opacity24;

		/// Construct an OpacityFilter with 50% opacity.
	public:
		OpacityFilter()
		{
			opacity = 0x88;
		}

		/// Construct an OpacityFilter with the given opacity (alpha).
		OpacityFilter(int newopacity)
		{
			opacity = newopacity;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			if ((rgb & ((int)(0xff000000))) != 0)
				return (rgb & 0xffffff) | opacity24;
			return rgb;
		}

		virtual CString *ToString()
		{
			return new CString("Colors/Transparency...");
		}
	};
}
