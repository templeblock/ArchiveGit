#pragma once
#include "WholeImageFilter.h"

namespace ImageEffects
{
	class MedianFilter : WholeImageFilter
	{

	private:


	public:
		MedianFilter()
		{
		}

	private:
		int median(CAtlArray<int> iArray)
		{
			int max, maxIndex;

			for (int i = 0; i < 4; i++)
			{
				max = 0;
				maxIndex = 0;
				for (int j = 0; j < 9; j++)
				{
					if (iArray[j] > max)
					{
						max = iArray[j];
						maxIndex = j;
					}
				}
				iArray[maxIndex] = 0;
			}
			max = 0;
			for (int i = 0; i < 9; i++)
			{
				if (iArray[i] > max)
					max = iArray[i];
			}
			return max;
		}

	public:
		int rgbMedian(CAtlArray<int>& r, CAtlArray<int>& g, CAtlArray<int>& b)
		{
			int sum, index = 0, min = MAXLONG;

			for (int i = 0; i < 9; i++)
			{
				sum = 0;
				for (int j = 0; j < 9; j++)
				{
					sum += abs(r[i] - r[j]);
					sum += abs(g[i] - g[j]);
					sum += abs(b[i] - b[j]);
				}
				if (sum < min)
				{
					min = sum;
					index = i;
				}
			}
			return index;
		}

	public:
		virtual CAtlArray<int>& filterPixels(int width, int height, CAtlArray<int> inPixels)
		{
			int index = 0;
			CAtlArray<int> argb; argb.SetCount(9);
			CAtlArray<int> r; r.SetCount(9);
			CAtlArray<int> g; g.SetCount(9);
			CAtlArray<int> b; b.SetCount(9);

			CAtlArray<int>* poutPixels = new CAtlArray<int>;
			CAtlArray<int>& outPixels = *poutPixels;
			outPixels.SetCount(width * height);

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int k = 0;
					for (int dy = - 1; dy <= 1; dy++)
					{
						int iy = y + dy;
						if (0 <= iy && iy < height)
						{
							int ioffset = iy * width;
							for (int dx = - 1; dx <= 1; dx++)
							{
								int ix = x + dx;
								if (0 <= ix && ix < width)
								{
									int rgb = inPixels[ioffset + ix];
									argb[k] = rgb;
									r[k] = (rgb >> 16) & 0xff;
									g[k] = (rgb >> 8) & 0xff;
									b[k] = rgb & 0xff;
									k++;
								}
							}
						}
					}
					while (k < 9)
					{
						argb[k] = ((int)(0xff000000));
						r[k] = g[k] = b[k] = 0;
						k++;
					}

					int m = rgbMedian(r, g, b);
					outPixels[index++] = argb[m];
				}
			}
			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Blur/Median");
		}
	};
}