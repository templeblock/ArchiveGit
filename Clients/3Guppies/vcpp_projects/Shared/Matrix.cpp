#include "stdafx.h"
#include "Matrix.h"
#include "Utility.h"
#include <math.h>

#define kPIdiv180	0.0174532925199432957692369076849
#define k180divPI	57.295779513082320876798154814105

//////////////////////////////////////////////////////////////////////
// Multiply the current matrix with the specified matrix the return a new matrix
CMatrix CMatrix::operator* (const CMatrix& Matrix) const
{
	return (CMatrix(m_ax * Matrix.m_ax + m_ay * Matrix.m_bx,
					  m_ax * Matrix.m_ay + m_ay * Matrix.m_by,
					  m_bx * Matrix.m_ax + m_by * Matrix.m_bx,
					  m_bx * Matrix.m_ay + m_by * Matrix.m_by,
					  m_cx * Matrix.m_ax + m_cy * Matrix.m_bx + Matrix.m_cx,
					  m_cx * Matrix.m_ay + m_cy * Matrix.m_by + Matrix.m_cy));
}

//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix()
{
	Unity();
}

//////////////////////////////////////////////////////////////////////
void CMatrix::Unity()
{
	m_ax = 1; m_ay = 0;
	m_bx = 0; m_by = 1;
	m_cx = 0; m_cy = 0;
}

//////////////////////////////////////////////////////////////////////
bool CMatrix::IsUnity() const
{
	return ((m_ax == 1) && (m_by == 1) && !m_ay && !m_bx && !m_cx && !m_cy);
}

//////////////////////////////////////////////////////////////////////
bool CMatrix::IsSimple() const
{
	return ((m_ax > 0) && (m_by > 0) && !m_ay && !m_bx);
}

