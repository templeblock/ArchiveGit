#pragma once

namespace ImageEffects
{
	class SparkleFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property int Color
		{
			int get()
			{
				return color;
			}

			void set(int value)
			{
				color = value;
			}

		}
		virtual property int Randomness
		{
			int get()
			{
				return randomness;
			}

			void set(int value)
			{
				randomness = value;
			}

		}
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
		virtual property int Rays
		{
			int get()
			{
				return rays;
			}

			void set(int value)
			{
				rays = value;
			}

		}
		virtual property int Radius
		{
			int get()
			{
				return radius;
			}

			void set(int value)
			{
				radius = value;
			}

		}
#endif NOTUSED

	private:


	private:
		int rays;
		int radius;
		int amount;
		int color;
		int randomness;
		int width, height;
		int centreX, centreY;
		__int64 seed;
		float rayLengths[200];
		bool haveNextNextGaussian;
		double nextNextGaussian;

	public:
		SparkleFilter()
		{
			rays = 50;
			radius = 25;
			amount = 50;
			color = ((int)(0xffffffff));
			randomness = 25;
			width, height;
			centreX, centreY;
			seed = 371;
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

		virtual void setDimensions(int width, int height)
		{
			width = width;
			height = height;
			centreX = width / 2;
			centreY = height / 2;
			__super::setDimensions(width, height);
			for (int i = 0; i < rays; i++)
			{
				rayLengths[i] = (float)radius + (float)randomness / 100.0f * (float)radius * (float)nextGaussian();
			}
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			float dx = (float)x - centreX;
			float dy = (float)y - centreY;
			float distance = dx * dx + dy * dy;
			float angle = (float)(atan2(dy, dx));
			float d = (angle + PI) / (TWO_PI) * rays;
			int i = (int)(d);
			float f = d - i;

			if (radius != 0)
			{
				float length = lerp(f, (float)rayLengths[i % rays], (float)rayLengths[(i + 1) % rays]);
				float g = length * length / (distance + 0.0001f);
				g = (float)(pow(g, (100.0f - amount) / 50.0f));
				f -= 0.5f;
				//f *= amount/50.0f;
				f = 1 - f * f;
				f *= g;
			}
			f = clamp(f, 0.0, 1.0);
			return mixColors(f, rgb, color);
		}

		virtual CString *ToString()
		{
			return new CString("Stylize/Sparkle...");
		}
	};
}
