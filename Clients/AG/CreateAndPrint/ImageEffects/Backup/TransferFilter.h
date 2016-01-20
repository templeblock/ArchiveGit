#pragma once

#include "PointFilter.h"

namespace ImageEffects
{


	class TransferFilter : PointFilter
	{

	public:
		Array<int> *rTable, *gTable, *bTable;
		bool initialized;

	public:
		TransferFilter()
		{
			canFilterIndexColorModel = true;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			if (!initialized)
			{
				initialize();
				initialized = true;
			}

			int a = rgb & ((int)(0xff000000));
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			r = rTable->GetAt(r);
			g = gTable->GetAt(g);
			b = bTable->GetAt(b);
			return a | (r << 16) | (g << 8) | b;
		}

		virtual void setDimensions(int width, int height)
		{
			initialized = false;
			PointFilter::setDimensions(width, height);
		}

	public:
		virtual void initialize()
		{
			rTable = gTable = bTable = makeTable();
		}

		virtual Array<int> *makeTable()
		{
			Array<int> *table = new Array<int>(256);
			for (int i = 0; i < 256; i++)
				table->SetValue(transferFunction(i), i);
			return table;
		}

		virtual int transferFunction(int v)
		{
			return 0;
		}
	};
}