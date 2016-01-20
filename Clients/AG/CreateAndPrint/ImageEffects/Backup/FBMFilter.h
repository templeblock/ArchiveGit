#pragma once

namespace ImageEffects
{
	class FBMFilter : PointFilter
	{
#ifdef NOTUSED
	public:
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
		virtual property float Stretch
		{
			float get()
			{
				return stretch;
			}

			void set(float value)
			{
				stretch = value;
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
				float cos = (float)(cos(angle));
				float sin = (float)(sin(angle));
				m00 = cos;
				m01 = sin;
				m10 = - sin;
				m11 = cos;
			}

		}
		virtual property float Octaves
		{
			float get()
			{
				return octaves;
			}

			void set(float value)
			{
				octaves = value;
			}

		}
		virtual property float Lacunarity
		{
			float get()
			{
				return lacunarity;
			}

			void set(float value)
			{
				lacunarity = value;
			}

		}
		virtual property float Gain
		{
			float get()
			{
				return gain;
			}

			void set(float value)
			{
				gain = value;
			}

		}
		virtual property float Bias
		{
			float get()
			{
				return bias;
			}

			void set(float value)
			{
				bias = value;
			}

		}
		virtual property Colormap *Colormap
		{
			Colormap *get()
			{
				return colormap;
			}

			void set(Colormap *value)
			{
				colormap = value;
			}

		}
		virtual property int BasisType
		{
			int get()
			{
				return basisType;
			}

			void set(int value)
			{
				basisType = value;
				switch (value)
				{

					case NOISE:
					default:
						basis = new Noise();
						break;

					case RIDGED:
						basis = new RidgedFBM();
						break;

					case VLNOISE:
						basis = new VLNoise();
						break;

					case SCNOISE:
						basis = new SCNoise();
						break;

					case CELLULAR:
						basis = new CellularFunction2D();
						break;
					}
			}

		}
		virtual property Function2D *Basis
		{
			Function2D *get()
			{
				return basis;
			}

			void set(Function2D *value)
			{
				basis = value;
			}
		}
#endif NOTUSED

		static const int NOISE = 0;
		static const int RIDGED = 1;
		static const int VLNOISE = 2;
		static const int SCNOISE = 3;
		static const int CELLULAR = 4;

	private:
		float scale;
		float stretch;
		float angle;
		float amount;
		float H;
		float octaves;
		float lacunarity;
		float gain;
		float bias;
		int operation;
		float m00;
		float m01;
		float m10;
		float m11;
		float min;
		float max;
		Colormap *colormap;
		bool ridged;
		FBM *fBm;
	private:
		bool haveNextNextGaussian;
		double nextNextGaussian;
		int basisType;
		Function2D *basis;

	public:
		FBMFilter()
		{
			scale = 32;
			stretch = 1.0f;
			angle = 0.0f;
			amount = 1.0f;
			H = 1.0f;
			octaves = 4.0f;
			lacunarity = 2.0f;
			gain = 0.5f;
			bias = 0.5f;
			m00 = 1.0f;
			m01 = 0.0f;
			m10 = 0.0f;
			m11 = 1.0f;
			colormap = new Gradient();
			nextNextGaussian = 0.0;
			basisType = NOISE;
		}

		virtual void setH(float H)
		{
			H = H;
		}

		virtual float getH()
		{
			return H;
		}

	public:
		virtual FBM *makeFBM(float H, float lacunarity, float octaves)
		{
			FBM *fbm = new FBM(H, lacunarity, octaves, basis);
			Array<float> *minmax = noise.findRange(fbm, NULL);
			min = minmax[0];
			max = minmax[1];
			return fbm;
		}

	public:
		virtual BITMAPINFOHEADER *filter(BITMAPINFOHEADER *src, BITMAPINFOHEADER *dst)
		{
			fBm = makeFBM(H, lacunarity, octaves);
			return __super::filter(src, dst);
		}

		virtual int filterRGB(int x, int y, int rgb)
		{
			float nx = m00 * x + m01 * y;
			float ny = m10 * x + m11 * y;
			nx /= scale;
			ny /= scale * stretch;
			float f = fBm->evaluate(nx, ny);
			// Normalize to 0..1
			f = (f - min) / (max - min);
			//		f = (f + 0.5) * 0.5;
			f = gain(f, gain);
			f = bias(f, bias);
			f *= amount;
			int a = rgb & ((int)(0xff000000));
			int v;
			if (colormap != NULL)
				v = colormap->getColor(f);
			else
			{
				v = PixelUtils::clamp((int)(f * 255));
				int r = v << 16;
				int g = v << 8;
				int b = v;
				v = a | r | g | b;
			}
			if (operation != PixelUtils::REPLACE)
				v = PixelUtils::combinePixels(rgb, v, operation);
			return v;
		}

		virtual void mutate(float mutationLevel, BITMAPINFOHEADER *d, bool keepShape, bool keepColors)
		{
			FBMFilter *dst = (FBMFilter*)d;
			if (keepShape || amount == 0)
			{
				dst->Scale = Scale;
				dst->Angle = Angle;
				dst->Stretch = Stretch;
				dst->Amount = Amount;
				dst->Lacunarity = Lacunarity;
				dst->Octaves = Octaves;
				dst->setH(getH());
				dst->Gain = Gain;
				dst->Bias = Bias;
				dst->Colormap = Colormap;
			}
			else
			{
				dst->scale = mutate(scale, 0.6f, 4, 3, 64);
				dst->Angle = mutate(angle, 0.6f, PI / 2);
				dst->stretch = mutate(stretch, 0.6f, 5, 1, 10);
				dst->amount = mutate(amount, 0.6f, 0.2f, 0, 1);
				dst->lacunarity = mutate(lacunarity, 0.5f, 0.5f, 0, 3);
				dst->octaves = mutate(octaves, 0.9f, 0.2f, 0, 12);
				dst->H = mutate(H, 0.7f, 0.2f, 0, 1);
				dst->gain = mutate(gain, 0.2f, 0.2f, 0, 1);
				dst->bias = mutate(bias, 0.2f, 0.2f, 0, 1);
			}
			if (keepColors || amount == 0)
				dst->Colormap = Colormap;
			else
				dst->Colormap = Gradient::randomGradient();
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
		float mutate(float n, float probability, float amount, float lower, float upper)
		{
			if ((float)(rand/*NextDouble*/()) >= probability)
				return n;
			return clamp(n + amount * (float)(nextGaussian()), lower, upper);
		}

		float mutate(float n, float probability, float amount)
		{
			if ((float)(rand/*NextDouble*/()) >= probability)
				return n;
			return n + amount * (float)(nextGaussian());
		}

	public:
		virtual CString *ToString()
		{
			return new CString("Texture/Fractal Brownian Motion...");
		}

		virtual System::Object *Clone()
		{
			return NULL;
		}
	};
}