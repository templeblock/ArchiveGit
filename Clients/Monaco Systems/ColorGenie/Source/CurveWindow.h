#pragma once

#include "curve.h"

class CCurveWindow : public CWnd
{
protected:	
	DECLARE_DYNCREATE(CCurveWindow)
	BOOL bColorImage;

	// for feedback to user when moving points
	CEdit* InBox;
	CEdit* OutBox;
	BOOL bPercent;
	BOOL bEnableEdits;
	
	CCurve CyanCurve;
	CCurve MagentaCurve;	
	CCurve YellowCurve;
	CCurve GammaCurve;	
	CCurve* Curves[CI_NCMYK];
	int nCurves;
	BOOL bCurveShown[CI_NCMYK];

	HDC hButtonDownDC;

public:
	void SetPercent(BOOL bNewPercent);
	void SetEnableEdits(BOOL bNewEnableEdits);
	void SetSpace(HDC dc) const;
	void ToLogicalSpace(const CPoint& in,CPoint& out) const;
	CCurveWindow(BOOL colorImage=TRUE);
	BOOL SelectCurve(CMYK iCurve, BOOL bSelect);
	void ShowCurve(CMYK iCurve, BOOL bShow);
	void ToggleCurve(CMYK iCurve);
	void UpdateEditBoxes(int x = -1);
	// for initialization, does not store pointers, makes a copy
	void Init(BOOL colorImage,
		CEdit* in,CEdit* out);
	void SetPoints( CMYK iCurve, double* xPoints, double* yPoints, int numPoints);
	// pointers must already be allocated
	int GetPoints( CMYK iCurve, double* xPoints, double* yPoints, int numPoints);
	// set the curves to defauts, not the selected curves
	void SetToDefaults();
	void SetToEmpty();
	void Draw(HDC theDC);
//	void CCurveWindow::Draw(HDC theDC); //pulled out 
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurveWindow)
public:
	virtual void OnDraw(CDC* pDC); 
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);	
	
protected:
	//}}AFX_VIRTUAL
public:
	virtual ~CCurveWindow();
protected:
// Generated message map functions
protected:
	//{{AFX_MSG(CCurveWindow)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	// custom control support
	static BOOL RegisterControlClass();
protected:
	void NotifyParentofChange(int iCurve);
	void NotifyParentofSelect(int iCurve);
	void OnArrowKey(UINT nChar);
	void DrawGrid(HDC hDC);
	virtual void PostNcDestroy();
	static LRESULT CALLBACK EXPORT WndProcHook(HWND, UINT, WPARAM, LPARAM);
};
