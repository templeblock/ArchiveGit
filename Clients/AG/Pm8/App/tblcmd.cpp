/*
// $Workfile: tblcmd.cpp $
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
// $Log: /PM8/App/tblcmd.cpp $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 10    7/19/98 6:27p Jayn
// Merge cells. Applying font attributes to cells.
// 
// 9     7/12/98 5:36p Jayn
// Various fixes.
// 
// 8     7/04/98 2:04p Jayn
// New table undo commands.
// 
// 7     6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 6     6/02/98 3:00p Jayn
// 
// 5     5/31/98 2:35p Jayn
// Delete row/columns
// 
// 4     5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 3     5/28/98 4:11p Jayn
// More cool table stuff (cell formatting, etc.)
// 
// 2     5/26/98 2:39p Jayn
// New table code.
// 
// 1     5/26/98 1:58p Jayn
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "tblobj.h"
#include "tblcmd.h"
#include "ctxp.h"
#include "clip.h"

/////////////////////////////////////////////////////////////////////////////
// CTextEditStateArray

CTextEditStateArray::~CTextEditStateArray()
{
	for (int n = 0; n < GetSize(); n++)
	{
		delete GetAt(n);
	}
}

//
// Undo/redo commands for table objects.
//

CCellFormattingCommand::CCellFormattingCommand(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
	m_pTable = NULL;
	m_pView = NULL;
}

// Before the change.
BOOL CCellFormattingCommand::Before(CPmwView* pView, CTableObject* pTable)
{
	m_pView = pView;
	m_pTable = pTable;
	return SaveFormatting(&m_BeforeFormatting);
}

// After the change. May return FALSE if no change happened.
BOOL CCellFormattingCommand::After(void)
{
	return SaveFormatting(&m_AfterFormatting);
}

// Undo this command.
BOOL CCellFormattingCommand::UndoIt(void)
{
	return RestoreFormatting(m_BeforeFormatting);
}

// Do this command.
BOOL CCellFormattingCommand::DoIt(void)
{
	return RestoreFormatting(m_AfterFormatting);
}

BOOL CCellFormattingCommand::SaveFormatting(CTableCellFormattingSave* pFormatting)
{
	if (m_pTable == NULL || !m_pTable->SomethingSelected())
	{
		return FALSE;
	}

	// Save the formatting now.
	return m_pTable->SaveCellFormatting(pFormatting);
}

BOOL CCellFormattingCommand::RestoreFormatting(CTableCellFormattingSave& Formatting)
{
	CPmwDoc* pDoc = m_pView->GetDocument();

	// Bring back the page with the table on it.
	m_pView->ViewObject(m_pTable);

	// Make the object be selected.
	pDoc->deselect_all();
	pDoc->select_object(m_pTable);

	// Restore the selection.
	m_pTable->SetSelect(Formatting.m_crArea);

	// Invalidate the selection before,in case it gets smaller.
	m_pTable->InvalidateSelection();

	// Restore the formatting.
	m_pTable->RestoreCellFormatting(Formatting);

	// Invalidate the selection after, in case it gets bigger.
	m_pTable->InvalidateSelection();

	// Mark the document as modified.
	pDoc->SetModifiedFlag();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCellDeleteCommand
CCellDeleteCommand::CCellDeleteCommand(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
}

// Destructor.
CCellDeleteCommand::~CCellDeleteCommand()
{
	// Destroy all objects still attached to us.
	CPtrArray& Array = m_Save.m_cpaCellContents;
	for (int nObject = 0; nObject < Array.GetSize(); nObject++)
	{
		PMGPageObject* pObject = (PMGPageObject*)Array.GetAt(nObject);
		// The object can be on-page if it was the same before and after.
		// In that case, we do not want to delete it!
		if (pObject != NULL && !pObject->IsOnPage())
		{
			// Free all other database references.
			pObject->destroy();
			// Delete the memory representation.
			delete pObject;
		}
	}
}

// Before the change.
BOOL CCellDeleteCommand::Snapshot(CTableObject* pTable)
{
	m_pTable = pTable;
	// We want to save the cell formatting and allocate empty "after" objects.
	if (pTable->SaveCellFormatting(&m_Save))
	{
		// Add NULL pointers for all objects. This is our initial "after" state.
		int nCells = (m_Save.m_crArea.Width()+1)*(m_Save.m_crArea.Height()+1);
		for (int nCell = 0; nCell < nCells; nCell++)
		{
			m_Save.m_cpaCellContents.Add(NULL);
		}
		return TRUE;
	}
	return FALSE;
}

// Undo this command.
BOOL CCellDeleteCommand::UndoIt(void)
{
	return SwapCells();
}

// Do this command.
BOOL CCellDeleteCommand::DoIt(void)
{
	return SwapCells();
}

// Swap the cells with the ones in the table.
BOOL CCellDeleteCommand::SwapCells(void)
{
	// Exchange the cells with what is in the table.
	m_pTable->SwapCells(m_Save);

	// Make them redraw.
	m_pTable->InvalidateCells(m_Save.m_crArea);

	// The document is modified.
	m_pTable->get_database()->document_modified();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTableFillCommand
CTableFillCommand::CTableFillCommand(WORD wCmdID /*=0*/)
	: CCellDeleteCommand(wCmdID)
{
}

