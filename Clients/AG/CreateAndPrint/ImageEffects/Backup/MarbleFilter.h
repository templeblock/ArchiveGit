#pragma once
#include "Noise.h"

namespace ImageEffects
{
	/// This filter applies a marbling effect to an image, displacing pixels by random amounts.
	class MarbleFilter : TransformFilter
	{
#ifdef NOTUSED
	public:
		virtual property float XScale
		{
			float get()
			{
				return xScale;
			}

			void set(float value)
			{
				xScale = value;
			}

		}
		virtual property float YScale
		{
			float get()
			{
				return yScale;
			}

			void set(float value)
			{
				yScale = value;
			}

		}
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
#endif NOTUSED

	private:


	public:
		float sinTable[256];
		float cosTable[256];
		float xScale ;
		float yScale;
		float amount;
		float turbulence;
		Noise noise;

		MarbleFilter()
		{
			xScale = 4;
			yScale = 4;
			amount = 1;
			turbulence = 1;
//j			EdgeAction = CLAMP;
		}

	private:
		void initialize()
		{
			for (int i = 0; i < 256; i++)
			{
				float angle = (TWO_PI * i) / 256.0f * turbulence;
				sinTable[i] = (float)((- yScale) * sin(angle));
				cosTable[i] = (float)(yScale * cos(angle));
			}
		}

		int displacementMap(int x, int y)
		{
			return PixelUtils::clamp((int)(127 * (1 + noise.noise2(x / xScale, y / xScale))));
		}

	public:
		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			int displacement = displacementMap(x, y);
			out_Renamed->SetValue(x + sinTable[displacement], 0);
			out_Renamed->SetValue(y + cosTable[displacement], 1);
		}

		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			initialize();
			return __super::filterPixels(width, height, inPixels, transformedSpace);
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Marble...");
		}
	};
}