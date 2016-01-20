/*
// $Workfile: WHlpWiz.h $
// $Revision: 1 $
// $Date: 3/03/99 6:14p $
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
// $Log: /PM8/App/WHlpWiz.h $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 2:47p Hforman
// name change
// 
// 20    6/05/98 9:04a Fredf
// Fixed merge screwup.
// 
// 19    6/05/98 8:41a Fredf
// Document Flags
// 
// 18    6/04/98 9:52p Hforman
// add InsertNewlines()
// 
// 17    6/03/98 9:16p Psasse
// Fixed initialization of new parameters problem
// 
// 16    6/03/98 7:06p Rlovejoy
// Code to get description string.
// 
// 15    6/03/98 2:26p Psasse
// interim WithHelp Label support
// 
// 14    6/02/98 2:31p Rlovejoy
// Added ModifyPreviewName().
// 
// 13    6/01/98 8:22p Hforman
// moved preview window to CWHelpPropertyPage
// 
// 12    5/31/98 5:04p Rlovejoy
// Added InitializedSuccessfully() function.
// 
// 11    5/31/98 3:31p Rlovejoy
// Included members to strip out unsupporetd project types.
// 
// 10    5/29/98 9:38p Hforman
// background drawing
// 
// 9     5/29/98 6:41p Rlovejoy
// Took out unused variable.
// 
// 8     5/29/98 6:23p Hforman
// fixed back/finish button problems; started working on background tiling
// 
// 7     5/28/98 8:31p Hforman
// added drawing of PrintMaster Man
// 
// 6     5/28/98 6:36p Rlovejoy
// Changes to learn more about branching direction.
// 
// 5     5/27/98 7:00p Hforman
// make RetrieveDlgInfo() NOT pure virtual
// 
// 4     5/27/98 5:24p Rlovejoy
// Added search string retrieval.
// 
// 3     5/26/98 3:02p Rlovejoy
// Massive revisions.
// 
// 2     4/16/98 5:58p Hforman
// remove obsolete header inclue
// 
//    Rev 1.0   14 Aug 1997 15:19:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:22   Fred
// Initial revision.
// 
// 
*/

#ifndef _WHLPWIZ_H_
#define _WHLPWIZ_H_

#include "pmwdoc.h"
#include "collmgr.h"

class CWHelpWizPropertySheet;

/////////////////////////////////////////////////////////////////////////////
// CWHelpPageContainer base class

class CWHelpPageContainer
{
// Construction
public:
	CWHelpPageContainer(CWHelpWizPropertySheet* pSheet);
	virtual ~CWHelpPageContainer();

// Attributes
public:
	void SetUserText1(LPCTSTR lpszText)		{ m_strText1 = lpszText; }
	LPCTSTR GetUserText1()						{ return m_strText1; }
	void SetUserText1Valid(BOOL fValid)		{ m_fText1Valid = fValid; }
	BOOL UserText1Valid()						{ return m_fText1Valid; }

	void SetUserText2(LPCTSTR lpszText)		{ m_strText2 = lpszText; }
	LPCTSTR GetUserText2()						{ return m_strText2; }
	void SetUserText2Valid(BOOL fValid)		{ m_fText2Valid = fValid; }
	BOOL UserText2Valid()						{ return m_fText2Valid; }

// Implementation
public:
	void AddPages();
	void RemovePages(int nOffset);

	virtual void RetrieveDlgInfo(UINT nIDContainerItem, void* stDlgInfo) {}
	virtual void StoreUserInfo(UINT nIDContainerItem, int nChoice) {}
	virtual void ModifyPreviewName(UINT nIDContainerItem, CString& csName) {}

protected:	
	CPtrArray					m_aPages;		// array of pointers to property pages
	CWHelpWizPropertySheet*	m_pSheet;

	CString		m_strText1, m_strText2;			// user fill-in strings (if needed)
	BOOL			m_fText1Valid, m_fText2Valid;	// fill-in field valid?

};

/////////////////////////////////////////////////////////////////////////////
// CWHTable

#define N_WIZ_TITLE_STRINGS	(2)
#define N_WIZ_TABLE_COLUMNS	(8)

