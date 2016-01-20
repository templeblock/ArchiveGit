#pragma once

namespace ImageEffects
{
	class RotateFilter : TransformFilter
	{
#ifdef NOTUSED
	public:
		virtual property float Angle
		{
			float get()
			{
				return angle;
			}

			void set(float value)
			{
				angle = value;
				cos = (float)(cos(angle));
				sin = (float)(sin(angle));
			}

		}
#endif NOTUSED

	private:


	private:
		float angle;
		float cos, sin;
		bool resize;

	public:
		RotateFilter()
		{
			angle = PI;
			resize = true;
		}

		RotateFilter(float newangle)
		{
			angle = newangle;
			resize = true;
		}

		RotateFilter(float newangle, bool newresize)
		{
			angle = newangle;
			resize = newresize;
		}

	public:
		virtual void transformSpace(CRect& rect)
		{
			if (resize)
			{
				CPoint out_Renamed = CPoint(0, 0);
				int minx = MAXLONG;
				int miny = MAXLONG;
				int maxx = MINLONG;
				int maxy = MINLONG;
				int w = rect.Width();
				int h = rect.Height();
				int x = rect.left;
				int y = rect.top;

				for (int i = 0; i < 4; i++)
				{
					switch (i)
					{

						case 0:
							transform(x, y, out_Renamed);
							break;

						case 1:
							transform(x + w, y, out_Renamed);
							break;

						case 2:
							transform(x, y + h, out_Renamed);
							break;

						case 3:
							transform(x + w, y + h, out_Renamed);
							break;
						}
					minx = min(minx, out_Renamed.x);
					miny = min(miny, out_Renamed.y);
					maxx = max(maxx, out_Renamed.x);
					maxy = max(maxy, out_Renamed.y);
				}

				rect.left = minx;
				rect.top = miny;
				rect.right = minx + maxx - rect.left;
				rect.bottom = miny + maxy - rect.top;
			}
		}

		virtual void transform(int x, int y, CPoint& out_Renamed)
		{
			out_Renamed.x = (int)((x * cos) + (y * sin));
			out_Renamed.y = (int)((y * cos) - (x * sin));
		}

		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			out_Renamed->SetValue((cos * x) - (sin * y), 0);
			out_Renamed->SetValue((cos * y) + (sin * x), 1);
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Rotate");
		}
	};
}
