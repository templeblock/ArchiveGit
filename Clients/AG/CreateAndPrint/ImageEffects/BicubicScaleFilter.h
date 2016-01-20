#pragma once

namespace ImageEffects
{
	/// Scales an image using bi-cubic interpolation, which can't be done with AffineTransformOp.
	class BicubicScaleFilter
	{

	private:
		int width;
		int height;

	public:
		BicubicScaleFilter()
		{
			width = 32;
			height = 32;
		}

		BicubicScaleFilter(int newwidth, int newheight)
		{
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
//j			g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, (System::Object) System::Drawing::Drawing2D.InterpolationMode.Bicubic);
//j			g->DrawImage(src, 0, 0, width, height);
//j			delete g;

			return dst;
		}

		virtual CString *ToString()
		{
			return new CString("Distort/Bicubic Scale");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}
