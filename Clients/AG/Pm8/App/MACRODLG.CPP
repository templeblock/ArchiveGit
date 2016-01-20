/*
// macrodlg.cpp : implementation file
//
// $Workfile: MACRODLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/MACRODLG.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:06   Fred
// Initial revision.
// 
//    Rev 1.4   07 Jul 1997 16:07:46   Jay
// Better handling of saved state.
// 
//    Rev 1.3   28 Mar 1997 17:10:46   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.2   13 Mar 1997 13:17:08   Jay
// Changes for 16-bit and 5.0 builds
// 
//    Rev 1.1   16 Jul 1996 17:30:38   Fred
// More address/sender book stuff
// 
//    Rev 1.0   14 Mar 1996 13:32:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:18   FRED
// Initial revision.
// 
//    Rev 1.36   18 Jan 1996 17:10:18   JAY
// New project configuration class
// 
//    Rev 1.35   15 Jan 1996 10:26:22   JAY
//  
// 
//    Rev 1.34   09 Nov 1995 16:59:56   JAY
// Restores document modified state on Cancel.
// 
//    Rev 1.33   03 Sep 1995 15:54:48   FRED
//  
// 
//    Rev 1.32   31 Aug 1995 10:04:14   FRED
// Miscellaneous fixes.
// 
//    Rev 1.31   27 Aug 1995 14:30:56   FRED
// Some menu/dialog improvements
// 
//    Rev 1.30   27 Aug 1995 12:34:14   FRED
// New fill-in-fields stuff
// 
//    Rev 1.29   22 Aug 1995 19:21:36   FRED
//  
// 
//    Rev 1.28   11 Aug 1995 12:51:30   FRED
//  
// 
//    Rev 1.27   11 Aug 1995 10:58:56   FRED
// Phil Frank page; macro size limits; move project album text
// 
//    Rev 1.26   10 Aug 1995 16:30:48   FRED
// Limits field value to 100 characters.
// 
// 
//    Rev 1.25   09 Aug 1995 13:11:22   JAY
// Misc changes.
// 
//    Rev 1.24   06 Aug 1995 16:33:10   FRED
// More Fill-in Fields dialog stuff.
// 
//    Rev 1.23   06 Aug 1995 09:45:30   FRED
// More fill-in fields stuff.
// 
//    Rev 1.22   04 Aug 1995 10:47:06   FRED
// Sender fields.
// 
//    Rev 1.21   03 Aug 1995 14:33:12   FRED
//  
// 
//    Rev 1.20   03 Aug 1995 08:00:36   FRED
// Fill in stuff.
// 
//    Rev 1.19   02 Aug 1995 14:56:26   JAY
//  
// 
//    Rev 1.18   02 Aug 1995 12:58:14   FRED
// Some more fill-in fields stuff.
// 
//    Rev 1.17   02 Aug 1995 08:07:14   FRED
// Fill-in fields stuff.
// 
//    Rev 1.16   12 Jul 1995 17:45:08   JAY
// New project sub-types (esp. for cards).
// 
//    Rev 1.15   11 Jul 1995 14:19:24   JAY
// Fill-in fields dialog.
// 
//    Rev 1.14   11 Jul 1995 09:41:42   JAY
// Initial fill-ins dialog for doc.
// 
//    Rev 1.13   06 Jul 1995 14:47:56   JAY
// Load and Save WindowPosition now in app.
// 
//    Rev 1.12   09 Jun 1995 07:44:42   FRED
// Sound Manager changes
// 
//    Rev 1.11   03 May 1995 09:40:32   JAY
// Window position remembering code
// 
//    Rev 1.10   02 May 1995 17:09:16   JAY
// Fixed some smart fields bugs.
// 
//    Rev 1.9   02 May 1995 15:39:40   JAY
// More smart fields stuff.
// 
//    Rev 1.8   01 May 1995 15:27:52   JAY
// Restructured the macro stuff
// 
//    Rev 1.7   28 Apr 1995 17:26:54   JAY
// Got rid of list double click for insert.
// 
//    Rev 1.6   28 Apr 1995 16:11:36   JAY
// Validation, better kill focus support, more stability, etc.
// 
//    Rev 1.5   26 Apr 1995 14:41:28   JAY
//  
// 
//    Rev 1.4   25 Apr 1995 15:24:42   JAY
//  
// 
//    Rev 1.3   25 Apr 1995 09:02:50   JAY
// More refinements.
// 
//    Rev 1.2   19 Apr 1995 13:24:18   JAY
// Now sets the size of the list box items.
// 
//    Rev 1.1   19 Apr 1995 12:34:22   JAY
//  
// 
//    Rev 1.0   29 Mar 1995 09:18:42   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "mainfrm.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "macrodlg.h"
#include "macrostd.h"
#include "nofillin.h"
#include "pmwcfg.h"

#include <afxpriv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static const char BASED_CODE SECTION_SmartFields[] = "SmartFields";

/////////////////////////////////////////////////////////////////////////////
// CPmwView support for smart fields (macro) dialog.

void CPmwView::OnViewSmartFields()
{
	CSmartFieldsDialog Dialog(this);

	Dialog.DoModal();
}

void CPmwView::OnStartupSmartFields()
{
	CSmartFieldsDialog Dialog(this, NULL, TRUE);

	Dialog.DoModal();
}

void CPmwView::OnUpdateViewSmartFields(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

/////////////////////////////////////////////////////////////////////////////
// CSmartFieldListBoxItem

CSmartFieldListBoxItem::CSmartFieldListBoxItem(CMacro* pMacro, int nIndex, CStdMacroServer* pMacroServer, BOOL fNew /*=FALSE*/)
{
	m_pMacro = pMacro;
	m_nIndex = nIndex;
	m_pMacroServer = pMacroServer;
	m_fNew = fNew;
}

