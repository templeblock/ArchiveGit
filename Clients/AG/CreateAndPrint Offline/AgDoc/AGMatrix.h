#pragma once

class CAGMatrix
{
public:
	CAGMatrix();
	CAGMatrix(double ax, double ay, double bx, double by, double cx = 0, double cy = 0)
		{ m_ax = ax; m_ay = ay; m_bx = bx; m_by = by; m_cx = cx; m_cy = cy; }
	CAGMatrix operator* (const CAGMatrix& Matrix) const;
	void operator*= (const CAGMatrix& Matrix)
		{ *this = *this * Matrix; }
	int operator!= (const CAGMatrix& Matrix) const
		{ return !operator==(Matrix); }
	int operator== (const CAGMatrix& Matrix) const
		{
			return	m_ax == Matrix.m_ax &&
					m_ay == Matrix.m_ay &&
					m_bx == Matrix.m_bx &&
					m_by == Matrix.m_by &&
					m_cx == Matrix.m_cx &&
					m_cy == Matrix.m_cy;
		}
	double GetAngle() const;
	CAGMatrix Inverse() const;
	void Unity();
	bool IsUnity() const;
	bool IsSimple() const;
	bool IsRotated() const;
	void Rotate(double xAngle, double yAngle, double x = 0, double y = 0);
	void Scale(double xScale, double yScale, double x = 0, double y = 0);
	void Center(const RECT& DestRect, const RECT& SrcRect);
	void ScaleToFit(const RECT& DestRect, const RECT& SrcRect, bool bUseSmallerFactor = true);
	void StretchToFit(const RECT& DestRect, const RECT& SrcRect);
	void ShearX(int height, int dx);
	void ShearY(int width, int dy);
	void Translate(double x, double y);
	void Translate(int x, int y);
	void SetMatrix(double ax, double ay, double bx, double by, double cx = 0, double cy = 0);
	void Transform(double& x, double& y, bool bTranslate) const;
	void Transform(POINT* pPts, int nCount, bool bTranslate = true) const;
	void Transform(const POINT& Pt, double& x, double& y) const;
	void Transform(const POINT& Pt, long& x, long& y) const;
	void Transform(POINT& Pt) const;
	void Transform(RECT& Rect) const;
	void TransformRectToPolygon(const RECT& Rect, POINT pt[4]) const;
	double TransformDistance(int iDistance) const;
	double TransformWidth(int iWidth) const;
	double TransformHeight(int iHeight) const;
private:
	void TransformBounds(RECT& Rect) const;

public:
	double m_ax;	// ax ay 0
	double m_ay;	// bx by 0
	double m_bx;	// cx cy 1
	double m_by;
	double m_cx;
	double m_cy;
};
