#pragma once
#include "TransferFilter.h"

namespace ImageEffects
{
	class GammaFilter : TransferFilter
	{
	private:
		float rGamma, gGamma, bGamma;

	public:
		GammaFilter()
		{
			rGamma = gGamma = bGamma = 1.0;
		}

		GammaFilter(float gamma)
		{
			rGamma = gGamma = bGamma = gamma;
		}

		GammaFilter(float rGamma, float gGamma, float bGamma)
		{
			setGamma(rGamma, gGamma, bGamma);
		}

		virtual void setGamma(float rGamma, float gGamma, float bGamma)
		{
			rGamma = rGamma;
			gGamma = gGamma;
			bGamma = bGamma;
			initialized = false;
		}

		virtual void setGamma(float gamma)
		{
			setGamma(gamma, gamma, gamma);
		}

		virtual float getGamma()
		{
			return rGamma;
		}

	public:
		virtual void initialize()
		{
			rTable = makeTable(rGamma);

			if (gGamma == rGamma)
				gTable = rTable;
			else
				gTable = makeTable(gGamma);

			if (bGamma == rGamma)
				bTable = rTable;
			else if (bGamma == gGamma)
				bTable = gTable;
			else
				bTable = makeTable(bGamma);
		}

		virtual Array<int> *makeTable(float gamma)
		{
			Array<int> *table = new Array<int>(256);
			for (int i = 0; i < 256; i++)
			{
				table->SetValue(transferFunction(i), i);
				int v = (int)((255.0 * pow(i / 255.0, 1.0 / gamma)) + 0.5);
				if (v > 255)
					v = 255;
				table->SetValue(v, i);
			}
			return table;
		}


	public:
		virtual CString *ToString()
		{
			return new CString("Colors/Gamma...");
		}
	};
}
