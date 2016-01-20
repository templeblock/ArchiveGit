#pragma once

namespace ImageEffects
{
	class /*interface*/ BinaryFunction
	{
		public: virtual bool isBlack(int rgb);
	};

	class /*interface*/ Function1D
	{
		public: virtual float evaluate(float v);
	};
	
	class /*interface*/ Function2D
	{
		public: virtual float evaluate(float x, float y);
	};
	
	class /*interface*/ Function3D
	{
		public: virtual float evaluate(float x, float y, float z);
	};
	
	class BlackFunction : BinaryFunction
	{
		public: /*virtual*/ bool isBlack(int rgb)
		{
			return (UINT)rgb == 0xff000000;
		}
	};
	
	static const float PI = (float)(PI);
	static const float HALF_PI = (float)(PI) / 2.0f;
	static const float QUARTER_PI = (float)(PI) / 4.0f;
	static const float TWO_PI = (float)(PI) * 2.0f;

	// Catmull-Rom splines
	static const float m00 = - 0.5f;
	static const float m01 = 1.5f;
	static const float m02 = - 1.5f;
	static const float m03 = 0.5f;
	static const float m10 = 1.0f;
	static const float m11 = - 2.5f;
	static const float m12 = 2.0f;
	static const float m13 = - 0.5f;
	static const float m20 = - 0.5f;
	static const float m21 = 0.0f;
	static const float m22 = 0.5f;
	static const float m23 = 0.0f;
	static const float m30 = 0.0f;
	static const float m31 = 1.0f;
	static const float m32 = 0.0f;
	static const float m33 = 0.0f;

	/// A class containing static math methods useful for image processing.
//j	class ImageMath
//j	{
//j	public:

		/// Apply a bias to a number in the unit interval, moving numbers towards 0 or 1
		/// according to the bias parameter.
		/// 
		/// <param name="a">the number to bias</param>
		/// <param name="b">the bias parameter. 0.5 means no change, smaller values bias towards 0, larger towards 1.</param>
		/// <returns> the output value</returns>
		static float bias(float a, float b)
		{
			//		return (float)Math.pow(a, Math.log(b) / Math.log(0.5));
			return a / ((1.0f / b - 2) * (1.0f - a) + 1);
		}

		/// A variant of the gamma function.
		/// <param name="a">the number to apply gain to</param>
		/// <param name="b">the gain parameter. 0.5 means no change, smaller values reduce gain, larger values increase gain.</param>
		/// <returns> the output value</returns>
		static float gain(float a, float b)
		{
			float c = (1.0f / b - 2.0f) * (1.0f - 2.0f * a);
			if (a < 0.5)
				return a / (c + 1.0f);
			else
				return (c - a) / (c - 1.0f);
		}

		/// The step function. Returns 0 below a threshold, 1 above.
		/// <param name="a">the threshold position</param>
		/// <param name="x">the input parameter</param>
		/// <returns> the output value - 0 or 1</returns>
		static float step(float a, float x)
		{
			return (x < a)?0.0f:1.0f;
		}

		/// The pulse function. Returns 1 between two thresholds, 0 outside.
		/// <param name="a">the lower threshold position</param>
		/// <param name="b">the upper threshold position</param>
		/// <param name="x">the input parameter</param>
		/// <returns> the output value - 0 or 1</returns>
		static float pulse(float a, float b, float x)
		{
			return (x < a || x >= b)?0.0f:1.0f;
		}

		/// A smoothed pulse function. A cubic function is used to smooth the step between two thresholds.
		/// <param name="a1">the lower threshold position for the start of the pulse</param>
		/// <param name="a2">the upper threshold position for the start of the pulse</param>
		/// <param name="b1">the lower threshold position for the end of the pulse</param>
		/// <param name="b2">the upper threshold position for the end of the pulse</param>
		/// <param name="x">the input parameter</param>
		/// <returns> the output value</returns>
		static float smoothPulse(float a1, float a2, float b1, float b2, float x)
		{
			if (x < a1 || x >= b2)
				return 0;
			if (x >= a2)
			{
				if (x < b1)
					return 1.0f;
				x = (x - b1) / (b2 - b1);
				return 1.0f - (x * x * (3.0f - 2.0f * x));
			}
			x = (x - a1) / (a2 - a1);
			return x * x * (3.0f - 2.0f * x);
		}

