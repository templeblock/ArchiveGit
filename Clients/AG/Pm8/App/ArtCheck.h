#if !defined(AFX_ARTCHECK_H__869AF6A1_28B9_11D2_A47C_00104B1FC240__INCLUDED_)
#define AFX_ARTCHECK_H__869AF6A1_28B9_11D2_A47C_00104B1FC240__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ArtCheck.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CArtCheckUtil dialog

class CArtCheckUtil : public CDialog
{
// Construction
public:
	CArtCheckUtil(BOOL bSrcIsDir, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CArtCheckUtil)
	enum { IDD = IDD_CHECK_ART_UTIL };
	CString	m_csArtSource;
	CString	m_csProjSource;
	BOOL	m_bCheckArt;
	BOOL	m_bGenerateFileList;
	BOOL	m_bSrcIsDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArtCheckUtil)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CArtCheckUtil)
	afx_msg void OnSelectSrcArt();
	afx_msg void OnSelectSrcProj();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARTCHECK_H__869AF6A1_28B9_11D2_A47C_00104B1FC240__INCLUDED_)
