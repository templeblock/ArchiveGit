#if !defined(AFX_CUSTOMEDITCTRL_H__EE401523_B6A2_11D2_BF30_0060088E2ECD__INCLUDED_)
#define AFX_CUSTOMEDITCTRL_H__EE401523_B6A2_11D2_BF30_0060088E2ECD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomEditCtrl.h : header file
//

const int	kNoEditBorderStyle = 0;
const int	kSunkenBorderStyle = 1;
const int	kBumpBorderStyle = 2;

/////////////////////////////////////////////////////////////////////////////
// CCustomEditCtrl window

class CCustomEditCtrl : public CEdit
{
// Construction & Destruction
public:
	CCustomEditCtrl();
	virtual ~CCustomEditCtrl();

// Attributes
public:
	void SetEditFont(CString strEditFont,int nEditFontSize );

	void SetEditTextColor(COLORREF txtClr){m_clrText = txtClr;}
	void SetEditBehindTextColor(COLORREF behindTxtClr){m_clrBehindText = behindTxtClr;}

	void SetEditBdrStyle(int theStyle) { m_nBorderStyle = theStyle; }
	void Set3DColors(COLORREF highlightClr, COLORREF shadowClr);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomEditCtrl)
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

private:
	void DrawCustomBorder(CDC& theDC, CRect& winRect);


	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomEditCtrl)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcPaint();
   //}}AFX_MSG
	afx_msg long OnSetFont(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	CFont*	m_pEditFont;

	COLORREF	m_clrText;
	COLORREF m_clrBehindText;

	COLORREF	m_clrBoxHighlight;	// for giving control 3D appearance
	COLORREF	m_clrBoxShadow;		// for giving control 3D appearance		

	BOOL		m_bCustCtrlColors;
	int		m_nBorderStyle;			// sucken or bump
	CSize		m_czFrameSize;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMEDITCTRL_H__EE401523_B6A2_11D2_BF30_0060088E2ECD__INCLUDED_)
