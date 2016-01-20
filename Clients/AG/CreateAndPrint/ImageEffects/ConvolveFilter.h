#pragma once

namespace ImageEffects
{
	/// A filter which applies a convolution kernel to an image.
	class ConvolveFilter
	{
#ifdef NOTUSED
	public:
		virtual property Kernel *Kernel
		{
			Kernel *get()
			{
				return kernel;
			}

			void set(Kernel *value)
			{
				kernel = value;
			}

		}
		virtual property int EdgeAction
		{
			int get()
			{
				return edgeAction;
			}

			void set(int value)
			{
				edgeAction = value;
			}

		}
#endif NOTUSED

	public:
		static const int ZERO_EDGES = 0;
		static const int CLAMP_EDGES = 1;
		static const int WRAP_EDGES = 2;

	public:
		Kernel* kernel;
		bool alpha;
	private:
		int edgeAction;

	/// Construct a filter with a null kernel. This is only useful if you're going to change the kernel later on.
	public:
		ConvolveFilter()
		{
			kernal = new float(9);
			alpha = true;
			edgeAction = CLAMP_EDGES;
		}

		/// Construct a filter with the given 3x3 kernel.
		/// <param name="matrix">an Array of 9 floats containing the kernel</param>
		ConvolveFilter(Array<float> *matrix)
		{
			kernal = new Kernel(3, 3, matrix);
			alpha = true;
			edgeAction = CLAMP_EDGES;
		}

		/// Construct a filter with the given kernel.
		/// <param name="rows	the">number of rows in the kernel</param>
		/// <param name="cols	the">number of columns in the kernel</param>
		/// <param name="matrix	an">Array of rows*cols floats containing the kernel</param>
		ConvolveFilter(int rows, int cols, Array<float> *matrix)
		{
			kernal = new Kernel(rows, cols, matrix);
			alpha = true;
			edgeAction = CLAMP_EDGES;
		}

		/// Construct a filter with the given 3x3 kernel.
		/// <param name="matrix">an Array of 9 floats containing the kernel</param>
		ConvolveFilter(Kernel *newkernel)
		{
			kernel = newkernel;
			alpha = true;
			edgeAction = CLAMP_EDGES;
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

			convolve(kernel, inPixels, outPixels, width, height, alpha, edgeAction);

//j			setRGB(dst, 0, 0, width, height, outPixels);
			return dst;
		}

		virtual BITMAPINFOHEADER *createCompatibleDestImage(BITMAPINFOHEADER *src, COLORREF& dstCM)
		{
			if (dstCM.IsEmpty)
			{
//j				dstCM = src.getColorModel();
			}

			return new BufferedImage(dstCM, src->biWidth, src->biHeight);
		}

		virtual CRectF getBounds2D(BITMAPINFOHEADER *src)
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

		static void convolve(Kernel *kernel, Array<int> *inPixels, Array<int> *outPixels, int width, int height, int edgeAction)
		{
			convolve(kernel, inPixels, outPixels, width, height, true, edgeAction);
		}

		static void convolve(Kernel *kernel, Array<int> *inPixels, Array<int> *outPixels, int width, int height, bool alpha, int edgeAction)
		{
			if (kernel->getHeight() == 1)
				convolveH(kernel, inPixels, outPixels, width, height, alpha, edgeAction);
			else
			{
				if (kernel->getWidth() == 1)
					convolveV(kernel, inPixels, outPixels, width, height, alpha, edgeAction);
				else
					convolveHV(kernel, inPixels, outPixels, width, height, alpha, edgeAction);
			}
		}

