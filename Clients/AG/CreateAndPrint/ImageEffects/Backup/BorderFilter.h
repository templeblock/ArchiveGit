#pragma once

namespace ImageEffects
{
	class BorderFilter
	{
#ifdef NOTUSED
	public:
		virtual property int LeftBorder
		{
			int get()
			{
				return leftBorder;
			}

			void set(int value)
			{
				leftBorder = value;
			}

		}
		virtual property int RightBorder
		{
			int get()
			{
				return rightBorder;
			}

			void set(int value)
			{
				rightBorder = value;
			}

		}
		virtual property int TopBorder
		{
			int get()
			{
				return topBorder;
			}

			void set(int value)
			{
				topBorder = value;
			}

		}
		virtual property int BottomBorder
		{
			int get()
			{
				return bottomBorder;
			}

			void set(int value)
			{
				bottomBorder = value;
			}

		}
#endif NOTUSED

	private:


	private:
		int width, height;
		int leftBorder, rightBorder;
		int topBorder, bottomBorder;

	public:
		BorderFilter()
		{
		}

		void setHints(int hintflags)
		{
//j			hintflags &= ~ TOPDOWNLEFTRIGHT;
//j			consumer::setHints(hintflags);
		}

		void setDimensions(int newwidth, int newheight)
		{
			width = newwidth;
			height = newheight;
//j			consumer::setDimensions(width + leftBorder + rightBorder, height + topBorder + bottomBorder);
		}

		void setPixels(int x, int y, int w, int h, COLORREF& model, Array<SByte> *pixels, int off, int scansize)
		{
//j			consumer::setPixels(x + leftBorder, y + topBorder, width, height, model, pixels, 0, scansize);
		}

		void setPixels(int x, int y, int w, int h, COLORREF& model, Array<int> *pixels, int offset, int scansize)
		{
//j			consumer::setPixels(x + leftBorder, y + topBorder, width, height, model, pixels, 0, scansize);
		}

		virtual CString *ToString()
		{
			return new CString("Distort/Border...");
		}
	};
}
