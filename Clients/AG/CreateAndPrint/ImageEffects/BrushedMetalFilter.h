#pragma once

namespace ImageEffects
{
	class BrushedMetalFilter
	{
#ifdef NOTUSED
	public:
		virtual property int Radius
		{
			int get()
			{
				return radius;
			}

			void set(int value)
			{
				radius = value;
			}

		}
		virtual property float Amount
		{
			float get()
			{
				return amount;
			}

			void set(float value)
			{
				amount = value;
			}

		}
		virtual property int Color
		{
			int get()
			{
				return color;
			}

			void set(int value)
			{
				color = value;
			}

		}
		virtual property bool Monochrome
		{
			bool get()
			{
				return monochrome;
			}

			void set(bool value)
			{
				monochrome = value;
			}

		}
		virtual property float Shine
		{
			float get()
			{
				return shine;
			}

			void set(float value)
			{
				shine = value;
			}

		}
#endif NOTUSED

	private:
		int radius;
		float amount;
		int color;
		float shine;
		bool monochrome;

	public:
		BrushedMetalFilter()
		{
			amount = 0.1f;
			color = ((int)(0xff888888));
		}

		BrushedMetalFilter(int newcolor, int newradius, float newamount, bool newmonochrome, float newshine)
		{
			color = newcolor;
			radius = newradius;
			amount = newamount;
			monochrome = newmonochrome;
			shine = newshine;
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

			Array<int> *inPixels = new Array<int>(width);
			Array<int> *outPixels = new Array<int>(width);

			int a = color & ((int)(0xff000000));
			int r = (color >> 16) & 0xff;
			int g = (color >> 8) & 0xff;
			int b = color & 0xff;
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int tr = r;
					int tg = g;
					int tb = b;
					if (shine != 0)
					{
						int f = (int)(255 * shine * sin((double)x / width * PI));
						tr += f;
						tg += f;
						tb += f;
					}
					if (monochrome)
					{
						int n = (int)(255 * (2 * (float)(rand/*NextDouble*/() - 1)) * amount);
						inPixels->SetValue(a | (clamp(tr + n) << 16) | (clamp(tg + n) << 8) | clamp(tb + n), x);
					}
					else
					{
						inPixels->SetValue(a | (random(tr) << 16) | (random(tg) << 8) | random(tb), x);
					}
				}

				if (radius != 0)
				{
					blur(inPixels, outPixels, width, radius);
					setRGB(dst, 0, y, width, 1, outPixels);
				}
				else
					setRGB(dst, 0, y, width, 1, inPixels);
			}
			return dst;
		}

	private:
		int random(int x)
		{
			x += (int)(255 * (2 * (float)(rand/*NextDouble*/() - 1)) * amount);
			if (x < 0)
				x = 0;
			else if (x > 0xff)
				x = 0xff;
			return x;
		}

		static int clamp(int c)
		{
			if (c < 0)
				return 0;
			if (c > 255)
				return 255;
			return c;
		}

		/// Return a mod b. This differs from the&  operator with respect to negative numbers.
		/// <param name="a">the dividend</param>
		/// <param name="b">the divisor</param>
		/// <returns> a mod b</returns>
		static int mod(int a, int b)
		{
			int n = a / b;

			a -= n * b;
			if (a < 0)
				return a + b;
			return a;
		}

	public:
		virtual void blur(Array<int> *in_Renamed, Array<int> *out_Renamed, int width, int radius)
		{
			int widthMinus1 = width - 1;
			int r2 = 2 * radius + 1;
			int tr = 0, tg = 0, tb = 0;

			for (int i = - radius; i <= radius; i++)
			{
				int rgb = in_Renamed->GetAt(mod(i, width));
				tr += ((rgb >> 16) & 0xff);
				tg += ((rgb >> 8) & 0xff);
				tb += (rgb & 0xff);
			}

			for (int x = 0; x < width; x++)
			{
				out_Renamed->SetValue(((int)(0xff000000)) | ((tr / r2) << 16) | ((tg / r2) << 8) | (tb / r2), x);

				int i1 = x + radius + 1;
				if (i1 > widthMinus1)
					i1 = mod(i1, width);
				int i2 = x - radius;
				if (i2 < 0)
					i2 = mod(i2, width);
				int rgb1 = in_Renamed->GetAt(i1);
				int rgb2 = in_Renamed->GetAt(i2);

				tr += (((rgb1 & 0xff0000) - (rgb2 & 0xff0000)) >> 16);
				tg += (((rgb1 & 0xff00) - (rgb2 & 0xff00)) >> 8);
				tb += (rgb1 & 0xff) - (rgb2 & 0xff);
			}
		}

		virtual BITMAPINFOHEADER *createCompatibleDestImage(BITMAPINFOHEADER *src, COLORREF& dstCM)
		{
//j			if (dstCM.IsEmpty)
			{
//j				dstCM = src.getColorModel();
			}
			return NULL; //jnew BufferedImage(dstCM, src->biWidth, src->biHeight);
		}

/*		virtual CRect getBounds2D(BITMAPINFOHEADER *src)
		{
			return CRect(0, 0, src->biWidth, src->biHeight);
		}

		virtual CPoint getPoint2D(CPointF& srcPt, CPointF& dstPt)
		{
			if (dstPt.IsEmpty)
				dstPt = CPointF();
			dstPt.x = (float)(srcPt.x);
			dstPt.y = (float)(srcPt.y);
			return dstPt;
		}

		virtual System::Collections::Hashtable *getRenderingHints()
		{
			return NULL;
		}
*/

		/// A convenience method for setting ARGB pixels in an image. This tries to avoid the performance
		/// penalty of BufferedImage.setRGB unmanaging the image.
		virtual void setRGB(BITMAPINFOHEADER *image, int x, int y, int width, int height, Array<int> *pixels)
		{
//j			int type = (int)(image->PixelFormat);
//j			image->setRGB(x, y, width, height, pixels, 0, width);
		}

		virtual CString *ToString()
		{
			return new CString("Texture/Brushed Metal...");
		}
	};
}
