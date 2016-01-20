#if !defined(AFX_PATCHFORMATPAGE_H__E965E9A3_6642_11D1_A43E_000094794634__INCLUDED_)
#define AFX_PATCHFORMATPAGE_H__E965E9A3_6642_11D1_A43E_000094794634__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PatchFormatPage.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CPatchFormatPage dialog

class CPatchFormatPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPatchFormatPage)

// Construction
public:
	CPatchFormatPage();
	~CPatchFormatPage();

// Dialog Data
	//{{AFX_DATA(CPatchFormatPage)
	enum { IDD = IDD_PATCHFORMAT_PAGE };
	int		m_calFrequency;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPatchFormatPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_patchFormat;
	// Generated message map functions
	//{{AFX_MSG(CPatchFormatPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHFORMATPAGE_H__E965E9A3_6642_11D1_A43E_000094794634__INCLUDED_)
