#pragma once

namespace ImageEffects
{
	/// This filter tries to apply the Swing "flush 3D" effect to the black lines in an image.

	class Flush3DFilter : WholeImageFilter
	{

	public:
		Flush3DFilter()
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

					if (pixel != ((int)(0xff000000)) && y > 0 && x > 0)
					{
						int count = 0;
						if (inPixels->GetAt(y * width + x - 1) == ((int)(0xff000000)))
							count++;
						if (inPixels->GetAt((y - 1) * width + x) == ((int)(0xff000000)))
							count++;
						if (inPixels->GetAt((y - 1) * width + x - 1) == ((int)(0xff000000)))
							count++;
						if (count >= 2)
							pixel = ((int)(0xffffffff));
					}
					outPixels->SetValue(pixel, index++);
				}
			}
			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Stylize/Flush 3D...");
		}
	};
}
