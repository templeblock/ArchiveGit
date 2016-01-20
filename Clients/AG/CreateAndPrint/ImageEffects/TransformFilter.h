#pragma once
#include "ImageMath.h"
#include "WholeImageFilter.h"

namespace ImageEffects
{

	/// An abstract superclass for filters which distort images in some way. The subclass only needs to
	/// two methods to provide the mapping between source and destination pixels.
	class TransformFilter abstract : public WholeImageFilter
	{
#ifdef NOTUSED
	public:
		virtual property int EdgeAction
		{
			int get()
			{
				return edgeAction;
			}

			void set(int value)
			{
				edgeAction = value;
			}

		}
#endif NOTUSED

	protected:
		static const int ZERO = 0;
		static const int CLAMP = 1;
		static const int WRAP = 2;

	public:
		int edgeAction;

		TransformFilter()
		{
			edgeAction = ZERO;
		}

		virtual void transformInverse(int, int, Array<float>*) abstract;

		virtual void transformSpace(CRect& rect)
		{
		}

		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			int srcWidth = width;
			int srcHeight = height;
			int outWidth = transformedSpace.Width();
			int outHeight = transformedSpace.Height();
			int outX, outY, srcX, srcY;
			int index = 0;
			Array<int> *outPixels = new Array<int>(outWidth * outHeight);

			outX = transformedSpace.left;
			outY = transformedSpace.top;
			int rgb[4];
			float out_Renamed[2];

			for (int y = 0; y < outHeight; y++)
			{
				for (int x = 0; x < outWidth; x++)
				{
//j					transformInverse(outX + x, outY + y, out_Renamed);
					srcX = (int)(out_Renamed[0]);
					srcY = (int)(out_Renamed[1]);
					// int casting rounds towards zero, so we check out[0] < 0, not srcX < 0
					if (out_Renamed[0] < 0 || srcX >= srcWidth || out_Renamed[1] < 0 || srcY >= srcHeight)
					{
						int p;
						switch (edgeAction)
						{

							case ZERO:
							default:
								p = 0;
								break;

							case WRAP:
								p = inPixels->GetAt((mod(srcY, srcHeight) * srcWidth) + mod(srcX, srcWidth));
								break;

							case CLAMP:
								p = inPixels->GetAt((clamp(srcY, 0, srcHeight - 1) * srcWidth) + clamp(srcX, 0, srcWidth - 1));
								break;
							}
						outPixels->SetValue(p, index++);
					}
					else
					{
						float xWeight = out_Renamed[0] - srcX;
						float yWeight = out_Renamed[1] - srcY;
						int i = srcWidth * srcY + srcX;
						int dx = srcX == srcWidth - 1?0:1;
						int dy = srcY == srcHeight - 1?0:srcWidth;
						rgb[0] = inPixels->GetAt(i);
						rgb[1] = inPixels->GetAt(i + dx);
						rgb[2] = inPixels->GetAt(i + dy);
						rgb[3] = inPixels->GetAt(i + dx + dy);
						outPixels->SetValue(bilinearInterpolate(xWeight, yWeight, rgb), index++);
					}
				}
			}
			return outPixels;
		}
	};
}
