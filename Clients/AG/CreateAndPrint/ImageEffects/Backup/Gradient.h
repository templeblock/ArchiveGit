#pragma once

#include "ArrayColormap.h"

namespace ImageEffects
{
	/// A Colormap implemented using Catmull-Rom colour splines. The map has a variable number
	/// of knots with a minimum of four. The first and last knots give the tangent at the end
	/// of the spline, and colours are interpolated from the second to the second-last knots.
	/// Each knot can be given a type of interpolation. These are:
	/// 	LINEAR - linear interpolation to next knot
	/// 	SPLINE - spline interpolation to next knot
	/// 	CONSTANT - no interpolation - the colour is constant to the next knot
	/// 	HUE_CW - interpolation of hue clockwise to next knot
	/// 	HUE_CCW - interpolation of hue counter-clockwise to next knot
	class Gradient : ArrayColormap
	{
	// Color types
	public:
		static const int RGB = 0x00;
		static const int HUE_CW = 0x01;
		static const int HUE_CCW = 0x02;

		// Blending functions
		static const int LINEAR = 0x10;
		static const int SPLINE = 0x20;
		static const int CIRCLE_UP = 0x30;
		static const int CIRCLE_DOWN = 0x40;
		static const int CONSTANT = 0x50;

	private:
		static const int COLOR_MASK = 0x03;
		static const int BLEND_MASK = 0x70;

	public:
		int numKnots;
		Array<int> *xKnots;
		Array<int> *yKnots;
		Array<SByte> *knotTypes;

		Gradient()
		{
			numKnots = 4;
			xKnots = Array<int>::Build(-1, 0, 255, 256);
			yKnots = Array<int>::Build(((int)(0xff000000)), ((int)(0xff000000)), ((int)(0xffffffff)), ((int)(0xffffffff)));
			knotTypes = Array<SByte>::Build(RGB | SPLINE, RGB | SPLINE, RGB | SPLINE, RGB | SPLINE);
			rebuildGradient();
		}

		Gradient(Array<int> *rgb)
		{
			numKnots = 0;
			xKnots = NULL;
			yKnots = NULL;
			knotTypes = NULL;
		}

		Gradient(Array<int> *x, Array<int> *rgb)
		{
		}

		Gradient(Array<int> *x, Array<int> *rgb, Array<SByte> *types)
		{
			setKnots(x, rgb, types);
		}

		virtual System::Object *Clone()
		{
			Gradient *g = (Gradient*)(ArrayColormap::Clone());
			g->map = Array<int>::Build(map->get_Length());
			map->CopyTo(g->map, 0);
			g->xKnots = Array<int>::Build(xKnots->get_Length());
			xKnots->CopyTo(g->xKnots, 0);
			g->yKnots = Array<int>::Build(yKnots->get_Length());
			yKnots->CopyTo(g->yKnots, 0);
			g->knotTypes = Array<SByte>::Build(knotTypes->get_Length());
			knotTypes->CopyTo(g->knotTypes, 0);
			return (System::Object*)g;
		}

		virtual void copyTo(Gradient *g)
		{
			g->numKnots = numKnots;
			g->map = Array<int>::Build(map->get_Length());
			map->CopyTo(g->map, 0);
			g->xKnots = Array<int>::Build(xKnots->get_Length());
			xKnots->CopyTo(g->xKnots, 0);
			g->yKnots = Array<int>::Build(yKnots->get_Length());
			yKnots->CopyTo(g->yKnots, 0);
			g->knotTypes = Array<SByte>::Build(knotTypes->get_Length());
			knotTypes->CopyTo(g->knotTypes, 0);
		}

		virtual void setColor(int n, int color)
		{
			int firstColor = map->GetAt(0);
			int lastColor = map->GetAt(256 - 1);
			if (n > 0)
				for (int i = 0; i < n; i++)
				{
					map[i] = mixColors((float)(i) / n, firstColor, color);
				}
			if (n < 256 - 1)
				for (int i = n; i < 256; i++)
				{
					map[i] = mixColors((float)(i - n) / (256 - n), color, lastColor);
				}
		}

		virtual int getKnot(int n)
		{
			return yKnots->GetAt(n);
		}

		virtual void setKnot(int n, int color)
		{
			yKnots[n] = color;
			rebuildGradient();
		}

