// PrntCalD.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "printer.h"
#include "pmwini.h"
#include "PrntCalD.h"
#include "caldoc.h"
#include "prntsize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintCalendarDlg dialog


CPrintCalendarDlg::CPrintCalendarDlg(CPmwView* pView, int nID)
	: CPmwPrint(pView, NULL, nID)
{
	//{{AFX_DATA_INIT(CPrintCalendarDlg)
	m_fDoubleSided = FALSE;
	//}}AFX_DATA_INIT
	
	m_pd.Flags &= ~PD_NOPAGENUMS;
	m_pd.nMinPage = 1;
	m_pd.nMaxPage = (USHORT)((CCalendarDoc*)pView->GetDocument())->NumberOfPrinterPages();

	m_pd.nFromPage = m_pd.nMinPage;
	m_pd.nToPage = m_pd.nMaxPage;

	m_pDoc = pView->GetDocument();
}


void CPrintCalendarDlg::DoDataExchange(CDataExchange* pDX)
{
	CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(m_pView->GetDocument()->get_orientation());

	if (!pDX->m_bSaveAndValidate)
	{
		m_fDoubleSided = pPrinterInfo->GetProfileInt(ENTRY_DoubleSided, TRUE);
	}

	CPmwPrint::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintCalendarDlg)
	DDX_Check(pDX, IDC_PRINT_DOUBLE_SIDED, m_fDoubleSided);
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
	{
		pPrinterInfo->WriteProfileInt(ENTRY_DoubleSided, m_fDoubleSided);
	}
}


BEGIN_MESSAGE_MAP(CPrintCalendarDlg, CPmwPrint)
	//{{AFX_MSG_MAP(CPrintCalendarDlg)
	ON_BN_CLICKED(IDC_PRINT_SIZE, OnPrintSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintCalendarDlg message handlers

BOOL CPrintCalendarDlg::OnInitDialog() 
{
	CPmwPrint::OnInitDialog();
	
	// Windows 95 changes this string. We set it back.
   SetDlgItemText(IDC_PRINT_ALL, GET_PMWAPP()->GetResourceStringPointer(IDS_PrintAll));

	UpdateInstructions();
	update_buttons();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPrintCalendarDlg::OnPrintSize() 
{
	CPmwPrint::OnClickedPrintSize();
}

void CPrintCalendarDlg::update_buttons()
{
	UpdateData();
	CWnd * child;
	BOOL bActive = m_pd.nMinPage == m_pd.nMaxPage;

	if ((child = GetDlgItem(IDC_PRINT_SIZE)) != NULL)
	{
		child->EnableWindow(bActive);
	}
	
	if (bActive) // de-activate print double sided
	{
		if ((child = GetDlgItem(IDC_PRINT_DOUBLE_SIDED)) != NULL)
		{
			((CButton*)child)->SetCheck(0);
			child->EnableWindow(!bActive);
		}
		
		if ((child = GetDlgItem(IDC_PRINT_INSTRUCTIONS)) != NULL)
		{
			((CButton*)child)->SetCheck(0);
			child->EnableWindow(!bActive);
		}
	}	
	
}

