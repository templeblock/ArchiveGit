#if !defined(AFX_CPreferenceDlg_H__9C5E5F61_EDAD_11D0_9B8C_444553540000__INCLUDED_)
#define AFX_CPreferenceDlg_H__9C5E5F61_EDAD_11D0_9B8C_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PreferenceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreferenceDlg dialog

class CPreferenceDlg : public CDialog
{
// Construction
public:
	CPreferenceDlg(double* pLab, double* pDeltaE, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPreferenceDlg)
	enum { IDD = IDD_PREFERENCES };
	double* m_pLab; //[5][3];
	double* m_pDeltaE; //[5];
	CButton	m_check_photo;
	CButton	m_check_icc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferenceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferenceDlg)
	afx_msg void OnValPatch();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPreferenceDlg_H__9C5E5F61_EDAD_11D0_9B8C_444553540000__INCLUDED_)
