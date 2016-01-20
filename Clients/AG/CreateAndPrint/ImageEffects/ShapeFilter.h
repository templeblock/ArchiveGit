#pragma once
#include "WholeImageFilter.h"

namespace ImageEffects
{
	class ShapeFilter : WholeImageFilter
	{
#ifdef NOTUSED
	public:
		virtual property float Factor
		{
			float get()
			{
				return factor;
			}

			void set(float value)
			{
				factor = value;
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
		virtual property bool UseAlpha
		{
			bool get()
			{
				return useAlpha;
			}

			void set(bool value)
			{
				useAlpha = value;
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
		virtual property bool Merge
		{
			bool get()
			{
				return merge;
			}

			void set(bool value)
			{
				merge = value;
			}

		}
#endif NOTUSED

		static const int LINEAR = 0;
		static const int CIRCLE_UP = 1;
		static const int CIRCLE_DOWN = 2;
		static const int SMOOTH = 3;

		static const int one = 41;
		static const int sqrt2 = (int)(41 * sqrt(2));
		static const int sqrt5 = (int)(41 * sqrt(5));

	public:
		Colormap *colormap;
	private:
		float factor;
		bool useAlpha;
		bool invert;
		bool merge;
		int type;

	public:
		ShapeFilter()
		{
			factor = 1.0f;
			useAlpha = true;
			colormap = new LinearColormap();
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			if (false)
			{
				Array<float> *map = new Array<float>(width * height);
				float max = gddistanceMap(inPixels, map, width, height);
				applyMap(map, inPixels, width, height, max);
			}
			else
			{
				Array<int> *map = new Array<int>(width * height);
				makeMap(inPixels, map, width, height);
				int max = distanceMap(map, width, height);
				applyMap(map, inPixels, width, height, max);
			}

			return inPixels;
		}

	public:
		virtual int distanceMap(Array<int> *map, int width, int height)
		{
			int xmax = width - 3;
			int ymax = height - 3;
			int max = 0;
			int v;

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int offset = x + y * width;
					if (map[offset] > 0)
					{
						if (x < 2 || x > xmax || y < 2 || y > ymax)
							v = setEdgeValue(x, y, map, width, offset, xmax, ymax);
						else
							v = setValue(map, width, offset);
						if (v > max)
							max = v;
					}
				}
			}
			for (int y = height - 1; y >= 0; y--)
			{
				for (int x = width - 1; x >= 0; x--)
				{
					int offset = x + y * width;
					if (map[offset] > 0)
					{
						if (x < 2 || x > xmax || y < 2 || y > ymax)
							v = setEdgeValue(x, y, map, width, offset, xmax, ymax);
						else
							v = setValue(map, width, offset);
						if (v > max)
							max = v;
					}
				}
			}
			return max;
		}

	private:
		void makeMap(Array<int> *pixels, Array<int> *map, int width, int height)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int offset = x + y * width;
					int b = useAlpha?(pixels[offset] >> 24) & 0xff:PixelUtils::brightness(pixels[offset]);
					map[offset] = b * one;
				}
			}
		}

