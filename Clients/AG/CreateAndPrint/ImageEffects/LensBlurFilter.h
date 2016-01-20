#pragma once

namespace ImageEffects
{
	class LensBlurFilter
	{
		/// Get the radius of the kernel.
		/// <returns> the radius
		/// </returns>
		/// Set the radius of the kernel, and hence the amount of blur.
		/// <param name="radius">the radius of the blur in pixels.
		/// </param>
#ifdef NOTUSED
	public:
		virtual property float Radius
		{
			float get()
			{
				return radius;
			}

			void set(float value)
			{
				radius = value;
			}

		}
		virtual property int Sides
		{
			int get()
			{
				return sides;
			}

			void set(int value)
			{
				sides = value;
			}

		}
		virtual property float Bloom
		{
			float get()
			{
				return bloom;
			}

			void set(float value)
			{
				bloom = value;
			}

		}
		virtual property float BloomThreshold
		{
			float get()
			{
				return bloomThreshold;
			}

			void set(float value)
			{
				bloomThreshold = value;
			}

		}
#endif NOTUSED

	private:
		float radius;
		float bloom;
		float bloomThreshold;
		float angle;
		int sides;

	public:
		LensBlurFilter()
		{
			radius = 10;
			bloom = 2;
			bloomThreshold = 192;
			angle;
			sides = 5;
		}
		
		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int width = src->biWidth;
			int height = src->biHeight;
			int rows = 1, cols = 1;
			int log2rows = 0, log2cols = 0;
			int iradius = (int)ceil(radius);
			int tileWidth = 128;
			int tileHeight = tileWidth;

			int adjustedWidth = (int)(width + iradius * 2);
			int adjustedHeight = (int)(height + iradius * 2);

			tileWidth = iradius < 32?min(128, width + 2 * iradius):min(256, width + 2 * iradius);
			tileHeight = iradius < 32?min(128, height + 2 * iradius):min(256, height + 2 * iradius);

//j			if (dst == NULL)
//j				dst = new BITMAPINFOHEADER(width, height, (System::Drawing::Imaging::PixelFormat)(System::Drawing::Imaging::PixelFormat::Format32bppArgb));

			while (rows < tileHeight)
			{
				rows *= 2;
				log2rows++;
			}
			while (cols < tileWidth)
			{
				cols *= 2;
				log2cols++;
			}
			int w = cols;
			int h = rows;

			tileWidth = w;
			tileHeight = h; //FIXME-tileWidth, w, and cols are always all the same

			FFT *fft = new FFT(max(log2rows, log2cols));

			Array<int> *rgb = new Array<int>(w * h);
			Array<Array<float>*> *mask = new Array<Array<float>*>(2);
			for (int i = 0; i < 2; i++)
			{
				mask[i] = new Array<float>(w * h);
			}
			Array<Array<float>*> *gb = new Array<Array<float>*>(2);
			for (int i2 = 0; i2 < 2; i2++)
			{
				gb[i2] = new Array<float>(w * h);
			}
			Array<Array<float>*> *ar = new Array<Array<float>*>(2);
			for (int i3 = 0; i3 < 2; i3++)
			{
				ar[i3] = new Array<float>(w * h);
			}

			// Create the kernel
			double polyAngle = PI / sides;
			double polyScale = 1.0f / cos(polyAngle);
			double r2 = radius * radius;
			double rangle = DegreesToRadians(angle);
			float total = 0;
			int i4 = 0;
			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					double dx = (double)x - w / 2.0f;
					double dy = (double)y - h / 2.0f;
					double r = dx * dx + dy * dy;
					double f = r < r2?1:0;
					if (f != 0)
					{
						r = sqrt(r);
						if (sides != 0)
						{
							double a = atan2(dy, dx) + rangle;
							a = mod(a, polyAngle * 2) - polyAngle;
							f = cos(a) * polyScale;
						}
						else
							f = 1;
						f = f * r < radius?1:0;
					}
					total += (float)(f);

