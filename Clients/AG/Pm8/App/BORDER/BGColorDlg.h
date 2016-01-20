#if !defined(AFX_BGCOLORDLG_H__19D08CC1_A389_11D1_8680_0060089672BE__INCLUDED_)
#define AFX_BGCOLORDLG_H__19D08CC1_A389_11D1_8680_0060089672BE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BGColorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBGColorDlg dialog

class CBGColorDlg : public CDialog
{
	// Current object
	CBEObjectD* m_pBEObj;
	CBECallbackInterface* m_pCallbackInterface;

	// Color box
	CPoint m_ptTopLeft, m_ptTopRight, m_ptBotRight, m_ptBotLeft;

// Construction
public:
	CBGColorDlg(CBEObjectD* pBEObject, CBECallbackInterface* pCallbackInterface, CWnd* pParent = NULL);   // standard constructor

// Changes
	BOOL m_bUseBGColor;			// TRUE when BG color is to be displayed
	COLORREF m_colBGColor;		// Actual RGB of the BG color (-1L for transparent)

// Dialog Data
	//{{AFX_DATA(CBGColorDlg)
	enum { IDD = IDD_CHOOSE_BG_COLOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBGColorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBGColorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDisplayBgColor();
	afx_msg void OnChooseColor();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BGCOLORDLG_H__19D08CC1_A389_11D1_8680_0060089672BE__INCLUDED_)
