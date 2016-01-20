#pragma once
#include "stdlib.h"

namespace ImageMath
{
#ifdef NOTUSEDATALL
#ifdef NOTUSED
	class CellularFunction2D : Function2D
	{
		
	public:
		float distancePower;
		bool cells;
		bool angular;
	private:
		Array<float>* coefficients;
		int random;
		Array<POINT>* results;
		
		public: CellularFunction2D()
		{
			distancePower = 2;
			cells = false;
			angular = false;
//j			coefficients->Add(1};
//j			coefficients->Add(0};
//j			coefficients->Add(0};
//j			coefficients->Add(0};
			random = rand();
			results = Array<POINT>::Build(0, 0);
		}
		
		public: /*virtual*/ void  setCoefficient(int c, float v)
		{
			coefficients[c] = v;
		}
		
		public: /*virtual*/ float getCoefficient(int c)
		{
			return coefficients[c];
		}
		
		private: float checkCube(float x, float y, int cubeX, int cubeY, Array<POINT>* results)
		{
			srand(571 * cubeX + 23 * cubeY);
			int numPoints = 3 + rand() % 4;
			numPoints = 4;
			
			for (int i = 0; i < numPoints; i++)
			{
				float px = (float) rand/*NextDouble*/();
				float py = (float) rand/*NextDouble*/();
				float dx = abs(x - px);
				float dy = abs(y - py);
				float d;
				if (distancePower == 1.0f)
					d = dx + dy;
				else if (distancePower == 2.0f)
				{
					d = (float) sqrt(dx * dx + dy * dy);
				}
				else
				{
					d = (float) pow(pow(dx, distancePower) + pow(dy, distancePower), 1 / distancePower);
				}
				
				// Insertion sort
				for (int j = 0; j < results->GetLength(); j++)
				{
					if (results[j].distance == MAXLONG)
					{
						POINT last = results[j];
						last.distance = d;
						last.x = px;
						last.y = py;
						results[j] = last;
						break;
					}
					else if (d < results[j].distance)
					{
						POINT last = results[results->GetLength() - 1];
						for (int k = results->GetLength() - 1; k > j; k--)
							results[k] = results[k - 1];
						last.distance = d;
						last.x = px;
						last.y = py;
						results[j] = last;
						break;
					}
				}
			}

			return results[1].distance;
		}
		
		public: /*virtual*/ float evaluate(float x, float y)
		{
			for (int j = 0; j < results->GetLength(); j++)
				results[j].distance = MAXLONG;
			
			int ix = (int) x;
			int iy = (int) y;
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
			for (int i = 0; i < 2; i++)
				t += coefficients[i] * results[i].distance;
			if (angular)
				t = (float) (t + atan2(fy - results[0].y, fx - results[0].x) / (2 * PI) + 0.5);
			return t;
		}
	};
#endif NOTUSED
	
	class CompositeFunction1D : Function1D
	{
		
		private: Function1D f1, f2;
		
		public: CompositeFunction1D(Function1D newf1, Function1D newf2)
		{
			f1 = newf1;
			f2 = newf2;
		}
		
		public: /*virtual*/ float evaluate(float v)
		{
			return f1.evaluate(f2.evaluate(v));
		}
	};
	
	class CompoundFunction2D : Function2D
	{
#ifdef NOTUSED
		/*virtual*/ public: Function2D Basis
		{
			get
			{
				return basis;
			}
			
			set
			{
				basis = value;
			}
		}
#endif NOTUSED
		
		protected: /*internal*/ Function2D basis;
		
		public: CompoundFunction2D(Function2D newbasis)
		{
			basis = newbasis;
		}
		public: float evaluate(float param1, float param2);
	};

	
	class FBM : Function2D
	{
#ifdef NOTUSED
		/*virtual*/ public: Function2D Basis
		{
			set
			{
				basis = value;
			}
			
		}
		/*virtual*/ public: Function2D BasisType
		{
			get
			{
				return basis;
			}
			
		}
#endif NOTUSED
		
		protected: /*internal*/ float* exponents;
		protected: /*internal*/ float H;
		protected: /*internal*/ float lacunarity;
		protected: /*internal*/ float octaves;
		protected: /*internal*/ Function2D basis;
		
		public: FBM(float H, float lacunarity, float octaves):this(H, lacunarity, octaves, new Noise())
		{
		}
		
