//****************************************************************************
//
// File Name:	ProjectDialog.h
//
// Project:		Print Shop Launcher
//
// Author:		Lance Wilson
//
// Description: Definition class for a project dialog.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//	             500 Redwood Blvd.
//               Novato, CA 94948
//	             (415) 382-4400
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/PSLaunchDLL/ProjectDialog.h                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:26p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************
//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#ifndef _PROJECTDIALOG_H_
#define _PROJECTDIALOG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "PSBaseDialog.h"

typedef struct tagSTATEINFO
{
	UINT	uiHeaderID;

} STATEINFO;

/////////////////////////////////////////////////////////////////////////////
// CProjectDialog dialog

class CProjectDialog : public CPSBaseDialog
{
// Construction
public:
	CProjectDialog( UINT nDialogID, CWnd* pParent = NULL, UINT nNextDlg = 0);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjectDialog)
	enum { IDD = IDD_PROJECT_METHOD };
	//}}AFX_DATA

	static STATEINFO StateInfo;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	UINT	m_nNextDlg;
	UINT	m_nNextNextDlg;

	// Generated message map functions
	//{{AFX_MSG(CProjectDialog)
	afx_msg void OnNext();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeFormatList();
	virtual void OnOK();
	afx_msg void OnFormatTall();
	afx_msg void OnFormatWide();
	afx_msg void OnSelchangePaperList();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnStartCustomize();
	afx_msg void OnStartScratch();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _PROJECTDIALOG_H_
