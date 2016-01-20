#if !defined(AFX_CELOUTD_H__0AC505F1_7ADF_11D1_90F2_00A0240C7400__INCLUDED_)
#define AFX_CELOUTD_H__0AC505F1_7ADF_11D1_90F2_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// celOutD.h : header file
//

#include "celctl.h"
#include "WinColorDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCellOutlineDlg dialog

class CCellOutlineDlg : public CDialog
{
// Construction
public:
	CCellOutlineDlg(CTableCellFormatting* pFormatting, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCellOutlineDlg)
	enum { IDD = IDD_CELL_FORMATTING };
	CSpinButtonCtrl	m_ctlSpinLine;
	CEdit					m_ctlLineWidth;
	int		m_nLineWidth;
	//}}AFX_DATA

	CCellOutlineControl* m_pOutlineControl;
	RWinColorBtn m_FillColor;
	RWinColorBtn m_LineColor;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCellOutlineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCellOutlineDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnEdgeSelect();
	afx_msg void OnFillColor();
	afx_msg void OnLineColor();
	afx_msg void OnChangeLineWidth();
	afx_msg void OnSetfocusLineWidth();
	afx_msg void OnKillfocusLineWidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	// Implementation
	CTableCellFormatting* m_pFormatting;

	// for edit cases
	int m_nLineWidthSave;

	void UpdateCellControls(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CELOUTD_H__0AC505F1_7ADF_11D1_90F2_00A0240C7400__INCLUDED_)
