#pragma once

namespace ImageEffects
{
	class FourColorFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property int ColorNW
		{
			int get()
			{
				return colorNW;
			}

			void set(int value)
			{
				colorNW = value;
				rNW = (value >> 16) & 0xff;
				gNW = (value >> 8) & 0xff;
				bNW = value & 0xff;
			}

		}
		virtual property int ColorNE
		{
			int get()
			{
				return colorNE;
			}

			void set(int value)
			{
				colorNE = value;
				rNE = (value >> 16) & 0xff;
				gNE = (value >> 8) & 0xff;
				bNE = value & 0xff;
			}

		}
		virtual property int ColorSW
		{
			int get()
			{
				return colorSW;
			}

			void set(int value)
			{
				colorSW = value;
				rSW = (value >> 16) & 0xff;
				gSW = (value >> 8) & 0xff;
				bSW = value & 0xff;
			}

		}
		virtual property int ColorSE
		{
			int get()
			{
				return colorSE;
			}

			void set(int value)
			{
				colorSE = value;
				rSE = (value >> 16) & 0xff;
				gSE = (value >> 8) & 0xff;
				bSE = value & 0xff;
			}

		}
#endif NOTUSED

	private:
		int width;
		int height;
		int colorNW;
		int colorNE;
		int colorSW;
		int colorSE;
		int rNW, gNW, bNW;
		int rNE, gNE, bNE;
		int rSW, gSW, bSW;
		int rSE, gSE, bSE;

	public:
		FourColorFilter()
		{
			colorNW = ((int)(0xffff0000));
			colorNE = ((int)(0xffff00ff));
			colorSW = ((int)(0xff0000ff));
			colorSE = ((int)(0xff00ffff));
		}

		virtual void setDimensions(int width, int height)
		{
			width = width;
			height = height;
			__super::setDimensions(width, height);
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			float fx = (float)(x) / width;
			float fy = (float)(y) / height;
			float p, q;

			p = rNW + (rNE - rNW) * fx;
			q = rSW + (rSE - rSW) * fx;
			int r = (int)(p + (q - p) * fy + 0.5f);

			p = gNW + (gNE - gNW) * fx;
			q = gSW + (gSE - gSW) * fx;
			int g = (int)(p + (q - p) * fy + 0.5f);

			p = bNW + (bNE - bNW) * fx;
			q = bSW + (bSE - bSW) * fx;
			int b = (int)(p + (q - p) * fy + 0.5f);

			return ((int)(0xff000000)) | (r << 16) | (g << 8) | b;
		}

		virtual CString *ToString()
		{
			return new CString("Texture/Four Color Fill...");
		}
	};
}