		public: FBM(float newH, float newlacunarity, float newoctaves, Function2D newbasis)
		{
			H = newH;
			lacunarity = newlacunarity;
			octaves = newoctaves;
			basis = newbasis;
			
			exponents = new float[(int) octaves + 1];
			float frequency = 1.0f;
			for (int i = 0; i <= (int) octaves; i++)
			{
				exponents[i] = (float) pow(frequency, - H);
				frequency *= lacunarity;
			}
		}
		
		public: /*virtual*/ float evaluate(float x, float y)
		{
			float value_Renamed = 0.0f;
			float remainder;
			int i;
			
			// to prevent "cascading" effects
			x += 371;
			y += 529;
			
			for (i = 0; i < (int) octaves; i++)
			{
				value_Renamed += basis.evaluate(x, y) * exponents[i];
				x *= lacunarity;
				y *= lacunarity;
			}
			
			remainder = octaves - (int) octaves;
			if (remainder != 0)
				value_Renamed += remainder * basis.evaluate(x, y) * exponents[i];
			
			return value_Renamed;
		}
	};
	
	class FFT
	{
		
		// Weighting factors
		protected: /*internal*/ float* w1;
		protected: /*internal*/ float* w2;
		protected: /*internal*/ float* w3;
		
		public: FFT(int logN)
		{
			// Prepare the weighting factors
			w1 = new float[logN];
			w2 = new float[logN];
			w3 = new float[logN];
			int N = 1;
			for (int k = 0; k < logN; k++)
			{
				N <<= 1;
				double angle = (- 2.0) * PI / N;
				w1[k] = (float) sin(0.5 * angle);
				w2[k] = (- 2.0f) * w1[k] * w1[k];
				w3[k] = (float) sin(angle);
			}
		}
		
		private: void  scramble(int n, float* real, float* imag)
		{
			int j = 0;
			
			for (int i = 0; i < n; i++)
			{
				if (i > j)
				{
					float t;
					t = real[j];
					real[j] = real[i];
					real[i] = t;
					t = imag[j];
					imag[j] = imag[i];
					imag[i] = t;
				}
				int m = n >> 1;
				while (j >= m && m >= 2)
				{
					j -= m;
					m >>= 1;
				}
				j += m;
			}
		}
		
		private: void  butterflies(int n, int logN, int direction, float* real, float* imag)
		{
			int N = 1;
			
			for (int k = 0; k < logN; k++)
			{
				float w_re, w_im, wp_re, wp_im, temp_re, temp_im, wt;
				int half_N = N;
				N <<= 1;
				wt = direction * w1[k];
				wp_re = w2[k];
				wp_im = direction * w3[k];
				w_re = 1.0f;
				w_im = 0.0f;
				for (int offset = 0; offset < half_N; offset++)
				{
					for (int i = offset; i < n; i += N)
					{
						int j = i + half_N;
						float re = real[j];
						float im = imag[j];
						temp_re = (w_re * re) - (w_im * im);
						temp_im = (w_im * re) + (w_re * im);
						real[j] = real[i] - temp_re;
						real[i] += temp_re;
						imag[j] = imag[i] - temp_im;
						imag[i] += temp_im;
					}
					wt = w_re;
					w_re = wt * wp_re - w_im * wp_im + w_re;
					w_im = w_im * wp_re + wt * wp_im + w_im;
				}
			}
			if (direction == - 1)
			{
				float nr = 1.0f / n;
				for (int i = 0; i < n; i++)
				{
					real[i] *= nr;
					imag[i] *= nr;
				}
			}
		}
		
		public: /*virtual*/ void  transform1D(float* real, float* imag, int logN, int n, bool forward)
		{
			scramble(n, real, imag);
			butterflies(n, logN, forward?1:- 1, real, imag);
		}
		
		public: /*virtual*/ void  transform2D(float* real, float* imag, int cols, int rows, bool forward)
		{
			int log2cols = log2(cols);
			int log2rows = log2(rows);
			int n = max(rows, cols);
			float* rtemp = new float[n];
			float* itemp = new float[n];
			
			// FFT the rows
			for (int y = 0; y < rows; y++)
			{
				int offset = y * cols;
				Array.Copy(real, offset, rtemp, 0, cols);
				Array.Copy(imag, offset, itemp, 0, cols);
				transform1D(rtemp, itemp, log2cols, cols, forward);
				Array.Copy(rtemp, 0, real, offset, cols);
				Array.Copy(itemp, 0, imag, offset, cols);
			}
			
			// FFT the columns
			for (int x = 0; x < cols; x++)
			{
				int index = x;
				for (int y = 0; y < rows; y++)
				{
					rtemp[y] = real[index];
					itemp[y] = imag[index];
					index += cols;
				}
				transform1D(rtemp, itemp, log2rows, rows, forward);
				index = x;
				for (int y = 0; y < rows; y++)
				{
					real[index] = rtemp[y];
					imag[index] = itemp[y];
					index += cols;
				}
			}
		}
		
