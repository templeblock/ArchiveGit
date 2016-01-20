//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/WHLtrhd.h 1     3/03/99 6:14p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/WHLtrhd.h $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 2:47p Hforman
// name change
// 
// 7     6/01/98 11:18p Hforman
// fixing IDs, etc.
// 
// 6     5/29/98 6:24p Hforman
// finishing basic functionality
// 
// 5     5/28/98 8:39p Hforman
// work in progress
// 
// 4     5/27/98 7:03p Hforman
// 
// 3     5/26/98 5:50p Hforman
// work in progress
// 
// 2     5/26/98 3:02p Rlovejoy
// Massive revisions.
// 
//    Rev 1.0   14 Aug 1997 15:19:48   Fred
// Initial revision.
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __WHLTRHD_H__
#define __WHLTRHD_H__

#include "WHlpWiz.h"
#include "WHlpView.h"


// Letterhead
class CWLetterheadPageContainer : public CWHelpPageContainer
{
public:

	// Letterhead pages
	enum
	{
		ID_LETTER_CATEGORY = 1,
		ID_LETTER_DESIGN,
		ID_LETTER_INFO,
		ID_LETTER_HERE_IT_IS
	};

	// Category choices (must correspond to spreadsheet!)
	enum
	{
		CATEGORY_BUSINESS,
		CATEGORY_FLORAL,
		CATEGORY_PERSONAL
	};


// Construction
public:
	CWLetterheadPageContainer(CWHelpWizPropertySheet* pSheet);
	virtual ~CWLetterheadPageContainer();

// Implementation
public:
	virtual void StoreUserInfo(UINT nIDContainerItem, int nChoice);

	int		GetCategory()							{ return m_nCategory; }

protected:
	int		m_nCategory;
};

/////////////////////////////////////////////////////////////////////////////
// CWLetterheadInfoPage dialog

class CWLetterheadInfoPage : public CWHelpPropertyPage
{
// Construction
public:
	CWLetterheadInfoPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CWLetterheadInfoPage)
	enum { IDD = IDD_WH_PERSON1 };
	CString	m_strNameCaption;
	CString	m_strAddressCaption;
	CString	m_strName;
	CString	m_strAddress;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWLetterheadInfoPage)
public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWLetterheadInfoPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWLetterheadView dialog

class CWLetterheadView : public CWView
{
// Construction
public:
	CWLetterheadView(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CWLetterheadView)
	//}}AFX_DATA

public:
   // incremented for each WH_TMPx file
//   static int m_nTmpFileNumber;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWLetterheadView)
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL	


protected:
	// Generated message map functions
	//{{AFX_MSG(CWLetterheadView)
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __WHLTRHD_H__
