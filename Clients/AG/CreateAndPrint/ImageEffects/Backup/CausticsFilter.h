#pragma once
#include "WholeImageFilter.h"
#include "Noise.h"

namespace ImageEffects
{
	class CausticsFilter : WholeImageFilter
	{
#ifdef NOTUSED
	public:
		virtual property float Scale
		{
			float get()
			{
				return scale;
			}

			void set(float value)
			{
				scale = value;
			}

		}
		virtual property int Brightness
		{
			int get()
			{
				return brightness;
			}

			void set(int value)
			{
				brightness = value;
			}

		}
		virtual property float Turbulence
		{
			float get()
			{
				return turbulence;
			}

			void set(float value)
			{
				turbulence = value;
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
		virtual property float Dispersion
		{
			float get()
			{
				return dispersion;
			}

			void set(float value)
			{
				dispersion = value;
			}

		}
		virtual property float Time
		{
			float get()
			{
				return time;
			}

			void set(float value)
			{
				time = value;
			}

		}
		virtual property int Samples
		{
			int get()
			{
				return samples;
			}

			void set(int value)
			{
				samples = value;
			}

		}
		virtual property int BgColor
		{
			int get()
			{
				return bgColor;
			}

			void set(int value)
			{
				bgColor = value;
			}

		}
#endif NOTUSED

	public:
		int brightness;
		float amount;
		float turbulence;
		float dispersion;
		float time;
	private:
		float scale;
		float angle;
		int samples;
		int bgColor;
		float s, c;
		Noise noise;

	public:
		CausticsFilter()
		{
			brightness = 10;
			amount = 1.0f;
			turbulence = 1.0f;
			dispersion = 0.0f;
			time = 0.0f;
			scale = 32;
			angle = 0.0f;
			samples = 2;
			bgColor = ((int)(0xff799fff));
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			srand(567);

			s = (float)(sin(0.1));
			c = (float)(cos(0.1));

			int srcWidth = originalSpace.Width();
			int srcHeight = originalSpace.Height();
			int outWidth = transformedSpace.Width();
			int outHeight = transformedSpace.Height();
			int index = 0;
			Array<int> *pixels = new Array<int>(outWidth * outHeight);

			for (int y = 0; y < outHeight; y++)
			{
				for (int x = 0; x < outWidth; x++)
				{
					pixels[index++] = bgColor;
				}
			}

			int v = brightness / samples;
			if (v == 0)
				v = 1;

			float rs = 1.0f / scale;
			float d = 0.95f;
			index = 0;
			for (int y = 0; y < outHeight; y++)
			{
				for (int x = 0; x < outWidth; x++)
				{
					for (int s = 0; s < samples; s++)
					{
						float sx = x + (float)(rand/*NextDouble*/());
						float sy = y + (float)(rand/*NextDouble*/());
						float nx = sx * rs;
						float ny = sy * rs;
						float xDisplacement, yDisplacement;
						float focus = 0.1f + amount;
						xDisplacement = evaluate(nx - d, ny) - evaluate(nx + d, ny);
						yDisplacement = evaluate(nx, ny + d) - evaluate(nx, ny - d);

						if (dispersion > 0)
						{
							for (int c = 0; c < 3; c++)
							{
								float ca = (1 + c * dispersion);
								float srcX = sx + scale * focus * xDisplacement * ca;
								float srcY = sy + scale * focus * yDisplacement * ca;

								if (srcX < 0 || srcX >= outWidth - 1 || srcY < 0 || srcY >= outHeight - 1)
								{
								}
								else
								{
									int i = ((int)(srcY)) * outWidth + (int)(srcX);
									int rgb = pixels[i];
									int r = (rgb >> 16) & 0xff;
									int g = (rgb >> 8) & 0xff;
									int b = rgb & 0xff;
									if (c == 2)
										r += v;
									else if (c == 1)
										g += v;
									else
										b += v;
									if (r > 255)
										r = 255;
									if (g > 255)
										g = 255;
									if (b > 255)
										b = 255;
									pixels[i] = ((int)(0xff000000)) | (r << 16) | (g << 8) | b;
								}
							}
						}
						else
						{
							float srcX = sx + scale * focus * xDisplacement;
							float srcY = sy + scale * focus * yDisplacement;

							if (srcX < 0 || srcX >= outWidth - 1 || srcY < 0 || srcY >= outHeight - 1)
							{
							}
							else
							{
								int i = ((int)(srcY)) * outWidth + (int)(srcX);
								int rgb = pixels[i];
								int r = (rgb >> 16) & 0xff;
								int g = (rgb >> 8) & 0xff;
								int b = rgb & 0xff;
								r += v;
								g += v;
								b += v;
								if (r > 255)
									r = 255;
								if (g > 255)
									g = 255;
								if (b > 255)
									b = 255;
								pixels[i] = ((int)(0xff000000)) | (r << 16) | (g << 8) | b;
							}
						}
					}
				}
			}
			return pixels;
		}

	private:
		static int add(int rgb, float brightness)
		{
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			r = (int)(r + brightness);
			g = (int)(g + brightness);
			b = (int)(b + brightness);
			if (r > 255)
				r = 255;
			if (g > 255)
				g = 255;
			if (b > 255)
				b = 255;
			return ((int)(0xff000000)) | (r << 16) | (g << 8) | b;
		}

		static int add(int rgb, float brightness, int c)
		{
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			if (c == 2)
				r = (int)(r + brightness);
			else if (c == 1)
				g = (int)(g + brightness);
			else
				b = (int)(b + brightness);
			if (r > 255)
				r = 255;
			if (g > 255)
				g = 255;
			if (b > 255)
				b = 255;
			return ((int)(0xff000000)) | (r << 16) | (g << 8) | b;
		}

	public:
		static float turbulence2(float x, float y, float time, float octaves)
		{
			float value_Renamed = 0.0f;
			float remainder;
			float lacunarity = 2.0f;
			float f = 1.0f;
			int i;

			// to prevent "cascading" effects
			x += 371;
			y += 529;

			for (i = 0; i < (int)(octaves); i++)
			{
				value_Renamed += noise.noise3(x, y, time) / f;
				x *= lacunarity;
				y *= lacunarity;
				f *= 2;
			}

			remainder = octaves - (int)(octaves);
			if (remainder != 0)
				value_Renamed += remainder * noise.noise3(x, y, time) / f;

			return value_Renamed;
		}

	public:
		virtual float evaluate(float x, float y)
		{
			float xt = s * x + c * time;
			float tt = c * x - c * time;
			float f = turbulence == 0.0?noise.noise3(xt, y, tt):turbulence2(xt, y, tt, turbulence);
			return f;
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Texture/Caustics...");
		}
	};
}
