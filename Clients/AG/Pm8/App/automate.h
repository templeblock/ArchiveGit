/*
// $Workfile: automate.h $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/automate.h $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 5     2/06/98 11:34a Jayn
// More complete automation support.
// 
// 4     12/23/97 10:26a Jayn
// 
// 3     12/15/97 12:45p Jayn
// Automation
// 
// 2     12/12/97 11:53a Jayn
// Beginning of automation support.
// 
// 1     12/12/97 11:27a Jayn
*/

#ifndef __AUTOMATE_H__
#define __AUTOMATE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CServerDlgAutoProxy command target

class CPrintMasterDispatch : public CCmdTarget
{
	DECLARE_DYNCREATE(CPrintMasterDispatch)

	CPrintMasterDispatch();           // protected constructor used by dynamic creation

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintMasterDispatch)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPrintMasterDispatch();

	// Generated message map functions
	//{{AFX_MSG(CPrintMasterDispatch)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CPrintMasterDispatch)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CPrintMasterDispatch)
	afx_msg BSTR RunArtGallery(long hWnd);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // ifndef __AUTOMATE_H__
