#include "stdafx.h"
#include "spline.h"

static void SplineCompute(double* Xarray, double* Yarray, double* slopes, int nPoints, double x, double *y);

/////////////////////////////////////////////////////////////////////////////
void Spline(LPPOINT Points, int nPoints, int nScale, LPPOINT SplinePoints, int nSplinePoints)
{
	if (!Points || !SplinePoints)
		return;

	if (nPoints <= 0 || nSplinePoints <= 0 || nScale <= 0)
		return;

	double* Xarray = new double[nPoints];
	double* Yarray = new double[nPoints];
	double* slopes = new double[nPoints];
	double* deltas = new double[nPoints];
	if (!Xarray || !Yarray || !slopes || !deltas)
	{
		delete [] Xarray;
		delete [] Yarray;
		delete [] slopes;
		delete [] deltas;
		return;
	}

	// Copy the points to the double arrays
	int i;
	for (i = 0; i < nPoints; i++)
	{
		POINT Point = Points[i];
		if (Point.x < 0) Point.x = 0;
		if (Point.y < 0) Point.y = 0;
		if (Point.x > nScale) Point.x = nScale;
		if (Point.y > nScale) Point.y = nScale;
		Xarray[i] = Point.x;
		Yarray[i] = Point.y;
	}

	// Compute the Deltas
	for (i = 0; i < nPoints-1; i++)
	{
		deltas[i] = (Yarray[i+1]-Yarray[i])/(Xarray[i+1]-Xarray[i]);
	}

	// Compute the Slopes
	for (i = 1; i < nPoints-1; i++)
	{
		if (deltas[i-1]*deltas[i] <=0)
			slopes[i] = 0;
		else 
			slopes[i] = 2.0*deltas[i-1]*deltas[i]/(deltas[i]+deltas[i-1]);
	}
	
	if (nPoints == 2)
	{
		slopes[0] = deltas[0];
		slopes[1] = deltas[0];
	}
	else
	{
		if (deltas[0] == 0.0)
			slopes[0] = 0.0;
		else
			slopes[0] = 2.0*deltas[0]*deltas[0]/(slopes[1]+deltas[0]);
		
		if (deltas[nPoints-2] == 0.0)
			slopes[nPoints-1] = 0.0;
		else
			slopes[nPoints-1] = 2.0*deltas[nPoints-2]*deltas[nPoints-2]/(slopes[nPoints-2]+deltas[nPoints-2]);
	}

	// Compute the SplinePoints
	for (i = 0; i < nSplinePoints; i++)
	{
		double x = (i * nScale);
		x /= (nSplinePoints - 1);
		double y;
		SplineCompute(Xarray, Yarray, slopes, nPoints, x, &y);
		SplinePoints[i].x = (long)(x + (x>0 ? 0.5 : -0.5));
		SplinePoints[i].y = (long)(y + (y>0 ? 0.5 : -0.5));
	}

	delete [] Xarray;
	delete [] Yarray;
	delete [] slopes;
	delete [] deltas;
}

/////////////////////////////////////////////////////////////////////////////
static void SplineCompute(double* Xarray, double* Yarray, double* slopes, int nPoints, double x, double *y)
{
	long lo = 0;
	long hi = nPoints-1;
	while (hi - lo > 1)
	{
		long mid = (hi + lo) >> 1;
		if (Xarray[mid] > x)
			hi = mid;
		else
			lo = mid;
	}	

	double x0 = Xarray[lo];
	double y0 = Yarray[lo];
	double s0 = slopes[lo];
	double x1 = Xarray[hi];
	double y1 = Yarray[hi];
	double s1 = slopes[hi];

	double qn = 0.5;
	double q = -0.5;
	double un = (3.0 / (x1-x0)) * (s1 - ((y1-y0)/(x1-x0)));
	double u  = (3.0 / (x1-x0)) * (((y1-y0)/(x1-x0)) - s0);

	double y2[2];
	y2[1] = (un - (qn * u)) / ((qn * q) + 1.0);
	y2[0] = (q * y2[1]) + u;
	
	double h = x1 - x0;
	double a = (x1 - x) / h;
	double b = (x - x0) / h;
	*y = a*y0+b*y1+((a*a*a-a)*y2[0]+(b*b*b-b)*y2[1])*(h*h)*0.16666666667;
}

/*
static void SplinePrep(double* x, double* y, double* y2, int n, double* u);
static void SplineCompute(double* x, double* y, double* y2, int n, double X, double* Y);

/////////////////////////////////////////////////////////////////////////////
void Spline(LPPOINT Points, int nPoints, int nScale, LPPOINT SplinePoints, int nSplinePoints)
{
	if (nPoints <= 0 || nSplinePoints <= 0 || nScale <= 0)
		return;

	double* Xarray = new double[nPoints];
	double* Yarray = new double[nPoints];
	double* YD2array = new double[nPoints];
	double* TempArray = new double[nSplinePoints];
	if (!Xarray || !Yarray || !YD2array || !TempArray)
		return;

	// Copy the points to the double arrays
	int i;
	for (i = 0; i < nPoints; i++)
	{
		Xarray[i] = Points[i].x;
		Yarray[i] = Points[i].y;
	}

	SplinePrep(Xarray, Yarray, YD2array, nPoints, TempArray);

	// Compute the SplinePoints
	for (i = 0; i < nSplinePoints; i++)
	{
		double x = (i * nScale);
		x /= (nSplinePoints - 1);
		double y;
		SplineCompute(Xarray, Yarray, YD2array, nPoints, x, &y);
		SplinePoints[i].x = (long)(x + (x>0 ? 0.5 : -0.5));
		SplinePoints[i].y = (long)(y + (y>0 ? 0.5 : -0.5));
	}

	delete [] Xarray;
	delete [] Yarray;
	delete [] YD2array;
	delete [] TempArray;
}

/////////////////////////////////////////////////////////////////////////////
// the spline routine adapted from the NRC code
void SplinePrep(double* x, double* y, double* y2, int n, double* u )
{
	double yp1 = (y[1]-y[0]) / (x[1]-x[0]);
	double ypn = (y[n-1]-y[n-2]) / (x[n-1]-x[n-2]);

	if (yp1 > 0.99e30)
		y2[0] = u[0] = 0.0;
	else
	{
		y2[0] = -0.5;
		u[0] = (3.0/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yp1);
	}

	for (int i=1; i < n-1; i++)
	{
		double sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
		double p = sig*y2[i-1]+2.0;
		y2[i] = (sig-1.0)/p;
		u[i] = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
		u[i] = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}

	double qn, un;
	if (ypn > 0.99e30)
		qn = un = 0.0;
	else
	{
		qn = 0.5;
		un = (3.0/(x[n-1]-x[n-2])) * (ypn-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
	}

	y2[n-1] = (un-qn*u[n-2]) / (qn*y2[n-2]+1.0);
	for (int k = n-2; k >= 0; k--)
		y2[k] = y2[k]*y2[k+1] + u[k];
}

/////////////////////////////////////////////////////////////////////////////
// The splint code adapted from the NRC code
void SplineCompute(double* x, double* y, double* y2, int n, double X, double* Y)
{
	int klo = 0;
	int khi = n-1;
	while (khi-klo > 1)
	{
		int k = (khi+klo) >> 1;
		if (x[k] > X)
			khi = k;
		else
			klo = k;
	}

	double h = x[khi] - x[klo];
	double a = (x[khi]-X) / h;
	double b = (X-x[klo]) / h;
	*Y = a*y[klo] + b*y[khi] + ((a*a*a-a)*y2[klo]+(b*b*b-b)*y2[khi])*(h*h)/6.0;
}
*/
