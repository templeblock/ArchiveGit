// TransBtn.h: interface for the CTransBtn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSBTN_H__A791F202_C043_11D2_B996_00A02416AB46__INCLUDED_)
#define AFX_TRANSBTN_H__A791F202_C043_11D2_B996_00A02416AB46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "imgctrl.h"


class CTransBtn : public CButton  
{
public:
    // enable runtime type checking
    DECLARE_DYNCREATE(CTransBtn)

	CTransBtn();
	virtual ~CTransBtn();

	void SetImageControl(CPImageControl* pImageCtrl);

	void SetBtnTextClr(COLORREF theColor) { m_clrText = theColor; }
	void SetBtnDisabledTextClr (COLORREF theColor) { m_clrDisabledText = theColor;	}
	void SetBtnFocusTextClr (COLORREF theColor) { m_clrFocusText = theColor; }

	void MakeStateBtn(BOOL fIsState) { m_fIsStateBtn = fIsState; }
protected:
	// Generated message map functions
	//{{AFX_MSG(CTransBtn)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	
	afx_msg LRESULT OnSetPushed( WPARAM, LPARAM );
	afx_msg LRESULT OnGetPushed( WPARAM, LPARAM );
	afx_msg LRESULT OnSizeCtrl( WPARAM, LPARAM );
	afx_msg LRESULT OnSetFont( WPARAM, LPARAM );

	DECLARE_MESSAGE_MAP()

	
	void FireClick();

protected:

	CPImageControl*	m_pImageControl;

	BOOL		m_fIsStateBtn;

	BOOL		m_fMouseIn;
	BOOL		m_fMouseCaptured;
	BOOL		m_fMouseDown;
	UINT		m_uiState;
	WORD		m_wStyle;
	int		m_nState;
	
	CFont*	m_pBtnFont;

	COLORREF	m_clrText;
	COLORREF	m_clrDisabledText;
	COLORREF	m_clrFocusText;
};

#endif // !defined(AFX_TRANSBTN_H__A791F202_C043_11D2_B996_00A02416AB46__INCLUDED_)
