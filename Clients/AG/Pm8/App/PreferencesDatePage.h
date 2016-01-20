#if !defined(AFX_PREFERENCESDATEPAGE_H__C937DEE1_A642_11D2_8D3A_00A0C9A342CC__INCLUDED_)
#define AFX_PREFERENCESDATEPAGE_H__C937DEE1_A642_11D2_8D3A_00A0C9A342CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesDatePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDatePage dialog

class CPreferencesDatePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPreferencesDatePage)

// Construction
public:
	CPreferencesDatePage();
	~CPreferencesDatePage();

// Dialog Data
	//{{AFX_DATA(CPreferencesDatePage)
	enum { IDD = IDD_PREFS_DATE };
	int		m_nDateFormat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesDatePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesDatePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESDATEPAGE_H__C937DEE1_A642_11D2_8D3A_00A0C9A342CC__INCLUDED_)
