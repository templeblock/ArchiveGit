//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/WHPoster.h 1     3/03/99 6:14p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/WHPoster.h $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 2:47p Hforman
// name change
// 
// 6     6/04/98 9:44p Hforman
// add CATEGORY_HOME
// 
// 5     6/01/98 11:18p Hforman
// fixing IDs, etc.
// 
// 4     5/29/98 6:24p Hforman
// finishing basic functionality
// 
// 3     5/28/98 8:39p Hforman
// work in progress
// 
// 2     5/28/98 12:56p Hforman
// 
// 1     5/28/98 12:37p Hforman
// initial revision
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __WHPOSTER_H__
#define __WHPOSTER_H__

#include "WHlpWiz.h"
#include "WHlpView.h"


// Poster
class CWPosterPageContainer : public CWHelpPageContainer
{
public:

	// Poster pages
	enum
	{
		ID_POSTER_CATEGORY = 1,
		ID_POSTER_DESIGN,
		ID_POSTER_INFO,
		ID_POSTER_HERE_IT_IS
	};

	// Category choices (must correspond to spreadsheet!)
	enum
	{
		CATEGORY_EVENT,
		CATEGORY_FORSALE,
		CATEGORY_HOME,
		CATEGORY_KIDSTUFF
	};


// Construction
public:
	CWPosterPageContainer(CWHelpWizPropertySheet* pSheet);
	virtual ~CWPosterPageContainer();

// Implementation
public:
	virtual void StoreUserInfo(UINT nIDContainerItem, int nChoice);

	int		GetCategory()						{ return m_nCategory; }

protected:
	int		m_nCategory;
};

/////////////////////////////////////////////////////////////////////////////
// CWPosterInfoPage dialog

class CWPosterInfoPage : public CWHelpPropertyPage
{
// Construction
public:
	CWPosterInfoPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CWPosterInfoPage)
	enum { IDD = IDD_WH_PERSON1 };
	CStatic	m_ctrlCaption2;
	CStatic	m_ctrlCaption1;
	CEdit	m_edit2;
	CEdit	m_edit1;
	CString	m_strCaption1;
	CString	m_strCaption2;
	CString	m_strText1;
	CString	m_strText2;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWPosterInfoPage)
public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWPosterInfoPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWPosterView dialog

class CWPosterView : public CWView
{
// Construction
public:
	CWPosterView(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CWPosterView)
	//}}AFX_DATA

public:
   // incremented for each WH_TMPx file
//   static int m_nTmpFileNumber;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWPosterView)
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL	


protected:
	// Generated message map functions
	//{{AFX_MSG(CWPosterView)
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __WHPOSTER_H__
