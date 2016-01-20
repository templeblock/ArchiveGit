#pragma once
#include "TransformFilter.h"

namespace ImageEffects
{
	class PolarFilter : TransformFilter
	{
#ifdef NOTUSED
	public:
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
#endif NOTUSED

		static const int RECT_TO_POLAR = 0;
		static const int POLAR_TO_RECT = 1;
		static const int INVERT_IN_CIRCLE = 2;

	private:
		int type;
		float width, height;
		float centreX, centreY;
		float radius;

	public:
		PolarFilter()
		{
			type = RECT_TO_POLAR;
		}

		PolarFilter(int newtype)
		{
			type = newtype;
			edgeAction = CLAMP;
		}

	public:
		virtual Array<int> *filterPixels(int newwidth, int newheight, Array<int> *inPixels, CRect& transformedSpace)
		{
			width = (float)newwidth;
			height = (float)newheight;
			centreX = width / 2;
			centreY = height / 2;
			radius = max(centreY, centreX);
			return TransformFilter::filterPixels(newwidth, newheight, inPixels, transformedSpace);
		}

	private:
		float sqr(float x)
		{
			return x * x;
		}

	public:
		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			float theta, t;
			float m, xmax, ymax;
			float r = 0;

			switch (type)
			{

				case RECT_TO_POLAR:
					theta = 0;
					if (x >= centreX)
					{
						if (y > centreY)
						{
							theta = PI - (float)(atan(((float)(x - centreX)) / ((float)(y - centreY))));
							r = (float)(sqrt(sqr(x - centreX) + sqr(y - centreY)));
						}
						else if (y < centreY)
						{
							theta = (float)(atan(((float)(x - centreX)) / ((float)(centreY - y))));
							r = (float)(sqrt(sqr(x - centreX) + sqr(centreY - y)));
						}
						else
						{
							theta = HALF_PI;
							r = x - centreX;
						}
					}
					else if (x < centreX)
					{
						if (y < centreY)
						{
							theta = TWO_PI - (float)(atan(((float)(centreX - x)) / ((float)(centreY - y))));
							r = (float)(sqrt(sqr(centreX - x) + sqr(centreY - y)));
						}
						else if (y > centreY)
						{
							theta = PI + (float)(atan(((float)(centreX - x)) / ((float)(y - centreY))));
							r = (float)(sqrt(sqr(centreX - x) + sqr(y - centreY)));
						}
						else
						{
							theta = 1.5f * PI;
							r = centreX - x;
						}
					}
					if (x != centreX)
						m = abs(((float)(y - centreY)) / ((float)(x - centreX)));
					else
						m = 0;

					if (m <= ((float)(height) / (float)(width)))
					{
						if (x == centreX)
						{
							xmax = 0;
							ymax = centreY;
						}
						else
						{
							xmax = centreX;
							ymax = m * xmax;
						}
					}
					else
					{
						ymax = centreY;
						xmax = ymax / m;
					}

					out_Renamed->SetValue((width - 1) - (width - 1) / TWO_PI * theta, 0);
					out_Renamed->SetValue(height * r / radius, 1);
					break;

				case POLAR_TO_RECT:
				{
					theta = x / width * TWO_PI;
					float theta2;

					if (theta >= 1.5f * PI)
						theta2 = TWO_PI - theta;
					else if (theta >= PI)
						theta2 = theta - PI;
					else if (theta >= 0.5f * PI)
						theta2 = PI - theta;
					else
						theta2 = theta;

					t = (float)(tan(theta2));
					if (t != 0)
						m = 1.0f / t;
					else
						m = 0;

					if (m <= ((float)(height) / (float)(width)))
					{
						if (theta2 == 0)
						{
							xmax = 0;
							ymax = centreY;
						}
						else
						{
							xmax = centreX;
							ymax = m * xmax;
						}
					}
					else
					{
						ymax = centreY;
						xmax = ymax / m;
					}

					r = radius * (float)(y / (float)(height));

					float nx = (- r) * (float)(sin(theta2));
					float ny = r * (float)(cos(theta2));

					float fx, fy;
					if (theta >= 1.5f * PI)
					{
						fx = (float)(centreX)- nx;
						fy = (float)(centreY)- ny;
					}
					else if (theta >= PI)
					{
						fx = (float)(centreX)- nx;
						fy = (float)(centreY) + ny;
					}
					else if (theta >= 0.5 * PI)
					{
						fx = (float)(centreX) + nx;
						fy = (float)(centreY) + ny;
					}
					else
					{
						fx = (float)(centreX) + nx;
						fy = (float)(centreY)- ny;
					}

					out_Renamed->SetValue(fx, 0);
					out_Renamed->SetValue(fy, 1);
					break;
				}

				case INVERT_IN_CIRCLE:
					float dx = x - centreX;
					float dy = y - centreY;
					float distance2 = dx * dx + dy * dy;
					out_Renamed->SetValue(centreX + centreX * centreX * dx / distance2, 0);
					out_Renamed->SetValue(centreY + centreY * centreY * dy / distance2, 1);
					break;
				}
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Polar Coordinates...");
		}
	};
}
