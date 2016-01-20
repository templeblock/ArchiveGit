/*
// $Workfile: PROFILE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PROFILE.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:32   Fred
// Initial revision.
// 
//    Rev 1.1   16 Sep 1996 12:39:44   Fred
// Profile stuff can be disabled
// 
//    Rev 1.0   13 Sep 1996 09:54:26   Fred
// Initial revision.
*/

class CProfile;

#ifndef __PROFILE_H__
#define __PROFILE_H__

//////////////////////////////////////////////////////////////////////////
// CProfile

class CProfile
{
public:
	CProfile();
	virtual ~CProfile();

public:
	void Add(LPCSTR pszLabel);
	void Add(LPCSTR pszLabel, DWORD dwValue);
	void Show();
	void Clear();
	CStringArray& GetProfileStrings(void)
		{ return m_Profile; }
	void Profiling(BOOL fProfiling = TRUE)
		{ m_fProfiling = fProfiling; }
	BOOL IsProfiling(void)
		{ return m_fProfiling; }

protected:
	CStringArray m_Profile;

	BOOL m_fProfiling;
};

/////////////////////////////////////////////////////////////////////////////
// CProfileShowDialog dialog

class CProfileShowDialog : public CDialog
{
// Construction
public:
	CProfileShowDialog(CProfile* pProfile, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProfileShowDialog)
	enum { IDD = IDD_PROFILE_SHOW };
	CListBox	m_ProfileList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProfileShowDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProfileShowDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnProfileClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void BuildProfileList(void);

protected:
	CProfile* m_pProfile;
};

#endif
