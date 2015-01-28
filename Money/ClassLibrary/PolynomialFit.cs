using System;
using System.Diagnostics;
using System.Collections;

// C# Variation at: http://code.google.com/p/sentience/source/browse/trunk/applications/fishfood/polynomial.cs?spec=svn533&r=472

//Polynomial fit functions
//
//PolynomialFit.cs contains a class that provides an easy way to add polynomial regression 
//functionality to any application. Whether you want linear regression or a very high 
//degree polynomial regression, this class has good performance and scales seamlessly with
//the complexity of your problem.

//What is regression?
//----------------------------
//Given a number of two dimensional data points, regression will 
//fit a theoretical curve that models the data as close as possible.

//Given the general equation of the polynomial fit function:
//	y(x) = c0*x^0 + c1*x^1 + c2*x^2 + c3*x^3 + ...

//This class will calculate the coefficients
//	( c0, c1, c2, c3, ...)

//Two special cases of polynomials that everyone is familiar with are the 
//first (linear) and second (parabolic) order curves:

//	y(x) = m*x + b			(linear regression)
//	y(x) = a*x^2 + b*x + c	(parabolic regression)

//You can optionally retrieve the coefficients with the Coef() property:

//	a = polyfit.Coef(2);
//	b = polyfit.Coef(1);
//	c = polyfit.Coef(0);
// gives the coeffs for y(x) = a*x^2 + b*x + c
//or
//	m = polyfit.Coef(1);
//	n = polyfit.Coef(0);
// gives the coeffs for y(x) = m*x + b

//That's all you need to know about mathematics.

//Decide the highest power of x (the degree of the polynomial) you want to consider.
//Use 1 for linear regression and 2 for parabolic. The class limits the degree to 25.
//Higher values lead to datatype overflow, and even 10 is much higher than necessary.

//Then create an instance of the class:
//	var polyfit = new PolynomialFit(degree); // 2 is also the default value

//Then add your data points. The data points are not stored in the class,
//so no extra memory is consumed

//	double[,] data =
//	{
//		{ 0,  0 },
//		{ 1, 10 },
//		{ 2, 20 },
//	};
//
//	for (int i = data.GetLowerBound(0); i <= data.GetUpperBound(0); i++)
//		polyfit.Add(data[i,0], data[i,1]);
//	polyfit.Build();

//Finally, you can extract the interpolated data as follows:

//	Pic1.CurrentX = xMin;
//	Pic1.CurrentY = polyfit.GetValue(xMin);
//	for (int x = xMin; x < xMax; x++)
//		Pic1.Line(x, polyfit.GetValue(x));

//It is not possible to retrieve the original data from the class object.
//However, the NumPoints property tracks the number of points added.

//This class is built upon the the gauss algorithm (GaussSolve) 
//in R.Sedgewick's "Algorithms in C", Addison-Wesley, with minor modifications

//Frank Schindler, Dresden, Germany, 16.9.2000

namespace ClassLibrary
{
	public class PolynomialFit
	{
		public int NumPoints { get; private set; }

		private const int k_MaxDegree = 100;
		private const int k_MaxArray = k_MaxDegree + 1;
		private int m_Degree;
		private double m_xMin;
		private double m_xScale;
		private bool m_Solved;
		private double[] m_SumX = new double[k_MaxArray * 2];
		private double[] m_SumYX = new double[k_MaxArray];
		private double[,] m_Matrix = new double[k_MaxArray, k_MaxArray + 1];
		private double[] m_Coef = new double[k_MaxArray];

		public void Init(int degree, double xMin, double xMax)
		{
			NumPoints = 0;
			m_Degree = Math.Min(degree, k_MaxDegree);
			m_xMin = xMin;
			m_Solved = false;
			((IList)m_SumX).Clear();
			((IList)m_SumYX).Clear();
			((IList)m_Matrix).Clear();
			((IList)m_Coef).Clear();

			double dx = xMax - xMin;
			if (dx <= 0)
				m_xScale = 1;
			else
			{
				double dxMax = Math.Pow(double.MaxValue, 1.0 / (m_Degree * 2));
				m_xScale = Math.Min(dxMax / dx, 1);
			}
		}

		public void Add(double x, double y)
		{
			x -= m_xMin;
			x *= m_xScale;
			NumPoints++;
			m_Solved = false;

			double TX = 1.0;
			for (int i = 0; i <= m_Degree * 2; i++)
			{
				m_SumX[i] += TX;
				if (i <= m_Degree)
					m_SumYX[i] += TX * y;
				TX *= x;
			}
		}

		public void Build()
		{
			int degree = Math.Min(m_Degree, NumPoints - 1);
			if (!m_Solved)
				GaussSolve(degree);
		}

		public double GetValue(double x)
		{
			x -= m_xMin;
			x *= m_xScale;
			int degree = Math.Min(m_Degree, NumPoints - 1);
			if (!m_Solved)
				GaussSolve(degree);

			double value = 0.0;
			for (int i = 0; i <= degree; i++)
				value += (m_Coef[i] * Math.Pow(x, i));

			return Math.Round(value, 8);
		}

		public double Coef(int exponent)
		{
			int degree = Math.Min(m_Degree, NumPoints - 1);
			if (!m_Solved)
				GaussSolve(degree);

			return (exponent <= degree ? Math.Round(m_Coef[exponent], 8) : 0);
		}

		private void GaussSolve(int degree)
		{
			try
			{
				if (degree < 0)
					return;

				BuildMatrix(degree);

				//first triangulize the matrix
				for (int i = 0; i <= degree; i++)
				{
					//find the line with the largest abs value in this row
					int largestRow = i;
					double largestValue = double.MinValue;
					for (int j = i; j <= degree; j++)
					{
						double value = Math.Abs(m_Matrix[j, i]);
						if (largestValue < value)
						{
							largestValue = value;
							largestRow = j;
						}
					}

					//exchange the two lines
					if (i < largestRow)
					{
						for (int k = i; k <= degree + 1; k++)
							Swap(ref m_Matrix[i, k], ref m_Matrix[largestRow, k]);
					}

					//scale all following lines to have a leading zero
					for (int j = i + 1; j <= degree; j++)
					{
						double value = m_Matrix[j, i] / m_Matrix[i, i];
						m_Matrix[j, i] = 0.0;
						for (int k = i + 1; k <= degree + 1; k++)
							m_Matrix[j, k] -= m_Matrix[i, k] * value;
					}
				}

				//then substitute the coefficients
				for (int j = degree; j >= 0; j--)
				{
					double T = m_Matrix[j, degree + 1];
					for (int k = j + 1; k <= degree; k++)
						T -= m_Matrix[j, k] * m_Coef[k];

					m_Coef[j] = T / m_Matrix[j, j];
				}
			}
			catch (Exception ex)
			{
				// If there is an exception, set all of the coefficients to 0
				Debug.WriteLine(ex.Message);
				for (int i = 0; i <= degree; i++)
					m_Coef[i] = 0.0;
			}

			m_Solved = true;
		}

		private void BuildMatrix(int degree)
		{
			for (int row = 0; row <= degree; row++)
			{
				for (int col = 0; col <= degree; col++)
					m_Matrix[row, col] = m_SumX[row + col];

				m_Matrix[row, degree + 1] = m_SumYX[row];
			}
		}

		private void Swap<T>(ref T lhs, ref T rhs)
		{
			T temp = lhs;
			lhs = rhs;
			rhs = temp;
		}
	}
}
