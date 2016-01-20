#pragma once

namespace ImageEffects
{
	class ThresholdFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property int LowerThreshold
		{
			int get()
			{
				return lowerThreshold;
			}

			void set(int value)
			{
				lowerThreshold = value;
				lowerThreshold3 = value * 3;
			}

		}
		virtual property int UpperThreshold
		{
			int get()
			{
				return upperThreshold;
			}

			void set(int value)
			{
				upperThreshold = value;
				upperThreshold3 = value * 3;
			}

		}
		virtual property int White
		{
			int get()
			{
				return white;
			}

			void set(int value)
			{
				white = value;
			}

		}
		virtual property int Black
		{
			int get()
			{
				return black;
			}

			void set(int value)
			{
				black = value;
			}

		}
#endif NOTUSED

	private:
		int lowerThreshold;
		int lowerThreshold3;
		int upperThreshold;
		int upperThreshold3;
		int white;
		int black;

	public:
		ThresholdFilter()
		{
			lowerThreshold = 127;
			lowerThreshold3 = 127;
			upperThreshold = 127;
			upperThreshold3 = 127;
			white = 0xffffff;
			black = 0x000000;
		}

		ThresholdFilter(int t)
		{
			lowerThreshold = t;
			upperThreshold = t;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			int a = rgb & ((int)(0xff000000));
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			int l = r + g + b;
			if (l < lowerThreshold3)
				return a | black;
			else if (l > upperThreshold3)
				return a | white;
			return rgb;
		}

		virtual CString *ToString()
		{
			return new CString("Stylize/Threshold...");
		}
	};
}
