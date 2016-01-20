/*
// $Workfile: TBLCMD.H $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/TBLCMD.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 6     7/19/98 6:27p Jayn
// Merge cells. Applying font attributes to cells.
// 
// 5     7/12/98 5:36p Jayn
// Various fixes.
// 
// 4     7/04/98 2:04p Jayn
// New table undo commands.
// 
// 3     6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 2     6/02/98 3:00p Jayn
// 
// 1     5/30/98 6:50p Jayn
*/

#ifndef __TBLCMD_H__
#define __TBLCMD_H__

#include "command.h"
#include "tbldef.h"
#include "tcommand.h"			// For text-related defines

class CPmwView;
class CTableObject;

class CCellFormattingCommand : public CCommand
{
public:
	CCellFormattingCommand(WORD wCmdID = 0);

	// Before the change.
	virtual BOOL Before(CPmwView* pView, CTableObject* pTable);
	// After the change. May return FALSE if no change happened.
	virtual BOOL After(void);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

protected:
	CPmwView* m_pView;
	CTableObject* m_pTable;
	CTableCellFormattingSave m_BeforeFormatting;
	CTableCellFormattingSave m_AfterFormatting;

	BOOL SaveFormatting(CTableCellFormattingSave* pFormatting);
	BOOL RestoreFormatting(CTableCellFormattingSave& Formatting);
};

class CCellDeleteCommand : public CCommand
{
public:
	CCellDeleteCommand(WORD wCmdID = 0);
	virtual ~CCellDeleteCommand();

	// Before the change.
	virtual BOOL Snapshot(CTableObject* pTable);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);
protected:
	CTableCellSave m_Save;
	CTableObject* m_pTable;
	BOOL SwapCells(void);
};

class CTableFillCommand : public CCellDeleteCommand
{
public:
	CTableFillCommand(WORD wCmdID = 0);

	// Before the change.
	virtual BOOL Snapshot(CTableObject* pTable, const CRect& crArea);
};

class CTableDeleteRowCommand : public CCellDeleteCommand
{
public:
	CTableDeleteRowCommand(WORD wCmdID = 0);

	// Before the change.
	virtual BOOL Snapshot(CTableObject* pTable, CPmwView* pView);

	// Undo this command.
	virtual BOOL UndoIt(void);

	// Do this command.
	virtual BOOL DoIt(void);
protected:
	CPmwView* m_pView;
	CDWordArray m_cdaRowHeights;

	void DeleteRows(void);
	void InsertRows(void);
};

class CTableDeleteColumnCommand : public CCellDeleteCommand
{
public:
	CTableDeleteColumnCommand(WORD wCmdID = 0);

	// Before the change.
	virtual BOOL Snapshot(CTableObject* pTable, CPmwView* pView);

	// Undo this command.
	virtual BOOL UndoIt(void);

	// Do this command.
	virtual BOOL DoIt(void);
protected:
	CPmwView* m_pView;
	CDWordArray m_cdaColumnWidths;

	void DeleteColumns(void);
	void InsertColumns(void);
};

class CTableInsertRowCommand : public CTableDeleteRowCommand
{
public:
	CTableInsertRowCommand(WORD wCmdID = 0);

	// Before the change.
	virtual BOOL Snapshot(CTableObject* pTable, CPmwView* pView);

	// Undo this command.
	virtual BOOL UndoIt(void);

	// Do this command.
	virtual BOOL DoIt(void);
};

class CTableInsertColumnCommand : public CTableDeleteColumnCommand
{
public:
	CTableInsertColumnCommand(WORD wCmdID = 0);

	// Before the change.
	virtual BOOL Snapshot(CTableObject* pTable, CPmwView* pView);

	// Undo this command.
	virtual BOOL UndoIt(void);

	// Do this command.
	virtual BOOL DoIt(void);
};

class CTextEditStateArray : public CPtrArray
{
public:
	CTextEditStateArray() {}
	virtual ~CTextEditStateArray();

	CTextEditState* GetAt(int n)
		{ return (CTextEditState*)CPtrArray::GetAt(n); }
};

//
// The command for splitting cells.
//

class CSplitCellsCommand : public CCommand
{
public:
	CSplitCellsCommand(WORD wCmdID = 0);

	// Before the change.
	virtual BOOL Snapshot(CTableObject* pTable, CPmwView* pView);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);
protected:
	CArray<CRect, CRect&> m_arMerged;
	CTableObject* m_pTable;
	CPmwView* m_pView;
};

//
// A command when text changes.
//

class CTableTextCommand : public CCommand
{
public:
	CTableTextCommand(WORD wCmdID = 0);

	// Save state before the change.
	BOOL Before(CPmwView* pView, CTableObject* pTable);
	// Save state after the change.
	BOOL After(void);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

protected:
	CRect m_crArea;
	CTableObject* m_pTable;
	CPmwView* m_pView;

	CTextEditStateArray m_BeforeStates;
	CTextEditStateArray m_AfterStates;

	void SaveAllText(CTextEditStateArray& StateArray, BOOL SaveText);
	void RestoreAllText(CTextEditStateArray& BeforeStateArray, CTextEditStateArray& AfterStateArray);
};

//
// The command for merging cells.
//

class CMergeCellsCommand : public CTableTextCommand
{
public:
	CMergeCellsCommand(WORD wCmdID = 0);

	// Before the change.
	virtual BOOL Snapshot(CTableObject* pTable, CPmwView* pView);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);
protected:
	CTableCellFormattingSave m_BeforeCells;
};

#endif
