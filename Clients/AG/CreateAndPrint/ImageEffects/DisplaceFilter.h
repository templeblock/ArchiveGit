#pragma once

//



namespace ImageEffects
{


	class DisplaceFilter : TransformFilter
	{
	public:
		virtual property BITMAPINFOHEADER *DisplacementMap
		{
			BITMAPINFOHEADER *get()
			{
				return displacementMap;
			}

			void set(BITMAPINFOHEADER *value)
			{
				displacementMap = value;
			}

		}
		virtual property float Amount
		{
			float get()
			{
				return amount;
			}

			void set(float value)
			{
				amount = value;
			}

		}

	private:
		float amount = 1;
		BITMAPINFOHEADER *displacementMap;
		Array<int> *xmap, *ymap;
		int dw, dh;

	public:
		DisplaceFilter()
		{
		}

		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			int w = src->biWidth;
			int h = src->biHeight;

			BITMAPINFOHEADER *dm = displacementMap != NULL?displacementMap:src;

			dw = dm->Width;
			dh = dm->Height;

			Array<int> *mapPixels = new Array<int>(dw * dh);
			getRGB(dm, 0, 0, dw, dh, mapPixels);
			xmap = new Array<int>(dw * dh);
			ymap = new Array<int>(dw * dh);

			int i = 0;
			for (int y = 0; y < dh; y++)
			{
				for (int x = 0; x < dw; x++)
				{
					int rgb = mapPixels[i];
					int r = (rgb >> 16) & 0xff;
					int g = (rgb >> 8) & 0xff;
					int b = rgb & 0xff;
					mapPixels[i] = (r + g + b) / 8; // An arbitrary scaling factor which gives a good range for "amount"
					i++;
				}
			}

			i = 0;
			for (int y = 0; y < dh; y++)
			{
				int j1 = ((y + dh - 1)&  dh) * dw;
				int j2 = y * dw;
				int j3 = ((y + 1)&  dh) * dw;
				for (int x = 0; x < dw; x++)
				{
					int k1 = (x + dw - 1)&  dw;
					int k2 = x;
					int k3 = (x + 1)&  dw;
					xmap[i] = mapPixels[k1 + j1] + mapPixels[k1 + j2] + mapPixels[k1 + j3] - mapPixels[k3 + j1] - mapPixels[k3 + j2] - mapPixels[k3 + j3];
					ymap[i] = mapPixels[k1 + j3] + mapPixels[k2 + j3] + mapPixels[k3 + j3] - mapPixels[k1 + j1] - mapPixels[k2 + j1] - mapPixels[k3 + j1];
					i++;
				}
			}
			mapPixels = NULL;
			dst = __super::filter(src, dst);
			xmap = ymap = NULL;
			return dst;
		}

	public:
		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			float nx = x;
			float ny = y;
			int i = (y&  dh) * dw + x&  dw;
			out_Renamed[0] = x + amount * xmap[i];
			out_Renamed[1] = y + amount * ymap[i];
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Displace...");
		}
	};
}