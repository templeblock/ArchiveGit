#pragma once

namespace ImageEffects
{
	class FlipFilter
	{
#ifdef NOTUSED
	public:
		virtual property int Operation
		{
			int get()
			{
				return operation;
			}

			void set(int value)
			{
				operation = value;
			}

		}
#endif NOTUSED

		static const int FLIP_H = 1;
		static const int FLIP_V = 2;
		static const int FLIP_HV = 3;
		static const int FLIP_90CW = 4;
		static const int FLIP_90CCW = 5;
		static const int FLIP_180 = 6;

	private:
		int operation;
		int width, height;
		int newWidth, newHeight;

	public:
		FlipFilter()
		{
			operation = FLIP_HV;
		}

		FlipFilter(int newoperation)
		{
			operation = newoperation;
		}

		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int width = src->biWidth;
			int height = src->biHeight;
//j			int type = (int)(src->PixelFormat);
			BITMAPINFOHEADER *srcRaster = src;

			Array<int> *inPixels = NULL; //jgetRGB(src, 0, 0, width, height, NULL);

			int x = 0, y = 0;
			int w = width;
			int h = height;

			int newX = 0;
			int newY = 0;
			int newW = w;
			int newH = h;
			switch (operation)
			{

				case FLIP_H:
					newX = width - (x + w);
					break;

				case FLIP_V:
					newY = height - (y + h);
					break;

				case FLIP_HV:
					newW = h;
					newH = w;
					newX = y;
					newY = x;
					break;

				case FLIP_90CW:
					newW = h;
					newH = w;
					newX = height - (y + h);
					newY = x;
					break;

				case FLIP_90CCW:
					newW = h;
					newH = w;
					newX = y;
					newY = width - (x + w);
					break;

				case FLIP_180:
					newX = width - (x + w);
					newY = height - (y + h);
					break;
				}

			Array<int> *newPixels = new Array<int>(newW * newH);

			for (int row = 0; row < h; row++)
			{
				for (int col = 0; col < w; col++)
				{
					int index = row * width + col;
					int newRow = row;
					int newCol = col;
					switch (operation)
					{

						case FLIP_H:
							newCol = w - col - 1;
							break;

						case FLIP_V:
							newRow = h - row - 1;
							break;

						case FLIP_HV:
							newRow = col;
							newCol = row;
							break;

						case FLIP_90CW:
							newRow = col;
							newCol = h - row - 1;
							;
							break;

						case FLIP_90CCW:
							newRow = w - col - 1;
							newCol = row;
							break;

						case FLIP_180:
							newRow = h - row - 1;
							newCol = w - col - 1;
							break;
						}
					int newIndex = newRow * newW + newCol;
					newPixels->SetValue(inPixels->GetAt(index), newIndex);
				}
			}

			if (dst == NULL)
			{
//j				COLORREF dstCM;
//j				dstCM = src.getColorModel();
//j				dst = new BufferedImage(dstCM, newW, newH);
			}

			BITMAPINFOHEADER *dstRaster = dst;
//j			setRGB(dst, 0, 0, newW, newH, newPixels);

			return dst;
		}

		virtual CString *ToString()
		{
			switch (operation)
			{

				case FLIP_H:
					return new CString("Flip Horizontal");

				case FLIP_V:
					return new CString("Flip Vertical");

				case FLIP_HV:
					return new CString("Flip Diagonal");

				case FLIP_90CW:
					return new CString("Rotate 90");

				case FLIP_90CCW:
					return new CString("Rotate -90");

				case FLIP_180:
					return new CString("Rotate 180");
				}
			return new CString("Flip");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}