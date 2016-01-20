#pragma once
#include "TransformFilter.h"

namespace ImageEffects
{
	class SphereFilter : TransformFilter
	{
#ifdef NOTUSED
	public:
		virtual property float RefractionIndex
		{
			float get()
			{
				return refractionIndex;
			}

			void set(float value)
			{
				refractionIndex = value;
			}

		}
		virtual property float Radius
		{
			float get()
			{
				return a;
			}

			void set(float value)
			{
				a = value;
				b = value;
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
#endif NOTUSED

	private:


	private:
		float a;
		float b;
		float a2;
		float b2;
		float centreX;
		float centreY;
		float refractionIndex;
		float icentreX;
		float icentreY;

	public:
		SphereFilter()
		{
			centreX = 0.5f;
			centreY = 0.5f;
			refractionIndex = 1.5f;
			edgeAction = CLAMP;
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			icentreX = width * centreX;
			icentreY = height * centreY;
			if (a == 0)
				a = (float)width / 2;
			if (b == 0)
				b = (float)height / 2;
			a2 = a * a;
			b2 = b * b;
			return __super::filterPixels(width, height, inPixels, transformedSpace);
		}

		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			float dx = x - icentreX;
			float dy = y - icentreY;
			float x2 = dx * dx;
			float y2 = dy * dy;
			if (y2 >= (b2 - (b2 * x2) / a2))
			{
				out_Renamed->SetValue((float)x, 0);
				out_Renamed->SetValue((float)y, 1);
			}
			else
			{
				float rRefraction = 1.0f / refractionIndex;

				float z = (float)(sqrt((1.0f - x2 / a2 - y2 / b2) * (a * b)));
				float z2 = z * z;

				float xAngle = (float)(acos(dx / sqrt(x2 + z2)));
				float angle1 = HALF_PI - xAngle;
				float angle2 = (float)(asin(sin(angle1) * rRefraction));
				angle2 = HALF_PI - xAngle - angle2;
				out_Renamed->SetValue(x - (float)(tan(angle2)) * z, 0);

				float yAngle = (float)(acos(dy / sqrt(y2 + z2)));
				angle1 = HALF_PI - yAngle;
				angle2 = (float)(asin(sin(angle1) * rRefraction));
				angle2 = HALF_PI - yAngle - angle2;
				out_Renamed->SetValue(y - (float)(tan(angle2)) * z, 1);
			}
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Sphere...");
		}
	};
}
