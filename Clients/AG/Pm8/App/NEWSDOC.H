/*
// $Workfile: NEWSDOC.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/NEWSDOC.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 8     2/01/99 2:51p Blee
// Added OnUpdateAddPage override
// 
// 7     8/19/98 9:06a Mwilson
// added project info
// 
// 6     8/17/98 5:36p Mwilson
// changed Tshirts to be based off of newsletters instead of posters
// 
// 5     8/06/98 4:32p Mwilson
// added new craft project type
// 
// 4     6/18/98 7:05p Psasse
// force first page to appear in workspace for templates
// 
// 3     3/19/98 2:32p Jayn
// Finish up master pages.
// 
// 2     3/10/98 5:24p Jayn
// Master page changes
// 
//    Rev 1.0   14 Aug 1997 15:22:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:44   Fred
// Initial revision.
// 
//    Rev 1.5   03 Apr 1997 14:31:32   Jay
// Resource cleanup
// 
//    Rev 1.4   04 Oct 1996 10:44:24   Jay
// Improvements, etc.
// 
//    Rev 1.3   02 Oct 1996 08:28:52   Jay
// Picture reference newsletter builder.
// 
//    Rev 1.2   01 Sep 1996 14:04:30   Fred
// Next page when on last page brings up add pages dialog
// 
//    Rev 1.1   01 Sep 1996 13:44:30   Fred
// Always builds album thumbnail from first page
// 
//    Rev 1.0   23 Jul 1996 10:39:56   Jay
// Initial revision.
*/

#ifndef __NEWSDOC_H__
#define __NEWSDOC_H__

/////////////////////////////////////////////////////////////////////////////
// CNewsletterDoc document

class CNewsletterDoc : public CPmwDoc
{
	DECLARE_SERIAL(CNewsletterDoc)
protected:
	CNewsletterDoc();			// protected constructor used by dynamic creation

// Attributes
public:

	static ProjectInfo StandardInfo;
// Operations
public:
	virtual bool IsMultiPageProjectType() {	return TRUE; }

// Implementation
protected:
	virtual ~CNewsletterDoc();
	virtual	BOOL	OnNewDocument();
	virtual	BOOL	OnOpenDocument(const char *pszPathName);

	// Preview overridables.
	virtual BOOL DrawPreview(REFRESH_EXTENT_PTR re, RedisplayContext* rc);
	virtual void GoToFirstPage();

	// Get a string for the current page (used by page macros).
	virtual CString GetPageString(DB_RECORD_NUMBER lPage = 0);

	BOOL UpdateMasterPages(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CNewsletterDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void BuildPictureNewsletter(void);
	void BuildPicturesPage(BOOL fBonusPack, LPCSTR pszName, int nBasePage, PBOX& PageBound);
	void BuildEmptyPictures(void);
	void CreatePattern(PBOX Bound);

public:
	virtual BOOL CanPageBack(void);
	virtual void DoPageBack(void);
	virtual BOOL CanPageNext(void);
	virtual void DoPageNext(void);
};

/////////////////////////////////////////////////////////////////////////////
// CTShirtDoc document

class CTShirtDoc : public CNewsletterDoc
{
	DECLARE_SERIAL(CTShirtDoc)
protected:
	CTShirtDoc();			// protected constructor used by dynamic creation

// Attributes
public:
	static ProjectInfo StandardInfo;

// Operations
public:
	virtual void GoToFirstPage();

// Implementation
protected:
	virtual ~CTShirtDoc();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTShirtDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CCraftDoc : public CNewsletterDoc
{
	DECLARE_SERIAL(CCraftDoc)
protected:
	CCraftDoc();			// protected constructor used by dynamic creation

// Attributes
public:
	static ProjectInfo StandardInfo;

// Operations
public:

// Implementation
protected:
	virtual ~CCraftDoc();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCraftDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
