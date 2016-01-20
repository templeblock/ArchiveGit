/*
// $Workfile: PAGEDLGS.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PAGEDLGS.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 2     3/10/98 5:24p Jayn
// Master page changes
// 
//    Rev 1.0   14 Aug 1997 15:23:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:32   Fred
// Initial revision.
// 
//    Rev 1.3   31 Jul 1996 11:57:00   Fred
// 16-bit compatibility
// 
//    Rev 1.2   26 Jul 1996 11:39:24   Jay
// Page stuff
// 
//    Rev 1.1   25 Jul 1996 19:38:42   Jay
//  
*/

#include "stdafx.h"
#include "pmw.h"
#include "util.h"
#include "pmwdoc.h"
#include "pagedlgs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageBaseDialog dialog

CPageBaseDialog::CPageBaseDialog(CPmwDoc* pDocument, UINT uID, CWnd* pParent /*=NULL*/)
	: CPmwDialog(uID, pParent)
{
	m_pDocument = pDocument;
}

void CPageBaseDialog::InvalidPageNumber(void)
{
	CString csFormat;
	CString csMessage;
	TRY
	{
		csFormat.LoadString(IDS_INVALID_PAGE);
		csMessage.Format((LPCSTR)csFormat, 1, m_pDocument->NumberOfPages());
		AfxMessageBox(csMessage);
	}
	END_TRY
}

void CPageBaseDialog::LimitEditText(UINT uID, int nLimit)
{
	CEdit* pPages = (CEdit*)GetDlgItem(uID);
	if (pPages != NULL)
	{
		pPages->LimitText(nLimit);
	}
}

BEGIN_MESSAGE_MAP(CPageBaseDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CPageBaseDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddPageDialog dialog

CAddPageDialog::CAddPageDialog(CPmwDoc* pDocument, CWnd* pParent /*=NULL*/, int nWhere /*=AfterCurrentPage*/, int nPages /*=1*/, int nContents /*=AddBlankPages*/)
	: CPageBaseDialog(pDocument, CAddPageDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddPageDialog)
	m_nWhere = nWhere;
	m_csPages = _T("");
	m_nContents = nContents;
	m_csCopyPage = _T("");
	//}}AFX_DATA_INIT

	TRY
	{
		m_csPages.Format("%d", nPages);
		m_csCopyPageSave.Format("%ld", pDocument->CurrentPageIndex()+1);
		if (m_nContents == AddPageCopies)
		{
			m_csCopyPage = m_csCopyPageSave;
		}
	}
	END_TRY
}

