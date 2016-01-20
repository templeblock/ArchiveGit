/*
// $Workfile: MLSPRINT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/MLSPRINT.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:52   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:41:42   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/MLSPRINT.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:30   Fred
// Initial revision.
// 
//    Rev 1.2   27 Mar 1996 17:03:44   JAY
// Sets orientation during printing. Sets parent correctly.
// 
//    Rev 1.1   27 Mar 1996 16:59:12   FRED
// Forces portrait
// 
//    Rev 1.0   11 Mar 1996 15:55:20   FRED
// Initial revision.
// 
//    Rev 1.1   02 Feb 1996 13:04:22   JAY
//  
// 
//    Rev 1.0   02 Feb 1996 08:58:28   JAY
// Initial revision.
// 
//    Rev 1.4   06 Dec 1994 13:38:52   FRED
//  
// 
//    Rev 1.3   11 Nov 1994 09:09:46   JAY
// 
//    Rev 1.2   10 Nov 1994 13:02:52   FRED
// 
//    Rev 1.1   09 Nov 1994 18:25:24   FRED
//  
// 
//    Rev 1.0   09 Nov 1994 17:15:04   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "mlsprint.h"


/////////////////////////////////////////////////////////////////////////////
// CMlsPrintingDialog

static BOOL NEAR fUserAbort = FALSE;

BEGIN_MESSAGE_MAP(CMlsPrintingDialog, CDialog)
	//{{AFX_MSG_MAP(CMlsPrintingDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMlsPrintingDialog::CMlsPrintingDialog(int nID, CWnd* pParent)
	: CBaseDialog(nID, pParent)
{
	m_pParent = pParent;
	
	Create(nID, m_pParent);
	fUserAbort = FALSE;
	
	if (m_pParent != NULL)
	{
		m_pParent->EnableWindow(FALSE);
	}
}

CMlsPrintingDialog::~CMlsPrintingDialog()
{
	if (m_pParent != NULL)
	{
		m_pParent->EnableWindow(TRUE);
	}
}

BOOL CALLBACK EXPORT MlsPrintAbortProc(HDC, int)
{
	MSG msg;
	while (!fUserAbort && ::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!AfxGetApp()->PumpMessage())
		{
			return FALSE;
		}
	}
	return !fUserAbort;
}

BOOL CMlsPrintingDialog::OnInitDialog()
{
	SetWindowText(AfxGetAppName());
	CenterWindow();
	return CBaseDialog::OnInitDialog();
}

void CMlsPrintingDialog::OnCancel()
{
	fUserAbort = TRUE;
	CBaseDialog::OnCancel();
}

BOOL CMlsPrintingDialog::DestroyWindow()
{
	if (m_pParent != NULL)
	{
		m_pParent->EnableWindow(TRUE);
		m_pParent = NULL;
	}

	return CBaseDialog::DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CMlsPrint

CMlsPrint::CMlsPrint()
{
	m_pInfo = NULL;
	m_pParent = NULL;
	m_pDC = NULL;
	m_pPrintingDialog = NULL;
}

CMlsPrint::~CMlsPrint()
{
	OnReleasePrintingDialog();
	m_pInfo = NULL;
	m_pParent = NULL;
	m_pDC = NULL;
}

BOOL CMlsPrint::Print(CMlsPrintInfo* pInfo, CWnd* pParent)
{
	BOOL fSuccess = FALSE;
	
	ASSERT(pInfo != NULL);
	
	m_pInfo = pInfo;
	m_pParent = pParent;
	
	if (OnPreparePrinting())
	{
		fSuccess = TRUE;
		
		ASSERT(m_pInfo->m_pPD->m_pd.hDC != NULL);

		CDC dcPrint;
		dcPrint.Attach(m_pInfo->m_pPD->m_pd.hDC);
		dcPrint.m_bPrinting = TRUE;
		m_pDC = &dcPrint;

		OnBeginPrinting();
		OnCreatePrintingDialog();
		
		dcPrint.SetAbortProc(MlsPrintAbortProc);

		CString docTitle;
		GetTitle(docTitle);
		
		if (docTitle.GetLength() > 31)
		{
			docTitle.ReleaseBuffer(31);
		}

		DOCINFO docInfo;
		memset(&docInfo, 0, sizeof(DOCINFO));
		docInfo.cbSize = sizeof(DOCINFO);
		docInfo.lpszDocName = docTitle;
//		docInfo.lpszOutput = NULL;

		if (dcPrint.StartDoc(&docInfo) != SP_ERROR)
		{
			UINT nEndPage = m_pInfo->GetToPage();
			UINT nStartPage = m_pInfo->GetFromPage();
	
			// Guarantee values are in the valid range
			if (nEndPage < m_pInfo->GetMinPage())
				nEndPage = m_pInfo->GetMinPage();
			if (nEndPage > m_pInfo->GetMaxPage())
				nEndPage = m_pInfo->GetMaxPage();
	
			if (nStartPage < m_pInfo->GetMinPage())
				nStartPage = m_pInfo->GetMinPage();
			if (nStartPage > m_pInfo->GetMaxPage())
				nStartPage = m_pInfo->GetMaxPage();
	
			int nStep = (nEndPage >= nStartPage) ? 1 : -1;
			nEndPage = (nEndPage == 0xffff) ? 0xffff : nEndPage + nStep;
	
			BOOL bError = FALSE;
			for (m_pInfo->m_nCurPage = nStartPage; !bError && m_pInfo->m_nCurPage != nEndPage; m_pInfo->m_nCurPage += nStep)
			{
				OnPrepareDC();
	
				if (!m_pInfo->m_bContinuePrinting)
				{
					break;
				}
	
				// Set up drawing rect to entire page (in logical coordinates)
				m_pInfo->m_rectDraw.SetRect(0, 0, dcPrint.GetDeviceCaps(HORZRES), dcPrint.GetDeviceCaps(VERTRES));
				dcPrint.DPtoLP(&m_pInfo->m_rectDraw);
				
				OnUpdatePrintingDialog();
				
				VERIFY(dcPrint.StartPage());
				
				OnPrint();
				
				if (dcPrint.EndPage() < 0)
				{
					bError = TRUE;
				}
			}
	
			if (!bError)
			{
				dcPrint.EndDoc();
			}
		}
		
		else
		{
			AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
		}
		
		OnEndPrinting();
		OnReleasePrintingDialog();
		
		m_pDC = NULL;
	}
	
	m_pInfo = NULL;
	
	return fSuccess;
}

BOOL CMlsPrint::OnPreparePrinting()
{
	ASSERT(m_pInfo != NULL);
	ASSERT(m_pInfo->m_pPD != NULL);

	// bring up the print dialog and allow user to change things

	// preset From-To range same as Min-Max range
	m_pInfo->m_pPD->m_pd.nFromPage = m_pInfo->GetMinPage();
	m_pInfo->m_pPD->m_pd.nToPage = m_pInfo->GetMaxPage();

	if (AfxGetApp()->DoPrintDialog(m_pInfo->m_pPD) != IDOK)
	{
		return FALSE;       // do not print
	}

	ASSERT(m_pInfo->m_pPD != NULL);
	ASSERT(m_pInfo->m_pPD->m_pd.hDC != NULL);

	return TRUE;
}

void CMlsPrint::OnPrepareDC()
{
	ASSERT_VALID(m_pDC);

	// Default to one page printing if doc length not known
	if (m_pInfo != NULL)
	{
		m_pInfo->m_bContinuePrinting = (m_pInfo->GetMaxPage() != 0xffff || (m_pInfo->m_nCurPage == 1));
	}
}

void CMlsPrint::OnBeginPrinting()
{
	ASSERT_VALID(m_pDC);
	
	CPrintDialog* pPD = m_pInfo->m_pPD;

	/* Make sure our orientation is set correctly. */

	HGLOBAL hDevMode = pPD->m_pd.hDevMode;

	if (hDevMode != NULL)
	{
		LPDEVMODE dm;

	/* Lock the DevMode structure! */

		if (GlobalSize(hDevMode) >= sizeof(DEVMODE))
		{
			if ((dm = (LPDEVMODE)GlobalLock(hDevMode)) != NULL)
			{
			/* Well, we have it now... */
		
				if (!(dm->dmFields & DM_ORIENTATION)
							|| dm->dmOrientation != m_pInfo->GetOrientation())
				{
				/*
				// Orientation is not set or not what we want.
				// Force it to be what we want.
				*/
					dm->dmFields |= DM_ORIENTATION;
					dm->dmOrientation = m_pInfo->GetOrientation();
					m_pDC->ResetDC(dm);
				}
			
				GlobalUnlock(hDevMode);
			}
		}
	}
}

