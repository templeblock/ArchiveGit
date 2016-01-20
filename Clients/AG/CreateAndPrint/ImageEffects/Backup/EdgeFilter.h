#pragma once
#include "WholeImageFilter.h"

namespace ImageEffects
{
	class EdgeFilter : WholeImageFilter
	{
#ifdef NOTUSED
	public:
		virtual property Array<float> *VEdgeMatrix
		{
			Array<float> *get()
			{
				return vEdgeMatrix;
			}

			void set(Array<float> *value)
			{
				vEdgeMatrix = value;
			}

		}
		virtual property Array<float> *HEdgeMatrix
		{
			Array<float> *get()
			{
				return hEdgeMatrix;
			}

			void set(Array<float> *value)
			{
				hEdgeMatrix = value;
			}

		}
#endif NOTUSED

	public:
		float *vEdgeMatrix;
		float *hEdgeMatrix;

	public:
		EdgeFilter()
		{
			static const float R2 = (float)(sqrt(2.0f));
			static const float ROBERTS_V[]   = {0, 0, -1, 0, 1, 0, 0, 0, 0};
			static const float ROBERTS_H[]   = {-1, 0, 0, 0, 1, 0, 0, 0, 0};
			static const float PREWITT_V[]   = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
			static const float PREWITT_H[]   = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
			static const float SOBEL_V[]     = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
			static const float SOBEL_H[]     = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
			static const float FREI_CHEN_V[] = {-1, 0, 1, -R2, 0, R2, -1, 0, 1};
			static const float FREI_CHEN_H[] = {-1, -R2, -1, 0, 0, 0, 1, R2, 1};

			vEdgeMatrix = (float*)SOBEL_V;
			hEdgeMatrix = (float*)SOBEL_H;
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			int index = 0;
			Array<int> *outPixels = new Array<int>(width * height);

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int r = 0, g = 0, b = 0;
					int rh = 0, gh = 0, bh = 0;
					int rv = 0, gv = 0, bv = 0;
					int a = inPixels->GetAt(y * width + x) & ((int)(0xff000000));

					for (int row = - 1; row <= 1; row++)
					{
						int iy = y + row;
						int ioffset;
						if (0 <= iy && iy < height)
							ioffset = iy * width;
						else
							ioffset = y * width;
						int moffset = 3 * (row + 1) + 1;
						for (int col = - 1; col <= 1; col++)
						{
							int ix = x + col;
							if (!(0 <= ix && ix < width))
								ix = x;
							int rgb = inPixels->GetAt(ioffset + ix);
							float h = hEdgeMatrix[moffset + col];
							float v = vEdgeMatrix[moffset + col];

							r = (rgb & 0xff0000) >> 16;
							g = (rgb & 0x00ff00) >> 8;
							b = rgb & 0x0000ff;
							rh += (int)(h * r);
							gh += (int)(h * g);
							bh += (int)(h * b);
							rv += (int)(v * r);
							gv += (int)(v * g);
							bv += (int)(v * b);
						}
					}

					r = (int)(sqrt((float)rh * rh + rv * rv) / 1.8f);
					g = (int)(sqrt((float)gh * gh + gv * gv) / 1.8f);
					b = (int)(sqrt((float)bh * bh + bv * bv) / 1.8f);
					r = PixelUtils::clamp(r);
					g = PixelUtils::clamp(g);
					b = PixelUtils::clamp(b);
					outPixels->SetValue(a | (r << 16) | (g << 8) | b, index++);
				}
			}
			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Blur/Detect Edges");
		}
	};
}