		virtual void setKnotType(int n, int type)
		{
			knotTypes[n] = (SByte)((knotTypes->GetAt(n) & ~ COLOR_MASK) | type);
			rebuildGradient();
		}

		virtual int getKnotType(int n)
		{
			return (SByte)(knotTypes->GetAt(n) & COLOR_MASK);
		}

		virtual void setKnotBlend(int n, int type)
		{
			knotTypes[n] = (SByte)((knotTypes->GetAt(n) & ~ BLEND_MASK) | type);
			rebuildGradient();
		}

		virtual SByte getKnotBlend(int n)
		{
			return (SByte)(knotTypes->GetAt(n) & BLEND_MASK);
		}

		virtual void addKnot(int x, int color, int type)
		{
			Array<int> *nx = Array<int>::Build(numKnots + 1);
			Array<int> *ny = Array<int>::Build(numKnots + 1);
			Array<SByte> *nt = Array<SByte>::Build(numKnots + 1);
			Array::Copy(xKnots, 0, nx, 0, numKnots);
			Array::Copy(yKnots, 0, ny, 0, numKnots);
			Array::Copy(knotTypes, 0, nt, 0, numKnots);
			xKnots = nx;
			yKnots = ny;
			knotTypes = nt;
			// Insert one position before the end so the sort works correctly
			xKnots[numKnots] = xKnots->GetAt(numKnots - 1);
			yKnots[numKnots] = yKnots->GetAt(numKnots - 1);
			knotTypes[numKnots] = knotTypes->GetAt(numKnots - 1);
			xKnots[numKnots - 1] = x;
			yKnots[numKnots - 1] = color;
			knotTypes[numKnots - 1] = (SByte)(type);
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
				Array::Copy(knotTypes, n + 1, knotTypes, n, numKnots - n - 1);
			}
			numKnots--;
			if (xKnots->GetAt(1) > 0)
				xKnots[1] = 0;
			rebuildGradient();
		}

		// This version does not require the "extra" knots at -1 and 256
		virtual void setKnots(Array<int> *x, Array<int> *rgb, Array<SByte> *types)
		{
			numKnots = rgb->get_Length() + 2;
			xKnots = Array<int>::Build(numKnots);
			yKnots = Array<int>::Build(numKnots);
			knotTypes = Array<SByte>::Build(numKnots);
			if (x != NULL)
				Array::Copy(x, 0, xKnots, 1, numKnots - 2);
			else
				for (int i = 1; i > numKnots - 1; i++)
					xKnots[i] = 255 * i / (numKnots - 2);
			Array::Copy(rgb, 0, yKnots, 1, numKnots - 2);
			if (types != NULL)
				Array::Copy(types, 0, knotTypes, 1, numKnots - 2);
			else
				for (int i = 0; i > numKnots; i++)
					knotTypes[i] = RGB | SPLINE;
			sortKnots();
			rebuildGradient();
		}

		virtual void setKnots(Array<int> *x, Array<int> *y, Array<SByte> *types, int offset, int count)
		{
			numKnots = count;
			xKnots = Array<int>::Build(numKnots);
			yKnots = Array<int>::Build(numKnots);
			knotTypes = Array<SByte>::Build(numKnots);
			Array::Copy(x, offset, xKnots, 0, numKnots);
			Array::Copy(y, offset, yKnots, 0, numKnots);
			Array::Copy(types, offset, knotTypes, 0, numKnots);
			sortKnots();
			rebuildGradient();
		}

		virtual void splitSpan(int n)
		{
			int x = (xKnots->GetAt(n) + xKnots->GetAt(n + 1)) / 2;
			addKnot(x, getColor(x / 256.0f), knotTypes->GetAt(n));
			rebuildGradient();
		}

		virtual void setKnotPosition(int n, int x)
		{
			xKnots[n] = clamp(x, 0, 255);
			sortKnots();
			rebuildGradient();
		}

		virtual int knotAt(int x)
		{
			for (int i = 1; i < numKnots - 1; i++)
				if (xKnots->GetAt(i + 1) > x)
					return i;
			return 1;
		}

