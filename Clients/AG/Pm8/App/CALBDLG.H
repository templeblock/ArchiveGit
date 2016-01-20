/*
// $Workfile: CALBDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CALBDLG.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:50   Fred
// Initial revision.
// 
//    Rev 1.6   27 Aug 1996 13:55:06   Jay
// Sound inspect from project window
// 
//    Rev 1.5   24 Aug 1996 14:14:14   Fred
// Properly focuses album control
// 
//    Rev 1.4   28 Jul 1996 07:46:40   Fred
// More album control stuff
// 
//    Rev 1.3   26 Jul 1996 17:42:22   Fred
//  
// 
//    Rev 1.2   26 Jul 1996 13:28:02   Fred
// New album controls
// 
//    Rev 1.1   11 Apr 1996 13:06:24   FRED
// Copied from PMW2
// 
//    Rev 2.23   27 Mar 1996 14:58:56   JAY
// Fixed open album/upgrade/gp problem
// 
//    Rev 2.22   13 Sep 1995 11:48:02   JAY
// New 32-bit changes
// 
//    Rev 2.21   02 Sep 1995 09:06:02   FRED
// Help fixes.
// 
//    Rev 2.20   29 Aug 1995 09:36:54   FRED
//  
// 
//    Rev 2.19   24 Aug 1995 07:42:22   JAY
// Added help hit test.
// 
//    Rev 2.18   22 Aug 1995 11:33:44   FRED
// Some album code cleanup
// 
//    Rev 2.17   21 Aug 1995 18:42:30   FRED
// Modeless picture album
// 
//    Rev 2.16   11 Aug 1995 07:15:58   JAY
// Misc. experiments
// 
//    Rev 2.15   10 Aug 1995 12:50:56   FRED
// Album menu items
// 
//    Rev 2.14   10 Jul 1995 16:26:28   JAY
// More useful stuff for save project.
// 
//    Rev 2.13   08 Jul 1995 16:27:38   FRED
// Album refinement
// 
//    Rev 2.12   08 Jul 1995 13:13:42   FRED
// Still more album focus/navigation stuff!
// 
//    Rev 2.11   06 Jul 1995 09:36:50   JAY
// Window position remembering code.
// 
//    Rev 2.10   05 Jul 1995 15:17:20   FRED
// Some album focus stuff.
// 
//    Rev 2.9   03 Jul 1995 18:48:42   FRED
// Open project button
// 
//    Rev 2.8   26 Jun 1995 16:58:48   JAY
//  
// 
//    Rev 2.7   16 Jun 1995 17:36:30   FRED
// Start of Make your own
// 
//    Rev 2.6   14 Jun 1995 14:12:50   FRED
// Album messages
// 
//    Rev 2.5   09 Jun 1995 10:35:44   FRED
// Uses new code adapted from MFC\SRC\PROPDLG for
// simulating a modelss dialog box in MFC.
// 
// 
//    Rev 2.4   09 Jun 1995 07:43:24   FRED
// Sound Manager changes
// 
//    Rev 2.3   07 Jun 1995 16:45:44   FRED
// Album dialog and Project album dialog.
// 
// 
//    Rev 2.2   05 Jun 1995 16:52:02   JAY
// Now a CPmwDialog.
// 
//    Rev 2.1   05 Jun 1995 11:32:56   JAY
//  
*/

class CAlbumDialog;

#ifndef __CALBDLG_H__
#define __CALBDLG_H__

#include "albumctl.h"

// calbdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlbumDialog dialog

class CAlbumDialog : public CWnd
{
// Construction
public:
	CAlbumDialog(LPCSTR pszCaption, CWnd* pParent, UINT nMenu, UINT wSoundID);
	~CAlbumDialog();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlbumDialog)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg LRESULT OnAlbumSelect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAlbumDeselect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAlbumChoose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAlbumInspect(WPARAM wParam, LPARAM lParam);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	
public:
	virtual afx_msg void OnExitAlbum();
	virtual afx_msg void OnPageBack();
	virtual afx_msg void OnPageNext();
	virtual void DoActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

public:
	int DoModal();
	virtual void EndDialog(int nEndID); // used to terminate a modal dialog
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	
	BOOL Create(
		CWnd* pParentWnd = NULL,
		DWORD dwStyle = WS_SYSMENU|WS_POPUP|WS_CAPTION|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME,
		DWORD dwExStyle = 0);
		
	UINT GetSoundID(void) const
		{ return m_wSoundID; }

protected:

	BOOL PumpMessage();
	virtual CAlbumControl* MakeAlbum(void);
	virtual void InitDialog(void);
	virtual void Resize(void);

	virtual LPCSTR WindowPositionSection(void);

	CAlbumControl* m_pAlbum;

	UINT m_nMenu;
	CMenu* m_pMenu;
	
	UINT m_wSoundID;
	
	BOOL m_fModeless;
	CWnd* m_pParentWnd;
	BOOL m_fParentDisabled;
	int m_nID;
	HWND m_hFocusWnd;
	CString m_csCaption;
	
	int m_nIDHelp;
	
protected:

	virtual LRESULT AlbumSelect(int wParam, CBookComponent* pComponent);
	virtual LRESULT AlbumDeselect(int wParam, CBookComponent* pComponent);
	virtual LRESULT AlbumChoose(int wParam, CBookComponent* pComponent);
	virtual LRESULT AlbumInspect(int wParam, CBookComponent* pComponent);

public:

	virtual CObject* Register(const CString& csKey, CObject* pValue = NULL);
	
protected:

	CMapStringToOb m_Register;
	
public:
	void CommandComponent(CBookComponent& Component);
	CAlbumControl* GetAlbumControl(void)
		{ return m_pAlbum; }
	void FreeingAlbum(void);
	void FixFocus(void);
};

#endif
