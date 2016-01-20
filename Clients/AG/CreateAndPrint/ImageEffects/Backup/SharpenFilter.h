#pragma once

//


namespace ImageEffects
{

	class SharpenFilter //j: ConvolveFilter
	{

	private:


	public:
//j		static Array<float> *sharpenMatrix = new Array<float>{0.0f, - 0.2f, 0.0f, - 0.2f, 1.8f, - 0.2f, 0.0f, - 0.2f, 0.0f};
		static Array<float> *sharpenMatrix;

	public:
		SharpenFilter() //j: ConvolveFilter(sharpenMatrix)
		{
			sharpenMatrix = Array<float>::Build(0.0f, - 0.2f, 0.0f, - 0.2f, 1.8f, - 0.2f, 0.0f, - 0.2f, 0.0f);
		}

		virtual CString *ToString()
		{
			return new CString("Blur/Sharpen");
		}
	};
}