		/// A smoothed step function. A cubic function is used to smooth the step between two thresholds.
		/// <param name="a">the lower threshold position</param>
		/// <param name="b">the upper threshold position</param>
		/// <param name="x">the input parameter</param>
		/// <returns> the output value</returns>
		static float smoothStep(float a, float b, float x)
		{
			if (x < a)
				return 0;
			if (x >= b)
				return 1;
			x = (x - a) / (b - a);
			return x * x * (3 - 2 * x);
		}

		/// A "circle up" function. Returns y on a unit circle given 1-x. Useful for forming bevels.
		/// <param name="x">the input parameter in the range 0..1</param>
		/// <returns> the output value</returns>
		static float circleUp(float x)
		{
			x = 1 - x;
			return (float)(sqrt(1 - x * x));
		}

		/// A "circle down" function. Returns 1-y on a unit circle given x. Useful for forming bevels.
		/// <param name="x">the input parameter in the range 0..1</param>
		/// <returns> the output value</returns>
		static float circleDown(float x)
		{
			return 1.0f - (float)(sqrt(1 - x * x));
		}

		/// Clamp a value to an interval.
		/// <param name="a">the lower clamp threshold</param>
		/// <param name="b">the upper clamp threshold</param>
		/// <param name="x">the input parameter</param>
		/// <returns> the clamped value</returns>
		static float clamp(float x, float a, float b)
		{
			return (x < a)?a:((x > b)?b:x);
		}

		/// Clamp a value to an interval.
		/// <param name="a">the lower clamp threshold</param>
		/// <param name="b">the upper clamp threshold</param>
		/// <param name="x">the input parameter</param>
		/// <returns> the clamped value</returns>
		static int clamp(int x, int a, int b)
		{
			return (x < a)?a:((x > b)?b:x);
		}

		/// Return a mod b. This differs from the&  operator with respect to negative numbers.
		/// <param name="a">the dividend</param>
		/// <param name="b">the divisor</param>
		/// <returns> a mod b</returns>
		static double mod(double a, double b)
		{
			int n = (int)(a / b);

			a -= n * b;
			if (a < 0)
				return a + b;
			return a;
		}

		/// Return a mod b. This differs from the&  operator with respect to negative numbers.
		/// <param name="a">the dividend</param>
		/// <param name="b">the divisor</param>
		/// <returns> a mod b</returns>
		static float mod(float a, float b)
		{
			int n = (int)(a / b);

			a -= n * b;
			if (a < 0)
				return a + b;
			return a;
		}

		/// Return a mod b. This differs from the&  operator with respect to negative numbers.
		/// <param name="a">the dividend</param>
		/// <param name="b">the divisor</param>
		/// <returns> a mod b</returns>
		static int mod(int a, int b)
		{
			int n = a / b;

			a -= n * b;
			if (a < 0)
				return a + b;
			return a;
		}

		/// The triangle function. Returns a repeating triangle shape in the range 0..1 with wavelength 1.0
		/// <param name="x">the input parameter</param>
		/// <returns> the output value</returns>
		static float triangle(float x)
		{
			float r = mod(x, 1.0f);
			return 2.0f * (r < 0.5?r:1 - r);
		}

		/// Linear interpolation.
		/// <param name="t">the interpolation parameter</param>
		/// <param name="a">the lower interpolation range</param>
		/// <param name="b">the upper interpolation range</param>
		/// <returns> the interpolated value</returns>
		static float lerp(float t, float a, float b)
		{
			return a + t * (b - a);
		}

		/// Linear interpolation.
		/// <param name="t">the interpolation parameter</param>
		/// <param name="a">the lower interpolation range</param>
		/// <param name="b">the upper interpolation range</param>
		/// <returns> the interpolated value</returns>
		static int lerp(float t, int a, int b)
		{
			return (int)(a + t * (b - a));
		}

		/// Linear interpolation of ARGB values.
		/// <param name="t">the interpolation parameter</param>
		/// <param name="rgb1">the lower interpolation range</param>
		/// <param name="rgb2">the upper interpolation range</param>
		/// <returns> the interpolated value</returns>
		static int mixColors(float t, int rgb1, int rgb2)
		{
			int a1 = (rgb1 >> 24) & 0xff;
			int r1 = (rgb1 >> 16) & 0xff;
			int g1 = (rgb1 >> 8) & 0xff;
			int b1 = rgb1 & 0xff;
			int a2 = (rgb2 >> 24) & 0xff;
			int r2 = (rgb2 >> 16) & 0xff;
			int g2 = (rgb2 >> 8) & 0xff;
			int b2 = rgb2 & 0xff;
			a1 = lerp(t, a1, a2);
			r1 = lerp(t, r1, r2);
			g1 = lerp(t, g1, g2);
			b1 = lerp(t, b1, b2);
			return (a1 << 24) | (r1 << 16) | (g1 << 8) | b1;
		}

