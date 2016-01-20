#pragma once

namespace ImageEffects
{
	class FadeFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property float Angle
		{
			float get()
			{
				return angle;
			}

			void set(float value)
			{
				angle = value;
				float cos = (float)(cos(value));
				float sin = (float)(sin(value));
				m00 = cos;
				m01 = sin;
				m10 = - sin;
				m11 = cos;
			}

		}
		virtual property int Sides
		{
			int get()
			{
				return sides;
			}

			void set(int value)
			{
				sides = value;
			}

		}
		virtual property float FadeStart
		{
			float get()
			{
				return fadeStart;
			}

			void set(float value)
			{
				fadeStart = value;
			}

		}
		virtual property float FadeWidth
		{
			float get()
			{
				return fadeWidth;
			}

			void set(float value)
			{
				fadeWidth = value;
			}

		}
		virtual property bool Invert
		{
			bool get()
			{
				return invert;
			}

			void set(bool value)
			{
				invert = value;
			}

		}
#endif NOTUSED

	private:
		int width, height;
		float angle;
		float fadeStart;
		float fadeWidth;
		int sides;
		bool invert;
		float m00;
		float m01;
		float m10;
		float m11;

	public:
		FadeFilter()
		{
			angle = 0.0f;
			fadeStart = 1.0f;
			fadeWidth = 10.0f;
			m00 = 1.0f;
			m01 = 0.0f;
			m10 = 0.0f;
			m11 = 1.0f;
		}
		
		virtual void setDimensions(int newwidth, int newheight)
		{
			width = newwidth;
			height = newheight;
			PointFilter::setDimensions(width, height);
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			float nx = m00 * x + m01 * y;
			float ny = m10 * x + m11 * y;
			if (sides == 2)
			{
				nx = (float)(sqrt(nx * nx + ny * ny));
			}
			else if (sides == 3)
				nx = mod(nx, 16);
			else if (sides == 4)
				nx = symmetry(nx, 16);
			int alpha = (int)(smoothStep(fadeStart, fadeStart + fadeWidth, nx) * 255);
			if (invert)
				alpha = 255 - alpha;
			return (alpha << 24) | (rgb & 0x00ffffff);
		}

		virtual float symmetry(float x, float b)
		{
			x = mod(x, 2 * b);
			if (x > b)
				return 2 * b - x;
			return x;
		}

		virtual CString *ToString()
		{
			return new CString("Fade...");
		}
	};
}
