// TblPrpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "TblPrpD.h"
#include "tblobj.h"
#include "tblctl.h"
#include "tblfmt.h"

#include "frameobj.h"
#include "framerec.h"
#include "ctxp.h"
#include "sortarry.h"			// CMlsStringArray
#include "typeface.h"
#include "pmgfont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTablePropertiesDlg dialog

CTablePropertiesDlg::CTablePropertiesDlg(CTableObject* pTable, CWnd* pParent /*=NULL*/, UINT uID /*=IDD*/)
	: CDialog(uID, pParent)
{
	m_pTable = pTable;
	//{{AFX_DATA_INIT(CTablePropertiesDlg)
	//}}AFX_DATA_INIT
	m_uLastCellData = 0;
}

void CTablePropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTablePropertiesDlg)
	DDX_Control(pDX, IDC_FORMATS, m_FormatList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTablePropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CTablePropertiesDlg)
	ON_LBN_SELCHANGE(IDC_FORMATS, OnSelchangeFormats)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTablePropertiesDlg message handlers

BOOL CTablePropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_pTableControl = (CTableFormatControl*)GetDlgItem(IDC_TABLE_PREVIEW);
	if (m_pTableControl != NULL)
	{
		m_pTableControl->Init();
		SetTableDefaults(m_pTableControl->GetTable());
	}

	// Build the format list.
	BuildFormatList();

	// Point to the first in the list.
	m_FormatList.SetCurSel(0);
	// Make the preview reflect it.
	UpdateFormatPreview();

	return FALSE;
}

void CTablePropertiesDlg::OnOK() 
{
	if (!UpdateData(TRUE))
	{
		return;
	}

	// Get the currently selected format.
	if (m_pTable != NULL)
	{
		int nCurSel = m_FormatList.GetCurSel();
		if (nCurSel >= 0)
		{
			CTableFormatter* pFormatter = (CTableFormatter*)m_FormatList.GetItemData(nCurSel);
			if (pFormatter != NULL)
			{
				// Reformat the table.
				pFormatter->FormatTable(m_pTable);
			}
		}
	}

	CDialog::OnOK();
}

//
// Set the defaults for the preview table.
//

void CTablePropertiesDlg::SetTableDefaults(CTableObject* pTable)
{
	PMGDatabase* pDatabase = pTable->get_database();

	CTextStyle Style(pDatabase);
	Style.SetDefault();

	PMGFontServer* pFontServer = (PMGFontServer*)pDatabase->get_font_server();
	int nFace = typeface_server.find_face("Arial", FALSE);
	if (nFace != -1)
	{
		DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nFace);
		if (lFace != 0)
		{
			Style.Font(lFace);
			Style.UpdateFontMetrics();
			pDatabase->free_font_record(lFace, TRUE);
		}
	}

	CPoint cpCell;
	int nSize = -1;

	// Run through all the cells and set the style.
	for (cpCell.y = 0; cpCell.y < pTable->GetRows(); cpCell.y++)
	{
		for (cpCell.x = 0; cpCell.x < pTable->GetColumns(); cpCell.x++)
		{
			CFrameObject* pFrame = pTable->GetCellFrame(cpCell);
			if (pFrame != NULL)
			{
				// If we have not computed the size yet, do it now.
				if (nSize == -1)
				{
					PBOX Bound = pFrame->get_bound();
					nSize = scale_pcoord(Bound.y1-Bound.y0, 5*72, 12*PAGE_RESOLUTION);
					Style.Size(MakeFixed(nSize));
					Style.BaseSize(MakeFixed(nSize));
					Style.UpdateFontMetrics();
				}
				pFrame->SetTextStyle(&Style);
			}
		}
	}
}

// Build the format list.
void CTablePropertiesDlg::BuildFormatList(void)
{
	m_FormatList.SetRedraw(FALSE);
	m_FormatList.ResetContent();
	CTableFormatter** pFormatters = CTableFormatter::GetTableFormatters();
	ASSERT(pFormatters != NULL);
	while (*pFormatters != NULL)
	{
		CTableFormatter* pFormatter = *pFormatters++;
		// Get the name of the formatter.
		CString csName;
		csName.LoadString(pFormatter->GetFormatterName());
		// Add the entry.
		int nIndex = m_FormatList.AddString(csName);
		if (nIndex >= 0)
		{
			// Point back to the formatter in question.
			m_FormatList.SetItemData(nIndex, (DWORD)pFormatter);
		}
	}
	m_FormatList.SetRedraw(TRUE);
}


