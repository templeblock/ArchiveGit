/*
// $Workfile: WHGreet.h $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
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
// $Log: /PM8/App/WHGreet.h $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 2:47p Hforman
// name change
// 
// 7     9/14/98 12:06p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 6     6/01/98 11:18p Hforman
// fixing IDs, etc.
// 
// 5     5/31/98 3:30p Rlovejoy
// 
// 4     5/31/98 12:35p Rlovejoy
// Update.
// 
// 3     5/28/98 6:35p Rlovejoy
// Incremental revision.
// 
// 2     5/26/98 3:02p Rlovejoy
// Massive revisions.
// 
//    Rev 1.0   14 Aug 1997 15:19:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:22   Fred
// Initial revision.
// 
//    Rev 1.1   22 Apr 1997 13:24:34   Jay
// Line-break after #endif
// 
//    Rev 1.0   21 Apr 1997 18:38:02   doberthur
// Initial revision.
// 
//    
*/

#ifndef _WHGREET_H_
#define _WHGREET_H_

#include "WHlpWiz.h"
#include "WHlpCmn.h"
#include "WHlpView.h"

// Greeting card
class CWGreetingPageContainer : public CWHelpPageContainer
{
	// Greeting card pages
	enum {
		IDe_GREETING_CATEGORY = 1,
		IDe_GREETING_TYPE,
		IDe_GREETING_ART,
		IDe_GREETING_ENTER_NAMES,
		IDe_GREETING_HERE_IT_IS,
		IDe_GREETING_HELP
	};

public:

	// Category choices (must correspond to spreadsheet!)
	enum
	{
		CATEGORY_ANNIVERSARY = 0,
		CATEGORY_BIRTHDAY,
		CATEGORY_HOLIDAY,
		CATEGORY_ROMANTIC,
		CATEGORY_INVITATION
	};

	// Invitation types (must correspond to spreadsheet!)
	// Need these for special case
	enum
	{
		INV_TYPE_BIRTHDAY = 0,
		INV_TYPE_XMAS,
		INV_TYPE_GRADUATION,
		INV_TYPE_NEWYEARS,
		INV_TYPE_WEDDING
	};

// Construction
public:
	CWGreetingPageContainer(CWHelpWizPropertySheet* pSheet);
	virtual ~CWGreetingPageContainer();

// Implementation
public:
	void StoreUserInfo(UINT nIDContainerItem, int nChoice);

	int		GetCategory()						{ return m_nCategory; }
	int		GetType()							{ return m_nType; }

protected:
	int		m_nCategory;
	int		m_nType;
	CString	m_strText1;
	CString	m_strText2;

};

/////////////////////////////////////////////////////////////////////////////
// CWGreetingDesignPage dialog

class CWGreetingDesignPage : public CWHelpCommonPropertyPage
{
// Construction
public:
	CWGreetingDesignPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CWGreetingDesignPage)
	CButton m_btnFront;
	CButton m_btnInside;
	//}}AFX_DATA
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWGreetingDesignPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetCardPanel(SHORT panel);

	// Generated message map functions
	//{{AFX_MSG(CWGreetingDesignPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnViewFront();
	afx_msg void OnViewInside();
	afx_msg void OnSelchangeChoiceList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWPersonalize dialog

class CWPersonalize : public CWHelpPropertyPage
{
// Construction
public:
	CWPersonalize(CWHelpPageContainer* pContainer, UINT nIDContainerItem); 

// Dialog Data
	//{{AFX_DATA(CWPersonalize)
	enum { IDD = IDD_WH_PERSON1 };
	CEdit	m_edit2;
	CEdit	m_edit1;
	CString	m_strCaption1;
	CString	m_strCaption2;
	CString	m_strText1;
	CString	m_strText2;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWPersonalize)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWPersonalize)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWGreetingView dialog

class CWGreetingView : public CWView
{
// Construction
public:
	CWGreetingView(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CWGreetingView)
	CButton m_btnFront;
	CButton m_btnInside;
	//}}AFX_DATA

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWGreetingView)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL	


protected:
	void SetCardPanel(SHORT panel);

	// Generated message map functions
	//{{AFX_MSG(CWGreetingView)
	virtual BOOL OnInitDialog();
	afx_msg void OnViewFront();
	afx_msg void OnViewInside();
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