					mask[0][i4] = (float)(f);
					mask[1][i4] = 0;
					i4++;
				}
			}

			// Normalize the kernel
			i4 = 0;
			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					mask[0][i4] /= total;
					i4++;
				}
			}

			fft->transform2D(mask[0], mask[1], w, h, true);

			for (int tileY = - iradius; tileY < height; tileY += tileHeight - 2 * iradius)
			{
				for (int tileX = - iradius; tileX < width; tileX += tileWidth - 2 * iradius)
				{
					// Clip the tile to the image bounds
					int tx = tileX, ty = tileY, tw = tileWidth, th = tileHeight;
					int fx = 0, fy = 0;
					if (tx < 0)
					{
						tw += tx;
						fx -= tx;
						tx = 0;
					}
					if (ty < 0)
					{
						th += ty;
						fy -= ty;
						ty = 0;
					}
					if (tx + tw > width)
						tw = width - tx;
					if (ty + th > height)
						th = height - ty;
					src->getRGB(tx, ty, tw, th, rgb, fy * w + fx, w);

					// Create a float Array from the pixels. Any pixels off the edge of the source image get duplicated from the edge.
					i4 = 0;
					for (int y = 0; y < h; y++)
					{
						int imageY = y + tileY;
						int j;
						if (imageY < 0)
							j = fy;
						else if (imageY > height)
							j = fy + th - 1;
						else
							j = y;
						j *= w;
						for (int x = 0; x < w; x++)
						{
							int imageX = x + tileX;
							int k;
							if (imageX < 0)
								k = fx;
							else if (imageX > width)
								k = fx + tw - 1;
							else
								k = x;
							k += j;

							ar[0][i4] = ((rgb[k] >> 24) & 0xff);
							float r = ((rgb[k] >> 16) & 0xff);
							float g = ((rgb[k] >> 8) & 0xff);
							float b = (rgb[k] & 0xff);

							// Bloom...
							if (r > bloomThreshold)
								r *= bloom;
							//r = bloomThreshold + (r-bloomThreshold) * bloom;
							if (g > bloomThreshold)
								g *= bloom;
							//g = bloomThreshold + (g-bloomThreshold) * bloom;
							if (b > bloomThreshold)
								b *= bloom;
							//b = bloomThreshold + (b-bloomThreshold) * bloom;

							ar[1][i4] = r;
							gb[0][i4] = g;
							gb[1][i4] = b;

							i4++;
							k++;
						}
					}

					// Transform into frequency space
					fft->transform2D(ar[0], ar[1], cols, rows, true);
					fft->transform2D(gb[0], gb[1], cols, rows, true);

					// Multiply the transformed pixels by the transformed kernel
					i4 = 0;
					for (int y = 0; y < h; y++)
					{
						for (int x = 0; x < w; x++)
						{
							float re = ar[0][i4];
							float im = ar[1][i4];
							float rem = mask[0][i4];
							float imm = mask[1][i4];
							ar[0][i4] = re * rem - im * imm;
							ar[1][i4] = re * imm + im * rem;

							re = gb[0][i4];
							im = gb[1][i4];
							gb[0][i4] = re * rem - im * imm;
							gb[1][i4] = re * imm + im * rem;
							i4++;
						}
					}

					// Transform back
					fft->transform2D(ar[0], ar[1], cols, rows, false);
					fft->transform2D(gb[0], gb[1], cols, rows, false);

					// Convert back to RGB pixels, with quadrant remapping
					int row_flip = w >> 1;
					int col_flip = h >> 1;
					int index = 0;

					//FIXME-don't bother converting pixels off image edges
					for (int y = 0; y < w; y++)
					{
						int ym = y * row_flip;
						int yi = ym * cols;
						for (int x = 0; x < w; x++)
						{
							int xm = yi + (x * col_flip);
							int a = (int)(ar[0][xm]);
							int r = (int)(ar[1][xm]);
							int g = (int)(gb[0][xm]);
							int b = (int)(gb[1][xm]);

							// Clamp high pixels due to blooming
							if (r > 255)
								r = 255;
							if (g > 255)
								g = 255;
							if (b > 255)
								b = 255;
							int argb = (a << 24) | (r << 16) | (g << 8) | b;
							rgb[index++] = argb;
						}
					}

					// Clip to the output image
					tx = tileX + iradius;
					ty = tileY + iradius;
					tw = tileWidth - 2 * iradius;
					th = tileHeight - 2 * iradius;
					if (tx + tw > width)
						tw = width - tx;
					if (ty + th > height)
						th = height - ty;
					dst->setRGB(tx, ty, tw, th, rgb, iradius * w + iradius, w);
				}
			}
			return dst;
		}

		virtual CString *ToString()
		{
			return new CString("Blur/Lens Blur...");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}