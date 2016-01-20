#pragma once
#include "TransformFilter.h"

namespace ImageEffects
{
	/// A filter which distorts an image by rippling it in the X or Y directions.
	/// The amplitude and wavelength of rippling can be specified as well as whether 
	/// pixels going off the edges are wrapped or not.
	class RippleFilter : TransformFilter
	{
		/// Get the amplitude of ripple in the X direction.
		/// <returns> the amplitude (in pixels).
		/// </returns>
		/// Set the amplitude of ripple in the X direction.
		/// <param name="xAmplitude">the amplitude (in pixels).
		/// </param>
#ifdef NOUTSED
	public:
		virtual property float XAmplitude
		{
			float get()
			{
				return xAmplitude;
			}

			void set(float value)
			{
				xAmplitude = value;
			}

		}
		/// Get the wavelength of ripple in the X direction.
		/// <returns> the wavelength (in pixels).
		/// </returns>
		/// Set the wavelength of ripple in the X direction.
		/// <param name="xWavelength">the wavelength (in pixels).
		/// </param>
		virtual property float XWavelength
		{
			float get()
			{
				return xWavelength;
			}

			void set(float value)
			{
				xWavelength = value;
			}

		}
		/// Get the amplitude of ripple in the Y direction.
		/// <returns> the amplitude (in pixels).
		/// </returns>
		/// Set the amplitude of ripple in the Y direction.
		/// <param name="yAmplitude">the amplitude (in pixels).
		/// </param>
		virtual property float YAmplitude
		{
			float get()
			{
				return yAmplitude;
			}

			void set(float value)
			{
				yAmplitude = value;
			}

		}
		/// Get the wavelength of ripple in the Y direction.
		/// <returns> the wavelength (in pixels).
		/// </returns>
		/// Set the wavelength of ripple in the Y direction.
		/// <param name="yWavelength">the wavelength (in pixels).
		/// </param>
		virtual property float YWavelength
		{
			float get()
			{
				return yWavelength;
			}

			void set(float value)
			{
				yWavelength = value;
			}

		}
		virtual property int WaveType
		{
			int get()
			{
				return waveType;
			}

			void set(int value)
			{
				waveType = value;
			}

		}
#endif NOTUSED

	private:


	public:
		static const int SINE = 0;
		static const int SAWTOOTH = 1;
		static const int TRIANGLE = 2;
		static const int NOISE = 3;

		float xAmplitude, yAmplitude;
		float xWavelength, yWavelength;
	private:
		int waveType;
		Noise noise;

		/// Construct a RIppleFIlter
	public:
		RippleFilter()
		{
			xAmplitude = 5.0f;
			yAmplitude = 0.0f;
			xWavelength = yWavelength = 16.0f;
		}

	public:
		virtual void transformSpace(CRect& r)
		{
			if (edgeAction == ZERO)
			{
				r.left -= (int)xAmplitude;
				r.right += (int)xAmplitude;
				r.top -= (int)yAmplitude;
				r.bottom += (int)yAmplitude;
			}
		}

		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			float nx = (float)(y) / xWavelength;
			float ny = (float)(x) / yWavelength;
			float fx, fy;
			switch (waveType)
			{
				case SINE:
				default:
					fx = (float)(sin(nx));
					fy = (float)(sin(ny));
					break;

				case SAWTOOTH:
					fx = mod(nx, 1.0f);
					fy = mod(ny, 1.0f);
					break;

				case TRIANGLE:
					fx = triangle(nx);
					fy = triangle(ny);
					break;

				case NOISE:
					fx = noise.noise1(nx);
					fy = noise.noise1(ny);
					break;
				}
			out_Renamed->SetValue(x + xAmplitude * fx, 0);
			out_Renamed->SetValue(y + yAmplitude * fy, 1);
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Ripple...");
		}
	};
}
