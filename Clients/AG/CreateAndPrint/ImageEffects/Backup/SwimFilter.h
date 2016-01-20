#pragma once
#include "Noise.h"
#include "TransformFilter.h"

namespace ImageEffects
{
	class SwimFilter : TransformFilter
	{
#ifdef NOTUSED
	public:
		virtual property float Amount
		{
			float get()
			{
				return amount;
			}

			void set(float value)
			{
				amount = value;
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
		virtual property float Time
		{
			float get()
			{
				return time;
			}

			void set(float value)
			{
				time = value;
			}

		}
#endif NOTUSED

	private:
		float scale;
		float stretch;
		float angle;
		float amount;
		float turbulence;
		float time;
		float m00;
		float m01;
		float m10;
		float m11;
		Noise noise;

	public:
		SwimFilter()
		{
			scale = 32;
			stretch = 1.0f;
			angle = 0.0f;
			amount = 1.0f;
			turbulence = 1.0f;
			time = 0.0f;
			m00 = 1.0f;
			m01 = 0.0f;
			m10 = 0.0f;
			m11 = 1.0f;
		}

	public:
		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			float nx = m00 * x + m01 * y;
			float ny = m10 * x + m11 * y;
			nx /= scale;
			ny /= scale * stretch;

			if (turbulence == 1.0f)
			{
				out_Renamed->SetValue(x + amount * noise.noise3(nx + 0.5f, ny, time), 0);
				out_Renamed->SetValue(y + amount * noise.noise3(nx, ny + 0.5f, time), 1);
			}
			else
			{
				out_Renamed->SetValue(x + amount * noise.turbulence3(nx + 0.5f, ny, turbulence, time), 0);
				out_Renamed->SetValue(y + amount * noise.turbulence3(nx, ny + 0.5f, turbulence, time), 1);
			}
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Swim...");
		}
	};
}
