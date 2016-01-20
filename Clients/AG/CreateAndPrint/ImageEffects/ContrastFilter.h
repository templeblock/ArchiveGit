#pragma once

#include "ImageMath.h"
#include "PixelUtils.h"
#include "TransferFilter.h"

namespace ImageEffects
{
	class ContrastFilter : TransferFilter
	{
#ifdef NOTUSED
	public:
		virtual property float Gain
		{
			float get()
			{
				return gain;
			}

			void set(float value)
			{
				gain = value;
				initialized = false;
			}

		}
		virtual property float Bias
		{
			float get()
			{
				return bias;
			}

			void set(float value)
			{
				bias = value;
				initialized = false;
			}
		}
#endif NOTUSED

	private:


	private:
		float cgain;
		float cbias;

	public:
		ContrastFilter()
		{
			cgain = 0.5f;
			cbias = 0.5f;
		}

		virtual int transferFunction(int v)
		{
			float f = (float)(v) / 255;
			f = gain(f, cgain);
			f = bias(f, cbias);
			return PixelUtils::clamp((int)(f * 255));
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Colors/Contrast...");
		}
	};
}
