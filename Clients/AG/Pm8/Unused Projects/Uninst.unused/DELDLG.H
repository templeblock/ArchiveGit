/*
// $Header: /PM8/UNINST/DELDLG.H 1     3/03/99 6:36p Gbeddow $
//
// $Log: /PM8/UNINST/DELDLG.H $
// 
// 1     3/03/99 6:36p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:40:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:30:40   Fred
// Initial revision.
// 
//    Rev 1.0   18 Jul 1996 12:38:14   Fred
// Initial revision.
// 
//    Rev 1.1   17 Oct 1995 09:36:44   JAY
//  
*/

#ifndef __DELDLG_H__
#define __DELDLG_H__

/////////////////////////////////////////////////////////////////////////////
// CSharedDllDialog dialog

class CSharedDllDialog : public CDialog
{
// Construction
public:
	CSharedDllDialog(LPCSTR pszName, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSharedDllDialog)
	enum { IDD = IDD_DELETE_SHARED_DLL };
	CString	m_csFileToDelete;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSharedDllDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSharedDllDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
