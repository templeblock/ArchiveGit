#pragma once

namespace ImageEffects
{
	class WaterFilter : TransformFilter
	{
#ifdef NOTUSED
	public:
		virtual property float Wavelength
		{
			float get()
			{
				return wavelength;
			}

			void set(float value)
			{
				wavelength = value;
			}

		}
		virtual property float Amplitude
		{
			float get()
			{
				return amplitude;
			}

			void set(float value)
			{
				amplitude = value;
			}

		}
		virtual property float Phase
		{
			float get()
			{
				return phase;
			}

			void set(float value)
			{
				phase = value;
			}

		}
		virtual property float CentreX
		{
			float get()
			{
				return centreX;
			}

			void set(float value)
			{
				centreX = value;
			}

		}
		virtual property float CentreY
		{
			float get()
			{
				return centreY;
			}

			void set(float value)
			{
				centreY = value;
			}

		}
		virtual property CPointF Centre
		{
			CPointF get()
			{
				return CPointF(centreX, centreY);
			}

			void set(CPointF value)
			{
				centreX = (float)(value::X);
				centreY = (float)(value::Y);
			}

		}
		virtual property float Radius
		{
			float get()
			{
				return radius;
			}

			void set(float value)
			{
				radius = value;
			}

		}
#endif NOTUSED

	private:
		float wavelength;
		float amplitude;
		float phase;
		float centreX;
		float centreY;
		float radius;

		float radius2;
		float icentreX;
		float icentreY;

	public:
		WaterFilter()
		{
			wavelength = 16;
			amplitude = 10;
			phase;
			centreX = 0.5f;
			centreY = 0.5f;
			edgeAction = CLAMP;
		}

	private:
		bool inside(int v, int a, int b)
		{
			return a <= v && v <= b;
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			icentreX = width * centreX;
			icentreY = height * centreY;
			if (radius == 0)
				radius = min(icentreX, icentreY);
			radius2 = radius * radius;
			return __super::filterPixels(width, height, inPixels, transformedSpace);
		}

		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			float dx = x - icentreX;
			float dy = y - icentreY;
			float distance2 = dx * dx + dy * dy;
			if (distance2 > radius2)
			{
				out_Renamed->SetValue((float)x, 0);
				out_Renamed->SetValue((float)y, 1);
			}
			else
			{
				float distance = (float)(sqrt(distance2));
				float amount = amplitude * (float)(sin(distance / wavelength * TWO_PI - phase));
				amount *= (radius - distance) / radius;
				if (distance != 0)
					amount *= wavelength / distance;
				out_Renamed->SetValue((float)x + dx * amount, 0);
				out_Renamed->SetValue((float)y + dy * amount, 1);
			}
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Water Ripples...");
		}
	};
}
