/*
// $Workfile: LBLPRINT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LBLPRINT.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     7/24/98 10:29p Jayn
// Post card printing. Label count adjustment for name list.
// 
//    Rev 1.0   14 Aug 1997 15:22:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:00   Fred
// Initial revision.
// 
//    Rev 1.1   02 Jul 1997 10:21:40   Jay
// Defaults CardCopies to number of slots on page.
// 
//    Rev 1.0   14 Mar 1996 13:32:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:10   FRED
// Initial revision.
// 
//    Rev 1.7   05 Sep 1995 10:18:08   JAY
// Now always reflows macros after printing.
// 
//    Rev 1.6   04 Sep 1995 15:11:32   JAY
// Now passes update_names_chosen() on to parent class.
// 
//    Rev 1.5   04 Sep 1995 14:56:44   JAY
// New collate handling.
// 
//    Rev 1.4   19 Aug 1995 12:04:20   FRED
// Business card print dialogs
// 
//    Rev 1.3   10 Aug 1995 13:22:40   JAY
//  
// 
//    Rev 1.2   09 Aug 1995 13:11:26   JAY
// Misc changes.
// 
//    Rev 1.1   21 Jul 1995 17:20:54   JAY
// Label printing
// 
//    Rev 1.0   20 Jul 1995 16:27:14   JAY
//  
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "lblprint.h"

#include "macrostd.h"
#include "paper.h"
#include "namerec.h"
#include "lblprvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabelPrintDialog dialog

CLabelPrintDialog::CLabelPrintDialog(CPmwView* pView, int nID /*=IDD*/)
	: CPmwPrint(pView, NULL, nID)
{
	//{{AFX_DATA_INIT(CLabelPrintDialog)
	m_fCollate = FALSE;
	m_fPageBreak = FALSE;
	m_nLabelCopies = 0;
	//}}AFX_DATA_INIT

	// Set the number of label copies to the full count unless
	// we have names chosen (in which case we default to 1 each).
	NameListRecord* pNameList = m_pDoc->GetMacroServer()->GetNameList();
	m_nLabelCopies = (pNameList->Names() == 0)
							? m_pDoc->GetPaperInfo()->Slots()
							: 1;

	m_nStartingLabel = 0;
}

void CLabelPrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwPrint::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLabelPrintDialog)
	DDX_Check(pDX, IDC_COLLATE, m_fCollate);
	DDX_Check(pDX, IDC_PAGE_BREAK, m_fPageBreak);
	DDX_Text(pDX, IDC_LABEL_COPIES, m_nLabelCopies);
	DDV_MinMaxInt(pDX, m_nLabelCopies, 1, 99);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLabelPrintDialog, CPmwPrint)
	//{{AFX_MSG_MAP(CLabelPrintDialog)
	ON_BN_CLICKED(IDC_STARTING_LABEL, OnStartingLabel)
	ON_EN_CHANGE(IDC_LABEL_COPIES, OnChangeLabelCopies)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabelPrintDialog message handlers


