#ifndef __AGMATRIX_H_
#define __AGMATRIX_H_

class CAGMatrix
{
public:
	CAGMatrix ()
	  { Unity (); }
	CAGMatrix (double m11, double m12, double m21, double m22,
	  double m31 = 0, double m32 = 0)
	  { m_11 = m11; m_12 = m12; m_21 = m21; m_22 = m22; m_31 = m31; m_32 = m32; }
	CAGMatrix operator * (const CAGMatrix &Matrix) const;
	void operator *= (const CAGMatrix &Matrix)
	  { *this = *this * Matrix; }
	bool GetRotation (int &Angle) const;
	CAGMatrix Inverse () const;
	bool IsUnity () const
	  { return (m_11 == 1 && m_12 == 0 && m_21 == 0 && m_22 == 1 &&
	  m_31 == 0 && m_32 == 0); }
	void Rotate (double Angle, double x = 0, double y = 0);
	void Scale (double xScale, double yScale, double x = 0, double y = 0);
	void ScaleAndCenter (const RECT &DestRect, const RECT &SrcRect,
	  bool bFlip = false);
	void SetMatrix (double m11, double m12, double m21, double m22,
	  double m31 = 0, double m32 = 0)
	  { m_11 = m11; m_12 = m12; m_21 = m21; m_22 = m22; m_31 = m31; m_32 = m32; }
	void Transform (POINT *pPts, int nCount, bool bTranslate = true) const;
	void Transform (RECT *pRect, bool bTranslate = true) const
	  { Transform ((POINT *) pRect, 2, bTranslate); }
	void Translate (double x, double y)
	  { m_31 += x; m_32 += y; }
	void Translate (int x, int y)
	  { m_31 += (double) x; m_32 += (double) y; }
	void Unity ()
	  { m_11 = m_22 = 1; m_12 = m_21 = m_31 = m_32 = 0; }

public:
	double	m_11;
	double	m_12;
	double	m_21;
	double	m_22;
	double	m_31;
	double	m_32;
};

#endif //__AGMATRIX_H_
