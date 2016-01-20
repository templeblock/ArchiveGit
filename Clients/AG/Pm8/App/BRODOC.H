/*
// $Workfile: BRODOC.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/BRODOC.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 2     6/18/98 7:05p Psasse
// force first page to appear in workspace for templates
// 
//    Rev 1.0   14 Aug 1997 15:18:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:46   Fred
// Initial revision.
// 
//    Rev 1.4   03 Apr 1997 14:31:30   Jay
// Resource cleanup
// 
//    Rev 1.3   01 Sep 1996 13:44:30   Fred
// Always builds album thumbnail from first page
// 
//    Rev 1.2   28 Jul 1996 11:28:38   Jay
// Brochure stuff
// 
//    Rev 1.1   25 Jul 1996 11:16:58   Jay
// More page stuff
// 
//    Rev 1.0   23 Jul 1996 10:39:34   Jay
// Initial revision.
*/

#ifndef __BRODOC_H__
#define __BRODOC_H__

/////////////////////////////////////////////////////////////////////////////
// CBrochureDoc document

class CBrochureDoc : public CPmwDoc
{
	DECLARE_SERIAL(CBrochureDoc)
protected:
	CBrochureDoc();			// protected constructor used by dynamic creation

// Attributes
public:

	static ProjectInfo StandardInfo;
// Operations
public:

// Implementation
protected:
	virtual ~CBrochureDoc();
	virtual	BOOL OnNewDocument();

	virtual	ERRORCODE CreatePages(void);

	// Preview overridables.
	virtual BOOL DrawPreview(REFRESH_EXTENT_PTR re, RedisplayContext* rc);
	virtual void GoToFirstPage();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBrochureDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
