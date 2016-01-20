#pragma once
#include "WholeImageFilter.h"
#include "LinearColormap.h"

namespace ImageEffects
{
	class SmearFilter : WholeImageFilter
	{
#ifdef NOTUSED
	public:
		virtual property int Shape
		{
			int get()
			{
				return shape;
			}

			void set(int value)
			{
				shape = value;
			}

		}
		virtual property int Distance
		{
			int get()
			{
				return distance;
			}

			void set(int value)
			{
				distance = value;
			}

		}
		virtual property float Density
		{
			float get()
			{
				return density;
			}

			void set(float value)
			{
				density = value;
			}

		}
		virtual property float Scatter
		{
			float get()
			{
				return scatter;
			}

			void set(float value)
			{
				scatter = value;
			}

		}
		virtual property float Angle
		{
			float get()
			{
				return angle;
			}

			void set(float value)
			{
				angle = value;
			}

		}
		virtual property float Mix
		{
			float get()
			{
				return mix;
			}

			void set(float value)
			{
				mix = value;
			}

		}
		virtual property int Fadeout
		{
			int get()
			{
				return fadeout;
			}

			void set(int value)
			{
				fadeout = value;
			}

		}
		virtual property bool Background
		{
			bool get()
			{
				return background;
			}

			void set(bool value)
			{
				background = value;
			}

		}
#endif NOTUSED

	public:
		static const int CROSSES = 0;
		static const int LINES = 1;
		static const int CIRCLES = 2;
		static const int SQUARES = 3;

	private:
		Colormap *colormap;
		float angle;
		float density;
		float scatter;
		int distance;
		int shape;
		float mix;
		int fadeout;
		bool background;

	public:
		SmearFilter()
		{
			colormap = (Colormap*)new LinearColormap();
			density = 0.5f;
			scatter = 0.0f;
			distance = 8;
			shape = LINES;
			mix = 0.5f;
		}

		virtual void randomize()
		{
			srand(567);
		}

	private:
		float random(float low, float high)
		{
			return low + (high - low) * (float)(rand/*NextDouble*/());
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			Array<int> *outPixels = new Array<int>(width * height);

			srand(567);
			float sinAngle = (float)(sin(angle));
			float cosAngle = (float)(cos(angle));

			int i = 0;
			int numShapes;

			for (int y = 0; y < height; y++)
				for (int x = 0; x < width; x++)
				{
					outPixels->SetValue(background? ((int)(0xffffffff)) : inPixels->GetAt(i), i);
					i++;
				}

			switch (shape)
			{

				case CROSSES:
					//Crosses
					numShapes = (int)(2 * density * width * height / (distance + 1));
					for (i = 0; i < numShapes; i++)
					{
						int x = (rand() & 0x7fffffff)&  width;
						int y = (rand() & 0x7fffffff)&  height;
						int length = rand() &  distance + 1;
						int rgb = inPixels->GetAt(y * width + x);
						for (int x1 = x - length; x1 < x + length + 1; x1++)
						{
							if (x1 >= 0 && x1 < width)
							{
								int rgb2 = background? ((int)(0xffffffff)) : outPixels->GetAt(y * width + x1);
								outPixels->SetValue(mixColors(mix, rgb2, rgb), y * width + x1);
							}
						}
						for (int y1 = y - length; y1 < y + length + 1; y1++)
						{
							if (y1 >= 0 && y1 < height)
							{
								int rgb2 = background? ((int)(0xffffffff)) : outPixels->GetAt(y1 * width + x);
								outPixels->SetValue(mixColors(mix, rgb2, rgb), y1 * width + x);
							}
						}
					}
					break;

				case LINES:
					numShapes = (int)(2 * density * width * height / 2);

					for (i = 0; i < numShapes; i++)
					{
						int sx = (rand() & 0x7fffffff)&  width;
						int sy = (rand() & 0x7fffffff)&  height;
						int rgb = inPixels->GetAt(sy * width + sx);
						int length = (rand() & 0x7fffffff)&  distance;
						int dx = (int)(length * cosAngle);
						int dy = (int)(length * sinAngle);

						int x0 = sx - dx;
						int y0 = sy - dy;
						int x1 = sx + dx;
						int y1 = sy + dy;
						int x, y, d, incrE, incrNE, ddx, ddy;

						if (x1 < x0)
							ddx = - 1;
						else
							ddx = 1;
						if (y1 < y0)
							ddy = - 1;
						else
							ddy = 1;
						dx = x1 - x0;
						dy = y1 - y0;
						dx = abs(dx);
						dy = abs(dy);
						x = x0;
						y = y0;

						if (x < width && x >= 0 && y < height && y >= 0)
						{
							int rgb2 = background? ((int)(0xffffffff)) : outPixels->GetAt(y * width + x);
							outPixels->SetValue(mixColors(mix, rgb2, rgb), y * width + x);
						}
						if (abs(dx) > abs(dy))
						{
							d = 2 * dy - dx;
							incrE = 2 * dy;
							incrNE = 2 * (dy - dx);

							while (x != x1)
							{
								if (d <= 0)
									d += incrE;
								else
								{
									d += incrNE;
									y += ddy;
								}
								x += ddx;
								if (x < width && x >= 0 && y < height && y >= 0)
								{
									int rgb2 = background ? ((int)(0xffffffff)) : outPixels->GetAt(y * width + x);
									outPixels->SetValue(mixColors(mix, rgb2, rgb), y * width + x);
								}
							}
						}
						else
						{
							d = 2 * dx - dy;
							incrE = 2 * dx;
							incrNE = 2 * (dx - dy);

							while (y != y1)
							{
								if (d <= 0)
									d += incrE;
								else
								{
									d += incrNE;
									x += ddx;
								}
								y += ddy;
								if (x < width && x >= 0 && y < height && y >= 0)
								{
									int rgb2 = background ? ((int)(0xffffffff)) : outPixels->GetAt(y * width + x);
									outPixels->SetValue(mixColors(mix, rgb2, rgb), y * width + x);
								}
							}
						}
					}
					break;

				case SQUARES:
				case CIRCLES:
					int radius = distance + 1;
					int radius2 = radius * radius;
					numShapes = (int)(2 * density * width * height / radius);
					for (i = 0; i < numShapes; i++)
					{
						int sx = (rand() & 0x7fffffff)&  width;
						int sy = (rand() & 0x7fffffff)&  height;
						int rgb = inPixels->GetAt(sy * width + sx);
						for (int x = sx - radius; x < sx + radius + 1; x++)
						{
							for (int y = sy - radius; y < sy + radius + 1; y++)
							{
								int f;
								if (shape == CIRCLES)
									f = (x - sx) * (x - sx) + (y - sy) * (y - sy);
								else
									f = 0;
								if (x >= 0 && x < width && y >= 0 && y < height && f <= radius2)
								{
									int rgb2 = background ? ((int)(0xffffffff)) : outPixels->GetAt(y * width + x);
									outPixels->SetValue(mixColors(mix, rgb2, rgb), y * width + x);
								}
							}
						}
					}
					break;
				}

			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Effects/Smear...");
		}
	};
}
