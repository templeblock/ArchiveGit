#if !defined(AFX_DISPTEXT_H__9DB12F9B_4308_11D2_8BEF_CB3B5D93F18E__INCLUDED_)
#define AFX_DISPTEXT_H__9DB12F9B_4308_11D2_8BEF_CB3B5D93F18E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DispText.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDisplayTextDialog dialog

class CDisplayTextDialog : public CDialog
{
	INHERIT(CDisplayTextDialog, CDialog)
// Construction
public:
   enum FileTypes
   {
      filetypeText,
      filetypeRTF
   };
	CDisplayTextDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDisplayTextDialog)
	enum { IDD = IDD_DISPLAY_TEXT };
	CButton	m_btnOK;
	CStatic	m_ctrlTextFrame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayTextDialog)
	public:
   virtual int  DoModal(LPCSTR szTitle, LPCSTR szFileName);
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support
   virtual int  DoModal(void);                        // Not Supported
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDisplayTextDialog)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CDisplayTextDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

   CRichEditCtrl  m_ctrlRichEdit;
   CString        m_csFileName, m_csTitle;
   CRect          m_crTextFrame;
   CRect          m_crOKButton;
   CSize          m_csizeBorder;
   FileTypes      m_enFileType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPTEXT_H__9DB12F9B_4308_11D2_8BEF_CB3B5D93F18E__INCLUDED_)
