#if !defined(AFX_WEBDLG_H__A17F3280_7246_11D1_A666_00A024EA69C0__INCLUDED_)
#define AFX_WEBDLG_H__A17F3280_7246_11D1_A666_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WebDlg.h : header file
//

#include "paper.h"      // For CPaperInfo class
#include "pdlgclr.h"    // CPmwDialogColor class
#include "colorcbo.h"   // For CColorCombo
#include "pmwdoc.h"
#include "webdef.h"     // For SZ_PUBLISH_SECTION, etc. 

/////////////////////////////////////////////////////////////////////////////
// CWebDialog dialog

class CWebDialog : public CPmwDialogColor
{
// Construction
public:
	CWebDialog(CPaperInfo* pPaperInfo, CWnd* pParent = NULL);   // standard constructor

   virtual void UpdatePreview(void);

// Dialog Data
	//{{AFX_DATA(CWebDialog)
	enum { IDD = IDD_WEB_PAGE_SETUP };
	CEdit	m_editWidth;
	CEdit	m_editHeight;
	CButton	m_btnCustomWidth;
	CButton	m_btnWideWidth;
	CButton	m_btnStandardWidth;
	double	m_dHeight;
	double	m_dWidth;
	CString	m_csWidthDesc;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWebDialog)
	afx_msg void OnWebPageCustom();
	afx_msg void OnWebPageStandard();
	afx_msg void OnWebPageWide();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnFinish();
	afx_msg void OnBack();
	afx_msg void OnKillfocusWebPageHeight();
	afx_msg void OnKillfocusWebPageWidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
// Helpers
   void SetNewPageSize(void);

protected:
// Data members
   CPaperInfo*    m_pPaperInfo;
};

/////////////////////////////////////////////////////////////////////////////
// CWebSitePropertiesDialog dialog

class CWebSitePropertiesDialog : public CDialog
{
// Construction
public:
	CWebSitePropertiesDialog(CWnd* pParent = NULL);   // standard constructor

public:
// Static Operations
//   static CString GetHomePageFileName();
//   static CString GetFileExtension();

// Dialog Data
	//{{AFX_DATA(CWebSitePropertiesDialog)
	enum { IDD = IDD_WEB_SITE_PROPERTIES };
	CStatic	m_NameAndExt;
	CComboBox	m_comboExtension;
	CComboBox	m_comboFileName;
	CString	   m_csExtension;
	CString	   m_csFileName;
	BOOL	m_bUseDesignChecker;
	BOOL	m_bDisplayWarning;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebSitePropertiesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWebSitePropertiesDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeWebSiteExtension();
	afx_msg void OnEditupdateWebSiteFilename();
	afx_msg void OnCloseupWebSiteFilename();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   void 
   OnNameChanged();
};
/////////////////////////////////////////////////////////////////////////////
// CWebPageProperties dialog

class CWebPageProperties : public CDialog
{
// Construction
public:
	CWebPageProperties(CWnd* pParent = NULL);   // standard constructor

   virtual int DoModal(CPmwDoc *pDoc);
   virtual void UpdateDocView();

// Dialog Data
	//{{AFX_DATA(CWebPageProperties)
	enum { IDD = IDD_WEB_PAGE_PROPERTIES };
	BOOL	m_AllPages;
	BOOL	m_Center;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebPageProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWebPageProperties)
	afx_msg void OnWebPageBrowseTexture();
	afx_msg void OnWebPageNoTexture();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnWebPageApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   // Hide default implementation
   virtual int DoModal() { return CDialog::DoModal(); }

protected:
// Data Members
   CColorCombo m_comboColorBackground;
//   CColorCombo m_comboColorBodyText;
   CColorCombo m_comboColorNVHLink;
   CColorCombo m_comboColorVHLink;
   CColorCombo m_comboColorAHLink;
   DB_RECORD_NUMBER  m_dbrecTexture;
   CPmwDoc     *m_pDoc;

   void
   GetProperties(CPageProperties *pPageProp);
};
/////////////////////////////////////////////////////////////////////////////
// COnlineProgramConfirmStart dialog

class COnlineProgramConfirmStart : public CDialog
{
// Construction
public:
	COnlineProgramConfirmStart(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COnlineProgramConfirmStart)
	enum { IDD = IDD_ONLINE_CONFIRM_START };
	BOOL	m_bDontShowMe;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnlineProgramConfirmStart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COnlineProgramConfirmStart)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBDLG_H__A17F3280_7246_11D1_A666_00A024EA69C0__INCLUDED_)
