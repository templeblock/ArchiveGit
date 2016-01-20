//==========================================================================//
//==========================================================================//
#include "stdafx.h"
#include "AGMatrix.h"

#include <math.h>

#ifdef _AFX
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#endif

#define PI	3.1415926535897932
#define ABS(Value)((Value) > 0 ?(Value) :(-(Value)))

int Arctan(int X, int Y, int xScale, int yScale)
{
	int Angle;
	int Quad;
	if (X > 0)
		Quad = (Y > 0) ? 4 : 1;
	else if (X < 0)
		Quad = (Y < 0) ? 2 : 3;
	else
		Quad = ((Y == 0) ? 5 :((Y < 0) ? 2 : 3));

	if (Quad == 5)
		Angle = 0;
	else
	{
		Y = ABS(Y);
		if (Quad > 2)
			X = -X;
		if (yScale == 0)
			Angle = 0;
		else if (xScale == 0)
			Angle = 900;
		else
		{
			double datan = atan2((double)Y *(double)xScale,
			 (double)X *(double)yScale);
			datan *= (57.29578 * 10);
			Angle = (int)(datan + ((datan >= 0) ? 0.5 : -0.5));
		}
		if (Quad > 2)
			Angle += 1800;
	}
	return (Angle);
}

//--------------------------------------------------------------------------//
// Double to int with rounding. 											//
//--------------------------------------------------------------------------//
inline int dtoi(double x)
{
	return ((int)(x > 0 ? x + 0.5 : x - 0.5));
}

//--------------------------------------------------------------------------//
// Multiply the current matrix with the specified matrix the return a new	//
// matrix.																	//
//--------------------------------------------------------------------------//
CAGMatrix CAGMatrix::operator *(const CAGMatrix &Matrix) const
{
	return (CAGMatrix(m_11 * Matrix.m_11 + m_12 * Matrix.m_21,
					   m_11 * Matrix.m_12 + m_12 * Matrix.m_22,
					   m_21 * Matrix.m_11 + m_22 * Matrix.m_21,
					   m_21 * Matrix.m_12 + m_22 * Matrix.m_22,
					   m_31 * Matrix.m_11 + m_32 * Matrix.m_21 + Matrix.m_31,
					   m_31 * Matrix.m_12 + m_32 * Matrix.m_22 + Matrix.m_32));
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGMatrix::GetRotation(int &Angle) const
{
	bool bRotated = false;
	if ((m_12 > 0.0000005) || (-m_12 > 0.0000005) ||
	 (m_21 > 0.0000005) || (-m_21 > 0.0000005) ||
	 (m_11 < 0.0) || (m_22 < 0.0))
	{
		POINT Pts[3];
		Pts[0].x = Pts[0].y = Pts[1].y = Pts[2].x = 0;
		Pts[1].x = Pts[2].y = 100000;
		Transform(Pts, 3);

		Angle = Arctan(Pts[1].x - Pts[0].x,Pts[1].y - Pts[0].y,1,1);
		int Angle2 = Arctan(Pts[2].x - Pts[0].x,Pts[2].y - Pts[0].y,1,1);
		int AngleRes = Angle2 - Angle;
		if (AngleRes < 0)
			AngleRes += 3600;
		bRotated = ((AngleRes >= 2698) && (AngleRes <= 2702));
	}
	return (bRotated);
}

//--------------------------------------------------------------------------//
// Return the inverse of the matrix.										//
//							1												//
// The inverse of a b is   ---	*	d -b									//
//				  c d	   det	   -c  a									//
//																			//
// The determinant is calculated as a*d - b*c								//
//--------------------------------------------------------------------------//
CAGMatrix CAGMatrix::Inverse() const
{
	CAGMatrix Inverse;
	double det = m_11 * m_22 - m_12 * m_21;
	if (det == 0.0)
		Inverse = *this;
	else
	{
		det = 1.0 / det;
		Inverse.m_11 = det * m_22;
		Inverse.m_12 = det * -m_12;
		Inverse.m_21 = det * -m_21;
		Inverse.m_22 = det * m_11;
	}

	Inverse.m_31 = -m_31 * Inverse.m_11 + -m_32 * Inverse.m_21;
	Inverse.m_32 = -m_31 * Inverse.m_12 + -m_32 * Inverse.m_22;

	return (Inverse);
}

//--------------------------------------------------------------------------//
// Rotate the matrix.														//
//--------------------------------------------------------------------------//
void CAGMatrix::Rotate(double Angle, double x, double y)
{
	double AngleRadian = Angle * PI / 180;	

	Translate(-x, -y);
	double CosAngle = cos(AngleRadian);
	double SinAngle = sin(AngleRadian);
	CAGMatrix Temp(CosAngle, -SinAngle, SinAngle, CosAngle);
	*this *= Temp;
	Translate(x, y);
}

//--------------------------------------------------------------------------//
// Scale the matrix.														//
//--------------------------------------------------------------------------//
void CAGMatrix::Scale(double xScale, double yScale, double x, double y)
{
	Translate(-x, -y);
	m_11 *= xScale;
	m_12 *= yScale;
	m_21 *= xScale;
	m_22 *= yScale;
	m_31 *= xScale;
	m_32 *= yScale;
	Translate(x, y);
}

//--------------------------------------------------------------------------//
// Calculate the matrix to scale and center the souce rectangle within the	//
// destination rectangle.													//
//--------------------------------------------------------------------------//
void CAGMatrix::ScaleAndCenter(const RECT &DestRect, const RECT &SrcRect,
  bool bFlip)
{
	double xScale = (double) WIDTH(DestRect) /(double) WIDTH(SrcRect);
	double yScale = (double) HEIGHT(DestRect) /(double) HEIGHT(SrcRect);
	if (xScale < yScale)
		yScale = xScale;
	else
		xScale = yScale;

	SetMatrix(xScale, 0, 0, yScale);
	RECT New = SrcRect;
	Transform(&New);
	Translate(DestRect.left - New.left + ((WIDTH(DestRect) - WIDTH(New)) / 2),
	  DestRect.top - New.top + ((HEIGHT(DestRect) - HEIGHT(New)) / 2));

	if (bFlip)
	{
		Scale(-1, -1, (DestRect.left + DestRect.right) / 2,
		 (DestRect.top + DestRect.bottom) / 2);
	}
}

//--------------------------------------------------------------------------//
// Transform the specified points.											//
//--------------------------------------------------------------------------//
void CAGMatrix::Transform(POINT *pPts, int nCount, bool bTranslate) const
{
	if (bTranslate)
	{
		while (nCount--)
		{
			int x = dtoi((double) pPts->x * m_11 + (double) pPts->y * m_21 + m_31);
			pPts->y = dtoi((double) pPts->x * m_12 + (double) pPts->y * m_22 + m_32);
			pPts->x = x;
			pPts++;
		}
	}
	else
	{
		while (nCount--)
		{
			int x = dtoi((double) pPts->x * m_11 + (double) pPts->y * m_21);
			pPts->y = dtoi((double) pPts->x * m_12 + (double) pPts->y * m_22);
			pPts->x = x;
			pPts++;
		}
	}
}

