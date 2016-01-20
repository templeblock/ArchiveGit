#pragma once

namespace ImageEffects
{
	/// A convenience class which implements those methods of BufferedImageOp which are rarely changed.
	class BufferedImageOp abstract
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