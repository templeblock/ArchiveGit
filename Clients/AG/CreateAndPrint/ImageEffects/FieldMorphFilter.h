#pragma once
#include "WholeImageFilter.h"

namespace ImageEffects
{
	class Line
	{
	public:
		int x1, y1, x2, y2;
		int dx, dy;
		float length, lengthSquared;

	public:
		Line(int x1, int y1, int x2, int y2)
		{
			x1 = x1;
			y1 = y1;
			x2 = x2;
			y2 = y2;
			dx = x1 - x2;
			dy = y1 - y2;
			lengthSquared = (float)dx * dx + dy * dy;
			length = (float)(sqrt(lengthSquared));
		}
	};

	class FieldMorphFilter : WholeImageFilter
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
#endif NOTUSED

	private:
		float amount; // how far through morph

	public:
		FieldMorphFilter()
		{
			amount = 1.0f;
		}

	public:
		virtual void transform(int x, int y, CPoint& out_Renamed)
		{
		}

		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
		}

	private:
		void fieldMorph(Array<int> *in_Renamed, Array<int> *out_Renamed, int width, int height, Array<Line*> *inLines, Array<Line*> *outLines)
		{
			float u = 0, v = 0; // intermediate source coordinate indices 
			Array<Line*> *intermediateLines; // Array of intermediate control lines 
			float fraction = 0, distance; // fraction along and distance from control line 
			float fdist; // intermediate variable used to compute distance 
			float weight; // line weight 
			float a = 0.001f; // small number used to avoid divide by 0 
			float b = 2.0f; // exponent used in line weight calculation 
			float p = 0.75f; // exponent used in line weight calculation 
			int index = 0;

			// interpolate intermediate control lines 
			intermediateLines = new Array<Line*>(inLines->get_Length());
			for (int line = 0; line < inLines->get_Length(); line++)
			{
				Line *l = intermediateLines->GetAt(line) = new Line(lerp(amount, inLines->GetAt(line)->x1, outLines->GetAt(line)->x1), lerp(amount, inLines->GetAt(line)->y1, outLines->GetAt(line)->y1), lerp(amount, inLines->GetAt(line)->x2, outLines->GetAt(line)->x2), lerp(amount, inLines->GetAt(line)->y2, outLines->GetAt(line)->y2));
			}

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					float weight_sum = 0.0f;
					float sum_x = 0.0f;
					float sum_y = 0.0f;

					for (int line = 0; line < inLines->get_Length(); line++)
					{
						Line *l = intermediateLines->GetAt(line);
						int dx = x - l->x1;
						int dy = y - l->y1;

						fraction = (dx * l->dx + dy * l->dy) / l->lengthSquared;
						fdist = (dy * l->dx - dx * l->dy) / l->length;
						if (fraction <= 0)
						{
							distance = (float)(sqrt((float)dx * dx + dy * dy));
						}
						else if (fraction >= 1)
						{
							dx = x - l->x2;
							dy = y - l->y2;
							distance = (float)(sqrt((float)dx * dx + dy * dy));
						}
						else if (fdist >= 0)
							distance = fdist;
						else
							distance = - fdist;
						u = inLines->GetAt(line)->x1 + fraction * inLines->GetAt(line)->dx - fdist * inLines->GetAt(line)->dy / inLines->GetAt(line)->length;
						v = inLines->GetAt(line)->y1 + fraction * inLines->GetAt(line)->dy + fdist * inLines->GetAt(line)->dx / inLines->GetAt(line)->length;

						weight = (float)(pow(pow(l->length, p) / (a + distance), b));

						sum_x += (u - x) * weight;
						sum_y += (v - y) * weight;
						weight_sum += weight;
					}

					int srcX = (int)(clamp((float)x + sum_x / weight_sum + 0.5f, 0.0f, (float)width - 1));
					int srcY = (int)(clamp((float)y + sum_y / weight_sum + 0.5f, 0.0f, (float)height - 1));
					out_Renamed->SetValue(in_Renamed->GetAt(srcY * width + srcX), index++);
				}
			}
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			Array<int> *outPixels = new Array<int>(width * height);
			Array<Line*> *inLines = NULL; //j new Array<Line*>{new Line(10, 10, 10, 60), new Line(54, 10, 54, 60)};
			Array<Line*> *outLines = NULL; //j new Array<Line*>{new Line(21, 10, 20, 60), new Line(43, 10, 44, 60)};
			fieldMorph(inPixels, outPixels, width, height, inLines, outLines);
			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Field Morph...");
		}
	};
}