		/// Bilinear interpolation of ARGB values.
		/// <param name="x">the X interpolation parameter 0..1</param>
		/// <param name="y">the y interpolation parameter 0..1</param>
		/// <param name="rgb">Array of four ARGB values in the order NW, NE, SW, SE</param>
		/// <returns> the interpolated value</returns>
		static int bilinearInterpolate(float x, float y, int p[])
		{
			float m0, m1;
			int a0 = (p[0] >> 24) & 0xff;
			int r0 = (p[0] >> 16) & 0xff;
			int g0 = (p[0] >> 8) & 0xff;
			int b0 = p[0] & 0xff;
			int a1 = (p[1] >> 24) & 0xff;
			int r1 = (p[1] >> 16) & 0xff;
			int g1 = (p[1] >> 8) & 0xff;
			int b1 = p[1] & 0xff;
			int a2 = (p[2] >> 24) & 0xff;
			int r2 = (p[2] >> 16) & 0xff;
			int g2 = (p[2] >> 8) & 0xff;
			int b2 = p[2] & 0xff;
			int a3 = (p[3] >> 24) & 0xff;
			int r3 = (p[3] >> 16) & 0xff;
			int g3 = (p[3] >> 8) & 0xff;
			int b3 = p[3] & 0xff;

			float cx = 1.0f - x;
			float cy = 1.0f - y;

			m0 = cx * a0 + x * a1;
			m1 = cx * a2 + x * a3;
			int a = (int)(cy * m0 + y * m1);

			m0 = cx * r0 + x * r1;
			m1 = cx * r2 + x * r3;
			int r = (int)(cy * m0 + y * m1);

			m0 = cx * g0 + x * g1;
			m1 = cx * g2 + x * g3;
			int g = (int)(cy * m0 + y * m1);

			m0 = cx * b0 + x * b1;
			m1 = cx * b2 + x * b3;
			int b = (int)(cy * m0 + y * m1);

			return (a << 24) | (r << 16) | (g << 8) | b;
		}

		/// Return the NTSC gray level of an RGB value.
		/// <param name="rgb1">the input pixel</param>
		/// <returns> the gray level (0-255)</returns>
		static int brightnessNTSC(int rgb)
		{
			int r = (rgb >> 16) & 0xff;
			int g = (rgb >> 8) & 0xff;
			int b = rgb & 0xff;
			return (int)(r * 0.299f + g * 0.587f + b * 0.114f);
		}

		/// Compute a Catmull-Rom spline.
		/// <param name="x">the input parameter</param>
		/// <param name="numKnots">the number of knots in the spline</param>
		/// <param name="knots">the Array of knots</param>
		/// <returns> the spline value</returns>
//j	public:
		static float spline(float x, int numKnots, Array<float> *knots)
		{
			int span;
			int numSpans = numKnots - 3;
			float k0, k1, k2, k3;
			float c0, c1, c2, c3;

			if (numSpans < 1)
				int zz = 0; //jthrow new System::ArgumentException("Too few knots in spline");

			x = clamp(x, 0.0, 1.0) * numSpans;
			span = (int)(x);
			if (span > numKnots - 4)
				span = numKnots - 4;
			x -= span;

			k0 = knots->GetAt(span);
			k1 = knots->GetAt(span + 1);
			k2 = knots->GetAt(span + 2);
			k3 = knots->GetAt(span + 3);

			c3 = m00 * k0 + m01 * k1 + m02 * k2 + m03 * k3;
			c2 = m10 * k0 + m11 * k1 + m12 * k2 + m13 * k3;
			c1 = m20 * k0 + m21 * k1 + m22 * k2 + m23 * k3;
			c0 = m30 * k0 + m31 * k1 + m32 * k2 + m33 * k3;

			return ((c3 * x + c2) * x + c1) * x + c0;
		}

