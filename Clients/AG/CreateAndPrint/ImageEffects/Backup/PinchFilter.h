#pragma once
#include "TransformFilter.h"

namespace ImageEffects
{
	class PinchFilter : TransformFilter
	{
	public:
		PinchFilter()
		{
		}

	public:
		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			int width = transformedSpace.Width();
			int height = transformedSpace.Height();
			int m = width / 2;
			float fxvalue = x + ((x - m) * (float)sin(PI * (float)(y) / height));
			out_Renamed->SetValue(fxvalue, 0);
			out_Renamed->SetValue((float)y, 1);
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Pinch");
		}
	};
}
