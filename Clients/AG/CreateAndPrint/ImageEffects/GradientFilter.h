#pragma once
#include "LinearColormap.h"

namespace ImageEffects
{
	class GradientFilter
	{
	private:
		void InitBlock()
		{
			paintMode = PixelUtils::NORMAL;
		}
#ifdef NOTUSED
	public:
		virtual property CPoint Point1
		{
			CPoint get()
			{
				return p1;
			}

			void set(CPoint value)
			{
				p1 = value;
			}

		}
		virtual property CPoint Point2
		{
			CPoint get()
			{
				return p2;
			}

			void set(CPoint value)
			{
				p2 = value;
			}

		}
		virtual property int Type
		{
			int get()
			{
				return type;
			}

			void set(int value)
			{
				type = value;
			}

		}
		virtual property int Interpolation
		{
			int get()
			{
				return interpolation;
			}

			void set(int value)
			{
				interpolation = value;
			}

		}
		virtual property float Angle
		{
			float get()
			{
				return angle;
			}

			void set(float value)
			{
				angle = value;
				p2 = CPoint((int)(64 * cos(value)), (int)(64 * sin(value)));
			}
		}
		virtual property Colormap *Colormap
		{
			Colormap *get()
			{
				return colormap;
			}

			void set(Colormap *value)
			{
				colormap = value;
			}

		}
		virtual property int PaintMode
		{
			int get()
			{
				return paintMode;
			}

			void set(int value)
			{
				paintMode = value;
			}

		}
#endif NOTUSED

	public:
		static const int LINEAR = 0;
		static const int BILINEAR = 1;
		static const int RADIAL = 2;
		static const int CONICAL = 3;
		static const int BICONICAL = 4;
		static const int SQUARE = 5;

		static const int INT_LINEAR = 0;
		static const int INT_CIRCLE_UP = 1;
		static const int INT_CIRCLE_DOWN = 2;
		static const int INT_SMOOTH = 3;

	private:
		float angle;
		int color1;
		int color2;
		bool repeat;
		float x1;
		float y1;
		float dx;
		float dy;
		Colormap *colormap;
		int type;
		int interpolation;
		int paintMode;
		int p1X;
		int p1Y;
		int p2X;
		int p2Y;

	public:
		GradientFilter()
		{
			int color1 = ((int)(0xff000000));
			int color2 = ((int)(0xffffffff));
			int interpolation = INT_LINEAR;
			p1X = 0;
			p1Y = 0;
			p2X = 64;
			p2Y = 64;
			InitBlock();
		}

		GradientFilter(CPoint& p1, CPoint& p2, int newcolor1, int newcolor2, bool newrepeat, int newtype, int newinterpolation)
		{
			InitBlock();
			p1X = p1.x;
			p1Y = p1.y;
			p2X = p2.x;
			p2Y = p2.y;
			color1 = newcolor1;
			color2 = newcolor2;
			repeat = newrepeat;
			type = newtype;
			interpolation = newinterpolation;
			colormap = (Colormap*)new LinearColormap(color1, color2);
		}

		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int width = src->biWidth;
			int height = src->biHeight;

			if (dst == NULL)
			{
//j				COLORREF tempAux = CLR_NONE;
//j				dst = createCompatibleDestImage(src, tempAux);
			}

			int rgb1, rgb2;
			float x1, y1, x2, y2;
			x1 = (float)p1X;
			x2 = (float)p2X;

			if (x1 > x2 && type != RADIAL)
			{
				y1 = x1;
				x1 = x2;
				x2 = y1;
				y1 = (float)p2Y;
				y2 = (float)p1Y;
				rgb1 = color2;
				rgb2 = color1;
			}
			else
			{
				y1 = (float)p1Y;
				y2 = (float)p2Y;
				rgb1 = color1;
				rgb2 = color2;
			}
			float dx = x2 - x1;
			float dy = y2 - y1;
			float lenSq = dx * dx + dy * dy;
			x1 = x1;
			y1 = y1;
			if (lenSq >= System::Single::Epsilon)
			{
				dx = dx / lenSq;
				dy = dy / lenSq;
				if (repeat)
				{
					dx = mod(dx, 1.0f);
					dy = mod(dy, 1.0f);
				}
			}
			dx = dx;
			dy = dy;

			Array<int> *pixels = new Array<int>(width);
			for (int y = 0; y < height; y++)
			{
//j				getRGB(src, 0, y, width, 1, pixels);
				switch (type)
				{

					case LINEAR:
					case BILINEAR:
						linearGradient(pixels, y, width, 1);
						break;

					case RADIAL:
						radialGradient(pixels, y, width, 1);
						break;

					case CONICAL:
					case BICONICAL:
						conicalGradient(pixels, y, width, 1);
						break;

					case SQUARE:
						squareGradient(pixels, y, width, 1);
						break;
					}

//j				setRGB(dst, 0, y, width, 1, pixels);
			}
			return dst;
		}

