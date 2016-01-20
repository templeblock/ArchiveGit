#if !defined(AFX_MYMESSAGEBOX_H__6506C364_B4D9_11D1_9B30_006097149CA7__INCLUDED_)
#define AFX_MYMESSAGEBOX_H__6506C364_B4D9_11D1_9B30_006097149CA7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MyMessageBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MyMessageBox dialog

class MyMessageBox : public CDialog
{
// Construction
public:
	MyMessageBox(CString my_meg, int disable_which, CWnd* pParent = NULL);   // standard constructor
	void DisableButton(int which_button); // 1, 2, 3
	
// Dialog Data
	//{{AFX_DATA(MyMessageBox)
	enum { IDD = IDD_EXCLAM_MESSAGE };
	CButton	m_save;
	CStatic	m_exclamicon;
	CButton	m_cancel;
	CButton	m_no_save;
	CString	m_exclam_text;
	//}}AFX_DATA
	
	int m_disabled;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MyMessageBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MyMessageBox)
	virtual BOOL OnInitDialog();
	afx_msg void OnSave();
	afx_msg void OnNoSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYMESSAGEBOX_H__6506C364_B4D9_11D1_9B30_006097149CA7__INCLUDED_)
