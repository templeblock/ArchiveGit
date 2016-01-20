#pragma once

//j#include "Gradient.h"
#include "Colormap.h"
#include "PointFilter.h"

namespace ImageEffects
{
	class LookupFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property Colormap *Colormap
		{
			Colormap *get()
			{
				return colormap;
			}

			void set(Colormap *value)
			{
				colormap = value;
			}
		}
#endif NOTUSED

	private:


	private:
		Colormap *colormap;

	public:
		LookupFilter()
		{
			colormap = NULL; //j new Gradient();
			canFilterIndexColorModel = true;
		}

		LookupFilter(Colormap *newcolormap)
		{
			canFilterIndexColorModel = true;
			colormap = newcolormap;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			rgb = (r + g + b) / 3;
			return colormap->getColor(rgb / 255.0f);
		}

		virtual CString *ToString()
		{
			return new CString("Colors/Lookup...");
		}
	};
}