//////////////////////////////////////////////////////////////////////
bool CMatrix::IsRotated()	const
{
	if ((m_ay <= 0.0000005) && (m_ay >= -0.0000005) &&
		(m_bx <= 0.0000005) && (m_bx >= -0.0000005) &&
		(m_ax >= 0.0) && (m_by >= 0.0))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
double CMatrix::GetAngle() const
{
	if ((m_ay <= 0.0000005) && (m_ay >= -0.0000005) &&
		(m_bx <= 0.0000005) && (m_bx >= -0.0000005) &&
		(m_ax >= 0.0) && (m_by >= 0.0))
		return 0.0;

	double pt0x = 0.0;
	double pt0y = 0.0;
	Transform(pt0x, pt0y, true/*bTranslate*/);

	double pt1x = 1.0;
	double pt1y = 0.0;
	Transform(pt1x, pt1y, true/*bTranslate*/);

//j	double pt2x = 0.0;
//j	double pt2y = 1.0;
//j	Transform(pt2x, pt2y, true/*bTranslate*/);

	double AngleX = atan2(pt1y - pt0y, pt1x - pt0x) * k180divPI;
//j	double AngleY = atan2(pt2y - pt0y, pt2x - pt0x) * k180divPI;

	if (AngleX > -0.1 && AngleX <= 0.1) AngleX = 0;
//j	if (AngleY > -0.1 && AngleY <= 0.1) AngleY = 0;

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
CMatrix CMatrix::Inverse() const
{
	CMatrix Inverse;
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
void CMatrix::SetMatrix(double ax, double ay, double bx, double by, double cx, double cy)
{
	m_ax = ax;
	m_ay = ay;
	m_bx = bx;
	m_by = by;
	m_cx = cx;
	m_cy = cy;
}

//////////////////////////////////////////////////////////////////////
void CMatrix::Translate(double x, double y)
{
	m_cx += x;
	m_cy += y;
}

//////////////////////////////////////////////////////////////////////
void CMatrix::Translate(int x, int y)
{
	m_cx += x;
	m_cy += y;
}

//////////////////////////////////////////////////////////////////////
void CMatrix::Rotate(double AngleX, double AngleY, double x, double y)
{
	Translate(-x, -y);

	double AngleRadianX = AngleX * kPIdiv180;	
	double CosX = cos(AngleRadianX);
	double SinX = sin(AngleRadianX);
	double AngleRadianY = AngleY * kPIdiv180;	
	double CosY = cos(AngleRadianY);
	double SinY = sin(AngleRadianY);

	CMatrix Temp(CosX, SinX, -SinY, CosY);
	*this *= Temp;

	Translate(x, y);
}

//////////////////////////////////////////////////////////////////////
void CMatrix::Scale(double xScale, double yScale, double x, double y)
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
void CMatrix::Center(const CRect& DestRect, const CRect& SrcRect)
{
	double scx = (double)(SrcRect.left + SrcRect.right) / 2;
	double scy = (double)(SrcRect.top + SrcRect.bottom) / 2;

	double dcx = (double)(DestRect.left + DestRect.right) / 2;
	double dcy = (double)(DestRect.top + DestRect.bottom) / 2;

	Translate(dcx - scx, dcy - scy);
}

//////////////////////////////////////////////////////////////////////
// Adjust the matrix to stretch the source within the destination rectangle
void CMatrix::ScaleToFit(const CRect& DestRect, const CRect& SrcRect, bool bUseSmallerFactor)
{
	double scx = (double)(SrcRect.left + SrcRect.right) / 2;
	double scy = (double)(SrcRect.top + SrcRect.bottom) / 2;

	double dcx = (double)(DestRect.left + DestRect.right) / 2;
	double dcy = (double)(DestRect.top + DestRect.bottom) / 2;

	int sw = SrcRect.Width();
	int sh = SrcRect.Height();

	Translate(-scx, -scy);
	double fxScale = (!sw ? 1.0 : (double)DestRect.Width() / sw);
	double fyScale = (!sh ? 1.0 : (double)DestRect.Height() / sh);
	double fScale = (bUseSmallerFactor ? min(fxScale, fyScale) : max(fxScale, fyScale));
	Scale(fScale, fScale);
	Translate(dcx, dcy);
}

//////////////////////////////////////////////////////////////////////
// Adjust the matrix to stretch the source within the destination rectangle
void CMatrix::StretchToFit(const CRect& DestRect, const CRect& SrcRect)
{
	double scx = (double)(SrcRect.left + SrcRect.right) / 2;
	double scy = (double)(SrcRect.top + SrcRect.bottom) / 2;

	double dcx = (double)(DestRect.left + DestRect.right) / 2;
	double dcy = (double)(DestRect.top + DestRect.bottom) / 2;

	int sw = SrcRect.Width();
	int sh = SrcRect.Height();

	Translate(-scx, -scy);
	double fxScale = (!sw ? 1.0 : (double)DestRect.Width() / sw);
	double fyScale = (!sh ? 1.0 : (double)DestRect.Height() / sh);
	Scale(fxScale, fyScale);
	Translate(dcx, dcy);
}

//////////////////////////////////////////////////////////////////////
void CMatrix::ShearX(int height, int dx)
{
	double h = sqrt((double)((height * height) + (dx * dx)));
	double b = (double)dx / h;

	m_ax += (m_ay * b);
	m_bx += (m_by * b);
	m_cx += (m_cy * b);
}

//////////////////////////////////////////////////////////////////////
void CMatrix::ShearY(int width, int dy)
{
	double h = sqrt((double)((width * width) + (dy * dy)));
	double a = (double)dy / h;

	m_ay += (m_ax * a);
	m_by += (m_bx * a);
	m_cy += (m_cx * a);
}

//////////////////////////////////////////////////////////////////////
// Transform a single point
void CMatrix::Transform(double& x, double& y, bool bTranslate) const
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
void CMatrix::Transform(const POINT& Pt, double& x, double& y) const
{
	double fx = Pt.x;
	double fy = Pt.y;
	x = fx * m_ax + fy * m_bx + m_cx;
	y = fx * m_ay + fy * m_by + m_cy;
}

//////////////////////////////////////////////////////////////////////
// Transform a single point
void CMatrix::Transform(const POINT& Pt, long& x, long& y) const
{
	double fx = Pt.x;
	double fy = Pt.y;
	x = dtoi(fx * m_ax + fy * m_bx + m_cx);
	y = dtoi(fx * m_ay + fy * m_by + m_cy);
}

//////////////////////////////////////////////////////////////////////
// Transform a single point
void CMatrix::Transform(POINT& Pt) const
{
	double fx = Pt.x;
	double fy = Pt.y;
	Pt.x = dtoi(fx * m_ax + fy * m_bx + m_cx);
	Pt.y = dtoi(fx * m_ay + fy * m_by + m_cy);
}

//////////////////////////////////////////////////////////////////////
// Transform an array of points
void CMatrix::Transform(POINT* pPts, int nCount, bool bTranslate) const
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
void CMatrix::Transform(RECT& Rect) const
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
void CMatrix::TransformRectToPolygon(const CRect& Rect, POINT pt[4]) const
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
void CMatrix::TransformBounds(RECT& Rect) const
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
double CMatrix::TransformDistance(int iDistance) const
{
	double fx = m_ax + m_bx;
	double fy = m_ay + m_by;

	if (fx == fy)
		return fabs(fx * iDistance);

	return sqrt(((fx * fx) + (fy * fy)) / 2) * abs(iDistance);
}

//////////////////////////////////////////////////////////////////////
double CMatrix::TransformWidth(int iDistance) const
{
	if (!m_ay)
		return fabs(m_ax * iDistance);
	if (!m_ax)
		return fabs(m_ay * iDistance);

	return sqrt((m_ax * m_ax) + (m_ay * m_ay)) * abs(iDistance);
}

//////////////////////////////////////////////////////////////////////
double CMatrix::TransformHeight(int iDistance) const
{
	if (!m_by)
		return fabs(m_bx * iDistance);
	if (!m_bx)
		return fabs(m_by * iDistance);

	return sqrt((m_bx * m_bx) + (m_by * m_by)) * abs(iDistance);
}
