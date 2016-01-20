#pragma once
#include "WholeImageFilter.h"

namespace ImageEffects
{
	class OilFilter : WholeImageFilter
	{
#ifdef NOTUSED
	public:
		virtual property int Range
		{
			int get()
			{
				return range;
			}

			void set(int value)
			{
				range = value;
			}

		}
		virtual property int Levels
		{
			int get()
			{
				return levels;
			}

			void set(int value)
			{
				levels = value;
			}

		}
#endif NOTUSED

	private:


	private:
		int range;
		int levels;

	public:
		OilFilter()
		{
			range = 3;
			levels = 256;
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels)
		{
			int index = 0;
			CAtlArray<int> rHistogram; rHistogram.SetCount(levels);
			CAtlArray<int> gHistogram; rHistogram.SetCount(levels);
			CAtlArray<int> bHistogram; rHistogram.SetCount(levels);
			CAtlArray<int> rTotal; rHistogram.SetCount(levels);
			CAtlArray<int> gTotal; rHistogram.SetCount(levels);
			CAtlArray<int> bTotal; rHistogram.SetCount(levels);
			Array<int> *outPixels = new Array<int>(width * height);

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					for (int i = 0; i < levels; i++)
						rHistogram[i] = gHistogram[i] = bHistogram[i] = rTotal[i] = gTotal[i] = bTotal[i] = 0;

					for (int row = - range; row <= range; row++)
					{
						int iy = y + row;
						int ioffset;
						if (0 <= iy && iy < height)
						{
							ioffset = iy * width;
							for (int col = - range; col <= range; col++)
							{
								int ix = x + col;
								if (0 <= ix && ix < width)
								{
									int rgb = inPixels->GetAt(ioffset + ix);
									int r = (rgb >> 16) & 0xff;
									int g = (rgb >> 8) & 0xff;
									int b = rgb & 0xff;
									int ri = r * levels / 256;
									int gi = g * levels / 256;
									int bi = b * levels / 256;
									rTotal[ri] += r;
									gTotal[gi] += g;
									bTotal[bi] += b;
									rHistogram[ri]++;
									gHistogram[gi]++;
									bHistogram[bi]++;
								}
							}
						}
					}

					int r2 = 0, g2 = 0, b2 = 0;
					for (int i = 1; i < levels; i++)
					{
						if (rHistogram[i] > rHistogram[r2])
							r2 = i;
						if (gHistogram[i] > gHistogram[g2])
							g2 = i;
						if (bHistogram[i] > bHistogram[b2])
							b2 = i;
					}
					r2 = rTotal[r2] / rHistogram[r2];
					g2 = gTotal[g2] / gHistogram[g2];
					b2 = rTotal[b2] / bHistogram[b2];
					outPixels->SetValue(((int)(0xff000000)) | (r2 << 16) | (g2 << 8) | b2, index++);
				}
			}

			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Stylize/Oil...");
		}
	};
}