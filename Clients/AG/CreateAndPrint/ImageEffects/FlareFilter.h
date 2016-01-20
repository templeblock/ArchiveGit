#pragma once
#include "Noise.h"

namespace ImageEffects
{
	class FlareFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property int Color
		{
			int get()
			{
				return color;
			}

			void set(int value)
			{
				color = value;
			}

		}
		virtual property float RingWidth
		{
			float get()
			{
				return ringWidth;
			}

			void set(float value)
			{
				ringWidth = value;
			}

		}
		virtual property float BaseAmount
		{
			float get()
			{
				return baseAmount;
			}

			void set(float value)
			{
				baseAmount = value;
			}

		}
		virtual property float RingAmount
		{
			float get()
			{
				return ringAmount;
			}

			void set(float value)
			{
				ringAmount = value;
			}

		}
		virtual property float RayAmount
		{
			float get()
			{
				return rayAmount;
			}

			void set(float value)
			{
				rayAmount = value;
			}

		}
		virtual property int Radius
		{
			int get()
			{
				return radius;
			}

			void set(int value)
			{
				radius = value;
				sigma = (float)(value) / 3;
			}

		}
#endif NOTUSED

	private:
		int rays;
		int radius;
		float baseAmount;
		float ringAmount;
		float rayAmount;
		int color;
		int width, height;
		int centreX, centreY;
		float ringWidth;
		Noise noise;
	private:
		float linear;
		float gauss;
		float mix;
		float falloff;
		float sigma;

	public:
		FlareFilter()
		{
			rays = 50;
			baseAmount = 1.0f;
			ringAmount = 0.2f;
			rayAmount = 0.1f;
			color = ((int)(0xffffffff));
			ringWidth = 1.6f;
			linear = 0.03f;
			gauss = 0.006f;
			mix = 0.50f;
			falloff = 6.0f;
			radius = 25;
		}

		virtual void setDimensions(int newwidth, int newheight)
		{
			width = newwidth;
			height = newheight;
			//radius = (int)(Math.min(width/2, height/2) - ringWidth - falloff);
			centreX = width / 2;
			centreY = height / 2;
			PointFilter::setDimensions(width, height);
		}

	public:
		virtual int filterRGB(int x, int y, int rgb)
		{
			float dx = (float)x - centreX;
			float dy = (float)y - centreY;
			float distance = (float)(sqrt(dx * dx + dy * dy));
			float a = (float)(exp((- distance) * distance * gauss)) * mix + (float)(exp((- distance) * linear)) * (1 - mix);
			float ring;

			a *= baseAmount;

			if (distance > radius + ringWidth)
				a = lerp((distance - (radius + ringWidth)) / falloff, a, 0.0);

			if (distance < radius - ringWidth || distance > radius + ringWidth)
				ring = 0;
			else
			{
				ring = abs(distance - radius) / ringWidth;
				ring = 1 - ring * ring * (3 - 2 * ring);
				ring *= ringAmount;
			}

			a += ring;

			// Creates random lines out from the center.
			// angle/PI*17 controls the number of lines.
			// angle*10 sets the noise frequency
			// *2 is the noise modulation

			float angle = (float)(atan2(dx, dy)) + PI;
			angle = (mod(angle / PI * 17 + 1.0f + noise.noise1(angle * 10), 1.0f) - 0.5f) * 2;
			angle = abs(angle);
			angle = (float)(pow((float)angle, 5.0f));

			float b = rayAmount * angle / (1 + distance * 0.1f);
			a += b;
			//b = ImageMath.clamp(b, 0, 1);
			//rgb = PixelUtils.combinePixels(0xff802010, rgb, PixelUtils.NORMAL, (int)(b*255));

			a = clamp(a, 0.0, 1.0);
			return mixColors(a, rgb, color);
		}

		virtual CString *ToString()
		{
			return new CString("Stylize/Flare...");
		}
	};
}
