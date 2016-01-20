#pragma once
#include "TransformFilter.h"

namespace ImageEffects
{


	class OffsetFilter : TransformFilter
	{
#ifdef NOTUSED
	public:
		virtual property int XOffset
		{
			int get()
			{
				return xOffset;
			}

			void set(int value)
			{
				xOffset = value;
			}
		}
		virtual property int YOffset
		{
			int get()
			{
				return yOffset;
			}

			void set(int value)
			{
				yOffset = value;
			}
		}
		virtual property bool Wrap
		{
			bool get()
			{
				return wrap;
			}

			void set(bool value)
			{
				wrap = value;
			}
		}
#endif NOTUSED

	private:


	private:
		int width, height;
		int xOffset, yOffset;
		bool wrap;

	public:
		OffsetFilter()
		{
			width = height = 0;
			xOffset = yOffset = 0;
			wrap = true;
		}

		OffsetFilter(int newxOffset, int newyOffset, bool newwrap)
		{
			xOffset = newxOffset;
			yOffset = newyOffset;
			wrap = newwrap;
		}

	public:
		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			out_Renamed->SetValue((float)((x + width - xOffset)&  width), 0);
			out_Renamed->SetValue((float)((y + height - yOffset)&  height), 1);
		}

		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels)
		{
			width = width;
			height = height;
			while (xOffset < 0)
				xOffset += width;
			while (yOffset < 0)
				yOffset += height;
			xOffset = width;
			yOffset = height;
			CRect rect;
			return TransformFilter::filterPixels(width, height, inPixels, rect);
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Offset...");
		}
	};
}