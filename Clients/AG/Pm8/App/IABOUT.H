/*
// $Workfile: IABOUT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/IABOUT.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     4/08/98 12:34p Fredf
// New explorer progress indicator.
// 
//    Rev 1.0   14 Aug 1997 15:21:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:06   Fred
// Initial revision.
// 
//    Rev 1.3   19 May 1997 13:55:42   Fred
// Event Reminder, etc.
// 
//    Rev 1.2   25 Apr 1997 14:22:58   Fred
// Removed introduction
// 
//    Rev 1.1   22 Aug 1996 15:59:08   Fred
// Introduction
// 
//    Rev 1.0   23 Apr 1996 08:26:18   Jay
// Initial revision.
// 
//    Rev 2.5   19 Apr 1996 08:54:26   JAY
// New button scheme. Install is always available.
// 
//    Rev 2.4   05 Feb 1996 14:09:06   FRED
// Dynamically displays program title is about box bitmap
// 
//    Rev 2.3   26 Jan 1996 08:27:46   FRED
// Added Autoplay support to installer.
// 
//    Rev 2.2   07 Aug 1995 17:15:26   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:39:32   JAY
// Reverted. New series.
// 
//    Rev 1.4   15 Jul 1994 16:18:52   FRED
// 
//    Rev 1.3   15 Jul 1994 14:07:26   FRED
// Initial support for "No Sound" button.
// 
// 
//    Rev 1.2   15 Jul 1994 10:39:44   JAY
// Support for monochrome sound buttons.
// 
//    Rev 1.1   29 Jun 1994 16:24:34   JAY
// Added initial sound.
// 
//    Rev 1.0   15 Jun 1994 18:59:20   FRED
// Initial revision.
*/ 

#ifndef _IABOUT_H_
#define _IABOUT_H_

#include "installr.h"
#include "ddb.h"

/////////////////////////////////////////////////////////////////////////////
// CInstallAboutDialog dialog

class CInstallAboutDialog : public CDialog
{
// Construction
public:
	CInstallAboutDialog(BOOL fInstalled, CWnd* pParent = NULL, BOOL fMonochrome = FALSE);	// standard constructor
	~CInstallAboutDialog(void);

// Dialog Data
	//{{AFX_DATA(CInstallAboutDialog)
	enum { IDD = IDD_INSTALL_ABOUT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CInstallAboutDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnInstall();
	afx_msg void OnRemove();
	afx_msg void OnRun();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CDDB *m_pDDB;
	CDDB *m_pTitleDDB;
	UINT m_nTimerID;
	BOOL m_fMonochrome;
	BOOL m_fInstalled;
};

#endif		// ifndef _IABOUT_H_
