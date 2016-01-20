#if !defined(AFX_PROJECTSETUPDLG_H__41BEE673_6906_11D2_9B93_00A0C99F6B3C__INCLUDED_)
#define AFX_PROJECTSETUPDLG_H__41BEE673_6906_11D2_9B93_00A0C99F6B3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectSetupDlg.h : header file
//

#include "ProjectDialog.h"
#include "DblSpin.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupPreview window

class CSetupPreview : public CStatic
{
// Construction
public:
	CSetupPreview();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupPreview)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSetupPreview();

protected:
	void DrawPage( CDC* pDC, CRect rect, int nPage ) ;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSetupPreview)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CProjectSetupDlg dialog

class CProjectSetupDlg : public CProjectDialog
{
// Construction
public:
	CProjectSetupDlg(CWnd* pParent = NULL);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void	InitDocDesc( BOOL fFirstInit = TRUE );
	CRect	GetPrintableArea();
	BOOL	SetOuterMarginLimits( const CRect &rcLimit, const CSize &szPage );

	// Generated message map functions
	//{{AFX_MSG(CProjectSetupDlg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnFooter();
	afx_msg void OnHeader();
	afx_msg void OnSelchangePagesize();
	afx_msg void OnDataChange();
	afx_msg void OnKillfocusColumns();
	afx_msg void OnKillfocusPages();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKillfocusBottommargin();
	afx_msg void OnKillfocusLeftmargin();
	afx_msg void OnKillfocusRightmargin();
	afx_msg void OnKillfocusTopmargin();
	afx_msg LONG OnUpdateData( WPARAM, LPARAM );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
private:

	//{{AFX_DATA(CProjectSetupDlg)
	enum { IDD = IDD_PROJECT_SETUP };
	CSetupPreview	m_cPreview;
	CBDoubleSpinner	m_spinTop;
	CBDoubleEdit	m_editTop;
	CBDoubleSpinner	m_spinRight;
	CBDoubleEdit	m_editRight;
	CBDoubleSpinner	m_spinLeft;
	CBDoubleEdit	m_editLeft;
	CBDoubleSpinner	m_spinBottom;
	CBDoubleEdit	m_editBottom;
	double	m_fBottom;
	double	m_fLeft;
	double	m_fRight;
	double	m_fTop;
	//}}AFX_DATA

    double	m_fMinTop, m_fMinLeft, m_fMinBottom, m_fMinRight;
    double	m_fMax;
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTSETUPDLG_H__41BEE673_6906_11D2_9B93_00A0C99F6B3C__INCLUDED_)
