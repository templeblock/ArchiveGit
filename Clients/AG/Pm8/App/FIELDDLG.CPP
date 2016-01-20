/*
// $Workfile: FIELDDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/FIELDDLG.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 3     6/03/98 9:51a Fredf
// More changes for address book and merge names.
// 
// 2     5/18/98 3:47p Cboggio
// Changed hard-coded Receiver and Sender strings for address block
// insertion for localization. If LOCALIZE is defined, the address format
// is loaded from the resource file.
// 
//    Rev 1.0   14 Aug 1997 15:20:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:24   Fred
// Initial revision.
// 
//    Rev 1.3   13 Mar 1997 13:17:04   Jay
// Changes for 16-bit and 5.0 builds
// 
//    Rev 1.2   28 Oct 1996 10:13:36   johno
// Moved strings to resource file
// 
//    Rev 1.1   27 Jun 1996 17:21:04   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:33:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:28   FRED
// Initial revision.
// 
//    Rev 1.10   18 Jan 1996 17:09:54   JAY
// New project configuration class
// 
//    Rev 1.9   06 Sep 1995 10:33:48   FRED
// Buttons updated after rename.
// 
//    Rev 1.8   31 Aug 1995 10:04:16   FRED
// Miscellaneous fixes.
// 
//    Rev 1.7   30 Aug 1995 12:55:42   FRED
// Inital calls to UpdateButtons()
// 
//    Rev 1.6   30 Aug 1995 11:41:16   FRED
// Field name validation
// 
//    Rev 1.5   27 Aug 1995 14:30:46   FRED
// Some menu/dialog improvements
// 
//    Rev 1.4   27 Aug 1995 12:35:30   FRED
// New fill-in-fields stuff
// 
//    Rev 1.3   25 Aug 1995 18:38:08   JAY
// Receiver and sender address blocks.
// 
//    Rev 1.2   25 Aug 1995 15:00:48   JAY
// Insert fields dialog
// 
//    Rev 1.1   25 Aug 1995 09:28:26   JAY
// Made it compile.
// 
//    Rev 1.0   25 Aug 1995 09:18:34   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "macrostd.h"
#include "fielddlg.h"
#include "util.h"
#include "pmwcfg.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInsertFieldViewHelper class
// This is a standard class used to interface a CPmwView to the Insert Field
// dialog.

CInsertFieldViewHelper::CInsertFieldViewHelper(CPmwView* pView)
{
	m_pView = pView;
}

void CInsertFieldViewHelper::InsertMacro(int nMacro)
{
	CPmwDoc* pDoc = m_pView->GetDocument();
	CHARACTER c = pDoc->GetMacroServer()->GetMacroCharacter(nMacro);

	m_pView->InsertCharacter(c);
}

void CInsertFieldViewHelper::InsertText(CHARACTER* pText, int nSize)
{
	while (nSize-- > 0)
	{
		m_pView->InsertCharacter(*pText++);
	}
}

void CInsertFieldViewHelper::ReflowMacros(void)
{
	m_pView->GetDocument()->ReflowMacros();
}

CMacroList& CInsertFieldViewHelper::MacroList(void)
{
	return m_pView->GetDocument()->GetMacroServer()->MacroList();
}

void CInsertFieldViewHelper::GetMacrosInUse(CWordArray& cwaMacrosInUse)
{
	m_pView->GetDocument()->GetMacrosInUse(cwaMacrosInUse);
}

/////////////////////////////////////////////////////////////////////////////
// CInsertFieldDialog dialog

CString CInsertFieldDialog::m_csLastFieldName;		// remembers last name choosen from dialog
	
CInsertFieldDialog::CInsertFieldDialog(CInsertFieldHelper* pHelper, BOOL fMergeFieldsOnly /*=FALSE*/, UINT uID /*=(UINT)-1*/, CWnd* pParent /*=NULL*/)
	: CPmwDialog((uID != (UINT)-1) ? uID : CInsertFieldDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInsertFieldDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	ASSERT(pHelper != NULL);
	m_pHelper = pHelper;
	m_fChangedMacroList = FALSE;
	m_fMergeFieldsOnly = fMergeFieldsOnly;
}

void CInsertFieldDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertFieldDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInsertFieldDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CInsertFieldDialog)
	ON_BN_CLICKED(IDC_ADD_FIELD, OnAddField)
	ON_LBN_SELCHANGE(IDC_FIELD_LIST, OnSelchangeFieldList)
	ON_LBN_DBLCLK(IDC_FIELD_LIST, OnDblclkFieldList)
	ON_BN_CLICKED(IDC_INSERT_FIELD, OnInsertField)
	ON_BN_CLICKED(IDC_INSERT_RECEIVER, OnInsertReceiver)
	ON_BN_CLICKED(IDC_INSERT_SENDER, OnInsertSender)
	ON_BN_CLICKED(IDC_REMOVE_FIELD, OnRemoveField)
	ON_BN_CLICKED(IDC_RENAME_FIELD, OnRenameField)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInsertFieldDialog message handlers

BOOL CInsertFieldDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();

	// Macro list assignment!
	m_MacroList = m_pHelper->MacroList();

	m_pHelper->GetMacrosInUse(m_cwaMacrosInUse);

	BuildList();
	
	CListBox* pList = (CListBox*)GetDlgItem(IDC_FIELD_LIST);
	if (pList != NULL)
	{
		pList->SelectString(-1, m_csLastFieldName);
	}

	UpdateButtons();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInsertFieldDialog::BuildList(int nCurSel /*=-1*/)
{
	CListBox* pList = (CListBox*)GetDlgItem(IDC_FIELD_LIST);
	if (pList != NULL)
	{
		pList->ResetContent();

		pList->SetRedraw(FALSE);

		int nMacros = m_MacroList.Macros();
		for (int nMacro = 0; nMacro < nMacros; nMacro++)
		{
			CMacro* pMacro = m_MacroList.Macro(nMacro);

			// Check if the caller only wants "Merge Fields".
			if (m_fMergeFieldsOnly)
			{
				if ((pMacro->MacroType() != CMacro::MACRO_TYPE_Receiver)
				 && (pMacro->MacroType() != CMacro::MACRO_TYPE_Sender)
				 && (pMacro->MacroType() != CMacro::MACRO_TYPE_Global))
				{
					pMacro = NULL;
				}
			}

			if (pMacro != NULL)
			{
			/* Add this field to the list. */
				int nItem = pList->AddString(pMacro->Name());
				if (nItem >= 0)
				{
				/* Remember a pointer to the data for easy access later. */
					pList->SetItemData(nItem, (DWORD)pMacro);
				}
			}
		}

		if (nCurSel != -1)
		{
			pList->SetCurSel(nCurSel);
			UpdateButtons();
		}
		
		pList->SetRedraw(TRUE);
	}
}

void CInsertFieldDialog::UpdateButtons(void)
{
	BOOL fCanRemove = FALSE;
	BOOL fCanRename = FALSE;
	BOOL fCanInsert = FALSE;

	CListBox* pList = (CListBox*)GetDlgItem(IDC_FIELD_LIST);
	if (pList != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel >= 0)
		{
			fCanInsert = TRUE;
			CMacro* pMacro = (CMacro*)pList->GetItemData(nCurSel);
			if (pMacro != NULL)
			{
				if (pMacro->MacroType() == CMacro::MACRO_TYPE_User
						&& !(pMacro->Flags() & MACRO_FLAG_PermanentName))
				{
					fCanRename = TRUE;
					fCanRemove = TRUE;
				}
			}
		}
	}
	EnableDlgItem(IDC_INSERT_FIELD, fCanInsert);
	EnableDlgItem(IDC_REMOVE_FIELD, fCanRemove);
	EnableDlgItem(IDC_RENAME_FIELD, fCanRename);
}

