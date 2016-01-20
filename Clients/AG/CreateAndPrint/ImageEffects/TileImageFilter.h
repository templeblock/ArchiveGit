#pragma once

namespace ImageEffects
{
	class TileImageFilter
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
		virtual property Array<Array<int>*> *SymmetryMatrix
		{
			Array<Array<int>*> *get()
			{
				return symmetryMatrix;
			}

			void set(Array<Array<int>*> *value)
			{
				symmetryMatrix = value;
				symmetryRows = value->Length;
				symmetryCols = value[0]->Length;
			}

		}
#endif NOTUSED

	private:


	public:
		static const int FLIP_NONE = 0;
		static const int FLIP_H = 1;
		static const int FLIP_V = 2;
		static const int FLIP_HV = 3;
		static const int FLIP_180 = 4;

	private:
		int width;
		int height;
		int tileWidth;
		int tileHeight;
		Array<Array<int>*> *symmetryMatrix;
		int symmetryRows;
		int symmetryCols;

	public:
		TileImageFilter()
		{
			width = 32;
			height = 32;
			symmetryRows = 2;
			symmetryCols = 2;
		}

		TileImageFilter(int newwidth, int newheight)
		{
			width = newwidth;
			height = newheight;
		}

		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int tileWidth = src->biWidth;
			int tileHeight = src->biHeight;

			if (dst == NULL)
			{
//j				COLORREF dstCM;
//j				dstCM = src.getColorModel();
//j				dst = new BufferedImage(dstCM, width, height);
			}

//g			System::Drawing::Graphics *g = System::Drawing::Graphics::FromImage(dst);
			for (int y = 0; y < height; y += tileHeight)
			{
				for (int x = 0; x < width; x += tileWidth)
				{
//g					g->DrawImage(src, x, y);
				}
			}
//g			delete g;

			return dst;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Tile");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}