// Before the change.
BOOL CTableFillCommand::Snapshot(CTableObject* pTable, const CRect& crArea)
{
	m_pTable = pTable;
	// We want to save the cell formatting and allocate empty "after" objects.
	if (pTable->SaveCellFormatting(&m_Save, crArea))
	{
		// Get all the objects that are currently in the table.
		// This will finish our save (we do not "do" the command when added).
		CPoint cpCell;
		for (cpCell.y = m_Save.m_crArea.top; cpCell.y <= m_Save.m_crArea.bottom; cpCell.y++)
		{
			for (cpCell.x = m_Save.m_crArea.left; cpCell.x <= m_Save.m_crArea.right; cpCell.x++)
			{
				m_Save.m_cpaCellContents.Add(m_pTable->GetCellFrame(cpCell));
			}
		}
		return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CTableDeleteRowCommand

CTableDeleteRowCommand::CTableDeleteRowCommand(WORD wCmdID /*=0*/)
	: CCellDeleteCommand(wCmdID)
{
}

// Before the change.
BOOL CTableDeleteRowCommand::Snapshot(CTableObject* pTable, CPmwView* pView)
{
	m_pView = pView;

	if (!CCellDeleteCommand::Snapshot(pTable))
	{
		return FALSE;
	}

	// Save all the row heights.
	for (int nRow = m_Save.m_crArea.top; nRow <= m_Save.m_crArea.bottom; nRow++)
	{
		m_cdaRowHeights.Add(m_pTable->GetRowHeight(nRow));
	}
	return TRUE;
}

void CTableDeleteRowCommand::InsertRows(void)
{
	CPmwDoc* pDoc = m_pView->GetDocument();

	// Add a refresh now since it will be getting larger.
	pDoc->refresh_object(m_pTable);

	// Toggle the object off.
	pDoc->toggle_object(m_pTable);

	// Deselect what we are about to delete.
	m_pTable->DeselectAll();

	int nColumns = m_pTable->GetColumns();
	// Recreate the rows.
	int nRow = m_Save.m_crArea.top;
	int nRows = m_Save.m_crArea.Height()+1;
	// Create dummy formatting info.
	// If will all be restored later anyway.
	CTableNewRCInfo NewRCInfo;
	NewRCInfo.AllocateInfo(nColumns);
	int nColumn;
	for (nColumn = 0; nColumn < nColumns; nColumn++)
	{
		NewRCInfo.m_pFillColors[nColumn] = TRANSPARENT_COLOR;
		NewRCInfo.m_pSegments[nColumn].SetDefault();
	}
	NewRCInfo.m_pSegments[nColumn].SetDefault();	// Last one, too.

	// Do the insertion.
	for (int i = 0; i < nRows; i++)
	{
		// Restore with the same width.
		NewRCInfo.m_Size = m_cdaRowHeights.GetAt(i);
		// Insert the row.
		m_pTable->AddRow(nRow, &NewRCInfo);
	}

	// Swap in the real cells.
	SwapCells();

	// Select what we just restored.
	m_pTable->SetSelect(m_Save.m_crArea);

	// Toggle the object back on.
	pDoc->toggle_object(m_pTable);

	// And make it redraw.
	pDoc->refresh_object(m_pTable);
}

void CTableDeleteRowCommand::DeleteRows(void)
{
	CPmwDoc* pDoc = m_pView->GetDocument();

	// Add a refresh now since it will be getting smaller.
	pDoc->refresh_object(m_pTable);

	// Toggle the object off.
	pDoc->toggle_object(m_pTable);

	// Deselect what we are about to delete.
	m_pTable->DeselectAll();

	// Swap in the NULL cells.
	SwapCells();

	// Delete the rows.
	int nRow = m_Save.m_crArea.top;
	int nRows = m_Save.m_crArea.Height()+1;
	for (int i = 0; i < nRows; i++)
	{
		m_pTable->DeleteRow(nRow);
	}

	// Toggle the object back on.
	pDoc->toggle_object(m_pTable);

	// And make it redraw.
	pDoc->refresh_object(m_pTable);
}

// Do this command.
BOOL CTableDeleteRowCommand::DoIt(void)
{
	DeleteRows();
	return TRUE;
}

// Undo this command.
BOOL CTableDeleteRowCommand::UndoIt(void)
{
	InsertRows();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTableDeleteColumnCommand

CTableDeleteColumnCommand::CTableDeleteColumnCommand(WORD wCmdID /*=0*/)
	: CCellDeleteCommand(wCmdID)
{
}

// Before the change.
BOOL CTableDeleteColumnCommand::Snapshot(CTableObject* pTable, CPmwView* pView)
{
	m_pView = pView;

	if (!CCellDeleteCommand::Snapshot(pTable))
	{
		return FALSE;
	}

	// Save all the column widths.
	for (int nColumn = m_Save.m_crArea.left; nColumn <= m_Save.m_crArea.right; nColumn++)
	{
		m_cdaColumnWidths.Add(m_pTable->GetColumnWidth(nColumn));
	}
	return TRUE;
}

void CTableDeleteColumnCommand::InsertColumns(void)
{
	CPmwDoc* pDoc = m_pView->GetDocument();

	// Add a refresh now since it will be getting larger.
	pDoc->refresh_object(m_pTable);

	// Toggle the object off.
	pDoc->toggle_object(m_pTable);

	// Deselect what we are about to delete.
	m_pTable->DeselectAll();

	int nRows = m_pTable->GetRows();
	// Recreate the columns.
	int nColumn = m_Save.m_crArea.left;
	int nColumns = m_Save.m_crArea.Width()+1;
	// Create dummy formatting info.
	// If will all be restored later anyway.
	CTableNewRCInfo NewRCInfo;
	NewRCInfo.AllocateInfo(nRows);
	int nRow;
	for (nRow = 0; nRow < nRows; nRow++)
	{
		NewRCInfo.m_pFillColors[nRow] = TRANSPARENT_COLOR;
		NewRCInfo.m_pSegments[nRow].SetDefault();
	}
	NewRCInfo.m_pSegments[nRow].SetDefault();	// Last one, too.

	// Do the insertion.
	for (int i = 0; i < nColumns; i++)
	{
		// Restore with the same width.
		NewRCInfo.m_Size = m_cdaColumnWidths.GetAt(i);
		// Insert the column.
		m_pTable->AddColumn(nColumn, &NewRCInfo);
	}

	// Swap in the real cells.
	SwapCells();

	// Select what we just restored.
	m_pTable->SetSelect(m_Save.m_crArea);

	// Toggle the object back on.
	pDoc->toggle_object(m_pTable);

	// And make it redraw.
	pDoc->refresh_object(m_pTable);
}

void CTableDeleteColumnCommand::DeleteColumns(void)
{
	CPmwDoc* pDoc = m_pView->GetDocument();

	// Add a refresh now since it will be getting smaller.
	pDoc->refresh_object(m_pTable);

	// Toggle the object off.
	pDoc->toggle_object(m_pTable);

	// Deselect what we are about to delete.
	m_pTable->DeselectAll();

	// Swap in the NULL cells.
	SwapCells();

	// Delete the columns.
	int nColumn = m_Save.m_crArea.left;
	int nColumns = m_Save.m_crArea.Width()+1;
	for (int i = 0; i < nColumns; i++)
	{
		m_pTable->DeleteColumn(nColumn);
	}

	// Toggle the object back on.
	pDoc->toggle_object(m_pTable);

	// And make it redraw.
	pDoc->refresh_object(m_pTable);
}

// Undo this command.
BOOL CTableDeleteColumnCommand::UndoIt(void)
{
	InsertColumns();
	return TRUE;
}

// Do this command.
BOOL CTableDeleteColumnCommand::DoIt(void)
{
	DeleteColumns();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTableInsertRowCommand

CTableInsertRowCommand::CTableInsertRowCommand(WORD wCmdID /*=0*/)
	: CTableDeleteRowCommand(wCmdID)
{
}

// Before the change.
BOOL CTableInsertRowCommand::Snapshot(CTableObject* pTable, CPmwView* pView)
{
	return CTableDeleteRowCommand::Snapshot(pTable, pView);
}

// Undo this command.
BOOL CTableInsertRowCommand::UndoIt(void)
{
	DeleteRows();
	return TRUE;
}

// Do this command.
BOOL CTableInsertRowCommand::DoIt(void)
{
	InsertRows();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTableInsertColumnCommand

CTableInsertColumnCommand::CTableInsertColumnCommand(WORD wCmdID /*=0*/)
	: CTableDeleteColumnCommand(wCmdID)
{
}

// Before the change.
BOOL CTableInsertColumnCommand::Snapshot(CTableObject* pTable, CPmwView* pView)
{
	return CTableDeleteColumnCommand::Snapshot(pTable, pView);
}

// Undo this command.
BOOL CTableInsertColumnCommand::UndoIt(void)
{
	DeleteColumns();
	return TRUE;
}

// Do this command.
BOOL CTableInsertColumnCommand::DoIt(void)
{
	InsertColumns();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSplitCellsCommand - The command for splitting cells.

CSplitCellsCommand::CSplitCellsCommand(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
}

// Before the change.
BOOL CSplitCellsCommand::Snapshot(CTableObject* pTable, CPmwView* pView)
{
	m_pTable = pTable;
	m_pView = pView;

	if (m_pTable->SomethingSelected())
	{
		m_pTable->GetMergedCells(m_pTable->GetSelect(), m_arMerged);
		if (m_arMerged.GetSize() > 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

// Undo this command.
BOOL CSplitCellsCommand::UndoIt(void)
{
	int nSize = m_arMerged.GetSize();
	for (int n = 0; n < nSize; n++)
	{
		CRect crMerged = m_arMerged.GetAt(n);
		m_pTable->MergeCells(crMerged, FALSE);
	}
	m_pView->GetDocument()->SetModifiedFlag();
	return TRUE;
}

// Do this command.
BOOL CSplitCellsCommand::DoIt(void)
{
	m_pTable->SplitCells(m_arMerged);
	m_pView->GetDocument()->SetModifiedFlag();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTableTextCommand - The command for changing table text.

CTableTextCommand::CTableTextCommand(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
}

BOOL CTableTextCommand::Before(CPmwView* pView, CTableObject* pTable)
{
	if (!pTable->SomethingSelected())
	{
		return FALSE;
	}

	m_pView = pView;
	m_pTable = pTable;
	m_crArea = pTable->GetSelect();

	// Save the text.
	SaveAllText(m_BeforeStates, TRUE);
	return TRUE;
}

BOOL CTableTextCommand::After(void)
{
	SaveAllText(m_AfterStates, TRUE);
	return TRUE;
}

// Undo this command.
BOOL CTableTextCommand::UndoIt(void)
{
	m_pTable->SetSelect(m_crArea);
	RestoreAllText(m_BeforeStates, m_AfterStates);
	m_pTable->InvalidateSelection();
	m_pTable->ComputeSelectTextStyle();
	return TRUE;
}

// Do this command.
BOOL CTableTextCommand::DoIt(void)
{
	m_pTable->SetSelect(m_crArea);
	RestoreAllText(m_AfterStates, m_BeforeStates);
	m_pTable->InvalidateSelection();
	m_pTable->ComputeSelectTextStyle();
	return TRUE;
}

//
// Save all the text in the cells
//

void CTableTextCommand::SaveAllText(CTextEditStateArray& StateArray, BOOL fSaveText)
{
	CPoint cpCell;
	// Loop on rows.
	for (cpCell.y = m_crArea.top; cpCell.y <= m_crArea.bottom; cpCell.y++)
	{
		// Loop on columns.
		for (cpCell.x = m_crArea.left; cpCell.x <= m_crArea.right; cpCell.x++)
		{
			// Save the next frame.
			CFrameObject* pFrame = m_pTable->GetCellFrame(cpCell);
			CTextEditState* pState = NULL;

			if (pFrame == NULL)
			{
				// No frame. This should not happen.
				ASSERT(FALSE);
			}
			else
			{
				// We want to create a new state and save the text in the frame.
				PMGDatabase* pDatabase = GetUndoDatabase();
				if (pDatabase == NULL)
				{
					// No undo database!
					ASSERT(FALSE);
					return;
				}

				// Create the state.
				pState = new CTextEditState;

				// Initialize a Txp so we can get to the text.
				CTxp Txp(NULL);
				Txp.Init(pFrame, 0);

				// Select all the text.
				Txp.SelectAll();
				if (Txp.SomethingSelected())
				{
					// We have a selection.
					pState->m_TextStart = Txp.SelectStart();
					pState->m_TextEnd = Txp.SelectEnd();

					// Copy the text to the database.
					CopyTextToDatabase(&Txp, pDatabase, &pState->m_Record, UNDO_Normal);
				}
				else
				{
					// Must be no text.
					pState->m_TextStart = pState->m_TextEnd = 0;
				}
				StateArray.Add(pState);		// May be NULL in error case.
			}
		}
	}
}

//
// Restore all the text in the cells.
//

void CTableTextCommand::RestoreAllText(CTextEditStateArray& BeforeStateArray, CTextEditStateArray& AfterStateArray)
{
	CPoint cpCell;
	int nState = 0;
	// Loop on rows.
	for (cpCell.y = m_crArea.top; cpCell.y <= m_crArea.bottom; cpCell.y++)
	{
		// Loop on columns.
		for (cpCell.x = m_crArea.left; cpCell.x <= m_crArea.right; cpCell.x++, nState++)
		{
			// Get the frame to operate on.
			CFrameObject* pFrame = m_pTable->GetCellFrame(cpCell);
			if (pFrame == NULL)
			{
				// No frame. This should not happen.
				ASSERT(FALSE);
				continue;
			}

			// Grab the state.
			CTextEditState* pBeforeState = BeforeStateArray.GetAt(nState);

			if (pBeforeState == NULL)
			{
				// Nothing to do.
			}
			else
			{
				// We need to restore the text.
				// Initialize a Txp so we can get to the text.
				CTxp Txp(NULL);
				Txp.Init(pFrame, 0);

				// First, delete the text currently there.
				CTextEditState* pAfterState = AfterStateArray.GetAt(nState);
				if (pAfterState == NULL)
				{
					// Before state but no after state.
					// This is a problem.
					ASSERT(FALSE);
				}
				else
				{
					// We have a frame and states. Do the restore.
					PMGDatabase* pDatabase = GetUndoDatabase();
					if (pDatabase == NULL)
					{
						// No undo database!
						ASSERT(FALSE);
						return;
					}

					// Remove the text in the after state.
					Txp.SelectText(pAfterState->m_TextStart, pAfterState->m_TextStart);
					Txp.DeleteText(pAfterState->m_TextEnd - pAfterState->m_TextStart);

					// Insert the before state text.
					Txp.SelectText(pBeforeState->m_TextStart, pBeforeState->m_TextStart);
					if (pBeforeState->m_Record != 0)
					{
						PasteTextFromText(&Txp,
												pDatabase,
												pBeforeState->m_Record, 
												UNDO_Normal);
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMergeCellsCommand - The command for merging cells.

CMergeCellsCommand::CMergeCellsCommand(WORD wCmdID /*=0*/)
	: CTableTextCommand(wCmdID)
{
}

// Before the change.
BOOL CMergeCellsCommand::Snapshot(CTableObject* pTable, CPmwView* pView)
{
	m_pTable = pTable;
	m_pView = pView;

	if (m_pTable->SomethingSelected())
	{
		m_crArea = m_pTable->GetSelect();
		// We need to set up for the undo/redo.
		SaveAllText(m_BeforeStates, TRUE);
		ASSERT(m_BeforeStates.GetSize() != 0);
		// Save the cell info (this includes the merged state)
		m_pTable->SaveCellFormatting(&m_BeforeCells, &m_crArea);
		return TRUE;
	}

	return FALSE;
}

// Undo this command.
BOOL CMergeCellsCommand::UndoIt(void)
{
	m_pTable->SetSelect(m_crArea);
	// Restore the cell info (including the merged states)
	m_pTable->RestoreCellFormatting(m_BeforeCells, TRUE);
	// Restore all the text.
	RestoreAllText(m_BeforeStates, m_AfterStates);
	// Make it redraw.
	m_pTable->InvalidateSelection();
	// The document is modified.
	m_pView->GetDocument()->SetModifiedFlag();
	return TRUE;
}

// Do this command.
BOOL CMergeCellsCommand::DoIt(void)
{
	// Set the select.
	m_pTable->SetSelect(m_crArea);
	// Merge the cells.
	m_pTable->MergeCells(m_crArea);
	// We are modified.
	m_pView->GetDocument()->SetModifiedFlag();

	if (m_AfterStates.GetSize() == 0)
	{
		SaveAllText(m_AfterStates, FALSE);
		ASSERT(m_AfterStates.GetSize() > 0);
	}

	return TRUE;
}

