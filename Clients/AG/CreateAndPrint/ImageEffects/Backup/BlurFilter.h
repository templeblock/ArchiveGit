#pragma once

//


namespace ImageEffects
{

	class BlurFilter //j: ConvolveFilter
	{
	public:
		float *blurMatrix;

	public:
		BlurFilter() //j: ConvolveFilter(blurMatrix)
		{
			static const float cblurMatrix[] = {1/14, 2/14, 1/14, 2/14, 2/14, 2/14, 1/14, 2/14, 1/14};
			blurMatrix = (float*)cblurMatrix;
		}

		virtual CString *ToString()
		{
			return new CString("Blur/Simple Blur");
		}
	};
}
