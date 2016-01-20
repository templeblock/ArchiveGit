/*
// $Workfile: matchset.h $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
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
// $Log: /PM8/App/matchset.h $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 1     12/01/98 9:12a Mwilson
// 
// 
// 4     5/19/98 3:02p Dennis
// Added History
// 
*/

#ifndef __MATCHSET_H__
#define __MATCHSET_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "pictprvw.h"
#include "error.h"
#include "PmwDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CMatchingSetsDlg dialog


class CCatData;

class CMatchingSetsDlg : public CPmwDialog
{
	INHERIT(CMatchingSetsDlg, CPmwDialog)
// Construction
public:
	CMatchingSetsDlg(CWnd* pParent = NULL);   // standard constructor
   virtual ~CMatchingSetsDlg();

	CStringArray& GetProjArray(){return m_csaSelectedProjects;}

// Dialog Data
	//{{AFX_DATA(CMatchingSetsDlg)
	enum { IDD = IDD_MATCHING_SETS };
	CComboBox	m_cSubCatList;
	CCheckListBox	m_cProjList;
	CComboBox	m_cCatList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMatchingSetsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMatchingSetsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeCatList();
	afx_msg void OnSelChangeSubCatList();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Helpers


protected:
// Data Members
	CStringArray m_csaSelectedProjects;
   CPicturePreview   m_cppPackPreview;
	CTypedPtrList<CObList, CCatData*> m_cplCategoryList; 
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __MATCHSET_H__
