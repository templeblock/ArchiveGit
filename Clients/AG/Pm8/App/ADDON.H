/*
// ADDON.H
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
//	Small hook routine to CMFADDON.
//
// $Header: /PM8/App/ADDON.H 1     3/03/99 6:01p Gbeddow $
//
// $Log: /PM8/App/ADDON.H $
// 
// 1     3/03/99 6:01p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:28:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:32:14   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:35:38   JAY
// Reverted. New series.
// 
//    Rev 1.4   06 Oct 1994 15:17:02   JAY
// Added IDD_ADDON_FONTS
// 
//    Rev 1.3   04 Aug 1994 08:30:30   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   28 Jan 1994 17:13:12   JAY
//  
// 
//    Rev 1.1   26 Jan 1994 16:37:42   JAY
//  
// 
//    Rev 1.0   13 Jan 1994 17:14:14   JAY
//  
*/

#ifndef __ADDON_H__
#define __ADDON_H__

extern int add_on_fonts(BOOL allow_free_fonts, BOOL only_free_fonts, LPSTR source = NULL, BOOL delete_otls = FALSE);

/////////////////////////////////////////////////////////////////////////////
// CAddonFontDialog dialog

class CAddonFontDialog : public CDialog
{
// Construction
public:
	CAddonFontDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAddonFontDialog)
	enum { IDD = IDD_ADDON_FONTS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CAddonFontDialog)
	afx_msg void OnContinue();
	afx_msg void OnAddFreeFonts();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif				/* __ADDON_H__ */
