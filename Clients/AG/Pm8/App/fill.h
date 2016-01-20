/*
// $Header: /PM8/App/fill.h 1     3/03/99 6:05p Gbeddow $
//
// $Workfile: fill.h $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/fill.h $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 10    6/18/98 6:11p Rlovejoy
// Fix for clip-offset bug. (Leaves 1-pixel residue behind).
// 
// 9     6/03/98 4:12p Rlovejoy
// Members to ensure minimum bitmap.
// 
// 8     5/14/98 10:52a Rlovejoy
// Apply x,y flips to gradient fills.
// 
// 7     5/12/98 11:34a Rlovejoy
// Added radial fills.
// 
// 6     5/08/98 12:03p Rlovejoy
// Added rotation.
// 
// 5     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 4     4/29/98 6:30p Rlovejoy
// Added 'RestoreClipRegion()'.
// 
// 3     4/29/98 5:59p Rlovejoy
// Sweep gradients & initial revisions.
// 
// 2     4/28/98 10:45a Rlovejoy
// Initial creation.
*/

#ifndef _FILL_H_
#define _FILL_H_

#include "utils.h"
#include "pagedefs.h"
#include "effects.h"	
#include "dibbrush.h"

#define FIRST_GRADIENT_TYPE	(FillFormatV2::FillSweepRight)
#define FRAC_BITS					(16)
#define FRAC_MASK					(0xffff)

#define STEP_PRECISION			(2)

class CPolyPolygon;

// Fill object hierarchy
// Objects exist to carry out various filling procedures.
class CFill
{
protected:
	int m_nFillType;	// used only by some derived classes
	BOOL m_fFillColorExists;
	COLORREF m_clFillColor;
   int m_nContext;
	ANGLE m_dRotation;			// gradient rotation
	ANGLE m_dShapeRotation;		// rotation of shape being filled

// Construction
public:
	CFill();

// Implementation
public:
	virtual ~CFill();

protected:
	// Brush properties
	CDibPatternBrush* GetDIBBrush(CPolyPolygon* pPoly);
	void SetBrushColor(CDibPatternBrush* pBrush, DWORD dwR, DWORD dwG, DWORD dwB)
	{
		pBrush->Color(RGB((dwR >> FRAC_BITS), (dwG >> FRAC_BITS), (dwB >> FRAC_BITS)));
	}

	// Operations required by the fill
	void GenerateRotatedFillBounds(CPoint ptBounds[], ANGLE dRotation, CPoint ptFillBounds[]);
	void ConstructThroughLine(CPoint& ptThrough, ANGLE dRotation, BOOL fHorizontal, int nExtend, CPoint ptLine[]);
	void FindCenterPoint(CPoint ptBounds[], int nPoints, CPoint& ptCenter);
	void RotateLine(CPoint ptLine[], CPoint ptCenter, ANGLE dRotation, CPoint ptRotatedLine[]);
	void RotatePoints(CPoint ptPoints[], CPoint ptCenter, int nPoints, ANGLE dRotation, CPoint ptRotatedPoints[]);
	int IntersectLines(CPoint* ptIntersect, CPoint* p1a, CPoint* p1b, CPoint* p2a, CPoint* p2b);

public:
	static inline IsValidColor(COLOR clColor)
	{
		return (clColor != UNDEFINED_COLOR && clColor != TRANSPARENT_COLOR);
	}
	virtual void SetFillColor(COLOR clFillColor, BOOL fFillColorExists = TRUE)
	{
		m_clFillColor = COLORREF_FROM_COLOR(clFillColor);
		m_fFillColorExists = fFillColorExists && IsValidColor(clFillColor);
	}
	virtual void SetFillColorref(COLORREF clFillColor, BOOL fFillColorExists = TRUE)
	{
		m_clFillColor = clFillColor;
		m_fFillColorExists = fFillColorExists;
	}
	COLORREF GetFillColor()
	{
		return m_clFillColor;
	}
	BOOL FillColorExists()
	{
		return m_fFillColorExists;
	}
	virtual BOOL SelectClipRegion(CDC* pDC, CPolyPolygon* pPoly) = 0;
	virtual void RestoreClipRegion(CDC* pDC) = 0;
	virtual void DoFill(CDC* pDC, CPolyPolygon* pPoly) = 0;

};

class CNullFill : public CFill
{
protected:

// Construction
public:
	CNullFill();

// Implementation
public:
	virtual ~CNullFill();

public:
	virtual void SetFillColor(COLOR clFillColor)
	{
		m_fFillColorExists = FALSE;	// reiterate this point
	}
	virtual BOOL SelectClipRegion(CDC* pDC, CPolyPolygon* pPoly) { return FALSE; }
	virtual void RestoreClipRegion(CDC* pDC) {}
	virtual void DoFill(CDC* pDC, CPolyPolygon* pPoly) {}

};

class CFlatFill : public CFill
{
protected:

// Construction
public:
	CFlatFill();

// Implementation
public:
	virtual ~CFlatFill();

public:
	virtual BOOL SelectClipRegion(CDC* pDC, CPolyPolygon* pPoly) { return TRUE; }
	virtual void RestoreClipRegion(CDC* pDC) {}
	virtual void DoFill(CDC* pDC, CPolyPolygon* pPoly);
};