void CAddPageDialog::DoDataExchange(CDataExchange* pDX)
{
	CPageBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddPageDialog)
	DDX_Radio(pDX, IDC_ADD_BEFORE_CURRENT_PAGE, m_nWhere);
	DDX_Text(pDX, IDC_NUMBER_OF_PAGES, m_csPages);
	DDX_Radio(pDX, IDC_ADD_BLANK, m_nContents);
	DDX_Text(pDX, IDC_ADD_COPY_PAGE, m_csCopyPage);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		long lPages = atol(m_csPages);
		if (lPages <= 0 || lPages > 99)
		{
			AfxMessageBox(IDS_ADD_TOO_MANY_PAGES);
			pDX->PrepareEditCtrl(IDC_NUMBER_OF_PAGES);
			AfxThrowUserException();
		}

		if (m_nContents == AddPageCopies)
		{
			long lCopyPage = atol(m_csCopyPage);
			if (lCopyPage < 1 || lCopyPage > (long)m_pDocument->NumberOfPages())
			{
				// Complain.
				InvalidPageNumber();
				// Try again.
				pDX->PrepareEditCtrl(IDC_ADD_COPY_PAGE);
				AfxThrowUserException();
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CAddPageDialog, CPageBaseDialog)
	//{{AFX_MSG_MAP(CAddPageDialog)
	ON_BN_CLICKED(IDC_ADD_BLANK, OnAddBlank)
	ON_BN_CLICKED(IDC_ADD_COPY, OnAddCopy)
	ON_EN_CHANGE(IDC_ADD_COPY_PAGE, OnChangeAddCopyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddPageDialog message handlers

BOOL CAddPageDialog::OnInitDialog() 
{
	CPageBaseDialog::OnInitDialog();

	LimitEditText(IDC_NUMBER_OF_PAGES, 6);

	return TRUE;
}

void CAddPageDialog::OnAddBlank() 
{
	// Switched to add blank.
	CString csText;
	TRY
	{
		Util::GetDlgItemText(this, IDC_ADD_COPY_PAGE, csText);
		if (!csText.IsEmpty())
		{
			m_csCopyPageSave = csText;
		}
	}
	END_TRY
	SetDlgItemText(IDC_ADD_COPY_PAGE, "");
}

void CAddPageDialog::OnAddCopy() 
{
	SetDlgItemText(IDC_ADD_COPY_PAGE, m_csCopyPageSave);
}

void CAddPageDialog::OnChangeAddCopyPage() 
{
	if (GetFocus() == this)
	{
		CheckDlgButton(IDC_ADD_COPY, TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDeletePageDialog dialog


CDeletePageDialog::CDeletePageDialog(CPmwDoc* pDocument, CWnd* pParent /*=NULL*/)
	: CPageBaseDialog(pDocument, CDeletePageDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeletePageDialog)
	m_csFrom = _T("");
	m_csTo = _T("");
	//}}AFX_DATA_INIT

	TRY
	{
		m_csFrom.Format("%ld", pDocument->CurrentPageIndex()+1);
		m_csTo = m_csFrom;
	}
	END_TRY
}

void CDeletePageDialog::DoDataExchange(CDataExchange* pDX)
{
	CPageBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeletePageDialog)
	DDX_Text(pDX, IDC_DELETE_FROM, m_csFrom);
	DDX_Text(pDX, IDC_DELETE_TO, m_csTo);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		long lFromPage = atol(m_csFrom);
		long lToPage = atol(m_csTo);
		UINT uBadID = 0;

		if (lFromPage == 1 && lToPage == (long)m_pDocument->NumberOfPages())
		{
			AfxMessageBox(IDS_CANT_DELETE_ALL_PAGES);
			uBadID = IDC_DELETE_FROM;
		}
		else if (lFromPage > lToPage)
		{
			AfxMessageBox(IDS_FROM_GREATER_THAN_TO);
			uBadID = IDC_DELETE_FROM;
		}
		else if (lToPage < 1 || lToPage > (long)m_pDocument->NumberOfPages())
		{
			InvalidPageNumber();
			uBadID = IDC_DELETE_TO;
		}
		else if (lFromPage < 1 || lFromPage > (long)m_pDocument->NumberOfPages())
		{
			InvalidPageNumber();
			uBadID = IDC_DELETE_FROM;
		}

		if (uBadID != 0)
		{
			pDX->PrepareEditCtrl(uBadID);
			AfxThrowUserException();
		}
	}
}

BEGIN_MESSAGE_MAP(CDeletePageDialog, CPageBaseDialog)
	//{{AFX_MSG_MAP(CDeletePageDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeletePageDialog message handlers

BOOL CDeletePageDialog::OnInitDialog() 
{
	CPageBaseDialog::OnInitDialog();

	LimitEditText(IDC_DELETE_FROM, 6);
	LimitEditText(IDC_DELETE_TO, 6);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGotoPageDialog dialog

CGotoPageDialog::CGotoPageDialog(CPmwDoc* pDocument, CWnd* pParent /*=NULL*/)
	: CPageBaseDialog(pDocument, CGotoPageDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGotoPageDialog)
	m_csPage = _T("");
	//}}AFX_DATA_INIT
	TRY
	{
		m_csPage.Format("%ld", pDocument->CurrentPageIndex()+1);
	}
	END_TRY
}

void CGotoPageDialog::DoDataExchange(CDataExchange* pDX)
{
	CPageBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGotoPageDialog)
	DDX_Text(pDX, IDC_GOTO_PAGE, m_csPage);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		long lPage = atol(m_csPage);
		if (lPage < 1 || lPage > (long)m_pDocument->NumberOfPages())
		{
			// Complain.
			InvalidPageNumber();
			// Try again.
			pDX->PrepareEditCtrl(IDC_GOTO_PAGE);
			AfxThrowUserException();
		}
	}
}


BEGIN_MESSAGE_MAP(CGotoPageDialog, CPageBaseDialog)
	//{{AFX_MSG_MAP(CGotoPageDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGotoPageDialog message handlers

BOOL CGotoPageDialog::OnInitDialog() 
{
	CPageBaseDialog::OnInitDialog();

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_GOTO_PAGE);
	pEdit->LimitText(6);
	pEdit->SetFocus();
	pEdit->SetSel(0, -1);

	return FALSE;
}