BOOL CInsertFieldDialog::MacroInUse(CMacro* pMacro)
{
/* Look for the macro in the list. */

	WORD wMacro = pMacro->Id() + MACRO_CHARACTER;

	int nMacrosInUse = m_cwaMacrosInUse.GetSize();
	for (int i = 0; i < nMacrosInUse; i++)
	{
		if (m_cwaMacrosInUse[i] == wMacro)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CInsertFieldDialog::OnSelchangeFieldList()
{
	UpdateButtons();
}

void CInsertFieldDialog::OnDblclkFieldList()
{
	OnInsertField();
}

void CInsertFieldDialog::UpdateFields(void)
{
	if (m_fChangedMacroList)
	{
		// Macro List assignment.
		m_pHelper->MacroList() = m_MacroList;
		m_pHelper->ReflowMacros();
		m_fChangedMacroList = FALSE;
	}
}

void CInsertFieldDialog::OnInsertField()
{
	UpdateFields();

	CListBox* pList = (CListBox*)GetDlgItem(IDC_FIELD_LIST);
	if (pList != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel >= 0)
		{
			m_pHelper->InsertMacro(nCurSel);
		}
	}

//	OnOK();
}

void CInsertFieldDialog::InsertBlock(LPCSTR* pStrings)
{
#ifdef LOCALIZE
	ASSERT(pStrings);
	if (!pStrings) return;
#endif
	
	int nMacros = m_MacroList.Macros();

	while (*pStrings != NULL)
	{
		LPCSTR pString = *pStrings++;

	/* Look for a macro matching this string. */
		for (int nMacro = 0; nMacro < nMacros; nMacro++)
		{
			CMacro* pMacro = m_MacroList.Macro(nMacro);
			if (pMacro != NULL && pMacro->Name() == pString)
			{
				m_pHelper->InsertMacro(nMacro);
				break;
			}
		}

		if (nMacro >= nMacros)
		{
		/* Insert the string character by character. */
			while (*pString != '\0')
			{
				CHARACTER c = (CHARACTER)*pString++;
				m_pHelper->InsertText(&c, 1);
			}
		}
	}
}


#ifndef LOCALIZE
static LPCSTR
ReceiverStrings[] =
{
	"First Name", " ", "Last Name", "\n",
	"Title", "\n",
	"Company", "\n",
	"Address1", "\n",
	"Address2", "\n",
	"City", ", ", "State", " ", "Zip",
	NULL
};

static LPCSTR
SenderStrings[] =
{
	"Sender First Name", " ", "Sender Last Name", "\n",
	"Sender Title", "\n",
	"Sender Company", "\n",
	"Sender Address1", "\n",
	"Sender Address2", "\n",
	"Sender City", ", ", "Sender State", " ", "Sender Zip",
	NULL
};
#endif // ifndef LOCALIZE

void CInsertFieldDialog::OnInsertReceiver()
{
	if (GetConfiguration()->SupportsAddressBook())
	{
		UpdateFields();
#ifdef LOCALIZE
		CString csAddressFormat;
		csAddressFormat.LoadString(IDS_RECEIVER_ADDRESS_FORMAT);
		CString csTemp = csAddressFormat;
		LPSTR lpszTemp = csTemp.GetBuffer(csTemp.GetLength()+1);
		InsertBlock((LPCSTR *) ParseAddressFormat(lpszTemp));
		csTemp.ReleaseBuffer();
#else
		InsertBlock(ReceiverStrings);
#endif // LOCALIZE
//		OnOK();
	}
}

void CInsertFieldDialog::OnInsertSender()
{
	if (GetConfiguration()->SupportsAddressBook())
	{
		UpdateFields();
#ifdef LOCALIZE
		CString csAddressFormat;
		csAddressFormat.LoadString(IDS_SENDER_ADDRESS_FORMAT);
		CString csTemp = csAddressFormat;
		LPSTR lpszTemp = csTemp.GetBuffer(csTemp.GetLength()+1);
		InsertBlock((LPCSTR *) ParseAddressFormat(lpszTemp));
		csTemp.ReleaseBuffer();
#else
		InsertBlock(SenderStrings);
#endif // LOCALIZE
//		OnOK();
	}
}

void CInsertFieldDialog::OnAddField()
{
	CFieldNameDialog Dialog(IDD_FIELD_ADD, this);

	if (Dialog.DoModal() == IDOK)
	{
		if (MacroNameIsUnique(Dialog.m_csName))
		{
			CMacro* pMacro = m_MacroList.AddMacro(Dialog.m_csName, "");
			if (pMacro != NULL)
			{
				pMacro->MacroType((WORD)CMacro::MACRO_TYPE_User);
				pMacro->MacroValueType((WORD)CMacro::VALUE_TYPE_String);
				pMacro->FieldClass("Standard");

				m_fChangedMacroList = TRUE;

				BuildList(m_MacroList.Macros()-1);
			}
		}
	}
}

void CInsertFieldDialog::OnRemoveField()
{
	CListBox* pList = (CListBox*)GetDlgItem(IDC_FIELD_LIST);
	if (pList != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel >= 0)
		{
			CMacro* pMacro = (CMacro*)pList->GetItemData(nCurSel);
			if (pMacro != NULL)
			{
				if (MacroInUse(pMacro))
				{
					AfxMessageBox(IDS_FIELD_IN_USE, MB_OK | MB_ICONEXCLAMATION);
				}
				else
				{
				// Delete the macro from the list.
					m_MacroList.DeleteMacro(nCurSel);

					m_fChangedMacroList = TRUE;

				// Figure out where we want the focus to go.
					if (nCurSel == pList->GetCount()-1)
					{
						nCurSel--;
					}

				// Build the new list.
					BuildList(nCurSel);
				}
			}
		}
	}
}

