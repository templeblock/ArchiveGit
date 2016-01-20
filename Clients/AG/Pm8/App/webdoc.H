/*
// $Workfile: webdoc.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/webdoc.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 9     12/22/98 10:30a Johno
// Moved most HTML conversion stuff to pmwdoc.*
// 
// 8     6/18/98 7:05p Psasse
// force first page to appear in workspace for templates
// 
// 7     5/28/98 4:58p Johno
// Changes for HTML compliance groups
// 
// 6     5/26/98 6:55p Johno
// 
// 5     4/03/98 4:42p Johno
// Changes for page links
// 
// 4     3/02/98 5:04p Johno
// Compile update
// 
// 3     12/18/97 3:21p Dennis
// Added overide for DefaultFace()
// 
// 2     12/11/97 4:25p Dennis
// Added override for size_to_paper()
// 
// 1     12/11/97 1:43p Dennis
// 
*/

#ifndef __WEBDOC_H__
#define __WEBDOC_H__

#include "newsdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CWebDoc document

class CWebDoc : public CNewsletterDoc
{
	INHERIT(CWebDoc, CNewsletterDoc)
   
   DECLARE_SERIAL(CWebDoc)
protected:
	CWebDoc();			// protected constructor used by dynamic creation

// Attributes
public:

	static ProjectInfo StandardInfo;
// Operations
public:
	virtual VOID size_to_paper(VOID) // don't allow resizing
      { }

   // Get Default Face
	virtual LPCSTR DefaultFace(void);

// Implementation
protected:
	virtual ~CWebDoc();

	// Preview overridables.
	void GoToFirstPage();


	// Generated message map functions
protected:
	//{{AFX_MSG(CWebDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

   //CBackgroundObject *m_pBackgroundObject;
   
   public:
/*
   CBackgroundObject * GetBackgroundObject()
      { return m_pBackgroundObject; }
*/
};

#endif
