#pragma once
#include "PointFilter.h"

namespace ImageEffects
{
	class MaskFilter : PointFilter
	{
	public:
#ifdef NOTUSED
		virtual property int Mask
		{
			int get()
			{
				return mask;
			}

			void set(int value)
			{
				mask = value;
			}

		}
#endif NOTUSED

	private:
		int mask;

	public:
		MaskFilter()
		{
			mask = (int)0xff00ffff;
		}

		MaskFilter(int newmask)
		{
			canFilterIndexColorModel = true;
			mask = newmask;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			return rgb & mask;
		}

		virtual CString *ToString()
		{
			return new CString("Mask");
		}
	};
}
