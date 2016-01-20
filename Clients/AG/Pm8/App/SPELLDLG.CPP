/*
// $Workfile: SPELLDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SPELLDLG.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:32   Fred
// Initial revision.
// 
//    Rev 1.4   01 Aug 1996 18:20:24   Jay
// Spell check highlight.
// 
//    Rev 1.3   01 Aug 1996 14:40:44   Jay
// Spelling fixes
// 
//    Rev 1.2   01 Aug 1996 09:23:48   Fred
// More 16-bit changes
// 
//    Rev 1.1   31 Jul 1996 18:22:02   Jay
// Spelling, etc.
// 
//    Rev 1.0   28 Jul 1996 15:09:00   Jay
//  
*/

#include "stdafx.h"
#include "pmw.h"
#include "Spelldlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char cbSpellWindow[] = "SpellWindow";

/////////////////////////////////////////////////////////////////////////////
// CSpellingDialog dialog

CSpellingDialog::CSpellingDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CSpellingDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpellingDialog)
	m_fIgnoreNumbers = FALSE;
	m_fIgnoreUpperCase = FALSE;
	m_csChangeTo = _T("");
	m_csWord = _T("");
	//}}AFX_DATA_INIT
	m_pSuggestions = NULL;
	m_fInnerSet = FALSE;
	m_fChangedChangeTo = FALSE;
	m_nMaxWordSize = 0;
}

void CSpellingDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpellingDialog)
	DDX_Check(pDX, IDC_IGNORE_NUMBERS, m_fIgnoreNumbers);
	DDX_Check(pDX, IDC_IGNORE_UPPERCASE, m_fIgnoreUpperCase);
	DDX_Text(pDX, IDC_CHANGE_TO, m_csChangeTo);
	DDX_Text(pDX, IDC_WORD, m_csWord);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		if (m_fValidateChangeTo)
		{
			// What do we need to validate?
		}
	}
}

BEGIN_MESSAGE_MAP(CSpellingDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CSpellingDialog)
	ON_BN_CLICKED(IDC_IGNORE, OnIgnore)
	ON_BN_CLICKED(IDC_IGNORE_ALL, OnIgnoreAll)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_BN_CLICKED(IDC_CHANGE_ALL, OnChangeAll)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_LBN_SELCHANGE(IDC_SUGGESTIONS, OnSelchangeSuggestions)
	ON_EN_CHANGE(IDC_CHANGE_TO, OnChangeChangeTo)
	ON_WM_MOUSEACTIVATE()
	ON_LBN_DBLCLK(IDC_SUGGESTIONS, OnDblclkSuggestions)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpellingDialog message handlers

void CSpellingDialog::OnExit(UINT uID, BOOL fValidateChangeTo)
{
	m_fValidateChangeTo = fValidateChangeTo;
	UpdateData(TRUE);
	EndDialog(uID);
}

void CSpellingDialog::OnIgnore() 
{
	OnExit(IDC_IGNORE, FALSE);
}

void CSpellingDialog::OnIgnoreAll() 
{
	OnExit(IDC_IGNORE_ALL, FALSE);
}

void CSpellingDialog::OnChange() 
{
	OnExit(IDC_CHANGE, TRUE);
}

void CSpellingDialog::OnChangeAll() 
{
	OnExit(IDC_CHANGE_ALL, TRUE);
}

void CSpellingDialog::OnAdd() 
{
	OnExit(IDC_ADD, FALSE);
}

void CSpellingDialog::OnClose() 
{
	OnExit(IDC_CLOSE, FALSE);
}

