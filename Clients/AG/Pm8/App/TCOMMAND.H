/*
// Text command support.
//
// $Workfile: TCOMMAND.H $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/TCOMMAND.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:10   Fred
// Initial revision.
// 
//    Rev 1.13   30 Jun 1997 11:55:20   Jay
// New stretching logic.
// 
//    Rev 1.12   27 May 1997 09:42:22   Jay
// Stretch undo with linked frames.
// 
//    Rev 1.11   16 Apr 1997 12:12:34   Jay
// Fixes for text undo
// 
//    Rev 1.10   07 Oct 1996 16:59:26   Jay
// Fixed a redo bug. 
// 
//    Rev 1.9   25 Jul 1996 13:14:28   Jay
// Multi-page frame glitch correction.
// 
//    Rev 1.8   15 Jul 1996 11:35:06   Jay
//  
// 
//    Rev 1.7   11 Jul 1996 11:35:48   Jay
//  
// 
//    Rev 1.6   05 Jul 1996 10:47:44   Jay
//  
// 
//    Rev 1.5   02 Jul 1996 08:55:32   Jay
//  
// 
//    Rev 1.4   21 Jun 1996 14:31:14   Jay
// Stretch text, etc.
// 
//    Rev 1.3   14 Jun 1996 17:06:48   Jay
//  
// 
//    Rev 1.2   05 Jun 1996 08:22:20   Jay
//  
// 
//    Rev 1.1   23 Apr 1996 08:09:12   Jay
// Renamed CTextState
// 
//    Rev 1.0   14 Mar 1996 13:31:06   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:38   FRED
// Initial revision.
// 
//    Rev 1.1   09 Mar 1995 08:39:04   JAY
// More undo.
// 
//    Rev 1.0   17 Feb 1995 16:46:00   JAY
// Initial revision.
// 
//    Rev 1.0   15 Feb 1995 17:17:14   JAY
// Initial revision.
*/

#ifndef __TCOMMAND_H__
#define __TCOMMAND_H__

class CTxp;
class CPmwView;

class CTextEditState
{
public:
	CTextEditState();
	~CTextEditState();

	DB_RECORD_NUMBER m_Record;
	CHARACTER_INDEX m_TextStart;
	CHARACTER_INDEX m_TextEnd;
};

class CBaseTextCommand : public CCommand
{
public:
	CBaseTextCommand(WORD wCmdID = 0);

	virtual BOOL IsTextEditCommand(void);

	// In case you can't call Before().
	void SetStuff(CPmwView* pView);

	CPmwView* GetEditView(void)
		{ return m_pView; }
	CFrameObject* GetEditFrame(void)
		{ return m_pObject; }
	CHARACTER_INDEX GetEditIndex(void)
		{ return m_lIndex; }

protected:
	CPmwView* m_pView;
	CFrameObject* m_pObject;
	CHARACTER_INDEX m_lIndex;
};

/*
// A text command.
// This command allows text (with all attributes) to be saved and restored.
*/

class CTextCommand : public CBaseTextCommand
{
public:
	CTextCommand(WORD wCmdID = 0);

	// Before the change.
	virtual BOOL Before(CPmwView* pView);
	// After the change. May return FALSE if no change happened.
	virtual BOOL After(void);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

protected:
	BOOL RemoveText(CTxp* pTxp, CTextEditState& State);
	BOOL SaveText(CTxp* pTxp, CTextEditState& State);
	BOOL RestoreText(CTxp* pTxp, CTextEditState& State, BOOL fTyping = FALSE);

	CTextRecord* CreateTextRecord(PMGDatabase* pDatabase, CTxp* pTxp);
	CTextRecord* LockTextRecord(PMGDatabase* pDatabase, DB_RECORD_NUMBER lText);

	CTextEditState m_BeforeState;
	CTextEditState m_AfterState;
};

class CCmdDeleteText : public CTextCommand
{
public:
	CCmdDeleteText(WORD wCmdID = 0);

	BOOL UpdateDelete(CPmwView* pView, CHARACTER_COUNT lCount);
};

class CCmdAddText : public CTextCommand
{
public:
	CCmdAddText(void);
	BOOL UpdateTyping(CPmwView* pView, CHARACTER c, CHARACTER_COUNT lCount);
};

class CCmdBackspace : public CTextCommand
{
public:
	CCmdBackspace(void);
	BOOL UpdateBackspace(CPmwView* pView, CHARACTER_COUNT lCount);
};

class CStretchState
{
public:
	BOOL m_fStretch;
	PPNT m_StretchDims;
	CFixed m_lStretchNumerator;
	CFixed m_lStretchDenominator;
	CFixed m_lStretchExpansion;
};

class CCmdStretchText : public CBaseTextCommand
{
public:
	CCmdStretchText();
	virtual ~CCmdStretchText();

	// Take a snapshot.
	virtual BOOL Snapshot(CPmwView* pView); 

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

protected:
	BOOL SetStretch(CPtrArray& State);

	CPtrArray m_Objects;
	CPtrArray m_BeforeStates;
	CPtrArray m_AfterStates;
};

#if 0
class CCmdEditText : public CCommand
{
public:
	CCmdEditText(WORD wCmdID = 0);

	virtual BOOL Snapshot(CPmwView* pView, CFrameObject* pFrame, PPNT_PTR p);

protected:
	BOOL StartEditingText(void);
	BOOL StopEditingText(void);

