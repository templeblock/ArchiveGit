#include "stdafx.h"
#include "AGMatrix.h"
#include <math.h>

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

#define PIdiv180	0.0174532925199432957692369076848861

//////////////////////////////////////////////////////////////////////
static double Arctan(int X, int Y)
{
	int Quad;
	if (X > 0)
		Quad = (Y > 0) ? 4 : 1;
	else
	if (X < 0)
		Quad = (Y < 0) ? 2 : 3;
	else
		Quad = (!Y ? 5 :((Y < 0) ? 2 : 3));

	if (Quad == 5)
		return 0.0;

	if (Y < 0)
		Y = -Y;
	if (Quad > 2)
		X = -X;

	double Angle = atan2((double)Y, (double)X) * 57.29578;

	if (Quad > 2)
		Angle += 180.0;

	return Angle;
}

//////////////////////////////////////////////////////////////////////
// Multiply the current matrix with the specified matrix the return a new matrix
CAGMatrix CAGMatrix::operator* (const CAGMatrix& Matrix) const
{
	return (CAGMatrix(m_ax * Matrix.m_ax + m_ay * Matrix.m_bx,
					  m_ax * Matrix.m_ay + m_ay * Matrix.m_by,
					  m_bx * Matrix.m_ax + m_by * Matrix.m_bx,
					  m_bx * Matrix.m_ay + m_by * Matrix.m_by,
					  m_cx * Matrix.m_ax + m_cy * Matrix.m_bx + Matrix.m_cx,
					  m_cx * Matrix.m_ay + m_cy * Matrix.m_by + Matrix.m_cy));
}

//////////////////////////////////////////////////////////////////////
CAGMatrix::CAGMatrix()
{
	Unity();
}

//////////////////////////////////////////////////////////////////////
void CAGMatrix::Unity()
{
	m_ax = 1; m_ay = 0;
	m_bx = 0; m_by = 1;
	m_cx = 0; m_cy = 0;
}

//////////////////////////////////////////////////////////////////////
bool CAGMatrix::IsUnity() const
{
	return ((m_ax == 1) && (m_by == 1) && !m_ay && !m_bx && !m_cx && !m_cy);
}

//////////////////////////////////////////////////////////////////////
bool CAGMatrix::IsSimple() const
{
	return ((m_ax > 0) && (m_by > 0) && !m_ay && !m_bx);
}