	private:
		void repeatGradient(Array<int> *pixels, int w, int h, float rowrel, float dx, float dy)
		{
			int off = 0;
			for (int y = 0; y < h; y++)
			{
				float colrel = rowrel;
				int j = w;
				int rgb;
				while (--j >= 0)
				{
					if (type == BILINEAR)
						rgb = colormap->getColor(map(triangle(colrel)));
					else
						rgb = colormap->getColor(map(mod(colrel, 1.0f)));
					pixels->SetValue(PixelUtils::combinePixels(rgb, pixels->GetAt(off), paintMode), off);
					off++;
					colrel += dx;
				}
				rowrel += dy;
			}
		}

		void singleGradient(Array<int> *pixels, int w, int h, float rowrel, float dx, float dy)
		{
			int off = 0;
			for (int y = 0; y < h; y++)
			{
				float colrel = rowrel;
				int j = w;
				int rgb;
				if (colrel <= 0.0)
				{
					rgb = colormap->getColor(0);
					do
					{
						pixels->SetValue(PixelUtils::combinePixels(rgb, pixels->GetAt(off), paintMode), off);
						off++;
						colrel += dx;
					} while (--j > 0 && colrel <= 0.0);
				}
				while (colrel < 1.0 && --j >= 0)
				{
					if (type == BILINEAR)
						rgb = colormap->getColor(map(triangle(colrel)));
					else
						rgb = colormap->getColor(map(colrel));
					pixels->SetValue(PixelUtils::combinePixels(rgb, pixels->GetAt(off), paintMode), off);
					off++;
					colrel += dx;
				}
				if (j > 0)
				{
					if (type == BILINEAR)
						rgb = colormap->getColor(0.0f);
					else
						rgb = colormap->getColor(1.0f);
					do
					{
						pixels->SetValue(PixelUtils::combinePixels(rgb, pixels->GetAt(off), paintMode), off);
						off++;
					} while (--j > 0);
				}
				rowrel += dy;
			}
		}

		void linearGradient(Array<int> *pixels, int y, int w, int h)
		{
			int x = 0;
			float rowrel = (x - x1) * dx + (y - y1) * dy;
			if (repeat)
				repeatGradient(pixels, w, h, rowrel, dx, dy);
			else
				singleGradient(pixels, w, h, rowrel, dx, dy);
		}

		void radialGradient(Array<int> *pixels, int y, int w, int h)
		{
			int off = 0;
			float radius = distance((float)p2X - p1X, (float)p2Y - p1Y);
			for (int x = 0; x < w; x++)
			{
				float distance1 = distance((float)x - p1X, (float)y - p1Y);
				float ratio = distance1 / radius;
				if (repeat)
					ratio = mod(ratio, 2.0f);
				else if (ratio > 1.0)
					ratio = 1.0f;
				int rgb = colormap->getColor(map(ratio));
				pixels->SetValue(PixelUtils::combinePixels(rgb, pixels->GetAt(off), paintMode), off);
				off++;
			}
		}

		void squareGradient(Array<int> *pixels, int y, int w, int h)
		{
			int off = 0;
			float radius = (float)max(abs(p2X - p1X), abs(p2Y - p1Y));
			for (int x = 0; x < w; x++)
			{
				float distance = (float)max(abs(x - p1X), abs(y - p1Y));
				float ratio = distance / radius;
				if (repeat)
					ratio = mod(ratio, 2.0f);
				else if (ratio > 1.0)
					ratio = 1.0f;
				int rgb = colormap->getColor(map(ratio));
				pixels->SetValue(PixelUtils::combinePixels(rgb, pixels->GetAt(off), paintMode), off);
				off++;
			}
		}

		void conicalGradient(Array<int> *pixels, int y, int w, int h)
		{
			int off = 0;
			float angle0 = atan2((float)p2X - p1X, (float)p2Y - p1Y);
			for (int x = 0; x < w; x++)
			{
				float angle = (atan2((float)x - p1X, (float)y - p1Y) - angle0) / (TWO_PI);
				angle += 1.0f;
				angle = mod(angle, 1.0f);
				if (type == BICONICAL)
					angle = triangle(angle);
				int rgb = colormap->getColor(map(angle));
				pixels->SetValue(PixelUtils::combinePixels(rgb, pixels->GetAt(off), paintMode), off);
				off++;
			}
		}

		float map(float v)
		{
			if (repeat)
				v = v > 1.0?2.0f - v:v;
			switch (interpolation)
			{

				case INT_CIRCLE_UP:
					v = circleUp(clamp(v, 0.0f, 1.0f));
					break;

				case INT_CIRCLE_DOWN:
					v = circleDown(clamp(v, 0.0f, 1.0f));
					break;

				case INT_SMOOTH:
					v = smoothStep(0, 1, v);
					break;
				}
			return v;
		}

		float distance(float a, float b)
		{
			return (float)(sqrt(a * a + b * b));
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Other/Gradient Fill...");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}
