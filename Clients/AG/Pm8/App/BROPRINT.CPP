/*
// $Workfile: BROPRINT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/BROPRINT.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:14   Fred
// Initial revision.
// 
//    Rev 1.5   08 Apr 1997 16:11:14   Jay
// Print Instructions control
// 
//    Rev 1.4   01 Nov 1996 17:12:14   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.3   07 Aug 1996 08:51:40   Jay
//  
// 
//    Rev 1.2   06 Aug 1996 14:36:34   Jay
//  
// 
//    Rev 1.1   06 Aug 1996 11:17:08   Jay
//  
// 
//    Rev 1.0   27 Jul 1996 10:09:26   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "broprint.h"
#include "printer.h"
#include "pmwini.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrochurePrintDialog dialog

CBrochurePrintDialog::CBrochurePrintDialog(CPmwView* pView, int nID /*=IDD*/)
	: CPmwPrint(pView, NULL, nID)
{
	//{{AFX_DATA_INIT(CBrochurePrintDialog)
	m_nPrintWhat = 0;
	m_fDoubleSided = FALSE;
	//}}AFX_DATA_INIT

	m_pd.nMinPage = 1;
	m_pd.nMaxPage = (USHORT)pView->GetDocument()->NumberOfPages();
}

void CBrochurePrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(m_pView->GetDocument()->get_orientation());

	if (!pDX->m_bSaveAndValidate)
	{
		m_fDoubleSided = pPrinterInfo->GetProfileInt(ENTRY_DoubleSided, TRUE);
	}

	CPmwPrint::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrochurePrintDialog)
	DDX_Radio(pDX, IDC_PRINT_BOTH, m_nPrintWhat);
	DDX_Check(pDX, IDC_PRINT_DOUBLE_SIDED, m_fDoubleSided);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		pPrinterInfo->WriteProfileInt(ENTRY_DoubleSided, m_fDoubleSided);
	}
}

BEGIN_MESSAGE_MAP(CBrochurePrintDialog, CPmwPrint)
	//{{AFX_MSG_MAP(CBrochurePrintDialog)
	ON_BN_CLICKED(IDC_PRINT_BOTH, OnPrintBoth)
	ON_BN_CLICKED(IDC_PRINT_BACK, OnPrintBack)
	ON_BN_CLICKED(IDC_PRINT_FRONT, OnPrintFront)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrochurePrintDialog message handlers

void CBrochurePrintDialog::OnOK()
{
	CPmwPrint::OnOK();
	
	/*
	// Can't do this in DoDataExchange because Print dialog changes them
	// during default processing.
	*/

	int nFromPage = 1;
	int nToPage = 2;

	switch (m_nPrintWhat)
	{
		case 0:
		{
			// Print Both
			// Set by default.
			break;
		}
		case 1:
		{
			// Print Front.
			nFromPage = 1;
			nToPage = 1;
			break;
		}
		case 2:
		{
			// Print Back
			nFromPage = 2;
			nToPage = 2;
			break;
		}
		default:
		{
			ASSERT(FALSE);
			break;
		}
	}
	m_pd.nFromPage = nFromPage;
	m_pd.nToPage = nToPage;
}

void CBrochurePrintDialog::OnPrintBoth() 
{
	SetPages(1, 2);
}

void CBrochurePrintDialog::OnPrintBack() 
{
	SetPages(2, 2);
}

void CBrochurePrintDialog::OnPrintFront() 
{
	SetPages(1, 1);
}

void CBrochurePrintDialog::SetPages(int nFromPage, int nToPage)
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

BOOL CBrochurePrintDialog::OnInitDialog() 
{
	CPmwPrint::OnInitDialog();
	UpdateInstructions();
	return TRUE;
}
