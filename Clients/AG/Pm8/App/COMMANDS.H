/*
// Specific command support.
//
// $Workfile: COMMANDS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/COMMANDS.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 7     7/14/98 6:34p Jayn
// 
// 6     7/02/98 3:48p Johno
// Added ProcessHyperlinks() - use so ReDo works.
// 
// 5     7/01/98 4:10p Johno
// Added mHyperlinksToMe to CCmdDeletePage (saves hyperlink record numbers
// that were pointing to a deleted page)..
// Used in CCmdDeletePage::UndoIt(), CCmdDeletePage::Snapshot()
// 
// 4     2/12/98 1:08p Hforman
// add CCmdBorderChange
// 
// 3     11/14/97 6:54p Hforman
// redid CCmdGuideChange
// 
// 2     11/14/97 11:56a Hforman
// add undo command for Guides
// 
//    Rev 1.0   14 Aug 1997 15:19:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:12   Fred
// Initial revision.
// 
//    Rev 1.8   18 Jul 1997 14:49:38   Jay
// Moved CObjectStateArray to pmgobj.
// 
//    Rev 1.7   09 Jun 1997 17:30:56   Jay
// Text improvements
// 
//    Rev 1.6   28 Oct 1996 13:53:24   Jay
// Moved from 3.01 source
// 
//    Rev 1.7   22 Oct 1996 08:21:04   Jay
// Did more complete fix for last bug.
// 
//    Rev 1.6   22 Oct 1996 00:23:08   Jay
// Fixed a sequencing/ownership bug with deleted text frames and the text record.
// 
//    Rev 1.5   01 Aug 1996 17:45:30   Jay
// Better handling panels, pages, and undo.
// 
//    Rev 1.4   26 Jul 1996 18:15:32   Jay
// Add/delete page commands
// 
//    Rev 1.3   26 Jul 1996 11:39:22   Jay
// Page stuff
// 
//    Rev 1.2   15 Jul 1996 17:46:26   Jay
//  
// 
//    Rev 1.1   12 Jul 1996 18:01:18   Jay
// Undo/redo commands now handle frame marking and updating
// 
//    Rev 1.0   14 Mar 1996 13:26:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:28   FRED
// Initial revision.
// 
//    Rev 1.3   01 Sep 1995 14:24:36   JAY
// Added undo for title text edit.
// 
//    Rev 1.2   15 Mar 1995 10:48:58   JAY
// Support for new text editing
// 
//    Rev 1.1   09 Mar 1995 08:39:00   JAY
// More undo.
// 
//    Rev 1.0   15 Feb 1995 17:17:14   JAY
// Initial revision.
*/

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

class CCmdSelects;
class CCmdRelayerSelects;
class CCmdDeleteSelects;
class CCmdSendToFront;
class CCmdSendToBack;
class CCmdForwardOne;
class CCmdBackOne;
class CCmdDeletePage;
class CCmdInsertPage;
class CObjectStateArray;
class CCmdChangeSelects;

class CCardView;
class CPmwView;
class CBEObject;
class BorderObject;

/*
// Base level class (above CCommand) which operates on the selected objects
// on a page.
*/

class CCmdSelects : public CCommand
{
public:
	CCmdSelects(WORD wCmdID = 0);
	virtual ~CCmdSelects();

	virtual BOOL Snapshot(CPmwView* pView, BOOL fCanRelayer = TRUE);

protected:
	void SelectAll(void);
	virtual void RelayerObjects(BOOL fUpdateMenu = TRUE);
	virtual void DetachObjects(BOOL fUpdateMenu = TRUE, BOOL fDoIntersect = TRUE);

	virtual void DetachFrames(void);
	virtual void DetachFrames(PMGPageObject* pObject, CMapPtrToPtr& Map);
	virtual void AttachFrames(void);
	virtual void AttachFrames(PMGPageObject* pObject, CMapPtrToPtr& Map);

	virtual void ReflowFrames(CMapPtrToPtr& Map);

	virtual void DeleteObjects(BOOL fDoDelete = TRUE);

	virtual void ViewObjects(PMGPageObject* pObject = NULL);

	BOOL m_fDetached;
	CMapPtrToWord m_mapDeletedText;

	CPmwView* m_pView;
	CPmwDoc* m_pDoc;
	CPtrList m_Objects;
	CWordArray m_Positions;
};

/*
// A command for managing objects which change layering.
// This is an abstract intermediate helper class.
*/

class CCmdRelayerSelects : public CCmdSelects
{
public:
	CCmdRelayerSelects(WORD wCmdID = 0);

	// Undo this command.
	virtual BOOL UndoIt(void);
};

/*
// The command for dealing with object deletion.
*/