		/// Compute a Catmull-Rom spline, but with variable knot spacing.
		/// <param name="x">the input parameter</param>
		/// <param name="numKnots">the number of knots in the spline</param>
		/// <param name="xknots">the Array of knot x values</param>
		/// <param name="yknots">the Array of knot y values</param>
		/// <returns> the spline value</returns>
		static float spline(float x, int numKnots, Array<int> *xknots, Array<int> *yknots)
		{
			int span;
			int numSpans = numKnots - 3;
			float k0, k1, k2, k3;
			float c0, c1, c2, c3;

			if (numSpans < 1)
				int zz = 0; //jthrow new System::ArgumentException("Too few knots in spline");

			for (span = 0; span < numSpans; span++)
				if (xknots->GetAt(span + 1) > x)
					break;
			if (span > numKnots - 3)
				span = numKnots - 3;
			float t = (float)(x - xknots->GetAt(span)) / (xknots->GetAt(span + 1) - xknots->GetAt(span));
			span--;
			if (span < 0)
			{
				span = 0;
				t = 0;
			}

			k0 = (float)yknots->GetAt(span);
			k1 = (float)yknots->GetAt(span + 1);
			k2 = (float)yknots->GetAt(span + 2);
			k3 = (float)yknots->GetAt(span + 3);

			c3 = m00 * k0 + m01 * k1 + m02 * k2 + m03 * k3;
			c2 = m10 * k0 + m11 * k1 + m12 * k2 + m13 * k3;
			c1 = m20 * k0 + m21 * k1 + m22 * k2 + m23 * k3;
			c0 = m30 * k0 + m31 * k1 + m32 * k2 + m33 * k3;

			return ((c3 * t + c2) * t + c1) * t + c0;
		}

		/// Compute a Catmull-Rom spline for RGB values.
		/// <param name="x">the input parameter</param>
		/// <param name="numKnots">the number of knots in the spline</param>
		/// <param name="knots">the Array of knots</param>
		/// <returns> the spline value</returns>
		static int colorSpline(float x, int numKnots, Array<int> *knots)
		{
			int span;
			int numSpans = numKnots - 3;
			float k0, k1, k2, k3;
			float c0, c1, c2, c3;

			if (numSpans < 1)
				int zz = 0; //jthrow new System::ArgumentException("Too few knots in spline");

			x = clamp(x, 0.0, 1.0) * numSpans;
			span = (int)(x);
			if (span > numKnots - 4)
				span = numKnots - 4;
			x -= span;

			int v = 0;
			for (int i = 0; i < 4; i++)
			{
				int shift = i * 8;

				k0 = (float)((knots->GetAt(span) >> shift) & 0xff);
				k1 = (float)((knots->GetAt(span + 1) >> shift) & 0xff);
				k2 = (float)((knots->GetAt(span + 2) >> shift) & 0xff);
				k3 = (float)((knots->GetAt(span + 3) >> shift) & 0xff);

				c3 = m00 * k0 + m01 * k1 + m02 * k2 + m03 * k3;
				c2 = m10 * k0 + m11 * k1 + m12 * k2 + m13 * k3;
				c1 = m20 * k0 + m21 * k1 + m22 * k2 + m23 * k3;
				c0 = m30 * k0 + m31 * k1 + m32 * k2 + m33 * k3;
				int n = (int)(((c3 * x + c2) * x + c1) * x + c0);
				if (n < 0)
					n = 0;
				else if (n > 255)
					n = 255;
				v |= n << shift;
			}

			return v;
		}

		/// Compute a Catmull-Rom spline for RGB values, but with variable knot spacing.
		/// <param name="x">the input parameter</param>
		/// <param name="numKnots">the number of knots in the spline</param>
		/// <param name="xknots">the Array of knot x values</param>
		/// <param name="yknots">the Array of knot y values</param>
		/// <returns> the spline value</returns>
		static int colorSpline(int x, int numKnots, Array<int> *xknots, Array<int> *yknots)
		{
			int span;
			int numSpans = numKnots - 3;
			float k0, k1, k2, k3;
			float c0, c1, c2, c3;

			if (numSpans < 1)
				int zz = 0; //jthrow new System::ArgumentException("Too few knots in spline");

			for (span = 0; span < numSpans; span++)
				if (xknots->GetAt(span + 1) > x)
					break;
			if (span > numKnots - 3)
				span = numKnots - 3;
			float t = (float)(x - xknots->GetAt(span)) / (xknots->GetAt(span + 1) - xknots->GetAt(span));
			span--;
			if (span < 0)
			{
				span = 0;
				t = 0;
			}

			int v = 0;
			for (int i = 0; i < 4; i++)
			{
				int shift = i * 8;

				k0 = (float)((yknots->GetAt(span) >> shift) & 0xff);
				k1 = (float)((yknots->GetAt(span + 1) >> shift) & 0xff);
				k2 = (float)((yknots->GetAt(span + 2) >> shift) & 0xff);
				k3 = (float)((yknots->GetAt(span + 3) >> shift) & 0xff);

				c3 = m00 * k0 + m01 * k1 + m02 * k2 + m03 * k3;
				c2 = m10 * k0 + m11 * k1 + m12 * k2 + m13 * k3;
				c1 = m20 * k0 + m21 * k1 + m22 * k2 + m23 * k3;
				c0 = m30 * k0 + m31 * k1 + m32 * k2 + m33 * k3;
				int n = (int)(((c3 * t + c2) * t + c1) * t + c0);
				if (n < 0)
					n = 0;
				else if (n > 255)
					n = 255;
				v |= n << shift;
			}

			return v;
		}