void CInsertFieldDialog::OnRenameField()
{
	CListBox* pList = (CListBox*)GetDlgItem(IDC_FIELD_LIST);
	if (pList != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel >= 0)
		{
			CMacro* pMacro = (CMacro*)pList->GetItemData(nCurSel);
			if (pMacro != NULL)
			{
				CFieldNameDialog Dialog(IDD_FIELD_RENAME, this);

				Dialog.m_csName = pMacro->Name();

				if (Dialog.DoModal() == IDOK)
				{
					if (MacroNameIsUnique(Dialog.m_csName, pMacro))
					{
						pMacro->Name(Dialog.m_csName);

						m_fChangedMacroList = TRUE;

					// Do something gross here.

						BuildList(nCurSel);
					}
				}
			}
		}
	}
}

int CInsertFieldDialog::FindMacro(LPCSTR pName, CMacro* pSkipMacro /*=NULL*/)
{
	int nMacros = m_MacroList.Macros();
	for (int nMacro = 0; nMacro < nMacros; nMacro++)
	{
		CMacro* pMacro = m_MacroList.Macro(nMacro);
		if (pMacro != NULL && pMacro != pSkipMacro)
		{
			if (pMacro->Name() == pName)
			{
				return nMacro;
			}
		}
	}
	return -1;
}

BOOL CInsertFieldDialog::MacroNameIsUnique(LPCSTR pName, CMacro* pSkipMacro /*=NULL*/)
{
	int nMacro = FindMacro(pName, pSkipMacro);

	if (nMacro == -1)
	{
		return TRUE;
	}
	else
	{
		AfxMessageBox(IDS_FIELD_EXISTS, MB_OK | MB_ICONEXCLAMATION);

	/* Set the current selection to the one that has been found. */

		CListBox* pList = (CListBox*)GetDlgItem(IDC_FIELD_LIST);
		if (pList != NULL)
		{
			pList->SetCurSel(nMacro);
			UpdateButtons();
		}
		return FALSE;
	}
}

void CInsertFieldDialog::OnOK()
{
	UpdateFields();
	
	CListBox* pList = (CListBox*)GetDlgItem(IDC_FIELD_LIST);
	if (pList != NULL)
	{
		int nIndex = pList->GetCurSel();
		if (nIndex >= 0)
		{
			pList->GetText(nIndex, m_csLastFieldName);
		}
		else
		{
			m_csLastFieldName.Empty();
		}
	}
		
	CPmwDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CFieldNameDialog dialog

CFieldNameDialog::CFieldNameDialog(WORD wIDD, CWnd* pParent /*=NULL*/)
	: CPmwDialog(wIDD, pParent)
{
	//{{AFX_DATA_INIT(CFieldNameDialog)
	m_csName = "";
	//}}AFX_DATA_INIT
}

void CFieldNameDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFieldNameDialog)
	DDX_Text(pDX, IDC_FIELD_NAME, m_csName);
	DDV_MaxChars(pDX, m_csName, 40);
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
	{
		ValidateFieldName(m_csName);
	}
}

BEGIN_MESSAGE_MAP(CFieldNameDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CFieldNameDialog)
	ON_EN_CHANGE(IDC_FIELD_NAME, OnChangeFieldName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFieldNameDialog::UpdateButtons(void)
{
	CString csName;
	
	TRY
	{
		CWnd* pFieldName = GetDlgItem(IDC_FIELD_NAME);
		if (pFieldName != NULL)
		{
			pFieldName->GetWindowText(csName);
			
			CButton* pButton = (CButton*)GetDlgItem(IDOK);
			if (pButton != NULL)
			{
				pButton->EnableWindow(ValidateFieldName(csName));
			}
		}
	}
	END_TRY
}

BOOL CFieldNameDialog::ValidateFieldName(CString& csName)
{
  	return Util::Trim(csName);
}



/////////////////////////////////////////////////////////////////////////////
// CFieldNameDialog message handlers

BOOL CFieldNameDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
	UpdateButtons();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFieldNameDialog::OnChangeFieldName()
{
	UpdateButtons();
}
