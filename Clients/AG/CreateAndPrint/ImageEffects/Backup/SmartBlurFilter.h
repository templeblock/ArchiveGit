#pragma once

namespace ImageEffects
{
	class SmartBlurFilter
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
		virtual property int Threshold
		{
			int get()
			{
				return threshold;
			}

			void set(int value)
			{
				threshold = value;
			}

		}
#endif NOTUSED

	private:
		int hRadius;
		int vRadius;
		int threshold;

	public:
		SmartBlurFilter()
		{
			hRadius = 5;
			vRadius = 5;
			threshold = 10;
		}
		
		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int width = src->biWidth;
			int height = src->biHeight;

			if (dst == NULL)
			{
//j				COLORREF tempAux = CLR_NONE;
//j				dst = createCompatibleDestImage(src, tempAux);
			}

			Array<int> *inPixels = new Array<int>(width * height);
			Array<int> *outPixels = new Array<int>(width * height);
//j			getRGB(src, 0, 0, width, height, inPixels);

			Kernel *kernel = NULL; //j GaussianFilter::makeKernel(hRadius);
			thresholdBlur(kernel, inPixels, outPixels, width, height, true);
			thresholdBlur(kernel, outPixels, inPixels, height, width, true);

//j			setRGB(dst, 0, 0, width, height, inPixels);
			return dst;
		}

		/// Convolve with a kernel consisting of one row
		virtual void thresholdBlur(Kernel *kernel, Array<int> *inPixels, Array<int> *outPixels, int width, int height, bool alpha)
		{
			Array<float> *matrix = kernel->getKernelData(NULL);
			int cols = kernel->getWidth();
			int cols2 = cols / 2;

			for (int y = 0; y < height; y++)
			{
				int ioffset = y * width;
				int outIndex = y;
				for (int x = 0; x < width; x++)
				{
					float r = 0, g = 0, b = 0, a = 0;
					int moffset = cols2;

					int rgb1 = inPixels->GetAt(ioffset + x);
					int a1 = (rgb1 >> 24) & 0xff;
					int r1 = (rgb1 >> 16) & 0xff;
					int g1 = (rgb1 >> 8) & 0xff;
					int b1 = rgb1 & 0xff;
					float af = 0, rf = 0, gf = 0, bf = 0;
					for (int col = - cols2; col <= cols2; col++)
					{
						float f = matrix->GetAt(moffset + col);

						if (f != 0)
						{
							int ix = x + col;
							if (!(0 <= ix && ix < width))
								ix = x;
							int rgb2 = inPixels->GetAt(ioffset + ix);
							int a2 = (rgb2 >> 24) & 0xff;
							int r2 = (rgb2 >> 16) & 0xff;
							int g2 = (rgb2 >> 8) & 0xff;
							int b2 = rgb2 & 0xff;

							int d;
							d = a1 - a2;
							if (d >= - threshold && d <= threshold)
							{
								a += f * a2;
								af += f;
							}
							d = r1 - r2;
							if (d >= - threshold && d <= threshold)
							{
								r += f * r2;
								rf += f;
							}
							d = g1 - g2;
							if (d >= - threshold && d <= threshold)
							{
								g += f * g2;
								gf += f;
							}
							d = b1 - b2;
							if (d >= - threshold && d <= threshold)
							{
								b += f * b2;
								bf += f;
							}
						}
					}
					a = af == 0?a1:a / af;
					r = rf == 0?r1:r / rf;
					g = gf == 0?g1:g / gf;
					b = bf == 0?b1:b / bf;
					int ia = alpha?PixelUtils::clamp((int)(a + 0.5)):0xff;
					int ir = PixelUtils::clamp((int)(r + 0.5));
					int ig = PixelUtils::clamp((int)(g + 0.5));
					int ib = PixelUtils::clamp((int)(b + 0.5));
					outPixels->SetValue((ia << 24) | (ir << 16) | (ig << 8) | ib, outIndex);
					outIndex += height;
				}
			}
		}

		virtual CString *ToString()
		{
			return new CString("Blur/Smart Blur...");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}
