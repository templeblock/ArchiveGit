#pragma once

namespace ImageEffects
{
	class NoiseFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property int Amount
		{
			int get()
			{
				return amount;
			}

			void set(int value)
			{
				amount = value;
			}

		}
		virtual property int Distribution
		{
			int get()
			{
				return distribution;
			}

			void set(int value)
			{
				distribution = value;
			}

		}
		virtual property bool Monochrome
		{
			bool get()
			{
				return monochrome;
			}

			void set(bool value)
			{
				monochrome = value;
			}

		}
#endif NOTUSED

	private:


	public:
		static const int GAUSSIAN = 0;
		static const int UNIFORM = 1;

	private:
		int amount;
		int distribution;
		bool monochrome;
		bool haveNextNextGaussian;
		double nextNextGaussian;

	public:
		NoiseFilter()
		{
			amount = 25;
			distribution = UNIFORM;
			nextNextGaussian = 0.0;
		}

	public:
		double nextGaussian()
		{
			if (haveNextNextGaussian)
			{
				haveNextNextGaussian = false;
				return nextNextGaussian;
			}

			double v1, v2, s;
			do
			{
				v1 = 2 * rand/*NextDouble*/() - 1; // between -1.0 and 1.0
				v2 = 2 * rand/*NextDouble*/() - 1; // between -1.0 and 1.0
				s = v1 * v1 + v2 * v2;
			} while (s >= 1 || s == 0);
			double multiplier = Math::Sqrt(-2 * Math::Log(s) / s);
			nextNextGaussian = v2 * multiplier;
			haveNextNextGaussian = true;
			return v1 * multiplier;
		}

	private:
		int random(int x)
		{
			x += (int)(((distribution == GAUSSIAN ? nextGaussian():2 * (float)(rand/*NextDouble*/() - 1))) * amount);
			if (x < 0)
				x = 0;
			else if (x > 0xff)
				x = 0xff;
			return x;
		}

	public:
		virtual int filterRGB(int x, int y, int rgb)
		{
			int a = rgb & ((int)(0xff000000));
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			if (monochrome)
			{
				int n = (int)(((distribution == GAUSSIAN ? nextGaussian():2 * (float)(rand/*NextDouble*/() - 1))) * amount);
				r = PixelUtils::clamp(r + n);
				g = PixelUtils::clamp(g + n);
				b = PixelUtils::clamp(b + n);
			}
			else
			{
				r = random(r);
				g = random(g);
				b = random(b);
			}
			return a | (r << 16) | (g << 8) | b;
		}

		virtual CString *ToString()
		{
			return new CString("Stylize/Add Noise...");
		}
	};
}
