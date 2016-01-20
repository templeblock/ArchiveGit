#pragma once
#include "PointFilter.h"

namespace ImageEffects
{
	class ReduceFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property int NumLevels
		{
			int get()
			{
				return numLevels;
			}

			void set(int value)
			{
				numLevels = value;
				initialized = false;
			}
		}
#endif NOTUSED

	private:
		int numLevels;
		Array<int> *levels;
		bool initialized;

	public:
		ReduceFilter()
		{
			numLevels = 6;
		}

	public:
		virtual void initialize()
		{
			levels = new Array<int>(256);
			if (numLevels != 1)
				for (int i = 0; i < 256; i++)
					levels->SetValue(255 * (numLevels * i / 256) / (numLevels - 1), i);
		}

	public:
		virtual int filterRGB(int x, int y, int rgb)
		{
			if (!initialized)
			{
				initialized = true;
				initialize();
			}

			int a = rgb & ((int)(0xff000000));
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			r = levels->GetAt(r);
			g = levels->GetAt(g);
			b = levels->GetAt(b);
			return a | (r << 16) | (g << 8) | b;
		}

		virtual CString *ToString()
		{
			return new CString("Colors/Posterize...");
		}
	};
}