class CWHTable
{
	// Table is stored in array of strings
	CMlsStringArray m_csaLine;

	// Attributes
	int m_nTitleStrings;
	int m_nColumns;
	char m_cListEnd;

	int m_nIndex;
	CList<int, int&> m_IndexStack;

// Construction
public:
	CWHTable(
		int nTitleStrings = N_WIZ_TITLE_STRINGS,
		int nColumns = N_WIZ_TABLE_COLUMNS,
		char cListEnd = '!');

// Attributes
public:
	BOOL IsEmpty()
	{
		return (m_csaLine.GetSize() == 0);
	}

// Operations
protected:
	void IncrementRow(int& nIndex)
	{
		nIndex += m_nColumns;
	}
	void IncrementColumn(int& nIndex)
	{
		nIndex++;
	}
	void FindChoiceIndex(int nChoice, int& nIndex);
	BOOL AddFoundString(CStringArray& csStrings, int nIndex);
	void InsertNewlines(CString& str);

public:
	void LoadTable(CString csTableName);
	void BranchAhead(int nBranchIndex);
	void BranchBack();
	void GetColumnStrings(CStringArray& csStrings, BOOL fKillUnsupportedTypes);
	void GetData(int nChoice, CString& csDataType, CString& csName, CString& csDescription);
	void GetSearchStrings(CStringArray& csSearchStrings);
};

/////////////////////////////////////////////////////////////////////////////
// CWHelpWizPropertySheet

class CWHelpWizPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CWHelpWizPropertySheet)

// Construction
public:
	CWHelpWizPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CWHelpWizPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	void ConstructSheet();
	BOOL InitializedSuccessfully();

// Attributes
public:
	CBitmap* GetBkgrndBmp() { return &m_bmpBkgrnd; }

	void SetActiveContainer(int nActiveContainer)
	{
		m_nActiveContainer = nActiveContainer;
	}
	int GetActiveContainer()
	{
		return m_nActiveContainer;
	}

   CString GetTmpProject() 
	{ 
		return m_csTmpDoc; 
	}
   void SetTmpProject(LPCSTR project) 
	{ 
		m_csTmpDoc = project; 
	}
   CString GetSelProject() 
	{ 
		return m_csSelectedDoc; 
	}
   void SetSelProject(LPCSTR project) 
	{ 
		m_csSelectedDoc = project;
	}

	WORD GetDocumentFlags(void) const
		{ return m_wDocumentFlags; }
	void SetDocumentFlags(WORD wSet, WORD wClear)
		{ m_wDocumentFlags |= wSet; m_wDocumentFlags &= ~wClear; }

// Operations
public:
	void AddActivePages();
	void RemoveActivePages();
	
	// Table operations
	void FindBranchAhead(int nBranchIndex);
	void FindBranchBack();
	int FollowBranch(CPropertyPage* pPage);
	void GetDlgStrings(CStringArray& csStrings, BOOL fKillUnsupportedTypes = FALSE)
	{
		m_WizTable.GetColumnStrings(csStrings, fKillUnsupportedTypes);
	}
	void RetrieveSelectedData(int nChoice, CString& csDataType, CString& csName, CString& csDescription);
	void RetrieveSearchStrings(CStringArray& csSearchStrings);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWHelpWizPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWHelpWizPropertySheet();

protected:
	CBitmap m_bmpBkgrnd;

	CPtrArray m_aPageContainers;		// page containers for each possible path
	int m_nActiveContainer;
	
	CPropertyPage* m_pProjectType;	// constant property page

	// The with help data table
	CWHTable m_WizTable;
	int m_nBranchDirection;				// -1 for back, 0 for none, 1 for ahead
	int m_nBranchIndex;
	CPropertyPage* m_pLastBranch;		// the page we were on last when m_nBranchDirection was set

   // Dialog title string
   // keep dialog path string, append name at each branch
   CString m_csDialogTitle;
   
   CString m_csSelectedDoc;			// ready made document template
   CString m_csTmpDoc;					// edited ready made doc passed to workspace

	WORD m_wDocumentFlags;				// flags to use for document creation

	// Generated message map functions
protected:
	//{{AFX_MSG(CWHelpWizPropertySheet)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _WHLPWIZ_H_
