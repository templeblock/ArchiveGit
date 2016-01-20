#pragma once
#include "TransformFilter.h"

namespace ImageEffects
{
	class PerspectiveFilter : TransformFilter
	{
#ifdef NOTUSED
	public:
		virtual property float OriginX
		{
			float get()
			{
				return x0 - (int)(min(min(x0, x1), min(x2, x3)));
			}

		}
		virtual property float OriginY
		{
			float get()
			{
				return y0 - (int)(min(min(y0, y1), min(y2, y3)));
			}

		}
#endif NOTUSED

	private:
		float x0, y0, x1, y1, x2, y2, x3, y3;
		float dx1, dy1, dx2, dy2, dx3, dy3;
		float A, B, C, D, E, F, G, H, I;

	public:
		PerspectiveFilter()
		{
			x0 = 0;
			y0 = 0;
			x1 = 100;
			y1 = 0;
			x2 = 100;
			y2 = 100;
			x3 = 0;
			y3 = 100;
		}

		PerspectiveFilter(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
		{
			setCorners(x0, y0, x1, y1, x2, y2, x3, y3);
		}

		virtual void setCorners(float newx0, float newy0, float newx1, float newy1, float newx2, float newy2, float newx3, float newy3)
		{
			x0 = newx0;
			y0 = newy0;
			x1 = newx1;
			y1 = newy1;
			x2 = newx2;
			y2 = newy2;
			x3 = newx3;
			y3 = newy3;

			dx1 = x1 - x2;
			dy1 = y1 - y2;
			dx2 = x3 - x2;
			dy2 = y3 - y2;
			dx3 = x0 - x1 + x2 - x3;
			dy3 = y0 - y1 + y2 - y3;

			float a11, a12, a13, a21, a22, a23, a31, a32;

			if (dx3 == 0 && dy3 == 0)
			{
				a11 = x1 - x0;
				a21 = x2 - x1;
				a31 = x0;
				a12 = y1 - y0;
				a22 = y2 - y1;
				a32 = y0;
				a13 = a23 = 0;
			}
			else
			{
				a13 = (dx3 * dy2 - dx2 * dy3) / (dx1 * dy2 - dy1 * dx2);
				a23 = (dx1 * dy3 - dy1 * dx3) / (dx1 * dy2 - dy1 * dx2);
				a11 = x1 - x0 + a13 * x1;
				a21 = x3 - x0 + a23 * x3;
				a31 = x0;
				a12 = y1 - y0 + a13 * y1;
				a22 = y3 - y0 + a23 * y3;
				a32 = y0;
			}

			A = a22 - a32 * a23;
			B = a31 * a23 - a21;
			C = a21 * a32 - a31 * a22;
			D = a32 * a13 - a12;
			E = a11 - a31 * a13;
			F = a31 * a12 - a11 * a32;
			G = a12 * a23 - a22 * a13;
			H = a21 * a13 - a11 * a23;
			I = a11 * a22 - a21 * a12;
		}

	public:
		virtual void transformSpace(CRect& rect)
		{
			rect.left = (int)(min(min(x0, x1), min(x2, x3)));
			rect.top = (int)(min(min(y0, y1), min(y2, y3)));
			rect.right = rect.left + (int)(max(max(x0, x1), max(x2, x3)))- rect.left;
			rect.bottom = rect.top + (int)(max(max(y0, y1), max(y2, y3)))- rect.top;
		}

		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			out_Renamed->SetValue(0.5f + originalSpace.Width() * (A * x + B * y + C) / (G * x + H * y + I), 0);
			out_Renamed->SetValue(0.5f + originalSpace.Height() * (D * x + E * y + F) / (G * x + H * y + I), 1);
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Perspective...");
		}
	};
}
