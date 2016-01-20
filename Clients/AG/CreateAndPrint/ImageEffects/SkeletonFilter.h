#pragma once

namespace ImageEffects
{
	// Based on an algorithm by Zhang and Suen (CACM, March 1984, 236-239).
	class SkeletonFilter : BinaryFilter
	{
	public:
		SkeletonFilter()
		{
			newColor = ((int)(0xffffffff));
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			static const BYTE skeletonTable[] = {
				0, 0, 0, 1, 0, 0, 1, 3, 0, 0, 3, 1, 1, 0, 1, 3,
				0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 3, 0, 3, 3,
				0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
				2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 3, 0, 2, 2,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0,
				3, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 2, 0,
				0, 1, 3, 1, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 1,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
				3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				2, 3, 1, 3, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 1,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				2, 3, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
				3, 3, 0, 1, 0, 0, 0, 0, 2, 2, 0, 0, 2, 0, 0, 0 };
			Array<int> *outPixels = new Array<int>(width * height);

			int count = 0;
			int black = ((int)(0xff000000));

			for (int i = 0; i < iterations; i++)
			{
				count = 0;
				for (int pass = 0; pass < 2; pass++)
				{
					for (int y = 1; y < height - 1; y++)
					{
						int offset = y * width + 1;
						for (int x = 1; x < width - 1; x++)
						{
							int pixel = inPixels->GetAt(offset);
							if (pixel == black)
							{
								int tableIndex = 0;

								if (inPixels->GetAt(offset - width - 1) == black)
									tableIndex |= 1;
								if (inPixels->GetAt(offset - width) == black)
									tableIndex |= 2;
								if (inPixels->GetAt(offset - width + 1) == black)
									tableIndex |= 4;
								if (inPixels->GetAt(offset + 1) == black)
									tableIndex |= 8;
								if (inPixels->GetAt(offset + width + 1) == black)
									tableIndex |= 16;
								if (inPixels->GetAt(offset + width) == black)
									tableIndex |= 32;
								if (inPixels->GetAt(offset + width - 1) == black)
									tableIndex |= 64;
								if (inPixels->GetAt(offset - 1) == black)
									tableIndex |= 128;
								int code = skeletonTable[tableIndex];
								if (pass == 1)
								{
									if (code == 2 || code == 3)
									{
										if (colormap != NULL)
										{
											pixel = colormap->getColor((float)(i) / iterations);
										}
										else
											pixel = newColor;
										count++;
									}
								}
								else
								{
									if (code == 1 || code == 3)
									{
										if (colormap != NULL)
										{
											pixel = colormap->getColor((float)(i) / iterations);
										}
										else
											pixel = newColor;
										count++;
									}
								}
							}
							outPixels->SetValue(pixel, offset++);
						}
					}
					if (pass == 0)
					{
						inPixels = outPixels;
						outPixels = new Array<int>(width * height);
					}
				}
				if (count == 0)
					break;
			}
			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Binary/Skeletonize...");
		}
	};
}
