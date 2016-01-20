#pragma once
#include "WholeImageFilter.h"
#include "LinearColormap.h"

namespace ImageEffects
{
	class QuiltFilter : WholeImageFilter
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
		virtual property float A
		{
			float get()
			{
				return a;
			}

			void set(float value)
			{
				a = value;
			}

		}
		virtual property float B
		{
			float get()
			{
				return b;
			}

			void set(float value)
			{
				b = value;
			}

		}
		virtual property float C
		{
			float get()
			{
				return c;
			}

			void set(float value)
			{
				c = value;
			}

		}
		virtual property float D
		{
			float get()
			{
				return d;
			}

			void set(float value)
			{
				d = value;
			}

		}
		virtual property int K
		{
			int get()
			{
				return k;
			}

			void set(int value)
			{
				k = value;
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
#endif NOTUSED

	private:
		int iterations;
		float a;
		float b;
		float c;
		float d;
		int k;
		Colormap *colormap;

	public:
		QuiltFilter()
		{
			iterations = 25000;
			a = - 0.59f;
			b = 0.2f;
			c = 0.1f;
			colormap = (Colormap*)new LinearColormap();
		}

		virtual void randomize()
		{
//j			srand(System::DateTime::Now.Ticks);
			a = (float)(rand/*NextDouble*/());
			b = (float)(rand/*NextDouble*/());
			c = (float)(rand/*NextDouble*/());
			d = (float)(rand/*NextDouble*/());
			k = (rand() &  20) - 10;
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			Array<int> *outPixels = new Array<int>(width * height);

			int max = 0;

			float x = 0.1f;
			float y = 0.3f;

			for (int n = 0; n < 20; n++)
			{
				float mx = PI * x;
				float my = PI * y;
				float smx2 = (float)(sin(2 * mx));
				float smy2 = (float)(sin(2 * my));
				float x1 = (float)(a * smx2 + b * smx2 * cos(2 * my) + c * sin(4 * mx) + d * sin(6 * mx) * cos(4 * my) + k * x);
				x1 = x1 >= 0?x1 - (int)(x1):x1 - (int)(x1) + 1;

				float y1 = (float)(a * smy2 + b * smy2 * cos(2 * mx) + c * sin(4 * my) + d * sin(6 * my) * cos(4 * mx) + k * y);
				y1 = y1 >= 0?y1 - (int)(y1):y1 - (int)(y1) + 1;
				x = x1;
				y = y1;
			}

			for (int n = 0; n < iterations; n++)
			{
				float mx = PI * x;
				float my = PI * y;
				float x1 = (float)(a * sin(2 * mx) + b * sin(2 * mx) * cos(2 * my) + c * sin(4 * mx) + d * sin(6 * mx) * cos(4 * my) + k * x);
				x1 = x1 >= 0?x1 - (int)(x1):x1 - (int)(x1) + 1;

				float y1 = (float)(a * sin(2 * my) + b * sin(2 * my) * cos(2 * mx) + c * sin(4 * my) + d * sin(6 * my) * cos(4 * mx) + k * y);
				y1 = y1 >= 0?y1 - (int)(y1):y1 - (int)(y1) + 1;
				x = x1;
				y = y1;
				int ix = (int)(width * x);
				int iy = (int)(height * y);
				if (ix >= 0 && ix < width && iy >= 0 && iy < height)
				{
					int t = outPixels->GetAt(width * iy + ix);
					outPixels->SetValue(t+1, width * iy + ix);
					if (t > max)
						max = t;
				}
			}

			if (colormap != NULL)
			{
				int index = 0;
				for (y = 0; y < height; y++)
				{
					for (x = 0; x < width; x++)
					{
						outPixels->SetValue(colormap->getColor(outPixels->GetAt(index) / (float)(max)), index);
						index++;
					}
				}
			}
			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Texture/Chaotic Quilt...");
		}
	};
}
