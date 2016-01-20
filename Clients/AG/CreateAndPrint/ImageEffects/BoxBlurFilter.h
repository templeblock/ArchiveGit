#pragma once

namespace ImageEffects
{

	class BoxBlurFilter
	{
#ifdef NOTUSED
	public:
		virtual property int HRadius
		{
			int get()
			{
				return hRadius;
			}

			void set(int value)
			{
				hRadius = value;
			}

		}
		virtual property int VRadius
		{
			int get()
			{
				return vRadius;
			}

			void set(int value)
			{
				vRadius = value;
			}

		}
		virtual property int Radius
		{
			int get()
			{
				return hRadius;
			}

			void set(int value)
			{
				hRadius = vRadius = value;
			}

		}
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
#endif NOTUSED

	private:
		int hRadius;
		int vRadius;
		int iterations;

	public:
		BoxBlurFilter()
		{
			iterations = 1;
		}

		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int width = src->biWidth;
			int height = src->biHeight;

			if (dst == NULL)
			{
				COLORREF tempAux = CLR_NONE;
//j				dst = createCompatibleDestImage(src, tempAux);
			}

			Array<int> *inPixels = new Array<int>(width * height);
			Array<int> *outPixels = new Array<int>(width * height);
//j			getRGB(src, 0, 0, width, height, inPixels);

			for (int i = 0; i < iterations; i++)
			{
				blur(inPixels, outPixels, width, height, hRadius);
				blur(outPixels, inPixels, height, width, vRadius);
			}

//j			setRGB(dst, 0, 0, width, height, inPixels);
			return dst;
		}

		static void blur(Array<int> *in_Renamed, Array<int> *out_Renamed, int width, int height, int radius)
		{
			int widthMinus1 = width - 1;
			int tableSize = 2 * radius + 1;
			Array<int> *divide = new Array<int>(256 * tableSize);

			for (int i = 0; i < 256 * tableSize; i++)
				divide->SetValue(i / tableSize, i);

			int inIndex = 0;

			for (int y = 0; y < height; y++)
			{
				int outIndex = y;
				int ta = 0, tr = 0, tg = 0, tb = 0;

				for (int i = - radius; i <= radius; i++)
				{
					int rgb = in_Renamed->GetAt(inIndex + clamp(i, 0, width - 1));
					ta += ((rgb >> 24) & 0xff);
					tr += ((rgb >> 16) & 0xff);
					tg += ((rgb >> 8) & 0xff);
					tb += (rgb & 0xff);
				}

				for (int x = 0; x < width; x++)
				{
					out_Renamed->SetValue((divide->GetAt(ta) << 24) | (divide->GetAt(tr) << 16) | (divide->GetAt(tg) << 8) | divide->GetAt(tb), outIndex);

					int i1 = x + radius + 1;
					if (i1 > widthMinus1)
						i1 = widthMinus1;
					int i2 = x - radius;
					if (i2 < 0)
						i2 = 0;
					int rgb1 = in_Renamed->GetAt(inIndex + i1);
					int rgb2 = in_Renamed->GetAt(inIndex + i2);

					ta += ((rgb1 >> 24) & 0xff) - ((rgb2 >> 24) & 0xff);
					tr += (((rgb1 & 0xff0000) - (rgb2 & 0xff0000)) >> 16);
					tg += (((rgb1 & 0xff00) - (rgb2 & 0xff00)) >> 8);
					tb += (rgb1 & 0xff) - (rgb2 & 0xff);
					outIndex += height;
				}
				inIndex += width;
			}
		}

		virtual CString *ToString()
		{
			return new CString("Blur/Box Blur...");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}