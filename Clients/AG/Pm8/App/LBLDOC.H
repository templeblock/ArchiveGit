/*
// $Workfile: LBLDOC.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LBLDOC.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 5     2/15/99 1:04p Psasse
// size_to_paper() becomes a public function
// 
// 4     8/24/98 6:02p Jayn
// Labels now preview out to label edge (for rounded case).
// 
// 3     7/15/98 11:49a Johno
// Added GoToFirstPage() and DrawPreview() to CPostCardDoc
// 
// 2     6/12/98 11:09a Cboggio
// added CBizCard::size_to_paper for localized business cards
// 
//    Rev 1.2   26 Sep 1997 16:49:54   johno
// Added PostCardDoc
// 
//    Rev 1.1   19 Sep 1997 17:34:30   johno
// Changes for new Post Card project type
// (preliminary)
// 
// 
//    Rev 1.0   14 Aug 1997 15:22:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:26   Fred
// Initial revision.
// 
//    Rev 1.1   03 Apr 1997 14:31:32   Jay
// Resource cleanup
// 
//    Rev 1.0   14 Mar 1996 13:38:14   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:36   FRED
// Initial revision.
// 
//    Rev 1.3   30 Aug 1995 14:13:40   JAY
// Added PrintPaperInfo() method.
// 
//    Rev 1.2   09 Aug 1995 13:11:24   JAY
// Misc changes.
// 
//    Rev 1.1   31 Jul 1995 10:26:58   JAY
// New projects.
// 
//    Rev 1.0   20 Jul 1995 12:52:20   JAY
// Initial revision.
*/

#ifndef __LBLDOC_H__
#define __LBLDOC_H__

/////////////////////////////////////////////////////////////////////////////
// CLabelDoc document

class CLabelDoc : public CPmwDoc
{
	DECLARE_SERIAL(CLabelDoc)
protected:
	CLabelDoc();			// protected constructor used by dynamic creation

// Attributes
public:

	static ProjectInfo StandardInfo;
// Operations
public:

	virtual int DefaultTextSize(int nDefault);
/*
// Resize the document to fit on the paper.
*/

	virtual VOID size_to_paper(VOID);


// Implementation
protected:
	virtual ~CLabelDoc();

	// Get the preview bound.
	virtual void GetPreviewBound(PBOX* pBound);

/*
// Return the paper info to use for tile calculations.
*/

	virtual CPaperInfo* PrintPaperInfo(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CLabelDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CBizCardDoc document

class CBizCardDoc : public CLabelDoc
{
	DECLARE_SERIAL(CBizCardDoc)
protected:
	CBizCardDoc();			// protected constructor used by dynamic creation

// Attributes
public:

	static ProjectInfo StandardInfo;

#ifdef LOCALIZE
	virtual VOID size_to_paper(VOID);
#endif

// Operations
public:

// Implementation
protected:
	virtual ~CBizCardDoc();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBizCardDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPostCardDoc document

class CPostCardDoc : public CLabelDoc
{
	DECLARE_SERIAL(CPostCardDoc)
protected:
	CPostCardDoc();			// protected constructor used by dynamic creation

// Attributes
public:

	static ProjectInfo StandardInfo;
// Operations
public:

// Implementation
protected:
	virtual ~CPostCardDoc();

	virtual	ERRORCODE CreatePages(void);
	virtual BOOL DrawPreview(REFRESH_EXTENT_PTR re, RedisplayContext* rc);
	virtual void GoToFirstPage();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPostCardDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
