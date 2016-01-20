#pragma once
#include "LinearColormap.h"

namespace ImageEffects
{
	class WoodFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property float Rings
		{
			float get()
			{
				return rings;
			}

			void set(float value)
			{
				rings = value;
			}

		}
		virtual property Function2D *Function
		{
			Function2D *get()
			{
				return function;
			}

			void set(Function2D *value)
			{
				function = value;
			}

		}
		virtual property float Scale
		{
			float get()
			{
				return scale;
			}

			void set(float value)
			{
				scale = value;
			}

		}
		virtual property float Stretch
		{
			float get()
			{
				return stretch;
			}

			void set(float value)
			{
				stretch = value;
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
				float cos = (float)(cos(value));
				float sin = (float)(sin(value));
				m00 = cos;
				m01 = sin;
				m10 = - sin;
				m11 = cos;
			}

		}
		virtual property float Turbulence
		{
			float get()
			{
				return turbulence;
			}

			void set(float value)
			{
				turbulence = value;
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
#endif NOTUSED

	public:
		float rings;
		float turbulence;
		float gain;
		float bias;
	private:
		float scale;
		float stretch;
		float angle;
		float m00;
		float m01;
		float m10;
		float m11;
		Colormap *colormap;
		Function2D *function;
		Noise noise;

	public:
		WoodFilter()
		{
			rings = 0.5f;
			turbulence = 1.0f;
			gain = 0.8f;
			bias = 0.1f;
			scale = 100;
			stretch = 1.0f;
			angle = 0.0f;
			m00 = 1.0f;
			m01 = 0.0f;
			m10 = 0.0f;
			m11 = 1.0f;
			colormap = (Colormap*)new LinearColormap(((int)(0xffefa900)), ((int)(0xff6d4e00)));
			function = NULL; //j new Noise();
		}

		int filterRGB(int x, int y, int rgb)
		{
			float nx = m00 * x + m01 * y;
			float ny = m10 * x + m11 * y;
			nx /= scale;
			ny /= scale * stretch;
			float f = turbulence == 1.0?noise.noise2(nx, ny):noise.turbulence2(nx, ny, turbulence);
			f = (f * 0.5f) + 0.5f;

			f *= rings * 50;
			f = f - (int)(f);
			f *= (1 - smoothStep(gain, 1.0f, f));

			float h = noise.noise2(nx * scale, ny * 50);
			f += bias * h;

			int a = rgb & ((int)(0xff000000));
			int v;
			if (colormap != NULL)
				v = colormap->getColor(f);
			else
			{
				v = PixelUtils::clamp((int)(f * 255));
				int r = v << 16;
				int g = v << 8;
				int b = v;
				v = a | r | g | b;
			}

			return v;
		}

		virtual CString *ToString()
		{
			return new CString("Texture/Wood...");
		}
	};
}
