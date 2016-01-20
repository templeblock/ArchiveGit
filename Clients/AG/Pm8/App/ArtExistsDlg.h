#if !defined(AFX_ARTEXISTSDLG_H__B830F363_EA64_11D1_B06E_00A024EA69C0__INCLUDED_)
#define AFX_ARTEXISTSDLG_H__B830F363_EA64_11D1_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ArtExistsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CArtExistsDlg dialog

class CArtExistsDlg : public CDialog
{
// Construction
public:
	CArtExistsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CArtExistsDlg)
	enum { IDD = IDD_ART_EXISTS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArtExistsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CArtExistsDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARTEXISTSDLG_H__B830F363_EA64_11D1_B06E_00A024EA69C0__INCLUDED_)