void CSpellingDialog::OnSelchangeSuggestions() 
{
	// Stick the selected word in the "change to" edit.

	CListBox* pList = (CListBox*)GetDlgItem(IDC_SUGGESTIONS);
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_CHANGE_TO);
	CString csText;

	if (pEdit != NULL)
	{
		if (pList != NULL)
		{
			TRY
			{
				int nCurSel = pList->GetCurSel();
				if (nCurSel >= 0)
				{
					pList->GetText(nCurSel, csText);
				}
			}
			END_TRY
		}
		// Set the text.
		BOOL fInnerSet = m_fInnerSet;
		m_fInnerSet = TRUE;
		pEdit->SetWindowText(csText);
		m_fInnerSet = fInnerSet;
	}
}

void CSpellingDialog::OnChangeChangeTo() 
{
	if (!m_fInnerSet)
	{
		CListBox* pList = (CListBox*)GetDlgItem(IDC_SUGGESTIONS);
		if (pList != NULL)
		{
			// Custom typing. Do not select anything in the list.
			pList->SetCurSel(-1);
		}
	}

	m_fChangedChangeTo = TRUE;

	UpdateButtons();
}

BOOL CSpellingDialog::OnInitDialog() 
{
	BOOL fResult = TRUE;

	CPmwDialog::OnInitDialog();

	GET_PMWAPP()->LoadWindowPosition(this, cbSpellWindow, SW_SHOWNORMAL, TRUE, TRUE);

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_CHANGE_TO);
	CListBox* pList = (CListBox*)GetDlgItem(IDC_SUGGESTIONS);

	// Set the edit size.
	ASSERT(m_nMaxWordSize > 0);
	if (m_nMaxWordSize > 0)
	{
		if (pEdit != NULL)
		{
			pEdit->LimitText(m_nMaxWordSize);
		}
	}

	// Stick the suggestions in the array.
	if (pList != NULL)
	{
		pList->SetRedraw(FALSE);
		pList->ResetContent();
		if (m_pSuggestions != NULL)
		{
			for (LPCSTR p = m_pSuggestions; *p != '\0'; p += strlen(p)+1)
			{
				pList->AddString(p);
			}
		}
		pList->SetRedraw(TRUE);
	}

	if (pEdit != NULL)
	{
#if 0
		if (pList != NULL && pList->GetCount() > 0)
		{
			pList->SetCurSel(0);
			OnSelchangeSuggestions();
		}
#endif

		// Always set focus to edit control.
		pEdit->SetFocus();
		pEdit->SetSel(0, -1);
		fResult = FALSE;
	}

	UpdateButtons();

	return fResult;
}

int CSpellingDialog::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	POINT p;

	GetCursorPos(&p);
	CWnd* pWnd = GetDlgItem(IDC_WORD);
	if (pWnd != NULL)
	{
		CRect crBound;
		pWnd->GetWindowRect(crBound);
		if (crBound.PtInRect(p))
		{
			return MA_ACTIVATEANDEAT;
		}
	}

	return CPmwDialog::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CSpellingDialog::OnDblclkSuggestions() 
{
	OnChange();
}

void CSpellingDialog::UpdateButtons(void)
{
	BOOL fEnable = FALSE;
	CWnd* pEdit = GetDlgItem(IDC_CHANGE_TO);
	CString csText;
	TRY
	{
		if (pEdit != NULL)
		{
			pEdit->GetWindowText(csText);
			fEnable = !csText.IsEmpty();
		}
	}
	END_TRY

	fEnable = fEnable && m_fChangedChangeTo;

	if (!fEnable)
	{
		SetDefID(IDC_IGNORE);
	}

	CWnd* pButton;
	if ((pButton = GetDlgItem(IDC_CHANGE)) != NULL)
	{
		pButton->EnableWindow(fEnable);
	}
	if ((pButton = GetDlgItem(IDC_CHANGE_ALL)) != NULL)
	{
		pButton->EnableWindow(fEnable);
	}

	if (fEnable)
	{
		SetDefID(IDC_CHANGE);
	}
}

void CSpellingDialog::OnDestroy() 
{
	GET_PMWAPP()->SaveWindowPosition(this, cbSpellWindow);
	CPmwDialog::OnDestroy();
}
