#pragma once
//#include "CellularFilter.h"

namespace ImageEffects
{
	class CrystallizeFilter //j: CellularFilter
	{
#ifdef NOTUSED
	public:
		virtual property float EdgeThickness
		{
			float get()
			{
				return edgeThickness;
			}

			void set(float value)
			{
				edgeThickness = value;
			}

		}
		virtual property bool FadeEdges
		{
			bool get()
			{
				return fadeEdges;
			}

			void set(bool value)
			{
				fadeEdges = value;
			}

		}
		virtual property int EdgeColor
		{
			int get()
			{
				return edgeColor;
			}

			void set(int value)
			{
				edgeColor = value;
			}

		}
		virtual property int Mode
		{
			int get()
			{
				return mode;
			}

			void set(int value)
			{
				mode = value;
			}

		}
#endif NOTUSED

		static const int CRYSTALLIZE = 0;
		static const int POINTILLIZE = 1;

	private:
		float edgeThickness;
		bool fadeEdges;
		int edgeColor;
		int mode;

	public:
		CrystallizeFilter()
		{
			edgeThickness = 0.4f;
			fadeEdges;
			edgeColor = ((int)(0xff000000));
			mode = CRYSTALLIZE;
//j			Scale = 16;
//j			Randomness = 0.0f;
		}

		virtual int getPixel(int x, int y, Array<int> *inPixels, int width, int height)
		{
			int v = 0;
#ifdef NOTUSED
			float nx = m00 * x + m01 * y;
			float ny = m10 * x + m11 * y;
			nx /= scale;
			ny /= scale * stretch;
			nx += 1000;
			ny += 1000; // Reduce artifacts around 0,0
			float f = 0; //jevaluate(nx, ny);

			float f1 = results[0]->distance;
			float f2 = results[1]->distance;
			int srcx = clamp((int)((results[0]->x - 1000) * scale), 0, width - 1);
			int srcy = clamp((int)((results[0]->y - 1000) * scale), 0, height - 1);
			int v = inPixels[srcy * width + srcx];
			if (mode == POINTILLIZE)
			{
				f = f1;
				f = 1 - smoothStep(2 * edgeThickness, 2 * edgeThickness + 0.1f, f);
			}
			else
			{
				f = (f2 - f1) / edgeThickness;
				f = smoothStep(0, edgeThickness, f);
			}
			if (fadeEdges)
			{
				srcx = clamp((int)((results[1]->x - 1000) * scale), 0, width - 1);
				srcy = clamp((int)((results[1]->y - 1000) * scale), 0, height - 1);
				int v2 = inPixels[srcy * width + srcx];
				v2 = mixColors(0.5f, v2, v);
				v = mixColors(f, v2, v);
			}
			else
				v = mixColors(f, edgeColor, v);
#endif NOTUSED
			return v;
		}

		virtual CString *ToString()
		{
			return new CString("Stylize/Crystallize...");
		}
	};
}