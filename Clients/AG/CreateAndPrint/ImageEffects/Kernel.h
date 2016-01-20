#pragma once

namespace ImageEffects
{
	class Kernel
	{
	public:
		static const int factor= 1;
		int offset;
		int width;
		int height;
		int m11, m12, m13;
		int m21, m22, m23;
		int m31, m32, m33;
		Array<float> *kernelData;

		Kernel(int iWidth, int iHeight, Array<float> *fKernelData)
		{
			width = iWidth;
			height = iHeight;
			kernelData = fKernelData;
		}
		BITMAPINFOHEADER *filter()
		{
			return NULL;
		}
		int getWidth()
		{
			return height;
		}
		int getHeight()
		{
			return height;
		}
		Array<float> *getKernelData(Object *dummy)
		{
			return kernelData;
		}
	};
}