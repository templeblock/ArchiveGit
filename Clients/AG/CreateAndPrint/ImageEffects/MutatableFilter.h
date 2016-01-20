#pragma once

namespace ImageEffects
{
	/*public interface*/ class MutatableFilter
	{
		virtual void mutate(float, BITMAPINFOHEADER*, bool, bool) = 0;
	};
}