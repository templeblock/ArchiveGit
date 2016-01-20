/*
// warptext.h : header file
//
// $Workfile: WARPTEXT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/WARPTEXT.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:36   Fred
// Initial revision.
// 
//    Rev 1.2   03 Apr 1997 14:31:38   Jay
// Resource cleanup
// 
//    Rev 1.1   26 Jun 1996 14:23:14   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:31:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:37:00   FRED
// Initial revision.
// 
//    Rev 1.17   22 Sep 1995 15:44:20   JAY
// Fix in WIN32 for misdirected tool bar updates.
// 
//    Rev 1.16   02 Sep 1995 15:04:54   FRED
// More changes to make help work
// 
//    Rev 1.15   25 Aug 1995 15:00:46   JAY
// Insert fields dialog
// 
//    Rev 1.14   14 Aug 1995 17:18:56   JAY
// Cut/paste/undo from warp text dialog.
// 
//    Rev 1.13   02 Aug 1995 12:57:16   JAY
// Insert fill-in fields.
// 
//    Rev 1.12   02 Aug 1995 08:06:46   FRED
// Fill-in fields stuff.
// 
//    Rev 1.11   16 Jul 1995 14:53:46   FRED
// Destructor is virtual
// 
//    Rev 1.10   12 May 1995 10:34:44   FRED
// Warp text preview
// 
//    Rev 1.9   05 May 1995 17:11:02   FRED
//  
// 
//    Rev 1.8   03 May 1995 18:10:08   FRED
//  
// 
//    Rev 1.7   02 May 1995 17:09:20   JAY
// Fixed some smart fields bugs.
// 
//    Rev 1.6   02 May 1995 15:39:40   JAY
// More smart fields stuff.
// 
//    Rev 1.5   01 May 1995 15:27:44   JAY
// Restructured the macro stuff
// 
//    Rev 1.4   01 May 1995 08:45:24   JAY
// Changed macrosvr.h to macrosrv.h (now in DATABASE).
// 
//    Rev 1.3   27 Apr 1995 13:29:08   FRED
// 
//    Rev 1.2   25 Apr 1995 15:25:36   JAY
//  
// 
//    Rev 1.1   25 Apr 1995 09:03:08   JAY
// This is becoming real, now...
// 
//    Rev 1.0   20 Apr 1995 16:16:18   FRED
// Initial revision.
*/

class CWarpTextTextPage;

#ifndef __WARPTEXT_H__
#define __WARPTEXT_H__

class WarpTextObject;

class CPmwDoc;
class CPmwView;
class CFrameEdit;

#include "pmwdlg.h"
#include "fmtbar.h"
#include "macrorec.h"
#include "pmwprop.h"

/////////////////////////////////////////////////////////////////////////////
// CWarpTextTextPage dialog

class CWarpTextTextPage : public CPmwPropertyPage
{
// Construction
public:
	CWarpTextTextPage(CPmwView* pView);	// standard constructor
	virtual ~CWarpTextTextPage();

// Dialog Data
	//{{AFX_DATA(CWarpTextTextPage)
	enum { IDD = IDD_WARP_TEXT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
	//{{AFX_MSG(CWarpTextTextPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnViewFillInFields();
	afx_msg void OnInsertField();
	//}}AFX_MSG
	afx_msg void OnTextTypeface();
	afx_msg void OnUpdateTextTypeface(CCmdUI* pCmdUI);
	afx_msg void OnTextBold();
	afx_msg void OnUpdateTextBold(CCmdUI* pCmdUI);
	afx_msg void OnTextItalic();
	afx_msg void OnUpdateTextItalic(CCmdUI* pCmdUI);
	afx_msg void OnAlignLeft();
	afx_msg void OnUpdateAlignLeft(CCmdUI* pCmdUI);
	afx_msg void OnAlignCenter();
	afx_msg void OnUpdateAlignCenter(CCmdUI* pCmdUI);
	afx_msg void OnAlignRight();
	afx_msg void OnUpdateAlignRight(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()

	void UpdateHAlignment(void);

	void BuildTypefaceList(void);

	void UpdateHorizontalAlignment(CCmdUI* pCmdUI, ALIGN_TYPE nType);
	void RealignFrame(ALIGN_TYPE nType);
	void ChangeFont(CTextStyle* pStyle, WORD wCmdID);

	static void ReflowHook(LPVOID pData);

public:
	void Object(WarpTextObject* pObject)
		{ m_pObject = pObject; }
	void GetText(void);

	CFrameEdit* GetFrameEdit(void)
		{ return m_pFrameEdit; }

	CPmwDoc* GetDocument(void) const
		{ return m_pDoc; }
	void GetMacrosInUse(CWordArray& cwaMacrosInUse);

	void InsertMacro(int nMacro);

	void ReflowMacros(void);

	CDialogFormatBar* GetFormatBar(void)
		{ return &m_FormatBar; }

protected:

	WarpTextObject* m_pObject;
	CFrameEdit* m_pFrameEdit;
	CPmwView* m_pView;
	CPmwDoc* m_pDoc;
	CDialogFormatBar m_FormatBar;

	CMacroList m_SaveMacroList;
	BOOL m_fMacrosChanged;

	HACCEL m_hAccelerators;
};

/////////////////////////////////////////////////////////////////////////////
#endif
