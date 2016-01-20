/*
// $Workfile: tbledit.cpp $
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
// $Log: /PM8/App/tbledit.cpp $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 12    8/26/98 4:52p Jayn
// Fixed a bug in the dialog setup.
// 
// 11    7/25/98 1:52p Jayn
// 
// 10    7/12/98 5:36p Jayn
// Various fixes.
// 
// 9     7/04/98 2:04p Jayn
// New table undo commands.
// 
// 8     6/05/98 2:30p Fredf
// Added null pointer check.
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
// 3     5/29/98 6:39p Jayn
// Insert column, etc.
// 
// 2     5/28/98 4:11p Jayn
// More cool table stuff (cell formatting, etc.)
// 
// 1     5/26/98 1:33p Jayn
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "tblobj.h"
#include "tblprpd.h"
#include "celobj.h"
#include "tbledit.h"
#include "tblcmd.h"		// Table undo commands.
#include "celoutd.h"		// Cell formatting.

BOOL CPmwView::TableIsSelected(void) 
{
	CPmwDoc* pDoc = GetDocument();
	return pDoc->one_selection()
					&& pDoc->selected_objects()->type() == OBJECT_TYPE_Table;
}

BOOL CPmwView::TableCellsSelected(void) 
{
	return TableIsSelected()
		&& ((CTableObject*)GetDocument()->selected_objects())->SomethingSelected();
}

void CPmwView::OnAutoFormat()
{
	CPmwDoc* pDoc = GetDocument();

	PMGPageObject* pObject = pDoc->selected_objects();
	ASSERT(pObject->type() == OBJECT_TYPE_Table);

	CTablePropertiesDlg dlg ((CTableObject*)pObject);

	if (dlg.DoModal() == IDOK)
	{
		pDoc->SetModifiedFlag();
		pDoc->refresh_object(pObject);
	}
}

void CPmwView::OnUpdateAutoFormat(CCmdUI* pCmdUI)
{
	OnTableSelected(pCmdUI);
}

void CPmwView::OnCellFormatting()
{
	CPmwDoc* pDoc = GetDocument();
	// Grab the formatting from the table.
	CTableCellFormatting Formatting;
	CTableObject* pTable = (CTableObject*)pDoc->selected_objects();
	pTable->GetCellFormatting(&Formatting);

	// Create the dialog.
	CCellOutlineDlg Dialog(&Formatting);
	// Run the dialog.
	if (Dialog.DoModal() == IDOK)
	{
		// User agreed. Set the cell formatting back.
		// Create an undo command.
		CCellFormattingCommand* pCommand = new CCellFormattingCommand(IDCmd_CellFormatting);
		if (pCommand->Before(this, pTable))
		{
			// Set the new cell formatting.
			pTable->SetCellFormatting(Formatting);
			pDoc->refresh_object(pTable);
			pDoc->SetModifiedFlag();

			// Finish up the undo command.
			if (!pCommand->After())
			{
				delete pCommand;
			}
			else
			{
				pDoc->AddCommand(pCommand, FALSE);
			}
		}
	}
}

void CPmwView::OnUpdateCellFormatting(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TableCellsSelected());
}

void CPmwView::OnTableSelected(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TableIsSelected());
}

void CPmwView::OnTableInsert() 
{
	CPmwDoc* pDoc = GetDocument();
	CTableObject* pTable = (CTableObject*)pDoc->selected_objects();
	ASSERT(pTable->type() == OBJECT_TYPE_Table);

	if (pTable->SomethingSelected())
	{
		// We have a selection.
		CTableInsertDialog Dialog;
		Dialog.m_nRows = pTable->GetRows();
		Dialog.m_nColumns = pTable->GetColumns();

		if (Dialog.DoModal() == IDOK)
		{
			if (Dialog.m_nWhat == CTableInsertDialog::INSERT_Rows)
			{
				CRect crSelect = pTable->GetSelect();
				int nRow;
				int nFormatRow;

				if (Dialog.m_nWhere == CTableInsertDialog::INSERT_Before)
				{
					nRow = crSelect.top;
					nFormatRow = nRow;
				}
				else
				{
					nRow = crSelect.bottom+1;
					nFormatRow = nRow-1;
				}

				// Get the formatting info to use for adding the new column.
				CTableNewRCInfo NewRCInfo;
				pTable->GetNewRowInfo(nFormatRow, &NewRCInfo);

				pDoc->toggle_object(pTable);

				// Add the column.
				for (int i = 0; i < Dialog.m_nInsertAmount; i++)
				{
					pTable->AddRow(nRow, &NewRCInfo);
				}

				pDoc->toggle_object(pTable);
				pDoc->refresh_object(pTable);

				// Select the new cells.
				pTable->SetSelect(CRect(0, nRow, pTable->GetColumns()-1, nRow+Dialog.m_nInsertAmount-1));

				// Create the command. The "after" state is our snapshot.
				CTableInsertRowCommand* pCommand = new CTableInsertRowCommand(IDCmd_InsertRows);
				if (pCommand->Snapshot(pTable, this))
				{
					// Add the command, but do not execute it.
					pDoc->AddCommand(pCommand, FALSE);
				}
				else
				{
					delete pCommand;
				}
			}
			else
			{
				ASSERT(Dialog.m_nWhat == CTableInsertDialog::INSERT_Columns);
				CRect crSelect = pTable->GetSelect();
				int nColumn;
				int nFormatColumn;

				if (Dialog.m_nWhere == CTableInsertDialog::INSERT_Before)
				{
					nColumn = crSelect.left;
					nFormatColumn = nColumn;
				}
				else
				{
					nColumn = crSelect.right+1;
					nFormatColumn = nColumn-1;
				}

				// Get the formatting info to use for adding the new column.
				CTableNewRCInfo NewRCInfo;
				pTable->GetNewColumnInfo(nFormatColumn, &NewRCInfo);

				pDoc->toggle_object(pTable);

				// Add the column.
				for (int i = 0; i < Dialog.m_nInsertAmount; i++)
				{
					pTable->AddColumn(nColumn, &NewRCInfo);
				}

				pDoc->toggle_object(pTable);
				pDoc->refresh_object(pTable);

				// Select the new cells.
				pTable->SetSelect(CRect(nColumn, 0, nColumn+Dialog.m_nInsertAmount-1, pTable->GetRows()-1));

				// Create the command. The "after" state is our snapshot.
				CTableInsertColumnCommand* pCommand = new CTableInsertColumnCommand(IDCmd_InsertColumns);
				if (pCommand->Snapshot(pTable, this))
				{
					// Add the command, but do not execute it.
					pDoc->AddCommand(pCommand, FALSE);
				}
				else
				{
					delete pCommand;
				}
			}
		}
	}
}

void CPmwView::OnUpdateTableInsert(CCmdUI* pCmdUI) 
{
	BOOL fEnable = FALSE;
	if (TableCellsSelected())
	{
		CPmwDoc* pDoc = GetDocument();
		CTableObject* pTable = (CTableObject*)pDoc->selected_objects();
		ASSERT(pTable->type() == OBJECT_TYPE_Table);
		fEnable = (pTable->GetRows() < pTable->GetMaxRows()
						|| pTable->GetColumns() < pTable->GetMaxColumns());
	}
	pCmdUI->Enable(fEnable);
}

void CPmwView::OnTableDelete() 
{
	CPmwDoc* pDoc = GetDocument();
	CTableObject* pTable = (CTableObject*)pDoc->selected_objects();
	ASSERT(pTable->type() == OBJECT_TYPE_Table);

	if (pTable->SomethingSelected())
	{
		// We have a selection.
		CRect crSelect = pTable->GetSelect();
		int nRows = pTable->GetRows();
		int nColumns = pTable->GetColumns();

		CTableDeleteDialog Dialog;

		if (crSelect.Width()+1 == nColumns)
		{
			Dialog.m_nWhat = CTableDeleteDialog::DELETE_Rows;
		}
		else if (crSelect.Height()+1 == nRows)
		{
			Dialog.m_nWhat = CTableDeleteDialog::DELETE_Columns;
		}
		else
		{
			if (Dialog.DoModal() != IDOK)
			{
				return;
			}
		}

		// See what we need to do.
		if (Dialog.m_nWhat == CTableDeleteDialog::DELETE_Rows)
		{
			CTableDeleteRowCommand* pCommand = new CTableDeleteRowCommand(IDCmd_DeleteRows);
			if (pCommand->Snapshot(pTable, this))
			{
				// Add the command, and execute it.
				pDoc->AddCommand(pCommand);
			}
			else
			{
				delete pCommand;
			}
		}
		else
		{
			ASSERT(Dialog.m_nWhat == CTableDeleteDialog::DELETE_Columns);
			CTableDeleteColumnCommand* pCommand = new CTableDeleteColumnCommand(IDCmd_DeleteColumns);
			if (pCommand->Snapshot(pTable, this))
			{
				// Add the command, and execute it.
				pDoc->AddCommand(pCommand);
			}
			else
			{
				delete pCommand;
			}
		}
	}
}

void CPmwView::OnUpdateTableDelete(CCmdUI* pCmdUI) 
{
	BOOL fEnable = FALSE;
	int nID = IDS_DELETERC;
	if (TableCellsSelected())
	{
		CPmwDoc* pDoc = GetDocument();
		CTableObject* pTable = (CTableObject*)pDoc->selected_objects();
		ASSERT(pTable->type() == OBJECT_TYPE_Table);

		CRect crSelect = pTable->GetSelect();
		BOOL fAllRows = crSelect.Height()+1 == pTable->GetRows();
		BOOL fAllColumns = crSelect.Width()+1 == pTable->GetColumns();
		if (!(fAllRows && fAllColumns))
		{
			fEnable = TRUE;
			if (fAllRows)
			{
				nID = (crSelect.Width() == 0) ? IDS_DELETECOLUMN : IDS_DELETECOLUMNS;
			}
			else if (fAllColumns)
			{
				nID = (crSelect.Height() == 0) ? IDS_DELETEROW : IDS_DELETEROWS;
			}
		}
	}
	CString csText;
	csText.LoadString(nID);
	pCmdUI->Enable(fEnable);
	pCmdUI->SetText(csText);
}

void CPmwView::OnTableFillDown() 
{
	CPmwDoc* pDoc = GetDocument();
	CTableObject* pTable = (CTableObject*)pDoc->selected_objects();
	if (pTable != NULL && pTable->CanFillDown())
	{
		CTableFillCommand* pCommand = new CTableFillCommand(IDCmd_FillDown);
		CRect crArea = pTable->GetSelect();
		crArea.top++;		// Do not save the top row.
		if (pCommand->Snapshot(pTable, crArea))
		{
			pTable->FillDown();
			pDoc->AddCommand(pCommand, FALSE);
			pDoc->SetModifiedFlag();
		}
		else
		{
			delete pCommand;
		}
	}
	else
	{
		// Should not be here!
		ASSERT(FALSE);
	}
}

void CPmwView::OnUpdateTableFillDown(CCmdUI* pCmdUI) 
{
	CPmwDoc* pDoc = GetDocument();
	pCmdUI->Enable(TableIsSelected()
						 && ((CTableObject*)pDoc->selected_objects())->CanFillDown());
}

void CPmwView::OnTableFillRight() 
{
	CPmwDoc* pDoc = GetDocument();
	CTableObject* pTable = (CTableObject*)pDoc->selected_objects();
	if (pTable != NULL && pTable->CanFillRight())
	{
		CTableFillCommand* pCommand = new CTableFillCommand(IDCmd_FillRight);
		CRect crArea = pTable->GetSelect();
		crArea.left++;		// Do not save the left column.
		if (pCommand->Snapshot(pTable, crArea))
		{
			pTable->FillRight();
			pDoc->AddCommand(pCommand, FALSE);
			pDoc->SetModifiedFlag();
		}
		else
		{
			delete pCommand;
		}
	}
	else
	{
		// Should not be here!
		ASSERT(FALSE);
	}
}

void CPmwView::OnUpdateTableFillRight(CCmdUI* pCmdUI) 
{
	CPmwDoc* pDoc = GetDocument();
	pCmdUI->Enable(TableIsSelected()
						 && ((CTableObject*)pDoc->selected_objects())->CanFillRight());
}

void CPmwView::OnTableMergeCells() 
{
	CPmwDoc* pDoc = GetDocument();
	CTableObject* pTable = (CTableObject*)pDoc->selected_objects();
	if (pTable != NULL && pTable->CanMergeCells())
	{
		// Allocate the merge cells command.
		CMergeCellsCommand* pCommand = new CMergeCellsCommand(IDCmd_MergeCells);
		if (pCommand->Snapshot(pTable, this))
		{
			// Add and execute the command.
			pDoc->AddCommand(pCommand);
		}
		else
		{
			delete pCommand;
		}
	}
}

void CPmwView::OnUpdateTableMergeCells(CCmdUI* pCmdUI) 
{
	CPmwDoc* pDoc = GetDocument();
	pCmdUI->Enable(TableIsSelected()
						 && ((CTableObject*)pDoc->selected_objects())->CanMergeCells());
}

void CPmwView::OnTableSplitCells() 
{
	CPmwDoc* pDoc = GetDocument();
	CTableObject* pTable = (CTableObject*)pDoc->selected_objects();
	if (pTable != NULL && pTable->CanSplitCells())
	{
		// Create the split command.
		CSplitCellsCommand* pCommand = new CSplitCellsCommand(IDCmd_SplitCells);
		if (pCommand->Snapshot(pTable, this))
		{
			// Add and execute the command.
			pDoc->AddCommand(pCommand);
		}
		else
		{
			delete pCommand;
		}
	}
}

void CPmwView::OnUpdateTableSplitCells(CCmdUI* pCmdUI) 
{
	CPmwDoc* pDoc = GetDocument();
	pCmdUI->Enable(TableIsSelected()
						 && ((CTableObject*)pDoc->selected_objects())->CanSplitCells());
}

void CPmwView::OnTableFitToText() 
{
	CPmwDoc* pDoc = GetDocument();
	CTableObject* pTable = (CTableObject*)pDoc->selected_objects();
	if (pTable != NULL)
	{
		pTable->SetFitToText(!pTable->GetFitToText());
		pDoc->SetModifiedFlag();
	}
}

void CPmwView::OnUpdateTableFitToText(CCmdUI* pCmdUI) 
{
#if 1
	// For now, this is a non-feature.
	pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
#else
	pCmdUI->Enable(TableIsSelected());
	CPmwDoc* pDoc = GetDocument();
	CTableObject* pTable = (CTableObject*)pDoc->selected_objects();
	if (pTable != NULL)
	{
		pCmdUI->SetCheck(pTable->GetFitToText());
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CTableInsertDialog dialog

CTableInsertDialog::CTableInsertDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CTableInsertDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTableInsertDialog)
	m_nWhere = 0;
	m_nWhat = 0;
	m_nInsertAmount = 1;
	//}}AFX_DATA_INIT
	m_nMaxInsert = 99;
	m_nRows = 0;
	m_nColumns = 0;
}

void CTableInsertDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTableInsertDialog)
	DDX_Radio(pDX, IDC_BEFORE_CELLS, m_nWhere);
	DDX_Radio(pDX, IDC_ROWS, m_nWhat);
	DDX_Text(pDX, IDC_INSERT_AMOUNT, m_nInsertAmount);
	//}}AFX_DATA_MAP
	DDV_MinMaxInt(pDX, m_nInsertAmount, 1, m_nMaxInsert);
}

BEGIN_MESSAGE_MAP(CTableInsertDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CTableInsertDialog)
	ON_BN_CLICKED(IDC_ROWS, OnRows)
	ON_BN_CLICKED(IDC_COLUMNS, OnColumns)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableInsertDialog message handlers

BOOL CTableInsertDialog::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();

	int nMaxRows = CTableObject::GetMaxRows();
	int nMaxColumns = CTableObject::GetMaxColumns();
	ASSERT(m_nRows < nMaxRows || m_nColumns < nMaxColumns);

	// Disable those functions that are not appropriate.
	if (m_nRows >= nMaxRows)
	{
		CheckDlgButton(IDC_COLUMNS, TRUE);
		CheckDlgButton(IDC_ROWS, FALSE);
		EnableDlgItem(IDC_ROWS, FALSE);
	}
	else if (m_nColumns >= nMaxColumns)
	{
		CheckDlgButton(IDC_ROWS, TRUE);
		CheckDlgButton(IDC_COLUMNS, FALSE);
		EnableDlgItem(IDC_COLUMNS, FALSE);
	}

	SetMaxInsert();

	return TRUE;
}

void CTableInsertDialog::SetMaxInsert()
{
	if (IsDlgButtonChecked(IDC_ROWS))
	{
		// Set the max for rows.
		m_nMaxInsert = CTableObject::GetMaxRows() - m_nRows;
	}
	else
	{
		// Set the max for columns.
		m_nMaxInsert = CTableObject::GetMaxColumns() - m_nColumns;
	}

	// Fix the amount if too large.
	int nAmount = GetDlgItemInt(IDC_INSERT_AMOUNT);
	if (nAmount > m_nMaxInsert)
	{
		SetDlgItemInt(IDC_INSERT_AMOUNT, m_nMaxInsert);
	}

	// Disable the edit control if useless.
	EnableDlgItem(IDC_INSERT_AMOUNT, m_nMaxInsert > 1);
}

void CTableInsertDialog::OnRows() 
{
	SetMaxInsert();
}

void CTableInsertDialog::OnColumns() 
{
	SetMaxInsert();
}

/////////////////////////////////////////////////////////////////////////////
// CTableDeleteDialog dialog

CTableDeleteDialog::CTableDeleteDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CTableDeleteDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTableDeleteDialog)
	m_nWhat = 0;
	//}}AFX_DATA_INIT
}

void CTableDeleteDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTableDeleteDialog)
	DDX_Radio(pDX, IDC_ROWS, m_nWhat);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTableDeleteDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CTableDeleteDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableDeleteDialog message handlers

BOOL CTableDeleteDialog::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();
	
	return TRUE;
}
