#pragma once

namespace ImageEffects
{
	/// Given a binary image, this filter performs binary erosion, setting all removed pixels to the given 'new' color.

	class ErodeFilter : BinaryFilter
	{
		/// Return the threshold - the number of neighbouring pixels for dilation to occur.
		/// <returns> the current threshold</returns>
		/// Set the threshold - the number of neighbouring pixels for dilation to occur.
		/// <param name="threshold">the new threshold</param>
#ifdef NOTUSED
	public:
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


	public:
		int threshold;

	public:
		ErodeFilter()
		{
			threshold = 2;
			newColor = ((int)(0xffffffff));
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels)
		{
			Array<int> *outPixels = new Array<int>(width * height);

			for (int i = 0; i < iterations; i++)
			{
				int index = 0;

				if (i > 0)
				{
					Array<int> *t = inPixels;
					inPixels = outPixels;
					outPixels = t;
				}
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						int pixel = inPixels->GetAt(y * width + x);
						if (blackFunction->isBlack(pixel))
						{
							int neighbours = 0;

							for (int dy = - 1; dy <= 1; dy++)
							{
								int iy = y + dy;
								int ioffset;
								if (0 <= iy && iy < height)
								{
									ioffset = iy * width;
									for (int dx = - 1; dx <= 1; dx++)
									{
										int ix = x + dx;
										if (!(dy == 0 && dx == 0) && 0 <= ix && ix < width)
										{
											int rgb = inPixels->GetAt(ioffset + ix);
											if (!blackFunction->isBlack(rgb))
												neighbours++;
										}
									}
								}
							}

							if (neighbours >= threshold)
							{
								if (colormap != NULL)
								{
									pixel = colormap->getColor((float)(i) / iterations);
								}
								else
									pixel = newColor;
							}
						}
						outPixels->SetValue(pixel, index++);
					}
				}
			}

			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Binary/Erode...");
		}
	};
}
