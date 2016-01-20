#pragma once

namespace ImageEffects
{
	/// Scales an image using the area-averaging algorithm, which can't be done with AffineTransformOp.
	class ScaleFilter
	{

	private:
		int width;
		int height;

	public:
		ScaleFilter()
		{
			width = 32;
			height = 32;
		}

		ScaleFilter(int width, int height)
		{
			width = width;
			height = height;
		}

		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int w = src->biWidth;
			int h = src->biHeight;

			if (dst == NULL)
			{
//j				COLORREF dstCM;
//j				dstCM = src.getColorModel();
//j				dst = new BufferedImage(dstCM, w, h);
			}

//j			System::Drawing::Image *scaleImage = new BITMAPINFOHEADER(src, System::Drawing::Size(w, h));
//j			System::Drawing::Graphics *g = System::Drawing::Graphics::FromImage(dst);
//j			g->DrawImage(src, 0, 0, width, height);
//j			delete g;

			return dst;
		}

		virtual CString *ToString()
		{
			return new CString("Distort/Scale");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}
