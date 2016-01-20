/*
// $Workfile: NWSPRINT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/NWSPRINT.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 4     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 3     8/24/98 5:38p Mwilson
// enabled more helpful hints
// 
// 2     8/17/98 5:37p Mwilson
// added special dialog for printing tshirts
// 
//    Rev 1.0   14 Aug 1997 15:23:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:24   Fred
// Initial revision.
// 
//    Rev 1.5   20 Jul 1997 15:04:04   Jay
// Forces All into IDC_PRINT_ALL
// 
//    Rev 1.4   08 Apr 1997 16:11:14   Jay
// Print Instructions control
// 
//    Rev 1.3   01 Nov 1996 17:12:22   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.2   06 Aug 1996 14:36:36   Jay
//  
// 
//    Rev 1.1   06 Aug 1996 11:17:06   Jay
//  
// 
//    Rev 1.0   24 Jul 1996 07:54:06   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "NwsPrint.h"
#include "printer.h"
#include "pmwini.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewsletterPrintDialog dialog

CNewsletterPrintDialog::CNewsletterPrintDialog(CPmwView* pView, int nID /*=IDD*/)
	: CPmwPrint(pView, NULL, nID)
{
	//{{AFX_DATA_INIT(CNewsletterPrintDialog)
	m_fDoubleSided = FALSE;
	//}}AFX_DATA_INIT
	m_pd.Flags &= ~PD_NOPAGENUMS;
	m_pd.nMinPage = 1;
	m_pd.nMaxPage = (USHORT)pView->GetDocument()->NumberOfPages();

	m_pd.nFromPage = m_pd.nMinPage;
	m_pd.nToPage = m_pd.nMaxPage;
}

void CNewsletterPrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(m_pView->GetDocument()->get_orientation());

	if (!pDX->m_bSaveAndValidate)
	{
		m_fDoubleSided = pPrinterInfo->GetProfileInt(ENTRY_DoubleSided, TRUE);
	}

	CPmwPrint::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewsletterPrintDialog)
	DDX_Check(pDX, IDC_PRINT_DOUBLE_SIDED, m_fDoubleSided);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		pPrinterInfo->WriteProfileInt(ENTRY_DoubleSided, m_fDoubleSided);
	}
}

BEGIN_MESSAGE_MAP(CNewsletterPrintDialog, CPmwPrint)
	//{{AFX_MSG_MAP(CNewsletterPrintDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewsletterPrintDialog message handlers

BOOL CNewsletterPrintDialog::OnInitDialog() 
{
	CPmwPrint::OnInitDialog();

   // Windows 95 changes this string. We set it back.
   SetDlgItemText(IDC_PRINT_ALL, GET_PMWAPP()->GetResourceStringPointer(IDS_PrintAll));

	UpdateInstructions();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNewsletterPrintDialog dialog

CTShirtPrintDialog::CTShirtPrintDialog(CPmwView* pView, int nID /*=IDD*/)
	: CNewsletterPrintDialog(pView, nID)
{
	//{{AFX_DATA_INIT(CTShirtPrintDialog)
	//}}AFX_DATA_INIT
}

void CTShirtPrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CNewsletterPrintDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTShirtPrintDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTShirtPrintDialog, CNewsletterPrintDialog)
	//{{AFX_MSG_MAP(CTShirtPrintDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewsletterPrintDialog message handlers

BOOL CTShirtPrintDialog::OnInitDialog() 
{
	CNewsletterPrintDialog::OnInitDialog();

	CButton* pButton = (CButton*)GetDlgItem(IDC_PRINT_DOUBLE_SIDED);
	if(pButton)
	{
		pButton->SetCheck(0);
		pButton->EnableWindow(FALSE);
	}

	CWnd* pWnd = GetDlgItem(IDC_PRINT_INSTRUCTIONS);
	if(pWnd)
		pWnd->EnableWindow(FALSE);
	
		return TRUE;
}
