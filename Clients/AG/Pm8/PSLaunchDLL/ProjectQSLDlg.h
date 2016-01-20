//****************************************************************************
//
// File Name:	ProjectQSLDlg.h
//
// Project:		Print Shop Launcher
//
// Author:		Lance Wilson
//
// Description: Definition class for a QSL dialogs.
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
//  $Logfile:: /PM8/PSLaunchDLL/ProjectQSLDlg.h                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:26p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _PROJECTQSLDLG_H_
#define _PROJECTQSLDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QSLItemList.h"
#include "ProjectDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CProjectQSLDlg dialog

class CProjectQSLDlg : public CProjectDialog
{
// Construction/Destruction
public:
	CProjectQSLDlg(CWnd* pParent = NULL);   // standard constructor
	~CProjectQSLDlg();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectQSLDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString FindQuickStartPath( short eProject );
	int		GetCDROMDrives(	char *szBuff ); 
	BOOL	InitImageList(	int nListIndex );

	// Generated message map functions
	//{{AFX_MSG(CProjectQSLDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	afx_msg void OnSelchangePaper();
	afx_msg void OnSelchangeOrient();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

// Dialog Data
	//{{AFX_DATA(CProjectQSLDlg)
	enum { IDD = IDD_PROJECT_QSL_PRESSWRITER };
	CQSLItemList	m_cQSLGrid;
	int		m_nOrientation;
	int		m_nPaperSize;
	BOOL	m_fSampleGraphics;
	BOOL	m_fSampleText;
	//}}AFX_DATA

	int m_nListIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _PROJECTQSLDLG_H_
