/*
// $Workfile: NEWSVIEW.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/NEWSVIEW.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 6     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 5     8/17/98 5:36p Mwilson
// changed Tshirts to be based off of newsletters instead of posters
// 
// 4     8/06/98 4:32p Mwilson
// added new craft project type
// 
// 3     3/17/98 11:56a Jayn
// Master-pages, double-sided flag.
// 
// 2     3/10/98 5:24p Jayn
// Master page changes
// 
//    Rev 1.0   14 Aug 1997 15:22:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:44   Fred
// Initial revision.
// 
//    Rev 1.3   06 Aug 1996 11:17:06   Jay
//  
// 
//    Rev 1.2   25 Jul 1996 13:52:04   Jay
// Gadget stuff.
// 
//    Rev 1.1   24 Jul 1996 07:53:46   Jay
// Added print dialog
// 
//    Rev 1.0   23 Jul 1996 10:39:58   Jay
// Initial revision.
*/

#ifndef __NEWSVIEW_H__
#define __NEWSVIEW_H__

/////////////////////////////////////////////////////////////////////////////
// CNewsletterView view

class CNewsletterView : public CPmwView
{
	DECLARE_DYNCREATE(CNewsletterView)
protected:
	CNewsletterView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

public:
	// Gadget interface.
	virtual BOOL GadgetVisible(UINT uID);
	virtual void GadgetText(UINT uID, CString& csText);
	virtual LPCSTR GadgetMenuName(UINT uID, UINT uPart);

// Implementation
protected:
	virtual ~CNewsletterView();
	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view

	virtual CPmwPrint* CreatePrintDialog(void);
	virtual SHORT OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);

	virtual void CreateEditDesktop(void);

	void RefreshMasterItems(void);

	// Generated message map functions
	DWORD m_dwPushedPageIndex;
protected:
	//{{AFX_MSG(CNewsletterView)
	afx_msg void OnGotoMasterPage();
	afx_msg void OnUpdateGotoMasterPage(CCmdUI* pCmdUI);
	afx_msg void OnIgnoreMasterItems();
	afx_msg void OnUpdateIgnoreMasterItems(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CTShirtView : public CNewsletterView
{
	DECLARE_DYNCREATE(CTShirtView)
protected:
	CTShirtView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

public:
	// Gadget interface.
	virtual BOOL GadgetVisible(UINT uID);

// Implementation
protected:
	virtual ~CTShirtView();

	virtual CPmwPrint* CreatePrintDialog(void);
	virtual SHORT OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);

	// Generated message map functions
protected:
	//{{AFX_MSG(CTShirtView)
	afx_msg void OnUpdateAddPage(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CCraftView : public CTShirtView
{
	DECLARE_DYNCREATE(CCraftView)
protected:
	CCraftView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

public:

// Implementation
protected:
	virtual ~CCraftView();

	virtual CPmwPrint* CreatePrintDialog(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CCraftView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
