#pragma once

#include "ProfilerDoc.h"
#include "grid.h"

// ViewDataDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ViewDataDialog dialog

class CViewDataDialog : public CDialog
{
// Construction
public:
	CViewDataDialog(CProfileDoc* pDoc, int iType, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViewDataDialog)
	enum { IDD = IDD_VIEWDATA };
	CGrid m_ctlGrid;
	CString	m_strPatch;
	//}}AFX_DATA
	int m_iType;
	CProfileDoc* m_pDoc;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewDataDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void GetDataFromDoc(CProfileDoc* pDoc);
protected:
	void DensityToRgb(char *cmyk, double *density, int num, int which);
	// Generated message map functions
	//{{AFX_MSG(CViewDataDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnGridClick();
	afx_msg void OnGridKeyDown(short* pKeyCode, short Shift);
	afx_msg void OnGridRowColChange();
	afx_msg void OnGridSelChange();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
