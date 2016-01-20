#pragma once

namespace ImageEffects
{
	/// Given a binary image, this filter converts it to its outline, replacing all interior pixels with the 'new' color.

	class OutlineFilter : BinaryFilter
	{
	private:
		int newColor;

	public:
		OutlineFilter()
		{
			newColor = ((int)(0xffffffff));
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels)
		{
			int index = 0;
			Array<int> *outPixels = new Array<int>(width * height);

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int pixel = inPixels->GetAt(y * width + x);
//j					if (blackFunction::isBlack(pixel))
					if (0)
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
//j										if (blackFunction::isBlack(rgb))
										if (0)
											neighbours++;
									}
									else
										neighbours++;
								}
							}
						}

						if (neighbours == 9)
							pixel = newColor;
					}
					outPixels->SetValue(pixel, index++);
				}
			}
			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Binary/Outline...");
		}
	};
}