/* Name access. */

CString& CSmartFieldListBoxItem::GetFieldText(int nField, CString& csField)
{
	if (IsMacroValid())
	{
		switch (nField)
		{
			case PROPERTY_NameField:
			{
				csField = m_pMacro->Name();
				break;
			}
			case PROPERTY_ValueField:
			{
				if (m_pMacro->MacroValueType() == CMacro::VALUE_TYPE_Field)
				{
					csField = m_pMacroServer->GetMacroTextString(m_pMacro);
				}
				else
				{
					csField = m_pMacro->Value();
				}

				break;
			}
			default:
			{
				csField.Empty();
				break;
			}
		}
	}
	return csField;
}

void CSmartFieldListBoxItem::SetFieldText(int nField, LPCSTR pNewText)
{
	if (IsMacroValid())
	{
		switch (nField)
		{
			case PROPERTY_NameField:
			{
				m_pMacro->Name(pNewText);
				break;
			}
			case PROPERTY_ValueField:
			{

			/* Assign it as a string literal. */

				m_pMacro->MacroValueType(CMacro::VALUE_TYPE_String);
				m_pMacro->Value(pNewText);
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

BOOL CSmartFieldListBoxItem::CanEditField(int nField)
{
	return TRUE;
}

BOOL CSmartFieldListBoxItem::IsMacroValid(void)
{
	BOOL fResult = FALSE;

	if (m_pMacro != NULL)
	{
		if (m_pMacroServer != NULL)
		{
			CMacroList& MacroList = m_pMacroServer->MacroList();
			if ((m_nIndex >= 0) && (m_nIndex < MacroList.Macros()))
			{
				if (MacroList.Macro(m_nIndex) == m_pMacro)
				{
					fResult = TRUE;
				}
			}
		}
	}

	if (!fResult)
	{
		m_pMacro = NULL;
	}

	return fResult;
}

void CSmartFieldListBoxItem::PaintField(int nField, CPropertyListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	INHERITED::PaintField(nField, pList, pDC, pDrawItemStruct);
}

void CSmartFieldListBoxItem::PaintGrid(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	INHERITED::PaintGrid(pList, pDC, pDrawItemStruct);
}

void CSmartFieldListBoxItem::ComputeColors(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF& clForeground, COLORREF& clBackground)
{
	clForeground = GetSysColor(COLOR_WINDOWTEXT);
	clBackground = GetSysColor(COLOR_WINDOW);
}

void CSmartFieldListBoxItem::ComputeBounds(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBound)
{
	INHERITED::ComputeBounds(nField, pList, pDC, pDrawItemStruct, crBound);
}

void CSmartFieldListBoxItem::PaintBackground(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crField, COLORREF clBackground)
{
	INHERITED::PaintBackground(nField, pList, pDC, pDrawItemStruct, crField, clBackground);
}

void CSmartFieldListBoxItem::PaintItem(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	switch (nField)
	{
		case PROPERTY_NameField:
		{
			INHERITED::PaintItem(nField, pList, pDC, pDrawItemStruct, crBounds, clForeground, clBackground);
			break;
		}
		case PROPERTY_ValueField:
		{
			CString csField;
			PaintString(GetFieldText(nField, csField), pList, pDC, pDrawItemStruct, crBounds, clForeground, clBackground);

			if (IsMacroValid())
			{
				CStdMacroServer* pMacroServer = MacroServer();
				if (pMacroServer != NULL)
				{
					if (m_pMacro != NULL)
					{
						if (m_pMacro->MacroValueType() == CMacro::VALUE_TYPE_Field)
						{
							if ((m_pMacro->MacroType() == CMacro::MACRO_TYPE_Receiver) && (pMacroServer->ReceiverIsBound()))
							{
								PaintIndicator("R", pList, pDC, pDrawItemStruct, crBounds, GetSysColor(COLOR_WINDOWTEXT), GetSysColor(COLOR_WINDOW));
							}
							if ((m_pMacro->MacroType() == CMacro::MACRO_TYPE_Sender) && (pMacroServer->SenderIsBound()))
							{
								PaintIndicator("S", pList, pDC, pDrawItemStruct, crBounds, GetSysColor(COLOR_WINDOWTEXT), GetSysColor(COLOR_WINDOW));
							}
						}
					}
				}
			}
			
			break;
		}
	}
}

void CSmartFieldListBoxItem::PaintString(LPCSTR pString, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	INHERITED::PaintString(pString, pList, pDC, pDrawItemStruct, crBounds, clForeground, clBackground);
}

void CSmartFieldListBoxItem::PaintIndicator(LPCSTR pString, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	int nSave = 0;
	CBrush cbBackground;
	
	TRY
	{
		if (pString != NULL)
		{
			nSave = pDC->SaveDC();
			
			if (nSave > 0)
			{
				pDC->SetTextColor(clForeground);
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextAlign(TA_CENTER|TA_TOP);
				
				CSize czExtent = pDC->GetTextExtent(" ", 1);
		
				CRect crIndicator(crBounds);
				crIndicator.left = crIndicator.right-(czExtent.cx*4);
				
				if (cbBackground.CreateSolidBrush(clBackground))
				{
					pDC->FillRect(crIndicator, &cbBackground);
					pDC->TextOut(crIndicator.left+czExtent.cx*2, crIndicator.top+4, pString, strlen(pString));
				}
			}
		}
	}
	END_TRY
	
	if (nSave > 0)
	{
		pDC->RestoreDC(nSave);
		nSave = 0;
	}
}

void CSmartFieldListBoxItem::PaintFocus(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect* pRect /*=NULL*/)
{
	INHERITED::PaintFocus(pList, pDC, pDrawItemStruct, pRect);
}

BOOL CSmartFieldListBoxItem::IsSelected(int nField, COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return INHERITED::IsSelected(nField, pList, pDrawItemStruct);
}

BOOL CSmartFieldListBoxItem::IsFocused(int nField, COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	BOOL fFocused = INHERITED::IsFocused(nField, pList, pDrawItemStruct);
	
	if (!fFocused)
	{
		fFocused = IsSelected(nField, pList, pDrawItemStruct) && (pList->GetFocus() == (CWnd*)pList);
	}
	
	return fFocused;
}

void CSmartFieldListBoxItem::SaveState(void)
{
	// Save the state of the item.
	if (IsMacroValid())
	{
		CMacro* pMacro = Macro();
		m_nSaveType = pMacro->MacroValueType();
		m_csSaveValue = pMacro->Value();
	}
}

void CSmartFieldListBoxItem::RestoreState(void)
{
	// Save the state of the item.
	if (IsMacroValid())
	{
		CMacro* pMacro = Macro();
		pMacro->MacroValueType(m_nSaveType);
		pMacro->Value(m_csSaveValue);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSmartFieldList

CSmartFieldListBox::CSmartFieldListBox()
{
	m_fEditDialog = FALSE;
	m_fSavedState = FALSE;
}

CSmartFieldListBox::~CSmartFieldListBox()
{
}

BEGIN_MESSAGE_MAP(CSmartFieldListBox, CPropertyListBox)
	//{{AFX_MSG_MAP(CSmartFieldListBox)
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSmartFieldListBox::Fields(CStdMacroServer* pMacroServer, CMacroList* pMacros /*=NULL*/, CWordArray* pInUse /*=NULL*/)
{
	m_pMacroServer = pMacroServer;
	m_pMacros = (pMacros == NULL) ? &pMacroServer->MacroList() : pMacros;
	m_pInUse = pInUse;

	Rebuild();
}

/*
// WARNING: This function discards the original state information
// of the macro. So, if this information is important to you, then
// you had better save it somewhere else or not call this function.
*/

void CSmartFieldListBox::Rebuild(void)
{
	int nMacros = m_pMacros->Macros();

	SetRedraw(FALSE);

	ResetContent();

/* Set the height of the items. */

	MEASUREITEMSTRUCT mi;

	MeasureItem(&mi);
	SetItemHeight(0, mi.itemHeight);

/* Add all the items. */

	for (int nMacro = 0; nMacro < nMacros; nMacro++)
	{
		CMacro* pMacro = m_pMacros->Macro(nMacro);

		ASSERT(pMacro != NULL);

		if (pMacro != NULL && ShowMacro(pMacro))
		{
			CSmartFieldListBoxItem* pItem = new CSmartFieldListBoxItem(pMacro, nMacro, m_pMacroServer);

			ASSERT(pItem != NULL);

			if (pItem != NULL)
			{
				AddString((LPCSTR)pItem);
			}
		}
	}
	SetRedraw(TRUE);

	// No state is saved yet.
	m_fSavedState = FALSE;
}

BOOL CSmartFieldListBox::ShowMacro(CMacro* pMacro)
{
	if (m_pInUse != NULL)
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

WNDPROC* CSmartFieldListBox::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

BOOL CSmartFieldListBox::BeginEdit(int nCurSel /*=-1*/)
{
/* Fixup nCurSel. */

	if (nCurSel == -1)
	{
		nCurSel = GetCurSel();
	}

/* Begin editing if we can. */

	if (nCurSel >= 0)
	{
	/* Ask our parent if we can begin editing. */

		if (((CSmartFieldsDialog*)GetParent())->CanBeginEdit(nCurSel, Field()))
		{
			return INHERITED::BeginEdit(nCurSel);
		}
	}
	return FALSE;
}

BOOL CSmartFieldListBox::FinishEdit(BOOL fValidate /*=TRUE*/, BOOL fByReturn /*=FALSE*/)
{
	if (!EditDialog())
	{
		return INHERITED::FinishEdit(fValidate, fByReturn);
	}
	return FALSE;
}

BOOL CSmartFieldListBox::OnFinishEdit(BOOL fValidate /*=TRUE*/, BOOL fByReturn /*=FALSE*/)
{
	int nCurSel = GetCurSel();
	int nCount = GetCount();

	CSmartFieldsDialog* pParent = (CSmartFieldsDialog*)GetParent();

	BOOL fModified = m_pEditor->GetEditor()->GetModify();
	
	if (INHERITED::OnFinishEdit(fValidate, fByReturn))
	{
		if (fModified)
		{
			/*
			// Field is valid.
			// Update the macro in the project.
			*/
	
			if (nCurSel == GetCurSel() && nCount == GetCount())
			{
			/* Get the macro index so we will know which one to update. */
	
				CSmartFieldListBoxItem* pItem = (CSmartFieldListBoxItem*)GetItemData(nCurSel);
				if (pItem != NULL)
				{
					pParent->UpdateFields(TRUE, pItem->Index());
				}
			}
		}
		
		if (fByReturn)
		{
			TRACE(">>> Advance to next field! <<<\n");
			
			int nCurSel = GetCurSel();
			int nCount = GetCount();

			if (nCurSel != nCount-1)
			{
				BeginEdit(nCurSel+1);
			}
			else
			{
				CWnd* pCloseButton = pParent->GetDlgItem(IDC_CLOSE);
				if (pCloseButton != NULL)
				{
					pCloseButton->SetFocus();
				}
			}
		}
		
		return TRUE;
	}
	
	return FALSE;
}

void CSmartFieldListBox::OnAbortEdit(void)
{
	INHERITED::OnAbortEdit();
	BeginEdit();
}

CPropertyEditor* CSmartFieldListBox::CreateEditor(int nField, const CRect& crPosition)
{
	CPropertyEditor* pEditor = INHERITED::CreateEditor(nField, crPosition);
	
	if (pEditor != NULL)
	{
		pEditor->GetEditor()->LimitText(256);
	}
	
	return pEditor;
}

void CSmartFieldListBox::DestroyEditor(CPropertyEditor*& pEditor)
{
	INHERITED::DestroyEditor(pEditor);
}

BOOL CSmartFieldListBox::CanEditItem(int nItem, int nField)
{
	return (nField == PROPERTY_ValueField);
}

// These functions are used to save and restore the state of the fill in fields
// for the cancel button.

void CSmartFieldListBox::SaveState(void)
{
	// Save the state of each item in the list.
	
	int nItem = 0;
	int nItems = GetCount();
		
	while (nItem < nItems)
	{
		CSmartFieldListBoxItem* pItem = (CSmartFieldListBoxItem*)GetItemData(nItem);
		if (pItem != NULL)
		{
			pItem->SaveState();
		}
			
		nItem++;
	}
	m_fSavedState = TRUE;
}

void CSmartFieldListBox::RestoreState(void)
{
	if (m_fSavedState)
	{
		// Restore the state of each item in the list.
		
		int nItem = 0;
		int nItems = GetCount();
			
		while (nItem < nItems)
		{
			CSmartFieldListBoxItem* pItem = (CSmartFieldListBoxItem*)GetItemData(nItem);
			if (pItem != NULL)
			{
				pItem->RestoreState();
			}
				
			nItem++;
		}
	}
}

UINT CSmartFieldListBox::OnGetDlgCode()
{
	// Tabs do not move between fields. Rather, they move between controls.
	return CPropertyListBox::OnGetDlgCode() & ~DLGC_WANTTAB;
}

/////////////////////////////////////////////////////////////////////////////
// CSmartFieldsDialog dialog

CSmartFieldsDialog::CSmartFieldsDialog(CPmwView* pView, CWnd* pParent /*=NULL*/, BOOL fStartup /*=FALSE*/)
	: CDialog(IDD, pParent == NULL ? pView : pParent)
{
	m_uSoundID = IDD;
	m_fShowValues = FALSE;
	m_pReflowHook = NULL;
	m_fStartup = fStartup;
	m_fOriginalModified = -1;

	//{{AFX_DATA_INIT(CSmartFieldsDialog)
	//}}AFX_DATA_INIT

	m_pList = NULL;

	ResetDialog();

	m_pView = pView;
}

CSmartFieldsDialog::~CSmartFieldsDialog()
{
	delete m_pList;
}

void CSmartFieldsDialog::OnDestroy()
{
	standard_dialog_shutdown(this, m_uSoundID);
	CDialog::OnDestroy();
}

void CSmartFieldsDialog::ResetDialog(void)
{
	m_pView = NULL;
	m_pMacroServer = NULL;
	m_pMacros = NULL;
	m_fChangedMacro = FALSE;

	if (m_pList != NULL)
	{
		if (m_pList->GetSafeHwnd() != NULL)
		{
			m_pList->ResetContent();
		}
	}
	m_cwaMacrosInUse.RemoveAll();
}

/*
// Set us to be working on a particular view.
*/

void CSmartFieldsDialog::SetView(CPmwView* pView, BOOL fDoItAnyway /*=FALSE*/)
{
	if (m_pView != pView || fDoItAnyway)
	{
	/* Reset everything. */

		ResetDialog();

	/* Set the new view. */

		if ((m_pView = pView) != NULL)
		{
			m_pDoc = pView->GetDocument();
			m_pDoc->GetMacrosInUse(m_cwaMacrosInUse);

			m_pMacroServer = m_pDoc->GetMacroServer();
			ASSERT(m_pMacroServer != NULL);

			m_fShowValues = m_pMacroServer->ShowMacroValues();

		/*
		// Make a local copy of the macros.
		*/

			m_pMacros = &m_pMacroServer->MacroList();

		/*
		// Bind the fields to the list.
		// This rebuilds the list as well.
		*/
			m_pList->Fields(m_pMacroServer,
								 m_pMacros,
								 &m_cwaMacrosInUse);
		}

		UpdateButtons();
	}
}

CPmwView* CSmartFieldsDialog::GetView(void)
{
	return m_pView;
}

void CSmartFieldsDialog::OnIdleUpdate(void)
{
	UpdateButtons();
}

void CSmartFieldsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSmartFieldsDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSmartFieldsDialog, CDialog)
	//{{AFX_MSG_MAP(CSmartFieldsDialog)
	ON_LBN_SELCHANGE(IDC_MACRO_LIST, OnSelchangeMacroList)
	ON_WM_PARENTNOTIFY()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CLOSE, OnClickedClose)
	ON_WM_GETDLGCODE()
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RECEIVERS, OnReceivers)
	ON_BN_CLICKED(IDC_SENDER, OnSender)
	ON_LBN_SETFOCUS(IDC_MACRO_LIST, OnSetfocusMacroList)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmartFieldsDialog message handlers

BOOL CSmartFieldsDialog::OnInitDialog()
{
	InitList();

/*
// Do the normal init dialog.
*/

	CDialog::OnInitDialog();

/*
// Bind this dialog to a view so we can see the document and the macros.
// Force it to use m_pView.
*/

	SetView(m_pView, TRUE);

/*
// See if there are any macros in use.
// If not, then we're not needed; so, we bail here.
*/

	if (m_cwaMacrosInUse.GetSize() == 0)
	{
		if (!m_fStartup)
		{
			CNoFillInFieldsDialog Dialog;
			Dialog.DoModal();
		}
		m_uSoundID = (UINT)-1;
		EndDialog(IDCANCEL);
		return FALSE;
	}
	
	standard_dialog_startup(this, m_uSoundID);
	UpdateButtons();
	
	// Save the state of the list box in case the user chooses to cancel.
	
	SaveState();
	
	if (m_pList->GetCount() > 0)
	{
		m_pList->SetCurSel(0);
	}

	m_pList->SetFocus();
	m_pList->BeginEdit();
	
	return FALSE;
}

void CSmartFieldsDialog::InitList()
{
	if (m_pList == NULL)
	{
		m_pList = CreateListBox();
	}
	m_pList->SubclassDlgItem(IDC_MACRO_LIST, this);

/* Set the splitter offset. Line it up with the value text. */

	CWnd* pValueText = GetDlgItem(IDC_VALUE);
	ASSERT(pValueText != NULL);

	CRect crValueText;
	pValueText->GetWindowRect(crValueText);
	CRect crList;
	m_pList->GetWindowRect(crList);
	m_pList->SplitterOffset(crValueText.left - crList.left);
}

void CSmartFieldsDialog::OnSelchangeMacroList()
{
	UpdateButtons();
	m_pList->BeginEdit();
}

LRESULT CSmartFieldsDialog::OnHelpHitTest(WPARAM, LPARAM lParam)
{
	return standard_dialog_hit_test(this, m_uSoundID, lParam);
}

void CSmartFieldsDialog::EnableDlgItem(int nId, BOOL fEnable)
{
	CWnd* pItem;

	if ((pItem = GetDlgItem(nId)) != NULL)
	{
		pItem->EnableWindow(fEnable);
	}
}

void CSmartFieldsDialog::UpdateButtons(void)
{
	BOOL fSelected = FALSE;
	BOOL fAssigned = FALSE;

	if (m_pView != NULL)
	{
		fAssigned = TRUE;

		int nCurSel = m_pList->GetCurSel();

		if (nCurSel >= 0)
		{
			fSelected = TRUE;
		}
	}
	
	// Update some buttons!
}

BOOL CSmartFieldsDialog::CanBeginEdit(int nItem, int nField)
{
	return (nField == PROPERTY_ValueField);
}

void CSmartFieldsDialog::UpdateFields(BOOL fReflow, int nMacro /*=-1*/)
{
	CWordArray cwaMacros;

/* If "values" changed, reflow all... */

	if (m_pMacroServer->ShowMacroValues() != m_fShowValues)
	{
		ASSERT(nMacro == -1);
		m_pMacroServer->ShowMacroValues(m_fShowValues);
		fReflow = TRUE;
	}

/* If a macro changed, update it... */

	if (nMacro != -1)
	{
		CMacro* pMacro = m_pMacros->Macro(nMacro);
		ASSERT(pMacro != NULL);

		cwaMacros.Add(pMacro->Id() + MACRO_CHARACTER);
	}

/* Reflow if needed. */

	m_pMacroServer->ModifiedMacros();
	m_pDoc->SetModifiedFlag();

	if (fReflow)
	{
		m_pDoc->ReflowMacros(nMacro == -1 ? NULL : &cwaMacros);
		if (m_pReflowHook != NULL)
		{
			m_pReflowHook(m_pReflowData);
		}
	}
}

CSmartFieldListBox* CSmartFieldsDialog::CreateListBox(void)
{
	return new CSmartFieldListBox;
}

/*
// Set the reflow hook.
*/

void CSmartFieldsDialog::SetReflowHook(REFLOW_HOOK pHook /*=NULL*/, LPVOID pHookData /*=NULL*/)
{
	m_pReflowHook = pHook;
	m_pReflowData = pHookData;
}

void CSmartFieldsDialog::OnParentNotify(UINT message, LPARAM lParam)
{
//	od("Before OnParentNotify(%u, %ld)...\r\n", message, lParam);
	CDialog::OnParentNotify(message, lParam);
//	od("...After OnParentNotify\r\n");
}

void CSmartFieldsDialog::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	od("Dlg: Before OnChar(%x)\r\n", nChar);
	CDialog::OnChar(nChar, nRepCnt, nFlags);
	od("Dlg: After OnChar(%x)\r\n", nChar);
}

void CSmartFieldsDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	od("Dlg: Before OnKeyDown(%x)\r\n", nChar);
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
	od("Dlg: After OnKeyDown(%x)\r\n", nChar);
}

void CSmartFieldsDialog::OnClose()
{
	if (m_pList->FinishEdit())
	{
		EndDialog(IDOK);
	}
}

void CSmartFieldsDialog::OnClickedClose()
{
	OnClose();
}

UINT CSmartFieldsDialog::OnGetDlgCode()
{
	UINT uRet = CDialog::OnGetDlgCode();

	od("Dialog: OnGetDlgCode (%x)\r\n", uRet);

	return uRet;
}

void CSmartFieldsDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	standard_dialog_activate(m_uSoundID, nState, pWndOther, bMinimized);
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}

void CSmartFieldsDialog::OnReceivers()
{
/*
// Finish up any current edit.
*/
	if (m_pList->Editing())
	{
		m_pList->FinishEdit();
		GetDlgItem(IDC_RECEIVERS)->SetFocus();
	}
	
/*
// Make sure the configuration supports address books.
*/

	if (!GetConfiguration()->SupportsAddressBook())
	{
		return;
	}

	if (m_pDoc->EditNameList(TRUE))
	{
		/*
		// Make a local copy of the macros.
		*/

		m_pMacros = &m_pMacroServer->MacroList();

		/*
		// Bind the fields to the list.
		// This rebuilds the list as well.
		*/
		m_pList->Fields(m_pMacroServer, m_pMacros, &m_cwaMacrosInUse);

		// Reset all the receiver fields back to their database values.
		
		int nItem = 0;
		int nItems = m_pList->GetCount();
		
		while (nItem < nItems)
		{
			CSmartFieldListBoxItem* pItem = (CSmartFieldListBoxItem*)m_pList->GetItemData(nItem);
			if (pItem != NULL)
			{
				CMacro* pMacro = pItem->Macro();
				
				if (pMacro != NULL)
				{
					if (pMacro->MacroType() == CMacro::MACRO_TYPE_Receiver)
					{
						// We have a receiver field, reset it back to its database value.
						pMacro->MacroValueType(CMacro::VALUE_TYPE_Field);
						pMacro->Value("");
					}
				}
			}
			
			nItem++;
		}
		
		UpdateFields(TRUE);
		m_pList->Invalidate();
	}
}

void CSmartFieldsDialog::OnSender()
{
/*
// Finish up any current edit.
*/

	if (m_pList->Editing())
	{
		m_pList->FinishEdit();
		GetDlgItem(IDC_SENDER)->SetFocus();
	}
	
/*
// Make sure the configuration supports address books.
*/

	if (!GetConfiguration()->SupportsAddressBook())
	{
		return;
	}

	// Allow the user to edit the sender record for the current address book.
   
   if (m_pDoc->EditSender())
   {
		// Reset all the sender fields back to their database values.
		
		int nItem = 0;
		int nItems = m_pList->GetCount();
		
		while (nItem < nItems)
		{
			CSmartFieldListBoxItem* pItem = (CSmartFieldListBoxItem*)m_pList->GetItemData(nItem);
			if (pItem != NULL)
			{
				CMacro* pMacro = pItem->Macro();
				
				if (pMacro != NULL)
				{
					if (pMacro->MacroType() == CMacro::MACRO_TYPE_Sender)
					{
						// We have a sender field, reset it back to its database value.
						pMacro->MacroValueType(CMacro::VALUE_TYPE_Field);
						pMacro->Value("");
					}
				}
			}
			
			nItem++;
		}
		
		UpdateFields(TRUE);
		m_pList->Invalidate();
   }
}

// These functions are used to save and restore the state of the fill in fields
// for the cancel button.

void CSmartFieldsDialog::SaveState(void)
{
	m_pList->SaveState();
	m_fOriginalModified = m_pDoc->IsModified();
}

void CSmartFieldsDialog::RestoreState(void)
{
	m_pList->RestoreState();
	UpdateFields(TRUE);
	if (m_fOriginalModified != -1)
	{
		m_pDoc->SetModifiedFlag(m_fOriginalModified);
	}
}

void CSmartFieldsDialog::OnCancel()
{
	RestoreState();
	CDialog::OnCancel();
}

void CSmartFieldsDialog::OnSetfocusMacroList()
{
	m_pList->BeginEdit();
}