BOOL CLabelPrintDialog::OnInitDialog()
{
	CPmwPrint::OnInitDialog();

	UpdateStartingLabel();
	UpdateCollate();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLabelPrintDialog::ComputeTotals(void)
{
	CPaperInfo* pPaperInfo = m_pDoc->GetPaperInfo();
	CStdMacroServer* pMacroServer = m_pDoc->GetMacroServer();
	NameListRecord* pNameList = pMacroServer->GetNameList();

	int nLabelsPerPage = pPaperInfo->Slots();
	int nLabelsOnFirstPage = nLabelsPerPage - m_nStartingLabel;

	int nNames = pNameList->Names();
	if (nNames == 0)
	{
		nNames++;
	}

/*
// Compute the total number of labels.
*/

	m_nTotalLabels = m_nLabelCopies*nNames;

/*
// Compute the total number of pages.
*/

	int nLabelsPerSet;
	int nSets;

	if (nNames > 1
		 && m_nLabelCopies > 1
		 && m_fPageBreak)
	{
	/*
	// We have a multi-name and multi-copy case with page breaks between sets.
	*/
		if (m_fCollate)
		{
		/*
		// Each set consists of one of each name.
		// There is one set printed for each copy requested.
		*/
			nLabelsPerSet = nNames;
			nSets = m_nLabelCopies;
		}
		else
		{
		/*
		// Each set consists of all copies of one name.
		// There is one set per name requested.
		*/
			nLabelsPerSet = m_nLabelCopies;
			nSets = nNames;
		}
	}
	else
	{
	/*
	// This is a case where all labels are contiguous on the pages.
	*/
		nLabelsPerSet = m_nTotalLabels;
		nSets = 1;
	}

/*
// Compute the number of pages per set.
*/

	m_nTotalPages = 1;

	int nLabelsOnThisPage = nLabelsOnFirstPage;

	while (nLabelsPerSet > nLabelsOnThisPage)
	{
		m_nTotalPages++;
		nLabelsPerSet -= nLabelsOnThisPage;

		nLabelsOnThisPage = nLabelsPerPage;
	}

/*
// And multuply by the number of sets.
*/

	m_nTotalPages *= nSets;
}

void CLabelPrintDialog::OnStartingLabel()
{
	CLabelStartDialog Dialog(m_pDoc->GetPaperInfo(), this);

	Dialog.m_nStartingLabel = m_nStartingLabel;

	if (Dialog.DoModal() == IDOK)
	{
		m_nStartingLabel = Dialog.m_nStartingLabel;
		UpdateStartingLabel();
	}
}

void CLabelPrintDialog::UpdateStartingLabel(void)
{
	char buffer[10];
	sprintf(buffer, "%d", m_nStartingLabel+1);

	SetDlgItemText(IDC_STARTING_LABEL_CHOSEN, buffer);
}

void CLabelPrintDialog::UpdateCollate(void)
{
	BOOL fEnable = FALSE;
	CWnd* pChild = GetDlgItem(IDC_LABEL_COPIES);
	if (pChild != NULL)
	{
		CString csText;
		pChild->GetWindowText(csText);
		if (atoi(csText) > 1)
		{
			CStdMacroServer* pMacroServer = m_pDoc->GetMacroServer();
			NameListRecord* pNameList = pMacroServer->GetNameList();

			if (pNameList->Names() > 1)
			{
				fEnable = TRUE;
			}
		}
	}
	pChild = GetDlgItem(IDC_COLLATE);
	if (pChild != NULL)
	{
		pChild->EnableWindow(fEnable);
	}
	pChild = GetDlgItem(IDC_PAGE_BREAK);
	if (pChild != NULL)
	{
		pChild->EnableWindow(fEnable);
	}
}

void CLabelPrintDialog::update_names_chosen(void)
{
	CPmwPrint::update_names_chosen();
	UpdateCollate();

	// If the user has chosen names, we do not want to use the number
	// of labels in the sheet. Even though the user may have chosen that
	// number (rare), it is more likely that it was set by the code.
	// So we correct it.

	CWnd* pChild = GetDlgItem(IDC_LABEL_COPIES);
	if (pChild != NULL)
	{
		NameListRecord* pNameList = m_pDoc->GetMacroServer()->GetNameList();
		int nNames = pNameList->Names();
		int nSlots = m_pDoc->GetPaperInfo()->Slots();

		CString csText;
		pChild->GetWindowText(csText);
		int nLabels = atoi(csText);
		int nNewLabels = nLabels;
		if (nLabels == nSlots && nNames > 0)
		{
			nNewLabels = 1;
		}
		else if (nLabels == 1 && nNames == 0)
		{
			nNewLabels = nSlots;
		}

		if (nLabels != nNewLabels)
		{
			CString cs;
			cs.Format("%d", nNewLabels);
			pChild->SetWindowText(cs);
		}
	}
}

void CLabelPrintDialog::OnChangeLabelCopies()
{
	UpdateCollate();
}

/////////////////////////////////////////////////////////////////////////////
// CLabelStartDialog dialog

CLabelStartDialog::CLabelStartDialog(CPaperInfo* pPaperInfo, CWnd* pParent /*=NULL*/, int nID /*=IDD*/)
	: CPmwDialog(nID, pParent)
{
	//{{AFX_DATA_INIT(CLabelStartDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPaperInfo = pPaperInfo;
	m_nStartingLabel = 0;
}

void CLabelStartDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLabelStartDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLabelStartDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CLabelStartDialog)
	ON_BN_DOUBLECLICKED(IDC_PREVIEW, OnDoubleclickedPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabelStartDialog message handlers

BOOL CLabelStartDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
	m_pPreview = (CLabelPreview*)GetDlgItem(IDC_PREVIEW);
	m_pPreview->WantSelect();
	m_pPreview->SetLabel(m_pPaperInfo);
	m_pPreview->SelectLabel(m_nStartingLabel);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLabelStartDialog::OnOK()
{
	m_nStartingLabel = m_pPreview->SelectedLabel();
	CPmwDialog::OnOK();
}

void CLabelStartDialog::OnDoubleclickedPreview()
{
	OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CBusinessCardPrintDialog dialog

CBusinessCardPrintDialog::CBusinessCardPrintDialog(CPmwView* pView, int nID /*=IDD*/)
	: CLabelPrintDialog(pView, nID)
{
	//{{AFX_DATA_INIT(CBusinessCardPrintDialog)
	//}}AFX_DATA_INIT
}

void CBusinessCardPrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CLabelPrintDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBusinessCardPrintDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBusinessCardPrintDialog, CLabelPrintDialog)
	//{{AFX_MSG_MAP(CBusinessCardPrintDialog)
	ON_BN_CLICKED(IDC_STARTING_LABEL, OnStartingLabel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBusinessCardPrintDialog message handlers

void CBusinessCardPrintDialog::OnStartingLabel()
{
	CBusinessCardStartDialog Dialog(m_pDoc->GetPaperInfo(), this);

	Dialog.m_nStartingLabel = m_nStartingLabel;

	if (Dialog.DoModal() == IDOK)
	{
		m_nStartingLabel = Dialog.m_nStartingLabel;
		UpdateStartingLabel();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBusinessCardStartDialog dialog

CBusinessCardStartDialog::CBusinessCardStartDialog(CPaperInfo* pPaperInfo, CWnd* pParent /*=NULL*/, int nID /*=IDD*/)
	: CLabelStartDialog(pPaperInfo, pParent, nID)
{
	//{{AFX_DATA_INIT(CBusinessCardStartDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CBusinessCardStartDialog::DoDataExchange(CDataExchange* pDX)
{
	CLabelStartDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBusinessCardStartDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBusinessCardStartDialog, CLabelStartDialog)
	//{{AFX_MSG_MAP(CBusinessCardStartDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBusinessCardStartDialog message handlers
