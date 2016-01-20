/*
// $Workfile: ROTDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/ROTDLG.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:46   Fred
// Initial revision.
// 
//    Rev 1.1   12 Mar 1997 15:52:24   Jay
// Rotation, etc.
// 
//    Rev 1.0   11 Mar 1997 17:43:26   Jay
// Rotate stuff
*/

class CCustomRotateDialog;

#ifndef __ROTDLG_H__
#define __ROTDLG_H__

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomRotateDialog dialog

class CCustomRotateDialog : public CPmwDialog
{
// Construction
public:
	CCustomRotateDialog(CWnd* pParent = NULL);   // standard constructor

	// Get and set the rotation (in degrees).
	void SetRotation(double dRotation)
		{ m_dRotation = dRotation; }
	double GetRotation(void) const
		{ return m_dRotation; }

// Dialog Data
	//{{AFX_DATA(CCustomRotateDialog)
	enum { IDD = IDD_CUSTOM_ROTATE };
	double	m_dRotation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomRotateDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomRotateDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
