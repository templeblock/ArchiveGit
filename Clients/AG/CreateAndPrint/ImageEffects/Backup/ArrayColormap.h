#pragma once

//#include "ImageMath.h"
#include "Colormap.h"

namespace ImageEffects
{
	/// A colormap implemented with an Array of colors. This corresponds to the IndexColorModel class.
	class ArrayColormap : Colormap
	{
	public:
#ifdef NOTUSED
		virtual property int *Map
		{
			int *get()
			{
				return map;
			}

			void set(int *value)
			{
				map = value;
			}
		}
#endif NOTUSED

	/// The Array of colors.
	public:
		int map[256];

		/// Construct an all-black colormap
	public:
		ArrayColormap()
		{
		}

		/// Construct a colormap with the given map
		/// <param name="map">the Array of ARGB colors</param>
		ArrayColormap(int *newmap)
		{
			for (int i=0; i<256; i++)
				map[i] = newmap[i];
		}

		virtual System::Object *Clone()
		{
			try
			{
				ArrayColormap *g = NULL; //j (ArrayColormap*)(MemberwiseClone());
//j				g->map = new Array<int>(map->Length);
//j				map->CopyTo(g->map, 0);
				return (System::Object*)g;
			}
			catch (...)
			{
			}
			return NULL;
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
			return map[n];
		}

		/// Set the color at "index" to "color". Entries are interpolated linearly from
		/// the existing entries at "firstIndex" and "lastIndex" to the new entry.
		/// firstIndex < index < lastIndex must hold.
		virtual void setColorInterpolated(int index, int firstIndex, int lastIndex, int color)
		{
			int firstColor = map[firstIndex];
			int lastColor = map[lastIndex];
			for (int i = firstIndex; i <= index; i++)
			{
				map[i] = mixColors((float)(i - firstIndex) / (index - firstIndex), firstColor, color);
			}
			for (int i = index; i < lastIndex; i++)
			{
				map[i] = mixColors((float)(i - index) / (lastIndex - index), color, lastColor);
			}
		}

		virtual void setColorRange(int firstIndex, int lastIndex, int color1, int color2)
		{
			for (int i = firstIndex; i <= lastIndex; i++)
			{
				map[i] = mixColors((float)(i - firstIndex) / (lastIndex - firstIndex), color1, color2);
			}
		}

		virtual void setColorRange(int firstIndex, int lastIndex, int color)
		{
			for (int i = firstIndex; i <= lastIndex; i++)
				map[i] = color;
		}

		virtual void setColor(int index, int color)
		{
			map[index] = color;
		}
	};
}
