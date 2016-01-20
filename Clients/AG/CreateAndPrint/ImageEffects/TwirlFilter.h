#pragma once

namespace ImageEffects
{
	/// A Filter which distorts an image by twisting it from the centre out.
	/// The twisting is centred at the centre of the image and extends out to the smallest of
	/// the width and height. Pixels outside this radius are unaffected.
	class TwirlFilter : TransformFilter
	{
		/// Get the angle of twist.
		/// <returns> the angle in radians.
		/// </returns>
		/// Set the angle of twirl in radians. 0 means no distortion.
		/// <param name="angle">the angle of twirl. This is the angle by which pixels at the nearest edge of the image will move.
		/// </param>
#ifdef NOTUSED
	public:
		virtual property float Angle
		{
			float get()
			{
				return angle;
			}

			void set(float value)
			{
				angle = value;
			}

		}
		virtual property float CentreX
		{
			float get()
			{
				return centreX;
			}

			void set(float value)
			{
				centreX = value;
			}

		}
		virtual property float CentreY
		{
			float get()
			{
				return centreY;
			}

			void set(float value)
			{
				centreY = value;
			}

		}
		virtual property CPointF Centre
		{
			CPointF get()
			{
				return CPointF(centreX, centreY);
			}

			void set(CPointF value)
			{
				centreX = (float)(value::X);
				centreY = (float)(value::Y);
			}

		}
		virtual property float Radius
		{
			float get()
			{
				return radius;
			}

			void set(float value)
			{
				radius = value;
			}

		}
#endif NOTUSED

	private:
		float angle;
		float centreX;
		float centreY;
		float radius;

		float radius2;
		float icentreX;
		float icentreY;

		/// Construct a TwirlFilter with no distortion.
	public:
		TwirlFilter()
		{
			centreX = 0.5f;
			centreY = 0.5f;
			edgeAction = CLAMP;
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			icentreX = width * centreX;
			icentreY = height * centreY;
			if (radius == 0)
				radius = min(icentreX, icentreY);
			radius2 = radius * radius;
			return __super::filterPixels(width, height, inPixels, transformedSpace);
		}

		virtual void transformInverse(int x, int y, Array<float> *out_Renamed)
		{
			float dx = x - icentreX;
			float dy = y - icentreY;
			float distance = dx * dx + dy * dy;
			if (distance > radius2)
			{
				out_Renamed->SetValue((float)x, 0);
				out_Renamed->SetValue((float)y, 1);
			}
			else
			{
				distance = (float)(sqrt(distance));
				float a = (float)(atan2(dy, dx)) + angle * (radius - distance) / radius;
				out_Renamed->SetValue(icentreX + distance * (float)(cos(a)), 0);
				out_Renamed->SetValue(icentreY + distance * (float)(sin(a)), 1);
			}
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Distort/Twirl...");
		}
	};
}
