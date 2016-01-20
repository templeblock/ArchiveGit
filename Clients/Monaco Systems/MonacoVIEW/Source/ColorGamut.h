#if !defined(AFX_CColorGamut_H__0A47BA21_F18D_11D0_9B8C_444553540000__INCLUDED_)
#define AFX_CColorGamut_H__0A47BA21_F18D_11D0_9B8C_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ColorGamut.h : header file
//
#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)
/////////////////////////////////////////////////////////////////////////////
// CColorGamut window

class CColorGamut : public CStatic
{
// Construction
public:
	CColorGamut();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorGamut)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorGamut();
	
	void DrawTriangle(CDC* dc);
	void GetColor(double L, CPoint where, double* labOut);
	void RenderColor(int iPercentage);
	void Init();
	// Generated message map functions
protected:
	//{{AFX_MSG(CColorGamut)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	CBitmap m_Bitmap;
	BYTE* m_pBitsOriginal;
	BYTE* m_pBitsModified;
	int m_num_bytes;
	int m_LastLDrawn;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CColorGamut_H__0A47BA21_F18D_11D0_9B8C_444553540000__INCLUDED_)
