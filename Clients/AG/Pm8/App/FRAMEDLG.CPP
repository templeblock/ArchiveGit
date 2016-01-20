/*
// $Header: /PM8/App/FRAMEDLG.CPP 1     3/03/99 6:05p Gbeddow $
//
// Tabs dialog
//
// $Log: /PM8/App/FRAMEDLG.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
//    Rev 1.0   14 Aug 1997 15:21:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:32   Fred
// Initial revision.
// 
//    Rev 1.2   06 Jan 1997 10:15:20   Fred
// Start of new text object
// 
//    Rev 1.1   24 Jun 1996 18:06:52   Jay
//  
// 
//    Rev 1.0   21 Jun 1996 17:50:42   Jay
//  
*/

#include "stdafx.h"
#include "pmw.h"
#include "framedlg.h"
#include "framerec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrameAttributesDialog dialog

CFrameAttributesDialog::CFrameAttributesDialog(const CFrameAttributes* pAttributes, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CFrameAttributesDialog::IDD, pParent)
{
	m_nVerticalAlignment = pAttributes->m_nVerticalAlignment;

	m_fLeftMargin = (pAttributes->m_Margins.Left != UNDEFINED_CFIXED);
	m_dLeftMargin = MakeDouble(pAttributes->m_Margins.Left);
	m_fRightMargin = (pAttributes->m_Margins.Right != UNDEFINED_CFIXED);
	m_dRightMargin = MakeDouble(pAttributes->m_Margins.Right);
	m_fTopMargin = (pAttributes->m_Margins.Top != UNDEFINED_CFIXED);
	m_dTopMargin = MakeDouble(pAttributes->m_Margins.Top);
	m_fBottomMargin = (pAttributes->m_Margins.Bottom != UNDEFINED_CFIXED);
	m_dBottomMargin = MakeDouble(pAttributes->m_Margins.Bottom);

	m_fColumns = (pAttributes->m_nColumns != -1);
	m_nColumns = pAttributes->m_nColumns;

	m_fColumnSpacing = (pAttributes->m_lColumnSpacing != UNDEFINED_CFIXED);
	m_dColumnSpacing = MakeDouble(pAttributes->m_lColumnSpacing);

	m_pWarpField = pAttributes->m_pWarpField;

	//{{AFX_DATA_INIT(CFrameAttributesDialog)
	//}}AFX_DATA_INIT
}

void CFrameAttributesDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrameAttributesDialog)
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		GetDoubleValue(pDX, IDC_LEFT_MARGIN, m_dLeftMargin, m_fLeftMargin, 0., 99.);
		GetDoubleValue(pDX, IDC_RIGHT_MARGIN, m_dRightMargin, m_fRightMargin, 0., 99.);
		GetDoubleValue(pDX, IDC_TOP_MARGIN, m_dTopMargin, m_fTopMargin, 0., 99.);
		GetDoubleValue(pDX, IDC_BOTTOM_MARGIN, m_dBottomMargin, m_fBottomMargin, 0., 99.);
		GetDoubleValue(pDX, IDC_COLUMN_SPACING, m_dColumnSpacing, m_fColumnSpacing, 0., 99.);
		GetIntValue(pDX, IDC_COLUMNS, m_nColumns, m_fColumns, 1, 20);
		m_pWarpField = m_WarpShape.WarpField();
	}
	else
	{
		SetDoubleValue(IDC_LEFT_MARGIN, m_dLeftMargin, -4, m_fLeftMargin);
		SetDoubleValue(IDC_RIGHT_MARGIN, m_dRightMargin, -4, m_fRightMargin);
		SetDoubleValue(IDC_TOP_MARGIN, m_dTopMargin, -4, m_fTopMargin);
		SetDoubleValue(IDC_BOTTOM_MARGIN, m_dBottomMargin, -4, m_fBottomMargin);
		SetDoubleValue(IDC_COLUMN_SPACING, m_dColumnSpacing, -4, m_fColumnSpacing);
		SetIntValue(IDC_COLUMNS, m_nColumns, m_fColumns);
	}

	DDX_CBIndex(pDX, IDC_ALIGNMENT, m_nVerticalAlignment);
}

BEGIN_MESSAGE_MAP(CFrameAttributesDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CFrameAttributesDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFrameAttributesDialog::GetExitAttributes(CFrameAttributes* pAttributes)
{
	pAttributes->m_nVerticalAlignment = m_nVerticalAlignment;

	pAttributes->m_Margins.Left = m_fLeftMargin ? MakeFixed(m_dLeftMargin) : UNDEFINED_CFIXED;
	pAttributes->m_Margins.Top = m_fTopMargin ? MakeFixed(m_dTopMargin) : UNDEFINED_CFIXED;
	pAttributes->m_Margins.Right = m_fRightMargin ? MakeFixed(m_dRightMargin) : UNDEFINED_CFIXED;
	pAttributes->m_Margins.Bottom = m_fBottomMargin ? MakeFixed(m_dBottomMargin) : UNDEFINED_CFIXED;

	pAttributes->m_nColumns = m_fColumns ? m_nColumns : -1;
	pAttributes->m_lColumnSpacing = m_fColumnSpacing ? MakeFixed(m_dColumnSpacing) : UNDEFINED_CFIXED;

	pAttributes->m_pWarpField = m_pWarpField;
}

/////////////////////////////////////////////////////////////////////////////
// CFrameAttributesDialog message handlers

BOOL CFrameAttributesDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
	// initialize the warp shape list box
	
	m_WarpShape.SubclassDlgItem(IDC_SHAPE_LIST, this);

	if (m_WarpFile.Init(GetGlobalPathManager()->ExpandPath("PMW.WRP")))
	{
		m_WarpShape.WarpFields(&m_WarpFile, "Rectangle");
	}
	
	m_WarpShape.SizeItems();
	
	m_WarpShape.WarpField(m_pWarpField);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
