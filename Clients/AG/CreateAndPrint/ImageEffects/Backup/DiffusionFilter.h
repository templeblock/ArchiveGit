#pragma once
#include "WholeImageFilter.h"

namespace ImageEffects
{
	/// A filter which uses Floyd-Steinberg error diffusion dithering to halftone an image.
	class DiffusionFilter : WholeImageFilter
	{
		/// Return the serpentine setting
		/// <returns> the current setting
		/// </returns>
		/// Set whether to use a serpentine pattern for return or not. This can reduce 'avalanche' artifacts in the output.
		/// <param name="serpentine">true to use serpentine pattern</param>
#ifdef NOTUSED
	public:
		virtual property bool Serpentine
		{
			bool get()
			{
				return serpentine;
			}

			void set(bool value)
			{
				serpentine = value;
			}

		}
		virtual property bool ColorDither
		{
			bool get()
			{
				return colorDither;
			}

			void set(bool value)
			{
				colorDither = value;
			}

		}
		virtual property Array<int> *Matrix
		{
			Array<int> *get()
			{
				return matrix;
			}

			void set(Array<int> *value)
			{
				matrix = value;
				sum = 0;
				for (int i = 0; i < value->Length; i++)
					sum += value[i];
			}

		}
		virtual property int Levels
		{
			int get()
			{
				return levels;
			}

			void set(int value)
			{
				levels = value;
			}

		}
#endif NOTUSED

	private:


	private:
		int* matrix;
		int sum;
		bool serpentine;
		bool colorDither;
	public:
		int levels;

		/// Construct a DiffusionFilter
		DiffusionFilter()
		{
			sum = 3 + 5 + 7 + 1;
			serpentine = true;
			colorDither = true;
			levels = 6;

			static const int diffusionMatrix[] = {0, 0, 0, 0, 0, 7, 3, 5, 1};
			matrix = (int*)diffusionMatrix;
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			Array<int> *outPixels = new Array<int>(width * height);

			int index = 0;
			int map[256];
			for (int i = 0; i < levels; i++)
			{
				int v = 255 * i / (levels - 1);
				map[i] = v;
			}
			int div[256];
			for (int i = 0; i < 256; i++)
				div[i] = levels * i / 256;

			for (int y = 0; y < height; y++)
			{
				bool reverse = serpentine && (y & 1) == 1;
				int direction;
				if (reverse)
				{
					index = y * width + width - 1;
					direction = - 1;
				}
				else
				{
					index = y * width;
					direction = 1;
				}
				for (int x = 0; x < width; x++)
				{
					int rgb1 = inPixels->GetAt(index);

					int r1 = (rgb1 >> 16) & 0xff;
					int g1 = (rgb1 >> 8) & 0xff;
					int b1 = rgb1 & 0xff;

					if (!colorDither)
						r1 = g1 = b1 = (r1 + g1 + b1) / 3;

					int r2 = map[div[r1]];
					int g2 = map[div[g1]];
					int b2 = map[div[b1]];

					outPixels->SetValue(((int)(0xff000000)) | (r2 << 16) | (g2 << 8) | b2, index);

					int er = r1 - r2;
					int eg = g1 - g2;
					int eb = b1 - b2;

					for (int i = - 1; i <= 1; i++)
					{
						int iy = i + y;
						if (0 <= iy && iy < height)
						{
							for (int j = - 1; j <= 1; j++)
							{
								int jx = j + x;
								if (0 <= jx && jx < width)
								{
									int w;
									if (reverse)
										w = matrix[(i + 1) * 3 - j + 1];
									else
										w = matrix[(i + 1) * 3 + j + 1];
									if (w != 0)
									{
										int k = reverse?index - j:index + j;
										rgb1 = inPixels->GetAt(k);
										r1 = (rgb1 >> 16) & 0xff;
										g1 = (rgb1 >> 8) & 0xff;
										b1 = rgb1 & 0xff;
										r1 += er * w / sum;
										g1 += eg * w / sum;
										b1 += eb * w / sum;
										inPixels->SetValue((PixelUtils::clamp(r1) << 16) | (PixelUtils::clamp(g1) << 8) | PixelUtils::clamp(b1), k);
									}
								}
							}
						}
					}
					index += direction;
				}
			}

			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Colors/Diffusion Dither...");
		}
	};
}