		private: int log2(int n)
		{
			int m = 1;
			int log2n = 0;
			
			while (m < n)
			{
				m *= 2;
				log2n++;
			}
			return m == n?log2n:- 1;
		}
	};
	
	class FractalSumFunction:CompoundFunction2D
	{
		
		private: float octaves = 1.0f;
		
		public: FractalSumFunction(Function2D basis):base(basis)
		{
		}
		
		public: /*override*/ float evaluate(float x, float y)
		{
			float t = 0.0f;
			
			for (float f = 1.0f; f <= octaves; f *= 2)
				t += basis.evaluate(f * x, f * y) / f;
			return t;
		}
	};
	
	class ImageFunction2D : Function2D
	{
#ifdef NOTUSED
		/*virtual*/ public: int EdgeAction
		{
			get
			{
				return edgeAction;
			}
			
			set
			{
				edgeAction = value;
			}
		}
		/*virtual*/ public: int Width
		{
			get
			{
				return width;
			}
		}
		/*virtual*/ public: int Height
		{
			get
			{
				return height;
			}
		}
		/*virtual*/ public: int[] Pixels
		{
			get
			{
				return pixels;
			}
		}
#endif NOTUSED
		
		public: const int ZERO = 0;
		public: const int CLAMP = 1;
		public: const int WRAP = 2;
		
		protected: /*internal*/ int[] pixels;
		protected: /*internal*/ int width;
		protected: /*internal*/ int height;
		protected: /*internal*/ int edgeAction = ZERO;
		protected: /*internal*/ bool alpha = false;
		
		public: ImageFunction2D(System::Drawing::Image image):this(image, false)
		{
		}
		
		public: ImageFunction2D(System::Drawing::Image image, bool alpha):this(image, ZERO, alpha)
		{
		}
		
		public: ImageFunction2D(System::Drawing::Image image, int edgeAction, bool alpha)
		{
			PixelGrabber pg = new PixelGrabber(image, 0, 0, -1, -1, null, 0, -1);
			try
			{
				pg.CapturePixels();
			}
			catch (System::Threading.ThreadInterruptedException e)
			{
				throw new System::SystemException("interrupted waiting for pixels!");
			}
			if ((pg.status() & ImageObserver.ABORT) != 0)
			{
				throw new System::SystemException("image fetch aborted");
			}
			init((int[]) pg.Pixels, pg.Width(), pg.Height(), edgeAction, alpha);
		}
		
		public: ImageFunction2D(int[] pixels, int width, int height, int edgeAction, bool alpha)
		{
			init(pixels, width, height, edgeAction, alpha);
		}
		
		public: /*virtual*/ void newinit(int[] newpixels, int newwidth, int newheight, int enewdgeAction, bool alpha)
		public: /*virtual*/ void newinit(int[] newpixels, int newwidth, int newheight, int newedgeAction, bool alpha)
		{
			pixels = newpixels;
			width = newwidth;
			height = newheight;
			edgeAction = newedgeAction;
			alpha = newalpha;
		}
		
		public: /*virtual*/ float evaluate(float x, float y)
		{
			int ix = (int) x;
			int iy = (int) y;
			if (edgeAction == WRAP)
			{
				ix = (ix&  width);
				iy = (iy&  height);
			}
			else if (ix < 0 || iy < 0 || ix >= width || iy >= height)
			{
				if (edgeAction == ZERO)
					return 0;
				if (ix < 0)
					ix = 0;
				else if (ix >= width)
					ix = width - 1;
				if (iy < 0)
					iy = 0;
				else if (iy >= height)
					iy = height - 1;
			}
			return alpha?((pixels[iy * width + ix] >> 24) & 0xff) / 255.0f:PixelUtils.brightness(pixels[iy * width + ix]) / 255.0f;
		}
	};
	
	class MarbleFunction:CompoundFunction2D
	{
		
		public: MarbleFunction():base(new TurbulenceFunction(new Noise(), 6))
		{
		}
		
		public: MarbleFunction(Function2D basis):base(basis)
		{
		}
		
		public: /*override*/ float evaluate(float x, float y)
		{
			return (float) pow(0.5 * (sin(8.0 * basis.evaluate(x, y)) + 1), 0.77);
		}
	};
	
	class MathFunction1D : Function1D
	{
		