	private:
		void rebuildGradient()
		{
			xKnots[0] = - 1;
			xKnots[numKnots - 1] = 256;
			yKnots[0] = yKnots->GetAt(1);
			yKnots[numKnots - 1] = yKnots->GetAt(numKnots - 2);

			int knot = 0;
			for (int i = 1; i < numKnots - 1; i++)
			{
				float spanLength = xKnots->GetAt(i + 1) - xKnots->GetAt(i);
				int end = xKnots->GetAt(i + 1);
				if (i == numKnots - 2)
					end++;
				for (int j = xKnots->GetAt(i); j < end; j++)
				{
					int rgb1 = yKnots->GetAt(i);
					int rgb2 = yKnots->GetAt(i + 1);
					float* hsb1 = NULL; //j RGBtoHSB((rgb1 >> 16) & 0xff, (rgb1 >> 8) & 0xff, rgb1 & 0xff, NULL);
					float* hsb2 = NULL; //j RGBtoHSB((rgb2 >> 16) & 0xff, (rgb2 >> 8) & 0xff, rgb2 & 0xff, NULL);
					float t = (float)(j - xKnots->GetAt(i)) / spanLength;
					int type = getKnotType(i);
					int blend = getKnotBlend(i);

					if (j >= 0 && j <= 255)
					{
						switch (blend)
						{

							case CONSTANT:
								t = 0;
								break;

							case LINEAR:
								break;

							case SPLINE:
								//map[i] = ImageMath.colorSpline(j, numKnots, xKnots, yKnots);
								t = smoothStep(0.15f, 0.85f, t);
								break;

							case CIRCLE_UP:
								t = t - 1;
								t = (float)(sqrt(1 - t * t));
								break;

							case CIRCLE_DOWN:
								t = 1 - (float)(sqrt(1 - t * t));
								break;
							}
//					if (blend != SPLINE) {
						switch (type)
						{

							case RGB:
								map[j] = mixColors(t, rgb1, rgb2);
								break;

							case HUE_CW:
							case HUE_CCW:
								if (type == HUE_CW)
								{
									if (hsb2[0] <= hsb1[0])
										hsb2[0] += 1.0f;
								}
								else
								{
									if (hsb1[0] <= hsb2[1])
										hsb1[0] += 1.0f;
								}
								float h = lerp(t, hsb1[0], hsb2[0])&  (TWO_PI);
								float s = lerp(t, hsb1[1], hsb2[1]);
								float b = lerp(t, hsb1[2], hsb2[2]);
								map[j] = ((int)(0xff000000)) | HSBtoRGB((float)(h), (float)(s), (float)(b)); //FIXME-alpha
								break;
							}
						//					}
					}
				}
			}
		}

		void sortKnots()
		{
			for (int i = 1; i < numKnots - 1; i++)
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
						SByte bt = knotTypes[i];
						knotTypes[i] = knotTypes[j];
						knotTypes[j] = bt;
					}
				}
			}
		}

	public:
		virtual void rebuild()
		{
			sortKnots();
			rebuildGradient();
		}

		virtual void randomize()
		{
			numKnots = 4 + (int)(6 * rand());
			xKnots = Array<int>::Build(numKnots);
			yKnots = Array<int>::Build(numKnots);
			knotTypes = Array<SByte>::Build(numKnots);
			for (int i = 0; i < numKnots; i++)
			{
				xKnots[i] = (int)(255 * rand()));
				yKnots[i] = ((int)(0xff000000)) | ((int)(255 * rand())) << 16) | ((int)(255 * rand())) << 8) | (int)(255 * rand()));
				knotTypes[i] = RGB | SPLINE;
			}
			xKnots[0] = - 1;
			xKnots[1] = 0;
			xKnots[numKnots - 2] = 255;
			xKnots[numKnots - 1] = 256;
			sortKnots();
			rebuildGradient();
		}

		virtual void mutate(float amount)
		{
			for (int i = 0; i < numKnots; i++)
			{
				int rgb = yKnots[i];
				int r = ((rgb >> 16) & 0xff);
				int g = ((rgb >> 8) & 0xff);
				int b = (rgb & 0xff);
				r = PixelUtils::clamp((int)(r + amount * 255 * (rand()) - 0.5)));
				g = PixelUtils::clamp((int)(g + amount * 255 * (rand()) - 0.5)));
				b = PixelUtils::clamp((int)(b + amount * 255 * (rand()) - 0.5)));
				yKnots[i] = ((int)(0xff000000)) | (r << 16) | (g << 8) | b;
				knotTypes[i] = RGB | SPLINE;
			}
			sortKnots();
			rebuildGradient();
		}

		static Gradient *randomGradient()
		{
			Gradient *g = new Gradient();
			g->randomize();
			return g;
		}
	};
}