		/// Convolve with a 2D kernel
		static void convolveHV(Kernel *kernel, Array<int> *inPixels, Array<int> *outPixels, int width, int height, bool alpha, int edgeAction)
		{
			int index = 0;
			Array<float> *matrix = kernel->getKernelData(NULL);
			int rows = kernel->getHeight();
			int cols = kernel->getWidth();
			int rows2 = rows / 2;
			int cols2 = cols / 2;

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					float r = 0, g = 0, b = 0, a = 0;

					for (int row = - rows2; row <= rows2; row++)
					{
						int iy = y + row;
						int ioffset;
						if (0 <= iy && iy < height)
							ioffset = iy * width;
						else if (edgeAction == CLAMP_EDGES)
							ioffset = y * width;
						else if (edgeAction == WRAP_EDGES)
							ioffset = ((iy + height)&  height) * width;
						else
							continue;
						int moffset = cols * (row + rows2) + cols2;
						for (int col = - cols2; col <= cols2; col++)
						{
							float f = matrix[moffset + col];

							if (f != 0)
							{
								int ix = x + col;
								if (!(0 <= ix && ix < width))
								{
									if (edgeAction == CLAMP_EDGES)
										ix = x;
									else if (edgeAction == WRAP_EDGES)
										ix = (x + width)&  width;
									else
										continue;
								}
								int rgb = inPixels[ioffset + ix];
								a += f * ((rgb >> 24) & 0xff);
								r += f * ((rgb >> 16) & 0xff);
								g += f * ((rgb >> 8) & 0xff);
								b += f * (rgb & 0xff);
							}
						}
					}

					int ia = alpha?PixelUtils::clamp((int)(a + 0.5)):0xff;
					int ir = PixelUtils::clamp((int)(r + 0.5));
					int ig = PixelUtils::clamp((int)(g + 0.5));
					int ib = PixelUtils::clamp((int)(b + 0.5));
					outPixels[index++] = (ia << 24) | (ir << 16) | (ig << 8) | ib;
				}
			}
		}

		/// Convolve with a kernel consisting of one row
		static void convolveH(Kernel *kernel, Array<int> *inPixels, Array<int> *outPixels, int width, int height, bool alpha, int edgeAction)
		{
			int index = 0;
			Array<float> *matrix = kernel->getKernelData(NULL);
			int cols = kernel->getWidth();
			int cols2 = cols / 2;

			for (int y = 0; y < height; y++)
			{
				int ioffset = y * width;
				for (int x = 0; x < width; x++)
				{
					float r = 0, g = 0, b = 0, a = 0;
					int moffset = cols2;
					for (int col = - cols2; col <= cols2; col++)
					{
						float f = matrix[moffset + col];

						if (f != 0)
						{
							int ix = x + col;
							if (ix < 0)
							{
								if (edgeAction == CLAMP_EDGES)
									ix = 0;
								else if (edgeAction == WRAP_EDGES)
									ix = (x + width)&  width;
							}
							else if (ix >= width)
							{
								if (edgeAction == CLAMP_EDGES)
									ix = width - 1;
								else if (edgeAction == WRAP_EDGES)
									ix = (x + width)&  width;
							}
							int rgb = inPixels[ioffset + ix];
							a += f * ((rgb >> 24) & 0xff);
							r += f * ((rgb >> 16) & 0xff);
							g += f * ((rgb >> 8) & 0xff);
							b += f * (rgb & 0xff);
						}
					}

					int ia = alpha?PixelUtils::clamp((int)(a + 0.5)):0xff;
					int ir = PixelUtils::clamp((int)(r + 0.5));
					int ig = PixelUtils::clamp((int)(g + 0.5));
					int ib = PixelUtils::clamp((int)(b + 0.5));
					outPixels[index++] = (ia << 24) | (ir << 16) | (ig << 8) | ib;
				}
			}
		}

		/// Convolve with a kernel consisting of one column
		static void convolveV(Kernel *kernel, Array<int> *inPixels, Array<int> *outPixels, int width, int height, bool alpha, int edgeAction)
		{
			int index = 0;
			Array<float> *matrix = kernel->getKernelData(NULL);
			int rows = kernel->getHeight();
			int rows2 = rows / 2;

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					float r = 0, g = 0, b = 0, a = 0;

					for (int row = - rows2; row <= rows2; row++)
					{
						int iy = y + row;
						int ioffset;
						if (iy < 0)
						{
							if (edgeAction == CLAMP_EDGES)
								ioffset = 0;
							else if (edgeAction == WRAP_EDGES)
								ioffset = ((y + height)&  height) * width;
							else
								ioffset = iy * width;
						}
						else if (iy >= height)
						{
							if (edgeAction == CLAMP_EDGES)
								ioffset = (height - 1) * width;
							else if (edgeAction == WRAP_EDGES)
								ioffset = ((y + height)&  height) * width;
							else
								ioffset = iy * width;
						}
						else
							ioffset = iy * width;

						float f = matrix[row + rows2];

						if (f != 0)
						{
							int rgb = inPixels[ioffset + x];
							a += f * ((rgb >> 24) & 0xff);
							r += f * ((rgb >> 16) & 0xff);
							g += f * ((rgb >> 8) & 0xff);
							b += f * (rgb & 0xff);
						}
					}

					int ia = alpha?PixelUtils::clamp((int)(a + 0.5)):0xff;
					int ir = PixelUtils::clamp((int)(r + 0.5));
					int ig = PixelUtils::clamp((int)(g + 0.5));
					int ib = PixelUtils::clamp((int)(b + 0.5));
					outPixels[index++] = (ia << 24) | (ir << 16) | (ig << 8) | ib;
				}
			}
		}

		virtual CString *ToString()
		{
			return new CString("Blur/Convolve...");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}
