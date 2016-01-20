#pragma once
#include "WholeImageFilter.h"
//j#include "OctTreeQuantizer.h"
#include "Quantizer.h"

namespace ImageEffects
{
	/// A filter which quantizes an image to a set number of colors - useful for producing
	/// images which are to be encoded using an index color model. The filter can perform
	/// Floyd-Steinberg error-diffusion dithering if required. At present, the quantization
	/// is done using an octtree algorithm but I eventually hope to add more quantization
	/// methods such as median cut. Note: at present, the filter produces an image which
	/// uses the RGB color model (because the application it was written for required it).
	/// I hope to extend it to produce an IndexColorModel by request.
	class QuantizeFilter : WholeImageFilter
	{
#ifdef NOTUSED
		/// Get the number of colors to quantize to.
		/// <returns> the number of colors.
		/// </returns>
		/// Set the number of colors to quantize to.
		/// <param name="numColors">the number of colors. The default is 256.
		/// </param>
	public:
		virtual property int NumColors
		{
			int get()
			{
				return numColors;
			}

			void set(int value)
			{
				numColors = min(max(value, 8), 256);
			}

		}

		/// Return the dithering setting
		/// <returns> the current setting
		/// </returns>
		/// Set whether to use dithering or not. If not, the image is posterized.
		/// <param name="dither">true to use dithering
		/// </param>
		virtual property bool Dither
		{
			bool get()
			{
				return dither;
			}

			void set(bool value)
			{
				dither = value;
			}

		}

		/// Return the serpentine setting
		/// <returns> the current setting
		/// </returns>
		/// Set whether to use a serpentine pattern for return or not. This can reduce 'avalanche' artifacts in the output.
		/// <param name="serpentine">true to use serpentine pattern
		/// </param>
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
#endif NOTUSED

	private:
		int sum;
		bool dither;
		int numColors;
		bool serpentine;

	public:
		QuantizeFilter()
		{
			sum = 3 + 5 + 7 + 1;
			numColors = 256;
			serpentine = true;
		}
		
		virtual void quantize(Array<int> *inPixels, Array<int> *outPixels, int width, int height, int numColors, bool dither, bool serpentine)
		{
			/// Floyd-Steinberg dithering matrix.
			static const int matrix[] = {0, 0, 0, 0, 0, 7, 3, 5, 1};

			int count = width * height;
			Quantizer *quantizer = NULL; //jnew OctTreeQuantizer();
			quantizer->setup(numColors);
			quantizer->addPixels(inPixels, 0, count);
			Array<int> *table = quantizer->buildColorTable();

			if (!dither)
			{
				for (int i = 0; i < count; i++)
					outPixels->SetValue(table->GetAt(quantizer->getIndexForColor(inPixels->GetAt(i))), i);
			}
			else
			{
				int index = 0;
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
						int rgb2 = table->GetAt(quantizer->getIndexForColor(rgb1));

						outPixels->SetValue(rgb2, index);

						int r1 = (rgb1 >> 16) & 0xff;
						int g1 = (rgb1 >> 8) & 0xff;
						int b1 = rgb1 & 0xff;

						int r2 = (rgb2 >> 16) & 0xff;
						int g2 = (rgb2 >> 8) & 0xff;
						int b2 = rgb2 & 0xff;

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
			}
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			Array<int> *outPixels = new Array<int>(width * height);

			quantize(inPixels, outPixels, width, height, numColors, dither, serpentine);

			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Colors/Quantize...");
		}
	};
}