void CMlsPrint::OnPrint()
{
	ASSERT_VALID(m_pDC);
	// Do printing here
}

void CMlsPrint::OnEndPrinting()
{
	ASSERT_VALID(m_pDC);
	// Do printing cleanup here
}

void CMlsPrint::OnCreatePrintingDialog()
{
	m_pPrintingDialog = new CMlsPrintingDialog(AFX_IDD_PRINTDLG, m_pParent);
	
	if (m_pPrintingDialog != NULL)
	{
		CString docTitle;
		GetTitle(docTitle);
		m_pPrintingDialog->SetDlgItemText(AFX_IDC_PRINT_DOCNAME, docTitle);
		m_pPrintingDialog->SetDlgItemText(AFX_IDC_PRINT_PRINTERNAME, m_pInfo->m_pPD->GetDeviceName());

		CString strPort;
		AfxFormatString1(strPort, AFX_IDS_PRINTONPORT, m_pInfo->m_pPD->GetPortName());
		m_pPrintingDialog->SetDlgItemText(AFX_IDC_PRINT_PORTNAME, strPort);
		m_pPrintingDialog->ShowWindow(SW_SHOW);
	}
}

void CMlsPrint::OnReleasePrintingDialog()
{
	if (m_pPrintingDialog != NULL)
	{
		m_pPrintingDialog->DestroyWindow();
		
		delete m_pPrintingDialog;
		m_pPrintingDialog = NULL;
	}
}