//////////////////////////////////////////////////////////////////////
bool CAGMatrix::IsRotated()	const
{
	if ((m_ay <= 0.0000005) && (m_ay >= -0.0000005) &&
		(m_bx <= 0.0000005) && (m_bx >= -0.0000005) &&
		(m_ax >= 0.0) && (m_by >= 0.0))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
double CAGMatrix::GetAngle() const
{
	if ((m_ay <= 0.0000005) && (m_ay >= -0.0000005) &&
		(m_bx <= 0.0000005) && (m_bx >= -0.0000005) &&
		(m_ax >= 0.0) && (m_by >= 0.0))
		return 0.0;

	POINT Pts[3];
	Pts[0].x = 0;     	Pts[0].y = 0;
	Pts[1].x = 100000;	Pts[1].y = 0;
	Pts[2].x = 0;     	Pts[2].y = 100000;
	Transform(Pts, 3);

	double AngleX = Arctan(Pts[1].x - Pts[0].x, Pts[1].y - Pts[0].y);
	double AngleY = Arctan(Pts[2].x - Pts[0].x, Pts[2].y - Pts[0].y);

	if (AngleX > -0.1 && AngleX <= 0.1)
		AngleX = 0;
	if (AngleY > -0.1 && AngleY <= 0.1)
		AngleY = 0;

	return AngleX;
}

//////////////////////////////////////////////////////////////////////
// Return the inverse of the matrix.								//
//							1										//
// The inverse of a b is   ---	*	d -b							//
//				  c d	   det	   -c  a							//
//																	//
// The determinant is calculated as a*d - b*c						//
//////////////////////////////////////////////////////////////////////
CAGMatrix CAGMatrix::Inverse() const
{
	CAGMatrix Inverse;
	double det = m_ax * m_by - m_ay * m_bx;
	if (!det)
		Inverse = *this;
	else
	{
		det = 1.0 / det;
		Inverse.m_ax = det * m_by;
		Inverse.m_ay = det * -m_ay;
		Inverse.m_bx = det * -m_bx;
		Inverse.m_by = det * m_ax;
	}

	Inverse.m_cx = -m_cx * Inverse.m_ax + -m_cy * Inverse.m_bx;
	Inverse.m_cy = -m_cx * Inverse.m_ay + -m_cy * Inverse.m_by;

	return Inverse;
}

//////////////////////////////////////////////////////////////////////
void CAGMatrix::SetMatrix(double ax, double ay, double bx, double by, double cx, double cy)
{
	m_ax = ax;
	m_ay = ay;
	m_bx = bx;
	m_by = by;
	m_cx = cx;
	m_cy = cy;
}

//////////////////////////////////////////////////////////////////////
void CAGMatrix::Translate(double x, double y)
{
	m_cx += x;
	m_cy += y;
}

//////////////////////////////////////////////////////////////////////
void CAGMatrix::Translate(int x, int y)
{
	m_cx += x;
	m_cy += y;
}

//////////////////////////////////////////////////////////////////////
void CAGMatrix::Rotate(double AngleX, double AngleY, double x, double y)
{
	Translate(-x, -y);

	double AngleRadianX = AngleX * PIdiv180;	
	double CosX = cos(AngleRadianX);
	double SinX = sin(AngleRadianX);
	double AngleRadianY = AngleY * PIdiv180;	
	double CosY = cos(AngleRadianY);
	double SinY = sin(AngleRadianY);

	CAGMatrix Temp(CosX, SinX, -SinY, CosY);
	*this *= Temp;

	Translate(x, y);
}

//////////////////////////////////////////////////////////////////////
void CAGMatrix::Scale(double xScale, double yScale, double x, double y)
{
	Translate(-x, -y);

	m_ax *= xScale;
	m_ay *= yScale;
	m_bx *= xScale;
	m_by *= yScale;
	m_cx *= xScale;
	m_cy *= yScale;

	Translate(x, y);
}

//////////////////////////////////////////////////////////////////////
// Adjust the matrix to center the source within the destination rectangle.													//
void CAGMatrix::Center(const RECT& DestRect, const RECT& SrcRect)
{
	double scx = (double)(SrcRect.left + SrcRect.right) / 2;
	double scy = (double)(SrcRect.top + SrcRect.bottom) / 2;

	double dcx = (double)(DestRect.left + DestRect.right) / 2;
	double dcy = (double)(DestRect.top + DestRect.bottom) / 2;

	Translate(dcx - scx, dcy - scy);
}

//////////////////////////////////////////////////////////////////////
// Adjust the matrix to stretch the source within the destination rectangle
void CAGMatrix::ScaleToFit(const RECT& DestRect, const RECT& SrcRect, bool bUseSmallerFactor)
{
	double scx = (double)(SrcRect.left + SrcRect.right) / 2;
	double scy = (double)(SrcRect.top + SrcRect.bottom) / 2;

	double dcx = (double)(DestRect.left + DestRect.right) / 2;
	double dcy = (double)(DestRect.top + DestRect.bottom) / 2;

	int sw = WIDTH(SrcRect);
	int sh = HEIGHT(SrcRect);

	Translate(-scx, -scy);
	double fxScale = (!sw ? 1.0 : (double)WIDTH(DestRect) / sw);
	double fyScale = (!sh ? 1.0 : (double)HEIGHT(DestRect) / sh);
	double fScale = (bUseSmallerFactor ? min(fxScale, fyScale) : max(fxScale, fyScale));
	Scale(fScale, fScale);
	Translate(dcx, dcy);
}

//////////////////////////////////////////////////////////////////////
// Adjust the matrix to stretch the source within the destination rectangle
void CAGMatrix::StretchToFit(const RECT& DestRect, const RECT& SrcRect)
{
	double scx = (double)(SrcRect.left + SrcRect.right) / 2;
	double scy = (double)(SrcRect.top + SrcRect.bottom) / 2;

	double dcx = (double)(DestRect.left + DestRect.right) / 2;
	double dcy = (double)(DestRect.top + DestRect.bottom) / 2;

	int sw = WIDTH(SrcRect);
	int sh = HEIGHT(SrcRect);

	Translate(-scx, -scy);
	double fxScale = (!sw ? 1.0 : (double)WIDTH(DestRect) / sw);
	double fyScale = (!sh ? 1.0 : (double)HEIGHT(DestRect) / sh);
	Scale(fxScale, fyScale);
	Translate(dcx, dcy);
}

//////////////////////////////////////////////////////////////////////
void CAGMatrix::ShearX(int height, int dx)
{
	double h = sqrt((double)((height * height) + (dx * dx)));
	double b = (double)dx / h;

	m_ax += (m_ay * b);
	m_bx += (m_by * b);
	m_cx += (m_cy * b);
}

//////////////////////////////////////////////////////////////////////
void CAGMatrix::ShearY(int width, int dy)
{
	double h = sqrt((double)((width * width) + (dy * dy)));
	double a = (double)dy / h;

	m_ay += (m_ax * a);
	m_by += (m_bx * a);
	m_cy += (m_cx * a);
}

//////////////////////////////////////////////////////////////////////
// Transform a single point
void CAGMatrix::Transform(double& x, double& y, bool bTranslate) const
{
	double dx = (bTranslate ? m_cx : 0.0);
	double dy = (bTranslate ? m_cy : 0.0);

	double fx = x;
	double fy = y;
	x = fx * m_ax + fy * m_bx + dx;
	y = fx * m_ay + fy * m_by + dy;
}

//////////////////////////////////////////////////////////////////////
// Transform a single point
void CAGMatrix::Transform(const POINT& Pt, double& x, double& y) const
{
	double fx = Pt.x;
	double fy = Pt.y;
	x = fx * m_ax + fy * m_bx + m_cx;
	y = fx * m_ay + fy * m_by + m_cy;
}

//////////////////////////////////////////////////////////////////////
// Transform a single point
void CAGMatrix::Transform(const POINT& Pt, long& x, long& y) const
{
	double fx = Pt.x;
	double fy = Pt.y;
	x = dtoi(fx * m_ax + fy * m_bx + m_cx);
	y = dtoi(fx * m_ay + fy * m_by + m_cy);
}

//////////////////////////////////////////////////////////////////////
// Transform a single point
void CAGMatrix::Transform(POINT& Pt) const
{
	double fx = Pt.x;
	double fy = Pt.y;
	Pt.x = dtoi(fx * m_ax + fy * m_bx + m_cx);
	Pt.y = dtoi(fx * m_ay + fy * m_by + m_cy);
}

//////////////////////////////////////////////////////////////////////
// Transform an array of points
void CAGMatrix::Transform(POINT* pPts, int nCount, bool bTranslate) const
{
	double dx = (bTranslate ? m_cx : 0.0);
	double dy = (bTranslate ? m_cy : 0.0);

	while (nCount--)
	{
		double fx = pPts->x;
		double fy = pPts->y;
		pPts->x = dtoi(fx * m_ax + fy * m_bx + dx);
		pPts->y = dtoi(fx * m_ay + fy * m_by + dy);
		pPts++;
	}
}

//////////////////////////////////////////////////////////////////////
// Transform a single rectangle
void CAGMatrix::Transform(RECT& Rect) const
{
	if (IsSimple())
	{
		Transform((POINT&)Rect.left);
		Transform((POINT&)Rect.right);
	}
	else
		TransformBounds(Rect);
}

//////////////////////////////////////////////////////////////////////
void CAGMatrix::TransformRectToPolygon(const RECT& Rect, POINT pt[4]) const
{
	pt[0].x = Rect.left;	pt[0].y = Rect.top;
	pt[1].x = Rect.right;	pt[1].y = Rect.top;
	pt[2].x = Rect.right;	pt[2].y = Rect.bottom;
	pt[3].x = Rect.left;	pt[3].y = Rect.bottom;

	if (IsUnity())
		return;

	Transform(pt, 4);
}

//////////////////////////////////////////////////////////////////////
// Transform the specified rect to get a new ordered bounding rect
void CAGMatrix::TransformBounds(RECT& Rect) const
{
	if (IsUnity())
		return;

	POINT pt[4];
	TransformRectToPolygon(Rect, pt);

	Rect.left   = min(min(min(pt[0].x, pt[1].x), pt[2].x), pt[3].x);
	Rect.right  = max(max(max(pt[0].x, pt[1].x), pt[2].x), pt[3].x);
	Rect.top    = min(min(min(pt[0].y, pt[1].y), pt[2].y), pt[3].y);
	Rect.bottom = max(max(max(pt[0].y, pt[1].y), pt[2].y), pt[3].y);
}

//////////////////////////////////////////////////////////////////////
double CAGMatrix::TransformDistance(int iDistance) const
{
	double fx = m_ax + m_bx;
	double fy = m_ay + m_by;

	if (fx == fy)
		return fabs(fx * iDistance);

	return sqrt(((fx * fx) + (fy * fy)) / 2) * abs(iDistance);
}

//////////////////////////////////////////////////////////////////////
double CAGMatrix::TransformWidth(int iDistance) const
{
	if (!m_ay)
		return fabs(m_ax * iDistance);
	if (!m_ax)
		return fabs(m_ay * iDistance);

	return sqrt((m_ax * m_ax) + (m_ay * m_ay)) * abs(iDistance);
}

//////////////////////////////////////////////////////////////////////
double CAGMatrix::TransformHeight(int iDistance) const
{
	if (!m_by)
		return fabs(m_bx * iDistance);
	if (!m_bx)
		return fabs(m_by * iDistance);

	return sqrt((m_bx * m_bx) + (m_by * m_by)) * abs(iDistance);
}
