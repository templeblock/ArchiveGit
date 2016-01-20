#pragma once

namespace ImageEffects
{
	class ShearFilter : TransformFilter
	{
#ifdef NOTUSED
	public:
		virtual property bool Resize
		{
			bool get()
			{
				return resize;
			}

			void set(bool value)
			{
				resize = value;
			}

		}
		virtual property float XAngle
		{
			float get()
			{
				return xangle;
			}

			void set(float value)
			{
				xangle = value;
				initialize();
			}

		}
		virtual property float YAngle
		{
			float get()
			{
				return yangle;
			}

			void set(float value)
			{
				yangle = value;
				initialize();
			}

		}
#endif NOTUSED

	private:
		float xangle;
		float yangle;
		float shx;
		float shy;
		float xoffset;
		float yoffset;
		bool resize;

	public:
		ShearFilter()
		{
			xangle = 0.0f;
			yangle = 0.0f;
			shx = 0.0f;
			shy = 0.0f;
			xoffset = 0.0f;
			yoffset = 0.0f;
			resize = true;
		}

	private:
		void initialize()
		{
			shx = (float)(sin(xangle));
			shy = (float)(sin(yangle));
		}

	public:
		virtual void transformSpace(CRect& r)
		{
			float tangent = (float)(tan(xangle));
			xoffset = (-r.Height()) * tangent;
			if (tangent < 0.0)
				tangent = - tangent;
			r.right = r.left + (int)(r.Height() * tangent + r.Width() + 0.999999f);
			tangent = (float)(tan(yangle));
			yoffset = (- r.Width()) * tangent;
			if (tangent < 0.0)
				tangent = - tangent;
			r.bottom = r.top + (int)(r.Width() * tangent + r.Height() + 0.999999f);
		}

		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			out_Renamed->SetValue(x + xoffset + (y * shx), 0);
			out_Renamed->SetValue(y + yoffset + (x * shy), 1);
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Shear...");
		}
	};
}
