#pragma once
#include "TransformFilter.h"

namespace ImageEffects
{
	/// A Filter to pixellate images.
	class BlockFilter : TransformFilter
	{
		/// Get the pixel block size
		/// <returns> the number of pixels along each block edge</returns>
		/// Set the pixel block size
		/// <param name="blockSize">the number of pixels along each block edge</param>
#ifdef NOTUSED
	public:
		virtual property int BlockSize
		{
			int get()
			{
				return blockSize;
			}

			void set(int value)
			{
				blockSize = value;
			}
		}
#endif NOTUSED

	private:


	private:
		int blockSize;

	public:
		BlockFilter()
		{
			blockSize = 2;
		}

	public:
		virtual void transform(int x, int y, POINT out_Renamed)
		{
			out_Renamed.x = (x / blockSize) * blockSize;
			out_Renamed.y = (y / blockSize) * blockSize;
		}

		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			out_Renamed->SetValue(((float)x / blockSize) * blockSize, 0);
			out_Renamed->SetValue(((float)y / blockSize) * blockSize, 1);
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Stylize/Mosaic...");
		}
	};
}