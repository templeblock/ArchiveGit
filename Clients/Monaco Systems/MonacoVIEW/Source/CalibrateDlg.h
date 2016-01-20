#if !defined(AFX_CALIBRATEDLG_H__348BDA41_E36F_11D0_9B8C_444553540000__INCLUDED_)
#define AFX_CALIBRATEDLG_H__348BDA41_E36F_11D0_9B8C_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CCalibrateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCalibrateDlg dialog

class CCalibrateDlg : public CDialog
{
// Construction
public:
	CCalibrateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCalibrateDlg)
	enum { IDD = IDD_CALIBRATE };
	//}}AFX_DATA
 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalibrateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCalibrateDlg)
	afx_msg void OnCancel();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALIBRATEDLG_H__348BDA41_E36F_11D0_9B8C_444553540000__INCLUDED_)