		void applyMap(Array<int> *map, Array<int> *pixels, int width, int height, int max)
		{
			if (max == 0)
				max = 1;
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int offset = x + y * width;
					int m = map[offset];
					float v = 0;
					int sa = 0, sr = 0, sg = 0, sb = 0;

					if (m == 0)
					{
						// default color
						sa = sr = sg = sb = 0;
					}
					else
					{
						// get V from map
						v = clamp(factor * m / max, 0, 1);
						switch (type)
						{

							case CIRCLE_UP:
								v = (circleUp(v));
								break;

							case CIRCLE_DOWN:
								v = (circleDown(v));
								break;

							case SMOOTH:
								v = (smoothStep(0, 1, v));
								break;
							}

						if (colormap == NULL)
						{
							sr = sg = sb = (int)(v * 255);
						}
						else
						{
							int c = (colormap->getColor(v));

							sr = (c >> 16) & 0xFF;
							sg = (c >> 8) & 0xFF;
							sb = (c) & 0xFF;
						}

						sa = useAlpha?(pixels[offset] >> 24) & 0xff:PixelUtils::brightness(pixels[offset]);

						// invert v if necessary
						if (invert)
						{
							sr = 255 - sr;
							sg = 255 - sg;
							sb = 255 - sb;
						}
					}

					// write results
					if (merge)
					{
						// merge with source
						int transp = 255;
						int col = pixels[offset];

						int a = (col & ((int)(0xFF000000))) >> 24;
						int r = (col & 0xFF0000) >> 16;
						int g = (col & 0xFF00) >> 8;
						int b = (col & 0xFF);

						r = (int)((sr * r / transp));
						g = (int)((sg * g / transp));
						b = (int)((sb * b / transp));

						// clip colors
						if (r < 0)
							r = 0;
						if (r > 255)
							r = 255;
						if (g < 0)
							g = 0;
						if (g > 255)
							g = 255;
						if (b < 0)
							b = 0;
						if (b > 255)
							b = 255;

						pixels[offset] = (a << 24) | (r << 16) | (g << 8) | b;
					}
					else
					{
						// write gray shades
						pixels[offset] = (sa << 24) | (sr << 16) | (sg << 8) | sb;
					}
				}
			}
		}

		int setEdgeValue(int x, int y, Array<int> *map, int width, int offset, int xmax, int ymax)
		{
			int min, v;
			int r1, r2, r3, r4, r5;

			r1 = offset - width - width - 2;
			r2 = r1 + width;
			r3 = r2 + width;
			r4 = r3 + width;
			r5 = r4 + width;

			if (y == 0 || x == 0 || y == ymax + 2 || x == xmax + 2)
				return map[offset] = one;

			v = map[r2 + 2] + one;
			min = v;

			v = map[r3 + 1] + one;
			if (v < min)
				min = v;

			v = map[r3 + 3] + one;
			if (v < min)
				min = v;

			v = map[r4 + 2] + one;
			if (v < min)
				min = v;

			v = map[r2 + 1] + sqrt2;
			if (v < min)
				min = v;

			v = map[r2 + 3] + sqrt2;
			if (v < min)
				min = v;

			v = map[r4 + 1] + sqrt2;
			if (v < min)
				min = v;

			v = map[r4 + 3] + sqrt2;
			if (v < min)
				min = v;

			if (y == 1 || x == 1 || y == ymax + 1 || x == xmax + 1)
				return map[offset] = min;

			v = map[r1 + 1] + sqrt5;
			if (v < min)
				min = v;

			v = map[r1 + 3] + sqrt5;
			if (v < min)
				min = v;

			v = map[r2 + 4] + sqrt5;
			if (v < min)
				min = v;

			v = map[r4 + 4] + sqrt5;
			if (v < min)
				min = v;

			v = map[r5 + 3] + sqrt5;
			if (v < min)
				min = v;

			v = map[r5 + 1] + sqrt5;
			if (v < min)
				min = v;

			v = map[r4] + sqrt5;
			if (v < min)
				min = v;

			v = map[r2] + sqrt5;
			if (v < min)
				min = v;

			return map[offset] = min;
		}

		int setValue(Array<int> *map, int width, int offset)
		{
			int min, v;
			int r1, r2, r3, r4, r5;

			r1 = offset - width - width - 2;
			r2 = r1 + width;
			r3 = r2 + width;
			r4 = r3 + width;
			r5 = r4 + width;

			v = map[r2 + 2] + one;
			min = v;
			v = map[r3 + 1] + one;
			if (v < min)
				min = v;
			v = map[r3 + 3] + one;
			if (v < min)
				min = v;
			v = map[r4 + 2] + one;
			if (v < min)
				min = v;

			v = map[r2 + 1] + sqrt2;
			if (v < min)
				min = v;
			v = map[r2 + 3] + sqrt2;
			if (v < min)
				min = v;
			v = map[r4 + 1] + sqrt2;
			if (v < min)
				min = v;
			v = map[r4 + 3] + sqrt2;
			if (v < min)
				min = v;

			v = map[r1 + 1] + sqrt5;
			if (v < min)
				min = v;
			v = map[r1 + 3] + sqrt5;
			if (v < min)
				min = v;
			v = map[r2 + 4] + sqrt5;
			if (v < min)
				min = v;
			v = map[r4 + 4] + sqrt5;
			if (v < min)
				min = v;
			v = map[r5 + 3] + sqrt5;
			if (v < min)
				min = v;
			v = map[r5 + 1] + sqrt5;
			if (v < min)
				min = v;
			v = map[r4] + sqrt5;
			if (v < min)
				min = v;
			v = map[r2] + sqrt5;
			if (v < min)
				min = v;

			return map[offset] = min;
		}

	public:
		virtual float gddistanceMap(Array<int> *pixels, Array<float> *map, int width, int height)
		{
			float max_iterations;
			Array<float> *distp_cur;
			Array<float> *distp_prev;
			float min_prev;
			float float_tmp;
			int minimum;
			int min_left;
			int length;
			int i, j, k;
			int fraction;
			int prev_frac;
			int x, y;
			int end;
			int inc;

			max_iterations = 0.0f;

			length = width + 1;
			distp_prev = new Array<float>(length);
			for (i = 0; i < length; i++)
				distp_prev[i] = 0.0f;
			distp_cur = new Array<float>(length);
			for (i = 0; i < length; i++)
				distp_cur[i] = 0.0f;

			for (i = 0; i < height; i++)
			{
				for (int m = 0; m < length; m++)
					distp_cur[m] = 0.0f;

				for (j = 0; j < width; j++)
				{
					min_prev = min(distp_cur[j], distp_prev[j + 1]);
					min_left = min((width - j - 1), (height - i - 1));
					minimum = (int)(min(min_left, min_prev));
					fraction = 255;
					int src = 0;

					//	This might need to be changed to 0 instead of k = (minimum) ? (minimum - 1) : 0
					for (k = 0; k <= minimum; k++)
					{
						x = j;
						y = i + k;
						end = y - k;

						while (y >= end)
						{
							int offset = y * width + x;
							int boundary = min(width, height);
							boundary = min(boundary, (y - end)) + 1;
							inc = 1 - width;

							while (boundary-- != 0)
							{
								src = useAlpha?(pixels[offset] >> 24) & 0xff:PixelUtils::brightness(pixels[offset]);
								if (src == 0)
								{
									minimum = k;
									y = - 1;
									break;
								}
								if (src < fraction)
									fraction = src;
								x++;
								y--;
								offset += inc;
							}
						}
					}

					if (src != 0)
					{
						//	If min_left != min_prev use the previous fraction
						//	 if it is less than the one found
						if (min_left != minimum)
						{
							prev_frac = (int)(255 * (min_prev - minimum));
							if (prev_frac == 255)
								prev_frac = 0;
							fraction = min(fraction, prev_frac);
						}
						minimum++;
					}

					float_tmp = distp_cur[j + 1] = minimum + fraction / 256.0f;

					if (float_tmp > max_iterations)
						max_iterations = float_tmp;
				}

				int index = i * width;
				for (int m = 1; m <= width; m++)
					map[index++] = distp_cur[m];

				Array<float> *tmp = distp_prev;
				distp_prev = distp_cur;
				distp_cur = tmp;
			}

			return max_iterations;
		}

		virtual float ddistanceMap(Array<int> *pixels, Array<float> *map, int width, int height)
		{
			float max = 0;
			for (int y = 0; y < height; y++)
			{
				float d = 0;
				int offset = y * width;
				for (int x = 0; x < width; x++)
				{
					int v = useAlpha?(pixels[offset] >> 24) & 0xff:PixelUtils::brightness(pixels[offset]);
					if (v != 0)
					{
						map[offset] = min(d + 255, v * 255);
						d += 255;
					}
					else
					{
						d = 0;
					}
					//				map[offset] = x;
					max = max(max, map[offset]);
					offset++;
				}
			}
			return max;
		}

	private:
		void applyMap(Array<float> *map, Array<int> *pixels, int width, int height, float max)
		{
			if (max == 0)
				max = 1;
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int offset = x + y * width;
					int m = (int)(map[offset]);
					float v = 0;
					int sa = 0, sr = 0, sg = 0, sb = 0;

					if (m == 0)
					{
						// default color
						sa = sr = sg = sb = 0;
					}
					else
					{
						// get V from map
						v = clamp(factor * m / max, 0, 1);
						switch (type)
						{

							case CIRCLE_UP:
								v = (circleUp(v));
								break;

							case CIRCLE_DOWN:
								v = (circleDown(v));
								break;

							case SMOOTH:
								v = (smoothStep(0, 1, v));
								break;
							}

						if (colormap == NULL)
						{
							sr = sg = sb = (int)(v * 255);
						}
						else
						{
							int c = (colormap->getColor(v));

							sr = (c >> 16) & 0xFF;
							sg = (c >> 8) & 0xFF;
							sb = (c) & 0xFF;
						}

						sa = useAlpha?(pixels[offset] >> 24) & 0xff:PixelUtils::brightness(pixels[offset]);

						// invert v if necessary
						if (invert)
						{
							sr = 255 - sr;
							sg = 255 - sg;
							sb = 255 - sb;
						}
					}

					// write results
					if (merge)
					{
						// merge with source
						int transp = 255;
						int col = pixels[offset];

						int a = (col & ((int)(0xFF000000))) >> 24;
						int r = (col & 0xFF0000) >> 16;
						int g = (col & 0xFF00) >> 8;
						int b = (col & 0xFF);

						r = (int)((sr * r / transp));
						g = (int)((sg * g / transp));
						b = (int)((sb * b / transp));

						// clip colors
						if (r < 0)
							r = 0;
						if (r > 255)
							r = 255;
						if (g < 0)
							g = 0;
						if (g > 255)
							g = 255;
						if (b < 0)
							b = 0;
						if (b > 255)
							b = 255;

						pixels[offset] = (a << 24) | (r << 16) | (g << 8) | b;
					}
					else
					{
						// write gray shades
						pixels[offset] = (sa << 24) | (sr << 16) | (sg << 8) | sb;
					}
				}
			}
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Stylize/Shapeburst...");
		}
	};
}
