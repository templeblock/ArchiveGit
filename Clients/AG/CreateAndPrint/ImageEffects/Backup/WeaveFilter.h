#pragma once
#include "PointFilter.h"

namespace ImageEffects
{
	class WeaveFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property float XGap
		{
			float get()
			{
				return xGap;
			}

			void set(float value)
			{
				xGap = value;
			}

		}
		virtual property float XWidth
		{
			float get()
			{
				return xWidth;
			}

			void set(float value)
			{
				xWidth = value;
			}

		}
		virtual property float YWidth
		{
			float get()
			{
				return yWidth;
			}

			void set(float value)
			{
				yWidth = value;
			}

		}
		virtual property float YGap
		{
			float get()
			{
				return yGap;
			}

			void set(float value)
			{
				yGap = value;
			}

		}
		virtual property Array<Array<int>*> *Crossings
		{
			Array<Array<int>*> *get()
			{
				return matrix;
			}

			void set(Array<Array<int>*> *value)
			{
				matrix = value;
			}

		}
		virtual property bool UseImageColors
		{
			bool get()
			{
				return useImageColors;
			}

			void set(bool value)
			{
				useImageColors = value;
			}

		}
		virtual property bool RoundThreads
		{
			bool get()
			{
				return roundThreads;
			}

			void set(bool value)
			{
				roundThreads = value;
			}

		}
		virtual property bool ShadeCrossings
		{
			bool get()
			{
				return shadeCrossings;
			}

			void set(bool value)
			{
				shadeCrossings = value;
			}

		}
#endif NOTUSED

	private:
		float xWidth;
		float yWidth;
		float xGap;
		float yGap;
		int rows;
		int cols;
		int rgbX;
		int rgbY;
		bool useImageColors;
		bool roundThreads;
		bool shadeCrossings;

	public:
		WeaveFilter()
		{
			xWidth = 16;
			yWidth = 16;
			xGap = 6;
			yGap = 6;
			rows = 4;
			cols = 4;
			rgbX = ((int)(0xffff8080));
			rgbY = ((int)(0xff8080ff));
			useImageColors = true;
			shadeCrossings = true;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			static const int matrix[4][4] = {
				{0, 1, 0, 1}, 
				{1, 0, 1, 0}, 
				{0, 1, 0, 1}, 
				{1, 0, 1, 0}};

			x = (int)(x + xWidth + xGap / 2);
			y = (int)(y + yWidth + yGap / 2);
			float nx = mod((float)x, xWidth + xGap);
			float ny = mod((float)y, yWidth + yGap);
			int ix = (int)(x / (xWidth + xGap));
			int iy = (int)(y / (yWidth + yGap));
			bool inX = nx < xWidth;
			bool inY = ny < yWidth;
			float dX, dY;
			float cX, cY;
			int lrgbX, lrgbY;

			if (roundThreads)
			{
				dX = abs(xWidth / 2 - nx) / xWidth / 2;
				dY = abs(yWidth / 2 - ny) / yWidth / 2;
			}
			else
			{
				dX = dY = 0;
			}

			if (shadeCrossings)
			{
				cX = smoothStep(xWidth / 2, xWidth / 2 + xGap, abs(xWidth / 2 - nx));
				cY = smoothStep(yWidth / 2, yWidth / 2 + yGap, abs(yWidth / 2 - ny));
			}
			else
			{
				cX = cY = 0;
			}

			if (useImageColors)
			{
				lrgbX = lrgbY = rgb;
			}
			else
			{
				lrgbX = rgbX;
				lrgbY = rgbY;
			}
			int v;
			int ixc = ix&  cols;
			int iyr = iy&  rows;
			int m = matrix[iyr][ixc];
			if (inX)
			{
				if (inY)
				{
					v = m == 1?lrgbX:lrgbY;
					v = mixColors(2 * (m == 1?dX:dY), v, ((int)(0xff000000)));
				}
				else
				{
					if (shadeCrossings)
					{
						if (m != matrix[(iy + 1)&  rows][ixc])
						{
							if (m == 0)
								cY = 1 - cY;
							cY *= 0.5f;
							lrgbX = mixColors(cY, lrgbX, ((int)(0xff000000)));
						}
						else if (m == 0)
							lrgbX = mixColors(0.5f, lrgbX, ((int)(0xff000000)));
					}
					v = mixColors(2 * dX, lrgbX, ((int)(0xff000000)));
				}
			}
			else if (inY)
			{
				if (shadeCrossings)
				{
					if (m != matrix[iyr][(ix + 1)&  cols])
					{
						if (m == 1)
							cX = 1 - cX;
						cX *= 0.5f;
						lrgbY = mixColors(cX, lrgbY, ((int)(0xff000000)));
					}
					else if (m == 1)
						lrgbY = mixColors(0.5f, lrgbY, ((int)(0xff000000)));
				}
				v = mixColors(2 * dY, lrgbY, ((int)(0xff000000)));
			}
			else
				v = 0x00000000;
			return v;
		}

		virtual CString *ToString()
		{
			return new CString("Texture/Weave...");
		}
	};
}