		public: const int SIN = 1;
		public: const int COS = 2;
		public: const int TAN = 3;
		public: const int SQRT = 4;
		public: const int ASIN = - 1;
		public: const int ACOS = - 2;
		public: const int ATAN = - 3;
		public: const int SQR = - 4;
		
		private: int operation;
		
		public: MathFunction1D(int newoperation)
		{
			operation = newoperation;
		}
		
		public: /*virtual*/ float evaluate(float v)
		{
			switch (operation)
			{
				
				case SIN: 
					return (float) sin(v);
				
				case COS: 
					return (float) cos(v);
				
				case TAN: 
					return (float) tan(v);
				
				case SQRT: 
					return (float) sqrt(v);
				
				case ASIN: 
					return (float) asin(v);
				
				case ACOS: 
					return (float) acos(v);
				
				case ATAN: 
					return (float) atan(v);
				
				case SQR: 
					return v * v;
				}
			return v;
		}
	};
	
	
	class RidgedFBM : Function2D
	{
		
		public: /*virtual*/ float evaluate(float x, float y)
		{
			return 1 - abs(Noise.noise2(x, y));
		}
	};
	
	/// Sparse Convolution Noise. This is computationally very expensive, but worth it.
	class SCNoise : Function1D, Function2D, Function3D
	{
		
		private: static System::Random randomGenerator = new System::Random();
		
		public: /*virtual*/ float evaluate(float x)
		{
			return evaluate(x, .1f);
		}
		
		public: /*virtual*/ float evaluate(float x, float y)
		{
			int i, j, h, n;
			int ix, iy;
			float sum = 0;
			float fx, fy, dx, dy, distsq;
			
			if (impulseTab == null)
				impulseTab = impulseTabInit(665);
			
			ix = floor(x); fx = x - ix;
			iy = floor(y); fy = y - iy;
			
			/* Perform the sparse convolution. */
			int m = 2;
			for (i = - m; i <= m; i++)
			{
				for (j = - m; j <= m; j++)
				{
					/* Compute voxel hash code. */
					h = perm[(ix + i + perm[(iy + j) & TABMASK]) & TABMASK];
					
					for (n = NIMPULSES; n > 0; n--, h = (h + 1) & TABMASK)
					{
						/* Convolve filter and impulse. */
						int h4 = h * 4;
						dx = fx - (i + impulseTab[h4++]);
						dy = fy - (j + impulseTab[h4++]);
						distsq = dx * dx + dy * dy;
						sum += catrom2(distsq) * impulseTab[h4];
					}
				}
			}
			
			return sum / NIMPULSES;
		}
		
		public: /*virtual*/ float evaluate(float x, float y, float z)
		{
			int i, j, k, h, n;
			int ix, iy, iz;
			float sum = 0;
			float fx, fy, fz, dx, dy, dz, distsq;
			
			if (impulseTab == null)
				impulseTab = impulseTabInit(665);
			
			ix = floor(x); fx = x - ix;
			iy = floor(y); fy = y - iy;
			iz = floor(z); fz = z - iz;
			
			/* Perform the sparse convolution. */
			int m = 2;
			for (i = - m; i <= m; i++)
			{
				for (j = - m; j <= m; j++)
				{
					for (k = - m; k <= m; k++)
					{
						/* Compute voxel hash code. */
						h = perm[(ix + i + perm[(iy + j + perm[(iz + k) & TABMASK]) & TABMASK]) & TABMASK];
						
						for (n = NIMPULSES; n > 0; n--, h = (h + 1) & TABMASK)
						{
							/* Convolve filter and impulse. */
							int h4 = h * 4;
							dx = fx - (i + impulseTab[h4++]);
							dy = fy - (j + impulseTab[h4++]);
							dz = fz - (k + impulseTab[h4++]);
							distsq = dx * dx + dy * dy + dz * dz;
							sum += catrom2(distsq) * impulseTab[h4];
						}
					}
				}
			}
			
			return sum / NIMPULSES;
		}
		
