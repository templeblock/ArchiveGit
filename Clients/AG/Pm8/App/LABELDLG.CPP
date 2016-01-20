/*
// $Workfile: LABELDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LABELDLG.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:58   Fred
// Initial revision.
// 
//    Rev 1.5   17 Apr 1997 16:07:12   johno
// I killed the little guy!
// 
// 
//    Rev 1.4   17 Apr 1997 15:15:12   johno
// Changes for the lil' PrintMaster guy
// 
// 
//    Rev 1.3   15 Apr 1997 16:56:32   johno
// Changes for "New Project" path
// 
// 
//    Rev 1.2   10 Apr 1997 17:14:40   johno
// Now a PmwDialogColor
// 
// 
//    Rev 1.1   07 May 1996 12:44:00   Jay
// From PMW2
// 
//    Rev 1.6   03 May 1996 15:20:16   JAY
// New label reading code
// 
//    Rev 1.5   06 Oct 1995 12:00:00   JAY
// New print code, etc.
// 
//    Rev 1.4   18 Aug 1995 13:10:06   FRED
// Fixed label dialog bug
// 
//    Rev 1.3   17 Aug 1995 07:49:30   JAY
// Added double-click handler for the list.
// 
//    Rev 1.2   16 Aug 1995 16:22:26   JAY
// New stuff.
// 
//    Rev 1.1   26 Jul 1995 07:41:44   JAY
// Envelope support
// 
//    Rev 1.0   20 Jul 1995 12:52:54   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "labeldlg.h"
#include "lblprvw.h"
#include "printer.h"
#include "colordef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabelTypeDialog dialog

CLabelTypeDialog::CLabelTypeDialog(CPaperInfo* pOldInfo, CWnd* pParent /*=NULL*/, DWORD WizFlags, COLORREF cr /*INVALID_DIALOG_COLOR*/)
	: CPmwDialogColor(CLabelTypeDialog::IDD, pParent, cr)
{
	//{{AFX_DATA_INIT(CLabelTypeDialog)
	//}}AFX_DATA_INIT

	if (pOldInfo == NULL || pOldInfo->Class() == CPaperInfo::CLASS_Labels)
	{
		m_pOldInfo = pOldInfo;
	}
	else
	{
		m_pOldInfo = NULL;
	}
	m_pChosenLabel = NULL;

	m_WizFlags = WizFlags;
}

void CLabelTypeDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialogColor::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLabelTypeDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLabelTypeDialog, CPmwDialogColor)
	//{{AFX_MSG_MAP(CLabelTypeDialog)
	ON_LBN_SELCHANGE(IDC_LABEL_LIST, OnSelchangeLabelList)
	ON_BN_CLICKED(IDC_LABEL_TYPE_LASER, OnLabelTypeChanged)
	ON_BN_CLICKED(IDC_LABEL_TYPE_DOT_MATRIX, OnLabelTypeChanged)
	ON_LBN_DBLCLK(IDC_LABEL_LIST, OnDblclkLabelList)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabelTypeDialog message handlers

BOOL CLabelTypeDialog::OnInitDialog()
{
	CPmwDialogColor::OnInitDialog();

	m_pPreview = (CLabelPreview*)GetDlgItem(IDC_PREVIEW);

/*
// Read the list.
*/

	m_List.ReadList("LABELS.DAT");

/*
// Set the type to view.
// This depends on either the old paper passed or the printer type.
*/

	BOOL fTractor;

	if (m_pOldInfo == NULL)
	{
	/* Get the type from the printer. */
		fTractor = GET_PMWAPP()->GetPrinterInfo(PORTRAIT)->Tractor() == 1;
	}
	else
	{
	/* Get the type from the old info. */
		fTractor = (m_pOldInfo->Type() == CPaperInfo::PAPER_Continuous);
	}

	CheckDlgButton(IDC_LABEL_TYPE_LASER, !fTractor);
	CheckDlgButton(IDC_LABEL_TYPE_DOT_MATRIX, fTractor);

/*
// Build the list box.
*/

	BuildList();

   CreateWizzardButtons ();
   EnableWizardButtons (m_WizFlags);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLabelTypeDialog::BuildList(void)
{
	int nType = IsDlgButtonChecked(IDC_LABEL_TYPE_LASER)
						? CPaperInfo::PAPER_CutSheet
						: CPaperInfo::PAPER_Continuous;

	/*
	// Get the name to match.
	*/

	CString csOldName;
	if (m_pOldInfo != NULL)
	{
		csOldName = m_pOldInfo->GetName();
	}

	int nNewSel = 0;
	CListBox* pList;

	if ((pList = (CListBox*)GetDlgItem(IDC_LABEL_LIST)) != NULL)
	{
		pList->SetRedraw(FALSE);
		pList->ResetContent();
		int nLabels = m_List.Labels();
		for (int nLabel = 0; nLabel < nLabels; nLabel++)
		{
			CLabelData* pLabel = m_List.Label(nLabel);
			ASSERT(pLabel != NULL);
			if (pLabel != NULL && pLabel->Type() == nType)
			{
				int nIndex = pList->AddString(pLabel->GetName());
				if (nIndex >= 0)
				{
					pList->SetItemData(nIndex, (DWORD)pLabel);

					if (pLabel->GetName() == csOldName)
					{
						nNewSel = nIndex;
					}
				}
			}
		}

	/*
	// Set the initial entry as necessary.
	*/

		if (pList->GetCount() > 0)
		{
			pList->SetCurSel(nNewSel);
			OnSelchangeLabelList();
		}
		pList->SetRedraw(TRUE);
	}
}

void CLabelTypeDialog::OnSelchangeLabelList()
{
	CLabelData* pLabelData = NULL;

	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_LABEL_LIST)) != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel != -1)
		{
			pLabelData = (CLabelData*)pList->GetItemData(nCurSel);
		}
	}
	m_pPreview->SetLabel(pLabelData);
}

CLabelData* CLabelTypeDialog::GetChosenLabel(void)
{
	return m_pChosenLabel;
}

void CLabelTypeDialog::OnLabelTypeChanged()
{
	BuildList();
}

void CLabelTypeDialog::OnOK()
{
	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_LABEL_LIST)) != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel != -1)
		{
			m_pChosenLabel = (CLabelData*)pList->GetItemData(nCurSel);
		}
	}
	CPmwDialogColor::OnOK();
}

void CLabelTypeDialog::OnDblclkLabelList()
{
	OnOK();
}

void CLabelTypeDialog::OnBack() 
{
   EndDialog (IDC_BACK);
}