class CGradientFill : public CFill
{
protected:
	COLORREF m_clBlendColor;
	BOOL m_fBlendColorExists;

// Construction
public:
	CGradientFill();

// Implementation
public:
	virtual ~CGradientFill();

protected:
	void GetColorIncrements(long nSpan, DWORD& dwR, DWORD& dwG, DWORD& dwB, long& dwRStep, long& dwGStep, long& dwBStep);

public:
	virtual void SetFillColor(COLOR clFillColor, BOOL fFillColorExists = TRUE)
	{
		CFill::SetFillColor(clFillColor, fFillColorExists);
		if (!m_fBlendColorExists && m_fFillColorExists)
		{
			SetBlendColor(clFillColor);
		}
	}
	void SetBlendColor(COLOR clBlendColor)
	{
		m_clBlendColor = COLORREF_FROM_COLOR(clBlendColor);
		m_fBlendColorExists = IsValidColor(clBlendColor);
		if (!m_fFillColorExists && m_fBlendColorExists)
		{
			SetFillColor(clBlendColor);
		}
	}
	COLORREF GetBlendColor()
	{
		return m_clBlendColor;
	}
	virtual void SetRotation(ANGLE dRotation)
	{
		m_dRotation = dRotation;
	}
	virtual void SetRotations(ANGLE dRotation, ANGLE dShapeRotation, BOOL fXFlip = FALSE, BOOL fYFlip = FALSE)
	{
		SetRotation(dRotation);
		m_dShapeRotation = dShapeRotation;
	}
	virtual BOOL SelectClipRegion(CDC* pDC, CPolyPolygon* pPoly);
	virtual void RestoreClipRegion(CDC* pDC);
	virtual void DoFill(CDC* pDC, CPolyPolygon* pPoly) = 0;

};

class CSweepGradientFill : public CGradientFill
{
// Construction
public:
	CSweepGradientFill();

// Implementation
public:
	virtual ~CSweepGradientFill();

public:
	void SweepFill(CDC* pDC, CDibPatternBrush* pBrush, CPoint ptBounds[], ANGLE dRotation);
	virtual void DoFill(CDC* pDC, CPolyPolygon* pPoly) = 0;
};

class CSweepGradientFillH : public CSweepGradientFill
{
// Construction
public:
	CSweepGradientFillH();

// Implementation
public:
	virtual ~CSweepGradientFillH();

public:
	virtual void SetRotations(ANGLE dRotation, ANGLE dShapeRotation, BOOL fXFlip = FALSE, BOOL fYFlip = FALSE);
	virtual void DoFill(CDC* pDC, CPolyPolygon* pPoly);

};

class CSweepGradientFillV : public CSweepGradientFill
{
// Construction
public:
	CSweepGradientFillV();

// Implementation
public:
	virtual ~CSweepGradientFillV();

public:
	virtual void SetRotations(ANGLE dRotation, ANGLE dShapeRotation, BOOL fXFlip = FALSE, BOOL fYFlip = FALSE);
	virtual void DoFill(CDC* pDC, CPolyPolygon* pPoly);

};


#define N_RING_POINTS			(32)

class CRadialGradientFill : public CGradientFill
{
// Construction
public:
	CRadialGradientFill();

// Implementation
public:
	virtual ~CRadialGradientFill();

protected:
	static BOOL m_fTrigComputed;
	void ComputePolygonEllipse(CPoint ptCenter, int nWidth, int nHeight, ANGLE dRotation, CPoint ptRing[]);

public:
	void RadialFill(CDC* pDC, CDibPatternBrush* pBrush, CPoint ptCenter, long nWidthRadius, long nHeightRadius, ANGLE dRotation);
	virtual void DoFill(CDC* pDC, CPolyPolygon* pPoly) = 0;

};

class CRadialGradientFillCenter : public CRadialGradientFill
{
// Construction
public:
	CRadialGradientFillCenter();

// Implementation
public:
	virtual ~CRadialGradientFillCenter();

public:
	virtual void DoFill(CDC* pDC, CPolyPolygon* pPoly);
};

class CRadialGradientFillCorner : public CRadialGradientFill
{
	BOOL m_fXFlip, m_fYFlip;

// Construction
public:
	CRadialGradientFillCorner();

// Implementation
public:
	virtual ~CRadialGradientFillCorner();

public:
	virtual void SetRotations(ANGLE dRotation, ANGLE dShapeRotation, BOOL fXFlip = FALSE, BOOL fYFlip = FALSE);
	virtual void DoFill(CDC* pDC, CPolyPolygon* pPoly);
};

// static helpers
CFill* DeriveFillType(int nType);

/////////////////////////////////////////////////////////////////////////////
// CBitBlt class

class CBitBlt
{
	CRect m_rctImgBound;
	CDC m_MemDC;	
	CBitmap m_Bitmap;
	CBitmap *m_pOldBitmap;
	BOOL m_fBadCreation;
	CPoint m_ptCenter, m_ptTopLeft;

// Construction
public:
	CBitBlt(CDC* pDC, int nWidth, int nHeight, int nTopLeftX, int nTopLeftY);
	CBitBlt(CDC* pDC, CRect& rctEnclose);

protected:
	void CreateMembers(CDC* pDC, int nWidth, int nHeight);

// Implementation
public:
	virtual ~CBitBlt();

public:
	CDC* GetMemDC();
	CPoint GetCenter();
	CPoint GetTopLeft();
	void CopyFromScreen(CDC* pDC);
	void CopyToScreen(CDC* pDC);

};


/////////////////////////////////////////////////////////////////////////////

#endif


