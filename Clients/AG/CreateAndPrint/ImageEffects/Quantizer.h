#pragma once

namespace ImageEffects
{
	/// The interface for an image quantizer. The addColor method is called (repeatedly
	/// if necessary) with all the image pixels. A color table can then be returned by 
	/// calling the buildColorTable method.
	/*public interface*/ class Quantizer
	{
	public:
		virtual void setup(int) = 0;
		virtual void addPixels(Array<int>*, int, int) = 0;
		virtual Array<int>* buildColorTable() = 0;
		virtual int getIndexForColor(int) = 0;
	};
}
