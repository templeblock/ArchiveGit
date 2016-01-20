#pragma once

namespace ImageEffects
{
	class LifeFilter : BinaryFilter
	{

	public:
		LifeFilter()
		{
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
					int a = pixel & ((int)(0xff000000));
					int neighbours = 0;

					for (int row = - 1; row <= 1; row++)
					{
						int iy = y + row;
						int ioffset;
						if (0 <= iy && iy < height)
						{
							ioffset = iy * width;
							for (int col = - 1; col <= 1; col++)
							{
								int ix = x + col;
								if (!(row == 0 && col == 0) && 0 <= ix && ix < width)
								{
									int rgb = inPixels->GetAt(ioffset + ix);
//j									if (blackFunction::isBlack(rgb))
									if (0)
										neighbours++;
								}
							}
						}
					}

//j					if (blackFunction::isBlack(pixel))
					if (0)
						outPixels->SetValue((neighbours == 2 || neighbours == 3) ? pixel : ((int)(0xffffffff)), index++);
					else
						outPixels->SetValue((neighbours == 3 ? ((int)(0xffffffff)) : pixel), index++);
				}
			}
			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Binary/Life");
		}
	};
}