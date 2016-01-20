#pragma once

namespace ImageEffects
{
	class VariableBlurFilter
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
		virtual property BITMAPINFOHEADER *BlurMask
		{
			BITMAPINFOHEADER *get()
			{
				return blurMask;
			}

			void set(BITMAPINFOHEADER *value)
			{
				blurMask = value;
			}

		}
#endif NOTUSED

	private:
		int hRadius;
		int vRadius;
		int iterations;
		BITMAPINFOHEADER *blurMask;

	public:
		VariableBlurFilter()
		{
			hRadius = 1;
			vRadius = 1;
			iterations = 1;
		}
		
		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int width = src->biWidth;
			int height = src->biHeight;

//j			if (dst == NULL)
//j				dst = new BITMAPINFOHEADER(width, height, (System::Drawing::Imaging::PixelFormat)(System::Drawing::Imaging::PixelFormat::Format32bppArgb));

			Array<int> *inPixels = new Array<int>(width * height);
			Array<int> *outPixels = new Array<int>(width * height);
//j			getRGB(src, 0, 0, width, height, inPixels);

			for (int i = 0; i < iterations; i++)
			{
				blur(inPixels, outPixels, width, height, hRadius, 1);
				blur(outPixels, inPixels, height, width, vRadius, 2);
			}

//j			setRGB(dst, 0, 0, width, height, inPixels);
			return dst;
		}

		virtual BITMAPINFOHEADER *createCompatibleDestImage(BITMAPINFOHEADER *src, COLORREF& dstCM)
		{
//j			if (dstCM.IsEmpty)
			{
//j				dstCM = src.getColorModel();
			}

			return new BufferedImage(dstCM, src->biWidth, src->biHeight);
		}

		virtual CRect F getBounds2D(BITMAPINFOHEADER *src)
		{
			return CRect(0, 0, src->biWidth, src->biHeight);
		}

		virtual CPointF getPoint2D(CPointF& srcPt, CPointF& dstPt)
		{
			if (dstPt.IsEmpty)
				dstPt = CPointF();
			dstPt.X = (float)(srcPt.X);
			dstPt.Y = (float)(srcPt.Y);
			return dstPt;
		}

		virtual System::Collections::Hashtable *getRenderingHints()
		{
			return NULL;
		}

		virtual void blur(Array<int> *in_Renamed, Array<int> *out_Renamed, int width, int height, int radius, int pass)
		{
			int widthMinus1 = width - 1;
			Array<int> *r = new Array<int>(width);
			Array<int> *g = new Array<int>(width);
			Array<int> *b = new Array<int>(width);
			Array<int> *a = new Array<int>(width);
			Array<int> *mask = new Array<int>(width);

			int inIndex = 0;

			for (int y = 0; y < height; y++)
			{
				int outIndex = y;

				if (blurMask != NULL)
				{
					if (pass == 1)
						getRGB(blurMask, 0, y, width, 1, mask);
					else
						getRGB(blurMask, y, 0, 1, width, mask);
				}

				for (int x = 0; x < width; x++)
				{
					int argb = in_Renamed[inIndex + x];
					a[x] = (argb >> 24) & 0xff;
					r[x] = (argb >> 16) & 0xff;
					g[x] = (argb >> 8) & 0xff;
					b[x] = argb & 0xff;
					if (x != 0)
					{
						a[x] += a[x - 1];
						r[x] += r[x - 1];
						g[x] += g[x - 1];
						b[x] += b[x - 1];
					}
				}

				for (int x = 0; x < width; x++)
				{
					// Get the blur radius at x, y
					int ra;
					if (blurMask != NULL)
					{
						if (pass == 1)
						{
							ra = (int)((mask[x] & 0xff) * hRadius / 255f);
						}
						else
						{
							ra = (int)((mask[x] & 0xff) * vRadius / 255f);
						}
					}
					else
					{
						if (pass == 1)
						{
							ra = (int)(blurRadiusAt(x, y, width, height) * hRadius);
						}
						else
						{
							ra = (int)(blurRadiusAt(y, x, height, width) * vRadius);
						}
					}

					int divisor = 2 * ra + 1;
					int ta = 0, tr = 0, tg = 0, tb = 0;
					int i1 = x + ra;
					if (i1 > widthMinus1)
					{
						int f = i1 - widthMinus1;
						int l = widthMinus1;
						ta += (a[l] - a[l - 1]) * f;
						tr += (r[l] - r[l - 1]) * f;
						tg += (g[l] - g[l - 1]) * f;
						tb += (b[l] - b[l - 1]) * f;
						i1 = widthMinus1;
					}
					int i2 = x - ra - 1;
					if (i2 < 0)
					{
						ta -= a[0] * i2;
						tr -= r[0] * i2;
						tg -= g[0] * i2;
						tb -= b[0] * i2;
						i2 = 0;
					}

					ta += a[i1] - a[i2];
					tr += r[i1] - r[i2];
					tg += g[i1] - g[i2];
					tb += b[i1] - b[i2];
					out_Renamed[outIndex] = ((ta / divisor) << 24) | ((tr / divisor) << 16) | ((tg / divisor) << 8) | (tb / divisor);

					outIndex += height;
				}
				inIndex += width;
			}
		}

		// Override this to get a different blur radius
	public:
		virtual float blurRadiusAt(int x, int y, int width, int height)
		{
			return (float)(x) / width;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Blur/Variable Blur...");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}