/*
// $Header: /PM8/App/FILLNDLG.CPP 1     3/03/99 6:05p Gbeddow $
//
// fillndlg.cpp : implementation file
//
// $Log: /PM8/App/FILLNDLG.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:33:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:30   FRED
// Initial revision.
// 
//    Rev 1.4   02 Aug 1995 08:08:16   FRED
// Fill-in fields stuff.
// 
//    Rev 1.3   31 Jul 1995 07:28:02   FRED
// Support for editing sender.
// 
//    Rev 1.2   26 Jul 1995 10:52:20   FRED
// New Address Book interface
// 
//    Rev 1.1   11 Jul 1995 14:19:24   JAY
// Fill-in fields dialog.
// 
//    Rev 1.0   11 Jul 1995 09:42:22   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "fillndlg.h"
#include "dlgmgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFillInDialog dialog


CFillInDialog::CFillInDialog(CPmwView* pParent /*=NULL*/)
	: CSmartFieldsDialog(CFillInDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFillInDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pView = pParent;
}

void CFillInDialog::DoDataExchange(CDataExchange* pDX)
{
	CSmartFieldsDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFillInDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFillInDialog, CSmartFieldsDialog)
	//{{AFX_MSG_MAP(CFillInDialog)
	ON_BN_CLICKED(IDC_RECEIVERS, OnReceivers)
	ON_BN_CLICKED(IDC_SENDER, OnSender)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFillInDialog message handlers

BOOL CFillInDialog::OnInitDialog()
{
	BOOL fRet = CSmartFieldsDialog::OnInitDialog();

/*
// Bind this dialog to a view so we can see the document and the macros.
*/

	SetView(m_pView);

/*
// See if there are any macros in use.
// If not, then we're not needed; so, we bail here.
*/

	if (m_cwaMacrosInUse.GetSize() == 0)
	{
		EndDialog(IDCANCEL);
	}

	return FALSE;
}

BOOL CFillInDialog::CanBeginEdit(int nItem, int nField)
{
	return (nField == PROPERTY_ValueField);
}

CSmartFieldListBox* CFillInDialog::CreateListBox(void)
{
	return new CFillInFieldListBox;
}

/////////////////////////////////////////////////////////////////////////////
// CFillInFieldListBox

CFillInFieldListBox::CFillInFieldListBox()
{
}

BOOL CFillInFieldListBox::OnFinishEdit(BOOL fValidate /*=TRUE*/, BOOL fByReturn /*=FALSE*/)
{
	if (INHERITED::OnFinishEdit(fValidate, fByReturn))
	{
		if (fByReturn)
		{
			int nCurSel = GetCurSel();
			int nCount = GetCount();

			if (nCurSel != nCount-1)
			{
				BeginEdit(nCurSel+1);
			}
			TRACE(">>> Advance to next field! <<<\n");
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CFillInFieldListBox::CanEditItem(int nItem, int nField)
{
	return (nField == PROPERTY_ValueField);
}

BOOL CFillInFieldListBox::ShowMacro(CMacro* pMacro)
{
	if (INHERITED::ShowMacro(pMacro) && m_pInUse != NULL)
	{
	/* Look for the macro in the list. */

		WORD wMacro = pMacro->Id() + MACRO_CHARACTER;

		int nMacrosInUse = m_pInUse->GetSize();
		for (int i = 0; i < nMacrosInUse; i++)
		{
			if ((*m_pInUse)[i] == wMacro)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

