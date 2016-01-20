#pragma once

#include "TransferFilter.h"

namespace ImageEffects
{

	/// A filter which solarizes an image.

	class SolarizeFilter : TransferFilter
	{

	private:


	public:
		virtual int transferFunction(int v)
		{
			return v > 127?2 * (v - 128):2 * (127 - v);
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Colors/Solarize");
		}
	};
}