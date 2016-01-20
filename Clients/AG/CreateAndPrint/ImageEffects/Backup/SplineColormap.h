#pragma once
#include "ArrayColormap.h"

namespace ImageEffects
{
	/// A Colormap implemented using Catmull-Rom colour splines. The map has a variable number
	/// of knots with a minimum of four. The first and last knots give the tangent at the end
	/// of the spline, and colours are interpolated from the second to the second-last knots.
	/// 

	class SplineColormap : ArrayColormap
	{

	public:
		int numKnots;
		CAtlArray<int> xKnots;
		CAtlArray<int> yKnots;

		SplineColormap()
		{
			numKnots = 4;
			xKnots.Add(0, 0, 255, 255);
			yKnots.Add(((int)(0xff000000)), ((int)(0xff000000)), ((int)(0xffffffff)), ((int)(0xffffffff)));
			rebuildGradient();
		}

		SplineColormap(CAtlArray<int> newxKnots, CAtlArray<int> newyKnots)
		{
			xKnots = newxKnots;
			yKnots = newyKnots;
			numKnots = xKnots.GetCount();
			rebuildGradient();
		}

		virtual int getKnot(int n)
		{
			return yKnots[n];
		}

		virtual void setKnot(int n, int color)
		{
			yKnots[n] = color;
			rebuildGradient();
		}

		virtual void addKnot(int x, int color)
		{
			CAtlArray<int> nx; nx.SetCount(numKnots + 1);
			CAtlArray<int> ny; ny.SetCount(numKnots + 1);
			xKnots.Copy(nx);
			yKnots.Copy(ny);
//j			Array::Copy(xKnots, 0, nx, 0, numKnots);
//j			Array::Copy(yKnots, 0, ny, 0, numKnots);
//j			xKnots = nx;
//j			yKnots = ny;
			xKnots[numKnots] = x;
			yKnots[numKnots] = color;
			numKnots++;
			sortKnots();
			rebuildGradient();
		}

		virtual void removeKnot(int n)
		{
			if (numKnots <= 4)
				return ;
			if (n < numKnots - 1)
			{
				Array::Copy(xKnots, n + 1, xKnots, n, numKnots - n - 1);
				Array::Copy(yKnots, n + 1, yKnots, n, numKnots - n - 1);
			}
			numKnots--;
			rebuildGradient();
		}

		virtual void setKnotPosition(int n, int x)
		{
			xKnots[n] = PixelUtils::clamp(x);
			sortKnots();
			rebuildGradient();
		}

	private:
		void rebuildGradient()
		{
			xKnots[0] = - 1;
			xKnots[numKnots - 1] = 256;
			yKnots[0] = yKnots[1];
			yKnots[numKnots - 1] = yKnots[numKnots - 2];
			for (int i = 0; i < 256; i++)
				map[i] = colorSpline(i, numKnots, xKnots, yKnots);
		}

		void sortKnots()
		{
			for (int i = 1; i < numKnots; i++)
			{
				for (int j = 1; j < i; j++)
				{
					if (xKnots[i] < xKnots[j])
					{
						int t = xKnots[i];
						xKnots[i] = xKnots[j];
						xKnots[j] = t;
						t = yKnots[i];
						yKnots[i] = yKnots[j];
						yKnots[j] = t;
					}
				}
			}
		}
	};
}
