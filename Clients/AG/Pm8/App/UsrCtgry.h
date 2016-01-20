//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/UsrCtgry.h 1     3/03/99 6:13p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
// User Category Management.
//
//
// $Log: /PM8/App/UsrCtgry.h $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 6     7/26/98 7:05p Hforman
// changed baseclass from CDialog to CPmwDialog for palette handling
// 
// 5     7/23/98 10:39a Hforman
// not allowing names with just spaces
// 
// 4     5/19/98 2:34p Hforman
// checking that category doesn't already exist
// 
// 3     5/06/98 6:17p Hforman
// 
// 2     4/30/98 6:45p Hforman
// fix maximum category name length
// 
// 1     4/13/98 11:11a Hforman
// initial version
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __USRCTGRY_H__
#define __USRCTGRY_H__

#include "PMWColl.h"
#include "PmwDlg.h"

class CUserCategoryManager
{
public:
	CUserCategoryManager();
	~CUserCategoryManager();

	BOOL			Initialize(CPMWCollection::CollectionTypes type);
	void			GetUserCategories(CStringArray& catArray);
	BOOL			GetNewCategory(CString& strCat);

protected:

	BOOL					m_fInitialized;
	CPMWCollection::CollectionTypes m_CollType;
	CPMWCollection*	m_pUserCollection;
	CStringArray		m_CategoryArray;
};

/////////////////////////////////////////////////////////////////////////////
// CAddUserCategory dialog

class CAddUserCategory : public CPmwDialog
{
// Construction
public:
	CAddUserCategory(const CStringArray& catArray, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CAddUserCategory)
	enum { IDD = IDD_ADD_USER_CATEGORY };
	CEdit	m_editCategory;
	CString	m_strCategory;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddUserCategory)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CStringArray		m_CategoryArray;	// array of current categories passed in

	// Generated message map functions
	//{{AFX_MSG(CAddUserCategory)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__USRCTGRY_H__