void CMlsPrint::OnUpdatePrintingDialog()
{
	if (m_pPrintingDialog != NULL)
	{
		CString strFmt;
		VERIFY(strFmt.LoadString(AFX_IDS_PRINTPAGENUM));
		char szBuf[80];
		wsprintf(szBuf, strFmt, m_pInfo->m_nCurPage);
		m_pPrintingDialog->SetDlgItemText(AFX_IDC_PRINT_PAGENUM, szBuf);
	}
}

void CMlsPrint::GetTitle(CString& csTitle)
{
	csTitle = "";
}


/////////////////////////////////////////////////////////////////////////////
// CMlsPrintInfo

CMlsPrintInfo::CMlsPrintInfo(CWnd* pParent /*=NULL*/)
{
	m_pPD = new CPrintDialog(FALSE, PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_HIDEPRINTTOFILE | PD_NOSELECTION, pParent);

	SetMinPage(1);              // one based page numbers
	SetMaxPage(0xffff);         // unknown how many pages

	m_nCurPage = 1;

	m_lpUserData = NULL;        // Initialize to no user data
	m_bContinuePrinting = TRUE; // Assume it is OK to print
	
	m_nOrientation = DMORIENT_PORTRAIT;
}

CMlsPrintInfo::~CMlsPrintInfo()
{
	delete m_pPD;
	m_pPD = NULL;
}