class CCmdDeleteSelects : public CCmdRelayerSelects
{
public:
	// Constructor.
	CCmdDeleteSelects(WORD wCmdID = 0);
	// Do this command.
	virtual BOOL DoIt(void);
	// Close any open OLE client items.
	void CloseOLEItems(void);
};

/*
// The command for dealing with object creation.
// Be sure to call Snapshot() *AFTER* the objects have been created and
// selected!
*/

class CCmdCreateSelects : public CCmdRelayerSelects
{
public:
	// Constructor.
	CCmdCreateSelects(WORD wCmdID = 0);
	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);
};

/*
// Send selects to front.
*/

class CCmdSendToFront : public CCmdRelayerSelects
{
public:
	// Constructor.
	CCmdSendToFront();
	// Do this command.
	virtual BOOL DoIt(void);
};

/*
// Send selects to back.
*/

class CCmdSendToBack : public CCmdRelayerSelects
{
public:
	// Constructor.
	CCmdSendToBack();
	// Do this command.
	virtual BOOL DoIt(void);
};

/*
// Send selects forward one layer.
*/

class CCmdForwardOne : public CCmdRelayerSelects
{
public:
	// Constructor.
	CCmdForwardOne();
	// Do this command.
	virtual BOOL DoIt(void);
};

/*
// Send selects back one layer.
*/

class CCmdBackOne : public CCmdRelayerSelects
{
public:
	// Constructor.
	CCmdBackOne();
	// Do this command.
	virtual BOOL DoIt(void);
};

/*
// Change selected objects.
*/

class CCmdChangeSelects : public CCmdSelects
{
	INHERIT(CCmdChangeSelects, CCmdSelects)
public:
	// Constructor.
	CCmdChangeSelects(WORD wCmdID = 0);

	// Before the change.
	virtual BOOL Before(CPmwView* pView, BOOL fCanRelayer = TRUE);
	// After the change. May return FALSE if no change happened.
	virtual BOOL After(void);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

protected:
	// Overridable.
	virtual BOOL SomethingChanged(void);
	BOOL SaveStates(CObjectStateArray* pStates);
	BOOL RestoreStates(CObjectStateArray* pStates, BOOL fSelected = TRUE);

	CObjectStateArray m_UndoStates;	// States saved before the action.
	CObjectStateArray m_RedoStates;	// States saved after the action.
private:
	/* So it won't be called. */
	virtual BOOL Snapshot(CPmwView* pView);
};

class CCmdChangeObject : public CCommand
{
public:
	CCmdChangeObject(WORD wCmdID = 0);
	virtual ~CCmdChangeObject();

	virtual BOOL Before(CPmwView* pView, PMGPageObject* pObject);
	virtual BOOL After(void);

// Undo this command.
	virtual BOOL UndoIt(void);
// Do this command.
	virtual BOOL DoIt(void);

protected:
	CPmwView* m_pView;
	CPmwDoc* m_pDoc;
	PMGPageObject* m_pObject;

	PageObjectState* m_pOldState;
	PageObjectState* m_pNewState;

	void ViewObject(void);
};

class CCmdFlipSelects : public CCmdSelects
{
public:
	CCmdFlipSelects(WORD wCmdID);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);
protected:
	BOOL FlipSelects(void);
};

/*
// Change the background.
*/

class CCmdNewBackground : public CCommand
{
public:
	CCmdNewBackground(WORD wCmdID = 0);
	virtual ~CCmdNewBackground();

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

	void SetNewBackground(CPmwDoc* pDoc, BackgroundObject* pBackground)
		{ m_pDoc = pDoc; m_pSwapBackground = pBackground; }

protected:
	BackgroundObject* SelectBackground(BackgroundObject* pBackground);
	BOOL SwapBackground(void);

	CPmwDoc* m_pDoc;
	BackgroundObject* m_pSwapBackground;
};

/*
// The base class for grouping and ungrouping.
*/

class CCmdGroup : public CCmdChangeSelects
{
public:
	CCmdGroup(WORD wCmdID = 0);
	virtual ~CCmdGroup();
protected:
	BOOL Ungroup(void);
	BOOL Group(void);
	GroupObject* m_pGroup;
	BOOL m_fInUse;

	BOOL m_fUngrouping;
};

class CCmdGroupSelects : public CCmdGroup
{
	INHERIT(CCmdGroupSelects, CCmdGroup)
public:
	CCmdGroupSelects();

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

	// Before the change.
	virtual BOOL Snapshot(CPmwView* pView, GroupObject* pGroup);
};

class CCmdUngroupSelects : public CCmdGroup
{
	INHERIT(CCmdUngroupSelects, CCmdChangeSelects)
public:
	CCmdUngroupSelects();

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

	// Before the change.
	virtual BOOL Snapshot(CPmwView* pView, GroupObject* pGroup);
};

