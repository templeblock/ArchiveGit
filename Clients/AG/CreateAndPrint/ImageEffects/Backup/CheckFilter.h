#pragma once

namespace ImageEffects
{
	/// A Filter to draw grids and check patterns.
	class CheckFilter : PointFilter
	{
#ifdef NOTUSED
	public:
		virtual property int Foreground
		{
			int get()
			{
				return foreground;
			}

			void set(int value)
			{
				foreground = value;
			}

		}
		virtual property int Background
		{
			int get()
			{
				return background;
			}

			void set(int value)
			{
				background = value;
			}

		}
		virtual property int XScale
		{
			int get()
			{
				return xScale;
			}

			void set(int value)
			{
				xScale = value;
			}

		}
		virtual property int YScale
		{
			int get()
			{
				return yScale;
			}

			void set(int value)
			{
				yScale = value;
			}

		}
		virtual property int Fuzziness
		{
			int get()
			{
				return fuzziness;
			}

			void set(int value)
			{
				fuzziness = value;
			}

		}
		virtual property int Operation
		{
			int get()
			{
				return operation;
			}

			void set(int value)
			{
				operation = value;
			}

		}
		virtual property float Angle
		{
			float get()
			{
				return angle;
			}

			void set(float value)
			{
				angle = value;
				float cos = (float)(cos(value));
				float sin = (float)(sin(value));
				m00 = cos;
				m01 = sin;
				m10 = - sin;
				m11 = cos;
			}

		}
#endif NOTUSED

	private:
		int xScale;
		int yScale;
		int foreground;
		int background;
		int fuzziness;
		float angle;
		int operation;
		float m00;
		float m01;
		float m10;
		float m11;

	public:
		CheckFilter()
		{
			xScale = 8;
			yScale = 8;
			foreground = ((int)(0xffffffff));
			background = ((int)(0xff000000));
			angle = 0.0f;
			m00 = 1.0f;
			m01 = 0.0f;
			m10 = 0.0f;
			m11 = 1.0f;
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			float nx = (m00 * x + m01 * y) / xScale;
			float ny = (m10 * x + m11 * y) / yScale;
			float f = ((((int)(nx + 100000) & 2) != ((int)(ny + 100000) & 2)) ? 1.0f : 0.0f);
			if (fuzziness != 0)
			{
				float fuzz = (fuzziness / 100.0f);
				float fx = smoothPulse(0, fuzz, 1 - fuzz, 1, mod(nx, 1.0f));
				float fy = smoothPulse(0, fuzz, 1 - fuzz, 1, mod(ny, 1.0f));
				f *= fx * fy;
			}
			return mixColors(f, foreground, background);
		}

		virtual CString *ToString()
		{
			return new CString("Texture/Checkerboard...");
		}
	};
}
