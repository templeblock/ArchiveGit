#pragma once

namespace ImageEffects
{
	class CellularFilter : WholeImageFilter, Function2D, MutatableFilter
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
				float cos = (float)(cos(value));
				float sin = (float)(sin(value));
				m00 = cos;
				m01 = sin;
				m10 = - sin;
				m11 = cos;
			}

		}
		virtual property float AngleCoefficient
		{
			float get()
			{
				return angleCoefficient;
			}

			void set(float value)
			{
				angleCoefficient = value;
			}

		}
		virtual property float GradientCoefficient
		{
			float get()
			{
				return gradientCoefficient;
			}

			void set(float value)
			{
				gradientCoefficient = value;
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
		virtual property float Randomness
		{
			float get()
			{
				return randomness;
			}

			void set(float value)
			{
				randomness = value;
			}

		}
		virtual property int GridType
		{
			int get()
			{
				return gridType;
			}

			void set(int value)
			{
				gridType = value;
			}

		}
		virtual property float DistancePower
		{
			float get()
			{
				return distancePower;
			}

			void set(float value)
			{
				distancePower = value;
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
#endif NOTUSED

	public:
		float scale = 32;
		float stretch = 1.0f;
		float angle = 0.0f;
	public:
		float amount = 1.0f;
		float turbulence = 1.0f;
		float gain = 0.5f;
		float bias = 0.5f;
		float distancePower = 2;
		bool useColor;
	public:
		Colormap *colormap = new Gradient();
		Array<float> *coefficients = new Array<float>{1, 0, 0, 0};
		float angleCoefficient;
		float m00 = 1.0f;
		float m01 = 0.0f;
		float m10 = 0.0f;
		float m11 = 1.0f;
		Array<Point> *results;
		float randomness;
		int gridType = HEXAGONAL;
	private:
		float min;
		float max;
		static Array<SByte> *probabilities;
		float gradientCoefficient;

	public:
		static const int RANDOM = 0;
		static const int SQUARE = 1;
		static const int HEXAGONAL = 2;
		static const int OCTAGONAL = 3;
		static const int TRIANGULAR = 4;

		CellularFilter()
		{
			results = new Array<Point>(3);
			for (int j = 0; j < results->Length; j++)
				results[j] = Point(this);
			if (probabilities == NULL)
			{
				probabilities = new Array<SByte>(8192);
				float factorial = 1;
				float total = 0;
				float mean = 2.5f;
				for (int i = 0; i < 10; i++)
				{
					if (i > 1)
						factorial *= i;
					float probability = (float)(pow(mean, i)) * (float)(exp(- mean)) / factorial;
					int start = (int)(total * 8192);
					total += probability;
					int end = (int)(total * 8192);
					for (int j = start; j < end; j++)
						probabilities[j] = (SByte)i;
				}
			}
		}

		virtual void setCoefficient(int i, float v)
		{
			coefficients[i] = v;
		}

		virtual float getCoefficient(int i)
		{
			return coefficients[i];
		}

	public:
		ref class Point
		{
		public:
			Point(CellularFilter *enclosingInstance)
			{
				InitBlock(enclosingInstance);
			}
		private:
			void InitBlock(CellularFilter *enclosingInstance)
			{
				enclosingInstance = enclosingInstance;
			}
			CellularFilter *enclosingInstance;
		public:
			property CellularFilter *Enclosing_Instance
			{
				CellularFilter *get()
				{
					return enclosingInstance;
				}

			}
			int index;
			float x, y;
			float dx, dy;
			float cubeX, cubeY;
			float distance;
		};

	private:
		float checkCube(float x, float y, int cubeX, int cubeY, Array<Point> *results)
		{
			int numPoints;
			srand(571 * cubeX + 23 * cubeY);
			switch (gridType)
			{

				case RANDOM:
				default:
					numPoints = probabilities[rand() & 0x1fff];
					//numPoints = 4;
					break;

				case SQUARE:
					numPoints = 1;
					break;

				case HEXAGONAL:
					numPoints = 1;
					break;

				case OCTAGONAL:
					numPoints = 2;
					break;

				case TRIANGULAR:
					numPoints = 2;
					break;
				}
			for (int i = 0; i < numPoints; i++)
			{
				float px = 0, py = 0;
				float weight = 1.0f;
				switch (gridType)
				{

					case RANDOM:
						px = (float)(rand/*NextDouble*/());
						py = (float)(rand/*NextDouble*/());
						break;

					case SQUARE:
						px = py = 0.5f;
						if (randomness != 0)
						{
							px = (float)(px + randomness * ((float)(rand/*NextDouble*/() - 0.5)));
							py = (float)(py + randomness * ((float)(rand/*NextDouble*/() - 0.5)));
						}
						break;

					case HEXAGONAL:
						if ((cubeX & 1) == 0)
						{
							px = 0.75f;
							py = 0;
						}
						else
						{
							px = 0.75f;
							py = 0.5f;
						}
						if (randomness != 0)
						{
							px += randomness * noise.noise2(271 * (cubeX + px), 271 * (cubeY + py));
							py += randomness * noise.noise2(271 * (cubeX + px) + 89, 271 * (cubeY + py) + 137);
						}
						break;

					case OCTAGONAL:
						switch (i)
						{

							case 0:
								px = 0.207f;
								py = 0.207f;
								break;

							case 1:
								px = 0.707f;
								py = 0.707f;
								weight = 1.6f;
								break;
							}
						if (randomness != 0)
						{
							px += randomness * noise.noise2(271 * (cubeX + px), 271 * (cubeY + py));
							py += randomness * noise.noise2(271 * (cubeX + px) + 89, 271 * (cubeY + py) + 137);
						}
						break;

					case TRIANGULAR:
						if ((cubeY & 1) == 0)
						{
							if (i == 0)
							{
								px = 0.25f;
								py = 0.35f;
							}
							else
							{
								px = 0.75f;
								py = 0.65f;
							}
						}
						else
						{
							if (i == 0)
							{
								px = 0.75f;
								py = 0.35f;
							}
							else
							{
								px = 0.25f;
								py = 0.65f;
							}
						}
						if (randomness != 0)
						{
							px += randomness * noise.noise2(271 * (cubeX + px), 271 * (cubeY + py));
							py += randomness * noise.noise2(271 * (cubeX + px) + 89, 271 * (cubeY + py) + 137);
						}
						break;
					}
				float dx = (float)(abs(x - px));
				float dy = (float)(abs(y - py));
				float d;
				dx *= weight;
				dy *= weight;
				if (distancePower == 1.0f)
					d = dx + dy;
				else if (distancePower == 2.0f)
				{
					d = (float)(sqrt(dx * dx + dy * dy));
				}
				else
				{
					d = (float)(pow((float)(pow(dx, distancePower)) + (float)(pow(dy, distancePower)), 1 / distancePower));
				}

				// Insertion sort the long way round to speed it up a bit
				if (d < results[0].distance)
				{
					Point p = results[2];
					results[2] = results[1];
					results[1] = results[0];
					results[0] = p;
					p.distance = d;
					p.dx = dx;
					p.dy = dy;
					p.x = cubeX + px;
					p.y = cubeY + py;
				}
				else if (d < results[1].distance)
				{
					Point p = results[2];
					results[2] = results[1];
					results[1] = p;
					p.distance = d;
					p.dx = dx;
					p.dy = dy;
					p.x = cubeX + px;
					p.y = cubeY + py;
				}
				else if (d < results[2].distance)
				{
					Point p = results[2];
					p.distance = d;
					p.dx = dx;
					p.dy = dy;
					p.x = cubeX + px;
					p.y = cubeY + py;
				}
			}
			return results[2].distance;
		}

	public:
		virtual float evaluate(float x, float y)
		{
			for (int j = 0; j < results->Length; j++)
				results[j].distance = MAXLONG;

			int ix = (int)(x);
			int iy = (int)(y);
			float fx = x - ix;
			float fy = y - iy;

			float d = checkCube(fx, fy, ix, iy, results);
			if (d > fy)
				d = checkCube(fx, fy + 1, ix, iy - 1, results);
			if (d > 1 - fy)
				d = checkCube(fx, fy - 1, ix, iy + 1, results);
			if (d > fx)
			{
				checkCube(fx + 1, fy, ix - 1, iy, results);
				if (d > fy)
					d = checkCube(fx + 1, fy + 1, ix - 1, iy - 1, results);
				if (d > 1 - fy)
					d = checkCube(fx + 1, fy - 1, ix - 1, iy + 1, results);
			}
			if (d > 1 - fx)
			{
				d = checkCube(fx - 1, fy, ix + 1, iy, results);
				if (d > fy)
					d = checkCube(fx - 1, fy + 1, ix + 1, iy - 1, results);
				if (d > 1 - fy)
					d = checkCube(fx - 1, fy - 1, ix + 1, iy + 1, results);
			}

			float t = 0;
			for (int i = 0; i < 3; i++)
				t += coefficients[i] * results[i].distance;
			if (angleCoefficient != 0)
			{
				float angle = (float)(atan2(y - results[0].y, x - results[0].x));
				if (angle < 0)
				{
					angle += 2 * (float)(PI);
				}
				angle /= 4 * (float)(PI);
				t += angleCoefficient * angle;
			}
			if (gradientCoefficient != 0)
			{
				float a = 1 / (results[0].dy + results[0].dx);
				t += gradientCoefficient * a;
			}
			return t;
		}

		virtual float turbulence2(float x, float y, float freq)
		{
			float t = 0.0f;

			for (float f = 1.0f; f <= freq; f *= 2)
				t += evaluate(f * x, f * y) / f;
			return t;
		}

		virtual int getPixel(int x, int y, Array<int> *inPixels, int width, int height)
		{
			try
			{
				float nx = m00 * x + m01 * y;
				float ny = m10 * x + m11 * y;
				nx /= scale;
				ny /= scale * stretch;
				nx += 1000;
				ny += 1000; // Reduce artifacts around 0,0
				float f = turbulence == 1.0f?evaluate(nx, ny):turbulence2(nx, ny, turbulence);
				// Normalize to 0..1
				//		f = (f-min)/(max-min);
				f *= 2;
				f *= amount;
				int a = ((int)(0xff000000));
				int v;
				if (colormap != NULL)
				{
					v = colormap->getColor(f);
					if (useColor)
					{
						int srcx = clamp((int)((results[0].x - 1000) * scale), 0, width - 1);
						int srcy = clamp((int)((results[0].y - 1000) * scale), 0, height - 1);
						v = inPixels[srcy * width + srcx];
						f = (results[1].distance - results[0].distance) / (results[1].distance + results[0].distance);
						f = smoothStep(coefficients[1], coefficients[0], f);
						v = mixColors(f, ((int)(0xff000000)), v);
					}
					return v;
				}
				else
				{
					v = PixelUtils::clamp((int)(f * 255));
					int r = v << 16;
					int g = v << 8;
					int b = v;
					return a | r | g | b;
				}
			}
			catch (...)
			{
				return 0;
			}
		}

	public:
		virtual Array<int> *filterPixels(int width, int height, Array<int> *inPixels, CRect& transformedSpace)
		{
			//		float[] minmax = Noise.findRange(this, null);
			//		min = minmax[0];
			//		max = minmax[1];

			int index = 0;
			Array<int> *outPixels = new Array<int>(width * height);

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					outPixels[index++] = getPixel(x, y, inPixels, width, height);
				}
			}
			return outPixels;
		}

	public:
		virtual void mutate(float mutationLevel, BITMAPINFOHEADER *d, bool keepShape, bool keepColors)
		{
			CellularFilter *dst = (CellularFilter*)d;
			srand((System::DateTime::Now.Ticks - 621355968000000000) / 10000);
			if (keepShape || amount == 0)
			{
				dst->GridType = GridType;
				dst->Randomness = Randomness;
				dst->Scale = Scale;
				dst->Angle = Angle;
				dst->Stretch = Stretch;
				dst->Amount = Amount;
				dst->Turbulence = Turbulence;
				dst->Colormap = Colormap;
				dst->DistancePower = DistancePower;
				dst->AngleCoefficient = AngleCoefficient;
				for (int i = 0; i < 4; i++)
					dst->setCoefficient(i, getCoefficient(i));
			}
			else
			{
				dst->scale = mutate(scale, mutationLevel, 0.4f, 5, 3, 200);
				dst->Angle = mutate(angle, mutationLevel, 0.3f, (float)(PI) / 2);
				dst->stretch = mutate(stretch, mutationLevel, 0.3f, 3, 1, 10);
				dst->amount = mutate(amount, mutationLevel, 0.3f, 0.2f, 0, 1);
				dst->turbulence = mutate(turbulence, mutationLevel, 0.3f, 0.5f, 1, 8);
				dst->distancePower = mutate(distancePower, mutationLevel, 0.2f, 0.5f, 1, 3);
				dst->randomness = mutate(randomness, mutationLevel, 0.4f, 0.2f, 0, 1);
				for (int i = 0; i < coefficients->Length; i++)
					dst->coefficients[i] = mutate(coefficients[i], mutationLevel, 0.3f, 0.2f, - 1, 1);
				if ((float)(rand/*NextDouble*/()) <= mutationLevel * 0.2)
				{
					dst->gridType = rand() % 5;
				}
				dst->angleCoefficient = mutate(angleCoefficient, mutationLevel, 0.2f, 0.5f, - 1, 1);
			}
			if (keepColors || mutationLevel == 0)
				dst->Colormap = Colormap;
			else if ((float)(rand/*NextDouble*/()) <= mutationLevel)
			{
				if ((float)(rand/*NextDouble*/()) <= mutationLevel)
					dst->Colormap = Gradient::randomGradient();
				else
					(Gradient*)((dst->Colormap)->mutate(mutationLevel);
			}
		}

	private:
		bool haveNextNextGaussian;
		double nextNextGaussian = 0.0;
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
			double multiplier = Math::Sqrt(-2 * Math::Log(s)/s);
			nextNextGaussian = v2 * multiplier;
			haveNextNextGaussian = true;
			return v1 * multiplier;
		 }

	private:
		float mutate(float n, float mutationLevel, float probability, float amount, float lower, float upper)
		{
			if ((float)(rand/*NextDouble*/()) <= mutationLevel * probability)
				return n;
			return clamp(n + mutationLevel * amount * (float)(nextGaussian()), lower, upper);
		}

		float mutate(float n, float mutationLevel, float probability, float amount)
		{
			if ((float)(rand/*NextDouble*/()) <= mutationLevel * probability)
				return n;
			return n + mutationLevel * amount * (float)(nextGaussian());
		}

	public:
		virtual System::Object *Clone()
		{
			CellularFilter *f = (CellularFilter*)(__super::Clone());
			f->coefficients = new Array<float>(coefficients->Length);
			coefficients->CopyTo(f->coefficients, 0);
			f->results = new Array<Point>(results->Length);
			results->CopyTo(f->results, 0);
			f->random = new System::Random();
			return f;
		}

		virtual CString *ToString()
		{
			return new CString("Texture/Cellular...");
		}
	};
}
