/*
// $Workfile: PCRDPRNT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
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
// $Log: /PM8/App/PCRDPRNT.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 3     7/24/98 10:29p Jayn
// Post card printing. Label count adjustment for name list.
// 
// 2     3/06/98 3:59p Jayn
// Post card print code (double-sided).
// 
//    Rev 1.1   02 Oct 1997 15:10:42   johno
// Various changes for post card printing
// 
//    Rev 1.0   29 Sep 1997 13:49:10   johno
// Initial revision.
// 
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "lblprint.h"

//#include "macrostd.h"
//#include "paper.h"
//#include "namerec.h"
//#include "lblprvw.h"

#include "pcrdprnt.h"

#include "printer.h"
#include "pmwini.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPostCardPrintDialog dialog

CPostCardPrintDialog::CPostCardPrintDialog(CPmwView* pView, int nID)
	: CLabelPrintDialog(pView, nID)
{
	//{{AFX_DATA_INIT(CPostCardPrintDialog)
	m_fDoubleSided = FALSE;
	m_nPrintWhat = 0;
	//}}AFX_DATA_INIT
}

void CPostCardPrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(m_pView->GetDocument()->get_orientation());

	if (!pDX->m_bSaveAndValidate)
	{
		m_fDoubleSided = pPrinterInfo->GetProfileInt(ENTRY_DoubleSided, TRUE);
	}

	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPostCardPrintDialog)
	DDX_Check(pDX, IDC_PRINT_DOUBLE_SIDED, m_fDoubleSided);
	DDX_Radio(pDX, IDC_PRINT_BOTH, m_nPrintWhat);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		pPrinterInfo->WriteProfileInt(ENTRY_DoubleSided, m_fDoubleSided);
	}
}

BEGIN_MESSAGE_MAP(CPostCardPrintDialog, CLabelPrintDialog)
	//{{AFX_MSG_MAP(CPostCardPrintDialog)
	ON_BN_CLICKED(IDC_STARTING_LABEL, OnStartingLabel)
	ON_BN_CLICKED(IDC_PRINT_BOTH, OnPrintBoth)
	ON_BN_CLICKED(IDC_PRINT_BACK, OnPrintBack)
	ON_BN_CLICKED(IDC_PRINT_FRONT, OnPrintFront)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPostCardPrintDialog::UpdateStartingLabel(void)
{
	char buffer[10];
	sprintf(buffer, "%d", m_nStartingLabel+1);

	SetDlgItemText(IDC_STARTING_LABEL_CHOSEN, buffer);
}

/////////////////////////////////////////////////////////////////////////////
// CPostCardPrintDialog message handlers

void CPostCardPrintDialog::OnStartingLabel()
{
	CPostCardStartDialog Dialog(m_pDoc->GetPaperInfo(), this);

	Dialog.m_nStartingLabel = m_nStartingLabel;

	if (Dialog.DoModal() == IDOK)
	{
		m_nStartingLabel = Dialog.m_nStartingLabel;
		UpdateStartingLabel();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPostCardStartDialog dialog

CPostCardStartDialog::CPostCardStartDialog(CPaperInfo* pPaperInfo, CWnd* pParent, int nID)
   : CLabelStartDialog(pPaperInfo, pParent, nID) 
{
	//{{AFX_DATA_INIT(CPostCardStartDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CPostCardStartDialog::DoDataExchange(CDataExchange* pDX)
{
	CLabelStartDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPostCardStartDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPostCardStartDialog, CLabelStartDialog)
	//{{AFX_MSG_MAP(CPostCardStartDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPostCardStartDialog message handlers

BOOL CPostCardPrintDialog::OnInitDialog() 
{
	INHERITED::OnInitDialog();
	UpdateInstructions();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPostCardPrintDialog::OnPrintBoth() 
{
	SetPages(1, 2);
}

void CPostCardPrintDialog::OnPrintBack() 
{
	SetPages(2, 2);
}

void CPostCardPrintDialog::OnPrintFront() 
{
	SetPages(1, 1);
}

void CPostCardPrintDialog::SetPages(int nFromPage, int nToPage)
{
	CString csNumber;

	TRY
	{
		CWnd* pControl;
		if ((pControl = GetDlgItem(IDC_PRINT_FROM)) != NULL)
		{
			csNumber.Format("%d", nFromPage);
			pControl->SetWindowText(csNumber);
		}
		if ((pControl = GetDlgItem(IDC_PRINT_TO)) != NULL)
		{
			csNumber.Format("%d", nToPage);
			pControl->SetWindowText(csNumber);
		}
	}
	END_TRY
}