void CTablePropertiesDlg::OnSelchangeFormats() 
{
	UpdateFormatPreview();
}

void CTablePropertiesDlg::UpdateFormatPreview(void)
{
	int nCurSel = m_FormatList.GetCurSel();
	if (nCurSel >= 0)
	{
		CTableFormatter* pFormatter = (CTableFormatter*)m_FormatList.GetItemData(nCurSel);
		CTableObject* pTable = m_pTableControl->GetTable();
		if (pFormatter != NULL && pTable != NULL)
		{
			if (pFormatter->FormatTable(pTable))
			{
				int nRows = pTable->GetRows();
				int nColumns = pTable->GetColumns();

				// Shove data into all the cells.
				UINT uCellData;
				if ((uCellData = pFormatter->GetCellData()) != 0)
				{
					if (m_uLastCellData != uCellData)
					{
						// Remember this as the last cell data we loaded.
						m_uLastCellData = uCellData;
						// Read the cell data.
						CString csData;
						csData.LoadString(uCellData);
						// Split it into the appropriate cells.
						CMlsStringArray csaCellData;
						csaCellData.SetNames(csData, '|');

						// Initialize a txp for general usage.
						CTxp Txp(pTable->get_database());
						CPoint cpCell;
						for (cpCell.y = 0; cpCell.y < nRows; cpCell.y++)
						{
							for (cpCell.x = 0; cpCell.x < nColumns; cpCell.x++)
							{
								// Get the text frame for this cell.
								CFrameObject* pFrame = pTable->GetCellFrame(cpCell, FALSE);
								if (pFrame != NULL)
								{
									// Set up to operate on the frame.
									Txp.Init(pFrame, 0);

									// Get rid of any text in the frame.
									Txp.SelectAll();
									if (Txp.SomethingSelected())
									{
										Txp.DeleteText(1);
									}

									// Insert the new text.
									int nIndex = cpCell.y*nColumns+cpCell.x;
									if (nIndex < csaCellData.GetSize())
									{
										// Calculate the style before inserting the text.
										Txp.CalcStyle();

										// Add the text.
										Txp.InsertString(csaCellData.GetAt(nIndex));
									}
									pFrame->calc();
								}
							}
						}
					}
				}
				m_pTableControl->Invalidate();
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTableCreateDialog dialog

CTableCreateDialog::CTableCreateDialog(CTableObject* pTable, CWnd* pParent /*=NULL*/)
	: CTablePropertiesDlg(pTable, pParent, CTableCreateDialog::IDD)
{
	//{{AFX_DATA_INIT(CTableCreateDialog)
	//}}AFX_DATA_INIT
}

void CTableCreateDialog::DoDataExchange(CDataExchange* pDX)
{
	CTablePropertiesDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTableCreateDialog)
	DDX_Control(pDX, IDC_SPIN_COLS, m_ctlColumns);
	DDX_Control(pDX, IDC_SPIN_ROWS, m_ctlRows);
	DDX_Text(pDX, IDC_EDIT_COLS, m_nColumns);
	DDV_MinMaxInt(pDX, m_nColumns, 1, CTableObject::GetMaxColumns());
	DDX_Text(pDX, IDC_EDIT_ROWS, m_nRows);
	DDV_MinMaxInt(pDX, m_nRows, 1, CTableObject::GetMaxRows());
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTableCreateDialog, CTablePropertiesDlg)
	//{{AFX_MSG_MAP(CTableCreateDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableCreateDialog message handlers

BOOL CTableCreateDialog::OnInitDialog() 
{
	// Set the default number of rows and columns.
	m_nRows = 5;
	m_nColumns = 5;

	CTablePropertiesDlg::OnInitDialog();

	m_ctlRows.SetRange(1, CTableObject::GetMaxRows());
	m_ctlColumns.SetRange(1, CTableObject::GetMaxColumns());

	return FALSE;
}

void CTableCreateDialog::OnOK() 
{
	if (!UpdateData(TRUE))
	{
		return;
	}

	// Build the rows and columns for the table.
	TABLE_INFO info;
	info.m_nRows = m_nRows;
	info.m_nColumns = m_nColumns;

	if (m_pTable != NULL)
	{
		m_pTable->Rebuild(&info);
	}

	CTablePropertiesDlg::OnOK();
}
