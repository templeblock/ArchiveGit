// StkrPrnD.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"

#include "StkrPrnD.h"


#include "macrostd.h"
#include "paper.h"
#include "namerec.h"
#include "lblprvw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStickerPrintDialog dialog


CStickerPrintDialog::CStickerPrintDialog(CPmwView* pView, int nID)
	: CPmwPrint(pView, NULL, nID)
{
	//{{AFX_DATA_INIT(CStickerPrintDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pd.Flags &= ~PD_NOPAGENUMS;
	m_pd.nMinPage = 1;
	m_pd.nMaxPage = (USHORT)pView->GetDocument()->NumberOfPages();

	m_pd.nFromPage = m_pd.nMinPage;
	m_pd.nToPage = m_pd.nMaxPage;

	m_pDoc = pView->GetDocument();
}


void CStickerPrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwPrint::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStickerPrintDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStickerPrintDialog, CPmwPrint)
	//{{AFX_MSG_MAP(CStickerPrintDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStickerPrintDialog message handlers

BOOL CStickerPrintDialog::OnInitDialog() 
{
	CPmwPrint::OnInitDialog();

   // Windows 95 changes this string. We set it back.
   SetDlgItemText(IDC_PRINT_ALL, GET_PMWAPP()->GetResourceStringPointer(IDS_PrintAll));

	return TRUE;
}
