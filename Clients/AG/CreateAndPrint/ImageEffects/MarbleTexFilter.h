#pragma once
#include "Noise.h"
#include "PointFilter.h"

namespace ImageEffects
{
	class MarbleTexFilter : PointFilter
	{
#ifdef NOTUSED
	public:
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
		virtual property float TurbulenceFactor
		{
			float get()
			{
				return turbulenceFactor;
			}

			void set(float value)
			{
				turbulenceFactor = value;
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

	private:
		float scale;
		float stretch;
		float angle;
		float turbulence;
		float turbulenceFactor;
		float m00;
		float m01;
		float m10;
		float m11;
		Colormap *colormap;
		Noise noise;

	public:
		MarbleTexFilter()
		{
			scale = 32;
			stretch = 1.0f;
			angle = 0.0f;
			turbulence = 1;
			turbulenceFactor = 0.5f;
			m00 = 1.0f;
			m01 = 0.0f;
			m10 = 0.0f;
			m11 = 1.0f;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			float nx = m00 * x + m01 * y;
			float ny = m10 * x + m11 * y;
			nx /= scale * stretch;
			ny /= scale;

			int a = rgb & ((int)(0xff000000));
			if (colormap != NULL)
			{
				float chaos = turbulenceFactor * noise.turbulence2(nx, ny, turbulence);
				float f = 3 * turbulenceFactor * chaos + ny;
				f = (float)(sin(f * PI));
				float perturb = (float)(sin(40.0 * chaos));
				f = (float)(f +.2 * perturb);
				return colormap->getColor(f);
			}
			else
			{
				float red, grn, blu;
				float chaos, brownLayer, greenLayer;
				float perturb, brownPerturb, greenPerturb, grnPerturb;
				float t;

				chaos = turbulenceFactor * noise.turbulence2(nx, ny, turbulence);
				t = (float)(sin(sin(8.0 * chaos + 7 * nx + 3.0 * ny)));

				greenLayer = brownLayer = abs(t);

				perturb = (float)(sin(40.0 * chaos));
				perturb = (float)(abs(perturb));

				brownPerturb =.6f * perturb + 0.3f;
				greenPerturb =.2f * perturb + 0.8f;
				grnPerturb =.15f * perturb + 0.85f;
				grn = 0.5f * (float)pow(abs(brownLayer), 0.3f);
				brownLayer = (float)pow(0.5f * (brownLayer + 1.0f), 0.6f) * brownPerturb;
				greenLayer = (float)pow(0.5f * (greenLayer + 1.0f), 0.6f) * greenPerturb;

				red = (0.5f * brownLayer + 0.35f * greenLayer) * 2.0f * grn;
				blu = (0.25f * brownLayer + 0.35f * greenLayer) * 2.0f * grn;
				grn *= max(brownLayer, greenLayer) * grnPerturb;
				int r = (rgb >> 16) & 0xff;
				int g = (rgb >> 8) & 0xff;
				int b = rgb & 0xff;
				r = PixelUtils::clamp((int)(r * red));
				g = PixelUtils::clamp((int)(g * grn));
				b = PixelUtils::clamp((int)(b * blu));
				return (rgb & ((int)(0xff000000))) | (r << 16) | (g << 8) | b;
			}
		}

		virtual CString *ToString()
		{
			return new CString("Texture/Marble Texture...");
		}
	};
}
