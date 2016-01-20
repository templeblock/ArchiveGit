#pragma once

//


namespace ImageEffects
{

	/*public interface*/ class TransferFunction
	{
		virtual int evaluate(int) = 0;
	};
}