//****************************************************************************
//
// File Name:		FolderDialog.h
//
// Project:			Renaissance framework
//
// Author:			Richard E. Grenfell
//
// Description:	Defines class RFolderDialog.  This dialog can be used for
//						directory selection.
//
// Portability:	Windows Specific
//
// Developed by:	Broderbund Software Inc.
//
//  Copyright (C) 1998 Brøderbund Software, Inc., All Rights Reserved.
//
//  $Logfile: /PM8/Framework/Include/FolderDialog.h $
//   $Author: Gbeddow $
//     $Date: 3/03/99 6:15p $
// $Revision: 1 $
//
//****************************************************************************

#ifndef _FOLDERDIALOG_H
#define _FOLDERDIALOG_H

#include "SaveAsDialog.h"	// Contains definition of the RSaveAsData structure.

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ****************************************************************************
//
// Class Name:		RFolderDialog dialog
// 
//	Description:	Handles displaying a dialog for selecting folders
// 
// ****************************************************************************
//
class FrameworkLinkage RFolderDialog : public CFileDialog
{
// Construction
public:
	RFolderDialog(CString& path, int nCurHTMLExt, CWnd* pParent = NULL);   // standard constructor

	// Return the path of the current directory	
	CString GetDirPath() { return m_PathString; }
	// Determine if the user selected to use the full HTML extension.
	int GetUseFullHTMLExt() { return m_nHTML; }
	// If TRUE, the user selected the OK button, otherwise Cancel.
	BOOLEAN OKSelected() { return m_bOK; }
	// Returns the user selected page size as a percentage value.
	YFloatType GetPageSizePercentage();
	
// Dialog Data
	//{{AFX_DATA(RFolderDialog)
		// NOTE: the ClassWizard will add data members here
	int	m_nHTML;
	CComboBox	m_ctlPageSize;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RFolderDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Store if the dialog was exited in a non-cancel style.
   bool 	m_bOK;
	// Hold directory name
	CString		m_PathString;
	// Tracks the page size selection made.
	int m_nPageListIndex;

	// Generated message map functions
	//{{AFX_MSG(RFolderDialog)
	afx_msg void OnHelp();
	virtual BOOL OnInitDialog();
	afx_msg void OnFolder();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _FOLDERDIALOG_H
