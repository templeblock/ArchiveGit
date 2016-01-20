/*
// $Workfile: CARDDOC.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
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
// $Log: /PM8/App/CARDDOC.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 9     2/01/99 9:38a Johno
// Changes for paneled HTML
// 
// 8     1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// Preliminary separate panel backgrounds
// 
// 7     1/27/99 1:48p Johno
// Compile update
// 
// 6     6/18/98 7:05p Psasse
// force first page to appear in workspace for templates
// 
// 5     6/12/98 4:36p Mwilson
// moved handler for personal delivery from doc to view
// 
// 4     5/20/98 5:20p Mwilson
// added handler for remote fulfillment
// 
// 3     4/23/98 4:00p Rlovejoy
// Added DoPostOpen() prototype.
// 
// 2     4/18/98 4:24p Fredf
// T-Shirt printing.
*/

class CCardDoc;
class CHalfCardDoc;
class CNoteCardDoc;

#ifndef __CARDDOC_H__
#define __CARDDOC_H__

/////////////////////////////////////////////////////////////////////////////
// CCardDoc document

class CCardDoc : public CPmwDoc
{
	INHERIT(CCardDoc, CPmwDoc)
	DECLARE_SERIAL(CCardDoc)

protected:
	virtual BOOL ReadDocument(void);

	virtual void FlipProject(FLAGS Flags);

	void FlipCardPanels(void);
	void FlipCardPanel(int nPanel, FLAGS Flags);

	void RemoveGroupBits(GroupObject* pGroup);
	void FixPanelValues(GroupObject* pGroup);

	void GoToFirstPage();

	CCardDoc();			// protected constructor used by dynamic creation

// Attributes
public:
	static ProjectInfo m_SideFoldInfo;
	static ProjectInfo m_TopFoldInfo;

	LPCSTR PanelTitle(int nPanel);

	GroupObjectPtr GetPanelGroup(int nPanel)
		{ return panel_group[nPanel]; }

protected:
	GroupObjectPtr	panel_group[3];		/* One for each panel. */
/*
// Process an object's action.
// This can be overridden by derived document types for special objects!
*/
	virtual VOID process_action(CView *view, PMG_OBJECT_ACTION action, PMGPageObjectPtr object, LPVOID data, PPNT p);
	virtual	BOOL	OnSaveDocument(const char *pszPathName);

	void MergePanelWithPanel(GroupObject* pNewGroup, GroupObject* pOldGroup);
	void RenumberPanel(GroupObject* pGroup, int nPanel, BOOL fRemoveLayout);
	BOOL MergeOldPanels(void);

	CBackgroundObject*	m_pPrintingBackground[3];

	CBackgroundObject*
	GetPrintingBackground(int pb);

   CPageProperties * 
	GetPageProperties(int pp); 

	void
	KillPrintingBackgrounds(void);

	virtual void
	AddBackgroundForDrawing(CPtrList &List, int idx);

public:
// Operations
	BOOL set_panel(SHORT panel);

   CPageProperties * 
	GetPageProperties(); 

	virtual void
	AddBackgroundForDrawing(CPtrList &List);

	virtual int DefaultTextSize(int nDefault);
	virtual void DoPostOpen(void);
/*
// Resize the document to fit on the paper.
*/
	virtual VOID size_to_paper(VOID);

	int FoldType(void) const
		{ return info.project_sub_type; }

public:
// Implementation
	virtual ProjectInfo* SideFoldInfo(void);
	virtual ProjectInfo* TopFoldInfo(void);

protected:
	virtual ~CCardDoc();
	virtual	BOOL OnNewDocument();
	virtual	BOOL OnOpenDocument(const char *pszPathName);
	// Preview overridables.
	virtual BOOL DrawPreview(REFRESH_EXTENT_PTR re, RedisplayContext* rc);
	virtual void GetPreviewBound(PBOX* pBound);
	// "New" helper.
	BOOL CreatePanels(void);
	// "Open" helper.
	BOOL LocatePanels(void);
	// Generated message map functions
protected:
	//{{AFX_MSG(CCardDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CHalfCardDoc document

class CHalfCardDoc : public CCardDoc
{
	INHERIT(CHalfCardDoc, CCardDoc)
	DECLARE_SERIAL(CHalfCardDoc)

protected:
	CHalfCardDoc();			// protected constructor used by dynamic creation

// Attributes

public:
	static ProjectInfo m_SideFoldInfo;
	static ProjectInfo m_TopFoldInfo;

public:
// Operations
	virtual void
	AddBackgroundForDrawing(CPtrList &List);
public:
	virtual ProjectInfo* SideFoldInfo(void);
	virtual ProjectInfo* TopFoldInfo(void);

protected:
	virtual ~CHalfCardDoc();

	virtual	BOOL OnOpenDocument(const char *pszPathName);

	virtual ERRORCODE CreatePages(void);

	void MovePanel(int nPanel, PCOORD lDeltaX, PCOORD lDeltaY);
	BOOL ConvertFromBrochure(void);
	void AutoCropGraphic(PMGPageObject* pObject);

	// Generated message map functions
protected:
	//{{AFX_MSG(CHalfCardDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CNoteCardDoc document

class CNoteCardDoc : public CCardDoc
{
	DECLARE_SERIAL(CNoteCardDoc)

protected:
	CNoteCardDoc();			// protected constructor used by dynamic creation

// Attributes

public:
	static ProjectInfo m_SideFoldInfo;
	static ProjectInfo m_TopFoldInfo;

public:
// Operations
public:
	virtual ProjectInfo* SideFoldInfo(void);
	virtual ProjectInfo* TopFoldInfo(void);

protected:
	virtual ~CNoteCardDoc();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNoteCardDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
