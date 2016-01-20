#pragma once
#include "ImageMath.h"
#include "TransformFilter.h"

namespace ImageEffects
{
	class MapFilter : TransformFilter
	{
	public:
#ifdef NOTUSED
		virtual property Function2D *XMapFunction
		{
			Function2D *get()
			{
				return xMapFunction;
			}

			void set(Function2D *value)
			{
				xMapFunction = value;
			}

		}
		virtual property Function2D *YMapFunction
		{
			Function2D *get()
			{
				return yMapFunction;
			}

			void set(Function2D *value)
			{
				yMapFunction = value;
			}

		}
#endif NOTUSED

	private:
		Function2D *xMapFunction;
		Function2D *yMapFunction;

	public:
		MapFilter()
		{
		}

	public:
		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			float xMap, yMap;
			xMap = xMapFunction->evaluate((float)x, (float)y);
			yMap = yMapFunction->evaluate((float)x, (float)y);
			int width = 99; //j transformedSpace->Width;
			int height = 99; //j transformedSpace->Height;
			out_Renamed->SetValue(xMap * width, 0);
			out_Renamed->SetValue(yMap * height, 1);
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Map Coordinates...");
		}
	};
}