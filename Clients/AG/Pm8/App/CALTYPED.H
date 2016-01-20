#if !defined(AFX_CALTYPED_H__F32F8B31_4E67_11D1_90C3_00A0240C7400__INCLUDED_)
#define AFX_CALTYPED_H__F32F8B31_4E67_11D1_90C3_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CalTypeD.h : header file
//
#include "pdlgclr.h"
#include "calinfo.h"
#include "oriendlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCalendarTypeDlg dialog

class CCalendarTypeDlg : public CPmwDialogColor
{
// Construction
public:
	CCalendarTypeDlg(COLORREF cr = INVALID_DIALOG_COLOR, CALENDAR_INFO * pInfo = NULL, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCalendarTypeDlg)
	enum { IDD = IDD_CALENDAR_TYPE };
	int		m_iCalType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendarTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	BOOL get_exit_info (CALENDAR_INFO_PTR pInfo);
// Implementation
protected:

	CProjectMakePreview  m_Preview;

	// Generated message map functions
	//{{AFX_MSG(CCalendarTypeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBack();
	afx_msg void OnYearly();
	afx_msg void OnMonthly();
	afx_msg void OnWeekly();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	DWORD		m_WizFlags;
	void		UpdateCBitmap();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALTYPED_H__F32F8B31_4E67_11D1_90C3_00A0240C7400__INCLUDED_)
