#pragma once
#include "PointFilter.h"

namespace ImageEffects
{
	class FillFilter : PointFilter
	{
	public:
#ifdef NOTUSED
		virtual property int FillColor
		{
			int get()
			{
				return fillColor;
			}

			void set(int value)
			{
				fillColor = value;
			}

		}
#endif NOTUSED

	private:
		int fillColor;

	public:
		FillFilter()
		{
			fillColor = (int)0xff000000;
		}

		FillFilter(int color)
		{
			fillColor = color;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			return fillColor;
		}
	};
}
