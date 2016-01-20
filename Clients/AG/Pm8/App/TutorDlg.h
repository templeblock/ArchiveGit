/*
// $Workfile: TutorDlg.h $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/TutorDlg.h $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 6     7/26/98 7:05p Hforman
// changed baseclass from CDialog to CPmwDialog for palette handling
// 
// 5     5/15/98 4:42p Fredf
// 
// 4     4/21/98 4:01p Rlovejoy
// Changes to put buttons on top of bitmap.
// 
// 3     11/06/97 6:04p Hforman
// minor fixes to ShowTutorial()
 * 
 * 2     10/30/97 6:52p Hforman
 * finishing up functionality, etc.
*/

#ifndef __TUTORDLG_H__
#define __TUTORDLG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "PmwDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CTutorialDlg dialog

class CTutorialDlg : public CPmwDialog
{
// Construction
public:
	CTutorialDlg(CWnd* pParent = NULL);   // standard constructor
   virtual ~CTutorialDlg();

// Dialog Data
	//{{AFX_DATA(CTutorialDlg)
	enum { IDD = IDD_TUTORIAL };
	CButton	m_btnCancel;
	CButton	m_btnNext;
	CButton	m_btnBack;
	CStatic	m_ctrlBitmap;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTutorialDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
   void        ShowTutorial(int nTutorialID);
	void			SetButtonFocus(CButton& btn);

protected:
	void			SetTitle(void);
private:
   void        ResetDialog();

   HINSTANCE   m_hLib;        // handle to Tutorial Library
   int         m_nCurPage;    // which page is showing
   int         m_numPages;    // number of pages in tutorial
   CString     m_csCaption;	// caption of Tutorial
   CPtrArray   m_bitmapArray; // array holding each page bitmap

protected:

	// Generated message map functions
	//{{AFX_MSG(CTutorialDlg)
	afx_msg void OnTutBack();
	afx_msg void OnTutNext();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
   LRESULT OnDisableModal(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // ifndef __TUTORDLG_H__
