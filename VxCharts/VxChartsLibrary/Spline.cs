using System.Collections;
using System.Collections.ObjectModel;
using System.Windows;
using System.Collections.Generic;

// C# Variation at: http://

namespace ClassLibrary
{
	public class SplineFit
	{
		public int NumPoints { get; private set; }
//j		private int m_Scale;
		private Collection<double> Xarray = new Collection<double>();
		private Collection<double> Yarray = new Collection<double>();
		private double[] m_Slopes;

		private bool m_Solved;

		public void Init(int degree, double xMin, double xMax)
		{
			NumPoints = 0;
			m_Solved = false;
//j			m_Scale = scale;
			Xarray.Clear();
			Yarray.Clear();
			m_Slopes = null;
		}

		public void Add(double x, double y)
		{
			NumPoints++;
			m_Solved = false;
//j			if (x < 0) x = 0;
//j			if (y < 0) y = 0;
//j			if (x > m_Scale) x = m_Scale;
//j			if (y > m_Scale) y = m_Scale;
			Xarray.Add(x);
			Yarray.Add(y);
		}

		public void Build()
		{
			if (!m_Solved)
				BuildSlopes();
		}

		private void BuildSlopes()
		{
			if (NumPoints <= 0)
				return;
//j			if (m_Scale <= 0)
//j				return;

			// Compute the Deltas
			double[] deltas = new double[NumPoints];
			for (int i = 0; i < NumPoints - 1; i++)
			{
				deltas[i] = (Yarray[i + 1] - Yarray[i]) / (Xarray[i + 1] - Xarray[i]);
			}

			// Compute the Slopes
			m_Slopes = new double[NumPoints];
			for (int i = 1; i < NumPoints - 1; i++)
			{
				if (deltas[i - 1] * deltas[i] <= 0)
					m_Slopes[i] = 0;
				else
					m_Slopes[i] = 2.0 * deltas[i - 1] * deltas[i] / (deltas[i] + deltas[i - 1]);
			}

			if (NumPoints == 1)
			{
				m_Slopes[0] = deltas[0];
			}
			else
			if (NumPoints == 2)
			{
				m_Slopes[0] = deltas[0];
				m_Slopes[1] = deltas[0];
			}
			else
			{
				if (deltas[0] == 0.0)
					m_Slopes[0] = 0.0;
				else
					m_Slopes[0] = 2.0 * deltas[0] * deltas[0] / (m_Slopes[1] + deltas[0]);

				if (deltas[NumPoints - 2] == 0.0)
					m_Slopes[NumPoints - 1] = 0.0;
				else
					m_Slopes[NumPoints - 1] = 2.0 * deltas[NumPoints - 2] * deltas[NumPoints - 2] / (m_Slopes[NumPoints - 2] + deltas[NumPoints - 2]);
			}

			m_Solved = true;
		}
#if NOTUSED
		private void Spline(Collection<Point> Points, int NumPoints, int nScale, Collection<Point> SplinePoints, int nSplinePoints)
		{
			// Compute the SplinePoints
			for (int i = 0; i < nSplinePoints; i++)
			{
				double x = (i * nScale);
				x /= (nSplinePoints - 1);
				double y = GetValue(x);
				SplinePoints.Add(new Point(x, y));
			}
		}
#endif
		public double GetValue(double x)
		{
			if (NumPoints <= 0)
				return 0;

			Build();

			int lo = 0;
			int hi = NumPoints - 1;
			while (hi - lo > 1)
			{
				int mid = (hi + lo) / 2;
				if (Xarray[mid] > x)
					hi = mid;
				else
					lo = mid;
			}

			double x0 = Xarray[lo];
			double y0 = Yarray[lo];
			double s0 = m_Slopes[lo];
			double x1 = Xarray[hi];
			double y1 = Yarray[hi];
			double s1 = m_Slopes[hi];

			double un = (3.0 / (x1 - x0)) * (s1 - ((y1 - y0) / (x1 - x0)));
			double u = (3.0 / (x1 - x0)) * (((y1 - y0) / (x1 - x0)) - s0);

			const double qn = 0.5;
			const double qp = -0.5;
			double y21 = (un - (qn * u)) / ((qn * qp) + 1.0);
			double y20 = (qp * y21) + u;

			double h = x1 - x0;
			double a = (x1 - x) / h;
			double b = (x - x0) / h;
			double value = a * y0 + b * y1 + ((a * a * a - a) * y20 + (b * b * b - b) * y21) * (h * h) * 0.16666666667;
			//j			value = Math.Round(value, 8);
			return value;
		}

		private void SolveTridiag(double[] sub, double[] diag, double[] sup, double[] b, int n)
		{
			/* solve linear system with tridiagonal n by n matrix a
			using Gaussian elimination *without* pivoting
			where
				a(i,i-1) = sub[i]  for 2<=i<=n
				a(i,i)   = diag[i] for 1<=i<=n
				a(i,i+1) = sup[i]  for 1<=i<=n-1
			(the values sub[1], sup[n] are ignored)
			right hand side vector b[1:n] is overwritten with solution 
			NOTE: 1...n is used in all arrays, 0 is unused */

			// factorization and forward substitution
			for (int i = 2; i <= n; i++)
			{
				sub[i] = sub[i] / diag[i - 1];
				diag[i] = diag[i] - sub[i] * sup[i - 1];
				b[i] = b[i] - sub[i] * b[i - 1];
			}

			b[n] = b[n] / diag[n];
			for (int i = n - 1; i >= 1; i--)
			{
				b[i] = (b[i] - sup[i] * b[i + 1]) / diag[i];
			}
		}

		public void SpLine(Collection<double> Xarray, Collection<double> Yarray)
		{
			int np = Xarray.Count;
			if (np <= 1)
				return;

			double[] h = new double[np];
			for (int i = 1; i <= np - 1; i++)
				h[i] = Xarray[i] - Xarray[i - 1];

			double[] a = new double[np];
			if (np > 2)
			{
				double[] sub = new double[np - 1];
				double[] diag = new double[np - 1];
				double[] sup = new double[np - 1];
				for (int i = 1; i <= np - 2; i++)
				{
					diag[i] = (h[i] + h[i + 1]) / 3;
					sup[i] = h[i + 1] / 6;
					sub[i] = h[i] / 6;
					a[i] = (Yarray[i + 1] - Yarray[i]) / h[i + 1] -
						   (Yarray[i] - Yarray[i - 1]) / h[i];
				}

				// SolveTridiag is a support function, see Marco Roello's original code
				// for more information at 
				// http://www.codeproject.com/useritems/SplineInterpolation.asp
				SolveTridiag(sub, diag, sup, a, np - 2);
			}

			// note that a[0] = a[np-1] = 0
			// draw
			double oldx = Xarray[0];
			double oldy = Yarray[0];
//			g.drawLine(oldx, oldy, oldx, oldy);
			const int precision = 10;
			for (int i = 1; i <= np - 1; i++)
			{
				// loop over intervals between nodes
				for (int j = 1; j <= precision; j++)
				{
					double x1 = (h[i] * j) / precision;
					double x2 = h[i] - x1;
					double y = ((-a[i - 1] / 6 * (x2 + h[i]) * x1 + Yarray[i - 1]) * x2 +
						 (-a[i] / 6 * (x1 + h[i]) * x2 + Yarray[i]) * x1) / h[i];
					double x = Xarray[i - 1] + x1;
//					g.drawLine(oldx, oldy, x, y);
					oldx = x;
					oldy = y;
				}
			}
		}
	}
}
