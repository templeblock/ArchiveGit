#pragma once
#include "WholeImageFilter.h"
#include "Colormap.h"

namespace ImageEffects
{
	class BinaryFilter : WholeImageFilter
	{
#ifdef NOTUSED
	public:
		virtual property int Iterations
		{
			int get()
			{
				return iterations;
			}

			void set(int value)
			{
				iterations = value;
			}
		}
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
		virtual property int NewColor
		{
			int get()
			{
				return newColor;
			}

			void set(int value)
			{
				newColor = value;
			}
		}
		virtual property BinaryFunction *BlackFunction
		{
			BinaryFunction *get()
			{
				return blackFunction;
			}

			void set(BinaryFunction *value)
			{
				blackFunction = value;
			}
		}
#endif NOTUSED

	public:
		int newColor;
		BinaryFunction *blackFunction;
		int iterations;
		Colormap *colormap;

		BinaryFilter()
		{
			newColor = (int)(0xff000000);
			blackFunction = (BinaryFunction*)new BlackFunction();
			iterations = 1;
			colormap = NULL;
		}
	};
}
