/*
// $Workfile: Urldlg.h $
// $Revision: 1 $
// $Date: 3/03/99 6:01p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/Agurl/Urldlg.h $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 1     7/06/98 8:55a Mwilson
// 
//    Rev 1.0   14 Aug 1997 15:39:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:28:36   Fred
// Initial revision.
// 
//    Rev 1.0   20 Aug 1996 18:06:32   Fred
// Initial revision.
*/

class CAgurlDlg;

#ifndef __URLDLG_H__
#define __URLDLG_H__

/////////////////////////////////////////////////////////////////////////////
// CAgurlDlg dialog

class CAgurlDlg : public CDialog
{
// Construction
public:
	CAgurlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAgurlDlg)
	enum { IDD = IDD_AGURL_DIALOG };
	CStatic	m_Url;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgurlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAgurlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