		/// An implementation of Fant's resampling algorithm.
		/// <param name="source">the source pixels</param>
		/// <param name="dest">the destination pixels</param>
		/// <param name="length">the length of the scanline to resample</param>
		/// <param name="offset">the start offset into the arrays</param>
		/// <param name="stride">the offset between pixels in consecutive rows</param>
		/// <param name="out">an Array of output positions for each pixel</param>
		static void resample(Array<int> *source, Array<int> *dest, int length, int offset, int stride, Array<float> *out_Renamed)
		{
			int i, j;
			float sizfac;
			float inSegment;
			float outSegment;
			int a, r, g, b, nextA, nextR, nextG, nextB;
			float aSum, rSum, gSum, bSum;
			Array<float> *in_Renamed;
			int srcIndex = offset;
			int destIndex = offset;
			int lastIndex = source->get_Length();
			int rgb;

			in_Renamed = new Array<float>(length + 1);
			i = 0;
			for (j = 0; j < length; j++)
			{
				while (out_Renamed->GetAt(i + 1) < j)
					i++;
				in_Renamed->GetAt(j) = i + (float)(j - out_Renamed->GetAt(i)) / (out_Renamed->GetAt(i + 1) - out_Renamed->GetAt(i));
			}
			in_Renamed->SetValue((float)length, length);

			inSegment = 1.0f;
			outSegment = in_Renamed->GetAt(1);
			sizfac = outSegment;
			aSum = rSum = gSum = bSum = 0.0f;
			rgb = source->GetAt(srcIndex);
			a = (rgb >> 24) & 0xff;
			r = (rgb >> 16) & 0xff;
			g = (rgb >> 8) & 0xff;
			b = rgb & 0xff;
			srcIndex += stride;
			rgb = source->GetAt(srcIndex);
			nextA = (rgb >> 24) & 0xff;
			nextR = (rgb >> 16) & 0xff;
			nextG = (rgb >> 8) & 0xff;
			nextB = rgb & 0xff;
			srcIndex += stride;
			i = 1;

			while (i < length)
			{
				float aIntensity = inSegment * a + (1.0f - inSegment) * nextA;
				float rIntensity = inSegment * r + (1.0f - inSegment) * nextR;
				float gIntensity = inSegment * g + (1.0f - inSegment) * nextG;
				float bIntensity = inSegment * b + (1.0f - inSegment) * nextB;
				if (inSegment < outSegment)
				{
					aSum += (aIntensity * inSegment);
					rSum += (rIntensity * inSegment);
					gSum += (gIntensity * inSegment);
					bSum += (bIntensity * inSegment);
					outSegment -= inSegment;
					inSegment = 1.0f;
					a = nextA;
					r = nextR;
					g = nextG;
					b = nextB;
					if (srcIndex < lastIndex)
						rgb = source->GetAt(srcIndex);
					nextA = (rgb >> 24) & 0xff;
					nextR = (rgb >> 16) & 0xff;
					nextG = (rgb >> 8) & 0xff;
					nextB = rgb & 0xff;
					srcIndex += stride;
				}
				else
				{
					aSum += (aIntensity * outSegment);
					rSum += (rIntensity * outSegment);
					gSum += (gIntensity * outSegment);
					bSum += (bIntensity * outSegment);
					dest->GetAt(destIndex) = ((int)(min(aSum / sizfac, 255)) << 24) | ((int)(min(rSum / sizfac, 255)) << 16) | ((int)(min(gSum / sizfac, 255)) << 8) | (int)(min(bSum / sizfac, 255));
					destIndex += stride;
					rSum = gSum = bSum = 0.0f;
					inSegment -= outSegment;
					outSegment = in_Renamed->GetAt(i + 1) - in_Renamed->GetAt(i);
					sizfac = outSegment;
					i++;
				}
			}
		}

//j	};
}