	CFrameObject*	m_pFrame;
	CPmwView*		m_pView;
	BOOL				m_fPValid;
	PPNT				m_p;
};

class CCmdStartEditText : public CCmdEditText
{
public:
	CCmdStartEditText();

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);
};

class CCmdStopEditText : public CCmdEditText
{
public:
	CCmdStopEditText();

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);
};
#endif

class CCmdFrameAttributes : public CBaseTextCommand
{
public:
	CCmdFrameAttributes(WORD wCmdID = IDCmd_FrameAttributes);
	virtual ~CCmdFrameAttributes();

	// Before the change.
	virtual BOOL Before(CPmwView* pView);
	// After the change. May return FALSE if no change happened.
	virtual BOOL After(const CFrameAttributes& AfterAttributes);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);
protected:
	BOOL GetFrameRange(DB_RECORD_NUMBER& lStartFrame, DB_RECORD_NUMBER& lEndFrame);
	virtual void FreeFrames(void);
	BOOL RestoreFrames(BOOL fBefore);
	BOOL SaveFrames(void);
	virtual void SaveFrame(CFrameRecord* pFrame);
	virtual void RestoreFrame(CFrameRecord* pFrame, int nIndex);

	DB_RECORD_NUMBER m_lStartFrame;
	DB_RECORD_NUMBER m_lEndFrame;

	CPtrArray m_BeforeAttributes;
	CFrameAttributes m_AfterAttributes;
};

class CCmdVAlignText : public CCmdFrameAttributes
{
public:
	CCmdVAlignText(VERT_ALIGN_TYPE Type);
	virtual ~CCmdVAlignText();

	// Take a snapshot.
	BOOL Snapshot(CPmwView* pView)
		{ return Before(pView); }

protected:

	virtual void FreeFrames(void);
	virtual void SaveFrame(CFrameRecord* pFrame);
	virtual void RestoreFrame(CFrameRecord* pFrame, int nIndex);
};

class CCmdParagraphFormatting : public CBaseTextCommand
{
public:
	CCmdParagraphFormatting(WORD wCmdID = IDCmd_ParagraphFormatting);
	virtual ~CCmdParagraphFormatting();

	// Before the change.
	virtual BOOL Before(CPmwView* pView);
	// After the change. May return FALSE if no change happened.
	virtual BOOL After(const CParagraphFormatting& AfterFormatting);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);
protected:
	virtual void FreeParagraphs(void);
	BOOL RestoreParagraphs(BOOL fBefore);
	BOOL SaveParagraphs(void);
	virtual void SaveParagraph(CTextRecord* pText, PARAGRAPH_INDEX lParagraph);
	virtual void RestoreParagraph(CTextRecord* pText, PARAGRAPH_INDEX lParagraph, int nIndex);

	PARAGRAPH_INDEX m_lStart;
	PARAGRAPH_INDEX m_lEnd;
	CHARACTER_INDEX m_lTextStart;
	CHARACTER_INDEX m_lTextEnd;

	CPtrArray m_BeforeFormatting;
	CParagraphFormatting m_AfterFormatting;
};

class CCmdHAlignText : public CCmdParagraphFormatting
{
public:
	CCmdHAlignText(ALIGN_TYPE Type);
	virtual ~CCmdHAlignText();

	// Take a snapshot.
	BOOL Snapshot(CPmwView* pView)
		{ return Before(pView); }

protected:

	virtual void FreeParagraphs(void);
	virtual void SaveParagraph(CTextRecord* pText, PARAGRAPH_INDEX lParagraph);
	virtual void RestoreParagraph(CTextRecord* pText, PARAGRAPH_INDEX lParagraph, int nIndex);
};

class CCmdTabs : public CCmdParagraphFormatting
{
public:
	CCmdTabs();
	virtual ~CCmdTabs();

	BOOL After(const CTabArray* pTabs);

protected:

	CTabArray m_AfterTabs;

	virtual void FreeParagraphs(void);
	virtual void SaveParagraph(CTextRecord* pText, PARAGRAPH_INDEX lParagraph);
	virtual void RestoreParagraph(CTextRecord* pText, PARAGRAPH_INDEX lParagraph, int nIndex);
};

/////////////////////////////////////////////////////////////////////////////
// CCmdLinkFrames - undo/redo a frame link change

class CCmdLinkFrames : public CCommand
{
public:
	CCmdLinkFrames();
	virtual ~CCmdLinkFrames();

	void Snapshot(CPmwDoc* pDoc,
						DB_RECORD_NUMBER lLinkSource,
						DB_RECORD_NUMBER lLinkSourceNext,
						DB_RECORD_NUMBER lLinkTarget,
						DB_RECORD_NUMBER lLinkTargetPrevious,
						const CStretchState& SourceStretch,
						const CStretchState& TargetStretch);
	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

protected:
	CPmwDoc* m_pDoc;

	DB_RECORD_NUMBER m_lLinkSource;
	DB_RECORD_NUMBER m_lLinkSourceNext;
	DB_RECORD_NUMBER m_lLinkTarget;
	DB_RECORD_NUMBER m_lLinkTargetPrevious;

	CStretchState m_SourceStretch;
	CStretchState m_TargetStretch;

	BOOL SetStretch(DB_RECORD_NUMBER lFrame, CStretchState& State);
};

#endif					// #ifndef __TCOMMAND_H__
