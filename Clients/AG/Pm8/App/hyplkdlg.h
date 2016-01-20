#if !defined(AFX_HYPLNKDLG_H__3ABC2AA1_B290_11D1_8541_444553540000__INCLUDED_)
#define AFX_HYPLNKDLG_H__3ABC2AA1_B290_11D1_8541_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// HyplnkDlg.h : header file
//

#include "pmwdlg.h"

#include "hyperrec.h"

class RectangleObject;

/////////////////////////////////////////////////////////////////////////////
// CHyperlinkDialog dialog

class CHyperlinkDialog : public CPmwDialog
{
   INHERIT(CHyperlinkDialog, CPmwDialog)

// Construction
public:
	CHyperlinkDialog(CWnd* pParent = NULL);   // standard constructor
/*
   // Used to set special properties of a Hyperlink object
   // Maybe it doesn't belong here
   void
   SetHotProperties(RectangleObject* rp);
*/
   void
   GetData(HyperlinkData* p);

   void
   SetData(HyperlinkData* p, CPmwDoc* pDoc = NULL);

// Dialog Data
	//{{AFX_DATA(CHyperlinkDialog)
	enum { IDD = IDD_DIALOG_HYPERLINK };
	CButton	m_Remove;
	CButton	m_Browse;
	CStatic	m_Static;
	CEdit	   m_EditNumControl;
	CEdit	   m_EditPathControl;
	CButton	m_PageControl1;
	CButton	m_PageControl2;
	CButton	m_PageControl3;
	CButton	m_PageControl4;
	CButton	m_LocalDoc;
	int		m_LinkType;
	int		m_PageType;
   HyperlinkData  m_HData;
	int		m_EditNumValue;
	CString	m_EditTemp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHyperlinkDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHyperlinkDialog)
	afx_msg void OnHyperNetdoc();
	afx_msg void OnHyperLocalfile();
	afx_msg void OnHyperLocaldoc();
	afx_msg void OnHyperEmail();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHyperPage1();
	afx_msg void OnHyperPageNext();
	afx_msg void OnHyperPagePrev();
	afx_msg void OnHyperPageSpec();
	afx_msg void OnHyperRemove();
	afx_msg void OnHyperBrowse();
	afx_msg void OnChangeHyperEditNumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   DWORD             mCurrentPageIndex;
   static int        HyperTypeTable [4];
   static int        PageTypeTable [4];
   CPmwDoc*          m_pDoc;
   BOOL              m_AllowRemove;

   void
   UpdateControlVisibility(void);

   DB_RECORD_NUMBER
   GetPageRecordNumber(int PageIndex);
   
   void
   SetPageNumber(DB_RECORD_NUMBER rn);

   DocumentRecord*
   GetDocumentRecord(void);

   DWORD
   GetNumberOfPages(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HYPLNKDLG_H__3ABC2AA1_B290_11D1_8541_444553540000__INCLUDED_)
