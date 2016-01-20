/*
// $Header: /PM8/App/PARADLG.CPP 1     3/03/99 6:08p Gbeddow $
//
// Paragraph format dialog
//
// $Log: /PM8/App/PARADLG.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 3     8/25/98 12:05p Mwilson
// changed some helpful hints
// 
// 2     8/24/98 5:38p Mwilson
// enabled more helpful hints
// 
//    Rev 1.0   14 Aug 1997 15:23:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:36   Fred
// Initial revision.
// 
//    Rev 1.3   18 Jul 1996 11:12:02   Jay
// Text Wrap
// 
//    Rev 1.2   21 Jun 1996 17:50:44   Jay
//  
// 
//    Rev 1.1   18 Jun 1996 16:15:16   Jay
//  
// 
//    Rev 1.0   17 Jun 1996 12:03:28   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "Paradlg.h"
#include "text.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParagraphFormatDialog dialog

CParagraphFormatDialog::CParagraphFormatDialog(CTextStyle* pStyle, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CParagraphFormatDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParagraphFormatDialog)
	//}}AFX_DATA_INIT

	m_fLeftMargin = (pStyle->LeftMargin() != UNDEFINED_CFIXED);
	m_dLeftMargin = MakeDouble(pStyle->LeftMargin());
	m_fRightMargin = (pStyle->RightMargin() != UNDEFINED_CFIXED);
	m_dRightMargin = MakeDouble(pStyle->RightMargin());
	m_fFirstLineIndent = (pStyle->FirstLineIndent() != UNDEFINED_CFIXED);
	m_dFirstLineIndent = MakeDouble(pStyle->FirstLineIndent());
	m_fLeading = (pStyle->Leading() != UNDEFINED_CFIXED);
	m_dLeading = MakeDouble(pStyle->Leading());
	m_fSpaceBefore = (pStyle->SpaceBefore() != UNDEFINED_CFIXED);
	m_dSpaceBefore = MakeDouble(pStyle->SpaceBefore());
	m_fSpaceAfter = (pStyle->SpaceAfter() != UNDEFINED_CFIXED);
	m_dSpaceAfter = MakeDouble(pStyle->SpaceAfter());

	m_nAlignment = pStyle->Alignment();
	m_nLeadingType = pStyle->LeadingType();
}

void CParagraphFormatDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParagraphFormatDialog)
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		GetDoubleValue(pDX, IDC_LEFT_MARGIN, m_dLeftMargin, m_fLeftMargin, 0., 99.);
		GetDoubleValue(pDX, IDC_RIGHT_MARGIN, m_dRightMargin, m_fRightMargin, 0., 99.);
		GetDoubleValue(pDX, IDC_FIRST_LINE_INDENT, m_dFirstLineIndent, m_fFirstLineIndent, -99., 99.);
		GetDoubleValue(pDX, IDC_LEADING_VALUE, m_dLeading, m_fLeading, 0., 99.);
		GetDoubleValue(pDX, IDC_SPACE_BEFORE, m_dSpaceBefore, m_fSpaceBefore, 0., 99.);
		GetDoubleValue(pDX, IDC_SPACE_AFTER, m_dSpaceAfter, m_fSpaceAfter, 0., 99.);
	}
	else
	{
		SetDoubleValue(IDC_LEFT_MARGIN, m_dLeftMargin, -4, m_fLeftMargin);
		SetDoubleValue(IDC_RIGHT_MARGIN, m_dRightMargin, -4, m_fRightMargin);
		SetDoubleValue(IDC_FIRST_LINE_INDENT, m_dFirstLineIndent, -4, m_fFirstLineIndent);
		SetDoubleValue(IDC_LEADING_VALUE, m_dLeading, -4, m_fLeading);
		SetDoubleValue(IDC_SPACE_BEFORE, m_dSpaceBefore, -4, m_fSpaceBefore);
		SetDoubleValue(IDC_SPACE_AFTER, m_dSpaceAfter, -4, m_fSpaceAfter);
	}

	// Unconditional ones.
	DDX_CBIndex(pDX, IDC_ALIGNMENT, m_nAlignment);
	DDX_Radio(pDX, IDC_LEADING_LINES, m_nLeadingType);
}

BEGIN_MESSAGE_MAP(CParagraphFormatDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CParagraphFormatDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParagraphFormatDialog message handlers

BOOL CParagraphFormatDialog::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();


	return TRUE;
}

/*
// Set a style with the exit information.
*/

void CParagraphFormatDialog::GetExitStyle(CTextStyle* pStyle)
{
	pStyle->MakeParagraphUndefined();
	if (m_fLeftMargin)
	{
		pStyle->LeftMargin(MakeFixed(m_dLeftMargin));
	}
	if (m_fRightMargin)
	{
		pStyle->RightMargin(MakeFixed(m_dRightMargin));
	}
	if (m_fFirstLineIndent)
	{
		pStyle->FirstLineIndent(MakeFixed(m_dFirstLineIndent));
	}
	if (m_fSpaceBefore)
	{
		pStyle->SpaceBefore(MakeFixed(m_dSpaceBefore));
	}
	if (m_fSpaceAfter)
	{
		pStyle->SpaceAfter(MakeFixed(m_dSpaceAfter));
	}

	pStyle->LeadingType(m_nLeadingType);
	if (m_fLeading)
	{
		pStyle->Leading(MakeFixed(m_dLeading));
	}

	pStyle->Alignment(m_nAlignment);
}
/////////////////////////////////////////////////////////////////////////////
// CTextWrapDialog dialog


CTextWrapDialog::CTextWrapDialog(int nFlow, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CTextWrapDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextWrapDialog)
	m_nFlow = nFlow;
	//}}AFX_DATA_INIT
}

void CTextWrapDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextWrapDialog)
	DDX_Radio(pDX, IDC_FLOW_OVER, m_nFlow);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextWrapDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CTextWrapDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextWrapDialog message handlers
BOOL CTextWrapDialog::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();

	GET_PMWAPP()->ShowHelpfulHint("Text Wrap Dialog", this);

	return TRUE;
}