/*
// An undo command for holding multiple undo commands.
*/

class CCmdMulti : public CCommand
{
public:
	CCmdMulti(WORD wCmdID = 0);
	virtual ~CCmdMulti();

	// Add a command.
	void AddCommand(CCommand* pCommand);
	// Do we have any commands?
	BOOL HasCommands(void)
		{ return m_Commands.GetSize() > 0; }

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

protected:
	CPtrArray m_Commands;
};

/*
// Massively change an object. This requires duplicating the object.
// This is mostly used for Edit Title Text undo.
*/

class CCmdBigChangeObject : public CCommand
{
	INHERIT(CCmdBigChangeObject, CCommand)
public:
	// Constructor.
	CCmdBigChangeObject(WORD wCmdID = 0);
	virtual ~CCmdBigChangeObject();

	// Before the change.
	virtual BOOL Before(PMGPageObject* pObject, CPmwView* pView);
	// After the change.
	virtual BOOL After(void);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

private:
	void ViewObject(void);
	void AssignObject(PMGPageObject* pStateObject);
	CPmwView* m_pView;
	CPmwDoc* m_pDoc;
	PMGPageObject* m_pObject;
	PMGPageObject* m_pBeforeObject;
	PMGPageObject* m_pAfterObject;
};

/*
// The containing command for page insertion and deletion.
*/

class CCmdChangePages : public CCmdMulti
{
public:
	CCmdChangePages(WORD wCmdID = 0);

	void Snapshot(CPmwView* pView, DWORD dwFirstPage, DWORD dwLastPage);

	void AfterInsert(void);
	void BeforeDelete(void);

	DWORD FirstPage(void) const
		{ return m_dwFirstPage; }
	DWORD LastPage(void) const
		{ return m_dwLastPage; }

protected:
	CPmwView* m_pView;
	CPmwDoc* m_pDoc;
	DWORD m_dwFirstPage;
	DWORD m_dwLastPage;
};

class CCmdInsertPages : public CCmdChangePages
{
public:
	CCmdInsertPages(void);

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

class CCmdDeletePages : public CCmdChangePages
{
public:
	CCmdDeletePages(void);

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

/*
// The command for dealing with page deletion.
*/

class CCmdDeletePage : public CCmdRelayerSelects
{
public:
	// Constructor.
	CCmdDeletePage(WORD wCmdID = 0);
	virtual ~CCmdDeletePage();

	// Take a snapshot of all objects on the page.
	virtual BOOL Snapshot(CPmwView* pView, DB_RECORD_NUMBER lPage, DWORD dwIndex);

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
protected:
	DB_RECORD_NUMBER m_lPage;
	DWORD m_dwIndex;
	CArray<DB_RECORD_NUMBER, DB_RECORD_NUMBER> mHyperlinksToMe;

	int
	ProcessHyperlinks(PMGDatabase	*pDatabase, BOOL Enable);
};

/*
// The command for dealing with page insertion.
*/

class CCmdInsertPage : public CCmdDeletePage
{
public:
	// Constructor.
	CCmdInsertPage(WORD wCmdID = 0);

	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);
};

//
// The command for dealing with Guideline changes
//
class CCmdGuideChange : public CCommand
{
public:
	// Constructor.
	CCmdGuideChange(WORD wCmdID = 0);

	virtual BOOL DoIt(void);
	virtual BOOL UndoIt(void);

   void  Setup(CPmwView* pView, int type, int nPage, int nPanel,
               DWORD oldpos, int oldindex, DWORD newpos, int newindex);

protected:
   void        SetPage(CPmwDoc* pDoc);
   BOOL        RestoreState(DWORD oldpos, int oldindex,
                            DWORD newpos, int newindex);
      
   CPmwView*   m_pView;
   int         m_guideType;   // GUIDE_HORIZ or GUIDE_VERT
   int         m_nPage;       // current page
   int         m_nPanel;      // current panal (if card)
   DWORD       m_dwOldPos;    // before position
   int         m_nOldIndex;   // before index into array (-1 if none)
   DWORD       m_dwNewPos;    // after position
   int         m_nNewIndex;   // after index into array (-1 if none)
};

//
// The command for dealing with Border Editor changes
//
class CCmdBorderChange : public CCommand
{
public:
	CCmdBorderChange(WORD wCmdID = 0);
	~CCmdBorderChange();

	virtual BOOL	UndoIt();
	virtual BOOL	DoIt();

	void	Setup(CPmwView* pView, BorderObject* pObject, CBEObject* pBEObject);

protected:
	void				DoSwap();

	CPmwView*		m_pView;
	BorderObject*	m_pBorderObject;
	CBEObject*		m_pBEObject;
};

#endif			// #ifndef __COMMANDS_H__
