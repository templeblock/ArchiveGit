#pragma once

namespace ImageEffects
{

	class AverageFilter //j: ConvolveFilter
	{

	public:
		float *theMatrix;

	public:
		AverageFilter() //j: ConvolveFilter(theMatrix)
		{
			static const float ctheMatrix[] = {0.1f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.1f};
			theMatrix = (float*)ctheMatrix;
		}

		virtual CString *ToString()
		{
			return new CString("Blur/Average Blur");
		}
	};
}
