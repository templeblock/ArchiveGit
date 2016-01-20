#pragma once
#include "WholeImageFilter.h"

namespace ImageEffects
{
	/// A class to emboss an image.

	class EmbossFilter : WholeImageFilter
	{
	private:
		void InitBlock()
		{
			azimuth = 135.0f * PI / 180.0f;
			elevation = 30.0f * PI / 180.0f;
		}
#ifdef NOTUSED
	public:
		virtual property float Azimuth
		{
			float get()
			{
				return azimuth;
			}

			void set(float value)
			{
				azimuth = value;
			}

		}
		virtual property float Elevation
		{
			float get()
			{
				return elevation;
			}

			void set(float value)
			{
				elevation = value;
			}

		}
		virtual property float BumpHeight
		{
			float get()
			{
				return width45 / 3;
			}

			void set(float value)
			{
				width45 = 3 * value;
			}

		}
		virtual property bool Emboss
		{
			bool get()
			{
				return emboss;
			}

			void set(bool value)
			{
				emboss = value;
			}

		}
#endif NOTUSED

	private:
		float azimuth, elevation;
		bool emboss;
		float width45;

	public:
		EmbossFilter()
		{
			width45 = 3.0f;
			InitBlock();
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			int index = 0;
			Array<int> *outPixels = new Array<int>(width * height);

			Array<int> *bumpPixels;
			int bumpMapWidth, bumpMapHeight;

			bumpMapWidth = width;
			bumpMapHeight = height;
			bumpPixels = new Array<int>(bumpMapWidth * bumpMapHeight);
			for (int i = 0; i < inPixels->get_Length(); i++)
				bumpPixels->SetValue(PixelUtils::brightness(inPixels->GetAt(i)), i);

			int Nx, Ny, Nz, Lx, Ly, Lz, Nz2, NzLz, NdotL;
			int shade, background;

			float pixelScale = 255.9f;
			Lx = (int)(cos(azimuth) * cos(elevation) * pixelScale);
			Ly = (int)(sin(azimuth) * cos(elevation) * pixelScale);
			Lz = (int)(sin(elevation) * pixelScale);

			Nz = (int)(6 * 255 / width45);
			Nz2 = Nz * Nz;
			NzLz = Nz * Lz;

			background = Lz;

			int bumpIndex = 0;

			for (int y = 0; y < height; y++, bumpIndex += bumpMapWidth)
			{
				int s1 = bumpIndex;
				int s2 = s1 + bumpMapWidth;
				int s3 = s2 + bumpMapWidth;

				for (int x = 0; x < width; x++, s1++, s2++, s3++)
				{
					if (y != 0 && y < height - 2 && x != 0 && x < width - 2)
					{
						Nx = bumpPixels->GetAt(s1 - 1) + bumpPixels->GetAt(s2 - 1) + bumpPixels->GetAt(s3 - 1) - bumpPixels->GetAt(s1 + 1) - bumpPixels->GetAt(s2 + 1) - bumpPixels->GetAt(s3 + 1);
						Ny = bumpPixels->GetAt(s3 - 1) + bumpPixels->GetAt(s3)     + bumpPixels->GetAt(s3 + 1) - bumpPixels->GetAt(s1 - 1) - bumpPixels->GetAt(s1)     - bumpPixels->GetAt(s1 + 1);

						if (Nx == 0 && Ny == 0)
							shade = background;
						else if ((NdotL = Nx * Lx + Ny * Ly + NzLz) < 0)
							shade = 0;
						else
						{
							shade = (int)(NdotL / sqrt((float)Nx * Nx + Ny * Ny + Nz2));
						}
					}
					else
						shade = background;

					if (emboss)
					{
						int rgb = inPixels->GetAt(index);
						int a = rgb & ((int)(0xff000000));
						int r = (rgb >> 16) & 0xff;
						int g = (rgb >> 8) & 0xff;
						int b = rgb & 0xff;
						r = (r * shade) >> 8;
						g = (g * shade) >> 8;
						b = (b * shade) >> 8;
						outPixels->SetValue(a | (r << 16) | (g << 8) | b, index++);
					}
					else
						outPixels->SetValue(((int)(0xff000000)) | (shade << 16) | (shade << 8) | shade, index++);
				}
			}

			return outPixels;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Stylize/Emboss...");
		}
	};
}
