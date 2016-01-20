// DlgURL.h : header file
//
#include "pdlgclr.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogURL dialog

class CDialogURL : public CPmwDialogColor
{
// Construction
public:
	CDialogURL(CWnd* pParent = NULL);   // standard constructor

   void SetSiteDescription(const CString &csDesc)
      { m_csSiteDescription = csDesc; }

// Dialog Data
	//{{AFX_DATA(CDialogURL)
	enum { IDD = IDD_PMWURL_DIALOG };
	CStatic	m_URL;
	CString	m_csSiteDescription;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogURL)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogURL)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
