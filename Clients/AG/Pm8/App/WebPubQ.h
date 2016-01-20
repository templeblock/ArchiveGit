#if !defined(AFX_WEBPUBQ_H__73600AC1_1D57_11D2_A49B_00A0C99E4C17__INCLUDED_)
#define AFX_WEBPUBQ_H__73600AC1_1D57_11D2_A49B_00A0C99E4C17__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WebPubQ.h : header file
//
#include	"PMWDLG.H"
/////////////////////////////////////////////////////////////////////////////
// CWebPublishQuery dialog

class CWebPublishQuery : public CPmwDialog
{
// Construction
public:
	CWebPublishQuery(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWebPublishQuery)
	enum { IDD = IDD_WEB_PUB_QUERY };
   BOOL     m_fPublishWarning;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebPublishQuery)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWebPublishQuery)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBPUBQ_H__73600AC1_1D57_11D2_A49B_00A0C99E4C17__INCLUDED_)
