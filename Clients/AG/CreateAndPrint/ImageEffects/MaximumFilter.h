#pragma once
#include "WholeImageFilter.h"
#include "PixelUtils.h"

namespace ImageEffects
{


	class MaximumFilter : WholeImageFilter
	{

	public:
		MaximumFilter()
		{
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels)
		{
			int index = 0;
			Array<int> *outPixels = new Array<int>(width * height);

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int pixel = ((int)(0xff000000));
					for (int dy = - 1; dy <= 1; dy++)
					{
						int iy = y + dy;
						int ioffset;
						if (0 <= iy && iy < height)
						{
							ioffset = iy * width;
							for (int dx = - 1; dx <= 1; dx++)
							{
								int ix = x + dx;
								if (0 <= ix && ix < width)
								{
									pixel = PixelUtils::combinePixels(pixel, inPixels->GetAt(ioffset + ix), PixelUtils::MAX);
								}
							}
						}
					}
					outPixels->SetValue(pixel, index++);
				}
			}
			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Blur/Maximum");
		}
	};
}