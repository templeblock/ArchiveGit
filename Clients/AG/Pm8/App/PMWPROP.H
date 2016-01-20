/*
// $Workfile: PMWPROP.H $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWPROP.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 3     4/16/98 7:33p Fredf
// "Preview Actual Size" control in font dialog (NEWFONT.CPP)
// 
// 2     1/19/98 1:21p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
//    Rev 1.0   14 Aug 1997 15:24:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:18   Fred
// Initial revision.
// 
//    Rev 1.1   28 Mar 1997 17:11:26   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.0   14 Mar 1996 13:30:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:46   FRED
// Initial revision.
// 
//    Rev 1.2   22 Sep 1995 15:42:56   JAY
// Made property pages be CLIPSIBLINGS
// 
//    Rev 1.1   13 Sep 1995 11:48:08   JAY
// New 32-bit changes
// 
//    Rev 1.0   03 Sep 1995 12:37:40   FRED
// Initial revision.
// 
//    Rev 1.1   16 Aug 1995 16:22:24   JAY
// New stuff.
// 
//    Rev 1.0   16 Jul 1995 15:08:18   FRED
// Initial revision.
*/

class CPmwPropertyPage;
class CPmwPropertySheet;

#ifndef __PMWPROP_H__
#define __PMWPROP_H__

/////////////////////////////////////////////////////////////////////////////
// CPmwPropertySheet

class CPmwPropertyPage : public CPropertyPage
{
// Construction
public:
	CPmwPropertyPage(WORD wId);	// standard constructor
	virtual ~CPmwPropertyPage();

// Dialog Data
	//{{AFX_DATA(CPmwPropertyPage)
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPmwPropertyPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	
public:
	WORD GetSoundId(void)
		{ return m_nIDHelp; }
};


/////////////////////////////////////////////////////////////////////////////
// CPmwPropertySheet

class CPmwPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPmwPropertySheet)

// Construction
public:
	CPmwPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPmwPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPmwPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPmwPropertySheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	afx_msg void OnHelp();
	afx_msg void OnOK();

protected:
	WORD GetSoundId(void);
};

/////////////////////////////////////////////////////////////////////////////

#endif
