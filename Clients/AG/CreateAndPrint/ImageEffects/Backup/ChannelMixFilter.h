#pragma once

#include "PixelUtils.h"
#include "PointFilter.h"

namespace ImageEffects
{
	class ChannelMixFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property int BlueGreen
		{
			int get()
			{
				return blueGreen;
			}

			void set(int value)
			{
				blueGreen = value;
			}
		}
		virtual property int RedBlue
		{
			int get()
			{
				return redBlue;
			}

			void set(int value)
			{
				redBlue = value;
			}
		}
		virtual property int GreenRed
		{
			int get()
			{
				return greenRed;
			}

			void set(int value)
			{
				greenRed = value;
			}
		}
		virtual property int IntoR
		{
			int get()
			{
				return intoR;
			}

			void set(int value)
			{
				intoR = value;
			}
		}
		virtual property int IntoG
		{
			int get()
			{
				return intoG;
			}

			void set(int value)
			{
				intoG = value;
			}
		}
		virtual property int IntoB
		{
			int get()
			{
				return intoB;
			}

			void set(int value)
			{
				intoB = value;
			}
		}
#endif NOTUSED

	private:


	public:
		int blueGreen, redBlue, greenRed;
		int intoR, intoG, intoB;

		ChannelMixFilter()
		{
			canFilterIndexColorModel = true;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			int a = rgb & ((int)(0xff000000));
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			int nr = PixelUtils::clamp((intoR * (blueGreen * g + (255 - blueGreen) * b) / 255 + (255 - intoR) * r) / 255);
			int ng = PixelUtils::clamp((intoG * (redBlue * b + (255 - redBlue) * r) / 255 + (255 - intoG) * g) / 255);
			int nb = PixelUtils::clamp((intoB * (greenRed * r + (255 - greenRed) * g) / 255 + (255 - intoB) * b) / 255);
			return a | (nr << 16) | (ng << 8) | nb;
		}

		virtual CString *ToString()
		{
			return new CString("Colors/Mix Channels...");
		}
	};
}