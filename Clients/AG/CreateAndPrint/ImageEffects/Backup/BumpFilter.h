#pragma once

namespace ImageEffects
{
	class BumpFilter //j: ConvolveFilter
	{
	public:
		float *embossMatrix;

	public:
		BumpFilter() //j: ConvolveFilter(embossMatrix)
		{
			static const float cembossMatrix[] = {-1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};
			embossMatrix = (float*)cembossMatrix;
		}

		virtual CString *ToString()
		{
			return new CString("Blur/Emboss Edges");
		}
	};
}