		public: short[] perm = new short[]{225, 155, 210, 108, 175, 199, 221, 144, 203, 116, 70, 213, 69, 158, 33, 252, 5, 82, 173, 133, 222, 139, 174, 27, 9, 71, 90, 246, 75, 130, 91, 191, 169, 138, 2, 151, 194, 235, 81, 7, 25, 113, 228, 159, 205, 253, 134, 142, 248, 65, 224, 217, 22, 121, 229, 63, 89, 103, 96, 104, 156, 17, 201, 129, 36, 8, 165, 110, 237, 117, 231, 56, 132, 211, 152, 20, 181, 111, 239, 218, 170, 163, 51, 172, 157, 47, 80, 212, 176, 250, 87, 49, 99, 242, 136, 189, 162, 115, 44, 43, 124, 94, 150, 16, 141, 247, 32, 10, 198, 223, 255, 72, 53, 131, 84, 57, 220, 197, 58, 50, 208, 11, 241, 28, 3, 192, 62, 202, 18, 215, 153, 24, 76, 41, 15, 179, 39, 46, 55, 6, 128, 167, 23, 188, 106, 34, 187, 140, 164, 73, 112, 182, 244, 195, 227, 13, 35, 77, 196, 185, 26, 200, 226, 119, 31, 123, 168, 125, 249, 68, 183, 230, 177, 135, 160, 180, 12, 1, 243, 148, 102, 166, 38, 238, 251, 37, 240, 126, 64, 74, 161, 40, 184, 149, 171, 178, 101, 66, 29, 59, 146, 61, 254, 107, 42, 86, 154, 4, 236, 232, 120, 21, 233, 209, 45, 98, 193, 114, 78, 19, 206, 14, 118, 127, 48, 79, 147, 85, 30, 207, 219, 54, 88, 234, 190, 122, 95, 67, 143, 109, 137, 214, 145, 93, 92, 100, 245, 0, 216, 186, 60, 83, 105, 97, 204, 52};
		
		private: const int TABSIZE = 256;
		private: static readonly int TABMASK = (TABSIZE - 1);
		private: const int NIMPULSES = 3;
		
		private: static float* impulseTab;
		
		public: static int floor(float x)
		{
			int ix = (int) x;
			if (x < 0 && x != ix)
				return ix - 1;
			return ix;
		}
		
		private: const int SAMPRATE = 100; /* table entries per unit distance */
		private: static readonly int NENTRIES = (4 * SAMPRATE + 1);
		private: static float* table;
		
		public: /*virtual*/ float catrom2(float d)
		{
			float x;
			int i;
			
			if (d >= 4)
				return 0;
			
			if (table == null)
			{
				table = new float[NENTRIES];
				for (i = 0; i < NENTRIES; i++)
				{
					x = i / (float) SAMPRATE;
					x = (float) sqrt(x);
					if (x < 1)
						table[i] = 0.5f * (2 + x * x * (- 5 + x * 3));
					else
						table[i] = 0.5f * (4 + x * (- 8 + x * (5 - x)));
				}
			}
			
			d = d * SAMPRATE + 0.5f;
			i = floor(d);
			if (i >= NENTRIES)
				return 0;
			return table[i];
		}
		
		/*internal*/ static float* impulseTabInit(int seed)
		{
			float* impulseTab = new float[TABSIZE * 4];
			
			for (int i = 0; i < TABSIZE; i++)
			{
				impulseTab[i++] = (float) rand/*NextDouble*/;
				impulseTab[i++] = (float) rand/*NextDouble*/;
				impulseTab[i++] = (float) rand/*NextDouble*/;
				impulseTab[i++] = 1.0f - 2.0f * (float) rand/*NextDouble*/;
			}
			
			return impulseTab;
		}
	};
	
	class TurbulenceFunction:CompoundFunction2D
	{
#ifdef NOTUSED
		/*virtual*/ public: float Octaves
		{
			get
			{
				return octaves;
			}
			
			set
			{
				octaves = value;
			}
		}
#endif NOTUSED
		
		private: float octaves;
		
		public: TurbulenceFunction(Function2D basis, float newoctaves):base(basis)
		{
			octaves = newoctaves;
		}
		
		public: /*override*/ float evaluate(float x, float y)
		{
			float t = 0.0f;
			
			for (float f = 1.0f; f <= octaves; f *= 2)
				t += abs(basis.evaluate(f * x, f * y)) / f;
			return t;
		}
	};
	
	class VLNoise : Function2D
	{
#ifdef NOTUSED
		/*virtual*/ public: float Distortion
		{
			get
			{
				return distortion;
			}
			
			set
			{
				distortion = value;
			}
		}
#endif NOTUSED
		
		private: float distortion = 10.0f;
		
		public: /*virtual*/ float evaluate(float x, float y)
		{
			float ox = Noise.noise2(x + 0.5f, y) * distortion;
			float oy = Noise.noise2(x, y + 0.5f) * distortion;
			return Noise.noise2(x + ox, y + oy);
		}
	};
#endif NOTUSEDATALL
}
