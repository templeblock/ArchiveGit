#pragma once

#include "PixelUtils.h"
#include "TransferFilter.h"

namespace ImageEffects
{
	class RescaleFilter : TransferFilter
	{
	public:
#ifdef NOTUSED
		virtual property float Scale
		{
			float get()
			{
				return scale;
			}

			void set(float value)
			{
				scale = value;
				initialized = false;
			}
		}
#endif NOTUSED

	private:


	private:
		float scale;

	public:
		RescaleFilter()
		{
			scale = 1.0f;
		}

		virtual int transferFunction(int v)
		{
			return PixelUtils::clamp((int)(v * scale));
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Colors/Rescale...");
		}
	};
}