#pragma once

namespace ImageEffects
{
	/// A convenience class which implements those methods of BufferedImageOp which are rarely changed.
	class AbstractBufferedImageOp abstract: BufferedImageOp
	{
	public:
		virtual BITMAPINFOHEADER *createCompatibleDestImage(BITMAPINFOHEADER *src, COLORREF& dstCM)
		{
			if (dstCM.IsEmpty)
			{
//j				dstCM = src.getColorModel();
			}
			return new BufferedImage(dstCM, src->biWidth, src->biHeight);
		}

		virtual CRectF getBounds2D(BITMAPINFOHEADER *src)
		{
			return CRect(0, 0, src->biWidth, src->biHeight);
		}

		virtual CPointF getPoint2D(CPointF& srcPt, CPointF& dstPt)
		{
			if (dstPt.IsEmpty)
				dstPt = CPointF();
			dstPt.X = (float)(srcPt.X);
			dstPt.Y = (float)(srcPt.Y);
			return dstPt;
		}

		virtual System::Collections::Hashtable *getRenderingHints()
		{
			return NULL;
		}

		/// A convenience method for getting ARGB pixels from an image. This tries to avoid the performance
		/// penalty of BufferedImage.getRGB unmanaging the image.
		virtual Array<int> *getRGB(BITMAPINFOHEADER *image, int x, int y, int width, int height, Array<int> *pixels)
		{
			int type = (int)(image->PixelFormat);
			return image->getRGB(x, y, width, height, pixels, 0, width);
		}

		/// A convenience method for setting ARGB pixels in an image. This tries to avoid the performance
		/// penalty of BufferedImage.setRGB unmanaging the image.
		virtual void setRGB(BITMAPINFOHEADER *image, int x, int y, int width, int height, Array<int> *pixels)
		{
			int type = (int)(image->PixelFormat);
			image->setRGB(x, y, width, height, pixels, 0, width);
		}

		virtual System::Object *Clone()
		{
			try
			{
				return __super::MemberwiseClone();
			}
			catch (...)
			{
				return NULL;
			}
		}
	};
}
