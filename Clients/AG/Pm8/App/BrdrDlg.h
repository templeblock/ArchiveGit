//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/BrdrDlg.h 1     3/03/99 6:03p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/BrdrDlg.h $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 5     2/05/98 11:59a Hforman
// added #defines for values passed back to caller
// 
// 4     1/09/98 6:52p Hforman
// 
// 3     1/09/98 11:05a Hforman
// interim checkin
// 
// 2     1/06/98 7:05p Hforman
// work-in-progress
// 
// 1     12/24/97 1:05p Hforman
// 

#ifndef __BRDRDLG_H__
#define __BRDRDLG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CAddBorderDlg dialog


// return values when IDOK clicked
#define ID_BORDER_CREATE	100	// user wants to create border from scratch
#define ID_BORDER_SELECT	101	// user wants to select border from browser

class CAddBorderDlg : public CDialog
{
	enum { FromScratch = 0, SelectExisting };

// Construction
public:
	CAddBorderDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddBorderDlg)
	enum { IDD = IDD_ADD_BORDER };
	int		m_nChoice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddBorderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddBorderDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef __BRDRDLG_H__
