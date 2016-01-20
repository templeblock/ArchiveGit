//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/WHWebPg.h 1     3/03/99 6:14p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/WHWebPg.h $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 5     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 4     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 3     6/03/98 9:38p Hforman
// remove num pages page
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __WHWEBPG_H__
#define __WHWEBPG_H__

// WebPage
class CWebPagePageContainer : public CWHelpPageContainer
{
	enum
	{
		ID_WEBPAGE_CATEGORY = 1,
		ID_WEBPAGE_STYLE,
		ID_WEBPAGE_HERE_IT_IS
	};

// Construction
public:
	CWebPagePageContainer(CWHelpWizPropertySheet* pSheet);
	virtual ~CWebPagePageContainer();
};


/////////////////////////////////////////////////////////////////////////////
// CWHWebPageView dialog

class CWHWebPageView : public CWView
{
// Construction
public:
	CWHWebPageView(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CWHWebPageView)
	//}}AFX_DATA

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWHWebPageView)
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL	

protected:
	// Generated message map functions
	//{{AFX_MSG(CWHWebPageView)
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __WHWEBPG_H__
