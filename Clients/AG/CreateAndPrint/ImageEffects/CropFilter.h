#pragma once

namespace ImageEffects
{
	class CropFilter
	{
#ifdef NOTUSED
	public:
		virtual property int Width
		{
			int get()
			{
				return width;
			}

			void set(int value)
			{
				width = value;
			}

		}
		virtual property int Height
		{
			int get()
			{
				return height;
			}

			void set(int value)
			{
				height = value;
			}

		}
#endif NOTUSED

	private:


	private:
		int x;
		int y;
		int width;
		int height;

	public:
		CropFilter()
		{
			x = 0;
			y = 0;
			width = 32;
			height = 32;
		}

		CropFilter(int newx, int newy, int newwidth, int newheight)
		{
			x = newx;
			y = newy;
			width = newwidth;
			height = newheight;
		}

		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int w = src->biWidth;
			int h = src->biHeight;

			if (dst == NULL)
			{
//j				COLORREF dstCM;
//j				dstCM = src.getColorModel();
//j				dst = new BufferedImage(dstCM, width, height);
			}

//j			System::Drawing::Graphics *g = System::Drawing::Graphics::FromImage(dst);
//j			System::Drawing::Drawing2D::Matrix *temp_Matrix;
//j			temp_Matrix = new System::Drawing::Drawing2D::Matrix();
//j			temp_Matrix->Translate((float)(- x), (float)(- y));
//j			g->Transform = temp_Matrix;
//j			g->DrawImage(src, 0, 0);
//j			delete g;

			return dst;
		}

		virtual CString *ToString()
		{
			return new CString("Distort/Crop");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}